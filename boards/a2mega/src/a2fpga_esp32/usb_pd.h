// SPDX-License-Identifier: MIT
#ifndef DISPLAYPORT_VERILOG_USB_PD_H
#define DISPLAYPORT_VERILOG_USB_PD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Small USB PD 2.0 vocabulary for the fixed 5 V DisplayPort-source policy.
 * This is deliberately not a general USB PD header.
 */

#define USB_PD_MAX_DATA_OBJECTS 7u

enum usb_pd_control_message {
    USB_PD_CTRL_GOOD_CRC       = 1,
    USB_PD_CTRL_ACCEPT         = 3,
    USB_PD_CTRL_REJECT         = 4,
    USB_PD_CTRL_PS_RDY         = 6,
    USB_PD_CTRL_GET_SOURCE_CAP = 7,
    USB_PD_CTRL_GET_SINK_CAP   = 8,
    USB_PD_CTRL_DR_SWAP        = 9,
    USB_PD_CTRL_PR_SWAP        = 10,
    USB_PD_CTRL_VCONN_SWAP     = 11,
    USB_PD_CTRL_WAIT           = 12,
    USB_PD_CTRL_SOFT_RESET     = 13,
};

enum usb_pd_data_message {
    USB_PD_DATA_SOURCE_CAP = 1,
    USB_PD_DATA_REQUEST    = 2,
    USB_PD_DATA_SINK_CAP   = 4,
    USB_PD_DATA_VENDOR     = 15,
};

enum usb_pd_svdm_command_type {
    USB_PD_SVDM_REQUEST = 0,
    USB_PD_SVDM_ACK     = 1,
    USB_PD_SVDM_NAK     = 2,
    USB_PD_SVDM_BUSY    = 3,
};

enum usb_pd_svdm_command {
    USB_PD_SVDM_DISCOVER_IDENTITY = 1,
    USB_PD_SVDM_DISCOVER_SVIDS    = 2,
    USB_PD_SVDM_DISCOVER_MODES    = 3,
    USB_PD_SVDM_ENTER_MODE        = 4,
    USB_PD_SVDM_EXIT_MODE         = 5,
    USB_PD_SVDM_ATTENTION         = 6,
    USB_PD_SVDM_DP_STATUS         = 16,
    USB_PD_SVDM_DP_CONFIGURE      = 17,
};

#define USB_PD_SID             UINT16_C(0xff00)
#define USB_PD_DISPLAYPORT_SID UINT16_C(0xff01)

#define USB_PD_DP_PIN_C UINT8_C(0x04)
#define USB_PD_DP_SIGNAL_V13 UINT8_C(0x01)
#define USB_PD_DP_MODE_SINK UINT8_C(0x01)

typedef struct {
    uint16_t header;
    uint32_t data[USB_PD_MAX_DATA_OBJECTS];
    uint8_t data_count;
} usb_pd_message_t;

static inline uint16_t usb_pd_header(uint8_t type,
                                     uint8_t data_count,
                                     uint8_t message_id,
                                     bool power_source,
                                     bool data_dfp)
{
    return (uint16_t)(((uint16_t)(data_count & 0x7u) << 12) |
                      ((uint16_t)(message_id & 0x7u) << 9) |
                      ((uint16_t)(power_source ? 1u : 0u) << 8) |
                      (UINT16_C(1) << 6) | /* USB PD revision 2.0 */
                      ((uint16_t)(data_dfp ? 1u : 0u) << 5) |
                      (uint16_t)(type & 0x1fu));
}

static inline uint8_t usb_pd_header_type(uint16_t header)
{
    return (uint8_t)(header & 0x1fu);
}

static inline uint8_t usb_pd_header_count(uint16_t header)
{
    return (uint8_t)((header >> 12) & 0x7u);
}

static inline uint8_t usb_pd_header_id(uint16_t header)
{
    return (uint8_t)((header >> 9) & 0x7u);
}

static inline uint32_t usb_pd_fixed_source_pdo(uint16_t millivolts,
                                                uint16_t milliamps)
{
    /* USB communications capable; fixed supply; no role-swap advertisement. */
    return (UINT32_C(1) << 26) |
           ((uint32_t)(millivolts / 50u) << 10) |
           (uint32_t)(milliamps / 10u);
}

static inline bool usb_pd_fixed_rdo_is_acceptable(uint32_t rdo,
                                                   uint16_t max_milliamps)
{
    const uint8_t object_position = (uint8_t)((rdo >> 28) & 0x7u);
    const uint16_t operating_ma = (uint16_t)(((rdo >> 10) & 0x3ffu) * 10u);
    const uint16_t maximum_ma = (uint16_t)((rdo & 0x3ffu) * 10u);

    return object_position == 1u &&
           operating_ma != 0u &&
           maximum_ma >= operating_ma &&
           operating_ma <= max_milliamps &&
           maximum_ma <= max_milliamps;
}

static inline uint32_t usb_pd_svdm_header(uint16_t svid,
                                          uint8_t command,
                                          uint8_t command_type,
                                          uint8_t object_position)
{
    return ((uint32_t)svid << 16) |
           (UINT32_C(1) << 15) |
           ((uint32_t)(object_position & 0x7u) << 8) |
           ((uint32_t)(command_type & 0x3u) << 6) |
           (uint32_t)(command & 0x1fu);
}

static inline uint16_t usb_pd_svdm_svid(uint32_t vdo)
{
    return (uint16_t)(vdo >> 16);
}

static inline bool usb_pd_svdm_is_structured(uint32_t vdo)
{
    return (vdo & (UINT32_C(1) << 15)) != 0u;
}

static inline uint8_t usb_pd_svdm_command(uint32_t vdo)
{
    return (uint8_t)(vdo & 0x1fu);
}

static inline uint8_t usb_pd_svdm_command_type(uint32_t vdo)
{
    return (uint8_t)((vdo >> 6) & 0x3u);
}

static inline uint8_t usb_pd_svdm_object_position(uint32_t vdo)
{
    return (uint8_t)((vdo >> 8) & 0x7u);
}

static inline uint8_t usb_pd_dp_partner_pin_assignments(uint32_t mode_vdo)
{
    /* A receptacle reports UFP_D pins in bits 23:16; a plug uses 15:8. */
    return (mode_vdo & (UINT32_C(1) << 6)) != 0u
               ? (uint8_t)((mode_vdo >> 16) & 0x3fu)
               : (uint8_t)((mode_vdo >> 8) & 0x3fu);
}

static inline bool usb_pd_dp_mode_is_sink(uint32_t mode_vdo)
{
    return (mode_vdo & 0x3u) == USB_PD_DP_MODE_SINK ||
           (mode_vdo & 0x3u) == 0x3u;
}

static inline uint32_t usb_pd_dp_status_vdo(bool enabled)
{
    /* We are connected as DFP_D; no multifunction preference. */
    return ((uint32_t)(enabled ? 1u : 0u) << 3) | UINT32_C(1);
}

static inline uint32_t usb_pd_dp_configure_vdo(void)
{
    /* Pin C, DP v1.3 signaling, configure the partner as UFP_D. */
    return ((uint32_t)USB_PD_DP_PIN_C << 8) |
           ((uint32_t)USB_PD_DP_SIGNAL_V13 << 2) |
           UINT32_C(2);
}

static inline bool usb_pd_dp_status_hpd_level(uint32_t status_vdo)
{
    return (status_vdo & (UINT32_C(1) << 7)) != 0u;
}

static inline bool usb_pd_dp_status_hpd_irq(uint32_t status_vdo)
{
    return (status_vdo & (UINT32_C(1) << 8)) != 0u;
}

static inline bool usb_pd_dp_status_requests_exit(uint32_t status_vdo)
{
    return (status_vdo & ((UINT32_C(1) << 6) | (UINT32_C(1) << 5))) != 0u;
}

#endif
