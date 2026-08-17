///////////////////////////////////////////////////////////////////////////////
// maud_measure.v : Measured Maud generation for synchronous DP audio
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// DP audio clock recovery: fs = f_LS * Maud / (512 * Naud). With Naud
// fixed at 32768, counting audio sample strobes over a window of exactly
// 512 * 32768 link-symbol periods yields Maud directly - no divider, and
// self-correcting against strobe-rate inaccuracy. The datapath clock runs
// at two link symbols per cycle, so the window is 2^23 cycles.
//
// The output is seeded from AUDIO_RATE / LINK_RATE_MBPS so the first
// window transmits a sane nominal value (48 kHz @ RBR -> 4971).
//
// The per-line Maud byte transmitted after VB-ID/Mvid in the main stream
// is the least-significant 8 bits of the Maud value (DP 1.1a section
// 2.2.5.2).
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module maud_measure #(
    parameter AUDIO_RATE     = 48000,
    parameter LINK_RATE_MBPS = 1620
)(
    input             clk_sym,      // link symbol clock / 2 domain
    input             reset,
    input             strobe_sym,   // one-cycle pulse per audio sample
    output reg [23:0] maud,         // measured Maud (Naud fixed = 32768)
    output wire [7:0] maud_byte     // per-line Maud byte = maud[7:0]
);

    assign maud_byte = maud[7:0];

    // nominal: Maud = fs * 512 * 32768 / f_LS ; f_LS = LINK_RATE_MBPS/10 MHz
    localparam [23:0] MAUD_NOMINAL =
        (AUDIO_RATE * 512 * 64'd32768) / (LINK_RATE_MBPS * 100_000);

    localparam WINDOW_CYCLES = 1 << 23;   // 2^24 symbols at 2 symbols/cycle

    reg [23:0] window_cnt = 0;
    reg [23:0] strobe_cnt = 0;

    initial begin
        maud = MAUD_NOMINAL;
    end

    always @(posedge clk_sym) begin
        if (reset) begin
            window_cnt <= 0;
            strobe_cnt <= 0;
            maud       <= MAUD_NOMINAL;
        end else begin
            if (window_cnt == WINDOW_CYCLES-1) begin
                window_cnt <= 0;
                maud       <= strobe_cnt + {23'b0, strobe_sym};
                strobe_cnt <= 0;
            end else begin
                window_cnt <= window_cnt + 1'b1;
                if (strobe_sym)
                    strobe_cnt <= strobe_cnt + 1'b1;
            end
        end
    end

endmodule
