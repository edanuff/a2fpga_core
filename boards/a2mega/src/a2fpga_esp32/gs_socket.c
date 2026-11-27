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
 *   4 AUTO     default. Always: release with the socket OFF, arm LATE_MS (20 ms) after
 *              the line reads high. Clock already running when we started (slot-powered
 *              start: the machine has been held by the 2G06 since power-on) -> release
 *              at once (G54 6/6). Clock appears later (card alive before the machine)
 *              -> hold AUTO_LATE_HOLD_MS (300 ms) after the clock first (release ~410 ms
 *              after the clock). Why: this ROM 01 IIgs (IIe keyboard on J13, no real
 *              65816 executing) re-asserts /RESET 9.95-9.99 ms after ANY rising edge
 *              that lands within ~10-20 ms of its own power-on release at 282.9 ms
 *              after its clocks start, about half the time, and parks (G45-G74); no
 *              edge at >= 306 ms has ever pulled (0/27). Arming 20 ms after the edge
 *              keeps the core out of that 10 ms too (G53 vs G54).
 * The ESP32 only sequences; the FPGA does the microsecond parts (0x2E bits
 * 2/3/4, connector). An FPGA already armed on ESP32 start is adopted. Storage
 * gating stays: the FPGA will not release until the disk task has written the
 * storage-ready bit.
 */
#include <stdarg.h>
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
/* Mode 4, clock appeared after us (card alive first): timed hold after the clock.
 * The ROM 01 bench (test_log G74) showed the machine's 10 ms re-assert follows ANY
 * rising edge of /RESET that lands within ~10-20 ms of its own release at 282.9 ms
 * after its clocks start (2 pulls / 6 for edges at 284-293 ms), and never one at
 * 306 ms or later (0 / 27 up to 1.6 s). The clock-alive confirmation already adds
 * ~110 ms, so 300 ms here puts our release ~410 ms after the clock: 100+ ms past the
 * window with the same socket-off release + late arm the slot-powered path uses. */
#define AUTO_LATE_HOLD_MS 300

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
static unsigned s_eff_mode = 1;             /* mode 4 resolves to mode 2 per sequence (hold differs) */
static unsigned s_auto_hold_ms = 0;         /* mode 4: 0 slot-powered (machine held since power-on), AUTO_LATE_HOLD_MS card-first */
static unsigned s_late_ms  = 20;            /* modes 2/4: arm this long after the slot reset reads high */
static int64_t  s_rise_us  = 0;
static bool     s_autotrig = false;   /* bench: arm the /RES-fall trace trigger inside the sequence */
static bool     s_trig_pending = false;
static int64_t  s_trig_at_us = 0;
#define AUTOTRIG_DELAY_US 20000       /* after the socket inputs come on; well before the ~230 ms release */

void     gs_socket_set_mode(unsigned m)     { s_mode = m > 4 ? 4 : m; }
unsigned gs_socket_get_mode(void)           { return s_mode; }
void     gs_socket_set_hold_ms(unsigned ms) { s_hold_ms = ms; }
unsigned gs_socket_get_hold_ms(void)        { return s_hold_ms; }
void     gs_socket_set_late_ms(unsigned ms) { s_late_ms = ms; }
unsigned gs_socket_get_late_ms(void)        { return s_late_ms; }
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
    if (s_mode == 4) return s_auto_hold_ms;   /* slot-powered: release at once; card-first: cover the window */
    return s_hold_ms;
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
    case ST_WAIT_RISE:   snprintf(s_str, sizeof(s_str), "AUTO: RELEASED, SOCKET OFF - WAITING FOR SLOT RESET HIGH (+%u ms)", s_late_ms); break;
    case ST_ARMED:       snprintf(s_str, sizeof(s_str), s_mode == 4 ? "AUTO: ARMED (mode 4 -> %u)" : "AUTO: ARMED (mode %u)", s_mode == 4 ? s_eff_mode : s_mode); break;
    case ST_NO_RIBBON:   snprintf(s_str, sizeof(s_str), "AUTO: NO PHI2 AT SOCKET - PLAIN CARD, IDLE"); break;
    case ST_MANUAL:      snprintf(s_str, sizeof(s_str), "MANUAL (gs auto TO RESUME)"); break;
    }
    if (why) GLOG("GS SOCKET: %s (st=%02X)", why, s_last_st07);
}

/* ---- S7 accelerator bring-up (gs_socket.h) ---------------------------------
 * Window registers of the accelerator-only gateware
 * (boards/a2mega/docs/gs_socket_s7_accel_only_build.md, section 2):
 *   12/13 verify count, 14 signature 5A, 15 mode in effect, 29 requests, 31 key.
 * Telemetry words through 23 (index) and 24..27. */
#define ACC_REG_VERIFY   12
#define ACC_REG_SIG      14
#define ACC_REG_MODE     15
#define ACC_REG_WORD_IDX 23
#define ACC_REG_WORD     24
#define ACC_REG_REQ      29
#define ACC_REG_KEY      31
#define ACC_SIG          0x5A
#define ACC_KEY          0xA5
#define ACC_REQ_FAST     0x01
#define ACC_REQ_CARDMEM  0x02
#define ACC_REQ_COPY     0x08
#define ACC_REQ_SLOW     0x10      /* reads of E0/E1 RAM from the card's copy */
#define ACC_REQ_RULES    0x20      /* fast gives way to the machine's pace by its speed rules (gs_accel_rules.sv;
                                      gateware without them ignores the bit) */
#define ACC_MODE_VALID   0x80      /* ROM image valid */
#define ACC_MODE_BUSY    0x40      /* copy in progress */
#define ACC_MODE_FAST    0x10
#define ACC_MODE_CARDMEM 0x08
#define ACC_MODE_SLOW    0x02
#define ACC_W13_BUILT    0x08      /* word 13: built with the copy of E0/E1 */
#define ACC_GS_RES_N     0x08      /* window register 1: socket /RES high */

#define ACC_POLL_US        20000   /* while a ladder is in flight */
#define ACC_SIG_SAMPLES        3
#define ACC_DELAY_MS_DEFAULT 2000  /* /RES high this long before the ladder: past the ROM's
                                      cold/warm test, which must read the machine's own RAM */
#define ACC_VERIFY_MS_DEFAULT 1000
#define ACC_COPY_TIMEOUT_US 1000000 /* a copy holds the core about 46 ms */
#define ACC_GRANT_TIMEOUT_US 300000
#define ACC_COPY_TRIES         3

typedef enum { ACC_NONE = 0, ACC_WAIT, ACC_COPY, ACC_VERIFY, ACC_VERIFY_COPY, ACC_FAST, ACC_PACED,
               ACC_FAILED, ACC_MANUAL } acc_t;

static acc_t    s_acc        = ACC_NONE;
static int      s_acc_sig    = 0;        /* consecutive polls with the signature */
static bool     s_acc_seen   = false;
static bool     s_acc_go     = false;    /* `gs accel go`: no delay, even with the setting off */
static int64_t  s_acc_t0     = 0;        /* start of the wait / of the step in flight */
static int      s_acc_tries  = 0;
static unsigned s_acc_base   = 0;        /* verify count when card memory was asked for */
static unsigned s_acc_sbase  = 0;        /* the copy's verify count when it was asked for */
static uint8_t  s_acc_req    = 0;        /* what we last wrote to register 29 */
static unsigned s_acc_delay_ms  = ACC_DELAY_MS_DEFAULT;
static unsigned s_acc_verify_ms = ACC_VERIFY_MS_DEFAULT;
static bool     s_acc_rules  = true;     /* ask for the speed rules with fast mode (`gs accel rules on|off`) */
static char     s_acc_str[96] = "ACCEL: -";

void     gs_accel_set_delay_ms(unsigned ms)  { s_acc_delay_ms = ms; }
unsigned gs_accel_get_delay_ms(void)         { return s_acc_delay_ms; }
void     gs_accel_set_verify_ms(unsigned ms) { s_acc_verify_ms = ms < 100 ? 100 : ms; }
unsigned gs_accel_get_verify_ms(void)        { return s_acc_verify_ms; }
bool     gs_accel_present(void)              { return s_acc_seen; }
const char *gs_accel_state_str(void)         { return s_acc_str; }

uint32_t gs_accel_word(unsigned n)
{
    uint32_t w = 0, prev = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t b[4];
        fpga_link_lock();
        fpga_reg_write(A2REG_GS_SEL, ACC_REG_WORD_IDX);
        fpga_reg_write(A2REG_GS_DATA, (uint8_t)n);
        for (int k = 0; k < 4; k++) {
            fpga_reg_write(A2REG_GS_SEL, (uint8_t)(ACC_REG_WORD + k));
            b[k] = fpga_reg_read(A2REG_GS_DATA);
        }
        fpga_link_unlock();
        /* the trace's "bank:addr data" read as one number */
        w = ((uint32_t)b[2] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[0] << 8) | b[3];
        if (i && w == prev) break;
        prev = w;
    }
    return w;
}

static unsigned acc_verify_count(void)
{
    return (unsigned)gs_socket_reg_read(ACC_REG_VERIFY) | ((unsigned)gs_socket_reg_read(ACC_REG_VERIFY + 1) << 8);
}

/* The signature, and bit 0 of the mode, which the accelerator never sets (an S6
 * bitstream has a sample counter in these two registers). */
static bool acc_sig_now(void)
{
    return gs_socket_reg_read(ACC_REG_SIG) == ACC_SIG && !(gs_socket_reg_read(ACC_REG_MODE) & 0x01);
}

static void acc_req(uint8_t req)
{
    if ((req & ACC_REQ_FAST) && s_acc_rules) req |= ACC_REQ_RULES;
    else                                     req &= (uint8_t)~ACC_REQ_RULES;
    gs_socket_reg_write(ACC_REG_KEY, ACC_KEY);
    gs_socket_reg_write(ACC_REG_REQ, req);
    s_acc_req = req;
}

static void acc_set(acc_t st, int64_t now, const char *fmt, ...)
{
    va_list ap;
    char msg[72];
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);
    bool changed = (st != s_acc);
    s_acc = st;
    s_acc_t0 = now;
    snprintf(s_acc_str, sizeof(s_acc_str), "ACCEL: %s", msg);
    if (changed) GLOG("%s", s_acc_str);
}

/* Start over without touching the gateware: what is in effect is read again. */
static void acc_forget(void)
{
    s_acc = ACC_NONE;
    s_acc_sig = 0;
    s_acc_seen = false;
    s_acc_go = false;
    s_acc_req = 0;
    snprintf(s_acc_str, sizeof(s_acc_str), "ACCEL: -");
}

/* Machine off: the next power-up must start on the machine's own memory. */
static void acc_machine_off(void)
{
    if (acc_sig_now()) gs_socket_reg_write(ACC_REG_REQ, 0x00);   /* asked of the gateware that is loaded now */
    acc_forget();
}

static void acc_start(int64_t now)
{
    /* 00 first: the copy starts on a rising edge of its bit. Never reached with
     * fast mode in effect (acc_poll adopts that). */
    gs_socket_reg_write(ACC_REG_KEY, ACC_KEY);
    gs_socket_reg_write(ACC_REG_REQ, 0x00);
    acc_req(ACC_REQ_COPY);
    acc_set(ACC_COPY, now, "COPYING THE ROM (TRY %d)", s_acc_tries + 1);
}

static void acc_poll(int64_t now)
{
    if (s_acc == ACC_MANUAL || s_acc == ACC_FAILED) return;
    if (s_acc == ACC_NONE) {
        s_acc_sig = acc_sig_now() ? s_acc_sig + 1 : 0;
        if (s_acc_sig < ACC_SIG_SAMPLES) return;
        s_acc_seen = true;
        s_acc_tries = 0;
        if (settings()->gs_accel_off) acc_set(ACC_WAIT, now, "OFF BY SETTING (gs accel go)");
        else                          acc_set(ACC_WAIT, now, "WAITING FOR /RES HIGH %u ms", s_acc_delay_ms);
    }

    uint8_t mode = gs_socket_reg_read(ACC_REG_MODE);
    bool res_n = (gs_socket_reg_read(1) & ACC_GS_RES_N) != 0;

    switch (s_acc) {
    case ACC_WAIT:
        if (mode & ACC_MODE_FAST) {                 /* set by hand, or an ESP32 restart under a running machine */
            s_acc_req = (uint8_t)(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_FAST | ((mode & ACC_MODE_SLOW) ? ACC_REQ_SLOW : 0));
            acc_set(ACC_FAST, now, "FAST (ADOPTED, MODE %02X)", mode);
            break;
        }
        if (settings()->gs_accel_off && !s_acc_go) break;
        if (!res_n) { s_acc_t0 = now; break; }
        if (!s_acc_go && now - s_acc_t0 < (int64_t)s_acc_delay_ms * 1000) break;
        s_acc_go = false;
        if (mode & ACC_MODE_CARDMEM) {
            /* paced card memory already (a ladder cut short, or "off" after fast): 00 is
             * not known to be safe from here, fast is */
            acc_req((uint8_t)(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_FAST | ((mode & ACC_MODE_SLOW) ? ACC_REQ_SLOW : 0)));
            acc_set(ACC_FAST, now, "FAST (FROM PACED, MODE %02X)", mode);
            break;
        }
        acc_start(now);
        break;

    case ACC_COPY:
        if ((mode & ACC_MODE_VALID) && !(mode & ACC_MODE_BUSY)) {
            uint32_t w8 = gs_accel_word(8);
            GLOG("ACCEL: ROM IMAGE VALID, SUM %04X", (unsigned)(w8 & 0xFFFF));
            s_acc_base = acc_verify_count();
            acc_req(ACC_REQ_COPY | ACC_REQ_CARDMEM);
            acc_set(ACC_VERIFY, now, "CARD MEMORY AT THE MACHINE'S PACE, VERIFY %u ms", s_acc_verify_ms);
        } else if (now - s_acc_t0 > ACC_COPY_TIMEOUT_US) {
            if (++s_acc_tries < ACC_COPY_TRIES) acc_start(now);     /* a /RES cuts a copy short */
            else { gs_socket_reg_write(ACC_REG_REQ, 0x00); s_acc_req = 0;
                   acc_set(ACC_FAILED, now, "NO ROM IMAGE AFTER %d COPIES (MODE %02X)", ACC_COPY_TRIES, mode); }
        }
        break;

    case ACC_VERIFY: {
        unsigned v = acc_verify_count();
        if (v != s_acc_base) {
            uint32_t at = gs_accel_word(7);
            acc_req(ACC_REQ_COPY);                                  /* back to socket bytes: fast has not run */
            acc_set(ACC_FAILED, now, "VERIFY MISMATCH AT %02X:%04X (%u) - ON SOCKET BYTES",
                    (unsigned)((at >> 16) & 0xFF), (unsigned)(at & 0xFFFF), v);
            break;
        }
        if (!(mode & ACC_MODE_CARDMEM)) {
            if (now - s_acc_t0 > ACC_GRANT_TIMEOUT_US) {
                uint32_t w12 = gs_accel_word(12);
                acc_req(ACC_REQ_COPY);
                acc_set(ACC_FAILED, now, "CARD MEMORY NOT GRANTED (MODE %02X, WORD 12 %04X)", mode, (unsigned)(w12 & 0xFFFF));
            }
            break;
        }
        if (now - s_acc_t0 < (int64_t)s_acc_verify_ms * 1000) break;
        if (gs_accel_word(13) & ACC_W13_BUILT) {
            s_acc_sbase = (unsigned)(gs_accel_word(13) >> 16);
            acc_req(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_SLOW);
            acc_set(ACC_VERIFY_COPY, now, "THE SAME WITH THE COPY OF E0/E1, VERIFY %u ms", s_acc_verify_ms);
        } else {
            acc_req(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_FAST);
            acc_set(ACC_FAST, now, "FAST (NO COPY OF E0/E1 IN THIS BUILD)");
        }
        break;
    }

    case ACC_VERIFY_COPY: {
        unsigned v = acc_verify_count();
        unsigned sv = (unsigned)(gs_accel_word(13) >> 16);
        if (v != s_acc_base) {
            uint32_t at = gs_accel_word(7);
            acc_req(ACC_REQ_COPY);
            acc_set(ACC_FAILED, now, "VERIFY MISMATCH AT %02X:%04X (%u) - ON SOCKET BYTES",
                    (unsigned)((at >> 16) & 0xFF), (unsigned)(at & 0xFFFF), v);
            break;
        }
        if (sv != s_acc_sbase) {
            uint32_t at = gs_accel_word(14);
            GLOG("ACCEL: COPY OF E0/E1 MISMATCH AT %02X:%04X (%u)", (unsigned)((at >> 16) & 0xFF), (unsigned)(at & 0xFFFF), sv);
            acc_req(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_FAST);
            acc_set(ACC_FAST, now, "FAST, E0/E1 FROM THE MACHINE (COPY MISMATCH)");
            break;
        }
        if (now - s_acc_t0 < (int64_t)s_acc_verify_ms * 1000) break;
        if (!(mode & ACC_MODE_SLOW)) {
            acc_req(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_FAST);
            acc_set(ACC_FAST, now, "FAST, E0/E1 FROM THE MACHINE (COPY NOT WHOLE)");
        } else {
            acc_req(ACC_REQ_COPY | ACC_REQ_CARDMEM | ACC_REQ_SLOW | ACC_REQ_FAST);
            acc_set(ACC_FAST, now, "FAST");
        }
        break;
    }

    default:
        break;
    }
    if (s_acc == ACC_COPY || s_acc == ACC_VERIFY || s_acc == ACC_VERIFY_COPY) s_next_us = now + ACC_POLL_US;
}

void gs_accel_manual(void)
{
    if (s_acc == ACC_MANUAL) return;
    s_acc = ACC_MANUAL;
    snprintf(s_acc_str, sizeof(s_acc_str), "ACCEL: MANUAL (gs accel go TO RESUME)");
}

void gs_accel_go(void)
{
    if (s_state != ST_ARMED) return;
    if (s_acc == ACC_FAST) return;
    s_acc_go = true;
    s_acc_tries = 0;
    if (s_acc != ACC_NONE) acc_set(ACC_WAIT, esp_timer_get_time(), "STARTING");
    s_next_us = 0;
}

void gs_accel_pace(bool fast)
{
    if (s_state != ST_ARMED) return;
    int64_t now = esp_timer_get_time();
    if (fast && s_acc == ACC_PACED) {
        acc_req((uint8_t)(s_acc_req | ACC_REQ_FAST));
        acc_set(ACC_FAST, now, "FAST");
    } else if (!fast && s_acc == ACC_FAST) {
        /* the machine's banks 00/01 are stale once fast mode has run: paced card
         * memory is the way back, never 00 */
        acc_req((uint8_t)(s_acc_req & ~ACC_REQ_FAST));
        acc_set(ACC_PACED, now, "PACED CARD MEMORY (gs accel fast)");
    }
}

void gs_accel_set_rules(bool on)
{
    s_acc_rules = on;
    if (s_state == ST_ARMED && s_acc == ACC_FAST) acc_req(s_acc_req);   /* at once: the gateware takes it at any step */
}
bool gs_accel_get_rules(void) { return s_acc_rules; }

void gs_accel_set_enabled(bool on)
{
    settings()->gs_accel_off = on ? 0 : 1;
    int64_t now = esp_timer_get_time();
    if (on) {
        if (s_acc == ACC_PACED) gs_accel_pace(true);
        else if (s_acc == ACC_WAIT) acc_set(ACC_WAIT, now, "WAITING FOR /RES HIGH %u ms", s_acc_delay_ms);
        else if (s_acc == ACC_FAILED || s_acc == ACC_MANUAL) gs_accel_go();
    } else {
        if (s_acc == ACC_FAST) gs_accel_pace(false);
        else if (s_acc == ACC_WAIT) acc_set(ACC_WAIT, now, "OFF BY SETTING (gs accel go)");
    }
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
    acc_machine_off();                   /* ... and the accelerator back to the machine's own memory */
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
        acc_forget();
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
            /* Both paths: release with the socket OFF, arm LATE_MS after the line reads
             * high (G54 6/6, G70 7/7). Clock already running when we came up = slot-
             * powered start, machine held by the 2G06 since power-on: release now.
             * Clock appeared after us = card alive first: hold AUTO_LATE_HOLD_MS past the
             * clock so our release lands well after the machine's own 282.9 ms release
             * and its ~20 ms re-assert window (G74). */
            s_eff_mode = 2;
            s_auto_hold_ms = s_alive_at_start ? 0 : AUTO_LATE_HOLD_MS;
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
        else           { s_rise_us = 0; set_state(ST_WAIT_RISE, "HOLD OVER - RESET RELEASED, SOCKET OFF"); }
        break;

    case ST_WAIT_RISE:
        s_next_us = now + (s_late_ms ? 1000 : POLL_US);   /* 1 ms polls once a delay is requested */
        if (!alive) { enter_machine_off("CLOCK LOST AFTER THE RELEASE"); break; }
        if (st07 & A2ST_A2_RESET_N) {
            if (!s_rise_us) s_rise_us = now;                /* first time we see it high */
            if (now - s_rise_us < (int64_t)s_late_ms * 1000) break;
            ctrl_write_keep(GS_CTRL_LISTEN | GS_CTRL_ARM);
            enter_armed(s_late_ms ? "SLOT RESET HIGH + DELAY - ARMED" : "SLOT RESET HIGH - ARMED", now);
        } else {
            s_rise_us = 0;
            if (!s_reported && now - s_since_us > RISE_REPORT_US) {
                GLOG("GS SOCKET: SLOT RESET NEVER ROSE AFTER OUR RELEASE (st=%02X)", st07);
                s_reported = true;
            }
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
        if (!s_probe) acc_poll(now);                /* S7 accelerator: the bring-up ladder */
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
