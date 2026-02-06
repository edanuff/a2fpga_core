//
// DDR3 SDRAM port interface wrapper for the A2Mega (Tang Mega 60K)
//
// (c) 2023,2024,2025 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// Description:
//
// Wraps the DDR3 SDRAM controller with SystemVerilog sdram_port_if interfaces
// for easier integration, matching the interface pattern used by sdram_ports.sv
// for standard SDRAM. This allows client modules (apple_memory, sound_glu,
// picosoc, DiskII, etc.) to use the same sdram_port_if.client interface
// regardless of whether the underlying memory is standard SDRAM or DDR3.
//
// Based on ddr3_framebuffer by nand2mario (https://github.com/nand2mario/ddr3_framebuffer_gowin)
// which demonstrates DDR3 access patterns for Gowin FPGAs.
//

module ddr3_ports #(
    // Port config
    parameter NUM_PORTS = 2,

    parameter PORT_ADDR_WIDTH = 29,
    parameter DATA_WIDTH = 32,
    parameter DQM_WIDTH = 4,
    parameter PORT_OUTPUT_WIDTH = 32,

    parameter DDR_ADDR_WIDTH = 29,   // Address width for DDR3 controller
    parameter DDR_DATA_WIDTH = 128   // Data width for DDR3 controller (128-bit burst)
) (
    input wire clk,
    input wire reset,
    output wire init_complete,

    // Ports - same sdram_port_if interface used by sdram_ports.sv
    sdram_port_if.controller ports[NUM_PORTS-1:0],

    // DDR3 controller interface - directly connected to Gowin DDR3 IP
    input init_calib_complete,

    input cmd_ready,
    output [2:0] cmd,
    output cmd_en,
    output [DDR_ADDR_WIDTH-1:0] addr,

    input wr_data_rdy,
    output [DDR_DATA_WIDTH-1:0] wr_data,
    output wr_data_en,
    output wr_data_end,
    output [DDR_DATA_WIDTH/8-1:0] wr_data_mask,

    input [DDR_DATA_WIDTH-1:0] rd_data,
    input rd_data_valid,
    input rd_data_end
);

    // Wire arrays to bridge sdram_port_if interfaces to the sdram module
    wire [PORT_ADDR_WIDTH-1:0] port_addr[NUM_PORTS-1:0];
    wire [DATA_WIDTH-1:0] port_data[NUM_PORTS-1:0];
    wire [DQM_WIDTH-1:0] port_byte_en[NUM_PORTS-1:0];
    wire [PORT_OUTPUT_WIDTH-1:0] port_q[NUM_PORTS-1:0];
    wire [DDR_DATA_WIDTH-1:0] port_q_burst[NUM_PORTS-1:0];
    wire port_wr[NUM_PORTS-1:0];
    wire port_rd[NUM_PORTS-1:0];
    wire port_available[NUM_PORTS-1:0];
    wire port_ready[NUM_PORTS-1:0];

    // Bridge sdram_port_if interfaces to wire arrays
    generate
        for (genvar i = 0; i < NUM_PORTS; i++) begin : port_interface_loop
            assign port_addr[i] = ports[i].addr;
            assign port_data[i] = ports[i].data;
            assign port_byte_en[i] = ports[i].byte_en;
            assign ports[i].q = port_q[i];
            assign port_wr[i] = ports[i].wr;
            assign port_rd[i] = ports[i].rd;
            assign ports[i].available = port_available[i];
            assign ports[i].ready = port_ready[i];
        end
    endgenerate

    // Instantiate the DDR3 SDRAM controller wrapper
    sdram #(
        .NUM_PORTS(NUM_PORTS),
        .PORT_ADDR_WIDTH(PORT_ADDR_WIDTH),
        .DATA_WIDTH(DATA_WIDTH),
        .DQM_WIDTH(DQM_WIDTH),
        .PORT_OUTPUT_WIDTH(PORT_OUTPUT_WIDTH),
        .DDR_ADDR_WIDTH(DDR_ADDR_WIDTH),
        .DDR_DATA_WIDTH(DDR_DATA_WIDTH)
    ) sdram_inst (
        .clk(clk),
        .reset(reset),
        .init_complete(init_complete),

        // Port arrays
        .port_addr(port_addr),
        .port_data(port_data),
        .port_byte_en(port_byte_en),
        .port_q(port_q),
        .port_q_burst(port_q_burst),
        .port_wr(port_wr),
        .port_rd(port_rd),
        .port_available(port_available),
        .port_ready(port_ready),

        // DDR3 controller interface
        .init_calib_complete(init_calib_complete),
        .cmd_ready(cmd_ready),
        .cmd(cmd),
        .cmd_en(cmd_en),
        .addr(addr),
        .wr_data_rdy(wr_data_rdy),
        .wr_data(wr_data),
        .wr_data_en(wr_data_en),
        .wr_data_end(wr_data_end),
        .wr_data_mask(wr_data_mask),
        .rd_data(rd_data),
        .rd_data_valid(rd_data_valid),
        .rd_data_end(rd_data_end)
    );

endmodule
