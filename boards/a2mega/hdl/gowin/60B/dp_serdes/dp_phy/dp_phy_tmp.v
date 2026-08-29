//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.01
//Part Number: GW5AT-LV60PG484AC1/I0
//Device: GW5AT-60
//Device Version: B
//Created Time: Tue Aug 18 11:09:07 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

	dp_phy your_instance_name(
		.Q0_LANE2_PCS_RX_O_FABRIC_CLK(Q0_LANE2_PCS_RX_O_FABRIC_CLK), //input Q0_LANE2_PCS_RX_O_FABRIC_CLK
		.Q0_LANE2_FABRIC_RX_CLK(Q0_LANE2_FABRIC_RX_CLK), //output Q0_LANE2_FABRIC_RX_CLK
		.Q0_FABRIC_LN2_RXDATA_O(Q0_FABRIC_LN2_RXDATA_O), //input [87:0] Q0_FABRIC_LN2_RXDATA_O
		.Q0_LANE2_RX_IF_FIFO_RDEN(Q0_LANE2_RX_IF_FIFO_RDEN), //output Q0_LANE2_RX_IF_FIFO_RDEN
		.Q0_LANE2_RX_IF_FIFO_RDUSEWD(Q0_LANE2_RX_IF_FIFO_RDUSEWD), //input [4:0] Q0_LANE2_RX_IF_FIFO_RDUSEWD
		.Q0_LANE2_RX_IF_FIFO_AEMPTY(Q0_LANE2_RX_IF_FIFO_AEMPTY), //input Q0_LANE2_RX_IF_FIFO_AEMPTY
		.Q0_LANE2_RX_IF_FIFO_EMPTY(Q0_LANE2_RX_IF_FIFO_EMPTY), //input Q0_LANE2_RX_IF_FIFO_EMPTY
		.Q0_FABRIC_LN2_RX_VLD_OUT(Q0_FABRIC_LN2_RX_VLD_OUT), //input Q0_FABRIC_LN2_RX_VLD_OUT
		.Q0_LANE2_PCS_TX_O_FABRIC_CLK(Q0_LANE2_PCS_TX_O_FABRIC_CLK), //input Q0_LANE2_PCS_TX_O_FABRIC_CLK
		.Q0_LANE2_FABRIC_TX_CLK(Q0_LANE2_FABRIC_TX_CLK), //output Q0_LANE2_FABRIC_TX_CLK
		.Q0_FABRIC_LN2_TXDATA_I(Q0_FABRIC_LN2_TXDATA_I), //output [79:0] Q0_FABRIC_LN2_TXDATA_I
		.Q0_FABRIC_LN2_TX_VLD_IN(Q0_FABRIC_LN2_TX_VLD_IN), //output Q0_FABRIC_LN2_TX_VLD_IN
		.Q0_LANE2_TX_IF_FIFO_WRUSEWD(Q0_LANE2_TX_IF_FIFO_WRUSEWD), //input [4:0] Q0_LANE2_TX_IF_FIFO_WRUSEWD
		.Q0_LANE2_TX_IF_FIFO_AFULL(Q0_LANE2_TX_IF_FIFO_AFULL), //input Q0_LANE2_TX_IF_FIFO_AFULL
		.Q0_LANE2_TX_IF_FIFO_FULL(Q0_LANE2_TX_IF_FIFO_FULL), //input Q0_LANE2_TX_IF_FIFO_FULL
		.Q0_FABRIC_LN2_STAT_O(Q0_FABRIC_LN2_STAT_O), //input [12:0] Q0_FABRIC_LN2_STAT_O
		.Q0_LANE2_FABRIC_C2I_CLK(Q0_LANE2_FABRIC_C2I_CLK), //output Q0_LANE2_FABRIC_C2I_CLK
		.Q0_LANE2_CHBOND_START(Q0_LANE2_CHBOND_START), //output Q0_LANE2_CHBOND_START
		.Q0_FABRIC_LN2_RSTN_I(Q0_FABRIC_LN2_RSTN_I), //output Q0_FABRIC_LN2_RSTN_I
		.Q0_LANE2_PCS_RX_RST(Q0_LANE2_PCS_RX_RST), //output Q0_LANE2_PCS_RX_RST
		.Q0_LANE2_PCS_TX_RST(Q0_LANE2_PCS_TX_RST), //output Q0_LANE2_PCS_TX_RST
		.Q0_FABRIC_LANE2_CMU_CK_REF_O(Q0_FABRIC_LANE2_CMU_CK_REF_O), //input Q0_FABRIC_LANE2_CMU_CK_REF_O
		.Q0_FABRIC_LN2_ASTAT_O(Q0_FABRIC_LN2_ASTAT_O), //input [5:0] Q0_FABRIC_LN2_ASTAT_O
		.Q0_FABRIC_LN2_PMA_RX_LOCK_O(Q0_FABRIC_LN2_PMA_RX_LOCK_O), //input Q0_FABRIC_LN2_PMA_RX_LOCK_O
		.Q0_LANE2_ALIGN_LINK(Q0_LANE2_ALIGN_LINK), //input Q0_LANE2_ALIGN_LINK
		.Q0_LANE2_K_LOCK(Q0_LANE2_K_LOCK), //input Q0_LANE2_K_LOCK
		.Q0_FABRIC_LANE2_CMU_OK_O(Q0_FABRIC_LANE2_CMU_OK_O), //input Q0_FABRIC_LANE2_CMU_OK_O
		.Q0_FABRIC_LANE2_64B66B_TX_INVLD_BLK(Q0_FABRIC_LANE2_64B66B_TX_INVLD_BLK), //input Q0_FABRIC_LANE2_64B66B_TX_INVLD_BLK
		.Q0_FABRIC_LANE2_64B66B_TX_FETCH(Q0_FABRIC_LANE2_64B66B_TX_FETCH), //input Q0_FABRIC_LANE2_64B66B_TX_FETCH
		.Q0_FABRIC_LANE2_64B66B_RX_VALID(Q0_FABRIC_LANE2_64B66B_RX_VALID), //input Q0_FABRIC_LANE2_64B66B_RX_VALID
		.q0_ln2_rx_pcs_clkout_o(q0_ln2_rx_pcs_clkout_o), //output q0_ln2_rx_pcs_clkout_o
		.q0_ln2_rx_clk_i(q0_ln2_rx_clk_i), //input q0_ln2_rx_clk_i
		.q0_ln2_rx_data_o(q0_ln2_rx_data_o), //output [87:0] q0_ln2_rx_data_o
		.q0_ln2_rx_fifo_rden_i(q0_ln2_rx_fifo_rden_i), //input q0_ln2_rx_fifo_rden_i
		.q0_ln2_rx_fifo_rdusewd_o(q0_ln2_rx_fifo_rdusewd_o), //output [4:0] q0_ln2_rx_fifo_rdusewd_o
		.q0_ln2_rx_fifo_aempty_o(q0_ln2_rx_fifo_aempty_o), //output q0_ln2_rx_fifo_aempty_o
		.q0_ln2_rx_fifo_empty_o(q0_ln2_rx_fifo_empty_o), //output q0_ln2_rx_fifo_empty_o
		.q0_ln2_rx_valid_o(q0_ln2_rx_valid_o), //output q0_ln2_rx_valid_o
		.q0_ln2_tx_pcs_clkout_o(q0_ln2_tx_pcs_clkout_o), //output q0_ln2_tx_pcs_clkout_o
		.q0_ln2_tx_clk_i(q0_ln2_tx_clk_i), //input q0_ln2_tx_clk_i
		.q0_ln2_tx_data_i(q0_ln2_tx_data_i), //input [79:0] q0_ln2_tx_data_i
		.q0_ln2_tx_fifo_wren_i(q0_ln2_tx_fifo_wren_i), //input q0_ln2_tx_fifo_wren_i
		.q0_ln2_tx_fifo_wrusewd_o(q0_ln2_tx_fifo_wrusewd_o), //output [4:0] q0_ln2_tx_fifo_wrusewd_o
		.q0_ln2_tx_fifo_afull_o(q0_ln2_tx_fifo_afull_o), //output q0_ln2_tx_fifo_afull_o
		.q0_ln2_tx_fifo_full_o(q0_ln2_tx_fifo_full_o), //output q0_ln2_tx_fifo_full_o
		.q0_ln2_ready_o(q0_ln2_ready_o), //output q0_ln2_ready_o
		.q0_ln2_pma_rstn_i(q0_ln2_pma_rstn_i), //input q0_ln2_pma_rstn_i
		.q0_ln2_pcs_rx_rst_i(q0_ln2_pcs_rx_rst_i), //input q0_ln2_pcs_rx_rst_i
		.q0_ln2_pcs_tx_rst_i(q0_ln2_pcs_tx_rst_i), //input q0_ln2_pcs_tx_rst_i
		.q0_ln2_refclk_o(q0_ln2_refclk_o), //output q0_ln2_refclk_o
		.q0_ln2_signal_detect_o(q0_ln2_signal_detect_o), //output q0_ln2_signal_detect_o
		.q0_ln2_rx_cdr_lock_o(q0_ln2_rx_cdr_lock_o), //output q0_ln2_rx_cdr_lock_o
		.q0_ln2_pll_lock_o(q0_ln2_pll_lock_o), //output q0_ln2_pll_lock_o
		.Q0_LANE3_PCS_RX_O_FABRIC_CLK(Q0_LANE3_PCS_RX_O_FABRIC_CLK), //input Q0_LANE3_PCS_RX_O_FABRIC_CLK
		.Q0_LANE3_FABRIC_RX_CLK(Q0_LANE3_FABRIC_RX_CLK), //output Q0_LANE3_FABRIC_RX_CLK
		.Q0_FABRIC_LN3_RXDATA_O(Q0_FABRIC_LN3_RXDATA_O), //input [87:0] Q0_FABRIC_LN3_RXDATA_O
		.Q0_LANE3_RX_IF_FIFO_RDEN(Q0_LANE3_RX_IF_FIFO_RDEN), //output Q0_LANE3_RX_IF_FIFO_RDEN
		.Q0_LANE3_RX_IF_FIFO_RDUSEWD(Q0_LANE3_RX_IF_FIFO_RDUSEWD), //input [4:0] Q0_LANE3_RX_IF_FIFO_RDUSEWD
		.Q0_LANE3_RX_IF_FIFO_AEMPTY(Q0_LANE3_RX_IF_FIFO_AEMPTY), //input Q0_LANE3_RX_IF_FIFO_AEMPTY
		.Q0_LANE3_RX_IF_FIFO_EMPTY(Q0_LANE3_RX_IF_FIFO_EMPTY), //input Q0_LANE3_RX_IF_FIFO_EMPTY
		.Q0_FABRIC_LN3_RX_VLD_OUT(Q0_FABRIC_LN3_RX_VLD_OUT), //input Q0_FABRIC_LN3_RX_VLD_OUT
		.Q0_LANE3_PCS_TX_O_FABRIC_CLK(Q0_LANE3_PCS_TX_O_FABRIC_CLK), //input Q0_LANE3_PCS_TX_O_FABRIC_CLK
		.Q0_LANE3_FABRIC_TX_CLK(Q0_LANE3_FABRIC_TX_CLK), //output Q0_LANE3_FABRIC_TX_CLK
		.Q0_FABRIC_LN3_TXDATA_I(Q0_FABRIC_LN3_TXDATA_I), //output [79:0] Q0_FABRIC_LN3_TXDATA_I
		.Q0_FABRIC_LN3_TX_VLD_IN(Q0_FABRIC_LN3_TX_VLD_IN), //output Q0_FABRIC_LN3_TX_VLD_IN
		.Q0_LANE3_TX_IF_FIFO_WRUSEWD(Q0_LANE3_TX_IF_FIFO_WRUSEWD), //input [4:0] Q0_LANE3_TX_IF_FIFO_WRUSEWD
		.Q0_LANE3_TX_IF_FIFO_AFULL(Q0_LANE3_TX_IF_FIFO_AFULL), //input Q0_LANE3_TX_IF_FIFO_AFULL
		.Q0_LANE3_TX_IF_FIFO_FULL(Q0_LANE3_TX_IF_FIFO_FULL), //input Q0_LANE3_TX_IF_FIFO_FULL
		.Q0_FABRIC_LN3_STAT_O(Q0_FABRIC_LN3_STAT_O), //input [12:0] Q0_FABRIC_LN3_STAT_O
		.Q0_LANE3_FABRIC_C2I_CLK(Q0_LANE3_FABRIC_C2I_CLK), //output Q0_LANE3_FABRIC_C2I_CLK
		.Q0_LANE3_CHBOND_START(Q0_LANE3_CHBOND_START), //output Q0_LANE3_CHBOND_START
		.Q0_FABRIC_LN3_RSTN_I(Q0_FABRIC_LN3_RSTN_I), //output Q0_FABRIC_LN3_RSTN_I
		.Q0_LANE3_PCS_RX_RST(Q0_LANE3_PCS_RX_RST), //output Q0_LANE3_PCS_RX_RST
		.Q0_LANE3_PCS_TX_RST(Q0_LANE3_PCS_TX_RST), //output Q0_LANE3_PCS_TX_RST
		.Q0_FABRIC_LANE3_CMU_CK_REF_O(Q0_FABRIC_LANE3_CMU_CK_REF_O), //input Q0_FABRIC_LANE3_CMU_CK_REF_O
		.Q0_FABRIC_LN3_ASTAT_O(Q0_FABRIC_LN3_ASTAT_O), //input [5:0] Q0_FABRIC_LN3_ASTAT_O
		.Q0_FABRIC_LN3_PMA_RX_LOCK_O(Q0_FABRIC_LN3_PMA_RX_LOCK_O), //input Q0_FABRIC_LN3_PMA_RX_LOCK_O
		.Q0_LANE3_ALIGN_LINK(Q0_LANE3_ALIGN_LINK), //input Q0_LANE3_ALIGN_LINK
		.Q0_LANE3_K_LOCK(Q0_LANE3_K_LOCK), //input Q0_LANE3_K_LOCK
		.Q0_FABRIC_LANE3_CMU_OK_O(Q0_FABRIC_LANE3_CMU_OK_O), //input Q0_FABRIC_LANE3_CMU_OK_O
		.Q0_FABRIC_LANE3_64B66B_TX_INVLD_BLK(Q0_FABRIC_LANE3_64B66B_TX_INVLD_BLK), //input Q0_FABRIC_LANE3_64B66B_TX_INVLD_BLK
		.Q0_FABRIC_LANE3_64B66B_TX_FETCH(Q0_FABRIC_LANE3_64B66B_TX_FETCH), //input Q0_FABRIC_LANE3_64B66B_TX_FETCH
		.Q0_FABRIC_LANE3_64B66B_RX_VALID(Q0_FABRIC_LANE3_64B66B_RX_VALID), //input Q0_FABRIC_LANE3_64B66B_RX_VALID
		.q0_ln3_rx_pcs_clkout_o(q0_ln3_rx_pcs_clkout_o), //output q0_ln3_rx_pcs_clkout_o
		.q0_ln3_rx_clk_i(q0_ln3_rx_clk_i), //input q0_ln3_rx_clk_i
		.q0_ln3_rx_data_o(q0_ln3_rx_data_o), //output [87:0] q0_ln3_rx_data_o
		.q0_ln3_rx_fifo_rden_i(q0_ln3_rx_fifo_rden_i), //input q0_ln3_rx_fifo_rden_i
		.q0_ln3_rx_fifo_rdusewd_o(q0_ln3_rx_fifo_rdusewd_o), //output [4:0] q0_ln3_rx_fifo_rdusewd_o
		.q0_ln3_rx_fifo_aempty_o(q0_ln3_rx_fifo_aempty_o), //output q0_ln3_rx_fifo_aempty_o
		.q0_ln3_rx_fifo_empty_o(q0_ln3_rx_fifo_empty_o), //output q0_ln3_rx_fifo_empty_o
		.q0_ln3_rx_valid_o(q0_ln3_rx_valid_o), //output q0_ln3_rx_valid_o
		.q0_ln3_tx_pcs_clkout_o(q0_ln3_tx_pcs_clkout_o), //output q0_ln3_tx_pcs_clkout_o
		.q0_ln3_tx_clk_i(q0_ln3_tx_clk_i), //input q0_ln3_tx_clk_i
		.q0_ln3_tx_data_i(q0_ln3_tx_data_i), //input [79:0] q0_ln3_tx_data_i
		.q0_ln3_tx_fifo_wren_i(q0_ln3_tx_fifo_wren_i), //input q0_ln3_tx_fifo_wren_i
		.q0_ln3_tx_fifo_wrusewd_o(q0_ln3_tx_fifo_wrusewd_o), //output [4:0] q0_ln3_tx_fifo_wrusewd_o
		.q0_ln3_tx_fifo_afull_o(q0_ln3_tx_fifo_afull_o), //output q0_ln3_tx_fifo_afull_o
		.q0_ln3_tx_fifo_full_o(q0_ln3_tx_fifo_full_o), //output q0_ln3_tx_fifo_full_o
		.q0_ln3_ready_o(q0_ln3_ready_o), //output q0_ln3_ready_o
		.q0_ln3_pma_rstn_i(q0_ln3_pma_rstn_i), //input q0_ln3_pma_rstn_i
		.q0_ln3_pcs_rx_rst_i(q0_ln3_pcs_rx_rst_i), //input q0_ln3_pcs_rx_rst_i
		.q0_ln3_pcs_tx_rst_i(q0_ln3_pcs_tx_rst_i), //input q0_ln3_pcs_tx_rst_i
		.q0_ln3_refclk_o(q0_ln3_refclk_o), //output q0_ln3_refclk_o
		.q0_ln3_signal_detect_o(q0_ln3_signal_detect_o), //output q0_ln3_signal_detect_o
		.q0_ln3_rx_cdr_lock_o(q0_ln3_rx_cdr_lock_o), //output q0_ln3_rx_cdr_lock_o
		.q0_ln3_pll_lock_o(q0_ln3_pll_lock_o), //output q0_ln3_pll_lock_o
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
		.Q0_FABRIC_CMU_CK_REF_O(Q0_FABRIC_CMU_CK_REF_O), //input Q0_FABRIC_CMU_CK_REF_O
		.Q0_FABRIC_CMU1_CK_REF_O(Q0_FABRIC_CMU1_CK_REF_O), //input Q0_FABRIC_CMU1_CK_REF_O
		.Q0_FABRIC_CMU1_OK_O(Q0_FABRIC_CMU1_OK_O), //input Q0_FABRIC_CMU1_OK_O
		.Q0_FABRIC_CMU_OK_O(Q0_FABRIC_CMU_OK_O), //input Q0_FABRIC_CMU_OK_O
		.Q1_FABRIC_CMU_CK_REF_O(Q1_FABRIC_CMU_CK_REF_O), //input Q1_FABRIC_CMU_CK_REF_O
		.Q1_FABRIC_CMU1_CK_REF_O(Q1_FABRIC_CMU1_CK_REF_O), //input Q1_FABRIC_CMU1_CK_REF_O
		.Q1_FABRIC_CMU1_OK_O(Q1_FABRIC_CMU1_OK_O), //input Q1_FABRIC_CMU1_OK_O
		.Q1_FABRIC_CMU_OK_O(Q1_FABRIC_CMU_OK_O) //input Q1_FABRIC_CMU_OK_O
	);

//--------Copy end-------------------
