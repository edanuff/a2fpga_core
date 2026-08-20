//Copyright (C)2014-2026 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.03
//IP Version: 1.0
//Part Number: GW5AST-LV138PG484AC1/I0
//Device: GW5AST-138
//Device Version: B
//Created Time: Thu Aug 20 09:50:25 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    dp_serdes your_instance_name(
        .dp_phy_q0_ln1_rx_pcs_clkout_o(dp_phy_q0_ln1_rx_pcs_clkout_o), //output dp_phy_q0_ln1_rx_pcs_clkout_o
        .dp_phy_q0_ln1_rx_data_o(dp_phy_q0_ln1_rx_data_o), //output [87:0] dp_phy_q0_ln1_rx_data_o
        .dp_phy_q0_ln1_rx_fifo_rdusewd_o(dp_phy_q0_ln1_rx_fifo_rdusewd_o), //output [4:0] dp_phy_q0_ln1_rx_fifo_rdusewd_o
        .dp_phy_q0_ln1_rx_fifo_aempty_o(dp_phy_q0_ln1_rx_fifo_aempty_o), //output dp_phy_q0_ln1_rx_fifo_aempty_o
        .dp_phy_q0_ln1_rx_fifo_empty_o(dp_phy_q0_ln1_rx_fifo_empty_o), //output dp_phy_q0_ln1_rx_fifo_empty_o
        .dp_phy_q0_ln1_rx_valid_o(dp_phy_q0_ln1_rx_valid_o), //output dp_phy_q0_ln1_rx_valid_o
        .dp_phy_q0_ln1_tx_pcs_clkout_o(dp_phy_q0_ln1_tx_pcs_clkout_o), //output dp_phy_q0_ln1_tx_pcs_clkout_o
        .dp_phy_q0_ln1_tx_fifo_wrusewd_o(dp_phy_q0_ln1_tx_fifo_wrusewd_o), //output [4:0] dp_phy_q0_ln1_tx_fifo_wrusewd_o
        .dp_phy_q0_ln1_tx_fifo_afull_o(dp_phy_q0_ln1_tx_fifo_afull_o), //output dp_phy_q0_ln1_tx_fifo_afull_o
        .dp_phy_q0_ln1_tx_fifo_full_o(dp_phy_q0_ln1_tx_fifo_full_o), //output dp_phy_q0_ln1_tx_fifo_full_o
        .dp_phy_q0_ln1_refclk_o(dp_phy_q0_ln1_refclk_o), //output dp_phy_q0_ln1_refclk_o
        .dp_phy_q0_ln1_signal_detect_o(dp_phy_q0_ln1_signal_detect_o), //output dp_phy_q0_ln1_signal_detect_o
        .dp_phy_q0_ln1_rx_cdr_lock_o(dp_phy_q0_ln1_rx_cdr_lock_o), //output dp_phy_q0_ln1_rx_cdr_lock_o
        .dp_phy_q0_ln1_pll_lock_o(dp_phy_q0_ln1_pll_lock_o), //output dp_phy_q0_ln1_pll_lock_o
        .dp_phy_q0_ln1_ready_o(dp_phy_q0_ln1_ready_o), //output dp_phy_q0_ln1_ready_o
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
        .dp_phy_drp_clk_o(dp_phy_drp_clk_o), //output dp_phy_drp_clk_o
        .dp_phy_drp_ready_o(dp_phy_drp_ready_o), //output dp_phy_drp_ready_o
        .dp_phy_drp_rdvld_o(dp_phy_drp_rdvld_o), //output dp_phy_drp_rdvld_o
        .dp_phy_drp_rddata_o(dp_phy_drp_rddata_o), //output [31:0] dp_phy_drp_rddata_o
        .dp_phy_drp_resp_o(dp_phy_drp_resp_o), //output dp_phy_drp_resp_o
        .por_n_i(por_n_i), //input por_n_i
        .dp_phy_q0_ln1_rx_clk_i(dp_phy_q0_ln1_rx_clk_i), //input dp_phy_q0_ln1_rx_clk_i
        .dp_phy_q0_ln1_rx_fifo_rden_i(dp_phy_q0_ln1_rx_fifo_rden_i), //input dp_phy_q0_ln1_rx_fifo_rden_i
        .dp_phy_q0_ln1_tx_clk_i(dp_phy_q0_ln1_tx_clk_i), //input dp_phy_q0_ln1_tx_clk_i
        .dp_phy_q0_ln1_tx_data_i(dp_phy_q0_ln1_tx_data_i), //input [79:0] dp_phy_q0_ln1_tx_data_i
        .dp_phy_q0_ln1_tx_fifo_wren_i(dp_phy_q0_ln1_tx_fifo_wren_i), //input dp_phy_q0_ln1_tx_fifo_wren_i
        .dp_phy_q0_ln1_pma_rstn_i(dp_phy_q0_ln1_pma_rstn_i), //input dp_phy_q0_ln1_pma_rstn_i
        .dp_phy_q0_ln1_pcs_rx_rst_i(dp_phy_q0_ln1_pcs_rx_rst_i), //input dp_phy_q0_ln1_pcs_rx_rst_i
        .dp_phy_q0_ln1_pcs_tx_rst_i(dp_phy_q0_ln1_pcs_tx_rst_i), //input dp_phy_q0_ln1_pcs_tx_rst_i
        .dp_phy_q0_ln2_rx_clk_i(dp_phy_q0_ln2_rx_clk_i), //input dp_phy_q0_ln2_rx_clk_i
        .dp_phy_q0_ln2_rx_fifo_rden_i(dp_phy_q0_ln2_rx_fifo_rden_i), //input dp_phy_q0_ln2_rx_fifo_rden_i
        .dp_phy_q0_ln2_tx_clk_i(dp_phy_q0_ln2_tx_clk_i), //input dp_phy_q0_ln2_tx_clk_i
        .dp_phy_q0_ln2_tx_data_i(dp_phy_q0_ln2_tx_data_i), //input [79:0] dp_phy_q0_ln2_tx_data_i
        .dp_phy_q0_ln2_tx_fifo_wren_i(dp_phy_q0_ln2_tx_fifo_wren_i), //input dp_phy_q0_ln2_tx_fifo_wren_i
        .dp_phy_q0_ln2_pma_rstn_i(dp_phy_q0_ln2_pma_rstn_i), //input dp_phy_q0_ln2_pma_rstn_i
        .dp_phy_q0_ln2_pcs_rx_rst_i(dp_phy_q0_ln2_pcs_rx_rst_i), //input dp_phy_q0_ln2_pcs_rx_rst_i
        .dp_phy_q0_ln2_pcs_tx_rst_i(dp_phy_q0_ln2_pcs_tx_rst_i), //input dp_phy_q0_ln2_pcs_tx_rst_i
        .dp_phy_drp_addr_i(dp_phy_drp_addr_i), //input [23:0] dp_phy_drp_addr_i
        .dp_phy_drp_wren_i(dp_phy_drp_wren_i), //input dp_phy_drp_wren_i
        .dp_phy_drp_wrdata_i(dp_phy_drp_wrdata_i), //input [31:0] dp_phy_drp_wrdata_i
        .dp_phy_drp_strb_i(dp_phy_drp_strb_i), //input [7:0] dp_phy_drp_strb_i
        .dp_phy_drp_rden_i(dp_phy_drp_rden_i) //input dp_phy_drp_rden_i
    );

//--------Copy end-------------------
