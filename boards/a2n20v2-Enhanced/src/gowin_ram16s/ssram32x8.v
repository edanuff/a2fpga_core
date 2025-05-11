//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: IP file
//Tool Version: V1.9.11.01 Education
//Part Number: GW2AR-LV18QN88C8/I7
//Device: GW2AR-18
//Device Version: C
//Created Time: Sat May 10 16:50:27 2025

module ssram32x8 (dout, wre, ad, di, clk);

output [7:0] dout;
input wre;
input [4:0] ad;
input [7:0] di;
input clk;

wire ad4_inv;
wire lut_f_0;
wire lut_f_1;
wire [3:0] ram16s_inst_0_dout;
wire [7:4] ram16s_inst_1_dout;
wire [3:0] ram16s_inst_2_dout;
wire [7:4] ram16s_inst_3_dout;
wire gw_vcc;

assign gw_vcc = 1'b1;

INV inv_inst_0 (.I(ad[4]), .O(ad4_inv));

LUT4 lut_inst_0 (
  .F(lut_f_0),
  .I0(wre),
  .I1(ad4_inv),
  .I2(gw_vcc),
  .I3(gw_vcc)
);
defparam lut_inst_0.INIT = 16'h8000;
LUT4 lut_inst_1 (
  .F(lut_f_1),
  .I0(wre),
  .I1(ad[4]),
  .I2(gw_vcc),
  .I3(gw_vcc)
);
defparam lut_inst_1.INIT = 16'h8000;
RAM16S4 ram16s_inst_0 (
    .DO(ram16s_inst_0_dout[3:0]),
    .DI(di[3:0]),
    .AD(ad[3:0]),
    .WRE(lut_f_0),
    .CLK(clk)
);

defparam ram16s_inst_0.INIT_0 = 16'h0000;
defparam ram16s_inst_0.INIT_1 = 16'h0000;
defparam ram16s_inst_0.INIT_2 = 16'h0000;
defparam ram16s_inst_0.INIT_3 = 16'h0000;

RAM16S4 ram16s_inst_1 (
    .DO(ram16s_inst_1_dout[7:4]),
    .DI(di[7:4]),
    .AD(ad[3:0]),
    .WRE(lut_f_0),
    .CLK(clk)
);

defparam ram16s_inst_1.INIT_0 = 16'h0000;
defparam ram16s_inst_1.INIT_1 = 16'h0000;
defparam ram16s_inst_1.INIT_2 = 16'h0000;
defparam ram16s_inst_1.INIT_3 = 16'h0000;

RAM16S4 ram16s_inst_2 (
    .DO(ram16s_inst_2_dout[3:0]),
    .DI(di[3:0]),
    .AD(ad[3:0]),
    .WRE(lut_f_1),
    .CLK(clk)
);

defparam ram16s_inst_2.INIT_0 = 16'h0000;
defparam ram16s_inst_2.INIT_1 = 16'h0000;
defparam ram16s_inst_2.INIT_2 = 16'h0000;
defparam ram16s_inst_2.INIT_3 = 16'h0000;

RAM16S4 ram16s_inst_3 (
    .DO(ram16s_inst_3_dout[7:4]),
    .DI(di[7:4]),
    .AD(ad[3:0]),
    .WRE(lut_f_1),
    .CLK(clk)
);

defparam ram16s_inst_3.INIT_0 = 16'h0000;
defparam ram16s_inst_3.INIT_1 = 16'h0000;
defparam ram16s_inst_3.INIT_2 = 16'h0000;
defparam ram16s_inst_3.INIT_3 = 16'h0000;

MUX2 mux_inst_0 (
  .O(dout[0]),
  .I0(ram16s_inst_0_dout[0]),
  .I1(ram16s_inst_2_dout[0]),
  .S0(ad[4])
);
MUX2 mux_inst_1 (
  .O(dout[1]),
  .I0(ram16s_inst_0_dout[1]),
  .I1(ram16s_inst_2_dout[1]),
  .S0(ad[4])
);
MUX2 mux_inst_2 (
  .O(dout[2]),
  .I0(ram16s_inst_0_dout[2]),
  .I1(ram16s_inst_2_dout[2]),
  .S0(ad[4])
);
MUX2 mux_inst_3 (
  .O(dout[3]),
  .I0(ram16s_inst_0_dout[3]),
  .I1(ram16s_inst_2_dout[3]),
  .S0(ad[4])
);
MUX2 mux_inst_4 (
  .O(dout[4]),
  .I0(ram16s_inst_1_dout[4]),
  .I1(ram16s_inst_3_dout[4]),
  .S0(ad[4])
);
MUX2 mux_inst_5 (
  .O(dout[5]),
  .I0(ram16s_inst_1_dout[5]),
  .I1(ram16s_inst_3_dout[5]),
  .S0(ad[4])
);
MUX2 mux_inst_6 (
  .O(dout[6]),
  .I0(ram16s_inst_1_dout[6]),
  .I1(ram16s_inst_3_dout[6]),
  .S0(ad[4])
);
MUX2 mux_inst_7 (
  .O(dout[7]),
  .I0(ram16s_inst_1_dout[7]),
  .I1(ram16s_inst_3_dout[7]),
  .S0(ad[4])
);
endmodule //ssram32x8
