// SDRAM Framebuffer for Tang Nano 20K — 480p output
//
// (c) 2025 Ed Anuff <ed@a2fpga.com>
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
// Description:
//
// SDRAM-backed framebuffer for the a2n20v2-GS board. Stores rendered Apple II
// video pixels in SDRAM and reads them back for 480p HDMI display.
//
// Architecture:
//   Write path: accepts fb_we/fb_data from renderers (apple_video_fb, vgc_fb),
//               converts RGB666 to RGB565, packs pixel pairs into 32-bit words,
//               buffers in a 256-entry FIFO, and drains to SDRAM via FB_WRITE_PORT.
//   Read path:  line fetch FSM prefetches scanlines from SDRAM into a dual-port
//               BRAM line buffer, unpacking 2 pixels per SDRAM word.
//               Yields to writes only when FIFO is near full (safety valve).
//   CDC:        only needed for the line buffer (54 MHz write, 27 MHz read)
//               using true dual-port BRAM with independent clocks.
//
// Pixel packing: 2x RGB565 per 32-bit SDRAM word halves SDRAM access count,
// solving the bandwidth contention between reads and writes that caused
// vertical artifacts with the original 1-pixel-per-word design.
//
// Display: 720x480 @ 59.94 Hz (VIDEO_ID_CODE=2)
//   Apple II modes: [80 border][560 active][80 border] x [48 border][384=192x2][48 border]
//   IIgs SHR modes: [40 border][640 active][40 border] x [40 border][400=200x2][40 border]
//   2x integer vertical scaling, optional scanline dimming on odd lines.
//

module sdram_framebuffer #(
    parameter COLOR_BITS = 18,           // RGB666
    parameter FB_BASE_ADDR = 21'h180000  // 1.5MB offset in SDRAM
) (
    // Clocks and reset
    input  logic        clk,             // 54 MHz logic clock (also SDRAM clock)
    input  logic        clk_pixel,       // 27 MHz pixel clock
    input  logic        rst_n,

    // Framebuffer write interface (from apple_video_fb / vgc_fb, clk domain)
    input  logic        fb_vsync,        // Frame start pulse
    input  logic        fb_we,           // Pixel write enable
    input  logic [COLOR_BITS-1:0] fb_data, // RGB666 pixel data
    input  logic [10:0] fb_width,        // Active width: 560 or 640
    input  logic [9:0]  fb_height,       // Active height: 192 or 200

    // SDRAM port interfaces
    mem_port_if.client  fb_write_port,   // For writing pixels to SDRAM
    mem_port_if.client  fb_read_port,    // For reading scanlines from SDRAM

    // HDMI scan position (pixel clock domain, from HDMI encoder)
    input  logic [10:0] hdmi_cx,
    input  logic [9:0]  hdmi_cy,

    // Video output (pixel clock domain)
    output logic [7:0]  r_o,
    output logic [7:0]  g_o,
    output logic [7:0]  b_o,

    // Configuration
    input  logic [COLOR_BITS-1:0] border_color,
    input  logic        scanline_en,     // Enable CRT scanline dimming
    input  logic        sleep_i,         // Output black when high

    // Debug counters/flags (clk domain)
    output logic [7:0]  dbg_fifo_level_o,      // Current FIFO fill level (clamped)
    output logic [7:0]  dbg_fifo_highwater_o,  // Per-frame FIFO high-water mark
    output logic [7:0]  dbg_fifo_overflow_o,   // Dropped packed writes per frame
    output logic [7:0]  dbg_fetch_start_o,     // Line fetch starts per frame
    output logic [7:0]  dbg_fetch_done_o,      // Line fetch completions per frame
    output logic [7:0]  dbg_read_blocked_o,    // FETCH_READ cycles blocked by port unavailable
    output logic [7:0]  dbg_yield_busy_o,      // FETCH_READ cycles yielding to near-full write FIFO
    output logic [7:0]  dbg_late_line_o,       // Behind-display late-line detections per frame
    output logic [7:0]  dbg_flags_o,           // Live status flags
    output logic [7:0]  dbg_line_not_ready_o,  // Display line-starts before line fully fetched
    output logic [7:0]  dbg_line_lag_max_o,    // Max display-vs-fetched line lag per frame
    output logic [7:0]  dbg_ready_phase_err_o, // Read ready pulses outside FETCH_WAIT
    output logic [7:0]  dbg_vsync_raw_o,       // Raw fb_vsync pulses seen in this frame
    output logic [7:0]  dbg_frame_start_accept_o, // Accepted frame starts in this frame
    output logic [7:0]  dbg_frame_start_reject_o  // Rejected fb_vsync pulses in this frame
);

    // =========================================================================
    // 480p display parameters
    // =========================================================================

    localparam HDMI_WIDTH  = 720;
    localparam HDMI_HEIGHT = 480;

    // =========================================================================
    // Color conversion functions
    // =========================================================================

    // RGB666 (18-bit) to RGB565 (16-bit) for SDRAM storage
    function automatic [15:0] rgb666_to_565(input [17:0] c);
        return {c[17:13], c[11:6], c[5:1]};
    endfunction

    // RGB565 (16-bit) to RGB666 (18-bit) for line buffer
    function automatic [17:0] rgb565_to_666(input [15:0] c);
        return {c[15:11], c[15],   // R: 5->6 bits
                c[10:5],           // G: 6 bits
                c[4:0], c[4]};    // B: 5->6 bits
    endfunction

    // RGB666 to RGB888 for HDMI output
    function automatic [23:0] torgb(input [COLOR_BITS-1:0] c);
        return {c[17:12], c[17:16],   // R
                c[11:6],  c[11:10],   // G
                c[5:0],   c[5:4]};    // B
    endfunction

    // =========================================================================
    // Write FIFO — buffers packed pixel pairs for SDRAM
    // =========================================================================
    //
    // Each entry: {21-bit addr, 32-bit data} = 53 bits
    // Depth 256 absorbs full write bursts during uninterrupted line fetches.
    // The fetcher only yields to writes as a safety valve when the FIFO is
    // near full, allowing line fetches to complete as fast as possible.

    localparam FIFO_DEPTH = 256;
    localparam FIFO_ADDR_BITS = 8;  // log2(256)
    // Yield reads only as a safety valve when write FIFO backs up.
    localparam FIFO_YIELD_THRESHOLD = 64;

    reg [52:0] wr_fifo [0:FIFO_DEPTH-1];
    reg [FIFO_ADDR_BITS:0] fifo_wr_ptr_r, fifo_rd_ptr_r;

    wire [FIFO_ADDR_BITS:0] fifo_count_w = fifo_wr_ptr_r - fifo_rd_ptr_r;
    wire fifo_empty_w = (fifo_wr_ptr_r == fifo_rd_ptr_r);
    wire fifo_full_w  = fifo_count_w[FIFO_ADDR_BITS];  // MSB set when count >= 256
    // Start yielding reads earlier so write FIFO never reaches drop-on-full behavior.
    wire fifo_busy_w  = (fifo_count_w >= FIFO_YIELD_THRESHOLD);
    wire [7:0] fifo_count_clamped_w = fifo_count_w[FIFO_ADDR_BITS] ? 8'hFF : fifo_count_w[7:0];

    // =========================================================================
    // Write path — pixel packing + FIFO (clk domain, 54 MHz)
    // =========================================================================
    //
    // Accepts RGB666 pixels from renderers, converts to RGB565, buffers even
    // pixels, packs pairs into 32-bit words, and pushes to FIFO for SDRAM.
    // SDRAM address uses packed width (fb_width/2 words per line).

    reg [10:0] wr_x_r;
    reg [9:0]  wr_y_r;
    reg [10:0] wr_width_r;           // latched at vsync
    reg [20:0] wr_line_base_r;       // FB_BASE_ADDR + y * (fb_width/2)
    reg [15:0] wr_pixel_even_r;      // buffered even pixel (RGB565)
    reg        frame_pending_r;      // waiting for first fb_we after accepted frame start
    // Reject implausibly short frame-start intervals so a spurious fb_vsync
    // pulse cannot reset write/read tracking mid-frame.
    localparam [19:0] FRAME_MIN_CYCLES = 20'd540000;  // ~10ms at 54MHz
    reg [19:0] frame_gap_r;
    wire frame_start_w = fb_vsync && (frame_gap_r >= FRAME_MIN_CYCLES);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            frame_gap_r <= FRAME_MIN_CYCLES;
        end else begin
            if (frame_gap_r != 20'hFFFFF)
                frame_gap_r <= frame_gap_r + 20'd1;
            if (frame_start_w)
                frame_gap_r <= 20'd0;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_x_r <= 11'd0;
            wr_y_r <= 10'd0;
            wr_width_r <= 11'd560;
            wr_line_base_r <= FB_BASE_ADDR;
            fifo_wr_ptr_r <= '0;
            wr_pixel_even_r <= 16'd0;
            frame_pending_r <= 1'b0;
        end else begin
            if (frame_start_w) begin
                // Arm reset, but align to the first pixel write pulse so
                // wr_x/wr_y stay phase-locked to the producer's fb_we stream.
                frame_pending_r <= 1'b1;
                wr_width_r <= fb_width;
            end

            if (fb_we) begin
                if (frame_pending_r || frame_start_w) begin
                    // First pixel of new frame: consume as x=0 even pixel.
                    wr_x_r <= 11'd1;
                    wr_y_r <= 10'd0;
                    wr_line_base_r <= FB_BASE_ADDR;
                    wr_pixel_even_r <= rgb666_to_565(fb_data);
                    frame_pending_r <= 1'b0;
                end else begin
                    if (wr_x_r[0] == 1'b0) begin
                        // Even pixel — buffer as RGB565
                        wr_pixel_even_r <= rgb666_to_565(fb_data);
                    end else if (!fifo_full_w) begin
                        // Odd pixel — pack with buffered even and push to FIFO
                        wr_fifo[fifo_wr_ptr_r[FIFO_ADDR_BITS-1:0]] <= {
                            wr_line_base_r + {11'd0, wr_x_r[10:1]},  // packed addr
                            rgb666_to_565(fb_data),                    // odd pixel [31:16]
                            wr_pixel_even_r                            // even pixel [15:0]
                        };
                        fifo_wr_ptr_r <= fifo_wr_ptr_r + 1;
                    end

                    // Advance position
                    if (wr_x_r == wr_width_r - 11'd1) begin
                        wr_x_r <= 11'd0;
                        wr_y_r <= wr_y_r + 10'd1;
                        wr_line_base_r <= wr_line_base_r + {11'd0, wr_width_r[10:1]};
                    end else begin
                        wr_x_r <= wr_x_r + 11'd1;
                    end
                end
            end
        end
    end

    // FIFO drain to SDRAM write port
    wire [52:0] fifo_head_w = wr_fifo[fifo_rd_ptr_r[FIFO_ADDR_BITS-1:0]];
    wire fifo_pop_w = !fifo_empty_w && fb_write_port.available;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            fifo_rd_ptr_r <= '0;
        else if (fifo_pop_w)
            fifo_rd_ptr_r <= fifo_rd_ptr_r + 1;
    end

    assign fb_write_port.addr    = fifo_head_w[52:32];
    assign fb_write_port.data    = fifo_head_w[31:0];
    assign fb_write_port.byte_en = 4'b1111;
    assign fb_write_port.wr      = fifo_pop_w;
    assign fb_write_port.rd      = 1'b0;
    assign fb_write_port.burst   = 1'b0;

    // =========================================================================
    // CDC: HDMI cy → clk domain (gray-code)
    // =========================================================================

    function automatic [9:0] bin2gray(input [9:0] b);
        return b ^ (b >> 1);
    endfunction

    function automatic [9:0] gray2bin(input [9:0] g);
        reg [9:0] b;
        b[9] = g[9];
        for (int i = 8; i >= 0; i--)
            b[i] = b[i+1] ^ g[i];
        return b;
    endfunction

    reg [9:0] cy_gray_px_r;
    always @(posedge clk_pixel) begin
        cy_gray_px_r <= bin2gray(hdmi_cy);
    end

    reg [9:0] cy_gray_sync1_r, cy_gray_sync2_r;
    reg [9:0] cy_sync_r;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cy_gray_sync1_r <= 10'd0;
            cy_gray_sync2_r <= 10'd0;
            cy_sync_r <= 10'd0;
        end else begin
            cy_gray_sync1_r <= cy_gray_px_r;
            cy_gray_sync2_r <= cy_gray_sync1_r;
            cy_sync_r <= gray2bin(cy_gray_sync2_r);
        end
    end

    // =========================================================================
    // Read path / Line Fetch FSM — clk domain (54 MHz)
    // =========================================================================
    //
    // Reactive fetch: continuously checks whether a new line needs fetching
    // based on the CDC'd display position (cy_sync_r). No counter, no frame
    // reset, no cy-change gating — conditions are evaluated every cycle in
    // FETCH_IDLE for immediate response.
    //
    // - When approaching the active area from vblank: prefetches line 0
    // - During active display of line N: prefetches line N+1
    // - last_fetched_line_r (sentinel 9'h1FF = invalid) prevents re-fetches
    // - Bank = target_line[0] (line parity); display reads bank fb_line[0]
    // - Natural pacing: the fetcher can't outrun the display because
    //   next_line == last_fetched prevents re-fetch of the same line.

    wire [10:0] h_border_w = (HDMI_WIDTH - fb_width) >> 1;
    wire [9:0]  v_border_w = (HDMI_HEIGHT - {fb_height, 1'b0}) >> 1;

    // Packed width: SDRAM words per line (fb_width / 2)
    wire [10:0] packed_width_w = {1'b0, fb_width[10:1]};
    localparam integer FB_READ_BURST_WORDS = 4;  // 4 x 32-bit words = 8 pixels per request
    localparam integer PREFETCH_LEAD_LINES = 12;

    localparam FETCH_IDLE    = 2'd0;
    localparam FETCH_READ    = 2'd1;
    localparam FETCH_WAIT    = 2'd2;

    reg [1:0]  fetch_state_r;
    reg [8:0]  last_fetched_line_r;  // 9'h1FF = invalid sentinel
    reg [10:0] fetch_word_r;
    reg [20:0] fetch_addr_r;
    reg        fetch_bank_r;
    reg [3:0]  fetch_beats_left_r;

    // Debug counters (saturating, reset each frame at fb_vsync)
    reg [7:0] dbg_fifo_highwater_r;
    reg [7:0] dbg_fifo_overflow_r;
    reg [7:0] dbg_fetch_start_r;
    reg [7:0] dbg_fetch_done_r;
    reg [7:0] dbg_read_blocked_r;
    reg [7:0] dbg_yield_busy_r;
    reg [7:0] dbg_late_line_r;
    reg       late_line_prev_r;
    reg [7:0] dbg_line_not_ready_r;
    reg [7:0] dbg_line_lag_max_r;
    reg [7:0] dbg_ready_phase_err_r;
    reg [7:0] dbg_vsync_raw_r;
    reg [7:0] dbg_frame_start_accept_r;
    reg [7:0] dbg_frame_start_reject_r;
    reg [8:0] completed_line_even_r;
    reg [8:0] completed_line_odd_r;
    reg [8:0] display_line_prev_r;
    reg       display_active_prev_r;

    // Display position in clk domain (from CDC'd cy)
    wire [9:0] cy_minus_border_w = cy_sync_r - v_border_w;
    wire [8:0] display_fb_line_w = cy_minus_border_w[9:1];
    wire       display_in_active_w = (cy_sync_r >= v_border_w) &&
                                      (cy_sync_r < v_border_w + {fb_height, 1'b0});

    // Approaching active area: prefetch line 0 early in vblank so first active
    // lines are guaranteed ready.
    wire [10:0] cy_prefetch_sum_w = {1'b0, cy_sync_r} + 11'(PREFETCH_LEAD_LINES);
    wire cy_approaching_active_w = (cy_sync_r < v_border_w) &&
                                    (cy_prefetch_sum_w >= {1'b0, v_border_w});

    wire [8:0] next_line_w = display_fb_line_w + 9'd1;
    wire [10:0] fetch_words_left_w = packed_width_w - fetch_word_r;
    wire fetch_use_burst_w = (fetch_words_left_w >= FB_READ_BURST_WORDS[10:0]);
    wire wr_drop_w = fb_we && wr_x_r[0] && fifo_full_w;
    wire [8:0] completed_line_for_display_w = display_fb_line_w[0] ? completed_line_odd_r : completed_line_even_r;
    wire line_ready_w = (completed_line_for_display_w == display_fb_line_w);
    wire [8:0] line_lag_w = (completed_line_for_display_w == 9'h1FF ||
                             completed_line_for_display_w >= display_fb_line_w) ?
                             9'd0 : (display_fb_line_w - completed_line_for_display_w);
    wire [7:0] line_lag_clamped_w = line_lag_w[8] ? 8'hFF : line_lag_w[7:0];
    wire display_line_step_w = display_in_active_w &&
                               (!display_active_prev_r || (display_fb_line_w != display_line_prev_r));
    wire ready_phase_err_w = fb_read_port.ready && (fetch_state_r != FETCH_WAIT);
    wire late_line_w = display_in_active_w &&
                       (display_fb_line_w != 9'd0) &&
                       (last_fetched_line_r != 9'h1FF) &&
                       (last_fetched_line_r < display_fb_line_w);
    wire fetch_start_line0_w = (fetch_state_r == FETCH_IDLE) &&
                               cy_approaching_active_w &&
                               (last_fetched_line_r != 9'd0);
    wire fetch_start_next_w = (fetch_state_r == FETCH_IDLE) &&
                              !fetch_start_line0_w &&
                              display_in_active_w &&
                              (next_line_w < fb_height[8:0]) &&
                              (next_line_w != last_fetched_line_r);
    wire fetch_start_pulse_w = fetch_start_line0_w || fetch_start_next_w;
    wire fetch_done_pulse_w = (fetch_state_r == FETCH_WAIT) &&
                              fb_read_port.ready &&
                              (fetch_word_r == packed_width_w - 11'd1);

    // Per-frame debug counters and live high-water tracking
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            dbg_fifo_highwater_r <= 8'd0;
            dbg_fifo_overflow_r <= 8'd0;
            dbg_fetch_start_r <= 8'd0;
            dbg_fetch_done_r <= 8'd0;
            dbg_read_blocked_r <= 8'd0;
            dbg_yield_busy_r <= 8'd0;
            dbg_late_line_r <= 8'd0;
            late_line_prev_r <= 1'b0;
            dbg_line_not_ready_r <= 8'd0;
            dbg_line_lag_max_r <= 8'd0;
            dbg_ready_phase_err_r <= 8'd0;
            dbg_vsync_raw_r <= 8'd0;
            dbg_frame_start_accept_r <= 8'd0;
            dbg_frame_start_reject_r <= 8'd0;
            completed_line_even_r <= 9'h1FF;
            completed_line_odd_r <= 9'h1FF;
            display_line_prev_r <= 9'd0;
            display_active_prev_r <= 1'b0;
        end else if (frame_start_w) begin
            dbg_fifo_highwater_r <= fifo_count_clamped_w;
            dbg_fifo_overflow_r <= 8'd0;
            dbg_fetch_start_r <= 8'd0;
            dbg_fetch_done_r <= 8'd0;
            dbg_read_blocked_r <= 8'd0;
            dbg_yield_busy_r <= 8'd0;
            dbg_late_line_r <= 8'd0;
            late_line_prev_r <= 1'b0;
            dbg_line_not_ready_r <= 8'd0;
            dbg_line_lag_max_r <= 8'd0;
            dbg_ready_phase_err_r <= 8'd0;
            dbg_vsync_raw_r <= 8'd1;
            dbg_frame_start_accept_r <= 8'd1;
            dbg_frame_start_reject_r <= 8'd0;
            completed_line_even_r <= 9'h1FF;
            completed_line_odd_r <= 9'h1FF;
            display_line_prev_r <= 9'd0;
            display_active_prev_r <= 1'b0;
        end else begin
            if (fb_vsync) begin
                if (dbg_vsync_raw_r != 8'hFF)
                    dbg_vsync_raw_r <= dbg_vsync_raw_r + 8'd1;
                if (dbg_frame_start_reject_r != 8'hFF)
                    dbg_frame_start_reject_r <= dbg_frame_start_reject_r + 8'd1;
            end

            if (fifo_count_clamped_w > dbg_fifo_highwater_r)
                dbg_fifo_highwater_r <= fifo_count_clamped_w;

            if (wr_drop_w && dbg_fifo_overflow_r != 8'hFF)
                dbg_fifo_overflow_r <= dbg_fifo_overflow_r + 8'd1;

            if (fetch_start_pulse_w && dbg_fetch_start_r != 8'hFF)
                dbg_fetch_start_r <= dbg_fetch_start_r + 8'd1;

            if (fetch_done_pulse_w && dbg_fetch_done_r != 8'hFF)
                dbg_fetch_done_r <= dbg_fetch_done_r + 8'd1;

            if ((fetch_state_r == FETCH_READ) && fifo_busy_w && dbg_yield_busy_r != 8'hFF)
                dbg_yield_busy_r <= dbg_yield_busy_r + 8'd1;

            if ((fetch_state_r == FETCH_READ) && !fifo_busy_w &&
                !fb_read_port.available && dbg_read_blocked_r != 8'hFF)
                dbg_read_blocked_r <= dbg_read_blocked_r + 8'd1;

            if (late_line_w && !late_line_prev_r && dbg_late_line_r != 8'hFF)
                dbg_late_line_r <= dbg_late_line_r + 8'd1;

            late_line_prev_r <= late_line_w;

            if (fetch_done_pulse_w) begin
                if (last_fetched_line_r[0])
                    completed_line_odd_r <= last_fetched_line_r;
                else
                    completed_line_even_r <= last_fetched_line_r;
            end

            if (display_line_step_w && !line_ready_w && dbg_line_not_ready_r != 8'hFF)
                dbg_line_not_ready_r <= dbg_line_not_ready_r + 8'd1;

            if (display_in_active_w && (line_lag_clamped_w > dbg_line_lag_max_r))
                dbg_line_lag_max_r <= line_lag_clamped_w;

            if (ready_phase_err_w && dbg_ready_phase_err_r != 8'hFF)
                dbg_ready_phase_err_r <= dbg_ready_phase_err_r + 8'd1;

            if (display_in_active_w)
                display_line_prev_r <= display_fb_line_w;
            display_active_prev_r <= display_in_active_w;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            fetch_state_r <= FETCH_IDLE;
            last_fetched_line_r <= 9'h1FF;
            fetch_word_r <= 11'd0;
            fetch_addr_r <= 21'd0;
            fetch_bank_r <= 1'b0;
            fetch_beats_left_r <= 4'd0;
        end else begin
            case (fetch_state_r)

            FETCH_IDLE: begin
                // Check every cycle — no cy_changed gate needed.
                // last_fetched_line_r prevents redundant fetches.
                if (cy_approaching_active_w && last_fetched_line_r != 9'd0) begin
                    // Approaching active area — prefetch line 0
                    last_fetched_line_r <= 9'd0;
                    fetch_bank_r <= 1'b0;
                    fetch_word_r <= 11'd0;
                    fetch_beats_left_r <= 4'd0;
                    fetch_addr_r <= FB_BASE_ADDR;
                    fetch_state_r <= FETCH_READ;
                end else if (display_in_active_w &&
                             next_line_w < fb_height[8:0] &&
                             next_line_w != last_fetched_line_r) begin
                    // Displaying line N — prefetch line N+1
                    last_fetched_line_r <= next_line_w;
                    fetch_bank_r <= next_line_w[0];
                    fetch_word_r <= 11'd0;
                    fetch_beats_left_r <= 4'd0;
                    fetch_addr_r <= FB_BASE_ADDR +
                                    {12'd0, next_line_w} * {10'd0, packed_width_w};
                    fetch_state_r <= FETCH_READ;
                end
            end

            FETCH_READ: begin
                // Yield to writes when FIFO has pending entries
                if (!fifo_busy_w && fb_read_port.available) begin
                    fetch_beats_left_r <= fetch_use_burst_w ? 4'(FB_READ_BURST_WORDS) : 4'd1;
                    fetch_state_r <= FETCH_WAIT;
                end
            end

            FETCH_WAIT: begin
                if (fb_read_port.ready) begin
                    if (fetch_word_r == packed_width_w - 11'd1) begin
                        fetch_state_r <= FETCH_IDLE;
                    end else begin
                        fetch_word_r <= fetch_word_r + 11'd1;
                        fetch_addr_r <= fetch_addr_r + 21'd1;
                        fetch_beats_left_r <= fetch_beats_left_r - 4'd1;
                        fetch_state_r <= (fetch_beats_left_r == 4'd1) ? FETCH_READ : FETCH_WAIT;
                    end
                end
            end

            default: fetch_state_r <= FETCH_IDLE;
            endcase
        end
    end

    // Drive read port — only request when not yielding to writes
    assign fb_read_port.addr    = fetch_addr_r;
    assign fb_read_port.data    = 32'd0;
    assign fb_read_port.byte_en = 4'b1111;
    assign fb_read_port.wr      = 1'b0;
    assign fb_read_port.rd      = (fetch_state_r == FETCH_READ) &&
                                   fb_read_port.available && !fifo_busy_w;
    assign fb_read_port.burst   = fetch_use_burst_w;

    assign dbg_fifo_level_o = fifo_count_clamped_w;
    assign dbg_fifo_highwater_o = dbg_fifo_highwater_r;
    assign dbg_fifo_overflow_o = dbg_fifo_overflow_r;
    assign dbg_fetch_start_o = dbg_fetch_start_r;
    assign dbg_fetch_done_o = dbg_fetch_done_r;
    assign dbg_read_blocked_o = dbg_read_blocked_r;
    assign dbg_yield_busy_o = dbg_yield_busy_r;
    assign dbg_late_line_o = dbg_late_line_r;
    assign dbg_line_not_ready_o = dbg_line_not_ready_r;
    assign dbg_line_lag_max_o = dbg_line_lag_max_r;
    assign dbg_ready_phase_err_o = dbg_ready_phase_err_r;
    assign dbg_vsync_raw_o = dbg_vsync_raw_r;
    assign dbg_frame_start_accept_o = dbg_frame_start_accept_r;
    assign dbg_frame_start_reject_o = dbg_frame_start_reject_r;
    assign dbg_flags_o = {
        fifo_full_w,                 // [7]
        fifo_busy_w,                 // [6]
        (fetch_state_r != FETCH_IDLE), // [5]
        fb_read_port.available,      // [4]
        fb_read_port.ready,          // [3]
        fb_write_port.available,     // [2]
        (dbg_fifo_overflow_r != 8'd0), // [1]
        (dbg_line_not_ready_r != 8'd0) // [0]
    };

    // =========================================================================
    // Line Buffer — true dual-port BRAM
    // =========================================================================
    //
    // Two banks of 1024 entries x 18 bits (RGB666). Ping-pong between banks.
    // Write port: clk (54 MHz) — unpacked SDRAM read responses
    // Read port: clk_pixel (27 MHz) — HDMI pixel output

    reg [(2*COLOR_BITS)-1:0] line_buf_pair [0:1023];

    // Write side (clk domain): each 32-bit read beat contains two RGB565 pixels.
    wire lb_wr_w = fb_read_port.ready && (fetch_state_r == FETCH_WAIT);
    wire [9:0] lb_wr_addr_pair_w = {fetch_bank_r, fetch_word_r[8:0]};

    always @(posedge clk) begin
        if (lb_wr_w) begin
            line_buf_pair[lb_wr_addr_pair_w] <= {
                rgb565_to_666(fb_read_port.q[31:16]),  // odd pixel
                rgb565_to_666(fb_read_port.q[15:0])    // even pixel
            };
        end
    end

    // Read side (clk_pixel domain)
    // Bank = line parity, computed directly from hdmi_cy — no CDC needed.
    // fb_line = (hdmi_cy - v_border) >> 1, so fb_line[0] = (hdmi_cy - v_border)[1]
    wire [9:0] display_cy_offset_px_w = hdmi_cy - v_border_px_r;
    wire       rd_bank_w = display_cy_offset_px_w[1];

    // =========================================================================
    // Output path — clk_pixel domain (27 MHz)
    // =========================================================================

    reg [10:0] fb_width_px_r;
    reg [9:0]  fb_height_px_r;
    reg [10:0] h_border_px_r;
    reg [9:0]  v_border_px_r;

    always @(posedge clk_pixel) begin
        fb_width_px_r <= fb_width;
        fb_height_px_r <= fb_height;
        h_border_px_r <= (HDMI_WIDTH - fb_width) >> 1;
        v_border_px_r <= (HDMI_HEIGHT - {fb_height, 1'b0}) >> 1;
    end

    wire in_v_active_px_w = (hdmi_cy >= v_border_px_r) &&
                             (hdmi_cy < v_border_px_r + {fb_height_px_r, 1'b0});
    wire in_h_active_px_w = (hdmi_cx >= {1'b0, h_border_px_r}) &&
                             (hdmi_cx < {1'b0, h_border_px_r} + fb_width_px_r);

    // Use next_cx to compensate for 1-cycle BRAM read latency (matches DDR3 approach).
    // At clock edge N we set the address for pixel at cx+1; the BRAM delivers that
    // data at edge N+1 when hdmi_cx has advanced to match.
    wire [10:0] next_cx_w = hdmi_cx + 11'd1;
    wire        next_in_h_active_w = (next_cx_w >= {1'b0, h_border_px_r}) &&
                                      (next_cx_w < {1'b0, h_border_px_r} + fb_width_px_r);
    wire [9:0]  next_fb_x_w = next_cx_w[9:0] - h_border_px_r[9:0];
    wire [9:0]  lb_rd_addr_pair_w = {rd_bank_w, next_fb_x_w[9:1]};
    reg [9:0] lb_rd_addr_pair_r;
    wire [(2*COLOR_BITS)-1:0] lb_rd_pair_w = line_buf_pair[lb_rd_addr_pair_r];
    reg lb_rd_pixel_odd_r;
    reg in_active_px_r;
    reg scanline_dim_r;
    always @(posedge clk_pixel) begin
        // Match the DDR3 framebuffer read style: register address, then read
        // unregistered BRAM data from that address in the next pixel cycle.
        lb_rd_addr_pair_r <= lb_rd_addr_pair_w;
        lb_rd_pixel_odd_r <= next_fb_x_w[0];
        in_active_px_r <= next_in_h_active_w && in_v_active_px_w;
        scanline_dim_r <= scanline_en && in_v_active_px_w && hdmi_cy[0];
    end
    wire [COLOR_BITS-1:0] lb_rd_data_w = lb_rd_pixel_odd_r
                                        ? lb_rd_pair_w[(2*COLOR_BITS)-1:COLOR_BITS]
                                        : lb_rd_pair_w[COLOR_BITS-1:0];

    wire [23:0] active_rgb_w = torgb(lb_rd_data_w);
    wire [23:0] border_rgb_w = torgb(border_color);

    wire [23:0] pixel_rgb_w = in_active_px_r ? active_rgb_w : border_rgb_w;

    wire [23:0] dimmed_rgb_w = {1'b0, pixel_rgb_w[23:17],
                                 1'b0, pixel_rgb_w[15:9],
                                 1'b0, pixel_rgb_w[7:1]};

    wire [23:0] final_rgb_w = sleep_i ? 24'd0 :
                               scanline_dim_r ? dimmed_rgb_w : pixel_rgb_w;

    assign r_o = final_rgb_w[23:16];
    assign g_o = final_rgb_w[15:8];
    assign b_o = final_rgb_w[7:0];

endmodule
