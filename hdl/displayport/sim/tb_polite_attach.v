// tb_polite_attach.v
//
// POLITE ATTACH + ESI SERVICE — closed-loop verification against a
// scripted Manchester AUX sink that models the Anker hub's measured
// behavior (Mac<->Anker answer key, 08-26):
//
//   - HPD is up but the hub is NOT READY: SINK_COUNT reads 0x40 (count 0)
//     for the first ~25 ms, then 0x41. The wire capture showed our legacy
//     ladder hammering ~1150 transactions into this window.
//   - EDID: every 16-byte I2C read DEFERs exactly twice, ~0.4-0.65 ms
//     apart, then ACKs (the hub's metronomic measured cadence).
//   - Sink events are ESI-ONLY: 0x201 is never raised (wire-proven);
//     service = RD 0x2003 len13 + W1C 0x2005.
//
// Checks (normal mode):
//   1. While the count field is 0, presence re-reads are spaced >=15 ms
//      (paced presence_wait) and NO training-config write goes out.
//   2. EDID preamble: I2C addr phase, then 8 block reads, each DEFERred
//      twice with retries spaced >=0.6 ms; 24 read requests total.
//   3. Attach-time ESI ack: RD 0x2003 + WR 0x2005=0x02, before training.
//   4. TPS1 is written only after: count>=1, EDID complete, ESI acked.
//   5. Lane-set writes are PER-ITERATION (spec-faithful): the hub model
//      grants CR only when a TRAINING_LANE_SET write arrives while it is
//      ready — modeling the IT6563's write-cued CR evaluation that
//      HARDWARE-REFUTED the write-on-change skip (7d6e205d: skip enabled
//      -> CR never granted, watchdog loops). Values stay 0x02 (INIT)
//      then 0x27 (applied change), nothing else, no 0x02 after 0x27.
//   6. Runtime ESI service: hpd_irq with 0x2005=0x02 pending -> status
//      read, RD 0x2003, W1C 0x2005 with the read value; exactly one ack;
//      tx_link_established never drops.
//
// +giveup mode (strict-converter fallback): the sink DEFERs EDID forever;
// the defer budget must abandon EDID and the link must still train.
//
// Run:
//   iverilog -g2012 -o /tmp/tb_polite.vvp \
//        hdl/displayport/sim/tb_polite_attach.v \
//        hdl/displayport/auxch/channel_managemnt.v \
//        hdl/displayport/auxch/aux_channel.v \
//        hdl/displayport/auxch/dp_aux_messages.v \
//        hdl/displayport/auxch/aux_interface.v \
//        hdl/displayport/auxch/link_signal_mgmt.v \
//        hdl/displayport/auxch/edid_decode.v \
//        hdl/displayport/auxch/dp_register_decode.v \
//        hdl/displayport/auxch/hotplug_decode.v \
//        hdl/displayport/gowin/afe_adjust_seq.v \
//        && vvp /tmp/tb_polite.vvp && vvp /tmp/tb_polite.vvp +giveup

`timescale 1ns/1ps

module tb_polite_attach;

    localparam HB = 500;               // AUX half-bit, ns (1 Mbps Manchester)

    reg clk100  = 1'b0;
    reg drp_clk = 1'b0;
    always #5   clk100  = ~clk100;
    always #4.1 drp_clk = ~drp_clk;

    integer errors = 0;
    reg giveup_mode = 1'b0;

    wire [11:0] dbg_evt;
    reg phase_done = 1'b0;
    localparam integer READY_DELAY = 300;   // fast DRP (hold covered elsewhere)

    // ------------------------------------------------------------------
    // DUT: POLITE_ATTACH=1, IRQ_SERVICE=2 (ESI), AFE_ADJUST=1
    // ------------------------------------------------------------------
    wire        auxch_out, auxch_tri;
    reg         auxch_in = 1'b0;
    wire        tx_clock_train, tx_align_train, tx_link_established;
    wire [3:0]  tx_powerup_channel;
    wire [15:0] debug_adjust;
    wire [15:0] train_set_byte;
    wire        adjust_evt;

    channel_management #(
        .LINK_RATE_MBPS (2700),
        .BLIND_SINK     (0),
        .IRQ_SERVICE    (2),
        .POLITE_ATTACH  (1),
        .WEDGE_CLKS     (30'd400_000),   // 4 ms for sim
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
        .MAX_VS            (2'd3),
        .MAX_PE            (2'd3)
    ) i_afe (
        .mgmt_clk        (clk100),
        .vs_request      ({debug_adjust[5:4], debug_adjust[1:0]}),
        .pe_request      ({debug_adjust[7:6], debug_adjust[3:2]}),
        .adjust_de       (adjust_evt),
        .training_active (tx_clock_train | tx_align_train), .phase_done(phase_done), .phy_reinit(1'b0),
        .train_set_byte  (train_set_byte),
        .afe_busy        (afe_busy),
        .dbg_afe         (dbg_afe),
        .dbg_afe1        (), .dbg_evt(dbg_evt),
        .drp_clk         (drp_clk),
        .drp_req         (drp_req),
        .drp_gnt         (drp_gnt),
        .drp_addr        (drp_addr),
        .drp_wrdata      (drp_wrdata),
        .drp_wren        (drp_wren),
        .drp_ready       (drp_ready)
    );

    integer rd_cnt = 0;
    integer wr_cnt = 0;
    reg     wren_d = 1'b0;
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
                wr_cnt <= wr_cnt + 1;
            end
        end
    end

    // ------------------------------------------------------------------
    // Scripted DPCD / hub model
    // ------------------------------------------------------------------
    reg [7:0] m_sink_count = 8'h40;    // hub not ready: count field 0
    reg [7:0] r202 = 8'h00;
    reg [7:0] r204 = 8'h00;
    reg [7:0] r205 = 8'h00;
    reg [7:0] r206 = 8'hBB;            // ADJUST_REQUEST: VS3/PE2 both lanes
    reg [7:0] m_esi_2005 = 8'h00;      // LINK_SERVICE_IRQ_VECTOR_ESI0
    reg [7:0] m_esi_2003 = 8'h00;      // DEVICE_SERVICE_IRQ_VECTOR_ESI0
    reg       cr_armed   = 1'b0;
    reg       esi_nack_mode = 1'b0;      // sink NACKs ESI reads (failure-tolerance test)       // hub ready to grant CR — but only
                                       // ON a lane-set write (IT6563
                                       // write-cued evaluation, hw-proven)

    function [7:0] dpcd_read(input [19:0] a);
        case (a)
            20'h00000: dpcd_read = 8'h11;
            20'h00001: dpcd_read = 8'h0A;
            20'h00002: dpcd_read = 8'h82;
            20'h00200: dpcd_read = m_sink_count;
            20'h00201: dpcd_read = 8'h00;   // ESI-only hub: NEVER raised
            20'h00202: dpcd_read = r202;
            20'h00204: dpcd_read = r204;
            20'h00205: dpcd_read = r205;
            20'h00206: dpcd_read = r206;
            20'h02005: dpcd_read = m_esi_2005;
            default:   dpcd_read = 8'h00;
        endcase
    endfunction

    // records
    integer   presence0_reads = 0;     // 0x200 len1 reads answered 0x40
    real      presence0_last  = -1;
    real      presence0_min_dt = 1e12;
    integer   i2c_addr_writes = 0;
    integer   i2c_reads       = 0;
    integer   edid_defers     = 0;
    integer   edid_acks       = 0;
    integer   defers_this_block = 0;
    reg       last_i2c_was_defer = 1'b0;
    real      last_i2c_t = -1;
    real      i2c_retry_min_dt = 1e12;
    real      t_last_i2c_read = -1;
    integer   esi_reads  = 0;
    integer   esi_writes = 0;
    reg [7:0] esi_write_val = 8'h00;
    integer   esi_writes_post = 0;     // after link_established
    integer   esi3_writes = 0;         // 0x2003 clears pre-establish
    integer   esi3_writes_post = 0;    // 0x2003 clears post-establish
    reg [7:0] esi3_write_val_post = 8'h00;
    reg [7:0] esi_write_val_post = 8'h00;
    reg       established_phase = 1'b0;
    real      t_first_tps1 = -1;
    real      t_edid_done  = -1;
    real      t_sink_ready = -1;
    real      t_esi_ack    = -1;
    integer   ls_cnt = 0;
    reg [7:0] lane_set_log [0:63];
    reg [7:0] last_pattern = 8'h00;
    reg       tps1_seen = 1'b0, tps2_seen = 1'b0;
    integer   cr_status_polls = 0;     // 0x200 len8 reads between TPS1/TPS2

    // link must never drop once established. Sampled at posedge: the
    // FSM's default-0-then-case-1 NBA pattern produces a ZERO-WIDTH 0
    // inside the NBA region on every transition (two scheduled updates,
    // executed in order) — real to @(negedge) monitors, invisible to
    // hardware and to any clocked consumer.
    integer le_drops = 0;
    always @(posedge clk100)
        if (established_phase && !tx_link_established) begin
            le_drops = le_drops + 1;
            if (le_drops < 4)
                $display("FAIL: tx_link_established low at t=%0t (state=%02x)",
                         $time, dut.i_aux_channel.state);
        end

    // ------------------------------------------------------------------
    // Manchester AUX sink
    // ------------------------------------------------------------------
    reg  [0:4095] hv;
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
    task send_reply;
        begin
            #8000;
            for (bo = 0; bo < 16; bo = bo + 1) send_bit_s(1'b0);
            send_raw8_s(8'b1111_0000);
            for (bo = 0; bo < rlen; bo = bo + 1) send_byte_s(rbuf[bo]);
            send_raw8_s(8'b1111_0000);
            auxch_in = 1'b0;
        end
    endtask

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
            i = 0;
            while (i < nh-8 &&
                   !( hv[i]   &  hv[i+1] &  hv[i+2] &  hv[i+3] &
                     ~hv[i+4] & ~hv[i+5] & ~hv[i+6] & ~hv[i+7]))
                i = i + 1;
            i = i + 8;
            req_len = 0; byte_acc = 8'h00; k = 0;
            while (i < nh-1) begin
                if (i < nh-3 && hv[i] & hv[i+1] & hv[i+2] & hv[i+3])
                    i = nh;
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
            if (req_len < 3) begin
                errors = errors + 1;
                $display("FAIL: sink decoded runt request (%0d bytes)", req_len);
            end else begin
                req_addr = {req[0][3:0], req[1], req[2]};
                dlen     = (req_len >= 4) ? req[3] + 1 : 0;
                case (req[0][7:4])
                4'h9: begin                             // native read
                    if (req_addr == 20'h00200 && dlen == 1) begin
                        if (m_sink_count[5:0] == 6'd0) begin
                            presence0_reads = presence0_reads + 1;
                            if (presence0_last >= 0 &&
                                ($realtime - presence0_last) < presence0_min_dt)
                                presence0_min_dt = $realtime - presence0_last;
                            presence0_last = $realtime;
                        end else if (t_sink_ready < 0)
                            t_sink_ready = $realtime;
                    end
                    if (req_addr == 20'h00200 && dlen == 8 &&
                        tps1_seen && !tps2_seen)
                        cr_status_polls = cr_status_polls + 1;
                    if (req_addr == 20'h02003 && esi_nack_mode) begin
                        esi_reads = esi_reads + 1;
                        rbuf[0] = 8'h10;            // AUX_NACK
                        rlen = 1;
                        send_reply;
                    end else if (req_addr == 20'h02003) begin
                        esi_reads = esi_reads + 1;
                        rbuf[0] = 8'h00;
                        for (j = 0; j < dlen; j = j + 1)
                            rbuf[j+1] = 8'h00;
                        rbuf[1] = m_esi_2003;           // 0x2003 = byte idx 0
                        rbuf[3] = m_esi_2005;           // 0x2005 = byte idx 2
                        rlen = dlen + 1;
                        send_reply;
                    end else begin
                        rbuf[0] = 8'h00;
                        for (j = 0; j < dlen; j = j + 1)
                            rbuf[j+1] = dpcd_read(req_addr + j);
                        rlen = dlen + 1;
                        send_reply;
                    end
                end
                4'h8: begin                             // native write
                    if (req_addr == 20'h02003) begin
                        if (!established_phase)
                            esi3_writes = esi3_writes + 1;
                        else begin
                            esi3_writes_post    = esi3_writes_post + 1;
                            esi3_write_val_post = req[4];
                        end
                        m_esi_2003 = m_esi_2003 & ~req[4];
                    end
                    if (req_addr == 20'h02005) begin
                        if (!established_phase) begin
                            esi_writes    = esi_writes + 1;
                            esi_write_val = req[4];
                            if (t_esi_ack < 0) t_esi_ack = $realtime;
                        end else begin
                            esi_writes_post    = esi_writes_post + 1;
                            esi_write_val_post = req[4];
                        end
                        m_esi_2005 = m_esi_2005 & ~req[4];
                    end
                    if (req_addr == 20'h00103 && ls_cnt < 64) begin
                        lane_set_log[ls_cnt] = req[4];
                        ls_cnt = ls_cnt + 1;
                        if (cr_armed && r202 == 8'h00) begin
                            r202 = 8'h11;
                            $display("  [sink] CR granted ON lane-set write (t=%0t)", $time);
                        end
                        if (ls_cnt < 6 || (ls_cnt % 32) == 0)
                            $display("  [sink] TRAINING_LANE_SET = %02x (t=%0t)", req[4], $time);
                    end
                    if (req_addr == 20'h00102) begin
                        last_pattern = req[4];
                        if (req[4][3:0] == 4'h1 && !tps1_seen) begin
                            tps1_seen = 1'b1;
                            t_first_tps1 = $realtime;
                        end
                        if (req[4][3:0] == 4'h2) tps2_seen = 1'b1;
                        $display("  [sink] TRAINING_PATTERN_SET = %02x (t=%0t)", req[4], $time);
                    end
                    rbuf[0] = 8'h00;
                    rlen = 1;
                    send_reply;
                end
                4'h4: begin                             // I2C write (MOT): addr phase
                    i2c_addr_writes = i2c_addr_writes + 1;
                    rbuf[0] = 8'h00;
                    rlen = 1;
                    send_reply;
                end
                4'h5: begin                             // I2C read (MOT): EDID block
                    i2c_reads = i2c_reads + 1;
                    if (last_i2c_was_defer && last_i2c_t >= 0 &&
                        ($realtime - last_i2c_t) < i2c_retry_min_dt)
                        i2c_retry_min_dt = $realtime - last_i2c_t;
                    last_i2c_t = $realtime;
                    t_last_i2c_read = $realtime;
                    if (giveup_mode || defers_this_block < 2) begin
                        defers_this_block = defers_this_block + 1;
                        edid_defers = edid_defers + 1;
                        last_i2c_was_defer = 1'b1;
                        rbuf[0] = 8'h20;                // AUX_DEFER
                        rlen = 1;
                        send_reply;
                    end else begin
                        defers_this_block = 0;
                        last_i2c_was_defer = 1'b0;
                        edid_acks = edid_acks + 1;
                        if (edid_acks == 8) t_edid_done = $realtime;
                        rbuf[0] = 8'h00;
                        for (j = 0; j < dlen; j = j + 1)
                            rbuf[j+1] = edid_acks*16 + j;
                        rlen = dlen + 1;
                        send_reply;
                    end
                end
                default: begin                          // other I2C: plain ACK
                    rbuf[0] = 8'h00;
                    rlen = 1;
                    send_reply;
                end
                endcase
            end
        end
    endtask

    // ------------------------------------------------------------------
    // Hub-ready script: count goes 0x40 -> 0x41 after two paced presence
    // reads (normal mode); CR granted after both AFE applications AND
    // three CR polls; EQ after TPS2.
    // ------------------------------------------------------------------
    initial begin
        if ($test$plusargs("giveup")) begin
            giveup_mode  = 1'b1;
            m_sink_count = 8'h41;       // ready immediately; EDID never ACKs
        end else begin
            wait (presence0_reads >= 2);
            #5_000_000;                 // 5 ms after the 2nd not-ready read
            m_sink_count = 8'h41;
            $display("  [script] hub ready: SINK_COUNT 0x40 -> 0x41 (t=%0t)", $time);
        end
        wait (wr_cnt == 16 && cr_status_polls >= 3);
        cr_armed = 1'b1;
        $display("  [script] CR ARMED (16 DRP writes, %0d polls) — grant on next lane-set write (t=%0t)",
                 cr_status_polls, $time);
        wait (tps2_seen);
        r202 = 8'h77;
        r204 = 8'h01;
        r205 = 8'h01;
        $display("  [script] EQ granted after TPS2 (t=%0t)", $time);
    end

    // trigger an on-demand periodic check (the real timer is 1 s)
    task force_check;
        begin
            force dut.i_aux_channel.link_check_now = 1'b1;
            repeat (2) @(posedge clk100);
            release dut.i_aux_channel.link_check_now;
        end
    endtask

    // ------------------------------------------------------------------
    // Main check sequence
    // ------------------------------------------------------------------
    integer m, saw02, saw27, sawother;
    integer er0, ew0;
    initial begin
        wait (tx_link_established);
        #300_000;
        established_phase = 1'b1;
        $display("  ok: ladder trained to link_established (t=%0t)", $time);

        if (giveup_mode) begin
            // strict-converter fallback: EDID abandoned, link still up
            if (edid_acks != 0) begin
                errors = errors + 1;
                $display("FAIL(giveup): %0d EDID acks (sink never ACKs)", edid_acks);
            end
            if (i2c_reads > 43) begin
                errors = errors + 1;
                $display("FAIL(giveup): %0d I2C reads — defer budget did not cap", i2c_reads);
            end else
                $display("  ok(giveup): EDID abandoned after %0d deferred reads", i2c_reads);
            if (t_last_i2c_read >= t_first_tps1) begin
                errors = errors + 1;
                $display("FAIL(giveup): I2C read after training started");
            end else
                $display("  ok(giveup): no EDID traffic after training start");
        end else begin
            // 1. paced presence while not ready
            if (presence0_reads < 2) begin
                errors = errors + 1;
                $display("FAIL: only %0d not-ready presence reads (want >=2)", presence0_reads);
            end else if (presence0_min_dt < 15_000_000) begin
                errors = errors + 1;
                $display("FAIL: presence re-reads %.1f ms apart (want >=15 ms)",
                         presence0_min_dt/1e6);
            end else
                $display("  ok: %0d not-ready presence reads, min spacing %.1f ms",
                         presence0_reads, presence0_min_dt/1e6);

            // 2. EDID preamble: 8 blocks, 2 defers each, paced retries
            if (i2c_addr_writes < 1) begin
                errors = errors + 1;
                $display("FAIL: no I2C address phase before EDID reads");
            end
            if (edid_acks != 8 || edid_defers != 16 || i2c_reads != 24) begin
                errors = errors + 1;
                $display("FAIL: EDID acks=%0d defers=%0d reads=%0d (want 8/16/24)",
                         edid_acks, edid_defers, i2c_reads);
            end else
                $display("  ok: EDID complete — 8 blocks, 2 DEFERs each, 24 reads");
            if (i2c_retry_min_dt < 600_000) begin
                errors = errors + 1;
                $display("FAIL: DEFER retry after %.0f us (want >=600 us)",
                         i2c_retry_min_dt/1e3);
            end else
                $display("  ok: DEFER retries paced (min %.0f us)", i2c_retry_min_dt/1e3);

            // 3+4. ordering: ESI ack + EDID + ready all before TPS1
            if (esi_reads < 1 || esi_writes < 1) begin
                errors = errors + 1;
                $display("FAIL: attach ESI service missing (reads=%0d writes=%0d)",
                         esi_reads, esi_writes);
            end else if (esi_write_val !== 8'h02) begin
                errors = errors + 1;
                $display("FAIL: attach ESI ack value %02x (want 02)", esi_write_val);
            end else if (esi3_writes != 0) begin
                errors = errors + 1;
                $display("FAIL: %0d attach-time 0x2003 clears (vector was 0 — none expected)", esi3_writes);
            end else
                $display("  ok: attach ESI ack — RD 0x2003 + WR 0x2005=02, no spurious 0x2003 clear");
            if (t_first_tps1 < 0 ||
                t_first_tps1 < t_sink_ready ||
                t_first_tps1 < t_edid_done ||
                t_first_tps1 < t_esi_ack) begin
                errors = errors + 1;
                $display("FAIL: TPS1 at %.1f ms vs ready %.1f / edid %.1f / esi %.1f ms",
                         t_first_tps1/1e6, t_sink_ready/1e6, t_edid_done/1e6, t_esi_ack/1e6);
            end else
                $display("  ok: training starts only after ready+EDID+ESI (TPS1 at %.1f ms)",
                         t_first_tps1/1e6);
        end

        // 5. per-iteration lane-set writes (spec-faithful), values
        // ordered 0x02 (INIT) then 0x27, never 0x02 again, nothing else
        saw02 = 0; saw27 = 0; sawother = 0;
        for (m = 0; m < ls_cnt; m = m + 1) begin
            if (lane_set_log[m] === 8'h02) begin
                saw02 = saw02 + 1;
                if (saw27 > 0) begin
                    errors = errors + 1;
                    $display("FAIL: 0x02 lane-set after 0x27 (idx %0d)", m);
                end
            end
            else if (lane_set_log[m] === 8'h27) saw27 = saw27 + 1;
            else    sawother = sawother + 1;
        end
        if (ls_cnt < 3 || saw02 < 1 || saw27 < 1 || sawother != 0) begin
            errors = errors + 1;
            $display("FAIL: lane-set writes n=%0d (02s=%0d 27s=%0d other=%0d) want per-iteration 02s then 27s",
                     ls_cnt, saw02, saw27, sawother);
        end else
            $display("  ok: per-iteration lane-set writes (%0d: %0d x 0x02, %0d x 0x27) across %0d CR polls; CR granted on a write",
                     ls_cnt, saw02, saw27, cr_status_polls);

        // 6. runtime ESI service — BOTH vectors pending: 0x2005 (link)
        // and 0x2003 (device-service, the register the first ESI build
        // read and left SET — the second-clear path under test)
        m_esi_2003 = 8'h40;
        m_esi_2005 = 8'h02;
        force dut.i_aux_channel.hpd_irq = 1'b1;
        repeat (3) @(posedge clk100);
        release dut.i_aux_channel.hpd_irq;
        begin : wait_rt_service
            integer t0; t0 = $time;
            while ((esi_writes_post == 0 || esi3_writes_post == 0) &&
                   ($time - t0) < 10_000_000)
                #10_000;
        end
        if (esi_writes_post != 1) begin
            errors = errors + 1;
            $display("FAIL: runtime ESI service writes = %0d (want 1)", esi_writes_post);
        end else if (esi_write_val_post !== 8'h02) begin
            errors = errors + 1;
            $display("FAIL: runtime ESI ack value %02x (want 02)", esi_write_val_post);
        end else if (m_esi_2005 !== 8'h00) begin
            errors = errors + 1;
            $display("FAIL: ESI vector not cleared at the sink (%02x)", m_esi_2005);
        end else if (esi3_writes_post != 1 || esi3_write_val_post !== 8'h40 ||
                     m_esi_2003 !== 8'h00) begin
            errors = errors + 1;
            $display("FAIL: 0x2003 second clear bad (writes=%0d val=%02x residual=%02x)",
                     esi3_writes_post, esi3_write_val_post, m_esi_2003);
        end else
            $display("  ok: runtime ESI service — both vectors: W1C 0x2005=02 AND W1C 0x2003=40, both clear");

        #1_000_000;                     // one more ms: no extra acks, no drop
        if (esi_writes_post > 1) begin
            errors = errors + 1;
            $display("FAIL: %0d runtime ESI writes (want exactly 1)", esi_writes_post);
        end

        // 7. PERIODIC ESI POLL: a standing vector with NO hpd_irq (the
        // attention-lost scenario) must be found and cleared by the next
        // 1 Hz check; a poll with nothing pending must WRITE nothing.
        m_esi_2005 = 8'h08;             // standing vector, no attention
        er0 = esi_reads; ew0 = esi_writes_post;
        force_check;
        #3_000_000;
        if (esi_reads <= er0 || esi_writes_post != ew0 + 1 ||
            m_esi_2005 !== 8'h00) begin
            errors = errors + 1;
            $display("FAIL: standing vector not found/cleared by poll (reads %0d->%0d writes %0d->%0d resid=%02x)",
                     er0, esi_reads, ew0, esi_writes_post, m_esi_2005);
        end else
            $display("  ok: 1 Hz poll found and cleared a standing vector (no attention needed)");
        er0 = esi_reads; ew0 = esi_writes_post;
        force_check;                    // nothing pending now
        #3_000_000;
        if (esi_reads <= er0 || esi_writes_post != ew0) begin
            errors = errors + 1;
            $display("FAIL: idle poll behavior (reads %0d->%0d writes %0d->%0d; want read+, writes unchanged)",
                     er0, esi_reads, ew0, esi_writes_post);
        end else
            $display("  ok: idle poll reads the block, writes NOTHING");

        // 8. ESI FAILURE TOLERANCE: sink starts NACKing ESI — one failed
        // read must self-disable ESI (no teardown, no retry hammer)
        esi_nack_mode = 1'b1;
        er0 = esi_reads;
        force_check;
        #3_000_000;
        if (esi_reads != er0 + 1) begin
            errors = errors + 1;
            $display("FAIL: NACK'd ESI read count %0d->%0d (want exactly one attempt)", er0, esi_reads);
        end
        er0 = esi_reads;
        force_check;                    // ESI now off: no read at all
        #3_000_000;
        force_check;
        #3_000_000;
        if (esi_reads != er0) begin
            errors = errors + 1;
            $display("FAIL: ESI traffic continued after failure (%0d->%0d)", er0, esi_reads);
        end else
            $display("  ok: one NACK self-disables ESI — no retry hammer, link never torn down");
        esi_nack_mode = 1'b0;

        // 9. WEDGE DETECTOR discrimination (WEDGE_CLKS = 4 ms in sim).
        // (a) K:00 with 0x204 bit7 CLEAR (healthy hub clearing on read):
        //     sustained -> must NOT fire.
        r205 = 8'h00; r204 = 8'h01;
        repeat (4) begin force_check; #2_500_000; end
        if (dut.i_aux_channel.wedge_suspect_o !== 1'b0) begin
            errors = errors + 1;
            $display("FAIL: detector fired on K:00 with bit7 clear (kick-era false positive)");
        end else
            $display("  ok: K:00 alone (bit7 clear) does NOT fire the detector");
        // (b) the wedge signature: bit7 STUCK set + K:00 sustained -> fires
        r204 = 8'h81;
        repeat (4) begin force_check; #2_500_000; end
        if (dut.i_aux_channel.wedge_suspect_o !== 1'b1) begin
            errors = errors + 1;
            $display("FAIL: detector did not fire on the wedge signature");
        end else
            $display("  ok: sustained bit7-stuck + K:00 fires wedge_suspect");
        // (c) streaming resumes -> stands down
        r205 = 8'h01; r204 = 8'h01;
        force_check;
        #3_000_000;
        if (dut.i_aux_channel.wedge_suspect_o !== 1'b0) begin
            errors = errors + 1;
            $display("FAIL: detector did not stand down on streaming resume");
        end else
            $display("  ok: detector stands down when streaming resumes");

        if (le_drops != 0) begin
            errors = errors + 1;
            $display("FAIL: tx_link_established sampled low %0d time(s) while established", le_drops);
        end
        if (errors == 0)
            $display("PASS: polite attach%0s — paced presence, DEFER-tolerant EDID, ESI ack, gated training start, per-iteration CR writes, runtime ESI service, link stable",
                     giveup_mode ? " (giveup fallback)" : "");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

    initial begin
        #150_000_000;
        $display("FAIL: TB watchdog (state=%02x p0=%0d acks=%0d defers=%0d ls=%0d tps1=%0d)",
                 dut.i_aux_channel.state, presence0_reads, edid_acks,
                 edid_defers, ls_cnt, tps1_seen);
        $finish;
    end

endmodule
