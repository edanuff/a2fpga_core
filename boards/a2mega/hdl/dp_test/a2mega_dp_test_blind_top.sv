///////////////////////////////////////////////////////////////////////////////
// a2mega_dp_test_blind_top.sv — BLIND-SINK variant of the dp_test colorbars
// bring-up top (open-loop AUX ladder: every DPCD write still goes out, no
// replies awaited). For sinks the 1.0a3 AUX receiver cannot decode — the
// Fangor USB-C monitor, generic USB-C->HDMI dongles, the VMM7100 adapter
// (adapter_census_results.md) — until the 1.0a4 receiver-bias divider.
// Same pins, same CST/SDC; built by a2mega_dp_test_blind.gprj.
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module a2mega_dp_test_blind_top (
    input  logic clk50_in,
    input  logic button,
    inout  wire  dp_aux_p,
    inout  wire  dp_aux_n,
    input  logic dp_hpd,
    output logic [3:0] led,
    output logic uart_tx
);
    a2mega_dp_test_top #(.BLIND(1)) i_core (
        .clk50_in (clk50_in),
        .button   (button),
        .dp_aux_p (dp_aux_p),
        .dp_aux_n (dp_aux_n),
        .dp_hpd   (dp_hpd),
        .led      (led),
        .uart_tx  (uart_tx)
    );
endmodule
