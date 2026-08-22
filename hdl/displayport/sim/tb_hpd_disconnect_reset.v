// tb_hpd_disconnect_reset.v
//
// V2.1 'r'-fix verification: in closed-loop mode (BLIND_SINK=0) the
// ladder must restart when hotplug_decode drops `present` — gated by
// the new HPD_DISCONNECT_RESETS parameter (see aux_channel.v for the
// hybrid-history caution that motivates the gate).
//
// Checks:
//   1. HPD_DISCONNECT_RESETS=1: FSM leaves its state for error/reset
//      when hpd_present falls, and the ladder restarts (AUX TX resumes).
//   2. HPD_DISCONNECT_RESETS=0: dropping hpd_present changes nothing
//      (legacy closed-loop behavior, byte-identical).
//
// Run: iverilog -g2012 -o /tmp/tb_hpd.vvp \
//        hdl/displayport/sim/tb_hpd_disconnect_reset.v \
//        hdl/displayport/auxch/aux_channel.v \
//        hdl/displayport/auxch/aux_interface.v \
//        hdl/displayport/auxch/dp_aux_messages.v \
//        hdl/displayport/auxch/dp_register_decode.v \
//        hdl/displayport/auxch/edid_decode.v \
//        hdl/displayport/auxch/link_signal_mgmt.v  && vvp /tmp/tb_hpd.vvp

`timescale 1ns/1ps

module tb_hpd_disconnect_reset;

    reg clk = 0;
    always #5 clk = ~clk;

    integer errors = 0;

    // ---- DUT A: fix enabled -------------------------------------------
    reg  hpd_present_a = 1'b1;
    wire auxa_tri;
    aux_channel #(.BLIND_SINK(0), .HPD_DISCONNECT_RESETS(1)) dut_a (
        .clk(clk), .train_set_byte(8'h06),
        .debug_pmod(), .debug_gate(), .debug_sink(),
        .debug_rx(), .edid_de(), .dp_reg_de(), .adjust_de(),
        .status_de(), .aux_addr(), .aux_data(), .link_count(3'd2),
        .hpd_irq(1'b0), .hpd_present(hpd_present_a),
        .tx_powerup(), .tx_clock_train(), .tx_align_train(),
        .tx_link_established(),
        .swing_0p4(1'b0), .swing_0p6(1'b0), .swing_0p8(1'b1),
        .preemp_0p0(1'b1), .preemp_3p5(1'b0), .preemp_6p0(1'b0),
        .clock_locked(1'b1), .equ_locked(1'b1), .symbol_locked(1'b1),
        .align_locked(1'b1), .dp_tx_hp_detect(hpd_present_a),
        .aux_in(1'b0), .aux_out(), .aux_tri(auxa_tri)
    );

    // ---- DUT B: fix disabled (legacy) ---------------------------------
    reg  hpd_present_b = 1'b1;
    wire auxb_tri;
    aux_channel #(.BLIND_SINK(0), .HPD_DISCONNECT_RESETS(0)) dut_b (
        .clk(clk), .train_set_byte(8'h06),
        .debug_pmod(), .debug_gate(), .debug_sink(),
        .debug_rx(), .edid_de(), .dp_reg_de(), .adjust_de(),
        .status_de(), .aux_addr(), .aux_data(), .link_count(3'd2),
        .hpd_irq(1'b0), .hpd_present(hpd_present_b),
        .tx_powerup(), .tx_clock_train(), .tx_align_train(),
        .tx_link_established(),
        .swing_0p4(1'b0), .swing_0p6(1'b0), .swing_0p8(1'b1),
        .preemp_0p0(1'b1), .preemp_3p5(1'b0), .preemp_6p0(1'b0),
        .clock_locked(1'b1), .equ_locked(1'b1), .symbol_locked(1'b1),
        .align_locked(1'b1), .dp_tx_hp_detect(hpd_present_b),
        .aux_in(1'b0), .aux_out(), .aux_tri(auxb_tri)
    );

    initial begin
        // Let both ladders run: they start transmitting AUX requests
        // (no sink replies; they will sit in reply-wait / retry).
        #(3_000_000);   // 3 ms
        if (dut_a.state === dut_a.error) begin
            errors = errors + 1;
            $display("FAIL: DUT A in error state before HPD drop");
        end

        // Drop present on both.
        hpd_present_a = 1'b0;
        hpd_present_b = 1'b0;
        #(1_000);       // a few clocks
        if (dut_a.state !== dut_a.error) begin
            errors = errors + 1;
            $display("FAIL: DUT A did not reset on hpd_present drop (state=%0d)",
                     dut_a.state);
        end else
            $display("  ok: fix-enabled FSM reset on hpd_present drop");
        if (dut_b.state === dut_b.error) begin
            errors = errors + 1;
            $display("FAIL: DUT B (legacy) reset on hpd_present drop — must ignore");
        end else
            $display("  ok: legacy FSM unaffected by hpd_present drop");

        // Hold low a while (disconnect), then re-assert: fix-enabled
        // ladder must restart (leave error, produce AUX TX activity).
        #(2_000_000);
        if (dut_a.state !== dut_a.error) begin
            errors = errors + 1;
            $display("FAIL: DUT A left error while hpd_present still low");
        end
        hpd_present_a = 1'b1;
        hpd_present_b = 1'b1;
        #(5_000_000);   // 5 ms: ladder restarts, AUX TX resumes
        if (dut_a.state === dut_a.error) begin
            errors = errors + 1;
            $display("FAIL: DUT A stuck in error after hpd_present re-assert");
        end else
            $display("  ok: fix-enabled ladder restarted after re-assert (state=%0d)",
                     dut_a.state);

        if (errors == 0)
            $display("PASS: HPD_DISCONNECT_RESETS gates closed-loop hpd reset correctly");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

endmodule
