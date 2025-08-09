/*
 * ESP32-S3 4-bit parallel capture via LCD_CAM + GDMA
 * Mode: Continuous capture, EOF on VSYNC (Option B)
 * - VSYNC generates GDMA EOF, but capture does NOT stop
 * - We take the first 8 bytes of each completed chunk as one 32-bit word (LSN-first)
 * - DE is forced HIGH via GPIO matrix constant (gate always open)
 * - Only bitfields are touched (no .val = 0 nukes)
 *
 * Commands:
 *   lcam        -> init + start capture
 *   stop        -> stop capture
 *   status      -> print GPIO + GDMA status
 *   edge 0/1    -> cam_clk_inv (0=rising, 1=falling)
 *   deinv 0/1   -> invert DE (0=active-high, 1=active-low)
 *   we N        -> print every N words
 *   autodead    -> auto-detect bit perm+invert vs LSN-first DEADBEEF
 */

#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "soc/gpio_sig_map.h"
#include "soc/lcd_cam_struct.h"
#include "soc/lcd_cam_reg.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"
#include "esp_rom_gpio.h"

#if __has_include("hal/dma_types.h")
  #include "hal/dma_types.h"
#else
  #include "rom/lldesc.h"
  typedef struct {
    volatile uint32_t size  :12;
    volatile uint32_t length:12;
    volatile uint32_t suc_eof:1;
    volatile uint32_t owner :1;
    volatile uint32_t rsvd  :6;
    uint8_t* buffer;
    void* next;
  } dma_descriptor_t; // simple shim for old ROM lldesc
#endif

// ---------- Pins (adjust to your wiring) ----------
#define PIN_CAM_PCLK   13
#define PIN_CAM_VSYNC  12   // VSYNC pulses at 1st nibble of each 32-bit word
#define PIN_CAM_D0     14
#define PIN_CAM_D1     15
#define PIN_CAM_D2     16
#define PIN_CAM_D3     17

// If CONST_ONE not available, we’ll tie a spare pin HIGH and feed that.
#define PIN_DE_VIRT    10

// ---------- Capture sizing ----------
#define CAM_BUFFER_SIZE   256   // large enough; VSYNC EOFs earlier
#define BYTES_PER_WORD      8   // 8 nibbles -> 8 bytes (we use low nibble of each)
#define PRINT_BYTES         16  // for debug prints if you need them (unused here)
#define GDMA_CH              0

// ---------- Runtime toggles ----------
static uint8_t s_clk_inv   = 0;   // 0=rising, 1=falling
static uint8_t s_de_invert = 0;   // 0=active-high DE, 1=active-low DE

// ---------- Word print throttling ----------
static uint32_t word_print_every = 256;
static uint32_t words_seen = 0;

// ---------- DMA state ----------
static uint8_t *cam_rx = nullptr;
static dma_descriptor_t s_desc __attribute__((aligned(16)));
static volatile uint32_t chunks_rx = 0;

// ---------- CAM signal macro compatibility ----------
#ifndef CAM_PCLK_IDX
#  ifdef CAM_PCLK_IN_IDX
#    define CAM_PCLK_IDX CAM_PCLK_IN_IDX
#  else
#    error "CAM_PCLK_IDX/CAM_PCLK_IN_IDX not found for this IDF."
#  endif
#endif

#ifndef CAM_V_SYNC_IDX
#  ifdef CAM_V_SYNC_IN_IDX
#    define CAM_V_SYNC_IDX CAM_V_SYNC_IN_IDX
#  else
#    error "CAM_V_SYNC_IDX/CAM_V_SYNC_IN_IDX not found for this IDF."
#  endif
#endif

#ifndef CAM_H_ENABLE_IDX
#  ifdef CAM_H_ENABLE_IN_IDX
#    define CAM_H_ENABLE_IDX CAM_H_ENABLE_IN_IDX
#  else
#    error "CAM_H_ENABLE_IDX/CAM_H_ENABLE_IN_IDX not found for this IDF."
#  endif
#endif

#ifndef CAM_DATA_IN0_IDX
#  error "CAM_DATA_IN0_IDX..CAM_DATA_IN3_IDX not found (S3 camera signals missing?)."
#endif

#ifndef GPIO_MATRIX_CONST_ONE_INPUT
  #define GPIO_MATRIX_CONST_ONE_INPUT  0x38
#endif

// ---------- Bit mapping / autodetect (permute+invert) ----------
static uint8_t g_perm[4] = {0,1,2,3};   // logical bit -> physical bit index
static uint8_t g_inv_mask = 0x0;        // bit i set => invert that logical bit

static inline uint8_t map_nibble(uint8_t n) {
  uint8_t out = 0;
  for (int b = 0; b < 4; ++b) {
    uint8_t src = (n >> g_perm[b]) & 1;
    if ((g_inv_mask >> b) & 1) src ^= 1;
    out |= (src << b);
  }
  return out & 0x0F;
}

// Keep a small ring of raw low-nibbles for autodetect
#define NIB_RING 512
static uint8_t nib_ring[NIB_RING];
static uint32_t nib_w = 0;

static void fill_nib_ring_from_bytes(const uint8_t* buf, int n) {
  for (int i = 0; i < n; ++i) {
    nib_ring[nib_w++ % NIB_RING] = buf[i] & 0x0F;
  }
}

static void cmd_autodead() {
  // LSN-first DEADBEEF => F,E,E,B,D,A,E,D
  const uint8_t pat[8] = { 0xF,0xE,0xE,0xB,0xD,0xA,0xE,0xD };

  // snapshot ring
  uint8_t buf[NIB_RING];
  uint32_t start = (nib_w >= NIB_RING ? nib_w - NIB_RING : 0);
  for (uint32_t i = 0; i < NIB_RING; ++i) buf[i] = nib_ring[(start + i) % NIB_RING];

  // all permutations of [0,1,2,3]
  static const uint8_t perms[24][4] = {
    {0,1,2,3},{0,1,3,2},{0,2,1,3},{0,2,3,1},{0,3,1,2},{0,3,2,1},
    {1,0,2,3},{1,0,3,2},{1,2,0,3},{1,2,3,0},{1,3,0,2},{1,3,2,0},
    {2,0,1,3},{2,0,3,1},{2,1,0,3},{2,1,3,0},{2,3,0,1},{2,3,1,0},
    {3,0,1,2},{3,0,2,1},{3,1,0,2},{3,1,2,0},{3,2,0,1},{3,2,1,0}
  };

  int best_hits = -1; uint8_t best_perm[4] = {0}; uint8_t best_inv = 0;

  for (int pi = 0; pi < 24; ++pi) {
    for (int inv = 0; inv < 16; ++inv) {
      int hits = 0;
      for (int i = 0; i <= (int)NIB_RING - 8; ++i) {
        bool ok = true;
        for (int j = 0; j < 8; ++j) {
          uint8_t raw = buf[i+j];
          // apply mapping (perms[pi], inv)
          uint8_t out = 0;
          for (int b = 0; b < 4; ++b) {
            uint8_t src = (raw >> perms[pi][b]) & 1;
            if ((inv >> b) & 1) src ^= 1;
            out |= (src << b);
          }
          if ((out & 0xF) != pat[j]) { ok = false; break; }
        }
        if (ok) hits++;
      }
      if (hits > best_hits) {
        best_hits = hits; memcpy(best_perm, perms[pi], 4); best_inv = (uint8_t)inv;
      }
    }
  }

  Serial.printf("autodead: best_hits=%d  perm=[%u,%u,%u,%u] invmask=0x%X\n",
                best_hits, best_perm[0], best_perm[1], best_perm[2], best_perm[3], best_inv);

  if (best_hits <= 0) {
    Serial.println("Pattern not found. Ensure FPGA outputs LSN-first DEADBEEF and try again.");
    return;
  }
  memcpy(g_perm, best_perm, 4);
  g_inv_mask = best_inv;

  // quick preview
  Serial.print("Preview mapped: ");
  for (int i = 0; i < 64; ++i) {
    Serial.printf("%X", map_nibble(buf[i]));
  }
  Serial.println();
}

// ---------- Small helpers ----------
static inline void route_in(int gpio, int sig_idx, bool inv=false) {
  esp_rom_gpio_connect_in_signal(gpio, sig_idx, inv);
}
static inline void route_const_one_to(int sig_idx) {
#ifdef GPIO_MATRIX_CONST_ONE_INPUT
  esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, sig_idx, false);
#else
  pinMode(PIN_DE_VIRT, OUTPUT);
  digitalWrite(PIN_DE_VIRT, HIGH);
  esp_rom_gpio_connect_in_signal(PIN_DE_VIRT, sig_idx, false);
#endif
}
static inline bool dma_eof() { return GDMA.channel[GDMA_CH].in.int_st.in_suc_eof; }
static inline void dma_ack_eof() { GDMA.channel[GDMA_CH].in.int_clr.in_suc_eof = 1; }

static inline void prep_descriptor(dma_descriptor_t* d, uint8_t* buf, size_t len) {
  memset(d, 0, sizeof(*d));
  d->buffer     = buf;
  d->next       = NULL;
#if __has_include("hal/dma_types.h")
  d->dw0.size   = len;
  d->dw0.length = len;
  d->dw0.owner  = 1;
  d->dw0.suc_eof= 0;
#else
  d->size       = len;
  d->length     = len;
  d->owner      = 1;
  d->suc_eof    = 0;
#endif
}

static void print_status_line() {
  uint8_t d =
    ((uint8_t)digitalRead(PIN_CAM_D3) << 3) |
    ((uint8_t)digitalRead(PIN_CAM_D2) << 2) |
    ((uint8_t)digitalRead(PIN_CAM_D1) << 1) |
    ((uint8_t)digitalRead(PIN_CAM_D0) << 0);
  Serial.printf("PCLK:%d VSYNC:%d D[3:0]=0x%X  clk_inv=%u de_inv=%u  IN_SUC_EOF:%u\n",
    digitalRead(PIN_CAM_PCLK), digitalRead(PIN_CAM_VSYNC), d,
    s_clk_inv, s_de_invert,
    (unsigned)GDMA.channel[GDMA_CH].in.int_st.in_suc_eof);
}

// ---------- Pack one 32-bit LSN-first word from 8 bytes ----------
static inline uint32_t pack_word_lsn_first(const uint8_t *b) {
  uint32_t w = 0;
  for (int i = 0; i < BYTES_PER_WORD; ++i) {
    uint8_t raw = b[i] & 0x0F;
    uint8_t m   = map_nibble(raw);
    w |= (uint32_t)m << (4*i);
  }
  return w;
}

// ---------- LCD_CAM + GDMA bring-up ----------
static esp_err_t setup_lcd_cam_once() {
  // Buffer
  if (!cam_rx) {
    cam_rx = (uint8_t*)heap_caps_aligned_alloc(16, CAM_BUFFER_SIZE,
                                               MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!cam_rx) { Serial.println("DMA buffer alloc failed"); return ESP_ERR_NO_MEM; }
    memset(cam_rx, 0xBB, CAM_BUFFER_SIZE);
  }

  // Enable peripherals (IDF helpers)
  periph_module_enable(PERIPH_LCD_CAM_MODULE);
  periph_module_reset(PERIPH_LCD_CAM_MODULE);
  periph_module_enable(PERIPH_GDMA_MODULE);
  periph_module_reset(PERIPH_GDMA_MODULE);

  // Reset CAM & AFIFO (bitwise)
  LCD_CAM.cam_ctrl1.cam_reset       = 1;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
  delayMicroseconds(3);
  LCD_CAM.cam_ctrl1.cam_reset       = 0;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;

  // Internal CAM clock (state machine) — 160MHz/4 = 40MHz
  LCD_CAM.cam_ctrl.cam_clk_sel        = 2;
  LCD_CAM.cam_ctrl.cam_clkm_div_a     = 0;
  LCD_CAM.cam_ctrl.cam_clkm_div_b     = 0;
  LCD_CAM.cam_ctrl.cam_clkm_div_num   = 4;
  LCD_CAM.cam_ctrl.cam_update         = 1;

  // Directions
  gpio_set_direction((gpio_num_t)PIN_CAM_PCLK,  GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_VSYNC, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D0,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D1,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D2,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D3,    GPIO_MODE_INPUT);

  // Matrix routing
  route_in(PIN_CAM_PCLK,  CAM_PCLK_IDX,   false);
  route_in(PIN_CAM_VSYNC, CAM_V_SYNC_IDX, false);    // used for EOF
  route_in(PIN_CAM_D0,    CAM_DATA_IN0_IDX, false);
  route_in(PIN_CAM_D1,    CAM_DATA_IN1_IDX, false);
  route_in(PIN_CAM_D2,    CAM_DATA_IN2_IDX, false);
  route_in(PIN_CAM_D3,    CAM_DATA_IN3_IDX, false);

  // Keep capture window always enabled via DE
  route_const_one_to(CAM_H_ENABLE_IDX);

  // GDMA RX setup
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 0;
  GDMA.channel[GDMA_CH].in.conf0.indscr_burst_en   = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_data_burst_en  = 1;

  prep_descriptor(&s_desc, cam_rx, CAM_BUFFER_SIZE);

  GDMA.channel[GDMA_CH].in.peri_sel.sel = 5; // LCD_CAM peripheral
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.link.start = 1;
  GDMA.channel[GDMA_CH].in.int_ena.val = 0;
  GDMA.channel[GDMA_CH].in.int_ena.in_suc_eof = 1;

  // CAM control — VSYNC generates EOF; do NOT stop; no VSYNC filtering
  LCD_CAM.cam_ctrl.cam_vs_eof_en        = 1;
  LCD_CAM.cam_ctrl.cam_stop_en          = 0;
LCD_CAM.cam_ctrl1.cam_vsync_filter_en = 0;   // ✅ correct on ESP32-S3
  LCD_CAM.cam_ctrl.cam_vsync_filter_thres = 0;
  LCD_CAM.cam_ctrl.cam_byte_order       = 0;
  LCD_CAM.cam_ctrl.cam_bit_order        = 0;
  LCD_CAM.cam_ctrl.cam_update           = 1;

  // 8-bit mode; use low nibble D[3:0]; DE-only gating (we force DE=1)
  LCD_CAM.cam_ctrl1.cam_2byte_en         = 0;                    // 8-bit
  LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = CAM_BUFFER_SIZE - 1;  // allow >8; VSYNC EOFs earlier
  LCD_CAM.cam_ctrl1.cam_clk_inv          = s_clk_inv;
  LCD_CAM.cam_ctrl1.cam_de_inv           = s_de_invert;
  LCD_CAM.cam_ctrl1.cam_vh_de_mode_en    = 0;                    // DE-only; VSYNC not used for gating
  LCD_CAM.cam_ctrl.cam_update            = 1;

  Serial.println("LCD_CAM + GDMA configured (VSYNC->EOF, continuous)");
  return ESP_OK;
}

static esp_err_t start_lcd_cam() {
  // Clear pending
  GDMA.channel[GDMA_CH].in.int_clr.val = 0xFFFFFFFF;
  // Re-arm descriptor
#if __has_include("hal/dma_types.h")
  s_desc.dw0.owner  = 1; s_desc.dw0.suc_eof = 0;
#else
  s_desc.owner  = 1; s_desc.suc_eof = 0;
#endif
  GDMA.channel[GDMA_CH].in.link.stop  = 1;
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // Apply runtime fields
  LCD_CAM.cam_ctrl1.cam_clk_inv = s_clk_inv;
  LCD_CAM.cam_ctrl1.cam_de_inv  = s_de_invert;
  LCD_CAM.cam_ctrl.cam_update   = 1;

  // Go!
  LCD_CAM.cam_ctrl1.cam_start = 1;
  return ESP_OK;
}

static void stop_lcd_cam() {
  LCD_CAM.cam_ctrl1.cam_start = 0;
  GDMA.channel[GDMA_CH].in.link.stop = 1;
}

// ---------- Per-chunk processing (one word per EOF) ----------
static void pump_once() {
  if (!dma_eof()) return;
  dma_ack_eof();
  chunks_rx++;

  // How many bytes did DMA really write into this chunk?
  uint32_t got = 0;
#if __has_include("hal/dma_types.h")
  got = s_desc.dw0.length;          // many targets update this on RX EOF
#else
  got = s_desc.length;              // ROM lldesc: same idea
#endif
  if (got == 0 || got > CAM_BUFFER_SIZE) got = CAM_BUFFER_SIZE;  // safety

  // Take the LAST 8 bytes before VSYNC (previous word). If less than 8 arrived, take what we have.
  int take = (got >= BYTES_PER_WORD) ? BYTES_PER_WORD : (int)got;
  int start = (got >= BYTES_PER_WORD) ? (int)got - BYTES_PER_WORD : 0;

  const uint8_t* base = cam_rx + start;
  uint32_t w = 0;
  // pack LSN-first from whichever portion we have
  for (int i = 0; i < take; ++i) {
    uint8_t raw = base[i] & 0x0F;
    uint8_t m   = map_nibble(raw);
    w |= (uint32_t)m << (4*i);
  }

  // feed ring for autodetect (optional)
  fill_nib_ring_from_bytes(base, take);

  // Re-arm for next chunk
#if __has_include("hal/dma_types.h")
  s_desc.dw0.owner  = 1;
  s_desc.dw0.suc_eof = 0;
#else
  s_desc.owner  = 1;
  s_desc.suc_eof = 0;
#endif
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // Throttled print
  if ((++words_seen % word_print_every) == 0) {
    Serial.printf("word[%lu]=0x%08X%s (got=%u)\n",
      (unsigned long)words_seen, w, (w==0xDEADBEEF ? "  \xE2\x9C\x93" : ""), got);
  }
}

// ---------- Commands ----------
static void cmd_process(String cmd) {
  cmd.trim(); cmd.toLowerCase();
  if (cmd == "lcam") {
    Serial.println("Init + start LCD_CAM...");
    if (setup_lcd_cam_once() != ESP_OK) { Serial.println("Setup failed"); return; }
    if (start_lcd_cam()      != ESP_OK) { Serial.println("Start failed"); return; }

    // 5s smoke test
    uint32_t start = millis(), w0 = words_seen;
    while (millis() - start < 5000) { pump_once(); delay(1); }
    uint32_t got = words_seen - w0;
    Serial.printf("VSYNC-EOF test: %lu words in 5s\n", (unsigned long)got);
    if (!got) {
      Serial.println("FAIL: No words. Check PCLK and that VSYNC pulses once per word.");
      print_status_line();
    }
  } else if (cmd == "stop") {
    stop_lcd_cam();
    Serial.println("Stopped.");
  } else if (cmd == "status") {
    print_status_line();
  } else if (cmd.startsWith("edge")) {
    int v = cmd.endsWith("1") ? 1 : 0;
    s_clk_inv = (uint8_t)v;
    LCD_CAM.cam_ctrl1.cam_clk_inv = s_clk_inv;
    LCD_CAM.cam_ctrl.cam_update   = 1;
    Serial.printf("cam_clk_inv=%u\n", s_clk_inv);
  } else if (cmd.startsWith("deinv")) {
    int v = cmd.endsWith("1") ? 1 : 0;
    s_de_invert = (uint8_t)v;
    LCD_CAM.cam_ctrl1.cam_de_inv = s_de_invert;
    LCD_CAM.cam_ctrl.cam_update  = 1;
    Serial.printf("cam_de_inv=%u\n", s_de_invert);
  } else if (cmd.startsWith("we ")) {
    long n = cmd.substring(3).toInt();
    if (n < 1) n = 1;
    word_print_every = (uint32_t)n;
    Serial.printf("word_print_every=%lu\n", (unsigned long)word_print_every);
  } else if (cmd == "autodead") {
    cmd_autodead();
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | edge 0/1 | deinv 0/1 | we N | autodead");
  } else {
    Serial.printf("Unknown: %s\n", cmd.c_str());
  }
}

// ---------- Arduino sketch ----------
void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println("ESP32-S3 LCD_CAM 4-bit capture (VSYNC->EOF, continuous)");
  Serial.println("Commands: lcam | stop | status | edge 0/1 | deinv 0/1 | we N | autodead");

  pinMode(PIN_CAM_PCLK, INPUT);
  pinMode(PIN_CAM_VSYNC, INPUT);
  pinMode(PIN_CAM_D0, INPUT);
  pinMode(PIN_CAM_D1, INPUT);
  pinMode(PIN_CAM_D2, INPUT);
  pinMode(PIN_CAM_D3, INPUT);
}

void loop() {
  pump_once();
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    cmd_process(s);
  }
}