// Sound GLU Test Harness
// Tests the complete sound path from GLU through DOC to audio output

// Simplified a2bus_if for testing
interface a2bus_if_test();
    logic clk_logic;
    logic clk_pixel;
    logic system_reset_n;
    logic device_reset_n;
    logic phi0;
    logic phi1;
    logic phi1_posedge;
    logic phi1_negedge;
    logic clk_2m_posedge;
    logic clk_7m;
    logic clk_7m_posedge;
    logic clk_7m_negedge;
    logic clk_14m_posedge;
    
    logic sw_gs;
    logic rw_n;
    logic [15:0] addr;
    logic m2sel_n;
    logic m2b0;
    logic [7:0] data;
    logic data_in_strobe;
    
    // These are missing in the original interface but needed for sound_glu
    logic io_select_n = 1'b1;
    logic dev_select_n = 1'b1;
    logic io_strobe_n = 1'b1;

    modport slave (
        input clk_logic,
        input clk_pixel,
        input system_reset_n,
        input device_reset_n,
        input phi0,
        input phi1,
        input phi1_posedge,
        input phi1_negedge,
        input clk_2m_posedge,
        input clk_7m,
        input clk_7m_posedge,
        input clk_7m_negedge,
        input clk_14m_posedge,
        
        input rw_n,
        input addr,
        input m2sel_n,
        input m2b0,
        input data,
        input data_in_strobe,
        input sw_gs,

        input io_select_n,
        input dev_select_n,
        input io_strobe_n
    );
endinterface

// Simplified sdram_port_if for testing
interface sdram_port_if_test();
    logic rd;
    logic wr;
    logic ready;
    logic [23:0] addr;
    logic [31:0] data;
    logic [3:0] byte_en;
    logic [31:0] q;

    modport client (
        output rd,
        output wr,
        input ready,
        output addr,
        output data,
        output byte_en,
        input q
    );
endinterface

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
    
    // Audio outputs (now signed for proper audio handling)
    output signed [15:0] audio_l_o,
    output signed [15:0] audio_r_o,
    
    // Memory interface
    input sdram_ready_i,
    output sdram_rd_o,
    output [23:0] sdram_addr_o,
    input [31:0] sdram_data_i
);

    // Apple Bus Interface
    a2bus_if_test bus_if();
    
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
    sdram_port_if_test glu_mem_if();
    sdram_port_if_test doc_mem_if();
    
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