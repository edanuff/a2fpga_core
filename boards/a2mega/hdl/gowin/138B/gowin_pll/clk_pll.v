module clk_pll(
    clkin,
    init_clk,
    clkout0,
    clkout1,
    clkout2,
    lock
);


input clkin;
input init_clk;
output clkout0;
output clkout1;
output clkout2;
output lock;
wire [5:0] icpsel;
wire [2:0] lpfres;
wire pll_lock;
wire pll_rst;


    // Port order cross-wired to match the 60B variant (top.sv expects
    // clkout0=27 MHz pixel, clkout1=135 MHz TMDS, clkout2=54 MHz logic).
    // On GW5AST the assignment is hardware-forced the other way around:
    // exact 54 MHz from a 50 MHz reference needs VCO=1350 MHz for an
    // integer divide, but this PLL's VCO range is 650-1300 MHz (PnR
    // PA2078), so 54 MHz must use the fractional divider, which exists
    // only on ODIV0 (675 MHz VCO / 12.5). Physical outputs are therefore
    // MOD clkout0=54 (fractional /12/13 alternating - see SDC note),
    // clkout1=135, clkout2=27; the swap below restores the shared port
    // semantics. Re-apply this edit if the wrapper is ever regenerated.
    clk_pll_MOD u_pll(
        .clkout1(clkout1),   // 135 MHz (ODIV1 = /5, integer)
        .clkout2(clkout0),   // 27 MHz  (ODIV2 = /25, integer) -> port clkout0
        .clkout0(clkout2),   // 54 MHz  (ODIV0 = /12.5, FRACTIONAL) -> port clkout2
        .lock(pll_lock),
        .clkin(clkin),
        .reset(pll_rst),
        .icpsel(icpsel),
        .lpfres(lpfres),
        .lpfcap(2'b00)
    );


    PLL_INIT u_pll_init(
        .CLKIN(init_clk),
        .I_RST(1'b0),
        .O_RST(pll_rst),
        .PLLLOCK(pll_lock),
        .O_LOCK(lock),
        .ICPSEL(icpsel),
        .LPFRES(lpfres)
    );
    defparam u_pll_init.CLK_PERIOD = 20;
    defparam u_pll_init.MULTI_FAC = 27;


endmodule
