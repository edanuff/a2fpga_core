module qspi_serializer (
    input         clk_i,
    input         rst_n,
    input         wr_i,
    input  [31:0] data_i,
    output        qspi_clk,
    output        qspi_cs,
    output [3:0]  qspi_data
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

    reg [1:0] clk_count_r = 2'b0;
    wire qspi_clk_w = clk_count_r[1]; // Use the second bit for 4x clock
    wire qspi_clk_rising_w = (clk_count_r == 2'b10);
    wire qspi_clk_falling_w = (clk_count_r == 2'b00);
    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            clk_count_r <= 2'b0;
        end else begin
            qclk_count_r <= clk_count_r + 1;
        end
    end
    assign qspi_clk = qspi_clk_w;

    reg packet_active_r = 1'b0;
    reg packet_pending_r = 1'b0;
    reg [31:0] packet_data_r;
    assign qspi_data = packet_data_r[3:0];
    assign qspi_cs = packet_active_r;
    reg [2:0] nibble_count_r = 3'b0;

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
            if (qspi_clk_falling_w) begin
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
