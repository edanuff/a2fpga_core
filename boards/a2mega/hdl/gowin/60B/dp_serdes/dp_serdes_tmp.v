//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.01
//IP Version: 1.0
//Part Number: GW5AT-LV60PG484AC1/I0
//Device: GW5AT-60
//Device Version: B
//Created Time: Sun Aug 16 08:33:31 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    dp_serdes your_instance_name(
        .edp_phy_ln2_cpll_ok_o(edp_phy_ln2_cpll_ok_o), //output edp_phy_ln2_cpll_ok_o
        .edp_phy_ln2_pma_rx_lock_o(edp_phy_ln2_pma_rx_lock_o), //output edp_phy_ln2_pma_rx_lock_o
        .edp_phy_ln2_align_link_o(edp_phy_ln2_align_link_o), //output edp_phy_ln2_align_link_o
        .edp_phy_ln2_k_lock_o(edp_phy_ln2_k_lock_o), //output edp_phy_ln2_k_lock_o
        .edp_phy_ln2_pcs_rx_clk_o(edp_phy_ln2_pcs_rx_clk_o), //output edp_phy_ln2_pcs_rx_clk_o
        .edp_phy_ln2_rxfifo_aempty_o(edp_phy_ln2_rxfifo_aempty_o), //output edp_phy_ln2_rxfifo_aempty_o
        .edp_phy_ln2_pcs_tx_clk_o(edp_phy_ln2_pcs_tx_clk_o), //output edp_phy_ln2_pcs_tx_clk_o
        .edp_phy_ln2_txfifo_afull_o(edp_phy_ln2_txfifo_afull_o), //output edp_phy_ln2_txfifo_afull_o
        .edp_phy_ln2_rxdata_o(edp_phy_ln2_rxdata_o), //output [15:0] edp_phy_ln2_rxdata_o
        .edp_phy_ln2_rxk_o(edp_phy_ln2_rxk_o), //output [1:0] edp_phy_ln2_rxk_o
        .edp_phy_ln3_cpll_ok_o(edp_phy_ln3_cpll_ok_o), //output edp_phy_ln3_cpll_ok_o
        .edp_phy_ln3_pma_rx_lock_o(edp_phy_ln3_pma_rx_lock_o), //output edp_phy_ln3_pma_rx_lock_o
        .edp_phy_ln3_align_link_o(edp_phy_ln3_align_link_o), //output edp_phy_ln3_align_link_o
        .edp_phy_ln3_k_lock_o(edp_phy_ln3_k_lock_o), //output edp_phy_ln3_k_lock_o
        .edp_phy_ln3_pcs_rx_clk_o(edp_phy_ln3_pcs_rx_clk_o), //output edp_phy_ln3_pcs_rx_clk_o
        .edp_phy_ln3_rxfifo_aempty_o(edp_phy_ln3_rxfifo_aempty_o), //output edp_phy_ln3_rxfifo_aempty_o
        .edp_phy_ln3_pcs_tx_clk_o(edp_phy_ln3_pcs_tx_clk_o), //output edp_phy_ln3_pcs_tx_clk_o
        .edp_phy_ln3_txfifo_afull_o(edp_phy_ln3_txfifo_afull_o), //output edp_phy_ln3_txfifo_afull_o
        .edp_phy_ln3_rxdata_o(edp_phy_ln3_rxdata_o), //output [15:0] edp_phy_ln3_rxdata_o
        .edp_phy_ln3_rxk_o(edp_phy_ln3_rxk_o), //output [1:0] edp_phy_ln3_rxk_o
        .edp_phy_qpll0_ok_o(edp_phy_qpll0_ok_o), //output edp_phy_qpll0_ok_o
        .edp_phy_qpll1_ok_o(edp_phy_qpll1_ok_o), //output edp_phy_qpll1_ok_o
        .edp_phy_ln2_rxfifo_rdusewd_o(edp_phy_ln2_rxfifo_rdusewd_o), //output [4:0] edp_phy_ln2_rxfifo_rdusewd_o
        .edp_phy_ln2_rxfifo_empty_o(edp_phy_ln2_rxfifo_empty_o), //output edp_phy_ln2_rxfifo_empty_o
        .edp_phy_ln2_txfifo_wrusewd_o(edp_phy_ln2_txfifo_wrusewd_o), //output [4:0] edp_phy_ln2_txfifo_wrusewd_o
        .edp_phy_ln2_txfifo_full_o(edp_phy_ln2_txfifo_full_o), //output edp_phy_ln2_txfifo_full_o
        .edp_phy_ln3_rxfifo_rdusewd_o(edp_phy_ln3_rxfifo_rdusewd_o), //output [4:0] edp_phy_ln3_rxfifo_rdusewd_o
        .edp_phy_ln3_rxfifo_empty_o(edp_phy_ln3_rxfifo_empty_o), //output edp_phy_ln3_rxfifo_empty_o
        .edp_phy_ln3_txfifo_wrusewd_o(edp_phy_ln3_txfifo_wrusewd_o), //output [4:0] edp_phy_ln3_txfifo_wrusewd_o
        .edp_phy_ln3_txfifo_full_o(edp_phy_ln3_txfifo_full_o), //output edp_phy_ln3_txfifo_full_o
        .edp_phy_drp_clk_o(edp_phy_drp_clk_o), //output edp_phy_drp_clk_o
        .edp_phy_drp_ready_o(edp_phy_drp_ready_o), //output edp_phy_drp_ready_o
        .edp_phy_drp_rdvld_o(edp_phy_drp_rdvld_o), //output edp_phy_drp_rdvld_o
        .edp_phy_drp_rddata_o(edp_phy_drp_rddata_o), //output [31:0] edp_phy_drp_rddata_o
        .edp_phy_drp_resp_o(edp_phy_drp_resp_o), //output edp_phy_drp_resp_o
        .por_n_i(por_n_i), //input por_n_i
        .edp_phy_ln2_fabric_rstn_i(edp_phy_ln2_fabric_rstn_i), //input edp_phy_ln2_fabric_rstn_i
        .edp_phy_ln2_rx_rst_i(edp_phy_ln2_rx_rst_i), //input edp_phy_ln2_rx_rst_i
        .edp_phy_ln2_fabric_rx_clk_i(edp_phy_ln2_fabric_rx_clk_i), //input edp_phy_ln2_fabric_rx_clk_i
        .edp_phy_ln2_rxfifo_rden_i(edp_phy_ln2_rxfifo_rden_i), //input edp_phy_ln2_rxfifo_rden_i
        .edp_phy_ln2_tx_rst_i(edp_phy_ln2_tx_rst_i), //input edp_phy_ln2_tx_rst_i
        .edp_phy_ln2_fabric_tx_clk_i(edp_phy_ln2_fabric_tx_clk_i), //input edp_phy_ln2_fabric_tx_clk_i
        .edp_phy_ln2_tx_vld_i(edp_phy_ln2_tx_vld_i), //input edp_phy_ln2_tx_vld_i
        .edp_phy_ln2_txdata_i(edp_phy_ln2_txdata_i), //input [15:0] edp_phy_ln2_txdata_i
        .edp_phy_ln2_txk_i(edp_phy_ln2_txk_i), //input [1:0] edp_phy_ln2_txk_i
        .edp_phy_ln3_fabric_rstn_i(edp_phy_ln3_fabric_rstn_i), //input edp_phy_ln3_fabric_rstn_i
        .edp_phy_ln3_rx_rst_i(edp_phy_ln3_rx_rst_i), //input edp_phy_ln3_rx_rst_i
        .edp_phy_ln3_fabric_rx_clk_i(edp_phy_ln3_fabric_rx_clk_i), //input edp_phy_ln3_fabric_rx_clk_i
        .edp_phy_ln3_rxfifo_rden_i(edp_phy_ln3_rxfifo_rden_i), //input edp_phy_ln3_rxfifo_rden_i
        .edp_phy_ln3_tx_rst_i(edp_phy_ln3_tx_rst_i), //input edp_phy_ln3_tx_rst_i
        .edp_phy_ln3_fabric_tx_clk_i(edp_phy_ln3_fabric_tx_clk_i), //input edp_phy_ln3_fabric_tx_clk_i
        .edp_phy_ln3_tx_vld_i(edp_phy_ln3_tx_vld_i), //input edp_phy_ln3_tx_vld_i
        .edp_phy_ln3_txdata_i(edp_phy_ln3_txdata_i), //input [15:0] edp_phy_ln3_txdata_i
        .edp_phy_ln3_txk_i(edp_phy_ln3_txk_i), //input [1:0] edp_phy_ln3_txk_i
        .edp_phy_ln2_chbond_start_i(edp_phy_ln2_chbond_start_i), //input edp_phy_ln2_chbond_start_i
        .edp_phy_ln3_chbond_start_i(edp_phy_ln3_chbond_start_i), //input edp_phy_ln3_chbond_start_i
        .edp_phy_drp_addr_i(edp_phy_drp_addr_i), //input [23:0] edp_phy_drp_addr_i
        .edp_phy_drp_wren_i(edp_phy_drp_wren_i), //input edp_phy_drp_wren_i
        .edp_phy_drp_wrdata_i(edp_phy_drp_wrdata_i), //input [31:0] edp_phy_drp_wrdata_i
        .edp_phy_drp_strb_i(edp_phy_drp_strb_i), //input [7:0] edp_phy_drp_strb_i
        .edp_phy_drp_rden_i(edp_phy_drp_rden_i) //input edp_phy_drp_rden_i
    );

//--------Copy end-------------------
