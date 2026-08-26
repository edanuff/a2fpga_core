# Acquisition resiliency matrix — results

Build under test: **`f569d4f8`** (per-lane M5, `phase_done` gate, CDC
synchronizer fixes, acquisition counter v2). Protocol:
`acquisition_matrix_protocol.md`.

Cells: **A** Ugreen+Fangor · **B** Ugreen+Sceptre · **C** Anker+Fangor ·
**D** Anker+Sceptre

`L:` is the free-running D4 assertion count. Expected `L: = blinks + 1`.
**`L:` HIGHER than blinks+1 means post-lock dropouts** — the link came back
after losing sink-confirmed video, which is a stability finding distinct
from slow acquisition.

| # | cell | regime | blinks | L: | expected L: | post-lock drops | Y: | C: | A: | notes |
|---|------|--------|--------|----|-------------|-----------------|----|----|----|-------|
| 1 | A | **TRUE COLD** (multi-hour inert) | 4 | 06 | 05 | 1 (seen) | 66 | 0177 | 0022 | 4 blinks to colorbars, then lost colorbars momentarily and recovered. L: matched the user's count exactly here |
| 2 | B | warm-ish (regimen) | 5 | 08 | 06 | **+2 unseen** | 88 | 0177 | 0022 | User reported 5 blinks then SOLID — no visible dropout — yet L: counted 2 assertions beyond blinks+1 |
| 3 | C | warm-ish (regimen) | 5 | 08 | 06 | **+2 unseen** | 88 | 0177 | 0022 | IDENTICAL to sample 2 in every field, despite a different hub AND a different monitor |
| 4 | D | warm-ish (regimen) | **0** | 01 | 01 | 0 | 11 | 0177 | 0022 | Colorbars on the FIRST lightup — cleanest possible. ⚠ 4th cycle of the pass = warmest board; position/thermal confound is live |
| 5 | B *(pass 2)* | warm-ish (regimen) | 2 | 03 | 03 | 0 | 33 | 0177 | 0022 | **WITHIN-CELL REPEAT of sample 2 (5 blinks / L:08 / Y:88) — same hub, same monitor, same build, now 2 blinks / L:03 / Y:33** |

## Running observations

- ⚠️ **`L:` and eye-counting are NOT the same measurement.** Sample 1 agreed
  exactly (4 blinks + dropout = 6). Sample 2 diverged by +2: the user saw 5
  blinks then solid, `L:` read 8. `L:` counts EVERY D4 assertion, including
  ones too brief to perceive; the user counts VISIBLE flashes. Treat `L:` as
  a superset and more sensitive instrument — the earlier claim that it
  mirrors the blink count exactly is WITHDRAWN. Both numbers are recorded
  per sample; where they diverge, the gap is itself data (sub-perceptual
  assertions = marginal, briefly-held locks).
- Post-lock dropouts are visible as a separate phenomenon from
  blinks-to-lock. A build that locks fast but drops is not the same as one
  that locks slowly and holds.
- ❌ **Uniformity hypothesis (after samples 2-3) is DEAD.** Cell D came in at
  0 blinks / L:01 / Y:11 — dramatically cleaner than B and C. The behavior
  is NOT uniform across combinations, so it is not purely build-level.
- ⚠️ **Pass 1 cannot attribute it.** Cycle order was A,B,C,D and the board
  warms monotonically through a pass, so "cell D is clean" and "position 4
  is clean" are perfectly confounded. Sample 1 (A) is separately confounded
  as the TRUE COLD start. This is exactly what the rotated passes exist to
  break — pass 2 runs B,C,D,A and pass 3 runs C,D,A,B.
- 🔑 **WITHIN-CELL VARIANCE RIVALS BETWEEN-CELL VARIANCE.** Cell B measured 5
  blinks then 2 blinks on identical hardware and build. That spread is as
  large as the spread that separated the "good" and "bad" cells in pass 1.
  Consequence: **the hub/monitor combination is not established as the
  dominant factor**, and no cell-vs-cell claim from pass 1 survives. This
  is the same stochastic behavior as row 86 ({0,1,1,6,1} on one bitstream),
  now reproduced inside the matrix.
- 🌡️ **Weak warm-up signal worth watching (NOT established):** by cycle
  number the results are 4, 5, 5, 0, 2 — the first three cycles are the
  worst and the last two the best, consistent with a board that acquires
  better once warm. Confounded with cell identity; the remaining rotated
  passes are what would separate them.
- 📈 **`Y:` is an excellent difficulty proxy.** Across pass 1 it tracked the
  blink count monotonically: 4 blinks -> Y:66, 5 -> Y:88, 5 -> Y:88,
  0 -> Y:11. Three independent signals (eye, L:, Y:) agree on ordering.
- Y: tracks difficulty so far: sample 1 (4 blinks) Y:66, sample 2 (5 blinks)
  Y:88 — the battle counter rises with the blink count, a third independent
  signal agreeing that these were hard acquisitions.

## Fangor post-lock dropouts — our DP link is NOT involved (08-24)

Ugreen + Fangor, board power-cycled, user observed the picture dropping and
recovering **several times in a row**. Telemetry sampled 6 times over ~35 s
during the episode:

```
S:24 D:2E HLVC:111x Y:11 C:0177 L:01   (identical every sample)
```

- `L:01` never incremented → D4 (`video_live && sink_confirmed`) never
  deasserted, so our side never lost the sink.
- `HLVC` H/L/V all constant 1 → HPD present, link established, video live
  throughout. (The 4th digit is a clk100 liveness toggle and is SUPPOSED to
  alternate line-to-line.)
- `Y:11` → no battling. `C:0177` → golden training. `D:2E` → link
  established. `S:24` → PLL locked, PCS out of reset.

**Conclusion: the dropouts occur downstream of our DisplayPort link.** The
hub continued reporting both lanes CR+EQ+SYM locked while the picture
blanked repeatedly. No amount of link-training work on our side addresses
this.

⚠️ CAVEAT ON EARLIER READING: sample 1 (cell A) showed `L:06` vs an expected
`05` and that extra count was attributed to the observed dropout. If
dropouts do not move `L:` at all — as this episode shows — that attribution
was wrong and the +1 was something else.

**CONTROL RESULT (08-24): the Fangor drops with ANOTHER SOURCE DEVICE too —
likely a cable issue.** So the post-lock blanking is NOT ours, confirmed
from both directions: our telemetry showed a perfectly stable link during
the episodes, and the monitor misbehaves independently of our board.

**Consequences for the matrix:**
- Fangor cells (A and C) carry a KNOWN-BAD variable. Their post-lock
  dropout observations are void.
- Their blinks-to-lock numbers are *probably* still meaningful (acquisition
  happens before any downstream blanking) but are no longer clean data —
  a flaky HDMI link could plausibly disturb the hub's downstream state and
  feed back into re-attach behavior.
- **Recommended: replace the Fangor's HDMI cable and re-run cells A and C,
  or drop the Fangor from the matrix until the cable is ruled out.**
  Continuing to collect Fangor samples over a suspect cable would spend
  bench cycles generating data we would have to discard.

**Two distinct phenomena, now separated** (they had been treated as one all
session):
1. **Acquisition / blinks-to-lock** — ours, genuinely stochastic
   (row 86: {0,1,1,6,1} on one bitstream), still unexplained.
2. **Post-lock blanking on the Fangor** — not ours; monitor/cable.

## Build A/B — Anker + Sceptre only (08-24)

Cleanest cell: no known confound (the Fangor's cable is suspect). Board
warmed with 2 uncounted cycles before each block so both blocks are
measured warm.

**Counting convention pinned:** the user's number = the number of D4
lightups = `L:` exactly. ("1" = locked on the first lightup, `L:01`.)
Earlier matrix rows recorded as "0 blinks" mean the same thing as "1" here.

### Block 1 — `f569d4f8` (per-lane M5 + phase_done gate + CDC fixes)

| cycle | count (= L:) | telemetry |
|-------|--------------|-----------|
| 1 | 6 | |
| 2 | 1 | |
| 3 | 1 | |
| 4 | 6 | |
| 5 | 1 | `L:01 Y:11 C:0177 D:2E S:24 A:0022 M:12/2 N:020` |

**Distribution: {6, 1, 1, 6, 1}** — median 1, two outliers at 6.

🔎 **STRONGLY BIMODAL: the value is 1 or 6, never 2-5.** That is not
continuous noise — it points to a DISCRETE retry path of fixed length. The
link either acquires on the first attempt, or it enters something that
costs ~5 extra attempts before succeeding.

**Prime suspect: the retry watchdog in `dp_transmitter`** (`wdog_count`
[2:0], `wdog_force`, `debug_wdog = {forcing, attempts[2:0]}`). A
fixed-budget retry loop would produce exactly this shape.

⚠️ **INSTRUMENTATION GAP: `debug_wdog` is NOT in the telemetry message.**
Fields carry S/D/F/HLVC/P/E/R/A/G/Y/C/Q/K/X/M/M1/N/L — `K:` is DPCD 0x205
sink status and `X:` is the capability profile; neither is the watchdog.
Adding `debug_wdog` would test the bimodality hypothesis directly and is
the highest-value next instrumentation step after this A/B.

Note the similarity to `1f99bdb1`'s earlier manual data {0,1,1,6,1}
(row 86) — same bimodal shape, same values. If block 2 reproduces it, the
two builds are indistinguishable on this metric and the bimodality is
intrinsic to the design rather than to either build.

## 🔎 MECHANISM CANDIDATE: the auto-recovery watchdog (08-24)

Telemetry from a bad (6-count) cycle vs a good (1-count) cycle on
Anker+Sceptre, `f569d4f8`:

| field | good | bad | |
|---|---|---|---|
| `L:` | 01 | **07** | D4 assertions |
| `Y:` | 11 | **77** | link-establish / video-start odometer |
| `C:` | 0177 | 0177 | golden training — IDENTICAL |
| `D:` | 2E | 2E | link established — IDENTICAL |
| `S:` | 24 | 24 | PLL locked, PCS out of reset — IDENTICAL |
| `A:` `M:` `N:` | 0022 / 12,2 / 020 | 0022 / 12,2 / 020 | IDENTICAL |

**Only the counters differ. Every state field is identical.** So the bad
case is not "training struggles to converge" — the link **fully establishes
seven times** and is torn down six times before it sticks.

`dp_transmitter`'s AUTO-RECOVERY WATCHDOG matches this exactly:
- fires when `tx_link_established && (debug_sink[1:0] == 2'b00)` — i.e. the
  link is up but DPCD 0x205 reports no stream — after `WDOG_GRACE` (8 s);
- action is a genuine cold restart: por_n low + full CSR replay + bring-up;
- **`WDOG_CAP = 7`** attempts, re-armed on success.

Either it never fires (`Y:11`) or it runs its whole budget (`Y:77`) —
which IS the observed bimodality (1 or 6/7, never 2-5).

**Not yet proven.** The decisive evidence is whether the watchdog actually
fired, i.e. `debug_wdog = {forcing, attempts[2:0]}`, and that signal is NOT
in the telemetry message.

### ⚠️ Instrumentation bug found while chasing this

The telnet bridge **drops one character at each chunk boundary**. The
reassembled DP message is **104 printable chars; the source emits 106**.
The two missing characters land exactly at observed chunk boundaries —
after `K:` and after `R:`. Consequences:
- **`K:` is losing its low nibble**, which is exactly the `debug_sink[1:0]`
  the watchdog tests — so the sink-status route to confirming this
  hypothesis is unavailable over telnet;
- any field straddling a boundary silently loses digits in EVERY read.
  `Y:`, `L:`, `C:`, `A:` have parsed as full-width consistently, so the
  values used above are believed sound, but this deserves a fix.

### Next step (highest value)

Add `debug_wdog` to the telemetry message. It answers the question
directly — attempts > 0 on bad cycles and 0 on good ones would confirm the
watchdog as the mechanism — and it sidesteps the truncated `K:` field.
If confirmed, the fix is about the watchdog's trigger condition (why does a
working, streaming link report `SINK_STATUS[1:0] == 0`?), not about link
training at all.

## Watchdog REFUTED; teardown-from-established is the real mechanism (08-24)

Build `0bf0c6f5` added `W:` (`debug_wdog = {forcing, attempts[2:0]}`) and
`K2:` (DPCD 0x205 re-emitted past the telnet truncation).

| field | good cycle | bad cycle (6) |
|---|---|---|
| `W:` | **0** | **0** |
| `K2:` | 03 | 03 |
| `L:` | 01 | 08 |
| `Y:` | 11 | 88 |
| `G:` | **F0** | **F4** |
| `C:` `D:` `S:` `A:` `M:` `N:` | golden/identical | golden/identical |

**The auto-recovery watchdog is REFUTED: `W:0` on the bad cycle — it never
fired.** `WDOG_CAP = 7` matching `Y:77` was a coincidence and was
over-weighted. `K2:03` also shows the sink reported streaming throughout,
so the watchdog's trigger condition was never even close.

**What the data does say:** the link ENTERS `link_established` 8 times and
is torn down 7 times, with every state field golden each time. Teardown of
a working link — not a training convergence problem.

From `aux_channel.v`, only two paths tear down an established link:
1. **`check_wait` gate failure** — the periodic link re-check
   (`link_check_now` -> `check_link` -> `check_wait`) requires ALL of
   clock/equ/symbol/align locked; if any is clear it does
   `dbg_gate_fail++` and jumps to `error` (full retrain). The in-source
   comment records that this teardown/retrain is deliberate and
   load-bearing for Anker recovery.
2. **`channel_timeout`** — an AUX transaction timeout resets the FSM
   (`dbg_timeouts++`).
   (`retry_now` is NOT a path: `link_established` is explicitly excluded.)

`G:` moved from `F0` (good) to `F4` (bad) = `gate_fail` 0 -> 1, so path 1
fired at least once. But:

⚠️ **BOTH COUNTERS ARE 2 BITS AND WRAP.** `gate_fail = 01` could be 1 or 5;
`timeouts = 00` could be 0, 4 or 8. With 7 teardowns to account for, the
telemetry **cannot attribute them**. This is the blocker.

### Next step

Widen `dbg_gate_fail` and `dbg_timeouts` to 4-bit saturating counters (or
add wider mirrors) so the 7 teardowns can be split between "sink reported a
lane unlocked at the re-check" and "AUX transaction timed out". Those two
lead to completely different fixes:
- gate failures -> the sink's lane status is genuinely dropping, or the
  re-check is too strict / racing the sink's own status update;
- timeouts -> an AUX reliability problem, not a link problem at all.

## Teardown attribution attempt — INCONCLUSIVE, instrumentation contradicts itself (08-24 late)

Build `86cc4125` added `T:` = `{gate_fail_sat[3:0], timeout_sat[3:0]}`,
4-bit saturating twins of the 2-bit counters already in `G:`.

| | good cycle (1) | bad cycle (6) |
|---|---|---|
| `L:` / `Y:` | 01 / 11 | **09 / 99** (9 establishes, 8 teardowns) |
| `G:` | F0 | **F4** -> 2-bit gate_fail = 1 |
| `T:` | 0C | **0C** -> 4-bit gate_fail_sat = **0**, timeout_sat = 12 |
| `W:` / `K2:` | 0 / 03 | 0 / 03 |

⚠️ **THE TWO GATE_FAIL COUNTERS DISAGREE.** `G:`'s 2-bit counter reads 1;
the new 4-bit saturating twin reads 0. They increment on the SAME line, in
the SAME always block, with only one assignment site each (verified by
grep) — so they cannot legitimately differ. Stable across repeated samples,
so it is not a transient.

The timeout pair IS self-consistent (2-bit reads 0, saturating reads 12,
12 mod 4 = 0), so the `debug_teardown` bus and telemetry path work; only
the gate_fail half misbehaves.

**Attribution is therefore UNRESOLVED.** Do not read `T:0C` as "no gate
failures" — the counter is not trustworthy until this is explained.

Also learned: `timeout_sat = 12` on a CLEAN cycle, meaning AUX timeouts are
routine during the pre-lock ladder walk (expected with a DEFER-ing
converter). So the timeout counter as built is not teardown-specific — it
should be scoped to timeouts occurring FROM `link_established` to be useful
for attribution. That is a design flaw in my instrumentation, independent
of the counter bug.

### What IS established

- The link enters `link_established` 9 times and is torn down 8 times, with
  every state field golden (`C:0177`, `D:2E`, `S:24`, `A:0022`, `M:12/2`).
- The auto-recovery watchdog is NOT involved (`W:0`, and `K2:03` shows the
  sink reporting a stream).
- `G:` moves F0 -> F4 between good and bad cycles, so the `check_wait` gate
  path does fire on bad cycles — but how many of the 8 teardowns it
  accounts for is unknown.

### Next session

1. Debug the `gate_fail_sat` counter — simulate `aux_channel`'s check_wait
   path and confirm both counters advance together. A sim would have caught
   this before a build+flash cycle.
2. Re-scope the timeout counter to `link_established`-only teardowns.
3. Then re-run the good/bad comparison for a real attribution.

## Counter contradiction RESOLVED — it is the telemetry TRANSPORT, not the RTL (08-24 late)

`hdl/displayport/sim/tb_gate_fail_counters.v` (new) drives `aux_channel`
through the `check_wait` gate with the lock inputs low and checks both
counters:

```
pass 1: gate_fail 0 -> 2   gate_fail_sat 0 -> 2
pass 2: gate_fail 2 -> 0   gate_fail_sat 2 -> 4
pass 3: gate_fail 0 -> 2   gate_fail_sat 4 -> 6
PASS: counters TRACK in RTL (2-bit == sat mod 4 at every step)
```

(The delta is 2 per drive because the forced transition spans two clock
edges — a TB artifact. The property under test is that they AGREE, and
they do.)

**So `gate_fail_sat = 0` alongside `gate_fail = 1` is impossible in RTL.
One of those readings was corrupted in transport.**

### Root cause of the corruption

The FPGA's UART telemetry reaches telnet through the ESP32's **39-column**
console:

- `osd_console.c`: `#define CON_COLS 39  /* 40-col screen, leave 1 to avoid auto-wrap */`
- `osd_log()` formats into `char line[CON_COLS + 1]` — **every line is
  truncated to 39 characters**.

Our DP message is 121 printable characters, so it is chunked to fit, and a
character is lost at each chunk boundary. This matches every observation:
- 39-char chunks with `\r\n` inserted, seen in the raw stream;
- the 107-char message reassembling to 104 (2 boundaries, 2 lost chars);
- `K:` losing its low nibble (a boundary landed right after `K:0`), which
  is why the `K2:` duplicate was needed and worked.

`tn_send()` in `telnetd.c` is correct (it loops on partial sends) — the
loss is upstream of it, in the console path.

### Recommended fix (gateware-side, no firmware risk)

Emit the DP telemetry as **several short lines, each <= 39 characters**,
instead of one 121-char line. Then nothing is ever chunked or truncated and
every field arrives intact. E.g. 4 lines with a short prefix:
`D1 S:.. D:.. F:.. HLVC:....`, `D2 A:.... Y:.. C:....`, etc.

Alternative (firmware): widen/repair the console path so long lines are
chunked without loss. Higher risk, and the gateware fix removes the
dependency entirely.

⚠️ **Until this is fixed, ALL multi-field telemetry readings from today are
suspect** wherever a chunk boundary may have landed inside a field. Values
confirmed by a second independent field (e.g. `L:` vs `Y:`, or `K2:` vs
`K:`) are the trustworthy ones.

## ✅ TRANSPORT FIXED — teardown ATTRIBUTED (08-24, build 51c659c0)

Telemetry now emits as four lines of 36/30/31/23 chars, all under the
ESP32 console's 39-column limit. First bad cycle read after the fix:

```
S:24 D:2E F:EC HLVC:1110 P:1 E:22 R:0 A:0022 G:F4 Y:99 C:0177
Q:80BF63E K:03 X:91 W:0 T:50 M:12 M1:2 N:020 L:09
missing: none — all four lines intact
```

**Self-consistency restored:** `T:` gate_fail = 5, `G:`'s 2-bit twin = 1,
and 5 mod 4 = 1 ✓. The impossible disagreement is gone, confirming the
corruption was purely transport (and that the RTL was always correct, as
`tb_gate_fail_counters.v` proved).

### 🎯 ATTRIBUTION

| | value |
|---|---|
| establishes (`Y:`/`L:`) | 9 (so 8 teardowns) |
| **check_wait gate failures** | **5** |
| **AUX timeouts** | **0** |
| watchdog attempts (`W:`) | 0 |
| final sink status (`K:`) | 03 (streaming) |
| final training (`C:`) | 0177 golden |

**5 of the 8 teardowns are the `check_wait` gate**: the periodic link
re-check read the sink's lane status, found not-all-four locked, and tore
the link down to retrain. AUX transaction timeouts contribute NOTHING, and
the watchdog is uninvolved.

⚠️ RETRACTION: the earlier claim that a clean cycle showed 12 AUX timeouts
("routine pre-lock DEFERs") came from the corrupted `T:0C` reading. AUX
timeouts are zero. That inference is withdrawn.

3 of the 8 teardowns remain unattributed — either another re-entry path
into `link_established`, or a counting subtlety in how `Y:` tallies
establishes.

### Next step

`G:`'s high nibble is `dbg_gate_locks`, latched at EVERY `check_wait`
evaluation — so it shows `F` (all locked) because the LAST evaluation
passed. To learn WHICH lock bit drops, latch the locks **only on the
failing evaluation** (a sticky first-failure snapshot). That names the
culprit among clock / equ / symbol / align, which determines the fix:
- align (DPCD 0x204) flapping -> tolerate a transient before teardown;
- a lane's CR/EQ bit dropping -> genuine signal marginality;
- all bits clear -> the status read itself is returning stale/garbage,
  which would make the gate the bug rather than the messenger.

## 🎯 THE GATE FAILS ON ALL-ZERO STATUS, NOT ON A MARGINAL LANE (08-24, build 85a182b7)

`T:` now carries a sticky mask of which lock bits were clear at a failing
`check_wait` evaluation.

| | good cycle | bad cycle |
|---|---|---|
| `T:` | **000** | **F50** |
| decode | no gate failures at all | mask=**F**, gate_fails=**5**, aux_timeouts=**0** |
| `L:` / `Y:` | 01 / 11 | 09 / 99 |
| `K:` `C:` `W:` | 03 / 0177 / 0 | 03 / 0177 / 0 |

**mask = F means ALL FOUR lock bits (clock, equ, symbol, align) were clear
at a failing evaluation.** That is not how a marginal link degrades — a
signal-integrity problem drops one bit on one lane. And the link ends
golden (`C:0177`) with the sink streaming (`K:03`).

### Why all-zero implicates stale/absent status, not the sink

In `link_signal_mgmt.v`:
- `clock_locked` / `equ_locked` / `symbol_locked` are derived from
  `channel_state` through a `case(active_channel_count_i)`;
- **`align_locked <= channel_state[16]` is assigned OUTSIDE that case.**

So align reading 0 cannot be explained by a lane-count mismatch — it means
`channel_state` itself was **zero**. `channel_state` is zeroed in exactly
one place: the `else` branch of `if(tx_powerup == 1'b1)`, i.e. whenever the
PHY is powered down — which is what a teardown does.

`tx_powerup` is held high across `link_established` / `check_link` /
`check_wait` (verified), so the zeroing does NOT happen during the check
sequence itself. The consistent reading is:

**a teardown zeroes `channel_state`; the ladder retrains; if the next
`check_link` status read has not repopulated `channel_state` by the time
`check_wait` evaluates, the gate sees all-zero and tears down AGAIN** —
a self-sustaining loop that runs until one cycle's status read lands in
time. That matches everything observed: the bimodality (either the first
check lands cleanly, or you get a run of them), gate_fails=5 with 8
teardowns, and zero AUX timeouts.

### Proposed fix (shared RTL — NOT applied, needs review)

The gate must not tear down a link on ABSENT data. Require that a status
read actually completed since entering `check_link` before the gate is
allowed to fail — i.e. evaluate the locks only when `channel_state` has
been freshly written, and otherwise treat the check as "no information"
and stay in `link_established`.

⚠️ This is load-bearing code: the in-source comment records that a previous
"check-non-fatal" change was REVERTED on 08-18 because it blocked the
Anker's recovery-by-retrain. The fix must distinguish "no fresh status"
(do nothing) from "fresh status says a lane is lost" (tear down and
retrain, as today). Sim first — `tb_gate_fail_counters.v` is the harness.

## Second opinion adopted; hypothesis ranking REVISED (08-24, build ea52aafc)

The stale-channel_state theory is demoted to LAST. Verified against the
code (user's second opinion, both claims confirmed):

1. **Initial-qualification / maintenance mismatch (new #1).**
   `align_wait_after` advances to `switch_to_normal` on `symbol_locked`
   ALONE — `align_locked` (DPCD 0x204 INTERLANE_ALIGN_DONE) is never
   required to establish, and D4 checks only `0x202 == 0x77`. But the
   periodic gate demands all four. The design can legally establish, light
   D4, then "discover" align=0 at the first poll and tear down: visible
   D4 assertions + retraining with NO staleness and NO signal problem.
   The 1-or-6 shape fits the free-running ~1 s `link_check_count` poll
   cadence (flash spacing at ~1 s intervals would corroborate — bench ear).
2. **Real transient lane/align status from the converter**, amplified by
   the teardown/retrain loop.
3. **Short or non-ACK periodic AUX replies** — both jump straight to
   `error` and were UNCOUNTED (verified); credible source of the 3
   unattributed teardowns. "0 AUX timeouts" never exonerated this path.
4. Stale local `channel_state` (my prior #1): a retrain cannot re-establish
   without fresh status reads, and the periodic read commits 0x202-0x204
   several clocks before `check_wait` evaluates. Kept only as a defensive
   check via the freshness token.

Also corrected: sticky mask F is NOT four independent failures — the lock
bits are hierarchical by mask construction (CR loss forces EQ/symbol clear:
0x11 ⊂ 0x33 ⊂ 0x77), so F can accumulate from one CR event plus one align
event.

### Instrumentation in ea52aafc (all sim-verified before build)

- `T:` = 4 digits `{first_fail_mask, sticky_mask, gate_fails, timeouts}` —
  the FIRST-failure mask is non-sticky and latched once.
- `Z:` = atomic first-failure snapshot `{raw 0x204, 0x203, 0x202,
  status_seq, time-since-established/100ms}` latched in the same clock as
  the first failing evaluation.
- `J:` = `{short_replies, non_ACKs}` during `check_link` (previously
  invisible).
- Telemetry now FIVE lines (36/30/33/23/18 chars), msg_idx widened.

### Decisive decode for the next bad cycle

| Z: / T: / J: reading | verdict |
|---|---|
| `0x202=77, 0x204 bit0=0`, first_mask=0001 | **#1: qualification mismatch** — fix is to require align at initial qualification (or tolerate it at the gate) |
| one lane's CR/EQ/symbol missing in 0x202 | #2: genuine marginality |
| raw all zero + status_seq advanced | sink/converter actually returned zeros |
| raw all zero + status_seq NOT advanced | #4: stale-state race after all |
| J: accounts for ~3 | #3 confirmed for the unattributed teardowns |

## 🧪 OBSERVATION-WINDOW RESULT (08-24, build 4ff799d9, GATE_OBSERVE=1)

Cycle report (user): picture up after a short delay, STABLE.
Sticky read: `T:FF11  Z:80000019  J:1026  B:12E0008  Y:22 L:02 K:03 C:0177`

**Q2 (picture during bad status): YES — SURVIVES.** OBS_SUPPRESSED=6: the
gate wanted to tear down six times; the picture stayed up throughout.

**Q1 (recovery without training patterns): MOSTLY.** Bad status persisted
~6 polls (~6 s). Total establishes collapsed from 6-10 (pre-observe) to 2.
BUT gate_fails=1 — one real teardown after window expiry — so the bad
status outlasted the 6 s window by ~1 poll. NOT yet separated: longer
window ⇒ zero teardowns, vs converter needed that one retrain.

**Late-reply path CONFIRMED (B:12E0008):** first teardown = short-reply
from `link_established` (0x2E) with expected=0, rx_count=8 — a late 8-byte
AUX reply tripping the expected-1 wraparound check. The second opinion's
predicted invisible path, directly observed.

This cycle's first-failure snapshot: 0x202=00 (both lanes nil, vs 01
before), 0x204=80, seq=1, t=900 ms — same 900 ms first-poll timing.

⚠️ OPEN — counter accounting does not close: tagged entries (gate 1 +
timeout 1 + short 1 + other 2 = 5) exceed what 2 establishes can express.
Prime suspect: the short-read site evaluates per received byte. Counters
are qualitatively right, not yet quantitatively trustworthy.

### Implications
- Tolerance is now EVIDENCE-BACKED: 6 suppressed teardowns, stable picture.
- The production shape is: tolerate gate failures for a window/N polls
  (which also starves the late-reply loop of its teardown-churn), plus
  fix the expected==0 wraparound so late replies are DRAINED, not fatal.
- Next diagnostic iteration if wanted: window 6->10 s to test for a
  zero-teardown cycle; fix the per-byte counting.

## 🕳️ STABLE-DARK CAPTURED UNDER FULL INSTRUMENTATION (08-24, build 7b872b57)

Third cycle on the grace build: colorbars appeared fast, DROPPED after a
few seconds, screen stayed black. Telemetry, sustained for minutes:

```
HLVC:111x  C:8177  K:00  Y:18  L:08  T:0000  J:0000  B:0000000  W:0 -> 7
```

- `C:8177`: the sink reports PERFECT lane status (CR+EQ+SYM both lanes,
  INTERLANE_ALIGN done, LINK_STATUS_UPDATED). The periodic gate PASSES —
  the grace window and the teardown paths are uninvolved (T/J/B all zero).
- `K:00`: SINK_STATUS = no stream. Dark.
- `Y:18 / L:08`: established ONCE; video flapped 8 times, then dark.
- This is the HISTORICAL stable-dark state the 08-16 auto-recovery
  watchdog was built for — first time captured with full counters.

**WATCHDOG RESULT: fired its ENTIRE budget (W:7 = 7 cold-restart attempts,
por_n pulse + full CSR replay each) with ZERO effect.** And through all 7
attempts, `Y:`/`L:`/`C:` never moved — the ladder never observed the link
drop and the sink's status readings never changed even transiently, across
7 PHY resets that each kill our TX for ~2 ms.

**Most consistent reading: the CONVERTER'S STATUS/STREAM ENGINE IS FROZEN**
— it answers AUX from a latched state (8177/00 forever) and does not
consume the stream, regardless of link-level action on our side. That
would explain: (a) 7 CMU re-draws fixing nothing — evidence AGAINST the
old bad-CMU-draw theory for this family; (b) the historical fact that only
power-drain recoveries (hub power + HDMI unplug) ever cleared it; (c) the
retracted "hybrid mechanism" observations.

Open alternatives not excluded: our MSA/stream became invalid in a way the
sink registers only in SINK_STATUS (but then a watchdog CSR-replay retrain
should have rebuilt it); or wdog_force is not actually reaching the PHY in
this build (but then attempts would also not increment... they did).

⚠️ Honest flag: the late-reply DRAIN removed a class of spurious teardowns
that previously caused full retrains from established. It is CONCEIVABLE
those accidental teardowns sometimes rescued this state early (while the
converter was still responsive). Stable-dark predates today's changes, but
the drain may have widened its window. Needs incidence data over more
cycles, not argument.

RECOVERY: power cycle (the watchdog budget is exhausted; nothing on our
side will act further).

## Second consecutive stable-dark; drain-regression hypothesis now LEAD (08-24 eve)

Cycle 4 on the grace build: lit up, went dark — SAME frozen signature
(C:8177 / K:00 / Y:18 / L:08 / W:7). Corrections and new facts:

- **RETRACTED: "wdog replay_req stuck".** W:7 = {forcing=0, count=7}: the
  watchdog is IDLE with budget spent, and idle forces replay_req low.
  M:02 is benign (baseline cleared by the replay pulses; cannot
  re-establish until a training pattern next runs).
- **AUX is ALIVE in the dark state**: E: drifts (91→66→44) across samples
  while K:/C: stay frozen — the sink genuinely answers 8177/00 over a
  working AUX channel.
- **UNRESOLVED: G:00 and N:000** — as if the periodic check never latched
  its locks nibble in minutes of link_established, which contradicts the
  1 Hz link_check cadence + live AUX. Could not be closed remotely.

**Incidence: 2 consecutive stable-darks on the drain build, vs rare
(~none observed in dozens of cycles) before it.** The flagged hypothesis
is promoted to LEAD for the PERSISTENCE (not the entry) of the dark
state: pre-drain, a stray/late AUX byte arriving in link_established
tripped the expected==0 wraparound and tore the link down — a de-facto
(accidental) recovery kick for a sink that had stopped consuming the
stream. The drain now eats those bytes silently, so the ladder sits
established forever while the sink stays dark, and the watchdog's 7
attempts do not help (proven above).

**Next bench experiment (A/B, one flash): reflash 4ff799d9** — the
observation build, which has ALL the same instrumentation but NOT the
drain fix — and compare stable-dark incidence/persistence over cycles.
- dark occurs and SELF-RECOVERS via a spurious-teardown retrain ⇒ the
  drain removed a load-bearing accident; the production fix must replace
  it with a DELIBERATE kick (e.g. drained stray bytes while established
  AND !streaming ⇒ treat as a nudge to retrain, or let the watchdog act
  on it) rather than silence.
- dark equally persistent there ⇒ the drain is innocent; entry mechanism
  investigation continues (video flapped 8x before settling dark).

## A/B: drain build vs pre-drain build — stable-dark incidence (08-24 eve)

Same bench, same hub (Anker+Sceptre — CORRECTED 08-25, was mislabeled Ugreen), same evening, alternating only the
bitstream. All cycles user-observed with telemetry read each time.

| build | cycles | clean | settling | **dark** |
|---|---|---|---|---|
| `7b872b57` grace + **drain** | 4 | 2 | 1 | **2 (both persistent, unrecovered)** |
| `4ff799d9` grace-window, **no drain** | 8 | 4 | 4 | **0** |

Fisher-exact on 2/4 vs 0/8 gives p≈0.09 — **suggestive, not conclusive**;
the sample is small and stable-dark may cluster with converter state. But
the qualitative evidence stacks the same way:

- On the pre-drain build, EVERY settling cycle shows late-reply teardowns
  firing (`short`=1-2, `B:12E0008` each time) — the exact events the drain
  suppresses. These teardowns retrain the link during the converter's
  fragile settling phase.
- On the drain build, both darks entered after video flapped 8x
  (`Y:x8/L:08`) and then NOTHING further ever happened: no teardowns
  (J:0000), watchdog budget spent uselessly (W:7, sink answers frozen at
  8177/00 through 7 PHY resets).
- Interpretation: during settling, the converter sometimes wedges into
  its dark state; a teardown+retrain arriving DURING that entry window
  (which the stray-byte bug conveniently provided) re-engages it before
  the wedge completes. Remove the accidental kick and the wedge, once
  entered, is permanent — nothing at the DP link level reaches it.

### Verdict on the drain fix

The drain is CORRECT as a matter of protocol (a stale reply must not be
treated as a link failure) but it removed an accidental safety mechanism.
Ship the drain only together with a DELIBERATE replacement kick.

## REVISED PRODUCTION FIX PLAN

Keep (hardware-validated):
1. **GATE_GRACE window** (8 s) — rides out the settling storms; validated
   on both builds (suppressed polls with stable picture, establishes
   collapsed 6-10 -> 1-3).
2. **Late-reply DRAIN** — correct, but ONLY with (3).
3. **NEW: deliberate dark-state kick.** Trigger: established AND
   NOT streaming (`debug_sink[1:0]==0`) persisting N seconds — the same
   condition the watchdog uses, but the ACTION is a LADDER teardown ->
   full retrain (what the accidental kick did), NOT the watchdog's
   PHY-level cold restart, which is PROVEN useless against this state
   (W:7, zero effect, n=2). Suggested N ≈ 2-3 s (the accidental kick
   fired within ~1-2 s; the watchdog's 8 s grace never got a chance).
   Bounded budget like the watchdog's, re-armed on streaming, so a truly
   dead sink does not retrain forever.
4. **Watchdog demotion.** Its v3 recovery (por_n + CSR replay) is
   hardware-refuted for the stable-dark family it was built for. Keep it
   as a last-resort backstop AFTER (3) exhausts, or retire it for closed
   -loop builds; decide after (3) has bench data.

Open items carried:
- `G:00`/`N:000` anomaly in the dark state (checks not latching) —
  unresolved; may fall out of understanding the wedge entry.
- Dark ENTRY mechanism (video flaps x8 then sink deafness) — untouched by
  all of the above; the kick treats the symptom, deliberately.
- Settling-phase teardowns on the pre-drain build reset the grace window
  (window re-arms per establish) — with drain+kick this interaction
  disappears.

## Kick build first bench session (08-25 morning, COLD bench, build b241cf74)

| cycle | screen | telemetry | verdict |
|---|---|---|---|
| 1 (cold start) | colorbars, no delay noticed | KICKS=1, B:reason=6, K:03, Y:22/L:02 | dark entry (or slow cold wake) RESCUED by one kick |
| 2 | up → brief black → reappeared | KICKS=1, B:reason=6, K:03 | **dark entry rescued LIVE — user watched the ~3 s recovery** |
| 3 | black, D3+D4 blink storm, stayed black | KICKS=7 then W:7, K:00, **C:0000**, Y:5C | **DEEPER WEDGE: sink answers ZEROS for lane status (vs the 8177 wedge); 7 ladder retrains + 7 PHY cold restarts all failed; permanent** |

Findings:
1. **The kick works for entry-window darks** (2/3 this morning) — exactly
   the rescue the accidental teardowns provided, now deliberate, visible
   as a ~3 s blink instead of permanent darkness.
2. **A fully-wedged converter is beyond ANY link-side action** — 14
   combined recovery attempts, zero effect. C:0000 (AUX answering zeros)
   marks this deeper state; the C:8177 wedge and the C:0000 wedge may be
   stages of the same hang.
3. ⚠️ **A/B RE-EVALUATION: dark entry appears COLD-CORRELATED, not
   build-correlated.** 3/3 dark entries this cold morning on the kick
   build; last night's 0/8 (pre-drain) ran on a warm, heavily-cycled
   bench, the 2/4 (drain) earlier. The "drain removed the rescue and
   caused the darks" conclusion (p~0.09) is CONFOUNDED with
   temperature/time-of-day and is hereby DOWNGRADED to unproven. The kick
   remains justified on direct evidence (2 live rescues; watchdog
   provably useless), independent of that A/B.
4. UX: dual budget exhaustion (7 kicks + 7 wdog) = a long D3/D4 blink
   storm before permanent dark. Consider a combined cap or backoff if
   deep wedges are not rare.

**08-25 later: COLD-CORRELATION REFUTED by its own prediction.** After two
clean warm cycles, a deep wedge occurred AT STANDARD TEMPS (KICKS=7+W:7,
C:8177/K:00 flavor, Y:8F/L:0F saturated by the attempt storm). Dark-entry
driver: UNKNOWN — not build (confounded A/B), not simply temperature
(refuted). Session tally on b241cf74: 6 cycles = 2 clean, 2 rescued darks,
2 deep wedges (one C:0000, one C:8177 flavor). What survives scrutiny:
the kick rescues entry-window darks; dual-budget exhaustion cannot reach a
fully-wedged converter (n=2, both flavors). Probe queued: HDMI-only replug
on a wedged state — clears it => hang lives HDMI-side in the hub; doesn't
=> DP/USB-C side, full drain required.

## 🔬 HDMI-ONLY REPLUG PROBE — the wedge anatomized (08-25)

On the warm deep wedge (KICKS=7, W:7, C:8177/K:00): user replugged ONLY
the HDMI cable at the hub. **Colorbars returned INSTANTLY — no kick, no
retrain, no delay.** Telemetry after: picture visibly up while the sink
STILL answers `K:00` (not streaming) and `C:8177`; AUX timeouts saturated.

1. **Our DP link was healthy through the entire wedge** — video flowed the
   moment the HDMI side was kicked, with zero link-side action. The wedge
   lives in the hub's HDMI-side machinery.
2. **The hub's AUX/status engine freezes INDEPENDENTLY of its video path**
   and STAYS frozen: it reports "not streaming" while demonstrably
   displaying. Post-wedge DPCD status from this hub is fiction.
3. ⚠️ **DESIGN LANDMINE (survived by luck): with K frozen at 00 and a
   WORKING picture, an unexhausted kick budget would tear down the healthy
   link every KICK_CLKS forever.** Stability right now exists only because
   the budget was already spent. Mitigations: (a) the budget re-arms only
   on K==03, which a frozen 00 can never fake — accidentally the correct
   fail-safe direction; KEEP IT THAT WAY (never self-re-arm on time);
   (b) consider requiring the kick to observe at least one K!=0 reading
   in the current attach before arming at all (a sink that NEVER reported
   streaming is indistinguishable from a frozen one).

Revised wedge model: one hub hang with two observable stages
(status-frozen-plausible C:8177, status-zeros C:0000), video path
recoverable from OUTSIDE (HDMI replug) at least in the 8177 stage;
link-side actions (kick, watchdog) reach it only during entry.


## ⚠️ ERRATA AND STATUS RECLASSIFICATION (08-25, user review)

**Hub labeling correction.** Every session from the "Build A/B" section
onward (build A/B, observation build, grace build, kick build) ran on the
**ANKER + Sceptre**, not the Ugreen. In-line labels corrected where found;
any remaining "Ugreen" in those sections should be read as Anker. The
Ugreen's separately-established profile (clean Y:11 warm draws, rows
80-83) is unaffected. The settling signature and the wedge behavior
characterized this week belong to the ANKER.

**Hypotheses, not established facts** (language above may overstate):
- "The late-reply teardowns were an accidental rescue" — HYPOTHESIS. The
  supporting A/B was session/temperature-confounded (acknowledged above)
  and the cold-correlation replacement was itself refuted.
- "~7 s HDMI-side bring-up / converter settling" — HYPOTHESIS inferred
  from suppressed-poll counts and timing; not independently measured.
- What IS directly observed: the deterministic first-failure snapshot
  (01/00/80 at ~1 s); picture stable through suppressed status failures;
  the deep wedge unreachable by 7 kicks + 7 watchdog restarts; HDMI-only
  replug restoring the picture instantly while AUX still answers K:00.

**Kick build `b241cf74` reclassified: EXPERIMENTAL, UNSUCCESSFUL** by the
user-visible metric. Session outcome (6 cycles, Anker, one morning):
2 clean first-try, 2 distracting kick-recoveries, 2 persistent dark
failures — versus the informal legacy profile of ~1/2 first-try, ~1/2
delayed, darks rare. Kick development is STOPPED pending the three-build
comparison below; no retuning.

**Design rules adopted:**
- `K:00` alone must never trigger a production recovery action: HDMI
  colorbars were directly observed working while the hub kept reporting
  K:00. An `ever_streaming` prerequisite does NOT fix this — the status
  engine can freeze after streaming was observed.
- Deep-dark is a SEPARATE, HDMI-SIDE failure class. On occurrence: FIRST
  perform the HDMI-only replug probe (no DP retrain), record whether the
  picture returns and whether AUX stays frozen. Do not spend kick/watchdog
  budgets on a state shown unreachable from the DP side.

## THREE-BUILD COMPARISON PROTOCOL (08-25)

Builds (all archived):
- `f569d4f8` — TRUE legacy baseline (no grace, no drain, no kick; has the
  late-reply bug and full instrumentation).
- `4ff799d9` — grace window only (retains the late-reply bug).
- `7b872b57` — grace + late-reply drain, no kick.

Procedure per cycle — AMENDED 08-25 (user): **plain board power cycle,
no HDMI manipulation.** This is the HISTORICAL procedure the baseline
impressions were formed under; the originally-written per-cycle HDMI
unplug was an accidental procedure change. Because the board is
hub-backfed, every cycle is a hot re-attach to a still-running hub — hub
state carried across attaches is part of what is being measured.
HDMI actions are RECOVERY STEPS ONLY, each recorded when used:
(a) HDMI replug, board powered; (b) HDMI replug, board off; (c) full
drain. Wedge rate under plain cycling is a primary per-build outcome.
3. USER records: colorbars immediately / delayed (elapsed seconds + D4
   events observed) / persistent dark; and if delayed or suppressed-status
   activity occurs, whether the picture stays stable once up.
4. Telemetry read each cycle: Y, L, C, K, teardown reason (B:), suppressed
   count (J: obs digit).
Primary metrics: SCREEN TRUTH + time-to-stable-colorbars. D4 and K: are
secondary (AUX status shown capable of freezing while the picture works).

n >= 8 cycles per build; INTERLEAVE or rotate build order across the
session so hub history/temperature/time-of-day are not correlated with
build. No inference from a single block.

If, after the distributions, a kick is still justified: first candidate is
DIAGNOSTIC and strictly bounded — at most ONE kick per physical attach,
attach-scoped window that does NOT restart after retraining — and is not
called a production fix until it beats BOTH baselines on first-try rate,
delay, and dark-failure rate.


## Block 1 (`4ff799d9`, Anker+Sceptre, 08-25, plain-cycle procedure)

| cycle | screen | recovery needed | telemetry notes |
|---|---|---|---|
| 1 | persistent dark | HDMI replug (powered) FAILED -> full drain recovered | flap-storm class: Y:FF churn, 12 late-reply + 11 gate + 15 other teardowns, W:7; NOT the quiet frozen wedge |
| 2 | delayed colorbars, stable | none | settling class: J:1025, 1 gate teardown, ended K:03 C:0177 |
| 3 | wedged black | HDMI replug while POWERED DOWN recovered | plain power cycle (no HDMI touch) preceded it |

**Block 1 COMPLETE (8 cycles): 2 clean (c5,c7), 3 delayed-stable
(c2,c4,c8), 3 dark/wedge (c1 full-drain, c3 off-state-replug, c6
powered-replug).** Same build measured 0/8 darks two nights ago under the
same plain-cycle procedure — the between-SESSION swing dwarfs any
between-build difference seen so far. Recovery ladder is probabilistic:
powered replug failed on c1's storm but worked on c6's.

**New observations:** two wedges in a row is NEW behavior; wedge followed a
plain power cycle; HDMI replug with the board OFF also clears it. The
powered-replug failure on cycle 1 vs its instant success yesterday
separates the deep states further (flap-storm vs quiet-frozen). Elevated
wedge rate today (across kick AND grace-only builds) points at hub-state/
session effects dominating build identity — the interleaved protocol is
the only defensible comparison.
