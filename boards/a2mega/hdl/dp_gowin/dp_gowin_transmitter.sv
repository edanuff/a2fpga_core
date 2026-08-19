///////////////////////////////////////////////////////////////////////////////
// dp_gowin_transmitter.sv — DP transmitter built on GOWIN'S OWN IP STACK
// (WS4: alternative to the hand-rolled hdl/displayport core)
//
// Composition (vs dp_transmitter.sv, which this parallels):
//
//   link policy   channel_management  — UNCHANGED (our closed-loop AUX
//                 (hdl/displayport)     ladder: DPCD caps, training,
//                                       retrain, blind fallback)
//   stream side   Gowin EDP Encoder   — REPLACES dp_video_timing +
//                 IP v2.7               pixel_cdc_fifo + packer + MSA +
//                 (edp_encoder/)        main_stream_processing +
//                                       scrambler + skew + fabric 8b10b
//   PHY           Gowin EDP PHY       — REPLACES the Customized-PHY raw
//                 (serdes_edp/ via      -mode emission; hardened PCS
//                 edp_phy_bank)         8b10b, matched to the encoder
//
// The encoder is fed push-style video (vs/hs/de + 2 pixels/clk) in the
// strm domain; it owns the strm->ls CDC through its internal line
// buffer. Synchronous clocking: clk_strm = tx_symbol_clk * 44/5/16 =
// 74.25 MHz (2 px/clk = 148.5 Mpx/s), so MISC0.0=1 and the exact
// Mvid/Nvid pair is supplied (MVID_CUSTOMER).
//
// Feature deltas vs our core (accepted for this evaluation):
//   - no audio (encoder v2.7 has no SDP/audio input ports)
//   - no idle-pattern guarantee between training end and video start
//     (video_data_en follows link_established within a few ls clocks)
//   - no QPLL bad-draw watchdog / CSR replay (the replay ROM is specific
//     to the raw-mode emission's CSR values)
//
// MSA/timing defaults: 1080p (H 1920/2200/44/192, V 1080/1125/5/41),
// positive syncs, RGB 8bpc.
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module dp_gowin_transmitter #(
    parameter LINK_RATE_MBPS = 2700,   // informational; PHY emission is HBR
    parameter BLIND_SINK     = 0,
    parameter H_VISIBLE    = 1920,
    parameter H_TOTAL      = 2200,
    parameter H_SYNC_WIDTH = 44,
    parameter H_START      = 192,
    parameter V_VISIBLE    = 1080,
    parameter V_TOTAL      = 1125,
    parameter V_SYNC_WIDTH = 5,
    parameter V_START      = 41,
    // Mvid/Nvid at the DP convention (f_strm_1px / f_ls_10x scaled to
    // Nvid): 148.5/270 = 11/20 -> 288358/524288 (production N=0x80000)
    parameter MVID = 24'd288358,
    parameter NVID = 24'd524288
)(
    input  logic clk100,           // 100 MHz management clock (AUX timing)

    // Video stream, strm domain (2 pixels per clock, {B,G,R} per pixel)
    output logic clk_strm,         // 74.25 MHz, locked to the link clock
    input  logic pix_vs,           // positive pulse
    input  logic pix_hs,           // positive pulse
    input  logic pix_de,
    input  logic [23:0] pix_data0, // pixel 2N
    input  logic [23:0] pix_data1, // pixel 2N+1

    // Hotplug / AUX channel (front-end pads live in the board top)
    input  logic hpd,
    input  logic auxch_in,
    output logic auxch_out,
    output logic auxch_tri,

    // Status
    output logic link_established,
    output logic video_live,
    output logic [7:0]  debug,
    output logic [15:0] debug_rx,
    output logic [7:0]  debug_gate,
    output logic [7:0]  debug_sink,
    output logic [15:0] debug_adjust,
    output logic [15:0] debug_chstate,
    output logic [7:0]  debug_caps,
    output logic clk_symbol_out,   // 135 MHz link-symbol clock
    output logic [7:0] serdes_status,
    output logic hpd_present_out,

    // DRP register-dump readback for the debug UART
    input  logic [4:0]  drp_dbg_idx,
    output logic [31:0] drp_dbg_data,
    output logic [23:0] drp_dbg_addr,
    output logic        drp_dbg_done
);

    // ------------------------------------------------------------------
    // Link policy (unchanged from the hand-rolled core)
    // ------------------------------------------------------------------
    logic [3:0] tx_powerup_channel;
    logic [3:0] tx_running4;
    logic [1:0] bank_running;
    logic       tx_clock_train, tx_align_train, tx_link_established;
    logic [3:0] debug_locks_nc;

    assign tx_running4      = {2'b00, bank_running};
    assign link_established = tx_link_established;

    channel_management #(.LINK_RATE_MBPS(LINK_RATE_MBPS),
                         .BLIND_SINK(BLIND_SINK)) i_channel_management (
        .clk100               (clk100),
        .debug                (debug),
        .debug_rx             (debug_rx),
        .debug_locks          (debug_locks_nc),
        .debug_gate           (debug_gate),
        .debug_sink           (debug_sink),
        .debug_adjust         (debug_adjust),
        .debug_chstate        (debug_chstate),
        .debug_caps           (debug_caps),
        .hpd                  (hpd),
        .auxch_in             (auxch_in),
        .auxch_out            (auxch_out),
        .auxch_tri            (auxch_tri),
        .stream_channel_count (3'd2),
        .source_channel_count (3'd2),
        .tx_clock_train       (tx_clock_train),
        .tx_align_train       (tx_align_train),
        .tx_powerup_channel   (tx_powerup_channel),
        .tx_preemp_0p0        (),   // static AFE in the PHY emission —
        .tx_preemp_3p5        (),   //  sinks accept "max swing reached"
        .tx_preemp_6p0        (),
        .tx_swing_0p4         (),
        .tx_swing_0p6         (),
        .tx_swing_0p8         (),
        .tx_running           (tx_running4),
        .tx_link_established  (tx_link_established),
        .hpd_present_out      (hpd_present_out)
    );

    // ------------------------------------------------------------------
    // EDP PHY bank (hardened 8b10b, lanes 2/3, see edp_phy_bank.sv)
    // ------------------------------------------------------------------
    logic        tx_symbol_clk, phy_tx_ready;
    logic [15:0] ml0_txdata, ml1_txdata;
    logic [1:0]  ml0_txk, ml1_txk;

    edp_phy_bank i_edp_phy_bank (
        .mgmt_clk        (clk100),
        .powerup_channel (tx_powerup_channel[1:0]),
        .tx_running      (bank_running),
        .tx_symbol_clk   (tx_symbol_clk),
        .tx_ready        (phy_tx_ready),
        .ml0_txdata      (ml0_txdata),
        .ml0_txk         (ml0_txk),
        .ml1_txdata      (ml1_txdata),
        .ml1_txk         (ml1_txk),
        .serdes_status   (serdes_status),
        .dbg_idx         (drp_dbg_idx),
        .dbg_data        (drp_dbg_data),
        .dbg_addr        (drp_dbg_addr),
        .dbg_done        (drp_dbg_done)
    );
    assign clk_symbol_out = tx_symbol_clk;

    // ------------------------------------------------------------------
    // Stream clock: 135 MHz * 44/5 = 1188 MHz VCO, /16 = 74.25 MHz
    // (2 px/clk = 148.5 Mpx/s, locked to the link clock -> MISC0.0=1)
    // ------------------------------------------------------------------
    gowin_pixel_pll #(.ODIV0(16), .IDIV(5), .MDIV(44), .FCLKIN("135"))
    i_strm_pll (
        .lock   (),
        .clkout (clk_strm),
        .clkin  (tx_symbol_clk)
    );

    // ------------------------------------------------------------------
    // Policy -> encoder control CDC (clk100 -> ls domain, 2FF; all
    // quasi-static levels)
    // ------------------------------------------------------------------
    logic [2:0] tps1_sync = '0, tps2_sync = '0, est_sync = '0;
    always_ff @(posedge tx_symbol_clk) begin
        tps1_sync <= {tps1_sync[1:0], tx_clock_train};
        tps2_sync <= {tps2_sync[1:0], tx_align_train};
        est_sync  <= {est_sync[1:0],  tx_link_established};
    end
    assign video_live = tx_link_established;

    // ------------------------------------------------------------------
    // Gowin EDP Encoder IP v2.7 (2 lanes, 1:20, 24 bpp, coef 1;
    // configuration in edp_encoder/edp_encoder_defines.v)
    // ------------------------------------------------------------------
    EDP_Encoder_Top i_edp_encoder (
        .I_rst_n            (phy_tx_ready),
        .I_tx_tps1_en       (tps1_sync[2]),
        .I_tx_tps2_en       (tps2_sync[2]),
        .I_tx_video_data_en (est_sync[2]),
        .I_frame_type       (1'b0),        // default framing (DPCD 0x101=0x02)
        .I_scrm_value       (1'b0),        // DP scrambler reset 0xFFFF
        .I_vb_id            (8'h10),      // AudioMute_Flag: no audio stream
                                          // (matches Gowin EDP refdesign's
                                          // no-audio contract; was 8'h00)
        .I_msa_mvid         (MVID),
        .I_msa_nvid         (NVID),
        .I_msa_htotal       (16'(H_TOTAL)),
        .I_msa_vtotal       (16'(V_TOTAL)),
        .I_msa_hstart       (16'(H_START)),
        .I_msa_vstart       (16'(V_START)),
        .I_msa_hwidth       (16'(H_VISIBLE)),
        .I_msa_vheight      (16'(V_VISIBLE)),
        .I_msa_hsphsw       ({1'b0, 15'(H_SYNC_WIDTH)}),  // positive sync
        .I_msa_vspvsw       ({1'b0, 15'(V_SYNC_WIDTH)}),
        .I_msa_misc0        (8'h21),       // RGB 8bpc, sync clocking
        .I_msa_misc1        (8'h00),
        .I_strm_clk         (clk_strm),
        .I_pix_vs           (pix_vs),
        .I_pix_hs           (pix_hs),
        .I_pix_de           (pix_de),
        .I_ls_clk           (tx_symbol_clk),
        .I_pix_data0        (pix_data0),
        .I_pix_data1        (pix_data1),
        .O_tx_data_0        (ml0_txdata),
        .O_tx_data_1        (ml1_txdata),
        .O_k_code_0         (ml0_txk),
        .O_k_code_1         (ml1_txk)
    );

endmodule
