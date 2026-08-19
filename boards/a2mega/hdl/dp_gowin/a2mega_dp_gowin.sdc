// Timing constraints — Gowin EDP Encoder + EDP PHY stack (WS4),
// 1080p @ 2-lane HBR, X40 geometry (1:2 gear — see WS4 forensics §10).
// Mirrors hdl/dp_test/a2mega_dp_test.sdc; the clk_sym anchor survives
// because the EDP PHY emission instantiates the same GTR12_QUADA
// (gtr12_quad_inst0) with LANE2 as the TX clock master.

// 50 MHz board oscillator -> gowin_mgmt_pll -> 100 MHz AUX clock
create_clock -name clk50  -period 20.000 -waveform {0 10.000} [get_ports {clk50_in}]
create_clock -name clk100 -period 10.000 -waveform {0 5.000} [get_pins {i_mgmt_pll/PLLA_inst/CLKOUT0}]

// 67.5 MHz TX word clock from the GTR12 quad (2.7 Gbps / 40 at 1:2
// gear), anchored on LANE2 — the a2mega TX clock master
create_clock -name clk_sym -period 14.815 -waveform {0 7.407} [get_pins {i_dp/i_edp_phy_bank/i_dp_serdes/gtr12_quad_inst0/LANE2_PCS_TX_O_FABRIC_CLK}]

// 37.125 MHz stream clock (4 px/clk): gowin_pixel_pll = 67.5 * 44/2 VCO / 40
create_clock -name clk_strm -period 26.936 -waveform {0 13.468} [get_pins {i_dp/i_strm_pll/PLLA_inst/CLKOUT0}]

// GTR12 housekeeping clock (unused in fabric; constrain to silence TA1132)
create_clock -name cm_life -period 10.000 [get_pins {i_dp/i_edp_phy_bank/i_dp_serdes/gtr12_quad_inst0/FABRIC_CM_LIFE_CLK_O}]

// Domains exchange data only through the encoder's line-buffer FIFO and
// 2FF synchronisers; no synchronous cross-domain paths exist. (clk_strm
// is frequency-locked to clk_sym but phase-unmanaged — all crossings go
// through the encoder's internal buffer.)
set_clock_groups -asynchronous -group [get_clocks {clk50}] -group [get_clocks {clk100}] -group [get_clocks {clk_sym}] -group [get_clocks {clk_strm}] -group [get_clocks {cm_life}]
