# Architecture Decision Records (ADRs)

An ADR captures a decision with lasting consequences — **the *why*, not the how** — so that
future contributors (and agents) don't re-litigate settled choices or unknowingly build
something contrary to a deliberate decision.

## When to write one

Write an ADR when a choice is **hard to reverse or easy to violate by accident**: a memory
architecture, a CDC strategy, a board's status change, a "we tried X and rejected it" outcome.
Don't write one for routine code — that's what the code and PR history are for.

## Format

- Filename: `NNNN-short-title.md` (zero-padded sequence, e.g. `0002-ddr3-no-double-buffering.md`).
- Copy [`template.md`](template.md) to start.
- Status is one of: `Proposed`, `Accepted`, `Superseded by NNNN`, `Deprecated`.
- Keep it short. The value is the rationale and consequences, not length.

## Index

| ADR | Title | Status |
|---|---|---|
| [0001](0001-agents-md-as-router.md) | AGENTS.md as the single router; docs/ as the wiki | Accepted |

> Add a row here for each new ADR.

## Candidates (decisions already made and recorded in logs, not yet written up)

> Status (2026-10-25): listed by the documentation audit; each has a source document that
> records the decision and its evidence. Writing them up is open work.

| Candidate | Decision as recorded | Where the record is |
|---|---|---|
| Timing-closure policy | No re-rolls as closure; durable = +0.5 ns on every clock on three rolls of the final source; a2mega SDCs carry `set_clock_uncertainty 0.5` | `boards/a2mega/docs/timing_round2.md`, [gotchas.md](../gotchas.md) |
| `bit_security 0` everywhere on GW5A | Gowin's SecurityBit default races the JTAG-SPI flash path; every GW5A process config sets it off | `boards/a2mega/docs/jtag_flash_reliability.md` (2026-08-15 section) |
| DOC on memory ports only behind a pipelined fetch | The baseline DOC cannot meet its deadline from DDR3/HyperRAM; `doc5503_pipelined` can, with the port at top priority on HyperRAM | `boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md`, `boards/a2p25/docs/RESOURCE_AUDIT.md` |
| One `*_profile.vh` per board | Feature and instrument switches live in one file per board; the register map never renumbers for a profile | [build_profiles.md](../build_profiles.md) |
| `Convert_SDP32_36_to_SDP16_18` left on | The IDE option doubles x32/x36 SDP blocks but is kept as a low-VCC workaround; plan BSRAM as ceil(width/18) blocks | `boards/a2p25/docs/RESOURCE_AUDIT.md` |
| Archive, don't delete | Superseded documents move to a per-folder `archive/` with a README entry | [conventions.md](../conventions.md#archive) |
| Fork-only material by manifest | `UPSTREAM_EXCLUDE.txt` + `tools/upstream_export.sh` decide what reaches the public upstream | repo root |
