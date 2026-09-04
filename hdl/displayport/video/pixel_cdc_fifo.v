///////////////////////////////////////////////////////////////////////////////
// pixel_cdc_fifo.v : Dual-clock pixel FIFO (gray-coded pointers, show-ahead)
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Crosses packed pixel words from the pixel clock domain into the link
// symbol clock domain. The two clocks are PLL-locked in DP synchronous
// clocking mode, so once the packer has aligned to a start-of-frame word
// the average rates match exactly and the level stays bounded.
//
// Read side is show-ahead (first word fall-through): rdata/rsof are valid
// whenever rvalid is high; rd_en pops the current word.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module pixel_cdc_fifo #(
    parameter WIDTH      = 49,   // payload width (pixels + sof flag)
    parameter ADDR_BITS  = 11    // depth = 2**ADDR_BITS words
)(
    input              wclk,
    input              wreset,
    input  [WIDTH-1:0] wdata,
    input              wen,
    output             wfull,

    input                  rclk,
    input                  rreset,
    output     [WIDTH-1:0] rdata,
    output                 rvalid,
    input                  rd_en,
    output reg [ADDR_BITS:0] rlevel   // approximate fill level, read domain
);

    localparam DEPTH = 1 << ADDR_BITS;

    reg [WIDTH-1:0] mem [0:DEPTH-1];

    // Binary and gray pointers, one extra bit for full/empty discrimination
    reg [ADDR_BITS:0] wptr_bin  = 0, wptr_gray = 0;
    reg [ADDR_BITS:0] rptr_bin  = 0, rptr_gray = 0;

    // Cross-domain synchronisers
    (* ASYNC_REG = "TRUE" *) reg [ADDR_BITS:0] rptr_gray_w1 = 0, rptr_gray_w2 = 0;
    (* ASYNC_REG = "TRUE" *) reg [ADDR_BITS:0] wptr_gray_r1 = 0, wptr_gray_r2 = 0;

    function [ADDR_BITS:0] bin2gray(input [ADDR_BITS:0] b);
        bin2gray = b ^ (b >> 1);
    endfunction

    function [ADDR_BITS:0] gray2bin(input [ADDR_BITS:0] g);
        integer i;
        begin
            gray2bin[ADDR_BITS] = g[ADDR_BITS];
            for (i = ADDR_BITS-1; i >= 0; i = i - 1)
                gray2bin[i] = gray2bin[i+1] ^ g[i];
        end
    endfunction

    // ------------------------------------------------------------------
    // Write domain. The gray->binary conversion is a long XOR chain, so
    // it is registered: the full test then uses a one-cycle-stale read
    // pointer, which only underestimates free space (conservative-safe).
    // ------------------------------------------------------------------
    reg [ADDR_BITS:0] rptr_bin_w = 0;
    // Timing campaign round 2, cone (l): the full test (subtract + compare)
    // sat directly on the BSRAM write enable and the pointer CEs
    // ("wptr_bin -> mem CEA" +0.15 ns at 148.5 MHz in the dp_test gate).
    // wfull is now a register computed from the NEXT write pointer and the
    // current registered read pointer — i.e. one cycle staler on the read
    // side than before, which by the argument above only underestimates
    // free space (conservative-safe); exact under wreset.
    reg wfull_r = 1'b0;
    assign wfull = wfull_r;
    wire wpush = wen && !wfull_r;
    wire [ADDR_BITS:0] wptr_bin_n = wreset ? {(ADDR_BITS+1){1'b0}} :
                                    wpush  ? wptr_bin + 1'b1 : wptr_bin;
    // subtraction must stay at pointer width so the mod-2^(N+1) wrap works
    wire [ADDR_BITS:0] w_used_n = wptr_bin_n - rptr_bin_w;

    always @(posedge wclk) begin
        rptr_gray_w1 <= rptr_gray;
        rptr_gray_w2 <= rptr_gray_w1;
        rptr_bin_w   <= gray2bin(rptr_gray_w2);
        wfull_r      <= (w_used_n >= DEPTH);
        if (wreset) begin
            wptr_bin  <= 0;
            wptr_gray <= 0;
        end else if (wpush) begin
            mem[wptr_bin[ADDR_BITS-1:0]] <= wdata;
            wptr_bin  <= wptr_bin + 1'b1;
            wptr_gray <= bin2gray(wptr_bin + 1'b1);
        end
    end

    // ------------------------------------------------------------------
    // Read domain. The RAM is read synchronously (BRAM-inferable - Gowin
    // BSRAM has no asynchronous read) into a one-word output stage that
    // provides show-ahead behaviour: rdata/rvalid reflect the head word,
    // rd_en consumes it, back-to-back reads sustain one word per cycle.
    // ------------------------------------------------------------------
    // empty test compares gray codes directly (gray is a bijection, so
    // equality holds iff the binary pointers are equal) - no XOR chain
    // in the fetch path. The binary conversion, needed only for the
    // level estimate, is registered.
    // Timing campaign round 2, cone (n): the gray compare sat directly on
    // the fetch term -> BSRAM read enable + pointer CEs ("wptr_gray_r2 ->
    // mem CEB" +0.43 ns at 135 MHz in the dp_test gate). ram_empty is now
    // a register computed from the synchroniser's FIRST stage (= next
    // value of the second) and the read pointer's next value — exactly
    // equal to the old combinational compare on every cycle.
    reg ram_empty = 1'b1;
    reg [ADDR_BITS:0] wptr_bin_r = 0;

    // ------------------------------------------------------------------
    // TWO-STAGE OUTPUT SKID (timing campaign round 2, 2026-09-02).
    // The old single output register was absorbed by synthesis into the
    // BSRAM's own output register, so the packer consumed the block RAM's
    // slow clock-to-out (~2.5-3 ns on GW5A) PLUS its ready/data muxes in
    // one 7.4 ns clk_sym cycle: "pixel_cdc_fifo mem DO -> packer ready_s0"
    // was the #1 knife-edge path in every dirty 60K roll (+0.097 .. -0.447
    // across five rolls of identical source). Stage A below is that BSRAM
    // read register; stage B is a FABRIC flop that presents the head word,
    // so the BSRAM Tco path ends at a flop with no logic behind it and the
    // packer gets a full cycle from a fabric register.
    // External contract unchanged: show-ahead rdata/rvalid, rd_en consumes,
    // back-to-back reads sustain one word per cycle (A refills from the RAM
    // in the same cycle it drains into B). Only visible differences: one
    // extra cycle of initial latency, and when the RAM itself runs dry the
    // last word reaches B one cycle later — both inside the packer's
    // PREFILL margin (full-chain harness: 3.04M cycles, no underrun,
    // pixel-exact, audio clean — see sim/run_full_chain.sh).
    // ------------------------------------------------------------------
    reg [WIDTH-1:0] a_data   = 0;   // stage A: BSRAM read register
    // syn_keep/preserve: the valid flags must stay FABRIC flops — a 60K
    // roll showed the fetch term starting from a BSRAM data output, i.e.
    // synthesis had folded stage-A valid logic behind the block RAM's
    // clock-to-out (cone n).
    (* syn_keep = 1, syn_preserve = 1 *) reg a_valid  = 0;
    reg [WIDTH-1:0] rdata_r  = 0;   // stage B: fabric head register
    (* syn_keep = 1, syn_preserve = 1 *) reg rvalid_r = 0;

    // B accepts a word whenever it is empty or being consumed
    wire b_take = a_valid && (!rvalid_r || rd_en);
    // A fetches from the RAM whenever it is empty or draining into B
    wire fetch  = !ram_empty && (!a_valid || b_take);
    // next read gray pointer (mirrors the sequential block below; cone n).
    // rptr_gray_p1_r always holds bin2gray(rptr_bin + 1) (a registered
    // lookahead, refreshed to +2 on a fetch), so the empty flag's input is
    // a mux and a compare of flops instead of increment + gray + compare
    // ("rptr_bin -> ram_empty/D" +0.36 ns in a 60K roll).
    reg  [ADDR_BITS:0] rptr_gray_p1_r = {{ADDR_BITS{1'b0}}, 1'b1};   // bin2gray(1)
    // The two candidate compares are evaluated in parallel from flops and
    // selected by fetch — the (ADDR_BITS+1)-bit equality maps to a carry
    // chain on GW5A, and in series behind fetch (which itself starts from
    // the BSRAM's clock-to-out via the stage-A valid logic) it reached
    // +0.00 ns in a 60K roll.
    wire eq_cur_w = (wptr_gray_r1 == rptr_gray);
    wire eq_p1_w  = (wptr_gray_r1 == rptr_gray_p1_r);
    wire eq_rst_w = (wptr_gray_r1 == {(ADDR_BITS+1){1'b0}});
    wire ram_empty_n = rreset ? eq_rst_w : (fetch ? eq_p1_w : eq_cur_w);

    assign rdata  = rdata_r;
    assign rvalid = rvalid_r;

    always @(posedge rclk) begin
        wptr_gray_r1 <= wptr_gray;
        wptr_gray_r2 <= wptr_gray_r1;
        wptr_bin_r   <= gray2bin(wptr_gray_r2);
        ram_empty    <= ram_empty_n;                      // cone (n)
        rptr_gray_p1_r <= rreset ? {{ADDR_BITS{1'b0}}, 1'b1} :
                          fetch  ? bin2gray(rptr_bin + 2'd2) : bin2gray(rptr_bin + 1'b1);
        rlevel       <= (wptr_bin_r - rptr_bin)
                        + {{ADDR_BITS{1'b0}}, rvalid_r}
                        + {{ADDR_BITS{1'b0}}, a_valid};
        if (rreset) begin
            rptr_bin  <= 0;
            rptr_gray <= 0;
            a_valid   <= 1'b0;
            rvalid_r  <= 1'b0;
        end else begin
            // stage A <- RAM
            if (fetch) begin
                a_data    <= mem[rptr_bin[ADDR_BITS-1:0]];   // sync read (BSRAM outreg)
                rptr_bin  <= rptr_bin + 1'b1;
                rptr_gray <= rptr_gray_p1_r;                  // == bin2gray(rptr_bin + 1)
                a_valid   <= 1'b1;
            end else if (b_take) begin
                a_valid   <= 1'b0;
            end
            // stage B <- stage A
            if (b_take) begin
                rdata_r   <= a_data;
                rvalid_r  <= 1'b1;
            end else if (rd_en && rvalid_r) begin
                rvalid_r  <= 1'b0;
            end
        end
    end

endmodule
