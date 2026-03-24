//Copyright (C)2014-2023 GOWIN Semiconductor Corporation.
//All rights reserved.
//File Title: Timing Constraints file
//GOWIN Version: 1.9.8.11 Education
//Created Time: 2023-06-02 16:47:45

// Board crystal — 50 MHz (used by clk_pll and DDR3 controller)
create_clock -name clk -period 20 -waveform {0 10} [get_ports {clk}]

// PLL-generated clocks from clk_pll (50 MHz -> 27/135 MHz)
create_generated_clock -name clk_pixel -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 27 [get_pins {clocks_pll/PLLA_inst/CLKOUT0}]
create_generated_clock -name clk_pixel_x5 -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 135 [get_pins {clocks_pll/PLLA_inst/CLKOUT1}]

// DDR3 internal clocks — 432 MHz memory, 108 MHz app clock
create_clock -name clk4x -period 2.315 -waveform {0 1.157} [get_pins {pll_ddr3_inst/u_pll/PLLA_inst/CLKOUT2}]
create_clock -name clk1x -period 9.259 -waveform {0 4.630} [get_pins {u_ddr3/gw3_top/u_ddr_phy_top/fclkdiv/CLKOUT}]

// clk_logic is derived from clk1x via CLKDIV2 (synchronous with DDR3, 54 MHz)
create_generated_clock -name clk_logic -source [get_pins {u_ddr3/gw3_top/u_ddr_phy_top/fclkdiv/CLKOUT}] -divide_by 2 [get_pins {clkdiv2_logic/CLKOUT}]

// Async groups: clk (50 MHz board crystal) vs DDR3 domain
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_logic}]

// clk_pixel (27 MHz) vs DDR3 domain — truly async (different PLL sources)
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk_logic}]

// clk_pixel_x5 (135 MHz TMDS) — async to all other domains
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk_logic}]

// clk <-> clk_pixel / clk_pixel_x5: same PLL but different frequencies
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_pixel}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_pixel_x5}]

// clk_pixel_x5 is related to clk_pixel (same PLL) but runs at 5x for TMDS
// Keep them in separate async groups since they drive different logic
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk_pixel}]

// Multicycle paths: clk_logic (54 MHz) is CLKDIV2 of clk1x (108 MHz).
// Data from clk_logic changes every 2 clk1x cycles. The CDC register in
// clk1x captures within that 2-cycle window. Allow 2 clk1x cycles for
// setup, and adjust hold accordingly.
set_multicycle_path 2 -setup -from [get_clocks {clk_logic}] -to [get_clocks {clk1x}]
set_multicycle_path 1 -hold -from [get_clocks {clk_logic}] -to [get_clocks {clk1x}]
