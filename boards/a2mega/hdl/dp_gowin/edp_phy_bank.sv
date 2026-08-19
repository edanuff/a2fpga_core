///////////////////////////////////////////////////////////////////////////////
// edp_phy_bank.sv — thin bank wrapper around the Gowin EDP PHY emission
// (WS4: Gowin-IP DP stack evaluation build)
//
// Wraps the IDE-generated SERDES wrapper `dp_serdes` (serdes_edp/: SERDES
// IP -> EDP PHY protocol module; recovered verbatim from the 2026-08-16
// generation at commit d535df5f) and presents the byte+K face the Gowin
// EDP Encoder IP drives. Configuration inside the emission:
//
//   Q0 lanes 2+3, REFCLK1 @ 135 MHz external osc, QPLL0, 2.7 Gbps (HBR),
//   1:40 user clock ratio / 1:2 gear (67.5 MHz fabric TX clock, 32-bit
//   +4K face — the ONLY geometry Gowin ships hardened 8b10b in; see WS4
//   forensics §10: repaired 8b10b enables at 1:1 gear yielded a broken
//   60.75 MHz fabric clock on silicon), hardened PCS 8b10b (width 20 on
//   the wire), TX PN invert BOTH lanes (board P/N swap at the
//   connector), txlev 13 (~804 mV), FFE auto, DRP port on.
//
// Board lane map (see transceiver_bank_gowin.v header): die lane 3 ->
// TUSB1046A DP0 (main link lane 0), die lane 2 -> DP1 (main link lane 1).
// So encoder lane 0 (O_tx_data_0) drives ln3 and encoder lane 1 drives
// ln2. The serial pads are dedicated SERDES bumps — no RTL ports, no cst.
//
// Hard-won sequencing rules carried over from the EDP-PHY-era
// transceiver_bank_gowin (commits 0353b5dc..d535df5f):
//   - release resets STATICALLY on a timer; never gate a release on a
//     lock indicator (deadlocks). fabric_rstn first, tx_rst ~21 ms later.
//   - tx_vld must NOT be tied 1 (reference-design style): with tx_rst
//     holding the PCS read side while the fabric writes, the TX FIFO
//     parks almost-full and drops words forever (sink CR-locks, never
//     symbol-locks). Release tx_vld a few fabric clocks after tx_rst.
//   - por_n follows the powerup request (refclk is stable long before).
//
// The DRP port is kept as a READ-ONLY register-dump bridge for the UART
// telemetry (same address ROM as the production build). No runtime CSR
// writes: the generator emission already carries tx_pol_invert.
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module edp_phy_bank (
    input             mgmt_clk,          // 100 MHz management clock
    input       [1:0] powerup_channel,   // from link policy (lane1, lane0)
    output reg  [1:0] tx_running,

    // Link-word clock domain face (67.5 MHz = 2.7G/40, from ln2 PCS)
    output            tx_symbol_clk,
    output            tx_ready,          // ~tx_rst synced into ls domain;
                                         // use as the encoder's I_rst_n
    // Encoder byte+K streams: word lane 0 = main link lane 0 -> die ln3,
    // word lane 1 = main link lane 1 -> die ln2. Low byte transmits
    // first; txk bit N flags byte N (IPUG1043 3.4.1).
    input      [31:0] ml0_txdata,
    input       [3:0] ml0_txk,
    input      [31:0] ml1_txdata,
    input       [3:0] ml1_txk,

    // Raw bring-up status {txfifo_afull, txfifo_full, pll_lock,
    // lane_ok[1:0], ~tx_rst, tx_running[1:0]} — status only
    output      [7:0] serdes_status,

    // DRP register-dump readback (idx quasi-static; data/addr safe to
    // sample loosely once done)
    input       [4:0] dbg_idx,
    output     [31:0] dbg_data,
    output     [23:0] dbg_addr,
    output            dbg_done
);

    // ------------------------------------------------------------------
    // Reset / power-up cadence (see header)
    // ------------------------------------------------------------------
    reg  fabric_rstn = 1'b0;
    reg  tx_rst      = 1'b1;
    wire pll_lock;
    wire [1:0] lane_ok;

    reg [21:0] seq_count = 0;
    always @(posedge mgmt_clk) begin
        if (powerup_channel == 2'b00) begin
            fabric_rstn <= 1'b0;
            tx_rst      <= 1'b1;
            seq_count   <= 0;
            tx_running  <= 2'b00;
        end else begin
            fabric_rstn <= 1'b1;
            if (seq_count[21])
                tx_rst <= 1'b0;          // ~21 ms after powerup; ungated
            else
                seq_count <= seq_count + 1'b1;
            // status only — consumed by nothing critical
            tx_running <= {2{!tx_rst}} & powerup_channel & lane_ok;
        end
    end

    wire tx_afull_ln2, tx_afull_ln3, tx_full_ln2, tx_full_ln3;
    assign serdes_status = {(tx_afull_ln2 | tx_afull_ln3),
                            (tx_full_ln2  | tx_full_ln3),
                            pll_lock, lane_ok, ~tx_rst, tx_running};

    // tx_vld write-gate (see header): empty-FIFO lockstep start
    reg [2:0] vld_sync = 3'b000;
    wire      tx_vld   = vld_sync[2];
    always @(posedge tx_symbol_clk)
        vld_sync <= {vld_sync[1:0], ~tx_rst};
    assign tx_ready = tx_vld;

    // ------------------------------------------------------------------
    // IDE-generated SERDES wrapper (EDP PHY protocol module)
    // ------------------------------------------------------------------
    wire tx_symbol_clk_raw;

    // DRP plumbing (reader FSM below)
    wire        drp_clk_w;
    reg  [23:0] drp_addr_r  = 24'd0;
    reg         drp_rden_r  = 1'b0;
    wire        drp_rdvld_w;
    wire [31:0] drp_rddata_w;
    wire        drp_ready_w;

    reg por_n = 1'b0;
    always @(posedge mgmt_clk)
        por_n <= (powerup_channel != 2'b00);

    wire qpll0_ok, qpll1_ok, ln2_cpll_ok, ln3_cpll_ok;
    // whichever PLL the emission uses asserts its indicator; OR them so
    // the status word is config-agnostic (this config: QPLL0)
    assign pll_lock = qpll0_ok | qpll1_ok | (ln2_cpll_ok & ln3_cpll_ok);
    assign lane_ok  = {2{pll_lock}};

    dp_serdes i_dp_serdes (
        .por_n_i                     (por_n),
        // die lane 2 (DP1, main link lane 1); TX clock master
        .edp_phy_ln2_fabric_rstn_i   (fabric_rstn),
        .edp_phy_ln2_tx_rst_i        (tx_rst),
        .edp_phy_ln2_fabric_tx_clk_i (tx_symbol_clk),
        .edp_phy_ln2_pcs_tx_clk_o    (tx_symbol_clk_raw),
        .edp_phy_ln2_tx_vld_i        (tx_vld),
        .edp_phy_ln2_txdata_i        (ml1_txdata),
        .edp_phy_ln2_txk_i           (ml1_txk),
        .edp_phy_ln2_txfifo_wrusewd_o(),
        .edp_phy_ln2_txfifo_afull_o  (tx_afull_ln2),
        .edp_phy_ln2_txfifo_full_o   (tx_full_ln2),
        .edp_phy_ln2_cpll_ok_o       (ln2_cpll_ok),
        // lane 2 RX unused (TX-only application)
        .edp_phy_ln2_rx_rst_i        (1'b1),
        .edp_phy_ln2_fabric_rx_clk_i (1'b0),
        .edp_phy_ln2_rxfifo_rden_i   (1'b0),
        .edp_phy_ln2_chbond_start_i  (1'b0),
        .edp_phy_ln2_pma_rx_lock_o   (),
        .edp_phy_ln2_align_link_o    (),
        .edp_phy_ln2_k_lock_o        (),
        .edp_phy_ln2_pcs_rx_clk_o    (),
        .edp_phy_ln2_rxfifo_rdusewd_o(),
        .edp_phy_ln2_rxfifo_aempty_o (),
        .edp_phy_ln2_rxfifo_empty_o  (),
        .edp_phy_ln2_rxdata_o        (),
        .edp_phy_ln2_rxk_o           (),
        // die lane 3 (DP0, main link lane 0)
        .edp_phy_ln3_fabric_rstn_i   (fabric_rstn),
        .edp_phy_ln3_tx_rst_i        (tx_rst),
        .edp_phy_ln3_fabric_tx_clk_i (tx_symbol_clk),
        .edp_phy_ln3_pcs_tx_clk_o    (),
        .edp_phy_ln3_tx_vld_i        (tx_vld),
        .edp_phy_ln3_txdata_i        (ml0_txdata),
        .edp_phy_ln3_txk_i           (ml0_txk),
        .edp_phy_ln3_txfifo_wrusewd_o(),
        .edp_phy_ln3_txfifo_afull_o  (tx_afull_ln3),
        .edp_phy_ln3_txfifo_full_o   (tx_full_ln3),
        .edp_phy_ln3_cpll_ok_o       (ln3_cpll_ok),
        // lane 3 RX unused
        .edp_phy_ln3_rx_rst_i        (1'b1),
        .edp_phy_ln3_fabric_rx_clk_i (1'b0),
        .edp_phy_ln3_rxfifo_rden_i   (1'b0),
        .edp_phy_ln3_chbond_start_i  (1'b0),
        .edp_phy_ln3_pma_rx_lock_o   (),
        .edp_phy_ln3_align_link_o    (),
        .edp_phy_ln3_k_lock_o        (),
        .edp_phy_ln3_pcs_rx_clk_o    (),
        .edp_phy_ln3_rxfifo_rdusewd_o(),
        .edp_phy_ln3_rxfifo_aempty_o (),
        .edp_phy_ln3_rxfifo_empty_o  (),
        .edp_phy_ln3_rxdata_o        (),
        .edp_phy_ln3_rxk_o           (),
        // PLL indicators
        .edp_phy_qpll0_ok_o          (qpll0_ok),
        .edp_phy_qpll1_ok_o          (qpll1_ok),
        // DRP: read-only register-dump bridge (writes never enabled)
        .edp_phy_drp_clk_o           (drp_clk_w),
        .edp_phy_drp_addr_i          (drp_addr_r),
        .edp_phy_drp_wren_i          (1'b0),
        .edp_phy_drp_wrdata_i        (32'd0),
        .edp_phy_drp_strb_i          (8'd0),
        .edp_phy_drp_rden_i          (drp_rden_r),
        .edp_phy_drp_ready_o         (drp_ready_w),
        .edp_phy_drp_rdvld_o         (drp_rdvld_w),
        .edp_phy_drp_rddata_o        (drp_rddata_w),
        .edp_phy_drp_resp_o          ()
    );

    assign tx_symbol_clk = tx_symbol_clk_raw;

    // ------------------------------------------------------------------
    // DRP register-dump reader (drp_clk_w domain): rden high with addr,
    // hold until rdvld, capture, drop rden the following cycle. A read
    // that never returns rdvld is recorded as 0xDEAD_xxxx. Address ROM
    // = the production build's physical-lane CSR probe set (same
    // physical lanes 2/3, so the map holds).
    // ------------------------------------------------------------------
    function [23:0] dump_addr_rom(input [4:0] i);
        case (i)
            // common/config block probes
            5'd0:  dump_addr_rom = 24'h8081a4;
            5'd1:  dump_addr_rom = 24'h8081a8;
            5'd2:  dump_addr_rom = 24'h808758;
            5'd3:  dump_addr_rom = 24'h808284;
            5'd4:  dump_addr_rom = 24'h808384;
            5'd5:  dump_addr_rom = 24'h808484;
            5'd6:  dump_addr_rom = 24'h808584;
            5'd7:  dump_addr_rom = 24'h808760;
            5'd8:  dump_addr_rom = 24'h800b91;
            // lane2 control/status block
            5'd9:  dump_addr_rom = 24'h809468;
            5'd10: dump_addr_rom = 24'h80946c;
            5'd11: dump_addr_rom = 24'h809400;
            5'd12: dump_addr_rom = 24'h80943c;
            5'd13: dump_addr_rom = 24'h809410;
            5'd14: dump_addr_rom = 24'h809420;
            // lane3
            5'd15: dump_addr_rom = 24'h809668;
            5'd16: dump_addr_rom = 24'h809600;
            5'd17: dump_addr_rom = 24'h80963c;
            // per-lane analog/driver blocks (lanes 2/3)
            5'd18: dump_addr_rom = 24'h8084c0;
            5'd19: dump_addr_rom = 24'h8085c0;
            5'd20: dump_addr_rom = 24'h8084b8;
            5'd21: dump_addr_rom = 24'h8084bc;
            // QPLL/refclk-adjacent lane blocks
            5'd22: dump_addr_rom = 24'h800658;
            5'd23: dump_addr_rom = 24'h800832;
            default: dump_addr_rom = 24'h000000;
        endcase
    endfunction

    reg [4:0]  idx_meta = 5'd0, idx_sync = 5'd0;
    reg [31:0] res_data = 32'd0;
    reg [23:0] res_addr = 24'd0;
    reg        res_vld  = 1'b0;
    reg [1:0]  rd_state = 2'd0;   // 0=gap, 1=read, 2=drop
    reg [15:0] rd_tmo   = 16'd0;
    reg [17:0] rd_gap   = 18'd0;  // settle between reads (~ms)

    always @(posedge drp_clk_w) begin
        idx_meta <= dbg_idx;
        idx_sync <= idx_meta;
        case (rd_state)
            2'd0: begin
                drp_rden_r <= 1'b0;
                rd_gap <= rd_gap + 18'd1;
                if (&rd_gap) begin
                    rd_tmo   <= 16'd0;
                    rd_state <= 2'd1;
                    drp_addr_r <= dump_addr_rom(idx_sync);
                end
            end
            2'd1: begin                  // rden high until rdvld or timeout
                drp_rden_r <= 1'b1;
                rd_tmo <= rd_tmo + 16'd1;
                if (drp_rdvld_w) begin
                    res_data <= drp_rddata_w;
                    res_addr <= drp_addr_r;
                    res_vld  <= 1'b1;
                    rd_state <= 2'd2;
                end else if (&rd_tmo) begin
                    res_data <= {16'hDEAD, 11'd0, idx_sync};
                    res_addr <= drp_addr_r;
                    res_vld  <= 1'b1;
                    rd_state <= 2'd2;
                end
            end
            default: begin               // release strobe, re-arm gap
                drp_rden_r <= 1'b0;
                rd_gap   <= 18'd0;
                rd_state <= 2'd0;
            end
        endcase
    end

    assign dbg_data = res_data;
    assign dbg_addr = res_addr;
    assign dbg_done = res_vld;

endmodule
