//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.01
//IP Version: 1.0
//Part Number: GW5AT-LV60PG484AC1/I0
//Device: GW5AT-60
//Device Version: B
//Created Time: Sat Aug 15 22:50:22 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    dp_serdes your_instance_name(
        .dp_phy_q0_ln2_rx_pcs_clkout_o(dp_phy_q0_ln2_rx_pcs_clkout_o), //output dp_phy_q0_ln2_rx_pcs_clkout_o
        .dp_phy_q0_ln2_rx_data_o(dp_phy_q0_ln2_rx_data_o), //output [87:0] dp_phy_q0_ln2_rx_data_o
        .dp_phy_q0_ln2_rx_fifo_rdusewd_o(dp_phy_q0_ln2_rx_fifo_rdusewd_o), //output [4:0] dp_phy_q0_ln2_rx_fifo_rdusewd_o
        .dp_phy_q0_ln2_rx_fifo_aempty_o(dp_phy_q0_ln2_rx_fifo_aempty_o), //output dp_phy_q0_ln2_rx_fifo_aempty_o
        .dp_phy_q0_ln2_rx_fifo_empty_o(dp_phy_q0_ln2_rx_fifo_empty_o), //output dp_phy_q0_ln2_rx_fifo_empty_o
        .dp_phy_q0_ln2_rx_valid_o(dp_phy_q0_ln2_rx_valid_o), //output dp_phy_q0_ln2_rx_valid_o
        .dp_phy_q0_ln2_tx_pcs_clkout_o(dp_phy_q0_ln2_tx_pcs_clkout_o), //output dp_phy_q0_ln2_tx_pcs_clkout_o
        .dp_phy_q0_ln2_tx_fifo_wrusewd_o(dp_phy_q0_ln2_tx_fifo_wrusewd_o), //output [4:0] dp_phy_q0_ln2_tx_fifo_wrusewd_o
        .dp_phy_q0_ln2_tx_fifo_afull_o(dp_phy_q0_ln2_tx_fifo_afull_o), //output dp_phy_q0_ln2_tx_fifo_afull_o
        .dp_phy_q0_ln2_tx_fifo_full_o(dp_phy_q0_ln2_tx_fifo_full_o), //output dp_phy_q0_ln2_tx_fifo_full_o
        .dp_phy_q0_ln2_refclk_o(dp_phy_q0_ln2_refclk_o), //output dp_phy_q0_ln2_refclk_o
        .dp_phy_q0_ln2_signal_detect_o(dp_phy_q0_ln2_signal_detect_o), //output dp_phy_q0_ln2_signal_detect_o
        .dp_phy_q0_ln2_rx_cdr_lock_o(dp_phy_q0_ln2_rx_cdr_lock_o), //output dp_phy_q0_ln2_rx_cdr_lock_o
        .dp_phy_q0_ln2_pll_lock_o(dp_phy_q0_ln2_pll_lock_o), //output dp_phy_q0_ln2_pll_lock_o
        .dp_phy_q0_ln2_ready_o(dp_phy_q0_ln2_ready_o), //output dp_phy_q0_ln2_ready_o
        .dp_phy_q0_ln3_rx_pcs_clkout_o(dp_phy_q0_ln3_rx_pcs_clkout_o), //output dp_phy_q0_ln3_rx_pcs_clkout_o
        .dp_phy_q0_ln3_rx_data_o(dp_phy_q0_ln3_rx_data_o), //output [87:0] dp_phy_q0_ln3_rx_data_o
        .dp_phy_q0_ln3_rx_fifo_rdusewd_o(dp_phy_q0_ln3_rx_fifo_rdusewd_o), //output [4:0] dp_phy_q0_ln3_rx_fifo_rdusewd_o
        .dp_phy_q0_ln3_rx_fifo_aempty_o(dp_phy_q0_ln3_rx_fifo_aempty_o), //output dp_phy_q0_ln3_rx_fifo_aempty_o
        .dp_phy_q0_ln3_rx_fifo_empty_o(dp_phy_q0_ln3_rx_fifo_empty_o), //output dp_phy_q0_ln3_rx_fifo_empty_o
        .dp_phy_q0_ln3_rx_valid_o(dp_phy_q0_ln3_rx_valid_o), //output dp_phy_q0_ln3_rx_valid_o
        .dp_phy_q0_ln3_tx_pcs_clkout_o(dp_phy_q0_ln3_tx_pcs_clkout_o), //output dp_phy_q0_ln3_tx_pcs_clkout_o
        .dp_phy_q0_ln3_tx_fifo_wrusewd_o(dp_phy_q0_ln3_tx_fifo_wrusewd_o), //output [4:0] dp_phy_q0_ln3_tx_fifo_wrusewd_o
        .dp_phy_q0_ln3_tx_fifo_afull_o(dp_phy_q0_ln3_tx_fifo_afull_o), //output dp_phy_q0_ln3_tx_fifo_afull_o
        .dp_phy_q0_ln3_tx_fifo_full_o(dp_phy_q0_ln3_tx_fifo_full_o), //output dp_phy_q0_ln3_tx_fifo_full_o
        .dp_phy_q0_ln3_refclk_o(dp_phy_q0_ln3_refclk_o), //output dp_phy_q0_ln3_refclk_o
        .dp_phy_q0_ln3_signal_detect_o(dp_phy_q0_ln3_signal_detect_o), //output dp_phy_q0_ln3_signal_detect_o
        .dp_phy_q0_ln3_rx_cdr_lock_o(dp_phy_q0_ln3_rx_cdr_lock_o), //output dp_phy_q0_ln3_rx_cdr_lock_o
        .dp_phy_q0_ln3_pll_lock_o(dp_phy_q0_ln3_pll_lock_o), //output dp_phy_q0_ln3_pll_lock_o
        .dp_phy_q0_ln3_ready_o(dp_phy_q0_ln3_ready_o), //output dp_phy_q0_ln3_ready_o
        .dp_phy_drp_clk_o(dp_phy_drp_clk_o), //output dp_phy_drp_clk_o
        .dp_phy_drp_ready_o(dp_phy_drp_ready_o), //output dp_phy_drp_ready_o
        .dp_phy_drp_rdvld_o(dp_phy_drp_rdvld_o), //output dp_phy_drp_rdvld_o
        .dp_phy_drp_rddata_o(dp_phy_drp_rddata_o), //output [31:0] dp_phy_drp_rddata_o
        .dp_phy_drp_resp_o(dp_phy_drp_resp_o), //output dp_phy_drp_resp_o
        .edp_phy_ln0_cpll_ok_o(edp_phy_ln0_cpll_ok_o), //output edp_phy_ln0_cpll_ok_o
        .edp_phy_ln0_pma_rx_lock_o(edp_phy_ln0_pma_rx_lock_o), //output edp_phy_ln0_pma_rx_lock_o
        .edp_phy_ln0_align_link_o(edp_phy_ln0_align_link_o), //output edp_phy_ln0_align_link_o
        .edp_phy_ln0_k_lock_o(edp_phy_ln0_k_lock_o), //output edp_phy_ln0_k_lock_o
        .edp_phy_ln0_pcs_rx_clk_o(edp_phy_ln0_pcs_rx_clk_o), //output edp_phy_ln0_pcs_rx_clk_o
        .edp_phy_ln0_rxfifo_aempty_o(edp_phy_ln0_rxfifo_aempty_o), //output edp_phy_ln0_rxfifo_aempty_o
        .edp_phy_ln0_pcs_tx_clk_o(edp_phy_ln0_pcs_tx_clk_o), //output edp_phy_ln0_pcs_tx_clk_o
        .edp_phy_ln0_txfifo_afull_o(edp_phy_ln0_txfifo_afull_o), //output edp_phy_ln0_txfifo_afull_o
        .edp_phy_ln0_rxdata_o(edp_phy_ln0_rxdata_o), //output [15:0] edp_phy_ln0_rxdata_o
        .edp_phy_ln0_rxk_o(edp_phy_ln0_rxk_o), //output [1:0] edp_phy_ln0_rxk_o
        .edp_phy_ln1_cpll_ok_o(edp_phy_ln1_cpll_ok_o), //output edp_phy_ln1_cpll_ok_o
        .edp_phy_ln1_pma_rx_lock_o(edp_phy_ln1_pma_rx_lock_o), //output edp_phy_ln1_pma_rx_lock_o
        .edp_phy_ln1_align_link_o(edp_phy_ln1_align_link_o), //output edp_phy_ln1_align_link_o
        .edp_phy_ln1_k_lock_o(edp_phy_ln1_k_lock_o), //output edp_phy_ln1_k_lock_o
        .edp_phy_ln1_pcs_rx_clk_o(edp_phy_ln1_pcs_rx_clk_o), //output edp_phy_ln1_pcs_rx_clk_o
        .edp_phy_ln1_rxfifo_aempty_o(edp_phy_ln1_rxfifo_aempty_o), //output edp_phy_ln1_rxfifo_aempty_o
        .edp_phy_ln1_pcs_tx_clk_o(edp_phy_ln1_pcs_tx_clk_o), //output edp_phy_ln1_pcs_tx_clk_o
        .edp_phy_ln1_txfifo_afull_o(edp_phy_ln1_txfifo_afull_o), //output edp_phy_ln1_txfifo_afull_o
        .edp_phy_ln1_rxdata_o(edp_phy_ln1_rxdata_o), //output [15:0] edp_phy_ln1_rxdata_o
        .edp_phy_ln1_rxk_o(edp_phy_ln1_rxk_o), //output [1:0] edp_phy_ln1_rxk_o
        .edp_phy_qpll0_ok_o(edp_phy_qpll0_ok_o), //output edp_phy_qpll0_ok_o
        .edp_phy_qpll1_ok_o(edp_phy_qpll1_ok_o), //output edp_phy_qpll1_ok_o
        .edp_phy_ln0_rxfifo_rdusewd_o(edp_phy_ln0_rxfifo_rdusewd_o), //output [4:0] edp_phy_ln0_rxfifo_rdusewd_o
        .edp_phy_ln0_rxfifo_empty_o(edp_phy_ln0_rxfifo_empty_o), //output edp_phy_ln0_rxfifo_empty_o
        .edp_phy_ln0_txfifo_wrusewd_o(edp_phy_ln0_txfifo_wrusewd_o), //output [4:0] edp_phy_ln0_txfifo_wrusewd_o
        .edp_phy_ln0_txfifo_full_o(edp_phy_ln0_txfifo_full_o), //output edp_phy_ln0_txfifo_full_o
        .edp_phy_ln1_rxfifo_rdusewd_o(edp_phy_ln1_rxfifo_rdusewd_o), //output [4:0] edp_phy_ln1_rxfifo_rdusewd_o
        .edp_phy_ln1_rxfifo_empty_o(edp_phy_ln1_rxfifo_empty_o), //output edp_phy_ln1_rxfifo_empty_o
        .edp_phy_ln1_txfifo_wrusewd_o(edp_phy_ln1_txfifo_wrusewd_o), //output [4:0] edp_phy_ln1_txfifo_wrusewd_o
        .edp_phy_ln1_txfifo_full_o(edp_phy_ln1_txfifo_full_o), //output edp_phy_ln1_txfifo_full_o
        .edp_phy_drp_clk_o(edp_phy_drp_clk_o), //output edp_phy_drp_clk_o
        .edp_phy_drp_ready_o(edp_phy_drp_ready_o), //output edp_phy_drp_ready_o
        .edp_phy_drp_rdvld_o(edp_phy_drp_rdvld_o), //output edp_phy_drp_rdvld_o
        .edp_phy_drp_rddata_o(edp_phy_drp_rddata_o), //output [31:0] edp_phy_drp_rddata_o
        .edp_phy_drp_resp_o(edp_phy_drp_resp_o), //output edp_phy_drp_resp_o
        .por_n_i(por_n_i), //input por_n_i
        .dp_phy_q0_ln2_rx_clk_i(dp_phy_q0_ln2_rx_clk_i), //input dp_phy_q0_ln2_rx_clk_i
        .dp_phy_q0_ln2_rx_fifo_rden_i(dp_phy_q0_ln2_rx_fifo_rden_i), //input dp_phy_q0_ln2_rx_fifo_rden_i
        .dp_phy_q0_ln2_tx_clk_i(dp_phy_q0_ln2_tx_clk_i), //input dp_phy_q0_ln2_tx_clk_i
        .dp_phy_q0_ln2_tx_data_i(dp_phy_q0_ln2_tx_data_i), //input [79:0] dp_phy_q0_ln2_tx_data_i
        .dp_phy_q0_ln2_tx_fifo_wren_i(dp_phy_q0_ln2_tx_fifo_wren_i), //input dp_phy_q0_ln2_tx_fifo_wren_i
        .dp_phy_q0_ln2_pma_rstn_i(dp_phy_q0_ln2_pma_rstn_i), //input dp_phy_q0_ln2_pma_rstn_i
        .dp_phy_q0_ln2_pcs_rx_rst_i(dp_phy_q0_ln2_pcs_rx_rst_i), //input dp_phy_q0_ln2_pcs_rx_rst_i
        .dp_phy_q0_ln2_pcs_tx_rst_i(dp_phy_q0_ln2_pcs_tx_rst_i), //input dp_phy_q0_ln2_pcs_tx_rst_i
        .dp_phy_q0_ln3_rx_clk_i(dp_phy_q0_ln3_rx_clk_i), //input dp_phy_q0_ln3_rx_clk_i
        .dp_phy_q0_ln3_rx_fifo_rden_i(dp_phy_q0_ln3_rx_fifo_rden_i), //input dp_phy_q0_ln3_rx_fifo_rden_i
        .dp_phy_q0_ln3_tx_clk_i(dp_phy_q0_ln3_tx_clk_i), //input dp_phy_q0_ln3_tx_clk_i
        .dp_phy_q0_ln3_tx_data_i(dp_phy_q0_ln3_tx_data_i), //input [79:0] dp_phy_q0_ln3_tx_data_i
        .dp_phy_q0_ln3_tx_fifo_wren_i(dp_phy_q0_ln3_tx_fifo_wren_i), //input dp_phy_q0_ln3_tx_fifo_wren_i
        .dp_phy_q0_ln3_pma_rstn_i(dp_phy_q0_ln3_pma_rstn_i), //input dp_phy_q0_ln3_pma_rstn_i
        .dp_phy_q0_ln3_pcs_rx_rst_i(dp_phy_q0_ln3_pcs_rx_rst_i), //input dp_phy_q0_ln3_pcs_rx_rst_i
        .dp_phy_q0_ln3_pcs_tx_rst_i(dp_phy_q0_ln3_pcs_tx_rst_i), //input dp_phy_q0_ln3_pcs_tx_rst_i
        .dp_phy_drp_addr_i(dp_phy_drp_addr_i), //input [23:0] dp_phy_drp_addr_i
        .dp_phy_drp_wren_i(dp_phy_drp_wren_i), //input dp_phy_drp_wren_i
        .dp_phy_drp_wrdata_i(dp_phy_drp_wrdata_i), //input [31:0] dp_phy_drp_wrdata_i
        .dp_phy_drp_strb_i(dp_phy_drp_strb_i), //input [7:0] dp_phy_drp_strb_i
        .dp_phy_drp_rden_i(dp_phy_drp_rden_i), //input dp_phy_drp_rden_i
        .edp_phy_ln0_fabric_rstn_i(edp_phy_ln0_fabric_rstn_i), //input edp_phy_ln0_fabric_rstn_i
        .edp_phy_ln0_rx_rst_i(edp_phy_ln0_rx_rst_i), //input edp_phy_ln0_rx_rst_i
        .edp_phy_ln0_fabric_rx_clk_i(edp_phy_ln0_fabric_rx_clk_i), //input edp_phy_ln0_fabric_rx_clk_i
        .edp_phy_ln0_rxfifo_rden_i(edp_phy_ln0_rxfifo_rden_i), //input edp_phy_ln0_rxfifo_rden_i
        .edp_phy_ln0_tx_rst_i(edp_phy_ln0_tx_rst_i), //input edp_phy_ln0_tx_rst_i
        .edp_phy_ln0_fabric_tx_clk_i(edp_phy_ln0_fabric_tx_clk_i), //input edp_phy_ln0_fabric_tx_clk_i
        .edp_phy_ln0_tx_vld_i(edp_phy_ln0_tx_vld_i), //input edp_phy_ln0_tx_vld_i
        .edp_phy_ln0_txdata_i(edp_phy_ln0_txdata_i), //input [15:0] edp_phy_ln0_txdata_i
        .edp_phy_ln0_txk_i(edp_phy_ln0_txk_i), //input [1:0] edp_phy_ln0_txk_i
        .edp_phy_ln1_fabric_rstn_i(edp_phy_ln1_fabric_rstn_i), //input edp_phy_ln1_fabric_rstn_i
        .edp_phy_ln1_rx_rst_i(edp_phy_ln1_rx_rst_i), //input edp_phy_ln1_rx_rst_i
        .edp_phy_ln1_fabric_rx_clk_i(edp_phy_ln1_fabric_rx_clk_i), //input edp_phy_ln1_fabric_rx_clk_i
        .edp_phy_ln1_rxfifo_rden_i(edp_phy_ln1_rxfifo_rden_i), //input edp_phy_ln1_rxfifo_rden_i
        .edp_phy_ln1_tx_rst_i(edp_phy_ln1_tx_rst_i), //input edp_phy_ln1_tx_rst_i
        .edp_phy_ln1_fabric_tx_clk_i(edp_phy_ln1_fabric_tx_clk_i), //input edp_phy_ln1_fabric_tx_clk_i
        .edp_phy_ln1_tx_vld_i(edp_phy_ln1_tx_vld_i), //input edp_phy_ln1_tx_vld_i
        .edp_phy_ln1_txdata_i(edp_phy_ln1_txdata_i), //input [15:0] edp_phy_ln1_txdata_i
        .edp_phy_ln1_txk_i(edp_phy_ln1_txk_i), //input [1:0] edp_phy_ln1_txk_i
        .edp_phy_ln0_chbond_start_i(edp_phy_ln0_chbond_start_i), //input edp_phy_ln0_chbond_start_i
        .edp_phy_ln1_chbond_start_i(edp_phy_ln1_chbond_start_i), //input edp_phy_ln1_chbond_start_i
        .edp_phy_drp_addr_i(edp_phy_drp_addr_i), //input [23:0] edp_phy_drp_addr_i
        .edp_phy_drp_wren_i(edp_phy_drp_wren_i), //input edp_phy_drp_wren_i
        .edp_phy_drp_wrdata_i(edp_phy_drp_wrdata_i), //input [31:0] edp_phy_drp_wrdata_i
        .edp_phy_drp_strb_i(edp_phy_drp_strb_i), //input [7:0] edp_phy_drp_strb_i
        .edp_phy_drp_rden_i(edp_phy_drp_rden_i) //input edp_phy_drp_rden_i
    );

//--------Copy end-------------------
