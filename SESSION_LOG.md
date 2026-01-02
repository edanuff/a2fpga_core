# A2FPGA Session Log

A lightweight running log to persist context across CLI sessions.

Conventions
- Append newest entries to the top.
- Keep entries concise: what changed, why, and next steps.
- Use sections: Context, Decisions, Open Items, Next Steps.

---

## 2025-10-23 — Interruptable Padding (LEN‑EOF) Implemented

Context
- VSYNC‑EOF path produced unsatisfactory behavior in practice (frequent VSYNC EOFs, occasional mis‑parse when alignment heuristics were left on). We agreed to switch to a deterministic LEN‑EOF approach with interruptable padding from the FPGA.

Changes
- HDL `boards/a2p25/hdl/esp32/cam_serializer.sv`:
  - Added parameters `PAD_MODE` (bool) and `PAD_COUNT` (int, default 409).
  - In PAD mode: disable VSYNC, bypass cadence/idle‑flush, and emit dummy packets after the last real packet until `PAD_COUNT` is exhausted; any real packet preempts padding and resets the budget.
  - Introduced `dummy_pending_r` and refactored queuing so `pending_data_r` is driven only by the write‑queue block (fixes multi‑driver synth errors EX2000/EX1999).
  - Dummy/heartbeat word is `0xC0FF0000`.
- HDL `boards/a2p25/hdl/esp32/a2bus_stream.sv`:
  - Instantiates `cam_serializer` with `PAD_MODE=1'b1`, `PAD_COUNT=409` (safe, deterministic).
- ESP32 `boards/a2p25/src/a2fpga_esp32/a2fpga_lcam.cpp`:
  - Default `s_use_vsync_eof = false` (LEN‑EOF). Alignment + stitch path remains enabled in this mode; stats already count unique buffers.

Rationale
- Guarantees crossing the ESP32 chunk boundary (`CHUNK_BYTES=4090`) regardless of internal phase. Worst‑case overhead is 409 dummy packets per burst tail (~1.25% for 32,768 packets); minimal time/CPU cost.

Open Items
- Build a2p25 in Gowin; confirm synth/PnR clean and EX2000/EX1999 resolved.
- Flash and validate on hardware: 0 drops, 0 corruptions; stable low EOF rate; tail flush occurs promptly at idle.
- Ensure dummy packets are filtered from higher‑level stats/consumers as intended.
- Tune `PAD_COUNT` if desired (trade tiny risk vs. overhead) and consider a runtime register to switch PAD vs VSYNC without rebuild.
- Confirm alignment/stitch counters look healthy in LEN‑EOF.

Next Steps
- Build and test; gather stats; iterate on parameters or add runtime control as needed.

## 2025-10-23 — Hotfix: Restore VSYNC‑EOF defaults

Context
- No capture observed on hardware after enabling PAD mode; likely mode mismatch (FPGA PAD mode disables VSYNC while ESP32 ran VSYNC‑EOF from a stale build).

Changes
- HDL: Set `PAD_MODE=1'b0` in `boards/a2p25/hdl/esp32/a2bus_stream.sv` to re‑enable gated VSYNC.
- ESP32: Default `s_use_vsync_eof = true` in `boards/a2p25/src/a2fpga_esp32/a2fpga_lcam.cpp` so firmware matches VSYNC framing out of the box.

Next Steps
- Rebuild/flash FPGA (a2p25.fs) and upload ESP32 firmware. Verify `stats` shows non‑zero “Words captured/received”.
- After baseline verified, re‑introduce PAD mode behind a runtime switch to avoid mismatches.

## 2025-10-23 — Log Initialized

Context
- Created `SESSION_LOG.md` to capture ongoing work across sessions.

Decisions
- Store session notes in-repo for reliable persistence.

Open Items
- Backfill summary from recent commits and the last session once recalled.

Next Steps
- Confirm what we were tackling last session and record it here.
