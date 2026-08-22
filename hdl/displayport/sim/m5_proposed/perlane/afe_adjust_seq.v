///////////////////////////////////////////////////////////////////////////////
// afe_adjust_seq_perlane.v : PROPOSED per-lane version of afe_adjust_seq
// (M5 runtime TX-AFE adjustment honoring ADJUST_REQUEST, DPCD 0x206).
//
// Same module name `afe_adjust_seq` so it is a drop-in replacement for
// hdl/displayport/gowin/afe_adjust_seq.v once the accompanying port-width
// diffs (hdl/displayport/sim/m5_proposed/perlane_*.diff) are applied.
//
// WHAT CHANGES vs the committed single-request module
//   - vs_request / pe_request are PER LANE, packed [2*NUM_LANES-1:0] as
//     {lane(N-1), ..., lane1, lane0}; for the 2-lane link: [3:0] =
//     {lane1[1:0], lane0[1:0]}. Lane 0 = DPCD 0x206[1:0]/[3:2], lane 1 =
//     0x206[5:4]/[7:6] (exactly the channel_adjust/debug_adjust nibbles).
//   - train_set_byte is PER LANE, packed [8*NUM_LANES-1:0]:
//     [7:0] = TRAINING_LANE0_SET (DPCD 0x103), [15:8] = LANE1 (0x104).
//   - applied state (vs, pe) is tracked per lane; the clamp (MAX_VS /
//     MAX_PE, ceiling flags) is evaluated per lane; an ADJUST_REQUEST is
//     applied iff AT LEAST ONE lane's clamped request differs from that
//     lane's applied state, and then ONLY the changed lanes receive the
//     4-write DRP sequence (their own LANE_BASEn only). A lane whose
//     request equals its applied state writes nothing.
//   - dbg_afe[5:0] keeps its meaning and still reports LANE 0
//     ({seq_err, known, pe0[1:0], vs0[1:0]}) so the 'M:' telemetry field
//     is unchanged; a NEW output dbg_afe1[3:0] = {pe1[1:0], vs1[1:0]}
//     exposes lane 1 (leave unconnected if unused).
//   - APPLY_ON_TRAINING_START semantics unchanged: 1 = INIT applied to
//     ALL lanes at training start (NUM_LANES x 4 writes); 0 = trust the
//     boot csr, declare INIT on every lane, write only on a real change.
//   - DRP port protocol (req/gnt/addr/wrdata/wren/ready) UNCHANGED. One
//     grant covers the whole multi-lane sequence, lanes in ascending
//     order, 4 writes per lane in the verified order (0x34, 0x38, 0xd8=
//     0x010, 0xd8=0x110), same inter-op gap and timeout/seq_err.
//
// Everything else (register model, level mapping, trigger policy, CDC
// toggle handshake, ENABLE_AFE_ADJUST=0 tie-offs) is as in the committed
// module header — see hdl/displayport/gowin/afe_adjust_seq.v.
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module afe_adjust_seq #(
    parameter ENABLE_AFE_ADJUST = 0,
    parameter NUM_LANES         = 2,           // 1..4 (lane bases below)
    parameter [23:0] LANE_BASE0 = 24'h808300,  // 138B die lane 1
    parameter [23:0] LANE_BASE1 = 24'h808400,  // 138B die lane 2
    parameter [23:0] LANE_BASE2 = 24'h000000,  // future 4-lane
    parameter [23:0] LANE_BASE3 = 24'h000000,
    parameter [1:0]  INIT_VS    = 2'd2,        // baseline at training start
    parameter [1:0]  INIT_PE    = 2'd0,        //   (VS2/PE0 = 804 mV, 0 dB)
    parameter APPLY_ON_TRAINING_START = 1,     // 0: trust resident config
    parameter [1:0]  MAX_VS = 2'd2,            // declared ceilings (row 75)
    parameter [1:0]  MAX_PE = 2'd3,
    parameter [7:0]  IDLE_SET_BYTE = 8'h06     // legacy swing2+MAX_SWING
)(
    // ---- management-clock domain (AUX ladder) ------------------------
    input  wire                     mgmt_clk,
    input  wire [2*NUM_LANES-1:0]   vs_request,     // {..,lane1,lane0} swing
    input  wire [2*NUM_LANES-1:0]   pe_request,     // {..,lane1,lane0} pre-emph
    input  wire                     adjust_de,      // pulse: 0x206 byte captured
    input  wire                     training_active,// tx_clock_train | tx_align_train
    output wire [8*NUM_LANES-1:0]   train_set_byte, // {..,LANE1_SET,LANE0_SET}
    output wire                     afe_busy,       // application in flight
    output wire [5:0]               dbg_afe,        // LANE 0: {seq_err, known, pe, vs}
    output wire [3:0]               dbg_afe1,       // LANE 1: {pe, vs} (0 if 1 lane)
    // ---- DRP-clock domain (SERDES UPAR/DRP port) ---------------------
    input  wire                     drp_clk,
    output wire                     drp_req,
    input  wire                     drp_gnt,
    output wire [23:0]              drp_addr,
    output wire [31:0]              drp_wrdata,
    output wire                     drp_wren,
    input  wire                     drp_ready
);

generate if (ENABLE_AFE_ADJUST == 0) begin : g_off
    assign train_set_byte = {NUM_LANES{IDLE_SET_BYTE}};
    assign afe_busy       = 1'b0;
    assign dbg_afe        = 6'd0;
    assign dbg_afe1       = 4'd0;
    assign drp_req        = 1'b0;
    assign drp_addr       = 24'd0;
    assign drp_wrdata     = 32'd0;
    assign drp_wren       = 1'b0;
end else begin : g_on

    // ------------------------------------------------------------------
    // Level mapping (m5_runtime_afe.md) — unchanged
    // ------------------------------------------------------------------
    function [3:0] vs_txlev(input [1:0] v);
        case (v)
            2'd0: vs_txlev = 4'd5;    // 420 mV
            2'd1: vs_txlev = 4'd9;    // 612 mV
            2'd2: vs_txlev = 4'd13;   // 804 mV
            default: vs_txlev = 4'd15;// 900 mV = ceiling
        endcase
    endfunction

    function [4:0] pe_c1(input [1:0] p);
        case (p)
            2'd0: pe_c1 = 5'd0;       // 0 dB
            2'd1: pe_c1 = 5'd7;       // 3.7 dB
            2'd2: pe_c1 = 5'd10;      // 6.0 dB
            default: pe_c1 = 5'd13;   // 9.1 dB = ceiling
        endcase
    endfunction

    function [23:0] lane_base(input [1:0] ln);
        case (ln)
            2'd0: lane_base = LANE_BASE0;
            2'd1: lane_base = LANE_BASE1;
            2'd2: lane_base = LANE_BASE2;
            default: lane_base = LANE_BASE3;
        endcase
    endfunction

    function [23:0] seq_addr(input [1:0] ln, input [1:0] step);
        case (step)
            2'd0: seq_addr = lane_base(ln) + 24'h034;   // swing
            2'd1: seq_addr = lane_base(ln) + 24'h038;   // FFE C1|CM
            default: seq_addr = lane_base(ln) + 24'h0d8;// enable / strobe
        endcase
    endfunction

    function [31:0] seq_data(input [1:0] step, input [1:0] v, input [1:0] p);
        case (step)
            2'd0: seq_data = {16'd0, vs_txlev(v), 12'd0};   // txlev << 12
            2'd1: seq_data = {19'd0, pe_c1(p), 8'd0};       // C1<<8 | CM(=0)
            2'd2: seq_data = 32'h0000_0010;                 // manual enable
            default: seq_data = 32'h0000_0110;              // + APPLY strobe
        endcase
    endfunction

    function [1:0] clamp2(input [1:0] x, input [1:0] mx);
        clamp2 = (x > mx) ? mx : x;
    endfunction

    // ------------------------------------------------------------------
    // Management-side trigger / bookkeeping (per lane)
    // ------------------------------------------------------------------
    reg [2*NUM_LANES-1:0] applied_vs = {2*NUM_LANES{1'b0}};
    reg [2*NUM_LANES-1:0] applied_pe = {2*NUM_LANES{1'b0}};
    reg                   applied_known = 1'b0;
    reg                   apply_tgl     = 1'b0;
    // quasi-static payload for the DRP side: which lanes + their levels
    reg [NUM_LANES-1:0]   mask_lat = {NUM_LANES{1'b0}};
    reg [2*NUM_LANES-1:0] vs_lat   = {2*NUM_LANES{1'b0}};
    reg [2*NUM_LANES-1:0] pe_lat   = {2*NUM_LANES{1'b0}};
    reg [1:0]             ack_sync = 2'b00;
    wire                  ack_tgl_w;
    wire                  busy = (apply_tgl != ack_sync[1]);

    reg                   adj_d     = 1'b0;
    reg                   eval_pend = 1'b0;
    reg [2*NUM_LANES-1:0] vs_pend   = {2*NUM_LANES{1'b0}};
    reg [2*NUM_LANES-1:0] pe_pend   = {2*NUM_LANES{1'b0}};

    // per-lane clamped pending request and change detection
    wire [2*NUM_LANES-1:0] vs_clamped, pe_clamped;
    wire [NUM_LANES-1:0]   changed;
    genvar gl;
    for (gl = 0; gl < NUM_LANES; gl = gl + 1) begin : g_lane
        assign vs_clamped[2*gl +: 2] = clamp2(vs_pend[2*gl +: 2], MAX_VS);
        assign pe_clamped[2*gl +: 2] = clamp2(pe_pend[2*gl +: 2], MAX_PE);
        assign changed[gl] = ({pe_clamped[2*gl +: 2], vs_clamped[2*gl +: 2]} !=
                              {applied_pe[2*gl +: 2], applied_vs[2*gl +: 2]});
        // per-lane TRAINING_LANEx_SET byte
        wire max_sw = (applied_vs[2*gl +: 2] >= MAX_VS);
        wire max_pe = (applied_pe[2*gl +: 2] >= MAX_PE);
        assign train_set_byte[8*gl +: 8] = applied_known
            ? {2'b00, max_pe, applied_pe[2*gl +: 2], max_sw, applied_vs[2*gl +: 2]}
            : IDLE_SET_BYTE;
    end

    integer i;
    always @(posedge mgmt_clk) begin
        ack_sync <= {ack_sync[0], ack_tgl_w};
        adj_d    <= adjust_de;
        if (!training_active) begin
            applied_known <= 1'b0;
            eval_pend     <= 1'b0;
        end else begin
            if (adj_d) begin
                vs_pend   <= vs_request;
                pe_pend   <= pe_request;
                eval_pend <= 1'b1;
            end
            if (!busy) begin
                if (!applied_known) begin
                    applied_known <= 1'b1;
                    applied_vs    <= {NUM_LANES{INIT_VS}};
                    applied_pe    <= {NUM_LANES{INIT_PE}};
                    if (APPLY_ON_TRAINING_START != 0) begin
                        vs_lat    <= {NUM_LANES{INIT_VS}};
                        pe_lat    <= {NUM_LANES{INIT_PE}};
                        mask_lat  <= {NUM_LANES{1'b1}};   // every lane
                        apply_tgl <= ~apply_tgl;
                    end
                end else if (eval_pend && !adj_d) begin
                    eval_pend <= 1'b0;
                    if (|changed) begin
                        for (i = 0; i < NUM_LANES; i = i + 1) begin
                            if (changed[i]) begin
                                applied_vs[2*i +: 2] <= vs_clamped[2*i +: 2];
                                applied_pe[2*i +: 2] <= pe_clamped[2*i +: 2];
                            end
                        end
                        vs_lat    <= vs_clamped;
                        pe_lat    <= pe_clamped;
                        mask_lat  <= changed;             // only changed lanes
                        apply_tgl <= ~apply_tgl;
                    end
                end
            end
        end
    end

    assign afe_busy = busy;

    // ------------------------------------------------------------------
    // DRP-side sequence player: for each lane in mask_s (ascending),
    // the 4 writes in order; one grant for the whole sequence.
    // ------------------------------------------------------------------
    localparam [2:0] S_IDLE = 3'd0, S_REQ = 3'd1, S_NEXT = 3'd2,
                     S_SETUP = 3'd3, S_WAIT = 3'd4, S_GAP = 3'd5;
    reg [2:0]  dstate    = S_IDLE;
    reg [1:0]  tgl_sync  = 2'b00;
    reg        ack_tgl   = 1'b0;
    reg        req_token = 1'b0;
    reg [NUM_LANES-1:0]   mask_s = {NUM_LANES{1'b0}};
    reg [2*NUM_LANES-1:0] vs_s   = {2*NUM_LANES{1'b0}};
    reg [2*NUM_LANES-1:0] pe_s   = {2*NUM_LANES{1'b0}};
    reg [2:0]  lane      = 3'd0;   // 0..NUM_LANES (== NUM_LANES: done)
    reg [1:0]  step      = 2'd0;
    reg [11:0] tmo       = 12'd0;
    reg [3:0]  gap       = 4'd0;
    reg        seq_err   = 1'b0;
    reg [23:0] addr_r    = 24'd0;
    reg [31:0] wrdata_r  = 32'd0;
    reg        wren_r    = 1'b0;

    assign ack_tgl_w  = ack_tgl;
    assign drp_req    = (dstate != S_IDLE);
    assign drp_addr   = addr_r;
    assign drp_wrdata = wrdata_r;
    assign drp_wren   = wren_r;

    wire [1:0] lane_vs = vs_s[2*lane[1:0] +: 2];
    wire [1:0] lane_pe = pe_s[2*lane[1:0] +: 2];

    always @(posedge drp_clk) begin
        tgl_sync <= {tgl_sync[0], apply_tgl};
        case (dstate)
            S_IDLE: begin
                wren_r <= 1'b0;
                if (tgl_sync[1] != ack_tgl) begin
                    req_token <= tgl_sync[1];
                    mask_s    <= mask_lat;
                    vs_s      <= vs_lat;
                    pe_s      <= pe_lat;
                    lane      <= 3'd0;
                    step      <= 2'd0;
                    dstate    <= S_REQ;
                end
            end
            S_REQ: begin                       // wait for the port grant
                if (drp_gnt)
                    dstate <= S_NEXT;
            end
            S_NEXT: begin                      // find the next masked lane
                if (lane >= NUM_LANES) begin
                    ack_tgl <= req_token;      // sequence complete
                    dstate  <= S_IDLE;
                end else if (!mask_s[lane[1:0]]) begin
                    lane <= lane + 3'd1;       // skip unchanged lane
                end else begin
                    step   <= 2'd0;
                    dstate <= S_SETUP;
                end
            end
            S_SETUP: begin
                addr_r   <= seq_addr(lane[1:0], step);
                wrdata_r <= seq_data(step, lane_vs, lane_pe);
                wren_r   <= 1'b1;
                tmo      <= 12'd0;
                dstate   <= S_WAIT;
            end
            S_WAIT: begin                      // wren held until ready/timeout
                tmo <= tmo + 12'd1;
                if (drp_ready || (&tmo)) begin
                    if (!drp_ready)
                        seq_err <= 1'b1;
                    wren_r <= 1'b0;
                    gap    <= 4'd0;
                    dstate <= S_GAP;
                end
            end
            default: begin                     // S_GAP: short inter-op gap
                gap <= gap + 4'd1;
                if (&gap) begin
                    if (step == 2'd3) begin
                        lane   <= lane + 3'd1;
                        dstate <= S_NEXT;
                    end else begin
                        step   <= step + 2'd1;
                        dstate <= S_SETUP;
                    end
                end
            end
        endcase
    end

    reg [1:0] err_sync = 2'b00;
    always @(posedge mgmt_clk)
        err_sync <= {err_sync[0], seq_err};
    assign dbg_afe = {err_sync[1], applied_known, applied_pe[1:0], applied_vs[1:0]};
    if (NUM_LANES > 1) begin : g_dbg1
        assign dbg_afe1 = {applied_pe[3:2], applied_vs[3:2]};
    end else begin : g_dbg1_off
        assign dbg_afe1 = 4'd0;
    end

end endgenerate

endmodule
