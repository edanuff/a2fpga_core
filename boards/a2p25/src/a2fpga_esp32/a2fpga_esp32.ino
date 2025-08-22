/*
 * ESP32-S3 4-bit capture via LCD_CAM + GDMA
 * Mode: VSYNC -> EOF, stop_on_eof, immediate re-arm (good for gated PCLK)
 *
 * FPGA packet (10 clocks per transfer):
 *   nib 0..7 = data LSN-first, nib 8 = VSYNC, nib 9 = stopper
 *
 * Producer: poller task (low-latency) packs + pushes to SPSC ring.
 * Consumer: packet task pops and does demo print (replace with your app).
 *
 * Commands:
 *   lcam        -> init + start capture
 *   stop        -> stop capture
 *   status      -> print GPIO + GDMA status
 *   edge 0/1    -> cam_clk_inv (0=rising, 1=falling)
 *   we N        -> consumer prints every N words
 */

#include <Arduino.h>
#include "driver/gpio.h"
#include "soc/usb_serial_jtag_reg.h" // JTAG WRITE_PERI_REG
#include "a2fpga_lcam.h"
#include "a2fpga_jtag.h"
#include "a2fpga_spi_link.h"
#include "esp_err.h"
#include <ctype.h>
#include <stdlib.h>
#include "driver/i2s_std.h"

// ---------- Build-time options ----------
#define USE_GDMA_ISR         0   // keep 0 unless your core exposes a reliable GDMA IRQ
#define AUTOSTART 1      // set to 0 to disable
const uint32_t SMOKE_MS = 0;  // 0 = don't block boot with the 5s smoke test

// ---------- Pins (adjust to your wiring) ----------
#define PIN_LED0 1
#define PIN_LED1 2

#define LED_ON  HIGH
#define LED_OFF LOW

// Serial interface to the FPGA
#define PIN_RXD  44
#define PIN_TXD  43
#define BAUD 115200

// Configuration done signal from the FPGA
#define PIN_FPGA_DONE  48

// CAM interface to the FPGA
const int PIN_CAM_PCLK   = 13;
const int PIN_CAM_VSYNC  = 12;
const int PIN_CAM_D0     = 14;
const int PIN_CAM_D1     = 15;
const int PIN_CAM_D2     = 16;
const int PIN_CAM_D3     = 17;
#define PIN_DE_VIRT    10

// JTAG interface to the FPGA
const int PIN_TCK  = 40;
const int PIN_TMS  = 41;
const int PIN_TDI  = 42;
const int PIN_TDO  = 45;
const int PIN_SRST = 7;  // unused and unconnected, but required by the JTAG bridge

// SPI interface to the FPGA
static const int PIN_SCLK = 9;
static const int PIN_MOSI = 11;
static const int PIN_MISO = 10;
static const int SPI_HZ   = 1 * 1000 * 1000;  // start at 20 MHz; tune as needed

// I2S interface to the FPGA
const int PIN_I2S_BCLK = 18;
const int PIN_I2S_LRCLK = 47;
const int PIN_I2S_DATA = 33;

bool usb_was_connected = false;

// ---------- I2S (slave TX test pattern, 16-bit stereo) ----------
static i2s_chan_handle_t s_i2s_tx = NULL;
static TaskHandle_t s_i2s_task = NULL;
static volatile bool s_i2s_run = false;

static void i2s_tx_task(void *arg) {
  // Prepare a small buffer of stereo 16-bit frames: L=0xCAFE, R=0xBABE
  const size_t frames = 512; // number of stereo frames per write
  static uint16_t buf[frames * 2];
  for (size_t i = 0; i < frames; ++i) {
    buf[2*i+0] = 0xCAFE; // Left
    buf[2*i+1] = 0xBABE; // Right
  }
  while (s_i2s_run) {
    size_t written = 0;
    // Short timeout to avoid hogging CPU if clocks/DMA not ready
    esp_err_t err = i2s_channel_write(s_i2s_tx, buf, sizeof(buf), &written, pdMS_TO_TICKS(10));
    if (err != ESP_OK) {
      // Brief delay to avoid tight loop on error (e.g., no external clocks yet)
      vTaskDelay(pdMS_TO_TICKS(5));
    }
    // Yield periodically to avoid starving other tasks (LCD_CAM)
    taskYIELD();
  }
  vTaskDelete(NULL);
}

static esp_err_t i2s_tx_setup_once() {
  if (s_i2s_tx) return ESP_OK;

  // Create TX channel in SLAVE role (external BCLK/LRCLK from FPGA)
  // Use I2S_NUM_1 to avoid any potential contention
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_SLAVE);
  i2s_chan_handle_t tx = NULL;
  // Request a TX channel (2nd arg) only; RX handle is NULL
  esp_err_t err = i2s_new_channel(&chan_cfg, &tx, NULL);
  if (err != ESP_OK) return err;

  i2s_std_gpio_config_t gpio_cfg = {
    .mclk = I2S_GPIO_UNUSED,
    .bclk = (gpio_num_t)PIN_I2S_BCLK,
    .ws   = (gpio_num_t)PIN_I2S_LRCLK,
    .dout = (gpio_num_t)PIN_I2S_DATA,
    .din  = I2S_GPIO_UNUSED,
    .invert_flags = {
      .mclk_inv = false,
      .bclk_inv = false,
      .ws_inv   = false,
    },
  };

  // Standard I2S (Philips) format, 16-bit samples, stereo.
  i2s_std_config_t std_cfg = {
    .clk_cfg = {
      .sample_rate_hz = 48000,        // ignored in slave, ext clocks used
      .clk_src = I2S_CLK_SRC_DEFAULT,
      .mclk_multiple = I2S_MCLK_MULTIPLE_256,
    },
    .slot_cfg = {
      .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
      .slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT,
      .slot_mode = I2S_SLOT_MODE_STEREO,
      .slot_mask = (i2s_std_slot_mask_t)(I2S_STD_SLOT_LEFT | I2S_STD_SLOT_RIGHT),
      .ws_width = I2S_DATA_BIT_WIDTH_16BIT,  // one slot width
      .ws_pol = false,                        // standard I2S: low=left
      .bit_shift = true,                      // I2S compliant (one-bit delay)
      .left_align = false,
      .big_endian = false,
      .bit_order_lsb = false,                 // MSB first
    },
    .gpio_cfg = gpio_cfg,
  };

  err = i2s_channel_init_std_mode(tx, &std_cfg);
  if (err != ESP_OK) {
    i2s_del_channel(tx);
    return err;
  }
  s_i2s_tx = tx;
  return ESP_OK;
}

static esp_err_t i2s_tx_start() {
  esp_err_t err = i2s_tx_setup_once();
  if (err != ESP_OK) return err;
  err = i2s_channel_enable(s_i2s_tx);
  if (err != ESP_OK) return err;
  s_i2s_run = true;
  if (!s_i2s_task) {
    // Lower priority and move to other core to reduce contention
    BaseType_t ok = xTaskCreatePinnedToCore(i2s_tx_task, "i2s_tx", 4096, NULL, 3, &s_i2s_task, 1);
    if (ok != pdPASS) return ESP_ERR_NO_MEM;
  }
  return ESP_OK;
}

static void i2s_tx_stop() {
  s_i2s_run = false;
  if (s_i2s_task) { vTaskDelay(1); /* let task exit */ s_i2s_task = NULL; }
  if (s_i2s_tx) {
    i2s_channel_disable(s_i2s_tx);
    i2s_del_channel(s_i2s_tx);
    s_i2s_tx = NULL;
  }
}

// ---------- CLI Escape Sequence ----------
const char* CLI_ESCAPE_SEQUENCE = "+++";  // Escape sequence to enter CLI mode
const int ESCAPE_TIMEOUT_MS = 1000;       // Timeout for escape sequence detection
bool cli_mode = false;                     // Current mode: false=forwarding, true=CLI
String escape_buffer = "";                 // Buffer for escape sequence detection
unsigned long last_char_time = 0;          // Time of last character for timeout

// ---------- Helpers ----------
static bool parse_u32(const String &s, uint32_t &out) {
  // Supports decimal and 0x-prefixed hex
  const char *c = s.c_str();
  char *endp = nullptr;
  unsigned long v = strtoul(c, &endp, 0);
  if (endp == c) return false;
  out = (uint32_t)v;
  return true;
}

static int split_ws(const String &line, String *toks, int max_toks) {
  int n = 0;
  int i = 0;
  while (i < (int)line.length() && n < max_toks) {
    // skip spaces
    while (i < (int)line.length() && isspace((int)line[i])) i++;
    if (i >= (int)line.length()) break;
    // start
    int j = i;
    while (j < (int)line.length() && !isspace((int)line[j])) j++;
    toks[n++] = line.substring(i, j);
    i = j;
  }
  return n;
}

// ---------- Commands ----------
static void cmd_process(String cmd) {
  cmd.trim(); cmd.toLowerCase();
  if (cmd == "lcam") {
    lcam_start();
  } else if (cmd == "stop") {
    lcam_stop();
  } else if (cmd == "status") {
    lcam_print_status();
  } else if (cmd.startsWith("we ")) {
    long n = cmd.substring(3).toInt();
    lcam_log_every_n_words(n);
  } else if (cmd.startsWith("spitest")) {
    Serial.println("[SPI] spitest: begin");
    // Small guard to ensure prior run is fully idle
    vTaskDelay(2);

    spi_link_t link;
    esp_err_t err = spi_link_init(&link, SPI2_HOST, /*SCLK*/ PIN_SCLK, /*MOSI*/ PIN_MOSI, /*MISO*/ PIN_MISO, SPI_HZ);
    Serial.printf("[SPI] init SCLK=%d MOSI=%d MISO=%d @%d Hz -> %s\n",
                  PIN_SCLK, PIN_MOSI, PIN_MISO, SPI_HZ, (err==ESP_OK?"OK":esp_err_to_name(err)));
    if (err != ESP_OK) return;

    auto print_status = [](uint8_t s){
      uint8_t ver = (s >> 4) & 0xF;
      uint8_t align = (s >> 3) & 1;
      uint8_t crcerr = (s >> 2) & 1;
      uint8_t busy = (s >> 1) & 1;
      uint8_t ok = s & 1;
      Serial.printf("[SPI] status=0x%02X ver=%u align=%u crcerr=%u busy=%u ok=%u\n", s, ver, align, crcerr, busy, ok);
    };

    // Register R/W
    err = spi_reg_write(&link, 0x06, 0x55);
    Serial.printf("[SPI] reg[0x06] <= 0x55 -> %s\n", (err==ESP_OK?"OK":esp_err_to_name(err)));

    uint8_t proto = 0xFF, st = 0x00;
    err = spi_reg_read_status(&link, 0x04, &proto, &st); // PROTO_VER from core
    Serial.printf("[SPI] reg[0x04] (PROTO_VER) -> 0x%02X (%s)\n", proto, (err==ESP_OK?"OK":esp_err_to_name(err)));
    print_status(st);

    uint8_t echo = 0x00;
    err = spi_reg_read_status(&link, 0x06, &echo, &st);
    Serial.printf("[SPI] reg[0x06] readback -> 0x%02X (%s)\n", echo, (err==ESP_OK?"OK":esp_err_to_name(err)));
    print_status(st);

    // XFER to SPACE 0 (demo RAM in core)
    uint8_t buf_w[4] = {1,2,3,4};
    Serial.printf("[SPI] xfer-w space=0 addr=0x%02X len=%d data=", 0x20, 4);
    for (int i=0;i<4;i++) Serial.printf("%s%02X", (i?" ":""), buf_w[i]);
    Serial.println();
    err = spi_xfer_write(&link, /*space*/0, /*addr*/0x20, buf_w, 4, /*inc*/true);
    Serial.printf("[SPI] xfer-w -> %s\n", (err==ESP_OK?"OK":esp_err_to_name(err)));

    uint8_t buf_r[4] = {0};
    err = spi_xfer_read_status(&link, 0, 0x20, buf_r, 4, true, &st);
    Serial.printf("[SPI] xfer-r -> %s, data=", (err==ESP_OK?"OK":esp_err_to_name(err)));
    for (int i=0;i<4;i++) Serial.printf("%s%02X", (i?" ":""), buf_r[i]);
    Serial.println();
    print_status(st);

    bool match = (memcmp(buf_w, buf_r, 4) == 0);
    Serial.printf("[SPI] roundtrip %s\n", match?"MATCH":"MISMATCH");

    spi_link_cleanup(&link);
    Serial.println("[SPI] spitest: end");
  } else if (cmd.startsWith("spireg")) {
    // Register access helper
    // Usage:
    //   spireg <reg>                -> read 1 byte
    //   spireg <reg> <val>          -> write 1 byte
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (nt < 2) {
      Serial.println("Usage: spireg <reg> [value]");
    } else {
      uint32_t reg; if (!parse_u32(toks[1], reg) || reg > 126) {
        Serial.println("spireg: invalid <reg> (0..126)");
      } else if (nt == 2) {
        spi_link_t link; uint8_t val = 0, st = 0; esp_err_t err;
        err = spi_link_init(&link, SPI2_HOST, PIN_SCLK, PIN_MOSI, PIN_MISO, SPI_HZ);
        if (err != ESP_OK) { Serial.printf("spireg: init error: %s\n", esp_err_to_name(err)); return; }
        err = spi_reg_read_status(&link, (uint8_t)reg, &val, &st);
        if (err == ESP_OK) Serial.printf("reg[0x%02X] -> 0x%02X (status=0x%02X)\n", (unsigned)reg, val, st);
        else Serial.printf("spireg: read error: %s\n", esp_err_to_name(err));
        spi_link_cleanup(&link);
      } else {
        uint32_t v; if (!parse_u32(toks[2], v) || v > 0xFF) {
          Serial.println("spireg: invalid <value> (0..255)");
        } else {
          spi_link_t link; esp_err_t err;
          err = spi_link_init(&link, SPI2_HOST, PIN_SCLK, PIN_MOSI, PIN_MISO, SPI_HZ);
          if (err != ESP_OK) { Serial.printf("spireg: init error: %s\n", esp_err_to_name(err)); return; }
          err = spi_reg_write(&link, (uint8_t)reg, (uint8_t)v);
          if (err == ESP_OK) Serial.printf("reg[0x%02X] <= 0x%02X\n", (unsigned)reg, (unsigned)v);
          else Serial.printf("spireg: write error: %s\n", esp_err_to_name(err));
          spi_link_cleanup(&link);
        }
      }
    }
  } else if (cmd.startsWith("spir ")) {
    // XFER read
    // Usage: spir <space> <addr> <len> [inc=1]
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (nt < 4) {
      Serial.println("Usage: spir <space> <addr> <len> [inc=1]");
    } else {
      uint32_t space, addr, len; uint32_t inc = 1;
      if (!parse_u32(toks[1], space) || space > 7) { Serial.println("spir: <space> 0..7"); return; }
      if (!parse_u32(toks[2], addr)) { Serial.println("spir: invalid <addr>"); return; }
      if (!parse_u32(toks[3], len) || len == 0 || len > 4096) { Serial.println("spir: <len> 1..4096"); return; }
      if (nt >= 5) { if (!parse_u32(toks[4], inc)) { Serial.println("spir: invalid [inc]"); return; } }
      spi_link_t link; esp_err_t err = spi_link_init(&link, SPI2_HOST, PIN_SCLK, PIN_MOSI, PIN_MISO, SPI_HZ);
      if (err != ESP_OK) { Serial.printf("spir: init error: %s\n", esp_err_to_name(err)); return; }
      uint8_t *buf = (uint8_t*)malloc(len);
      if (!buf) { Serial.println("spir: OOM"); spi_link_cleanup(&link); return; }
      uint8_t st = 0;
      err = spi_xfer_read_status(&link, (uint8_t)space, addr, buf, (uint16_t)len, inc != 0, &st);
      if (err == ESP_OK) {
        Serial.printf("spir: space=%u addr=0x%06lX len=%lu inc=%u status=0x%02X\n", (unsigned)space, (unsigned long)addr, (unsigned long)len, (unsigned)(inc!=0), st);
        for (uint32_t i = 0; i < len; i++) {
          if ((i % 16) == 0) Serial.printf("%s%06lX:", (i?"\n":""), (unsigned long)(addr + i));
          Serial.printf(" %02X", buf[i]);
        }
        Serial.println();
      } else {
        Serial.printf("spir: read error: %s\n", esp_err_to_name(err));
      }
      free(buf);
      spi_link_cleanup(&link);
    }
  } else if (cmd.startsWith("spiw ")) {
    // XFER write
    // Usage: spiw <space> <addr> <inc> <b0> [b1 ...]
    //        spiw <space> <addr> <len> <b0> [b1 ...]  (len must match data count)
    String toks[64]; int nt = split_ws(cmd, toks, 64);
    if (nt < 5) {
      Serial.println("Usage: spiw <space> <addr> <inc> <b0> [b1 ...]");
    } else {
      uint32_t space, addr; uint32_t third;
      if (!parse_u32(toks[1], space) || space > 7) { Serial.println("spiw: <space> 0..7"); return; }
      if (!parse_u32(toks[2], addr)) { Serial.println("spiw: invalid <addr>"); return; }
      if (!parse_u32(toks[3], third)) { Serial.println("spiw: invalid <inc>/<len>"); return; }
      // Heuristic: if there are exactly 5 tokens, assume 'inc' and a single byte
      // Otherwise if third <= 1 treat as 'inc' else treat as 'len'
      bool has_len = false; uint32_t len = 0; bool inc = true; int data_start_idx = 4;
      if (third <= 1) { inc = (third != 0); len = nt - data_start_idx; }
      else { has_len = true; len = third; inc = true; }
      if (len == 0 || len > 4096) { Serial.println("spiw: <len> 1..4096"); return; }
      if ((uint32_t)(nt - data_start_idx) < len) { Serial.println("spiw: not enough data bytes"); return; }
      uint8_t *buf = (uint8_t*)malloc(len);
      if (!buf) { Serial.println("spiw: OOM"); return; }
      for (uint32_t i = 0; i < len; i++) {
        uint32_t v; if (!parse_u32(toks[data_start_idx + i], v) || v > 0xFF) {
          Serial.printf("spiw: bad byte at %lu\n", (unsigned long)i);
          free(buf); return;
        }
        buf[i] = (uint8_t)v;
      }
      spi_link_t link; esp_err_t err = spi_link_init(&link, SPI2_HOST, PIN_SCLK, PIN_MOSI, PIN_MISO, SPI_HZ);
      if (err != ESP_OK) { Serial.printf("spiw: init error: %s\n", esp_err_to_name(err)); free(buf); return; }
      err = spi_xfer_write(&link, (uint8_t)space, addr, buf, (uint16_t)len, inc);
      if (err == ESP_OK) Serial.printf("spiw: wrote %lu bytes to space=%u addr=0x%06lX inc=%u\n", (unsigned long)len, (unsigned)space, (unsigned long)addr, (unsigned)inc);
      else Serial.printf("spiw: write error: %s\n", esp_err_to_name(err));
      spi_link_cleanup(&link);
      free(buf);
    }
  } else if (cmd == "i2sstart") {
    esp_err_t err = i2s_tx_start();
    if (err == ESP_OK) Serial.println("I2S slave-TX started: L=0xCAFE R=0xBABE (16-bit)");
    else Serial.printf("I2S start error: %s\n", esp_err_to_name(err));
  } else if (cmd == "i2sstop") {
    i2s_tx_stop();
    Serial.println("I2S stopped");
  } else if (cmd == "exit") {
    cli_mode = false;
    lcam_set_logging(0);
    Serial.println("Exiting CLI mode. Returning to serial forwarding mode.");
    Serial.println("Use '+++' to enter CLI mode again.");
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | spitest | spireg | spir | spiw | i2sstart | i2sstop | exit | we N");
    Serial.println("  spireg <reg> [val]       - read/write 1-byte register (0..126)");
    Serial.println("  spir <space> <addr> <len> [inc=1] - read bytes");
    Serial.println("  spiw <space> <addr> <inc|len> <b0> [b1 ...] - write bytes");
    Serial.println("  i2sstart                  - start I2S slave-TX (ext BCLK/WS) sending L=0xCA, R=0xFE");
    Serial.println("  i2sstop                   - stop I2S output");
    Serial.println("  exit - Return to serial forwarding mode");
  } else if (cmd.length()) {
    Serial.printf("Unknown: %s\n", cmd.c_str());
  }
}

// ---------- Escape Sequence Detection ----------
void check_escape_timeout() {
  if (escape_buffer.length() > 0 && (millis() - last_char_time) > ESCAPE_TIMEOUT_MS) {
    // Timeout reached, forward any buffered characters
    if (!cli_mode) {  // Only forward if we're still in forwarding mode
      for (int i = 0; i < escape_buffer.length(); i++) {
        Serial1.write(escape_buffer.charAt(i));
      }
    }
    escape_buffer = "";
  }
}

String check_escape_sequence(char c) {
  last_char_time = millis();
  escape_buffer += c;
  
  // Check if we have the complete escape sequence
  if (escape_buffer == CLI_ESCAPE_SEQUENCE) {
    escape_buffer = "";
    cli_mode = true;
    Serial.println("\nEntering CLI mode. Type 'help' for commands or 'exit' to return to forwarding.");
    lcam_set_logging(2);
    return "";  // Don't forward anything
  }
  
  // Check if current buffer is a valid prefix of the escape sequence
  if (String(CLI_ESCAPE_SEQUENCE).startsWith(escape_buffer)) {
    return "";  // Keep buffering, don't forward yet
  }
  
  // Not a valid prefix, need to forward buffered chars except the last one
  String to_forward = escape_buffer.substring(0, escape_buffer.length() - 1);
  escape_buffer = String(c);  // Start new buffer with current char
  
  // Check if the single character is a prefix
  if (String(CLI_ESCAPE_SEQUENCE).startsWith(escape_buffer)) {
    return to_forward;  // Forward the old chars, keep current char in buffer
  } else {
    // Current char is also not a prefix, forward everything
    String result = to_forward + c;
    escape_buffer = "";
    return result;
  }
}

// ---------- Arduino sketch ----------
void setup() {
  Serial.begin(115200);
  Serial1.begin(BAUD, SERIAL_8N1, PIN_RXD, PIN_TXD); // hardware serial
  delay(300);
  Serial.printf("A2FPGA ESP32-S3 Firmware (%s %s)\n", __DATE__, __TIME__);

  Serial.println("Serial forwarding mode active. Use '+++' to enter CLI mode.");
  
  // Initialize in forwarding mode
  cli_mode = false;

  pinMode(PIN_CAM_PCLK, INPUT);
  pinMode(PIN_CAM_VSYNC, INPUT);
  pinMode(PIN_CAM_D0, INPUT);
  pinMode(PIN_CAM_D1, INPUT);
  pinMode(PIN_CAM_D2, INPUT);
  pinMode(PIN_CAM_D3, INPUT);

  pinMode(PIN_FPGA_DONE, INPUT_PULLUP);
  pinMode(PIN_LED0, OUTPUT);

  #if AUTOSTART
  lcam_start();
  #endif
}

void loop() {
  digitalWrite(PIN_LED0, digitalRead(PIN_FPGA_DONE)); // FPGA status

  // Handle USB JTAG connection changes
  bool usb_is_connected = usb_serial_jtag_is_connected();
  if(usb_was_connected == false && usb_is_connected == true)
    route_usb_jtag_to_gpio();
  if(usb_was_connected == true && usb_is_connected == false)
    unroute_usb_jtag_to_gpio();
  usb_was_connected = usb_is_connected;

  // Check escape sequence timeout
  check_escape_timeout();

  if (cli_mode) {
    // CLI Mode: Process commands
    if (Serial.available()) {
      String s = Serial.readStringUntil('\n');
      cmd_process(s);
    }
  } else {
    // Forwarding Mode: Forward between Serial and Serial1
    // Check for escape sequence on incoming Serial data
    if (Serial.available()) {
      char c = Serial.read();
      String to_forward = check_escape_sequence(c);
      if (to_forward.length() > 0) {
        // Forward any characters that aren't part of escape sequence
        for (int i = 0; i < to_forward.length(); i++) {
          Serial1.write(to_forward.charAt(i));
        }
      }
    }
    
    // Always forward Serial1 to Serial
    if (Serial1.available()) {
      Serial.write(Serial1.read());
    }
  }

  vTaskDelay(1);
}
