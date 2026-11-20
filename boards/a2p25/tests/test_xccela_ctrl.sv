//
// hyperram_ctrl in Xccela mode against the APS6408L model — `make xccela`
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES.
//
// hyperram_ctrl (PROTOCOL = 1, or 2 for boot-time detection) +
// hyperram_phy_gw5a against sim_models/aps6408l_model.sv through
// test_hyperram_harness compiled with -DXCCELA_DEVICE (board delay,
// contention; the model checks its own pin timing).
//
// Plusargs: +BOARD_DLY_PS=<ps> +SEED=<n> +NTRANS=<n> +XC_COLD (the model
// ignores everything until a Global Reset: exercises the probe retry).
//
// Sequence: boot (probe, MR0/MR4, calibration), directed write-placement
// checks in every latency setting, randomized phases (writes/reads, len
// 1..16, random byte enables, scoreboard) after runtime {MR4, MR0}
// changes: LC/WLC 5/5 var, 4/4 var, 3/3 var, 5/5 fixed, 4/5 var.
// Checks: data, MR0/MR4 in the model == cr0_active, writes never report
// 2x, fixed-mode reads report 2x, refresh-pushout reads occurred, no
// timeouts, no undefined instructions / page wraps / ignored commands,
// timing and contention clean, detection result. Last line:
// "XCCELA_CTRL PASS ..." / "... FAIL ...".
//

`timescale 1ns / 1ps

module test_xccela_ctrl;

    localparam ADDR_WIDTH = 21;
    localparam [ADDR_WIDTH-1:0] CAL_ADDR = {ADDR_WIDTH{1'b1}} - 15;
    parameter  PROTOCOL = 1;

    // ---------------------------------------------------------------------
    // Harness + DUT
    // ---------------------------------------------------------------------
    wire        clk, clk_mem, clk_mem_90;
    wire        hr_ck, hr_cs_n;
    wire [7:0]  hr_dq;
    wire        hr_rwds;
    wire [7:0]  h_dq_in;
    wire        h_rwds_in;
    wire [7:0]  f_dq_drv;
    wire        f_rwds_drv;
    integer     timing_errors, contention_errors;

    GSR GSR (.GSRI(gsr_n));
    logic gsr_n = 1'b0;
    initial #20 gsr_n = 1'b1;

    test_hyperram_harness u_h (
        .clk               (clk),
        .clk_mem           (clk_mem),
        .clk_mem_90        (clk_mem_90),
        .hr_ck             (hr_ck),
        .hr_cs_n           (hr_cs_n),
        .f_dq_drv          (f_dq_drv),
        .f_rwds_drv        (f_rwds_drv),
        .hr_dq_in          (h_dq_in),
        .hr_rwds_in        (h_rwds_in),
        .inj_xor           (8'h00),
        .timing_errors     (timing_errors),
        .contention_errors (contention_errors)
    );

    assign hr_dq   = h_dq_in;
    assign hr_rwds = h_rwds_in;
    // The boards pull PSRAM_RWDS down at the FPGA pad (CST PULL_MODE=DOWN):
    // an undriven strobe reads 0, never X. Matters when the device is dead
    // or has not been reset (the HyperBus latency sample would otherwise go X).
    pulldown (hr_rwds);

    genvar gi;
    generate
        for (gi = 0; gi < 8; gi = gi + 1) begin : g_drv
            assign f_dq_drv[gi] = u_dut.u_phy.dq_tri[gi] ? 1'bz : u_dut.u_phy.dq_out[gi];
        end
    endgenerate
    assign f_rwds_drv = u_dut.u_phy.rwds_tri ? 1'bz : u_dut.u_phy.rwds_out;

    logic        rst_n = 1'b0;
    logic        req = 1'b0, we = 1'b0;
    logic [ADDR_WIDTH-1:0] addr = '0;
    logic [4:0]  len = 5'd1;
    logic [31:0] wdata = '0;
    logic [3:0]  wbe = 4'hF;
    logic        wbuf_we = 1'b0;
    logic [3:0]  wbuf_idx = '0;
    logic [31:0] wbuf_data = '0;
    logic [3:0]  wbuf_be = '0;
    logic        recal = 1'b0;
    logic [15:0] cfg_cr0 = '0;
    logic        cfg_cr0_write = 1'b0;

    wire         ready, init_done, rvalid, done, rd_err, xfer_lat2x, cal_ok;
    wire  [31:0] rdata, cal_map, stat_xfers, stat_lat2x, stat_rd_timeouts, stat_lat_mismatch;
    wire  [15:0] cr0_active;
    wire  [7:0]  dly_code;
    wire  [4:0]  rd_arrival;
    wire         proto_xccela, det_fail;
    wire  [15:0] dev_id;

    hyperram_ctrl #(
        .CLK_HZ     (54_000_000),
        .ADDR_WIDTH (ADDR_WIDTH),
        .PROTOCOL   (PROTOCOL),
        .XC_RLC     (5),
        .XC_WLC     (5)
    ) u_dut (
        .clk               (clk),
        .clk_mem           (clk_mem),
        .clk_mem_90        (clk_mem_90),
        .clk_cap           (clk_mem),
        .rst_n             (rst_n),
        .ready             (ready),
        .init_done         (init_done),
        .req               (req),
        .we                (we),
        .addr              (addr),
        .len               (len),
        .wdata             (wdata),
        .wbe               (wbe),
        .wbuf_we           (wbuf_we),
        .wbuf_idx          (wbuf_idx),
        .wbuf_data         (wbuf_data),
        .wbuf_be           (wbuf_be),
        .rvalid            (rvalid),
        .rdata             (rdata),
        .done              (done),
        .rd_err            (rd_err),
        .xfer_lat2x        (xfer_lat2x),
        .recal             (recal),
        .cfg_cr0           (cfg_cr0),
        .cfg_cr0_write     (cfg_cr0_write),
        .cr0_active        (cr0_active),
        .dly_force         (1'b0),
        .dly_force_code    (8'd0),
        .dly_code          (dly_code),
        .cal_tap           (),
        .cal_map           (cal_map),
        .cal_ok            (cal_ok),
        .rd_arrival        (rd_arrival),
        .stat_xfers        (stat_xfers),
        .stat_lat2x        (stat_lat2x),
        .stat_rd_timeouts  (stat_rd_timeouts),
        .stat_lat_mismatch (stat_lat_mismatch),
        .proto_xccela      (proto_xccela),
        .dev_id            (dev_id),
        .det_fail          (det_fail),
        .hr_ck             (hr_ck),
        .hr_cs_n           (hr_cs_n),
        .hr_rwds           (hr_rwds),
        .hr_dq             (hr_dq)
    );

    // ---------------------------------------------------------------------
    // Scoreboard
    // ---------------------------------------------------------------------
    bit   [31:0] shadow [0:(1<<ADDR_WIDTH)-1];
    integer      errors = 0;
    integer      n_wr = 0, n_rd = 0, n_words_rd = 0;
    integer      n_rd2x_var = 0, n_2x_fixed = 0, n_fixed = 0;
    integer      seed;
    integer      ntrans;
    logic        cur_fixed = 1'b0;
    logic        trace = 1'b0;
    logic        cold;

    logic [31:0] rd_buf [0:15];
    integer      rd_cnt;
    always @(posedge clk) begin
        if (rvalid) begin
            if (rd_cnt < 16) rd_buf[rd_cnt] = rdata;
            rd_cnt = rd_cnt + 1;
        end
    end

    task automatic fail(input string msg);
        begin
            errors = errors + 1;
            if (errors <= 20) $display("[%0t] ERROR %s", $time, msg);
        end
    endtask

    task automatic wait_done();
        integer guard;
        begin
            guard = 0;
            while (!done && guard < 2000) begin
                @(posedge clk);
                guard = guard + 1;
            end
            if (!done) begin
                fail("transaction hang");
                $display("XCCELA_CTRL FAIL (hang)");
                $finish;
            end
        end
    endtask

    task automatic apply(input [ADDR_WIDTH-1:0] a, input [31:0] d, input [3:0] be);
        integer k;
        begin
            for (k = 0; k < 4; k = k + 1)
                if (be[k]) shadow[a][k*8 +: 8] = d[k*8 +: 8];
        end
    endtask

    task automatic do_write(input [ADDR_WIDTH-1:0] a, input integer n);
        integer i;
        logic [31:0] d, w0;
        logic [3:0]  be, b0;
        begin
            while (!ready) @(posedge clk);
            for (i = 1; i < n; i = i + 1) begin
                d  = $random(seed);
                be = $random(seed);
                wbuf_we   <= 1'b1;
                wbuf_idx  <= i[3:0];
                wbuf_data <= d;
                wbuf_be   <= be;
                @(posedge clk);
                apply(a + i, d, be);
            end
            wbuf_we <= 1'b0;
            w0 = $random(seed);
            b0 = $random(seed);
            req   <= 1'b1;
            we    <= 1'b1;
            addr  <= a;
            len   <= n[4:0];
            wdata <= w0;
            wbe   <= b0;
            @(posedge clk);
            req <= 1'b0;
            apply(a, w0, b0);
            wait_done();
            if (xfer_lat2x) fail("Xccela write reported 2x latency");
            if (cur_fixed) n_fixed = n_fixed + 1;
            n_wr = n_wr + 1;
            if (trace) $display("[%0t] WR %h len %0d", $time, a, n);
        end
    endtask

    task automatic do_read(input [ADDR_WIDTH-1:0] a, input integer n);
        integer i;
        begin
            while (!ready) @(posedge clk);
            rd_cnt = 0;
            req  <= 1'b1;
            we   <= 1'b0;
            addr <= a;
            len  <= n[4:0];
            @(posedge clk);
            req <= 1'b0;
            wait_done();
            @(posedge clk);
            if (rd_cnt != n)
                fail($sformatf("read len %0d got %0d words", n, rd_cnt));
            if (rd_err)
                fail($sformatf("read timeout at %h len %0d", a, n));
            for (i = 0; i < n && i < rd_cnt; i = i + 1)
                if (rd_buf[i] !== shadow[a + i])
                    fail($sformatf("read %h+%0d: got %h exp %h (len %0d lat2x %0d)",
                                   a, i, rd_buf[i], shadow[a + i], n, xfer_lat2x));
            if (xfer_lat2x) begin
                if (cur_fixed) n_2x_fixed = n_2x_fixed + 1;
                else           n_rd2x_var = n_rd2x_var + 1;
            end else if (cur_fixed) begin
                fail("fixed-latency read reported 1x");
            end
            if (cur_fixed) n_fixed = n_fixed + 1;
            n_rd = n_rd + 1;
            n_words_rd = n_words_rd + n;
            if (trace) $display("[%0t] RD %h len %0d lat2x %0d", $time, a, n, xfer_lat2x);
        end
    endtask

    function automatic [7:0] mk_mr0(input integer lc, input logic fixed);
        logic [2:0] code;
        begin
            code = (lc == 3) ? 3'b000 : (lc == 4) ? 3'b001 : (lc == 5) ? 3'b010 :
                   (lc == 6) ? 3'b011 : 3'b100;
            mk_mr0 = {2'b00, fixed, code, 2'b00};
        end
    endfunction

    function automatic [7:0] mk_mr4(input integer wlc);
        logic [2:0] code;
        begin
            code = (wlc == 3) ? 3'b000 : (wlc == 4) ? 3'b100 : (wlc == 5) ? 3'b010 :
                   (wlc == 6) ? 3'b110 : 3'b001;
            mk_mr4 = {code, 5'b00000};
        end
    endfunction

    task automatic set_mr(input integer lc, input integer wlc, input logic fixed);
        begin
            while (!ready) @(posedge clk);
            cfg_cr0       <= {mk_mr4(wlc), mk_mr0(lc, fixed)};
            cfg_cr0_write <= 1'b1;
            @(posedge clk);
            cfg_cr0_write <= 1'b0;
            @(posedge clk);
            while (!ready) @(posedge clk);
            repeat (4) @(posedge clk);
            if (u_h.u_xram.mr0 !== cr0_active[7:0] || u_h.u_xram.mr4 !== cr0_active[15:8])
                fail($sformatf("model MR0/MR4 %h/%h != active %h", u_h.u_xram.mr0, u_h.u_xram.mr4, cr0_active));
            cur_fixed = fixed;
            $display("[%0t] {MR4,MR0} = %h (LC %0d WLC %0d, %s)", $time, cr0_active, lc, wlc,
                     fixed ? "fixed 2x" : "variable");
        end
    endtask

    // Write a known burst and check where it landed in the model array
    // (independent of the read path): verifies the write data start cycle.
    task automatic placement_check(input [ADDR_WIDTH-1:0] a, input integer n);
        integer i, k;
        logic [31:0] exp;
        logic [7:0]  m;
        begin
            while (!ready) @(posedge clk);
            for (i = 1; i < n; i = i + 1) begin
                wbuf_we   <= 1'b1;
                wbuf_idx  <= i[3:0];
                wbuf_data <= {a[11:0], i[3:0], 16'hC0DE ^ i[15:0]};
                wbuf_be   <= 4'hF;
                @(posedge clk);
                apply(a + i, {a[11:0], i[3:0], 16'hC0DE ^ i[15:0]}, 4'hF);
            end
            wbuf_we <= 1'b0;
            req   <= 1'b1;
            we    <= 1'b1;
            addr  <= a;
            len   <= n[4:0];
            wdata <= {a[11:0], 4'h0, 16'hC0DE};
            wbe   <= 4'hF;
            @(posedge clk);
            req <= 1'b0;
            apply(a, {a[11:0], 4'h0, 16'hC0DE}, 4'hF);
            wait_done();
            repeat (4) @(posedge clk);
            for (i = -1; i <= n; i = i + 1) begin
                exp = shadow[a + i];
                for (k = 0; k < 4; k = k + 1) begin
                    m = u_h.u_xram.Mem[(a + i) * 4 + k];
                    if (m !== exp[k*8 +: 8])
                        fail($sformatf("placement %h+%0d.%0d: model %h exp %h (len %0d cfg %h)",
                                       a, i, k, m, exp[k*8 +: 8], n, cr0_active));
                end
            end
            if (trace) $display("[%0t] PLACE %h len %0d", $time, a, n);
        end
    endtask

    // Force refresh pushout: start reads just as the model's refresh window
    // opens until `need` pushed-out reads have been seen.
    task automatic collision_hunt(input integer need);
        integer tries, p0, l;
        logic [ADDR_WIDTH-1:0] a;
        begin
            p0 = u_h.u_xram.n_pushout;
            tries = 0;
            while (u_h.u_xram.n_pushout - p0 < need && tries < 200) begin
                @(posedge u_h.u_xram.refresh_pulse);
                @(posedge clk);
                l = ($random(seed) & 15) + 1;
                a = in_page($random(seed) & 12'hFFF, l);
                do_read(a, l);
                tries = tries + 1;
            end
            if (u_h.u_xram.n_pushout - p0 < need)
                fail($sformatf("collision hunt: only %0d pushed-out reads in %0d tries",
                               u_h.u_xram.n_pushout - p0, tries));
            else if (trace)
                $display("[%0t] collision hunt: %0d tries", $time, tries);
        end
    endtask

    task automatic perf(input logic is_we, input integer n, input integer reps);
        integer r, c_tot, c_lat, t_req, clkcnt_start;
        begin
            for (r = 1; r < n; r = r + 1) begin
                while (!ready) @(posedge clk);
                wbuf_we <= 1'b1; wbuf_idx <= r[3:0]; wbuf_data <= r; wbuf_be <= 4'hF;
                @(posedge clk);
            end
            wbuf_we <= 1'b0;
            while (!ready) @(posedge clk);
            clkcnt_start = clkcnt;
            c_lat = 0;
            for (r = 0; r < reps; r = r + 1) begin
                while (!ready) @(posedge clk);
                req <= 1'b1; we <= is_we; addr <= 21'h18_0000 + r * 16; len <= n[4:0];
                wdata <= r; wbe <= 4'hF;
                t_req = clkcnt;
                @(posedge clk);
                req <= 1'b0;
                wait_done();
                c_lat = c_lat + (clkcnt - t_req);
            end
            c_tot = clkcnt - clkcnt_start;
            $display("[%0t] perf %s len %2d (cfg %h): %0.1f clk/xfer (%0.1f ns), req->done %0.1f clk",
                     $time, is_we ? "WR" : "RD", n, cr0_active, c_tot * 1.0 / reps,
                     c_tot * 18.52 / reps, c_lat * 1.0 / reps);
        end
    endtask

    integer clkcnt = 0;
    always @(posedge clk) clkcnt = clkcnt + 1;

    // The device wraps linear bursts inside its 1 KB page (256 words);
    // hyperram_ports never issues a burst across one (bursts are aligned to
    // their own size), so neither does this testbench.
    function automatic [ADDR_WIDTH-1:0] in_page(input [ADDR_WIDTH-1:0] a, input integer l);
        begin
            if ((a[7:0] + l) > 256) in_page = {a[ADDR_WIDTH-1:8], 8'(256 - l)};
            else                    in_page = a;
        end
    endfunction

    task automatic random_phase(input integer n);
        integer t, l, r;
        logic [ADDR_WIDTH-1:0] a;
        begin
            for (t = 0; t < n; t = t + 1) begin
                l = ($random(seed) & 15) + 1;
                r = $random(seed) & 7;
                if (r < 5)
                    a = $random(seed) & 12'hFFF;
                else
                    a = ($random(seed) & 21'h1FFFFF) % (CAL_ADDR - 16);
                a = in_page(a, l);
                if ($random(seed) & 1) do_write(a, l);
                else                   do_read(a, l);
            end
        end
    endtask

    // ---------------------------------------------------------------------
    // Main
    // ---------------------------------------------------------------------
    integer i;
    initial begin
        if (!$value$plusargs("SEED=%d", seed))     seed = 1;
        if (!$value$plusargs("NTRANS=%d", ntrans)) ntrans = 450;
        trace = $test$plusargs("TRACE");
        cold  = $test$plusargs("XC_COLD");
        for (i = 0; i < (1 << ADDR_WIDTH); i = i + 1) shadow[i] = 32'hFFFF_FFFF;
        rd_cnt = 0;

        repeat (8) @(posedge clk);
        rst_n = 1'b1;

        wait (init_done);
        @(posedge clk);
        $display("[%0t] init_done  board delay %0d ps  cal_ok=%0d  cal_map=%b  tap=%0d  arrival=%0d.%0d  proto_xccela=%0d dev_id=%h det_fail=%0d resets=%0d ignored=%0d",
                 $time, u_h.dly_ps, cal_ok, cal_map, dly_code, rd_arrival[4:1], rd_arrival[0] ? 5 : 0,
                 proto_xccela, dev_id, det_fail, u_h.u_xram.reset_count, u_h.u_xram.ignored_cmds);
        if (!cal_ok) fail("calibration failed");
        if (proto_xccela !== 1'b1) fail("proto_xccela != 1");
        if (det_fail !== 1'b0)     fail("det_fail set");
        if (dev_id !== 16'h930D)   fail($sformatf("dev_id %h != {MR2,MR1} 930D", dev_id));
        if (cold) begin
            if (u_h.u_xram.reset_count != 1) fail("cold: expected exactly one Global Reset");
            if (u_h.u_xram.ignored_cmds != 4) fail($sformatf("cold: %0d ignored commands (expected 4: the first probe round)", u_h.u_xram.ignored_cmds));
        end else begin
            if (u_h.u_xram.reset_count != 0) fail("warm: unexpected Global Reset");
            if (u_h.u_xram.ignored_cmds != 0) fail($sformatf("warm: %0d ignored commands", u_h.u_xram.ignored_cmds));
        end
        if (u_h.u_xram.mr0 !== cr0_active[7:0] || u_h.u_xram.mr4 !== cr0_active[15:8])
            fail($sformatf("boot MR0/MR4 model %h/%h != active %h", u_h.u_xram.mr0, u_h.u_xram.mr4, cr0_active));

        // Directed: exact write placement in every latency setting
        set_mr(5, 5, 1'b1);  placement_check(21'h1000, 3);
        set_mr(4, 4, 1'b1);  placement_check(21'h1010, 3);
        set_mr(3, 3, 1'b0);  placement_check(21'h1020, 3);
        set_mr(4, 5, 1'b0);  placement_check(21'h1030, 3);
        set_mr(5, 4, 1'b0);  placement_check(21'h1040, 3);
        set_mr(5, 5, 1'b0);  placement_check(21'h1050, 3);

        random_phase(ntrans);  collision_hunt(3);
        set_mr(4, 4, 1'b0);  random_phase(ntrans);  collision_hunt(3);
        set_mr(3, 3, 1'b0);  random_phase(ntrans / 2);  collision_hunt(2);
        set_mr(5, 5, 1'b1);  random_phase(ntrans / 2);
        set_mr(4, 5, 1'b0);  random_phase(ntrans);
        set_mr(5, 5, 1'b0);  random_phase(ntrans / 2);

        for (i = 0; i < 4096; i = i + 16) do_read(i, 16);

        perf(1'b0, 1, 64);  perf(1'b1, 1, 64);
        perf(1'b0, 4, 64);  perf(1'b1, 4, 64);
        perf(1'b0, 16, 32); perf(1'b1, 16, 32);
        set_mr(4, 4, 1'b0);
        perf(1'b0, 1, 64);  perf(1'b1, 1, 64);
        perf(1'b0, 4, 64);  perf(1'b1, 4, 64);

        repeat (20) @(posedge clk);
        if (stat_rd_timeouts != 0)  fail($sformatf("%0d read timeouts", stat_rd_timeouts));
        if (stat_lat_mismatch != 0) fail($sformatf("%0d latency sample mismatches", stat_lat_mismatch));
        if (n_rd2x_var == 0)        fail("no pushed-out (late) read seen");
        if (timing_errors != 0)     fail($sformatf("%0d pin timing violations", timing_errors));
        if (contention_errors != 0) fail($sformatf("%0d bus contention events", contention_errors));
        if (u_h.u_xram.undef_inst != 0) fail($sformatf("%0d undefined instructions", u_h.u_xram.undef_inst));
        if (u_h.u_xram.page_wraps != 0) fail($sformatf("%0d linear bursts wrapped inside the 1 KB page", u_h.u_xram.page_wraps));

        $display("[%0t] xfers=%0d (wr %0d rd %0d, %0d words read)  late reads: var %0d fixed %0d/%0d  model pushouts=%0d  stat_lat2x=%0d timeouts=%0d timing=%0d contention=%0d",
                 $time, n_wr + n_rd, n_wr, n_rd, n_words_rd, n_rd2x_var, n_2x_fixed, n_fixed,
                 u_h.u_xram.n_pushout, stat_lat2x, stat_rd_timeouts, timing_errors, contention_errors);
        $display("[%0t] pin timing (ns): min tCSP %0.2f  min tCHD %0.2f  min tCPH %0.2f  min tRC %0.2f  max tCEM %0.1f  min tDS %0.2f  min tDH %0.2f",
                 $time, u_h.u_xram.min_tcsp, u_h.u_xram.min_tchd, u_h.u_xram.min_tcph, u_h.u_xram.min_trc,
                 u_h.u_xram.max_tcem, u_h.u_xram.min_tds, u_h.u_xram.min_tdh);
        if (errors == 0)
            $display("XCCELA_CTRL PASS dly=%0dps tap=%0d cal_map=%h arrival=%0d.%0d xfers=%0d pushouts=%0d%s",
                     u_h.dly_ps, dly_code, cal_map, rd_arrival[4:1], rd_arrival[0] ? 5 : 0,
                     n_wr + n_rd, u_h.u_xram.n_pushout, cold ? " cold" : "");
        else
            $display("XCCELA_CTRL FAIL dly=%0dps errors=%0d", u_h.dly_ps, errors);
        $finish;
    end

    initial begin
        #20_000_000;
        $display("XCCELA_CTRL FAIL (watchdog)");
        $finish;
    end

endmodule
