// tb_video_restart.v — reproduce the video-restart-after-retrain bug.
//
// Hypothesis under test: idle_pattern_inserter's idle->video switch needs
// THREE-way coincidence: source_ready && in_data[72] (packer switch point)
// && idle_switch_point (the inserter's own free-running 8192-symbol
// cadence). Both cadences have the same period; they phase-align only via
// common reset at boot. After a retrain (channel_ready dropped and
// re-raised), count_to_switch resets but idle_count free-runs and the
// packer keeps streaming -> arbitrary fixed phase offset -> if misaligned,
// the switch NEVER fires again -> idle pattern (VB-ID NoVideo) forever ->
// SINK_STATUS=0 until full reset.
//
// The TB drives main_stream_processing through establish #1 (from reset,
// aligned) and establish #2 (after a simulated retrain, with a
// configurable packer-phase disturbance), and reports whether/when the
// switch fires in each case.

`timescale 1ns/1ps

module tb_video_restart;

    reg clk = 0;
    always #2 clk = ~clk;   // 250 MHz sim clock (arbitrary)

    reg        link_established = 0;
    reg        source_ready     = 0;
    reg        clock_train      = 0;
    reg        align_train      = 0;
    reg [72:0] in_data          = 73'd0;
    wire [79:0] tx_symbols;

    main_stream_processing dut (
        .symbol_clk          (clk),
        .tx_link_established (link_established),
        .source_ready        (source_ready),
        .tx_clock_train      (clock_train),
        .tx_align_train      (align_train),
        .in_data             (in_data),
        .tx_symbols          (tx_symbols)
    );

    // ------------------------------------------------------------------
    // Synthetic packer: emits a switch-point marker (bit 72) once per
    // 4096-cycle "scanline cadence", phase-adjustable via packer_phase.
    // Data payload = counter (content irrelevant to the choreography).
    // ------------------------------------------------------------------
    integer packer_phase = 0;
    reg [12:0] pk_cnt = 0;
    always @(posedge clk) begin
        pk_cnt <= pk_cnt + 1;
        // marker high for 8 cycles at the phase-offset point of each period
        if (((pk_cnt + packer_phase) & 13'h0FFF) < 8)
            in_data <= {1'b1, 72'h0123456789ABCDEF01};
        else
            in_data <= {1'b0, {8{pk_cnt[8:0]}}};
    end

    // observe the switch: count_to_switch[16] inside the idle inserter
    wire switched = dut.i_idle_pattern_inserter.count_to_switch[16];
    wire [16:0] c2s = dut.i_idle_pattern_inserter.count_to_switch;

    task establish(input integer phase_disturb, input [127:0] tag);
        integer i;
        integer fired;
        begin
            fired = -1;
            // trains (brief, then release)
            clock_train = 1; repeat (2000) @(posedge clk);
            clock_train = 0; align_train = 1; repeat (2000) @(posedge clk);
            align_train = 0;
            packer_phase = phase_disturb;
            link_established = 1; source_ready = 1;
            for (i = 0; i < 400000; i = i + 1) begin
                @(posedge clk);
                if (switched && fired < 0) fired = i;
            end
            if (fired >= 0)
                $display("%0s: VIDEO SWITCH FIRED at cycle %0d (c2s=%05x)", tag, fired, c2s);
            else
                $display("%0s: SWITCH NEVER FIRED in 400k cycles (c2s stuck at %05x) *** BUG ***", tag, c2s);
        end
    endtask

    integer ph;
    initial begin
        // boot: common reset alignment
        repeat (100) @(posedge clk);
        establish(0, "BOOT   (phase 0)");

        // retrain with no phase disturbance (control)
        link_established = 0;
        repeat (5000) @(posedge clk);
        establish(0, "RETRAIN(phase 0)");

        // retrains with representative phase disturbances
        for (ph = 512; ph <= 3584; ph = ph + 1024) begin
            link_established = 0;
            repeat (5000) @(posedge clk);
            establish(ph, ph == 512  ? "RETRAIN(phase  512)" :
                          ph == 1536 ? "RETRAIN(phase 1536)" :
                          ph == 2560 ? "RETRAIN(phase 2560)" :
                                       "RETRAIN(phase 3584)");
        end
        $finish;
    end
endmodule
