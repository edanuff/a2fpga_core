// tb_link_observe.v — LINK_CHECK=2 (observe) on aux_channel.
//
// Same scripted Manchester sink as tb_polite_attach.v (polite attach, ESI,
// AFE closed loop) with the a2p25 dp_test policy (ERRCNT_READ=0) and
// LINK_CHECK=2. Once established, the sink's periodic status reads
// (0x200 len 8) fail one class at a time — NACK, DEFER, silence (AUX
// timeout), short reply — plus a failed lock gate (0x204 align bit
// clear). Checks: tx_link_established never drops, the FSM returns to
// link_established, and exactly the matching debug_obs counter advances
// (once per check), with the check counter tracking reads.
//
// Generated from tb_polite_attach.v's sink; run (repo root):
//   iverilog -g2012 -o /tmp/tb_obs.vvp hdl/displayport/sim/tb_link_observe.v \
//        hdl/displayport/auxch/channel_managemnt.v hdl/displayport/auxch/aux_channel.v \
//        hdl/displayport/auxch/dp_aux_messages.v hdl/displayport/auxch/aux_interface.v \
//        hdl/displayport/auxch/link_signal_mgmt.v hdl/displayport/auxch/edid_decode.v \
//        hdl/displayport/auxch/dp_register_decode.v hdl/displayport/auxch/hotplug_decode.v \
//        hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_obs.vvp

`timescale 1ns/1ps

module tb_link_observe;

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
        .ERRCNT_READ    (0),
        .LINK_CHECK     (2),
        .WEDGE_BIT      (19),
        .WEDGE_PRELOAD  (32'h0002_0000), // 2^19-2^17 clks = ~3.9 ms sim fire
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
        .hpd_present_out      (),
        .debug_obs            (obs)
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
    reg       esi_nack_mode = 1'b0;
    integer   status_fault = 0;      // 0 ok,1 NACK,2 DEFER,3 silent,4 short
    wire [103:0] obs;
    reg       errcnt_nack_mode = 1'b0;   // sink NACKs 0x210 reads
    reg [7:0] r210 = 8'h00, r211 = 8'h80, r212 = 8'h00, r213 = 8'h80;
    integer   errcnt_reads = 0;      // sink NACKs ESI reads (failure-tolerance test)       // hub ready to grant CR — but only
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
            20'h00210: dpcd_read = r210;
            20'h00211: dpcd_read = r211;
            20'h00212: dpcd_read = r212;
            20'h00213: dpcd_read = r213;
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
                    if (req_addr == 20'h00210 && dlen == 4) begin
                        errcnt_reads = errcnt_reads + 1;
                        if (errcnt_nack_mode) begin
                            rbuf[0] = 8'h10;        // AUX_NACK
                            rlen = 1;
                            send_reply;
                        end else begin
                            rbuf[0] = 8'h00;
                            for (j = 0; j < dlen; j = j + 1)
                                rbuf[j+1] = dpcd_read(req_addr + j);
                            rlen = dlen + 1;
                            send_reply;
                        end
                    end else if (established_phase && status_fault != 0 &&
                                 req_addr == 20'h00200 && dlen == 8) begin
                        case (status_fault)
                        1: begin rbuf[0] = 8'h10; rlen = 1; send_reply; end
                        2: begin rbuf[0] = 8'h20; rlen = 1; send_reply; end
                        3: ;                               // silent: timeout
                        default: begin                     // short: 3 of 8
                            rbuf[0] = 8'h00;
                            for (j = 0; j < 3; j = j + 1)
                                rbuf[j+1] = dpcd_read(req_addr + j);
                            rlen = 4; send_reply;
                        end
                        endcase
                    end else if (req_addr == 20'h02003 && esi_nack_mode) begin
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
        // r205 left 00: the detector phases below reproduce the REAL
        // first-wedge profile (sink NEVER reported streaming) — the
        // ever_streamed gate false-negatived exactly here
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

    wire [15:0] o_chk   = obs[95:80];
    wire [15:0] o_short = obs[79:64];
    wire [15:0] o_nack  = obs[63:48];
    wire [15:0] o_defer = obs[47:32];
    wire [15:0] o_tmo   = obs[31:16];
    wire [15:0] o_gate  = obs[15:0];
    reg  [95:0] snap;
    localparam [7:0] LINK_ESTABLISHED = 8'h2E;

    // one check under a fault; field: 1 short,2 nack,3 defer,4 tmo,5 gate, 0 none
    integer f;
    task check_class(input integer fault, input [8*8-1:0] name, input integer field);
        begin
            status_fault = fault;
            snap = obs[95:0];
            force_check;
            if (fault == 3) #25_000_000;    // AUX reply timeout is ~20 ms
            else            #4_000_000;
            if (obs[95:80] != snap[95:80] + 16'd1) begin
                errors = errors + 1;
                $display("FAIL(%0s): checks %0d -> %0d (want +1)", name, snap[95:80], obs[95:80]);
            end
            for (f = 1; f < 6; f = f + 1)
                if (obs[95-16*f -: 16] != snap[95-16*f -: 16] + ((f == field) ? 16'd1 : 16'd0)) begin
                    errors = errors + 1;
                    $display("FAIL(%0s): counter %0d moved %0d -> %0d", name, f,
                             snap[95-16*f -: 16], obs[95-16*f -: 16]);
                end
            if (dut.i_aux_channel.state != LINK_ESTABLISHED) begin
                errors = errors + 1;
                $display("FAIL(%0s): FSM in %02x, not link_established", name,
                         dut.i_aux_channel.state);
            end
            $display("  %0s: chk=%0d short=%0d nack=%0d defer=%0d tmo=%0d gate=%0d hdr=%02x drops=%0d",
                     name, o_chk, o_short, o_nack, o_defer, o_tmo, o_gate, obs[103:96], le_drops);
            status_fault = 0;
        end
    endtask

    initial begin
        wait (tx_link_established);
        #300_000;
        established_phase = 1'b1;
        $display("  ok: ladder trained to link_established (t=%0t)", $time);
        #3_000_000;                         // let any attach-time traffic settle
        check_class(0, "clean",   0);
        check_class(1, "NACK",    2);
        check_class(2, "DEFER",   3);
        check_class(3, "silent",  4);
        check_class(4, "short",   1);
        r204 = 8'h00;                       // align lost: gate fails
        check_class(0, "gate",    5);
        r204 = 8'h01;
        check_class(0, "clean2",  0);
        check_class(1, "NACK2",   2);
        if (obs[103:96] !== 8'h10) begin
            errors = errors + 1;
            $display("FAIL: last header %02x (want 10)", obs[103:96]);
        end
        if (le_drops != 0) begin
            errors = errors + 1;
            $display("FAIL: tx_link_established sampled low %0d time(s)", le_drops);
        end
        if (errors == 0)
            $display("PASS: LINK_CHECK=2 observe — every failure class counted once, link held");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

    initial begin
        #250_000_000;
        $display("FAIL: TB watchdog (state=%02x)", dut.i_aux_channel.state);
        $finish;
    end

endmodule
