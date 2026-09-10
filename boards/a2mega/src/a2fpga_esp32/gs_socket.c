/* gs_socket.c — auto-arm for the in-socket 65C816. See gs_socket.h.
 *
 * v7 (bench G31–G41, 2026-09-09). Measured rules on the ROM 01 IIgs:
 *   - nothing may drive (or even listen at) the socket while the machine is
 *     in its own power-on reset;
 *   - a machine that leaves reset with no CPU present for long wedges until
 *     a power cycle; so does a second reset pulse right after the first.
 * Hence the card holds the slot reset whenever the machine is dead, and the
 * FPGA does the timing-critical part: with the POR PROBE enabled it lets the
 * line float for 40 us every ms while holding; the first time the machine's
 * own reset logic no longer pulls it low it stays released and ARMS THE
 * SOCKET IN THE SAME CLOCK (0x2E bits 2/3, connector). The ESP32 only
 * sequences: MACHINE_OFF (hold) -> clock alive -> PROBE -> ARMED, and
 * re-holds when the clock stops. An FPGA already armed on ESP32 start is
 * adopted untouched. Storage gating stays: the FPGA will not release until
 * the disk task has written the storage-ready bit.
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

#define POLL_US           10000     /* 10 ms sampling */
#define ARMED_POLL_US    100000     /* 100 ms while armed */
#define ALIVE_SAMPLES        10     /* 100 ms of clock before we act */
#define DEAD_SAMPLES          5     /* clock gone for 500 ms -> machine off */
#define NO_RIBBON_US    2000000     /* armed but no PHI2 at the socket for 2 s: plain card */
#define PROBE_REPORT_US 15000000    /* log if the machine never lets go of its reset */

typedef enum { ST_OFF = 0, ST_MACHINE_OFF, ST_PROBE, ST_ARMED, ST_NO_RIBBON, ST_MANUAL } st_t;

static st_t    s_state    = ST_OFF;
static bool    s_started  = false;
static bool    s_release  = false;   /* storage-ready release requested (0x2E.0) */
static bool    s_probe    = false;   /* probe + hardware auto-arm requested (0x2E.2/3) */
static int     s_count    = 0;
static int64_t s_next_us  = 0;
static int64_t s_since_us = 0;
static char    s_str[72]  = "AUTO: OFF";
static uint8_t s_last_st07 = 0;
static unsigned s_por_hold_ms = 0;   /* legacy tunable, no longer used by the sequence */

void gs_socket_set_por_hold_ms(unsigned ms) { s_por_hold_ms = ms; }
unsigned gs_socket_get_por_hold_ms(void) { return s_por_hold_ms; }

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

/* 0x2E owner: {autoarm, probe, assert, release} written together */
static void a2_reset_write(bool assert_hold)
{
    fpga_reg_write(A2REG_A2_RST_RELEASE,
                   (uint8_t)((assert_hold ? A2RST_ASSERT : 0) | (s_release ? A2RST_RELEASE : 0) |
                             (s_probe ? (A2RST_PROBE | A2RST_AUTOARM) : 0)));
}

void gs_socket_a2_release(void)
{
    s_release = true;
    a2_reset_write(s_state == ST_MACHINE_OFF || s_state == ST_PROBE);   /* keep our hold while we own it */
}

static void set_state(st_t st, const char *why)
{
    s_state = st;
    s_count = 0;
    switch (st) {
    case ST_OFF:         snprintf(s_str, sizeof(s_str), "AUTO: OFF"); break;
    case ST_MACHINE_OFF: snprintf(s_str, sizeof(s_str), "AUTO: HOLDING RESET, SOCKET OFF (WAITING FOR CLOCK)"); break;
    case ST_PROBE:       snprintf(s_str, sizeof(s_str), "AUTO: CLOCK UP - PROBING THE MACHINE RESET (FPGA ARMS ON RELEASE)"); break;
    case ST_ARMED:       snprintf(s_str, sizeof(s_str), "AUTO: ARMED"); break;
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
    if (s_state == ST_MACHINE_OFF || s_state == ST_PROBE) { s_probe = false; a2_reset_write(false); }   /* never leave our hold behind */
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
    gs_socket_reg_write(0, 0x00);                    /* everything off ... */
    a2_reset_write(false);                           /* assert 0 -> 1 = a NEW sequence (clears por_done) */
    a2_reset_write(true);                            /* ... and hold the slot reset: the IIgs must power
                                                        up under a proper reset (G35); the FPGA releases it */
    set_state(ST_MACHINE_OFF, why);
}

void gs_socket_poll(void)
{
    if (!fpga_link_ok()) return;
    int64_t now = esp_timer_get_time();

    if (!s_started) {
        s_started = true;
        /* STATUS read first: it latches "MCU ready" in the FPGA. */
        (void)fpga_reg_read(A2REG_STATUS);
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
            set_state(ST_ARMED, "ALREADY ARMED - ADOPTED");   /* ESP32 restart, machine running */
            s_since_us = now;
            s_next_us = now + ARMED_POLL_US;
            return;
        }
        enter_machine_off("WAITING FOR MACHINE CLOCK");
        s_next_us = now + POLL_US;
        return;
    }
    if (s_state == ST_OFF || s_state == ST_MANUAL) return;
    if (now < s_next_us) return;

    uint8_t st07 = fpga_reg_read(A2REG_STATUS);
    if (!(st07 & 0x01)) return;            /* ready bit clear = bad link read; never act on it */
    bool alive = (st07 & A2ST_A2_ALIVE) != 0;
    s_last_st07 = st07;

    switch (s_state) {
    case ST_MACHINE_OFF:
        s_next_us = now + POLL_US;
        s_count = alive ? s_count + 1 : 0;
        if (s_count >= ALIVE_SAMPLES) {
            /* clock is up: let the FPGA find the exact moment the machine's own
             * reset logic lets go, and arm the socket in that same clock */
            s_probe = true;
            a2_reset_write(true);
            s_since_us = now;
            set_state(ST_PROBE, "MACHINE CLOCK UP - PROBE + AUTO-ARM ENABLED");
        }
        break;

    case ST_PROBE: {
        s_next_us = now + POLL_US;
        if (!alive) { enter_machine_off("CLOCK LOST WHILE PROBING"); break; }
        uint8_t rst = fpga_reg_read(A2REG_A2_RST_RELEASE);
        if (rst & A2RST_POR_DONE) {
            set_state(ST_ARMED, "MACHINE RESET RELEASED - SOCKET ARMED BY THE FPGA");
            s_since_us = now;
            s_next_us = now + ARMED_POLL_US;
        } else if (now - s_since_us > PROBE_REPORT_US) {
            GLOG("GS SOCKET: MACHINE RESET NEVER RELEASED (rst=%02X st=%02X)", rst, st07);
            s_since_us = now;
        }
        break;
    }

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
                gs_socket_reg_write(0, GS_CTRL_LISTEN | GS_CTRL_ARM);   /* make the arm explicit in CTRL */
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
