// SPI serializer - sends 32-bit data as serial SPI 
// Compatible with ESP32 SPI slave
// (converts from 4-bit parallel to 1-bit serial)

module qspi_to_spi_serializer #(
    parameter int COUNT_WIDTH = 8
) (
    input         clk_i,
    input         rst_n,
    input         wr_i,
    input  [31:0] data_i,
    output        spi_clk,
    output        spi_cs,
    output        spi_mosi
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
    wire spi_clk_w = clk_count_r[COUNT_WIDTH-1]; // Use the MSB for clock
    wire spi_clk_rising_w = (clk_count_r == (1 << (COUNT_WIDTH-1))); // MSB transitions 0->1
    wire spi_clk_falling_w = (clk_count_r == '0); // Counter wraps to 0
    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            clk_count_r <= '0;
        end else begin
            clk_count_r <= clk_count_r + 1;
        end
    end
    assign spi_clk = spi_clk_w;

    reg packet_active_r = 1'b0;
    reg packet_pending_r = 1'b0;
    reg [31:0] packet_data_r;
    assign spi_mosi = packet_data_r[31]; // Send MSB first
    assign spi_cs = ~packet_active_r;    // CS active low
    reg [4:0] bit_count_r = 5'b0;       // 0-31 bits (5 bits for counting)

    always @(posedge clk_i or negedge rst_n) begin
        if (!rst_n) begin
            bit_count_r <= 5'b0;
            packet_active_r <= 1'b0;
            packet_pending_r <= 1'b0;
            packet_data_r <= 32'h0;
        end else begin 
            if (wr_i) begin
                packet_pending_r <= 1'b1;
            end
            if (spi_clk_falling_w) begin
                if (!packet_active_r && packet_pending_r) begin
                    packet_data_r <= pending_data_r;
                    packet_active_r <= 1'b1;
                    bit_count_r <= 5'b00000;
                    packet_pending_r <= 1'b0;
                end else if (packet_active_r) begin
                    packet_data_r <= {packet_data_r[30:0], 1'b0}; // Shift left, MSB first
                    bit_count_r <= bit_count_r + 1;
                    if (bit_count_r == 5'b11111) begin // After 32 bits (0-31)
                        packet_active_r <= 1'b0; // End of packet
                    end
                end
            end
        end
    end

endmodule