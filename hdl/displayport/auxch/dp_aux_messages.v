///////////////////////////////////////////////////////////////////////////////
// ./src/auxch/dp_aux_messages.v : 
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

module dp_aux_messages #(
    parameter LINK_RATE_MBPS = 2700   // 1620 (RBR) or 2700 (HBR)
)(
   input        clk,

   // Interface to send messages
   input        msg_de,
   input  [7:0] msg,
   output reg   busy,

   // M5 runtime AFE adjust: the TRAINING_LANEx_SET value message 0x19
   // declares — the ACTUALLY-APPLIED swing/pre-emphasis + cap flags from
   // afe_adjust_seq, PER LANE: [7:0] = lane 0 (DPCD 0x103), [15:8] =
   // lane 1 (0x104). Tie to 16'h0606 (legacy swing2+MAX_SWING) when unused.
   input  [15:0] train_set_byte,
   // IRQ SERVICE (08-26): the DEVICE_SERVICE_IRQ_VECTOR bits captured from
   // the last status read, written BACK to DPCD 0x201 (write-1s-to-clear)
   // by msg 0x1A. A conformant source must acknowledge sink IRQs; ours
   // never did — every frozen-wedge snapshot showed LINK_STATUS_UPDATED
   // latched with the sink waiting for service that never came.
   input  [7:0] irq_clear_byte,

   // Interface to the AUX Channel
   output reg       aux_tx_wr_en,
   output reg [7:0] aux_tx_data
);

   reg [11:0] counter;
initial begin
    counter = 12'b0;
    busy    = 1'b0;
    aux_tx_wr_en = 1'b0;
    aux_tx_data  = 8'b0;
end
always @(posedge clk) begin
    case(counter)
       // Write to I2C device at x50 (EDID)
       12'h010: begin aux_tx_data <= 8'h40; aux_tx_wr_en <= 1'b1; end
       12'h011: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h012: begin aux_tx_data <= 8'h50; aux_tx_wr_en <= 1'b1; end
       12'h013: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h014: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
    
       // Read a block of EDID data
       12'h020: begin aux_tx_data <= 8'h50; aux_tx_wr_en <= 1'b1; end
       12'h021: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h022: begin aux_tx_data <= 8'h50; aux_tx_wr_en <= 1'b1; end
       12'h023: begin aux_tx_data <= 8'h0F; aux_tx_wr_en <= 1'b1; end

       // Read Sink count
       12'h030: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h031: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h032: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h033: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end

       // Read DP configuration registers (12 of them)
       12'h040: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h041: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h042: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h043: begin aux_tx_data <= 8'h0B; aux_tx_wr_en <= 1'b1; end

       // Write DPCD powerstate D3 
       12'h050: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h051: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end
       12'h052: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h053: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h054: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end

  // Set channel coding (8b/10b)
       12'h060: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h061: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h062: begin aux_tx_data <= 8'h08; aux_tx_wr_en <= 1'b1; end
       12'h063: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h064: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end

       // Set link bandwidth (DPCD 0x100: rate / 0.27 Gbps)
       12'h070: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h071: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h072: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h073: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h074: begin aux_tx_data <= LINK_RATE_MBPS/270; aux_tx_wr_en <= 1'b1; end

       // Write Link Downspread
       12'h080: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h081: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h082: begin aux_tx_data <= 8'h07; aux_tx_wr_en <= 1'b1; end
       12'h083: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       // TRUTHFUL DOWNSPREAD (2026-08-18): was 0x10 (SPREAD_AMP=1,
       // inherited) — but the GTR12 QPLL does NO spread-spectrum (Q:
       // meter: fixed +7.5ppm). Declaring SSC that never comes makes
       // strict bridge CDRs mis-tune their tracking loops — candidate
       // for the "locks TPS, dies on scrambled data" class (Ugreen).
       // 0x00 = no downspread: declare what the clock actually does.
       // UN-PARKED (same day): scrambler fix tested alone (Ugreen still
       // C:8000) — truthful downspread now ships as the next single
       // variable.
       12'h084: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end

       // Set link count 1
       12'h090: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h091: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h092: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h093: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h094: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end  // Standard framing, one channel

       // Set link count 2
       12'h0A0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h0A1: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h0A2: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h0A3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h0A4: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end

       // Set link count 4
       12'h0B0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h0B1: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h0B2: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h0B3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h0B4: begin aux_tx_data <= 8'h04; aux_tx_wr_en <= 1'b1; end

       // Set training pattern 1
       12'h0C0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h0C1: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h0C2: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h0C3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h0C4: begin aux_tx_data <= 8'h21; aux_tx_wr_en <= 1'b1; end

       // Read link status for all four lanes 
       12'h0D0: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h0D1: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h0D2: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h0D3: begin aux_tx_data <= 8'h07; aux_tx_wr_en <= 1'b1; end

       //  Read the Adjust_Request registers
       12'h0E0: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h0E1: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h0E2: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end
       12'h0E3: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end

       // Set training pattern 2
       12'h0F0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h0F1: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h0F2: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h0F3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h0F4: begin aux_tx_data <= 8'h22; aux_tx_wr_en <= 1'b1; end

       // M5: dynamic TRAINING_LANEx_SET (DPCD 0x103-0x106) — declares the
       // levels the TX-AFE ACTUALLY drives (registered train_set_byte from
       // afe_adjust_seq) instead of a fixed constant. Selected by
       // aux_channel in every set state when AFE_ADJUST != 0.
       12'h190: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h191: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h192: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h193: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       // PER LANE: 0x103 <= lane 0 byte, 0x104 <= lane 1 byte. Lanes 2/3
       // (0x105/0x106) are not part of our 2-lane link and are ignored by
       // the sink; they mirror lanes 0/1 so the message keeps its 4-byte
       // length (AUX timing identical to msg 0x18 and to the previous 0x19).
       12'h194: begin aux_tx_data <= train_set_byte[7:0];  aux_tx_wr_en <= 1'b1; end  // 0x103 lane 0
       12'h195: begin aux_tx_data <= train_set_byte[15:8]; aux_tx_wr_en <= 1'b1; end  // 0x104 lane 1
       12'h196: begin aux_tx_data <= train_set_byte[7:0];  aux_tx_wr_en <= 1'b1; end  // 0x105 (unused lane)
       12'h197: begin aux_tx_data <= train_set_byte[15:8]; aux_tx_wr_en <= 1'b1; end  // 0x106 (unused lane)

       // IRQ service: native 1-byte WRITE of the latched vector back to
       // DPCD 0x201 — write-1s-to-clear acknowledgment (msg 0x1A)
       12'h1A0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h1A1: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h1A2: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h1A3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h1A4: begin aux_tx_data <= irq_clear_byte; aux_tx_wr_en <= 1'b1; end

       // ESI SERVICE (polite-attach build): the hub NEVER raises legacy
       // 0x201 (wire-proven: zero nonzero reads across attach/unplug/replug
       // captures; the Mac services exclusively via ESI). msg 0x1B reads
       // the ESI0 block 0x2003-0x200F in one transaction (the Mac's exact
       // read); msg 0x1C write-1-clears LINK_SERVICE_IRQ_VECTOR_ESI0
       // (0x2005) with irq_clear_byte (the latched vector, or the
       // unconditional 0x02 attach ack the Mac performs).
       12'h1B0: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h1B1: begin aux_tx_data <= 8'h20; aux_tx_wr_en <= 1'b1; end
       12'h1B2: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h1B3: begin aux_tx_data <= 8'h0C; aux_tx_wr_en <= 1'b1; end

       12'h1C0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h1C1: begin aux_tx_data <= 8'h20; aux_tx_wr_en <= 1'b1; end
       12'h1C2: begin aux_tx_data <= 8'h05; aux_tx_wr_en <= 1'b1; end
       12'h1C3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h1C4: begin aux_tx_data <= irq_clear_byte; aux_tx_wr_en <= 1'b1; end

       // msg 0x1D: W1C DEVICE_SERVICE_IRQ_VECTOR_ESI0 (0x2003) — second
       // clear write, sent only when the block read showed it nonzero
       // Read SYMBOL_ERROR_COUNT lanes 0-1 (0x210-0x213; per-lane 15-bit
       // count + bit15 validity) — the link-margin instrument (08-29)
       12'h1E0: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h1E1: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h1E2: begin aux_tx_data <= 8'h10; aux_tx_wr_en <= 1'b1; end
       12'h1E3: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end

       12'h1D0: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h1D1: begin aux_tx_data <= 8'h20; aux_tx_wr_en <= 1'b1; end
       12'h1D2: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h1D3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h1D4: begin aux_tx_data <= irq_clear_byte; aux_tx_wr_en <= 1'b1; end

       // Resd lane align status for all four lanes
       12'h100: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
       12'h101: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h102: begin aux_tx_data <= 8'h04; aux_tx_wr_en <= 1'b1; end
       12'h103: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end

       // Turn off training patterns / Switch to normal
       12'h110: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h111: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h112: begin aux_tx_data <= 8'h02; aux_tx_wr_en <= 1'b1; end
       12'h113: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h114: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end  // Scrambler enabled

       // Set Premp level 0, votage 0.4V
       // Write DPCD powerstate D0 (0x600 = 0x01): spec-mandated wake
       // before link config. The stock flow never sent it (live-verified
       // by the TUSB1046A AUX snooper: SET_POWER_STATE stayed 00); a sink
       // that idles in low power after Alt-Mode entry ignores training
       // until woken. Also doubles as the wire-verification loop: after
       // this lands, mux reg 0x12[6:5] must read 01.
       12'h130: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h131: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end
       12'h132: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h133: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h134: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end

       12'h140: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h141: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h142: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h143: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h144: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h145: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h146: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
       12'h147: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end

       // Set Premp level 0, votage 0.6V
       12'h160: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h161: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h162: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h163: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h164: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h165: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h166: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h167: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end

       // Set Premp level 0, votage 0.8V  -- Max voltage
       12'h180: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h181: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h182: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h183: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h184: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end
       12'h185: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end
       12'h186: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end
       12'h187: begin aux_tx_data <= 8'h06; aux_tx_wr_en <= 1'b1; end

	   	   // Set Premp level 1, votage 0.4V
       12'h240: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h241: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h242: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h243: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h244: begin aux_tx_data <= 8'h10; aux_tx_wr_en <= 1'b1; end
       12'h245: begin aux_tx_data <= 8'h10; aux_tx_wr_en <= 1'b1; end
       12'h246: begin aux_tx_data <= 8'h10; aux_tx_wr_en <= 1'b1; end
       12'h247: begin aux_tx_data <= 8'h10; aux_tx_wr_en <= 1'b1; end

       // Set Premp level 1, votage 0.6V
       12'h260: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h261: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h262: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h263: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h264: begin aux_tx_data <= 8'h11; aux_tx_wr_en <= 1'b1; end
       12'h265: begin aux_tx_data <= 8'h11; aux_tx_wr_en <= 1'b1; end
       12'h266: begin aux_tx_data <= 8'h11; aux_tx_wr_en <= 1'b1; end
       12'h267: begin aux_tx_data <= 8'h11; aux_tx_wr_en <= 1'b1; end

       // Set Premp level 1, votage 0.8V  -- Max voltage
       12'h280: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h281: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h282: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h283: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h284: begin aux_tx_data <= 8'h16; aux_tx_wr_en <= 1'b1; end
       12'h285: begin aux_tx_data <= 8'h16; aux_tx_wr_en <= 1'b1; end
       12'h286: begin aux_tx_data <= 8'h16; aux_tx_wr_en <= 1'b1; end
       12'h287: begin aux_tx_data <= 8'h16; aux_tx_wr_en <= 1'b1; end
	   
		// Set Premp level 2, votage 0.4V	-- Max pre-emphasis
       12'h340: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h341: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h342: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h343: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h344: begin aux_tx_data <= 8'h60; aux_tx_wr_en <= 1'b1; end
       12'h345: begin aux_tx_data <= 8'h60; aux_tx_wr_en <= 1'b1; end
       12'h346: begin aux_tx_data <= 8'h60; aux_tx_wr_en <= 1'b1; end
       12'h347: begin aux_tx_data <= 8'h60; aux_tx_wr_en <= 1'b1; end

       // Set Premp level 2, votage 0.6V
       12'h360: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h361: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h362: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h363: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h364: begin aux_tx_data <= 8'h61; aux_tx_wr_en <= 1'b1; end
       12'h365: begin aux_tx_data <= 8'h61; aux_tx_wr_en <= 1'b1; end
       12'h366: begin aux_tx_data <= 8'h61; aux_tx_wr_en <= 1'b1; end
       12'h367: begin aux_tx_data <= 8'h61; aux_tx_wr_en <= 1'b1; end

       // Set Premp level 2, votage 0.8V  -- Max voltage   -- Max pre-emphasis   
	   // this condition should not be reached for the standard do not allow this pair
       12'h380: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
       12'h381: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
       12'h382: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h383: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
       12'h384: begin aux_tx_data <= 8'h66; aux_tx_wr_en <= 1'b1; end
       12'h385: begin aux_tx_data <= 8'h66; aux_tx_wr_en <= 1'b1; end
       12'h386: begin aux_tx_data <= 8'h66; aux_tx_wr_en <= 1'b1; end
       12'h387: begin aux_tx_data <= 8'h66; aux_tx_wr_en <= 1'b1; end

       default: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b0; end
    endcase

    //--------------------------
    // Move on to the next word?
    //--------------------------
    if(counter[3:0] == 4'hF) begin
       busy <= 1'b0;
    end else begin
       counter <= counter+1;
    end

    //--------------------------------------
    // Are we being asked to send a message?
    //
    // But only do it of we are not already
    // sending something!
    //--------------------------------------
    if(msg_de == 1'b1 && counter[3:0] == 4'hF) begin
       counter <= {msg, 4'b0000};
       busy    <= 1'b1;
    end
end 

endmodule
