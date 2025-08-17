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
static const int PIN_SCLK = 18;
static const int PIN_MOSI = 9;
static const int PIN_MISO = 10;
static const int SPI_HZ   = 20 * 1000 * 1000;  // start at 20 MHz; tune as needed

bool usb_was_connected = false;

// ---------- CLI Escape Sequence ----------
const char* CLI_ESCAPE_SEQUENCE = "+++";  // Escape sequence to enter CLI mode
const int ESCAPE_TIMEOUT_MS = 1000;       // Timeout for escape sequence detection
bool cli_mode = false;                     // Current mode: false=forwarding, true=CLI
String escape_buffer = "";                 // Buffer for escape sequence detection
unsigned long last_char_time = 0;          // Time of last character for timeout

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
  } else if (cmd == "exit") {
    cli_mode = false;
    lcam_set_logging(0);
    Serial.println("Exiting CLI mode. Returning to serial forwarding mode.");
    Serial.println("Use '+++' to enter CLI mode again.");
  } else if (cmd == "help") {
    Serial.println("Commands: lcam | stop | status | exit | we N");
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
  Serial.println("A2FPGA ESP32-S3 Firmware");
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