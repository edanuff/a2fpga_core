/*
 * ESP32-S3 4-bit parallel capture via LCD_CAM + GDMA
 * Mode: VSYNC -> EOF, stop_on_eof, immediate re-arm (works with gated PCLK)
 *
 * Packet (FPGA):
 *   - 10 clocks per transfer:
 *       nib 0..7 = data LSN-first, nib 8 = VSYNC (1 PCLK), nib 9 = stopper
 *
 * Processing (no heuristics):
 *   - On EOF we read descriptor length "got".
 *   - extras = got - 8; pack 8 data bytes from (base + extras).
 *   - This cuts off exactly the VSYNC+stopper that the DMA wrote *ahead* of the data.
 *
 * ISR strategy:
 *   - VSYNC GPIO ISR only sets a wake flag and bumps a counter.
 *   - Foreground sees the flag, spins up to VSYNC_SPIN_US for GDMA EOF to appear,
 *     then processes exactly one packet and immediately re-arms/start.
 *
 * Commands:
 *   lcam        -> init + start capture
 *   stop        -> stop capture
 *   status      -> print GPIO + GDMA status
 *   edge 0/1    -> cam_clk_inv (0=rising, 1=falling)
 *   we N        -> print every N words
 *   spin N      -> set VSYNC spin wait in microseconds (default 40)
 */

#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "esp_rom_gpio.h"

#include "soc/gpio_sig_map.h"
#include "soc/lcd_cam_struct.h"
#include "soc/lcd_cam_reg.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"

// ---------- Pins (adjust to your wiring) ----------
#define PIN_CAM_PCLK   13
#define PIN_CAM_VSYNC  12
#define PIN_CAM_D0     14
#define PIN_CAM_D1     15
#define PIN_CAM_D2     16
#define PIN_CAM_D3     17
#define PIN_DE_VIRT    10   // only used if we must force a constant '1' via a spare pin

// ---------- Constants tuned to the packet format ----------
static const int BYTES_PER_WORD   = 8;   // data bytes (8 nibbles)
static const int STOP_BYTES       = 2;   // VSYNC + stopper
static const int PACK_BYTES       = BYTES_PER_WORD + STOP_BYTES;  // 10 total
static const int CHUNK_BYTES      = 16;  // DMA chunk capacity (>= PACK_BYTES)
static const int DESC_COUNT       = 2;   // ping-pong
#define GDMA_CH                  0

// VSYNC → EOF publish delay cushion (adjustable at runtime via "spin N")
static volatile uint32_t VSYNC_SPIN_US = 40;

// ---------- CAM signal macro compatibility ----------
#ifndef CAM_PCLK_IDX
#  ifdef CAM_PCLK_IN_IDX
#    define CAM_PCLK_IDX CAM_PCLK_IN_IDX
#  else
#    error "CAM_PCLK_IDX / CAM_PCLK_IN_IDX not defined by this Arduino core."
#  endif
#endif
#ifndef CAM_V_SYNC_IDX
#  ifdef CAM_V_SYNC_IN_IDX
#    define CAM_V_SYNC_IDX CAM_V_SYNC_IN_IDX
#  else
#    error "CAM_V_SYNC_IDX / CAM_V_SYNC_IN_IDX not defined by this Arduino core."
#  endif
#endif
#ifndef CAM_H_ENABLE_IDX
#  ifdef CAM_H_ENABLE_IN_IDX
#    define CAM_H_ENABLE_IDX CAM_H_ENABLE_IN_IDX
#  endif
#endif
#ifndef CAM_DATA_IN0_IDX
#  error "CAM_DATA_IN0_IDX..CAM_DATA_IN3_IDX not defined (S3 camera signals missing?)."
#endif
#ifndef GPIO_MATRIX_CONST_ONE_INPUT
#  define GPIO_MATRIX_CONST_ONE_INPUT  0x38
#endif

// ---------- DMA descriptor compatibility ----------
#if __has_include("hal/dma_types.h")
  #include "hal/dma_types.h"
  typedef dma_descriptor_t DESC_T;
  static inline void desc_set_buf(DESC_T* d, uint8_t* buf)      { d->buffer = buf; }
  static inline uint8_t* desc_get_buf(DESC_T* d)                { return (uint8_t*)d->buffer; }
  static inline void desc_set_next(DESC_T* d, DESC_T* n)        { d->next   = n; }
  static inline void desc_prep(DESC_T* d, uint8_t* buf, size_t len, DESC_T* next) {
    memset(d, 0, sizeof(*d));
    d->buffer       = buf;
    d->dw0.size     = len;
    d->dw0.length   = len;     // updated by HW on RX EOF (common on S3)
    d->dw0.owner    = 1;
    d->dw0.suc_eof  = 0;
    d->next         = next;
  }
  static inline void desc_rearm(DESC_T* d) { d->dw0.owner = 1; d->dw0.suc_eof = 0; }
  static inline uint32_t desc_len(DESC_T* d) { return d->dw0.length; }
#else
  // ROM fallback (layout compatible for simple RX)
  typedef struct {
    volatile uint32_t size    :12;
    volatile uint32_t length  :12;
    volatile uint32_t suc_eof :1;
    volatile uint32_t owner   :1;
    volatile uint32_t rsvd    :6;
    void*    buffer;
    void*    next;
  } DESC_T;
  static inline void desc_set_buf(DESC_T* d, uint8_t* buf)      { d->buffer = buf; }
  static inline uint8_t* desc_get_buf(DESC_T* d)                { return (uint8_t*)d->buffer; }
  static inline void desc_set_next(DESC_T* d, DESC_T* n)        { d->next   = n; }
  static inline void desc_prep(DESC_T* d, uint8_t* buf, size_t len, DESC_T* next) {
    memset(d, 0, sizeof(*d));
    d->buffer   = buf;
    d->size     = len;
    d->length   = len;
    d->owner    = 1;
    d->suc_eof  = 0;
    d->next     = next;
  }
  static inline void desc_rearm(DESC_T* d) { d->owner = 1; d->suc_eof = 0; }
  static inline uint32_t desc_len(DESC_T* d) { return d->length; }
#endif

// ---------- Runtime state ----------
static uint8_t  s_clk_inv = 0;            // 0=rising, 1=falling
static uint32_t word_print_every = 512;
static uint32_t words_seen = 0;

static uint8_t* s_buf      = nullptr;     // DESC_COUNT * CHUNK_BYTES
static DESC_T*  s_desc     = nullptr;     // [DESC_COUNT], 16-byte aligned

static volatile bool     s_vsync_wake = false;
static volatile uint32_t s_vsync_isr_count = 0;

// ---------- Small helpers ----------
static inline void route_in(int gpio, int sig_idx, bool inv=false) {
  esp_rom_gpio_connect_in_signal(gpio, sig_idx, inv);
}
static inline void route_const_one_to(int sig_idx) {
#ifdef CAM_H_ENABLE_IDX
  esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, sig_idx, false);
#endif
}
static inline bool dma_eof() { return GDMA.channel[GDMA_CH].in.int_st.in_suc_eof; }
static inline void dma_ack_eof() { GDMA.channel[GDMA_CH].in.int_clr.in_suc_eof = 1; }
static inline volatile DESC_T* dma_eof_desc() { return (volatile DESC_T*)GDMA.channel[GDMA_CH].in.suc_eof_des_addr; }

static inline uint32_t pack_word_lsn_first(const uint8_t *b) {
  uint32_t w = 0;
  for (int i = 0; i < BYTES_PER_WORD; ++i) {
    uint8_t nib = b[i] & 0x0F;
    w |= ((uint32_t)nib) << (4*i);
  }
  return w;
}

static void print_status_line() {
  uint8_t d =
    ((uint8_t)digitalRead(PIN_CAM_D3) << 3) |
    ((uint8_t)digitalRead(PIN_CAM_D2) << 2) |
    ((uint8_t)digitalRead(PIN_CAM_D1) << 1) |
    ((uint8_t)digitalRead(PIN_CAM_D0) << 0);
  Serial.printf("CLK:%d VSYNC:%d D[3:0]=0x%X  edge=%u  words=%lu vsync_isr=%lu spin=%luus\n",
    digitalRead(PIN_CAM_PCLK), digitalRead(PIN_CAM_VSYNC), d, s_clk_inv,
    (unsigned long)words_seen, (unsigned long)s_vsync_isr_count, (unsigned long)VSYNC_SPIN_US);
  Serial.printf("GDMA: IN_ST=0x%08X INLINK=0x%08X EOF_DES=0x%08X\n",
    (unsigned)GDMA.channel[GDMA_CH].in.int_st.val,
    (unsigned)GDMA.channel[GDMA_CH].in.link.addr,
    (unsigned)GDMA.channel[GDMA_CH].in.suc_eof_des_addr);
}

// ---------- Bring-up ----------
static esp_err_t setup_lcd_cam_once() {
  // Allocate buffers/descs once
  if (!s_buf) {
    s_buf = (uint8_t*)heap_caps_aligned_alloc(16, DESC_COUNT * CHUNK_BYTES,
             MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!s_buf) { Serial.println("DMA buffer alloc failed"); return ESP_ERR_NO_MEM; }
    memset(s_buf, 0xBB, DESC_COUNT * CHUNK_BYTES);
  }
  if (!s_desc) {
    s_desc = (DESC_T*)heap_caps_aligned_alloc(16, DESC_COUNT * sizeof(DESC_T),
              MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!s_desc) { Serial.println("Desc alloc failed"); return ESP_ERR_NO_MEM; }
    memset(s_desc, 0, DESC_COUNT * sizeof(DESC_T));
  }

  // Enable and reset peripherals
  periph_module_enable(PERIPH_LCD_CAM_MODULE);
  periph_module_reset(PERIPH_LCD_CAM_MODULE);
  periph_module_enable(PERIPH_GDMA_MODULE);
  periph_module_reset(PERIPH_GDMA_MODULE);

  // Reset CAM core + AFIFO (bitwise)
  LCD_CAM.cam_ctrl1.cam_reset       = 1;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
  delayMicroseconds(3);
  LCD_CAM.cam_ctrl1.cam_reset       = 0;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;

  // Modest internal CAM clock (state machine). Capture is externally clocked by PCLK.
  LCD_CAM.cam_ctrl.cam_clk_sel        = 2;    // 160MHz/4 = 40MHz base
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

  // GPIO matrix routing
  route_in(PIN_CAM_PCLK,  CAM_PCLK_IDX,   false);
  route_in(PIN_CAM_VSYNC, CAM_V_SYNC_IDX, false);    // used solely for EOF
  route_in(PIN_CAM_D0,    CAM_DATA_IN0_IDX, false);
  route_in(PIN_CAM_D1,    CAM_DATA_IN1_IDX, false);
  route_in(PIN_CAM_D2,    CAM_DATA_IN2_IDX, false);
  route_in(PIN_CAM_D3,    CAM_DATA_IN3_IDX, false);

  // Keep capture window open via DE (if the core exposes H_ENABLE)
#ifdef CAM_H_ENABLE_IDX
  route_const_one_to(CAM_H_ENABLE_IDX);
#endif

  // GDMA RX setup
  GDMA.channel[GDMA_CH].in.conf0.in_rst           = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_rst           = 0;
  GDMA.channel[GDMA_CH].in.conf0.indscr_burst_en  = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_data_burst_en = 1;

  // Prepare ping-pong descriptors (we stop on EOF and re-arm in SW)
  for (int i = 0; i < DESC_COUNT; ++i) {
    uint8_t* buf = s_buf + i * CHUNK_BYTES;
    DESC_T*  nxt = (i+1 < DESC_COUNT) ? &s_desc[i+1] : nullptr;
    desc_prep(&s_desc[i], buf, CHUNK_BYTES, nxt);
  }

  // Hook GDMA to LCD_CAM
  GDMA.channel[GDMA_CH].in.peri_sel.sel = 5;      // 5 = LCD_CAM

  // Link descriptors and enable EOF interrupt status
  GDMA.channel[GDMA_CH].in.int_ena.val = 0;
  GDMA.channel[GDMA_CH].in.int_ena.in_suc_eof = 1;
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc[0];
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // LCD_CAM control — VSYNC->EOF; STOP on EOF (we will immediately restart)
  LCD_CAM.cam_ctrl.cam_vs_eof_en         = 1;
  LCD_CAM.cam_ctrl.cam_stop_en           = 1;   // stop on EOF; SW restarts
  LCD_CAM.cam_ctrl.cam_vsync_filter_thres= 0;
  LCD_CAM.cam_ctrl.cam_byte_order        = 0;
  LCD_CAM.cam_ctrl.cam_bit_order         = 0;
  LCD_CAM.cam_ctrl.cam_update            = 1;

  // 8-bit mode, window big enough; not using DE gate
  LCD_CAM.cam_ctrl1.cam_2byte_en          = 0;                     // 8-bit
  LCD_CAM.cam_ctrl1.cam_rec_data_bytelen  = CHUNK_BYTES - 1;       // EOF is VSYNC
  LCD_CAM.cam_ctrl1.cam_clk_inv           = s_clk_inv;
  LCD_CAM.cam_ctrl1.cam_de_inv            = 0;
  LCD_CAM.cam_ctrl1.cam_vh_de_mode_en     = 0;                     // ignore DE
  LCD_CAM.cam_ctrl.cam_update             = 1;

  // VSYNC GPIO ISR (rising edge)
  attachInterrupt(digitalPinToInterrupt(PIN_CAM_VSYNC), [] IRAM_ATTR {
    s_vsync_isr_count++;
    s_vsync_wake = true;
  }, RISING);

  Serial.println("LCD_CAM + GDMA configured (VSYNC->EOF, stop-on-EOF, ISR-woken)");
  return ESP_OK;
}

static inline void lcdcam_restart_after_eof() {
  // Re-arm all descriptors (owner=1) and relaunch link + cam_start
  for (int i = 0; i < DESC_COUNT; ++i) desc_rearm(&s_desc[i]);
  GDMA.channel[GDMA_CH].in.link.start = 1;
  LCD_CAM.cam_ctrl1.cam_start = 1;
}

// ---------- EOF processing ----------
static void on_eof_process() {
  // Which descriptor completed?
  volatile DESC_T* vd = dma_eof_desc();
  DESC_T* d = (DESC_T*)vd;
  uint8_t* base = desc_get_buf(d);

  // "got" may be updated by HW; clamp to CHUNK_BYTES
  uint32_t got = desc_len(d);
  if (got == 0 || got > (uint32_t)CHUNK_BYTES) got = CHUNK_BYTES;

  // Exact, non-heuristic skip: extras = got - 8
  const uint32_t extras = (got > (uint32_t)BYTES_PER_WORD) ? (got - (uint32_t)BYTES_PER_WORD) : 0;
  const uint8_t* p = base + extras;
  if (extras + BYTES_PER_WORD > got) { p = base; } // safety (shouldn't happen)

  const uint32_t w = pack_word_lsn_first(p);

  if ((++words_seen % word_print_every) == 0) {
    Serial.printf("word[%lu]=0x%08X (got=%u, skip=%u)\n",
      (unsigned long)words_seen, w, got, extras);
  }

  // Ack EOF, then immediately re-arm and restart for the next packet
  dma_ack_eof();
  lcdcam_restart_after_eof();
}

// Handle one VSYNC wake: wait briefly for EOF, then process once
static void handle_vsync_wake_once() {
  const uint32_t t0 = micros();
  while (!dma_eof()) {
    if ((micros() - t0) > VSYNC_SPIN_US) break;
  }
  if (dma_eof()) on_eof_process();
}

// ---------- Commands ----------
static void cmd_process(String cmd) {
  cmd.trim(); cmd.toLowerCase();
  if (cmd == "lcam") {
    Serial.println("Init + start LCD_CAM (VSYNC EOF, ISR-woken)...");
    if (setup_lcd_cam_once() != ESP_OK) { Serial.println("Setup failed"); return; }

    // Start fresh
    s_vsync_isr_count = 0;
    s_vsync_wake      = false;
    words_seen        = 0;

    GDMA.channel[GDMA_CH].in.int_clr.val = 0xFFFFFFFF;
    GDMA.channel[GDMA_CH].in.link.addr   = (uint32_t)&s_desc[0];
    GDMA.channel[GDMA_CH].in.link.start  = 1;
    LCD_CAM.cam_ctrl1.cam_clk_inv        = s_clk_inv;
    LCD_CAM.cam_ctrl.cam_update          = 1;
    LCD_CAM.cam_ctrl1.cam_start          = 1;

    // 5s smoke test (foreground reacts to ISR wakes)
    uint32_t start_ms = millis(), w0 = words_seen;
    while (millis() - start_ms < 5000) {
      if (s_vsync_wake) {
        s_vsync_wake = false;
        handle_vsync_wake_once();
      }
      delay(1);
    }
    uint32_t gotw = words_seen - w0;
    Serial.printf("VSYNC-EOF test: %lu words in 5s\n", (unsigned long)gotw);
    if (!gotw) {
      Serial.println("No words. Check PCLK burst (10 clocks/packet) and VSYNC on nibble 9.");
    }
  } else if (cmd == "stop") {
    LCD_CAM.cam_ctrl1.cam_start = 0;
    GDMA.channel[GDMA_CH].in.link.stop = 1;
    detachInterrupt(digitalPinToInterrupt(PIN_CAM_VSYNC));
    Serial.println("Stopped.");
  } else if (cmd == "status") {
    print_status_line();
  } else if (cmd.startsWith("edge")) {
    int v = cmd.endsWith("1") ? 1 : 0;
    s_clk_inv = (uint8_t)v;
    LCD_CAM.cam_ctrl1.cam_clk_inv = s_clk_inv;
    LCD_CAM.cam_ctrl.cam_update   = 1;
    Serial.printf("cam_clk_inv=%u\n", s_clk_inv);
  } else if (cmd.startsWith("we ")) {
    long n = cmd.substring(3).toInt();
    if (n < 1) n = 1;
    word_print_every = (uint32_t)n;
    Serial.printf("word_print_every=%lu\n", (unsigned long)word_print_every);
  } else if (cmd.startsWith("spin ")) {
    long us = cmd.substring(5).toInt();
    if (us < 0) us = 0;
    VSYNC_SPIN_US = (uint32_t)us;
    Serial.printf("VSYNC_SPIN_US=%lu\n", (unsigned long)VSYNC_SPIN_US);
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | edge 0/1 | we N | spin us");
  } else {
    Serial.printf("Unknown: %s\n", cmd.c_str());
  }
}

// ---------- Arduino sketch ----------
void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println("ESP32-S3 LCD_CAM 4-bit capture (VSYNC->EOF, stop-on-EOF, ISR-woken, pack last 8)");
  Serial.println("Commands: lcam | stop | status | edge 0/1 | we N | spin us");

  pinMode(PIN_CAM_PCLK, INPUT);
  pinMode(PIN_CAM_VSYNC, INPUT);
  pinMode(PIN_CAM_D0, INPUT);
  pinMode(PIN_CAM_D1, INPUT);
  pinMode(PIN_CAM_D2, INPUT);
  pinMode(PIN_CAM_D3, INPUT);
}

void loop() {
  // React to VSYNC IRQ wakes with minimal latency
  if (s_vsync_wake) {
    s_vsync_wake = false;
    handle_vsync_wake_once();
  }

  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    cmd_process(s);
  }
}