// Differential testbench: baseline doc5503 (fast BSRAM-class memory) vs
// doc5503_pipelined (word cache + serialized DDR3 arbiter contention model
// with a competing framebuffer client).
//
// Both DUTs share clk / clk_en / reset and the host register bus, and both
// memory models read the SAME wavetable byte array, so any divergence in the
// logged architectural event streams is caused by the pipelined fetch, not
// by stimulus skew.
//
// The pipelined DUT's memory model reflects the a2mega ddr3_ports reality
// that broke the first prototype on hardware:
//   * a serialized server — each grant occupies the arbiter NON-PREEMPTIBLY
//     for 600-1000 ns (uniform), regardless of requester;
//   * a competing FB client with strictly higher priority that needs 16
//     word grants per 31.7 us line window, but (like the real CDC) can only
//     keep ONE request pending at a time with a turnaround gap between its
//     own grants — which is why a continuously-pending DOC steals alternate
//     grants and halves FB throughput;
//   * the DOC port accepts at most 2 outstanding requests (ddr3_port_cdc),
//     responses strictly in order, 32-bit word data sampled at grant
//     completion (worst-case staleness).
// The TB counts FB line-deadline misses; outside the forced-overload stress
// phase they must be zero with all 32 oscillators running.
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
    // Pipelined DUT + serialized-arbiter contention model
    // ---------------------------------------------------------------------
    integer grant_min = 33;      // ~600 ns @ 54 MHz
    integer grant_max = 54;      // ~1000 ns
    integer outlier_pct = 0;     // % of DOC grants stretched (stress phase)
    integer outlier_len = 330;   // ~6.1 us
    integer rseed = 1;
    initial begin
        if ($value$plusargs("seed=%d", rseed))
            $display("Using random seed %0d", rseed);
    end

    wire [15:0] p_addr;
    wire        p_rd;
    reg         p_ready = 0;
    reg  [31:0] p_q_word = 0;

    // DOC request queue (arrival order; <=2 outstanding enforced via p_avail)
    localparam MQ = 8;
    reg [15:0] q_addr [0:MQ-1];
    reg [2:0]  q_head = 0, q_tail = 0;
    reg [3:0]  q_count = 0;
    integer    cyc = 0;
    reg        p_avail = 0;

    // Serialized server: 0 = idle, 1 = FB grant, 2 = DOC grant
    reg [1:0]  srv_kind = 0;
    integer    srv_busy_until = 0;
    reg [15:0] srv_doc_addr = 0;

    // FB client: 16 word grants per line window; can keep only ONE request
    // pending at a time with a CDC-turnaround gap between its own grants.
    localparam FB_WINDOW = 1712;    // 31.7 us @ 54 MHz
    localparam FB_GAP    = 8;       // CDC turnaround between FB grants
    reg        fb_enabled = 0;
    integer    fb_win_timer = 0;
    integer    fb_remaining = 0;
    integer    fb_gap = 0;
    integer    fb_miss = 0;
    integer    fb_lines = 0;

    integer glen;
    reg pop_v, push_v, freed_v;

    always @(posedge clk) begin
        cyc <= cyc + 1;
        p_ready <= 0;
        pop_v = 0;
        push_v = 0;
        freed_v = (srv_kind != 0) && (cyc >= srv_busy_until);

        // FB line window bookkeeping
        if (fb_enabled) begin
            if (fb_win_timer == 0) begin
                if (fb_remaining > 0) fb_miss <= fb_miss + 1;
                fb_lines <= fb_lines + 1;
                fb_remaining <= 16;
                fb_win_timer <= FB_WINDOW - 1;
            end else begin
                fb_win_timer <= fb_win_timer - 1;
            end
            if (fb_gap > 0) fb_gap <= fb_gap - 1;
        end

        // Grant completion
        if (freed_v) begin
            if (srv_kind == 2) begin
                // DOC word returned; data sampled NOW (worst-case staleness)
                p_q_word <= {mem[{srv_doc_addr[15:2], 2'b11}],
                             mem[{srv_doc_addr[15:2], 2'b10}],
                             mem[{srv_doc_addr[15:2], 2'b01}],
                             mem[{srv_doc_addr[15:2], 2'b00}]};
                p_ready <= 1;
            end else begin
                fb_remaining <= fb_remaining - 1;
                fb_gap <= FB_GAP;
            end
            srv_kind <= 0;
        end

        // Grant issue (only when idle this cycle; a freed server re-grants
        // next cycle — models the arbiter's S_DONE/S_IDLE hop)
        if (srv_kind == 0 && !freed_v) begin
            if (fb_enabled && fb_remaining > 0 && fb_gap == 0) begin
                glen = grant_min + ({$random(rseed)} % (grant_max - grant_min + 1));
                srv_kind <= 1;
                srv_busy_until <= cyc + glen;
            end else if (q_count != 0) begin
                glen = grant_min + ({$random(rseed)} % (grant_max - grant_min + 1));
                if (({$random(rseed)} % 100) < outlier_pct) glen = outlier_len;
                srv_kind <= 2;
                srv_doc_addr <= q_addr[q_head];
                srv_busy_until <= cyc + glen;
                q_head <= q_head + 3'd1;
                pop_v = 1;
            end
        end

        // DOC request accept
        if (p_rd) begin
            if (q_count >= MQ) begin
                $display("FATAL: DOC request queue overflow at cyc %0d", cyc);
                $finish;
            end
            q_addr[q_tail] <= p_addr;
            q_tail <= q_tail + 3'd1;
            push_v = 1;
        end

        q_count <= q_count + (push_v ? 4'd1 : 4'd0) - (pop_v ? 4'd1 : 4'd0);
        // Conservative availability: <=2 DOC requests outstanding
        // (queued + in service), mirroring the ddr3_port_cdc request FIFO
        p_avail <= (q_count + (push_v ? 1 : 0) - (pop_v ? 1 : 0)
                    + ((srv_kind == 2 && !freed_v) ? 1 : 0)) < 2;
    end

    // Flush pulse to the pipelined DUT (GLU sound-RAM write)
    reg cache_flush = 0;

    wire signed [15:0] p_mono, p_left, p_right;
    wire signed [15:0] p_chan[16];
    wire [7:0] p_dbg_en;
    wire [1:0] p_dbg_mode[8];
    wire [7:0] p_dbg_halt;
    wire [7:0] p_data_o;
    wire [7:0] dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop;
    wire [15:0] dbg_fetch_count;

    doc5503_pipelined dut_pipe (
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
        .dbg_fetch_count_o(dbg_fetch_count)
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

    reg logging = 0;

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

    // Wavetable memory write (models a GLU sound-RAM write: both DUTs'
    // memory models see the new contents at the same instant, and the
    // pipelined DUT gets its cache_flush_i pulse)
    task glu_wr(input [15:0] a, input [7:0] d);
        begin
            $fdisplay(fe_f, "G %0d %04x %02x %02x", slot, a, mem[a], d);
            mem[a] = d;
            @(negedge clk);
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
        fb_enabled = 1;          // FB client contends from the start

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

        // ================= P9: stale-repeat stress ======================
        phase_mark(9);
        doc_wr(8'hE1, 8'h02);           // osc_max = 1 → 4-slot scan (4.47 us)
        wait_slots(50);
        outlier_pct = 10;               // 10% of DOC grants take ~6.1 us
        wait_slots(3000);
        outlier_pct = 0;                // recovery: must re-sync
        wait_slots(50);
        phase_mark(10);
        wait_slots(400);

        phase_mark(11);
        $display("FINAL: prime_miss=%0d stale_fetch=%0d fetch_drop=%0d fetches=%0d fb_lines=%0d fb_miss=%0d",
                 dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop,
                 dbg_fetch_count, fb_lines, fb_miss);
        $fdisplay(fe_f, "COUNTERS %0d %0d %0d",
                  dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);
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
