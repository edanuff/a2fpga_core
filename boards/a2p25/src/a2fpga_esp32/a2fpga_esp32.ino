/*
 * ESP32-S3 4-bit capture via LCD_CAM + GDMA
 * Mode: VSYNC->EOF + STOP on EOF, immediate restart
 * Packet (FPGA): 10 PCLKs total -> 8 data nibbles (LSN-first), 1 VSYNC pulse (nibble #8),
 *                1 dummy/stopper nibble. PCLK idles between packets.
 *
 * We stop on VSYNC EOF, pack the FIRST 8 bytes, then rearm & restart immediately.
 *
 * Commands:
 *   lcam | stop | status | edge 0/1 | we N
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

// ---------- Camera signal IDs ----------
#ifndef CAM_PCLK_IDX
  #ifdef CAM_PCLK_IN_IDX
    #define CAM_PCLK_IDX CAM_PCLK_IN_IDX
  #else
    #error "CAM_PCLK_IDX/CAM_PCLK_IN_IDX not found."
  #endif
#endif
#ifndef CAM_V_SYNC_IDX
  #ifdef CAM_V_SYNC_IN_IDX
    #define CAM_V_SYNC_IDX CAM_V_SYNC_IN_IDX
  #else
    #error "CAM_V_SYNC_IDX/CAM_V_SYNC_IN_IDX not found."
  #endif
#endif
#ifndef CAM_H_ENABLE_IDX
  #ifdef CAM_H_ENABLE_IN_IDX
    #define CAM_H_ENABLE_IDX CAM_H_ENABLE_IN_IDX
  #else
    #define CAM_H_ENABLE_IDX (-1) // proceed without DE gate
  #endif
#endif
#ifndef CAM_DATA_IN0_IDX
  #error "CAM_DATA_IN0..3_IDX not found (S3 camera signals missing?)."
#endif
#ifndef GPIO_MATRIX_CONST_ONE_INPUT
  #define GPIO_MATRIX_CONST_ONE_INPUT 0x38
#endif

// ---------- Pins ----------
#define PIN_CAM_PCLK   13
#define PIN_CAM_VSYNC  12
#define PIN_CAM_D0     14
#define PIN_CAM_D1     15
#define PIN_CAM_D2     16
#define PIN_CAM_D3     17
#define PIN_DE_VIRT    10

// ---------- Sizes ----------
#define BYTES_PER_WORD   8
#define CAM_BUFFER_SIZE 16
#define GDMA_CH          0

// ---------- Runtime ----------
static uint8_t  s_clk_inv = 0;
static uint32_t word_print_every = 512;
static uint32_t words_seen = 0;
static volatile uint32_t chunks_rx = 0;

// ---------- DMA descriptor portability ----------
#if __has_include("hal/dma_types.h")
  #include "hal/dma_types.h"
  typedef dma_descriptor_t DESC_T;
  #define DESC_OWNER(d)      ((d)->dw0.owner)
  #define DESC_SUC_EOF(d)    ((d)->dw0.suc_eof)
  #define DESC_SIZE(d)       ((d)->dw0.size)
  #define DESC_LENGTH(d)     ((d)->dw0.length)
  static inline void      DESC_SET_BUF(DESC_T* d, uint8_t* p) { d->buffer = (void*)p; }
  static inline uint8_t*  DESC_GET_BUF(DESC_T* d)             { return (uint8_t*)d->buffer; }
  static inline void      DESC_SET_NEXT(DESC_T* d, DESC_T* n) { d->next   = n; }
#else
  typedef struct my_dma_desc_s {
    volatile uint32_t size   :12;
    volatile uint32_t length :12;
    volatile uint32_t suc_eof:1;
    volatile uint32_t owner  :1;
    volatile uint32_t rsvd   :6;
    uint8_t*                 buffer;
    struct my_dma_desc_s*    next;
  } DESC_T;
  #define DESC_OWNER(d)      ((d)->owner)
  #define DESC_SUC_EOF(d)    ((d)->suc_eof)
  #define DESC_SIZE(d)       ((d)->size)
  #define DESC_LENGTH(d)     ((d)->length)
  static inline void      DESC_SET_BUF(DESC_T* d, uint8_t* p) { d->buffer = p; }
  static inline uint8_t*  DESC_GET_BUF(DESC_T* d)             { return d->buffer; }
  static inline void      DESC_SET_NEXT(DESC_T* d, DESC_T* n) { d->next   = n; }
#endif

// ---------- State ----------
static uint8_t* cam_rx = nullptr;
static DESC_T   s_desc __attribute__((aligned(16)));

// ---------- Helpers ----------
static inline void route_in(int gpio, int sig_idx, bool inv=false) {
  esp_rom_gpio_connect_in_signal(gpio, sig_idx, inv);
}
static inline void route_const_one_to(int sig_idx) {
  if (sig_idx < 0) return;
  esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, sig_idx, false);
}
static inline bool dma_eof() { return GDMA.channel[GDMA_CH].in.int_st.in_suc_eof; }
static inline void dma_ack_eof() { GDMA.channel[GDMA_CH].in.int_clr.in_suc_eof = 1; }

static void desc_init_one(DESC_T* d, uint8_t* buf, size_t len, DESC_T* next) {
  memset(d, 0, sizeof(*d));
  DESC_SET_BUF(d, buf);
  DESC_SET_NEXT(d, next);
  DESC_SIZE(d)    = len;
  DESC_LENGTH(d)  = len;
  DESC_OWNER(d)   = 1;
  DESC_SUC_EOF(d) = 0;
}

static void print_status_line() {
  uint8_t d =
    ((uint8_t)digitalRead(PIN_CAM_D3) << 3) |
    ((uint8_t)digitalRead(PIN_CAM_D2) << 2) |
    ((uint8_t)digitalRead(PIN_CAM_D1) << 1) |
    ((uint8_t)digitalRead(PIN_CAM_D0) << 0);
  Serial.printf("PCLK:%d VSYNC:%d D[3:0]=0x%X  edge=%u  chunks=%lu words=%lu\n",
    digitalRead(PIN_CAM_PCLK), digitalRead(PIN_CAM_VSYNC), d, s_clk_inv,
    (unsigned long)chunks_rx, (unsigned long)words_seen);
  Serial.printf("GDMA: IN_ST=0x%08X INLINK=0x%08X\n",
    GDMA.channel[GDMA_CH].in.int_st.val, GDMA.channel[GDMA_CH].in.link.addr);
}

// Pack 32-bit word from first 8 bytes (LSN-first, low nibble only)
static inline uint32_t pack_word_lsn_first(const uint8_t *b) {
  uint32_t w = 0;
  #pragma unroll
  for (int i = 0; i < BYTES_PER_WORD; ++i) {
    w |= (uint32_t)(b[i] & 0x0F) << (4*i);
  }
  return w;
}

// ---------- LCD_CAM + GDMA bring-up ----------
static esp_err_t setup_lcd_cam_once() {
  if (!cam_rx) {
    cam_rx = (uint8_t*)heap_caps_aligned_alloc(16, CAM_BUFFER_SIZE,
                                               MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!cam_rx) { Serial.println("DMA buffer alloc failed"); return ESP_ERR_NO_MEM; }
    memset(cam_rx, 0xBB, CAM_BUFFER_SIZE);
  }

  periph_module_enable(PERIPH_LCD_CAM_MODULE);
  periph_module_reset(PERIPH_LCD_CAM_MODULE);
  periph_module_enable(PERIPH_GDMA_MODULE);
  periph_module_reset(PERIPH_GDMA_MODULE);

  // CAM reset / AFIFO reset
  LCD_CAM.cam_ctrl1.cam_reset       = 1;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
  delayMicroseconds(3);
  LCD_CAM.cam_ctrl1.cam_reset       = 0;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;

  // Internal CAM state machine clock (not PCLK)
  LCD_CAM.cam_ctrl.cam_clk_sel        = 2;  // 160MHz/4 = 40MHz
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
  route_in(PIN_CAM_VSYNC, CAM_V_SYNC_IDX, false);    // EOF source
  route_in(PIN_CAM_D0,    CAM_DATA_IN0_IDX, false);
  route_in(PIN_CAM_D1,    CAM_DATA_IN1_IDX, false);
  route_in(PIN_CAM_D2,    CAM_DATA_IN2_IDX, false);
  route_in(PIN_CAM_D3,    CAM_DATA_IN3_IDX, false);
  if (CAM_H_ENABLE_IDX >= 0) {
    route_const_one_to(CAM_H_ENABLE_IDX);           // keep window open
  } else {
    Serial.println("Note: CAM_H_ENABLE not exposed by this core; proceeding without DE gate.");
  }

  // GDMA RX
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 0;
  GDMA.channel[GDMA_CH].in.conf0.indscr_burst_en   = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_data_burst_en  = 1;

  desc_init_one(&s_desc, cam_rx, CAM_BUFFER_SIZE, nullptr);

  GDMA.channel[GDMA_CH].in.peri_sel.sel = 5; // LCD_CAM
  GDMA.channel[GDMA_CH].in.link.addr    = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.int_ena.val  = 0;
  GDMA.channel[GDMA_CH].in.int_ena.in_suc_eof = 1;

  // Camera control — VSYNC triggers EOF *and stops*, we restart in software
  LCD_CAM.cam_ctrl.cam_vs_eof_en        = 1;  // EOF at VSYNC
  LCD_CAM.cam_ctrl.cam_stop_en          = 1;  // STOP on EOF (fresh window per word)
  LCD_CAM.cam_ctrl.cam_byte_order       = 0;
  LCD_CAM.cam_ctrl.cam_bit_order        = 0;
  LCD_CAM.cam_ctrl.cam_update           = 1;

  // 8-bit mode; low nibble used
  LCD_CAM.cam_ctrl1.cam_2byte_en         = 0;
  LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = CAM_BUFFER_SIZE - 1;  // harmless in VSYNC mode
  LCD_CAM.cam_ctrl1.cam_clk_inv          = s_clk_inv;
  LCD_CAM.cam_ctrl1.cam_de_inv           = 0;
  LCD_CAM.cam_ctrl1.cam_vh_de_mode_en    = 0;
  LCD_CAM.cam_ctrl.cam_update            = 1;

  Serial.println("LCD_CAM + GDMA configured (VSYNC->EOF, stop-on-EOF, immediate restart)");
  return ESP_OK;
}

static esp_err_t start_lcd_cam() {
  GDMA.channel[GDMA_CH].in.int_clr.val = 0xFFFFFFFF;
  DESC_OWNER(&s_desc)   = 1;
  DESC_SUC_EOF(&s_desc) = 0;

  GDMA.channel[GDMA_CH].in.link.stop  = 1;
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.link.start = 1;

  LCD_CAM.cam_ctrl1.cam_clk_inv = s_clk_inv;
  LCD_CAM.cam_ctrl.cam_update   = 1;
  LCD_CAM.cam_ctrl1.cam_start   = 1;
  return ESP_OK;
}

static void stop_lcd_cam() {
  LCD_CAM.cam_ctrl1.cam_start = 0;
  GDMA.channel[GDMA_CH].in.link.stop = 1;
}

// ---------- Per-EOF processing ----------
static void pump_once() {
  if (!dma_eof()) return;

  dma_ack_eof();
  chunks_rx++;

  // How many bytes actually came in this EOF
  uint32_t got = DESC_LENGTH(&s_desc);
  if (got > CAM_BUFFER_SIZE) got = CAM_BUFFER_SIZE;

  // Need at least the 8 data bytes
  if (got >= BYTES_PER_WORD) {
    // In our packet format, extras (stopper + VSYNC) land at the *front*.
    // So skip exactly (got - 8) leading bytes and pack the next 8.
    uint32_t extras = (got > BYTES_PER_WORD) ? (got - BYTES_PER_WORD) : 0;
    const uint8_t* base = DESC_GET_BUF(&s_desc) + extras;

    // Safety: if anything went odd (shouldn’t), clamp so we still read 8 bytes in-bounds
    if (extras + BYTES_PER_WORD > got) {
      base = DESC_GET_BUF(&s_desc);  // fall back to start
    }

    uint32_t w = pack_word_lsn_first(base);

    if ((++words_seen % word_print_every) == 0) {
      Serial.printf("word[%lu]=0x%08X (got=%u, skip=%u)\n",
        (unsigned long)words_seen, w, got, extras);
    }
  }

  // Re-arm & restart (since cam_stop_en=1)
  DESC_OWNER(&s_desc)   = 1;
  DESC_SUC_EOF(&s_desc) = 0;
  GDMA.channel[GDMA_CH].in.link.start = 1;
  LCD_CAM.cam_ctrl1.cam_start = 1;
}

// ---------- Commands ----------
static void cmd_process(String cmd) {
  cmd.trim(); cmd.toLowerCase();
  if (cmd == "lcam") {
    Serial.println("Init + start LCD_CAM (VSYNC EOF, stop-on-EOF)...");
    if (setup_lcd_cam_once() != ESP_OK) { Serial.println("Setup failed"); return; }
    if (start_lcd_cam()      != ESP_OK) { Serial.println("Start failed"); return; }

    uint32_t start = millis(), w0 = words_seen;
    while (millis() - start < 5000) { pump_once(); delay(1); }
    uint32_t gotw = words_seen - w0;
    Serial.printf("VSYNC-EOF test: %lu words in 5s\n", (unsigned long)gotw);
    if (!gotw) {
      Serial.println("No words. Check that VSYNC pulses on nibble #8 and PCLK idles between packets.");
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
  } else if (cmd.startsWith("we ")) {
    long n = cmd.substring(3).toInt();
    if (n < 1) n = 1;
    word_print_every = (uint32_t)n;
    Serial.printf("word_print_every=%lu\n", (unsigned long)word_print_every);
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | edge 0/1 | we N");
  } else {
    Serial.printf("Unknown: %s\n", cmd.c_str());
  }
}

// ---------- Arduino sketch ----------
void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println("ESP32-S3 LCD_CAM 4-bit capture (VSYNC->EOF, stop-on-EOF, immediate restart)");
  Serial.println("Commands: lcam | stop | status | edge 0/1 | we N");

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