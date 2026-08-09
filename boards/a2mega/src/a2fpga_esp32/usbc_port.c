// SPDX-License-Identifier: MIT
#include "usbc_port.h"

#include <string.h>

enum {
    PS_RDY_DELAY_MS = 25,
    DISCOVERY_START_DELAY_MS = 10,
    VDM_BUSY_DELAY_MS = 50,
};

static bool time_reached(uint32_t now, uint32_t deadline)
{
    return (int32_t)(now - deadline) >= 0;
}

static uint32_t now_ms(const usbc_port_t *port)
{
    return port->hal.millis(port->hal.context);
}

static void log_message(usbc_port_t *port, usbc_log_level_t level,
                        const char *message)
{
    if (port->hal.log != NULL)
        port->hal.log(port->hal.context, level, message);
}

static void set_dp_outputs(usbc_port_t *port, bool enable, bool hpd)
{
    if (!enable)
        hpd = false;
    if (port->hal.set_fpga_hpd != NULL)
        port->hal.set_fpga_hpd(port->hal.context, hpd);
    if (port->hal.set_tusb_hpd != NULL)
        port->hal.set_tusb_hpd(port->hal.context, hpd);
    if (port->hal.set_tusb1046 != NULL)
        port->hal.set_tusb1046(port->hal.context, enable,
                              port->polarity == FUSB302_POLARITY_CC2);
    if (port->hal.set_fpga_dp_enable != NULL)
        port->hal.set_fpga_dp_enable(port->hal.context, enable);
    port->dp_hpd_level = hpd;
}

static void set_usb_role(usbc_port_t *port, usbc_usb_role_t role)
{
    if (port->hal.set_usb_role != NULL)
        port->hal.set_usb_role(port->hal.context, role);
}

static void set_vbus(usbc_port_t *port, bool enable)
{
    if (port->hal.set_vbus_source != NULL)
        port->hal.set_vbus_source(port->hal.context, enable);
}

static void reset_protocol(usbc_port_t *port)
{
    port->tx_message_id = 0u;
    port->last_rx_message_id = 0u;
    port->have_last_rx_message_id = false;
    port->tx_busy = false;
    port->tx_kind = USBC_TX_NONE;
    port->tx_attempts = 0u;
    port->dp_mode_position = 0u;
    port->vdm_retry_count = 0u;
    port->expected_vdm_command = 0u;
    port->dp_hpd_level = false;
}

static int enter_unattached(usbc_port_t *port)
{
    set_dp_outputs(port, false, false);
    set_usb_role(port, USBC_USB_ROLE_OFF);
    set_vbus(port, false);
    reset_protocol(port);
    port->state = USBC_STATE_UNATTACHED;
    log_message(port, USBC_LOG_INFO, "USB-C unattached; DRP toggle enabled");
    return fusb302_start_drp_toggle(&port->fusb302);
}

void usbc_port_default_config(usbc_port_config_t *config)
{
    if (config == NULL)
        return;
    config->source_millivolts = 5000u;
    config->source_milliamps = 1000u;
    config->source_vbus_settle_ms = 30u;
    config->hard_reset_off_ms = 750u;
    config->source_caps_period_ms = 150u;
    config->vdm_response_timeout_ms = 45u;
    config->vdm_retries = 2u;
}

int usbc_port_init(usbc_port_t *port,
                   const usbc_port_config_t *config,
                   const usbc_port_hal_t *hal,
                   const fusb302_io_t *fusb_io,
                   uint8_t fusb_i2c_address)
{
    if (port == NULL || config == NULL || hal == NULL ||
        fusb_io == NULL || hal->millis == NULL ||
        hal->set_vbus_source == NULL || hal->set_usb_role == NULL)
        return -1;
    if (config->source_millivolts != 5000u ||
        config->source_milliamps == 0u ||
        config->source_milliamps > 1500u)
        return -1;

    memset(port, 0, sizeof(*port));
    port->hal = *hal;
    port->config = *config;
    port->state = USBC_STATE_DISABLED;
    port->fusb302.rp_milliamps = config->source_milliamps;
    return fusb302_init(&port->fusb302, fusb_io, fusb_i2c_address);
}

int usbc_port_enable(usbc_port_t *port)
{
    if (port == NULL)
        return -1;
    return enter_unattached(port);
}

void usbc_port_disable(usbc_port_t *port)
{
    if (port == NULL)
        return;
    set_dp_outputs(port, false, false);
    set_usb_role(port, USBC_USB_ROLE_OFF);
    set_vbus(port, false);
    (void)fusb302_disable(&port->fusb302);
    reset_protocol(port);
    port->state = USBC_STATE_DISABLED;
}

static int queue_message(usbc_port_t *port, uint8_t type,
                         const uint32_t *data, uint8_t count,
                         usbc_tx_kind_t kind)
{
    usb_pd_message_t message;
    int rc;

    if (port->tx_busy || count > USB_PD_MAX_DATA_OBJECTS)
        return -1;
    memset(&message, 0, sizeof(message));
    message.header = usb_pd_header(type, count, port->tx_message_id,
                                   true, true);
    message.data_count = count;
    if (count != 0u && data != NULL)
        memcpy(message.data, data, (size_t)count * sizeof(data[0]));

    rc = fusb302_transmit(&port->fusb302, &message);
    if (rc != 0)
        return rc;
    port->tx_message = message;
    port->tx_kind = kind;
    port->tx_busy = true;
    port->tx_attempts = 0u;
    return 0;
}

static int send_source_caps(usbc_port_t *port)
{
    const uint32_t pdo = usb_pd_fixed_source_pdo(
        port->config.source_millivolts, port->config.source_milliamps);
    const int rc = queue_message(port, USB_PD_DATA_SOURCE_CAP,
                                 &pdo, 1u, USBC_TX_SOURCE_CAPS);
    if (rc == 0)
        port->deadline_ms = now_ms(port) + port->config.source_caps_period_ms;
    return rc;
}

static int send_control(usbc_port_t *port, uint8_t type, usbc_tx_kind_t kind)
{
    return queue_message(port, type, NULL, 0u, kind);
}

static int send_vdm(usbc_port_t *port, uint16_t svid, uint8_t command,
                    uint8_t object_position, const uint32_t *extra,
                    uint8_t extra_count, usbc_state_t wait_state)
{
    uint32_t vdos[USB_PD_MAX_DATA_OBJECTS];
    if (extra_count + 1u > USB_PD_MAX_DATA_OBJECTS)
        return -1;
    vdos[0] = usb_pd_svdm_header(svid, command, USB_PD_SVDM_REQUEST,
                                 object_position);
    if (extra_count != 0u && extra != NULL)
        memcpy(&vdos[1], extra, (size_t)extra_count * sizeof(extra[0]));
    if (queue_message(port, USB_PD_DATA_VENDOR, vdos,
                      (uint8_t)(extra_count + 1u), USBC_TX_VDM) != 0)
        return -1;
    port->expected_vdm_command = command;
    port->vdm_retry_count = 0u;
    port->deadline_ms = now_ms(port) + port->config.vdm_response_timeout_ms;
    port->state = wait_state;
    return 0;
}

static int send_discover_identity(usbc_port_t *port)
{
    return send_vdm(port, USB_PD_SID, USB_PD_SVDM_DISCOVER_IDENTITY,
                    0u, NULL, 0u, USBC_STATE_VDM_WAIT_IDENTITY);
}

static int send_discover_svids(usbc_port_t *port)
{
    return send_vdm(port, USB_PD_SID, USB_PD_SVDM_DISCOVER_SVIDS,
                    0u, NULL, 0u, USBC_STATE_VDM_WAIT_SVIDS);
}

static int send_discover_modes(usbc_port_t *port)
{
    return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                    USB_PD_SVDM_DISCOVER_MODES, 0u, NULL, 0u,
                    USBC_STATE_VDM_WAIT_MODES);
}

static int send_enter_mode(usbc_port_t *port)
{
    return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                    USB_PD_SVDM_ENTER_MODE, port->dp_mode_position,
                    NULL, 0u, USBC_STATE_VDM_WAIT_ENTER);
}

static int send_dp_status(usbc_port_t *port)
{
    const uint32_t status = usb_pd_dp_status_vdo(false);
    return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                    USB_PD_SVDM_DP_STATUS, port->dp_mode_position,
                    &status, 1u, USBC_STATE_VDM_WAIT_STATUS);
}

static int send_dp_configure(usbc_port_t *port)
{
    const uint32_t configure = usb_pd_dp_configure_vdo();
    return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                    USB_PD_SVDM_DP_CONFIGURE, port->dp_mode_position,
                    &configure, 1u, USBC_STATE_VDM_WAIT_CONFIGURE);
}

static void fall_back_to_usb_only(usbc_port_t *port, const char *reason)
{
    set_dp_outputs(port, false, false);
    port->state = USBC_STATE_USB_ONLY;
    port->expected_vdm_command = 0u;
    log_message(port, USBC_LOG_WARNING, reason);
}

static void apply_hpd_status(usbc_port_t *port, uint32_t status_vdo)
{
    const bool level = usb_pd_dp_status_hpd_level(status_vdo);
    const bool irq = usb_pd_dp_status_hpd_irq(status_vdo);

    port->dp_hpd_level = level;
    if (port->hal.set_tusb_hpd != NULL)
        port->hal.set_tusb_hpd(port->hal.context, level);
    if (port->hal.set_fpga_hpd != NULL)
        port->hal.set_fpga_hpd(port->hal.context, level);
    if (irq && level && port->hal.pulse_fpga_hpd_irq != NULL)
        port->hal.pulse_fpga_hpd_irq(port->hal.context);
    if (usb_pd_dp_status_requests_exit(status_vdo))
        fall_back_to_usb_only(port, "DP partner requested Alt Mode exit");
}

static bool response_has_dp_svid(const usb_pd_message_t *message)
{
    for (uint8_t i = 1u; i < message->data_count; ++i) {
        const uint16_t first = (uint16_t)(message->data[i] >> 16);
        const uint16_t second = (uint16_t)(message->data[i] & 0xffffu);
        if (first == USB_PD_DISPLAYPORT_SID || second == USB_PD_DISPLAYPORT_SID)
            return true;
    }
    return false;
}

static uint8_t find_dp_mode(const usb_pd_message_t *message)
{
    for (uint8_t i = 1u; i < message->data_count; ++i) {
        const uint32_t mode = message->data[i];
        if (usb_pd_dp_mode_is_sink(mode) &&
            (usb_pd_dp_partner_pin_assignments(mode) & USB_PD_DP_PIN_C) != 0u)
            return i;
    }
    return 0u;
}

static int handle_vdm_response(usbc_port_t *port,
                               const usb_pd_message_t *message)
{
    uint32_t header_vdo;
    uint16_t expected_svid;
    uint8_t command;
    uint8_t command_type;

    if (message->data_count == 0u)
        return 0;
    header_vdo = message->data[0];
    command = usb_pd_svdm_command(header_vdo);
    command_type = usb_pd_svdm_command_type(header_vdo);
    expected_svid = port->state <= USBC_STATE_VDM_WAIT_SVIDS
                        ? USB_PD_SID : USB_PD_DISPLAYPORT_SID;

    if (!usb_pd_svdm_is_structured(header_vdo) ||
        usb_pd_svdm_svid(header_vdo) != expected_svid ||
        command != port->expected_vdm_command)
        return 0;
    if (port->state >= USBC_STATE_VDM_WAIT_ENTER &&
        usb_pd_svdm_object_position(header_vdo) != port->dp_mode_position)
        return 0;
    if (command_type == USB_PD_SVDM_BUSY) {
        port->deadline_ms = now_ms(port) + VDM_BUSY_DELAY_MS;
        return 0;
    }
    if (command_type != USB_PD_SVDM_ACK) {
        if (port->state == USBC_STATE_VDM_WAIT_IDENTITY)
            return send_discover_svids(port);
        fall_back_to_usb_only(port, "DP discovery was not acknowledged");
        return 0;
    }

    switch (port->state) {
    case USBC_STATE_VDM_WAIT_IDENTITY:
        return send_discover_svids(port);
    case USBC_STATE_VDM_WAIT_SVIDS:
        if (!response_has_dp_svid(message)) {
            fall_back_to_usb_only(port, "Partner has no DisplayPort SVID");
            return 0;
        }
        return send_discover_modes(port);
    case USBC_STATE_VDM_WAIT_MODES:
        port->dp_mode_position = find_dp_mode(message);
        if (port->dp_mode_position == 0u) {
            fall_back_to_usb_only(port, "Partner has no DP sink mode with pin C");
            return 0;
        }
        return send_enter_mode(port);
    case USBC_STATE_VDM_WAIT_ENTER:
        return send_dp_status(port);
    case USBC_STATE_VDM_WAIT_STATUS:
        if (message->data_count >= 2u)
            apply_hpd_status(port, message->data[1]);
        if (port->state == USBC_STATE_USB_ONLY)
            return 0;
        return send_dp_configure(port);
    case USBC_STATE_VDM_WAIT_CONFIGURE:
        set_dp_outputs(port, true, port->dp_hpd_level);
        port->state = USBC_STATE_DP_ACTIVE;
        port->expected_vdm_command = 0u;
        log_message(port, USBC_LOG_INFO, "DisplayPort Alt Mode active (pin C)");
        return 0;
    default:
        return 0;
    }
}

static int handle_received_message(usbc_port_t *port,
                                   const usb_pd_message_t *message)
{
    const uint8_t count = message->data_count;
    const uint8_t type = usb_pd_header_type(message->header);
    const uint8_t message_id = usb_pd_header_id(message->header);

    if (count == 0u && type == USB_PD_CTRL_GOOD_CRC)
        return 0;
    if (count == 0u && type == USB_PD_CTRL_SOFT_RESET) {
        set_dp_outputs(port, false, false);
        port->tx_message_id = 0u;
        port->have_last_rx_message_id = false;
        port->tx_busy = false;
        port->tx_kind = USBC_TX_NONE;
        port->dp_mode_position = 0u;
        port->state = USBC_STATE_SOURCE_WAIT_REQUEST;
        port->deadline_ms = now_ms(port);
        return send_control(port, USB_PD_CTRL_ACCEPT, USBC_TX_ACCEPT);
    }
    if (port->have_last_rx_message_id && port->last_rx_message_id == message_id)
        return 0;
    port->last_rx_message_id = message_id;
    port->have_last_rx_message_id = true;

    if (count == 1u && type == USB_PD_DATA_REQUEST) {
        if (usb_pd_fixed_rdo_is_acceptable(message->data[0],
                                           port->config.source_milliamps)) {
            port->state = USBC_STATE_SOURCE_ACCEPT_SENT;
            return send_control(port, USB_PD_CTRL_ACCEPT, USBC_TX_ACCEPT);
        }
        return send_control(port, USB_PD_CTRL_REJECT, USBC_TX_REJECT);
    }
    if (count == 0u && type == USB_PD_CTRL_GET_SOURCE_CAP)
        return send_source_caps(port);
    if (count == 0u && (type == USB_PD_CTRL_DR_SWAP ||
                        type == USB_PD_CTRL_PR_SWAP ||
                        type == USB_PD_CTRL_VCONN_SWAP ||
                        type == USB_PD_CTRL_GET_SINK_CAP))
        return send_control(port, USB_PD_CTRL_REJECT, USBC_TX_REJECT);

    if (count >= 1u && type == USB_PD_DATA_VENDOR) {
        const uint32_t vdm = message->data[0];
        const uint8_t command = usb_pd_svdm_command(vdm);
        const bool mode_entered =
            port->state == USBC_STATE_VDM_WAIT_STATUS ||
            port->state == USBC_STATE_VDM_WAIT_CONFIGURE ||
            port->state == USBC_STATE_DP_ACTIVE;
        if (mode_entered && usb_pd_svdm_is_structured(vdm) &&
            usb_pd_svdm_svid(vdm) == USB_PD_DISPLAYPORT_SID &&
            command == USB_PD_SVDM_ATTENTION && count >= 2u) {
            if (usb_pd_svdm_object_position(vdm) != port->dp_mode_position)
                return 0;
            apply_hpd_status(port, message->data[1]);
            return 0;
        }
        return handle_vdm_response(port, message);
    }
    return 0;
}

static int drain_receive_fifo(usbc_port_t *port)
{
    usb_pd_message_t message;
    int rc;
    for (;;) {
        rc = fusb302_receive(&port->fusb302, &message);
        if (rc == 1)
            return 0;
        if (rc != 0)
            return rc;
        rc = handle_received_message(port, &message);
        if (rc != 0)
            return rc;
    }
}

static void handle_tx_success(usbc_port_t *port)
{
    const usbc_tx_kind_t kind = port->tx_kind;
    port->tx_busy = false;
    port->tx_kind = USBC_TX_NONE;
    port->tx_message_id = (uint8_t)((port->tx_message_id + 1u) & 0x7u);

    if (kind == USBC_TX_ACCEPT && port->state == USBC_STATE_SOURCE_ACCEPT_SENT) {
        port->state = USBC_STATE_SOURCE_SEND_PS_RDY;
        port->deadline_ms = now_ms(port) + PS_RDY_DELAY_MS;
    } else if (kind == USBC_TX_PS_RDY) {
        port->state = USBC_STATE_SOURCE_READY;
        port->deadline_ms = now_ms(port) + DISCOVERY_START_DELAY_MS;
        log_message(port, USBC_LOG_INFO, "Fixed 5 V PD contract established");
    }
}

static int handle_tx_failure(usbc_port_t *port)
{
    if (!port->tx_busy)
        return 0;
    if (++port->tx_attempts <= 2u)
        return fusb302_transmit(&port->fusb302, &port->tx_message);
    port->tx_busy = false;
    port->tx_kind = USBC_TX_NONE;
    if (port->state >= USBC_STATE_VDM_WAIT_IDENTITY &&
        port->state <= USBC_STATE_VDM_WAIT_CONFIGURE)
        fall_back_to_usb_only(port, "PD transmission failed during DP discovery");
    else
        port->deadline_ms = now_ms(port) + port->config.source_caps_period_ms;
    return 0;
}

static int retry_vdm_for_state(usbc_port_t *port)
{
    const usbc_state_t state = port->state;
    uint32_t extra;

    port->tx_busy = false;
    if (state == USBC_STATE_VDM_WAIT_IDENTITY)
        return send_discover_identity(port);
    if (state == USBC_STATE_VDM_WAIT_SVIDS)
        return send_discover_svids(port);
    if (state == USBC_STATE_VDM_WAIT_MODES)
        return send_discover_modes(port);
    if (state == USBC_STATE_VDM_WAIT_ENTER)
        return send_enter_mode(port);
    if (state == USBC_STATE_VDM_WAIT_STATUS) {
        extra = usb_pd_dp_status_vdo(false);
        return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                        USB_PD_SVDM_DP_STATUS, port->dp_mode_position,
                        &extra, 1u, state);
    }
    if (state == USBC_STATE_VDM_WAIT_CONFIGURE) {
        extra = usb_pd_dp_configure_vdo();
        return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                        USB_PD_SVDM_DP_CONFIGURE, port->dp_mode_position,
                        &extra, 1u, state);
    }
    return 0;
}

static int service_state_timer(usbc_port_t *port)
{
    const uint32_t now = now_ms(port);
    bool vbus_present;

    switch (port->state) {
    case USBC_STATE_DEVICE_WAIT_VBUS:
        if (fusb302_vbus_present(&port->fusb302, &vbus_present) == 0 &&
            vbus_present) {
            set_usb_role(port, USBC_USB_ROLE_DEVICE);
            port->state = USBC_STATE_DEVICE;
            log_message(port, USBC_LOG_INFO, "USB-C device attached at 5 V");
        }
        break;
    case USBC_STATE_SOURCE_WAIT_VBUS:
        if (!time_reached(now, port->deadline_ms))
            break;
        if (fusb302_vbus_present(&port->fusb302, &vbus_present) != 0 ||
            !vbus_present) {
            log_message(port, USBC_LOG_ERROR, "Source VBUS failed to reach valid 5 V");
            return enter_unattached(port);
        }
        set_usb_role(port, USBC_USB_ROLE_HOST);
        if (fusb302_set_pd_receiver(&port->fusb302, true) != 0)
            return -1;
        port->state = USBC_STATE_SOURCE_WAIT_REQUEST;
        return send_source_caps(port);
    case USBC_STATE_SOURCE_WAIT_REQUEST:
        if (!port->tx_busy && time_reached(now, port->deadline_ms))
            return send_source_caps(port);
        break;
    case USBC_STATE_SOURCE_SEND_PS_RDY:
        if (!port->tx_busy && time_reached(now, port->deadline_ms))
            return send_control(port, USB_PD_CTRL_PS_RDY, USBC_TX_PS_RDY);
        break;
    case USBC_STATE_SOURCE_READY:
        if (!port->tx_busy && time_reached(now, port->deadline_ms))
            return send_discover_identity(port);
        break;
    case USBC_STATE_VDM_WAIT_IDENTITY:
    case USBC_STATE_VDM_WAIT_SVIDS:
    case USBC_STATE_VDM_WAIT_MODES:
    case USBC_STATE_VDM_WAIT_ENTER:
    case USBC_STATE_VDM_WAIT_STATUS:
    case USBC_STATE_VDM_WAIT_CONFIGURE:
        if (!port->tx_busy && time_reached(now, port->deadline_ms)) {
            if (port->vdm_retry_count < port->config.vdm_retries) {
                const uint8_t next_retry = (uint8_t)(port->vdm_retry_count + 1u);
                const int rc = retry_vdm_for_state(port);
                port->vdm_retry_count = next_retry;
                return rc;
            }
            if (port->state == USBC_STATE_VDM_WAIT_IDENTITY)
                return send_discover_svids(port);
            fall_back_to_usb_only(port, "DP Alt Mode response timeout");
        }
        break;
    case USBC_STATE_HARD_RESET_OFF:
        if (time_reached(now, port->deadline_ms)) {
            if (fusb302_configure_source(&port->fusb302, port->polarity,
                                         false) != 0)
                return -1;
            set_vbus(port, true);
            port->state = USBC_STATE_SOURCE_WAIT_VBUS;
            port->deadline_ms = now + port->config.source_vbus_settle_ms;
        }
        break;
    default:
        break;
    }
    return 0;
}

static int handle_toggle_result(usbc_port_t *port,
                                fusb302_toggle_result_t result)
{
    switch (result) {
    case FUSB302_TOGGLE_ATTACHED_SOURCE_CC1:
    case FUSB302_TOGGLE_ATTACHED_SOURCE_CC2:
        port->polarity = result == FUSB302_TOGGLE_ATTACHED_SOURCE_CC2
                             ? FUSB302_POLARITY_CC2 : FUSB302_POLARITY_CC1;
        reset_protocol(port);
        if (fusb302_configure_source(&port->fusb302, port->polarity,
                                     false) != 0)
            return -1;
        set_vbus(port, true);
        port->state = USBC_STATE_SOURCE_WAIT_VBUS;
        port->deadline_ms = now_ms(port) + port->config.source_vbus_settle_ms;
        log_message(port, USBC_LOG_INFO, "USB-C host/source partner attached");
        return 0;
    case FUSB302_TOGGLE_ATTACHED_SINK_CC1:
    case FUSB302_TOGGLE_ATTACHED_SINK_CC2:
        port->polarity = result == FUSB302_TOGGLE_ATTACHED_SINK_CC2
                             ? FUSB302_POLARITY_CC2 : FUSB302_POLARITY_CC1;
        reset_protocol(port);
        if (fusb302_configure_sink(&port->fusb302, port->polarity) != 0)
            return -1;
        set_vbus(port, false);
        port->state = USBC_STATE_DEVICE_WAIT_VBUS;
        log_message(port, USBC_LOG_INFO, "USB-C device/sink partner attached");
        return 0;
    default:
        return enter_unattached(port);
    }
}

static int begin_hard_reset_recovery(usbc_port_t *port)
{
    int rc;

    set_dp_outputs(port, false, false);
    set_usb_role(port, USBC_USB_ROLE_OFF);
    set_vbus(port, false);
    rc = fusb302_set_pd_receiver(&port->fusb302, false);
    reset_protocol(port);
    port->state = USBC_STATE_HARD_RESET_OFF;
    port->deadline_ms = now_ms(port) + port->config.hard_reset_off_ms;
    log_message(port, USBC_LOG_WARNING, "PD hard reset; cycling 5 V source");
    return rc;
}

int usbc_port_task(usbc_port_t *port)
{
    fusb302_events_t events;
    bool detached;
    bool vbus_present;
    int rc;

    if (port == NULL || port->state == USBC_STATE_DISABLED)
        return 0;
    rc = fusb302_poll_events(&port->fusb302, &events);
    if (rc != 0)
        return rc;

    if ((events.bits & FUSB302_EVENT_FAULT) != 0u) {
        log_message(port, USBC_LOG_ERROR, "FUSB302 VCONN/thermal fault");
        return enter_unattached(port);
    }
    if ((events.bits & FUSB302_EVENT_TOGGLE_DONE) != 0u)
        return handle_toggle_result(port, events.toggle_result);
    if ((events.bits & FUSB302_EVENT_HARD_RESET) != 0u &&
        port->fusb302.source_role)
        return begin_hard_reset_recovery(port);
    if ((events.bits & FUSB302_EVENT_TX_SUCCESS) != 0u)
        handle_tx_success(port);
    if ((events.bits & FUSB302_EVENT_TX_FAILED) != 0u) {
        rc = handle_tx_failure(port);
        if (rc != 0)
            return rc;
    }
    if ((events.bits & FUSB302_EVENT_RX_MESSAGE) != 0u) {
        rc = drain_receive_fifo(port);
        if (rc != 0)
            return rc;
    }

    if (port->fusb302.source_role &&
        (events.bits & FUSB302_EVENT_CC_CHANGED) != 0u &&
        fusb302_source_detached(&port->fusb302, &detached) == 0 && detached)
        return enter_unattached(port);
    if (!port->fusb302.source_role &&
        (events.bits & FUSB302_EVENT_VBUS_CHANGED) != 0u &&
        fusb302_vbus_present(&port->fusb302, &vbus_present) == 0 &&
        !vbus_present)
        return enter_unattached(port);

    return service_state_timer(port);
}

const char *usbc_port_state_name(usbc_state_t state)
{
    static const char *const names[] = {
        "disabled", "unattached", "device-wait-vbus", "device",
        "source-wait-vbus", "source-wait-request", "source-accept-sent",
        "source-send-ps-rdy", "source-ready", "vdm-wait-identity",
        "vdm-wait-svids", "vdm-wait-modes", "vdm-wait-enter",
        "vdm-wait-status", "vdm-wait-configure", "dp-active",
        "usb-only", "hard-reset-off",
    };
    const size_t count = sizeof(names) / sizeof(names[0]);
    return (size_t)state < count ? names[state] : "invalid";
}
