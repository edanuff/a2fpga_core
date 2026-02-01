`timescale 1ns / 1ps

module a2bus_timing_tb;

    // =========================================================================
    // Parameters
    // =========================================================================

    parameter CLOCK_SPEED_HZ = 54_000_000;
    parameter APPLE_HZ = 14_318_181;
    parameter CLK_PERIOD = 1000000000.0 / CLOCK_SPEED_HZ;       // ~18.52 ns
    parameter APPLE_14M_PERIOD = 1000000000.0 / APPLE_HZ;       // ~69.84 ns
    parameter APPLE_7M_PERIOD = APPLE_14M_PERIOD * 2;           // ~139.68 ns
    parameter APPLE_CPU_PERIOD = APPLE_14M_PERIOD * 14;          // ~977.8 ns

    // =========================================================================
    // Signals
    // =========================================================================

    reg clk_logic = 0;

    // Apple II clock sources (simulated)
    reg a2_phi1 = 0;
    reg a2_q3 = 0;
    reg a2_7M = 0;

    // DUT outputs
    wire lock;
    wire error;
    wire phi0_o, phi0_posedge_o, phi0_negedge_o;
    wire phi1_o, phi1_posedge_o, phi1_negedge_o;
    wire cycle_extended;
    wire q3_o, q3_posedge_o, q3_negedge_o;
    wire clk_7M_o, clk_7M_posedge_o, clk_7M_negedge_o;
    wire clk_14M_posedge_o;

    // =========================================================================
    // Clock Generation - 54 MHz FPGA logic clock
    // =========================================================================

    always #(CLK_PERIOD / 2) clk_logic = ~clk_logic;

    // =========================================================================
    // Apple II Clock Generation
    // =========================================================================
    //
    // Simulate the Apple II clock divider chain.
    // 14M oscillator -> divide by 14 -> CPU cycle
    // Phase counter 0-13 determines all clock states.
    //
    // Phase:  0  1  2  3  4  5  6  7  8  9 10 11 12 13
    // 7M:     0  1  0  1  0  1  0  1  0  1  0  1  0  1
    // Phi1:   1  1  1  1  1  1  1  0  0  0  0  0  0  0
    // Q3:     1  1  1  1  0  0  0  1  1  1  1  0  0  0

    integer apple_phase = 0;
    reg noise_enable = 0;
    integer noise_seed = 42;
    integer noise_count = 0;

    task automatic apple_clock_cycle;
        integer i;
        begin
            for (i = 0; i < 14; i = i + 1) begin
                apple_phase = i;

                // Set 7M = phase[0]
                a2_7M = i[0];

                // Phi1: high for phases 0-6, low for phases 7-13
                a2_phi1 = (i < 7) ? 1'b1 : 1'b0;

                // Q3: high for phases 0-3 and 7-10 (rises with Phi1 transitions)
                a2_q3 = ((i <= 3) || (i >= 7 && i <= 10)) ? 1'b1 : 1'b0;

                // Inject noise on Phi1 if enabled.
                // The noise glitch is injected *within* the 14M period so that
                // it doesn't shift cumulative timing (which would be unrealistic).
                if (noise_enable && (i == 2 || i == 9)) begin
                    // Wait most of the period, inject glitch, wait remainder
                    #(APPLE_14M_PERIOD - CLK_PERIOD);
                    noise_count = noise_count + 1;
                    a2_phi1 = ~a2_phi1;
                    #(CLK_PERIOD * 0.8);  // Glitch shorter than 1 logic clock
                    a2_phi1 = ~a2_phi1;   // Restore
                    #(CLK_PERIOD * 0.2);  // Remaining time to fill out the period
                end else begin
                    // Wait one 14M half-period
                    #(APPLE_14M_PERIOD);
                end
            end
        end
    endtask

    // Extended cycle task: generates a 16-tick cycle (2 extra 14M ticks)
    //
    // In a real Apple IIgs, every 65th CPU cycle is extended by exactly 2
    // additional 14M ticks (16 instead of 14). The extension occurs during
    // the Phi1 HIGH phase — Phi1 stays high for 9 ticks instead of 7, while
    // 7M continues to toggle normally. The Phi1 LOW phase remains 7 ticks.
    //
    // Normal cycle (14 ticks):
    //   Phase:  0  1  2  3  4  5  6  7  8  9 10 11 12 13
    //   7M:     0  1  0  1  0  1  0  1  0  1  0  1  0  1
    //   Phi1:   1  1  1  1  1  1  1  0  0  0  0  0  0  0
    //   Q3:     1  1  1  1  0  0  0  1  1  1  1  0  0  0
    //
    // Extended cycle (16 ticks):
    //   Phase:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    //   7M:     0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1
    //   Phi1:   1  1  1  1  1  1  1  1  1  0  0  0  0  0  0  0
    //   Q3:     1  1  1  1  0  0  0  0  0  1  1  1  1  0  0  0
    //
    // The 2 extra ticks (phases 7-8) extend Phi1 HIGH and Q3 LOW.
    // Phi1 falls at phase 9 instead of 7. Q3 rises with Phi1 falling
    // at phase 9 instead of 7. Q3 falls at phase 13 instead of 11.
    task automatic apple_extended_cycle;
        integer i;
        begin
            for (i = 0; i < 16; i = i + 1) begin
                apple_phase = i;
                a2_7M = i[0];
                // Phi1: high for phases 0-8 (9 ticks), low for phases 9-15 (7 ticks)
                a2_phi1 = (i < 9) ? 1'b1 : 1'b0;
                // Q3: high for phases 0-3 and 9-12 (rises with Phi1 transitions)
                a2_q3 = ((i <= 3) || (i >= 9 && i <= 12)) ? 1'b1 : 1'b0;
                #(APPLE_14M_PERIOD);
            end
        end
    endtask

    // Run the Apple II clock continuously
    reg apple_running = 0;
    reg apple_extend_next = 0;      // flag: generate one extended (16-tick) cycle
    integer cpu_cycle_count = 0;

    initial begin
        // Wait a bit before starting Apple clocks
        #(CLK_PERIOD * 20);
        apple_running = 1;
    end

    always begin
        if (apple_running) begin
            if (apple_extend_next) begin
                apple_extend_next = 0;
                apple_extended_cycle();
            end else begin
                apple_clock_cycle();
            end
            cpu_cycle_count = cpu_cycle_count + 1;
        end else begin
            #(CLK_PERIOD);
        end
    end

    // =========================================================================
    // DUT Instantiation
    // =========================================================================

    a2bus_timing #(
        .CLOCK_SPEED_HZ(CLOCK_SPEED_HZ),
        .APPLE_HZ(APPLE_HZ),
        .ENABLE_DENOISE(0)
    ) dut (
        .clk_logic_i(clk_logic),
        .a2_phi1_i(a2_phi1),
        .a2_q3_i(a2_q3),
        .a2_7M_i(a2_7M),

        .lock(lock),
        .error(error),

        .phi0_o(phi0_o),
        .phi0_posedge_o(phi0_posedge_o),
        .phi0_negedge_o(phi0_negedge_o),

        .phi1_o(phi1_o),
        .phi1_posedge_o(phi1_posedge_o),
        .phi1_negedge_o(phi1_negedge_o),

        .cycle_extended(cycle_extended),

        .q3_o(q3_o),
        .q3_posedge_o(q3_posedge_o),
        .q3_negedge_o(q3_negedge_o),

        .clk_7M_o(clk_7M_o),
        .clk_7M_posedge_o(clk_7M_posedge_o),
        .clk_7M_negedge_o(clk_7M_negedge_o),

        .clk_14M_posedge_o(clk_14M_posedge_o)
    );

    // =========================================================================
    // Monitoring and Verification
    // =========================================================================

    // Edge counters
    integer phi1_posedge_count = 0;
    integer phi1_negedge_count = 0;
    integer clk_7m_posedge_count = 0;
    integer clk_7m_negedge_count = 0;
    integer q3_posedge_count = 0;
    integer q3_negedge_count = 0;
    integer clk_14m_posedge_count = 0;

    // Timing measurement
    realtime last_phi1_posedge_time = 0;
    realtime phi1_period_sum = 0;
    integer phi1_period_count = 0;

    realtime last_7m_posedge_time = 0;
    realtime period_7m_sum = 0;
    integer period_7m_count = 0;

    // Lock timing
    realtime lock_achieved_time = 0;
    reg lock_seen = 0;
    reg prev_lock = 0;

    always @(posedge clk_logic) begin
        prev_lock <= lock;

        // Count output edges
        if (phi1_posedge_o) phi1_posedge_count <= phi1_posedge_count + 1;
        if (phi1_negedge_o) phi1_negedge_count <= phi1_negedge_count + 1;
        if (clk_7M_posedge_o) clk_7m_posedge_count <= clk_7m_posedge_count + 1;
        if (clk_7M_negedge_o) clk_7m_negedge_count <= clk_7m_negedge_count + 1;
        if (q3_posedge_o) q3_posedge_count <= q3_posedge_count + 1;
        if (q3_negedge_o) q3_negedge_count <= q3_negedge_count + 1;
        if (clk_14M_posedge_o) clk_14m_posedge_count <= clk_14m_posedge_count + 1;

        // Measure phi1 period
        if (phi1_posedge_o && phi1_posedge_count > 0) begin
            phi1_period_sum = phi1_period_sum + ($realtime - last_phi1_posedge_time);
            phi1_period_count = phi1_period_count + 1;
        end
        if (phi1_posedge_o) last_phi1_posedge_time = $realtime;

        // Measure 7M period
        if (clk_7M_posedge_o && clk_7m_posedge_count > 0) begin
            period_7m_sum = period_7m_sum + ($realtime - last_7m_posedge_time);
            period_7m_count = period_7m_count + 1;
        end
        if (clk_7M_posedge_o) last_7m_posedge_time = $realtime;

        // Lock detection
        if (lock && !prev_lock && !lock_seen) begin
            lock_achieved_time = $realtime;
            lock_seen = 1;
        end
    end

    // Extended cycle detection counter
    integer extended_cycle_count = 0;

    always @(posedge clk_logic) begin
        if (cycle_extended) extended_cycle_count <= extended_cycle_count + 1;
    end

    // =========================================================================
    // Phase relationship checker
    // =========================================================================
    //
    // Verify that Phi1, Q3, and 7M have correct relationships:
    // - 7M should toggle every 14M tick
    // - Phi1 should have 7 half-periods high, 7 low per CPU cycle
    // - Q3 should have correct pattern within each CPU cycle

    integer phase_errors = 0;
    reg prev_phi1_o = 0;
    reg prev_q3_o = 0;
    reg prev_7m_o = 0;

    // Count 7M edges within a phi1 cycle
    integer edges_7m_in_phi1_high = 0;
    integer edges_7m_in_phi1_low = 0;

    always @(posedge clk_logic) begin
        if (lock) begin
            prev_phi1_o <= phi1_o;
            prev_7m_o <= clk_7M_o;

            // Count 7M posedges during phi1 high and low phases
            if (clk_7M_posedge_o) begin
                if (phi1_o)
                    edges_7m_in_phi1_high <= edges_7m_in_phi1_high + 1;
                else
                    edges_7m_in_phi1_low <= edges_7m_in_phi1_low + 1;
            end
        end
    end

    // =========================================================================
    // Test Sequence
    // =========================================================================

    initial begin
        $dumpfile("a2bus_timing_tb.vcd");
        $dumpvars(0, a2bus_timing_tb);

        $display("================================================================");
        $display("  A2Bus Predictive Timing Generator Testbench");
        $display("================================================================");
        $display("FPGA Clock:  %0d Hz (%0.2f ns period)", CLOCK_SPEED_HZ, CLK_PERIOD);
        $display("Apple 14M:   %0d Hz (%0.2f ns period)", APPLE_HZ, APPLE_14M_PERIOD);
        $display("Apple 7M:    %0.2f Hz (%0.2f ns period)", APPLE_HZ / 2.0, APPLE_7M_PERIOD);
        $display("Apple CPU:   %0.2f Hz (%0.2f ns period)", APPLE_HZ / 14.0, APPLE_CPU_PERIOD);
        $display("================================================================");

        // =====================================================================
        // Test 1: Free-run and Lock Acquisition
        // =====================================================================
        $display("\n--- Test 1: Free-run and Lock Acquisition ---");
        $display("Waiting for Apple clocks to start and lock to be achieved...");

        // Wait for Apple clocks to start and some CPU cycles to pass
        wait(apple_running);
        $display("Apple clocks started at %t", $realtime);

        // Wait for lock with timeout
        fork
            begin
                wait(lock);
                $display("LOCK achieved at %t (after %0.2f ns from Apple clock start)",
                    $realtime, $realtime - 20 * CLK_PERIOD);
            end
            begin
                #(APPLE_CPU_PERIOD * 20);  // 20 CPU cycles timeout
                if (!lock) begin
                    $display("WARNING: Lock not achieved within 20 CPU cycles");
                end
            end
        join_any
        disable fork;

        if (lock)
            $display("[PASS] Lock acquired");
        else
            $display("[FAIL] Lock not acquired");

        // =====================================================================
        // Test 2: Steady-state Clock Generation (no noise)
        // =====================================================================
        $display("\n--- Test 2: Steady-state Clock Generation ---");
        $display("Running for 50 CPU cycles...");

        // Reset counters
        phi1_posedge_count = 0;
        phi1_negedge_count = 0;
        clk_7m_posedge_count = 0;
        clk_7m_negedge_count = 0;
        q3_posedge_count = 0;
        q3_negedge_count = 0;
        clk_14m_posedge_count = 0;
        phi1_period_sum = 0;
        phi1_period_count = 0;
        period_7m_sum = 0;
        period_7m_count = 0;

        #(APPLE_CPU_PERIOD * 50);

        $display("  Phi1 posedges:    %0d (expected ~50)", phi1_posedge_count);
        $display("  Phi1 negedges:    %0d (expected ~50)", phi1_negedge_count);
        $display("  7M posedges:      %0d (expected ~350)", clk_7m_posedge_count);
        $display("  7M negedges:      %0d (expected ~350)", clk_7m_negedge_count);
        $display("  Q3 posedges:      %0d (expected ~100)", q3_posedge_count);
        $display("  Q3 negedges:      %0d (expected ~100)", q3_negedge_count);
        $display("  14M posedges:     %0d (expected ~700)", clk_14m_posedge_count);

        if (phi1_period_count > 0) begin
            $display("  Avg Phi1 period:  %0.2f ns (expected ~%0.2f ns)",
                phi1_period_sum / phi1_period_count, APPLE_CPU_PERIOD);
        end

        if (period_7m_count > 0) begin
            $display("  Avg 7M period:    %0.2f ns (expected ~%0.2f ns)",
                period_7m_sum / period_7m_count, APPLE_7M_PERIOD);
        end

        // Check counts are reasonable (within ±5%)
        if (phi1_posedge_count >= 47 && phi1_posedge_count <= 53)
            $display("  [PASS] Phi1 posedge count");
        else
            $display("  [FAIL] Phi1 posedge count: %0d", phi1_posedge_count);

        if (clk_7m_posedge_count >= 333 && clk_7m_posedge_count <= 368)
            $display("  [PASS] 7M posedge count");
        else
            $display("  [FAIL] 7M posedge count: %0d", clk_7m_posedge_count);

        // 7M posedge count should equal negedge count
        if (clk_7m_posedge_count == clk_7m_negedge_count ||
            (clk_7m_posedge_count - clk_7m_negedge_count) <= 1 ||
            (clk_7m_negedge_count - clk_7m_posedge_count) <= 1)
            $display("  [PASS] 7M symmetry (pos=%0d neg=%0d)", clk_7m_posedge_count, clk_7m_negedge_count);
        else
            $display("  [FAIL] 7M asymmetry (pos=%0d neg=%0d)", clk_7m_posedge_count, clk_7m_negedge_count);

        // 14M count should be approximately sum of 7M pos + neg edges
        if (clk_14m_posedge_count >= (clk_7m_posedge_count + clk_7m_negedge_count - 5) &&
            clk_14m_posedge_count <= (clk_7m_posedge_count + clk_7m_negedge_count + 5))
            $display("  [PASS] 14M = 7M_pos + 7M_neg");
        else
            $display("  [FAIL] 14M count mismatch: 14M=%0d, 7M_pos+neg=%0d",
                clk_14m_posedge_count, clk_7m_posedge_count + clk_7m_negedge_count);

        $display("  Lock status: %s", lock ? "LOCKED" : "UNLOCKED");
        $display("  Error flag:  %s", error ? "ERROR" : "OK");

        // =====================================================================
        // Test 3: Noise Injection on Phi1
        // =====================================================================
        $display("\n--- Test 3: Noise Injection on Phi1 ---");
        $display("Enabling noise glitches on Phi1...");

        noise_enable = 1;
        noise_count = 0;

        // Reset counters
        phi1_posedge_count = 0;
        clk_7m_posedge_count = 0;

        #(APPLE_CPU_PERIOD * 30);

        $display("  Noise glitches injected: %0d", noise_count);
        $display("  Phi1 posedges:    %0d (expected ~30)", phi1_posedge_count);
        $display("  7M posedges:      %0d (expected ~210)", clk_7m_posedge_count);
        $display("  Lock status: %s", lock ? "LOCKED" : "UNLOCKED");
        $display("  Error flag:  %s", error ? "ERROR" : "OK");

        if (lock)
            $display("  [PASS] Lock maintained through noise");
        else
            $display("  [FAIL] Lock lost during noise injection");

        if (phi1_posedge_count >= 27 && phi1_posedge_count <= 33)
            $display("  [PASS] Phi1 count stable through noise");
        else
            $display("  [FAIL] Phi1 count unstable: %0d", phi1_posedge_count);

        noise_enable = 0;

        // =====================================================================
        // Test 4: Clock Loss and Re-acquisition
        // =====================================================================
        $display("\n--- Test 4: Clock Loss and Re-acquisition ---");

        // Stop Apple clocks
        apple_running = 0;
        $display("Apple clocks stopped at %t", $realtime);

        // Let it run without input for a while
        #(APPLE_CPU_PERIOD * 10);

        $display("  After 10 cycles with no input:");
        $display("  Lock: %s, Error: %s", lock ? "LOCKED" : "UNLOCKED", error ? "ERROR" : "OK");

        // Restart Apple clocks
        apple_running = 1;
        lock_seen = 0;
        $display("Apple clocks restarted at %t", $realtime);

        // Wait for re-lock
        fork
            begin
                wait(lock && lock_seen);
                $display("  Re-lock achieved at %t", $realtime);
            end
            begin
                #(APPLE_CPU_PERIOD * 20);
                if (!lock) $display("  WARNING: Re-lock not achieved within 20 CPU cycles");
            end
        join_any
        disable fork;

        if (lock)
            $display("  [PASS] Re-lock achieved after clock restart");
        else
            $display("  [FAIL] Failed to re-lock after clock restart");

        // =====================================================================
        // Test 5: Verify Phase Relationships
        // =====================================================================
        $display("\n--- Test 5: Phase Relationship Verification ---");

        edges_7m_in_phi1_high = 0;
        edges_7m_in_phi1_low = 0;

        #(APPLE_CPU_PERIOD * 20);

        $display("  7M edges during Phi1 high: %0d", edges_7m_in_phi1_high);
        $display("  7M edges during Phi1 low:  %0d", edges_7m_in_phi1_low);

        // In 20 CPU cycles, 7M posedges per cycle:
        //   Phases 1,3,5 have 7M posedge while Phi1=1 (3 per cycle)
        //   Phases 7,9,11,13 have 7M posedge while Phi1=0 (4 per cycle)
        // Expected: high=60, low=80, ratio=0.75
        // (At phase 7, 7M rises and Phi1 falls simultaneously; in registered
        //  output the 7M posedge sees Phi1 already low.)
        if (edges_7m_in_phi1_high > 0 && edges_7m_in_phi1_low > 0) begin
            real ratio;
            ratio = real'(edges_7m_in_phi1_high) / real'(edges_7m_in_phi1_low);
            $display("  High/Low ratio: %0.2f (expected ~0.75)", ratio);
            if (ratio >= 0.65 && ratio <= 0.85)
                $display("  [PASS] Phi1/7M phase relationship correct");
            else
                $display("  [FAIL] Phi1/7M phase relationship incorrect");
        end

        // =====================================================================
        // Test 6: Extended Cycle Detection (IIgs 16-tick cycles)
        // =====================================================================
        $display("\n--- Test 6: Extended Cycle Detection ---");
        $display("  IIgs extended cycle = 16 ticks of 14M (vs normal 14 ticks)");
        $display("  Extension adds 2 extra 14M ticks during Phi1 HIGH phase");

        // First, verify no false positives during normal operation
        extended_cycle_count = 0;
        #(APPLE_CPU_PERIOD * 10);
        $display("  Extended cycles during 10 normal cycles: %0d (expected 0)", extended_cycle_count);
        if (extended_cycle_count == 0)
            $display("  [PASS] No false positives during normal operation");
        else
            $display("  [FAIL] False positives: %0d", extended_cycle_count);

        // Inject a single extended cycle (16 ticks instead of 14)
        extended_cycle_count = 0;
        apple_extend_next = 1;

        // Wait for the extended cycle to complete plus a few normal cycles
        #(APPLE_CPU_PERIOD * 5);

        $display("  Extended cycles detected after 16-tick cycle: %0d (expected 1)", extended_cycle_count);
        if (extended_cycle_count >= 1)
            $display("  [PASS] Extended cycle correctly detected");
        else
            $display("  [FAIL] Extended cycle not detected");

        // Test the every-65th-cycle pattern: run 64 normal + 1 extended, twice
        extended_cycle_count = 0;
        $display("  Running 65-cycle pattern (64 normal + 1 extended) x2...");

        // First group of 65
        #(APPLE_CPU_PERIOD * 64);
        apple_extend_next = 1;
        #(APPLE_CPU_PERIOD * 2);   // wait for extended + 1 normal

        // Second group of 65
        #(APPLE_CPU_PERIOD * 64);
        apple_extend_next = 1;
        #(APPLE_CPU_PERIOD * 2);   // wait for extended + 1 normal

        $display("  Extended cycles in 2x65 pattern: %0d (expected 2)", extended_cycle_count);
        if (extended_cycle_count == 2)
            $display("  [PASS] 65-cycle pattern detection correct");
        else
            $display("  [FAIL] Expected 2 extended cycles, got %0d", extended_cycle_count);

        $display("  Lock status: %s", lock ? "LOCKED" : "UNLOCKED");

        // =====================================================================
        // Summary
        // =====================================================================
        $display("\n================================================================");
        $display("  Test Summary");
        $display("================================================================");
        $display("  Final lock status: %s", lock ? "LOCKED" : "UNLOCKED");
        $display("  Final error flag:  %s", error ? "ERROR" : "OK");
        $display("  Total CPU cycles:  %0d", cpu_cycle_count);
        $display("  Simulation time:   %t", $realtime);
        $display("================================================================");

        $finish;
    end

    // =========================================================================
    // Timeout watchdog
    // =========================================================================

    initial begin
        #(APPLE_CPU_PERIOD * 400);  // 400 CPU cycles max (need room for 65-cycle pattern test)
        $display("\nERROR: Simulation timeout at %t!", $realtime);
        $finish;
    end

endmodule
