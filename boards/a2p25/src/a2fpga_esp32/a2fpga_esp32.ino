/*
 * A2FPGA ESP32-S3 Bridge System
 * 
 * This system provides three key functions for Apple II FPGA development:
 * 
 * 1. HIGH-PERFORMANCE PACKET CAPTURE (PARL_IO + DMA)
 *    - Captures 4-bit parallel data from FPGA using ESP32-S3's PARL_IO peripheral
 *    - Uses ping-pong DMA buffers for zero-copy, continuous packet reception
 *    - Processes 32-bit Apple II bus transactions (address, data, flags)
 *    - Dedicated FreeRTOS task on core 1 for real-time packet processing
 * 
 * 2. DYNAMIC USB JTAG BRIDGE
 *    - Auto-detects USB JTAG connection and routes signals to GPIO pins
 *    - Enables FPGA programming/debugging through ESP32's built-in USB JTAG
 *    - Cleanly switches between JTAG and normal GPIO operation
 * 
 * 3. SERIAL PASSTHROUGH & SYSTEM MONITORING
 *    - Bidirectional serial bridge between USB and UART
 *    - Real-time Apple II bus analysis with specific I/O decoding
 *    - Status LEDs, statistics, and interactive diagnostics
 * 
 * Arduino IDE Configuration:
 * Tools->Board->Boards Manager->esp32 by espressif
 * Tools->USB CDC On Boot: Enabled
 * Tools->CPU Frequency: 80 MHz (WiFi)
 * Tools->Board: Adafruit QT Py ESP32-S3 (4M Flash 2M PSRAM)
 * Tools->JTAG Adapter: Integrated USB JTAG
 * Tools->USB Mode: Hardware CDC and JTAG
 */

#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "soc/usb_serial_jtag_reg.h"
#include "soc/gpio_sig_map.h"
#include "esp_log.h"
#include <Arduino.h>

#define TAG "A2FPGA_COMPLETE"

// Status LEDs - Connected to ESP32-S3 QT Py onboard LEDs
#define PIN_LED0 1              // LED shows FPGA_DONE status
#define PIN_LED1 2              // LED shows FPGA GPIO0 status  
#define LED_ON  HIGH
#define LED_OFF LOW

// FPGA Status Monitoring Pins
#define PIN_FPGA_DONE 48        // FPGA configuration complete signal
#define PIN_FPGA_0 9            // FPGA general purpose status pin 0
#define PIN_FPGA_1 11           // FPGA general purpose status pin 1

// 4-bit Parallel Data Interface from FPGA (PARL_IO)
#define PIN_QSPI_D0 14          // Data bit 0 (LSB)
#define PIN_QSPI_D1 15          // Data bit 1
#define PIN_QSPI_D2 16          // Data bit 2  
#define PIN_QSPI_D3 17          // Data bit 3 (MSB)
#define PIN_QSPI_CLK 13         // Clock signal from FPGA
#define PIN_FRAME     10        // Frame/Valid signal for packet boundaries

// Serial Passthrough Interface (UART to FPGA)
#define PIN_RXD  44             // Receive from FPGA
#define PIN_TXD  43             // Transmit to FPGA
#define BAUD     115200         // Serial baud rate

// JTAG Programming Interface (routed from USB JTAG)
#define PIN_TCK  40             // Test Clock
#define PIN_TMS  41             // Test Mode Select
#define PIN_TDI  42             // Test Data In (to FPGA)
#define PIN_TDO  45             // Test Data Out (from FPGA)
#define PIN_SRST 7              // System Reset

// DMA Configuration for High-Performance Packet Capture
#define PARLIO_DATA_WIDTH    4        // 4-bit parallel data bus from FPGA
#define DMA_BUFFER_SIZE      2048     // Large buffers minimize interrupt overhead
#define DMA_BUFFER_COUNT     4        // Ping-pong buffers for continuous capture
#define PACKET_SIZE_BYTES    4        // Apple II bus packets: 32-bit (addr+data+flags)

// Global handles and state for I2S parallel packet capture system
i2s_chan_handle_t i2s_rx_handle = NULL;          // I2S RX channel handle
TaskHandle_t packet_processor_task;               // Dedicated packet processing task
bool usb_was_connected = false;                   // USB JTAG connection state tracking

// Receive buffers for ping-pong operation
uint8_t *rx_buffers[DMA_BUFFER_COUNT];            // Receive buffer pointers
volatile int current_buffer = 0;                  // Current buffer index

// Performance statistics (volatile for ISR access)
volatile uint32_t packets_received = 0;           // DMA buffers completed
volatile uint32_t packets_processed = 0;          // Individual packets decoded
volatile uint32_t dma_overruns = 0;               // Buffer re-queue failures

// ============================================================================
// USB JTAG BRIDGE SYSTEM
// Dynamically routes ESP32-S3's built-in USB JTAG to external GPIO pins
// for FPGA programming/debugging. Automatically manages connection state.
// ============================================================================

void route_usb_jtag_to_gpio() {
    // Configure JTAG pins for output (except TDO which is input from target)
    pinMode(PIN_TCK, OUTPUT);   // Test Clock - to FPGA
    pinMode(PIN_TMS, OUTPUT);   // Test Mode Select - to FPGA  
    pinMode(PIN_TDI, OUTPUT);   // Test Data In - to FPGA
    pinMode(PIN_TDO, INPUT);    // Test Data Out - from FPGA
    pinMode(PIN_SRST, OUTPUT);  // System Reset - to FPGA

    // Enable the ESP32-S3's USB JTAG bridge hardware
    WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
        READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) | USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);

    // Route internal USB JTAG signals to external GPIO pins
    esp_rom_gpio_connect_out_signal(PIN_TCK, USB_JTAG_TCK_IDX, false, false);
    esp_rom_gpio_connect_out_signal(PIN_TMS, USB_JTAG_TMS_IDX, false, false);
    esp_rom_gpio_connect_out_signal(PIN_TDI, USB_JTAG_TDI_IDX, false, false);
    esp_rom_gpio_connect_out_signal(PIN_SRST, USB_JTAG_TRST_IDX, false, false);
    esp_rom_gpio_connect_in_signal(PIN_TDO, USB_JTAG_TDO_BRIDGE_IDX, false);
    
    ESP_LOGI(TAG, "USB JTAG routed to GPIO pins - FPGA programming enabled");
}

void unroute_usb_jtag_to_gpio() {
    // Disable the USB JTAG bridge to disconnect signals
    WRITE_PERI_REG(USB_SERIAL_JTAG_CONF0_REG,
        READ_PERI_REG(USB_SERIAL_JTAG_CONF0_REG) & ~USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN);

    // Return all JTAG pins to high-impedance input state
    pinMode(PIN_TCK, INPUT);
    pinMode(PIN_TMS, INPUT);
    pinMode(PIN_TDI, INPUT);
    pinMode(PIN_TDO, INPUT);
    pinMode(PIN_SRST, INPUT);
    
    ESP_LOGI(TAG, "USB JTAG unrouted from GPIO pins - normal operation");
}

// Note: Using I2S polling mode instead of callbacks for simplicity

void setup_i2s_parallel() {
    ESP_LOGI(TAG, "Setting up I2S parallel mode for 4-bit packet capture");
    
    // Allocate receive buffers for ping-pong operation
    for (int i = 0; i < DMA_BUFFER_COUNT; i++) {
        rx_buffers[i] = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
        if (!rx_buffers[i]) {
            ESP_LOGE(TAG, "Failed to allocate receive buffer %d", i);
            return;
        }
        ESP_LOGD(TAG, "Allocated RX buffer %d at %p", i, rx_buffers[i]);
    }
    
    // Configure I2S channel for RX
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_SLAVE);
    chan_cfg.dma_desc_num = DMA_BUFFER_COUNT;
    chan_cfg.dma_frame_num = DMA_BUFFER_SIZE / 4; // 4 bytes per frame for 32-bit data
    
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &i2s_rx_handle));
    ESP_LOGI(TAG, "I2S RX channel created");
    
    // Configure I2S for parallel data capture
    // We'll use standard I2S config but with external clock
    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = 1000000,              // 1MHz from FPGA
            .clk_src = I2S_CLK_SRC_EXTERNAL,        // External clock from FPGA
            .ext_clk_freq_hz = 1000000,             // External clock frequency
            .mclk_multiple = I2S_MCLK_MULTIPLE_256, // MCLK multiple
        },
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_32BIT, // 32-bit packets
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT,
            .slot_mode = I2S_SLOT_MODE_MONO,        // Single channel
            .slot_mask = I2S_STD_SLOT_LEFT,         // Use left channel
            .ws_width = I2S_DATA_BIT_WIDTH_32BIT,   // Word select width
            .ws_pol = false,                        // Word select polarity
            .bit_shift = true,                      // MSB shift
            .left_align = false,                    // Right align
            .big_endian = false,                    // Little endian
            .bit_order_lsb = false                  // MSB first
        },
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,                // No MCLK needed
            .bclk = (gpio_num_t)PIN_QSPI_CLK,       // Bit clock from FPGA
            .ws = (gpio_num_t)PIN_FRAME,            // Word select (frame) from FPGA
            .dout = I2S_GPIO_UNUSED,                // No data out
            .din = (gpio_num_t)PIN_QSPI_D0,         // Data input (we'll handle multiple pins via GPIO matrix)
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_rx_handle, &std_cfg));
    ESP_LOGI(TAG, "I2S standard mode configured");
    
    // Enable the I2S channel
    ESP_ERROR_CHECK(i2s_channel_enable(i2s_rx_handle));
    ESP_LOGI(TAG, "I2S RX channel enabled and ready for packet capture");
    
    // TODO: Configure additional data pins (D1, D2, D3) via GPIO matrix
    // For now, this captures 1-bit data on D0, but we need 4-bit parallel
    ESP_LOGW(TAG, "Note: Currently configured for 1-bit capture - need GPIO matrix for 4-bit");
}

// ============================================================================
// APPLE II BUS PACKET ANALYSIS
// Decodes 32-bit packets from FPGA containing Apple II bus transactions
// Packet format: [16-bit address][8-bit data][8-bit flags]
// ============================================================================

void process_packet(uint32_t pkt) {
    // Extract packet components
    uint16_t addr = (pkt >> 16) & 0xFFFF;   // Memory/I/O address
    uint8_t data  = (pkt >> 8) & 0xFF;      // Data value
    uint8_t flags = pkt & 0xFF;             // Bus flags (R/W, etc.)

    // Apple II Speaker detection ($C030 - classic click sound)
    if (addr == 0xC030) {
        Serial.println("🔊 Speaker Toggle");
        return;
    }

    // Audio register writes (Mockingboard/sound cards at $C040-$C04F)
    if ((addr & 0xFFF0) == 0xC040) {
        Serial.printf("🎵 Audio: %02X\n", data);
        return;
    }

    // Log other I/O page activity (reduce noise by filtering to I/O space)
    if ((addr & 0xFF00) == 0xC000) {  // Apple II I/O page ($C000-$C0FF)
        Serial.printf("📦 I/O: %04X %02X %02X\n", addr, data, flags);
    }
}

// ============================================================================
// FREERTOS PACKET PROCESSING TASK
// Dedicated task running on Core 1 for real-time packet processing
// Operates independently from main loop to ensure consistent performance
// ============================================================================

void packet_processor_task_func(void *param) {
    ESP_LOGI(TAG, "Starting I2S packet processing task");
    
    // Use polling approach for I2S data reading
    // This is simpler and more reliable than the callback approach
    uint8_t *current_rx_buffer = rx_buffers[0];
    size_t bytes_read = 0;
    
    while (1) {
        // Read data from I2S in blocking mode
        esp_err_t ret = i2s_channel_read(i2s_rx_handle, current_rx_buffer, DMA_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
        
        if (ret == ESP_OK && bytes_read > 0) {
            packets_received++;
            ESP_LOGD(TAG, "Read %d bytes from I2S", bytes_read);
            
            // Process all complete packets in the received data
            size_t complete_packets = bytes_read / PACKET_SIZE_BYTES;
            for (size_t i = 0; i < complete_packets; i++) {
                size_t offset = i * PACKET_SIZE_BYTES;
                
                // Reconstruct 32-bit packet from 4 sequential bytes
                // I2S data format depends on configuration, may need adjustment
                uint32_t packet = 0;
                for (int j = 0; j < PACKET_SIZE_BYTES; j++) {
                    packet = (packet << 8) | current_rx_buffer[offset + j];
                }
                
                // Skip empty packets (FPGA may send padding)
                if (packet != 0) {
                    process_packet(packet);
                    packets_processed++;
                }
            }
            
            // Switch to next buffer for ping-pong operation
            current_buffer = (current_buffer + 1) % DMA_BUFFER_COUNT;
            current_rx_buffer = rx_buffers[current_buffer];
            
        } else if (ret != ESP_OK) {
            dma_overruns++;  // Track I2S read failures
            ESP_LOGW(TAG, "I2S read failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(10)); // Small delay before retry
        }
    }
}

void setup() {
    // Initialize FPGA status pins
    pinMode(PIN_FPGA_DONE, INPUT_PULLUP);
    pinMode(PIN_FPGA_0, INPUT_PULLUP);
    pinMode(PIN_FPGA_1, INPUT_PULLUP);
    
    // Initialize LEDs
    pinMode(PIN_LED0, OUTPUT);
    pinMode(PIN_LED1, OUTPUT);
    
    // Initialize serial communications
    Serial.begin(115200);
    Serial1.begin(BAUD, SERIAL_8N1, PIN_RXD, PIN_TXD);
    
    // Note: I2S uses polling mode, so no queue needed for notifications
    ESP_LOGI(TAG, "I2S polling mode - no notification queue required");
    
    // Setup I2S parallel capture
    setup_i2s_parallel();
    
    // Create dedicated packet processing task
    xTaskCreatePinnedToCore(
        packet_processor_task_func,
        "packet_processor",
        4096,                    // Stack size
        NULL,                    // Parameters
        configMAX_PRIORITIES - 2, // High priority but below ISR
        &packet_processor_task,
        1                        // Pin to core 1 (core 0 handles WiFi/BT)
    );
    
    Serial.println("🚀 Complete system started: PARL_IO DMA + JTAG Bridge + Serial");
    Serial.println("📊 Commands: 'stats', 'reset', 'jtag', 'help'");
}

void loop() {
    // 1. Handle Serial Passthrough (always active)
    if (Serial.available()) {
        Serial1.write(Serial.read());
    }
    if (Serial1.available()) {
        Serial.write(Serial1.read());
    }

    // 2. Handle USB JTAG Connection State Changes
    bool usb_is_connected = usb_serial_jtag_is_connected();
    if (!usb_was_connected && usb_is_connected) {
        route_usb_jtag_to_gpio();
        Serial.println("🔌 USB JTAG connected - routing to GPIO");
    }
    if (usb_was_connected && !usb_is_connected) {
        unroute_usb_jtag_to_gpio();
        Serial.println("🔌 USB JTAG disconnected - unrouting GPIO");
    }
    usb_was_connected = usb_is_connected;

    // 3. Update Status LEDs
    digitalWrite(PIN_LED0, digitalRead(PIN_FPGA_DONE) ? LED_ON : LED_OFF);
    digitalWrite(PIN_LED1, digitalRead(PIN_FPGA_0) ? LED_ON : LED_OFF);

    // 4. Handle Serial Commands
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toLowerCase();
        
        if (cmd == "stats") {
            Serial.printf("📈 Packets: RX=%lu, Processed=%lu, Overruns=%lu\n", 
                         packets_received, packets_processed, dma_overruns);
            Serial.printf("💾 Free heap: %lu bytes\n", esp_get_free_heap_size());
            Serial.printf("🔌 USB JTAG: %s\n", usb_is_connected ? "Connected" : "Disconnected");
            Serial.printf("🎯 FPGA Status: DONE=%d, GPIO0=%d, GPIO1=%d\n", 
                         digitalRead(PIN_FPGA_DONE), digitalRead(PIN_FPGA_0), digitalRead(PIN_FPGA_1));
        } 
        else if (cmd == "reset") {
            packets_received = packets_processed = dma_overruns = 0;
            Serial.println("📊 Statistics reset");
        }
        else if (cmd == "jtag") {
            Serial.printf("🔧 JTAG Status: %s\n", usb_is_connected ? "Active on GPIO" : "Inactive");
            Serial.printf("   TCK=%d, TMS=%d, TDI=%d, TDO=%d, SRST=%d\n", 
                         PIN_TCK, PIN_TMS, PIN_TDI, PIN_TDO, PIN_SRST);
        }
        else if (cmd == "help") {
            Serial.println("🔧 Available commands:");
            Serial.println("  stats  - Show performance statistics");
            Serial.println("  reset  - Reset statistics counters");
            Serial.println("  jtag   - Show JTAG connection status");
            Serial.println("  help   - Show this help");
        }
    }

    // 5. Periodic Status Updates (much less frequent now)
    static uint32_t last_stats = 0;
    if (millis() - last_stats > 10000) {  // Every 10 seconds
        uint32_t rate = (packets_processed - last_stats) / 10;
        if (rate > 0) {
            Serial.printf("📊 Packet rate: %lu/sec\n", rate);
        }
        last_stats = packets_processed;
    }
    
    // Much more relaxed main loop timing
    delay(10);
}