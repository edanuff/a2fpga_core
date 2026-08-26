///////////////////////////////////////////////////////////////////////////////
// ./src/auxch/channel_managemnt.v : 
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

module channel_management #(
    parameter LINK_RATE_MBPS = 2700,
    parameter BLIND_SINK = 0,  // open-loop link policy (see aux_channel.v)
    parameter HPD_DISCONNECT_RESETS = 0, // closed-loop HPD-drop reset — DEFAULT OFF (row 72 flap storms; see aux_channel.v)
    parameter AFE_ADJUST = 0   // M5: closed-loop TX-AFE adjust (aux_channel.v)
)(
        input  clk100,
        // M5 runtime AFE adjust: applied-level declaration in, one pulse
        // out per received ADJUST_REQUEST 0x206 byte (afe_adjust_seq's
        // evaluation/debounce tick). train_set_byte is PER LANE: [7:0]
        // lane 0 (0x103), [15:8] lane 1 (0x104); tie 16'h0606 when off.
        input  [15:0] train_set_byte,
        input        afe_busy,      // M5: hold next lane-set while applying (tie 0 when off)
        output       adjust_evt,
        output [7:0] debug,
        output [15:0] debug_rx,  // AUX RX: {last byte, sync hits, accepted bytes} (registered)
        output [3:0]  debug_locks, // {clock,equ,symbol,align}_locked (registered)
        output [7:0]  debug_gate,  // latched-at-gate locks + fail/timeout counters
        output [15:0]  debug_teardown, // {first_mask, fail_mask, gate_fail_sat, timeout_sat}
        output [7:0]  debug_sink,  // DPCD 0x205 SINK_STATUS
        output [15:0] debug_adjust, // raw sink ADJUST_REQUEST (0x206/0x207)
        output [23:0] debug_chstate, // raw DPCD {0x204, 0x203, 0x202}
        output [7:0]  debug_aux_err, // {short_reply_sat, nack_sat} in check_link
        // ATOMIC FIRST-FAILURE SNAPSHOT (second-opinion instrumentation,
        // 08-24): latched in the same clock as the FIRST failing check_wait
        // evaluation since config. {raw 0x204, 0x203, 0x202 at that moment,
        // status_seq (completed periodic status reads = freshness token),
        // time since the last link_established rise in 100 ms units,
        // saturating}. Decisive decode:
        //   0x202=77, 0x204[0]=0            -> initial-qualification mismatch
        //                                      (align never required at
        //                                      switch_to_normal, only at the
        //                                      periodic gate)
        //   one lane's CR/EQ/symbol missing -> genuine marginality
        //   raw all zero + fresh seq        -> sink/converter returned zero
        //   raw all zero + STALE seq        -> the stale-channel_state race
        output [31:0] debug_snapshot, // {chstate24, status_seq4, tsl4}
        output [7:0]  debug_caps,    // sink caps: {ext_framing, rate270,
                                     //  rate162, dp_valid, max_downspread[3:0]}

        input   hpd,
        input   auxch_in,
        output  auxch_out,
        output  auxch_tri,

        // Datapath requirements
        input  [2:0] stream_channel_count,
        input  [2:0] source_channel_count,

        // Datapath control
        output tx_clock_train,
        output tx_align_train,

        // Transceiver management
        output [3:0] tx_powerup_channel,

        output tx_preemp_0p0,
        output tx_preemp_3p5,
        output tx_preemp_6p0,
           
        output tx_swing_0p4,
        output tx_swing_0p6,
        output tx_swing_0p8,
          
        input  [3:0] tx_running,
        output tx_link_established,
        output hpd_present_out
    );

    wire       edid_de;
    wire       dp_reg_de;
    wire       adjust_de;
    wire       status_de;
    wire [7:0] aux_data;
    wire [7:0] aux_addr;
    wire       invalidate;
    wire       tx_powerup;
    
    wire       preemp_0p0_i;
    wire       preemp_3p5_i;
    wire       preemp_6p0_i;
           
    wire       swing_0p4_i;
    wire       swing_0p6_i;
    wire       swing_0p8_i;
    
    wire       support_RGB444;
    wire       support_YCC444;
    wire       support_YCC422;
    
    //------------------------------------------
    // EDID data
    //-------------------------------------------
    wire        edid_valid;
    wire [15:0] pixel_clock_x10k;
    
    wire [11:0] h_visible_len;
    wire [11:0] h_blank_len;
    wire [11:0] h_front_len;
    wire [11:0] h_sync_len;
    
    wire [11:0] v_visible_len;
    wire [11:0] v_blank_len;
    wire [11:0] v_front_len;
    wire [11:0] v_sync_len;
    wire        interlaced;
    //------------------------------------------
    // Display port data
    //-------------------------------------------
    wire        dp_valid;
    wire [7:0]  dp_revision;
    wire        dp_link_rate_2_70;
    wire        dp_link_rate_1_62;
    wire        dp_extended_framing;
    wire [3:0]  dp_link_count;
    wire [7:0]  dp_max_downspread;
    wire [7:0]  dp_coding_supported;
    wire [15:0] dp_port0_capabilities;
    wire [15:0] dp_port1_capabilities;
    wire [7:0]  dp_norp;
    //------------------------------------------------------------------------
    
    wire       clock_locked;
    wire       equ_locked;
    wire       symbol_locked;
    wire       align_locked;
    //----------------------------------------------
    // registered export: keeps debug plumbing off the RX engine's
    // timing-critical paths (PnR placed rx_r->aux_addr CE at -0.6 ns
    // when exported combinationally)
    wire [15:0] debug_rx_w;
    reg  [15:0] debug_rx_r = 16'd0;
    reg  [3:0]  debug_locks_r = 4'd0;
    always @(posedge clk100) begin
        debug_rx_r    <= debug_rx_w;
        debug_locks_r <= {clock_locked, equ_locked, symbol_locked, align_locked};
    end
    assign debug_rx = debug_rx_r;
    assign debug_locks = debug_locks_r;
    wire [7:0] interface_debug;
    wire [7:0] mgmt_debug;
   
    wire [2:0] sink_channel_count;
    wire [2:0] active_channel_count;

    wire       hpd_irq;
    wire       hpd_present;
    assign     hpd_present_out = hpd_present;


    // Feed the number of links from the registers into the link management
    // logic. BLIND_SINK: the DPCD capability read never yields data (AUX RX
    // dead) — assume the sink matches the source's lane count.
assign      sink_channel_count = (BLIND_SINK != 0) ? source_channel_count
                                                   : dp_link_count[2:0];
assign     tx_preemp_0p0 = preemp_0p0_i;
assign     tx_preemp_3p5 = preemp_3p5_i;
assign     tx_preemp_6p0 = preemp_6p0_i;
           
assign     debug_caps = {dp_extended_framing, dp_link_rate_2_70,
                         dp_link_rate_1_62, dp_valid, dp_max_downspread[3:0]};
assign     tx_swing_0p4 = swing_0p4_i;
assign     tx_swing_0p6 = swing_0p6_i;
assign     tx_swing_0p8 = swing_0p8_i;

hotplug_decode i_hotplug_decode(
        .clk     (clk100),
        .hpd     (hpd),
        .irq     (hpd_irq),
        .present (hpd_present)
    );

    // M5: the ADJUST_REQUEST lane0/1 byte (DPCD 0x206) is the aux_addr==0
    // beat of the adjust read — one pulse per training iteration.
    assign adjust_evt = adjust_de & (aux_addr == 8'h00);

    // ---- first-failure snapshot (see the port comment) ----------------
    wire       gate_fail_evt_w;
    wire [3:0] status_seq_w;
    reg [31:0] snap_r    = 32'd0;
    reg        snapped_r = 1'b0;
    // time since the last link_established rise, 100 ms units, saturating
    reg [3:0]  tsl_r     = 4'd0;
    reg [23:0] tsl_tick  = 24'd0;
    reg        est_d     = 1'b0;
    always @(posedge clk100) begin
        est_d <= tx_link_established;
        if (tx_link_established && !est_d) begin
            tsl_r <= 4'd0; tsl_tick <= 24'd0;
        end else if (tx_link_established) begin
            if (tsl_tick == 24'd9_999_999) begin   // 100 ms @ clk100
                tsl_tick <= 24'd0;
                if (tsl_r != 4'hF) tsl_r <= tsl_r + 4'd1;
            end else
                tsl_tick <= tsl_tick + 24'd1;
        end
        if (gate_fail_evt_w && !snapped_r) begin
            snapped_r <= 1'b1;
            snap_r    <= {debug_chstate, status_seq_w, tsl_r};
        end
    end
    assign debug_snapshot = snap_r;


aux_channel #(.LINK_RATE_MBPS(LINK_RATE_MBPS),
              .BLIND_SINK(BLIND_SINK),
              .HPD_DISCONNECT_RESETS(HPD_DISCONNECT_RESETS),
              .AFE_ADJUST(AFE_ADJUST)) i_aux_channel(
        .clk             (clk100),
        .train_set_byte  (train_set_byte),
        .afe_busy        (afe_busy),
        .debug_pmod      (debug),
        .debug_gate      (debug_gate),
        .debug_teardown  (debug_teardown),
        .debug_aux_err   (debug_aux_err),
        .gate_fail_evt   (gate_fail_evt_w),
        .status_seq      (status_seq_w),
        .debug_sink      (debug_sink),
        .debug_rx        (debug_rx_w),
         //------------------------------
        .edid_de         (edid_de),
        .dp_reg_de       (dp_reg_de),
        .adjust_de       (adjust_de),
        .status_de       (status_de),
        .aux_addr        (aux_addr),
        .aux_data        (aux_data),
         //----------------------------
        .link_count      (active_channel_count),
        .hpd_irq         (hpd_irq),
        .hpd_present     (hpd_present),
         //------------------------------
        .preemp_0p0      (preemp_0p0_i), 
        .preemp_3p5      (preemp_3p5_i),
        .preemp_6p0      (preemp_6p0_i),           
        .swing_0p4       (swing_0p4_i),
        .swing_0p6       (swing_0p6_i),
        .swing_0p8       (swing_0p8_i),
          
        .clock_locked    (clock_locked),
        .equ_locked      (equ_locked),
        .symbol_locked   (symbol_locked),
        .align_locked    (align_locked),
           
         //----------------------------
        .tx_powerup          (tx_powerup),
        .tx_clock_train      (tx_clock_train),
        .tx_align_train      (tx_align_train),
        .tx_link_established (tx_link_established),
         //----------------------------
        .dp_tx_hp_detect (hpd),
        .aux_in     (auxch_in),
        .aux_out    (auxch_out),
        .aux_tri    (auxch_tri)
     );


 edid_decode i_edid_decode( 
           .clk              (clk100),
           .edid_de          (edid_de),
           .edid_addr        (aux_addr),
           .edid_data        (aux_data),
           .invalidate       (1'b0),
    
           .valid            (edid_valid),
    
           .support_RGB444   (support_RGB444),
           .support_YCC444   (support_YCC444),
           .support_YCC422   (support_YCC422),
    
           .pixel_clock_x10k (pixel_clock_x10k),
    
           .h_visible_len    (h_visible_len),
           .h_blank_len      (h_blank_len),
           .h_front_len      (h_front_len),
           .h_sync_len       (h_sync_len),
    
           .v_visible_len    (v_visible_len),
           .v_blank_len      (v_blank_len),
           .v_front_len      (v_front_len),
           .v_sync_len       (v_sync_len),
           .interlaced       (interlaced));

dp_register_decode i_dp_reg_decode( 
            .clk                (clk100),
            .de                 (dp_reg_de),
            .addr               (aux_addr),
            .data               (aux_data),
            .invalidate         (1'b0),
            .valid              (dp_valid),
            
            .revision           (dp_revision),
            .link_rate_2_70     (dp_link_rate_2_70),
            .link_rate_1_62     (dp_link_rate_1_62),
            .extended_framing   (dp_extended_framing),
            .link_count         (dp_link_count),
            .max_downspread     (dp_max_downspread),
            .coding_supported   (dp_coding_supported),
            .port0_capabilities (dp_port0_capabilities),
            .port1_capabilities (dp_port1_capabilities),
            .norp               (dp_norp)
       );

link_signal_mgmt i_link_signal_mgmt(
        .mgmt_clk             (clk100),
        .debug_adjust         (debug_adjust),
        .debug_chstate        (debug_chstate),

        .tx_powerup           (tx_powerup), 
        
        .status_de            (status_de),
        .adjust_de            (adjust_de),
        .addr                 (aux_addr),
        .data                 (aux_data),

        .sink_channel_count   (sink_channel_count),
        .source_channel_count (source_channel_count),
        .active_channel_count (active_channel_count),
        .stream_channel_count (stream_channel_count),

        .powerup_channel      (tx_powerup_channel),

        .clock_locked         (clock_locked),
        .equ_locked           (equ_locked),
        .symbol_locked        (symbol_locked),
        .align_locked         (align_locked),

        .preemp_0p0           (preemp_0p0_i), 
        .preemp_3p5           (preemp_3p5_i),
        .preemp_6p0           (preemp_6p0_i),
            
        .swing_0p4            (swing_0p4_i),
        .swing_0p6            (swing_0p6_i),
        .swing_0p8            (swing_0p8_i)
    );

endmodule
