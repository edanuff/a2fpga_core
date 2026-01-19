# A2P25 TODO

## Status

**Work In Progress** - Tang Primer 25K version, coming soon.

## High Priority

- [x] Eliminate LCAM packet loss during IIgs burst transfers (confirmed 32,768/32,768, 0 corrupted, 0 drops)
- [ ] Complete the ES5503 code which runs on the ESP32-S3

## Medium Priority

- [ ] Complete the FAT32 SD Card support
- [ ] Enable configuration through a web-based UX rather than OSD

## Low Priority / Future

- [ ] Document ESP32-S3 to FPGA communication protocol
- [ ] Create web-based configuration UI

## Architecture Notes

- Uses ESP32-S3 for control/configuration
- LCAM interface between ESP32-S3 and FPGA
- ES5503 (Ensoniq DOC) emulation runs on ESP32-S3

## Known Issues

- VSYNC cadence metric: In VSYNC‑EOF mode, "Packets per EOF" may read ~250–350 vs the ideal ~409 because LCD_CAM length‑EOF can still trigger alongside VSYNC. Data integrity is unaffected (counts match, 0% corruption, 0 drops). Firmware sets
  `cam_rec_data_bytelen=4095` in VSYNC‑EOF to minimize interference. Consider adding a de‑noised cadence estimate/warning or use length‑EOF preset for canonical tests.

## Build Status

- Last verified build: Unknown
- Board design in progress


## Current Work

Testing using the following assembly code running on the IIgs and monitoring via the Arduino console:
```
        CLC             ; enter native mode (if coming from emulation)
        XCE

        SEP   #$20      ; A = 8-bit
        REP   #$10      ; X/Y = 16-bit

        LDA   #$00
        PHA
        PLB             ; DBR = $00 so absolute accesses use bank 00

        LDX   #$8000    ; 32768 iterations
@loop:  STZ   $C03D     ; store zero to $C03D
        DEX
        BNE   @loop

        RTS
```

which assembles to the following bytes:
```
0300: 18 FB E2 20 C2 10 A9 00 48 AB A2 00 80 9C 3D C0
0310: CA D0 FA 60
```

This stores bytes at the theoretical maximum speed that the IIgs 65816 is capable of writing to
the 65816 and allows us to see error rates with the current ESP32-S3 LCAM code.

In order for the A2P25 to be viable for IIgs use, we must be able to handle memory transfers
with zero packet loss since the A2P25 requires ES5503 sound generation to occur on the ESP32-S3.

## Current Status (2026‑01‑18)

- IIgs burst capture (32,768 writes to $C03D): PASS — Words received 32768, Corruption 0.0%, Ring drops 0, Address range $C03D–$C03D.
- Default mode: VSYNC‑EOF with gated VSYNC every 409 packets in HDL; ESP32 sets `cam_rec_data_bytelen=4095` in VSYNC‑EOF and `CHUNK_BYTES=4090` in length‑EOF.
- Diagnostics & CLI: Added `statsbrief`, throttled per‑buffer logs (`lcamlog`, `lcamlogevery`, `lcamlograte`), and one‑shot `lcamdebug N`. Parsing fixes applied.
- I2S concurrency: LCD_CAM capture sustained with GDMA_CH=2 and background recovery.

## Next Actions

- [ ] Optional: Add de‑noised VSYNC cadence metric and warn if deviates >±10% from target (409) in VSYNC‑EOF.
- [ ] Optional: Add a test preset to temporarily switch to length‑EOF for canonical runs (pretty EOF stats) and switch back to VSYNC‑EOF for normal operation.
- [ ] Document field checklist for verifying capture (expected stats, CLI snippets) in the firmware README (linked below).

Further Reading / Playbook
- Full session knowledge and procedures: `boards/a2p25/LCAM_SESSION_NOTES.md`
- Firmware checklist and expected stats: `boards/a2p25/src/a2fpga_esp32/README.md`
## LCAM Investigation Summary

- Apple IIgs test sends 32,768 ES5503 packets at ~13.5 MB/s (PCLK 13.5 MHz).
- Root cause of loss was GDMA descriptor churn from per-packet VSYNC→EOF (EOF every ~10 bytes), not just FIFO depth.
- Fixes implemented so far:
  - SPI protocol on FPGA: bit order, sync sequencing, first-byte read behavior, and single-driver cleanup; testbench passes.
  - ESP32: length-EOF mode with large chunks and stitching; VSYNC‑EOF mode with gated cadence; GDMA channel moved to 2; background relink on GDMA reset; dummy handling for XFER; improved stats/CLI.
- Results to date:
  - Corruption: 0.0% in both modes once aligned.
  - Capture count: near-complete; exact 32,768 requires proper end-of-burst handling (see plan).

Further Reading / Playbook
- Full session knowledge and procedures: `boards/a2p25/LCAM_SESSION_NOTES.md`
- Firmware checklist and expected stats: `boards/a2p25/src/a2fpga_esp32/README.md`

## LCAM Resolution Plan (Actionable)

1) FPGA (Serializer/Sync)
- Gate VSYNC once every 409 packets and assert one VSYNC at end-of-burst.
- Ensure no stray VSYNC inside the burst; keep PCLK at 13.5 MHz.

2) ESP32 Firmware Defaults
- Default mode: VSYNC‑EOF.
- `CHUNK_BYTES=4090` (409×10), `DESC_COUNT=8`, `GDMA_CH=2`.
- Disable alignment/stitching in VSYNC‑EOF mode; keep VSYNC glitch filter enabled.

3) Verification (Hardware)
- IIgs burst test: confirm
  - Words received = 32768 exactly
  - Corruption rate = 0.0%
  - Words captured ≈ 80–81 (EOFs per 4 KB + final partial)
- Re-test with I2S enabled to verify no LCD_CAM interruption; GDMA relink should maintain streaming.

4) Fallback (If VSYNC cadence unavailable)
- Use length‑EOF with alignment detection + tail flush (send dummies until `bytes_sent % CHUNK_BYTES == 0`).
- Expect perfect counts with 0.0% corruption.

5) Ops/Docs
- Keep CLI: `lcammode vs|len`, `status`, `addrwin`.
- Document expected stats and pass criteria in README/firmware notes.
