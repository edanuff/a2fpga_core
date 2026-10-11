//
// hyperram_ports with a mem_port_if client array (board-top convenience
// wrapper; see hyperram_ports.sv for the contract). The per-port interface
// fields are mapped to the flat vectors one assignment each, outside any
// procedural block.
//
// (c) 2026 Ed Anuff <ed@a2fpga.com> — same license as hyperram_ports.sv
//
`timescale 1ns / 1ps

module hyperram_ports_if #(
    parameter NUM_PORTS       = 4,
    parameter PORT_ADDR_WIDTH = 21,
    parameter [NUM_PORTS*PORT_ADDR_WIDTH-1:0] PORT_BASE_ADDR   = '0,
    parameter [NUM_PORTS*5-1:0]               PORT_BURST_WORDS = {NUM_PORTS{5'd4}},
    parameter integer WIDE_WR_PORT    = -1
) (
    input  wire clk,
    input  wire rst_n,

    mem_port_if.controller ports [NUM_PORTS],
    input  wire [95:0]  wide_wr_data_hi,

    input  wire                       hr_ready,
    input  wire                       hr_init_done,
    output wire                       hr_req,
    output wire                       hr_we,
    output wire [PORT_ADDR_WIDTH-1:0] hr_addr,
    output wire [4:0]                 hr_len,
    output wire [31:0]                hr_wdata,
    output wire [3:0]                 hr_wbe,
    output wire                       hr_wbuf_we,
    output wire [3:0]                 hr_wbuf_idx,
    output wire [31:0]                hr_wbuf_data,
    output wire [3:0]                 hr_wbuf_be,
    input  wire                       hr_rvalid,
    input  wire [31:0]                hr_rdata,
    input  wire                       hr_done,

    output wire [NUM_PORTS-1:0]       dbg_req_pending,
    output wire [3:0]                 dbg_state
);
    localparam AW = PORT_ADDR_WIDTH;
    wire [NUM_PORTS-1:0]    c_rd, c_wr, c_burst, c_available, c_ready;
    wire [NUM_PORTS*AW-1:0] c_addr;
    wire [NUM_PORTS*32-1:0] c_data;
    wire [NUM_PORTS*4-1:0]  c_be;
    wire [31:0]             c_q;

    generate
        for (genvar gi = 0; gi < NUM_PORTS; gi++) begin : gen_map
            assign c_rd[gi]              = ports[gi].rd;
            assign c_wr[gi]              = ports[gi].wr;
            assign c_burst[gi]           = ports[gi].burst;
            assign c_addr[gi*AW +: AW]   = ports[gi].addr;
            assign c_data[gi*32 +: 32]   = ports[gi].data;
            assign c_be[gi*4 +: 4]       = ports[gi].byte_en;
            assign ports[gi].available   = c_available[gi];
            assign ports[gi].ready       = c_ready[gi];
            assign ports[gi].q           = c_q;
        end
    endgenerate

    hyperram_ports #(
        .NUM_PORTS        (NUM_PORTS),
        .PORT_ADDR_WIDTH  (PORT_ADDR_WIDTH),
        .PORT_BASE_ADDR   (PORT_BASE_ADDR),
        .PORT_BURST_WORDS (PORT_BURST_WORDS),
        .WIDE_WR_PORT     (WIDE_WR_PORT)
    ) u_core (
        .clk (clk), .rst_n (rst_n),
        .c_rd (c_rd), .c_wr (c_wr), .c_burst (c_burst), .c_addr (c_addr),
        .c_data (c_data), .c_be (c_be), .wide_wr_data_hi (wide_wr_data_hi),
        .c_available (c_available), .c_ready (c_ready), .c_q (c_q),
        .hr_ready (hr_ready), .hr_init_done (hr_init_done),
        .hr_req (hr_req), .hr_we (hr_we), .hr_addr (hr_addr), .hr_len (hr_len),
        .hr_wdata (hr_wdata), .hr_wbe (hr_wbe),
        .hr_wbuf_we (hr_wbuf_we), .hr_wbuf_idx (hr_wbuf_idx),
        .hr_wbuf_data (hr_wbuf_data), .hr_wbuf_be (hr_wbuf_be),
        .hr_rvalid (hr_rvalid), .hr_rdata (hr_rdata), .hr_done (hr_done),
        .dbg_req_pending (dbg_req_pending), .dbg_state (dbg_state)
    );
endmodule
