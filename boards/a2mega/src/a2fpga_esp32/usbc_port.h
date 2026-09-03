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
    /* Sink-attach path (self-powered monitor presents Rp and powers us):
     * PD contract as sink, then DR_Swap to DFP, then the shared VDM
     * ladder below. Keep the VDM_* block contiguous — code uses ordered
     * comparisons over it. */
    USBC_STATE_SINK_WAIT_SRC_CAPS,
    USBC_STATE_SINK_REQUEST_SENT,
    USBC_STATE_SINK_WAIT_PS_RDY,
    USBC_STATE_SINK_READY,
    USBC_STATE_SINK_DR_SWAP_SENT,
    USBC_STATE_VDM_WAIT_IDENTITY,
    USBC_STATE_VDM_WAIT_SVIDS,
    USBC_STATE_VDM_WAIT_MODES,
    USBC_STATE_VDM_WAIT_ENTER,
    USBC_STATE_VDM_WAIT_STATUS,
    USBC_STATE_VDM_WAIT_CONFIGURE,
    USBC_STATE_DP_ACTIVE,
    USBC_STATE_USB_ONLY,
    USBC_STATE_HARD_RESET_OFF,
    USBC_STATE_VIRTUAL_DETACH,
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
    USBC_TX_REQUEST,         /* sink: RDO for the source's 5 V PDO */
    USBC_TX_SINK_CAPS,       /* sink: Get_Sink_Cap response */
    USBC_TX_DR_SWAP,         /* sink: our DR_Swap request (UFP -> DFP) */
    USBC_TX_ACCEPT_DR_SWAP,  /* sink: Accept of the partner's DR_Swap */
} usbc_tx_kind_t;

/* PD event trace (2026-09-02, PD-silent adapter dig): a fixed ring of the
 * last USBC_TRACE_LEN protocol events — TX/TXOK/TXFAIL, RX message types,
 * FUSB302 interrupt bits, hard resets, VBUS toggles, state transitions —
 * dumped on demand (telnet 't'). Exists to answer "did WE stop talking or
 * did the partner never answer" without a PD analyzer on the SBU lines. */
#define USBC_TRACE_LEN 48u
typedef enum {
    USBC_TR_NONE = 0, USBC_TR_TX, USBC_TR_TXOK, USBC_TR_TXFAIL, USBC_TR_RX,
    USBC_TR_HRST, USBC_TR_STATE, USBC_TR_IRQ, USBC_TR_UNATT, USBC_TR_VBUS
} usbc_trace_kind_t;
typedef struct {
    uint32_t t_ms;
    uint8_t kind, a, b, c;
} usbc_trace_ev_t;

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
    uint8_t source_caps_attempts;  /* Source_Caps resends this attach */
    uint32_t vbus_seen_ms;         /* VBUS-while-unattached debounce */

    uint8_t dp_mode_position;
    /* PD event trace ring (see usbc_trace_ev_t) */
    usbc_trace_ev_t trace[USBC_TRACE_LEN];
    uint8_t trace_wr, trace_n;
    uint8_t trace_last_state;
    uint8_t dp_pin_assignment;     /* USB_PD_DP_PIN_C or _E chosen for Configure */
    uint8_t vdm_retry_count;
    uint8_t expected_vdm_command;
    bool dp_hpd_level;

    /* Adapter census: raw DP mode VDOs from the last Discover Modes ACK,
     * kept across mode selection (even when we fall back to USB-only) so
     * the lane capability of hubs/adapters can be dumped on demand.
     * Motivated by the a2p25 RBR x4 plan: pin assignment C/E = 4-lane,
     * D/F = 2-lane + USB3 (no 1080p60 from an RBR-only source). */
    uint32_t dp_modes_vdo[USB_PD_MAX_DATA_OBJECTS];
    uint8_t dp_modes_count;

    /* Role tracking (power role fixed at attach; data role can DR_Swap). */
    bool power_sink;         /* true = partner sources VBUS (monitor) */
    bool data_dfp;           /* current data role for TX headers */
    uint8_t sink_attempts;   /* contract retries before USB-only fallback */
    /* stale-session guard (see stale_session_guard in usbc_port.c) */
    uint32_t guard_verify_deadline_ms;
    uint32_t guard_last_rx_ms;   /* idle-bus gate for the CC re-qual */
    uint8_t  guard_fires;    /* ceremonies fired this boot (budgeted) */
    uint8_t  guard_cc_moved;     /* consecutive re-quals: Rp on other CC */
    uint8_t  guard_cc_absent;    /* consecutive re-quals: no Rp anywhere */
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

/* Virtual replug: emulate a physical unplug/replug without touching a
 * cable — DP outputs down, VBUS off, PD receiver off, and CC
 * terminations OPENED (fusb302_disable), so the partner sees a true
 * detach. After hold_ms the port re-enters unattached and the normal
 * DRP toggle / attach / alt-mode ceremony runs from scratch. Bench
 * primitive for renegotiation-inclusive acquisition statistics
 * (PRE_REV_EXERCISE_PLAN V2.1); known limit: may not clear the
 * drain-only hub wedge class (test log row 57), which needs its caps
 * drained — this clears everything a detach clears. */
int usbc_port_virtual_replug(usbc_port_t *port, uint32_t hold_ms);
/* Dump the PD event trace (oldest first) through the HAL log callback. */
void usbc_port_trace_dump(usbc_port_t *port);

const char *usbc_port_state_name(usbc_state_t state);

#endif
