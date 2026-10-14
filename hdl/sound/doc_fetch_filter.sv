// Orders the DOC's untagged single-word wavetable responses against its
// requests. The DOC wants at most one response at a time: when it accepts a
// new request, every response still outstanding belongs to a fetch it has
// already given up on (slot budget missed, 0x80 substituted) and must not be
// delivered — delivering it would make it the NEXT oscillator's sample and
// cascade through every following oscillator until a timeout realigned the
// stream (the "rev-1 stale hazard" of the a2mega DDR3 bring-up). Responses
// arrive in request order (hyperram_ports / ddr3_ports queues are FIFOs), so
// a count of superseded-but-outstanding responses is exact. Supersession
// follows EVERY DOC request (req_i): the DOC has moved on whether or not the
// arbiter took the request. Only the outstanding count follows accept_i
// (rd && available): a request the arbiter drops never produces a response
// and must not be counted — the DOC simply times out waiting for it.
module doc_fetch_filter (
    input  wire        clk_i,
    input  wire        rst_n_i,
    input  wire        req_i,          // DOC issued a request this cycle (accepted or not)
    input  wire        accept_i,       // ... and the memory port accepted it
    input  wire [1:0]  lane_i,         // byte lane of that request (addr[1:0])
    input  wire        resp_i,         // a 32-bit response word is valid this cycle
    input  wire [31:0] resp_data_i,
    output reg         valid_o,        // byte for the DOC's current request
    output reg  [7:0]  data_o,
    output reg  [7:0]  dbg_late_o      // discarded responses (wraps)
);
    reg [3:0] outstanding_r;   // accepted requests without a response yet
    reg [3:0] stale_owed_r;    // of those, how many are superseded (oldest first)
    reg [1:0] lane_r;          // lane of the newest accepted request
    reg [3:0] o_v, w_v;
    always_ff @(posedge clk_i) begin
        if (!rst_n_i) begin
            outstanding_r <= '0; stale_owed_r <= '0; lane_r <= '0;
            valid_o <= 1'b0; data_o <= '0; dbg_late_o <= '0;
        end else begin
            o_v = outstanding_r;
            w_v = stale_owed_r;
            valid_o <= 1'b0;
            if (resp_i) begin
                if (o_v != 4'd0) o_v = o_v - 4'd1;
                if (w_v != 4'd0) begin
                    w_v = w_v - 4'd1;                 // owed to a superseded request
                    dbg_late_o <= dbg_late_o + 1'd1;
                end else if (req_i || outstanding_r == 4'd0) begin
                    dbg_late_o <= dbg_late_o + 1'd1;  // superseded this very cycle / unexpected
                end else begin
                    valid_o <= 1'b1;
                    data_o  <= resp_data_i[8*lane_r +: 8];
                end
            end
            if (req_i) begin
                w_v = w_v + o_v;                      // everything still outstanding is now stale
                lane_r <= lane_i;
            end
            if (accept_i && o_v != 4'hF) o_v = o_v + 4'd1;
            outstanding_r <= o_v;
            stale_owed_r  <= w_v;
        end
    end
endmodule
