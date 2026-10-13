/*
 * test_dr_swap.c — host tests for the sink+UFP -> DFP Data-Role Swap
 * (usbc_port.c). Scenario: a powered USB-C->HDMI adapter (Cable Matters
 * with its own power input, a2p25 plan 6h, 2026-10-12) sources VBUS, so
 * our DRP attaches as sink + UFP. Only the DFP runs the DP alt-mode
 * ladder, so after the explicit 5 V contract we request DR_Swap, become
 * DFP on Accept and run the SAME Discover Identity ... DP Configure path
 * the source+DFP attach uses. Power role stays sink throughout.
 *
 *   D1  PS_RDY -> DR_Swap sent ~10 ms later, header = sink + UFP
 *   D2  Accept -> data role DFP, Discover Identity sent with DFP header
 *   D3  Wait -> second DR_Swap after tDRSwapWait (100 ms); Accept works
 *   D4  Reject -> USB device, ONE error log, no further TX for 3 s
 *   D4b Not_Supported (PD3 reply) handled like Reject
 *   D5  silence -> 3 sends (1 + 2 retries), then USB device, one log
 *   D6  Accept BEFORE our TX_SUCCESS (fast partner) -> tx not stuck busy
 *   D7  partner-initiated DR_Swap -> we Accept, become DFP, discover
 *   D8  Wait storm (10x) -> give up once, one error log
 *   D9  source+DFP path untouched: sink partner attach -> Source_Caps
 *       with source + DFP header, no DR_Swap machinery involved
 *   D10 after give-up a fresh Source_Caps -> new contract re-arms swap
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

static uint32_t g_now_ms = 1000;
static uint32_t hal_millis(void *ctx) { (void)ctx; return g_now_ms; }
static void hal_delay(void *ctx, uint32_t us) { (void)ctx; (void)us; }
static void hal_vbus(void *ctx, bool en) { (void)ctx; (void)en; }
static void hal_role(void *ctx, usbc_usb_role_t r) { (void)ctx; (void)r; }
static int  error_logs = 0;
static char last_log[128];
static void hal_log(void *ctx, usbc_log_level_t lvl, const char *m)
{
    (void)ctx;
    if (lvl == USBC_LOG_ERROR) error_logs++;
    snprintf(last_log, sizeof last_log, "%s", m);
    if (getenv("VERBOSE")) printf("    [%lu log %d] %s\n",
                                  (unsigned long)g_now_ms, (int)lvl, m);
}
static int io_stub_rw(void *c, uint8_t a, uint8_t r, uint8_t *d, size_t n)
{ (void)c;(void)a;(void)r;(void)d;(void)n; return 0; }
static int io_stub_wr(void *c, uint8_t a, uint8_t r, const uint8_t *d, size_t n)
{ (void)c;(void)a;(void)r;(void)d;(void)n; return 0; }

static usbc_port_t port;

/* A port that just received Accept for its Request and awaits PS_RDY
 * (the real attach ladder up to there is exercised by the source-caps
 * test in test_stale_guard.c). */
static void fresh_sink_awaiting_ps_rdy(void)
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
    port.state = USBC_STATE_SINK_WAIT_PS_RDY;
    port.deadline_ms = g_now_ms + 600u;
    port.power_sink = true;
    port.data_dfp = false;
    port.fusb302.source_role = false;
    port.fusb302.rx_enabled = true;
    error_logs = 0;
    last_log[0] = 0;
}

/* 1 ms task cadence, like the ESP32 task. */
static void tick(uint32_t ms)
{
    while (ms--) {
        g_now_ms++;
        usbc_port_task(&port);
    }
}

static uint8_t g_rx_id = 0;   /* shared so the dedupe filter never bites */

static void inject_ctrl(uint8_t type)
{
    /* partner = source + DFP */
    mock_fusb.rx_message.header = usb_pd_header(type, 0u, g_rx_id++ & 7u, true, true);
    mock_fusb.rx_message.data_count = 0u;
    mock_fusb.rx_pending = true;
    mock_fusb.next_events = FUSB302_EVENT_RX_MESSAGE;
    usbc_port_task(&port);
}

static void inject_source_caps(void)
{
    mock_fusb.rx_message.header = usb_pd_header(USB_PD_DATA_SOURCE_CAP,
                                                1u, g_rx_id++ & 7u, true, true);
    mock_fusb.rx_message.data[0] = (UINT32_C(100) << 10) | UINT32_C(90);
    mock_fusb.rx_message.data_count = 1u;
    mock_fusb.rx_pending = true;
    mock_fusb.next_events = FUSB302_EVENT_RX_MESSAGE;
    usbc_port_task(&port);
}

static void tx_ok(void)
{
    mock_fusb.next_events = FUSB302_EVENT_TX_SUCCESS;
    usbc_port_task(&port);
}

static uint8_t last_tx_type(void) { return usb_pd_header_type(mock_fusb.last_tx.header); }
static bool last_tx_src(void)  { return (mock_fusb.last_tx.header >> 8) & 1u; }
static bool last_tx_dfp(void)  { return (mock_fusb.last_tx.header >> 5) & 1u; }
static bool last_tx_is_vdm(uint8_t cmd)
{
    return last_tx_type() == USB_PD_DATA_VENDOR &&
           mock_fusb.last_tx.data_count >= 1u &&
           usb_pd_svdm_command(mock_fusb.last_tx.data[0]) == cmd;
}

/* contract -> our DR_Swap on the wire, TXOK'd */
static void contract_then_dr_swap(void)
{
    fresh_sink_awaiting_ps_rdy();
    inject_ctrl(USB_PD_CTRL_PS_RDY);
    tick(12);
    assert(last_tx_type() == USB_PD_CTRL_DR_SWAP);
    tx_ok();
}

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("  ok: %s\n", name); \
    else { printf("FAIL: %s (last log: %s)\n", name, last_log); failures++; } \
} while (0)

int main(void)
{
    /* D1 */
    fresh_sink_awaiting_ps_rdy();
    inject_ctrl(USB_PD_CTRL_PS_RDY);
    CHECK(port.state == USBC_STATE_SINK_READY && mock_fusb.tx_calls == 0,
          "D1  PS_RDY -> sink-ready, nothing sent yet");
    tick(12);
    CHECK(mock_fusb.tx_calls == 1 && last_tx_type() == USB_PD_CTRL_DR_SWAP &&
          !last_tx_src() && !last_tx_dfp() &&
          port.state == USBC_STATE_SINK_DR_SWAP_SENT && port.dr_swap_attempts == 1,
          "D1  DR_Swap sent ~10 ms after PS_RDY with sink+UFP header");

    /* D2 */
    tx_ok();
    inject_ctrl(USB_PD_CTRL_ACCEPT);
    CHECK(port.data_dfp && port.fusb302.data_role_dfp && port.power_sink &&
          port.state == USBC_STATE_SINK_READY,
          "D2  Accept -> DFP data role, sink power role kept");
    tick(12);
    CHECK(mock_fusb.tx_calls == 2 && last_tx_is_vdm(USB_PD_SVDM_DISCOVER_IDENTITY) &&
          !last_tx_src() && last_tx_dfp() &&
          port.state == USBC_STATE_VDM_WAIT_IDENTITY,
          "D2  Discover Identity follows with sink+DFP header (shared ladder)");

    /* D3 */
    contract_then_dr_swap();
    inject_ctrl(USB_PD_CTRL_WAIT);
    CHECK(port.state == USBC_STATE_SINK_READY && !port.data_dfp && error_logs == 0,
          "D3  Wait -> back to sink-ready, no error");
    tick(50);
    CHECK(mock_fusb.tx_calls == 1, "D3  no resend before tDRSwapWait");
    tick(60);
    CHECK(mock_fusb.tx_calls == 2 && last_tx_type() == USB_PD_CTRL_DR_SWAP &&
          port.state == USBC_STATE_SINK_DR_SWAP_SENT && port.dr_swap_attempts == 2,
          "D3  second DR_Swap ~100 ms after Wait");
    tx_ok();
    inject_ctrl(USB_PD_CTRL_ACCEPT);
    tick(12);
    CHECK(port.data_dfp && last_tx_is_vdm(USB_PD_SVDM_DISCOVER_IDENTITY),
          "D3  Accept after Wait -> DFP + discovery");

    /* D4 */
    contract_then_dr_swap();
    inject_ctrl(USB_PD_CTRL_REJECT);
    CHECK(port.state == USBC_STATE_DEVICE && !port.data_dfp && port.power_sink &&
          error_logs == 1,
          "D4  Reject -> USB device, one error log");
    tick(3000);
    CHECK(mock_fusb.tx_calls == 1 && port.state == USBC_STATE_DEVICE && error_logs == 1,
          "D4  no further DR_Swap for 3 s, no more logs");

    /* D4b */
    contract_then_dr_swap();
    inject_ctrl(USB_PD_CTRL_NOT_SUPPORTED);
    CHECK(port.state == USBC_STATE_DEVICE && error_logs == 1,
          "D4b Not_Supported handled like Reject");

    /* D5 */
    contract_then_dr_swap();
    tick(2000);
    CHECK(mock_fusb.tx_calls == 3 && port.state == USBC_STATE_DEVICE &&
          error_logs == 1 && !port.data_dfp && !port.tx_busy,
          "D5  silence -> 1 + 2 retries, then USB device, one error log");

    /* D6 */
    fresh_sink_awaiting_ps_rdy();
    inject_ctrl(USB_PD_CTRL_PS_RDY);
    tick(12);
    assert(port.tx_busy && port.tx_kind == USBC_TX_DR_SWAP);
    inject_ctrl(USB_PD_CTRL_ACCEPT);        /* before TX_SUCCESS */
    CHECK(!port.tx_busy && port.data_dfp, "D6  Accept before TXOK completes the send");
    tx_ok();                                /* late TXOK is a no-op */
    tick(12);
    CHECK(mock_fusb.tx_calls == 2 && last_tx_is_vdm(USB_PD_SVDM_DISCOVER_IDENTITY) &&
          port.state == USBC_STATE_VDM_WAIT_IDENTITY,
          "D6  discovery not refused by a stale tx_busy");

    /* D7 */
    fresh_sink_awaiting_ps_rdy();
    inject_ctrl(USB_PD_CTRL_PS_RDY);
    tick(2);                                /* before our own DR_Swap */
    inject_ctrl(USB_PD_CTRL_DR_SWAP);
    CHECK(mock_fusb.tx_calls == 1 && last_tx_type() == USB_PD_CTRL_ACCEPT &&
          port.tx_kind == USBC_TX_ACCEPT_DR_SWAP,
          "D7  partner DR_Swap -> we Accept");
    tx_ok();
    tick(12);
    CHECK(port.data_dfp && last_tx_is_vdm(USB_PD_SVDM_DISCOVER_IDENTITY),
          "D7  partner-initiated swap -> DFP + discovery");

    /* D8 */
    contract_then_dr_swap();
    for (int i = 0; i < 12 && port.state != USBC_STATE_DEVICE; i++) {
        inject_ctrl(USB_PD_CTRL_WAIT);
        tick(110);
        if (port.state == USBC_STATE_SINK_DR_SWAP_SENT) tx_ok();
    }
    CHECK(port.state == USBC_STATE_DEVICE && error_logs == 1 &&
          mock_fusb.tx_calls == 10,
          "D8  Wait storm -> 10 attempts then give up once");

    /* D9: source path — the toggle found Rd (a bus-powered hub/monitor),
     * so WE source VBUS and are DFP (FUSB302 "attached as source"). */
    fresh_sink_awaiting_ps_rdy();
    port.state = USBC_STATE_UNATTACHED;
    port.power_sink = false; port.data_dfp = true;
    mock_fusb.toggle_result = FUSB302_TOGGLE_ATTACHED_SOURCE_CC1;
    mock_fusb.next_events = FUSB302_EVENT_TOGGLE_DONE;
    usbc_port_task(&port);
    CHECK(port.state == USBC_STATE_SOURCE_WAIT_VBUS && !port.power_sink,
          "D9  sink partner -> source attach");
    tick(40);
    CHECK(mock_fusb.tx_calls == 1 && last_tx_type() == USB_PD_DATA_SOURCE_CAP &&
          last_tx_src() && last_tx_dfp() && port.dr_swap_attempts == 0,
          "D9  Source_Caps with source+DFP header; swap machinery idle");

    /* D10 */
    contract_then_dr_swap();
    inject_ctrl(USB_PD_CTRL_REJECT);
    assert(port.state == USBC_STATE_DEVICE && error_logs == 1);
    inject_source_caps();                   /* partner renegotiates */
    CHECK(port.state == USBC_STATE_SINK_REQUEST_SENT &&
          last_tx_type() == USB_PD_DATA_REQUEST,
          "D10 Source_Caps after give-up -> new Request");
    tx_ok();
    inject_ctrl(USB_PD_CTRL_ACCEPT);
    inject_ctrl(USB_PD_CTRL_PS_RDY);
    tick(12);
    CHECK(last_tx_type() == USB_PD_CTRL_DR_SWAP && port.dr_swap_attempts == 1 &&
          !port.dr_swap_given_up,
          "D10 new contract re-arms the DR_Swap budget");

    if (failures) { printf("FAIL: %d scenario(s)\n", failures); return 1; }
    printf("PASS: DR_Swap — all scenarios\n");
    return 0;
}
