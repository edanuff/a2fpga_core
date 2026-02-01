# A2FPGA Session Log

A lightweight running log to persist context across CLI sessions.

Conventions
- Append newest entries to the top.
- Keep entries concise: what changed, why, and next steps.
- Use sections: Context, Decisions, Open Items, Next Steps.

---

## 2026-01-31 — Extended Cycle Output Reproduction

Context
- User observed in VCD that extended (IIgs) cycles were detected (`cycle_extended` flag) but not reproduced in `phi1_o`. The output always generated fixed 14-tick cycles, causing 2 ticks of drift per extended cycle (~every 65th cycle on IIgs).

Problem
- Cannot detect extended cycle before Phi1 normally falls at phase 7. At output phase 6, `cdc_phi1_w` is HIGH for **both** normal and extended cycles because the real Phi1 hasn't fallen yet in either case.
- The distinction only becomes clear after the normal Phi1 falling point: in normal cycles, `cdc_phi1_w` goes LOW after CDC_LATENCY clocks; in extended cycles, it stays HIGH.

Solution
- **Mid-cycle detection**: After the predicted Phi1 negedge (phase 7), start a counter. At `CDC_LATENCY + ACCEPT_WINDOW + 2` FPGA clocks, check `cdc_phi1_w`. If still HIGH → extended cycle detected. If LOW → normal cycle.
- **Phase 13 hold**: When an extended cycle is detected, hold the phase counter at phase 13 for 2 extra ticks before wrapping to phase 0. This extends the output cycle from 14 to 16 ticks, matching the input period.
- **Placement**: The 2 extra ticks are inserted in the Phi1 LOW / Q3 LOW portion (end of cycle), not in the Phi1 HIGH portion. This is because detection happens after Phi1 has already fallen.

Verification
- Test 8 (new): 200 CPU cycles with extended cycles every 65th. Drift range = 0.2 ns. Without compensation, would be ~420 ns. Confirms drift prevention works.
- All 8 tests pass (was 7, added extended cycle drift test).

Files Changed
- `hdl/bus/a2bus_timing.sv` — Added `extend_hold_w` combinational signal, `current_cycle_extended_r` detection, `extend_hold_count_r` counter, `clks_after_phi1_negedge_r` timer. Modified phase counter `always_comb` to hold at phase 13 when `extend_hold_w` active.
- `tests/bus/timing/a2bus_timing_tb.sv` — Updated Test 6 to verify output period extension. Added Test 8 for drift stability with extended cycles. Increased timeout to 900 cycles.

Open Items
- Output Phi1 HIGH duration is NOT extended (it's still 7 ticks). The extra 2 ticks go into the Phi1 LOW portion. If downstream logic depends on the Phi1 HIGH duration being 9 ticks (like the real IIgs), this would need a different approach (speculative hold or CDC-coupled Phi1 output).
- Q3 pattern during extended output cycle: phases 12-13 repeat with Q3=0, which is correct for the extended pattern but different from the input's Q3 timing.

---

## 2026-01-31 — Q3 Fix, Phase Lock Drift Fix

Context
- Q3 was inverted in PHASE_MAP (validated against Apple IIe HAL timing diagram).
- Phase lock drifted because 7M half-period (~3.77 FPGA clocks) was smaller than CDC_LATENCY + ACCEPT_WINDOW, making every edge "accepted" and corrections random.

Fixes
- Corrected Q3 in PHASE_MAP: rises at phases 0 and 7 (with Phi1 transitions), falls at phases 4 and 11. 4-high/3-low asymmetric pattern.
- Switched phase lock tracking from 7M posedges to Phi1 posedges (~53 FPGA clocks per cycle, providing adequate resolution).
- Added Test 7: drift monitor measuring input-to-output Phi1 alignment over 100 cycles. Drift range: 1.0 ns.

---

## 2026-01-31 — Predictive Bus Timing Generator + APPLE_HZ Plumbing

Context
- Replaced the old CDC-passthrough `a2bus_timing.sv` with a predictive clock generation engine that free-runs at nominal Apple II frequencies and phase-locks to input signals. This eliminates CDC latency from the output path and provides noise immunity.

Architecture
- **Fractional-N accumulator**: generates 14M ticks at exact average APPLE_HZ rate from FPGA clock (54 MHz). Pattern borrowed from `audio_timing.sv`.
- **14-phase counter (0-13)**: maps each 14M tick to output states {7M, Phi1, Q3} via PHASE_MAP lookup table. One CPU cycle = 14 ticks.
- **Phase lock FSM**: UNLOCKED → ACQUIRING → LOCKED. Snaps on CDC'd Phi1 posedge, tracks via 7M posedges. CDC latency compensated via accumulator pre-load (`SNAP_ACC_PRELOAD = CDC_LATENCY * TICK_RATE`).
- **Next-phase lookahead**: `always_comb` computes next phase combinationally; output decode uses `PHASE_MAP[next_phase_w]` for zero-latency registered outputs.
- **Extended cycle detection**: counts FPGA clocks between CDC'd Phi1 posedges; threshold set midway between normal (14-tick, ~53 clocks) and extended (16-tick, ~61 clocks) periods.

Key Design Decisions
- CDC'd edges are used ONLY for phase comparison, never for output generation.
- Acceptance window is dual-sided: edges near current predicted edge OR near next predicted edge are both valid (handles fractional-N jitter where 7M half-period alternates between 7 and 8 FPGA clocks).
- Correction nudges are ±25% of TICK_RATE, rate-limited to once per 8 clocks, to prevent oscillation.
- IIgs extended cycle = 16 ticks of 14M (2 extra during Phi1 HIGH). Every 65th cycle. 912 ticks/scanline.

APPLE_HZ Parameter Plumbing
- Added `APPLE_HZ` parameter to all 6 board `top.sv` files (default: 14_318_181 for NTSC)
- Passes through `top.sv` → `apple_bus.sv` → `a2bus_timing.sv`
- PAL builds can override at the top level:
  - Europlus/IIe PAL: `APPLE_HZ = 14_250_450`
  - IIc PAL: `APPLE_HZ = 14_250_000`
  - IIe Platinum Intl NTSC 50Hz: `APPLE_HZ = 14_238_000`
- The phase lock controller is frequency-agnostic; even with wrong APPLE_HZ (~0.5% off for PAL), it will lock and function. Correct APPLE_HZ just reduces correction frequency and output jitter.

Crystal Frequencies (reference)
- NTSC: 14,318,181 Hz (315/22 MHz, 4× NTSC color subcarrier)
- PAL Europlus/IIe: 14,250,450 Hz (NDK crystal, targeting 15,625 × 912)
- PAL IIc: 14,250,000 Hz (exact 15,625 × 912)
- IIe Platinum Intl: 14,238,000 Hz (non-standard compromise)
- IIgs: 28,636,363 Hz (315/11 MHz, divided to 14.318M by VGC) — NTSC only

Files Changed
- `hdl/bus/a2bus_timing.sv` — Complete rewrite: predictive timing generator
- `hdl/bus/a2bus_if.sv` — Added `timing_lock`, `timing_error`, `cycle_extended` signals
- `boards/*/hdl/top.sv` (all 6) — Added `APPLE_HZ` parameter, passed to apple_bus
- `boards/*/hdl/bus/apple_bus.sv` (all 6) — Pass `APPLE_HZ` to a2bus_timing instantiation
- `tests/bus/timing/` — Testbench with 6 tests (lock acquisition, steady-state, noise immunity, clock loss/re-acquisition, phase relationships, extended cycle detection)

Testbench Tests (all passing)
1. Lock acquisition (~1.1 us from first input edge)
2. Steady-state: exact edge counts over 50 CPU cycles
3. Noise immunity: 60 Phi1 glitches injected, lock maintained
4. Clock loss and re-acquisition
5. Phase relationships: 7M/Phi1 high/low ratio = 0.75
6. Extended cycle detection: single 16-tick cycle, 65-cycle pattern (64+1) x2

Open Items
- Q3 phase mapping may need validation against actual Apple II hardware (current mapping based on standard divider chain analysis)
- Auto-detection of NTSC vs PAL by measuring input frequency (Option D) deferred — manual parameter override sufficient for now
- No PAL-specific board builds exist yet; APPLE_HZ override is ready when needed

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
