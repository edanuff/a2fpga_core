//
// Apple II Bus Capture with I2S 4-bit Stream Interface
// Properly using the a2bus_if interface
//
// (c) 2025 Correct bus interface usage
//

// I2S Stream Module (unchanged)
module i2s_4bit_stream_fifo #(
    parameter FIFO_DEPTH = 32,
    parameter FIFO_ADDR_WIDTH = $clog2(FIFO_DEPTH)
)(
    input         clk,
    input         rst_n,
    input         fifo_wr_en,
    input  [31:0] fifo_data_in,
    output        fifo_full,
    output        fifo_empty,
    output        fifo_almost_full,
    output        qclk,
    output        frame,
    output [3:0]  qdata,
    output [FIFO_ADDR_WIDTH:0] fifo_count
);

    // Clock Generation: 54 MHz → 27 MHz
    reg clk_div = 1'b0;
    assign qclk = clk_div;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            clk_div <= 1'b0;
        else
            clk_div <= ~clk_div;
    end

    // Edge detection for state machine timing
    reg clk_div_d = 1'b0;
    wire qclk_rising = (clk_div == 1'b1 && clk_div_d == 1'b0);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            clk_div_d <= 1'b0;
        else
            clk_div_d <= clk_div;
    end

    // FIFO Implementation
    reg [31:0] fifo_mem[FIFO_DEPTH-1:0];
    reg [FIFO_ADDR_WIDTH:0] wr_ptr = 0;
    reg [FIFO_ADDR_WIDTH:0] rd_ptr = 0;
    reg [FIFO_ADDR_WIDTH:0] fifo_count_reg = 0;

    assign fifo_count = fifo_count_reg;
    assign fifo_full = (fifo_count_reg == FIFO_DEPTH);
    assign fifo_empty = (fifo_count_reg == 0);
    assign fifo_almost_full = (fifo_count_reg >= (FIFO_DEPTH - 2));

    // FIFO write logic
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr <= 0;
        end else if (fifo_wr_en && !fifo_full) begin
            fifo_mem[wr_ptr[FIFO_ADDR_WIDTH-1:0]] <= fifo_data_in;
            wr_ptr <= wr_ptr + 1;
        end
    end

    // FIFO read logic
    wire fifo_rd_en;
    reg [31:0] fifo_data_out;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rd_ptr <= 0;
            fifo_data_out <= 32'h0;
        end else if (fifo_rd_en && !fifo_empty) begin
            fifo_data_out <= fifo_mem[rd_ptr[FIFO_ADDR_WIDTH-1:0]];
            rd_ptr <= rd_ptr + 1;
        end
    end

    // FIFO count management
    wire fifo_wr_valid = fifo_wr_en && !fifo_full;
    wire fifo_rd_valid = fifo_rd_en && !fifo_empty;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            fifo_count_reg <= 0;
        end else begin
            case ({fifo_wr_valid, fifo_rd_valid})
                2'b10: fifo_count_reg <= fifo_count_reg + 1;
                2'b01: fifo_count_reg <= fifo_count_reg - 1;
                default: fifo_count_reg <= fifo_count_reg;
            endcase
        end
    end

    // Output State Machine
    localparam [1:0] IDLE     = 2'b00,
                     LOADING  = 2'b01,
                     SENDING  = 2'b10;

    reg [1:0]  state = IDLE;
    reg [1:0]  next_state;
    reg [31:0] shift_reg = 32'h0;
    reg [2:0]  bit_cnt = 3'b0;
    reg        frame_reg = 1'b0;
    reg [3:0]  qdata_reg = 4'h0;

    assign frame = frame_reg;
    assign qdata = qdata_reg;
    assign fifo_rd_en = (state == IDLE) && !fifo_empty && qclk_rising;

    // State machine
    always @(*) begin
        next_state = state;
        case (state)
            IDLE: begin
                if (!fifo_empty && qclk_rising)
                    next_state = LOADING;
            end
            LOADING: begin
                if (qclk_rising)
                    next_state = SENDING;
            end
            SENDING: begin
                if (qclk_rising && bit_cnt == 3'd7)
                    next_state = IDLE;
            end
            default: next_state = IDLE;
        endcase
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            state <= IDLE;
        else
            state <= next_state;
    end

    // Output logic
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            shift_reg <= 32'h0;
            bit_cnt <= 3'b0;
            frame_reg <= 1'b0;
            qdata_reg <= 4'h0;
        end else if (qclk_rising) begin
            case (state)
                IDLE: begin
                    frame_reg <= 1'b0;
                    qdata_reg <= 4'h0;
                    bit_cnt <= 3'b0;
                end
                LOADING: begin
                    shift_reg <= fifo_data_out;
                    frame_reg <= 1'b1;
                    qdata_reg <= fifo_data_out[31:28];
                    bit_cnt <= 3'b1;
                end
                SENDING: begin
                    frame_reg <= 1'b0;
                    shift_reg <= {shift_reg[27:0], 4'h0};
                    qdata_reg <= shift_reg[27:24];
                    bit_cnt <= bit_cnt + 1;
                end
                default: begin
                    frame_reg <= 1'b0;
                    qdata_reg <= 4'h0;
                end
            endcase
        end
    end

endmodule

