///////////////////////////////////////////////////////////////////////////////
// enc_8b10b.v : 8b/10b encoder wrapper with forced-disparity support
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Wraps the public-domain Chuck Benz 8b/10b encoder (benz_encode.v) and
// adds the per-character force-negative-disparity control that the main
// link datapath carries as the 10th symbol bit (insert_training_pattern.v)
// - the same semantics the Artix GTP implements via TXCHARDISPMODE/VAL.
// TPS2 requires specific K28.5 disparities during link training.
//
// Purely combinational; the caller chains disp_in/disp_out through the
// two symbols of each double-pumped lane word and registers the result.
//
// dout[0] ('a') is transmitted first on the wire.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module enc_8b10b (
    input  [8:0] din,        // {K flag, data[7:0]}
    input        disp_in,    // running disparity: 0 = RD-, 1 = RD+
    input        force_neg,  // encode this character starting from RD-
    output [9:0] dout,       // 10-bit code, bit 0 first on the wire
    output       disp_out
);

    encode i_benz (
        .datain  (din),
        .dispin  (force_neg ? 1'b0 : disp_in),
        .dataout (dout),
        .dispout (disp_out)
    );

endmodule
