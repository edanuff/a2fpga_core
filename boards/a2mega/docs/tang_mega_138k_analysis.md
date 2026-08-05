# Tang Mega 138K Module Analysis — Memory System Implications

*Analysis date: 2026-07-24. Status: analysis only — no bring-up work committed.*

## Scope

The Tang Mega 138K module is largely pin/form-factor compatible with the Tang
Mega 60K module currently used by the a2mega board. This document analyzes the
implications of moving a2mega to the 138K, with emphasis on the memory system:
the second DDR3 chip, latency and throughput effects, and which contemplated
workloads (Ensoniq-from-DDR3, 1080p DisplayPort output, SecondSight VGA
emulation) would actually benefit. It closes with the steelman case for the
32-bit DDR3 datapath and a recommended sequencing.

## Module differences

| | Tang Mega 60K (current) | Tang Mega 138K |
|---|---|---|
| FPGA | GW5AT-LV60B PG484 | GW5AST-LV138B (or -138C) **PG484 — same package** |
| Logic | 60K LUT4 | 138,240 LUT4 (measured, PnR report) |
| BSRAM | 118 blocks | **340 blocks** (measured, PnR report) |
| DDR3 | 1× H5TQ4G63EFR-RDC (x16, 512 MB) | 2× H5TQ4G63EFR-RDC (x32, 1 GB) |
| SerDes / DP | — | DisplayPort-capable hardware design (1080p) |

Key facts:

- **The second DDR3 chip is not a second memory.** Both chips share
  address/command/clock/CKE/ODT and a **single chip select** — they form one
  32-bit-wide array. Chip 2's 512 MB is *only* reachable through the x32
  datapath; there is no configuration that uses chip 2 for capacity without
  the full 256-bit user-interface rework.
- The 138K reference `top.sv` port comments are stale copy-paste (they list
  x16 widths beside 32-bit ports); conversely the current a2mega
  `top.sv:117-131` carries x32 comments beside its 16-bit ports. Declared
  widths are authoritative in both files.
- **Gowin license: verified covered.** Full syn → PnR → bitstream runs for
  `GW5AST-LV138PG484AC1/I0` (the exact `a2mega_138B.gprj` part, device
  version B, internal id `gw5ast138b-011`) and the FPG676 package variant
  completed 2026-07-24 under the current node-locked license
  (exp. 2027-06-10). No license action needed.

### Retarget work required regardless of DDR3 decisions

1. New device target (current IP generated for `gw5at60b-002`; 138K module
   is `gw5ast138b-011`).
2. CST — **resolved: the ball map carries over completely.** Verified
   against nand2mario's console60k/console138k reference CSTs (which
   differ only in attributes, not pins): the DDR3 chip-1 balls are
   IDENTICAL on both PG484 SOMs. The only required CST change is IO
   types — the GW5AST die has no `SSTL15_I`/`SSTL15D_I` (PnR CT1109);
   use `SSTL15`/`SSTL15D` (and DRIVE=8, matching the proven 138K
   reference). `hdl/a2mega_138b.cst` carries these deltas; keep its
   non-DDR3 sections in sync with `a2mega.cst`.
   One more die difference: **bank partitioning shifts between dies** (the
   ball grid is identical but bank membership is not — verify against the
   IDE's package database at
   `IDE/data/device/GW5AST-138B/PBGA484A.json`). Concretely: ball AB13
   (`button`) moved from Bank 9 (1.5 V, fine on the 60B) into **Bank 5**,
   which it shares with the 3.3 V UART pins (U15/V14) on the 138B —
   CT1136 bank-VCCIO conflict. Sipeed's 138K examples run Bank 5 at
   3.3 V, so `a2mega_138b.cst` constrains `button` as LVCMOS33 (it is an
   unused input, so this is safe regardless of the SOM's AB13 wiring).
   Separately, the phantom `ddr_addr[15]` (ball R1) was removed from
   top.sv and both CSTs — it corresponds to no DDR3 pin (the H5TQ4G63EFR
   has A[14:0]), Sipeed's own 138K DDR3 example comments it out, and
   nand2mario's 60K design never had it (`ddr_addr` is now `[14:0]` on
   both variants).
3. Regenerate Gowin IP for the new die: DDR3 interface (mandatory — the
   generated netlist bakes in device-specific PHY configuration), and the
   PLLs (verify; family primitives may carry over but regeneration is
   cheap insurance).
4. Timing re-validation; DDR3 eye calibration is handled by the IP.

### x16-first bring-up option

The IP can be regenerated for the new die at DQ_WIDTH=16, wiring only chip 1
and leaving chip 2's DQ/DQS/DM unconstrained. The chips share the command
bus: chip 2 harmlessly stores garbage on writes and drives its own
(unconnected) DQ nets during reads — no contention. This validates the new
die with the already-proven 128-bit datapath at zero HDL cost, and can remain
the permanent configuration unless a workload demands x32.

## Supporting both SOMs in the repo

**Decision: single `boards/a2mega/` directory with parallel `.gprj` files**
— not separate board directories. The chip type lives in the `.gprj`
`<Device>` line, so each FPGA variant needs its own project file, and the
repo already has the working precedent: `a2n20v2-Enhanced` ships three
`.gprj` variants (base / `_fb` / `_dualrate`) in one directory, differing
via per-variant file swaps and a per-variant SDC in the file list. Output
artifacts never collide because everything under `impl/` is
project-name-prefixed (`impl/pnr/<project>.fs`, `.rpt.txt`, …). Separate
board directories remain reserved for genuinely different hardware/HDL
(a2n20v2 vs -GS vs -Enhanced).

### Variant matrix

| Project file | Device | Notes |
|---|---|---|
| `a2mega.gprj` | GW5AT-60B (`gw5at60b-002`) | current hardware; remains the default build |
| `a2mega_138B.gprj` | GW5AST-138B (`gw5ast138b-011`) | created 2026-07 |
| `a2mega_138C.gprj` | GW5AST-138C | when C-revision hardware arrives |

The 138K exists in B and C die revisions; **bitstreams are not
interchangeable across die revisions**, so each revision needs its own
`.gprj` and its own released `.fs`. Since a revision differs by one
`<Device>` line (plus regenerated IP), the single-directory model scales to
this without friction.

### What must actually diverge per variant

1. **`<Device>` line** — the only current difference between `a2mega.gprj`
   and `a2mega_138B.gprj`.
2. **Gowin IP** — ⚠️ `a2mega_138B.gprj` currently points at
   `hdl/gowin/ddr3_memory_interface/` generated for `gw5at60b-002`. It may
   synthesize, but the DDR3 IP netlist bakes in device-specific PHY
   configuration and cannot be trusted on the 138B. Regenerate per device
   into a per-variant directory (e.g. `hdl/gowin_138b/`) and repoint the
   file list. Same treatment for PLLs if verification shows they differ.
3. **CST** — forked to `hdl/a2mega_138b.cst`, but for IO *attributes*, not
   pins: the ball map (DDR3 included) is identical across both PG484 SOMs;
   only the DDR3 IO types differ (`SSTL15_I`→`SSTL15`, `SSTL15D_I`→
   `SSTL15D`, DRIVE 12→8). When touching pins in `a2mega.cst`, mirror the
   change in `a2mega_138b.cst`.
4. **Top-level / x32 divergence (later)** — when the x32 datapath happens,
   the variant `.gprj` swaps `hdl/top.sv` for a variant top and picks up
   the widened parameters via its file list — the same mechanism the
   Enhanced `_fb` variant uses — keeping all core modules shared.

### PLL migration (implemented 2026-07)

The GW5AST-138B's PLLs are a different primitive generation than the
GW5AT-60B's PLLA, with three consequences (all handled; details below):

1. **clk_pll output ordering is hardware-forced.** PnR DRC (PA2078) gives
   the GW5AST PLL's envelope: PFD 19–81.25 MHz (so IDIV ∈ {1,2} from the
   50 MHz crystal) and VCO 650–1300 MHz. Exact 54 MHz = 50 × 27/25 needs
   VCO 1350 MHz for an integer divide — just over the ceiling (the 60B
   PLLA allowed it; that is its integer /50 /10 /25 config at VCO 1350).
   No legal integer configuration exists (verified empirically: VCO 540
   rejected for both PFD and VCO range; 810/1080 are not reachable from a
   50 MHz reference with n/8 fractional feedback), so **54 MHz must use
   the fractional divider, which exists only on ODIV0** — the generator
   GUI's insistence on 54/135/27 ordering was correct. Resolution: the
   generated `clk_pll.v` wrapper is cross-wired so the *ports* present the
   60B order (clkout0=27, clkout1=135, clkout2=54); which physical ODIV
   makes which frequency is invisible to top.sv.
2. **Fractional clk_logic needs a tighter constraint.** ODIV0 /12.5 is
   implemented as alternating /12 and /13 of the 675 MHz VCO — period
   alternates 17.78/19.26 ns. `hdl/a2mega_138b.sdc` therefore constrains
   clk_logic as an independent 56.25 MHz clock (worst-case short cycle);
   it is async-grouped from every other domain so the lost generated-clock
   relationship is harmless. Functional impact is nil (clk_logic is async
   to the Apple bus, DDR3 CDC, and video FIFOs); timing closes ~4%
   tighter. If this ever becomes a hardware problem, the clean fallback is
   a cascaded PLL (27 MHz → ×40 VCO 1080 → /20 = integer 54 MHz).
3. **pll_stop gets a first-class port.** The GW5AST PLL has per-output
   ENCLK gates. The DDR3 IP's pll_stop (idles HIGH = clock running; drops
   LOW during init phase alignment, IPUG281 §4.4.4) wires directly to
   enclk2 — `pll_ddr3.mod` regenerated with `-clock_en true`. This
   replaces the 60B's entire mDRP glue (`pll_mDRP_intf` + edge-pulse
   logic), which existed only because PLLA has no ENCLK. Polarity and
   wiring verified against nand2mario's shipped 138K design
   (ddr3_framebuffer_gowin/console138k). Note the generator's default ties
   ENCLK to VCC — the same silent pll_stop no-op that caused the 60B's
   intermittent dead-DDR3 cold boots; the regenerated MOD + wrapper
   exposes it properly.

Also: GW5AST PLLs need ICPSEL/LPFRES calibration from a `PLL_INIT`
companion. ⚠️ **PLL_INIT footgun**: there are two incompatible modules
named `PLL_INIT` — the old mDRP-style (GW5AT era) and the new direct-port
ICPSEL/LPFRES style (GW5AST) — and the IDE writes the companion file to
`<project>/src/pll_init.v` when generating PLL IP, silently overwriting
whatever is there (this clobbered the tracked 60B-era file during the 138B
regeneration). The new-style file now lives with its variant at
`hdl/gowin/138B/gowin_pll/pll_init.v` and `src/pll_init.v` is deleted; the
60B build uses no PLL_INIT at all (raw PLLA + `pll_mDRP_intf` — its
old-style copy in `hdl/gowin/60B/gowin_pll/` is inert and unreferenced).
After any future IDE-GUI IP regeneration, check whether the IDE recreated
`src/pll_init.v` and re-added it to the `.gprj`; if so, move the content
into the variant dir and repoint the file list. **The GW5AST die has no
PLLA resources at all** — confirmed empirically by PnR error RP0008
("There is no PLLA resource in current device") when the hand-written
PLLA `pll_usb.v` was tried. Every PLL on the 138B must be the PLL+PLL_INIT
style. `pll_usb` was regenerated accordingly (`pll_usb.mod` →
`GowinModgen`; VCO 900 = 50×18, /15 = integer 60 MHz) with the wrapper
keeping the 60B-identical port interface (PLL_INIT runs from clkin, which
is already the 50 MHz crystal).

**Structure**: all PLLs now live in a per-variant `board_plls` module —
`hdl/clocks_60b.sv` (PLLA + mDRP glue, relocated verbatim from top.sv) and
`hdl/clocks_138b.sv` (PLL + PLL_INIT, enclk2 direct) — selected by each
variant's `.gprj` file list. `top.sv` instantiates `board_plls` once and is
fully shared; the DDR3 reset sequencer + calibration watchdog stays in
top.sv (device-independent). Each variant has its own SDC
(`a2mega.sdc` / `a2mega_138b.sdc`) because the PLL instance paths and the
fractional clk_logic constraint differ.

IP regeneration is scriptable: `GowinModgen -do <file>.mod` regenerates a
MOD from its flag file (the `.mod` files are plain-text CLI arg lists).
The thin `clk_pll.v`/`pll_ddr3.v` wrappers carry hand edits (cross-wiring,
enclk pass-through) — re-apply them if the IDE ever regenerates the
wrappers from the `.ipc`.

### Building variants

`tools/build.sh` already supports this: `GPRJ=a2mega_138B.gprj
tools/build.sh a2mega`. The default (first `.gprj` alphabetically) resolves
to `a2mega.gprj`, so the 60K build remains the no-argument default. Update
the build skill notes and `docs/boards.md` when the 138B variant becomes
buildable (IP regenerated, CST verified).

## Latency: what the module swap changes (and doesn't)

**The wider bus does not reduce latency.** Every stage of a read round trip
is identical on both modules — same controller IP, same 324 MHz PHY /
81 MHz `clk_x1` / 54 MHz `clk_logic`, same CDC design:

| Stage | Time |
|---|---|
| Client request + CDC into 81 MHz (`ddr3_port_cdc.sv`) | ~45–60 ns |
| Arbiter grant when idle (`ddr3_ports.sv`) | ~25 ns |
| Controller command → `rd_data_valid` (~35 `clk_x1` cycles) | ~350–430 ns |
| Slot extract + CDC back to 54 MHz | ~50–70 ns |
| **Uncontended total** | **~500–580 ns** |

What x32 changes is **occupancy**: each BL8 burst carries 32 bytes instead of
16, so block-transfer clients need half the grants for the same data. The
latency floor doesn't move; the contention tail improves.

## Throughput: the serialized-arbiter ceiling

Verified against the FSM (`hdl/ddr3/ddr3_ports.sv`, `S_READ_CMD` →
`S_READ_WAIT` → `S_RESPOND` → `S_READ_CMD2`): the burst8 grant issues its
second command only after the first word's data has returned and all four
beats have drained — it pays the ~35-cycle controller latency **twice,
serially**. Per-grant cost is latency-dominated, so per-port read throughput
is `bytes-per-grant / grant-time`:

| Configuration | Payload/grant | Grant time | Per-port throughput |
|---|---|---|---|
| x16, today's FSM (2 serial commands) | 32 B | ~84 cyc | **~31 MB/s** |
| x32, same FSM unchanged | 64 B | ~84 cyc | ~62 MB/s |
| x16, overlap the 2 commands | 32 B | ~46 cyc | ~56 MB/s |
| x32 + overlap | 64 B | ~48 cyc | ~108 MB/s |
| x16, 8-deep overlapped chain | 128 B | ~72 cyc | ~144 MB/s |
| x32, 8-deep overlapped chain | 256 B | ~76 cyc | ~270 MB/s |
| x16, 16-deep chain | 256 B | ~104 cyc | ~200 MB/s |

Additional end-to-end cap: the response path delivers 32-bit beats to a
54 MHz client → **216 MB/s** hard ceiling through the existing per-port CDC,
independent of arbiter improvements. High-bandwidth read clients must take
data in the `clk_x1` domain (324 MB/s of 32-bit beats) or via a wider
sideband (see chain rework sketch below).

Wire-rate context: x16 = 1.296 GB/s theoretical (~0.8–0.9 GB/s realistic);
x32 = 2.6 GB/s theoretical.

## Workload analysis

### Ensoniq (DOC5503) from DDR3 — parked

`hdl/sound/doc5503.sv` requires the sample fetch within 7/8 of an oscillator
slot: **~978 ns** at the IIgs 7.159 MHz clock. Worst case through the current
stack, even with the DOC at top arbiter priority: in-flight grant drain
(~450–500 ns for a burst8) + possible refresh collision (tRFC = 260 ns,
every 7.8 µs) + own round trip (~550 ns) ≈ **1.0–1.2 µs** — at or past the
deadline with no margin, and *identical on both modules* (latency-bound, not
width-bound).

What would actually enable it (either module): a per-oscillator wavetable
line buffer — one 32-byte burst per oscillator, ~31/32 hit rate, ~1 KB of
fabric — decoupling the DOC deadline from the DDR3 tail.

The 138K inverts the motivation: `ensoniq_bsram.sv` (16K×32 = 64 KB)
measures **32 BSRAM blocks** — 27% of the 60K's 118 (a major contributor to
its 118/118 exhaustion) but only 9% of the 138K's 340. On the 138K, the DOC
stays in BSRAM and the question disappears. Ensoniq-on-DDR3 is a *60K*
space-recovery play only.

### 1080p DisplayPort output — pixel-domain work, not memory work

All framebuffer implementations use **RGB565** (see
`hdl/video/framebuffer_480p.sv`; lossless for all Apple II/IIgs/TMS9918A
sources). The shipped architecture stores the FB at **source resolution**
and scales at scan-out (each FB line fetched once, replayed across output
lines). Keeping that structure at 1080p:

- **Bandwidth does not scale with output resolution.** A 560×192 / 640×200
  source at 60 Hz remains ~13–15 MB/s of read traffic.
- **Fetch deadlines stay comfortable.** Each source line covers ~5 output
  lines (192→960, 200→1000, with borders): budget ≈ 74 µs vs ~36–41 µs
  fetch through even the *current* serialized arbiter (~2× margin,
  comparable to today's 480p margin).
- Integer ratios are clean: IIgs 640-mode ×3 = exactly 1920; 320-mode ×6 =
  exactly 1920; 560-wide ×3 = 1680 + borders; vertical ×5 for 192/200-line
  modes. Scanline dimming generalizes to a choice among the 5 repeats.

The real 1080p work is downstream of the line buffer: the 148.5 MHz pixel
domain (line-buffer read, horizontal scaler, timing closure), DP sink
timing, and a ~2×7.7 KB ping-pong line buffer. The full-raster alternative
(compose 1920×1080 in DDR3, scan out raw) costs ~250 MB/s on *each* of the
write and read sides and buys nothing the scan-out scaler can't do — it is
not required and not recommended.

### SecondSight VGA emulation (Oak OTI-087) — the first real consumer of arbiter work

The OTI-087 is a fully VGA-compatible SVGA controller (1 MB class) with
Oak's bank-switching extensions. Emulation scope is therefore **a VGA core**
— Graphics Controller datapath (4 write modes, set/reset, bit mask, rotate,
ALU, latches), sequencer, CRTC interpretation, attribute/DAC — plus thin Oak
extensions and the card-specific Apple-slot glue. MiSTer ao486's VGA core is
proven prior art for the VGA semantics.

Design conclusions:

- **VRAM layout**: store as the VGA's native **256K × 32-bit planar words**
  in DDR3 (1 MB; one 128-bit DDR3 word = 4 VGA addresses × 4 planes).
  Per-plane writes map onto existing `mem_port_if` byte enables. Every mode
  (planar 16-color, chain-4, packed 8bpp) is a scan-out serializer choice.
  Palette lookup at scan-out; the VGA DAC is RGB666 — exactly the pipeline's
  `COLOR_BITS = 18` internal format.
- **CPU access**: a write-through BSRAM window cache sitting *above* the GC
  datapath, caching planar words (read mode 1 color-compare and latch loads
  need all four planes). Solves the Apple-bus read deadline (~few hundred ns)
  cleanly; same class of problem as the DOC deadline but with a windowed
  access pattern that caches well.
- **Scan-out**: interpret CRTC registers (geometry, pitch, start address,
  line compare, pel panning) and map onto the fixed-1080p scaling scan-out;
  do not reproduce VGA timing on the wire. 1080 = 2³·3³·5 gives exact
  rational ratios for every mode; the 4:3 modes land at exactly 1440×1080.

Line-fetch requirements at 1080p (against the throughput ladder above):

| Mode | Scale | Line budget | Required @8bpp | Required @RGB565 |
|---|---|---|---|---|
| 320×200, 640×200/400 | ×5.4 / ×2.7 | 40–80 µs | ≤16 MB/s | ≤32 MB/s |
| 640×480 | ×2.25 | ~33 µs | ~19 MB/s | ~39 MB/s |
| 800×600 | ×1.8 | ~27 µs | ~30 MB/s | ~60 MB/s |
| 1024×768 | ×1.40625 | ~21 µs | ~49 MB/s | ~98 MB/s |

With native (≤8bpp) storage, everything through 800×600 sits at/under the
current ceiling; 1024×768 needs ~1.6× — cleared by the minimal
overlap-the-two-commands fix, with the 8-deep chain providing margin. No
mode approaches x16 chained capability or the 216 MB/s CDC cap.

Open items when this becomes real: whether Second Sight paired the OTI-087
with a HiColor RAMDAC (15-bit at 640×480 — structural non-event, adds a
serializer mode); and the Apple-slot interface mapping (I/O ports 3C0–3DF,
VRAM banking) — likely requires driver disassembly or surviving developer
docs.

## Chain rework sketch (`ddr3_ports.sv` read path)

Three parts; ordinary single-word ports keep the existing path untouched.

1. **Grant FSM**: replace the serial `S_READ_CMD/S_READ_WAIT/S_RESPOND/
   S_READ_CMD2` loop with one `S_READ_STREAM` state running two concurrent
   counters — a command issuer (N commands as fast as `cmd_ready` allows,
   address +16 per command, replacing the precomputed addr/addr2 pair with an
   incrementer) and an in-order response collector (count `rd_data_valid`).
   Grant retires when both reach N. An N=8 chain occupies ~50 cycles
   (~620 ns) — *shorter* than today's burst8 grant (~84 cycles / ~1.04 µs)
   while moving 4× the data, so every other port's worst-case wait improves.
   Chain length N is the fairness knob (FB throughput vs other-port
   blocking); parameter `READ_CHAIN_LEN` replaces `READ_BURST8_PORT`.
2. **Data plane — wide read sideband**: the per-beat CDC (32-bit @ 54 MHz,
   16-deep FIFO) cannot carry chained payloads (216 MB/s cap, overflow
   hazard). Mirror the existing `wide_wr_data_hi` precedent with a
   `wide_rd_data_o[127:0]` + `wide_rd_valid_o` channel in the `clk_ddr`
   domain, wired only to the chained port, writing directly into the FB
   line-buffer BSRAM (dual-clock, width-ratioed: 128-bit @ 81 MHz write,
   16-bit @ pixel clock read). CDC carries control only (request out,
   `req_done` back). Side benefit: FB request traffic drops ~8–16×,
   shrinking exposure to the still-open CDC request-tearing issue.
3. **Client** (`framebuffer_480p`): issue one request per chain instead of
   per burst8; beats arrive via the sideband; ping-pong structure and the
   "stall, never emit stale" discipline unchanged.

**Measurement prerequisite** (one afternoon, current 60K hardware): the
throughput ladder assumes the Gowin controller accepts sequential commands
near back-to-back and keeps rows open across a 2 KB page — its page policy
is undocumented. Use the idle `ddr3_debug_reader` port to issue back-to-back
reads and measure `cmd_ready` acceptance and `rd_data_valid` spacing via the
debug overlay before committing to the design.

## Capacity: the opportunity is already here

Current usage is ~10 MB of 512 MB. Chip 2's capacity is gated behind the x32
rework (shared CS), so capacity alone can never justify x32 — but the idle
~500 MB on chip 1 can host, today:

- **Whole-image disk caching** — stage entire `.hdv`/`.2mg` images (or a
  library) into DDR3 at mount; seeks become memory reads, SPI latency leaves
  the hot path, writes buffer for lazy flush. Highest-value use of idle RAM.
- **Memory-expansion card emulation** — Slinky-style (≤16 MB, pointer-
  sequential, latency-tolerant); a reset-surviving RAM disk with optional
  MCU-side persistence.
- **Save states / rewind** — 128 KB (II) to few-MB (IIgs) snapshots;
  thousands fit.
- **Video/audio capture** — ring-buffer the RGB565 stream, drain via
  Ethernet/USB.
- **Software library staging** — ROM sets / Total Replay-class collections
  at memory speed.

All are latency-tolerant, modest-bandwidth, capacity-hungry — served by the
existing x16 fabric and idle/low-priority ports.

## The steelman case for x32

The analysis above repeatedly concludes "x16 suffices." The honest case for
the 32-bit datapath, argued as strongly as the facts allow:

1. **It is the only throughput doubling with zero arbiter redesign.** Same
   FSM, same states, same CDC, same verification status — every alternative
   (overlap, chains, wide sideband) is new logic in the most
   debugging-expensive module in the system. The display-debug history (four
   root-caused hardware bugs; CDC request tearing still open) is the
   argument: arbiter changes cost weeks of hardware validation; a width
   change is parametric. If engineering time — not silicon — is the scarce
   resource, x32 buys 2× everywhere for the price of an IP regeneration and
   wire widening.
2. **Occupancy halves at fixed payload, improving *everyone's* tail.** A
   32-byte FB fetch drops from ~84 to ~46 cycles of arbiter occupancy. Every
   worst-case latency bound in the system — shadow-port response, the parked
   DOC deadline math, Apple-bus window-cache misses — improves because the
   blocking term ahead of any grant shrinks by half, and FB grant *frequency*
   halves too. Width is a system-wide tail-latency subsidy that no single
   client has to ask for.
3. **Concurrent-workload aggregation is real.** Single workloads fit x16,
   but the roadmap composes: SecondSight 1024×768 scan-out (~50–100 MB/s) +
   Apple-native rendering + shadow traffic + **1080p output capture**
   (ring-buffering the final 1080p RGB565 output stream is ~250 MB/s of
   *write* bandwidth) + disk-cache staging bursts. Aggregates in the
   350–450 MB/s range crowd x16's realistic envelope once arbitration
   efficiency is applied; x32 keeps every combination trivially inside
   budget. The most compelling single line item: full-rate capture/streaming
   of the 1080p output is only comfortable on x32.
4. **Chains and width multiply.** If the chain rework happens anyway (for
   SecondSight margin), x32 doubles its yield: 270 MB/s vs 144 MB/s at N=8.
   On x16, reaching 1080p full-raster numbers requires N≈16 chains pressing
   against page-boundary and fairness limits; on x32, N=8 clears them with
   headroom. Width halves how aggressive every future arbiter improvement
   must be.
5. **Display ambitions compound.** RGB888 pipelines (smoother scaler
   gradients), 2-tap-filtered scaling everywhere, higher-than-1080p DP modes
   the transceivers could support, phosphor-persistence effects reading back
   prior frames — each multiplies read bandwidth 1.5–2×. Any two together on
   x16 means rationing; x32 means not thinking about it.
6. **Capacity arrives free.** The 1 GB array makes the whole-library disk
   cache and deep save-state history unconditional rather than budgeted.

Counterweight (for balance): nothing on the *committed* roadmap requires
x32; the datapath rework (256-bit user IF, 8-slot extraction, mask/beat
logic, FB wide paths) is real work multiplying through validated code; and a
single x16 codebase avoids a permanent two-configuration test matrix.
(Mitigation if both configs must coexist: `ddr3_ports` already parameterizes
`DDR_DATA_WIDTH`/`DDR_MASK_WIDTH` — one codebase, two build configs is
plausible, at the cost of doubling the hardware test matrix.)

## Recommended sequencing

1. **138K bring-up at x16** — device/CST/IP retarget only; chip 2 dark.
   Validates the die with the proven datapath. (License already verified.)
2. **Controller characterization** on current hardware — page policy /
   back-to-back command experiment via `ddr3_debug_reader`. Derisks all
   chain math for one afternoon.
3. **1080p DP scan-out** — pixel-domain scaler + DP timing; DDR3 side
   unchanged (scale-at-scanout). First user-visible 138K win.
4. **Arbiter overlap fix** (cheap: issue burst8's second command before the
   first returns) — covers SecondSight through 1024×768 @ 8bpp.
5. **Chain rework + wide read sideband** — when SecondSight lands or capture
   is wanted; margin for everything else.
6. **x32 datapath** — trigger: 1080p output capture/streaming, RGB888
   pipeline, or measured aggregate pressure. Capacity features ride along.

Steps 1–3 deliver the competitive display-quality win (1080p DP) without
touching the validated memory fabric at all.
