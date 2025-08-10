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
    output        busy 
);

    // ----------------------------
    // Write queue (1-deep)
    // ----------------------------
    reg [31:0] pending_data_r;
    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            pending_data_r <= 32'h0;
        end else if (wr_i) begin
            pending_data_r <= data_i;    // last write wins
        end
    end

    // ----------------------------
    // Clock divider (free-runs)
    // ----------------------------
    reg [COUNT_WIDTH-1:0] clk_count_r = '0;
    wire cam_pclk_w          = clk_count_r[COUNT_WIDTH-1];                // MSB
    wire cam_pclk_rising_w   = (clk_count_r == (1 << (COUNT_WIDTH-1)));   // 0->1
    wire cam_pclk_falling_w  = (clk_count_r == '0);                       // 1->0

    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            clk_count_r <= '0;
        end else begin
            clk_count_r <= clk_count_r + 1;
        end
    end

    // ----------------------------
    // Packet engine
    // ----------------------------
    // Packet Protocol:
    // Clock active during transmission, otherwise halted
    // Packet consists of 10 4-bit nibbles
    // Send 32-bit data word, one nibble at a time on nibbles 0 to 7
    // Raise SYNC bit on nibble 8
    // Empty pad on nibble 9
    // ----------------------------
    reg         packet_pending_r;
    reg         packet_active_r;
    reg  [31:0] packet_data_r;
    reg  [3:0]  nibble_count_r;

    // Drive outputs
    assign cam_data = packet_data_r[3:0];
    assign cam_pclk = packet_active_r ? cam_pclk_w : 1'b0;

    // VSYNC at end-of-word (nibble 8) and only while active
    assign cam_sync = packet_active_r && (nibble_count_r == 4'd8);

    // busy = active OR queued
    assign busy = packet_active_r | packet_pending_r;

    // Serializer / flow
    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            nibble_count_r  <= 4'd0;
            packet_active_r <= 1'b0;
            packet_data_r   <= '0;
            packet_pending_r <= 1'b0;
        end else begin
            if (wr_i) begin
                packet_pending_r <= 1'b1;
            end
            if (cam_pclk_falling_w) begin
                // Start a new word exactly after finishing one, if something is pending
                if (packet_active_r && (nibble_count_r == 4'd9)) begin
                    if (packet_pending_r) begin
                        packet_data_r   <= pending_data_r;
                        packet_active_r <= 1'b1;      // stay active (back-to-back)
                        nibble_count_r  <= 4'd0;
                        packet_pending_r <= 1'b0;
                    end else begin
                        packet_data_r   <= '0;        // idle pattern
                        packet_active_r <= 1'b0;      // gate off PCLK
                        nibble_count_r  <= 4'd0;
                    end
                end
                // If idle and we have a packet queued, launch it
                else if (!packet_active_r && packet_pending_r) begin
                    packet_data_r   <= pending_data_r;
                    packet_active_r <= 1'b1;
                    nibble_count_r  <= 4'd0;
                    packet_pending_r <= 1'b0;
                end

                // SHIFT & COUNT only when active, on the PCLK falling edge
                if (packet_active_r) begin
                    packet_data_r  <= {4'b0, packet_data_r[31:4]};  // LSN-first
                    nibble_count_r <= nibble_count_r + 4'd1;
                end
            end
        end
    end

endmodule