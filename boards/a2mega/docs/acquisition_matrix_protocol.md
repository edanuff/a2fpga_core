# Acquisition resiliency matrix — protocol (08-24)

Measures **blinks-to-solid** (D4 lighting and holding = colorbar lock)
across every hub × monitor combination, so we get a resiliency picture
instead of another single-observation story.

## Why this and not the automated instrument

The `L:` counter + `v`-draw method (row 88) samples the WARM regime: a `v`
draw power-cycles the board (hub-backfed VBUS) but the **hub stays powered
and the monitor stays awake**. The regime that matters — and where the
multi-blink behavior lives — is the one where the HUB comes up too. That
requires the manual power-down regimen, so this matrix is manual by
necessity. The row-88 warm baseline (30× `L:01`) does NOT speak to it.

## Cells

| | Fangor | Sceptre |
|---|---|---|
| **Ugreen** | A | B |
| **Anker**  | C | D |

## Per-cycle regimen (identical every time — this is the controlled variable)

1. Power down the board.
2. **Unplug HDMI** (the power-down regimen: HDMI +5 V backpowers the hub).
3. Wait ~30 s so the hub actually drops.
4. **Reconnect HDMI** ⚠️ — row 89: a missing HDMI cable makes D4 blink
   without ever locking, indistinguishable from a training regression.
5. Power up.
6. **Count D4 blinks until it goes solid.** Solid = colorbar lock.
7. Report the count; the telemetry read is automated from there.

## Order — rotate to balance thermal drift

Three passes, rotating the starting cell so no combination is always
first (cold) or always last (warm):

- pass 1: A B C D
- pass 2: B C D A
- pass 3: C D A B

12 cycles, n=3 per cell, order-balanced. Record board temperature at the
start of each pass if convenient.

**The very first cycle of a session is a TRUE cold start** (board inert for
hours). Flag it separately — it is its own regime and n=1 per session.

## What gets recorded per cycle

| field | source |
|---|---|
| blinks-to-solid | user (the primary metric) |
| `L:` | telemetry — should equal blinks+1; a mismatch is itself a finding |
| `Y:` | battle counter (Y:11 clean … Y:FF saturated) |
| `C:` | 0177 = golden training result |
| `D:` | ladder state (2E = link_established) |
| `A:` / `M:` / `M1:` / `N:` | sink request, applied levels per lane, gate counters |

`L:` gives an automatic cross-check on every sample: `L:01` = 0 blinks,
`L:02` = 1 blink, and so on. If the user's count and `L:` disagree, that
means blinks occurred that never reached `sink_confirmed` — worth knowing.

## Escalation

3 per cell is a SCREEN. Cells that come out unambiguous (e.g. 0,0,0) need
no more. Cells that come out mixed (e.g. 0,4,1) get another 3-5 before any
conclusion — a mixed cell at n=3 means "unresolved", not "worse".

## Statistical honesty

- Do NOT compare cells on single samples; the metric is stochastic
  (one unchanged bitstream measured {0,1,1,6,1}, row 86).
- A difference of one blink between cells at n=3 is noise.
- What this CAN establish: whether some combination consistently fails to
  first-lock, and whether any combination is reliably clean.

## Build under test

`f569d4f8` — per-lane M5, `phase_done` gate, CDC synchronizer fixes,
acquisition counter v2. Control build `1f99bdb1` is rebuilt bit-identical
and staged if a build-vs-build comparison follows.
