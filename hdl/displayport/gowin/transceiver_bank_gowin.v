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

// TX_PROBE: bring-up aid. Forces the quad powered (no link-policy
// dependency) and replaces the 8b10b stream with a raw 16-words-low /
// 16-words-high pattern = line-rate/640 ~= 4.2 MHz square wave on both
// lanes — visible on a basic scope at a connector breakout. Answers
// "are the lanes electrically alive, on which pins, with which
// polarity" without any DP protocol in the loop.
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
    // {pll_lock, lane_ready[1:0], ~pcs_tx_rst, tx_running[1:0]} — the
    // signals the reset sequencer gates on, never before observable.
    output      [7:0] serdes_status,
    // DRP register-dump readback (IPUG1024 3.11): a background FSM
    // reads a curated list of CSR addresses over the DRP and latches
    // the results. dbg_data/dbg_addr are quasi-static once dbg_done —
    // safe to sample from any clock for a debug UART. Answers whether
    // the bitstream's CSR replay actually landed in the silicon.
    input       [4:0] dbg_idx,
    output     [31:0] dbg_data,
    output     [23:0] dbg_addr,
    output            dbg_done
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

    // ROOT-CAUSE FIX (board #1, 2026-08-12): the original sequencer waited
    // for pll_lock && lane_ready before releasing pcs_tx_rst — but the
    // GTR12's ready_o does not assert while PCS is held in reset, so the
    // transmitter deadlocked in reset FOREVER (live status readback:
    // pll_lock=1, lane_ready=00, pcs_tx_rst=1 — the lanes never carried a
    // bit while every upstream indicator looked healthy). The working
    // Sipeed SFP+ example releases both resets STATICALLY and treats
    // ready_o as status only. Mirror that: release pma at powerup, release
    // pcs_tx_rst a short settle later, gate on nothing.
    wire [1:0] powerup_eff = TX_PROBE ? 2'b11 : powerup_channel;

    // probe pattern generator (tx_symbol_clk domain): 128-word frame,
    // 96 words high / 32 low = ~1.05 MHz square at 75% duty. The low
    // rate suits flywire scope probes; the ASYMMETRIC duty reads out
    // polarity directly (P-leg: 75% high; an inverted lane: 25%).
    reg [6:0] probe_cnt = 7'd0;
    always @(posedge tx_symbol_clk)
        probe_cnt <= probe_cnt + 7'd1;
    wire [19:0] probe_word = (probe_cnt < 7'd96) ? 20'hFFFFF : 20'h00000;

    reg [15:0] seq_count = 0;
    always @(posedge mgmt_clk) begin
        if (powerup_eff == 2'b00) begin
            pma_rstn   <= 1'b0;
            pcs_tx_rst <= 1'b1;
            seq_count  <= 0;
            tx_running <= 2'b00;
        end else begin
            pma_rstn <= 1'b1;
            if (seq_count[15])
                pcs_tx_rst <= 1'b0;      // ~330 us after powerup; ungated
            else
                seq_count <= seq_count + 1'b1;
            // status only — consumed by nothing critical
            tx_running <= {2{!pcs_tx_rst}} & powerup_eff & lane_ready;
        end
    end

    assign serdes_status = {fifo_afull_used, fifo_full_used,
                            pll_lock, lane_ready, ~pcs_tx_rst, tx_running};

    // release the encoder reset synchronously to the word clock
    reg [1:0] enc_rst_sync = 2'b11;
    always @(posedge tx_symbol_clk) begin
        enc_rst_sync <= {enc_rst_sync[0], pcs_tx_rst};
        enc_reset    <= enc_rst_sync[1];
    end

    wire tx_afull_ln0, tx_afull_ln1, tx_afull_ln2, tx_afull_ln3;
    wire tx_full_ln0,  tx_full_ln1,  tx_full_ln2,  tx_full_ln3;
`ifdef DP_SERDES_LANES_23
    wire fifo_afull_used = tx_afull_ln2 | tx_afull_ln3;
    wire fifo_full_used  = tx_full_ln2  | tx_full_ln3;
`else
    wire fifo_afull_used = tx_afull_ln0 | tx_afull_ln1;
    wire fifo_full_used  = tx_full_ln0  | tx_full_ln1;
`endif

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

    // DRP readback plumbing (reader FSM below the instantiations)
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

`ifdef DP_SERDES_LANES_23
    // ------------------------------------------------------------------
    // a2-mega carrier (GW5AT-60 SOM): board routing delivers die lane 3
    // to TUSB1046A DP0 and die lane 2 to DP1, both pairs P/N-inverted
    // (compensated by tx_pol_invert inside the generated IP). Fabric
    // word lane 0 (ML0) therefore drives ln3 and word lane 1 (ML1)
    // drives ln2. Bonding master is lane 2: its PCS clkout and pll_lock
    // serve the bank. IP generated with lanes 2+3, REFCLK1, QPLL0.
    // ------------------------------------------------------------------
    // A/B EXPERIMENT (board #1 Stage 5): serializer word convention. The
    // GTR12's wire-order for the 20-bit word (two 10-bit 8b10b symbols)
    // was never verified on silicon. A wrong convention lets TPS1
    // (alternating, order-invariant) give sinks CR lock while
    // TPS2/symbols garble — matching the observed trained-but-no-signal.
    //   0 = straight (documented LSB-first assumption)   — TESTED: no lock
    //   1 = full 20-bit reversal (= symbol swap + per-symbol reverse)
    //                                                    — TESTED: no lock
    //   2 = symbol swap only ({sym0, sym1})              — untested
    //   3 = per-symbol bit reverse, order kept           — untested
    // Flip and rebuild to A/B; fold the winner in and delete when settled.
    localparam TX_WORD_MODE = 0; // canonical for probe
    function [9:0] rev10(input [9:0] x);
        integer ri;
        for (ri = 0; ri < 10; ri = ri + 1)
            rev10[ri] = x[9 - ri];
    endfunction
    function [19:0] word_conv(input [19:0] x);
        case (TX_WORD_MODE)
            1: word_conv = {rev10(x[9:0]), rev10(x[19:10])};  // full reversal
            2: word_conv = {x[9:0], x[19:10]};                // symbol swap
            3: word_conv = {rev10(x[19:10]), rev10(x[9:0])};  // per-symbol rev
            default: word_conv = x;
        endcase
    endfunction
    wire [19:0] tx_wire0 = TX_PROBE ? probe_word : word_conv(tx_code0);
    wire [19:0] tx_wire1 = TX_PROBE ? probe_word : word_conv(tx_code1);

    dp_serdes i_dp_serdes (
        .por_n_i                    (por_n),
        // lane 3 TX <= word lane 0 (ML0)
        .dp_phy_q0_ln3_tx_clk_i     (tx_symbol_clk),
        .dp_phy_q0_ln3_tx_pcs_clkout_o (),
        .dp_phy_q0_ln3_tx_data_i    ({4{tx_wire0}}),
        .dp_phy_q0_ln3_tx_fifo_wren_i (~tx_afull_ln3),
        .dp_phy_q0_ln3_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln3_tx_fifo_afull_o (tx_afull_ln3),
        .dp_phy_q0_ln3_tx_fifo_full_o (tx_full_ln3),
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
        .dp_phy_q0_ln2_tx_data_i    ({4{tx_wire1}}),
        .dp_phy_q0_ln2_tx_fifo_wren_i (~tx_afull_ln2),
        .dp_phy_q0_ln2_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln2_tx_fifo_afull_o (tx_afull_ln2),
        .dp_phy_q0_ln2_tx_fifo_full_o (tx_full_ln2),
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
        // DRP: read-only register-dump bridge (writes never enabled)
        .dp_phy_drp_clk_o           (drp_clk_w),
        .dp_phy_drp_addr_i          (drp_addr_r),
        .dp_phy_drp_wren_i          (1'b0),
        .dp_phy_drp_wrdata_i        (32'b0),
        .dp_phy_drp_strb_i          (8'b0),
        .dp_phy_drp_rden_i          (drp_rden_r),
        .dp_phy_drp_ready_o         (drp_ready_w),
        .dp_phy_drp_rdvld_o         (drp_rdvld_w),
        .dp_phy_drp_rddata_o        (drp_rddata_w),
        .dp_phy_drp_resp_o          ()
    );
`else
    dp_serdes i_dp_serdes (
        .por_n_i                    (por_n),
        // lane 0 TX
        .dp_phy_q0_ln0_tx_clk_i     (tx_symbol_clk),
        .dp_phy_q0_ln0_tx_pcs_clkout_o (tx_symbol_clk_raw),
        .dp_phy_q0_ln0_tx_data_i    ({4{tx_wire0}}),
        .dp_phy_q0_ln0_tx_fifo_wren_i (~tx_afull_ln0),
        .dp_phy_q0_ln0_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln0_tx_fifo_afull_o (tx_afull_ln0),
        .dp_phy_q0_ln0_tx_fifo_full_o (tx_full_ln0),
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
        .dp_phy_q0_ln1_tx_data_i    ({4{tx_wire1}}),
        .dp_phy_q0_ln1_tx_fifo_wren_i (~tx_afull_ln1),
        .dp_phy_q0_ln1_tx_fifo_wrusewd_o (),
        .dp_phy_q0_ln1_tx_fifo_afull_o (tx_afull_ln1),
        .dp_phy_q0_ln1_tx_fifo_full_o (tx_full_ln1),
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
        // DRP: read-only register-dump bridge (writes never enabled)
        .dp_phy_drp_clk_o           (drp_clk_w),
        .dp_phy_drp_addr_i          (drp_addr_r),
        .dp_phy_drp_wren_i          (1'b0),
        .dp_phy_drp_wrdata_i        (32'b0),
        .dp_phy_drp_strb_i          (8'b0),
        .dp_phy_drp_rden_i          (drp_rden_r),
        .dp_phy_drp_ready_o         (drp_ready_w),
        .dp_phy_drp_rdvld_o         (drp_rdvld_w),
        .dp_phy_drp_rddata_o        (drp_rddata_w),
        .dp_phy_drp_resp_o          ()
    );
`endif
    assign tx_symbol_clk = tx_symbol_clk_raw;
    // serial data leaves through dedicated pads; these RTL ports idle
    assign gtptx_p = 2'b00;
    assign gtptx_n = 2'b11;

    // ------------------------------------------------------------------
    // DRP register-dump reader (drp_clk_w domain). Read timing per
    // IPUG1024 3.11: rden high with addr, hold until rdvld, capture
    // rddata, drop rden the following cycle. Re-dumps continuously
    // (~every 0.3 s) so the UART always shows fresh values; a read that
    // never returns rdvld times out and is recorded as 0xDEAD_xxxx.
    // ------------------------------------------------------------------
    function [23:0] dump_addr_rom(input [4:0] i);
        case (i)
            // the 7 writes the generator emits BEFORE the 0xb00000 key
            5'd0:  dump_addr_rom = 24'h8081a4;
            5'd1:  dump_addr_rom = 24'h8081a8;
            5'd2:  dump_addr_rom = 24'h808758;
            5'd3:  dump_addr_rom = 24'h808284;
            5'd4:  dump_addr_rom = 24'h808384;
            5'd5:  dump_addr_rom = 24'h808484;
            5'd6:  dump_addr_rom = 24'h808584;
            // common-block deltas vs tang_mega
            5'd7:  dump_addr_rom = 24'h808760;
            5'd8:  dump_addr_rom = 24'h800b91;
            // lane2 (active; bonding master) control/status block
            5'd9:  dump_addr_rom = 24'h809468;
            5'd10: dump_addr_rom = 24'h80946c;
            5'd11: dump_addr_rom = 24'h809400;
            5'd12: dump_addr_rom = 24'h80943c;
            5'd13: dump_addr_rom = 24'h809410;
            5'd14: dump_addr_rom = 24'h809420;
            // lane3 (active)
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
    // domain and that single address is re-read continuously. One result
    // register instead of a full dump RAM — negligible fabric footprint.
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
    // driven at the word rate (81 MHz for RBR/20-bit) by the testbench;
    // lock/ready assert after short fake delays.
    // ------------------------------------------------------------------
    assign dbg_data = 32'd0;
    assign dbg_addr = 24'd0;
    assign dbg_done = 1'b0;
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
