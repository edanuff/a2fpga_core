//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.01
//Part Number: GW5AT-LV60PG484AC1/I0
//Device: GW5AT-60
//Device Version: B
//Created Time: Sun Aug 16 08:33:31 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

	edp_phy your_instance_name(
		.serdes_ln2_cpll_ok_i(serdes_ln2_cpll_ok_i), //input serdes_ln2_cpll_ok_i
		.serdes_ln2_fabric_rstn_o(serdes_ln2_fabric_rstn_o), //output serdes_ln2_fabric_rstn_o
		.serdes_ln2_rx_rst_o(serdes_ln2_rx_rst_o), //output serdes_ln2_rx_rst_o
		.serdes_ln2_pma_rx_lock_i(serdes_ln2_pma_rx_lock_i), //input serdes_ln2_pma_rx_lock_i
		.serdes_ln2_chbond_start_o(serdes_ln2_chbond_start_o), //output serdes_ln2_chbond_start_o
		.serdes_ln2_align_link_i(serdes_ln2_align_link_i), //input serdes_ln2_align_link_i
		.serdes_ln2_k_lock_i(serdes_ln2_k_lock_i), //input serdes_ln2_k_lock_i
		.serdes_ln2_pcs_rx_clk_i(serdes_ln2_pcs_rx_clk_i), //input serdes_ln2_pcs_rx_clk_i
		.serdes_ln2_fabric_rx_clk_o(serdes_ln2_fabric_rx_clk_o), //output serdes_ln2_fabric_rx_clk_o
		.serdes_ln2_rxdata_i(serdes_ln2_rxdata_i), //input [87:0] serdes_ln2_rxdata_i
		.serdes_ln2_rxfifo_rdusewd_i(serdes_ln2_rxfifo_rdusewd_i), //input [4:0] serdes_ln2_rxfifo_rdusewd_i
		.serdes_ln2_rxfifo_aempty_i(serdes_ln2_rxfifo_aempty_i), //input serdes_ln2_rxfifo_aempty_i
		.serdes_ln2_rxfifo_empty_i(serdes_ln2_rxfifo_empty_i), //input serdes_ln2_rxfifo_empty_i
		.serdes_ln2_rxfifo_rden_o(serdes_ln2_rxfifo_rden_o), //output serdes_ln2_rxfifo_rden_o
		.serdes_ln2_tx_rst_o(serdes_ln2_tx_rst_o), //output serdes_ln2_tx_rst_o
		.serdes_ln2_pcs_tx_clk_i(serdes_ln2_pcs_tx_clk_i), //input serdes_ln2_pcs_tx_clk_i
		.serdes_ln2_fabric_tx_clk_o(serdes_ln2_fabric_tx_clk_o), //output serdes_ln2_fabric_tx_clk_o
		.serdes_ln2_txdata_o(serdes_ln2_txdata_o), //output [79:0] serdes_ln2_txdata_o
		.serdes_ln2_txfifo_wrusewd_i(serdes_ln2_txfifo_wrusewd_i), //input [4:0] serdes_ln2_txfifo_wrusewd_i
		.serdes_ln2_txfifo_afull_i(serdes_ln2_txfifo_afull_i), //input serdes_ln2_txfifo_afull_i
		.serdes_ln2_txfifo_full_i(serdes_ln2_txfifo_full_i), //input serdes_ln2_txfifo_full_i
		.serdes_ln2_tx_vld_o(serdes_ln2_tx_vld_o), //output serdes_ln2_tx_vld_o
		.ln2_cpll_ok_o(ln2_cpll_ok_o), //output ln2_cpll_ok_o
		.ln2_fabric_rstn_i(ln2_fabric_rstn_i), //input ln2_fabric_rstn_i
		.ln2_rx_rst_i(ln2_rx_rst_i), //input ln2_rx_rst_i
		.ln2_pma_rx_lock_o(ln2_pma_rx_lock_o), //output ln2_pma_rx_lock_o
		.ln2_chbond_start_i(ln2_chbond_start_i), //input ln2_chbond_start_i
		.ln2_align_link_o(ln2_align_link_o), //output ln2_align_link_o
		.ln2_k_lock_o(ln2_k_lock_o), //output ln2_k_lock_o
		.ln2_pcs_rx_clk_o(ln2_pcs_rx_clk_o), //output ln2_pcs_rx_clk_o
		.ln2_fabric_rx_clk_i(ln2_fabric_rx_clk_i), //input ln2_fabric_rx_clk_i
		.ln2_rxfifo_rdusewd_o(ln2_rxfifo_rdusewd_o), //output [4:0] ln2_rxfifo_rdusewd_o
		.ln2_rxfifo_aempty_o(ln2_rxfifo_aempty_o), //output ln2_rxfifo_aempty_o
		.ln2_rxfifo_empty_o(ln2_rxfifo_empty_o), //output ln2_rxfifo_empty_o
		.ln2_rxfifo_rden_i(ln2_rxfifo_rden_i), //input ln2_rxfifo_rden_i
		.ln2_tx_rst_i(ln2_tx_rst_i), //input ln2_tx_rst_i
		.ln2_pcs_tx_clk_o(ln2_pcs_tx_clk_o), //output ln2_pcs_tx_clk_o
		.ln2_fabric_tx_clk_i(ln2_fabric_tx_clk_i), //input ln2_fabric_tx_clk_i
		.ln2_txfifo_wrusewd_o(ln2_txfifo_wrusewd_o), //output [4:0] ln2_txfifo_wrusewd_o
		.ln2_txfifo_afull_o(ln2_txfifo_afull_o), //output ln2_txfifo_afull_o
		.ln2_txfifo_full_o(ln2_txfifo_full_o), //output ln2_txfifo_full_o
		.ln2_tx_vld_i(ln2_tx_vld_i), //input ln2_tx_vld_i
		.ln2_rxdata_o(ln2_rxdata_o), //output [15:0] ln2_rxdata_o
		.ln2_rxk_o(ln2_rxk_o), //output [1:0] ln2_rxk_o
		.ln2_txdata_i(ln2_txdata_i), //input [15:0] ln2_txdata_i
		.ln2_txk_i(ln2_txk_i), //input [1:0] ln2_txk_i
		.serdes_ln3_cpll_ok_i(serdes_ln3_cpll_ok_i), //input serdes_ln3_cpll_ok_i
		.serdes_ln3_fabric_rstn_o(serdes_ln3_fabric_rstn_o), //output serdes_ln3_fabric_rstn_o
		.serdes_ln3_rx_rst_o(serdes_ln3_rx_rst_o), //output serdes_ln3_rx_rst_o
		.serdes_ln3_pma_rx_lock_i(serdes_ln3_pma_rx_lock_i), //input serdes_ln3_pma_rx_lock_i
		.serdes_ln3_chbond_start_o(serdes_ln3_chbond_start_o), //output serdes_ln3_chbond_start_o
		.serdes_ln3_align_link_i(serdes_ln3_align_link_i), //input serdes_ln3_align_link_i
		.serdes_ln3_k_lock_i(serdes_ln3_k_lock_i), //input serdes_ln3_k_lock_i
		.serdes_ln3_pcs_rx_clk_i(serdes_ln3_pcs_rx_clk_i), //input serdes_ln3_pcs_rx_clk_i
		.serdes_ln3_fabric_rx_clk_o(serdes_ln3_fabric_rx_clk_o), //output serdes_ln3_fabric_rx_clk_o
		.serdes_ln3_rxdata_i(serdes_ln3_rxdata_i), //input [87:0] serdes_ln3_rxdata_i
		.serdes_ln3_rxfifo_rdusewd_i(serdes_ln3_rxfifo_rdusewd_i), //input [4:0] serdes_ln3_rxfifo_rdusewd_i
		.serdes_ln3_rxfifo_aempty_i(serdes_ln3_rxfifo_aempty_i), //input serdes_ln3_rxfifo_aempty_i
		.serdes_ln3_rxfifo_empty_i(serdes_ln3_rxfifo_empty_i), //input serdes_ln3_rxfifo_empty_i
		.serdes_ln3_rxfifo_rden_o(serdes_ln3_rxfifo_rden_o), //output serdes_ln3_rxfifo_rden_o
		.serdes_ln3_tx_rst_o(serdes_ln3_tx_rst_o), //output serdes_ln3_tx_rst_o
		.serdes_ln3_pcs_tx_clk_i(serdes_ln3_pcs_tx_clk_i), //input serdes_ln3_pcs_tx_clk_i
		.serdes_ln3_fabric_tx_clk_o(serdes_ln3_fabric_tx_clk_o), //output serdes_ln3_fabric_tx_clk_o
		.serdes_ln3_txdata_o(serdes_ln3_txdata_o), //output [79:0] serdes_ln3_txdata_o
		.serdes_ln3_txfifo_wrusewd_i(serdes_ln3_txfifo_wrusewd_i), //input [4:0] serdes_ln3_txfifo_wrusewd_i
		.serdes_ln3_txfifo_afull_i(serdes_ln3_txfifo_afull_i), //input serdes_ln3_txfifo_afull_i
		.serdes_ln3_txfifo_full_i(serdes_ln3_txfifo_full_i), //input serdes_ln3_txfifo_full_i
		.serdes_ln3_tx_vld_o(serdes_ln3_tx_vld_o), //output serdes_ln3_tx_vld_o
		.ln3_cpll_ok_o(ln3_cpll_ok_o), //output ln3_cpll_ok_o
		.ln3_fabric_rstn_i(ln3_fabric_rstn_i), //input ln3_fabric_rstn_i
		.ln3_rx_rst_i(ln3_rx_rst_i), //input ln3_rx_rst_i
		.ln3_pma_rx_lock_o(ln3_pma_rx_lock_o), //output ln3_pma_rx_lock_o
		.ln3_chbond_start_i(ln3_chbond_start_i), //input ln3_chbond_start_i
		.ln3_align_link_o(ln3_align_link_o), //output ln3_align_link_o
		.ln3_k_lock_o(ln3_k_lock_o), //output ln3_k_lock_o
		.ln3_pcs_rx_clk_o(ln3_pcs_rx_clk_o), //output ln3_pcs_rx_clk_o
		.ln3_fabric_rx_clk_i(ln3_fabric_rx_clk_i), //input ln3_fabric_rx_clk_i
		.ln3_rxfifo_rdusewd_o(ln3_rxfifo_rdusewd_o), //output [4:0] ln3_rxfifo_rdusewd_o
		.ln3_rxfifo_aempty_o(ln3_rxfifo_aempty_o), //output ln3_rxfifo_aempty_o
		.ln3_rxfifo_empty_o(ln3_rxfifo_empty_o), //output ln3_rxfifo_empty_o
		.ln3_rxfifo_rden_i(ln3_rxfifo_rden_i), //input ln3_rxfifo_rden_i
		.ln3_tx_rst_i(ln3_tx_rst_i), //input ln3_tx_rst_i
		.ln3_pcs_tx_clk_o(ln3_pcs_tx_clk_o), //output ln3_pcs_tx_clk_o
		.ln3_fabric_tx_clk_i(ln3_fabric_tx_clk_i), //input ln3_fabric_tx_clk_i
		.ln3_txfifo_wrusewd_o(ln3_txfifo_wrusewd_o), //output [4:0] ln3_txfifo_wrusewd_o
		.ln3_txfifo_afull_o(ln3_txfifo_afull_o), //output ln3_txfifo_afull_o
		.ln3_txfifo_full_o(ln3_txfifo_full_o), //output ln3_txfifo_full_o
		.ln3_tx_vld_i(ln3_tx_vld_i), //input ln3_tx_vld_i
		.ln3_rxdata_o(ln3_rxdata_o), //output [15:0] ln3_rxdata_o
		.ln3_rxk_o(ln3_rxk_o), //output [1:0] ln3_rxk_o
		.ln3_txdata_i(ln3_txdata_i), //input [15:0] ln3_txdata_i
		.ln3_txk_i(ln3_txk_i), //input [1:0] ln3_txk_i
		.drp_clk_o(drp_clk_o), //output drp_clk_o
		.drp_addr_i(drp_addr_i), //input [23:0] drp_addr_i
		.drp_wren_i(drp_wren_i), //input drp_wren_i
		.drp_wrdata_i(drp_wrdata_i), //input [31:0] drp_wrdata_i
		.drp_strb_i(drp_strb_i), //input [7:0] drp_strb_i
		.drp_ready_o(drp_ready_o), //output drp_ready_o
		.drp_rden_i(drp_rden_i), //input drp_rden_i
		.drp_rdvld_o(drp_rdvld_o), //output drp_rdvld_o
		.drp_rddata_o(drp_rddata_o), //output [31:0] drp_rddata_o
		.drp_resp_o(drp_resp_o), //output drp_resp_o
		.drp_clk_i(drp_clk_i), //input drp_clk_i
		.drp_addr_o(drp_addr_o), //output [23:0] drp_addr_o
		.drp_wren_o(drp_wren_o), //output drp_wren_o
		.drp_wrdata_o(drp_wrdata_o), //output [31:0] drp_wrdata_o
		.drp_strb_o(drp_strb_o), //output [7:0] drp_strb_o
		.drp_ready_i(drp_ready_i), //input drp_ready_i
		.drp_rden_o(drp_rden_o), //output drp_rden_o
		.drp_rdvld_i(drp_rdvld_i), //input drp_rdvld_i
		.drp_rddata_i(drp_rddata_i), //input [31:0] drp_rddata_i
		.drp_resp_i(drp_resp_i), //input drp_resp_i
		.serdes_qpll0_ok_i(serdes_qpll0_ok_i), //input serdes_qpll0_ok_i
		.serdes_qpll1_ok_i(serdes_qpll1_ok_i), //input serdes_qpll1_ok_i
		.qpll0_ok_o(qpll0_ok_o), //output qpll0_ok_o
		.qpll1_ok_o(qpll1_ok_o) //output qpll1_ok_o
	);

//--------Copy end-------------------
