//
//Written by GowinSynthesis
//Tool Version "V1.9.12.03"
//Wed Aug 19 10:42:59 2026

//Source file index table:
//file0 "\/private/tmp/claude-501/-Users-edanuff-GitHub-a2fpga-core--claude-worktrees-epic-lovelace-54ee14/c0e533cf-bc1f-4f7f-8236-ebfb944649c1/scratchpad/edpphy_x40/src/edp_phy_top.v"
//file1 "\/private/tmp/claude-501/-Users-edanuff-GitHub-a2fpga-core--claude-worktrees-epic-lovelace-54ee14/c0e533cf-bc1f-4f7f-8236-ebfb944649c1/scratchpad/edpphy_x40/src/edp_phy_wrapper.v"
//
// === WS4 geometry migration provenance (2026-08-19) ===
// X40 (1:2 gear) regeneration of the EDP PHY protocol-module shim,
// produced HEADLESSLY (no GUI): the IDE's template trio
//   ipcore/SERDES_IP/IPlib/EDPPHY/data/{edp_phy_top.v,
//   edp_phy_wrapper.v,static_macro_define.v}   (IDE V1.9.12.03)
// compiled by GowinSynthesis in a scratch project under hand-written
// defines (LINE2_EN, LINE3_EN, TX_X40_MODE, RX_X40_MODE, DRP;
// module_name edp_phy — matching the reference design's own generation,
// cf. Gowin_EDP_RefDesign temp/edp_phy/edp_phy_defines.v), then the
// chip-top IBUF/OBUF boilerplate mechanically rewritten to assigns
// (696 buffers; the IP generator's own emissions are pure assigns).
// User face: ln2/ln3 txdata[31:0]+txk[3:0] @ 67.5 MHz (2.7G / 40).
// TX packing verified: {0, txk[i], byte[i]} x4 in FABRIC_LN_TXDATA low
// 40 bits — the hardened-8b10b format, extending the audited X20 form.
// Replaces the 1:20/X20 shim from the d535df5f emission (16-bit face),
// whose paired CSR produced a broken 60.75 MHz fabric clock once the
// 8b10b-mode enables were repaired — see WS4_GOWIN_STACK.md section 10.
//
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
output [31:0] ln2_rxdata_o;
output [3:0] ln2_rxk_o;
input [31:0] ln2_txdata_i;
input [3:0] ln2_txk_i;
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
output [31:0] ln3_rxdata_o;
output [3:0] ln3_rxk_o;
input [31:0] ln3_txdata_i;
input [3:0] ln3_txk_i;
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
wire ln2_cpll_ok_o_d;
wire ln2_pma_rx_lock_o_d;
wire ln2_align_link_o_d;
wire ln2_k_lock_o_d;
wire ln2_pcs_rx_clk_o_d;
wire ln2_rxfifo_aempty_o_d;
wire ln2_rxfifo_empty_o_d;
wire ln2_pcs_tx_clk_o_d;
wire ln2_txfifo_afull_o_d;
wire ln2_txfifo_full_o_d;
wire serdes_ln2_fabric_rstn_o_d;
wire serdes_ln2_rx_rst_o_d;
wire serdes_ln2_chbond_start_o_d;
wire serdes_ln2_fabric_rx_clk_o_d;
wire serdes_ln2_rxfifo_rden_o_d;
wire serdes_ln2_tx_rst_o_d;
wire serdes_ln2_fabric_tx_clk_o_d;
wire serdes_ln2_tx_vld_o_d;
wire ln3_cpll_ok_o_d;
wire ln3_pma_rx_lock_o_d;
wire ln3_align_link_o_d;
wire ln3_k_lock_o_d;
wire ln3_pcs_rx_clk_o_d;
wire ln3_rxfifo_aempty_o_d;
wire ln3_rxfifo_empty_o_d;
wire ln3_pcs_tx_clk_o_d;
wire ln3_txfifo_afull_o_d;
wire ln3_txfifo_full_o_d;
wire serdes_ln3_fabric_rstn_o_d;
wire serdes_ln3_rx_rst_o_d;
wire serdes_ln3_chbond_start_o_d;
wire serdes_ln3_fabric_rx_clk_o_d;
wire serdes_ln3_rxfifo_rden_o_d;
wire serdes_ln3_tx_rst_o_d;
wire serdes_ln3_fabric_tx_clk_o_d;
wire serdes_ln3_tx_vld_o_d;
wire drp_wren_o_d;
wire drp_rden_o_d;
wire drp_clk_o_d;
wire drp_ready_o_d;
wire drp_rdvld_o_d;
wire drp_resp_o_d;
wire qpll0_ok_o_d;
wire qpll1_ok_o_d;
wire [31:0] ln2_rxdata_o_d;
wire [3:0] ln2_rxk_o_d;
wire [4:0] ln2_rxfifo_rdusewd_o_d;
wire [4:0] ln2_txfifo_wrusewd_o_d;
wire [38:0] serdes_ln2_txdata_o_d;
wire [31:0] ln3_rxdata_o_d;
wire [3:0] ln3_rxk_o_d;
wire [4:0] ln3_rxfifo_rdusewd_o_d;
wire [4:0] ln3_txfifo_wrusewd_o_d;
wire [38:0] serdes_ln3_txdata_o_d;
wire [23:0] drp_addr_o_d;
wire [31:0] drp_wrdata_o_d;
wire [7:0] drp_strb_o_d;
wire [31:0] drp_rddata_o_d;
wire VCC;
wire GND;
  assign ln2_cpll_ok_o_d = serdes_ln2_cpll_ok_i;
  assign ln2_pma_rx_lock_o_d = serdes_ln2_pma_rx_lock_i;
  assign ln2_align_link_o_d = serdes_ln2_align_link_i;
  assign ln2_k_lock_o_d = serdes_ln2_k_lock_i;
  assign ln2_pcs_rx_clk_o_d = serdes_ln2_pcs_rx_clk_i;
  assign ln2_rxdata_o_d[0] = serdes_ln2_rxdata_i[0];
  assign ln2_rxdata_o_d[1] = serdes_ln2_rxdata_i[1];
  assign ln2_rxdata_o_d[2] = serdes_ln2_rxdata_i[2];
  assign ln2_rxdata_o_d[3] = serdes_ln2_rxdata_i[3];
  assign ln2_rxdata_o_d[4] = serdes_ln2_rxdata_i[4];
  assign ln2_rxdata_o_d[5] = serdes_ln2_rxdata_i[5];
  assign ln2_rxdata_o_d[6] = serdes_ln2_rxdata_i[6];
  assign ln2_rxdata_o_d[7] = serdes_ln2_rxdata_i[7];
  assign ln2_rxk_o_d[0] = serdes_ln2_rxdata_i[8];
  assign ln2_rxdata_o_d[8] = serdes_ln2_rxdata_i[10];
  assign ln2_rxdata_o_d[9] = serdes_ln2_rxdata_i[11];
  assign ln2_rxdata_o_d[10] = serdes_ln2_rxdata_i[12];
  assign ln2_rxdata_o_d[11] = serdes_ln2_rxdata_i[13];
  assign ln2_rxdata_o_d[12] = serdes_ln2_rxdata_i[14];
  assign ln2_rxdata_o_d[13] = serdes_ln2_rxdata_i[15];
  assign ln2_rxdata_o_d[14] = serdes_ln2_rxdata_i[16];
  assign ln2_rxdata_o_d[15] = serdes_ln2_rxdata_i[17];
  assign ln2_rxk_o_d[1] = serdes_ln2_rxdata_i[18];
  assign ln2_rxdata_o_d[16] = serdes_ln2_rxdata_i[20];
  assign ln2_rxdata_o_d[17] = serdes_ln2_rxdata_i[21];
  assign ln2_rxdata_o_d[18] = serdes_ln2_rxdata_i[22];
  assign ln2_rxdata_o_d[19] = serdes_ln2_rxdata_i[23];
  assign ln2_rxdata_o_d[20] = serdes_ln2_rxdata_i[24];
  assign ln2_rxdata_o_d[21] = serdes_ln2_rxdata_i[25];
  assign ln2_rxdata_o_d[22] = serdes_ln2_rxdata_i[26];
  assign ln2_rxdata_o_d[23] = serdes_ln2_rxdata_i[27];
  assign ln2_rxk_o_d[2] = serdes_ln2_rxdata_i[28];
  assign ln2_rxdata_o_d[24] = serdes_ln2_rxdata_i[30];
  assign ln2_rxdata_o_d[25] = serdes_ln2_rxdata_i[31];
  assign ln2_rxdata_o_d[26] = serdes_ln2_rxdata_i[32];
  assign ln2_rxdata_o_d[27] = serdes_ln2_rxdata_i[33];
  assign ln2_rxdata_o_d[28] = serdes_ln2_rxdata_i[34];
  assign ln2_rxdata_o_d[29] = serdes_ln2_rxdata_i[35];
  assign ln2_rxdata_o_d[30] = serdes_ln2_rxdata_i[36];
  assign ln2_rxdata_o_d[31] = serdes_ln2_rxdata_i[37];
  assign ln2_rxk_o_d[3] = serdes_ln2_rxdata_i[38];
  assign ln2_rxfifo_rdusewd_o_d[0] = serdes_ln2_rxfifo_rdusewd_i[0];
  assign ln2_rxfifo_rdusewd_o_d[1] = serdes_ln2_rxfifo_rdusewd_i[1];
  assign ln2_rxfifo_rdusewd_o_d[2] = serdes_ln2_rxfifo_rdusewd_i[2];
  assign ln2_rxfifo_rdusewd_o_d[3] = serdes_ln2_rxfifo_rdusewd_i[3];
  assign ln2_rxfifo_rdusewd_o_d[4] = serdes_ln2_rxfifo_rdusewd_i[4];
  assign ln2_rxfifo_aempty_o_d = serdes_ln2_rxfifo_aempty_i;
  assign ln2_rxfifo_empty_o_d = serdes_ln2_rxfifo_empty_i;
  assign ln2_pcs_tx_clk_o_d = serdes_ln2_pcs_tx_clk_i;
  assign ln2_txfifo_wrusewd_o_d[0] = serdes_ln2_txfifo_wrusewd_i[0];
  assign ln2_txfifo_wrusewd_o_d[1] = serdes_ln2_txfifo_wrusewd_i[1];
  assign ln2_txfifo_wrusewd_o_d[2] = serdes_ln2_txfifo_wrusewd_i[2];
  assign ln2_txfifo_wrusewd_o_d[3] = serdes_ln2_txfifo_wrusewd_i[3];
  assign ln2_txfifo_wrusewd_o_d[4] = serdes_ln2_txfifo_wrusewd_i[4];
  assign ln2_txfifo_afull_o_d = serdes_ln2_txfifo_afull_i;
  assign ln2_txfifo_full_o_d = serdes_ln2_txfifo_full_i;
  assign serdes_ln2_fabric_rstn_o_d = ln2_fabric_rstn_i;
  assign serdes_ln2_rx_rst_o_d = ln2_rx_rst_i;
  assign serdes_ln2_chbond_start_o_d = ln2_chbond_start_i;
  assign serdes_ln2_fabric_rx_clk_o_d = ln2_fabric_rx_clk_i;
  assign serdes_ln2_rxfifo_rden_o_d = ln2_rxfifo_rden_i;
  assign serdes_ln2_tx_rst_o_d = ln2_tx_rst_i;
  assign serdes_ln2_fabric_tx_clk_o_d = ln2_fabric_tx_clk_i;
  assign serdes_ln2_tx_vld_o_d = ln2_tx_vld_i;
  assign serdes_ln2_txdata_o_d[0] = ln2_txdata_i[0];
  assign serdes_ln2_txdata_o_d[1] = ln2_txdata_i[1];
  assign serdes_ln2_txdata_o_d[2] = ln2_txdata_i[2];
  assign serdes_ln2_txdata_o_d[3] = ln2_txdata_i[3];
  assign serdes_ln2_txdata_o_d[4] = ln2_txdata_i[4];
  assign serdes_ln2_txdata_o_d[5] = ln2_txdata_i[5];
  assign serdes_ln2_txdata_o_d[6] = ln2_txdata_i[6];
  assign serdes_ln2_txdata_o_d[7] = ln2_txdata_i[7];
  assign serdes_ln2_txdata_o_d[10] = ln2_txdata_i[8];
  assign serdes_ln2_txdata_o_d[11] = ln2_txdata_i[9];
  assign serdes_ln2_txdata_o_d[12] = ln2_txdata_i[10];
  assign serdes_ln2_txdata_o_d[13] = ln2_txdata_i[11];
  assign serdes_ln2_txdata_o_d[14] = ln2_txdata_i[12];
  assign serdes_ln2_txdata_o_d[15] = ln2_txdata_i[13];
  assign serdes_ln2_txdata_o_d[16] = ln2_txdata_i[14];
  assign serdes_ln2_txdata_o_d[17] = ln2_txdata_i[15];
  assign serdes_ln2_txdata_o_d[20] = ln2_txdata_i[16];
  assign serdes_ln2_txdata_o_d[21] = ln2_txdata_i[17];
  assign serdes_ln2_txdata_o_d[22] = ln2_txdata_i[18];
  assign serdes_ln2_txdata_o_d[23] = ln2_txdata_i[19];
  assign serdes_ln2_txdata_o_d[24] = ln2_txdata_i[20];
  assign serdes_ln2_txdata_o_d[25] = ln2_txdata_i[21];
  assign serdes_ln2_txdata_o_d[26] = ln2_txdata_i[22];
  assign serdes_ln2_txdata_o_d[27] = ln2_txdata_i[23];
  assign serdes_ln2_txdata_o_d[30] = ln2_txdata_i[24];
  assign serdes_ln2_txdata_o_d[31] = ln2_txdata_i[25];
  assign serdes_ln2_txdata_o_d[32] = ln2_txdata_i[26];
  assign serdes_ln2_txdata_o_d[33] = ln2_txdata_i[27];
  assign serdes_ln2_txdata_o_d[34] = ln2_txdata_i[28];
  assign serdes_ln2_txdata_o_d[35] = ln2_txdata_i[29];
  assign serdes_ln2_txdata_o_d[36] = ln2_txdata_i[30];
  assign serdes_ln2_txdata_o_d[37] = ln2_txdata_i[31];
  assign serdes_ln2_txdata_o_d[8] = ln2_txk_i[0];
  assign serdes_ln2_txdata_o_d[18] = ln2_txk_i[1];
  assign serdes_ln2_txdata_o_d[28] = ln2_txk_i[2];
  assign serdes_ln2_txdata_o_d[38] = ln2_txk_i[3];
  assign ln3_cpll_ok_o_d = serdes_ln3_cpll_ok_i;
  assign ln3_pma_rx_lock_o_d = serdes_ln3_pma_rx_lock_i;
  assign ln3_align_link_o_d = serdes_ln3_align_link_i;
  assign ln3_k_lock_o_d = serdes_ln3_k_lock_i;
  assign ln3_pcs_rx_clk_o_d = serdes_ln3_pcs_rx_clk_i;
  assign ln3_rxdata_o_d[0] = serdes_ln3_rxdata_i[0];
  assign ln3_rxdata_o_d[1] = serdes_ln3_rxdata_i[1];
  assign ln3_rxdata_o_d[2] = serdes_ln3_rxdata_i[2];
  assign ln3_rxdata_o_d[3] = serdes_ln3_rxdata_i[3];
  assign ln3_rxdata_o_d[4] = serdes_ln3_rxdata_i[4];
  assign ln3_rxdata_o_d[5] = serdes_ln3_rxdata_i[5];
  assign ln3_rxdata_o_d[6] = serdes_ln3_rxdata_i[6];
  assign ln3_rxdata_o_d[7] = serdes_ln3_rxdata_i[7];
  assign ln3_rxk_o_d[0] = serdes_ln3_rxdata_i[8];
  assign ln3_rxdata_o_d[8] = serdes_ln3_rxdata_i[10];
  assign ln3_rxdata_o_d[9] = serdes_ln3_rxdata_i[11];
  assign ln3_rxdata_o_d[10] = serdes_ln3_rxdata_i[12];
  assign ln3_rxdata_o_d[11] = serdes_ln3_rxdata_i[13];
  assign ln3_rxdata_o_d[12] = serdes_ln3_rxdata_i[14];
  assign ln3_rxdata_o_d[13] = serdes_ln3_rxdata_i[15];
  assign ln3_rxdata_o_d[14] = serdes_ln3_rxdata_i[16];
  assign ln3_rxdata_o_d[15] = serdes_ln3_rxdata_i[17];
  assign ln3_rxk_o_d[1] = serdes_ln3_rxdata_i[18];
  assign ln3_rxdata_o_d[16] = serdes_ln3_rxdata_i[20];
  assign ln3_rxdata_o_d[17] = serdes_ln3_rxdata_i[21];
  assign ln3_rxdata_o_d[18] = serdes_ln3_rxdata_i[22];
  assign ln3_rxdata_o_d[19] = serdes_ln3_rxdata_i[23];
  assign ln3_rxdata_o_d[20] = serdes_ln3_rxdata_i[24];
  assign ln3_rxdata_o_d[21] = serdes_ln3_rxdata_i[25];
  assign ln3_rxdata_o_d[22] = serdes_ln3_rxdata_i[26];
  assign ln3_rxdata_o_d[23] = serdes_ln3_rxdata_i[27];
  assign ln3_rxk_o_d[2] = serdes_ln3_rxdata_i[28];
  assign ln3_rxdata_o_d[24] = serdes_ln3_rxdata_i[30];
  assign ln3_rxdata_o_d[25] = serdes_ln3_rxdata_i[31];
  assign ln3_rxdata_o_d[26] = serdes_ln3_rxdata_i[32];
  assign ln3_rxdata_o_d[27] = serdes_ln3_rxdata_i[33];
  assign ln3_rxdata_o_d[28] = serdes_ln3_rxdata_i[34];
  assign ln3_rxdata_o_d[29] = serdes_ln3_rxdata_i[35];
  assign ln3_rxdata_o_d[30] = serdes_ln3_rxdata_i[36];
  assign ln3_rxdata_o_d[31] = serdes_ln3_rxdata_i[37];
  assign ln3_rxk_o_d[3] = serdes_ln3_rxdata_i[38];
  assign ln3_rxfifo_rdusewd_o_d[0] = serdes_ln3_rxfifo_rdusewd_i[0];
  assign ln3_rxfifo_rdusewd_o_d[1] = serdes_ln3_rxfifo_rdusewd_i[1];
  assign ln3_rxfifo_rdusewd_o_d[2] = serdes_ln3_rxfifo_rdusewd_i[2];
  assign ln3_rxfifo_rdusewd_o_d[3] = serdes_ln3_rxfifo_rdusewd_i[3];
  assign ln3_rxfifo_rdusewd_o_d[4] = serdes_ln3_rxfifo_rdusewd_i[4];
  assign ln3_rxfifo_aempty_o_d = serdes_ln3_rxfifo_aempty_i;
  assign ln3_rxfifo_empty_o_d = serdes_ln3_rxfifo_empty_i;
  assign ln3_pcs_tx_clk_o_d = serdes_ln3_pcs_tx_clk_i;
  assign ln3_txfifo_wrusewd_o_d[0] = serdes_ln3_txfifo_wrusewd_i[0];
  assign ln3_txfifo_wrusewd_o_d[1] = serdes_ln3_txfifo_wrusewd_i[1];
  assign ln3_txfifo_wrusewd_o_d[2] = serdes_ln3_txfifo_wrusewd_i[2];
  assign ln3_txfifo_wrusewd_o_d[3] = serdes_ln3_txfifo_wrusewd_i[3];
  assign ln3_txfifo_wrusewd_o_d[4] = serdes_ln3_txfifo_wrusewd_i[4];
  assign ln3_txfifo_afull_o_d = serdes_ln3_txfifo_afull_i;
  assign ln3_txfifo_full_o_d = serdes_ln3_txfifo_full_i;
  assign serdes_ln3_fabric_rstn_o_d = ln3_fabric_rstn_i;
  assign serdes_ln3_rx_rst_o_d = ln3_rx_rst_i;
  assign serdes_ln3_chbond_start_o_d = ln3_chbond_start_i;
  assign serdes_ln3_fabric_rx_clk_o_d = ln3_fabric_rx_clk_i;
  assign serdes_ln3_rxfifo_rden_o_d = ln3_rxfifo_rden_i;
  assign serdes_ln3_tx_rst_o_d = ln3_tx_rst_i;
  assign serdes_ln3_fabric_tx_clk_o_d = ln3_fabric_tx_clk_i;
  assign serdes_ln3_tx_vld_o_d = ln3_tx_vld_i;
  assign serdes_ln3_txdata_o_d[0] = ln3_txdata_i[0];
  assign serdes_ln3_txdata_o_d[1] = ln3_txdata_i[1];
  assign serdes_ln3_txdata_o_d[2] = ln3_txdata_i[2];
  assign serdes_ln3_txdata_o_d[3] = ln3_txdata_i[3];
  assign serdes_ln3_txdata_o_d[4] = ln3_txdata_i[4];
  assign serdes_ln3_txdata_o_d[5] = ln3_txdata_i[5];
  assign serdes_ln3_txdata_o_d[6] = ln3_txdata_i[6];
  assign serdes_ln3_txdata_o_d[7] = ln3_txdata_i[7];
  assign serdes_ln3_txdata_o_d[10] = ln3_txdata_i[8];
  assign serdes_ln3_txdata_o_d[11] = ln3_txdata_i[9];
  assign serdes_ln3_txdata_o_d[12] = ln3_txdata_i[10];
  assign serdes_ln3_txdata_o_d[13] = ln3_txdata_i[11];
  assign serdes_ln3_txdata_o_d[14] = ln3_txdata_i[12];
  assign serdes_ln3_txdata_o_d[15] = ln3_txdata_i[13];
  assign serdes_ln3_txdata_o_d[16] = ln3_txdata_i[14];
  assign serdes_ln3_txdata_o_d[17] = ln3_txdata_i[15];
  assign serdes_ln3_txdata_o_d[20] = ln3_txdata_i[16];
  assign serdes_ln3_txdata_o_d[21] = ln3_txdata_i[17];
  assign serdes_ln3_txdata_o_d[22] = ln3_txdata_i[18];
  assign serdes_ln3_txdata_o_d[23] = ln3_txdata_i[19];
  assign serdes_ln3_txdata_o_d[24] = ln3_txdata_i[20];
  assign serdes_ln3_txdata_o_d[25] = ln3_txdata_i[21];
  assign serdes_ln3_txdata_o_d[26] = ln3_txdata_i[22];
  assign serdes_ln3_txdata_o_d[27] = ln3_txdata_i[23];
  assign serdes_ln3_txdata_o_d[30] = ln3_txdata_i[24];
  assign serdes_ln3_txdata_o_d[31] = ln3_txdata_i[25];
  assign serdes_ln3_txdata_o_d[32] = ln3_txdata_i[26];
  assign serdes_ln3_txdata_o_d[33] = ln3_txdata_i[27];
  assign serdes_ln3_txdata_o_d[34] = ln3_txdata_i[28];
  assign serdes_ln3_txdata_o_d[35] = ln3_txdata_i[29];
  assign serdes_ln3_txdata_o_d[36] = ln3_txdata_i[30];
  assign serdes_ln3_txdata_o_d[37] = ln3_txdata_i[31];
  assign serdes_ln3_txdata_o_d[8] = ln3_txk_i[0];
  assign serdes_ln3_txdata_o_d[18] = ln3_txk_i[1];
  assign serdes_ln3_txdata_o_d[28] = ln3_txk_i[2];
  assign serdes_ln3_txdata_o_d[38] = ln3_txk_i[3];
  assign drp_addr_o_d[0] = drp_addr_i[0];
  assign drp_addr_o_d[1] = drp_addr_i[1];
  assign drp_addr_o_d[2] = drp_addr_i[2];
  assign drp_addr_o_d[3] = drp_addr_i[3];
  assign drp_addr_o_d[4] = drp_addr_i[4];
  assign drp_addr_o_d[5] = drp_addr_i[5];
  assign drp_addr_o_d[6] = drp_addr_i[6];
  assign drp_addr_o_d[7] = drp_addr_i[7];
  assign drp_addr_o_d[8] = drp_addr_i[8];
  assign drp_addr_o_d[9] = drp_addr_i[9];
  assign drp_addr_o_d[10] = drp_addr_i[10];
  assign drp_addr_o_d[11] = drp_addr_i[11];
  assign drp_addr_o_d[12] = drp_addr_i[12];
  assign drp_addr_o_d[13] = drp_addr_i[13];
  assign drp_addr_o_d[14] = drp_addr_i[14];
  assign drp_addr_o_d[15] = drp_addr_i[15];
  assign drp_addr_o_d[16] = drp_addr_i[16];
  assign drp_addr_o_d[17] = drp_addr_i[17];
  assign drp_addr_o_d[18] = drp_addr_i[18];
  assign drp_addr_o_d[19] = drp_addr_i[19];
  assign drp_addr_o_d[20] = drp_addr_i[20];
  assign drp_addr_o_d[21] = drp_addr_i[21];
  assign drp_addr_o_d[22] = drp_addr_i[22];
  assign drp_addr_o_d[23] = drp_addr_i[23];
  assign drp_wren_o_d = drp_wren_i;
  assign drp_wrdata_o_d[0] = drp_wrdata_i[0];
  assign drp_wrdata_o_d[1] = drp_wrdata_i[1];
  assign drp_wrdata_o_d[2] = drp_wrdata_i[2];
  assign drp_wrdata_o_d[3] = drp_wrdata_i[3];
  assign drp_wrdata_o_d[4] = drp_wrdata_i[4];
  assign drp_wrdata_o_d[5] = drp_wrdata_i[5];
  assign drp_wrdata_o_d[6] = drp_wrdata_i[6];
  assign drp_wrdata_o_d[7] = drp_wrdata_i[7];
  assign drp_wrdata_o_d[8] = drp_wrdata_i[8];
  assign drp_wrdata_o_d[9] = drp_wrdata_i[9];
  assign drp_wrdata_o_d[10] = drp_wrdata_i[10];
  assign drp_wrdata_o_d[11] = drp_wrdata_i[11];
  assign drp_wrdata_o_d[12] = drp_wrdata_i[12];
  assign drp_wrdata_o_d[13] = drp_wrdata_i[13];
  assign drp_wrdata_o_d[14] = drp_wrdata_i[14];
  assign drp_wrdata_o_d[15] = drp_wrdata_i[15];
  assign drp_wrdata_o_d[16] = drp_wrdata_i[16];
  assign drp_wrdata_o_d[17] = drp_wrdata_i[17];
  assign drp_wrdata_o_d[18] = drp_wrdata_i[18];
  assign drp_wrdata_o_d[19] = drp_wrdata_i[19];
  assign drp_wrdata_o_d[20] = drp_wrdata_i[20];
  assign drp_wrdata_o_d[21] = drp_wrdata_i[21];
  assign drp_wrdata_o_d[22] = drp_wrdata_i[22];
  assign drp_wrdata_o_d[23] = drp_wrdata_i[23];
  assign drp_wrdata_o_d[24] = drp_wrdata_i[24];
  assign drp_wrdata_o_d[25] = drp_wrdata_i[25];
  assign drp_wrdata_o_d[26] = drp_wrdata_i[26];
  assign drp_wrdata_o_d[27] = drp_wrdata_i[27];
  assign drp_wrdata_o_d[28] = drp_wrdata_i[28];
  assign drp_wrdata_o_d[29] = drp_wrdata_i[29];
  assign drp_wrdata_o_d[30] = drp_wrdata_i[30];
  assign drp_wrdata_o_d[31] = drp_wrdata_i[31];
  assign drp_strb_o_d[0] = drp_strb_i[0];
  assign drp_strb_o_d[1] = drp_strb_i[1];
  assign drp_strb_o_d[2] = drp_strb_i[2];
  assign drp_strb_o_d[3] = drp_strb_i[3];
  assign drp_strb_o_d[4] = drp_strb_i[4];
  assign drp_strb_o_d[5] = drp_strb_i[5];
  assign drp_strb_o_d[6] = drp_strb_i[6];
  assign drp_strb_o_d[7] = drp_strb_i[7];
  assign drp_rden_o_d = drp_rden_i;
  assign drp_clk_o_d = drp_clk_i;
  assign drp_ready_o_d = drp_ready_i;
  assign drp_rdvld_o_d = drp_rdvld_i;
  assign drp_rddata_o_d[0] = drp_rddata_i[0];
  assign drp_rddata_o_d[1] = drp_rddata_i[1];
  assign drp_rddata_o_d[2] = drp_rddata_i[2];
  assign drp_rddata_o_d[3] = drp_rddata_i[3];
  assign drp_rddata_o_d[4] = drp_rddata_i[4];
  assign drp_rddata_o_d[5] = drp_rddata_i[5];
  assign drp_rddata_o_d[6] = drp_rddata_i[6];
  assign drp_rddata_o_d[7] = drp_rddata_i[7];
  assign drp_rddata_o_d[8] = drp_rddata_i[8];
  assign drp_rddata_o_d[9] = drp_rddata_i[9];
  assign drp_rddata_o_d[10] = drp_rddata_i[10];
  assign drp_rddata_o_d[11] = drp_rddata_i[11];
  assign drp_rddata_o_d[12] = drp_rddata_i[12];
  assign drp_rddata_o_d[13] = drp_rddata_i[13];
  assign drp_rddata_o_d[14] = drp_rddata_i[14];
  assign drp_rddata_o_d[15] = drp_rddata_i[15];
  assign drp_rddata_o_d[16] = drp_rddata_i[16];
  assign drp_rddata_o_d[17] = drp_rddata_i[17];
  assign drp_rddata_o_d[18] = drp_rddata_i[18];
  assign drp_rddata_o_d[19] = drp_rddata_i[19];
  assign drp_rddata_o_d[20] = drp_rddata_i[20];
  assign drp_rddata_o_d[21] = drp_rddata_i[21];
  assign drp_rddata_o_d[22] = drp_rddata_i[22];
  assign drp_rddata_o_d[23] = drp_rddata_i[23];
  assign drp_rddata_o_d[24] = drp_rddata_i[24];
  assign drp_rddata_o_d[25] = drp_rddata_i[25];
  assign drp_rddata_o_d[26] = drp_rddata_i[26];
  assign drp_rddata_o_d[27] = drp_rddata_i[27];
  assign drp_rddata_o_d[28] = drp_rddata_i[28];
  assign drp_rddata_o_d[29] = drp_rddata_i[29];
  assign drp_rddata_o_d[30] = drp_rddata_i[30];
  assign drp_rddata_o_d[31] = drp_rddata_i[31];
  assign drp_resp_o_d = drp_resp_i;
  assign qpll0_ok_o_d = serdes_qpll0_ok_i;
  assign qpll1_ok_o_d = serdes_qpll1_ok_i;
  assign serdes_ln2_fabric_rstn_o = serdes_ln2_fabric_rstn_o_d;
  assign serdes_ln2_rx_rst_o = serdes_ln2_rx_rst_o_d;
  assign serdes_ln2_chbond_start_o = serdes_ln2_chbond_start_o_d;
  assign serdes_ln2_fabric_rx_clk_o = serdes_ln2_fabric_rx_clk_o_d;
  assign serdes_ln2_rxfifo_rden_o = serdes_ln2_rxfifo_rden_o_d;
  assign serdes_ln2_tx_rst_o = serdes_ln2_tx_rst_o_d;
  assign serdes_ln2_fabric_tx_clk_o = serdes_ln2_fabric_tx_clk_o_d;
  assign serdes_ln2_txdata_o[0] = serdes_ln2_txdata_o_d[0];
  assign serdes_ln2_txdata_o[1] = serdes_ln2_txdata_o_d[1];
  assign serdes_ln2_txdata_o[2] = serdes_ln2_txdata_o_d[2];
  assign serdes_ln2_txdata_o[3] = serdes_ln2_txdata_o_d[3];
  assign serdes_ln2_txdata_o[4] = serdes_ln2_txdata_o_d[4];
  assign serdes_ln2_txdata_o[5] = serdes_ln2_txdata_o_d[5];
  assign serdes_ln2_txdata_o[6] = serdes_ln2_txdata_o_d[6];
  assign serdes_ln2_txdata_o[7] = serdes_ln2_txdata_o_d[7];
  assign serdes_ln2_txdata_o[8] = serdes_ln2_txdata_o_d[8];
  assign serdes_ln2_txdata_o[9] = GND;
  assign serdes_ln2_txdata_o[10] = serdes_ln2_txdata_o_d[10];
  assign serdes_ln2_txdata_o[11] = serdes_ln2_txdata_o_d[11];
  assign serdes_ln2_txdata_o[12] = serdes_ln2_txdata_o_d[12];
  assign serdes_ln2_txdata_o[13] = serdes_ln2_txdata_o_d[13];
  assign serdes_ln2_txdata_o[14] = serdes_ln2_txdata_o_d[14];
  assign serdes_ln2_txdata_o[15] = serdes_ln2_txdata_o_d[15];
  assign serdes_ln2_txdata_o[16] = serdes_ln2_txdata_o_d[16];
  assign serdes_ln2_txdata_o[17] = serdes_ln2_txdata_o_d[17];
  assign serdes_ln2_txdata_o[18] = serdes_ln2_txdata_o_d[18];
  assign serdes_ln2_txdata_o[19] = GND;
  assign serdes_ln2_txdata_o[20] = serdes_ln2_txdata_o_d[20];
  assign serdes_ln2_txdata_o[21] = serdes_ln2_txdata_o_d[21];
  assign serdes_ln2_txdata_o[22] = serdes_ln2_txdata_o_d[22];
  assign serdes_ln2_txdata_o[23] = serdes_ln2_txdata_o_d[23];
  assign serdes_ln2_txdata_o[24] = serdes_ln2_txdata_o_d[24];
  assign serdes_ln2_txdata_o[25] = serdes_ln2_txdata_o_d[25];
  assign serdes_ln2_txdata_o[26] = serdes_ln2_txdata_o_d[26];
  assign serdes_ln2_txdata_o[27] = serdes_ln2_txdata_o_d[27];
  assign serdes_ln2_txdata_o[28] = serdes_ln2_txdata_o_d[28];
  assign serdes_ln2_txdata_o[29] = GND;
  assign serdes_ln2_txdata_o[30] = serdes_ln2_txdata_o_d[30];
  assign serdes_ln2_txdata_o[31] = serdes_ln2_txdata_o_d[31];
  assign serdes_ln2_txdata_o[32] = serdes_ln2_txdata_o_d[32];
  assign serdes_ln2_txdata_o[33] = serdes_ln2_txdata_o_d[33];
  assign serdes_ln2_txdata_o[34] = serdes_ln2_txdata_o_d[34];
  assign serdes_ln2_txdata_o[35] = serdes_ln2_txdata_o_d[35];
  assign serdes_ln2_txdata_o[36] = serdes_ln2_txdata_o_d[36];
  assign serdes_ln2_txdata_o[37] = serdes_ln2_txdata_o_d[37];
  assign serdes_ln2_txdata_o[38] = serdes_ln2_txdata_o_d[38];
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
  assign serdes_ln2_tx_vld_o = serdes_ln2_tx_vld_o_d;
  assign ln2_cpll_ok_o = ln2_cpll_ok_o_d;
  assign ln2_pma_rx_lock_o = ln2_pma_rx_lock_o_d;
  assign ln2_align_link_o = ln2_align_link_o_d;
  assign ln2_k_lock_o = ln2_k_lock_o_d;
  assign ln2_pcs_rx_clk_o = ln2_pcs_rx_clk_o_d;
  assign ln2_rxfifo_rdusewd_o[0] = ln2_rxfifo_rdusewd_o_d[0];
  assign ln2_rxfifo_rdusewd_o[1] = ln2_rxfifo_rdusewd_o_d[1];
  assign ln2_rxfifo_rdusewd_o[2] = ln2_rxfifo_rdusewd_o_d[2];
  assign ln2_rxfifo_rdusewd_o[3] = ln2_rxfifo_rdusewd_o_d[3];
  assign ln2_rxfifo_rdusewd_o[4] = ln2_rxfifo_rdusewd_o_d[4];
  assign ln2_rxfifo_aempty_o = ln2_rxfifo_aempty_o_d;
  assign ln2_rxfifo_empty_o = ln2_rxfifo_empty_o_d;
  assign ln2_pcs_tx_clk_o = ln2_pcs_tx_clk_o_d;
  assign ln2_txfifo_wrusewd_o[0] = ln2_txfifo_wrusewd_o_d[0];
  assign ln2_txfifo_wrusewd_o[1] = ln2_txfifo_wrusewd_o_d[1];
  assign ln2_txfifo_wrusewd_o[2] = ln2_txfifo_wrusewd_o_d[2];
  assign ln2_txfifo_wrusewd_o[3] = ln2_txfifo_wrusewd_o_d[3];
  assign ln2_txfifo_wrusewd_o[4] = ln2_txfifo_wrusewd_o_d[4];
  assign ln2_txfifo_afull_o = ln2_txfifo_afull_o_d;
  assign ln2_txfifo_full_o = ln2_txfifo_full_o_d;
  assign ln2_rxdata_o[0] = ln2_rxdata_o_d[0];
  assign ln2_rxdata_o[1] = ln2_rxdata_o_d[1];
  assign ln2_rxdata_o[2] = ln2_rxdata_o_d[2];
  assign ln2_rxdata_o[3] = ln2_rxdata_o_d[3];
  assign ln2_rxdata_o[4] = ln2_rxdata_o_d[4];
  assign ln2_rxdata_o[5] = ln2_rxdata_o_d[5];
  assign ln2_rxdata_o[6] = ln2_rxdata_o_d[6];
  assign ln2_rxdata_o[7] = ln2_rxdata_o_d[7];
  assign ln2_rxdata_o[8] = ln2_rxdata_o_d[8];
  assign ln2_rxdata_o[9] = ln2_rxdata_o_d[9];
  assign ln2_rxdata_o[10] = ln2_rxdata_o_d[10];
  assign ln2_rxdata_o[11] = ln2_rxdata_o_d[11];
  assign ln2_rxdata_o[12] = ln2_rxdata_o_d[12];
  assign ln2_rxdata_o[13] = ln2_rxdata_o_d[13];
  assign ln2_rxdata_o[14] = ln2_rxdata_o_d[14];
  assign ln2_rxdata_o[15] = ln2_rxdata_o_d[15];
  assign ln2_rxdata_o[16] = ln2_rxdata_o_d[16];
  assign ln2_rxdata_o[17] = ln2_rxdata_o_d[17];
  assign ln2_rxdata_o[18] = ln2_rxdata_o_d[18];
  assign ln2_rxdata_o[19] = ln2_rxdata_o_d[19];
  assign ln2_rxdata_o[20] = ln2_rxdata_o_d[20];
  assign ln2_rxdata_o[21] = ln2_rxdata_o_d[21];
  assign ln2_rxdata_o[22] = ln2_rxdata_o_d[22];
  assign ln2_rxdata_o[23] = ln2_rxdata_o_d[23];
  assign ln2_rxdata_o[24] = ln2_rxdata_o_d[24];
  assign ln2_rxdata_o[25] = ln2_rxdata_o_d[25];
  assign ln2_rxdata_o[26] = ln2_rxdata_o_d[26];
  assign ln2_rxdata_o[27] = ln2_rxdata_o_d[27];
  assign ln2_rxdata_o[28] = ln2_rxdata_o_d[28];
  assign ln2_rxdata_o[29] = ln2_rxdata_o_d[29];
  assign ln2_rxdata_o[30] = ln2_rxdata_o_d[30];
  assign ln2_rxdata_o[31] = ln2_rxdata_o_d[31];
  assign ln2_rxk_o[0] = ln2_rxk_o_d[0];
  assign ln2_rxk_o[1] = ln2_rxk_o_d[1];
  assign ln2_rxk_o[2] = ln2_rxk_o_d[2];
  assign ln2_rxk_o[3] = ln2_rxk_o_d[3];
  assign serdes_ln3_fabric_rstn_o = serdes_ln3_fabric_rstn_o_d;
  assign serdes_ln3_rx_rst_o = serdes_ln3_rx_rst_o_d;
  assign serdes_ln3_chbond_start_o = serdes_ln3_chbond_start_o_d;
  assign serdes_ln3_fabric_rx_clk_o = serdes_ln3_fabric_rx_clk_o_d;
  assign serdes_ln3_rxfifo_rden_o = serdes_ln3_rxfifo_rden_o_d;
  assign serdes_ln3_tx_rst_o = serdes_ln3_tx_rst_o_d;
  assign serdes_ln3_fabric_tx_clk_o = serdes_ln3_fabric_tx_clk_o_d;
  assign serdes_ln3_txdata_o[0] = serdes_ln3_txdata_o_d[0];
  assign serdes_ln3_txdata_o[1] = serdes_ln3_txdata_o_d[1];
  assign serdes_ln3_txdata_o[2] = serdes_ln3_txdata_o_d[2];
  assign serdes_ln3_txdata_o[3] = serdes_ln3_txdata_o_d[3];
  assign serdes_ln3_txdata_o[4] = serdes_ln3_txdata_o_d[4];
  assign serdes_ln3_txdata_o[5] = serdes_ln3_txdata_o_d[5];
  assign serdes_ln3_txdata_o[6] = serdes_ln3_txdata_o_d[6];
  assign serdes_ln3_txdata_o[7] = serdes_ln3_txdata_o_d[7];
  assign serdes_ln3_txdata_o[8] = serdes_ln3_txdata_o_d[8];
  assign serdes_ln3_txdata_o[9] = GND;
  assign serdes_ln3_txdata_o[10] = serdes_ln3_txdata_o_d[10];
  assign serdes_ln3_txdata_o[11] = serdes_ln3_txdata_o_d[11];
  assign serdes_ln3_txdata_o[12] = serdes_ln3_txdata_o_d[12];
  assign serdes_ln3_txdata_o[13] = serdes_ln3_txdata_o_d[13];
  assign serdes_ln3_txdata_o[14] = serdes_ln3_txdata_o_d[14];
  assign serdes_ln3_txdata_o[15] = serdes_ln3_txdata_o_d[15];
  assign serdes_ln3_txdata_o[16] = serdes_ln3_txdata_o_d[16];
  assign serdes_ln3_txdata_o[17] = serdes_ln3_txdata_o_d[17];
  assign serdes_ln3_txdata_o[18] = serdes_ln3_txdata_o_d[18];
  assign serdes_ln3_txdata_o[19] = GND;
  assign serdes_ln3_txdata_o[20] = serdes_ln3_txdata_o_d[20];
  assign serdes_ln3_txdata_o[21] = serdes_ln3_txdata_o_d[21];
  assign serdes_ln3_txdata_o[22] = serdes_ln3_txdata_o_d[22];
  assign serdes_ln3_txdata_o[23] = serdes_ln3_txdata_o_d[23];
  assign serdes_ln3_txdata_o[24] = serdes_ln3_txdata_o_d[24];
  assign serdes_ln3_txdata_o[25] = serdes_ln3_txdata_o_d[25];
  assign serdes_ln3_txdata_o[26] = serdes_ln3_txdata_o_d[26];
  assign serdes_ln3_txdata_o[27] = serdes_ln3_txdata_o_d[27];
  assign serdes_ln3_txdata_o[28] = serdes_ln3_txdata_o_d[28];
  assign serdes_ln3_txdata_o[29] = GND;
  assign serdes_ln3_txdata_o[30] = serdes_ln3_txdata_o_d[30];
  assign serdes_ln3_txdata_o[31] = serdes_ln3_txdata_o_d[31];
  assign serdes_ln3_txdata_o[32] = serdes_ln3_txdata_o_d[32];
  assign serdes_ln3_txdata_o[33] = serdes_ln3_txdata_o_d[33];
  assign serdes_ln3_txdata_o[34] = serdes_ln3_txdata_o_d[34];
  assign serdes_ln3_txdata_o[35] = serdes_ln3_txdata_o_d[35];
  assign serdes_ln3_txdata_o[36] = serdes_ln3_txdata_o_d[36];
  assign serdes_ln3_txdata_o[37] = serdes_ln3_txdata_o_d[37];
  assign serdes_ln3_txdata_o[38] = serdes_ln3_txdata_o_d[38];
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
  assign serdes_ln3_tx_vld_o = serdes_ln3_tx_vld_o_d;
  assign ln3_cpll_ok_o = ln3_cpll_ok_o_d;
  assign ln3_pma_rx_lock_o = ln3_pma_rx_lock_o_d;
  assign ln3_align_link_o = ln3_align_link_o_d;
  assign ln3_k_lock_o = ln3_k_lock_o_d;
  assign ln3_pcs_rx_clk_o = ln3_pcs_rx_clk_o_d;
  assign ln3_rxfifo_rdusewd_o[0] = ln3_rxfifo_rdusewd_o_d[0];
  assign ln3_rxfifo_rdusewd_o[1] = ln3_rxfifo_rdusewd_o_d[1];
  assign ln3_rxfifo_rdusewd_o[2] = ln3_rxfifo_rdusewd_o_d[2];
  assign ln3_rxfifo_rdusewd_o[3] = ln3_rxfifo_rdusewd_o_d[3];
  assign ln3_rxfifo_rdusewd_o[4] = ln3_rxfifo_rdusewd_o_d[4];
  assign ln3_rxfifo_aempty_o = ln3_rxfifo_aempty_o_d;
  assign ln3_rxfifo_empty_o = ln3_rxfifo_empty_o_d;
  assign ln3_pcs_tx_clk_o = ln3_pcs_tx_clk_o_d;
  assign ln3_txfifo_wrusewd_o[0] = ln3_txfifo_wrusewd_o_d[0];
  assign ln3_txfifo_wrusewd_o[1] = ln3_txfifo_wrusewd_o_d[1];
  assign ln3_txfifo_wrusewd_o[2] = ln3_txfifo_wrusewd_o_d[2];
  assign ln3_txfifo_wrusewd_o[3] = ln3_txfifo_wrusewd_o_d[3];
  assign ln3_txfifo_wrusewd_o[4] = ln3_txfifo_wrusewd_o_d[4];
  assign ln3_txfifo_afull_o = ln3_txfifo_afull_o_d;
  assign ln3_txfifo_full_o = ln3_txfifo_full_o_d;
  assign ln3_rxdata_o[0] = ln3_rxdata_o_d[0];
  assign ln3_rxdata_o[1] = ln3_rxdata_o_d[1];
  assign ln3_rxdata_o[2] = ln3_rxdata_o_d[2];
  assign ln3_rxdata_o[3] = ln3_rxdata_o_d[3];
  assign ln3_rxdata_o[4] = ln3_rxdata_o_d[4];
  assign ln3_rxdata_o[5] = ln3_rxdata_o_d[5];
  assign ln3_rxdata_o[6] = ln3_rxdata_o_d[6];
  assign ln3_rxdata_o[7] = ln3_rxdata_o_d[7];
  assign ln3_rxdata_o[8] = ln3_rxdata_o_d[8];
  assign ln3_rxdata_o[9] = ln3_rxdata_o_d[9];
  assign ln3_rxdata_o[10] = ln3_rxdata_o_d[10];
  assign ln3_rxdata_o[11] = ln3_rxdata_o_d[11];
  assign ln3_rxdata_o[12] = ln3_rxdata_o_d[12];
  assign ln3_rxdata_o[13] = ln3_rxdata_o_d[13];
  assign ln3_rxdata_o[14] = ln3_rxdata_o_d[14];
  assign ln3_rxdata_o[15] = ln3_rxdata_o_d[15];
  assign ln3_rxdata_o[16] = ln3_rxdata_o_d[16];
  assign ln3_rxdata_o[17] = ln3_rxdata_o_d[17];
  assign ln3_rxdata_o[18] = ln3_rxdata_o_d[18];
  assign ln3_rxdata_o[19] = ln3_rxdata_o_d[19];
  assign ln3_rxdata_o[20] = ln3_rxdata_o_d[20];
  assign ln3_rxdata_o[21] = ln3_rxdata_o_d[21];
  assign ln3_rxdata_o[22] = ln3_rxdata_o_d[22];
  assign ln3_rxdata_o[23] = ln3_rxdata_o_d[23];
  assign ln3_rxdata_o[24] = ln3_rxdata_o_d[24];
  assign ln3_rxdata_o[25] = ln3_rxdata_o_d[25];
  assign ln3_rxdata_o[26] = ln3_rxdata_o_d[26];
  assign ln3_rxdata_o[27] = ln3_rxdata_o_d[27];
  assign ln3_rxdata_o[28] = ln3_rxdata_o_d[28];
  assign ln3_rxdata_o[29] = ln3_rxdata_o_d[29];
  assign ln3_rxdata_o[30] = ln3_rxdata_o_d[30];
  assign ln3_rxdata_o[31] = ln3_rxdata_o_d[31];
  assign ln3_rxk_o[0] = ln3_rxk_o_d[0];
  assign ln3_rxk_o[1] = ln3_rxk_o_d[1];
  assign ln3_rxk_o[2] = ln3_rxk_o_d[2];
  assign ln3_rxk_o[3] = ln3_rxk_o_d[3];
  assign drp_clk_o = drp_clk_o_d;
  assign drp_ready_o = drp_ready_o_d;
  assign drp_rdvld_o = drp_rdvld_o_d;
  assign drp_rddata_o[0] = drp_rddata_o_d[0];
  assign drp_rddata_o[1] = drp_rddata_o_d[1];
  assign drp_rddata_o[2] = drp_rddata_o_d[2];
  assign drp_rddata_o[3] = drp_rddata_o_d[3];
  assign drp_rddata_o[4] = drp_rddata_o_d[4];
  assign drp_rddata_o[5] = drp_rddata_o_d[5];
  assign drp_rddata_o[6] = drp_rddata_o_d[6];
  assign drp_rddata_o[7] = drp_rddata_o_d[7];
  assign drp_rddata_o[8] = drp_rddata_o_d[8];
  assign drp_rddata_o[9] = drp_rddata_o_d[9];
  assign drp_rddata_o[10] = drp_rddata_o_d[10];
  assign drp_rddata_o[11] = drp_rddata_o_d[11];
  assign drp_rddata_o[12] = drp_rddata_o_d[12];
  assign drp_rddata_o[13] = drp_rddata_o_d[13];
  assign drp_rddata_o[14] = drp_rddata_o_d[14];
  assign drp_rddata_o[15] = drp_rddata_o_d[15];
  assign drp_rddata_o[16] = drp_rddata_o_d[16];
  assign drp_rddata_o[17] = drp_rddata_o_d[17];
  assign drp_rddata_o[18] = drp_rddata_o_d[18];
  assign drp_rddata_o[19] = drp_rddata_o_d[19];
  assign drp_rddata_o[20] = drp_rddata_o_d[20];
  assign drp_rddata_o[21] = drp_rddata_o_d[21];
  assign drp_rddata_o[22] = drp_rddata_o_d[22];
  assign drp_rddata_o[23] = drp_rddata_o_d[23];
  assign drp_rddata_o[24] = drp_rddata_o_d[24];
  assign drp_rddata_o[25] = drp_rddata_o_d[25];
  assign drp_rddata_o[26] = drp_rddata_o_d[26];
  assign drp_rddata_o[27] = drp_rddata_o_d[27];
  assign drp_rddata_o[28] = drp_rddata_o_d[28];
  assign drp_rddata_o[29] = drp_rddata_o_d[29];
  assign drp_rddata_o[30] = drp_rddata_o_d[30];
  assign drp_rddata_o[31] = drp_rddata_o_d[31];
  assign drp_resp_o = drp_resp_o_d;
  assign drp_addr_o[0] = drp_addr_o_d[0];
  assign drp_addr_o[1] = drp_addr_o_d[1];
  assign drp_addr_o[2] = drp_addr_o_d[2];
  assign drp_addr_o[3] = drp_addr_o_d[3];
  assign drp_addr_o[4] = drp_addr_o_d[4];
  assign drp_addr_o[5] = drp_addr_o_d[5];
  assign drp_addr_o[6] = drp_addr_o_d[6];
  assign drp_addr_o[7] = drp_addr_o_d[7];
  assign drp_addr_o[8] = drp_addr_o_d[8];
  assign drp_addr_o[9] = drp_addr_o_d[9];
  assign drp_addr_o[10] = drp_addr_o_d[10];
  assign drp_addr_o[11] = drp_addr_o_d[11];
  assign drp_addr_o[12] = drp_addr_o_d[12];
  assign drp_addr_o[13] = drp_addr_o_d[13];
  assign drp_addr_o[14] = drp_addr_o_d[14];
  assign drp_addr_o[15] = drp_addr_o_d[15];
  assign drp_addr_o[16] = drp_addr_o_d[16];
  assign drp_addr_o[17] = drp_addr_o_d[17];
  assign drp_addr_o[18] = drp_addr_o_d[18];
  assign drp_addr_o[19] = drp_addr_o_d[19];
  assign drp_addr_o[20] = drp_addr_o_d[20];
  assign drp_addr_o[21] = drp_addr_o_d[21];
  assign drp_addr_o[22] = drp_addr_o_d[22];
  assign drp_addr_o[23] = drp_addr_o_d[23];
  assign drp_wren_o = drp_wren_o_d;
  assign drp_wrdata_o[0] = drp_wrdata_o_d[0];
  assign drp_wrdata_o[1] = drp_wrdata_o_d[1];
  assign drp_wrdata_o[2] = drp_wrdata_o_d[2];
  assign drp_wrdata_o[3] = drp_wrdata_o_d[3];
  assign drp_wrdata_o[4] = drp_wrdata_o_d[4];
  assign drp_wrdata_o[5] = drp_wrdata_o_d[5];
  assign drp_wrdata_o[6] = drp_wrdata_o_d[6];
  assign drp_wrdata_o[7] = drp_wrdata_o_d[7];
  assign drp_wrdata_o[8] = drp_wrdata_o_d[8];
  assign drp_wrdata_o[9] = drp_wrdata_o_d[9];
  assign drp_wrdata_o[10] = drp_wrdata_o_d[10];
  assign drp_wrdata_o[11] = drp_wrdata_o_d[11];
  assign drp_wrdata_o[12] = drp_wrdata_o_d[12];
  assign drp_wrdata_o[13] = drp_wrdata_o_d[13];
  assign drp_wrdata_o[14] = drp_wrdata_o_d[14];
  assign drp_wrdata_o[15] = drp_wrdata_o_d[15];
  assign drp_wrdata_o[16] = drp_wrdata_o_d[16];
  assign drp_wrdata_o[17] = drp_wrdata_o_d[17];
  assign drp_wrdata_o[18] = drp_wrdata_o_d[18];
  assign drp_wrdata_o[19] = drp_wrdata_o_d[19];
  assign drp_wrdata_o[20] = drp_wrdata_o_d[20];
  assign drp_wrdata_o[21] = drp_wrdata_o_d[21];
  assign drp_wrdata_o[22] = drp_wrdata_o_d[22];
  assign drp_wrdata_o[23] = drp_wrdata_o_d[23];
  assign drp_wrdata_o[24] = drp_wrdata_o_d[24];
  assign drp_wrdata_o[25] = drp_wrdata_o_d[25];
  assign drp_wrdata_o[26] = drp_wrdata_o_d[26];
  assign drp_wrdata_o[27] = drp_wrdata_o_d[27];
  assign drp_wrdata_o[28] = drp_wrdata_o_d[28];
  assign drp_wrdata_o[29] = drp_wrdata_o_d[29];
  assign drp_wrdata_o[30] = drp_wrdata_o_d[30];
  assign drp_wrdata_o[31] = drp_wrdata_o_d[31];
  assign drp_strb_o[0] = drp_strb_o_d[0];
  assign drp_strb_o[1] = drp_strb_o_d[1];
  assign drp_strb_o[2] = drp_strb_o_d[2];
  assign drp_strb_o[3] = drp_strb_o_d[3];
  assign drp_strb_o[4] = drp_strb_o_d[4];
  assign drp_strb_o[5] = drp_strb_o_d[5];
  assign drp_strb_o[6] = drp_strb_o_d[6];
  assign drp_strb_o[7] = drp_strb_o_d[7];
  assign drp_rden_o = drp_rden_o_d;
  assign qpll0_ok_o = qpll0_ok_o_d;
  assign qpll1_ok_o = qpll1_ok_o_d;
  VCC VCC_cZ (
    .V(VCC)
);
  GND GND_cZ (
    .G(GND)
);
  GSR GSR (
    .GSRI(VCC) 
);
endmodule /* edp_phy */
