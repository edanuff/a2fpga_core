/*
 * mock_fusb302.c — host-test double for the FUSB302 driver. Implements
 * the full fusb302.h API against test-controllable state so usbc_port.c
 * compiles and runs unmodified on the host.
 */
#include <string.h>

#include "fusb302.h"
#include "mock_fusb302.h"

mock_fusb302_state_t mock_fusb;

void mock_fusb302_reset(void)
{
    memset(&mock_fusb, 0, sizeof(mock_fusb));
    mock_fusb.verify_intact = true;
    mock_fusb.vbus_present  = true;
}

int fusb302_init(fusb302_t *device, const fusb302_io_t *io,
                 uint8_t i2c_address)
{
    (void)io;
    device->i2c_address = i2c_address;
    device->source_role = false;
    mock_fusb.init_calls++;
    return 0;
}

int fusb302_set_source_current(fusb302_t *device, uint16_t milliamps)
{
    (void)device; (void)milliamps;
    return 0;
}

int fusb302_disable(fusb302_t *device)
{
    device->rx_enabled = false;
    device->source_role = false;
    mock_fusb.disable_calls++;
    return 0;
}

int fusb302_start_drp_toggle(fusb302_t *device)
{
    (void)device;
    mock_fusb.toggle_calls++;
    return 0;
}

int fusb302_configure_source(fusb302_t *device, fusb302_polarity_t polarity,
                             bool enable_pd_receiver)
{
    (void)polarity; (void)enable_pd_receiver;
    device->source_role = true;
    return 0;
}

int fusb302_detect_source_orientation(fusb302_t *device,
                                      fusb302_polarity_t *polarity,
                                      bool *found)
{
    (void)device;
    *polarity = FUSB302_POLARITY_CC1;
    *found = true;
    return 0;
}

int fusb302_configure_sink(fusb302_t *device, fusb302_polarity_t polarity)
{
    (void)polarity;
    device->source_role = false;
    return 0;
}

int fusb302_set_pd_receiver(fusb302_t *device, bool enable)
{
    device->rx_enabled = enable;
    return 0;
}

int fusb302_set_data_role(fusb302_t *device, bool dfp)
{
    device->data_role_dfp = dfp;
    return 0;
}

int fusb302_poll_events(fusb302_t *device, fusb302_events_t *events)
{
    (void)device;
    events->bits = mock_fusb.next_events;
    events->toggle_result = mock_fusb.toggle_result;
    mock_fusb.next_events = 0;
    return 0;
}

int fusb302_vbus_present(fusb302_t *device, bool *present)
{
    (void)device;
    *present = mock_fusb.vbus_present;
    return 0;
}

int fusb302_source_detached(fusb302_t *device, bool *detached)
{
    (void)device;
    *detached = mock_fusb.source_detached;
    return 0;
}

int fusb302_receive(fusb302_t *device, usb_pd_message_t *message)
{
    (void)device;
    if (!mock_fusb.rx_pending)
        return 1;                       /* FIFO empty */
    *message = mock_fusb.rx_message;
    mock_fusb.rx_pending = false;
    return 0;
}

int fusb302_transmit(fusb302_t *device, const usb_pd_message_t *message)
{
    (void)device;
    mock_fusb.tx_calls++;
    mock_fusb.last_tx = *message;
    return 0;
}

int fusb302_send_hard_reset(fusb302_t *device)
{
    (void)device;
    return 0;
}

int fusb302_verify_powered(fusb302_t *device, bool *intact)
{
    (void)device;
    mock_fusb.verify_calls++;
    *intact = mock_fusb.verify_intact;
    return 0;
}
