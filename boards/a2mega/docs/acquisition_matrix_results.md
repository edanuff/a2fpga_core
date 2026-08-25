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
