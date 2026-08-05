//
// Board clock generation — Tang Mega 60K SOM (GW5AT-60B)
//
// Per-SOM-variant module: a2mega.gprj includes this file; a2mega_138B.gprj
// includes clocks_138b.sv instead. Both define board_plls with an identical
// interface so top.sv is shared unchanged between SOM variants.
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
    output wire clk_logic,      // 54 MHz logic (independent of DDR3)
    output wire clk_lock,       // board PLL locked

    output wire clk_usb,        // 60 MHz USB host clock (crystal-accurate)
    output wire usb_pll_lock,

    output wire memory_clk,     // 324 MHz to DDR3 controller
    output wire ddr3_pll_lock
);

    // Board PLL: PLLA, VCO = 50 x 27 = 1350 MHz, all-integer output divides
    // (/50 = 27, /10 = 135, /25 = 54).
    clk_pll clocks_pll (
        .lock(clk_lock),
        .clkout0(clk_pixel),
        .clkout1(clk_hdmi),
        .clkout2(clk_logic),
        .clkin(clk)
    );

    // Dedicated USB host clock — usb_hid_host needs crystal-accurate 60 MHz
    pll_usb pll_usb_inst (
        .lock(usb_pll_lock),
        .clkout0(clk_usb),
        .clkin(clk)
    );

    // -----------------------------------------------------------------
    // DDR3 PLL — 324 MHz memory clock from 27 MHz input
    // -----------------------------------------------------------------
    // Raw PLLA wrapper (no PLL_INIT): on GW5AT-60 the DDR3 IP's pll_stop
    // output MUST actually stop memory_clk during init — the controller's
    // Sync_mod stops the clock, releases its dividers, and restarts it to
    // phase-align the /4 domains (IPUG281 §4.4.4). The 60K PLLA has no
    // enclk, so pll_stop acts through the PLL's mDRP port via
    // pll_mDRP_intf. The previous generated wrapper routed the mDRP bus
    // through PLL_INIT permanently (bypass mux), making pll_stop a silent
    // no-op: cold-boot calibration then relied on luck-of-the-GSR phase
    // alignment (the intermittent dead-DDR3 boots), and any rst_n re-init
    // was guaranteed to fail. Raw PLLA + wired mDRP matches Sipeed's and
    // nand2mario's shipped designs on this exact part.
    wire       mdrp_inc_w;
    wire [1:0] mdrp_op_w;
    wire [7:0] mdrp_wdata_w;
    wire [7:0] mdrp_rdata_w;

    pll_ddr3_MOD pll_ddr3_inst (
        .clkin(clk_pixel),             // 27 MHz from board PLL
        .clkout0(),                    // unused
        .clkout2(memory_clk),          // 324 MHz to DDR3
        .lock(ddr3_pll_lock),
        .mdclk(clk),                   // 50 MHz board crystal = IP clk domain
        .mdopc(mdrp_op_w),
        .mdainc(mdrp_inc_w),
        .mdwdi(mdrp_wdata_w),
        .mdrdo(mdrp_rdata_w),
        .reset(~clk_lock)
    );

    // pll_stop -> mDRP glue (refdesign pattern): pulse wr on either edge
    // of pll_stop, gated by PLL lock; pll_mDRP_intf turns each pulse into
    // the register write that stops/starts CLKOUT2.
    reg pll_lock_mdrp_sync0, pll_lock_mdrp_sync1;
    reg pll_stop_d_r, mdrp_wr_r;
    always @(posedge clk) begin
        pll_lock_mdrp_sync0 <= ddr3_pll_lock;
        pll_lock_mdrp_sync1 <= pll_lock_mdrp_sync0;
        pll_stop_d_r <= pll_stop;
        mdrp_wr_r    <= pll_lock_mdrp_sync1 ? (pll_stop ^ pll_stop_d_r) : 1'b0;
    end

    pll_mDRP_intf u_pll_mDRP_intf (
        .clk(clk),
        .rst_n(clk_lock),
        .pll_lock(pll_lock_mdrp_sync1),
        .wr(mdrp_wr_r),
        .mdrp_inc(mdrp_inc_w),
        .mdrp_op(mdrp_op_w),
        .mdrp_wdata(mdrp_wdata_w),
        .mdrp_rdata(mdrp_rdata_w)
    );

endmodule
