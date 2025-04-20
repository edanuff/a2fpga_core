module DebugOverlay #(
    parameter [8*14-1:0] VERSION = "00000000000000", // 14 ASCII characters
    parameter bit ENABLE = 1'b1
)(
    input  wire        clk_i,
    input  wire        reset_n,

    input  wire [9:0]  screen_x_i,
    input  wire [9:0]  screen_y_i,

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
    localparam NUM_CHARS   = 14;
    localparam X_OFFSET    = 16;
    localparam Y_OFFSET    = 24;
    
    // Character ROM: 10 digits × 8 rows
    reg [7:0] char_rom [80] = '{
        8'b00111100,
        8'b01000010,
        8'b01000110,
        8'b01001010,
        8'b01010010,
        8'b01100010,
        8'b01000010,
        8'b00111100,
        8'b00010000,
        8'b00110000,
        8'b00010000,
        8'b00010000,
        8'b00010000,
        8'b00010000,
        8'b00010000,
        8'b00111000,
        8'b00111100,
        8'b01000010,
        8'b00000010,
        8'b00001100,
        8'b00010000,
        8'b00100000,
        8'b01000000,
        8'b01111110,
        8'b00111100,
        8'b01000010,
        8'b00000010,
        8'b00011100,
        8'b00000010,
        8'b00000010,
        8'b01000010,
        8'b00111100,
        8'b00001000,
        8'b00011000,
        8'b00101000,
        8'b01001000,
        8'b10001000,
        8'b11111110,
        8'b00001000,
        8'b00001000,
        8'b01111110,
        8'b01000000,
        8'b01000000,
        8'b01111100,
        8'b00000010,
        8'b00000010,
        8'b01000010,
        8'b00111100,
        8'b00011100,
        8'b00100000,
        8'b01000000,
        8'b01111100,
        8'b01000010,
        8'b01000010,
        8'b01000010,
        8'b00111100,
        8'b01111110,
        8'b00000010,
        8'b00000100,
        8'b00001000,
        8'b00010000,
        8'b00010000,
        8'b00010000,
        8'b00010000,
        8'b00111100,
        8'b01000010,
        8'b01000010,
        8'b00111100,
        8'b01000010,
        8'b01000010,
        8'b01000010,
        8'b00111100,
        8'b00111100,
        8'b01000010,
        8'b01000010,
        8'b01000010,
        8'b00111110,
        8'b00000010,
        8'b00000100,
        8'b00111000
    };

    // Position calculations
    wire [9:0] rel_x = 10'(screen_x_i - X_OFFSET);
    wire [9:0] rel_y = 10'(screen_y_i - Y_OFFSET);
    
    // Fixed-width bit slicing to avoid division/modulo
    wire [3:0] char_pos = rel_x[6:3]; // Which character (0-13)
    wire [2:0] x_bit = rel_x[2:0];    // Which bit within character (0-7)
    wire [2:0] y_bit = rel_y[2:0];    // Which row within character (0-7)
    
    // For boundary checking
    wire x_in_range = (screen_x_i >= X_OFFSET) ? rel_x < (NUM_CHARS * CHAR_WIDTH) : 1'b0;
    wire y_in_range = (screen_y_i >= Y_OFFSET) ? rel_y < CHAR_HEIGHT : 1'b0;
    wire in_bounds = x_in_range && y_in_range;

    // Character extraction
    wire [7:0] char_data;
    
    // Extract character for current position (big-endian string format)
    assign char_data = VERSION[(NUM_CHARS-1-char_pos)*8 +: 8];

    // Decode logic
    logic [7:0] font_row;
    logic pixel_on;
    
    always_comb begin
        // Default values
        font_row = 8'd0;
        pixel_on = 1'b0;
        
        if (ENABLE && in_bounds && (char_pos < 4'd14)) begin
            // Only handle digits 0-9
            if (char_data >= 8'h30 && char_data <= 8'h39) begin
                // Convert ASCII to digit value (0-9)                
                // Get the font row for this digit
                font_row = char_rom[{char_data[3:0], y_bit}];
                
                // Get the specific pixel from the font row
                // MSB = leftmost pixel
                pixel_on = font_row[7-x_bit];
            end else begin
                // Non-digits show as solid blocks for debugging
                pixel_on = 1'b1;
            end
        end
    end
    
    // Output coloring
    assign r_o = pixel_on ? 8'hFF : r_i;
    assign g_o = pixel_on ? 8'hFF : g_i;
    assign b_o = pixel_on ? 8'hFF : b_i;

endmodule