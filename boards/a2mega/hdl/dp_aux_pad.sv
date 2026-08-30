///////////////////////////////////////////////////////////////////////////////
// dp_aux_pad — the a2mega AUX-channel pad cell, EXTRACTED VERBATIM from
// a2mega_dp_test_top.sv (08-30) so the dp_test and full-core tops share
// one source of truth for the hardware-battled AUX electrical config.
//
// AUX_TLVDS=1 (the PROVEN closed-loop config, campaign-validated):
//   G15/G16 as a true LVDS pair — TLVDS_IOBUF gives spec-compliant
//   ~350 mV differential TX; both-leg PULL_MODE=DOWN in the CST decays
//   the AC-coupled stored offset toward zero between bursts (round 7);
//   counter-park drives the OPPOSITE polarity for ~2.5 us after each
//   burst so the stored differential lands near zero at release
//   (round 8); NO RX inversion (round 10: TLVDS sense is opposite the
//   ELVDS-derived convention).
//   CST REQUIREMENT:  IO_LOC "dp_aux_p" G15,G16;
//                     IO_PORT "dp_aux_p" IO_TYPE=LVDS25 PULL_MODE=DOWN;
//   (single differential port; no separate dp_aux_n CST entry)
//
// AUX_TLVDS=0 (legacy pseudo-diff, blind-era): two single-ended
//   LVCMOS33 pins; kept for archaeology.
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module dp_aux_pad #(
    parameter AUX_TLVDS = 1
)(
    input  wire clk100,
    inout  wire dp_aux_p,     // G15 (pair master when AUX_TLVDS=1)
    inout  wire dp_aux_n,     // G16
    output wire auxch_in,
    input  wire auxch_out,
    input  wire auxch_tri
);
    generate if (AUX_TLVDS != 0) begin : g_aux_tlvds
        logic auxch_in_raw;
        // Round 10: inverter REMOVED — TLVDS RX sense is non-inverted.
        assign auxch_in = auxch_in_raw;
        // Round 8 COUNTER-PARK: after the STOP, hold the OPPOSITE
        // polarity ~2.5 us (~0.5 tau of the far-side termination path)
        // so the AC-coupling caps discharge to near-zero differential
        // before release — the reply's SYNC-END then decodes cleanly.
        logic       tri_d = 1'b1;
        logic       out_last = 1'b0;
        logic [8:0] park_cnt = 9'd0;
        always_ff @(posedge clk100) begin
            tri_d <= auxch_tri;
            if (!auxch_tri)
                out_last <= auxch_out;
            if (auxch_tri && !tri_d)
                park_cnt <= 9'd250;          // 2.5 us @ 100 MHz
            else if (park_cnt != 9'd0)
                park_cnt <= park_cnt - 9'd1;
        end
        wire parking = (park_cnt != 9'd0);

        TLVDS_IOBUF i_aux_diff (
            .O   (auxch_in_raw),
            .IO  (dp_aux_p),
            .IOB (dp_aux_n),
            .I   (parking ? ~out_last : auxch_out),
            .OEN (auxch_tri && !parking)
        );
    end else begin : g_aux_pseudo
        assign dp_aux_p = auxch_tri ? 1'bz : auxch_out;
        assign dp_aux_n = auxch_tri ? 1'bz : ~auxch_out;
        assign auxch_in = dp_aux_p;
    end endgenerate
endmodule
