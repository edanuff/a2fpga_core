/*
 * sn75dp130.c — see sn75dp130.h. Probe + raw dump/write only; the part is
 * left in auto mode.
 */
#include <stdio.h>

#include "board_pins.h"
#include "sn75dp130.h"
#include "usbc_glue.h"

void osd_log(const char *fmt, ...);

#if A2MEGA_HAS_DP130

static uint8_t s_addr = DP130_I2C_ADDR_DEFAULT;
static bool    s_present;

static bool try_addr(uint8_t a)
{
    uint8_t v;
    if (!usbc_i2c_probe(a))
        return false;
    /* Address ACK alone could be any part; require a register read too. */
    return usbc_i2c_read(a, 0x00, &v, 1) == 0;
}

bool dp130_init(void)
{
    s_present = false;
    if (try_addr(DP130_I2C_ADDR_DEFAULT)) {
        s_addr = DP130_I2C_ADDR_DEFAULT;
        s_present = true;
    } else {
        for (uint8_t a = DP130_I2C_ADDR_DEFAULT + 1; a <= 0x2F; a++) {
            if (try_addr(a)) {
                s_addr = a;
                s_present = true;
                break;
            }
        }
    }
    if (s_present)
        osd_log("DP130: FOUND AT 0x%02X (AUTO MODE)", s_addr);
    else
        osd_log("DP130: NOT FOUND (0x2C-0x2F)");
    return s_present;
}

bool    dp130_present(void) { return s_present; }
uint8_t dp130_addr(void)    { return s_addr; }

int dp130_read(uint8_t reg, uint8_t *val)
{
    return usbc_i2c_read(s_addr, reg, val, 1);
}

int dp130_write(uint8_t reg, uint8_t val)
{
    return usbc_i2c_write(s_addr, reg, &val, 1);
}

void dp130_dump_log(uint8_t first, uint8_t count)
{
    if (!s_present && !try_addr(s_addr)) {
        osd_log("DP130: NO ACK AT 0x%02X", s_addr);
        return;
    }
    s_present = true;
    osd_log("DP130 @0x%02X REGS %02X-%02X:", s_addr, first,
            (uint8_t)(first + count - 1));
    for (unsigned base = first; base < (unsigned)first + count; base += 8) {
        char line[41];
        int n = snprintf(line, sizeof line, "%02X:", base);
        for (unsigned r = base; r < base + 8 && r < (unsigned)first + count; r++) {
            uint8_t v;
            if (dp130_read((uint8_t)r, &v) == 0)
                n += snprintf(line + n, sizeof line - n, " %02X", v);
            else
                n += snprintf(line + n, sizeof line - n, " --");
        }
        osd_log("%s", line);
    }
}

#endif /* A2MEGA_HAS_DP130 */
