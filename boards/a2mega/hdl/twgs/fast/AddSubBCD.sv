//============================================================================
//  65C816 CPU core — fast variant (S7 accelerator exploration)
//
//  Binary/BCD add-subtract, rewritten from the gate-level ripple of
//  P65C816_AddSubBCD (4 x BCDAdder = 2 x adder4 = 8 x bit_adder per nibble,
//  a 16-stage carry ripple plus four serial correction adders) into:
//    * one behavioral 17-bit add, which GowinSynthesis maps onto the CLS
//      carry chain;
//    * the decimal corrections computed in parallel per nibble.
//  Bit-exact with the original for every input, including non-BCD digits
//  (checked by the +bcd sweep in hdl/twgs/sim/bench and the ROM-boot lockstep).
//
//  Original per-nibble semantics (T = A + B2 + cin, B2 = ADD ? B : ~B):
//    binary (BCD=0):     S = T mod 16,            cout = T >= 16
//    BCD add (ADD=1):    cout = T >= 10,           S = (T + 6*cout) mod 16
//    BCD sub (ADD=0):    cout = T >= 16 (binary),  S = (T - 6*~cout) mod 16
//  VO is the two's-complement overflow of the UNCORRECTED nibble sum at
//  bit 7 / bit 15, with that nibble's actual carry-in (the decimal carry in
//  BCD add mode).  CO = carry out of nibble 1 (8-bit) or nibble 3 (16-bit).
//
//  Copyright (c) srg320 -- original VHDL implementation
//  Copyright (c) 2026 Alan Steremberg -- Verilog conversion and IIgs changes
//  GPL-3.0 (see README.md in hdl/twgs/65C816).
//============================================================================
module F65C816_AddSubBCD
  (
   input [15:0]  A,
   input [15:0]  B,
   input         CI,
   input         ADD,
   input         BCD,
   input         w16,
   output [15:0] S,
   output        CO,
   output        VO
   );

   // ---- binary path: one carry-chain add
   wire [15:0] B2  = B ^ {16{~ADD}};
   wire [16:0] bin = {1'b0, A} + {1'b0, B2} + {16'd0, CI};
   // binary carries INTO nibbles 1..3 and out of nibble 3, recovered from the sum
   wire bc1 = bin[4]  ^ A[4]  ^ B2[4];
   wire bc2 = bin[8]  ^ A[8]  ^ B2[8];
   wire bc3 = bin[12] ^ A[12] ^ B2[12];
   wire bc4 = bin[16];

   // ---- BCD subtract: binary result, -6 on every nibble whose carry out is 0
   wire [15:0] sub_s = { bin[15:12] + (bc4 ? 4'd0 : 4'd10),
                         bin[11:8]  + (bc3 ? 4'd0 : 4'd10),
                         bin[7:4]   + (bc2 ? 4'd0 : 4'd10),
                         bin[3:0]   + (bc1 ? 4'd0 : 4'd10) };

   // ---- BCD add: decimal carries by generate/propagate, digits in parallel
   wire [4:0] t0 = {1'b0, A[3:0]}   + {1'b0, B[3:0]};
   wire [4:0] t1 = {1'b0, A[7:4]}   + {1'b0, B[7:4]};
   wire [4:0] t2 = {1'b0, A[11:8]}  + {1'b0, B[11:8]};
   wire [4:0] t3 = {1'b0, A[15:12]} + {1'b0, B[15:12]};
   wire g0 = (t0 >= 5'd10), p0 = (t0 == 5'd9);
   wire g1 = (t1 >= 5'd10), p1 = (t1 == 5'd9);
   wire g2 = (t2 >= 5'd10), p2 = (t2 == 5'd9);
   wire g3 = (t3 >= 5'd10), p3 = (t3 == 5'd9);
   wire dc1 = g0 | (p0 & CI);
   wire dc2 = g1 | (p1 & dc1);
   wire dc3 = g2 | (p2 & dc2);
   wire dc4 = g3 | (p3 & dc3);
   wire [3:0] u0 = t0[3:0] + {3'd0, CI};     // uncorrected nibble sums (mod 16)
   wire [3:0] u1 = t1[3:0] + {3'd0, dc1};
   wire [3:0] u2 = t2[3:0] + {3'd0, dc2};
   wire [3:0] u3 = t3[3:0] + {3'd0, dc3};
   wire [15:0] add_s = { u3 + (dc4 ? 4'd6 : 4'd0),
                         u2 + (dc3 ? 4'd6 : 4'd0),
                         u1 + (dc2 ? 4'd6 : 4'd0),
                         u0 + (dc1 ? 4'd6 : 4'd0) };

   // ---- select
   wire bcd_add = BCD & ADD;
   wire bcd_sub = BCD & ~ADD;
   assign S = bcd_add ? add_s : bcd_sub ? sub_s : bin[15:0];

   wire co8  = bcd_add ? dc2 : bc2;
   wire co16 = bcd_add ? dc4 : bc4;
   assign CO = w16 ? co16 : co8;

   // overflow on the uncorrected sum with the mode's carry-in
   wire s7  = bcd_add ? u1[3] : bin[7];
   wire s15 = bcd_add ? u3[3] : bin[15];
   wire vo8  = ~(A[7]  ^ B2[7])  & (A[7]  ^ s7);
   wire vo16 = ~(A[15] ^ B2[15]) & (A[15] ^ s15);
   assign VO = w16 ? vo16 : vo8;

endmodule
