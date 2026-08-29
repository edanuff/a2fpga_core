///////////////////////////////////////////////////////////////////////////////
// dp_video_timing.v : Pixel-domain video timing generator and FIFO writer
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Generates cx/cy coordinates hdl-util-hdmi style: the consumer watches
// cx/cy and supplies the pixel colour for that coordinate on rgb one clock
// later. Active pixels are packed LANE_COUNT to a word and written into the
// clock-crossing FIFO, with a start-of-frame marker on the first word.
//
// Capture is armed by the symbol-domain packer (capture_arm, already
// synchronised by the caller) and begins at the next frame start, so the
// FIFO fill point is aligned to a frame boundary exactly once; after that
// both domains free-run at PLL-locked rates.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module dp_video_timing #(
    parameter LANE_COUNT = 2,           // pixels packed per FIFO word
    parameter H_VISIBLE  = 1280,
    parameter H_TOTAL    = 1650,
    parameter V_VISIBLE  = 720,
    parameter V_TOTAL    = 750,
    parameter BIT_WIDTH  = 11,
    parameter BIT_HEIGHT = 10
)(
    input                       clk_pixel,
    input                       reset,
    input                       capture_arm,   // start capturing at next frame start
    input      [23:0]           rgb,           // pixel for (cx,cy) of previous cycle
    output reg [BIT_WIDTH-1:0]  cx,
    output reg [BIT_HEIGHT-1:0] cy,
    // FIFO write side
    output reg [24*LANE_COUNT-1:0] fifo_wdata,
    output reg                  fifo_wsof,     // first word of a frame
    output reg                  fifo_wen
);

    reg [BIT_WIDTH-1:0]  cx_d;                 // coordinate matching current rgb
    reg [BIT_HEIGHT-1:0] cy_d;
    reg                  active_d;
    reg                  capturing;

    initial begin
        cx         = 0;
        cy         = 0;
        cx_d       = 0;
        cy_d       = 0;
        active_d   = 1'b0;
        capturing  = 1'b0;
        fifo_wen   = 1'b0;
        fifo_wsof  = 1'b0;
    end

    always @(posedge clk_pixel) begin
        if (reset) begin
            cx        <= 0;
            cy        <= 0;
            active_d  <= 1'b0;
            capturing <= 1'b0;
        end else begin
            // ----------------------------------------------------------
            // Coordinate counters
            // ----------------------------------------------------------
            if (cx == H_TOTAL-1) begin
                cx <= 0;
                cy <= (cy == V_TOTAL-1) ? {BIT_HEIGHT{1'b0}} : cy + 1'b1;
            end else begin
                cx <= cx + 1'b1;
            end

            // rgb arriving now corresponds to last cycle's coordinates
            cx_d     <= cx;
            cy_d     <= cy;
            active_d <= (cx < H_VISIBLE) && (cy < V_VISIBLE);

            // ----------------------------------------------------------
            // Arm/launch capture only at a frame boundary so the FIFO
            // stream always begins with pixel (0,0)
            // ----------------------------------------------------------
            if (capture_arm && !capturing && cx == 0 && cy == 0)
                capturing <= 1'b1;
        end
    end

    // --------------------------------------------------------------
    // Pack LANE_COUNT pixels per FIFO word; low 24 bits = lane 0
    // --------------------------------------------------------------
    generate
    if (LANE_COUNT == 1) begin : g_pack1
        always @(posedge clk_pixel) begin
            fifo_wen  <= 1'b0;
            fifo_wsof <= 1'b0;
            if (!reset && capturing && active_d) begin
                fifo_wdata <= rgb;
                fifo_wsof  <= (cy_d == 0) && (cx_d == 0);
                fifo_wen   <= 1'b1;
            end
        end
    end else begin : g_packn
        reg [$clog2(LANE_COUNT)-1:0] pack_count = 0;
        reg [24*(LANE_COUNT-1)-1:0]  pack_shift;
        always @(posedge clk_pixel) begin
            fifo_wen  <= 1'b0;
            fifo_wsof <= 1'b0;
            if (reset) begin
                pack_count <= 0;
            end else if (capturing && active_d) begin
                if (pack_count == LANE_COUNT-1) begin
                    pack_count <= 0;
                    fifo_wdata <= {rgb, pack_shift};
                    fifo_wsof  <= (cy_d == 0) && (cx_d == LANE_COUNT-1);
                    fifo_wen   <= 1'b1;
                end else begin
                    // right-shift accumulate; width-safe for any LANE_COUNT
                    pack_shift <= {rgb, pack_shift} >> 24;
                    pack_count <= pack_count + 1'b1;
                end
            end
        end
    end
    endgenerate

endmodule
