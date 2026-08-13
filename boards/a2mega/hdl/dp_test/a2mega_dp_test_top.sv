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

module a2mega_dp_test_top (
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
    // AUX analog interface: drive/tri-state the pseudo-diff pair
    // ------------------------------------------------------------------
    logic auxch_in, auxch_out, auxch_tri;
    assign dp_aux_p = auxch_tri ? 1'bz : auxch_out;
    assign dp_aux_n = auxch_tri ? 1'bz : ~auxch_out;
    assign auxch_in = dp_aux_p;

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

    dp_transmitter #(
        .LANE_COUNT     (2),
        .LINK_RATE_MBPS (2700),
        // 1.0a3: AUX receive is electrically dead (AC caps, no FPGA-side
        // bias, LVCMOS thresholds unreachable by a <=1.38 Vpp reply; board
        // not field-modifiable). TX works — run the link policy open-loop.
        // Remove when a board rev provides a real AUX front-end.
        .BLIND_SINK     (1),
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
        .clk_symbol_out    (clk_sym_w),
        .serdes_status     (serdes_status)
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
    always_ff @(posedge clk50_in) begin
        st_s0  <= serdes_status;  st_s  <= st_s0;
        dbg_s0 <= debug;          dbg_s <= dbg_s0;
        frm_s0 <= frame_cnt;      frm_s <= frm_s0;
        flg_s0 <= {dp_hpd, link_established, video_live};
        flg_s  <= flg_s0;
    end

    function automatic [7:0] hexch(input [3:0] n);
        hexch = (n < 4'd10) ? (8'h30 + 8'(n)) : (8'h37 + 8'(n));
    endfunction

    localparam int MSG_LEN = 27;
    logic [7:0] msg [0:MSG_LEN-1];
    always_comb begin
        msg[0]="D"; msg[1]="P"; msg[2]=" "; msg[3]="S"; msg[4]=":";
        msg[5]=hexch(st_s[7:4]); msg[6]=hexch(st_s[3:0]);
        msg[7]=" "; msg[8]="D"; msg[9]=":";
        msg[10]=hexch(dbg_s[7:4]); msg[11]=hexch(dbg_s[3:0]);
        msg[12]=" "; msg[13]="F"; msg[14]=":";
        msg[15]=hexch(frm_s[7:4]); msg[16]=hexch(frm_s[3:0]);
        msg[17]=" "; msg[18]="H"; msg[19]="L"; msg[20]="V"; msg[21]=":";
        msg[22]=8'h30 + 8'(flg_s[2]);
        msg[23]=8'h30 + 8'(flg_s[1]);
        msg[24]=8'h30 + 8'(flg_s[0]);
        msg[25]=8'h0D; msg[26]=8'h0A;
    end

    // 115200 baud from 50 MHz (divisor 434); one message per ~0.5 s
    logic [8:0]  baud_cnt = '0;
    logic        baud_tick;
    logic [24:0] msg_timer = '0;
    logic [4:0]  msg_idx = MSG_LEN[4:0];   // idle when == MSG_LEN
    logic [3:0]  bit_idx = '0;
    logic [9:0]  shifter = 10'h3FF;
    always_ff @(posedge clk50_in) begin
        baud_tick <= 1'b0;
        if (baud_cnt == 9'd433) begin baud_cnt <= '0; baud_tick <= 1'b1; end
        else baud_cnt <= baud_cnt + 9'd1;

        msg_timer <= msg_timer + 25'd1;
        if (msg_timer == 25'd0 && msg_idx == MSG_LEN[4:0]) begin
            msg_idx <= '0;                 // start a new message
            bit_idx <= 4'd10;              // force reload on next tick
        end

        if (baud_tick && msg_idx != MSG_LEN[4:0]) begin
            if (bit_idx >= 4'd10) begin    // load next char: start+8+stop
                shifter <= {1'b1, msg[msg_idx], 1'b0};
                bit_idx <= 4'd0;
            end else begin
                shifter <= {1'b1, shifter[9:1]};
                bit_idx <= bit_idx + 4'd1;
                if (bit_idx == 4'd9)
                    msg_idx <= msg_idx + 5'd1;
            end
        end
    end
    assign uart_tx = (msg_idx == MSG_LEN[4:0]) ? 1'b1 : shifter[0];

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
            sym_delta <= sym_cnt - sym_last;
            sym_last  <= sym_cnt;
            // 135 M ± ~2%: 132.3M .. 137.7M
            freq_ok <= (sym_cnt - sym_last > 28'd132_300_000) &&
                       (sym_cnt - sym_last < 28'd137_700_000);
        end
    end

    // ------------------------------------------------------------------
    // Bring-up ladder on the LEDs (active low)
    // ------------------------------------------------------------------
    logic [24:0] hb_cnt;
    always_ff @(posedge clk50_in)
        hb_cnt <= hb_cnt + 25'd1;

    assign led[0] = ~hb_cnt[24];        // heartbeat
    assign led[1] = ~dp_hpd;            // HPD from the ESP32
    assign led[2] = ~freq_ok;           // clk_sym == 135 MHz (line rate OK)
    assign led[3] = ~video_live;        // pixels flowing

endmodule
