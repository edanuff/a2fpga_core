/*
 * test_gs_accel.c — host tests for the S7 accelerator bring-up ladder
 * (gs_socket.c, gs_accel_*), against a small model of the accelerator-only
 * gateware's register window (boards/a2mega/docs/gs_socket_s7_accel_only_build.md,
 * section 2): requests in window register 29 under the key in 31, the mode in
 * effect in 15, the signature 5A in 14, the verify count in 12/13, telemetry
 * words through 23 and 24..27.
 *
 * What must hold, in every scenario:
 *   - register 29 is never written 00 or 08 after FAST has been in effect,
 *     unless the machine is off (the machine's banks 00/01 are stale);
 *   - FAST is never asked for unless card memory ran its verify time with the
 *     count unmoved;
 *   - nothing is written to 29/31 on gateware without the signature.
 *
 * Scenarios:
 *   T1  slot-powered start             -> 00 08 0A 1A 3B (fast, the copy, the speed rules), not before /RES was high 2 s
 *   T2  setting off                    -> no request; `gs accel go` runs the ladder
 *   T3  verify mismatch in card memory -> stops on 08, FAST never asked
 *   T4  mismatch in the copy of E0/E1  -> fast without it (2B)
 *   T5  build without the copy         -> 2B
 *   T6  /RES during the ROM copy       -> copied again, then the ladder
 *   T7  machine off while fast, on again -> 00 with the machine off, ladder again
 *   T8  ESP32 restart, machine in fast -> adopted, nothing written
 *   T9  slow / fast / setting off at run time -> 1A, 3B, 1A; never 00
 *   T13 the speed rules off and on at run time -> 1B, 3B
 *   T10 a hand write of register 29    -> the ladder stops
 *   T11 S6 gateware (no signature)     -> 29 and 31 never written
 *   T12 DDR3 not ready                 -> card memory not granted, stops on 08
 *
 * Build+run: tests/host/run.sh
 */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "esp_timer.h"
#include "fpga_link.h"
#include "settings.h"
#include "osd_console.h"
#include "gs_socket.h"

/* ---- clock, settings, log ------------------------------------------ */
static int64_t g_us;
int64_t esp_timer_get_time(void) { return g_us; }
static a2_settings_t g_cfg;
a2_settings_t *settings(void) { return &g_cfg; }
static char g_log[8192];
void osd_log(const char *fmt, ...)
{
    char b[160]; va_list ap; va_start(ap, fmt); vsnprintf(b, sizeof(b), fmt, ap); va_end(ap);
    if (strlen(g_log) + strlen(b) + 2 < sizeof(g_log)) { strcat(g_log, b); strcat(g_log, "\n"); }
}

/* ---- the model ------------------------------------------------------ */
static struct {
    bool    machine_on, accel_gw;           /* slot clock; accelerator-only bitstream */
    bool    ext_reset;                      /* something else holds /RESET low */
    uint8_t rst2e, ctrl, sel, key, req, widx;
    bool    valid, busy, fast_ran;
    int64_t copy_done_us;
    bool    built, whole, ddr_ready;
    bool    bad_verify, bad_copy;           /* card memory / the copy disagree with the machine */
    unsigned verify, sverify, copies;
    /* the record */
    uint8_t wr29[64]; int n29; int n31;
    bool    rule_broken;
} m;

static bool res_n(void)  { return m.machine_on && !(m.rst2e & A2RST_ASSERT) && !m.ext_reset; }
static bool armed(void)  { return (m.ctrl & 0x01) != 0; }
static bool keyed(void)  { return m.key == 0xA5; }
static bool cardmem(void){ return keyed() && (m.req & 0x02) && m.valid && m.ddr_ready; }
static bool fast(void)   { return cardmem() && (m.req & 0x01); }
static bool slow(void)   { return cardmem() && (m.req & 0x10) && m.built && m.whole; }

static void model_tick(void)
{
    if (m.busy && g_us >= m.copy_done_us) {
        m.busy = false;
        m.valid = res_n() && armed();       /* a /RES during the copy cuts it short */
        if (m.valid) m.copies++;
    }
    if (m.busy && !res_n()) { m.busy = false; m.valid = false; }
    if (armed() && res_n()) {
        if (cardmem() && !fast() && m.bad_verify && m.verify < 65535) m.verify++;
        if (slow() && !fast() && m.bad_copy && m.sverify < 65535) m.sverify++;
        if (fast()) m.fast_ran = true;
    }
}

static uint8_t mode_byte(void)
{
    return (uint8_t)((m.valid ? 0x80 : 0) | (m.busy ? 0x40 : 0) | (m.busy ? 0x20 : 0) | (fast() ? 0x10 : 0) |
                     (cardmem() ? 0x08 : 0) | (slow() ? 0x02 : 0));
}

static uint32_t word(unsigned n)
{
    switch (n) {
    case 6:  return ((uint32_t)m.verify << 16) | mode_byte();
    case 7:  return 0x00041234;
    case 8:  return ((uint32_t)m.copies << 24) | 0x7EE8;
    case 12: return 0x400 | (m.ddr_ready ? 0x100 : 0) | 0x7E;
    case 13: return ((uint32_t)m.sverify << 16) | (m.built ? 8 : 0) | (m.whole ? 4 : 0) | ((m.req & 0x10) ? 2 : 0) | (slow() ? 1 : 0);
    case 14: return 0x00E10400;
    default: return 0;
    }
}

bool fpga_link_ok(void) { return true; }
void fpga_link_lock(void) {}
void fpga_link_unlock(void) {}

uint8_t fpga_reg_read(uint8_t reg)
{
    if (reg == A2REG_STATUS) return (uint8_t)(0x01 | (m.machine_on ? A2ST_A2_ALIVE : 0) | (res_n() ? A2ST_A2_RESET_N : 0));
    if (reg == A2REG_A2_RST_RELEASE) return m.rst2e;
    if (reg != A2REG_GS_DATA) return 0;
    if (!m.accel_gw) {                      /* S6: a sample counter in 14/15, the trace in 24.. */
        if (m.sel == 0) return m.ctrl;
        if (m.sel == 1) return (uint8_t)((m.machine_on ? 0x80 : 0) | (res_n() ? 0x08 : 0));
        return 0x33;
    }
    switch (m.sel) {
    case 0:  return m.ctrl;
    case 1:  return (uint8_t)((m.machine_on ? 0x80 : 0) | (res_n() ? 0x08 : 0));
    case 12: return (uint8_t)(m.verify & 0xFF);
    case 13: return (uint8_t)(m.verify >> 8);
    case 14: return 0x5A;
    case 15: return mode_byte();
    case 24: return (uint8_t)(word(m.widx) >> 8);
    case 25: return (uint8_t)(word(m.widx) >> 16);
    case 26: return (uint8_t)(word(m.widx) >> 24);
    case 27: return (uint8_t)(word(m.widx));
    default: return 0;
    }
}

void fpga_reg_write(uint8_t reg, uint8_t val)
{
    if (reg == A2REG_A2_RST_RELEASE) { m.rst2e = val; return; }
    if (reg == A2REG_GS_SEL) { m.sel = val; return; }
    if (reg != A2REG_GS_DATA) return;
    switch (m.sel) {
    case 0:  m.ctrl = val; break;
    case 23: m.widx = val; break;
    case 31: m.key = val; m.n31++; break;
    case 29:
        if (m.n29 < 64) m.wr29[m.n29++] = val;
        /* the rule: after fast mode has run, never back to socket bytes with the machine on */
        if (m.fast_ran && m.machine_on && !(val & 0x02)) m.rule_broken = true;
        if (keyed() && (val & 0x08) && !(m.req & 0x08) && m.accel_gw) { m.busy = true; m.valid = false; m.copy_done_us = g_us + 46000; }
        m.req = val;
        break;
    default: break;
    }
}

/* ---- driving --------------------------------------------------------- */
static void run_ms(int ms)
{
    for (int i = 0; i < ms; i++) { g_us += 1000; model_tick(); gs_socket_poll(); }
}

static void fresh(bool accel_gw)
{
    memset(&m, 0, sizeof(m));
    memset(&g_cfg, 0, sizeof(g_cfg));
    g_log[0] = 0;
    m.accel_gw = accel_gw; m.built = true; m.whole = true; m.ddr_ready = true;
    m.rst2e = A2RST_RELEASE;
    gs_socket_resume();                     /* the module's "not started" state */
    gs_accel_set_delay_ms(2000);
    gs_accel_set_verify_ms(1000);
}

static int fails;
#define CHECK(c) do { if (!(c)) { printf("  FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)
static void show(const char *t)
{
    printf("%-4s 29 <=", t);
    for (int i = 0; i < m.n29; i++) printf(" %02X", m.wr29[i]);
    printf("   | %s\n", gs_accel_state_str());
}

int main(void)
{
    /* T1 */
    fresh(true); m.machine_on = true;
    run_ms(1500);                           /* arm at about 150 ms; the ladder not before 2 s of /RES high */
    CHECK(armed()); CHECK(m.n29 <= 1 && m.req == 0);   /* only the machine-off clear */
    { int before = m.n29; run_ms(5000);
      static const uint8_t want[] = { 0x00, 0x08, 0x0A, 0x1A, 0x3B };
      CHECK(m.n29 - before == 5 && memcmp(m.wr29 + before, want, 5) == 0); }
    CHECK(fast() && slow() && m.copies == 1 && !m.rule_broken);
    CHECK(strstr(gs_accel_state_str(), "FAST") != NULL);
    show("T1");

    /* T2 */
    fresh(true); g_cfg.gs_accel_off = 1; m.machine_on = true;
    run_ms(8000);
    CHECK(armed() && m.req == 0 && !m.valid);
    gs_accel_go(); run_ms(3000);
    CHECK(fast() && m.req == 0x3B && !m.rule_broken);
    show("T2");

    /* T3 */
    fresh(true); m.machine_on = true; m.bad_verify = true;
    run_ms(8000);
    CHECK(m.req == 0x08 && !m.fast_ran && !cardmem());
    CHECK(strstr(gs_accel_state_str(), "VERIFY MISMATCH AT 04:1234") != NULL);
    run_ms(5000); CHECK(m.req == 0x08);     /* and it stays there */
    show("T3");

    /* T4 */
    fresh(true); m.machine_on = true; m.bad_copy = true;
    run_ms(8000);
    CHECK(m.req == 0x2B && fast() && !slow() && !m.rule_broken);
    show("T4");

    /* T5 */
    fresh(true); m.machine_on = true; m.built = false; m.whole = false;
    run_ms(8000);
    CHECK(m.req == 0x2B && fast() && !m.rule_broken);
    { bool asked_slow = false; for (int i = 0; i < m.n29; i++) if (m.wr29[i] & 0x10) asked_slow = true; CHECK(!asked_slow); }
    show("T5");

    /* T6 */
    fresh(true); m.machine_on = true;
    run_ms(2000);
    for (int i = 0; i < 2000 && !m.busy; i++) run_ms(1);
    CHECK(m.busy);
    m.ext_reset = true; run_ms(30); m.ext_reset = false;     /* a 30 ms /RES in the copy */
    CHECK(!m.valid);
    run_ms(8000);
    CHECK(m.req == 0x3B && fast() && m.copies == 1 && !m.rule_broken);
    show("T6");

    /* T7 */
    fresh(true); m.machine_on = true;
    run_ms(8000); CHECK(fast());
    m.machine_on = false; run_ms(2000);
    CHECK(!armed() && m.req == 0x00 && !m.rule_broken);
    m.fast_ran = false;                     /* a power cycle is the reset the rule asks for */
    m.machine_on = true; run_ms(10000);
    CHECK(armed() && m.req == 0x3B && fast() && m.copies == 2 && !m.rule_broken);
    show("T7");

    /* T8 */
    fresh(true); m.machine_on = true; m.ctrl = 0x05; m.key = 0xA5; m.req = 0x3B; m.valid = true; m.fast_ran = true;
    run_ms(8000);
    CHECK(m.n29 == 0 && m.n31 == 0 && fast() && !m.rule_broken);
    CHECK(strstr(gs_accel_state_str(), "ADOPTED") != NULL);
    show("T8");

    /* T9 */
    fresh(true); m.machine_on = true;
    run_ms(8000); CHECK(m.req == 0x3B);
    gs_accel_pace(false); run_ms(200); CHECK(m.req == 0x1A && cardmem() && !fast());
    gs_accel_pace(true);  run_ms(200); CHECK(m.req == 0x3B && fast());
    gs_accel_set_enabled(false); run_ms(200); CHECK(m.req == 0x1A && g_cfg.gs_accel_off == 1);
    run_ms(5000); CHECK(m.req == 0x1A);     /* off stays paced: no ladder, no 00 */
    gs_accel_set_enabled(true); run_ms(200); CHECK(m.req == 0x3B && g_cfg.gs_accel_off == 0);
    CHECK(!m.rule_broken);
    show("T9");

    /* T10 */
    fresh(true); m.machine_on = true;
    run_ms(1000);
    { int before = m.n29; gs_accel_manual(); run_ms(8000); CHECK(m.n29 == before && !m.valid); }
    gs_accel_go(); run_ms(4000); CHECK(m.req == 0x3B);
    show("T10");

    /* T11 */
    fresh(false); m.machine_on = true;
    run_ms(8000);
    m.machine_on = false; run_ms(2000); m.machine_on = true; run_ms(8000);
    CHECK(armed() && m.n29 == 0 && m.n31 == 0);
    show("T11");

    /* T12 */
    fresh(true); m.machine_on = true; m.ddr_ready = false;
    run_ms(8000);
    CHECK(m.req == 0x08 && !m.fast_ran);
    CHECK(strstr(gs_accel_state_str(), "NOT GRANTED") != NULL);
    show("T12");

    /* T13 */
    fresh(true); m.machine_on = true;
    run_ms(8000); CHECK(m.req == 0x3B);
    gs_accel_set_rules(false); run_ms(100); CHECK(m.req == 0x1B && fast());
    gs_accel_pace(false); run_ms(100); CHECK(m.req == 0x1A);
    gs_accel_pace(true);  run_ms(100); CHECK(m.req == 0x1B);
    gs_accel_set_rules(true);  run_ms(100); CHECK(m.req == 0x3B && fast() && !m.rule_broken);
    show("T13");

    printf(fails ? "%d FAILED\n" : "all gs_accel scenarios pass\n", fails);
    return fails ? 1 : 0;
}
