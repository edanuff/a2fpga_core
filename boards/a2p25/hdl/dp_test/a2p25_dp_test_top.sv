///////////////////////////////////////////////////////////////////////////////
// a2p25_dp_test_top.sv — a2-p25 (displayport1 rev) DisplayPort bring-up top
//
// First-light bitstream for the DP-over-MIPI-D-PHY architecture
// (boards/a2p25/docs/DP_ALTMODE_DPHY_DESIGN.md): colorbars + a quiet test
// tone through the a2mega DP link layer (hdl/displayport, unchanged except
// the DP_VENDOR_DPHY export), serialized by the GW5A-25 hardened D-PHY
// (transceiver_bank_dphy). Nothing Apple-bus, PSRAM or ESP32-link related
// is instantiated — this is the smallest design that answers "does the
// D-PHY-as-DP-serializer chain train and show a picture", and the AUX
// telemetry needed to validate the relocated AUX front-end before the
// a2mega board turn.
//
// Default configuration: 1280x720p60, RBR (1.62 Gbps) x 2 lanes — fits
// every sink class we have (2-lane hub converters included). DP lanes 0/1
// leave on D-PHY lanes 3/2 (board routing reversed; see
// transceiver_bank_dphy). a2p25_dp_test_4l.gprj defines
// DP_TEST_4LANE_1080P: 1920x1080p60 on RBR x 4 (148.5 MHz pixel clock,
// 69% of link capacity) — needs a 4-lane sink path (pin assignment C/E
// AND a 4-lane converter or native DP).
//
// Bring-up-only pin repurposing (this bitstream only — the production core
// uses these pins for the ESP32 OPI link):
//   dp_hpd  <- B2  ESP32_INT_N   ESP32 drives HPD level + 0.75 ms IRQ low
//                                pulses reconstructed from PD VDMs (a2mega
//                                firmware model, GPIO10 on this board)
//   uart_tx -> C2  ESP32_OPI_D0  115200 8N1 telemetry; ESP32 routes a UART
//                                RX to GPIO11 and tees it to its console
//
// LED (B10, active high): heartbeat, ~1.5 Hz, toggling in every state
// (a2mega led[0] parity) — a dark or solid LED means the bitstream/clock is
// NOT running. Link state lives in the telemetry (P1 HLV:) and the ESP32's
// LED1 (HPD level).
//
// Telemetry lines (every ~0.34 s): see the a2mega dp_test field legend
// (boards/a2mega/hdl/dp_test/a2mega_dp_test_top.sv) — same letters; S: is
// replaced by the D-PHY status byte and Q: by the measured symbol clock
// (should read 0x04D3F640 = 81,000,000 per second).
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

`ifdef DP_TEST_4LANE_1080P
 `define DPT_LANES 4
`else
 `define DPT_LANES 2
`endif
`ifdef DP_TEST_PROBE
 `define DPT_PROBE 1
`else
 `define DPT_PROBE 0
`endif
`ifdef DP_TEST_LANESWAP
 `define DPT_LANE_MAP 8'h4E      // D-PHY 3<-DP1, 2<-DP0, 1<-DP3, 0<-DP2
`else
 `define DPT_LANE_MAP 8'h00      // REVERSE_LANES (netlist reading)
`endif
`ifdef DP_TEST_POLINV
 `define DPT_POL_INV 4'hF
`else
 `define DPT_POL_INV 4'h0
`endif
`ifdef DP_TEST_BLIND
 `define DPT_BLIND 1
`else
 `define DPT_BLIND 0
`endif
`ifdef DP_TEST_AUXOBS
 `define DPT_LINK_CHECK 2       // observe: periodic checks, count, never tear down
`else
 `define DPT_LINK_CHECK 0       // hold the link once trained
`endif

module a2p25_dp_test_top #(
    // Selected per project (.gprj defines file): 2 lanes = 720p60,
    // 4 lanes = 1080p60. PROBE = raw square wave on all lanes (scope).
    parameter int LANES = `DPT_LANES,
    parameter int PROBE = `DPT_PROBE
) (
    input  logic clk50_in,          // E2, SOM 50 MHz oscillator

    inout  wire  dp_aux_p,          // L2 (IOR18A) — TLVDS pair with L1
    inout  wire  dp_aux_n,          // L1 (IOR18B)
    input  logic dp_hpd,            // B2 (ESP32_INT_N, repurposed)

    output logic led,               // B10, active high
    output logic uart_tx,           // C2 (ESP32_OPI_D0, repurposed)

    // MIPI D-PHY M0 quad (dedicated pads)
    inout  wire  dphy_ck_p,
    inout  wire  dphy_ck_n,
    inout  wire  [3:0] dphy_d_p,
    inout  wire  [3:0] dphy_d_n
);

    // ------------------------------------------------------------------
    // Clocks and reset
    // ------------------------------------------------------------------
    logic clk100, clk54, pll_lock;
    gowin_mgmt_pll i_mgmt_pll (.lock(), .clkout(clk100), .clkin(clk50_in));

    Gowin_PLL i_sys_pll (
        .clkin   (clk50_in),
        .mdclk   (clk50_in),
        .clkout0 (),                 // 27 MHz
        .clkout1 (),                 // 135 MHz
        .clkout2 (clk54),            // 54 MHz -> D-PHY PLL reference
        .lock    (pll_lock)
    );

    logic [19:0] por_cnt = '0;
    logic        por_done = 1'b0;
    always_ff @(posedge clk50_in) begin
        if (!pll_lock) begin
            por_cnt  <= '0;
            por_done <= 1'b0;
        end else if (por_cnt != 20'hFFFFF) begin
            por_cnt <= por_cnt + 20'd1;
        end else begin
            por_done <= 1'b1;
        end
    end
    wire reset = !por_done;

    // ------------------------------------------------------------------
    // AUX pad (shared with a2mega — the cell under validation)
    // ------------------------------------------------------------------
    logic auxch_in, auxch_out, auxch_tri;
    dp_aux_pad #(.AUX_TLVDS(1)) i_aux_pad (
        .clk100    (clk100),
        .dp_aux_p  (dp_aux_p),
        .dp_aux_n  (dp_aux_n),
        .auxch_in  (auxch_in),
        .auxch_out (auxch_out),
        .auxch_tri (auxch_tri)
    );

    // ------------------------------------------------------------------
    // AUX reply capture (bring-up instrument, telemetry lines P7 and PA).
    // After each AUX transmit ends (auxch_tri rises) record: the level at
    // release, the gap to the first RX edge (G:, 10 ns ticks, 16 bit) and
    // the next 48 edge-to-edge intervals (8 bit, saturating; 50 = one
    // 0.5 us Manchester half-bit, 100 = full bit, ~200 = SYNC-END pulse).
    // Window: 600 us without an edge ends a capture. Every transaction is
    // captured; the last complete one is kept.
    //   P7: the last complete capture, republished every ~2.7 s.
    //   PA: the capture of the last FAILING established-state reply
    //       (LINK_CHECK=2 observe counters: S short, N non-ACK, D DEFER,
    //       T timeout). A timeout is flagged ~20 ms after its (empty)
    //       capture completed, so an idle capture engine publishes the
    //       kept copy; one still in progress publishes when it finishes.
    // ------------------------------------------------------------------
    logic [103:0] aux_dbg_obs;            // dp_transmitter observe counters
    localparam int CAP_N = 48;
    logic [2:0]  cap_rx_s = '0;
    logic        cap_tri_d = 1'b1;
    logic [1:0]  cap_st = 2'd0;           // 0 armed, 1 wait first edge, 2 record, 3 done
    logic [15:0] cap_t = '0;
    logic [15:0] cap_gap = '0;
    logic [5:0]  cap_n = '0;
    logic        cap_lvl = 1'b0;
    logic [7:0]  cap_iv [0:CAP_N-1];
    logic [27:0] cap_rearm = '0;
    logic        per_due = 1'b0;
    // last complete capture
    logic [7:0]  lst_iv [0:CAP_N-1];
    logic [15:0] lst_gap = '0;
    logic [5:0]  lst_n = '0;
    logic        lst_lvl = 1'b0;
    // published copies (quasi-static, read by the clk50 telemetry)
    logic [7:0]  pub_iv [0:CAP_N-1];
    logic [15:0] pub_gap = '0;
    logic [5:0]  pub_n = '0;
    logic        pub_lvl = 1'b0;
    logic [7:0]  pub_seq = '0;
    logic [7:0]  fpub_iv [0:CAP_N-1];
    logic [15:0] fpub_gap = '0;
    logic [5:0]  fpub_n = '0;
    logic        fpub_lvl = 1'b0;
    logic [7:0]  fpub_seq = '0;
    logic [7:0]  fpub_rsn = "-";
    // failure events from the observe counters (same clk100 domain)
    logic [63:0] obs_d = '0;              // {short, nack, defer, timeout}
    logic        f_pend = 1'b0;
    logic [7:0]  f_rsn = "-";
    wire  [63:0] obs_now = aux_dbg_obs[79:16];
    // change flags registered (timing: one compare per clock, then decode)
    logic [3:0]  f_chg = '0;              // {short, nack, defer, timeout}
    wire         f_evt = |f_chg;
    wire  [7:0]  f_evt_rsn = f_chg[3] ? "S" : f_chg[2] ? "N" : f_chg[1] ? "D" : "T";
    always_ff @(posedge clk100) begin
        cap_rx_s  <= {cap_rx_s[1:0], auxch_in};
        cap_tri_d <= auxch_tri;
        cap_rearm <= cap_rearm + 28'd1;
        if (cap_rearm == '0) per_due <= 1'b1;
        obs_d <= obs_now;
        f_chg <= {obs_now[63:48] != obs_d[63:48], obs_now[47:32] != obs_d[47:32],
                  obs_now[31:16] != obs_d[31:16], obs_now[15:0]  != obs_d[15:0]};
        if (f_evt) begin
            if (cap_st == 2'd0) begin             // idle: publish the kept copy
                for (int i = 0; i < CAP_N; i++) fpub_iv[i] <= lst_iv[i];
                fpub_gap <= lst_gap;
                fpub_n   <= lst_n;
                fpub_lvl <= lst_lvl;
                fpub_rsn <= f_evt_rsn;
                fpub_seq <= fpub_seq + 8'd1;
            end else begin
                f_pend <= 1'b1;
                f_rsn  <= f_evt_rsn;
            end
        end
        case (cap_st)
        2'd0: if (auxch_tri && !cap_tri_d) begin       // TX just released
                  cap_st  <= 2'd1;
                  cap_t   <= '0;
                  cap_n   <= '0;
                  cap_lvl <= cap_rx_s[2];
              end
        2'd1: begin
                  cap_t <= cap_t + 16'd1;
                  if (cap_rx_s[2] != cap_rx_s[1]) begin
                      cap_gap <= cap_t;
                      cap_t   <= '0;
                      cap_st  <= 2'd2;
                  end else if (cap_t == 16'd60000) begin
                      cap_gap <= 16'hFFFF;              // no reply edge
                      cap_st  <= 2'd3;
                  end
              end
        2'd2: begin
                  cap_t <= cap_t + 16'd1;
                  if (cap_rx_s[2] != cap_rx_s[1]) begin
                      cap_iv[cap_n] <= (cap_t > 16'd255) ? 8'hFF : cap_t[7:0];
                      cap_n <= cap_n + 6'd1;
                      cap_t <= '0;
                      if (cap_n == 6'(CAP_N - 1)) cap_st <= 2'd3;
                  end else if (cap_t == 16'd60000) begin
                      cap_st <= 2'd3;
                  end
              end
        2'd3: begin                                    // keep, publish, re-arm
                  for (int i = 0; i < CAP_N; i++) lst_iv[i] <= cap_iv[i];
                  lst_gap <= cap_gap;
                  lst_n   <= cap_n;
                  lst_lvl <= cap_lvl;
                  if (per_due) begin
                      for (int i = 0; i < CAP_N; i++) pub_iv[i] <= cap_iv[i];
                      pub_gap <= cap_gap;
                      pub_n   <= cap_n;
                      pub_lvl <= cap_lvl;
                      pub_seq <= pub_seq + 8'd1;
                      per_due <= 1'b0;
                  end
                  if (f_pend && !f_evt) begin
                      for (int i = 0; i < CAP_N; i++) fpub_iv[i] <= cap_iv[i];
                      fpub_gap <= cap_gap;
                      fpub_n   <= cap_n;
                      fpub_lvl <= cap_lvl;
                      fpub_rsn <= f_rsn;
                      fpub_seq <= fpub_seq + 8'd1;
                      f_pend   <= 1'b0;
                  end
                  cap_st <= 2'd0;
              end
        endcase
    end

    // ------------------------------------------------------------------
    // Colorbars (1280 wide: eight 160-px bars) + 2-px white frame
    // ------------------------------------------------------------------
    localparam bit HD    = (LANES == 4);
    localparam int H_VIS = HD ? 1920 : 1280;
    localparam int H_TOT = HD ? 2200 : 1650;
    localparam int H_SYN = HD ? 44   : 40;
    localparam int H_STA = HD ? 192  : 260;
    localparam int V_VIS = HD ? 1080 : 720;
    localparam int V_TOT = HD ? 1125 : 750;
    localparam int V_SYN = 5;
    localparam int V_STA = HD ? 41   : 25;
    localparam int PIX_DIV  = HD ? 6 : 12;      // 81 MHz * 11 / DIV
    localparam int PIX_HZ   = HD ? 148_500_000 : 74_250_000;
    localparam int BAR_W    = H_VIS / 8;
    localparam int CY_MSB   = $clog2(V_TOT) - 1;   // dp_transmitter's cy width - 1

    logic        clk_pixel;
    logic [11:0] cx;
    logic [10:0] cy;
    logic [23:0] rgb, bar;

    always_comb begin
        if      (cx < 12'(BAR_W*1)) bar = 24'hFFFFFF;
        else if (cx < 12'(BAR_W*2)) bar = 24'hFFFF00;
        else if (cx < 12'(BAR_W*3)) bar = 24'h00FFFF;
        else if (cx < 12'(BAR_W*4)) bar = 24'h00FF00;
        else if (cx < 12'(BAR_W*5)) bar = 24'hFF00FF;
        else if (cx < 12'(BAR_W*6)) bar = 24'hFF0000;
        else if (cx < 12'(BAR_W*7)) bar = 24'h0000FF;
        else                        bar = 24'h000000;
    end

    always_ff @(posedge clk_pixel) begin
        if (cx < 12'd2 || cx >= 12'(H_VIS - 2) || cy < 11'd2 || cy >= 11'(V_VIS - 2))
            rgb <= 24'hFFFFFF;
        else
            rgb <= bar;
    end

    // 48 kHz strobe, ~1 kHz sawtooth at -18 dB
    logic               clk_audio;
    logic signed [28:0] aud_acc = -29'(PIX_HZ);
    logic [15:0]        tone;
    always_ff @(posedge clk_pixel) begin
        clk_audio <= 1'b0;
        if (!aud_acc[28]) begin
            aud_acc   <= aud_acc + 29'sd48_000 - 29'(PIX_HZ);
            clk_audio <= 1'b1;
            tone      <= tone + 16'd1365;
        end else
            aud_acc <= aud_acc + 29'sd48_000;
    end
    logic [15:0] audio_sample_word [1:0];
    assign audio_sample_word[0] = {{3{tone[15]}}, tone[15:3]};
    assign audio_sample_word[1] = {{3{tone[15]}}, tone[15:3]};

    // ------------------------------------------------------------------
    // DisplayPort link layer (external D-PHY serializer)
    // ------------------------------------------------------------------
    logic        link_established, video_live;
    logic [7:0]  debug;
    logic [7:0]  serdes_status;
    logic [15:0] aux_dbg_rx;
    logic [7:0]  aux_dbg_gate;
    logic [15:0] aux_dbg_adjust;
    logic [23:0] aux_dbg_chstate;
    logic [3:0]  aux_dbg_wdog;
    logic [15:0] aux_dbg_tear;
    logic [23:0] aux_dbg_auxerr;
    logic [15:0] aux_dbg_esi;
    logic [6:0]  aux_dbg_defer;
    logic        aux_wedge;
    logic [15:0] aux_errcnt0, aux_errcnt1;
    logic [27:0] aux_dbg_errdet;
    logic [31:0] aux_dbg_snap;
    logic [7:0]  aux_dbg_sink, aux_dbg_caps;
    logic        hpd_present_w;

    logic        clk_sym;
    logic [7:0]  urun_cnt;
    logic [15:0] fifo_lvl;
    logic [79:0] tx_symbols;
    logic [3:0]  powerup_channel, tx_running;

    dp_transmitter #(
        .LANE_COUNT       (LANES),
        .LINK_RATE_MBPS   (1620),
        // closed loop (AUX RX under test) unless the _blind project selects
        // the a2mega 1.0a3 open-loop policy (writes DPCD, never waits for
        // replies) to prove the main link independently of AUX receive
        .BLIND_SINK       (`DPT_BLIND),
        // With LINK_CHECK=0 nothing else notices a replug: restart the
        // ladder when HPD drops >= 2 ms (bench 10-10: Y stayed 11 through
        // replugs, new attach never retrained). a2mega keeps this OFF
        // (sleeping-monitor HPD flap storms) — revisit if the FANGOR flaps.
        .HPD_DISCONNECT_RESETS(1),
        .ENABLE_AFE_ADJUST(0),       // swing/pre-emphasis live in the SN75DP130
        .AFE_APPLY_ON_START(0),
        .ENABLE_PHASE_DONE_GATE(1),
        .GATE_GRACE       (0),
        .GATE_KICK        (0),
        .LATE_REPLY_DRAIN (0),
        .IRQ_SERVICE      (2),
        .POLITE_ATTACH    (1),
        .ERRCNT_READ      (0),
        // Bring-up 10-10: training succeeds (0x202=77, aligned, 0 symbol
        // errors) but established-state AUX checks intermittently fail and
        // tore the link down every ~1 s, before the FANGOR could display.
        // Hold the link once trained until AUX RX margin is fixed.
        // _auxobs variant: LINK_CHECK=2 runs the periodic checks but only
        // counts failures (P9) and freezes the failing reply (PA).
        .LINK_CHECK       (`DPT_LINK_CHECK),
        .TX_PROBE         (0),
        // 720p60 (VIC 4, 74.25 MHz) or 1080p60 (VIC 16, 148.5 MHz);
        // pixel = 81 MHz * 11 / PIX_DIV via VCO 891
        .H_VISIBLE (H_VIS), .H_TOTAL (H_TOT), .H_SYNC_WIDTH (H_SYN), .H_START (H_STA),
        .V_VISIBLE (V_VIS), .V_TOTAL (V_TOT), .V_SYNC_WIDTH (V_SYN), .V_START (V_STA),
        .PIXEL_CLK_MULT   (11),
        .PIXEL_CLK_DIV    (PIX_DIV),
        .PIXEL_PLL_IDIV   (1),
        .PIXEL_PLL_MDIV   (11),
        .PIXEL_PLL_ODIV0  (PIX_DIV),
        .PIXEL_PLL_FCLKIN ("81"),
        .AUDIO_RATE       (48000),
        .AUDIO_BIT_WIDTH  (16)
    ) i_dp (
        .clk100            (clk100),
        .refclk0           (1'b0),
        .refclk1           (1'b0),
        .sim_clk_pixel     (1'b0),
        .reset             (reset),
        .clk_audio         (clk_audio),
        .audio_sample_word (audio_sample_word),
        .clk_pixel         (clk_pixel),
        .rgb               (rgb),
        .cx                (cx),
        .cy                (cy),
        .frame_width(), .screen_width(), .frame_height(), .screen_height(),
        .dp_tx_lane_p      (),
        .dp_tx_lane_n      (),
        .hpd               (dp_hpd),
        .auxch_in          (auxch_in),
        .auxch_out         (auxch_out),
        .auxch_tri         (auxch_tri),
        .link_established  (link_established),
        .video_live        (video_live),
        .debug             (debug),
        .debug_rx          (aux_dbg_rx),
        .debug_locks       (),
        .debug_gate        (aux_dbg_gate),
        .debug_sink        (aux_dbg_sink),
        .debug_adjust      (aux_dbg_adjust),
        .debug_chstate     (aux_dbg_chstate),
        .debug_aux_err     (aux_dbg_auxerr),
        .debug_esi         (aux_dbg_esi),
        .debug_defer       (aux_dbg_defer),
        .wedge_suspect     (aux_wedge),
        .debug_drp_clk     (),
        .debug_errcnt0     (aux_errcnt0),
        .debug_errcnt1     (aux_errcnt1),
        .debug_err_detail  (aux_dbg_errdet),
        .debug_obs         (aux_dbg_obs),
        .debug_snapshot    (aux_dbg_snap),
        .debug_caps        (aux_dbg_caps),
        .debug_wdog        (aux_dbg_wdog),
        .debug_teardown    (aux_dbg_tear),
        .debug_wrusewd     (),
        .debug_afe         (),
        .debug_afe1        (),
        .debug_evt         (),
        .clk_symbol_out    (),
        .serdes_status     (serdes_status),
        .hpd_present_out   (hpd_present_w),
        .debug_underrun_cnt(urun_cnt),
        .debug_fifo_level  (fifo_lvl),
        .ext_symbol_clk    (clk_sym),
        .ext_tx_running    (tx_running),
        .ext_tx_symbols    (tx_symbols),
        .ext_powerup_channel(powerup_channel),
        .drp_dbg_idx       (5'd0),
        .drp_dbg_data      (),
        .drp_dbg_addr      (),
        .drp_dbg_done      ()
    );

    // ------------------------------------------------------------------
    // D-PHY serializer
    // ------------------------------------------------------------------
    logic [7:0] dphy_status;
    transceiver_bank_dphy #(
        .REVERSE_LANES (1),
        .LANE_MAP      (`DPT_LANE_MAP),
        .POL_INV       (`DPT_POL_INV),
        .PROBE         (PROBE)
    ) i_phy (
        .mgmt_clk        (clk100),
        .clk_dphy_ref    (clk54),
        .clk_md          (clk50_in),
        .reset           (reset),
        .powerup_channel (powerup_channel),
        .tx_running      (tx_running),
        .tx_symbol_clk   (clk_sym),
        .tx_symbols      (tx_symbols),
        .status          (dphy_status),
        .tx_clk_out      (),
        .dphy_ck_p       (dphy_ck_p),
        .dphy_ck_n       (dphy_ck_n),
        .dphy_d_p        (dphy_d_p),
        .dphy_d_n        (dphy_d_n)
    );

    // ------------------------------------------------------------------
    // Symbol-clock frequency meter: symbols counted per 1 s window of the
    // 50 MHz crystal. Expect 81,000,000.
    // ------------------------------------------------------------------
    logic [25:0] win_cnt = '0;
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
    always_ff @(posedge clk_sym) begin
        sym_cnt  <= sym_cnt + 28'd1;
        tgl_sync <= {tgl_sync[1:0], win_tgl};
        if (tgl_sync[2] != tgl_sync[1]) begin
            sym_delta <= sym_cnt - sym_last;
            sym_last  <= sym_cnt;
        end
    end

    // Pixel-clock meter: pixels per 1 s crystal window (expect 74,250,000
    // at 720p, 148,500,000 at 1080p — exactly MULT/DIV of Q:)
    logic [27:0] pix_cnt = '0, pix_last = '0, pix_delta = '0;
    logic [2:0]  ptgl_sync = '0;
    always_ff @(posedge clk_pixel) begin
        pix_cnt   <= pix_cnt + 28'd1;
        ptgl_sync <= {ptgl_sync[1:0], win_tgl};
        if (ptgl_sync[2] != ptgl_sync[1]) begin
            pix_delta <= pix_cnt - pix_last;
            pix_last  <= pix_cnt;
        end
    end

    logic [7:0] frame_cnt = '0;
    logic       cy_msb_d = 1'b0;
    always_ff @(posedge clk_pixel) begin
        cy_msb_d <= cy[CY_MSB];
        if (cy_msb_d && !cy[CY_MSB])
            frame_cnt <= frame_cnt + 8'd1;
    end

    // ------------------------------------------------------------------
    // Synchronize status into clk50 for telemetry
    // ------------------------------------------------------------------
    logic [7:0]  st_s0, st_s, dbg_s0, dbg_s, frm_s0, frm_s;
    logic [2:0]  flg_s0, flg_s;
    logic [16:0] hp_s0, hp_s;
    logic [7:0]  gate_s0, gate_s;
    logic [15:0] adj_s0, adj_s;
    logic [23:0] chst_s0, chst_s;
    logic [27:0] symd_s0, symd_s;
    logic [7:0]  snk_s0, snk_s, cap_s0, cap_s;
    logic [15:0] ec0_s0, ec0_s, ec1_s0, ec1_s;
    logic [3:0]  wdog_s0, wdog_s;
    logic [15:0] tear_s0, tear_s;
    logic [23:0] aerr_s0, aerr_s;
    logic [15:0] esi_s0, esi_s;
    logic [7:0]  defr_s0, defr_s;
    logic [27:0] edet_s0, edet_s;
    logic [31:0] snap_s0, snap_s;
    logic [27:0] pxd_s0, pxd_s;
    logic [7:0]  ur_s0, ur_s;
    logic [15:0] fl_s0, fl_s;
    logic [103:0] obs_s0, obs_s;
    always_ff @(posedge clk50_in) begin
        st_s0   <= dphy_status;         st_s   <= st_s0;
        dbg_s0  <= debug;               dbg_s  <= dbg_s0;
        frm_s0  <= frame_cnt;           frm_s  <= frm_s0;
        flg_s0  <= {dp_hpd, link_established, video_live};  flg_s <= flg_s0;
        hp_s0   <= {hpd_present_w, aux_dbg_rx};             hp_s  <= hp_s0;
        gate_s0 <= aux_dbg_gate;        gate_s <= gate_s0;
        adj_s0  <= aux_dbg_adjust;      adj_s  <= adj_s0;
        chst_s0 <= aux_dbg_chstate;     chst_s <= chst_s0;
        symd_s0 <= sym_delta;           symd_s <= symd_s0;
        snk_s0  <= aux_dbg_sink;        snk_s  <= snk_s0;
        cap_s0  <= aux_dbg_caps;        cap_s  <= cap_s0;
        ec0_s0  <= aux_errcnt0;         ec0_s  <= ec0_s0;
        ec1_s0  <= aux_errcnt1;         ec1_s  <= ec1_s0;
        wdog_s0 <= aux_dbg_wdog;        wdog_s <= wdog_s0;
        tear_s0 <= aux_dbg_tear;        tear_s <= tear_s0;
        aerr_s0 <= aux_dbg_auxerr;      aerr_s <= aerr_s0;
        esi_s0  <= aux_dbg_esi;         esi_s  <= esi_s0;
        defr_s0 <= {aux_wedge, aux_dbg_defer}; defr_s <= defr_s0;
        edet_s0 <= aux_dbg_errdet;      edet_s <= edet_s0;
        snap_s0 <= aux_dbg_snap;        snap_s <= snap_s0;
        pxd_s0  <= pix_delta;           pxd_s  <= pxd_s0;
        ur_s0   <= urun_cnt;            ur_s   <= ur_s0;
        fl_s0   <= fifo_lvl;            fl_s   <= fl_s0;
        obs_s0  <= aux_dbg_obs;         obs_s  <= obs_s0;
    end

    logic [3:0] link_rises = '0, vid_rises = '0;
    logic       link_d = 1'b0, vid_d = 1'b0;
    always_ff @(posedge clk50_in) begin
        link_d <= flg_s[1];
        vid_d  <= flg_s[0];
        if (flg_s[1] && !link_d && link_rises != 4'hF) link_rises <= link_rises + 4'd1;
        if (flg_s[0] && !vid_d  && vid_rises  != 4'hF) vid_rises  <= vid_rises  + 4'd1;
    end

    // ------------------------------------------------------------------
    // Telemetry UART: 115200 8N1, six status lines per message
    // ------------------------------------------------------------------
    function automatic [7:0] hexch(input [3:0] n);
        hexch = (n < 4'd10) ? (8'h30 + 8'(n)) : (8'h37 + 8'(n));
    endfunction

    localparam int MSG_LEN = 489;   // P1..P8, P9, PA lines
    logic [7:0] msg [0:MSG_LEN-1];
    always_comb begin
        for (int i = 0; i < MSG_LEN; i++) msg[i] = " ";
        // P1: link ladder
        msg[0]="P"; msg[1]="1"; msg[2]=" "; msg[3]="S"; msg[4]=":";
        msg[5]=hexch(st_s[7:4]); msg[6]=hexch(st_s[3:0]);
        msg[8]="D"; msg[9]=":"; msg[10]=hexch(dbg_s[7:4]); msg[11]=hexch(dbg_s[3:0]);
        msg[13]="F"; msg[14]=":"; msg[15]=hexch(frm_s[7:4]); msg[16]=hexch(frm_s[3:0]);
        msg[18]="H"; msg[19]="L"; msg[20]="V"; msg[21]=":";
        msg[22]=8'h30 + 8'(flg_s[2]); msg[23]=8'h30 + 8'(flg_s[1]); msg[24]=8'h30 + 8'(flg_s[0]);
        msg[26]="P"; msg[27]=":"; msg[28]=8'h30 + 8'(hp_s[16]);
        msg[30]="E"; msg[31]=":"; msg[32]=hexch(hp_s[7:4]); msg[33]=hexch(hp_s[3:0]);
        msg[35]=8'h0A;
        // P2: AUX receive + training state
        msg[36]="P"; msg[37]="2"; msg[39]="R"; msg[40]=":"; msg[41]=hexch(hp_s[15:12]);
        msg[43]="A"; msg[44]=":"; msg[45]=hexch(adj_s[15:12]); msg[46]=hexch(adj_s[11:8]);
        msg[47]=hexch(adj_s[7:4]); msg[48]=hexch(adj_s[3:0]);
        msg[50]="G"; msg[51]=":"; msg[52]=hexch(gate_s[7:4]); msg[53]=hexch(gate_s[3:0]);
        msg[55]="Y"; msg[56]=":"; msg[57]=hexch(link_rises); msg[58]=hexch(vid_rises);
        msg[60]="C"; msg[61]=":"; msg[62]=hexch(chst_s[23:20]); msg[63]=hexch(chst_s[19:16]);
        msg[64]=hexch(chst_s[7:4]); msg[65]=hexch(chst_s[3:0]);
        msg[67]=8'h0A;
        // P3: symbol clock meter, sink status/caps, watchdog, teardowns
        msg[68]="P"; msg[69]="3"; msg[71]="Q"; msg[72]=":";
        msg[73]=hexch(symd_s[27:24]); msg[74]=hexch(symd_s[23:20]); msg[75]=hexch(symd_s[19:16]);
        msg[76]=hexch(symd_s[15:12]); msg[77]=hexch(symd_s[11:8]); msg[78]=hexch(symd_s[7:4]);
        msg[79]=hexch(symd_s[3:0]);
        msg[81]="K"; msg[82]=":"; msg[83]=hexch(snk_s[7:4]); msg[84]=hexch(snk_s[3:0]);
        msg[86]="X"; msg[87]=":"; msg[88]=hexch(cap_s[7:4]); msg[89]=hexch(cap_s[3:0]);
        msg[91]="W"; msg[92]=":"; msg[93]=hexch(wdog_s);
        msg[95]="T"; msg[96]=":"; msg[97]=hexch(tear_s[15:12]); msg[98]=hexch(tear_s[11:8]);
        msg[99]=hexch(tear_s[7:4]); msg[100]=hexch(tear_s[3:0]);
        msg[101]=8'h0A;
        // P4: AUX error classes, ESI, defers
        msg[102]="P"; msg[103]="4"; msg[105]="J"; msg[106]=":";
        msg[107]=hexch(aerr_s[23:20]); msg[108]=hexch(aerr_s[19:16]); msg[109]=hexch(aerr_s[15:12]);
        msg[110]=hexch(aerr_s[11:8]); msg[111]=hexch(aerr_s[7:4]); msg[112]=hexch(aerr_s[3:0]);
        msg[114]="V"; msg[115]=":"; msg[116]=hexch(esi_s[15:12]); msg[117]=hexch(esi_s[11:8]);
        msg[118]=hexch(esi_s[7:4]); msg[119]=hexch(esi_s[3:0]);
        msg[121]="U"; msg[122]=":"; msg[123]=hexch(defr_s[7:4]); msg[124]=hexch(defr_s[3:0]);
        msg[125]=8'h0A;
        // P5: first-failure snapshot + teardown detail
        msg[126]="P"; msg[127]="5"; msg[129]="Z"; msg[130]=":";
        for (int i = 0; i < 8; i++) msg[131+i] = hexch(snap_s[31-4*i -: 4]);
        msg[140]="B"; msg[141]=":";
        for (int i = 0; i < 7; i++) msg[142+i] = hexch(edet_s[27-4*i -: 4]);
        msg[149]=8'h0A;
        // P6: per-lane symbol error counters
        msg[150]="P"; msg[151]="6"; msg[153]="0"; msg[154]=":";
        for (int i = 0; i < 4; i++) msg[155+i] = hexch(ec0_s[15-4*i -: 4]);
        msg[160]="1"; msg[161]=":";
        for (int i = 0; i < 4; i++) msg[162+i] = hexch(ec1_s[15-4*i -: 4]);
        msg[171]=8'h0A;
        // P7: AUX reply capture (see instrument above)
        msg[172]="P"; msg[173]="7"; msg[175]="#"; msg[176]=hexch(pub_seq[3:0]);
        msg[178]="L"; msg[179]=":"; msg[180]=8'h30 + 8'(pub_lvl);
        msg[182]="G"; msg[183]=":";
        for (int i = 0; i < 4; i++) msg[184+i] = hexch(pub_gap[15-4*i -: 4]);
        msg[189]="N"; msg[190]=":"; msg[191]=hexch({2'b0, pub_n[5:4]}); msg[192]=hexch(pub_n[3:0]);
        msg[193]=" ";
        for (int i = 0; i < CAP_N; i++) begin
            msg[194+2*i] = hexch(pub_iv[i][7:4]);
            msg[195+2*i] = hexch(pub_iv[i][3:0]);
        end
        msg[290]=8'h0A;
        // P8: pixel clock (per 1 s), packer underruns, pixel FIFO level
        msg[291]="P"; msg[292]="8"; msg[294]="X"; msg[295]=":";
        for (int i = 0; i < 7; i++) msg[296+i] = hexch(pxd_s[27-4*i -: 4]);
        msg[304]="U"; msg[305]="R"; msg[306]=":"; msg[307]=hexch(ur_s[7:4]); msg[308]=hexch(ur_s[3:0]);
        msg[310]="L"; msg[311]=":";
        for (int i = 0; i < 4; i++) msg[312+i] = hexch(fl_s[15-4*i -: 4]);
        msg[317]=8'h0A;
        // P9: LINK_CHECK=2 observe counters (16-bit, wrapping):
        // C checks issued, S short, N non-ACK, D DEFER, T timeout,
        // A lock-gate failures, H last non-ACK header byte
        msg[318]="P"; msg[319]="9";
        for (int f = 0; f < 6; f++) begin
            msg[321+7*f] = (f==0)?"C":(f==1)?"S":(f==2)?"N":(f==3)?"D":(f==4)?"T":"A";
            msg[322+7*f] = ":";
            for (int i = 0; i < 4; i++)
                msg[323+7*f+i] = hexch(obs_s[95-16*f-4*i -: 4]);
        end
        msg[362]="H"; msg[363]=":"; msg[364]=hexch(obs_s[103:100]); msg[365]=hexch(obs_s[99:96]);
        msg[366]=8'h0A;
        // PA: capture of the last FAILING reply (layout as P7, + reason)
        msg[367]="P"; msg[368]="A"; msg[370]="#"; msg[371]=hexch(fpub_seq[3:0]);
        msg[372]=fpub_rsn;
        msg[374]="L"; msg[375]=":"; msg[376]=8'h30 + 8'(fpub_lvl);
        msg[378]="G"; msg[379]=":";
        for (int i = 0; i < 4; i++) msg[380+i] = hexch(fpub_gap[15-4*i -: 4]);
        msg[385]="N"; msg[386]=":"; msg[387]=hexch({2'b0, fpub_n[5:4]}); msg[388]=hexch(fpub_n[3:0]);
        for (int i = 0; i < CAP_N; i++) begin
            msg[390+2*i] = hexch(fpub_iv[i][7:4]);
            msg[391+2*i] = hexch(fpub_iv[i][3:0]);
        end
        msg[488]=8'h0A;
    end

    logic [8:0]  baud_cnt = '0;
    logic        baud_tick;
    logic [24:0] msg_timer = '0;
    logic [8:0]  msg_idx = MSG_LEN[8:0];
    logic [3:0]  bit_idx = '0;
    logic [9:0]  shifter = 10'h3FF;
    always_ff @(posedge clk50_in) begin
        baud_tick <= 1'b0;
        if (baud_cnt == 9'd433) begin baud_cnt <= '0; baud_tick <= 1'b1; end
        else baud_cnt <= baud_cnt + 9'd1;

        msg_timer <= msg_timer + 25'd1;
        if (msg_timer[23:0] == 24'd0 && msg_idx == MSG_LEN[8:0]) begin
            msg_idx <= '0;
            bit_idx <= 4'd10;
        end

        if (baud_tick && msg_idx != MSG_LEN[8:0]) begin
            if (bit_idx >= 4'd10) begin
                shifter <= {1'b1, msg[msg_idx], 1'b0};
                bit_idx <= 4'd0;
            end else begin
                shifter <= {1'b1, shifter[9:1]};
                bit_idx <= bit_idx + 4'd1;
                if (bit_idx == 4'd9)
                    msg_idx <= msg_idx + 9'd1;
            end
        end
    end
    assign uart_tx = (msg_idx == MSG_LEN[8:0]) ? 1'b1 : shifter[0];

    // ------------------------------------------------------------------
    // Heartbeat: hb[24] at 50 MHz = 1.49 Hz (same as a2mega led[0])
    // ------------------------------------------------------------------
    logic [24:0] hb = '0;
    always_ff @(posedge clk50_in) begin
        hb  <= hb + 25'd1;
        led <= hb[24];
    end

endmodule
