// SPDX-License-Identifier: MIT
#ifndef DISPLAYPORT_VERILOG_FUSB302_H
#define DISPLAYPORT_VERILOG_FUSB302_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "usb_pd.h"

#define FUSB302_I2C_ADDRESS_DEFAULT UINT8_C(0x22)

typedef int (*fusb302_i2c_read_fn)(void *context,
                                   uint8_t address,
                                   uint8_t reg,
                                   uint8_t *data,
                                   size_t length);
typedef int (*fusb302_i2c_write_fn)(void *context,
                                    uint8_t address,
                                    uint8_t reg,
                                    const uint8_t *data,
                                    size_t length);
typedef void (*fusb302_delay_us_fn)(void *context, uint32_t microseconds);

typedef struct {
    void *context;
    fusb302_i2c_read_fn read;
    fusb302_i2c_write_fn write;
    fusb302_delay_us_fn delay_us;
} fusb302_io_t;

typedef enum {
    FUSB302_POLARITY_CC1 = 0,
    FUSB302_POLARITY_CC2 = 1,
} fusb302_polarity_t;

typedef enum {
    FUSB302_TOGGLE_NONE = 0,
    FUSB302_TOGGLE_ATTACHED_SOURCE_CC1,
    FUSB302_TOGGLE_ATTACHED_SOURCE_CC2,
    FUSB302_TOGGLE_ATTACHED_SINK_CC1,
    FUSB302_TOGGLE_ATTACHED_SINK_CC2,
    FUSB302_TOGGLE_AUDIO_ACCESSORY,
} fusb302_toggle_result_t;

enum fusb302_event_bits {
    FUSB302_EVENT_NONE          = 0,
    FUSB302_EVENT_TOGGLE_DONE   = 1u << 0,
    FUSB302_EVENT_VBUS_CHANGED  = 1u << 1,
    FUSB302_EVENT_CC_CHANGED    = 1u << 2,
    FUSB302_EVENT_RX_MESSAGE    = 1u << 3,
    FUSB302_EVENT_TX_SUCCESS    = 1u << 4,
    FUSB302_EVENT_TX_FAILED     = 1u << 5,
    FUSB302_EVENT_HARD_RESET    = 1u << 6,
    FUSB302_EVENT_FAULT         = 1u << 7,
};

typedef struct {
    uint32_t bits;
    fusb302_toggle_result_t toggle_result;
} fusb302_events_t;

typedef struct {
    fusb302_io_t io;
    uint8_t i2c_address;
    fusb302_polarity_t polarity;
    bool source_role;
    bool rx_enabled;
    uint16_t rp_milliamps;   /* advertised Type-C source current tier */
} fusb302_t;

int fusb302_init(fusb302_t *device,
                 const fusb302_io_t *io,
                 uint8_t i2c_address);
/* Select the Rp advertisement from the configured source budget:
 * >= 1500 mA advertises the 1.5 A tier, anything lower advertises
 * default USB current (500 mA). Call after fusb302_init. */
int fusb302_set_source_current(fusb302_t *device, uint16_t milliamps);
int fusb302_disable(fusb302_t *device);
int fusb302_start_drp_toggle(fusb302_t *device);
int fusb302_configure_source(fusb302_t *device,
                             fusb302_polarity_t polarity,
                             bool enable_pd_receiver);
int fusb302_configure_sink(fusb302_t *device,
                           fusb302_polarity_t polarity);
int fusb302_set_pd_receiver(fusb302_t *device, bool enable);
int fusb302_poll_events(fusb302_t *device, fusb302_events_t *events);
int fusb302_vbus_present(fusb302_t *device, bool *present);
int fusb302_source_detached(fusb302_t *device, bool *detached);
int fusb302_receive(fusb302_t *device, usb_pd_message_t *message);
int fusb302_transmit(fusb302_t *device, const usb_pd_message_t *message);
int fusb302_send_hard_reset(fusb302_t *device);

#endif
