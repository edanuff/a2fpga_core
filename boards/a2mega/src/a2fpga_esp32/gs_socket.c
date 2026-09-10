/* gs_socket.c — auto-arm for the in-socket 65C816. See gs_socket.h.
 *
 * Sequence (bench G30–G34, 2026-09-09):
 *   MACHINE_OFF : socket shifters fully off (CTRL = 0). Enabling even the
 *                 input shifter across the IIgs power-up keeps the machine
 *                 in its power-on reset (G34), so nothing is touched until
 *                 the slot side shows the machine ALIVE (PHI1 running,
 *                 STATUS.7) and OUT of its own reset (STATUS.2, which also
 *                 reads low while the machine is unpowered).
 *   ARMING      : assert our reset (0x2E.1), listen (CTRL = 4), wait for
 *                 PHI2 alive at the socket, arm (CTRL = 5), release our
 *                 reset -> the core cold-starts on /RES rising. Arming under
 *                 OUR reset is the sequence that worked all along (G30).
 *   ARMED       : stay armed through the machine's own resets; when the
 *                 clock stops (machine off) -> CTRL = 0, back to MACHINE_OFF.
 *   start       : an FPGA that is already armed (ESP32 restart with the
 *                 machine running) is adopted untouched; an alive machine
 *                 with an unarmed socket has no CPU, so it is reset+armed.
 */
#include <stdio.h>
#include "esp_timer.h"
#include "fpga_link.h"
#include "settings.h"
#include "osd_console.h"
#include "a2fpga_regs.h"
#include "gs_socket.h"

#define GLOG(...) do { printf("[gs] " __VA_ARGS__); printf("\n"); osd_log(__VA_ARGS__); } while (0)

/* GS window: CTRL (index 0) bits, STATUS (index 1) bits */
#define GS_CTRL_ARM     0x01
#define GS_CTRL_LISTEN  0x04
#define GS_ST_PH2_ALIVE 0x80
#define GS_ST_RES_N     0x08

#define POLL_US          10000      /* 10 ms sampling */
#define ARMED_POLL_US   100000      /* 100 ms while armed */
#define ALIVE_SAMPLES        10     /* 100 ms of clock before we act */
#define POR_HOLD_US     5000000     /* keep OUR reset asserted this long after the clock appears before
                                       the first release (G37: a release 100 ms after the clock wedges
                                       the IIgs's own reset logic until a power cycle; ~10 s worked) */
#define PH2_SAMPLES           5     /* PHI2 seen at the socket for 50 ms */
#define DEAD_SAMPLES          5     /* clock gone for 500 ms -> machine off */
#define ARMING_TIMEOUT_US 2000000   /* no PHI2 at the socket: back off, retry */

typedef enum { ST_OFF = 0, ST_MACHINE_OFF, ST_POR_RELEASE, ST_ARMING, ST_ARMED, ST_NO_RIBBON, ST_MANUAL } st_t;

static st_t    s_state    = ST_OFF;
static bool    s_started  = false;
static bool    s_release  = false;   /* storage-ready release already requested (0x2E.0) */
static int     s_count    = 0;
static int64_t s_next_us  = 0;
static int64_t s_arming_since_us = 0;
static char    s_str[56]  = "AUTO: OFF";
static uint8_t s_last_st07 = 0;

uint8_t gs_socket_reg_read(uint8_t idx)
{
    uint8_t v;
    fpga_link_lock();
    fpga_reg_write(A2REG_GS_SEL, idx);
    v = fpga_reg_read(A2REG_GS_DATA);
    fpga_link_unlock();
    return v;
}

void gs_socket_reg_write(uint8_t idx, uint8_t val)
{
    fpga_link_lock();
    fpga_reg_write(A2REG_GS_SEL, idx);
    fpga_reg_write(A2REG_GS_DATA, val);
    fpga_link_unlock();
}

/* 0x2E owner: {assert, release} written together so neither side clobbers the other */
static void a2_reset_write(bool assert_hold)
{
    fpga_reg_write(A2REG_A2_RST_RELEASE,
                   (uint8_t)((assert_hold ? A2RST_ASSERT : 0) | (s_release ? A2RST_RELEASE : 0)));
}

void gs_socket_a2_release(void)
{
    s_release = true;
    a2_reset_write(s_state == ST_ARMING || s_state == ST_MACHINE_OFF);   /* keep our hold while we own the reset */
}

static void set_state(st_t st, const char *why)
{
    s_state = st;
    s_count = 0;
    switch (st) {
    case ST_OFF:         snprintf(s_str, sizeof(s_str), "AUTO: OFF"); break;
    case ST_MACHINE_OFF: snprintf(s_str, sizeof(s_str), "AUTO: HOLDING RESET, SOCKET OFF (WAITING FOR CLOCK + 5 S)"); break;
    case ST_NO_RIBBON:   snprintf(s_str, sizeof(s_str), "AUTO: NO PHI2 AT SOCKET - RELEASED, IDLE"); break;
    case ST_POR_RELEASE: snprintf(s_str, sizeof(s_str), "AUTO: MACHINE ALIVE - FIRST RESET RELEASE (SOCKET OFF)"); break;
    case ST_ARMING:      snprintf(s_str, sizeof(s_str), "AUTO: ARMING (MACHINE HELD IN RESET)"); break;
    case ST_ARMED:       snprintf(s_str, sizeof(s_str), "AUTO: ARMED"); break;
    case ST_MANUAL:      snprintf(s_str, sizeof(s_str), "MANUAL (gs auto TO RESUME)"); break;
    }
    if (why) GLOG("GS SOCKET: %s (st=%02X)", why, s_last_st07);
}

const char *gs_socket_state_str(void) { return s_str; }
bool gs_socket_is_manual(void) { return s_state == ST_MANUAL; }
bool gs_socket_ready(void) { return s_started; }

void gs_socket_manual(void)
{
    if (s_state == ST_MANUAL) return;
    if (s_state == ST_ARMING || s_state == ST_MACHINE_OFF) a2_reset_write(false);   /* never leave our hold behind */
    set_state(ST_MANUAL, "MANUAL CONTROL");
}

void gs_socket_resume(void)
{
    s_started = false;
    set_state(ST_OFF, NULL);
}

void gs_socket_apply(void)
{
    if (s_state == ST_MANUAL) return;
    s_started = false;
    set_state(ST_OFF, NULL);
}

static void enter_machine_off(const char *why)
{
    gs_socket_reg_write(0, 0x00);                    /* everything off ... */
    a2_reset_write(true);                            /* ... and hold the slot reset: the IIgs must power
                                                        up under a proper reset (G35), we release it */
    set_state(ST_MACHINE_OFF, why);
}

static void begin_arming(const char *why)
{
    a2_reset_write(true);                            /* (already held) */
    gs_socket_reg_write(0, GS_CTRL_LISTEN);          /* input shifter on, under our reset */
    s_arming_since_us = esp_timer_get_time();
    set_state(ST_ARMING, why);
}

void gs_socket_poll(void)
{
    if (!fpga_link_ok()) return;
    int64_t now = esp_timer_get_time();

    if (!s_started) {
        s_started = true;
        /* STATUS read first: it latches "MCU ready" in the FPGA, whose no-MCU
         * fallback would otherwise drop and later re-assert the Apple II reset. */
        (void)fpga_reg_read(A2REG_STATUS);
        s_release = (fpga_reg_read(A2REG_A2_RST_RELEASE) & A2RST_RELEASE) != 0;
        if (settings()->gs_socket_off) {
            gs_socket_reg_write(0, 0x00);
            set_state(ST_OFF, "DISABLED BY SETTING");
            return;
        }
        uint8_t ctrl = gs_socket_reg_read(0);
        if (ctrl & GS_CTRL_ARM) {
            set_state(ST_ARMED, "ALREADY ARMED - ADOPTED");   /* ESP32 restart, machine running */
            s_next_us = now + ARMED_POLL_US;
            return;
        }
        enter_machine_off("WAITING FOR MACHINE CLOCK");
        s_next_us = now + POLL_US;
        return;
    }
    if (s_state == ST_OFF || s_state == ST_MANUAL) return;
    if (now < s_next_us) return;

    uint8_t st07   = fpga_reg_read(A2REG_STATUS);
    if (!(st07 & 0x01)) return;            /* ready bit clear = bad link read; never act on it */
    bool alive     = (st07 & A2ST_A2_ALIVE) != 0;
    bool a2_rst_hi = (st07 & A2ST_A2_RESET_N) != 0;
    s_last_st07 = st07;

    switch (s_state) {
    case ST_MACHINE_OFF:
        s_next_us = now + POLL_US;
        /* alive AND out of reset: the machine finished its own power-on reset
         * (or our storage hold was released) with the socket untouched */
        (void)a2_rst_hi;                          /* low: we are holding it */
        if (!alive) { s_count = 0; break; }
        if (s_count == 0) s_arming_since_us = now; /* clock just appeared: start the POR hold timer */
        s_count++;
        if (now - s_arming_since_us >= POR_HOLD_US) {
            /* The machine's FIRST reset release after power-up must happen with
             * the socket shifters OFF (G36: enabling them before it leaves the
             * machine stuck in reset); later resets with the socket on are fine. */
            a2_reset_write(false);
            set_state(ST_POR_RELEASE, "MACHINE ALIVE - RELEASING RESET (SOCKET OFF)");
            s_arming_since_us = now;
        }
        break;

    case ST_POR_RELEASE:
        s_next_us = now + POLL_US;
        if (!alive) { enter_machine_off("CLOCK LOST"); break; }
        s_count = a2_rst_hi ? s_count + 1 : 0;
        if (s_count >= ALIVE_SAMPLES) {
            begin_arming("MACHINE OUT OF RESET - RE-ASSERTING, LISTENING");
        } else if (now - s_arming_since_us > ARMING_TIMEOUT_US) {
            GLOG("GS SOCKET: MACHINE RESET DID NOT RISE - RETRYING (st=%02X)", st07);
            enter_machine_off("RETRY");
        }
        break;

    case ST_ARMING: {
        s_next_us = now + POLL_US;
        if (!alive) { a2_reset_write(false); enter_machine_off("CLOCK LOST WHILE ARMING"); break; }
        uint8_t gs = gs_socket_reg_read(1);
        s_count = (gs & GS_ST_PH2_ALIVE) ? s_count + 1 : 0;
        if (s_count >= PH2_SAMPLES) {
            gs_socket_reg_write(0, GS_CTRL_LISTEN | GS_CTRL_ARM);
            a2_reset_write(false);                   /* release: core cold-starts */
            set_state(ST_ARMED, "PHI2 ALIVE - ARMED, RESET RELEASED");
            s_next_us = now + ARMED_POLL_US;
        } else if (now - s_arming_since_us > ARMING_TIMEOUT_US) {
            gs_socket_reg_write(0, 0x00);
            a2_reset_write(false);                   /* no ribbon: behave like a plain card */
            set_state(ST_NO_RIBBON, "NO PHI2 AT THE SOCKET - RESET RELEASED, IDLE");
            s_next_us = now + ARMED_POLL_US;
        }
        break;
    }

    case ST_NO_RIBBON:
        s_next_us = now + ARMED_POLL_US;
        s_count = alive ? 0 : s_count + 1;
        if (s_count >= DEAD_SAMPLES)
            enter_machine_off("MACHINE CLOCK STOPPED - HOLDING RESET");
        break;

    case ST_ARMED:
        s_next_us = now + ARMED_POLL_US;
        s_count = alive ? 0 : s_count + 1;
        if (s_count >= DEAD_SAMPLES)
            enter_machine_off("MACHINE CLOCK STOPPED - SOCKET OFF, HOLDING RESET");
        break;

    default:
        break;
    }
}
