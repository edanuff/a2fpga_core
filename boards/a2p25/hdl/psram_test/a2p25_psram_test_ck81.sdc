// a2p25 standalone HyperRAM test — CK 81 MHz variant
create_clock -name clk50 -period 20.000 -waveform {0 10.000} [get_ports {clk50_in}]

// One PLLA: clk = CK/2, clk_mem = CK, clk_mem_90 = CK + 90 deg. All three are
// phase-related (rising edges of clk and clk_mem coincide) — NOT async.
create_clock -name clk        -period 24.691 -waveform {0 12.345} [get_pins {i_pll/PLLA_inst/CLKOUT0}]
create_clock -name clk_mem    -period 12.346 -waveform {0 6.173} [get_pins {i_pll/PLLA_inst/CLKOUT1}]
create_clock -name clk_mem_90 -period 12.346 -waveform {3.086 9.259} [get_pins {i_pll/PLLA_inst/CLKOUT2}]

// Quasi-static IODELAY code (changes only during calibration / eye scan,
// followed by a settle wait before any capture is used)
set_false_path -from [get_regs {i_ctrl/dly_code*}]

// 54 MHz board-PLL output feeding the HyperRAM PLL (no fabric logic on it)
create_clock -name clk54 -period 18.519 -waveform {0 9.259} [get_pins {i_pll0/u_pll/PLLA_inst/CLKOUT2}]

set_clock_groups -asynchronous -group [get_clocks {clk50}] -group [get_clocks {clk54}] -group [get_clocks {clk clk_mem clk_mem_90}]
