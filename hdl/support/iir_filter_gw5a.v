//  3-tap IIR filter for 2 channels -- Arora V (GW5A) arithmetic.
//  Copyright (C) 2020 Sorgelig
//
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by the Free
//  Software Foundation; either version 2 of the License, or (at your option)
//  any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  Same filter as hdl/support/iir_filter.v: structure, coefficients, ports and
//  module names are identical, so hdl/sound/audio_out.v instantiates IIR_filter
//  and DC_blocker from whichever of the two files the project lists (the GW2A
//  Tang Nano projects list iir_filter.v, the GW5A projects list this one). What
//  differs is how the two wide products are written.
//
//  The GW2A file writes a 16 x 40 and a 37 x 24 signed multiply and leaves the
//  mapping to synthesis. On the Arora V (GW5A) that does not give a working
//  filter, and neither does writing them as <= 27 x <= 36 pieces: the cells
//  GowinSynthesis 1.9.12.03 infers for those, MULT27X36 and MULTALU27X18,
//  compute wrong results on the GW5A-25 -- the netlist wiring checked out by
//  hand (operands, constants, absorbed AREG with its CE, static ALU mode = A*B)
//  and the RTL is bit-exact with the GW2A file in simulation, yet on the a2p25
//  the output swung full scale every sample (2026-11-18, builds 8b449d4f and
//  00c69b77, audio CDC jump counter saturating at Mockingboard gain << 3). The
//  only DSP cell this repo has hardware proof for is MULT12X12 (the DOC's), so
//  every product here is a signed <= 12 x <= 12 piece: operands are split into
//  a signed top chunk and 11-bit unsigned chunks (a zero sign bit makes each a
//  signed 12-bit operand), one MULT12X12 per pair, shifted and summed in fabric.
//  Bit-exact with the GW2A file (sim/audio_out/run.sh), clean on the card
//  (build fcde464a: jumps 0, max step 0 at << 3).
//
//      inp(16) * cx(40):  inp = s(inp[15:11]) << 11 + u(inp[10:0])
//                         cx  = s(cx[39:33]) << 33 + u(cx[32:22]) << 22
//                             + u(cx[21:11]) << 11 + u(cx[10:0])      -> 8 products
//      y(37) * cy(24):    y   = s(y[36:33]) << 33 + u(y[32:22]) << 22
//                             + u(y[21:11]) << 11 + u(y[10:0])
//                         cy  = s(cy[23:12]) << 12 + u(cy[11:1]) << 1 + cy[0]
//                                                                    -> 8 products + y
//
//  Cost on the a2p25: 32 MULT12X12 = 16 DSP blocks (the cells are instantiated,
//  not inferred, so the four whose cx piece is a constant 0 or 1 are not folded
//  away -- inferred, 2026-11-18, they were: 28 cells / 14 blocks), about 1,350
//  LUT/ALU and 550 FF with audio_out and the DC blockers, vs 11 blocks / 275
//  LUT for the wrong 27x36 version. The instances are named (m_q_*, m_r_*) so
//  boards/a2p25/hdl/a2p25.cst can pin them in any GowinSynthesis version; the
//  inferred cells' names differed between 1.9.11.03 and 1.9.12 (2026-11-21).
//  Every DSP register stage is bypassed, so `inp` stays a fabric register and
//  the multicycle exception in boards/a2p25/hdl/a2p25.sdc covers the cells'
//  DOUT pins as well as the fabric registers.
//
//  Can be converted to 2-tap (coeff_x2 = 0, coeff_y2 = 0) or 1-tap (coeff_x1,2 = 0, coeff_y1,2 = 0)
//
module IIR_filter
#(
	parameter use_params = 1,                     // set to 1 to use following parameters, 0 for input port variables.
	parameter stereo   =  1,                      // 0 for mono (input_l)

	parameter coeff_x  =  0.00000774701983513660, // Base gain value for X. Float. Range: 0.0 ... 0.999(9)
	parameter coeff_x0 =  3,                      // Gain scale factor for X0. Integer. Range -7 ... +7
	parameter coeff_x1 =  3,                      // Gain scale factor for X1. Integer. Range -7 ... +7
	parameter coeff_x2 =  1,                      // Gain scale factor for X2. Integer. Range -7 ... +7
	parameter coeff_y0 = -2.96438150626551080000, // Coefficient for Y0. Float. Range -3.999(9) ... 3.999(9)
	parameter coeff_y1 =  2.92939452735121100000, // Coefficient for Y1. Float. Range -3.999(9) ... 3.999(9)
	parameter coeff_y2 = -0.96500747158831091000  // Coefficient for Y2. Float. Range -3.999(9) ... 3.999(9)
)
(
	input         clk,
	input         reset,

	input         ce,        // must be double of calculated rate for stereo!
	input         sample_ce, // desired output sample rate

	input  [39:0] cx,
	input   [7:0] cx0,
	input   [7:0] cx1,
	input   [7:0] cx2,
	input  [23:0] cy0,
	input  [23:0] cy1,
	input  [23:0] cy2,

	input  [15:0] input_l,  input_r,  // signed samples
	output [15:0] output_l, output_r  // signed samples
);

localparam  [39:0] pcoeff_x  = 40'($realtobits(coeff_x  * 40'h8000000000));
localparam  [31:0] pcoeff_y0 = 32'($realtobits(coeff_y0 * 24'h200000));
localparam  [31:0] pcoeff_y1 = 32'($realtobits(coeff_y1 * 24'h200000));
localparam  [31:0] pcoeff_y2 = 32'($realtobits(coeff_y2 * 24'h200000));

wire [39:0] vcoeff    = use_params ? pcoeff_x        : cx;
wire [23:0] vcoeff_y0 = use_params ? pcoeff_y0[23:0] : cy0;
wire [23:0] vcoeff_y1 = use_params ? pcoeff_y1[23:0] : cy1;
wire [23:0] vcoeff_y2 = use_params ? pcoeff_y2[23:0] : cy2;

reg [15:0] inp, inp_m;

// inp * vcoeff in MULT12X12 pieces: inp = s(inp[15:11]) << 11 + u(inp[10:0]);
// vcoeff = s(v[39:33]) << 33 + u(v[32:22]) << 22 + u(v[21:11]) << 11 + u(v[10:0]).
// Every unsigned piece is 11 bits so it is a signed 12-bit operand with a zero
// sign bit; 2 x 4 = 8 products, each a MULT12X12.
wire signed [11:0] ia_h = {{7{inp[15]}}, inp[15:11]};
wire signed [11:0] ia_l = {1'b0, inp[10:0]};
wire signed [11:0] vb_3 = {{5{vcoeff[39]}}, vcoeff[39:33]};
wire signed [11:0] vb_2 = {1'b0, vcoeff[32:22]};
wire signed [11:0] vb_1 = {1'b0, vcoeff[21:11]};
wire signed [11:0] vb_0 = {1'b0, vcoeff[10:0]};
// Each product is an explicitly instantiated MULT12X12 (combinational: every
// register stage bypassed; the cell is signed 12 x 12 -> 24 on the GW5A) so the
// instance names below are stable across GowinSynthesis versions. a2p25.cst
// pins them by these names; an inferred `*` gets a synthesizer-generated name
// (q_h0_0_s0 in 1.9.12, something else in 1.9.11.03) and the constraint breaks.
wire signed [23:0] q_h3, q_h2, q_h1, q_h0, q_l3, q_l2, q_l1, q_l0;
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_h3 (.DOUT(q_h3), .A(ia_h), .B(vb_3), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_h2 (.DOUT(q_h2), .A(ia_h), .B(vb_2), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_h1 (.DOUT(q_h1), .A(ia_h), .B(vb_1), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_h0 (.DOUT(q_h0), .A(ia_h), .B(vb_0), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_l3 (.DOUT(q_l3), .A(ia_l), .B(vb_3), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_l2 (.DOUT(q_l2), .A(ia_l), .B(vb_2), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_l1 (.DOUT(q_l1), .A(ia_l), .B(vb_1), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_q_l0 (.DOUT(q_l0), .A(ia_l), .B(vb_0), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
// weights: h*b3 << 44, h*b2 << 33, h*b1 << 22, h*b0 << 11, l*b3 << 33, l*b2 << 22, l*b1 << 11, l*b0 << 0
wire signed [59:0] inp_mul_s =
      ({{36{q_h3[23]}}, q_h3} <<< 44) + ({{36{q_h2[23]}}, q_h2} <<< 33) + ({{36{q_h1[23]}}, q_h1} <<< 22) + ({{36{q_h0[23]}}, q_h0} <<< 11)
    + ({{36{q_l3[23]}}, q_l3} <<< 33) + ({{36{q_l2[23]}}, q_l2} <<< 22) + ({{36{q_l1[23]}}, q_l1} <<< 11) + {{36{q_l0[23]}}, q_l0};
wire        [59:0] inp_mul = inp_mul_s;

wire [39:0] x = inp_mul[59:20];
wire [39:0] y = x + tap0;

wire [39:0] tap0;
wire [39:0] tap1;
wire [39:0] tap2;
reg        ch = 0;

iir_filter_tap iir_tap_0
(
	.clk(clk),
	.reset(reset),
	.ce(ce),
	.ch(ch),
	.cx(use_params ? coeff_x0[7:0] : cx0),
	.cy(vcoeff_y0),
	.x(x),
	.y(y),
	.z(tap1),
	.tap(tap0)
);

iir_filter_tap iir_tap_1
(
	.clk(clk),
	.reset(reset),
	.ce(ce),
	.ch(ch),
	.cx(use_params ? coeff_x1[7:0] : cx1),
	.cy(vcoeff_y1),
	.x(x),
	.y(y),
	.z(tap2),
	.tap(tap1)
);

iir_filter_tap iir_tap_2
(
	.clk(clk),
	.reset(reset),
	.ce(ce),
	.ch(ch),
	.cx(use_params ? coeff_x2[7:0] : cx2),
	.cy(vcoeff_y2),
	.x(x),
	.y(y),
	.z(40'd0),
	.tap(tap2)
);

wire [15:0] y_clamp = (~y[39] & |y[38:35]) ? 16'h7FFF : (y[39] & ~&y[38:35]) ? 16'h8000 : y[35:20];

reg [15:0] out_l, out_r, out_m;
always @(posedge clk) if (ce) begin
	if(!stereo) begin
		ch    <= 0;
		inp   <= input_l;
		out_l <= y_clamp;
		out_r <= y_clamp;
	end
	else begin
		ch <= ~ch;
		if(ch) begin
			out_m <= y_clamp;
			inp   <= inp_m;
		end
		else begin
			out_l <= out_m;
			out_r <= y_clamp;
			inp   <= input_l;
			inp_m <= input_r;
		end
	end
end

reg [31:0] out;
always @(posedge clk) if (sample_ce) out <= {out_l, out_r};

assign {output_l, output_r} = out;

endmodule

module iir_filter_tap
(
	input         clk,
	input         reset,

	input         ce,
	input         ch,

	input   [7:0] cx,
	input  [23:0] cy,

	input  [39:0] x,
	input  [39:0] y,
	input  [39:0] z,
	output [39:0] tap
);

// y[36:0] * cy in MULT12X12 pieces: y = s(y[36:33]) << 33 + u(y[32:22]) << 22
// + u(y[21:11]) << 11 + u(y[10:0]); cy = s(cy[23:12]) << 12 + u(cy[11:1]) << 1 + cy[0].
// 4 x 2 = 8 products, each a MULT12X12, plus y shifted in when cy[0] is set.
wire signed [11:0] ya_3 = {{8{y[36]}}, y[36:33]};
wire signed [11:0] ya_2 = {1'b0, y[32:22]};
wire signed [11:0] ya_1 = {1'b0, y[21:11]};
wire signed [11:0] ya_0 = {1'b0, y[10:0]};
wire signed [11:0] cb_h = cy[23:12];
wire signed [11:0] cb_l = {1'b0, cy[11:1]};
// Explicit MULT12X12 instances with stable names (see IIR_filter above).
wire signed [23:0] r_3h, r_2h, r_1h, r_0h, r_3l, r_2l, r_1l, r_0l;
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_3h (.DOUT(r_3h), .A(ya_3), .B(cb_h), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_2h (.DOUT(r_2h), .A(ya_2), .B(cb_h), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_1h (.DOUT(r_1h), .A(ya_1), .B(cb_h), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_0h (.DOUT(r_0h), .A(ya_0), .B(cb_h), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_3l (.DOUT(r_3l), .A(ya_3), .B(cb_l), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_2l (.DOUT(r_2l), .A(ya_2), .B(cb_l), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_1l (.DOUT(r_1l), .A(ya_1), .B(cb_l), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
MULT12X12 #(.AREG_CLK("BYPASS"), .BREG_CLK("BYPASS"), .PREG_CLK("BYPASS"), .OREG_CLK("BYPASS"))
	m_r_0l (.DOUT(r_0l), .A(ya_0), .B(cb_l), .CLK(2'b00), .CE(2'b00), .RESET(2'b00));
wire signed [60:0] y_s37 = {{24{y[36]}}, y[36:0]};
wire signed [60:0] y_mul_s =
      ({{37{r_3h[23]}}, r_3h} <<< 45) + ({{37{r_2h[23]}}, r_2h} <<< 34) + ({{37{r_1h[23]}}, r_1h} <<< 23) + ({{37{r_0h[23]}}, r_0h} <<< 12)
    + ({{37{r_3l[23]}}, r_3l} <<< 34) + ({{37{r_2l[23]}}, r_2l} <<< 23) + ({{37{r_1l[23]}}, r_1l} <<< 12) + ({{37{r_0l[23]}}, r_0l} <<< 1)
    + (cy[0] ? y_s37 : 61'sd0);
wire        [60:0] y_mul = y_mul_s;

function [39:0] x_mul;
	input [39:0] x;
begin
	x_mul = 0;
	if(cx[0]) x_mul =  x_mul + {{4{x[39]}}, x[39:4]};
	if(cx[1]) x_mul =  x_mul + {{3{x[39]}}, x[39:3]};
	if(cx[2]) x_mul =  x_mul + {{2{x[39]}}, x[39:2]};
	if(cx[7]) x_mul = ~x_mul; //cheap NEG
end
endfunction

// Gowin syntax
reg [39:0] intreg[2] /* synthesis syn_ramstyle="registers" */;
always @(posedge clk, posedge reset) begin
	if(reset) {intreg[0],intreg[1]} <= 80'd0;
	else if(ce) intreg[ch] <= x_mul(x) - y_mul[60:21] + z;
end

assign tap = intreg[ch];

endmodule

// simplified IIR 1-tap.
module DC_blocker
(
	input         clk,
	input         ce, // 48/96 KHz
	input         mute,

	input  [15:0] din,
	output [15:0] dout
);

wire [39:0] x  = {din[15], din, 23'd0};
wire [39:0] x0 = x - {{10{x[39]}}, x[39:10]};
wire [39:0] y1 = y - {{09{y[39]}}, y[39:09]};
wire [39:0] y0 = x0 - x1 + y1;

reg  [39:0] x1, y;
always @(posedge clk) if(ce) begin
	x1 <= x0;
	y  <= ^y0[39:38] ? {{2{y0[39]}},{38{y0[38]}}} : y0;
end

assign dout = mute ? 16'd0 : y[38:23];

endmodule
