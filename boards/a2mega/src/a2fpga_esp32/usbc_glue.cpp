/*
 * usbc_glue — board glue between the vendored USB-C PD stack (usbc_port.c /
 * fusb302.c, from DisplayPort_Verilog usb-c/) and the a2mega 1.0a3 hardware.
 *
 * The vendored esp32s3_integration.c skeleton assumes a GPIO-mode TUSB1046A;
 * the 1.0a3 board straps the mux into I2C mode (I2C_EN pulled up, CTL/HPD
 * pins NC), so this file provides its own usbc_port_hal_t instead:
 *
 *   - FUSB302B (0x22) + TUSB1046A (0x12) share one I2C bus (Wire on
 *     IO1=SCL / IO2=SDA).
 *   - DP enable = TUSB1046A General reg 0x0A: CTLSEL[1:0]=10 (four-lane DP)
 *     | FLIPSEL per orientation | HPDIN_OVRRIDE (pin 23 is NC). Disable =
 *     CTLSEL=01. RX EQ stays at defaults; AUX snooping (on by default)
 *     trims active lanes to whatever the FPGA trains. (usb-c/SPEC.md §6.)
 *   - HPD to the FPGA is a real wire (PIN_DP_HPD_OUT -> gateware
 *     hotplug_decode): level driven directly, IRQ = 0.75 ms low pulse
 *     generated here (SPEC.md §8; 0.5-1.0 ms window).
 *   - VBUS sourcing = TPS2553 EN on PIN_VBUS_SRC_EN (1 A limit in hardware).
 *
 * The PD policy task runs at 1 kHz and is woken immediately by the FUSB302B
 * INT_N falling edge, per the usbc_port_task() contract.
 */
#include "board_pins.h"

#if A2MEGA_HAS_USBC_PD

#include <Arduino.h>
#include <Wire.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern "C" {
#include "usbc_port.h"
#include "osd_console.h"
}
#include "usbc_glue.h"

#define TUSB1046_I2C_ADDR   0x12
#define TUSB1046_REG_GENERAL 0x0A
#define TUSB_GEN_CTLSEL_USB3 0x01   /* DP lanes off (as-built disable state) */
#define TUSB_GEN_CTLSEL_DP4  0x02   /* four-lane DP, pin assignment C/E */
#define TUSB_GEN_FLIPSEL     0x04
#define TUSB_GEN_HPDIN_OVR   0x08   /* ignore the (NC) HPDIN pin */
#define TUSB_GEN_EQ_OVERRIDE 0x10   /* EQ from registers, not strap pins */
#define TUSB1046_REG_DPEQ10  0x10   /* DP1EQ_SEL[7:4] | DP0EQ_SEL[3:0] */
#define TUSB1046_REG_DPEQ32  0x11   /* DP3EQ_SEL[7:4] | DP2EQ_SEL[3:0] */
#define TUSB1046_REG_DPCTL3  0x13   /* AUX_SNOOP_DISABLE | AUX_SBU_OVR | DPx_DISABLE */
#define TUSB_DP_SNOOP_DIS    0x80   /* lanes governed by DPx_DISABLE (all default-enabled) */

/* DP receiver EQ. CRITICAL (board #1 root-cause candidate, 2026-08-12):
 * in I2C mode the DPEQ strap pins double as address pins and float, so
 * the chip latches EQ setting 10 = 12.3 dB at reset (datasheet Table 7,
 * F/F row) — ~11 dB of over-equalization for a few-cm board trace,
 * enough to wreck the redriven eye at 2.7 Gb/s. EQ_OVERRIDE + low
 * register EQ is mandatory in I2C mode; SPEC.md always said EQ is
 * "register-settable instead of strap resistors". Setting 0 = 1.0 dB. */
static uint8_t s_dp_eq_setting = 0;

static usbc_port_t s_port;
static SemaphoreHandle_t s_wake;
static bool s_running;

/* Status mirrors for the CLI / telnet 'pd' command. */
static volatile bool s_hpd_level;
static volatile bool s_vbus_on;
static volatile bool s_dp_mux_on;
static volatile bool s_fpga_dp_en;

/* ---- I2C (shared bus: FUSB302B + TUSB1046A) ------------------------------
 * Bus mutex: the 1 kHz PD task, the CLI (loop task), and the telnet task
 * (mux dumps / EQ cycling) all touch Wire — unlocked access produced
 * intermittent read failures (the 'x' dump's 0xEE noise). */

static SemaphoreHandle_t s_i2c_lock;

static void i2c_lock(void)
{
    if (s_i2c_lock)
        xSemaphoreTake(s_i2c_lock, portMAX_DELAY);
}

static void i2c_unlock(void)
{
    if (s_i2c_lock)
        xSemaphoreGive(s_i2c_lock);
}

static int i2c_read(void *ctx, uint8_t address, uint8_t reg,
                    uint8_t *data, size_t length)
{
    (void)ctx;
    i2c_lock();
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        i2c_unlock();
        return -1;
    }
    size_t got = Wire.requestFrom(address, (uint8_t)length);
    if (got != length) {
        i2c_unlock();
        return -1;
    }
    for (size_t i = 0; i < length; i++)
        data[i] = (uint8_t)Wire.read();
    i2c_unlock();
    return 0;
}

static int i2c_write(void *ctx, uint8_t address, uint8_t reg,
                     const uint8_t *data, size_t length)
{
    (void)ctx;
    i2c_lock();
    Wire.beginTransmission(address);
    Wire.write(reg);
    for (size_t i = 0; i < length; i++)
        Wire.write(data[i]);
    int rc = Wire.endTransmission(true) == 0 ? 0 : -1;
    i2c_unlock();
    return rc;
}

static void io_delay_us(void *ctx, uint32_t us)
{
    (void)ctx;
    delayMicroseconds(us);
}

static uint8_t s_general_last = 0;   /* last value written to reg 0x0A */
static bool    s_flip_invert = false; /* live A/B: invert FLIPSEL convention */

static int tusb_write_general(uint8_t v)
{
    s_general_last = v;
    return i2c_write(NULL, TUSB1046_I2C_ADDR, TUSB1046_REG_GENERAL, &v, 1);
}

/* ---- HAL callbacks ------------------------------------------------------- */

static uint32_t hal_millis(void *ctx)
{
    (void)ctx;
    return (uint32_t)millis();
}

static void hal_set_vbus(void *ctx, bool enable)
{
    (void)ctx;
    /* Strapping pin: held low by R29 at reset; only ever driven here. */
    digitalWrite(PIN_VBUS_SRC_EN, enable ? HIGH : LOW);
    s_vbus_on = enable;
}

static void hal_set_usb_role(void *ctx, usbc_usb_role_t role)
{
    (void)ctx;
    /* The ESP32-S3's own USB (IO19/20, hardwired to the connector) is the
     * flashing/console CDC device; there is no role switch to perform on
     * this board. Recorded for visibility only. */
    Serial.printf("[usbc] usb role -> %s\n",
                  role == USBC_USB_ROLE_HOST ? "host" :
                  role == USBC_USB_ROLE_DEVICE ? "device" : "off");
}

static int tusb_write_dp_eq(uint8_t setting)
{
    uint8_t v = (uint8_t)((setting << 4) | (setting & 0x0F));
    int rc = i2c_write(NULL, TUSB1046_I2C_ADDR, TUSB1046_REG_DPEQ10, &v, 1);
    rc |= i2c_write(NULL, TUSB1046_I2C_ADDR, TUSB1046_REG_DPEQ32, &v, 1);
    return rc;
}

static void hal_set_tusb1046(void *ctx, bool dp_enable, bool flipped)
{
    (void)ctx;
    uint8_t v;
    if (dp_enable) {
        /* s_flip_invert: live A/B of the CC-orientation -> FLIPSEL
         * convention (telnet 'f'); needed up here for AUX_SBU_OVR too. */
        bool flip_eff = flipped ^ s_flip_invert;
        /* Program sane EQ BEFORE enabling the lanes (see s_dp_eq_setting
         * comment: floating straps latch a link-killing 12.3 dB). */
        if (tusb_write_dp_eq(s_dp_eq_setting) != 0)
            Serial.println("[usbc] TUSB1046A EQ reg write FAILED");
        /* POLICY (2026-08-13, permanent): AUX snooping stays DISABLED.
         * We own the link state machine; the snooper is a second, silent
         * state machine inferring lane state from AUX traffic, and it
         * bit us three ways: reg 0x12 resets per attach so a missed
         * LANE_COUNT_SET write leaves lanes muted; the D0 write was
         * never snooped at all; and AUX-less diagnostic bitstreams
         * (TX_PROBE) transmit into a muted mux forever. Its only benefit
         * is power savings — irrelevant on a slot-powered card. If lane
         * trimming is ever wanted, drive DPx_DISABLE explicitly from
         * known link state instead. With snoop disabled, DPx_DISABLE
         * governs and defaults to all-enabled.
         *
         * ALSO force the AUX<->SBU crossbar closed (AUX_SBU_OVR, Table 6:
         * override "regardless of CTLSEL1 and FLIPSEL"). Found 2026-08-13:
         * with the automatic mapping the SBU pins idled at ~0.5 V on BOTH
         * lines (AUXN's 100k-to-3V3 bias never crossed the chip) — the
         * switch was effectively open, our AUX bursts reached the monitor
         * only capacitively, and the sink never replied to ANY request.
         * 01 = AUXp->SBU1/AUXn->SBU2 (normal), 10 = crossed (flipped). */
        uint8_t dc = TUSB_DP_SNOOP_DIS | (flip_eff ? 0x20 : 0x10);
        if (i2c_write(NULL, TUSB1046_I2C_ADDR, TUSB1046_REG_DPCTL3, &dc, 1) != 0)
            Serial.println("[usbc] TUSB1046A snoop-disable/aux-ovr write FAILED");
        /* FLIPSEL steers BOTH the SBU/AUX crossbar and the SS-lane
         * crosspoint (flip_eff computed at the top of this branch). */
        v = TUSB_GEN_CTLSEL_DP4 | TUSB_GEN_HPDIN_OVR | TUSB_GEN_EQ_OVERRIDE |
            (flip_eff ? TUSB_GEN_FLIPSEL : 0);
    } else {
        v = TUSB_GEN_CTLSEL_USB3 | TUSB_GEN_HPDIN_OVR;
    }
    if (tusb_write_general(v) != 0)
        Serial.println("[usbc] TUSB1046A general reg write FAILED");
    s_dp_mux_on = dp_enable;
}

static void hal_set_tusb_hpd(void *ctx, bool level)
{
    (void)ctx;
    (void)level;
    /* I2C mode: pin 23 is NC and HPDIN_OVRRIDE is set — the mux never sees
     * HPD; lane trimming rides the AUX snooper. Nothing to do. */
}

static void hal_set_fpga_dp_enable(void *ctx, bool enable)
{
    (void)ctx;
    /* Gateware-side DP gating hook (SPEC.md §9: hold the DP source in
     * reset until DP Configure is ACKed). The bring-up gateware keys off
     * HPD alone; when a gate is added it will be an OSPI register write
     * here. Recorded for the status command meanwhile. */
    s_fpga_dp_en = enable;
}

static void hal_set_fpga_hpd(void *ctx, bool level)
{
    (void)ctx;
    digitalWrite(PIN_DP_HPD_OUT, level ? HIGH : LOW);
    s_hpd_level = level;
}

/* Telnet 'r': drop HPD to the FPGA for ~250 ms and restore — restarts
 * the gateware's blind AUX/training ladder on demand (BLIND_SINK resets
 * on hpd_present falling). No cables touched, nothing power-cycles. */
extern "C" void usbc_hpd_retrain(void)
{
    if (!s_hpd_level) {
        osd_log("RETRAIN: HPD is low (no sink) - nothing to pulse");
        return;
    }
    osd_log("RETRAIN: pulsing HPD low 250 ms (ladder restart)");
    digitalWrite(PIN_DP_HPD_OUT, LOW);
    vTaskDelay(pdMS_TO_TICKS(250));
    digitalWrite(PIN_DP_HPD_OUT, HIGH);
}

/* Telnet 'u': raw FUSB302B status snapshot for in-slot attach debugging. */
extern "C" void usbc_fusb_dump_log(void)
{
    static const uint8_t regs[] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x3C, 0x3E};
    char line[64];
    char *w = line;
    w += snprintf(w, sizeof line, "FUSB");
    for (size_t i = 0; i < sizeof regs; i++) {
        uint8_t v = 0xEE;
        (void)i2c_read(NULL, 0x22, regs[i], &v, 1);
        w += snprintf(w, sizeof line - (w - line), " %02X:%02X", regs[i], v);
    }
    osd_log(line);
}

static void hal_pulse_fpga_hpd_irq(void *ctx)
{
    (void)ctx;
    /* IRQ_HPD = 0.5-1.0 ms low pulse while HPD is otherwise high
     * (hotplug_decode's IRQ threshold is 0.5 ms, disconnect is 2 ms). */
    if (!s_hpd_level)
        return;
    digitalWrite(PIN_DP_HPD_OUT, LOW);
    delayMicroseconds(750);
    digitalWrite(PIN_DP_HPD_OUT, HIGH);
}

static void hal_log(void *ctx, usbc_log_level_t level, const char *message)
{
    (void)ctx;
    Serial.printf("[usbc] %s\n", message);
    if (level >= USBC_LOG_WARNING)
        osd_log("USBC: %s", message);
}

/* ---- task + interrupt ---------------------------------------------------- */

static void IRAM_ATTR fusb_int_isr(void)
{
    BaseType_t woke = pdFALSE;
    if (s_wake)
        xSemaphoreGiveFromISR(s_wake, &woke);
    if (woke)
        portYIELD_FROM_ISR();
}

static void usbc_task(void *arg)
{
    (void)arg;
    for (;;) {
        /* 1 ms cadence, woken early by INT_N falling. */
        xSemaphoreTake(s_wake, pdMS_TO_TICKS(1));
        usbc_port_task(&s_port);
    }
}

/* ---- public -------------------------------------------------------------- */

extern "C" bool usbc_pd_init(void)
{
    if (s_running)
        return true;

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 400000);

    /* Park the mux in its documented disable state (DP lanes off, HPDIN
     * ignored) before PD policy starts. A NAK here usually means the board
     * is unpowered on the 3.3 V rail or the mux is missing. */
    if (tusb_write_general(TUSB_GEN_CTLSEL_USB3 | TUSB_GEN_HPDIN_OVR) != 0)
        Serial.println("[usbc] warning: TUSB1046A (0x12) not responding");

    fusb302_io_t io = {};
    io.context = NULL;
    io.read = i2c_read;
    io.write = i2c_write;
    io.delay_us = io_delay_us;

    usbc_port_hal_t hal = {};
    hal.context = NULL;
    hal.millis = hal_millis;
    hal.set_vbus_source = hal_set_vbus;
    hal.set_usb_role = hal_set_usb_role;
    hal.set_tusb1046 = hal_set_tusb1046;
    hal.set_tusb_hpd = hal_set_tusb_hpd;
    hal.set_fpga_dp_enable = hal_set_fpga_dp_enable;
    hal.set_fpga_hpd = hal_set_fpga_hpd;
    hal.pulse_fpga_hpd_irq = hal_pulse_fpga_hpd_irq;
    hal.log = hal_log;

    usbc_port_config_t config;
    usbc_port_default_config(&config);   /* 5 V / 1 A single PDO */

    if (usbc_port_init(&s_port, &config, &hal, &io,
                       FUSB302_I2C_ADDRESS_DEFAULT) != 0) {
        Serial.println("[usbc] FUSB302B (0x22) init FAILED");
        osd_log("USBC: PD PHY NOT FOUND");
        return false;
    }
    if (usbc_port_enable(&s_port) != 0) {
        Serial.println("[usbc] port enable FAILED");
        return false;
    }

    s_i2c_lock = xSemaphoreCreateMutex();
    s_wake = xSemaphoreCreateBinary();
    attachInterrupt(digitalPinToInterrupt(PIN_FUSB_INT), fusb_int_isr, FALLING);
    xTaskCreatePinnedToCore(usbc_task, "usbc_pd", 4096, NULL, 6, NULL, 1);
    s_running = true;
    osd_log("USBC: PD STACK UP");
    return true;
}

extern "C" void usbc_pd_status(void)
{
    if (!s_running) {
        Serial.println("pd: stack not running");
        return;
    }
    Serial.printf("pd: state %s (%s power role, %s data role)\n",
                  usbc_port_state_name(s_port.state),
                  s_port.power_sink ? "sink" : "source",
                  s_port.data_dfp ? "DFP" : "UFP");
    Serial.printf("    orientation CC%d, HPD %d, VBUS-src %d, mux-DP %d, "
                  "fpga-dp-en %d\n",
                  s_port.polarity == FUSB302_POLARITY_CC2 ? 2 : 1,
                  (int)s_hpd_level, (int)s_vbus_on, (int)s_dp_mux_on,
                  (int)s_fpga_dp_en);
}

/* TUSB1046A register dump via osd_log (OSD + telnet tee). The DCI AUX
 * snooper sits directly on the AUX pair and parses our DPCD writes
 * (LANE_COUNT_SET, SET_POWER_STATE) to auto-trim lanes — its registers
 * are on-the-wire evidence of whether the FPGA's AUX transmissions are
 * well-formed, which nothing else on this board can observe. */
extern "C" void usbc_mux_dump_log(void)
{
    uint8_t v[8];
    char line[41];
    int n = 0;
    for (uint8_t r = 0x08; r <= 0x0B; r++)
        if (i2c_read(NULL, TUSB1046_I2C_ADDR, r, &v[n], 1) != 0)
            v[n] = 0xEE, n++;
        else
            n++;
    snprintf(line, sizeof(line), "MUX 08-0B: %02X %02X %02X %02X",
             v[0], v[1], v[2], v[3]);
    osd_log("%s", line);
    n = 0;
    for (uint8_t r = 0x10; r <= 0x13; r++)
        if (i2c_read(NULL, TUSB1046_I2C_ADDR, r, &v[n], 1) != 0)
            v[n] = 0xEE, n++;
        else
            n++;
    snprintf(line, sizeof(line), "MUX 10-13: %02X %02X %02X %02X",
             v[0], v[1], v[2], v[3]);
    osd_log("%s", line);
}

/* Cycle the DP receiver EQ through bring-up presets, live over telnet.
 * Table 7 gains at 4.05 GHz: 0=1.0 dB, 3=6.5 dB, 6=9.5 dB, 10=12.3 dB
 * (10 = the floating-strap default that motivated all of this). Writes
 * take effect immediately on the running link — A/B without reflashing. */
extern "C" void usbc_mux_eq_cycle(void)
{
    static const uint8_t presets[4] = { 0, 3, 6, 10 };
    static const char *gains[4] = { "1.0", "6.5", "9.5", "12.3" };
    static uint8_t idx = 0;
    idx = (uint8_t)((idx + 1) & 3);
    s_dp_eq_setting = presets[idx];
    if (tusb_write_dp_eq(s_dp_eq_setting) == 0)
        osd_log("MUX EQ: SETTING %u (%s DB)", presets[idx], gains[idx]);
    else
        osd_log("MUX EQ: WRITE FAILED");
}

/* Toggle the FLIPSEL convention live (telnet 'f') and rewrite the mux
 * General register in place. If the CC->FLIPSEL mapping convention is
 * inverted, this one bit un-crosses the AUX pair AND moves the DP lanes
 * onto the connector pins the sink actually watches. */
extern "C" void usbc_mux_flip_toggle(void)
{
    s_flip_invert = !s_flip_invert;
    uint8_t v = (uint8_t)(s_general_last ^ TUSB_GEN_FLIPSEL);
    if (i2c_write(NULL, TUSB1046_I2C_ADDR, TUSB1046_REG_GENERAL, &v, 1) == 0) {
        s_general_last = v;
        osd_log("MUX FLIPSEL NOW %d (INVERT %d)",
                (v & TUSB_GEN_FLIPSEL) ? 1 : 0, (int)s_flip_invert);
    } else {
        osd_log("MUX FLIP: WRITE FAILED");
    }
}

/* Condensed PD status through osd_log: reaches the OSD *and* every telnet
 * console session (the tee) — the only live status path once a monitor
 * occupies the USB-C port. Lines fit the 40-col console. */
extern "C" void usbc_pd_status_log(void)
{
    if (!s_running) {
        osd_log("PD: STACK NOT RUNNING");
        return;
    }
    osd_log("PD: %s %s/%s", usbc_port_state_name(s_port.state),
            s_port.power_sink ? "SNK" : "SRC",
            s_port.data_dfp ? "DFP" : "UFP");
    osd_log("PD: CC%d HPD=%d SRC=%d MUX=%d EN=%d",
            s_port.polarity == FUSB302_POLARITY_CC2 ? 2 : 1,
            (int)s_hpd_level, (int)s_vbus_on, (int)s_dp_mux_on,
            (int)s_fpga_dp_en);
}

#endif /* A2MEGA_HAS_USBC_PD */
