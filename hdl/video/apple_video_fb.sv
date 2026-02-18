`timescale 1ns / 1ps
//
// Apple II Video Framebuffer Renderer
//
// (c) 2023,2024,2025 Ed Anuff <ed@a2fpga.com>
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
// Renders Apple II video to the DDR3 framebuffer using an inline pipeline
// that processes one 28-pixel chunk at a time. On each visible scanline's
// hsync pulse, fetches and expands VRAM data chunk-by-chunk while
// simultaneously outputting pixels from the previous chunk.
//
// Pipeline: fetch next chunk from BRAM (4 cycles), expand through character
// ROM if text (5-6 additional cycles), output 28 pixels at 1 per 4 cycles
// (112 cycles per chunk). Fetch/expand overlaps with pixel output.
//
// Total per-scanline: ~2,250 cycles at 54 MHz = ~41.7 µs, well within the
// ~63.5 µs scanline period.
//
// Runs entirely on clk_logic (54 MHz).
//

module apple_video_fb (
    a2bus_if.slave a2bus_if,
    a2mem_if.slave a2mem_if,

    video_control_if.display video_control_if,

    // VRAM read interface (to apple_memory)
    output reg [15:0] video_address_o,
    output reg video_bank_o,
    output reg video_rd_o,
    input [31:0] video_data_i,

    // Scan timer inputs
    input [8:0]  scanline_i,     // 0-261 from scan_timer
    input        hsync_i,        // pulse at extended_cycle, once per scanline
    input        vsync_i,        // frame reset (scanline 261->0)

    // Framebuffer write outputs
    output reg        fb_we_o,
    output reg [17:0] fb_data_o, // RGB666
    output reg        fb_vsync_o,

    // Apple II RGB output for SuperSprite compositing (expanded from RGB444)
    output reg [7:0]  apple_r_o,
    output reg [7:0]  apple_g_o,
    output reg [7:0]  apple_b_o,
    output reg        apple_active_o,

    // SuperSprite composited input (VDP overlaid on Apple II)
    input [7:0]       ssp_r_i,
    input [7:0]       ssp_g_i,
    input [7:0]       ssp_b_i,
    input             ssp_active_i
);

    localparam FORCE_NIBBLE_COLORS = 1;
    localparam FORCE_MONOCHROME = 0;
    localparam FORCE_GS_PALETTE = 0;

    localparam NUM_CHUNKS = 20;          // 560 / 28
    localparam STEP_LENGTH = 28;
    localparam PIX_BUFFER_SIZE = STEP_LENGTH + 1; // 29
    localparam PIX_HISTORY_SIZE = 8;

    localparam HISTORY_ARTIFACT_OFFSET = 1;
    localparam HISTORY_PIXEL_OFFSET = 4;

    // Number of pixel-processing cycles to run before enabling fb_we_o.
    // Compensates for the HISTORY_PIXEL_OFFSET pipeline delay: the first
    // WARMUP_PIXELS cycles shift real data into the history buffer so that
    // pix_history_r[HISTORY_PIXEL_OFFSET] holds valid data when output begins.
    // apple_video.sv uses SCAN_PIX_OFFSET=32 for this; we use a small warmup.
    localparam WARMUP_PIXELS = 4;

    // ----------------------------------------------------------------------------------------------------------------
    // Video ROM — single-registered, 2-cycle latency (set addr@N, valid@N+2)
    // ----------------------------------------------------------------------------------------------------------------

    reg [7:0] viderom_r[4095:0];
    initial $readmemh("video.hex", viderom_r, 0);
    reg [11:0] viderom_a_r;
    reg [7:0] viderom_d_r;
    // Dedicated always block for character ROM read — required for BSRAM inference.
    // Synthesizer cannot extract memory read patterns from large state machines.
    always @(posedge a2bus_if.clk_logic) viderom_d_r <= ~viderom_r[viderom_a_r];

    reg [22:0] flash_cnt_r;
    always @(posedge a2bus_if.clk_logic) flash_cnt_r <= flash_cnt_r + 1'b1;
    wire flash_clk_w = flash_cnt_r[22];

    // ----------------------------------------------------------------------------------------------------------------
    // Utility functions — verbatim from apple_video.sv
    // ----------------------------------------------------------------------------------------------------------------

    // Regular Hires
    function automatic bit [28:0] expandHires40([31:0] vd);
        reg [28:0] vs;
        case ({vd[23],vd[7]})
            2'b00: vs = {
                            1'b0,
                            vd[22],vd[22],vd[21],vd[21],
                            vd[20],vd[20],vd[19],vd[19],
                            vd[18],vd[18],vd[17],vd[17],
                            vd[16],vd[16],vd[6],vd[6],
                            vd[5],vd[5],vd[4],vd[4],
                            vd[3],vd[3],vd[2],vd[2],
                            vd[1],vd[1],vd[0],vd[0]
            };
            2'b01: vs = {
                            1'b0,
                            vd[22],vd[22],vd[21],vd[21],
                            vd[20],vd[20],vd[19],vd[19],
                            vd[18],vd[18],vd[17],vd[17],
                            vd[16],vd[16],vd[6],vd[5],
                            vd[5],vd[4],vd[4],vd[3],
                            vd[3],vd[2],vd[2],vd[1],
                            vd[1],vd[0],vd[0],1'b0
            };
            2'b10: vs = {
                            vd[22],
                            vd[22],vd[21],vd[21],vd[20],
                            vd[20],vd[19],vd[19],vd[18],
                            vd[18],vd[17],vd[17],vd[16],
                            vd[16],vd[16] & vd[6],vd[6],vd[6],
                            vd[5],vd[5],vd[4],vd[4],
                            vd[3],vd[3],vd[2],vd[2],
                            vd[1],vd[1],vd[0],vd[0]
            };
            2'b11: vs = {
                            vd[22],
                            vd[22],vd[21],vd[21],vd[20],
                            vd[20],vd[19],vd[19],vd[18],
                            vd[18],vd[17],vd[17],vd[16],
                            vd[16],vd[6],vd[6],vd[5],
                            vd[5],vd[4],vd[4],vd[3],
                            vd[3],vd[2],vd[2],vd[1],
                            vd[1],vd[0],vd[0],1'b0
            };
        endcase
        return vs;
    endfunction

    // Double Hires
    function automatic bit [27:0] expandHires80([31:0] vd);
        reg [27:0] vs;
        vs = {
                vd[22:16],
                vd[30:24],
                vd[6:0],
                vd[14:8]
            };
        return vs;
    endfunction

    // Text expansion (character ROM byte → 14 doubled pixels)
    function automatic bit [13:0] expandText40([7:0] vd);
        reg [13:0] vs;
        vs = {
            vd[6],vd[6],
            vd[5],vd[5],
            vd[4],vd[4],
            vd[3],vd[3],
            vd[2],vd[2],
            vd[1],vd[1],
            vd[0],vd[0]
        };
        return vs;
    endfunction

    // Regular Lores
    function automatic bit [27:0] expandLores40([31:0] vd, bit seg);
        reg [27:0] vs;
        case (seg)
            1'b0: vs = {
                vd[19],vd[18],vd[17],vd[16],
                vd[19],vd[18],vd[17],vd[16],
                vd[19],vd[18],vd[17],vd[16],
                vd[19],vd[18],vd[1],vd[0],
                vd[3],vd[2],vd[1],vd[0],
                vd[3],vd[2],vd[1],vd[0],
                vd[3],vd[2],vd[1],vd[0]
            };
            1'b1: vs = {
                vd[23],vd[22],vd[21],vd[20],
                vd[23],vd[22],vd[21],vd[20],
                vd[23],vd[22],vd[21],vd[20],
                vd[23],vd[22],vd[5],vd[4],
                vd[7],vd[6],vd[5],vd[4],
                vd[7],vd[6],vd[5],vd[4],
                vd[7],vd[6],vd[5],vd[4]
            };
        endcase
        return vs;
    endfunction

    // Double Lores
    function automatic bit [27:0] expandLores80([31:0] vd, bit seg);
        reg [27:0] vs;
        case (seg)
            1'b0: vs = {
                vd[16],vd[19],vd[18],vd[17],
                vd[16],vd[19],vd[18],vd[24],
                vd[27],vd[26],vd[25],vd[24],
                vd[27],vd[26],vd[2],vd[1],
                vd[0],vd[3],vd[2],vd[1],
                vd[0],vd[10],vd[9],vd[8],
                vd[11],vd[10],vd[9],vd[8]
            };
            1'b1: vs = {
                vd[20],vd[23],vd[22],vd[21],
                vd[20],vd[23],vd[22],vd[28],
                vd[31],vd[30],vd[29],vd[28],
                vd[31],vd[30],vd[6],vd[5],
                vd[4],vd[7],vd[6],vd[5],
                vd[4],vd[14],vd[13],vd[12],
                vd[15],vd[14],vd[13],vd[12]
            };
        endcase
        return vs;
    endfunction

    // Memory address generation, per Sather
    function automatic bit [15:0] lineaddr([9:0] y, input gr);
        reg [15:0] a;
        a[2:0] = 4'b0;
        a[6:3] = ({ 1'b1, y[6], 1'b1, 1'b1}) +
                 ({ y[7], 1'b1, y[7], 1'b1}) +
                 ({ 3'b000,           y[6]});
        a[9:7] = y[5:3];
        a[14:10] = (hires_mode_r & gr) == 1'b0 ?
            {2'b00, 1'b0, page2_r &  ~store80_r, ~(page2_r &  ~store80_r)} :
            {page2_r &  ~store80_r, ~(page2_r &  ~store80_r), y[2:0]};
        a[15] = 1'b0;
        return a;
    endfunction

    // ----------------------------------------------------------------------------------------------------------------
    // Mode registers — latched once per frame at vsync
    // ----------------------------------------------------------------------------------------------------------------

    reg text_mode_r;
    reg page2_r;
    reg hires_mode_r;
    reg mixed_mode_r;
    reg col80_r;
    reg store80_r;
    reg an3_r;
    reg altchar_r;
    reg video_bank_r;

    reg [3:0] text_color_r;
    reg [3:0] background_color_r;
    reg monochrome_mode_r;
    reg monochrome_dhires_mode_r;
    reg shrg_mode_r;

    // ----------------------------------------------------------------------------------------------------------------
    // Line type
    // ----------------------------------------------------------------------------------------------------------------

    reg [7:0] render_line_r;     // Apple II line 0-191
    wire GR = ~(text_mode_r | (render_line_r[5] & render_line_r[7] & mixed_mode_r));

    localparam [2:0] TEXT40_LINE = 0;
    localparam [2:0] TEXT80_LINE = 1;
    localparam [2:0] LORES40_LINE = 4;
    localparam [2:0] LORES80_LINE = 5;
    localparam [2:0] HIRES40_LINE = 6;
    localparam [2:0] HIRES80_LINE = 7;

    wire [2:0] line_type_w = (!GR & !col80_r) ? TEXT40_LINE :
        (!GR & col80_r) ? TEXT80_LINE :
        (GR & !hires_mode_r & an3_r) ? LORES40_LINE :
        (GR & col80_r & !hires_mode_r & !an3_r) ? LORES80_LINE :
        (GR & !col80_r & hires_mode_r & an3_r) ? HIRES40_LINE :
        (GR & col80_r & hires_mode_r & !an3_r) ? HIRES80_LINE :
        TEXT40_LINE;

    wire lores_line_type_w = (line_type_w == LORES40_LINE) | (line_type_w == LORES80_LINE);
    wire text_line_type_w = (line_type_w == TEXT40_LINE) | (line_type_w == TEXT80_LINE);

    // ----------------------------------------------------------------------------------------------------------------
    // Color pipeline — combinational wires from apple_video.sv
    // ----------------------------------------------------------------------------------------------------------------

    wire GSP = FORCE_GS_PALETTE | a2bus_if.sw_gs;

    reg [11:0] palette_rgb_r[0:31] = '{
        12'h000, 12'h924, 12'h42a, 12'hd4e,
        12'h064, 12'h888, 12'h39e, 12'hcbf,
        12'h450, 12'hc73, 12'h888, 12'hfac,
        12'h3c2, 12'hcd6, 12'h7ec, 12'hfff,
        12'h000, 12'hd03, 12'h009, 12'hd2d,
        12'h072, 12'h555, 12'h22f, 12'h6af,
        12'h850, 12'hf60, 12'haaa, 12'hf98,
        12'h1d0, 12'hff0, 12'h4f9, 12'hfff
    } /* synthesis syn_romstyle = "distributed_rom" */;

    reg [3:0] artifact_r[0:127] = '{
        4'h0,4'h0,4'h0,4'h0,4'h8,4'h0,4'h0,4'h0,4'h1,4'h1,4'h5,4'h1,4'h9,4'h9,4'hd,4'hf,
        4'h2,4'h2,4'h6,4'h6,4'ha,4'ha,4'he,4'he,4'h3,4'h3,4'h3,4'h3,4'hb,4'hb,4'hf,4'hf,
        4'h0,4'h0,4'h4,4'h4,4'hc,4'hc,4'hc,4'hc,4'h5,4'h5,4'h5,4'h5,4'h9,4'h9,4'hd,4'hf,
        4'h0,4'h2,4'h6,4'h6,4'he,4'ha,4'he,4'he,4'h7,4'h7,4'h7,4'h7,4'hf,4'hf,4'hf,4'hf,
        4'h0,4'h0,4'h0,4'h0,4'h8,4'h8,4'h8,4'h8,4'h1,4'h1,4'h5,4'h1,4'h9,4'h9,4'hd,4'hf,
        4'h0,4'h2,4'h6,4'h6,4'ha,4'ha,4'ha,4'ha,4'h3,4'h3,4'h3,4'h3,4'hb,4'hb,4'hf,4'hf,
        4'h0,4'h0,4'h4,4'h4,4'hc,4'hc,4'hc,4'hc,4'h1,4'h1,4'h5,4'h5,4'h9,4'h9,4'hd,4'hd,
        4'h0,4'h2,4'h6,4'h6,4'he,4'ha,4'he,4'he,4'hf,4'hf,4'hf,4'h7,4'hf,4'hf,4'hf,4'hf
    };

    wire BW = FORCE_MONOCHROME | monochrome_mode_r | monochrome_dhires_mode_r;

    // Combinational artifact lookup — extracted from state machine for ROM16 inference.
    wire [6:0] artifact_window_w = pix_history_r[HISTORY_ARTIFACT_OFFSET + 6:HISTORY_ARTIFACT_OFFSET];
    wire [3:0] artifact_data_w = artifact_r[artifact_window_w];

    // Combinational palette lookup — extracted from state machine for ROM16 inference.
    // Address is registered in gap_cnt_r==0; data is valid in gap_cnt_r==1.
    reg [4:0] palette_addr_r;
    wire [11:0] palette_data_w = palette_rgb_r[palette_addr_r];

    // ----------------------------------------------------------------------------------------------------------------
    // Nibble helper
    // ----------------------------------------------------------------------------------------------------------------

    function automatic bit [3:0] calcNibble(
        input [PIX_HISTORY_SIZE-1:0] hist,
        input is_lores,
        input [2:0] step7,
        input [1:0] step4,
        input [3:0] prev_nibble
    );
        reg [3:0] n;
        if (is_lores & (step7 == 3'd4)) begin
            case (step4)
                2'b00: n = hist[7:4];
                2'b01: n = {hist[6:4], hist[7]};
                2'b10: n = {hist[5:4], hist[7:6]};
                2'b11: n = {hist[4], hist[7:5]};
            endcase
        end else if (!is_lores & (step4 == 2'd0)) begin
            n = hist[7:4];
        end else begin
            n = prev_nibble;
        end
        return n;
    endfunction

    // ----------------------------------------------------------------------------------------------------------------
    // State machine
    // ----------------------------------------------------------------------------------------------------------------

    localparam ST_IDLE   = 1'b0;
    localparam ST_ACTIVE = 1'b1;

    reg state_r;

    // Inline pipeline counters
    reg [4:0] chunk_r;           // current output chunk (0-19, 20=done)
    reg [4:0] pix_cnt_r;         // pixel position within chunk (0-27)
    reg [1:0] gap_cnt_r;         // 4-cycle gap counter (0=write, 1-3=gap)
    reg [3:0] fe_step_r;         // fetch/expand pipeline step (0-10)
    reg       fe_done_r;         // fetch/expand complete for next chunk
    reg       primed_r;          // first chunk loaded, pixel output active

    // Working registers for fetch/expand pipeline
    reg [PIX_BUFFER_SIZE-1:0] next_pix_buffer_r;  // expanded data for next chunk
    reg       next_pix_delay_r;  // HIRES40 delay bit for next chunk
    reg [31:0] video_data_r;     // latched BRAM data during fetch/expand
    reg [5:0] h_offset_r;        // horizontal chunk offset (chunk * 2)
    reg [4:0] fe_chunk_r;        // which chunk we're fetching (chunk_r + 1 during steady state)

    // Scanline pixel counter — gates fb_we_o at exactly 560 pulses
    reg [9:0] scanline_pix_cnt_r;

    // SSP capture flag: defers fb_data_o/fb_we_o by 1 clk_logic cycle so that
    // the combinational SuperSprite path sees the updated apple_r_o value.
    reg ssp_capture_r;

    // Pixel output pending flag: set during gap_cnt_r==0 when a pixel is ready,
    // consumed during gap_cnt_r==1 after palette_data_w has settled.
    reg pixel_output_pending_r;

    // Pixel pipeline
    reg [PIX_BUFFER_SIZE-1:0] pix_shift_r;
    reg [PIX_HISTORY_SIZE-1:0] pix_history_r;
    reg       pix_delay_r;
    reg [1:0] pix_step4_r;
    reg [2:0] pix_step7_r;
    reg [3:0] pix_nibble_r;
    reg [3:0] pix_color_r;

    // Line address base
    reg [15:0] line_base_r;

    // Render line
    // (render_line_r declared in Line type section above)

    // ----------------------------------------------------------------------------------------------------------------
    // Character ROM address helper
    // ----------------------------------------------------------------------------------------------------------------

    function automatic bit [11:0] charRomAddr(
        input [7:0] char_byte,
        input [2:0] line
    );
        return {
            1'b0,
            char_byte[7] | (char_byte[6] & flash_clk_w & ~altchar_r),
            char_byte[6] & (altchar_r | char_byte[7]),
            char_byte[5:0],
            line
        };
    endfunction

    // ----------------------------------------------------------------------------------------------------------------
    // Main state machine
    // ----------------------------------------------------------------------------------------------------------------

    wire visible_w = (scanline_i < 9'd192);

    // GR computed from incoming scanline_i (for use at ST_IDLE → ST_ACTIVE transition)
    wire next_GR = ~(text_mode_r | (scanline_i[5] & scanline_i[7] & mixed_mode_r));

    wire [2:0] next_line_type_w = (!next_GR & !col80_r) ? TEXT40_LINE :
        (!next_GR & col80_r) ? TEXT80_LINE :
        (next_GR & !hires_mode_r & an3_r) ? LORES40_LINE :
        (next_GR & col80_r & !hires_mode_r & !an3_r) ? LORES80_LINE :
        (next_GR & !col80_r & hires_mode_r & an3_r) ? HIRES40_LINE :
        (next_GR & col80_r & hires_mode_r & !an3_r) ? HIRES80_LINE :
        TEXT40_LINE;

    always @(posedge a2bus_if.clk_logic) begin
        if (!a2bus_if.system_reset_n) begin
            state_r <= ST_IDLE;
            fb_we_o <= 1'b0;
            fb_vsync_o <= 1'b0;
            fb_data_o <= 18'd0;
            apple_r_o <= 8'd0;
            apple_g_o <= 8'd0;
            apple_b_o <= 8'd0;
            apple_active_o <= 1'b0;
            video_rd_o <= 1'b0;
            video_address_o <= 16'd0;
            video_bank_o <= 1'b0;
            viderom_a_r <= 12'd0;

            chunk_r <= 5'd0;
            pix_cnt_r <= 5'd0;
            gap_cnt_r <= 2'd0;
            fe_step_r <= 4'd0;
            fe_done_r <= 1'b0;
            primed_r <= 1'b0;
            fe_chunk_r <= 5'd0;
            next_pix_buffer_r <= '0;
            next_pix_delay_r <= 1'b0;
            video_data_r <= 32'd0;
            h_offset_r <= 6'd0;
            scanline_pix_cnt_r <= 10'd0;
            ssp_capture_r <= 1'b0;
            pixel_output_pending_r <= 1'b0;
            palette_addr_r <= 5'd0;

            pix_shift_r <= '0;
            pix_history_r <= '0;
            pix_delay_r <= 1'b0;
            pix_step4_r <= 2'd0;
            pix_step7_r <= 3'd0;
            pix_nibble_r <= 4'd0;
            pix_color_r <= 4'd0;
            render_line_r <= 8'd0;
            line_base_r <= 16'd0;

            text_mode_r <= 1'b1;
            page2_r <= 1'b0;
            hires_mode_r <= 1'b0;
            mixed_mode_r <= 1'b0;
            col80_r <= 1'b0;
            store80_r <= 1'b0;
            an3_r <= 1'b1;
            altchar_r <= 1'b0;
            video_bank_r <= 1'b0;
            text_color_r <= 4'hF;
            background_color_r <= 4'h0;
            monochrome_mode_r <= 1'b0;
            monochrome_dhires_mode_r <= 1'b0;
            shrg_mode_r <= 1'b0;
        end else begin

            fb_we_o <= 1'b0;
            fb_vsync_o <= 1'b0;
            apple_active_o <= 1'b0;
            video_rd_o <= 1'b0;

            // SSP capture: 1 clk_logic cycle after apple_r_o is set,
            // the SuperSprite combinational path has propagated and
            // ssp_r_i/g_i/b_i are valid. Capture and write to framebuffer.
            if (ssp_capture_r) begin
                fb_data_o <= {ssp_r_i[7:2], ssp_g_i[7:2], ssp_b_i[7:2]};
                fb_we_o <= 1'b1;
                ssp_capture_r <= 1'b0;
            end

            // Vsync: latch mode registers and emit fb_vsync early
            // (70 non-visible scanlines before first pixel gives the
            // ddr3_framebuffer ample time to drain FIFO and reset wr_x)
            if (vsync_i) begin
                fb_vsync_o <= 1'b1;
                video_bank_r             <= video_control_if.enable;
                text_mode_r             <= video_control_if.text_mode(a2mem_if.TEXT_MODE);
                page2_r                 <= video_control_if.page2(a2mem_if.PAGE2);
                hires_mode_r            <= video_control_if.hires_mode(a2mem_if.HIRES_MODE);
                mixed_mode_r            <= video_control_if.mixed_mode(a2mem_if.MIXED_MODE);
                col80_r                 <= video_control_if.col80(a2mem_if.COL80);
                store80_r               <= video_control_if.store80(a2mem_if.STORE80);
                an3_r                   <= video_control_if.an3(a2mem_if.AN3);
                altchar_r               <= video_control_if.altchar(a2mem_if.ALTCHAR);

                text_color_r            <= video_control_if.text_color(a2mem_if.TEXT_COLOR);
                background_color_r      <= video_control_if.background_color(a2mem_if.BACKGROUND_COLOR);
                monochrome_mode_r       <= video_control_if.monochrome_mode(a2mem_if.MONOCHROME_MODE);
                monochrome_dhires_mode_r <= video_control_if.monochrome_dhires_mode(a2mem_if.MONOCHROME_DHIRES_MODE);
                shrg_mode_r             <= video_control_if.shrg_mode(a2mem_if.SHRG_MODE);
            end

            case (state_r)

            // ================================================================
            ST_IDLE: begin
                if (hsync_i && visible_w) begin
                    render_line_r <= scanline_i[7:0];
                    // Compute GR using incoming scanline (not stale render_line_r)
                    line_base_r <= lineaddr({2'b0, scanline_i[7:0]}, next_GR);

                    // Initialize all pipeline state
                    chunk_r <= 5'd0;
                    pix_cnt_r <= 5'd0;
                    gap_cnt_r <= 2'd0;
                    fe_step_r <= 4'd0;
                    fe_done_r <= 1'b0;
                    primed_r <= 1'b0;
                    fe_chunk_r <= 5'd0;
                    h_offset_r <= 6'd0;
                    next_pix_delay_r <= 1'b0;
                    scanline_pix_cnt_r <= 10'd0;
                    ssp_capture_r <= 1'b0;
                    pixel_output_pending_r <= 1'b0;

                    pix_shift_r <= '0;
                    // Reset pix_history_r to zeros at the start of each scanline.
                    // apple_video.sv carries history across scanlines but compensates
                    // with a 32-pixel head start (SCAN_PIX_OFFSET) that flushes stale
                    // data before visible output. We have no head start, so we must
                    // clear the history to avoid the previous scanline's trailing
                    // pixels appearing as a wrap-around at the start of the next line.
                    pix_history_r <= '0;
                    pix_delay_r <= 1'b0;
                    // Use next_line_type_w (from incoming scanline) not line_type_w (from stale render_line_r)
                    pix_step4_r <= (next_line_type_w == HIRES80_LINE || next_line_type_w == LORES80_LINE) ? 2'd1 : 2'd0;
                    pix_step7_r <= 3'd0;
                    // Set initial pix_color_r appropriate for the scanline mode to
                    // avoid mode-transition artifacts on the first pixel
                    pix_color_r <= next_GR ? 4'h0 : background_color_r;

                    state_r <= ST_ACTIVE;
                end
            end

            // ================================================================
            ST_ACTIVE: begin

                // ============================================================
                // FETCH/EXPAND PIPELINE — runs every cycle when not done
                // ============================================================
                if (!fe_done_r) begin
                    case (fe_step_r)
                        // --- BRAM fetch (common to all modes) ---
                        4'd0: begin
                            video_address_o <= line_base_r + {9'd0, h_offset_r};
                            video_bank_o <= video_bank_r;
                            video_rd_o <= 1'b1;
                            fe_step_r <= 4'd1;
                        end
                        4'd1: fe_step_r <= 4'd2;  // BRAM pipeline stage 1
                        4'd2: fe_step_r <= 4'd3;  // BRAM pipeline stage 2

                        4'd3: begin
                            // BRAM data valid — latch and expand for graphics, or
                            // latch for text modes to proceed with ROM lookups
                            video_data_r <= video_data_i;
                            case (line_type_w)
                                HIRES40_LINE: begin
                                    next_pix_buffer_r <= expandHires40(video_data_i);
                                    next_pix_delay_r <= video_data_i[7];
                                    fe_done_r <= 1'b1;
                                end
                                HIRES80_LINE: begin
                                    next_pix_buffer_r[27:0] <= expandHires80(video_data_i);
                                    next_pix_buffer_r[28] <= 1'b0;
                                    next_pix_delay_r <= 1'b0;
                                    fe_done_r <= 1'b1;
                                end
                                LORES40_LINE: begin
                                    next_pix_buffer_r[27:0] <= expandLores40(video_data_i, render_line_r[2]);
                                    next_pix_buffer_r[28] <= 1'b0;
                                    next_pix_delay_r <= 1'b0;
                                    fe_done_r <= 1'b1;
                                end
                                LORES80_LINE: begin
                                    next_pix_buffer_r[27:0] <= expandLores80(video_data_i, render_line_r[2]);
                                    next_pix_buffer_r[28] <= 1'b0;
                                    next_pix_delay_r <= 1'b0;
                                    fe_done_r <= 1'b1;
                                end
                                default: begin
                                    // TEXT40 or TEXT80: proceed to ROM lookups
                                    fe_step_r <= 4'd4;
                                end
                            endcase
                        end

                        // --- TEXT40: 2 character ROM lookups ---
                        4'd4: begin
                            case (line_type_w)
                                TEXT40_LINE: begin
                                    viderom_a_r <= charRomAddr(video_data_r[7:0], render_line_r[2:0]);
                                    fe_step_r <= 4'd5;
                                end
                                TEXT80_LINE: begin
                                    viderom_a_r <= charRomAddr(video_data_r[7:0], render_line_r[2:0]);
                                    fe_step_r <= 4'd5;
                                end
                                default: begin
                                    fe_done_r <= 1'b1;
                                end
                            endcase
                        end

                        4'd5: begin
                            case (line_type_w)
                                TEXT40_LINE: begin
                                    viderom_a_r <= charRomAddr(video_data_r[23:16], render_line_r[2:0]);
                                    fe_step_r <= 4'd6;
                                end
                                TEXT80_LINE: begin
                                    viderom_a_r <= charRomAddr(video_data_r[15:8], render_line_r[2:0]);
                                    fe_step_r <= 4'd6;
                                end
                                default: fe_done_r <= 1'b1;
                            endcase
                        end

                        4'd6: begin
                            case (line_type_w)
                                TEXT40_LINE: begin
                                    next_pix_buffer_r[13:0] <= expandText40(viderom_d_r);
                                    next_pix_buffer_r[28] <= 1'b0;
                                    next_pix_delay_r <= 1'b0;
                                    fe_step_r <= 4'd7;
                                end
                                TEXT80_LINE: begin
                                    // Char 0 (video_data_r[7:0]) → [13:7]
                                    // In TEXT80, aux memory (odd chars) goes to lower
                                    // buffer positions for correct display order:
                                    // aux col N at [6:0], main col N at [13:7]
                                    next_pix_buffer_r[13:7] <= viderom_d_r[6:0];
                                    next_pix_buffer_r[28] <= 1'b0;
                                    next_pix_delay_r <= 1'b0;
                                    viderom_a_r <= charRomAddr(video_data_r[23:16], render_line_r[2:0]);
                                    fe_step_r <= 4'd7;
                                end
                                default: fe_done_r <= 1'b1;
                            endcase
                        end

                        4'd7: begin
                            case (line_type_w)
                                TEXT40_LINE: begin
                                    next_pix_buffer_r[27:14] <= expandText40(viderom_d_r);
                                    fe_done_r <= 1'b1;
                                end
                                TEXT80_LINE: begin
                                    // Char 1 (video_data_r[15:8]) → [6:0]
                                    next_pix_buffer_r[6:0] <= viderom_d_r[6:0];
                                    viderom_a_r <= charRomAddr(video_data_r[31:24], render_line_r[2:0]);
                                    fe_step_r <= 4'd8;
                                end
                                default: fe_done_r <= 1'b1;
                            endcase
                        end

                        4'd8: begin
                            // Char 2 (video_data_r[23:16]) → [27:21]
                            next_pix_buffer_r[27:21] <= viderom_d_r[6:0];
                            fe_step_r <= 4'd9;
                        end

                        4'd9: begin
                            // Char 3 (video_data_r[31:24]) → [20:14]
                            next_pix_buffer_r[20:14] <= viderom_d_r[6:0];
                            fe_done_r <= 1'b1;
                        end

                        default: fe_done_r <= 1'b1;
                    endcase
                end

                // ============================================================
                // PRIMING: load first chunk before starting pixel output
                // ============================================================
                if (!primed_r) begin
                    if (fe_done_r) begin
                        // First chunk is ready — load shift register
                        pix_shift_r <= next_pix_buffer_r;
                        pix_delay_r <= next_pix_delay_r;
                        primed_r <= 1'b1;

                        // Start fetching chunk 1
                        fe_chunk_r <= 5'd1;
                        h_offset_r <= 6'd2;
                        fe_step_r <= 4'd0;
                        fe_done_r <= 1'b0;

                        // Initialize gap counter so first pixel writes on next cycle
                        gap_cnt_r <= 2'd3;
                    end
                end

                // ============================================================
                // PIXEL OUTPUT — active when primed
                //
                // Two-phase pixel pipeline:
                //   gap_cnt_r==0: compute color, register palette address
                //   gap_cnt_r==1: palette_data_w valid, output pixel to FB
                //
                // pix_history_r is cleared at scanline start to prevent
                // previous scanline's trailing pixels from wrapping around.
                // The first WARMUP_PIXELS cycles run the pipeline to prime
                // the history buffer before fb_we_o begins.
                //
                // Total pixel cycles: WARMUP_PIXELS + 560.
                // fb_we_o pulses: exactly 560.
                // ============================================================
                else begin
                    gap_cnt_r <= gap_cnt_r + 2'd1;

                    // --- Phase 1: Color computation (gap_cnt_r == 0) ---
                    if (gap_cnt_r == 2'd0) begin

                        // Compute post-shift history (what pix_history_r will
                        // become after this cycle's non-blocking assignment)
                        reg [PIX_HISTORY_SIZE-1:0] next_history;
                        reg [3:0] color;
                        reg [3:0] next_rot_artifact;
                        reg [3:0] current_nibble;

                        next_history = {pix_shift_r[0], pix_history_r[PIX_HISTORY_SIZE-1:1]};

                        // Artifact color from PRE-shift history via module-scope
                        // combinational wire (artifact_data_w), extracted for ROM16
                        // inference. Phase-sensitive: must use pix_history_r, not
                        // next_history, to avoid swapping artifact color palette.
                        case (pix_step4_r)
                            2'b00: next_rot_artifact = artifact_data_w;
                            2'b01: next_rot_artifact = {artifact_data_w[2:0], artifact_data_w[3]};
                            2'b10: next_rot_artifact = {artifact_data_w[1:0], artifact_data_w[3:2]};
                            2'b11: next_rot_artifact = {artifact_data_w[0], artifact_data_w[3:1]};
                        endcase

                        // Shift history with current pixel
                        pix_history_r <= next_history;

                        // Compute nibble inline (combinationally) matching
                        // apple_video.sv's pix_nibble_w wire from getCurrentNibble().
                        // Uses pre-shift history (pix_history_r) for same-cycle result.
                        current_nibble = calcNibble(
                            pix_history_r,
                            lores_line_type_w,
                            pix_step7_r, pix_step4_r, pix_nibble_r
                        );

                        // Inline color computation from post-shift history
                        // (minimum delay — same cycle as shift)
                        color = background_color_r;
                        if (BW)
                            color = next_history[HISTORY_PIXEL_OFFSET] ? 4'hF : 4'h0;
                        else if (!GR)
                            color = next_history[HISTORY_PIXEL_OFFSET] ? text_color_r : background_color_r;
                        else if (lores_line_type_w)
                            color = (FORCE_NIBBLE_COLORS | a2bus_if.sw_gs) ? current_nibble : next_rot_artifact;
                        else
                            color = next_rot_artifact;

                        pix_color_r <= color;

                        // Register palette address for ROM16-inferred lookup.
                        // palette_data_w will be valid next cycle (gap_cnt_r==1).
                        palette_addr_r <= {GSP, color};

                        // Set pending flag if this pixel is in the output window
                        if (scanline_pix_cnt_r >= WARMUP_PIXELS && scanline_pix_cnt_r < (WARMUP_PIXELS + 10'd560))
                            pixel_output_pending_r <= 1'b1;
                        else
                            pixel_output_pending_r <= 1'b0;

                        // Always advance the pixel cycle counter
                        scanline_pix_cnt_r <= scanline_pix_cnt_r + 10'd1;

                        // Register the inline-computed nibble for use as
                        // prev_nibble on the next cycle's calcNibble() call.
                        pix_nibble_r <= current_nibble;

                        // Shift pixel data
                        pix_shift_r <= {1'b0, pix_shift_r[PIX_BUFFER_SIZE-1:1]};

                        // Advance step counters every pixel cycle, including warmup.
                        // In apple_video.sv, step counters run unconditionally from
                        // scan_start. With WARMUP_PIXELS=4, un-gated advancement means:
                        // - pix_step7_r reaches 4 at the first output pixel, aligning
                        //   nibble capture (calcNibble at step7==4) with byte boundaries.
                        // - pix_step4_r wraps fully (4 mod 4 = 0), preserving NTSC
                        //   artifact color phase for HIRES modes.
                        pix_step4_r <= pix_step4_r + 2'd1;
                        pix_step7_r <= (pix_step7_r == 3'd6) ? 3'd0 : pix_step7_r + 3'd1;

                        // Advance pixel counter within chunk
                        pix_cnt_r <= pix_cnt_r + 5'd1;

                        // End of chunk: wrap pixel counter, advance chunk counter
                        if (pix_cnt_r == STEP_LENGTH - 1) begin
                            pix_cnt_r <= 5'd0;
                            chunk_r <= chunk_r + 5'd1;
                        end
                    end

                    // --- Phase 2: Palette output + scanline complete (gap_cnt_r == 1) ---
                    if (gap_cnt_r == 2'd1) begin
                        // palette_data_w is now valid (addr was registered last cycle).
                        // Output pixel to framebuffer and SuperSprite interface.
                        if (pixel_output_pending_r) begin
                            pixel_output_pending_r <= 1'b0;

                            // Expose Apple II RGB expanded to 8-bit for SuperSprite input.
                            // Use {nibble, 4'b0} so that SSP pass-through truncated to [7:2]
                            // produces {nibble, 2'b00}, matching the direct RGB444→RGB666 path
                            // and the border color expansion.
                            apple_r_o <= {palette_data_w[11:8], 4'b0};
                            apple_g_o <= {palette_data_w[7:4], 4'b0};
                            apple_b_o <= {palette_data_w[3:0], 4'b0};
                            apple_active_o <= 1'b1;

                            // When SuperSprite is active, defer the framebuffer write by
                            // 1 clk_logic cycle so the combinational SSP path sees the
                            // updated apple_r_o. Without this delay, the first pixel of
                            // each scanline reads stale (black) apple_r_o, creating a
                            // 1-pixel notch artifact.
                            if (ssp_active_i) begin
                                ssp_capture_r <= 1'b1;
                            end else begin
                                fb_data_o <= {palette_data_w[11:8], 2'b00, palette_data_w[7:4], 2'b00, palette_data_w[3:0], 2'b00};
                                fb_we_o <= 1'b1;
                            end
                        end

                        // Scanline complete check
                        if (scanline_pix_cnt_r == (WARMUP_PIXELS + 10'd560)) begin
                            state_r <= ST_IDLE;
                        end
                    end

                    // --- Chunk boundary: load next shift register (gap_cnt_r == 3) ---
                    if (gap_cnt_r == 2'd3 && pix_cnt_r == 5'd0 && chunk_r != 5'd0 && chunk_r < NUM_CHUNKS) begin
                        pix_shift_r <= {next_pix_buffer_r[PIX_BUFFER_SIZE-1:1],
                                        next_pix_delay_r ? pix_shift_r[0] : next_pix_buffer_r[0]};
                        pix_delay_r <= next_pix_delay_r;

                        // Start fetching next chunk (if not the last)
                        if (fe_chunk_r < NUM_CHUNKS) begin
                            fe_chunk_r <= fe_chunk_r + 5'd1;
                            h_offset_r <= h_offset_r + 6'd2;
                            fe_step_r <= 4'd0;
                            fe_done_r <= 1'b0;
                        end
                    end
                end

            end // ST_ACTIVE

            default: state_r <= ST_IDLE;
            endcase
        end
    end

endmodule
