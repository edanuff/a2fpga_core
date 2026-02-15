`timescale 1ns / 1ps
//
// Apple IIgs Video Graphics Controller — Framebuffer Renderer
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
// Renders IIgs Super Hi-Res Graphics (320x200 and 640x200) to the DDR3
// framebuffer. Self-driven by hsync_i from scan_timer — on each visible
// SHR scanline, fetches the scanline control byte (SCB), 16 palette
// entries, and 40 pixel data words from VGC blockram, then renders
// 640 pixels to fb_we_o/fb_data_o.
//
// In 320 mode, each 4-bit pixel is doubled to produce 640 output pixels.
// In 640 mode, 2-bit pixels with palette pair dithering are rendered
// directly at 640 pixels wide.
//
// Per-scanline budget: ~956 cycles at 54 MHz out of ~3,429 available.
//
// Runs entirely on clk_logic (54 MHz).
//

module vgc_fb (
    a2bus_if.slave a2bus_if,
    a2mem_if.slave a2mem_if,

    video_control_if.display video_control_if,

    // VGC memory interface (to apple_memory blockram)
    output        vgc_active_o,
    output reg [12:0] vgc_address_o,
    output reg        vgc_rd_o,
    input [31:0]      vgc_data_i,

    // Scan timer inputs
    input [8:0]  scanline_i,     // 0-261 from scan_timer
    input        hsync_i,        // pulse at extended_cycle, once per scanline
    input        vsync_i,        // frame reset (scanline 261->0)

    // Framebuffer write outputs
    output reg        fb_we_o,
    output reg [17:0] fb_data_o, // RGB666
    output reg        fb_vsync_o
);

    // =========================================================================
    // VGC active signal — tells apple_memory to route aux BRAM to VGC
    // =========================================================================

    assign vgc_active_o = a2mem_if.SHRG_MODE;

    // =========================================================================
    // State machine
    // =========================================================================

    localparam ST_IDLE      = 3'd0;
    localparam ST_FETCH_SCB = 3'd1;  // 4-cycle SCB fetch
    localparam ST_FETCH_PAL = 3'd2;  // 8 x 4-cycle palette fetches
    localparam ST_RENDER    = 3'd3;  // 40 x (4 fetch + 16 render + 3 stalls) pixel output

    reg [2:0] state_r;

    // Fetch pipeline sub-step: 0=set addr, 1=BRAM wait1, 2=BRAM wait2, 3=latch data, 4=render pixels
    reg [2:0] fetch_step_r;

    // =========================================================================
    // VRAM address constants
    // =========================================================================

    localparam [12:0] SCB_BASE     = 13'd8000;  // scanline control bytes
    localparam [12:0] PALETTE_BASE = 13'd8064;  // palette entries

    // =========================================================================
    // Registers
    // =========================================================================

    // SHRG mode — latched at vsync for frame-coherent rendering
    reg shrg_mode_r;

    // Scanline tracking
    reg [7:0] render_line_r;           // current SHR line (0-199)
    reg [12:0] pix_addr_r;            // pixel data base address for scanline

    // SCB fields
    reg [7:0] scan_ctl_r;
    wire PIX640_w      = scan_ctl_r[7];
    wire COLOR_FILL_w  = scan_ctl_r[5];
    wire [3:0] palette_select_w = scan_ctl_r[3:0];

    // Palette
    reg [11:0] palette_rgb_r [0:15];
    reg [3:0] pal_fetch_cnt_r;        // 0-7 (8 palette word fetches)

    // Pixel rendering
    reg [31:0] pixel_word_r;          // current 32-bit pixel data word
    reg [5:0] pix_word_cnt_r;         // 0-39 (40 words per scanline)
    reg [3:0] pix_sub_cnt_r;          // 0-15 (16 output pixels per word)
    reg [3:0] prev_palette_r;         // for color fill mode

    // =========================================================================
    // Pixel decode logic (from vgc.sv, preserved verbatim)
    // =========================================================================

    // Byte select from 32-bit word based on sub-pixel position
    wire [7:0] pix_byte_w = pix_sub_cnt_r[3:2] == 2'd3 ? pixel_word_r[31:24] :
                             pix_sub_cnt_r[3:2] == 2'd2 ? pixel_word_r[23:16] :
                             pix_sub_cnt_r[3:2] == 2'd1 ? pixel_word_r[15:8]  :
                             pixel_word_r[7:0];

    // 320 mode: 4-bit nibble. pix_sub_cnt_r[0] is ignored, doubling each pixel.
    wire [3:0] pix320_w = pix_sub_cnt_r[1] ? pix_byte_w[3:0] : pix_byte_w[7:4];

    // 640 mode: 2-bit value
    wire [1:0] pix640_w = pix_sub_cnt_r[1:0] == 2'd3 ? pix_byte_w[1:0] :
                           pix_sub_cnt_r[1:0] == 2'd2 ? pix_byte_w[3:2] :
                           pix_sub_cnt_r[1:0] == 2'd1 ? pix_byte_w[5:4] :
                           pix_byte_w[7:6];

    // 640 mode palette pair select
    wire [1:0] pix640_palette_select_w = pix_sub_cnt_r[1:0] == 2'd3 ? 2'b01 :
                                          pix_sub_cnt_r[1:0] == 2'd2 ? 2'b00 :
                                          pix_sub_cnt_r[1:0] == 2'd1 ? 2'b11 :
                                          2'b10;

    // Palette index: 320 mode uses direct 4-bit, 640 mode combines pair + 2-bit
    wire [3:0] pix_palette_w = PIX640_w ? {pix640_palette_select_w, pix640_w} : pix320_w;

    // Color fill: pixel value 0 replaced by previous non-zero pixel's color
    wire [3:0] pix_fill_w = (COLOR_FILL_w && (pix_palette_w == 4'b0000)) ?
                             prev_palette_r : pix_palette_w;

    // Palette lookup: RGB444 -> RGB666
    wire [11:0] pix_rgb_w = palette_rgb_r[pix_fill_w];
    wire [17:0] fb_pixel_w = {pix_rgb_w[11:8], 2'b00,   // R[5:0]
                               pix_rgb_w[7:4],  2'b00,   // G[5:0]
                               pix_rgb_w[3:0],  2'b00};  // B[5:0]

    // =========================================================================
    // Main state machine
    // =========================================================================

    always @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            state_r <= ST_IDLE;
            fetch_step_r <= 3'd0;
            fb_we_o <= 1'b0;
            fb_vsync_o <= 1'b0;
            vgc_rd_o <= 1'b0;
            vgc_address_o <= 13'd0;
            shrg_mode_r <= 1'b0;
            render_line_r <= 8'd0;
            pix_addr_r <= 13'd0;
            scan_ctl_r <= 8'd0;
            pal_fetch_cnt_r <= 4'd0;
            pixel_word_r <= 32'd0;
            pix_word_cnt_r <= 6'd0;
            pix_sub_cnt_r <= 4'd0;
            prev_palette_r <= 4'd0;
        end else begin
            // Default: deassert single-cycle pulses
            fb_we_o <= 1'b0;
            fb_vsync_o <= 1'b0;
            vgc_rd_o <= 1'b0;

            // Frame boundary
            if (vsync_i) begin
                fb_vsync_o <= 1'b1;
                shrg_mode_r <= video_control_if.shrg_mode(a2mem_if.SHRG_MODE);
            end

            case (state_r)

            // -----------------------------------------------------------------
            // IDLE — wait for hsync on a visible SHR scanline
            // -----------------------------------------------------------------
            ST_IDLE: begin
                if (hsync_i && scanline_i < 9'd200 && shrg_mode_r) begin
                    render_line_r <= scanline_i[7:0];
                    // Compute pixel data start address: line * 40 = (line << 5) + (line << 3)
                    pix_addr_r <= {5'b0, scanline_i[7:0]} * 13'd40;
                    prev_palette_r <= 4'd0;
                    fetch_step_r <= 3'd0;
                    state_r <= ST_FETCH_SCB;
                end
            end

            // -----------------------------------------------------------------
            // FETCH SCB — 3-cycle fetch of scanline control byte
            // -----------------------------------------------------------------
            ST_FETCH_SCB: begin
                case (fetch_step_r)
                    3'd0: begin
                        // Set address for SCB word (4 SCBs per 32-bit word)
                        vgc_address_o <= SCB_BASE + {5'b0, render_line_r[7:2]};
                        vgc_rd_o <= 1'b1;
                        fetch_step_r <= 3'd1;
                    end
                    3'd1: begin
                        // BRAM pipeline wait 1
                        fetch_step_r <= 3'd2;
                    end
                    3'd2: begin
                        // BRAM pipeline wait 2
                        fetch_step_r <= 3'd3;
                    end
                    3'd3: begin
                        // Latch SCB byte based on line[1:0]
                        case (render_line_r[1:0])
                            2'd0: scan_ctl_r <= vgc_data_i[7:0];
                            2'd1: scan_ctl_r <= vgc_data_i[15:8];
                            2'd2: scan_ctl_r <= vgc_data_i[23:16];
                            2'd3: scan_ctl_r <= vgc_data_i[31:24];
                        endcase
                        pal_fetch_cnt_r <= 4'd0;
                        fetch_step_r <= 3'd0;
                        state_r <= ST_FETCH_PAL;
                    end
                    default: fetch_step_r <= 3'd0;
                endcase
            end

            // -----------------------------------------------------------------
            // FETCH PALETTE — 8 iterations x 3 cycles = 24 cycles
            // Each read loads 2 palette entries (12-bit RGB444 each)
            // -----------------------------------------------------------------
            ST_FETCH_PAL: begin
                case (fetch_step_r)
                    3'd0: begin
                        // Address: PALETTE_BASE + {palette_select, 3'b000} + fetch_cnt
                        vgc_address_o <= PALETTE_BASE + {palette_select_w, 3'b000} + {9'b0, pal_fetch_cnt_r[2:0]};
                        vgc_rd_o <= 1'b1;
                        fetch_step_r <= 3'd1;
                    end
                    3'd1: begin
                        // BRAM pipeline wait 1
                        fetch_step_r <= 3'd2;
                    end
                    3'd2: begin
                        // BRAM pipeline wait 2
                        fetch_step_r <= 3'd3;
                    end
                    3'd3: begin
                        // Store 2 palette entries from 32-bit word
                        palette_rgb_r[{pal_fetch_cnt_r[2:0], 1'b0}]     <= vgc_data_i[11:0];
                        palette_rgb_r[{pal_fetch_cnt_r[2:0], 1'b0} + 1] <= vgc_data_i[27:16];

                        if (pal_fetch_cnt_r == 4'd7) begin
                            // All 16 palette entries loaded — start pixel rendering
                            pix_word_cnt_r <= 6'd0;
                            pix_sub_cnt_r <= 4'd0;
                            fetch_step_r <= 3'd0;
                            state_r <= ST_RENDER;
                        end else begin
                            pal_fetch_cnt_r <= pal_fetch_cnt_r + 4'd1;
                            fetch_step_r <= 3'd0;
                        end
                    end
                    default: fetch_step_r <= 3'd0;
                endcase
            end

            // -----------------------------------------------------------------
            // RENDER — for each of 40 pixel data words:
            //   fetch_step 0: set address, assert rd
            //   fetch_step 1: BRAM pipeline wait 1
            //   fetch_step 2: BRAM pipeline wait 2
            //   fetch_step 3: latch pixel word
            //   fetch_step 4: render 16 pixels (pix_sub_cnt 0-15)
            //   fetch_step 5: stall after every 4th pixel for wgrp_pending
            // Total: 40 * (4 + 16 + 3 stalls) = 920 cycles
            // -----------------------------------------------------------------
            ST_RENDER: begin
                case (fetch_step_r)
                    3'd0: begin
                        // Set address for current pixel data word
                        vgc_address_o <= pix_addr_r + {7'b0, pix_word_cnt_r};
                        vgc_rd_o <= 1'b1;
                        fetch_step_r <= 3'd1;
                    end
                    3'd1: begin
                        // BRAM pipeline wait 1
                        fetch_step_r <= 3'd2;
                    end
                    3'd2: begin
                        // BRAM pipeline wait 2
                        fetch_step_r <= 3'd3;
                    end
                    3'd3: begin
                        // Latch pixel data word
                        pixel_word_r <= vgc_data_i;
                        pix_sub_cnt_r <= 4'd0;
                        fetch_step_r <= 3'd4;
                    end
                    3'd4: begin
                        // Render one pixel per cycle
                        fb_data_o <= fb_pixel_w;
                        fb_we_o <= 1'b1;

                        // Update color fill tracking
                        prev_palette_r <= pix_fill_w;

                        if (pix_sub_cnt_r == 4'd15) begin
                            // All 16 pixels from this word rendered
                            pix_word_cnt_r <= pix_word_cnt_r + 6'd1;

                            if (pix_word_cnt_r == 6'd39) begin
                                // All 640 pixels done
                                state_r <= ST_IDLE;
                            end else begin
                                // Fetch next word
                                fetch_step_r <= 3'd0;
                            end
                        end else if (pix_sub_cnt_r[1:0] == 2'd3) begin
                            // 4th pixel of group — stall 1 cycle for
                            // ddr3_framebuffer wgrp_pending to clear
                            pix_sub_cnt_r <= pix_sub_cnt_r + 4'd1;
                            fetch_step_r <= 3'd5;
                        end else begin
                            pix_sub_cnt_r <= pix_sub_cnt_r + 4'd1;
                        end
                    end
                    3'd5: begin
                        // Stall cycle — wgrp_pending clears in ddr3_framebuffer
                        fetch_step_r <= 3'd4;
                    end
                    default: fetch_step_r <= 3'd0;
                endcase
            end

            default: state_r <= ST_IDLE;

            endcase
        end
    end

endmodule
