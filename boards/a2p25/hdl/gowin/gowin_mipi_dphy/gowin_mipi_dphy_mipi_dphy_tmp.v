//Copyright (C)2014-2026 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.03
//IP Version: 1.1
//Part Number: GW5A-LV25MG121NC1/I0
//Device: GW5A-25
//Device Version: A
//Created Time: Tue Sep  1 16:51:00 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    Gowin_MIPI_DPHY_MIPI_DPHY your_instance_name(
        .tx_clk_o(tx_clk_o), //output tx_clk_o
        .di_lprx0_n(di_lprx0_n), //output di_lprx0_n
        .di_lprx0_p(di_lprx0_p), //output di_lprx0_p
        .di_lprx1_n(di_lprx1_n), //output di_lprx1_n
        .di_lprx1_p(di_lprx1_p), //output di_lprx1_p
        .di_lprx2_n(di_lprx2_n), //output di_lprx2_n
        .di_lprx2_p(di_lprx2_p), //output di_lprx2_p
        .di_lprx3_n(di_lprx3_n), //output di_lprx3_n
        .di_lprx3_p(di_lprx3_p), //output di_lprx3_p
        .ck_n(ck_n), //inout ck_n
        .ck_p(ck_p), //inout ck_p
        .d0_n(d0_n), //inout d0_n
        .d0_p(d0_p), //inout d0_p
        .d1_n(d1_n), //inout d1_n
        .d1_p(d1_p), //inout d1_p
        .d2_n(d2_n), //inout d2_n
        .d2_p(d2_p), //inout d2_p
        .d3_n(d3_n), //inout d3_n
        .d3_p(d3_p), //inout d3_p
        .hstxen_ln0(hstxen_ln0), //input hstxen_ln0
        .hstxen_ln1(hstxen_ln1), //input hstxen_ln1
        .hstxen_ln2(hstxen_ln2), //input hstxen_ln2
        .hstxen_ln3(hstxen_ln3), //input hstxen_ln3
        .lptxen_ln0(lptxen_ln0), //input lptxen_ln0
        .lptxen_ln1(lptxen_ln1), //input lptxen_ln1
        .lptxen_ln2(lptxen_ln2), //input lptxen_ln2
        .lptxen_ln3(lptxen_ln3), //input lptxen_ln3
        .txdpen_ln0(txdpen_ln0), //input txdpen_ln0
        .txdpen_ln1(txdpen_ln1), //input txdpen_ln1
        .txdpen_ln2(txdpen_ln2), //input txdpen_ln2
        .txdpen_ln3(txdpen_ln3), //input txdpen_ln3
        .txhclk_en(txhclk_en), //input txhclk_en
        .d0ln_hstxd(d0ln_hstxd), //input [15:0] d0ln_hstxd
        .d1ln_hstxd(d1ln_hstxd), //input [15:0] d1ln_hstxd
        .d2ln_hstxd(d2ln_hstxd), //input [15:0] d2ln_hstxd
        .d3ln_hstxd(d3ln_hstxd), //input [15:0] d3ln_hstxd
        .hstxd_vld(hstxd_vld), //input hstxd_vld
        .ck0(ck0), //input ck0
        .ck90(ck90), //input ck90
        .ck180(ck180), //input ck180
        .ck270(ck270), //input ck270
        .do_lptx0_n(do_lptx0_n), //input do_lptx0_n
        .do_lptx1_n(do_lptx1_n), //input do_lptx1_n
        .do_lptx2_n(do_lptx2_n), //input do_lptx2_n
        .do_lptx3_n(do_lptx3_n), //input do_lptx3_n
        .do_lptx0_p(do_lptx0_p), //input do_lptx0_p
        .do_lptx1_p(do_lptx1_p), //input do_lptx1_p
        .do_lptx2_p(do_lptx2_p), //input do_lptx2_p
        .do_lptx3_p(do_lptx3_p), //input do_lptx3_p
        .lprx_en_d0(lprx_en_d0), //input lprx_en_d0
        .lprx_en_d1(lprx_en_d1), //input lprx_en_d1
        .lprx_en_d2(lprx_en_d2), //input lprx_en_d2
        .lprx_en_d3(lprx_en_d3), //input lprx_en_d3
        .tx_drst_n(tx_drst_n) //input tx_drst_n
    );

//--------Copy end-------------------
