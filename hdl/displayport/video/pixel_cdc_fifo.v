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
    // subtraction must stay at pointer width so the mod-2^(N+1) wrap works
    wire [ADDR_BITS:0] w_used = wptr_bin - rptr_bin_w;
    assign wfull = w_used >= DEPTH;

    always @(posedge wclk) begin
        rptr_gray_w1 <= rptr_gray;
        rptr_gray_w2 <= rptr_gray_w1;
        rptr_bin_w   <= gray2bin(rptr_gray_w2);
        if (wreset) begin
            wptr_bin  <= 0;
            wptr_gray <= 0;
        end else if (wen && !wfull) begin
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
    wire ram_empty = (wptr_gray_r2 == rptr_gray);
    reg [ADDR_BITS:0] wptr_bin_r = 0;

    reg [WIDTH-1:0] rdata_r  = 0;
    reg             rvalid_r = 0;

    // refill the output stage whenever it is empty or being consumed
    wire fetch = !ram_empty && (!rvalid_r || rd_en);

    assign rdata  = rdata_r;
    assign rvalid = rvalid_r;

    always @(posedge rclk) begin
        wptr_gray_r1 <= wptr_gray;
        wptr_gray_r2 <= wptr_gray_r1;
        wptr_bin_r   <= gray2bin(wptr_gray_r2);
        rlevel       <= (wptr_bin_r - rptr_bin) + {{ADDR_BITS{1'b0}}, rvalid_r};
        if (rreset) begin
            rptr_bin  <= 0;
            rptr_gray <= 0;
            rvalid_r  <= 1'b0;
        end else begin
            if (fetch) begin
                rdata_r   <= mem[rptr_bin[ADDR_BITS-1:0]];   // sync read
                rptr_bin  <= rptr_bin + 1'b1;
                rptr_gray <= bin2gray(rptr_bin + 1'b1);
                rvalid_r  <= 1'b1;
            end else if (rd_en && rvalid_r) begin
                rvalid_r  <= 1'b0;
            end
        end
    end

endmodule
