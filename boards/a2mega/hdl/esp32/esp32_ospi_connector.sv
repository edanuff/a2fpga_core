// Octal SPI Connector for ESP32-S3 to FPGA communication
// Based on esp32_spi_connector.sv but for 8-bit parallel (Octal SPI) interface
//
// This module provides:
// - Bidirectional 8-bit data bus interface
// - 16 x 8-bit register file
// - 256-byte memory (SPACE 0) with 1-cycle read latency
// - Protocol processing via esp32_ospi_proto_proc
//
module esp32_ospi_connector #(
    parameter USE_SYNC    = 1,
    parameter USE_CRC     = 0,
    parameter IDLE_TO_CYC = 5_400_000
)(
    input  wire        clk,
    input  wire        rst_n,

    // Octal SPI physical interface
    input  wire        sclk,
    input  wire [7:0]  data_i,       // Input data from pads
    output wire [7:0]  data_o,       // Output data to pads
    output wire        data_oe       // Output enable (directly usable for tristate)
);

    // =========================================================================
    // Register File (16 x 8-bit)
    // =========================================================================
    reg [7:0] reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7;
    reg [7:0] reg8, reg9, reg10, reg11, reg12, reg13, reg14, reg15;

    // Device identification
    localparam [7:0] DEVICE_ID0 = "A";
    localparam [7:0] DEVICE_ID1 = "2";
    localparam [7:0] DEVICE_ID2 = "F";
    localparam [7:0] DEVICE_ID3 = "P";
    localparam [7:0] PROTO_VER  = 8'h01;

    // Capability register: [0]=SYNC, [1]=CRC support
    wire [7:0] CAP0 = {6'b0, USE_CRC[0], 1'b1};

    // =========================================================================
    // Internal Memory (256 bytes at SPACE 0)
    // =========================================================================
    reg [7:0] mem [0:255];
    reg [7:0] mem_rd_data_q;
    reg       mem_rd_valid_q;

    // =========================================================================
    // Protocol Processor Interface Wires
    // =========================================================================
    wire        reg_wr_req;
    wire [6:0]  reg_idx;
    wire [7:0]  reg_wdata;
    reg  [7:0]  reg_rdata;

    wire        mem_wr_en;
    wire [2:0]  mem_space;
    wire [23:0] mem_wr_addr;
    wire [7:0]  mem_wr_data;

    wire        mem_rd_req;
    wire [2:0]  mem_rd_space;
    wire [23:0] mem_rd_addr;
    wire        mem_rd_valid;
    wire [7:0]  mem_rd_data;

    // =========================================================================
    // Register Read Multiplexer
    // =========================================================================
    always @* begin
        case (reg_idx[3:0])
            4'h0: reg_rdata = reg0;
            4'h1: reg_rdata = reg1;
            4'h2: reg_rdata = reg2;
            4'h3: reg_rdata = reg3;
            4'h4: reg_rdata = reg4;
            4'h5: reg_rdata = reg5;
            4'h6: reg_rdata = reg6;
            4'h7: reg_rdata = reg7;
            4'h8: reg_rdata = reg8;
            4'h9: reg_rdata = reg9;
            4'hA: reg_rdata = reg10;
            4'hB: reg_rdata = reg11;
            4'hC: reg_rdata = reg12;
            4'hD: reg_rdata = reg13;
            4'hE: reg_rdata = reg14;
            default: reg_rdata = reg15;
        endcase
    end

    // =========================================================================
    // Register Write and Reset
    // =========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // Initialize with device ID and protocol info
            reg0  <= DEVICE_ID0;
            reg1  <= DEVICE_ID1;
            reg2  <= DEVICE_ID2;
            reg3  <= DEVICE_ID3;
            reg4  <= PROTO_VER;
            reg5  <= CAP0;
            reg6  <= 8'h00;
            reg7  <= 8'h00;
            reg8  <= 8'h00;
            reg9  <= 8'h00;
            reg10 <= 8'h00;
            reg11 <= 8'h00;
            reg12 <= 8'h00;
            reg13 <= 8'h00;
            reg14 <= 8'h00;
            reg15 <= 8'h00;
        end else if (reg_wr_req) begin
            case (reg_idx[3:0])
                4'h0: reg0  <= reg_wdata;
                4'h1: reg1  <= reg_wdata;
                4'h2: reg2  <= reg_wdata;
                4'h3: reg3  <= reg_wdata;
                4'h4: reg4  <= reg_wdata;
                4'h5: reg5  <= reg_wdata;
                4'h6: reg6  <= reg_wdata;
                4'h7: reg7  <= reg_wdata;
                4'h8: reg8  <= reg_wdata;
                4'h9: reg9  <= reg_wdata;
                4'hA: reg10 <= reg_wdata;
                4'hB: reg11 <= reg_wdata;
                4'hC: reg12 <= reg_wdata;
                4'hD: reg13 <= reg_wdata;
                4'hE: reg14 <= reg_wdata;
                default: reg15 <= reg_wdata;
            endcase
        end
    end

    // =========================================================================
    // Memory Write (SPACE 0 only)
    // =========================================================================
    always @(posedge clk) begin
        if (mem_wr_en && (mem_space == 3'd0)) begin
            mem[mem_wr_addr[7:0]] <= mem_wr_data;
        end
    end

    // =========================================================================
    // Memory Read (1-cycle latency)
    // =========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            mem_rd_valid_q <= 0;
            mem_rd_data_q <= 8'h00;
        end else begin
            mem_rd_valid_q <= 0;
            if (mem_rd_req) begin
                mem_rd_data_q <= (mem_rd_space == 3'd0) ? mem[mem_rd_addr[7:0]] : 8'hFF;
                mem_rd_valid_q <= 1;
            end
        end
    end

    assign mem_rd_data  = mem_rd_data_q;
    assign mem_rd_valid = mem_rd_valid_q;

    // =========================================================================
    // Octal SPI Protocol Processor Instance
    // =========================================================================
    esp32_ospi_proto_proc #(
        .USE_SYNC(USE_SYNC),
        .USE_CRC(USE_CRC),
        .IDLE_TO_CYC(IDLE_TO_CYC)
    ) proto (
        .clk(clk),
        .rst_n(rst_n),

        // Octal SPI interface
        .sclk(sclk),
        .data_in(data_i),
        .data_out(data_o),
        .data_oe(data_oe),

        // Register file interface
        .reg_wr_req(reg_wr_req),
        .reg_idx(reg_idx),
        .reg_wdata(reg_wdata),
        .reg_rdata(reg_rdata),

        // Memory interface
        .mem_wr_en(mem_wr_en),
        .mem_space(mem_space),
        .mem_wr_addr(mem_wr_addr),
        .mem_wr_data(mem_wr_data),
        .mem_rd_req(mem_rd_req),
        .mem_rd_space(mem_rd_space),
        .mem_rd_addr(mem_rd_addr),
        .mem_rd_valid(mem_rd_valid),
        .mem_rd_data(mem_rd_data)
    );

endmodule
