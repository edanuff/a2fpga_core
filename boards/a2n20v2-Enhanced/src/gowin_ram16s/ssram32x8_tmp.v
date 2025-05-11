//Copyright (C)2014-2025 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.11.01 Education
//Part Number: GW2AR-LV18QN88C8/I7
//Device: GW2AR-18
//Device Version: C
//Created Time: Sat May 10 16:50:27 2025

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    ssram32x8 your_instance_name(
        .dout(dout), //output [7:0] dout
        .wre(wre), //input wre
        .ad(ad), //input [4:0] ad
        .di(di), //input [7:0] di
        .clk(clk) //input clk
    );

//--------Copy end-------------------
