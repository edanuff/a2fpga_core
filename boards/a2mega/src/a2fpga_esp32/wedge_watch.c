/*
 * wedge_watch.c — see wedge_watch.h for the policy contract.
 */
#include <stdint.h>
#include <string.h>

#include "nvs.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "wedge_watch.h"
#include "usbc_glue.h"          /* usbc_virtual_replug() */

void osd_log(const char *fmt, ...);

#define WW_NVS_NAMESPACE   "a2fpga"
#define WW_NVS_KEY_BUDGET  "ww_budget"   /* u8: replug attempts used */
#define WW_NVS_KEY_ENABLE  "ww_enable"   /* u8: 0 = disabled (default on) */

#define WEDGE_MAX_ATTEMPTS   3   /* total until streaming re-arms */
#define WEDGE_CONSEC_LINES   2   /* consecutive flagged D4 lines to fire
                                  * (latency trim; FPGA already sustains 8 s) */
#define WEDGE_MIN_UPTIME_MS  30000u  /* never fire in the first 30 s */
#define WEDGE_COOLDOWN_MS    60000u  /* between attempts (slot-powered case) */
#define REARM_STREAM_LINES   20  /* consecutive K:!=0 D3 lines to re-arm */

static const char *TAG = "wedge_watch";

static bool     s_enabled = true;
static uint8_t  s_budget;            /* attempts used (persisted) */
static uint8_t  s_consec_wedge;
static uint8_t  s_consec_stream;
static uint32_t s_last_fire_ms;
static bool     s_exhausted_logged;

static uint32_t uptime_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

static void nvs_write_u8(const char *key, uint8_t v)
{
    nvs_handle_t h;
    if (nvs_open(WW_NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK)
        return;
    if (nvs_set_u8(h, key, v) == ESP_OK)
        nvs_commit(h);
    nvs_close(h);
}

static uint8_t nvs_read_u8(const char *key, uint8_t dflt)
{
    nvs_handle_t h;
    uint8_t v = dflt;
    if (nvs_open(WW_NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK)
        return dflt;
    if (nvs_get_u8(h, key, &v) != ESP_OK)
        v = dflt;
    nvs_close(h);
    return v;
}

void wedge_watch_init(void)
{
    s_budget  = nvs_read_u8(WW_NVS_KEY_BUDGET, 0);
    s_enabled = nvs_read_u8(WW_NVS_KEY_ENABLE, 1) != 0;
    ESP_LOGI(TAG, "auto-recovery %s, budget used %u/%u",
             s_enabled ? "ENABLED" : "disabled",
             (unsigned)s_budget, WEDGE_MAX_ATTEMPTS);
}

/* Parse two hex digits after `tag` in `line`; -1 if absent/garbled. */
static int hex_field(const char *line, const char *tag)
{
    const char *p = strstr(line, tag);
    int v = 0;
    if (p == NULL)
        return -1;
    p += strlen(tag);
    for (int i = 0; i < 2; i++, p++) {
        v <<= 4;
        if (*p >= '0' && *p <= '9')      v |= *p - '0';
        else if (*p >= 'A' && *p <= 'F') v |= *p - 'A' + 10;
        else if (*p >= 'a' && *p <= 'f') v |= *p - 'a' + 10;
        else return -1;
    }
    return v;
}

static void fire_replug(void)
{
    /* Persist the increment BEFORE acting: on a hub-backfed bench the
     * replug power-cycles this chip — an unpersisted count would make a
     * recurring wedge an infinite replug loop. */
    s_budget++;
    nvs_write_u8(WW_NVS_KEY_BUDGET, s_budget);
    s_last_fire_ms = uptime_ms();
    osd_log("AUTOREC: wedge sustained - virtual replug (attempt %u/%u)",
            (unsigned)s_budget, WEDGE_MAX_ATTEMPTS);
    usbc_virtual_replug_fast();
}

void wedge_watch_line(const char *line)
{
    if (line == NULL)
        return;

    if (strncmp(line, "D3 ", 3) == 0) {
        int k = hex_field(line, "K:");
        if (k > 0) {
            /* real streaming observed: count toward re-arm */
            if (s_consec_stream < 255)
                s_consec_stream++;
            if (s_consec_stream >= REARM_STREAM_LINES && s_budget != 0) {
                s_budget = 0;
                s_exhausted_logged = false;
                nvs_write_u8(WW_NVS_KEY_BUDGET, 0);
                osd_log("AUTOREC: streaming stable - budget re-armed");
            }
        } else if (k == 0) {
            s_consec_stream = 0;
        }
        return;
    }

    if (strncmp(line, "D4 ", 3) != 0)
        return;
    {
        int u = hex_field(line, "U:");
        if (u < 0)
            return;                       /* garbled line: ignore */
        if ((u & 0x80) == 0) {
            s_consec_wedge = 0;
            return;
        }
        if (s_consec_wedge < 255)
            s_consec_wedge++;
        if (!s_enabled || s_consec_wedge < WEDGE_CONSEC_LINES)
            return;
        if (uptime_ms() < WEDGE_MIN_UPTIME_MS)
            return;
        if (s_last_fire_ms != 0 &&
            (uptime_ms() - s_last_fire_ms) < WEDGE_COOLDOWN_MS)
            return;
        if (s_budget >= WEDGE_MAX_ATTEMPTS) {
            if (!s_exhausted_logged) {
                s_exhausted_logged = true;
                osd_log("AUTOREC: budget exhausted (%u) - manual recovery "
                        "required (replug HDMI or telnet 'v')",
                        WEDGE_MAX_ATTEMPTS);
            }
            return;
        }
        s_consec_wedge = 0;
        fire_replug();
    }
}

void wedge_watch_toggle_cmd(void)
{
    s_enabled = !s_enabled;
    nvs_write_u8(WW_NVS_KEY_ENABLE, s_enabled ? 1 : 0);
    osd_log("AUTOREC: %s (budget %u/%u used%s)",
            s_enabled ? "ENABLED" : "DISABLED",
            (unsigned)s_budget, WEDGE_MAX_ATTEMPTS,
            s_budget >= WEDGE_MAX_ATTEMPTS ? ", EXHAUSTED" : "");
}
