// a2p25 DP bring-up timing constraints

// 50 MHz SOM oscillator
create_clock -name clk50  -period 20.000 -waveform {0 10.000} [get_ports {clk50_in}]

// 100 MHz AUX/management clock (gowin_mgmt_pll)
create_clock -name clk100 -period 10.000 -waveform {0 5.000} [get_pins {i_mgmt_pll/PLLA_inst/CLKOUT0}]

// 54 MHz D-PHY PLL reference (board PLL, VCO 1350 / 25)
create_clock -name clk54  -period 18.519 -waveform {0 9.259} [get_pins {i_sys_pll/u_pll/PLLA_inst/CLKOUT2}]

// D-PHY PLL: VCO 810 MHz. clkout4 / 10 = 81 MHz link-symbol clock.
create_clock -name clk_sym -period 12.346 -waveform {0 6.173} [get_pins {i_phy/u_dphy/u_pll/PLLA_inst/CLKOUT4}]

// 101.25 MHz D-PHY byte clock (16-bit gearbox), from the hard PHY
create_clock -name clk_tx -period 9.877 -waveform {0 4.938} [get_pins {i_phy/u_dphy/u_mipi_dphy/mipi_dphy_inst/TX_CLK_O}]

// 74.25 MHz pixel clock: 81 MHz * 11 / 12 (gowin_pixel_pll, VCO 891)
create_clock -name clk_pix -period 13.468 -waveform {0 6.734} [get_pins {i_dp/i_pixel_pll/PLLA_inst/CLKOUT0}]

// All domain crossings go through gray-coded FIFOs or 2FF synchronizers.
set_clock_groups -asynchronous -group [get_clocks {clk50}] -group [get_clocks {clk100}] -group [get_clocks {clk54}] -group [get_clocks {clk_sym}] -group [get_clocks {clk_tx}] -group [get_clocks {clk_pix}]
