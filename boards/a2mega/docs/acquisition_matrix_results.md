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
