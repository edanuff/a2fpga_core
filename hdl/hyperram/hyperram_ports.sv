//
// Multi-port HyperRAM arbiter (mem_port_if clients -> hyperram_ctrl)
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
// The HyperRAM counterpart of hdl/ddr3/ddr3_ports.sv: N clients multiplexed
// onto one hyperram_ctrl, with the same client contract as ddr3_ports so
// framebuffer_1080p, the apple_memory shadow ports and sound_glu / the DOC
// wavetable port drop in unchanged:
//
//   - A client fires a request by asserting rd or wr for one cycle while
//     available is high. Each port queues up to two requests (available
//     drops only when both slots are full), so a client may fire on
//     consecutive cycles exactly as it does against ddr3_port_cdc.
//   - ready pulses once per READ beat with the word on q. Writes complete
//     silently (ddr3_ports never pulses ready for a write either).
//   - rd with burst=0: one beat, the addressed word.
//     rd with burst=1: PORT_BURST_WORDS[port] beats from the address
//     rounded down to that burst (4 = the DDR3 128-bit word; 8 for the
//     framebuffer read port).
//   - wr on WIDE_WR_PORT: 4 consecutive words {wide_wr_data_hi, data} at
//     the address rounded down to 4, all bytes.
//     wr elsewhere: one word, byte_en honoured.
//   - Clients address 32-bit words from 0; PORT_BASE_ADDR is added here.
//
// This module takes the client signals as flat vectors (port p is field p of
// each vector). hyperram_ports_if.sv wraps it with a mem_port_if array for
// the board tops; the flat form is what the simulation harness drives
// (iverilog has no interface-array ports) and it also sidesteps the Gowin
// interface-array flattening bug noted in ddr3_port_cdc.sv.
//
// Same clock as the clients (hyperram_ctrl's user side is on clk), so no
// CDC and no settle states. Static priority, port 0 highest; a granted
// request runs to completion before the next pick.
//
`timescale 1ns / 1ps

module hyperram_ports #(
    parameter NUM_PORTS       = 4,
    parameter PORT_ADDR_WIDTH = 21,          // 32-bit words (8 MB)
    // Packed per-port tables, port 0 in the least significant field:
    //   PORT_BASE_ADDR   {base[N-1], ..., base[0]}  word addresses
    //   PORT_BURST_WORDS {len[N-1],  ..., len[0]}   1..16 each
    parameter [NUM_PORTS*PORT_ADDR_WIDTH-1:0] PORT_BASE_ADDR   = '0,
    parameter [NUM_PORTS*5-1:0]               PORT_BURST_WORDS = {NUM_PORTS{5'd4}},
    parameter integer WIDE_WR_PORT    = -1
) (
    input  wire clk,
    input  wire rst_n,

    // clients, flat (field p = port p)
    input  wire [NUM_PORTS-1:0]                 c_rd,
    input  wire [NUM_PORTS-1:0]                 c_wr,
    input  wire [NUM_PORTS-1:0]                 c_burst,
    input  wire [NUM_PORTS*PORT_ADDR_WIDTH-1:0] c_addr,
    input  wire [NUM_PORTS*32-1:0]              c_data,
    input  wire [NUM_PORTS*4-1:0]               c_be,
    input  wire [95:0]                          wide_wr_data_hi,   // WIDE_WR_PORT words 1..3
    output wire [NUM_PORTS-1:0]                 c_available,
    output reg  [NUM_PORTS-1:0]                 c_ready,
    output reg  [31:0]                          c_q,               // shared read-beat bus

    // hyperram_ctrl user interface
    input  wire                       hr_ready,
    input  wire                       hr_init_done,
    output reg                        hr_req,
    output reg                        hr_we,
    output reg  [PORT_ADDR_WIDTH-1:0] hr_addr,
    output reg  [4:0]                 hr_len,
    output reg  [31:0]                hr_wdata,
    output reg  [3:0]                 hr_wbe,
    output reg                        hr_wbuf_we,
    output reg  [3:0]                 hr_wbuf_idx,
    output reg  [31:0]                hr_wbuf_data,
    output reg  [3:0]                 hr_wbuf_be,
    input  wire                       hr_rvalid,
    input  wire [31:0]                hr_rdata,
    input  wire                       hr_done,

    output wire [NUM_PORTS-1:0]       dbg_req_pending,
    output wire [3:0]                 dbg_state
);
    localparam PORT_IDX_WIDTH = (NUM_PORTS > 1) ? $clog2(NUM_PORTS) : 1;
    localparam AW = PORT_ADDR_WIDTH;

    function automatic [AW-1:0] base_of(input integer p);
        base_of = PORT_BASE_ADDR[p*AW +: AW];
    endfunction
    function automatic [4:0] burst_of(input integer p);
        burst_of = PORT_BURST_WORDS[p*5 +: 5];
    endfunction

    // ---------------------------------------------------------------------
    // Per-port 2-entry request queue
    // ---------------------------------------------------------------------
    localparam REQ_W = 1 + AW + 32 + 4 + 1 + 96;   // {wr, addr, data, be, burst, wide_hi}

    reg  [REQ_W-1:0]        q_slot0 [NUM_PORTS];
    reg  [REQ_W-1:0]        q_slot1 [NUM_PORTS];
    reg  [1:0]              q_wr_ptr [NUM_PORTS];
    reg  [1:0]              q_rd_ptr [NUM_PORTS];
    wire [NUM_PORTS-1:0]    q_empty, q_full, fire;
    reg  [NUM_PORTS-1:0]    q_pop;

    generate
        for (genvar gi = 0; gi < NUM_PORTS; gi++) begin : gen_q
            assign q_empty[gi] = (q_wr_ptr[gi] == q_rd_ptr[gi]);
            assign q_full[gi]  = (q_wr_ptr[gi][0] == q_rd_ptr[gi][0]) &&
                                 (q_wr_ptr[gi][1] != q_rd_ptr[gi][1]);
            assign c_available[gi] = hr_init_done && !q_full[gi];
            assign fire[gi] = c_available[gi] && (c_rd[gi] || c_wr[gi]);

            wire [REQ_W-1:0] req_w = {c_wr[gi], c_addr[gi*AW +: AW], c_data[gi*32 +: 32],
                                      c_be[gi*4 +: 4], c_burst[gi],
                                      (gi == WIDE_WR_PORT) ? wide_wr_data_hi : 96'd0};
            always_ff @(posedge clk or negedge rst_n) begin
                if (!rst_n) begin
                    q_wr_ptr[gi] <= 2'd0;
                    q_rd_ptr[gi] <= 2'd0;
                end else begin
                    if (fire[gi]) begin
                        if (q_wr_ptr[gi][0]) q_slot1[gi] <= req_w;
                        else                 q_slot0[gi] <= req_w;
                        q_wr_ptr[gi] <= q_wr_ptr[gi] + 2'd1;
                    end
                    if (q_pop[gi])
                        q_rd_ptr[gi] <= q_rd_ptr[gi] + 2'd1;
                end
            end
        end
    endgenerate

    assign dbg_req_pending = ~q_empty;

    // ---------------------------------------------------------------------
    // Arbiter
    // ---------------------------------------------------------------------
    localparam S_IDLE  = 4'd0;
    localparam S_LOAD  = 4'd1;   // decode the head request
    localparam S_WBUF  = 4'd2;   // wide write: words 1..3 into the write buffer
    localparam S_ISSUE = 4'd3;   // req when the controller is ready
    localparam S_WAIT  = 4'd4;   // beats out, then done
    reg [3:0] state;
    assign dbg_state = state;

    reg [PORT_IDX_WIDTH-1:0] active;
    reg                      a_wr, a_burst;
    reg [AW-1:0]             a_addr;
    reg [31:0]               a_data;
    reg [3:0]                a_be;
    reg [95:0]               a_hi;
    reg [1:0]                wbuf_n;

    wire [REQ_W-1:0] h = q_rd_ptr[active][0] ? q_slot1[active] : q_slot0[active];
    wire             h_wr    = h[REQ_W-1];
    wire [AW-1:0]    h_addr  = h[REQ_W-2 -: AW];
    wire [31:0]      h_data  = h[96+1+4 +: 32];
    wire [3:0]       h_be    = h[96+1 +: 4];
    wire             h_burst = h[96];
    wire [95:0]      h_hi    = h[95:0];
    wire             a_wide  = (WIDE_WR_PORT >= 0) && (active == WIDE_WR_PORT[PORT_IDX_WIDTH-1:0]);

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state       <= S_IDLE;
            active      <= '0;
            hr_req      <= 1'b0;
            hr_we       <= 1'b0;
            hr_addr     <= '0;
            hr_len      <= 5'd1;
            hr_wdata    <= '0;
            hr_wbe      <= 4'hF;
            hr_wbuf_we  <= 1'b0;
            hr_wbuf_idx <= 4'd0;
            hr_wbuf_data<= '0;
            hr_wbuf_be  <= 4'hF;
            q_pop       <= '0;
            c_ready     <= '0;
            c_q         <= '0;
            a_wr <= 1'b0; a_burst <= 1'b0; a_addr <= '0; a_data <= '0; a_be <= '0; a_hi <= '0;
            wbuf_n      <= 2'd0;
        end else begin
            hr_req     <= 1'b0;
            hr_wbuf_we <= 1'b0;
            q_pop      <= '0;
            c_ready    <= '0;
            case (state)
                S_IDLE: begin : pick
                    // q_pop is still asserted in the first IDLE cycle after a
                    // transaction and its pointer update lands at the end of
                    // it: a port being popped must not be re-picked from its
                    // stale head (that re-issued finished requests).
                    integer i;
                    for (i = NUM_PORTS - 1; i >= 0; i = i - 1)
                        if (!q_empty[i] && !q_pop[i]) begin
                            active <= i[PORT_IDX_WIDTH-1:0];
                            state  <= S_LOAD;
                        end
                end
                S_LOAD: begin
                    a_wr    <= h_wr;
                    a_burst <= h_burst;
                    a_data  <= h_data;
                    a_be    <= h_be;
                    a_hi    <= h_hi;
                    if (h_wr && a_wide) begin
                        a_addr <= (base_of(active) + h_addr) & ~{{(AW-2){1'b0}}, 2'b11};
                        wbuf_n <= 2'd1;
                        state  <= S_WBUF;
                    end else if (!h_wr && h_burst) begin
                        a_addr <= (base_of(active) + h_addr) &
                                  ~({{(AW-5){1'b0}}, burst_of(active)} - {{(AW-1){1'b0}}, 1'b1});
                        state  <= S_ISSUE;
                    end else begin
                        a_addr <= base_of(active) + h_addr;
                        state  <= S_ISSUE;
                    end
                end
                S_WBUF: begin
                    // the controller accepts write-buffer loads while ready
                    if (hr_ready) begin
                        hr_wbuf_we   <= 1'b1;
                        hr_wbuf_idx  <= {2'b00, wbuf_n};
                        hr_wbuf_data <= a_hi[32*(wbuf_n-1) +: 32];
                        hr_wbuf_be   <= 4'hF;
                        if (wbuf_n == 2'd3) state <= S_ISSUE;
                        wbuf_n <= wbuf_n + 2'd1;
                    end
                end
                S_ISSUE: begin
                    if (hr_ready) begin
                        hr_req   <= 1'b1;
                        hr_we    <= a_wr;
                        hr_addr  <= a_addr;
                        hr_wdata <= a_data;
                        if (a_wr) begin
                            hr_len <= a_wide ? 5'd4 : 5'd1;
                            hr_wbe <= a_wide ? 4'hF : a_be;
                        end else begin
                            hr_len <= a_burst ? burst_of(active) : 5'd1;
                            hr_wbe <= 4'hF;
                        end
                        state <= S_WAIT;
                    end
                end
                S_WAIT: begin
                    if (hr_rvalid) begin
                        c_q             <= hr_rdata;
                        c_ready[active] <= 1'b1;
                    end
                    if (hr_done) begin
                        q_pop[active] <= 1'b1;
                        state <= S_IDLE;
                    end
                end
                default: state <= S_IDLE;
            endcase
        end
    end
endmodule
