//
// Multi-port DDR3 arbiter with CDC
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
// Accepts an array of mem_port_if clients running at clk_client and multiplexes
// them onto a single Gowin DDR3 Memory Interface IP running at clk_ddr.
//
// Architecture:
//   - Per-port CDC via ddr3_port_cdc (toggle request, gray-code response FIFO)
//   - Static priority arbiter (port 0 = highest) scans for pending requests
//   - Width conversion: 128-bit DDR3 ↔ 32-bit clients
//   - Writes use wr_data_mask to write 4 bytes within 128-bit word (no RMW)
//   - Burst reads decompose 128-bit response into 4 × 32-bit beats
//   - Non-burst reads extract the addressed 32-bit slot from 128-bit response
//   - Per-port 128-bit read cache: hits skip DDR3 entirely (~75% hit rate
//     for sequential access patterns). Invalidated on writes to same burst addr.
//
// DDR3 IP interface follows Gowin DDR3 Memory Interface conventions:
//   - cmd[2:0]: 000=write, 001=read
//   - addr[27:0]: byte address, 16-byte aligned
//   - wr_data[127:0] + wr_data_mask[15:0]: masked write (1=don't write)
//   - rd_data[127:0] + rd_data_valid: read response
//
// Client addressing: PORT_ADDR_WIDTH-bit word address (32-bit words).
//   - addr[1:0] selects 32-bit slot within 128-bit DDR3 burst
//   - addr[PORT_ADDR_WIDTH-1:2] selects 128-bit burst
//   - Clients are responsible for applying any base address offset
//

module ddr3_ports #(
    parameter NUM_PORTS = 4,
    parameter PORT_ADDR_WIDTH = 21,
    parameter DATA_WIDTH = 32,
    parameter DQM_WIDTH = 4,
    parameter DDR_ADDR_WIDTH = 28,
    parameter DDR_DATA_WIDTH = 128,
    parameter DDR_MASK_WIDTH = 16
) (
    input  wire clk_client,          // Client clock (e.g. 54 MHz)
    input  wire clk_ddr,             // DDR3 controller clock (e.g. 74.25 MHz)
    input  wire rst,                 // Active-high reset (clk_ddr domain, from DDR3 IP)
    input  wire init_complete,       // DDR3 calibration complete (clk_ddr domain)

    // Client ports (clk_client domain)
    mem_port_if.controller ports [NUM_PORTS],

    // DDR3 IP command interface (clk_ddr domain)
    input  wire                        cmd_ready,
    output reg  [2:0]                  cmd,
    output reg                         cmd_en,
    output reg  [DDR_ADDR_WIDTH-1:0]   addr,

    // DDR3 IP write interface (clk_ddr domain)
    input  wire                        wr_data_rdy,
    output reg  [DDR_DATA_WIDTH-1:0]   wr_data,
    output reg                         wr_data_en,
    output wire                        wr_data_end,
    output reg  [DDR_MASK_WIDTH-1:0]   wr_data_mask,

    // DDR3 IP read interface (clk_ddr domain)
    input  wire                        rd_data_valid,
    input  wire [DDR_DATA_WIDTH-1:0]   rd_data,
    input  wire                        rd_data_end
);

    assign wr_data_end = 1'b1;  // Single-beat writes (BL8, one 128-bit word)

    localparam CMD_WRITE = 3'b000;
    localparam CMD_READ  = 3'b001;

    localparam PORT_IDX_WIDTH = (NUM_PORTS > 1) ? $clog2(NUM_PORTS) : 1;

    // =========================================================================
    // Per-port CDC instances
    // =========================================================================

    wire [NUM_PORTS-1:0]          cdc_req_pending;
    wire [PORT_ADDR_WIDTH-1:0]    cdc_req_addr    [NUM_PORTS];
    wire [DATA_WIDTH-1:0]         cdc_req_data    [NUM_PORTS];
    wire [DQM_WIDTH-1:0]          cdc_req_byte_en [NUM_PORTS];
    wire [NUM_PORTS-1:0]          cdc_req_wr;
    wire [NUM_PORTS-1:0]          cdc_req_burst;
    reg  [NUM_PORTS-1:0]          cdc_req_done;
    reg  [NUM_PORTS-1:0]          cdc_resp_valid;
    reg  [DATA_WIDTH-1:0]         cdc_resp_data;    // Shared bus

    generate
        for (genvar gi = 0; gi < NUM_PORTS; gi++) begin : gen_cdc
            ddr3_port_cdc #(
                .PORT_ADDR_WIDTH(PORT_ADDR_WIDTH),
                .DATA_WIDTH(DATA_WIDTH),
                .DQM_WIDTH(DQM_WIDTH)
            ) u_cdc (
                .clk_client    (clk_client),
                .clk_ddr       (clk_ddr),
                .rst           (rst),
                .client        (ports[gi]),
                .req_pending   (cdc_req_pending[gi]),
                .req_addr      (cdc_req_addr[gi]),
                .req_data      (cdc_req_data[gi]),
                .req_byte_en   (cdc_req_byte_en[gi]),
                .req_wr        (cdc_req_wr[gi]),
                .req_burst     (cdc_req_burst[gi]),
                .req_done      (cdc_req_done[gi]),
                .resp_valid    (cdc_resp_valid[gi]),
                .resp_data     (cdc_resp_data),
                .init_complete (init_complete)
            );
        end
    endgenerate

    // =========================================================================
    // Per-port read cache (clk_ddr domain)
    // =========================================================================
    // One 128-bit cache line per port, tagged by burst-aligned address.
    // Serves ~75% of sequential reads from cache (4 words per line).
    // Invalidated when any write targets the same burst address.

    localparam BURST_TAG_WIDTH = PORT_ADDR_WIDTH - 2;  // addr[PORT_ADDR_WIDTH-1:2]

    reg [DDR_DATA_WIDTH-1:0]   cache_data  [NUM_PORTS];
    reg [BURST_TAG_WIDTH-1:0]  cache_tag   [NUM_PORTS];
    reg [NUM_PORTS-1:0]        cache_valid;

    // =========================================================================
    // Arbiter state machine (clk_ddr domain)
    // =========================================================================

    localparam S_IDLE      = 3'd0;
    localparam S_WRITE     = 3'd1;
    localparam S_READ_CMD  = 3'd2;
    localparam S_READ_WAIT = 3'd3;
    localparam S_RESPOND   = 3'd4;
    localparam S_DONE      = 3'd5;

    reg [2:0] state;
    reg [PORT_IDX_WIDTH-1:0] active_port;
    reg                      active_wr;
    reg                      active_burst;
    reg [1:0]                active_slot;
    reg [DDR_ADDR_WIDTH-1:0] active_ddr_addr;
    reg [DDR_DATA_WIDTH-1:0] rd_data_latched;
    reg [1:0]                beat_cnt;
    reg                      cache_hit;

    // Compute DDR3 address from client word address.
    // Client addr is PORT_ADDR_WIDTH bits of 32-bit word addressing.
    // DDR3 addr is DDR_ADDR_WIDTH bits of byte addressing, 16-byte aligned.
    // addr[1:0] = slot within 128-bit word (not sent to DDR3).
    // addr[PORT_ADDR_WIDTH-1:2] = burst address, shifted left by 4.
    localparam BURST_ADDR_WIDTH = PORT_ADDR_WIDTH - 2;
    localparam PAD_WIDTH = DDR_ADDR_WIDTH - BURST_ADDR_WIDTH - 4;

    function [DDR_ADDR_WIDTH-1:0] word_to_ddr_addr;
        input [PORT_ADDR_WIDTH-1:0] word_addr;
        begin
            word_to_ddr_addr = {{PAD_WIDTH{1'b0}}, word_addr[PORT_ADDR_WIDTH-1:2], 4'b0};
        end
    endfunction

    // Write mask: mask all 16 bytes, then unmask the 4 bytes at the target slot,
    // gated by client byte_en. wr_data_mask is active-high (1 = don't write).
    function [DDR_MASK_WIDTH-1:0] compute_write_mask;
        input [1:0]            slot;
        input [DQM_WIDTH-1:0]  byte_en;
        reg [DDR_MASK_WIDTH-1:0] mask;
        begin
            mask = {DDR_MASK_WIDTH{1'b1}};
            case (slot)
                2'd0: mask[3:0]   = ~byte_en;
                2'd1: mask[7:4]   = ~byte_en;
                2'd2: mask[11:8]  = ~byte_en;
                2'd3: mask[15:12] = ~byte_en;
            endcase
            compute_write_mask = mask;
        end
    endfunction

    // Extract 32-bit slot from 128-bit DDR3 read data
    function [DATA_WIDTH-1:0] extract_slot;
        input [DDR_DATA_WIDTH-1:0] ddr_data;
        input [1:0]                slot;
        begin
            case (slot)
                2'd0: extract_slot = ddr_data[31:0];
                2'd1: extract_slot = ddr_data[63:32];
                2'd2: extract_slot = ddr_data[95:64];
                2'd3: extract_slot = ddr_data[127:96];
            endcase
        end
    endfunction

    always @(posedge clk_ddr or posedge rst) begin
        if (rst) begin
            state          <= S_IDLE;
            cmd_en         <= 1'b0;
            cmd            <= CMD_READ;
            addr           <= '0;
            wr_data_en     <= 1'b0;
            wr_data        <= '0;
            wr_data_mask   <= {DDR_MASK_WIDTH{1'b1}};
            cdc_req_done   <= '0;
            cdc_resp_valid <= '0;
            cdc_resp_data  <= '0;
            active_port    <= '0;
            active_wr      <= 1'b0;
            active_burst   <= 1'b0;
            active_slot    <= 2'b0;
            active_ddr_addr <= '0;
            rd_data_latched <= '0;
            beat_cnt       <= 2'b0;
            cache_valid    <= '0;
        end else begin
            // Default: deassert one-cycle pulses
            cmd_en         <= 1'b0;
            wr_data_en     <= 1'b0;
            cdc_req_done   <= '0;
            cdc_resp_valid <= '0;

            case (state)
                S_IDLE: begin
                    if (init_complete) begin : pick_port
                        integer i;
                        for (i = 0; i < NUM_PORTS; i = i + 1) begin
                            if (cdc_req_pending[i]) begin
                                active_port     <= i[PORT_IDX_WIDTH-1:0];
                                active_wr       <= cdc_req_wr[i];
                                active_burst    <= cdc_req_burst[i];
                                active_slot     <= cdc_req_addr[i][1:0];
                                active_ddr_addr <= word_to_ddr_addr(cdc_req_addr[i]);
                                if (cdc_req_wr[i]) begin
                                    state <= S_WRITE;
                                end else if (cache_valid[i] &&
                                             cache_tag[i] == cdc_req_addr[i][PORT_ADDR_WIDTH-1:2]) begin
                                    // Cache hit: load cached data, skip DDR3
                                    rd_data_latched <= cache_data[i];
                                    beat_cnt        <= cdc_req_burst[i] ? 2'd0 : cdc_req_addr[i][1:0];
                                    cache_hit       <= 1'b1;
                                    state           <= S_RESPOND;
                                end else begin
                                    cache_hit <= 1'b0;
                                    state     <= S_READ_CMD;
                                end
                                disable pick_port;
                            end
                        end
                    end
                end

                S_WRITE: begin
                    if (cmd_ready && wr_data_rdy) begin
                        cmd_en       <= 1'b1;
                        cmd          <= CMD_WRITE;
                        addr         <= active_ddr_addr;
                        wr_data_en   <= 1'b1;
                        wr_data      <= {4{cdc_req_data[active_port]}};
                        wr_data_mask <= compute_write_mask(active_slot,
                                                          cdc_req_byte_en[active_port]);
                        // Invalidate any cache line matching this burst address
                        begin : invalidate_caches
                            integer j;
                            for (j = 0; j < NUM_PORTS; j = j + 1) begin
                                if (cache_valid[j] &&
                                    cache_tag[j] == cdc_req_addr[active_port][PORT_ADDR_WIDTH-1:2])
                                    cache_valid[j] <= 1'b0;
                            end
                        end
                        state <= S_DONE;
                    end
                end

                S_READ_CMD: begin
                    if (cmd_ready) begin
                        cmd_en <= 1'b1;
                        cmd    <= CMD_READ;
                        addr   <= active_ddr_addr;
                        state  <= S_READ_WAIT;
                    end
                end

                S_READ_WAIT: begin
                    if (rd_data_valid) begin
                        rd_data_latched <= rd_data;
                        beat_cnt        <= active_burst ? 2'd0 : active_slot;
                        // Fill cache for this port
                        cache_data[active_port]  <= rd_data;
                        cache_tag[active_port]   <= cdc_req_addr[active_port][PORT_ADDR_WIDTH-1:2];
                        cache_valid[active_port] <= 1'b1;
                        state <= S_RESPOND;
                    end
                end

                S_RESPOND: begin
                    cdc_resp_valid[active_port] <= 1'b1;
                    cdc_resp_data <= extract_slot(rd_data_latched, beat_cnt);

                    if (active_burst) begin
                        beat_cnt <= beat_cnt + 2'd1;
                        if (beat_cnt == 2'd3)
                            state <= S_DONE;
                    end else begin
                        state <= S_DONE;
                    end
                end

                S_DONE: begin
                    cdc_req_done[active_port] <= 1'b1;
                    state <= S_IDLE;
                end

                default: state <= S_IDLE;
            endcase
        end
    end

endmodule
