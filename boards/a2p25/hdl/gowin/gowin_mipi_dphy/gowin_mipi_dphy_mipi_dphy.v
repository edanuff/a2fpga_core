//Copyright (C)2014-2026 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: IP file
//Tool Version: V1.9.12.03
//IP Version: 1.1
//Part Number: GW5A-LV25MG121NC1/I0
//Device: GW5A-25
//Device Version: A
//Created Time: Tue Sep  1 16:51:00 2026

module Gowin_MIPI_DPHY_MIPI_DPHY (tx_clk_o, di_lprx0_n, di_lprx0_p, di_lprx1_n, di_lprx1_p, di_lprx2_n, di_lprx2_p, di_lprx3_n, di_lprx3_p, ck_n, ck_p, d0_n, d0_p, d1_n, d1_p, d2_n, d2_p, d3_n, d3_p, hstxen_ln0, hstxen_ln1, hstxen_ln2, hstxen_ln3, lptxen_ln0, lptxen_ln1, lptxen_ln2, lptxen_ln3, txdpen_ln0, txdpen_ln1, txdpen_ln2, txdpen_ln3, txhclk_en, d0ln_hstxd, d1ln_hstxd, d2ln_hstxd, d3ln_hstxd, hstxd_vld, ck0, ck90, ck180, ck270, do_lptx0_n, do_lptx1_n, do_lptx2_n, do_lptx3_n, do_lptx0_p, do_lptx1_p, do_lptx2_p, do_lptx3_p, lprx_en_d0, lprx_en_d1, lprx_en_d2, lprx_en_d3, tx_drst_n);

output tx_clk_o;
output di_lprx0_n;
output di_lprx0_p;
output di_lprx1_n;
output di_lprx1_p;
output di_lprx2_n;
output di_lprx2_p;
output di_lprx3_n;
output di_lprx3_p;
inout ck_n;
inout ck_p;
inout d0_n;
inout d0_p;
inout d1_n;
inout d1_p;
inout d2_n;
inout d2_p;
inout d3_n;
inout d3_p;
input hstxen_ln0;
input hstxen_ln1;
input hstxen_ln2;
input hstxen_ln3;
input lptxen_ln0;
input lptxen_ln1;
input lptxen_ln2;
input lptxen_ln3;
input txdpen_ln0;
input txdpen_ln1;
input txdpen_ln2;
input txdpen_ln3;
input txhclk_en;
input [15:0] d0ln_hstxd;
input [15:0] d1ln_hstxd;
input [15:0] d2ln_hstxd;
input [15:0] d3ln_hstxd;
input hstxd_vld;
input ck0;
input ck90;
input ck180;
input ck270;
input do_lptx0_n;
input do_lptx1_n;
input do_lptx2_n;
input do_lptx3_n;
input do_lptx0_p;
input do_lptx1_p;
input do_lptx2_p;
input do_lptx3_p;
input lprx_en_d0;
input lprx_en_d1;
input lprx_en_d2;
input lprx_en_d3;
input tx_drst_n;

wire rx_clk_o;
wire tx_clk_o_o;
wire walign_dvld;
wire [15:0] d0ln_hsrxd;
wire [15:0] d1ln_hsrxd;
wire [15:0] d2ln_hsrxd;
wire [15:0] d3ln_hsrxd;
wire d0ln_hsrxd_vld;
wire d1ln_hsrxd_vld;
wire d2ln_hsrxd_vld;
wire d3ln_hsrxd_vld;
wire di_lprxck_n;
wire di_lprxck_p;
wire [7:0] mrdata;
wire alpedo_lane0;
wire alpedo_lane1;
wire alpedo_lane2;
wire alpedo_lane3;
wire alpedo_laneck;
wire d0ln_deskew_done;
wire d1ln_deskew_done;
wire d2ln_deskew_done;
wire d3ln_deskew_done;
wire d0ln_deskew_error;
wire d1ln_deskew_error;
wire d2ln_deskew_error;
wire d3ln_deskew_error;
wire gw_vcc;
wire gw_gnd;

assign walign_dvld = d0ln_deskew_done & d1ln_deskew_done & d2ln_deskew_done & d3ln_deskew_done;
assign tx_clk_o = tx_clk_o_o;
assign gw_vcc = 1'b1;
assign gw_gnd = 1'b0;

MIPI_DPHY mipi_dphy_inst (
    .RX_CLK_O(rx_clk_o),
    .TX_CLK_O(tx_clk_o_o),
    .D0LN_HSRXD(d0ln_hsrxd),
    .D1LN_HSRXD(d1ln_hsrxd),
    .D2LN_HSRXD(d2ln_hsrxd),
    .D3LN_HSRXD(d3ln_hsrxd),
    .D0LN_HSRXD_VLD(d0ln_hsrxd_vld),
    .D1LN_HSRXD_VLD(d1ln_hsrxd_vld),
    .D2LN_HSRXD_VLD(d2ln_hsrxd_vld),
    .D3LN_HSRXD_VLD(d3ln_hsrxd_vld),
    .DI_LPRX0_N(di_lprx0_n),
    .DI_LPRX0_P(di_lprx0_p),
    .DI_LPRX1_N(di_lprx1_n),
    .DI_LPRX1_P(di_lprx1_p),
    .DI_LPRX2_N(di_lprx2_n),
    .DI_LPRX2_P(di_lprx2_p),
    .DI_LPRX3_N(di_lprx3_n),
    .DI_LPRX3_P(di_lprx3_p),
    .DI_LPRXCK_N(di_lprxck_n),
    .DI_LPRXCK_P(di_lprxck_p),
    .MRDATA(mrdata),
    .ALPEDO_LANE0(alpedo_lane0),
    .ALPEDO_LANE1(alpedo_lane1),
    .ALPEDO_LANE2(alpedo_lane2),
    .ALPEDO_LANE3(alpedo_lane3),
    .ALPEDO_LANECK(alpedo_laneck),
    .D0LN_DESKEW_DONE(d0ln_deskew_done),
    .D1LN_DESKEW_DONE(d1ln_deskew_done),
    .D2LN_DESKEW_DONE(d2ln_deskew_done),
    .D3LN_DESKEW_DONE(d3ln_deskew_done),
    .D0LN_DESKEW_ERROR(d0ln_deskew_error),
    .D1LN_DESKEW_ERROR(d1ln_deskew_error),
    .D2LN_DESKEW_ERROR(d2ln_deskew_error),
    .D3LN_DESKEW_ERROR(d3ln_deskew_error),
    .CK_N(ck_n),
    .CK_P(ck_p),
    .D0_N(d0_n),
    .D0_P(d0_p),
    .D1_N(d1_n),
    .D1_P(d1_p),
    .D2_N(d2_n),
    .D2_P(d2_p),
    .D3_N(d3_n),
    .D3_P(d3_p),
    .HSRX_STOP(gw_gnd),
    .HSTXEN_LN0(hstxen_ln0),
    .HSTXEN_LN1(hstxen_ln1),
    .HSTXEN_LN2(hstxen_ln2),
    .HSTXEN_LN3(hstxen_ln3),
    .HSTXEN_LNCK(gw_gnd),
    .LPTXEN_LN0(lptxen_ln0),
    .LPTXEN_LN1(lptxen_ln1),
    .LPTXEN_LN2(lptxen_ln2),
    .LPTXEN_LN3(lptxen_ln3),
    .LPTXEN_LNCK(gw_gnd),
    .PWRON_RX(gw_vcc),
    .PWRON_TX(gw_vcc),
    .RESET(gw_gnd),
    .RX_CLK_1X(rx_clk_o),
    .TX_CLK_1X(tx_clk_o_o),
    .TXDPEN_LN0(txdpen_ln0),
    .TXDPEN_LN1(txdpen_ln1),
    .TXDPEN_LN2(txdpen_ln2),
    .TXDPEN_LN3(txdpen_ln3),
    .TXDPEN_LNCK(gw_gnd),
    .TXHCLK_EN(txhclk_en),
    .CKLN_HSTXD({gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc,gw_vcc}),
    .D0LN_HSTXD(d0ln_hstxd),
    .D1LN_HSTXD(d1ln_hstxd),
    .D2LN_HSTXD(d2ln_hstxd),
    .D3LN_HSTXD(d3ln_hstxd),
    .HSTXD_VLD(hstxd_vld),
    .CK0(ck0),
    .CK90(ck90),
    .CK180(ck180),
    .CK270(ck270),
    .DO_LPTX0_N(do_lptx0_n),
    .DO_LPTX1_N(do_lptx1_n),
    .DO_LPTX2_N(do_lptx2_n),
    .DO_LPTX3_N(do_lptx3_n),
    .DO_LPTXCK_N(gw_gnd),
    .DO_LPTX0_P(do_lptx0_p),
    .DO_LPTX1_P(do_lptx1_p),
    .DO_LPTX2_P(do_lptx2_p),
    .DO_LPTX3_P(do_lptx3_p),
    .DO_LPTXCK_P(gw_gnd),
    .HSRX_EN_CK(gw_gnd),
    .HSRX_EN_D0(gw_gnd),
    .HSRX_EN_D1(gw_gnd),
    .HSRX_EN_D2(gw_gnd),
    .HSRX_EN_D3(gw_gnd),
    .HSRX_ODTEN_CK(gw_gnd),
    .HSRX_ODTEN_D0(gw_gnd),
    .HSRX_ODTEN_D1(gw_gnd),
    .HSRX_ODTEN_D2(gw_gnd),
    .HSRX_ODTEN_D3(gw_gnd),
    .LPRX_EN_CK(gw_gnd),
    .LPRX_EN_D0(lprx_en_d0),
    .LPRX_EN_D1(lprx_en_d1),
    .LPRX_EN_D2(lprx_en_d2),
    .LPRX_EN_D3(lprx_en_d3),
    .RX_DRST_N(gw_gnd),
    .TX_DRST_N(tx_drst_n),
    .WALIGN_DVLD(walign_dvld),
    .MA_INC(gw_gnd),
    .MCLK(gw_gnd),
    .MOPCODE({gw_gnd,gw_gnd}),
    .MWDATA({gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd,gw_gnd}),
    .D0LN_DESKEW_REQ(gw_gnd),
    .D1LN_DESKEW_REQ(gw_gnd),
    .D2LN_DESKEW_REQ(gw_gnd),
    .D3LN_DESKEW_REQ(gw_gnd),
    .HSRX_DLYDIR_LANE0(gw_gnd),
    .HSRX_DLYDIR_LANE1(gw_gnd),
    .HSRX_DLYDIR_LANE2(gw_gnd),
    .HSRX_DLYDIR_LANE3(gw_gnd),
    .HSRX_DLYDIR_LANECK(gw_gnd),
    .HSRX_DLYLDN_LANE0(gw_gnd),
    .HSRX_DLYLDN_LANE1(gw_gnd),
    .HSRX_DLYLDN_LANE2(gw_gnd),
    .HSRX_DLYLDN_LANE3(gw_gnd),
    .HSRX_DLYLDN_LANECK(gw_gnd),
    .HSRX_DLYMV_LANE0(gw_gnd),
    .HSRX_DLYMV_LANE1(gw_gnd),
    .HSRX_DLYMV_LANE2(gw_gnd),
    .HSRX_DLYMV_LANE3(gw_gnd),
    .HSRX_DLYMV_LANECK(gw_gnd),
    .ALP_EDEN_LANE0(gw_gnd),
    .ALP_EDEN_LANE1(gw_gnd),
    .ALP_EDEN_LANE2(gw_gnd),
    .ALP_EDEN_LANE3(gw_gnd),
    .ALP_EDEN_LANECK(gw_gnd),
    .ALPEN_LN0(gw_gnd),
    .ALPEN_LN1(gw_gnd),
    .ALPEN_LN2(gw_gnd),
    .ALPEN_LN3(gw_gnd),
    .ALPEN_LNCK(gw_gnd),
    .D0LN_HSRX_DREN(gw_gnd),
    .D1LN_HSRX_DREN(gw_gnd),
    .D2LN_HSRX_DREN(gw_gnd),
    .D3LN_HSRX_DREN(gw_gnd)
);

defparam mipi_dphy_inst.TX_PLLCLK = "FAST";
defparam mipi_dphy_inst.TX_HS_8BIT_MODE = 1'b0;
defparam mipi_dphy_inst.HSREG_EN_LN0 = 1'b1;
defparam mipi_dphy_inst.HSREG_EN_LN1 = 1'b1;
defparam mipi_dphy_inst.HSREG_EN_LN2 = 1'b1;
defparam mipi_dphy_inst.HSREG_EN_LN3 = 1'b1;
defparam mipi_dphy_inst.LANE_DIV_SEL = 2'b00;
defparam mipi_dphy_inst.HSRX_LANESEL = 4'b1111;
defparam mipi_dphy_inst.HSTX_EN_LN0 = 1'b1;
defparam mipi_dphy_inst.HSTX_EN_LN1 = 1'b1;
defparam mipi_dphy_inst.HSTX_EN_LN2 = 1'b1;
defparam mipi_dphy_inst.HSTX_EN_LN3 = 1'b1;
defparam mipi_dphy_inst.LPTX_EN_LN0 = 1'b1;
defparam mipi_dphy_inst.LPTX_EN_LN1 = 1'b1;
defparam mipi_dphy_inst.LPTX_EN_LN2 = 1'b1;
defparam mipi_dphy_inst.LPTX_EN_LN3 = 1'b1;
defparam mipi_dphy_inst.TXDP_EN_LN0 = 1'b1;
defparam mipi_dphy_inst.TXDP_EN_LN1 = 1'b1;
defparam mipi_dphy_inst.TXDP_EN_LN2 = 1'b1;
defparam mipi_dphy_inst.TXDP_EN_LN3 = 1'b1;
defparam mipi_dphy_inst.HSREG_VREF_CTL = 3'b100;
defparam mipi_dphy_inst.HSREG_VREF_EN = 1'b1;

endmodule //Gowin_MIPI_DPHY_MIPI_DPHY
