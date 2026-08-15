/*
 * telnetd — remote console/menu mirror on TCP port 23.
 *
 * a2mega ESP32 port of the a2n20v2-Enhanced firmware_host/telnetd.c
 * (usb_osal -> FreeRTOS, fpga_spi -> fpga_link/a2spi, XINPUT_* -> A2PAD_*).
 * The Enhanced-only diagnostics (bus-event FIFO snapshot/scope/trigger,
 * boot timeline) are not ported — that capture fabric does not exist in the
 * a2mega gateware.
 *
 * One client at a time. Two views, toggled by single keys:
 *
 *   'c'  CONSOLE (default): dumps the osd_console backlog, then streams
 *        every new osd_log() line live (the tee is a lock-protected line
 *        ring so logging threads never block on the network).
 *   'm'  MENU: mirrors the FPGA's 40x24 OSD text page (menu, install pages,
 *        whatever is painted) as ANSI, repainted on change, and maps keys
 *        to pad buttons so the whole menu is drivable remotely:
 *          up/down    = D-pad          Enter/a = A (OK)
 *          left/right = D-pad L/R      Backspace/b = B (back)
 *          y = Y (menu<->console view) s/Tab = SELECT (Apple II <-> MCU)
 *   'q'  disconnect.
 *
 * On 1.0a3 this is the PRIMARY console: the USB-C port faces the DP
 *  monitor, so there is no PC serial link while the board is in service.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "lwip/sockets.h"
#include "lwip/netif.h"

#include "fpga_screen.h"
#include "osd_console.h"
#include "menu.h"
#include "board_pins.h"
#if A2MEGA_HAS_USBC_PD
#include "usbc_glue.h"
#endif
#include "a2fpga_regs.h"
#include "telnetd.h"

#define TELNET_PORT     23
#define TEE_LINES       32
#define TEE_COLS        80

/* ---- console tee ring (written by osd_log from any thread) -------------- */
static char     s_tee[TEE_LINES][TEE_COLS + 1];
static volatile uint32_t s_tee_wr;        /* total lines ever written  */
static uint32_t s_tee_rd;                 /* telnet thread's position  */
static SemaphoreHandle_t s_tee_lock;
static volatile bool s_client_up;

void telnetd_console_tee(const char *line)
{
    if (!s_client_up || !s_tee_lock)
        return;
    xSemaphoreTake(s_tee_lock, portMAX_DELAY);
    snprintf(s_tee[s_tee_wr % TEE_LINES], sizeof(s_tee[0]), "%s", line);
    s_tee_wr++;
    xSemaphoreGive(s_tee_lock);
}

/* ---- helpers ------------------------------------------------------------- */
/* Set when a send errors or times out (SO_SNDTIMEO): the peer vanished
 * without closing (crashed client, port scanner) and its window filled.
 * tn_send goes no-op and session() bails, so the next client can connect. */
static bool s_peer_dead;

static int tn_send(int fd, const void *buf, int len)
{
    const char *p = buf;
    while (len > 0 && !s_peer_dead) {
        int n = lwip_send(fd, p, len, 0);
        if (n <= 0) {
            s_peer_dead = true;
            return -1;
        }
        p += n;
        len -= n;
    }
    return s_peer_dead ? -1 : 0;
}

static void tn_puts(int fd, const char *s)
{
    tn_send(fd, s, (int)strlen(s));
}

/* non-static wrapper for C++ side (jtag bridge toggle feedback) */
void tn_puts_ext(int fd, const char *s) { tn_puts(fd, s); }

/* Render one 40-char row of Apple II screen codes as ANSI. Inverse video
 * is codes $00-$3F ($00-$1F = '@'+c, $20-$3F = c); normal is ASCII+0x80. */
static void render_row(int fd, const uint8_t *row)
{
    char out[40 * 4 + 16];
    int n = 0;
    bool inv = false;
    for (int x = 0; x < 40; x++) {
        uint8_t c = row[x];
        bool want_inv = c < 0x40;
        char ch;
        if (want_inv)
            ch = (c < 0x20) ? (char)('@' + c) : (char)c;
        else
            ch = (char)(c & 0x7F);
        if (ch < 0x20 || ch > 0x7E)
            ch = ' ';
        if (want_inv != inv) {
            n += snprintf(out + n, sizeof(out) - n, want_inv ? "\x1b[7m" : "\x1b[0m");
            inv = want_inv;
        }
        if (n < (int)sizeof(out) - 8)
            out[n++] = ch;
    }
    if (inv)
        n += snprintf(out + n, sizeof(out) - n, "\x1b[0m");
    n += snprintf(out + n, sizeof(out) - n, "\r\n");
    tn_send(fd, out, n);
}

static void render_screen(int fd)
{
    tn_puts(fd, "\x1b[H");                 /* home, no clear: less flicker */
    for (int y = 0; y < FPGA_SCREEN_H; y++)
        render_row(fd, fpga_screen_shadow_row(y));
}

/* Map a received key to a one-tick pad pulse (A2PAD_* bits). Returns 0 if
 * unmapped. st tracks a tiny ESC [ sequence parser for arrows across calls. */
static uint16_t key_to_buttons(uint8_t ch, int *st)
{
    if (*st == 1) {                        /* got ESC */
        *st = (ch == '[') ? 2 : 0;
        return 0;
    }
    if (*st == 2) {                        /* got ESC [ */
        *st = 0;
        switch (ch) {
        case 'A': return A2PAD_U;
        case 'B': return A2PAD_D;
        case 'C': return A2PAD_R;          /* choice next / value +   */
        case 'D': return A2PAD_L;          /* choice prev / value -   */
        }
        return 0;
    }
    switch (ch) {
    case 0x1b: *st = 1; return 0;
    case '\r': case '\n': case 'a': return A2PAD_A;      /* OK    */
    case 0x7f: case 0x08: case 'b': return A2PAD_B;      /* back  */
    case 'y':                       return A2PAD_Y;      /* view  */
    case 's': case '\t':            return A2PAD_SELECT; /* select*/
    }
    return 0;
}

/* ---- session ------------------------------------------------------------- */
static void session(int fd)
{
    s_peer_dead = false;
    /* char-at-a-time: WILL ECHO, WILL SGA, DO SGA */
    static const uint8_t nego[] = { 255, 251, 1, 255, 251, 3, 255, 253, 3 };
    tn_send(fd, nego, sizeof(nego));
    tn_puts(fd, "\r\nA2FPGA a2mega remote console\r\n"
                "keys: c=console m=menu p=pd x=regs e=eq f=flip r=retrain u=fusb j=jtagbridge q=quit\r\n"
                "menu: up/down move, left/right change, enter/a=ok,\r\n"
                "      esc/backspace/b=back, y=view, s/tab=select\r\n\r\n");

    bool menu_mode = false;
    int esc_st = 0, iac_st = 0;
    uint32_t last_paint = 0;

    /* start in console mode: replay the on-screen backlog */
    {
        char snap[23][40];
        int n = osd_console_snapshot(snap, 23);
        for (int i = 0; i < n; i++) {
            tn_puts(fd, snap[i]);
            tn_puts(fd, "\r\n");
        }
        xSemaphoreTake(s_tee_lock, portMAX_DELAY);
        s_tee_rd = s_tee_wr;               /* live from here on */
        xSemaphoreGive(s_tee_lock);
    }

    for (;;) {
        if (s_peer_dead)
            return;                        /* send timed out/failed */
        /* input (non-blocking-ish: 50 ms poll via SO_RCVTIMEO) */
        uint8_t ch;
        int r = lwip_recv(fd, &ch, 1, 0);
        if (r == 0)
            return;                        /* closed */
        if (r < 0 && errno != EWOULDBLOCK && errno != EAGAIN)
            return;                        /* reset/keepalive-reaped, not the 50 ms poll */
        if (r != 1 && esc_st == 1) {
            /* lone ESC (no sequence followed within the 50 ms poll): back */
            esc_st = 0;
            if (menu_mode)
                menu_inject(A2PAD_B);
        }
        if (r == 1) {
            if (iac_st == 1) {             /* IAC verb  */
                iac_st = (ch >= 251 && ch <= 254) ? 2 : 0;
                continue;
            }
            if (iac_st == 2) {             /* IAC verb option */
                iac_st = 0;
                continue;
            }
            if (ch == 255) {
                iac_st = 1;
                continue;
            }
            if (esc_st == 0 && ch == 'q')
                return;
            if (esc_st == 0 && ch == 'c' && menu_mode) {
                menu_mode = false;
                tn_puts(fd, "\x1b[0m\x1b[2J\x1b[H-- console --\r\n");
                xSemaphoreTake(s_tee_lock, portMAX_DELAY);
                s_tee_rd = s_tee_wr;
                xSemaphoreGive(s_tee_lock);
                continue;
            }
            if (esc_st == 0 && ch == 'p' && !menu_mode) {
                /* PD status into the console tee: works with the monitor
                 * on the port, when telnet is the only console. */
#if A2MEGA_HAS_USBC_PD
                usbc_pd_status_log();
#else
                tn_puts(fd, "pd: not built for this board rev\r\n");
#endif
                continue;
            }
            if (esc_st == 0 && ch == 'f' && !menu_mode) {
                /* Flip the mux FLIPSEL convention live: tests whether the
                 * CC-orientation mapping is inverted (AUX crossed + lanes
                 * on unwatched pins) without any reflash. */
#if A2MEGA_HAS_USBC_PD
                usbc_mux_flip_toggle();
#else
                tn_puts(fd, "flip: not built for this board rev\r\n");
#endif
                continue;
            }
            if (esc_st == 0 && ch == 'r' && !menu_mode) {
                /* Restart the FPGA's blind training ladder via HPD pulse. */
#if A2MEGA_HAS_USBC_PD
                usbc_hpd_retrain();
#else
                tn_puts(fd, "retrain: not built for this board rev\r\n");
#endif
                continue;
            }
            if (esc_st == 0 && ch == 'u' && !menu_mode) {
                /* Raw FUSB302B status snapshot (in-slot attach debug). */
#if A2MEGA_HAS_USBC_PD
                usbc_fusb_dump_log();
#else
                tn_puts(fd, "fusb: not built for this board rev\r\n");
#endif
                continue;
            }
            if (esc_st == 0 && ch == 'j' && !menu_mode) {
                /* Toggle the USB-JTAG bridge routing. Release puts all
                 * ESP32 JTAG pads at high-Z so an EXTERNAL programmer can
                 * own the FPGA JTAG bus without contention (board #1 flash
                 * forensics, 2026-08-15). */
                extern void telnetd_jtag_bridge_toggle(int fd);
                telnetd_jtag_bridge_toggle(fd);
                continue;
            }
            if (esc_st == 0 && ch == 'e' && !menu_mode) {
                /* Cycle DP receiver EQ presets on the mux, live. */
#if A2MEGA_HAS_USBC_PD
                usbc_mux_eq_cycle();
#else
                tn_puts(fd, "eq: not built for this board rev\r\n");
#endif
                continue;
            }
            if (esc_st == 0 && ch == 'x' && !menu_mode) {
                /* TUSB1046A register dump — the AUX snooper is the only
                 * on-the-wire view of the FPGA's AUX transmissions. */
#if A2MEGA_HAS_USBC_PD
                usbc_mux_dump_log();
#else
                tn_puts(fd, "mux: not built for this board rev\r\n");
#endif
                continue;
            }
            if (esc_st == 0 && ch == 'm' && !menu_mode) {
                menu_mode = true;
                last_paint = 0;            /* force full repaint */
                tn_puts(fd, "\x1b[2J");
                /* Force the MENU view specifically: a bare SELECT lands in
                 * whichever MCU view was last active (possibly the console,
                 * where arrows are ignored). */
                menu_request_menu_view();
                continue;
            }
            if (menu_mode) {
                uint16_t b = key_to_buttons(ch, &esc_st);
                if (b)
                    menu_inject(b);
            }
        }

        if (menu_mode) {
            if (!menu_mcu_view_active()) {
                /* B at the root menu (or SELECT) handed the display back
                 * to the Apple II — mirror that instead of showing a
                 * stale frame. */
                menu_mode = false;
                tn_puts(fd, "\x1b[0m\x1b[2J\x1b[H"
                            "-- board returned to Apple II view; "
                            "m to re-enter menu --\r\n");
                xSemaphoreTake(s_tee_lock, portMAX_DELAY);
                s_tee_rd = s_tee_wr;
                xSemaphoreGive(s_tee_lock);
                continue;
            }
            uint32_t gen = fpga_screen_shadow_gen();
            if (gen != last_paint) {
                last_paint = gen;
                render_screen(fd);
            }
        } else {
            /* drain the console tee */
            for (;;) {
                char line[TEE_COLS + 1];
                xSemaphoreTake(s_tee_lock, portMAX_DELAY);
                bool have = s_tee_rd < s_tee_wr;
                if (have) {
                    if (s_tee_wr - s_tee_rd > TEE_LINES)
                        s_tee_rd = s_tee_wr - TEE_LINES;   /* dropped */
                    strcpy(line, s_tee[s_tee_rd % TEE_LINES]);
                    s_tee_rd++;
                }
                xSemaphoreGive(s_tee_lock);
                if (!have)
                    break;
                tn_puts(fd, line);
                tn_puts(fd, "\r\n");
            }
        }
    }
}

static void telnetd_task(void *arg)
{
    (void)arg;
    /* Bind only after lwIP core init has certainly finished (same guard as
     * the BL616 original and ftpd.c: netif_default appearing means the
     * tcpip thread is long past lwip_init). */
    while (netif_default == NULL)
        vTaskDelay(pdMS_TO_TICKS(200));

    int lfd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0)
        vTaskDelete(NULL);
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = PP_HTONS(TELNET_PORT);
    sa.sin_addr.s_addr = PP_HTONL(INADDR_ANY);
    int one = 1;
    lwip_setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if (lwip_bind(lfd, (struct sockaddr *)&sa, sizeof(sa)) < 0 ||
        lwip_listen(lfd, 1) < 0) {
        lwip_close(lfd);
        vTaskDelete(NULL);
    }
    osd_log("TELNET: LISTENING ON PORT 23");

    for (;;) {
        int fd = lwip_accept(lfd, NULL, NULL);
        if (fd < 0) {
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }
        struct timeval tv = { .tv_sec = 0, .tv_usec = 50 * 1000 };
        lwip_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        /* A peer that vanishes without closing (port scan, killed nc) stops
         * ACKing; once its window fills an untimed send blocks this thread
         * forever and the single-session server is wedged until reboot. */
        struct timeval stv = { .tv_sec = 3, .tv_usec = 0 };
        lwip_setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &stv, sizeof(stv));
        /* Keepalive reaps half-open sessions that go idle (console mode
         * with no log traffic never sends, so SO_SNDTIMEO alone can't). */
        int idle = 10, intvl = 5, cnt = 3;
        lwip_setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one));
        lwip_setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
        lwip_setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl));
        lwip_setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
        s_client_up = true;
        osd_log("TELNET: CLIENT CONNECTED");
        session(fd);
        s_client_up = false;
        lwip_close(fd);
        osd_log("TELNET: CLIENT DISCONNECTED");
    }
}

void telnetd_init(void)
{
    if (s_tee_lock)
        return;                            /* already running */
    s_tee_lock = xSemaphoreCreateMutex();
    /* Same core/priority band as ftpd: below the lwIP tcpip thread. */
    xTaskCreatePinnedToCore(telnetd_task, "telnetd", 4096, NULL, 4, NULL, 1);
}
