// Enhanced harness for DOC5503 testing
// This wraps the DOC5503 module and provides extensive debug signals

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
    // Debug signals - expose more internal state
    logic [4:0] dbg_cycle;
    logic [4:0] dbg_cycle_max;
    logic [7:0] dbg_osc_en;
    logic [2:0] dbg_osc_state;
    logic dbg_ready;
    logic signed [15:0] dbg_output;
    logic [7:0] dbg_wds;
    logic [7:0] dbg_vol;
    logic [7:0] dbg_control;
    logic [23:0] dbg_acc;
    
    // Connect wave signals directly to doc5503
    assign wave_address_o = doc5503_inst.wave_address_o;
    assign wave_rd_o = doc5503_inst.wave_rd_o;
    
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

        .wave_address_o(), // Connected directly above
        .wave_rd_o(),      // Connected directly above
        .wave_data_ready_i(wave_data_ready_i),
        .wave_data_i(wave_data_i),

        .mono_mix_o(mono_mix_o),
        .left_mix_o(left_mix_o),
        .right_mix_o(right_mix_o),
        
        .channel_o(),  // Not connected for this test
        
        .osc_en_o(dbg_osc_en)
    );
    
    // Enhanced debug monitoring
    always @(posedge clk_i) begin
        if (doc5503_inst.ready_r) begin
            dbg_cycle <= doc5503_inst.cycle_r;
            dbg_cycle_max <= doc5503_inst.cycle_max_w;
            dbg_osc_state <= doc5503_inst.osc_state_r;
            dbg_output <= doc5503_inst.output_r;
            dbg_wds <= doc5503_inst.wds_w;
            dbg_vol <= doc5503_inst.vol_w;
            dbg_control <= doc5503_inst.control_w;
            dbg_acc <= doc5503_inst.acc_w;
        end
        dbg_ready <= doc5503_inst.ready_r;
    end

endmodule