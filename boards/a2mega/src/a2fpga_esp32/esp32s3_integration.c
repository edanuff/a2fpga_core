// SPDX-License-Identifier: MIT
/*
 * ESP32-S3 integration skeleton.
 *
 * This file intentionally does not include ESP-IDF headers. Map the board_*
 * functions below to your existing I2C/GPIO/time layer and map
 * app_usb_select_role() to the already-working ESP32-S3 host/device firmware.
 * The protocol core then has no dependency on a particular ESP-IDF release.
 */
#include "esp32s3_integration.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Supplied by the incorporating ESP32-S3 project. Return zero on success. */
extern int board_i2c_read(uint8_t address, uint8_t reg,
                          uint8_t *data, size_t length);
extern int board_i2c_write(uint8_t address, uint8_t reg,
                           const uint8_t *data, size_t length);
extern void board_delay_us(uint32_t microseconds);
extern uint32_t board_millis(void);
extern void board_gpio_write(int gpio, bool level);
extern void board_log(const char *message);

/* Stop the previous controller/stack before starting the requested role. */
extern void app_usb_select_role(usbc_usb_role_t role);

static int io_read(void *context, uint8_t address, uint8_t reg,
                   uint8_t *data, size_t length)
{
    (void)context;
    return board_i2c_read(address, reg, data, length);
}

static int io_write(void *context, uint8_t address, uint8_t reg,
                    const uint8_t *data, size_t length)
{
    (void)context;
    return board_i2c_write(address, reg, data, length);
}

static void io_delay_us(void *context, uint32_t microseconds)
{
    (void)context;
    board_delay_us(microseconds);
}

static uint32_t hal_millis(void *context)
{
    (void)context;
    return board_millis();
}

static void hal_set_vbus(void *context, bool enable)
{
    esp32s3_usbc_t *instance = context;
    board_gpio_write(instance->pins.vbus_enable, enable);
}

static void hal_set_usb_role(void *context, usbc_usb_role_t role)
{
    (void)context;
    app_usb_select_role(role);
}

static void hal_set_tusb1046(void *context, bool dp_enable, bool flipped)
{
    esp32s3_usbc_t *instance = context;

    /* GPIO mode: CTL1=1, CTL0=0 is four-pair DP assignment C/E. */
    if (!dp_enable) {
        board_gpio_write(instance->pins.tusb_ctl1, false);
        board_gpio_write(instance->pins.tusb_ctl0, false);
        return;
    }
    board_gpio_write(instance->pins.tusb_flip, flipped);
    board_gpio_write(instance->pins.tusb_ctl0, false);
    board_gpio_write(instance->pins.tusb_ctl1, true);
}

static void hal_set_tusb_hpd(void *context, bool level)
{
    esp32s3_usbc_t *instance = context;
    board_gpio_write(instance->pins.tusb_hpdin, level);
}

static void hal_set_fpga_enable(void *context, bool enable)
{
    esp32s3_usbc_t *instance = context;
    board_gpio_write(instance->pins.fpga_dp_enable, enable);
}

static void hal_set_fpga_hpd(void *context, bool level)
{
    esp32s3_usbc_t *instance = context;
    board_gpio_write(instance->pins.fpga_hpd_level, level);
}

static void hal_pulse_fpga_hpd_irq(void *context)
{
    esp32s3_usbc_t *instance = context;
    instance->hpd_irq_toggle = !instance->hpd_irq_toggle;
    board_gpio_write(instance->pins.fpga_hpd_irq_toggle,
                     instance->hpd_irq_toggle);
}

static void hal_log(void *context, usbc_log_level_t level, const char *message)
{
    (void)context;
    (void)level;
    board_log(message);
}

int esp32s3_usbc_init(esp32s3_usbc_t *instance,
                      const esp32s3_usbc_pins_t *pins)
{
    usbc_port_config_t config;
    fusb302_io_t fusb_io;
    usbc_port_hal_t hal;
    int rc;

    if (instance == NULL || pins == NULL)
        return -1;
    memset(instance, 0, sizeof(*instance));
    instance->pins = *pins;

    /* TUSB1046A powers up in USB3 mode; this L-H-L pulse powers it down. */
    board_gpio_write(pins->tusb_ctl1, false);
    board_gpio_write(pins->tusb_ctl0, false);
    board_delay_us(10u);
    board_gpio_write(pins->tusb_ctl0, true);
    board_delay_us(10u);
    board_gpio_write(pins->tusb_ctl0, false);
    board_gpio_write(pins->tusb_hpdin, false);
    board_gpio_write(pins->fpga_dp_enable, false);
    board_gpio_write(pins->fpga_hpd_level, false);
    board_gpio_write(pins->fpga_hpd_irq_toggle, false);
    board_gpio_write(pins->vbus_enable, false);

    fusb_io = (fusb302_io_t) {
        .context = instance,
        .read = io_read,
        .write = io_write,
        .delay_us = io_delay_us,
    };
    hal = (usbc_port_hal_t) {
        .context = instance,
        .millis = hal_millis,
        .set_vbus_source = hal_set_vbus,
        .set_usb_role = hal_set_usb_role,
        .set_tusb1046 = hal_set_tusb1046,
        .set_tusb_hpd = hal_set_tusb_hpd,
        .set_fpga_dp_enable = hal_set_fpga_enable,
        .set_fpga_hpd = hal_set_fpga_hpd,
        .pulse_fpga_hpd_irq = hal_pulse_fpga_hpd_irq,
        .log = hal_log,
    };
    usbc_port_default_config(&config);

    rc = usbc_port_init(&instance->port, &config, &hal, &fusb_io,
                        FUSB302_I2C_ADDRESS_DEFAULT);
    return rc == 0 ? usbc_port_enable(&instance->port) : rc;
}

int esp32s3_usbc_task(esp32s3_usbc_t *instance)
{
    return instance == NULL ? -1 : usbc_port_task(&instance->port);
}

void esp32s3_usbc_disable(esp32s3_usbc_t *instance)
{
    if (instance != NULL)
        usbc_port_disable(&instance->port);
}
