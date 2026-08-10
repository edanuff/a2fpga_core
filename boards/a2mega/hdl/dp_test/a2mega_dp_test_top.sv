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

    output logic [3:0] led          // D15 D14 R14 P14, active low
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

    dp_transmitter #(
        .LANE_COUNT     (2),
        .LINK_RATE_MBPS (2700),
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
        .debug             (debug)
    );

    // ------------------------------------------------------------------
    // Bring-up ladder on the LEDs (active low)
    // ------------------------------------------------------------------
    logic [24:0] hb_cnt;
    always_ff @(posedge clk50_in)
        hb_cnt <= hb_cnt + 25'd1;

    assign led[0] = ~hb_cnt[24];        // heartbeat
    assign led[1] = ~dp_hpd;            // HPD from the ESP32
    assign led[2] = ~link_established;  // link training done
    assign led[3] = ~video_live;        // pixels flowing

endmodule
