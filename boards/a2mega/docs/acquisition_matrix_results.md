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
| 1 | A | **TRUE COLD** (multi-hour inert) | 4 | 06 | 05 | **1** | 66 | 0177 | 0022 | 4 blinks to colorbars, then lost colorbars momentarily and recovered. L: confirms the user's count exactly (4 blinks + hold + 1 recovery = 6 assertions) |

## Running observations

- The `L:` cross-check held on a HARD sample (not just clean ones): user
  counted 4 blinks plus a dropout, telemetry independently read 6
  assertions. Instrument trusted for this matrix.
- Post-lock dropouts are now visible as a separate phenomenon from
  blinks-to-lock. Worth tracking: a build that locks fast but drops is not
  the same as one that locks slow and holds.
