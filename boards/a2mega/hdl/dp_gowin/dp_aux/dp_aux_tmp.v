//Copyright (C)2014-2026 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.03
//IP Version: 1.0
//Part Number: GW5AT-LV60PG484AC1/I0
//Device: GW5AT-60
//Device Version: B
//Created Time: Wed Aug 19 09:31:28 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

	dp_aux your_instance_name(
		.I_drp_clk(I_drp_clk), //input I_drp_clk
		.I_drp_rddata(I_drp_rddata), //input [31:0] I_drp_rddata
		.I_drp_rdvld(I_drp_rdvld), //input I_drp_rdvld
		.I_drp_ready(I_drp_ready), //input I_drp_ready
		.O_drp_addr(O_drp_addr), //output [23:0] O_drp_addr
		.O_drp_rden(O_drp_rden), //output O_drp_rden
		.O_drp_wrdata(O_drp_wrdata), //output [31:0] O_drp_wrdata
		.O_drp_wren(O_drp_wren), //output O_drp_wren
		.I_test0(I_test0), //input [31:0] I_test0
		.I_test1(I_test1), //input [31:0] I_test1
		.I_test2(I_test2), //input [31:0] I_test2
		.I_test3(I_test3), //input [31:0] I_test3
		.O_test0(O_test0), //output [31:0] O_test0
		.O_test1(O_test1), //output [31:0] O_test1
		.O_test2(O_test2), //output [31:0] O_test2
		.O_test3(O_test3), //output [31:0] O_test3
		.O_tx_frame_type(O_tx_frame_type), //output O_tx_frame_type
		.O_tx_scrm_value(O_tx_scrm_value), //output O_tx_scrm_value
		.O_tx_tps1_en(O_tx_tps1_en), //output O_tx_tps1_en
		.O_tx_tps2_en(O_tx_tps2_en), //output O_tx_tps2_en
		.O_tx_tps3_en(O_tx_tps3_en), //output O_tx_tps3_en
		.O_tx_video_data_en(O_tx_video_data_en), //output O_tx_video_data_en
		.O_tx_end(O_tx_end), //output O_tx_end
		.O_tx_aux_is_ok(O_tx_aux_is_ok), //output O_tx_aux_is_ok
		.O_tx_lane_num(O_tx_lane_num), //output [2:0] O_tx_lane_num
		.O_tx_man_test(O_tx_man_test), //output O_tx_man_test
		.IO_tx_man_p(IO_tx_man_p), //inout IO_tx_man_p
		.IO_tx_man_n(IO_tx_man_n), //inout IO_tx_man_n
		.O_rx_irqn(O_rx_irqn), //output O_rx_irqn
		.O_rx_man_test(O_rx_man_test), //output O_rx_man_test
		.O_rx_hpd(O_rx_hpd), //output O_rx_hpd
		.O_rx_end(O_rx_end), //output O_rx_end
		.O_rx_frame_type(O_rx_frame_type), //output O_rx_frame_type
		.O_rx_scrm_value(O_rx_scrm_value), //output O_rx_scrm_value
		.O_rx_tps1_en(O_rx_tps1_en), //output O_rx_tps1_en
		.O_rx_tps2_en(O_rx_tps2_en), //output O_rx_tps2_en
		.O_rx_tps3_en(O_rx_tps3_en), //output O_rx_tps3_en
		.IO_rx_man_p(IO_rx_man_p), //inout IO_rx_man_p
		.IO_rx_man_n(IO_rx_man_n), //inout IO_rx_man_n
		.I_paddr(I_paddr), //input [9:0] I_paddr
		.I_pclk(I_pclk), //input I_pclk
		.I_penable(I_penable), //input I_penable
		.I_presetn(I_presetn), //input I_presetn
		.I_psel(I_psel), //input I_psel
		.I_pwdata(I_pwdata), //input [31:0] I_pwdata
		.I_pwrite(I_pwrite), //input I_pwrite
		.O_prdata(O_prdata), //output [31:0] O_prdata
		.O_pready(O_pready) //output O_pready
	);

//--------Copy end-------------------
