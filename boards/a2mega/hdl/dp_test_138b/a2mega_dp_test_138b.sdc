// Timing constraints — a2mega_dp_test on the GW5AST-138B SOM.
//
// IDENTICAL to hdl/dp_test/a2mega_dp_test.sdc except for the PLL pin paths:
// the 60B's PLLA wrapper exposes CLKOUT0 on `PLLA_inst`, while the 138B
// wrappers (hdl/dp_test_138b/gowin_plls_138b.sv) instantiate the GW5AST PLL
// primitive as `u_pll/PLL_inst`.
//
// The clk_sym anchor is UNCHANGED between the dies. The carrier's DP1 pair
// (A6/B6) is Q0 lane 2 on BOTH the GW5AT-60B and the GW5AST-138B
// (docs/SOM_138B_BALLMAP.md §2), and DP1 is the TX master, so the bonding
// master's fabric clock pin is still LANE2_PCS_TX_O_FABRIC_CLK. Only the
// carrier's DP0 pair moves (die lane 3 -> die lane 1), and it is not a clock
// source.
//
// Instance names: i_mgmt_pll / i_dp come from a2mega_dp_test_top.sv (shared,
// unmodified); i_dp_serdes_138b is the emission inside the lane shim
// (hdl/dp_test_138b/dp_serdes_lane_shim.sv).

// 50 MHz board oscillator -> gowin_mgmt_pll -> 100 MHz AUX clock
create_clock -name clk50  -period 20.000 -waveform {0 10.000} [get_ports {clk50_in}]
create_clock -name clk100 -period 10.000 -waveform {0 5.000} [get_pins {i_mgmt_pll/u_pll/PLL_inst/CLKOUT0}]

// 135 MHz TX word clock from the GTR12 quad (2.7 Gbps / 20), anchored on
// LANE2 — the bonding master for the a2mega lane pair (same lane on both dies)
create_clock -name clk_sym -period 7.407 -waveform {0 3.703} [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_quad_inst0/LANE2_PCS_TX_O_FABRIC_CLK}]

// 148.5 MHz pixel clock: gowin_pixel_pll = 135 * 44/5 VCO / 8
create_clock -name clk_pix -period 6.734 -waveform {0 3.367} [get_pins {i_dp/i_pixel_pll/u_pll/PLL_inst/CLKOUT0}]

// GTR12 housekeeping clock — the CSR replay / DRP FSM in
// transceiver_bank_gowin.v runs on it (the "unused in fabric" note
// predated the replay machinery)
create_clock -name cm_life -period 15.0 [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_quad_inst0/FABRIC_CM_LIFE_CLK_O}]

// CSR replay ROM: now a sync BSRAM pROM (csr_replay_rom_lut has a
// registered read) — the historical multicycle crutch is gone; the +1
// read latency is absorbed by the same >=16-cycle rd_gap pacing that
// justified it.

// Domains exchange data only through the gray-coded CDC FIFO and 2FF
// synchronisers; no synchronous cross-domain paths exist.
set_clock_groups -asynchronous -group [get_clocks {clk50}] -group [get_clocks {clk100}] -group [get_clocks {clk_sym}] -group [get_clocks {clk_pix}] -group [get_clocks {cm_life}]

// ---------------------------------------------------------------------
// Durability margin (timing campaign round 2, 2026-09-05). Setup
// uncertainty of 0.5 ns on the fabric clocks makes the durability bar a
// property of the build: "0 setup violations" now means every path has
// at least 0.5 ns of real margin, and the timing-driven placer optimises
// against that target (diagnostic builds with a clock tightened by 1 ns
// closed clean on the 138B, so the margin is there to be found). This is
// a STRICTER requirement, not an exception; reported slack is after the
// uncertainty. Do not remove to "fix" a violation.
// ---------------------------------------------------------------------
set_clock_uncertainty 0.5 -setup -from [get_clocks {clk100}]
set_clock_uncertainty 0.5 -setup -from [get_clocks {clk_sym}]
set_clock_uncertainty 0.5 -setup -from [get_clocks {clk_pix}]
