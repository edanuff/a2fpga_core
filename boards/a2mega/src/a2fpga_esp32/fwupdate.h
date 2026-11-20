/*
 * fwupdate — MCU firmware self-update from a file on the image volume
 * (USB stick or the flash FS at /sdcard), the BL616 fwupdate flow on
 * ESP-IDF OTA: the new image is written into the inactive OTA slot
 * (esp_ota_begin/write/end), made the boot partition and the ESP32
 * restarts into it. Needs an OTA partition scheme (Makefile:
 * PartitionScheme=default — two 1.25 MB app slots on the 4 MB part).
 *
 * Same state machine as fpgaupdate: request(path) -> CHECKING (header +
 * size sanity) -> READY (message shows file/size/target) -> commit() ->
 * INSTALLING (progress in the message) -> DONE (restart in 2 s) / ERROR.
 * fwupdate_poll() runs from the disk task (the image file lives on the
 * volume that task owns).
 */
#ifndef FWUPDATE_H
#define FWUPDATE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FWU_IDLE = 0,
    FWU_CHECKING,
    FWU_READY,
    FWU_INSTALL_REQ,
    FWU_INSTALLING,
    FWU_DONE,
    FWU_ERROR,
} fwu_state_t;

/* path relative to the image volume root ("a2fpga_esp32.ino.bin" or
 * "A2FPGA/fw.bin"); false if a check/install is already running */
bool        fwupdate_request(const char *path);
void        fwupdate_commit(void);
void        fwupdate_cancel(void);
fwu_state_t fwupdate_state(void);
const char *fwupdate_message(void);        /* one-line status / error */
bool        fwupdate_dirty(void);          /* message changed since last call */
void        fwupdate_poll(void);           /* disk task */
/* running slot and boot slot labels for the status screen ("ota_0") */
const char *fwupdate_running_slot(void);

#ifdef __cplusplus
}
#endif
#endif
