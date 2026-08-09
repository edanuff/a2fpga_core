// SPDX-License-Identifier: MIT
#include "fusb302.h"

#include <string.h>

enum {
    REG_DEVICE_ID  = 0x01,
    REG_SWITCHES0  = 0x02,
    REG_SWITCHES1  = 0x03,
    REG_MEASURE    = 0x04,
    REG_CONTROL0   = 0x06,
    REG_CONTROL1   = 0x07,
    REG_CONTROL2   = 0x08,
    REG_CONTROL3   = 0x09,
    REG_MASK       = 0x0a,
    REG_POWER      = 0x0b,
    REG_RESET      = 0x0c,
    REG_MASKA      = 0x0e,
    REG_MASKB      = 0x0f,
    REG_STATUS1A   = 0x3d,
    REG_INTERRUPTA = 0x3e,
    REG_INTERRUPTB = 0x3f,
    REG_STATUS0    = 0x40,
    REG_STATUS1    = 0x41,
    REG_INTERRUPT  = 0x42,
    REG_FIFOS      = 0x43,
};

enum {
    SW0_CC1_PD     = 1u << 0,
    SW0_CC2_PD     = 1u << 1,
    SW0_MEASURE_CC1 = 1u << 2,
    SW0_MEASURE_CC2 = 1u << 3,
    SW0_CC1_PU     = 1u << 6,
    SW0_CC2_PU     = 1u << 7,

    SW1_TX_CC1     = 1u << 0,
    SW1_TX_CC2     = 1u << 1,
    SW1_AUTO_CRC   = 1u << 2,
    SW1_DATA_DFP   = 1u << 4,
    SW1_PD_REV20   = 1u << 5,
    SW1_POWER_SRC  = 1u << 7,

    CONTROL0_TX_FLUSH  = 1u << 6,
    CONTROL0_INT_MASK  = 1u << 5,
    CONTROL0_RP_DEF    = 1u << 2,
    CONTROL0_RP_1A5    = 2u << 2,

    CONTROL1_RX_FLUSH = 1u << 2,

    CONTROL2_MODE_DRP = 1u << 1,
    CONTROL2_TOGGLE   = 1u << 0,

    CONTROL3_SEND_HARD_RESET = 1u << 6,
    CONTROL3_AUTO_RETRY      = 1u << 0,
    CONTROL3_RETRY_COUNT_3   = 3u << 1,

    RESET_SOFTWARE = 1u << 0,
    POWER_ALL      = 0x0f,

    STATUS0_VBUS_OK = 1u << 7,
    STATUS0_COMP    = 1u << 5,
    STATUS0_CRC_OK  = 1u << 4,
    STATUS1_RX_EMPTY = 1u << 5,

    INTERRUPT_VBUS_OK = 1u << 7,
    INTERRUPT_COMP    = 1u << 5,
    INTERRUPT_CRC     = 1u << 4,

    INTERRUPTA_TOGGLE_DONE = 1u << 6,
    INTERRUPTA_RETRY_FAIL  = 1u << 4,
    INTERRUPTA_TX_SUCCESS  = 1u << 2,
    INTERRUPTA_SOFT_RESET  = 1u << 1,
    INTERRUPTA_HARD_RESET  = 1u << 0,
    INTERRUPTA_FAULT       = 1u << 7,

    INTERRUPTB_GOOD_CRC_SENT = 1u << 0,
};

enum {
    TOKEN_TX_ON   = 0xa1,
    TOKEN_SYNC1   = 0x12,
    TOKEN_SYNC2   = 0x13,
    TOKEN_PACKSYM = 0x80,
    TOKEN_JAM_CRC = 0xff,
    TOKEN_EOP     = 0x14,
    TOKEN_TX_OFF  = 0xfe,
};

static int read_bytes(fusb302_t *device, uint8_t reg, uint8_t *data, size_t length)
{
    if (device == NULL || device->io.read == NULL || data == NULL || length == 0u)
        return -1;
    return device->io.read(device->io.context, device->i2c_address,
                           reg, data, length);
}

static int write_bytes(fusb302_t *device, uint8_t reg,
                       const uint8_t *data, size_t length)
{
    if (device == NULL || device->io.write == NULL || data == NULL || length == 0u)
        return -1;
    return device->io.write(device->io.context, device->i2c_address,
                            reg, data, length);
}

static int read_reg(fusb302_t *device, uint8_t reg, uint8_t *value)
{
    return read_bytes(device, reg, value, 1u);
}

static int write_reg(fusb302_t *device, uint8_t reg, uint8_t value)
{
    return write_bytes(device, reg, &value, 1u);
}

static int update_reg(fusb302_t *device, uint8_t reg,
                      uint8_t clear_mask, uint8_t set_mask)
{
    uint8_t value;
    int rc = read_reg(device, reg, &value);
    if (rc != 0)
        return rc;
    value = (uint8_t)((value & (uint8_t)~clear_mask) | set_mask);
    return write_reg(device, reg, value);
}

static uint8_t active_measure(fusb302_polarity_t polarity)
{
    return polarity == FUSB302_POLARITY_CC2 ? SW0_MEASURE_CC2 : SW0_MEASURE_CC1;
}

static uint8_t active_tx(fusb302_polarity_t polarity)
{
    return polarity == FUSB302_POLARITY_CC2 ? SW1_TX_CC2 : SW1_TX_CC1;
}

int fusb302_init(fusb302_t *device,
                 const fusb302_io_t *io,
                 uint8_t i2c_address)
{
    uint8_t device_id;
    uint8_t control0;
    int rc;

    if (device == NULL || io == NULL || io->read == NULL ||
        io->write == NULL || io->delay_us == NULL)
        return -1;

    memset(device, 0, sizeof(*device));
    device->io = *io;
    device->i2c_address = i2c_address;

    rc = write_reg(device, REG_RESET, RESET_SOFTWARE);
    if (rc != 0)
        return rc;
    device->io.delay_us(device->io.context, 1000u);

    rc = read_reg(device, REG_DEVICE_ID, &device_id);
    if (rc != 0 || (device_id & 0xf0u) == 0u)
        return -1;

    if ((rc = write_reg(device, REG_POWER, POWER_ALL)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_CONTROL3,
                        CONTROL3_RETRY_COUNT_3 | CONTROL3_AUTO_RETRY)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_MASK,
                        (uint8_t)~(INTERRUPT_VBUS_OK | INTERRUPT_COMP |
                                   INTERRUPT_CRC))) != 0)
        return rc;
    if ((rc = write_reg(device, REG_MASKA,
                        (uint8_t)~(INTERRUPTA_TOGGLE_DONE |
                                   INTERRUPTA_RETRY_FAIL |
                                   INTERRUPTA_TX_SUCCESS |
                                   INTERRUPTA_SOFT_RESET |
                                   INTERRUPTA_HARD_RESET |
                                   INTERRUPTA_FAULT))) != 0)
        return rc;
    if ((rc = write_reg(device, REG_MASKB,
                        (uint8_t)~INTERRUPTB_GOOD_CRC_SENT)) != 0)
        return rc;

    rc = read_reg(device, REG_CONTROL0, &control0);
    if (rc != 0)
        return rc;
    control0 &= (uint8_t)~(CONTROL0_INT_MASK | (3u << 2));
    control0 |= device->rp_milliamps >= 1500u ? CONTROL0_RP_1A5
                                              : CONTROL0_RP_DEF;
    return write_reg(device, REG_CONTROL0, control0);
}

int fusb302_set_source_current(fusb302_t *device, uint16_t milliamps)
{
    uint8_t control0;
    int rc;

    if (device == NULL || milliamps == 0u)
        return -1;
    device->rp_milliamps = milliamps;
    rc = read_reg(device, REG_CONTROL0, &control0);
    if (rc != 0)
        return rc;
    control0 &= (uint8_t)~(3u << 2);
    control0 |= milliamps >= 1500u ? CONTROL0_RP_1A5 : CONTROL0_RP_DEF;
    return write_reg(device, REG_CONTROL0, control0);
}

int fusb302_start_drp_toggle(fusb302_t *device)
{
    int rc;
    device->rx_enabled = false;
    device->source_role = false;

    if ((rc = write_reg(device, REG_SWITCHES0, 0u)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_SWITCHES1, SW1_PD_REV20)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_POWER, POWER_ALL)) != 0)
        return rc;
    return write_reg(device, REG_CONTROL2,
                     CONTROL2_MODE_DRP | CONTROL2_TOGGLE);
}

int fusb302_disable(fusb302_t *device)
{
    int rc;

    device->rx_enabled = false;
    device->source_role = false;
    if ((rc = write_reg(device, REG_CONTROL2, CONTROL2_MODE_DRP)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_SWITCHES0, 0u)) != 0)
        return rc;
    return write_reg(device, REG_SWITCHES1, SW1_PD_REV20);
}

int fusb302_set_pd_receiver(fusb302_t *device, bool enable)
{
    uint8_t switches1 = SW1_PD_REV20;
    int rc;

    if (device->source_role)
        switches1 |= SW1_POWER_SRC | SW1_DATA_DFP | active_tx(device->polarity);
    if (enable)
        switches1 |= SW1_AUTO_CRC;

    if (enable) {
        if ((rc = update_reg(device, REG_CONTROL1, 0u,
                             CONTROL1_RX_FLUSH)) != 0)
            return rc;
    }
    rc = write_reg(device, REG_SWITCHES1, switches1);
    if (rc != 0)
        return rc;
    device->rx_enabled = enable;
    return 0;
}

int fusb302_configure_source(fusb302_t *device,
                             fusb302_polarity_t polarity,
                             bool enable_pd_receiver)
{
    int rc;
    device->polarity = polarity;
    device->source_role = true;

    if ((rc = write_reg(device, REG_CONTROL2, CONTROL2_MODE_DRP)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_SWITCHES0,
                        SW0_CC1_PU | SW0_CC2_PU |
                        active_measure(polarity))) != 0)
        return rc;
    /* 0x26 is the 1.6 V attach/detach threshold for Rp = 1.5 A. */
    if ((rc = write_reg(device, REG_MEASURE, 0x26u)) != 0)
        return rc;
    return fusb302_set_pd_receiver(device, enable_pd_receiver);
}

int fusb302_configure_sink(fusb302_t *device,
                           fusb302_polarity_t polarity)
{
    int rc;
    device->polarity = polarity;
    device->source_role = false;
    device->rx_enabled = false;

    if ((rc = write_reg(device, REG_CONTROL2, CONTROL2_MODE_DRP)) != 0)
        return rc;
    if ((rc = write_reg(device, REG_SWITCHES0,
                        SW0_CC1_PD | SW0_CC2_PD | active_measure(polarity))) != 0)
        return rc;
    return write_reg(device, REG_SWITCHES1, SW1_PD_REV20);
}

int fusb302_poll_events(fusb302_t *device, fusb302_events_t *events)
{
    uint8_t interrupt_main;
    uint8_t interrupt_a;
    uint8_t interrupt_b;
    uint8_t status1a;
    uint8_t status0;
    int rc;

    if (events == NULL)
        return -1;
    memset(events, 0, sizeof(*events));

    if ((rc = read_reg(device, REG_INTERRUPT, &interrupt_main)) != 0)
        return rc;
    if ((rc = read_reg(device, REG_INTERRUPTA, &interrupt_a)) != 0)
        return rc;
    if ((rc = read_reg(device, REG_INTERRUPTB, &interrupt_b)) != 0)
        return rc;

    if ((interrupt_a & INTERRUPTA_TOGGLE_DONE) != 0u) {
        events->bits |= FUSB302_EVENT_TOGGLE_DONE;
        if ((rc = read_reg(device, REG_STATUS1A, &status1a)) != 0)
            return rc;
        switch ((status1a >> 3) & 0x7u) {
        case 1: events->toggle_result = FUSB302_TOGGLE_ATTACHED_SOURCE_CC1; break;
        case 2: events->toggle_result = FUSB302_TOGGLE_ATTACHED_SOURCE_CC2; break;
        case 5: events->toggle_result = FUSB302_TOGGLE_ATTACHED_SINK_CC1; break;
        case 6: events->toggle_result = FUSB302_TOGGLE_ATTACHED_SINK_CC2; break;
        case 7: events->toggle_result = FUSB302_TOGGLE_AUDIO_ACCESSORY; break;
        default: events->toggle_result = FUSB302_TOGGLE_NONE; break;
        }
    }
    if ((interrupt_main & INTERRUPT_VBUS_OK) != 0u)
        events->bits |= FUSB302_EVENT_VBUS_CHANGED;
    if ((interrupt_main & INTERRUPT_COMP) != 0u)
        events->bits |= FUSB302_EVENT_CC_CHANGED;
    if ((interrupt_main & INTERRUPT_CRC) != 0u) {
        if ((rc = read_reg(device, REG_STATUS0, &status0)) != 0)
            return rc;
        if ((status0 & STATUS0_CRC_OK) != 0u)
            events->bits |= FUSB302_EVENT_RX_MESSAGE;
        else
            (void)update_reg(device, REG_CONTROL1, 0u, CONTROL1_RX_FLUSH);
    }
    if ((interrupt_b & INTERRUPTB_GOOD_CRC_SENT) != 0u)
        events->bits |= FUSB302_EVENT_RX_MESSAGE;
    if ((interrupt_a & INTERRUPTA_TX_SUCCESS) != 0u)
        events->bits |= FUSB302_EVENT_TX_SUCCESS;
    if ((interrupt_a & INTERRUPTA_RETRY_FAIL) != 0u)
        events->bits |= FUSB302_EVENT_TX_FAILED;
    if ((interrupt_a & INTERRUPTA_HARD_RESET) != 0u)
        events->bits |= FUSB302_EVENT_HARD_RESET;
    if ((interrupt_a & INTERRUPTA_FAULT) != 0u)
        events->bits |= FUSB302_EVENT_FAULT;

    return 0;
}

int fusb302_vbus_present(fusb302_t *device, bool *present)
{
    uint8_t status;
    int rc;
    if (present == NULL)
        return -1;
    rc = read_reg(device, REG_STATUS0, &status);
    if (rc == 0)
        *present = (status & STATUS0_VBUS_OK) != 0u;
    return rc;
}

int fusb302_source_detached(fusb302_t *device, bool *detached)
{
    uint8_t status;
    int rc;
    if (detached == NULL)
        return -1;
    rc = read_reg(device, REG_STATUS0, &status);
    if (rc == 0)
        *detached = (status & STATUS0_COMP) != 0u;
    return rc;
}

int fusb302_receive(fusb302_t *device, usb_pd_message_t *message)
{
    uint8_t status1;
    uint8_t first[3];
    uint8_t tail[USB_PD_MAX_DATA_OBJECTS * 4u + 4u];
    size_t payload_bytes;
    size_t tail_bytes;
    int rc;

    if (message == NULL)
        return -1;
    if ((rc = read_reg(device, REG_STATUS1, &status1)) != 0)
        return rc;
    if ((status1 & STATUS1_RX_EMPTY) != 0u)
        return 1;

    if ((rc = read_bytes(device, REG_FIFOS, first, sizeof(first))) != 0)
        return rc;
    if ((first[0] & 0xe0u) != 0xe0u) {
        (void)update_reg(device, REG_CONTROL1, 0u, CONTROL1_RX_FLUSH);
        return -1;
    }

    message->header = (uint16_t)first[1] | ((uint16_t)first[2] << 8);
    message->data_count = usb_pd_header_count(message->header);
    if (message->data_count > USB_PD_MAX_DATA_OBJECTS)
        return -1;
    payload_bytes = (size_t)message->data_count * 4u;
    tail_bytes = payload_bytes + 4u; /* FUSB302 appends the received CRC. */
    if ((rc = read_bytes(device, REG_FIFOS, tail, tail_bytes)) != 0)
        return rc;

    memset(message->data, 0, sizeof(message->data));
    for (size_t i = 0; i < message->data_count; ++i) {
        const size_t n = i * 4u;
        message->data[i] = (uint32_t)tail[n] |
                           ((uint32_t)tail[n + 1u] << 8) |
                           ((uint32_t)tail[n + 2u] << 16) |
                           ((uint32_t)tail[n + 3u] << 24);
    }
    return 0;
}

int fusb302_transmit(fusb302_t *device, const usb_pd_message_t *message)
{
    uint8_t bytes[40];
    size_t n = 0u;
    size_t packed_length;

    if (device == NULL || message == NULL ||
        message->data_count > USB_PD_MAX_DATA_OBJECTS)
        return -1;

    packed_length = 2u + (size_t)message->data_count * 4u;
    bytes[n++] = TOKEN_SYNC1;
    bytes[n++] = TOKEN_SYNC1;
    bytes[n++] = TOKEN_SYNC1;
    bytes[n++] = TOKEN_SYNC2;
    bytes[n++] = (uint8_t)(TOKEN_PACKSYM | packed_length);
    bytes[n++] = (uint8_t)(message->header & 0xffu);
    bytes[n++] = (uint8_t)(message->header >> 8);
    for (size_t i = 0; i < message->data_count; ++i) {
        const uint32_t value = message->data[i];
        bytes[n++] = (uint8_t)(value & 0xffu);
        bytes[n++] = (uint8_t)((value >> 8) & 0xffu);
        bytes[n++] = (uint8_t)((value >> 16) & 0xffu);
        bytes[n++] = (uint8_t)((value >> 24) & 0xffu);
    }
    bytes[n++] = TOKEN_JAM_CRC;
    bytes[n++] = TOKEN_EOP;
    bytes[n++] = TOKEN_TX_OFF;
    bytes[n++] = TOKEN_TX_ON;
    return write_bytes(device, REG_FIFOS, bytes, n);
}

int fusb302_send_hard_reset(fusb302_t *device)
{
    return update_reg(device, REG_CONTROL3, 0u, CONTROL3_SEND_HARD_RESET);
}
