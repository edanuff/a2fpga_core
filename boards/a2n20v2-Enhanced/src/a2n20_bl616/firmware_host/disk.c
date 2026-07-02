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

/* Default image filenames on the SD card root. (OSD/config selection later.) */
static const char *const g_imgname[NDRV] = { "0:/disk1.nib", "0:/disk2.nib" };

static FATFS   g_fs;
static FIL     g_img[NDRV];
static bool    g_mounted[NDRV];
static bool    g_writable[NDRV];
static uint8_t g_trackbuf[MAX_TRACK_BYTES];

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
    fpga_spi_reg_write(VOL_READY(v), 0);
    fpga_spi_reg_write(VOL_MOUNTED(v), 0);

    /* Prefer read-write so the FPGA can flush dirty tracks; fall back to
     * read-only (e.g. a write-protected card or locked file). */
    if (f_open(&g_img[v], g_imgname[v], FA_READ | FA_WRITE) == FR_OK)
        g_writable[v] = true;
    else if (f_open(&g_img[v], g_imgname[v], FA_READ) != FR_OK)
        return;

    uint32_t bytes  = (uint32_t)f_size(&g_img[v]);
    uint32_t blocks = bytes / SECTOR_BYTES;
    /* A .nib holds 6656 bytes/track; a full DOS 3.3 disk is 35 tracks =
     * 232960 bytes. Log the actual size so a truncated/short image (which
     * would zero-fill and I/O-error at the first missing track) is obvious. */
    osd_log("DISK II: D%d %s = %lu B (%lu trk)", v + 1, g_imgname[v] + 3,
            (unsigned long)bytes, (unsigned long)(bytes / MAX_TRACK_BYTES));
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
            osd_log("DISK II: INSERT USB DRIVE WITH DISK1.NIB");
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
        osd_log("DISK II: NO .NIB IMAGES ON VOLUME");
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
            UINT bw = 0;
            if (f_lseek(&g_img[v], (FSIZE_t)lba * SECTOR_BYTES) == FR_OK) {
                f_write(&g_img[v], g_trackbuf, nbyte, &bw);
                f_sync(&g_img[v]);
            }
        }
    } else {
        /* Load the requested track: image file -> SDRAM window. */
        UINT br = 0;
        if (f_lseek(&g_img[v], (FSIZE_t)lba * SECTOR_BYTES) == FR_OK)
            f_read(&g_img[v], g_trackbuf, nbyte, &br);
        if (br < nbyte) {
            /* The image has no (full) data for this track: the read hit EOF. A
             * zero-filled track has no sync/prologue nibbles, so RWTS finds
             * nothing and DOS reports "I/O ERROR" — deterministically, at this
             * same track, every boot. This log pinpoints a truncated/short .nib
             * (e.g. fewer than 35 tracks) as the cause. */
            osd_console_show();
            osd_log("DISK II: TRK%lu SHORT br=%lu/%lu (EOF) -> zero-fill",
                    (unsigned long)(lba / 13u),
                    (unsigned long)br, (unsigned long)nbyte);
            memset(g_trackbuf + br, 0, nbyte - br);   /* zero-fill short reads */
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
