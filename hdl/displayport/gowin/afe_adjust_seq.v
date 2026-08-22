///////////////////////////////////////////////////////////////////////////////
// afe_adjust_seq.v : M5 runtime TX-AFE adjustment (honor ADJUST_REQUEST)
//
// Part of the DisplayPort_Verilog-derived a2fpga DP source. New module —
// no existing shared RTL is modified by adding this file.
//
// Makes the sink's ADJUST_REQUEST (DPCD 0x206/0x207) a real control loop:
// when the requested voltage-swing / pre-emphasis level differs from what
// the transmitter currently drives, this module plays the per-lane GTR12
// TX-AFE write sequence over the SERDES DRP/UPAR port, then reports the
// actually-applied levels for TRAINING_LANE_SET construction.
//
// Register model (GW5AST-138B Customized PHY; VERIFIED 08-21, two
// independent sources — boards/a2mega/docs/m5_runtime_afe.md):
//   per-lane base: lane1 = 0x8083xx, lane2 = 0x8084xx (stride 0x100)
//   base+0x34 = txlev << 12          (TX swing, txlev 0-15)
//   base+0x38 = (C1 << 8) | CM       (FFE post-tap C1 [12:8], pre-tap CM [4:0])
//   base+0xd8 = 0x010                (FFE manual enable)
//   base+0xd8 = 0x110                (APPLY strobe on top of enable)
// Sequencing law (from the IDE Reconfiguration-dialog export): the four
// writes above IN THAT ORDER, complete per lane, then the next lane. A
// RUNNING transmitter needs the 0x110 strobe — boot-time config does not.
// Writes are full-value (all other bits 0), matching both the boot .csr
// emission and the dialog exports.
//
// DP level mapping (m5_runtime_afe.md, swing law Vdiffpp = 180+48*txlev):
//   VS 0/1/2/3 -> txlev  5/ 9/13/15   (420/612/804/900 mV; 900 = our max,
//                                      so a VS3 request raises MAX_SWING)
//   PE 0/1/2/3 -> C1     0/ 7/10/13   (0/3.7/6.0/9.1 dB; PE3 raises MAX_PE)
//   CM stays 0 (no pre-shoot tap in DP 1.x training).
//
// Trigger policy (mgmt_clk domain, all inputs from the AUX ladder):
//   - only while a training pattern is being transmitted (training_active
//     = tx_clock_train | tx_align_train, i.e. TPS1/TPS2);
//   - at each training start (training_active rise) the INIT_VS/INIT_PE
//     baseline is applied once, so the analog state is deterministic per
//     training run regardless of what a previous run left behind;
//   - afterwards, evaluation happens ONLY on adjust_de (the pulse that
//     accompanies each received ADJUST_REQUEST 0x206 byte — one per
//     clock_adjust/align_adjust read, i.e. one per training iteration):
//     the request is applied iff it differs from the currently-applied
//     (vs,pe). This is the hard once-per-iteration debounce, and it also
//     keeps the powerup-cleared request register (0x0000 = "sink has not
//     asked yet") from being mistaken for a VS0/PE0 request.
//   - a new application is never started while one is in flight; an
//     adjust byte arriving mid-flight is latched and evaluated after.
//
// TRAINING_LANE_SET reporting: train_set_byte is the ready-to-send DPCD
// 0x103-0x106 value {2'b00, MAX_PE, pe[1:0], MAX_SWING, vs[1:0]} echoing
// the applied levels; the MAX flags assert at level 3 (VS3 applies our
// 900 mV ceiling, PE3 our 9.1 dB ceiling — both truthfully declared as
// "maximum reached"). Before the first application (and always when
// ENABLE_AFE_ADJUST = 0) the byte is IDLE_SET_BYTE = 0x06, the shipping
// fixed swing2+MAX_SWING declaration — byte-identical legacy behavior.
//
// DRP side (drp_clk domain — the arbiter's drp_clk_o, same domain as the
// existing read/replay engine in transceiver_bank_gowin): a req/gnt pair
// arbitrates against the bank's background register dump and CSR replay;
// each write asserts wren with addr/wrdata until ready (timeout-guarded,
// sticky seq_err on timeout), then a short gap, per the bank's own write
// choreography. The bank derives strb = wren ? 8'hFF : 8'h00 at the port.
//
// CDC: apply_tgl / ack_tgl toggle handshake; vs_lat/pe_lat and the
// applied/err debug fields are quasi-static by construction (settled >= 2
// destination clocks before the toggle edge is observed). Constrain the
// crossings as false paths / loosely-timed like the bank's dbg_* signals.
//
// ENABLE_AFE_ADJUST = 0 (default): every output is a constant tie-off —
// no DRP activity, train_set_byte = IDLE_SET_BYTE — current fixed-swing
// behavior, byte-identical.
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
    // Declared ceilings (row 75): requests above these are CLAMPED and
    // the MAX_SWING / MAX_PE flags are raised AT the ceiling. Our 900 mV
    // hardware max sits nearer DP nominal VS2 (800 mV) than VS3 (1200),
    // so declaring VS3 reachable overstated the PHY — sinks escalated to
    // it and trained worse. Default MAX_VS=2: the sink settles at VS2
    // (804 mV) and spends its next request on pre-emphasis instead.
    parameter [1:0]  MAX_VS = 2'd2,
    parameter [1:0]  MAX_PE = 2'd3,
    parameter [7:0]  IDLE_SET_BYTE = 8'h06     // legacy swing2+MAX_SWING
)(
    // ---- management-clock domain (AUX ladder) ------------------------
    input  wire        mgmt_clk,
    input  wire  [1:0] vs_request,       // parsed ADJUST_REQUEST lane0 swing
    input  wire  [1:0] pe_request,       //   (DPCD 0x206[1:0] / 0x206[3:2])
    input  wire        adjust_de,        // pulse: 0x206 byte just captured
    input  wire        training_active,  // tx_clock_train | tx_align_train
    output wire  [7:0] train_set_byte,   // TRAINING_LANEx_SET value to send
    output wire        afe_busy,         // application in flight
    output wire  [5:0] dbg_afe,          // {seq_err, known, pe[1:0], vs[1:0]}
    // ---- DRP-clock domain (SERDES UPAR/DRP port) ---------------------
    input  wire        drp_clk,
    output wire        drp_req,          // want the DRP port (level)
    input  wire        drp_gnt,          // port granted by the bank engine
    output wire [23:0] drp_addr,
    output wire [31:0] drp_wrdata,
    output wire        drp_wren,
    input  wire        drp_ready
);

generate if (ENABLE_AFE_ADJUST == 0) begin : g_off
    // Feature disabled: constant tie-offs, no DRP activity ever.
    assign train_set_byte = IDLE_SET_BYTE;
    assign afe_busy       = 1'b0;
    assign dbg_afe        = 6'd0;
    assign drp_req        = 1'b0;
    assign drp_addr       = 24'd0;
    assign drp_wrdata     = 32'd0;
    assign drp_wren       = 1'b0;
end else begin : g_on

    // ------------------------------------------------------------------
    // Level mapping (m5_runtime_afe.md)
    // ------------------------------------------------------------------
    function [3:0] vs_txlev(input [1:0] v);
        case (v)
            2'd0: vs_txlev = 4'd5;    // 420 mV
            2'd1: vs_txlev = 4'd9;    // 612 mV
            2'd2: vs_txlev = 4'd13;   // 804 mV
            default: vs_txlev = 4'd15;// 900 mV = ceiling (MAX_SWING)
        endcase
    endfunction

    function [4:0] pe_c1(input [1:0] p);
        case (p)
            2'd0: pe_c1 = 5'd0;       // 0 dB
            2'd1: pe_c1 = 5'd7;       // 3.7 dB
            2'd2: pe_c1 = 5'd10;      // 6.0 dB
            default: pe_c1 = 5'd13;   // 9.1 dB = ceiling (MAX_PE)
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

    // write index: [3:2] = lane, [1:0] = step (per-lane complete, in order)
    localparam [3:0] LAST_WIDX = (NUM_LANES * 4) - 1;

    function [23:0] seq_addr(input [3:0] i);
        case (i[1:0])
            2'd0: seq_addr = lane_base(i[3:2]) + 24'h034;  // swing
            2'd1: seq_addr = lane_base(i[3:2]) + 24'h038;  // FFE C1|CM
            default: seq_addr = lane_base(i[3:2]) + 24'h0d8; // enable/strobe
        endcase
    endfunction

    function [31:0] seq_data(input [3:0] i, input [1:0] v, input [1:0] p);
        case (i[1:0])
            2'd0: seq_data = {16'd0, vs_txlev(v), 12'd0};   // txlev << 12
            2'd1: seq_data = {19'd0, pe_c1(p), 8'd0};       // C1<<8 | CM(=0)
            2'd2: seq_data = 32'h0000_0010;                 // manual enable
            default: seq_data = 32'h0000_0110;              // + APPLY strobe
        endcase
    endfunction

    // ------------------------------------------------------------------
    // Management-side trigger / bookkeeping
    // ------------------------------------------------------------------
    reg [1:0] applied_vs    = 2'd0;
    reg [1:0] applied_pe    = 2'd0;
    reg       applied_known = 1'b0;
    reg       apply_tgl     = 1'b0;
    reg [1:0] vs_lat        = 2'd0;   // quasi-static payload for the DRP side
    reg [1:0] pe_lat        = 2'd0;
    reg [1:0] ack_sync      = 2'b00;
    wire      ack_tgl_w;              // from DRP side
    wire      busy = (apply_tgl != ack_sync[1]);

    reg       adj_d         = 1'b0;   // adjust_de delayed: parsed request
                                      // registers hold the new value now
    reg       eval_pend     = 1'b0;   // an adjust byte awaits evaluation
    reg [1:0] vs_pend       = 2'd0;
    reg [1:0] pe_pend       = 2'd0;

    always @(posedge mgmt_clk) begin
        ack_sync <= {ack_sync[0], ack_tgl_w};
        adj_d    <= adjust_de;
        if (!training_active) begin
            // training ended (or never ran): next run re-baselines
            applied_known <= 1'b0;
            eval_pend     <= 1'b0;
        end else begin
            // capture: one snapshot per ADJUST_REQUEST read (= iteration)
            if (adj_d) begin
                vs_pend   <= vs_request;
                pe_pend   <= pe_request;
                eval_pend <= 1'b1;
            end
            if (!busy) begin
                if (!applied_known) begin
                    applied_known <= 1'b1;
                    applied_vs    <= INIT_VS;
                    applied_pe    <= INIT_PE;
                    if (APPLY_ON_TRAINING_START != 0) begin
                        vs_lat    <= INIT_VS;
                        pe_lat    <= INIT_PE;
                        apply_tgl <= ~apply_tgl;
                    end
                end else if (eval_pend && !adj_d) begin
                    eval_pend <= 1'b0;
                    if ({pe_clamped, vs_clamped} != {applied_pe, applied_vs}) begin
                        applied_vs <= vs_clamped;
                        applied_pe <= pe_clamped;
                        vs_lat     <= vs_clamped;
                        pe_lat     <= pe_clamped;
                        apply_tgl  <= ~apply_tgl;
                    end
                end
            end
        end
    end

    // clamp the pending request to the declared ceilings
    wire [1:0] vs_clamped = (vs_pend > MAX_VS) ? MAX_VS : vs_pend;
    wire [1:0] pe_clamped = (pe_pend > MAX_PE) ? MAX_PE : pe_pend;
    wire max_sw = (applied_vs >= MAX_VS);
    wire max_pe = (applied_pe >= MAX_PE);
    assign train_set_byte = applied_known
        ? {2'b00, max_pe, applied_pe, max_sw, applied_vs}
        : IDLE_SET_BYTE;
    assign afe_busy = busy;

    // ------------------------------------------------------------------
    // DRP-side sequence player
    // ------------------------------------------------------------------
    localparam [2:0] S_IDLE = 3'd0, S_REQ = 3'd1, S_SETUP = 3'd2,
                     S_WAIT = 3'd3, S_GAP = 3'd4;
    reg [2:0]  dstate    = S_IDLE;
    reg [1:0]  tgl_sync  = 2'b00;
    reg        ack_tgl   = 1'b0;
    reg        req_token = 1'b0;
    reg [1:0]  vs_s      = 2'd0;
    reg [1:0]  pe_s      = 2'd0;
    reg [3:0]  widx      = 4'd0;
    reg [11:0] tmo       = 12'd0;
    reg [3:0]  gap       = 4'd0;
    reg        seq_err   = 1'b0;   // sticky: a write never saw ready
    reg [23:0] addr_r    = 24'd0;
    reg [31:0] wrdata_r  = 32'd0;
    reg        wren_r    = 1'b0;

    assign ack_tgl_w  = ack_tgl;
    assign drp_req    = (dstate != S_IDLE);
    assign drp_addr   = addr_r;
    assign drp_wrdata = wrdata_r;
    assign drp_wren   = wren_r;

    always @(posedge drp_clk) begin
        tgl_sync <= {tgl_sync[0], apply_tgl};
        case (dstate)
            S_IDLE: begin
                wren_r <= 1'b0;
                if (tgl_sync[1] != ack_tgl) begin
                    // payload settled >= 2 drp_clk before the edge arrives
                    req_token <= tgl_sync[1];
                    vs_s      <= vs_lat;
                    pe_s      <= pe_lat;
                    widx      <= 4'd0;
                    dstate    <= S_REQ;
                end
            end
            S_REQ: begin                       // wait for the port grant
                if (drp_gnt)
                    dstate <= S_SETUP;
            end
            S_SETUP: begin
                addr_r   <= seq_addr(widx);
                wrdata_r <= seq_data(widx, vs_s, pe_s);
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
                    if (widx == LAST_WIDX) begin
                        ack_tgl <= req_token;  // sequence complete
                        dstate  <= S_IDLE;
                    end else begin
                        widx   <= widx + 4'd1;
                        dstate <= S_SETUP;
                    end
                end
            end
        endcase
    end

    // seq_err is sticky/quasi-static: loose 2FF into mgmt for debug only
    reg [1:0] err_sync = 2'b00;
    always @(posedge mgmt_clk)
        err_sync <= {err_sync[0], seq_err};
    assign dbg_afe = {err_sync[1], applied_known, applied_pe, applied_vs};

end endgenerate

endmodule
