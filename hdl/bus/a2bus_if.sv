//
// Apple II Bus Interface
//
// (c) 2023,2024 Ed Anuff <ed@a2fpga.com> 
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
// SystemVerilog interface for the Apple II bus
//

interface a2bus_if (
    input logic clk_logic,
    input logic clk_pixel,
    input logic system_reset_n,
    input logic device_reset_n,
    input logic phi0,
    input logic phi1,
    input logic phi1_posedge,
    input logic phi1_negedge,
    input logic clk_2m_posedge,
    input logic clk_7m,
    input logic clk_7m_posedge,
    input logic clk_7m_negedge,
    input logic clk_14m_posedge
);

    logic sw_gs;
    logic rw_n;
    logic [15:0] addr;
    logic m2sel_n;
    logic m2b0;
    logic [7:0] data;
    logic data_in_strobe;

    logic control_inh_n;
    logic control_irq_n;
    logic control_rdy_n;
    logic control_dma_n;
    logic control_nmi_n;
    logic control_reset_n;
    
    modport master (
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
        
        output rw_n,
        output addr,
        output m2sel_n,
        output m2b0,
        output data,
        output data_in_strobe,
        output sw_gs,

        output control_inh_n,
        output control_irq_n,
        output control_rdy_n,
        output control_dma_n,
        output control_nmi_n,
        output control_reset_n
    );

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

        input control_inh_n,
        input control_irq_n,
        input control_rdy_n,
        input control_dma_n,
        input control_nmi_n,
        input control_reset_n,

        import io_select_n,
        import dev_select_n,
        import io_strobe_n
    );

endinterface: a2bus_if

    