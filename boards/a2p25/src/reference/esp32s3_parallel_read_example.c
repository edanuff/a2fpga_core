/*
 * Working ESP32-S3 Parallel Data Read Example
 * Source: https://raw.githubusercontent.com/odelot/esp32s3-fast-parallel-read/a0af16d3051923b9252acc1c84bb732e3101f336/main/main.c
 * 
 * This is the reference implementation that successfully uses LCD_CAM peripheral
 * for parallel data capture without camera libraries.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "soc/gpio_sig_map.h"
#include "esp_rom_gpio.h"
#include "soc/lcd_cam_struct.h"
#include "soc/lcd_cam_reg.h"
#include "hal/dma_types.h"
#include "esp_private/gdma.h"
#include "driver/periph_ctrl.h"
#include "esp_intr_alloc.h"

static const char *TAG = "parallel_read";

// Configuration
#define DMA_BUFFER_SIZE 512
#define DMA_NUM_BUFFERS 2

// GPIO configuration for 16-bit parallel data
#define NUM_DATA_PINS 16
static const int data_pins[NUM_DATA_PINS] = {
    GPIO_NUM_1, GPIO_NUM_2, GPIO_NUM_3, GPIO_NUM_4,
    GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8,
    GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12,
    GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16
};

#define MCLK_PIN GPIO_NUM_17

// DMA variables
static gdma_channel_handle_t dma_chan;
static dma_descriptor_t *dma_descriptors;
static uint8_t *dma_buffer;
static volatile bool transfer_complete = false;

// Interrupt handlers
static void IRAM_ATTR vsyncHandler(void *arg) {
    // VSYNC interrupt handler (placeholder)
}

static bool IRAM_ATTR dma_rx_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data) {
    transfer_complete = true;
    return false;
}

// Clock configuration
static void configure_camera_clock(int clksel, int div_a, int div_b, int div_n) {
    lcd_cam_dev_t *dev = &LCD_CAM;
    
    // Configure clock source and dividers
    dev->cam_ctrl.cam_clk_sel = clksel;
    dev->cam_ctrl.cam_clkm_div_a = div_a;
    dev->cam_ctrl.cam_clkm_div_b = div_b;
    dev->cam_ctrl.cam_clkm_div_num = div_n;
}

// GPIO matrix configuration
static void configure_gpio_matrix() {
    // Configure data pins
    for (int i = 0; i < NUM_DATA_PINS; i++) {
        gpio_set_direction(data_pins[i], GPIO_MODE_INPUT);
        esp_rom_gpio_pad_select_gpio(data_pins[i]);
        gpio_matrix_in(data_pins[i], CAM_DATA_IN0_IDX + i, false);
    }
    
    // Configure master clock output
    gpio_set_direction(MCLK_PIN, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(MCLK_PIN);
    gpio_matrix_out(MCLK_PIN, CAM_CLK_IDX, false, false);
}

// DMA setup
static esp_err_t setup_dma() {
    // Allocate DMA channel
    gdma_channel_alloc_config_t rx_alloc_config = {
        .sibling_chan = NULL,
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .flags = {
            .reserve_sibling = 0
        }
    };
    
    ESP_ERROR_CHECK(gdma_new_channel(&rx_alloc_config, &dma_chan));
    ESP_ERROR_CHECK(gdma_connect(dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_CAM, 0)));
    
    // Set up DMA callbacks
    gdma_rx_event_callbacks_t cbs = {
        .on_recv_eof = dma_rx_callback,
    };
    gdma_register_rx_event_callbacks(dma_chan, &cbs, NULL);
    
    // Allocate DMA buffers and descriptors
    size_t total_size = DMA_BUFFER_SIZE * DMA_NUM_BUFFERS;
    dma_buffer = heap_caps_malloc(total_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!dma_buffer) {
        ESP_LOGE(TAG, "Failed to allocate DMA buffer");
        return ESP_ERR_NO_MEM;
    }
    
    dma_descriptors = heap_caps_malloc(DMA_NUM_BUFFERS * sizeof(dma_descriptor_t), 
                                       MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (!dma_descriptors) {
        ESP_LOGE(TAG, "Failed to allocate DMA descriptors");
        return ESP_ERR_NO_MEM;
    }
    
    // Configure DMA descriptors
    for (int i = 0; i < DMA_NUM_BUFFERS; i++) {
        dma_descriptors[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        dma_descriptors[i].dw0.suc_eof = 0;
        dma_descriptors[i].next = &dma_descriptors[(i + 1) % DMA_NUM_BUFFERS];
        dma_descriptors[i].dw0.size = DMA_BUFFER_SIZE;
        dma_descriptors[i].dw0.length = DMA_BUFFER_SIZE;
        dma_descriptors[i].buffer = dma_buffer + (DMA_BUFFER_SIZE * i);
    }
    
    return ESP_OK;
}

// Camera peripheral configuration
static void configure_camera_peripheral(size_t dma_node_buffer_size) {
    lcd_cam_dev_t *dev = &LCD_CAM;
    
    // Reset camera bus
    dev->cam_ctrl1.cam_afifo_reset = 1;
    dev->cam_ctrl1.cam_afifo_reset = 0;
    
    // Configure camera control registers
    dev->cam_ctrl.cam_update = 1;
    dev->cam_ctrl.cam_stop_en = 0;
    dev->cam_ctrl.cam_bit_order = 1;      // CRITICAL setting
    dev->cam_ctrl.cam_byte_order = 0;
    dev->cam_ctrl.cam_vs_eof_en = 0;      // VSYNC EOF disabled
    
    // Configure camera control 1 register
    dev->cam_ctrl1.cam_vh_de_mode_en = 0;
    dev->cam_ctrl1.cam_rec_data_bytelen = dma_node_buffer_size - 1;
    dev->cam_ctrl1.cam_line_int_num = 0;
    dev->cam_ctrl1.cam_clk_inv = 0;
    dev->cam_ctrl1.cam_vsync_filter_en = 0;  // VSYNC filter disabled
    dev->cam_ctrl1.cam_2byte_en = 1;         // 16-bit mode
    dev->cam_ctrl1.cam_start = 1;
}

// VSYNC workaround to kickstart peripheral
static void vsync_workaround() {
    gpio_matrix_in(GPIO_MATRIX_CONST_ZERO_INPUT, CAM_V_SYNC_IDX, false);
    esp_rom_delay_us(100);
    gpio_matrix_in(GPIO_MATRIX_CONST_ONE_INPUT, CAM_V_SYNC_IDX, false);
}

// Main initialization function
static esp_err_t init_parallel_read() {
    // Enable LCD_CAM peripheral
    periph_module_enable(PERIPH_LCD_CAM_MODULE);
    
    // Configure GPIO matrix
    configure_gpio_matrix();
    
    // Set up DMA
    ESP_ERROR_CHECK(setup_dma());
    
    // Configure camera clock (for ~157kHz)
    configure_camera_clock(1, 1, 1, 127);  // XTAL_CLK, div settings
    
    // Configure camera peripheral
    configure_camera_peripheral(DMA_BUFFER_SIZE);
    
    // Start DMA
    gdma_start(dma_chan, (intptr_t)dma_descriptors);
    
    // VSYNC workaround to kickstart
    vsync_workaround();
    
    ESP_LOGI(TAG, "Parallel read initialized successfully");
    return ESP_OK;
}

void app_main() {
    ESP_LOGI(TAG, "ESP32-S3 Fast Parallel Read Example");
    
    // Initialize parallel reading
    ESP_ERROR_CHECK(init_parallel_read());
    
    // Main loop
    while (1) {
        if (transfer_complete) {
            transfer_complete = false;
            ESP_LOGI(TAG, "DMA transfer completed");
            
            // Process data here
            // ...
            
            // Restart transfer
            gdma_start(dma_chan, (intptr_t)dma_descriptors);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}