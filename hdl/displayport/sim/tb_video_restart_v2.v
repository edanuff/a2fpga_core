// tb_video_restart_v2.v — full-chain repro of the video-restart-after-
// retrain bug, on REAL cadences: FIFO model -> video_stream_packer ->
// msa_inserter_2ch -> main_stream_processing.
//
// Per establish phase, reports:
//   - whether/when the idle->video switch fired (count_to_switch[16])
//   - SS (K28.2) symbol count post-switch  = MSA emission evidence
//   - idle VB-ID (NoVideo) count post-switch = stream-validity evidence
//   - packer ready state
// Boot phase vs post-retrain phase metrics should match; any divergence
// is the bug.

`timescale 1ns/1ps

module tb_video_restart_v2;

    // 720p default geometry of the vendored core (smaller sim than 1080p)
    localparam H_VISIBLE = 1280, V_VISIBLE = 720, V_TOTAL = 750;
    localparam TU_SIZE = 64, SYMS_PER_LINE = 3600;
    localparam VALID_NUM = 2112, VALID_DEN = 48;
    localparam WORDS_PER_LINE = 640;   // H_VISIBLE / LANE_COUNT(2)

    reg clk = 0;
    always #2 clk = ~clk;

    // ---------------- FIFO model (always ready, framed) ----------------
    wire        fifo_rd;
    reg  [47:0] fifo_pix = 48'h123456789ABC;
    integer     word_idx = 0;
    wire        fifo_rsof = (word_idx == 0);
    always @(posedge clk)
        if (fifo_rd)
            word_idx <= (word_idx == WORDS_PER_LINE*V_VISIBLE-1) ? 0 : word_idx + 1;

    // ---------------- DUT chain ----------------
    reg reset = 1;
    wire        pk_ready, sdp_gap, frame_pulse, underrun, capture_arm;
    wire [72:0] packed_data, msa_merged_data;

    video_stream_packer #(
        .LANE_COUNT(2), .H_VISIBLE(H_VISIBLE), .V_VISIBLE(V_VISIBLE),
        .V_TOTAL(V_TOTAL), .TU_SIZE(TU_SIZE), .SYMS_PER_LINE(SYMS_PER_LINE),
        .VALID_NUM(VALID_NUM), .VALID_DEN(VALID_DEN), .PREFILL(WORDS_PER_LINE)
    ) i_packer (
        .clk(clk), .reset(reset),
        .mvid_byte(8'h34), .maud_byte(8'h00), .audio_mute(1'b1),
        .fifo_rdata(fifo_pix), .fifo_rsof(fifo_rsof), .fifo_rvalid(1'b1),
        .fifo_rd(fifo_rd), .fifo_rlevel(16'hFFFF),
        .capture_arm(capture_arm), .ready(pk_ready), .data(packed_data),
        .sdp_gap(sdp_gap), .frame_pulse(frame_pulse), .underrun(underrun)
    );

    msa_inserter_2ch i_msa (
        .clk(clk), .active(1'b1),
        .M_value(24'h012345), .N_value(24'h080000),
        .H_visible(12'(H_VISIBLE)), .V_visible(12'(V_VISIBLE)),
        .H_total(12'd1650), .V_total(12'(V_TOTAL)),
        .H_sync_width(12'd40), .V_sync_width(12'd5),
        .H_start(12'd260), .V_start(12'd25),
        .H_vsync_active_high(1'b0), .V_vsync_active_high(1'b0),
        .flag_sync_clock(1'b1), .flag_YCCnRGB(1'b0), .flag_422n444(1'b0),
        .flag_range_reduced(1'b0), .flag_interlaced_even(1'b0),
        .flag_YCC_colour_709(1'b0), .flags_3d_Indicators(2'b00),
        .bits_per_colour(5'b01000),
        .in_data(packed_data), .out_data(msa_merged_data)
    );

    reg link_established = 0, clock_train = 0, align_train = 0;
    wire [79:0] tx_symbols;

    main_stream_processing i_msp (
        .symbol_clk(clk),
        .tx_link_established(link_established),
        .source_ready(pk_ready),
        .tx_clock_train(clock_train), .tx_align_train(align_train),
        .in_data(msa_merged_data), .tx_symbols(tx_symbols)
    );

    // ---------------- probes ----------------
    wire switched = i_msp.i_idle_pattern_inserter.count_to_switch[16];
    wire [71:0] post_idle = i_msp.i_idle_pattern_inserter.out_data;
    localparam [8:0] SS = 9'b101011100;      // K28.2 (MSA framing)
    localparam [8:0] IDLE_VBID = 9'b000001001; // VB-ID w/ NoVideo asserted

    integer ss_count, novid_count;
    always @(posedge clk) begin
        if (switched) begin
            if (post_idle[8:0]==SS  || post_idle[17:9]==SS ||
                post_idle[26:18]==SS|| post_idle[35:27]==SS)
                ss_count = ss_count + 1;
            if (post_idle[8:0]==IDLE_VBID || post_idle[17:9]==IDLE_VBID)
                novid_count = novid_count + 1;
        end
    end

    integer dumpfd;
    task establish(input [159:0] tag, input [255:0] dumpfile);
        integer i, fired, dumped;
        reg armed;
        begin
            fired = -1; ss_count = 0; novid_count = 0;
            dumped = 0; armed = 0;
            clock_train = 1; repeat (2000) @(posedge clk);
            clock_train = 0; align_train = 1; repeat (2000) @(posedge clk);
            align_train = 0;
            link_established = 1;
            dumpfd = $fopen(dumpfile, "w");
            for (i = 0; i < 4000000; i = i + 1) begin
                @(posedge clk);
                if (switched && fired < 0) fired = i;
                // frame-aligned exact dump: start at first frame_pulse
                // after the switch, capture 8192 cycles of tx_symbols
                if (switched && frame_pulse) armed = 1;
                if (armed && dumped < 8192) begin
                    $fdisplay(dumpfd, "%018x", i_msp.sr_inserted_data);
                    dumped = dumped + 1;
                end
            end
            $fclose(dumpfd);
            $display("%0s: switch=%0d  ready=%b  SS_syms=%0d  idleVBID_post=%0d  dumped=%0d",
                     tag, fired, pk_ready, ss_count, novid_count, dumped);
        end
    endtask

    initial begin
        repeat (50) @(posedge clk);
        reset = 0;
        repeat (5000) @(posedge clk);   // packer starts, prefill satisfied
        establish("BOOT     ", "dump_boot.txt");
        // retrain: drop link (ladder-style), packer keeps free-running
        link_established = 0;
        repeat (200000) @(posedge clk);  // ~ladder retrain duration
        establish("RETRAIN 1", "dump_rt1.txt");
        link_established = 0;
        repeat (137731) @(posedge clk);  // odd interval: different phase
        establish("RETRAIN 2", "dump_rt2.txt");
        $finish;
    end
endmodule
