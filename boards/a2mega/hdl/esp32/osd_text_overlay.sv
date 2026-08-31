//
// OSD text overlay — renders the ESP32's 40x24 menu/console text page over
// the framebuffer output (clk_pixel domain).
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
// The a2n20v2-Enhanced BL616 shows its menu by writing the Apple II shadowed
// text page in SDRAM and flipping the video path. On the a2mega the shadow
// lives in DDR3 behind the port arbiter, so the ESP32 instead writes 40x24
// Apple II screen codes into a dedicated BSRAM page (XFER SPACE 1, linear
// y*40+x) and this module paints it directly over the framebuffer output —
// opaque when enabled, so the menu is visible even when the Apple II video
// path is dead.
//
// Characters are 7x8 Apple II glyphs from the shared video ROM (video.hex).
// H_REP/V_REP set how many screen pixels/lines each glyph pixel/line spans,
// so the console matches the Apple II display scale exactly: the framebuffer
// scan-out runs x3 horizontal / x5 vertical of the 2x-doubled source, i.e.
// 6 screen px per glyph px and 5 lines per glyph line — 42x40 cells,
// 1680x960 at (120,60), the SAME rectangle as the Apple video. The legacy
// 480p geometry is H_REP=2/V_REP=2 (14x16 cells, 560x384).
// Screen-code semantics (inverse $00-$3F, flash $40-$7F) match the Apple II.
//
// Each cell k prefetches the character for cell k+1: the text RAM read
// (registered, external port) and the font ROM read each take a cycle, and
// the assembled row byte is latched at the cell boundary. Cell 0 starts one
// cell before X_OFFSET so column 0 is prefetched during the border.
//
module osd_text_overlay #(
    parameter X_OFFSET = 80,   // left edge of the 40-column window
    parameter Y_OFFSET = 48,   // top edge of the 24-row window
    parameter H_REP    = 2,    // screen pixels per glyph pixel (cell = 7*H_REP)
    parameter V_REP    = 2     // screen lines per glyph line (cell = 8*V_REP)
)(
    input  wire        clk_i,
    input  wire        reset_n,

    input  wire        enable_i,

    input  wire [11:0] screen_x_i,
    input  wire [10:0] screen_y_i,

    // OSD text page read port (registered read in this clock domain)
    output reg  [10:0] vram_addr_o,
    input  wire [7:0]  vram_data_i,

    // RGB input and output
    input  wire [7:0]  r_i,
    input  wire [7:0]  g_i,
    input  wire [7:0]  b_i,

    output reg  [7:0]  r_o,
    output reg  [7:0]  g_o,
    output reg  [7:0]  b_o
);

    localparam CELL_W = 7 * H_REP;
    localparam [11:0] X_START = 12'(X_OFFSET - CELL_W);

    // ------------------------------------------------------------------------
    // Font ROM — shared Apple II video ROM, same addressing as apple_video_gen
    // ------------------------------------------------------------------------
    // block_rom: at 148.5 MHz (1080p) a LUT-inferred 4Kx8 ROM is a deep
    // mux chain and misses timing by ~3 ns. The read must be a PURE sync
    // read for BSRAM inference — the glyph inversion moved to the
    // consumer (8 NOT gates, free), since ~rom[addr] in the read path
    // blocks the block-ROM mapping.
    (* syn_romstyle = "block_rom" *) reg [7:0] viderom_r [4095:0];
    initial $readmemh("video.hex", viderom_r, 0);
    reg [11:0] viderom_a_r;
    reg [7:0]  viderom_d_r;
    always @(posedge clk_i) viderom_d_r <= viderom_r[viderom_a_r];

    // Flash cadence (~2 Hz at 27 MHz)
    reg [23:0] flash_cnt_r;
    always @(posedge clk_i) flash_cnt_r <= flash_cnt_r + 1'b1;
    wire flash_clk_w = flash_cnt_r[23];

    // Screen code -> ROM address (altchar = 0, matches apple_video_gen)
    function automatic bit [11:0] charRomAddr(
        input [7:0] char_byte,
        input [2:0] line
    );
        return {
            1'b0,
            char_byte[7] | (char_byte[6] & flash_clk_w),
            char_byte[6] & char_byte[7],
            char_byte[5:0],
            line
        };
    endfunction

    // ------------------------------------------------------------------------
    // Vertical trackers — incremental off the once-per-line X_START pulse
    // (same no-divider pattern as the framebuffer scan-out scaler; V_REP is
    // not a power of two at Apple scale so rel_y bit-selects can't work)
    // ------------------------------------------------------------------------
    reg [2:0] vrep_r;       // 0..V_REP-1 within a glyph line
    reg [2:0] glyph_line_r; // 0..7 within a character row
    reg [4:0] char_row_r;   // 0..23
    reg       vactive_r;

    // row * 40 = (row << 5) + (row << 3)
    wire [10:0] row_base = ({6'b0, char_row_r} << 5) + ({6'b0, char_row_r} << 3);

    // ------------------------------------------------------------------------
    // Horizontal cell walker with one-cell prefetch
    // ------------------------------------------------------------------------
    reg [2:0] hrep_r;      // 0..H_REP-1 within a glyph pixel
    reg [2:0] gpx_r;       // 0..6 glyph pixel within a cell
    reg [5:0] cell_r;      // 0 = prefetch cell (border), 1-40 = visible columns 0-39
    reg       running_r;

    reg [7:0] row_byte_r;  // glyph row currently being displayed

    always @(posedge clk_i or negedge reset_n) begin
        if (!reset_n) begin
            hrep_r <= 3'd0;
            gpx_r <= 3'd0;
            cell_r <= 6'd0;
            running_r <= 1'b0;
            vrep_r <= 3'd0;
            glyph_line_r <= 3'd0;
            char_row_r <= 5'd0;
            vactive_r <= 1'b0;
            vram_addr_o <= 11'd0;
            viderom_a_r <= 12'd0;
            row_byte_r <= 8'd0;
        end else begin
            if (screen_x_i == X_START) begin
                hrep_r <= 3'd0;
                gpx_r <= 3'd0;
                cell_r <= 6'd0;
                // vertical advance happens once per line, here
                if (screen_y_i == 11'(Y_OFFSET)) begin
                    vrep_r <= 3'd0;
                    glyph_line_r <= 3'd0;
                    char_row_r <= 5'd0;
                    vactive_r <= 1'b1;
                    running_r <= 1'b1;
                end else if (vactive_r) begin
                    if (vrep_r == 3'(V_REP - 1)) begin
                        vrep_r <= 3'd0;
                        if (glyph_line_r == 3'd7) begin
                            glyph_line_r <= 3'd0;
                            if (char_row_r == 5'd23) begin
                                vactive_r <= 1'b0;
                                running_r <= 1'b0;
                            end else begin
                                char_row_r <= char_row_r + 5'd1;
                                running_r <= 1'b1;
                            end
                        end else begin
                            glyph_line_r <= glyph_line_r + 3'd1;
                            running_r <= 1'b1;
                        end
                    end else begin
                        vrep_r <= vrep_r + 3'd1;
                        running_r <= 1'b1;
                    end
                end else begin
                    running_r <= 1'b0;
                end
            end else if (running_r) begin
                if (hrep_r == 3'(H_REP - 1)) begin
                    hrep_r <= 3'd0;
                    if (gpx_r == 3'd6) begin
                        gpx_r <= 3'd0;
                        if (cell_r == 6'd40)
                            running_r <= 1'b0;
                        else
                            cell_r <= cell_r + 6'd1;
                    end else begin
                        gpx_r <= gpx_r + 3'd1;
                    end
                end else begin
                    hrep_r <= hrep_r + 3'd1;
                end
            end

            // Prefetch pipeline for the character shown in the next cell:
            //   cell start (gpx 0, hrep 0): address the text RAM
            //   gpx 1, hrep 0: registered char code valid -> address font ROM
            //   cell end: latch the glyph row for display in the next cell
            if (running_r && cell_r < 6'd40) begin
                if (gpx_r == 3'd0 && hrep_r == 3'd0)
                    vram_addr_o <= row_base + {5'b0, cell_r};
                if (gpx_r == 3'd1 && hrep_r == 3'd0)
                    viderom_a_r <= charRomAddr(vram_data_i, glyph_line_r);
            end
            if (running_r && gpx_r == 3'd6 && hrep_r == 3'(H_REP - 1))
                row_byte_r <= (cell_r < 6'd40) ? ~viderom_d_r : 8'd0;
        end
    end

    // ------------------------------------------------------------------------
    // Pixel output — opaque when enabled
    // ------------------------------------------------------------------------
    wire in_text_w = running_r && (cell_r >= 6'd1);
    // Glyph bit 0 is the leftmost pixel
    wire pixel_w = in_text_w && row_byte_r[gpx_r];

    always @(posedge clk_i) begin
        if (enable_i) begin
            r_o <= pixel_w ? 8'hFF : 8'h00;
            g_o <= pixel_w ? 8'hFF : 8'h00;
            b_o <= pixel_w ? 8'hFF : 8'h00;
        end else begin
            r_o <= r_i;
            g_o <= g_i;
            b_o <= b_i;
        end
    end

endmodule
