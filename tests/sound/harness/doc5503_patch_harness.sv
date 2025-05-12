// Simple harness for testing the patched DOC5503 module

module doc5503_patch_harness(
    input         clk_i,
    input         reset_n_i,
    input         clk_en_i,

    output logic  irq_n_o,         // Not used in patch test

    input         cs_n_i,
    input         we_n_i,

    input  [7:0]  addr_i,
    input  [7:0]  data_i,
    output [7:0]  data_o,

    output [15:0] wave_address_o,  // Not used in patch test
    output        wave_rd_o,       // Not used in patch test
    input         wave_data_ready_i, // Not used in patch test
    input  [7:0]  wave_data_i,     // Not used in patch test

    output signed [15:0] mono_mix_o, // Not used in patch test
    output signed [15:0] left_mix_o, // Not used in patch test
    output signed [15:0] right_mix_o, // Not used in patch test
    
    // Debug outputs for testing
    output [7:0]  dbg_osc_en_o,
    
    // Module ready signal
    output        ready_o
);

    // Instantiate the patched DOC5503 module
    doc5503_patch #(
        .CLOCK_SPEED_HZ(54_000_000),
        .DOC_CLOCK_SPEED_HZ(7_159_090)
    ) doc5503_patch_inst (
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
        
        // Connect ready signal
        .ready_o(ready_o),

        // Connect osc_en_o for debug
        .osc_en_o(dbg_osc_en_o)
    );

endmodule