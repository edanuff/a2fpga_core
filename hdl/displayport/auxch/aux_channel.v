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
    parameter [5:0] EDID_DEFER_CAP = 6'd40,
    // Free-running retry watchdog: resets an unfinished attach ceremony.
    // 0.5 s was fine for monitors (~0.4 s to established); a DP->HDMI
    // converter that brings up its HDMI side first takes longer (a2p25
    // 10-12: cut at align_test every attempt).
    parameter [28:0] RETRY_WATCHDOG_CLKS = 29'd49999999,
    // EQ dwell: align_wait0-3 are 4 x 100 us (the DP default when DPCD
    // 0x00E TRAINING_AUX_RD_INTERVAL = 0). Sinks advertising 4 ms+ (DP->HDMI
    // converters, a2p25 10-12) need the status read held off that long;
    // reading early and re-issuing TPS2 restarts their equalizer forever.
    // Each wait state dwells max(EQ_WAIT_MULT, interval-derived) x 100 us;
    // the interval is latched from DPCD 0x00E during the capability read.
    parameter [5:0] EQ_WAIT_MULT = 6'd1,
    // TRAINING RECOVERY (08-28, from the 60K bisect + AD3 capture of the
    // step-6b stall): a garbled/short/lost reply to a TRAINING-phase read
    // used to route to `error`/reset — a FULL ceremony restart (presence
    // pacing + EDID + config, ~25 ms) that also POWER-CYCLES the TX
    // lanes, so the next status read again lands ~2 ms into a fresh TX
    // power-up transient and the attach loops forever (225 ceremonies,
    // ZERO second status polls on the wire; the 20.16 ms round gap = the
    // AUX reply timeout). The legacy ladder survived the same marginality
    // only because its restart was cheap and its TX stayed up. This
    // parameter (with POLITE_ATTACH) makes training-phase failures:
    //   1) retry the SAME read after a ~650 us pace (train_wait), TX
    //      flags HELD, from a per-pass budget (TRAIN_RETRY_CAP);
    //   2) on budget exhaustion, restart at clock_training (TX stays
    //      powered) instead of the full ceremony;
    // the 4 s retry watchdog remains the full-ceremony backstop, and
    // HPD loss still resets everything. Default 1 (inert when
    // POLITE_ATTACH=0).
    parameter TRAIN_RECOVER = 1,
    // SYMBOL-ERROR COUNTERS (08-29, review item): read DPCD 0x210-0x213
    // on every periodic link check and latch per-lane raw 16-bit values
    // (bit15 = validity, preserved). Failures of this instrumentation
    // read NEVER tear down the link — every error path skips to
    // check_wait. Default 0 = legacy byte-identical.
    parameter ERRCNT_READ = 0,
    // TRAIN_ERRCNT: read SYMBOL_ERROR_COUNT (0x210-0x213) after every EQ
    // status read (instrument: does the sink see symbol errors on TPS2?)
    parameter TRAIN_ERRCNT = 0,
    // LINK_CHECK: 1 (default) = periodic / hpd_irq status checks while the
    // link is established (spec flow; a failed check tears the link down).
    // 0 = once established, stay established until HPD drops or the
    // watchdog fires — bring-up aid for a marginal AUX RX path (a2p25
    // displayport1: training succeeds, established-state AUX transactions
    // intermittently NACK/time out and tore the link down every ~1 s).
    // 2 = OBSERVE: periodic checks run as in 1, but nothing in the
    // established set ever tears the link down — short replies, non-ACKs,
    // DEFERs, timeouts and failed lock gates are only counted (debug_obs)
    // and the FSM returns to link_established. Bring-up instrument for
    // measuring established-state AUX reliability with the picture up.
    parameter LINK_CHECK = 1,
    // Periodic established-state check interval in clk cycles (default
    // ~1 s at 100 MHz). a2p25 polls faster so a sink that drops CR after
    // establish (Sceptre: ~2 s and ~5 s in) is retrained promptly.
    parameter [26:0] LINK_CHECK_CLKS = 27'd99999999,
    // WEDGE-SUSPECT DETECTOR (advisory ONLY — no autonomous action in
    // this build; the ESP32 auto-replug policy arms separately after
    // bench validation). Fires when, within one established session:
    // the sink HAS streamed (ever_streamed — a link that never worked
    // can never trigger: bad-cable safe), then SINK_STATUS[1:0]==0 AND
    // 0x204 bit7 (LINK_STATUS_UPDATED) reads STUCK SET across polls
    // (healthy hubs clear it on read; the quiet-frozen hub's dead MCU
    // cannot) sustained for WEDGE_CLKS. Any teardown, flap, or status
    // change resets the timer (sleeping-monitor storms cannot
    // accumulate). UNPROVEN discriminator vs the kick-era
    // K:00-with-good-picture state — hence advisory-only.
    // Single-bit fire at [WEDGE_BIT] with a PRELOAD: the timer clears to
    // WEDGE_PRELOAD instead of 0, so it trips after 2^WEDGE_BIT - PRELOAD
    // clocks. 2^30 - 2^28 = 8.05 s @ 100 MHz — the cut from 10.7 s (ed:
    // 30 s recovery too long) while staying above the ~7 s settling-storm
    // ghost. Preload-not-AND because BOTH richer fire conditions (a >=
    // comparator, then a two-bit AND) re-rolled the cm_life placement
    // into violations (21 and 67/42 across seeds); the preload keeps the
    // fire logic bit-identical to the build that closed at 100.952.
    parameter WEDGE_BIT = 30,
    parameter [31:0] WEDGE_PRELOAD = 32'h1000_0000,
    // Lane-set write-on-change: HARDWARE-REFUTED as a default (7d6e205d,
    // 08-26 bench): the DP CR/EQ loops MANDATE a TRAINING_LANEx_SET write
    // every iteration even when unchanged, and the IT6563 uses that write
    // as its cue to evaluate CR — with the skip enabled the hub never
    // granted CR (C:0000 forever, watchdog loops). The Mac only looked
    // like write-once because its CR passed on the first poll. Kept as an
    // opt-in experiment knob ONLY. Default 0 = spec-faithful per-iteration
    // writes.
    parameter LANE_SET_WOC = 0
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
        // sticky OR of every ESI vector byte read ({0x2003, 0x2005}) and
        // the EDID defer bookkeeping ({giveup, defer_cnt}) — the two
        // observability gaps from the f0a48ae6 grading sessions
        output [15:0] debug_esi,
        output [6:0]  debug_defer,
        output        wedge_suspect_o,  // advisory wedge-suspect flag
        output [15:0] errcnt0_o,  // SYMBOL_ERROR_COUNT lane0 (raw, bit15=valid)
        output [15:0] errcnt1_o,  // SYMBOL_ERROR_COUNT lane1
        // CENTRALIZED first-teardown detail latch (latched at the first
        // transition to `error` from the established set since config):
        // {reason[3:0], from_state[7:0], expected[7:0], rx_byte_count[7:0]}
        // reason: 1=short reply, 2=non-ACK, 3=gate, 4=AUX timeout, 5=retry
        output [27:0] debug_err_detail,
        // LINK_CHECK==2 observe counters (16-bit, wrapping; zero otherwise):
        // {last_hdr[7:0], checks, short, nack, defer, timeout, gate}
        // checks = check_link reads issued; one error class counted per
        // read; last_hdr = reply header byte of the last non-ACK.
        output [103:0] debug_obs,
        // AUX interface liveness: {tx_state[2:0], rx_state[2:0], channel_busy,
        //  aux_rx_empty, expected[7:0], rx_byte_count[7:0]}
        output [23:0] debug_auxif,
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
        output reg   adjust_evt,   // adjust_de for data byte 0 (0x206), registered at the same
                                   // edge as adjust_de/aux_addr — replaces the combinational
                                   // "adjust_de & (aux_addr == 0)" decode that fed the AFE
                                   // sequencer's busy term and came back into afe_hold (round 2, cone a)
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
    // Second ESI clear: DEVICE_SERVICE_IRQ_VECTOR_ESI0 (0x2003) W1C when
    // the block read showed it nonzero — before this, a vector raised in
    // 0x2003 was read and left SET (the unserviced pathology one register
    // over from the one we fixed; candidate for the Anker's IRQ-silent
    // dark class if its events land there).
    localparam [7:0] esi_clear2    = 8'h3B, esi_clear2_rt = 8'h3C;
    localparam [7:0] esi_eval_rt   = 8'h3D;  // decide: clear or back to established
    // TRAIN_RECOVER: paced retry of a failed training-phase read. Unlike
    // defer_wait this state HOLDS the TX flags of the pending state —
    // dropping tx_powerup mid-CR is exactly the round-trip transient the
    // recovery exists to avoid.
    localparam [7:0] train_wait    = 8'h3E;
    localparam [7:0] errcnt_read   = 8'h3F;  // 0x210-0x213 read (in-established)
    localparam [7:0] errcnt_train  = 8'h40;  // same read inside the EQ loop (TRAIN_ERRCNT)
    localparam [3:0] TRAIN_RETRY_CAP = 4'd8;

    // Checking the state of the link
    localparam [7:0] check_link = 8'h2F, check_wait = 8'h30;
                    
    // syn_maxfan (timing campaign round 2, 138B durability): the encoded
    // state registers fan out to ~60 loads each (28 equality decodes,
    // three case blocks, the transition-pending compare). GowinSynthesis
    // duplicates the register to honour the limit — pure replication, no
    // logic change; the ladder's next_state cone was the clk100 floor on
    // both dies (+0.00 on the 138B gate, dominated by 2.3 ns of route off
    // the fanout-61 replica).
    reg  [7:0]  state            = error /* synthesis syn_maxfan = 12 */;
    reg  [7:0]  next_state       = error /* synthesis syn_maxfan = 12 */;
    reg  [7:0]  state_on_success = error;
    // (138B clk100 ladder, 2026-10-24) next_state's assignment sites,
    // grouped. The sites used to be ~15 independent `next_state <= ...`
    // statements whose textual order is their priority; synthesis built
    // that as one 2:1 mux per site in series, so the lowest-priority
    // sites (the wait advances) reached the register through 10 mux
    // levels (blind_dwell -> next_state D, 14 cells, -0.88 ns reported).
    // Each site now records its request in the temporaries of its group
    // (blocking, last writer within a group wins exactly as before), and
    // ONE statement at the end of the block applies the groups in the
    // original priority order: kick > hpd > link check > timeouts >
    // reply processing > wait advances. Same winner, same value, same
    // cycle, for every combination of sites.
    reg         ns_a_v, ns_b_v, ns_c_v, ns_d_v, ns_e_v, ns_f_v;
    reg  [7:0]  ns_b, ns_c;
    reg         retry_now;
    initial begin gate_fail_evt = 1'b0; status_seq = 4'd0; end
    reg  [28:0] retry_count;
    reg         retry_zero_r = 1'b0;      // == (retry_count == 0); initial 0x200
    localparam [28:0] RETRY_RELOAD = (BLIND_SINK != 0) ? 29'd399999999 : RETRY_WATCHDOG_CLKS;
    reg         link_check_now;
    reg [26:0]  link_check_count;
    reg         lc_zero_r = 1'b0;         // == (link_check_count == 0); initial 0x200
    reg [14:0]  count_100us;
    reg [6:0]   train_interval_r = 7'd0;   // DPCD 0x00E (latched per caps read)
    // effective per-align_wait-state multiplier (x100 us, 4 states):
    // the sink's interval (n x 4 ms -> n x 10 per state, capped at 16 ms)
    // or the EQ_WAIT_MULT floor, whichever is larger
    wire [5:0]  eq_int_mult_w = (train_interval_r == 7'd0) ? 6'd1 :
                                (train_interval_r >= 7'd4) ? 6'd40 : {train_interval_r[1:0], 1'b0} + {train_interval_r[1:0], 3'b0};  // n*10
    wire [5:0]  eq_mult_w     = (eq_int_mult_w > EQ_WAIT_MULT) ? eq_int_mult_w : EQ_WAIT_MULT;
    // Registered (timing): the multiplier only changes at the capability
    // read and blind_dwell counts cycles in state, so a one-cycle-late
    // "more dwell needed" flag is exact where it is consumed and keeps
    // the 7-bit compare out of the FSM's advance condition (-1.2 ns at
    // clk100 when it was combinational, a2p25 10-12).
    reg         is_align_wait_r   = 1'b0;   // state is align_wait0-3
    reg  [23:0] eq_dwell_cycles_r = 24'd10000;
    reg         eq_dwell_done_r   = 1'b0;   // blind_dwell reached the threshold
    // wait-advance pre-decodes (see the advance site for the exactness
    // argument): state / expected are stable for thousands of cycles
    // wherever the dwell bits that consume these are set
    reg         adv_ok_r = 1'b0;   // expected == 0 and not a paced wait state
    reg         adv_cr_r = 1'b0;   // state is clock_wait / clock_wait_after
    reg         adv_pw_r = 1'b0;   // state == presence_wait
    reg         adv_dw_r = 1'b0;   // state == defer_wait
    reg         adv_tw_r = 1'b0;   // state == train_wait
    always @(posedge clk) begin
        is_align_wait_r   <= (state == align_wait0 || state == align_wait1 ||
                              state == align_wait2 || state == align_wait3);
        eq_dwell_cycles_r <= {eq_mult_w, 4'd0} * 24'd625;   // mult x 10000 cycles (100 us @ 100 MHz)
        eq_dwell_done_r   <= (blind_dwell >= eq_dwell_cycles_r);
        adv_ok_r <= (expected == 8'h00) &&
                    state != presence_wait && state != defer_wait && state != train_wait &&
                    !(state == align_wait0 || state == align_wait1 ||
                      state == align_wait2 || state == align_wait3);
        adv_cr_r <= (state == clock_wait || state == clock_wait_after);
        adv_pw_r <= (state == presence_wait);
        adv_dw_r <= (state == defer_wait);
        adv_tw_r <= (state == train_wait);
    end
    
    reg       adjust_de_active;
    reg       dp_reg_de_active;
    reg       edid_de_active;
    reg       status_de_active;
    reg       msg_de;
    // syn_maxfan: the message engine now decodes its whole byte table from
    // msg at load time (dp_aux_messages.v) — ~90 decoders read these bits
    reg [7:0] msg /* synthesis syn_maxfan = 16 */;
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
    
    // (138B clk100 ladder, 2026-10-24) the reply-byte branch reads these
    // in every latch enable; replicated like the state registers
    reg [7:0] expected      /* synthesis syn_maxfan = 12 */;
    reg [7:0] rx_byte_count /* synthesis syn_maxfan = 12 */;
    // maintained at both writes of rx_byte_count (same sites, same
    // priority): rxc_nz_r == (rx_byte_count != 0), rxc_p1_r ==
    // rx_byte_count + 1 (mod 256), so `rx_byte_count == expected - 1`
    // is the plain equality `rxc_p1_r == expected` (same modular
    // identity) and the branch prefix `rx_byte_count != 0` is one flop.
    // Those two compares (8-bit decrement + equality, 8-bit OR) sat in
    // series with the busy/just-read prefix in front of ~40 reply-byte
    // latches (tx_empty -> esi_2005_r CE, +0.12 on the gate after the
    // ladder fixes).
    reg       rxc_nz_r = 1'b0;
    reg [7:0] rxc_p1_r = 8'd1;
    reg [7:0] aux_addr_i;
    reg reset_addr_on_change;
    
    reg       just_read_from_rx /* synthesis syn_maxfan = 12 */;
    reg  [3:0] powerup_mask;

    // Per-state dwell timer (blind mode). 2026-08-14: widened from bit 17
    // (~1.3 ms @100 MHz) to bit 23 (~84 ms) after the AD3 AUX decode showed
    // the sink ACKing every write yet LANE0_1_STATUS stuck at CR_DONE=0 —
    // the 1.3 ms sprint through the training states gave the sink no time
    // to lock TPS1 before we advanced to scrambled video. ~84 ms per state
    // is deep inside every sink's training budget; the full ladder still
    // completes in ~2 s after HPD.
    // (138B clk100 ladder, 2026-10-24) two halves with a maintained
    // "low half is all ones" flag, like wedge_timer: bit-identical count.
    reg [15:0] bd_lo       = 16'd0;
    reg [7:0]  bd_hi       = 8'd0;
    reg        bd_lo_max_r = 1'b0;        // == (bd_lo == 16'hFFFF)
    wire [23:0] blind_dwell = {bd_hi, bd_lo};

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
    reg        link_check_pend  = 1'b0;   // periodic check due (held while msg engine busy)
    reg [3:0]  dbg_irq_sat      = 4'd0;   // IRQ services performed
    // POLITE_ATTACH / ESI state (all inert at legacy defaults)
    reg [7:0]  sink_count_r     = 8'd0;   // presence-read count byte
    reg [7:0]  defer_pend_state = 8'd0;   // state to re-enter after defer_wait
    reg [5:0]  defer_cnt        = 6'd0;   // DEFERs taken this HPD session
    reg        edid_giveup      = 1'b0;   // defer budget exhausted: skip EDID
    // EDID SESSION POLICY (08-28): EDID runs ONCE per HPD session — done
    // or given-up, ladder restarts within the session skip it (the 60K
    // capture showed the hub deferring EDID to the cap on EVERY ceremony,
    // 783 DEFERs burned re-asking a question it had already answered).
    // Both latches clear only on HPD loss (session boundary), not on
    // ladder reset.
    reg        edid_done        = 1'b0;   // EDID preamble completed this session
    // TRAIN_RECOVER state (see parameter comment)
    reg [7:0]  train_pend       = 8'd0;   // training state to retry after train_wait
    reg [3:0]  train_retry_cnt  = 4'd0;   // retries taken this training pass
    reg        train_st         = 1'b0;   // registered: state is in 0x14..0x2C
    reg        errcnt_st        = 1'b0;   // registered: state == errcnt_read / errcnt_train
    // (TRAIN_ERRCNT == 0 — every board today — folds errcnt_train out of
    // these decodes: the state is unreachable then, its only entry being
    // align_test's state_on_success under the same parameter)
    wire [7:0] errcnt_skip_w = (TRAIN_ERRCNT != 0 && state == errcnt_train) ? align_adjust : check_wait;
    reg [15:0] err0_r           = 16'd0;  // SYMBOL_ERROR_COUNT lane0 {0x211,0x210}
    reg [15:0] err1_r           = 16'd0;  // SYMBOL_ERROR_COUNT lane1 {0x213,0x212}
    reg        lane_set_sent    = 1'b0;   // a lane-set write went out this training
    reg [15:0] lane_set_last    = 16'd0;  // the value it carried
    reg [7:0]  esi_2005_r       = 8'd0;   // latched LINK_SERVICE_IRQ_VECTOR_ESI0
    reg [7:0]  esi_2003_r       = 8'd0;   // latched DEVICE_SERVICE_IRQ_VECTOR_ESI0
    reg [7:0]  dbg_esi2003      = 8'd0;   // sticky OR of every 0x2003 read
    reg [7:0]  dbg_esi2005      = 8'd0;   // sticky OR of every 0x2005 read
    // ESI self-disable: ONE failed ESI transaction (NACK/short/timeout/
    // defer-budget) turns all ESI traffic off for the rest of the attach
    // — a non-ESI sink costs exactly one failed read, never a retry loop
    reg        esi_off          = 1'b0;
    reg [3:0]  esi_defer_cnt    = 4'd0;
    reg        esi_ack_owed     = 1'b0;   // this rt pass was IRQ-triggered:
                                          // unconditional ack (Mac parity)
    // wedge-suspect detector (advisory). NO ever-streamed gate: it
    // false-negatived on the first real wedge (K:00 held all session,
    // the brief flaps never landed on a 1 s poll) and the bad-cable
    // case it guarded is already blocked upstream by the presence gate
    // (no sink -> never establishes -> detector unreachable).
    reg [7:0]  r204_q           = 8'd0;   // last 0x204 byte from a status read
    // (138B clk100 ladder, 2026-10-24) the 32-bit timer is two 16-bit
    // halves; wedge_lo_max_r is maintained so that it equals
    // (wedge_lo == 16'hFFFF) on every cycle (preload: constant compare;
    // hold: both unchanged; count: lo+1 == FFFF <=> lo == FFFE), and the
    // high half increments exactly on the cycle the monolithic counter
    // carried. Bit-identical timer value; the increment was a 12-cell LUT
    // chain on the 138B (wedge_timer_1 -> wedge_timer_30 CE, -0.58 ns).
    reg [15:0] wedge_lo         = 16'd0;
    reg [15:0] wedge_hi         = 16'd0;
    reg        wedge_lo_max_r   = 1'b0;
    wire [31:0] wedge_timer     = {wedge_hi, wedge_lo};
    reg        wedge_suspect    = 1'b0;
    reg        irq_service_due  = 1'b0;   // hpd_irq consumed; ESI service owed
    reg        err_evt          = 1'b0;   // tagged error-entry event
    reg [3:0]  err_reason       = 4'd0;
    // (138B follow-up, timing campaign round 2) the failing state /
    // expected / byte count used to be captured at each error site into
    // err_from/err_exp/err_rxc — three registers whose enables were the
    // sites' full decode cones (next_state -> err_from CE -0.10 ns at
    // 100 MHz on the 138B die). The central err_evt block runs one cycle
    // after any site, so it now reads the same values from one-cycle
    // delayed copies (state_d already existed): identical err_detail.
    reg [7:0]  expected_d       = 8'd0;
    reg [7:0]  rxc_d            = 8'd0;
    reg [27:0] err_detail       = 28'd0;
    reg        err_detail_v     = 1'b0;
    reg [29:0] obs_timer        = 30'd0;  // time in the established set
    reg [7:0]  state_d          = 8'd0;
    reg        std_err_r        = 1'b0;   // (state_d == error), maintained
    reg        inest_d          = 1'b0;
    // (138B clk100 ladder, 2026-10-24) the three state-class decodes the
    // FSM reads most — "in the established set" (9 values; the retry
    // watchdog exclusion, the error tagging, the wedge detector), "state
    // is error" (the error-detail safety net) and "state is reset" (the
    // watchdog) — are flags written at EVERY site that writes `state`,
    // from the value being written (SET_STATE below), so each equals its
    // decode of `state` on every cycle by construction (including the
    // force sites, which write `state <= error` directly). A 9-way decode
    // of the state replicas in series with the next_state mux was the
    // gate's clk100 floor after the first fix set (state -> in_established
    // _set -> next_state D, +0.146 reported, 7 cells / 5.6 ns of route).
    reg  inest_r  = 1'b0;    // == in the established set
    reg  st_err_r = 1'b1;    // == (state == error)   (initial state is error)
    reg  st_rst_r = 1'b0;    // == (state == reset)
    function inest_f(input [7:0] v);
        inest_f = (v == link_established) ||
                  (v == check_link) || (v == check_wait) ||
                  (v == errcnt_read) ||
                  (v == irq_clear) ||
                  (v == esi_read_rt) || (v == esi_clear_rt) ||
                  (v == esi_clear2_rt) || (v == esi_eval_rt);
    endfunction
    // "is a TRAINING_LANEx_SET state" — the 12 states whose entry the M5
    // AFE hold gates. Maintained for next_state (ns_laneset_r, written by
    // the same combine that writes next_state, from the same group's
    // value), for state_on_success (sos_laneset_r, written with it) and
    // for state (st_ls_r, SET_STATE), so afe_hold is afe_busy AND one flop
    // instead of a 12-way decode of next_state (next_state -> afe_hold ->
    // train_retry_cnt RESET / aux_addr_i D: 5 decode cells, +0.14 / +0.21
    // on the diagnostic roll after the first two fix sets).
    function laneset_f(input [7:0] v);
        laneset_f = (v == clock_voltage_0p4 || v == clock_voltage_0p6 || v == clock_voltage_0p8 ||
                     v == align_p0_V0p4 || v == align_p0_V0p6 || v == align_p0_V0p8 ||
                     v == align_p1_V0p4 || v == align_p1_V0p6 || v == align_p1_V0p8 ||
                     v == align_p2_V0p4 || v == align_p2_V0p6 || v == align_p2_V0p8);
    endfunction
    reg  ns_laneset_r  = 1'b0;   // == laneset_f(next_state)
    reg  sos_laneset_r = 1'b0;   // == laneset_f(state_on_success)
    reg  st_ls_r       = 1'b0;   // == laneset_f(state)
    reg  sos_n_v;                // state_on_success written this cycle (blocking temporaries below)
    reg  [7:0] sos_n;
    reg  sos_ls_n, ns_b_ls, ns_c_ls;
`define SET_STATE(v) begin state <= (v); inest_r <= inest_f(v); st_err_r <= ((v) == error); st_rst_r <= ((v) == reset); st_ls_r <= laneset_f(v); end
    wire in_established_set = inest_r;
    wire obs_active = (LINK_CHECK == 2) ||
                      ((GATE_GRACE != 0) && (obs_timer < GATE_GRACE_CLKS));
    wire obs_mode   = (LINK_CHECK == 2) && in_established_set;
    reg [15:0] ob_chk = 16'd0, ob_short = 16'd0, ob_nack = 16'd0,
               ob_defer = 16'd0, ob_tmo = 16'd0, ob_gate = 16'd0;
    reg [7:0]  ob_hdr = 8'd0;
    reg        ob_counted = 1'b0;     // one class per check_link read
    reg        ob_defer_evt = 1'b0;
    // Registered at the state transition (not combinational off `state`):
    // the six 8-bit compares fed the short-read/non-ACK/DEFER/timeout
    // branches of the next_state mux and tipped the knife-edge (23
    // violations). Replies arrive many cycles after entry, so a decode
    // registered at the transition is settled long before it is used.
    reg esi_attach_st = 1'b0;
    reg esi_rt_st     = 1'b0;
    reg [1:0] dbg_timeouts   = 2'd0;
    assign debug_gate     = {dbg_gate_locks, dbg_gate_fail, dbg_timeouts};
    assign debug_teardown = {dbg_first_mask, dbg_fail_mask, dbg_gate_fail_sat, dbg_timeout_sat};
    assign debug_aux_err  = {dbg_short_sat, dbg_nack_sat, dbg_other_sat,
                             dbg_obs_sat, dbg_kick_sat, dbg_irq_sat};
    assign debug_err_detail = err_detail;
    assign debug_obs = {ob_hdr, ob_chk, ob_short, ob_nack, ob_defer, ob_tmo, ob_gate};
    wire [5:0] auxif_state;
    assign debug_auxif = {auxif_state, channel_busy, aux_rx_empty, expected, rx_byte_count};
    assign debug_esi   = {dbg_esi2003, dbg_esi2005};
    assign debug_defer = {edid_giveup, defer_cnt};
    assign wedge_suspect_o = wedge_suspect;
    assign errcnt0_o = err0_r;
    assign errcnt1_o = err1_r;
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
    adjust_evt          = 1'b0;
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
    // (138B clk100 ladder, 2026-10-24) the `next_state != state` term that
    // used to sit here was redundant at every consumer — the transition
    // block already ANDs it in, `state <= next_state` is a no-op when the
    // two are equal, and the count_100us reload is inside the unequal
    // branch — but it put the 8-bit inequality in series with the 12-way
    // decode on every register the hold gates (next_state -> afe_hold ->
    // 40 CEs, -0.92 ns reported). Same value wherever afe_hold is read.
    // Three copies of the same AND (syn_keep: not merged), one per
    // consumer group — the transition block (~25 enables), the
    // pure-function block (~45 enables), state/count — so no single net
    // carries all ~75 loads: roll 2 of 2026-10-24 drew 2.6 ns of route
    // on the one afe_hold net (afe busy -> afe_hold -> aux_addr_i CE,
    // -0.149). Identical value on every copy.
    wire afe_hold = (AFE_ADJUST != 0) && afe_busy && ns_laneset_r;
    (* syn_keep = 1 *) wire afe_hold_t = (AFE_ADJUST != 0) && afe_busy && ns_laneset_r;   // transition block
    (* syn_keep = 1 *) wire afe_hold_p = (AFE_ADJUST != 0) && afe_busy && ns_laneset_r;   // pure-function block

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
           .timeout     (channel_timeout),
           .debug_state (auxif_state)
    );

    assign aux_rx_rd_en = (!channel_busy) & (!aux_rx_empty);  // CHECK THIS!
      
always @(posedge clk) begin
    // next_state request groups (see the declaration): nothing requested
    ns_a_v = 1'b0; ns_b_v = 1'b0; ns_c_v = 1'b0; ns_d_v = 1'b0; ns_e_v = 1'b0; ns_f_v = 1'b0;
    ns_b = 8'h00; ns_c = 8'h00; ns_b_ls = 1'b0; ns_c_ls = 1'b0;
    sos_n = state_on_success; sos_ls_n = sos_laneset_r; sos_n_v = 1'b0;
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
    // LINK_CHECK==2 observe accounting (sites set err_evt/ob_defer_evt)
    ob_defer_evt <= 1'b0;
    if (LINK_CHECK == 2) begin
        if (state == check_link && state_d != check_link) begin
            ob_chk     <= ob_chk + 16'd1;
            ob_counted <= 1'b0;
        end else if ((err_evt || ob_defer_evt) && !ob_counted) begin
            ob_counted <= 1'b1;
            if (ob_defer_evt)              ob_defer <= ob_defer + 16'd1;
            else if (err_reason == 4'd1)   ob_short <= ob_short + 16'd1;
            else if (err_reason == 4'd2)   ob_nack  <= ob_nack  + 16'd1;
            else if (err_reason == 4'd3)   ob_gate  <= ob_gate  + 16'd1;
            else if (err_reason == 4'd4)   ob_tmo   <= ob_tmo   + 16'd1;
        end
    end
    if (err_evt) begin
        if (!err_detail_v) begin
            err_detail_v <= 1'b1;
            err_detail   <= {err_reason, state_d, expected_d, rxc_d};
        end
        if (err_reason == 4'd1 && dbg_short_sat != 4'hF)
            dbg_short_sat <= dbg_short_sat + 4'd1;
        if (err_reason == 4'd2 && dbg_nack_sat != 4'hF)
            dbg_nack_sat <= dbg_nack_sat + 4'd1;
    end
    // OTHER: an entry into `error` from the established set that no site
    // tagged — the safety net that catches teardown paths the reason
    // sites miss (the reason this latch is centralized at all)
    state_d    <= state;
    std_err_r  <= st_err_r;      // == (state_d == error)
    expected_d <= expected;
    rxc_d      <= rx_byte_count;
    inest_d  <= in_established_set;
    if (st_err_r && !std_err_r && inest_d && !err_evt) begin
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
     
    if(next_state != state && !afe_hold_t) begin
        //-----------------------------------------------------------
        // Get ready to count how many reply bytes have been received
        //-----------------------------------------------------------
        rx_byte_count <= 0;
        rxc_nz_r      <= 1'b0;
        rxc_p1_r      <= 8'd1;
        
        //-------------------------------------------------
        // Controlling which FSM state to go to on success
        //-------------------------------------------------
        case(next_state)
            reset:              begin sos_n = check_presence; sos_ls_n = laneset_f(check_presence); sos_n_v = 1'b1; end
            // Legacy: EDID skipped (DDC-class, converters DEFER it).
            // POLITE: evaluate the count byte first — presence_eval
            // routes to the paced re-read (count 0), the EDID preamble,
            // or straight on (EDID given up).
            check_presence:     begin sos_n = (POLITE_ATTACH != 0)
                                                    ? presence_eval : read_sink_count; sos_ls_n = laneset_f((POLITE_ATTACH != 0)
                                                    ? presence_eval : read_sink_count); sos_n_v = 1'b1; end
            presence_eval:      begin sos_n = (sink_count_r[5:0] == 6'd0)
                                                    ? presence_wait
                                                    : ((edid_giveup || edid_done)
                                                                   ? read_sink_count
                                                                   : edid_addr); sos_ls_n = laneset_f((sink_count_r[5:0] == 6'd0)
                                                    ? presence_wait
                                                    : ((edid_giveup || edid_done)
                                                                   ? read_sink_count
                                                                   : edid_addr)); sos_n_v = 1'b1; end
            presence_wait:      begin sos_n = check_presence; sos_ls_n = laneset_f(check_presence); sos_n_v = 1'b1; end
            edid_addr:          begin sos_n = edid_block0; sos_ls_n = laneset_f(edid_block0); sos_n_v = 1'b1; end
            defer_wait:         begin sos_n = defer_pend_state; sos_ls_n = laneset_f(defer_pend_state); sos_n_v = 1'b1; end
            train_wait:         begin sos_n = train_pend; sos_ls_n = laneset_f(train_pend); sos_n_v = 1'b1; end
            esi_read:           begin sos_n = esi_clear; sos_ls_n = laneset_f(esi_clear); sos_n_v = 1'b1; end
            esi_clear:          begin sos_n = (esi_2003_r != 8'h00)
                                                    ? esi_clear2 : set_power_d0; sos_ls_n = laneset_f((esi_2003_r != 8'h00)
                                                    ? esi_clear2 : set_power_d0); sos_n_v = 1'b1; end
            esi_clear2:         begin sos_n = set_power_d0; sos_ls_n = laneset_f(set_power_d0); sos_n_v = 1'b1; end
            esi_read_rt:        begin sos_n = esi_eval_rt; sos_ls_n = laneset_f(esi_eval_rt); sos_n_v = 1'b1; end
            // evaluated on entry, with the latches fresh from the read:
            // clear only when something is pending, or when this pass was
            // IRQ-triggered (unconditional ack, Mac parity). The 1 Hz
            // poll with nothing pending costs ZERO writes.
            esi_eval_rt:        begin sos_n = (esi_2005_r != 8'h00 ||
                                                     esi_2003_r != 8'h00 ||
                                                     esi_ack_owed)
                                                    ? esi_clear_rt : link_established; sos_ls_n = laneset_f((esi_2005_r != 8'h00 ||
                                                     esi_2003_r != 8'h00 ||
                                                     esi_ack_owed)
                                                    ? esi_clear_rt : link_established); sos_n_v = 1'b1; end
            esi_clear_rt:       begin sos_n = (esi_2003_r != 8'h00)
                                                    ? esi_clear2_rt : link_established; sos_ls_n = laneset_f((esi_2003_r != 8'h00)
                                                    ? esi_clear2_rt : link_established); sos_n_v = 1'b1; end
            esi_clear2_rt:      begin sos_n = link_established; sos_ls_n = laneset_f(link_established); sos_n_v = 1'b1; end
            edid_block0:        begin sos_n = edid_block1; sos_ls_n = laneset_f(edid_block1); sos_n_v = 1'b1; end
            edid_block1:        begin sos_n = edid_block2; sos_ls_n = laneset_f(edid_block2); sos_n_v = 1'b1; end
            edid_block2:        begin sos_n = edid_block3; sos_ls_n = laneset_f(edid_block3); sos_n_v = 1'b1; end
            edid_block3:        begin sos_n = edid_block4; sos_ls_n = laneset_f(edid_block4); sos_n_v = 1'b1; end
            edid_block4:        begin sos_n = edid_block5; sos_ls_n = laneset_f(edid_block5); sos_n_v = 1'b1; end
            edid_block5:        begin sos_n = edid_block6; sos_ls_n = laneset_f(edid_block6); sos_n_v = 1'b1; end
            edid_block6:        begin sos_n = edid_block7; sos_ls_n = laneset_f(edid_block7); sos_n_v = 1'b1; end
            edid_block7:        begin sos_n = read_sink_count; sos_ls_n = laneset_f(read_sink_count); sos_n_v = 1'b1; end
            read_sink_count:    begin sos_n = read_registers; sos_ls_n = laneset_f(read_registers); sos_n_v = 1'b1; end
            // ESI mode: the Mac's unconditional attach-time ESI ack goes
            // here — between the caps read and the D0 wake.
            read_registers:     begin sos_n = (IRQ_SERVICE == 2 && !esi_off)
                                                    ? esi_read : set_power_d0; sos_ls_n = laneset_f((IRQ_SERVICE == 2 && !esi_off)
                                                    ? esi_read : set_power_d0); sos_n_v = 1'b1; end
            set_power_d0:       begin sos_n = set_channel_coding; sos_ls_n = laneset_f(set_channel_coding); sos_n_v = 1'b1; end
            set_channel_coding: begin sos_n = set_speed_270; sos_ls_n = laneset_f(set_speed_270); sos_n_v = 1'b1; end                        
            set_speed_270:      begin sos_n = set_downspread; sos_ls_n = laneset_f(set_downspread); sos_n_v = 1'b1; end                        
            set_downspread:     case(link_count)
                                           3'b001:  begin sos_n = set_link_count_1; sos_ls_n = laneset_f(set_link_count_1); sos_n_v = 1'b1; end                        
                                           3'b010:  begin sos_n = set_link_count_2; sos_ls_n = laneset_f(set_link_count_2); sos_n_v = 1'b1; end                        
                                           3'b100:  begin sos_n = set_link_count_4; sos_ls_n = laneset_f(set_link_count_4); sos_n_v = 1'b1; end
                                           default: begin sos_n = error; sos_ls_n = laneset_f(error); sos_n_v = 1'b1; end
                                       endcase
            set_link_count_1:   begin sos_n = clock_training; sos_ls_n = laneset_f(clock_training); sos_n_v = 1'b1; end 
            set_link_count_2:   begin sos_n = clock_training; sos_ls_n = laneset_f(clock_training); sos_n_v = 1'b1; end 
            set_link_count_4:   begin sos_n = clock_training; sos_ls_n = laneset_f(clock_training); sos_n_v = 1'b1; end 
            //----- Display Port clock training -------------------                        
            clock_training:     begin sos_n = clock_voltage_0p4; sos_ls_n = laneset_f(clock_voltage_0p4); sos_n_v = 1'b1; end
            clock_voltage_0p4:  begin sos_n = clock_wait; sos_ls_n = laneset_f(clock_wait); sos_n_v = 1'b1; end
            clock_voltage_0p6:  begin sos_n = clock_wait; sos_ls_n = laneset_f(clock_wait); sos_n_v = 1'b1; end
            clock_voltage_0p8:  begin sos_n = clock_wait; sos_ls_n = laneset_f(clock_wait); sos_n_v = 1'b1; end
            clock_wait:         begin sos_n = clock_test; sos_ls_n = laneset_f(clock_test); sos_n_v = 1'b1; end                        
            clock_test:         begin sos_n = clock_adjust; sos_ls_n = laneset_f(clock_adjust); sos_n_v = 1'b1; end
            clock_adjust:       begin sos_n = clock_wait_after; sos_ls_n = laneset_f(clock_wait_after); sos_n_v = 1'b1; end
            clock_wait_after:   if(clock_locked_i == 1'b1) begin
                                    begin sos_n = align_training; sos_ls_n = laneset_f(align_training); sos_n_v = 1'b1; end
                                // POLITE: write-on-change — the wire showed
                                // 383 rewrites of an UNCHANGED 0x103 per
                                // attach; skip the write and just poll again
                                // when the value we'd send is what the sink
                                // already has.
                                end else if(POLITE_ATTACH != 0 && LANE_SET_WOC != 0 && lane_set_sent &&
                                            lane_set_last == train_set_byte) begin
                                    begin sos_n = clock_wait; sos_ls_n = laneset_f(clock_wait); sos_n_v = 1'b1; end
                                end else if(swing_0p8 == 1'b1) begin
                                    begin sos_n = clock_voltage_0p8; sos_ls_n = laneset_f(clock_voltage_0p8); sos_n_v = 1'b1; end
                                end else if(swing_0p6 == 1'b1) begin
                                    begin sos_n = clock_voltage_0p6; sos_ls_n = laneset_f(clock_voltage_0p6); sos_n_v = 1'b1; end
                                end else begin
                                    begin sos_n = clock_voltage_0p4; sos_ls_n = laneset_f(clock_voltage_0p4); sos_n_v = 1'b1; end
                                end
            //----- Display Port Alignment traning ------------                        
            align_training:     if(POLITE_ATTACH != 0 && LANE_SET_WOC != 0 && lane_set_sent &&
                                   lane_set_last == train_set_byte) begin
                                     begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
                                end else if(swing_0p8 == 1'b1) begin
                                     begin sos_n = align_p0_V0p8; sos_ls_n = laneset_f(align_p0_V0p8); sos_n_v = 1'b1; end
                                end else if(swing_0p6 == 1'b1) begin
                                     begin sos_n = align_p0_V0p6; sos_ls_n = laneset_f(align_p0_V0p6); sos_n_v = 1'b1; end
                                end else begin
                                     begin sos_n = align_p0_V0p4; sos_ls_n = laneset_f(align_p0_V0p4); sos_n_v = 1'b1; end
                                end
            align_p0_V0p4:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p0_V0p6:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p0_V0p8:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p1_V0p4:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p1_V0p6:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p1_V0p8:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p2_V0p4:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p2_V0p6:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_p2_V0p8:      begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
            align_wait0:        begin sos_n = align_wait1; sos_ls_n = laneset_f(align_wait1); sos_n_v = 1'b1; end                        
            align_wait1:        begin sos_n = align_wait2; sos_ls_n = laneset_f(align_wait2); sos_n_v = 1'b1; end                        
            align_wait2:        begin sos_n = align_wait3; sos_ls_n = laneset_f(align_wait3); sos_n_v = 1'b1; end                        
            align_wait3:        begin sos_n = align_test; sos_ls_n = laneset_f(align_test); sos_n_v = 1'b1; end                        
            align_test:         begin sos_n = (TRAIN_ERRCNT != 0) ? errcnt_train : align_adjust; sos_ls_n = laneset_f((TRAIN_ERRCNT != 0) ? errcnt_train : align_adjust); sos_n_v = 1'b1; end
            errcnt_train:       begin sos_n = align_adjust; sos_ls_n = laneset_f(align_adjust); sos_n_v = 1'b1; end
            align_adjust:       begin sos_n = align_wait_after; sos_ls_n = laneset_f(align_wait_after); sos_n_v = 1'b1; end
            align_wait_after:   if(symbol_locked_i == 1'b1) begin
                                           begin sos_n = switch_to_normal; sos_ls_n = laneset_f(switch_to_normal); sos_n_v = 1'b1; end
                                end else if(POLITE_ATTACH != 0 && LANE_SET_WOC != 0 && lane_set_sent &&
                                            lane_set_last == train_set_byte) begin
                                    begin sos_n = align_wait0; sos_ls_n = laneset_f(align_wait0); sos_n_v = 1'b1; end
                                end else if(swing_0p8 == 1'b1) begin
                                    if(preemp_6p0 == 1'b1) begin
                                        begin sos_n = align_p2_V0p8; sos_ls_n = laneset_f(align_p2_V0p8); sos_n_v = 1'b1; end
                                    end else if(preemp_3p5 == 1'b1) begin
                                        begin sos_n = align_p1_V0p8; sos_ls_n = laneset_f(align_p1_V0p8); sos_n_v = 1'b1; end
                                    end else begin
                                        begin sos_n = align_p0_V0p8; sos_ls_n = laneset_f(align_p0_V0p8); sos_n_v = 1'b1; end
                                    end
                                end else if(swing_0p6 == 1'b1) begin
                                    if(preemp_6p0 == 1'b1) begin
                                        begin sos_n = align_p2_V0p6; sos_ls_n = laneset_f(align_p2_V0p6); sos_n_v = 1'b1; end
                                    end else if(preemp_3p5 == 1'b1) begin
                                        begin sos_n = align_p1_V0p6; sos_ls_n = laneset_f(align_p1_V0p6); sos_n_v = 1'b1; end
                                    end else begin
                                        begin sos_n = align_p0_V0p6; sos_ls_n = laneset_f(align_p0_V0p6); sos_n_v = 1'b1; end
                                    end
                                end else begin
                                    if(preemp_6p0 == 1'b1) begin
                                        begin sos_n = align_p2_V0p4; sos_ls_n = laneset_f(align_p2_V0p4); sos_n_v = 1'b1; end
                                    end else if(preemp_3p5 == 1'b1) begin
                                        begin sos_n = align_p1_V0p4; sos_ls_n = laneset_f(align_p1_V0p4); sos_n_v = 1'b1; end
                                    end else begin
                                        begin sos_n = align_p0_V0p4; sos_ls_n = laneset_f(align_p0_V0p4); sos_n_v = 1'b1; end
                                    end 
                                end                        
            switch_to_normal:   begin sos_n = link_established; sos_ls_n = laneset_f(link_established); sos_n_v = 1'b1; end  
            link_established:   begin sos_n = link_established; sos_ls_n = laneset_f(link_established); sos_n_v = 1'b1; end
            check_link:         begin sos_n = (ERRCNT_READ != 0) ? errcnt_read : check_wait; sos_ls_n = laneset_f((ERRCNT_READ != 0) ? errcnt_read : check_wait); sos_n_v = 1'b1; end
            errcnt_read:        begin sos_n = check_wait; sos_ls_n = laneset_f(check_wait); sos_n_v = 1'b1; end
            irq_clear:          begin sos_n = link_established; sos_ls_n = laneset_f(link_established); sos_n_v = 1'b1; end
            check_wait:         begin
                                dbg_gate_locks <= {clock_locked_i, equ_locked_i, symbol_locked_i, align_locked_i};
                                // (check-non-fatal diagnostic reverted 08-18 night: it
                                // answered the Ugreen question — genuine sink loss — and
                                // then BLOCKED the Anker path's recovery-by-retrain on
                                // marginal instances. Teardown/retrain is load-bearing.)
                                if(clock_locked_i == 1'b1 && equ_locked_i == 1'b1 && symbol_locked_i == 1'b1 && align_locked_i == 1'b1) begin
                                    begin sos_n = (IRQ_SERVICE == 2 && !esi_off)
                                                        ? esi_read_rt :
                                                        (IRQ_SERVICE == 1 && irq_vec_r != 8'h00)
                                                        ? irq_clear : link_established; sos_ls_n = laneset_f((IRQ_SERVICE == 2 && !esi_off)
                                                        ? esi_read_rt :
                                                        (IRQ_SERVICE == 1 && irq_vec_r != 8'h00)
                                                        ? irq_clear : link_established); sos_n_v = 1'b1; end
                                end else if (obs_active) begin
                                    // GRACE PERIOD: record, do NOT tear
                                    // down; stay established and keep
                                    // polling (status stays visible in C:
                                    // at the poll cadence).
                                    if (dbg_obs_sat != 4'hF)
                                        dbg_obs_sat <= dbg_obs_sat + 4'd1;
                                    if (LINK_CHECK == 2) begin
                                        err_evt <= 1'b1; err_reason <= 4'd3;
                                    end
                                    begin sos_n = link_established; sos_ls_n = laneset_f(link_established); sos_n_v = 1'b1; end
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
                                    begin sos_n = error; sos_ls_n = laneset_f(error); sos_n_v = 1'b1; end
                                end
                                end
            error:              begin sos_n = error; sos_ls_n = laneset_f(error); sos_n_v = 1'b1; end
            // every value next_state can hold is listed above (all
            // assignments are from the localparams, state_on_success,
            // state, train_pend or defer_pend_state); the default is
            // unreachable and only tells synthesis the enable is the
            // transition alone, not transition AND "value is listed"
            default:            begin sos_n = error; sos_ls_n = laneset_f(error); sos_n_v = 1'b1; end
        endcase
        if (sos_n_v) begin
            state_on_success <= sos_n;
            sos_laneset_r    <= sos_ls_n;
        end

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
        if (next_state == esi_clear2 || next_state == esi_clear2_rt) begin
            irq_clear_byte_r <= esi_2003_r;
            esi_2003_r       <= 8'h00;
        end
        // EDID completed this session (giveup exits set edid_giveup in
        // the same cycle; a redundant edid_done alongside it is harmless)
        if (state == edid_block7 && next_state == read_sink_count)
            edid_done <= 1'b1;
        if (next_state == esi_read_rt) begin
            esi_ack_owed    <= irq_service_due;
            irq_service_due <= 1'b0;
        end
        if (next_state == link_established)
            esi_ack_owed <= 1'b0;
        // any completed ESI read re-arms the defer budget
        if (next_state == esi_eval_rt || next_state == esi_clear)
            esi_defer_cnt <= 4'd0;
        // lane-set write tracking (POLITE write-on-change)
        if (ns_laneset_r) begin          // next_state is a lane-set state (maintained flag)
            lane_set_sent <= 1'b1;
            lane_set_last <= train_set_byte;
        end
        // one increment per paced-retry entry (moved out of the AUX
        // completion cone for timing: tx_rd_ptr->CE was the knife-edge)
        if (state != train_wait && next_state == train_wait &&
            train_retry_cnt != TRAIN_RETRY_CAP)
            train_retry_cnt <= train_retry_cnt + 4'd1;
        if (next_state == reset || next_state == clock_training) begin
            lane_set_sent   <= 1'b0;
            train_retry_cnt <= 4'd0;   // fresh budget per training pass
        end
        if (next_state == reset) begin
            // NOTE: defer_cnt / edid_giveup / edid_done deliberately NOT
            // reset here — EDID policy is per HPD SESSION (cleared on HPD
            // loss below), not per ladder ceremony.
            irq_service_due <= 1'b0;
            esi_off         <= 1'b0;
            esi_defer_cnt   <= 4'd0;
            esi_ack_owed    <= 1'b0;
            wedge_suspect   <= 1'b0;
        end
    end

    //-----------------------------------------------------------------
    // Registers that are pure functions of next_state (138B clk100
    // ladder, 2026-10-24). msg / expected / the *_de_active routing
    // flags / reset_addr_on_change / the tx_* flags / the esi_*_st,
    // train_st and errcnt_st decodes used to be written inside the
    // transition block above, i.e. with `next_state != state && !afe_hold`
    // as their clock enable — the 8-bit inequality plus the hold decode in
    // front of ~40 registers (16 of the 22 violated endpoints in the
    // 2026-10-24 roll: next_state -> dp_reg_de_active/tx_clock_train/msg/
    // train_st/... CE). They are now written on EVERY cycle the ladder is
    // not held, from the same case statements (moved verbatim). Identical
    // on every cycle: between transitions next_state == state, so the
    // case re-selects the value the transition wrote; during an afe_hold
    // neither version writes (the old block did not run, this one is
    // gated by the same !afe_hold); at the transition both write from the
    // same next_state at the same edge. `expected` keeps its hold on
    // `error` and unlisted values exactly as the case left it before.
    // (msg_de, state_on_success, rx_byte_count and the side-effect
    // registers stay on the transition enable — they must fire once.)
    //-----------------------------------------------------------------
    if (!afe_hold_p) begin
        esi_attach_st <= (next_state == esi_read) || (next_state == esi_clear) ||
                         (next_state == esi_clear2);
        esi_rt_st     <= (next_state == esi_read_rt) || (next_state == esi_clear_rt) ||
                         (next_state == esi_clear2_rt);
        // registered like the ESI decodes (combinational state-range
        // compares in the error paths cost setup at the knife-edge)
        train_st      <= (next_state >= clock_training) &&
                         (next_state <= align_wait_after);
        errcnt_st     <= (next_state == errcnt_read) ||
                         (TRAIN_ERRCNT != 0 && next_state == errcnt_train);
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
            train_wait:           begin msg <= 8'h00; expected <= 8'h00; end
            esi_read:             begin msg <= 8'h1B; expected <= 8'h0E; end
            esi_clear:            begin msg <= 8'h1C; expected <= 8'h01; end
            esi_read_rt:          begin msg <= 8'h1B; expected <= 8'h0E; end
            esi_clear_rt:         begin msg <= 8'h1C; expected <= 8'h01; end
            esi_clear2:           begin msg <= 8'h1D; expected <= 8'h01; end
            esi_clear2_rt:        begin msg <= 8'h1D; expected <= 8'h01; end
            esi_eval_rt:          begin msg <= 8'h00; expected <= 8'h00; end

            edid_block0:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block1:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block2:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block3:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block4:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block5:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block6:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
            edid_block7:          begin msg <= 8'h02; expected <= 8'h11; edid_de_active <= 1'b1; end
                    
            read_sink_count:      begin msg <= 8'h03; expected <= 8'h02; reset_addr_on_change <= 1'b1; end
            read_registers:       begin msg <= 8'h04; expected <= 8'h11; dp_reg_de_active <= 1'b1; end   // 16 bytes 0x000-0x00F (0x00E = TRAINING_AUX_RD_INTERVAL)
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
            errcnt_read:          begin msg <= 8'h1E; expected <= 8'h05; end
            errcnt_train:         begin msg <= 8'h1E; expected <= 8'h05; end
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
            errcnt_train:         begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end   // keep TPS2 on during the count read
            align_adjust:         begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            align_wait_after:     begin tx_powerup <= 1'b1; tx_align_train <= 1'b1; end
            switch_to_normal:     begin tx_powerup <= 1'b1; end
            link_established:     begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            check_link:           begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            check_wait:           begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            errcnt_read:          begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            irq_clear:            begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            // TRAIN_RECOVER: hold the pending training state's TX flags
            // through the paced retry — the whole point is no TX bounce
            train_wait:           begin tx_powerup <= 1'b1;
                                        tx_clock_train <= (train_pend <= clock_wait_after);
                                        tx_align_train <= (train_pend >= align_training); end
            // runtime ESI service: the stream is live — hold the TX up
            esi_read_rt:          begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            esi_clear_rt:         begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            esi_clear2_rt:        begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
            esi_eval_rt:          begin tx_powerup <= 1'b1; tx_link_established <= 1'b1; end
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
        `SET_STATE(next_state)   // state <= next_state (+ the class flags)
            
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
    // The paced wait states advance on their own (longer) dwells —
    // excluded here. POLITE also stretches the two CR wait states by
    // ~330 us each (blind_dwell[15]) so the CR poll loop runs at ~1 ms
    // instead of ~650 us.
    //
    // (138B clk100 ladder, 2026-10-24) the five wait advances below all
    // assign state_on_success and sat as five consecutive lowest-priority
    // sites of the next_state mux chain; they are ONE site now (same
    // priority position, same value — identical). Their state and
    // `expected` decodes are the registered adv_*_r flags: count_100us
    // reloads on every transition and its bit 14 is clear for the first
    // 10,000 cycles in a state, blind_dwell resets on every transition and
    // bits 16/21 are clear for the first 65,536, and `expected` is only
    // written at a transition — so wherever a dwell bit is set the state
    // and expected have been stable for thousands of cycles and the
    // one-cycle-late flags equal the live decodes (the argument
    // is_align_wait_r already relies on). blind_dwell[15] stays live in
    // the POLITE CR term: it is only read ANDed with count_100us[14],
    // whose first set cycle in a state (10,000) and the dwell bit's rise
    // (32,768) never coincide. `state == next_state` and the dwell bits
    // themselves stay live.
    if(state == next_state && (
         (count_100us[14] == 1'b1 && adv_ok_r &&
          (POLITE_ATTACH == 0 || !adv_cr_r || blind_dwell[15] == 1'b1)) ||
         // presence_wait: ~21 ms between sink-count re-reads while the
         // branch reports no sink (blind_dwell counts cycles-in-state)
         (adv_pw_r && blind_dwell[21] == 1'b1) ||
         // defer_wait: ~650 us before re-sending a DEFERred request (the
         // hub ACKs the Mac's EDID reads on the 3rd try at this pacing)
         (adv_dw_r && blind_dwell[16] == 1'b1) ||
         // train_wait: same ~650 us pace before retrying a failed training read
         (adv_tw_r && blind_dwell[16] == 1'b1) ||
         // align_wait0-3: EQ dwell = eq_mult_w x 100 us per state (the
         // sink's TRAINING_AUX_RD_INTERVAL from DPCD 0x00E, or the
         // EQ_WAIT_MULT floor)
         (is_align_wait_r && eq_dwell_done_r))) begin
        ns_a_v = 1'b1;                     // -> state_on_success
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
        bd_lo       <= bd_lo + 16'd1;
        bd_lo_max_r <= (bd_lo == 16'hFFFE);
        if (bd_lo_max_r)
            bd_hi   <= bd_hi + 8'd1;
    end else begin
        bd_lo       <= 16'd0;
        bd_hi       <= 8'd0;
        bd_lo_max_r <= 1'b0;
    end
    if(BLIND_SINK != 0 && channel_busy == 1'b0 && blind_dwell[23] == 1'b1) begin
        ns_a_v = 1'b1;                     // -> state_on_success
    end
            
    //------------------------------------------------------------
    // Processing the data that has been received from the sink
    // over the AUX channel. The data bytes are just streamed out
    // to a downstream component that uses the values, and may 
    // set flags that feed back in to control the FSM.
    //------------------------------------------------------------
    edid_de    <= 1'b0;
    adjust_de  <= 1'b0;
    adjust_evt <= 1'b0;
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
            if((LATE_REPLY_DRAIN == 0 || expected != 8'h00) && rxc_p1_r != expected && aux_rx_empty == 1'b1) begin
                if (esi_attach_st) begin
                    esi_off    <= 1'b1;      // sink rejects ESI: one try, then off
                    ns_b = set_power_d0; ns_b_ls = laneset_f(set_power_d0); ns_b_v = 1'b1;
                end else if (esi_rt_st) begin
                    esi_off    <= 1'b1;
                    ns_b = link_established; ns_b_ls = laneset_f(link_established); ns_b_v = 1'b1;
                end else if (errcnt_st) begin
                    // instrumentation read failed: never tear down for it
                    ns_b = errcnt_skip_w; ns_b_ls = laneset_f(errcnt_skip_w); ns_b_v = 1'b1;
                end else if (POLITE_ATTACH != 0 && TRAIN_RECOVER != 0 && train_st) begin
                    // short/garbled training-read reply: paced retry with
                    // TX held, cheap retrain on budget exhaustion (the
                    // counter increments on train_wait ENTRY — see the
                    // transition tracker — keeping this cone shallow)
                    if (train_retry_cnt != TRAIN_RETRY_CAP) begin
                        train_pend      <= state;
                        ns_b = train_wait; ns_b_ls = laneset_f(train_wait); ns_b_v = 1'b1;
                    end else begin
                        ns_b = clock_training; ns_b_ls = laneset_f(clock_training); ns_b_v = 1'b1;
                    end
                end else if (obs_mode) begin
                    ns_b = link_established; ns_b_ls = laneset_f(link_established); ns_b_v = 1'b1;   // observe: count only
                    err_evt <= 1'b1; err_reason <= 4'd1;
                end else begin
                    ns_b = error; ns_b_ls = laneset_f(error); ns_b_v = 1'b1;
                    if (in_established_set) begin
                        err_evt <= 1'b1; err_reason <= 4'd1;
                    end
                end
            end
                                
            if(!rxc_nz_r) begin
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
                    if (esi_attach_st && aux_rx_data != 8'h20) begin
                        esi_off    <= 1'b1;
                        ns_b = set_power_d0; ns_b_ls = laneset_f(set_power_d0); ns_b_v = 1'b1;
                    end else if (esi_rt_st && aux_rx_data != 8'h20) begin
                        esi_off    <= 1'b1;
                        ns_b = link_established; ns_b_ls = laneset_f(link_established); ns_b_v = 1'b1;
                    end else if (errcnt_st && aux_rx_data != 8'h20) begin
                        ns_b = errcnt_skip_w; ns_b_ls = laneset_f(errcnt_skip_w); ns_b_v = 1'b1;   // instrumentation: skip
                    end else if (POLITE_ATTACH != 0 && TRAIN_RECOVER != 0 &&
                                 train_st && aux_rx_data != 8'h20) begin
                        // non-ACK header on a training read (a clean DEFER
                        // falls through to the paced defer path below)
                        if (train_retry_cnt != TRAIN_RETRY_CAP) begin
                            train_pend      <= state;
                            ns_b = train_wait; ns_b_ls = laneset_f(train_wait); ns_b_v = 1'b1;
                        end else begin
                            ns_b = clock_training; ns_b_ls = laneset_f(clock_training); ns_b_v = 1'b1;
                        end
                    end else if (obs_mode) begin
                        ns_b = link_established; ns_b_ls = laneset_f(link_established); ns_b_v = 1'b1;   // observe: count only
                        ob_hdr     <= aux_rx_data;
                        if (aux_rx_data != 8'h20) begin   // DEFER counted below
                            err_evt <= 1'b1; err_reason <= 4'd2;
                        end
                    end else begin
                        ns_b = error; ns_b_ls = laneset_f(error); ns_b_v = 1'b1;
                        if (in_established_set) begin
                            err_evt <= 1'b1; err_reason <= 4'd2;
                        end
                    end
                end
                if(rxc_p1_r == expected && aux_rx_empty == 1'b1) begin
                    ns_b = state_on_success; ns_b_ls = sos_laneset_r; ns_b_v = 1'b1;
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
                    if (obs_mode) begin
                        ob_defer_evt <= 1'b1;             // observe: count only
                        ns_b = link_established; ns_b_ls = laneset_f(link_established); ns_b_v = 1'b1;
                    end else if (errcnt_st) begin
                        // deferred instrumentation read: skip (defer_wait
                        // would drop tx_link_established mid-stream)
                        ns_b = errcnt_skip_w; ns_b_ls = laneset_f(errcnt_skip_w); ns_b_v = 1'b1;
                    end else if ((esi_attach_st || esi_rt_st) && IRQ_SERVICE == 2) begin
                        // ESI defer: paced retry from a small budget; the
                        // rt states are watchdog-excluded, so WITHOUT the
                        // budget a forever-deferring hub would spin here
                        // unbounded with the link held
                        if (esi_defer_cnt >= 4'd8) begin
                            esi_off    <= 1'b1;
                            ns_b = esi_attach_st ? set_power_d0
                                                        : link_established; ns_b_ls = laneset_f(esi_attach_st ? set_power_d0
                                                        : link_established); ns_b_v = 1'b1;
                        end else begin
                            esi_defer_cnt    <= esi_defer_cnt + 4'd1;
                            defer_pend_state <= state;
                            ns_b = defer_wait; ns_b_ls = laneset_f(defer_wait); ns_b_v = 1'b1;
                        end
                    end else if (POLITE_ATTACH != 0) begin
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
                            ns_b = read_sink_count; ns_b_ls = laneset_f(read_sink_count); ns_b_v = 1'b1;
                        end else begin
                            defer_pend_state <= state;
                            ns_b = defer_wait; ns_b_ls = laneset_f(defer_wait); ns_b_v = 1'b1;
                        end
                    end else begin
                        // legacy: just flip states to force a retry.
                        `SET_STATE(state_on_success)
                        ns_b = state; ns_b_ls = st_ls_r; ns_b_v = 1'b1;
                    end
                end
            end else begin
                //-----------------------------------------------------------------
                // Process a non-ack data byte, routing it out using the DE signals
                //-----------------------------------------------------------------
                edid_de    <= edid_de_active;
                adjust_de  <= adjust_de_active;
                adjust_evt <= adjust_de_active && (aux_addr_i == 8'h00);
                dp_reg_de  <= dp_reg_de_active;                                
                status_de  <= status_de_active;                                

                aux_data   <= aux_rx_data;
                aux_addr   <= aux_addr_i;
                aux_addr_i <= aux_addr_i+1;
                if(status_de_active == 1'b1 && aux_addr_i == 8'd4)
                    r204_q <= aux_rx_data;
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
                   rx_byte_count == 8'h01) begin
                    esi_2003_r  <= aux_rx_data;
                    dbg_esi2003 <= dbg_esi2003 | aux_rx_data;
                end
                if((state == esi_read || state == esi_read_rt) &&
                   rx_byte_count == 8'h02)
                    dbg_esi2003 <= dbg_esi2003 | aux_rx_data;
                if((state == esi_read || state == esi_read_rt) &&
                   rx_byte_count == 8'h03) begin
                    esi_2005_r  <= aux_rx_data;
                    dbg_esi2005 <= dbg_esi2005 | aux_rx_data;
                end
                // TRAINING_AUX_RD_INTERVAL (DPCD 0x00E = caps read byte 15):
                // bits 6:0, 0 = 400 us EQ interval, n = n x 4 ms
                if(state == read_registers && rx_byte_count == 8'h0F)
                    train_interval_r <= aux_rx_data[6:0];
                // SYMBOL_ERROR_COUNT: bytes 1-4 = 0x210..0x213
                if((state == errcnt_read || (TRAIN_ERRCNT != 0 && state == errcnt_train)) && rx_byte_count == 8'h01)
                    err0_r[7:0]  <= aux_rx_data;
                if((state == errcnt_read || (TRAIN_ERRCNT != 0 && state == errcnt_train)) && rx_byte_count == 8'h02)
                    err0_r[15:8] <= aux_rx_data;
                if((state == errcnt_read || (TRAIN_ERRCNT != 0 && state == errcnt_train)) && rx_byte_count == 8'h03)
                    err1_r[7:0]  <= aux_rx_data;
                if((state == errcnt_read || (TRAIN_ERRCNT != 0 && state == errcnt_train)) && rx_byte_count == 8'h04)
                    err1_r[15:8] <= aux_rx_data;
                        
                if(rxc_p1_r == expected && aux_rx_empty == 1'b1) begin
                    ns_b = state_on_success; ns_b_ls = sos_laneset_r; ns_b_v = 1'b1;
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
    if (BLIND_SINK == 0 && channel_timeout == 1'b1 &&
        (esi_attach_st || esi_rt_st)) begin
        // sink ignores ESI entirely: same self-disable, no FSM reset
        esi_off    <= 1'b1;
        ns_c = esi_attach_st ? set_power_d0 : link_established; ns_c_ls = laneset_f(esi_attach_st ? set_power_d0 : link_established); ns_c_v = 1'b1;
        `SET_STATE(error)   // force the transition machinery
    end else
    if (BLIND_SINK == 0 && channel_timeout == 1'b1 && obs_mode) begin
        ns_c = link_established; ns_c_ls = laneset_f(link_established); ns_c_v = 1'b1;   // observe: count only
        `SET_STATE(error)              // force the transition machinery
        err_evt    <= 1'b1; err_reason <= 4'd4;
    end else
    if (BLIND_SINK == 0 && channel_timeout == 1'b1 && errcnt_st) begin
        ns_c = errcnt_skip_w; ns_c_ls = laneset_f(errcnt_skip_w); ns_c_v = 1'b1;   // instrumentation timeout: skip
        `SET_STATE(error)        // force the transition machinery
    end else
    if (BLIND_SINK == 0 && channel_timeout == 1'b1 &&
        POLITE_ATTACH != 0 && TRAIN_RECOVER != 0 && train_st) begin
        // training-read reply never arrived (the 60K capture's 20.16 ms
        // round gap): paced retry with TX held instead of the full
        // ceremony teardown; cheap retrain when the budget runs out
        if (train_retry_cnt != TRAIN_RETRY_CAP) begin
            train_pend      <= state;
            ns_c = train_wait; ns_c_ls = laneset_f(train_wait); ns_c_v = 1'b1;
        end else begin
            ns_c = clock_training; ns_c_ls = laneset_f(clock_training); ns_c_v = 1'b1;
        end
        `SET_STATE(error)   // force the transition machinery
    end else
    // (the exclusion list was reset + the nine established-set states,
    // written out; it is the two maintained flags now — same set)
    if((BLIND_SINK == 0 && channel_timeout == 1'b1) ||
                                  (!st_rst_r && !inest_r && retry_now == 1'b1)) begin
        ns_c = reset; ns_c_ls = laneset_f(reset); ns_c_v = 1'b1;
        `SET_STATE(error)
        if (in_established_set) begin
            err_evt <= 1'b1;
            err_reason <= (BLIND_SINK == 0 && channel_timeout == 1'b1) ? 4'd4 : 4'd5;
           
        end
    end
    
    //-----------------------------------------------
    // If the link was established, then every
    // now and then check the state of the link
    //-----------------------------------------------
    if (IRQ_SERVICE != 0 && hpd_irq)
        irq_pending <= 1'b1;
    // (msg_busy: never hand the message engine a request while it is still
    // emitting the previous one — it would be dropped and the FSM would
    // wait forever in check_link. link_check_now is a pulse, so remember it.)
    // Latch the periodic-check pulse only for the busy-engine hold; a pulse
    // that lands while the FSM is elsewhere is dropped exactly as before
    // (tb_polite_attach counts its runtime ESI services).
    if (link_check_now) link_check_pend <= 1'b1;
    if (state != link_established) link_check_pend <= 1'b0;
    if(LINK_CHECK != 0 && state == link_established && !msg_busy &&
       (link_check_pend == 1'b1 || (IRQ_SERVICE != 0 && irq_pending))) begin
        link_check_pend <= 1'b0;
        ns_d_v = 1'b1;                     // -> check_link
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
        ns_e_v = 1'b1;                     // -> reset
        `SET_STATE(error)
    end
    // HPD SESSION BOUNDARY: the per-session EDID policy re-arms on
    // presence loss regardless of the teardown policy above (which
    // production keeps OFF — sleeping-monitor flap storms). Flap-storm
    // pulses are sub-2 ms and never drop hpd_present, so this only fires
    // on a genuine unplug — a fresh hot-plug is a fresh sink.
    if (hpd_present == 1'b0) begin
        defer_cnt   <= 6'd0;
        edid_giveup <= 1'b0;
        edid_done   <= 1'b0;
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
        rxc_nz_r      <= (rx_byte_count != 8'hFF);   // (count+1) != 0
        rxc_p1_r      <= rx_byte_count + 8'd2;        // (count+1) + 1
    end

    //---------------------------------------
    // Manage the reset timer
    //---------------------------------------
    // (138B clk100 ladder, 2026-10-24) both down-counters reloaded off a
    // live 29-/27-bit "== 0" compare of their own bits (retry_count ->
    // retry_count RESET, 8 cells, the gate's clk100 floor after the first
    // fix set). The zero flags are registers kept equal to the compare on
    // every cycle by construction: reload -> (reload value == 0), count ->
    // (count == 1 now <=> count-1 == 0 next). Counter values and the
    // retry_now / link_check_now pulses are bit-identical.
    if(retry_zero_r) begin
        retry_now    <= 1'b1;
        // Blind mode walks the ladder at ~84 ms/state (~2.1 s total) —
        // the 0.5 s watchdog reset every walk forever (live-hit
        // 2026-08-14: D cycling 02..06). 4 s keeps the watchdog while
        // clearing the full walk with 2x margin.
        retry_count  <= RETRY_RELOAD;
        retry_zero_r <= (RETRY_RELOAD == 29'd0);
    end else begin
        retry_now    <= 1'b0;
        retry_count  <= retry_count - 1;
        retry_zero_r <= (retry_count == 29'd1);
    end
    if(lc_zero_r) begin
        link_check_now   <= 1'b1;
        // PPS actually became a 2Hz pulse....
        link_check_count <= LINK_CHECK_CLKS;
        lc_zero_r        <= (LINK_CHECK_CLKS == 27'd0);
    end else begin
        link_check_now   <= 1'b0;
        link_check_count <= link_check_count - 1;
        lc_zero_r        <= (link_check_count == 27'd1);
    end

    //-----------------------------------------------------------
    // WEDGE-SUSPECT DETECTOR (advisory only — see parameter comment).
    // Timer accumulates ONLY while: established, previously streamed,
    // sink now reporting not-streaming, AND 0x204 bit7 reading stuck
    // set (a healthy hub clears it on read). Everything else resets it.
    //-----------------------------------------------------------
    if (!in_established_set ||
        dbg_sink_status[1:0] != 2'b00 || !r204_q[7]) begin
        wedge_lo       <= WEDGE_PRELOAD[15:0];
        wedge_hi       <= WEDGE_PRELOAD[31:16];
        wedge_lo_max_r <= (WEDGE_PRELOAD[15:0] == 16'hFFFF);
        if (dbg_sink_status[1:0] != 2'b00)
            wedge_suspect <= 1'b0;         // streaming again: stand down
    end else if (wedge_timer[WEDGE_BIT]) begin
        wedge_suspect <= 1'b1;
    end else begin
        wedge_lo       <= wedge_lo + 16'd1;
        wedge_lo_max_r <= (wedge_lo == 16'hFFFE);
        if (wedge_lo_max_r)
            wedge_hi   <= wedge_hi + 16'd1;
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
            ns_f_v = 1'b1;                 // -> reset
            `SET_STATE(error)
            err_evt    <= 1'b1; err_reason <= 4'd6;
        end else
            kick_timer <= kick_timer + 30'd1;
    end

    //-----------------------------------------------------------
    // next_state: apply the request groups in the original priority
    // order (kick > hpd > link check > timeouts > reply processing >
    // wait advances); no request = hold. See the ns_* declaration.
    //-----------------------------------------------------------
    next_state <= ns_f_v ? reset :
                  ns_e_v ? reset :
                  ns_d_v ? check_link :
                  ns_c_v ? ns_c :
                  ns_b_v ? ns_b :
                  ns_a_v ? state_on_success : next_state;
    // the same selection, for the lane-set flag of the value written
    ns_laneset_r <= ns_f_v ? 1'b0 :
                    ns_e_v ? 1'b0 :
                    ns_d_v ? 1'b0 :
                    ns_c_v ? ns_c_ls :
                    ns_b_v ? ns_b_ls :
                    ns_a_v ? sos_laneset_r : ns_laneset_r;
end        
`undef SET_STATE
endmodule
