//
// Board clock generation — Tang Mega 138K SOM (GW5AST-138B)
//
// Per-SOM-variant module: a2mega_138B.gprj includes this file; a2mega.gprj
// includes clocks_60b.sv instead. Both define board_plls with an identical
// interface so top.sv is shared unchanged between SOM variants.
//
// GW5AST PLL differences vs the 60B's PLLA (see
// docs/tang_mega_138k_analysis.md):
//  - The PLL primitive needs ICPSEL/LPFRES calibration from a PLL_INIT
//    companion (src/pll_init.v), clocked from the 50 MHz crystal.
//  - Exact 54 MHz is impossible with integer divides (needs VCO 1350 MHz;
//    this PLL's range is 650-1300 MHz), so clk_logic comes from the ODIV0
//    FRACTIONAL divider (675 MHz VCO / 12.5, alternating /12 and /13).
//    Constrain the clk_logic domain at 56.25 MHz (17.77 ns) — see
//    a2mega_138b.sdc.
//  - The PLL has per-output ENCLK gates, so the DDR3 IP's pll_stop drives
//    enclk2 directly (idles HIGH = clock running; dropped LOW during init
//    phase alignment, IPUG281 §4.4.4). No mDRP glue needed — this replaces
//    the 60B's pll_mDRP_intf machinery. Wiring matches nand2mario's
//    shipped 138K design (ddr3_framebuffer_gowin/console138k).
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
//

module board_plls (
    input  wire clk,            // 50 MHz board crystal
    input  wire pll_stop,       // DDR3 IP clock-stop request (IPUG281 §4.4.4)

    output wire clk_pixel,      // 27 MHz pixel clock
    output wire clk_hdmi,       // 135 MHz TMDS
    output wire clk_logic,      // 54 MHz logic (FRACTIONAL — see header)
    output wire clk_lock,       // board PLL locked (via PLL_INIT calibration)

    output wire clk_usb,        // 60 MHz USB host clock (crystal-accurate)
    output wire usb_pll_lock,

    output wire memory_clk,     // 324 MHz to DDR3 controller
    output wire ddr3_pll_lock
);

    // Board PLL: GW5AST PLL + PLL_INIT, VCO = 25 x 27 = 675 MHz.
    // Wrapper presents 60B-compatible port order (clkout0=27, clkout1=135,
    // clkout2=54); the physical ODIV mapping is cross-wired inside
    // hdl/gowin/138B/gowin_pll/clk_pll.v.
    clk_pll clocks_pll (
        .lock(clk_lock),
        .clkout0(clk_pixel),
        .clkout1(clk_hdmi),
        .clkout2(clk_logic),
        .clkin(clk),
        .init_clk(clk)
    );

    // Dedicated USB host clock — usb_hid_host needs crystal-accurate 60 MHz
    pll_usb pll_usb_inst (
        .lock(usb_pll_lock),
        .clkout0(clk_usb),
        .clkin(clk)
    );

    // DDR3 PLL — 324 MHz from the 27 MHz board PLL output (VCO 972 MHz).
    // pll_stop -> enclk2 direct; no mDRP glue on this device.
    pll_ddr3 pll_ddr3_inst (
        .clkin(clk_pixel),             // 27 MHz from board PLL
        .init_clk(clk),                // 50 MHz crystal for PLL_INIT
        .enclk0(1'b0),                 // clkout0 unused — keep gated off
        .enclk2(pll_stop),             // clock-stop request (idles high = run)
        .clkout0(),                    // unused
        .clkout2(memory_clk),          // 324 MHz to DDR3
        .lock(ddr3_pll_lock),
        .reset(~clk_lock)
    );

endmodule
