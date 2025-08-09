module cam_serializer #(
    parameter COUNT_WIDTH = 8
) (
    input         clk_i,
    input         rst_n,
    input         wr_i,
    input  [31:0] data_i,
    output        cam_pclk,
    output        cam_sync,
    output [3:0]  cam_data,
    output        busy      // New: indicates serializer is busy
);

    reg [31:0] pending_data_r;
    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            pending_data_r <= 32'h0;
        end else begin
            if (wr_i) begin
                pending_data_r <= data_i;
            end
        end
    end

    reg [COUNT_WIDTH-1:0] clk_count_r = '0;
    wire cam_pclk_w = clk_count_r[COUNT_WIDTH-1]; // Use the MSB for clock
    wire cam_pclk_rising_w = (clk_count_r == (1 << (COUNT_WIDTH-1))); // MSB transitions 0->1
    wire cam_pclk_falling_w = (clk_count_r == '0); // Counter wraps to 0
    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            clk_count_r <= '0;
        end else begin
            clk_count_r <= clk_count_r + 1;  // Fixed: was 'qclk_count_r'
        end
    end
    assign cam_pclk = cam_pclk_w;

    reg packet_pending_r = 1'b0;
    reg [31:0] packet_data_r;
    assign cam_data = packet_data_r[3:0];
    reg [2:0] nibble_count_r = 3'b0;
    
    // cam_sync is VSYNC signal - pulses HIGH at start of each 8-nibble frame
    assign cam_sync = (nibble_count_r == 3'b000);  // HIGH for first nibble of each frame
    assign busy = packet_pending_r;

    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            nibble_count_r <= 3'b0;
            packet_pending_r <= 1'b0;
            packet_data_r <= 32'h12345678; // Initialize with test pattern
        end else begin 
            if (wr_i) begin
                packet_pending_r <= 1'b1;
            end
            if (cam_pclk_falling_w) begin
                // Continuous 8-nibble transmission
                nibble_count_r <= nibble_count_r + 1;
                packet_data_r <= {4'b0, packet_data_r[31:4]}; // Shift out the next nibble
                
                // Load new data when we complete 8 nibbles and have pending data
                if (nibble_count_r == 3'b111) begin
                    if (packet_pending_r) begin
                        packet_data_r <= pending_data_r;
                        packet_pending_r <= 1'b0;
                    end else begin
                        // If no pending data, load default pattern to avoid all-zeros
                        packet_data_r <= 32'hDEADBEEF; // Default test pattern
                    end
                end
            end
        end
    end

endmodule
