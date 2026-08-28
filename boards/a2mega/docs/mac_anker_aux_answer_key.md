# Mac ↔ Anker AUX capture: the conformance answer key

2026-08-26. AD3 on a USB-C breakout inline between a MacBook Pro and the
Anker hub (Sceptre on the hub's HDMI). CH1 = SBU1 (A8), CH2 = SBU2 (B8;
turned out flat — decode is effectively single-ended on SBU1, which is
sufficient). 30 s window, 4 MS/s, plug-in inside the window. The hub was
externally POWERED throughout both captures — i.e. every capture is a
hot-attach to a running hub, the same condition our hub-backfed board
sees on every power cycle, so the Pass C diff is apples-to-apples
including hub MCU state. Raw CSV =
`mac_aux_bringup.csv` (~3 GB, not committed); full annotated transcript =
`mac_anker_bringup_annotated.txt` (committed alongside this doc).

Decoder chain fixed in the same session (see tools):
- `ad3_aux_capture.py`: streams raw samples to disk (long windows no
  longer exhaust RAM); zero-fills lost samples to keep the timeline.
- `aux_decode2.py`: interpolated edge timing (was ±0.25 µs quantized =
  half a Manchester half-cell); NEW anchor-based frame parser — finds
  each frame's SYNC-END (first ≥2-run pair after a stretch of length-1
  runs) and derives polarity from the anchor itself, instead of
  pair-parsing the whole burst with one global phase. Manchester
  convention corrected ('1' = second half-cell low); the old selftest
  passed only because its synthesizer had the same inversion.
- `aux_annotate.py`: request/reply direction comes from alternation
  state (headers overlap: ACK=0x00 collides with the I2C_WR nibble),
  with an exact-length native-cmd escape hatch for lost replies.

Internal consistency checks that certify the decode: the ESI block read
at 0x2003 (13 B) shows `77 00 01 03` at offsets 0x200C-0x200F — the
exact mirror of the separately-read 0x202-0x204 lane status; the EDID
blocks reassemble into a valid Sceptre F22 EDID ("Sceptre F22" string,
checksum-plausible structure, 0x02 0x03 CTA extension).

## Timeline (capture t=0 at record start; Mac plugged in ~t=0-2 s)

- **t=11.72 s**: first AUX transaction. Everything before is macOS
  display-stack latency — the >10 s to picture is the Mac's, not the
  link's. The DP conversation itself is FAST.
- 11.72-11.83 s: capability phase (~100 ms).
- 11.86-11.90 s: EDID (both blocks, ~50 ms).
- 11.91-11.95 s: link training — **complete in ~30 ms**.
- 12.4 s onward: steady-state polling + vendor writes; HDCP at 13.8 s.

## The conversation, phase by phase

### 1. Capabilities
- `RD 0x0000 len16` → `12 14 C2 81 01 1D 01 81 02 00 04 04 3E 00 81 00`
  - DPCD rev **1.2**, max rate **0x14 = HBR2**, `0xC2` = 2 lanes +
    TPS3 + **enhanced framing**; 0x00E byte = 0x81: extended-caps
    present, EQ interval 4 ms.
- `RD 0x2200 len16` (extended caps; first byte 0x14 = rev 1.4 there).
- `RD 0x0080 len4` (sink-specific caps), `RD 0x0500` branch OUI →
  `00 10 FA` + "cHDMIb" (DP→HDMI branch identity), hw/fw rev bytes.
- **`WR 0x0300` = `00 10 FA 41 41 50 4C 00 00 10 01 00`** — the Mac
  writes its SOURCE OUI: Apple + "AAPL" + rev. Read back at 0x300 to
  verify. Branch firmware can (and often does) key quirks off this.
- Reads of 0x310, 0x400/0x403 (sink OUI), 0x503, 0x090, 0x0B0, 0x060,
  0xE000D, HDCP2.2 RxCaps region (0x6921D), 0x2210/0x2214.

### 2. Stale-IRQ servicing BEFORE anything else (the big one)
At 11.857 s, before EDID and training:
- `RD 0x0200 len6` → `41 ...` (sink count 1 + CP_READY)
- `RD 0x0202 len3` → `00 00 80` — **0x204 bit7 LINK_STATUS_UPDATED set**
- `RD 0x2003 len13` (whole ESI0 block 0x2003-0x200F) → shows
  **0x2005 = 0x02** (link-service IRQ pending) and 0x200E = 0x80.
- **`WR 0x2005 = 02`** — write-1-to-clear, ACKed.
- Subsequent status polls read `77 00 01` — **the 0x80 latch is GONE**.

The Mac services sink IRQs through the **ESI space** (0x2003/0x2005),
not legacy 0x201 — 0x201 is never touched in the whole capture. And the
0x204-bit7 latch that appears in EVERY one of our wedge snapshots (Z:)
clears exactly when the ESI vector is acknowledged. This is the answer
key confirming the unserviced-IRQ diagnosis, with the addendum that the
reference implementation clears via ESI. Our 8acfe351 build clears
legacy 0x201; if bench shows 0x204 bit7 still sticking, the move is to
mirror the Mac: read 0x2002-0x200F as a block, write-1-clear 0x2004/0x2005.

### 3. EDID — the Anker does NOT defer forever
Pattern per 16-byte block, every block, metronomic:
`I2C_RD_MOT len16` → **DEFER** → retry ~0.4 ms → **DEFER** → retry
~0.6 ms → **ACK + 16 bytes**. Segment-pointer write 0x00 first, address
write 0x80 for the extension block, closing `I2C_RD` (len 0, MOT end).
Both 128-byte blocks read in ~50 ms total.

This kills the "converters DEFER all DDC forever" belief (that
observation likely came from a monitor-absent or wedged state): with a
monitor present the Anker serves EDID after exactly two DEFERs per
transaction. A tolerant EDID read = retry on DEFER with ~0.4 ms spacing,
budget ~3-5 tries per block.

### 4. Power + link training (30 ms, first try, no ladder)
- `RD 0x600` → 01, `WR 0x600 = 01` (D0).
- `WR 0x107 = 00` (no downspread), `WR 0x100 = 0x14` (**HBR2**),
  `WR 0x101 = 02` then **`= 82`** (2 lanes + ENHANCED_FRAME_EN,
  read-back between writes).
- `WR 0x102 = 21` (TPS1 + scrambling disable) → `RD 0x202 len6` →
  CR done `11` per lane in ONE poll → `WR 0x103 = 01 01` (lane set) →
  `WR 0x102` = EQ pattern → `RD 0x202 len6` → `77 00 81 ...` EQ+align
  done → `WR 0x102 = 00` (training off).
- `RD 0x210 len8` (symbol error counters) → clean.

Notable: status reads during training use len6 (0x202-0x207 in one
transaction — status AND adjust requests together), and the pre-training
status read tolerated the regressed `00 00 80` without drama.

### 5. Steady state
Polling cluster roughly every 0.3-1 s (relaxing to ~2.8 s):
- `RD 0x600` (power), `RD 0x200 len6` (sink count + status block),
  `RD 0x202 len3`.
- **`WR 0x3050 = 01`** every cycle, plus `RD/WR 0x3051` (= 02 then 06
  pre-training, re-confirmed 06 afterwards): 0x3000-0x30FF is branch
  vendor space — a VIA/ITE vendor handshake, possibly a keepalive.
  Candidate explanation for why known-good sources never see the hub
  MCU freeze. Experiment-worthy, clearly labeled cargo-cult risk.
- Second ESI service cycle at 16.66 s (RD 0x2003 len13, WR 0x2005=02).
- HDCP 1.x auth at 13.8 s (An/Aksv writes, Bksv read, R0' read) —
  optional for us; picture does not depend on it here.

## Conformance gaps ranked (ours vs the answer key)

1. **IRQ servicing via ESI** — Mac reads the ESI block and
   write-1-clears 0x2005; never touches 0x201. Our IRQ_SERVICE=1 build
   (8acfe351) uses legacy 0x201 — bench will show whether the IT6563
   honors the legacy path (watch whether 0x204 bit7 clears after a
   J: irq_service). If not: switch clear target to 0x2004/0x2005.
2. **Source OUI write (0x300)** — we never announce ourselves; the Mac
   writes AAPL. Cheap to add; branch quirk-gating is real.
3. **EDID with DEFER-tolerant retry** — 2 DEFERs then ACK is the hub's
   documented-by-capture behavior; our blanket skip forfeits mode info
   and diverges from what the branch expects a source to do.
4. **Status reads as multi-byte blocks** (0x200 len6 / 0x202 len6) —
   we read registers singly; blocks are atomic snapshots (status +
   adjust in one transaction) and fewer AUX round-trips.
5. **Vendor 0x3050/0x3051 writes** — unknown semantics; note for a
   controlled experiment only after 1-4 are graded.
6. Enhanced framing (`0x101 = 0x82`) — verify what we set.

## Pass A2: HDMI unplug → replug (captured same session)

`mac_aux_hdmireplug.csv` → `mac_anker_hdmireplug_annotated.txt`. HDMI
was pulled before/at record start (AUX silent 0-5 s — the Mac had
already torn down on the virtualized HPD drop); replug at ~5.03 s.

- **HDMI replug = a full fresh hot-plug to the source.** The hub
  virtualizes the IT6563's DP-HPD re-assert into a complete HPD cycle,
  and the Mac re-runs its ENTIRE bring-up script from `RD 0x0000` —
  byte-identical caps phase, EDID with the same 2-DEFER cadence,
  training first-try. No incremental retrain path, no IRQ-driven
  recovery: attach handling IS the recovery mechanism.
- **Warm re-attach is fast**: first AUX → training complete in ~230 ms;
  HDCP done ~2.2 s. The 10+ s cold-plug delay is macOS display-stack
  latency, confirmed.
- **The `WR 0x2005 = 02` is UNCONDITIONAL.** Here the ESI block read
  showed 0x2005 = 00 (no IRQ pending) and the Mac wrote the clear
  anyway. It's a scripted ack in the attach sequence, not a
  conditional response to a pending vector.
- **Branch DPCD state survives the HDMI cycle**: LANE_COUNT_SET read
  back 0x82 (previous session's value) before being rewritten; 0x3051
  already 06. The hub does not reset its DPCD on HDMI unplug — matches
  the stale-but-served physiology (autonomous AUX hardware, MCU-owned
  state).

### Correction to §2 (both captures agree)
The 0x204 bit7 LINK_STATUS_UPDATED latch does NOT clear at the ESI ack:
it still reads 0x80 during training (re-set by every status change:
00→11→77) and only reads clear in steady state after training completes
— consistent with clear-on-read re-armed by changes. So the latch in
our wedge snapshots is a *status engine frozen mid-change* signature,
not directly an unserviced-IRQ flag. The ESI ack write remains part of
the reference attach script and IRQ servicing remains conformance-
required (capture 1's 0x2005=02 pending bit was real), but "clearing
0x204 bit7" is not the observable to grade IRQ servicing by.

## Pass C: OUR board's bring-up (build 8acfe351, same session)

`our_board_bringup.csv` → `our_board_bringup_annotated.txt`. Breakout
inline board↔hub, board hot-attached by seating the connector (hub
backfeed powers it). Screen truth: **fast colorbars, first attach**.
2389 frames.

Timeline (attach at t=4598.6 ms):
- t+0: `RD 0x000 len12`, `WR 0x600=01`, `WR 0x108=01`, `WR 0x100=0A`
  (HBR), `WR 0x107=00`, `WR 0x101=02`, `WR 0x102=21` (TPS1),
  `WR 0x103 = 06 06 06 06` — **training starts in the same
  millisecond as the first AUX transaction**.
- t+0 → t+257 ms: CR poll storm. **383 iterations** of
  {`RD 0x200 len8` → `40 00 00 00 80 00 22 00`, `RD 0x206 len2`,
  `WR 0x103 = 06 06 06 06`} at ~650 µs per loop — ~1150 transactions.
  Note the block: **SINK_COUNT = 0x40 (count 0, no sink!)**, CR bits
  0x00 the whole time, adjust request steady 0x22 (VS2, honored).
- t+232 ms (4830.7): hub **DEFERs** a read — its MCU is busy.
- t+257 ms (4856.1): status flips in ONE poll: CR `11`, then
  `WR 0x102=22` (TPS2), one poll later EQ+align done (`77`/`81`),
  `WR 0x102=00`. Training completes in ~1.1 ms once the hub is ready.
  SINK_COUNT flips 0x40→0x41 at the same moment.
- Steady state: `41 00 77 00 01 03 00 00` (0x204=01, 0x205=03), one
  `81` blip when the stream started. 0x201 read every poll (the len8
  block covers it): 0x00 throughout — no IRQ fired this attach, so the
  IRQ_SERVICE path stayed idle (J: would show 0), as expected.

### The diff that matters

The hub was NOT ready at HPD: it raised hot-plug, then spent ~257 ms
finishing its own downstream attach (IT6563 HDMI hot-plug + EDID from
the monitor) while reporting SINK_COUNT=0 and CR=0. In that window:

- **The Mac** spends ~200 ms on capabilities, OUI, vendor reads, and
  EDID (~40 polite transactions), and by the time it writes its first
  training register the hub is ready — training passes on the first
  poll. It never trains against a 0-sink branch.
- **We** start training at t+0 and hammer ~1150 transactions into the
  busy hub MCU, rewriting TRAINING_LANE0_SET 383 times with an
  unchanged value (the Mac writes lane settings ONCE per phase).

It worked this time (fast colorbars), but this is the strongest
induced-wedge candidate yet: our peak-rate AUX hammering lands exactly
in the window where the VL103/IT6563 firmware is doing its attach
housekeeping — and the hub visibly DEFERred under it. The round-1
result (interruption-heavy legacy policy wedging least) is consistent:
teardowns break up the hammering.

### Fix list from the diff (priority order)
1. **Wait for the branch to be ready before training**: after HPD,
   poll 0x200 gently (≈10 ms period) until SINK_COUNT count-field ≥ 1,
   THEN start the ladder. Cheap, spec-aligned, directly detunes the
   hammer from the vulnerable window.
2. **Write 0x103 only when the adjust request changes** (or once per
   pattern phase) instead of every poll; slow the CR poll loop toward
   ~1 ms.
3. Enhanced framing: `0x101 = 0x82` (branch advertises the cap; the
   Mac sets it).
4. Source OUI write + tolerant EDID read (per §1/§3), giving the hub
   the same grace period the Mac gives it as a side effect.

Minor/unverified: a few stray frames (3x `I2C_WR I2C[00]`,
`RD 0x1000 len5`, a mid-training `RD 0x000 len1`) don't match ladder
states and may be decode artifacts of record-loss gaps — not
load-bearing, flagged only.


### Grading rows for 8acfe351 (same session, AD3 rigged)
1. Pass C attach: FAST colorbars. Hub ready-lag 257 ms (count 0
   throughout), one-shot train at ready-flip.
2. Cycle: FAST. Telemetry clean (Y:11, L:01, G fails 0, T:0000,
   Z/B/J all zero). Capture deleted (clean).
3. Cycle: FAST with one post-lock dark blip, self-recovered. Captured
   and decoded: TWO TPS1 writes (first ladder attempt tore down against
   the not-ready hub — the G:+1 fail / T:0001 tick), hub ready-lag
   ~435 ms this time with SINK_COUNT flipping to 1 a full ~277 ms
   BEFORE CR became achievable — so a sink-count gate alone is NOT a
   sufficient readiness test (fix 1 revised: the Mac-style polite
   caps/EDID preamble is the robust form). Post-lock: metronomic 1 s
   polls, all healthy (77/01/03), no retrain, no IRQ — the dark blip
   had NO DP-link or AUX signature at all (Y:11): it was downstream
   (IT6563 HDMI side or monitor re-sync). CSV deleted after analysis.
4. Cycle: FAST. Clean telemetry (Y:11, L:01, G fails 0, T:0000).
5. Cycle: FAST. One silent ladder restart (G:F1, T:0001, same
   not-ready-window signature as row 3) that never reached the screen.

**8acfe351 n=5 result: 5 fast / 0 delayed / 0 dark** (block-2 legacy
baseline: 5/3/0). The delayed class did not appear — consistent with
the FSM-race fix (periodic-check advance no longer coin-flips), though
the session-swing caveat stands (same build+procedure has swung between
sessions before; cross-session comparisons are weak). The IRQ_SERVICE
path has NOT yet been exercised (J:000000 every cycle — no HPD IRQ
fired during clean attaches); the scenario that generates one is an
HDMI unplug/replug while the board session is live, which is also the
quiet-frozen wedge trigger. That is the next bench test.

### Live HDMI unplug->replug on 8acfe351 (n=1, wire + telemetry)
Screen: video dropped and came back. Y:22/L:02 (one clean re-rise),
G:F3 (+2 ladder restarts), B: first-teardown latched, C:0177 healthy
after. Wire anatomy: unplug -> one poll catches 40-count with link
still 77 -> board falls into its polite 20 ms len1 sink-wait loop (no
hammering; good existing behavior); replug -> count 41 -> ladder
attempt 1 spins CR ~270 ms against the not-ready hub and tears down ->
attempt 2 locks; replug-to-lock ~550 ms; healthy 1 s polls after.
(History note: the 2/2 dark-storm stat belongs to the SLEEPING-MONITOR
flap scenario, not this live-monitor replug — powered replug was
historically the recovery operation. This row says the live-monitor
unplug/replug cycle is handled cleanly, nothing more.)

**DEFINITIVE: the hub NEVER sets legacy 0x201.** Zero nonzero reads
across ~5000 frames spanning attach, unplug, and replug (and the Mac
never touched 0x201 either — ESI only). The 0x201-based IRQ_SERVICE
path is therefore INERT on this hub (J:000000 in every test, hpd_irq
never coincided with a pending vector). Conformance parity requires
the ESI form: periodic + IRQ-triggered read of 0x2002-0x200F as one
block, write-1-clear of 0x2004/0x2005. Fold into the polite-attach
build (sim-first) alongside the caps/EDID preamble, write-on-change
0x103, and enhanced framing.

### Grading rows for f0a48ae6 (polite-attach v2: per-iteration writes)
Predecessor 7d6e205d (write-on-change skip) NEVER trained — hardware
refutation documented at LANE_SET_WOC in aux_channel.v.
1. Cycle: FAST. Cleanest signature of any build: G:F0 (ZERO ladder
   fails — the preamble absorbed the not-ready window that cost
   8acfe351 restarts on 2/5 cycles), T:0000, Y:11, L:01, C:0177, K:03.
   J:000002 — the attach ESI ack ran twice: the free-running 0.5 s
   watchdog likely bounced the preamble once (invisible to gate/timeout
   counters pre-training). Benign; noted as a candidate refinement
   (longer pre-establishment watchdog budget for polite builds).
2. Cycle: DARK — NEW SUBCLASS: dark with FULLY healthy DP + status.
   D:2E, C:0177, Y:11 (video streaming), and K:03 — the hub's
   SINK_STATUS claims it IS receiving the stream on both ports. Not the
   quiet-frozen wedge (that is C:8177/K:00). Failure is entirely
   downstream of the DPCD (IT6563 HDMI output side / monitor sync);
   no self-recovery after ~15 s wait; HDMI replug at the hub recovered
   immediately. Replug re-attach: Y:22, L:02, G:F0 STILL zero restarts
   (8acfe351's replug cost 2), J: 2->5 (re-attach acks + runtime
   service). AUX conformance work cannot reach this class by
   construction — the DPCD shows nothing wrong to service. If it
   recurs at rate, the lever is HDMI-side (or the Mac's vendor
   0x3050/51 per-poll writes, still unexplored).
3. Cycle: FAST, ideal signature — J:000001 (single-pass preamble; row
   1's J:2 confirmed as the watchdog-bounce case, not a bug).
4. Cycle: FAST, ideal signature (identical).
5. Cycle: FAST, ideal signature (identical).

**f0a48ae6 n=5: 4 fast / 0 delayed / 1 dark-downstream.** The headline
is G:F0 on EVERY row including the HDMI-replug recovery: ZERO ladder
restarts across the whole series (8acfe351: 2 restarts in 5 cycles;
legacy block-2: a visible delayed class). The polite preamble fully
absorbs the hub's not-ready window on this bench. The one failure was
the dark-with-K:03 downstream class that the DPCD interface cannot see
by construction. Bench run WITHOUT the AD3 breakout inline (captures
possible in a later pass if an anomaly needs the wire).
Before production-candidate status: (a) a Ugreen smoke test — the EDID
give-up budget is the strict-converter fallback and is sim-proven but
not hardware-proven; (b) optional round-2 for session-swing.

### Ugreen smoke test (f0a48ae6, 3 cycles)
3/3 FAST colorbars, identical clean signatures every row: G:F0 (zero
ladder restarts), Y:11, L:01, C:0177, K:03, T:0000, books empty. The
historically strict converter (0-for-everything at 900 mV; EDID-hostile)
takes the polite build cleanly at 804 mV. Fingerprint: J:000002 on ALL
THREE rows (vs J:1 on Anker rows 3-5) — a DETERMINISTIC single
pre-training restart on this converter (invisible to gate/timeout
counters), benign. Caveat recorded honestly: telemetry cannot
distinguish EDID-served from EDID-given-up (no counter exposes
edid_giveup); the load-bearing property — EDID handling never blocks
video on a strict converter — held 3/3. Definitive answer = one
breakout capture, or expose defer_cnt in a spare telemetry nibble.

### Bar for production (user directive, 08-26)
80% bench success is NOT acceptable — field rates degrade from bench
rates. The dark-with-K:03 downstream class is an OPEN INVESTIGATION,
not an accepted loss. Candidate levers: (a) more n to establish its
true rate (it may be the self-recovering benign class caught in a
non-recovering instance); (b) the Mac's vendor 0x3050/0x3051 per-poll
handshake (controlled experiment); (c) IT6563 HDMI-side behavior
(datasheet review of output-side hot-plug/timeout machinery).

### FIRST IN-THE-WILD RUNTIME ESI SERVICE (Ugreen, continued testing)
User's 5th Ugreen cycle: colorbars -> dark blink -> colorbars,
self-recovered. Telemetry: Y:11 (link NEVER dropped through the blink),
G:F0/T:0000/B:0 (zero teardowns), and **J:000003 vs the deterministic
J:2 Ugreen baseline — the +1 is the runtime ESI service firing**: the
converter raised an HPD IRQ during the blink, the ESP32 chain delivered
it, and the build serviced it Mac-style (status check -> RD 0x2003 ->
W1C 0x2005) with the link held. Video recovered WITHOUT intervention.
Contrast the Anker dark-with-K:03 row: no IRQ offered (J unchanged),
no recovery until manual HDMI replug. n=1, correlation not proof — but
this is the conformance fix executing its designed scenario end-to-end
on hardware for the first time (every prior build left this IRQ
dangling). Instances 2 and 3 followed at the same fingerprint (J:3,
Y:11, zero teardowns, self-recovered every time) — the serviced-blink
class is a stable, reproducible mechanism: exactly one IRQ, one
service, full recovery, 3/3. Five further cycles all fast (no blinks).
Ugreen running total: 15 fast + 3 self-recovered serviced blinks over
18 cycles (blink incidence ~17%; recovery 3/3; 0 dark, 0 delayed —
100% screen success on the historically 0-for-everything converter).

Anker IRQ-silence hypothesis space (ed, 08-26): the dark-with-K:03
non-recovery could be (a) the hub never SENDS an attention/IRQ, or
(b) it sends one we fail to see or act on. Candidate gap found on
review for (b): our runtime ESI service reads the 0x2003 len13 block
but latches/clears ONLY 0x2005 — if the Anker's dark event raises
bits in 0x2003 (DEVICE_SERVICE_IRQ_VECTOR_ESI0) we read them and
leave them SET, exactly the unserviced-vector pathology in miniature.
(The Mac capture never showed 0x2003 nonzero, so this is untested
territory, but W1C of 0x2003 is cheap conformance completeness.)
Discrimination plan for (a)-vs-(b): ESP32-side attention/hpd-pulse
counter readable over telnet (did the chain deliver?), a spare-nibble
telemetry build exposing the latched ESI bytes + defer_cnt/giveup,
and — definitive — an AUX capture during an Anker dark event.

### Polite v3 (a03b7e73: + W1C 0x2003, + V:/U: telemetry) — first read
First Ugreen attach answered BOTH observability questions:
- **U:08 — the Ugreen SERVES EDID**: one DEFER per 16-byte block then
  ACK (8 defers / 8 blocks; give-up budget untouched). "Converters
  defer DDC forever" now refuted on BOTH bench converters — the Anker
  defers 2x/block, the Ugreen 1x/block.
- **V:0002 — a REAL pending link-service IRQ (0x2005=0x02) at attach**,
  cleared by our unconditional Mac-parity ack: that behavior is
  load-bearing, not ritual. 0x2003 carried nothing (second-clear
  correctly silent). Row otherwise ideal: fast, G:F0, Y:11, L:01, J:2
  Ugreen fingerprint.
Rows 2-8: all FAST, byte-identical signatures (V:0002 / U:08 / J:2 /
G:F0 every row — the Ugreen attach is deterministic down to the vector
contents and defer count). **v3 Ugreen: 8/8 fast**, no blinks offered
yet (~17% historical rate; within odds).
Standing watch items: V: after a serviced blink (runtime vector
contents), and V:/U: on the Anker — especially at a dark event.

### QUIET-FROZEN WEDGE ON v3 (Anker, first swap-back attach) — INSTRUMENTED
Light-up then dark, stayed dark. Telemetry: the ORIGINAL wedge signature
C:8177/K:00 (LINK_STATUS_UPDATED latched, lanes trained, hub claims
not-receiving), Y:17/L:07 (seven video flaps then dark), G:00/T:0/B:0
(no gate fails, no teardowns — the polite FSM polling politely into a
wedged hub), W:6. **J:000001 — NO runtime IRQ ever arrived** through
the whole event: the VL103 delivered no attention. The polite stack
does NOT prevent this class (as suspected: it is hub-internal).
INSTRUMENT LIMITATION EXPOSED: V: showed only the attach read because
the ESI block is read ONLY on hpd_irq — a standing vector the hub set
without delivering an attention would be invisible. NEXT BUILD: fold an
ESI block read into the 1 s check so V: records standing vectors.

**RECOVERY: PD-level virtual replug ('v') CLEARED the wedge (n=1).**
Board drained/rebooted/re-attached clean (fresh counters, Y:11, C:0177,
K:03; V:0000 — no pending vector at this Anker attach, unlike the
Ugreen's every-attach 02). Implication: 'v' is ESP32-initiated, so an
AUTOMATED self-heal path exists — FPGA detects sustained C:8177/K:00,
signals ESP32, ESP32 executes virtual replug. Historically manual power
cycles did NOT reliably clear this wedge (it recurred), so this needs
more n — but it is the first credible field-rate answer for the class
the source cannot prevent. Design constraints if built: budgeted,
flap-tolerant (the sleeping-monitor storm lesson), screen-truth-gated.

### Anker v3 n=5
1 wedge (the swap-back first attach, above) + 4 FAST — every clean
attach byte-identical (G:F0/Y:11/L:01/J:1/V:0002/U:08). V:0002 now
consistent on Anker clean attaches (pending 0x2005 at attach, acked).
U:08 = the Anker defers our EDID only ONCE per block, vs the 2-defer
cadence it showed the Mac — defer depth is load/state dependent, not a
fixed hub property.

### DETECTOR VALIDATED ON LIVE WEDGE (v5 7cb54bda, first attach)
Quiet-frozen wedge on the first v5 attach (C:8177/K:00, Y:18/L:08 eight
flaps, ESI silent again, J:1 no attention) — **U:88: wedge_suspect
FIRED** (~11 s window; the ever_streamed-gate fix did its job — this
same profile false-negatived on v4). Clean attaches read U:08 (bit7
clear): live discrimination proven in BOTH directions, n=1 each.
Recovery: 'v' again (n=3; fast). Remaining false-positive unknowns
before arming auto-'v': the kick-era K:00-with-good-picture state (if
it also freezes bit7) — unobserved so far; policy will be budgeted
(<=3, cooldown, re-arm on streaming) so worst-case exposure is bounded
screen blackouts, not a storm. NEXT: ESP32 policy patch — parse U: bit7
from the relayed UART telemetry (or wire the wedge_suspect port), 
trigger budgeted virtual replug.

### SELF-HEALING VALIDATED END-TO-END (first live opportunity)
ESP32 wedge_watch flashed; board swapped Mac->hub; the attach WEDGED
(light-up then dark) and ~30 s later the monitor CAME BACK with zero
human action. Receipts: fresh counters (the replug rebooted the board),
clean healthy session (C:0177/K:03/G:F0), and the console line
"AUTOREC: STREAMING STABLE - BUDGET RE-ARMED" — the budget was consumed
(fire persisted to NVS pre-replug), survived the reboot, and was
re-armed to 0/3 by 20 consecutive K:03 lines. Every policy element
(detect -> fire -> persist -> recover -> verify-streaming -> re-arm)
demonstrated in one pass. Telnet 'w' toggle verified (DISABLED/ENABLED
with budget readout). The quiet-frozen wedge — the class the source
cannot prevent and the hub signals nothing about — is now a ~30 s
self-recovering blink instead of a dead display.

### Post-self-heal plain-cycle block (v5 + wedge_watch, Anker) + taxonomy
6 plain power cycles: 4 clean + 2 six-second dark blinks, ZERO
quiet-frozen wedges. Both blinks: Y:11/L:01 (our video NEVER dropped),
C:0177/K:03 healthy throughout, no IRQ, no flag — the benign DOWNSTREAM
BLINK class (IT6563 HDMI side / monitor resync), self-recovering, and
the detector's K:00 gate correctly ignored both (no spurious replug —
the false-positive discipline working live). Anker blink rate tonight
~1/3, resembling the Ugreen's ~1/5: plausibly the same converter habit.

IDLE-GAP HYPOTHESIS (ed's observation, refined): all THREE true
quiet-frozen wedges today were the FIRST attach after the hub sat
SOURCE-LESS (post-flash / post-swap); plain cycles have produced none.
The Mac's capture-1 attach into an idle hub worked — so the idle-hub
state is survivable with the right attach, moving the OUI/vendor-
handshake experiment up to PRIME SUSPECT for wedge prevention.
Pending experiments: (a) 5-minute source-less idle then attach =
on-demand wedge generator if the hypothesis holds; (b) detection-time
cut 10.7s -> 8.05s (two-bit AND, guards the ~7s settling-storm ghost)
+ consec 3->2: recovery ~30s -> ~18-20s bench / ~12s production
(ed: 30s is too long).

### WEDGE ROOT CAUSE FOUND: ESP32 SURVIVES THE SWAP GAP (ping-instrumented)
Generator refined then root-caused in one instrumented run (continuous
1 Hz ping watch + screen truth), timeline:
- 12:05:59 down: unplug from hub -> Mac dwell (~30 s suffices; the 5-min
  idle test was CLEAN — sourceless idle is NOT the generator).
- 12:07:00 down / **12:07:09 up: a 9-SECOND hole including cable
  handling — the ESP32 NEVER REBOOTED across the Mac->hub swap** (a real
  reboot costs 30-60 s of WiFi rejoin, measured all day). Bulk caps
  float the chip through the 1-3 s unpowered gap (ed's call).
- Screen: lit then WEDGED (generator now 5/5 for Mac-dwell+quick-swap;
  0/7 plain cycles; 0/1 five-minute drain).
- 12:07:39 down / 12:07:42 up: **a 3-second hole = the virtual replug's
  VBUS-off hold — the ESP32 rides through its OWN replug too** (which is
  why telnet always answered quickly post-recovery while FPGA counters
  zeroed: the FPGA's POR trips on the sag, the ESP32's does not).
- Colorbars ~32 s after attach: THIRD live self-heal, exact budget.

MECHANISM: the swap-attach happens with NO fresh boot — no
fusb302_init (the chip software reset lives there), the PD stack
running on Mac-session RAM state, and plausibly phantom-VBUS (the
measured backfeed) hiding the detach from the stack entirely, so the
hub arrives mid-stale-session. Mixed-survival hazard: the FUSB302 may
brown out to POR defaults while the ESP32 does not — a blind, desynced
stack attaching. WHY 'v' CURES IT: usbc_port_virtual_replug is a
CONTROLLED teardown — fusb302_disable + reset_protocol + from-scratch
attach ceremony. The cure is not the power cycle; it is the PD-layer
clean slate. Cure == prevention: run the same teardown-reinit ceremony
whenever a source transition is detected with prior-session state
(field-proven 5+ times tonight as the recovery path).
NEXT BUILD (ESP32): detach-detection hardening + ceremony-on-
reattach — the wedge class should stop occurring at attach entirely.

### Stale-session guard: first acceptance runs (honest ledger)
Guard flashed. Run 1 (normal-speed swap): NO WEDGE — but the 45 s WiFi
outage + empty console backlog show the ESP32 actually REBOOTED (gap ran
long): clean-by-fresh-boot, guard not exercised. Run 2 (FAST swap):
lit-and-wedged — the guard MISSED, and the miss identifies its blind
spot: if the surviving FUSB302 is configured for the MAC session's CC
orientation/measure path, the hub's Source_Caps never physically arrive
(no RX -> Trigger B can never fire) and the chip is intact (Trigger A
silent). THE NO-RX HOLE. The wedge_watch backstop fired and recovered
(~30 s) — backstop now 4/4 lifetime. Every swap outcome converges on
colorbars: fresh boot / guard / backstop.

TRIGGER C (designed, next firmware build): 1 Hz CC-orientation
re-qualification while attached — reuse fusb302_detect_source_
orientation; if Rp moved to the other CC (new cable orientation) or Rp
absent on both (missed detach, phantom-VBUS-proof) -> ceremony. Run
only on idle bus (no recent RX) so the measure-block flip cannot clip a
PD message; restore configure_sink(polarity) after. This also replaces
VBUS-based detach detection as the primary — CC is the ground truth.

### Autonomous work block (ed away): Trigger C + latency trims, sim-proven
Built while the bench was idle; NOTHING flashed yet.
1. **Trigger C shipped in firmware source** (host-sim 14/14):
   fusb302_requalify_cc measures both CC pins mid-session and restores
   ONLY SWITCHES0 (design review caught that configure_sink would have
   silently killed rx_enable + the DR-swapped role). 1 Hz on the guard
   tick, sink-role + idle-bus (>100 ms since RX) gated, 2-sample
   debounce. Rp-moved -> ceremony (budgeted); Rp-absent-on-both ->
   clean enter_unattached (unbudgeted; CC ground truth replaces the
   phantom-VBUS-blind detach detection for this case).
2. **Recovery-latency trims**: FPGA detector 10.7 s -> 8.05 s
   (two-bit AND, comparator-free; floor set by the ~7 s settling-storm
   ghost); wedge_watch 3->2 consecutive lines; auto-fire hold 3 s ->
   1.5 s (usbc_virtual_replug_fast; manual 'v' keeps 3 s). Recovery
   ~30 s -> ~21 s bench / ~13 s slot-powered.

NEXT BENCH SESSION — flash list + acceptance:
- FPGA 138B **67c40cc5** (8.05 s detector via timer PRELOAD — the >=
  comparator and two-bit AND forms both re-rolled the vendor cm_life
  placement into violations across three Place_Options; the preload
  keeps fire logic gate-identical to v5 and closed clean at 100.156).
- ESP32 firmware (Trigger C + trims).
- Acceptance: fast-swap generator n>=3. Expect: guard ceremony
  handles it in ~5 s (console: "CC orientation moved — ceremony n/3"
  or Source_Caps line), no wedge; if the guard is somehow evaded, the
  backstop lands at ~21 s. Plain cycles + Ugreen smoke unchanged.

### SESSION CLOSE: scope determination + unified wedge theory
Final baseline (extended): 15 plain cycles on the full v6 stack —
Anker->Sceptre 10 (9 fast + 1 six-second self-recovered downstream
blink, the known benign class), Anker->FANGOR 5/5 fast (first Fangor
data on the new stack: the historically cable-flaky monitor runs
clean). Original close-out run: 5/5 fast-colorbars plain cycles
(FPGA 67c40cc5 + ESP32 w/ Trigger C @4Hz + trims). The remaining wedge
generator is BENCH-ONLY: Mac-dwell -> fast cross-bench swap into a
continuously-powered hub. Same-session fast replug (warm hub): CLEAN.
Cold-hub attaches: CLEAN (large n). Field boards live in a slot with a
permanent USB-C run — the generator's circumstance does not occur in
the field; field exposure = the rare organic quiet-frozen, covered by
the layered self-heal (prevent -> detect+recover <=30s -> manual).

UNIFIED THEORY (best fit, capture-unverified): wedge = a ride-through
attach carrying MAC-SESSION state into the warm hub. Same-session
stale = coherent -> hub treats as bounce; fresh boot = clean ceremony;
cold hub = fresh both sides; Mac-flavored stale = incoherent -> VL103
wedges pre-PD. The one apparent counterexample (boot banner in the
wedge run) is timing-ambiguous: the reboot may have occurred at the
BACKSTOP replug, not the swap. DEFINITIVE INSTRUMENT when curiosity
wins an evening: AD3 breakout capture of a wedging Mac-swap attach
(where does the hub's DP conversation die). Lesson logged: the
live-console-through-swap instrument fails SILENTLY (zombie TCP,
radio browns out before the core) — use ping transitions + post-hoc
ring reads (the boot banner is reliable ground truth).

### Pairing fingerprint: Ugreen x Fangor = deterministic first-sync blink
5/5 Ugreen->Fangor attaches: fast colorbars with a ~3 s blackout blip
before stability, EVERY attach. Telemetry: Y:11 (DP video continuous),
J:2 (Ugreen attach fingerprint, NO runtime IRQ), clean books — the
DP-invisible downstream class, here deterministic. Contrast: Ugreen->
Sceptre ~1-in-5 stochastic 6 s blinks; Anker->Fangor 5/5 with none.
Reading: the Ugreen's HDMI output sequencing (TMDS re-clock/re-assert
after the DP stream starts) x the Fangor's input sync = a first-sync
blink; the Sceptre usually re-locks fast enough to hide it, the Anker
sequences HDMI differently and avoids it. Downstream of DPCD, no
vector raised, nothing serviceable source-side. Cosmetic; logged as a
converter-x-monitor fingerprint, not a defect in our stack.

### 60K CAMPAIGN (late session): two real defects fixed, one regression scoped
1. **CSR-ROM miswire FOUND+FIXED** (bfb118ca): the shared include had
   become 138B-flavored; 60B builds replayed 138B CSR into the 60K quad
   (S:04 no-PLL). Now per-project modules; stale includes fail loudly.
   Every 60K bitstream since the 138B CSR fork had this defect — the
   condemned "bad" SOM deserves a retrial.
2. **AFE-less drive gap FOUND+FIXED** (f754114d): both dies' CSR bakes
   txlev 14 (~850 mV); the 138K's proven 804 mV comes from the runtime
   AFE rewriting txlev 13 every training start — the AFE-less 60B never
   got the cure. 804 now baked into the 60B ROM (808434/808534).
3. **REMAINING: a die-sensitive TX-datapath regression** in shared code
   between e81682ba and HEAD — now PROPERLY evidenced by a timed
   patience test (ed's catch: the first no-CR verdict used an
   observation window known to be too short for the polite ladder's
   retry cadence): f754114d (804 mV) given 4.5+ minutes of continuous
   30 s telemetry polls — ZERO CR, ZERO adjust requests across every
   attempt, where the old vintage at the same age was passing CR
   constantly and EQ-looping. Old-vintage 60B: CR + eventual colorbars
   (minutes, ~850 mV) — good SOM PROVEN FUNCTIONAL; current builds:
   hub never sees signal at 850 OR 804 mV. S: telemetry cannot discriminate
   (lane_ready bits read 00 even on the working 138K). TOP SUSPECT:
   symbol/bit-order layer — the 138B's regenerated SERDES IP may pack
   raw-mode bits differently, and a shared serializer/8b10b change from
   138B bring-up would garbage the 60B's symbols (PLL locked, hub
   deaf). NEXT SESSION: focused diff of the encoder/serializer path,
   else 4-5-step hardware bisect over e81682ba..HEAD (build+flash+cycle
   per step). Note: e81682ba-vintage rebuild = scratch worktree recipe;
   datetime.svh makes bit-identity impossible (sha differs, logically
   identical).

## 60K TX-regression bisect (Anker-controlled, one variable per step)

Protocol: scratch-worktree checkout per commit + overlay of the PURE
e81682ba-vintage shared ROM (357 entries, txlev E000) so the CSR config is
constant at the golden setup; AFE off on every step (60B die pkg); same
board, same CONFIRMED Anker hub, same monitor; patience window honored.
The rebuild pipeline is DETERMINISTIC (step-4 rebuild reproduced bin sha
977eab4a bit-identically — datetime is not in this bitstream), so bin
sha = commit identity.

- Step 1  e81682ba (vintage anchor, rebuilt 2b0aadd9): GOOD — CR passes,
  eventual colorbars. Golden reference re-proven on this board.
- Step 2  c403e98f (bin fe6b2e82): GOOD — CR passes, D:2A/2E cycling.
- Step 3  05f24edd (bin e43882fb): GOOD — CR passes. NOTE: this build
  already contains the disabled M5 machinery (afe_adjust_seq g_off
  tie-offs + 60B die pkg) => M5-present-but-off is hardware-exonerated.
- BAD     5474dee2 (bin 1bc3b709) and HEAD-era f754114d: A:0000, hub
  never sees signal (f754114d re-confirmed 4.5 min on the controlled
  Anker after the hub-identity confound was caught).
- Step 4  eaffc583 (bin 977eab4a, last pre-polite: IRQ-service v1 +
  FSM-race fix, legacy ladder): GOOD — FAST colorbars first cycle.
  Archived as impl/archive/a2mega_dp_test_60b_bisect4_eaffc583_977eab4a.bin.

- Step 5  798b7472 (bin 080af10c, polite v1 AS COMMITTED): BAD — no
  colorbars, A:0000/K:00, ladder cycling D:35/36 (EDID preamble states),
  E: aux errors, W:5. VERDICT: NOT a bisect data point for the die
  regression — polite v1 shipped with lane-set write-on-change ON, the
  defect hardware-refuted on the 138K the very next commit (7d6e205d
  "preamble ran, never trained"; IT6563 evaluates CR on the write). The
  60K reproduces the known WOC failure identically (a die-INsensitive
  confirmation). Step chosen in error; the first shipping-form polite
  commit is 5eba67f3 (WOC demoted to default 0) = step 6.

Boundary after step 4: eaffc583..5474dee2. Every commit between eaffc583
and 798b7472 is docs/tooling only (verified: git diff --stat on hdl/ is
empty), so 798b7472 (POLITE-ATTACH + ESI) is the FIRST RTL change in the
bad range. Step 5 = 798b7472 itself (SUPERSEDED — see step-5 row: WOC known-bad): GOOD
moves suspicion to the later ESI/detector/timing commits (5eba67f3,
dfa455ca, 2c28451e, 01a79ed7, 916ac191, c98cbbdf, 5474dee2).
- Step 6  5eba67f3 (bin f9802e2b, polite SHIPPING FORM: WOC=0,
  per-iteration lane-set, IRQ_SERVICE=2, POLITE_ATTACH=1): BAD —
  repeatable across multiple cycles. D3 lit/blinking, D4 dark, telemetry
  parked in EDID preamble states (D:35/36) with cycling aux errors,
  A:0000, K:00. This config graded 4/5-fast on the 138K.
  ==> POLITE-ATTACH CONVICTED as the die-sensitive regression boundary:
  eaffc583 (IRQ v1, no polite) = fast colorbars; 5eba67f3 (polite) = dead.

MECHANISM EVIDENCE (retro-decoded from the HEAD-era f754114d reads on
this board): V:0002 every row = ESI attach ack SUCCEEDS (AUX phy fine);
U: climbs 0x1D -> 0x28 (=40 = EDID_DEFER_CAP exactly) -> 0x69 (bit6
edid_giveup SET + defers past cap). The Anker DEFERS EDID TO THE CAP on
every attach on this board, vs ~2 defers/block on the 138K bench. After
give-up, training runs, CR never granted, teardown, re-attach, forever.
The old ladder never touches EDID (skips straight to patient CR retry) —
which is why every pre-polite step trains. Open question: EDID preamble
as poison (give-up abandoning I2C-over-AUX mid-MOT sours the hub) vs
symptom (hub not-ready the whole window; August-era ESP32 HPD timing on
this board is a suspect).

- Step 6b (queued): 5eba67f3 + EDID_DEFER_CAP=1 (one defer then clean
  give-up; presence gate + ESI + pacing kept). Trains => EDID preamble
  convicted specifically. Fails => presence/ESI timing implicated.

- Step 6b  5eba67f3 + EDID_DEFER_CAP=1 (bin 88855d3f): telemetry flips
  from A:0000 to A:0022/C:8000/D:15 — hub ENGAGES for the first time on
  any polite build — but 5-min patience = stable stall, K:00 throughout.

AD3 CAPTURE OF THE STEP-6b STALL (our60k_anker_attach.csv, 30 s, hub
power-up caught at t=24.36 s; 6799 frames annotated):
- The ladder runs a COMPLETE attach ceremony every ~25 ms, 225 times in
  the window: presence read (hub answers 0x41 READY instantly), EDID rd
  -> genuine hub DEFER -> cap-1 give-up, then the full config burst all
  ACKed (DPCD_REV read 12 14 C2 81..., ESI 0x2003 rd / 0x2005=02 W1C,
  SET_POWER=01, coding, BW=0A, lanes=2, TPS1 0x21, LANE_SET 06 06 06 06).
- Then EXACTLY 1.952 ms after lane-set: ONE status-read exchange whose
  reply is SHORT/GARBLED on the wire (parses as lone "ACK 07" / 1-byte /
  59-edge no-frame across rounds; clock_test expects a 9-byte reply),
  followed by 20.16 ms of silence (presence pacing) and ceremony restart.
- ZERO reads of 00202-region beyond that single attempt, ZERO 00206
  adjust reads, ZERO TPS2, zero NACKs, 783 EDID DEFERs, Y:00/L:00 (no
  teardowns, no HPD flaps — this is the ladder's own error-restart).
- 138K reference capture (our_board_bringup, pre-polite): ONE ceremony,
  then 400x 00200-region polls + 389x 00206 + 392 lane-set rewrites +
  3 pattern-set writes = a real training loop to lock.

MECHANISM (wire-proven at the round level): the first status read after
TPS1 gets a short/garbled reply; the reply fails the expected-length
check; the ladder ERROR path restarts the ENTIRE polite ceremony
(presence pacing + EDID + config, 25 ms) instead of retrying the read or
restarting cheaply at training. The hub never receives sustained TPS1.
Legacy ladder hit errors too but restarts AT TRAINING with TX up, so it
retried CR "constantly" and eventually locked (the historical minutes-
to-colorbars). The 138K never hits the path because its status replies
come back clean. Open sub-question (analog, still die/board-sensitive):
whether the short reply is the hub DEFER-ing/churning because die lane 3
gives it nothing to lock (the one parse that looked like status data
read lane0=CR+EQ+SYMLOCK, lane1=nothing), or our AUX RX clipping the
reply only when TX lanes are running (RX-squelch TODO interaction) —
config replies parse clean every round, so plain RX marginality is out.

FIX DIRECTIONS (sim-first, in order):
1. Status-read resilience in the CR/EQ loop: tolerate DEFER/short reply
   with a paced retry budget instead of ceremony-restart-on-error.
2. Cheap restart policy: CR-phase failures restart at clock_training
   (TX stays up), not at presence/EDID; the full ceremony is reserved
   for HPD/presence loss. (This alone restores legacy-grade persistence
   while keeping every polite conformance win.)
3. EDID session policy: on this board the hub defers EDID indefinitely
   — one EDID attempt budget per HPD session, skip on ladder restarts
   (cap=40 burning 26 ms x 8 blocks per ceremony was the original
   never-signal killer; cap=1 proved causality).
4. Separately: the analog question of die lane 3 signal quality vs the
   hub churn (Ugreen/804 mV matrix once training persists).

Suspects cleared by static/sim audit before step 5 flew:
- SERDES/PHY wiring: transceiver bank, 60B IP dir, die pkg, CST, SDC all
  byte-unchanged across the GOOD->BAD boundary; the only TX-path file
  change (dp_transmitter.sv +54/-2) is parameter/debug threading, no
  datapath logic.
- Sidecar provenance (ed's 138-contamination question): 60B .csr is NOT
  a 138 copy — lane-block histogram is die-correct (60B heavy on
  0x8084xx/0x8085xx = die lanes 2+3; 138B heavy on 0x8083xx/0x8084xx =
  lanes 1+2); 60B IP last touched by the WS4 1.1-emission restore
  (39bfea7e, 08-18), pre-dating the anchor. The one real contamination
  (shared replay ROM regenerated from the 138B csr at 6c4de392) is the
  already-fixed miswire, and the bisect overlay controls it out.
- Polite ladder logic with AFE off: tb_polite_attach_60b (POLITE=1,
  IRQ_SERVICE=2, AFE_ADJUST=0 — a combination never previously
  simulated or run on any die) PASSES all phases. If polite is the
  culprit it is a hardware-timing interaction (e.g. the PHY-powered-down
  preamble rhythm), not a logic bug.
- The prior "symbol/bit-order layer" top-suspect note above is
  SUPERSEDED by the bisect: steps 1-3 prove the current IP + bank +
  ROM train fine with the old ladder.


## Still wanted

- Pass B: CC1/CC2 (A5/B5) PD capture; needs a BMC decoder.
- Wedge-window capture: leave the AD3 rigged and capture an attach that
  goes delayed/dark — the transcript will show exactly what the hub
  serves during a failed window.
