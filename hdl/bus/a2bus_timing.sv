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

    // Lock state machine
    localparam [1:0] ST_UNLOCKED  = 2'd0;
    localparam [1:0] ST_ACQUIRING = 2'd1;
    localparam [1:0] ST_LOCKED    = 2'd2;

    logic [1:0] lock_state_r;
    logic [3:0] good_edge_count_r;     // consecutive valid edges
    logic [3:0] miss_edge_count_r;     // consecutive missed edges
    logic       lock_r;
    logic       error_r;

    // Track time since last predicted 7M rising edge for acceptance window
    logic [5:0] ticks_since_7m_edge_r;

    always_ff @(posedge clk_logic_i) begin
        if (pred_7m_posedge_w)
            ticks_since_7m_edge_r <= 6'd0;
        else if (ticks_since_7m_edge_r != 6'h3F)
            ticks_since_7m_edge_r <= ticks_since_7m_edge_r + 6'd1;
    end

    // A CDC'd 7M rising edge is "valid" if it arrives within a reasonable
    // window around the predicted edge. Since the predicted output is
    // approximately aligned to real input timing (via accumulator pre-load),
    // the CDC'd edge arrives roughly CDC_LATENCY clocks after the predicted
    // edge. However, fractional-N jitter means the predicted 7M period
    // alternates between floor and ceil of the true period (~7 and ~8 clocks
    // at 54MHz). An edge arriving at ticks_since near 0 (just after the
    // predicted edge) or near the period length (just before the NEXT
    // predicted edge) are both valid.
    //
    // The 7M half-period in logic clocks:
    //   CLOCK_SPEED_HZ / APPLE_HZ * 2 ≈ 7.54 for 54M/14.318M
    // Accept edges within ACCEPT_WINDOW of 0 or within ACCEPT_WINDOW of the
    // predicted period boundary.
    localparam int unsigned TICKS_PER_7M_HALF = (2 * CLOCK_SPEED_HZ + APPLE_HZ - 1) / APPLE_HZ; // ceil ≈ 8
    wire [5:0] edge_offset_w = ticks_since_7m_edge_r;
    wire edge_near_current_w = (edge_offset_w <= (CDC_LATENCY + ACCEPT_WINDOW));
    wire edge_near_next_w    = (edge_offset_w >= (TICKS_PER_7M_HALF - ACCEPT_WINDOW));
    wire edge_in_window_w    = cdc_7m_posedge_w && (edge_near_current_w || edge_near_next_w);

    // A CDC'd 7M rising edge is "unexpected" if it arrives outside the window
    wire edge_outside_window_w = cdc_7m_posedge_w && !edge_in_window_w;

    // Phase correction cooldown to prevent oscillation
    logic [5:0] correction_cooldown_r;

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
    // After snapping to phase 0 on Phi1 posedge, subsequent 7M edges
    // are used for fine tracking and lock verification.

    always_ff @(posedge clk_logic_i) begin
        // Default: clear one-shot signals
        snap_phase_r <= 1'b0;
        apply_correction_slow_r <= 1'b0;
        apply_correction_fast_r <= 1'b0;

        if (correction_cooldown_r != 6'd0)
            correction_cooldown_r <= correction_cooldown_r - 6'd1;

        case (lock_state_r)

            ST_UNLOCKED: begin
                lock_r <= 1'b0;
                good_edge_count_r <= 4'd0;
                miss_edge_count_r <= 4'd0;
                correction_cooldown_r <= 6'd0;

                // Snap on Phi1 posedge (unambiguous phase 0 reference).
                // Then use 7M edges for lock verification.
                if (cdc_phi1_posedge_w) begin
                    snap_phase_r <= 1'b1;
                    snap_phase_value_r <= 4'd0;
                    lock_state_r <= ST_ACQUIRING;
                    good_edge_count_r <= 4'd0;
                end
            end

            ST_ACQUIRING: begin
                lock_r <= 1'b0;

                if (edge_in_window_w) begin
                    good_edge_count_r <= good_edge_count_r + 4'd1;
                    miss_edge_count_r <= 4'd0;
                    if (good_edge_count_r >= LOCK_THRESHOLD - 1) begin
                        lock_state_r <= ST_LOCKED;
                        lock_r <= 1'b1;
                        error_r <= 1'b0;  // clear error on lock
                    end
                end else if (edge_outside_window_w) begin
                    miss_edge_count_r <= miss_edge_count_r + 4'd1;
                    good_edge_count_r <= 4'd0;
                    if (miss_edge_count_r >= UNLOCK_THRESHOLD - 1) begin
                        lock_state_r <= ST_UNLOCKED;
                    end
                end
            end

            ST_LOCKED: begin
                lock_r <= 1'b1;

                if (edge_in_window_w) begin
                    miss_edge_count_r <= 4'd0;

                    // Fine phase correction: nudge accumulator if the CDC'd edge
                    // is slightly early or late relative to the predicted edge.
                    // Edges near 0 arrived just after the predicted edge (on time
                    // or generator slightly fast). Edges near the period boundary
                    // arrived just before the next predicted edge (generator
                    // slightly slow). Only correct if clearly off-center.
                    if (correction_cooldown_r == 6'd0) begin
                        if (edge_near_next_w && !edge_near_current_w)
                            // CDC edge arrived late (near next predicted edge)
                            // → generator is too slow, speed up
                            apply_correction_fast_r <= 1'b1;
                        else if (edge_near_current_w && edge_offset_w > CDC_LATENCY[5:0])
                            // CDC edge arrived later than expected CDC latency
                            // → generator is slightly fast, slow down
                            apply_correction_slow_r <= 1'b1;
                        if (edge_near_next_w || edge_offset_w > CDC_LATENCY[5:0])
                            correction_cooldown_r <= 6'd8;
                    end
                end else if (edge_outside_window_w) begin
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
    // Phi1 Cross-check and Extended Cycle Detection
    // =========================================================================

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
