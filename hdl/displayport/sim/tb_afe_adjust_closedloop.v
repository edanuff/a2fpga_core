// tb_afe_adjust_closedloop.v
//
// M5 runtime TX-AFE adjustment — CLOSED-LOOP verification: the real AUX
// ladder (channel_management -> aux_channel -> aux_interface, using the
// PROPOSED shared-RTL changes in sim/m5_proposed/) trains against a
// scripted Manchester AUX sink while afe_adjust_seq drives a mock DRP
// slave. End-to-end checks:
//
//   1. The ladder trains to link_established against the scripted sink
//      (proposed changes do not break training).
//   2. At training start the INIT baseline (VS2/PE0) is applied over the
//      DRP (8 writes, sequencing law), and the first TRAINING_LANEx_SET
//      on the WIRE declares it (0x02).
//   3. The sink's ADJUST_REQUEST = VS3/PE2 (0xBB) causes exactly ONE
//      further application (txlev 15 / C1 10), after which every
//      TRAINING_LANEx_SET write on the wire declares 0x27
//      (vs3 + MAX_SWING_REACHED, pe2) — including the align-phase sets,
//      where the unchanged request is debounced (no third application).
//   4. Total DRP writes = exactly 16; no lane-set value other than
//      0x02/0x27 ever appears. (Since the per-lane change the sink's
//      symmetric request makes 0x103 and 0x104 carry the same value;
//      per-lane divergence is covered by tb_afe_perlane.)
//   5. No TRAINING_LANE_SET reaches the wire while afe_busy (the ladder's
//      afe_hold), with the DRP slave slowed past the 100 us wait_after.
//
// Run (PRODUCTION RTL — the m5_proposed copies this header used to name
// are pre-merge snapshots and no longer elaborate):
//   iverilog -g2012 -o /tmp/tb_afe_cl.vvp \
//        hdl/displayport/sim/tb_afe_adjust_closedloop.v \
//        hdl/displayport/auxch/channel_managemnt.v \
//        hdl/displayport/auxch/aux_channel.v \
//        hdl/displayport/auxch/dp_aux_messages.v \
//        hdl/displayport/auxch/aux_interface.v \
//        hdl/displayport/auxch/link_signal_mgmt.v \
//        hdl/displayport/auxch/edid_decode.v \
//        hdl/displayport/auxch/dp_register_decode.v \
//        hdl/displayport/auxch/hotplug_decode.v \
//        hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_afe_cl.vvp

`timescale 1ns/1ps

module tb_afe_adjust_closedloop;

    localparam HB = 500;               // AUX half-bit, ns (1 Mbps Manchester)

    reg clk100  = 1'b0;
    reg drp_clk = 1'b0;
    always #5   clk100  = ~clk100;
    always #4.1 drp_clk = ~drp_clk;

    integer errors = 0;
    // phase_done = 0: the trained phase never reports success, which is
    // the condition under which ADJUST_REQUESTs are applied at all.
    reg phase_done = 1'b0;
    // DRP slave write latency. 3000 drp clocks x 8 writes ~ 200 us per
    // sequence — longer than the AUX gap between the ADJUST read and the
    // next lane-set message (the 100 us wait_after state), so the ladder's afe_hold MUST engage for the
    // sink-requested change (makes the wire invariant below non-vacuous).
    localparam integer READY_DELAY = 3000;
    integer hold_seen = 0, wire_while_busy = 0;
    always @(posedge clk100) if (dut.i_aux_channel.afe_hold) hold_seen = hold_seen + 1;

    // ------------------------------------------------------------------
    // DUT: the proposed channel_management with AFE_ADJUST=1
    // ------------------------------------------------------------------
    wire        auxch_out, auxch_tri;
    reg         auxch_in = 1'b0;       // sink's TX (parked idle 0)
    wire        tx_clock_train, tx_align_train, tx_link_established;
    wire [3:0]  tx_powerup_channel;
    wire [15:0] debug_adjust;
    wire [15:0] train_set_byte;
    wire        adjust_evt;

    channel_management #(
        .LINK_RATE_MBPS (2700),
        .BLIND_SINK     (0),
        .AFE_ADJUST     (1)
    ) dut (
        .clk100               (clk100),
        .train_set_byte       (train_set_byte),
        .adjust_evt           (adjust_evt),
        .afe_busy             (afe_busy),
        .debug                (),
        .debug_rx             (),
        .debug_locks          (),
        .debug_gate           (),
        .debug_sink           (),
        .debug_adjust         (debug_adjust),
        .debug_chstate        (),
        .debug_caps           (),
        .hpd                  (1'b1),
        .auxch_in             (auxch_in),
        .auxch_out            (auxch_out),
        .auxch_tri            (auxch_tri),
        .stream_channel_count (3'b010),
        .source_channel_count (3'b010),
        .tx_clock_train       (tx_clock_train),
        .tx_align_train       (tx_align_train),
        .tx_powerup_channel   (tx_powerup_channel),
        .tx_preemp_0p0        (),
        .tx_preemp_3p5        (),
        .tx_preemp_6p0        (),
        .tx_swing_0p4         (),
        .tx_swing_0p6         (),
        .tx_swing_0p8         (),
        .tx_running           (4'b0011),
        .tx_link_established  (tx_link_established),
        .hpd_present_out      ()
    );

    // ------------------------------------------------------------------
    // afe_adjust_seq wired exactly as the proposed dp_transmitter diff
    // ------------------------------------------------------------------
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
    ) i_afe (
        .mgmt_clk        (clk100),
        .vs_request      ({debug_adjust[5:4], debug_adjust[1:0]}),
        .pe_request      ({debug_adjust[7:6], debug_adjust[3:2]}),
        .adjust_de       (adjust_evt),
        .training_active (tx_clock_train | tx_align_train), .phase_done(phase_done), .phy_reinit(1'b0),
        .train_set_byte  (train_set_byte),
        .afe_busy        (afe_busy),
        .dbg_afe         (dbg_afe),
        .dbg_afe1        (),
        .drp_clk         (drp_clk),
        .drp_req         (drp_req),
        .drp_gnt         (drp_gnt),
        .drp_addr        (drp_addr),
        .drp_wrdata      (drp_wrdata),
        .drp_wren        (drp_wren),
        .drp_ready       (drp_ready)
    );

    // ------------------------------------------------------------------
    // Mock DRP slave (as in tb_afe_adjust): grant + delayed ready, records
    // ------------------------------------------------------------------
    integer rd_cnt = 0;
    reg [23:0] wr_addr [0:63];
    reg [31:0] wr_data [0:63];
    integer    wr_cnt = 0;
    reg        wren_d = 1'b0;

    always @(posedge drp_clk) begin
        if (!drp_req)      begin drp_gnt <= 1'b0; rd_cnt <= 0; end
        else if (!drp_gnt) begin
            rd_cnt <= rd_cnt + 1;
            if (rd_cnt >= 3) drp_gnt <= 1'b1;
        end
        drp_ready <= 1'b0;
        wren_d <= drp_wren;
        if (drp_wren && !wren_d) rd_cnt <= 0;
        if (drp_wren && wren_d) begin
            rd_cnt <= rd_cnt + 1;
            if (rd_cnt == READY_DELAY) begin
                drp_ready <= 1'b1;
                wr_addr[wr_cnt] <= drp_addr;
                wr_data[wr_cnt] <= drp_wrdata;
                wr_cnt <= wr_cnt + 1;
            end
        end
    end

    // ------------------------------------------------------------------
    // Scripted DPCD model
    // ------------------------------------------------------------------
    reg [7:0] r202 = 8'h00;            // lane0/1 status
    reg [7:0] r204 = 8'h00;            // align status
    reg [7:0] r205 = 8'h00;            // sink status
    reg [7:0] r206 = 8'hBB;            // ADJUST_REQUEST: VS3/PE2 both lanes

    function [7:0] dpcd_read(input [19:0] a);
        case (a)
            20'h00000: dpcd_read = 8'h11;   // DPCD rev 1.1
            20'h00001: dpcd_read = 8'h0A;   // 2.7 Gbps
            20'h00002: dpcd_read = 8'h82;   // enhanced framing, 2 lanes
            20'h00200: dpcd_read = 8'h01;   // sink count
            20'h00202: dpcd_read = r202;
            20'h00204: dpcd_read = r204;
            20'h00205: dpcd_read = r205;
            20'h00206: dpcd_read = r206;
            default:   dpcd_read = 8'h00;
        endcase
    endfunction

    // records
    reg [7:0] lane_set_log [0:63];     // every DPCD 0x103 write (byte 0)
    integer   ls_cnt = 0;
    reg [7:0] last_pattern = 8'h00;    // DPCD 0x102
    reg       tps2_seen = 1'b0;

    // ------------------------------------------------------------------
    // Manchester AUX sink: decode requests from auxch_out/auxch_tri,
    // reply on auxch_in
    // ------------------------------------------------------------------
    reg  [0:4095] hv;                  // sampled half-bits
    integer nh;
    reg  [7:0] req  [0:31];
    integer    req_len;
    reg  [7:0] rbuf [0:31];
    integer    rlen;
    integer i, j, k, b, bo, bitv;
    reg [7:0] byte_acc;
    reg  [19:0] req_addr;
    integer     dlen;

    task send_half(input v); begin auxch_in = v; #(HB); end endtask
    task send_bit_s(input v); begin send_half(v); send_half(~v); end endtask
    task send_raw8_s(input [7:0] h);
        begin for (b = 7; b >= 0; b = b - 1) send_half(h[b]); end
    endtask
    task send_byte_s(input [7:0] v);
        begin for (b = 7; b >= 0; b = b - 1) send_bit_s(v[b]); end
    endtask

    task send_reply;                    // rbuf[0..rlen-1]
        begin
            #8000;                      // clear the DUT's RX holdoff
            for (bo = 0; bo < 16; bo = bo + 1) send_bit_s(1'b0);  // sync
            send_raw8_s(8'b1111_0000);  // START
            for (bo = 0; bo < rlen; bo = bo + 1) send_byte_s(rbuf[bo]);
            send_raw8_s(8'b1111_0000);  // STOP
            auxch_in = 1'b0;            // parked idle
        end
    endtask

    // capture one request whenever the source drives the wire
    initial forever begin
        @(negedge auxch_tri);
        nh = 0;
        #(HB/2);
        while (auxch_tri === 1'b0 && nh < 4096) begin
            hv[nh] = auxch_out;
            nh = nh + 1;
            #(HB);
        end
        decode_and_respond;
    end

    task decode_and_respond;
        begin
            // locate START: the exact 11110000 half-bit pattern (the last
            // sync half is also 1, so a bare 1111 match lands one early)
            i = 0;
            while (i < nh-8 &&
                   !( hv[i]   &  hv[i+1] &  hv[i+2] &  hv[i+3] &
                     ~hv[i+4] & ~hv[i+5] & ~hv[i+6] & ~hv[i+7]))
                i = i + 1;
            i = i + 8;                  // skip 1111 0000
            // collect Manchester bits until STOP (1111 at pair boundary)
            req_len = 0; byte_acc = 8'h00; k = 0;
            while (i < nh-1) begin
                if (i < nh-3 && hv[i] & hv[i+1] & hv[i+2] & hv[i+3])
                    i = nh;             // STOP
                else begin
                    bitv = hv[i];
                    byte_acc = {byte_acc[6:0], bitv[0]};
                    k = k + 1;
                    if (k == 8) begin
                        req[req_len] = byte_acc;
                        req_len = req_len + 1;
                        k = 0;
                    end
                    i = i + 2;
                end
            end
            if ($test$plusargs("verbose")) begin
                $display("  [sink] nh=%0d req_len=%0d req=%02x %02x %02x %02x %02x (t=%0t)",
                         nh, req_len, req[0], req[1], req[2], req[3], req[4], $time);
                $write("  [sink] halves: ");
                for (j = 0; j < (nh > 64 ? 64 : nh); j = j + 1)
                    $write("%b", hv[j]);
                $write("\n");
            end
            if (req_len < 4) begin
                errors = errors + 1;
                $display("FAIL: sink decoded runt request (%0d bytes)", req_len);
            end else begin
                req_addr = {req[0][3:0], req[1], req[2]};
                dlen     = req[3] + 1;
                if (req[0][7:4] == 4'h9) begin          // native read
                    rbuf[0] = 8'h00;                    // ACK
                    for (j = 0; j < dlen; j = j + 1)
                        rbuf[j+1] = dpcd_read(req_addr + j);
                    rlen = dlen + 1;
                    send_reply;
                end else if (req[0][7:4] == 4'h8) begin // native write
                    if (req_addr == 20'h00103) begin
                        lane_set_log[ls_cnt] = req[4];
                        ls_cnt = ls_cnt + 1;
                        // invariant: a lane-set byte never reaches the wire
                        // while the sequencer is still applying/evaluating
                        if (afe_busy) wire_while_busy = wire_while_busy + 1;
                        if (req[5] !== req[4] || req[6] !== req[4] ||
                            req[7] !== req[4]) begin
                            errors = errors + 1;
                            $display("FAIL: 0x103 write lanes differ: %02x %02x %02x %02x",
                                     req[4], req[5], req[6], req[7]);
                        end
                        $display("  [sink] TRAINING_LANE_SET = %02x (t=%0t)",
                                 req[4], $time);
                    end
                    if (req_addr == 20'h00102) begin
                        last_pattern = req[4];
                        if (req[4][3:0] == 4'h2) tps2_seen = 1'b1;
                        $display("  [sink] TRAINING_PATTERN_SET = %02x (t=%0t)",
                                 req[4], $time);
                    end
                    rbuf[0] = 8'h00;                    // ACK
                    rlen = 1;
                    send_reply;
                end
                // I2C-class requests would be NACK'd; the ladder sends none
            end
        end
    endtask

    // ------------------------------------------------------------------
    // Script: grant CR after both applications landed; grant EQ after TPS2
    // ------------------------------------------------------------------
    initial begin
        wait (wr_cnt == 16);            // INIT + VS3/PE2 both applied
        r202 = 8'h11;                   // CR done, both lanes
        $display("  [script] CR granted after 2nd AFE application (t=%0t)", $time);
        wait (tps2_seen);
        r202 = 8'h77;                   // CR+EQ+SYM
        r204 = 8'h01;                   // inter-lane align done
        r205 = 8'h01;                   // sink receiving
        $display("  [script] EQ granted after TPS2 (t=%0t)", $time);
    end

    // ------------------------------------------------------------------
    // Expected 8-write image (shared with tb_afe_adjust)
    // ------------------------------------------------------------------
    integer m;
    function [23:0] exp_addr(input integer x);
        reg [23:0] base;
        begin
            base = (x < 4) ? 24'h808300 : 24'h808400;
            case (x % 4)
                0: exp_addr = base + 24'h034;
                1: exp_addr = base + 24'h038;
                default: exp_addr = base + 24'h0d8;
            endcase
        end
    endfunction
    function [31:0] exp_data(input integer x, input [3:0] txlev, input [4:0] c1);
        case (x % 4)
            0: exp_data = {16'd0, txlev, 12'd0};
            1: exp_data = {19'd0, c1, 8'd0};
            2: exp_data = 32'h0000_0010;
            default: exp_data = 32'h0000_0110;
        endcase
    endfunction
    task check_seq(input integer first, input [3:0] txlev, input [4:0] c1,
                   input [255:0] label);
        begin
            for (m = 0; m < 8; m = m + 1)
                if (wr_addr[first+m] !== exp_addr(m) ||
                    wr_data[first+m] !== exp_data(m, txlev, c1)) begin
                    errors = errors + 1;
                    $display("FAIL(%0s): write %0d got %06x=%08x want %06x=%08x",
                             label, m, wr_addr[first+m], wr_data[first+m],
                             exp_addr(m), exp_data(m, txlev, c1));
                end
        end
    endtask

    // ------------------------------------------------------------------
    // Main check sequence
    // ------------------------------------------------------------------
    integer saw02, saw27, sawother, first27;
    initial begin
        wait (tx_link_established);
        #200_000;                       // let trailing AUX traffic finish

        $display("  ok: ladder trained to link_established (t=%0t)", $time);

        if (wr_cnt !== 16) begin
            errors = errors + 1;
            $display("FAIL: DRP writes = %0d, want 16 (INIT + one adjust)", wr_cnt);
        end else
            $display("  ok: exactly two AFE applications (16 DRP writes)");
        check_seq(0, 4'd13, 5'd0,  "INIT VS2/PE0");
        check_seq(8, 4'd15, 5'd0, "adjust VS3/PE2 -> sanitised VS3/PE0 (VS+PE<=3)");

        // lane-set wire log: some 0x02 first, then only 0x27, nothing else
        saw02 = 0; saw27 = 0; sawother = 0; first27 = -1;
        for (m = 0; m < ls_cnt; m = m + 1) begin
            if (lane_set_log[m] === 8'h02) begin
                saw02 = saw02 + 1;
                if (first27 >= 0) begin
                    errors = errors + 1;
                    $display("FAIL: 0x02 lane-set after 0x27 (idx %0d)", m);
                end
            end else if (lane_set_log[m] === 8'h27) begin
                saw27 = saw27 + 1;
                if (first27 < 0) first27 = m;
            end else
                sawother = sawother + 1;
        end
        if (ls_cnt < 2 || saw02 < 1 || saw27 < 1 || sawother != 0) begin
            errors = errors + 1;
            $display("FAIL: lane-set wire log bad: n=%0d 02s=%0d 27s=%0d other=%0d",
                     ls_cnt, saw02, saw27, sawother);
        end else
            $display("  ok: wire TRAINING_LANE_SET: %0d x 0x02 (INIT) then %0d x 0x27 (VS3+MAX_SWING, PE sanitised to 0 +MAX_PE)",
                     saw02, saw27);

        // after training ends (training_active low) the declaration
        // intentionally reverts to the idle byte; no lane-set message is
        // sent in that window, and a re-train re-baselines via INIT
        if (train_set_byte[7:0] !== 8'h27) begin
            errors = errors + 1;
            $display("FAIL: post-training train_set_byte %02x want 27 (retained)",
                     train_set_byte[7:0]);
        end
        if (last_pattern !== 8'h00) begin
            errors = errors + 1;
            $display("FAIL: training patterns not cleared (0x102=%02x)", last_pattern);
        end

        if (wire_while_busy != 0) begin
            errors = errors + 1;
            $display("FAIL: %0d TRAINING_LANE_SET write(s) on the wire while afe_busy", wire_while_busy);
        end else if (hold_seen == 0) begin
            errors = errors + 1;
            $display("FAIL: afe_hold never engaged (invariant vacuous) — slave too fast?");
        end else
            $display("  ok: afe_hold engaged %0d clocks; no lane-set on the wire while busy", hold_seen);
        if (errors == 0)
            $display("PASS: closed-loop — sink ADJUST_REQUEST applied over DRP once per change, TRAINING_LANE_SET on the wire reflects applied levels + caps, link trains");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

    initial begin
        #60_000_000;                    // 60 ms
        $display("FAIL: TB watchdog timeout (state=%02x wr_cnt=%0d ls_cnt=%0d)",
                 dut.i_aux_channel.state, wr_cnt, ls_cnt);
        $finish;
    end

endmodule
