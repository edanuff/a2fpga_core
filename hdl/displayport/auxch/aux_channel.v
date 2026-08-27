///////////////////////////////////////////////////////////////////////////////
// ./src/auxch/aux_channel.v : 
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

// BLIND_SINK: open-loop link policy for boards whose AUX RECEIVE path is
// electrically dead but whose transmit works (a2mega 1.0a3: AUX is
// AC-coupled with bias only on the mux side; a DP AUX reply's <=1.38 Vpp
// swing can never cross an LVCMOS33 input threshold, and the board cannot
// be field-modified). Every DPCD WRITE still goes out on the wire — the
// sink gets configured normally — but the FSM advances on a timer instead
// of reply bytes, reply timeouts do not reset it, training status reads
// are assumed good (fixed dwell per pattern), and the sink is assumed to
// match the source's lane count and rate. Costs: no EDID, no per-lane
// EQ/swing adaptation, no link-quality re-check. Default 0 = spec flow.
module aux_channel #(
    parameter LINK_RATE_MBPS = 2700,
    parameter BLIND_SINK = 0,
    // HPD_DISCONNECT_RESETS: in closed-loop mode (BLIND_SINK=0), restart
    // the ladder from reset when hotplug_decode drops `present` (>=2 ms
    // HPD low = disconnect per spec). ⚠ ROW-72 VERDICT: DEFAULT OFF —
    // hubs FLAP HPD while their downstream monitor sleeps, and each
    // flap resets the ladder = reset storm, dark until monitor wake
    // (the old plow-through ladder had video ready at wake; plausibly
    // the retracted hybrid's actual mechanism, row 51). Re-enable only
    // after the flap-tolerant redesign (sustained-disconnect-only).
    parameter HPD_DISCONNECT_RESETS = 0,
    // M5: 1 = closed-loop TX-AFE adjust — TRAINING_LANEx_SET declares the
    // registered train_set_byte (msg 0x19) instead of the fixed 0x06
    // constant (msg 0x18). 0 = legacy, byte-identical.
    parameter AFE_ADJUST = 0,
    // GATE GRACE PERIOD (08-24, promoted from the observation-window
    // diagnostic after hardware evidence): for GATE_GRACE_CLKS after the
    // link enters the established set, a failing periodic check is
    // recorded but NOT acted on — the ladder stays established and keeps
    // polling. Grounds (acquisition_matrix_results.md, n=2 identical bad
    // cycles): the DP->HDMI converter reports regressed lane status
    // (0x202=01/00, INTERLANE_ALIGN 0, LINK_STATUS_UPDATED set) for ~7 s
    // after first qualifying while the picture is demonstrably stable;
    // teardown+retrain during that phase fixes nothing and produced the
    // 6-blink acquisition storms. The window is BOUNDED: after it, the
    // 08-18 load-bearing behavior (teardown -> recovery-by-retrain, the
    // Anker lesson) applies unchanged, so a genuine persistent loss still
    // retrains — at most delayed by the grace period after an establish.
    // Default 0 = byte-identical legacy behavior.
    parameter GATE_GRACE = 0,
    parameter GATE_GRACE_CLKS = 30'd800_000_000, // ~8 s @ 100 MHz
    // DARK-STATE KICK (08-24, replaces an ACCIDENTAL mechanism): the A/B
    // showed the converter can wedge into "perfect lanes, SINK_STATUS=0,
    // dark" (C:8177/K:00) and that the late-reply teardown BUG was what
    // had been rescuing it — a ladder teardown arriving during the wedge
    // entry re-engages the sink, while the watchdog's PHY cold restart is
    // hardware-refuted (7 attempts, zero effect). With the bug fixed
    // (drained), this provides the same rescue DELIBERATELY: if the link
    // is established but the sink has reported not-streaming for
    // KICK_CLKS, tear down and retrain. Budgeted (KICK_CAP, re-armed
    // whenever streaming is observed) so a truly dead sink does not
    // retrain forever. Closed-loop only: blind builds cannot read
    // SINK_STATUS. Default 0 = legacy.
    parameter GATE_KICK  = 0,
    parameter KICK_CLKS  = 30'd250_000_000,      // ~2.5 s @ 100 MHz
    parameter [2:0] KICK_CAP = 3'd7,
    // IRQ SERVICE (08-26, conformance fix): react to the sink's HPD IRQ
    // pulse with an IMMEDIATE status read (instead of the 1 Hz poll), and
    // acknowledge DEVICE_SERVICE_IRQ_VECTOR (0x201) by writing the read
    // value back (write-1s-to-clear). We NEVER did either: hpd_irq was a
    // dangling input and the 0x201 byte was read and discarded — while
    // every frozen-wedge snapshot showed LINK_STATUS_UPDATED latched with
    // the sink's MCU waiting for service (IT6563: the Attention->IRQ chain
    // works end-to-end through the VL103/PD/ESP32 and died at this input).
    // Conformant sources (Macs) service IRQs within 100 ms and never drive
    // this hub into its wedge states. Default 0 = legacy for other builds.
    // IRQ_SERVICE values: 0 = off (legacy), 1 = legacy-0x201 write-back,
    // 2 = ESI (08-26, wire-proven): the Anker NEVER raises 0x201 — the
    // Mac answer-key services exclusively via the ESI space (RD 0x2003
    // len13, W1C 0x2005), including an UNCONDITIONAL 0x02 ack during
    // every attach. Mode 2 mirrors that: attach-time ack after
    // read_registers, and hpd_irq-triggered service from the established
    // set (status check first, then ESI read + clear).
    parameter IRQ_SERVICE = 0,
    // Late-reply drain (08-24) is now parameterized so a build can carry
    // the LEGACY error behavior exactly (round-1 comparison baseline).
    parameter LATE_REPLY_DRAIN = 0,
    // POLITE ATTACH (08-26, from the Mac<->Anker answer key): the hub
    // raises HPD ~257-435 ms before it can train (SINK_COUNT=0, CR
    // impossible) and our legacy ladder hammered ~1150 transactions into
    // that window (383 CR polls, each REWRITING an unchanged 0x103) —
    // the strongest induced-wedge candidate on record; the hub visibly
    // DEFERred under the load. The Mac never hits the window: ~200 ms of
    // caps/EDID traffic first, lane writes once per phase. This
    // parameter adds, together: (a) a sink-present gate — after the
    // presence read, if the SINK_COUNT count field is 0, wait ~21 ms and
    // re-read instead of training into a sinkless branch; (b) the EDID
    // preamble — segment write + 8 block reads with paced DEFER retries
    // (the Anker answers on the 3rd try; a defer budget abandons EDID
    // for strict converters without blocking video); (c) lane-set
    // write-on-change — TRAINING_LANEx_SET is rewritten only when the
    // value changes, not every poll; (d) a slower CR poll cadence
    // (~330 us extra dwell per wait state). Default 0 = legacy.
    parameter POLITE_ATTACH = 0,
    parameter [5:0] EDID_DEFER_CAP = 6'd40
)(
        input        clk,
        // ready-to-send TRAINING_LANEx_SET values from afe_adjust_seq,
        // per lane: [7:0] lane 0 (0x103), [15:8] lane 1 (0x104)
        // (tie to 16'h0606 when AFE_ADJUST == 0)
        input  [15:0] train_set_byte,
        // M5: hold the NEXT lane-set message while the AFE sequencer is
        // applying (or evaluating) a request, so the declared levels are
        // in effect on the wire before the sink reads them. Tie 1'b0
        // when AFE_ADJUST == 0.
        input        afe_busy,
        output [7:0] debug_pmod,  // = ladder FSM state (see localparams)
        output [7:0] debug_gate,  // {locks@check_wait[3:0], gate_fails[1:0], timeouts[1:0]}
        // TEARDOWN ATTRIBUTION (08-24): the two counters above are 2 bits
        // and WRAP, so a run of ~7 link teardowns cannot be split between
        // the check_wait gate and AUX timeouts. These are the same two
        // events counted 4-bit SATURATING, purely for diagnosis.
        // [11:8] = sticky mask of WHICH lock bits were CLEAR at a FAILING
        //          check_wait evaluation, OR-accumulated over all failures:
        //          {clock, equ, symbol, align}. debug_gate's locks field
        //          latches EVERY evaluation, so it shows the last (passing)
        //          one and cannot name the culprit.
        // [15:12] = NON-STICKY mask of the FIRST failing evaluation only —
        //           the sticky OR below cannot distinguish one all-zero
        //           failure from several hierarchical single-bit failures
        //           (CR loss forces EQ/symbol clear by mask construction).
        output [15:0] debug_teardown, // {first_mask, fail_mask, gate_fail_sat, timeout_sat}
        // teardown-relevant AUX reply errors during the periodic check
        // (state == check_link): short replies and non-ACK replies both
        // jump straight to `error` and were previously UNCOUNTED — credible
        // sources of teardowns the gate/timeout counters cannot see.
        // teardown-reason counters, all scoped to the ESTABLISHED SET
        // (link_established / check_link / check_wait):
        // {short_reply, non_ACK, other/unattributed, observe_suppressed,
        //  dark_kicks}
        output [23:0] debug_aux_err,  // + [3:0] = IRQ services performed
        // CENTRALIZED first-teardown detail latch (latched at the first
        // transition to `error` from the established set since config):
        // {reason[3:0], from_state[7:0], expected[7:0], rx_byte_count[7:0]}
        // reason: 1=short reply, 2=non-ACK, 3=gate, 4=AUX timeout, 5=retry
        output [27:0] debug_err_detail,
        // 1-clk pulse on a failing check_wait evaluation (snapshot trigger)
        output reg   gate_fail_evt,
        // completed periodic status reads: increments when the 0x204 byte
        // of a check_link read is delivered (freshness token)
        output reg [3:0] status_seq,
        output [7:0] debug_sink,  // DPCD 0x205 SINK_STATUS (latched each status read)
        output [15:0] debug_rx,   // = {last byte, sync hits, rx bytes} from aux_interface
        //------------------------------
        output reg   edid_de,
        output reg   dp_reg_de,
        output reg   adjust_de,
        output reg   status_de,
        output reg   [7:0] aux_addr,
        output reg   [7:0] aux_data,
        //------------------------------
        input  [2:0] link_count,
        //----------------------------
        input        hpd_irq,
        input        hpd_present,
        //------------------------------
        output reg  tx_powerup,
        output reg  tx_clock_train,
        output reg  tx_align_train,
        output reg  tx_link_established,
        //-------------------------------
        input       swing_0p4,
        input       swing_0p6,
        input       swing_0p8,
        input       preemp_0p0,
        input       preemp_3p5,
        input       preemp_6p0,
        input       clock_locked,
        input       equ_locked,
        input       symbol_locked,
        input       align_locked,
        //------------------------------
        input       dp_tx_hp_detect,
        input       aux_in,
        output      aux_out,
        output      aux_tri
    );

    localparam [7:0] error = 8'h00, reset = 8'h01, check_presence = 8'h02;

    // Gathering Display information 
    localparam [7:0] edid_block0     = 8'h03, edid_block1    = 8'h04, edid_block2 = 8'h05, edid_block3 = 8'h06;
    localparam [7:0] edid_block4     = 8'h08, edid_block5    = 8'h09, edid_block6 = 8'h0A, edid_block7 = 8'h0B;

    // Gathering display Port information
    localparam [7:0] read_sink_count = 8'h0C, read_registers = 8'h0D;

    // Link configuration states 
    localparam [7:0] set_channel_coding = 8'h0E, set_speed_270    = 8'h0F, set_downspread   = 8'h10;
    localparam [7:0] set_link_count_1   = 8'h11, set_link_count_2 = 8'h12, set_link_count_4 = 8'h13;

    // Link training - clock recovery
    localparam [7:0] clock_training = 8'h14, clock_voltage_0p4 = 8'h15, clock_voltage_0p6 = 8'h16, clock_voltage_0p8 = 8'h17;
    localparam [7:0] clock_wait     = 8'h18, clock_test        = 8'h19, clock_adjust      = 8'h1A, clock_wait_after  = 8'h1B;

    // Link training - alignment and preemphasis
    localparam [7:0] align_training = 8'h1C; 
    localparam [7:0] align_p0_V0p4 = 8'h1D, align_p0_V0p6 = 8'h1E, align_p0_V0p8    = 8'h1F;
    localparam [7:0] align_p1_V0p4 = 8'h20, align_p1_V0p6 = 8'h21, align_p1_V0p8    = 8'h22;
    localparam [7:0] align_p2_V0p4 = 8'h23, align_p2_V0p6 = 8'h24, align_p2_V0p8    = 8'h25;
    localparam [7:0] align_wait0   = 8'h26, align_wait1   = 8'h27, align_wait2      = 8'h28, align_wait3 = 8'h29;
    localparam [7:0] align_test    = 8'h2A,  align_adjust = 8'h2B, align_wait_after = 8'h2C;   

    // Link up.
    localparam [7:0] switch_to_normal = 8'h2D, link_established = 8'h2E;

    // DPCD power state D0 wake (inserted before link configuration)
    localparam [7:0] set_power_d0 = 8'h31;
    localparam [7:0] irq_clear    = 8'h32;

    // POLITE_ATTACH states (unreachable when the parameter is 0)
    localparam [7:0] presence_eval = 8'h33;  // branch on the sink-count byte
    localparam [7:0] presence_wait = 8'h34;  // ~21 ms, then re-read presence
    localparam [7:0] edid_addr     = 8'h35;  // I2C segment/offset write (msg 0x01)
    localparam [7:0] defer_wait    = 8'h36;  // ~650 us pacing before a DEFER retry
    // ESI service states. Attach-time and runtime variants are separate
    // states because their TX flags differ: the runtime pair must HOLD
    // tx_link_established (the stream is live), the attach pair must not.
    localparam [7:0] esi_read      = 8'h37, esi_clear    = 8'h38;
    localparam [7:0] esi_read_rt   = 8'h39, esi_clear_rt = 8'h3A;

    // Checking the state of the link
    localparam [7:0] check_link = 8'h2F, check_wait = 8'h30;
                    
    reg  [7:0]  state            = error;
    reg  [7:0]  next_state       = error;
    reg  [7:0]  state_on_success = error;
    reg         retry_now;
    initial begin gate_fail_evt = 1'b0; status_seq = 4'd0; end
    reg  [28:0] retry_count;
    reg         link_check_now;
    reg [26:0]  link_check_count;
    reg [14:0]  count_100us;
    
    reg       adjust_de_active;
    reg       dp_reg_de_active;
    reg       edid_de_active;
    reg       status_de_active;
    reg       msg_de;
    reg [7:0] msg;
    wire      msg_busy;

    wire       aux_tx_wr_en;
    wire [7:0] aux_tx_data;
    wire       aux_tx_full;

    wire       aux_rx_rd_en;
    wire [7:0] aux_rx_data;
    wire       aux_rx_empty;

    reg [7:0] link_count_sink;
    
    wire      channel_busy;
    wire      channel_timeout;
    
    reg [7:0] expected;
    reg [7:0] rx_byte_count;
    reg [7:0] aux_addr_i;
    reg reset_addr_on_change;
    
    reg       just_read_from_rx;
    reg  [3:0] powerup_mask;

    // Per-state dwell timer (blind mode). 2026-08-14: widened from bit 17
    // (~1.3 ms @100 MHz) to bit 23 (~84 ms) after the AD3 AUX decode showed
    // the sink ACKing every write yet LANE0_1_STATUS stuck at CR_DONE=0 —
    // the 1.3 ms sprint through the training states gave the sink no time
    // to lock TPS1 before we advanced to scrambled video. ~84 ms per state
    // is deep inside every sink's training budget; the full ladder still
    // completes in ~2 s after HPD.
    reg [23:0] blind_dwell = 24'd0;

    assign debug_pmod = state;

    // BLIND_SINK: assume training succeeded (no status reads possible)
    wire clock_locked_i  = clock_locked  | (BLIND_SINK != 0);

    // Instrument (2026-08-15): the lock signals are decode-time pulses —
    // sample them exactly when the check_wait gate evaluates, and count
    // the two possible established-loop reset causes separately.
    reg [3:0] dbg_gate_locks = 4'd0;
    reg [1:0] dbg_gate_fail  = 2'd0;
    reg [3:0] dbg_gate_fail_sat = 4'd0;   // saturating twin, for attribution
    reg [3:0] dbg_timeout_sat   = 4'd0;
    reg [3:0] dbg_fail_mask     = 4'd0;   // sticky OR over all failures
    reg [3:0] dbg_first_mask    = 4'd0;   // the FIRST failure only
    reg       first_latched     = 1'b0;
    reg [3:0] dbg_short_sat     = 4'd0;   // short replies (established set)
    reg [3:0] dbg_nack_sat      = 4'd0;   // non-ACK replies (established set)
    reg [3:0] dbg_other_sat     = 4'd0;   // error entries w/o a tagged reason
    reg [3:0] dbg_obs_sat       = 4'd0;   // gate failures SUPPRESSED (observe)
    reg [3:0] dbg_kick_sat      = 4'd0;   // dark-state kicks fired
    reg [29:0] kick_timer       = 30'd0;
    reg [2:0]  kick_cnt         = 3'd0;   // budget used since last streaming
    reg [7:0]  irq_vec_r        = 8'd0;   // last nonzero 0x201 read
    reg [7:0]  irq_clear_byte_r = 8'd0;   // value being written back
    reg        irq_pending      = 1'b0;   // hpd_irq seen; service due
    reg [3:0]  dbg_irq_sat      = 4'd0;   // IRQ services performed
    // POLITE_ATTACH / ESI state (all inert at legacy defaults)
    reg [7:0]  sink_count_r     = 8'd0;   // presence-read count byte
    reg [7:0]  defer_pend_state = 8'd0;   // state to re-enter after defer_wait
    reg [5:0]  defer_cnt        = 6'd0;   // DEFERs taken this attach
    reg        edid_giveup      = 1'b0;   // defer budget exhausted: skip EDID
    reg        lane_set_sent    = 1'b0;   // a lane-set write went out this training
    reg [15:0] lane_set_last    = 16'd0;  // the value it carried
    reg [7:0]  esi_2005_r       = 8'd0;   // latched LINK_SERVICE_IRQ_VECTOR_ESI0
    reg        irq_service_due  = 1'b0;   // hpd_irq consumed; ESI service owed
    reg        err_evt          = 1'b0;   // tagged error-entry event
    reg [3:0]  err_reason       = 4'd0;
    reg [7:0]  err_from         = 8'd0;
    reg [7:0]  err_exp          = 8'd0;
    reg [7:0]  err_rxc          = 8'd0;
    reg [27:0] err_detail       = 28'd0;
    reg        err_detail_v     = 1'b0;
    reg [29:0] obs_timer        = 30'd0;  // time in the established set
    reg [7:0]  state_d          = 8'd0;
    reg        inest_d          = 1'b0;
    wire in_established_set = (state == link_established) ||
                              (state == check_link) || (state == check_wait) ||
                              (state == irq_clear) ||
                              (state == esi_read_rt) || (state == esi_clear_rt);
    wire obs_active = (GATE_GRACE != 0) && (obs_timer < GATE_GRACE_CLKS);
    reg [1:0] dbg_timeouts   = 2'd0;
    assign debug_gate     = {dbg_gate_locks, dbg_gate_fail, dbg_timeouts};
    assign debug_teardown = {dbg_first_mask, dbg_fail_mask, dbg_gate_fail_sat, dbg_timeout_sat};
    assign debug_aux_err  = {dbg_short_sat, dbg_nack_sat, dbg_other_sat,
                             dbg_obs_sat, dbg_kick_sat, dbg_irq_sat};
    assign debug_err_detail = err_detail;
    assign debug_sink = dbg_sink_status;
    // DPCD 0x205 SINK_STATUS (byte index 5 of the 0x200-0x207 status
    // read): bit0/1 = RECEIVE_PORT_0/1 "sink is receiving a valid main
    // stream". Splits dark-screen-with-solid-link: 0 = our MSA/stream
    // rejected (source side); 1 = sink sees the stream, its output stage
    // is the problem (converter/HDCP side).
    reg [7:0] dbg_sink_status = 8'd0;
    wire equ_locked_i    = equ_locked    | (BLIND_SINK != 0);
    wire symbol_locked_i = symbol_locked | (BLIND_SINK != 0);
    wire align_locked_i  = align_locked  | (BLIND_SINK != 0);
  

initial begin
    state            = error;
    next_state       = error;
    state_on_success = error;
    retry_now        = 1'b0;
    retry_count      = 29'h0200;
    link_check_now   = 1'b0;
    link_check_count = 27'h0200;
    count_100us      = 15'd1000; 

    adjust_de_active     = 1'b0;
    dp_reg_de_active     = 1'b0;
    edid_de_active       = 1'b0;
    status_de_active     = 1'b0;
    msg_de               = 1'b0;
    msg                  = 8'b0;
    link_count_sink      = 8'b0;
    expected             = 8'b0;
    rx_byte_count        = 8'b0;
    aux_addr_i           = 8'b0;
    reset_addr_on_change = 1'b0;

    just_read_from_rx    = 1'b0;
    powerup_mask         = 4'b0;

    edid_de             = 1'b0;
    dp_reg_de           = 1'b0;
    adjust_de           = 1'b0;
    status_de           = 1'b0;
    aux_addr            = 8'b0;
    aux_data            = 8'b0;
    tx_powerup          = 1'b0;
    tx_clock_train      = 1'b0;
    tx_align_train      = 1'b0;
    tx_link_established = 1'b0;
end

    // M5: which TRAINING_LANEx_SET message the set states send
    wire [7:0] lane_set_msg = (AFE_ADJUST != 0) ? 8'h19 : 8'h18;
    // M5 hold: the pending transition would issue a lane-set message and
    // the AFE sequencer has not finished applying the levels it declares.
    // Holding here (before the state change) leaves every other path of
    // the FSM untouched: no message is sent, so no reply/timeout logic
    // runs; the 100 us timer is re-armed each held cycle.
    wire afe_hold = (AFE_ADJUST != 0) && afe_busy && (next_state != state) && (
        next_state == clock_voltage_0p4 || next_state == clock_voltage_0p6 || next_state == clock_voltage_0p8 ||
        next_state == align_p0_V0p4 || next_state == align_p0_V0p6 || next_state == align_p0_V0p8 ||
        next_state == align_p1_V0p4 || next_state == align_p1_V0p6 || next_state == align_p1_V0p8 ||
        next_state == align_p2_V0p4 || next_state == align_p2_V0p6 || next_state == align_p2_V0p8);

dp_aux_messages #(.LINK_RATE_MBPS(LINK_RATE_MBPS)) i_aux_messages(
         .clk          (clk),
         .train_set_byte (train_set_byte),
         .irq_clear_byte (irq_clear_byte_r),
         // Interface to send messages
         .msg_de       (msg_de),
         .msg          (msg),
         .busy         (msg_busy),
         // Interface to the AUX Channel
         .aux_tx_wr_en (aux_tx_wr_en),
         .aux_tx_data  (aux_tx_data)
     );

aux_interface #(
           // Blind mode: replies never come; don't hold `busy` 20 ms per
           // transaction (sim-caught watchdog collision). 799 ticks = 400 us.
           .REPLY_TIMEOUT_TICKS(BLIND_SINK != 0 ? 16'd799 : 16'd39999)
       ) i_aux_interface(
           .clk         (clk),
           .debug_pmod  (debug_rx),   // {sync hits, rx bytes}
            //---------------------------
            .aux_in     (aux_in),
            .aux_out    (aux_out),
            .aux_tri    (aux_tri),
            //----------------------------
           .tx_wr_en    (aux_tx_wr_en),
           .tx_data     (aux_tx_data),
           .tx_full     (aux_tx_full),
           //------------------------------
           .rx_rd_en    (aux_rx_rd_en),
           .rx_data     (aux_rx_data),
           .rx_empty    (aux_rx_empty),
           //------------------------------
           .busy        (channel_busy),
           .abort       (1'b0),   
           .timeout     (channel_timeout)
    );

    assign aux_rx_rd_en = (!channel_busy) & (!aux_rx_empty);  // CHECK THIS!
      
always @(posedge clk) begin
    //-----------------------------------------
    // Are we going to change state this cycle?
    //-----------------------------------------
    msg_de <= 1'b0;
    gate_fail_evt <= 1'b0;
    err_evt <= 1'b0;
    // observation-window timer: time spent continuously in the
    // established set (saturating; resets on leaving the set)
    if (in_established_set) begin
        if (obs_timer != 30'h3FFFFFFF) obs_timer <= obs_timer + 30'd1;
    end else
        obs_timer <= 30'd0;
    // centralized first-teardown detail (from the established set).
    // The per-reason counters are ALSO incremented here — once per tagged
    // event — instead of at the sites: the short-read site evaluates per
    // received byte and over-counted (accounting discrepancy, 08-24).
    if (err_evt) begin
        if (!err_detail_v) begin
            err_detail_v <= 1'b1;
            err_detail   <= {err_reason, err_from, err_exp, err_rxc};
        end
        if (err_reason == 4'd1 && dbg_short_sat != 4'hF)
            dbg_short_sat <= dbg_short_sat + 4'd1;
        if (err_reason == 4'd2 && dbg_nack_sat != 4'hF)
            dbg_nack_sat <= dbg_nack_sat + 4'd1;
    end
    // OTHER: an entry into `error` from the established set that no site
    // tagged — the safety net that catches teardown paths the reason
    // sites miss (the reason this latch is centralized at all)
    state_d  <= state;
    inest_d  <= in_established_set;
    if (state == error && state_d != error && inest_d && !err_evt) begin
        if (dbg_other_sat != 4'hF)
            dbg_other_sat <= dbg_other_sat + 4'd1;
        if (!err_detail_v) begin
            err_detail_v <= 1'b1;
            err_detail   <= {4'd0, state_d, expected, rx_byte_count};
        end
    end
    // freshness token: the 0x204 byte of a status read was just delivered
    if (status_de == 1'b1 && aux_addr == 8'd4)
        status_seq <= status_seq + 4'd1;
     
    if(next_state != state && !afe_hold) begin
        //-----------------------------------------------------------
        // Get ready to count how many reply bytes have been received
        //-----------------------------------------------------------
        rx_byte_count <= 0;
        
        //-------------------------------------------------
        // Controlling which FSM state to go to on success
        //-------------------------------------------------
        case(next_state)
            reset:              state_on_success <= check_presence;
            // Legacy: EDID skipped (DDC-class, converters DEFER it).
            // POLITE: evaluate the count byte first — presence_eval
            // routes to the paced re-read (count 0), the EDID preamble,
            // or straight on (EDID given up).
            check_presence:     state_on_success <= (POLITE_ATTACH != 0)
                                                    ? presence_eval : read_sink_count;
            presence_eval:      state_on_success <= (sink_count_r[5:0] == 6'd0)
                                                    ? presence_wait
                                                    : (edid_giveup ? read_sink_count
                                                                   : edid_addr);
            presence_wait:      state_on_success <= check_presence;
            edid_addr:          state_on_success <= edid_block0;
            defer_wait:         state_on_success <= defer_pend_state;
            esi_read:           state_on_success <= esi_clear;
            esi_clear:          state_on_success <= set_power_d0;
            esi_read_rt:        state_on_success <= esi_clear_rt;
            esi_clear_rt:       state_on_success <= link_established;
            edid_block0:        state_on_success <= edid_block1;
            edid_block1:        state_on_success <= edid_block2;
            edid_block2:        state_on_success <= edid_block3;
            edid_block3:        state_on_success <= edid_block4;
            edid_block4:        state_on_success <= edid_block5;
            edid_block5:        state_on_success <= edid_block6;
            edid_block6:        state_on_success <= edid_block7;
            edid_block7:        state_on_success <= read_sink_count;
            read_sink_count:    state_on_success <= read_registers;
            // ESI mode: the Mac's unconditional attach-time ESI ack goes
            // here — between the caps read and the D0 wake.
            read_registers:     state_on_success <= (IRQ_SERVICE == 2)
                                                    ? esi_read : set_power_d0;
            set_power_d0:       state_on_success <= set_channel_coding;
            set_channel_coding: state_on_success <= set_speed_270;                        
            set_speed_270:      state_on_success <= set_downspread;                        
            set_downspread:     case(link_count)
                                           3'b001:  state_on_success <= set_link_count_1;                        
                                           3'b010:  state_on_success <= set_link_count_2;                        
                                           3'b100:  state_on_success <= set_link_count_4;
                                           default: state_on_success <= error;
                                       endcase
            set_link_count_1:   state_on_success <= clock_training; 
            set_link_count_2:   state_on_success <= clock_training; 
            set_link_count_4:   state_on_success <= clock_training; 
            //----- Display Port clock training -------------------                        
            clock_training:     state_on_success <= clock_voltage_0p4;
            clock_voltage_0p4:  state_on_success <= clock_wait;
            clock_voltage_0p6:  state_on_success <= clock_wait;
            clock_voltage_0p8:  state_on_success <= clock_wait;
            clock_wait:         state_on_success <= clock_test;                        
            clock_test:         state_on_success <= clock_adjust;
            clock_adjust:       state_on_success <= clock_wait_after;
            clock_wait_after:   if(clock_locked_i == 1'b1) begin
                                    state_on_success <= align_training;
                                // POLITE: write-on-change — the wire showed
                                // 383 rewrites of an UNCHANGED 0x103 per
                                // attach; skip the write and just poll again
                                // when the value we'd send is what the sink
                                // already has.
                                end else if(POLITE_ATTACH != 0 && lane_set_sent &&
                                            lane_set_last == train_set_byte) begin
                                    state_on_success <= clock_wait;
                                end else if(swing_0p8 == 1'b1) begin
                                    state_on_success <= clock_voltage_0p8;
                                end else if(swing_0p6 == 1'b1) begin
                                    state_on_success <= clock_voltage_0p6;
                                end else begin
                                    state_on_success <= clock_voltage_0p4;
                                end
            //----- Display Port Alignment traning ------------                        
            align_training:     if(POLITE_ATTACH != 0 && lane_set_sent &&
                                   lane_set_last == train_set_byte) begin
                                     state_on_success <= align_wait0;
                                end else if(swing_0p8 == 1'b1) begin
                                     state_on_success <= align_p0_V0p8;
                                end else if(swing_0p6 == 1'b1) begin
                                     state_on_success <= align_p0_V0p6;
                                end else begin
                                     state_on_success <= align_p0_V0p4;
                                end
            align_p0_V0p4:      state_on_success <= align_wait0;
            align_p0_V0p6:      state_on_success <= align_wait0;
            align_p0_V0p8:      state_on_success <= align_wait0;
            align_p1_V0p4:      state_on_success <= align_wait0;
            align_p1_V0p6:      state_on_success <= align_wait0;
            align_p1_V0p8:      state_on_success <= align_wait0;
            align_p2_V0p4:      state_on_success <= align_wait0;
            align_p2_V0p6:      state_on_success <= align_wait0;
            align_p2_V0p8:      state_on_success <= align_wait0;
            align_wait0:        state_on_success <= align_wait1;                        
            align_wait1:        state_on_success <= align_wait2;                        
            align_wait2:        state_on_success <= align_wait3;                        
            align_wait3:        state_on_success <= align_test;                        
            align_test:         state_on_success <= align_adjust;                        
            align_adjust:       state_on_success <= align_wait_after;
            align_wait_after:   if(symbol_locked_i == 1'b1) begin
                                           state_on_success <= switch_to_normal;
                                end else if(POLITE_ATTACH != 0 && lane_set_sent &&
                                            lane_set_last == train_set_byte) begin
                                    state_on_success <= align_wait0;
                                end else if(swing_0p8 == 1'b1) begin
                                    if(preemp_6p0 == 1'b1) begin
                                        state_on_success <= align_p2_V0p8;
                                    end else if(preemp_3p5 == 1'b1) begin
                                        state_on_success <= align_p1_V0p8;
                                    end else begin
                                        state_on_success <= align_p0_V0p8;
                                    end
                                end else if(swing_0p6 == 1'b1) begin
                                    if(preemp_6p0 == 1'b1) begin
                                        state_on_success <= align_p2_V0p6;
                                    end else if(preemp_3p5 == 1'b1) begin
                                        state_on_success <= align_p1_V0p6;
                                    end else begin
                                        state_on_success <= align_p0_V0p6;
                                    end
                                end else begin
                                    if(preemp_6p0 == 1'b1) begin
                                        state_on_success <= align_p2_V0p4;
                                    end else if(preemp_3p5 == 1'b1) begin
                                        state_on_success <= align_p1_V0p4;
                                    end else begin
                                        state_on_success <= align_p0_V0p4;
                                    end 
                                end                        
            switch_to_normal:   state_on_success <= link_established;  
            link_established:   state_on_success <= link_established;
            check_link:         state_on_success <= check_wait;
            irq_clear:          state_on_success <= link_established;
            check_wait:         begin
                                dbg_gate_locks <= {clock_locked_i, equ_locked_i, symbol_locked_i, align_locked_i};
                                // (check-non-fatal diagnostic reverted 08-18 night: it
                                // answered the Ugreen question — genuine sink loss — and
                                // then BLOCKED the Anker path's recovery-by-retrain on
                                // marginal instances. Teardown/retrain is load-bearing.)
                                if(clock_locked_i == 1'b1 && equ_locked_i == 1'b1 && symbol_locked_i == 1'b1 && align_locked_i == 1'b1) begin
                                    state_on_success <= (IRQ_SERVICE == 2 && irq_service_due)
                                                        ? esi_read_rt :
                                                        (IRQ_SERVICE == 1 && irq_vec_r != 8'h00)
                                                        ? irq_clear : link_established;
                                end else if (obs_active) begin
                                    // GRACE PERIOD: record, do NOT tear
                                    // down; stay established and keep
                                    // polling (status stays visible in C:
                                    // at the poll cadence).
                                    if (dbg_obs_sat != 4'hF)
                                        dbg_obs_sat <= dbg_obs_sat + 4'd1;
                                    state_on_success <= link_established;
                                    dbg_fail_mask <= dbg_fail_mask |
                                        ~{clock_locked_i, equ_locked_i,
                                          symbol_locked_i, align_locked_i};
                                    if (!first_latched) begin
                                        first_latched  <= 1'b1;
                                        dbg_first_mask <= ~{clock_locked_i, equ_locked_i,
                                                            symbol_locked_i, align_locked_i};
                                    end
                                    gate_fail_evt <= 1'b1;
                                end else begin
                                    dbg_gate_fail    <= dbg_gate_fail + 2'd1;
                                    if (dbg_gate_fail_sat != 4'hF)
                                        dbg_gate_fail_sat <= dbg_gate_fail_sat + 4'd1;
                                    err_evt <= 1'b1; err_reason <= 4'd3;
                                    err_from <= state; err_exp <= expected;
                                    err_rxc <= rx_byte_count;
                                    // remember WHICH bits were clear (sticky OR)
                                    dbg_fail_mask <= dbg_fail_mask |
                                        ~{clock_locked_i, equ_locked_i,
                                          symbol_locked_i, align_locked_i};
                                    if (!first_latched) begin
                                        first_latched  <= 1'b1;
                                        dbg_first_mask <= ~{clock_locked_i, equ_locked_i,
                                                            symbol_locked_i, align_locked_i};
                                    end
                                    gate_fail_evt <= 1'b1;
                                    state_on_success <= error;
                                end
                                end
            error:              state_on_success <= error;
        endcase

        //----------------------------------------------------------
        // Controlling what message will be sent, how many words are 
        // expected back, and where it will be routed
        //
        // NOTE: If you set 'expected' incorrectly then bytes will
        //       get left in the RX FIFO, potentially corrupting things
        //----------------------------------------------------------
        msg_de               <= 1'b1;
        if (next_state == irq_clear) begin
            irq_clear_byte_r <= irq_vec_r;   // stable for the message TX
            irq_vec_r        <= 8'h00;
            if (dbg_irq_sat != 4'hF)
                dbg_irq_sat <= dbg_irq_sat + 4'd1;
        end
        // ESI clears: attach-time = the Mac's unconditional 0x02 ack;
        // runtime = the latched vector (0x02 fallback if it read zero)
        if (next_state == esi_clear) begin
            irq_clear_byte_r <= 8'h02;
            if (dbg_irq_sat != 4'hF)
                dbg_irq_sat <= dbg_irq_sat + 4'd1;
        end
        if (next_state == esi_clear_rt) begin
            irq_clear_byte_r <= (esi_2005_r != 8'h00) ? esi_2005_r : 8'h02;
            esi_2005_r       <= 8'h00;
            if (dbg_irq_sat != 4'hF)
                dbg_irq_sat <= dbg_irq_sat + 4'd1;
        end
        if (next_state == esi_read_rt)
            irq_service_due <= 1'b0;
        // lane-set write tracking (POLITE write-on-change)
        if (next_state == clock_voltage_0p4 || next_state == clock_voltage_0p6 ||
            next_state == clock_voltage_0p8 ||
            next_state == align_p0_V0p4 || next_state == align_p0_V0p6 ||
            next_state == align_p0_V0p8 ||
            next_state == align_p1_V0p4 || next_state == align_p1_V0p6 ||
            next_state == align_p1_V0p8 ||
            next_state == align_p2_V0p4 || next_state == align_p2_V0p6 ||
            next_state == align_p2_V0p8) begin
            lane_set_sent <= 1'b1;
            lane_set_last <= train_set_byte;
        end
        if (next_state == reset || next_state == clock_training) begin
            lane_set_sent <= 1'b0;
        end
        if (next_state == reset) begin
            defer_cnt       <= 6'd0;
            edid_giveup     <= 1'b0;
            irq_service_due <= 1'b0;
        end
        status_de_active     <= 1'b0;
        adjust_de_active     <= 1'b0;
        dp_reg_de_active     <= 1'b0;
        edid_de_active       <= 1'b0;
        reset_addr_on_change <= 1'b0;                
        case(next_state)
            reset:                begin msg <= 8'h00; expected <= 8'h00; end
            // 2026-08-15: presence via NATIVE DPCD read (msg 0x03 = sink
            // count @ 0x200), not the I2C address-phase to 0x50 — DP->HDMI
            // converters DEFER all DDC-class traffic indefinitely while
            // their HDMI side settles, pinning the ladder at step one.
            check_presence:       begin msg <= 8'h03; expected <= 8'h02; reset_addr_on_change <= 1'b1; end
            presence_eval:        begin msg <= 8'h00; expected <= 8'h00; end
            presence_wait:        begin msg <= 8'h00; expected <= 8'h00; end
            edid_addr:            begin msg <= 8'h01; expected <= 8'h01; end
            defer_wait:           begin msg <= 8'h00; expected <= 8'h00; end
            esi_read:             begin msg <= 8'h1B; expected <= 8'h0E; end
            esi_clear:            begin msg <= 8'h1C; expected <= 8'h01; end
            esi_read_rt:          begin msg <= 8'h1B; expected <= 8'h0E; end
            esi_clear_rt:         begin msg <= 8'h1C; expected <= 8'h01; end

            edid_block0:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block1:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block2:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block3:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block4:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block5:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block6:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block7:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
                    
            read_sink_count:      begin msg <= 8'h03; expected <= 8'h02; reset_addr_on_change <= 1'b1; end
            read_registers:       begin msg <= 8'h04; expected <= 8'h0D; dp_reg_de_active <= 1'b1; end
            set_power_d0:         begin msg <= 8'h13; expected <= 8'h01; end
            set_channel_coding:   begin msg <= 8'h06; expected <= 8'h01;  end
            set_speed_270:        begin msg <= 8'h07; expected <= 8'h01;  end
            set_downspread:       begin msg <= 8'h08; expected <= 8'h01;  end
            set_link_count_1:     begin msg <= 8'h09; expected <= 8'h01;  end
            set_link_count_2:     begin msg <= 8'h0A; expected <= 8'h01;  end
            set_link_count_4:     begin msg <= 8'h0B; expected <= 8'h01;  end
                    
            clock_training:       begin msg <= 8'h0C; expected <= 8'h01;  end
            // BLIND_SINK: TRAINING_LANEx_SET must DECLARE what the TX
            // actually drives — the sink calibrates against it and stalls
            // while its ADJUST_REQUEST goes unanswered (live-hit 2026-08-14:
            // monitor requested 0x22 all training while we declared level 0;
            // CR never completed). The GTR12 is fixed at 804 mV = DP swing
            // level 2, so blind mode always sends the 0p8/max message
            // (0x06/lane = level 2 + MAX_SWING_REACHED).
            // TRUTHFUL DECLARATIONS (2026-08-18): the GTR12 drive is fixed
            // (804/900 mV = DP swing level 2, preemp 0) — the closed-loop
            // ladder used to walk declared levels 0->1->2 while the analog
            // never moved, the exact declared-vs-actual mismatch the blind
            // path already fixed ("sink calibrates against it and stalls",
            // live-hit 08-14). Declare the truth (msg 0x18 = level 2 +
            // MAX_SWING_REACHED, preemp 0) in EVERY set state; the ladder
            // keeps its state walk for pacing/retry structure.
            clock_voltage_0p4:    begin msg <= lane_set_msg; expected <= 8'h01; end
            clock_voltage_0p6:    begin msg <= lane_set_msg; expected <= 8'h01; end
            clock_voltage_0p8:    begin msg <= lane_set_msg; expected <= 8'h01; end
            clock_wait:           begin msg <= 8'h00; expected <= 8'h00;  reset_addr_on_change <= 1'b1; end
            clock_test:           begin msg <= 8'h0D; expected <= 8'h09;  status_de_active <= 1'b1; reset_addr_on_change <= 1'b1; end
            clock_adjust:         begin msg <= 8'h0E; expected <= 8'h03;  adjust_de_active <= 1'b1; end
            clock_wait_after:     begin msg <= 8'h00; expected <= 8'h00;  end
                    
            align_training:       begin msg <= 8'h0F; expected <= 8'h01; end
            // Truthful declarations here too: we never drive preemp — the
            // p1/p2 messages (0x24..0x38) declared pre-emphasis the analog
            // doesn't produce.
            align_p0_V0p4:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p0_V0p6:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p0_V0p8:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p1_V0p4:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p1_V0p6:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p1_V0p8:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p2_V0p4:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p2_V0p6:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_p2_V0p8:        begin msg <= lane_set_msg; expected <= 8'h01; end
            align_wait0:          begin msg <= 8'h00; expected <= 8'h00; end
            align_wait1:          begin msg <= 8'h00; expected <= 8'h00; end
            align_wait2:          begin msg <= 8'h00; expected <= 8'h00; end
            align_wait3:          begin msg <= 8'h00; expected <= 8'h00;  reset_addr_on_change <= 1'b1; end
            align_test:           begin msg <= 8'h0D; expected <= 8'h09;  status_de_active <= 1'b1; reset_addr_on_change <= 1'b1; end
            align_adjust:         begin msg <= 8'h0E; expected <= 8'h03;  adjust_de_active <= 1'b1; end
            align_wait_after:     begin msg <= 8'h00; expected <= 8'h00; end
            switch_to_normal:     begin msg <= 8'h11; expected <= 8'h01; end
            link_established:     begin msg <= 8'h00; expected <= 8'h00; reset_addr_on_change <= 1'b1; end
            check_link:           begin msg <= 8'h0D; expected <= 8'h09; status_de_active <= 1'b1;  end
            check_wait:           begin msg <= 8'h00; expected <= 8'h00; end
            // write the latched vector back to 0x201 (ACK reply = 1 byte)
            irq_clear:            begin msg <= 8'h1A; expected <= 8'h01; end
            error:                begin msg <= 8'h00; end
            default:              begin msg <= 8'h00; end
        endcase

        //------------------------------------------------------
        // Set the control signals the state for the link state,  
        // transceivers andmain channel pipeline 
        //------------------------------------------------------
        tx_powerup          <= 1'b0; 
        tx_clock_train      <= 1'b0; 
        tx_align_train      <= 1'b0; 
        tx_link_established <= 1'b0;
        case(next_state)
            clock_training:       begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_voltage_0p4:    begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_voltage_0p6:    begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_voltage_0p8:    begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_wait:           begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_test:           begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_adjust:         begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
            clock_wait_after:     begin tx_powerup <= 1'b1; tx_clock_train <= 1'b1; end
                    
            align_training:       begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p0_V0p4:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p0_V0p6:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p0_V0p8:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p1_V0p4:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p1_V0p6:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p1_V0p8:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p2_V0p4:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p2_V0p6:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_p2_V0p8:        begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_wait0:          begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_wait1:          begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_wait2:          begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_wait3:          begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_test:           begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_adjust:         begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_wait_after:     begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            switch_to_normal:     begin tx_powerup <= 1'b1; end
            link_established:     begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            check_link:           begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            check_wait:           begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            irq_clear:            begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            // runtime ESI service: the stream is live — hold the TX up
            esi_read_rt:          begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            esi_clear_rt:         begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
        endcase
    end

    //------------------------------------------------------
    // Manage the small timer that counts how long we have 
    // been in the current state (used for implementing 
    // short waits for some FSM states) 
    //------------------------------------------------------
    if(state == next_state) begin
        count_100us <= count_100us - 1;
    end else begin
        count_100us <= 15'd9999;                                        
        if(reset_addr_on_change == 1'b1 && !afe_hold) begin
            aux_addr_i <= 8'h0;
        end                                       
    end
    if(!afe_hold)
        state <= next_state;
            
    //-----------------------------------------------------------
    // How a short wait is implemented...
    //
    // Has the 100us pause expired, when no data was expected?
    // If so, move to the next test.            
    //-----------------------------------------------------------
    // RACE FIX (08-26, found in sim while adding IRQ service): this timer
    // advance must only fire from a SETTLED state. `count_100us` free-runs
    // and WRAPS while sitting in link_established (reload happens only on
    // a transition), so its bit14 is stale-high ~50% of the time; firing
    // during the one-cycle transition INTO check_link (old expected==0,
    // old state_on_success==link_established) bounced the FSM straight
    // back out before the status read ran — periodic link checks silently
    // skipped on a coin flip. This is the mechanism behind the dark-state
    // G:00/N:000 anomaly (checks "not running" for minutes).
    // The paced wait states advance on their own (longer) dwells below —
    // exclude them here. POLITE also stretches the two CR wait states by
    // ~330 us each (blind_dwell[15]) so the CR poll loop runs at ~1 ms
    // instead of ~650 us.
    if(state == next_state && expected == 8'h00 && count_100us[14] == 1'b1 &&
       state != presence_wait && state != defer_wait &&
       (POLITE_ATTACH == 0 ||
        !(state == clock_wait || state == clock_wait_after) ||
        blind_dwell[15] == 1'b1)) begin
        next_state <= state_on_success;
    end
    // presence_wait: ~21 ms between sink-count re-reads while the branch
    // reports no sink (blind_dwell counts cycles-in-state unconditionally)
    if(state == next_state && state == presence_wait && blind_dwell[21] == 1'b1) begin
        next_state <= state_on_success;
    end
    // defer_wait: ~650 us before re-sending a DEFERred request (the hub
    // ACKs the Mac's EDID reads on the 3rd try at this pacing)
    if(state == next_state && state == defer_wait && blind_dwell[16] == 1'b1) begin
        next_state <= state_on_success;
    end

    //-----------------------------------------------------------
    // BLIND_SINK: reply bytes will never arrive. Advance once the
    // request has fully left the wire (channel idle) and a dwell
    // has elapsed — the sink still RECEIVED the message; we simply
    // do not wait to hear back. The dwell is ~1.3 ms (not the
    // 100 us wait-state timer): with no status readback, each
    // training pattern must persist long enough for a slow sink to
    // adapt; the whole walk stays ~60 ms, inside the watchdog.
    //-----------------------------------------------------------
    if(state == next_state) begin
        blind_dwell <= blind_dwell + 24'd1;
    end else begin
        blind_dwell <= 24'd0;
    end
    if(BLIND_SINK != 0 && channel_busy == 1'b0 && blind_dwell[23] == 1'b1) begin
        next_state <= state_on_success;
    end
            
    //------------------------------------------------------------
    // Processing the data that has been received from the sink
    // over the AUX channel. The data bytes are just streamed out
    // to a downstream component that uses the values, and may 
    // set flags that feed back in to control the FSM.
    //------------------------------------------------------------
    edid_de    <= 1'b0;
    adjust_de  <= 1'b0;
    dp_reg_de  <= 1'b0;                                
    status_de  <= 1'b0;
    if(channel_busy == 1'b0) begin
        if(just_read_from_rx == 1'b1) begin
            // Is this a short read? ONLY meaningful when a reply is
            // actually outstanding: with expected==0 the expected-1
            // comparison wraps to 0xFF and a LATE reply spilling into
            // link_established/check_wait became a spurious teardown
            // (caught on hardware: B: reason=short from 0x2E, expected=0,
            // rx=8 — a stale check_link reply). With expected==0 the
            // bytes are simply DRAINED (aux_rx_rd_en already empties the
            // FIFO) and ignored.
            if((LATE_REPLY_DRAIN == 0 || expected != 8'h00) && rx_byte_count != expected-1 && aux_rx_empty == 1'b1) begin
                next_state <= error;
                if (in_established_set) begin
                    err_evt <= 1'b1; err_reason <= 4'd1;
                    err_from <= state; err_exp <= expected;
                    err_rxc <= rx_byte_count;
                end
            end
                                
            if(rx_byte_count == 8'h00) begin
                //------------------------------------------------
                // Is the Ack missing? This doesn't work correctly
                // if only byte is expected, as it gets overwritten 
                // by the following 'if' statement.
                //
                // Do not change this behaviour, by what it should do
                // is test for "In progress" or "Again" requests, and 
                // retry the current operation.
                //---------------------------------------------------- 
                if((LATE_REPLY_DRAIN == 0 || expected != 8'h00) && aux_rx_data != 8'h00) begin
                    next_state <= error;
                    if (in_established_set) begin
                        err_evt <= 1'b1; err_reason <= 4'd2;
                        err_from <= state; err_exp <= expected;
                        err_rxc <= rx_byte_count;
                    end
                end
                if(rx_byte_count == expected-1 && aux_rx_empty == 1'b1) begin
                    next_state <= state_on_success;
                end
                //--------------------------------------------
                // Has the Sink indicated that we should retry
                // the current command, to allow the sink time
                // to process the request?
                //
                // This only works if there is just one byte
                // in the FIFO. This only works for DPCD
                // transactions that aeert "AUX DEFER"
                //--------------------------------------------
                if(aux_rx_data == 8'h20) begin
                    if (POLITE_ATTACH != 0) begin
                        // Paced retry (~650 us, the Mac's cadence) instead
                        // of an immediate resend. EDID retries draw from a
                        // per-attach budget; exhausting it abandons EDID
                        // (strict-converter fallback) rather than blocking
                        // the attach.
                        if (defer_cnt != 6'h3F)
                            defer_cnt <= defer_cnt + 6'd1;
                        if ((state == edid_addr ||
                             (state >= edid_block0 && state <= edid_block7)) &&
                            defer_cnt >= EDID_DEFER_CAP) begin
                            edid_giveup <= 1'b1;
                            next_state  <= read_sink_count;
                        end else begin
                            defer_pend_state <= state;
                            next_state       <= defer_wait;
                        end
                    end else begin
                        // legacy: just flip states to force a retry.
                        state      <= state_on_success;
                        next_state <= state;
                    end
                end
            end else begin
                //-----------------------------------------------------------------
                // Process a non-ack data byte, routing it out using the DE signals
                //-----------------------------------------------------------------
                edid_de    <= edid_de_active;
                adjust_de  <= adjust_de_active;
                dp_reg_de  <= dp_reg_de_active;                                
                status_de  <= status_de_active;                                

                aux_data   <= aux_rx_data;
                aux_addr   <= aux_addr_i;
                aux_addr_i <= aux_addr_i+1;
                if(status_de_active == 1'b1 && aux_addr_i == 8'd5)
                    dbg_sink_status <= aux_rx_data;
                // DEVICE_SERVICE_IRQ_VECTOR: previously read and DISCARDED
                if(status_de_active == 1'b1 && aux_addr_i == 8'd1 && aux_rx_data != 8'h00)
                    irq_vec_r <= aux_rx_data;
                // POLITE: the presence read's count byte (data byte 1)
                if(state == check_presence && rx_byte_count == 8'h01)
                    sink_count_r <= aux_rx_data;
                // ESI block read: data byte 3 = 0x2005 (0x2003 is byte 1)
                if((state == esi_read || state == esi_read_rt) &&
                   rx_byte_count == 8'h03)
                    esi_2005_r <= aux_rx_data;
                        
                if(rx_byte_count == expected-1 && aux_rx_empty == 1'b1) begin
                    next_state <= state_on_success;
                    if(reset_addr_on_change == 1'b1) begin
                        aux_addr_i <= 8'h00; 
                    end
                end
            end
        end
    end

    //---------------------------------------------------
    // Manage the AUX channel timeout and the retry to  
    // establish a link. 
    //-----------------------------------------------------------                            
    //    if channel_timeout = 1'b1 or (state /= reset and state /= link_established and retry_now = 1'b1) then
    // (BLIND_SINK: reply timeouts are the EXPECTED outcome of every
    // transaction — they must not reset the FSM. The periodic retry_now
    // watchdog is kept in both modes.)
    if (BLIND_SINK == 0 && channel_timeout == 1'b1) begin
        dbg_timeouts <= dbg_timeouts + 2'd1;
        if (dbg_timeout_sat != 4'hF)
            dbg_timeout_sat <= dbg_timeout_sat + 4'd1;
    end
    if((BLIND_SINK == 0 && channel_timeout == 1'b1) ||
                                  (state != reset      && state != link_established &&
                                   state != check_link && state != check_wait       &&
                                   state != irq_clear  &&
                                   state != esi_read_rt && state != esi_clear_rt &&
                                   retry_now == 1'b1)) begin
        next_state <= reset;
        state      <= error;
        if (in_established_set) begin
            err_evt <= 1'b1;
            err_reason <= (BLIND_SINK == 0 && channel_timeout == 1'b1) ? 4'd4 : 4'd5;
            err_from <= state; err_exp <= expected; err_rxc <= rx_byte_count;
        end
    end
    
    //-----------------------------------------------
    // If the link was established, then every
    // now and then check the state of the link
    //-----------------------------------------------
    if (IRQ_SERVICE != 0 && hpd_irq)
        irq_pending <= 1'b1;
    if(state == link_established &&
       (link_check_now == 1'b1 || (IRQ_SERVICE != 0 && irq_pending))) begin
        next_state  <= check_link;
        // ESI mode: remember an hpd_irq-triggered check owes an ESI
        // service pass after the status gate (0x201 is never raised by
        // ESI-only branches, so the legacy vector latch cannot trigger it)
        if (IRQ_SERVICE == 2 && irq_pending)
            irq_service_due <= 1'b1;
        irq_pending <= 1'b0;
    end

    //---------------------------------------------------------------
    // BLIND_SINK: hold the ladder until a sink is actually present.
    // The spec flow was implicitly gated by "the sink replies"; with
    // replies assumed, the ladder would otherwise walk to
    // link_established against an empty connector — and a monitor
    // attached later would never see the training patterns it needs
    // (live-hit on board #1: led2/3 asserted on a PC with no sink).
    // Holding here also restarts training from scratch whenever HPD
    // drops and returns.
    //---------------------------------------------------------------
    if((BLIND_SINK != 0 || HPD_DISCONNECT_RESETS != 0) &&
       hpd_present == 1'b0) begin
        next_state <= reset;
        state      <= error;
    end

    //-----------------------------------------------
    // If the full message has been received, then 
    // read any waiting data out of the FIFO.
    // Also update the count of bytes read.
    //-----------------------------------------------
    if(channel_busy == 1'b0 && aux_rx_empty == 1'b0) begin
        just_read_from_rx <= 1'b1;
    end else begin
        just_read_from_rx <= 1'b0;
    end
    if(just_read_from_rx == 1'b1) begin
        rx_byte_count <= rx_byte_count+1;
    end

    //---------------------------------------
    // Manage the reset timer
    //---------------------------------------
    if(retry_count == 0) begin
        retry_now   <= 1'b1;
        // Blind mode walks the ladder at ~84 ms/state (~2.1 s total) —
        // the 0.5 s watchdog reset every walk forever (live-hit
        // 2026-08-14: D cycling 02..06). 4 s keeps the watchdog while
        // clearing the full walk with 2x margin.
        retry_count <= (BLIND_SINK != 0) ? 29'd399999999 : 29'd49999999;
    end else begin
        retry_now   <= 1'b0;
        retry_count <= retry_count - 1;
    end
    if(link_check_count == 0) begin
        link_check_now   <= 1'b1;
        // PPS actually became a 2Hz pulse....
        link_check_count <= 27'd99999999;
    end else begin
        link_check_now   <= 1'b0;
        link_check_count <= link_check_count - 1;
    end

    //-----------------------------------------------------------
    // DARK-STATE KICK (see the parameter comment). Placed LAST in
    // this block so its teardown wins the nonblocking ordering.
    // dbg_sink_status[1:0] is DPCD 0x205 bits[1:0], latched at each
    // periodic status read (~1 Hz); a healthy link refreshes it to
    // non-zero well inside KICK_CLKS.
    //-----------------------------------------------------------
    if (dbg_sink_status[1:0] != 2'b00) begin
        kick_timer <= 30'd0;
        kick_cnt   <= 3'd0;              // streaming observed: re-arm budget
    end else if (!in_established_set) begin
        kick_timer <= 30'd0;             // timer runs only while established
    end else if (GATE_KICK != 0 && BLIND_SINK == 0 && kick_cnt != KICK_CAP) begin
        if (kick_timer >= KICK_CLKS) begin
            kick_timer <= 30'd0;
            kick_cnt   <= kick_cnt + 3'd1;
            if (dbg_kick_sat != 4'hF)
                dbg_kick_sat <= dbg_kick_sat + 4'd1;
            next_state <= reset;
            state      <= error;
            err_evt    <= 1'b1; err_reason <= 4'd6;
            err_from   <= state; err_exp <= expected;
            err_rxc    <= rx_byte_count;
        end else
            kick_timer <= kick_timer + 30'd1;
    end
end        
endmodule
