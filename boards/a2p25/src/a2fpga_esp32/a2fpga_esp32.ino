/*
Tools->Board->Boards Manager->esp32 by espressif
Tools->USB CDC On Boot: Enabled
Tools->CPU Frequency: 240 MHz (WiFi)
Tools->Board: Adafruit QT Py ESP32-S3 (4M Flash 2M PSRAM)
Tools->JTAG Adapter: Integrated USB JTAG
Tools->USB Mode: Hardware CDC and JTAG
Tools->Upload Mode: UART0 / Hardware CDC
Tools->Upload Speed: 921600
*/

/*
 * A2FPGA ESP32-S3 Bridge System
 * 
 * This system provides three key functions for Apple II FPGA development:
 * 
 * 1. HIGH-PERFORMANCE PACKET CAPTURE (LCD_CAM + DMA)
 *    - Captures 4-bit parallel data from FPGA using ESP32-S3's LCD_CAM peripheral
 *    - FPGA streams packets at Apple II bus rate (~1MHz) with 8×4-bit chunks per packet
 *    - Uses ping-pong DMA buffers for zero-copy packet reception
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
 */

// Camera library causes crashes - using direct LCD_CAM register access instead
// #include "esp_camera.h"
#include "driver/gpio.h"
#include "soc/usb_serial_jtag_reg.h"
#include "soc/gpio_sig_map.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_debug_helpers.h"

// Direct LCD_CAM peripheral access
#include "soc/lcd_cam_struct.h"
#include "soc/lcd_cam_reg.h"
#include "soc/gdma_struct.h"
#include "soc/gdma_reg.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "driver/periph_ctrl.h"

#include <Arduino.h>

// Custom exception handler to print detailed crash info
void print_backtrace() {
    Serial.println("[CRASH] Attempting to print backtrace...");
    esp_backtrace_print(10);  // Print up to 10 stack frames
}

#define TAG "A2FPGA_COMPLETE"

// Status LEDs - Connected to ESP32-S3 QT Py onboard LEDs
#define PIN_LED0 1              // LED shows FPGA_DONE status
#define PIN_LED1 2              // LED shows FPGA GPIO0 status  
#define LED_ON  HIGH
#define LED_OFF LOW

// FPGA Status Monitoring Pins
#define PIN_FPGA_DONE 48        // FPGA configuration complete signal
#define PIN_FPGA_0 9            // FPGA general purpose status pin 0
#define PIN_FPGA_1 10           // FPGA general purpose status pin 1
#define PIN_FPGA_RECONFIG_N  21

// 4-bit Parallel Data Interface from FPGA (PARL_IO)
#define PIN_PARL_D0 14          // Data bit 0 (LSB)
#define PIN_PARL_D1 15          // Data bit 1
#define PIN_PARL_D2 16          // Data bit 2  
#define PIN_PARL_D3 17          // Data bit 3 (MSB)
#define PIN_PARL_CLK 13         // Clock signal from FPGA
#define PIN_PARL_FRAME     12        // Frame/Valid signal for packet boundaries

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

// Performance statistics (volatile for ISR access) - MUST be declared before callback
volatile uint32_t packets_received = 0;           // DMA buffers completed
volatile uint32_t packets_processed = 0;          // Individual packets decoded
volatile uint32_t dma_overruns = 0;               // Buffer re-queue failures

// Global handles and state for direct LCD_CAM peripheral access
gdma_channel_handle_t dma_rx_channel = NULL;      // GDMA channel for camera RX
dma_descriptor_t dma_descriptors[DMA_BUFFER_COUNT]; // DMA descriptor array
uint8_t *dma_buffers[DMA_BUFFER_COUNT];           // DMA buffer pointers
volatile int current_buffer = 0;                  // Current buffer index
volatile int buffers_ready = 0;                   // Number of buffers ready for processing
bool usb_was_connected = false;                   // USB JTAG connection state tracking
volatile bool dma_capture_active = false;         // DMA capture state

// DMA interrupt callback
static bool IRAM_ATTR dma_rx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data) {
    BaseType_t high_task_awoken = pdFALSE;
    
    // Buffer completed - increment ready count
    buffers_ready++;
    packets_received++;
    
    // Cycle to next buffer
    current_buffer = (current_buffer + 1) % DMA_BUFFER_COUNT;
    
    return high_task_awoken == pdTRUE;
}

// Performance statistics moved above (before DMA callback)

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

// Direct LCD_CAM peripheral initialization without camera library
esp_err_t setup_lcd_cam_direct() {
    Serial.println("[INFO] Setting up LCD_CAM peripheral via direct register access");
    Serial.printf("[INFO] Free heap before init: %lu bytes\n", esp_get_free_heap_size());
    
    // 1. Enable LCD_CAM peripheral clock
    periph_module_enable(PERIPH_LCD_CAM_MODULE);
    
    
    // 2. Configure GPIO pins for 4-bit parallel input
    pinMode(PIN_PARL_D0, INPUT);        // Data bit 0 (LSB)
    pinMode(PIN_PARL_D1, INPUT);        // Data bit 1
    pinMode(PIN_PARL_D2, INPUT);        // Data bit 2
    pinMode(PIN_PARL_D3, INPUT);        // Data bit 3 (MSB)
    pinMode(PIN_PARL_CLK, INPUT);       // Clock signal from FPGA
    pinMode(PIN_PARL_FRAME, INPUT);     // Frame/packet sync from FPGA
    
    // 3. Connect GPIO pins to LCD_CAM peripheral signals (use correct ESP32-S3 signal names)
    esp_rom_gpio_connect_in_signal(PIN_PARL_D0, CAM_DATA_IN0_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_PARL_D1, CAM_DATA_IN1_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_PARL_D2, CAM_DATA_IN2_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_PARL_D3, CAM_DATA_IN3_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_PARL_CLK, CAM_PCLK_IDX, false);
    esp_rom_gpio_connect_in_signal(PIN_PARL_FRAME, CAM_V_SYNC_IDX, false);
    
    // 4. Allocate GDMA channel for camera RX
    gdma_channel_alloc_config_t rx_alloc_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
    };
    esp_err_t ret = gdma_new_channel(&rx_alloc_config, &dma_rx_channel);
    if (ret != ESP_OK) {
        Serial.printf("[ERROR] Failed to allocate GDMA channel: 0x%x\n", ret);
        return ret;
    }
    
    // 5. Connect GDMA channel to LCD_CAM peripheral
    gdma_connect(dma_rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_CAM, 0));
    
    // 6. Set up DMA interrupt callback
    gdma_rx_event_callbacks_t cbs = {
        .on_recv_done = dma_rx_callback,
    };
    gdma_register_rx_event_callbacks(dma_rx_channel, &cbs, NULL);
    
    // 7. Allocate DMA buffers in DRAM
    for (int i = 0; i < DMA_BUFFER_COUNT; i++) {
        dma_buffers[i] = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
        if (!dma_buffers[i]) {
            Serial.printf("[ERROR] Failed to allocate DMA buffer %d\n", i);
            return ESP_ERR_NO_MEM;
        }
        Serial.printf("[DEBUG] DMA buffer %d allocated at 0x%08x\n", i, (uint32_t)dma_buffers[i]);
    }
    
    // 8. Set up DMA descriptors (linked list for continuous capture)
    for (int i = 0; i < DMA_BUFFER_COUNT; i++) {
        dma_descriptors[i].dw0.size = DMA_BUFFER_SIZE;
        dma_descriptors[i].dw0.length = 0;
        dma_descriptors[i].dw0.suc_eof = 1;  // Generate EOF interrupt
        dma_descriptors[i].dw0.owner = 1;    // DMA owns this descriptor
        dma_descriptors[i].buffer = dma_buffers[i];
        dma_descriptors[i].next = &dma_descriptors[(i + 1) % DMA_BUFFER_COUNT];  // Circular buffer
    }
    
    // 9. Configure LCD_CAM control registers for continuous capture
    LCD_CAM.cam_ctrl.cam_stop_en = 0;                    // Disable auto-stop
    LCD_CAM.cam_ctrl.cam_vsync_filter_thres = 4;        // VSYNC filter threshold
    LCD_CAM.cam_ctrl.cam_byte_order = 0;                // Normal byte order
    LCD_CAM.cam_ctrl.cam_bit_order = 0;                 // Normal bit order
    LCD_CAM.cam_ctrl.cam_line_int_en = 0;               // Disable line interrupt
    LCD_CAM.cam_ctrl.cam_vs_eof_en = 1;                 // Enable VSYNC EOF
    
    LCD_CAM.cam_ctrl1.cam_rec_data_bytelen = DMA_BUFFER_SIZE - 1;  // Max bytes per transfer
    LCD_CAM.cam_ctrl1.cam_line_int_num = 0;             // Line interrupt number
    LCD_CAM.cam_ctrl1.cam_clk_inv = 0;                  // Don't invert clock
    LCD_CAM.cam_ctrl1.cam_vsync_filter_en = 1;          // Enable VSYNC filter
    LCD_CAM.cam_ctrl1.cam_2byte_en = 0;                 // Single byte mode
    LCD_CAM.cam_ctrl1.cam_de_inv = 0;                   // Don't invert DE
    LCD_CAM.cam_ctrl1.cam_hsync_inv = 0;                // Don't invert HSYNC
    LCD_CAM.cam_ctrl1.cam_vsync_inv = 0;                // Don't invert VSYNC
    
    // 10. Reset LCD_CAM peripheral
    LCD_CAM.cam_ctrl1.cam_afifo_reset = 1;
    LCD_CAM.cam_ctrl1.cam_afifo_reset = 0;
    
    Serial.println("[SUCCESS] LCD_CAM peripheral configured via direct register access!");
    Serial.printf("[INFO] Data pins: D0=%d, D1=%d, D2=%d, D3=%d\n", 
             PIN_PARL_D0, PIN_PARL_D1, PIN_PARL_D2, PIN_PARL_D3);
    Serial.printf("[INFO] Clock pin: PCLK=%d, Frame sync: VSYNC=%d\n", 
             PIN_PARL_CLK, PIN_PARL_FRAME);
    Serial.printf("[INFO] Allocated %d DMA buffers of %d bytes each\n", DMA_BUFFER_COUNT, DMA_BUFFER_SIZE);
    Serial.println("[INFO] DMA descriptors configured for continuous capture");
    
    return ESP_OK;
}

// Start DMA capture
esp_err_t start_dma_capture() {
    if (dma_capture_active) {
        Serial.println("[WARNING] DMA capture already active");
        return ESP_OK;
    }
    
    Serial.println("[INFO] Starting DMA capture...");
    
    // Reset buffer state
    current_buffer = 0;
    buffers_ready = 0;
    
    // Start GDMA transfer with first descriptor
    esp_err_t ret = gdma_start(dma_rx_channel, (intptr_t)&dma_descriptors[0]);
    if (ret != ESP_OK) {
        Serial.printf("[ERROR] Failed to start GDMA: 0x%x\n", ret);
        return ret;
    }
    
    // Update LCD_CAM registers to reflect current transfer settings
    LCD_CAM.cam_ctrl.cam_update = 1;
    
    // Start LCD_CAM capture
    LCD_CAM.cam_ctrl1.cam_start = 1;
    
    dma_capture_active = true;
    Serial.println("[SUCCESS] DMA capture started - ready to receive FPGA data!");
    
    return ESP_OK;
}

// Stop DMA capture
esp_err_t stop_dma_capture() {
    if (!dma_capture_active) {
        return ESP_OK;
    }
    
    Serial.println("[INFO] Stopping DMA capture...");
    
    // Stop LCD_CAM capture
    LCD_CAM.cam_ctrl1.cam_start = 0;
    
    // Stop GDMA
    gdma_stop(dma_rx_channel);
    
    dma_capture_active = false;
    Serial.println("[INFO] DMA capture stopped");
    
    return ESP_OK;
}

// Process received data buffers
void process_dma_buffers() {
    while (buffers_ready > 0) {
        // Calculate which buffer to process
        int process_buffer = (current_buffer - buffers_ready + DMA_BUFFER_COUNT) % DMA_BUFFER_COUNT;
        
        uint8_t *buffer_data = dma_buffers[process_buffer];
        size_t bytes_received = DMA_BUFFER_SIZE; // For now, assume full buffer
        
        Serial.printf("[DEBUG] Processing buffer %d: %d bytes\n", process_buffer, bytes_received);
        
        // Process 4-bit parallel data as Apple II packets
        // Each byte contains two 4-bit nibbles from consecutive clock cycles
        for (size_t i = 0; i < bytes_received && i < 64; i++) { // Limit debug output
            uint8_t byte_data = buffer_data[i];
            uint8_t nibble_high = (byte_data >> 4) & 0x0F;  // Upper 4 bits
            uint8_t nibble_low = byte_data & 0x0F;          // Lower 4 bits
            
            // For now, just show the raw data
            if (i < 16) {  // Show first 16 bytes for debugging
                Serial.printf("[DATA] Byte %02d: 0x%02X (nibbles: %X %X)\n", i, byte_data, nibble_high, nibble_low);
            }
        }
        
        packets_processed++;
        buffers_ready--;
        
        // Show some basic statistics
        if (packets_processed % 10 == 0) {
            Serial.printf("[STATS] Processed %lu buffers, %lu total packets\n", packets_processed, packets_received);
        }
    }
}

// ============================================================================
// APPLE II BUS PACKET ANALYSIS
// Decodes 32-bit packets from FPGA containing Apple II bus transactions
// Packet format: [16-bit address][8-bit data][7-bit flags][1-bit reset indicator]
// Flags: [7]=RW_N, [6]=M2SEL_N, [5]=M2B0, [4]=SW_GS, [3:1]=Reserved, [0]=Reset
// ============================================================================

void process_packet(uint32_t pkt) {
    // Check for reset indicator packet (bit 0 set)
    if (pkt & 0x1) {
        //Serial.println("🔄 FPGA System Reset Detected");
        packets_received = packets_processed = dma_overruns = 0;  // Reset statistics
        return;
    }
    
    // Extract packet components for normal packets
    uint16_t addr = (pkt >> 16) & 0xFFFF;   // Memory/I/O address
    uint8_t data  = (pkt >> 8) & 0xFF;      // Data value
    uint8_t flags = pkt & 0xFE;             // Bus flags (excluding reset bit)

    // Apple II Speaker detection ($C030 - classic click sound)
    if (addr == 0xC030) {
        //Serial.println("🔊 Speaker Toggle");
        return;
    }

    // Audio register writes (Mockingboard/sound cards at $C040-$C04F)
    if ((addr & 0xFFF0) == 0xC040) {
        //Serial.printf("🎵 Audio: %02X\n", data);
        return;
    }

    // Log other I/O page activity (reduce noise by filtering to I/O space)
    if ((addr & 0xFF00) == 0xC000) {  // Apple II I/O page ($C000-$C0FF)
        //Serial.printf("📦 I/O: %04X %02X %02X\n", addr, data, flags);
    }
}

// ============================================================================
// FREERTOS PACKET PROCESSING TASK
// Dedicated task running on Core 1 for real-time packet processing
// Operates independently from main loop to ensure consistent performance
// ============================================================================

/*void packet_processor_task_func(void *param) {
    ESP_LOGI(TAG, "Starting LCD_CAM packet processing task");
    
    while (1) {
        // Capture frame from LCD_CAM (this blocks until frame is ready)
        camera_fb_t *fb = esp_camera_fb_get();
        
        if (fb != NULL) {
            packets_received++;
            ESP_LOGD(TAG, "Captured frame: %d bytes, %dx%d pixels", 
                     fb->len, fb->width, fb->height);
            
            // Process the captured frame data as packets
            // Each "pixel" in our grayscale frame represents captured parallel data
            uint8_t *frame_data = fb->buf;
            size_t frame_size = fb->len;
            
            // Process data based on how LCD_CAM captures our 4-bit parallel data
            // The exact format depends on the LCD_CAM configuration
            if (fb->format == PIXFORMAT_GRAYSCALE) {
                // In grayscale mode, each byte represents 8 bits of captured data
                // Our 4-bit parallel data gets packed into bytes
                
                // Process all complete packets in the frame
                size_t complete_packets = frame_size / PACKET_SIZE_BYTES;
                for (size_t i = 0; i < complete_packets; i++) {
                    size_t offset = i * PACKET_SIZE_BYTES;
                    
                    // Reconstruct 32-bit packet from captured parallel data
                    // Note: The exact bit packing depends on LCD_CAM configuration
                    uint32_t packet = 0;
                    for (int j = 0; j < PACKET_SIZE_BYTES; j++) {
                        packet = (packet << 8) | frame_data[offset + j];
                    }
                    
                    // Skip empty packets (FPGA may send padding)
                    if (packet != 0) {
                        process_packet(packet);
                        packets_processed++;
                    }
                }
            } else {
                ESP_LOGW(TAG, "Unexpected pixel format: %d", fb->format);
            }
            
            // Return the frame buffer to the driver for reuse
            esp_camera_fb_return(fb);
            
        } else {
            dma_overruns++;  // Track camera capture failures
            ESP_LOGW(TAG, "Camera capture failed - no frame buffer");
            vTaskDelay(pdMS_TO_TICKS(10)); // Small delay before retry
        }
    }
}*/

void setup() {
    // Enable detailed crash reporting
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    
    // Initialize FPGA status pins
    pinMode(PIN_FPGA_DONE, INPUT_PULLUP);
    pinMode(PIN_FPGA_0, INPUT_PULLUP);
    pinMode(PIN_FPGA_1, INPUT_PULLUP);
    
    // Initialize LEDs
    pinMode(PIN_LED0, OUTPUT);
    pinMode(PIN_LED1, OUTPUT);

    // Enable FPGA configuration
    pinMode(PIN_FPGA_RECONFIG_N, OUTPUT);
    digitalWrite(PIN_FPGA_RECONFIG_N, HIGH);
    
    // Initialize serial communications
    Serial.begin();
    Serial1.begin(BAUD, SERIAL_8N1, PIN_RXD, PIN_TXD);
    
    // Setup LCD_CAM peripheral via direct register access
    esp_err_t cam_result = setup_lcd_cam_direct();
    if (cam_result != ESP_OK) {
        Serial.printf("[ERROR] LCD_CAM setup failed: 0x%x\n", cam_result);
    } else {
        // Start DMA capture automatically
        esp_err_t capture_result = start_dma_capture();
        if (capture_result != ESP_OK) {
            Serial.printf("[ERROR] Failed to start DMA capture: 0x%x\n", capture_result);
        }
    }
    
    // Create dedicated packet processing task
    /*
    xTaskCreatePinnedToCore(
        packet_processor_task_func,
        "packet_processor",
        4096,                    // Stack size
        NULL,                    // Parameters
        configMAX_PRIORITIES - 2, // High priority but below ISR
        &packet_processor_task,
        1                        // Pin to core 1 (core 0 handles WiFi/BT)
    );
    */
    
    Serial.println("🚀 Complete system started: LCD_CAM DMA + JTAG Bridge + Serial");
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
        //Serial.println("🔌 USB JTAG connected - routing to GPIO");
    }
    if (usb_was_connected && !usb_is_connected) {
        unroute_usb_jtag_to_gpio();
        //Serial.println("🔌 USB JTAG disconnected - unrouting GPIO");
    }
    usb_was_connected = usb_is_connected;

    // 3. Process DMA buffers if data is ready
    if (buffers_ready > 0) {
        process_dma_buffers();
    }
    
    // 4. Update Status LEDs
    digitalWrite(PIN_LED0, digitalRead(PIN_FPGA_DONE) ? LED_ON : LED_OFF);
    digitalWrite(PIN_LED1, digitalRead(PIN_FPGA_0) ? LED_ON : LED_OFF);

    // 5. Handle Serial Commands for DMA control
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toLowerCase();
        
        if (cmd == "stats") {
            Serial.printf("📈 Packets: RX=%lu, Processed=%lu, Ready=%d\n", 
                         packets_received, packets_processed, buffers_ready);
            Serial.printf("💾 Free heap: %lu bytes\n", esp_get_free_heap_size());
            Serial.printf("🔌 USB JTAG: %s\n", usb_is_connected ? "Connected" : "Disconnected");
            Serial.printf("📡 DMA Capture: %s\n", dma_capture_active ? "Active" : "Stopped");
            Serial.printf("🎯 FPGA Status: DONE=%d, GPIO0=%d, GPIO1=%d\n", 
                         digitalRead(PIN_FPGA_DONE), digitalRead(PIN_FPGA_0), digitalRead(PIN_FPGA_1));
        } 
        else if (cmd == "start") {
            start_dma_capture();
        }
        else if (cmd == "stop") {
            stop_dma_capture();
        }
        else if (cmd == "reset") {
            packets_received = packets_processed = dma_overruns = 0;
            buffers_ready = 0;
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
            Serial.println("  start  - Start DMA capture");
            Serial.println("  stop   - Stop DMA capture");
            Serial.println("  reset  - Reset statistics counters");
            Serial.println("  jtag   - Show JTAG connection status");
            Serial.println("  help   - Show this help");
        }
    }

    // 6. Periodic Status Updates (much less frequent now)
    static uint32_t last_stats = 0;
    static uint32_t last_processed = 0;
    if (millis() - last_stats > 10000) {  // Every 10 seconds
        uint32_t rate = (packets_processed - last_processed) / 10;
        if (rate > 0 || dma_capture_active) {
            Serial.printf("📊 DMA Status: %s, Buffer rate: %lu/sec, Ready: %d\n", 
                         dma_capture_active ? "ACTIVE" : "STOPPED", rate, buffers_ready);
        }
        last_processed = packets_processed;
        last_stats = millis();
    }
    
    // Much more relaxed main loop timing
    delay(10);
}