# BrianHG DDR3 Controller - Gowin GW5AT Compatibility Evaluation

**Date:** 2026-03-17
**Target FPGA:** Gowin Arora V GW5AT-LV60PG484AC1/I0 (Tang Mega 60K)
**Repository:** https://github.com/BrianHGinc/BrianHG-DDR3-Controller

---

## Executive Summary

**Verdict: Not recommended for porting.** The BrianHG DDR3 Controller is a
well-designed soft DDR3 controller, but porting it to the Gowin GW5AT would
require significant effort with no clear advantage over the existing Gowin DDR3
IP already integrated into the a2mega project. The key barriers are
vendor-specific IO primitives, PLL architecture differences, and the tight
timing dependencies in the PHY layer.

---

## 1. BrianHG DDR3 Controller Overview

### Architecture

The controller is a hierarchical SystemVerilog stack:

| Layer | Module | Function |
|-------|--------|----------|
| Top | `BrianHG_DDR3_CONTROLLER_v16_top.sv` | Top-level wrapper |
| Arbiter | `BrianHG_DDR3_COMMANDER_v16.sv` | 16-port read/write arbitration with per-port priority, burst size, and cache |
| Sequencer | `BrianHG_DDR3_CMD_SEQUENCER_v16.sv` | Translates memory requests → DDR3 commands (activate, read, write, precharge, refresh) |
| PHY | `BrianHG_DDR3_PHY_SEQ_v16.sv` | Low-level DDR3 protocol timing |
| PLL | `BrianHG_DDR3_PLL.sv` | **Vendor-specific** - Altera `altpll` primitive |
| IO | `BrianHG_DDR3_IO_PORT_ALTERA.sv` | **Vendor-specific** - Altera `altddio_out`/`altddio_bidir` primitives |

### Key Features

- **Entirely soft controller** -- no hard DDR3 PHY dependency
- 16 simultaneous read/write ports with configurable priority
- Supports 8 to 128-bit data widths
- Full-rate, half-rate (50%), and quarter-rate (25%) interface modes
- Up to 400 MHz / 800 MT/s on Cyclone V -6 speed grade
- Optional graphics subsystem with 64 alpha-blended window layers
- HDL: SystemVerilog throughout
- License: "For public use" (no formal OSI license specified)

### Supported FPGAs (Official)

- Intel/Altera Cyclone III, IV, V, 10
- Intel/Altera MAX 10
- Intel/Altera Arria V (GX, GT, SX, ST, GZ)
- **Planned but not implemented:** Lattice ECP5, Xilinx Artix 7
- **Gowin is not on the roadmap**

---

## 2. Porting Barriers

### 2.1 PLL Module (HIGH complexity)

The BrianHG PLL generates 5-6 clock outputs at various phases:
- 0-degree DDR3_CLK
- 90-degree write-data clock
- Adjustable-phase read-data clock (runtime calibration)
- Quarter-rate and eighth-rate clocks

**Critical dependency:** Runtime dynamic phase adjustment via Altera's
`phasestep`/`phaseupdn` signals for read DQS calibration.

**Gowin equivalent:** The GW5AT has 8 PLLs (`rPLL`/`PLLVR`) with phase
adjustment and `DLLDLY` for DLL-based clock alignment, plus `IODELAY` with
256 taps at ~12.5ps resolution. The phase-stepping mechanism is architecturally
different -- Gowin uses per-pin IODELAY rather than PLL-level phase shifts for
DQS alignment.

**Effort:** Medium-high. A partial adaptation exists at
[Spaced-Cowboy/gowin-pll-ddr3-BrianHG](https://github.com/Spaced-Cowboy/gowin-pll-ddr3-BrianHG)
but appears to be early-stage and untested on GW5AT.

### 2.2 IO Port Module (HIGH complexity)

The Altera IO port uses:
- `altddio_out` for DDR output registers
- `altddio_bidir` for bidirectional DDR on DQ pins
- Pseudo-differential output for CLK/DQS
- Per-byte output enable with separate write/read clock domains

**Gowin equivalents needed:**
| Altera | Gowin GW5AT |
|--------|-------------|
| `altddio_out` | `ODDRC` / `OSER4` / `OSER8` |
| `altddio_bidir` | `IOBUF` + `IDDRC` / `IDES4` / `IDES8` |
| Pseudo-diff CLK | `TLVDS_OBUF` / `ELVDS_OBUF` |
| DDR register mode | `IDDR` / `ODDR` primitives |
| DQS strobe | Dedicated `DQS` primitive with `DQSR90`, `WPOINT`, `RPOINT` |

**Effort:** High. The GW5AT's DQS circuit architecture (DQSR90 for 90-degree
shifted strobe, WPOINT/RPOINT for FIFO pointer management) is fundamentally
different from Altera's approach. The entire PHY-level timing model would need
to be reworked, not just primitives swapped.

### 2.3 SystemVerilog Compatibility (MEDIUM risk)

The BrianHG controller uses SystemVerilog extensively. Gowin's GowinSynthesis
has limited but growing SystemVerilog support. Potential issues:
- Parameterized interfaces
- Complex generate blocks
- SystemVerilog-specific data types
- Unpacked array handling

This would require either testing each construct for compatibility or
translating to a subset that GowinSynthesis supports.

### 2.4 Timing Closure (UNKNOWN risk)

The PHY layer relies on tight IO timing margins that are validated against
Altera's IO cell characteristics. The GW5AT's 22nm process has different IO
timing characteristics. Meeting DDR3-800 or DDR3-1333 timing on the GW5AT
would require full re-characterization and potentially redesigning the
calibration logic.

---

## 3. Current A2Mega DDR3 Setup

The a2mega project **already has a working DDR3 interface** using Gowin's
official DDR3 Memory Interface IP (IPUG281):

- **Location:** `boards/a2mega/hdl/gowin/ddr3_memory_interface/`
- **Configuration:** 16-bit data width, 1:4 CLK ratio, 297 MHz memory clock
- **Interface clock:** 74.25 MHz (`clk_x1`)
- **App interface:** 128-bit data bus (BL=8 burst)
- **Peak bandwidth:** 1,188 MB/s (current utilization ~24%)
- **PLL:** Dedicated `pll_ddr3` generating 297 MHz from 27 MHz input

The framebuffer (`ddr3_framebuffer_480p.sv`) uses this interface for Apple II
video rendering to HDMI at 480p with clean CDC via async FIFOs and gray-code
synchronizers.

---

## 4. What BrianHG Would Offer (If Ported)

| Feature | BrianHG | Gowin IP (current) |
|---------|---------|-------------------|
| Multi-port arbiter | 16 ports with priority | Single-port (app manages arbitration) |
| Graphics subsystem | 64 alpha-blended windows | None (custom framebuffer logic) |
| Command caching | Per-port burst cache | Basic burst support |
| Open source | Yes (viewable/modifiable) | Encrypted/obfuscated |
| Validated on GW5AT | No | Yes (Gowin-supported) |

The 16-port arbiter is the most appealing feature for future expansion (shadow
memory, Ensoniq DOC, multiple framebuffers). However, a simpler arbiter can be
written on top of the existing Gowin IP with far less effort.

---

## 5. Alternatives Considered

| Alternative | Pros | Cons |
|-------------|------|------|
| **Gowin DDR3 IP (current)** | Already integrated, silicon-validated, Gowin-supported | Obfuscated, single user port |
| **BrianHG port** | Multi-port, open source, graphics engine | Massive porting effort, timing risk |
| **LiteDRAM** | Python-generated, AXI/Wishbone, multi-platform | GW2A support only, no confirmed GW5AT |
| **axi-ddr3-lite** | Small (~1300 LUTs), AXI interface, Gowin-proven | GW2A only, DLL-off mode (125 MHz max) |
| **UberDDR3** | Formally verified, Wishbone, portable | Not Gowin-tested, 4:1 ratio only |
| **nand2mario DDR3** | Purpose-built for Gowin, low latency | GW2A only, would need GW5AT IO port |
| **Custom arbiter on Gowin IP** | Minimal effort, proven base | Must design arbiter from scratch |

---

## 6. Recommendation

**Do not port the BrianHG controller.** Instead:

1. **Keep the existing Gowin DDR3 IP** -- it is silicon-validated, already
   integrated, and functioning at 297 MHz / 1,188 MB/s.

2. **Build a custom multi-port arbiter** on top of the Gowin IP's single-port
   interface. With 76% bandwidth headroom, time-division multiplexing between
   framebuffer reads/writes, shadow memory, and Ensoniq DOC is straightforward.

3. **If a fully open-source DDR3 stack is needed in the future**, evaluate
   UberDDR3 or axi-ddr3-lite as starting points for a GW5AT port -- they are
   simpler and more portable than BrianHG's Altera-centric design.

The porting effort for BrianHG (estimated weeks to months of PHY-level work
with uncertain timing closure) is not justified when the existing Gowin IP
already meets the project's bandwidth requirements with substantial headroom.
