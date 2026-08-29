/*
 * wedge_watch.h — automatic quiet-frozen-wedge recovery (policy layer).
 *
 * The FPGA's wedge-suspect detector (aux_channel.v, U: bit7 of the D4
 * telemetry line) fires on the hub's quiet-frozen wedge: link trained,
 * LINK_STATUS_UPDATED stuck latched across polls, sink not receiving,
 * sustained ~11 s. Live-validated 08-26: U:88 on a real wedge, U:0x on
 * every clean attach. The hub raises NOTHING during the wedge (ESI
 * space zero, no attention) — recovery is the only lever, and the
 * virtual replug clears it fast (3/3 on the bench).
 *
 * This module watches the FPGA UART telemetry the main loop already
 * relays, and fires usbc_virtual_replug() under a strictly bounded
 * policy. Pathological cases designed against:
 *   - bad cable / no display: the FPGA detector is unreachable there
 *     (presence gate holds pre-training), so the flag never rises;
 *   - a wedge that RECURS after every replug: the attempt budget lives
 *     in NVS, so it survives the board reboot a hub-backfed replug
 *     causes — at most WEDGE_MAX_ATTEMPTS replugs total, then manual;
 *   - the budget re-arms ONLY on observed real streaming (K: nonzero
 *     sustained), so a display that never comes back cannot loop;
 *   - UART corruption: three CONSECUTIVE flagged D4 lines required;
 *   - kick-era false positive (good picture, frozen status): if it ever
 *     occurs, cost is bounded at WEDGE_MAX_ATTEMPTS brief blackouts.
 *
 * Telnet 'w' toggles the watch (persisted) and prints status.
 */
#ifndef WEDGE_WATCH_H
#define WEDGE_WATCH_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Load persisted enable/budget (call after settings_init: NVS is up). */
void wedge_watch_init(void);

/* Feed one complete FPGA telemetry line (no CR/LF). Cheap; call for
 * every line the UART relay assembles. */
void wedge_watch_line(const char *line);

/* Telnet 'w': toggle enable (persisted) and log status. */
void wedge_watch_toggle_cmd(void);

#ifdef __cplusplus
}
#endif

#endif /* WEDGE_WATCH_H */
