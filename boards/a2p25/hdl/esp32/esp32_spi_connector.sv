// ============================================================================
// Parent: esp32_spi_connector
//  - Owns 16×8-bit discrete registers and a 256B memory (SPACE=0)
//  - Memory is implemented as synchronous read (1-cycle latency) to
//    exercise the child's mem_rd_valid handshake.
// ============================================================================
module esp32_spi_connector #(
  parameter bit USE_SYNC    = 1,
  parameter bit USE_CRC     = 0,
  parameter int IDLE_TO_CYC = 54_000
)(
  input  logic clk,         // 54 MHz
  input  logic rst_n,
  input  logic sclk,
  input  logic mosi,
  output logic miso
);

  // ---- 16×8-bit discrete registers ----
  logic [7:0] reg0,  reg1,  reg2,  reg3;
  logic [7:0] reg4,  reg5,  reg6,  reg7;
  logic [7:0] reg8,  reg9,  reg10, reg11;
  logic [7:0] reg12, reg13, reg14, reg15;

  localparam [7:0] DEVICE_ID0 = "A";
  localparam [7:0] DEVICE_ID1 = "2";
  localparam [7:0] DEVICE_ID2 = "F";
  localparam [7:0] DEVICE_ID3 = "P";
  localparam [7:0] PROTO_VER  = 8'h01;
  localparam [7:0] CAP0       = {6'b0, USE_CRC, 1'b1}; // bit0: XFER present, bit1: CRC support

  // ---- 256B memory (SPACE=0) with synchronous read (1-cycle latency) ----
  logic [7:0] mem [0:255];
  logic [7:0] mem_rd_data_q;
  logic       mem_rd_valid_q;

  // ---- Wires to child ----
  logic        reg_wr_req;
  logic [6:0]  reg_idx;
  logic [7:0]  reg_wdata;
  logic [7:0]  reg_rdata;

  logic        mem_wr_en;
  logic [2:0]  mem_space;
  logic [23:0] mem_wr_addr;
  logic [7:0]  mem_wr_data;

  logic        mem_rd_req;
  logic [2:0]  mem_rd_space;
  logic [23:0] mem_rd_addr;
  logic        mem_rd_valid;
  logic [7:0]  mem_rd_data;

  // ---- Combinational register read mux ----
  always_comb begin
    unique case (reg_idx[3:0])
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

  // ---- Register writes ----
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      reg0  <= DEVICE_ID0; reg1  <= DEVICE_ID1; reg2  <= DEVICE_ID2; reg3  <= DEVICE_ID3;
      reg4  <= PROTO_VER;  reg5  <= CAP0;
      reg6  <= 8'h00; reg7  <= 8'h00; reg8  <= 8'h00; reg9  <= 8'h00;
      reg10 <= 8'h00; reg11 <= 8'h00; reg12 <= 8'h00; reg13 <= 8'h00; reg14 <= 8'h00; reg15 <= 8'h00;
    end else if (reg_wr_req) begin
      unique case (reg_idx[3:0])
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

  // ---- Memory writes (SPACE 0 only) ----
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin : init_mem
      integer i;
      for (i=0;i<256;i++) mem[i] <= 8'h00;
    end else if (mem_wr_en && (mem_space==3'd0)) begin
      mem[mem_wr_addr[7:0]] <= mem_wr_data;
    end
  end

  // ---- Memory synchronous read (1-cycle latency) ----
  // Request on mem_rd_req; present data and mem_rd_valid one clk later.
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      mem_rd_data_q  <= 8'h00;
      mem_rd_valid_q <= 1'b0;
    end else begin
      mem_rd_valid_q <= 1'b0;
      if (mem_rd_req) begin
        mem_rd_data_q  <= (mem_rd_space==3'd0) ? mem[mem_rd_addr[7:0]] : 8'hFF;
        mem_rd_valid_q <= 1'b1; // exactly 1 cycle after request
      end
    end
  end
  assign mem_rd_data  = mem_rd_data_q;
  assign mem_rd_valid = mem_rd_valid_q;

  // ---- Instantiate protocol processor ----
  esp32_spi_proto_proc #(
    .USE_SYNC(USE_SYNC),
    .USE_CRC(USE_CRC),
    .IDLE_TO_CYC(IDLE_TO_CYC)
  ) u_proto (
    .clk, .rst_n,
    .sclk, .mosi, .miso,

    .reg_wr_req, .reg_idx, .reg_wdata, .reg_rdata,

    .mem_wr_en, .mem_space, .mem_wr_addr, .mem_wr_data,
    .mem_rd_req, .mem_rd_space, .mem_rd_addr, .mem_rd_valid, .mem_rd_data
  );

endmodule