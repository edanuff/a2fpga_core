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
// Unlike the DDR3 framebuffer (ddr3_framebuffer_480p.v), this module is
// significantly simpler because the SDRAM controller runs on the same 54 MHz
// logic clock — no async FIFO or complex CDC is needed for the write path.
//
// Architecture:
//   Write path: accepts fb_we/fb_data from renderers (apple_video_fb, vgc_fb),
//               writes pixels sequentially to SDRAM via FB_WRITE_PORT.
//   Read path:  line fetch FSM prefetches scanlines from SDRAM into a dual-port
//               BRAM line buffer. The HDMI output reads from the line buffer at
//               pixel clock rate (27 MHz).
//   CDC:        only needed for the line buffer (54 MHz write, 27 MHz read)
//               using true dual-port BRAM with independent clocks.
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
    input  logic        sleep_i          // Output black when high
);

    // =========================================================================
    // 480p display parameters
    // =========================================================================

    localparam HDMI_WIDTH  = 720;
    localparam HDMI_HEIGHT = 480;

    // =========================================================================
    // RGB666 to RGB888 expansion
    // =========================================================================

    function automatic [23:0] torgb(input [COLOR_BITS-1:0] c);
        // Expand 6-bit channels to 8-bit: {c[5:0], c[5:4]}
        return {c[17:12], c[17:16],   // R
                c[11:6],  c[11:10],   // G
                c[5:0],   c[5:4]};    // B
    endfunction

    // =========================================================================
    // Write path — clk domain (54 MHz)
    // =========================================================================
    //
    // Accepts pixels from renderers and writes them to SDRAM sequentially.
    // Since both the renderer and SDRAM controller run at 54 MHz, no FIFO
    // is needed — we write directly through the mem_port_if.
    //
    // If the port is busy (available=0), we buffer one pixel and retry.

    reg [10:0] wr_x_r;
    reg [9:0]  wr_y_r;
    reg [10:0] wr_width_r;           // latched at vsync

    // Pending write buffer (for when port is busy)
    reg        wr_pending_r;
    reg [COLOR_BITS-1:0] wr_pending_data_r;
    reg [20:0] wr_pending_addr_r;

    // Compute linear address from (x, y) position
    // Address = FB_BASE_ADDR + y * fb_width + x
    // We use a registered multiply to avoid timing issues
    reg [20:0] wr_line_base_r;       // FB_BASE_ADDR + y * fb_width

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_x_r <= 11'd0;
            wr_y_r <= 10'd0;
            wr_width_r <= 11'd560;
            wr_pending_r <= 1'b0;
            wr_pending_data_r <= '0;
            wr_pending_addr_r <= '0;
            wr_line_base_r <= FB_BASE_ADDR;
        end else begin
            // Try to drain pending write first
            if (wr_pending_r && fb_write_port.available) begin
                wr_pending_r <= 1'b0;
            end

            if (fb_vsync) begin
                wr_x_r <= 11'd0;
                wr_y_r <= 10'd0;
                wr_width_r <= fb_width;
                wr_line_base_r <= FB_BASE_ADDR;
            end else if (fb_we) begin
                // Buffer the write
                wr_pending_r <= 1'b1;
                wr_pending_data_r <= fb_data;
                wr_pending_addr_r <= wr_line_base_r + {10'd0, wr_x_r};

                // Advance position
                if (wr_x_r == wr_width_r - 11'd1) begin
                    wr_x_r <= 11'd0;
                    wr_y_r <= wr_y_r + 10'd1;
                    wr_line_base_r <= wr_line_base_r + {10'd0, wr_width_r};
                end else begin
                    wr_x_r <= wr_x_r + 11'd1;
                end
            end
        end
    end

    // Drive write port
    assign fb_write_port.addr    = wr_pending_addr_r;
    assign fb_write_port.data    = {14'd0, wr_pending_data_r};
    assign fb_write_port.byte_en = 4'b1111;
    assign fb_write_port.wr      = wr_pending_r && fb_write_port.available;
    assign fb_write_port.rd      = 1'b0;

    // =========================================================================
    // CDC: HDMI cy/cx → clk domain (gray-code)
    // =========================================================================

    // We need to know the current HDMI line in the clk domain to trigger
    // line fetches. Gray-code CDC for the 10-bit cy value.

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

    // Pixel clock domain: encode cy as gray
    reg [9:0] cy_gray_px_r;
    always @(posedge clk_pixel) begin
        cy_gray_px_r <= bin2gray(hdmi_cy);
    end

    // Logic clock domain: double-flop synchronizer + decode
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
    // Monitors the HDMI display line (via CDC). When a new framebuffer line
    // needs to be displayed, fetches the entire scanline from SDRAM into the
    // line buffer BRAM.
    //
    // With 2x vertical scaling, each FB line covers 2 HDMI lines.
    // We fetch when the HDMI line changes to a new FB line.

    // Border calculations (centered in 720x480)
    wire [10:0] h_border_w = (HDMI_WIDTH - fb_width) >> 1;   // 80 or 40
    wire [9:0]  v_border_w = (HDMI_HEIGHT - {fb_height, 1'b0}) >> 1;  // 48 or 40
    // fb_height << 1 for 2x scaling

    localparam FETCH_IDLE     = 2'd0;
    localparam FETCH_READ     = 2'd1;
    localparam FETCH_WAIT     = 2'd2;

    reg [1:0]  fetch_state_r;
    reg [9:0]  fetch_line_r;          // last fetched FB line
    reg        fetch_line_valid_r;    // have we fetched at least one line?
    reg [10:0] fetch_x_r;            // current x position within scanline
    reg [20:0] fetch_addr_r;         // current SDRAM read address
    reg        fetch_bank_r;         // which line buffer bank to write into

    // Compute which FB line the current HDMI cy corresponds to
    wire [9:0] cy_minus_border_w = cy_sync_r - v_border_w;
    wire [8:0] target_fb_line_w = cy_minus_border_w[9:1];  // divide by 2 for 2x scaling
    wire       in_v_active_w = (cy_sync_r >= v_border_w) &&
                                (cy_sync_r < v_border_w + {fb_height, 1'b0});
    wire       need_fetch_w = in_v_active_w &&
                               (!fetch_line_valid_r || target_fb_line_w != fetch_line_r[8:0]);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            fetch_state_r <= FETCH_IDLE;
            fetch_line_r <= 10'd0;
            fetch_line_valid_r <= 1'b0;
            fetch_x_r <= 11'd0;
            fetch_addr_r <= 21'd0;
            fetch_bank_r <= 1'b0;
        end else begin
            case (fetch_state_r)

            FETCH_IDLE: begin
                if (need_fetch_w) begin
                    fetch_line_r <= {1'b0, target_fb_line_w};
                    fetch_line_valid_r <= 1'b1;
                    fetch_x_r <= 11'd0;
                    // Compute start address: FB_BASE + line * width
                    fetch_addr_r <= FB_BASE_ADDR +
                                    {12'd0, target_fb_line_w} * {10'd0, fb_width};
                    fetch_bank_r <= ~fetch_bank_r;  // swap bank
                    fetch_state_r <= FETCH_READ;
                end

                // Reset valid flag at vsync (start of new frame)
                if (fb_vsync) begin
                    fetch_line_valid_r <= 1'b0;
                end
            end

            FETCH_READ: begin
                if (fb_read_port.available) begin
                    fetch_state_r <= FETCH_WAIT;
                end
            end

            FETCH_WAIT: begin
                if (fb_read_port.ready) begin
                    // Data received — advance to next pixel
                    if (fetch_x_r == fb_width - 11'd1) begin
                        // Line complete
                        fetch_state_r <= FETCH_IDLE;
                    end else begin
                        fetch_x_r <= fetch_x_r + 11'd1;
                        fetch_addr_r <= fetch_addr_r + 21'd1;
                        fetch_state_r <= FETCH_READ;
                    end
                end
            end

            default: fetch_state_r <= FETCH_IDLE;
            endcase
        end
    end

    // Drive read port
    assign fb_read_port.addr    = fetch_addr_r;
    assign fb_read_port.data    = 32'd0;
    assign fb_read_port.byte_en = 4'b1111;
    assign fb_read_port.wr      = 1'b0;
    assign fb_read_port.rd      = (fetch_state_r == FETCH_READ) && fb_read_port.available;

    // =========================================================================
    // Line Buffer — true dual-port BRAM
    // =========================================================================
    //
    // Two banks of 1024 entries x 18 bits. Ping-pong between banks to prevent
    // read/write collision across clock domains.
    //
    // Write port: clk (54 MHz) — SDRAM read responses
    // Read port: clk_pixel (27 MHz) — HDMI pixel output

    reg [COLOR_BITS-1:0] line_buf [0:2047];  // 2 banks x 1024

    // Write side (clk domain): write SDRAM read data into line buffer
    wire [10:0] lb_wr_addr_w = {fetch_bank_r, fetch_x_r[9:0]};
    wire [COLOR_BITS-1:0] lb_wr_data_w = fb_read_port.q[COLOR_BITS-1:0];
    wire lb_wr_en_w = fb_read_port.ready && (fetch_state_r == FETCH_WAIT);

    always @(posedge clk) begin
        if (lb_wr_en_w) begin
            line_buf[lb_wr_addr_w] <= lb_wr_data_w;
        end
    end

    // Read side (clk_pixel domain): read pixels for HDMI output
    // Use the opposite bank from the one being written
    reg        rd_bank_r;

    // Synchronize fetch_bank_r from clk to clk_pixel domain
    reg fetch_bank_sync1_r, fetch_bank_sync2_r;
    always @(posedge clk_pixel) begin
        fetch_bank_sync1_r <= fetch_bank_r;
        fetch_bank_sync2_r <= fetch_bank_sync1_r;
        rd_bank_r <= ~fetch_bank_sync2_r;  // read from opposite bank
    end

    // =========================================================================
    // Output path — clk_pixel domain (27 MHz)
    // =========================================================================

    // Synchronize fb_width and fb_height to pixel clock domain
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

    // Active region detection
    wire in_h_active_px_w = (hdmi_cx >= h_border_px_r) &&
                             (hdmi_cx < h_border_px_r + fb_width_px_r);
    wire in_v_active_px_w = (hdmi_cy >= v_border_px_r) &&
                             (hdmi_cy < v_border_px_r + {fb_height_px_r, 1'b0});
    wire in_active_px_w = in_h_active_px_w && in_v_active_px_w;

    // Line buffer read address
    wire [9:0] lb_rd_x_w = hdmi_cx[9:0] - h_border_px_r[9:0];
    wire [10:0] lb_rd_addr_w = {rd_bank_r, lb_rd_x_w};

    // Read from line buffer (registered for timing)
    reg [COLOR_BITS-1:0] lb_rd_data_r;
    always @(posedge clk_pixel) begin
        lb_rd_data_r <= line_buf[lb_rd_addr_w];
    end

    // Delay active/border signals by 1 cycle to match line buffer read latency
    reg in_active_px_d1_r;
    reg scanline_dim_d1_r;
    always @(posedge clk_pixel) begin
        in_active_px_d1_r <= in_active_px_w;
        // Scanline dimming: dim odd HDMI lines within active area
        scanline_dim_d1_r <= scanline_en && in_v_active_px_w && hdmi_cy[0];
    end

    // Final pixel output
    wire [23:0] active_rgb_w = torgb(lb_rd_data_r);
    wire [23:0] border_rgb_w = torgb(border_color);

    wire [23:0] pixel_rgb_w = in_active_px_d1_r ? active_rgb_w : border_rgb_w;

    // Apply scanline dimming (shift right by 1 = 50% brightness)
    wire [23:0] dimmed_rgb_w = {1'b0, pixel_rgb_w[23:17],
                                 1'b0, pixel_rgb_w[15:9],
                                 1'b0, pixel_rgb_w[7:1]};

    wire [23:0] final_rgb_w = sleep_i ? 24'd0 :
                               scanline_dim_d1_r ? dimmed_rgb_w : pixel_rgb_w;

    assign r_o = final_rgb_w[23:16];
    assign g_o = final_rgb_w[15:8];
    assign b_o = final_rgb_w[7:0];

endmodule
