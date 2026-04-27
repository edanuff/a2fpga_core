// BL616 4-Wire SPI Protocol Processor
// Adapted from esp32_spi_proto_proc.sv
// Key changes: CS# framing (no sync bytes, no idle timeout)
module bl616_spi_proto_proc #(
    parameter USE_CRC = 0
)(
    input  wire        clk,
    input  wire        rst_n,

    // SPI pins
    input  wire        spi_cs_n,
    input  wire        spi_sclk,
    input  wire        spi_mosi,
    output wire        spi_miso,

    // Register file interface
    output reg         reg_rd_req,
    output reg         reg_wr_req,
    output reg  [6:0]  reg_idx,
    output reg  [7:0]  reg_wdata,
    input  wire [7:0]  reg_rdata,

    // Memory/XFER interface
    output reg         mem_wr_en,
    output reg  [2:0]  mem_space,
    output reg  [23:0] mem_wr_addr,
    output reg  [7:0]  mem_wr_data,

    output reg         mem_rd_req,
    output reg  [2:0]  mem_rd_space,
    output reg  [23:0] mem_rd_addr,
    input  wire        mem_rd_valid,
    input  wire [7:0]  mem_rd_data
);

    // -------------------------------------------------------
    // 2FF synchronizers for SPI inputs
    // -------------------------------------------------------
    reg cs_n_q1, cs_n_q2, sclk_q1, sclk_q2, mosi_q1, mosi_q2;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cs_n_q1 <= 1'b1; cs_n_q2 <= 1'b1;
            sclk_q1 <= 1'b0; sclk_q2 <= 1'b0;
            mosi_q1 <= 1'b0; mosi_q2 <= 1'b0;
        end else begin
            cs_n_q1 <= spi_cs_n; cs_n_q2 <= cs_n_q1;
            sclk_q1 <= spi_sclk; sclk_q2 <= sclk_q1;
            mosi_q1 <= spi_mosi; mosi_q2 <= mosi_q1;
        end
    end

    wire cs_active = !cs_n_q2;
    wire sclk_rise = !sclk_q2 && sclk_q1;
    wire sclk_fall = sclk_q2 && !sclk_q1;

    // -------------------------------------------------------
    // Bit/byte shifters
    // -------------------------------------------------------
    reg [2:0] bit_cnt;
    reg [7:0] rx_shift;
    reg       byte_rx_stb;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            bit_cnt     <= 3'd0;
            rx_shift    <= 8'h00;
            byte_rx_stb <= 1'b0;
        end else begin
            byte_rx_stb <= 1'b0;
            if (!cs_active) begin
                bit_cnt <= 3'd0;
            end else if (sclk_rise) begin
                rx_shift[7 - bit_cnt] <= mosi_q2;
                bit_cnt <= bit_cnt + 3'd1;
                if (bit_cnt == 3'd7) begin
                    bit_cnt     <= 3'd0;
                    byte_rx_stb <= 1'b1;
                end
            end
        end
    end

    // -------------------------------------------------------
    // Status byte
    // -------------------------------------------------------
    localparam [7:0] PROTO_VER = 8'h01;
    reg status_crcerr, status_busy, status_ok;
    wire [7:0] status_byte = {PROTO_VER[3:0], 1'b0, status_crcerr, status_busy, status_ok};

    // -------------------------------------------------------
    // FSM states
    // -------------------------------------------------------
    localparam [3:0]
        ST_IDLE        = 4'd0,
        ST_OPCODE      = 4'd1,
        ST_REG_RW      = 4'd2,
        ST_X0          = 4'd3,
        ST_XA0         = 4'd4,
        ST_XA1         = 4'd5,
        ST_XA2         = 4'd6,
        ST_XL0         = 4'd7,
        ST_XL1         = 4'd8,
        ST_XPAY_WR     = 4'd9,
        ST_XPAY_RD_DMY = 4'd10,
        ST_XPAY_RD     = 4'd11,
        ST_DONE        = 4'd12;
    reg [3:0] st;

    // Header fields
    reg        op_is_read;
    reg [6:0]  op_reg;
    reg [7:0]  sub0;
    reg [23:0] addr;
    reg [15:0] len, len_cnt;
    reg        sub_dir, sub_crc, sub_inc;
    reg [2:0]  sub_space;

    // Read pipeline
    reg [7:0] rd_buf;
    reg       rd_buf_valid;
    reg [7:0] reg_read_value;
    reg       load_reg_read_next;

    // -------------------------------------------------------
    // TX shift register -- MISO output
    // -------------------------------------------------------
    reg [7:0] tx_shift;
    assign spi_miso = tx_shift[7];

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tx_shift <= 8'hFF;
        end else if (!cs_active) begin
            tx_shift <= status_byte;
        end else if (sclk_fall) begin
            if (bit_cnt == 3'd0) begin
                // Load next byte at byte boundary
                if (st == ST_REG_RW && op_is_read && (op_reg != 7'd127)) begin
                    tx_shift <= reg_rdata;
                end else if (load_reg_read_next) begin
                    tx_shift <= reg_read_value;
                end else begin
                    case (st)
                        ST_XPAY_RD_DMY: tx_shift <= 8'hFF;
                        ST_XPAY_RD:     tx_shift <= rd_buf_valid ? rd_buf : 8'hFF;
                        default:         tx_shift <= status_byte;
                    endcase
                end
            end else begin
                tx_shift <= {tx_shift[6:0], 1'b1};
            end
        end
    end

    // -------------------------------------------------------
    // Main FSM
    // -------------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            st            <= ST_IDLE;
            status_crcerr <= 1'b0;
            status_busy   <= 1'b0;
            status_ok     <= 1'b1;
            op_is_read    <= 1'b0;
            op_reg        <= 7'd0;
            sub0          <= 8'd0;
            addr          <= 24'd0;
            len           <= 16'd0;
            len_cnt       <= 16'd0;
            sub_dir       <= 1'b0;
            sub_crc       <= 1'b0;
            sub_inc       <= 1'b0;
            sub_space     <= 3'd0;
            rd_buf        <= 8'h00;
            rd_buf_valid  <= 1'b0;
            load_reg_read_next <= 1'b0;
            reg_read_value     <= 8'h00;
            reg_rd_req    <= 1'b0;
            reg_wr_req    <= 1'b0;
            reg_idx       <= 7'd0;
            reg_wdata     <= 8'd0;
            mem_wr_en     <= 1'b0;
            mem_space     <= 3'd0;
            mem_wr_addr   <= 24'd0;
            mem_wr_data   <= 8'd0;
            mem_rd_req    <= 1'b0;
            mem_rd_space  <= 3'd0;
            mem_rd_addr   <= 24'd0;
        end else begin
            // One-shot clears
            reg_rd_req <= 1'b0;
            reg_wr_req <= 1'b0;
            mem_wr_en  <= 1'b0;
            mem_rd_req <= 1'b0;

            // CS# deassert -> IDLE
            if (!cs_active) begin
                st           <= ST_IDLE;
                rd_buf_valid <= 1'b0;
                load_reg_read_next <= 1'b0;
            end else begin
                // Latch mem read data
                if (mem_rd_valid) begin
                    rd_buf       <= mem_rd_data;
                    rd_buf_valid <= 1'b1;
                end

                if (byte_rx_stb) begin
                    load_reg_read_next <= 1'b0;

                    case (st)
                        ST_IDLE: begin
                            status_ok    <= 1'b1;
                            status_crcerr <= 1'b0;
                            status_busy  <= 1'b0;
                            rd_buf_valid <= 1'b0;
                            // First byte in a CS# assertion is always opcode
                            op_is_read <= rx_shift[7];
                            op_reg     <= rx_shift[6:0];
                            reg_idx    <= rx_shift[6:0];
                            if (rx_shift[6:0] != 7'd127) begin
                                if (rx_shift[7]) begin
                                    // READ register
                                    reg_read_value     <= reg_rdata;
                                    load_reg_read_next <= 1'b1;
                                    st <= ST_REG_RW;
                                end else begin
                                    // WRITE register
                                    st <= ST_REG_RW;
                                end
                            end else begin
                                st <= ST_X0;
                            end
                        end

                        ST_REG_RW: begin
                            if (op_is_read) begin
                                // Read done, next byte is next opcode
                                reg_rd_req <= 1'b1;
                                st <= ST_DONE;
                            end else begin
                                // Write payload byte
                                reg_wdata  <= rx_shift;
                                reg_wr_req <= 1'b1;
                                st <= ST_DONE;
                            end
                        end

                        ST_DONE: begin
                            // Back-to-back: treat next byte as opcode
                            rd_buf_valid <= 1'b0;
                            op_is_read <= rx_shift[7];
                            op_reg     <= rx_shift[6:0];
                            reg_idx    <= rx_shift[6:0];
                            if (rx_shift[6:0] != 7'd127) begin
                                if (rx_shift[7]) begin
                                    reg_read_value     <= reg_rdata;
                                    load_reg_read_next <= 1'b1;
                                    st <= ST_REG_RW;
                                end else begin
                                    st <= ST_REG_RW;
                                end
                            end else begin
                                st <= ST_X0;
                            end
                        end

                        // XFER subheader
                        ST_X0: begin
                            sub0      <= rx_shift;
                            sub_dir   <= rx_shift[0];
                            sub_space <= rx_shift[3:1];
                            sub_inc   <= rx_shift[4];
                            sub_crc   <= rx_shift[5];
                            addr      <= 24'd0;
                            len       <= 16'd0;
                            st <= ST_XA0;
                        end
                        ST_XA0: begin addr[7:0]   <= rx_shift; st <= ST_XA1; end
                        ST_XA1: begin addr[15:8]  <= rx_shift; st <= ST_XA2; end
                        ST_XA2: begin addr[23:16] <= rx_shift; st <= ST_XL0; end
                        ST_XL0: begin len[7:0]    <= rx_shift; st <= ST_XL1; end
                        ST_XL1: begin
                            len[15:8]    <= rx_shift;
                            len_cnt      <= {rx_shift, len[7:0]};
                            mem_space    <= sub_space;
                            mem_rd_space <= sub_space;
                            st <= sub_dir ? ST_XPAY_RD_DMY : ST_XPAY_WR;
                        end

                        // WRITE payload
                        ST_XPAY_WR: begin
                            if (len_cnt == 16'd0) begin
                                st <= ST_DONE;
                            end else begin
                                mem_wr_addr <= addr;
                                mem_wr_data <= rx_shift;
                                mem_wr_en   <= 1'b1;
                                len_cnt <= len_cnt - 16'd1;
                                if (sub_inc) addr <= addr + 24'd1;
                                if (len_cnt == 16'd1) st <= ST_DONE;
                            end
                        end

                        // READ payload (dummy byte first)
                        ST_XPAY_RD_DMY: begin
                            mem_rd_addr <= addr;
                            mem_rd_req  <= 1'b1;
                            st <= ST_XPAY_RD;
                        end

                        ST_XPAY_RD: begin
                            if (len_cnt == 16'd0) begin
                                st <= ST_DONE;
                            end else if (rd_buf_valid) begin
                                len_cnt      <= len_cnt - 16'd1;
                                rd_buf_valid <= 1'b0;
                                if (len_cnt > 16'd1) begin
                                    mem_rd_addr <= sub_inc ? (addr + 24'd1) : addr;
                                    mem_rd_req  <= 1'b1;
                                    if (sub_inc) addr <= addr + 24'd1;
                                end
                            end
                        end

                        default: st <= ST_IDLE;
                    endcase
                end
            end
        end
    end

endmodule
