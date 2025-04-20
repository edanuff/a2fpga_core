// Sound GLU Test Harness
// Tests the complete sound path from GLU through DOC to audio output

module glu_harness (
    input clk_i,
    input reset_n_i,
    
    // GLU Register Interface
    input reg_cs_n_i,
    input reg_we_n_i,
    input [15:0] reg_addr_i,
    input [7:0] reg_data_i,
    output [7:0] reg_data_o,
    
    // Clock enables
    input clk_7m_posedge_i,
    
    // Audio outputs
    output [15:0] audio_l_o,
    output [15:0] audio_r_o,
    
    // Memory interface
    input sdram_ready_i,
    output sdram_rd_o,
    output [23:0] sdram_addr_o,
    input [31:0] sdram_data_i
);

    // Apple Bus Interface
    a2bus_if bus_if();
    
    // Connect the interface to inputs
    assign bus_if.clk_logic = clk_i;
    assign bus_if.system_reset_n = reset_n_i;
    assign bus_if.phi0 = 1'b1;  // Always enabled for testing
    assign bus_if.m2sel_n = 1'b0;  // Always selected for testing
    assign bus_if.rw_n = reg_we_n_i;
    assign bus_if.addr = reg_addr_i;
    assign bus_if.data = reg_data_i;
    assign bus_if.data_in_strobe = ~reg_cs_n_i & ~reg_we_n_i;
    assign bus_if.clk_7m_posedge = clk_7m_posedge_i;
    
    // SDRAM Port Interfaces
    sdram_port_if glu_mem_if();
    sdram_port_if doc_mem_if();
    
    // Connect memory interfaces to external pins
    assign glu_mem_if.ready = sdram_ready_i;
    assign doc_mem_if.ready = sdram_ready_i;
    assign doc_mem_if.q = sdram_data_i;
    assign glu_mem_if.q = sdram_data_i;
    
    // Mux the memory requests
    assign sdram_rd_o = glu_mem_if.rd | doc_mem_if.rd;
    assign sdram_addr_o = glu_mem_if.rd ? glu_mem_if.addr : doc_mem_if.addr;
    
    // IRQ output (not used in test but needed for GLU)
    wire irq_n_w;
    
    // Instantiate Sound GLU module
    sound_glu glu_inst (
        .a2bus_if(bus_if),
        .data_o(reg_data_o),
        .rd_en_o(),       // Not used in test
        .irq_n_o(irq_n_w), // Not used in test
        .audio_l_o(audio_l_o),
        .audio_r_o(audio_r_o),
        .glu_mem_if(glu_mem_if),
        .doc_mem_if(doc_mem_if)
    );

endmodule