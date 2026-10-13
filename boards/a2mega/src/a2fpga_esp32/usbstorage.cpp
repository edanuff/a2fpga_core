/*
 * usbstorage.cpp — FatFS diskio glue for the USB stick (see usbstorage.h).
 * ESP-IDF FatFS: ff_diskio_register() with our sector functions, then
 * esp_vfs_fat_register_cfg() + f_mount() to put the volume at /sdcard.
 * Equivalent of the BL616's diskio_host.c USB branch.
 */
#include <Arduino.h>
#include <string.h>

#include "board_pins.h"
#include "usbstorage.h"
#include "usbhost.h"

#if A2MEGA_HAS_OSPI_LINK

#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "ff.h"

static const char *TAG = "usbstorage";

static volatile bool s_pending     = false;
static volatile bool s_pending_usb = false;
static volatile bool s_is_usb      = false;

static BYTE   s_pdrv = 0xFF;
static FATFS *s_fs   = NULL;
static char   s_drive[4] = "";
static char   s_base[16] = "";

void usbstorage_on_msc(bool present)
{
    s_pending_usb = present;
    s_pending     = true;
}
bool usbstorage_pending(void) { return s_pending; }
bool usbstorage_take_pending(bool *usb_out)
{
    if (!s_pending) return false;
    s_pending = false;
    if (usb_out) *usb_out = s_pending_usb;
    return true;
}
bool usbstorage_is_usb(void) { return s_is_usb; }

/* ---- FatFS diskio ------------------------------------------------------- */
static DSTATUS ff_init(BYTE pdrv)   { (void)pdrv; return usbhost_msc_present() ? 0 : STA_NOINIT; }
static DSTATUS ff_status(BYTE pdrv) { (void)pdrv; return usbhost_msc_present() ? 0 : STA_NOINIT; }
static DRESULT ff_read(BYTE pdrv, BYTE *buff, uint32_t sector, unsigned count)
{
    (void)pdrv;
    if (!usbhost_msc_present()) return RES_NOTRDY;
    return usbhost_msc_read(sector, count, buff) ? RES_OK : RES_ERROR;
}
static DRESULT ff_write(BYTE pdrv, const BYTE *buff, uint32_t sector, unsigned count)
{
    (void)pdrv;
    if (!usbhost_msc_present()) return RES_NOTRDY;
    return usbhost_msc_write(sector, count, buff) ? RES_OK : RES_ERROR;
}
static DRESULT ff_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    (void)pdrv;
    uint32_t blocks = 0, bsize = 512;
    switch (cmd) {
    case CTRL_SYNC: return RES_OK;
    case GET_SECTOR_COUNT:
        if (!usbhost_msc_capacity(&blocks, &bsize)) return RES_NOTRDY;
        *(DWORD *)buff = blocks; return RES_OK;
    case GET_SECTOR_SIZE:
        *(WORD *)buff = 512; return RES_OK;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1; return RES_OK;
    default: return RES_PARERR;
    }
}

bool usbstorage_mount(const char *base_path)
{
    if (s_fs) return true;
    if (ff_diskio_get_drive(&s_pdrv) != ESP_OK) { ESP_LOGE(TAG, "no free FatFS drive"); return false; }
    static const ff_diskio_impl_t impl = { ff_init, ff_status, ff_read, ff_write, ff_ioctl };
    ff_diskio_register(s_pdrv, &impl);
    snprintf(s_drive, sizeof(s_drive), "%u:", (unsigned)s_pdrv);
    strlcpy(s_base, base_path, sizeof(s_base));
    esp_vfs_fat_conf_t conf = {};
    conf.base_path = s_base;
    conf.fat_drive = s_drive;
    conf.max_files = 8;
    esp_err_t e = esp_vfs_fat_register_cfg(&conf, &s_fs);
    if (e != ESP_OK) { ESP_LOGE(TAG, "vfs register: %d", (int)e); ff_diskio_unregister(s_pdrv); s_pdrv = 0xFF; return false; }
    FRESULT r = f_mount(s_fs, s_drive, 1);
    if (r != FR_OK) {
        ESP_LOGE(TAG, "f_mount: %d", (int)r);
        esp_vfs_fat_unregister_path(s_base); s_fs = NULL;
        ff_diskio_unregister(s_pdrv); s_pdrv = 0xFF;
        return false;
    }
    s_is_usb = true;
    ESP_LOGI(TAG, "USB stick mounted at %s (drive %s)", s_base, s_drive);
    return true;
}

void usbstorage_unmount(void)
{
    if (!s_fs) { s_is_usb = false; return; }
    f_mount(NULL, s_drive, 0);
    esp_vfs_fat_unregister_path(s_base);
    s_fs = NULL;
    ff_diskio_unregister(s_pdrv);
    s_pdrv = 0xFF;
    s_is_usb = false;
    ESP_LOGI(TAG, "USB stick unmounted from %s", s_base);
}

#else

void usbstorage_on_msc(bool present) { (void)present; }
bool usbstorage_pending(void) { return false; }
bool usbstorage_take_pending(bool *usb_out) { (void)usb_out; return false; }
bool usbstorage_is_usb(void) { return false; }
bool usbstorage_mount(const char *base_path) { (void)base_path; return false; }
void usbstorage_unmount(void) {}

#endif
