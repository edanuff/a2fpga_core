///////////////////////////////////////////////////////////////////////////////
// lane_encoder_8b10b.v : Fabric 8b/10b encoding for one double-pumped lane
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Encodes the two 10-bit symbols of one lane word ({force_neg, K, data}
// each, symbol 0 = bits [9:0] transmitted first) into two 10-bit line
// codes for a raw-mode SERDES. Running disparity chains through symbol 0
// then symbol 1 within the cycle and across cycles; a set force_neg bit
// restarts the character at RD- (TPS2 requirement, same semantics as the
// Artix GTP TXCHARDISPMODE path this replaces).
//
// tx_code[0] is the first bit on the wire.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module lane_encoder_8b10b (
    input             clk,        // tx_symbol_clk
    input             reset,
    input      [19:0] tx_symbol,  // {force1, sym1[8:0], force0, sym0[8:0]}
    output reg [19:0] tx_code     // {code1[9:0], code0[9:0]}
);

    reg  disp = 1'b0;             // running disparity: 0 = RD-
    wire [9:0] c0, c1;
    wire d0, d1;

    // Input register (timing campaign round 2, 138B follow-up): on the
    // 138B die the DP lanes sit on other die lanes than the 60K's, and the
    // route from the core's skew/training-pattern registers into the two
    // chained 8b/10b lookups landed at +0.02 ns of the 7.407 ns symbol
    // period. Registering the symbol here puts the whole encoder (both
    // lookups and the running-disparity chain) behind a local flop. One
    // symbol clock of extra latency, identical on every lane; the link
    // is a stream, so nothing downstream or upstream observes it.
    reg [19:0] sym_q = 20'b0;

    enc_8b10b e0 (
        .din(sym_q[8:0]),   .disp_in(disp), .force_neg(sym_q[9]),
        .dout(c0), .disp_out(d0)
    );
    // Second symbol in carry-select form (138B durability): both running
    // disparity cases are encoded in parallel and the first symbol's
    // disparity output picks one — the two lookups no longer sit in
    // series (+0.33 ns at 135 MHz on the 138B with them chained). Exact.
    wire [9:0] c1n, c1p;
    wire       d1n, d1p;
    enc_8b10b e1n (
        .din(sym_q[18:10]), .disp_in(1'b0), .force_neg(sym_q[19]),
        .dout(c1n), .disp_out(d1n)
    );
    enc_8b10b e1p (
        .din(sym_q[18:10]), .disp_in(1'b1), .force_neg(sym_q[19]),
        .dout(c1p), .disp_out(d1p)
    );
    assign c1 = d0 ? c1p : c1n;
    assign d1 = d0 ? d1p : d1n;

    always @(posedge clk) begin
        sym_q <= tx_symbol;
        if (reset) begin
            disp    <= 1'b0;
            tx_code <= 20'b0;
        end else begin
            disp    <= d1;
            tx_code <= {c1, c0};
        end
    end

endmodule
