# Roadmap & Backlog

Forward-looking work for the A2FPGA core. This is the place an agent or contributor looks to
answer *"what could I work on?"* and to surface candidate features to the maintainer.

## How this relates to other sources

| Source | Use for |
|---|---|
| **This file** | Larger directional items and the maintainer's intended future work. |
| **GitHub issues** (upstream `a2fpga/a2fpga_core`) | Reported bugs and concrete tracked tasks. **Always check these too.** |
| **`boards/<board>/TODO.md`** | Per-board, often in-progress or short-term items. |

> **Agents:** before proposing or starting a feature, check all three, then *offer* the user
> a specific item rather than picking one unilaterally. Confirm scope and target board first.

## Checking GitHub issues

Issues live on the **upstream** repo `a2fpga/a2fpga_core` (this fork has issues disabled), so
target it explicitly:

```bash
gh issue list -R a2fpga/a2fpga_core                       # open issues
gh issue list -R a2fpga/a2fpga_core --label enhancement   # feature requests
gh issue view <n> -R a2fpga/a2fpga_core                   # details
```

## Themes

> Status (2026-10-25): rewritten from the board plans and the open-work lists of the
> 2026-10-22 merge; the previous version described the June 2026 state (ES5503 on the ESP32,
> a2mega "60K bring-up", Enhanced "audio-clock fix, OSD, Disk II"), all of which has shipped.

These are the active directions, summarized from the per-board plans, `TODO.md` files and open
issues. They are **not prioritized** — priority is the maintainer's call. Each links to the
authoritative detail rather than copying it.

### 1. Graphics accuracy & artifacts (cross-board)

The largest cluster of reported bugs is Apple II / IIgs display accuracy in the shared
`hdl/video/` pipeline. Reproductions and discussion in issues:
[#39 Shufflepuck HGR](https://github.com/a2fpga/a2fpga_core/issues/39),
[#26 graphic/character artifacts](https://github.com/a2fpga/a2fpga_core/issues/26),
[#25 hi-res with 80-col](https://github.com/a2fpga/a2fpga_core/issues/25),
[#24 lines/artifacts on some IIgs software](https://github.com/a2fpga/a2fpga_core/issues/24),
[#30 IIgs super-hi-res](https://github.com/a2fpga/a2fpga_core/issues/30),
[#47 Ensoniq HDMI output with the debug overlay](https://github.com/a2fpga/a2fpga_core/issues/47).

### 2. IIgs bus-timing reliability (production)

Sporadic garbage data on some IIgs systems from the data-byte sampling window in
`apple_bus.sv` (a regression introduced when denoising the ph1 clock to pass mbaudit), and
[#56 another card's slot reads disturbed on a IIgs](https://github.com/a2fpga/a2fpga_core/issues/56).
Top-priority item in [a2n20v2/TODO.md](../boards/a2n20v2/TODO.md) and
[a2n20v2-GS/TODO.md](../boards/a2n20v2-GS/TODO.md). Affects the stable board. A predictive
bus-clock recovery (`apple_timer`, simulation-verified, opt-in) was prototyped on a branch that
is not in the tree (listed in the 2026-10-22 branch audit as rescued, uncommitted work).

### 3. IIgs audio — Ensoniq DOC 5503

The pipelined DOC runs on every board with off-chip memory (DDR3 on the a2mega, HyperRAM on
the a2p25, SDRAM through the baseline DOC on the Tang Nano boards). Open:
[#33 DOC5503 mixer](https://github.com/a2fpga/a2fpga_core/issues/33); a deep listening pass of
the pipelined DOC (percussive and swap cases); keeping the a2p25's per-frame LATE count at
zero as the core grows (`boards/a2p25/docs/RESOURCE_AUDIT.md`).

### 4. Coprocessor feature parity (BL616 and ESP32)

The Enhanced feature set (menu, disk-image serving, Uthernet II, FPGA self-update, telnet
and FTP) is on the BL616 and ported to the shared ESP32 firmware for the a2mega and a2p25
(`boards/a2p25/docs/ESP32_FEATURE_PORT.md`: P1–P4 verified, P5 next). Open on the ESP32:
USB host on the a2p25's USB1 port (hubs, pads, mass storage) with the runtime OTG role policy,
and the image storage strategy without an SD card. Open on the BL616: the dual-rate / `_fb`
builds' CDC request-path crossing
([#52](https://github.com/a2fpga/a2fpga_core/issues/52)).

### 5. a2mega: board revision 1.0a4 and the IIgs socket

The 1.0a3 board runs the full core over USB-C DisplayPort on both SOMs (60K, 138K). The
1.0a4 revision is gated by `boards/a2mega/docs/board_1_0a4_requirements.md` and the carry-over
from the a2p25 (`a2p25_carryover_2026-10-11.md`); the pre-revision exercise plan's remaining
gates are an open question for the maintainer (see `docs/DOCS_AUDIT_2026-10.md` section 5).
On the 138K, the 65C816 in-socket replacement boots ROM 01 and ProDOS through the real socket
(`gs_socket_65816_scoping.md` section 8b); next are the auto-arm reset sequencing and the
TransWarp-style accelerator scoping (`gs_socket_s7_accelerator_scoping.md`), whose HyperRAM
backing store is a board-revision item. Timing policy for every a2mega build: the no-skating
rule in `boards/a2mega/docs/timing_round2.md` (+0.5 ns on every clock, three clean rolls).

### 6. a2p25: displayport1 revision

The IIgs boots on the new core over DP 1080p with SHR and DOC audio (2026-10-11,
`boards/a2p25/docs/MODERNIZATION_PLAN.md` sections 6f–6g). Open: the known risks in that
plan's section 7 (D-PHY amplitude margin at the SN75DP130, the PLL cascade soak, CK# floating
on non-Infineon HyperRAM parts), the resource ceiling (97% CLS; Mockingboard, SuperSprite and
Uthernet II are off), and the HDD image corruption seen around installs. The rev-0 LCAM
firmware tree under `boards/a2p25/src/` is a pending decision.

### 7. Board maturity

- **a2n20v2-GS** is intended to eventually become the main a2n20v2 release; the GS and
  Enhanced cores now share `apple_bus` / `apple_memory_sdram`, and folding the GS into the
  Enhanced build (dual-rate SDRAM, then the framebuffer parameter) is the path.
- **a2n20v1** and **a2n9** are deprecated (maintenance-only, kept in sync with shared HDL).

### Explicitly not planned / rejected

- **On-FPGA PicoSoC (PicoRV32) soft core** — removed. Coprocessor functionality is now an
  external MCU: BL616 on a2n20v2-Enhanced, ESP32-S3 on a2mega and a2p25. See
  [ADR-0001](adr/0001-agents-md-as-router.md) context and the Enhanced
  [src/README.md](../boards/a2n20v2-Enhanced/src/README.md).
- **BrianHG DDR3 controller port** — evaluated and rejected (Altera-centric PHY; use the
  Gowin DDR3 IP instead).
- **DP output through Gowin's own EDP IP stack** on the a2mega — evaluated and not taken; the
  record is fork-only (`boards/a2mega/docs/archive/dp_gowin/`).
- **4-lane RBR on the a2mega** — not taken; the a2mega stays 2-lane HBR, the 4-lane path shipped
  on the a2p25 through the shared core (`boards/a2mega/docs/archive/dp-bringup-2026-08/`).
- **Running the ES5503 on the ESP32 over an LCD_CAM capture link** (rev-0 a2p25) — replaced by
  the fabric pipelined DOC on HyperRAM (`boards/a2p25/docs/archive/`).

> Maintainer: add longer-term/aspirational items here as they come up, and record anything you
> specifically do *not* want built (with a one-line why) so contributors don't re-propose it.
