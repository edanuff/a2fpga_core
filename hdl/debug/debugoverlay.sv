module DebugOverlay #(
    parameter [8*14-1:0] VERSION = "00000000000000", // 14 ASCII characters
    parameter bit ENABLE = 1'b1,
    parameter NUM_HEX_BYTES = 8,        // Number of hex bytes to display
    parameter X_OFFSET = 16,
    parameter Y_OFFSET = 24
)(
    input  wire        clk_i,
    input  wire        reset_n,

    input wire enable_i,

    // widened for 1080p rasters (narrower connections zero-extend)
    input  wire [11:0] screen_x_i,
    input  wire [10:0] screen_y_i,

    // 8 hex bytes to display
    input wire [7:0] hex_values[NUM_HEX_BYTES],

    // 2 bit fields to display
    input  wire [7:0]  debug_bits_0_i,
    input  wire [7:0]  debug_bits_1_i,

    // RGB input and output
    input  wire [7:0]  r_i,
    input  wire [7:0]  g_i,
    input  wire [7:0]  b_i,

    output reg  [7:0]  r_o,
    output reg  [7:0]  g_o,
    output reg  [7:0]  b_o
);

    // Constants
    localparam CHAR_WIDTH  = 8;
    localparam CHAR_HEIGHT = 8;
    localparam NUM_CHARS   = 14;          // Number of characters in VERSION string
    localparam NUM_BITS_FIELDS = 2;       // Number of bit fields
    localparam NUM_BITS_PER_FIELD = 8;    // Number of bits per field
    localparam DEBUG_SPACE = 8;           // Space between debug values

    // Character ROM: 16 hex digits (0-9, A-F) x 8 rows
    reg [7:0] char_rom [128] = '{
        // Digit 0
        8'b00011100, 8'b00100010, 8'b00110010, 8'b00101010,
        8'b00100110, 8'b00100010, 8'b00011100, 8'b00000000,
        // Digit 1
        8'b00001000, 8'b00001100, 8'b00001000, 8'b00001000,
        8'b00001000, 8'b00001000, 8'b00011100, 8'b00000000,
        // Digit 2
        8'b00111100, 8'b00100010, 8'b00100000, 8'b00011000,
        8'b00000100, 8'b00000010, 8'b00111110, 8'b00000000,
        // Digit 3
        8'b00111110, 8'b00100000, 8'b00010000, 8'b00011000,
        8'b00100000, 8'b00100010, 8'b00011100, 8'b00000000,
        // Digit 4
        8'b00010000, 8'b00011000, 8'b00010100, 8'b00010010,
        8'b00111110, 8'b00010000, 8'b00010000, 8'b00000000,
        // Digit 5
        8'b00111110, 8'b00000010, 8'b00011110, 8'b00100000,
        8'b00100000, 8'b00100010, 8'b00011100, 8'b00000000,
        // Digit 6
        8'b00111000, 8'b00000100, 8'b00000010, 8'b00011110,
        8'b00100010, 8'b00100010, 8'b00011100, 8'b00000000,
        // Digit 7
        8'b00111110, 8'b00100000, 8'b00010000, 8'b00001000,
        8'b00000100, 8'b00000100, 8'b00000100, 8'b00000000,
        // Digit 8
        8'b00011100, 8'b00100010, 8'b00100010, 8'b00011100,
        8'b00100010, 8'b00100010, 8'b00011100, 8'b00000000,
        // Digit 9
        8'b00011100, 8'b00100010, 8'b00100010, 8'b00111100,
        8'b00100000, 8'b00010000, 8'b00001110, 8'b00000000,
        // Letter A
        8'b00001000, 8'b00010100, 8'b00100010, 8'b00100010,
        8'b00111110, 8'b00100010, 8'b00100010, 8'b00000000,
        // Letter B
        8'b00011110, 8'b00100010, 8'b00100010, 8'b00111110,
        8'b00100010, 8'b00100010, 8'b00011110, 8'b00000000,
        // Letter C
        8'b00011100, 8'b00100010, 8'b00000010, 8'b00000010,
        8'b00000010, 8'b00100010, 8'b00011100, 8'b00000000,
        // Letter D
        8'b00011110, 8'b00100010, 8'b00100010, 8'b00100010,
        8'b00100010, 8'b00100010, 8'b00011110, 8'b00000000,
        // Letter E
        8'b00111110, 8'b00000010, 8'b00000010, 8'b00111110,
        8'b00000010, 8'b00000010, 8'b00111110, 8'b00000000,
        // Letter F
        8'b00111110, 8'b00000010, 8'b00000010, 8'b00111110,
        8'b00000010, 8'b00000010, 8'b00000010, 8'b00000000
    };

    //=========================================================================
    // PIPELINE STAGE 0: input registration. At 148.5 MHz (1080p) the
    // route from the DP core's cx/cy counters into the stage-1 region
    // ladder is the critical path; starting the ladder from local flops
    // recovers it. Shifts the whole overlay one pixel right — irrelevant
    // for a debug surface.
    //=========================================================================
    // syn_keep: without it synthesis merges these with the framebuffer's
    // identical cx/cy input registers, recreating the long cross-module
    // route the stage-0 registration exists to remove.
    // quasi-static debug data registered locally so the stage-B muxes
    // read nearby flops instead of routing from the top-level CDC regs
    reg [7:0] hex_vals_r [NUM_HEX_BYTES];
    reg [7:0] bits0_r, bits1_r;
    integer hv;
    always @(posedge clk_i) begin
        for (hv = 0; hv < NUM_HEX_BYTES; hv = hv + 1)
            hex_vals_r[hv] <= hex_values[hv];
        bits0_r <= debug_bits_0_i;
        bits1_r <= debug_bits_1_i;
    end

    (* syn_keep = 1, syn_preserve = 1 *) reg [11:0] s0_x_r;
    (* syn_keep = 1, syn_preserve = 1 *) reg [10:0] s0_y_r;
    reg [7:0]  s0_r_r, s0_g_r, s0_b_r;
    always @(posedge clk_i) begin
        s0_x_r <= screen_x_i;
        s0_y_r <= screen_y_i;
        s0_r_r <= r_i;
        s0_g_r <= g_i;
        s0_b_r <= b_i;
    end

    //=========================================================================
    // PIPELINE STAGE 1: Position calculation, region detection, ROM address
    //=========================================================================

    // --- Combinational: Stage 1 inputs ---
    wire [11:0] rel_x = 12'(s0_x_r - X_OFFSET);
    wire [10:0] rel_y = 11'(s0_y_r - Y_OFFSET);
    wire [3:0]  char_pos = rel_x[6:3];
    wire [2:0]  y_bit = rel_y[2:0];

    // Version string region
    localparam [10:0] VERSION_END = 11'(NUM_CHARS * CHAR_WIDTH);
    wire x_version_in_range = (s0_x_r >= X_OFFSET) && (rel_x < VERSION_END);

    // Debug region start
    localparam [10:0] DEBUG_START = 11'(VERSION_END + DEBUG_SPACE);
    localparam [10:0] HEX_WIDTH = 11'(2 * CHAR_WIDTH);
    localparam [10:0] HEX_REGION_WIDTH = 11'(HEX_WIDTH + DEBUG_SPACE);

    // Hex region starts/ends (compile-time constants)
    wire [10:0] hex_region_starts[NUM_HEX_BYTES];
    wire [10:0] hex_region_ends[NUM_HEX_BYTES];
    generate
        for (genvar i = 0; i < NUM_HEX_BYTES; i++) begin : hex_regions
            assign hex_region_starts[i] = 11'(DEBUG_START + i * HEX_REGION_WIDTH);
            assign hex_region_ends[i] = 11'(hex_region_starts[i] + HEX_WIDTH);
        end
    endgenerate

    // Bit field region starts/ends
    localparam [10:0] BITS_START = 11'(DEBUG_START + NUM_HEX_BYTES * HEX_REGION_WIDTH);
    localparam [10:0] BITS_FIELD_WIDTH = 11'(NUM_BITS_PER_FIELD * CHAR_WIDTH);
    localparam [10:0] BITS_REGION_WIDTH = 11'(BITS_FIELD_WIDTH + DEBUG_SPACE);

    wire [10:0] bits_region_starts[NUM_BITS_FIELDS];
    wire [10:0] bits_region_ends[NUM_BITS_FIELDS];
    generate
        for (genvar i = 0; i < NUM_BITS_FIELDS; i++) begin : bits_regions
            assign bits_region_starts[i] = 11'(BITS_START + i * BITS_REGION_WIDTH);
            assign bits_region_ends[i] = 11'(bits_region_starts[i] + BITS_FIELD_WIDTH);
        end
    endgenerate

    localparam [10:0] DEBUG_END = 11'(BITS_START + NUM_BITS_FIELDS * BITS_REGION_WIDTH);

    wire debug_region = (rel_x >= DEBUG_START);
    wire y_in_range = (s0_y_r >= Y_OFFSET) && (rel_y < CHAR_HEIGHT);

    // Region detection (combinational)
    reg signed [4:0] comb_hex_byte;
    reg comb_is_hex;
    reg [0:0] comb_bit_field;
    reg comb_is_bits;
    reg comb_in_space;
    reg [10:0] comb_rel_hex_pos;
    reg [10:0] comb_rel_bits_pos;

    always_comb begin
        comb_hex_byte = -1;
        comb_is_hex = 1'b0;
        comb_bit_field = 0;
        comb_is_bits = 1'b0;
        comb_in_space = 1'b0;
        if (debug_region) begin
            for (int i = 0; i < NUM_HEX_BYTES; i++) begin
                if (rel_x >= hex_region_starts[i] && rel_x < hex_region_ends[i]) begin
                    comb_hex_byte = 4'(i);
                    comb_is_hex = 1'b1;
                end
            end
            for (int i = 0; i < NUM_BITS_FIELDS; i++) begin
                if (rel_x >= bits_region_starts[i] && rel_x < bits_region_ends[i]) begin
                    comb_bit_field = i;
                    comb_is_bits = 1'b1;
                end
            end
            if (!(comb_is_hex || comb_is_bits) && (rel_x < DEBUG_END)) begin
                comb_in_space = 1'b1;
            end
        end
    end

    // Region-relative subtracts moved BEHIND the sA registers (compares
    // and subtract chained in one cycle missed 148.5 MHz): the region
    // index is registered, then a single constant-mux + subtract follows.
    always_comb begin
        comb_rel_hex_pos  = sA_is_hex  ? (sA_rel_x[10:0] - hex_region_starts[sA_hex_byte[2:0]])  : 11'd0;
        comb_rel_bits_pos = sA_is_bits ? (sA_rel_x[10:0] - bits_region_starts[sA_bit_field]) : 11'd0;
    end

    //=========================================================================
    // PIPELINE STAGE A: register the region-detection results. At 148.5 MHz
    // (1080p) the compare ladder above plus the address composition below is
    // ~9 ns in one cycle; the split makes each half comfortable. Costs one
    // more pixel of overlay shift — irrelevant for a debug surface.
    //=========================================================================
    reg               sA_is_hex, sA_is_bits, sA_in_space, sA_debug_region;
    reg signed [4:0]  sA_hex_byte;
    reg [0:0]         sA_bit_field;
    reg [11:0]        sA_rel_x;
    reg               sA_x_version_in_range, sA_y_in_range;
    reg [2:0]         sA_y_bit;
    reg [3:0]         sA_char_pos;

    reg [7:0]         sA_r, sA_g, sA_b;

    always @(posedge clk_i) begin
        sA_is_hex       <= comb_is_hex;
        sA_is_bits      <= comb_is_bits;
        sA_in_space     <= comb_in_space;
        sA_debug_region <= debug_region;
        sA_hex_byte     <= comb_hex_byte;
        sA_bit_field    <= comb_bit_field;
        sA_rel_x        <= rel_x;
        sA_x_version_in_range <= x_version_in_range;
        sA_y_in_range   <= y_in_range;
        sA_y_bit        <= y_bit;
        sA_char_pos     <= char_pos;
        sA_r <= s0_r_r;
        sA_g <= s0_g_r;
        sA_b <= s0_b_r;
    end

    // Pre-compute intermediate values to avoid local reg inside always_comb
    wire [2:0] bits_bp = 3'(comb_rel_bits_pos / 11'(CHAR_WIDTH));
    wire bits_bv = (sA_bit_field == 1'b0) ? bits0_r[7-bits_bp] : bits1_r[7-bits_bp];

    wire hex_nibble_sel = comb_rel_hex_pos >= 11'(CHAR_WIDTH);
    wire [7:0] hex_hval = sA_is_hex ? hex_vals_r[sA_hex_byte[2:0]] : 8'h00;
    wire [3:0] hex_nibble = hex_nibble_sel ? hex_hval[3:0] : hex_hval[7:4];

    wire [7:0] ver_ch = VERSION[(NUM_CHARS-1-sA_char_pos)*8 +: 8];

    // Compute ROM address and x_bit in stage 1
    reg [6:0] comb_rom_addr;
    reg [2:0] comb_x_bit;
    reg comb_in_bounds;
    reg comb_is_space;
    reg comb_is_solid;

    always_comb begin
        comb_rom_addr = 7'd0;
        comb_x_bit = 3'd0;
        comb_in_bounds = 1'b0;
        comb_is_space = 1'b0;
        comb_is_solid = 1'b0;

        if (ENABLE && enable_i && sA_y_in_range) begin
            if (sA_debug_region) begin
                if (sA_in_space) begin
                    comb_in_bounds = 1'b1;
                    comb_is_space = 1'b1;
                end else if (sA_is_bits && sA_rel_x < DEBUG_END) begin
                    comb_in_bounds = 1'b1;
                    comb_x_bit = comb_rel_bits_pos[2:0];
                    comb_rom_addr = {4'(bits_bv ? 4'd1 : 4'd0), sA_y_bit};
                end else if (sA_is_hex && sA_rel_x < DEBUG_END) begin
                    comb_in_bounds = 1'b1;
                    comb_x_bit = comb_rel_hex_pos[2:0];
                    comb_rom_addr = {hex_nibble, sA_y_bit};
                end
            end else begin
                if (sA_x_version_in_range) begin
                    comb_in_bounds = 1'b1;
                    comb_x_bit = sA_rel_x[2:0];
                    if (ver_ch >= 8'h30 && ver_ch <= 8'h39)
                        comb_rom_addr = {ver_ch[3:0], sA_y_bit};
                    else if (ver_ch >= 8'h41 && ver_ch <= 8'h46)
                        comb_rom_addr = {4'd10 + (ver_ch[3:0] - 4'd1), sA_y_bit};
                    else if (ver_ch >= 8'h61 && ver_ch <= 8'h66)
                        comb_rom_addr = {4'd10 + (ver_ch[3:0] - 4'd1), sA_y_bit};
                    else
                        comb_is_solid = 1'b1;
                end
            end
        end
    end

    // --- Stage 1 registers ---
    reg [6:0] s1_rom_addr;
    reg [2:0] s1_x_bit;
    reg       s1_in_bounds;
    reg       s1_is_space;
    reg       s1_is_solid;
    reg [7:0] s1_r, s1_g, s1_b;

    always @(posedge clk_i) begin
        s1_rom_addr  <= comb_rom_addr;
        s1_x_bit     <= comb_x_bit;
        s1_in_bounds <= comb_in_bounds;
        s1_is_space  <= comb_is_space;
        s1_is_solid  <= comb_is_solid;
        // Delay RGB passthrough by 1 cycle to align with pipeline
        s1_r <= sA_r;
        s1_g <= sA_g;
        s1_b <= sA_b;
    end

    //=========================================================================
    // PIPELINE STAGE 2: ROM lookup, pixel test, output mux
    //=========================================================================

    // Stage 2: register the (LUT-ROM) glyph row and controls; the 128-entry
    // async ROM mux plus the output select was the next failing path at
    // 148.5 MHz. One more pixel of overlay shift.
    reg [7:0] s2_font_row;
    reg       s2_in_bounds, s2_is_space, s2_is_solid;
    reg [2:0] s2_x_bit;
    reg [7:0] s2_r, s2_g, s2_b;
    always @(posedge clk_i) begin
        s2_font_row  <= char_rom[s1_rom_addr];
        s2_in_bounds <= s1_in_bounds;
        s2_is_space  <= s1_is_space;
        s2_is_solid  <= s1_is_solid;
        s2_x_bit     <= s1_x_bit;
        s2_r <= s1_r;
        s2_g <= s1_g;
        s2_b <= s1_b;
    end

    wire pixel_on = s2_in_bounds && !s2_is_space && (s2_is_solid || s2_font_row[s2_x_bit]);

    always @(posedge clk_i) begin
        r_o <= pixel_on ? 8'hFF : s2_r;
        g_o <= pixel_on ? 8'hFF : s2_g;
        b_o <= pixel_on ? 8'hFF : s2_b;
    end

endmodule
