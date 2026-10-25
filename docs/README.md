# A2FPGA Documentation Wiki

> Status (2026-10-25): index regrouped after the documentation audit; every page
> carries a dated status line under its title saying how current it is.

This is the reference wiki for the A2FPGA Multicard Core, written to be useful to both
human contributors and coding agents. Each file is a single, dense topic. Start here, then
follow links into the topic you need.

> **Agents:** the routing rules and inviolable constraints live in [`../AGENTS.md`](../AGENTS.md).
> This index is the map of *reference* material.

## Start here (read in this order)

1. [getting-started.md](getting-started.md) — first-run environment setup: which toolchains you need and how to install them (or run `/setup`).
2. [setup-gowin-cli.md](setup-gowin-cli.md) — get the Gowin toolchain running for headless/agentic builds (macOS-focused).
3. [build_profiles.md](build_profiles.md) — the per-board `*_profile.vh` feature/instrument switches and the resource report every build runs.
4. [architecture.md](architecture.md) — how the code is organized: shared `hdl/` vs per-board, the video/sound/bus pipelines, key file map.
5. [boards.md](boards.md) — the board matrix: which board uses which chip, memory, display output and coprocessor, plus every board's documentation index.
6. [conventions.md](conventions.md) — coding style, `.gprj` rules, how to add files, the archive rule.
7. [gotchas.md](gotchas.md) — hard-won lessons and traps that will silently cost you days.

## Architecture

| Doc | What's in it |
|---|---|
| [architecture.md](architecture.md) | Module organization, the subsystems, where things live |
| [video-pipeline.md](video-pipeline.md) | Generators, `pixel_stream_if`, the framebuffer and direct-display consumers, HDMI and DisplayPort backends, the DebugOverlay |
| [memory-system.md](memory-system.md) | `mem_port_if`, multi-port arbitration, the SDRAM / DDR3 / HyperRAM backends, CDC, framebuffers |
| [audio.md](audio.md) | Sound sources, mixing, filtering, CDC, HDMI and DisplayPort audio; per-board differences |
| [bus-interface.md](bus-interface.md) | Apple II bus snooping (`a2bus_if`), the slot/card system, IIgs autodetect, building a virtual card |
| [coprocessor-interface.md](coprocessor-interface.md) | How an MCU (BL616 / ESP32-S3) observes and controls the core; building a connector |
| [peripheral-cards.md](peripheral-cards.md) | The emulated cards (SuperSprite, Mockingboard, SSC, Disk II, ProDOS HDD, Uthernet II, CardROM) and reused cores |

## Boards and direction

| Doc | What's in it |
|---|---|
| [boards.md](boards.md) | Per-board chip / memory / display / status matrix, `.gprj` names, flashing, per-board doc index |
| [ROADMAP.md](ROADMAP.md) | Backlog and future direction; how it relates to GitHub issues and the board TODOs |

## Decisions

| Doc | What's in it |
|---|---|
| [adr/](adr/README.md) | Architecture Decision Records: the *why* behind irreversible choices, and the candidates still to be written |

## Hardware notes

| Doc | What's in it |
|---|---|
| [gameport-dongle.md](gameport-dongle.md) | USB game-port dongle design note (gamepad to joystick hardware path) + schematic in `images/` |

## Deep references

| Doc | Topic |
|---|---|
| [memory_bandwidth_analysis.md](memory_bandwidth_analysis.md) | DDR3 / SDRAM / HyperRAM bandwidth headroom analysis (sections 1 and 2 partly superseded; see its status line) |

## Per-board documentation

Every board has a README with a **Documentation** section that indexes its `docs/`
folder; [boards.md](boards.md#per-board-documentation) lists them all. The deep
material (bring-up logs, design notes, scoping studies) lives there, not here.

## Archive

Documents that no longer describe the code are not deleted; they move to an
`archive/` folder beside the documents they came from, each archive with a README
that names what superseded every file:

- [docs/archive/](archive/README.md) — repository-wide history (merge records are fork-only)
- [boards/a2mega/docs/archive/](../boards/a2mega/docs/archive/README.md) — DDR3 framebuffer, Ensoniq-on-DDR3, JTAG, the 2026-08 DP bring-up
- [boards/a2p25/docs/archive/](../boards/a2p25/docs/archive/README.md) — the previous revision: LCAM capture link (reusable), 3-wire SPI
- [boards/a2n20v2-GS/docs/archive/](../boards/deprecated/a2n20v2-GS/docs/archive/README.md) — the SDRAM framebuffer workplan (the GS build is deprecated; see [boards/deprecated/](../boards/deprecated/README.md))

The rule is in [conventions.md](conventions.md#archive). The file-by-file audit that
produced this structure is `docs/DOCS_AUDIT_2026-10.md` (a fork-only record, excluded
from the public upstream by `UPSTREAM_EXCLUDE.txt`).

## Maintaining these docs

- **One source of truth per fact.** If a command or detail appears in two files, one of
  them should link to the other instead of copying.
- **Evergreen here, temporal elsewhere.** Time-sensitive state (what's in progress, current
  branch, today's blocker) belongs in `boards/<board>/TODO.md`, [ROADMAP.md](ROADMAP.md), or
  GitHub issues, not in reference docs that are assumed current.
- **Date your status.** Every page starts with a one-line `Status (YYYY-MM-DD)` note. When
  you verify or change a page, update the date; when a statement cannot be verified from the
  code, say so there rather than guessing.
- **Promote lessons.** When you discover a non-obvious trap, add it to [gotchas.md](gotchas.md)
  so the next contributor (human or agent) doesn't rediscover it.
- **Archive, don't delete.** See the archive section above.
