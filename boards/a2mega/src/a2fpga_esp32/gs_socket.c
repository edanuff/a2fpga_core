/* gs_socket.c — auto-arm for the in-socket 65C816. See gs_socket.h. */
#include <stdio.h>
#include "esp_timer.h"
#include "fpga_link.h"
#include "settings.h"
#include "osd_console.h"
#include "a2fpga_regs.h"
#include "gs_socket.h"

#define GLOG(...) do { printf("[gs] " __VA_ARGS__); printf("\n"); osd_log(__VA_ARGS__); } while (0)

/* CTRL register bits (window index 0) */
#define GS_CTRL_ARM     0x01
#define GS_CTRL_LISTEN  0x04
/* STATUS bits (window index 1) */
#define GS_ST_PH2_ALIVE 0x80

#define ALIVE_SAMPLES   5        /* consecutive alive samples before arming */
#define DEAD_SAMPLES    5        /* consecutive dead samples before disarming */
#define LISTEN_PERIOD_US   10000 /* 10 ms while waiting for PHI2 */
#define ARMED_PERIOD_US   100000 /* 100 ms while armed */

typedef enum { ST_OFF = 0, ST_LISTEN, ST_ARMED, ST_MANUAL } st_t;

static st_t    s_state   = ST_OFF;
static bool    s_started = false;
static int     s_count   = 0;
static int64_t s_next_us = 0;
static char    s_str[48] = "AUTO: OFF";

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

static void set_state(st_t st, const char *why)
{
    s_state = st;
    s_count = 0;
    switch (st) {
    case ST_OFF:    snprintf(s_str, sizeof(s_str), "AUTO: OFF"); break;
    case ST_LISTEN: snprintf(s_str, sizeof(s_str), "AUTO: LISTEN (NO PHI2)"); break;
    case ST_ARMED:  snprintf(s_str, sizeof(s_str), "AUTO: ARMED (PHI2 ALIVE)"); break;
    case ST_MANUAL: snprintf(s_str, sizeof(s_str), "MANUAL (gs auto TO RESUME)"); break;
    }
    if (why) GLOG("GS SOCKET: %s", why);
}

const char *gs_socket_state_str(void) { return s_str; }
bool gs_socket_is_manual(void) { return s_state == ST_MANUAL; }

void gs_socket_manual(void)
{
    if (s_state != ST_MANUAL) set_state(ST_MANUAL, "MANUAL CONTROL");
}

void gs_socket_resume(void)
{
    s_started = false;           /* re-run the entry decision */
    set_state(ST_OFF, NULL);
}

void gs_socket_apply(void)
{
    if (s_state == ST_MANUAL) return;
    s_started = false;
    set_state(ST_OFF, NULL);
}

void gs_socket_poll(void)
{
    if (!fpga_link_ok()) return;
    int64_t now = esp_timer_get_time();

    if (!s_started) {
        s_started = true;
        if (settings()->gs_socket_off) {
            gs_socket_reg_write(0, 0x00);            /* nothing driven, nothing listened */
            set_state(ST_OFF, "DISABLED BY SETTING");
            return;
        }
        gs_socket_reg_write(0, GS_CTRL_LISTEN);      /* input shifter only */
        set_state(ST_LISTEN, "LISTENING FOR PHI2");
        s_next_us = now + LISTEN_PERIOD_US;
        return;
    }
    if (s_state == ST_OFF || s_state == ST_MANUAL) return;
    if (now < s_next_us) return;

    uint8_t st = gs_socket_reg_read(1);
    bool alive = (st & GS_ST_PH2_ALIVE) != 0;

    if (s_state == ST_LISTEN) {
        s_next_us = now + LISTEN_PERIOD_US;
        s_count = alive ? s_count + 1 : 0;
        if (s_count >= ALIVE_SAMPLES) {
            gs_socket_reg_write(0, GS_CTRL_LISTEN | GS_CTRL_ARM);
            set_state(ST_ARMED, "PHI2 ALIVE - ARMED");
            s_next_us = now + ARMED_PERIOD_US;
        }
    } else if (s_state == ST_ARMED) {
        s_next_us = now + ARMED_PERIOD_US;
        s_count = alive ? 0 : s_count + 1;
        if (s_count >= DEAD_SAMPLES) {
            gs_socket_reg_write(0, GS_CTRL_LISTEN);  /* machine off: release the socket */
            set_state(ST_LISTEN, "PHI2 LOST - RELEASED");
            s_next_us = now + LISTEN_PERIOD_US;
        }
    }
}
