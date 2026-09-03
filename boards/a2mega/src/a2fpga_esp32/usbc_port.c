// SPDX-License-Identifier: MIT
#include "usbc_port.h"

#include <string.h>
#include <stdio.h>

enum {
    PS_RDY_DELAY_MS = 25,
    DISCOVERY_START_DELAY_MS = 10,
    VDM_BUSY_DELAY_MS = 50,
    ENTER_MODE_RESPONSE_MS = 500,      /* slow adapters: see send_vdm */
    ENTER_MODE_RETRIES = 6,            /* ~3.5 s total before USB-only */
    /* Sink path (we are powered by the monitor). tTypeCSinkWaitCap is
     * 620 ms max per PD; sources rebroadcast caps, so wait generously. */
    SINK_WAIT_CAPS_MS = 3000,
    SINK_ACCEPT_TIMEOUT_MS = 250,      /* tSenderResponse 30 ms + margin */
    SINK_PS_RDY_TIMEOUT_MS = 600,      /* tPSTransition 550 ms max */
    SINK_DR_SWAP_TIMEOUT_MS = 200,
    SINK_CONTRACT_RETRIES = 2,
    /* Sink requests from the source's 5 V PDO, bounded by our real draw
     * (~0.5 A board) and what the source offers. */
    SINK_OPERATING_MA = 500,
    SINK_MAXIMUM_MA = 1000,
};

static bool time_reached(uint32_t now, uint32_t deadline)
{
    return (int32_t)(now - deadline) >= 0;
}

static int stale_session_guard(usbc_port_t *port, const char *why);

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

static void trace_ev(usbc_port_t *port, usbc_trace_kind_t kind,
                     uint8_t a, uint8_t b, uint8_t c)
{
    usbc_trace_ev_t *e = &port->trace[port->trace_wr];
    e->t_ms = now_ms(port);
    e->kind = (uint8_t)kind; e->a = a; e->b = b; e->c = c;
    port->trace_wr = (uint8_t)((port->trace_wr + 1u) % USBC_TRACE_LEN);
    if (port->trace_n < USBC_TRACE_LEN)
        port->trace_n++;
}

void usbc_port_trace_dump(usbc_port_t *port)
{
    static const char *kn[] = {"--", "TX", "TXOK", "TXFL", "RX", "HRST",
                               "ST", "IRQ", "UNAT", "VBUS", "RDO"};
    char line[48];
    uint8_t n = port->trace_n > 40u ? 40u : port->trace_n;   /* tee-safe burst */
    uint8_t i0 = (uint8_t)((port->trace_wr + USBC_TRACE_LEN - n) % USBC_TRACE_LEN);
    uint32_t t0 = n ? port->trace[i0].t_ms : 0u;
    /* WARNING level: the glue routes only >= WARNING to the telnet/OSD
     * console (INFO is USB-serial only, unreachable while the adapter
     * owns USB-C). Lines stay <= 33 chars for the 39-col console after
     * the "USBC: " prefix. */
    snprintf(line, sizeof line, "TRC s%u b%u k%u a%u n%u",
             (unsigned)port->state, (unsigned)port->tx_busy,
             (unsigned)port->tx_kind, (unsigned)port->tx_attempts, (unsigned)n);
    log_message(port, USBC_LOG_WARNING, line);
    for (uint8_t k = 0; k < n; k++) {
        const usbc_trace_ev_t *e = &port->trace[(i0 + k) % USBC_TRACE_LEN];
        snprintf(line, sizeof line, "+%06lu %-4s %02X %02X %02X",
                 (unsigned long)(e->t_ms - t0),
                 e->kind < 11u ? kn[e->kind] : "??", e->a, e->b, e->c);
        log_message(port, USBC_LOG_WARNING, line);
    }
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
    trace_ev(port, USBC_TR_VBUS, (uint8_t)enable, 0u, 0u);
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
    port->dp_pin_assignment = 0u;
    port->vdm_retry_count = 0u;
    port->expected_vdm_command = 0u;
    port->power_sink = false;
    port->data_dfp = true;
    port->sink_attempts = 0u;
    port->dp_hpd_level = false;
}

static int enter_unattached(usbc_port_t *port)
{
    trace_ev(port, USBC_TR_UNATT, (uint8_t)port->state, 0u, 0u);
    set_dp_outputs(port, false, false);
    set_usb_role(port, USBC_USB_ROLE_OFF);
    set_vbus(port, false);
    reset_protocol(port);
    port->state = USBC_STATE_UNATTACHED;
    port->guard_cc_moved  = 0u;
    port->guard_cc_absent = 0u;
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
    /* Rp ADVERTISEMENT (Type-C CC signaling) decoupled from the PD PDO:
     * 1.5 A Rp = 180 uA pull-up, more than doubling the CC noise margin
     * vs default-USB's 80 uA (with 80 uA, <=20 kOhm of leakage on an
     * exposed connector reads as an attached sink — the in-slot phantom
     * attach, 2026-08-14). The TPS2553 still hardware-limits at 1 A and
     * the PDO still offers 5 V/1 A. */
    port->fusb302.rp_milliamps = 1500u;
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
                                   !port->power_sink, port->data_dfp);
    message.data_count = count;
    if (count != 0u && data != NULL)
        memcpy(message.data, data, (size_t)count * sizeof(data[0]));

    rc = fusb302_transmit(&port->fusb302, &message);
    if (rc != 0)
        return rc;
    port->tx_message = message;
    port->tx_kind = kind;
    port->tx_busy = true;
    trace_ev(port, USBC_TR_TX, (uint8_t)kind, port->tx_attempts, type);
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
    /* ENTER_MODE PATIENCE (2026-09-02, BolAAzuL trace): Discover* ACKs
     * arrive in 1-3 ms, but this adapter answers Enter Mode ~1.9 s later
     * (it brings its DP path up first); our spec-paced ~45 ms x 3 gave
     * up at 150 ms and the late reply was ignored in USB-only. Enter Mode
     * alone gets a long per-try wait; total budget = (retries+1) x wait. */
    port->deadline_ms = now_ms(port) +
        (command == USB_PD_SVDM_ENTER_MODE ? ENTER_MODE_RESPONSE_MS
                                           : port->config.vdm_response_timeout_ms);
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
    const uint32_t configure = usb_pd_dp_configure_vdo(port->dp_pin_assignment);
    return send_vdm(port, USB_PD_DISPLAYPORT_SID,
                    USB_PD_SVDM_DP_CONFIGURE, port->dp_mode_position,
                    &configure, 1u, USBC_STATE_VDM_WAIT_CONFIGURE);
}

static void fall_back_to_usb_only(usbc_port_t *port, const char *reason)
{
    set_dp_outputs(port, false, false);
    port->state = USBC_STATE_USB_ONLY;
    port->expected_vdm_command = 0u;
    /* Sink+DFP: schedule a DP discovery retry (see service_state_timer).
     * The VDM exchange flakes on some attaches; parking here until a
     * physical replug wastes an otherwise healthy contract. */
    port->deadline_ms = now_ms(port) + 2000u;
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

/* Pick the partner's DP sink mode and the pin assignment we will Configure.
 * C and E are the SAME mux configuration (TUSB1046 CTLSEL=10, four DP
 * lanes); E just omits USB 2.0 and is what a USB-C->DP cable/plug offers
 * (found 2026-09-02 census: the cable was refused as "no pin C", so a2mega
 * had never worked with a plain USB-C->DP cable). Prefer C when both are
 * offered so every hub keeps its byte-identical negotiation. D/F (two
 * lanes + USB3) need a different CTLSEL and stay unsupported here. */
static uint8_t find_dp_mode(usbc_port_t *port, const usb_pd_message_t *message)
{
    for (uint8_t i = 1u; i < message->data_count; ++i) {
        const uint32_t mode = message->data[i];
        const uint8_t pins = usb_pd_dp_partner_pin_assignments(mode);
        if (!usb_pd_dp_mode_is_sink(mode))
            continue;
        if (pins & USB_PD_DP_PIN_C) {
            port->dp_pin_assignment = USB_PD_DP_PIN_C;
            return i;
        }
        if (pins & USB_PD_DP_PIN_E) {
            port->dp_pin_assignment = USB_PD_DP_PIN_E;
            return i;
        }
    }
    port->dp_pin_assignment = 0u;
    return 0u;
}

/* Adapter census: stash and log every DP mode VDO the partner advertised.
 * Runs before mode selection so the data survives a USB-only fallback.
 * Motivated by the a2p25 RBR x4 plan: C/E = 4-lane capable mux path,
 * D/F = 2-lane + USB3 (an RBR-only source cannot do 1080p60 through it). */
static void capture_dp_modes(usbc_port_t *port, const usb_pd_message_t *message)
{
    char line[96];
    port->dp_modes_count = 0u;
    for (uint8_t i = 1u; i < message->data_count; ++i) {
        const uint32_t mode = message->data[i];
        const uint8_t pins = usb_pd_dp_partner_pin_assignments(mode);
        char letters[7];
        uint8_t n = 0u;
        if (pins & USB_PD_DP_PIN_A) letters[n++] = 'A';
        if (pins & USB_PD_DP_PIN_B) letters[n++] = 'B';
        if (pins & USB_PD_DP_PIN_C) letters[n++] = 'C';
        if (pins & USB_PD_DP_PIN_D) letters[n++] = 'D';
        if (pins & USB_PD_DP_PIN_E) letters[n++] = 'E';
        if (pins & USB_PD_DP_PIN_F) letters[n++] = 'F';
        letters[n] = '\0';
        port->dp_modes_vdo[port->dp_modes_count++] = mode;
        snprintf(line, sizeof(line),
                 "PD census: mode %u vdo=%08lx pins=%s %s%s -> %s",
                 (unsigned)i, (unsigned long)mode,
                 n != 0u ? letters : "none",
                 usb_pd_dp_mode_is_sink(mode) ? "sink" : "src-only",
                 usb_pd_dp_mode_is_receptacle(mode) ? " recept" : " plug",
                 (pins & (USB_PD_DP_PIN_C | USB_PD_DP_PIN_E)) != 0u
                     ? "4-LANE OK"
                     : (pins & (USB_PD_DP_PIN_D | USB_PD_DP_PIN_F)) != 0u
                           ? "2-LANE ONLY" : "no DP pins");
        log_message(port, USBC_LOG_INFO, line);
    }
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
        capture_dp_modes(port, message);
        port->dp_mode_position = find_dp_mode(port, message);
        if (port->dp_mode_position == 0u) {
            fall_back_to_usb_only(port, "Partner has no DP sink mode with pin C/E");
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
    trace_ev(port, USBC_TR_RX, type, message_id,
             (type == USB_PD_DATA_VENDOR && count != 0u)
                 ? (uint8_t)(usb_pd_svdm_command(message->data[0]) |
                             (usb_pd_svdm_command_type(message->data[0]) << 5))
                 : message->data_count);

    if (count == 0u && type == USB_PD_CTRL_GOOD_CRC)
        return 0;
    if (count == 0u && type == USB_PD_CTRL_SOFT_RESET) {
        set_dp_outputs(port, false, false);
        port->tx_message_id = 0u;
        port->have_last_rx_message_id = false;
        port->tx_busy = false;
        port->tx_kind = USBC_TX_NONE;
        port->dp_mode_position = 0u;
        port->dp_pin_assignment = 0u;
        if (port->power_sink) {
            port->data_dfp = false;
            fusb302_set_data_role(&port->fusb302, false);
            port->state = USBC_STATE_SINK_WAIT_SRC_CAPS;
            port->deadline_ms = now_ms(port) + SINK_WAIT_CAPS_MS;
        } else {
            port->state = USBC_STATE_SOURCE_WAIT_REQUEST;
            port->deadline_ms = now_ms(port);
        }
        return send_control(port, USB_PD_CTRL_ACCEPT, USBC_TX_ACCEPT);
    }
    if (port->have_last_rx_message_id && port->last_rx_message_id == message_id)
        return 0;
    port->last_rx_message_id = message_id;
    port->have_last_rx_message_id = true;

    if (count == 1u && type == USB_PD_DATA_REQUEST) {
        /* trace the RDO: a = objpos<<4 | mismatch, b = op mA/20, c = max mA/20 */
        trace_ev(port, USBC_TR_RDO,
                 (uint8_t)((((message->data[0] >> 28) & 0x7u) << 4) |
                           (usb_pd_rdo_capability_mismatch(message->data[0]) ? 1u : 0u)),
                 (uint8_t)(usb_pd_rdo_operating_ma(message->data[0]) / 20u),
                 (uint8_t)(usb_pd_rdo_maximum_ma(message->data[0]) / 20u));
        if (usb_pd_fixed_rdo_is_acceptable(message->data[0],
                                           port->config.source_milliamps)) {
            port->state = USBC_STATE_SOURCE_ACCEPT_SENT;
            return send_control(port, USB_PD_CTRL_ACCEPT, USBC_TX_ACCEPT);
        }
        return send_control(port, USB_PD_CTRL_REJECT, USBC_TX_REJECT);
    }

    /* ---- sink path: partner sources VBUS and offers capabilities ---- */
    if (count >= 1u && type == USB_PD_DATA_SOURCE_CAP && port->power_sink) {
        /* PDO 1 is vSafe5V fixed by spec; request it, bounded by the
         * offer. Accept caps (re)broadcasts from the wait state or after
         * a fallback to plain-device — some sources are slow starters.
         * Mid-negotiation rebroadcasts stay tolerated (retry machinery).
         * Source_Caps in a POST-negotiation state means the partner
         * believes this is a FRESH attach — we missed a detach (the
         * swap-gap survival case): previously this was SILENTLY DROPPED
         * (`return 0`), leaving the freshly-attached hub broadcasting
         * caps at a deaf, already-negotiated stack until its VL103
         * wedged. Run the ceremony instead. */
        if (port->state == USBC_STATE_SINK_REQUEST_SENT ||
            port->state == USBC_STATE_SINK_WAIT_PS_RDY)
            return 0;
        if (port->state >= USBC_STATE_SINK_READY &&
            port->state <= USBC_STATE_USB_ONLY)
            return stale_session_guard(port,
                                       "Source_Caps while negotiated");
        if (port->state != USBC_STATE_SINK_WAIT_SRC_CAPS &&
            port->state != USBC_STATE_DEVICE)
            return 0;
        const uint32_t pdo = message->data[0];
        uint16_t offered_ma = (uint16_t)((pdo & 0x3ffu) * 10u);
        uint16_t op_ma = offered_ma < SINK_OPERATING_MA ? offered_ma
                                                        : SINK_OPERATING_MA;
        uint16_t max_ma = offered_ma < SINK_MAXIMUM_MA ? offered_ma
                                                       : SINK_MAXIMUM_MA;
        const uint32_t rdo = (UINT32_C(1) << 28) |     /* object position 1 */
                             (UINT32_C(1) << 25) |     /* USB comm capable */
                             ((uint32_t)(op_ma / 10u) << 10) |
                             (uint32_t)(max_ma / 10u);
        if (queue_message(port, USB_PD_DATA_REQUEST, &rdo, 1u,
                          USBC_TX_REQUEST) != 0)
            return -1;
        port->state = USBC_STATE_SINK_REQUEST_SENT;
        port->deadline_ms = now_ms(port) + SINK_ACCEPT_TIMEOUT_MS;
        return 0;
    }
    if (count == 0u && type == USB_PD_CTRL_ACCEPT && port->power_sink) {
        if (port->state == USBC_STATE_SINK_REQUEST_SENT) {
            port->state = USBC_STATE_SINK_WAIT_PS_RDY;
            port->deadline_ms = now_ms(port) + SINK_PS_RDY_TIMEOUT_MS;
            return 0;
        }
        if (port->state == USBC_STATE_SINK_DR_SWAP_SENT) {
            port->data_dfp = true;
            fusb302_set_data_role(&port->fusb302, true);
            log_message(port, USBC_LOG_INFO,
                        "DR_Swap accepted; now DFP, starting DP discovery");
            port->state = USBC_STATE_SINK_READY;
            port->deadline_ms = now_ms(port) + DISCOVERY_START_DELAY_MS;
            return 0;
        }
    }
    if (count == 0u && type == USB_PD_CTRL_PS_RDY && port->power_sink &&
        port->state == USBC_STATE_SINK_WAIT_PS_RDY) {
        log_message(port, USBC_LOG_INFO, "Sink 5 V PD contract established");
        port->sink_attempts = 0u;
        port->state = USBC_STATE_SINK_READY;
        port->deadline_ms = now_ms(port) + DISCOVERY_START_DELAY_MS;
        return 0;
    }
    if (count == 0u && (type == USB_PD_CTRL_REJECT ||
                        type == USB_PD_CTRL_WAIT) &&
        port->state == USBC_STATE_SINK_DR_SWAP_SENT) {
        log_message(port, USBC_LOG_ERROR,
                    "DR_Swap rejected; partner keeps DFP (no DP source role)");
        port->state = USBC_STATE_DEVICE;
        return 0;
    }
    if (count == 0u && type == USB_PD_CTRL_DR_SWAP &&
        port->power_sink && !port->data_dfp) {
        /* Partner hands us DFP — exactly what the DP ladder needs. */
        return send_control(port, USB_PD_CTRL_ACCEPT, USBC_TX_ACCEPT_DR_SWAP);
    }
    if (count == 0u && type == USB_PD_CTRL_GET_SINK_CAP && port->power_sink) {
        const uint32_t sink_pdo = (UINT32_C(100) << 10) |  /* 5 V (50 mV) */
                                  (uint32_t)(SINK_OPERATING_MA / 10u);
        return queue_message(port, USB_PD_DATA_SINK_CAP, &sink_pdo, 1u,
                             USBC_TX_SINK_CAPS);
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
    trace_ev(port, USBC_TR_TXOK, (uint8_t)kind, port->tx_attempts, 0u);
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
    } else if (kind == USBC_TX_ACCEPT_DR_SWAP) {
        /* Our Accept of the partner's DR_Swap is on the wire: the swap is
         * effective. We are now DFP; run the shared DP ladder. */
        port->data_dfp = true;
        fusb302_set_data_role(&port->fusb302, true);
        log_message(port, USBC_LOG_INFO,
                    "Accepted partner DR_Swap; now DFP, starting DP discovery");
        port->state = USBC_STATE_SINK_READY;
        port->deadline_ms = now_ms(port) + DISCOVERY_START_DELAY_MS;
    }
}

static int handle_tx_failure(usbc_port_t *port)
{
    trace_ev(port, USBC_TR_TXFAIL, (uint8_t)port->tx_kind, port->tx_attempts,
             (uint8_t)port->tx_busy);
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
        extra = usb_pd_dp_configure_vdo(port->dp_pin_assignment);
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
            /* Powered by the partner. If it speaks PD (a monitor that
             * presents Rp), take a 5 V contract and DR_Swap to DFP so the
             * shared DP ladder can run; if no Source_Capabilities arrive,
             * fall back to the plain USB 2.0 device behavior. */
            if (fusb302_set_pd_receiver(&port->fusb302, true) != 0)
                return -1;
            port->state = USBC_STATE_SINK_WAIT_SRC_CAPS;
            port->deadline_ms = now_ms(port) + SINK_WAIT_CAPS_MS;
            log_message(port, USBC_LOG_INFO,
                        "USB-C sink at 5 V; waiting for source capabilities");
        }
        break;
    case USBC_STATE_SINK_WAIT_SRC_CAPS:
        if (time_reached(now, port->deadline_ms)) {
            log_message(port, USBC_LOG_INFO,
                        "No PD source capabilities; USB-only device");
            port->state = USBC_STATE_DEVICE;
        }
        break;
    case USBC_STATE_SINK_REQUEST_SENT:
    case USBC_STATE_SINK_WAIT_PS_RDY:
        if (time_reached(now, port->deadline_ms)) {
            if (++port->sink_attempts <= SINK_CONTRACT_RETRIES) {
                /* Sources rebroadcast caps; rejoin the wait. */
                port->state = USBC_STATE_SINK_WAIT_SRC_CAPS;
                port->deadline_ms = now + SINK_WAIT_CAPS_MS;
            } else {
                log_message(port, USBC_LOG_ERROR,
                            "Sink PD contract failed; USB-only device");
                port->state = USBC_STATE_DEVICE;
            }
        }
        break;
    case USBC_STATE_SINK_READY:
        if (!port->tx_busy && time_reached(now, port->deadline_ms)) {
            if (port->data_dfp)
                return send_discover_identity(port);
            if (send_control(port, USB_PD_CTRL_DR_SWAP,
                             USBC_TX_DR_SWAP) != 0)
                return -1;
            port->state = USBC_STATE_SINK_DR_SWAP_SENT;
            port->deadline_ms = now + SINK_DR_SWAP_TIMEOUT_MS;
        }
        break;
    case USBC_STATE_SINK_DR_SWAP_SENT:
        if (time_reached(now, port->deadline_ms)) {
            log_message(port, USBC_LOG_ERROR,
                        "DR_Swap unanswered; staying UFP (no DP)");
            port->state = USBC_STATE_DEVICE;
        }
        break;
    case USBC_STATE_USB_ONLY:
        /* Auto-retry DP discovery after a VDM flake (sink+DFP only —
         * the contract survived; only the Alt-Mode exchange failed). */
        if (port->power_sink && port->data_dfp &&
            time_reached(now, port->deadline_ms)) {
            log_message(port, USBC_LOG_INFO, "Retrying DP discovery");
            port->state = USBC_STATE_SINK_READY;
            port->deadline_ms = now + DISCOVERY_START_DELAY_MS;
        }
        break;
    case USBC_STATE_UNATTACHED: {
        /* Fallback attach (in-slot, 2026-08-14): a source-role partner
         * (USB-C monitor) drives VBUS but the FUSB302B DRP toggle never
         * reports the attach. VBUS while unattached is proof of a
         * source: debounce 300 ms, find the Rp side by measurement, and
         * enter the bench-proven sink flow directly. */
        bool vbus_now = false;
        if (fusb302_vbus_present(&port->fusb302, &vbus_now) == 0 && vbus_now) {
            if (port->vbus_seen_ms == 0u)
                port->vbus_seen_ms = now;
            else if ((int32_t)(now - port->vbus_seen_ms) >= 300) {
                fusb302_polarity_t pol;
                bool found = false;
                port->vbus_seen_ms = 0u;
                if (fusb302_detect_source_orientation(&port->fusb302,
                                                      &pol, &found) != 0 ||
                    !found) {
                    /* No Rp found: in-slot VBUS backfeed makes VBUS-present
                     * permanent, so don't churn — restart the toggle once
                     * and back off 2 s before re-probing. */
                    (void)fusb302_start_drp_toggle(&port->fusb302);
                    port->vbus_seen_ms = now + 2000u;
                    break;
                }
                {
                    log_message(port, USBC_LOG_INFO,
                                "VBUS with silent toggle: source partner "
                                "assumed; entering sink flow");
                    port->polarity = pol;
                    reset_protocol(port);
                    port->power_sink = true;
                    port->data_dfp = false;
                    if (fusb302_configure_sink(&port->fusb302,
                                               port->polarity) != 0)
                        return -1;
                    set_vbus(port, false);
                    port->state = USBC_STATE_DEVICE_WAIT_VBUS;
                    return 0;
                }
            }
        } else {
            port->vbus_seen_ms = 0u;
        }
        break;
    }
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
        if (!port->tx_busy && time_reached(now, port->deadline_ms)) {
            /* Give-up bound (2026-08-14): a partner that never sends a
             * Request after repeated Source_Caps is a non-PD sink or a
             * phantom (leakage) — this port exists solely for DP Alt
             * Mode, which needs PD, so release VBUS and resume toggling
             * rather than parking forever (the old behavior parked here
             * for good and ignored later real attaches). */
            if (++port->source_caps_attempts >= 12u) {
                log_message(port, USBC_LOG_INFO,
                            "no PD Request after 12 Source_Caps; "
                            "releasing (non-PD sink or phantom)");
                return enter_unattached(port);
            }
            return send_source_caps(port);
        }
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
            if (port->vdm_retry_count <
                (port->state == USBC_STATE_VDM_WAIT_ENTER ? ENTER_MODE_RETRIES
                                                          : port->config.vdm_retries)) {
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
    case USBC_STATE_VIRTUAL_DETACH:
        if (time_reached(now, port->deadline_ms))
            return enter_unattached(port);
        break;
    case USBC_STATE_HARD_RESET_OFF:
        if (time_reached(now, port->deadline_ms)) {
            if (fusb302_configure_source(&port->fusb302, port->polarity,
                                         false) != 0)
                return -1;
            set_vbus(port, true);
            port->state = USBC_STATE_SOURCE_WAIT_VBUS;
            port->deadline_ms = now + port->config.source_vbus_settle_ms;
            port->source_caps_attempts = 0u;
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
    case FUSB302_TOGGLE_ATTACHED_SOURCE_CC2: {
        /* Type-C: a source may only apply VBUS after confirming it is
         * ABSENT (partner really is a sink). Against another DRP (a PC
         * port) the toggle race can make both sides see "sink attached";
         * sourcing then back-drives the partner's live VBUS, our own
         * supply (that same VBUS on the bench) collapses, and the board
         * brownout-loops at the toggle period — seen live 2026-08-13 as
         * rhythmic power+ready LED flashing with no USB enumeration. */
        /* GUARD REMOVED (2026-08-14): the VBUS-present veto that lived
         * here (bench brownout protection) proved WRONG in the slot —
         * slot power permanently backfeeds the VBUS node, so the veto
         * blocked every legitimate source attach while the toggle was
         * correctly reporting the monitor's Rd (TOGSS=2). Phantom/race
         * protection now: 1.5 A Rp (honest Rd threshold) + the
         * 12-Source_Caps give-up (bad attaches self-clear in ~2 s). */
        port->polarity = result == FUSB302_TOGGLE_ATTACHED_SOURCE_CC2
                             ? FUSB302_POLARITY_CC2 : FUSB302_POLARITY_CC1;
        reset_protocol(port);
        if (fusb302_configure_source(&port->fusb302, port->polarity,
                                     false) != 0)
            return -1;
        set_vbus(port, true);
        port->state = USBC_STATE_SOURCE_WAIT_VBUS;
        port->deadline_ms = now_ms(port) + port->config.source_vbus_settle_ms;
        port->source_caps_attempts = 0u;
        log_message(port, USBC_LOG_INFO, "USB-C host/source partner attached");
        return 0;
    }
    case FUSB302_TOGGLE_ATTACHED_SINK_CC1:
    case FUSB302_TOGGLE_ATTACHED_SINK_CC2:
        port->polarity = result == FUSB302_TOGGLE_ATTACHED_SINK_CC2
                             ? FUSB302_POLARITY_CC2 : FUSB302_POLARITY_CC1;
        reset_protocol(port);
        port->power_sink = true;
        port->data_dfp = false;      /* Rd side attaches as UFP */
        if (fusb302_configure_sink(&port->fusb302, port->polarity) != 0)
            return -1;
        set_vbus(port, false);
        port->state = USBC_STATE_DEVICE_WAIT_VBUS;
        log_message(port, USBC_LOG_INFO, "USB-C device/sink partner attached");
        return 0;
    default: {
        char msg[48];
        snprintf(msg, sizeof msg, "toggle result code %d unhandled",
                 (int)result);
        log_message(port, USBC_LOG_INFO, msg);
        return enter_unattached(port);
    }
    }
}

int usbc_port_virtual_replug(usbc_port_t *port, uint32_t hold_ms)
{
    if (port == NULL || port->state == USBC_STATE_DISABLED)
        return -1;
    set_dp_outputs(port, false, false);
    set_usb_role(port, USBC_USB_ROLE_OFF);
    set_vbus(port, false);
    (void)fusb302_set_pd_receiver(&port->fusb302, false);
    (void)fusb302_disable(&port->fusb302);   /* CC open = true detach */
    reset_protocol(port);
    port->state = USBC_STATE_VIRTUAL_DETACH;
    port->deadline_ms = now_ms(port) + hold_ms;
    log_message(port, USBC_LOG_WARNING,
                "virtual replug: CC open, holding detached");
    return 0;
}

/* STALE-SESSION GUARD (08-26 root cause): the ESP32 rides through the
 * 1-3 s cable-swap gap on bulk caps, so a new source can attach against
 * Mac-session PD state — no fusb302 re-init, phantom-VBUS hiding the
 * detach. The cure was proven as the recovery all night: the controlled
 * teardown-reinit ceremony inside the virtual replug. Two triggers
 * funnel here (Source_Caps-in-negotiated-state = live-chip missed
 * detach; REG_POWER signature mismatch = chip reset under a live
 * stack). Budgeted per boot: the ESP32 survives its own replug, so the
 * RAM counter is meaningful — a genuinely broken chip costs
 * GUARD_MAX_FIRES short detaches, then holds for manual recovery. */
#define GUARD_MAX_FIRES    3u
#define GUARD_HOLD_MS      1500u   /* >> tCCDebounce; shorter than the
                                    * manual 'v' 3 s to cut latency */
/* 250 ms: the CC re-qual must catch the swap GAP itself — a fast
 * cable swap's unpowered window is ~1-1.5 s while the chips ride
 * through on bulk caps; at 1 Hz the 2-sample absent debounce needed a
 * >2 s gap and a real fast swap sailed through still "attached", DP
 * lanes blasting into the freshly-attaching hub (evaded all three
 * triggers, bench 08-26). At 4 Hz a >=500 ms gap fires the clean
 * unattach MID-GAP, so the hub attaches against a quiet board. */
#define GUARD_VERIFY_MS    250u

static int stale_session_guard(usbc_port_t *port, const char *why)
{
    char msg[96];
    if (port->guard_fires >= GUARD_MAX_FIRES) {
        snprintf(msg, sizeof msg,
                 "stale-session guard: %s — budget exhausted (%u), holding",
                 why, (unsigned)port->guard_fires);
        log_message(port, USBC_LOG_ERROR, msg);
        return 0;
    }
    port->guard_fires++;
    snprintf(msg, sizeof msg,
             "stale-session guard: %s — ceremony %u/%u",
             why, (unsigned)port->guard_fires, GUARD_MAX_FIRES);
    log_message(port, USBC_LOG_WARNING, msg);
    return usbc_port_virtual_replug(port, GUARD_HOLD_MS);
}

static int begin_hard_reset_recovery(usbc_port_t *port)
{
    int rc;
    trace_ev(port, USBC_TR_HRST, (uint8_t)port->state, (uint8_t)port->tx_busy,
             (uint8_t)port->tx_kind);

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
    if ((uint8_t)port->state != port->trace_last_state) {
        trace_ev(port, USBC_TR_STATE, port->trace_last_state,
                 (uint8_t)port->state, (uint8_t)port->tx_busy);
        port->trace_last_state = (uint8_t)port->state;
    }
    if (events.bits != 0u)
        trace_ev(port, USBC_TR_IRQ, (uint8_t)events.bits,
                 (uint8_t)(events.bits >> 8), (uint8_t)port->tx_busy);

    if ((events.bits & FUSB302_EVENT_FAULT) != 0u) {
        log_message(port, USBC_LOG_ERROR, "FUSB302 VCONN/thermal fault");
        return enter_unattached(port);
    }
    if ((events.bits & FUSB302_EVENT_TOGGLE_DONE) != 0u)
        return handle_toggle_result(port, events.toggle_result);
    if ((events.bits & FUSB302_EVENT_HARD_RESET) != 0u &&
        port->fusb302.source_role)
        return begin_hard_reset_recovery(port);
    if ((events.bits & FUSB302_EVENT_HARD_RESET) != 0u && port->power_sink) {
        /* Source will drop and restore VBUS; restart the sink ladder. */
        set_dp_outputs(port, false, false);
        port->tx_message_id = 0u;
        port->have_last_rx_message_id = false;
        port->tx_busy = false;
        port->tx_kind = USBC_TX_NONE;
        port->dp_mode_position = 0u;
        port->dp_pin_assignment = 0u;
        port->data_dfp = false;
        fusb302_set_data_role(&port->fusb302, false);
        port->state = USBC_STATE_DEVICE_WAIT_VBUS;
        log_message(port, USBC_LOG_INFO, "Hard reset as sink; re-attaching");
        return 0;
    }
    if ((events.bits & FUSB302_EVENT_TX_SUCCESS) != 0u)
        handle_tx_success(port);
    if ((events.bits & FUSB302_EVENT_TX_FAILED) != 0u) {
        rc = handle_tx_failure(port);
        if (rc != 0)
            return rc;
    }
    if ((events.bits & FUSB302_EVENT_RX_MESSAGE) != 0u) {
        port->guard_last_rx_ms = now_ms(port);
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

    /* stale-session guard, trigger A: 1 Hz REG_POWER signature verify
     * while in the attached family — catches the FUSB302 having reset
     * to POR defaults underneath a live stack (rail-sag mixed
     * survival), which loses every interrupt latch and leaves the
     * event-driven detach detection above permanently blind. */
    if (port->state >= USBC_STATE_DEVICE_WAIT_VBUS &&
        port->state <= USBC_STATE_USB_ONLY) {
        const uint32_t now = now_ms(port);
        if (port->guard_verify_deadline_ms == 0u ||
            time_reached(now, port->guard_verify_deadline_ms)) {
            bool intact;
            port->guard_verify_deadline_ms = now + GUARD_VERIFY_MS;
            if (fusb302_verify_powered(&port->fusb302, &intact) == 0 &&
                !intact)
                return stale_session_guard(port,
                                           "FUSB302 reset under stack");
            /* trigger C: CC ground truth (sink role, idle bus only —
             * the measure flips could clip an inbound message; PD
             * retries absorb the rare collision). Debounced 2
             * consecutive re-quals (~2 s): BMC activity on CC can
             * alias a single sample. */
            if (!port->fusb302.source_role &&
                (uint32_t)(now - port->guard_last_rx_ms) > 100u) {
                bool cc_present, cc_moved;
                if (fusb302_requalify_cc(&port->fusb302,
                                         &cc_present, &cc_moved) == 0) {
                    if (!cc_present) {
                        port->guard_cc_moved = 0u;
                        if (++port->guard_cc_absent >= 2u) {
                            /* partner GONE (phantom VBUS was hiding it):
                             * a proper detach, not a ceremony — free,
                             * unbudgeted, and the fresh attach ladder
                             * handles whatever arrives next */
                            log_message(port, USBC_LOG_WARNING,
                                        "stale-session guard: no Rp on "
                                        "either CC — missed detach");
                            return enter_unattached(port);
                        }
                    } else if (cc_moved) {
                        port->guard_cc_absent = 0u;
                        if (++port->guard_cc_moved >= 2u)
                            return stale_session_guard(port,
                                                       "CC orientation moved");
                    } else {
                        port->guard_cc_moved  = 0u;
                        port->guard_cc_absent = 0u;
                    }
                }
            }
        }
    }

    return service_state_timer(port);
}

const char *usbc_port_state_name(usbc_state_t state)
{
    static const char *const names[] = {
        "disabled", "unattached", "device-wait-vbus", "device",
        "source-wait-vbus", "source-wait-request", "source-accept-sent",
        "source-send-ps-rdy", "source-ready",
        "sink-wait-src-caps", "sink-request-sent", "sink-wait-ps-rdy",
        "sink-ready", "sink-dr-swap-sent",
        "vdm-wait-identity",
        "vdm-wait-svids", "vdm-wait-modes", "vdm-wait-enter",
        "vdm-wait-status", "vdm-wait-configure", "dp-active",
        "usb-only", "hard-reset-off", "virtual-detach",
    };
    const size_t count = sizeof(names) / sizeof(names[0]);
    return (size_t)state < count ? names[state] : "invalid";
}
