/*
 * usbstorage — a USB mass-storage stick as the disk-image volume.
 *
 * The BL616 model: ONE image root. When a stick is present it is the
 * volume (FatFS over usbhost's SCSI helpers, mounted on the same VFS prefix
 * disk.c / ftpd.c / fpgaupdate.c use, "/sdcard"); otherwise the flash
 * LittleFS is. The switch happens inside the disk task (it owns the open
 * image files): usbhost reports attach/detach here, disk_poll() sees the
 * pending change, unmounts its images, calls storage_activate() (the
 * integrator's swap of what sits at /sdcard) and remounts.
 */
#ifndef USBSTORAGE_H
#define USBSTORAGE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* from usbhost: a mass-storage device appeared / disappeared */
void usbstorage_on_msc(bool present);

/* disk task side: a backend change is pending; take it (returns the new
 * backend: true = USB stick) */
bool usbstorage_pending(void);
bool usbstorage_take_pending(bool *usb_out);
/* current backend */
bool usbstorage_is_usb(void);

/* FatFS/VFS mount of the stick on `base_path` (e.g. "/sdcard"); unmount */
bool usbstorage_mount(const char *base_path);
void usbstorage_unmount(void);

/* integrator (the .ino): swap what is mounted at /sdcard. Called by disk.c
 * from the disk task after it has closed its images. */
void storage_activate(bool usb);

#ifdef __cplusplus
}
#endif
#endif
