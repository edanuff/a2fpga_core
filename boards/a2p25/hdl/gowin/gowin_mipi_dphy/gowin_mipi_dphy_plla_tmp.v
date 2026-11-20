//Copyright (C)2014-2026 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.12.03
//IP Version: 1.0
//Part Number: GW5A-LV25MG121NC1/I0
//Device: GW5A-25
//Device Version: A
//Created Time: Tue Sep  1 16:51:00 2026

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    Gowin_MIPI_DPHY_PLLA your_instance_name(
        .lock(lock), //output lock
        .clkout0(clkout0), //output clkout0
        .clkout1(clkout1), //output clkout1
        .clkout2(clkout2), //output clkout2
        .clkout3(clkout3), //output clkout3
        .clkout4(clkout4), //output clkout4
        .mdrdo(mdrdo), //output [7:0] mdrdo
        .clkin(clkin), //input clkin
        .reset(reset), //input reset
        .mdclk(mdclk), //input mdclk
        .mdopc(mdopc), //input [1:0] mdopc
        .mdainc(mdainc), //input mdainc
        .mdwdi(mdwdi) //input [7:0] mdwdi
    );

//--------Copy end-------------------
