// pll_usb — 60 MHz USB host clock for GW5AST-138B (Tang Mega 138K SOM).
//
// The GW5AST die has no PLLA resources (PnR RP0008), so this is the
// GW5AST PLL primitive + PLL_INIT calibration companion; pll_usb_mod.v is
// generated via `GowinModgen -do pll_usb.mod`:
//
//   FVCO = FCLKIN * MDIV_SEL / IDIV_SEL = 50 * 18 / 1 = 900 MHz  (650-1300 range)
//   CLKOUT0 = FVCO / ODIV0_SEL = 900 / 15 = 60.000 MHz           (all-integer)
//
// 60 MHz is required by usb_hid_host (FULL_SPEED=1): 1 ms USB frame = 60000
// cycles, 5 clocks per full-speed bit, internal /5 prescaler derives the
// 12 MHz low-speed timing.  Must be crystal-accurate (<=0.25% / 2500 ppm)
// -> derive from the 50 MHz board oscillator, never from a video/logic tap.
//
// Port interface matches the 60B PLLA wrapper exactly (lock, clkout0,
// clkin) so board_plls/top.sv need no variant handling: clkin is the
// 50 MHz crystal, which also serves as the PLL_INIT clock
// (CLK_PERIOD = 20 ns).
module pll_usb (
    input  clkin,       // 50 MHz board crystal
    output clkout0,     // 60 MHz USB host clock
    output lock
);

wire [5:0] icpsel;
wire [2:0] lpfres;
wire pll_lock;
wire pll_rst;

    pll_usb_MOD u_pll (
        .clkout0(clkout0),
        .lock(pll_lock),
        .clkin(clkin),
        .reset(pll_rst),
        .icpsel(icpsel),
        .lpfres(lpfres),
        .lpfcap(2'b00)
    );

    PLL_INIT u_pll_init (
        .CLKIN(clkin),
        .I_RST(1'b0),
        .O_RST(pll_rst),
        .PLLLOCK(pll_lock),
        .O_LOCK(lock),
        .ICPSEL(icpsel),
        .LPFRES(lpfres)
    );
    defparam u_pll_init.CLK_PERIOD = 20;
    defparam u_pll_init.MULTI_FAC = 18;

endmodule
