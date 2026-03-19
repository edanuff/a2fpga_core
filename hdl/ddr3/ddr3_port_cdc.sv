//
// CDC wrapper for mem_port_if — bridges async client clock to DDR3 controller clock
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
// Clock domain crossing between a client-clock mem_port_if and a DDR3-clock
// arbiter. Designed for truly asynchronous clocks (e.g. 54 MHz ↔ 74.25 MHz)
// where CLKDIV2 alignment cannot be assumed.
//
// Request path (client → DDR3): toggle handshake. Client captures addr/data/
// byte_en/wr/burst in local registers and toggles req_toggle. Data is stable
// by the time the toggle crosses (2+ clk_ddr cycles). Arbiter acknowledges
// via req_done, which propagates back to re-assert available.
//
// Response path (DDR3 → client): gray-code FIFO. Arbiter pushes resp_data
// on each resp_valid pulse. Handles burst reads (up to 4 beats). Gray-code
// pointers with 2FF synchronization ensure robust CDC regardless of routing.
//
// One outstanding request per port. Available de-asserts on wr|rd pulse,
// re-asserts after req_done propagates back (~4-6 client cycles).
//

module ddr3_port_cdc #(
    parameter PORT_ADDR_WIDTH = 21,
    parameter DATA_WIDTH = 32,
    parameter DQM_WIDTH = 4,
    parameter RESP_FIFO_ADDR_BITS = 3  // 2^3 = 8 entries
) (
    input  wire clk_client,          // Client clock domain (e.g. 54 MHz)
    input  wire clk_ddr,             // DDR3 controller clock (e.g. 74.25 MHz)
    input  wire rst,                 // Active-high reset (clk_ddr domain)

    // Client-facing port (clk_client domain)
    mem_port_if.controller client,

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
    input  wire                        init_complete // DDR3 calibration done
);

    localparam FIFO_DEPTH = 2 ** RESP_FIFO_ADDR_BITS;
    localparam PTR_WIDTH = RESP_FIFO_ADDR_BITS + 1;  // Extra bit for wrap detection

    // =========================================================================
    // Reset synchronization: rst (clk_ddr) → clk_client
    // =========================================================================
    (* syn_preserve=1 *) reg [1:0] rst_client_sync;
    wire rst_client = rst_client_sync[1];
    always @(posedge clk_client or posedge rst) begin
        if (rst)
            rst_client_sync <= 2'b11;
        else
            rst_client_sync <= {rst_client_sync[0], 1'b0};
    end

    // =========================================================================
    // Request path: clk_client → clk_ddr (toggle handshake)
    // =========================================================================

    // Client domain: capture request data and toggle
    reg                        req_toggle_c;
    reg [PORT_ADDR_WIDTH-1:0]  req_addr_c;
    reg [DATA_WIDTH-1:0]       req_data_c;
    reg [DQM_WIDTH-1:0]        req_byte_en_c;
    reg                        req_wr_c;
    reg                        req_burst_c;

    // Sync init_complete to client domain
    (* syn_preserve=1 *) reg [1:0] init_sync;
    always @(posedge clk_client or posedge rst_client) begin
        if (rst_client)
            init_sync <= 2'b0;
        else
            init_sync <= {init_sync[0], init_complete};
    end

    // Sync req_toggle_prev back to client domain for available detection
    reg                        req_toggle_prev;  // clk_ddr domain (declared here, driven below)
    (* syn_preserve=1 *) reg [1:0] done_sync;
    wire client_idle = (done_sync[1] == req_toggle_c);

    assign client.available = client_idle & init_sync[1];

    always @(posedge clk_client or posedge rst_client) begin
        if (rst_client) begin
            req_toggle_c  <= 1'b0;
            req_addr_c    <= '0;
            req_data_c    <= '0;
            req_byte_en_c <= '0;
            req_wr_c      <= 1'b0;
            req_burst_c   <= 1'b0;
            done_sync     <= 2'b0;
        end else begin
            done_sync <= {done_sync[0], req_toggle_prev};

            if ((client.wr || client.rd) && client_idle) begin
                req_addr_c    <= client.addr;
                req_data_c    <= client.data;
                req_byte_en_c <= client.byte_en;
                req_wr_c      <= client.wr;
                req_burst_c   <= client.burst;
                req_toggle_c  <= ~req_toggle_c;
            end
        end
    end

    // DDR3 domain: sync toggle, detect pending request
    (* syn_preserve=1 *) reg [1:0] req_toggle_sync;

    always @(posedge clk_ddr or posedge rst) begin
        if (rst) begin
            req_toggle_sync <= 2'b0;
            req_toggle_prev <= 1'b0;
        end else begin
            req_toggle_sync <= {req_toggle_sync[0], req_toggle_c};
            if (req_done)
                req_toggle_prev <= req_toggle_sync[1];
        end
    end

    assign req_pending = (req_toggle_sync[1] != req_toggle_prev);

    // Request data: read directly from client domain registers.
    // Data was captured on the same clk_client edge as req_toggle_c changed.
    // By the time the 2FF synchronizer detects the toggle (2+ clk_ddr cycles
    // later), data has been stable for well over setup time requirements.
    assign req_addr    = req_addr_c;
    assign req_data    = req_data_c;
    assign req_byte_en = req_byte_en_c;
    assign req_wr      = req_wr_c;
    assign req_burst   = req_burst_c;

    // =========================================================================
    // Response path: clk_ddr → clk_client (gray-code FIFO)
    // =========================================================================
    // Arbiter pushes resp_data on each resp_valid pulse (clk_ddr).
    // Client pops entries when FIFO is non-empty (clk_client).

    reg [DATA_WIDTH-1:0] fifo_mem [0:FIFO_DEPTH-1];

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
    always @(posedge clk_client or posedge rst_client) begin
        if (rst_client) begin
            wr_gray_sync1 <= '0;
            wr_gray_sync2 <= '0;
        end else begin
            wr_gray_sync1 <= wr_ptr_gray;
            wr_gray_sync2 <= wr_gray_sync1;
        end
    end

    wire fifo_empty = (wr_gray_sync2 == rd_ptr_gray);

    // Pop and deliver to client
    reg                   client_ready_r;
    reg [DATA_WIDTH-1:0]  client_q_r;

    always @(posedge clk_client or posedge rst_client) begin
        if (rst_client) begin
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

    assign client.q     = client_q_r;
    assign client.ready = client_ready_r;

endmodule
