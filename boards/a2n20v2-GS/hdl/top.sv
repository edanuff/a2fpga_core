//
// Top module for Tang Nano 20K and A2N20v2 Apple II card
//
// (c) 2023,2024 Ed Anuff <ed@a2fpga.com> 
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

`include "datetime.svh"

module top #(
    parameter int CLOCK_SPEED_HZ = 54_000_000,
    parameter int MEM_MHZ = CLOCK_SPEED_HZ / 1_000_000,

    parameter bit SCANLINES_ENABLE = 0,
    parameter bit APPLE_SPEAKER_ENABLE = 0,

    parameter bit SUPERSPRITE_ENABLE = 1,
    parameter bit [7:0] SUPERSPRITE_ID = 1,
    parameter bit SUPERSPRITE_FORCE_VDP_OVERLAY = 0,

    parameter bit MOCKINGBOARD_ENABLE = 1,
    parameter bit [7:0] MOCKINGBOARD_ID = 2,

    parameter bit SUPERSERIAL_ENABLE = 1,
    parameter bit SUPERSERIAL_IRQ_ENABLE = 1,
    parameter bit [7:0] SUPERSERIAL_ID = 3,

    parameter bit ENSONIQ_ENABLE = 1,
    parameter bit ENSONIQ_MONO_MIX = 0, // If true, mono mix is used instead of stereo

    parameter bit CLEAR_APPLE_VIDEO_RAM = 1,    // Clear video ram on startup
    parameter bit HDMI_SLEEP_ENABLE = 1,        // Sleep HDMI output on CPU stop
    parameter bit IRQ_OUT_ENABLE = 1,           // Allow driving IRQ to Apple bus
    parameter bit BUS_DATA_OUT_ENABLE = 1       // Allow driving data to Apple bus

) (
    // fpga clocks
    input clk,

    // fpga buttons
    input s1,
    input s2,

    // A2 signals
    input a2_reset_n,
    input a2_phi1,
    input a2_7M,

    // A2Bridge signals
    output [2:0] a2_bridge_sel,
    output a2_bridge_bus_a_oe_n,
    output a2_bridge_bus_d_oe_n,
    output a2_bridge_rd_n,
    output a2_bridge_wr_n,
    inout [7:0] a2_bridge_d,

    // hdmi ports
    output tmds_clk_p,
    output tmds_clk_n,
    output [2:0] tmds_d_p,
    output [2:0] tmds_d_n,

    // leds
    output reg [4:0] led,

    // uart
    output  uart_tx,
    input  uart_rx,

    // "Magic" port names that the gowin compiler connects to the on-chip SDRAM
    output        O_sdram_clk,
    output        O_sdram_cke,
    output        O_sdram_cs_n,   // chip select
    output        O_sdram_cas_n,  // columns address select
    output        O_sdram_ras_n,  // row address select
    output        O_sdram_wen_n,  // write enable
    inout  [31:0] IO_sdram_dq,    // 32 bit bidirectional data bus
    output [10:0] O_sdram_addr,   // 11 bit multiplexed address bus
    output [ 1:0] O_sdram_ba,     // two banks
    output [ 3:0] O_sdram_dqm     // 32/4

);

    wire rst_n = ~s1;

    // Clocks

    wire clk_logic_w;
    wire clk_logic_p_w;
    wire clk_logic_lock_w;
    wire clk_pixel_w;
    wire clk_hdmi_w;
    wire clk_hdmi_lock_w;
    wire hdmi_rst_n_w;
    wire a2_2M;

    // PLL - 54hz from 27
    clk_logic clk_logic_inst (
        .clkout(clk_logic_w),  //output clkout
        .lock(clk_logic_lock_w),  //output lock
        .clkoutp(clk_logic_p_w),  //output clkoutp
        .clkoutd(clk_pixel_w),  //output clkoutd
        .reset(~rst_n),  //input reset
        .clkin(clk)  //input clkin
    );

    // PLL - 135Mhz from 27
    clk_hdmi clk_hdmi_inst (
        .clkout(clk_hdmi_w),  //output clkout
        .lock(clk_hdmi_lock_w),  //output lock
        .reset(~clk_logic_lock_w),  //input reset
        .clkin(clk_pixel_w)  //input clkin
    );

    // Reset

    wire device_reset_n_w = rst_n & clk_logic_lock_w & clk_hdmi_lock_w;

    wire system_reset_n_w = device_reset_n_w & a2_reset_n;

    // SDRAM Controller

    wire sdram_init_complete;

    // SDRAM ports: lower number = higher priority
    localparam FB_READ_PORT  = 0;   // Framebuffer line reads (highest priority)
    localparam FB_WRITE_PORT = 1;   // Framebuffer pixel writes
    localparam DOC_MEM_PORT  = 2;   // Ensoniq DOC sound memory
    localparam GLU_MEM_PORT  = 3;   // Ensoniq GLU registers
    localparam NUM_PORTS     = 4;

    localparam PORT_ADDR_WIDTH = 21;
    localparam DATA_WIDTH = 32;
    localparam DQM_WIDTH = 4;
    localparam PORT_OUTPUT_WIDTH = 32;

    mem_port_if #(
        .PORT_ADDR_WIDTH(PORT_ADDR_WIDTH),
        .DATA_WIDTH(DATA_WIDTH),
        .DQM_WIDTH(DQM_WIDTH),
        .PORT_OUTPUT_WIDTH(PORT_OUTPUT_WIDTH)
    ) mem_ports[NUM_PORTS-1:0]();

    sdram_ports #(
        .CLOCK_SPEED_MHZ(MEM_MHZ),
        .NUM_PORTS(NUM_PORTS),
        .PORT_ADDR_WIDTH(PORT_ADDR_WIDTH),
        .PORT_OUTPUT_WIDTH(PORT_OUTPUT_WIDTH),
        .CAS_LATENCY(2),
        .SETTING_REFRESH_TIMER_NANO_SEC(15000),
        .SETTING_T_WR_MIN_WRITE_AUTO_PRECHARGE_RECOVERY_NANO_SEC(16),
        .SETTING_USE_FAST_INPUT_REGISTER(0),
        .BURST_LENGTH(4),
        .READ_BURST_LENGTH(16),
        .PORT_BURST_LENGTH(1),
        .DATA_WIDTH(DATA_WIDTH),
        .ROW_WIDTH(11),
        .COL_WIDTH(8),
        .PRECHARGE_BIT(10),
        .DQM_WIDTH(DQM_WIDTH)
    ) sdram_ports (
        .clk(clk_logic_w),
        .sdram_clk(clk_logic_p_w),
        .reset(!device_reset_n_w),
        .init_complete(sdram_init_complete),

        .ports(mem_ports),

        .SDRAM_DQ(IO_sdram_dq),
        .SDRAM_A(O_sdram_addr),
        .SDRAM_DQM(O_sdram_dqm),
        .SDRAM_BA(O_sdram_ba),
        .SDRAM_nCS(O_sdram_cs_n),
        .SDRAM_nWE(O_sdram_wen_n),
        .SDRAM_nRAS(O_sdram_ras_n),
        .SDRAM_nCAS(O_sdram_cas_n),
        .SDRAM_CKE(O_sdram_cke),
        .SDRAM_CLK(O_sdram_clk)
    );

    // Interface to Apple II

    // data and address latches on input

    a2bus_if a2bus_if ();

    wire sleep_w;
    wire data_in_strobe_w;

    wire irq_n_w;

    wire data_out_en_w;
    wire [7:0] data_out_w;

    wire [7:0] a2_bridge_d_buf_w;
    wire [7:0] a2_bridge_d_o_w;
    wire a2_bridge_d_oe_w;

    wire [3:0] dip_switches_n_w;
    wire sw_scanlines_w = !dip_switches_n_w[0];
    wire sw_apple_speaker_w = !dip_switches_n_w[1];
    wire sw_slot_7_w = !dip_switches_n_w[2];
    wire sw_gs_w = !dip_switches_n_w[3];

    IOBUF a2_bridge_d_iobuf[7:0] (
        .O  (a2_bridge_d_buf_w),
        .IO (a2_bridge_d),
        .I  (a2_bridge_d_o_w),
        .OEN(!a2_bridge_d_oe_w)
    );

    apple_bus #(
        .CLOCK_SPEED_HZ(CLOCK_SPEED_HZ),
        .BUS_DATA_OUT_ENABLE(BUS_DATA_OUT_ENABLE),
        .IRQ_OUT_ENABLE(IRQ_OUT_ENABLE)
    ) apple_bus (
        .clk_logic_i(clk_logic_w),
        .clk_pixel_i(clk_pixel_w),
        .system_reset_n_i(system_reset_n_w),
        .device_reset_n_i(device_reset_n_w),
        .a2_phi1_i(a2_phi1),
        .a2_q3_i(1'b0),
        .a2_7M_i(a2_7M),

        .a2bus_if(a2bus_if),

        .a2_bridge_sel_o(a2_bridge_sel),
        .a2_bridge_bus_a_oe_n_o(a2_bridge_bus_a_oe_n),
        .a2_bridge_bus_d_oe_n_o(a2_bridge_bus_d_oe_n),
        .a2_bridge_rd_n_o(a2_bridge_rd_n),
        .a2_bridge_wr_n_o(a2_bridge_wr_n),
        .a2_bridge_d_i(a2_bridge_d_buf_w),
        .a2_bridge_d_o(a2_bridge_d_o_w),
        .a2_bridge_d_oe_o(a2_bridge_d_oe_w),

        .data_out_en_i(data_out_en_w),
        .data_out_i(data_out_w),

        .irq_n_i(irq_n_w),

        .dip_switches_n_o(dip_switches_n_w),

        .sleep_o(sleep_w)
    );

    // Memory

    a2mem_if a2mem_if();

    wire [15:0] video_address_w;
    wire video_bank_w;
    wire video_rd_w;
    wire [31:0] video_data_w;

    wire vgc_active_w;
    wire [12:0] vgc_address_w;
    wire vgc_rd_w;
    wire [31:0] vgc_data_w;

    apple_memory #(
        .VGC_MEMORY(1)
    ) apple_memory (
        .a2bus_if(a2bus_if),
        .a2mem_if(a2mem_if),

        .video_address_i(video_address_w),
        .video_rd_i(video_rd_w),
        .video_data_o(video_data_w),

        .vgc_active_i(vgc_active_w),
        .vgc_address_i(vgc_address_w),
        .vgc_rd_i(vgc_rd_w),
        .vgc_data_o(vgc_data_w)
    );

    // Slots

    slot_if slot_if();
    slotmaker_config_if slotmaker_config_if();

    slotmaker slotmaker (
        .a2bus_if(a2bus_if),
        .a2mem_if(a2mem_if),

        .cfg_if(slotmaker_config_if),

        .slot_if(slot_if)
    );

    assign slotmaker_config_if.slot = 3'b0;
    assign slotmaker_config_if.wr = 1'b0;
    assign slotmaker_config_if.card_i = 8'b0;
    assign slotmaker_config_if.reconfig = 1'b0;

    // Video

    video_control_if video_control_if();
    assign video_control_if.enable = 1'b0;
    assign video_control_if.TEXT_MODE = 1'b0;
    assign video_control_if.MIXED_MODE = 1'b0;
    assign video_control_if.PAGE2 = 1'b0;
    assign video_control_if.HIRES_MODE = 1'b0;
    assign video_control_if.AN3 = 1'b0;
    assign video_control_if.STORE80 = 1'b0;
    assign video_control_if.COL80 = 1'b0;
    assign video_control_if.ALTCHAR = 1'b0;
    assign video_control_if.TEXT_COLOR = 4'b0;
    assign video_control_if.BACKGROUND_COLOR = 4'b0;
    assign video_control_if.BORDER_COLOR = 4'b0;
    assign video_control_if.MONOCHROME_MODE = 1'b0;
    assign video_control_if.MONOCHROME_DHIRES_MODE = 1'b0;
    assign video_control_if.SHRG_MODE = 1'b0;

    // HDMI raster position (from HDMI encoder, pixel clock domain)
    wire [9:0] hdmi_x;
    wire [9:0] hdmi_y;

    // Scan timer — generates scanline/hsync/vsync from Apple II bus timing
    wire [8:0] scanline_w;
    wire hsync_w;
    wire vsync_w;
    wire [9:0] pixel_w;

    // Scan timer debug outputs for DebugOverlay
    wire [8:0] scan_dbg_delta_w;
    wire [8:0] scan_dbg_expected_w;
    wire [8:0] scan_dbg_actual_w;
    wire [7:0] scan_dbg_raw_data_w;
    wire [7:0] scan_dbg_vbl_correct_w;
    wire [7:0] scan_dbg_vertcnt_correct_w;
    wire [7:0] scan_dbg_c02e_cnt_w;
    wire [7:0] scan_dbg_c019_cnt_w;

    scan_timer #(
        .VGC_VERTCNT_LOCK(1),
        .VGC_VBL_LOCK(1),
        .RESYNC_THRESHOLD(2)
    ) scan_timer (
        .a2bus_if(a2bus_if),
        .scanline_o(scanline_w),
        .hsync_o(hsync_w),
        .vsync_o(vsync_w),
        .pixel_o(pixel_w),
        .dbg_last_delta_o(scan_dbg_delta_w),
        .dbg_last_expected_o(scan_dbg_expected_w),
        .dbg_last_actual_o(scan_dbg_actual_w),
        .dbg_last_raw_data_o(scan_dbg_raw_data_w),
        .dbg_vbl_correct_o(scan_dbg_vbl_correct_w),
        .dbg_vertcnt_correct_o(scan_dbg_vertcnt_correct_w),
        .dbg_c02e_count_o(scan_dbg_c02e_cnt_w),
        .dbg_c019_count_o(scan_dbg_c019_cnt_w)
    );

    // Apple II video → framebuffer scanner
    wire fb_we_w;
    wire [17:0] fb_data_w;
    wire fb_vsync_w;

    // Apple II RGB outputs for SuperSprite compositing
    wire [7:0] apple_fb_r_w;
    wire [7:0] apple_fb_g_w;
    wire [7:0] apple_fb_b_w;
    wire       apple_fb_active_w;
    wire [7:0] rgb_r_w;
    wire [7:0] rgb_g_w;
    wire [7:0] rgb_b_w;

    apple_video_fb apple_video_fb (
        .a2bus_if(a2bus_if),
        .a2mem_if(a2mem_if),

        .video_control_if(video_control_if),

        .video_address_o(video_address_w),
        .video_bank_o(video_bank_w),
        .video_rd_o(video_rd_w),
        .video_data_i(video_data_w),

        .scanline_i(scanline_w),
        .hsync_i(hsync_w),
        .vsync_i(vsync_w),

        .fb_we_o(fb_we_w),
        .fb_data_o(fb_data_w),
        .fb_vsync_o(fb_vsync_w),

        // Apple II RGB output for SuperSprite
        .apple_r_o(apple_fb_r_w),
        .apple_g_o(apple_fb_g_w),
        .apple_b_o(apple_fb_b_w),
        .apple_active_o(apple_fb_active_w),

        // SuperSprite composited input
        .ssp_r_i(rgb_r_w),
        .ssp_g_i(rgb_g_w),
        .ssp_b_i(rgb_b_w),
        .ssp_active_i(1'b1)
    );

    // VGC framebuffer scanner — IIgs Super Hi-Res modes
    wire vgc_fb_we_w;
    wire [17:0] vgc_fb_data_w;
    wire vgc_fb_vsync_w;

    vgc_fb vgc_fb (
        .a2bus_if(a2bus_if),
        .a2mem_if(a2mem_if),

        .video_control_if(video_control_if),

        .vgc_active_o(vgc_active_w),
        .vgc_address_o(vgc_address_w),
        .vgc_rd_o(vgc_rd_w),
        .vgc_data_i(vgc_data_w),

        .scanline_i(scanline_w),
        .hsync_i(hsync_w),
        .vsync_i(vsync_w),

        .fb_we_o(vgc_fb_we_w),
        .fb_data_o(vgc_fb_data_w),
        .fb_vsync_o(vgc_fb_vsync_w)
    );

    // Framebuffer output mux — select apple_video_fb or vgc_fb based on SHRG_MODE
    // Latched at frame boundary for clean transitions
    reg use_vgc_r;
    always @(posedge clk_logic_w) begin
        if (vsync_w) use_vgc_r <= a2mem_if.SHRG_MODE;
    end

    wire fb_we_mux_w          = use_vgc_r ? vgc_fb_we_w    : fb_we_w;
    wire [17:0] fb_data_mux_w = use_vgc_r ? vgc_fb_data_w  : fb_data_w;
    wire fb_vsync_mux_w       = use_vgc_r ? vgc_fb_vsync_w : fb_vsync_w;

    wire [10:0] fb_width_w  = use_vgc_r ? 11'd640 : 11'd560;
    wire [9:0]  fb_height_w = use_vgc_r ? 10'd200 : 10'd192;

    // SuperSprite

    wire VDP_OVERLAY_SW;
    wire APPLE_VIDEO_SW;
    wire [0:7] ssp_d_w;
    wire ssp_rd;
    wire [3:0] vdp_r;
    wire [3:0] vdp_g;
    wire [3:0] vdp_b;
    wire vdp_transparent;
    wire vdp_ext_video;
    wire vdp_irq_n;
    wire [9:0] ssp_audio_w;
    wire vdp_unlocked_w;
    wire [3:0] vdp_gmode_w;
    wire scanlines_w;

    f18a_gpu_if f18a_gpu_if();
    assign f18a_gpu_if.running = 1'b0;
    assign f18a_gpu_if.pause_ack = 1'b1;
    assign f18a_gpu_if.vwe = 1'b0;
    assign f18a_gpu_if.vaddr = 14'b0;
    assign f18a_gpu_if.vdout = 8'b0;
    assign f18a_gpu_if.pwe = 1'b0;
    assign f18a_gpu_if.paddr = 6'b0;
    assign f18a_gpu_if.pdout = 12'b0;
    assign f18a_gpu_if.rwe = 1'b0;
    assign f18a_gpu_if.raddr = 13'b0;
    assign f18a_gpu_if.gstatus = 7'b0;

    // VDP Raster Counter — synced to Apple II scan_timer (clk_logic domain)
    // vdp_cx: horizontal counter advancing once per 4 clk_logic cycles
    // vdp_cy: uses scanline_w directly from scan_timer (0–261)
    localparam VDP_HMAX = 10'd856;

    reg [9:0] vdp_cx;
    reg [1:0] vdp_div;

    reg hsync_prev_r;
    always @(posedge a2bus_if.clk_logic) begin
        hsync_prev_r <= hsync_w;
    end

    always @(posedge a2bus_if.clk_logic) begin
        if (hsync_w) begin
            vdp_cx <= 10'd0;
            vdp_div <= 2'd0;
        end else begin
            vdp_div <= vdp_div + 2'd1;
            if (vdp_div == 2'd3 && vdp_cx < VDP_HMAX) begin
                vdp_cx <= vdp_cx + 10'd1;
            end
        end
    end

    SuperSprite #(
        .ENABLE(SUPERSPRITE_ENABLE),
        .ID(SUPERSPRITE_ID),
        .FORCE_VDP_OVERLAY(SUPERSPRITE_FORCE_VDP_OVERLAY)
    ) supersprite (
        .a2bus_if(a2bus_if),
        .slot_if(slot_if),

        .data_o(ssp_d_w),
        .rd_en_o(ssp_rd),
        .irq_n_o(vdp_irq_n),

        .screen_x_i(vdp_cx),
        .screen_y_i({1'b0, scanline_w}),
        .apple_vga_r_i(apple_fb_r_w),
        .apple_vga_g_i(apple_fb_g_w),
        .apple_vga_b_i(apple_fb_b_w),
        .apple_vga_active_i(apple_fb_active_w),

        .scanlines_i(SCANLINES_ENABLE | sw_scanlines_w),

        .ssp_r_o(rgb_r_w),
        .ssp_g_o(rgb_g_w),
        .ssp_b_o(rgb_b_w),

        .scanlines_o(scanlines_w),

        .vdp_ext_video_o(vdp_ext_video),
        .vdp_unlocked_o(vdp_unlocked_w),
        .vdp_gmode_o(vdp_gmode_w),

        .f18a_gpu_if(f18a_gpu_if),

        .ssp_audio_o(ssp_audio_w)
    );

    // Mockingboard

    wire [7:0] mb_d_w;
    wire mb_rd;
    wire mb_irq_n;
    wire [9:0] mb_audio_l;
    wire [9:0] mb_audio_r;

    Mockingboard #(
        .ENABLE(MOCKINGBOARD_ENABLE),
        .ID(MOCKINGBOARD_ID)
    ) mockingboard (
        .a2bus_if(a2bus_if),  // use system_reset_n
        .slot_if(slot_if),

        .data_o(mb_d_w),
        .rd_en_o(mb_rd),
        .irq_n_o(mb_irq_n),

        .audio_l_o(mb_audio_l),
        .audio_r_o(mb_audio_r)
    );

    // SuperSerial Card

    wire [7:0] ssc_d_w;
    wire ssc_rd;
    wire ssc_irq_n;
    wire ssc_rom_en;

    wire ssc_uart_rx;
    wire ssc_uart_tx;
    assign ssc_uart_rx = uart_rx;
    assign uart_tx = ssc_uart_tx;

    SuperSerial #(
        .CLOCK_SPEED_HZ(CLOCK_SPEED_HZ),
        .ENABLE(SUPERSERIAL_ENABLE),
        .IRQ_ENABLE(SUPERSERIAL_IRQ_ENABLE),
        .ID(SUPERSERIAL_ID)
    ) superserial (
        .a2bus_if(a2bus_if),
        .a2mem_if(a2mem_if),
        .slot_if(slot_if),

        .data_o(ssc_d_w),
        .rd_en_o(ssc_rd),
        .irq_n_o(ssc_irq_n),

        .rom_en_o(ssc_rom_en),
        .uart_rx_i(ssc_uart_rx),
        .uart_tx_o(ssc_uart_tx)
    );

    // Data output

    assign data_out_en_w = ssp_rd || mb_rd || ssc_rd;

    assign data_out_w = ssc_rd ? ssc_d_w :
        ssp_rd ? ssp_d_w : 
        mb_rd ? mb_d_w : 
        a2bus_if.data;

    // Interrupts

    assign irq_n_w = mb_irq_n && vdp_irq_n && ssc_irq_n;

    // Audio

    wire speaker_audio_w;

    apple_speaker apple_speaker (
        .a2bus_if(a2bus_if),
        .enable(APPLE_SPEAKER_ENABLE | sw_apple_speaker_w),
        .speaker_o(speaker_audio_w)
    );

    // Ensoniq DOC5503 Sound

    wire [15:0] sg_audio_l;
    wire [15:0] sg_audio_r;
    wire [7:0] sg_d_w;
    wire sg_rd_w;
    wire [7:0] doc_osc_en_w;
    wire [1:0] doc_osc_mode_w[8];
    wire [7:0] doc_osc_halt_w;

    sound_glu #(
        .ENABLE(ENSONIQ_ENABLE),
        .MONO_MIX(ENSONIQ_MONO_MIX)
    ) sg (
        .a2bus_if(a2bus_if),
        .data_o(sg_d_w),
        .rd_en_o(sg_rd_w),

        .audio_l_o(sg_audio_l),
        .audio_r_o(sg_audio_r),

        .debug_osc_en_o(doc_osc_en_w),
        .debug_osc_mode_o(doc_osc_mode_w),
        .debug_osc_halt_o(doc_osc_halt_w),

        .glu_mem_if(mem_ports[GLU_MEM_PORT]),
        .doc_mem_if(mem_ports[DOC_MEM_PORT])
    );

    // Extend all the unsigned audio signals to 13 bits
    wire [12:0] speaker_audio_ext_w = {speaker_audio_w, 12'b0};
    wire [12:0] ssp_audio_ext_w = {ssp_audio_w, 3'b0};
    wire [12:0] mb_audio_l_ext_w = {mb_audio_l, 3'b0};
    wire [12:0] mb_audio_r_ext_w = {mb_audio_r, 3'b0};

    wire signed [15:0] core_audio_l_w;
    wire signed [15:0] core_audio_r_w;
    // Combine all the audio sources into a single 16-bit signed audio signal
    // This could theoretically overflow by 1 bit and clip, but unlikely
    assign core_audio_l_w = sg_audio_l + ssp_audio_ext_w + mb_audio_l_ext_w + speaker_audio_ext_w;
    assign core_audio_r_w = sg_audio_r + ssp_audio_ext_w + mb_audio_r_ext_w + speaker_audio_ext_w;

    // CDC FIFO to shift audio to the pixel clock domain from the logic clock domain

    wire [15:0] cdc_audio_l;
    wire [15:0] cdc_audio_r;

    cdc_sampling #(
        .WIDTH(16)
    ) audio_cdc_left (
        .rst_n(device_reset_n_w),
        .clk_fast(clk_logic_w),
        .clk_slow(clk_pixel_w),
        .data_in(core_audio_l_w),
        .data_out(cdc_audio_l)
    );

    cdc_sampling #(
        .WIDTH(16)
    ) audio_cdc_right (
        .rst_n(device_reset_n_w),
        .clk_fast(clk_logic_w),
        .clk_slow(clk_pixel_w),
        .data_in(core_audio_r_w),
        .data_out(cdc_audio_r)
    );

    localparam [31:0] aflt_rate = 7_056_000;
    localparam [39:0] acx  = 4258969;
    localparam  [7:0] acx0 = 3;
    localparam  [7:0] acx1 = 3;
    localparam  [7:0] acx2 = 1;
    localparam [23:0] acy0 = -24'd6216759;
    localparam [23:0] acy1 =  24'd6143386;
    localparam [23:0] acy2 = -24'd2023767;

    localparam AUDIO_RATE = 44100;
    localparam AUDIO_BIT_WIDTH = 16;
    wire clk_audio_w;
    audio_timing #(
        .CLK_RATE(CLOCK_SPEED_HZ / 2),
        .AUDIO_RATE(AUDIO_RATE)
    ) audio_timing (
        .reset(~device_reset_n_w),
        .clk(clk_pixel_w),
        .audio_clk(clk_audio_w),
        .i2s_bclk(),
        .i2s_lrclk(),
        .i2s_data_shift_strobe(),
        .i2s_data_load_strobe()
    );

    wire [15:0] audio_sample_word[1:0];
    audio_out #(
        .CLK_RATE(CLOCK_SPEED_HZ / 2),
        .AUDIO_RATE(AUDIO_RATE)
    ) audio_out
    (
        .reset(~device_reset_n_w),
        .clk(clk_pixel_w),

        .flt_rate(aflt_rate),
        .cx(acx),
        .cx0(acx0),
        .cx1(acx1),
        .cx2(acx2),
        .cy0(acy0),
        .cy1(acy1),
        .cy2(acy2),

        .is_signed(1'b1),
        .core_l(cdc_audio_l),
        .core_r(cdc_audio_r),

        .audio_clk(clk_audio_w),
        .audio_l(audio_sample_word[0]),
        .audio_r(audio_sample_word[1])
    );

    // Border color: convert 4-bit palette index to RGB666
    wire border_gsp_w = a2bus_if.sw_gs;
    wire [4:0] border_idx_w = {border_gsp_w, a2mem_if.BORDER_COLOR};
    wire [11:0] border_palette_w [0:31];
    assign border_palette_w = '{
        12'h000, 12'h924, 12'h42a, 12'hd4e,   // Apple II  0-3
        12'h064, 12'h888, 12'h39e, 12'hcbf,   //           4-7
        12'h450, 12'hc73, 12'h888, 12'hfac,   //           8-11
        12'h3c2, 12'hcd6, 12'h7ec, 12'hfff,   //          12-15
        12'h000, 12'hd03, 12'h009, 12'hd2d,   // IIgs      0-3
        12'h072, 12'h555, 12'h22f, 12'h6af,   //           4-7
        12'h850, 12'hf60, 12'haaa, 12'hf98,   //           8-11
        12'h1d0, 12'hff0, 12'h4f9, 12'hfff    //          12-15
    };
    wire [11:0] border_rgb444_w = border_palette_w[border_idx_w];
    wire [17:0] border_rgb666_w = {
        border_rgb444_w[11:8], 2'b00,
        border_rgb444_w[7:4],  2'b00,
        border_rgb444_w[3:0],  2'b00
    };

    // SDRAM Framebuffer
    wire [7:0] fb_r_w;
    wire [7:0] fb_g_w;
    wire [7:0] fb_b_w;
    wire [7:0] fb_dbg_fifo_level_w;
    wire [7:0] fb_dbg_fifo_highwater_w;
    wire [7:0] fb_dbg_fifo_overflow_w;
    wire [7:0] fb_dbg_fetch_start_w;
    wire [7:0] fb_dbg_fetch_done_w;
    wire [7:0] fb_dbg_read_blocked_w;
    wire [7:0] fb_dbg_yield_busy_w;
    wire [7:0] fb_dbg_late_line_w;
    wire [7:0] fb_dbg_line_not_ready_w;
    wire [7:0] fb_dbg_line_lag_max_w;
    wire [7:0] fb_dbg_ready_phase_err_w;
    wire [7:0] fb_dbg_vsync_raw_w;
    wire [7:0] fb_dbg_frame_start_accept_w;
    wire [7:0] fb_dbg_frame_start_reject_w;
    wire [7:0] fb_dbg_flags_w;

    sdram_framebuffer sdram_framebuffer (
        .clk(clk_logic_w),
        .clk_pixel(clk_pixel_w),
        .rst_n(device_reset_n_w),

        .fb_vsync(fb_vsync_mux_w),
        .fb_we(fb_we_mux_w),
        .fb_data(fb_data_mux_w),
        .fb_width(fb_width_w),
        .fb_height(fb_height_w),

        .fb_write_port(mem_ports[FB_WRITE_PORT]),
        .fb_read_port(mem_ports[FB_READ_PORT]),

        .hdmi_cx({1'b0, hdmi_x}),
        .hdmi_cy(hdmi_y),

        .r_o(fb_r_w),
        .g_o(fb_g_w),
        .b_o(fb_b_w),

        .border_color(border_rgb666_w),
        .scanline_en(scanlines_w),
        .sleep_i(sleep_w),

        .dbg_fifo_level_o(fb_dbg_fifo_level_w),
        .dbg_fifo_highwater_o(fb_dbg_fifo_highwater_w),
        .dbg_fifo_overflow_o(fb_dbg_fifo_overflow_w),
        .dbg_fetch_start_o(fb_dbg_fetch_start_w),
        .dbg_fetch_done_o(fb_dbg_fetch_done_w),
        .dbg_read_blocked_o(fb_dbg_read_blocked_w),
        .dbg_yield_busy_o(fb_dbg_yield_busy_w),
        .dbg_late_line_o(fb_dbg_late_line_w),
        .dbg_line_not_ready_o(fb_dbg_line_not_ready_w),
        .dbg_line_lag_max_o(fb_dbg_line_lag_max_w),
        .dbg_ready_phase_err_o(fb_dbg_ready_phase_err_w),
        .dbg_vsync_raw_o(fb_dbg_vsync_raw_w),
        .dbg_frame_start_accept_o(fb_dbg_frame_start_accept_w),
        .dbg_frame_start_reject_o(fb_dbg_frame_start_reject_w),
        .dbg_flags_o(fb_dbg_flags_w)
    );

    // HDMI

    reg show_debug_overlay_r = 1'b0;

    wire [7:0] debug_r_w;
    wire [7:0] debug_g_w;
    wire [7:0] debug_b_w;
    DebugOverlay #(
        .VERSION(`BUILD_DATETIME),  // 14-digit timestamp version
        .ENABLE(1'b1)
    ) debug_overlay (
        .clk_i          (clk_pixel_w),
        .reset_n (device_reset_n_w),
        .enable_i(show_debug_overlay_r),

        .hex_values ({
            fb_dbg_fifo_level_w,
            fb_dbg_fifo_highwater_w,
            fb_dbg_fifo_overflow_w,
            fb_dbg_line_not_ready_w,
            fb_dbg_line_lag_max_w,
            fb_dbg_vsync_raw_w,
            fb_dbg_frame_start_accept_w,
            fb_dbg_frame_start_reject_w
        }),

        .debug_bits_0_i (fb_dbg_flags_w),
        .debug_bits_1_i ({
            1'b0,
            scanlines_w,
            use_vgc_r,
            a2mem_if.SHRG_MODE,
            a2mem_if.HIRES_MODE,
            a2mem_if.TEXT_MODE,
            sdram_init_complete,
            sleep_w
        }),

        .screen_x_i     (hdmi_x),
        .screen_y_i     (hdmi_y),

        .r_i            (fb_r_w),
        .g_i            (fb_g_w),
        .b_i            (fb_b_w),

        .r_o            (debug_r_w),
        .g_o            (debug_g_w),
        .b_o            (debug_b_w)
    );  

    logic [2:0] tmds;
    wire tmdsClk;

    hdmi #(
        .VIDEO_ID_CODE(2),
        .DVI_OUTPUT(0),
        .VIDEO_REFRESH_RATE(59.94),
        .IT_CONTENT(1),
        .AUDIO_RATE(AUDIO_RATE),
        .AUDIO_BIT_WIDTH(AUDIO_BIT_WIDTH),
        .VENDOR_NAME({"Unknown", 8'd0}),  // Must be 8 bytes null-padded 7-bit ASCII
        .PRODUCT_DESCRIPTION({"FPGA", 96'd0}),  // Must be 16 bytes null-padded 7-bit ASCII
        .SOURCE_DEVICE_INFORMATION(8'h00), // See README.md or CTA-861-G for the list of valid codes
        .START_X(0),
        .START_Y(0)
    ) hdmi (
        .clk_pixel_x5(clk_hdmi_w),
        .clk_pixel(clk_pixel_w),
        .clk_audio(clk_audio_w),
        .rgb({
            debug_r_w,
            debug_g_w,
            debug_b_w
        }),
        .reset(~device_reset_n_w),
        .audio_sample_word(audio_sample_word),
        .tmds(tmds),
        .tmds_clock(tmdsClk),
        .cx(hdmi_x),
        .cy(hdmi_y),
        .frame_width(),
        .frame_height(),
        .screen_width(),
        .screen_height()
    );

    // Gowin LVDS output buffer
    ELVDS_TBUF tmds_bufds[3:0] (
        .I({clk_pixel_w, tmds}),
        .O({tmds_clk_p, tmds_d_p}),
        .OB({tmds_clk_n, tmds_d_n}),
        .OEN(sleep_w && HDMI_SLEEP_ENABLE)
    );

    wire s2_debounced_w;
    debounce #(
        .DEBOUNCE_TIME(10000)
    ) debounce_a2reset (
        .clk(clk_logic_w),
        .rst(~device_reset_n_w),
        .i(s2),
        .o(s2_debounced_w)
    );

    reg prev_button_s2 = 1'b0;
    wire button_s2_posedge_w = s2_debounced_w && !prev_button_s2;
    always @(posedge clk_logic_w) begin 
        prev_button_s2 <= s2_debounced_w;
        if (button_s2_posedge_w) begin
            show_debug_overlay_r <= !show_debug_overlay_r;
        end
        //led <= {4'b1111, !picosoc_led};
        //if (!s2) 
        led <= {!a2mem_if.TEXT_MODE, !a2mem_if.SHRG_MODE, !a2mem_if.HIRES_MODE, !a2mem_if.RAMWRT, !a2mem_if.STORE80};
        //if (!s2) led <= {!a2mem_if.TEXT_MODE, !a2mem_if.MIXED_MODE, !a2mem_if.HIRES_MODE, !a2mem_if.RAMWRT, !a2mem_if.STORE80};
        //if (!s2) led <= {!a2mem_if.TEXT_MODE, !a2mem_if.MIXED_MODE, !a2mem_if.HIRES_MODE, !a2mem_if.AN3, !a2mem_if.STORE80};
        //else led <= {!vdp_unlocked_w, ~vdp_gmode_w};
        //else led <= {!vdp_unlocked_w, dip_switches_n_w};
    end


endmodule
