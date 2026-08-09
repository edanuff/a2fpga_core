///////////////////////////////////////////////////////////////////////////////
// ./src/auxch/hotplug_decode.v : 
//
// Author: Mike Field <hamster@snap.net.nz>
//
// Part of the DisplayPort_Verlog project - an open implementation of the 
// DisplayPort protocol for FPGA boards. 
//
// See https://github.com/hamsternz/DisplayPort_Verilog for latest versions.
//
///////////////////////////////////////////////////////////////////////////////
// Version |  Notes
// ----------------------------------------------------------------------------
//   1.0   | Initial Release
//
///////////////////////////////////////////////////////////////////////////////
//
// MIT License
// 
// Copyright (c) 2019 Mike Field
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////
//
// Want to say thanks?
//
// This design has taken many hours - 3 months of work for the initial VHDL
// design, and another month or so to convert it to Verilog for this release.
//
// I'm more than happy to share it if you can make use of it. It is released
// under the MIT license, so you are not under any onus to say thanks, but....
//
// If you what to say thanks for this design either drop me an email, or how about
// trying PayPal to my email (hamster@snap.net.nz)?
//
//  Educational use - Enough for a beer
//  Hobbyist use    - Enough for a pizza
//  Research use    - Enough to take the family out to dinner
//  Commercial use  - A weeks pay for an engineer (I wish!)
//
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module hotplug_decode #(
    parameter integer CLK_HZ         = 100000000,
    parameter integer IRQ_MIN_US     = 500,
    parameter integer IRQ_MAX_US     = 1000,
    parameter integer DISCONNECT_US  = 2000,
    parameter integer PRESENT_US     = 2000
)(
    input  clk,
    input  hpd,
    output reg irq,
    output reg present
);

   (* ASYNC_REG = "TRUE" *) reg hpd_meta1;
   (* ASYNC_REG = "TRUE" *) reg hpd_meta2;
   reg hpd_synced;
   reg hpd_last;

   // The management clock used by this repository is an integer number of MHz.
   localparam integer CYCLES_PER_US     = CLK_HZ / 1000000;
   localparam integer IRQ_MIN_CYCLES    = CYCLES_PER_US * IRQ_MIN_US;
   localparam integer IRQ_MAX_CYCLES    = CYCLES_PER_US * IRQ_MAX_US;
   localparam integer DISCONNECT_CYCLES = CYCLES_PER_US * DISCONNECT_US;
   localparam integer PRESENT_CYCLES    = CYCLES_PER_US * PRESENT_US;
   localparam integer MAX_CYCLES =
       (DISCONNECT_CYCLES > PRESENT_CYCLES) ?
       DISCONNECT_CYCLES : PRESENT_CYCLES;
   localparam integer COUNTER_WIDTH = $clog2(MAX_CYCLES + 1);

   reg [COUNTER_WIDTH-1:0] pulse_count;


initial begin
    hpd_meta1    = 1'b0;
    hpd_meta2    = 1'b0;
    hpd_synced   = 1'b0;
    hpd_last     = 1'b0;
    pulse_count  = {COUNTER_WIDTH{1'b0}};
    present      = 1'b0;
    irq          = 1'b0;
end 

always @(posedge clk) begin
    irq <= 1'b0;
    if(hpd_synced == 1'b0) begin
        if(hpd_last == 1'b1) begin
            // Start timing a low interval.
            pulse_count <= {{(COUNTER_WIDTH-1){1'b0}}, 1'b1};
        end else if(pulse_count < DISCONNECT_CYCLES) begin
            pulse_count <= pulse_count + 1'b1;
            if(pulse_count == DISCONNECT_CYCLES-1)
                present <= 1'b0;
        end
    end else begin
        if(hpd_last == 1'b0) begin
            // A 0.5-1.0 ms low interval is a DisplayPort HPD IRQ. A low
            // interval of 2 ms or more is disconnect/reconnect.
            if(present == 1'b1 &&
               pulse_count >= IRQ_MIN_CYCLES &&
               pulse_count <= IRQ_MAX_CYCLES)
                irq <= 1'b1;
            pulse_count <= {{(COUNTER_WIDTH-1){1'b0}}, 1'b1};
        end else if(pulse_count < PRESENT_CYCLES) begin
            pulse_count <= pulse_count + 1'b1;
            if(pulse_count == PRESENT_CYCLES-1)
                present <= 1'b1;
        end
    end
    hpd_last   <= hpd_synced;
    hpd_synced <= hpd_meta1;
    hpd_meta1  <= hpd_meta2;
    hpd_meta2  <= hpd;
end
endmodule
