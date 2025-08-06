module stream_serializer (
    input         clk,
    input         rst_n,
    input         wr_i,
    input  [31:0] data_i,
    output        qclk,
    output        frame,
    output [3:0]  qdata,
    output        busy      // New: indicates serializer is busy
);

    reg [31:0] pending_data_r;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            pending_data_r <= 32'h0;
        end else begin
            if (wr_i) begin
                pending_data_r <= data_i;
            end
        end
    end

    reg [1:0] clk_count_r = 2'b0;
    wire clk4 = clk_count_r[1]; // Use the second bit for 4x clock
    wire clk4_rising_w = (clk_count_r == 2'b10);
    wire ckl4_falling_w = (clk_count_r == 2'b00);
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            clk_count_r <= 2'b0;
        end else begin
            clk_count_r <= clk_count_r + 1;
        end
    end

    reg packet_active_r = 1'b0;
    reg packet_pending_r = 1'b0;
    reg [31:0] packet_data_r;
    assign qdata = packet_data_r[3:0];
    assign qclk = clk4 && packet_active_r;
    reg [2:0] nibble_count_r = 3'b0;
    assign frame = packet_active_r && (nibble_count_r == 3'b000);
    assign busy = packet_active_r | packet_pending_r;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            nibble_count_r <= 3'b0;
            packet_active_r <= 1'b0;
            packet_pending_r <= 1'b0;
            packet_data_r <= 32'h0;
        end else begin 
            if (wr_i) begin
                packet_pending_r <= 1'b1;
            end
            if (ckl4_falling_w) begin
                if (!packet_active_r && packet_pending_r) begin
                    packet_data_r <= pending_data_r;
                    packet_active_r <= 1'b1;
                    nibble_count_r <= 3'b000;
                    packet_pending_r <= 1'b0;
                end else if (packet_active_r) begin
                    packet_data_r <= {4'b0, packet_data_r[31:4]}; // Shift out the next nibble
                    nibble_count_r <= nibble_count_r + 1;
                    if (nibble_count_r == 3'b111) begin
                        packet_active_r <= 1'b0; // End of packet
                    end
                end
            end

        end
    end

endmodule
