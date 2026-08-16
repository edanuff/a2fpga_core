///////////////////////////////////////////////////////////////////////////////
// transceiver_bank_gowin.v : GW5AT (Arora V) SERDES bank for the DP main link
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Drop-in replacement for src/artix7/transceiver_bank.v (same port
// contract) targeting the Gowin GTR12 transceiver quad via the
// IDE-generated EDP PHY IP (IPUG1043):
//
//   - EDP PHY replaces the earlier Customized PHY (IPUG1024) usage. The
//     Customized PHY generator emitted HALF a TX bond for this config
//     (slave-lane tx_if FIFO chained to the master, but per-lane
//     independent PCS clocks, pcs_tx_clk_src=1) - chained FIFOs on
//     unrelated read clocks made interlane phase a per-powerup lottery.
//     The EDP PHY's architecture is a shared PCS TX clock
//     (pcs_tx_clk_src=2, IPUG1043 3.3.1: "the fabric_tx_clk_i of each
//     lane shares the pcs_tx_clk_o of one of the lanes"), which fixes
//     interlane phase by construction.
//   - line rate 2.7 Gbps (DP HBR), 1:20 user clock ratio -> 135 MHz
//     fabric TX clock = tx_symbol_clk, sourced from logical lane 0's
//     pcs_tx_clk_o and fed back to both lanes' fabric_tx_clk_i
//   - 8b10b is the HARDENED PCS encoder (encode_mode=8b10b): we feed
//     raw bytes + K flags (txdata[15:0]/txk[1:0], low byte transmitted
//     first, LSB first). The fabric lane_encoder_8b10b path is gone
//     with one semantic delta: the hardened encoder has no per-character
//     disparity-force input, so TPS2's first K28.5 is no longer forced
//     to RD-. The 10-symbol TPS2 period is disparity-self-sustaining,
//     so only the initial +/- phase of the K28.5 alternation can differ
//     - a legal 8b10b stream either way (Gowin's own EDP Encoder drives
//     TPS2 through this same interface with no disparity control).
//   - a2mega lane map: board routes die lane 3 to TUSB1046A DP0 and die
//     lane 2 to DP1, both pairs P/N-inverted (TX PN Invert in the IP).
//     EDP PHY generated on Q0 lanes 2+3 -> logical ln0 = die lane 2,
//     logical ln1 = die lane 3. Word lane 0 (ML0) therefore drives ln1
//     and word lane 1 (ML1) drives ln0.
//   - swing / FFE are static in the IP config (804 mV, FFE auto);
//     AUX-requested levels are exposed on swing_sel/preemp_sel for a
//     future DRP FSM, and DP sinks accept "max swing reached" replies
//
// The IP instantiation is guarded by GOWIN_SERDES_IP because the
// generated dp_serdes wrapper and its .toml/.csr sidecars must come from
// the Gowin IP Core Generator (SERDES IP -> EDP PHY protocol module; the
// wrapper instantiates the edp_phy interposer internally and exposes the
// user face directly). Without the define, a behavioural stub stands in
// so the full design simulates and lints: refclk0 is used directly as
// the word clock and lock indicators assert after fake delays.
//
// References: IPUG1043E (EDP PHY), IPUG1024E, IPUG1179E,
// github.com/key2/gowin-serdes, Sipeed TangMega examples.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

// TX_PROBE: bring-up relic from the raw-mode Customized PHY (it bypassed
// 8b10b with a ~1 MHz square). Through the hardened encoder a raw square
// is impossible (DC balance); the probe now just forces the quad powered
// and streams D10.2 clock pattern on both lanes - still answers "are the
// lanes electrically alive" on a fast scope.
module transceiver_bank_gowin #(
    parameter TX_PROBE = 0
)(
    input             mgmt_clk,
    // Master control
    input       [1:0] powerup_channel,
    // Output signal control (AUX-requested levels, see header)
    input             preemp_0p0,
    input             preemp_3p5,
    input             preemp_6p0,
    input             swing_0p4,
    input             swing_0p6,
    input             swing_0p8,
    // Status feedback
    output reg  [1:0] tx_running,
    // Reference clocks (refclk1 unused; kept for port compatibility)
    input             refclk0,
    input             refclk1,
    // Symbols to transmit
    output            tx_symbol_clk,
    input      [79:0] tx_symbols,
    // Lanes
    output      [1:0] gtptx_p,
    output      [1:0] gtptx_n,
    // Raw SERDES bring-up status (mgmt_clk domain except where noted):
    // {txfifo_afull, txfifo_full, pll_lock, lane_ok[1:0], ~tx_rst,
    //  tx_running[1:0]} - status only, nothing gates on it.
    output      [7:0] serdes_status,
    // DRP register-dump readback (see reader FSM below): dbg_data/
    // dbg_addr are quasi-static once dbg_done - safe to sample from any
    // clock for a debug UART. Answers whether the bitstream's CSR
    // replay actually landed in the silicon.
    input       [4:0] dbg_idx,
    output     [31:0] dbg_data,
    output     [23:0] dbg_addr,
    output            dbg_done
);

    // ------------------------------------------------------------------
    // Requested drive levels, encoded for a future DRP reconfiguration
    // FSM (TX AFE swing + FFE are DRP-writable)
    // ------------------------------------------------------------------
    wire [1:0] swing_sel  = swing_0p8  ? 2'd2 : swing_0p6  ? 2'd1 : 2'd0;
    wire [1:0] preemp_sel = preemp_6p0 ? 2'd2 : preemp_3p5 ? 2'd1 : 2'd0;

    // ------------------------------------------------------------------
    // Reset / power-up sequencing. Lesson from board #1 Stage 5 (and the
    // Sipeed SFP+ example): release resets STATICALLY on a timer and
    // treat every lock/ready indicator as status only - gating a release
    // on an indicator that itself waits for the release deadlocks.
    // Cadence per the Gowin EDP reference design (rst_gen.v: fabric_rstn
    // released first, tx_rst released tens of ms later; tx_vld tied 1):
    //   powerup -> release fabric_rstn (low-active) immediately ->
    //   release tx_rst (high-active) ~21 ms later (mgmt_clk = 100 MHz).
    // ------------------------------------------------------------------
    reg  fabric_rstn = 1'b0;
    reg  tx_rst      = 1'b1;
    wire pll_lock;
    wire [1:0] lane_ok;

    wire [1:0] powerup_eff = TX_PROBE ? 2'b11 : powerup_channel;

    reg [21:0] seq_count = 0;
    always @(posedge mgmt_clk) begin
        if (powerup_eff == 2'b00) begin
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
            // status only - consumed by nothing critical
            tx_running <= {2{!tx_rst}} & powerup_eff & lane_ok;
        end
    end

    wire fifo_afull_used, fifo_full_used;
    assign serdes_status = {fifo_afull_used, fifo_full_used,
                            pll_lock, lane_ok, ~tx_rst, tx_running};

    // tx_vld (TX buffer read enable, IPUG1043 3.7.1): the reference
    // design ties it permanently high and sequences with the resets
    // alone - mirror that exactly
    wire tx_vld = 1'b1;

    // ------------------------------------------------------------------
    // Per-lane byte/K extraction from the 80-bit symbol bus. Each lane
    // word is {force1, sym1[8:0], force0, sym0[8:0]} with sym = {K,
    // data[7:0]} and sym0 transmitted first. EDP PHY convention: low
    // byte of txdata transmits first, txk bit N flags byte N. The force
    // (disparity) bits [9]/[19] are intentionally dropped - see header.
    // ------------------------------------------------------------------
    localparam [8:0] SYM_D10_2 = 9'b001001010;  // TX_PROBE filler
    wire [8:0] w0s0 = TX_PROBE ? SYM_D10_2 : tx_symbols[8:0];
    wire [8:0] w0s1 = TX_PROBE ? SYM_D10_2 : tx_symbols[18:10];
    wire [8:0] w1s0 = TX_PROBE ? SYM_D10_2 : tx_symbols[28:20];
    wire [8:0] w1s1 = TX_PROBE ? SYM_D10_2 : tx_symbols[38:30];

    // word lane 0 -> logical ln1 (die lane 3 = DP0), word lane 1 ->
    // logical ln0 (die lane 2 = DP1) - see header lane map
    wire [15:0] ln1_txdata = {w0s1[7:0], w0s0[7:0]};
    wire [1:0]  ln1_txk    = {w0s1[8],   w0s0[8]};
    wire [15:0] ln0_txdata = {w1s1[7:0], w1s0[7:0]};
    wire [1:0]  ln0_txk    = {w1s1[8],   w1s0[8]};

`ifdef GOWIN_SERDES_IP
    // ------------------------------------------------------------------
    // IDE-generated SERDES wrapper ("dp_serdes") carrying the EDP PHY
    // protocol module on Q0 lanes 2+3, REFCLK1 @ 135 MHz, 2.7 Gbps,
    // 1:20, hardened 8b10b, TX PN invert both lanes, 804 mV, FFE auto,
    // DRP on. The serial pads are dedicated transceiver bumps - they do
    // not appear as RTL ports. Sidecars dp_serdes.csr/.ipc/serdes.mod
    // must stay with the project.
    // ------------------------------------------------------------------
    wire tx_symbol_clk_raw;

    // DRP readback plumbing (reader FSM below the instantiation)
    wire        drp_clk_w;
    reg  [23:0] drp_addr_r  = 24'd0;
    reg         drp_rden_r  = 1'b0;
    wire        drp_rdvld_w;
    wire [31:0] drp_rddata_w;
    wire        drp_ready_w;

    // por_n: release after power-up request (refclk must be stable; the
    // board's clock generator is programmed before the FPGA runs)
    reg por_n = 1'b0;
    always @(posedge mgmt_clk)
        por_n <= (powerup_eff != 2'b00);

    wire qpll0_ok, qpll1_ok, ln0_cpll_ok, ln1_cpll_ok;
    wire tx_afull_ln0, tx_afull_ln1, tx_full_ln0, tx_full_ln1;
    // whichever PLL the IP config uses asserts its indicator; the rest
    // stay low. OR them so the status word is config-agnostic.
    assign pll_lock = qpll0_ok | qpll1_ok | (ln0_cpll_ok & ln1_cpll_ok);
    assign lane_ok  = {2{pll_lock}};

    dp_serdes i_dp_serdes (
        .por_n_i                     (por_n),
        // logical lane 0 (die lane 2 = DP1) <= word lane 1 (ML1)
        .edp_phy_ln0_fabric_rstn_i   (fabric_rstn),
        .edp_phy_ln0_tx_rst_i        (tx_rst),
        .edp_phy_ln0_fabric_tx_clk_i (tx_symbol_clk),
        .edp_phy_ln0_pcs_tx_clk_o    (tx_symbol_clk_raw),
        .edp_phy_ln0_tx_vld_i        (tx_vld),
        .edp_phy_ln0_txdata_i        (ln0_txdata),
        .edp_phy_ln0_txk_i           (ln0_txk),
        .edp_phy_ln0_txfifo_wrusewd_o(),
        .edp_phy_ln0_txfifo_afull_o  (tx_afull_ln0),
        .edp_phy_ln0_txfifo_full_o   (tx_full_ln0),
        .edp_phy_ln0_cpll_ok_o       (ln0_cpll_ok),
        // lane 0 RX unused (TX-only application)
        .edp_phy_ln0_rx_rst_i        (1'b1),
        .edp_phy_ln0_fabric_rx_clk_i (1'b0),
        .edp_phy_ln0_rxfifo_rden_i   (1'b0),
        .edp_phy_ln0_chbond_start_i  (1'b0),
        .edp_phy_ln0_pma_rx_lock_o   (),
        .edp_phy_ln0_align_link_o    (),
        .edp_phy_ln0_k_lock_o        (),
        .edp_phy_ln0_pcs_rx_clk_o    (),
        .edp_phy_ln0_rxfifo_rdusewd_o(),
        .edp_phy_ln0_rxfifo_aempty_o (),
        .edp_phy_ln0_rxfifo_empty_o  (),
        .edp_phy_ln0_rxdata_o        (),
        .edp_phy_ln0_rxk_o           (),
        // logical lane 1 (die lane 3 = DP0) <= word lane 0 (ML0)
        .edp_phy_ln1_fabric_rstn_i   (fabric_rstn),
        .edp_phy_ln1_tx_rst_i        (tx_rst),
        .edp_phy_ln1_fabric_tx_clk_i (tx_symbol_clk),
        .edp_phy_ln1_pcs_tx_clk_o    (),
        .edp_phy_ln1_tx_vld_i        (tx_vld),
        .edp_phy_ln1_txdata_i        (ln1_txdata),
        .edp_phy_ln1_txk_i           (ln1_txk),
        .edp_phy_ln1_txfifo_wrusewd_o(),
        .edp_phy_ln1_txfifo_afull_o  (tx_afull_ln1),
        .edp_phy_ln1_txfifo_full_o   (tx_full_ln1),
        .edp_phy_ln1_cpll_ok_o       (ln1_cpll_ok),
        // lane 1 RX unused
        .edp_phy_ln1_rx_rst_i        (1'b1),
        .edp_phy_ln1_fabric_rx_clk_i (1'b0),
        .edp_phy_ln1_rxfifo_rden_i   (1'b0),
        .edp_phy_ln1_chbond_start_i  (1'b0),
        .edp_phy_ln1_pma_rx_lock_o   (),
        .edp_phy_ln1_align_link_o    (),
        .edp_phy_ln1_k_lock_o        (),
        .edp_phy_ln1_pcs_rx_clk_o    (),
        .edp_phy_ln1_rxfifo_rdusewd_o(),
        .edp_phy_ln1_rxfifo_aempty_o (),
        .edp_phy_ln1_rxfifo_empty_o  (),
        .edp_phy_ln1_rxdata_o        (),
        .edp_phy_ln1_rxk_o           (),
        // PLL indicators
        .edp_phy_qpll0_ok_o          (qpll0_ok),
        .edp_phy_qpll1_ok_o          (qpll1_ok),
        // DRP: read-only register-dump bridge (writes never enabled)
        .edp_phy_drp_clk_o           (drp_clk_w),
        .edp_phy_drp_addr_i          (drp_addr_r),
        .edp_phy_drp_wren_i          (1'b0),
        .edp_phy_drp_wrdata_i        (32'b0),
        .edp_phy_drp_strb_i          (8'b0),
        .edp_phy_drp_rden_i          (drp_rden_r),
        .edp_phy_drp_ready_o         (drp_ready_w),
        .edp_phy_drp_rdvld_o         (drp_rdvld_w),
        .edp_phy_drp_rddata_o        (drp_rddata_w),
        .edp_phy_drp_resp_o          ()
    );

    assign fifo_afull_used = tx_afull_ln0 | tx_afull_ln1;
    assign fifo_full_used  = tx_full_ln0  | tx_full_ln1;

    assign tx_symbol_clk = tx_symbol_clk_raw;
    // serial data leaves through dedicated pads; these RTL ports idle
    assign gtptx_p = 2'b00;
    assign gtptx_n = 2'b11;

    // ------------------------------------------------------------------
    // DRP register-dump reader (drp_clk_w domain). rden high with addr,
    // hold until rdvld, capture rddata, drop rden the following cycle.
    // Re-reads continuously so the UART always shows fresh values; a
    // read that never returns rdvld is recorded as 0xDEAD_xxxx.
    // Addresses are physical-lane CSR offsets - lanes 2/3 are the same
    // physical lanes as under the Customized PHY, so the map holds.
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
            // lane2 (logical ln0) control/status block
            5'd9:  dump_addr_rom = 24'h809468;
            5'd10: dump_addr_rom = 24'h80946c;
            5'd11: dump_addr_rom = 24'h809400;
            5'd12: dump_addr_rom = 24'h80943c;
            5'd13: dump_addr_rom = 24'h809410;
            5'd14: dump_addr_rom = 24'h809420;
            // lane3 (logical ln1)
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
    // Only the register the UART currently displays is served: dbg_idx
    // (quasi-static, changes every ~0.7 s) is synchronized into the DRP
    // domain and that single address is re-read continuously.
    reg [4:0]  idx_meta = 5'd0, idx_sync = 5'd0;
    reg [31:0] res_data = 32'd0;
    reg [23:0] res_addr = 24'd0;
    reg        res_vld  = 1'b0;
    reg [1:0]  rd_state = 2'd0;          // 0=gap, 1=read, 2=drop
    reg [15:0] rd_tmo   = 16'd0;
    reg [17:0] rd_gap   = 18'd0;         // settle between reads (~ms)

    always @(posedge drp_clk_w) begin
        idx_meta <= dbg_idx;
        idx_sync <= idx_meta;
        case (rd_state)
            2'd0: begin
                drp_rden_r <= 1'b0;
                rd_gap <= rd_gap + 18'd1;
                if (&rd_gap) begin
                    rd_state <= 2'd1;
                    drp_addr_r <= dump_addr_rom(idx_sync);
                    rd_tmo <= 16'd0;
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
            default: begin
                drp_rden_r <= 1'b0;
                rd_gap <= 18'd0;
                rd_state <= 2'd0;
            end
        endcase
    end

    assign dbg_data = res_data;
    assign dbg_addr = res_addr;
    assign dbg_done = res_vld;
`else
    // ------------------------------------------------------------------
    // Behavioural stand-in for simulation and lint: refclk0 must be
    // driven at the word rate (135 MHz for HBR/1:20) by the testbench;
    // lock indicators assert after short fake delays. The fabric 8b10b
    // encoders survive here only so the encoded stream is observable
    // and the sim timing resembles the hardened path.
    // ------------------------------------------------------------------
    assign dbg_data = 32'd0;
    assign dbg_addr = 24'd0;
    assign dbg_done = 1'b0;
    assign tx_symbol_clk = refclk0;

    reg [7:0] fake_lock_cnt = 0;
    always @(posedge mgmt_clk)
        if (!fabric_rstn) fake_lock_cnt <= 0;
        else if (!fake_lock_cnt[7]) fake_lock_cnt <= fake_lock_cnt + 1'b1;
    assign pll_lock = fake_lock_cnt[7];
    assign lane_ok  = {2{fake_lock_cnt[7]}};

    wire [19:0] tx_code0, tx_code1;
    reg         enc_reset = 1'b1;
    reg [1:0]   enc_rst_sync = 2'b11;
    always @(posedge tx_symbol_clk) begin
        enc_rst_sync <= {enc_rst_sync[0], tx_rst};
        enc_reset    <= enc_rst_sync[1];
    end
    lane_encoder_8b10b enc0 (
        .clk(tx_symbol_clk), .reset(enc_reset),
        .tx_symbol(tx_symbols[19:0]),  .tx_code(tx_code0)
    );
    lane_encoder_8b10b enc1 (
        .clk(tx_symbol_clk), .reset(enc_reset),
        .tx_symbol(tx_symbols[39:20]), .tx_code(tx_code1)
    );

    assign fifo_afull_used = 1'b0;
    assign fifo_full_used  = 1'b0;

    // no analogue serialiser in sim: reduce the encoded codes onto the
    // dummy lane pins so the 8b/10b path is not swept by synthesis and
    // gets timed in the viability build; TBs observe tx_code* directly
    assign gtptx_p = {^tx_code1, ^tx_code0};
    assign gtptx_n = ~gtptx_p;
`endif

endmodule
