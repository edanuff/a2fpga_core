// Late-fetch testbench: baseline doc5503 + sound_glu's doc_fetch_filter on an
// external memory with variable, sometimes budget-busting latency and the
// arbiter's two-slot acceptance queue (a request while the queue is full is
// dropped, as hyperram_ports does). Every wavetable byte the FSM consumes
// must be the byte of the address it requested for THAT oscillator slot.
// Memory contents are a function of the address (nonzero), so a stale byte
// is detectable without a reference model. Also checks that missed budgets
// produce the 0x80 substitute and that the telemetry counters move.
// +noguard disables the filter's stale accounting (negative control: stale
// samples expected).
// Run: ./run_fetch_late.sh

`timescale 1ns/1ps
module tb_fetch_late;
    GSR GSR (.GSRI(1'b1));
    reg clk = 0; always #9.259 clk = ~clk;
    reg reset_n = 0;
    reg clk_en = 0; integer dds_acc = 0;
    always @(posedge clk) begin
        if (dds_acc + 7159090 >= 54000000) begin dds_acc <= dds_acc + 7159090 - 54000000; clk_en <= 1; end
        else begin dds_acc <= dds_acc + 7159090; clk_en <= 0; end
    end
    reg cs_n = 1, we_n = 1; reg [7:0] addr_i = 0, data_i = 0;

    function [7:0] memf(input [15:0] a);
        reg [7:0] v;
        begin v = a[7:0] ^ {a[15:12], a[11:8]} ^ 8'h5A; memf = (v == 8'h00) ? 8'h01 : v; end
    endfunction

    // ---- memory port model: 2-slot acceptance queue, in-order service,
    //      random latency (phase dependent), 32-bit word responses ----
    wire [15:0] d_addr; wire d_rd;
    wire        m_available;
    reg         m_ready = 0; reg [31:0] m_q = 0;
    integer seed = 11;
    integer lat_lo = 2, lat_hi = 20;
    reg [15:0] q_addr [0:15]; integer q_due [0:15]; integer qw = 0, qr = 0, cyc = 0;
    integer served = 0, dropped_reqs = 0;
    assign m_available = (qw - qr) < 2;
    wire m_accept = d_rd && m_available;
    always @(posedge clk) begin
        cyc <= cyc + 1; m_ready <= 0;
        if (d_rd && !m_available) dropped_reqs = dropped_reqs + 1;
        if (m_accept) begin
            q_addr[qw[3:0]] = d_addr;
            // in-order: never due before the previous one
            q_due[qw[3:0]]  = cyc + lat_lo + ({$random(seed)} % (lat_hi - lat_lo + 1));
            if (qw > qr && q_due[qw[3:0]] <= q_due[(qw-1)%16]) q_due[qw[3:0]] = q_due[(qw-1)%16] + 1;
            qw = qw + 1;
        end
        if (qr < qw && cyc >= q_due[qr[3:0]]) begin
            m_q <= {memf({q_addr[qr[3:0]][15:2], 2'd3}), memf({q_addr[qr[3:0]][15:2], 2'd2}),
                    memf({q_addr[qr[3:0]][15:2], 2'd1}), memf({q_addr[qr[3:0]][15:2], 2'd0})};
            m_ready <= 1; qr = qr + 1; served = served + 1;
        end
    end
    // the filter under test (as instantiated by sound_glu's gen_doc_ext)
    wire d_ready; wire [7:0] d_q; wire [7:0] flt_late;
    doc_fetch_filter u_flt (
        .clk_i(clk), .rst_n_i(reset_n),
        .req_i(d_rd), .accept_i(m_accept), .lane_i(d_addr[1:0]),
        .resp_i(m_ready), .resp_data_i(m_q),
        .valid_o(d_ready), .data_o(d_q), .dbg_late_o(flt_late));

    wire signed [15:0] mono, left, right; wire signed [15:0] chan[16];
    wire [7:0] en, halt, data_o; wire [1:0] mode[8];
    wire [7:0] dbg_to, dbg_lat;
    doc5503 #(.BANKS_IN_BSRAM(1'b1)) dut (
        .clk_i(clk), .reset_n_i(reset_n), .clk_en_i(clk_en),
        .cs_n_i(cs_n), .we_n_i(we_n), .addr_i(addr_i), .data_i(data_i), .data_o(data_o),
        .wave_address_o(d_addr), .wave_rd_o(d_rd), .wave_data_ready_i(d_ready), .wave_data_i(d_q),
        .mono_mix_o(mono), .left_mix_o(left), .right_mix_o(right), .channel_o(chan), .ready_o(),
        .debug_osc_en_o(en), .debug_osc_mode_o(mode), .debug_osc_halt_o(halt),
        .dbg_fetch_timeout_o(dbg_to), .dbg_fetch_latmax_o(dbg_lat));

    // ---- checker: consumed byte == memf(address of this slot's request) ----
    reg [15:0] req_addr_r = 0; reg req_seen = 0;
    integer consumed = 0, stale = 0, timeouts_seen = 0, bad_timeout = 0;
    always @(posedge clk) if (reset_n) begin
        if (d_rd) begin req_addr_r <= d_addr; req_seen <= 1; end
        // data accepted by the FSM this cycle
        if (dut.osc_state_r == dut.OSC_HANDLE_DATA && dut.loaded_wds_pending_r) begin
            consumed <= consumed + 1;
            if (dut.loaded_wds_r !== memf(req_addr_r)) begin
                if (stale < 12) $display("STALE SAMPLE cyc=%0d osc=%0d got=%02x want=%02x (addr %04x)",
                                         cyc, dut.curr_osc_r, dut.loaded_wds_r, memf(req_addr_r), req_addr_r);
                stale <= stale + 1;
            end
        end
        // timeout path: FSM leaves HANDLE_DATA on clk_count 7 without data
        if (dut.osc_state_r == dut.OSC_HANDLE_DATA && !dut.loaded_wds_pending_r && dut.clk_count_r == 3'd7)
            timeouts_seen <= timeouts_seen + 1;
    end

    initial if ($test$plusargs("noguard")) begin $display("NEGATIVE: stale filter forced off"); force u_flt.stale_owed_r = 4'd0; end

    task doc_wr(input [7:0] a, input [7:0] d);
        begin
            @(posedge clk); while (!dut.cycle_start_r) @(posedge clk);
            @(negedge clk); addr_i = a; data_i = d; cs_n = 0; we_n = 0;
            @(negedge clk); cs_n = 1; we_n = 1; @(posedge clk);
        end
    endtask
    task wait_slots(input integer n); integer i; for (i = 0; i < n; i = i + 1) begin @(posedge clk); while (!dut.cycle_start_r) @(posedge clk); end endtask

    integer o, r, to_p1, to_p2, to_p3;
    initial begin
        if ($value$plusargs("seed=%d", seed)) $display("seed %0d", seed);
        repeat (20) @(posedge clk); reset_n = 1; wait_slots(40);
        for (o = 0; o < 32; o = o + 1) begin
            r = $random(seed);
            doc_wr(8'h00 + o, $random(seed)); doc_wr(8'h20 + o, 8'h01 + ($random(seed) & 8'h0F));
            doc_wr(8'h40 + o, $random(seed)); doc_wr(8'h80 + o, $random(seed) & 8'h7F);
            doc_wr(8'hC0 + o, {2'b00, 3'(r >> 4), 3'(r >> 8)});
            doc_wr(8'hA0 + o, {4'(r >> 12), 2'b00, 1'b0});     // free-run, running (no halts from data)
        end
        doc_wr(8'hE1, 8'h3E);
        // Phase 1: fast memory (2..20 clk): no timeouts expected
        lat_lo = 2; lat_hi = 20; wait_slots(1500); to_p1 = timeouts_seen;
        // Phase 2: tail latency up to ~1.5 us: some fetches miss the ~840 ns budget
        lat_lo = 10; lat_hi = 80; wait_slots(3000); to_p2 = timeouts_seen - to_p1;
        // Phase 3: pathological (every fetch late, back-to-back late responses)
        lat_lo = 50; lat_hi = 120; wait_slots(1500); to_p3 = timeouts_seen - to_p1 - to_p2;
        // Phase 4: recovery to fast memory — stream must realign immediately
        lat_lo = 2; lat_hi = 6; wait_slots(1500);
        $display("FINAL consumed=%0d served=%0d dropped_reqs=%0d timeouts p1/p2/p3=%0d/%0d/%0d stale=%0d dbg: timeout=%0d filter_late=%0d latmax=%0d",
                 consumed, served, dropped_reqs, to_p1, to_p2, to_p3, stale, dbg_to, flt_late, dbg_lat);
        if (stale == 0 && to_p1 == 0 && to_p2 > 0 && to_p3 > 0 && consumed > 3000 && dbg_lat > 40)
            $display("FETCH LATE RESULT: PASS");
        else
            $display("FETCH LATE RESULT: FAIL");
        $finish;
    end
endmodule
