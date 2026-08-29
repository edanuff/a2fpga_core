///////////////////////////////////////////////////////////////////////////////
// gowin_plls_138b.sv — GW5AST-138B replacements for the DP transmitter PLLs
//
// The shared hdl/displayport/gowin/gowin_plls.v builds `gowin_mgmt_pll` and
// `gowin_pixel_pll` on the **PLLA** primitive. **The GW5AST-138B die has no
// PLLA resource at all** (PnR error RP0008 — established during the 138B
// main-core port, see docs/tang_mega_138k_analysis.md), so a 138B build must
// substitute PLL + PLL_INIT versions of the same two modules.
//
// This file therefore defines the SAME two module names with the SAME port
// (and, for the pixel PLL, parameter) interfaces, and the 138B project's
// .gprj lists it INSTEAD OF hdl/displayport/gowin/gowin_plls.v. No shared RTL
// is modified; a2mega_dp_test_top.sv and dp_transmitter.sv are untouched.
//
// ┌ Ratios ────────────────────────────────────────────────────────────────┐
// │ gowin_mgmt_pll   50 MHz  → 100.000 MHz   IDIV 1 / MDIV 20 / ODIV0 10   │
// │ gowin_pixel_pll 135 MHz  → 148.500 MHz   IDIV 5 / MDIV 44 / ODIV0  8   │
// └────────────────────────────────────────────────────────────────────────┘
// Both are inside the GW5AST envelope (PA2078): PFD = FCLKIN/IDIV ∈ 19..81.25
// MHz (50 and 27), VCO = FCLKIN·MDIV/IDIV ∈ 650..1300 MHz (1000 and 1188).
// The pixel ratios are IDENTICAL to the 60B's; only the mgmt PLL's feedback
// pair changes (the 60B's 24/12 also lands on 100 MHz but needs the PLLA).
//
// ⚠ PLL_INIT CLOCK — REVIEW BEFORE HARDWARE USE.
//   GW5AST PLLs need ICPSEL/LPFRES calibration from a PLL_INIT companion.
//   `gowin_mgmt_pll` clocks PLL_INIT from the 50 MHz crystal — always
//   running, exactly the arrangement used by hdl/clocks_138b.sv.
//   `gowin_pixel_pll` has no such luxury: the shared dp_transmitter
//   instantiates it with (lock, clkout, clkin) only, so its PLL_INIT must run
//   from `clkin` — the 135 MHz SERDES word clock, which the PCS **gates**
//   during SERDES reset/teardown phases (established in WS4_GOWIN_STACK.md
//   §10). Calibration is therefore only valid once the word clock is running,
//   and a teardown that stops the word clock may leave the pixel PLL
//   uncalibrated until the next lock cycle. If that proves to be a problem on
//   hardware, the fix is a shared-RTL change: give `gowin_pixel_pll` an
//   `init_clk` port fed from the 50 MHz oscillator (the same shape
//   hdl/gowin/138B/gowin_pll/clk_pll.v already uses) — parameterised so the
//   60B PLLA version can ignore it.
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

// ---------------------------------------------------------------------------
// 100 MHz management / AUX clock from the 50 MHz SOM oscillator.
// Port interface identical to the 60B PLLA wrapper.
// ---------------------------------------------------------------------------
module gowin_mgmt_pll (output lock, output clkout, input clkin);

    wire [5:0] icpsel;
    wire [2:0] lpfres;
    wire       pll_lock;
    wire       pll_rst;

    gowin_mgmt_pll_MOD u_pll (
        .clkout0 (clkout),
        .lock    (pll_lock),
        .clkin   (clkin),
        .reset   (pll_rst),
        .icpsel  (icpsel),
        .lpfres  (lpfres),
        .lpfcap  (2'b00)
    );

    PLL_INIT u_pll_init (
        .CLKIN   (clkin),         // 50 MHz crystal — always running
        .I_RST   (1'b0),
        .O_RST   (pll_rst),
        .PLLLOCK (pll_lock),
        .O_LOCK  (lock),
        .ICPSEL  (icpsel),
        .LPFRES  (lpfres)
    );
    defparam u_pll_init.CLK_PERIOD = 20;   // ns, 50 MHz
    defparam u_pll_init.MULTI_FAC  = 20;   // MDIV_SEL

endmodule

// ---------------------------------------------------------------------------
// 148.5 MHz pixel clock from the 135 MHz SERDES word clock.
//
// The parameter list mirrors the 60B wrapper so dp_transmitter's
// PIXEL_CLK_MULT/DIV override mechanism still elaborates. NOTE: unlike the
// PLLA version, the ratios here are baked into the generated MOD — the
// parameters are accepted and CHECKED against the MOD, not applied to it.
// A different pixel rate needs a new MOD (see pll/*.mod + README).
// ---------------------------------------------------------------------------
module gowin_pixel_pll #(parameter ODIV0  = 8,
                         parameter IDIV   = 5,
                         parameter MDIV   = 44,
                         parameter FCLKIN = "135")
                        (output lock, output clkout, input clkin);

    // Elaboration-time guard: the generated MOD is fixed at 5/44/8. If a
    // future variant overrides these, it must regenerate the MOD too.
    initial begin
        if (ODIV0 != 8 || IDIV != 5 || MDIV != 44) begin
            $display("gowin_pixel_pll (138B): parameters %0d/%0d/%0d do not match",
                     IDIV, MDIV, ODIV0);
            $display("  the generated gowin_pixel_pll_MOD (5/44/8) — regenerate the MOD.");
        end
    end

    wire [5:0] icpsel;
    wire [2:0] lpfres;
    wire       pll_lock;
    wire       pll_rst;

    gowin_pixel_pll_MOD u_pll (
        .clkout0 (clkout),
        .lock    (pll_lock),
        .clkin   (clkin),
        .reset   (pll_rst),
        .icpsel  (icpsel),
        .lpfres  (lpfres),
        .lpfcap  (2'b00)
    );

    PLL_INIT u_pll_init (
        .CLKIN   (clkin),         // ⚠ 135 MHz word clock — gated by the PCS
        .I_RST   (1'b0),
        .O_RST   (pll_rst),
        .PLLLOCK (pll_lock),
        .O_LOCK  (lock),
        .ICPSEL  (icpsel),
        .LPFRES  (lpfres)
    );
    defparam u_pll_init.CLK_PERIOD = 7;    // ns, 135 MHz (7.407 rounded down)
    defparam u_pll_init.MULTI_FAC  = 44;   // MDIV_SEL

endmodule
