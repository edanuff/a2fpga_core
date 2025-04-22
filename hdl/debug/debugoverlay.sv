module DebugOverlay #(
    parameter [8*14-1:0] VERSION = "00000000000000", // 14 ASCII characters
    parameter bit ENABLE = 1'b1
)(
    input  wire        clk_i,
    input  wire        reset_n,

    input  wire [9:0]  screen_x_i,
    input  wire [9:0]  screen_y_i,

    input  wire [7:0]  debug_hex_0_i,
    input  wire [7:0]  debug_hex_1_i,
    input  wire [7:0]  debug_hex_2_i,
    input  wire [7:0]  debug_hex_3_i,
    
    input  wire [7:0]  debug_bits_0_i,
    input  wire [7:0]  debug_bits_1_i,

    input  wire [7:0]  r_i,
    input  wire [7:0]  g_i,
    input  wire [7:0]  b_i,

    output wire [7:0]  r_o,
    output wire [7:0]  g_o,
    output wire [7:0]  b_o
);

    // Constants
    localparam CHAR_WIDTH  = 8;
    localparam CHAR_HEIGHT = 8;
    localparam NUM_CHARS   = 14;          // Number of characters in VERSION string
    localparam NUM_DEBUG_CHARS = 8;       // 4 debug values x 2 hex digits each
    localparam X_OFFSET    = 16;
    localparam Y_OFFSET    = 24;
    localparam DEBUG_SPACE = 8;           // Space between debug values
    
    // Character ROM: 16 hex digits (0-9, A-F) × 8 rows
    // Digits 0-9 extracted from video.hex starting at byte 1408 (character code 176*8)
    // Letters A-F extracted from video.hex starting at byte 1032 (character code 129*8)
    reg [7:0] char_rom [128] = '{
        // Digit 0 (Line 177, first 8 bytes)
        8'b00011100, // 1C - Original hex: 1C 22 32 2A 26 22 1C 00
        8'b00100010,
        8'b00110010,
        8'b00101010,
        8'b00100110,
        8'b00100010,
        8'b00011100,
        8'b00000000,
        
        // Digit 1 (Line 177, next 8 bytes)
        8'b00001000, // 08 - Original hex: 08 0C 08 08 08 08 1C 00
        8'b00001100,
        8'b00001000,
        8'b00001000,
        8'b00001000,
        8'b00001000,
        8'b00011100,
        8'b00000000,
        
        // Digit 2 (Line 178, first 8 bytes)
        8'b00111100, // 1C - Original hex: 1C 22 20 18 04 02 3E 00
        8'b00100010,
        8'b00100000,
        8'b00011000,
        8'b00000100,
        8'b00000010,
        8'b00111110,
        8'b00000000,
        
        // Digit 3 (Line 178, next 8 bytes)
        8'b00111110, // 3E - Original hex: 3E 20 10 18 20 22 1C 00
        8'b00100000,
        8'b00010000,
        8'b00011000,
        8'b00100000,
        8'b00100010,
        8'b00011100,
        8'b00000000,
        
        // Digit 4 (Line 179, first 8 bytes)
        8'b00010000, // 10 - Original hex: 10 18 14 12 3E 10 10 00
        8'b00011000,
        8'b00010100,
        8'b00010010,
        8'b00111110,
        8'b00010000,
        8'b00010000,
        8'b00000000,
        
        // Digit 5 (Line 179, next 8 bytes)
        8'b00111110, // 3E - Original hex: 3E 02 1E 20 20 22 1C 00
        8'b00000010,
        8'b00011110,
        8'b00100000,
        8'b00100000,
        8'b00100010,
        8'b00011100,
        8'b00000000,
        
        // Digit 6 (Line 180, first 8 bytes)
        8'b00111000, // 38 - Original hex: 38 04 02 1E 22 22 1C 00
        8'b00000100,
        8'b00000010,
        8'b00011110,
        8'b00100010,
        8'b00100010,
        8'b00011100,
        8'b00000000,
        
        // Digit 7 (Line 180, next 8 bytes)
        8'b00111110, // 3E - Original hex: 3E 20 10 08 04 04 04 00
        8'b00100000,
        8'b00010000,
        8'b00001000,
        8'b00000100,
        8'b00000100,
        8'b00000100,
        8'b00000000,
        
        // Digit 8 (Line 181, first 8 bytes)
        8'b00011100, // 1C - Original hex: 1C 22 22 1C 22 22 1C 00
        8'b00100010,
        8'b00100010,
        8'b00011100,
        8'b00100010,
        8'b00100010,
        8'b00011100,
        8'b00000000,
        
        // Digit 9 (Line 181, next 8 bytes)
        8'b00011100, // 1C - Original hex: 1C 22 22 3C 20 10 0E 00
        8'b00100010,
        8'b00100010,
        8'b00111100,
        8'b00100000,
        8'b00010000,
        8'b00001110,
        8'b00000000,
        
        // Letter A (Line 1, second 8 bytes)
        8'b00001000, // 08 - Original hex from video.hex: 08 14 22 22 3E 22 22 00
        8'b00010100, // 14
        8'b00100010, // 22
        8'b00100010, // 22
        8'b00111110, // 3E
        8'b00100010, // 22
        8'b00100010, // 22
        8'b00000000, // 00
        
        // Letter B (Line 1, next 8 bytes)
        8'b00011110, // 1E - Original hex: 1E 22 22 1E 22 22 1E 00
        8'b00100010,
        8'b00100010,
        8'b00111110,
        8'b00100010,
        8'b00100010,
        8'b00011110,
        8'b00000000,
        
        // Letter C (Line 2, first 8 bytes)
        8'b00011100, // 1C - Original hex: 1C 22 02 02 02 22 1C 00
        8'b00100010,
        8'b00000010,
        8'b00000010,
        8'b00000010,
        8'b00100010,
        8'b00011100,
        8'b00000000,
        
        // Letter D (Line 2, next 8 bytes)
        8'b00011110, // 1E - Original hex: 1E 22 22 22 22 22 1E 00
        8'b00100010,
        8'b00100010,
        8'b00100010,
        8'b00100010,
        8'b00100010,
        8'b00011110,
        8'b00000000,
        
        // Letter E (Line 3, first 8 bytes)
        8'b00111110, // 3E - Original hex: 3E 02 02 1E 02 02 3E 00
        8'b00000010,
        8'b00000010,
        8'b00111110,
        8'b00000010,
        8'b00000010,
        8'b00111110,
        8'b00000000,
        
        // Letter F (Line 3, next 8 bytes)
        8'b00111110, // 3E - Original hex: 3E 02 02 1E 02 02 02 00
        8'b00000010,
        8'b00000010,
        8'b00111110,
        8'b00000010,
        8'b00000010,
        8'b00000010,
        8'b00000000
    };

    // Position calculations
    wire [9:0] rel_x = 10'(screen_x_i - X_OFFSET);
    wire [9:0] rel_y = 10'(screen_y_i - Y_OFFSET);
    
    // Fixed-width bit slicing to avoid division/modulo
    wire [3:0] char_pos = rel_x[6:3]; // Which character (0-13) in version string
    
    // Bit position within character
    reg [2:0] x_bit;
    wire [2:0] y_bit = rel_y[2:0];    // Which row within character (0-7)
    
    // Calculate x_bit position based on region
    always_comb begin
        // Default value to avoid latch inference
        x_bit = 3'd0;
        
        if (debug_region) begin
            if (in_bits_region) begin
                // For bit display regions, calculate based on which bit region
                if (bit_region_index == 1'b0) begin
                    // First bit region (debug_bits_0_i)
                    x_bit = 3'(rel_bits_0 % 10'd8); // Bit position within character (CHAR_WIDTH = 8)
                end
                else begin
                    // Second bit region (debug_bits_1_i)
                    x_bit = 3'(rel_bits_1 % 10'd8); // Bit position within character (CHAR_WIDTH = 8)
                end
            end
            else begin
                // For debug hex value regions
                if (debug_value_index == 2'd0) begin
                    // First debug value (high or low nibble)
                    x_bit = debug_nibble ? 3'(rel_debug_0 - CHAR_WIDTH) : rel_debug_0[2:0];
                end
                else if (debug_value_index == 2'd1) begin
                    // Second debug value (high or low nibble)
                    x_bit = debug_nibble ? 3'(rel_debug_1 - CHAR_WIDTH) : rel_debug_1[2:0];
                end
                else if (debug_value_index == 2'd2) begin
                    // Third debug value (high or low nibble)
                    x_bit = debug_nibble ? 3'(rel_debug_2 - CHAR_WIDTH) : rel_debug_2[2:0];
                end
                else if (debug_value_index == 2'd3) begin
                    // Fourth debug value (high or low nibble)
                    x_bit = debug_nibble ? 3'(rel_debug_3 - CHAR_WIDTH) : rel_debug_3[2:0];
                end
            end
            // All cases covered, no latch needed
        end
        else begin
            // For version string, just use the low 3 bits of rel_x
            x_bit = rel_x[2:0];
        end
    end
    
    // For boundary checking
    // Version string region
    wire x_version_in_range = (screen_x_i >= X_OFFSET) ? rel_x < (NUM_CHARS * CHAR_WIDTH) : 1'b0;
    
    // Debug values region (all visible elements)
    wire [9:0] x_debug_end = bits_1_end; // Use the end of all display regions
    wire x_debug_in_range = (rel_x >= x_debug_start) && (rel_x < x_debug_end);
    
    // Combined x-range check
    wire x_in_range = (screen_x_i >= X_OFFSET) && (x_version_in_range || x_debug_in_range);
    
    // Y-range is the same for both regions
    wire y_in_range = (screen_y_i >= Y_OFFSET) ? rel_y < CHAR_HEIGHT : 1'b0;
    wire in_bounds = x_in_range && y_in_range;

    // Character extraction
    wire [7:0] char_data;
    
    // Define the start of the debug region
    wire [9:0] x_debug_start = 10'(NUM_CHARS * CHAR_WIDTH + DEBUG_SPACE);
    
    // Extract character for current position (big-endian string format)
    // Check if we're in the debug hex value region to the right of the version string
    wire debug_region = (rel_x >= x_debug_start);
    
    // Each debug value display region
    // For debug_hex_0_i: 2 chars starting at x_debug_start
    // For debug_hex_1_i: 2 chars starting at x_debug_start + 2*CHAR_WIDTH + DEBUG_SPACE
    // For debug_hex_2_i: 2 chars starting at x_debug_start + 4*CHAR_WIDTH + 2*DEBUG_SPACE
    // For debug_hex_3_i: 2 chars starting at x_debug_start + 6*CHAR_WIDTH + 3*DEBUG_SPACE
    // For debug_bits_0_i: 8 chars starting after debug_hex with DEBUG_SPACE
    // For debug_bits_1_i: 8 chars starting after debug_bits_0_i with DEBUG_SPACE
    
    // Define the boundaries for each debug region
    wire [9:0] debug_0_start = x_debug_start;
    wire [9:0] debug_0_end = debug_0_start + 10'd16; // 2*CHAR_WIDTH = 16
    
    wire [9:0] debug_1_start = debug_0_end + 10'd8;  // DEBUG_SPACE = 8
    wire [9:0] debug_1_end = debug_1_start + 10'd16; // 2*CHAR_WIDTH = 16
    
    wire [9:0] debug_2_start = debug_1_end + 10'd8;  // DEBUG_SPACE = 8
    wire [9:0] debug_2_end = debug_2_start + 10'd16; // 2*CHAR_WIDTH = 16
    
    wire [9:0] debug_3_start = debug_2_end + 10'd8;  // DEBUG_SPACE = 8
    wire [9:0] debug_3_end = debug_3_start + 10'd16; // 2*CHAR_WIDTH = 16
    
    // Define bit display regions (8 characters each, one per bit)
    wire [9:0] bits_0_start = debug_3_end + 10'd8;   // DEBUG_SPACE = 8
    wire [9:0] bits_0_end = bits_0_start + 10'd64;   // 8*CHAR_WIDTH = 64
    
    wire [9:0] bits_1_start = bits_0_end + 10'd8;    // DEBUG_SPACE = 8
    wire [9:0] bits_1_end = bits_1_start + 10'd64;   // 8*CHAR_WIDTH = 64
    
    // Calculate positions for debug hex characters and bits
    reg [3:0] debug_char_pos;
    reg [1:0] debug_value_index;
    reg debug_nibble;
    
    // For bit display
    reg in_bits_region;
    reg [2:0] bit_index;      // 0-7 for bits_0 or bits_1 (which bit we're rendering)
    reg bit_value;            // 0 or 1 for the current bit value
    reg bit_region_index;     // 0 for bits_0, 1 for bits_1
    
    // Calculate the relative position for each region
    wire [9:0] rel_debug_0 = rel_x - debug_0_start;
    wire [9:0] rel_debug_1 = rel_x - debug_1_start;
    wire [9:0] rel_debug_2 = rel_x - debug_2_start;
    wire [9:0] rel_debug_3 = rel_x - debug_3_start;
    wire [9:0] rel_bits_0 = rel_x - bits_0_start;
    wire [9:0] rel_bits_1 = rel_x - bits_1_start;
    
    // Determine if we're in a space between debug values
    wire in_space_0 = (rel_x >= debug_0_end && rel_x < debug_1_start);
    wire in_space_1 = (rel_x >= debug_1_end && rel_x < debug_2_start);
    wire in_space_2 = (rel_x >= debug_2_end && rel_x < debug_3_start);
    wire in_space_3 = (rel_x >= debug_3_end && rel_x < bits_0_start);
    wire in_space_4 = (rel_x >= bits_0_end && rel_x < bits_1_start);
    wire in_any_space = in_space_0 || in_space_1 || in_space_2 || in_space_3 || in_space_4;
    
    always_comb begin
        // Default values
        debug_char_pos = 4'd0;
        debug_value_index = 2'd0;
        debug_nibble = 1'b0;
        in_bits_region = 1'b0;
        bit_index = 3'd0;
        bit_value = 1'b0;
        bit_region_index = 1'b0;
        
        // Calculate relative position within debug region
        if (debug_region && !in_any_space) begin
            // First set of hex digits (debug_hex_0_i)
            if (rel_x >= debug_0_start && rel_x < debug_0_end) begin
                debug_value_index = 2'd0;
                debug_nibble = (rel_debug_0 >= CHAR_WIDTH);
                debug_char_pos = {2'd0, debug_nibble};
            end
            // Second set of hex digits (debug_hex_1_i)
            else if (rel_x >= debug_1_start && rel_x < debug_1_end) begin
                debug_value_index = 2'd1;
                debug_nibble = (rel_debug_1 >= CHAR_WIDTH);
                debug_char_pos = {2'd1, debug_nibble};
            end
            // Third set of hex digits (debug_hex_2_i)
            else if (rel_x >= debug_2_start && rel_x < debug_2_end) begin
                debug_value_index = 2'd2;
                debug_nibble = (rel_debug_2 >= CHAR_WIDTH);
                debug_char_pos = {2'd2, debug_nibble};
            end
            // Fourth set of hex digits (debug_hex_3_i)
            else if (rel_x >= debug_3_start && rel_x < debug_3_end) begin
                debug_value_index = 2'd3;
                debug_nibble = (rel_debug_3 >= CHAR_WIDTH);
                debug_char_pos = {2'd3, debug_nibble};
            end
            // First set of bits (debug_bits_0_i)
            else if (rel_x >= bits_0_start && rel_x < bits_0_end) begin
                in_bits_region = 1'b1;
                bit_region_index = 1'b0;
                // Determine which bit position (0-7)
                bit_index = 3'(rel_bits_0 / 10'd8); // CHAR_WIDTH = 8
                // Get the actual bit value from debug_bits_0
                bit_value = debug_bits_0_i[7-bit_index]; // MSB first
            end
            // Second set of bits (debug_bits_1_i)
            else if (rel_x >= bits_1_start && rel_x < bits_1_end) begin
                in_bits_region = 1'b1;
                bit_region_index = 1'b1;
                // Determine which bit position (0-7)
                bit_index = 3'(rel_bits_1 / 10'd8); // CHAR_WIDTH = 8
                // Get the actual bit value from debug_bits_1
                bit_value = debug_bits_1_i[7-bit_index]; // MSB first
            end
        end
    end
    
    // Get the appropriate debug value based on index
    wire [7:0] debug_value = (debug_value_index == 2'd0) ? debug_hex_0_i :
                            (debug_value_index == 2'd1) ? debug_hex_1_i :
                            (debug_value_index == 2'd2) ? debug_hex_2_i :
                                                         debug_hex_3_i;
                                                         
    // Get the correct nibble from the debug value
    wire [3:0] debug_nibble_value = debug_nibble ? debug_value[3:0] : debug_value[7:4];
    
    // Convert nibble to ASCII character code
    wire [7:0] debug_char = (debug_nibble_value < 4'hA) ? 
                           {4'h3, debug_nibble_value} :     // 0-9 -> ASCII '0'-'9'
                           {4'h4, 4'(debug_nibble_value - 4'h9)}; // A-F -> ASCII 'A'-'F'
                             
    // Select between VERSION string, debug hex values, and bit values
    // If we're in a spacing area between values, display a space
    // If we're in bit region, display '0' or '1' based on bit value
    assign char_data = (debug_region && !in_any_space && !in_bits_region) ? debug_char : 
                       (debug_region && in_any_space) ? 8'h20 :  // Space character
                       (debug_region && in_bits_region) ? (bit_value ? 8'h31 : 8'h30) : // '1' or '0'
                       VERSION[(NUM_CHARS-1-char_pos)*8 +: 8];
    
    // Decode logic
    logic [7:0] font_row;
    logic pixel_on;
    
    always_comb begin
        // Default values
        font_row = 8'd0;
        pixel_on = 1'b0;
        
        if (ENABLE && in_bounds) begin
            // Handle spaces specially - should be blank
            if (debug_region && in_any_space) begin
                pixel_on = 1'b0;
            end
            // Handle both version string region and debug region
            else if ((!debug_region && (char_pos < 4'd14)) || debug_region) begin
            // Handle both digits 0-9 and hex letters A-F
            if (char_data >= 8'h30 && char_data <= 8'h39) begin
                // Convert ASCII to digit value (0-9)                
                // Get the font row for this digit
                font_row = char_rom[{char_data[3:0], y_bit}];
                
                // Get the specific pixel from the font row
                // MSB = leftmost pixel
                pixel_on = font_row[x_bit];
            end else if (char_data >= 8'h41 && char_data <= 8'h46) begin
                // Handle uppercase hex digits A-F
                // Convert ASCII to array index (A=10, B=11, etc.)
                // A is 0x41, so 0x41-0x41+10 = 10
                font_row = char_rom[{4'd10 + (char_data[3:0] - 4'd1), y_bit}];
                
                // Get the specific pixel from the font row
                // MSB = leftmost pixel
                pixel_on = font_row[x_bit];
            end else if (char_data >= 8'h61 && char_data <= 8'h66) begin
                // Handle lowercase hex digits a-f (display as uppercase)
                // Convert ASCII to array index (a=10, b=11, etc.)
                // a is 0x61, so 0x61-0x61+10 = 10
                font_row = char_rom[{4'd10 + (char_data[3:0] - 4'd1), y_bit}];
                
                // Get the specific pixel from the font row
                // MSB = leftmost pixel
                pixel_on = font_row[x_bit];
            end else begin
                // Non-hex characters show as solid blocks for debugging
                pixel_on = 1'b1;
            end
            end // end of version/debug region condition
        end
    end
    
    // Output coloring
    assign r_o = pixel_on ? 8'hFF : r_i;
    assign g_o = pixel_on ? 8'hFF : g_i;
    assign b_o = pixel_on ? 8'hFF : b_i;

endmodule