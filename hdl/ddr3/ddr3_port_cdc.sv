//
// Synchronous CDC wrapper — bridges 54 MHz client clock to 108 MHz DDR3
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
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
// Clock domain crossing between a 54 MHz client port and a 108 MHz DDR3
// arbiter. Designed for synchronous clocks where 54 MHz is derived from
// 108 MHz via CLKDIV2, guaranteeing every 54 MHz rising edge coincides
// with a 108 MHz rising edge.
//
// Modeled after hdl/sdram/mem_port_cdc.sv (proven working on GS board).
//
// Request path (54 MHz → 108 MHz): registered in 108 MHz domain. All
// request signals are captured on the same 108 MHz edge, guaranteeing
// consistency regardless of routing delays or CLKDIV2 phase. Registers
// freeze while a request is pending to keep the arbiter's address
// pipeline stable. Edge detection on wr/rd prevents re-trigger when
// the freeze lifts.
//
// Available path (108 MHz → 54 MHz): registered in 54 MHz domain.
//
// Response path (108 MHz → 54 MHz): gray-code FIFO (same as
// mem_port_cdc.sv). The ready signal is a 1-cycle pulse at 108 MHz
// that would be missed by 54 MHz sampling. Burst reads produce up to
// 4 consecutive ready pulses needing buffering.
//
// IMPORTANT: All client-facing signals use explicit wire ports (not
// SystemVerilog interfaces). Gowin's optimizer reverses interface array
// indices during flattening in generate loops, causing CDC instances
// to connect to wrong-port signals. Explicit wires avoid this bug.
//

module ddr3_port_cdc #(
    parameter PORT_ADDR_WIDTH = 21,
    parameter DATA_WIDTH = 32,
    parameter DQM_WIDTH = 4,
    parameter RESP_FIFO_ADDR_BITS = 3  // 2^3 = 8 entries
) (
    input  wire clk_client,          // 54 MHz client clock (from CLKDIV2)
    input  wire clk_ddr,             // 108 MHz DDR3 controller clock
    input  wire rst,                 // Active-high reset (clk_ddr domain)

    // Client-facing port (clk_client domain) — explicit wires to avoid
    // Gowin interface array flattening bug
    input  wire                        client_rd,
    input  wire                        client_wr,
    input  wire [PORT_ADDR_WIDTH-1:0]  client_addr,
    input  wire [DATA_WIDTH-1:0]       client_data,
    input  wire [DQM_WIDTH-1:0]        client_byte_en,
    input  wire                        client_burst,
    output wire                        client_available,
    output wire                        client_ready,
    output wire [DATA_WIDTH-1:0]       client_q,

    // Arbiter-facing request (clk_ddr domain)
    output wire                        req_pending,  // Request waiting for service
    output wire [PORT_ADDR_WIDTH-1:0]  req_addr,     // Stable while req_pending
    output wire [DATA_WIDTH-1:0]       req_data,     // Stable while req_pending
    output wire [DQM_WIDTH-1:0]        req_byte_en,  // Stable while req_pending
    output wire                        req_wr,       // 1=write, 0=read
    output wire                        req_burst,    // Burst read request
    input  wire                        req_done,     // Pulse: full transaction complete

    // Arbiter-facing response (clk_ddr domain)
    input  wire                        resp_valid,   // Pulse: response beat available
    input  wire [DATA_WIDTH-1:0]       resp_data,    // Response data word

    // Status (clk_ddr domain)
    input  wire                        init_complete  // DDR3 calibration done
);

    localparam FIFO_DEPTH = 2 ** RESP_FIFO_ADDR_BITS;
    localparam PTR_WIDTH = RESP_FIFO_ADDR_BITS + 1;  // Extra bit for wrap detection

    // =========================================================================
    // Request path: 54 MHz → 108 MHz (synchronous registered crossing)
    // =========================================================================
    // All request signals are registered in the 108 MHz domain to guarantee
    // they are captured on the same clock edge. With CLKDIV2 alignment,
    // 54 MHz signals are stable for 2 full 108 MHz cycles. The register
    // captures them within that window. Edge detection on wr/rd latches
    // pending and clears on req_done.

    reg [PORT_ADDR_WIDTH-1:0] req_addr_r;
    reg [DATA_WIDTH-1:0]      req_data_r;
    reg [DQM_WIDTH-1:0]       req_byte_en_r;
    reg                       req_wr_r;
    reg                       req_rd_r;
    reg                       req_burst_r;

    // Freeze request registers while pending — keeps pipeline inputs stable
    // so mapped_addr and port_ddr_addr hold the correct address until the
    // arbiter consumes them. Without freeze, the client can change addr on
    // the next 54 MHz cycle, corrupting the pipeline before the arbiter acts.
    always @(posedge clk_ddr or posedge rst) begin
        if (rst) begin
            req_addr_r    <= '0;
            req_data_r    <= '0;
            req_byte_en_r <= '0;
            req_wr_r      <= 1'b0;
            req_rd_r      <= 1'b0;
            req_burst_r   <= 1'b0;
        end else if (!pending_r) begin
            req_addr_r    <= client_addr;
            req_data_r    <= client_data;
            req_byte_en_r <= client_byte_en;
            req_wr_r      <= client_wr;
            req_rd_r      <= client_rd;
            req_burst_r   <= client_burst;
        end
    end

    assign req_addr    = req_addr_r;
    assign req_data    = req_data_r;
    assign req_byte_en = req_byte_en_r;
    assign req_wr      = req_wr_r;
    assign req_burst   = req_burst_r;

    // Edge detection: trigger pending on rising edge of wr/rd only.
    // Without this, when req_done clears pending_r and the freeze lifts,
    // the still-frozen req_wr_r (=1) would immediately re-trigger pending.
    reg prev_req_active;
    always @(posedge clk_ddr or posedge rst) begin
        if (rst)
            prev_req_active <= 1'b0;
        else
            prev_req_active <= req_wr_r || req_rd_r;
    end
    wire req_edge = (req_wr_r || req_rd_r) && !prev_req_active;

    // Pending flag: set on rising edge of wr/rd, clear on req_done.
    // No delay chain — the arbiter's S_LOAD state provides the 1-cycle
    // pipeline alignment (mapped_addr → port_ddr_addr settles during S_LOAD).
    reg pending_r;
    always @(posedge clk_ddr or posedge rst) begin
        if (rst)
            pending_r <= 1'b0;
        else if (req_done)
            pending_r <= 1'b0;
        else if (req_edge && !pending_r)
            pending_r <= 1'b1;
    end
    assign req_pending = pending_r;

    // =========================================================================
    // Available path: 108 MHz → 54 MHz (registered)
    // =========================================================================
    // Registered in the 54 MHz domain to prevent combinational consumers
    // from seeing transient 108 MHz transitions between 54 MHz clock edges.

    reg client_available_r;
    always @(posedge clk_client or posedge rst) begin
        if (rst)
            client_available_r <= 1'b0;
        else
            client_available_r <= init_complete && !pending_r;
    end
    assign client_available = client_available_r;

    // =========================================================================
    // Response path: 108 MHz → 54 MHz (gray-code FIFO)
    // =========================================================================
    // Arbiter pushes resp_data on each resp_valid pulse (clk_ddr).
    // Client pops entries when FIFO is non-empty (clk_client).
    //
    // Gray-code pointers with 2FF synchronization:
    //   - Only one pointer bit changes per increment → no transient glitches
    //   - 2FF synchronizer handles metastability on the single changing bit
    //   - Empty detection works directly on gray-coded values
    //   - Robust regardless of routing delays (proven on GS board)
    //
    // No full check needed: depth 8 with burst-4 max gives ample headroom.

    (* syn_ramstyle="registers" *) reg [DATA_WIDTH-1:0] fifo_mem [0:FIFO_DEPTH-1];

    // --- Write side (clk_ddr domain) ---
    reg [PTR_WIDTH-1:0] fifo_wr_ptr;
    wire [PTR_WIDTH-1:0] wr_ptr_gray = fifo_wr_ptr ^ (fifo_wr_ptr >> 1);

    always @(posedge clk_ddr or posedge rst) begin
        if (rst) begin
            fifo_wr_ptr <= '0;
        end else if (resp_valid) begin
            fifo_mem[fifo_wr_ptr[RESP_FIFO_ADDR_BITS-1:0]] <= resp_data;
            fifo_wr_ptr <= fifo_wr_ptr + 1'd1;
        end
    end

    // --- Read side (clk_client domain) ---
    reg [PTR_WIDTH-1:0] fifo_rd_ptr;
    wire [PTR_WIDTH-1:0] rd_ptr_gray = fifo_rd_ptr ^ (fifo_rd_ptr >> 1);

    // Sync write pointer gray code to client domain
    (* syn_preserve=1 *) reg [PTR_WIDTH-1:0] wr_gray_sync1, wr_gray_sync2;
    always @(posedge clk_client or posedge rst) begin
        if (rst) begin
            wr_gray_sync1 <= '0;
            wr_gray_sync2 <= '0;
        end else begin
            wr_gray_sync1 <= wr_ptr_gray;
            wr_gray_sync2 <= wr_gray_sync1;
        end
    end

    wire fifo_empty = (wr_gray_sync2 == rd_ptr_gray);

    // Pop and deliver to client — auto-pop whenever FIFO is non-empty.
    reg                   client_ready_r;
    reg [DATA_WIDTH-1:0]  client_q_r;

    always @(posedge clk_client or posedge rst) begin
        if (rst) begin
            fifo_rd_ptr    <= '0;
            client_ready_r <= 1'b0;
            client_q_r     <= '0;
        end else if (!fifo_empty) begin
            client_q_r     <= fifo_mem[fifo_rd_ptr[RESP_FIFO_ADDR_BITS-1:0]];
            fifo_rd_ptr    <= fifo_rd_ptr + 1'd1;
            client_ready_r <= 1'b1;
        end else begin
            client_ready_r <= 1'b0;
        end
    end

    assign client_q     = client_q_r;
    assign client_ready = client_ready_r;

endmodule
