# Rotated-colorbars offset-with-wraparound: mechanism, sim reproducer, fix proposal

Investigation of the hardware bug logged in `test_log.md` rows 49/53/56/60:
after a **marginal** DP link acquisition (retrain battles / video restarts),
the displayed image is offset with **clean wraparound** — horizontal-only
(~20 px), vertical-only, or both axes — and the offset is then rock-stable
for the session. Clean first-try acquisitions always render correctly.
Seen on 2 dies (GW5AT-60, GW5AST-138), 2 monitors, 2 DP→HDMI converter
sinks, n=4. Product impact: `board_1_0a4_requirements.md` item 11 (marginal
catches can ship a shifted screen; recovery requires a full re-acquisition).

Status: **mechanism identified by RTL analysis and REPRODUCED in simulation**
(`hdl/displayport/sim/tb_offset_wrap.v`). Fix proposed below as a diff
(not applied — shared production RTL).

---

## 1. Mechanism map: who counts what, in which clock domain

The generic video front-end (`hdl/displayport/dp_transmitter.sv` wiring):

```
board top rgb = f(cx,cy)          pixel domain          symbol domain
        │                          (clk_pixel =          (tx_symbol_clk =
        ▼                           sym × 11/10, PLL      SERDES word clk,
  dp_video_timing ──► pixel_cdc_fifo ──► video_stream_packer ──► msa_inserter
  (cx/cy counters,    (gray-ptr async    (line_cycle/line_num    ──► main_stream
   SOF marker,         FIFO, depth 4096   framing counters,           processing
   pack 2 px/word)     words)             BE/BS placement,            (idle ins.,
                                          FIFO pops)                   SR, scram)
```

### Pixel domain (`hdl/displayport/video/dp_video_timing.v`)

* `cx`/`cy` free-run over H_TOTAL×V_TOTAL from `reset` on. The board top's
  pattern is a registered pure function of them (the pull contract,
  `a2mega_dp_test_top.sv` ~line 131) — **the image cannot rotate relative
  to cx/cy**, confirming the row-60 by-construction argument.
* `capturing` latches at the first frame boundary after `capture_arm`
  (from the packer, constant 1 after reset) and **never clears**. Active
  pixels are packed 2-per-word and written to the FIFO; the first word of
  each frame carries a start-of-frame (SOF) flag (`fifo_wsof`).

### CDC FIFO (`hdl/displayport/video/pixel_cdc_fifo.v`)

* Standard gray-pointer dual-clock FIFO, depth 4096 words (ADDR_BITS =
  clog2(3×960) = 12), show-ahead read.
* **Writes while full are silently dropped** (`wen && !wfull` guard,
  line 79). No error is latched, the writer does not know.
* **Reads while empty are silently ignored** (`rd_en` does nothing when
  `rvalid_r`=0). Pointers are reset ONLY by `reset` (the global
  pushbutton reset in the dp_test top), never by link events.

### Symbol domain (`hdl/displayport/video/video_stream_packer.v`)

* `line_cycle` (0..1999) and `line_num` (0..1124) are **the framing
  counters**: they place BE at slot `TU_SIZE-1`, the data region, BS at
  `BS_POS` (3549 for 1080p 2-lane), VB-ID/Mvid/Maud — everything the sink
  uses to reconstruct raster position.
* **Video start is the single alignment event** (lines 659-670): while
  `!running`, the packer discards non-SOF words (`fifo_rd = fifo_rvalid &&
  !fifo_rsof`), then `start_ok <= fifo_rvalid && fifo_rsof && (fifo_rlevel
  >= PREFILL)`; `running` sets with `line_cycle=0, line_num=0` — pixel
  (0,0) is at the FIFO head, framing counters at frame origin. Correct by
  construction.
* **After start, `fifo_rsof` is never consulted again.** The packer pops
  exactly 960 words per active line (prime fetch + per-pixel fetches),
  forever. `running` clears only on `reset`. Retrains, link drops, idle
  switchovers — none of them touch the packer or the FIFO.

### Downstream (`core/idle_pattern_inserter.v`, `core/main_stream_processing.v`)

* On link drop / source drop, the idle inserter reverts to the idle
  pattern and re-switches to video at a safe switch point ≥64k symbols
  after `channel_ready` returns. This choreography was the subject of
  `tb_video_restart{,_v2}.v` and is **not** the offset mechanism — it
  gates WHETHER video flows, not its alignment.

## 2. The defect: one-shot alignment + silent word slip = permanent rotation

Because framing is aligned to the pixel stream exactly **once**, any net
word slip between the two domains after that moment rotates the image
within the active raster permanently:

* Let S = (words dropped by FIFO overflow) − (packer fetches missed on an
  empty FIFO). The word at framing slot k then contains source word k+S,
  so the whole active raster is rotated by Δ = 2·S pixels (2 px/word):
  **dx = Δ mod 1920, dy = Δ div 1920, clean wraparound in both axes,
  dx always even** (the hardware's ~20 px is even — consistent).

How marginal acquisitions produce the slip — the battle events:

* **Symbol-clock stall, pixel domain still clocking → overflow-drop.**
  `tx_symbol_clk` is the SERDES word clock; ladder retrain states and the
  auto-recovery watchdog genuinely power the PHY down (2 ms pulses + ~10 ms
  CSR replay), stopping it. `clk_pixel` comes from a fabric PLL whose
  reference is that same word clock — when the reference stops, the VCO
  free-runs near frequency for a long time (and on the 138B the pixel PLL
  is fed from a *gated* word clock — the row-56 caveat). The writer keeps
  writing; the FIFO pegs full after ~3.3 line-times (~44 µs) and then
  **drops every word** until the symbol clock returns. S > 0, arbitrary
  mod frame.
* **Pixel-clock stall, symbol domain clocking → underrun-miss.** When the
  PHY comes back the packer resumes popping instantly, but the pixel PLL
  is still relocking. The FIFO drains (only one line of slack: PREFILL =
  960 words) and every fetch on empty is a missed pop the packer's walk
  does not repeat. S < 0, arbitrary. The `underrun` sticky flag does set,
  but nothing acts on it.
* Real battles do both in sequence, with arbitrary durations → arbitrary
  net S → h-only, v-only, or both-axes offsets: exactly the n=4 spread.
* A start can even be **born misaligned**: if the FIFO overflowed before
  the very first `start_ok` (battle raging before video ever started),
  the backlog behind the SOF word contains drop-gaps; the packer starts
  "aligned" on a stream that is internally slipped.

Why the offset is rock-stable afterwards: once the link is quiet, both
domains are PLL-locked at exactly 11:10 — average rates match, the FIFO
level equilibrates, no further slip. Why clean catches never show it: no
battle → no clock-stall episode → FIFO never saturates → S = 0.

The hub/converter is an innocent renderer (row-60 demotion confirmed):
the shifted pixels really are shifted relative to BE/BS in our transmitted
stream; Fangor/Sceptre just render honestly.

Reconciliation with the row-60 leading theory ("phase latched wrong at a
mid-battle video start"): close, with one refinement. The video-start
alignment procedure itself is correct by construction — what a battle
does is either (a) corrupt the FIFO *backlog* the start latches onto
(pre-start overflow gaps), or (b) slip the already-latched alignment
afterwards through saturation events. Both are one-time events whose
effect is indistinguishable from "latched an arbitrary phase": constant
for the session, cleanly wrapped, different each incident. The
"pixel-request counters vs framing counters" framing is exactly right —
the missing piece was that the coupling between them is the FIFO's word
*count*, and both saturation directions silently break it.

## 3. Sim reproducer: `hdl/displayport/sim/tb_offset_wrap.v`

Full production chain at the real 1080p 2-lane HBR geometry (H_TOTAL 2200,
V_TOTAL 1125, SYMS_PER_LINE 4000, TU 64, PREFILL 960) with clk_pixel :
clk_symbol at the exact 11:10 ratio: board-top-contract rgb (registered
answer to cx/cy) **encoding (cx,cy) into the pixel value** →
dp_video_timing → pixel_cdc_fifo → video_stream_packer → msa_inserter_2ch
→ main_stream_processing.

A lane-0 stream decoder (packer output, post-framing) finds each BE,
decodes the first pixel of every line, and computes
Δ = decoded_raster_pos − framing_pos per frame (UNIFORM when all lines
agree — the clean-wrap signature). Scenarios drop the link and stall the
symbol clock (PHY-down model), the pixel clock (PLL-relock model), both
together, or neither, at controlled frame phases; after each retrain the
settled offset and the FIFO drop/miss counters are reported, plus the
idle→video switch status (the property the existing restart tbs guard).

Run (from repo root, ~35 M symbol cycles, several minutes):

```
iverilog -g2012 -o /tmp/tb_offset_wrap.vvp \
  hdl/displayport/sim/tb_offset_wrap.v \
  hdl/displayport/video/{dp_video_timing,pixel_cdc_fifo,video_stream_packer,msa_inserter_2ch}.v \
  hdl/displayport/core/{main_stream_processing,idle_pattern_inserter,scrambler_reset_inserter,scrambler_all_channels,insert_training_pattern,skew_channels}.v
vvp /tmp/tb_offset_wrap.vvp
```

The geometry is parameterized (stall lengths and restart phases are
expressed in line-times / frame fractions so the FIFO-saturation physics
scale). Configurations exercised, 2026-08-20, iverilog 12+ `-g2012`:

* **tiny** (H 64/110, V 48/60): full-mechanism sweep, seconds per run
* **wide** (H 1920/2200 — real width, real 4096-word FIFO, real
  words-per-line — V 64/70): real horizontal arithmetic
* **V/4** (full H, V 270/282, pre-parameterization binary): real-width
  corroboration with pixel-count stalls (20k/35k/50k pixel clocks)
* full 1080p/1125: compiles and boots clean; full sweep not practical in
  iverilog wall-time (each frame is 2.25M symbol cycles) — the scaled
  runs carry the evidence

### Results: BASELINE (unmodified production RTL)

Boot (clean acquisition) in every configuration: `dx=0 dy=0 UNIFORM` —
the video-start alignment procedure itself is correct.

Tiny sweep (offsets in pixels; `d_drop/d_miss` = FIFO overflow-drops /
starved fetches during the scenario window; `shift` = change vs the
previous scenario's settled offset):

| # | scenario (restart phase)     | dx | dy | frame     | drops | misses |
|---|------------------------------|----|----|-----------|-------|--------|
| 1 | CONTROL retrain only @ln 4   | 0  | 0  | UNIFORM STABLE | 0 | 0 |
| 2 | CONTROL both clocks stalled 13.5 ln @ln 12 | 0 | 0 | UNIFORM STABLE | 0 | 0 |
| 3 | SYM stall 1.5 ln (sub-saturation) @ln 9 | 0 | 0 | UNIFORM STABLE | 0 | 0 |
| 4 | SYM stall 9 ln @ln 4         | 0  | 0* | MIXED (net-zero limit cycle) | 2976 | 2976 |
| 5 | SYM stall 9 ln @ln 32        | 0  | **8**  | shifted | 3470 | 3462 |
| 6 | SYM stall 9 ln @ln 54 (vblank) | 0 | **17** | shifted | 3541 | 3348 |
| 7 | SYM stall 23 ln @ln 12       | 0  | **40** | shifted | 1854 | 1397 |
| 8 | PIX stall 9 ln @ln 4         | 0  | **31** | shifted | 3069 | 3472 |
| 9 | PIX stall 16 ln @ln 36       | 0  | **15** | shifted | 3188 | 3700 |
| 10| PIX stall 0.9 ln @ln 16      | 0  | **14** | shifted | 3155 | 3435 |

`RESULT: 7 scenario(s) shifted the image — BUG REPRODUCED`, `underrun
sticky=1`. (In the tiny geometry the offsets quantize to whole lines,
dx=0 — 32-word lines make partial-line slips rare.)

Wide sweep (real H 1920/2200, real 4096-word FIFO; boot and controls
clean, then; `RESULT: 7 scenario(s) shifted — BUG REPRODUCED`):

| scenario                  | dx      | dy | shift vs previous |
|---------------------------|---------|----|-------------------|
| SYM stall 9 ln @ln 6      | **566** | **5**  | +566, +5 |
| SYM stall 9 ln @ln 42     | **566** | **14** | 0, +9 |
| SYM stall 9 ln @ln 67 (vblank) | **564** | **23** | **−2**, +9 |
| SYM stall 23 ln @ln 16    | **564** | **46** | 0, +23 |
| PIX stall 9 ln @ln 6      | **564** | **37** | 0, −9 |
| PIX stall 16 ln @ln 48    | **564** | **21** | 0, −16 |
| PIX stall 0.9 ln @ln 21   | **784** | **20** | +220, −1 |

V/4 real-width sweep (pre-parameterization binary, pixel-count stalls;
boot and controls clean; `RESULT: 7 scenario(s) shifted — BUG
REPRODUCED`):

| scenario                  | dx      | dy | shift vs previous |
|---------------------------|---------|----|-------------------|
| SYM stall 20k px @ln 30   | **266** | 0  | +266, 0 |
| SYM stall 20k px @ln 150  | **466** | **9**  | +200, +9 |
| SYM stall 20k px @ln 275 (vblank) | **666** | **18** | +200, +9 |
| SYM stall 50k px @ln 60   | **64**  | **41** | −602, +23 |
| PIX stall 20k px @ln 30   | 0       | **32** | −64, −9 (pixel-stall slips the opposite sign) |
| PIX stall 35k px @ln 200  | **66**  | **16** | +66, −16 |
| PIX stall  2k px @ln 100  | **268** | **15** | +202, −1 |

Note the small incremental shifts (−2 px; +202/−1; +66/−16): near-even
saturation episodes leave exactly the small clean offsets seen on
hardware (~20 px class), while every horizontal component stays even
(2 px per slipped word) — a falsifiable hardware prediction.

Real-width slips produce **nonzero, even, per-phase-different horizontal
components combined with vertical components** — the full n=4 hardware
signature (h-only ~20 px, v-only, both-axes, always cleanly wrapped,
different every incident, stable per session):

* clean acquisition -> always (0,0);
* retrain without clock disturbance -> (0,0) (the idle/switch machinery
  is innocent, as `tb_video_restart{,_v2}` already established);
* equal stall of both clocks -> (0,0) (no relative slip);
* any relative stall below the FIFO's absorption slack -> (0,0);
* any relative stall beyond it -> a stable wrapped offset whose value
  depends on the restart phase and stall length.

Two post-slip regimes appear in sim: a **clean settled offset** (the
hardware-observed state) when the net writer/reader frame-phase lag fits
back inside the FIFO's slack, and a **frame-aligned sputter limit cycle**
(offset stable or zero but per-line wobble, continuous equal drops and
misses) when it does not. Hardware's clean stable screenshots correspond
to the first regime; the second may be what a "battled" session looks
like shortly before/without stabilizing. Both are cured by the v3 fix.

### Results: FIX v3 (scratch-patched packer, same testbench, tiny sweep)

| # | scenario                  | dx | dy | frame | drops | misses |
|---|---------------------------|----|----|-------|-------|--------|
| 1-3 | controls + sub-saturation | 0 | 0 | UNIFORM STABLE | 0 | 0 |
| 4 | SYM stall 9 ln @ln 4      | 0  | 0  | UNIFORM STABLE | 248 | 1 |
| 5 | SYM stall 9 ln @ln 32     | 0  | 0  | UNIFORM STABLE | 191 | 1 |
| 6 | SYM stall 9 ln @ln 54     | 0  | 0  | UNIFORM STABLE | 191 | 1 |
| 7 | SYM stall 23 ln @ln 12    | 0  | 0  | UNIFORM STABLE | 639 | 1 |
| 8 | PIX stall 9 ln @ln 4      | 0  | 0  | UNIFORM STABLE | 0 | 1 |
| 9 | PIX stall 16 ln @ln 36    | 0  | 0  | UNIFORM STABLE | 0 | 1 |
| 10| PIX stall 0.9 ln @ln 16   | 0  | 0  | UNIFORM STABLE | 0 | 1 |

`RESULT: all scenarios kept the boot alignment — bug NOT reproduced.`
Every disturbance is healed by a single self-restart (the lone starved
fetch is the trigger event); the idle->video switch re-fires in every
scenario; the packer's own shadow-model equivalence assertions stay
silent through every resync.

### Regression: existing testbench properties

* `tb_video_restart_v2.v` (untouched RTL, 2026-08-20): `BOOT switch
  fired / RETRAIN 1 fired / RETRAIN 2 fired`, `SS_syms=2`,
  `idleVBID_post=0`, full 8192-word dumps on all three — unchanged.
* The reproducer itself re-verifies the switch property after every
  simulated retrain (switch=1 in all scenarios, both RTL variants).

## 4. Proposed fix (NOT applied — diff against shared RTL)

Minimal principle: **make the alignment invariant continuously checked
instead of one-shot.** The stream already carries the ground truth (the
SOF marker); the packer verifies it at every frame's first fetch — and
conversely that it never appears mid-frame — and additionally treats any
starved fetch (underrun miss) as the slip event it is. On any trigger it
drops back to the existing `!running` re-alignment sequence (discard to
SOF, PREFILL, restart at a frame boundary), zeroing the framing counters
so the pre-start pipeline recomputes from reset-equivalent state. A slip
then costs one video restart (the sink sees the already-tested
idle/no-video blip and the switch re-fires) instead of a permanently
rotated session.

This is the v3 form, **iterated in simulation** (v1 without counter
zeroing tripped the packer's own shadow-model equivalence assertion at
the restart — the frozen mid-frame `line_cycle` leaked a stale prime
decision into the flag pipeline; v2 with only the SOF check left a
frame-aligned net-zero drop/miss limit cycle unhealed in 2 of 10
scenarios — offset 0 but per-line wobble; the starved-fetch trigger
heals that state too because it always exhibits misses):

```diff
--- a/hdl/displayport/video/video_stream_packer.v
+++ b/hdl/displayport/video/video_stream_packer.v
@@ -465,6 +465,28 @@
     reg fetch_r, prime_r, load0_r;
+    // ------------------------------------------------------------------
+    // OFFSET-WRAP FIX: frame-boundary SOF verification. The FIFO's
+    // start-of-frame marker must arrive exactly at the frame's first
+    // prime fetch. Any word slip (overflow drop or underrun miss during
+    // a retrain battle) breaks the invariant; on detection fall back to
+    // the !running re-alignment sequence (discard to SOF, PREFILL,
+    // restart at a frame boundary) instead of rendering a permanently
+    // rotated image. Presents downstream as a normal source_ready-drop
+    // video restart.
+    // ------------------------------------------------------------------
+    wire frame_first_fetch = prime_r && (line_num == {$clog2(V_TOTAL){1'b0}});
+    wire sof_mismatch = fifo_rvalid &&
+                        ((frame_first_fetch && !fifo_rsof) ||
+                         (fetch_r && !frame_first_fetch && fifo_rsof));
+    // A starved fetch (underrun) is itself a slip event: the walk
+    // advances without consuming. A frame-phase disturbance can settle
+    // into a frame-aligned net-zero drop/miss limit cycle that the SOF
+    // check alone cannot see (offset 0 but per-line wobble) — every
+    // such state exhibits starved fetches, so restarting on the first
+    // one heals it too.
+    wire fetch_starved = fetch_r && !fifo_rvalid;
     reg [1:0] dec_px_r, dec_fs_r, dec_fe_r;
@@ -694,6 +716,22 @@
                 if (line_cycle == CYCLES_PER_LINE-1) begin
                     line_cycle <= 0;
                     line_num   <= (line_num == V_TOTAL-1) ? {$clog2(V_TOTAL){1'b0}}
                                                           : line_num + 1'b1;
                 end else begin
                     line_cycle <= line_cycle + 1'b1;
                 end
+
+                // OFFSET-WRAP FIX: pixel-vs-framing slip detected — stop
+                // streaming and re-align at the next frame start. The
+                // counters are zeroed so the pre-start flag pipeline
+                // recomputes from the same state as after a reset (they
+                // freeze while !running; leaving them at mid-frame
+                // values would leak stale prime/fetch decisions into
+                // the first cycles of the restart). Placed after the
+                // counter increment so the zeroing wins.
+                if (sof_mismatch || fetch_starved) begin
+                    running    <= 1'b0;
+                    ready      <= 1'b0;
+                    start_ok   <= 1'b0;
+                    line_cycle <= 0;
+                    line_num   <= 0;
+                end
             end
```

Design notes:

* `start_ok` MUST be cleared explicitly: it is only ever assigned inside
  the `!running` branch and would otherwise still hold the stale 1 that
  started the previous session, re-arming `running` one cycle after the
  resync with no re-alignment at all.
* `line_cycle`/`line_num` MUST be zeroed: they freeze while `!running`,
  and the two-stage flag pre-computation (`pf_*` -> `p_*` -> `*_r`)
  keeps evaluating them during the pre-start wait. At boot they are zero
  (reset); a restart from frozen mid-frame values can coincidentally
  raise `pf_prime` the moment `start_ok` fires (sim-caught: the
  fetch-prediction shadow assert). Zeroing makes restart identical to
  the boot-proven path. The zeroing branch sits after the counter
  increment so its assignment wins.
* The SOF mismatch term requires `fifo_rvalid` (an underrun at the
  frame-first fetch has no flag to inspect) — but the starved-fetch
  trigger covers exactly that case, so every slip event is caught at
  its first observable symptom.
* The mid-frame term also fires on an SOF word appearing at any
  non-frame-first fetch (drop-slip moves the next frame's SOF early) —
  both slip signs are caught within at most one frame.
* Timing: the new cone into `running/ready/start_ok/line_*` is
  `prime_r, fetch_r, fifo_rsof, fifo_rvalid` plus an 11-bit
  `line_num==0` compare — small against the packer's registered-walk
  paths. If PnR ever lists it, `line_num==0` can be pre-registered the
  same way `ln1`/`pf_*` are (it only changes at end-of-line).
* Shadow-model interaction: the translate_off equivalence checks are
  gated on `running`; with the counter zeroing the v3 patch passes all
  of them through every resync (v1 did not — that is what caught it).
* The sticky `underrun` output keeps its diagnostic meaning (a set flag
  now additionally implies at least one self-heal restart happened).
* Optional belt-and-braces (not required once the SOF check exists): in
  `dp_video_timing.v`, clear `capturing` when a write is dropped
  (`wen && wfull`, needs the FIFO's `wfull` exported) and re-arm at the
  next frame boundary — quantizes writer-side loss to whole frames so
  the backlog behind an SOF is always internally clean.
* Fix verified in simulation against a scratch-patched copy of the
  packer (patch applied outside the repo; production RTL untouched):
  see results tables.

## 5. Interactions and baseline notes

* **Scrambler-reset high-half fix** (`cda5a1fc`, `tb_sr_highhalf.v`): no
  interaction. The proposed fix drops `ready` during re-alignment, which
  routes through the idle inserter exactly like today's video restarts;
  the SR inserter counts BS events on whatever stream flows and is
  agnostic to the switchover.
* **`tb_video_restart{,_v2}` properties**: the fix does not touch the
  idle/switch choreography; a re-alignment presents to
  `main_stream_processing` as the already-tested source_ready-drop
  restart. Reproducer scenarios confirm the switch re-fires after every
  simulated retrain.
* Pre-existing baseline observations (unchanged by this work, both
  reproduce identically on the untouched RTL):
  * `tb_sr_highhalf.v` reports `sr_events=3` vs its expected 2 and prints
    "TEST FAILED" **while all functional checks pass** (errors=0, SR on
    all lanes/high half, LFSR 00→FF reset). The RTL substitutes the BS
    event *after* `bs_count` wraps (events 2, 514, 1026 in 1200) — a
    512-period as intended; the tb's expected-count arithmetic (513/1025
    → 2) is off by the initial substitution. TB bookkeeping nit, not an
    RTL regression.
  * `tb_video_restart.v` (v1, synthetic 8-cycle-wide switch marker):
    phase-0 cases report "SWITCH NEVER FIRED in 400k cycles" while the
    four phase-disturbed retrains fire at ~66-69k cycles — the
    coincidence demonstration it was built for (its synthetic packer
    raises in_data[72] for only 8 cycles per period; the real packer's
    switch window is the whole vblank dummy region). Verified 2026-08-20
    on untouched RTL; same output as when the tb was preserved.

## 6. Confidence assessment

* **That word slip in the pixel CDC FIFO produces exactly the observed
  symptom** (stable, cleanly-wrapped, per-incident-arbitrary offset;
  dx even; clean catches immune): HIGH — by construction from the RTL
  (one-shot SOF alignment + silent drop/miss paths) and demonstrated in
  the reproducer with the real production modules at the real geometry.
* **That the hardware's slip events are the battle-time clock stalls
  modeled here** (PHY power-down stopping the SERDES word clock under a
  still-ticking pixel PLL VCO, and pixel-PLL relock under a running
  symbol clock): MEDIUM-HIGH. The ladder and the auto-recovery watchdog
  demonstrably power the PHY down mid-battle, the pixel PLL's reference
  IS the gated word clock (explicitly so on the 138B), and no other
  identified path can move pixels relative to framing (the packer's
  internal walk is deterministic; the board pattern is a pure function
  of cx/cy). Alternative slip triggers (e.g. brief symbol-clock
  glitching during CSR replay) reduce to the same FIFO-saturation
  mechanism, so the fix covers them regardless — this uncertainty
  affects the narrative, not the remedy.
* **Hardware discriminators** (cheap, next bench session):
  * The packer's `underrun` sticky flag is the smoking gun for the
    miss-direction slip — expose it in the UART status line (one spare
    field); a session that shows the offset should show it set (or,
    with a drop-side counter added, the drop case).
  * Predicted invariant: the horizontal component of any observed
    offset is always an even number of pixels (2 px per slipped word).
    ~20 px (row 49) is consistent; worth confirming on the next
    incident with a screen photo.
  * A fixed build must never show a rotated session; worst case is a
    sub-second video blink at the moment the slip would have happened.
* **Residual risk of the fix**: false resyncs would require a spurious
  SOF flag, a lost SOF flag, or a starved fetch without any slip — the
  first two would be FIFO data corruption (out of scope), and the third
  cannot occur in a healthy steady state (all control scenarios show
  zero misses; PREFILL is the boot-proven margin). A resync storm would
  require a *persistent* clock-rate disturbance, during which no
  correct video is possible anyway, and each restart re-establishes the
  boot-proven operating point. The resync path reuses the existing
  `!running` machinery; the new logic is 3 wires + 5 register next-state
  terms. Timing addition is small and can be pre-registered if PnR
  objects (see design notes). Fix iterated against the packer's own
  shadow-model assertions (v1 was caught by them; v3 passes them
  through every resync).
