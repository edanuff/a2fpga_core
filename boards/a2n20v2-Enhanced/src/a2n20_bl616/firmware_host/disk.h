/*
 * disk.h — Apple II Disk II image serving (track-on-demand) for the host build.
 *
 * The FPGA Disk II controller (hdl/disk/drive_ii.sv) keeps only the track the
 * head is currently over resident in a small SDRAM window. On a seek it raises
 * a per-drive read request over the drive_volume_if block protocol (exposed by
 * bl616_spi_connector as SPI volume registers 0x40-0x5F). This module mounts the
 * SD card, opens .nib disk images, and services those requests: it reads the
 * requested track from the file and DMAs it into the FPGA SDRAM track window via
 * XFER SPACE 1, then acknowledges.
 *
 * Supports read (track load) and write (dirty-track flush) when the image opens
 * read-write; falls back to read-only for write-protected images.
 */

#ifndef _DISK_H
#define _DISK_H

/* Mount the SD card and open the disk images. Call once before disk_poll(). */
void disk_init(void);

/* Service any pending track requests for both drives. Call repeatedly. */
void disk_poll(void);

/* Request a re-mount on the next poll (e.g. after a USB stick is attached or
 * removed). Safe to call from the USB host thread. */
void disk_request_remount(void);

#endif
