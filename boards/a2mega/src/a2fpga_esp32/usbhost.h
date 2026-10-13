/*
 * usbhost — ESP32-S3 USB host on the module's own USB-C port (USB1 on the
 * a2p25 / a2mega 1.0a3): the port the BL616 uses on the Tang Nano 20K,
 * wired the same way (D+/D- on the OTG pins, VBUS tied to the board's
 * +5 V through a PTC, Rd on CC). Gamepads, HID keyboards/remotes and
 * (later) mass storage plug in there with the usual C-to-A adapters.
 *
 * Role policy: USB1 is also the Mac CDC/JTAG port. The firmware boots in
 * device mode (hardware USB-Serial-JTAG); usbhost_start() waits a short
 * while for a host to enumerate us and, if none does, re-routes the PHY
 * to the OTG controller in host mode. From then on the CDC console is
 * gone (telnet remains) until the next reset with a Mac attached.
 *
 * Output: a button word in the menu's A2PAD_* layout (a2fpga_regs.h),
 * merged by menu_tick() with the FPGA pad readback.
 */
#ifndef USBHOST_H
#define USBHOST_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Start the role-decision task (production builds only). Safe to call once. */
void     usbhost_start(void);
/* true once the PHY has been switched to OTG host mode */
bool     usbhost_active(void);
/* a gamepad (XInput) is attached and initialised */
bool     usbhost_pad_present(void);
/* any HID keyboard / consumer-control device is attached */
bool     usbhost_hid_present(void);
/* current buttons, A2PAD_* layout: gamepad | keyboard/remote mapping */
uint16_t usbhost_buttons(void);
/* one-line status for the OSD/telnet status screens */
const char *usbhost_status_str(void);

#ifdef __cplusplus
}
#endif
#endif
