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
#include <WiFi.h>
#include "a2fpga_radio.h"
#include "a2fpga_tone.h"
#include "driver/gpio.h"
#include "soc/usb_serial_jtag_reg.h" // JTAG WRITE_PERI_REG
#include "a2fpga_lcam.h"
#include "a2fpga_jtag.h"
#include "a2fpga_spi_link.h"
#include "esp_err.h"
#include <ctype.h>
#include <stdlib.h>
#include "driver/i2s_std.h"
#include "es5503.h"

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

// ---------- ES5503 Sound Chip ----------
static ES5503* g_es5503 = nullptr;
static volatile bool s_es5503_run = false; // Simple flag to enable/disable ES5503
static const uint32_t ES5503_SAMPLE_RATE = 44100;
static const uint32_t ES5503_CLOCK_RATE = 7159090; // Apple IIgs clock rate
static const size_t AUDIO_BUFFER_FRAMES = 512;
static int16_t s_audio_buffer[AUDIO_BUFFER_FRAMES * 2]; // stereo

// I2S test mode
static volatile bool s_i2s_test_mode = false;
static int16_t s_test_buffer[AUDIO_BUFFER_FRAMES * 2]; // stereo test pattern

// Internet Radio streaming - using new radio module

// Helper to access ES5503 wave memory (for testing only!)
uint8_t* es5503_get_wave_memory(ES5503* es) {
  if (!es) return nullptr;
  return es->get_wave_memory();
}

// ES5503 direct generation (no buffering - generate on-demand in I2S task)

// ---------- I2S (slave TX for ES5503 audio, 16-bit stereo) ----------
static i2s_chan_handle_t s_i2s_tx = NULL;
static TaskHandle_t s_i2s_task = NULL;
static volatile bool s_i2s_run = false;

bool i2s_tx_is_running() {
  return (s_i2s_tx != NULL && s_i2s_run);
}

static void i2s_tx_task(void *arg) {
  while (s_i2s_run) {
    size_t written = 0;
    // Tone generator mode (highest priority for debugging)
    if (A2FPGATone::isActive()) {
      static int16_t mono_buffer[AUDIO_BUFFER_FRAMES];
      static int16_t stereo_buffer[AUDIO_BUFFER_FRAMES * 2];
      
      // Generate mono tone samples
      A2FPGATone::generateSamples(mono_buffer, AUDIO_BUFFER_FRAMES);
      
      // Convert mono to stereo (duplicate each sample to L+R)
      for (size_t i = 0; i < AUDIO_BUFFER_FRAMES; i++) {
        stereo_buffer[i * 2] = mono_buffer[i];     // Left channel
        stereo_buffer[i * 2 + 1] = mono_buffer[i]; // Right channel  
      }
      
      if (s_i2s_tx) {
        esp_err_t err = i2s_channel_write(s_i2s_tx, stereo_buffer, sizeof(stereo_buffer), &written, pdMS_TO_TICKS(10));
        if (err != ESP_OK) {
          vTaskDelay(pdMS_TO_TICKS(5));
        }
      }
    }
    // Radio mode - check for PCM data from radio module
    else if (A2FPGARadio::isActive()) {
      // Get PCM data from radio module and send to I2S
      static int16_t radio_buffer[AUDIO_BUFFER_FRAMES * 2];
      size_t samples_read = A2FPGARadio::readPCMSamples(radio_buffer, AUDIO_BUFFER_FRAMES * 2);
      
      if (samples_read > 0 && s_i2s_tx) {
        esp_err_t err = i2s_channel_write(s_i2s_tx, radio_buffer, sizeof(radio_buffer), &written, pdMS_TO_TICKS(10));
        if (err != ESP_OK) {
          vTaskDelay(pdMS_TO_TICKS(5));
        }
      } else {
        vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
    // Use test pattern if in test mode
    else if (s_i2s_test_mode && s_i2s_tx) {
      esp_err_t err = i2s_channel_write(s_i2s_tx, s_test_buffer, sizeof(s_test_buffer), &written, pdMS_TO_TICKS(10));
      if (err != ESP_OK) {
        // Brief delay to avoid tight loop on error (e.g., no external clocks yet)
        vTaskDelay(pdMS_TO_TICKS(5));
      }
    }
    // ES5503 direct generation (like tone generator)
    else if (g_es5503 && s_es5503_run && s_i2s_tx) {
      static int16_t mono_buffer[AUDIO_BUFFER_FRAMES]; // mono generation buffer
      static int16_t stereo_buffer[AUDIO_BUFFER_FRAMES * 2]; // stereo output buffer
      
      // Generate ES5503 mono audio directly on-demand
      g_es5503->generate_audio(mono_buffer, AUDIO_BUFFER_FRAMES);
      
      // Debug: Check if ES5503 is generating non-zero samples
      static int debug_count = 0;
      if (++debug_count % 5000 == 0) {
        int non_zero_count = 0;
        for (size_t i = 0; i < AUDIO_BUFFER_FRAMES; i++) {
          if (mono_buffer[i] != 0) non_zero_count++;
        }
        Serial.printf("ES5503 direct: %d/%d non-zero samples (range: %d to %d)\n", 
                      non_zero_count, AUDIO_BUFFER_FRAMES,
                      mono_buffer[0], mono_buffer[AUDIO_BUFFER_FRAMES-1]);
      }
      
      // Convert mono to stereo (duplicate each mono sample to L+R)
      for (size_t i = 0; i < AUDIO_BUFFER_FRAMES; i++) {
        stereo_buffer[i * 2] = mono_buffer[i];     // Left channel
        stereo_buffer[i * 2 + 1] = mono_buffer[i]; // Right channel
      }
      
      esp_err_t err = i2s_channel_write(s_i2s_tx, stereo_buffer, sizeof(stereo_buffer), &written, pdMS_TO_TICKS(10));
      if (err != ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(5));
      }
    } else if (s_i2s_tx) {
      // Send silence if ES5503 not running
      static int16_t silence_buffer[AUDIO_BUFFER_FRAMES * 2] = {0};
      esp_err_t err = i2s_channel_write(s_i2s_tx, silence_buffer, sizeof(silence_buffer), &written, pdMS_TO_TICKS(10));
      if (err != ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(5));
      }
    } else {
      // No I2S channel, just wait
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    // Yield periodically to avoid starving other tasks (LCD_CAM)
    taskYIELD();
  }
  
  // Clear task handle before deleting
  s_i2s_task = NULL;
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
      .bit_shift = false,                     // Left-justified I2S (no delay)
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
  // Just stop test mode - keep I2S running but send silence
  s_i2s_test_mode = false;
  
  // Don't tear down the I2S channel - just let it send silence
  // This avoids the crash in i2s_del_channel()
}

// ---------- ES5503 Control Functions ----------
static esp_err_t es5503_init() {
  if (g_es5503) return ESP_OK;  // Already initialized
  
  // Show memory info before allocation
  size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  
  g_es5503 = ES5503::create_with_memory(ES5503_CLOCK_RATE);
  if (!g_es5503) {
    Serial.println("Failed to create ES5503 instance");
    return ESP_ERR_NO_MEM;
  }
  
  g_es5503->set_channels(1);  // Mono output (avoid stereo channel assignment issues)
  
  // Show where memory was allocated
  size_t psram_free_after = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  size_t internal_free_after = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  
  Serial.println("ES5503 initialized with 65536 bytes wave memory");
  if (psram_free != psram_free_after) {
    Serial.printf("Wave memory allocated in PSRAM (%d bytes used)\n", (int)(psram_free - psram_free_after));
  } else if (internal_free != internal_free_after) {
    Serial.printf("Wave memory allocated in internal RAM (%d bytes used)\n", (int)(internal_free - internal_free_after));
  }
  
  return ESP_OK;
}

static esp_err_t es5503_start() {
  esp_err_t err = es5503_init();
  if (err != ESP_OK) return err;
  
  s_es5503_run = true;
  Serial.println("ES5503 direct audio generation enabled");
  return ESP_OK;
}

static void es5503_stop() {
  s_es5503_run = false;
  Serial.println("ES5503 direct audio generation disabled");
}

static void es5503_cleanup() {
  es5503_stop();
  if (g_es5503) {
    delete g_es5503;
    g_es5503 = nullptr;
    Serial.println("ES5503 cleaned up");
  }
}

// Sound GLU state
static struct {
  uint8_t control_reg;     // $C03C - Sound Control register
  uint16_t address_ptr;     // $C03E-$C03F - Address pointer
  bool doc_access;          // false = RAM, true = DOC registers
  bool auto_increment;      // Auto-increment address after access
  uint8_t volume;           // Volume control (bits 3-0)
} s_glu = {0};

// Track ES5503 wave memory writes
static uint32_t s_wave_memory_writes = 0;
static bool s_es5503_debug = false;
static bool s_bus_debug = false;  // Debug all bus packets

// Bus packet statistics (global for reset)
static int s_total_packet_count = 0;
static int s_write_packet_count = 0;
static int s_es5503_packet_count = 0;      // Packets in ES5503 range
static int s_corrupted_packet_count = 0;   // Packets outside ES5503 range (excluding heartbeat)
static uint32_t s_last_packet_time_us = 0; // For timing analysis
static uint32_t s_total_packet_time_us = 0;
static uint32_t s_max_packet_gap_us = 0;
static uint32_t s_min_packet_gap_us = UINT32_MAX;
static uint16_t s_min_addr = 0xFFFF, s_max_addr = 0x0000;

// Handle Sound GLU and ES5503 writes from bus packet
static void handle_es5503_write(uint16_t address, uint8_t data) {
  if (!g_es5503) return;
  
  // Sound GLU registers at $C03C-$C03F
  if (address == 0xC03C) {
    // Sound Control Register
    s_glu.control_reg = data;
    s_glu.doc_access = !(data & 0x40);  // Bit 6: 0=DOC, 1=RAM
    s_glu.auto_increment = (data & 0x20);  // Bit 5: auto-increment
    s_glu.volume = data & 0x0F;  // Bits 3-0: volume
    
    if (s_es5503_debug) {
      Serial.printf("GLU Control: 0x%02X (access=%s, auto_inc=%d, vol=%d)\n", 
                    data, s_glu.doc_access ? "DOC" : "RAM", 
                    s_glu.auto_increment ? 1 : 0, s_glu.volume);
    }
  }
  else if (address == 0xC03D) {
    // Data Register - write to DOC or RAM based on control register
    if (s_glu.doc_access) {
      // Write to DOC register (low byte of address pointer is register number)
      uint8_t reg = s_glu.address_ptr & 0xFF;
      g_es5503->write(reg, data);
      
      if (s_es5503_debug) {
        Serial.printf("DOC[0x%02X] <= 0x%02X\n", reg, data);
      }
    } else {
      // Write to wave RAM at address pointer
      uint8_t* wave_mem = es5503_get_wave_memory(g_es5503);
      if (wave_mem && (s_glu.address_ptr < 65536)) {
        wave_mem[s_glu.address_ptr] = data;
        s_wave_memory_writes++;
        
        if (s_es5503_debug) {
          // Show first few writes and then every 256th
          if (s_wave_memory_writes <= 10 || (s_wave_memory_writes % 256 == 0)) {
            Serial.printf("Wave RAM[0x%04X] <= 0x%02X (total writes: %lu)\n", 
                          s_glu.address_ptr, data, s_wave_memory_writes);
          }
        }
      } else if (wave_mem && s_glu.address_ptr >= 65536) {
        if (s_es5503_debug) {
          Serial.printf("Wave RAM write IGNORED: addr 0x%04X out of bounds\n", s_glu.address_ptr);
        }
      }
    }
    
    // Auto-increment address if enabled
    if (s_glu.auto_increment) {
      s_glu.address_ptr++;
    }
  }
  else if (address == 0xC03E) {
    // Address Pointer Low
    s_glu.address_ptr = (s_glu.address_ptr & 0xFF00) | data;
    if (s_es5503_debug) {
      Serial.printf("GLU Addr Low: 0x%02X (ptr=0x%04X)\n", data, s_glu.address_ptr);
    }
  }
  else if (address == 0xC03F) {
    // Address Pointer High
    s_glu.address_ptr = (s_glu.address_ptr & 0x00FF) | (data << 8);
    if (s_es5503_debug) {
      Serial.printf("GLU Addr High: 0x%02X (ptr=0x%04X)\n", data, s_glu.address_ptr);
    }
  }
}

// Process bus packet from FPGA
void process_bus_packet(uint32_t packet) {
  // Decode packet: [ADDR:16][DATA:8][FLAGS:8]
  uint16_t address = (packet >> 16) & 0xFFFF;
  uint8_t data = (packet >> 8) & 0xFF;
  uint8_t flags = packet & 0xFF;
  
  // Debug packet format for first few packets
  static int packet_debug_count = 0;
  if (packet_debug_count < 10) {
    packet_debug_count++;
    Serial.printf("Raw packet: 0x%08X → addr=$%04X data=$%02X flags=$%02X\n", 
                  packet, address, data, flags);
  }
  
  bool rw_n = (flags >> 7) & 1;      // Read/Write (1=read, 0=write)
  bool reset_indicator = flags & 1;   // Reset packet indicator
  
  // Debug bus packets if enabled (filter out heartbeat spam)
  if (s_bus_debug && !reset_indicator && address != 0xC0FF) {
    Serial.printf("Bus: %s $%04X = $%02X (flags=$%02X)\n", 
                  rw_n ? "READ" : "WRITE", address, data, flags);
  }
  
  // Track address range we're receiving from FPGA (excluding heartbeat)
  if (!reset_indicator && address != 0xC0FF) {  // Exclude heartbeat
    s_total_packet_count++;
    if (!rw_n) s_write_packet_count++;
    
    // Timing analysis - measure gaps between packets
    uint32_t now_us = micros();
    if (s_last_packet_time_us > 0) {
      uint32_t gap_us = now_us - s_last_packet_time_us;
      s_total_packet_time_us += gap_us;
      if (gap_us > s_max_packet_gap_us) s_max_packet_gap_us = gap_us;
      if (gap_us < s_min_packet_gap_us) s_min_packet_gap_us = gap_us;
    }
    s_last_packet_time_us = now_us;
    
    // Count ES5503 vs corrupted packets
    if (address >= 0xC03C && address <= 0xC03F) {
      s_es5503_packet_count++;
    } else {
      s_corrupted_packet_count++;
    }
    
    // Track address range (excluding heartbeat)
    if (address < s_min_addr) s_min_addr = address;
    if (address > s_max_addr) s_max_addr = address;
    
    // Show statistics every 100 non-heartbeat packets  
    if (s_total_packet_count % 100 == 0) {
      Serial.printf("FPGA filter stats (no heartbeat): %d total, %d writes, ES5503: %d, corrupted: %d, addr range $%04X-$%04X\n", 
                    s_total_packet_count, s_write_packet_count, s_es5503_packet_count, s_corrupted_packet_count, s_min_addr, s_max_addr);
    }
  }
  
  // Skip reset packets and read operations
  if (reset_indicator || rw_n) return;
  
  // Check if this is an ES5503 write
  if (address >= 0xC03C && address <= 0xC03F) {
    handle_es5503_write(address, data);
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
    // Also show ES5503 and bus capture stats
    Serial.printf("ES5503 wave memory writes: %lu\n", s_wave_memory_writes);
    Serial.printf("GLU address pointer: 0x%04X\n", s_glu.address_ptr);
    Serial.printf("Bus debug: %s, ES5503 debug: %s\n", 
                  s_bus_debug ? "ON" : "OFF", s_es5503_debug ? "ON" : "OFF");
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
    Serial.println("I2S test pattern stopped (I2S channel still active)");
  } else if (cmd == "i2stest") {
    // Fill test buffer with 0xCAFE (left) and 0xBABE (right)
    for (int i = 0; i < AUDIO_BUFFER_FRAMES; i++) {
      s_test_buffer[i * 2] = (int16_t)0xCAFE;      // Left channel
      s_test_buffer[i * 2 + 1] = (int16_t)0xBABE;  // Right channel
    }
    s_i2s_test_mode = true;
    
    // Debug: show actual byte values being sent
    uint8_t* byte_ptr = (uint8_t*)s_test_buffer;
    Serial.printf("First 8 bytes in buffer: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                  byte_ptr[0], byte_ptr[1], byte_ptr[2], byte_ptr[3],
                  byte_ptr[4], byte_ptr[5], byte_ptr[6], byte_ptr[7]);
    Serial.printf("As int16_t values: L=0x%04X R=0x%04X\n", 
                  (uint16_t)s_test_buffer[0], (uint16_t)s_test_buffer[1]);
    
    // Start I2S if not already running
    if (!i2s_tx_is_running()) {
      esp_err_t err = i2s_tx_start();
      if (err != ESP_OK) {
        Serial.printf("I2S start error: %s\n", esp_err_to_name(err));
        s_i2s_test_mode = false;
        return;
      }
    }
    Serial.println("I2S test mode: sending L=0xCAFE R=0xBABE continuously");
    Serial.println("Use 'i2sstop' to stop the test");
  } else if (cmd == "es5503start") {
    esp_err_t err = es5503_start();
    if (err == ESP_OK) {
      Serial.println("ES5503 started");
      
      // Auto-start I2S for audio output
      if (!i2s_tx_is_running()) {
        esp_err_t i2s_err = i2s_tx_start();
        if (i2s_err != ESP_OK) {
          Serial.printf("Failed to start I2S for ES5503: %s\n", esp_err_to_name(i2s_err));
        } else {
          Serial.println("I2S started automatically for ES5503 audio");
        }
      }
    } else {
      Serial.printf("ES5503 start failed: %s\n", esp_err_to_name(err));
    }
  } else if (cmd == "es5503stop") {
    es5503_stop();
  } else if (cmd == "audiostop") {
    // Stop all audio generation by halting all oscillators
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else {
      // Halt all 32 oscillators by setting their control bit 0 to 1
      for (int osc = 0; osc < 32; osc++) {
        uint8_t current_control = g_es5503->read(0xA0 + osc);
        g_es5503->write(0xA0 + osc, current_control | 0x01);  // Set halt bit
      }
      Serial.println("All ES5503 audio generation stopped (all oscillators halted)");
    }
  } else if (cmd == "starttone" || cmd.startsWith("starttone ")) {
    // Start tone generator for audio debugging
    float frequency = 440.0f; // Default frequency
    
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (nt >= 2) {
      frequency = toks[1].toFloat();
      if (frequency <= 0.0f || frequency > 22050.0f) {
        Serial.println("Frequency must be between 0.1 and 22050 Hz");
        return;
      }
    }
    
    if (A2FPGATone::start(frequency)) {
      // Auto-start I2S for audio output
      if (!i2s_tx_is_running()) {
        esp_err_t i2s_err = i2s_tx_start();
        if (i2s_err != ESP_OK) {
          Serial.printf("Failed to start I2S for tone: %s\n", esp_err_to_name(i2s_err));
          A2FPGATone::stop();
        } else {
          Serial.println("I2S started automatically for tone generator");
        }
      }
      Serial.printf("Tone generator started at %.1fHz\n", frequency);
    } else {
      Serial.println("Failed to start tone generator");
    }
  } else if (cmd == "stoptone") {
    A2FPGATone::stop();
    Serial.println("Tone generator stopped");
  } else if (cmd.startsWith("audiostart ")) {
    // Enable audio generation with specified number of oscillators
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else if (nt < 2) {
      Serial.println("Usage: audiostart <num_oscillators> (1-32)");
    } else {
      long num_oscs = toks[1].toInt();
      if (num_oscs < 1 || num_oscs > 32) {
        Serial.println("Number of oscillators must be 1-32");
      } else {
        g_es5503->write(0xE0, (uint8_t)(num_oscs - 1));  // E0 register: num_oscs - 1
        Serial.printf("ES5503 audio enabled with %ld oscillators\n", num_oscs);
      }
    }
  } else if (cmd == "es5503test") {
    // Test ES5503 by loading waveform and writing test register values
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else {
      // Ensure I2S is running for audio output
      if (!i2s_tx_is_running()) {
        esp_err_t err = i2s_tx_start();
        if (err != ESP_OK) {
          Serial.printf("Failed to start I2S for ES5503: %s\n", esp_err_to_name(err));
          return;
        }
        Serial.println("I2S started automatically for ES5503 test");
      }
      Serial.println("Testing ES5503 - loading test waveform and setting up oscillator...");
      
      // First load a high-quality sine wave into wave memory
      uint8_t* wave_mem = g_es5503->get_wave_memory();
      if (wave_mem) {
        // Generate a 4096-sample sine wave for high quality
        // ES5503 expects unsigned 8-bit (0-255) where 128 is zero
        const int WAVE_SIZE = 4096;
        Serial.printf("Generating %d-sample high-quality sine wave...\n", WAVE_SIZE);
        
        for (int i = 0; i < WAVE_SIZE; i++) {
          float angle = (2.0f * 3.14159265359f * i) / (float)WAVE_SIZE;
          // Generate signed value, avoid 0x00 which stops the oscillator!
          // Use range 1-255 (avoiding 0) for ES5503 compatibility
          float sample_f = 127.0f * sinf(angle) + 128.0f;
          uint8_t sample = (uint8_t)(sample_f + 0.5f); // Round to nearest
          
          // Clamp to avoid 0x00 (oscillator stop command)
          if (sample == 0) sample = 1;
          if (sample > 255) sample = 255;
          
          wave_mem[i] = sample;
        }
        Serial.printf("High-quality %d-sample sine wave loaded into wave memory\n", WAVE_SIZE);
        
        // Debug: show samples at key points (0°, 90°, 180°, 270°)
        Serial.printf("Sample values: [0]=0x%02X [%d]=0x%02X [%d]=0x%02X [%d]=0x%02X\n",
                      wave_mem[0], WAVE_SIZE/4, wave_mem[WAVE_SIZE/4], 
                      WAVE_SIZE/2, wave_mem[WAVE_SIZE/2], 3*WAVE_SIZE/4, wave_mem[3*WAVE_SIZE/4]);
        Serial.printf("Expected: 128 (0°), 255 (90°), 128 (180°), 1 (270° - avoiding 0x00!)\n");
      }
      
      // Set up oscillator 0 for A440 (440 Hz)
      // ES5503 frequency calculation: freq_reg = (target_freq * 65536) / sample_rate
      // For 440 Hz at 44100 Hz sample rate: (440 * 65536) / 44100 = 654 = 0x028E
      g_es5503->write(0x00, 0x8E);    // freq lo = 0x8E
      g_es5503->write(0x20, 0x02);    // freq hi = 0x02 (0x028E = A440 @ 44.1kHz)
      g_es5503->write(0x40, 0x80);    // volume = 0x80 (medium)
      g_es5503->write(0x80, 0x00);    // wavetable pointer = 0x0000
      g_es5503->write(0xC0, 0x20);    // wavetable size = 4096 (bits 5-3 = 100), resolution = 0
      g_es5503->write(0xE1, 0x01);    // enable 1 oscillator (E1 = number of oscillators)
      g_es5503->write(0xA0, 0x00);    // control = 0x00 (FREE mode, channel 0, running, explicit)
      
      // Verify the oscillator configuration
      uint8_t control_readback = g_es5503->read(0xA0);
      uint8_t mode = control_readback & 0x03;
      uint8_t channel = (control_readback >> 4) & 0x0F;
      bool halted = control_readback & 0x01;
      
      const char* mode_names[] = {"FREE", "ONCE", "SYNC", "SWAP"};
      Serial.printf("ES5503 Oscillator 0: mode=%s, channel=%d, halted=%s, control=0x%02X\n", 
                    mode_names[mode], channel, halted ? "YES" : "NO", control_readback);
      Serial.println("ES5503 configured with high-quality 4096-sample sine wave @ 440Hz.");
    }
  } else if (cmd == "es5503wave") {
    // Generate a test waveform in ES5503 wave memory
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else {
      Serial.println("Loading test sawtooth waveform (256 samples at address 0)...");
      // Direct access to wave memory for testing - this is a hack!
      // Normally wave memory is loaded via GLU at $C03C-$C03F
      extern uint8_t* es5503_get_wave_memory(ES5503* es);
      uint8_t* wave_mem = es5503_get_wave_memory(g_es5503);
      if (wave_mem) {
        for (int i = 0; i < 256; i++) {
          wave_mem[i] = (uint8_t)i;  // Simple sawtooth 0-255
        }
        Serial.println("Test waveform loaded. Run 'es5503test' to play it.");
      } else {
        Serial.println("Wave memory not accessible");
      }
    }
  } else if (cmd == "i2sstatus") {
    // Show I2S status and statistics
    extern bool i2s_tx_is_running();
    Serial.println("I2S Status:");
    Serial.printf("  I2S transmit: %s\n", i2s_tx_is_running() ? "RUNNING" : "STOPPED");
    Serial.printf("  BCLK on pin %d\n", PIN_I2S_BCLK);
    Serial.printf("  LRCLK on pin %d\n", PIN_I2S_LRCLK); 
    Serial.printf("  DATA on pin %d\n", PIN_I2S_DATA);
    if (i2s_tx_is_running()) {
      Serial.println("  Output: ES5503 audio samples");
    }
    if (g_es5503) {
      uint8_t num_osc = g_es5503->read(0xE1);
      Serial.printf("  ES5503: %d oscillators enabled\n", (num_osc/2) + 1);
    }
  } else if (cmd.startsWith("es5503reg ")) {
    // Direct ES5503 register access: es5503reg <reg> [value]
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else if (nt < 2) {
      Serial.println("Usage: es5503reg <reg> [value]");
    } else {
      uint32_t reg; if (!parse_u32(toks[1], reg) || reg > 0xFF) {
        Serial.println("es5503reg: invalid <reg> (0..255)");
      } else if (nt == 2) {
        uint8_t val = g_es5503->read((uint16_t)reg);
        Serial.printf("ES5503[0x%02X] -> 0x%02X\n", (unsigned)reg, val);
      } else {
        uint32_t v; if (!parse_u32(toks[2], v) || v > 0xFF) {
          Serial.println("es5503reg: invalid <value> (0..255)");
        } else {
          g_es5503->write((uint16_t)reg, (uint8_t)v);
          Serial.printf("ES5503[0x%02X] <= 0x%02X\n", (unsigned)reg, (unsigned)v);
        }
      }
    }
  } else if (cmd == "es5503debug") {
    // Toggle ES5503 register write debugging
    s_es5503_debug = !s_es5503_debug;
    Serial.printf("ES5503 debug %s\n", s_es5503_debug ? "enabled" : "disabled");
  } else if (cmd == "busdebug") {
    // Toggle all bus packet debugging
    s_bus_debug = !s_bus_debug;
    Serial.printf("Bus packet debug %s (heartbeat filtered)\n", s_bus_debug ? "enabled" : "disabled");
  } else if (cmd == "es5503busdebug") {
    // Toggle ES5503-only bus debugging (addresses $C030-$C04F)
    static bool es5503_bus_debug = false;
    es5503_bus_debug = !es5503_bus_debug;
    Serial.printf("ES5503 bus debug %s\n", es5503_bus_debug ? "enabled" : "disabled");
    // Patch the bus debug function temporarily
    // This is hacky but will work for debugging
  } else if (cmd == "es5503resetwrite") {
    // Clear write count and reset GLU state
    s_wave_memory_writes = 0;
    memset(&s_glu, 0, sizeof(s_glu));  // Reset GLU state
    Serial.printf("ES5503 write count and GLU state reset\n");
  } else if (cmd == "stats") {
    // Display current bus packet statistics
    Serial.printf("Bus packet statistics:\n");
    Serial.printf("  Total packets: %d\n", s_total_packet_count);
    Serial.printf("  Write packets: %d\n", s_write_packet_count);
    Serial.printf("  Read packets: %d\n", s_total_packet_count - s_write_packet_count);
    Serial.printf("  ES5503 packets: %d (correct)\n", s_es5503_packet_count);
    Serial.printf("  Corrupted packets: %d (wrong address)\n", s_corrupted_packet_count);
    if (s_total_packet_count > 0) {
      Serial.printf("  Address range: $%04X-$%04X\n", s_min_addr, s_max_addr);
      Serial.printf("  Corruption rate: %.1f%%\n", (100.0 * s_corrupted_packet_count) / s_total_packet_count);
      
      // Timing analysis
      if (s_total_packet_count > 1) {
        uint32_t avg_gap_us = s_total_packet_time_us / (s_total_packet_count - 1);
        Serial.printf("  Timing: avg=%lu us, min=%lu us, max=%lu us\n", 
                      (unsigned long)avg_gap_us, (unsigned long)s_min_packet_gap_us, (unsigned long)s_max_packet_gap_us);
      }
    } else {
      Serial.printf("  Address range: (no packets received)\n");
    }
    
    // LCD_CAM ring buffer stats
    uint32_t lcam_words = lcam_get_words_seen();
    uint32_t lcam_drops = lcam_get_ring_drops();
    Serial.printf("LCD_CAM statistics:\n");
    Serial.printf("  Words received: %lu\n", (unsigned long)lcam_words);
    Serial.printf("  Ring buffer drops: %lu\n", (unsigned long)lcam_drops);
    if (lcam_words > 0) {
      Serial.printf("  Drop rate: %.1f%%\n", (100.0 * lcam_drops) / (lcam_words + lcam_drops));
    }
  } else if (cmd == "resetstats") {
    // Reset bus packet statistics
    s_total_packet_count = 0;
    s_write_packet_count = 0;
    s_es5503_packet_count = 0;
    s_corrupted_packet_count = 0;
    s_min_addr = 0xFFFF;
    s_max_addr = 0x0000;
    s_last_packet_time_us = 0;
    s_total_packet_time_us = 0;
    s_max_packet_gap_us = 0;
    s_min_packet_gap_us = UINT32_MAX;
    Serial.println("Bus packet statistics reset");
  } else if (cmd == "es5503info") {
    // Display ES5503 oscillator information in human-readable format
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else {
      Serial.println("ES5503 Oscillator Status:");
      Serial.println("Osc | Freq(Hz) | Vol | Wave  | Size | Control | Status");
      Serial.println("----|----------|-----|-------|------|---------|------------------");
      
      // Get number of enabled oscillators
      uint8_t num_enabled = ((g_es5503->read(0xE1) >> 1) & 0x1F) + 1;
      
      for (int osc = 0; osc < 32; osc++) {
        // Read oscillator registers
        uint8_t freq_lo = g_es5503->read(0x00 + osc);
        uint8_t freq_hi = g_es5503->read(0x20 + osc);
        uint8_t volume = g_es5503->read(0x40 + osc);
        uint8_t data = g_es5503->read(0x60 + osc);
        uint8_t wave_ptr_hi = g_es5503->read(0x80 + osc);
        uint8_t control = g_es5503->read(0xA0 + osc);
        uint8_t config = g_es5503->read(0xC0 + osc);
        
        // Calculate frequency
        uint16_t freq_reg = (freq_hi << 8) | freq_lo;
        float frequency = 0;
        if (freq_reg > 0) {
          // Frequency = (freq_reg * sample_rate) / 65536
          // Sample rate for ES5503 is typically 26320 Hz (7159090 / 8 / 34)
          // But we're using 22050 Hz in our implementation
          frequency = (float)(freq_reg * 22050) / 65536.0f;
        }
        
        // Decode control bits
        bool halted = (control & 0x01) != 0;
        int mode = (control >> 1) & 0x03;
        bool irq_enabled = (control & 0x08) != 0;
        int channel = (control >> 4) & 0x0F;
        
        // Decode config bits
        bool bank = (config & 0x40) != 0;
        int wave_size_idx = (config >> 3) & 0x07;
        int resolution = config & 0x07;
        
        // Wave sizes: 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
        const int wave_sizes[] = {256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
        int wave_size = wave_sizes[wave_size_idx];
        
        // Wave pointer
        uint32_t wave_ptr = (wave_ptr_hi << 8);
        if (bank) wave_ptr |= 0x10000;
        
        // Mode names
        const char* mode_names[] = {"FREE", "ONCE", "SYNC", "SWAP"};
        
        // Status string
        char status[32];
        if (osc >= num_enabled) {
          snprintf(status, sizeof(status), "DISABLED");
        } else if (halted) {
          snprintf(status, sizeof(status), "HALTED");
        } else {
          snprintf(status, sizeof(status), "%s Ch%d%s", 
                   mode_names[mode], channel, irq_enabled ? " IRQ" : "");
        }
        
        // Print oscillator info
        Serial.printf("%2d  | %8.1f | %3d | %05X | %4d | %02X      | %s\n",
                      osc, frequency, volume, wave_ptr, wave_size, control, status);
        
        // Show only enabled oscillators unless verbose
        if (osc >= num_enabled && osc >= 3) {
          Serial.printf("... (%d oscillators disabled)\n", 32 - num_enabled);
          break;
        }
      }
      
      Serial.printf("\nEnabled oscillators: %d/32\n", num_enabled);
      Serial.printf("Wave memory writes: %lu\n", s_wave_memory_writes);
    }
  } else if (cmd.startsWith("es5503mem ")) {
    // Examine ES5503 wave memory
    // Format: es5503mem <addr> [length]
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (!g_es5503) {
      Serial.println("ES5503 not initialized. Use 'es5503start' first.");
    } else if (nt < 2) {
      Serial.println("Usage: es5503mem <addr> [length]");
      Serial.println("  addr: hex address (0-FFFF)");
      Serial.println("  length: number of bytes to display (default 256)");
    } else {
      uint32_t addr = strtoul(toks[1].c_str(), NULL, 16);
      uint32_t len = (nt >= 3) ? strtoul(toks[2].c_str(), NULL, 10) : 256;
      
      if (addr >= 65536) {
        Serial.printf("Address 0x%04X out of range (max 0xFFFF)\n", addr);
        return;
      }
      
      uint8_t* wave_mem = g_es5503->get_wave_memory();
      if (!wave_mem) {
        Serial.println("Wave memory not accessible");
        return;
      }
      
      // Limit length to avoid excessive output
      if (len > 512) len = 512;
      if (addr + len > 65536) len = 65536 - addr;
      
      Serial.printf("ES5503 memory at 0x%04X:\n", addr);
      
      // Display in hex dump format
      for (uint32_t i = 0; i < len; i += 16) {
        Serial.printf("%04X: ", addr + i);
        
        // Hex bytes
        for (int j = 0; j < 16 && (i + j) < len; j++) {
          Serial.printf("%02X ", wave_mem[addr + i + j]);
          if (j == 7) Serial.print(" ");
        }
        
        // Pad if less than 16 bytes on last line
        for (int j = (len - i); j < 16; j++) {
          Serial.print("   ");
          if (j == 7) Serial.print(" ");
        }
        
        Serial.print(" |");
        
        // ASCII representation
        for (int j = 0; j < 16 && (i + j) < len; j++) {
          uint8_t b = wave_mem[addr + i + j];
          Serial.printf("%c", (b >= 32 && b < 127) ? b : '.');
        }
        
        Serial.println("|");
      }
      
      // Show summary of non-zero regions
      uint32_t non_zero_count = 0;
      for (uint32_t i = 0; i < len; i++) {
        if (wave_mem[addr + i] != 0) non_zero_count++;
      }
      Serial.printf("Non-zero bytes: %d/%d\n", non_zero_count, len);
    }
  } else if (cmd == "fulltest") {
    // Full audio pipeline test
    Serial.println("Starting full ES5503 audio pipeline test...");
    
    // Initialize ES5503
    esp_err_t err = es5503_start();
    if (err != ESP_OK) {
      Serial.printf("ES5503 start failed: %s\n", esp_err_to_name(err));
      return;
    }
    Serial.println("ES5503 started");
    
    // Start I2S if not already running
    if (!i2s_tx_is_running()) {
      err = i2s_tx_start();
      if (err != ESP_OK) {
        Serial.printf("I2S start failed: %s\n", esp_err_to_name(err));
        return;
      }
      Serial.println("I2S started");
    } else {
      Serial.println("I2S already running");
    }
    
    // Load high-quality sine wave into wave memory
    Serial.println("Loading high-quality sine wave into wave memory...");
    uint8_t* wave_mem = g_es5503->get_wave_memory();
    if (!wave_mem) {
      Serial.println("ERROR: Cannot access ES5503 wave memory");
      return;
    }
    
    // Generate a 4096-sample sine wave for high quality  
    const int WAVE_SIZE = 4096;
    Serial.printf("Generating %d-sample high-quality sine wave...\n", WAVE_SIZE);
    
    for (int i = 0; i < WAVE_SIZE; i++) {
      float angle = (2.0f * 3.14159265359f * i) / (float)WAVE_SIZE;
      // Generate signed value, avoid 0x00 which stops the oscillator!
      // Use range 1-255 (avoiding 0) for ES5503 compatibility
      float sample_f = 127.0f * sinf(angle) + 128.0f;
      uint8_t sample = (uint8_t)(sample_f + 0.5f); // Round to nearest
      
      // Clamp to avoid 0x00 (oscillator stop command)
      if (sample == 0) sample = 1;
      if (sample > 255) sample = 255;
      
      wave_mem[i] = sample;
    }
    Serial.printf("High-quality %d-sample sine wave loaded into wave memory\n", WAVE_SIZE);
    
    // Write test ES5503 settings
    Serial.println("Writing ES5503 test configuration...");
    // Use frequency value that should produce ~440Hz
    // Original 0x1000 was high-pitched but audible, try 0x0800 (half that)
    g_es5503->write(0x00, 0x00);    // freq lo = 0x00 (low byte)  
    g_es5503->write(0x20, 0x08);    // freq hi = 0x08 (high byte) → 0x0800 total  
    g_es5503->write(0x40, 0x80);    // volume = 0x80 (medium)
    g_es5503->write(0x80, 0x00);    // wavetable pointer = 0x0000
    g_es5503->write(0xC0, 0x20);    // wavetable size = 4096 samples, resolution = 0
    g_es5503->write(0xA0, 0x00);    // control = 0x00 (start oscillator, channel 0)
    g_es5503->write(0xE1, 0x00);    // enable 1 oscillator
    
    Serial.println("Full audio pipeline test complete. You should hear ES5503 audio if FPGA I2S clocks are present.");
    Serial.println("Use 'lcam' to start bus capture if not already running.");
    Serial.println("Set capture mode 7 (ES5503 only) for optimal ES5503 bus capture.");
  } else if (cmd == "meminfo") {
    // Show memory allocation information
    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    size_t psram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t internal_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    
    Serial.printf("Memory Information:\n");
    Serial.printf("  PSRAM:    %d / %d bytes free (%.1f%% used)\n", 
                  (int)psram_free, (int)psram_total, 
                  100.0 * (psram_total - psram_free) / psram_total);
    Serial.printf("  Internal: %d / %d bytes free (%.1f%% used)\n", 
                  (int)internal_free, (int)internal_total,
                  100.0 * (internal_total - internal_free) / internal_total);
    
    if (g_es5503) {
      Serial.println("  ES5503 wave memory: 65536 bytes allocated");
    } else {
      Serial.println("  ES5503 not initialized");
    }
  } else if (cmd == "radiostatus") {
    // Show radio streaming status
    if (A2FPGARadio::isActive()) {
      Serial.printf("Radio active: YES\n");
      Serial.printf("Current URL: %s\n", A2FPGARadio::getCurrentURL().c_str());
      Serial.printf("Prebuffered: %s\n", A2FPGARadio::isPrebuffered() ? "YES" : "NO");
      Serial.printf("Grace cycles: %d\n", A2FPGARadio::getGracePeriodCycles());
      size_t available, total;
      A2FPGARadio::getBufferStatus(&available, &total);
      Serial.printf("Buffer: %d/%d samples\n", available, total);
      Serial.printf("I2S task running: %s\n", s_i2s_run ? "YES" : "NO");
    } else {
      Serial.println("Radio not active");
    }
  } else if (cmd.startsWith("radio ")) {
    // Stream internet radio using radio module
    // Format: radio <url> or radio stop
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (nt < 2) {
      Serial.println("Usage: radio <url>  or  radio stop");
      Serial.println("Example URLs:");
      Serial.println("  http://ice1.somafm.com/defcon-128-mp3  (SomaFM DEF CON)");
      Serial.println("  http://ice1.somafm.com/groovesalad-128-mp3  (SomaFM Groove Salad)");
      Serial.println("  http://stream.radioparadise.com/mp3-32  (Radio Paradise MP3)");
    } else if (toks[1] == "stop") {
      if (A2FPGARadio::isActive()) {
        Serial.println("Stopping radio stream...");
        A2FPGARadio::stop();
        Serial.println("Radio stopped");
      } else {
        Serial.println("Radio not playing");
      }
    } else {
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error: WiFi not connected. Use 'wifi' command first.");
        return;
      }
      
      String url = toks[1];
      Serial.printf("Starting radio stream: %s\n", url.c_str());
      
      // Stop other audio sources
      s_i2s_test_mode = false;
      if (g_es5503) {
        for (int osc = 0; osc < 32; osc++) {
          uint8_t ctrl = g_es5503->read(0xA0 + osc);
          g_es5503->write(0xA0 + osc, ctrl | 0x01);  // Halt oscillator
        }
      }
      
      // Initialize radio module if not already done
      A2FPGARadio::begin();
      
      // Start I2S to consume radio PCM data
      esp_err_t err = i2s_tx_start();
      if (err != ESP_OK) {
        Serial.printf("Failed to start I2S for radio: %s\n", esp_err_to_name(err));
        return;
      }
      
      // Start radio streaming
      if (A2FPGARadio::start(url)) {
        Serial.println("Radio stream started!");
        Serial.println("Use 'radio stop' to stop streaming");
        Serial.println("Use 'radiostatus' to check buffer status");
      } else {
        Serial.println("Failed to start radio stream");
      }
    }
  } else if (cmd.startsWith("wifi ")) {
    // Connect to WiFi network
    // Format: wifi <ssid> <password>
    String toks[16]; int nt = split_ws(cmd, toks, 16);
    if (nt < 3) {
      Serial.println("Usage: wifi <ssid> <password>");
    } else {
      String ssid = toks[1];
      String password = toks[2];
      
      Serial.printf("Connecting to WiFi network: %s\n", ssid.c_str());
      
      // Disconnect if already connected
      if (WiFi.status() == WL_CONNECTED) {
        WiFi.disconnect();
        delay(100);
      }
      
      WiFi.begin(ssid.c_str(), password.c_str());
      
      // Wait up to 30 seconds for connection
      int timeout = 60; // 30 seconds (500ms * 60)
      Serial.print("Connecting");
      while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(500);
        Serial.print(".");
        timeout--;
      }
      Serial.println();
      
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("WiFi connected successfully!\n");
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());
        Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
      } else {
        Serial.println("WiFi connection failed or timed out");
        Serial.printf("Status: %d\n", WiFi.status());
      }
    }
  } else if (cmd == "exit") {
    cli_mode = false;
    lcam_set_logging(0);
    Serial.println("Exiting CLI mode. Returning to serial forwarding mode.");
    Serial.println("Use '+++' to enter CLI mode again.");
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | spitest | spireg | spir | spiw | i2sstart | i2sstop | i2stest | i2sstatus | es5503start | es5503stop | es5503wave | audiostop | audiostart | es5503test | es5503reg | es5503debug | es5503info | es5503mem | fulltest | meminfo | wifi | radio | exit | we N");
    Serial.println("  spireg <reg> [val]        - read/write 1-byte register (0..126)");
    Serial.println("  spir <space> <addr> <len> [inc=1] - read bytes");
    Serial.println("  spiw <space> <addr> <inc|len> <b0> [b1 ...] - write bytes");
    Serial.println("  i2sstart                  - start I2S slave-TX (ext BCLK/WS)");
    Serial.println("  i2sstop                   - stop I2S output");
    Serial.println("  i2stest                   - send test pattern L=0xCAFE R=0xBABE");
    Serial.println("  i2sstatus                 - show I2S status and pin configuration");
    Serial.println("  es5503start               - initialize and start ES5503 audio generation");
    Serial.println("  es5503stop                - stop ES5503 audio generation");
    Serial.println("  es5503wave                - load test sawtooth waveform into wave memory");
    Serial.println("  audiostop                 - immediately silence all ES5503 audio (disable all oscillators)");
    Serial.println("  audiostart <num>          - enable ES5503 audio with specified number of oscillators (1-32)");
    Serial.println("  es5503test                - write test registers to ES5503");
    Serial.println("  es5503reg <reg> [val]     - read/write ES5503 register");
    Serial.println("  es5503debug               - toggle ES5503 register write debugging");
    Serial.println("  es5503info                - display oscillator status (frequencies, volumes, etc.)");
    Serial.println("  es5503mem <addr> [len]    - examine ES5503 wave memory (hex dump)");
    Serial.println("  fulltest                  - complete ES5503 audio pipeline test");
    Serial.println("  meminfo                   - show PSRAM and internal memory usage");
    Serial.println("  wifi <ssid> <password>    - connect to WiFi network");
    Serial.println("  radio <url>               - stream internet radio (MP3/AAC)");
    Serial.println("  radio stop                - stop radio streaming");
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
  Serial.printf("A2FPGA ESP32-S3 Firmware (%s %s) with ES5503 Audio\n", __DATE__, __TIME__);

  Serial.println("Serial forwarding mode active. Use '+++' to enter CLI mode.");
  Serial.println("ES5503 Ensoniq DOC emulation integrated.");
  
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
  
  // Initialize tone generator for audio debugging
  A2FPGATone::begin();
  
  // Note: ES5503 will be initialized when first used via CLI commands
  // or when bus packets are received that require ES5503 processing
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
