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
