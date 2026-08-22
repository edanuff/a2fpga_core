// tb_afe_adjust.v
//
// M5 runtime TX-AFE adjustment — unit verification of afe_adjust_seq
// against a mock DRP slave that records every write.
//
// Verifies, in order:
//   1. Cold idle: no DRP activity, train_set_byte = legacy 0x06.
//   2. Training start applies the INIT baseline (VS2/PE0) once: exactly
//      8 writes, per-lane complete, in the sequencing-law order
//      (+0x34 swing, +0x38 C1|CM, +0xd8=0x010 enable, +0xd8=0x110
//      strobe), correct per-lane base addressing, byte -> 0x02.
//   3. An ADJUST_REQUEST equal to the applied levels never re-applies
//      (debounce: unchanged request across iterations = no DRP writes).
//   4. A differing request (VS3/PE2) applies txlev 15 / C1 10 and the
//      reported byte carries MAX_SWING (0x17); repeating the same
//      request applies nothing further.
//   5. PE3 request sets MAX_PE (byte 0x3F, C1 13).
//   6. Requests while training_active=0 are ignored.
//   7. Re-entering training re-baselines (INIT applied once again), then
//      VS1/PE1 maps to txlev 9 / C1 7 (byte 0x09).
//   8. A request arriving while a (slowed) sequence is in flight is not
//      lost and never interleaves: two complete back-to-back 8-write
//      sequences, final applied = the latest request.
//   9. Write-timeout: a sequence whose ready never comes completes by
//      timeout and raises the sticky seq_err debug bit.
//  10. ENABLE_AFE_ADJUST=0 twin fed the same stimuli: zero DRP activity,
//      drp_req never asserts, train_set_byte constant 0x06 throughout.
//
// Run: iverilog -g2012 -o /tmp/tb_afe_adjust.vvp \
//        hdl/displayport/sim/tb_afe_adjust.v \
//        hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_afe_adjust.vvp

`timescale 1ns/1ps

module tb_afe_adjust;

    // async clocks: 100 MHz mgmt, ~122 MHz DRP
    reg mgmt_clk = 1'b0;
    reg drp_clk  = 1'b0;
    always #5    mgmt_clk = ~mgmt_clk;
    always #4.1  drp_clk  = ~drp_clk;

    // stimuli
    reg  [1:0] vs_request      = 2'd0;
    reg  [1:0] pe_request      = 2'd0;
    reg        adjust_de       = 1'b0;
    reg        training_active = 1'b0;

    // one ADJUST_REQUEST arrival, as link_signal_mgmt would produce it:
    // the request registers take the new value on the adjust_de cycle
    task send_adjust(input [1:0] vs, input [1:0] pe);
        begin
            @(negedge mgmt_clk);
            vs_request = vs;
            pe_request = pe;
            adjust_de  = 1'b1;
            @(negedge mgmt_clk);
            adjust_de  = 1'b0;
        end
    endtask

    // DUT (enabled)
    wire [7:0]  train_set_byte;
    wire        afe_busy;
    wire [5:0]  dbg_afe;
    wire        drp_req;
    reg         drp_gnt = 1'b0;
    wire [23:0] drp_addr;
    wire [31:0] drp_wrdata;
    wire        drp_wren;
    reg         drp_ready = 1'b0;

    afe_adjust_seq #(
        .ENABLE_AFE_ADJUST (1),
        .NUM_LANES         (2),
        .LANE_BASE0        (24'h808300),
        .LANE_BASE1        (24'h808400),
        .INIT_VS           (2'd2),
        .INIT_PE           (2'd0),
        .MAX_VS            (2'd3),   // echo mode: preserves the VS3 assertions below
        .MAX_PE            (2'd3)    //   (production default is MAX_VS=2, see tb_afe_clamp)
    ) dut (
        .mgmt_clk        (mgmt_clk),
        .vs_request      (vs_request),
        .pe_request      (pe_request),
        .adjust_de       (adjust_de),
        .training_active (training_active),
        .train_set_byte  (train_set_byte),
        .afe_busy        (afe_busy),
        .dbg_afe         (dbg_afe),
        .drp_clk         (drp_clk),
        .drp_req         (drp_req),
        .drp_gnt         (drp_gnt),
        .drp_addr        (drp_addr),
        .drp_wrdata      (drp_wrdata),
        .drp_wren        (drp_wren),
        .drp_ready       (drp_ready)
    );

    // Disabled twin: same stimuli, must stay inert
    wire [7:0]  off_byte;
    wire        off_busy;
    wire [5:0]  off_dbg;
    wire        off_req;
    wire [23:0] off_addr;
    wire [31:0] off_data;
    wire        off_wren;

    afe_adjust_seq #(
        .ENABLE_AFE_ADJUST (0)
    ) dut_off (
        .mgmt_clk        (mgmt_clk),
        .vs_request      (vs_request),
        .pe_request      (pe_request),
        .adjust_de       (adjust_de),
        .training_active (training_active),
        .train_set_byte  (off_byte),
        .afe_busy        (off_busy),
        .dbg_afe         (off_dbg),
        .drp_clk         (drp_clk),
        .drp_req         (off_req),
        .drp_gnt         (1'b1),
        .drp_addr        (off_addr),
        .drp_wrdata     (off_data),
        .drp_wren        (off_wren),
        .drp_ready       (1'b1)
    );

    integer off_violations = 0;
    always @(posedge drp_clk)
        if (off_req !== 1'b0 || off_wren !== 1'b0)
            off_violations = off_violations + 1;
    always @(posedge mgmt_clk)
        if (off_byte !== 8'h06)
            off_violations = off_violations + 1;

    // ------------------------------------------------------------------
    // Mock DRP slave: grant after a few cycles; each accepted write gets
    // a one-cycle ready after `ready_delay` cycles (0 = never: timeout
    // test). Records (addr, data) of every accepted write.
    // ------------------------------------------------------------------
    integer ready_delay = 6;
    integer rd_cnt = 0;
    reg [23:0] wr_addr [0:255];
    reg [31:0] wr_data [0:255];
    integer    wr_cnt = 0;
    reg        wren_d = 1'b0;

    always @(posedge drp_clk) begin
        // grant: 3-cycle arbitration latency, held while requested
        if (!drp_req)       begin drp_gnt <= 1'b0; rd_cnt <= 0; end
        else if (!drp_gnt)  begin
            rd_cnt <= rd_cnt + 1;
            if (rd_cnt >= 3) drp_gnt <= 1'b1;
        end

        // write handshake
        drp_ready <= 1'b0;
        wren_d <= drp_wren;
        if (drp_wren && !wren_d)
            rd_cnt <= 0;               // reuse as ready timer
        if (drp_wren && wren_d && ready_delay != 0) begin
            rd_cnt <= rd_cnt + 1;
            if (rd_cnt == ready_delay) begin
                drp_ready <= 1'b1;
                wr_addr[wr_cnt] <= drp_addr;
                wr_data[wr_cnt] <= drp_wrdata;
                wr_cnt <= wr_cnt + 1;
            end
        end
        // sanity: wren must never assert without the grant
        if (drp_wren && !drp_gnt) begin
            errors = errors + 1;
            $display("FAIL: drp_wren asserted without grant");
        end
    end

    // ------------------------------------------------------------------
    // Checks
    // ------------------------------------------------------------------
    integer errors = 0;
    integer base_cnt;
    integer k;

    // expected 8-write image for a (vs,pe) application
    function [23:0] exp_addr(input integer i);
        reg [23:0] base;
        begin
            base = (i < 4) ? 24'h808300 : 24'h808400;
            case (i % 4)
                0: exp_addr = base + 24'h034;
                1: exp_addr = base + 24'h038;
                default: exp_addr = base + 24'h0d8;
            endcase
        end
    endfunction

    function [31:0] exp_data(input integer i, input [3:0] txlev, input [4:0] c1);
        case (i % 4)
            0: exp_data = {16'd0, txlev, 12'd0};
            1: exp_data = {19'd0, c1, 8'd0};
            2: exp_data = 32'h0000_0010;
            default: exp_data = 32'h0000_0110;
        endcase
    endfunction

    task check_seq(input integer first, input [3:0] txlev, input [4:0] c1,
                   input [255:0] label);
        begin
            for (k = 0; k < 8; k = k + 1) begin
                if (wr_addr[first+k] !== exp_addr(k) ||
                    wr_data[first+k] !== exp_data(k, txlev, c1)) begin
                    errors = errors + 1;
                    $display("FAIL(%0s): write %0d got %06x=%08x want %06x=%08x",
                             label, k, wr_addr[first+k], wr_data[first+k],
                             exp_addr(k), exp_data(k, txlev, c1));
                end
            end
        end
    endtask

    task wait_idle;                    // sequence completion (mgmt view)
        begin
            wait (afe_busy);
            wait (!afe_busy);
            #200;                      // let the last recorded write settle
        end
    endtask

    task check_byte(input [7:0] want, input [255:0] label);
        begin
            if (train_set_byte !== want) begin
                errors = errors + 1;
                $display("FAIL(%0s): train_set_byte %02x want %02x",
                         label, train_set_byte, want);
            end
        end
    endtask

    task check_count(input integer want, input [255:0] label);
        begin
            if (wr_cnt !== want) begin
                errors = errors + 1;
                $display("FAIL(%0s): wr_cnt %0d want %0d", label, wr_cnt, want);
            end else
                $display("  ok: %0s (wr_cnt %0d)", label, wr_cnt);
        end
    endtask

    initial begin
        // ---- 1. cold idle -------------------------------------------
        #2000;
        check_count(0, "cold idle: no DRP writes");
        check_byte(8'h06, "cold idle: legacy byte");
        if (drp_req !== 1'b0) begin
            errors = errors + 1; $display("FAIL: drp_req in idle");
        end

        // ---- 2. training start: INIT baseline (VS2/PE0) --------------
        @(negedge mgmt_clk) training_active = 1'b1;
        wait_idle;
        check_count(8, "training start applies INIT once");
        check_seq(0, 4'd13, 5'd0, "INIT VS2/PE0");
        check_byte(8'h02, "INIT byte vs2, no MAX flags");

        // ---- 3. request == applied: nothing happens -------------------
        send_adjust(2'd2, 2'd0);
        #5000;
        check_count(8, "matching request applies nothing");

        // ---- 4. VS3/PE2: apply, MAX_SWING, debounce -------------------
        send_adjust(2'd3, 2'd2);
        wait_idle;
        check_count(16, "VS3/PE2 applied once");
        check_seq(8, 4'd15, 5'd10, "VS3/PE2 txlev15/C1=10");
        check_byte(8'h17, "VS3/PE2 byte: MAX_SWING, pe=2");
        send_adjust(2'd3, 2'd2);   // further iterations, request unchanged
        send_adjust(2'd3, 2'd2);
        #8000;
        check_count(16, "unchanged request: debounced");

        // ---- 5. PE3: MAX_PE -------------------------------------------
        send_adjust(2'd3, 2'd3);
        wait_idle;
        check_count(24, "PE3 applied once");
        check_seq(16, 4'd15, 5'd13, "VS3/PE3 txlev15/C1=13");
        check_byte(8'h3F, "VS3/PE3 byte: MAX_SWING+MAX_PE");

        // ---- 6. not training: ignored ---------------------------------
        @(negedge mgmt_clk) training_active = 1'b0;
        send_adjust(2'd0, 2'd0);
        #5000;
        check_count(24, "request outside training ignored");

        // ---- 7. re-enter training: re-baseline, then VS1/PE1 ----------
        @(negedge mgmt_clk) training_active = 1'b1;
        wait_idle;
        check_count(32, "re-training re-applies INIT");
        check_seq(24, 4'd13, 5'd0, "re-INIT VS2/PE0");
        send_adjust(2'd1, 2'd1);
        wait_idle;
        check_count(40, "VS1/PE1 applied");
        check_seq(32, 4'd9, 5'd7, "VS1/PE1 txlev9/C1=7");
        check_byte(8'h09, "VS1/PE1 byte");

        // ---- 8. request change mid-flight: applied after, atomic ------
        ready_delay = 60;                        // slow the slave down
        send_adjust(2'd2, 2'd1);
        wait (afe_busy);
        #300;                                    // mid-sequence
        send_adjust(2'd0, 2'd0);
        wait (!afe_busy);                        // first sequence done
        wait_idle;                               // second follows on its own
        check_count(56, "mid-flight change: two atomic sequences");
        check_seq(40, 4'd13, 5'd7,  "in-flight VS2/PE1 completes");
        check_seq(48, 4'd5,  5'd0,  "queued VS0/PE0 applied after");
        check_byte(8'h00, "final byte VS0/PE0");
        ready_delay = 6;

        // ---- 9. timeout: sequence completes, sticky error -------------
        if (dbg_afe[5] !== 1'b0) begin
            errors = errors + 1; $display("FAIL: seq_err set before timeout test");
        end
        ready_delay = 0;                         // ready never comes
        base_cnt = wr_cnt;
        send_adjust(2'd1, 2'd0);
        wait_idle;                               // must not wedge
        if (wr_cnt !== base_cnt) begin
            errors = errors + 1;
            $display("FAIL: timeout writes were recorded as accepted");
        end
        if (dbg_afe[5] !== 1'b1) begin
            errors = errors + 1; $display("FAIL: seq_err not sticky after timeout");
        end else
            $display("  ok: timeout completes sequence, seq_err sticky");
        ready_delay = 6;

        // ---- 10. disabled twin stayed inert ---------------------------
        if (off_violations != 0) begin
            errors = errors + 1;
            $display("FAIL: ENABLE=0 twin showed %0d violations", off_violations);
        end else
            $display("  ok: ENABLE=0 twin fully inert (byte 0x06, no DRP)");

        if (errors == 0)
            $display("PASS: afe_adjust_seq — sequencing law, mapping, debounce, caps, gating, timeout, ENABLE=0 all verified");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

    // global watchdog
    initial begin
        #2_000_000;
        $display("FAIL: TB watchdog timeout");
        $finish;
    end

endmodule
