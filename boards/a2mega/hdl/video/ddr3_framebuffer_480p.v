// DDR3-backed framebuffer for Tang Mega 60K — 480p non-scaling output
// Based on ddr3_framebuffer.v by nand2mario, March 2025
// Modified for 480p by Ed Anuff, 2025
//
// - A framebuffer stored in DDR3, displayed at 480p (720x480) with NO upscaling.
// - Pixels are stored at native resolution (e.g. 560x192 or 640x200) and displayed
//   with integer 2x vertical scaling, centered in the 720x480 frame with borders.
// - The DDR3 intermediate buffer solves the beam-racing problem while preserving
//   clean integer-scaled pixel output.
// - Color depth supported: 12, 15, 18, and 24 bits.
// - Dynamic change of framebuffer size is supported (e.g. Apple II 560x192 <-> VGC 640x200).
//
// Internals:
// - Write path: identical to 720p version (async FIFO, 4-pixel grouping, batch DDR3 writes)
// - Read path: line buffer (dual-port BRAM, ping-pong) bridges DDR3 (clk_x1) to HDMI (clk_pixel)
// - 2x vertical scaling: each FB line is fetched once, displayed on 2 consecutive HDMI lines
// - Borders: computed from fb_width/fb_height, centered in 720x480
// - Scanline dimming: odd HDMI lines in active area dimmed to 50%
//
// 480p timings:
//   858 = 720 + 16(front porch) + 62(sync) + 60(back porch)
//   525 = 480 +  9(front porch) +  6(sync) + 30(back porch)

module ddr3_framebuffer_480p #(
    parameter WIDTH = 640,           // multiples of 4
    parameter HEIGHT = 480,
    parameter COLOR_BITS = 18        // RGB666
)(
    input               clk_27,      // 27 MHz input clock (for DDR3 PLL)
    input               clk_g,       // 50 MHz crystal
    input               pll_lock_27,
    input               rst_n,
    output              clk_out,     // 74.25 MHz DDR3 domain clock (not for HDMI)
    output              ddr_rst,     // output reset signal for clk_out
    output              init_calib_complete,

    // HDMI pixel clocks (from board PLL)
    input               clk_pixel,   // 27 MHz pixel clock
    input               clk_pixel_x5,// 135 MHz TMDS 5x clock

    // Framebuffer interface
    input               clk,         // any clock <= 74.25 MHz
    input [10:0]        fb_width,    // actual width of the framebuffer (560 or 640)
    input [9:0]         fb_height,   // actual height of the framebuffer (192 or 200)
    input               fb_vsync,    // start of frame signal
    input               fb_we,       // update a pixel and move to next pixel
    input [COLOR_BITS-1:0] fb_data,  // pixel data
    input [COLOR_BITS-1:0] border_color, // border color for inactive display area
    input               sleep_i,     // when high, output black (clk domain, sync'd internally)

    input [15:0]        sound_left,
    input [15:0]        sound_right,

    // DDR3 interface
    output [14:0]       ddr_addr,
    output [3-1:0]      ddr_bank,
    output              ddr_cs,
    output              ddr_ras,
    output              ddr_cas,
    output              ddr_we,
    output              ddr_ck,
    output              ddr_ck_n,
    output              ddr_cke,
    output              ddr_odt,
    output              ddr_reset_n,
    output [2-1:0]      ddr_dm,
    inout  [16-1:0]     ddr_dq,
    inout  [2-1:0]      ddr_dqs,
    inout  [2-1:0]      ddr_dqs_n,

    // HDMI overlay interface (active in clk_pixel / 27 MHz domain)
    output [10:0]       hdmi_cx,
    output [9:0]        hdmi_cy,
    output [23:0]       fb_rgb_o,
    input  [23:0]       overlay_rgb_i,
    input               overlay_en_i,

    // HDMI output
    output              tmds_clk_n,
    output              tmds_clk_p,
    output [2:0]        tmds_d_n,
    output [2:0]        tmds_d_p
);

`include "config.vh"

/////////////////////////////////////////////////////////////////////
// Clocks
wire memory_clk;
wire clk_x1;
assign clk_out = clk_x1;
wire pll_lock;

// dynamic reconfiguration port from DDR controller to framebuffer PLL
reg wr;     // for mDRP
wire mdrp_inc;
wire [1:0] mdrp_op;
wire [7:0] mdrp_wdata;
wire [7:0] mdrp_rdata;
wire pll_stop;
reg pll_stop_r;

// 297 MHz DDR3 clock from PLL
`ifdef CONSOLE_138K
pll_ddr3 pll_ddr3_inst(
    .clkin(clk_27),
    .clkout0(),
    .clkout2(memory_clk),
    .enclk2(pll_stop),
    .reset(~pll_lock_27),
    .lock(pll_lock),
    .init_clk(clk_g)
);

`else

pll_ddr3 pll_ddr3_inst(
    .lock(pll_lock),
    .clkout0(),
    .clkout2(memory_clk),
    .clkin(clk_27),
    .reset(~pll_lock_27),
    .mdclk(clk_g),
    .mdopc(mdrp_op),
    .mdainc(mdrp_inc),
    .mdwdi(mdrp_wdata),
    .mdrdo(mdrp_rdata)
);

reg mdrp_wr;
reg [7:0] pll_stop_count;
pll_mDRP_intf u_pll_mDRP_intf(
    .clk(clk_g),
    .rst_n(pll_lock_27),
    .pll_lock(pll_lock),
    .wr(mdrp_wr),
    .mdrp_inc(mdrp_inc),
    .mdrp_op(mdrp_op),
    .mdrp_wdata(mdrp_wdata),
    .mdrp_rdata(mdrp_rdata)
);

always@(posedge clk_g) begin
    pll_stop_r <= pll_stop;
    mdrp_wr <= pll_stop ^ pll_stop_r;
    if (pll_stop_r && !pll_stop && pll_stop_count != 8'hff) begin
        pll_stop_count <= pll_stop_count + 1;
    end
end
`endif

// No internal HDMI PLL needed — board PLL provides clk_pixel (27 MHz)
// and clk_pixel_x5 (135 MHz) directly.

/////////////////////////////////////////////////////////////////////
// DDR3 controller

wire           app_rdy;
reg            app_en;
reg    [2:0]   app_cmd;
reg   [27:0]   app_addr;

wire           app_wdf_rdy;
reg            app_wdf_wren;
wire  [15:0]   app_wdf_mask = 0;
wire           app_wdf_end = 1;
reg  [127:0]   app_wdf_data;

wire           app_rd_data_valid;
wire           app_rd_data_end;
wire [127:0]   app_rd_data;

wire           app_sre_req = 0;
wire           app_ref_req = 0;
wire           app_burst = 1;
wire           app_sre_act;
wire           app_ref_ack;

DDR3_Memory_Interface_Top u_ddr3 (
    .memory_clk      (memory_clk),
    .pll_stop        (pll_stop),
    .clk             (clk_g),
    .rst_n           (1'b1),
    .cmd_ready       (app_rdy),
    .cmd             (app_cmd),
    .cmd_en          (app_en),
    .addr            (app_addr),
    .wr_data_rdy     (app_wdf_rdy),
    .wr_data         (app_wdf_data),
    .wr_data_en      (app_wdf_wren),
    .wr_data_end     (app_wdf_end),
    .wr_data_mask    (app_wdf_mask),
    .rd_data         (app_rd_data),
    .rd_data_valid   (app_rd_data_valid),
    .rd_data_end     (app_rd_data_end),
    .sr_req          (0),
    .ref_req         (0),
    .sr_ack          (app_sre_act),
    .ref_ack         (app_ref_ack),
    .init_calib_complete(init_calib_complete),
    .clk_out         (clk_x1),
    .pll_lock        (pll_lock),
    .burst           (app_burst),
    .ddr_rst         (ddr_rst),
    .O_ddr_addr      (ddr_addr),
    .O_ddr_ba        (ddr_bank),
    .O_ddr_cs_n      (ddr_cs),
    .O_ddr_ras_n     (ddr_ras),
    .O_ddr_cas_n     (ddr_cas),
    .O_ddr_we_n      (ddr_we),
    .O_ddr_clk       (ddr_ck),
    .O_ddr_clk_n     (ddr_ck_n),
    .O_ddr_cke       (ddr_cke),
    .O_ddr_odt       (ddr_odt),
    .O_ddr_reset_n   (ddr_reset_n),
    .O_ddr_dqm       (ddr_dm),
    .IO_ddr_dq       (ddr_dq),
    .IO_ddr_dqs      (ddr_dqs),
    .IO_ddr_dqs_n    (ddr_dqs_n)
);

/////////////////////////////////////////////////////////////////////
// Audio — generate clk_audio in clk_pixel (27 MHz) domain

localparam AUDIO_RATE = 48000;
localparam AUDIO_CLK_DELAY = 27000000 / AUDIO_RATE;  // 562 (27M/48k = 562.5)
logic [$clog2(AUDIO_CLK_DELAY)-1:0] audio_divider;
logic clk_audio;
reg audio_frac;  // alternating pattern for fractional 562.5

always_ff @(posedge clk_pixel) begin
    clk_audio <= 1'b0;
    if (audio_divider >= (audio_frac ? AUDIO_CLK_DELAY : AUDIO_CLK_DELAY - 1)) begin
        clk_audio <= 1'b1;
        audio_divider <= 0;
        audio_frac <= ~audio_frac;
    end else begin
        audio_divider <= audio_divider + 1;
    end
end

// Audio CDC: clk (54 MHz) -> clk_pixel (27 MHz) via double-flop
reg [15:0] audio_sample_word [1:0], audio_sample_word0 [1:0];
always @(posedge clk_pixel) begin
    audio_sample_word0[0] <= sound_left;
    audio_sample_word[0] <= audio_sample_word0[0];
    audio_sample_word0[1] <= sound_right;
    audio_sample_word[1] <= audio_sample_word0[1];
end

/////////////////////////////////////////////////////////////////////
// CDC: border_color and sleep from clk to clk_pixel

// Border color CDC (quasi-static)
reg [COLOR_BITS-1:0] border_color_px0, border_color_px;
always @(posedge clk_pixel) begin
    border_color_px0 <= border_color;
    border_color_px <= border_color_px0;
end

// Sleep CDC (quasi-static, 2-stage sync from clk to clk_pixel)
reg sleep_sync0, sleep_sync1;
always @(posedge clk_pixel) begin
    sleep_sync0 <= sleep_i;
    sleep_sync1 <= sleep_sync0;
end

/////////////////////////////////////////////////////////////////////
// HDMI TX — 480p

wire [9:0] cx;
wire [9:0] cy;
reg [23:0] rgb;

assign hdmi_cx = {1'b0, cx};   // extend to 11 bits for port compatibility
assign hdmi_cy = cy;
assign fb_rgb_o = rgb;

wire [2:0] tmds;
localparam VIDEOID = 2;             // 720x480p 59.94Hz
localparam VIDEO_REFRESH = 59.94;
localparam AUDIO_BIT_WIDTH = 16;
localparam AUDIO_OUT_RATE = AUDIO_RATE;

hdmi #( .VIDEO_ID_CODE(VIDEOID),
        .DVI_OUTPUT(0),
        .VIDEO_REFRESH_RATE(VIDEO_REFRESH),
        .IT_CONTENT(1),
        .AUDIO_RATE(AUDIO_OUT_RATE),
        .AUDIO_BIT_WIDTH(AUDIO_BIT_WIDTH),
        .START_X(0),
        .START_Y(0) )

hdmi_inst(
        .clk_pixel_x5(clk_pixel_x5),
        .clk_pixel(clk_pixel),
        .clk_audio(clk_audio),
        .rgb(overlay_en_i ? overlay_rgb_i : rgb),
        .reset(ddr_rst),
        .audio_sample_word(audio_sample_word),
        .tmds(tmds),
        .tmds_clock(),
        .cx(cx),
        .cy(cy),
        .frame_width(),
        .frame_height() );

// Gowin LVDS output buffer
ELVDS_OBUF tmds_bufds [3:0] (
    .I({clk_pixel, tmds}),
    .O({tmds_clk_p, tmds_d_p}),
    .OB({tmds_clk_n, tmds_d_n})
);

/////////////////////////////////////////////////////////////////////
// Framebuffer Write Path (clk -> async FIFO -> clk_x1 -> DDR3)
// This section is identical to the 720p version.

localparam FB_SIZE = WIDTH * HEIGHT;

reg write_pixels_req;
reg write_pixels_ack;
reg [9:0] wr_x, wr_y;
reg [$clog2(FB_SIZE*2)-1:0] wr_addr;

wire fifo_can_read;
wire fifo_can_write;
wire [4*COLOR_BITS-1:0] fifo_data;
wire [6:0] fifo_level;
reg fifo_read;
reg fifo_write;

reg [1:0]  wgrp_cnt;
reg [4*COLOR_BITS-1:0] wgrp_data;
reg        wgrp_pending;

async_fifo #(.BUFFER_ADDR_WIDTH(6), .DATA_WIDTH(4*COLOR_BITS)) u_asyncfifo (
  .reset(ddr_rst),
  .write_clk(clk), .write(fifo_write), .write_data(wgrp_data), .can_write(fifo_can_write),
  .read_clk(clk_x1), .read(fifo_read), .read_data(fifo_data),  .can_read(fifo_can_read),
  .read_available(fifo_level)
);

reg fb_vsync_r;
reg b_vsync_toggle, b_vsync_toggle_r, b_vsync_toggle_rr;

always @(posedge clk) begin
    fb_vsync_r <= fb_vsync;
    if (fb_vsync & ~fb_vsync_r) begin
        b_vsync_toggle <= ~b_vsync_toggle;
    end
end

// Assemble 4 pixels per FIFO word (clk domain)
always @(posedge clk) begin
    fifo_write <= 1'b0;
    if (ddr_rst | ~init_calib_complete) begin
        wgrp_cnt <= 0;
        wgrp_pending <= 0;
        wgrp_data <= 0;
    end else begin
        if (fb_vsync & ~fb_vsync_r) begin
            wgrp_cnt <= 0;
            wgrp_pending <= 0;
        end
        if (fb_we && !wgrp_pending) begin
            case (wgrp_cnt)
                2'd0: begin
                    wgrp_data[COLOR_BITS-1:0] <= fb_data;
                    wgrp_cnt <= 2'd1;
                end
                2'd1: begin
                    wgrp_data[2*COLOR_BITS-1:COLOR_BITS] <= fb_data;
                    wgrp_cnt <= 2'd2;
                end
                2'd2: begin
                    wgrp_data[3*COLOR_BITS-1:2*COLOR_BITS] <= fb_data;
                    wgrp_cnt <= 2'd3;
                end
                2'd3: begin
                    wgrp_data[4*COLOR_BITS-1:3*COLOR_BITS] <= fb_data;
                    wgrp_cnt <= 2'd0;
                    wgrp_pending <= 1'b1;
                end
            endcase
        end
        if (wgrp_pending && fifo_can_write) begin
            fifo_write <= 1'b1;
            wgrp_pending <= 1'b0;
        end
    end
end

// cross vsync toggle to clk_x1 domain
always @(posedge clk_x1) begin
    b_vsync_toggle_rr <= b_vsync_toggle_r;
    b_vsync_toggle_r <= b_vsync_toggle;
end

// Batch write control (clk_x1 domain)
reg write_batch_active;
reg mem_dir_write;
reg [3:0] batch_groups_left;
wire write_inflight = write_pixels_req ^ write_pixels_ack;
wire new_frame = b_vsync_toggle_rr != b_vsync_toggle_r;
reg fifo_draining;
// Forward declarations for read/write arbitration coupling.
reg line_fetch_needed;
reg line_fetch_active;
localparam [6:0] WRITE_BATCH_MIN_LEVEL = 7'd8;
localparam [6:0] WRITE_URGENT_LEVEL = 7'd48;
localparam [3:0] WRITE_BATCH_NORMAL = 4'd8;
localparam [3:0] WRITE_BATCH_URGENT = 4'd4;
wire read_critical = line_fetch_needed || line_fetch_active;
wire write_urgent = fifo_level >= WRITE_URGENT_LEVEL;

always @(posedge clk_x1) begin : write_batch_control
    fifo_read <= 1'b0;
    if (ddr_rst | ~init_calib_complete) begin
        wr_x <= 0; wr_y <= 0;
        write_pixels_req <= 0;
        write_batch_active <= 0;
        mem_dir_write <= 0;
        batch_groups_left <= 0;
        fifo_draining <= 0;

    end else begin
        if (new_frame) begin
            write_batch_active <= 0;
            fifo_draining <= 1'b1;
            mem_dir_write <= 1'b0;
        end else if (fifo_draining) begin
            if (fifo_can_read) begin
                fifo_read <= 1'b1;
            end else if (!write_inflight) begin
                wr_x <= 0; wr_y <= 0;
                fifo_draining <= 0;
            end
        end else begin
            if (!write_batch_active && !mem_dir_write &&
                fifo_level >= WRITE_BATCH_MIN_LEVEL &&
                (!read_critical || write_urgent)) begin
                write_batch_active <= 1'b1;
                mem_dir_write <= 1'b1;
                batch_groups_left <= read_critical ? WRITE_BATCH_URGENT : WRITE_BATCH_NORMAL;
            end

            if (write_batch_active) begin
                if (fifo_can_read && !write_inflight) begin
                    wr_addr <= {wr_y * WIDTH + {wr_x[9:2], 2'b0}, 1'b0};
                    app_wdf_data <= { {(32-COLOR_BITS){1'b0}}, fifo_data[4*COLOR_BITS-1:3*COLOR_BITS],
                                      {(32-COLOR_BITS){1'b0}}, fifo_data[3*COLOR_BITS-1:2*COLOR_BITS],
                                      {(32-COLOR_BITS){1'b0}}, fifo_data[2*COLOR_BITS-1:1*COLOR_BITS],
                                      {(32-COLOR_BITS){1'b0}}, fifo_data[1*COLOR_BITS-1:0] };
                    write_pixels_req <= ~write_pixels_req;
                    fifo_read <= 1'b1;
                    batch_groups_left <= batch_groups_left - 1;
                    if (batch_groups_left == 1) write_batch_active <= 0;
                    wr_x <= wr_x + 4;
                    if (wr_x + 4 >= fb_width) begin
                        wr_x <= 0;
                        wr_y <= wr_y + 1;
                    end
                end
            end

            if (!write_batch_active && mem_dir_write && !write_inflight) begin
                mem_dir_write <= 1'b0;
            end
        end
    end
end

/////////////////////////////////////////////////////////////////////
// Line Buffer — dual-bank BRAM, true dual-port (write: clk_x1, read: clk_pixel)
//
// Bank 0/1 alternate by framebuffer line parity:
// - Display line N reads bank (N[0])
// - While line N is displayed, fetch line N+1 into bank ((N+1)[0])
//
// This avoids same-bank read/write overlap entirely.

localparam LB_ADDR_BITS = 10;  // 2^10 = 1024 entries per bank (>= WIDTH=640)
localparam LB_BANK_BITS = 1;   // 2 banks
reg [COLOR_BITS-1:0] line_buf [0:(1<<(LB_ADDR_BITS+LB_BANK_BITS))-1] /* synthesis syn_ramstyle="block_ram" */;

// Write port (clk_x1 domain) — single pixel per cycle
reg [LB_ADDR_BITS+LB_BANK_BITS-1:0] lb_wr_addr;
reg [COLOR_BITS-1:0] lb_wr_data;
reg lb_wr_en;

always @(posedge clk_x1) begin
    if (lb_wr_en)
        line_buf[lb_wr_addr] <= lb_wr_data;
end

// Read port (clk_pixel domain)
reg [LB_ADDR_BITS+LB_BANK_BITS-1:0] lb_rd_addr;
reg [COLOR_BITS-1:0] lb_rd_data;

always @(posedge clk_pixel) begin
    lb_rd_data <= line_buf[lb_rd_addr];
end

/////////////////////////////////////////////////////////////////////
// DDR3 Read Path — Scanline Prefetch (clk_x1 domain)
//
// Fetches one scanline at a time from DDR3 into the "fill" line buffer.
// With 2x vertical scaling, each FB line is displayed for 2 HDMI lines,
// so we only need to fetch a new line every other scanline.

// CDC: bring cy from clk_pixel to clk_x1 using gray code
// Binary multi-bit CDC via double-flop is unsafe when multiple bits change
// simultaneously (e.g. 47->48), causing glitched intermediate values.
// Gray code ensures only 1 bit changes per increment, making double-flop safe.
reg [9:0] cy_gray;
always @(posedge clk_pixel) begin
    cy_gray <= cy ^ (cy >> 1);  // binary to gray code
end

reg [9:0] cy_gray_sync0, cy_gray_sync1;
always @(posedge clk_x1) begin
    cy_gray_sync0 <= cy_gray;
    cy_gray_sync1 <= cy_gray_sync0;
end

// Gray to binary conversion
wire [9:0] cy_sync1;
assign cy_sync1[9] = cy_gray_sync1[9];
assign cy_sync1[8] = cy_gray_sync1[8] ^ cy_sync1[9];
assign cy_sync1[7] = cy_gray_sync1[7] ^ cy_sync1[8];
assign cy_sync1[6] = cy_gray_sync1[6] ^ cy_sync1[7];
assign cy_sync1[5] = cy_gray_sync1[5] ^ cy_sync1[6];
assign cy_sync1[4] = cy_gray_sync1[4] ^ cy_sync1[5];
assign cy_sync1[3] = cy_gray_sync1[3] ^ cy_sync1[4];
assign cy_sync1[2] = cy_gray_sync1[2] ^ cy_sync1[3];
assign cy_sync1[1] = cy_gray_sync1[1] ^ cy_sync1[2];
assign cy_sync1[0] = cy_gray_sync1[0] ^ cy_sync1[1];

// CDC: bring fb_width and fb_height to clk_x1 (quasi-static)
reg [10:0] fb_width_x1;
reg [9:0]  fb_height_x1;
always @(posedge clk_x1) begin
    fb_width_x1 <= fb_width;
    fb_height_x1 <= fb_height;
end

// Border calculations in clk_x1 domain
wire [9:0] h_border_x1 = (10'd720 - fb_width_x1[9:0]) >> 1;
wire [9:0] v_border_x1 = (10'd480 - {fb_height_x1[8:0], 1'b0}) >> 1;  // fb_height * 2

// Determine which FB line to fetch
wire [9:0] v_active_start_x1 = v_border_x1;
wire [9:0] v_active_end_x1 = 10'd480 - v_border_x1;

wire cy_in_active_x1 = (cy_sync1 >= v_active_start_x1) && (cy_sync1 < v_active_end_x1);
wire [8:0] fb_line_x1 = (cy_sync1 - v_active_start_x1) >> 1;  // 2x vertical: divide by 2

// Line fetch timing for 2x vertical scaling (dual-bank):
//
// Line 0 is prefetched during VBlank. During active display of line N,
// we fetch line N+1 into the opposite bank. This gives a full 2-scanline
// window (~63.6 us) for each line fetch and avoids read/write overlap on
// the bank currently being displayed.

wire cy_is_vblank_before_active = (cy_sync1 < v_active_start_x1) &&
                                   (cy_sync1 + 10'd2 >= v_active_start_x1);

// Track which line was last fetched, and whether cy changed
reg [9:0] cy_prev;
reg [8:0] last_fetched_line;
reg [8:0] fetch_target_line;
reg fetch_bank;
reg [9:0] fetch_write_x;     // current write position into line buffer

// DDR3 read address for current line — computed incrementally to avoid multiplier
reg [$clog2(FB_SIZE*2)-1:0] fetch_line_addr;

// Pipelined read response buffering.
localparam integer RD_FIFO_DEPTH = 32;
localparam integer RD_FIFO_ADDR_BITS = 5;
localparam integer RD_MAX_OUTSTANDING = 8;
reg [127:0] rd_fifo [0:RD_FIFO_DEPTH-1] /* synthesis syn_ramstyle="block_ram" */;
reg [RD_FIFO_ADDR_BITS-1:0] rd_fifo_wr_ptr, rd_fifo_rd_ptr;
reg [RD_FIFO_ADDR_BITS:0] rd_fifo_count;
wire rd_fifo_empty = (rd_fifo_count == 0);
wire rd_fifo_full = (rd_fifo_count == RD_FIFO_DEPTH);
// Keep enough FIFO headroom for all in-flight read responses.
wire rd_fifo_almost_full = (rd_fifo_count >= (RD_FIFO_DEPTH - RD_MAX_OUTSTANDING));

// Response handler: pop 128-bit words from rd_fifo and write 4 pixels sequentially.
reg [127:0] rd_data_latched;
reg [1:0] rd_pixel_idx;
reg rd_pixel_active;
wire rd_fifo_push = app_rd_data_valid && !rd_fifo_full;
wire rd_fifo_pop = !rd_pixel_active && !rd_fifo_empty;

always @(posedge clk_x1) begin
    lb_wr_en <= 1'b0;

    if (ddr_rst | ~init_calib_complete) begin
        cy_prev <= 10'h3FF;
        last_fetched_line <= 9'h1FF;
        line_fetch_needed <= 0;
        line_fetch_active <= 0;
        fetch_target_line <= 0;
        fetch_bank <= 0;
        fetch_write_x <= 0;
        fetch_line_addr <= 0;
        rd_fifo_wr_ptr <= 0;
        rd_fifo_rd_ptr <= 0;
        rd_fifo_count <= 0;
        rd_pixel_idx <= 0;
        rd_pixel_active <= 0;

    end else begin
        // Detect cy transition — fetch new FB line when it changes
        if (cy_sync1 != cy_prev) begin
            cy_prev <= cy_sync1;
            if (!line_fetch_active && !line_fetch_needed) begin
                if (cy_is_vblank_before_active && last_fetched_line != 9'd0) begin
                    // Approaching active area from VBlank — fetch line 0
                    line_fetch_needed <= 1'b1;
                    fetch_target_line <= 9'd0;
                    fetch_write_x <= 0;
                end else if (cy_in_active_x1 && (fb_line_x1 + 9'd1) < fb_height_x1[8:0] &&
                             (fb_line_x1 + 9'd1) != last_fetched_line) begin
                    // Displaying line N: fetch line N+1 into the opposite bank
                    line_fetch_needed <= 1'b1;
                    fetch_target_line <= fb_line_x1 + 9'd1;
                    fetch_write_x <= 0;
                end
            end
        end

        // Start fetching
        if (line_fetch_needed && !line_fetch_active && !mem_dir_write) begin
            line_fetch_active <= 1'b1;
            line_fetch_needed <= 1'b0;
            last_fetched_line <= fetch_target_line;
            fetch_bank <= fetch_target_line[0];
            fetch_line_addr <= {fetch_target_line * WIDTH, 1'b0};
        end

        // Push incoming DDR3 responses into the FIFO.
        if (rd_fifo_push) begin
            rd_fifo[rd_fifo_wr_ptr] <= app_rd_data;
        end

        // Pop a buffered response word when pixel writer is idle.
        if (rd_fifo_pop) begin
            rd_data_latched <= rd_fifo[rd_fifo_rd_ptr];
            rd_pixel_idx <= 0;
            rd_pixel_active <= 1'b1;
        end

        case ({rd_fifo_push, rd_fifo_pop})
            2'b10: begin
                rd_fifo_wr_ptr <= rd_fifo_wr_ptr + 1'b1;
                rd_fifo_count <= rd_fifo_count + 1'b1;
            end
            2'b01: begin
                rd_fifo_rd_ptr <= rd_fifo_rd_ptr + 1'b1;
                rd_fifo_count <= rd_fifo_count - 1'b1;
            end
            2'b11: begin
                rd_fifo_wr_ptr <= rd_fifo_wr_ptr + 1'b1;
                rd_fifo_rd_ptr <= rd_fifo_rd_ptr + 1'b1;
            end
        endcase

        // Write out 4 pixels from each buffered response word.
        if (rd_pixel_active) begin
            lb_wr_en <= 1'b1;
            lb_wr_addr <= {fetch_bank, fetch_write_x[LB_ADDR_BITS-1:0]};
            case (rd_pixel_idx)
                2'd0: lb_wr_data <= rd_data_latched[COLOR_BITS-1:0];
                2'd1: lb_wr_data <= rd_data_latched[32+COLOR_BITS-1:32];
                2'd2: lb_wr_data <= rd_data_latched[64+COLOR_BITS-1:64];
                2'd3: lb_wr_data <= rd_data_latched[96+COLOR_BITS-1:96];
            endcase
            fetch_write_x <= fetch_write_x + 1;
            rd_pixel_idx <= rd_pixel_idx + 1;
            if (rd_pixel_idx == 2'd3) begin
                rd_pixel_active <= 1'b0;
            end
        end

        // Line fetch complete: all reads issued, all responses returned, FIFO drained.
        if (line_fetch_active &&
            (fetch_pixel_x >= fb_width_x1[9:0]) &&
            (rd_outstanding == 0) &&
            !rd_pixel_active && rd_fifo_empty &&
            (fetch_write_x >= fb_width_x1[9:0]) &&
            (fetch_write_x > 0)) begin
            line_fetch_active <= 1'b0;
        end
    end
end

/////////////////////////////////////////////////////////////////////
// DDR3 Read/Write Arbitration (clk_x1 domain)
// Supports multiple in-flight reads to reduce fetch latency.

reg cmd_done, data_done;
reg [9:0] fetch_pixel_x;
reg line_fetch_active_prev;
reg fetch_needs_reset;
reg [3:0] rd_outstanding;
reg rd_issue_fire;

always @(posedge clk_x1) begin : ddr3_rw
    app_en <= 0;
    app_wdf_wren <= 0;
    rd_issue_fire = 1'b0;

    if (ddr_rst | ~init_calib_complete) begin
        cmd_done <= 0;
        data_done <= 0;
        fetch_pixel_x <= 0;
        line_fetch_active_prev <= 0;
        fetch_needs_reset <= 0;
        rd_outstanding <= 0;
    end else begin
        line_fetch_active_prev <= line_fetch_active;

        if (line_fetch_active && !line_fetch_active_prev) begin
            fetch_needs_reset <= 1'b1;
            rd_outstanding <= 0;
        end else begin
            // Writes have priority
            if (write_pixels_req ^ write_pixels_ack) begin
                if (!cmd_done && app_rdy) begin
                    app_en <= 1'b1;
                    app_cmd <= 3'b000;
                    app_addr <= wr_addr;
                    cmd_done <= 1'b1;
                end
                if (!data_done && app_wdf_rdy) begin
                    app_wdf_wren <= 1;
                    data_done <= 1'b1;
                end
                if ((cmd_done | app_rdy) & (data_done | app_wdf_rdy)) begin
                    write_pixels_ack <= write_pixels_req;
                    cmd_done <= 0;
                    data_done <= 0;
                end
            end else if (fetch_needs_reset) begin
                fetch_pixel_x <= 0;
                fetch_needs_reset <= 0;
            end else begin
                if (!mem_dir_write && line_fetch_active &&
                         fetch_pixel_x < fb_width_x1[9:0] && app_rdy &&
                         (rd_outstanding < RD_MAX_OUTSTANDING) &&
                         !rd_fifo_almost_full) begin
                    app_en <= 1;
                    app_cmd <= 3'b001;
                    app_addr <= {fetch_pixel_x, 1'b0} + fetch_line_addr;
                    fetch_pixel_x <= fetch_pixel_x + 4;
                    rd_issue_fire = 1'b1;
                end
            end

            // Track outstanding read responses accurately, including
            // issue+response in the same clock (net 0 delta).
            case ({rd_issue_fire, (app_rd_data_valid && (rd_outstanding != 0))})
                2'b10: rd_outstanding <= rd_outstanding + 1'b1;
                2'b01: rd_outstanding <= rd_outstanding - 1'b1;
                default: rd_outstanding <= rd_outstanding;
            endcase
        end
    end
end

/////////////////////////////////////////////////////////////////////
// 480p Output Path (clk_pixel domain, 27 MHz)
//
// Read from line buffer and apply borders, scanline dimming, sleep.

// Border calculations in clk_pixel domain
reg [9:0] h_border_px, v_border_px;
reg [9:0] h_active_start_px, h_active_end_px;
reg [9:0] v_active_start_px, v_active_end_px;

always @(posedge clk_pixel) begin
    h_border_px <= (10'd720 - fb_width[9:0]) >> 1;
    v_border_px <= (10'd480 - {fb_height[8:0], 1'b0}) >> 1;
    h_active_start_px <= h_border_px;
    h_active_end_px <= 10'd720 - h_border_px;
    v_active_start_px <= v_border_px;
    v_active_end_px <= 10'd480 - v_border_px;
end

wire in_h_active = (cx[9:0] >= h_active_start_px) && (cx[9:0] < h_active_end_px);
wire in_v_active = (cy >= v_active_start_px) && (cy < v_active_end_px);
wire in_active = in_h_active && in_v_active;
wire [8:0] fb_line_px = (cy - v_active_start_px) >> 1;
wire lb_rd_bank = fb_line_px[0];

// Line buffer read address: pixel position within active area
// Needs to be set 1 cycle early for BRAM read latency
wire [9:0] next_cx = cx[9:0] + 1;
wire [9:0] next_fb_x = next_cx - h_active_start_px;

always @(posedge clk_pixel) begin
    // Set read address for next cycle's pixel
    if (next_cx >= h_active_start_px && next_cx < h_active_end_px && in_v_active)
        lb_rd_addr <= {lb_rd_bank, next_fb_x[LB_ADDR_BITS-1:0]};
    else
        lb_rd_addr <= 0;
end

// Line buffer pixel output
wire [23:0] pixel_rgb_raw = torgb(in_active ? lb_rd_data : border_color_px);

// Scanline dimming: odd HDMI lines at 50% brightness
wire dim = cy[0];
wire [23:0] pixel_rgb_dimmed = dim ?
    {1'b0, pixel_rgb_raw[23:17], 1'b0, pixel_rgb_raw[15:9], 1'b0, pixel_rgb_raw[7:1]} :
    pixel_rgb_raw;

always @(posedge clk_pixel) begin
    if (ddr_rst | ~init_calib_complete)
        rgb <= 24'h000000;
    else
        rgb <= sleep_sync1 ? 24'h000000 : pixel_rgb_dimmed;
end

/////////////////////////////////////////////////////////////////////
// Convert color to RGB888
function [23:0] torgb(input [23:0] pixel);
    case (COLOR_BITS)
    12: torgb = {pixel[11:8], 4'b0, pixel[7:4], 4'b0, pixel[3:0], 4'b0};
    15: torgb = {pixel[14:10], 3'b0, pixel[9:5], 3'b0, pixel[4:0], 3'b0};
    18: torgb = {pixel[17:12], 2'b0, pixel[11:6], 2'b0, pixel[5:0], 2'b0};
    21: torgb = {pixel[20:14], 1'b0, pixel[13:7], 1'b0, pixel[6:0], 1'b0};
    24: torgb = pixel;
    default: torgb = 24'hbabeef;
    endcase
endfunction

endmodule

// Based on: https://log.martinatkins.me/2020/06/07/verilog-async-fifo/
// Async FIFO implementation
module async_fifo(
  input                       reset,
  input                       write_clk,
  input                       write,
  input      [DATA_WIDTH-1:0] write_data,
  output reg                  can_write,
  input                       read_clk,
  input                       read,
  output reg [DATA_WIDTH-1:0] read_data,
  output reg                  can_read,
  output     [BUFFER_ADDR_WIDTH:0] read_available
);
    parameter DATA_WIDTH = 16;
    parameter BUFFER_ADDR_WIDTH = 8;
    parameter BUFFER_SIZE = 2 ** BUFFER_ADDR_WIDTH;

    // Our buffer as a whole is accessed by both the write_clk and read_clk
    // domains, but read_clk is only used to access elements >= read_ptr and
    // write_clk only for elements < read_ptr. We're expecting this buffer to
    // be inferred as a dual-port block RAM, so the board-specific top module
    // should choose a suitable buffer size to allow that inference.
    reg [DATA_WIDTH-1:0] buffer[BUFFER_SIZE-1:0] /*xx synthesis syn_ramstyle="block_ram" */ ;

    ///// WRITE CLOCK DOMAIN /////

    // This is an address into the buffer array.
    // It intentionally has one additional bit so we can track wrap-around by
    // comparing with the MSB of read_ptr (or, at least, with the grey-code
    // form that we synchronize over into this clock domain.)
    reg [BUFFER_ADDR_WIDTH:0] write_ptr;
    wire [BUFFER_ADDR_WIDTH-1:0] write_addr = write_ptr[BUFFER_ADDR_WIDTH-1:0]; // truncated version without the wrap bit

    // This is the grey-coded version of write_ptr in the write clock domain.
    reg [BUFFER_ADDR_WIDTH:0] write_ptr_grey_w;

    // This is the grey-coded version of read_ptr in the write clock domain,
    // synchronized over here using module read_ptr_grey_sync declared later.
    wire [BUFFER_ADDR_WIDTH:0] read_ptr_grey_w;

    // Write pointer (and its grey-coded equivalent) increments whenever
    // "write" is set on a clock, as long as our buffer isn't full.
    wire [BUFFER_ADDR_WIDTH:0] next_write_ptr = write_ptr + 1;
    wire [BUFFER_ADDR_WIDTH:0] next_write_ptr_grey_w = (next_write_ptr >> 1) ^ next_write_ptr;
    // Our buffer is full if the read and write addresses are the same but the
    // MSBs (wrap bits) are different. We compare the grey code versions here
    // so we can use our cross-domain-synchronized copy of the read pointer.
    wire current_can_write = write_ptr_grey_w != { ~read_ptr_grey_w[BUFFER_ADDR_WIDTH:BUFFER_ADDR_WIDTH-1], read_ptr_grey_w[BUFFER_ADDR_WIDTH-2:0] };
    wire next_can_write = next_write_ptr_grey_w != { ~read_ptr_grey_w[BUFFER_ADDR_WIDTH:BUFFER_ADDR_WIDTH-1], read_ptr_grey_w[BUFFER_ADDR_WIDTH-2:0] };
    always @(posedge write_clk or posedge reset) begin
        if (reset) begin
            write_ptr <= 0;
            write_ptr_grey_w <= 0;
            can_write <= 1;
        end else begin
            if (write && can_write) begin
                write_ptr <= next_write_ptr;
                write_ptr_grey_w <= next_write_ptr_grey_w;
                can_write <= next_can_write;
            end else begin
                can_write <= current_can_write;
            end
        end
    end

    // If "write" is set on a clock then we commit write_data into the current
    // write address.
    always @(posedge write_clk) begin
        if (write && can_write) begin
            buffer[write_addr] <= write_data;
        end
    end

    ///// READ CLOCK DOMAIN /////

    // This is an address into the buffer array.
    // It intentionally has one additional bit so we can track wrap-around by
    // comparing with the MSB of write_ptr (or, at least, with the grey-code
    // form that we synchronize over into this clock domain.)
    reg [BUFFER_ADDR_WIDTH:0] read_ptr;
    wire [BUFFER_ADDR_WIDTH-1:0] read_addr = read_ptr[BUFFER_ADDR_WIDTH-1:0]; // truncated version without the wrap bit

    // This is the grey-coded version of write_ptr in the read clock domain.
    reg [BUFFER_ADDR_WIDTH:0] read_ptr_grey_r;

    // This is the grey-coded version of write_ptr in the read clock domain,
    // synchronized over here using module write_ptr_grey_sync declared later.
    wire [BUFFER_ADDR_WIDTH:0] write_ptr_grey_r;

    // Convert grey-coded write pointer to binary in read clock domain
    function [BUFFER_ADDR_WIDTH:0] grey2bin;
        input [BUFFER_ADDR_WIDTH:0] g;
        integer i;
        begin
            grey2bin[BUFFER_ADDR_WIDTH] = g[BUFFER_ADDR_WIDTH];
            for (i = BUFFER_ADDR_WIDTH-1; i >= 0; i = i - 1) begin
                grey2bin[i] = grey2bin[i+1] ^ g[i];
            end
        end
    endfunction
    wire [BUFFER_ADDR_WIDTH:0] write_ptr_bin_r = grey2bin(write_ptr_grey_r);
    assign read_available = write_ptr_bin_r - read_ptr;

    // Read pointer (and its grey-coded equivalent) increments whenever
    // "read" is set on a clock, as long as our buffer isn't full.
    wire [BUFFER_ADDR_WIDTH:0] next_read_ptr = read_ptr + 1;
    wire [BUFFER_ADDR_WIDTH:0] next_read_ptr_grey_r = (next_read_ptr >> 1) ^ next_read_ptr;
    // Our buffer is empty if the read and write addresses are the same and the
    // MSBs (wrap bits) are also equal. We compare the grey code versions here
    // so we can use our cross-domain-synchronized copy of the write pointer.
    wire current_can_read = read_ptr_grey_r != write_ptr_grey_r;
    wire next_can_read = next_read_ptr_grey_r != write_ptr_grey_r;
    always @(posedge read_clk or posedge reset) begin
        if (reset) begin
            read_ptr <= 0;
            read_ptr_grey_r <= 0;
            read_data <= 0;
            can_read <= 0;
        end else begin
            if (read) begin
                if (can_read) begin
                    read_ptr <= next_read_ptr;
                    read_ptr_grey_r <= next_read_ptr_grey_r;
                end
                can_read <= next_can_read;
                if (next_can_read) begin
                    read_data <= buffer[next_read_ptr];
                end else begin
                    read_data <= 0;
                end
            end else begin
                can_read <= current_can_read;
                if (current_can_read) begin
                    read_data <= buffer[read_addr];
                end else begin
                    read_data <= 0;
                end
            end
        end
    end

    ///// CROSS-DOMAIN /////

    // Synchronize read_ptr_grey_r into read_ptr_grey_w.
    crossdomain #(.SIZE(BUFFER_ADDR_WIDTH+1)) read_ptr_grey_sync (
        .reset(reset),
        .clk(write_clk),
        .data_in(read_ptr_grey_r),
        .data_out(read_ptr_grey_w)
    );

    // Synchronize write_ptr_grey_w into write_ptr_grey_r.
    crossdomain #(.SIZE(BUFFER_ADDR_WIDTH+1)) write_ptr_grey_sync (
        .reset(reset),
        .clk(read_clk),
        .data_in(write_ptr_grey_w),
        .data_out(write_ptr_grey_r)
    );

endmodule

// This is a generalization of the crossdomain module from earlier that
// now supports a customizable value size, so we can safely transmit multi-bit
// values as long as they are grey coded.
module crossdomain #(parameter SIZE = 1) (
  input reset,
  input clk,
  input [SIZE-1:0] data_in,
  output reg [SIZE-1:0] data_out
);

    reg [SIZE-1:0] data_tmp;

    always @(posedge clk) begin
        if (reset) begin
            {data_out, data_tmp} <= 0;
        end else begin
            {data_out, data_tmp} <= {data_tmp, data_in};
        end
    end

endmodule
