//============================================================================
//  65C816 CPU core — fast variant (S7 accelerator exploration)
//
//  ALU with one-hot control (rung 2d).  Same arithmetic as P65C816_ALU; the
//  fstOp/secOp case statements become AND-OR selects driven by the one-hot
//  FST/SEC words and SEL_ADD that the microcode generator pre-decodes
//  (gen_fast_mcode.py), so every select is one LUT level off the register.
//  Bit-exact with the stock ALU (tb_alu_eq.sv: all 256 control words x
//  random operands and flags).
//
//  Copyright (c) srg320 -- original VHDL implementation
//  Copyright (c) 2026 Alan Steremberg -- Verilog conversion and IIgs changes
//  GPL-3.0 (see README.md in hdl/twgs/65C816).
//============================================================================
import F65C816_pkg::*;

module F65C816_ALU
  (
   input [15:0]        L,
   input [15:0]        R,
   input FALUCtrl_r    CTRL,
   input               w16,
   input               BCD,
   input               CI,
   input               VI,
   input               SI,
   output logic        CO,
   output logic        VO,
   output logic        SO,
   output logic        ZO,
   output logic [15:0] RES,
   output logic [15:0] IntR
   );

   wire [7:0] F = CTRL.FST;
   wire [7:0] S = CTRL.SEC;

   // ---- first operation on R (shift/rotate/pass/swap/dec/inc) and its carry
   wire [7:0]  IntR8  = ({8{F[0]}} & {R[6:0], 1'b0}) | ({8{F[1]}} & {R[6:0], CI}) |
                        ({8{F[2]}} & {1'b0, R[7:1]}) | ({8{F[3]}} & {CI, R[7:1]}) |
                        ({8{F[4]}} & R[7:0])          | ({8{F[5]}} & R[15:8]) |
                        ({8{F[6]}} & (R[7:0] - 8'd1)) | ({8{F[7]}} & (R[7:0] + 8'd1));
   wire [15:0] IntR16 = ({16{F[0]}} & {R[14:0], 1'b0}) | ({16{F[1]}} & {R[14:0], CI}) |
                        ({16{F[2]}} & {1'b0, R[15:1]}) | ({16{F[3]}} & {CI, R[15:1]}) |
                        ({16{F[4]}} & R)                | ({16{F[5]}} & {R[7:0], R[15:8]}) |
                        ({16{F[6]}} & (R - 16'd1))      | ({16{F[7]}} & (R + 16'd1));
   wire shl = F[0] | F[1], shr = F[2] | F[3], pass = F[4] | F[5] | F[6] | F[7];
   wire CR8  = (shl & R[7])  | (shr & R[0]) | (pass & CI);
   wire CR16 = (shl & R[15]) | (shr & R[0]) | (pass & CI);
   wire CR   = w16 ? CR16 : CR8;

   // ---- adder
   wire CIIn  = CR | ~CTRL.secOp[0];
   wire ADDIn = ~CTRL.secOp[2];
   wire BCDIn = BCD & CTRL.secOp[0];
   wire [15:0] AddR;
   wire        AddCO, AddVO;
   F65C816_AddSubBCD AddSub (.A(L), .B(R), .CI(CIIn), .ADD(ADDIn), .BCD(BCDIn), .w16(w16),
                             .S(AddR), .CO(AddCO), .VO(AddVO));

   // ---- second operation
   wire [15:0] bit_r16 = CTRL.fc ? (IntR16 | L) : (IntR16 & ~L);   // TSB / TRB
   wire [7:0]  bit_r8  = CTRL.fc ? (IntR8 | L[7:0]) : (IntR8 & ~L[7:0]);
   wire [7:0]  Result8  = ({8{S[0]}} & (L[7:0] | IntR8)) | ({8{S[1]}} & (L[7:0] & IntR8)) |
                          ({8{S[2]}} & (L[7:0] ^ IntR8)) | ({8{CTRL.SEL_ADD}} & AddR[7:0]) |
                          ({8{S[4]}} & IntR8)            | ({8{S[5]}} & bit_r8);
   wire [15:0] Result16 = ({16{S[0]}} & (L | IntR16)) | ({16{S[1]}} & (L & IntR16)) |
                          ({16{S[2]}} & (L ^ IntR16)) | ({16{CTRL.SEL_ADD}} & AddR) |
                          ({16{S[4]}} & IntR16)       | ({16{S[5]}} & bit_r16);
   assign CO = (CTRL.SEL_ADD & AddCO) | (~CTRL.SEL_ADD & CR);

   // bit-test zero (TSB/TRB/BIT): Z from IntR & L, not from the result
   wire ZR = w16 ? ((IntR16 & L) == 16'h0000) : ((IntR8 & L[7:0]) == 8'h00);
   assign ZO = S[5] ? ZR : (w16 ? (Result16 == 16'h0000) : (Result8 == 8'h00));

   // ---- overflow / sign
   wire bit_flags = S[1] & CTRL.fc;                 // BIT: N/V from the operand
   always_comb begin
      VO = VI;
      SO = w16 ? Result16[15] : Result8[7];
      if (bit_flags) begin
         VO = w16 ? IntR16[14] : IntR8[6];
         SO = w16 ? IntR16[15] : IntR8[7];
      end
      if (S[3] | (S[7] & CTRL.fc)) VO = AddVO;
      if (S[5]) SO = SI;
   end

   assign RES  = w16 ? Result16 : {8'h00, Result8};
   assign IntR = w16 ? IntR16   : {8'h00, IntR8};

endmodule
