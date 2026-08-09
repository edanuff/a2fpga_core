///////////////////////////////////////////////////////////////////////////////
// transceiver_bank_gowin.v : GW5AT (Arora V) SERDES bank for the DP main link
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Drop-in replacement for src/artix7/transceiver_bank.v (same port
// contract) targeting the Gowin GTR12 transceiver quad via the
// IDE-generated "Customized PHY" IP (IPUG1024):
//
//   - line rate 1.62 Gbps (DP RBR), internal width 20, raw mode
//     (hard 8B10B disabled - the GTR12 PCS has no per-character
//     disparity-force input, which DP's TPS2 requires, so 8b/10b is
//     done in fabric by lane_encoder_8b10b)
//   - TX fabric clock: q0_ln0_tx_pcs_clkout_o = line/20 = 81 MHz,
//     shared by both lanes (IPUG1043 3.3.1); this is tx_symbol_clk
//   - parallel data serialises LSB first, matching lane_encoder output
//   - refclk: 135 MHz recommended for the DP rate family (50-800 MHz
//     accepted); must be stable before pma reset release
//   - swing / pre-emphasis: static in the IP config (Vdiffpp, 3-tap
//     FFE); runtime adjustment is possible over the DRP port by
//     replaying IDE-exported .csr write sequences - the AUX-requested
//     levels are exposed on swing_sel/preemp_sel for a future DRP FSM,
//     and DP sinks accept "max swing reached" replies meanwhile
//
// The actual IP instantiation is guarded by GOWIN_SERDES_IP because the
// generated SerDes_Top wrapper and its .toml/.csr sidecars must come
// from the Gowin IP Core Generator (Customized PHY, TX only, QPLL,
// 1.62G, width 20, encoding off, 2 bonded lanes). Without the define, a
// behavioural stub stands in so the full design simulates and lints:
// refclk0 is used directly as the 81 MHz word clock and the reset
// sequencer runs against fake lock/ready timers.
//
// References: IPUG1024E, IPUG1043E, github.com/key2/gowin-serdes,
// github.com/sipeed/TangMega-138KPro-example (sfp+/customized_phy).
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module transceiver_bank_gowin (
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
    output      [1:0] gtptx_n
);

    // ------------------------------------------------------------------
    // Requested drive levels, encoded for a future DRP reconfiguration
    // FSM (IPUG1024 3.10/3.11: TX AFE swing + FFE written over DRP)
    // ------------------------------------------------------------------
    wire [1:0] swing_sel  = swing_0p8  ? 2'd2 : swing_0p6  ? 2'd1 : 2'd0;
    wire [1:0] preemp_sel = preemp_6p0 ? 2'd2 : preemp_3p5 ? 2'd1 : 2'd0;

    // ------------------------------------------------------------------
    // Fabric 8b/10b (raw-mode SERDES): two encoded lanes
    // ------------------------------------------------------------------
    wire [19:0] tx_code0, tx_code1;
    reg         enc_reset = 1'b1;

    lane_encoder_8b10b enc0 (
        .clk(tx_symbol_clk), .reset(enc_reset),
        .tx_symbol(tx_symbols[19:0]),  .tx_code(tx_code0)
    );
    lane_encoder_8b10b enc1 (
        .clk(tx_symbol_clk), .reset(enc_reset),
        .tx_symbol(tx_symbols[39:20]), .tx_code(tx_code1)
    );

    // ------------------------------------------------------------------
    // Reset / power-up sequencing (IPUG1024 4-1 port semantics):
    // refclk stable -> release pma_rstn (low-active) -> wait pll_lock
    // and lane ready -> release pcs_tx_rst (high-active) -> running
    // ------------------------------------------------------------------
    reg  pma_rstn   = 1'b0;
    reg  pcs_tx_rst = 1'b1;
    wire pll_lock;
    wire [1:0] lane_ready;

    reg [15:0] seq_count = 0;
    always @(posedge mgmt_clk) begin
        if (powerup_channel == 2'b00) begin
            pma_rstn   <= 1'b0;
            pcs_tx_rst <= 1'b1;
            seq_count  <= 0;
            tx_running <= 2'b00;
        end else begin
            pma_rstn <= 1'b1;
            if (pll_lock && &lane_ready) begin
                if (seq_count[15])
                    pcs_tx_rst <= 1'b0;
                else
                    seq_count <= seq_count + 1'b1;
            end
            tx_running <= {2{!pcs_tx_rst}} & powerup_channel;
        end
    end

    // release the encoder reset synchronously to the word clock
    reg [1:0] enc_rst_sync = 2'b11;
    always @(posedge tx_symbol_clk) begin
        enc_rst_sync <= {enc_rst_sync[0], pcs_tx_rst};
        enc_reset    <= enc_rst_sync[1];
    end

`ifdef GOWIN_SERDES_IP
    // ------------------------------------------------------------------
    // IDE-generated Customized PHY ("dp_serdes" integration top from the
    // IP Core Generator: dp_phy + upar_arbiter + GTR12 quad). The serial
    // pads are dedicated transceiver bumps - they do not appear as RTL
    // ports. Config: TX only, 2 lanes, QPLL0, 2.7 Gbps, refclk 135 MHz,
    // width 20 raw (8B10B off), TX bonding, 420 mV, FFE flat, DRP on.
    // Sidecars dp_serdes.csr/.ipc/serdes.mod must stay with the project.
    // ------------------------------------------------------------------
    wire tx_symbol_clk_raw;

    // por_n: release after power-up request (refclk must be stable; the
    // board's clock generator is programmed before the FPGA runs)
    reg por_n = 1'b0;
    always @(posedge mgmt_clk)
        por_n <= (powerup_channel != 2'b00);

`ifdef DP_SERDES_LANES_23
    // ------------------------------------------------------------------
    // a2-mega carrier (GW5AT-60 SOM): board routing delivers die lane 3
    // to TUSB1046A DP0 and die lane 2 to DP1, both pairs P/N-inverted
    // (compensated by tx_pol_invert inside the generated IP). Fabric
    // word lane 0 (ML0) therefore drives ln3 and word lane 1 (ML1)
    // drives ln2. Bonding master is lane 2: its PCS clkout and pll_lock
    // serve the bank. IP generated with lanes 2+3, REFCLK1, QPLL0.
    // ------------------------------------------------------------------
    dp_serdes i_dp_serdes (
        .por_n_i                    (por_n),
        // lane 3 TX <= word lane 0 (ML0)
        .dp_phy_q0_ln3_tx_clk_i     (tx_symbol_clk),
        .dp_phy_q0_ln3_tx_pcs_clkout_o (),
        .dp_phy_q0_ln3_tx_data_i    ({60'b0, tx_code0}),
        .dp_phy_q0_ln3_tx_fifo_wren_i (1'b1),
        .dp_phy_q0_ln3_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln3_tx_fifo_afull_o (),
        .dp_phy_q0_ln3_tx_fifo_full_o (),
        .dp_phy_q0_ln3_pma_rstn_i   (pma_rstn),
        .dp_phy_q0_ln3_pcs_tx_rst_i (pcs_tx_rst),
        .dp_phy_q0_ln3_pll_lock_o   (),
        .dp_phy_q0_ln3_ready_o      (lane_ready[0]),
        .dp_phy_q0_ln3_refclk_o     (),
        .dp_phy_q0_ln3_rx_clk_i     (1'b0),
        .dp_phy_q0_ln3_rx_fifo_rden_i (1'b0),
        .dp_phy_q0_ln3_pcs_rx_rst_i (1'b1),
        .dp_phy_q0_ln3_rx_pcs_clkout_o (),
        .dp_phy_q0_ln3_rx_data_o    (),
        .dp_phy_q0_ln3_rx_fifo_rdusewd_o (),
        .dp_phy_q0_ln3_rx_fifo_aempty_o (),
        .dp_phy_q0_ln3_rx_fifo_empty_o (),
        .dp_phy_q0_ln3_rx_valid_o   (),
        .dp_phy_q0_ln3_signal_detect_o (),
        .dp_phy_q0_ln3_rx_cdr_lock_o (),
        // lane 2 TX <= word lane 1 (ML1); bonding master
        .dp_phy_q0_ln2_tx_clk_i     (tx_symbol_clk),
        .dp_phy_q0_ln2_tx_pcs_clkout_o (tx_symbol_clk_raw),
        .dp_phy_q0_ln2_tx_data_i    ({60'b0, tx_code1}),
        .dp_phy_q0_ln2_tx_fifo_wren_i (1'b1),
        .dp_phy_q0_ln2_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln2_tx_fifo_afull_o (),
        .dp_phy_q0_ln2_tx_fifo_full_o (),
        .dp_phy_q0_ln2_pma_rstn_i   (pma_rstn),
        .dp_phy_q0_ln2_pcs_tx_rst_i (pcs_tx_rst),
        .dp_phy_q0_ln2_pll_lock_o   (pll_lock),
        .dp_phy_q0_ln2_ready_o      (lane_ready[1]),
        .dp_phy_q0_ln2_refclk_o     (),
        .dp_phy_q0_ln2_rx_clk_i     (1'b0),
        .dp_phy_q0_ln2_rx_fifo_rden_i (1'b0),
        .dp_phy_q0_ln2_pcs_rx_rst_i (1'b1),
        .dp_phy_q0_ln2_rx_pcs_clkout_o (),
        .dp_phy_q0_ln2_rx_data_o    (),
        .dp_phy_q0_ln2_rx_fifo_rdusewd_o (),
        .dp_phy_q0_ln2_rx_fifo_aempty_o (),
        .dp_phy_q0_ln2_rx_fifo_empty_o (),
        .dp_phy_q0_ln2_rx_valid_o   (),
        .dp_phy_q0_ln2_signal_detect_o (),
        .dp_phy_q0_ln2_rx_cdr_lock_o (),
        // DRP idle (future swing/FFE reconfiguration hook)
        .dp_phy_drp_clk_o           (),
        .dp_phy_drp_addr_i          (24'b0),
        .dp_phy_drp_wren_i          (1'b0),
        .dp_phy_drp_wrdata_i        (32'b0),
        .dp_phy_drp_strb_i          (4'b0),
        .dp_phy_drp_rden_i          (1'b0),
        .dp_phy_drp_ready_o         (),
        .dp_phy_drp_rdvld_o         (),
        .dp_phy_drp_rddata_o        (),
        .dp_phy_drp_resp_o          ()
    );
`else
    dp_serdes i_dp_serdes (
        .por_n_i                    (por_n),
        // lane 0 TX
        .dp_phy_q0_ln0_tx_clk_i     (tx_symbol_clk),
        .dp_phy_q0_ln0_tx_pcs_clkout_o (tx_symbol_clk_raw),
        .dp_phy_q0_ln0_tx_data_i    ({60'b0, tx_code0}),
        .dp_phy_q0_ln0_tx_fifo_wren_i (1'b1),
        .dp_phy_q0_ln0_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln0_tx_fifo_afull_o (),
        .dp_phy_q0_ln0_tx_fifo_full_o (),
        .dp_phy_q0_ln0_pma_rstn_i   (pma_rstn),
        .dp_phy_q0_ln0_pcs_tx_rst_i (pcs_tx_rst),
        .dp_phy_q0_ln0_pll_lock_o   (pll_lock),
        .dp_phy_q0_ln0_ready_o      (lane_ready[0]),
        .dp_phy_q0_ln0_refclk_o     (),
        // lane 0 RX unused (TX-only configuration)
        .dp_phy_q0_ln0_rx_clk_i     (1'b0),
        .dp_phy_q0_ln0_rx_fifo_rden_i (1'b0),
        .dp_phy_q0_ln0_pcs_rx_rst_i (1'b1),
        .dp_phy_q0_ln0_rx_pcs_clkout_o (),
        .dp_phy_q0_ln0_rx_data_o    (),
        .dp_phy_q0_ln0_rx_fifo_rdusewd_o (),
        .dp_phy_q0_ln0_rx_fifo_aempty_o (),
        .dp_phy_q0_ln0_rx_fifo_empty_o (),
        .dp_phy_q0_ln0_rx_valid_o   (),
        .dp_phy_q0_ln0_signal_detect_o (),
        .dp_phy_q0_ln0_rx_cdr_lock_o (),
        // lane 1 TX
        .dp_phy_q0_ln1_tx_clk_i     (tx_symbol_clk),
        .dp_phy_q0_ln1_tx_pcs_clkout_o (),
        .dp_phy_q0_ln1_tx_data_i    ({60'b0, tx_code1}),
        .dp_phy_q0_ln1_tx_fifo_wren_i (1'b1),
        .dp_phy_q0_ln1_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln1_tx_fifo_afull_o (),
        .dp_phy_q0_ln1_tx_fifo_full_o (),
        .dp_phy_q0_ln1_pma_rstn_i   (pma_rstn),
        .dp_phy_q0_ln1_pcs_tx_rst_i (pcs_tx_rst),
        .dp_phy_q0_ln1_pll_lock_o   (),
        .dp_phy_q0_ln1_ready_o      (lane_ready[1]),
        .dp_phy_q0_ln1_refclk_o     (),
        // lane 1 RX unused
        .dp_phy_q0_ln1_rx_clk_i     (1'b0),
        .dp_phy_q0_ln1_rx_fifo_rden_i (1'b0),
        .dp_phy_q0_ln1_pcs_rx_rst_i (1'b1),
        .dp_phy_q0_ln1_rx_pcs_clkout_o (),
        .dp_phy_q0_ln1_rx_data_o    (),
        .dp_phy_q0_ln1_rx_fifo_rdusewd_o (),
        .dp_phy_q0_ln1_rx_fifo_aempty_o (),
        .dp_phy_q0_ln1_rx_fifo_empty_o (),
        .dp_phy_q0_ln1_rx_valid_o   (),
        .dp_phy_q0_ln1_signal_detect_o (),
        .dp_phy_q0_ln1_rx_cdr_lock_o (),
        // DRP idle (future swing/FFE reconfiguration hook)
        .dp_phy_drp_clk_o           (),
        .dp_phy_drp_addr_i          (24'b0),
        .dp_phy_drp_wren_i          (1'b0),
        .dp_phy_drp_wrdata_i        (32'b0),
        .dp_phy_drp_strb_i          (4'b0),
        .dp_phy_drp_rden_i          (1'b0),
        .dp_phy_drp_ready_o         (),
        .dp_phy_drp_rdvld_o         (),
        .dp_phy_drp_rddata_o        (),
        .dp_phy_drp_resp_o          ()
    );
`endif
    assign tx_symbol_clk = tx_symbol_clk_raw;
    // serial data leaves through dedicated pads; these RTL ports idle
    assign gtptx_p = 2'b00;
    assign gtptx_n = 2'b11;
`else
    // ------------------------------------------------------------------
    // Behavioural stand-in for simulation and lint: refclk0 must be
    // driven at the word rate (81 MHz for RBR/20-bit) by the testbench;
    // lock/ready assert after short fake delays.
    // ------------------------------------------------------------------
    assign tx_symbol_clk = refclk0;

    reg [7:0] fake_lock_cnt = 0;
    always @(posedge mgmt_clk)
        if (!pma_rstn) fake_lock_cnt <= 0;
        else if (!fake_lock_cnt[7]) fake_lock_cnt <= fake_lock_cnt + 1'b1;
    assign pll_lock   = fake_lock_cnt[7];
    assign lane_ready = {2{fake_lock_cnt[7]}};

    // no analogue serialiser in sim: reduce the encoded codes onto the
    // dummy lane pins so the 8b/10b path is not swept by synthesis and
    // gets timed in the viability build; TBs observe tx_code* directly
    assign gtptx_p = {^tx_code1, ^tx_code0};
    assign gtptx_n = ~gtptx_p;
`endif

endmodule
