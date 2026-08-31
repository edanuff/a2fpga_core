// Timing constraints — a2mega FULL CORE, GW5AST-138B variant.
// REGENERATED 08-30 from a2mega.sdc: PLL pin paths (u_pll/PLL_inst),
// serdes shim path (i_dp_serdes_138b), + the cm_life CSR-ROM multicycle.
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

// GTR12 housekeeping clock (unused in fabric; constrain to silence TA1132)
create_clock -name cm_life -period 10.000 [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/i_dp_serdes_138b/gtr12_quad_inst0/FABRIC_CM_LIFE_CLK_O}]

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


// CSR replay ROM lookup (138B: 399-entry case, deep since the per-die
// MODULE conversion) — same-clock multicycle, FSM holds idx >=16 cycles
// (see the dp_test_138b SDC for the full rationale).
set_multicycle_path 2 -setup -from [get_regs {i_dp/i_transceiver_bank/replay_idx*}] -to [get_regs {i_dp/i_transceiver_bank/drp_addr_r* i_dp/i_transceiver_bank/drp_wrdata_r*}]
set_multicycle_path 1 -hold  -from [get_regs {i_dp/i_transceiver_bank/replay_idx*}] -to [get_regs {i_dp/i_transceiver_bank/drp_addr_r* i_dp/i_transceiver_bank/drp_wrdata_r*}]
