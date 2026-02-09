//Copyright (C)2014-2023 GOWIN Semiconductor Corporation.
//All rights reserved.
//File Title: Timing Constraints file
//GOWIN Version: 1.9.8.11 Education
//Created Time: 2023-06-02 16:47:45

// Board crystal — 50 MHz (used by clk_pll and as clk_g for DDR3 framebuffer)
create_clock -name clk -period 20 -waveform {0 10} [get_ports {clk}]

// PLL-generated clocks from clk_pll (50 MHz -> 27/54 MHz)
create_generated_clock -name clk_pixel -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 27 [get_pins {clocks_pll/PLLA_inst/CLKOUT0}]
create_generated_clock -name clk_logic -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 54 [get_pins {clocks_pll/PLLA_inst/CLKOUT2}]

// DDR3 framebuffer internal clocks (from reference project constraints)
create_clock -name clk4x -period 3.367 -waveform {0 1.684} [get_pins {u_ddr3_fb/pll_ddr3_inst/PLLA_inst/CLKOUT2}]
create_clock -name clk1x -period 13.47 -waveform {0 6.734} [get_pins {u_ddr3_fb/u_ddr3/gw3_top/u_ddr_phy_top/fclkdiv/CLKOUT}]

// All clock domains are asynchronous — CDC handled by synchronizers/async FIFOs
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk4x}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_logic}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_logic}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk1x}]

// clk <-> clk_logic: 50 MHz crystal vs 54 MHz PLL output.
// These are related (same PLL) but different frequencies, and the apple_bus
// module uses CDC to synchronize Apple II bus signals between them.
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_logic}]

// clk_g inside ddr3_framebuffer is the 50 MHz board crystal buffered onto a
// separate internal net. After synthesis, the top-level 'clk' port becomes
// 'clk_d' in the netlist — this is the same physical clock used as clk_g.
create_clock -name clk_g -period 20 -waveform {0 10} [get_nets {clk_d}]
set_clock_groups -asynchronous -group [get_clocks {clk_g}] -group [get_clocks {clk_logic}]
set_clock_groups -asynchronous -group [get_clocks {clk_g}] -group [get_clocks {clk_pixel}]
set_clock_groups -asynchronous -group [get_clocks {clk_g}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_g}] -group [get_clocks {clk4x}]
