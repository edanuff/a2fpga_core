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
- Guru Meditation (stack canary) in `lcam_poll`:
  - Cause: poller task stack too small under heavy logging/printf.
  - Fix: increased `lcam_poll` stack to 4096 bytes. Keep logging throttled (`lcamlog 1`, `lcamlogevery 200`, `lcamlograte 1000`).

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
 - Throttle/ cadence (defaults set):
  - Default: level=1, every=200 buffers, rate=1000 ms (safe, low-noise).
  - `lcamlogevery N` emits at most one log every N buffers (level≥1; still logs when offset changes).
  - `lcamlograte ms` enforces a minimum wall‑time between logs (burst unaffected).
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

Presets
- `lcampreset normal`: VSYNC‑EOF, quiet logging (no extra setup, good default).
- `lcampreset canon`: length‑EOF, quiet logging (for canonical pretty EOF stats; requires sender padding).

---

ES5503 Bring‑Up Notes

Overview
- Path: IIgs writes $C03C–$C03F → ESP32 GLU interprets (DOC registers vs wave RAM) → ES5503 emulation generates samples → I2S slave‑TX to FPGA.
- Status: Recognizable audio achieved from IIgs program; tone/fulltest confirm I2S; capture integrity maintained.

Key Behaviors & Gotchas
- Sample encoding: ES5503 uses unsigned bytes; mixer converts via `sample ^ 0x80`.
  - 0x80 = DC zero (silence). A region of 0x80 will be silent (no halt).
  - 0x00 halts the oscillator (end marker). Ensure first non‑zero sample to avoid immediate halt.
- Oscillator enable (E1): Value is `(num_osc - 1) << 1`. e.g., `E1=0x02` enables 2 voices.
- Control (A#): bit0=halt; modes include FREE(0), ONCE(1), SYNC(2), SWAP(3). Pairing matters in SWAP.
- Mixer coverage: Emulator updated to scan all 32 oscillators so voices configured before E1 aren’t missed.

ESP32 Counters & Mirrors
- GLU counters: `Wave memory writes` (C03D RAM), `DOC reg writes` (C03D DOC), `GLU ctrl` (C03C), `GLU addr` (C03E/F).
- ES writes (FPGA‑mirror): ESP32-side mirror of total writes to $C03C–$C03F since reset, with breakdown.
  - Usage: `es5503resetwrite` → playback → `es5503info` → compare `total` with OSD delta; `total ≈ RAM bytes + control/address/DOC writes`.

CLI (Audio)
- Start/stop: `es5503start` / `es5503stop` (auto‑starts I2S on start), `audiostop` (halt all voices), `i2sstop`.
- Diagnostics: `es5503info` (osc table + counters + ES write mirror), `es5503reg <reg> [val]` (accepts $E1/0xE1/E1), `es5503mem <addr> [len]`.
- Tests: `fulltest` (load sine & play), `starttone`/`stoptone` (I2S route check).
- Resets: `es5503resetwrite` (GLU/ES write counters), `resetstats` (bus/LCD_CAM stats).

Typical Flow
1) `lcampreset normal` → verify capture (`stats`).
2) `es5503start` → run IIgs program → `es5503info`.
3) If silent: check voice halt bits (A#), E1 value, and wave region (non‑0x80). Unhalt voice once RAM filled.
4) Correlate ES bytes: `es5503resetwrite` before playback; `es5503info` after → ES writes total should match manual OSD delta per playback.

Next Steps
- Add voice activity + peak meter to `es5503info`.
- Optional compat toggle to ignore 0x00 halts during bring‑up (off by default).

---

ES5503 Sample Rate Conversion (Jan 2026)

Problem
- ES5503 native sample rate: `clock / 8 / (oscsenabled + 2)` = 7159090 / 8 / 34 = **26,320 Hz** (for 32 oscillators)
- FPGA I2S output rate: **48,000 Hz**
- Without conversion, audio plays 1.82x too fast ("chipmunk" effect)

Solution: Zero-Order Hold Upsampling
- Generate 281 ES5503 samples per 512-sample I2S buffer (281/26320 ≈ 512/48000 ≈ 10.67ms)
- Upsample via sample duplication: output[i] = es5503[i * 281 / 512]
- Each ES5503 sample is repeated ~1.82 times on average

Key Learnings
1. **Oscillator count default**: IIgs writes register 0xE1 at boot to set 32 oscillators, but this
   happens before ESP32/LCAM capture is ready. Default m_oscsenabled to 32 in es5503.cpp reset().

2. **Rate calculation**: With oscs=1 (wrong), rate = 298,295 Hz. With oscs=32 (correct), rate = 26,320 Hz.
   The gen=281 sample count was correct, but wrong oscs broke everything else.

3. **SWAP mode tracking**: Attempted to track where partner oscillators should start in the buffer
   to avoid overlap. This caused gaps instead because when odd oscillators (1,3,5...) trigger even
   partners (0,2,4...), the even ones were already processed earlier in the oscillator loop.
   Reverted to original MAME behavior (partners start from sample 0, slight overlap is acceptable).

4. **Audio gaps root cause**: ~10ms gaps occurred because register writes from IIgs arrive AFTER the
   I2S task has already generated silence for that time period. The timeline was:
   - T=0: IIgs writes register to start oscillator
   - T=0-10ms: LCAM packet in transit, I2S outputs silence (oscillator not started yet)
   - T=10ms: Register write arrives, oscillator starts
   - Result: 10ms gap at start of each note

5. **Prebuffer solution**: Added a ring buffer that delays audio output by ~15ms. Now the timeline is:
   - T=0: IIgs writes register
   - T=10ms: Register write arrives, oscillator starts generating into ring buffer
   - T=15ms: Audio output begins (prebuffer has filled)
   - Result: No gap because register writes arrive before we need the audio

Verified Result
- Pitch is now correct (audio plays at proper speed)
- Gaps addressed via two mechanisms: prebuffer + grace period (see below)

---

Audio Prebuffer Implementation (Jan 2026)

Problem
- IIgs register writes travel: CPU → FPGA → LCAM serializer → DMA → ESP32 → ES5503 emulator
- This path has ~10ms latency
- Without prebuffering, I2S outputs silence while waiting for register writes to arrive
- Result: audible ~10ms gap at the start of every note

Solution: Ring Buffer with Prebuffer
- Added 2048-frame ring buffer (~42ms capacity at 48kHz)
- ES5503 generates samples into the ring buffer (producer)
- I2S reads from ring buffer with 15ms delay (consumer)
- Buffer must fill to 720 frames (~15ms) before output starts

Implementation Details
- Buffer size: 2048 stereo frames (4096 int16_t values)
- Default prebuffer: 720 frames = 15ms (adjustable 5-40ms)
- Single producer (ES5503 generation), single consumer (I2S output)
- No locks needed (volatile read/write positions suffice)
- Underrun handling: output silence, log warning, continue

CLI Commands
- `i2sstatus` - shows ring buffer status, prebuffer state, underrun count, gaps detected
- `prebuffer <ms>` - adjust prebuffer latency (5-40ms)
- `es5503start` - resets ring buffer and starts fresh

Finding: Prebuffer Alone Didn't Fix Gaps
- Testing showed 0 underruns but gaps still present
- Gap logging revealed transitions from active→silent→active in ES5503 generation itself
- Gaps were 11ms, 49ms durations - occurring DURING audio generation, not I2S output
- Conclusion: gaps were baked into generated audio, prebuffer only delays output

---

Oscillator Grace Period (Jan 2026)

Problem: Timing Granularity Mismatch
- Real ES5503 generates audio continuously at 26kHz (one sample every ~38μs)
- Our emulator generates in ~10ms chunks (281 samples at a time)
- When IIgs software briefly halts an oscillator (to update wave table, change parameters):
  - Real ES5503: ~100μs of silence = imperceptible
  - Our emulator: if halt happens during our chunk, entire 10ms chunk = silence = very noticeable

Diagnostic Evidence
```
ES5503: rate=26320 oscs=32 buf=512/2048 prebuf=YES active=26/50 gaps=0
[GAP] #1 duration=11ms
[GAP] #2 duration=49ms
ES5503: rate=26320 oscs=32 buf=512/2048 prebuf=YES active=45/50 gaps=2
```
- 0 underruns = ring buffer working fine
- gaps detected = ES5503 emulator itself transitioning between audio and silence
- Gap durations (11ms, 49ms) match our chunk timing

Solution: Grace Period for Halted Oscillators
- Track when each oscillator was last generating audio (`last_active_ms`, `was_generating`)
- When oscillator is halted (control bit 0 set), check if it was recently active
- If halted for less than GRACE_PERIOD_MS (20ms), continue generating audio from it
- Only truly stopped oscillators (halted >20ms) go silent

Implementation in es5503.cpp:
```cpp
bool is_halted = (pOsc->control & 1);
bool in_grace_period = pOsc->was_generating &&
                       (now_ms - pOsc->last_active_ms) < GRACE_PERIOD_MS;
bool should_generate = (!is_halted || in_grace_period) && channel_matches;
```

Why This Works
- IIgs software typically halts oscillators briefly (<1ms) to update parameters
- Our 20ms grace period bridges these brief halts
- Audio continues smoothly instead of having 10ms gaps
- Oscillators that are truly stopped (user stops playback) will halt after 20ms

Tradeoffs
- Sounds may continue for up to 20ms after software halts them
- For music/sound effects, this is imperceptible
- If issues arise, grace period can be reduced to 10ms

---

MAME-Style Stream Update (Jan 2026)

After reviewing MAME's ES5503 source code, we discovered the proper solution.

MAME's Approach (from es5503.cpp)
```cpp
void es5503_device::write(offs_t offset, u8 data)
{
    m_stream->update();  // Generate audio up to NOW before applying write
    // ... then apply register changes
}
```

Every register write first generates all pending audio samples, THEN applies the change.
This ensures audio state is captured BEFORE the register modification takes effect.

Our Implementation
Two triggers generate audio into the ring buffer:
1. **Timer trigger**: I2S task calls `es5503_stream_update()` when it needs samples
2. **Write trigger**: `handle_es5503_write()` calls `es5503_stream_update()` before applying each register write

Both use a shared timestamp (`s_last_update_us`) to know how many samples to generate.

Key Components
- `s_es5503_mutex`: FreeRTOS mutex protects ES5503 state (LCAM task + I2S task)
- `s_last_update_us`: Timestamp of last audio generation (microseconds)
- `es5503_stream_update()`: Generates samples since last update, pushes to ring buffer
- Ring buffer: Same prebuffer infrastructure, now fed by both triggers

Why This Works
When IIgs software does:
```
1. Halt oscillator (to change settings)
2. Update wave pointer
3. Update frequency
4. Restart oscillator
```

Each write triggers stream update FIRST:
- Write 1: Generate audio (oscillator running), then apply halt
- Writes 2-3: Generate tiny amount (halted), apply changes
- Write 4: Generate tiny amount (halted), restart oscillator
- Next timer: Generate audio (oscillator running)

Only microseconds of silence between writes, not 10ms chunks.

This matches MAME's behavior and should eliminate the timing granularity gaps.
