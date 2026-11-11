# S7 fast core — exploration log (benchmax)

Status: active, started 2026-11-11. Child of
[`gs_socket_s7_accelerator_scoping.md`](gs_socket_s7_accelerator_scoping.md)
(§2.1 clock choice, §3 performance model). Nothing here is in a board
build; everything is measured in simulation and in a core-only Fmax probe.

## 0. Goal and the two levers

Wall time on the bench = `steps × step_time + external_cycles`. The
scoping doc fixes the integration structure: the core runs in the 108 MHz
domain with a clock enable, so the step rate is **108 / N** (N = 4: 27 MHz,
the plan; N = 3: 36 MHz; N = 2: 54 MHz). Within one step the memory of
record (BSRAM, banks $00/$01) needs its own clock edge, so a step is

    address clock (regs → A_OUT → BSRAM address)  +  data clocks (BSRAM DO → mux → ALU → flags/regs)

Levers, in the order this exploration pulls them:

1. **Shorten the data cone** (timing-only rewrites, cycle-exact). Decides N:
   cone Fmax ≥ 54 MHz ⇒ two data clocks fit ⇒ N = 3; cone ≥ 108 ⇒ N = 2.
2. **Fewer steps per instruction** (cycle-count changes, architecture-exact
   only). Dead internal cycles, 16-bit fetches from the local memory, a
   data-late pipeline that frees the address clock.

The classic cards never got past lever 1 because their memory was on the
motherboard; ours is on the card, so lever 2 is open to us.

## 1. Infrastructure

| piece | path | what it does |
|---|---|---|
| fast core | `boards/a2mega/hdl/twgs/fast/` (`F65C816*`) | the variant under exploration; starts as a renamed copy of `65C816/` |
| bench | `hdl/twgs/sim/bench/run_cpu_bench.sh` | Verilator: stock `P65C816` (reference) and `F65C816` (DUT) on identical ideal memories, same clock/reset/RDY |
| programs | `hdl/twgs/sim/bench/prog/bench_all.s` (ca65) | seven workloads with checkpoint marks, each self-checking a known result |
| Fmax probe | `boards/a2mega/a2mega_cpu_probe_138b_{stock,fast}.gprj` | core + 128 KB BSRAM in the address-clock/data-clock step on the 138B; `GPRJ=… tools/build.sh a2mega all` |
| adder equivalence | `hdl/twgs/sim/bench/tb_addsub_eq.sv` | exhaustive 8-bit × modes + random 16-bit, stock vs fast `AddSubBCD` |

The bench compares on two layers. **Lockstep** (`+lockstep=1`, default):
address, RWB, VPA/VDA/VPB and write data must match every step — for
cycle-exact rungs. **Architectural** (always on): at every opcode fetch the
register file (PC, A, X, Y, SP, D, P with E, PBR, DBR) and the ordered
write stream are queued per core and compared head-to-head — for rungs that
change the step count, run with `+lockstep=0`. `+stall` adds pseudo-random
RDY stalls; `+rom=<boot1.rom>` boots the ROM 01 image (banks FE/FF, the
reset-state map in 00/01/E0/E1, $C0xx as a write-back register file) as a
lockstep stress — the ROM is never committed, point at the MiSTer
`vsim/boot1.rom`.

Probe reading: the clock Fmax reported for `clk_cpu` is the cone frequency.
The constraint (18 ns) is a target that makes the placer try, not a
pass/fail; the build "fails" timing by design until the cone is faster than
the constraint.

## 2. Baseline (stock core, 2026-11-11)

Bench (`run_cpu_bench.sh`, 4.6 M steps, 1.42 M instructions, 4.5 s wall):

| mark | workload | steps | steps / instr |
|---|---|---|---|
| 1 | sieve (BYTE, 8191 flags, 16-bit zp, (zp),Y) | 1 359 532 | 2.93 |
| 2 | fib16 (iterative 16-bit, 512 × 23) | 588 855 | 2.88 |
| 3 | strings ((zp),Y copy + compare, 256 × 64) | 542 503 | 3.65 |
| 4 | bcd (decimal counter + ADC/SBC sweep) | 507 425 | 3.13 |
| 5 | stack (recursive fib, JSR/RTS/PHA/PLX, `adc 1,s`) | 459 299 | 3.39 |
| 6 | interp (bytecode VM, `jmp (abs,x)`) | 759 201 | 3.84 |
| 7 | native (16-bit copy/sum, MVN, shift-add mul, 16-bit BCD) | 387 709 | 3.70 |

Probe, stock core, 138B (`a2mega_cpu_probe_138b_stock`, 18 ns constraint):

| | |
|---|---|
| clk_cpu Fmax | **48.09 MHz** (17 logic levels, 228 violated endpoints at 55.6) |
| worst data delay | 19.96 ns |
| critical cone | microcode BSRAM `DO` → ALU control → ALU → `PCr/CE` (via carry out), `T`, `P` |
| logic / regs / CLS | 2 028 LUT / 290 / 1 190 |

Two things the report says that the scoping doc did not know:

- The cone **starts at a BSRAM output**: the microcode ROM lives in BSRAM
  (`MC.VA_0_s7/DO[3]` etc.), so every control path pays the BSRAM
  clock-to-out before its first LUT. Pre-decoding control into wider ROM
  words costs BSRAM width (one 2048×9 block per 9 bits) and removes LUT
  levels — the 138B has 340 blocks.
- It **ends in the ALU**: `PCr/CE` is reached through the ALU carry out
  (`PCDec`), `P` through the flags, `T` through the result. The adder was a
  gate-level ripple (`bit_adder` × 16 plus four serial decimal correction
  adders), which synthesis keeps as written.

## 2b. Decision 2026-11-11: fidelity is true north

Ed's call after the baseline: **the shipping core stays cycle-faithful**.
Every instruction takes the step count a real 65816 takes, so the lockstep
bench is the gate for everything on the main line and "native speed"
remains one step per PHI2 with real-CPU timing. The reason is testing cost:
a core whose cycle counts differ from the real part needs the whole
cycle-timed software corpus (IWM 5.25-inch writes, demos, sound loops)
exercised to find the edge cases, and we do not want to pay for that.

A **ludicrous mode** — non-faithful, bubbled data-late, step diet — is
welcome as an *optional, selectable* core for users who want the bleeding
edge and for benchmarks, on two conditions: the faithful core is always
present and is the default, and ludicrous is never the only path to a
feature. It is vanity-metric work and is scheduled after the main line.

Consequences for the ladder:

- The same ~2.7 ns is needed either way. In the two-edge step, N = 3
  gives the data cone two 108 MHz clocks (≈ 17.5 ns usable) for today's
  20.2 ns. In a one-edge data-late step at 54 MHz, the cone from BSRAM DO
  to the registers has the same 17.5 ns. So **rung 2 is first on every
  path**, and it is cycle-exact by construction.
- A **cycle-exact data-late** core exists in principle: the address for a
  bus cycle must be at the BSRAM pins one step earlier than the stock core
  produces it, which means the *microcode* must also be one step ahead —
  the next microinstruction has to come out of a LUT ROM in the same step
  that fetches the opcode, not out of a BSRAM read a step later. That is a
  restructure of `P65C816.sv`/`mcode.sv`, not a microcode rewrite, and
  lockstep verifies it. It is the main line's route to N = 2 (54 MHz) and
  it is gated on rung 2a below (microcode ROM in fabric) being affordable.
- Bubbled data-late and the step diet are ludicrous-mode only.

## 2c. After the plateau: the structural options (2026-11-11)

The two-edge step needs, per step, the address cone (one clock) plus the
data cone (tC2Q + output mux + core, measured 18.7 ns after rungs 1-2d).
At 108 MHz that is N = 4 unless the data cone fits two clocks (17.5 ns
usable). All four options below keep the core cycle-exact; they differ in
what else they touch. The speedups use the scoping doc's §3 model
(Applesoft-class, 1-3 % external cycles); 2× Squeezer ≈ 11.8× stock.

| option | step rate | touches | status |
|---|---|---|---|
| A. 108 MHz, N = 4 | 27 MHz, ≈ 9-10× | nothing (the plan) | always available |
| B. core PLL at 100 MHz, N = 3 | 33 MHz, ≈ 11× | the HyperRAM-spine clock decision of 09-05 (100 is not 2:1 with clk_logic 54; broker CDC) | data fits (0.8 ns margin); **address 10.9 > 9.5 ns** — needs the ADDR_BUS IR pre-decode (≈ −2 ns) first |
| C. 108 MHz, N = 3, memory clock phase-shifted | 36 MHz, ≈ 12-13× | a second PLL output for the BSRAM clock | **dead**: address 10.9 and data 18.7 sum to 29.6 ns against a 27.8 ns step; a phase shift only moves the shortfall between the edges |
| D. cycle-exact data-late core (3x) | 54 MHz at 108 (needs the same 1.2 ns) or 50 MHz at 100 (fits), ≈ 15-16× | core control-path restructure: microcode and address one step ahead | design not started |

A fifth lever is fidelity-compatible and worth noting: a small cache in
front of the 128 KB banks (one to four blocks, placed near the core; one
mux level) with a **CE stall on a miss**. The core's step count per
instruction is unchanged — the broker already stretches steps for external
cycles — only wall time moves. Hit path saves ~2 levels and the long BSRAM
exit nets; a 5-10 % miss rate at two clocks each costs ~0.15 clocks per
step. It is the kind of thing that turns a 1.2 ns miss into a fit, but it
is two weeks of work and a second memory hierarchy; only if B/C fail.

## 2d. Proposed clocking after the verdict (ed, 2026-11-11)

**One accelerator domain, isolated at the socket.** A dedicated PLL output
(100 MHz) clocks the socket PHY's pin sequencer; the data-late core steps at
50 MHz (CE or a synchronous 50 MHz output of the same PLL); the 128 KB banks
and the ROM image live in that domain. PHY ↔ core is synchronous logic.
Everything the accelerator does with the rest of the machine — video, I/O,
slot space, the Mega II banks, the card's own peripherals — is a socket
cycle: out to the motherboard and back in through the slot, as every real
accelerator does. No fabric fast path; S8 (serving our own slot cards
inside the chip) becomes a deliberate later exception.

Consequences:

- The 108 MHz HyperRAM/DOC spine is untouched. The DOC budget on a2p25
  was a 7 % miss fixed by priority, not headroom, and "glitch-free under
  typical usage" is not a margin to spend on a 7 % clock cut.
- The only remaining clock crossing is the expansion-RAM cache miss path
  (DDR3 now, HyperRAM later), which already stalls the core: a few core
  clocks per miss in wall time, never in step count. ESP32 housekeeping is
  a slow register handshake.
- On the shared backing store the DOC cannot be stalled and the core can:
  **DOC first, CPU stalls.** (The TransWarp plan's "CPU at arbiter
  priority 0" is superseded by this for the shared-memory case.)
- The PHY sequencer at 10 ns instead of 9.3 ns resolution: sample points
  and output-enable delays are re-derived in clocks; the socket-timing
  bench (`hdl/twgs/sim/tb_gs_socket.sv`) re-validates it.
- The core clock is free to move later: if a rung finds the 1.5 ns, the
  PLL goes to 108 and nothing else is re-timed.

Open checks: a free 100 MHz PLL output on the 138B SOM (the a2mega PLL
plan lists unused CLKOUTs); the broker's miss-path CDC design.

**Memory placement on the HyperRAM board rev (ed, 2026-11-11):** instead of
putting the accelerator's expansion banks on the HyperRAM beside the DOC,
**give the DDR3 to the accelerator and move the slot side (framebuffer,
shadow, DOC, peripherals) to the HyperRAM** — i.e. make a2mega's slot side
the a2p25 architecture, which already runs the full core at 1080p×4 with
SHR and DOC audio from a 108 MHz HyperRAM (`docs/memory_bandwidth_analysis.md`
§3, `boards/a2p25/docs/MODERNIZATION_PLAN.md`). The DDR3 then has one
client: no arbiter tails (the 0.55-1.6 µs unbounded-under-FB-load tails
were the reason the TransWarp plan rejected CPU-on-DDR3), the
`ddr3_port_cdc` FIFO BSRAM (33 of 90 blocks) is freed, and the two boards
share one slot-side gateware. Supersedes the "CPU → HyperRAM" placement in
the TransWarp plan for the board rev.

**Testing the split on today's boards (ed, 2026-11-11).** The board-rev
architecture can be exercised before any board exists: an
**accelerator-only a2mega build** (socket PHY + core + local banks/ROM +
DDR3 as the single-client expansion store + ESP32 for control/telemetry
over WiFi/telnet; no framebuffer, shadow snoop, DOC, slot ROM/I/O decode or
OSD) in one slot of a IIgs, with an **a2p25 in another slot** providing
video, DOC audio, disks and peripherals from its HyperRAM. The two cards
share nothing but the motherboard bus, which is exactly the claim the
single-domain design makes, so the pair is a stronger test than the merged
board would be: any coupling the accelerator secretly needs shows up as a
failure instead of a fast path. It also makes the single-client DDR3
latency measurable on real hardware and shows the slot-side gateware
running unmodified beside an accelerator (a2p25 sees the accelerator's
forwarded bank $00/$01 writes as ordinary CPU writes, §2.4 of the scoping
doc).

Interaction points to watch: (1) **/RESET** — a2p25 holds the open-drain
slot /RESET until its mounts are done (`reset_hold`); the accelerator's S6
sequencer sees that as the socket /RES it already re-registers, so the
hold simply lengthens the reset, but the arm window is measured from the
*release*, which is now a2p25's, not the ADB micro's. This is not a
bench artefact to design around: the accelerator must coexist with any
reset-holding card (our own slot cards, other vendors' storage cards), so
the S6 sequencer's arm reference is defined as *the last release of
/RESET*, whoever held it (ed, 2026-11-11). (2) **Slot I/O
ownership** — the accelerator build must claim no $C0n0/$Cn00/$C800 space
and must never assert /INH or /DMA, or it collides with a2p25's slots.
(3) **No OSD on the accelerator** — speed/mode control goes through the
ESP32 (telnet), or later through a2p25's OSD by a side channel; nothing on
the bench needs that yet. (4) The a2mega must carry the **138B SOM** (the
cone numbers are 138B; the 60K has not been measured for this core).

## 3. The ladder

| rung | change | exactness | expected |
|---|---|---|---|
| 1 | `AddSubBCD` as one behavioral add (carry chain) + parallel decimal correction; `ALU_TAB` folded into the microcode ROM (`AF_TAB`) | cycle-exact | done: exact, no cone change |
| 2a | microcode tables `syn_romstyle = "logic"`: microinstruction register in fabric flops behind LUT ROM, removing the BSRAM tC2Q + column net (~3.2 ns) from the head of every control path | cycle-exact | cone −1.5…−3 ns; also the enabler for 3x |
| 2b | pre-decode into ROM width: one-hot `BUS_CTRL` selects + `w16` terms (done), then `LOAD_P`/`LOAD_PC` conditions per opcode, shallower LUT ROM | cycle-exact | done part: 18 → 16 levels, Fmax flat within roll noise (50-52); ~1.5 ns still to find |
| 3 | cycle-exact data-late: the address of the next step formed from next-state values (`NA_OUT`), registered inside the BSRAM at the step edge; one clock per step; `A_OUT` = its register; writes at the step edge with a one-entry read bypass | cycle-exact (lockstep, incl. stalls, interrupts, ROM 01 boot) | **done 2026-11-11**: 41.1 MHz naive, 49.5 MHz after two next-address cuts (first rolls, core + 128 KB memory) — **main line** |
| L1 | ludicrous: bubbled data-late where 3x's address path does not close | architecture-exact | optional core |
| L2 | ludicrous: step diet (RMW double write, dummy reads on indexed page crossings, 16-bit operand fetch as one local read) | architecture-exact, local memory only | 10-20 % fewer steps |

**Re-ranked after rung 1 (2026-11-11).** The data clocks of the
two-edge step have to hold tC2Q + output mux + the whole core cone
(≈ 20 ns), so N = 3 needs ≈ 2.5 ns out of a 14-level fabric cone and N = 2
is out of reach by tuning. Rung 3 changes the shape instead: with the
synchronous BSRAM, latching the address *at* the step edge and consuming
the data in the following step makes each step one clock — the memory
access overlaps the previous step's execute, the structure Arlet Ottens'
6502 uses to run one bus cycle per clock from block RAM. The same ≈ 20 ns
cone then gives N = 2 (54 MHz steps at 108) directly, and every rung-2 ns
is upside on top. The cost is a microcode re-schedule: every use of `D_IN`
(the `DB` mux, `AddrGen` operand captures, `NextIR`, `PBR/DBR/P/DR/T`
loads) moves one step later, and an address that depends on the byte just
read (operand high byte → effective address) needs either a bubble or a
short combinational path from the BSRAM output into the address mux
(tC2Q + mux + a byte concatenation, inside one 9.3 ns clock). Rung 3 is
therefore promoted to the main line; rung 2 becomes its finishing pass.

Rungs 1, 2a, 2b and 3x are verified by lockstep; L1 and L2 by the
architectural layer plus the ROM boot, and they ship only beside the
faithful core. Each rung gets one probe build and a row in §4; a rung that
does not move the cone or breaks the bench is reverted, not kept.

## 4. Log

- **2026-11-11 baseline.** Bench and probe built; stock numbers above.
- **2026-11-11 rung 1 (sim).** New `F65C816_AddSubBCD`: 17-bit behavioral
  add for the binary and decimal-subtract paths (decimal subtract's nibble
  carries are the binary carries; digits get −6 where the carry out is 0),
  generate/propagate decimal carries and parallel digits for decimal add,
  overflow on the uncorrected nibble sum with the mode's carry-in. Bit-exact
  against the stock module on 3 572 864 vectors (exhaustive 8-bit operand
  space in all 16 mode combinations, exhaustive upper byte with random lower
  bytes, 2 M random 16-bit). `ALU_TAB` folded into a second registered ROM
  column (`AF_TAB`, generated from the stock tables). Full bench, `+stall`
  and 1 M-step ROM 01 boot in lockstep: PASS. Probe build (`a2mega_cpu_probe_138b_fast`):
  **48.40 MHz**, 17 levels, 186 violated endpoints, 2 145 LUT — the cone
  did not move. The worst path is now data-BSRAM `DO[0]` → `DB` mux → ALU →
  zero-detect/result select → `P[1]` (Z), 20.16 ns:

  | segment | levels | ns | note |
  |---|---|---|---|
  | BSRAM tC2Q | — | 2.2 | data memory block, `mem_mem_6_6_s` |
  | 64:1 BSRAM output mux (`mem_DOL`) | 3 | 4.2 | 2.0 ns net out of the BSRAM column, 1.4 ns fan-out-19 net into the core |
  | `DB` operand mux | 3 | 2.9 | `BUS_CTRL` decode + select |
  | adder (`t1`, `AluR`) | 3 | 2.6 | the rewritten path; was similar before |
  | result select / zero detect (`n755`, `n752`) | 5 | 5.0 | `secOp` result mux, 16-bit zero, `ZO` |
  | `P[1]` load mux (`n1524`) | 3 | 2.2 | the `LOAD_P == 001` condition tree |
  | | 17 | 20.2 | cells 33 %, routing 56 %, tC2Q 11 % |

  Reading: the GW5A fabric costs ~1.2 ns per level here, routing included,
  so 17 levels ≈ 20 ns whatever the adder does. The memory's share
  (tC2Q + output mux + the nets to the core) is 6.4 ns on its own; the
  core's own cone from the `DB` inputs to `P` is ~13.7 ns / 14 levels. The
  stock worst path had the same length but started at the microcode
  BSRAM; the fold moved that start into fabric flops (`MC.ALU_CTRL.w16_s4/Q`
  now appears as a launch point at 20.0 ns), which is why the worst path
  changed identity without changing length. Rung 1 is kept (it is exact and
  removes a 16-stage ripple from the netlist) but it is not the lever.
- **2026-11-11 memory share isolated.** `a2mega_cpu_probe_138b_fast_1blk`
  (same core, one 2 KB block, no output mux): **55.17 MHz**, 18 levels,
  17.3 ns, 8 violated endpoints. The worst path now starts at the
  *microcode* BSRAM (`MC.ALU_CTRL.w16_s9/DO[3]`, tC2Q 2.0 + 1.2 ns net) →
  `SB`/`DB` muxes → adder → result → zero → `P`. So:

  | cone | MHz | ns | what it bounds |
  |---|---|---|---|
  | core alone (microcode BSRAM → regs) | 55.2 | 17.3 | the fabric cone; ~3.2 ns of it is the microcode ROM's tC2Q + net |
  | core + 128 KB memory (tC2Q + 64:1 mux) | 48.4 | 20.2 | the data clocks of the two-edge step as the plan has it |

  For N = 3 at 108 MHz the two data clocks give 18.5 ns minus 0.5 ns
  uncertainty and setup ≈ 17.5 ns for the 20.2 ns cone: ~2.7 ns to find.
  Candidates, by size: microcode register out of BSRAM into fabric flops or
  a pre-decoded wider ROM whose first LUT level disappears (≈ 1.5-2 ns),
  one-hot `BUS_CTRL` for the `SB`/`DB` muxes (≈ 1 ns), flag-load tree
  pre-decoded per opcode (≈ 1 ns). That is rung 2, and it decides whether
  N = 3 is real. N = 2 is rung 3 only.

- **2026-11-11 rung 2a.** `F65C816_mcode` carries `syn_romstyle = "logic"`
  at module level (on the table itself the attribute is silently ignored —
  SUG550 §5.17 allows module, reg or instance). Probe
  `a2mega_cpu_probe_138b_fast`: **52.07 MHz**, 18 levels, 18.68 ns, 74
  violated endpoints (from 48.40 / 20.16 ns / 186). MCode is now 1 253 LUT +
  56 regs and 0 BSRAM (was 23 LUT + 7 BSRAM); whole probe 3 359 LUT, 64
  BSRAM. Worst path still data-BSRAM `DO` → `DB` mux (3) → `CIIn` carry-in
  (2) → carry chain → `AluR` result select (5) → `P[1]` load (3). Gap to the
  N = 3 budget (≈ 17.5 ns): **1.2 ns**. Lockstep: PASS (attribute only).

- **2026-11-11 rung 2b.** `gen_fast_mcode.py` now generates `fast/mcode.sv`
  with a second pre-decoded column (`XT_TAB`: one-hot `SB_SEL`/`DB_SEL`,
  `W16_FORCE/M/X`); `w16` and the operand muxes are built from it. Lockstep
  full bench, `+stall`, 1 M-step ROM boot: PASS. Probe: **50.11 MHz, 16
  levels** (from 18), 19.12 ns, 94 violated endpoints, 3 593 LUT. Two fewer
  levels but 0.4 ns *slower* than 2a: the worst path carries three nets of
  0.9-1.5 ns (`DB_2`, `n848_s6`, `PCr_14_s5`) where 2a's had one. This is one
  placement roll against one; the two results are inside the roll-to-roll
  spread this project has measured elsewhere (timing_round2.md), so 2a+2b
  reads as **≈ 50-52 MHz, 18.7-19.1 ns**, about **1.5 ns short of the N = 3
  budget**. The worst path now ends at `PCr/CE`: carry out → `PCDec` →
  `LOAD_PC` decode → PC enable; path 4 is the LUT-ROM lookup itself
  (`X_1/Q` → `NextState` → `XT_TAB` → `XT`, 19.3 ns), so the microcode LUT
  ROM is at the edge of the budget too.

  Where this leaves the main line: the two-edge step is N = 4 (27 MHz, the
  plan) unless ~1.5 ns more comes out, and the same cone gives a cycle-exact
  data-late core N = 2 (54 MHz) minus the same 1.5 ns. Remaining cycle-exact
  cuts, by expected size: pre-decode the `LOAD_P == 001` flag-load condition
  and the `LOAD_PC`/`PCDec` enable per opcode (1 level each); split the
  microcode LUT ROM by opcode class so the lookup is shallower; two more
  placement rolls of the final 2b source before trusting any of these
  numbers to ±0.5 ns.

- **2026-11-11 rung 2c.** Flag-load condition tree (`LOAD_P == 001`, the
  PLB/PLD/TSX specials) pre-decoded into `XT_TAB` (now 25 bits); the
  MVN/MVP PC repeat written as `PCr + ({16{PCDec}} & 16'hFFFD)` so the ALU
  carry reaches the PC through the data path instead of a register enable
  behind the whole decode; probe constraint tightened to 17 ns (the N = 3
  budget). Bench gained PLB/PLD/TSX/PHB/PHD coverage in both modes (and a
  push-order bug in that new code was caught by the mark counts, not by
  lockstep — both cores executed it identically). Lockstep full, `+stall`,
  ROM 1 M: PASS. Probe: **52.32 MHz, 19 levels, 18.62 ns**, 124 violated
  endpoints, 3 645 LUT. The PC endpoints are now `PCr/D` (enable gone), but
  the worst path shows **six LUT levels after the adder just to form `CO`**
  (`CO_s26…s8`): the rung-1 adder recovers byte/nibble carries from the sum
  by XOR, then muxes by width and decimal mode, then by `secOp`. Path 4 is
  again the LUT-ROM lookup (`DR_0/Q` → `MI.va`, 18.5 ns). Budget gap ≈ 1.1 ns.

- **2026-11-11 rung 2d, first try — REGRESSION, reverted in part.** One-hot
  ALU control (`FST`/`SEC`/`SEL_ADD` as microcode columns, `FALUCtrl_r` 25
  bits, ALU selects as AND-OR; bit-exact on 5.12 M vectors × all 256 control
  words, `tb_alu_eq.sv`) plus a 20-bit adder with a guard bit above every
  nibble so nibble/byte carries are sum bits. Lockstep: PASS. Probe:
  **47.46 MHz, 23 levels, 20.6 ns.** The path says why: GowinSynthesis
  splits the carry chain at each guard bit into four CLS segments
  (`g_3 → g_5`, `g_8 → g_10`, `g_13 → g_15`) with 0.97-1.2 ns of routing
  between them, so the chain costs ~4 ns instead of ~1.2. Lesson: **never
  put constants inside a carry chain on GW5A; one unbroken add, side
  chains for extra carries.** Replaced by a 17-bit chain plus a parallel
  9-bit low-byte chain for `co8` (adder equivalence sweep re-run).

- **2026-11-11 rung 2d, corrected.** One 17-bit chain + parallel 9-bit
  low-byte chain for `co8`, flat decimal lookahead, one-hot ALU control.
  Lockstep full, `+stall`, ROM 1 M: PASS. Probe: **52.20 MHz, 19 levels,
  18.69 ns**, 104 violated endpoints, 3 616 LUT. Worst path: data `DO` →
  mux → `DB` (3) → adder → `Result8` → carry/zero select (6, `CO_Z`) →
  the masked PC add (16-bit chain, 1.8 ns net in front) → `PCr/D`.

  **Plateau.** Four cycle-exact rungs with sound local reasoning (2b, 2c,
  2d×2) left the cone where 2a put it: 50.1 / 52.3 / 47.5 / 52.2 MHz. The
  remaining 18.7 ns is roughly half routing on a 19-level cone that starts
  5.7 ns after the edge (tC2Q 2.2 + BSRAM exit net 2.1 + mux and entry
  nets). Rearranging LUTs is not going to find the last 1.2 ns here.
  What is left is structural, and it comes in three flavours (see §2c).

- **2026-11-11 address cone measured.** `a2mega_cpu_probe_138b_fast_addr`
  (27.8 ns base period; `set_max_delay 9.260` from core registers to the
  BSRAM `AD` pins; results in the report's Timing Exceptions section):
  **10.9 ns, 8 levels — 2.8 ns cells, 7.7 ns routing.** The path is
  `IR` → the ADDR_BUS special-case decode (`n1710`, 2 levels) → the
  address mux (`a_*`, 4 levels, 1.0-1.3 ns nets between) → a **2.3 ns net
  with fan-out 65** into the 64 BSRAM blocks' address pins. The same report
  also shows the microcode LUT-ROM lookup at 26 ns under a relaxed
  constraint (`Y` → `NewAAL` → `NextState` (fan-out 175, 3.1 ns net) →
  five ROM levels), i.e. the core-internal paths stretch to whatever they
  are given; in the real step they have the whole step, and the earlier
  probes held them to one data-clock budget — an over-constraint that
  distorted every data-cone number so far. The `_mc` variant corrects it
  (core → core as 2-cycle, data cone at 18.52 ns).

  **Implication for the two-edge step at 108 MHz:** address 10.9 > 9.26
  and data 18.7 > 17.5 — short on *both* edges, by ~2 ns each. At 100 MHz
  N = 3 the data cone fits but the address cone does not (10.9 > 9.5)
  unless the IR-dependent ADDR_BUS terms are pre-decoded (−2 levels,
  ≈ −2 ns) or the memory clock is phase-lagged ~1.5 ns, which eats the data
  margin again. The address fan-out net is inherent to a 64-block memory.
  The data-late structure (D) is the only option that does not have to
  win both edges: its step is max(address, data) = the data cone, so 2 ×
  100 MHz fits today (50 MHz steps) and 2 × 108 needs the same 1.2 ns.

- **2026-11-11 data cone, step structure modelled.**
  `a2mega_cpu_probe_138b_fast_mc` (18.52 ns base = two 108 MHz clocks,
  core → core paths 2-cycle): **50.96 MHz, 19.0-19.2 ns on the data cone**,
  65 violated endpoints, all of them `DO` → core registers (`PCr`, `P`,
  `T`, `X`, `Y`, microcode registers). Same path anatomy as before: tC2Q
  2.2 + 1.9 ns BSRAM exit net + mux + 0.8/0.9 ns nets into `DB` + ALU +
  carry/zero select + 1.6 ns net into the PC adder. Across five placements
  the data cone has read 18.6-19.2 ns; call it **19 ± 0.3 ns**. Verdict
  for the two-edge step at 108 MHz: not by LUT work. (Note: `get_regs
  {u_cpu/*}` did not reach the nested `u_cpu/MCode/*` registers — one
  microcode-lookup path still appears at one cycle; use two patterns next
  time.)

### Where this leaves the goal (2× Squeezer ≈ 11.8× stock)

| structure | step | ≈ speedup | what it needs |
|---|---|---|---|
| two-edge, 108 MHz, N = 4 | 27 MHz | 9-10× (1.6× Squeezer) | nothing; the plan |
| two-edge, 100 MHz, N = 3 | 33 MHz | 11× (1.9×) | address cone 10.9 → ≤ 9.5 ns (ADDR_BUS IR pre-decode, est. −2 ns); data 19.0 vs 19.5 budget — no margin |
| data-late (cycle-exact), 100 MHz, N = 2 | 50 MHz | 15-16× (2.6×) | the 3x restructure; data cone 19.0 fits the 19.5 budget; address cone irrelevant (< step) |
| data-late (cycle-exact), 108 MHz, N = 2 | 54 MHz | 16-17× (2.8×) | 3x plus 1.5 ns the fabric has not given up |

Only the data-late core clears 2× with margin, and it does so at the same
cone we have today. The cost is the restructure of the control path
(microcode and address one step ahead) and the 100 MHz clock decision
(the 09-05 HyperRAM-spine choice assumed 108; 100 is not 2:1 with
clk_logic 54, so the broker gets a CDC or the spine moves too).

### Review of the morning (2026-11-11, re-read of the `_mc` probe report)

Read from `impl/pnr/a2mega_cpu_probe_138b_fast_mc` (no new build):

- **Correction:** the data memory is not behind a 64:1 mux. Synthesis
  mapped the 128 KB as 64 blocks of 16K×1 (`BIT_WIDTH=1`), eight blocks per
  data bit, so the output mux is **8:1 per bit** and costs 0.74 ns of cells.
  The memory side of the cone is tC2Q 2.2 + exit net 1.9 + mux 0.7 =
  4.9 ns; the "64:1" label in the earlier anatomy tables is wrong, the
  delays there are right. Nothing is left to win from re-organising the
  memory; only the 1.9 ns exit net (placement) is soft.
- **The top 25 paths are a wall 0.34 ns thick** (19.16 … 18.82 ns) ending in
  five different places: `PCr` through `ALU CO → PCDec → PC adder` (the
  MVN/MVP repeat decision, a 4 ns tail after the ALU result), `Y`/`X`/`T`,
  `P`, and the microcode registers. Cutting any one exposes the next within
  0.1 ns, which is what rungs 2b-2d experienced. A cycle-exact cut for the
  first exists (the repeat decision is `A != 0` at the start of the step,
  a register compare or a maintained flag, not the ALU carry-out) and is
  worth doing inside the restructure, not as another rung.
- **A pure core→core path is in the wall:** `X → AddrGen add → NewAAL →
  NextState → microcode LUT ROM → MI`, 18.3 ns, of which about 8 ns is the
  ROM lookup behind `NextState`. (It showed up single-cycle because the
  multicycle exception does not match the nested `u_cpu/MCode/*`
  registers.) In the two-edge step this path has the whole step; in the
  data-late step it has **one** step, so it is a real 50 MHz constraint.
- **Honest margin at 50 MHz steps:** 20.0 − 0.5 uncertainty = 19.5 ns
  against a 19 ± 0.3 ns wall is +0.2 … +0.8 ns, and the data-late path
  that no probe has measured yet is *memory data → next memory address in
  one step*. The 2× goal needs only ≥ 33-36 MHz steps, and the accelerator
  domain's PLL is free, so the frequency is a dial: data-late at 40 MHz
  (25 ns) clears 2× with ~5 ns in hand; 50 is the stretch, not the
  requirement.

### Rung 3: the data-late step (2026-11-11)

**What changed.** The microcode was already one step ahead (the
microinstruction register is loaded from `NextIR`/`NextState`), so the whole
restructure reduces to *the address one step ahead*: `F65C816` now outputs
`NA_OUT`, the stock address function (`AddrBus.sv`, the `ADDR_BUS` block
moved verbatim) evaluated on the values the registers will hold after the
current step. `A_OUT` is the register of `NA_OUT`. The memory registers
`NA_OUT` at the step edge (BSRAM read port, clock-enabled by `RDY_OUT`),
its data is the core's `D_IN` for the following step, and writes go to
`A_OUT`/`D_OUT` at the step edge on the second port with a one-entry bypass
for a read of the byte just written. One clock per step.

**Exactness.** Lockstep against the stock core, every step, in all modes:

| bench | result |
|---|---|
| program, ideal memory | PASS, 4,610,768 steps |
| program, `+datalate +stall` | PASS; 4,096 same-edge write/read collisions, all served by the bypass (the bench memory returns garbage there) |
| ROM 01, `+datalate +stall`, 6 M clocks | PASS |
| ROM 01, `+datalate +irq +stall` (2,924 IRQ assertions, 373 NMI pulses) | PASS |

`+irq` (pseudo-random IRQ/NMI on both cores) is new; no earlier rung had
interrupt coverage. Where the next-address function reads a register
instead of its next value (`IR`, `D`, `DBR`, `EF`, the interrupt-class
flags), `gen_fast_mcode.py` asserts the table property that makes it exact
(every opcode's step 1 addresses `PBR:PC` with no write; those registers
load only where no successor step reads them). In simulation the registered
`A_OUT` is also compared with the stock function of the registers on every
clock.

**Timing (probe `a2mega_cpu_probe_138b_fast_dl`, 138B, 0.5 ns uncertainty,
one clock = one step, so Fmax is the step rate):**

| build | change | step rate | worst slack @ target | failing paths |
|---|---|---|---|---|
| 1 | naive next-address logic, 20 ns target | 41.05 MHz | −4.36 ns | 932 |
| 2 | + repeat decision from `A`, + registered successor address fields, 20 ns target | **49.52 MHz** | −0.19 ns | 38 |
| 3 | same source, 21 ns target (clean-close check) | **47.62 MHz, clean** | +0.002 ns | 0 |

Build 3 is the gate result: the data-late core plus its 128 KB memory closes
at **47.6 MHz steps with the 0.5 ns policy uncertainty and zero violations**
(one roll; the three-roll durability rule applies to the integrated build,
not to this probe). The placer stops at the constraint, so +0.002 ns says
"met", not "no more to find"; build 2 shows about 0.8 ns more is there.

- Build 1's worst paths were all in the new logic: `mem → ALU → carry-out →
  PC adder → next address` (the MVN/MVP repeat decision), and `mem → NextIR
  → next-address fields`. The second was a synthesis artefact: the direct
  lookup `NA_TAB[{IR, NextState}]` shared decode terms with the main ROM
  (`M_TAB[{NextIR, NextState}]`; sound, because `NextIR` is `IR` whenever
  `STATE != 0`, but it puts the memory data in front of the lookup).
- Cuts (both cycle-exact, both asserted): the repeat decision is `A != 0`
  from the register (in that step the ALU computes `A − 1` in 16 bits, so
  its carry-out is exactly that); the address fields of **both** possible
  successor rows are registered with the microinstruction (`SUCC_TAB`) and
  the `NextState` decision picks one, so no ROM sits behind `NextState`.
- After build 2 the wall is the old data wall again (`mem → ALU → P / T`,
  `mem → microcode registers`) with the next-address paths level with it
  (`Y → AAL carry → NextState → RTL offset → address`, `mem → D + operand →
  DX + inc → address`). 3,870 LUT / 416 FF / 64 BSRAM.

**What the probe does not contain** (integration costs to expect):
the ROM image and cache as further `D_IN` sources (one more mux level on
the data cone); the broker's local-or-socket decision on the registered
`A_OUT` feeding the core's clock enable; whatever the full build's placement
costs versus an empty device. The PLL is a dial (§2d): plan the first
integrated build at 40 MHz steps and move up from there.

**Clock choice this opens (not yet decided).** Under the two-edge step,
three clocks of the existing 108 MHz (27.8 ns) did not fit (address 10.9 +
data 19 ns). Under the data-late step the same 27.8 ns only has to cover
the ~21 ns single-clock cone, with about 6 ns in hand. So **108 MHz / 3 =
36 MHz steps needs no new PLL frequency and no re-derived socket timing**
(the PHY keeps its validated 9.3 ns sequencer), and 36 MHz is at the 2×
AppleSqueezer line. It is the natural first integrated build; 100 / 2 or a
faster PHY clock / 3 is the later step toward the 47.6 MHz the probe closes.

**System track, same day** (branch `s7-system-replica-broker`, scoping doc
§9 there): memory-map replica, broker and a last-release-of-/RESET arm
sequencer, all in simulation, all benches re-run here and passing. The
broker is written for the two-edge step (memory strobed after the step
edge from `A_OUT`); moving it to the data-late contract (read address =
`NA_OUT`, local-or-socket decision from the registered `A_OUT` feeding the
clock enable) is the integration step. Its ROM 01 run is a reminder of the
socket bound: 3 M steps took 487 ms fast against 1549 ms native, 5.3 steps
per PHI2 cycle, because that stretch is 81,872 shadowed text-page writes.

**Consequence for the two-edge probes:** they still build, but their
address clock now starts at the `A_OUT` register, so they no longer measure
the address cone.

## 5. Questions for ed

1. Rung 3 changes the step count per instruction. The scoping doc's
   slowdown rules (native pacing in disk windows, CYAREG) assume the core
   can be made cycle-faithful on demand; a data-late core needs a
   "faithful" mode or the pacing comes from the broker. Which?
2. Memory-side rungs (mux select pre-decode, 16-bit reads) belong to the
   S7 broker, not the core. Fine to prototype them inside the probe?
