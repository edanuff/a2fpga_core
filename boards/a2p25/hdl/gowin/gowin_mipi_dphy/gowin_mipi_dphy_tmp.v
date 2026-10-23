//Copyright (C)2014-2026 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Part Number: GW5A-LV25MG121NC1/I0
//Device: GW5A-25
//Device Version: A


//Change the instance name and port connections to the signal names
//--------Copy here to design--------
    Gowin_MIPI_DPHY your_instance_name(
        .ck_n(ck_n), //inout  ck_n
        .ck_p(ck_p), //inout  ck_p
        .d0_n(d0_n), //inout  d0_n
        .d0_p(d0_p), //inout  d0_p
        .d1_n(d1_n), //inout  d1_n
        .d1_p(d1_p), //inout  d1_p
        .d2_n(d2_n), //inout  d2_n
        .d2_p(d2_p), //inout  d2_p
        .d3_n(d3_n), //inout  d3_n
        .d3_p(d3_p), //inout  d3_p
        .clkin(clkin), //input  clkin
        .mdclk(mdclk), //input  mdclk
        .clkout0(clkout0), //output  clkout0
        .clkout1(clkout1), //output  clkout1
        .clkout2(clkout2), //output  clkout2
        .clkout3(clkout3), //output  clkout3
        .clkout4(clkout4), //output  clkout4
        .tx_drst_n(tx_drst_n), //input  tx_drst_n
        .tx_clk_o(tx_clk_o), //output  tx_clk_o
        .txhclk_en(txhclk_en), //input  txhclk_en
        .hstxd_vld(hstxd_vld), //input  hstxd_vld
        .hstxen_ln0(hstxen_ln0), //input  hstxen_ln0
        .txdpen_ln0(txdpen_ln0), //input  txdpen_ln0
        .d0ln_hstxd(d0ln_hstxd), //input [15:0] d0ln_hstxd
        .lprx_en_d0(lprx_en_d0), //input  lprx_en_d0
        .di_lprx0_n(di_lprx0_n), //output  di_lprx0_n
        .di_lprx0_p(di_lprx0_p), //output  di_lprx0_p
        .lptxen_ln0(lptxen_ln0), //input  lptxen_ln0
        .do_lptx0_n(do_lptx0_n), //input  do_lptx0_n
        .do_lptx0_p(do_lptx0_p), //input  do_lptx0_p
        .hstxen_ln1(hstxen_ln1), //input  hstxen_ln1
        .txdpen_ln1(txdpen_ln1), //input  txdpen_ln1
        .d1ln_hstxd(d1ln_hstxd), //input [15:0] d1ln_hstxd
        .lprx_en_d1(lprx_en_d1), //input  lprx_en_d1
        .di_lprx1_n(di_lprx1_n), //output  di_lprx1_n
        .di_lprx1_p(di_lprx1_p), //output  di_lprx1_p
        .lptxen_ln1(lptxen_ln1), //input  lptxen_ln1
        .do_lptx1_n(do_lptx1_n), //input  do_lptx1_n
        .do_lptx1_p(do_lptx1_p), //input  do_lptx1_p
        .hstxen_ln2(hstxen_ln2), //input  hstxen_ln2
        .txdpen_ln2(txdpen_ln2), //input  txdpen_ln2
        .d2ln_hstxd(d2ln_hstxd), //input [15:0] d2ln_hstxd
        .lprx_en_d2(lprx_en_d2), //input  lprx_en_d2
        .di_lprx2_n(di_lprx2_n), //output  di_lprx2_n
        .di_lprx2_p(di_lprx2_p), //output  di_lprx2_p
        .lptxen_ln2(lptxen_ln2), //input  lptxen_ln2
        .do_lptx2_n(do_lptx2_n), //input  do_lptx2_n
        .do_lptx2_p(do_lptx2_p), //input  do_lptx2_p
        .hstxen_ln3(hstxen_ln3), //input  hstxen_ln3
        .txdpen_ln3(txdpen_ln3), //input  txdpen_ln3
        .d3ln_hstxd(d3ln_hstxd), //input [15:0] d3ln_hstxd
        .lprx_en_d3(lprx_en_d3), //input  lprx_en_d3
        .di_lprx3_n(di_lprx3_n), //output  di_lprx3_n
        .di_lprx3_p(di_lprx3_p), //output  di_lprx3_p
        .lptxen_ln3(lptxen_ln3), //input  lptxen_ln3
        .do_lptx3_n(do_lptx3_n), //input  do_lptx3_n
        .do_lptx3_p(do_lptx3_p) //input  do_lptx3_p
);


//--------Copy end-------------------
