///////////////////////////////////////////////////////////////////////////
// tb_train_recover — TRAIN_RECOVER + EDID-session-policy testbench, built
// from the 08-28 AD3 capture of the 60K step-6b stall (the die-sensitive
// polite-attach regression): the hub's reply to the FIRST status read
// after TPS1 came back short/garbled (or not at all — the 20.16 ms round
// gap = the AUX reply timeout), the ladder error-restarted the ENTIRE
// polite ceremony, power-cycling TX each round; 225 ceremonies, zero
// second status polls, no lock ever. The legacy ladder survived the same
// marginality via cheap restarts with TX held — this TB proves the polite
// ladder now does too.
//
// DUT config = the 60K die package flavor: POLITE_ATTACH=1, IRQ_SERVICE=2,
// AFE off (afe_busy tied 0, lane-set byte fixed 0x06) — the exact
// parameter combination that failed on hardware.
//
// Modes:
//   (default)  hub GARBLES (truncates) the first 3 status replies after
//              TPS1, then answers clean. Checks: paced same-read retries,
//              ONE ceremony (one LINK_BW_SET write, one EDID pass), TX
//              powerup never drops between first TPS1 and lock, link
//              locks. Then two more phases:
//                C: mid-session link loss (check_link sees CR gone) —
//                   full ceremony re-runs (BW write #2) but EDID is
//                   SKIPPED (session policy) and the link re-locks.
//                D: HPD drop 3 ms (session boundary) — re-attach re-runs
//                   EDID (addr-phase write #2) and re-locks.
//   +silent    hub sends NO reply to the first 2 status reads (each burns
//              the 20 ms AUX reply timeout) — the timeout recovery path.
//   +exhaust   hub garbles 10 replies (> TRAIN_RETRY_CAP=8): the retry
//              budget exhausts and the ladder must CHEAP-RESTART at
//              clock_training (TPS1 written a 2nd time, LINK_BW_SET still
//              written only once) and then lock when replies turn clean.
//
// Run (from repo root):
//   iverilog -g2012 -o /tmp/tb_trec.vvp \
//        hdl/displayport/sim/tb_train_recover.v \
//        hdl/displayport/auxch/channel_managemnt.v \
//        hdl/displayport/auxch/aux_channel.v \
//        hdl/displayport/auxch/dp_aux_messages.v \
//        hdl/displayport/auxch/aux_interface.v \
//        hdl/displayport/auxch/link_signal_mgmt.v \
//        hdl/displayport/auxch/edid_decode.v \
//        hdl/displayport/auxch/dp_register_decode.v \
//        hdl/displayport/auxch/hotplug_decode.v \
//        && vvp /tmp/tb_trec.vvp && vvp /tmp/tb_trec.vvp +silent \
//        && vvp /tmp/tb_trec.vvp +exhaust
///////////////////////////////////////////////////////////////////////////

`timescale 1ns/1ps

module tb_train_recover;

    localparam HB = 500;               // AUX half-bit, ns (1 Mbps Manchester)

    reg clk100 = 1'b0;
    always #5 clk100 = ~clk100;

    integer errors = 0;
    integer bad_status_budget = 3;     // replies to corrupt (default: garble 3)
    reg     silent_mode  = 1'b0;
    reg     exhaust_mode = 1'b0;

    // ------------------------------------------------------------------
    // DUT: the 60K flavor — POLITE_ATTACH=1, IRQ_SERVICE=2, AFE OFF
    // ------------------------------------------------------------------
    wire auxch_out, auxch_tri;
    reg  auxch_in = 1'b0;
    reg  hpd_pin  = 1'b1;
    wire tx_clock_train, tx_align_train, tx_link_established;
    wire [3:0] tx_powerup_channel;

    channel_management #(
        .LINK_RATE_MBPS (2700),
        .BLIND_SINK     (0),
        .IRQ_SERVICE    (2),
        .POLITE_ATTACH  (1),
        .TRAIN_RECOVER  (1),
        .WEDGE_BIT      (19),
        .WEDGE_PRELOAD  (32'h0002_0000),
        .AFE_ADJUST     (0)
    ) dut (
        .clk100               (clk100),
        .train_set_byte       (),
        .adjust_evt           (),
        .afe_busy             (1'b0),
        .debug                (),
        .debug_rx             (),
        .debug_locks          (),
        .debug_gate           (),
        .debug_sink           (),
        .debug_adjust         (),
        .debug_chstate        (),
        .debug_caps           (),
        .hpd                  (hpd_pin),
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
    // Scripted DPCD / hub model
    // ------------------------------------------------------------------
    reg [7:0] m_sink_count = 8'h41;    // ready immediately (speed)
    reg [7:0] r202 = 8'h00;
    reg [7:0] r204 = 8'h00;
    reg [7:0] r205 = 8'h00;
    reg [7:0] r206 = 8'hBB;
    reg [7:0] m_esi_2005 = 8'h00;
    reg [7:0] m_esi_2003 = 8'h00;
    reg       cr_armed = 1'b0;
    integer   clean_polls = 0;         // clean status replies served

    function [7:0] dpcd_read(input [19:0] a);
        case (a)
            20'h00000: dpcd_read = 8'h11;
            20'h00001: dpcd_read = 8'h0A;
            20'h00002: dpcd_read = 8'h82;
            20'h00200: dpcd_read = m_sink_count;
            20'h00201: dpcd_read = 8'h00;
            20'h00202: dpcd_read = r202;
            20'h00204: dpcd_read = r204;
            20'h00205: dpcd_read = r205;
            20'h00206: dpcd_read = r206;
            20'h02005: dpcd_read = m_esi_2005;
            default:   dpcd_read = 8'h00;
        endcase
    endfunction

    // records
    integer bw_writes        = 0;   // 0x100 LINK_BW_SET writes  (= ceremony count)
    integer tps1_writes      = 0;   // 0x102 pattern-1 writes    (= training entries)
    integer i2c_addr_writes  = 0;   // EDID address phases       (= EDID passes)
    integer i2c_reads        = 0;
    integer edid_acks        = 0;
    integer defers_this_block = 0;
    integer status_reads     = 0;   // 0x200 len8 reads
    integer bad_status_served = 0;  // replies corrupted/withheld
    real    t_first_tps1     = -1;
    real    t_last_bad       = -1;
    real    retry_min_dt     = 1e12; // spacing of the read AFTER a bad reply
    reg     last_status_bad  = 1'b0;
    reg [7:0] last_pattern   = 8'h00;

    // TX powerup must HOLD from first TPS1 to lock (sampled at posedge —
    // zero-width NBA glitches are sim artifacts)
    integer powerup_drops = 0;
    reg     watch_powerup = 1'b0;
    always @(posedge clk100)
        if (watch_powerup && !tx_link_established && tx_powerup_channel[0] == 1'b0) begin
            powerup_drops = powerup_drops + 1;
            if (powerup_drops < 4)
                $display("FAIL: tx_powerup dropped during training recovery (t=%0t state=%02x)",
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
                    if (req_addr == 20'h00200 && dlen == 8) begin
                        status_reads = status_reads + 1;
                        if (last_status_bad && t_last_bad >= 0 &&
                            ($realtime - t_last_bad) < retry_min_dt)
                            retry_min_dt = $realtime - t_last_bad;
                        if (bad_status_served < bad_status_budget &&
                            tps1_writes > 0) begin
                            bad_status_served = bad_status_served + 1;
                            last_status_bad   = 1'b1;
                            t_last_bad        = $realtime;
                            if (silent_mode) begin
                                $display("  [sink] status read #%0d: SILENT (reply withheld, t=%0t)",
                                         status_reads, $time);
                                // no reply at all
                            end else begin
                                $display("  [sink] status read #%0d: GARBLED (3-byte truncation, t=%0t)",
                                         status_reads, $time);
                                rbuf[0] = 8'h00;
                                rbuf[1] = 8'h41; rbuf[2] = 8'h00; rbuf[3] = 8'h00;
                                rlen = 4;       // expected 9 — short read
                                send_reply;
                            end
                        end else begin
                            last_status_bad = 1'b0;
                            clean_polls = clean_polls + 1;
                            rbuf[0] = 8'h00;
                            for (j = 0; j < dlen; j = j + 1)
                                rbuf[j+1] = dpcd_read(req_addr + j);
                            rlen = dlen + 1;
                            send_reply;
                        end
                    end else if (req_addr == 20'h02003) begin
                        rbuf[0] = 8'h00;
                        for (j = 0; j < dlen; j = j + 1)
                            rbuf[j+1] = 8'h00;
                        rbuf[1] = m_esi_2003;
                        rbuf[3] = m_esi_2005;
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
                    if (req_addr == 20'h02003) m_esi_2003 = m_esi_2003 & ~req[4];
                    if (req_addr == 20'h02005) m_esi_2005 = m_esi_2005 & ~req[4];
                    if (req_addr == 20'h00100) begin
                        bw_writes = bw_writes + 1;
                        $display("  [sink] LINK_BW_SET (ceremony #%0d, t=%0t)", bw_writes, $time);
                    end
                    if (req_addr == 20'h00103) begin
                        if (cr_armed && r202 == 8'h00) begin
                            r202 = 8'h11;
                            $display("  [sink] CR granted ON lane-set write (t=%0t)", $time);
                        end
                    end
                    if (req_addr == 20'h00102) begin
                        last_pattern = req[4];
                        if (req[4][3:0] == 4'h1) begin
                            tps1_writes = tps1_writes + 1;
                            if (t_first_tps1 < 0) t_first_tps1 = $realtime;
                            $display("  [sink] TPS1 (entry #%0d, t=%0t)", tps1_writes, $time);
                        end
                        if (req[4][3:0] == 4'h2) begin
                            r202 = 8'h77;
                            r204 = 8'h01;
                            $display("  [sink] TPS2 -> EQ granted (t=%0t)", $time);
                        end
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
                    if (defers_this_block < 1) begin
                        defers_this_block = defers_this_block + 1;
                        rbuf[0] = 8'h20;                // AUX_DEFER
                        rlen = 1;
                        send_reply;
                    end else begin
                        defers_this_block = 0;
                        edid_acks = edid_acks + 1;
                        rbuf[0] = 8'h00;
                        for (j = 0; j < dlen; j = j + 1)
                            rbuf[j+1] = edid_acks*16 + j;
                        rlen = dlen + 1;
                        send_reply;
                    end
                end
                default: begin
                    rbuf[0] = 8'h00;
                    rlen = 1;
                    send_reply;
                end
                endcase
            end
        end
    endtask

    // CR arms once two clean status polls have been served (write-cued
    // grant happens on the next lane-set write, IT6563-style)
    always @(clean_polls)
        if (clean_polls >= 2) cr_armed = 1'b1;

    // trigger an on-demand periodic check (the real timer is 1 s)
    task force_check;
        begin
            force dut.i_aux_channel.link_check_now = 1'b1;
            repeat (2) @(posedge clk100);
            release dut.i_aux_channel.link_check_now;
        end
    endtask

    // fire the 4 s ladder-retry watchdog on demand (the real post-teardown
    // restart path — hardware recovers in one watchdog period)
    task force_retry;
        begin
            force dut.i_aux_channel.retry_now = 1'b1;
            repeat (2) @(posedge clk100);
            release dut.i_aux_channel.retry_now;
        end
    endtask

    // ------------------------------------------------------------------
    // Main check sequence
    // ------------------------------------------------------------------
    integer bw0, edid0, reads0;
    initial begin
        if ($test$plusargs("silent")) begin
            silent_mode = 1'b1;
            bad_status_budget = 2;      // two 20 ms timeouts
        end
        if ($test$plusargs("exhaust")) begin
            exhaust_mode = 1'b1;
            bad_status_budget = 10;     // > TRAIN_RETRY_CAP (8)
        end

        // arm the powerup monitor at the first TPS1
        wait (tps1_writes > 0);
        watch_powerup = 1'b1;

        wait (tx_link_established);
        watch_powerup = 1'b0;
        #300_000;
        $display("  ok: trained to link_established through %0d bad status replies (t=%0t)",
                 bad_status_served, $time);

        // A1. the hub actually served the scripted corruption
        if (bad_status_served != bad_status_budget) begin
            errors = errors + 1;
            $display("FAIL: served %0d bad status replies (script wanted %0d)",
                     bad_status_served, bad_status_budget);
        end
        // A2. ONE ceremony, ONE EDID pass — the entire regression
        if (bw_writes != 1) begin
            errors = errors + 1;
            $display("FAIL: %0d LINK_BW_SET writes — ceremony re-ran (the 60K stall signature)", bw_writes);
        end else
            $display("  ok: exactly ONE ceremony — no full restart on bad status replies");
        if (i2c_addr_writes != 1) begin
            errors = errors + 1;
            $display("FAIL: %0d EDID address phases (want 1)", i2c_addr_writes);
        end else
            $display("  ok: exactly ONE EDID pass (8 blocks: %0d acks, %0d reads)",
                     edid_acks, i2c_reads);
        // A3. the read was RETRIED (more status reads than corruptions)
        if (status_reads < bad_status_served + 2) begin
            errors = errors + 1;
            $display("FAIL: only %0d status reads for %0d bad replies — no retry loop",
                     status_reads, bad_status_served);
        end else
            $display("  ok: status read retried (%0d reads, %0d bad)",
                     status_reads, bad_status_served);
        // A4. paced retries (>=600 us after a failed read; timeout mode
        // paces itself with the 20 ms reply window)
        if (!silent_mode && retry_min_dt < 600_000) begin
            errors = errors + 1;
            $display("FAIL: retry %.0f us after a garbled reply (want >=600 us)",
                     retry_min_dt/1e3);
        end else
            $display("  ok: retries paced (min %.0f us after a bad reply)",
                     silent_mode ? (retry_min_dt/1e3) : (retry_min_dt/1e3));
        // A5. TX held up throughout recovery
        if (powerup_drops != 0) begin
            errors = errors + 1;
            $display("FAIL: tx_powerup dropped %0d time(s) between TPS1 and lock", powerup_drops);
        end else
            $display("  ok: tx_powerup HELD from first TPS1 to lock");
        // A6. exhaust mode: budget ran out -> cheap restart at
        // clock_training (TPS1 again) without a new ceremony
        if (exhaust_mode) begin
            if (tps1_writes < 2) begin
                errors = errors + 1;
                $display("FAIL(exhaust): %0d TPS1 writes — no cheap retrain after budget exhaustion",
                         tps1_writes);
            end else if (bw_writes != 1) begin
                errors = errors + 1;
                $display("FAIL(exhaust): cheap restart re-ran the ceremony");
            end else
                $display("  ok(exhaust): budget exhausted -> retrain from clock_training (%0d TPS1s, still 1 ceremony)",
                         tps1_writes);
        end

        if (!silent_mode && !exhaust_mode) begin
            // ----------------------------------------------------------
            // C. mid-session link loss: full ceremony re-runs but EDID is
            // skipped (session policy)
            // ----------------------------------------------------------
            bw0 = bw_writes; edid0 = i2c_addr_writes; reads0 = i2c_reads;
            r202 = 8'h00; r204 = 8'h00; cr_armed = 1'b0; clean_polls = 0;
            bad_status_budget = 0;      // clean replies from here on
            bad_status_served = 0;
            force_check;
            begin : wait_drop1
                integer t0; t0 = $time;
                while (tx_link_established && ($time - t0) < 20_000_000)
                    #10_000;
            end
            if (tx_link_established) begin
                errors = errors + 1;
                $display("FAIL(C): link never dropped after CR loss + check");
            end
            force_retry;                // hardware: the 4 s watchdog restart
            wait_for_relock(200_000_000);
            if (bw_writes != bw0 + 1) begin
                errors = errors + 1;
                $display("FAIL(C): %0d ceremonies after link loss (want %0d)", bw_writes, bw0+1);
            end else if (i2c_addr_writes != edid0 || i2c_reads != reads0) begin
                errors = errors + 1;
                $display("FAIL(C): EDID re-ran on a mid-session ceremony (addr %0d->%0d reads %0d->%0d)",
                         edid0, i2c_addr_writes, reads0, i2c_reads);
            end else
                $display("  ok: link loss -> ONE fresh ceremony, EDID SKIPPED (session policy), re-locked");

            // ----------------------------------------------------------
            // D. HPD drop = session boundary: EDID re-arms
            // ----------------------------------------------------------
            bw0 = bw_writes; edid0 = i2c_addr_writes;
            // production policy: HPD drop does NOT tear the link down
            // (flap storms) — it only re-arms the EDID session. Model a
            // real unplug: presence lost 3 ms, then a fresh hot-plug
            // whose first check finds the link gone.
            hpd_pin = 1'b0;
            #3_000_000;                 // 3 ms low: disconnect-class
            if (!tx_link_established) begin
                errors = errors + 1;
                $display("FAIL(D): HPD drop tore the link down (flap-storm policy violated)");
            end
            hpd_pin = 1'b1;
            #3_000_000;                 // present re-qualifies (2 ms)
            r202 = 8'h00; r204 = 8'h00; cr_armed = 1'b0; clean_polls = 0;
            force_check;                // check finds CR gone -> teardown
            begin : wait_drop2
                integer t0; t0 = $time;
                while (tx_link_established && ($time - t0) < 20_000_000)
                    #10_000;
            end
            force_retry;                // watchdog restart
            wait_for_relock(200_000_000);
            if (i2c_addr_writes != edid0 + 1) begin
                errors = errors + 1;
                $display("FAIL(D): EDID passes %0d after HPD session boundary (want %0d)",
                         i2c_addr_writes, edid0+1);
            end else
                $display("  ok: HPD drop re-armed the session — EDID re-ran on the fresh hot-plug, re-locked");
        end

        if (errors == 0)
            $display("PASS: train-recover%0s — paced status retries, TX held, single ceremony, session-scoped EDID",
                     silent_mode ? " (+silent/timeout)" :
                     exhaust_mode ? " (+exhaust/cheap-restart)" : "");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

    task wait_for_relock(input integer budget_ns);
        integer t0;
        begin
            t0 = $time;
            while (!tx_link_established && ($time - t0) < budget_ns)
                #100_000;
            if (!tx_link_established) begin
                errors = errors + 1;
                $display("FAIL: link did not re-establish within %0d ms (state=%02x)",
                         budget_ns/1_000_000, dut.i_aux_channel.state);
            end
            #300_000;
        end
    endtask

    initial begin
        #400_000_000;
        $display("FAIL: TB watchdog (state=%02x bw=%0d tps1=%0d status=%0d bad=%0d edid_addr=%0d le=%0b)",
                 dut.i_aux_channel.state, bw_writes, tps1_writes,
                 status_reads, bad_status_served, i2c_addr_writes,
                 tx_link_established);
        $finish;
    end

endmodule
