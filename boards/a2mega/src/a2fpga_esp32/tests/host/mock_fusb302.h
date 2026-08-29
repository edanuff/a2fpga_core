#ifndef MOCK_FUSB302_H
#define MOCK_FUSB302_H

#include <stdbool.h>
#include <stdint.h>

#include "fusb302.h"

typedef struct {
    uint32_t next_events;              /* consumed by next poll */
    fusb302_toggle_result_t toggle_result;
    bool     vbus_present;
    bool     source_detached;
    bool     verify_intact;            /* fusb302_verify_powered result */
    bool     cc_present;               /* fusb302_requalify_cc results */
    bool     cc_moved;
    int      requalify_calls;
    bool     rx_pending;
    usb_pd_message_t rx_message;
    usb_pd_message_t last_tx;
    int init_calls, disable_calls, toggle_calls, tx_calls, verify_calls;
} mock_fusb302_state_t;

extern mock_fusb302_state_t mock_fusb;

void mock_fusb302_reset(void);

#endif
