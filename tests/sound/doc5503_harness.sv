// Simple harness for DOC5503 testing
// This wraps the DOC5503 module and provides simple register mocks

module doc5503_harness(
    input         clk_i,
    input         reset_n_i,
    input         clk_en_i,

    output        irq_n_o,

    input         cs_n_i,
    input         we_n_i,

    input  [7:0]  addr_i,
    input  [7:0]  data_i,
    output [7:0]  data_o,

    output [15:0] wave_address_o,
    output        wave_rd_o,
    input         wave_data_ready_i,
    input  [7:0]  wave_data_i,

    output signed [15:0] mono_mix_o,
    output signed [15:0] left_mix_o,
    output signed [15:0] right_mix_o
);
    // Mocking the register RAM modules
    // For testing, we'll use simple registers to hold values
    
    // Register declarations
    reg [7:0] freq_low[31:0];
    reg [7:0] freq_high[31:0];
    reg [7:0] volume[31:0];
    reg [7:0] data_sample[31:0];
    reg [7:0] addr_pointer[31:0];
    reg [7:0] control[31:0];
    reg [7:0] resolution_table_size[31:0];
    
    // Global registers
    reg [7:0] osc_int;
    reg [7:0] osc_en;
    
    // Register read logic
    reg [7:0] data_out;
    
    always @(*) begin
        case (addr_i[7:5])
            3'b000: data_out = freq_low[addr_i[4:0]];
            3'b001: data_out = freq_high[addr_i[4:0]];
            3'b010: data_out = volume[addr_i[4:0]];
            3'b011: data_out = data_sample[addr_i[4:0]];
            3'b100: data_out = addr_pointer[addr_i[4:0]];
            3'b101: data_out = control[addr_i[4:0]];
            3'b110: data_out = resolution_table_size[addr_i[4:0]];
            3'b111: begin
                case (addr_i[4:0])
                    5'd0: data_out = osc_int;
                    5'd1: data_out = osc_en;
                    default: data_out = 8'h00;
                endcase
            end
        endcase
    end
    
    assign data_o = data_out;
    
    // Register write logic
    always @(posedge clk_i or negedge reset_n_i) begin
        if (!reset_n_i) begin
            // Reset all registers
            for (int i = 0; i < 32; i++) begin
                freq_low[i] <= 8'h00;
                freq_high[i] <= 8'h00;
                volume[i] <= 8'h00;
                data_sample[i] <= 8'h00;
                addr_pointer[i] <= 8'h00;
                control[i] <= 8'h01; // Halt bit set
                resolution_table_size[i] <= 8'h00;
            end
            osc_int <= 8'h00;
            osc_en <= 8'h02; // One oscillator enabled by default
        end
        else if (!cs_n_i && !we_n_i) begin
            // Write to registers
            case (addr_i[7:5])
                3'b000: freq_low[addr_i[4:0]] <= data_i;
                3'b001: freq_high[addr_i[4:0]] <= data_i;
                3'b010: volume[addr_i[4:0]] <= data_i;
                3'b011: data_sample[addr_i[4:0]] <= data_i;
                3'b100: addr_pointer[addr_i[4:0]] <= data_i;
                3'b101: control[addr_i[4:0]] <= data_i;
                3'b110: resolution_table_size[addr_i[4:0]] <= data_i;
                3'b111: begin
                    case (addr_i[4:0])
                        5'd0: osc_int <= data_i;
                        5'd1: osc_en <= data_i;
                        default: ; // Do nothing
                    endcase
                end
            endcase
        end
    end
    
    // Instantiate the DOC5503 module
    doc5503 #(
        .OUTPUT_CHANNEL_MIX(1),
        .OUTPUT_MONO_MIX(1),
        .OUTPUT_STEREO_MIX(1),
        .NUM_CHANNELS(16)
    ) doc5503_inst (
        .clk_i(clk_i),
        .reset_n_i(reset_n_i),
        .clk_en_i(clk_en_i),

        .irq_n_o(irq_n_o),

        .cs_n_i(cs_n_i),
        .we_n_i(we_n_i),

        .addr_i(addr_i),
        .data_i(data_i),
        .data_o(data_o),

        .wave_address_o(wave_address_o),
        .wave_rd_o(wave_rd_o),
        .wave_data_ready_i(wave_data_ready_i),
        .wave_data_i(wave_data_i),

        .mono_mix_o(mono_mix_o),
        .left_mix_o(left_mix_o),
        .right_mix_o(right_mix_o),
        
        .channel_o()  // Not connected for this test
    );

endmodule