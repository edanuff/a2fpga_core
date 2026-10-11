//
// hyperram_ports (mem_port_if arbiter) against hyperram_ctrl + the Infineon
// model. Three clients with the a2mega port roles:
//   port 0: single-word read/write with byte enables (shadow / DOC style)
//   port 1: WIDE_WR_PORT — 128-bit (4-word) writes (framebuffer write port)
//   port 2: burst reads of 8 words (framebuffer read port), non-burst reads
// Random traffic on all three, checked against a software shadow of the
// memory. Also checks the client contract: two requests may be queued per
// port (available drops only when both slots are full), ready pulses once
// per read beat with the word on q, writes complete silently.
//
// Run: make -C boards/a2p25/tests hyperram_ports
//
`timescale 1ns / 1ps
module test_hyperram_ports;
    localparam ADDR_WIDTH = 21;
    localparam NUM_PORTS  = 3;
    localparam [ADDR_WIDTH-1:0] BASE0 = 21'h080000;   // "shadow"
    localparam [ADDR_WIDTH-1:0] BASE1 = 21'h000000;   // "framebuffer" write
    localparam [ADDR_WIDTH-1:0] BASE2 = 21'h000000;   // "framebuffer" read

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
        .clk (clk), .clk_mem (clk_mem), .clk_mem_90 (clk_mem_90),
        .hr_ck (hr_ck), .hr_cs_n (hr_cs_n),
        .f_dq_drv (f_dq_drv), .f_rwds_drv (f_rwds_drv),
        .hr_dq_in (h_dq_in), .hr_rwds_in (h_rwds_in),
        .inj_xor (8'h00),
        .timing_errors (timing_errors), .contention_errors (contention_errors)
    );
    assign hr_dq   = h_dq_in;
    assign hr_rwds = h_rwds_in;
    genvar gi;
    generate
        for (gi = 0; gi < 8; gi = gi + 1) begin : g_drv
            assign f_dq_drv[gi] = u_ctrl.u_phy.dq_tri[gi] ? 1'bz : u_ctrl.u_phy.dq_out[gi];
        end
    endgenerate
    assign f_rwds_drv = u_ctrl.u_phy.rwds_tri ? 1'bz : u_ctrl.u_phy.rwds_out;

    logic rst_n = 1'b0;

    // controller <-> arbiter
    wire        hr_ready, hr_init_done, hr_req, hr_we, hr_rvalid, hr_done;
    wire [ADDR_WIDTH-1:0] hr_addr;
    wire [4:0]  hr_len;
    wire [31:0] hr_wdata, hr_wbuf_data, hr_rdata;
    wire [3:0]  hr_wbe, hr_wbuf_idx, hr_wbuf_be;
    wire        hr_wbuf_we;

    hyperram_ctrl #(.CLK_HZ(54_000_000), .ADDR_WIDTH(ADDR_WIDTH), .LATENCY(6)) u_ctrl (
        .clk (clk), .clk_mem (clk_mem), .clk_mem_90 (clk_mem_90), .clk_cap (clk_mem),
        .rst_n (rst_n),
        .ready (hr_ready), .init_done (hr_init_done),
        .req (hr_req), .we (hr_we), .addr (hr_addr), .len (hr_len),
        .wdata (hr_wdata), .wbe (hr_wbe),
        .wbuf_we (hr_wbuf_we), .wbuf_idx (hr_wbuf_idx), .wbuf_data (hr_wbuf_data), .wbuf_be (hr_wbuf_be),
        .rvalid (hr_rvalid), .rdata (hr_rdata), .done (hr_done),
        .rd_err (), .xfer_lat2x (),
        .recal (1'b0), .cfg_cr0 (16'd0), .cfg_cr0_write (1'b0), .cr0_active (),
        .dly_force (1'b0), .dly_force_code (8'd0), .dly_code (), .cal_tap (), .cal_map (),
        .cal_ok (), .rd_arrival (),
        .stat_xfers (), .stat_lat2x (), .stat_rd_timeouts (), .stat_lat_mismatch (),
        .hr_ck (hr_ck), .hr_cs_n (hr_cs_n), .hr_rwds (hr_rwds), .hr_dq (hr_dq)
    );

    // flat client vectors (field p = port p)
    logic [NUM_PORTS-1:0]            c_rd = '0, c_wr = '0, c_burst = '0;
    logic [NUM_PORTS*ADDR_WIDTH-1:0] c_addr = '0;
    logic [NUM_PORTS*32-1:0]         c_data = '0;
    logic [NUM_PORTS*4-1:0]          c_be = {NUM_PORTS{4'hF}};
    logic [95:0] wide_hi = '0;
    wire  [NUM_PORTS-1:0] c_available, c_ready;
    wire  [31:0] c_q;

    hyperram_ports #(
        .NUM_PORTS (NUM_PORTS),
        .PORT_BASE_ADDR ({BASE2, BASE1, BASE0}),
        .PORT_BURST_WORDS ({5'd8, 5'd4, 5'd4}),
        .WIDE_WR_PORT (1)
    ) u_dut (
        .clk (clk), .rst_n (rst_n),
        .c_rd (c_rd), .c_wr (c_wr), .c_burst (c_burst), .c_addr (c_addr),
        .c_data (c_data), .c_be (c_be), .wide_wr_data_hi (wide_hi),
        .c_available (c_available), .c_ready (c_ready), .c_q (c_q),
        .hr_ready (hr_ready), .hr_init_done (hr_init_done),
        .hr_req (hr_req), .hr_we (hr_we), .hr_addr (hr_addr), .hr_len (hr_len),
        .hr_wdata (hr_wdata), .hr_wbe (hr_wbe),
        .hr_wbuf_we (hr_wbuf_we), .hr_wbuf_idx (hr_wbuf_idx), .hr_wbuf_data (hr_wbuf_data), .hr_wbuf_be (hr_wbuf_be),
        .hr_rvalid (hr_rvalid), .hr_rdata (hr_rdata), .hr_done (hr_done),
        .dbg_req_pending (), .dbg_state ()
    );
    wire avail0 = c_available[0], avail1 = c_available[1], avail2 = c_available[2];
    wire ready0 = c_ready[0], ready1 = c_ready[1], ready2 = c_ready[2];
    wire [31:0] q0 = c_q, q2 = c_q;

    // software shadow of the whole device (word addressed)
    bit [31:0] shadow [0:(1<<ADDR_WIDTH)-1];
    bit        written [0:(1<<ADDR_WIDTH)-1];
    integer n_unchecked = 0;
    integer errors = 0, n_wr = 0, n_wide = 0, n_rd = 0, n_burst = 0, n_beats = 0;
    integer seed = 7;

    // +trace: every client fire, controller request and read beat
    logic trace = 1'b0;
    initial if ($test$plusargs("trace")) trace = 1'b1;
    always @(posedge clk) if (trace) begin
        for (int pp = 0; pp < NUM_PORTS; pp = pp + 1)
            if (u_dut.fire[pp])
                $display("[%0t] FIRE p%0d %s burst=%0d addr=%0h data=%08x be=%h", $time, pp,
                         c_wr[pp] ? "WR" : "RD", c_burst[pp], c_addr[pp*ADDR_WIDTH +: ADDR_WIDTH], c_data[pp*32 +: 32], c_be[pp*4 +: 4]);
        if (hr_req) $display("[%0t] REQ we=%0d addr=%0h len=%0d wdata=%08x wbe=%h", $time, hr_we, hr_addr, hr_len, hr_wdata, hr_wbe);
        if (hr_rvalid) $display("[%0t] RVALID %08x", $time, hr_rdata);
        if (hr_done) $display("[%0t] DONE  st=%0d act=%0d ptrs p0 w%0d r%0d p1 w%0d r%0d p2 w%0d r%0d", $time, u_dut.state, u_dut.active,
                              u_dut.q_wr_ptr[0], u_dut.q_rd_ptr[0], u_dut.q_wr_ptr[1], u_dut.q_rd_ptr[1], u_dut.q_wr_ptr[2], u_dut.q_rd_ptr[2]);
        if (u_dut.q_pop != 0) $display("[%0t] POP %b", $time, u_dut.q_pop);
        if (c_ready != 0) $display("[%0t] READY %b q=%08x", $time, c_ready, c_q);
    end

    task automatic fail(input string msg);
        errors = errors + 1;
        if (errors <= 20) $display("[%0t] ERROR %s", $time, msg);
    endtask

    // read beat capture per port
    logic [31:0] beats0 [0:15], beats2 [0:15];
    integer      nb0 = 0, nb2 = 0;
    always @(posedge clk) begin
        if (ready0) begin if (nb0 < 16) beats0[nb0] = q0; nb0 = nb0 + 1; end
        if (ready2) begin if (nb2 < 16) beats2[nb2] = q2; nb2 = nb2 + 1; end
        if (ready1) fail("ready pulse on the write-only port");
    end

    task automatic fire(input integer p, input bit wr, input bit burst,
                        input [ADDR_WIDTH-1:0] a, input [31:0] d, input [3:0] be);
        integer guard;
        begin
            guard = 0;
            while (!c_available[p] && guard < 5000) begin
                @(posedge clk); guard = guard + 1;
            end
            if (guard >= 5000) begin fail("port never available"); $finish; end
            @(negedge clk);
            c_wr[p] = wr; c_rd[p] = !wr; c_burst[p] = burst;
            c_addr[p*ADDR_WIDTH +: ADDR_WIDTH] = a; c_data[p*32 +: 32] = d; c_be[p*4 +: 4] = be;
            @(negedge clk);
            c_wr[p] = 0; c_rd[p] = 0;
        end
    endtask

    task automatic wait_idle();
        integer guard;
        begin
            guard = 0;
            // all queues empty and controller back at ready
            while ((u_dut.dbg_req_pending != 0 || u_dut.state != 0) && guard < 20000) begin
                @(posedge clk); guard = guard + 1;
            end
            if (guard >= 20000) begin fail("arbiter hang"); $display("HYPERRAM_PORTS FAIL (hang)"); $finish; end
            repeat (4) @(posedge clk);
        end
    endtask

    function automatic [31:0] merge(input [31:0] old, input [31:0] nw, input [3:0] be);
        merge = {be[3] ? nw[31:24] : old[31:24], be[2] ? nw[23:16] : old[23:16],
                 be[1] ? nw[15:8]  : old[15:8],  be[0] ? nw[7:0]   : old[7:0]};
    endfunction

    integer i, k, t;
    logic [ADDR_WIDTH-1:0] a, abase;
    logic [31:0] d;
    logic [3:0]  be;
    logic [127:0] wide;

    initial begin
        // the model powers up with 0xFF in every byte; byte-enabled writes
        // merge into that, so the shadow must start the same way
        for (i = 0; i < (1<<ADDR_WIDTH); i = i + 1) shadow[i] = 32'hFFFFFFFF;
        #200 rst_n = 1'b1;
        wait (hr_init_done);
        $display("[%0t] init_done", $time);
        repeat (20) @(posedge clk);

        // ---- 1. client contract: two back-to-back fires are accepted, third waits
        @(negedge clk);
        c_wr[0] = 1; c_addr[20:0] = 21'd10; c_data[31:0] = 32'h11111111; c_be[3:0] = 4'hF;
        @(negedge clk);
        if (!avail0) fail("available dropped after a single queued request");
        c_addr[20:0] = 21'd11; c_data[31:0] = 32'h22222222;
        @(negedge clk);
        c_wr[0] = 0;
        @(negedge clk);
        if (avail0) fail("available still high with two requests queued");
        shadow[BASE0 + 10] = 32'h11111111; shadow[BASE0 + 11] = 32'h22222222;
        written[BASE0 + 10] = 1; written[BASE0 + 11] = 1;
        wait_idle();
        n_wr = n_wr + 2;

        // ---- 2. random traffic
        for (t = 0; t < 600; t = t + 1) begin
            case ($urandom(seed) % 5)
                0: begin // port 0 single write with byte enables
                    a = $urandom(seed) % 256; d = $urandom(seed); be = $urandom(seed) % 16;
                    if (be == 0) be = 4'hF;
                    fire(0, 1, 0, a, d, be);
                    shadow[BASE0 + a] = merge(shadow[BASE0 + a], d, be); written[BASE0 + a] = 1;
                    n_wr = n_wr + 1;
                end
                1: begin // port 0 single read
                    a = $urandom(seed) % 256;
                    nb0 = 0;
                    fire(0, 0, 0, a, 32'd0, 4'hF);
                    wait_idle();
                    if (nb0 != 1) fail($sformatf("single read: %0d beats (want 1)", nb0));
                    else if (!written[BASE0 + a]) n_unchecked = n_unchecked + 1;
                    else if (beats0[0] !== shadow[BASE0 + a])
                        fail($sformatf("single read addr %0h: got %08x want %08x", a, beats0[0], shadow[BASE0 + a]));
                    n_rd = n_rd + 1;
                end
                2: begin // port 1 wide write (4 words, aligned)
                    abase = ($urandom(seed) % 512) & ~21'd3;
                    wide = {$urandom(seed), $urandom(seed), $urandom(seed), $urandom(seed)};
                    wide_hi = wide[127:32];
                    fire(1, 1, 0, abase + ($urandom(seed) % 4), wide[31:0], 4'hF);
                    for (k = 0; k < 4; k = k + 1) begin shadow[BASE1 + abase + k] = wide[32*k +: 32]; written[BASE1 + abase + k] = 1; end
                    wait_idle();   // wide_hi is a sideband: keep it stable until consumed
                    n_wide = n_wide + 1;
                end
                3: begin // port 2 burst-8 read (aligned)
                    abase = ($urandom(seed) % 512) & ~21'd7;
                    nb2 = 0;
                    fire(2, 0, 1, abase + ($urandom(seed) % 8), 32'd0, 4'hF);
                    wait_idle();
                    if (nb2 != 8) fail($sformatf("burst read: %0d beats (want 8)", nb2));
                    else for (k = 0; k < 8; k = k + 1)
                        if (!written[BASE2 + abase + k]) n_unchecked = n_unchecked + 1;
                        else if (beats2[k] !== shadow[BASE2 + abase + k])
                            fail($sformatf("burst beat %0d addr %0h: got %08x want %08x", k, abase + k, beats2[k], shadow[BASE2 + abase + k]));
                    n_burst = n_burst + 1; n_beats = n_beats + nb2;
                end
                default: begin // port 2 single read of framebuffer space
                    a = $urandom(seed) % 512;
                    nb2 = 0;
                    fire(2, 0, 0, a, 32'd0, 4'hF);
                    wait_idle();
                    if (nb2 != 1) fail($sformatf("p2 single read: %0d beats (want 1)", nb2));
                    else if (!written[BASE2 + a]) n_unchecked = n_unchecked + 1;
                    else if (beats2[0] !== shadow[BASE2 + a])
                        fail($sformatf("p2 single read addr %0h: got %08x want %08x", a, beats2[0], shadow[BASE2 + a]));
                    n_rd = n_rd + 1;
                end
            endcase
        end

        // ---- 3. contention: all three ports fire together, priority order honoured
        nb0 = 0; nb2 = 0;
        @(negedge clk);
        c_wr[1] = 1; c_addr[41:21] = 21'd512; c_data[63:32] = 32'hCAFE0000; wide_hi = {32'hCAFE0003, 32'hCAFE0002, 32'hCAFE0001};
        c_rd[0] = 1; c_addr[20:0] = 21'd10;
        c_rd[2] = 1; c_burst[2] = 1; c_addr[62:42] = 21'd512;
        @(negedge clk);
        c_wr[1] = 0; c_rd[0] = 0; c_rd[2] = 0;
        wait_idle();
        for (k = 0; k < 4; k = k + 1) begin shadow[BASE1 + 512 + k] = 32'hCAFE0000 + k; written[BASE1 + 512 + k] = 1; end
        if (nb0 != 1 || beats0[0] !== shadow[BASE0 + 10]) fail("contention: port 0 read wrong");
        if (nb2 != 8) fail($sformatf("contention: port 2 burst %0d beats", nb2));
        // port 1's write (priority 1) lands before port 2's burst (priority 2)
        else for (k = 0; k < 4; k = k + 1)
            if (beats2[k] !== (32'hCAFE0000 + k))
                fail($sformatf("contention: priority order (beat %0d = %08x)", k, beats2[k]));

        if (timing_errors != 0 || contention_errors != 0)
            fail($sformatf("pin timing %0d / contention %0d", timing_errors, contention_errors));

        $display("writes %0d wide %0d reads %0d bursts %0d (%0d beats), %0d unwritten words skipped", n_wr, n_wide, n_rd, n_burst, n_beats, n_unchecked);
        if (errors == 0) $display("HYPERRAM_PORTS PASS");
        else             $display("HYPERRAM_PORTS FAIL (%0d errors)", errors);
        $finish;
    end

    initial begin
        #60_000_000;
        $display("HYPERRAM_PORTS FAIL (timeout)");
        $finish;
    end
endmodule
