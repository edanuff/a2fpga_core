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

1. **Phase 0 — Groundwork:** resolve the confirmed /VP drive gap (§7.1; bank
   voltages are verified safe — §7.2), core provenance, project hygiene, simulation
   strategy.
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

Data bus and control (banks are the *true* GW5AT-60 banks from the Tang Mega 60K
SOM schematic — the a2-mega schematic's `BANK3/4/5_*` net-label prefixes are stale
and should be ignored):

| Signal | Ball | Bank | Dir (FPGA) | Signal | Ball | Bank | Dir (FPGA) |
|---|---|---|---|---|---|---|---|
| FPGA_GS_D0 | N15 | 6 | bidir | FPGA_GS_NMI | N13 | 6 | in |
| FPGA_GS_D1 | M15 | 5 | bidir | FPGA_GS_BE | N14 | 6 | in |
| FPGA_GS_D2 | M16 | 5 | bidir | FPGA_GS_IRQ | Y17 | 7 | in |
| FPGA_GS_D3 | N17 | 6 | bidir | FPGA_GS_PH2 | AB17 | 7 | in |
| FPGA_GS_D4 | V20 | 8 | bidir | FPGA_GS_ABORT | AA16 | 7 | in |
| FPGA_GS_D5 | U20 | 8 | bidir | FPGA_GS_RDY | AB16 | 7 | in |
| FPGA_GS_D6 | M17 | 5 | bidir | FPGA_GS_VP | AA15 | 7 | in (⚠ §7.1) |
| FPGA_GS_D7 | P17 | 6 | bidir | FPGA_GS_DATA_OE | AB15 | 8 | out |
| FPGA_GS_RW | U18 | 8 | out | FPGA_GS_ADDR_OE | Y16 | 7 | out |
| FPGA_GS_RESET | U17 | 8 | in | FPGA_GS_OE | W15 | 7 | out |
| FPGA_GS_D_DIR | W21 | 7 | out | | | | |

The address pins (table above) sit on banks 7/8 (AA21/AA20 = bank 7, the rest
bank 8). **All GS pins are on banks 5/6/7/8, whose VCCIO rails (`VCCIO_45`,
`VCCIO_678`) are fixed at 3.3 V on the SOM** — constrain everything
`IO_TYPE=LVCMOS33`. See §7.2 (resolved).

> ⚠ Verify ball numbers against the KiCad netlist before committing the `.cst`; the
> table above was transcribed from the schematic PDF.

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
| 0.3 | Resolve remaining hardware items: **/VP drive path** (confirmed gap, §7.1 — fix in the next board rev; **not a development blocker**, the Phase 2.8a vector snapshot covers current boards) and ribbon/DIP-40 pin mapping. (Bank VCCIO: resolved, no conflict — §7.2) | Updated twgs docs; go/no-go for Phase 1 power-on. All gateware phases proceed on current-rev boards in parallel with the respin |
| 0.4 | Choose simulation approach for mixed VHDL/SV (the core is VHDL). Options: nvc/GHDL for core-only opcode tests + Verilator for the SV bus engine with the core swapped for a behavioral stub; or a commercial mixed-language sim if available | `sim/` scaffold + CI-runnable smoke test |
| 0.5 | Record architecture decisions as ADRs in `docs/adr/` (see §6) | ADR-1..4 |
| 0.6 | Baseline resource/timing snapshot of current a2mega build (LUT/BSRAM/Fmax headroom for the core + engine) | Numbers in this doc |

### Phase 1 — Passive bring-up (board connected, FPGA never drives the socket)

| # | Task | Notes |
|---|---|---|
| 1.1 | Add all `FPGA_GS_*` pins to `a2mega.cst` (§2.2 table), `IO_TYPE=LVCMOS33` (all GS pins are on banks 5/6/7/8; `VCCIO_45`/`VCCIO_678` are fixed 3.3 V on the SOM — §7.2) | Outputs default tri-state |
| 1.2 | Add GS ports to `top.sv`; hard-assert all three OE lines inactive at reset and until a "TWGS enable" config bit is set | Safety invariant: a bitstream with TWGS disabled must be electrically identical to today's board |
| 1.3 | GS signal monitor module: count PH2 edges, measure fast/slow frequency (2.864 / 1.023 MHz), detect stretched cycles, observe RESET/IRQ/NMI/BE activity | Surface via DebugOverlay + UART |
| 1.4 | Bench validation with the ribbon attached to a socketed IIgs **with the real 65C816 still installed on the motherboard side of the interposer** if the cable permits, else scope-only | Acceptance: measured PH2 matches expectations incl. cycle stretching |

### Phase 2 — Transparent CPU replacement at native speed  ← **critical milestone**

The FPGA is the CPU; the machine must boot and behave exactly like stock. No
acceleration, no cache. Everything in later phases degrades to this mode.

| # | Task | Notes |
|---|---|---|
| 2.1 | Fix wrapper: correct `P65C816` port map (`VPB`, `RDY_OUT`), single coherent RDY design, BE-driven tri-state, level-shifter OE/DIR ports, reset semantics; **connect and use the core's VPA/VDA** for cycle classification (the TWGS GAL3 equivalent — internal-operation cycles repeat the last address, vector-pull qualification for /VP when a drive path exists) | Rewrite `iigs_65816_wrapper.sv`; keep the PHI1 bank-address multiplex (it is required — FPI latches the bank from D0–D7 at PH2 rise) |
| 2.2 | **Timing budget analysis** (design doc): W65C816S provides tADS/tBAS ≤ 40 ns after PH2 fall; our chain is PH2 sync (2–3 × 18.5 ns) + output reg + IOB + 164245 tpd ≈ 60–90 ns. **Confirmed** (Apple IIGS Tech Note #68, *Tips for I/O Expansion Slot Card Design*): the FPI captures the bank address in a latch that is *transparent while PH2 is low and closes at PH2 rise*, and holds the latch closed while RDY is low because the CPU emits an invalid bank address then; RDY must change only while PH2 is high. So the hard deadline for address + bank is PH2 rise (~175 ns after fall in fast mode), not the 40 ns a real chip provides — our 60–90 ns chain fits, but verify slow/stretched-cycle cases too. If margin is thin: sample PH2 on a faster clock (existing 135 MHz `clk_pixel_x5` or a dedicated PLL tap), use IOB registers, or clock the GS engine from PH2 directly via a BUFG | Doc + chosen mechanism; add `set_input_delay`/`set_output_delay` SDC constraints for the GS pins; bus engine must never change its RDY-dependent behavior mid-PHI1 |
| 2.3 | Bus-cycle engine v1: CE-gate the core 1:1 with PH2 (one core step per bus cycle), PHI1 bank mux, write-data drive during PH2 high, read sample at PH2 fall, RDY stretch, `GS_BE` release | Evolution of current FSM |
| 2.4 | Data-bus turnaround: dead-time states around `FPGA_GS_D_DIR` flips so the 164245 never fights the motherboard | Bus-contention checklist |
| 2.5 | Simulation: bus-functional model of the FPI (fast/slow/stretched cycles, RDY, DMA/BE) + opcode-level tests of the core (65C816-aware test suites) + full-cycle traces compared against a known-good emulator (e.g., recorded boot traces) | Regression suite in `sim/` |
| 2.6 | Integrate into `top.sv` + `.gprj` (relative paths), build, **check timing per repo rule #2** | Bitstream |
| 2.7 | Hardware bring-up ladder: (a) free-run with socket disconnected, verify vectors on debug overlay; (b) connected, watch reset vector fetch `$00FFFC/D` and first ROM fetches; (c) self-test (`Cmd-Opt-Reset`); (d) boot GS/OS; (e) run IIgs diagnostics | Debug: last-N address/opcode trace buffer readable over UART/ESP32 |
| 2.8a | **Vector snapshot (/VP workaround, §7.1):** until a /VP drive path exists, capture bank-0 `$FFE0–$FFFF` from ROM at reset (ROM is banked in then) and serve all vector-pull reads (core VPB asserted) from the snapshot instead of the bus — replicating the FPI's "vectors always from ROM" redirect internally. Superseded by the full ROM shadow in Phase 4 | Interrupts survive LC-RAM-banked configurations without the board fix |

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
| 4.5 | Bus parking policy while executing internally: **re-present the last bus address with R/W=read**, imitating a real 65816 internal-operation cycle (verified safe — the TWGS cable carries no VDA/VPA, so the FPI must tolerate exactly these cycles; see §7.1). Guard: never park on a read-side-effect address (last access to `$C0xx` parks on the previous non-I/O address instead) |
| 4.6 | Benchmarks: instruction-mix micro-benchmarks + period speed tests; target ≥ 2.5× stock at first release |

### Phase 5 — Coherency & safety

| # | Task |
|---|---|
| 5.1 | Auto-slowdown triggers per TWGS §10: any `$C0xx` access, slot device windows (`$C090–$C0FF`, slots 4–7 configurable), during and shortly after disk I/O; implement as a throttle timer dropping to Fast/Normal |
| 5.2 | DMA (TWGS-verified model, §7.1): honor **both** halt paths. (a) `GS_BE` — TWGS receives BE from the motherboard and gates its R/W and address drivers with it, so implement BE-deassert ⇒ release drivers combinationally, exactly as TWGS's 74F125/573-OC arrangement does. (b) RDY — honor per the FPI rule (RDY changes only while PH2 high; bank invalid while RDY low). Detect and classify DMA from the **slot side** — `a2_dma_n`/`a2_rdy_n` into `apple_bus` — which is precisely how TWGS did it (`SLOT_DMA`/`SLOT_RDY` into GAL8; the CPU cable carries no RDY at all). For shadow coherency, snoop DMA write addresses via the slot-bus interface (it samples the bus independently of the CPU) and invalidate touched pages; conservative fallback: invalidate all shadowed RAM on any DMA burst |
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

### 7.1 Findings from the original TWGS schematic — checklist RESOLVED

Verified against Geoff Body's reverse-engineered TWGS schematic, Release 1, 2016
(Eagle export; committed at
[`reference/twgs_schematic_geoff_body_release1_2016.pdf`](reference/twgs_schematic_geoff_body_release1_2016.pdf),
original from [ReActiveMicro](https://downloads.reactivemicro.com/Apple%20II%20Items/Hardware/TransWarp_GS/2016-01-17%20-%20ReActiveMicro%20-%20Transwarp%20GS%20-%20Geoff%20Body%20-%20Schematic%20Release%201.pdf)).
TWGS shipped and was compatible, so what it drives or ignores at the CPU socket is
the proven-sufficient signal set. The complete J5 "To CPU cable" pin set is:
`GS_VP, GS_RESET, GS_ABORT, GS_IRQ, GS_PH2, GS_BE, GS_NMI, GS_RW, GS_D0–7,
GS_A0–15, +5V, GND` — nothing else.

| Signal | TWGS (verified) | a2mega consequence |
|---|---|---|
| **/VP** | **In the cable, DRIVEN toward the motherboard** (`GS_VP`, J5 pin 1) by a 74F32 OR gate (U5C) — VPB from the on-card CPU, qualified by card logic so vector-pull only asserts on real GS bus cycles | ⚠ **Confirmed a2mega hardware gap.** J8's `GS_VP` enters the FPGA through U8's input-only group; nothing can drive /VP to the motherboard. **What /VP does on the IIgs:** the FPI watches it to force interrupt-vector fetches from ROM — *"interrupt vectors are always pulled from ROM regardless of whether banks $00/$01 are enabled"* (IIgs Hardware Reference; the $C071–$C07F interrupt ROM is part of this dispatch path). **Without it:** boot/reset works (ROM is banked in), but any IRQ/NMI/BRK taken while LC RAM occupies bank-0 $E000–$FFFF — the normal state under GS/OS and ProDOS 8 — fetches vector bytes from RAM instead of ROM and jumps to garbage; expect a crash on the first VBL/ADB/heartbeat interrupt, possibly intermittent if the OS happens to keep valid vectors in RAM. **Mitigations:** (a) board rev / bodge driving /VP from the FPGA (TWGS-style qualified drive); (b) gateware: serve vector-pull reads from an internal ROM copy, replicating the FPI redirect — full shadow does this in Phase 4, and Phase 2 can use a 32-byte **vector snapshot** captured from ROM at reset (see Phase 2.8a); (c) controlled experiment to confirm the failure signature. |
| **VDA / VPA** | **Not in the cable.** Consumed on-card only: GAL3 takes VPB + VPA + VDA + D0–7 (bank byte) for cycle qualification; they also appear on the unused expansion connector J1 | ✅ J8 leaving VDA/VPA n/c is correct — the motherboard never sees them. Our bus engine must do what GAL3 did: use the soft core's VPA/VDA outputs (P65C816 provides both; the current wrapper leaves them unconnected) to classify cycles. Also implies the FPI treats *every* socket cycle as valid — see the bus-parking note below. |
| **E, M/X, MLB** | **Not in the cable** (routed only to expansion connector J1) | ✅ J8 n/c is correct; no path needed. |
| **BE** | **In the cable, received from the motherboard**; OR-ed with conditioned GS_PH2 to gate the card's bus drivers — R/W is driven through a 74F125 *tri-state* buffer, addresses through 74HCT573 latches with OC control | ✅ a2mega matches: `GS_BE` is an FPGA input, and our OE topology (164245 enables + 74LV1T125 on R/W gated by `FPGA_GS_ADDR_OE`) mirrors TWGS's 573-OC/74F125 arrangement. Bus engine must release drivers on BE deassert. |
| **RDY** | **Not in the cable.** TWGS taps `SLOT_RDY` and `SLOT_DMA` from its slot fingers (GAL8 domain) — DMA/RDY handling is done entirely from the slot side | ✅ Validates Phase 5.2's slot-side DMA/RDY detection via `apple_bus`. a2mega additionally has `GS_RDY` at the socket (same electrical net) — a superset; use whichever is cleaner. |
| **PH2** | Received through 27 Ω series + RC (100 Ω/100 pF) + 74AC04, then synchronized into the card's 4× oscillator domain with 74AC74 double-flops (`GS_PH2_EARLY`/`X4_EARLY`) | ✅ Same CDC strategy as the wrapper (a2mega even has the matching 27 Ω series R52); conditioning moves into the FPGA. |
| **IRQ / NMI / ABORT / RESET** | Received; GS_IRQ additionally feeds the GALs *and* runs directly to the on-card CPU's IRQB (IRQ-aware throttling taps) | ✅ Matches J8; soft core + speed logic both see IRQ, as planned in Phase 5.3. |
| **Cable pin order** | J5 is emphatically **not** DIP-40 order (VP, RESET, ABORT, IRQ, PH2, BE, NMI, RW, then D/A interleaved) | Verify the a2mega ribbon/plug adapter maps J8's own ordering to the DIP-40 socket pin-for-pin before first connection. |

**Bus-parking insight (feeds Phase 4.5):** since the cable carries no VDA/VPA, the
FPI necessarily tolerates the 65816's internal-operation cycles — which repeat the
*last used address* with R/W high. So the provably-safe parking policy while
executing from shadow is to re-present the last bus address, exactly imitating a
real 65816 internal cycle — no "benign ROM address" heuristic needed.

Also corroborated: Apple IIGS Tech Note #68 (expansion-card design): the FPI
bank-address latch is transparent during PH2-low and closes at PH2 rise; RDY low ⇒
CPU bank address invalid, so the FPI freezes the latch; RDY may only change while
PH2 is high. Baked into Phase 2.2/5.2. TWGS's J2 debug header (TAG_MATCH, RAM-WE,
ROM-IN, TWGS_SPEED, GOSLOW) is a good template for our DebugOverlay signal list.

### 7.2a Tang Mega 138K module cross-check (resolved)

The a2-mega schematic's BTB net labels turn out to be copied verbatim from the
**Tang Mega 138K** SOM documentation (`BANK4_Y22_IOB131B`, `BANK5_Y17`, …, match
the 138K schematic exactly), which explains the stale-label confusion in §7.2:
same balls, different chip, different banks. Verified against the 138K SOM
schematic (committed at
[`reference/tang_mega_138k_som_schematic.pdf`](reference/tang_mega_138k_som_schematic.pdf)):

**138K rails:** `VCCIO2/3/4/5/10` = 3.3 V (FB15/FB16/FB14/FB13/FB12);
`VCCIO67` (banks 6/7 = the 138K's DDR3 banks) = 1.5 V (FB11); `VCCX` = 1.8 V
(FB23 from `1V8_VCCX`; the 3V3 option bead FB24 is DNP).

**Result: every BTB pin the a2-mega uses is on a 3.3 V bank on *both* modules.**
GS pins land on 138K banks 3/4/5, the slot bus on banks 2/3/4, HDMI/ESP32/dips/
LEDs/uart on banks 2/3/5 — all 3.3 V. `rst` (F4) is on a 1.5 V rail on both
modules (60K bank 11 / 138K bank 7) and is already constrained LVCMOS15. The
1.5 V danger zones do not overlap any used signal:

| Module | 1.5 V banks | BTB-visible 1.5 V pins | a2-mega usage |
|---|---|---|---|
| Mega 60K (GW5AT-60) | 9, 10, 11 (`VCCIO_DDR`) | BTB2 pins 52–86 (balls Y13…W12, incl. AB13) + F4 | all n/c except `button` (AB13, internal; LVCMOS15) |
| Mega 138K (GW5AST-138) | 6, 7 (`VCCIO67`) | F4 (`CPU_RESET`) only | `rst` (LVCMOS15 ✓) |

**Board keep-out rule for future revs:** never assign 3.3 V signals to BTB2
pins 52–86 — they are 1.5 V on the 60K (fine on the 138K).

**Porting a build to the 138K is gateware-only:** change the `.gprj` device to
GW5AST-138 (PG484A), regenerate the DDR3 IP and rewrite the `ddr_*` constraints
for the 138K's DDR wiring (banks 6/7, different balls — do not flash a 60K
bitstream on a 138K), and flip `button` (AB13) to LVCMOS33 (bank 5 there).
All GS/slot/HDMI/ESP32 pin constraints carry over unchanged.

### 7.2b Consolidated next-board-rev change list

Everything hardware-related found across the TWGS/SOM/USB reviews, in one place:

1. **/VP drive path** (§7.1): /VP is point-to-point to the FPI (nothing else
   drives it), so no transceiver is needed — but do not wire it raw to a 3.3 V
   FPGA pin: (a) FPI V_IH is unspecified (TWGS's 74F32 TTL-level drive suggests
   ~3.4 V highs suffice, but that is inference, not spec); (b) the line must
   default *deasserted* whenever the FPGA is unconfigured or reflashing;
   (c) a motherboard 5 V pullup, if present, would exceed FPGA pin tolerance.
   **Verified on the IIgs schematic (Ed): the 65816 /VP pin runs point-to-point
   to the FPI/CYA input with no pullup and no other junctions.** Consequences:
   (i) on current boards the FPI input *floats* when the interposer is
   installed — nondeterministic redirect behavior, so **bench bodge for
   Phase 1/2: fit a 1–4.7 K pullup to +5 V on /VP at J8 pin 1 / the cable** to
   pin it deasserted (the vector snapshot then handles redirects alone);
   (ii) no 5 V source on the net, so FPGA pin tolerance is not a concern.
   Recommended rev fix: **74LVC1G07 open-drain buffer + ~1 K pullup to 5 V**
   (assert = pull low; deassert = true 5 V high via the pullup — which also
   supplies the pullup the motherboard lacks, keeping the line defined with the
   FPGA unconfigured; ~50 ns release edge). Alternative: TWGS-style 5 V-powered
   74AHCT1G32 with its input pulled to the deasserted state. Gateware asserts
   only on genuine vector-pull bus cycles. Vector snapshot covers current
   boards meanwhile.
2. **22 Ω series resistors on the USB 2.0 lines into the FPGA** (J1 D+/D− →
   `USB_PHY_P/N`): the schematic today has only the ESD array (D2–D4) and 15 K
   host pulldowns (R7/R9) — no series termination. USB FS requires a driver
   output impedance of ~28–44 Ω; FPGA LVCMOS33 drivers are well below that, so
   add ~22–27 Ω in series, placed close to the FPGA/SOM end of the trace.
   (The USB-C port needs nothing — it goes to the ESP32-S3's integrated PHY.)
3. **Verify USB_PHY net polarity naming:** the schematic wires D+ (USB-A pin 3)
   to the net named `USB_PHY_N` and D− (pin 2) to `USB_PHY_P`. If intentional,
   document it; otherwise rename (harmless electrically — GPIO bit-banged USB —
   but a gateware polarity trap: J/K states and SE0 detection invert).
4. **BTB2 pins 52–86 keep-out for 3.3 V signals** (§7.2a): 1.5 V on the Mega
   60K (bank 9), 3.3 V on the 138K — leave n/c to stay dual-module compatible.

### 7.2 Other open items

1. **Bank VCCIO conflict — RESOLVED, no conflict.** Verified against the Tang Mega
   60K SOM schematic (committed at
   [`reference/tang_mega_60k_som_schematic.pdf`](reference/tang_mega_60k_som_schematic.pdf),
   sheets 1, 3, 6). The a2-mega schematic's `BANK3/4/5` net-name prefixes are stale;
   true banks: GS pins sit on banks 5/6/7/8. The SOM hard-wires `VCCIO_45` (banks
   4/5, ferrite FB16) and `VCCIO_678` (banks 6/7/8, FB14) to **3.3 V**, and
   `VCCIO_DDR` (banks **9/10/11** — sheet 3: "BANK 9,10,11 IO Voltage is 1.5V !!!")
   to 1.5 V. The only 1.5 V balls on BTB2 are the bank-9 group (pins 52–86: Y13,
   AA13, AB13, Y12, AB12, V10, W11, Y11, W10, AB11, AA11, AB10, AA10, AA9, W12),
   which a2-mega leaves unconnected except `button` (AB13, already correctly
   LVCMOS15); `rst` (F4) is bank 11, likewise correctly 1.5 V in the `.cst`.
   **All `FPGA_GS_*` pins are 3.3 V-safe against the 164245 A-side; constrain
   LVCMOS33.**
2. **Bus parking while accelerated.** Unverified assumption that the FPI tolerates
   repeated benign read cycles (it should — that's what a spinning CPU looks like;
   see also the §7.1 parking insight), and that Mega II refresh is CPU-independent.
   Validate early in Phase 4.
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
- **TWGS schematic** (Geoff Body, Release 1, 2016) — committed at
  [`reference/twgs_schematic_geoff_body_release1_2016.pdf`](reference/twgs_schematic_geoff_body_release1_2016.pdf)
  (original: [ReActiveMicro](https://downloads.reactivemicro.com/Apple%20II%20Items/Hardware/TransWarp_GS/2016-01-17%20-%20ReActiveMicro%20-%20Transwarp%20GS%20-%20Geoff%20Body%20-%20Schematic%20Release%201.pdf),
  [apple2.org.za mirror](https://mirrors.apple2.org.za/ftp.apple.asimov.net/documentation/hardware/accelerators/transwarp-gs---schematic.pdf)).
  Reviewed; findings in §7.1. Key landmarks on the sheet: J5 = "To CPU cable"
  (GS_* nets), U15 = on-card W65C816S (PLCC-44), U5C 74F32 = /VP driver,
  U32A 74F125 = R/W tri-state, GAL3 = VPB/VPA/VDA + bank-byte cycle decode,
  J1 = unused expansion connector (raw CPU pins incl. VDA/VPA/E/MLB/M-X),
  J2 = debug header
- **Tang Mega 60K SOM schematic** (Sipeed, rev 1.0) — committed at
  [`reference/tang_mega_60k_som_schematic.pdf`](reference/tang_mega_60k_som_schematic.pdf).
  Sheet 1 = BTB connectors with true bank/ball names, sheet 3 = DDR3 + the
  "BANK 9,10,11 IO Voltage is 1.5V" note, sheet 6 = FPGA power (VCCIO rail feeds)
- [TransWarp GS — ReActiveMicro wiki](https://wiki.reactivemicro.com/TransWarp_GS) —
  GAL functions, cable notes ("not straight-through"), upgrade history
- [TWGS manual (PDF)](https://downloads.reactivemicro.com/Apple%20II%20Items/Hardware/TransWarp_GS/TranswarpGS%20Manual.pdf)
- *Apple IIGS Tech Note #68 — Tips for I/O Expansion Slot Card Design*
  ([archive.org](https://ia600206.us.archive.org/16/items/IIgs_2523068_Expansion_Card_Design/IIgs_2523068_Expansion_Card_Design.pdf))
  — source for the FPI bank-latch/RDY rules in Phase 2.2
- WDC W65C816S datasheet — bus timing (tADS, tBAS, tDSR/tDHR, BE)
- *Apple IIgs Hardware Reference* — FPI/Mega II cycle behavior, speed register `$C036`
- [docs/setup-gowin-cli.md](../../../docs/setup-gowin-cli.md), [docs/conventions.md](../../../docs/conventions.md) — build & repo rules
