// Timing constraints — dp_transmitter with generated SERDES + PLL IP
// (1080p60 @ HBR x2 production rates). Copied from the timing-verified
// DisplayPort_Verilog examples/a2_mega/a2_mega_dp.sdc; instance names
// (i_mgmt_pll, i_dp) match a2mega_dp_test_top.sv.

// 50 MHz board oscillator -> gowin_mgmt_pll -> 100 MHz AUX clock
create_clock -name clk50  -period 20.000 -waveform {0 10.000} [get_ports {clk50_in}]
create_clock -name clk100 -period 10.000 -waveform {0 5.000} [get_pins {i_mgmt_pll/PLLA_inst/CLKOUT0}]

// 135 MHz TX word clock from the GTR12 quad (2.7 Gbps / 20), anchored on
// LANE2 — the bonding master for the a2mega lane pair
create_clock -name clk_sym -period 7.407 -waveform {0 3.703} [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/gtr12_quad_inst0/LANE2_PCS_TX_O_FABRIC_CLK}]

// 148.5 MHz pixel clock: gowin_pixel_pll = 135 * 44/5 VCO / 8
create_clock -name clk_pix -period 6.734 -waveform {0 3.367} [get_pins {i_dp/i_pixel_pll/PLLA_inst/CLKOUT0}]

// GTR12 housekeeping clock (unused in fabric; constrain to silence TA1132)
create_clock -name cm_life -period 10.000 [get_pins {i_dp/i_transceiver_bank/i_dp_serdes/gtr12_quad_inst0/FABRIC_CM_LIFE_CLK_O}]

// Domains exchange data only through the gray-coded CDC FIFO and 2FF
// synchronisers; no synchronous cross-domain paths exist.
set_clock_groups -asynchronous -group [get_clocks {clk50}] -group [get_clocks {clk100}] -group [get_clocks {clk_sym}] -group [get_clocks {clk_pix}] -group [get_clocks {cm_life}]
