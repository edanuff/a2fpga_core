// The accelerator-only build's 105 MHz sequencer clock (see gowin_acc_pll_mod.v):
// gowin_gs_pll's wrapper (hdl/dp_test_138b/gowin_plls_138b.sv) with MDIV 21.
module gowin_acc_pll (output lock, output clkout, input clkin);

    wire [5:0] icpsel;
    wire [2:0] lpfres;
    wire       pll_lock;
    wire       pll_rst;

    gowin_acc_pll_MOD u_pll (
        .clkout0 (clkout),
        .lock    (pll_lock),
        .clkin   (clkin),
        .reset   (pll_rst),
        .icpsel  (icpsel),
        .lpfres  (lpfres),
        .lpfcap  (2'b00)
    );

    PLL_INIT u_pll_init (
        .CLKIN   (clkin),         // 50 MHz crystal - always running
        .I_RST   (1'b0),
        .O_RST   (pll_rst),
        .PLLLOCK (pll_lock),
        .O_LOCK  (lock),
        .ICPSEL  (icpsel),
        .LPFRES  (lpfres)
    );
    defparam u_pll_init.CLK_PERIOD = 20;   // ns, 50 MHz
    defparam u_pll_init.MULTI_FAC  = 21;   // MDIV_SEL

endmodule
