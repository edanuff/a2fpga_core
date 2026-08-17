/*
 * FPGA Screen — OSD text output via XFER writes to the OSD text page.
 *
 * a2mega version: the OSD text page is a LINEAR 40x24 array of Apple II
 * screen codes at offset y*40+x in XFER space A2SPACE_OSD (rendered by
 * osd_text_overlay.sv). Same public API as the a2n20v2-Enhanced version.
 */

#ifndef _FPGA_SCREEN_H
#define _FPGA_SCREEN_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FPGA_SCREEN_W 40
#define FPGA_SCREEN_H 24

/* Clear the 40-column text screen (fill with 0xA0 = space) */
void fpga_screen_clear(void);

/* Write a single character at the current cursor position */
void fpga_screen_putchar(uint8_t c);

/* Write a null-terminated string */
void fpga_screen_puts(const char *str);

/* Reset cursor to (0, 0) */
void fpga_screen_home(void);

/* Move the cursor (clamped to the screen) */
void fpga_screen_goto(int x, int y);

/* Inverse-video mode for subsequent putchar/puts (Apple II screen codes
 * $00-$3F). Lowercase is uppercased; only uppercase/digits/symbols render. */
void fpga_screen_set_inverse(bool inverse);

/* Telnet menu mirror: read one 40-byte row of the local shadow (Apple II
 * screen codes), and a generation counter bumped on every shadow write so
 * the mirror knows when to repaint. Lock-free; a torn read self-heals on
 * the next generation change. */
const uint8_t *fpga_screen_shadow_row(int y);
uint32_t fpga_screen_shadow_gen(void);

#ifdef __cplusplus
}
#endif

#endif
