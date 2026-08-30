// a2mega (1.0a3) timing constraints — DisplayPort output build.
// History: the HDMI-era file constrained clk_pixel_x5 (TMDS) and clk_usb;
// both domains are gone on 1.0a3 (DP over USB-C, no USB-A host).

// Board crystal -- 50 MHz (used by clk_pll, DDR3 controller, DP mgmt PLL)
create_clock -name clk -period 20 -waveform {0 10} [get_ports {clk}]

// PLL-generated clocks from clk_pll (50 MHz -> 27/54 MHz).
// clk_pixel here is the BOARD PLL 27 MHz tap: on 1.0a3 it only sources
// pll_ddr3 (video runs on the DP core's own pixel clock, clk_pix below).
create_generated_clock -name clk_pixel -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 27 [get_pins {u_board_plls/clocks_pll/PLLA_inst/CLKOUT0}]
create_generated_clock -name clk_logic -source [get_ports {clk}] -master_clock clk -divide_by 25 -multiply_by 27 [get_pins {u_board_plls/clocks_pll/PLLA_inst/CLKOUT2}]

// DDR3 internal clocks -- 324 MHz memory, 81 MHz app clock
create_clock -name clk4x -period 3.086 -waveform {0 1.543} [get_pins {u_board_plls/pll_ddr3_inst/PLLA_inst/CLKOUT2}]
create_clock -name clk1x -period 12.346 -waveform {0 6.173} [get_pins {u_ddr3/gw3_top/u_ddr_phy_top/fclkdiv/CLKOUT}]

// ---------------------------------------------------------------------
// DisplayPort clocks (see boards/a2mega/hdl/dp_test/a2mega_dp_test.sdc
// for the standalone bring-up variant of the same set)
// ---------------------------------------------------------------------

// 100 MHz management/AUX clock: 50 MHz osc -> gowin_mgmt_pll
create_clock -name clk100 -period 10.000 -waveform {0 5.000} [get_pins {i_mgmt_pll/PLLA_inst/CLKOUT0}]

// 135 MHz TX word clock from the GTR12 quad (2.7 Gbps / 20), anchored on
// LANE2 -- the bonding master for the a2mega lane pair
create_clock -name clk_sym -period 7.407 -waveform {0 3.703} [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/gtr12_quad_inst0/LANE2_PCS_TX_O_FABRIC_CLK}]

// 148.5 MHz video pixel clock: gowin_pixel_pll = 135 MHz * 11/10 (1080p)
create_clock -name clk_pix -period 6.734 -waveform {0 3.367} [get_pins {i_dp/i_pixel_pll/PLLA_inst/CLKOUT0}]

// GTR12 housekeeping clock (unused in fabric; constrain to silence TA1132)
create_clock -name cm_life -period 10.000 [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/gtr12_quad_inst0/FABRIC_CM_LIFE_CLK_O}]

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


// CSR replay ROM lookup (same-clock multicycle; the replay FSM holds
// replay_idx >=16 cycles before the capture — see the dp_test_138b SDC
// for the full rationale; added to the 60K full core 08-30 when its
// tighter placement started violating the same path family)
set_multicycle_path 2 -setup -from [get_regs {i_dp/i_transceiver_bank/replay_idx*}] -to [get_regs {i_dp/i_transceiver_bank/drp_addr_r* i_dp/i_transceiver_bank/drp_wrdata_r*}]
set_multicycle_path 1 -hold  -from [get_regs {i_dp/i_transceiver_bank/replay_idx*}] -to [get_regs {i_dp/i_transceiver_bank/drp_addr_r* i_dp/i_transceiver_bank/drp_wrdata_r*}]
