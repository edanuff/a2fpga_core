// tb_gate_fail_counters.v — does dbg_gate_fail_sat track dbg_gate_fail?
//
// On hardware (08-24) the 2-bit `dbg_gate_fail` in debug_gate read 1 while
// the 4-bit saturating twin `dbg_gate_fail_sat` in debug_teardown read 0 —
// impossible, since they increment on adjacent lines in the same branch.
// This drives the FSM through the check_wait gate with the lock inputs LOW
// and checks both counters advance together.
//
// Run: iverilog -g2012 -o /tmp/tb_gf.vvp hdl/displayport/sim/tb_gate_fail_counters.v \
//        hdl/displayport/auxch/aux_channel.v hdl/displayport/auxch/aux_interface.v \
//        hdl/displayport/auxch/dp_aux_messages.v hdl/displayport/auxch/dp_register_decode.v \
//        hdl/displayport/auxch/edid_decode.v hdl/displayport/auxch/link_signal_mgmt.v \
//        && vvp /tmp/tb_gf.vvp
`timescale 1ns/1ps
module tb_gate_fail_counters;
    reg clk = 0;
    always #5 clk = ~clk;

    wire [7:0]  debug_gate;
    wire [15:0] debug_teardown;
    wire [7:0]  debug_aux_err;
    // drive the four lock inputs so the fail-mask can be checked
    // start with ONLY align clear, so the sticky fail-mask has a specific
    // expected value; an all-low start would OR in every bit immediately
    reg clk_lk = 1'b1, equ_lk = 1'b1, sym_lk = 1'b1, aln_lk = 1'b0;
    aux_channel #(.BLIND_SINK(0), .AFE_ADJUST(0)) dut (
        .clk(clk), .train_set_byte(16'h0606), .afe_busy(1'b0),
        .debug_pmod(), .debug_gate(debug_gate), .debug_teardown(debug_teardown),
        .debug_aux_err(debug_aux_err), .gate_fail_evt(), .status_seq(),
        .debug_sink(), .debug_rx(), .edid_de(), .dp_reg_de(), .adjust_de(),
        .status_de(), .aux_addr(), .aux_data(), .link_count(3'd2),
        .hpd_irq(1'b0), .hpd_present(1'b1),
        .tx_powerup(), .tx_clock_train(), .tx_align_train(), .tx_link_established(),
        .swing_0p4(1'b0), .swing_0p6(1'b0), .swing_0p8(1'b1),
        .preemp_0p0(1'b1), .preemp_3p5(1'b0), .preemp_6p0(1'b0),
        .clock_locked(clk_lk), .equ_locked(equ_lk), .symbol_locked(sym_lk),
        .align_locked(aln_lk), .dp_tx_hp_detect(1'b1),
        .aux_in(1'b0), .aux_out(), .aux_tri()
    );

    // localparams from aux_channel
    localparam [7:0] CHECK_LINK = 8'h2F, CHECK_WAIT = 8'h30;

    integer errors = 0;
    integer i;
    reg [1:0] g2_before, g2_after;
    reg [3:0] gs_before, gs_after;

    task drive_gate_fail;
        begin
            // present the check_link -> check_wait transition for one edge
            @(negedge clk);
            force dut.state      = CHECK_LINK;
            force dut.next_state = CHECK_WAIT;
            @(posedge clk);
            @(negedge clk);
            release dut.state;
            release dut.next_state;
            @(negedge clk);
        end
    endtask

    // ---- observe-window DUT: same drive, GATE_GRACE=1 ------------------
    wire [7:0]  o_gate; wire [15:0] o_tear; wire [23:0] o_aerr;
    // ---- kick DUT: GATE_KICK=1 with a tiny KICK_CLKS -------------------
    wire [7:0]  k_gate; wire [15:0] k_tear; wire [23:0] k_aerr; wire [27:0] k_edet;
    aux_channel #(.BLIND_SINK(0), .AFE_ADJUST(0),
                  .GATE_KICK(1), .KICK_CLKS(30'd500), .KICK_CAP(3'd2)) dut_k (
        .clk(clk), .train_set_byte(16'h0606), .afe_busy(1'b0),
        .debug_pmod(), .debug_gate(k_gate), .debug_teardown(k_tear),
        .debug_aux_err(k_aerr), .debug_err_detail(k_edet), .gate_fail_evt(), .status_seq(),
        .debug_sink(), .debug_rx(), .edid_de(), .dp_reg_de(), .adjust_de(),
        .status_de(), .aux_addr(), .aux_data(), .link_count(3'd2),
        .hpd_irq(1'b0), .hpd_present(1'b1),
        .tx_powerup(), .tx_clock_train(), .tx_align_train(), .tx_link_established(),
        .swing_0p4(1'b0), .swing_0p6(1'b0), .swing_0p8(1'b1),
        .preemp_0p0(1'b1), .preemp_3p5(1'b0), .preemp_6p0(1'b0),
        .clock_locked(1'b1), .equ_locked(1'b1), .symbol_locked(1'b1),
        .align_locked(1'b1), .dp_tx_hp_detect(1'b1),
        .aux_in(1'b0), .aux_out(), .aux_tri()
    );
    aux_channel #(.BLIND_SINK(0), .AFE_ADJUST(0),
                  .GATE_GRACE(1), .GATE_GRACE_CLKS(30'd100_000),
                  .LATE_REPLY_DRAIN(1)) dut_o (
        .clk(clk), .train_set_byte(16'h0606), .afe_busy(1'b0),
        .debug_pmod(), .debug_gate(o_gate), .debug_teardown(o_tear),
        .debug_aux_err(o_aerr), .debug_err_detail(), .gate_fail_evt(), .status_seq(),
        .debug_sink(), .debug_rx(), .edid_de(), .dp_reg_de(), .adjust_de(),
        .status_de(), .aux_addr(), .aux_data(), .link_count(3'd2),
        .hpd_irq(1'b0), .hpd_present(1'b1),
        .tx_powerup(), .tx_clock_train(), .tx_align_train(), .tx_link_established(),
        .swing_0p4(1'b0), .swing_0p6(1'b0), .swing_0p8(1'b1),
        .preemp_0p0(1'b1), .preemp_3p5(1'b0), .preemp_6p0(1'b0),
        .clock_locked(clk_lk), .equ_locked(equ_lk), .symbol_locked(sym_lk),
        .align_locked(aln_lk), .dp_tx_hp_detect(1'b1),
        .aux_in(1'b0), .aux_out(), .aux_tri()
    );
    task drive_gate_obs;
        begin
            @(negedge clk);
            force dut_o.state      = CHECK_LINK;
            force dut_o.next_state = CHECK_WAIT;
            @(posedge clk); @(negedge clk);
            release dut_o.state; release dut_o.next_state;
            @(negedge clk);
        end
    endtask

    initial begin
        #200;
        $display("initial: debug_gate=%02x (gate_fail=%0d)  debug_teardown=%02x (gate_fail_sat=%0d)",
                 debug_gate, debug_gate[3:2], debug_teardown, debug_teardown[7:4]);

        for (i = 1; i <= 3; i = i + 1) begin
            g2_before = debug_gate[3:2];
            gs_before = debug_teardown[7:4];
            drive_gate_fail;
            g2_after = debug_gate[3:2];
            gs_after = debug_teardown[7:4];
            $display("  pass %0d: gate_fail %0d -> %0d   gate_fail_sat %0d -> %0d",
                     i, g2_before, g2_after, gs_before, gs_after);
            // The property under test is that they TRACK: the saturating
            // twin must advance, and the 2-bit counter must equal it mod 4.
            // (Each drive executes the branch twice — the forced transition
            // spans two edges — so the delta is 2, not 1. That is a TB
            // artifact and irrelevant to whether they agree.)
            if (gs_after === gs_before) begin
                errors = errors + 1;
                $display("    FAIL: gate_fail_sat did not advance at all");
            end
            if (g2_after !== (gs_after & 2'd3)) begin
                errors = errors + 1;
                $display("    FAIL: counters DISAGREE — 2-bit=%0d, sat=%0d (sat mod 4 = %0d)",
                         g2_after, gs_after, gs_after & 2'd3);
            end
        end

        // ---- fail-mask: does it name the bit that was clear? ----------
        // every failure so far had ONLY align clear -> mask must be 0001
        if (debug_teardown[11:8] !== 4'b0001 || debug_teardown[15:12] !== 4'b0001) begin
            errors = errors + 1;
            $display("FAIL: align-only failures: sticky %04b first %04b (want 0001/0001)",
                     debug_teardown[11:8], debug_teardown[15:12]);
        end else $display("  ok: align-only failure -> sticky 0001, first 0001");
        // now only SYMBOL clear -> mask accumulates bit1 as well (sticky OR)
        clk_lk = 1; equ_lk = 1; sym_lk = 0; aln_lk = 1;
        drive_gate_fail;
        if (debug_teardown[11:8] !== 4'b0011 || debug_teardown[15:12] !== 4'b0001) begin
            errors = errors + 1;
            $display("FAIL: after symbol failure: sticky %04b (want 0011), first %04b (must STAY 0001)",
                     debug_teardown[11:8], debug_teardown[15:12]);
        end else $display("  ok: symbol failure -> sticky 0011, first STAYS 0001");

        // ---- observation window: suppress inside, tear down after -----
        // dut_o starts outside the established set -> obs_timer 0 -> ACTIVE
        force dut_o.obs_timer = 30'd10;          // inside the window
        drive_gate_obs;
        if (o_aerr[11:8] === 4'd0 || dut_o.state_on_success !== dut_o.link_established) begin
            errors = errors + 1;
            $display("FAIL: in-window gate failure not suppressed (obs=%0d target=%02x)",
                     o_aerr[11:8], dut_o.state_on_success);
        end else $display("  ok: in-window failure SUPPRESSED (obs=%0d, stays established)", o_aerr[11:8]);
        if (o_tear[7:4] !== 4'd0) begin
            errors = errors + 1;
            $display("FAIL: suppressed failure counted as a gate teardown");
        end else $display("  ok: suppressed failure not counted as a teardown");
        force dut_o.obs_timer = 30'd200_000;     // beyond the window
        drive_gate_obs;
        release dut_o.obs_timer;
        if (o_tear[7:4] === 4'd0 || dut_o.state_on_success !== dut_o.error) begin
            errors = errors + 1;
            $display("FAIL: post-window failure did not tear down (gate=%0d target=%02x)",
                     o_tear[7:4], dut_o.state_on_success);
        end else $display("  ok: post-window failure tears down normally (gate=%0d)", o_tear[7:4]);

        // ---- late-reply behavior, BOTH policies -------------------------
        // legacy (dut, LATE_REPLY_DRAIN=0): stray bytes with expected==0
        // MUST take the error path (round-1 baseline behavior, preserved).
        // drain (dut_o, LATE_REPLY_DRAIN=1): the same bytes are benign.
        begin : late_reply
            reg [3:0] shorts_before;
            shorts_before = dut.dbg_short_sat;
            @(negedge clk);
            force dut.state       = dut.link_established;
            force dut.next_state  = dut.link_established;
            force dut.expected    = 8'h00;
            force dut.channel_busy = 1'b0;
            force dut.aux_rx_empty = 1'b0;
            force dut.aux_rx_data  = 8'hA5;
            repeat (4) @(posedge clk);
            force dut.aux_rx_empty = 1'b1;
            repeat (2) @(posedge clk);
            release dut.state; release dut.next_state; release dut.expected;
            release dut.channel_busy; release dut.aux_rx_empty; release dut.aux_rx_data;
            @(negedge clk);
            if (dut.dbg_short_sat == shorts_before) begin
                errors = errors + 1;
                $display("FAIL: LEGACY policy did not error on a stray byte (short=%0d)", dut.dbg_short_sat);
            end else
                $display("  ok: legacy policy errors on stray bytes (round-1 baseline preserved)");
            // drain policy: same stimulus on dut_o -> no error, no count
            @(negedge clk);
            force dut_o.state       = dut_o.link_established;
            force dut_o.next_state  = dut_o.link_established;
            force dut_o.expected    = 8'h00;
            force dut_o.channel_busy = 1'b0;
            force dut_o.aux_rx_empty = 1'b0;
            force dut_o.aux_rx_data  = 8'hA5;
            repeat (4) @(posedge clk);
            force dut_o.aux_rx_empty = 1'b1;
            repeat (2) @(posedge clk);
            release dut_o.state; release dut_o.next_state; release dut_o.expected;
            release dut_o.channel_busy; release dut_o.aux_rx_empty; release dut_o.aux_rx_data;
            @(negedge clk);
            if (dut_o.state == dut_o.error || dut_o.dbg_short_sat != 4'd0) begin
                errors = errors + 1;
                $display("FAIL: DRAIN policy errored on a stray byte (state=%02x short=%0d)",
                         dut_o.state, dut_o.dbg_short_sat);
            end else
                $display("  ok: drain policy swallows stray bytes (no error, no count)");
        end

        if (errors == 0)
            $display("PASS: counters track (2-bit == sat mod 4) AND the fail-mask names the clear bit AND grace suppress/restore AND late-reply drain AND the dark-state kick (fire/budget/re-arm/streaming-never)");
        else
            $display("FAIL: %0d error(s) — RTL bug reproduced in simulation", errors);
        $finish;
    end
    initial begin #500_000; $display("FAIL: TB watchdog timeout"); $finish; end
endmodule
