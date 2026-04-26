/*
 * FPGA Screen — OSD text output via SDRAM XFER writes.
 *
 * SDRAM address mapping (from PicoSOC a2mem.c):
 *   PicoSOC 0x04020800 = SDRAM byte 0x020800 (OSD text page)
 *
 * Address encoding (Apple II interleaved text page):
 *   addr = 0x020800 + (0x50 * (y >> 3)) + ((y & 0x07) << 8) + (x << 1)
 *
 * Characters are stored as Apple II screen codes: ASCII + 128 for normal text.
 * v1 simplification: cursor wraps to top (no scroll).
 */

#include "fpga_screen.h"
#include "fpga_spi.h"

#define OSD_BASE  0x020800
#define OSD_SIZE  2048
#define SCREEN_W  40
#define SCREEN_H  24

static int cursor_h = 0;
static int cursor_v = 0;

static uint32_t screen_addr(int x, int y)
{
    return OSD_BASE + (0x50 * (y >> 3)) + ((y & 0x07) << 8) + (x << 1);
}

void fpga_screen_clear(void)
{
    fpga_spi_xfer_fill(FPGA_SPACE_SDRAM, OSD_BASE, 0xA0, OSD_SIZE);
    cursor_h = 0;
    cursor_v = 0;
}

static void newline(void)
{
    cursor_h = 0;
    cursor_v++;
    if (cursor_v >= SCREEN_H) {
        cursor_v = 0;  /* Wrap to top (no scroll in v1) */
    }
}

void fpga_screen_putchar(uint8_t c)
{
    if (c == '\n') {
        newline();
        return;
    }
    if (c < 32) return;

    uint8_t sc = c + 128;  /* Apple II normal text screen code */
    fpga_spi_xfer_write(FPGA_SPACE_SDRAM, screen_addr(cursor_h, cursor_v), &sc, 1);

    cursor_h++;
    if (cursor_h >= SCREEN_W) {
        newline();
    }
}

void fpga_screen_puts(const char *str)
{
    while (*str) {
        fpga_screen_putchar((uint8_t)*str++);
    }
}

void fpga_screen_home(void)
{
    cursor_h = 0;
    cursor_v = 0;
}
