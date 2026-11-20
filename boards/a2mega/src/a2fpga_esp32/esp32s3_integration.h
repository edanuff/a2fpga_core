// SPDX-License-Identifier: MIT
#ifndef DISPLAYPORT_VERILOG_ESP32S3_USBC_INTEGRATION_H
#define DISPLAYPORT_VERILOG_ESP32S3_USBC_INTEGRATION_H

#include <stdbool.h>

#include "usbc_port.h"

typedef struct {
    int vbus_enable;
    int tusb_flip;
    int tusb_ctl0;
    int tusb_ctl1;
    int tusb_hpdin;
    int fpga_dp_enable;
    int fpga_hpd_level;
    int fpga_hpd_irq_toggle; /* Connect to optional usbc_dp_control.sv. */
} esp32s3_usbc_pins_t;

typedef struct {
    usbc_port_t port;
    esp32s3_usbc_pins_t pins;
    bool hpd_irq_toggle;
} esp32s3_usbc_t;

int esp32s3_usbc_init(esp32s3_usbc_t *instance,
                      const esp32s3_usbc_pins_t *pins);
int esp32s3_usbc_task(esp32s3_usbc_t *instance);
void esp32s3_usbc_disable(esp32s3_usbc_t *instance);

#endif
