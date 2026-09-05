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
void usbc_mux_eq_step(int dir); /* fine step through all 16 settings (telnet '+'/'-') */
void usbc_mux_lanes23_toggle(void); /* M1: toggle unused mux DP2/DP3 inputs (telnet 'l') */
void usbc_virtual_replug(void); /* V2.1: 3s CC-open detach + fresh attach ceremony (telnet 'v') */
void usbc_virtual_replug_fast(void); /* auto-recovery: 1.5s hold (wedge_watch) */
void usbc_mux_flip_toggle(void);/* toggle FLIPSEL convention live ('f') */
void usbc_hpd_retrain(void);    /* pulse HPD low 250 ms: ladder restart ('r') */
void usbc_fusb_dump_log(void);  /* raw FUSB302B status regs ('u') */
void usbc_trace_dump_log(void); /* PD event trace ring ('t') */
void usbc_modes_census_log(void); /* adapter lane census: last Discover Modes
                                   * VDOs, pin-assignment decode ('d').
                                   * C/E = 4-lane (a2p25 RBR x4 OK);
                                   * D/F = 2-lane + USB3 (720p fallback). */

#ifdef __cplusplus
}
#endif

#endif
