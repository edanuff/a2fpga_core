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

---

## 7. The Real Problem: Multi-Client Arbitration

The motivation for evaluating BrianHG was not the DDR3 PHY -- it was the
**16-port COMMANDER arbitration layer**. The a2mega board needs framebuffer,
shadow memory, and Ensoniq DOC to share DDR3 simultaneously, just as the
a2n20v2 board shares SDRAM across 6 ports via the `sdram.sv` controller.

### How SDRAM Multi-Port Works (a2n20v2 Reference)

The SDRAM controller (`hdl/sdram/sdram.sv`) has **native multi-port
arbitration** built into the controller itself:

| Port | Client | Priority |
|------|--------|----------|
| 0 | FB_READ_PORT (framebuffer line prefetch) | Highest |
| 1 | FB_WRITE_PORT (pixel writes) | |
| 2 | SHADOW_READ_PORT (video gen reads) | |
| 3 | SHADOW_WRITE_PORT (CPU writes) | |
| 4 | DOC_MEM_PORT (Ensoniq wavetable reads) | |
| 5 | GLU_MEM_PORT (Ensoniq parameter writes) | Lowest |

Each client gets its own `mem_port_if` with `rd`/`wr`/`addr`/`data`/`ready`
signals. The controller scans ports 0→5, selects the first with a pending
request, executes it, and returns to IDLE. Clients are completely decoupled --
they don't know about each other. CDC is handled per-port via `mem_port_cdc.sv`
(54 MHz ↔ 108 MHz gray-code FIFOs).

### Why DDR3 Can't Do This Today

The Gowin DDR3 IP provides a **single command port**: one `cmd`/`cmd_en`/`addr`
input, one `wr_data`/`rd_data` path. The `ddr3_framebuffer_480p` module
currently owns this port entirely and handles external clients via a bolted-on
state machine (`ext_state` / `EXT_IDLE` → `EXT_DECODE` → `EXT_ISSUE`) that
only runs when the framebuffer isn't busy:

**Current architecture (problematic):**
```
                              ┌──────────────────────────────────┐
  apple_fb_writer ──►         │                                  │
  vgc_fb_writer ──►           │   ddr3_framebuffer_480p          │
                              │   (owns DDR3 command port)       │
  ext_ports[0] (shadow rd) ──►│                                  ├──► Gowin DDR3 IP
  ext_ports[1] (shadow wr) ──►│   ext_state machine:             │    (single port)
  ext_ports[2] (DOC rd) ────►│   only runs when FB is idle      │
  ext_ports[3] (GLU wr) ────►│                                  │
                              └──────────────────────────────────┘
```

**Problems:**
1. **Ext ports are starved** -- they only access DDR3 in gaps between FB
   writes and line fetches
2. **Arbitration is inside the framebuffer** -- tightly coupled to FB state,
   not a reusable service
3. **One ext read at a time** -- `ext_rd_pending` blocks all other ext port
   activity until the DDR3 response arrives
4. **CDC issues** -- async FIFOs on GW5AT corrupt multi-bit data during the
   54 MHz → 74.25 MHz crossing; toggle-based workaround only handles ports 0-1
5. **Ensoniq latency risk** -- DOC oscillators need samples every ~1.1 µs;
   a 16-cycle FB write batch (~215 ns) could delay responses unpredictably

### Recommended Architecture: Standalone Multi-Port DDR3 Arbiter

Extract arbitration from the framebuffer and build a standalone module that
mirrors `sdram.sv`'s architecture but wraps the Gowin DDR3 IP:

**Target architecture:**
```
                              ┌──────────────────────────────┐
  Port 0: FB reads ─────────►│                              │
  Port 1: FB writes ────────►│   ddr3_arbiter               │
  Port 2: Shadow reads ─────►│   (standalone, reusable)     │
  Port 3: Shadow writes ────►│                              ├──► Gowin DDR3 IP
  Port 4: DOC reads ────────►│   Static priority scan       │    (single port)
  Port 5: GLU writes ───────►│   Per-port CDC               │
                              │   128-bit burst caching      │
                              └──────────────────────────────┘

  ddr3_framebuffer_480p becomes just another client (ports 0+1),
  same as sdram_framebuffer.sv on the SDRAM board.
```

**Design requirements:**

1. **Same `mem_port_if` interface** that all clients already use -- no client
   code changes needed for shadow memory or Ensoniq
2. **Static priority arbitration** matching `sdram.sv`'s `get_priority_port()`
   scan -- FB reads highest, GLU writes lowest
3. **Per-port CDC** via `mem_port_cdc.sv` instances (54 MHz client ↔ 74.25 MHz
   DDR3), using toggle-based handshake to avoid GW5AT async FIFO corruption
4. **128-bit burst cache** -- each 128-bit DDR3 read returns 4 × 32-bit words;
   cache the burst and serve up to 4 consecutive reads without re-issuing
   DDR3 commands (4× reduction in shadow memory DDR3 traffic)
5. **Pipelined command issuing** -- overlap read latency (~20 cycles) with
   write processing for other ports, using the Gowin IP's `cmd_ready` flow
   control
6. **Framebuffer decoupling** -- `ddr3_framebuffer_480p` shrinks from ~1200
   lines of interleaved DDR3+display logic to a clean display module that
   talks through `mem_port_if`, matching `sdram_framebuffer.sv`'s structure

**Bandwidth budget with arbiter (all clients active):**

| Client | Mcyc/s @ 74.25 MHz | % Peak |
|--------|-------------------|--------|
| Framebuffer writes | 6.75 | 9.1% |
| Framebuffer reads | 10.1 | 13.6% |
| Shadow writes (CPU) | ~1.0 | 1.3% |
| Shadow reads (video gen) | 1.15 (0.29 with cache) | 1.5% (0.4%) |
| Ensoniq DOC reads | 0.57 (0.14 with cache) | 0.8% (0.2%) |
| Ensoniq GLU writes | <0.01 | <0.01% |
| DDR3 refresh | ~1.0 | 1.3% |
| **Total** | **~20.6 (~19.2 cached)** | **~27.7% (~25.9%)** |
| **Headroom** | **~53.7 (~55.1)** | **~72.3% (~74.1%)** |

With 128-bit burst caching, shadow and Ensoniq reads drop to <1% combined,
leaving 74% headroom for future expansion.

### Why BrianHG's COMMANDER Doesn't Solve This

The COMMANDER (`BrianHG_DDR3_COMMANDER_v16.sv`) is vendor-neutral
SystemVerilog, but it's tightly coupled to BrianHG's own PHY interface
(command sequencer signals), not to the Gowin DDR3 IP's `cmd`/`addr`/`wr_data`
interface. Using the COMMANDER would require:

1. A translation shim between COMMANDER output and Gowin IP input
2. Adapting the COMMANDER's timing assumptions (designed for Altera IO latency)
3. Testing SystemVerilog compatibility with GowinSynthesis
4. Carrying ~5000 lines of COMMANDER code when a ~300 line custom arbiter
   suffices

The custom arbiter approach is simpler, proven (it mirrors the working SDRAM
architecture), and directly targets the Gowin DDR3 IP's interface.
