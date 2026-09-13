// a2_event_log.sv — timestamped log of Apple II reset / GS-socket bring-up
// events (bench instrument for the a2mega S6 auto-arm, test log 09-10/11).
//
// Up to 24 event strobes come in per clock; each is queued and written as
// one 48-bit entry {time[31:0], code[7:0], data[7:0]} — code = strobe index
// + 1, data = the context byte at the time of writing (or the register value
// for the two register-write events). Coincident strobes drain one per clock
// in index order, so their timestamps can be a few clocks late; the log
// stops when full (first DEPTH events after a clear are what matters for a
// boot sequence) and is read back non-destructively by index.
module a2_event_log #(
    parameter DEPTH_LOG2 = 7
) (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        clear_i,        // 1 for one clock: empty the log and start again
    input  wire [23:0] ev_i,           // event strobes (1 = happened this clock)
    input  wire [7:0]  ctx_i,          // context snapshot stored as the data byte
    input  wire [7:0]  rst_wdata_i,    // data byte for event code EV_RST_WR
    input  wire [7:0]  ctrl_wdata_i,   // data byte for event code EV_CTRL_WR
    input  wire [31:0] time_i,
    input  wire [DEPTH_LOG2-1:0] rd_idx_i,
    output reg  [47:0] rd_data_o,      // {time, code, data} at rd_idx (registered)
    output wire [DEPTH_LOG2-1:0] wptr_o,
    output wire        full_o
);
    localparam DEPTH = 1 << DEPTH_LOG2;
    // codes whose data byte is the written register value rather than ctx
    localparam [7:0] EV_RST_WR  = 8'd7;   // strobe index 6
    localparam [7:0] EV_CTRL_WR = 8'd8;   // strobe index 7

    reg [47:0] mem [0:DEPTH-1] /* synthesis syn_ramstyle = "distributed_ram" */;
    reg [DEPTH_LOG2:0] wptr_r;          // MSB = full (log stopped)
    reg [23:0] pend_r;
    reg [7:0]  rst_wdata_r, ctrl_wdata_r;

    assign wptr_o = wptr_r[DEPTH_LOG2-1:0];
    assign full_o = wptr_r[DEPTH_LOG2];

    // lowest pending strobe
    reg [4:0] sel;
    reg       any;
    integer i;
    always @* begin
        sel = 5'd0;
        any = 1'b0;
        for (i = 23; i >= 0; i = i - 1)
            if (pend_r[i]) begin sel = i[4:0]; any = 1'b1; end
    end
    wire [7:0] code_w = {3'b0, sel} + 8'd1;
    wire [7:0] data_w = (code_w == EV_RST_WR)  ? rst_wdata_r :
                        (code_w == EV_CTRL_WR) ? ctrl_wdata_r : ctx_i;
    wire       take_w = any & ~full_o;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wptr_r       <= '0;
            pend_r       <= '0;
            rst_wdata_r  <= 8'h00;
            ctrl_wdata_r <= 8'h00;
        end else begin
            if (ev_i[6]) rst_wdata_r  <= rst_wdata_i;
            if (ev_i[7]) ctrl_wdata_r <= ctrl_wdata_i;
            if (clear_i) begin
                wptr_r <= '0;
                pend_r <= ev_i;
            end else begin
                pend_r <= (pend_r | ev_i) & ~(take_w ? (24'd1 << sel) : 24'd0);
                if (take_w)
                    wptr_r <= wptr_r + 1'b1;
            end
        end
    end

    always @(posedge clk) begin
        if (take_w && !clear_i)
            mem[wptr_r[DEPTH_LOG2-1:0]] <= {time_i, code_w, data_w};
        rd_data_o <= mem[rd_idx_i];
    end
endmodule
