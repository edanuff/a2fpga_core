//
// Top module for Tang Mega 60K and A2Mega Apple II card
//
// (c) 2023,2024,2025 Ed Anuff <ed@a2fpga.com> 
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
    parameter int PIXEL_SPEED_HZ = CLOCK_SPEED_HZ / 2,
    parameter int MEM_MHZ = CLOCK_SPEED_HZ / 1_000_000,

    parameter bit SCANLINES_ENABLE = 0,
    parameter bit APPLE_SPEAKER_ENABLE = 1,

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

    parameter int GS = 0,                       // Apple IIgs mode
    parameter int ENABLE_FILTER = 0,            // Enable audio filtering
    parameter int ENABLE_DENOISE = 0,           // Enable denoise of clocks
    parameter bit CLEAR_APPLE_VIDEO_RAM = 1,    // Clear video ram on startup
    parameter bit HDMI_SLEEP_ENABLE = 0,        // Sleep HDMI output on CPU stop
    parameter bit IRQ_OUT_ENABLE = 1,           // Allow driving IRQ to Apple bus
    parameter bit BUS_DATA_OUT_ENABLE = 1       // Allow driving data to Apple bus

) (
    // fpga clocks
    input clk,
    input resetn,
    input rst,

    // A2 signals
    output a2_bus_oe,

    input  a2_rw_n,
    input  a2_inh_n,
    input  a2_reset_n,
    input  a2_rdy_n,
    output a2_irq_n,
    input  a2_dma_n,
    input  a2_nmi_n,
    input  a2_mb20,
    input  a2_sync_n,
    input  a2_m2sel_n,
    output  a2_res_out_n,
    output a2_int_out_n,
    input  a2_int_in_n,
    output a2_dma_out_n,
    input a2_dma_in_n,
    input  a2_phi1,
    input  a2_q3,
    input  a2_7M,

    output a2_a_dir,
    input [15:0] a2_a,

    output a2_d_dir,
    inout [7:0] a2_d,

    // hdmi ports
    output tmds_clk_p,
    output tmds_clk_n,
    output [2:0] tmds_d_p,
    output [2:0] tmds_d_n,

    input hdmi_hpd,
    output hdmi_scl,
    output hdmi_sda,
    output hdmi_cec,

    // leds
    output [1:0] led,

    input button,  // 0 when pressed

    input [3:0] dip_switches_n,

    // uart
    output  uart_tx,
    input  uart_rx,

    // ddr3 interface
    output [15:0] ddr_addr, //ROW_WIDTH=16
	output [2:0] ddr_bank, //BANK_WIDTH=3
	output ddr_cs,
	output ddr_ras,
	output ddr_cas,
	output ddr_we,
	output ddr_ck,
	output ddr_ck_n,
	output ddr_cke,
	output ddr_odt,
	output ddr_reset_n,
	output [1:0] ddr_dm, //DM_WIDTH=4
	inout  [15:0] ddr_dq, //DQ_WIDTH=32
	inout  [1:0] ddr_dqs, //DQS_WIDTH=4
	inout  [1:0] ddr_dqs_n, //DQS_WIDTH=4

    // ESP32 Octal SPI interface
    input         esp_sclk,
    inout  [7:0]  esp_data

);

    assign hdmi_scl = 1'b1;
    assign hdmi_sda = 1'b1;
    assign hdmi_cec = 1'b0;

    // Clocks

    wire clk_logic_w;
    wire clk_lock_w;
    wire clk_pixel_w;
    wire clk_hdmi_w;
    wire clk_27M_w;

    clk_pll clocks_pll (
        .lock(clk_lock_w), //output lock
        .clkout0(clk_pixel_w), //output clkout0
        .clkout1(clk_hdmi_w), //output clkout1
        .clkout2(clk_logic_w), //output clkout2
        .clkin(clk) //input clkin
    );

    /*
    CLKDIV clkdiv_inst (
        .HCLKIN(clk_hdmi_w),
        .RESETN(clk_lock_w),
        .CALIB(1'b0),
        .CLKOUT(clk_pixel_w)
    );
    defparam clkdiv_inst.DIV_MODE="5";
    */

    // LED blinking logic with ES5503 counter indication
    reg led_r = 1'b0;
    reg [25:0] led_counter_r = 26'd0;

    always @(posedge clk_logic_w) begin

        
        if (led_counter_r == 26'd09_999_999) begin
            led_counter_r <= 0;

            led_r <= ~led_r;  // Normal heartbeat every 0.5s
        end else begin
            led_counter_r <= led_counter_r + 1;
        end
    end
    assign led[0] = !led_r;

    // Power-on reset generation
    localparam RESET_CYCLES = 100;  // Number of clock cycles to hold reset
    
    reg rstn_r = 1'b0;
    reg [$clog2(RESET_CYCLES+1)-1:0] reset_counter_r = '0;

    always @(posedge clk_logic_w) begin
        if (reset_counter_r == RESET_CYCLES) begin
            rstn_r <= 1'b1;  // Release reset after RESET_CYCLES clocks
        end else begin
            reset_counter_r <= reset_counter_r + 1;
        end
    end

    // Reset

    wire device_reset_n_w = rstn_r; // Use reset signal from power-on reset logic

    //wire device_reset_n_w = ~rst;

    wire system_reset_n_w = device_reset_n_w & a2_reset_n;

    // Interface to Apple II

    // Buffer/level shifters are held in tri-state
    // during FPGA configuration to ensure no interference
    // with the Apple II bus.
    assign a2_bus_oe = 1'b0;

    // Address bus is input-only unless performing DMA
    // 0 = from Apple II bus to FPGA, 1 = from FPGA to Apple II bus
    assign a2_a_dir  = 1'b0;

    // data and address latches on input

    a2bus_if a2bus_if ();

    wire sleep_w;

    wire irq_n_w;
    assign a2_irq_n = IRQ_OUT_ENABLE && !irq_n_w ? 1'b0 : 1'bz;

    wire sw_scanlines_w = !dip_switches_n[0];
    wire sw_apple_speaker_w = !dip_switches_n[1];
    wire sw_slot_7_w = !dip_switches_n[2];
    wire sw_gs_w = !dip_switches_n[3];

    wire [7:0] a2_d_buf_w;
    wire data_out_en_w;
    wire [7:0] data_out_w;
    assign a2_d_dir = data_out_en_w && BUS_DATA_OUT_ENABLE;

    IOBUF a2_d_iobuf[7:0] (
        .O  (a2_d_buf_w),
        .IO (a2_d),
        .I  (data_out_w),
        .OEN(!a2_d_dir)
    );

    apple_bus #(
        .GS(GS),
        .CLOCK_SPEED_HZ(CLOCK_SPEED_HZ),
        .ENABLE_DENOISE(ENABLE_DENOISE)
    ) apple_bus (
        .clk_logic_i(clk_logic_w),
        .clk_pixel_i(clk_logic_w),    // F18A runs on clk_logic (54 MHz) — no separate pixel clock
        .system_reset_n_i(system_reset_n_w),
        .device_reset_n_i(device_reset_n_w),
        .a2_phi1_i(a2_phi1),
        .a2_q3_i(a2_q3),
        .a2_7M_i(a2_7M),

        .sw_gs_i(!dip_switches_n[3]),

        .a2bus_if(a2bus_if),

        .a2_a_i(a2_a),
        .a2_d_i(a2_d_buf_w),
        .a2_rw_n_i(a2_rw_n),
        
        .a2_inh_n(a2_inh_n),
        .a2_rdy_n(a2_rdy_n),
        .a2_dma_n(a2_dma_n),
        .a2_nmi_n(a2_nmi_n),
        .a2_reset_n(a2_reset_n),
        .a2_mb20(a2_mb20),
        .a2_sync_n(a2_sync_n),
        .a2_m2sel_n(a2_m2sel_n),
        .a2_res_out_n(a2_res_out_n),
        .a2_int_out_n(a2_int_out_n),
        .a2_int_in_n(a2_int_in_n),
        .a2_dma_out_n(a2_dma_out_n),
        .a2_dma_in_n(a2_dma_in_n),
        .irq_n_i(1'b1),

        .sleep_o(sleep_w)
    );

    // LED indicators for phi1 and 2M clock
    
    wire led_phi1_w;
    reg [10:0]led_phi1_ctr_r;
    always @(posedge clk_logic_w) begin
        if (a2bus_if.phi1_posedge) led_phi1_ctr_r <= led_phi1_ctr_r + 1;
    end
    assign led_phi1_w = led_phi1_ctr_r[10];

    wire led_2m_w;
    reg [10:0]led_2m_ctr_r;
    always @(posedge clk_logic_w) begin
        if (a2bus_if.clk_q3_posedge) led_2m_ctr_r <= led_2m_ctr_r + 1;
    end
    assign led_2m_w = led_2m_ctr_r[10];


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

    // Scan timer outputs — forward-declared for apple_video_fb
    wire [8:0] scanline_w;
    wire hsync_w;
    wire vsync_w;
    wire [9:0] pixel_w;

    // Apple II video → DDR3 framebuffer renderer
    wire fb_we_w;
    wire [17:0] fb_data_w;
    wire fb_vsync_w;

    // Apple II RGB outputs for SuperSprite compositing
    wire [7:0] apple_fb_r_w;
    wire [7:0] apple_fb_g_w;
    wire [7:0] apple_fb_b_w;
    wire       apple_fb_active_w;

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

    // VGC framebuffer renderer — IIgs Super Hi-Res modes
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

    // Ensoniq DOC5503 Sound

    wire [15:0] sg_audio_l;
    wire [15:0] sg_audio_r;

    wire [7:0] sg_d_w;
    wire sg_rd_w;
    wire [7:0] doc_osc_en_w;
    wire [1:0] doc_osc_mode_w[8];
    wire [7:0] doc_osc_halt_w;

    // 64KB sound RAM backed by blockram via mem_port_if
    mem_port_if #(
        .PORT_ADDR_WIDTH(21),
        .DATA_WIDTH(32),
        .DQM_WIDTH(4),
        .PORT_OUTPUT_WIDTH(32)
    ) glu_mem_if();

    mem_port_if #(
        .PORT_ADDR_WIDTH(21),
        .DATA_WIDTH(32),
        .DQM_WIDTH(4),
        .PORT_OUTPUT_WIDTH(32)
    ) doc_mem_if();

    mem_port_bram #(
        .ADDR_WIDTH(14)  // 16K words x 4 bytes = 64KB
    ) sound_ram (
        .clk(clk_logic_w),
        .write_port(glu_mem_if),
        .read_port(doc_mem_if)
    );

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

        .glu_mem_if(glu_mem_if),
        .doc_mem_if(doc_mem_if)
    );

    // SuperSprite

    wire VDP_OVERLAY_SW;
    wire APPLE_VIDEO_SW;
    // =========================================================================
    // VDP Raster Counter — synced to Apple II scan_timer (clk_logic domain)
    // =========================================================================
    // vdp_cx: horizontal counter advancing once per 4 clk_logic cycles (pixel rate)
    // to match apple_video_fb's gap_cnt_r. Clamped at VDP_HMAX to prevent wrap.
    //
    // vdp_cy: uses scanline_w directly from scan_timer (0–261) to guarantee exact
    // alignment with apple_video_fb. This ensures the F18A sees the correct scanline
    // numbers (including 260/261 for scanline_reset during blanking) and that VDP
    // line 0 corresponds to framebuffer line 0.
    //
    // At 54 MHz with ~63.5 µs scanline: ~3,429 clk_logic cycles / 4 = ~857 ticks/line.
    // VDP_HMAX=856 ensures y_tick fires near end of each scanline.
    localparam VDP_HMAX = 10'd856;

    reg [9:0] vdp_cx;   // 0–856 (VDP_HMAX)
    reg [1:0] vdp_div;  // 2-bit divider: vdp_cx advances when vdp_div wraps

    reg hsync_prev_r;
    always @(posedge a2bus_if.clk_logic) begin
        hsync_prev_r <= hsync_w;
    end
    wire hsync_edge_w = hsync_w && !hsync_prev_r;

    always @(posedge a2bus_if.clk_logic) begin
        if (hsync_w) begin
            // Reset horizontal counter on hsync (same cycle as scan_timer scanline change)
            vdp_cx <= 10'd0;
            vdp_div <= 2'd0;
        end else begin
            vdp_div <= vdp_div + 2'd1;
            if (vdp_div == 2'd3 && vdp_cx < VDP_HMAX) begin
                vdp_cx <= vdp_cx + 10'd1;
            end
        end
    end

    // =========================================================================
    // SuperSprite / VDP
    // =========================================================================

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

    wire [7:0] rgb_r_w;
    wire [7:0] rgb_g_w;
    wire [7:0] rgb_b_w;

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

        .screen_x_i(vdp_cx),              // VDP raster X (10-bit, 0–856)
        .screen_y_i({1'b0, scanline_w}),  // VDP raster Y from scan_timer (9-bit→10-bit, 0–261)
        .apple_vga_r_i(apple_fb_r_w),     // Apple II RGB from apple_video_fb
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

    // =========================================================================
    // DDR3 Framebuffer + HDMI Output (480p)
    // =========================================================================
    //
    // The ddr3_framebuffer_480p module handles:
    //   - DDR3 controller + memory interface
    //   - HDMI encoder + TMDS output (480p 59.94Hz)
    //   - Audio CDC (2-stage sync from clk_logic to 27 MHz pixel clock)
    //   - 2× vertical integer scaling with scanline dimming
    //   - Centered display with border color fill
    //
    // Board PLL provides clk_pixel (27 MHz) and clk_pixel_x5 (135 MHz) directly.
    // No internal HDMI PLL needed — simpler than the 720p version.

    // Scan timer — authoritative Apple II scanline timing (kept for future use)
    // Wire declarations are forward-declared near apple_video_fb instantiation

    scan_timer scan_timer (
        .a2bus_if(a2bus_if),
        .scanline_o(scanline_w),
        .hsync_o(hsync_w),
        .vsync_o(vsync_w),
        .pixel_o(pixel_w)
    );

    // Framebuffer dynamic dimensions — switch at frame boundary
    localparam [10:0] APPLE_FB_WIDTH  = 11'd560;
    localparam [9:0]  APPLE_FB_HEIGHT = 10'd192;
    localparam [10:0] VGC_FB_WIDTH    = 11'd640;
    localparam [9:0]  VGC_FB_HEIGHT   = 10'd200;
    wire [10:0] fb_width_w  = use_vgc_r ? VGC_FB_WIDTH  : APPLE_FB_WIDTH;
    wire [9:0]  fb_height_w = use_vgc_r ? VGC_FB_HEIGHT : APPLE_FB_HEIGHT;

    // Border color: convert 4-bit palette index to RGB666
    // Uses {GSP, BORDER_COLOR} as 5-bit index into 32-entry palette,
    // same as apple_video_fb.sv: entries 0-15 = Apple II, 16-31 = IIgs
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
    // Expand RGB444 to RGB666: replicate top 2 bits into bottom 2 for accurate scaling
    wire [17:0] border_rgb666_w = {
        border_rgb444_w[11:8], 2'b00,  // R: 4->6 bits (zero-pad, matches apple_video_fb.sv)
        border_rgb444_w[7:4],  2'b00,  // G: 4->6 bits
        border_rgb444_w[3:0],  2'b00   // B: 4->6 bits
    };

    wire init_calib_complete_w;
    wire ddr_rst_w;
    wire clk_x1_w;              // 74.25 MHz from DDR3 controller (for DebugOverlay)
    wire [10:0] hdmi_cx_w;      // HDMI raster X (0–857)
    wire [9:0]  hdmi_cy_w;      // HDMI raster Y (0–524)
    wire [23:0] fb_rgb_w;       // Current framebuffer RGB output
    wire [23:0] overlay_rgb_w;  // DebugOverlay RGB output
    wire        overlay_en_w;   // DebugOverlay enable

    // DDR3 framebuffer — drives DDR3 and HDMI output (480p)
    wire [14:0] ddr_addr_fb;

    ddr3_framebuffer_480p #(
        .WIDTH(640),                   // max of 560 (Apple II) and 640 (SHR)
        .HEIGHT(480),                  // 480p output height
        .COLOR_BITS(18)
    ) u_ddr3_fb (
        // Clock inputs
        .clk_27(clk_pixel_w),          // 27 MHz from board PLL
        .clk_g(clk),                   // 50 MHz board crystal (same pin as PLL clkin)
        .pll_lock_27(clk_lock_w),
        .rst_n(1'b1),
        .clk_out(clk_x1_w),             // 74.25 MHz output for DebugOverlay
        .ddr_rst(ddr_rst_w),
        .init_calib_complete(init_calib_complete_w),

        // HDMI pixel clocks (from board PLL — no internal HDMI PLL needed)
        .clk_pixel(clk_pixel_w),       // 27 MHz pixel clock
        .clk_pixel_x5(clk_hdmi_w),     // 135 MHz TMDS 5x clock

        // Framebuffer write interface — on clk_logic (54 MHz), muxed between apple_video_fb and vgc_fb
        .clk(clk_logic_w),
        .fb_width(fb_width_w),
        .fb_height(fb_height_w),
        .fb_vsync(fb_vsync_mux_w),
        .fb_we(fb_we_mux_w),
        .fb_data(fb_data_mux_w),
        .border_color(border_rgb666_w),
        .sleep_i(sleep_w),

        // Audio — CDC'd internally via 2-stage sync
        .sound_left(core_audio_l_w),
        .sound_right(core_audio_r_w),

        // HDMI overlay interface (clk_pixel / 27 MHz domain)
        .hdmi_cx(hdmi_cx_w),
        .hdmi_cy(hdmi_cy_w),
        .fb_rgb_o(fb_rgb_w),
        .overlay_rgb_i(overlay_rgb_w),
        .overlay_en_i(overlay_en_w),

        // DDR3 pins
        .ddr_addr(ddr_addr_fb),
        .ddr_bank(ddr_bank),
        .ddr_cs(ddr_cs),
        .ddr_ras(ddr_ras),
        .ddr_cas(ddr_cas),
        .ddr_we(ddr_we),
        .ddr_ck(ddr_ck),
        .ddr_ck_n(ddr_ck_n),
        .ddr_cke(ddr_cke),
        .ddr_odt(ddr_odt),
        .ddr_reset_n(ddr_reset_n),
        .ddr_dm(ddr_dm),
        .ddr_dq(ddr_dq),
        .ddr_dqs(ddr_dqs),
        .ddr_dqs_n(ddr_dqs_n),

        // HDMI TMDS output
        .tmds_clk_p(tmds_clk_p),
        .tmds_clk_n(tmds_clk_n),
        .tmds_d_p(tmds_d_p),
        .tmds_d_n(tmds_d_n)
    );

    // DDR3 address: framebuffer uses 15 bits, top port has 16 bits
    assign ddr_addr[14:0] = ddr_addr_fb;
    assign ddr_addr[15] = 1'b0;

    // DDR3 calibration status on LED[1]
    assign led[1] = !init_calib_complete_w;

    // =========================================================================
    // Debug Overlay — runs in clk_pixel (27 MHz) domain
    // =========================================================================
    // The 480p framebuffer's overlay interface (hdmi_cx, hdmi_cy, fb_rgb_o,
    // overlay_rgb_i, overlay_en_i) is in the clk_pixel domain, so DebugOverlay
    // must also run in clk_pixel.

    // CDC for debug hex values: double-flop from clk_logic to clk_pixel
    // These are quasi-static values, so double-flop is sufficient
    reg [7:0] dbg_hex_sync0 [8], dbg_hex_sync1 [8];
    reg [7:0] dbg_bits0_sync0, dbg_bits0_sync1;
    reg [7:0] dbg_bits1_sync0, dbg_bits1_sync1;
    always @(posedge clk_pixel_w) begin
        dbg_hex_sync0[0] <= scanline_w[8:1];   dbg_hex_sync1[0] <= dbg_hex_sync0[0];
        dbg_hex_sync0[1] <= {7'b0, scanline_w[0]}; dbg_hex_sync1[1] <= dbg_hex_sync0[1];
        dbg_hex_sync0[2] <= 8'h00;              dbg_hex_sync1[2] <= dbg_hex_sync0[2];
        dbg_hex_sync0[3] <= 8'h00;              dbg_hex_sync1[3] <= dbg_hex_sync0[3];
        dbg_hex_sync0[4] <= 8'h00;              dbg_hex_sync1[4] <= dbg_hex_sync0[4];
        dbg_hex_sync0[5] <= 8'h00;              dbg_hex_sync1[5] <= dbg_hex_sync0[5];
        dbg_hex_sync0[6] <= 8'h00;              dbg_hex_sync1[6] <= dbg_hex_sync0[6];
        dbg_hex_sync0[7] <= {4'b0, a2mem_if.BACKGROUND_COLOR};
                                                dbg_hex_sync1[7] <= dbg_hex_sync0[7];
        dbg_bits0_sync0 <= {a2mem_if.SHRG_MODE, a2mem_if.TEXT_MODE, a2mem_if.MIXED_MODE,
                            a2mem_if.HIRES_MODE, a2mem_if.RAMWRT, a2mem_if.STORE80,
                            a2bus_if.system_reset_n, a2bus_if.device_reset_n};
        dbg_bits0_sync1 <= dbg_bits0_sync0;
        dbg_bits1_sync0 <= 8'h00;
        dbg_bits1_sync1 <= dbg_bits1_sync0;
    end

    DebugOverlay #(
        .VERSION(`BUILD_DATETIME),
        .ENABLE(1'b1),
        .X_OFFSET(16),
        .Y_OFFSET(24)
    ) debug_overlay (
        .clk_i          (clk_pixel_w),
        .reset_n        (device_reset_n_w),
        .enable_i       (1'b1),

        .hex_values     ('{dbg_hex_sync1[0], dbg_hex_sync1[1], dbg_hex_sync1[2], dbg_hex_sync1[3],
                           dbg_hex_sync1[4], dbg_hex_sync1[5], dbg_hex_sync1[6], dbg_hex_sync1[7]}),

        .debug_bits_0_i (dbg_bits0_sync1),
        .debug_bits_1_i (dbg_bits1_sync1),

        .screen_x_i     (hdmi_cx_w),
        .screen_y_i     (hdmi_cy_w),

        .r_i            (fb_rgb_w[23:16]),
        .g_i            (fb_rgb_w[15:8]),
        .b_i            (fb_rgb_w[7:0]),

        .r_o            (overlay_rgb_w[23:16]),
        .g_o            (overlay_rgb_w[15:8]),
        .b_o            (overlay_rgb_w[7:0])
    );

    assign overlay_en_w = 1'b1;

    // =========================================================================
    // ESP32 Octal SPI Interface
    // =========================================================================

    wire [7:0] esp_data_i;
    wire [7:0] esp_data_o;
    wire       esp_data_oe;

    // Bidirectional I/O buffers for Octal SPI data lines
    IOBUF esp_data_iobuf[7:0] (
        .O  (esp_data_i),       // Input from pads
        .IO (esp_data),         // Bidirectional pads
        .I  (esp_data_o),       // Output to pads
        .OEN(!esp_data_oe)      // Output enable (active low for IOBUF)
    );

    // Synchronize SCLK to logic clock domain
    wire esp_sclk_sync;
    cdc_denoise cdc_esp_sclk (
        .clk(clk_logic_w),
        .i(esp_sclk),
        .o(esp_sclk_sync),
        .o_n(),
        .o_posedge(),
        .o_negedge()
    );

    // ESP32 control interfaces
    slotmaker_config_if esp_slotmaker_config_if();
    f18a_gpu_if esp_f18a_gpu_if();
    video_control_if esp_video_control_if();
    drive_volume_if esp_volumes[2]();

    // Octal SPI connector instance
    esp32_ospi_connector #(
        .USE_SYNC(1),
        .USE_CRC(0),
        .IDLE_TO_CYC(5_400_000)  // ~100ms at 54MHz
    ) esp32_ospi (
        .clk(clk_logic_w),
        .rst_n(device_reset_n_w),
        .sclk(esp_sclk_sync),
        .data_i(esp_data_i),
        .data_o(esp_data_o),
        .data_oe(esp_data_oe),
        .slotmaker_config_if(esp_slotmaker_config_if),
        .f18a_gpu_if(esp_f18a_gpu_if),
        .video_control_if(esp_video_control_if),
        .volumes(esp_volumes)
    );

    // Note: These interfaces are currently independent of the main system.
    // Future integration:
    // - esp_slotmaker_config_if can be muxed with slotmaker_config_if
    // - esp_f18a_gpu_if can replace f18a_gpu_if when ESP32 controls VDP
    // - esp_video_control_if can replace video_control_if for OSD
    // - esp_volumes can be connected to disk drive infrastructure

    /*
    // Data bus IOBUF instantiation
    wire [7:0] cpu_data_in;
    wire [7:0] cpu_data_out;
    wire       cpu_data_oe;
    
    // Gowin IOBUF primitive - adjust to match your library
    genvar i;
    generate
        for (i = 0; i < 8; i = i + 1) begin : data_iobuf
            IOBUF data_buf (
                .O  (cpu_data_in[i]),
                .IO (DATA[i]),
                .I  (cpu_data_out[i]),
                .OEN(~cpu_data_oe)      // Gowin OEN is active low
            );
        end
    endgenerate
    */

endmodule

module reset_sync (
  input  wire clk,
  input  wire arst,   // async reset in, active-high
  output wire srst    // sync reset out, active-high
);
  reg [1:0] ff;

  always @(posedge clk or posedge arst) begin
    if (arst)
      ff <= 2'b11;          // assert immediately (async)
    else
      ff <= {ff[0], 1'b0};  // deassert cleanly (sync)
  end

  assign srst = ff[1];
endmodule