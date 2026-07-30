// Differential testbench: baseline doc5503 (fast BSRAM-class memory) vs
// doc5503_pipelined (randomized-latency DDR3-class memory model).
//
// Both DUTs share clk / clk_en / reset and the host register bus, and both
// memory models read the SAME wavetable byte array, so any divergence in the
// logged architectural event streams is caused by the pipelined fetch, not
// by stimulus skew.
//
// Logged event streams (see compare.py for the grammar):
//   W slot osc data vol addr   — waveform-data-sample register file write
//                                (one per running-oscillator service slot;
//                                the consumed/fetched byte and the volume it
//                                is scaled by)
//   C slot osc val             — control register file write (halt/swap/
//                                retrigger events + host control writes)
//   V slot osc val             — volume register file write (host writes +
//                                SYNC_AM modulation writes)
//   M slot mono left right     — final mixer outputs, latched once per scan
//   P slot n                   — phase marker
//
// events.log additionally records host register writes (R), wavetable memory
// writes (G, with old/new bytes) and phase markers for the classifier.
//
// Run: ./run.sh   (iverilog -g2012)

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
    // Pipelined DUT + randomized-latency DDR3-class memory model
    //
    // Serialized service (like the arbiter): each request completes at
    // max(arrival, previous completion) + latency. Latency is uniform
    // lat_min..lat_max clk with outlier_pct% chance of outlier_lat clk.
    // Data is sampled from mem[] at COMPLETION time (worst-case staleness).
    // available reflects <=2 outstanding, like the ddr3_port_cdc request
    // FIFO; responses return strictly in order.
    // ---------------------------------------------------------------------
    integer lat_min = 11;        // ~200 ns @ 54 MHz
    integer lat_max = 65;        // ~1.2 us
    integer outlier_pct = 2;     // % of requests
    integer outlier_lat = 108;   // ~2 us
    integer rseed = 1;
    initial begin
        if ($value$plusargs("seed=%d", rseed))
            $display("Using random seed %0d", rseed);
    end

    wire [15:0] p_addr;
    wire        p_rd;
    reg         p_ready = 0;
    reg  [7:0]  p_q = 0;

    localparam MQ = 8;
    reg [15:0] q_addr [0:MQ-1];
    integer    q_done [0:MQ-1];
    reg [2:0]  q_head = 0, q_tail = 0;
    reg [3:0]  q_count = 0;
    integer    cyc = 0;
    integer    next_free = 0;
    reg        p_avail = 0;
    integer    lat;
    reg        pop_v, push_v;

    always @(posedge clk) begin
        cyc <= cyc + 1;
        p_ready <= 0;
        pop_v = 0;
        push_v = 0;

        if ((q_count != 0) && (cyc >= q_done[q_head])) begin
            p_q     <= mem[q_addr[q_head]];
            p_ready <= 1;
            q_head  <= q_head + 3'd1;
            pop_v = 1;
        end

        if (p_rd) begin
            if (q_count >= MQ) begin
                $display("FATAL: pipelined memory model queue overflow at cyc %0d", cyc);
                $finish;
            end
            lat = lat_min + ({$random(rseed)} % (lat_max - lat_min + 1));
            if (({$random(rseed)} % 100) < outlier_pct) lat = outlier_lat;
            q_addr[q_tail] <= p_addr;
            q_done[q_tail] <= ((cyc > next_free) ? cyc : next_free) + lat;
            next_free      <= ((cyc > next_free) ? cyc : next_free) + lat;
            q_tail         <= q_tail + 3'd1;
            push_v = 1;
        end

        q_count <= q_count + (push_v ? 4'd1 : 4'd0) - (pop_v ? 4'd1 : 4'd0);
        // Conservative availability (mirrors the CDC's 2-entry request FIFO)
        p_avail <= (q_count + (push_v ? 1 : 0) - (pop_v ? 1 : 0)) < 2;
    end

    wire signed [15:0] p_mono, p_left, p_right;
    wire signed [15:0] p_chan[16];
    wire [7:0] p_dbg_en;
    wire [1:0] p_dbg_mode[8];
    wire [7:0] p_dbg_halt;
    wire [7:0] p_data_o;
    wire [7:0] dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop;

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
        .wave_data_i(p_q),
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
        .dbg_fetch_drop_o(dbg_fetch_drop)
    );

    // ---------------------------------------------------------------------
    // Event logging
    // ---------------------------------------------------------------------
    integer fb, fp, fe;
    reg [31:0] slot = 0;
    always @(posedge clk) if (dut_base.cycle_start_r) slot <= slot + 1;

    // Track the wavetable address each DUT's record corresponds to.
    // Baseline: address captured at the in-slot fetch. Pipelined: the tag of
    // the consumed result-store entry.
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
                $fdisplay(fb, "%0d W %0d %02x %02x %04x", slot,
                          dut_base.ram_wds_osc_r, dut_base.ram_wds_din_r,
                          dut_base.curr_vol_r, last_addr_b[dut_base.ram_wds_osc_r]);
            if (dut_base.ram_control_we_r && dut_base.cycle_state_r == ST_OSC)
                $fdisplay(fb, "%0d C %0d %02x", slot,
                          dut_base.ram_control_osc_r, dut_base.ram_control_din_r);
            if (dut_base.ram_vol_we_r && dut_base.cycle_state_r == ST_OSC)
                $fdisplay(fb, "%0d V %0d %02x", slot,
                          dut_base.ram_vol_osc_r, dut_base.ram_vol_din_r);
            if (dut_base.cycle_start_r && dut_base.cycle_state_r == ST_R1)
                $fdisplay(fb, "%0d M %0d %0d %0d", slot, b_mono, b_left, b_right);

            // Pipelined records
            if (dut_pipe.ram_wds_we_r && dut_pipe.cycle_state_r == ST_OSC)
                $fdisplay(fp, "%0d W %0d %02x %02x %04x", slot,
                          dut_pipe.ram_wds_osc_r, dut_pipe.ram_wds_din_r,
                          dut_pipe.curr_vol_r,
                          dut_pipe.fetch_tag_r[dut_pipe.ram_wds_osc_r]);
            if (dut_pipe.ram_control_we_r && dut_pipe.cycle_state_r == ST_OSC)
                $fdisplay(fp, "%0d C %0d %02x", slot,
                          dut_pipe.ram_control_osc_r, dut_pipe.ram_control_din_r);
            if (dut_pipe.ram_vol_we_r && dut_pipe.cycle_state_r == ST_OSC)
                $fdisplay(fp, "%0d V %0d %02x", slot,
                          dut_pipe.ram_vol_osc_r, dut_pipe.ram_vol_din_r);
            if (dut_pipe.cycle_start_r && dut_pipe.cycle_state_r == ST_R1)
                $fdisplay(fp, "%0d M %0d %0d %0d", slot, p_mono, p_left, p_right);
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
            $fdisplay(fe, "R %0d %02x %02x", slot, a, d);
            @(negedge clk);
            addr_i = a; data_i = d; cs_n = 0; we_n = 0;
            @(negedge clk);
            cs_n = 1; we_n = 1;
            @(posedge clk);
        end
    endtask

    // Wavetable memory write (models a GLU sound-RAM write; both DUTs'
    // memory models see it at the same instant)
    task glu_wr(input [15:0] a, input [7:0] d);
        begin
            $fdisplay(fe, "G %0d %04x %02x %02x", slot, a, mem[a], d);
            mem[a] = d;
        end
    endtask

    task phase_mark(input integer n);
        begin
            @(posedge clk);
            while (!dut_base.cycle_start_r) @(posedge clk);
            $fdisplay(fe, "PHASE %0d %0d", n, slot);
            $fdisplay(fe, "COUNTP %0d %0d %0d %0d", n,
                      dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);
            $fdisplay(fb, "%0d P %0d", slot, n);
            $fdisplay(fp, "%0d P %0d", slot, n);
            $display("PHASE %0d at slot %0d (prime=%0d stale=%0d drop=%0d)",
                     n, slot, dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);
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
            // T3 @0x0800, one-shot with 0x00 terminator at offset 0x40 (osc3)
            for (i = 0; i < 256; i = i + 1) mem[16'h0800 + i] = 8'h90 + (i & 8'h0F);
            mem[16'h0840] = 8'h00;
            // T4 @0x0900, swap A, 0x00 at offset 0x30 (osc4)
            for (i = 0; i < 256; i = i + 1) mem[16'h0900 + i] = 8'h20 + (i & 8'h1F);
            mem[16'h0930] = 8'h00;
            // T5 @0x0A00, swap B, 0x00 at offset 0x20 (osc5)
            for (i = 0; i < 256; i = i + 1) mem[16'h0A00 + i] = 8'hC0 + (i & 8'h1F);
            mem[16'h0A20] = 8'h00;
            // T6 @0x0C00-0x0FFF, 1KB retarget destination for osc2's WTP test
            for (i = 0; i < 1024; i = i + 1) begin
                mem[16'h0C00 + i] = (i * 11 + 3) & 8'hFF;
                if (mem[16'h0C00 + i] == 8'h00) mem[16'h0C00 + i] = 8'h03;
            end
        end
    endtask

    // ---------------------------------------------------------------------
    // Test sequence
    // ---------------------------------------------------------------------
    integer k;
    initial begin
        fb = $fopen("base.log", "w");
        fp = $fopen("pipe.log", "w");
        fe = $fopen("events.log", "w");

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

        wait_slots(40);                 // let prefetch prime everything

        // ================= P1: steady multi-osc free-run ================
        phase_mark(1);
        doc_wr(8'hA0, 8'h00);           // osc0 run
        doc_wr(8'hA1, 8'h10);           // osc1 run, ca=1
        doc_wr(8'hA2, 8'h00);           // osc2 run
        wait_slots(4000);

        // ================= P2: one-shot with 0x00 terminator ============
        phase_mark(2);
        doc_wr(8'hA3, 8'h02);           // osc3 run, one-shot
        wait_slots(1000);               // halts itself after ~64 samples

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
        wait_slots(800);

        // ================= P8: stale-repeat stress ======================
        phase_mark(8);
        doc_wr(8'hE1, 8'h02);           // osc_max = 1 → 4-slot scan (4.47 us)
        wait_slots(50);
        outlier_pct = 10;
        outlier_lat = 330;              // ~6.1 us — exceeds the service period
        wait_slots(3000);
        outlier_pct = 0;                // recovery: must re-sync
        wait_slots(50);
        phase_mark(9);
        wait_slots(400);

        phase_mark(10);
        $display("FINAL: prime_miss=%0d stale_fetch=%0d fetch_drop=%0d",
                 dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);
        $fdisplay(fe, "COUNTERS %0d %0d %0d",
                  dbg_prime_miss, dbg_stale_fetch, dbg_fetch_drop);

        $fclose(fb);
        $fclose(fp);
        $fclose(fe);
        $finish;
    end

    // Safety timeout
    initial begin
        #1_000_000_000;  // 1 s of sim time
        $display("FATAL: timeout");
        $finish;
    end

endmodule
