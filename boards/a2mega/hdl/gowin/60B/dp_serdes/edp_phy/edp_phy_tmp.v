//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.01
//Part Number: GW5AT-LV60PG484AC1/I0
//Device: GW5AT-60
//Device Version: B
//Created Time: Sat Aug 15 22:50:22 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

	edp_phy your_instance_name(
		.serdes_ln0_cpll_ok_i(serdes_ln0_cpll_ok_i), //input serdes_ln0_cpll_ok_i
		.serdes_ln0_fabric_rstn_o(serdes_ln0_fabric_rstn_o), //output serdes_ln0_fabric_rstn_o
		.serdes_ln0_rx_rst_o(serdes_ln0_rx_rst_o), //output serdes_ln0_rx_rst_o
		.serdes_ln0_pma_rx_lock_i(serdes_ln0_pma_rx_lock_i), //input serdes_ln0_pma_rx_lock_i
		.serdes_ln0_chbond_start_o(serdes_ln0_chbond_start_o), //output serdes_ln0_chbond_start_o
		.serdes_ln0_align_link_i(serdes_ln0_align_link_i), //input serdes_ln0_align_link_i
		.serdes_ln0_k_lock_i(serdes_ln0_k_lock_i), //input serdes_ln0_k_lock_i
		.serdes_ln0_pcs_rx_clk_i(serdes_ln0_pcs_rx_clk_i), //input serdes_ln0_pcs_rx_clk_i
		.serdes_ln0_fabric_rx_clk_o(serdes_ln0_fabric_rx_clk_o), //output serdes_ln0_fabric_rx_clk_o
		.serdes_ln0_rxdata_i(serdes_ln0_rxdata_i), //input [87:0] serdes_ln0_rxdata_i
		.serdes_ln0_rxfifo_rdusewd_i(serdes_ln0_rxfifo_rdusewd_i), //input [4:0] serdes_ln0_rxfifo_rdusewd_i
		.serdes_ln0_rxfifo_aempty_i(serdes_ln0_rxfifo_aempty_i), //input serdes_ln0_rxfifo_aempty_i
		.serdes_ln0_rxfifo_empty_i(serdes_ln0_rxfifo_empty_i), //input serdes_ln0_rxfifo_empty_i
		.serdes_ln0_rxfifo_rden_o(serdes_ln0_rxfifo_rden_o), //output serdes_ln0_rxfifo_rden_o
		.serdes_ln0_tx_rst_o(serdes_ln0_tx_rst_o), //output serdes_ln0_tx_rst_o
		.serdes_ln0_pcs_tx_clk_i(serdes_ln0_pcs_tx_clk_i), //input serdes_ln0_pcs_tx_clk_i
		.serdes_ln0_fabric_tx_clk_o(serdes_ln0_fabric_tx_clk_o), //output serdes_ln0_fabric_tx_clk_o
		.serdes_ln0_txdata_o(serdes_ln0_txdata_o), //output [79:0] serdes_ln0_txdata_o
		.serdes_ln0_txfifo_wrusewd_i(serdes_ln0_txfifo_wrusewd_i), //input [4:0] serdes_ln0_txfifo_wrusewd_i
		.serdes_ln0_txfifo_afull_i(serdes_ln0_txfifo_afull_i), //input serdes_ln0_txfifo_afull_i
		.serdes_ln0_txfifo_full_i(serdes_ln0_txfifo_full_i), //input serdes_ln0_txfifo_full_i
		.serdes_ln0_tx_vld_o(serdes_ln0_tx_vld_o), //output serdes_ln0_tx_vld_o
		.ln0_cpll_ok_o(ln0_cpll_ok_o), //output ln0_cpll_ok_o
		.ln0_fabric_rstn_i(ln0_fabric_rstn_i), //input ln0_fabric_rstn_i
		.ln0_rx_rst_i(ln0_rx_rst_i), //input ln0_rx_rst_i
		.ln0_pma_rx_lock_o(ln0_pma_rx_lock_o), //output ln0_pma_rx_lock_o
		.ln0_chbond_start_i(ln0_chbond_start_i), //input ln0_chbond_start_i
		.ln0_align_link_o(ln0_align_link_o), //output ln0_align_link_o
		.ln0_k_lock_o(ln0_k_lock_o), //output ln0_k_lock_o
		.ln0_pcs_rx_clk_o(ln0_pcs_rx_clk_o), //output ln0_pcs_rx_clk_o
		.ln0_fabric_rx_clk_i(ln0_fabric_rx_clk_i), //input ln0_fabric_rx_clk_i
		.ln0_rxfifo_rdusewd_o(ln0_rxfifo_rdusewd_o), //output [4:0] ln0_rxfifo_rdusewd_o
		.ln0_rxfifo_aempty_o(ln0_rxfifo_aempty_o), //output ln0_rxfifo_aempty_o
		.ln0_rxfifo_empty_o(ln0_rxfifo_empty_o), //output ln0_rxfifo_empty_o
		.ln0_rxfifo_rden_i(ln0_rxfifo_rden_i), //input ln0_rxfifo_rden_i
		.ln0_tx_rst_i(ln0_tx_rst_i), //input ln0_tx_rst_i
		.ln0_pcs_tx_clk_o(ln0_pcs_tx_clk_o), //output ln0_pcs_tx_clk_o
		.ln0_fabric_tx_clk_i(ln0_fabric_tx_clk_i), //input ln0_fabric_tx_clk_i
		.ln0_txfifo_wrusewd_o(ln0_txfifo_wrusewd_o), //output [4:0] ln0_txfifo_wrusewd_o
		.ln0_txfifo_afull_o(ln0_txfifo_afull_o), //output ln0_txfifo_afull_o
		.ln0_txfifo_full_o(ln0_txfifo_full_o), //output ln0_txfifo_full_o
		.ln0_tx_vld_i(ln0_tx_vld_i), //input ln0_tx_vld_i
		.ln0_rxdata_o(ln0_rxdata_o), //output [15:0] ln0_rxdata_o
		.ln0_rxk_o(ln0_rxk_o), //output [1:0] ln0_rxk_o
		.ln0_txdata_i(ln0_txdata_i), //input [15:0] ln0_txdata_i
		.ln0_txk_i(ln0_txk_i), //input [1:0] ln0_txk_i
		.serdes_ln1_cpll_ok_i(serdes_ln1_cpll_ok_i), //input serdes_ln1_cpll_ok_i
		.serdes_ln1_fabric_rstn_o(serdes_ln1_fabric_rstn_o), //output serdes_ln1_fabric_rstn_o
		.serdes_ln1_rx_rst_o(serdes_ln1_rx_rst_o), //output serdes_ln1_rx_rst_o
		.serdes_ln1_pma_rx_lock_i(serdes_ln1_pma_rx_lock_i), //input serdes_ln1_pma_rx_lock_i
		.serdes_ln1_chbond_start_o(serdes_ln1_chbond_start_o), //output serdes_ln1_chbond_start_o
		.serdes_ln1_align_link_i(serdes_ln1_align_link_i), //input serdes_ln1_align_link_i
		.serdes_ln1_k_lock_i(serdes_ln1_k_lock_i), //input serdes_ln1_k_lock_i
		.serdes_ln1_pcs_rx_clk_i(serdes_ln1_pcs_rx_clk_i), //input serdes_ln1_pcs_rx_clk_i
		.serdes_ln1_fabric_rx_clk_o(serdes_ln1_fabric_rx_clk_o), //output serdes_ln1_fabric_rx_clk_o
		.serdes_ln1_rxdata_i(serdes_ln1_rxdata_i), //input [87:0] serdes_ln1_rxdata_i
		.serdes_ln1_rxfifo_rdusewd_i(serdes_ln1_rxfifo_rdusewd_i), //input [4:0] serdes_ln1_rxfifo_rdusewd_i
		.serdes_ln1_rxfifo_aempty_i(serdes_ln1_rxfifo_aempty_i), //input serdes_ln1_rxfifo_aempty_i
		.serdes_ln1_rxfifo_empty_i(serdes_ln1_rxfifo_empty_i), //input serdes_ln1_rxfifo_empty_i
		.serdes_ln1_rxfifo_rden_o(serdes_ln1_rxfifo_rden_o), //output serdes_ln1_rxfifo_rden_o
		.serdes_ln1_tx_rst_o(serdes_ln1_tx_rst_o), //output serdes_ln1_tx_rst_o
		.serdes_ln1_pcs_tx_clk_i(serdes_ln1_pcs_tx_clk_i), //input serdes_ln1_pcs_tx_clk_i
		.serdes_ln1_fabric_tx_clk_o(serdes_ln1_fabric_tx_clk_o), //output serdes_ln1_fabric_tx_clk_o
		.serdes_ln1_txdata_o(serdes_ln1_txdata_o), //output [79:0] serdes_ln1_txdata_o
		.serdes_ln1_txfifo_wrusewd_i(serdes_ln1_txfifo_wrusewd_i), //input [4:0] serdes_ln1_txfifo_wrusewd_i
		.serdes_ln1_txfifo_afull_i(serdes_ln1_txfifo_afull_i), //input serdes_ln1_txfifo_afull_i
		.serdes_ln1_txfifo_full_i(serdes_ln1_txfifo_full_i), //input serdes_ln1_txfifo_full_i
		.serdes_ln1_tx_vld_o(serdes_ln1_tx_vld_o), //output serdes_ln1_tx_vld_o
		.ln1_cpll_ok_o(ln1_cpll_ok_o), //output ln1_cpll_ok_o
		.ln1_fabric_rstn_i(ln1_fabric_rstn_i), //input ln1_fabric_rstn_i
		.ln1_rx_rst_i(ln1_rx_rst_i), //input ln1_rx_rst_i
		.ln1_pma_rx_lock_o(ln1_pma_rx_lock_o), //output ln1_pma_rx_lock_o
		.ln1_chbond_start_i(ln1_chbond_start_i), //input ln1_chbond_start_i
		.ln1_align_link_o(ln1_align_link_o), //output ln1_align_link_o
		.ln1_k_lock_o(ln1_k_lock_o), //output ln1_k_lock_o
		.ln1_pcs_rx_clk_o(ln1_pcs_rx_clk_o), //output ln1_pcs_rx_clk_o
		.ln1_fabric_rx_clk_i(ln1_fabric_rx_clk_i), //input ln1_fabric_rx_clk_i
		.ln1_rxfifo_rdusewd_o(ln1_rxfifo_rdusewd_o), //output [4:0] ln1_rxfifo_rdusewd_o
		.ln1_rxfifo_aempty_o(ln1_rxfifo_aempty_o), //output ln1_rxfifo_aempty_o
		.ln1_rxfifo_empty_o(ln1_rxfifo_empty_o), //output ln1_rxfifo_empty_o
		.ln1_rxfifo_rden_i(ln1_rxfifo_rden_i), //input ln1_rxfifo_rden_i
		.ln1_tx_rst_i(ln1_tx_rst_i), //input ln1_tx_rst_i
		.ln1_pcs_tx_clk_o(ln1_pcs_tx_clk_o), //output ln1_pcs_tx_clk_o
		.ln1_fabric_tx_clk_i(ln1_fabric_tx_clk_i), //input ln1_fabric_tx_clk_i
		.ln1_txfifo_wrusewd_o(ln1_txfifo_wrusewd_o), //output [4:0] ln1_txfifo_wrusewd_o
		.ln1_txfifo_afull_o(ln1_txfifo_afull_o), //output ln1_txfifo_afull_o
		.ln1_txfifo_full_o(ln1_txfifo_full_o), //output ln1_txfifo_full_o
		.ln1_tx_vld_i(ln1_tx_vld_i), //input ln1_tx_vld_i
		.ln1_rxdata_o(ln1_rxdata_o), //output [15:0] ln1_rxdata_o
		.ln1_rxk_o(ln1_rxk_o), //output [1:0] ln1_rxk_o
		.ln1_txdata_i(ln1_txdata_i), //input [15:0] ln1_txdata_i
		.ln1_txk_i(ln1_txk_i), //input [1:0] ln1_txk_i
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
