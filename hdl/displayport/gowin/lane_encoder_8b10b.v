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

    enc_8b10b e0 (
        .din(tx_symbol[8:0]),   .disp_in(disp), .force_neg(tx_symbol[9]),
        .dout(c0), .disp_out(d0)
    );
    enc_8b10b e1 (
        .din(tx_symbol[18:10]), .disp_in(d0),   .force_neg(tx_symbol[19]),
        .dout(c1), .disp_out(d1)
    );

    always @(posedge clk) begin
        if (reset) begin
            disp    <= 1'b0;
            tx_code <= 20'b0;
        end else begin
            disp    <= d1;
            tx_code <= {c1, c0};
        end
    end

endmodule
