//
// Apple II Bus Timing - Predictive Clock Generator
//
// (c) 2023,2024 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// Description:
//
// Predictive clock generator for Apple II bus timing. Instead of directly
// passing CDC-synchronized input clocks to outputs (which adds latency and
// is susceptible to noise), this module runs a free-running clock generation
// engine at nominal Apple II frequencies and phase-locks it to the incoming
// signals. Spurious noise on input clocks cannot corrupt the output timing
// because the predictive engine is authoritative and input edges only serve
// to correct accumulated phase drift.
//
// Architecture (three layers):
//
//   1. CDC Input Path — cdc.sv or cdc_denoise.sv synchronizes raw Apple II
//      signals into the FPGA clock domain. These have 2-5 cycle latency but
//      are only used for phase comparison, never for output generation.
//
//   2. Predictive Engine — A fractional-N accumulator generates 14M ticks at
//      APPLE_HZ average rate. A 4-bit phase counter (0-13) maps each tick to
//      output clock states via a lookup table. This runs continuously and
//      drives ALL outputs with zero added latency beyond one register stage.
//
//   3. Phase Lock Controller — Compares CDC'd input edges against predicted
//      edges. Applies small corrections to maintain alignment. Rejects noise
//      by only accepting edges within a timing window.
//
// The Apple II uses a 14.318181 MHz master oscillator divided by 14 to
// produce the CPU clock. A 4-bit phase counter (0-13) within each CPU
// cycle determines the state of 7M, Q3, and Phi0/Phi1.
//
// APPLE_HZ parameter:
//   NTSC Apple II/IIe/IIc/IIgs: 14_318_181 (315/22 MHz, 4x NTSC subcarrier)
//   PAL  Apple II Europlus/IIe:  14_250_450 (from 15,625 Hz × 912 pixels/line)
//   PAL  Apple IIc:              14_250_000 (exact 15,625 × 912)
//   The phase lock controller is frequency-agnostic — it tracks whatever
//   input clock it sees. Using the correct APPLE_HZ reduces correction
//   effort and output jitter, but the module will lock and function
//   correctly even with a ~0.5% frequency mismatch.
//

module a2bus_timing #(
    parameter int CLOCK_SPEED_HZ = 54_000_000,                      // 18.52 ns
    parameter int APPLE_HZ = 14_318_181,
    parameter bit ENABLE_DENOISE = 0                                 // 0 = use cdc, 1 = use cdc_denoise
) (
    input  clk_logic_i,
    input  a2_phi1_i,
    input  a2_q3_i,
    input  a2_7M_i,

    output lock,
    output error,

    output phi0_o,
    output phi0_posedge_o,
    output phi0_negedge_o,

    output phi1_o,
    output phi1_posedge_o,
    output phi1_negedge_o,

    output cycle_extended,

    output q3_o,
    output q3_posedge_o,
    output q3_negedge_o,

    output clk_7M_o,
    output clk_7M_posedge_o,
    output clk_7M_negedge_o,

    output clk_14M_posedge_o
);

    // =========================================================================
    // Constants
    // =========================================================================

    localparam int unsigned TICK_RATE = APPLE_HZ;                    // 14,318,181
    localparam int unsigned ACC_WIDTH = $clog2(CLOCK_SPEED_HZ);      // 26 bits for 54M
    typedef logic [ACC_WIDTH:0] acc_t;                               // +1 for overflow

    // Phase counter runs 0-13, one CPU cycle = 14 half-periods of 14M
    localparam int unsigned PHASE_COUNT = 14;

    // Phase lock parameters
    // The CDC synchronizer adds a fixed latency of ~3 logic clocks (3-stage FF chain
    // + edge detection). We expect the CDC'd edge to arrive CDC_LATENCY clocks after
    // the predicted edge, and accept it within ±ACCEPT_WINDOW of that offset.
    localparam int unsigned CDC_LATENCY     = ENABLE_DENOISE ? 5 : 3;  // expected CDC pipeline delay
    localparam int unsigned ACCEPT_WINDOW   = 2;   // ±2 logic clocks around CDC_LATENCY
    localparam int unsigned LOCK_THRESHOLD  = 8;   // consecutive good edges to achieve lock
    localparam int unsigned UNLOCK_THRESHOLD = 4;  // consecutive missed edges to lose lock

    // When we snap on a CDC'd edge, the real transition happened CDC_LATENCY
    // clocks ago. Pre-load the accumulator to compensate so the first tick
    // fires at the correct time rather than being delayed by CDC_LATENCY clocks.
    // Each clock adds TICK_RATE to the accumulator, so CDC_LATENCY clocks
    // corresponds to CDC_LATENCY * TICK_RATE in accumulator units.
    localparam acc_t SNAP_ACC_PRELOAD = acc_t'(CDC_LATENCY) * acc_t'(TICK_RATE);

    // Extended cycle detection: if phi1 period exceeds this many logic clocks,
    // it's an extended (IIgs) cycle. An IIgs extended cycle is 16 ticks of 14M
    // instead of 14 (2 extra ticks during Phi1 HIGH). In logic clocks:
    //   Normal:   14 * (54M/14.318M) ≈ 52.8 clocks
    //   Extended: 16 * (54M/14.318M) ≈ 60.3 clocks
    // Set threshold midway between normal and extended (~56 clocks) to reliably
    // distinguish the two while tolerating fractional-N jitter of ±1 clock.
    localparam int unsigned NORMAL_PERIOD_CLKS = (CLOCK_SPEED_HZ * 14 + APPLE_HZ - 1) / APPLE_HZ;    // ceil(54M*14/14.318M) ≈ 53
    localparam int unsigned EXTENDED_PERIOD_CLKS = (CLOCK_SPEED_HZ * 16 + APPLE_HZ - 1) / APPLE_HZ;  // ceil(54M*16/14.318M) ≈ 61
    localparam int unsigned EXTENDED_THRESHOLD = (NORMAL_PERIOD_CLKS + EXTENDED_PERIOD_CLKS) / 2;     // midpoint ≈ 57

    // Extended cycle output reproduction: when an extended input cycle is
    // detected (at output phase 9, CDC'd Phi1 still HIGH), the phase counter
    // holds at phase 13 for 2 extra ticks before wrapping to phase 0.
    // This inserts the extra time at the end of the cycle (Phi1 LOW, Q3 LOW),
    // extending the total output period from 14 to 16 ticks to match input.
    //
    // Note: the extension time is placed in the Phi1 LOW portion of the cycle
    // rather than extending Phi1 HIGH, because we cannot reliably detect the
    // extended cycle before phase 7 (where Phi1 normally falls). At output
    // phase 6, cdc_phi1_w is HIGH for both normal and extended cycles since
    // the real Phi1 hasn't fallen yet in either case. The distinction only
    // becomes clear at phase 9+ when normal cycles have cdc_phi1_w LOW but
    // extended cycles still have it HIGH.

    // =========================================================================
    // Apple II Phase Map
    // =========================================================================
    //
    // The Apple II 14M oscillator feeds a divide-by-14 counter in the IOU/MMU.
    // 7M toggles every 14M tick. Phi1 and Q3 have specific transition points.
    //
    // From the Apple IIe HAL timing diagram and Apple II+ 74S195 shift register:
    //
    // Phase:  0  1  2  3  4  5  6  7  8  9 10 11 12 13
    // 7M:     0  1  0  1  0  1  0  1  0  1  0  1  0  1
    // Phi1:   1  1  1  1  1  1  1  0  0  0  0  0  0  0
    // Q3:     1  1  1  1  0  0  0  1  1  1  1  0  0  0
    //
    // Q3 rises with Phi1 rising (phase 0) and Phi1 falling (phase 7).
    // Q3 falls at phase 4 and phase 11.
    // Q3 has a 4-high/3-low/4-high/3-low asymmetric pattern per CPU cycle.
    // 7M = phase[0] (least significant bit of phase counter)

    // Output state lookup indexed by phase counter
    // Bits: [2]=7M, [1]=Phi1, [0]=Q3
    logic [2:0] PHASE_MAP [0:13];
    initial begin
        //              7M  Phi1  Q3
        PHASE_MAP[0]  = 3'b011;  // 7M=0, Phi1=1, Q3=1  (Phi1 rises, Q3 rises)
        PHASE_MAP[1]  = 3'b111;  // 7M=1, Phi1=1, Q3=1
        PHASE_MAP[2]  = 3'b011;  // 7M=0, Phi1=1, Q3=1
        PHASE_MAP[3]  = 3'b111;  // 7M=1, Phi1=1, Q3=1
        PHASE_MAP[4]  = 3'b010;  // 7M=0, Phi1=1, Q3=0  (Q3 falls)
        PHASE_MAP[5]  = 3'b110;  // 7M=1, Phi1=1, Q3=0
        PHASE_MAP[6]  = 3'b010;  // 7M=0, Phi1=1, Q3=0
        PHASE_MAP[7]  = 3'b101;  // 7M=1, Phi1=0, Q3=1  (Phi1 falls, Q3 rises)
        PHASE_MAP[8]  = 3'b001;  // 7M=0, Phi1=0, Q3=1
        PHASE_MAP[9]  = 3'b101;  // 7M=1, Phi1=0, Q3=1
        PHASE_MAP[10] = 3'b001;  // 7M=0, Phi1=0, Q3=1
        PHASE_MAP[11] = 3'b100;  // 7M=1, Phi1=0, Q3=0  (Q3 falls)
        PHASE_MAP[12] = 3'b000;  // 7M=0, Phi1=0, Q3=0
        PHASE_MAP[13] = 3'b100;  // 7M=1, Phi1=0, Q3=0
    end

    // =========================================================================
    // CDC Input Path
    // =========================================================================

    logic cdc_7m_w, cdc_7m_posedge_w, cdc_7m_negedge_w;
    logic cdc_phi1_w, cdc_phi1_posedge_w, cdc_phi1_negedge_w;
    logic cdc_q3_w, cdc_q3_posedge_w, cdc_q3_negedge_w;

    generate
        if (ENABLE_DENOISE) begin : gen_denoise
            cdc_denoise cdc_7m_inst (
                .clk(clk_logic_i),
                .i(a2_7M_i),
                .o(cdc_7m_w),
                .o_n(),
                .o_posedge(cdc_7m_posedge_w),
                .o_negedge(cdc_7m_negedge_w)
            );
            cdc_denoise cdc_phi1_inst (
                .clk(clk_logic_i),
                .i(a2_phi1_i),
                .o(cdc_phi1_w),
                .o_n(),
                .o_posedge(cdc_phi1_posedge_w),
                .o_negedge(cdc_phi1_negedge_w)
            );
            cdc_denoise cdc_q3_inst (
                .clk(clk_logic_i),
                .i(a2_q3_i),
                .o(cdc_q3_w),
                .o_n(),
                .o_posedge(cdc_q3_posedge_w),
                .o_negedge(cdc_q3_negedge_w)
            );
        end else begin : gen_cdc
            cdc cdc_7m_inst (
                .clk(clk_logic_i),
                .i(a2_7M_i),
                .o(cdc_7m_w),
                .o_n(),
                .o_posedge(cdc_7m_posedge_w),
                .o_negedge(cdc_7m_negedge_w)
            );
            cdc cdc_phi1_inst (
                .clk(clk_logic_i),
                .i(a2_phi1_i),
                .o(cdc_phi1_w),
                .o_n(),
                .o_posedge(cdc_phi1_posedge_w),
                .o_negedge(cdc_phi1_negedge_w)
            );
            cdc cdc_q3_inst (
                .clk(clk_logic_i),
                .i(a2_q3_i),
                .o(cdc_q3_w),
                .o_n(),
                .o_posedge(cdc_q3_posedge_w),
                .o_negedge(cdc_q3_negedge_w)
            );
        end
    endgenerate

    // =========================================================================
    // Phase Lock Controller (forward declarations)
    // =========================================================================

    logic snap_phase_w;              // snap request from lock controller
    logic [3:0] snap_phase_value_w;  // phase value to snap to
    logic apply_correction_slow_w;   // nudge accumulator: slow down (generator too fast)
    logic apply_correction_fast_w;   // nudge accumulator: speed up (generator too slow)
    logic extend_hold_w;             // hold phase counter for extended cycle output

    // =========================================================================
    // Fractional-N 14M Tick Generator + Phase Counter
    // =========================================================================
    //
    // Combined into a single always_ff to avoid multiple-driver conflicts.
    // The snap_phase_w signal overrides normal operation to re-align
    // the phase counter and accumulator to a detected input edge.
    // The apply_correction_w signal nudges the accumulator to compensate
    // for small phase drift.

    acc_t acc_14m_r;
    logic tick_14m_r;
    logic [3:0] phase_r;
    logic [3:0] prev_phase_r;

    // Combinational next-phase: computed before registration so we can use it
    // for zero-latency output decode. This is the phase value that phase_r
    // will hold on the next clock edge.
    logic [3:0] next_phase_w;
    logic       next_tick_w;
    acc_t       next_acc_w;

    always_comb begin
        next_phase_w = phase_r;
        next_tick_w  = 1'b0;

        if (snap_phase_w) begin
            // Pre-load accumulator to compensate for CDC latency.
            // The real edge happened CDC_LATENCY clocks ago, so the
            // accumulator should already be CDC_LATENCY * TICK_RATE ahead.
            // If that pre-load already exceeds CLOCK_SPEED_HZ, a tick should
            // have fired, so advance the phase by one.
            if (SNAP_ACC_PRELOAD >= CLOCK_SPEED_HZ) begin
                next_phase_w = (snap_phase_value_w == 4'd13) ? 4'd0 : snap_phase_value_w + 4'd1;
                next_acc_w   = SNAP_ACC_PRELOAD - CLOCK_SPEED_HZ;
                next_tick_w  = 1'b1;
            end else begin
                next_phase_w = snap_phase_value_w;
                next_acc_w   = SNAP_ACC_PRELOAD;
            end
        end else begin
            if (apply_correction_fast_w)
                next_acc_w = acc_14m_r + TICK_RATE + (TICK_RATE / 4);
            else if (apply_correction_slow_w)
                next_acc_w = acc_14m_r + TICK_RATE - (TICK_RATE / 4);
            else
                next_acc_w = acc_14m_r + TICK_RATE;

            if (next_acc_w >= CLOCK_SPEED_HZ) begin
                next_acc_w   = next_acc_w - CLOCK_SPEED_HZ;
                next_tick_w  = 1'b1;
                // Extended cycle hold: when phase is 13 and we've detected
                // an extended cycle, hold at phase 13 instead of wrapping
                // to phase 0. The accumulator still advances (tick fires)
                // but the phase counter stays put, effectively inserting
                // extra 14M ticks at the end of the cycle to match the
                // IIgs 16-tick extended cycle period.
                if (extend_hold_w && phase_r == 4'd13)
                    next_phase_w = 4'd13;  // hold: don't wrap to phase 0
                else
                    next_phase_w = (phase_r == 4'd13) ? 4'd0 : phase_r + 4'd1;
            end
        end
    end

    always_ff @(posedge clk_logic_i) begin
        prev_phase_r <= phase_r;
        tick_14m_r   <= next_tick_w;
        phase_r      <= next_phase_w;
        acc_14m_r    <= next_acc_w;
    end

    // =========================================================================
    // Output Decode from Phase Counter
    // =========================================================================
    //
    // Use next_phase_w (combinational) for output decode. This is the value
    // that phase_r will be on the next clock edge, so looking it up now means
    // the registered output reflects the correct state with zero additional
    // latency beyond the single register stage needed for clean FPGA outputs.
    //
    // Output pipeline:
    //   Cycle N: accumulator overflows, next_phase_w computed combinationally,
    //            PHASE_MAP[next_phase_w] available combinationally
    //   Cycle N+1 posedge: phase_out_r latches the new state, prev_phase_out_r
    //            latches the old state → edge pulses fire
    //
    // The level outputs (phi1_o, clk_7M_o, etc.) come from phase_out_r, which
    // transitions on the same clock edge as phase_r. Edge pulses fire the same
    // cycle by comparing phase_out_r vs prev_phase_out_r.

    logic [2:0] phase_out_r;
    logic [2:0] prev_phase_out_r;

    always_ff @(posedge clk_logic_i) begin
        prev_phase_out_r <= phase_out_r;
        phase_out_r <= PHASE_MAP[next_phase_w];
    end

    // Extract individual clock signals
    wire pred_7m_w   = phase_out_r[2];
    wire pred_phi1_w = phase_out_r[1];
    wire pred_q3_w   = phase_out_r[0];
    wire pred_phi0_w = ~pred_phi1_w;

    // Edge detection: compare current vs previous registered output
    wire pred_7m_posedge_w   = phase_out_r[2] & ~prev_phase_out_r[2];
    wire pred_7m_negedge_w   = ~phase_out_r[2] & prev_phase_out_r[2];
    wire pred_phi1_posedge_w = phase_out_r[1] & ~prev_phase_out_r[1];
    wire pred_phi1_negedge_w = ~phase_out_r[1] & prev_phase_out_r[1];
    wire pred_q3_posedge_w   = phase_out_r[0] & ~prev_phase_out_r[0];
    wire pred_q3_negedge_w   = ~phase_out_r[0] & prev_phase_out_r[0];

    // =========================================================================
    // Phase Lock Controller
    // =========================================================================
    //
    // Uses Phi1 posedge as the tracking reference instead of 7M. Phi1 occurs
    // once per CPU cycle (~53 FPGA clocks), giving much better resolution
    // for phase comparison than 7M (~3.77 FPGA clocks per half-period, which
    // is too short relative to CDC latency for meaningful discrimination).
    //
    // Tracking approach:
    //   - Count FPGA clocks since the predicted Phi1 posedge
    //   - When CDC'd Phi1 posedge arrives, check offset against CDC_LATENCY
    //   - If offset < CDC_LATENCY: predicted edge was late → speed up
    //   - If offset > CDC_LATENCY: predicted edge was early → slow down
    //   - If offset ≈ CDC_LATENCY: on time, no correction needed

    // Lock state machine
    localparam [1:0] ST_UNLOCKED  = 2'd0;
    localparam [1:0] ST_ACQUIRING = 2'd1;
    localparam [1:0] ST_LOCKED    = 2'd2;

    logic [1:0] lock_state_r;
    logic [3:0] good_edge_count_r;     // consecutive valid edges
    logic [3:0] miss_edge_count_r;     // consecutive missed edges
    logic       lock_r;
    logic       error_r;

    // Track time since last predicted Phi1 posedge for phase comparison
    // CPU cycle is ~53 FPGA clocks, so 7 bits is plenty
    logic [6:0] ticks_since_phi1_edge_r;

    always_ff @(posedge clk_logic_i) begin
        if (pred_phi1_posedge_w)
            ticks_since_phi1_edge_r <= 7'd0;
        else if (ticks_since_phi1_edge_r != 7'h7F)
            ticks_since_phi1_edge_r <= ticks_since_phi1_edge_r + 7'd1;
    end

    // CPU cycle period in FPGA clocks (for acceptance window sizing)
    // Normal: 14 * (54M/14.318M) ≈ 52.8 clocks
    localparam int unsigned TICKS_PER_CPU_CYCLE = (CLOCK_SPEED_HZ * 14 + APPLE_HZ - 1) / APPLE_HZ; // ceil ≈ 53

    // Acceptance window: CDC'd Phi1 posedge should arrive CDC_LATENCY clocks
    // after predicted Phi1 posedge, ±ACCEPT_WINDOW for jitter tolerance.
    // With ~53 clocks per cycle, a ±2 window around offset 3 (CDC_LATENCY)
    // is well-resolved: valid range is [1..5], clearly distinguishable from
    // "near next cycle" at ~50+.
    wire [6:0] phi1_offset_w = ticks_since_phi1_edge_r;

    // Valid edge: arrives within CDC_LATENCY ± ACCEPT_WINDOW of predicted edge
    wire phi1_near_current_w = (phi1_offset_w <= (CDC_LATENCY + ACCEPT_WINDOW));
    // Also accept edges that arrive near the NEXT predicted Phi1 posedge
    // (generator drifted almost a full cycle — edge is "early" for next cycle)
    wire phi1_near_next_w    = (phi1_offset_w >= (TICKS_PER_CPU_CYCLE - ACCEPT_WINDOW));
    wire phi1_in_window_w    = cdc_phi1_posedge_w && (phi1_near_current_w || phi1_near_next_w);

    // Unexpected edge: outside both windows
    wire phi1_outside_window_w = cdc_phi1_posedge_w && !phi1_in_window_w;

    // Phase correction cooldown to prevent oscillation (one correction per CPU cycle)
    logic [6:0] correction_cooldown_r;

    // Snap and correction output signals (directly wired to tick generator)
    logic snap_phase_r;
    logic [3:0] snap_phase_value_r;
    logic apply_correction_slow_r;
    logic apply_correction_fast_r;

    assign snap_phase_w = snap_phase_r;
    assign snap_phase_value_w = snap_phase_value_r;
    assign apply_correction_slow_w = apply_correction_slow_r;
    assign apply_correction_fast_w = apply_correction_fast_r;

    // Phase snap uses Phi1 posedge as the primary reference because it's
    // unambiguous — Phi1 rises exactly once per CPU cycle at phase 0.
    // After snapping to phase 0 on Phi1 posedge, subsequent Phi1 posedges
    // are used for fine tracking and lock verification.

    always_ff @(posedge clk_logic_i) begin
        // Default: clear one-shot signals
        snap_phase_r <= 1'b0;
        apply_correction_slow_r <= 1'b0;
        apply_correction_fast_r <= 1'b0;

        if (correction_cooldown_r != 7'd0)
            correction_cooldown_r <= correction_cooldown_r - 7'd1;

        case (lock_state_r)

            ST_UNLOCKED: begin
                lock_r <= 1'b0;
                good_edge_count_r <= 4'd0;
                miss_edge_count_r <= 4'd0;
                correction_cooldown_r <= 7'd0;

                // Snap on Phi1 posedge (unambiguous phase 0 reference).
                if (cdc_phi1_posedge_w) begin
                    snap_phase_r <= 1'b1;
                    snap_phase_value_r <= 4'd0;
                    lock_state_r <= ST_ACQUIRING;
                    good_edge_count_r <= 4'd0;
                end
            end

            ST_ACQUIRING: begin
                lock_r <= 1'b0;

                if (phi1_in_window_w) begin
                    good_edge_count_r <= good_edge_count_r + 4'd1;
                    miss_edge_count_r <= 4'd0;
                    if (good_edge_count_r >= LOCK_THRESHOLD - 1) begin
                        lock_state_r <= ST_LOCKED;
                        lock_r <= 1'b1;
                        error_r <= 1'b0;  // clear error on lock
                    end
                end else if (phi1_outside_window_w) begin
                    miss_edge_count_r <= miss_edge_count_r + 4'd1;
                    good_edge_count_r <= 4'd0;
                    if (miss_edge_count_r >= UNLOCK_THRESHOLD - 1) begin
                        lock_state_r <= ST_UNLOCKED;
                    end
                end
            end

            ST_LOCKED: begin
                lock_r <= 1'b1;

                if (phi1_in_window_w) begin
                    miss_edge_count_r <= 4'd0;

                    // Fine phase correction using Phi1 edge offset.
                    // Expected offset is CDC_LATENCY clocks.
                    // With ~53 clocks per CPU cycle, we have good resolution.
                    if (correction_cooldown_r == 7'd0) begin
                        if (phi1_near_next_w && !phi1_near_current_w) begin
                            // CDC edge arrived near next predicted edge
                            // → generator is too slow, speed up
                            apply_correction_fast_r <= 1'b1;
                            correction_cooldown_r <= TICKS_PER_CPU_CYCLE[6:0];
                        end else if (phi1_near_current_w && phi1_offset_w > CDC_LATENCY[6:0]) begin
                            // CDC edge arrived later than expected CDC latency
                            // → generator is slightly fast, slow down
                            apply_correction_slow_r <= 1'b1;
                            correction_cooldown_r <= TICKS_PER_CPU_CYCLE[6:0];
                        end else if (phi1_near_current_w && phi1_offset_w < CDC_LATENCY[6:0]) begin
                            // CDC edge arrived earlier than expected CDC latency
                            // → generator is slightly slow, speed up
                            apply_correction_fast_r <= 1'b1;
                            correction_cooldown_r <= TICKS_PER_CPU_CYCLE[6:0];
                        end
                        // If offset == CDC_LATENCY exactly, no correction needed
                    end
                end else if (phi1_outside_window_w) begin
                    miss_edge_count_r <= miss_edge_count_r + 4'd1;
                    good_edge_count_r <= 4'd0;
                    error_r <= 1'b1;  // sticky error
                    if (miss_edge_count_r >= UNLOCK_THRESHOLD - 1) begin
                        lock_state_r <= ST_UNLOCKED;
                        lock_r <= 1'b0;
                    end
                end
            end

            default: begin
                lock_state_r <= ST_UNLOCKED;
            end

        endcase
    end

    // =========================================================================
    // Extended Cycle Detection and Output Reproduction
    // =========================================================================
    //
    // Two functions:
    //   1. Detect extended cycles (for the cycle_extended status flag)
    //   2. Reproduce extended cycles in output to prevent drift
    //
    // Detection (status flag): measure FPGA clocks between CDC'd Phi1
    // posedges. If the period exceeds EXTENDED_THRESHOLD, flag it.
    //
    // Output reproduction: detect mid-cycle at output phase 9 by checking
    // if cdc_phi1_w is still HIGH (normal cycles have it LOW by then).
    // When detected, hold the phase counter at phase 13 for 2 extra ticks
    // before wrapping to phase 0. This extends the total output cycle
    // from 14 to 16 ticks, matching the IIgs extended cycle period and
    // preventing cumulative drift (2 ticks per extended cycle, every 65th
    // cycle on IIgs).

    logic [6:0] phi1_period_counter_r;
    logic       cycle_extended_r;

    always_ff @(posedge clk_logic_i) begin
        cycle_extended_r <= 1'b0;

        if (cdc_phi1_posedge_w) begin
            // Check if the period was longer than expected (extended cycle)
            if (phi1_period_counter_r > EXTENDED_THRESHOLD[6:0])
                cycle_extended_r <= 1'b1;
            phi1_period_counter_r <= 7'd0;
        end else if (phi1_period_counter_r != 7'h7F) begin
            phi1_period_counter_r <= phi1_period_counter_r + 7'd1;
        end
    end

    // Extended cycle output reproduction
    //
    // Detect extended cycles mid-cycle by counting FPGA clocks after the
    // predicted Phi1 negedge and checking if cdc_phi1_w is still HIGH.
    //
    // After the predicted Phi1 negedge (phase 7):
    //   - Normal cycle: real Phi1 fell at input phase 7 (approximately
    //     simultaneous with predicted). CDC propagation takes CDC_LATENCY
    //     clocks. So cdc_phi1_w goes LOW about CDC_LATENCY clocks later.
    //   - Extended cycle: real Phi1 stays HIGH for 2 more ticks after
    //     input phase 7 (falls at phase 9). cdc_phi1_w stays HIGH for
    //     2*3.77 + CDC_LATENCY ≈ 10.5 clocks after predicted negedge.
    //
    // Check point: CDC_LATENCY + ACCEPT_WINDOW + 1 clocks after predicted
    // negedge. At this point:
    //   - Normal: cdc_phi1_w has been LOW for ~ACCEPT_WINDOW+1 clocks
    //   - Extended: cdc_phi1_w is still HIGH (~7+ clocks until it goes LOW)
    //
    // This gives reliable discrimination with good margin.
    //
    // Compensation: hold at phase 13 for 2 extra ticks. This inserts
    // the extra time in the Phi1 LOW / Q3 LOW portion of the cycle.
    // Total output cycle = 14 + 2 = 16 ticks, matching IIgs timing.

    localparam int unsigned EXTEND_CHECK_DELAY = CDC_LATENCY + ACCEPT_WINDOW + 2;  // ~7 clocks

    logic current_cycle_extended_r;
    logic [1:0] extend_hold_count_r;
    logic [3:0] clks_after_phi1_negedge_r;
    logic       phi1_negedge_seen_r;

    always_ff @(posedge clk_logic_i) begin
        if (pred_phi1_posedge_w) begin
            // New cycle (Phi1 just rose): reset state for this cycle
            current_cycle_extended_r <= 1'b0;
            extend_hold_count_r <= 2'd0;
            phi1_negedge_seen_r <= 1'b0;
            clks_after_phi1_negedge_r <= 4'd0;
        end else begin
            // Track clocks since predicted Phi1 negedge
            if (pred_phi1_negedge_w) begin
                phi1_negedge_seen_r <= 1'b1;
                clks_after_phi1_negedge_r <= 4'd0;
            end else if (phi1_negedge_seen_r && clks_after_phi1_negedge_r < 4'd15) begin
                clks_after_phi1_negedge_r <= clks_after_phi1_negedge_r + 4'd1;
            end

            // Detect extended cycle: check cdc_phi1_w at the check point
            if (phi1_negedge_seen_r &&
                clks_after_phi1_negedge_r == EXTEND_CHECK_DELAY[3:0] &&
                cdc_phi1_w && lock_r && !current_cycle_extended_r)
                current_cycle_extended_r <= 1'b1;

            // Count hold ticks when holding at phase 13
            if (next_tick_w && extend_hold_w)
                extend_hold_count_r <= extend_hold_count_r + 2'd1;
        end
    end

    // Hold signal: active when extended cycle detected, at phase 13,
    // with hold budget remaining (max 2 extra ticks).
    assign extend_hold_w = current_cycle_extended_r && (phase_r == 4'd13)
                           && (extend_hold_count_r < 2'd2) && lock_r;

    // =========================================================================
    // Output Assignments
    // =========================================================================

    assign phi0_o          = pred_phi0_w;
    assign phi0_posedge_o  = pred_phi1_negedge_w;   // phi0 posedge = phi1 negedge
    assign phi0_negedge_o  = pred_phi1_posedge_w;   // phi0 negedge = phi1 posedge

    assign phi1_o          = pred_phi1_w;
    assign phi1_posedge_o  = pred_phi1_posedge_w;
    assign phi1_negedge_o  = pred_phi1_negedge_w;

    assign q3_o            = pred_q3_w;
    assign q3_posedge_o    = pred_q3_posedge_w;
    assign q3_negedge_o    = pred_q3_negedge_w;

    assign clk_7M_o        = pred_7m_w;
    assign clk_7M_posedge_o = pred_7m_posedge_w;
    assign clk_7M_negedge_o = pred_7m_negedge_w;

    assign clk_14M_posedge_o = pred_7m_posedge_w | pred_7m_negedge_w;

    assign lock            = lock_r;
    assign error           = error_r;
    assign cycle_extended  = cycle_extended_r;

endmodule
