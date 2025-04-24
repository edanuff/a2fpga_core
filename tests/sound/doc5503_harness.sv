// Simple harness for DOC5503 testing
// This wraps the DOC5503 module and provides debug signals

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
    // Debug signals - expose internal state
    logic [4:0] dbg_cycle;
    logic [4:0] dbg_cycle_max;
    logic [7:0] dbg_osc_en;
    
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
        
        .channel_o(),  // Not connected for this test
        
        .osc_en_o(dbg_osc_en)    // Debug output for oscillator enable register
    );
    
    // Debug signals - monitor these from the testbench
    always @(posedge clk_i) begin
        if (doc5503_inst.ready_r) begin
            dbg_cycle <= doc5503_inst.cycle_r;
            dbg_cycle_max <= doc5503_inst.cycle_max_w;
        end
    end

endmodule