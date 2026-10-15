// Bench for doc_fetch_cache in front of the baseline doc5503: two-slot
// in-order memory port with variable (sometimes budget-busting) latency, a
// MUTABLE wavetable (rewritten under playback with the GLU-style flush pulse)
// and the same check as tb_fetch_late: every byte the DOC consumes must be the
// current memory content of the address it requested for that slot. Reports
// hit/miss counts (traffic reduction) and timeouts. +noflush suppresses the
// coherency flush (negative control: stale cached words expected).
`timescale 1ns/1ps
module tb_fetch_cache;
    GSR GSR (.GSRI(1'b1));
    reg clk = 0; always #9.259 clk = ~clk;
    reg reset_n = 0;
    reg clk_en = 0; integer dds_acc = 0;
    always @(posedge clk) begin
        if (dds_acc + 7159090 >= 54000000) begin dds_acc <= dds_acc + 7159090 - 54000000; clk_en <= 1; end
        else begin dds_acc <= dds_acc + 7159090; clk_en <= 0; end
    end
    reg cs_n = 1, we_n = 1; reg [7:0] addr_i = 0, data_i = 0;

    reg [7:0] wav [0:65535];
    integer seed = 23, k;
    reg noflush = 0;

    // ---- memory port model ----
    wire        m_rd; wire [13:0] m_waddr;
    reg         m_ready = 0; reg [31:0] m_q = 0;
    integer lat_lo = 2, lat_hi = 20;
    reg [13:0] q_a [0:15]; integer q_due [0:15]; integer qw = 0, qr = 0, cyc = 0;
    integer served = 0, dropped = 0;
    wire m_available = (qw - qr) < 2;
    wire m_accept = m_rd && m_available;
    always @(posedge clk) begin
        cyc <= cyc + 1; m_ready <= 0;
        if (m_rd && !m_available) dropped = dropped + 1;
        if (m_accept) begin
            q_a[qw[3:0]] = m_waddr;
            q_due[qw[3:0]] = cyc + lat_lo + ({$random(seed)} % (lat_hi - lat_lo + 1));
            if (qw > qr && q_due[qw[3:0]] <= q_due[(qw-1)%16]) q_due[qw[3:0]] = q_due[(qw-1)%16] + 1;
            qw = qw + 1;
        end
        if (qr < qw && cyc >= q_due[qr[3:0]]) begin
            m_q <= {wav[{q_a[qr[3:0]],2'd3}], wav[{q_a[qr[3:0]],2'd2}], wav[{q_a[qr[3:0]],2'd1}], wav[{q_a[qr[3:0]],2'd0}]};
            m_ready <= 1; qr = qr + 1; served = served + 1;
        end
    end

    // ---- DUT: DOC + cache ----
    wire [15:0] d_addr; wire d_rd; wire d_ready; wire [7:0] d_q;
    reg flush = 0;
    wire [15:0] c_hits, c_miss; wire [7:0] c_late;
    doc_fetch_cache u_cache (
        .clk_i(clk), .rst_n_i(reset_n),
        .req_i(d_rd), .addr_i(d_addr), .flush_i(flush), .valid_o(d_ready), .data_o(d_q),
        .mem_rd_o(m_rd), .mem_addr_o(m_waddr), .mem_available_i(m_available), .mem_ready_i(m_ready), .mem_q_i(m_q),
        .dbg_hits_o(c_hits), .dbg_misses_o(c_miss), .dbg_late_o(c_late));
    wire signed [15:0] mono, left, right; wire signed [15:0] chan[16];
    wire [7:0] en, halt, data_o; wire [1:0] mode[8]; wire [15:0] dbg_to; wire [7:0] dbg_lat;
    doc5503 #(.BANKS_IN_BSRAM(1'b1)) dut (
        .clk_i(clk), .reset_n_i(reset_n), .clk_en_i(clk_en),
        .cs_n_i(cs_n), .we_n_i(we_n), .addr_i(addr_i), .data_i(data_i), .data_o(data_o),
        .wave_address_o(d_addr), .wave_rd_o(d_rd), .wave_data_ready_i(d_ready), .wave_data_i(d_q),
        .mono_mix_o(mono), .left_mix_o(left), .right_mix_o(right), .channel_o(chan), .ready_o(),
        .debug_osc_en_o(en), .debug_osc_mode_o(mode), .debug_osc_halt_o(halt),
        .dbg_fetch_timeout_o(dbg_to), .dbg_fetch_latmax_o(dbg_lat));

    // ---- checker ----
    reg [15:0] req_addr_r = 0; integer consumed = 0, stale = 0, timeouts = 0, hits = 0, misses = 0, reqs = 0;
    always @(posedge clk) if (reset_n) begin
        if (d_rd) begin req_addr_r <= d_addr; reqs <= reqs + 1; end
        if (dut.osc_state_r == dut.OSC_HANDLE_DATA && dut.loaded_wds_pending_r) begin
            consumed <= consumed + 1;
            if (dut.loaded_wds_r !== wav[req_addr_r]) begin
                if (stale < 10) $display("STALE cyc=%0d osc=%0d got=%02x want=%02x addr=%04x", cyc, dut.curr_osc_r, dut.loaded_wds_r, wav[req_addr_r], req_addr_r);
                stale <= stale + 1;
            end
        end
        if (dut.osc_state_r == dut.OSC_HANDLE_DATA && !dut.loaded_wds_pending_r && dut.clk_count_r == 3'd7) timeouts <= timeouts + 1;
        if (d_rd && u_cache.hit_w) hits <= hits + 1;
        if (m_accept) misses <= misses + 1;
    end

    task doc_wr(input [7:0] a, input [7:0] d);
        begin
            @(posedge clk); while (!dut.cycle_start_r) @(posedge clk);
            @(negedge clk); addr_i = a; data_i = d; cs_n = 0; we_n = 0;
            @(negedge clk); cs_n = 1; we_n = 1; @(posedge clk);
        end
    endtask
    task wait_slots(input integer n); integer i; for (i = 0; i < n; i = i + 1) begin @(posedge clk); while (!dut.cycle_start_r) @(posedge clk); end endtask
    // GLU-style wavetable write: memory changes, flush pulses (unless +noflush)
    task wav_wr(input [15:0] a, input [7:0] d);
        begin @(negedge clk); wav[a] = d; if (!noflush) flush = 1; @(negedge clk); flush = 0; end
    endtask

    integer o, r, h1, m1, r1;
    initial begin
        if ($value$plusargs("seed=%d", seed)) $display("seed %0d", seed);
        if ($test$plusargs("noflush")) begin noflush = 1; $display("NEGATIVE: coherency flush suppressed"); end
        for (k = 0; k < 65536; k = k + 1) begin wav[k] = k[7:0] ^ {k[15:12], k[11:8]} ^ 8'h5A; if (wav[k] == 0) wav[k] = 8'h01; end
        repeat (20) @(posedge clk); reset_n = 1; wait_slots(40);
        for (o = 0; o < 32; o = o + 1) begin
            r = $random(seed);
            doc_wr(8'h00 + o, $random(seed)); doc_wr(8'h20 + o, (o < 16) ? 8'h00 : 8'h01 + ($random(seed) & 8'h07)); // 0-15: < 1 byte/sample (music), 16-31: fast walks
            doc_wr(8'h40 + o, $random(seed)); doc_wr(8'h80 + o, $random(seed) & 8'h7F);
            doc_wr(8'hC0 + o, {2'b00, 3'(r >> 4), 3'(r >> 8)}); doc_wr(8'hA0 + o, {4'(r >> 12), 2'b00, 1'b0});
        end
        doc_wr(8'hE1, 8'h3E);
        // P1 fast memory: hit rate + no stale
        lat_lo = 2; lat_hi = 20; wait_slots(2000); h1 = hits; m1 = misses; r1 = reqs;
        $display("P1 requests=%0d hits=%0d (hit rate %0d%%) memreads=%0d (incl. prefetch) timeouts=%0d", r1, h1, (100*h1)/(r1+1), m1, timeouts);
        // P2 tail latency
        lat_lo = 10; lat_hi = 80; wait_slots(2500);
        // P3 wavetable rewrite under playback (coherency), with tail latency
        for (k = 0; k < 3000; k = k + 1) begin wav_wr($random(seed) & 16'h7FFF, $random(seed) | 8'h01); if ((k % 50) == 49) wait_slots(3); end
        wait_slots(1500);
        // P4 pathological
        lat_lo = 50; lat_hi = 120; wait_slots(1200);
        lat_lo = 2; lat_hi = 6; wait_slots(800);
        $display("FINAL consumed=%0d served=%0d dropped=%0d hits=%0d misses=%0d timeouts=%0d stale=%0d dbg: to=%0d latmax=%0d hits=%0d miss=%0d late=%0d",
                 consumed, served, dropped, hits, misses, timeouts, stale, dbg_to, dbg_lat, c_hits, c_miss, c_late);
        if (stale == 0 && consumed > 3000 && (100*h1)/(r1+1) >= 40)
            $display("FETCH CACHE RESULT: PASS");
        else
            $display("FETCH CACHE RESULT: FAIL");
        $finish;
    end
endmodule
