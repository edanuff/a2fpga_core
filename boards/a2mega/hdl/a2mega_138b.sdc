// Timing constraints — Tang Mega 138K SOM variant (GW5AST-138B)
//
// Differences vs a2mega.sdc (60B):
//  - clk_pll / pll_ddr3 use the GW5AST PLL primitive (instance path
//    .../u_pll/PLL_inst), not PLLA.
//  - The clk_pll wrapper cross-wires ports to keep top.sv's semantics:
//    physical CLKOUT2 = 27 MHz pixel, CLKOUT1 = 135 MHz TMDS,
//    CLKOUT0 = 54 MHz logic via the FRACTIONAL divider (675 MHz VCO /
//    12.5, alternating /12 and /13). The short cycle is 17.78 ns, so
//    clk_logic is constrained as an independent 56.25 MHz clock (it is
//    async-grouped from every other domain, so losing the generated-clock
//    relationship to clk is harmless).

// Board crystal -- 50 MHz (used by clk_pll and DDR3 controller)
create_clock -name clk -period 20 -waveform {0 10} [get_ports {clk}]

// PLL-generated clocks from clk_pll (50 MHz -> 27/135/54 MHz, VCO 675 MHz)
create_generated_clock -name clk_pixel -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 27 [get_pins {u_board_plls/clocks_pll/u_pll/PLL_inst/CLKOUT2}]
create_generated_clock -name clk_pixel_x5 -source [get_ports {clk}] -master_clock clk -divide_by 50 -multiply_by 135 [get_pins {u_board_plls/clocks_pll/u_pll/PLL_inst/CLKOUT1}]
// 54 MHz nominal, fractional /12.5: constrain at the short /12 cycle
// (675 MHz / 12 = 56.25 MHz, 17.777 ns) so logic closes on the worst case.
create_clock -name clk_logic -period 17.777 -waveform {0 8.888} [get_pins {u_board_plls/clocks_pll/u_pll/PLL_inst/CLKOUT0}]

// DDR3 internal clocks -- 324 MHz memory, 81 MHz app clock
create_clock -name clk4x -period 3.086 -waveform {0 1.543} [get_pins {u_board_plls/pll_ddr3_inst/u_pll/PLL_inst/CLKOUT2}]
create_clock -name clk1x -period 12.346 -waveform {0 6.173} [get_pins {u_ddr3/gw3_top/u_ddr_phy_top/fclkdiv/CLKOUT}]

// DDR3 IP internal clocks: clk1x (app) and clk4x (PHY) are managed by the
// IP's calibration mechanism. STA cannot verify these internal paths.
set_clock_groups -asynchronous -group [get_clocks {clk4x}] -group [get_clocks {clk1x}]

// Async groups: clk (50 MHz board crystal) vs DDR3 domain
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk1x}]

// clk_pixel (27 MHz) vs DDR3 domain -- truly async (different PLL sources)
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk1x}]

// clk_logic (54 MHz) vs DDR3 domain -- fully async (independent PLLs)
set_clock_groups -asynchronous -group [get_clocks {clk_logic}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_logic}] -group [get_clocks {clk1x}]

// clk_pixel_x5 (135 MHz TMDS) -- async to all other domains
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk1x}]
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk_logic}]

// clk <-> clk_pixel / clk_pixel_x5 / clk_logic
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_pixel}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_pixel_x5}]
set_clock_groups -asynchronous -group [get_clocks {clk}] -group [get_clocks {clk_logic}]

// clk_pixel_x5 is related to clk_pixel (same PLL) but runs at 5x for TMDS
// Keep them in separate async groups since they drive different logic
set_clock_groups -asynchronous -group [get_clocks {clk_pixel_x5}] -group [get_clocks {clk_pixel}]

// clk_logic and clk_pixel are from the same PLL but at different frequencies.
// CDC between them uses double-flop synchronizers; mark async for STA.
set_clock_groups -asynchronous -group [get_clocks {clk_pixel}] -group [get_clocks {clk_logic}]

// USB host clock -- 60 MHz from dedicated pll_usb (GW5AST PLL + PLL_INIT,
// VCO 900 MHz = 50 x 18, /15 = 60 MHz integer)
create_generated_clock -name clk_usb -source [get_ports {clk}] -master_clock clk -divide_by 5 -multiply_by 6 [get_pins {u_board_plls/pll_usb_inst/u_pll/PLL_inst/CLKOUT0}]

// clk_usb -- own PLL, self-contained USB domain; async to everything.
// HID outputs must cross into consumer domains via double-flop synchronizers.
set_clock_groups -asynchronous -group [get_clocks {clk_usb}] -group [get_clocks {clk}]
set_clock_groups -asynchronous -group [get_clocks {clk_usb}] -group [get_clocks {clk_logic}]
set_clock_groups -asynchronous -group [get_clocks {clk_usb}] -group [get_clocks {clk_pixel}]
set_clock_groups -asynchronous -group [get_clocks {clk_usb}] -group [get_clocks {clk_pixel_x5}]
set_clock_groups -asynchronous -group [get_clocks {clk_usb}] -group [get_clocks {clk4x}]
set_clock_groups -asynchronous -group [get_clocks {clk_usb}] -group [get_clocks {clk1x}]
