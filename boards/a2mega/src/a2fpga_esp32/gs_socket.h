/* gs_socket.h — auto-arm for the in-socket 65C816 (a2mega 138B gateware)
 *
 * The FPGA never drives the IIgs CPU socket until the arm bit in the GS
 * control register is set (boards/a2mega/docs/gs_socket_65816_scoping.md
 * §8b). This module makes that automatic and safe:
 *
 *   boot -> LISTEN (input shifter only; nothing driven)
 *        -> PHI2 alive AND /RES high for a few samples -> ARM (the core
 *           cold-starts at once; arming while the machine is still in its
 *           power-on reset keeps that reset asserted — bench G31/G32)
 *        -> PHI2 lost for a while (machine off) -> back to LISTEN
 *
 * A card without the ribbon never sees PHI2 (the pin is pulled down), so
 * it never arms; a 60K bitstream has no socket logic and reads the
 * status as 0. The setting gs_socket_off (menu: SETTINGS -> GS SOCKET)
 * disables it entirely; the telnet/CLI `gs` commands take manual control
 * until `gs auto` hands it back.
 */
#ifndef GS_SOCKET_H
#define GS_SOCKET_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Call from the disk task loop (~2 ms period). Cheap when idle. */
void gs_socket_poll(void);

/* Setting changed (menu/CLI): re-evaluate immediately. */
void gs_socket_apply(void);

/* Manual control taken via `gs arm|off|listen|set 0 ..`: the state machine
 * stops touching CTRL until gs_socket_resume(). */
void gs_socket_manual(void);
void gs_socket_resume(void);
bool gs_socket_is_manual(void);

/* True once the auto-arm has settled: armed, disabled, manual, or listening
 * long enough (300 ms) to conclude there is no ribbon / no powered socket.
 * The Apple II reset release waits for this so the core is armed BEFORE
 * /RES rises, like the real chip (bench G30: the release beat the arm). */
bool gs_socket_ready(void);

/* One-line state for `gs` / status pages, e.g. "AUTO: ARMED (PHI2 ALIVE)". */
const char *gs_socket_state_str(void);

/* Storage-ready release of the Apple II reset (0x2E.0). disk.c calls this
 * instead of writing the register so an arm sequence in flight keeps its
 * own reset assert (0x2E.1). */
void gs_socket_a2_release(void);

/* Register-window helpers shared with telnetd (locked). */
uint8_t gs_socket_reg_read(uint8_t idx);
void    gs_socket_reg_write(uint8_t idx, uint8_t val);

#ifdef __cplusplus
}
#endif
#endif
