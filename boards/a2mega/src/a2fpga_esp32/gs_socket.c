/* gs_socket.c — auto-arm for the in-socket 65C816. See gs_socket.h.
 *
 * v8 (2026-09-11). Bench rules measured on the ROM 01 IIgs (G31–G44):
 *   - the machine must power up under our slot-reset hold (card powered first
 *     and nothing holding: its own power-on reset never completes, G35);
 *   - a release ~100 ms after the clock wedges it; a second reset pulse right
 *     after the first wedges it (G37, G39);
 *   - the FPGA's POR probe finds the machine's own release (~230 ms after the
 *     clock = the M50740's 200 k/1 uF power-on RC) and can arm in that clock.
 * The TransWarp GS reference (scoping §3.3/§8b): the socket CPU's reset is
 * the machine's own /RES, the card's buffers are up from configuration, and
 * nothing is switched at release time. Mode 1 below is that model.
 *
 * Bring-up modes (`gs mode <n>`, default GS_MODE_DEFAULT):
 *   0 NATURAL  hold -> clock -> FPGA probe; FPGA arms at the machine's own release (v7)
 *   1 EARLY    hold -> clock -> FPGA arms UNDER our hold (core in reset on a driven
 *              bus, like a real chip) -> probe -> release at the machine's own end
 *   2 TIMED    hold -> clock -> hold_ms -> release, socket off -> slot reset high -> arm (v6)
 *   3 TIMED+   hold -> clock -> arm under our hold -> hold_ms -> release
 *   4 AUTO     clock already running when we started (slot-powered start: the machine
 *              has been held by the 2G06 since power-on and its own release/re-assert
 *              window at ~283/293 ms is long past) -> mode 1; clock appears later (card
 *              alive before the machine) -> mode 3 with a 1 s hold, which covers that
 *              window (G45-G49: released inside it, this IIgs re-asserts and never lets go)
 * The ESP32 only sequences; the FPGA does the microsecond parts (0x2E bits
 * 2/3/4, connector). An FPGA already armed on ESP32 start is adopted. Storage
 * gating stays: the FPGA will not release until the disk task has written the
 * storage-ready bit.
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
#define GS_CTRL_KEEP    0x50      /* bench trigger bits (4 trig-en, 6 address mode) survive our writes */
#define GS_ST_PH2_ALIVE 0x80

#define POLL_US           10000     /* 10 ms sampling */
#define ARMED_POLL_US    100000     /* 100 ms while armed */
#define ALIVE_SAMPLES        10     /* 100 ms of clock before we act */
#define DEAD_SAMPLES          5     /* clock gone for 500 ms -> machine off */
#define NO_RIBBON_US    2000000     /* armed but no PHI2 at the socket for 2 s: plain card */
#define PROBE_REPORT_US 15000000    /* log if the machine never lets go of its reset */
#define RISE_REPORT_US   3000000    /* mode 2: log if the slot reset never rises after our release */

#ifndef GS_MODE_DEFAULT
#define GS_MODE_DEFAULT 4
#endif
#define HOLD_MS_DEFAULT 1000
#define AUTO_LATE_HOLD_MS 1000   /* mode 4, clock appeared after us: hold after the clock */

typedef enum { ST_OFF = 0, ST_MACHINE_OFF, ST_PROBE, ST_TIMED_HOLD, ST_WAIT_RISE, ST_ARMED, ST_NO_RIBBON, ST_MANUAL } st_t;

static st_t     s_state    = ST_OFF;
static bool     s_started  = false;
static bool     s_release  = false;   /* storage-ready release requested (0x2E.0) */
static bool     s_probe    = false;   /* FPGA probe + hardware arm requested (0x2E.2/3[/4]) */
static int      s_count    = 0;
static int64_t  s_next_us  = 0;
static int64_t  s_since_us = 0;
static char     s_str[80]  = "AUTO: OFF";
static uint8_t  s_last_st07 = 0;
static unsigned s_mode     = GS_MODE_DEFAULT;
static unsigned s_hold_ms  = HOLD_MS_DEFAULT;
static bool     s_reported = false;
static bool     s_alive_at_start = false;   /* machine clock already running on our first STATUS read */
static unsigned s_eff_mode = 1;             /* mode 4 resolves to 1 or 3 per sequence */
static bool     s_autotrig = false;   /* bench: arm the /RES-fall trace trigger inside the sequence */
static bool     s_trig_pending = false;
static int64_t  s_trig_at_us = 0;
#define AUTOTRIG_DELAY_US 20000       /* after the socket inputs come on; well before the ~230 ms release */

void     gs_socket_set_mode(unsigned m)     { s_mode = m > 4 ? 4 : m; }
unsigned gs_socket_get_mode(void)           { return s_mode; }
void     gs_socket_set_hold_ms(unsigned ms) { s_hold_ms = ms; }
unsigned gs_socket_get_hold_ms(void)        { return s_hold_ms; }
void     gs_socket_set_autotrig(bool on)    { s_autotrig = on; }
bool     gs_socket_get_autotrig(void)       { return s_autotrig; }

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

static void ctrl_write_keep(uint8_t bits)
{
    gs_socket_reg_write(0, (uint8_t)((gs_socket_reg_read(0) & GS_CTRL_KEEP) | bits));
}

/* /RES-fall trace trigger: TADDR = FF:FFFF, clear pulse with trig-en + address mode set */
static void arm_res_trigger(void)
{
    gs_socket_reg_write(29, 0xFF); gs_socket_reg_write(30, 0xFF); gs_socket_reg_write(31, 0xFF);
    uint8_t c = (uint8_t)((gs_socket_reg_read(0) & ~0x08) | 0x50);
    gs_socket_reg_write(0, (uint8_t)(c | 0x80));
    gs_socket_reg_write(0, c);
    GLOG("GS SOCKET: /RES-FALL TRACE TRIGGER ARMED");
}
static void schedule_autotrig(int64_t now)
{
    if (!s_autotrig) return;
    s_trig_pending = true;
    s_trig_at_us = now + AUTOTRIG_DELAY_US;
}

/* 0x2E owner: {arm_early, autoarm, probe, assert, release} written together */
static void a2_reset_write(bool assert_hold)
{
    uint8_t v = (uint8_t)((assert_hold ? A2RST_ASSERT : 0) | (s_release ? A2RST_RELEASE : 0));
    if (s_probe) {
        v |= A2RST_PROBE | A2RST_AUTOARM;
        if (s_eff_mode == 1) v |= A2RST_ARM_EARLY;
    }
    fpga_reg_write(A2REG_A2_RST_RELEASE, v);
}

static bool owning_hold(void)
{
    return s_state == ST_MACHINE_OFF || s_state == ST_PROBE || s_state == ST_TIMED_HOLD;
}

void gs_socket_a2_release(void)
{
    s_release = true;
    a2_reset_write(owning_hold());   /* keep our hold while we own it */
}

static unsigned eff_hold_ms(void)
{
    return s_mode == 4 ? AUTO_LATE_HOLD_MS : s_hold_ms;
}

static void set_state(st_t st, const char *why)
{
    s_state = st;
    s_count = 0;
    s_reported = false;
    switch (st) {
    case ST_OFF:         snprintf(s_str, sizeof(s_str), "AUTO: OFF"); break;
    case ST_MACHINE_OFF: snprintf(s_str, sizeof(s_str), "AUTO: HOLDING RESET, SOCKET OFF (WAITING FOR CLOCK)"); break;
    case ST_PROBE:       snprintf(s_str, sizeof(s_str), s_eff_mode == 1 ? "AUTO: CLOCK UP - ARMED UNDER OUR HOLD, PROBING FOR THE MACHINE'S RELEASE"
                                                                    : "AUTO: CLOCK UP - PROBING (FPGA ARMS AT THE MACHINE'S RELEASE)"); break;
    case ST_TIMED_HOLD:  snprintf(s_str, sizeof(s_str), "AUTO: CLOCK UP - TIMED HOLD %u ms (SOCKET %s)", eff_hold_ms(), s_eff_mode == 3 ? "ARMED" : "OFF"); break;
    case ST_WAIT_RISE:   snprintf(s_str, sizeof(s_str), "AUTO: RELEASED, SOCKET OFF - WAITING FOR SLOT RESET HIGH"); break;
    case ST_ARMED:       snprintf(s_str, sizeof(s_str), s_mode == 4 ? "AUTO: ARMED (mode 4 -> %u)" : "AUTO: ARMED (mode %u)", s_mode == 4 ? s_eff_mode : s_mode); break;
    case ST_NO_RIBBON:   snprintf(s_str, sizeof(s_str), "AUTO: NO PHI2 AT SOCKET - PLAIN CARD, IDLE"); break;
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
    if (owning_hold()) { s_probe = false; a2_reset_write(false); }   /* never leave our hold behind */
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
    s_probe = false;
    s_trig_pending = false;
    ctrl_write_keep(0);                  /* everything off (bench trigger bits kept) ... */
    a2_reset_write(false);               /* assert 0 -> 1 = a NEW sequence (clears por_done) */
    a2_reset_write(true);                /* ... and hold the slot reset: the IIgs must power
                                            up under a proper reset (G35) */
    set_state(ST_MACHINE_OFF, why);
}

static void enter_armed(const char *why, int64_t now)
{
    set_state(ST_ARMED, why);
    s_since_us = now;
    s_next_us = now + ARMED_POLL_US;
}

void gs_socket_poll(void)
{
    if (!fpga_link_ok()) return;
    int64_t now = esp_timer_get_time();

    if (!s_started) {
        s_started = true;
        /* STATUS read first: it latches "MCU ready" in the FPGA. */
        {
            uint8_t st0 = fpga_reg_read(A2REG_STATUS);
            s_alive_at_start = (st0 & 0x01) && (st0 & A2ST_A2_ALIVE);
        }
        s_release = (fpga_reg_read(A2REG_A2_RST_RELEASE) & A2RST_RELEASE) != 0;
        if (settings()->gs_socket_off) {
            s_probe = false;
            gs_socket_reg_write(0, 0x00);
            a2_reset_write(false);
            set_state(ST_OFF, "DISABLED BY SETTING");
            return;
        }
        uint8_t ctrl = gs_socket_reg_read(0);
        if (ctrl & GS_CTRL_ARM) {
            enter_armed("ALREADY ARMED - ADOPTED", now);   /* ESP32 restart, machine running */
            return;
        }
        enter_machine_off("WAITING FOR MACHINE CLOCK");
        s_next_us = now + POLL_US;
        return;
    }
    if (s_state == ST_OFF || s_state == ST_MANUAL) return;
    if (now < s_next_us) return;

    if (s_trig_pending && now >= s_trig_at_us) { s_trig_pending = false; arm_res_trigger(); }

    uint8_t st07 = fpga_reg_read(A2REG_STATUS);
    if (!(st07 & 0x01)) return;            /* ready bit clear = bad link read; never act on it */
    bool alive = (st07 & A2ST_A2_ALIVE) != 0;
    s_last_st07 = st07;

    switch (s_state) {
    case ST_MACHINE_OFF:
        s_next_us = now + POLL_US;
        s_count = alive ? s_count + 1 : 0;
        if (s_count < ALIVE_SAMPLES) break;
        s_since_us = now;
        if (s_mode == 4) {
            /* clock already running when we came up = slot-powered start, machine held
             * since power-on: release now (probe finds the line free). Clock appeared
             * after us = card alive first: cover the machine's 283/293 ms window. */
            s_eff_mode = s_alive_at_start ? 1 : 3;
            s_alive_at_start = false;              /* only the first sequence can be the slot-powered one */
        } else {
            s_eff_mode = s_mode;
        }
        switch (s_eff_mode) {
        case 0:
        case 1:
            /* clock is up: the FPGA finds the moment the machine's own reset
             * logic lets go; mode 1 also arms the socket now, under the hold */
            s_probe = true;
            a2_reset_write(true);
            set_state(ST_PROBE, s_eff_mode == 1 ? "MACHINE CLOCK UP - ARMED UNDER HOLD, PROBE ENABLED"
                                            : "MACHINE CLOCK UP - PROBE + AUTO-ARM ENABLED");
            schedule_autotrig(now);
            break;
        case 3:
            ctrl_write_keep(GS_CTRL_LISTEN | GS_CTRL_ARM);   /* core sits in reset on a driven bus */
            set_state(ST_TIMED_HOLD, "MACHINE CLOCK UP - ARMED UNDER HOLD, TIMED RELEASE");
            schedule_autotrig(now);
            break;
        default:
            set_state(ST_TIMED_HOLD, "MACHINE CLOCK UP - TIMED RELEASE, SOCKET OFF");
            break;
        }
        break;

    case ST_PROBE: {
        s_next_us = now + POLL_US;
        if (!alive) { enter_machine_off("CLOCK LOST WHILE PROBING"); break; }
        uint8_t rst = fpga_reg_read(A2REG_A2_RST_RELEASE);
        if (rst & A2RST_POR_DONE) {
            enter_armed(s_eff_mode == 1 ? "MACHINE RESET RELEASED - CORE STARTED (ARMED UNDER HOLD)"
                                    : "MACHINE RESET RELEASED - SOCKET ARMED BY THE FPGA", now);
        } else if (now - s_since_us > PROBE_REPORT_US) {
            GLOG("GS SOCKET: MACHINE RESET NEVER RELEASED (rst=%02X st=%02X)", rst, st07);
            s_since_us = now;
        }
        break;
    }

    case ST_TIMED_HOLD:
        s_next_us = now + POLL_US;
        if (!alive) { enter_machine_off("CLOCK LOST DURING THE HOLD"); break; }
        if (now - s_since_us < (int64_t)eff_hold_ms() * 1000) break;
        a2_reset_write(false);                       /* release our hold */
        s_since_us = now;
        if (s_eff_mode == 3) enter_armed("HOLD OVER - RESET RELEASED WITH THE SOCKET ARMED", now);
        else             set_state(ST_WAIT_RISE, "HOLD OVER - RESET RELEASED, SOCKET OFF");
        break;

    case ST_WAIT_RISE:
        s_next_us = now + POLL_US;
        if (!alive) { enter_machine_off("CLOCK LOST AFTER THE RELEASE"); break; }
        if (st07 & A2ST_A2_RESET_N) {
            ctrl_write_keep(GS_CTRL_LISTEN | GS_CTRL_ARM);
            enter_armed("SLOT RESET HIGH - ARMED", now);
        } else if (!s_reported && now - s_since_us > RISE_REPORT_US) {
            GLOG("GS SOCKET: SLOT RESET NEVER ROSE AFTER OUR RELEASE (st=%02X)", st07);
            s_reported = true;
        }
        break;

    case ST_ARMED: {
        s_next_us = now + ARMED_POLL_US;
        s_count = alive ? 0 : s_count + 1;
        if (s_count >= DEAD_SAMPLES) { enter_machine_off("MACHINE CLOCK STOPPED - SOCKET OFF, HOLDING RESET"); break; }
        if (s_probe && now - s_since_us > NO_RIBBON_US) {
            uint8_t gs = gs_socket_reg_read(1);
            if (!(gs & GS_ST_PH2_ALIVE)) {          /* armed but no PHI2 at the socket: no ribbon */
                s_probe = false;                    /* drop the hardware arm (CTRL back to 0) */
                a2_reset_write(false);
                gs_socket_reg_write(0, 0x00);
                set_state(ST_NO_RIBBON, "NO PHI2 AT THE SOCKET - PLAIN CARD, IDLE");
            } else {
                /* make the arm explicit in CTRL, then drop the hardware terms */
                ctrl_write_keep(GS_CTRL_LISTEN | GS_CTRL_ARM);
                s_probe = false;
                a2_reset_write(false);
            }
        }
        break;
    }

    case ST_NO_RIBBON:
        s_next_us = now + ARMED_POLL_US;
        s_count = alive ? 0 : s_count + 1;
        if (s_count >= DEAD_SAMPLES)
            enter_machine_off("MACHINE CLOCK STOPPED - HOLDING RESET");
        break;

    default:
        break;
    }
}
