/*
 * ESP32-S3 QSPI Slave Mode for 4-bit Data Capture
 * Using dedicated SPI3 peripheral in slave mode with GDMA
 */

#include <Arduino.h>
#include "driver/spi_slave.h"
#include "driver/spi_slave_hd.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "soc/usb_serial_jtag_reg.h"  // For USB-JTAG bridge
#include "soc/gpio_sig_map.h"         // For GPIO signal routing

// 4-bit Parallel Data Interface via QSPI Slave
#define PIN_QSPI_CLK   13    // SCLK - Clock from FPGA
#define PIN_QSPI_CS    12    // CS - Frame signal from FPGA (active low)
#define PIN_QSPI_D0    14    // SIO0 - Data bit 0 (MOSI equivalent)
#define PIN_QSPI_D1    15    // SIO1 - Data bit 1 (MISO equivalent)  
#define PIN_QSPI_D2    16    // SIO2 - Data bit 2
#define PIN_QSPI_D3    17    // SIO3 - Data bit 3

// JTAG Interface to FPGA (for USB programming)
#define PIN_TCK        40    // JTAG Clock
#define PIN_TMS        41    // JTAG Mode Select
#define PIN_TDI        42    // JTAG Data In
#define PIN_TDO        45    // JTAG Data Out  
#define PIN_SRST       7     // JTAG Reset (unused but required)
#define PIN_FPGA_DONE  48    // Configuration done signal

#define PIN_FPGA_0      9

// LEDs and other pins
#define PIN_LED0       1     // Status LED 0
#define PIN_LED1       2     // Status LED 1
#define PIN_RXD        44    // UART RX from FPGA
#define PIN_TXD        43    // UART TX to FPGA
#define UART_BAUD      115200

// SPI Configuration
#define SPI_HOST_ID    SPI3_HOST
#define DMA_BUFFER_SIZE 512
#define DMA_NUM_BUFFERS 2

// Data processing
volatile int packets_received = 0;
volatile int packets_processed = 0;

// QSPI Slave variables
static uint8_t *qspi_rx_buffer;
static uint8_t *qspi_tx_buffer;  // Not used but required by SPI slave
static spi_slave_transaction_t qspi_trans;
static volatile bool transaction_complete = false;

// USB-JTAG variables
static bool usb_was_connected = false;
static bool jtag_enabled = false;

// QSPI slave callbacks  
static void qspi_post_setup_cb(spi_slave_transaction_t *trans) {
    // Transaction setup complete
}
static void qspi_post_trans_cb(spi_slave_transaction_t *trans) {
    // Transaction completed
    transaction_complete = true;
}

// Setup QSPI slave with QUAD flag for 4-bit parallel data capture
static esp_err_t setup_qspi_slave() {
    Serial.println("Setting up QSPI slave with QUAD flag...");
    
    // Allocate DMA-capable buffers
    qspi_rx_buffer = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
    qspi_tx_buffer = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
    
    if (!qspi_rx_buffer || !qspi_tx_buffer) {
        Serial.println("Failed to allocate QSPI buffers");
        return ESP_ERR_NO_MEM;
    }
    
    // Clear buffers
    memset(qspi_rx_buffer, 0, DMA_BUFFER_SIZE);
    memset(qspi_tx_buffer, 0, DMA_BUFFER_SIZE);
    
    // Configure SPI bus for QSPI with QUAD flag
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_QSPI_D0,      // SIO0 - Data 0
        .miso_io_num = PIN_QSPI_D1,      // SIO1 - Data 1  
        .sclk_io_num = PIN_QSPI_CLK,     // SCLK - Clock from FPGA
        .quadwp_io_num = PIN_QSPI_D2,    // SIO2 - Data 2
        .quadhd_io_num = PIN_QSPI_D3,    // SIO3 - Data 3
        .max_transfer_sz = DMA_BUFFER_SIZE,
        .flags = SPICOMMON_BUSFLAG_SLAVE | SPICOMMON_BUSFLAG_GPIO_PINS | SPICOMMON_BUSFLAG_QUAD
    };
    
    spi_slave_interface_config_t slave_cfg = {
        .spics_io_num = PIN_QSPI_CS,     // CS - Frame signal (active low)
        .flags = 0,                      // Standard flags
        .queue_size = 3,                 // Larger queue for better handling
        .mode = 0,                       // SPI Mode 0 (CPOL=0, CPHA=0)
        .post_setup_cb = qspi_post_setup_cb,
        .post_trans_cb = qspi_post_trans_cb
    };
    
    // Initialize SPI slave
    esp_err_t ret = spi_slave_initialize(SPI_HOST_ID, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Serial.printf("QSPI slave init failed: %x\n", ret);
        return ret;
    }
    
    Serial.println("QSPI slave with QUAD flag setup complete");
    Serial.printf("Pin mapping - CLK:%d, CS:%d, D0:%d, D1:%d, D2:%d, D3:%d\n",
                  PIN_QSPI_CLK, PIN_QSPI_CS, PIN_QSPI_D0, PIN_QSPI_D1, PIN_QSPI_D2, PIN_QSPI_D3);
    
    return ESP_OK;
}

// USB-JTAG Bridge Functions
void route_usb_jtag_to_gpio() {
    Serial.println("Enabling USB-JTAG bridge for FPGA programming...");
    
    pinMode(PIN_TCK, OUTPUT);
    pinMode(PIN_TMS, OUTPUT);
    pinMode(PIN_TDI, OUTPUT);
    pinMode(PIN_TDO, INPUT);
    pinMode(PIN_SRST, OUTPUT);
    
    // Enable USB-JTAG bridge
    WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
        READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) | USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
    
    // Route JTAG signals to GPIO pins
    esp_rom_gpio_connect_out_signal(PIN_TCK,  USB_JTAG_TCK_IDX,  false, false);
    esp_rom_gpio_connect_out_signal(PIN_TMS,  USB_JTAG_TMS_IDX,  false, false);
    esp_rom_gpio_connect_out_signal(PIN_TDI,  USB_JTAG_TDI_IDX,  false, false);
    esp_rom_gpio_connect_out_signal(PIN_SRST, USB_JTAG_TRST_IDX, false, false);
    esp_rom_gpio_connect_in_signal(PIN_TDO,   USB_JTAG_TDO_BRIDGE_IDX, false);
    
    jtag_enabled = true;
}

void unroute_usb_jtag_to_gpio() {
    Serial.println("Disabling USB-JTAG bridge, returning to normal operation...");
    
    // Disable USB-JTAG bridge
    WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
        READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) & ~USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);
    
    // Set JTAG pins to input (high impedance)
    pinMode(PIN_TCK, INPUT);
    pinMode(PIN_TMS, INPUT);
    pinMode(PIN_TDI, INPUT);
    pinMode(PIN_TDO, INPUT);
    pinMode(PIN_SRST, INPUT);
    
    jtag_enabled = false;
    
    // Restart QSPI after JTAG session (pins may have been affected)
    Serial.println("Restarting QSPI slave after JTAG session...");
    spi_slave_free(SPI_HOST_ID);
    delay(100);
    setup_qspi_slave();
    start_qspi_transaction();
}

// Start QSPI transaction to capture data
esp_err_t start_qspi_transaction() {
    // Clear transaction complete flag
    transaction_complete = false;
    
    // Setup transaction for 32-bit packet (8 nibbles = 32 bits)
    memset(&qspi_trans, 0, sizeof(qspi_trans));
    qspi_trans.length = 32;              // 32 bits = 8 nibbles = 4 bytes
    qspi_trans.rx_buffer = qspi_rx_buffer;
    qspi_trans.tx_buffer = qspi_tx_buffer;
    
    Serial.printf("QSPI transaction configured for %d bits (%d bytes)\n", 
                  qspi_trans.length, qspi_trans.length / 8);
    
    // Queue the transaction
    esp_err_t ret = spi_slave_queue_trans(SPI_HOST_ID, &qspi_trans, portMAX_DELAY);
    if (ret != ESP_OK) {
        Serial.printf("QSPI transaction queue failed: %x\n", ret);
        return ret;
    }
    
    Serial.println("QSPI transaction queued, waiting for FPGA data...");
    return ESP_OK;
}

// Check for completed QSPI transactions
void check_qspi_data() {
    if (transaction_complete) {
        transaction_complete = false;
        packets_received++;
        
        // Get the completed transaction
        spi_slave_transaction_t *trans_out;
        esp_err_t ret = spi_slave_get_trans_result(SPI_HOST_ID, &trans_out, 0);
        
        if (ret == ESP_OK) {
            Serial.printf("QSPI Transaction Complete - %d bits received (%d bytes), Buffer: ", 
                         trans_out->trans_len, trans_out->trans_len / 8);
            
            // Show first 8 bytes  
            for (int i = 0; i < 8 && i < DMA_BUFFER_SIZE; i++) {
                Serial.printf("%02X ", qspi_rx_buffer[i]);
            }
            Serial.println();
            
            // Debug: show current CS and CLK states
            Serial.printf("  Current GPIO - CS:%d, CLK:%d, Data:0x%X\n", 
                         digitalRead(PIN_QSPI_CS), digitalRead(PIN_QSPI_CLK),
                         (digitalRead(PIN_QSPI_D3)<<3) | (digitalRead(PIN_QSPI_D2)<<2) |
                         (digitalRead(PIN_QSPI_D1)<<1) | digitalRead(PIN_QSPI_D0));
            
            // Process the 4-bit parallel data
            process_4bit_data(qspi_rx_buffer, trans_out->trans_len / 8);
            packets_processed++;
            
            // Start next transaction for continuous capture
            start_qspi_transaction();
        }
    }
}

// Process 4-bit parallel data from QSPI buffer
void process_4bit_data(uint8_t* buffer, size_t length) {
    if (length >= 4) {
        // Reconstruct the 32-bit packet from 4 bytes
        uint32_t packet = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
        
        Serial.printf("32-bit packet: 0x%08X = [%02X][%02X][%02X][%02X]\n", 
                     packet, buffer[0], buffer[1], buffer[2], buffer[3]);
        
        // Extract expected pattern: [counter][00][01][02]
        uint8_t counter = buffer[0];
        uint8_t byte1 = buffer[1]; 
        uint8_t byte2 = buffer[2];
        uint8_t byte3 = buffer[3];
        
        Serial.printf("  Counter: 0x%02X, Pattern: %02X-%02X-%02X ", counter, byte1, byte2, byte3);
        
        if (byte1 == 0x00 && byte2 == 0x01 && byte3 == 0x02) {
            Serial.println("✓ CORRECT PATTERN");
        } else {
            Serial.println("✗ Pattern mismatch");
        }
    } else {
        Serial.printf("Processing %d bytes (too short): ", length);
        for (size_t i = 0; i < length; i++) {
            Serial.printf("%02X ", buffer[i]);
        }
        Serial.println();
    }
}

void process_command(String cmd) {
    cmd.toLowerCase();
    
    if (cmd == "start") {
        start_qspi_transaction();
    }
    else if (cmd == "stop") {
        Serial.println("Stopping QSPI capture...");
        // Note: SPI slave transactions auto-complete, no explicit stop needed
        transaction_complete = false;
        Serial.println("QSPI capture stopped");
    }
    else if (cmd == "test") {
        Serial.println("Testing QSPI slave capture...");
        
        // Start capture
        start_qspi_transaction();
        
        // Check for data for 5 seconds
        Serial.println("Monitoring QSPI for 5 seconds...");
        unsigned long start_time = millis();
        int initial_packets = packets_received;
        
        while (millis() - start_time < 5000) {
            check_qspi_data();
            delay(50);
        }
        
        int packets_captured = packets_received - initial_packets;
        Serial.printf("Test result: %d QSPI transactions completed in 5 seconds\n", packets_captured);
        
        if (packets_captured > 0) {
            Serial.println("SUCCESS: QSPI capturing data!");
        } else {
            Serial.println("FAIL: No QSPI transactions completed");
            Serial.println("Check FPGA is sending clock and CS signals");
            Serial.println("CS should go LOW to start transaction");
        }
    }
    else if (cmd == "check") {
        Serial.println("Single QSPI check...");
        check_qspi_data();
    }
    else if (cmd == "status") {
        Serial.printf("QSPI Slave Status:\n");
        Serial.printf("  Packets received: %d\n", packets_received);
        Serial.printf("  Packets processed: %d\n", packets_processed);
        Serial.printf("  Transaction complete flag: %s\n", transaction_complete ? "true" : "false");
        
        // Check for queued transactions
        Serial.printf("  Expected transaction length: 32 bits (4 bytes)\n");
        Serial.printf("  Success rate: %.1f%%\n", packets_received > 0 ? (float)packets_processed * 100 / packets_received : 0);
        
        // Read current GPIO states for comparison
        Serial.printf("Current GPIO states:\n");
        Serial.printf("  Data: %d%d%d%d (0x%X)\n", 
                     digitalRead(PIN_QSPI_D3), digitalRead(PIN_QSPI_D2),
                     digitalRead(PIN_QSPI_D1), digitalRead(PIN_QSPI_D0),
                     (digitalRead(PIN_QSPI_D3)<<3) | (digitalRead(PIN_QSPI_D2)<<2) |
                     (digitalRead(PIN_QSPI_D1)<<1) | digitalRead(PIN_QSPI_D0));
        Serial.printf("  Clock: %d, CS: %d\n", digitalRead(PIN_QSPI_CLK), digitalRead(PIN_QSPI_CS));
        
        Serial.println("Run 'test' to see current transaction format");
    }
    else if (cmd == "bitbang") {
        Serial.println("Bit-banged manual capture (waiting for FPGA transaction)...");
        
        // Temporarily disable SPI slave to avoid conflicts
        Serial.println("Temporarily disabling QSPI slave...");
        spi_slave_free(SPI_HOST_ID);
        
        // Configure pins as regular GPIO inputs
        pinMode(PIN_QSPI_CS, INPUT);
        pinMode(PIN_QSPI_CLK, INPUT);
        pinMode(PIN_QSPI_D0, INPUT);
        pinMode(PIN_QSPI_D1, INPUT);
        pinMode(PIN_QSPI_D2, INPUT);
        pinMode(PIN_QSPI_D3, INPUT);
        
        delay(100);  // Let pins settle
        
        // Show current pin states first
        Serial.printf("Current pin states: CS=%d, CLK=%d, Data=0x%X\n",
                     digitalRead(PIN_QSPI_CS), digitalRead(PIN_QSPI_CLK),
                     (digitalRead(PIN_QSPI_D3)<<3) | (digitalRead(PIN_QSPI_D2)<<2) |
                     (digitalRead(PIN_QSPI_D1)<<1) | digitalRead(PIN_QSPI_D0));
        
        // Test continuous clock detection - should see toggles even when CS=HIGH
        Serial.println("Testing clock detection (should see continuous toggles)...");
        bool last_clk_test = digitalRead(PIN_QSPI_CLK);
        int toggle_count = 0;
        unsigned long test_start = millis();
        while (millis() - test_start < 100 && toggle_count < 10) {  // 100ms test
            bool current_clk_test = digitalRead(PIN_QSPI_CLK);
            if (current_clk_test != last_clk_test) {
                toggle_count++;
                Serial.printf("Clock toggle %d: %d->%d at %dms\n", 
                             toggle_count, last_clk_test, current_clk_test, 
                             (int)(millis() - test_start));
                last_clk_test = current_clk_test;
            }
            delayMicroseconds(10);
        }
        Serial.printf("Clock test complete: %d toggles in 100ms\n", toggle_count);
        if (toggle_count == 0) {
            Serial.println("ERROR: No clock toggles detected! Check FPGA or pin connections.");
            return;
        }
        
        // Wait for CS to go LOW (start of transaction)
        Serial.println("Waiting for CS LOW...");
        unsigned long timeout = millis() + 5000;  // 5 second timeout
        int wait_count = 0;
        while (digitalRead(PIN_QSPI_CS) == HIGH && millis() < timeout) {
            delayMicroseconds(10);
            wait_count++;
            
            // Show progress every 100ms
            if (wait_count % 10000 == 0) {
                Serial.printf("Still waiting... CS=%d (waited %d ms)\n", 
                             digitalRead(PIN_QSPI_CS), (wait_count * 10) / 1000);
            }
        }
        
        if (digitalRead(PIN_QSPI_CS) == HIGH) {
            Serial.println("TIMEOUT: No CS LOW detected in 5 seconds");
            return;
        }
        
        // Start capturing immediately - no printing delays!
        int clock_edges = 0;
        bool last_clk = digitalRead(PIN_QSPI_CLK);
        uint8_t captured_data[16];  // Store up to 16 nibbles
        int loop_count = 0;
        
        unsigned long start_time = millis();
        unsigned long timeout_duration = 100;  // 100ms max transaction time  
        bool timed_out = false;
        
        while (clock_edges < 8 && !timed_out) {  // Capture 8 clock edges for 8 nibbles
            loop_count++;
            bool current_clk = digitalRead(PIN_QSPI_CLK);
            
            // Only capture data when CS is still LOW (transaction active)
            bool current_cs = digitalRead(PIN_QSPI_CS);
            if (current_cs == LOW && current_clk && !last_clk) {  // Rising edge during active transaction
                // Read 4-bit data
                uint8_t data_nibble = 0;
                data_nibble |= digitalRead(PIN_QSPI_D0) << 0;
                data_nibble |= digitalRead(PIN_QSPI_D1) << 1;
                data_nibble |= digitalRead(PIN_QSPI_D2) << 2;
                data_nibble |= digitalRead(PIN_QSPI_D3) << 3;
                
                captured_data[clock_edges] = data_nibble;
                
                // Don't print during capture - too slow!
                
                clock_edges++;
            }
            
            last_clk = current_clk;
            
            // Check timeout only occasionally to avoid constant millis() calls
            if (loop_count % 1000 == 0) {
                timed_out = (millis() - start_time > timeout_duration);
            }
        }
        
        // Report why the loop exited
        if (clock_edges >= 8) {
            Serial.println("Loop exit reason: Captured 8 clock edges (SUCCESS)");
        } else if (timed_out) {
            Serial.println("Loop exit reason: TIMEOUT - no clock edges detected");
        } else {
            Serial.println("Loop exit reason: UNKNOWN");
        }
        
        Serial.printf("Transaction complete: %d clock edges captured\n", clock_edges);
        Serial.printf("Final CS state: %d, Final CLK state: %d\n", 
                     digitalRead(PIN_QSPI_CS), digitalRead(PIN_QSPI_CLK));
        Serial.printf("Total loop iterations: %d\n", loop_count);
        
        // Print captured data
        if (clock_edges > 0) {
            Serial.print("Captured data during CS LOW: ");
            for (int i = 0; i < clock_edges; i++) {
                Serial.printf("0x%X ", captured_data[i]);
            }
            Serial.println();
        }
        
        // Reconstruct bytes from nibbles
        if (clock_edges >= 8) {
            Serial.println("Reconstructed data:");
            for (int i = 0; i < clock_edges; i += 2) {
                if (i + 1 < clock_edges) {
                    uint8_t byte_val = (captured_data[i] << 4) | captured_data[i + 1];
                    Serial.printf("  Byte %d: 0x%02X (nibbles 0x%X, 0x%X)\n", 
                                 i/2, byte_val, captured_data[i], captured_data[i + 1]);
                }
            }
        }
        
        // Show raw nibble sequence
        Serial.print("Raw nibble sequence: ");
        for (int i = 0; i < clock_edges; i++) {
            Serial.printf("%X", captured_data[i]);
        }
        Serial.println();
        
        // Restore QSPI slave
        Serial.println("Restoring QSPI slave...");
        setup_qspi_slave();
        start_qspi_transaction();
    }
    else if (cmd == "reset") {
        packets_received = 0;
        packets_processed = 0;
        transaction_complete = false;
        Serial.println("Statistics reset");
    }
    else if (cmd == "help") {
        Serial.println("QSPI Slave Parallel Capture Commands:");
        Serial.println("  start   - Start QSPI transaction");
        Serial.println("  stop    - Stop QSPI capture"); 
        Serial.println("  test    - Test capture for 5 seconds");
        Serial.println("  check   - Single QSPI check");
        Serial.println("  bitbang - Manual bit-banged capture (debug)");
        Serial.println("  status  - Show status and GPIO states");
        Serial.println("  reset   - Reset statistics");
        Serial.println("  help    - Show this help");
    }
    else {
        Serial.printf("Unknown command: %s\n", cmd.c_str());
    }
}

void setup() {
    // Initialize pins
    pinMode(PIN_FPGA_DONE, INPUT_PULLUP);
    pinMode(PIN_LED0, OUTPUT);
    pinMode(PIN_FPGA_0, INPUT_PULLUP);
    pinMode(PIN_LED1, OUTPUT);
    digitalWrite(PIN_LED0, LOW);
    digitalWrite(PIN_LED1, LOW);
    
    // Initialize serial communications
    Serial.begin(115200);           // USB serial for commands
    Serial1.begin(UART_BAUD, SERIAL_8N1, PIN_RXD, PIN_TXD);  // UART to FPGA
    delay(1000);  // Allow serial to stabilize
    
    Serial.println("ESP32-S3 QSPI Slave + USB-JTAG Bridge");
    Serial.println("Using SPI3 peripheral in quad slave mode with GDMA");
    Serial.println("USB-JTAG bridge automatically enables when programmer connects");
    
    if (setup_qspi_slave() == ESP_OK) {
        Serial.println("QSPI slave setup complete - type 'help' for commands");
        Serial.println("Ready to capture 4-bit parallel data from FPGA");
        Serial.printf("Pin mapping - CLK:%d, CS:%d, D0:%d, D1:%d, D2:%d, D3:%d\n",
                     PIN_QSPI_CLK, PIN_QSPI_CS, PIN_QSPI_D0, PIN_QSPI_D1, PIN_QSPI_D2, PIN_QSPI_D3);
        Serial.printf("JTAG pins - TCK:%d, TMS:%d, TDI:%d, TDO:%d\n",
                     PIN_TCK, PIN_TMS, PIN_TDI, PIN_TDO);
        Serial.println("FPGA QSPI Protocol:");
        Serial.println("  - CS starts HIGH, goes LOW for transaction");  
        Serial.println("  - SCLK toggles during transaction");
        Serial.println("  - 32-bit packets sent as 8 nibbles on D0-D3");
        Serial.println("  - Packet format: [counter][00][01][02] for testing");
    } else {
        Serial.println("QSPI slave setup failed!");
    }
}

void loop() {
    // Handle USB-JTAG bridge detection
    bool usb_is_connected = usb_serial_jtag_is_connected();
    if (usb_was_connected == false && usb_is_connected == true) {
        route_usb_jtag_to_gpio();
    }
    if (usb_was_connected == true && usb_is_connected == false) {
        unroute_usb_jtag_to_gpio();
    }
    usb_was_connected = usb_is_connected;
    
    // UART bridge between USB serial and FPGA serial
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            // Check if it's a command or should be forwarded to FPGA
            if (input.startsWith("start") || input.startsWith("stop") || 
                input.startsWith("test") || input.startsWith("check") || 
                input.startsWith("bitbang") || input.startsWith("status") || 
                input.startsWith("reset") || input.startsWith("help")) {
                // ESP32 command
                process_command(input);
            } else {
                // Forward to FPGA UART
                Serial1.println(input);
            }
        }
    }
    
    // Forward FPGA UART output to USB serial
    if (Serial1.available()) {
        Serial.write(Serial1.read());
    }
    
    // Check for QSPI transactions (only when JTAG not active)
    if (!jtag_enabled) {
        check_qspi_data();
    }
    
    // Update status LEDs
    digitalWrite(PIN_LED0, digitalRead(PIN_FPGA_DONE));  // LED0 shows FPGA configured
    // LED1 controlled by JTAG status (set in route/unroute functions)
    digitalWrite(PIN_LED1, digitalRead(PIN_FPGA_0)); 
    
    // Periodic status update
    static unsigned long last_status = 0;
    if (millis() - last_status > 15000) {  // Every 15 seconds
        if (packets_received > 0 && !jtag_enabled) {
            Serial.printf("QSPI Activity: %d transactions completed, %d processed\n", 
                         packets_received, packets_processed);
        }
        last_status = millis();
    }
    
    delay(5);  // Short delay for responsive operation
}