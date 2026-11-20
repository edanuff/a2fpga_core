// Differential testbench: baseline doc5503 (fast BSRAM-class memory) vs
// doc5503_pipelined rev 3 (16-byte line cache + lookahead + FB gating),
// against a contention model derived cycle-by-cycle from the real
// ddr3_port_cdc / ddr3_ports / framebuffer_480p RTL (see the model comment
// block below and design doc §12 for the line-cited accounting).
//
// Both DUTs share clk / clk_en / reset and the host register bus, and both
// memory models read the SAME wavetable byte array, so any divergence in the
// logged architectural event streams is caused by the pipelined fetch, not
// by stimulus skew.
//
// The TB counts FB line-deadline misses; outside the forced-overload stress
// phase they must be zero with all 32 oscillators running. Compiling with
// -DREV2_MODE and running with +nogate reproduces the rev-2 field failure
// (lookahead off, gating off, synthetic word-granularity DOC-class load).
//
// Logged event streams (see compare.py for the grammar):
//   W slot osc data vol addr   — waveform-data-sample register file write
//   C slot osc val             — control register file write
//   V slot osc val             — volume register file write
//   M slot mono left right     — final mixer outputs, latched once per scan
//   P slot n                   — phase marker
//
// events.log additionally records host register writes (R), wavetable
// memory writes (G, with old/new bytes), phase markers, per-phase DUT
// counters (COUNTP), per-phase fetch-traffic/FB-miss numbers (TRAFFIC/FBM).
//
// Run: ./run.sh   (iverilog -g2012); +seed=N varies the latency stream.

`timescale 1ns/1ps

module tb_doc5503_diff;

    // ---------------------------------------------------------------------
    // Clocks: 54 MHz clk, 7.159 MHz clk_en (DDS, same scheme as hardware)
    // ---------------------------------------------------------------------
    reg clk = 0;
    always #9.259 clk = ~clk;

    reg reset_n = 0;

    reg clk_en = 0;
    integer dds_acc = 0;
    always @(posedge clk) begin
        if (dds_acc + 7159090 >= 54000000) begin
            dds_acc <= dds_acc + 7159090 - 54000000;
            clk_en  <= 1;
        end else begin
            dds_acc <= dds_acc + 7159090;
            clk_en  <= 0;
        end
    end

    // ---------------------------------------------------------------------
    // Shared host register bus
    // ---------------------------------------------------------------------
    reg        cs_n = 1;
    reg        we_n = 1;
    reg [7:0]  addr_i = 0;
    reg [7:0]  data_i = 0;

    // ---------------------------------------------------------------------
    // Shared 64KB wavetable memory (contents only; each DUT has its own
    // access-latency model)
    // ---------------------------------------------------------------------
    reg [7:0] mem [0:65535];

    // ---------------------------------------------------------------------
    // Baseline DUT + fast (2 clk) memory model
    // ---------------------------------------------------------------------
    wire [15:0] b_addr;
    wire        b_rd;
    reg         b_ready = 0;
    reg  [7:0]  b_q = 0;
    reg         b_pend = 0;
    reg  [15:0] b_pend_addr = 0;

    always @(posedge clk) begin
        b_ready <= 0;
        if (b_pend) begin
            b_q     <= mem[b_pend_addr];
            b_ready <= 1;
            b_pend  <= 0;
        end
        if (b_rd) begin
            b_pend      <= 1;
            b_pend_addr <= b_addr;
        end
    end

    wire signed [15:0] b_mono, b_left, b_right;
    wire signed [15:0] b_chan[16];
    wire [7:0] b_dbg_en;
    wire [1:0] b_dbg_mode[8];
    wire [7:0] b_dbg_halt;
    wire [7:0] b_data_o;

    doc5503 dut_base (
        .clk_i(clk),
        .reset_n_i(reset_n),
        .clk_en_i(clk_en),
        .cs_n_i(cs_n),
        .we_n_i(we_n),
        .addr_i(addr_i),
        .data_i(data_i),
        .data_o(b_data_o),
        .wave_address_o(b_addr),
        .wave_rd_o(b_rd),
        .wave_data_ready_i(b_ready),
        .wave_data_i(b_q),
        .mono_mix_o(b_mono),
        .left_mix_o(b_left),
        .right_mix_o(b_right),
        .channel_o(b_chan),
        .ready_o(),
        .debug_osc_en_o(b_dbg_en),
        .debug_osc_mode_o(b_dbg_mode),
        .debug_osc_halt_o(b_dbg_halt)
    );

    // ---------------------------------------------------------------------
    // Pipelined DUT + corrected arbiter/CDC contention model (rev 3)
    //
    // Derived cycle-by-cycle from hdl/ddr3/ddr3_port_cdc.sv and
    // hdl/ddr3/ddr3_ports.sv (see design doc §12 for line-cited
    // accounting), calibrated with controller read latency K ~= 20 ddr
    // cycles so an uncontended DOC read lands at the known ~550 ns:
    //   * DOC port: 2-entry CDC request FIFO (available deasserts at
    //     occupancy 2), requests serviced strictly single-file; a request
    //     becomes arbiter-visible ~6 clk after the rd pulse; after
    //     req_done the next queued request is visible ~4 clk later and
    //     the freed slot reaches 'available' after a 2FF sync.
    //   * DOC grant (4-beat burst read, 16 bytes): 29 ddr ~= 20 clk_logic
    //     non-preemptible; the four response beats reach the client
    //     starting ~3 clk after grant completion, one per clk.
    //   * FB client (priority ABOVE DOC): a 640-wide line = 320 words =
    //     40 burst8 grants of 52 ddr ~= 35 clk each, issued back-to-back
    //     (2-deep CDC keeps it pending ~3 clk after each completion).
    //     Line fetch alone ~= 1440 clk of the 1712-clk line window. FB
    //     prefetches up to 2 lines ahead and runs CONTINUOUSLY when
    //     behind (catch-up). Lines 240..261 of each 262-line frame are
    //     vblank (no fetch).
    //   * Background client (shadow reads/writes + FB writes, priority
    //     ABOVE DOC): 12-clk grants at ~6% duty.
    //   * 3% of FB/DOC grants take +14 clk (tRFC refresh collision).
    // Aggregate high-priority utilization during active display is ~90%,
    // so rev-2-rate DOC traffic (~9%) sits exactly on the knife edge —
    // matching the field failure. `+nogate` (with REV2_MODE compiled in)
    // reproduces it; the rev-3 DUT must pass.
    // ---------------------------------------------------------------------
    integer outlier_pct = 0;     // % of DOC grants stretched (stress phase)
    integer outlier_len = 330;   // ~6.1 us
    integer rseed = 1;
    reg gate_en = 0;   // main-tree config: fb_fetch_active_i tied 0
    initial begin
        if ($value$plusargs("seed=%d", rseed))
            $display("Using random seed %0d", rseed);
        if ($test$plusargs("gate")) begin
            gate_en = 1;
            $display("FB-activity gating ENABLED (+gate)");
        end
    end

`ifdef REV2_MODE
    localparam bit PIPE_LOOKAHEAD = 1'b0;
`else
    localparam bit PIPE_LOOKAHEAD = 1'b1;
`endif
`ifdef FFBANKS_MODE
    localparam bit PIPE_BANKS_BSRAM = 1'b0;   // rev-3.2-style FF banks
`else
    localparam bit PIPE_BANKS_BSRAM = 1'b1;
`endif
`ifdef NODRAIN_MODE
    localparam int PIPE_DRAIN_CLKS = 0;       // framing-slip repro: no drain
`else
    localparam int PIPE_DRAIN_CLKS = 1024;
`endif

    localparam DOC_GRANT   = 20;   // 29 ddr: LOAD+CMD+WAIT(20)+4xRESP+DONE (4-beat burst)
    localparam FB_GRANT    = 35;   // 52 ddr: burst8 (2x CMD+WAIT + 8 RESP)
    localparam BG_GRANT    = 12;
    localparam REFRESH_EXTRA = 14; // ~260 ns tRFC collision
    localparam LINE_CLKS   = 1712; // 31.7 us
    localparam LINE_WORDS  = 320;  // 640-wide, 2 px/word
    localparam FRAME_LINES = 262;
    localparam ACTIVE_LINES = 240;

    wire [15:0] p_addr;
    wire        p_rd;
    reg         p_ready = 0;
    reg  [31:0] p_q_word = 0;
    reg         p_avail = 0;

    integer cyc = 0;

    // DOC CDC model: 4-deep ring, availability enforces <=2 outstanding
    reg [15:0] dq_addr [0:3];
    integer    dq_vis  [0:3];
    reg [1:0]  dq_hd = 0, dq_tl = 0;
    integer    dq_cnt = 0;        // accepted, not yet req_done
    integer    resp_at = -1;
    reg [127:0] resp_line = 0;
    integer    resp_beats = 0;    // beats remaining to deliver

    // Serialized server: 0 idle, 1 FB, 2 DOC, 3 BG, 4 synthetic rev-2 load
    reg [2:0]  srv_kind = 0;
    integer    srv_end = 0;
    reg [15:0] srv_addr = 0;

`ifdef REV2_MODE
    // Rev-2 traffic emulator: the rev-2 DUT issued single-WORD fetches at
    // ~0.31/slot (vs the line cache's ~0.078). Model the extra ~0.23/slot
    // of word-granularity grants as a synthetic client at DOC-class
    // priority (above the real DOC in the pick order, standing in for the
    // DOC's own extra requests that would be ahead in line).
    integer synth_next_at = 0;
    localparam SYNTH_GRANT = 17;   // single-word grant (26 ddr)
`endif

    // FB line-fetch engine
    integer fb_words_left = 0;
    integer fb_next_ok_at = 0;
    integer disp_abs = 0;         // absolute display line counter
    integer fetched_abs = 1;      // vblank-start equivalent: 2-line cushion prefetched
    integer line_timer = 0;
    integer fb_miss = 0;
    integer fb_lines = 0;
    wire fb_active_w = (fb_words_left > 0);

    // Background high-priority client
    integer bg_next_at = 200;

    integer glen;
    reg doc_completing_v;

    function automatic integer line_in_frame(input integer abs_line);
        line_in_frame = abs_line % FRAME_LINES;
    endfunction

    always @(posedge clk) begin
        cyc <= cyc + 1;
        p_ready <= 0;
        doc_completing_v = 0;

        // Display line advance + line-not-ready check
        if (line_timer == LINE_CLKS - 1) begin
            line_timer <= 0;
            disp_abs <= disp_abs + 1;
            if (line_in_frame(disp_abs + 1) < ACTIVE_LINES) begin
                fb_lines <= fb_lines + 1;
                if (fetched_abs < disp_abs + 1) fb_miss <= fb_miss + 1;
            end
        end else begin
            line_timer <= line_timer + 1;
        end

        // FB fetch scheduling: vblank lines complete instantly; active
        // lines fetch when within the 2-line prefetch limit (continuous
        // when behind = catch-up)
        if (fb_words_left == 0) begin
            if (line_in_frame(fetched_abs + 1) >= ACTIVE_LINES) begin
                fetched_abs <= fetched_abs + 1;      // vblank line: nothing to do
            end else if (fetched_abs + 1 <= disp_abs + 2) begin
                fb_words_left <= LINE_WORDS;          // start fetching next line
                fb_next_ok_at <= cyc;
            end
        end

        // Grant completion
        if (srv_kind != 0 && cyc >= srv_end) begin
            case (srv_kind)
                3'd1: begin // FB burst8: 8 words delivered
                    fb_words_left <= fb_words_left - 8;
                    if (fb_words_left <= 8) fetched_abs <= fetched_abs + 1;
                    fb_next_ok_at <= cyc + 3;         // 2-deep CDC re-pend
                end
                3'd2: begin // DOC: req_done — free CDC slot, schedule resp
                    // 4-beat burst: the whole aligned 16-byte line, sampled
                    // at completion time (beat 0 = bytes 0-3)
                    resp_line <= {mem[{srv_addr[15:4], 4'hF}], mem[{srv_addr[15:4], 4'hE}],
                                  mem[{srv_addr[15:4], 4'hD}], mem[{srv_addr[15:4], 4'hC}],
                                  mem[{srv_addr[15:4], 4'hB}], mem[{srv_addr[15:4], 4'hA}],
                                  mem[{srv_addr[15:4], 4'h9}], mem[{srv_addr[15:4], 4'h8}],
                                  mem[{srv_addr[15:4], 4'h7}], mem[{srv_addr[15:4], 4'h6}],
                                  mem[{srv_addr[15:4], 4'h5}], mem[{srv_addr[15:4], 4'h4}],
                                  mem[{srv_addr[15:4], 4'h3}], mem[{srv_addr[15:4], 4'h2}],
                                  mem[{srv_addr[15:4], 4'h1}], mem[{srv_addr[15:4], 4'h0}]};
                    resp_beats <= 4;
                    resp_at <= cyc + 3;
                    dq_cnt <= dq_cnt - 1 + (p_rd ? 1 : 0);
                    doc_completing_v = 1;
                    // next queued request becomes arbiter-visible later
                    if (dq_cnt > 1) dq_vis[dq_hd] <= (dq_vis[dq_hd] > cyc + 4)
                                                     ? dq_vis[dq_hd] : cyc + 4;
                end
                3'd3: begin // background client
                    bg_next_at <= cyc + 90 + ({$random(rseed)} % 120);
                end
`ifdef REV2_MODE
                3'd4: begin // synthetic rev-2 word-granularity load
                    synth_next_at <= cyc + 230 + ({$random(rseed)} % 56);
                end
`endif
                default: ;
            endcase
            srv_kind <= 0;
        end else if (srv_kind == 0) begin
            // Grant pick (1-cycle S_DONE/S_IDLE hop modeled by the
            // completion branch above taking a full cycle)
            // Main-tree priority config: shadow/background (0-1) > DOC (2)
            // > FB read — the DOC now sits ABOVE the framebuffer.
            if (cyc >= bg_next_at) begin
                srv_kind <= 3;
                srv_end <= cyc + BG_GRANT;
`ifdef REV2_MODE
            end else if (cyc >= synth_next_at) begin
                srv_kind <= 3'd4;
                srv_end <= cyc + SYNTH_GRANT;
`endif
            end else if (dq_cnt > 0 && cyc >= dq_vis[dq_hd] && resp_at < 0) begin
                glen = DOC_GRANT;
                if (({$random(rseed)} % 100) < 3) glen = glen + REFRESH_EXTRA;
                if (({$random(rseed)} % 100) < outlier_pct) glen = outlier_len;
                srv_kind <= 2;
                srv_addr <= dq_addr[dq_hd];
                dq_hd <= dq_hd + 2'd1;
                srv_end <= cyc + glen;
            end else if (fb_active_w && cyc >= fb_next_ok_at) begin
                glen = FB_GRANT;
                if (({$random(rseed)} % 100) < 3) glen = glen + REFRESH_EXTRA;
                srv_kind <= 1;
                srv_end <= cyc + glen;
            end
        end

        // Response delivery to client: 4 beats, single-file, with random
        // 0-2 idle cycles between beats (imperfect-delivery realism —
        // grouped but not necessarily back-to-back)
        if (resp_at >= 0 && cyc >= resp_at && resp_beats > 0) begin
            p_q_word <= resp_line[31:0];
            resp_line <= {32'h0, resp_line[127:32]};
            p_ready <= 1;
            resp_beats <= resp_beats - 1;
            if (resp_beats == 1) resp_at <= -1;
            else resp_at <= cyc + 1 + ({$random(rseed)} % 3);
        end

        // DOC request accept (client fired only when p_avail was high)
        if (p_rd) begin
            if (dq_cnt >= 2 && !doc_completing_v) begin
                $display("FATAL: DOC port over-subscribed at cyc %0d", cyc);
                $finish;
            end
            dq_addr[dq_tl] <= p_addr;
            dq_vis[dq_tl] <= cyc + 6;   // req CDC sync + capture + pending
            dq_tl <= dq_tl + 2'd1;
            if (!doc_completing_v) dq_cnt <= dq_cnt + 1;
        end

        // Availability: <=2 outstanding; reassertion after req_done lags
        // by the 2FF gray sync (registered here = ~1-2 clk, close enough)
        p_avail <= (dq_cnt + (p_rd ? 1 : 0) - (doc_completing_v ? 1 : 0)) < 2;
    end

    // Flush pulse + written byte address to the pipelined DUT (GLU write)
    reg cache_flush = 0;
    reg [15:0] cache_flush_addr = 0;

    wire signed [15:0] p_mono, p_left, p_right;
    wire signed [15:0] p_chan[16];
    wire [7:0] p_dbg_en;
    wire [1:0] p_dbg_mode[8];
    wire [7:0] p_dbg_halt;
    wire [7:0] p_data_o;
    wire [7:0] dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop;
    wire [15:0] dbg_fetch_count;
    wire [7:0] dbg_frame_resync;

    doc5503_pipelined #(
        .FETCH_LOOKAHEAD(PIPE_LOOKAHEAD),
        .BANKS_IN_BSRAM(PIPE_BANKS_BSRAM),
        .RESET_DRAIN_CLKS(PIPE_DRAIN_CLKS)
    ) dut_pipe (
        .clk_i(clk),
        .reset_n_i(reset_n),
        .clk_en_i(clk_en),
        .cs_n_i(cs_n),
        .we_n_i(we_n),
        .addr_i(addr_i),
        .data_i(data_i),
        .data_o(p_data_o),
        .wave_address_o(p_addr),
        .wave_rd_o(p_rd),
        .wave_available_i(p_avail),
        .wave_data_ready_i(p_ready),
        .wave_data_i(8'h00),
        .wave_data_word_i(p_q_word),
        .cache_flush_i(cache_flush),
        .cache_flush_addr_i(cache_flush_addr),
        .fb_fetch_active_i(fb_active_w && gate_en),
        .mono_mix_o(p_mono),
        .left_mix_o(p_left),
        .right_mix_o(p_right),
        .channel_o(p_chan),
        .ready_o(),
        .debug_osc_en_o(p_dbg_en),
        .debug_osc_mode_o(p_dbg_mode),
        .debug_osc_halt_o(p_dbg_halt),
        .dbg_prime_miss_o(dbg_prime_miss),
        .dbg_stale_fetch_o(dbg_stale_fetch),
        .dbg_fetch_drop_o(dbg_fetch_drop),
        .dbg_fetch_count_o(dbg_fetch_count),
        .dbg_frame_resync_o(dbg_frame_resync)
    );

    // ---------------------------------------------------------------------
    // Event logging
    // ---------------------------------------------------------------------
    integer fb_f, fp_f, fe_f;
    reg [31:0] slot = 0;
    always @(posedge clk) if (dut_base.cycle_start_r) slot <= slot + 1;

    // Track the wavetable address each DUT's record corresponds to.
    // Baseline: address captured at the in-slot fetch. Pipelined: the
    // consumed byte's effective address ({cache tag, lane}), exported by
    // the DUT as consume_addr_r in the same NBA batch as the wds strobe.
    reg [15:0] last_addr_b [0:31];
    always @(posedge clk)
        if (b_rd) last_addr_b[dut_base.curr_osc_r] <= b_addr;

    localparam [1:0] ST_OSC = 2'b01;
    localparam [1:0] ST_R1  = 2'b11;
    localparam [1:0] ST_R0  = 2'b10;

    reg logging = 0;

    // -----------------------------------------------------------------
    // ABSOLUTE CADENCE EQUALITY (permanent assertion class).
    // The stream comparator aligns by service-slot INDEX and provably
    // cannot see a wall-clock cadence divergence (slots stretched or
    // slipped => bit-identical values at a different emission rate =
    // pitch shift on silicon). Assert, EVERY clk_i cycle, that both
    // DUTs' slot machines are in lockstep — slot starts, slot state,
    // oscillator sequencing, sub-slot clk_en count — and that the
    // mix-output update strobes (REFRESH_0, timer==15) coincide exactly.
    // This is strictly stronger than recorded timestamp lists.
    // -----------------------------------------------------------------
    integer cadence_errs = 0;
    integer mix_upd_count = 0;
    wire b_mixupd_w = (dut_base.cycle_state_r == ST_R0)
                      && (dut_base.cycle_timer_r < 'd16)
                      && (dut_base.cycle_timer_r[3:0] == 4'hF);
    wire p_mixupd_w = (dut_pipe.cycle_state_r == ST_R0)
                      && (dut_pipe.cycle_timer_r < 'd16)
                      && (dut_pipe.cycle_timer_r[3:0] == 4'hF);
    always @(posedge clk) begin
        if (reset_n) begin
            if (dut_base.cycle_start_r !== dut_pipe.cycle_start_r
             || dut_base.cycle_state_r !== dut_pipe.cycle_state_r
             || dut_base.curr_osc_r    !== dut_pipe.curr_osc_r
             || dut_base.clk_count_r   !== dut_pipe.clk_count_r
             || b_mixupd_w !== p_mixupd_w) begin
                if (cadence_errs < 10)
                    $display("CADENCE MISMATCH at cyc %0d slot %0d (start %b/%b state %b/%b osc %0d/%0d cnt %0d/%0d mix %b/%b)",
                             cyc, slot,
                             dut_base.cycle_start_r, dut_pipe.cycle_start_r,
                             dut_base.cycle_state_r, dut_pipe.cycle_state_r,
                             dut_base.curr_osc_r, dut_pipe.curr_osc_r,
                             dut_base.clk_count_r, dut_pipe.clk_count_r,
                             b_mixupd_w, p_mixupd_w);
                cadence_errs = cadence_errs + 1;
            end
            if (b_mixupd_w) mix_upd_count = mix_upd_count + 1;
        end
    end

    always @(posedge clk) begin
        if (logging) begin
            // Baseline records
            if (dut_base.ram_wds_we_r && dut_base.cycle_state_r == ST_OSC)
                $fdisplay(fb_f, "%0d W %0d %02x %02x %04x", slot,
                          dut_base.ram_wds_osc_r, dut_base.ram_wds_din_r,
                          dut_base.curr_vol_r, last_addr_b[dut_base.ram_wds_osc_r]);
            if (dut_base.ram_control_we_r && dut_base.cycle_state_r == ST_OSC)
                $fdisplay(fb_f, "%0d C %0d %02x", slot,
                          dut_base.ram_control_osc_r, dut_base.ram_control_din_r);
            if (dut_base.ram_vol_we_r && dut_base.cycle_state_r == ST_OSC)
                $fdisplay(fb_f, "%0d V %0d %02x", slot,
                          dut_base.ram_vol_osc_r, dut_base.ram_vol_din_r);
            if (dut_base.cycle_start_r && dut_base.cycle_state_r == ST_R1)
                $fdisplay(fb_f, "%0d M %0d %0d %0d", slot, b_mono, b_left, b_right);

            // Pipelined records
            if (dut_pipe.ram_wds_we_r && dut_pipe.cycle_state_r == ST_OSC)
                $fdisplay(fp_f, "%0d W %0d %02x %02x %04x", slot,
                          dut_pipe.ram_wds_osc_r, dut_pipe.ram_wds_din_r,
                          dut_pipe.curr_vol_r, dut_pipe.consume_addr_r);
            if (dut_pipe.ram_control_we_r && dut_pipe.cycle_state_r == ST_OSC)
                $fdisplay(fp_f, "%0d C %0d %02x", slot,
                          dut_pipe.ram_control_osc_r, dut_pipe.ram_control_din_r);
            if (dut_pipe.ram_vol_we_r && dut_pipe.cycle_state_r == ST_OSC)
                $fdisplay(fp_f, "%0d V %0d %02x", slot,
                          dut_pipe.ram_vol_osc_r, dut_pipe.ram_vol_din_r);
            if (dut_pipe.cycle_start_r && dut_pipe.cycle_state_r == ST_R1)
                $fdisplay(fp_f, "%0d M %0d %0d %0d", slot, p_mono, p_left, p_right);
        end
    end

    // ---------------------------------------------------------------------
    // Stimulus helpers
    // ---------------------------------------------------------------------

    task wait_slots(input integer n);
        integer i;
        begin
            for (i = 0; i < n; i = i + 1) begin
                @(posedge clk);
                while (!dut_base.cycle_start_r) @(posedge clk);
            end
        end
    endtask

    // Host register write, synchronized to the start of a service slot so
    // both DUTs deterministically apply it in the same slot (see design doc
    // section on the sub-slot register-application knife edge).
    task doc_wr(input [7:0] a, input [7:0] d);
        begin
            @(posedge clk);
            while (!dut_base.cycle_start_r) @(posedge clk);
            $fdisplay(fe_f, "R %0d %02x %02x", slot, a, d);
            @(negedge clk);
            addr_i = a; data_i = d; cs_n = 0; we_n = 0;
            @(negedge clk);
            cs_n = 1; we_n = 1;
            @(posedge clk);
        end
    endtask

    // Host register READ from both DUTs simultaneously; compares data_o
    // after both response paths settle (baseline: 1-cycle response;
    // pipelined: 2-cycle via the synchronous bank port B). Only called on
    // registers that are stable at the call site, so the comparison is
    // deterministic.
    integer hostread_ok = 0;
    integer hostread_errs = 0;
    integer sat_errs = 0;
    integer sat_prev = 0;
    task doc_rd(input [7:0] a);
        begin
            @(posedge clk);
            while (!dut_base.cycle_start_r) @(posedge clk);
            @(negedge clk);
            addr_i = a; cs_n = 0; we_n = 1;
            @(negedge clk);
            cs_n = 1;
            repeat (30) @(posedge clk);
            if (b_data_o !== p_data_o) begin
                $display("HOSTREAD MISMATCH addr=%02x base=%02x pipe=%02x slot=%0d",
                         a, b_data_o, p_data_o, slot);
                hostread_errs = hostread_errs + 1;
            end else begin
                hostread_ok = hostread_ok + 1;
            end
        end
    endtask

    // Host register write launched at a controlled cycle OFFSET within a
    // service slot — sweeps the write phase across every alignment of the
    // slot pipeline (boundary, chain launch points, idle tail).
    task doc_wr_at(input [7:0] a, input [7:0] d, input integer off);
        begin
            @(posedge clk);
            while (!dut_base.cycle_start_r) @(posedge clk);
            repeat (off) @(posedge clk);
            $fdisplay(fe_f, "R %0d %02x %02x", slot, a, d);
            @(negedge clk);
            addr_i = a; data_i = d; cs_n = 0; we_n = 0;
            @(negedge clk);
            cs_n = 1; we_n = 1;
            @(posedge clk);
        end
    endtask

    // Wavetable memory write (models a GLU sound-RAM write: both DUTs'
    // memory models see the new contents at the same instant, and the
    // pipelined DUT gets its cache_flush_i pulse)
    task glu_wr(input [15:0] a, input [7:0] d);
        begin
            $fdisplay(fe_f, "G %0d %04x %02x %02x", slot, a, mem[a], d);
            mem[a] = d;
            @(negedge clk);
            cache_flush_addr = a;
            cache_flush = 1;
            @(negedge clk);
            cache_flush = 0;
        end
    endtask

    // Per-phase traffic/deadline bookkeeping
    integer ph_fetch_base = 0;
    integer ph_fbmiss_base = 0;
    reg [31:0] ph_slot_base = 0;

    task phase_mark(input integer n);
        begin
            @(posedge clk);
            while (!dut_base.cycle_start_r) @(posedge clk);
            $fdisplay(fe_f, "PHASE %0d %0d", n, slot);
            $fdisplay(fe_f, "COUNTP %0d %0d %0d %0d", n,
                      dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);
            // Traffic and FB misses accumulated during the PREVIOUS phase
            $fdisplay(fe_f, "TRAFFIC %0d %0d %0d", n,
                      (dbg_fetch_count - ph_fetch_base) & 16'hFFFF,
                      slot - ph_slot_base);
            $fdisplay(fe_f, "FBM %0d %0d", n, fb_miss - ph_fbmiss_base);
            $fdisplay(fb_f, "%0d P %0d", slot, n);
            $fdisplay(fp_f, "%0d P %0d", slot, n);
            $display("PHASE %0d at slot %0d (prime=%0d stale=%0d drop=%0d fetches_prev_phase=%0d fbmiss_prev_phase=%0d)",
                     n, slot, dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop,
                     (dbg_fetch_count - ph_fetch_base) & 16'hFFFF,
                     fb_miss - ph_fbmiss_base);
            ph_fetch_base = dbg_fetch_count;
            ph_fbmiss_base = fb_miss;
            ph_slot_base = slot;
        end
    endtask

    // ---------------------------------------------------------------------
    // Wavetable initialization
    // ---------------------------------------------------------------------
    integer i;
    task init_mem;
        begin
            for (i = 0; i < 65536; i = i + 1) mem[i] = 8'h55;
            // T0 @0x0000, 256B, free-run (osc0; also osc8 SYNC_AM carrier)
            for (i = 0; i < 256; i = i + 1) mem[16'h0000 + i] = 8'h40 + (i & 8'h3F);
            // T1 @0x0100, 256B, free-run (osc1; also osc7 SYNC_AM modulator)
            for (i = 0; i < 256; i = i + 1) begin
                mem[16'h0100 + i] = (i * 3 + 1) & 8'hFF;
                if (mem[16'h0100 + i] == 8'h00) mem[16'h0100 + i] = 8'h01;
            end
            // T2 @0x0400-0x07FF, 1KB (osc2, wts=2, res=2)
            for (i = 0; i < 1024; i = i + 1) begin
                mem[16'h0400 + i] = (i * 5 + 7) & 8'hFF;
                if (mem[16'h0400 + i] == 8'h00) mem[16'h0400 + i] = 8'h07;
            end
            // T3 @0x0800, one-shot with 0x00 terminator at offset 0x41
            // (mid-word: exercises lane-accurate zero detection)
            for (i = 0; i < 256; i = i + 1) mem[16'h0800 + i] = 8'h90 + (i & 8'h0F);
            mem[16'h0841] = 8'h00;
            // T4 @0x0900, swap A, 0x00 at offset 0x31 (mid-word terminator)
            for (i = 0; i < 256; i = i + 1) mem[16'h0900 + i] = 8'h20 + (i & 8'h1F);
            mem[16'h0931] = 8'h00;
            // T5 @0x0A00, swap B, 0x00 at offset 0x22 (mid-word terminator)
            for (i = 0; i < 256; i = i + 1) mem[16'h0A00 + i] = 8'hC0 + (i & 8'h1F);
            mem[16'h0A22] = 8'h00;
            // T6 @0x0C00-0x0FFF, 1KB retarget destination for osc2's WTP test
            for (i = 0; i < 1024; i = i + 1) begin
                mem[16'h0C00 + i] = (i * 11 + 3) & 8'hFF;
                if (mem[16'h0C00 + i] == 8'h00) mem[16'h0C00 + i] = 8'h03;
            end
            // T8 @0x3000: constant 0xFF table (max positive samples) for
            // the mixer-saturation phase
            for (i = 0; i < 256; i = i + 1) mem[16'h3000 + i] = 8'hFF;
            // T7 @0x1000-0x2FFF: 8KB region shared by the all-32 phase
            // (pages 0x10-0x2F, one 256B table per oscillator)
            for (i = 0; i < 8192; i = i + 1) begin
                mem[16'h1000 + i] = (i * 7 + 9) & 8'hFF;
                if (mem[16'h1000 + i] == 8'h00) mem[16'h1000 + i] = 8'h09;
            end
        end
    endtask

    // ---------------------------------------------------------------------
    // Test sequence
    // ---------------------------------------------------------------------
    integer k;
    initial begin
        fb_f = $fopen("base.log", "w");
        fp_f = $fopen("pipe.log", "w");
        fe_f = $fopen("events.log", "w");

        init_mem;

        repeat (20) @(posedge clk);
        reset_n = 1;

        // Let CYCLE_RESET finish and a few scans run
        wait_slots(8);
        logging = 1;

        // ================= P0: configuration (all oscs halted) ===========
        phase_mark(0);
        doc_wr(8'hE1, 8'h12);            // osc_max = 9 → oscs 0..9 in scan

        // osc0: T0, free, FC=0x0200, vol FF
        doc_wr(8'h00, 8'h00); doc_wr(8'h20, 8'h02);
        doc_wr(8'h40, 8'hFF); doc_wr(8'h80, 8'h00);
        doc_wr(8'hC0, 8'h00); doc_wr(8'hA0, 8'h01);   // halted, free
        // osc1: T1, free, FC=0x0333, vol C0, ca=1
        doc_wr(8'h01, 8'h33); doc_wr(8'h21, 8'h03);
        doc_wr(8'h41, 8'hC0); doc_wr(8'h81, 8'h01);
        doc_wr(8'hC1, 8'h00); doc_wr(8'hA1, 8'h11);   // halted, free, ca=1
        // osc2: T2 (1KB, wts=2,res=2), FC=0x0155, vol A0
        doc_wr(8'h02, 8'h55); doc_wr(8'h22, 8'h01);
        doc_wr(8'h42, 8'hA0); doc_wr(8'h82, 8'h04);
        doc_wr(8'hC2, 8'h12); doc_wr(8'hA2, 8'h01);   // halted, free
        // osc3: T3, one-shot, FC=0x0200, vol 90
        doc_wr(8'h03, 8'h00); doc_wr(8'h23, 8'h02);
        doc_wr(8'h43, 8'h90); doc_wr(8'h83, 8'h08);
        doc_wr(8'hC3, 8'h00); doc_wr(8'hA3, 8'h03);   // halted, one-shot
        // osc4: T4, swap, FC=0x0400, vol 80
        doc_wr(8'h04, 8'h00); doc_wr(8'h24, 8'h04);
        doc_wr(8'h44, 8'h80); doc_wr(8'h84, 8'h09);
        doc_wr(8'hC4, 8'h00); doc_wr(8'hA4, 8'h07);   // halted, swap
        // osc5: T5, swap, FC=0x0200, vol 80
        doc_wr(8'h05, 8'h00); doc_wr(8'h25, 8'h02);
        doc_wr(8'h45, 8'h80); doc_wr(8'h85, 8'h0A);
        doc_wr(8'hC5, 8'h00); doc_wr(8'hA5, 8'h07);   // halted, swap
        // osc7: T1, SYNC_AM modulator (odd), FC=0x0180
        doc_wr(8'h07, 8'h80); doc_wr(8'h27, 8'h01);
        doc_wr(8'h47, 8'h00); doc_wr(8'h87, 8'h01);
        doc_wr(8'hC7, 8'h00); doc_wr(8'hA7, 8'h05);   // halted, sync_am
        // osc8: T0, SYNC_AM carrier (even), FC=0x0300
        doc_wr(8'h08, 8'h00); doc_wr(8'h28, 8'h03);
        doc_wr(8'h48, 8'h60); doc_wr(8'h88, 8'h00);
        doc_wr(8'hC8, 8'h00); doc_wr(8'hA8, 8'h05);   // halted, sync_am

        wait_slots(40);                 // let prime-once fetches land

        // Host register readback verification (all oscillators halted and
        // stable): exercises the pipelined variant's bank port-B read path
        for (k = 0; k < 6; k = k + 1) begin
            doc_rd(8'h00 + k[7:0]);     // FL
            doc_rd(8'h20 + k[7:0]);     // FH
            doc_rd(8'h40 + k[7:0]);     // vol
            doc_rd(8'h80 + k[7:0]);     // WTP
            doc_rd(8'hA0 + k[7:0]);     // control
            doc_rd(8'hC0 + k[7:0]);     // RTS
        end
        doc_rd(8'hE0);
        doc_rd(8'hE1);
        $display("HOSTREAD check: %0d ok, %0d mismatches", hostread_ok, hostread_errs);
        $fdisplay(fe_f, "HOSTREAD %0d %0d", hostread_ok, hostread_errs);

        // ================= P1: steady multi-osc free-run ================
        phase_mark(1);
        doc_wr(8'hA0, 8'h00);           // osc0 run
        doc_wr(8'hA1, 8'h10);           // osc1 run, ca=1
        doc_wr(8'hA2, 8'h00);           // osc2 run
        wait_slots(4000);

        // ================= P2: one-shot with 0x00 terminator ============
        phase_mark(2);
        doc_wr(8'hA3, 8'h02);           // osc3 run, one-shot
        wait_slots(1000);               // halts itself after ~65 samples

        // ================= P3: swap-mode looped pair, many iterations ===
        phase_mark(3);
        doc_wr(8'hA4, 8'h06);           // osc4 run, swap (osc5 stays halted+swap)
        wait_slots(30000);              // ~50+ swap loops — drift check

        // ================= P4: SYNC_AM pair ==============================
        phase_mark(4);
        doc_wr(8'hA4, 8'h07);           // park the swap pair
        doc_wr(8'hA5, 8'h07);
        doc_wr(8'hA7, 8'h04);           // osc7 run, sync_am (modulates vol[8])
        doc_wr(8'hA8, 8'h04);           // osc8 run, sync_am (overflow restarts acc7)
        wait_slots(6000);

        // ================= P5: mid-stream register writes ===============
        phase_mark(5);
        // FC sweep on osc0
        for (k = 0; k < 16; k = k + 1) begin
            doc_wr(8'h00, 8'((k * 8'h20) | 8'h05));
            wait_slots(40);
        end
        // Volume ramp on osc1
        for (k = 0; k < 16; k = k + 1) begin
            doc_wr(8'h41, 8'(8'hF0 - k * 8'h0E));
            wait_slots(25);
        end
        // WTP retarget on osc2 (T2 → T6 → T2)
        doc_wr(8'h82, 8'h0C);
        wait_slots(200);
        doc_wr(8'h82, 8'h04);
        wait_slots(200);
        // RTS change on osc2 (1KB/res2 → 256B/res0 → back)
        doc_wr(8'hC2, 8'h00);
        wait_slots(200);
        doc_wr(8'hC2, 8'h12);
        wait_slots(200);
        // Halt toggle on osc0 (halt, wait, un-halt: acc resets, prime path)
        doc_wr(8'hA0, 8'h01);
        wait_slots(100);
        doc_wr(8'hA0, 8'h00);
        wait_slots(300);
        // Control rewrite (CA change, halt stays 0) on RUNNING osc0 — this
        // resets ACC in this implementation (baseline behavior) and in the
        // pipelined variant additionally yields one centerline sample
        doc_wr(8'hA0, 8'h20);           // ca=2, free, running
        wait_slots(300);

        // ================= P6: E1 oscillator-count changes ==============
        phase_mark(6);
        doc_wr(8'hE1, 8'h28);           // osc_max = 20
        wait_slots(1000);
        doc_wr(8'hE1, 8'h04);           // osc_max = 2 (oscs 3..9 leave scan)
        wait_slots(1000);
        doc_wr(8'hE1, 8'h12);           // restore
        wait_slots(1500);

        // ================= P7: GLU writes to a playing table ============
        phase_mark(7);
        for (k = 0; k < 64; k = k + 1) begin
            glu_wr(16'h0000 + k[15:0] * 4, 8'h80 + k[7:0]);
            wait_slots(3);
        end
        wait_slots(800);                // > cooldown: re-primes coalesce here

        // ================= P8: ALL 32 oscillators + FB contention =======
        phase_mark(8);
        // Configure the remaining oscillators (6, 9..31): 256B tables in
        // the T7 region, FCs in the typical musical range (address steps
        // <= ~1.5 bytes/sample), free-run, halted for now.
        for (k = 0; k < 32; k = k + 1) begin
            if (k == 6 || k >= 9) begin
                doc_wr(8'h00 + k[7:0], 8'(8'h80 + ((k * 37) & 8'h7F))); // FL
                doc_wr(8'h20 + k[7:0], 8'(8'h01 + (k[7:0] & 8'h02)));   // FH: 0x01xx-0x03xx
                doc_wr(8'h40 + k[7:0], 8'(8'h40 + (k[7:0] * 4)));       // vol
                doc_wr(8'h80 + k[7:0], 8'(8'h10 + k[7:0]));             // wtp: T7 pages
                doc_wr(8'hC0 + k[7:0], 8'h00);                          // rts: 256B/res0
                doc_wr(8'hA0 + k[7:0], 8'h01);                          // halted, free
            end
        end
        doc_wr(8'hE1, 8'h3E);           // osc_max = 31: all 32 in scan
        wait_slots(80);                 // prime-once for the new oscillators
        // Start everything (osc3 one-shot re-runs; swap pair 4/5 as free)
        doc_wr(8'hA3, 8'h00);
        doc_wr(8'hA4, 8'h00);
        doc_wr(8'hA5, 8'h00);
        for (k = 0; k < 32; k = k + 1) begin
            if (k == 6 || k >= 9)
                doc_wr(8'hA0 + k[7:0], 8'h00);
        end
        wait_slots(20000);              // ~590 scans, ~700 FB lines

        // ================= P9: GLU write storm DURING all-32 playback ===
        // Realistic streaming: sequential byte writes at ~1 write/us in
        // bursts, at ARBITRARY bus phase (not slot-synchronized), walking
        // through tables that playing oscillators are consuming. This is
        // the scenario every earlier phase missed — bulk invalidation
        // regenerated rev-1-class traffic here; targeted invalidation
        // must keep traffic near quiescent and FB deadlines clean.
        phase_mark(9);
        for (k = 0; k < 900; k = k + 1) begin
            glu_wr(16'h1900 + k[15:0], 8'h80 | k[6:0]);   // never 0x00
            repeat (40 + ({$random(rseed)} % 28)) @(posedge clk);
            if ((k % 60) == 59) repeat (1000) @(posedge clk);
        end
        wait_slots(200);                // drain: dirty ranges rotate clear

        // ================= P10: frame-periodic host writes + GLU stream =
        // Realistic title behavior (Arkanoid-class): every "frame",
        // volume sweeps on several oscillators plus RETRIGGERS (control
        // writes with halt=0) on RUNNING oscillators, concurrent with
        // continued sample-RAM streaming — all during all-32 playback.
        // Swap-on-arrival requirement: ZERO prime misses here; retrigger
        // consumes serve the stale line until fresh f(0) data lands.
        phase_mark(10);
        for (k = 0; k < 12; k = k + 1) begin
            // volume sweep on six oscillators
            doc_wr(8'h40, 8'(8'hF0 - k*8'h08));
            doc_wr(8'h4A, 8'(8'h30 + k*8'h08));
            doc_wr(8'h4B, 8'(8'h40 + k*8'h04));
            doc_wr(8'h4C, 8'(8'hC0 - k*8'h08));
            doc_wr(8'h4D, 8'(8'h80 ^ (k[7:0]*8'h11)));
            doc_wr(8'h4E, 8'(8'h60 + k*8'h05));
            // retriggers on four RUNNING oscillators
            doc_wr(8'hA9, 8'h00);
            doc_wr(8'hAA, 8'h00);
            doc_wr(8'hAB, 8'h00);
            doc_wr(8'hAC, 8'h00);
            // concurrent sample-RAM stream (arbitrary phase)
            glu_wr(16'h1B00 + (k[15:0]*16'h20), 8'h80 | k[6:0]);
            glu_wr(16'h1B10 + (k[15:0]*16'h20), 8'h90 | k[6:0]);
            wait_slots(1400);            // ~one "frame"
        end

        // ================= P11: exhaustive write-phase sweep ============
        // Volume writes and retriggers launched at EVERY cycle offset
        // within the slot period, hitting the boundary and all three
        // chain-launch points — the register-path corruption audit.
        phase_mark(11);
        for (k = 0; k < 58; k = k + 1) begin
            doc_wr_at(8'h41, 8'(8'h80 + k[7:0]), k);   // volume, running osc1
            wait_slots(3);
            doc_wr_at(8'hA0, 8'h20, k);                // CA-rewrite/retrigger, RUNNING osc0
            wait_slots(3);
            doc_wr_at(8'hA9, 8'h00, k);                // retrigger, running osc9
            wait_slots(4);
        end
        wait_slots(100);

        // ================= P12: stale-repeat stress =====================
        phase_mark(12);
        doc_wr(8'hE1, 8'h02);           // osc_max = 1 → 4-slot scan (4.47 us)
        wait_slots(50);
        outlier_pct = 10;               // 10% of DOC grants take ~6.1 us
        wait_slots(3000);
        outlier_pct = 0;                // recovery: must re-sync
        wait_slots(50);
        phase_mark(13);
        wait_slots(400);

        // ================= P14: RESET WITH IN-FLIGHT BURST ==============
        // The orphan-tail scenario: force one very slow DOC grant, strike
        // reset mid-burst, and let the model (whose arbiter/CDC state
        // survives module reset, as on silicon) deliver the four tail
        // beats long after reset deasserts — after post-reset prime
        // issues would already be queued without the drain gate. With
        // RESET_DRAIN_CLKS the orphans are discarded+counted and framing
        // stays clean; with the NODRAIN repro build the tail is counted
        // into the first post-reset request = permanent entry skew.
        phase_mark(14);
        $fdisplay(fe_f, "FRSYNC_PRE %0d", dbg_frame_resync);
        outlier_pct = 100; outlier_len = 400;   // next grant: ~7.4 us
        wait (srv_kind == 3'd2);
        outlier_pct = 0;
        repeat (58) @(posedge clk);             // mid-burst
        logging = 0;                            // suppress half-reset log artifacts
        repeat (2) @(posedge clk);
        reset_n = 0;
        repeat (12) @(posedge clk);
        reset_n = 1;
        wait_slots(4);                          // past CYCLE_RESET's init-write slot
        logging = 1;
        wait_slots(56);                         // covers drain window + tail
        // Minimal re-init (post-reset both DUTs are factory-state).
        // Deliberately WITHOUT any prime_pending-setting write for osc0
        // (no control-halt, no WTP/RTS — the reset defaults are already
        // wtp=0/rts=0): a re-prime would heal an orphan-corrupted line
        // before playback and mask the framing defect this phase exists
        // to detect. The boot prime (prime_pending resets to all-ones)
        // is the one that must be delivered uncorrupted.
        doc_wr(8'hE1, 8'h12);
        doc_wr(8'h00, 8'h00); doc_wr(8'h20, 8'h02); doc_wr(8'h40, 8'hFF);
        wait_slots(30);
        doc_wr(8'hA0, 8'h00);                   // osc0 plays again
        wait_slots(2000);
        $display("FRSYNC after reset phase: %0d (orphan beats discarded)",
                 dbg_frame_resync);
        $fdisplay(fe_f, "FRSYNC %0d", dbg_frame_resync);

        // ================= P15: mixer saturation (>8 hot voices) ========
        // All voices play a constant-0xFF table at full volume: per-voice
        // contribution = 0x7F * 0xFF = 32385 per scan. The raw window
        // slice wraps at 8.09 voices; the pipelined DUT must CLAMP:
        // mono_mix == min(N*32385 >> 3, 32767), monotonic in N, never
        // negative. (Baseline keeps the legacy wrap — M-record diffs here
        // are intentional and report-only.)
        phase_mark(15);
        doc_wr(8'hE1, 8'h20);           // osc_max = 16 -> oscs 0..16
        for (k = 0; k < 16; k = k + 1) begin
            doc_wr(8'h00 + k[7:0], 8'h00);   // FL
            doc_wr(8'h20 + k[7:0], 8'h02);   // FH: FC=0x0200
            doc_wr(8'h40 + k[7:0], 8'hFF);   // vol max
            doc_wr(8'h80 + k[7:0], 8'h30);   // T8
            doc_wr(8'hC0 + k[7:0], 8'h00);   // 256B
            doc_wr(8'hA0 + k[7:0], 8'h01);   // halted
        end
        wait_slots(60);                 // prime
        begin : sat_check
            integer nvoice;
            integer exp_mix;
            integer got;
            for (nvoice = 1; nvoice <= 16; nvoice = nvoice + 1) begin
                doc_wr(8'hA0 + nvoice[7:0] - 8'd1, 8'h00);  // enable voice n-1
                wait_slots(60);
                got = $signed(dut_pipe.mono_mix_o);
                exp_mix = (nvoice * 32385) >> 3;
                if (exp_mix > 32767) exp_mix = 32767;
                if (got < exp_mix - 8 || got > exp_mix + 8) begin
                    $display("SAT FAIL: %0d voices: mono=%0d expected ~%0d",
                             nvoice, got, exp_mix);
                    sat_errs = sat_errs + 1;
                end
                if (got < sat_prev) begin
                    $display("SAT FAIL: non-monotonic at %0d voices (%0d < %0d)",
                             nvoice, got, sat_prev);
                    sat_errs = sat_errs + 1;
                end
                sat_prev = got;
            end
        end
        $display("SATCHECK: %0d errors", sat_errs);
        $fdisplay(fe_f, "SATCHECK %0d", sat_errs);

        phase_mark(16);
        $display("FINAL: prime_miss=%0d stale_fetch=%0d fetch_drop=%0d fetches=%0d fb_lines=%0d fb_miss=%0d",
                 dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop,
                 dbg_fetch_count, fb_lines, fb_miss);
        $fdisplay(fe_f, "COUNTERS %0d %0d %0d",
                  dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);
        $display("CADENCE: %0d mix-update strobes, %0d lockstep mismatches",
                 mix_upd_count, cadence_errs);
        $fdisplay(fe_f, "CADENCE %0d %0d", mix_upd_count, cadence_errs);
        $fdisplay(fe_f, "FBTOTAL %0d %0d", fb_lines, fb_miss);

        $fclose(fb_f);
        $fclose(fp_f);
        $fclose(fe_f);
        $finish;
    end

    // Safety timeout
    initial begin
        #1_500_000_000;  // 1.5 s of sim time
        $display("FATAL: timeout");
        $finish;
    end

endmodule
