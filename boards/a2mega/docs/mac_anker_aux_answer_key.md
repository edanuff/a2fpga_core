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

## Still wanted

- Pass B: CC1/CC2 (A5/B5) PD capture; needs a BMC decoder.
- Wedge-window capture: leave the AD3 rigged and capture an attach that
  goes delayed/dark — the transcript will show exactly what the hub
  serves during a failed window.
