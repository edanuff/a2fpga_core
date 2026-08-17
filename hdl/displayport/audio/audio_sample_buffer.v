///////////////////////////////////////////////////////////////////////////////
// audio_sample_buffer.v : Audio strobe CDC and 4-sample ping-pong buffer
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Input contract mirrors the hdl-util HDMI module: clk_audio is a
// one-clk_pixel-wide strobe at the audio sample rate, with the stereo
// sample word valid while it is high. Samples cross into the link symbol
// clock domain through a toggle synchroniser (the strobe rate is orders
// of magnitude below either clock), and are collected four at a time into
// a ping-pong buffer for the Audio_Stream SDP builder.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module audio_sample_buffer #(
    parameter AUDIO_BIT_WIDTH = 16
)(
    // pixel clock domain
    input                            clk_pixel,
    input                            clk_audio,      // sample-rate strobe
    input  [AUDIO_BIT_WIDTH-1:0]     sample_l,
    input  [AUDIO_BIT_WIDTH-1:0]     sample_r,
    // link symbol clock domain
    input                            clk_sym,
    input                            reset,
    output reg                       strobe_sym,     // one-cycle pulse per sample
    output reg [4*2*AUDIO_BIT_WIDTH-1:0] buffer,     // 4 stereo samples {s3r,s3l,..,s0r,s0l}
    output reg [2:0]                 buffer_count,   // samples currently staged (0..4)
    output reg                       buffer_ready,   // 4 samples staged; cleared by take
    input                            buffer_take     // scheduler consumed the buffer
);

    // ------------------------------------------------------------------
    // Pixel domain: latch sample, flip toggle
    // ------------------------------------------------------------------
    reg                        cdc_toggle = 1'b0;
    reg [2*AUDIO_BIT_WIDTH-1:0] cdc_sample = 0;

    always @(posedge clk_pixel) begin
        if (clk_audio) begin
            cdc_sample <= {sample_r, sample_l};
            cdc_toggle <= ~cdc_toggle;
        end
    end

    // ------------------------------------------------------------------
    // Symbol domain: detect toggle edge, stage samples
    // ------------------------------------------------------------------
    (* ASYNC_REG = "TRUE" *) reg tgl_m = 1'b0, tgl_s = 1'b0;
    reg tgl_last = 1'b0;

    always @(posedge clk_sym) begin
        tgl_m    <= cdc_toggle;
        tgl_s    <= tgl_m;
        tgl_last <= tgl_s;
        strobe_sym <= 1'b0;

        if (reset) begin
            buffer_count <= 3'd0;
            buffer_ready <= 1'b0;
        end else begin
            if (buffer_take)
                buffer_ready <= 1'b0;

            if (tgl_s != tgl_last) begin
                // cdc_sample has been stable for >2 clk_sym by now
                strobe_sym <= 1'b1;
                buffer[buffer_count[1:0]*2*AUDIO_BIT_WIDTH +: 2*AUDIO_BIT_WIDTH]
                    <= cdc_sample;
                if (buffer_count == 3'd3) begin
                    buffer_count <= 3'd0;
                    buffer_ready <= 1'b1;   // overwrites unconsumed buffer
                end else begin
                    buffer_count <= buffer_count + 1'b1;
                end
            end
        end
    end

endmodule
