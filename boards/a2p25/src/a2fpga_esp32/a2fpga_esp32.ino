/*
 * ESP32-S3 4-bit parallel capture via LCD_CAM + GDMA
 * - Continuous capture, gated only by DE (H_ENABLE) held HIGH
 * - No VSYNC/HSYNC required
 * - No full-register .val = 0 writes (don’t clobber clock fields etc.)
 * - Compatible with CAM_* signal macro variants (_IDX vs _IN_IDX)
 *
 * Commands over USB serial:
 *   lcam      -> init + start
 *   stop      -> stop capture
 *   status    -> print GPIO + GDMA/LCD_CAM status
 *   edge 0/1  -> set cam_clk_inv (0=rising, 1=falling)
 *   deinv 0/1 -> invert DE (0=active-high, 1=active-low)
 */

#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "esp_heap_caps.h"
#include "soc/gpio_sig_map.h"
#include "soc/lcd_cam_struct.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"
#include "esp_rom_gpio.h"
#include "hal/dma_types.h"

// ---------- Pins (adjust to your board) ----------
#define PIN_CAM_PCLK   13
#define PIN_CAM_VSYNC  12   // not used for gating/EOF here, but routed
#define PIN_CAM_D0     14
#define PIN_CAM_D1     15
#define PIN_CAM_D2     16
#define PIN_CAM_D3     17

// If GPIO_MATRIX_CONST_ONE_INPUT isn’t available, we’ll drive this pin HIGH and feed that.
#define PIN_DE_VIRT    10   // any spare GPIO you’re not using

// ---------- Capture sizing ----------
#define CAM_BUFFER_SIZE  256     // bytes per DMA chunk
#define PRINT_BYTES       16     // how many leading bytes to print
#define GDMA_CH           0

// Runtime-tweakable flags (you can change with commands)
static uint8_t s_clk_inv = 0;              // 0=rising edge, 1=falling
static uint8_t s_de_invert = 0;            // 0=active-high, 1=active-low

// ---------- State ----------
static uint8_t *cam_rx = nullptr;
static dma_descriptor_t s_desc __attribute__((aligned(16)));
static bool usb_was_connected = false;
static bool jtag_enabled = false;
static volatile uint32_t chunks_rx = 0;

// ---------- Signal macro compatibility ----------
// Some IDF versions name inputs *_IN_IDX, some just *_IDX.
#ifndef CAM_PCLK_IDX
#  ifdef CAM_PCLK_IN_IDX
#    define CAM_PCLK_IDX CAM_PCLK_IN_IDX
#  else
#    error "CAM_PCLK_IDX/CAM_PCLK_IN_IDX not found"
#  endif
#endif

#ifndef CAM_V_SYNC_IDX
#  ifdef CAM_V_SYNC_IN_IDX
#    define CAM_V_SYNC_IDX CAM_V_SYNC_IN_IDX
#  else
#    error "CAM_V_SYNC_IDX/CAM_V_SYNC_IN_IDX not found"
#  endif
#endif

#ifndef CAM_H_ENABLE_IDX
#  ifdef CAM_H_ENABLE_IN_IDX
#    define CAM_H_ENABLE_IDX CAM_H_ENABLE_IN_IDX
#  else
#    error "CAM_H_ENABLE_IDX/CAM_H_ENABLE_IN_IDX not found"
#  endif
#endif

// Data lines are consistently named CAM_DATA_INn_IDX across S3
#ifndef CAM_DATA_IN0_IDX
#  error "CAM_DATA_IN0_IDX..CAM_DATA_IN3_IDX not found"
#endif

// ---------- Helpers ----------
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

static inline bool dma_eof() {
  return GDMA.channel[GDMA_CH].in.int_st.in_suc_eof;
}
static inline void dma_ack_eof() {
  GDMA.channel[GDMA_CH].in.int_clr.in_suc_eof = 1;
}

static void print_status_line() {
  uint8_t d =
    ((uint8_t)digitalRead(PIN_CAM_D3) << 3) |
    ((uint8_t)digitalRead(PIN_CAM_D2) << 2) |
    ((uint8_t)digitalRead(PIN_CAM_D1) << 1) |
    ((uint8_t)digitalRead(PIN_CAM_D0) << 0);
  Serial.printf("PCLK:%d VSYNC:%d D[3:0]=0x%X clk_inv=%u de_inv=%u  IN_SUC_EOF:%u\n",
    digitalRead(PIN_CAM_PCLK), digitalRead(PIN_CAM_VSYNC), d,
    s_clk_inv, s_de_invert,
    (unsigned)GDMA.channel[GDMA_CH].in.int_st.in_suc_eof);
}

// ---------- LCD_CAM + GDMA Bring-up ----------
static void prep_descriptor(dma_descriptor_t* d, uint8_t* buf, size_t len) {
  memset(d, 0, sizeof(*d));
  d->buffer        = buf;
  d->next          = NULL;
  d->dw0.size      = len;
  d->dw0.length    = len;
  d->dw0.owner     = 1;     // DMA owns
  d->dw0.suc_eof   = 0;     // HW will set
}

static esp_err_t setup_lcd_cam_once() {
  // Buffers
  if (!cam_rx) {
    cam_rx = (uint8_t*)heap_caps_aligned_alloc(16, CAM_BUFFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!cam_rx) {
      Serial.println("DMA buffer alloc failed");
      return ESP_ERR_NO_MEM;
    }
    memset(cam_rx, 0xBB, CAM_BUFFER_SIZE);
  }

  // Enable peripherals via driver helpers (don’t poke raw reset bits)
  periph_module_enable(PERIPH_LCD_CAM_MODULE);
  periph_module_reset(PERIPH_LCD_CAM_MODULE);
  periph_module_enable(PERIPH_GDMA_MODULE);
  periph_module_reset(PERIPH_GDMA_MODULE);

  // Hard-reset CAM block & AFIFO (bitwise, don’t zero the whole register)
  LCD_CAM.cam_ctrl1.cam_reset       = 1;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
  delayMicroseconds(3);
  LCD_CAM.cam_ctrl1.cam_reset       = 0;
  LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;

  // Program CAM module clock (internal FSM clock). Keep existing fields we’re not touching.
  // Use 160MHz source / 4 = 40MHz for the internal state machine.
  LCD_CAM.cam_ctrl.cam_clk_sel        = 2; // 160MHz
  LCD_CAM.cam_ctrl.cam_clkm_div_a     = 0;
  LCD_CAM.cam_ctrl.cam_clkm_div_b     = 0;
  LCD_CAM.cam_ctrl.cam_clkm_div_num   = 4;
  LCD_CAM.cam_ctrl.cam_update         = 1;

  // GPIO directions
  gpio_set_direction((gpio_num_t)PIN_CAM_PCLK,  GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_VSYNC, GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D0,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D1,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D2,    GPIO_MODE_INPUT);
  gpio_set_direction((gpio_num_t)PIN_CAM_D3,    GPIO_MODE_INPUT);

  // Matrix routing
  route_in(PIN_CAM_PCLK,  CAM_PCLK_IDX,   false);
  route_in(PIN_CAM_VSYNC, CAM_V_SYNC_IDX, false);
  route_in(PIN_CAM_D0,    CAM_DATA_IN0_IDX, false);
  route_in(PIN_CAM_D1,    CAM_DATA_IN1_IDX, false);
  route_in(PIN_CAM_D2,    CAM_DATA_IN2_IDX, false);
  route_in(PIN_CAM_D3,    CAM_DATA_IN3_IDX, false);

  // Keep capture window always enabled (we’ll optionally invert via cam_de_inv)
  route_const_one_to(CAM_H_ENABLE_IDX);

  // GDMA RX channel basic setup
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_rst            = 0;
  GDMA.channel[GDMA_CH].in.conf0.indscr_burst_en   = 1;
  GDMA.channel[GDMA_CH].in.conf0.in_data_burst_en  = 1;

  // Link descriptor
  prep_descriptor(&s_desc, cam_rx, CAM_BUFFER_SIZE);

  // Connect GDMA to LCD_CAM (S3 uses peripheral ID 5 for LCD_CAM)
  GDMA.channel[GDMA_CH].in.peri_sel.sel = 5; // LCD_CAM

  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // Enable only the EOF interrupt; we poll it
  GDMA.channel[GDMA_CH].in.int_ena.val    = 0;
  GDMA.channel[GDMA_CH].in.int_ena.in_suc_eof = 1;

  // Core CAM settings (bitwise; don’t clobber other fields)
  LCD_CAM.cam_ctrl.cam_vs_eof_en     = 0;             // don’t rely on VSYNC for EOF
  LCD_CAM.cam_ctrl.cam_stop_en       = 0;             // don’t auto-stop on VSYNC
  LCD_CAM.cam_ctrl.cam_update        = 1;

  // 8-bit mode; we’ll read D[3:0]. Use DE gate, ignore HSYNC/VSYNC windows.
  LCD_CAM.cam_ctrl1.cam_2byte_en         = 0;         // 8-bit
  LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = CAM_BUFFER_SIZE - 1;
  LCD_CAM.cam_ctrl1.cam_clk_inv          = s_clk_inv;
  LCD_CAM.cam_ctrl1.cam_de_inv           = s_de_invert;
  LCD_CAM.cam_ctrl1.cam_vh_de_mode_en    = 0;         // DE-only gating (HSYNC/VSYNC not used)
  LCD_CAM.cam_ctrl.cam_update            = 1;

  return ESP_OK;
}

static esp_err_t start_lcd_cam() {
  // Clear pending ints
  GDMA.channel[GDMA_CH].in.int_clr.val = 0xFFFFFFFF;
  // Re-arm descriptor
  s_desc.dw0.owner  = 1;
  s_desc.dw0.suc_eof = 0;
  GDMA.channel[GDMA_CH].in.link.stop  = 1;
  GDMA.channel[GDMA_CH].in.link.addr  = (uint32_t)&s_desc;
  GDMA.channel[GDMA_CH].in.link.start = 1;

  // Apply runtime-toggled fields safely
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

// ---------- Processing ----------
static void pump_once() {
  if (!dma_eof()) return;

  dma_ack_eof();
  chunks_rx++;

  Serial.printf("Chunk %lu (first %d bytes): ", (unsigned long)chunks_rx, PRINT_BYTES);
  for (int i = 0; i < PRINT_BYTES && i < CAM_BUFFER_SIZE; i++) {
    Serial.printf("%02X ", cam_rx[i]);
  }
  Serial.println();

  Serial.print("Low nibbles: ");
  int lim = min(PRINT_BYTES, (int)CAM_BUFFER_SIZE);
  for (int i = 0; i < lim; i++) Serial.printf("%X ", cam_rx[i] & 0x0F);
  Serial.println();

  // Re-arm descriptor for continuous stream
  s_desc.dw0.owner  = 1;
  s_desc.dw0.suc_eof = 0;
  GDMA.channel[GDMA_CH].in.link.start = 1;
}

// ---------- Commands ----------
static void cmd_process(String cmd) {
  cmd.trim(); cmd.toLowerCase();
  if (cmd == "lcam") {
    Serial.println("Init + start LCD_CAM...");
    if (setup_lcd_cam_once() != ESP_OK) {
      Serial.println("Setup failed");
      return;
    }
    if (start_lcd_cam() != ESP_OK) {
      Serial.println("Start failed");
      return;
    }
    // 10s smoke test
    uint32_t start = millis(), got0 = chunks_rx;
    while (millis() - start < 10000) {
      pump_once();
      delay(2);
    }
    uint32_t got = chunks_rx - got0;
    Serial.printf("LCD_CAM test: %lu chunks in 10s\n", (unsigned long)got);
    if (!got) {
      Serial.println("FAIL: No chunks. Check PCLK and that DE is asserted (we force DE=HIGH).");
      print_status_line();
    }
  } else if (cmd == "stop") {
    stop_lcd_cam();
    Serial.println("Stopped.");
  } else if (cmd.startsWith("edge")) {
    int v = cmd.endsWith("1") ? 1 : 0;
    s_clk_inv = (uint8_t)v;
    LCD_CAM.cam_ctrl1.cam_clk_inv = s_clk_inv;
    LCD_CAM.cam_ctrl.cam_update   = 1;
    Serial.printf("cam_clk_inv set to %u\n", s_clk_inv);
  } else if (cmd.startsWith("deinv")) {
    int v = cmd.endsWith("1") ? 1 : 0;
    s_de_invert = (uint8_t)v;
    LCD_CAM.cam_ctrl1.cam_de_inv = s_de_invert;
    LCD_CAM.cam_ctrl.cam_update  = 1;
    Serial.printf("cam_de_inv set to %u\n", s_de_invert);
  } else if (cmd == "status") {
    print_status_line();
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | edge 0/1 | deinv 0/1");
  } else {
    Serial.printf("Unknown: %s\n", cmd.c_str());
  }
}

// ---------- Minimal setup/loop ----------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("ESP32-S3 LCD_CAM 4-bit capture (continuous, DE-gated)");
  Serial.println("Type 'help' for commands.");
}

void loop() {
  // poll stream if already running
  pump_once();

  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    cmd_process(s);
  }
}