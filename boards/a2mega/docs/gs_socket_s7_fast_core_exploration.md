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

## 3. The ladder

| rung | change | exactness | expected |
|---|---|---|---|
| 1 | `AddSubBCD` as one behavioral add (carry chain) + parallel decimal correction; `ALU_TAB` folded into the microcode ROM (`AF_TAB`) | cycle-exact | done: exact, no cone change |
| 2a | microcode tables `syn_romstyle = "logic"`: microinstruction register in fabric flops behind LUT ROM, removing the BSRAM tC2Q + column net (~3.2 ns) from the head of every control path | cycle-exact | cone −1.5…−3 ns; also the enabler for 3x |
| 2b | pre-decode into ROM width: one-hot `BUS_CTRL` selects + `w16` terms (done), then `LOAD_P`/`LOAD_PC` conditions per opcode, shallower LUT ROM | cycle-exact | done part: 18 → 16 levels, Fmax flat within roll noise (50-52); ~1.5 ns still to find |
| 3x | cycle-exact data-late: microcode and address one step ahead (LUT ROM looked up from the opcode byte in the fetch step), BSRAM addressed at the step edge, data consumed next step | cycle-exact (lockstep) | N = 2, 54 MHz steps — **main line** |
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

## 5. Questions for ed

1. Rung 3 changes the step count per instruction. The scoping doc's
   slowdown rules (native pacing in disk windows, CYAREG) assume the core
   can be made cycle-faithful on demand; a data-late core needs a
   "faithful" mode or the pacing comes from the broker. Which?
2. Memory-side rungs (mux select pre-decode, 16-bit reads) belong to the
   S7 broker, not the core. Fine to prototype them inside the probe?
