// Timing constraints — a2mega FULL CORE, GW5AST-138B variant.
// REGENERATED 08-30 from a2mega.sdc: PLL pin paths (u_pll/PLL_inst),
// serdes shim path (i_dp_serdes_138b).
// a2mega (1.0a3) timing constraints — DisplayPort output build.
// History: the HDMI-era file constrained clk_pixel_x5 (TMDS) and clk_usb;
// both domains are gone on 1.0a3 (DP over USB-C, no USB-A host).

// Board crystal -- 50 MHz (used by clk_pll, DDR3 controller, DP mgmt PLL)
create_clock -name clk -period 20 -waveform {0 10} [get_ports {clk}]

// PLL-generated clocks from clk_pll (50 MHz -> 27/54 MHz).
// clk_pixel here is the BOARD PLL 27 MHz tap: on 1.0a3 it only sources
// pll_ddr3 (video runs on the DP core's own pixel clock, clk_pix below).
//
// GW5AST PIN SWAP (differs from the 60B SDC!): the 138B PLL wrapper
// cross-wires ports to physical outputs — exact 54 MHz needs the
// FRACTIONAL divider, which only exists on ODIV0, so physical CLKOUT0
// carries 54 MHz (675 MHz VCO / 12.5, alternating /12 and /13) and
// physical CLKOUT2 carries 27 MHz (see gowin_pll/clk_pll.v and the
// clocks_138b.sv header). The SDC targets PHYSICAL pins, so the pin
// indices here are swapped vs a2mega.sdc. The fractional /12 phases make
// the worst-case instantaneous clk_logic period 675MHz/12 = 17.777 ns
// (56.25 MHz) — constrained as a primary clock at that rate, per the
// clocks_138b.sv prescription (a generated-clock ratio cannot express
// the /12/13 alternation).
create_generated_clock -name clk_pixel -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 27 [get_pins {u_board_plls/clocks_pll/u_pll/PLL_inst/CLKOUT2}]
create_clock -name clk_logic -period 17.777 -waveform {0 8.888} [get_pins {u_board_plls/clocks_pll/u_pll/PLL_inst/CLKOUT0}]

// DDR3 internal clocks -- 324 MHz memory, 81 MHz app clock
create_clock -name clk4x -period 3.086 -waveform {0 1.543} [get_pins {u_board_plls/pll_ddr3_inst/u_pll/PLL_inst/CLKOUT2}]
create_clock -name clk1x -period 12.346 -waveform {0 6.173} [get_pins {u_ddr3/gw3_top/u_ddr_phy_top/fclkdiv/CLKOUT}]

// ---------------------------------------------------------------------
// DisplayPort clocks (see boards/a2mega/hdl/dp_test/a2mega_dp_test.sdc
// for the standalone bring-up variant of the same set)
// ---------------------------------------------------------------------

// 100 MHz management/AUX clock: 50 MHz osc -> gowin_mgmt_pll
create_clock -name clk100 -period 10.000 -waveform {0 5.000} [get_pins {i_mgmt_pll/u_pll/PLL_inst/CLKOUT0}]

// 135 MHz TX word clock from the GTR12 quad (2.7 Gbps / 20), anchored on
// LANE2 -- the bonding master for the a2mega lane pair
create_clock -name clk_sym -period 7.407 -waveform {0 3.703} [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_quad_inst0/LANE2_PCS_TX_O_FABRIC_CLK}]

// 148.5 MHz video pixel clock: gowin_pixel_pll = 135 MHz * 11/10 (1080p)
create_clock -name clk_pix -period 6.734 -waveform {0 3.367} [get_pins {i_dp/i_pixel_pll/u_pll/PLL_inst/CLKOUT0}]

// cm_life = the GTR12 CM block's internal oscillator-derived clock.
// MEASURED on hardware 2026-08-31 (138K board, on-screen freq meter,
// diag bin b0a9c746): 60.4 MHz at power-on cooling to 60.1 MHz warm —
// a 60 MHz nominal RC-oscillator clock (210 MHz reference class, DS981
// Table 3-41: +/-5% commercial, +/-10% extended temp). The old 10 ns
// value had NO provenance ("silence TA1132") and over-constrained by
// ~66%, manufacturing a -300 ns/68-endpoint phantom family on the 138B.
// Constraint = 60 MHz * 1.10 fast-corner guard = 66 MHz -> 15.0 ns.
create_clock -name cm_life -period 15.0 [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_quad_inst0/FABRIC_CM_LIFE_CLK_O}]

// ---------------------------------------------------------------------
// Clock groups. Every cross-domain exchange goes through async FIFOs or
// 2FF synchronizers (framebuffer write clk_logic -> read clk_pix; DP
// pixel CDC clk_pix -> clk_sym; quasi-static debug/OSD CDC). Board and
// DDR3 groups are unchanged from the hardware-proven HDMI-era file.
// ---------------------------------------------------------------------
set_clock_groups -asynchronous -group [get_clocks {clk4x}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_logic}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_logic}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_pixel}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_logic}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk_logic}]

// DP domains vs everything else (and each other)
set_clock_groups -asynchronous -group [get_clocks {clk100}] -group [get_clocks {clk}] -group [get_clocks {clk_logic}] -group [get_clocks {clk_pix}] -group [get_clocks {clk_sym}] -group [get_clocks {cm_life}]
set_clock_groups -asynchronous -group [get_clocks {clk_pix}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pix}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pix}] -group [get_clocks {clk_pixel}]
set_clock_groups -asynchronous -group [get_clocks {clk_sym}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_sym}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_sym}] -group [get_clocks {clk_pixel}]
// cm_life vs the 27 MHz board tap (freq-meter diag CDC; 2FF-synced quasi-static)
set_clock_groups -asynchronous -group [get_clocks {cm_life}] -group [get_clocks {clk_pixel}]

// CSR replay ROM: now a sync BSRAM pROM — no multicycle needed.

// UPAR arbiter multicycles — VENDOR-SANCTIONED: this exact exception set
// (same register names) appears in Gowin's own PCIe reference project SDC
// (via the gowin-serdes RE analysis, docs/gowin_serdes_re_findings.md
// section 3.2). Semantics: addr/wrdata/strb are latched at the end of
// JUDG_ADDR and held stable through the whole UPAR_EN handshake; wren/rden
// are registered — the *_s1 capture registers see multi-cycle-stable data
// by construction. This retires the cm_life wren->CE path family that
// dominated the 138B timing reports.
set_multicycle_path 2 -setup -from [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_wren_o_s1/Q}]
set_multicycle_path 1 -hold  -from [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_wren_o_s1/Q}]
set_multicycle_path 2 -setup -from [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_rden_o_s1/Q}]
set_multicycle_path 1 -hold  -from [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_rden_o_s1/Q}]
set_multicycle_path 2 -setup -from [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_upar_inst/UPAR_READY_S}]
set_multicycle_path 1 -hold  -from [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_upar_inst/UPAR_READY_S}]
set_multicycle_path 2 -setup -to [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_addr_o_*_s1/D}]
set_multicycle_path 1 -hold  -to [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_addr_o_*_s1/D}]
set_multicycle_path 2 -setup -to [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_strb_o_*_s1/D}]
set_multicycle_path 1 -hold  -to [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_strb_o_*_s1/D}]
set_multicycle_path 2 -setup -to [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_wrdata_o_*_s1/D}]
set_multicycle_path 1 -hold  -to [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/upar_wrdata_o_*_s1/D}]

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
set_clock_uncertainty 0.5 -setup -from [get_clocks {clk_logic}]

// ---------------------------------------------------------------------
// IIgs CPU socket (hdl/twgs/gs_socket.sdc is the reference copy).
// gs_ph2 = the FPI's PHI2 at the socket: 2.86 MHz fast cycle, ~140 ns
// low / ~209 ns high; the 65C816 core is clocked by its inversion (active
// edge = the fall).  Period = the FAST cycle; sync/slow cycles only add
// slack.  AB17 is not a dedicated clock ball on 1.0a3 - PnR routes the
// clock generically, which is expected and harmless at 2.86 MHz.
// The pin sequencer runs on clk100 (shared with DP management; a
// dedicated PLL clock would consume the die's 8th and last PRIMARY net
// and push gs_ph2 onto long wires - see top.sv).  gs_ph2 is asynchronous
// to everything: the crossings are a two-flop toggle sync, quasi-static
// control/telemetry, and the core's quasi-static outputs sampled >= 2
// clk100 after the fall by design (gs_socket_phy OUT_DELAY; the core's
// longest path bounds its output settle at ~27 ns - keep OUT_DELAY >= 2).
// ---------------------------------------------------------------------
create_clock -name gs_ph2 -period 349.0 -waveform {209.4 349.0} [get_ports {gs_ph2}]
set_clock_groups -asynchronous -group [get_clocks {gs_ph2}] -group [get_clocks {clk100}] -group [get_clocks {clk}] -group [get_clocks {clk_logic}] -group [get_clocks {clk_pixel}] -group [get_clocks {clk_pix}] -group [get_clocks {clk_sym}] -group [get_clocks {cm_life}] -group [get_clocks {clk4x}] -group [get_clocks {clk1x}]
