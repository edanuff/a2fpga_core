/*
 * sn75dp130 — minimal SN75DP130 DisplayPort redriver support (a2p25).
 *
 * The DP130 sits between the FPGA's D-PHY lanes and the TUSB546A mux and
 * shares the ESP32's DP I2C bus (usbc_glue owns Wire + the bus mutex).
 * Policy: the part is LEFT IN ITS POWER-UP (AUTO) MODE — it snoops the
 * DPCD link-training writes on its AUX pass-through and sets its own
 * VOD/pre-emphasis/EQ (boards/a2p25/docs/DP_ALTMODE_DPHY_DESIGN.md). This
 * driver never writes on its own; I2C is for debug reads and explicit,
 * operator-issued overrides (CLI "dp130 w <reg> <val>").
 *
 * No register map is encoded here: the TI datasheet (SLLSE57) was not
 * available locally when this was written, so the dump is raw. See
 * boards/a2p25/docs/ESP32_FW_A2P25.md.
 */
#ifndef SN75DP130_H
#define SN75DP130_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 7-bit address with ADDR_EQ (pin 3) strapped to GND, per the design doc.
 * dp130_init() falls back to scanning 0x2C-0x2F if this one NAKs. */
#define DP130_I2C_ADDR_DEFAULT 0x2C

/* Probe the part (address-only + one register read). Logs the result via
 * osd_log. Writes nothing. Returns true if it answered. Call after
 * usbc_pd_init() (which starts the I2C bus). */
bool    dp130_init(void);
bool    dp130_present(void);
uint8_t dp130_addr(void);

/* Raw register access (0 = OK, -1 = NAK). */
int dp130_read(uint8_t reg, uint8_t *val);
int dp130_write(uint8_t reg, uint8_t val);

/* Dump registers [first, first+count) via osd_log, 8 per 40-col line;
 * unreadable registers print as "--". Telnet 'n' uses 0x00..0x3F. */
void dp130_dump_log(uint8_t first, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif
