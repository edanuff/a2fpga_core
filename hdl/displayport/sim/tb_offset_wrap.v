// tb_offset_wrap.v — reproduce the "rotated colorbars" offset-with-clean-
// wraparound bug (a2mega test_log rows 49/53/56/60).
//
// MECHANISM UNDER TEST (from RTL reading, this tb proves/measures it):
//
//   The board top's pattern is a pure function of the (cx,cy) the
//   transmitter requests, so the image cannot rotate relative to the
//   pixel-domain counters. Inside the transmitter, video_stream_packer
//   aligns its framing counters (line_num/line_cycle, which place BE/BS)
//   to the pixel stream EXACTLY ONCE: at video start it discards FIFO
//   words until a start-of-frame (SOF) word is at the head, prefetches
//   PREFILL words, and sets `running`. From then on it consumes exactly
//   WORDS_PER_LINE words per active line and NEVER looks at fifo_rsof
//   again. Any word slip between the two domains after that moment is
//   therefore a PERMANENT rotation of the image within the active
//   raster (clean wrap in x and/or y), constant until full reset:
//
//     * pixel_cdc_fifo OVERFLOW: writes while full are dropped
//       (pixel_cdc_fifo.v write branch `wen && !wfull`). Happens when
//       the symbol clock stalls (PHY powered down mid-battle by the
//       ladder or the watchdog: tx_symbol_clk is the SERDES word clock)
//       while the fabric pixel PLL's VCO keeps free-running. Each
//       dropped word advances subsequent content: image shifts by
//       +2 pixels per dropped word.
//
//     * packer UNDERRUN: a fetch with !fifo_rvalid does not pop (the
//       FIFO ignores rd_en when empty) but the packer's walk advances
//       anyway. Happens when the pixel clock halts/limps (pixel PLL
//       relocking after the symbol clock returns) while the symbol
//       domain runs. Each missed fetch delays subsequent content:
//       image shifts by -2 pixels per miss.
//
//   Marginal acquisitions = retrain battles = PHY power cycles =
//   clock-stall episodes = FIFO saturation = slip. Clean first-try
//   acquisitions have none, hence always correct. The slip is
//   mod-(1920x1080) arbitrary -> horizontal-only, vertical-only, or
//   both-axes offsets, always cleanly wrapped: exactly the hardware
//   signature (n=4, two dies, two monitors, two converter sinks).
//
// WHAT THE TB DOES:
//   Real production chain at the real 1080p 2-lane HBR geometry:
//     rgb=f(cx,cy) [board-top contract] -> dp_video_timing ->
//     pixel_cdc_fifo -> video_stream_packer -> msa_inserter_2ch ->
//     main_stream_processing
//   with clk_pixel : clk_symbol at the exact 11:10 PLL ratio (periods
//   200:220 sim units). The rgb answer encodes (cx,cy) into the pixel
//   value, so decoding any pixel from the symbol stream reveals which
//   source coordinate landed in which framing slot.
//
//   A stream decoder (lane 0 of the packer output, post-framing
//   pre-scrambler) finds each BE, decodes the first pixel of the line,
//   and computes the linear offset delta = decoded_raster_position -
//   framing_position, aggregated per frame (UNIFORM/MIXED). dx =
//   delta % H_VISIBLE, dy = delta / H_VISIBLE: the on-screen offset.
//
//   The sequencer then replays "video restart" events at varied frame
//   phases: link drop + retrain with (a) no clock disturbance,
//   (b) both clocks stalled identically, (c) symbol-clock stalls of
//   varied length/position (PHY-down model), (d) pixel-clock stalls
//   (PLL-relock model). After each it re-establishes the link, waits
//   for the idle->video switch (the property tb_video_restart{,_v2}
//   guard) and reports the settled offset plus the FIFO drop/miss
//   counters that explain it.
//
// PASS  = offset (0,0) in every scenario.
// BUG REPRODUCED = controls stay (0,0) while stall scenarios settle at
//   stable nonzero wrapped offsets that match 2*(drops-misses) mod frame.
//
// Run:
//   iverilog -g2012 -o /tmp/tb_offset_wrap.vvp \
//     hdl/displayport/sim/tb_offset_wrap.v \
//     hdl/displayport/video/dp_video_timing.v \
//     hdl/displayport/video/pixel_cdc_fifo.v \
//     hdl/displayport/video/video_stream_packer.v \
//     hdl/displayport/video/msa_inserter_2ch.v \
//     hdl/displayport/core/main_stream_processing.v \
//     hdl/displayport/core/idle_pattern_inserter.v \
//     hdl/displayport/core/scrambler_reset_inserter.v \
//     hdl/displayport/core/scrambler_all_channels.v \
//     hdl/displayport/core/insert_training_pattern.v \
//     hdl/displayport/core/skew_channels.v
//   vvp /tmp/tb_offset_wrap.vvp
//
// (a full run is ~35 M symbol cycles; allow several minutes)

`timescale 1ps/1ps

module tb_offset_wrap;

    // ---------------- 1080p 2-lane HBR geometry (production config) ----
    localparam H_VISIBLE = 1920, H_TOTAL = 2200;
    localparam V_VISIBLE = 1080, V_TOTAL = 1125;
    localparam TU_SIZE = 64;
    localparam SYMS_PER_LINE = 4000;          // 2200*2*10/11
    localparam VALID_NUM = 2112, VALID_DEN = 40;
    localparam WORDS_PER_LINE = 960;          // H_VISIBLE/2
    localparam FIFO_ADDR_BITS = 12;           // clog2(3*960)=12 -> depth 4096
    localparam FRAME_PIX = H_VISIBLE*V_VISIBLE;

    // ---------------- clocks: exact 11:10 pixel:symbol ratio -----------
    // symbol 220 units/period, pixel 200 units/period. Gate-able: a
    // stalled clock parks low (models PHY power-down / PLL loss).
    reg sym_en = 1'b1, pix_en = 1'b1;
    reg clk_sym = 1'b0, clk_pix = 1'b0;
    always #110 clk_sym = sym_en ? ~clk_sym : 1'b0;
    always #100 clk_pix = pix_en ? ~clk_pix : 1'b0;

    reg reset = 1'b1;

    // ---------------- pixel front end (board-top pull contract) --------
    // rgb answers the PREVIOUS cycle's (cx,cy), exactly like
    // a2mega_dp_test_top.sv, but encodes the coordinates so the decoder
    // can identify any pixel: rgb = {cx[11:0], cy[10:0], 1'b0}
    wire [11:0] cx;
    wire [10:0] cy;
    reg  [23:0] rgb;
    always @(posedge clk_pix)
        rgb <= {cx, cy, 1'b0};

    // capture_arm CDC, as in dp_transmitter.sv
    wire capture_arm;
    reg  capture_arm_m = 0, capture_arm_px = 0;
    always @(posedge clk_pix) begin
        capture_arm_m  <= capture_arm;
        capture_arm_px <= capture_arm_m;
    end

    wire [47:0] fifo_wdata;
    wire        fifo_wsof, fifo_wen, fifo_wfull;
    dp_video_timing #(
        .LANE_COUNT(2), .H_VISIBLE(H_VISIBLE), .H_TOTAL(H_TOTAL),
        .V_VISIBLE(V_VISIBLE), .V_TOTAL(V_TOTAL),
        .BIT_WIDTH(12), .BIT_HEIGHT(11)
    ) i_timing (
        .clk_pixel(clk_pix), .reset(reset), .capture_arm(capture_arm_px),
        .rgb(rgb), .cx(cx), .cy(cy),
        .fifo_wdata(fifo_wdata), .fifo_wsof(fifo_wsof), .fifo_wen(fifo_wen)
    );

    wire [47:0] fifo_rpix;
    wire        fifo_rsof, fifo_rvalid, fifo_rd;
    wire [FIFO_ADDR_BITS:0] fifo_rlevel;
    pixel_cdc_fifo #(.WIDTH(49), .ADDR_BITS(FIFO_ADDR_BITS)) i_fifo (
        .wclk(clk_pix), .wreset(reset),
        .wdata({fifo_wsof, fifo_wdata}), .wen(fifo_wen), .wfull(fifo_wfull),
        .rclk(clk_sym), .rreset(reset),
        .rdata({fifo_rsof, fifo_rpix}), .rvalid(fifo_rvalid),
        .rd_en(fifo_rd), .rlevel(fifo_rlevel)
    );

    wire        pk_ready, sdp_gap, frame_pulse, underrun;
    wire [72:0] packed_data, msa_merged_data;
    video_stream_packer #(
        .LANE_COUNT(2), .H_VISIBLE(H_VISIBLE), .V_VISIBLE(V_VISIBLE),
        .V_TOTAL(V_TOTAL), .TU_SIZE(TU_SIZE), .SYMS_PER_LINE(SYMS_PER_LINE),
        .VALID_NUM(VALID_NUM), .VALID_DEN(VALID_DEN), .PREFILL(WORDS_PER_LINE)
    ) i_packer (
        .clk(clk_sym), .reset(reset),
        .mvid_byte(8'hA6), .maud_byte(8'h00), .audio_mute(1'b1),
        .fifo_rdata(fifo_rpix), .fifo_rsof(fifo_rsof), .fifo_rvalid(fifo_rvalid),
        .fifo_rd(fifo_rd), .fifo_rlevel({3'b000, fifo_rlevel}),
        .capture_arm(capture_arm), .ready(pk_ready), .data(packed_data),
        .sdp_gap(sdp_gap), .frame_pulse(frame_pulse), .underrun(underrun)
    );

    msa_inserter_2ch i_msa (
        .clk(clk_sym), .active(1'b1),
        .M_value(24'h0466A6), .N_value(24'h080000),
        .H_visible(12'(H_VISIBLE)), .V_visible(12'(V_VISIBLE)),
        .H_total(12'(H_TOTAL)), .V_total(12'(V_TOTAL)),
        .H_sync_width(12'd44), .V_sync_width(12'd5),
        .H_start(12'd192), .V_start(12'd41),
        .H_vsync_active_high(1'b1), .V_vsync_active_high(1'b1),
        .flag_sync_clock(1'b1), .flag_YCCnRGB(1'b0), .flag_422n444(1'b0),
        .flag_range_reduced(1'b0), .flag_interlaced_even(1'b0),
        .flag_YCC_colour_709(1'b0), .flags_3d_Indicators(2'b00),
        .bits_per_colour(5'b01000),
        .in_data(packed_data), .out_data(msa_merged_data)
    );

    reg link_established = 0, clock_train = 0, align_train = 0;
    wire [79:0] tx_symbols;
    main_stream_processing i_msp (
        .symbol_clk(clk_sym),
        .tx_link_established(link_established),
        .source_ready(pk_ready),
        .tx_clock_train(clock_train), .tx_align_train(align_train),
        .in_data(msa_merged_data), .tx_symbols(tx_symbols)
    );
    wire switched = i_msp.i_idle_pattern_inserter.count_to_switch[16];

    // ---------------- slip evidence counters ---------------------------
    integer drop_cnt = 0;    // FIFO writes discarded while full (overflow)
    integer miss_cnt = 0;    // packer fetches that found the FIFO empty
    always @(posedge clk_pix)
        if (!reset && fifo_wen && fifo_wfull) drop_cnt = drop_cnt + 1;
    always @(posedge clk_sym)
        if (!reset && i_packer.running && i_packer.fetch_r && !fifo_rvalid)
            miss_cnt = miss_cnt + 1;

    // ---------------- symbol-stream decoder (lane 0) -------------------
    // Sequentially examines lane 0's two symbols per cycle (slot0 =
    // packed_data[8:0] first, then slot1 = packed_data[17:9]).
    //   BS -> next data symbol is VB-ID (bit0 = vblank flag)
    //   BE -> line boundary; the next three data symbols are pixel 0 of
    //         the line on lane 0 = source raster position (exp_cy*1920+0)
    // The decoded {cx,cy} embedded in the pixel gives the actual source
    // position; delta = (decoded - expected) mod FRAME_PIX must be 0.
    localparam [8:0] SYM_BS = 9'b110111100;
    localparam [8:0] SYM_BE = 9'b111111011;

    reg        after_bs   = 0;
    reg        vblank_seen= 0;
    reg        collecting = 0;
    integer    col_cnt    = 0;
    reg [23:0] pixw       = 0;
    integer    exp_cy     = -1;   // -1: no frame reference yet

    // per-frame aggregation
    integer f_delta = -1;         // first measured line's delta
    reg     f_mixed = 0;
    integer f_lines = 0;
    // latest completed-frame result (read by the sequencer)
    integer frame_count = 0;
    integer cur_dx = 0, cur_dy = 0;
    reg     cur_uniform = 0;
    integer lin_dec, lin_exp, delta;

    // NOTE: called twice per clock (slot0 then slot1) — all state updates
    // must be BLOCKING so slot1 sees slot0's effects within the cycle.
    task decode_sym(input [8:0] s);
        begin
            if (s[8]) begin
                // K symbol
                if (s == SYM_BS) begin
                    after_bs   = 1;
                    collecting = 0;
                end else if (s == SYM_BE) begin
                    if (vblank_seen) begin
                        // frame boundary: summarize the frame just ended
                        if (f_lines > 0) begin
                            cur_dx      = (f_delta >= 0) ? (f_delta % H_VISIBLE) : -1;
                            cur_dy      = (f_delta >= 0) ? (f_delta / H_VISIBLE) : -1;
                            cur_uniform = !f_mixed;
                            frame_count = frame_count + 1;
                        end
                        f_delta = -1; f_mixed = 0; f_lines = 0;
                        exp_cy  = 0;
                        vblank_seen = 0;
                    end else if (exp_cy >= 0)
                        exp_cy = exp_cy + 1;
                    if (exp_cy >= 0) begin
                        collecting = 1;
                        col_cnt    = 0;
                    end
                    after_bs = 0;
                end else begin
                    after_bs   = 0;
                    collecting = 0;  // FS/FE or other K: never before pixel 0
                end
            end else begin
                // data symbol
                if (after_bs) begin
                    if (s[0]) vblank_seen = 1;     // VB-ID vblank flag
                    after_bs = 0;
                end else if (collecting) begin
                    pixw = {pixw[15:0], s[7:0]};
                    col_cnt = col_cnt + 1;
                    if (col_cnt == 3) begin
                        collecting = 0;
                        // pixw = {cx[11:0], cy[10:0], 1'b0}
                        lin_dec = pixw[11:1]*H_VISIBLE + pixw[23:12];
                        lin_exp = exp_cy*H_VISIBLE;
                        delta   = (lin_dec - lin_exp) % FRAME_PIX;
                        if (delta < 0) delta = delta + FRAME_PIX;
                        if (f_lines == 0)           f_delta = delta;
                        else if (delta != f_delta)  f_mixed = 1;
                        f_lines = f_lines + 1;
                    end
                end
            end
        end
    endtask

    always @(posedge clk_sym) begin
        if (!reset && i_packer.running) begin
            decode_sym(packed_data[8:0]);
            decode_sym(packed_data[17:9]);
        end
    end

    // ---------------- scenario machinery -------------------------------
    integer prev_dx = 0, prev_dy = 0;
    integer scen_fail = 0;      // scenarios where offset changed
    integer scen_n = 0;

    // wait until the packer is at a given line (mid-line), i.e. control
    // the restart phase within the frame
    task automatic wait_line(input integer l);
        begin
            wait (i_packer.line_num == l[10:0] && i_packer.line_cycle == 11'd1000);
        end
    endtask

    // simulated retrain battle: link drops; optional clock stalls model
    // the PHY power-down (symbol clock stops, pixel PLL VCO free-runs)
    // and the pixel-PLL relock (symbol clock back, pixel clock late).
    // Durations are in pixel-clock periods (200 units each).
    task automatic scenario(
        input [479:0] name,
        input integer at_line,
        input integer sym_stall,     // pixel-periods the SYMBOL clock stops
        input integer pix_stall,     // pixel-periods the PIXEL clock stops
        input integer both_stall     // pixel-periods BOTH stop together
    );
        integer fc0, d0, m0, t0;
        begin
            scen_n = scen_n + 1;
            wait_line(at_line);
            d0 = drop_cnt; m0 = miss_cnt;
            // battle begins: sink loses the link, ladder retrains
            link_established = 0;
            if (both_stall > 0) begin
                sym_en = 0; pix_en = 0;
                #(both_stall*200);
                sym_en = 1; pix_en = 1;
            end
            if (sym_stall > 0) begin
                sym_en = 0;              // PHY down: word clock gone
                #(sym_stall*200);        // pixel VCO keeps running
                sym_en = 1;
            end
            if (pix_stall > 0) begin
                pix_en = 0;              // pixel PLL relocking
                #(pix_stall*200);        // symbol domain already back
                pix_en = 1;
            end
            // brief training phases, then the link re-establishes
            clock_train = 1; #(2000*220);
            clock_train = 0; align_train = 1; #(2000*220);
            align_train = 0;
            link_established = 1;
            // settle: transition frame(s) flush through, then measure
            fc0 = frame_count;
            wait (frame_count >= fc0 + 3);
            // idle->video switch must have re-fired (tb_video_restart
            // property) by now; give it one more frame if not
            t0 = frame_count;
            if (!switched) wait (frame_count >= t0 + 1 || switched);
            $display("SCEN %0d %0s @line %0d: dx=%0d dy=%0d %0s  (d_drop=%0d d_miss=%0d, switch=%0d, shift=%0d,%0d)",
                     scen_n, name, at_line, cur_dx, cur_dy,
                     cur_uniform ? "UNIFORM" : "MIXED",
                     drop_cnt-d0, miss_cnt-m0, switched,
                     cur_dx-prev_dx, cur_dy-prev_dy);
            if (!cur_uniform || cur_dx != prev_dx || cur_dy != prev_dy)
                scen_fail = scen_fail + 1;
            prev_dx = cur_dx; prev_dy = cur_dy;
        end
    endtask

    integer fcb;
    initial begin
        // boot
        #2000 reset = 0;
        // first video start: clean acquisition. Expect (0,0).
        link_established = 1;
        fcb = frame_count;
        wait (frame_count >= fcb + 2);
        $display("BOOT: dx=%0d dy=%0d %0s (drops=%0d misses=%0d switch=%0d)",
                 cur_dx, cur_dy, cur_uniform ? "UNIFORM" : "MIXED",
                 drop_cnt, miss_cnt, switched);
        if (cur_dx != 0 || cur_dy != 0 || !cur_uniform)
            $display("*** BOOT MISALIGNED — clean-start alignment broken ***");
        prev_dx = cur_dx; prev_dy = cur_dy;

        //            name                    line  sym    pix    both
        scenario("CONTROL retrain-only      ",  100,     0,     0,     0);
        scenario("CONTROL both-clocks-stall ",  300, 0,     0,     30000);
        scenario("SYM-stall  5k (sub-satur.)",  200,  5000,     0,     0);
        scenario("SYM-stall 20k             ",  100, 20000,     0,     0);
        scenario("SYM-stall 20k             ",  700, 20000,     0,     0);
        scenario("SYM-stall 20k (vblank)    ", 1100, 20000,     0,     0);
        scenario("SYM-stall 50k             ",  300, 50000,     0,     0);
        scenario("PIX-stall 20k             ",  100,     0, 20000,     0);
        scenario("PIX-stall 35k             ",  800,     0, 35000,     0);
        scenario("PIX-stall  2k (short)     ",  400,     0,  2000,     0);

        $display("");
        if (scen_fail == 0)
            $display("RESULT: all scenarios kept the boot alignment — bug NOT reproduced");
        else
            $display("RESULT: %0d scenario(s) shifted the image (stable wrapped offset) — BUG REPRODUCED",
                     scen_fail);
        $display("(underrun sticky=%0d, total drops=%0d, total misses=%0d)",
                 underrun, drop_cnt, miss_cnt);
        $finish;
    end

    // global watchdog so a wedged run terminates with evidence
    // (each frame is ~0.495e9 ps; full run ~25e9 ps; cap at 60e9)
    initial begin
        repeat (120) #500_000_000;
        $display("TIMEOUT: tb_offset_wrap did not complete (frame_count=%0d)", frame_count);
        $finish;
    end

endmodule
