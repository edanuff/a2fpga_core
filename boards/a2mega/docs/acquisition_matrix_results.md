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
- 🔎 **Early pattern (n=1 per cell — NOT a conclusion):** samples 2 and 3
  are identical in every field (5 blinks, L:08, Y:88, C:0177, A:0022) even
  though they differ in BOTH hub and monitor. If the remaining cells match,
  the multi-blink behavior is a property of the BUILD or the board state,
  not of any hub/monitor combination — which would send us back to the
  build comparison (f569d4f8 vs the staged 1f99bdb1) rather than to
  per-sink tuning. Sample 1 differing (4 blinks, Y:66) is confounded: it
  was the TRUE COLD start.
- Y: tracks difficulty so far: sample 1 (4 blinks) Y:66, sample 2 (5 blinks)
  Y:88 — the battle counter rises with the blink count, a third independent
  signal agreeing that these were hard acquisitions.
