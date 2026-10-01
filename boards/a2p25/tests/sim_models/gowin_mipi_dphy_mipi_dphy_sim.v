// Behavioural stand-in for Gowin_MIPI_DPHY_MIPI_DPHY (simulation only).
//
// Gowin's MIPI_DPHY simulation model in prim_sim.v is an empty shell
// (ports + parameters, no behaviour), so the generated wrapper cannot be
// simulated as-is. This model replaces the IP's inner module with the TX
// behaviour the a2p25 DP path relies on (UG296E):
//   - TX_CLK_O = CK0 / 8 (16-bit gearbox at 1620 Mbps -> 101.25 MHz),
//     running only while TX_DRST_N is high (the conservative assumption
//     transceiver_bank_dphy is designed around)
//   - each lane serializes its 16-bit word LSB first at 2 bits per CK0
//     period (DDR on the 810 MHz phase clock) while HSTXD_VLD, HSTXEN and
//     TXHCLK_EN are high; otherwise the lane idles low (LP is never used)
// Same port list as the generated gowin_mipi_dphy_mipi_dphy.v.
`timescale 1ps/1ps
module Gowin_MIPI_DPHY_MIPI_DPHY (tx_clk_o, di_lprx0_n, di_lprx0_p, di_lprx1_n, di_lprx1_p, di_lprx2_n, di_lprx2_p, di_lprx3_n, di_lprx3_p, ck_n, ck_p, d0_n, d0_p, d1_n, d1_p, d2_n, d2_p, d3_n, d3_p, hstxen_ln0, hstxen_ln1, hstxen_ln2, hstxen_ln3, lptxen_ln0, lptxen_ln1, lptxen_ln2, lptxen_ln3, txdpen_ln0, txdpen_ln1, txdpen_ln2, txdpen_ln3, txhclk_en, d0ln_hstxd, d1ln_hstxd, d2ln_hstxd, d3ln_hstxd, hstxd_vld, ck0, ck90, ck180, ck270, do_lptx0_n, do_lptx1_n, do_lptx2_n, do_lptx3_n, do_lptx0_p, do_lptx1_p, do_lptx2_p, do_lptx3_p, lprx_en_d0, lprx_en_d1, lprx_en_d2, lprx_en_d3, tx_drst_n);
    output tx_clk_o;
    output di_lprx0_n, di_lprx0_p, di_lprx1_n, di_lprx1_p, di_lprx2_n, di_lprx2_p, di_lprx3_n, di_lprx3_p;
    inout  ck_n, ck_p, d0_n, d0_p, d1_n, d1_p, d2_n, d2_p, d3_n, d3_p;
    input  hstxen_ln0, hstxen_ln1, hstxen_ln2, hstxen_ln3;
    input  lptxen_ln0, lptxen_ln1, lptxen_ln2, lptxen_ln3;
    input  txdpen_ln0, txdpen_ln1, txdpen_ln2, txdpen_ln3;
    input  txhclk_en;
    input  [15:0] d0ln_hstxd, d1ln_hstxd, d2ln_hstxd, d3ln_hstxd;
    input  hstxd_vld;
    input  ck0, ck90, ck180, ck270;
    input  do_lptx0_n, do_lptx1_n, do_lptx2_n, do_lptx3_n;
    input  do_lptx0_p, do_lptx1_p, do_lptx2_p, do_lptx3_p;
    input  lprx_en_d0, lprx_en_d1, lprx_en_d2, lprx_en_d3;
    input  tx_drst_n;

    assign {di_lprx0_n, di_lprx0_p, di_lprx1_n, di_lprx1_p} = 4'b0;
    assign {di_lprx2_n, di_lprx2_p, di_lprx3_n, di_lprx3_p} = 4'b0;
    assign ck_p = 1'b0;
    assign ck_n = 1'b1;

    // Bit clock: both edges of CK0. 16 edges per word = 8 CK0 periods.
    reg [3:0]  edge_cnt = 4'd0;
    reg        txclk = 1'b0;
    reg [15:0] sh [0:3];
    reg [3:0]  on = 4'b0;
    wire [15:0] din [0:3];
    assign din[0] = d0ln_hstxd; assign din[1] = d1ln_hstxd;
    assign din[2] = d2ln_hstxd; assign din[3] = d3ln_hstxd;
    wire [3:0] hsen = {hstxen_ln3, hstxen_ln2, hstxen_ln1, hstxen_ln0};
    integer k;

    always @(ck0) begin
        if (!tx_drst_n) begin
            edge_cnt <= 4'd0;
            txclk    <= 1'b0;
            on       <= 4'b0;
        end else begin
            edge_cnt <= edge_cnt + 4'd1;
            if (edge_cnt == 4'd0) txclk <= 1'b1;
            if (edge_cnt == 4'd8) txclk <= 1'b0;
            for (k = 0; k < 4; k = k + 1) begin
                if (edge_cnt == 4'd0) begin
                    // word boundary: capture the word registered on the
                    // previous tx_clk rising edge
                    on[k]  <= hstxd_vld && hsen[k] && txhclk_en;
                    sh[k]  <= din[k];
                end else begin
                    sh[k]  <= {1'b0, sh[k][15:1]};
                end
            end
        end
    end

    assign tx_clk_o = txclk;
    assign d0_p = on[0] & sh[0][0];  assign d0_n = ~d0_p;
    assign d1_p = on[1] & sh[1][0];  assign d1_n = ~d1_p;
    assign d2_p = on[2] & sh[2][0];  assign d2_n = ~d2_p;
    assign d3_p = on[3] & sh[3][0];  assign d3_n = ~d3_p;
endmodule
