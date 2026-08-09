// SPDX-License-Identifier: MIT
#ifndef DISPLAYPORT_VERILOG_USBC_PORT_H
#define DISPLAYPORT_VERILOG_USBC_PORT_H

#include <stdbool.h>
#include <stdint.h>

#include "fusb302.h"

typedef enum {
    USBC_USB_ROLE_OFF = 0,
    USBC_USB_ROLE_DEVICE,
    USBC_USB_ROLE_HOST,
} usbc_usb_role_t;

typedef enum {
    USBC_STATE_DISABLED = 0,
    USBC_STATE_UNATTACHED,
    USBC_STATE_DEVICE_WAIT_VBUS,
    USBC_STATE_DEVICE,
    USBC_STATE_SOURCE_WAIT_VBUS,
    USBC_STATE_SOURCE_WAIT_REQUEST,
    USBC_STATE_SOURCE_ACCEPT_SENT,
    USBC_STATE_SOURCE_SEND_PS_RDY,
    USBC_STATE_SOURCE_READY,
    USBC_STATE_VDM_WAIT_IDENTITY,
    USBC_STATE_VDM_WAIT_SVIDS,
    USBC_STATE_VDM_WAIT_MODES,
    USBC_STATE_VDM_WAIT_ENTER,
    USBC_STATE_VDM_WAIT_STATUS,
    USBC_STATE_VDM_WAIT_CONFIGURE,
    USBC_STATE_DP_ACTIVE,
    USBC_STATE_USB_ONLY,
    USBC_STATE_HARD_RESET_OFF,
} usbc_state_t;

typedef enum {
    USBC_LOG_DEBUG = 0,
    USBC_LOG_INFO,
    USBC_LOG_WARNING,
    USBC_LOG_ERROR,
} usbc_log_level_t;

typedef struct {
    void *context;

    uint32_t (*millis)(void *context);
    void (*set_vbus_source)(void *context, bool enable);
    void (*set_usb_role)(void *context, usbc_usb_role_t role);

    /* TUSB1046A GPIO mode: implementation drives FLIP, CTL0 and CTL1. */
    void (*set_tusb1046)(void *context, bool dp_enable, bool flipped);
    void (*set_tusb_hpd)(void *context, bool level);

    /* FPGA callbacks may directly drive top-level pins or an optional bridge. */
    void (*set_fpga_dp_enable)(void *context, bool enable);
    void (*set_fpga_hpd)(void *context, bool level);
    void (*pulse_fpga_hpd_irq)(void *context);

    void (*log)(void *context, usbc_log_level_t level, const char *message);
} usbc_port_hal_t;

typedef struct {
    uint16_t source_millivolts;       /* Must remain 5000 for this example. */
    uint16_t source_milliamps;        /* 1000 for the scoped board;
                                         see SPEC.md before raising. */
    uint16_t source_vbus_settle_ms;   /* Board switch/ramp delay, typically 30. */
    uint16_t hard_reset_off_ms;       /* Source-recovery off interval. */
    uint16_t source_caps_period_ms;   /* Retry interval before a PD Request. */
    uint16_t vdm_response_timeout_ms;
    uint8_t vdm_retries;
} usbc_port_config_t;

typedef enum {
    USBC_TX_NONE = 0,
    USBC_TX_SOURCE_CAPS,
    USBC_TX_ACCEPT,
    USBC_TX_PS_RDY,
    USBC_TX_REJECT,
    USBC_TX_VDM,
} usbc_tx_kind_t;

typedef struct {
    fusb302_t fusb302;
    usbc_port_hal_t hal;
    usbc_port_config_t config;

    usbc_state_t state;
    fusb302_polarity_t polarity;
    uint32_t deadline_ms;
    uint8_t tx_message_id;
    uint8_t last_rx_message_id;
    bool have_last_rx_message_id;
    bool tx_busy;
    usbc_tx_kind_t tx_kind;
    usb_pd_message_t tx_message;
    uint8_t tx_attempts;

    uint8_t dp_mode_position;
    uint8_t vdm_retry_count;
    uint8_t expected_vdm_command;
    bool dp_hpd_level;
} usbc_port_t;

void usbc_port_default_config(usbc_port_config_t *config);
int usbc_port_init(usbc_port_t *port,
                   const usbc_port_config_t *config,
                   const usbc_port_hal_t *hal,
                   const fusb302_io_t *fusb_io,
                   uint8_t fusb_i2c_address);
int usbc_port_enable(usbc_port_t *port);
void usbc_port_disable(usbc_port_t *port);

/* Call from one task every 1 ms and immediately after FUSB302 INT_N falls. */
int usbc_port_task(usbc_port_t *port);

const char *usbc_port_state_name(usbc_state_t state);

#endif
