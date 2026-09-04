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
// whatever scan-out scale the framebuffer is running. Each source-line
// group splits into a bright band then a contiguous 50%-dim band of
// roughly equal size (x5: 3+2, x2: 1+1 — the original 480p alternation),
// so the banding matches the source vertical resolution (192 for Apple II).
//
// The dim path is purely combinational between the upstream output
// registers and the downstream input registers (one mux level): no added
// pixel latency, no RGB_LATENCY contract change. Phase bookkeeping for a
// line runs at H_TRIGGER in the PREVIOUS line's horizontal blanking, so
// the dim flag is stable before pixel 0 of every line.
//
// Border continuity note: below the window the free-run is always seamless.
// Above the window, the run-through from the previous frame lines up
// whenever V_TOTAL is a multiple of the scale (1125 = 225*5 today); for a
// future scale where it isn't, the top border re-locks at the window top —
// in-window stripes are always source-aligned by construction.
//
module scanline_dim #(
    // Phase bookkeeping runs at this x position — must sit in horizontal
    // blanking (past the active width, below H_TOTAL) so the dim flag for
    // a line is stable BEFORE its first pixel. Advancing on the cy change
    // itself lands 2-3 clocks into the line and displaces the stripe
    // boundary on the leftmost pixels (seen on hardware).
    parameter [11:0] H_TRIGGER = 12'd1930,
    parameter [10:0] V_TOTAL   = 11'd1125
)(
    input  wire        clk_i,
    input  wire        enable_i,

    input  wire [11:0] screen_x_i,   // registered raster column counter
    input  wire [10:0] screen_y_i,   // registered raster line counter
    input  wire [2:0]  v_scale_i,    // raster lines per source line (from fb)
    input  wire [10:0] v_border_i,   // active-window top raster line (from fb)

    input  wire [23:0] rgb_i,
    output wire [23:0] rgb_o
);

    reg [2:0] phase_r;
    reg       dim_q;

    // The line the phase is being prepared FOR (trigger fires on line N,
    // in its blanking, computing line N+1's phase)
    wire [10:0] y_next_w = (screen_y_i == V_TOTAL - 11'd1) ? 11'd0
                                                           : screen_y_i + 11'd1;

    // Timing campaign round 2, cone (m): both trigger terms are
    // pre-registered, exactly. screen_x_i is sequential, so
    // x_trig_r <= (x + 1 == H_TRIGGER) is true precisely in the cycle
    // where screen_x_i == H_TRIGGER (2199 + 1 never matches); screen_y_i
    // is constant for the whole line at the trigger column, so y_lock_r
    // registered from y_next_w one cycle earlier equals the live compare
    // there. The compare tree used to run straight from the overlay's cy
    // copy into phase_r's reset (+0.51 ns at 148.5 MHz in a 60K roll).
    reg x_trig_r = 1'b0;
    reg y_lock_r = 1'b0;
    always @(posedge clk_i) begin
        x_trig_r <= (screen_x_i + 12'd1 == H_TRIGGER);
        y_lock_r <= (y_next_w == v_border_i);
    end

    always @(posedge clk_i) begin
        if (x_trig_r) begin                 // screen_x_i == H_TRIGGER
            if (y_lock_r)                   // y_next_w == v_border_i
                phase_r <= 3'd0;   // lock to the source-line grid
            else
                phase_r <= (phase_r >= v_scale_i - 3'd1) ? 3'd0
                                                         : phase_r + 3'd1;
        end
        // Equal-ish bright/dark split per SOURCE line, dark lines
        // CONTIGUOUS: dim when phase >= ceil(scale/2). x5 -> 3 bright +
        // 2-line dark band; x2 -> plain alternation (the original 480p
        // look); x4 -> 2+2. The bands ride the source-line pitch (192 for
        // Apple II). (The earlier dim-odd-sub-lines rule put SCATTERED
        // dark lines inside each source line — an every-other-raster
        // texture that beat against monitor/capture scaling instead of
        // reading as scanlines.) Registered a full blanking interval
        // before the line's first pixel.
        dim_q <= enable_i && (phase_r >= v_scale_i - (v_scale_i >> 1));
    end

    wire [23:0] dimmed_w = {1'b0, rgb_i[23:17],
                            1'b0, rgb_i[15:9],
                            1'b0, rgb_i[7:1]};

    assign rgb_o = dim_q ? dimmed_w : rgb_i;

endmodule
