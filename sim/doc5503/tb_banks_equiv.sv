// Equivalence testbench: baseline doc5503 with FF/SSRAM register banks
// (BANKS_IN_BSRAM=0, the reference) vs the same core with the banks in five
// Gowin DPB blocks (BANKS_IN_BSRAM=1, port A pipeline / port B host). Both
// DUTs see identical clocks, host traffic and wavetable memory through a
// fast 2-clock memory model. The BSRAM variant inserts wait cycles inside
// the oscillator slot, so the comparison is on architectural outputs:
//   * mixer outputs (mono/left/right) and channel_o every clock
//   * the ordered stream of wavetable fetch addresses
//   * host register read-back data
//   * debug_osc_* outputs every clock
// Host writes are swept across every cycle offset of the service slot so
// the forced-IDLE / LOAD_REGISTERS alignment (the one same-edge write/read
// case) is exercised, and the DOC_SIM_CHECKS assertion in osc_reg_bank_dpb
// flags any same-address write collision between the two DPB ports.
//
// Run: ./run_banks_equiv.sh   (iverilog -g2012 + Gowin gw5a prim_sim.v)

`timescale 1ns/1ps

module tb_banks_equiv;

    GSR GSR (.GSRI(1'b1));

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

    reg        cs_n = 1;
    reg        we_n = 1;
    reg [7:0]  addr_i = 0;
    reg [7:0]  data_i = 0;

    reg [7:0] mem [0:65535];

    // ---- memory models (identical, 2-clock) ----
    wire [15:0] r_addr, d_addr;
    wire        r_rd,   d_rd;
    reg         r_ready = 0, d_ready = 0;
    reg  [7:0]  r_q = 0, d_q = 0;
    reg         r_pend = 0, d_pend = 0;
    reg  [15:0] r_pend_addr = 0, d_pend_addr = 0;
    always @(posedge clk) begin
        r_ready <= 0; d_ready <= 0;
        if (r_pend) begin r_q <= mem[r_pend_addr]; r_ready <= 1; r_pend <= 0; end
        if (r_rd)   begin r_pend <= 1; r_pend_addr <= r_addr; end
        if (d_pend) begin d_q <= mem[d_pend_addr]; d_ready <= 1; d_pend <= 0; end
        if (d_rd)   begin d_pend <= 1; d_pend_addr <= d_addr; end
    end

    wire signed [15:0] r_mono, r_left, r_right, d_mono, d_left, d_right;
    wire signed [15:0] r_chan[16], d_chan[16];
    wire [7:0] r_en, d_en, r_halt, d_halt, r_data_o, d_data_o;
    wire [1:0] r_mode[8], d_mode[8];

    doc5503 #(.BANKS_IN_BSRAM(1'b0)) dut_ref (
        .clk_i(clk), .reset_n_i(reset_n), .clk_en_i(clk_en),
        .cs_n_i(cs_n), .we_n_i(we_n), .addr_i(addr_i), .data_i(data_i), .data_o(r_data_o),
        .wave_address_o(r_addr), .wave_rd_o(r_rd), .wave_data_ready_i(r_ready), .wave_data_i(r_q),
        .mono_mix_o(r_mono), .left_mix_o(r_left), .right_mix_o(r_right), .channel_o(r_chan),
        .ready_o(), .debug_osc_en_o(r_en), .debug_osc_mode_o(r_mode), .debug_osc_halt_o(r_halt));

    doc5503 #(.BANKS_IN_BSRAM(1'b1)) dut_bsram (
        .clk_i(clk), .reset_n_i(reset_n), .clk_en_i(clk_en),
        .cs_n_i(cs_n), .we_n_i(we_n), .addr_i(addr_i), .data_i(data_i), .data_o(d_data_o),
        .wave_address_o(d_addr), .wave_rd_o(d_rd), .wave_data_ready_i(d_ready), .wave_data_i(d_q),
        .mono_mix_o(d_mono), .left_mix_o(d_left), .right_mix_o(d_right), .channel_o(d_chan),
        .ready_o(), .debug_osc_en_o(d_en), .debug_osc_mode_o(d_mode), .debug_osc_halt_o(d_halt));

    // ---- comparators ----
    integer mix_errs = 0, chan_errs = 0, dbg_errs = 0, fetch_errs = 0, read_errs = 0, read_ok = 0;
    integer fetches = 0, mix_changes = 0, scans = 0;
    // coverage of the BSRAM-specific paths in the DUT
    integer cov_hold = 0, cov_hold_same = 0, cov_wait = 0, cov_prev = 0, cov_hostwr = 0;
    always @(posedge clk) if (reset_n) begin
        if (dut_bsram.hb_wr_last_r && dut_bsram.osc_state_r == dut_bsram.OSC_LOAD_REGISTERS) begin
            cov_hold <= cov_hold + 1;
            if (dut_bsram.host_addr_r[4:0] == dut_bsram.curr_osc_r) cov_hold_same <= cov_hold_same + 1;
        end
        if (dut_bsram.bank_wait_r) cov_wait <= cov_wait + 1;
        if (dut_bsram.prev_ctl_pend_r) cov_prev <= cov_prev + 1;
        if (dut_bsram.hb_we_any_w) cov_hostwr <= cov_hostwr + 1;
    end
    // negative controls: defeat the new timing logic and expect a FAIL
    initial begin
        if ($test$plusargs("nohold")) begin $display("NEGATIVE: hold defeated"); force dut_bsram.hb_wr_last_r = 1'b0; end
        if ($test$plusargs("nowait")) begin $display("NEGATIVE: wait defeated"); force dut_bsram.bank_wait_r = 1'b1; end
    end
    integer cyc = 0;
    integer k;

    // ordered fetch-address streams
    reg [15:0] rq [0:4095]; integer rq_w = 0, rq_r = 0;
    reg [15:0] dq [0:4095]; integer dq_w = 0, dq_r = 0;
    integer max_skew = 0;

    reg signed [15:0] r_mono_d = 0;
    always @(posedge clk) begin
        cyc <= cyc + 1;
        if (reset_n) begin
            if (r_mono !== d_mono || r_left !== d_left || r_right !== d_right) begin
                if (mix_errs < 20)
                    $display("MIX MISMATCH cyc=%0d ref m/l/r=%0d/%0d/%0d bsram=%0d/%0d/%0d",
                             cyc, r_mono, r_left, r_right, d_mono, d_left, d_right);
                mix_errs <= mix_errs + 1;
            end
            for (k = 0; k < 16; k = k + 1)
                if (r_chan[k] !== d_chan[k]) begin
                    if (chan_errs < 20) $display("CHAN MISMATCH cyc=%0d ch=%0d ref=%0d bsram=%0d", cyc, k, r_chan[k], d_chan[k]);
                    chan_errs <= chan_errs + 1;
                end
            // debug taps are updated mid-slot (two cycles later in the BSRAM
            // variant, inside the wait cycles); compare them at slot
            // boundaries, once the first scan has initialised them
            if (dut_ref.cycle_start_r && scans > 1 && (r_en !== d_en || r_halt !== d_halt)) begin
                if (dbg_errs < 20) $display("DBG MISMATCH cyc=%0d en %02x/%02x halt %02x/%02x", cyc, r_en, d_en, r_halt, d_halt);
                dbg_errs <= dbg_errs + 1;
            end
            if (dut_ref.cycle_start_r && dut_ref.curr_osc_r == 5'd0) scans <= scans + 1;
            if (r_mono !== r_mono_d) mix_changes <= mix_changes + 1;
            r_mono_d <= r_mono;

            if (r_rd) begin rq[rq_w[11:0]] = r_addr; rq_w = rq_w + 1; fetches <= fetches + 1; end
            if (d_rd) begin dq[dq_w[11:0]] = d_addr; dq_w = dq_w + 1; end
            while (rq_r < rq_w && dq_r < dq_w) begin
                if (rq[rq_r[11:0]] !== dq[dq_r[11:0]]) begin
                    if (fetch_errs < 20)
                        $display("FETCH MISMATCH #%0d cyc=%0d ref=%04x bsram=%04x", rq_r, cyc, rq[rq_r[11:0]], dq[dq_r[11:0]]);
                    fetch_errs <= fetch_errs + 1;
                end
                rq_r = rq_r + 1; dq_r = dq_r + 1;
            end
            if ((rq_w - rq_r) > max_skew) max_skew = rq_w - rq_r;
            if ((dq_w - dq_r) > max_skew) max_skew = dq_w - dq_r;
        end
    end

    // ---- host access tasks ----
    task wait_slot_start;
        begin
            @(posedge clk);
            while (!dut_ref.cycle_start_r) @(posedge clk);
        end
    endtask

    task doc_wr_at(input [7:0] a, input [7:0] d, input integer off);
        begin
            wait_slot_start;
            repeat (off) @(posedge clk);
            @(negedge clk);
            addr_i = a; data_i = d; cs_n = 0; we_n = 0;
            @(negedge clk);
            cs_n = 1; we_n = 1;
            @(posedge clk);
        end
    endtask

    task doc_wr(input [7:0] a, input [7:0] d);
        doc_wr_at(a, d, 0);
    endtask

    task doc_rd_at(input [7:0] a, input integer off);
        begin
            wait_slot_start;
            repeat (off) @(posedge clk);
            @(negedge clk);
            addr_i = a; cs_n = 0; we_n = 1;
            @(negedge clk);
            cs_n = 1;
            repeat (40) @(posedge clk);
            if (r_data_o !== d_data_o) begin
                if (read_errs < 20)
                    $display("HOSTREAD MISMATCH addr=%02x off=%0d ref=%02x bsram=%02x cyc=%0d", a, off, r_data_o, d_data_o, cyc);
                read_errs = read_errs + 1;
            end else read_ok = read_ok + 1;
        end
    endtask

    task wait_slots(input integer n);
        integer i;
        for (i = 0; i < n; i = i + 1) wait_slot_start;
    endtask

    // ---- stimulus ----
    integer seed = 7;
    integer i, o, r, off;
    reg [7:0] v;

    task program_osc(input [4:0] osc, input [7:0] fl, input [7:0] fh, input [7:0] vol,
                     input [7:0] wtp, input [7:0] rts, input [7:0] ctl);
        begin
            doc_wr(8'h00 + osc, fl);
            doc_wr(8'h20 + osc, fh);
            doc_wr(8'h40 + osc, vol);
            doc_wr(8'h80 + osc, wtp);
            doc_wr(8'hC0 + osc, rts);
            doc_wr(8'hA0 + osc, ctl);
        end
    endtask

    function [7:0] rnd8; input integer dummy; begin rnd8 = $random(seed); end endfunction

    initial begin
        if ($value$plusargs("seed=%d", seed)) $display("seed %0d", seed);
        // wavetable: random bytes, no zeros in the first 32 KB (free-running
        // tables), zeros sprinkled in the upper half (one-shot/halt-zero paths)
        for (i = 0; i < 65536; i = i + 1) begin
            v = $random(seed);
            if (i < 32768 && v == 8'h00) v = 8'h01;
            if (i >= 32768 && (($random(seed) & 63) == 0)) v = 8'h00;
            mem[i] = v;
        end

        repeat (20) @(posedge clk);
        reset_n = 1;
        wait_slots(40);     // reset cycle + a few scans

        // Phase 1: 32 oscillators, mixed modes and table sizes
        for (o = 0; o < 32; o = o + 1) begin
            r = $random(seed);
            program_osc(o[4:0],
                        rnd8(0), 8'h01 + (rnd8(0) & 8'h0F),            // fl, fh (audible range)
                        rnd8(0),                                      // vol
                        (o < 16) ? 8'h00 + (rnd8(0) & 8'h7F) : 8'h80 | (rnd8(0) & 8'h7F), // wtp: lower/upper half
                        {2'b00, 3'(r >> 4), 3'(r >> 8)},              // rts: wts, res
                        {4'(r >> 12), 2'(r >> 2), 1'b0});             // ctl: ca, mode, running
        end
        doc_wr(8'hE1, 8'h3E);          // all 32 oscillators enabled
        wait_slots(2000);

        // Phase 2: host writes swept across every slot offset, incl. control
        // writes that clear the halt bit (acc reset via port B) and reads
        for (i = 0; i < 400; i = i + 1) begin
            off = $random(seed) & 63;
            o = $random(seed) & 31;
            case ($random(seed) & 7)
                0: doc_wr_at(8'h00 + o, rnd8(0), off);
                1: doc_wr_at(8'h20 + o, 8'h01 + (rnd8(0) & 8'h0F), off);
                2: doc_wr_at(8'h40 + o, rnd8(0), off);
                3: doc_wr_at(8'h80 + o, rnd8(0), off);
                4: doc_wr_at(8'hC0 + o, rnd8(0) & 8'h3F, off);
                5: doc_wr_at(8'hA0 + o, rnd8(0) & 8'hFE, off);            // running, random mode
                6: doc_wr_at(8'hA0 + o, (rnd8(0) & 8'hF6) | 8'h01, off);  // halted
                7: begin
                       r = $random(seed) & 7;
                       doc_rd_at((r == 7) ? 8'hE1 : {r[2:0], o[4:0]}, off);
                   end
            endcase
            if ((i % 25) == 24) wait_slots(60);
        end
        wait_slots(1000);

        // Phase 2b: targeted same-edge hazard — a host write to oscillator K's
        // own registers timed to execute in the forced-IDLE cycle that starts
        // K's slot (the port-B write and the port-A LOAD_REGISTERS read then
        // meet on one clock edge). Sweep the launch offset across the tail of
        // the preceding slot so every alignment is hit.
        for (o = 0; o < 32; o = o + 1)
            for (off = 48; off < 64; off = off + 1) begin
                @(posedge clk);
                while (!(dut_ref.cycle_start_r && dut_ref.curr_osc_r == ((o + 31) % 32))) @(posedge clk);
                repeat (off) @(posedge clk);
                @(negedge clk);
                case (off % 4)
                    0: begin addr_i = 8'h00 + o; data_i = rnd8(0); end                  // fl
                    1: begin addr_i = 8'h40 + o; data_i = rnd8(0); end                  // vol
                    2: begin addr_i = 8'h80 + o; data_i = rnd8(0); end                  // wtp
                    3: begin addr_i = 8'hA0 + o; data_i = rnd8(0) & 8'hFE; end          // control, running
                endcase
                cs_n = 0; we_n = 0;
                @(negedge clk);
                cs_n = 1; we_n = 1;
                @(posedge clk);
            end
        wait_slots(400);

        // Phase 3: read back every register of every oscillator at random offsets
        for (o = 0; o < 32; o = o + 1)
            for (r = 0; r < 7; r = r + 1)
                doc_rd_at({r[2:0], o[4:0]}, $random(seed) & 63);

        // Phase 4: wavetable rewrite under playback + reprogram with halts
        for (i = 0; i < 2000; i = i + 1) mem[$random(seed) & 16'hFFFF] = $random(seed);
        for (o = 0; o < 32; o = o + 1) begin
            r = $random(seed);
            doc_wr_at(8'hA0 + o, {4'(r >> 12), 2'(r >> 2), 1'(r & 1)}, $random(seed) & 63);
        end
        wait_slots(1500);

        // Phase 5: fewer oscillators enabled (shorter scans), then full again
        doc_wr(8'hE1, 8'h0E);
        wait_slots(800);
        doc_wr(8'hE1, 8'h3E);
        wait_slots(800);

        $display("FINAL fetches=%0d mix_changes=%0d scans=%0d max_fetch_skew=%0d host_reads_ok=%0d",
                 fetches, mix_changes, scans, max_skew, read_ok);
        $display("COVERAGE hold=%0d hold_same_osc=%0d wait=%0d prev=%0d hostwr=%0d", cov_hold, cov_hold_same, cov_wait, cov_prev, cov_hostwr);
        $display("FINAL mix_errs=%0d chan_errs=%0d dbg_errs=%0d fetch_errs=%0d read_errs=%0d",
                 mix_errs, chan_errs, dbg_errs, fetch_errs, read_errs);
        if (mix_errs == 0 && chan_errs == 0 && dbg_errs == 0 && fetch_errs == 0 && read_errs == 0 &&
            fetches > 2000 && mix_changes > 100 && read_ok > 200)
            $display("BANKS EQUIV RESULT: PASS");
        else
            $display("BANKS EQUIV RESULT: FAIL");
        $finish;
    end
endmodule
