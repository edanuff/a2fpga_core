//
// hyperram_bist testbench — `make hyperram_bist`
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES.
//
// hyperram_bist -> hyperram_ctrl -> Gowin primitives -> board delay ->
// S27KL0642 model (test_hyperram_harness). Everything is driven through
// the BIST register interface, as the ESP32 would.
//
//   1. PRBS32, random burst lengths + random masked-overwrite pass,
//      2 passes                                       -> zero errors
//   2. every pattern, single pass, fixed 16-word bursts -> zero errors
//   3. continuous mode + STOP                          -> stopped, passes>0,
//                                                         zero errors
//   4. CR0 write (latency 7) + RECAL through the BIST  -> CR0 read back,
//      cal OK, pass with zero errors
//   5. eye scan (step 8, 4 x 16-word reads per tap)    -> table agrees with
//      the controller's cal_map, clean at the calibrated tap
//   6. injection: DLY_FORCE to a failing tap, read-only verify
//                                                      -> errors detected
//      release, read-only verify                       -> zero errors
//   7. injection: XOR 0x04 on the RAM->FPGA DQ path for a whole
//      read-only pass                                  -> every word wrong,
//      err_bits = 4 x words, first error diff = 04040404
//   8. client pass-through write/read while the BIST is idle
//

`timescale 1ns / 1ps

module test_hyperram_bist;

    localparam ADDR_WIDTH = 21;

    wire        clk, clk_mem, clk_mem_90;
    wire        hr_ck, hr_cs_n;
    wire [7:0]  hr_dq;
    wire        hr_rwds;
    wire [7:0]  h_dq_in;
    wire        h_rwds_in;
    wire [7:0]  f_dq_drv;
    wire        f_rwds_drv;
    integer     timing_errors, contention_errors;
    logic [7:0] inj_xor = 8'h00;

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
        .inj_xor           (inj_xor),
        .timing_errors     (timing_errors),
        .contention_errors (contention_errors)
    );

    assign hr_dq   = h_dq_in;
    assign hr_rwds = h_rwds_in;

    genvar gi;
    generate
        for (gi = 0; gi < 8; gi = gi + 1) begin : g_drv
            assign f_dq_drv[gi] = u_ctl.u_phy.dq_tri[gi] ? 1'bz : u_ctl.u_phy.dq_out[gi];
        end
    endgenerate
    assign f_rwds_drv = u_ctl.u_phy.rwds_tri ? 1'bz : u_ctl.u_phy.rwds_out;

    logic rst_n = 1'b0;

    // ---- controller <-> BIST ----
    wire                  m_ready, m_init_done, m_req, m_we, m_wbuf_we;
    wire [ADDR_WIDTH-1:0] m_addr;
    wire [4:0]            m_len;
    wire [31:0]           m_wdata, m_wbuf_data, m_rdata;
    wire [3:0]            m_wbe, m_wbuf_idx, m_wbuf_be;
    wire                  m_rvalid, m_done, m_rd_err, m_xfer_lat2x;
    wire                  m_recal, m_cfg_cr0_write, m_dly_force, m_cal_ok;
    wire [15:0]           m_cfg_cr0, m_cr0_active;
    wire [7:0]            m_dly_force_code, m_dly_code, m_cal_tap;
    wire [31:0]           m_cal_map, m_stat_xfers, m_stat_lat2x, m_stat_rd_timeouts, m_stat_lat_mismatch;
    wire [4:0]            m_rd_arrival;

    hyperram_ctrl #(
        .CLK_HZ     (54_000_000),
        .ADDR_WIDTH (ADDR_WIDTH),
        .LATENCY    (6)
    ) u_ctl (
        .clk               (clk),
        .clk_mem           (clk_mem),
        .clk_mem_90        (clk_mem_90),
        .clk_cap           (clk_mem),
        .rst_n             (rst_n),
        .ready             (m_ready),
        .init_done         (m_init_done),
        .req               (m_req),
        .we                (m_we),
        .addr              (m_addr),
        .len               (m_len),
        .wdata             (m_wdata),
        .wbe               (m_wbe),
        .wbuf_we           (m_wbuf_we),
        .wbuf_idx          (m_wbuf_idx),
        .wbuf_data         (m_wbuf_data),
        .wbuf_be           (m_wbuf_be),
        .rvalid            (m_rvalid),
        .rdata             (m_rdata),
        .done              (m_done),
        .rd_err            (m_rd_err),
        .xfer_lat2x        (m_xfer_lat2x),
        .recal             (m_recal),
        .cfg_cr0           (m_cfg_cr0),
        .cfg_cr0_write     (m_cfg_cr0_write),
        .cr0_active        (m_cr0_active),
        .dly_force         (m_dly_force),
        .dly_force_code    (m_dly_force_code),
        .dly_code          (m_dly_code),
        .cal_tap           (m_cal_tap),
        .cal_map           (m_cal_map),
        .cal_ok            (m_cal_ok),
        .rd_arrival        (m_rd_arrival),
        .stat_xfers        (m_stat_xfers),
        .stat_lat2x        (m_stat_lat2x),
        .stat_rd_timeouts  (m_stat_rd_timeouts),
        .stat_lat_mismatch (m_stat_lat_mismatch),
        .hr_ck             (hr_ck),
        .hr_cs_n           (hr_cs_n),
        .hr_rwds           (hr_rwds),
        .hr_dq             (hr_dq)
    );

    // ---- BIST + client ----
    logic [7:0]  reg_addr = 8'd0;
    logic [31:0] reg_wdata = 32'd0;
    logic        reg_we = 1'b0;
    wire  [31:0] reg_rdata;
    wire         bist_active;

    logic                  u_req = 1'b0, u_we = 1'b0, u_wbuf_we = 1'b0;
    logic [ADDR_WIDTH-1:0] u_addr = '0;
    logic [4:0]            u_len = 5'd1;
    logic [31:0]           u_wdata = '0, u_wbuf_data = '0;
    logic [3:0]            u_wbe = 4'hF, u_wbuf_idx = '0, u_wbuf_be = 4'hF;
    wire                   u_ready, u_rvalid, u_done, u_rd_err;
    wire  [31:0]           u_rdata;

    hyperram_bist #(.ADDR_WIDTH(ADDR_WIDTH)) u_bist (
        .clk                 (clk),
        .rst_n               (rst_n),
        .reg_addr            (reg_addr),
        .reg_wdata           (reg_wdata),
        .reg_we              (reg_we),
        .reg_rdata           (reg_rdata),
        .active              (bist_active),
        .u_ready             (u_ready),
        .u_req               (u_req),
        .u_we                (u_we),
        .u_addr              (u_addr),
        .u_len               (u_len),
        .u_wdata             (u_wdata),
        .u_wbe               (u_wbe),
        .u_wbuf_we           (u_wbuf_we),
        .u_wbuf_idx          (u_wbuf_idx),
        .u_wbuf_data         (u_wbuf_data),
        .u_wbuf_be           (u_wbuf_be),
        .u_rvalid            (u_rvalid),
        .u_rdata             (u_rdata),
        .u_done              (u_done),
        .u_rd_err            (u_rd_err),
        .m_ready             (m_ready),
        .m_init_done         (m_init_done),
        .m_req               (m_req),
        .m_we                (m_we),
        .m_addr              (m_addr),
        .m_len               (m_len),
        .m_wdata             (m_wdata),
        .m_wbe               (m_wbe),
        .m_wbuf_we           (m_wbuf_we),
        .m_wbuf_idx          (m_wbuf_idx),
        .m_wbuf_data         (m_wbuf_data),
        .m_wbuf_be           (m_wbuf_be),
        .m_rvalid            (m_rvalid),
        .m_rdata             (m_rdata),
        .m_done              (m_done),
        .m_rd_err            (m_rd_err),
        .m_xfer_lat2x        (m_xfer_lat2x),
        .m_recal             (m_recal),
        .m_cfg_cr0           (m_cfg_cr0),
        .m_cfg_cr0_write     (m_cfg_cr0_write),
        .m_dly_force         (m_dly_force),
        .m_dly_force_code    (m_dly_force_code),
        .m_cr0_active        (m_cr0_active),
        .m_dly_code          (m_dly_code),
        .m_cal_tap           (m_cal_tap),
        .m_cal_map           (m_cal_map),
        .m_cal_ok            (m_cal_ok),
        .m_rd_arrival        (m_rd_arrival),
        .m_stat_xfers        (m_stat_xfers),
        .m_stat_lat2x        (m_stat_lat2x),
        .m_stat_rd_timeouts  (m_stat_rd_timeouts),
        .m_stat_lat_mismatch (m_stat_lat_mismatch)
    );

    // ---------------------------------------------------------------------
    // Register access (as the ESP32 bridge would)
    // ---------------------------------------------------------------------
    localparam [7:0] R_ID = 8'h00, R_CTRL = 8'h01, R_STATUS = 8'h02, R_MODE = 8'h03,
                     R_START = 8'h04, R_LENGTH = 8'h05, R_PASSES = 8'h06, R_SEED = 8'h07,
                     R_EYE_CFG = 8'h08, R_CR0 = 8'h09, R_DLY_FORCE = 8'h0A,
                     R_WORDS_WR = 8'h10, R_WORDS_RD = 8'h11, R_ERR_WORDS = 8'h12,
                     R_ERR_BITS = 8'h13, R_FIRST_ADDR = 8'h14, R_FIRST_EXP = 8'h15,
                     R_FIRST_ACT = 8'h16, R_LAST_ADDR = 8'h17, R_XFERS = 8'h18,
                     R_LAT2X = 8'h19, R_RD_TMO = 8'h1A, R_PASS_CNT = 8'h1B,
                     R_CAL_MAP = 8'h20, R_CAL_INFO = 8'h21, R_CR0_ACT = 8'h22,
                     R_CTL_MISM = 8'h26, R_EYE_IDX = 8'h30, R_EYE_DATA = 8'h31;

    integer errors = 0;

    task automatic fail(input string msg);
        begin
            errors = errors + 1;
            $display("[%0t] ERROR %s", $time, msg);
        end
    endtask

    task automatic wr(input [7:0] a, input [31:0] d);
        begin
            @(posedge clk);
            reg_addr  <= a;
            reg_wdata <= d;
            reg_we    <= 1'b1;
            @(posedge clk);
            reg_we    <= 1'b0;
        end
    endtask

    task automatic rd(input [7:0] a, output [31:0] d);
        begin
            @(posedge clk);
            reg_addr <= a;
            repeat (3) @(posedge clk);
            d = reg_rdata;
        end
    endtask

    task automatic wait_idle(input integer max_us);
        logic [31:0] st;
        integer t;
        begin
            t = 0;
            rd(R_STATUS, st);
            while (st[0] && t < max_us) begin
                #1000;
                t = t + 1;
                rd(R_STATUS, st);
            end
            if (st[0]) begin
                fail("BIST did not finish");
                $display("HYPERRAM_BIST FAIL (hang)");
                $finish;
            end
        end
    endtask

    logic [31:0] v_wr, v_rd, v_err, v_bits, v_x, v_l2, v_tmo, v_pass, v_fa, v_fe, v_fx;

    task automatic snap();
        begin
            rd(R_WORDS_WR, v_wr);   rd(R_WORDS_RD, v_rd);
            rd(R_ERR_WORDS, v_err); rd(R_ERR_BITS, v_bits);
            rd(R_XFERS, v_x);       rd(R_LAT2X, v_l2);
            rd(R_RD_TMO, v_tmo);    rd(R_PASS_CNT, v_pass);
            rd(R_FIRST_ADDR, v_fa); rd(R_FIRST_EXP, v_fe); rd(R_FIRST_ACT, v_fx);
        end
    endtask

    task automatic show(input string name);
        begin
            snap();
            $display("[%0t] %-28s wr=%0d rd=%0d err_words=%0d err_bits=%0d xfers=%0d lat2x=%0d tmo=%0d passes=%0d first=%s%h exp=%h act=%h",
                     $time, name, v_wr, v_rd, v_err, v_bits, v_x, v_l2, v_tmo, v_pass,
                     v_fa[31] ? "@" : "-", v_fa[30:0], v_fe, v_fx);
        end
    endtask

    task automatic run(input [31:0] mode, input [31:0] start, input [31:0] length,
                       input [31:0] passes, input [31:0] seed);
        begin
            wr(R_CTRL, 32'h4);              // CLEAR
            wr(R_MODE, mode);
            wr(R_START, start);
            wr(R_LENGTH, length);
            wr(R_PASSES, passes);
            wr(R_SEED, seed);
            wr(R_CTRL, 32'h1);              // START
            wait_idle(20000);
            snap();
        end
    endtask

    function automatic [31:0] mk_mode(input [2:0] pat, input [1:0] run, input rlen,
                                      input rmask, input eye, input ro, input invalt,
                                      input [4:0] burst);
        mk_mode = {12'd0, 4'(burst - 5'd1), 5'd0, invalt, ro, eye, rmask, rlen, run, 1'b0, pat};
    endfunction

    function automatic [15:0] mk_cr0(input integer lat, input logic fixed);
        logic [3:0] code;
        begin
            code = (lat == 5) ? 4'b0000 : (lat == 6) ? 4'b0001 : 4'b0010;
            mk_cr0 = {1'b1, 3'b000, 4'b1111, code, fixed, 1'b1, 2'b11};
        end
    endfunction

    // ---------------------------------------------------------------------
    // Main
    // ---------------------------------------------------------------------
    integer      i, pat, n_fail_taps, n_clean_taps, disagree, bad_tap;
    logic [31:0] v, st, cal_map, cal_info, ent;
    integer      seed;

    initial begin
        if (!$value$plusargs("SEED=%d", seed)) seed = 1;
        repeat (8) @(posedge clk);
        rst_n = 1'b1;
        wait (m_init_done);
        rd(R_ID, v);
        if (v !== 32'h48524231) fail($sformatf("ID %h", v));
        rd(R_CAL_MAP, cal_map);
        rd(R_CAL_INFO, cal_info);
        $display("[%0t] init_done  board delay %0d ps  cal_ok=%0d  cal_map=%h  tap=%0d  arrival=%0d.%0d",
                 $time, u_h.dly_ps, cal_info[21], cal_map, cal_info[15:8],
                 cal_info[20:17], cal_info[16] ? 5 : 0);
        if (!cal_info[21]) fail("calibration failed");

        // 1. PRBS32, random lengths, random masked overwrite, 2 passes
        run(mk_mode(3'd1, 2'd1, 1'b1, 1'b1, 1'b0, 1'b0, 1'b0, 5'd16),
            32'h0001_0000 + (seed & 32'hFF), 2048, 2, seed);
        show("1 PRBS rlen rmask x2");
        if (v_err != 0) fail("test 1 errors");
        if (v_wr != 2 * 2 * 2048 || v_rd != 2 * 2048 || v_pass != 2)
            fail($sformatf("test 1 counts wr %0d rd %0d passes %0d", v_wr, v_rd, v_pass));

        // 2. every pattern
        for (pat = 0; pat < 8; pat = pat + 1) begin
            run(mk_mode(pat[2:0], 2'd0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 5'd16),
                32'h0002_0000, 512, 1, seed + pat);
            show($sformatf("2 pattern %0d", pat));
            if (v_err != 0 || v_rd != 512) fail($sformatf("test 2 pattern %0d", pat));
        end

        // 3. continuous + STOP (INV_ALT, addr-in-addr, random lengths)
        wr(R_CTRL, 32'h4);
        wr(R_MODE, mk_mode(3'd0, 2'd2, 1'b1, 1'b0, 1'b0, 1'b0, 1'b1, 5'd16));
        wr(R_START, 32'h0003_0000);
        wr(R_LENGTH, 256);
        wr(R_CTRL, 32'h1);
        #60000;
        wr(R_CTRL, 32'h2);                  // STOP
        wait_idle(1000);
        rd(R_STATUS, st);
        show("3 continuous + STOP");
        if (!st[5] || !st[1]) fail($sformatf("test 3 status %h", st));
        if (v_pass < 2 || v_err != 0) fail("test 3 passes / errors");

        // 4. CR0 (latency 7) + RECAL through the BIST, then a pass
        wr(R_CR0, mk_cr0(7, 1'b0));
        wr(R_CTRL, 32'h10);                 // CR0_WRITE
        repeat (40) @(posedge clk);
        rd(R_CR0_ACT, v);
        if (v[15:0] !== mk_cr0(7, 1'b0) || u_h.u_ram.Config_reg0 !== mk_cr0(7, 1'b0))
            fail($sformatf("test 4 CR0 %h model %h", v, u_h.u_ram.Config_reg0));
        wr(R_CTRL, 32'h8);                  // RECAL
        repeat (40) @(posedge clk);
        wait (m_init_done);
        rd(R_CAL_INFO, cal_info);
        rd(R_CAL_MAP, cal_map);
        $display("[%0t] recal at latency 7: cal_ok=%0d cal_map=%h tap=%0d", $time,
                 cal_info[21], cal_map, cal_info[15:8]);
        if (!cal_info[21]) fail("test 4 recal");
        run(mk_mode(3'd1, 2'd0, 1'b1, 1'b1, 1'b0, 1'b0, 1'b0, 5'd16), 32'h0004_0000, 1024, 1, seed);
        show("4 L7 PRBS rlen rmask");
        if (v_err != 0) fail("test 4 errors");

        // 5. eye scan: step 8, 4 reads of 16 words per tap, over 64 words
        wr(R_EYE_CFG, {16'd4, 8'd0, 8'd8});
        run(mk_mode(3'd1, 2'd0, 1'b0, 1'b0, 1'b1, 1'b0, 1'b0, 5'd16), 32'h0005_0000, 64, 1, seed);
        rd(R_STATUS, st);
        if (st[31:16] != 32) fail($sformatf("test 5 eye entries %0d", st[31:16]));
        n_fail_taps = 0; n_clean_taps = 0; disagree = 0; bad_tap = -1;
        for (i = 0; i < 32; i = i + 1) begin
            wr(R_EYE_IDX, i);
            rd(R_EYE_DATA, ent);
            $display("[%0t]   eye tap %3d : %0d error words %s", $time, ent[31:24], ent[23:0],
                     cal_map[i] ? "" : "(cal fail)");
            if (ent[31:24] != i * 8) fail($sformatf("eye entry %0d tap %0d", i, ent[31:24]));
            if (ent[23:0] != 0) begin
                n_fail_taps = n_fail_taps + 1;
                if (bad_tap < 0 || ent[23:0] > 16) bad_tap = ent[31:24];
            end else n_clean_taps = n_clean_taps + 1;
            if ((ent[23:0] == 0) != cal_map[i]) disagree = disagree + 1;
            if (ent[31:24] == {cal_info[15:11], 3'b000} && ent[23:0] != 0)
                fail("eye: errors at the calibrated tap");
        end
        $display("[%0t] eye: %0d clean / %0d failing taps, %0d disagree with cal_map", $time,
                 n_clean_taps, n_fail_taps, disagree);
        if (n_clean_taps == 0) fail("eye: no clean taps");
        if (disagree > 2) fail("eye scan disagrees with cal_map");
        rd(R_CAL_INFO, v);
        if (v[7:0] != v[15:8]) fail($sformatf("delay not restored after eye scan: %0d vs cal %0d", v[7:0], v[15:8]));

        // 6. injection: forced bad tap
        if (bad_tap < 0) begin
            $display("[%0t] (no failing tap at this board delay; skipping forced-tap injection)", $time);
        end else begin
            run(mk_mode(3'd1, 2'd0, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0, 5'd16), 32'h0006_0000, 256, 1, seed);
            if (v_err != 0) fail("test 6 setup errors");
            wr(R_DLY_FORCE, 32'h100 | bad_tap);
            run(mk_mode(3'd1, 2'd0, 1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 5'd16), 32'h0006_0000, 256, 1, seed);
            show($sformatf("6 forced tap %0d (RO)", bad_tap));
            if (v_err == 0 || !v_fa[31]) fail("test 6: forced bad tap not detected");
            wr(R_DLY_FORCE, 32'h0);
            run(mk_mode(3'd1, 2'd0, 1'b0, 1'b0, 1'b0, 1'b1, 1'b0, 5'd16), 32'h0006_0000, 256, 1, seed);
            show("6 released (RO)");
            if (v_err != 0) fail("test 6: errors after releasing the forced tap");
        end

        // 7. injection: XOR 0x04 on the read path for a whole RO pass
        run(mk_mode(3'd4, 2'd0, 1'b1, 1'b0, 1'b0, 1'b0, 1'b0, 5'd16), 32'h0007_0000, 300, 1, seed);
        if (v_err != 0) fail("test 7 setup errors");
        inj_xor = 8'h04;
        run(mk_mode(3'd4, 2'd0, 1'b1, 1'b0, 1'b0, 1'b1, 1'b0, 5'd16), 32'h0007_0000, 300, 1, seed);
        inj_xor = 8'h00;
        show("7 inject xor 04 (RO)");
        if (v_err != 300 || v_bits != 4 * 300 || (v_fe ^ v_fx) != 32'h04040404 ||
            v_fa[30:0] != 32'h0007_0000)
            fail("test 7: injected corruption not reported exactly");
        run(mk_mode(3'd4, 2'd0, 1'b1, 1'b0, 1'b0, 1'b1, 1'b0, 5'd16), 32'h0007_0000, 300, 1, seed);
        if (v_err != 0) fail("test 7: errors after injection removed");

        // 8. client pass-through while idle
        while (!u_ready) @(posedge clk);
        u_req <= 1'b1; u_we <= 1'b1; u_addr <= 21'h8000; u_len <= 5'd1;
        u_wdata <= 32'hCAFEF00D; u_wbe <= 4'hF;
        @(posedge clk);
        u_req <= 1'b0;
        while (!u_done) @(posedge clk);
        while (!u_ready) @(posedge clk);
        u_req <= 1'b1; u_we <= 1'b0;
        @(posedge clk);
        u_req <= 1'b0;
        while (!u_rvalid) @(posedge clk);
        if (u_rdata !== 32'hCAFEF00D) fail($sformatf("client read %h", u_rdata));
        while (!u_done) @(posedge clk);

        rd(R_CTL_MISM, v);
        if (v != 0) fail($sformatf("%0d controller latency mismatches", v));
        if (timing_errors != 0)     fail($sformatf("%0d pin timing violations", timing_errors));
        if (contention_errors != 0) fail($sformatf("%0d contention events", contention_errors));

        if (errors == 0) $display("HYPERRAM_BIST PASS dly=%0dps", u_h.dly_ps);
        else             $display("HYPERRAM_BIST FAIL dly=%0dps errors=%0d", u_h.dly_ps, errors);
        $finish;
    end

    initial begin
        #60_000_000;
        $display("HYPERRAM_BIST FAIL (watchdog)");
        $finish;
    end

endmodule
