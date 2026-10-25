# Memory System

> Status (2026-10-25): port maps re-read from every board top; HyperRAM backend and the
> Enhanced build variants added; the a2mega DOC ports are DDR3-backed.

How the core's clients reach off-chip memory: the common port interface, multi-port arbitration,
the per-board backends (SDRAM on the Tang Nano 20K, DDR3 on the a2mega, HyperRAM on the a2p25),
and the clock-domain crossings.

## One interface, three backends

Every memory client — video shadow RAM, the framebuffer, the Ensoniq wavetable, the
coprocessor's disk windows — talks to memory through the same interface,
[`mem_port_if`](../hdl/memory/mem_port_if.sv). What's behind it is board-specific and
**invisible to the client**:

- **Tang Nano 20K boards** (`a2n20v2-GS`, `a2n20v2-Enhanced`): SDRAM, via
  [`sdram_ports`](../hdl/sdram/sdram_ports.sv) → [`sdram.sv`](../hdl/sdram/sdram.sv). The
  classic `a2n20v2` build uses no off-chip memory.
- **a2mega** (Tang Mega 60K / 138K SOM): DDR3, via [`ddr3_ports`](../hdl/ddr3/ddr3_ports.sv) →
  the Gowin DDR3 IP (per-die, under `boards/a2mega/hdl/gowin/`).
- **a2p25** (Tang Primer 25K, displayport1 rev): a 64 Mbit HyperBus PSRAM, via
  [`hyperram_ports`](../hdl/hyperram/hyperram_ports.sv) → the in-house
  [`hyperram_ctrl`](../hdl/hyperram/hyperram_ctrl.sv) + [`hyperram_phy_gw5a`](../hdl/hyperram/hyperram_phy_gw5a.sv)
  (why not the Gowin IP: `boards/a2p25/docs/MODERNIZATION_PLAN.md` section 4).

All three `*_ports` arbiters present an array of `mem_port_if` ports, so client code
(framebuffer, DOC, shadow memory) is portable across the memory technologies; the a2p25 top
was derived from the a2mega top with the arbiter swapped and the same seven ports.

## The port interface (`mem_port_if`)

A client drives the `client` modport; the controller side drives `controller`.

| Signal | Dir (client) | Meaning |
|---|---|---|
| `addr` | out | Word address (each board applies a per-port base offset, see below). |
| `data` | out | Write data (`DATA_WIDTH`, default 16). |
| `byte_en` | out | Per-byte write mask (`DQM_WIDTH`). |
| `wr` / `rd` | out | Write / read request (pulse). |
| `burst` | out | When high, a read returns the controller's burst length. |
| `q` | in | Read data (`PORT_OUTPUT_WIDTH`, default `DATA_WIDTH*2`). |
| `available` | in | Port can accept a new request. |
| `ready` | in | One pulse per returned read beat (or write completion). |

**Semantics:** issue a read with `rd` (+`burst` for a multi-word fetch) or a write with `wr` +
`byte_en`; the controller answers with one `ready` pulse per beat, data on `q`. Because a burst
returns several words, `q` is wider than `data` and beats arrive across successive `ready`
pulses — the CDC (below) buffers them so the client consumes one per cycle. Addressing is
**word-based**; the controller converts to the physical byte/burst address.

## Multi-port arbitration

Each board instantiates one `*_ports` arbiter with `NUM_PORTS` client ports muxed onto the single
controller:

- **Static priority — lower port index wins.** Port 0 preempts port 1, etc.
- **`PORT_BASE_ADDR[]`** gives each port its own address window (applied inside the arbiter, so
  clients address from 0). Windows must not overlap or memory aliases.
- [`mem_if_mux`](../hdl/memory/mem_if_mux.sv) is a separate 2→1 stateless mux used where two
  clients share one port by a select line (not priority arbitration).

### Per-board port maps

The index assignment encodes the priority decision for that board.

**`a2n20v2-GS` (SDRAM)** — [top.sv:162](../boards/a2n20v2-GS/hdl/top.sv)
| Port | Use |
|---|---|
| 0 | Framebuffer line **reads** (highest — must not starve, or the scanout stalls) |
| 1 | Framebuffer pixel **writes** |
| 2 | Shadow/video RAM reads (video generators) |
| 3 | Shadow/video RAM writes (CPU) |
| 4 | Ensoniq DOC reads |
| 5 | Ensoniq GLU writes |

**`a2mega` (DDR3)** — `boards/a2mega/hdl/top.sv`, `u_ddr3_ports`
| Port | Use |
|---|---|
| 0 | Shadow/video RAM **reads** (highest) |
| 1 | Shadow/video RAM **writes** |
| 2 | Ensoniq DOC wavetable reads (above the framebuffer: tiny traffic, short grants) |
| 3 | GLU sound-RAM writes (above the framebuffer: a dropped write corrupts the wavetable) |
| 4 | Framebuffer pixel **writes** |
| 5 | Framebuffer line **reads** |
| 6 | DDR3 debug reader for the ESP32 (absolute addresses, rare traffic, lowest) |

**`a2p25` (HyperRAM)** — `boards/a2p25/hdl/top.sv`, `u_hr_ports`: the same seven clients, but
the **DOC wavetable read is port 0**. Behind continuous SHR shadow reads the DOC missed its
fetch budget often enough to be audible (the 2026-10-15 pops); the fix was priority 0 plus the
[`doc_fetch_cache`](../hdl/sound/doc_fetch_cache.sv) in front of the port. Shadow read/write
are 1/2, GLU write 3, framebuffer write/read 4/5, debug reader 6.

> The priorities differ between boards on purpose — each one is the tuning that keeps the
> scanout fed, the writes undropped and the DOC's deadline met on that memory. See
> [gotchas.md](gotchas.md), [memory_bandwidth_analysis.md](memory_bandwidth_analysis.md) and
> `boards/a2p25/docs/RESOURCE_AUDIT.md`.

**`a2n20v2-Enhanced` (SDRAM)** has three builds (see [boards.md](boards.md#the-three-enhanced-builds)):

| Port | classic and `_dualrate` (`top.sv`) | `_fb` (`VIDEO_FRAMEBUFFER`) |
|---|---|---|
| 0 | Shadow/video RAM reads | Framebuffer line reads |
| 1 | CPU shadow/aux writes | Framebuffer pixel writes |
| 2 | Ensoniq DOC reads | Ensoniq DOC reads |
| 3 | GLU sound-RAM writes | GLU sound-RAM writes |
| 4 | **Coprocessor** SDRAM port (`MCU_MEM_PORT`) | Shadow/video RAM reads |
| 5 | Disk II track-on-demand window | CPU shadow/aux writes |
| 6 | ProDOS HDD block window | MCU + Disk II + HDD, multiplexed by [`mem_port_arb`](../hdl/memory/mem_port_arb.sv) |

In the `_fb` build the DOC sits directly behind the framebuffer because a missed oscillator
fetch is an audible pop, while video reads and CPU writes tolerate latency there (the
framebuffer absorbs late line priming and the shadow write path queues through a FIFO).

## Clock domains & CDC

Memory runs faster than the 54 MHz logic clock, so every port crosses a clock domain:

- **SDRAM (Tang Nano 20K):** SDRAM at **108 MHz**, logic at **54 MHz**, related by `CLKDIV2`
  (every logic edge coincides with an SDRAM edge). Bridged per-port by
  [`mem_port_cdc`](../hdl/sdram/mem_port_cdc.sv). The SDRAM read clock **phase** is critical —
  a wrong PLL phase (`PSDA_SEL`) caused the GW2AR "ghosting" bug; see [gotchas.md](gotchas.md).
- **DDR3 (a2mega):** the DDR3 app clock (`clk_x1`) is **81 MHz** (324 MHz memory clock ÷ 4) and is
  **asynchronous** to the 54 MHz logic clock. Bridged per-port by an async CDC,
  [`ddr3_port_cdc`](../hdl/ddr3/ddr3_port_cdc.sv). On the GW5AT, async FIFOs must use block RAM
  (not FF arrays) or data bits corrupt — see [gotchas.md](gotchas.md).
- **HyperRAM (a2p25):** no port CDC at all. `hyperram_ctrl` runs on the 54 MHz logic clock and
  drives the HyperBus at 108 MHz from a PLL cascade (54 / 108 / 108+90°) inside the PHY; every
  port is on `clk_logic`. The 108 MHz rate was bench-qualified with the PSRAM eye-scan test
  (`boards/a2p25/docs/PSRAM_TEST.md`, zero errors).

> Note: some comments in [`ddr3_ports.sv`](../hdl/ddr3/ddr3_ports.sv) still describe an older
> "108 MHz / CLKDIV2-synchronous" DDR3 clocking; the live design is 81 MHz async (per `top.sv` and
> the `ddr3_port_cdc` header). Trust the wiring over those comments.

## The framebuffers

The board framebuffer is a memory client that owns the FB read+write ports and turns them into a
display scanout:

- [`sdram_framebuffer`](../hdl/video/sdram_framebuffer.sv) (GS and the Enhanced `_fb` build,
  720x480 HDMI), [`framebuffer_1080p`](../hdl/video/framebuffer_1080p.sv) (a2mega and a2p25:
  source-resolution lines scaled x3/x5 into a 1920x1080 DisplayPort raster, fed by the DP core's
  `cx`/`cy` pull), and [`framebuffer_480p`](../hdl/video/framebuffer_480p.sv) (the 480p variant
  the 1080p one was derived from).
- **Write side:** packs incoming pixels (2× RGB565 per 32-bit word) and writes them via the FB
  write port.
- **Read side:** prefetches each display line into a dual-port line buffer via the FB read port,
  then the display side reads the line buffer at the pixel clock. The line buffer is itself the
  CDC between the memory side and the pixel clock.
- This is the *consumer* end of the [video pipeline](video-pipeline.md)'s Apple-timed framebuffer
  path. Tuned parameters (burst length, buffer offsets, fetch timing) live in the code and in
  [a2mega/docs/archive/ddr3-framebuffer/ddr3_framebuffer_480p_design.md](../boards/a2mega/docs/archive/ddr3-framebuffer/ddr3_framebuffer_480p_design.md)
  / [a2n20v2-GS/docs/archive/WORKPLAN.md](../boards/a2n20v2-GS/docs/archive/WORKPLAN.md).

## Critical constraints (don't get bitten)

- **Do not modify [`sdram.sv`](../hdl/sdram/sdram.sv)** — it's tuned controller IP.
- The **Gowin "magic" SDRAM port names** in `top.sv` (`O_sdram_*`, `IO_sdram_dq`) are matched by
  the toolchain to the on-chip SDRAM; don't rename them.
- **CAS latency and the nanosecond timing parameters are board/device-specific** — changing them
  without re-running timing analysis risks corruption.
- The SDRAM framebuffer read burst width is constrained (a value that's too large deadlocks the
  controller). Respect the value in the instantiation; see [gotchas.md](gotchas.md).
- These and other memory traps are collected in [gotchas.md](gotchas.md).

## See also

- [memory_bandwidth_analysis.md](memory_bandwidth_analysis.md) — bandwidth budget per board/port.
- [gotchas.md](gotchas.md) — SDRAM phase, GW5AT async-FIFO/BSRAM, burst-length, port-priority traps.
- [video-pipeline.md](video-pipeline.md) — the framebuffer as a display-path consumer.
- [coprocessor-interface.md](coprocessor-interface.md) — the coprocessor's `mem_port_if` SDRAM port.
- [architecture.md](architecture.md) — where memory sits in the whole design.
