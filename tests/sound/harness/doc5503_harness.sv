// Enhanced harness for DOC5503 testing
// This wraps the DOC5503 module and provides extensive debug signals

module doc5503_harness(
    input         clk_i,
    input         reset_n_i,
    input         clk_en_i,

    output logic  irq_n_o, // Placeholder - not connected in doc5503 module

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
    
    // Debug outputs for advanced testing
    output [4:0]  dbg_cycle_o,
    output [2:0]  dbg_osc_state_o,
    output [7:0]  dbg_vol_o,
    output [7:0]  dbg_wds_o,
    output signed [15:0] dbg_output_o,
    
    // Clock synchronization signals
    output [2:0]  dbg_E_o,            // Debug clock count signal
    output logic [2:0]  dbg_clk_count_o,    // Debug clock counter (not connected to doc5503)
    
    // Module ready signal
    output        ready_o             // Indicates when the module is ready
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
    
    // REMOVE THESE DIRECT ASSIGNMENTS - They don't work because the ports are left unconnected below
    // assign wave_address_o = doc5503_inst.wave_address_o;
    // assign wave_rd_o = doc5503_inst.wave_rd_o;
    
    // Expose internal signals to make them directly accessible to the testbench
    // These assignments will be available to debug testbenches to examine internal state
    
    // Instantiate the DOC5503 module with refactored design
    doc5503 #(
        .CLOCK_SPEED_HZ(54_000_000),
        .DOC_CLOCK_SPEED_HZ(7_159_090)
    ) doc5503_inst (
        .clk_i(clk_i),
        .reset_n_i(reset_n_i),
        .clk_en_i(clk_en_i),

        // .irq_n_o(irq_n_o), // Not connected in doc5503 module

        .cs_n_i(cs_n_i),
        .we_n_i(we_n_i),

        .addr_i(addr_i),
        .data_i(data_i),
        .data_o(data_o),

        .wave_address_o(wave_address_o), // Connect directly to harness outputs
        .wave_rd_o(wave_rd_o),      // Connect directly to harness outputs
        .wave_data_ready_i(wave_data_ready_i),
        .wave_data_i(wave_data_i),

        .mono_mix_o(mono_mix_o),
        .left_mix_o(left_mix_o),
        .right_mix_o(right_mix_o),
        
        .channel_o(),  // Not connected for this test
        
        // Connect debug signals
        // .clk_count_o(dbg_clk_count_o), // Not connected in doc5503 module

        // Connect ready signal
        .ready_o(ready_o),

        // Connect osc_en_o for debug
        .osc_en_o()
    );
    
    // Expose debug signals to external world
    assign dbg_cycle_o = dbg_cycle;
    assign dbg_osc_state_o = dbg_osc_state;
    assign dbg_vol_o = dbg_vol;
    assign dbg_wds_o = dbg_wds;
    assign dbg_output_o = dbg_output;
    
    // Enhanced debug monitoring for refactored code
    always @(posedge clk_i) begin
        // Map to new signal naming from refactored doc5503.sv
        dbg_cycle <= doc5503_inst.curr_osc_r;
        dbg_cycle_max <= doc5503_inst.osc_max_w;
        dbg_osc_state <= doc5503_inst.osc_state_r;
        dbg_output <= doc5503_inst.curr_output_r;
        dbg_wds <= doc5503_inst.curr_wds_r;
        dbg_vol <= doc5503_inst.curr_vol_r;
        dbg_control <= doc5503_inst.curr_control_r;
        dbg_acc <= doc5503_inst.curr_acc_r;
        dbg_osc_en <= doc5503_inst.osc_en_r;
        
        // In refactored code, we don't need the ready_r check
        dbg_ready <= 1'b1;
        
        // Debug output check code removed
        // We'll let the actual DOC5503 module generate outputs
    end

endmodule
