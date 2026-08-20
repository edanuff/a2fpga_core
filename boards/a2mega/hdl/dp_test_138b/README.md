# `a2mega_dp_test_138b` — DP colorbars bring-up on the GW5AST-138B SOM

Functional twin of `a2mega_dp_test` (2-lane HBR DisplayPort colorbars, AUX
TLVDS front end, telemetry UART, LED ladder) targeting the **Tang Mega 138K**
SOM instead of the 60K. The top level, the whole DP core, and the telemetry /
LED semantics are **the shared, unmodified sources** — `hdl/displayport/**`
and `hdl/dp_test/a2mega_dp_test_top.sv` are byte-identical between the two
projects, so bench procedure and log parsing are unchanged (stream with
`tools/fpga_stream.py`, classify on the `C:` / `Y:` fields).

## Status

| Piece | State |
|---|---|
| Ball map / pin constraints | **done** — audited ball-by-ball, `a2mega_dp_test_138b.cst` |
| Timing constraints | **done** — `a2mega_dp_test_138b.sdc` (clk_sym anchor unchanged) |
| Lane remap | **done** — `dp_serdes_lane_shim.sv` (no shared-RTL change) |
| PLLs | **done** — `gowin_plls_138b.sv` + `pll/` (PLLA does not exist on this die) |
| Project / SecurityBit | **done** — `a2mega_dp_test_138b.gprj`, `SECURITY_BIT: false` |
| **DP SERDES IP emission** | ⛔ **blocked — must be generated in the IDE GUI** |
| Build / timing / `.fs` | ⛔ **not run** — blocked on the above |

**There is no bitstream to flash yet, and nothing in this directory should be
flashed.** The blocker is not effort or polish: the 138 die uses a different
transceiver primitive (`GTR12_QUAD` vs the 60B's `GTR12_QUADA`, 43 ports
different) and a different CSR schema, so the emission cannot be hand-adapted
or produced headlessly. Full evidence and the ~5-minute GUI checklist are in
**[`../gowin/138B/dp_serdes/README.md`](../gowin/138B/dp_serdes/README.md)**.

## What differs from the 60B project (complete list)

1. **`<Device>`** → `GW5AST-138B` / `GW5AST-LV138PG484AC1/I0` (`gw5ast138b-011`).
2. **SERDES lane pair** → Q0 **lanes 1 + 2** (60B: 2 + 3), master still lane 2.
   Carrier DP0 (C7/D7) is `Q0_LN3` on the 60B but `Q0_LN1` on the 138B;
   carrier DP1 (A6/B6) is `Q0_LN2` on both. See `docs/SOM_138B_BALLMAP.md` §2.
   Bridged by `dp_serdes_lane_shim.sv` so the shared transceiver bank's
   existing `DP_SERDES_LANES_23` branch is reused unchanged.
3. **PLLs** → `gowin_plls_138b.sv` replaces `hdl/displayport/gowin/gowin_plls.v`
   in the file list: the GW5AST die has **no PLLA resource** (PnR RP0008), so
   `gowin_mgmt_pll` (50→100 MHz) and `gowin_pixel_pll` (135→148.5 MHz) are
   rebuilt on the PLL + `PLL_INIT` primitive. Pixel ratios are unchanged
   (IDIV 5 / MDIV 44 / ODIV0 8). ⚠ See the PLL_INIT clock caveat in that file's
   header — the pixel PLL's calibration clock is the *gated* SERDES word clock.
4. **`button` (AB13)** → `LVCMOS33` (bank 5 on this die, all-3.3 V) instead of
   `LVCMOS15` (bank 9, shared with DDR3, on the 60B).
5. **`.sdc` PLL pin paths** → `u_pll/PLL_inst/CLKOUT0` instead of
   `PLLA_inst/CLKOUT0`; the `clk_sym` and `cm_life` anchors gain the
   `i_dp_serdes_138b/` level from the shim. The GTR12 anchor *pin name* is
   unchanged (`LANE2_PCS_TX_O_FABRIC_CLK`) because DP1 is lane 2 on both dies.

Everything else — defines (`hdl/dp_gowin_defines.v`, shared), colorbars, AUX
front end, ladder policy, telemetry format — is common.

## Files

```
boards/a2mega/a2mega_dp_test_138b.gprj            project (SecurityBit OFF)
boards/a2mega/a2mega_dp_test_138b.gprj.user       carries RES.serdes.csr path
boards/a2mega/impl/a2mega_dp_test_138b_process_config.json
boards/a2mega/hdl/dp_test_138b/
  a2mega_dp_test_138b.cst      pins (60B set + the AB13 bank delta)
  a2mega_dp_test_138b.sdc      timing (PLL pin paths + shim level)
  dp_serdes_lane_shim.sv       60B-named dp_serdes face over the 138B emission
  gowin_plls_138b.sv           PLL+PLL_INIT gowin_mgmt_pll / gowin_pixel_pll
  pll/gowin_mgmt_pll_mod.v     PLL primitive, IDIV 1 / MDIV 20 / ODIV0 10
  pll/gowin_pixel_pll_mod.v    PLL primitive, IDIV 5 / MDIV 44 / ODIV0 8
  pll/*.mod                    GowinModgen flag files for regeneration
boards/a2mega/hdl/gowin/138B/dp_serdes/
  README.md                    ⛔ the blocker + the GUI generation checklist
  *.reference                  60B config translated to 138B lane numbers
```

The two `pll/*_mod.v` files were derived **mechanically** from the IDE-generated
`hdl/gowin/138B/gowin_pll/pll_usb_mod.v` (same die, same part): only the module
name and four ratio `defparam`s differ; every port, tie-off and other defparam
is byte-identical to the vendor emission. `GowinModgen` could not be driven
headlessly in this environment (exits 1 with no diagnostic even on the
untouched vendor `.mod`), so the `.mod` flag files are supplied for
regeneration in the IDE alongside the SERDES step — worth doing in the same
GUI session as a cross-check.

## Build (after the GUI generation step)

```
GW_SH=/Users/edanuff/gowin-1.9.12-ide/bin/gw_sh \
GPRJ=a2mega_dp_test_138b.gprj tools/build.sh a2mega
```

Gate before flashing: 0 setup / 0 hold violations, TNS 0.000, Fmax ≥ constraint
on every domain (`clk_sym` ≥ 135.007 MHz is the one that matters), and
`//SecurityBit: OFF` present in `impl/pnr/a2mega_dp_test_138b.fs`. GW5A PnR has
seed variance — re-run once before diagnosing a violation as a regression.
