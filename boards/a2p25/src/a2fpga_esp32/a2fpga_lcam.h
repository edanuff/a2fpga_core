#ifndef A2FPGA_LCAM_H
#define A2FPGA_LCAM_H

#include <Arduino.h>
#include "esp_err.h"

// External constants (defined in main .ino file)
extern const int PIN_CAM_PCLK;
extern const int PIN_CAM_VSYNC;
extern const int PIN_CAM_D0;
extern const int PIN_CAM_D1;
extern const int PIN_CAM_D2;
extern const int PIN_CAM_D3;

// Build-time options (defined in main .ino file)
extern const uint32_t SMOKE_MS;

// Function declarations
void lcam_start();
void lcam_stop();
void lcam_log_every_n_words(uint32_t n);
void lcam_print_status();
void lcam_set_logging(uint8_t n);

// Task management functions
esp_err_t lcam_init_tasks();
void lcam_cleanup_tasks();

#endif // A2FPGA_LCAM_H