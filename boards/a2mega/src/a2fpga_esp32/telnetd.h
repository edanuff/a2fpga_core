/*
 * telnetd — remote console/menu mirror on TCP port 23 (see telnetd.c).
 */
#ifndef _TELNETD_H
#define _TELNETD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Spawn the server task. Call once WiFi is being brought up; the task waits
 * for lwIP core init itself. Safe to call more than once. */
void telnetd_init(void);

/* Tee one console line to a connected client (called by osd_log from any
 * thread; never blocks on the network — ring buffered). No-op before init
 * or with no client. */
void telnetd_console_tee(const char *line);

#ifdef __cplusplus
}
#endif

#endif
