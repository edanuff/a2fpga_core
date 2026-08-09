///////////////////////////////////////////////////////////////////////////////
// dp_transmitter.sv : Self-contained DisplayPort transmitter with audio
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Ergonomics modelled on the hdl-util HDMI module: the consumer feeds RGB
// pixels (pull-style: this module outputs cx/cy, the consumer supplies the
// pixel for that coordinate) plus an audio-sample-rate strobe and stereo PCM,
// and receives DP main-link lanes.
//
// Clocking is DP synchronous-clock mode: clk_pixel is an OUTPUT, generated
// from the link symbol clock, so Mvid/Nvid are exact constants.
//
// Vendor selection (same pattern as hdl-util hdmi's serializer.sv):
//   `define DP_VENDOR_XILINX_GTP  - Artix-7 GTPE2 (original hamster target)
//   `define DP_VENDOR_GOWIN       - Gowin GW5AT Arora-V SERDES (in progress)
//   (neither)                     - behavioural stub for simulation/lint
//
// Status: simulation-verified. Generic video front-end (pull-style rgb via
// cx/cy), audio SDP subsystem (Audio_TimeStamp / Audio_Stream / Audio
// InfoFrame with RS(15,13) ECC), fabric 8b/10b for the Gowin raw-mode
// SERDES, and RBR link training are all covered by the test_benches/
// suite with independent C-model checkers (misc/). Hardware bring-up has
// not been attempted yet; see examples/tang_mega/README.md.
//
// MIT License - derived from work Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module dp_transmitter #(
    parameter int LANE_COUNT     = 2,      // 1 or 2 (4 once a 4-lane PHY bank exists)
    parameter int LINK_RATE_MBPS = 1620,   // 1620 (RBR); plumbing for 2700 later
    // Video timing - defaults are 1280x720p60, which fits RBR x2
    parameter int H_VISIBLE = 1280, H_TOTAL = 1650, H_SYNC_WIDTH = 40, H_START = 260,
    parameter int V_VISIBLE = 720,  V_TOTAL = 750,  V_SYNC_WIDTH = 5,  V_START = 25,
    parameter bit H_SYNC_ACTIVE_HIGH = 1'b1,
    parameter bit V_SYNC_ACTIVE_HIGH = 1'b1,
    parameter int TU_SIZE = 64,
    // F_pixel = F_symbol_clk * PIXEL_CLK_MULT / PIXEL_CLK_DIV (81 MHz * 11/12 = 74.25 MHz)
    parameter int PIXEL_CLK_MULT = 11,
    parameter int PIXEL_CLK_DIV  = 12,
    // Audio - HDMI-style contract
    parameter int AUDIO_RATE      = 48000, // 44100 | 48000
    parameter int AUDIO_BIT_WIDTH = 16,
    parameter int BIT_WIDTH  = $clog2(H_TOTAL),
    parameter int BIT_HEIGHT = $clog2(V_TOTAL)
)(
    input  logic clk100,          // 100 MHz management clock (AUX bit timing depends on it)
    input  logic refclk0,         // SERDES/transceiver reference clock(s); buffers live
    input  logic refclk1,         //   in the board top (IBUFDS_GTE2 / Gowin equivalent)
    input  logic sim_clk_pixel,   // no-vendor sim builds only: externally generated pixel
                                  //   clock (F_symbol * PIXEL_CLK_MULT/PIXEL_CLK_DIV);
                                  //   vendor builds generate clk_pixel from a fabric PLL
    input  logic reset,

    // Audio in: clk_audio is a one-clk_pixel-wide strobe at AUDIO_RATE;
    // audio_sample_word must be valid when the strobe fires. (Phase 3)
    input  logic clk_audio,
    input  logic [AUDIO_BIT_WIDTH-1:0] audio_sample_word [1:0],

    // Pull-style video (Phase 2): module owns clk_pixel and coordinates
    output logic clk_pixel,
    input  logic [23:0] rgb,
    output logic [BIT_WIDTH-1:0]  cx,
    output logic [BIT_HEIGHT-1:0] cy,
    output logic [BIT_WIDTH-1:0]  frame_width,
    output logic [BIT_WIDTH-1:0]  screen_width,
    output logic [BIT_HEIGHT-1:0] frame_height,
    output logic [BIT_HEIGHT-1:0] screen_height,

    // DP main link
    output logic [LANE_COUNT-1:0] dp_tx_lane_p,
    output logic [LANE_COUNT-1:0] dp_tx_lane_n,

    // AUX / HPD - raw digital; the analog bidirectional buffer (IOBUFDS /
    // ELVDS_IOBUF / pseudo-differential pair) is instantiated in the board top
    input  logic hpd,
    input  logic auxch_in,
    output logic auxch_out,
    output logic auxch_tri,

    // Status
    output logic link_established,
    output logic video_live,
    output logic [7:0] debug
);

    // ------------------------------------------------------------------
    // Elaboration-time bandwidth check: visible payload must fit in the
    // link's data capacity with margin for blanking overhead + SDPs.
    // Payload bytes/s = Fpix * 3; capacity = LANE_COUNT * rate/10 bytes/s.
    // Using the 0.9 factor from the plan.
    // ------------------------------------------------------------------
    localparam longint F_SYMBOL_HZ = longint'(LINK_RATE_MBPS) * 1_000_000 / 20; // 73-bit word clock
    localparam longint F_PIXEL_HZ  = F_SYMBOL_HZ * PIXEL_CLK_MULT / PIXEL_CLK_DIV;
    initial begin
        if (F_PIXEL_HZ * 3 * 10 > longint'(LANE_COUNT) * LINK_RATE_MBPS * 100_000 * 9)
            $error("dp_transmitter: video bandwidth (%0d B/s) exceeds 90%% of %0dx RBR/HBR lane capacity",
                   F_PIXEL_HZ * 3, LANE_COUNT);
    end

    // ------------------------------------------------------------------
    // Link-layer control signals
    // ------------------------------------------------------------------
    logic  [3:0] tx_powerup_channel;
    logic        preemp_0p0, preemp_3p5, preemp_6p0;
    logic        swing_0p4,  swing_0p6,  swing_0p8;
    logic  [3:0] tx_running;
    logic        tx_symbol_clk;
    logic [79:0] tx_symbols;
    logic        tx_align_train, tx_clock_train, tx_link_established;
    logic  [2:0] stream_channel_count;
    logic  [2:0] source_channel_count;
    logic [72:0] msa_merged_data;
    logic        test_signal_ready;

    assign source_channel_count = 3'(LANE_COUNT);
    assign link_established     = tx_link_established;
    assign video_live           = tx_link_established & test_signal_ready;
    assign stream_channel_count = 3'(LANE_COUNT);

    assign frame_width   = BIT_WIDTH'(H_TOTAL);
    assign screen_width  = BIT_WIDTH'(H_VISIBLE);
    assign frame_height  = BIT_HEIGHT'(V_TOTAL);
    assign screen_height = BIT_HEIGHT'(V_VISIBLE);

    // ------------------------------------------------------------------
    // Sync-clocking M/N: Mvid/Nvid = Fpixel/Fsymbol_lane = MULT/(2*DIV),
    // scaled to Nvid = 2^19 with rounding (exact small rationals are
    // rejected by some sinks - see README's M/N discussion)
    // ------------------------------------------------------------------
    localparam int N_VALUE = 24'h080000;
    localparam int M_VALUE = (PIXEL_CLK_MULT * 262144 + PIXEL_CLK_DIV/2) / PIXEL_CLK_DIV;

    // Link symbols per lane per video line; must divide out exactly
    localparam int SYMS_PER_LINE = H_TOTAL * 2 * PIXEL_CLK_DIV / PIXEL_CLK_MULT;
    localparam int VALID_NUM     = TU_SIZE * 3 * PIXEL_CLK_MULT;
    localparam int VALID_DEN     = 2 * PIXEL_CLK_DIV * LANE_COUNT;
    localparam int WORDS_PER_LINE = H_VISIBLE / LANE_COUNT;
    localparam int FIFO_ADDR_BITS = $clog2(3 * WORDS_PER_LINE);

    initial begin
        if (H_TOTAL * 2 * PIXEL_CLK_DIV % PIXEL_CLK_MULT != 0)
            $error("dp_transmitter: H_TOTAL*2*PIXEL_CLK_DIV must be divisible by PIXEL_CLK_MULT");
        if (H_VISIBLE % LANE_COUNT != 0)
            $error("dp_transmitter: H_VISIBLE must be a multiple of LANE_COUNT");
        if (SYMS_PER_LINE % 2 != 0)
            $error("dp_transmitter: SYMS_PER_LINE must be even (two symbols per lane per clock)");
    end

    // ------------------------------------------------------------------
    // Pixel clock (synchronous clocking: generated from the symbol clock)
    // ------------------------------------------------------------------
`ifdef DP_VENDOR_GOWIN
 `ifdef GOWIN_PLL_IP
    // IDE-generated PLLA wrapper: clk_pixel = tx_symbol_clk * MULT / DIV
    // (81 MHz * 11/12 = 74.25 MHz for 720p60 @ RBR)
    gowin_pixel_pll i_pixel_pll (
        .clkin  (tx_symbol_clk),
        .clkout (clk_pixel)
    );
 `else
    assign clk_pixel = sim_clk_pixel;   // simulation of the Gowin config
 `endif
`elsif DP_VENDOR_XILINX_GTP
    // A MMCM instantiation belongs here when the generic front-end is
    // used on Xilinx; the legacy example tops still use test_source.
    initial $error("dp_transmitter: Xilinx pixel MMCM not wired (use legacy example top)");
`else
    assign clk_pixel = sim_clk_pixel;
`endif

    // ------------------------------------------------------------------
    // Generic video front-end: timing generator (pixel domain) -> CDC
    // FIFO -> symbol-domain stream packer -> MSA insertion
    // ------------------------------------------------------------------
    logic capture_arm, capture_arm_m, capture_arm_px;
    always_ff @(posedge clk_pixel) begin
        capture_arm_m  <= capture_arm;
        capture_arm_px <= capture_arm_m;
    end

    logic [24*LANE_COUNT-1:0] fifo_wdata, fifo_rpix;
    logic fifo_wsof, fifo_wen, fifo_rsof, fifo_rvalid, fifo_rd;
    logic [FIFO_ADDR_BITS:0] fifo_rlevel;

    dp_video_timing #(
        .LANE_COUNT (LANE_COUNT),
        .H_VISIBLE  (H_VISIBLE),
        .H_TOTAL    (H_TOTAL),
        .V_VISIBLE  (V_VISIBLE),
        .V_TOTAL    (V_TOTAL),
        .BIT_WIDTH  (BIT_WIDTH),
        .BIT_HEIGHT (BIT_HEIGHT)
    ) i_dp_video_timing (
        .clk_pixel   (clk_pixel),
        .reset       (reset),
        .capture_arm (capture_arm_px),
        .rgb         (rgb),
        .cx          (cx),
        .cy          (cy),
        .fifo_wdata  (fifo_wdata),
        .fifo_wsof   (fifo_wsof),
        .fifo_wen    (fifo_wen)
    );

    pixel_cdc_fifo #(
        .WIDTH     (24*LANE_COUNT + 1),
        .ADDR_BITS (FIFO_ADDR_BITS)
    ) i_pixel_cdc_fifo (
        .wclk   (clk_pixel),
        .wreset (reset),
        .wdata  ({fifo_wsof, fifo_wdata}),
        .wen    (fifo_wen),
        .wfull  (),
        .rclk   (tx_symbol_clk),
        .rreset (reset),
        .rdata  ({fifo_rsof, fifo_rpix}),
        .rvalid (fifo_rvalid),
        .rd_en  (fifo_rd),
        .rlevel (fifo_rlevel)
    );

    logic [72:0] packed_data, sdp_merged_data;
    logic        sdp_gap, frame_pulse, fifo_underrun;
    logic        audio_strobe_sym, audio_buffer_ready, audio_buffer_take;
    logic        audio_mute;
    logic [4*2*AUDIO_BIT_WIDTH-1:0] audio_buffer_flat;
    logic [23:0] maud;
    logic  [7:0] maud_byte;

    video_stream_packer #(
        .LANE_COUNT    (LANE_COUNT),
        .H_VISIBLE     (H_VISIBLE),
        .V_VISIBLE     (V_VISIBLE),
        .V_TOTAL       (V_TOTAL),
        .TU_SIZE       (TU_SIZE),
        .SYMS_PER_LINE (SYMS_PER_LINE),
        .VALID_NUM     (VALID_NUM),
        .VALID_DEN     (VALID_DEN),
        .PREFILL       (WORDS_PER_LINE)
    ) i_video_stream_packer (
        .clk         (tx_symbol_clk),
        .reset       (reset),
        .mvid_byte   (8'(M_VALUE & 8'hFF)),
        .maud_byte   (maud_byte),
        .audio_mute  (audio_mute),
        .fifo_rdata  (fifo_rpix),
        .fifo_rsof   (fifo_rsof),
        .fifo_rvalid (fifo_rvalid),
        .fifo_rd     (fifo_rd),
        .fifo_rlevel (16'(fifo_rlevel)),
        .capture_arm (capture_arm),
        .ready       (test_signal_ready),
        .data        (packed_data),
        .sdp_gap     (sdp_gap),
        .frame_pulse (frame_pulse),
        .underrun    (fifo_underrun)
    );

    // ------------------------------------------------------------------
    // Audio: strobe CDC + sample buffering, Maud measurement, SDP engine
    // ------------------------------------------------------------------
    audio_sample_buffer #(
        .AUDIO_BIT_WIDTH (AUDIO_BIT_WIDTH)
    ) i_audio_sample_buffer (
        .clk_pixel    (clk_pixel),
        .clk_audio    (clk_audio),
        .sample_l     (audio_sample_word[0]),
        .sample_r     (audio_sample_word[1]),
        .clk_sym      (tx_symbol_clk),
        .reset        (reset),
        .strobe_sym   (audio_strobe_sym),
        .buffer       (audio_buffer_flat),
        .buffer_count (),
        .buffer_ready (audio_buffer_ready),
        .buffer_take  (audio_buffer_take)
    );

    maud_measure #(
        .AUDIO_RATE     (AUDIO_RATE),
        .LINK_RATE_MBPS (LINK_RATE_MBPS)
    ) i_maud_measure (
        .clk_sym    (tx_symbol_clk),
        .reset      (reset),
        .strobe_sym (audio_strobe_sym),
        .maud       (maud),
        .maud_byte  (maud_byte)
    );

    sdp_engine #(
        .LANE_COUNT      (LANE_COUNT),
        .AUDIO_BIT_WIDTH (AUDIO_BIT_WIDTH),
        .AUDIO_RATE      (AUDIO_RATE)
    ) i_sdp_engine (
        .clk          (tx_symbol_clk),
        .reset        (reset),
        .in_data      (packed_data),
        .sdp_gap      (sdp_gap),
        .frame_pulse  (frame_pulse),
        .out_data     (sdp_merged_data),
        .buffer       (audio_buffer_flat),
        .buffer_ready (audio_buffer_ready),
        .buffer_take  (audio_buffer_take),
        .maud         (maud),
        .audio_mute   (audio_mute)
    );

    // ------------------------------------------------------------------
    // MSA (main stream attributes) secondary packet insertion
    // ------------------------------------------------------------------
    generate
    if (LANE_COUNT == 1) begin : g_msa1
        msa_inserter_1ch i_msa(
            .clk                 (tx_symbol_clk),
            .active              (1'b1),
            .M_value             (24'(M_VALUE)),
            .N_value             (24'(N_VALUE)),
            .H_visible           (12'(H_VISIBLE)),
            .V_visible           (12'(V_VISIBLE)),
            .H_total             (12'(H_TOTAL)),
            .V_total             (12'(V_TOTAL)),
            .H_sync_width        (12'(H_SYNC_WIDTH)),
            .V_sync_width        (12'(V_SYNC_WIDTH)),
            .H_start             (12'(H_START)),
            .V_start             (12'(V_START)),
            .H_vsync_active_high (H_SYNC_ACTIVE_HIGH),
            .V_vsync_active_high (V_SYNC_ACTIVE_HIGH),
            .flag_sync_clock     (1'b1),
            .flag_YCCnRGB        (1'b0),
            .flag_422n444        (1'b0),
            .flag_range_reduced  (1'b0),
            .flag_interlaced_even(1'b0),
            .flag_YCC_colour_709 (1'b0),
            .flags_3d_Indicators (2'b00),
            .bits_per_colour     (5'b01000),
            .in_data             (sdp_merged_data),
            .out_data            (msa_merged_data)
        );
    end else begin : g_msa2
        msa_inserter_2ch i_msa(
            .clk                 (tx_symbol_clk),
            .active              (1'b1),
            .M_value             (24'(M_VALUE)),
            .N_value             (24'(N_VALUE)),
            .H_visible           (12'(H_VISIBLE)),
            .V_visible           (12'(V_VISIBLE)),
            .H_total             (12'(H_TOTAL)),
            .V_total             (12'(V_TOTAL)),
            .H_sync_width        (12'(H_SYNC_WIDTH)),
            .V_sync_width        (12'(V_SYNC_WIDTH)),
            .H_start             (12'(H_START)),
            .V_start             (12'(V_START)),
            .H_vsync_active_high (H_SYNC_ACTIVE_HIGH),
            .V_vsync_active_high (V_SYNC_ACTIVE_HIGH),
            .flag_sync_clock     (1'b1),
            .flag_YCCnRGB        (1'b0),
            .flag_422n444        (1'b0),
            .flag_range_reduced  (1'b0),
            .flag_interlaced_even(1'b0),
            .flag_YCC_colour_709 (1'b0),
            .flags_3d_Indicators (2'b00),
            .bits_per_colour     (5'b01000),
            .in_data             (sdp_merged_data),
            .out_data            (msa_merged_data)
        );
    end
    endgenerate

    // ------------------------------------------------------------------
    // Main link datapath: idle insertion, scrambling, training patterns
    // ------------------------------------------------------------------
    main_stream_processing i_main_stream_processing(
        .symbol_clk          (tx_symbol_clk),
        .tx_link_established (tx_link_established),
        .source_ready        (test_signal_ready),
        .tx_clock_train      (tx_clock_train),
        .tx_align_train      (tx_align_train),
        .in_data             (msa_merged_data),
        .tx_symbols          (tx_symbols)
    );

    // ------------------------------------------------------------------
    // Link policy: AUX channel, EDID/DPCD, link training
    // ------------------------------------------------------------------
    channel_management #(.LINK_RATE_MBPS(LINK_RATE_MBPS)) i_channel_management(
        .clk100               (clk100),
        .debug                (debug),
        .hpd                  (hpd),
        .auxch_in             (auxch_in),
        .auxch_out            (auxch_out),
        .auxch_tri            (auxch_tri),
        .stream_channel_count (stream_channel_count),
        .source_channel_count (source_channel_count),
        .tx_clock_train       (tx_clock_train),
        .tx_align_train       (tx_align_train),
        .tx_powerup_channel   (tx_powerup_channel),
        .tx_preemp_0p0        (preemp_0p0),
        .tx_preemp_3p5        (preemp_3p5),
        .tx_preemp_6p0        (preemp_6p0),
        .tx_swing_0p4         (swing_0p4),
        .tx_swing_0p6         (swing_0p6),
        .tx_swing_0p8         (swing_0p8),
        .tx_running           (tx_running),
        .tx_link_established  (tx_link_established)
    );

    // ------------------------------------------------------------------
    // Vendor PHY
    // ------------------------------------------------------------------
`ifdef DP_VENDOR_XILINX_GTP
    transceiver_bank i_transceiver_bank(
        .mgmt_clk        (clk100),
        .powerup_channel (tx_powerup_channel[1:0]),
        .preemp_0p0      (preemp_0p0),
        .preemp_3p5      (preemp_3p5),
        .preemp_6p0      (preemp_6p0),
        .swing_0p4       (swing_0p4),
        .swing_0p6       (swing_0p6),
        .swing_0p8       (swing_0p8),
        .tx_running      (tx_running[1:0]),
        .refclk0         (refclk0),
        .refclk1         (refclk1),
        .tx_symbol_clk   (tx_symbol_clk),
        .tx_symbols      (tx_symbols),
        .gtptx_p         (dp_tx_lane_p),
        .gtptx_n         (dp_tx_lane_n)
    );
    assign tx_running[3:2] = 2'b00;
`elsif DP_VENDOR_GOWIN
    transceiver_bank_gowin i_transceiver_bank(
        .mgmt_clk        (clk100),
        .powerup_channel (tx_powerup_channel[1:0]),
        .preemp_0p0      (preemp_0p0),
        .preemp_3p5      (preemp_3p5),
        .preemp_6p0      (preemp_6p0),
        .swing_0p4       (swing_0p4),
        .swing_0p6       (swing_0p6),
        .swing_0p8       (swing_0p8),
        .tx_running      (tx_running[1:0]),
        .refclk0         (refclk0),
        .refclk1         (refclk1),
        .tx_symbol_clk   (tx_symbol_clk),
        .tx_symbols      (tx_symbols),
        .gtptx_p         (dp_tx_lane_p),
        .gtptx_n         (dp_tx_lane_n)
    );
    assign tx_running[3:2] = 2'b00;
`else
    // Behavioural stub for simulation and lint: refclk0 stands in for the
    // recovered symbol clock, lanes are tied off, all channels report running
    // once powered.
    assign tx_symbol_clk = refclk0;
    assign tx_running    = tx_powerup_channel;
    assign dp_tx_lane_p  = '0;
    assign dp_tx_lane_n  = '1;
`endif

endmodule
