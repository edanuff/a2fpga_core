// Derived MECHANICALLY from the IDE-generated GW5AST-138B PLL MOD
// boards/a2mega/hdl/gowin/138B/gowin_pll/pll_usb_mod.v (Gowin IDE
// V1.9.12.01 emission, part GW5AST-LV138PG484AC1/I0): the ONLY edits are
// the module name and the four ratio defparams below. Every port, tie-off
// and remaining defparam is byte-identical to the vendor emission.
// GowinModgen could not be driven headlessly in this environment (exits 1
// with no diagnostic even on the untouched vendor .mod), so the companion
// .mod flag files are provided for regeneration in the IDE - see
// boards/a2mega/hdl/dp_test_138b/README.md.
//
// GW5AST PLL envelope (PnR PA2078): PFD = FCLKIN/IDIV in 19..81.25 MHz,
// VCO = FCLKIN*MDIV/IDIV in 650..1300 MHz.
//
// gowin_pixel_pll (138B): 135 MHz SERDES word clock -> 148.5 MHz pixel
// clock (1080p, 2-lane HBR). PFD = 135/5 = 27 MHz (ok),
// VCO = 135*44/5 = 1188 MHz (ok), /8 = 148.500 MHz - the SAME ratios the
// 60B PLLA wrapper uses (IDIV 5 / MDIV 44 / ODIV0 8).

module gowin_pixel_pll_MOD (lock, clkout0, clkin, reset, icpsel, lpfres, lpfcap);

output lock;
output clkout0;
input clkin;
input reset;
input [5:0] icpsel;
input [2:0] lpfres;
input [1:0] lpfcap;

wire clkout1;
wire clkout2;
wire clkout3;
wire clkout4;
wire clkout5;
wire clkout6;
wire clkfbout;
wire gw_vcc;
wire gw_gnd;

assign gw_vcc = 1'b1;
assign gw_gnd = 1'b0;

PLL PLL_inst (
    .LOCK(lock),
    .CLKOUT0(clkout0),
    .CLKOUT1(clkout1),
    .CLKOUT2(clkout2),
    .CLKOUT3(clkout3),
    .CLKOUT4(clkout4),
    .CLKOUT5(clkout5),
    .CLKOUT6(clkout6),
    .CLKFBOUT(clkfbout),
    .CLKIN(clkin),
    .CLKFB(gw_gnd),
    .RESET(reset),
    .PLLPWD(gw_gnd),
    .RESET_I(gw_gnd),
    .RESET_O(gw_gnd),
    .FBDSEL({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .IDSEL({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .MDSEL({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .MDSEL_FRAC({gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL0({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL0_FRAC({gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL1({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL2({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL3({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL4({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL5({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ODSEL6({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .DT0({gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .DT1({gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .DT2({gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .DT3({gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .ICPSEL(icpsel),
    .LPFRES(lpfres),
    .LPFCAP(lpfcap),
    .PSSEL({gw_gnd,gw_gnd,gw_gnd}),
    .PSDIR(gw_gnd),
    .PSPULSE(gw_gnd),
    .ENCLK0(gw_vcc),
    .ENCLK1(gw_vcc),
    .ENCLK2(gw_vcc),
    .ENCLK3(gw_vcc),
    .ENCLK4(gw_vcc),
    .ENCLK5(gw_vcc),
    .ENCLK6(gw_vcc),
    .SSCPOL(gw_gnd),
    .SSCON(gw_gnd),
    .SSCMDSEL({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .SSCMDSEL_FRAC({gw_gnd,gw_gnd,gw_gnd})
);

defparam PLL_inst.FCLKIN = "135";
defparam PLL_inst.IDIV_SEL = 5;
defparam PLL_inst.FBDIV_SEL = 1;
defparam PLL_inst.ODIV0_SEL = 8;
defparam PLL_inst.ODIV1_SEL = 5;
defparam PLL_inst.ODIV2_SEL = 25;
defparam PLL_inst.ODIV3_SEL = 8;
defparam PLL_inst.ODIV4_SEL = 8;
defparam PLL_inst.ODIV5_SEL = 8;
defparam PLL_inst.ODIV6_SEL = 8;
defparam PLL_inst.MDIV_SEL = 44;
defparam PLL_inst.MDIV_FRAC_SEL = 0;
defparam PLL_inst.ODIV0_FRAC_SEL = 0;
defparam PLL_inst.CLKOUT0_EN = "TRUE";
defparam PLL_inst.CLKOUT1_EN = "FALSE";
defparam PLL_inst.CLKOUT2_EN = "FALSE";
defparam PLL_inst.CLKOUT3_EN = "FALSE";
defparam PLL_inst.CLKOUT4_EN = "FALSE";
defparam PLL_inst.CLKOUT5_EN = "FALSE";
defparam PLL_inst.CLKOUT6_EN = "FALSE";
defparam PLL_inst.CLKFB_SEL = "INTERNAL";
defparam PLL_inst.CLKOUT0_DT_DIR = 1'b1;
defparam PLL_inst.CLKOUT1_DT_DIR = 1'b1;
defparam PLL_inst.CLKOUT2_DT_DIR = 1'b1;
defparam PLL_inst.CLKOUT3_DT_DIR = 1'b1;
defparam PLL_inst.CLKOUT0_DT_STEP = 0;
defparam PLL_inst.CLKOUT1_DT_STEP = 0;
defparam PLL_inst.CLKOUT2_DT_STEP = 0;
defparam PLL_inst.CLKOUT3_DT_STEP = 0;
defparam PLL_inst.CLK0_IN_SEL = 1'b0;
defparam PLL_inst.CLK0_OUT_SEL = 1'b0;
defparam PLL_inst.CLK1_IN_SEL = 1'b0;
defparam PLL_inst.CLK1_OUT_SEL = 1'b0;
defparam PLL_inst.CLK2_IN_SEL = 1'b0;
defparam PLL_inst.CLK2_OUT_SEL = 1'b0;
defparam PLL_inst.CLK3_IN_SEL = 1'b0;
defparam PLL_inst.CLK3_OUT_SEL = 1'b0;
defparam PLL_inst.CLK4_IN_SEL = 2'b00;
defparam PLL_inst.CLK4_OUT_SEL = 1'b0;
defparam PLL_inst.CLK5_IN_SEL = 1'b0;
defparam PLL_inst.CLK5_OUT_SEL = 1'b0;
defparam PLL_inst.CLK6_IN_SEL = 1'b0;
defparam PLL_inst.CLK6_OUT_SEL = 1'b0;
defparam PLL_inst.DYN_DPA_EN = "FALSE";
defparam PLL_inst.CLKOUT0_PE_COARSE = 0;
defparam PLL_inst.CLKOUT0_PE_FINE = 0;
defparam PLL_inst.CLKOUT1_PE_COARSE = 0;
defparam PLL_inst.CLKOUT1_PE_FINE = 0;
defparam PLL_inst.CLKOUT2_PE_COARSE = 0;
defparam PLL_inst.CLKOUT2_PE_FINE = 0;
defparam PLL_inst.CLKOUT3_PE_COARSE = 0;
defparam PLL_inst.CLKOUT3_PE_FINE = 0;
defparam PLL_inst.CLKOUT4_PE_COARSE = 0;
defparam PLL_inst.CLKOUT4_PE_FINE = 0;
defparam PLL_inst.CLKOUT5_PE_COARSE = 0;
defparam PLL_inst.CLKOUT5_PE_FINE = 0;
defparam PLL_inst.CLKOUT6_PE_COARSE = 0;
defparam PLL_inst.CLKOUT6_PE_FINE = 0;
defparam PLL_inst.DYN_PE0_SEL = "FALSE";
defparam PLL_inst.DYN_PE1_SEL = "FALSE";
defparam PLL_inst.DYN_PE2_SEL = "FALSE";
defparam PLL_inst.DYN_PE3_SEL = "FALSE";
defparam PLL_inst.DYN_PE4_SEL = "FALSE";
defparam PLL_inst.DYN_PE5_SEL = "FALSE";
defparam PLL_inst.DYN_PE6_SEL = "FALSE";
defparam PLL_inst.DE0_EN = "FALSE";
defparam PLL_inst.DE1_EN = "FALSE";
defparam PLL_inst.DE2_EN = "FALSE";
defparam PLL_inst.DE3_EN = "FALSE";
defparam PLL_inst.DE4_EN = "FALSE";
defparam PLL_inst.DE5_EN = "FALSE";
defparam PLL_inst.DE6_EN = "FALSE";
defparam PLL_inst.RESET_I_EN = "FALSE";
defparam PLL_inst.RESET_O_EN = "FALSE";
defparam PLL_inst.ICP_SEL = 6'bXXXXXX;
defparam PLL_inst.LPF_RES = 3'bXXX;
defparam PLL_inst.LPF_CAP = 2'b00;
defparam PLL_inst.SSC_EN = "FALSE";
defparam PLL_inst.DYN_IDIV_SEL = "FALSE";
defparam PLL_inst.DYN_FBDIV_SEL = "FALSE";
defparam PLL_inst.DYN_MDIV_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV0_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV1_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV2_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV3_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV4_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV5_SEL = "FALSE";
defparam PLL_inst.DYN_ODIV6_SEL = "FALSE";
defparam PLL_inst.DYN_DT0_SEL = "FALSE";
defparam PLL_inst.DYN_DT1_SEL = "FALSE";
defparam PLL_inst.DYN_DT2_SEL = "FALSE";
defparam PLL_inst.DYN_DT3_SEL = "FALSE";
defparam PLL_inst.DYN_ICP_SEL = "TRUE";
defparam PLL_inst.DYN_LPF_SEL = "TRUE";

endmodule //gowin_pixel_pll_MOD
