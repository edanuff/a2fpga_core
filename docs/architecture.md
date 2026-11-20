# Architecture & Code Organization

> Status (2026-10-25): layout table and the one-paragraph summary brought up to the code
> (DisplayPort, HyperRAM, disk and Uthernet II directories; DP 1080p on the a2mega and a2p25).

How the A2FPGA codebase is laid out, so you (or an agent) can find the right file and know
which questions to ask before changing anything.

## Top-level layout

| Path | Contents |
|---|---|
| `hdl/` | **Shared HDL** (SystemVerilog/Verilog/VHDL) portable across all boards. By subsystem: `bus/` (Apple II bus, IIgs detector), `memory/` (`mem_port_if`, shadow memory, arbiters), `slots/`, `video/` (generators, consumers, the three framebuffers), `hdmi/`, `displayport/` (the DP transmitter core with its GW5A SERDES backend), `sdram/`, `ddr3/`, `hyperram/` (the three memory backends), `sound/` (speaker, DOC5503 baseline and pipelined, fetch cache, mixer), `disk/` (Disk II and ProDOS HDD), `uthernet2/`, `mockingboard/`, `supersprite/` + `f18a/`, `ssc/`, `debug/` (DebugOverlay), `support/`. |
| `boards/<board>/` | Per-board project: Gowin `.gprj` (one per die or variant), board-specific `hdl/` (incl. `top.sv` and `<board>_profile.vh`), build outputs in `impl/pnr/` plus the resource snapshot `impl/*_resources.json`, `TODO.md`, and `docs/` (with `archive/`). See [boards.md](boards.md). |
| `boards/<board>/hdl/` | Board wiring and `top.sv`; board-specific bus front end, clocks, PHYs (a2mega `dp_test*`, `twgs/` socket; a2p25 `dp/` D-PHY) and generated Gowin IP under `gowin/`. |
| `boards/<board>/src/` | Coprocessor firmware and Apple II-side sources: the BL616 firmware and CardROM under `boards/a2n20v2-Enhanced/src/`, the shared ESP32-S3 firmware under `boards/a2mega/src/a2fpga_esp32/` (built for the a2p25 as `BOARD_REV=225`). |
| `sim/` | Shared simulations (`doc5503/`: the differential bench for the pipelined DOC). Board benches live under `boards/<board>/tests` or `hdl/displayport/sim`. |
| `tools/` | Build, flash and resource-report tooling, Gowin setup scripts, the doc link checker and the upstream export ([setup-gowin-cli.md](setup-gowin-cli.md), [tools/README.md](../tools/README.md)). |
| `releases/` | Prebuilt artifacts for users. |
| `docs/` | This wiki; `docs/archive/` for superseded material. |

**Rule of thumb:** portable logic → `hdl/`; anything that knows about a specific FPGA, its
pins, or its memory → `boards/<board>/hdl/`. The per-board `top.sv` is the integration point.

## What the core does (one paragraph)

The A2FPGA card sits on the Apple II bus, snoops all display-memory accesses, and renders the
Apple II / //e / IIgs display to a modern monitor: 720x480 HDMI on the Tang Nano boards,
1080p USB-C DisplayPort on the a2mega and a2p25. It also emulates popular peripheral cards
(Mockingboard sound, SuperSprite/TMS9918A, Super Serial Card, Disk II, ProDOS hard disk,
Uthernet II) and the IIgs Ensoniq DOC, with a coprocessor (BL616 or ESP32-S3) serving disk
images, networking and an on-screen menu on the boards that have one. See the top-level
[README.md](../README.md) for the user-facing feature description and credits.

## The video pipeline

Video generators are decoupled from the display backend by `pixel_stream_if`: a generator emits
pixels into that interface and knows nothing about the framebuffer. Each board picks a **consumer**:

- **`framebuffer_writer`** → **Apple-timed framebuffer** (SDRAM, DDR3 or HyperRAM) — keeps Apple
  beam-racing techniques intact. Used by `a2mega` and `a2p25` (`framebuffer_1080p`, scanned out
  by the DisplayPort transmitter), `a2n20v2-GS` and the Enhanced `_fb` build (`sdram_framebuffer`,
  HDMI).
- **`direct_display`** → **display-locked render** — no framebuffer; the classic `a2n20v2` and the
  classic Enhanced build.

**Full detail — the contract, the HDMI and DisplayPort backends, and how to add a video
generator (e.g. a Videx card) — is in [video-pipeline.md](video-pipeline.md).** Do not edit the
SDRAM controller `hdl/sdram/sdram.sv`.

## Memory

Off-chip memory (SDRAM on the Tang Nano 20K boards, DDR3 on the a2mega, HyperRAM on the a2p25)
is reached through one common port interface, `mem_port_if`, fronted by a per-board multi-port
arbiter with static priority and, where the memory clock differs from the logic clock, a
clock-domain crossing. The framebuffer, video shadow RAM, the Ensoniq wavetable (through the
pipelined DOC) and the coprocessor's disk windows are all memory clients. **Full detail — the
port contract, arbitration/priority, the three backends, the CDC, and the framebuffers — is in
[memory-system.md](memory-system.md).**

## Sound & peripheral emulation

Audio — the sound sources, mixing, filtering, CDC, and HDMI / DisplayPort audio — is documented
in [audio.md](audio.md). The emulated peripheral cards themselves (SuperSprite/F18A,
Mockingboard, Super Serial Card, Disk II, ProDOS HDD, Uthernet II, CardROM) and the open cores
they reuse are in [peripheral-cards.md](peripheral-cards.md).

## Bus interface

The core snoops the Apple II bus (via the per-board `apple_bus` and board-specific interface
hardware — a CPLD on the a2n20 cards, discrete level-shifters elsewhere) and exposes it as
`a2bus_if`; virtual peripheral cards consume it plus `slot_if`. The IIgs is detected at runtime
(`gs_detector`), and the a2mega 138K build can additionally take over the IIgs CPU socket
(`GS_SOCKET`, `boards/a2mega/hdl/twgs/`). **Full detail — the
`a2bus_if`/`slot_if` contracts, the slot/card decode, driving data back, and how to add a card —
is in [bus-interface.md](bus-interface.md).** The coprocessor-facing control/config interfaces
(`a2bus_control_if`, `video_control_if`, `slotmaker_config_if`, `drive_volume_if`, `f18a_gpu_if`)
are in [coprocessor-interface.md](coprocessor-interface.md).

## See also

- [boards.md](boards.md) — per-board differences
- [gotchas.md](gotchas.md) — the non-obvious traps in this pipeline
- [conventions.md](conventions.md) — how to add a module correctly
