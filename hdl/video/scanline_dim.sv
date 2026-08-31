//
// scanline_dim — CRT scanline effect on the final video composite.
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// The ONE place the scanline effect lives: applied to the video+OSD
// composite (before the DebugOverlay, which stays crisp), instead of each
// layer carrying its own copy. Scale-aware: the vertical scale and window
// top arrive as inputs from the framebuffer — the phase locks to the
// SOURCE-line grid at the window top each frame and free-runs through the
// borders, so the stripes are continuous across the whole screen and track
// whatever scan-out scale the framebuffer is running. Odd sub-lines dim
// 50% — the scale-generic rule (sub-lines 1,3 of 5 at x5; 1 of 2 at x2).
//
// The dim path is purely combinational between the upstream output
// registers and the downstream input registers (one mux level): no added
// pixel latency, no RGB_LATENCY contract change. Phase bookkeeping updates
// on raster-line changes, which happen in blanking.
//
// Border continuity note: below the window the free-run is always seamless.
// Above the window, the run-through from the previous frame lines up
// whenever V_TOTAL is a multiple of the scale (1125 = 225*5 today); for a
// future scale where it isn't, the top border re-locks at the window top —
// in-window stripes are always source-aligned by construction.
//
module scanline_dim (
    input  wire        clk_i,
    input  wire        enable_i,

    input  wire [10:0] screen_y_i,   // registered raster line counter
    input  wire [2:0]  v_scale_i,    // raster lines per source line (from fb)
    input  wire [10:0] v_border_i,   // active-window top raster line (from fb)

    input  wire [23:0] rgb_i,
    output wire [23:0] rgb_o
);

    reg [10:0] cy_prev_r;
    reg [2:0]  phase_r;
    reg        dim_q;

    always @(posedge clk_i) begin
        cy_prev_r <= screen_y_i;
        if (screen_y_i != cy_prev_r) begin
            if (screen_y_i == v_border_i)
                phase_r <= 3'd0;   // lock to the source-line grid
            else
                phase_r <= (phase_r >= v_scale_i - 3'd1) ? 3'd0
                                                         : phase_r + 3'd1;
        end
        // odd sub-lines dim — scale-generic
        dim_q <= enable_i && phase_r[0];
    end

    wire [23:0] dimmed_w = {1'b0, rgb_i[23:17],
                            1'b0, rgb_i[15:9],
                            1'b0, rgb_i[7:1]};

    assign rgb_o = dim_q ? dimmed_w : rgb_i;

endmodule
