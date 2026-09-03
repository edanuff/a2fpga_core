///////////////////////////////////////////////////////////////////////////////
// a2mega_dp_test_top.sv — 1.0a3 DisplayPort bring-up top (colorbars)
//
// Phase 2 of the 1.0a3 bring-up plan (docs/BRINGUP_1_0A3_PLAN.md): the
// first bitstream the new board runs. Instantiates the vendored
// dp_transmitter (hdl/displayport) in the exact configuration that was
// timing-closed in DisplayPort_Verilog examples/a2_mega — 1080p59.94,
// 2-lane HBR, SERDES Q0 lanes 2+3 (bonded, master L2) on REFCLK1 — and
// feeds it SMPTE-style colorbars.
//
// The DP main-link lanes and the 135 MHz refclk are dedicated SERDES
// bumps configured inside the generated PHY IP: they never appear here or
// in the .cst. What the fabric pins are: the AUX pseudo-diff pair
// (G15/G16, through the TUSB1046A onto SBU1/2), HPD from the ESP32
// (ESP32_GPIO0, level + 0.75 ms IRQ low pulses reconstructed from PD
// VDMs), the 50 MHz SOM oscillator, the SOM pushbutton as reset, and the
// four carrier LEDs as the bring-up ladder display:
//
//   led[0]  heartbeat (~1.5 Hz)      — bitstream alive
//   led[1]  HPD level                — PD negotiation delivered a sink
//   led[2]  link_established         — AUX/link training succeeded
//   led[3]  video_live               — main stream is being transmitted
//
// (LEDs are active-low on the carrier: anode to +3V3 through R23-R26.)
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module a2mega_dp_test_top #(
    // BLIND=1 builds the open-loop (BLIND_SINK) variant for sinks whose AUX
    // replies the 1.0a3 receiver cannot decode (Fangor, generic dongles,
    // VMM7100 header-only). Kept as a SEPARATE project
    // (a2mega_dp_test_blind.gprj -> a2mega_dp_test_blind_top) per ed
    // 2026-09-02 so the closed-loop test bin stays the default.
    parameter int BLIND = 0
) (
    input  logic clk50_in,          // 50 MHz SOM oscillator (V22)
    input  logic button,            // SOM pushbutton, active low (AB13)

    // DP AUX channel: pseudo-differential on two GPIOs; bias network and
    // AC coupling live on the carrier between here and the TUSB1046A
    inout  wire  dp_aux_p,          // G15 (DPAUX_P, J2.64)
    inout  wire  dp_aux_n,          // G16 (DPAUX_N, J2.62)
    input  logic dp_hpd,            // A19 (ESP32_GPIO0, J2.17)

    output logic [3:0] led,         // D15 D14 R14 P14, active low

    // Debug UART to the ESP32 (H13 -> ESP32 Serial1 -> telnet tee): the
    // FPGA-side status channel that works while the monitor owns USB-C.
    output logic uart_tx
);

    // ------------------------------------------------------------------
    // 100 MHz management/AUX clock from the board oscillator
    // ------------------------------------------------------------------
    logic clk100;
    gowin_mgmt_pll i_mgmt_pll (.lock(), .clkout(clk100), .clkin(clk50_in));

    // ------------------------------------------------------------------
    // AUX analog interface. Two selectable front-ends:
    //
    // AUX_TLVDS=0 (pseudo-diff, proven): two LVCMOS33 pins, ~2 Vpp on
    //   the wire, RX electrically unreachable -> requires BLIND_SINK=1.
    //
    // AUX_TLVDS=1 (closed-loop experiment, 2026-08-14): G15/G16 are the
    //   die's IOR24A/B true differential pair (old TMDS pins), and GW5A
    //   allows true-LVDS (LVDS25) output in a 3.3 V bank (DS Table 2-5).
    //   One TLVDS_IOBUF gives spec-compliant ~350 mV differential TX
    //   (vs our 2 Vpp over-drive — likely what strict sinks want) AND a
    //   real mV-class differential RX, making the sink's replies
    //   readable for the first time -> pair with BLIND_SINK=0 for true
    //   closed-loop training. PULL_MODE UP/DOWN on the pads (cst) sets
    //   the AC-coupled DC point + defined idle polarity.
    // ------------------------------------------------------------------
    localparam AUX_TLVDS = 1;   // experiment ON; set 0 to restore proven path
    localparam AUX_BLIND = (BLIND != 0) ? 1 : ((AUX_TLVDS != 0) ? 0 : 1);

    logic auxch_in, auxch_out, auxch_tri;
    // AUX pad cell EXTRACTED to the shared dp_aux_pad module (08-30) —
    // one source of truth for dp_test AND the full core. The round-by-
    // round battle history (rounds 6-10: ELVDS->TLVDS, pulldowns,
    // counter-park, RX sense) lives in dp_aux_pad.sv.
    dp_aux_pad #(.AUX_TLVDS(AUX_TLVDS)) i_aux_pad (
        .clk100    (clk100),
        .dp_aux_p  (dp_aux_p),
        .dp_aux_n  (dp_aux_n),
        .auxch_in  (auxch_in),
        .auxch_out (auxch_out),
        .auxch_tri (auxch_tri)
    );

    // ------------------------------------------------------------------
    // Colorbars: eight 240-px vertical bars (white, yellow, cyan, green,
    // magenta, red, blue, black) with a 2-px white frame border to make
    // geometry/cropping errors visible. rgb answers the previous cycle's
    // (cx, cy) per the dp_transmitter pull contract.
    // ------------------------------------------------------------------
    logic clk_pixel;
    logic [11:0] cx;
    logic [10:0] cy;
    logic [23:0] rgb;
    logic [23:0] bar;

    always_comb begin
        if      (cx < 12'd240)  bar = 24'hFFFFFF;   // white
        else if (cx < 12'd480)  bar = 24'hFFFF00;   // yellow
        else if (cx < 12'd720)  bar = 24'h00FFFF;   // cyan
        else if (cx < 12'd960)  bar = 24'h00FF00;   // green
        else if (cx < 12'd1200) bar = 24'hFF00FF;   // magenta
        else if (cx < 12'd1440) bar = 24'hFF0000;   // red
        else if (cx < 12'd1680) bar = 24'h0000FF;   // blue
        else                    bar = 24'h000000;   // black
    end

    always_ff @(posedge clk_pixel) begin
        if (cx < 12'd2 || cx >= 12'd1918 || cy < 11'd2 || cy >= 11'd1078)
            rgb <= 24'hFFFFFF;                      // frame border
        else
            rgb <= bar;
    end

    // ------------------------------------------------------------------
    // Audio: 48 kHz strobe with a quiet ~1 kHz sawtooth so the DP audio
    // path is verifiable on the monitor without being obnoxious
    // ------------------------------------------------------------------
    logic        clk_audio;
    logic signed [28:0] aud_acc = -29'sd148_500_000;
    logic [15:0] tone;
    always_ff @(posedge clk_pixel) begin
        clk_audio <= 1'b0;
        if (!aud_acc[28]) begin
            aud_acc   <= aud_acc + 29'sd48_000 - 29'sd148_500_000;
            clk_audio <= 1'b1;
            tone      <= tone + 16'd1365;
        end else
            aud_acc <= aud_acc + 29'sd48_000;
    end
    logic [15:0] audio_sample_word [1:0];
    assign audio_sample_word[0] = {{3{tone[15]}}, tone[15:3]};  // -18 dB
    assign audio_sample_word[1] = {{3{tone[15]}}, tone[15:3]};

    // ------------------------------------------------------------------
    // DisplayPort transmitter — the timing-verified a2_mega configuration
    // ------------------------------------------------------------------
    logic link_established, video_live;
    logic [7:0] debug;
    logic [7:0] serdes_status;
    logic [15:0] aux_dbg_rx;
    logic [7:0]  aux_dbg_gate;
    logic [15:0] aux_dbg_adjust;
    logic [23:0] aux_dbg_chstate;   // raw {0x204, 0x203, 0x202}
    logic [5:0]  aux_dbg_afe;    // M5 applied AFE lane 0 (telemetry M:)
    logic [3:0]  aux_dbg_afe1;   // M5 applied AFE lane 1 (telemetry M1:)
    logic [11:0] aux_dbg_evt;    // M5 {zero_seen, gate_drops, applies} (telemetry N:)
    logic [3:0]  aux_dbg_wdog;   // {cold-restart forcing, attempts[2:0]} (telemetry W:)
    logic [15:0] aux_dbg_tear;   // {first_mask, fail_mask, gate_sat, timeout_sat}
    logic [23:0] aux_dbg_auxerr; // {short, nack, other, obs, kicks, irq} (J:)
    logic [15:0] aux_dbg_esi;    // sticky OR {0x2003, 0x2005} ESI reads (V:)
    logic [6:0]  aux_dbg_defer;  // {edid_giveup, defer_cnt} (U: low 7)
    logic        aux_wedge;      // advisory wedge-suspect (U: bit 7)
    logic [15:0] aux_errcnt0;    // SYMBOL_ERROR_COUNT lane0 (raw, bit15=valid)
    logic [15:0] aux_errcnt1;    // SYMBOL_ERROR_COUNT lane1
    logic [27:0] aux_dbg_errdet; // first teardown {reason, state, exp, rxc} (B:)
    logic [31:0] aux_dbg_snap;   // first-failure snapshot (Z:)
    logic [7:0]  aux_dbg_sink;
    logic [7:0]  aux_dbg_caps;
    // raw {DPCD 0x001 MAX_LINK_RATE, DPCD 0x002 MAX_LANE_COUNT byte} —
    // the adapter census's second gate (X: above is the LOSSY decode)
    logic [15:0] aux_sink_caps;
    logic       hpd_present_w;
    logic [4:0]  drp_idx;
    logic [31:0] drp_data;
    logic [23:0] drp_addr;
    logic        drp_done;

    dp_transmitter #(
        .LANE_COUNT     (2),
        .LINK_RATE_MBPS (2700),
        // 1.0a3: AUX receive is electrically dead (AC caps, no FPGA-side
        // bias, LVCMOS thresholds unreachable by a <=1.38 Vpp reply; board
        // not field-modifiable). TX works — run the link policy open-loop.
        // Remove when a board rev provides a real AUX front-end.
        .BLIND_SINK     (AUX_BLIND),
        // OFF (isolation, 08-21): the default-on HPD-disconnect reset
        // RESET-STORMS on strict sinks that flap HPD/status during hard
        // attaches (Ugreen: Y:FF saturation, C:/A: never accumulate —
        // qualitatively unlike the plow-through ladder's real battles).
        // Very plausibly the actual hybrid-regression mechanism (row 51).
        // Costs the 'r' key until the reset is redesigned (train-phase
        // masked / rate-limited); 'v' still provides re-rolls.
        .HPD_DISCONNECT_RESETS(0),
        // M5 (user-approved 08-21): honor ADJUST_REQUEST via DRP —
        // per-sink swing/pre-emphasis instead of a fixed goldilocks drive.
        // Die-specific: the enable and the DRP lane bases come from the
        // per-project dp_test_die_pkg — this top is shared. BOTH dies are
        // ON with dual-source-verified bases since 08-28 (the earlier
        // 60B-off rule is superseded; see the pkg headers).
        .ENABLE_AFE_ADJUST(dp_test_die_pkg::ENABLE_AFE_ADJUST),
        .AFE_LANE_BASE0   (dp_test_die_pkg::AFE_LANE_BASE0),
        .AFE_LANE_BASE1   (dp_test_die_pkg::AFE_LANE_BASE1),
        // Isolation A/B (row 76): no INIT re-application — trust the boot
        // csr (804/FFE-auto); DRP only fires on a real sink change.
        .AFE_APPLY_ON_START(0),
        // Row 85 placement experiment: gate back ON, placement perturbed
        // via the placer effort switch (identical netlist) to test whether
        // the row-84 regression tracks the gate or the placement.
        .ENABLE_PHASE_DONE_GATE(1),
        // Gate grace period, PRODUCTION (08-24): rides out the converter's
        // ~7 s post-establish status settling (n=2 identical bad cycles:
        // 6 suppressed polls, stable picture throughout) instead of the
        // 6-blink teardown storms. 8 s window; after it, teardown/retrain
        // behavior is unchanged (the 08-18 Anker recovery lesson).
        // ROUND-1 VERDICT (08-25): legacy ladder policy wins on this hub —
        // grace/drain/kick OFF (acquisition_matrix_results.md).
        .GATE_GRACE(0),
        .GATE_KICK(0),
        .LATE_REPLY_DRAIN(0),
        // POLITE-ATTACH BUILD (08-26, from the Mac<->Anker answer key):
        // legacy ladder policy + Mac-parity attach behavior. IRQ_SERVICE=2
        // = ESI servicing (the hub NEVER raises legacy 0x201 — wire-proven;
        // attach-time unconditional 0x2005=02 ack + hpd_irq-triggered
        // ESI block read/clear). POLITE_ATTACH=1 = sink-present gate
        // (paced ~21 ms re-reads while count=0), EDID preamble with paced
        // DEFER retries + give-up budget, lane-set write-on-change (legacy
        // rewrote an unchanged 0x103 383x/attach into the hub's not-ready
        // window), ~1 ms CR poll cadence. Sim: tb_polite_attach (both
        // modes) + full ladder regression PASS. Predecessor 8acfe351
        // (IRQ_SERVICE=1) graded 5/0/0 n=5; its 0x201 path was inert.
        .IRQ_SERVICE(2),
        .POLITE_ATTACH(1),
        .ERRCNT_READ(1),   // symbol-error counters each link check (08-29)
        .TX_PROBE       (0),  // 1 = lane-probe build: raw 4.2 MHz square on
                              // both lanes for AD2 breakout measurement.
                              // Set back to 0 for the real colorbars.
        .H_VISIBLE (1920), .H_TOTAL (2200), .H_SYNC_WIDTH (44), .H_START (192),
        .V_VISIBLE (1080), .V_TOTAL (1125), .V_SYNC_WIDTH (5),  .V_START (41),
        .PIXEL_CLK_MULT (11),
        .PIXEL_CLK_DIV  (10),
        .AUDIO_RATE     (48000),
        .AUDIO_BIT_WIDTH(16)
    ) i_dp (
        .clk100            (clk100),
        .refclk0           (1'b0),      // IP mode: refclk = Q0_REFCLK1 pad
        .refclk1           (1'b0),
        .sim_clk_pixel     (1'b0),
        .reset             (~button),
        .clk_audio         (clk_audio),
        .audio_sample_word (audio_sample_word),
        .clk_pixel         (clk_pixel),
        .rgb               (rgb),
        .cx                (cx),
        .cy                (cy),
        .frame_width(), .screen_width(), .frame_height(), .screen_height(),
        .dp_tx_lane_p      (),          // dedicated SERDES bumps, not fabric
        .dp_tx_lane_n      (),
        .hpd               (dp_hpd),
        .auxch_in          (auxch_in),
        .auxch_out         (auxch_out),
        .auxch_tri         (auxch_tri),
        .link_established  (link_established),
        .video_live        (video_live),
        .debug             (debug),
        .debug_rx       (aux_dbg_rx),
        .debug_gate        (aux_dbg_gate),
        .debug_adjust      (aux_dbg_adjust),
        .debug_chstate     (aux_dbg_chstate),
        .debug_afe         (aux_dbg_afe),
        .debug_afe1        (aux_dbg_afe1),
        .debug_evt         (aux_dbg_evt),
        .debug_wdog        (aux_dbg_wdog),
        .debug_teardown    (aux_dbg_tear),
        .debug_aux_err     (aux_dbg_auxerr),
        .debug_esi         (aux_dbg_esi),
        .debug_defer       (aux_dbg_defer),
        .wedge_suspect     (aux_wedge),
        .debug_errcnt0     (aux_errcnt0),
        .debug_errcnt1     (aux_errcnt1),
        .debug_err_detail  (aux_dbg_errdet),
        .debug_snapshot    (aux_dbg_snap),
        .debug_sink        (aux_dbg_sink),
        .debug_caps        (aux_dbg_caps),
        .debug_sink_caps   (aux_sink_caps),
        .clk_symbol_out    (clk_sym_w),
        .serdes_status     (serdes_status),
        .hpd_present_out   (hpd_present_w),
        .drp_dbg_idx       (drp_idx),
        .drp_dbg_data      (drp_data),
        .drp_dbg_addr      (drp_addr),
        .drp_dbg_done      (drp_done)
    );

    // ------------------------------------------------------------------
    // Debug UART: one ASCII status line ~2x/s at 115200. Everything is
    // sampled loosely into the 50 MHz domain — all quasi-static or
    // diagnostic counters; exactness is not required.
    //   "DP S:xx D:yy F:zz HLV:abc"
    //   S = {pll_lock, lane_ready[1:0], tx_out_of_reset, tx_running[1:0]}
    //   D = dp_transmitter debug byte
    //   F = frame counter (cy wraps, mod 256) — proves pixel pump alive
    //   H/L/V = hpd, link_established, video_live
    // ------------------------------------------------------------------
    logic [7:0] hpd_fall_cnt = '0;
    logic [2:0] hpd_sync = '0;
    always_ff @(posedge clk50_in) begin
        hpd_sync <= {hpd_sync[1:0], dp_hpd};
        if (hpd_sync[2] && !hpd_sync[1])
            hpd_fall_cnt <= hpd_fall_cnt + 8'd1;
    end

    // TLVDS-RX experiment probe: count edges on the differential AUX
    // receive line (auxch_in). Replies from the sink are ~100 edges per
    // transaction; a counter that moves during attach = the ELVDS input
    // path is alive at the pin (then any decode failure is downstream);
    // frozen = the primitive's input path itself is dead. Temporarily
    // displayed in the UART E: field (HPD edge count parked).
    // ...refined: LISTEN-WINDOW edge counter. Count only edges seen while
    // our driver is tri-stated AND >10 us have passed since release, so
    // our own TX and its ring-down can never be counted. Any count here
    // is the sink's reply arriving at the differential receiver output.
    // ...refined again: BURST-RUN discriminator. E: now reports the
    // LONGEST run of consecutive listen-window edges spaced <2 us apart
    // (saturating). Manchester replies are dense runs of ~1 us-spaced
    // edges (a full reply ~100+); pull-decay comparator chatter gives
    // isolated crossings (runs of 1-3). Sticky max, never cleared —
    // one attach tells the story.
    logic [7:0] auxin_edge_cnt = '0;   // now: max fast-edge run length
    logic [2:0] auxin_sync = '0;
    logic [10:0] listen_us = '0;
    logic [7:0] gap_us8 = 8'hFF;       // 100 MHz ticks/2.56us since last edge
    logic [7:0] run_len = '0;
    always_ff @(posedge clk100) begin
        if (!auxch_tri)
            listen_us <= '0;
        else if (listen_us != 11'h7FF)
            listen_us <= listen_us + 11'd1;
        auxin_sync <= {auxin_sync[1:0], auxch_in};
        if (gap_us8 != 8'hFF)
            gap_us8 <= gap_us8 + 8'd1;
        if (!auxch_tri)
            run_len <= '0;
        else if ((listen_us > 11'd1000) && (auxin_sync[2] != auxin_sync[1])) begin
            gap_us8 <= 8'd0;
            // <2 us since previous edge (200 ticks) -> same burst
            run_len <= (gap_us8 < 8'd200 && run_len != 8'hFF) ? run_len + 8'd1 : 8'd1;
        end else if (gap_us8 >= 8'd200)
            run_len <= '0;
        if (run_len > auxin_edge_cnt)
            auxin_edge_cnt <= run_len;
    end

    logic [26:0] c100_cnt = '0;
    always_ff @(posedge clk100)
        c100_cnt <= c100_cnt + 27'd1;

    logic [7:0] frame_cnt = '0;
    logic       cy_msb_d = 1'b0;
    always_ff @(posedge clk_pixel) begin
        cy_msb_d <= cy[10];
        if (cy_msb_d && !cy[10])         // cy wrapped: one frame
            frame_cnt <= frame_cnt + 8'd1;
    end

    // loose 2FF samples into clk50
    logic [7:0] st_s0, st_s;
    logic [7:0] dbg_s0, dbg_s, frm_s0, frm_s;
    logic [2:0] flg_s0, flg_s;
    logic       c100_s0, c100_s;
    logic [16:0] hp_s0, hp_s;
    logic [7:0]  gate_s0, gate_s;
    logic [15:0] adj_s0, adj_s;
    logic [23:0] chst_s0, chst_s;
    logic [27:0] symd_s0, symd_s;
    logic [7:0] snk_s0, snk_s, cap_s0, cap_s;
    logic [15:0] scap_s0, scap_s;  // raw sink caps {0x001, 0x002}
    logic [5:0] afe_s0, afe_s;
    logic [15:0] ec0_s0, ec0_s, ec1_s0, ec1_s;  // symbol-error counters
    logic [3:0] afe1_s0, afe1_s;   // lane 1 {pe, vs}
    logic [11:0] evt_s0, evt_s;    // {zero_seen, gate_drops, applies}
    logic [7:0]  d4_cnt_s0, d4_cnt_s;  // FREE-RUNNING D4 assertion count
    logic [3:0]  wdog_s0, wdog_s;      // watchdog {forcing, attempts}
    logic [15:0] tear_s0, tear_s;      // {first,sticky,gate,timeout}
    logic [23:0] aerr_s0, aerr_s;      // {short, nack, other, obs, kicks, irq}
    logic [15:0] esi_s0, esi_s;        // sticky {0x2003, 0x2005}
    logic [7:0]  defr_s0, defr_s;      // {0, giveup, defer_cnt}
    logic [27:0] edet_s0, edet_s;      // first teardown detail
    logic [31:0] snap_s0, snap_s;      // first-failure snapshot
    // driven by the acquisition-counter block further down; declared here
    // because the telemetry sampler below consumes d4_cnt
    logic        d4_r   = 1'b0;
    logic [7:0]  d4_cnt = 8'd0;
    always_ff @(posedge clk50_in) begin
        st_s0  <= serdes_status;  st_s  <= st_s0;
        dbg_s0 <= debug;          dbg_s <= dbg_s0;
        frm_s0 <= frame_cnt;      frm_s <= frm_s0;
        flg_s0 <= {dp_hpd, link_established, video_live};
        c100_s0 <= c100_cnt[26];  c100_s <= c100_s0;
        hp_s0 <= {hpd_present_w, aux_dbg_rx};  hp_s <= hp_s0;  // E: = {sync,bytes}, R: = last byte
        flg_s  <= flg_s0;
        gate_s0 <= aux_dbg_gate;   gate_s <= gate_s0;
        adj_s0  <= aux_dbg_adjust; adj_s  <= adj_s0;
        chst_s0 <= aux_dbg_chstate; chst_s <= chst_s0;
        // quasi-static: sym_delta only changes once per 1 s window
        symd_s0 <= sym_delta;       symd_s <= symd_s0;
        snk_s0 <= aux_dbg_sink;     snk_s <= snk_s0;
        cap_s0 <= aux_dbg_caps;     cap_s <= cap_s0;
        scap_s0 <= aux_sink_caps;   scap_s <= scap_s0;
        afe_s0 <= aux_dbg_afe;      afe_s <= afe_s0;
        afe1_s0 <= aux_dbg_afe1;    afe1_s <= afe1_s0;
        ec0_s0  <= aux_errcnt0;     ec0_s  <= ec0_s0;
        ec1_s0  <= aux_errcnt1;     ec1_s  <= ec1_s0;
        evt_s0 <= aux_dbg_evt;      evt_s <= evt_s0;
        d4_cnt_s0 <= d4_cnt;        d4_cnt_s <= d4_cnt_s0;
        wdog_s0 <= aux_dbg_wdog;    wdog_s <= wdog_s0;
        tear_s0 <= aux_dbg_tear;    tear_s <= tear_s0;
        aerr_s0 <= aux_dbg_auxerr;  aerr_s <= aerr_s0;
        esi_s0  <= aux_dbg_esi;     esi_s  <= esi_s0;
        defr_s0 <= {aux_wedge, aux_dbg_defer}; defr_s <= defr_s0;
        edet_s0 <= aux_dbg_errdet;  edet_s <= edet_s0;
        snap_s0 <= aux_dbg_snap;    snap_s <= snap_s0;
    end

    // Y: link/video rise odometer — counts every establish (flg_s[1]) and
    // every video start (flg_s[0]) since config, 4-bit saturating each.
    // Catches D4 "flash" events the ~2.5 Hz UART line rate would miss.
    logic [3:0] link_rises = '0, vid_rises = '0;
    logic       link_d = 1'b0, vid_d = 1'b0;
    always_ff @(posedge clk50_in) begin
        link_d <= flg_s[1];
        vid_d  <= flg_s[0];
        if (flg_s[1] && !link_d && link_rises != 4'hF)
            link_rises <= link_rises + 4'd1;
        if (flg_s[0] && !vid_d && vid_rises != 4'hF)
            vid_rises <= vid_rises + 4'd1;
    end

    function automatic [7:0] hexch(input [3:0] n);
        hexch = (n < 4'd10) ? (8'h30 + 8'(n)) : (8'h37 + 8'(n));
    endfunction

    localparam int MSG_LEN = 200;  // six lines; msg_idx is [7:0]
    logic [7:0] msg [0:MSG_LEN-1];
    // DRP register-dump interleave: every message slot alternates between
    // the status line and one "CR ii aaaaaa dddddddd" register line (idx
    // advances per reg line; ~24 regs -> full dump every few seconds).
    // Values are latched at message start so a line is never torn.
    logic        line_is_reg = 1'b0;
    logic [4:0]  drp_idx_q = 5'd0;
    logic [31:0] reg_data_l = '0;
    logic [23:0] reg_addr_l = '0;
    logic        reg_done_l = 1'b0;
    assign drp_idx = drp_idx_q;
    always_comb begin
        if (line_is_reg) begin
            msg[0]="C"; msg[1]="R"; msg[2]=" ";
            msg[3]=hexch({3'b0, drp_idx_q[4]}); msg[4]=hexch(drp_idx_q[3:0]);
            msg[5]=" ";
            msg[6]=hexch(reg_addr_l[23:20]); msg[7]=hexch(reg_addr_l[19:16]);
            msg[8]=hexch(reg_addr_l[15:12]); msg[9]=hexch(reg_addr_l[11:8]);
            msg[10]=hexch(reg_addr_l[7:4]);  msg[11]=hexch(reg_addr_l[3:0]);
            msg[12]=" ";
            msg[13]=hexch(reg_data_l[31:28]); msg[14]=hexch(reg_data_l[27:24]);
            msg[15]=hexch(reg_data_l[23:20]); msg[16]=hexch(reg_data_l[19:16]);
            msg[17]=hexch(reg_data_l[15:12]); msg[18]=hexch(reg_data_l[11:8]);
            msg[19]=hexch(reg_data_l[7:4]);   msg[20]=hexch(reg_data_l[3:0]);
            msg[21]=" "; msg[22]="W";          // W: dump-pass-complete flag
            msg[23]=8'h30 + 8'(reg_done_l);
            msg[24]=" "; msg[25]=" "; msg[26]=" "; msg[27]=" ";
            msg[28]=" "; msg[29]=" "; msg[30]=" "; msg[31]=" ";
            msg[32]=" "; msg[33]=" "; msg[34]=" "; msg[35]=" ";
            msg[36]=8'h0A; msg[37]=" "; msg[38]=" "; msg[39]=" ";
            msg[40]=" "; msg[41]=" "; msg[42]=" "; msg[43]=" ";
            msg[44]=" "; msg[45]=" "; msg[46]=" "; msg[47]=" ";
            msg[48]=" "; msg[49]=" "; msg[50]=" "; msg[51]=" ";
            msg[52]=" "; msg[53]=" "; msg[54]=" "; msg[55]=" ";
            msg[56]=" "; msg[57]=" "; msg[58]=" "; msg[59]=" ";
            msg[60]=" "; msg[61]=" "; msg[62]=" "; msg[63]=" ";
            msg[64]=" "; msg[65]=" "; msg[66]=" "; msg[67]=8'h0A;
            msg[68]=" "; msg[69]=" "; msg[70]=" "; msg[71]=" ";
            msg[72]=" "; msg[73]=" "; msg[74]=" "; msg[75]=" ";
            msg[76]=" "; msg[77]=" "; msg[78]=" "; msg[79]=" ";
            msg[80]=" "; msg[81]=" "; msg[82]=" "; msg[83]=" ";
            msg[84]=" "; msg[85]=" "; msg[86]=" "; msg[87]=" ";
            msg[88]=" "; msg[89]=" "; msg[90]=" "; msg[91]=" ";
            msg[92]=" "; msg[93]=" "; msg[94]=" "; msg[95]=" ";
            msg[96]=" "; msg[97]=" "; msg[98]=" "; msg[99]=" ";
            msg[100]=" "; msg[101]=8'h0A; msg[102]=" "; msg[103]=" ";
            msg[104]=" "; msg[105]=" "; msg[106]=" "; msg[107]=" ";
            msg[108]=" "; msg[109]=" "; msg[110]=" "; msg[111]=" ";
            msg[112]=" "; msg[113]=" "; msg[114]=" "; msg[115]=" ";
            msg[116]=" "; msg[117]=" "; msg[118]=" "; msg[119]=" ";
            msg[120]=" "; msg[121]=" "; msg[122]=" "; msg[123]=" ";
            msg[124]=" "; msg[125]=" "; msg[126]=" "; msg[127]=" ";
            msg[128]=" "; msg[129]=" "; msg[130]=" "; msg[131]=" ";
            msg[132]=" "; msg[133]=" "; msg[134]=" "; msg[135]=" ";
            msg[136]=" "; msg[137]=8'h0A; msg[138]=" "; msg[139]=" ";
            msg[140]=" "; msg[141]=" "; msg[142]=" "; msg[143]=" ";
            msg[144]=" "; msg[145]=" "; msg[146]=" "; msg[147]=" ";
            msg[148]=" "; msg[149]=" "; msg[150]=" "; msg[151]=" ";
            msg[152]=" "; msg[153]=" "; msg[154]=" "; msg[155]=" ";
            msg[156]=" "; msg[157]=" "; msg[158]=" "; msg[159]=" ";
            msg[160]=" "; msg[161]=" "; msg[162]=" "; msg[163]=" ";
            msg[164]=" "; msg[165]=" "; msg[166]=" "; msg[167]=" ";
            msg[168]=" "; msg[169]=" "; msg[170]=8'h0A;
            msg[171]=" "; msg[172]=" "; msg[173]=" "; msg[174]=" ";
            msg[175]=" "; msg[176]=" "; msg[177]=" "; msg[178]=" ";
            msg[179]=" "; msg[180]=" "; msg[181]=" "; msg[182]=" ";
            msg[183]=" "; msg[184]=" "; msg[185]=" "; msg[186]=" ";
            msg[187]=" "; msg[188]=" "; msg[189]=" "; msg[190]=" ";
            msg[191]=" "; msg[192]=" "; msg[193]=" "; msg[194]=" ";
            msg[195]=" "; msg[196]=" "; msg[197]=" "; msg[198]=" ";
            msg[199]=8'h0A;
        end else begin
        // ---------------------------------------------------------------
        // FIVE SHORT LINES, each <= 39 printable chars (39-col console
        // drops a char per longer-line chunk). GENERATED with the idle
        // padding below; keep in sync.
        //   T: {first_fail_mask, sticky_mask, gate_fails, timeouts}
        //   Z: first gate-failure snapshot {0x204,0x203,0x202, seq, tsl}
        //   J: {short, nACK, other, observe_suppressed, kicks, IRQ_SERVICES}
        //   B: first teardown {reason, from_state, expected, rx_count}
        //      reason: 0=untagged 1=short 2=nACK 3=gate 4=timeout 5=retry
        //              6=dark-state kick
        // ---------------------------------------------------------------
        msg[0]="D"; msg[1]="P"; msg[2]=" "; msg[3]="S";
        msg[4]=":"; msg[5]=hexch(st_s[7:4]); msg[6]=hexch(st_s[3:0]); msg[7]=" ";
        msg[8]="D"; msg[9]=":"; msg[10]=hexch(dbg_s[7:4]); msg[11]=hexch(dbg_s[3:0]);
        msg[12]=" "; msg[13]="F"; msg[14]=":"; msg[15]=hexch(frm_s[7:4]);
        msg[16]=hexch(frm_s[3:0]); msg[17]=" "; msg[18]="H"; msg[19]="L";
        msg[20]="V"; msg[21]="C"; msg[22]=":"; msg[23]=8'h30 + 8'(flg_s[2]);
        msg[24]=8'h30 + 8'(flg_s[1]); msg[25]=8'h30 + 8'(flg_s[0]); msg[26]=8'h30 + 8'(c100_s); msg[27]=" ";
        msg[28]="P"; msg[29]=":"; msg[30]=8'h30 + 8'(hp_s[16]); msg[31]=" ";
        msg[32]="E"; msg[33]=":"; msg[34]=hexch(hp_s[7:4]); msg[35]=hexch(hp_s[3:0]);
        msg[36]=8'h0A;
        msg[37]="D"; msg[38]="2"; msg[39]=" "; msg[40]="R";
        msg[41]=":"; msg[42]=hexch(hp_s[15:12]); msg[43]=" "; msg[44]="A";
        msg[45]=":"; msg[46]=hexch(adj_s[15:12]); msg[47]=hexch(adj_s[11:8]); msg[48]=hexch(adj_s[7:4]);
        msg[49]=hexch(adj_s[3:0]); msg[50]=" "; msg[51]="G"; msg[52]=":";
        msg[53]=hexch(gate_s[7:4]); msg[54]=hexch(gate_s[3:0]); msg[55]=" "; msg[56]="Y";
        msg[57]=":"; msg[58]=hexch(link_rises); msg[59]=hexch(vid_rises); msg[60]=" ";
        msg[61]="C"; msg[62]=":"; msg[63]=hexch(chst_s[23:20]); msg[64]=hexch(chst_s[19:16]);
        msg[65]=hexch(chst_s[7:4]); msg[66]=hexch(chst_s[3:0]); msg[67]=8'h0A;
        msg[68]="D"; msg[69]="3"; msg[70]=" "; msg[71]="Q";
        msg[72]=":"; msg[73]=hexch(symd_s[27:24]); msg[74]=hexch(symd_s[23:20]); msg[75]=hexch(symd_s[19:16]);
        msg[76]=hexch(symd_s[15:12]); msg[77]=hexch(symd_s[11:8]); msg[78]=hexch(symd_s[7:4]); msg[79]=hexch(symd_s[3:0]);
        msg[80]=" "; msg[81]="K"; msg[82]=":"; msg[83]=hexch(snk_s[7:4]);
        msg[84]=hexch(snk_s[3:0]); msg[85]=" "; msg[86]="X"; msg[87]=":";
        msg[88]=hexch(cap_s[7:4]); msg[89]=hexch(cap_s[3:0]); msg[90]=" "; msg[91]="W";
        msg[92]=":"; msg[93]=hexch(wdog_s); msg[94]=" "; msg[95]="T";
        msg[96]=":"; msg[97]=hexch(tear_s[15:12]); msg[98]=hexch(tear_s[11:8]); msg[99]=hexch(tear_s[7:4]);
        msg[100]=hexch(tear_s[3:0]); msg[101]=8'h0A;
        msg[102]="D"; msg[103]="4"; msg[104]=" "; msg[105]="M";
        msg[106]=":"; msg[107]=hexch({2'b0, afe_s[5:4]}); msg[108]=hexch(afe_s[3:0]); msg[109]=" ";
        msg[110]="M"; msg[111]="1"; msg[112]=":"; msg[113]=hexch(afe1_s);
        msg[114]=" "; msg[115]="N"; msg[116]=":"; msg[117]=hexch(evt_s[11:8]);
        msg[118]=hexch(evt_s[7:4]); msg[119]=hexch(evt_s[3:0]); msg[120]=" "; msg[121]="L";
        msg[122]=":"; msg[123]=hexch(d4_cnt_s[7:4]); msg[124]=hexch(d4_cnt_s[3:0]);
        // V: sticky ESI vector bytes {0x2003, 0x2005}; U: {giveup, defer_cnt}
        msg[125]=" "; msg[126]="V"; msg[127]=":";
        msg[128]=hexch(esi_s[15:12]); msg[129]=hexch(esi_s[11:8]);
        msg[130]=hexch(esi_s[7:4]);  msg[131]=hexch(esi_s[3:0]);
        msg[132]=" "; msg[133]="U"; msg[134]=":";
        msg[135]=hexch(defr_s[7:4]); msg[136]=hexch(defr_s[3:0]); msg[137]=8'h0A;
        msg[138]="D"; msg[139]="5"; msg[140]=" "; msg[141]="Z";
        msg[142]=":"; msg[143]=hexch(snap_s[31:28]); msg[144]=hexch(snap_s[27:24]); msg[145]=hexch(snap_s[23:20]);
        msg[146]=hexch(snap_s[19:16]); msg[147]=hexch(snap_s[15:12]); msg[148]=hexch(snap_s[11:8]); msg[149]=hexch(snap_s[7:4]);
        msg[150]=hexch(snap_s[3:0]); msg[151]=" "; msg[152]="J"; msg[153]=":";
        msg[154]=hexch(aerr_s[23:20]); msg[155]=hexch(aerr_s[19:16]); msg[156]=hexch(aerr_s[15:12]); msg[157]=hexch(aerr_s[11:8]);
        msg[158]=hexch(aerr_s[7:4]); msg[159]=hexch(aerr_s[3:0]); msg[160]=" "; msg[161]="B";
        msg[162]=":"; msg[163]=hexch(edet_s[27:24]); msg[164]=hexch(edet_s[23:20]); msg[165]=hexch(edet_s[19:16]);
        msg[166]=hexch(edet_s[15:12]); msg[167]=hexch(edet_s[11:8]); msg[168]=hexch(edet_s[7:4]); msg[169]=hexch(edet_s[3:0]);
        msg[170]=8'h0A;
        // D6: SYMBOL_ERROR_COUNT lanes 0/1 (raw DPCD 0x210-0x213; bit15
        // of each = validity — read every periodic link check)
        msg[171]="D"; msg[172]="6"; msg[173]=" ";
        msg[174]="S"; msg[175]="E"; msg[176]="0"; msg[177]=":";
        msg[178]=hexch(ec0_s[15:12]); msg[179]=hexch(ec0_s[11:8]);
        msg[180]=hexch(ec0_s[7:4]);   msg[181]=hexch(ec0_s[3:0]);
        msg[182]=" "; msg[183]="S"; msg[184]="E"; msg[185]="1"; msg[186]=":";
        msg[187]=hexch(ec1_s[15:12]); msg[188]=hexch(ec1_s[11:8]);
        msg[189]=hexch(ec1_s[7:4]);   msg[190]=hexch(ec1_s[3:0]);
        // O: raw sink caps {DPCD 0x001 MAX_LINK_RATE, 0x002 lane byte} —
        // the census second gate (0000 = no DPCD read this HPD session)
        msg[191]=" "; msg[192]="O"; msg[193]=":";
        msg[194]=hexch(scap_s[15:12]); msg[195]=hexch(scap_s[11:8]);
        msg[196]=hexch(scap_s[7:4]);   msg[197]=hexch(scap_s[3:0]);
        msg[198]=" ";
        msg[199]=8'h0A;
        end
    end

    // 115200 baud from 50 MHz (divisor 434); one message per ~0.5 s
    logic [8:0]  baud_cnt = '0;
    logic        baud_tick;
    logic [24:0] msg_timer = '0;
    logic [7:0]  msg_idx = MSG_LEN[7:0];   // idle when == MSG_LEN
    logic [3:0]  bit_idx = '0;
    logic [9:0]  shifter = 10'h3FF;
    always_ff @(posedge clk50_in) begin
        baud_tick <= 1'b0;
        if (baud_cnt == 9'd433) begin baud_cnt <= '0; baud_tick <= 1'b1; end
        else baud_cnt <= baud_cnt + 9'd1;

        msg_timer <= msg_timer + 25'd1;
        if (msg_timer[23:0] == 24'd0 && msg_idx == MSG_LEN[7:0]) begin
            msg_idx <= '0;                 // start a new message
            bit_idx <= 4'd10;              // force reload on next tick
            line_is_reg <= ~line_is_reg;   // alternate status / register
            if (line_is_reg)               // last line was a reg: advance
                drp_idx_q <= (drp_idx_q == 5'd29) ? 5'd0 : drp_idx_q + 5'd1;  // 24-29 = live AFE regs
            reg_data_l <= drp_data;        // latch (quasi-static source)
            reg_addr_l <= drp_addr;
            reg_done_l <= drp_done;
        end

        if (baud_tick && msg_idx != MSG_LEN[7:0]) begin
            if (bit_idx >= 4'd10) begin    // load next char: start+8+stop
                shifter <= {1'b1, msg[msg_idx], 1'b0};
                bit_idx <= 4'd0;
            end else begin
                shifter <= {1'b1, shifter[9:1]};
                bit_idx <= bit_idx + 4'd1;
                if (bit_idx == 4'd9)
                    msg_idx <= msg_idx + 6'd1;
            end
        end
    end
    assign uart_tx = (msg_idx == MSG_LEN[7:0]) ? 1'b1 : shifter[0];

    // ------------------------------------------------------------------
    // Line-rate verification: count clk_sym (the GTR12 TX word clock,
    // line-rate/20) over a 1 s window timed by the 50 MHz crystal. If
    // the QPLL is really running 2.7 Gb/s, the delta is 135 M ± tol.
    // This is the only zero-equipment way to confirm the actual serial
    // rate — with blind training, "link_established" (the old led[2])
    // is self-report and carries no information.
    // ------------------------------------------------------------------
    logic [25:0] win_cnt = '0;          // 0..49,999,999 @ 50 MHz
    logic        win_tgl = 1'b0;
    always_ff @(posedge clk50_in) begin
        if (win_cnt == 26'd49_999_999) begin
            win_cnt <= '0;
            win_tgl <= ~win_tgl;
        end else
            win_cnt <= win_cnt + 26'd1;
    end

    logic [27:0] sym_cnt = '0, sym_last = '0, sym_delta = '0;
    logic [2:0]  tgl_sync = '0;
    logic        freq_ok = 1'b0;
    logic        clk_sym_w;
    always_ff @(posedge clk_sym_w) begin
        sym_cnt  <= sym_cnt + 28'd1;
        tgl_sync <= {tgl_sync[1:0], win_tgl};
        if (tgl_sync[2] != tgl_sync[1]) begin
            sym_delta <= sym_cnt - sym_last;   // registered snapshot
            sym_last  <= sym_cnt;
        end
        // pipelined vs the snapshot: sym_delta is static for a full 1 s
        // window, so comparing it a cycle later costs nothing and keeps
        // the wide subtract-compare off the single-cycle 135 MHz path.
        // 135 M ± ~2%: 132.3M .. 137.7M
        freq_ok <= (sym_delta > 28'd132_300_000) &&
                   (sym_delta < 28'd137_700_000);
    end

    // ------------------------------------------------------------------
    // Bring-up ladder on the LEDs (active low)
    // ------------------------------------------------------------------
    logic [24:0] hb_cnt;
    always_ff @(posedge clk50_in)
        hb_cnt <= hb_cnt + 25'd1;

    // Honest D4 (2026-08-19): on closed-loop builds video_live alone can
    // lie — we can stream into a deaf sink (all-LEDs-lit-but-dark,
    // live-observed). Require the SINK's own lane status to confirm
    // (DPCD 0x202 == 0x77: both lanes CR+EQ+SYM). Blind builds can't
    // know, so they keep the transmit-side meaning.
    logic [15:0] adj_chst_led;
    always_ff @(posedge clk50_in)
        adj_chst_led <= aux_dbg_chstate[15:0];  // [7:0]=0x202 is all D4 uses
    wire sink_confirmed = (AUX_BLIND != 0) ? 1'b1
                                           : (adj_chst_led[7:0] == 8'h77);

    assign led[0] = ~hb_cnt[24];        // heartbeat
    assign led[1] = ~dp_hpd;            // HPD from the ESP32
    assign led[2] = ~freq_ok;           // clk_sym == 135 MHz (line rate OK)
    assign led[3] = ~(video_live && sink_confirmed);  // pixels flowing AND sink locked

    // ------------------------------------------------------------------
    // Acquisition-attempt counter (test log row 86). Blinks-to-lock is the
    // best predictor we have of build reliability, but it is STOCHASTIC
    // run-to-run (one bitstream measured {0,1,1,6,1}), so a comparison
    // needs ~10 samples per build — untenable when each sample costs a
    // physical power cycle. This counts D4's rising edges in fabric so a
    // telnet 'v' draw plus a telemetry read is one unattended sample.
    //   d4_cnt = FREE-RUNNING count of (video_live && sink_confirmed)
    //            rising edges. 8-bit, wraps; the reader takes DELTAS
    //            between telemetry samples.
    // v1 of this counter reset on !dp_hpd and was BROKEN: a failed
    // acquisition usually drops HPD, so the reset wiped the preceding
    // attempts and it read 1 whether the user saw 0 blinks or 1 (measured
    // both ways, test log row 87). Free-running removes any dependence on
    // reset semantics: delta-1 = the number of failed attempts before the
    // lock that held.
    // NOTE: a 'v' virtual replug is NOT identical to a power cycle (the
    // reset scope differs) — calibrate before treating them as equivalent.
    // ------------------------------------------------------------------
    always_ff @(posedge clk50_in) begin
        d4_r <= (video_live && sink_confirmed);
        if ((video_live && sink_confirmed) && !d4_r)
            d4_cnt <= d4_cnt + 8'd1;    // free-running, wraps
    end

endmodule
