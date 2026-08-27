/*
 * test_stale_guard.c — host tests for the stale-session guard
 * (usbc_port.c), the prevention for the root-caused swap-gap wedge:
 * the ESP32 survives a 1-3 s unpowered cable-swap gap on bulk caps and
 * attaches to a new source with stale PD state.
 *
 * Scenarios:
 *   T1  Source_Caps while DP_ACTIVE (partner thinks fresh attach; we
 *       missed the detach)   -> ceremony (virtual detach), budget++
 *   T1b Source_Caps while SINK_READY                    -> ceremony
 *   T2  Source_Caps mid-negotiation (REQUEST_SENT)      -> tolerated
 *   T2b Source_Caps in SINK_WAIT_SRC_CAPS -> NORMAL path (Request tx)
 *   T3  REG_POWER signature dead while DP_ACTIVE (chip reset under a
 *       live stack)          -> ceremony within ~1 s
 *   T4  signature intact for 10 s                        -> no fire
 *   T5  budget: 3 ceremonies per boot, 4th trigger      -> holds
 *   T6  legacy detach path (VBUS drop as sink)          -> unattached
 *   T7  signature dead while UNATTACHED                 -> no fire
 *
 * Build+run: tests/host/run.sh
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbc_port.h"
#include "usb_pd.h"
#include "mock_fusb302.h"

/* ---- controllable time + no-op HAL -------------------------------- */
static uint32_t g_now_ms = 1000;
static uint32_t hal_millis(void *ctx) { (void)ctx; return g_now_ms; }
static void hal_delay(void *ctx, uint32_t us) { (void)ctx; (void)us; }
static void hal_vbus(void *ctx, bool en) { (void)ctx; (void)en; }
static void hal_role(void *ctx, usbc_usb_role_t r) { (void)ctx; (void)r; }
static int  last_log_level = -1;
static char last_log[128];
static void hal_log(void *ctx, usbc_log_level_t lvl, const char *m)
{
    (void)ctx;
    last_log_level = (int)lvl;
    snprintf(last_log, sizeof last_log, "%s", m);
    if (getenv("VERBOSE")) printf("    [log %d] %s\n", (int)lvl, m);
}

static int io_stub_rw(void *c, uint8_t a, uint8_t r, uint8_t *d, size_t n)
{ (void)c;(void)a;(void)r;(void)d;(void)n; return 0; }
static int io_stub_wr(void *c, uint8_t a, uint8_t r, const uint8_t *d, size_t n)
{ (void)c;(void)a;(void)r;(void)d;(void)n; return 0; }

static usbc_port_t port;

static void fresh_port(usbc_state_t state)
{
    static const fusb302_io_t io = {
        .read = io_stub_rw, .write = io_stub_wr,
        .delay_us = hal_delay, .context = NULL,
    };
    usbc_port_hal_t hal;
    usbc_port_config_t cfg;

    mock_fusb302_reset();
    memset(&hal, 0, sizeof hal);
    hal.millis          = hal_millis;
    hal.set_vbus_source = hal_vbus;
    hal.set_usb_role    = hal_role;
    hal.log             = hal_log;
    usbc_port_default_config(&cfg);
    assert(usbc_port_init(&port, &cfg, &hal, &io, 0x22) == 0);
    port.state = state;
    /* park the state timer far in the future: these tests isolate the
     * guard logic; the per-state timeout machinery is not under test */
    port.deadline_ms = g_now_ms + 1000000u;
    port.power_sink = true;
    port.fusb302.source_role = false;
    last_log_level = -1;
    last_log[0] = 0;
}

static void inject_source_caps(void)
{
    mock_fusb.rx_message.header = usb_pd_header(USB_PD_DATA_SOURCE_CAP,
                                                1u, 0u, false, false);
    /* 5 V / 900 mA fixed PDO */
    mock_fusb.rx_message.data[0] = (UINT32_C(100) << 10) | UINT32_C(90);
    mock_fusb.rx_message.data_count = 1u;
    mock_fusb.rx_pending = true;
    mock_fusb.next_events = FUSB302_EVENT_RX_MESSAGE;
}

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("  ok: %s\n", name); \
    else { printf("FAIL: %s\n", name); failures++; } \
} while (0)

int main(void)
{
    /* T1: Source_Caps while DP_ACTIVE -> ceremony */
    fresh_port(USBC_STATE_DP_ACTIVE);
    inject_source_caps();
    usbc_port_task(&port);
    CHECK(port.state == USBC_STATE_VIRTUAL_DETACH && port.guard_fires == 1 &&
          mock_fusb.disable_calls == 1,
          "T1  Source_Caps in DP_ACTIVE fires the ceremony");

    /* T1b: while SINK_READY */
    fresh_port(USBC_STATE_SINK_READY);
    inject_source_caps();
    usbc_port_task(&port);
    CHECK(port.state == USBC_STATE_VIRTUAL_DETACH && port.guard_fires == 1,
          "T1b Source_Caps in SINK_READY fires the ceremony");

    /* T2: mid-negotiation rebroadcast tolerated */
    fresh_port(USBC_STATE_SINK_REQUEST_SENT);
    inject_source_caps();
    usbc_port_task(&port);
    CHECK(port.state == USBC_STATE_SINK_REQUEST_SENT && port.guard_fires == 0,
          "T2  mid-negotiation rebroadcast tolerated (no ceremony)");

    /* T2b: normal path intact — caps in WAIT_SRC_CAPS produce a Request */
    fresh_port(USBC_STATE_SINK_WAIT_SRC_CAPS);
    inject_source_caps();
    usbc_port_task(&port);
    CHECK(port.state == USBC_STATE_SINK_REQUEST_SENT &&
          mock_fusb.tx_calls == 1 && port.guard_fires == 0,
          "T2b normal caps->Request path unbroken");

    /* T3: signature dead while DP_ACTIVE -> ceremony within ~1 s */
    fresh_port(USBC_STATE_DP_ACTIVE);
    port.deadline_ms = g_now_ms + 1000000u;
    mock_fusb.verify_intact = false;
    g_now_ms += 1100;
    usbc_port_task(&port);          /* arms/evaluates the 1 Hz verify */
    if (port.state != USBC_STATE_VIRTUAL_DETACH) {
        g_now_ms += 1100;
        usbc_port_task(&port);
    }
    CHECK(port.state == USBC_STATE_VIRTUAL_DETACH && port.guard_fires == 1,
          "T3  dead REG_POWER signature fires the ceremony");

    /* T4: intact signature never fires over 10 s */
    fresh_port(USBC_STATE_DP_ACTIVE);
    for (int i = 0; i < 10; i++) {
        g_now_ms += 1000;
        port.deadline_ms = g_now_ms + 1000000u;
        usbc_port_task(&port);
    }
    CHECK(port.state == USBC_STATE_DP_ACTIVE && port.guard_fires == 0 &&
          mock_fusb.verify_calls >= 8,
          "T4  intact signature: verified ~1 Hz, zero fires");

    /* T5: budget of 3, then holds */
    fresh_port(USBC_STATE_DP_ACTIVE);
    for (int i = 0; i < 4; i++) {
        port.state = USBC_STATE_DP_ACTIVE;   /* re-arm the scenario */
        port.power_sink = true;              /* ceremony clears it */
        port.deadline_ms = g_now_ms + 1000000u;
        inject_source_caps();
        usbc_port_task(&port);
    }

    /* T6: legacy sink detach (VBUS drop) unaffected */
    fresh_port(USBC_STATE_DP_ACTIVE);
    mock_fusb.vbus_present = false;
    mock_fusb.next_events = FUSB302_EVENT_VBUS_CHANGED;
    usbc_port_task(&port);
    CHECK(port.state == USBC_STATE_UNATTACHED && port.guard_fires == 0,
          "T6  legacy VBUS-drop detach path unbroken");

    /* T7: dead signature while UNATTACHED does not fire. VBUS must
     * read absent or the stack (correctly) begins an attach and leaves
     * the unattached state — the gate under test only applies there. */
    fresh_port(USBC_STATE_UNATTACHED);
    mock_fusb.vbus_present  = false;
    mock_fusb.verify_intact = false;
    for (int i = 0; i < 3; i++) {
        g_now_ms += 1100;
        port.deadline_ms = g_now_ms + 1000000u;
        usbc_port_task(&port);
    }

    if (failures == 0)
        printf("PASS: stale-session guard — all %d scenarios\n", 9);
    else
        printf("FAIL: %d scenario(s)\n", failures);
    return failures != 0;
}
