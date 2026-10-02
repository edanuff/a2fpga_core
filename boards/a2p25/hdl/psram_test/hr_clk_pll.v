// hr_clk_pll — HyperRAM clock set for the a2p25 PSRAM bring-up bitstream:
// clk (logic), clk_mem (HyperBus CK rate, 2 x clk) and clk_mem_90 from one
// PLLA, so all three are phase-related (hyperram_ctrl needs aligned 2:1
// rising edges and a +90 deg CK clock). Derived from the a2mega-proven
// gowin_pixel_pll template (hdl/displayport/gowin/gowin_plls.v).
//
// Phase: PE_COARSE counts VCO cycles, PE_FINE eighths of a VCO cycle (as in
// the IDE-generated D-PHY PLL: ODIV 1, 90 deg = PE_FINE 2). For ODIV_MEM = N,
// 90 deg = N/4 VCO cycles -> COARSE = N/4, FINE = 2*(N%4).
// Input is 54 MHz from the board PLL (Gowin_PLL), NOT the 50 MHz
// oscillator: GW5A PLLA needs PFD = FCLKIN/IDIV in 19..87.5 MHz, so from
// 50 MHz IDIV is 1 or 2 and no VCO is a multiple of 108 (PA2078). From 54:
//   CK 108: VCO 1080 (1/20), ODIV 20/10, PE 2/4
//   CK  81: VCO  810 (1/15), ODIV 20/10, PE 2/4
//   CK  54: VCO 1080 (1/20), ODIV 40/20, PE 5/0
// Verify the phase on the bench scope (CK vs DQ) — the eye scan also
// tolerates an error here.
module hr_clk_pll #(
    parameter IDIV        = 1,
    parameter MDIV        = 20,      // VCO = FCLKIN / IDIV * MDIV
    parameter ODIV_LOGIC  = 20,      // CLKOUT0: clk (user/arbiter)
    parameter ODIV_MEM    = 10,      // CLKOUT1: clk_mem (HyperBus CK rate)
    parameter PE90_COARSE = 2,       // CLKOUT2 phase = clk_mem + 90 deg:
    parameter PE90_FINE   = 4,       //   ODIV_MEM/4 VCO cycles (fine = 1/8)
    parameter FCLKIN      = "54"
) (
    output lock,
    output clk,
    output clk_mem,
    output clk_mem_90,
    input  clkin
);
wire clkout = clk;



wire clkout3;
wire clkout4;
wire clkout5;
wire clkout6;
wire clkfbout;
wire [7:0] mdrdo;
wire gw_gnd;

assign gw_gnd = 1'b0;

PLLA PLLA_inst (
    .LOCK(lock),
    .CLKOUT0(clk),
    .CLKOUT1(clk_mem),
    .CLKOUT2(clk_mem_90),
    .CLKOUT3(clkout3),
    .CLKOUT4(clkout4),
    .CLKOUT5(clkout5),
    .CLKOUT6(clkout6),
    .CLKFBOUT(clkfbout),
    .MDRDO(mdrdo),
    .CLKIN(clkin),
    .CLKFB(gw_gnd),
    .RESET(gw_gnd),
    .PLLPWD(gw_gnd),
    .RESET_I(gw_gnd),
    .RESET_O(gw_gnd),
    .PSSEL({gw_gnd,gw_gnd,gw_gnd}),
    .PSDIR(gw_gnd),
    .PSPULSE(gw_gnd),
    .SSCPOL(gw_gnd),
    .SSCON(gw_gnd),
    .SSCMDSEL({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .SSCMDSEL_FRAC({gw_gnd,gw_gnd,gw_gnd}),
    .MDCLK(gw_gnd),
    .MDOPC({gw_gnd,gw_gnd}),
    .MDAINC(gw_gnd),
    .MDWDI({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd})
);

defparam PLLA_inst.FCLKIN = FCLKIN;
defparam PLLA_inst.IDIV_SEL = IDIV;
defparam PLLA_inst.FBDIV_SEL = 1;
defparam PLLA_inst.ODIV0_SEL = ODIV_LOGIC;
defparam PLLA_inst.ODIV1_SEL = ODIV_MEM;
defparam PLLA_inst.ODIV2_SEL = ODIV_MEM;
defparam PLLA_inst.ODIV3_SEL = 8;
defparam PLLA_inst.ODIV4_SEL = 8;
defparam PLLA_inst.ODIV5_SEL = 8;
defparam PLLA_inst.ODIV6_SEL = 8;
defparam PLLA_inst.MDIV_SEL = MDIV;
defparam PLLA_inst.MDIV_FRAC_SEL = 0;
defparam PLLA_inst.ODIV0_FRAC_SEL = 0;
defparam PLLA_inst.CLKOUT0_EN = "TRUE";
defparam PLLA_inst.CLKOUT1_EN = "TRUE";
defparam PLLA_inst.CLKOUT2_EN = "TRUE";
defparam PLLA_inst.CLKOUT3_EN = "FALSE";
defparam PLLA_inst.CLKOUT4_EN = "FALSE";
defparam PLLA_inst.CLKOUT5_EN = "FALSE";
defparam PLLA_inst.CLKOUT6_EN = "FALSE";
defparam PLLA_inst.CLKFB_SEL = "INTERNAL";
defparam PLLA_inst.CLKOUT0_DT_DIR = 1'b1;
defparam PLLA_inst.CLKOUT1_DT_DIR = 1'b1;
defparam PLLA_inst.CLKOUT2_DT_DIR = 1'b1;
defparam PLLA_inst.CLKOUT3_DT_DIR = 1'b1;
defparam PLLA_inst.CLKOUT0_DT_STEP = 0;
defparam PLLA_inst.CLKOUT1_DT_STEP = 0;
defparam PLLA_inst.CLKOUT2_DT_STEP = 0;
defparam PLLA_inst.CLKOUT3_DT_STEP = 0;
defparam PLLA_inst.CLK0_IN_SEL = 1'b0;
defparam PLLA_inst.CLK0_OUT_SEL = 1'b0;
defparam PLLA_inst.CLK1_IN_SEL = 1'b0;
defparam PLLA_inst.CLK1_OUT_SEL = 1'b0;
defparam PLLA_inst.CLK2_IN_SEL = 1'b0;
defparam PLLA_inst.CLK2_OUT_SEL = 1'b0;
defparam PLLA_inst.CLK3_IN_SEL = 1'b0;
defparam PLLA_inst.CLK3_OUT_SEL = 1'b0;
defparam PLLA_inst.CLK4_IN_SEL = 2'b00;
defparam PLLA_inst.CLK4_OUT_SEL = 1'b0;
defparam PLLA_inst.CLK5_IN_SEL = 1'b0;
defparam PLLA_inst.CLK5_OUT_SEL = 1'b0;
defparam PLLA_inst.CLK6_IN_SEL = 1'b0;
defparam PLLA_inst.CLK6_OUT_SEL = 1'b0;
defparam PLLA_inst.DYN_DPA_EN = "FALSE";
defparam PLLA_inst.CLKOUT0_PE_COARSE = 0;
defparam PLLA_inst.CLKOUT0_PE_FINE = 0;
defparam PLLA_inst.CLKOUT1_PE_COARSE = 0;
defparam PLLA_inst.CLKOUT1_PE_FINE = 0;
defparam PLLA_inst.CLKOUT2_PE_COARSE = PE90_COARSE;
defparam PLLA_inst.CLKOUT2_PE_FINE = PE90_FINE;
defparam PLLA_inst.CLKOUT3_PE_COARSE = 0;
defparam PLLA_inst.CLKOUT3_PE_FINE = 0;
defparam PLLA_inst.CLKOUT4_PE_COARSE = 0;
defparam PLLA_inst.CLKOUT4_PE_FINE = 0;
defparam PLLA_inst.CLKOUT5_PE_COARSE = 0;
defparam PLLA_inst.CLKOUT5_PE_FINE = 0;
defparam PLLA_inst.CLKOUT6_PE_COARSE = 0;
defparam PLLA_inst.CLKOUT6_PE_FINE = 0;
defparam PLLA_inst.DYN_PE0_SEL = "FALSE";
defparam PLLA_inst.DYN_PE1_SEL = "FALSE";
defparam PLLA_inst.DYN_PE2_SEL = "FALSE";
defparam PLLA_inst.DYN_PE3_SEL = "FALSE";
defparam PLLA_inst.DYN_PE4_SEL = "FALSE";
defparam PLLA_inst.DYN_PE5_SEL = "FALSE";
defparam PLLA_inst.DYN_PE6_SEL = "FALSE";
defparam PLLA_inst.DE0_EN = "FALSE";
defparam PLLA_inst.DE1_EN = "FALSE";
defparam PLLA_inst.DE2_EN = "FALSE";
defparam PLLA_inst.DE3_EN = "FALSE";
defparam PLLA_inst.DE4_EN = "FALSE";
defparam PLLA_inst.DE5_EN = "FALSE";
defparam PLLA_inst.DE6_EN = "FALSE";
defparam PLLA_inst.RESET_I_EN = "FALSE";
defparam PLLA_inst.RESET_O_EN = "FALSE";
defparam PLLA_inst.ICP_SEL = 6'bXXXXXX;
defparam PLLA_inst.LPF_RES = 3'bXXX;
defparam PLLA_inst.LPF_CAP = 2'b00;
defparam PLLA_inst.SSC_EN = "FALSE";

endmodule
