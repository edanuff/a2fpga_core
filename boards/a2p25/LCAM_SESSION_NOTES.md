LCAM Capture: Investigation Summary and Playbook

Context
- Board: A2P25 (Tang Primer 25K) with ESP32‑S3 capture side.
- Goal: Lossless capture of IIgs ES5503 audio bursts (32,768 packets) via LCD_CAM.
- Baseline issue: ~25% packet loss on ESP32; occasional corruption; I2S interaction stopping capture.

Protocol and Timing
- Serializer: 10 nibbles/packet (8 data, VSYNC nibble, stopper nibble). PCLK = 13.5 MHz (COUNT_WIDTH=2 from 54 MHz).
- Throughput: ~13.5 MB/s during bursts; typical burst length ~24 ms for 32,768 packets.

FPGA (SPI/Protocol) Fixes
- Corrected SPI bit ordering and sync handling in `esp32_spi_proto_proc.sv`.
- Added one‑shot first‑byte load for register/XFER reads to avoid phase hazards.
- Removed multi‑driver conditions (e.g., `tx_shift` single driver in fall‑edge loader).
- Simulation: `make -C boards/a2p25/tests spi_connector` passes (reg read/write and XFER readback).

ESP32 LCD_CAM Capture Architecture
- Two operating modes supported:
  1) Length‑EOF mode (recommended fallback):
     - Large DMA chunks (`CHUNK_BYTES ≈ 4090`, multiple of 10), descriptor ring (`DESC_COUNT ≥ 8`).
     - Alignment detector per chunk + cross‑boundary stitching via 0–9 byte tail.
     - Optional tail flush: pad dummy packets until `bytes_sent % CHUNK_BYTES == 0` for exact counts.
  2) VSYNC‑EOF mode (preferred when FPGA can gate VSYNC):
     - Gate VSYNC once every N=409 packets, plus one VSYNC at end‑of‑burst.
     - Disable alignment/stitching (buffers start on packet boundary by design).
     - Expect ~80–81 EOFs for a 32,768‑packet burst with `CHUNK_BYTES=4090`.

Key ESP32 Settings/Changes
- GDMA channel isolation: `GDMA_CH=2` for LCD_CAM to avoid I2S conflicts.
- GDMA recovery: background poller re‑arms link if `in.link.addr == 0` (function `lcdcam_recover_if_needed`).
- VSYNC filter: `cam_vsync_filter_thres = 1` to suppress glitches.
- CLI toggles: `lcammode vs|len`, `status`, `addrwin`, `i2sstart`, `i2sstop`.

Stats and Expected Results
- Success criteria (burst of 32,768 packets):
  - Words received (non‑heartbeat): 32768
  - Corruption rate: 0.0%
  - Words captured (EOF count): ≈ 80–81 in VSYNC‑EOF, ≈ 80–90 in length‑EOF.
  - Ring buffer drops: 0

Recommended Defaults
- Mode: VSYNC‑EOF (when FPGA provides gated VSYNC cadence).
- `CHUNK_BYTES = 4090`, `DESC_COUNT = 8`, `GDMA_CH = 2`, VSYNC filter = 1.

IIgs Test Procedure
1) Load and run the tight write loop to `$C03D` (32,768 iterations). Example bytes:
   - Assembly skeleton and byte encoding are documented in `boards/a2p25/TODO.md` under Current Work.
2) On ESP32 serial console:
   - `+++` (enter console)
   - `status` (confirm LCD_CAM running)
   - `lcammode vs` (or `lcammode len`)
   - Optional: `i2sstart` to run concurrent I2S slave‑TX (verify GDMA stability)
   - After payload completes, run `status`/`stats` to read results.

Troubleshooting
- Fewer than 32768 words:
  - In VSYNC‑EOF: ensure FPGA gates VSYNC every 409 packets and asserts a final VSYNC after the burst.
  - In length‑EOF: enable tail flush on the sender (pad dummies until `bytes_sent % CHUNK_BYTES == 0`).
- Corruption > 0%:
  - Ensure alignment/stitching disabled in VSYNC‑EOF; enabled and active in length‑EOF.
  - Confirm PCLK is 13.5 MHz and serializer nibble order matches parser expectations.
- LCD_CAM stalls after starting I2S:
  - Verify `GDMA_CH=2`, background recovery enabled, and `i2sstop`/`i2sstart` do not reset GDMA link persistently.

Notes
- Using length‑EOF with alignment/stitching yields 0.0% corruption and near‑perfect counts; adding a sender‑side tail flush guarantees exact 32768.
- VSYNC‑EOF provides naturally aligned buffers and low CPU overhead when cadence and final pulse are correct.

Gotcha: VSYNC‑EOF buffer alignment
- LCD_CAM asserts EOF at the VSYNC sample (nibble 8). The next buffer typically begins at nibble 9, not data nibble 0.
- We initially disabled alignment in VSYNC‑EOF, assuming buffers began on packet boundaries; this caused “corrupted” idle packets with address ranges like $0FF0–$FFFF.
- Fix: Always run alignment detection (both modes). Keep cross‑boundary stitch only for length‑EOF.

---

Current Implementation Status (snapshot)
- HDL
  - `hdl/esp32/cam_serializer.sv`: `SYNC_EVERY_PKTS=409`, idle flush forces one VSYNC at end of burst; `PAD_MODE=0` (no automatic padding); PCLK from COUNT_WIDTH=2 → 13.5 MHz.
  - `hdl/esp32/a2bus_stream.sv`: Instantiates serializer with `.SYNC_EVERY_PKTS(409)`, `.PAD_MODE(1'b0)`; heartbeat packets = `0xC0FF`.
- ESP32
  - `src/a2fpga_esp32/a2fpga_lcam.cpp`: Defaults to VSYNC‑EOF (`s_use_vsync_eof=true`), `CHUNK_BYTES=4090`, `DESC_COUNT=8`, `GDMA_CH=2`, VSYNC filter = 1.
  - Alignment/stitching: disabled in VSYNC‑EOF, enabled in length‑EOF; tail saved only in length‑EOF.
  - Robustness: background `lcdcam_recover_if_needed()` for GDMA inlink resets; status/CLI present (`lcammode vs|len`, `i2sstart|i2sstop`).
  - SPI helpers: XFER dummy properly consumed; status captured on header.

Verification Checklist (hardware)
- Mode VSYNC‑EOF with gated VSYNC every 409 and final VSYNC:
  - Words received (non‑HB): 32768
  - Corruption rate: 0.0%
  - Words captured (EOFs): ~80–81
  - Ring drops: 0
  - With I2S running: same as above
- Mode length‑EOF:
  - Use alignment + stitching; sender pads dummies to `CHUNK_BYTES` boundary → exactly 32768 received, 0.0% corruption.

Regression Guardrails
- Keep `CHUNK_BYTES` a multiple of 10; if `SYNC_EVERY_PKTS` changes, adjust `CHUNK_BYTES ≈ 10*N` and docs.
- Do not re‑enable per‑packet VSYNC→EOF (i.e., `SYNC_EVERY_PKTS=1`) without switching firmware to length‑EOF.
- Preserve `GDMA_CH=2` for LCD_CAM; confirm recovery path remains in poller.
- Keep VSYNC glitch filter enabled (`cam_vsync_filter_thres=1`).
- Always run the I2S concurrency test after capture changes.

Open Items / Next
- Re‑verify exact 32768 counts on current bitstream + firmware in VSYNC‑EOF with I2S active.
- Optional: expose `SYNC_EVERY_PKTS` as a top parameter switch if experimenting with other chunk sizes.

Changelog
- 2026‑01‑18: Added status snapshot, checklist, and guardrails; confirmed gated VSYNC (N=409), VSYNC‑EOF default, CHUNK_BYTES=4090, DESC_COUNT=8, GDMA_CH=2.
- 2026‑01‑18: Re‑enabled alignment detection in VSYNC‑EOF to handle buffers starting at nibble 9. Added per‑buffer debug log (level 1: changes/periodic, level 2: every buffer) and `statsbrief` + `lcamlog` CLI.
- 2026‑01‑18: Fixed CLI parsing so `lcamlogevery`/`lcamlograte` are not shadowed by `lcamlog` (exact match for `lcamlog`).

Debug Logging Usage
 - Set level: `lcamlog 1` (changes/periodic) or `lcamlog 2` (every buffer). Disable: `lcamlog 0`. Levels >2 are clamped to 2.
- Quick summary: `statsbrief` prints `mode/off/words/cap/drops`.
- One‑shot burst: `lcamdebug N` logs the next N buffers regardless of level (useful to capture a transition), `lcamdebug 0` cancels.
- Throttle/ cadence:
  - `lcamlogevery N` emits at most one log every N buffers (level≥1; still logs when offset changes).
  - `lcamlograte ms` enforces a minimum wall‑time between logs (burst unaffected). Default ~500 ms.
- Log line format:
  `[LCAM] buf#<n> mode=<VSYNC|LEN> len=<bytes> off=<0..9> proc=<packets> seen=<words> cap=<bufs> drop=<ring> a0=$<addr>`
  - `off`: detected 10‑byte phase (expect stable non‑zero in VSYNC‑EOF; 0 in length‑EOF if flush/pad aligns).
  - `a0`: first word’s address in buffer (quick sanity for ES5503 vs heartbeat/reset).

Stats Interpretation Notes
- Idle VSYNC‑EOF behavior: Expect small `len` (~19–20), `proc≈2`, `a0=$C0FF` (heartbeat) and `off` fluttering (1–2) — this is normal and not corruption.
- Burst behavior: `off` stabilizes (typically 1), `a0=$C03D`, `words received` climbs with 0 drops/corruption.
- Accounting: `words received` excludes heartbeat (`$C0FF`) but may include the single reset indicator packet (address `$0000`), so it can be +1 vs ES5503 packet count; bus stats exclude reset packets by design.
- VSYNC cadence sanity: In VSYNC‑EOF, derive cadence as `words_received / words_captured` (printed by `stats`). Expect ~409 with current gating; large deviations suggest an old bitstream or unintended VSYNC sources.
  - If you observe ~250–350 instead of ~409, length‑based EOF is also firing (rec_data_bytelen not disabled). Firmware sets `cam_rec_data_bytelen=4095` in VSYNC‑EOF to suppress length EOF; ensure you reflashed after this change.
