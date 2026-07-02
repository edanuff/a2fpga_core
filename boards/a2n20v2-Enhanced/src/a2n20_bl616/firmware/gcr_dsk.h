/*
 * gcr_dsk.h — on-the-fly Apple II 6-and-2 GCR codec for DOS 3.3 order .dsk/.do
 * disk images.
 *
 * A .dsk/.do image stores 16 x 256-byte sectors per track in DOS logical order
 * (DSK_TRACK_BYTES = 4096 bytes/track). The Disk II hardware, however, reads a
 * nibblized track: a 6-and-2 GCR stream of address/data fields and self-sync
 * gaps (GCR_TRACK_BYTES = 6656, the same size the .nib path already streams to
 * the SDRAM window). These routines convert between the two on demand:
 *   - gcr_encode_dos_track(): sectors -> nibble stream (on track load)
 *   - gcr_decode_dos_track(): nibble stream -> sectors (on dirty-track flush)
 *
 * The 6-and-2 encode/decode, the write-translate table, the DOS sector-order
 * interleave, and the track gap sizes are ported from AppleWin's
 * CImageBase::Code62 / Decode62 / NibblizeTrack (source/DiskImageHelper.cpp),
 * which is a widely-proven, RWTS-write-compatible layout.
 *
 * Pure C, no BL616 / FPGA / FatFS dependencies — unit-testable off-target.
 */
#ifndef _GCR_DSK_H
#define _GCR_DSK_H

#include <stddef.h>
#include <stdint.h>

#define DSK_SECTORS        16u
#define DSK_SECTOR_BYTES   256u
#define DSK_TRACK_BYTES    4096u   /* 16 * 256, one .dsk/.do track            */
#define GCR_TRACK_BYTES    6656u   /* resident SDRAM window / .nib track size */
#define DSK_DEFAULT_VOLUME 254u    /* .dsk stores no volume number; DOS default */

/*
 * Encode one DOS-order track (16*256 = 4096 bytes) into a 6-and-2 GCR nibble
 * stream of exactly GCR_TRACK_BYTES, padded with self-sync $FF. The layout is
 * RWTS-write-compatible so DOS can rewrite a single sector's data field in
 * place. Returns GCR_TRACK_BYTES on success, or 0 if out_cap is too small.
 */
size_t gcr_encode_dos_track(const uint8_t dsk_track[DSK_TRACK_BYTES],
                            uint8_t track, uint8_t volume,
                            uint8_t *out, size_t out_cap);

/*
 * Decode a nibble window (len bytes, typically GCR_TRACK_BYTES; may have been
 * partially rewritten in place by DOS RWTS) back into a DOS-order track.
 *
 * Scans the whole window (treated as circular, so a field wrapping the end is
 * still found) for address (D5 AA 96) and data (D5 AA AD) fields, verifies the
 * address and data checksums, and stores each recovered sector at its DOS-order
 * file offset in dsk_track. Returns a 16-bit mask: bit s set = sector s decoded
 * with a valid checksum. Sectors whose bit is CLEAR are LEFT UNTOUCHED, so the
 * caller should preload dsk_track with the current on-file track and a bad/
 * missing sector then keeps its existing bytes (never write garbage back).
 */
uint16_t gcr_decode_dos_track(const uint8_t *nibbles, size_t len,
                              uint8_t dsk_track[DSK_TRACK_BYTES]);

#endif /* _GCR_DSK_H */
