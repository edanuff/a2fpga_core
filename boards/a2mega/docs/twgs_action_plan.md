# TransWarp GS on A2Mega — Action Plan

**Goal:** Use the A2Mega's GS connector (65816 CPU-socket interposer) to implement a
TransWarp GS-style accelerator for the Apple IIgs, per the behavior model in
[twgs_reference.md](twgs_reference.md) and the hardware in
[sch/a2-mega.pdf](../sch/a2-mega.pdf).

**Status:** Planning. This document is the gap analysis and phased implementation plan.

---

## 1. Executive Summary

The A2Mega board already has the correct hardware topology for a TWGS-class
accelerator: it is simultaneously a **slot card** (Apple II bus via P1) and a
**CPU-socket interposer** (J8 40-pin header → ribbon cable → 65816 socket), with the
FPGA standing in for the removed 65C816. This is exactly the architecture the TWGS
reference identifies as correct ("CPU execution island" + native-speed bus fallback).

What exists today:

- **Hardware (schematic):** complete GS socket interface — 16 address lines, 8 data
  lines, R/W, and all control signals, level-shifted through 74LVC164245s (U8, U11)
  and an SN74LV1T125 (U10), with FPGA-controlled direction/output-enable lines.
- **HDL:** a copy of srg320's `P65C816` VHDL core plus a first-draft
  `iigs_65816_wrapper.sv` bus state machine in `hdl/twgs/`. **Neither is wired into
  `top.sv`, included in `a2mega.gprj`, nor pin-constrained in `a2mega.cst`.** The
  wrapper as written will not elaborate (port-name mismatch against the core, §3.2).
- **Docs:** the TWGS behavioral reference (register model, speed matrix, slowdown
  rules, cache/write-back model).

What has to happen, in order:

1. **Phase 0 — Groundwork:** resolve open hardware questions (bank voltages, /VP
   direction), core provenance, project hygiene, simulation strategy.
2. **Phase 1 — Passive bring-up:** pin constraints + tri-stated top-level ports +
   signal monitoring. No driving of the socket.
3. **Phase 2 — Transparent CPU replacement at native speed.** The IIgs boots and runs
   identically to a stock 65C816, CE-locked 1:1 to motherboard PH2. **This is the
   critical milestone; everything after it is incremental.**
4. **Phase 3 — Speed-control infrastructure:** `$C074`, speed matrix, motherboard
   speed tracking. Still no acceleration.
5. **Phase 4 — Acceleration engine:** fast internal execution against shadow
   memory/cache with a write-back FIFO to the motherboard.
6. **Phase 5 — Coherency & safety:** auto-slowdown, DMA handling, IRQ-aware
   throttling.
7. **Phase 6 — Software interface & UX:** ZipGS registers, CDA/control panel, ESP32
   web config, DebugOverlay status.
8. **Phase 7 — Validation:** compatibility matrix, benchmarks, timing closure, soak.

---

## 2. What the Hardware Gives Us (Schematic Analysis)

### 2.1 Topology

- **J8** (sheet 8, "GS 65816 Connector"): 40-pin 2.54 mm header, ribbon-cabled to the
  IIgs 65816 socket. Carries `GS_A[0..15]` (75 Ω series), `GS_D[0..7]` (47 Ω series),
  `GS_RW`, `GS_PH2` (27 Ω series), `GS_RESET`, `GS_IRQ`, `GS_NMI`, `GS_ABORT`,
  `GS_RDY`, `GS_BE`, `GS_VP`, plus `VDA`, `MX`, `VPA` (marked no-connect on the
  schematic), `MLB`, and `E`.
- **U8** (74LVC164245, dual 8-bit, split rails 3V3/5V): port 1 = `GS_D[0..7]`
  bidirectional, direction from `FPGA_GS_D_DIR`; port 2 = control inputs
  (`GS_RESET`, `GS_VP`, `GS_RDY`, `GS_ABORT`, `GS_PH2`, `GS_IRQ`, `GS_BE`, `GS_NMI`)
  into the FPGA. Enables from `FPGA_GS_OE` / `FPGA_GS_DATA_OE`.
- **U11** (74LVC164245): `GS_A[0..15]` driven **from** the FPGA to the socket
  (the FPGA is the CPU and owns the address bus). Enable from `FPGA_GS_ADDR_OE`.
- **U10** (SN74LV1T125): drives `GS_RW` from `FPGA_GS_RWD`, gated by
  `FPGA_GS_ADDR_OE`.

The FPGA therefore has everything needed to *be* the 65816: it drives A/D/RW with
proper 5 V levels and receives PH2/RDY/IRQ/NMI/ABORT/BE/RESET, with independent
output-enable control so the board is safe (tri-stated) until firmware takes over.

### 2.2 GS connector FPGA pin map (extracted from sheet 6, BTB2/J6)

Address bus (BANK4):

| Signal | Ball | Signal | Ball |
|---|---|---|---|
| FPGA_GS_A0 | V17 | FPGA_GS_A8 | AB20 |
| FPGA_GS_A1 | W17 | FPGA_GS_A9 | AA21 |
| FPGA_GS_A2 | Y18 | FPGA_GS_A10 | AB22 |
| FPGA_GS_A3 | Y19 | FPGA_GS_A11 | Y22 |
| FPGA_GS_A4 | AA18 | FPGA_GS_A12 | Y21 |
| FPGA_GS_A5 | AB18 | FPGA_GS_A13 | AB21 |
| FPGA_GS_A6 | W19 | FPGA_GS_A14 | AA20 |
| FPGA_GS_A7 | W20 | FPGA_GS_A15 | AA19 |

Data bus and control:

| Signal | Ball | Bank | Dir (FPGA) | Signal | Ball | Bank | Dir (FPGA) |
|---|---|---|---|---|---|---|---|
| FPGA_GS_D0 | N15 | 4 | bidir | FPGA_GS_NMI | N13 | 4 | in |
| FPGA_GS_D1 | M15 | 3 | bidir | FPGA_GS_BE | N14 | 4 | in |
| FPGA_GS_D2 | M16 | 3 | bidir | FPGA_GS_IRQ | Y17 | 5 | in |
| FPGA_GS_D3 | N17 | 4 | bidir | FPGA_GS_PH2 | AB17 | 5 | in |
| FPGA_GS_D4 | V20 | 4 | bidir | FPGA_GS_ABORT | AA16 | 5 | in |
| FPGA_GS_D5 | U20 | 4 | bidir | FPGA_GS_RDY | AB16 | 5 | in |
| FPGA_GS_D6 | M17 | 3 | bidir | FPGA_GS_VP | AA15 | 5 | in (⚠ §7.1) |
| FPGA_GS_D7 | P17 | 4 | bidir | FPGA_GS_DATA_OE | AB15 | 5 | out |
| FPGA_GS_RW | U18 | 4 | out | FPGA_GS_ADDR_OE | Y16 | 5 | out |
| FPGA_GS_RESET | U17 | 4 | in | FPGA_GS_OE | W15 | 5 | out |
| FPGA_GS_D_DIR | W21 | 4 | out | | | | |

> ⚠ Verify ball numbers against the KiCad netlist before committing the `.cst`; the
> table above was transcribed from the schematic PDF. See also the Bank 5 VCCIO
> question in §7.2 — do not power a connected IIgs until resolved.

---

## 3. What Exists in the Repo Today — Inventory & Defects

### 3.1 Inventory

| Item | Path | State |
|---|---|---|
| 65C816 soft core (srg320, VHDL, 7 files) | `hdl/twgs/65C816/` | Present, not in `.gprj` |
| Bus interposer wrapper | `hdl/twgs/iigs_65816_wrapper.sv` | Draft; not instantiated anywhere; has defects |
| TWGS behavior reference | `docs/twgs_reference.md` | Complete |
| GS pin constraints | `hdl/a2mega.cst` | **Absent** — no `FPGA_GS_*` pins |
| Top-level GS ports / integration | `hdl/top.sv` | **Absent** |
| Slot-bus side (apple_bus, apple_memory, DDR3, ESP32, HDMI) | various | Working base design |

### 3.2 Known defects in the existing draft

1. **Wrapper/core port mismatch — will not elaborate.** The wrapper instantiates
   `P65C816` with a `.VP(cpu_vp)` port; the entity's port is `VPB` (active-low,
   already inverted). The wrapper also omits `RDY_OUT` and `VPB` vs `VP` polarity
   handling (`vp_n <= ~cpu_vp` would double-invert).
2. **BE is not handled.** The wrapper never tri-states address/data/RW in response to
   `GS_BE` (bus release for DMA). It also has no ports for the level-shifter
   controls (`FPGA_GS_ADDR_OE`, `FPGA_GS_DATA_OE`, `FPGA_GS_OE`, `FPGA_GS_D_DIR`) —
   currently only a single `data_oe`.
3. **`RDY_IN` is tied to `1'b1`** at the core while the FSM separately implements a
   WAIT state; the interaction between the two RDY paths needs a single design.
4. **CDC latency budget is unanalyzed.** The 3-FF PH2 synchronizer at 54 MHz adds
   ~37–56 ns of detection latency before outputs change (plus output register +
   translator tpd). A real W65C816S provides address ≤ 40 ns after PHI2 fall. See
   §5, Phase 2 task 2.2.
5. **`a2mega.gprj` uses absolute paths** (`/Users/edanuff/...`) for all shared-core
   files, violating repo rule #4 (relative paths only). Any new files added for this
   work must not follow that pattern — and the existing paths should be repaired
   first so the project builds anywhere.
6. **Core provenance/licensing is undocumented.** The `P65C816` core is srg320's
   65C816 (as used in MiSTer SNES/IIgs cores). Files carry no license headers and the
   repo has no note of origin. Needs an attribution/license decision before release.

---

## 4. Gap Analysis (Reference Model → Implementation)

Mapping `twgs_reference.md` §14 ("FPGA Implementation Notes") onto the board:

| TWGS requirement | A2Mega implementation vehicle | Exists? |
|---|---|---|
| CPU socket interposition | J8 + U8/U10/U11 + soft 65C816 | HW yes; HDL draft |
| `$C074` speed register | Decode on soft-CPU's own address bus | No |
| 8–32 KB cache (BSRAM) | GW5AT-60 has ~4.6 Mb BSRAM; can hold far more than 32 KB | No |
| Write-back buffer | FIFO + native-speed bus cycle generator | No |
| Auto-slowdown (I/O, slots, DMA) | Address decode of own CPU + slot-side `a2_dma_n` visibility | No |
| IRQ monitoring | Soft core exposes P-register I-flag directly (better than TWGS's external inference) | No |
| Selective DMA cache invalidation | `GS_BE` + snooping during bus release | No |
| Speed matrix ("Normal speed = no acceleration") | Track `$C036` writes + stretched-cycle detection | No |
| ZipGS `$C05A`/`$C05B` compat | Same decode path as `$C074` | No |

Unique advantages over the original TWGS worth exploiting:

- **We see every CPU access** (we *are* the CPU), so slowdown heuristics and I/O
  detection are exact, not inferred from the slot.
- **We also sit in a slot**, so we see the DMA daisy chain and `M2SEL/M2B0`
  from the other side — useful for cross-checking DMA and Mega II cycles.
- **Abundant memory:** full ROM shadow and large RAM shadow are feasible (BSRAM +
  DDR3), versus TWGS's 8–32 KB tag cache. This can eliminate most cache-tag
  complexity (see ADR-1, §6).

---

## 5. Phased Implementation Plan

### Phase 0 — Groundwork (no hardware risk)

| # | Task | Deliverable |
|---|---|---|
| 0.1 | Fix `a2mega.gprj` absolute→relative paths (pre-existing rule violation; blocks anyone else building) | Buildable `.gprj`; verify with `/build` per `docs/setup-gowin-cli.md` |
| 0.2 | Resolve `P65C816` provenance & license; add attribution headers or replace core | Note in `docs/` + headers |
| 0.3 | Answer open hardware questions (§7): work the TWGS-schematic checklist (§7.1 — /VP, **VDA/VPA**, E, M/X, BE, cable pin order), then Bank 5 VCCIO (§7.2) | Updated twgs docs; go/no-go for Phase 1 power-on |
| 0.4 | Choose simulation approach for mixed VHDL/SV (the core is VHDL). Options: nvc/GHDL for core-only opcode tests + Verilator for the SV bus engine with the core swapped for a behavioral stub; or a commercial mixed-language sim if available | `sim/` scaffold + CI-runnable smoke test |
| 0.5 | Record architecture decisions as ADRs in `docs/adr/` (see §6) | ADR-1..4 |
| 0.6 | Baseline resource/timing snapshot of current a2mega build (LUT/BSRAM/Fmax headroom for the core + engine) | Numbers in this doc |

### Phase 1 — Passive bring-up (board connected, FPGA never drives the socket)

| # | Task | Notes |
|---|---|---|
| 1.1 | Add all `FPGA_GS_*` pins to `a2mega.cst` (§2.2 table) with IO_TYPE per resolved bank voltages | Outputs default tri-state |
| 1.2 | Add GS ports to `top.sv`; hard-assert all three OE lines inactive at reset and until a "TWGS enable" config bit is set | Safety invariant: a bitstream with TWGS disabled must be electrically identical to today's board |
| 1.3 | GS signal monitor module: count PH2 edges, measure fast/slow frequency (2.864 / 1.023 MHz), detect stretched cycles, observe RESET/IRQ/NMI/BE activity | Surface via DebugOverlay + UART |
| 1.4 | Bench validation with the ribbon attached to a socketed IIgs **with the real 65C816 still installed on the motherboard side of the interposer** if the cable permits, else scope-only | Acceptance: measured PH2 matches expectations incl. cycle stretching |

### Phase 2 — Transparent CPU replacement at native speed  ← **critical milestone**

The FPGA is the CPU; the machine must boot and behave exactly like stock. No
acceleration, no cache. Everything in later phases degrades to this mode.

| # | Task | Notes |
|---|---|---|
| 2.1 | Fix wrapper: correct `P65C816` port map (`VPB`, `RDY_OUT`), single coherent RDY design, BE-driven tri-state, level-shifter OE/DIR ports, reset semantics | Rewrite `iigs_65816_wrapper.sv`; keep the PHI1 bank-address multiplex (it is required — FPI latches the bank from D0–D7 at PH2 rise) |
| 2.2 | **Timing budget analysis** (design doc): W65C816S provides tADS/tBAS ≤ 40 ns after PH2 fall; our chain is PH2 sync (2–3 × 18.5 ns) + output reg + IOB + 164245 tpd ≈ 60–90 ns. **Confirmed** (Apple IIGS Tech Note #68, *Tips for I/O Expansion Slot Card Design*): the FPI captures the bank address in a latch that is *transparent while PH2 is low and closes at PH2 rise*, and holds the latch closed while RDY is low because the CPU emits an invalid bank address then; RDY must change only while PH2 is high. So the hard deadline for address + bank is PH2 rise (~175 ns after fall in fast mode), not the 40 ns a real chip provides — our 60–90 ns chain fits, but verify slow/stretched-cycle cases too. If margin is thin: sample PH2 on a faster clock (existing 135 MHz `clk_pixel_x5` or a dedicated PLL tap), use IOB registers, or clock the GS engine from PH2 directly via a BUFG | Doc + chosen mechanism; add `set_input_delay`/`set_output_delay` SDC constraints for the GS pins; bus engine must never change its RDY-dependent behavior mid-PHI1 |
| 2.3 | Bus-cycle engine v1: CE-gate the core 1:1 with PH2 (one core step per bus cycle), PHI1 bank mux, write-data drive during PH2 high, read sample at PH2 fall, RDY stretch, `GS_BE` release | Evolution of current FSM |
| 2.4 | Data-bus turnaround: dead-time states around `FPGA_GS_D_DIR` flips so the 164245 never fights the motherboard | Bus-contention checklist |
| 2.5 | Simulation: bus-functional model of the FPI (fast/slow/stretched cycles, RDY, DMA/BE) + opcode-level tests of the core (65C816-aware test suites) + full-cycle traces compared against a known-good emulator (e.g., recorded boot traces) | Regression suite in `sim/` |
| 2.6 | Integrate into `top.sv` + `.gprj` (relative paths), build, **check timing per repo rule #2** | Bitstream |
| 2.7 | Hardware bring-up ladder: (a) free-run with socket disconnected, verify vectors on debug overlay; (b) connected, watch reset vector fetch `$00FFFC/D` and first ROM fetches; (c) self-test (`Cmd-Opt-Reset`); (d) boot GS/OS; (e) run IIgs diagnostics | Debug: last-N address/opcode trace buffer readable over UART/ESP32 |

**Acceptance:** IIgs ROM self-test passes; GS/OS 6.0.1 boots; ProDOS 8 + IIe-mode
software runs; machine is indistinguishable from stock at both Normal and Fast
system speeds, including RDY-stretched and DMA cycles.

### Phase 3 — Speed-control infrastructure (still 1:1 execution)

| # | Task |
|---|---|
| 3.1 | Track motherboard speed state: snoop own CPU writes to `$C036` (speed bit), observe stretched-cycle pattern to infer effective speed |
| 3.2 | Implement `$C074` register (write $00 = max, $01 = 1 MHz) decoded from the soft CPU's own bus; reads must not disturb motherboard (register lives in the FPGA) |
| 3.3 | Enforce the TWGS speed matrix: System Normal ⇒ acceleration locked out |
| 3.4 | Config plumbing: enable/speed via DIP switch + ESP32 OSPI protocol; status on DebugOverlay |

### Phase 4 — Acceleration engine

Recommended architecture (ADR-1): **full shadow memory + write-through FIFO** rather
than a TWGS-style small tag cache — memory is plentiful and it removes tag/valid
corner cases:

- **ROM shadow:** copy motherboard ROM (128/256 KB) into DDR3/BSRAM at boot (or lazily,
  read-miss fill); ROM reads then never touch the bus.
- **RAM shadow:** shadow banks `$00/$01` (and optionally expansion RAM) in
  BSRAM/DDR3 with per-page valid bits, lazily filled on first read.
- **Writes are always replayed to the motherboard** (write-through) via a FIFO
  drained by the bus-cycle generator at legal bus speed. This preserves Mega II
  video shadowing, soft-switch side effects, and DMA-visible memory for free.
- **Never accelerate:** `$C000–$C0FF` I/O (strict), banks `$E0/$E1` Mega II space
  (reads and writes at native speed initially; optimize later), any page marked
  uncacheable.

| # | Task |
|---|---|
| 4.1 | Cacheability/shadow map with soft-switch awareness (LC banking, shadowing, `$C02D` slot ROM switches, state read from own CPU's soft-switch writes — reuse/extend `a2mem_if` logic which already tracks these for the slot side) |
| 4.2 | Decouple core clocking: run core on `clk_logic` with CE duty control (target 7–14 MHz effective first; headroom later), or async run-ahead with handshake to the bus engine |
| 4.3 | Write-back FIFO + ordering rules: drain-before-I/O barrier, read-after-write hazard on uncached space, FIFO-full stall |
| 4.4 | Bus-cycle generator: native-speed cycle replay (writes, uncached reads) synchronized to PH2, including slow (1 MHz) cycles for `$E0/$E1`/I-O per FPI stretching |
| 4.5 | Bus parking policy while executing internally: hold a benign ROM address with R/W=read so FPI-visible cycles are side-effect-free; validate no I/O location is ever parked on |
| 4.6 | Benchmarks: instruction-mix micro-benchmarks + period speed tests; target ≥ 2.5× stock at first release |

### Phase 5 — Coherency & safety

| # | Task |
|---|---|
| 5.1 | Auto-slowdown triggers per TWGS §10: any `$C0xx` access, slot device windows (`$C090–$C0FF`, slots 4–7 configurable), during and shortly after disk I/O; implement as a throttle timer dropping to Fast/Normal |
| 5.2 | DMA: the 65816 sits on the FPI's *fast side*; slot DMA happens on the Mega II *slow side*, so the halt mechanism visible at the CPU socket is expected to be **RDY deassertion / clock stretching, not BE** (BE is likely strapped high on the motherboard — verify against the TWGS schematic, §7.1). Honor RDY exactly per the FPI rule (changes only while PH2 high). Detect DMA from the **slot side** — `a2_dma_n` daisy chain into `apple_bus` — just as the original TWGS did from slot 3 (GAL8: "slot decoding, DMA/RDY"). For shadow coherency, snoop DMA write addresses via the slot-bus interface (it samples the bus independently of the CPU) and invalidate touched pages; conservative fallback: invalidate all shadowed RAM on any DMA burst. Keep `GS_BE`-triggered tri-state as a defensive path regardless |
| 5.3 | IRQ-aware throttling (TWGS "AppleTalk/IRQ ON"): when I-flag set (interrupts disabled) and code is polling I/O, drop to non-accelerated speed; direct P-register visibility makes this exact |
| 5.4 | WAI/STP semantics: use core `RDY_OUT` to idle the bus engine correctly; wake on IRQ/NMI/ABORT |
| 5.5 | ABORT support pass-through (the IIgs uses /ABORT for the Mega II?—verify actual usage; ensure at minimum it behaves as stock) |

### Phase 6 — Software interface & UX

| # | Task |
|---|---|
| 6.1 | ZipGS-compatible `$C05A/$C05B` registers (unlock sequence, speed nibble, cache/status bits) for third-party software compatibility |
| 6.2 | Control Panel integration: CDA or classic desk accessory to set TransWarp speed (mirrors original TWGS UX); stored via ESP32 config |
| 6.3 | ESP32 web UI page: enable/disable, speed, per-slot slowdown, AppleTalk-safe toggle |
| 6.4 | DebugOverlay: current speed, cache/shadow hit rate, FIFO depth, slowdown cause |

### Phase 7 — Validation & release

| # | Task |
|---|---|
| 7.1 | Compatibility matrix: GS/OS 6.0.1 & System 5, ProDOS 8, IIe-mode titles, timing-sensitive games, AppleTalk, 3.5"/5.25" disk I/O, memory expansion, self-test |
| 7.2 | Regression: confirm the slot-card feature set (video/HDMI, sound, Mockingboard, SSC, disk) is unaffected with TWGS enabled and disabled |
| 7.3 | Timing closure on both Tang Mega 60K and 138K SOM variants; 0 violated endpoints, TNS = 0.000 (repo rule #2) |
| 7.4 | Long-run soak (hours) at max speed; thermal check on the 164245s and SOM |
| 7.5 | Update `boards/a2mega/TODO.md`, `docs/boards.md`, ROADMAP; user-facing install doc (CPU removal, ribbon orientation, slot choice) |

---

## 6. Design Decisions To Record (ADRs)

1. **ADR-1 — Shadow-memory vs tag-cache acceleration.** Recommendation: full shadow +
   write-through FIFO (§ Phase 4) — simpler coherency, exploits abundant BSRAM/DDR3;
   revisit a smaller true cache only if DDR3 latency hurts.
2. **ADR-2 — GS engine clocking.** CE-locked on `clk_logic` (54 MHz) vs dedicated
   fast-sampling domain for PH2 edges. Driven by the Phase 2.2 timing analysis.
3. **ADR-3 — Where soft-switch state lives.** Reuse the slot-side `a2mem_if`
   tracking vs a dedicated CPU-side decoder (the CPU-side sees strictly more; the
   slot side is a cross-check).
4. **ADR-4 — 65C816 core choice.** Keep srg320 `P65C816` (proven in MiSTer IIgs
   usage, but VHDL + license TBD) vs alternatives. Default: keep, validate with
   opcode suite, document provenance.

---

## 7. Risks & Open Hardware Questions (resolve in Phase 0)

### 7.1 Verification checklist against the original TWGS schematic

The authoritative cross-check for every socket-side question below is Geoff Body's
reverse-engineered TWGS schematic
([Schematic Release 1, ReActiveMicro](https://downloads.reactivemicro.com/Apple%20II%20Items/Hardware/TransWarp_GS/2016-01-17%20-%20ReActiveMicro%20-%20Transwarp%20GS%20-%20Geoff%20Body%20-%20Schematic%20Release%201.pdf),
mirrored [at apple2.org.za](https://mirrors.apple2.org.za/ftp.apple.asimov.net/documentation/hardware/accelerators/transwarp-gs---schematic.pdf)).
TWGS shipped and was compatible; whatever it drives or ignores at the CPU socket is
the proven-sufficient signal set. **This PDF could not be fetched from the cloud
sandbox (outbound network policy blocks these domains), so the following must be
checked by someone with the file — recommend committing it to
`boards/a2mega/docs/reference/` alongside this plan.** For each CPU-cable pin,
record: driven by TWGS toward motherboard / received / not connected:

| Check | Why it matters for a2mega |
|---|---|
| Does TWGS drive **/VP** down the cable? | a2mega routes `GS_VP` *into* the FPGA through U8's input-only group, so nothing can drive /VP to the motherboard. If TWGS drives it, a2mega needs a board rev or bodge; if TWGS leaves it n/c, close this risk. |
| Does TWGS drive **VDA / VPA**? | **J8 marks VDA and VPA as no-connect.** If the FPI qualifies cycles with VDA/VPA (they exist precisely for memory-cycle qualification), floating them is either fatal (machine ignores all cycles) or dangerous (dummy/internal cycles get treated as real accesses to soft-switch/I-O addresses). This is potentially a *harder* problem than /VP — a real 65816 emits internal-operation cycles with garbage addresses that only VDA/VPA=00 marks as ignorable, and our "bus parking" plan (Phase 4.5) assumes the same escape hatch exists or isn't needed. |
| Does TWGS drive **E** and **M/X**? | J8 shows E and MX present on the header but seemingly unwired. The soft core tracks E internally; if TWGS drives E, we need a path to present it. |
| Is **BE** in the TWGS cable, and is it driven by the motherboard? | Determines whether socket-side DMA release is even a thing on the IIgs (see §5.2 — expectation is that RDY, not BE, is the halt mechanism, with BE strapped high). |
| How does TWGS connect **RDY** (bidirectional? pulled?) | RDY is also a 65816 *output* during WAI; check whether TWGS buffers it one-way. |
| Bank-address latch parts (74HCT573s) between card CPU and cable | Confirms the PHI1 bank-mux discipline and gives a reference implementation for our turnaround timing; search snippets confirm 573-family latches and a wire-link (U28 p12 → U44 p2) exist on the card. |
| GAL1 (CPU clock) / GAL2 (write-back) / GAL8 (slot decode, DMA/RDY) input signals | GAL8's inputs show exactly which *slot* signals TWGS used for DMA/slowdown — the template for our `apple_bus`-side detection. Note TWGS's 8 GALs replace ~40 discrete 74-series ICs ([ReActiveMicro wiki](https://wiki.reactivemicro.com/TransWarp_GS)). |
| CPU cable pin order | ReActiveMicro documents that the TWGS CPU cable is **not straight-through**. Independently verify J8 ribbon pin-mapping against the DIP-40 socket before first connection. |

What *was* confirmed this round (Apple IIGS Tech Note #68, expansion-card design):
the FPI bank-address latch is transparent during PH2-low and closes at PH2 rise;
RDY low ⇒ CPU bank address invalid, so the FPI freezes the latch; RDY may only
change while PH2 is high. These are now baked into Phase 2.2/5.2.

### 7.2 Other open items

1. **Bank 5 VCCIO conflict.** GS control pins (PH2, IRQ, ABORT, RDY, VP, and all
   three OE/DIR outputs) sit on SOM "BANK5" balls. The existing `.cst` constrains
   `button` (AB13) as LVCMOS15/`BANK_VCCIO=1.5` yet `uart_tx` (U15, also labeled
   BANK5 on the SOM schematic) as LVCMOS33. The 164245 A-side drives 3.3 V. Confirm
   from the Tang Mega 60K SOM schematic which BTB2 balls are on which bank and at
   what VCCIO **before first power-on with the ribbon attached** — a 3.3 V drive
   into a 1.5 V bank would damage the FPGA.
2. **Bus parking while accelerated.** Unverified assumption that the FPI tolerates
   repeated benign read cycles (it should — that's what a spinning CPU looks like),
   and that Mega II refresh is CPU-independent. Validate early in Phase 4.
3. **CDC latency vs FPI setup expectations** (§ Phase 2.2). If 54 MHz sampling is
   too slow, mitigation paths exist (faster sample clock, IOB regs, PH2-derived
   clocking) but they touch clock architecture — schedule the analysis first.
4. **Core accuracy edge cases:** decimal mode, emulation-mode quirks, open-bus
   reads, interrupt timing. Mitigated by the Phase 2.5 opcode/trace test suites.
5. **Licensing** of the imported core (§3.2 item 6).
6. **Dual-role interactions:** the same FPGA is a slot peripheral (drives `a2_d`,
   IRQ, INH) and the CPU. Auditing for feedback loops (e.g., CPU reading a register
   the slot side is serving in the same cycle) is required in Phase 2.6 — this is
   actually the common case, since every slot-card access *is* our own CPU cycle
   seen from both sides.

---

## 8. Resource Budget (to confirm in Phase 0.6)

| Item | Estimate | GW5AT-60 capacity |
|---|---|---|
| P65C816 core | ~3–4 k LUT | 59 k LUT total; current design usage TBD |
| Bus engine + FIFOs + decode | ~1–2 k LUT | |
| ROM shadow (256 KB) | DDR3 (exists) or 2 Mb BSRAM | ~4.6 Mb BSRAM total (framebuffer shares) |
| Bank $00/$01 shadow (128 KB) | 1 Mb BSRAM | |
| Trace/debug buffer | 1–2 BSRAM blocks | |

The 138K SOM variant removes any pressure if the 60K gets tight.

---

## 9. Suggested Milestone Sequence

| Milestone | Contents | Exit criteria |
|---|---|---|
| **M0** | Phase 0 | Open HW questions answered; project builds from clean clone |
| **M1** | Phase 1 | Signals verified on real IIgs; no drive |
| **M2** | Phase 2 | **IIgs boots on the soft CPU at stock speed** |
| **M3** | Phase 3 | Speed registers live; matrix enforced; still 1:1 |
| **M4** | Phase 4 | ≥ 2.5× benchmark, GS/OS stable |
| **M5** | Phases 5–7 | DMA/AppleTalk/disk clean; compat matrix green; release |

M2 is the schedule anchor: it retires the majority of technical risk (timing,
level shifters, core accuracy) while remaining fully reversible for users.

---

## 10. References

- [twgs_reference.md](twgs_reference.md) — TWGS behavior model this plan implements
- [sch/a2-mega.pdf](../sch/a2-mega.pdf) — board schematic (sheets 6 & 8 for GS path)
- `hdl/twgs/` — existing core + wrapper draft
- **TWGS schematic** (Geoff Body, Release 1, 2016) —
  [ReActiveMicro](https://downloads.reactivemicro.com/Apple%20II%20Items/Hardware/TransWarp_GS/2016-01-17%20-%20ReActiveMicro%20-%20Transwarp%20GS%20-%20Geoff%20Body%20-%20Schematic%20Release%201.pdf),
  [apple2.org.za mirror](https://mirrors.apple2.org.za/ftp.apple.asimov.net/documentation/hardware/accelerators/transwarp-gs---schematic.pdf)
  — authoritative for §7.1; not yet reviewed page-by-page (fetch blocked from the
  cloud sandbox); commit a copy under `docs/reference/` when available
- [TransWarp GS — ReActiveMicro wiki](https://wiki.reactivemicro.com/TransWarp_GS) —
  GAL functions, cable notes ("not straight-through"), upgrade history
- [TWGS manual (PDF)](https://downloads.reactivemicro.com/Apple%20II%20Items/Hardware/TransWarp_GS/TranswarpGS%20Manual.pdf)
- *Apple IIGS Tech Note #68 — Tips for I/O Expansion Slot Card Design*
  ([archive.org](https://ia600206.us.archive.org/16/items/IIgs_2523068_Expansion_Card_Design/IIgs_2523068_Expansion_Card_Design.pdf))
  — source for the FPI bank-latch/RDY rules in Phase 2.2
- WDC W65C816S datasheet — bus timing (tADS, tBAS, tDSR/tDHR, BE)
- *Apple IIgs Hardware Reference* — FPI/Mega II cycle behavior, speed register `$C036`
- [docs/setup-gowin-cli.md](../../../docs/setup-gowin-cli.md), [docs/conventions.md](../../../docs/conventions.md) — build & repo rules
