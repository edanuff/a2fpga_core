//
//Written by GowinSynthesis
//Tool Version "V1.9.12.01"
//Sun Aug 16 08:33:31 2026

//Source file index table:
//file0 "\/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/ipcore/SERDES_IP/IPlib/EDPPHY/data/edp_phy_top.v"
//file1 "\/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/ipcore/SERDES_IP/IPlib/EDPPHY/data/edp_phy_wrapper.v"
`timescale 100 ps/100 ps
module edp_phy (
  serdes_ln2_cpll_ok_i,
  serdes_ln2_fabric_rstn_o,
  serdes_ln2_rx_rst_o,
  serdes_ln2_pma_rx_lock_i,
  serdes_ln2_chbond_start_o,
  serdes_ln2_align_link_i,
  serdes_ln2_k_lock_i,
  serdes_ln2_pcs_rx_clk_i,
  serdes_ln2_fabric_rx_clk_o,
  serdes_ln2_rxdata_i,
  serdes_ln2_rxfifo_rdusewd_i,
  serdes_ln2_rxfifo_aempty_i,
  serdes_ln2_rxfifo_empty_i,
  serdes_ln2_rxfifo_rden_o,
  serdes_ln2_tx_rst_o,
  serdes_ln2_pcs_tx_clk_i,
  serdes_ln2_fabric_tx_clk_o,
  serdes_ln2_txdata_o,
  serdes_ln2_txfifo_wrusewd_i,
  serdes_ln2_txfifo_afull_i,
  serdes_ln2_txfifo_full_i,
  serdes_ln2_tx_vld_o,
  ln2_cpll_ok_o,
  ln2_fabric_rstn_i,
  ln2_rx_rst_i,
  ln2_pma_rx_lock_o,
  ln2_chbond_start_i,
  ln2_align_link_o,
  ln2_k_lock_o,
  ln2_pcs_rx_clk_o,
  ln2_fabric_rx_clk_i,
  ln2_rxfifo_rdusewd_o,
  ln2_rxfifo_aempty_o,
  ln2_rxfifo_empty_o,
  ln2_rxfifo_rden_i,
  ln2_tx_rst_i,
  ln2_pcs_tx_clk_o,
  ln2_fabric_tx_clk_i,
  ln2_txfifo_wrusewd_o,
  ln2_txfifo_afull_o,
  ln2_txfifo_full_o,
  ln2_tx_vld_i,
  ln2_rxdata_o,
  ln2_rxk_o,
  ln2_txdata_i,
  ln2_txk_i,
  serdes_ln3_cpll_ok_i,
  serdes_ln3_fabric_rstn_o,
  serdes_ln3_rx_rst_o,
  serdes_ln3_pma_rx_lock_i,
  serdes_ln3_chbond_start_o,
  serdes_ln3_align_link_i,
  serdes_ln3_k_lock_i,
  serdes_ln3_pcs_rx_clk_i,
  serdes_ln3_fabric_rx_clk_o,
  serdes_ln3_rxdata_i,
  serdes_ln3_rxfifo_rdusewd_i,
  serdes_ln3_rxfifo_aempty_i,
  serdes_ln3_rxfifo_empty_i,
  serdes_ln3_rxfifo_rden_o,
  serdes_ln3_tx_rst_o,
  serdes_ln3_pcs_tx_clk_i,
  serdes_ln3_fabric_tx_clk_o,
  serdes_ln3_txdata_o,
  serdes_ln3_txfifo_wrusewd_i,
  serdes_ln3_txfifo_afull_i,
  serdes_ln3_txfifo_full_i,
  serdes_ln3_tx_vld_o,
  ln3_cpll_ok_o,
  ln3_fabric_rstn_i,
  ln3_rx_rst_i,
  ln3_pma_rx_lock_o,
  ln3_chbond_start_i,
  ln3_align_link_o,
  ln3_k_lock_o,
  ln3_pcs_rx_clk_o,
  ln3_fabric_rx_clk_i,
  ln3_rxfifo_rdusewd_o,
  ln3_rxfifo_aempty_o,
  ln3_rxfifo_empty_o,
  ln3_rxfifo_rden_i,
  ln3_tx_rst_i,
  ln3_pcs_tx_clk_o,
  ln3_fabric_tx_clk_i,
  ln3_txfifo_wrusewd_o,
  ln3_txfifo_afull_o,
  ln3_txfifo_full_o,
  ln3_tx_vld_i,
  ln3_rxdata_o,
  ln3_rxk_o,
  ln3_txdata_i,
  ln3_txk_i,
  drp_clk_o,
  drp_addr_i,
  drp_wren_i,
  drp_wrdata_i,
  drp_strb_i,
  drp_ready_o,
  drp_rden_i,
  drp_rdvld_o,
  drp_rddata_o,
  drp_resp_o,
  drp_clk_i,
  drp_addr_o,
  drp_wren_o,
  drp_wrdata_o,
  drp_strb_o,
  drp_ready_i,
  drp_rden_o,
  drp_rdvld_i,
  drp_rddata_i,
  drp_resp_i,
  serdes_qpll0_ok_i,
  serdes_qpll1_ok_i,
  qpll0_ok_o,
  qpll1_ok_o
)
;
input serdes_ln2_cpll_ok_i;
output serdes_ln2_fabric_rstn_o;
output serdes_ln2_rx_rst_o;
input serdes_ln2_pma_rx_lock_i;
output serdes_ln2_chbond_start_o;
input serdes_ln2_align_link_i;
input serdes_ln2_k_lock_i;
input serdes_ln2_pcs_rx_clk_i;
output serdes_ln2_fabric_rx_clk_o;
input [87:0] serdes_ln2_rxdata_i;
input [4:0] serdes_ln2_rxfifo_rdusewd_i;
input serdes_ln2_rxfifo_aempty_i;
input serdes_ln2_rxfifo_empty_i;
output serdes_ln2_rxfifo_rden_o;
output serdes_ln2_tx_rst_o;
input serdes_ln2_pcs_tx_clk_i;
output serdes_ln2_fabric_tx_clk_o;
output [79:0] serdes_ln2_txdata_o;
input [4:0] serdes_ln2_txfifo_wrusewd_i;
input serdes_ln2_txfifo_afull_i;
input serdes_ln2_txfifo_full_i;
output serdes_ln2_tx_vld_o;
output ln2_cpll_ok_o;
input ln2_fabric_rstn_i;
input ln2_rx_rst_i;
output ln2_pma_rx_lock_o;
input ln2_chbond_start_i;
output ln2_align_link_o;
output ln2_k_lock_o;
output ln2_pcs_rx_clk_o;
input ln2_fabric_rx_clk_i;
output [4:0] ln2_rxfifo_rdusewd_o;
output ln2_rxfifo_aempty_o;
output ln2_rxfifo_empty_o;
input ln2_rxfifo_rden_i;
input ln2_tx_rst_i;
output ln2_pcs_tx_clk_o;
input ln2_fabric_tx_clk_i;
output [4:0] ln2_txfifo_wrusewd_o;
output ln2_txfifo_afull_o;
output ln2_txfifo_full_o;
input ln2_tx_vld_i;
output [15:0] ln2_rxdata_o;
output [1:0] ln2_rxk_o;
input [15:0] ln2_txdata_i;
input [1:0] ln2_txk_i;
input serdes_ln3_cpll_ok_i;
output serdes_ln3_fabric_rstn_o;
output serdes_ln3_rx_rst_o;
input serdes_ln3_pma_rx_lock_i;
output serdes_ln3_chbond_start_o;
input serdes_ln3_align_link_i;
input serdes_ln3_k_lock_i;
input serdes_ln3_pcs_rx_clk_i;
output serdes_ln3_fabric_rx_clk_o;
input [87:0] serdes_ln3_rxdata_i;
input [4:0] serdes_ln3_rxfifo_rdusewd_i;
input serdes_ln3_rxfifo_aempty_i;
input serdes_ln3_rxfifo_empty_i;
output serdes_ln3_rxfifo_rden_o;
output serdes_ln3_tx_rst_o;
input serdes_ln3_pcs_tx_clk_i;
output serdes_ln3_fabric_tx_clk_o;
output [79:0] serdes_ln3_txdata_o;
input [4:0] serdes_ln3_txfifo_wrusewd_i;
input serdes_ln3_txfifo_afull_i;
input serdes_ln3_txfifo_full_i;
output serdes_ln3_tx_vld_o;
output ln3_cpll_ok_o;
input ln3_fabric_rstn_i;
input ln3_rx_rst_i;
output ln3_pma_rx_lock_o;
input ln3_chbond_start_i;
output ln3_align_link_o;
output ln3_k_lock_o;
output ln3_pcs_rx_clk_o;
input ln3_fabric_rx_clk_i;
output [4:0] ln3_rxfifo_rdusewd_o;
output ln3_rxfifo_aempty_o;
output ln3_rxfifo_empty_o;
input ln3_rxfifo_rden_i;
input ln3_tx_rst_i;
output ln3_pcs_tx_clk_o;
input ln3_fabric_tx_clk_i;
output [4:0] ln3_txfifo_wrusewd_o;
output ln3_txfifo_afull_o;
output ln3_txfifo_full_o;
input ln3_tx_vld_i;
output [15:0] ln3_rxdata_o;
output [1:0] ln3_rxk_o;
input [15:0] ln3_txdata_i;
input [1:0] ln3_txk_i;
output drp_clk_o;
input [23:0] drp_addr_i;
input drp_wren_i;
input [31:0] drp_wrdata_i;
input [7:0] drp_strb_i;
output drp_ready_o;
input drp_rden_i;
output drp_rdvld_o;
output [31:0] drp_rddata_o;
output drp_resp_o;
input drp_clk_i;
output [23:0] drp_addr_o;
output drp_wren_o;
output [31:0] drp_wrdata_o;
output [7:0] drp_strb_o;
input drp_ready_i;
output drp_rden_o;
input drp_rdvld_i;
input [31:0] drp_rddata_i;
input drp_resp_i;
input serdes_qpll0_ok_i;
input serdes_qpll1_ok_i;
output qpll0_ok_o;
output qpll1_ok_o;
wire VCC;
wire GND;
  VCC VCC_cZ (
    .V(VCC)
);
  GND GND_cZ (
    .G(GND)
);
  GSR GSR (
    .GSRI(VCC) 
);
assign serdes_ln2_fabric_rstn_o = ln2_fabric_rstn_i;
assign serdes_ln2_rx_rst_o = ln2_rx_rst_i;
assign serdes_ln2_chbond_start_o = ln2_chbond_start_i;
assign serdes_ln2_fabric_rx_clk_o = ln2_fabric_rx_clk_i;
assign serdes_ln2_rxfifo_rden_o = ln2_rxfifo_rden_i;
assign serdes_ln2_tx_rst_o = ln2_tx_rst_i;
assign serdes_ln2_fabric_tx_clk_o = ln2_fabric_tx_clk_i;
assign serdes_ln2_txdata_o[0] = ln2_txdata_i[0];
assign serdes_ln2_txdata_o[1] = ln2_txdata_i[1];
assign serdes_ln2_txdata_o[2] = ln2_txdata_i[2];
assign serdes_ln2_txdata_o[3] = ln2_txdata_i[3];
assign serdes_ln2_txdata_o[4] = ln2_txdata_i[4];
assign serdes_ln2_txdata_o[5] = ln2_txdata_i[5];
assign serdes_ln2_txdata_o[6] = ln2_txdata_i[6];
assign serdes_ln2_txdata_o[7] = ln2_txdata_i[7];
assign serdes_ln2_txdata_o[8] = ln2_txk_i[0];
assign serdes_ln2_txdata_o[9] = GND;
assign serdes_ln2_txdata_o[10] = ln2_txdata_i[8];
assign serdes_ln2_txdata_o[11] = ln2_txdata_i[9];
assign serdes_ln2_txdata_o[12] = ln2_txdata_i[10];
assign serdes_ln2_txdata_o[13] = ln2_txdata_i[11];
assign serdes_ln2_txdata_o[14] = ln2_txdata_i[12];
assign serdes_ln2_txdata_o[15] = ln2_txdata_i[13];
assign serdes_ln2_txdata_o[16] = ln2_txdata_i[14];
assign serdes_ln2_txdata_o[17] = ln2_txdata_i[15];
assign serdes_ln2_txdata_o[18] = ln2_txk_i[1];
assign serdes_ln2_txdata_o[19] = GND;
assign serdes_ln2_txdata_o[20] = GND;
assign serdes_ln2_txdata_o[21] = GND;
assign serdes_ln2_txdata_o[22] = GND;
assign serdes_ln2_txdata_o[23] = GND;
assign serdes_ln2_txdata_o[24] = GND;
assign serdes_ln2_txdata_o[25] = GND;
assign serdes_ln2_txdata_o[26] = GND;
assign serdes_ln2_txdata_o[27] = GND;
assign serdes_ln2_txdata_o[28] = GND;
assign serdes_ln2_txdata_o[29] = GND;
assign serdes_ln2_txdata_o[30] = GND;
assign serdes_ln2_txdata_o[31] = GND;
assign serdes_ln2_txdata_o[32] = GND;
assign serdes_ln2_txdata_o[33] = GND;
assign serdes_ln2_txdata_o[34] = GND;
assign serdes_ln2_txdata_o[35] = GND;
assign serdes_ln2_txdata_o[36] = GND;
assign serdes_ln2_txdata_o[37] = GND;
assign serdes_ln2_txdata_o[38] = GND;
assign serdes_ln2_txdata_o[39] = GND;
assign serdes_ln2_txdata_o[40] = GND;
assign serdes_ln2_txdata_o[41] = GND;
assign serdes_ln2_txdata_o[42] = GND;
assign serdes_ln2_txdata_o[43] = GND;
assign serdes_ln2_txdata_o[44] = GND;
assign serdes_ln2_txdata_o[45] = GND;
assign serdes_ln2_txdata_o[46] = GND;
assign serdes_ln2_txdata_o[47] = GND;
assign serdes_ln2_txdata_o[48] = GND;
assign serdes_ln2_txdata_o[49] = GND;
assign serdes_ln2_txdata_o[50] = GND;
assign serdes_ln2_txdata_o[51] = GND;
assign serdes_ln2_txdata_o[52] = GND;
assign serdes_ln2_txdata_o[53] = GND;
assign serdes_ln2_txdata_o[54] = GND;
assign serdes_ln2_txdata_o[55] = GND;
assign serdes_ln2_txdata_o[56] = GND;
assign serdes_ln2_txdata_o[57] = GND;
assign serdes_ln2_txdata_o[58] = GND;
assign serdes_ln2_txdata_o[59] = GND;
assign serdes_ln2_txdata_o[60] = GND;
assign serdes_ln2_txdata_o[61] = GND;
assign serdes_ln2_txdata_o[62] = GND;
assign serdes_ln2_txdata_o[63] = GND;
assign serdes_ln2_txdata_o[64] = GND;
assign serdes_ln2_txdata_o[65] = GND;
assign serdes_ln2_txdata_o[66] = GND;
assign serdes_ln2_txdata_o[67] = GND;
assign serdes_ln2_txdata_o[68] = GND;
assign serdes_ln2_txdata_o[69] = GND;
assign serdes_ln2_txdata_o[70] = GND;
assign serdes_ln2_txdata_o[71] = GND;
assign serdes_ln2_txdata_o[72] = GND;
assign serdes_ln2_txdata_o[73] = GND;
assign serdes_ln2_txdata_o[74] = GND;
assign serdes_ln2_txdata_o[75] = GND;
assign serdes_ln2_txdata_o[76] = GND;
assign serdes_ln2_txdata_o[77] = GND;
assign serdes_ln2_txdata_o[78] = GND;
assign serdes_ln2_txdata_o[79] = GND;
assign serdes_ln2_tx_vld_o = ln2_tx_vld_i;
assign ln2_cpll_ok_o = serdes_ln2_cpll_ok_i;
assign ln2_pma_rx_lock_o = serdes_ln2_pma_rx_lock_i;
assign ln2_align_link_o = serdes_ln2_align_link_i;
assign ln2_k_lock_o = serdes_ln2_k_lock_i;
assign ln2_pcs_rx_clk_o = serdes_ln2_pcs_rx_clk_i;
assign ln2_rxfifo_rdusewd_o[0] = serdes_ln2_rxfifo_rdusewd_i[0];
assign ln2_rxfifo_rdusewd_o[1] = serdes_ln2_rxfifo_rdusewd_i[1];
assign ln2_rxfifo_rdusewd_o[2] = serdes_ln2_rxfifo_rdusewd_i[2];
assign ln2_rxfifo_rdusewd_o[3] = serdes_ln2_rxfifo_rdusewd_i[3];
assign ln2_rxfifo_rdusewd_o[4] = serdes_ln2_rxfifo_rdusewd_i[4];
assign ln2_rxfifo_aempty_o = serdes_ln2_rxfifo_aempty_i;
assign ln2_rxfifo_empty_o = serdes_ln2_rxfifo_empty_i;
assign ln2_pcs_tx_clk_o = serdes_ln2_pcs_tx_clk_i;
assign ln2_txfifo_wrusewd_o[0] = serdes_ln2_txfifo_wrusewd_i[0];
assign ln2_txfifo_wrusewd_o[1] = serdes_ln2_txfifo_wrusewd_i[1];
assign ln2_txfifo_wrusewd_o[2] = serdes_ln2_txfifo_wrusewd_i[2];
assign ln2_txfifo_wrusewd_o[3] = serdes_ln2_txfifo_wrusewd_i[3];
assign ln2_txfifo_wrusewd_o[4] = serdes_ln2_txfifo_wrusewd_i[4];
assign ln2_txfifo_afull_o = serdes_ln2_txfifo_afull_i;
assign ln2_txfifo_full_o = serdes_ln2_txfifo_full_i;
assign ln2_rxdata_o[0] = serdes_ln2_rxdata_i[0];
assign ln2_rxdata_o[1] = serdes_ln2_rxdata_i[1];
assign ln2_rxdata_o[2] = serdes_ln2_rxdata_i[2];
assign ln2_rxdata_o[3] = serdes_ln2_rxdata_i[3];
assign ln2_rxdata_o[4] = serdes_ln2_rxdata_i[4];
assign ln2_rxdata_o[5] = serdes_ln2_rxdata_i[5];
assign ln2_rxdata_o[6] = serdes_ln2_rxdata_i[6];
assign ln2_rxdata_o[7] = serdes_ln2_rxdata_i[7];
assign ln2_rxdata_o[8] = serdes_ln2_rxdata_i[10];
assign ln2_rxdata_o[9] = serdes_ln2_rxdata_i[11];
assign ln2_rxdata_o[10] = serdes_ln2_rxdata_i[12];
assign ln2_rxdata_o[11] = serdes_ln2_rxdata_i[13];
assign ln2_rxdata_o[12] = serdes_ln2_rxdata_i[14];
assign ln2_rxdata_o[13] = serdes_ln2_rxdata_i[15];
assign ln2_rxdata_o[14] = serdes_ln2_rxdata_i[16];
assign ln2_rxdata_o[15] = serdes_ln2_rxdata_i[17];
assign ln2_rxk_o[0] = serdes_ln2_rxdata_i[8];
assign ln2_rxk_o[1] = serdes_ln2_rxdata_i[18];
assign serdes_ln3_fabric_rstn_o = ln3_fabric_rstn_i;
assign serdes_ln3_rx_rst_o = ln3_rx_rst_i;
assign serdes_ln3_chbond_start_o = ln3_chbond_start_i;
assign serdes_ln3_fabric_rx_clk_o = ln3_fabric_rx_clk_i;
assign serdes_ln3_rxfifo_rden_o = ln3_rxfifo_rden_i;
assign serdes_ln3_tx_rst_o = ln3_tx_rst_i;
assign serdes_ln3_fabric_tx_clk_o = ln3_fabric_tx_clk_i;
assign serdes_ln3_txdata_o[0] = ln3_txdata_i[0];
assign serdes_ln3_txdata_o[1] = ln3_txdata_i[1];
assign serdes_ln3_txdata_o[2] = ln3_txdata_i[2];
assign serdes_ln3_txdata_o[3] = ln3_txdata_i[3];
assign serdes_ln3_txdata_o[4] = ln3_txdata_i[4];
assign serdes_ln3_txdata_o[5] = ln3_txdata_i[5];
assign serdes_ln3_txdata_o[6] = ln3_txdata_i[6];
assign serdes_ln3_txdata_o[7] = ln3_txdata_i[7];
assign serdes_ln3_txdata_o[8] = ln3_txk_i[0];
assign serdes_ln3_txdata_o[9] = GND;
assign serdes_ln3_txdata_o[10] = ln3_txdata_i[8];
assign serdes_ln3_txdata_o[11] = ln3_txdata_i[9];
assign serdes_ln3_txdata_o[12] = ln3_txdata_i[10];
assign serdes_ln3_txdata_o[13] = ln3_txdata_i[11];
assign serdes_ln3_txdata_o[14] = ln3_txdata_i[12];
assign serdes_ln3_txdata_o[15] = ln3_txdata_i[13];
assign serdes_ln3_txdata_o[16] = ln3_txdata_i[14];
assign serdes_ln3_txdata_o[17] = ln3_txdata_i[15];
assign serdes_ln3_txdata_o[18] = ln3_txk_i[1];
assign serdes_ln3_txdata_o[19] = GND;
assign serdes_ln3_txdata_o[20] = GND;
assign serdes_ln3_txdata_o[21] = GND;
assign serdes_ln3_txdata_o[22] = GND;
assign serdes_ln3_txdata_o[23] = GND;
assign serdes_ln3_txdata_o[24] = GND;
assign serdes_ln3_txdata_o[25] = GND;
assign serdes_ln3_txdata_o[26] = GND;
assign serdes_ln3_txdata_o[27] = GND;
assign serdes_ln3_txdata_o[28] = GND;
assign serdes_ln3_txdata_o[29] = GND;
assign serdes_ln3_txdata_o[30] = GND;
assign serdes_ln3_txdata_o[31] = GND;
assign serdes_ln3_txdata_o[32] = GND;
assign serdes_ln3_txdata_o[33] = GND;
assign serdes_ln3_txdata_o[34] = GND;
assign serdes_ln3_txdata_o[35] = GND;
assign serdes_ln3_txdata_o[36] = GND;
assign serdes_ln3_txdata_o[37] = GND;
assign serdes_ln3_txdata_o[38] = GND;
assign serdes_ln3_txdata_o[39] = GND;
assign serdes_ln3_txdata_o[40] = GND;
assign serdes_ln3_txdata_o[41] = GND;
assign serdes_ln3_txdata_o[42] = GND;
assign serdes_ln3_txdata_o[43] = GND;
assign serdes_ln3_txdata_o[44] = GND;
assign serdes_ln3_txdata_o[45] = GND;
assign serdes_ln3_txdata_o[46] = GND;
assign serdes_ln3_txdata_o[47] = GND;
assign serdes_ln3_txdata_o[48] = GND;
assign serdes_ln3_txdata_o[49] = GND;
assign serdes_ln3_txdata_o[50] = GND;
assign serdes_ln3_txdata_o[51] = GND;
assign serdes_ln3_txdata_o[52] = GND;
assign serdes_ln3_txdata_o[53] = GND;
assign serdes_ln3_txdata_o[54] = GND;
assign serdes_ln3_txdata_o[55] = GND;
assign serdes_ln3_txdata_o[56] = GND;
assign serdes_ln3_txdata_o[57] = GND;
assign serdes_ln3_txdata_o[58] = GND;
assign serdes_ln3_txdata_o[59] = GND;
assign serdes_ln3_txdata_o[60] = GND;
assign serdes_ln3_txdata_o[61] = GND;
assign serdes_ln3_txdata_o[62] = GND;
assign serdes_ln3_txdata_o[63] = GND;
assign serdes_ln3_txdata_o[64] = GND;
assign serdes_ln3_txdata_o[65] = GND;
assign serdes_ln3_txdata_o[66] = GND;
assign serdes_ln3_txdata_o[67] = GND;
assign serdes_ln3_txdata_o[68] = GND;
assign serdes_ln3_txdata_o[69] = GND;
assign serdes_ln3_txdata_o[70] = GND;
assign serdes_ln3_txdata_o[71] = GND;
assign serdes_ln3_txdata_o[72] = GND;
assign serdes_ln3_txdata_o[73] = GND;
assign serdes_ln3_txdata_o[74] = GND;
assign serdes_ln3_txdata_o[75] = GND;
assign serdes_ln3_txdata_o[76] = GND;
assign serdes_ln3_txdata_o[77] = GND;
assign serdes_ln3_txdata_o[78] = GND;
assign serdes_ln3_txdata_o[79] = GND;
assign serdes_ln3_tx_vld_o = ln3_tx_vld_i;
assign ln3_cpll_ok_o = serdes_ln3_cpll_ok_i;
assign ln3_pma_rx_lock_o = serdes_ln3_pma_rx_lock_i;
assign ln3_align_link_o = serdes_ln3_align_link_i;
assign ln3_k_lock_o = serdes_ln3_k_lock_i;
assign ln3_pcs_rx_clk_o = serdes_ln3_pcs_rx_clk_i;
assign ln3_rxfifo_rdusewd_o[0] = serdes_ln3_rxfifo_rdusewd_i[0];
assign ln3_rxfifo_rdusewd_o[1] = serdes_ln3_rxfifo_rdusewd_i[1];
assign ln3_rxfifo_rdusewd_o[2] = serdes_ln3_rxfifo_rdusewd_i[2];
assign ln3_rxfifo_rdusewd_o[3] = serdes_ln3_rxfifo_rdusewd_i[3];
assign ln3_rxfifo_rdusewd_o[4] = serdes_ln3_rxfifo_rdusewd_i[4];
assign ln3_rxfifo_aempty_o = serdes_ln3_rxfifo_aempty_i;
assign ln3_rxfifo_empty_o = serdes_ln3_rxfifo_empty_i;
assign ln3_pcs_tx_clk_o = serdes_ln3_pcs_tx_clk_i;
assign ln3_txfifo_wrusewd_o[0] = serdes_ln3_txfifo_wrusewd_i[0];
assign ln3_txfifo_wrusewd_o[1] = serdes_ln3_txfifo_wrusewd_i[1];
assign ln3_txfifo_wrusewd_o[2] = serdes_ln3_txfifo_wrusewd_i[2];
assign ln3_txfifo_wrusewd_o[3] = serdes_ln3_txfifo_wrusewd_i[3];
assign ln3_txfifo_wrusewd_o[4] = serdes_ln3_txfifo_wrusewd_i[4];
assign ln3_txfifo_afull_o = serdes_ln3_txfifo_afull_i;
assign ln3_txfifo_full_o = serdes_ln3_txfifo_full_i;
assign ln3_rxdata_o[0] = serdes_ln3_rxdata_i[0];
assign ln3_rxdata_o[1] = serdes_ln3_rxdata_i[1];
assign ln3_rxdata_o[2] = serdes_ln3_rxdata_i[2];
assign ln3_rxdata_o[3] = serdes_ln3_rxdata_i[3];
assign ln3_rxdata_o[4] = serdes_ln3_rxdata_i[4];
assign ln3_rxdata_o[5] = serdes_ln3_rxdata_i[5];
assign ln3_rxdata_o[6] = serdes_ln3_rxdata_i[6];
assign ln3_rxdata_o[7] = serdes_ln3_rxdata_i[7];
assign ln3_rxdata_o[8] = serdes_ln3_rxdata_i[10];
assign ln3_rxdata_o[9] = serdes_ln3_rxdata_i[11];
assign ln3_rxdata_o[10] = serdes_ln3_rxdata_i[12];
assign ln3_rxdata_o[11] = serdes_ln3_rxdata_i[13];
assign ln3_rxdata_o[12] = serdes_ln3_rxdata_i[14];
assign ln3_rxdata_o[13] = serdes_ln3_rxdata_i[15];
assign ln3_rxdata_o[14] = serdes_ln3_rxdata_i[16];
assign ln3_rxdata_o[15] = serdes_ln3_rxdata_i[17];
assign ln3_rxk_o[0] = serdes_ln3_rxdata_i[8];
assign ln3_rxk_o[1] = serdes_ln3_rxdata_i[18];
assign drp_clk_o = drp_clk_i;
assign drp_ready_o = drp_ready_i;
assign drp_rdvld_o = drp_rdvld_i;
assign drp_rddata_o[0] = drp_rddata_i[0];
assign drp_rddata_o[1] = drp_rddata_i[1];
assign drp_rddata_o[2] = drp_rddata_i[2];
assign drp_rddata_o[3] = drp_rddata_i[3];
assign drp_rddata_o[4] = drp_rddata_i[4];
assign drp_rddata_o[5] = drp_rddata_i[5];
assign drp_rddata_o[6] = drp_rddata_i[6];
assign drp_rddata_o[7] = drp_rddata_i[7];
assign drp_rddata_o[8] = drp_rddata_i[8];
assign drp_rddata_o[9] = drp_rddata_i[9];
assign drp_rddata_o[10] = drp_rddata_i[10];
assign drp_rddata_o[11] = drp_rddata_i[11];
assign drp_rddata_o[12] = drp_rddata_i[12];
assign drp_rddata_o[13] = drp_rddata_i[13];
assign drp_rddata_o[14] = drp_rddata_i[14];
assign drp_rddata_o[15] = drp_rddata_i[15];
assign drp_rddata_o[16] = drp_rddata_i[16];
assign drp_rddata_o[17] = drp_rddata_i[17];
assign drp_rddata_o[18] = drp_rddata_i[18];
assign drp_rddata_o[19] = drp_rddata_i[19];
assign drp_rddata_o[20] = drp_rddata_i[20];
assign drp_rddata_o[21] = drp_rddata_i[21];
assign drp_rddata_o[22] = drp_rddata_i[22];
assign drp_rddata_o[23] = drp_rddata_i[23];
assign drp_rddata_o[24] = drp_rddata_i[24];
assign drp_rddata_o[25] = drp_rddata_i[25];
assign drp_rddata_o[26] = drp_rddata_i[26];
assign drp_rddata_o[27] = drp_rddata_i[27];
assign drp_rddata_o[28] = drp_rddata_i[28];
assign drp_rddata_o[29] = drp_rddata_i[29];
assign drp_rddata_o[30] = drp_rddata_i[30];
assign drp_rddata_o[31] = drp_rddata_i[31];
assign drp_resp_o = drp_resp_i;
assign drp_addr_o[0] = drp_addr_i[0];
assign drp_addr_o[1] = drp_addr_i[1];
assign drp_addr_o[2] = drp_addr_i[2];
assign drp_addr_o[3] = drp_addr_i[3];
assign drp_addr_o[4] = drp_addr_i[4];
assign drp_addr_o[5] = drp_addr_i[5];
assign drp_addr_o[6] = drp_addr_i[6];
assign drp_addr_o[7] = drp_addr_i[7];
assign drp_addr_o[8] = drp_addr_i[8];
assign drp_addr_o[9] = drp_addr_i[9];
assign drp_addr_o[10] = drp_addr_i[10];
assign drp_addr_o[11] = drp_addr_i[11];
assign drp_addr_o[12] = drp_addr_i[12];
assign drp_addr_o[13] = drp_addr_i[13];
assign drp_addr_o[14] = drp_addr_i[14];
assign drp_addr_o[15] = drp_addr_i[15];
assign drp_addr_o[16] = drp_addr_i[16];
assign drp_addr_o[17] = drp_addr_i[17];
assign drp_addr_o[18] = drp_addr_i[18];
assign drp_addr_o[19] = drp_addr_i[19];
assign drp_addr_o[20] = drp_addr_i[20];
assign drp_addr_o[21] = drp_addr_i[21];
assign drp_addr_o[22] = drp_addr_i[22];
assign drp_addr_o[23] = drp_addr_i[23];
assign drp_wren_o = drp_wren_i;
assign drp_wrdata_o[0] = drp_wrdata_i[0];
assign drp_wrdata_o[1] = drp_wrdata_i[1];
assign drp_wrdata_o[2] = drp_wrdata_i[2];
assign drp_wrdata_o[3] = drp_wrdata_i[3];
assign drp_wrdata_o[4] = drp_wrdata_i[4];
assign drp_wrdata_o[5] = drp_wrdata_i[5];
assign drp_wrdata_o[6] = drp_wrdata_i[6];
assign drp_wrdata_o[7] = drp_wrdata_i[7];
assign drp_wrdata_o[8] = drp_wrdata_i[8];
assign drp_wrdata_o[9] = drp_wrdata_i[9];
assign drp_wrdata_o[10] = drp_wrdata_i[10];
assign drp_wrdata_o[11] = drp_wrdata_i[11];
assign drp_wrdata_o[12] = drp_wrdata_i[12];
assign drp_wrdata_o[13] = drp_wrdata_i[13];
assign drp_wrdata_o[14] = drp_wrdata_i[14];
assign drp_wrdata_o[15] = drp_wrdata_i[15];
assign drp_wrdata_o[16] = drp_wrdata_i[16];
assign drp_wrdata_o[17] = drp_wrdata_i[17];
assign drp_wrdata_o[18] = drp_wrdata_i[18];
assign drp_wrdata_o[19] = drp_wrdata_i[19];
assign drp_wrdata_o[20] = drp_wrdata_i[20];
assign drp_wrdata_o[21] = drp_wrdata_i[21];
assign drp_wrdata_o[22] = drp_wrdata_i[22];
assign drp_wrdata_o[23] = drp_wrdata_i[23];
assign drp_wrdata_o[24] = drp_wrdata_i[24];
assign drp_wrdata_o[25] = drp_wrdata_i[25];
assign drp_wrdata_o[26] = drp_wrdata_i[26];
assign drp_wrdata_o[27] = drp_wrdata_i[27];
assign drp_wrdata_o[28] = drp_wrdata_i[28];
assign drp_wrdata_o[29] = drp_wrdata_i[29];
assign drp_wrdata_o[30] = drp_wrdata_i[30];
assign drp_wrdata_o[31] = drp_wrdata_i[31];
assign drp_strb_o[0] = drp_strb_i[0];
assign drp_strb_o[1] = drp_strb_i[1];
assign drp_strb_o[2] = drp_strb_i[2];
assign drp_strb_o[3] = drp_strb_i[3];
assign drp_strb_o[4] = drp_strb_i[4];
assign drp_strb_o[5] = drp_strb_i[5];
assign drp_strb_o[6] = drp_strb_i[6];
assign drp_strb_o[7] = drp_strb_i[7];
assign drp_rden_o = drp_rden_i;
assign qpll0_ok_o = serdes_qpll0_ok_i;
assign qpll1_ok_o = serdes_qpll1_ok_i;
endmodule /* edp_phy */
