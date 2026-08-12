/*
 * usbc_glue — a2mega 1.0a3 USB-C PD stack bring-up (see usbc_glue.cpp).
 * Only built when board_pins.h says A2MEGA_HAS_USBC_PD.
 */
#ifndef _USBC_GLUE_H
#define _USBC_GLUE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Start the PD policy task (FUSB302B + TUSB1046A on I2C, HPD to the FPGA).
 * Returns false if the PD PHY does not answer. Safe to call once at setup;
 * runs independently of the FPGA link. */
bool usbc_pd_init(void);

/* Print PD state to Serial (the CLI 'pd' command). */
void usbc_pd_status(void);
void usbc_pd_status_log(void);  /* condensed, via osd_log (OSD + telnet tee) */
void usbc_mux_dump_log(void);   /* TUSB1046A regs via osd_log (AUX snooper) */
void usbc_mux_eq_cycle(void);   /* cycle DP RX EQ presets live (telnet 'e') */

#ifdef __cplusplus
}
#endif

#endif
