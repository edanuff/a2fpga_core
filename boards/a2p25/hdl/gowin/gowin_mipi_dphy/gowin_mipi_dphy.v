module Gowin_MIPI_DPHY(
    ck_n,
    ck_p,
    d0_n,
    d0_p,
    d1_n,
    d1_p,
    d2_n,
    d2_p,
    d3_n,
    d3_p,
    clkin,
    mdclk,
    clkout0,
    clkout1,
    clkout2,
    clkout3,
    clkout4,
    tx_drst_n,
    tx_clk_o,
    txhclk_en,
    hstxd_vld,
    hstxen_ln0,
    txdpen_ln0,
    d0ln_hstxd,
    lprx_en_d0,
    di_lprx0_n,
    di_lprx0_p,
    lptxen_ln0,
    do_lptx0_n,
    do_lptx0_p,
    hstxen_ln1,
    txdpen_ln1,
    d1ln_hstxd,
    lprx_en_d1,
    di_lprx1_n,
    di_lprx1_p,
    lptxen_ln1,
    do_lptx1_n,
    do_lptx1_p,
    hstxen_ln2,
    txdpen_ln2,
    d2ln_hstxd,
    lprx_en_d2,
    di_lprx2_n,
    di_lprx2_p,
    lptxen_ln2,
    do_lptx2_n,
    do_lptx2_p,
    hstxen_ln3,
    txdpen_ln3,
    d3ln_hstxd,
    lprx_en_d3,
    di_lprx3_n,
    di_lprx3_p,
    lptxen_ln3,
    do_lptx3_n,
    do_lptx3_p
);


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
input clkin;
input mdclk;
output clkout0;
output clkout1;
output clkout2;
output clkout3;
output clkout4;
input tx_drst_n;
output tx_clk_o;
input txhclk_en;
input hstxd_vld;
input hstxen_ln0;
input txdpen_ln0;
input [15:0] d0ln_hstxd;
input lprx_en_d0;
output di_lprx0_n;
output di_lprx0_p;
input lptxen_ln0;
input do_lptx0_n;
input do_lptx0_p;
input hstxen_ln1;
input txdpen_ln1;
input [15:0] d1ln_hstxd;
input lprx_en_d1;
output di_lprx1_n;
output di_lprx1_p;
input lptxen_ln1;
input do_lptx1_n;
input do_lptx1_p;
input hstxen_ln2;
input txdpen_ln2;
input [15:0] d2ln_hstxd;
input lprx_en_d2;
output di_lprx2_n;
output di_lprx2_p;
input lptxen_ln2;
input do_lptx2_n;
input do_lptx2_p;
input hstxen_ln3;
input txdpen_ln3;
input [15:0] d3ln_hstxd;
input lprx_en_d3;
output di_lprx3_n;
output di_lprx3_p;
input lptxen_ln3;
input do_lptx3_n;
input do_lptx3_p;
wire [7:0] mdrdo;
wire lock;
wire [1:0] wMdOpc;
wire wMdAInc;
wire [7:0] wMdDIn;
wire [7:0] wMdQOut;
wire pll_lock;
wire pll_rst;
wire clkout0_o;
wire clkout1_o;
wire clkout2_o;
wire clkout3_o;
assign clkout0 = clkout0_o;
assign clkout1 = clkout1_o;
assign clkout2 = clkout2_o;
assign clkout3 = clkout3_o;


    Gowin_MIPI_DPHY_MIPI_DPHY u_mipi_dphy(
        .ck0(clkout0_o),
        .ck90(clkout1_o),
        .ck180(clkout2_o),
        .ck270(clkout3_o),
        .ck_n(ck_n),
        .ck_p(ck_p),
        .d0_n(d0_n),
        .d0_p(d0_p),
        .d1_n(d1_n),
        .d1_p(d1_p),
        .d2_n(d2_n),
        .d2_p(d2_p),
        .d3_n(d3_n),
        .d3_p(d3_p),
        .tx_drst_n(tx_drst_n),
        .tx_clk_o(tx_clk_o),
        .txhclk_en(txhclk_en),
        .hstxd_vld(hstxd_vld),
        .hstxen_ln0(hstxen_ln0),
        .txdpen_ln0(txdpen_ln0),
        .d0ln_hstxd(d0ln_hstxd),
        .lprx_en_d0(lprx_en_d0),
        .di_lprx0_n(di_lprx0_n),
        .di_lprx0_p(di_lprx0_p),
        .lptxen_ln0(lptxen_ln0),
        .do_lptx0_n(do_lptx0_n),
        .do_lptx0_p(do_lptx0_p),
        .hstxen_ln1(hstxen_ln1),
        .txdpen_ln1(txdpen_ln1),
        .d1ln_hstxd(d1ln_hstxd),
        .lprx_en_d1(lprx_en_d1),
        .di_lprx1_n(di_lprx1_n),
        .di_lprx1_p(di_lprx1_p),
        .lptxen_ln1(lptxen_ln1),
        .do_lptx1_n(do_lptx1_n),
        .do_lptx1_p(do_lptx1_p),
        .hstxen_ln2(hstxen_ln2),
        .txdpen_ln2(txdpen_ln2),
        .d2ln_hstxd(d2ln_hstxd),
        .lprx_en_d2(lprx_en_d2),
        .di_lprx2_n(di_lprx2_n),
        .di_lprx2_p(di_lprx2_p),
        .lptxen_ln2(lptxen_ln2),
        .do_lptx2_n(do_lptx2_n),
        .do_lptx2_p(do_lptx2_p),
        .hstxen_ln3(hstxen_ln3),
        .txdpen_ln3(txdpen_ln3),
        .d3ln_hstxd(d3ln_hstxd),
        .lprx_en_d3(lprx_en_d3),
        .di_lprx3_n(di_lprx3_n),
        .di_lprx3_p(di_lprx3_p),
        .lptxen_ln3(lptxen_ln3),
        .do_lptx3_n(do_lptx3_n),
        .do_lptx3_p(do_lptx3_p)
    );


    Gowin_MIPI_DPHY_PLLA u_pll(
        .clkout0(clkout0_o),
        .clkout1(clkout1_o),
        .clkout2(clkout2_o),
        .clkout3(clkout3_o),
        .clkout4(clkout4),
        .lock(pll_lock),
        .mdrdo(wMdQOut),
        .clkin(clkin),
        .reset(pll_rst),
        .mdclk(mdclk),
        .mdopc(wMdOpc),
        .mdainc(wMdAInc),
        .mdwdi(wMdDIn)
    );


    PLL_INIT u_pll_init(
        .I_RST(1'b0),
        .O_RST(pll_rst),
        .I_LOCK(pll_lock),
        .O_LOCK(lock),
        .I_MD_CLK(mdclk),
        .O_MD_INC(wMdAInc),
        .O_MD_OPC(wMdOpc),
        .O_MD_WR_DATA(wMdDIn),
        .I_MD_RD_DATA(wMdQOut),
        .PLL_INIT_BYPASS(1'b0),
        .MDRDO(mdrdo),
        .MDOPC(2'b00),
        .MDAINC(1'b0),
        .MDWDI(8'h0)
    );
    defparam u_pll_init.CLK_PERIOD = 20;
    defparam u_pll_init.MULTI_FAC = 15;


endmodule
