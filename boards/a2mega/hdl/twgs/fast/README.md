# F65C816 — fast variant of the 65C816 core (S7 exploration)

This directory is the **working copy** of `../65C816/` for the S7 accelerator
fast-core exploration ([`boards/a2mega/docs/gs_socket_s7_fast_core_exploration.md`](../../../docs/gs_socket_s7_fast_core_exploration.md)).
Every module carries an `F65C816_` prefix (package `F65C816_pkg`) so the
stock core and this one can be instantiated side by side — the bench in
`../sim/bench/` runs them in lockstep, the stock core being the reference.

Provenance and license are those of `../65C816/README.md` (srg320's
SNES_MiSTer P65C816, GPL-3.0, SystemVerilog conversion and IIgs changes by
Alan Steremberg). Nothing here is in a board build; the stock core stays
the one `a2mega_138B.gprj` uses until a rung is promoted.

Differences from the stock core, by rung (the log in the exploration doc
is authoritative):

- **Rung 1** — `AddSubBCD.sv`: one behavioral 17-bit add plus parallel
  decimal correction instead of `BCDAdder`/`adder4`/`bit_adder` (those
  files are not part of this variant). Bit-exact (`../sim/bench/tb_addsub_eq.sv`).
  `mcode.sv`: `AF_TAB`, the `ALU_TAB` lookup folded into the microcode ROM
  at generation time, registered alongside `MI`.
- **Rung 2a** — `mcode.sv` carries `syn_romstyle = "logic"` at module level:
  the microinstruction register is fabric flops behind LUT ROM, not BSRAM.
- **Rung 2b** — `gen_fast_mcode.py` now generates `mcode.sv` from the stock
  table with the pre-decoded columns `AF_TAB` (ALU control) and `XT_TAB`
  (one-hot `SB_SEL`/`DB_SEL`, `W16_FORCE/M/X`); `F65C816_pkg.sv` extends
  `FMCode_r` with those fields; `F65C816.sv` builds `w16` and the `SB`/`DB`
  operand muxes from them. The struct types are prefixed `F` (`FMCode_r`,
  `FALUCtrl_r`, `FMicroInst_r`) because both cores import their package at
  file scope and share a compilation unit in the bench.

`mcode.sv` here is **generated** — edit `gen_fast_mcode.py` (or the stock
table) and rerun it; do not hand-edit the tables.
- **Rung 2c** — `XT_TAB` gains the `LOAD_P == 001` condition tree
  (`PF_PLB/PLD/TSX/FORCE/M8/X8/M16/X16`); `AddrGen.sv` writes the MVN/MVP
  PC repeat as `PCr + ({16{PCDec}} & 16'hFFFD)` so the ALU carry reaches
  the PC through the data path, not a register enable.
- **Rung 2d** — `AddSubBCD.sv`: one unbroken 17-bit carry chain plus a
  parallel 9-bit low-byte chain for the byte carry, and a flat decimal-carry
  lookahead (a guard-bit-per-nibble chain was tried and regressed: the
  synthesizer splits the chain at each guard; see the exploration log). `ALU.sv`: one-hot `FST`/`SEC` and
  `SEL_ADD` from the microcode (`FALUCtrl_r` is 25 bits) turn every select
  into one AND-OR level; bit-exact against the stock ALU on all 256 control
  words (`../sim/bench/tb_alu_eq.sv`).

Probe variants: `a2mega_cpu_probe_138b_fast_addr.gprj` constrains only the
register → BSRAM-address paths to one 108 MHz clock (`set_max_delay`), so
its report shows the address cone the normal probe hides behind the data cone.
