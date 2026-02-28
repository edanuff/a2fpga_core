//
// CDC wrapper for mem_port_if — bridges 54 MHz clients to 108 MHz SDRAM
//
// (c) 2025 Ed Anuff <ed@a2fpga.com>
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
// Clock domain crossing wrapper between a 54 MHz mem_port_if client and a
// 108 MHz mem_port_if controller (SDRAM). Designed for use with Gowin CLKDIV2
// where 54 MHz is derived from 108 MHz, guaranteeing that every 54 MHz rising
// edge coincides with a 108 MHz rising edge.
//
// Request path (54 MHz → 108 MHz): pass-through. addr/data/byte_en/burst are
// stable during wr/rd assertion. A 54 MHz wr/rd pulse spans 2 cycles at
// 108 MHz; the SDRAM controller's edge detection triggers exactly once.
//
// Available path (108 MHz → 54 MHz): registered in 54 MHz domain. Prevents
// combinational consumers from seeing transient 108 MHz transitions.
//
// Response path (108 MHz → 54 MHz): 4-entry FIFO. The ready signal is a
// 1-cycle pulse at 108 MHz that would be missed by 54 MHz sampling.
// Burst-2 reads produce 2 consecutive ready pulses requiring buffering.
// Binary pointers are safe (no gray-code) because CLKDIV2 alignment
// ensures multi-bit pointer reads are always stable across domains.
//

module mem_port_cdc #(
    parameter PORT_ADDR_WIDTH = 21,
    parameter DATA_WIDTH = 32,
    parameter DQM_WIDTH = 4,
    parameter PORT_OUTPUT_WIDTH = 32
) (
    input  wire clk_client,    // 54 MHz client clock (from CLKDIV2)
    input  wire clk_sdram,     // 108 MHz SDRAM clock
    input  wire rst_n,

    // Client-facing port (54 MHz consumers connect here)
    mem_port_if.controller client,

    // SDRAM-facing port (connects to sdram_ports)
    mem_port_if.client sdram
);

    // =========================================================================
    // Request path: 54 MHz → 108 MHz (pass-through)
    // =========================================================================
    // addr, data, byte_en, burst are stable while wr/rd is asserted.
    // A 54 MHz wr/rd pulse appears as 2 cycles at 108 MHz. The SDRAM
    // controller (sdram.sv) edge-detects (port_wr && !port_wr_prev),
    // so the 2-cycle pulse triggers exactly one operation.

    assign sdram.addr    = client.addr;
    assign sdram.data    = client.data;
    assign sdram.byte_en = client.byte_en;
    assign sdram.wr      = client.wr;
    assign sdram.rd      = client.rd;
    assign sdram.burst   = client.burst;

    // =========================================================================
    // Available path: 108 MHz → 54 MHz (registered)
    // =========================================================================
    // Registered in the 54 MHz domain to prevent combinational consumers
    // (e.g. sdram_framebuffer's fifo_pop_w = !fifo_empty && available) from
    // seeing transient 108 MHz transitions between 54 MHz clock edges.
    // Without this, a write can reach the SDRAM (via combinational wr
    // pass-through) while the 54 MHz FIFO pointer never advances.

    reg client_available_r;
    always @(posedge clk_client or negedge rst_n) begin
        if (!rst_n)
            client_available_r <= 1'b0;
        else
            client_available_r <= sdram.available;
    end
    assign client.available = client_available_r;

    // =========================================================================
    // Response path: 108 MHz → 54 MHz (4-entry FIFO)
    // =========================================================================
    // ready is a 1-cycle pulse at 108 MHz — could be missed by 54 MHz.
    // Burst-2 reads produce 2 consecutive ready pulses needing buffering.
    // FIFO stores q on each 108 MHz ready pulse, delivers at 54 MHz.
    //
    // Binary pointers are safe (no gray-code needed) because CLKDIV2
    // guarantees the 54 MHz read pointer changes only on 108 MHz edges,
    // so the write side always sees a stable read pointer value.
    // 4 entries: max 2 in-flight from burst + margin.

    localparam FIFO_DEPTH = 4;
    localparam FIFO_ADDR_BITS = 2;

    reg [PORT_OUTPUT_WIDTH-1:0] fifo_mem [0:FIFO_DEPTH-1];

    // Write pointer (108 MHz domain)
    reg [FIFO_ADDR_BITS:0] fifo_wr_ptr;

    // Read pointer (54 MHz domain)
    reg [FIFO_ADDR_BITS:0] fifo_rd_ptr;

    wire fifo_empty = (fifo_wr_ptr == fifo_rd_ptr);

    // Write side: push on each 108 MHz ready pulse
    always @(posedge clk_sdram or negedge rst_n) begin
        if (!rst_n) begin
            fifo_wr_ptr <= '0;
        end else if (sdram.ready) begin
            fifo_mem[fifo_wr_ptr[FIFO_ADDR_BITS-1:0]] <= sdram.q;
            fifo_wr_ptr <= fifo_wr_ptr + 1;
        end
    end

    // Read side: pop and deliver at 54 MHz
    reg                          client_ready_r;
    reg [PORT_OUTPUT_WIDTH-1:0]  client_q_r;

    always @(posedge clk_client or negedge rst_n) begin
        if (!rst_n) begin
            fifo_rd_ptr    <= '0;
            client_ready_r <= 1'b0;
            client_q_r     <= '0;
        end else if (!fifo_empty) begin
            client_q_r     <= fifo_mem[fifo_rd_ptr[FIFO_ADDR_BITS-1:0]];
            fifo_rd_ptr    <= fifo_rd_ptr + 1;
            client_ready_r <= 1'b1;
        end else begin
            client_ready_r <= 1'b0;
        end
    end

    assign client.q     = client_q_r;
    assign client.ready = client_ready_r;

endmodule
