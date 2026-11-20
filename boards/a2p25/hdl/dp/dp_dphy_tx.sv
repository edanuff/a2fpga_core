// dp_dphy_tx.sv - DisplayPort main-link PHY shim for the GW5A-25 hardened
// MIPI D-PHY (Gowin_MIPI_DPHY wrapper, 16-bit gearbox mode, RBR 1620 Mbps).
//
// Sits between the DP link layer (8b/10b symbol stream, 4 lanes in lockstep)
// and the Gowin_MIPI_DPHY IP. The D-PHY is used as a raw serializer: LP mode
// is never entered (all LP ports tied off) and hstxd_vld is held high for the
// life of the link, producing a continuous NRZ stream.
//
// Rate/gearing (per lane):
//   1620 Mbps serial = 16 bits per tx_clk (101.25 MHz) = 162 Msym/s of 10-bit
//   symbols. 10->16 regearing over LCM 80 bits: 8 symbols per 5 clocks, i.e.
//   the shim pulls one 2-symbol pair (20 bits) per lane in 4 of every 5
//   clocks. All four lanes advance in lockstep (matching DP lane striping),
//   so one shared request/valid handshake serves all lanes.
//
// Symbol supply contract (no backpressure - DP is a constant-rate link and
// the link layer always has fill symbols):
//   - When sym_req_o is high in cycle N, the link layer MUST present
//     sym_pair_i with sym_vld_i=1 in cycle N+1.
//   - sym_pair_i packs {lane3,lane2,lane1,lane0} x {sym1,sym0}: for each lane,
//     bits [9:0] = first symbol on the wire, [19:10] = second.
//   - Bit order: bit 0 of each 10-bit symbol is the FIRST bit on the wire.
//     The D-PHY serializes each 16-bit word bit 0 first (16-bit little-endian
//     dual-word, lower byte first, LSB first - UG296E Table 4-1 /
//     TX_WORD_LITTLE_ENDIAN=1 default), so symbols are packed into ascending
//     bit positions. If the link layer's 8b/10b encoder emits 'a' (the first
//     transmitted bit of abcdeifghj) in a different position, swap there,
//     not here.
//
// Startup: the generated wrapper does NOT expose the PLL lock signal (it is a
// dangling internal wire), so this shim sequences on a settle timer after
// tx_drst_n release: driver enables -> DRV_SETTLE -> HS enables -> HS_SETTLE
// -> PRIME (one cycle, first symbol request) -> ACTIVE (continuous stream).
// TODO(board bring-up): re-expose `lock` from Gowin_MIPI_DPHY and gate SETTLE
// on it instead of (in addition to) the timer.

module dp_dphy_tx #(
    // Settle times in tx_clk (101.25 MHz) cycles. Defaults ~100 us each;
    // the TB overrides with small values.
    parameter DRV_SETTLE_CYCLES = 20'd10125,
    parameter HS_SETTLE_CYCLES  = 20'd10125
) (
    input  logic        tx_clk_i,        // tx_clk_o from Gowin_MIPI_DPHY
    input  logic        rst_n_i,         // sync reset, tx_clk domain

    // Link control
    input  logic        link_en_i,       // main link on (from link policy)
    input  logic [2:0]  lane_count_i,    // 1, 2 or 4 active lanes

    // Symbol supply (lockstep across lanes; see contract above)
    output logic        sym_req_o,
    input  logic        sym_vld_i,
    input  logic [79:0] sym_pair_i,

    // Status
    output logic        phy_active_o,    // streaming (hstxd_vld high)
    output logic        gb_underflow_o,  // sticky: link layer missed a request

    // ---- Gowin_MIPI_DPHY connections ----
    output logic        tx_drst_n_o,
    output logic        txhclk_en_o,
    output logic        hstxd_vld_o,
    output logic [15:0] d0ln_hstxd_o,
    output logic [15:0] d1ln_hstxd_o,
    output logic [15:0] d2ln_hstxd_o,
    output logic [15:0] d3ln_hstxd_o,
    output logic [3:0]  hstxen_ln_o,     // [0]=ln0 .. [3]=ln3
    output logic [3:0]  txdpen_ln_o,
    // LP / RX side: never used for DP - tied off
    output logic [3:0]  lptxen_ln_o,
    output logic [3:0]  do_lptx_n_o,
    output logic [3:0]  do_lptx_p_o,
    output logic [3:0]  lprx_en_d_o
);

    // LP/RX permanently off
    assign lptxen_ln_o = 4'b0000;
    assign do_lptx_n_o = 4'b0000;
    assign do_lptx_p_o = 4'b0000;
    assign lprx_en_d_o = 4'b0000;

    assign tx_drst_n_o = rst_n_i;

    // Active-lane mask from lane count (DP uses lanes 0..N-1)
    logic [3:0] lane_mask;
    always_comb begin
        case (lane_count_i)
            3'd1:    lane_mask = 4'b0001;
            3'd2:    lane_mask = 4'b0011;
            default: lane_mask = 4'b1111;   // 4 lanes
        endcase
    end

    // ---- enable sequencing FSM ----
    localparam [2:0] S_IDLE       = 3'd0;
    localparam [2:0] S_DRV_SETTLE = 3'd1;
    localparam [2:0] S_HS_SETTLE  = 3'd2;
    localparam [2:0] S_PRIME      = 3'd3;
    localparam [2:0] S_ACTIVE     = 3'd4;

    logic [2:0]  state_r;
    logic [19:0] settle_cnt_r;

    // ---- shared 10->16 gearbox (one bit-count for all lanes) ----
    // gb_buf holds gb_cnt residual bits (LSB-aligned, bit 0 = next on wire).
    // Each ACTIVE cycle: append 20 bits (when sym_vld_i) at offset gb_cnt,
    // emit low 16, shift down. gb_cnt cycles 4,8,12,16,0,... (pull skipped
    // in the cycle after gb_cnt reaches 16).
    logic [35:0] gb_buf_r  [0:3];
    logic [5:0]  gb_cnt_r;

    logic [19:0] lane_pair [0:3];
    assign lane_pair[0] = sym_pair_i[19:0];
    assign lane_pair[1] = sym_pair_i[39:20];
    assign lane_pair[2] = sym_pair_i[59:40];
    assign lane_pair[3] = sym_pair_i[79:60];

    // Combinational next-state of the gearbox datapath
    logic [5:0]  eff_cnt;
    logic [35:0] gb_merged [0:3];
    genvar gi;
    generate
        for (gi = 0; gi < 4; gi = gi + 1) begin : g_merge
            assign gb_merged[gi] = sym_vld_i
                ? (gb_buf_r[gi] | ({16'd0, lane_pair[gi]} << gb_cnt_r))
                : gb_buf_r[gi];
        end
    endgenerate
    assign eff_cnt = gb_cnt_r + (sym_vld_i ? 6'd20 : 6'd0);

    integer li;

    always_ff @(posedge tx_clk_i) begin
        if (!rst_n_i) begin
            state_r        <= S_IDLE;
            settle_cnt_r   <= 20'd0;
            gb_cnt_r       <= 6'd0;
            sym_req_o      <= 1'b0;
            hstxd_vld_o    <= 1'b0;
            phy_active_o   <= 1'b0;
            gb_underflow_o <= 1'b0;
            txhclk_en_o    <= 1'b0;
            hstxen_ln_o    <= 4'b0;
            txdpen_ln_o    <= 4'b0;
            d0ln_hstxd_o   <= 16'h0;
            d1ln_hstxd_o   <= 16'h0;
            d2ln_hstxd_o   <= 16'h0;
            d3ln_hstxd_o   <= 16'h0;
            for (li = 0; li < 4; li = li + 1) gb_buf_r[li] <= 36'd0;
        end else begin
            case (state_r)
                S_IDLE: begin
                    hstxd_vld_o  <= 1'b0;
                    phy_active_o <= 1'b0;
                    sym_req_o    <= 1'b0;
                    gb_cnt_r     <= 6'd0;
                    for (li = 0; li < 4; li = li + 1) gb_buf_r[li] <= 36'd0;
                    if (link_en_i) begin
                        // drivers on first (UG296E Fig 4-1 ordering:
                        // txdpen before hstxen)
                        txdpen_ln_o  <= lane_mask;
                        txhclk_en_o  <= 1'b1;
                        settle_cnt_r <= DRV_SETTLE_CYCLES;
                        state_r      <= S_DRV_SETTLE;
                    end else begin
                        txdpen_ln_o <= 4'b0;
                        hstxen_ln_o <= 4'b0;
                        txhclk_en_o <= 1'b0;
                    end
                end

                S_DRV_SETTLE: begin
                    if (!link_en_i) state_r <= S_IDLE;
                    else if (settle_cnt_r != 20'd0) settle_cnt_r <= settle_cnt_r - 20'd1;
                    else begin
                        hstxen_ln_o  <= lane_mask;
                        settle_cnt_r <= HS_SETTLE_CYCLES;
                        state_r      <= S_HS_SETTLE;
                    end
                end

                S_HS_SETTLE: begin
                    if (!link_en_i) state_r <= S_IDLE;
                    else if (settle_cnt_r != 20'd0) settle_cnt_r <= settle_cnt_r - 20'd1;
                    else begin
                        sym_req_o <= 1'b1;      // first pair arrives next cycle
                        state_r   <= S_PRIME;
                    end
                end

                S_PRIME: begin
                    // First pair lands during the coming ACTIVE cycle
                    // (eff_cnt will be 20 there: emit + request again).
                    sym_req_o <= 1'b1;
                    state_r   <= S_ACTIVE;
                end

                S_ACTIVE: begin
                    if (!link_en_i) begin
                        state_r      <= S_IDLE;
                        sym_req_o    <= 1'b0;
                        hstxd_vld_o  <= 1'b0;
                        phy_active_o <= 1'b0;
                    end else begin
                        if (eff_cnt < 6'd16) begin
                            gb_underflow_o <= 1'b1;   // supply contract broken
                        end else begin
                            d0ln_hstxd_o <= lane_mask[0] ? gb_merged[0][15:0] : 16'h0;
                            d1ln_hstxd_o <= lane_mask[1] ? gb_merged[1][15:0] : 16'h0;
                            d2ln_hstxd_o <= lane_mask[2] ? gb_merged[2][15:0] : 16'h0;
                            d3ln_hstxd_o <= lane_mask[3] ? gb_merged[3][15:0] : 16'h0;
                            for (li = 0; li < 4; li = li + 1)
                                gb_buf_r[li] <= gb_merged[li] >> 16;
                            gb_cnt_r     <= eff_cnt - 6'd16;
                            hstxd_vld_o  <= 1'b1;
                            phy_active_o <= 1'b1;
                        end
                        // Request pipeline is 2 cycles deep (req registered at
                        // end of X -> visible X+1 -> vld X+2), so project the
                        // level at X+2's emit including the pull already in
                        // flight (current sym_req_o -> vld at X+1):
                        //   level(X+1) = (eff-16) + 20*req_now; request iff
                        //   level(X+1) - 16 < 16, i.e. level(X+1) < 32.
                        sym_req_o <= (((eff_cnt - 6'd16)
                                       + (sym_req_o ? 6'd20 : 6'd0)) < 6'd32);
                    end
                end

                default: state_r <= S_IDLE;
            endcase
        end
    end

endmodule
