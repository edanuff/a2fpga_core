/*
 * ESP32-S3 4-bit parallel capture via LCD_CAM + GDMA
 * Mode: Continuous capture, EOF on VSYNC (Option B)
 * - VSYNC generates GDMA EOF, capture continues (we re-arm the same descriptor)
 * - Each completed chunk: we pack the last 8 bytes (low nibbles) into one 32-bit word (LSN-first)
 * - DE is forced HIGH via GPIO-matrix constant when available (gate always open)
 * - Only bitfields are touched (no .val = 0 nukes)
 *
 * Commands:
 *   lcam        -> init + start capture (5s smoke test)
 *   stop        -> stop capture
 *   status      -> print GPIO + GDMA status
 *   edge 0/1    -> cam_clk_inv (0=rising, 1=falling)
 *   deinv 0/1   -> invert DE (0=active-high, 1=active-low)
 *   we N        -> print every N words
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

// lldesc descriptor (Arduino-ESP32 3.3.0 provides ROM header)
#if __has_include("esp32/rom/lldesc.h")
  #include "esp32/rom/lldesc.h"
#else
  #include "rom/lldesc.h"
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
#define CAM_BUFFER_SIZE   256   // allows >8 bytes; VSYNC EOFs earlier
#define BYTES_PER_WORD      8   // 8 nibbles -> 8 bytes (we use low nibble of each)
#define PRINT_BYTES        16
#define GDMA_CH             0

// ---------- Runtime toggles ----------
static uint8_t s_clk_inv   = 0;   // 0=rising, 1=falling
static uint8_t s_de_invert = 0;   // 0=active-high DE, 1=active-low DE

// ---------- Word print throttling ----------
static uint32_t word_print_every = 256;
static uint32_t words_seen = 0;

// ---------- DMA state ----------
static uint8_t  *cam_rx = nullptr;
static lldesc_t  s_desc __attribute__((aligned(16)));
static volatile uint32_t chunks_rx = 0;

// ---------- CAM signal macro compatibility ----------
#ifndef CAM_PCLK_IDX
#  ifdef CAM_PCLK_IN_IDX
#    define CAM_PCLK_IDX CAM_PCLK_IN_IDX
#  else
#    error "CAM_PCLK_IDX/CAM_PCLK_IN_IDX not found for this core."
#  endif
#endif

#ifndef CAM_V_SYNC_IDX
#  ifdef CAM_V_SYNC_IN_IDX
#    define CAM_V_SYNC_IDX CAM_V_SYNC_IN_IDX
#  else
#    error "CAM_V_SYNC_IDX/CAM_V_SYNC_IN_IDX not found for this core."
#  endif
#endif

// DE (H_ENABLE) is optional in some core versions
#if defined(CAM_H_ENABLE_IN_IDX)
  #define CAM_H_ENABLE_IDX CAM_H_ENABLE_IN_IDX
#endif

#ifndef GPIO_MATRIX_CONST_ONE_INPUT
  #define GPIO_MATRIX_CONST_ONE_INPUT  0x38
#endif

// ---------- Small helpers ----------
static inline void route_in(int gpio, int sig_idx, bool inv=false) {
  esp_rom_gpio_connect_in_signal(gpio, sig_idx, inv);
}
static inline void route_const_one_to(int sig_idx) {
#ifdef CAM_H_ENABLE_IDX
  esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, sig_idx, false);
#else
  (void)sig_idx; // not exposed by this core; skip
#endif
}
static inline bool dma_eof() { return GDMA.channel[GDMA_CH].in.int_st.in_suc_eof; }
static inline void dma_ack_eof() { GDMA.channel[GDMA_CH].in.int_clr.in_suc_eof = 1; }

static inline void prep_descriptor(lldesc_t* d, uint8_t* buf, size_t len) {
  memset(d, 0, sizeof(*d));
  d->size   = len;
  d->length = len;
  d->owner  = 1;   // DMA owns
  d->eof    = 0;   // HW sets on EOF
  d->buf    = buf;
  d->qe.stqe_next = NULL;
}

static void print_status_line() {
  uint8_t d =
    ((uint8_t)digitalRead(PIN_CAM_D3) << 3) |
    ((uint8_t)digitalRead(PIN_CAM_D2) << 2) |
    ((uint8_t)digitalRead(PIN_CAM_D1) << 1) |
    ((uint8_t)digitalRead(PIN_CAM_D0) << 0);
  Serial.printf("PCLK:%d VSYNC:%d D[3:0]=0x%X  edge=%u de_inv=%u  chunks=%u words=%lu\n",
    digitalRead(PIN_CAM_PCLK), digitalRead(PIN_CAM_VSYNC), d,
    s_clk_inv, s_de_invert, (unsigned)chunks_rx, (unsigned long)words_seen);
}

// ---------- Pack one 32-bit LSN-first word from 8 bytes (low nibble of each) ----------
static inline uint32_t pack_word_lsn_first(const uint8_t *b) {
  uint32_t w = 0;
  for (int i = 0; i < BYTES_PER_WORD; ++i) {
    w |= (uint32_t)(b[i] & 0x0F) << (4*i);
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

  // Internal CAM clock (state machine) — 160MHz/4 = 40MHz (safe; external PCLK still drives sampling)
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
  route_in(PIN_CAM_PCLK,  CAM_PCLK_IDX,    false);
  route_in(PIN_CAM_VSYNC, CAM_V_SYNC_IDX,  false);    // used for EOF
  route_in(PIN_CAM_D0,    CAM_DATA_IN0_IDX, false);
  route_in(PIN_CAM_D1,    CAM_DATA_IN1_IDX, false);
  route_in(PIN_CAM_D2,    CAM_DATA_IN2_IDX, false);
  route_in(PIN_CAM_D3,    CAM_DATA_IN3_IDX, false);

  // Keep capture window always enabled via DE (if H_ENABLE signal is exposed by the core)
  route_const_one_to(
  #ifdef CAM_H_ENABLE_IDX
    CAM_H_ENABLE_IDX
  #else
    0
  #endif
  );

  // GDMA RX setup
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 0;
  GDMA.channel[GDMA_CH].in.conf0.indscr_burst_en   = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_data_burst_en  = 1;

  prep_descriptor(&s_desc, cam_rx, CAM_BUFFER_SIZE);

  // Link GDMA to LCD_CAM (legacy selector value 5 = LCD_CAM on ESP32-S3)
  GDMA.channel[GDMA_CH].in.peri_sel.sel = 5;
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.link.start = 1;
  GDMA.channel[GDMA_CH].in.int_ena.val = 0;
  GDMA.channel[GDMA_CH].in.int_ena.in_suc_eof = 1;

  // CAM control — VSYNC generates EOF; do NOT stop; byte/bit order normal
  LCD_CAM.cam_ctrl.cam_vs_eof_en        = 1;
  LCD_CAM.cam_ctrl.cam_stop_en          = 0;
  // (Do not touch *vsync_filter* bits; not present on all cores)
  LCD_CAM.cam_ctrl.cam_byte_order       = 0;
  LCD_CAM.cam_ctrl.cam_bit_order        = 0;
  LCD_CAM.cam_ctrl.cam_update           = 1;

  // 8-bit mode; we only use low nibble D[3:0]; DE gate kept high
  LCD_CAM.cam_ctrl1.cam_2byte_en         = 0;                    // 8-bit
  LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = CAM_BUFFER_SIZE - 1;  // VSYNC EOFs earlier
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
  s_desc.owner  = 1;
  s_desc.eof    = 0;
  s_desc.length = CAM_BUFFER_SIZE;
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

  // How many bytes did DMA write?
  uint32_t got = s_desc.length;
  if (got == 0 || got > CAM_BUFFER_SIZE) got = CAM_BUFFER_SIZE;

  // Take the LAST 8 bytes before VSYNC (previous word). If less than 8 arrived, take what we have.
  int take  = (got >= BYTES_PER_WORD) ? BYTES_PER_WORD : (int)got;
  int start = (got >= BYTES_PER_WORD) ? (int)got - BYTES_PER_WORD : 0;

  const uint8_t* base = cam_rx + start;
  uint32_t w = pack_word_lsn_first(base);

  // Re-arm for next chunk
  s_desc.owner  = 1;
  s_desc.eof    = 0;
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // Throttled print
  if ((++words_seen % word_print_every) == 0) {
    Serial.printf("word[%lu]=0x%08X (got=%u)\n",
      (unsigned long)words_seen, w, (unsigned)got);
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
    uint32_t t0 = millis(), w0 = words_seen;
    while (millis() - t0 < 5000) { pump_once(); delay(1); }
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
    // quick peek at GDMA link regs
    Serial.printf("GDMA: sel=%u IN_ST=0x%08X INLINK=0x%08X\n",
      (unsigned)GDMA.channel[GDMA_CH].in.peri_sel.sel,
      (unsigned)GDMA.channel[GDMA_CH].in.int_st.val,
      (unsigned)GDMA.channel[GDMA_CH].in.link.addr);
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
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | edge 0/1 | deinv 0/1 | we N");
  } else {
    Serial.printf("Unknown: %s\n", cmd.c_str());
  }
}

// ---------- Arduino sketch ----------
void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println("ESP32-S3 LCD_CAM 4-bit capture (VSYNC->EOF, continuous)");
  Serial.println("Commands: lcam | stop | status | edge 0/1 | deinv 0/1 | we N");

  pinMode(PIN_CAM_PCLK,  INPUT);
  pinMode(PIN_CAM_VSYNC, INPUT);
  pinMode(PIN_CAM_D0,    INPUT);
  pinMode(PIN_CAM_D1,    INPUT);
  pinMode(PIN_CAM_D2,    INPUT);
  pinMode(PIN_CAM_D3,    INPUT);
}

void loop() {
  pump_once();
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    cmd_process(s);
  }
}