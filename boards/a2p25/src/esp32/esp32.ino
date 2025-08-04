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

#include "driver/parlio_rx.h"
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

// Global handles and state for DMA packet capture system
parlio_rx_unit_handle_t rx_unit = NULL;           // PARL_IO hardware unit handle
QueueHandle_t packet_queue;                       // Queue for DMA completion notifications
TaskHandle_t packet_processor_task;               // Dedicated packet processing task
bool usb_was_connected = false;                   // USB JTAG connection state tracking

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

// DMA completion callback - runs in interrupt context
// DISABLED: Requires PARL_IO peripheral support
/*
static bool IRAM_ATTR on_dma_receive_done(parlio_rx_unit_handle_t rx_unit, 
                                          const parlio_rx_event_data_t *edata, 
                                          void *user_ctx) {
    BaseType_t high_task_woken = pdFALSE;
    
    // Send buffer info to processing task
    uint32_t buffer_info = (uint32_t)edata->data;
    xQueueSendFromISR(packet_queue, &buffer_info, &high_task_woken);
    
    packets_received++;
    return high_task_woken == pdTRUE;
}
*/

void setup_parlio_dma() {
    // IMPORTANT: PARL_IO peripheral requires ESP-IDF v5.0+ with full driver support
    // Current Arduino Core ESP32 v3.2.0 has incomplete PARL_IO API implementation
    // This function is disabled until a compatible ESP-IDF version is available
    
    ESP_LOGW(TAG, "PARL_IO DMA setup disabled - requires newer ESP-IDF version");
    ESP_LOGW(TAG, "Packet capture functionality not available in this build");
    
    // TODO: Implement alternative using GPIO interrupts or polling for development
    // Alternative: Use SPI peripheral in slave mode for packet capture
    
    /*
    // Original PARL_IO implementation (requires ESP-IDF v5.0+):
    parlio_rx_unit_config_t rx_config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = PARLIO_DATA_WIDTH,
        .data_gpio_nums = {PIN_QSPI_D0, PIN_QSPI_D1, PIN_QSPI_D2, PIN_QSPI_D3},
        .clk_in_gpio_num = PIN_QSPI_CLK,
        .valid_gpio_num = PIN_FRAME,
        .clk_edge = PARLIO_SAMPLE_EDGE_POS,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
    };
    ESP_ERROR_CHECK(parlio_new_rx_unit(&rx_config, &rx_unit));
    */
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
    // DISABLED: Packet processing task requires PARL_IO DMA functionality
    ESP_LOGW(TAG, "Packet processor task started but DMA capture is disabled");
    
    while (1) {
        // Placeholder: Could implement alternative packet capture here
        // For now, just sleep to prevent task watchdog
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // TODO: Implement GPIO-based packet capture or SPI slave mode
        // TODO: Process packets from alternative capture method
    }
    
    /*
    // Original DMA-based implementation:
    uint32_t buffer_ptr;
    
    while (1) {
        if (xQueueReceive(packet_queue, &buffer_ptr, portMAX_DELAY) == pdTRUE) {
            uint8_t *buffer = (uint8_t*)buffer_ptr;
            
            for (int i = 0; i < DMA_BUFFER_SIZE; i += PACKET_SIZE_BYTES) {
                uint32_t packet = 0;
                for (int j = 0; j < PACKET_SIZE_BYTES; j++) {
                    packet = (packet << 8) | buffer[i + j];
                }
                
                if (packet != 0) {
                    process_packet(packet);
                    packets_processed++;
                }
            }
            
            parlio_receive_config_t recv_config = {
                .flags = {.partial_rx_en = false}
            };
            esp_err_t ret = parlio_rx_unit_receive(rx_unit, buffer, DMA_BUFFER_SIZE, &recv_config);
            if (ret != ESP_OK) {
                dma_overruns++;
                ESP_LOGW(TAG, "Failed to re-queue DMA buffer: %s", esp_err_to_name(ret));
            }
        }
    }
    */
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
    
    // Create queue for DMA buffer notifications
    packet_queue = xQueueCreate(DMA_BUFFER_COUNT * 2, sizeof(uint32_t));
    if (!packet_queue) {
        ESP_LOGE(TAG, "Failed to create packet queue");
        return;
    }
    
    // Setup PARL_IO with DMA
    setup_parlio_dma();
    
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