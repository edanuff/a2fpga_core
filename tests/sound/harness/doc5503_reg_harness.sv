// Simplified harness for DOC5503 register testing
// This wraps the DOC5503 module and focuses only on register access

module doc5503_reg_harness #(
    parameter int CLOCK_SPEED_HZ = 54_000_000,
    parameter int DOC_CLOCK_SPEED_HZ = 7_159_090
) (
    input         clk_i,
    input         reset_n_i,
    input         clk_en_i,

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
    output signed [15:0] right_mix_o,
    
    // Module ready signal
    output        ready_o         // Indicates when the module is ready
);

    // Instantiate the DOC5503 module
    doc5503 #(
        .CLOCK_SPEED_HZ(CLOCK_SPEED_HZ),
        .DOC_CLOCK_SPEED_HZ(DOC_CLOCK_SPEED_HZ)
    ) doc5503_inst (
        .clk_i(clk_i),
        .reset_n_i(reset_n_i),
        .clk_en_i(clk_en_i),

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
        
        .channel_o(),  // Not connected for this test
        
        .ready_o(ready_o),
        
        .osc_en_o()   // Not connected for this test
    );

endmodule
