/*
 * disk.c — Apple II Disk II image serving (track-on-demand) for the host build.
 *
 * See disk.h for the architecture. Mirrors the NanoApple2 floppy_track model:
 * the FPGA requests one track at a time (lba = track*13, 13 * 512 = 0x1A00
 * bytes), we read it from the .nib file on the SD card and stream it into the
 * FPGA SDRAM track window, then pulse ack.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ff.h"
#include "fpga_spi.h"
#include "fpga_sd.h"
#include "fpga_screen.h"
#include "disk.h"
#include "diskio_host.h"   /* SD/USB backend selection */
#include "usb_osal.h"      /* usb_osal_msleep */
#include "osd_console.h"   /* shared boot/status console */
#include "bflb_mtimer.h"   /* bflb_mtimer_get_time_us — load-latency timing */
#include "gcr_dsk.h"       /* on-the-fly .dsk/.do <-> 6-and-2 GCR nibble codec */

/* ---- Volume register map (must match bl616_spi_connector.sv 0x40-0x5F) ---- */
#define VOL_BASE(v)       (0x40u + (v) * 0x10u)
#define VOL_READY(v)      (VOL_BASE(v) + 0x0u)   /* W: volume ready */
#define VOL_MOUNTED(v)    (VOL_BASE(v) + 0x2u)   /* W: disk present */
#define VOL_READONLY(v)   (VOL_BASE(v) + 0x3u)   /* W: write-protected */
#define VOL_SIZE(v)       (VOL_BASE(v) + 0x4u)   /* W: 4 bytes LE, size in blocks */
#define VOL_LBA(v)        (VOL_BASE(v) + 0x8u)   /* R: 4 bytes LE, requested LBA */
#define VOL_BLKCNT(v)     (VOL_BASE(v) + 0xCu)   /* R: block count - 1 */
#define VOL_RD(v)         (VOL_BASE(v) + 0xDu)   /* R: read request pending */
#define VOL_WR(v)         (VOL_BASE(v) + 0xEu)   /* R: write request pending */
#define VOL_ACK(v)        (VOL_BASE(v) + 0xFu)   /* W: acknowledge (strobe) */

/* ---- SDRAM track windows (must match top.sv DISK_WORD_BASE + d*0x2000) ----
 * DISK_WORD_BASE = word 0x080000 = byte 0x200000; per-drive stride = 8KB. */
#define DISK_WINDOW_BASE    0x200000u
#define DISK_WINDOW_STRIDE  0x2000u
#define SECTOR_BYTES        512u
#define MAX_TRACK_BYTES     0x1A00u   /* 6656 = 13 sectors */

#define NDRV 2

/* Per-drive image format: .nib is streamed as-is (raw nibble track); .dsk/.do
 * is a DOS 3.3 sector image that gcr_dsk nibblizes on load / de-nibblizes on
 * flush. */
typedef enum { FMT_NONE = 0, FMT_NIB, FMT_DSK } disk_fmt_t;

/* Candidate images per drive, tried in order (first that opens wins). .dsk/.do
 * take priority over .nib. OSD/config selection can replace this later. */
static const char *const g_candidates[NDRV][3] = {
    { "0:/disk1.dsk", "0:/disk1.do", "0:/disk1.nib" },
    { "0:/disk2.dsk", "0:/disk2.do", "0:/disk2.nib" },
};

static FATFS      g_fs;
static FIL        g_img[NDRV];
static bool       g_mounted[NDRV];
static bool       g_writable[NDRV];
static disk_fmt_t g_fmt[NDRV];
static char       g_imgname[NDRV][32];          /* resolved image path         */
static uint8_t    g_trackbuf[MAX_TRACK_BYTES];  /* nibble track (SDRAM window) */
static uint8_t    g_secbuf[DSK_TRACK_BYTES];    /* one .dsk track (16*256)     */

/* Format from the filename extension (.nib -> FMT_NIB, .dsk/.do -> FMT_DSK). */
static disk_fmt_t detect_format(const char *name)
{
    size_t n = strlen(name);
    if (n >= 4) {
        const char *e = name + n - 4;
        if (e[0] == '.' &&
            (e[1] | 0x20) == 'n' && (e[2] | 0x20) == 'i' && (e[3] | 0x20) == 'b')
            return FMT_NIB;
        if (e[0] == '.' &&
            (e[1] | 0x20) == 'd' && (e[2] | 0x20) == 's' && (e[3] | 0x20) == 'k')
            return FMT_DSK;
    }
    if (n >= 3) {
        const char *e = name + n - 3;
        if (e[0] == '.' && (e[1] | 0x20) == 'd' && (e[2] | 0x20) == 'o')
            return FMT_DSK;
    }
    return FMT_NONE;
}

static uint32_t reg_read32(uint8_t reg)
{
    return ((uint32_t)fpga_spi_reg_read(reg)) |
           ((uint32_t)fpga_spi_reg_read(reg + 1) << 8) |
           ((uint32_t)fpga_spi_reg_read(reg + 2) << 16) |
           ((uint32_t)fpga_spi_reg_read(reg + 3) << 24);
}

static void reg_write32(uint8_t reg, uint32_t val)
{
    fpga_spi_reg_write(reg + 0, (uint8_t)(val));
    fpga_spi_reg_write(reg + 1, (uint8_t)(val >> 8));
    fpga_spi_reg_write(reg + 2, (uint8_t)(val >> 16));
    fpga_spi_reg_write(reg + 3, (uint8_t)(val >> 24));
}

static void mount_drive(int v)
{
    g_mounted[v]  = false;
    g_writable[v] = false;
    g_fmt[v]      = FMT_NONE;
    fpga_spi_reg_write(VOL_READY(v), 0);
    fpga_spi_reg_write(VOL_MOUNTED(v), 0);

    /* Default display name (primary candidate) for the not-found log. */
    strncpy(g_imgname[v], g_candidates[v][0], sizeof(g_imgname[v]) - 1);
    g_imgname[v][sizeof(g_imgname[v]) - 1] = '\0';

    /* Try each candidate in order; first that opens wins. Prefer read-write so
     * dirty tracks can flush; fall back to read-only. */
    int opened = 0;
    for (int c = 0; c < 3 && !opened; c++) {
        const char *name = g_candidates[v][c];
        if (f_open(&g_img[v], name, FA_READ | FA_WRITE) == FR_OK) {
            g_writable[v] = true;  opened = 1;
        } else if (f_open(&g_img[v], name, FA_READ) == FR_OK) {
            g_writable[v] = false; opened = 1;
        }
        if (opened) {
            strncpy(g_imgname[v], name, sizeof(g_imgname[v]) - 1);
            g_imgname[v][sizeof(g_imgname[v]) - 1] = '\0';
            g_fmt[v] = detect_format(name);
        }
    }
    if (!opened)
        return;

    uint32_t bytes  = (uint32_t)f_size(&g_img[v]);
    uint32_t blocks = bytes / SECTOR_BYTES;
    /* .dsk/.do: 35 trk * 16 * 256 = 143360 B; .nib: 35 * 6656 = 232960 B.
     * VOL_SIZE is informational for a floppy (drive_ii.sv doesn't gate on it). */
    osd_log("DISK II: D%d %s = %lu B (%s)", v + 1, g_imgname[v] + 3,
            (unsigned long)bytes, g_fmt[v] == FMT_DSK ? "dsk" : "nib");
    reg_write32(VOL_SIZE(v), blocks);
    fpga_spi_reg_write(VOL_READONLY(v), g_writable[v] ? 0 : 1);
    fpga_spi_reg_write(VOL_MOUNTED(v), 1);
    fpga_spi_reg_write(VOL_READY(v), 1);
    g_mounted[v] = true;
}

/* Re-evaluate the storage backend and (re)mount the images. Triggered at
 * startup and whenever a USB stick is attached/removed. Prefers the USB stick
 * if present, otherwise the SD card. */
static volatile bool g_remount_req = true;

/* Non-blocking console hide: disk_remount() schedules the "READY" screen to be
 * handed back to the Apple II after a readable delay, and disk_poll() performs
 * the hide when the time arrives. Doing this instead of msleep() keeps the 2 ms
 * disk task serving track loads throughout the delay — a blocking sleep here
 * freezes serving for the whole delay and hangs a boot that seeks into it.
 * 0 = nothing scheduled. */
static uint32_t g_hide_console_at_us = 0;

void disk_request_remount(void)
{
    g_remount_req = true;
}

/* ---- Disk II (re)mount, logged to the shared OSD console -------------------
 * Mount status is appended to the boot console (osd_console). The console is
 * shown while we look for the image(s); on a successful mount we hide it so the
 * screen returns to the Apple II, and on failure we leave it up so the message
 * can be read. */
static void disk_remount(void)
{
    osd_console_show();
    osd_log("DISK II: SEARCHING FOR STORAGE...");

    /* Tear down the previous mount. */
    for (int v = 0; v < NDRV; v++) {
        if (g_mounted[v])
            f_close(&g_img[v]);
        g_mounted[v]  = false;
        g_writable[v] = false;
        fpga_spi_reg_write(VOL_READY(v), 0);
        fpga_spi_reg_write(VOL_MOUNTED(v), 0);
    }
    f_mount(NULL, "0:", 0);

    bool use_usb = (g_msc_class != NULL);
    disk_io_set_backend_usb(use_usb);
    osd_log(use_usb ? "DISK II: USB MASS STORAGE"
                    : "DISK II: NO USB - TRYING SD CARD");

    FRESULT fr = f_mount(&g_fs, "0:", 1);
    if (fr != FR_OK) {
        if (use_usb) {
            /* FR=13 FR_NO_FILESYSTEM (not FAT / GPT scheme), FR=1 FR_DISK_ERR
             * (USB read path / block size), FR=3 FR_NOT_READY, etc. */
            osd_log("DISK II: USB MOUNT FAILED (FR=%d)", (int)fr);
            osd_log("DISK II: USB %lu BLK x %u B",
                    (unsigned long)disk_usb_block_count(),
                    (unsigned)disk_usb_block_size());
        } else {
            osd_log("DISK II: INSERT USB DRIVE WITH DISK1.DSK/.NIB");
        }
        return;   /* leave the console up so the message is visible */
    }

    int n_mounted = 0;
    for (int v = 0; v < NDRV; v++) {
        mount_drive(v);
        if (g_mounted[v]) {
            osd_log("DISK II: DRIVE %d %s MOUNTED (%s)", v + 1,
                    g_imgname[v] + 3, g_writable[v] ? "RW" : "RO");
            n_mounted++;
        } else {
            osd_log("DISK II: DRIVE %d %s NOT FOUND", v + 1, g_imgname[v] + 3);
        }
    }

    if (n_mounted > 0) {
        osd_log("DISK II: READY - STARTING APPLE II");
        /* Hand the screen back after a readable delay, WITHOUT blocking: a
         * msleep here would freeze the 2 ms disk task and hang a boot that
         * seeks during the delay (observed as a ~1.54 s poll gap). */
        g_hide_console_at_us = (uint32_t)bflb_mtimer_get_time_us() + 1500000u;
    } else {
        osd_log("DISK II: NO DISK IMAGES ON VOLUME");
        /* leave the console up so the message is visible */
    }
}

void disk_init(void)
{
    fpga_sd_init();   /* SD tunnel defaults; mount happens on the first poll */
}

static void serve_drive(int v)
{
    if (!g_mounted[v])
        return;

    uint8_t rd = fpga_spi_reg_read(VOL_RD(v)) & 0x01;
    uint8_t wr = fpga_spi_reg_read(VOL_WR(v)) & 0x01;
    if (!rd && !wr)
        return;   /* nothing pending */

    uint32_t lba   = reg_read32(VOL_LBA(v));
    uint32_t nblk  = (uint32_t)fpga_spi_reg_read(VOL_BLKCNT(v)) + 1u;
    uint32_t nbyte = nblk * SECTOR_BYTES;
    if (nbyte > MAX_TRACK_BYTES)
        nbyte = MAX_TRACK_BYTES;

    uint32_t addr = DISK_WINDOW_BASE + (uint32_t)v * DISK_WINDOW_STRIDE;

    /* Log disk activity to the console BUFFER on track change (a boot re-polling
     * the same track must not spam). Does NOT force the console visible — the
     * Apple II keeps the screen so the booted disk is actually usable; press
     * Select to review the track log. */
    {
        static int s_last_trk[NDRV] = { -1, -1 };
        int trk = (int)(lba / 13u);
        if (trk != s_last_trk[v]) {
            s_last_trk[v] = trk;
            osd_log("DISK II: D%d %s TRK %d (LBA %lu N%lu)", v + 1,
                    wr ? "WR" : "RD", trk, (unsigned long)lba, (unsigned long)nblk);
        }
    }

    if (wr) {
        /* Flush a dirty track: SDRAM window -> image file. */
        if (g_writable[v]) {
            for (uint32_t off = 0; off < nbyte; off += SECTOR_BYTES) {
                uint32_t chunk = nbyte - off;
                if (chunk > SECTOR_BYTES)
                    chunk = SECTOR_BYTES;
                fpga_spi_xfer_read(FPGA_SPACE_SDRAM, addr + off,
                                   g_trackbuf + off, (uint16_t)chunk);
            }
            if (g_fmt[v] == FMT_DSK) {
                /* Decode the (possibly partly rewritten) nibble track back to
                 * DOS-order sectors. Preload the current on-file track so any
                 * sector that fails to decode keeps its existing bytes; gate the
                 * write on the found-mask so a bad decode never corrupts the
                 * image. */
                uint32_t track = lba / 13u;
                UINT br = 0;
                if (f_lseek(&g_img[v], (FSIZE_t)track * DSK_TRACK_BYTES) == FR_OK)
                    f_read(&g_img[v], g_secbuf, DSK_TRACK_BYTES, &br);
                if (br < DSK_TRACK_BYTES)
                    memset(g_secbuf + br, 0, DSK_TRACK_BYTES - br);
                uint16_t mask = gcr_decode_dos_track(g_trackbuf, MAX_TRACK_BYTES,
                                                     g_secbuf);
                if (mask != 0) {
                    UINT bw = 0;
                    if (f_lseek(&g_img[v], (FSIZE_t)track * DSK_TRACK_BYTES) == FR_OK) {
                        f_write(&g_img[v], g_secbuf, DSK_TRACK_BYTES, &bw);
                        f_sync(&g_img[v]);
                    }
                }
                if (mask != 0xFFFF)
                    osd_log("DISK II: D%d TRK%lu wr partial mask=%04X",
                            v + 1, (unsigned long)track, (unsigned)mask);
            } else {
                UINT bw = 0;
                if (f_lseek(&g_img[v], (FSIZE_t)lba * SECTOR_BYTES) == FR_OK) {
                    f_write(&g_img[v], g_trackbuf, nbyte, &bw);
                    f_sync(&g_img[v]);
                }
            }
        }
    } else {
        /* Load the requested track: image file -> SDRAM window. */
        uint32_t track = lba / 13u;
        if (g_fmt[v] == FMT_DSK) {
            /* Read this track's 16*256 DOS-order sectors and nibblize them into
             * the 6-and-2 GCR stream the window expects. */
            UINT br = 0;
            if (f_lseek(&g_img[v], (FSIZE_t)track * DSK_TRACK_BYTES) == FR_OK)
                f_read(&g_img[v], g_secbuf, DSK_TRACK_BYTES, &br);
            if (br < DSK_TRACK_BYTES)
                memset(g_secbuf + br, 0, DSK_TRACK_BYTES - br);
            gcr_encode_dos_track(g_secbuf, (uint8_t)track, DSK_DEFAULT_VOLUME,
                                 g_trackbuf, MAX_TRACK_BYTES);
        } else {
            /* .nib: raw nibble stream, streamed as-is. */
            UINT br = 0;
            if (f_lseek(&g_img[v], (FSIZE_t)lba * SECTOR_BYTES) == FR_OK)
                f_read(&g_img[v], g_trackbuf, nbyte, &br);
            if (br < nbyte) {
                /* EOF: a zero-filled track has no sync/prologue nibbles, so RWTS
                 * finds nothing and DOS I/O-errors at this same track every boot
                 * — pinpoints a truncated/short .nib. */
                osd_console_show();
                osd_log("DISK II: TRK%lu SHORT br=%lu/%lu (EOF) -> zero-fill",
                        (unsigned long)track,
                        (unsigned long)br, (unsigned long)nbyte);
                memset(g_trackbuf + br, 0, nbyte - br);
            }
        }

        for (uint32_t off = 0; off < nbyte; off += SECTOR_BYTES) {
            uint32_t chunk = nbyte - off;
            if (chunk > SECTOR_BYTES)
                chunk = SECTOR_BYTES;
            fpga_spi_xfer_write(FPGA_SPACE_SDRAM, addr + off,
                                g_trackbuf + off, (uint16_t)chunk);
        }
    }

    fpga_spi_reg_write(VOL_ACK(v), 1);   /* request serviced — release the head */
}

void disk_poll(void)
{
    if (g_remount_req) {
        g_remount_req = false;
        disk_remount();
    }

    /* Non-blocking console hide (scheduled by disk_remount on a good mount). */
    if (g_hide_console_at_us &&
        (int32_t)((uint32_t)bflb_mtimer_get_time_us() - g_hide_console_at_us) >= 0) {
        g_hide_console_at_us = 0;
        osd_console_hide();
    }

    for (int v = 0; v < NDRV; v++)
        serve_drive(v);
}
