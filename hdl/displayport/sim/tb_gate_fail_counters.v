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
    wire [11:0] debug_teardown;
    // drive the four lock inputs so the fail-mask can be checked
    // start with ONLY align clear, so the sticky fail-mask has a specific
    // expected value; an all-low start would OR in every bit immediately
    reg clk_lk = 1'b1, equ_lk = 1'b1, sym_lk = 1'b1, aln_lk = 1'b0;
    aux_channel #(.BLIND_SINK(0), .AFE_ADJUST(0)) dut (
        .clk(clk), .train_set_byte(16'h0606), .afe_busy(1'b0),
        .debug_pmod(), .debug_gate(debug_gate), .debug_teardown(debug_teardown),
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
        if (debug_teardown[11:8] !== 4'b0001) begin
            errors = errors + 1;
            $display("FAIL: align-only failure gave mask %04b (want 0001)", debug_teardown[11:8]);
        end else $display("  ok: align-only failure -> mask 0001");
        // now only SYMBOL clear -> mask accumulates bit1 as well (sticky OR)
        clk_lk = 1; equ_lk = 1; sym_lk = 0; aln_lk = 1;
        drive_gate_fail;
        if (debug_teardown[11:8] !== 4'b0011) begin
            errors = errors + 1;
            $display("FAIL: after symbol failure mask %04b (want 0011 sticky)", debug_teardown[11:8]);
        end else $display("  ok: symbol failure ORs in -> mask 0011 (sticky)");

        if (errors == 0)
            $display("PASS: counters track (2-bit == sat mod 4) AND the fail-mask names which lock bit was clear");
        else
            $display("FAIL: %0d error(s) — RTL bug reproduced in simulation", errors);
        $finish;
    end
    initial begin #500_000; $display("FAIL: TB watchdog timeout"); $finish; end
endmodule
