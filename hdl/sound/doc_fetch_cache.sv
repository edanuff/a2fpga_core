// Word cache + request ordering for the baseline DOC on an external memory.
//
// The DOC fetches one wavetable BYTE per oscillator per slot. On a shared
// memory (HyperRAM / DDR3 behind a serialized arbiter) that is ~15% of the
// bus at full oscillator load and every fetch has a hard ~840 ns deadline.
// Sequential walks at <= 1 byte/sample touch the same 32-bit word up to four
// times, so a small word cache cuts the traffic ~4x and answers hits in two
// cycles. 64 entries, direct-mapped on a hash of the word address (addr[7:2]
// ^ addr[13:8], so oscillators parked at the same offset of different
// 256-byte tables do not evict each other), 8-bit tag addr[15:8] (the index
// hash is invertible given the tag, so the pair identifies the word).
//
// Ordering (formerly doc_fetch_filter): the DOC wants at most one response at
// a time. Any outstanding memory read is superseded by the next DOC request —
// its word is still written into the cache when it returns, but it is not
// delivered. Only requests the port accepted (rd && available) are tracked;
// a request issued into a full port queue produces no response and the DOC
// times out on its own. A hit is never answered while a memory read for the
// SAME request could still be pending (there is only ever one live request).
//
// Coherency: valid bits are cleared on every GLU sound-RAM write and again
// when that write is actually issued to memory (the write sits in a queue;
// a miss in between could cache the pre-write word).
//
// Storage: data 64 x 32 in one single-port BSRAM (SP x32 = one 18 Kb block on
// GW5A, measured); fills yield to a same-cycle hit read and retry next cycle.
module doc_fetch_cache (
    input  wire        clk_i,
    input  wire        rst_n_i,
    // DOC side
    input  wire        req_i,          // DOC wave_rd (one-cycle pulse)
    input  wire [15:0] addr_i,         // byte address
    input  wire        flush_i,        // GLU wavetable write (or its commit): invalidate all
    output reg         valid_o,        // byte for the DOC's current request
    output reg  [7:0]  data_o,
    // memory port (single-word reads, in-order responses)
    output wire        mem_rd_o,
    output wire [13:0] mem_addr_o,     // word address
    input  wire        mem_available_i,
    input  wire        mem_ready_i,
    input  wire [31:0] mem_q_i,
    // telemetry (wrap)
    output reg  [15:0] dbg_hits_o,
    output reg  [15:0] dbg_misses_o,
    output reg  [7:0]  dbg_late_o      // superseded responses discarded
);
    // ---- tags / valid (FF) ----
    wire [5:0] idx_w = addr_i[7:2] ^ addr_i[13:8];
    wire [7:0] tag_w = addr_i[15:8];
    reg  [7:0] tag_r   [0:63];
    reg  [63:0] valid_r;
    wire hit_w = valid_r[idx_w] && (tag_r[idx_w] == tag_w);

    // ---- data (one single-port BSRAM, registered read; see doc_cache_data_sp) ----
    // The port is shared: a hit read has priority, a fill that loses the cycle
    // is retried next cycle (reads come at most once per ~60 clk).
    wire [31:0] rd_word_w;
    reg         rd_pend_r;              // a hit read was issued last cycle
    reg  [1:0]  rd_lane_r;
    reg         fill_pend_r;
    reg  [5:0]  fill_idx_r;
    reg  [31:0] fill_word_r;
    wire        hit_rd_w  = req_i && hit_w;
    wire        resp_w    = mem_ready_i && !q_empty_w;
    wire        fill_now_w = !hit_rd_w && (resp_w || fill_pend_r);
    wire [5:0]  fill_addr_w = resp_w ? q_idx[q_rp[1:0]] : fill_idx_r;
    wire [31:0] fill_data_w = resp_w ? mem_q_i : fill_word_r;
    doc_cache_data_sp u_data (
        .clk_i(clk_i),
        .we_i(fill_now_w),
        .addr_i(fill_now_w ? fill_addr_w : idx_w),
        .din_i(fill_data_w),
        .dout_o(rd_word_w));

    // ---- outstanding memory reads: tiny FIFO of {idx, tag, lane, stale} ----
    // Depth 4 > the port's 2-slot queue + 1.
    reg  [5:0] q_idx  [0:3];
    reg  [7:0] q_tag  [0:3];
    reg  [1:0] q_lane [0:3];
    reg        q_stale[0:3];
    reg  [2:0] q_wp, q_rp;
    wire       q_empty_w = (q_wp == q_rp);
    wire       q_full_w  = (q_wp[1:0] == q_rp[1:0]) && (q_wp[2] != q_rp[2]);

    // live request = the newest accepted memory read, if not yet answered;
    // a hit is only taken when there is none (the DOC asks one at a time, so a
    // live request means the DOC's previous fetch is still in flight — which
    // this new request supersedes anyway).
    wire miss_issue_w = req_i && !hit_w && mem_available_i && !q_full_w;
    assign mem_rd_o   = miss_issue_w;
    assign mem_addr_o = addr_i[15:2];

    integer i;
    always_ff @(posedge clk_i) begin
        if (!rst_n_i) begin
            valid_r <= '0; valid_o <= 1'b0; data_o <= '0;
            rd_pend_r <= 1'b0; rd_lane_r <= '0; fill_pend_r <= 1'b0;
            q_wp <= '0; q_rp <= '0;
            dbg_hits_o <= '0; dbg_misses_o <= '0; dbg_late_o <= '0;
        end else begin
            valid_o <= 1'b0;

            // 1. memory response: fill, deliver if not superseded
            if (mem_ready_i && !q_empty_w) begin
                if (!q_stale[q_rp[1:0]] && !req_i) begin
                    valid_o <= 1'b1;
                    data_o  <= mem_q_i[8*q_lane[q_rp[1:0]] +: 8];
                end else begin
                    dbg_late_o <= dbg_late_o + 1'd1;   // superseded (or superseded this cycle)
                end
                // fill: the data port is busy if a hit read happens this cycle —
                // then the word is parked and written next cycle (valid is set
                // now; a hit on that index next cycle reads the parked write, so
                // mark valid only once the data is in the array)
                if (hit_rd_w) begin
                    fill_pend_r <= 1'b1; fill_idx_r <= q_idx[q_rp[1:0]]; fill_word_r <= mem_q_i;
                end else begin
                    valid_r[q_idx[q_rp[1:0]]] <= 1'b1;
                end
                tag_r[q_idx[q_rp[1:0]]] <= q_tag[q_rp[1:0]];
                q_rp <= q_rp + 3'd1;
            end else if (fill_pend_r && !hit_rd_w) begin
                valid_r[fill_idx_r] <= 1'b1;
                fill_pend_r <= 1'b0;
            end

            // 2. hit read completes (registered BSRAM read from last cycle)
            if (rd_pend_r) begin
                rd_pend_r <= 1'b0;
                valid_o   <= 1'b1;
                data_o    <= rd_word_w[8*rd_lane_r +: 8];
            end

            // 3. DOC request
            if (req_i) begin
                // everything still outstanding is now stale
                for (i = 0; i < 4; i = i + 1) q_stale[i] <= 1'b1;   // (slot pushed below is re-cleared)
                if (hit_w) begin
                    rd_pend_r <= 1'b1;                 // data arrives from u_data next cycle
                    rd_lane_r <= addr_i[1:0];
                    dbg_hits_o <= dbg_hits_o + 1'd1;
                end else if (miss_issue_w) begin
                    q_idx[q_wp[1:0]]   <= idx_w;
                    q_tag[q_wp[1:0]]   <= tag_w;
                    q_lane[q_wp[1:0]]  <= addr_i[1:0];
                    q_stale[q_wp[1:0]] <= 1'b0;
                    q_wp <= q_wp + 3'd1;
                    dbg_misses_o <= dbg_misses_o + 1'd1;
                end
                // (miss with port unavailable: dropped by the arbiter, DOC times out)
            end

            // 4. coherency (a parked fill completing after a flush would
            // resurrect a stale word: drop it)
            if (flush_i) begin valid_r <= '0; fill_pend_r <= 1'b0; end
        end
    end
endmodule

// 64 x 32 single-port RAM with registered read (one 18 Kb block on GW5A as
// SP x32). Write-else-read on one address: the cache never reads and writes
// in the same cycle (the fill yields). Gowin-safe inference idiom: separate
// write and read statements, no reset on the array.
module doc_cache_data_sp (
    input  wire        clk_i,
    input  wire        we_i,
    input  wire [5:0]  addr_i,
    input  wire [31:0] din_i,
    output reg  [31:0] dout_o
);
    reg [31:0] mem [0:63] /* synthesis syn_ramstyle="block_ram" */;
    always @(posedge clk_i) begin
        if (we_i) mem[addr_i] <= din_i;
        else      dout_o <= mem[addr_i];
    end
endmodule
