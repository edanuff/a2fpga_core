///////////////////////////////////////////////////////////////////////////////
// a2p25_psram_test_top.sv — a2-p25 (displayport1 rev) standalone HyperRAM
// bring-up / error-rate bitstream
//
// hyperram_ctrl + hyperram_bist on the S27KL0642, driven from the ESP32
// over a plain UART (hr_uart_cmd) so the test does not depend on the OPI
// link. Nothing Apple-bus or DP related runs; the Apple bus transceivers
// are held disabled.
//
// Built in three HyperBus clock variants (one .gprj each, selected by the
// defines file): CK 108 / 81 / 54 MHz, logic clock = CK / 2.
//
// Pins (bring-up repurposing of the OPI lines, this bitstream only):
//   uart_tx -> C2  ESP32_OPI_D0 -> ESP32 IO11 (firmware dp_test-mode RX)
//   uart_rx <- F2  ESP32_OPI_D1 <- ESP32 IO12 (firmware dp_test-mode TX)
// The firmware's serial forwarding mode makes this UART the USB console.
//
// Autostart: once calibration finishes, the BIST is started in continuous
// PRBS mode with random burst lengths and random byte masks over the whole
// device except the controller's calibration block. A periodic status line
// reports progress (see hr_uart_cmd). Any command written over the UART can
// STOP / reconfigure / restart it (register map: hyperram_bist.sv header).
//
// LED (B10, active high): heartbeat that toggles in every state (a2mega
// led[0] parity), clocked from the cascaded HyperRAM PLL's logic clock, so
// blinking also proves both PLLs locked. A dark or solid LED = not running.
//   ~1.5 Hz   calibrating, or test running with no errors
//   ~6 Hz     errors seen (BIST STATUS bit4) or calibration failed
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

`ifdef PSRAM_TEST_CK81
 `define PT_CK_MHZ 81
 `define PT_IDIV   1
 `define PT_MDIV   15
 `define PT_OD_LOG 20
 `define PT_OD_MEM 10
 `define PT_PE_C   2
 `define PT_PE_F   4
`elsif PSRAM_TEST_CK54
 `define PT_CK_MHZ 54
 `define PT_IDIV   1
 `define PT_MDIV   20
 `define PT_OD_LOG 40
 `define PT_OD_MEM 20
 `define PT_PE_C   5
 `define PT_PE_F   0
`else
 `define PT_CK_MHZ 108
 `define PT_IDIV   1
 `define PT_MDIV   20
 `define PT_OD_LOG 20
 `define PT_OD_MEM 10
 `define PT_PE_C   2
 `define PT_PE_F   4
`endif

module a2p25_psram_test_top #(
    parameter int CK_MHZ    = `PT_CK_MHZ,
    parameter int AUTOSTART = 1,
    parameter int AUTO_LEN  = 2**21 - 16,       // words (top 16 = calibration block)
    parameter int UART_BAUD = 115_200,
    parameter int STATUS_CLKS = CK_MHZ * 1_000_000 / 2   // 1 s
) (
    input  logic clk50_in,          // E2

    output logic hr_ck,             // J11
    output logic hr_cs_n,           // C3
    inout  wire  hr_rwds,           // E3
    inout  wire  [7:0] hr_dq,       // L4 L3 J1 J2 G4 H4 H1 H2

    input  logic uart_rx,           // F2 (ESP32_OPI_D1, repurposed)
    output logic uart_tx,           // C2 (ESP32_OPI_D0, repurposed)
    output logic led,               // B10
    output logic a2_bus_oe_n        // H5: hold the Apple bus transceivers off
);

    localparam int CLK_HZ = CK_MHZ * 1_000_000 / 2;
    localparam int AW     = 21;

    assign a2_bus_oe_n = 1'b1;

    // ------------------------------------------------------------------
    // Clocks + reset: 50 MHz -> board PLL (54 MHz) -> hr_clk_pll. The
    // cascade is required: PLLA's PFD range (19-87.5 MHz) leaves no 50 MHz
    // divider set that reaches a 108 MHz multiple. All logic runs on the
    // second PLL's outputs, which are mutually phase-related.
    // ------------------------------------------------------------------
    logic clk54, pll0_lock, pll1_lock;
    Gowin_PLL i_pll0 (
        .clkin   (clk50_in),
        .mdclk   (clk50_in),
        .clkout0 (),
        .clkout1 (),
        .clkout2 (clk54),
        .lock    (pll0_lock)
    );

    logic clk, clk_mem, clk_mem_90, pll_lock;
    assign pll_lock = pll0_lock & pll1_lock;
    hr_clk_pll #(
        .IDIV        (`PT_IDIV),
        .MDIV        (`PT_MDIV),
        .ODIV_LOGIC  (`PT_OD_LOG),
        .ODIV_MEM    (`PT_OD_MEM),
        .PE90_COARSE (`PT_PE_C),
        .PE90_FINE   (`PT_PE_F)
    ) i_pll (
        .lock       (pll1_lock),
        .clk        (clk),
        .clk_mem    (clk_mem),
        .clk_mem_90 (clk_mem_90),
        .clkin      (clk54)
    );

    logic [15:0] por_cnt = '0;
    logic        rst_n = 1'b0;
    always_ff @(posedge clk) begin
        if (!pll_lock) begin
            por_cnt <= '0;
            rst_n   <= 1'b0;
        end else if (por_cnt != 16'hFFFF) begin
            por_cnt <= por_cnt + 16'd1;
        end else begin
            rst_n <= 1'b1;
        end
    end

    // ------------------------------------------------------------------
    // Controller + BIST
    // ------------------------------------------------------------------
    logic            m_ready, m_init_done, m_req, m_we;
    logic [AW-1:0]   m_addr;
    logic [4:0]      m_len;
    logic [31:0]     m_wdata;
    logic [3:0]      m_wbe;
    logic            m_wbuf_we;
    logic [3:0]      m_wbuf_idx;
    logic [31:0]     m_wbuf_data;
    logic [3:0]      m_wbuf_be;
    logic            m_rvalid, m_done, m_rd_err, m_xfer_lat2x;
    logic [31:0]     m_rdata;
    logic            m_recal, m_cfg_cr0_write, m_dly_force;
    logic [15:0]     m_cfg_cr0, m_cr0_active;
    logic [7:0]      m_dly_force_code, m_dly_code, m_cal_tap;
    logic [31:0]     m_cal_map;
    logic            m_cal_ok;
    logic [4:0]      m_rd_arrival;
    logic [31:0]     m_stat_xfers, m_stat_lat2x, m_stat_rd_timeouts, m_stat_lat_mismatch;

    hyperram_ctrl #(
        .CLK_HZ     (CLK_HZ),
        .ADDR_WIDTH (AW)
    ) i_ctrl (
        .clk              (clk),
        .clk_mem          (clk_mem),
        .clk_mem_90       (clk_mem_90),
        .clk_cap          (clk_mem),
        .rst_n            (rst_n),
        .ready            (m_ready),
        .init_done        (m_init_done),
        .req              (m_req),
        .we               (m_we),
        .addr             (m_addr),
        .len              (m_len),
        .wdata            (m_wdata),
        .wbe              (m_wbe),
        .wbuf_we          (m_wbuf_we),
        .wbuf_idx         (m_wbuf_idx),
        .wbuf_data        (m_wbuf_data),
        .wbuf_be          (m_wbuf_be),
        .rvalid           (m_rvalid),
        .rdata            (m_rdata),
        .done             (m_done),
        .rd_err           (m_rd_err),
        .xfer_lat2x       (m_xfer_lat2x),
        .recal            (m_recal),
        .cfg_cr0          (m_cfg_cr0),
        .cfg_cr0_write    (m_cfg_cr0_write),
        .cr0_active       (m_cr0_active),
        .dly_force        (m_dly_force),
        .dly_force_code   (m_dly_force_code),
        .dly_code         (m_dly_code),
        .cal_tap          (m_cal_tap),
        .cal_map          (m_cal_map),
        .cal_ok           (m_cal_ok),
        .rd_arrival       (m_rd_arrival),
        .stat_xfers       (m_stat_xfers),
        .stat_lat2x       (m_stat_lat2x),
        .stat_rd_timeouts (m_stat_rd_timeouts),
        .stat_lat_mismatch(m_stat_lat_mismatch),
        .hr_ck            (hr_ck),
        .hr_cs_n          (hr_cs_n),
        .hr_rwds          (hr_rwds),
        .hr_dq            (hr_dq)
    );

    logic [7:0]  reg_addr, c_addr, a_addr;
    logic [31:0] reg_wdata, c_wdata, a_wdata, reg_rdata;
    logic        reg_we, c_we, a_we;
    logic        auto_active;

    assign reg_addr  = auto_active ? a_addr  : c_addr;
    assign reg_wdata = auto_active ? a_wdata : c_wdata;
    assign reg_we    = auto_active ? a_we    : c_we;

    hyperram_bist #(.ADDR_WIDTH(AW)) i_bist (
        .clk                 (clk),
        .rst_n               (rst_n),
        .reg_addr            (reg_addr),
        .reg_wdata           (reg_wdata),
        .reg_we              (reg_we),
        .reg_rdata           (reg_rdata),
        .active              (),
        // no other client in this bitstream
        .u_ready             (),
        .u_req               (1'b0),
        .u_we                (1'b0),
        .u_addr              ('0),
        .u_len               (5'd0),
        .u_wdata             (32'd0),
        .u_wbe               (4'd0),
        .u_wbuf_we           (1'b0),
        .u_wbuf_idx          (4'd0),
        .u_wbuf_data         (32'd0),
        .u_wbuf_be           (4'd0),
        .u_rvalid            (),
        .u_rdata             (),
        .u_done              (),
        .u_rd_err            (),
        .m_ready             (m_ready),
        .m_init_done         (m_init_done),
        .m_req               (m_req),
        .m_we                (m_we),
        .m_addr              (m_addr),
        .m_len               (m_len),
        .m_wdata             (m_wdata),
        .m_wbe               (m_wbe),
        .m_wbuf_we           (m_wbuf_we),
        .m_wbuf_idx          (m_wbuf_idx),
        .m_wbuf_data         (m_wbuf_data),
        .m_wbuf_be           (m_wbuf_be),
        .m_rvalid            (m_rvalid),
        .m_rdata             (m_rdata),
        .m_done              (m_done),
        .m_rd_err            (m_rd_err),
        .m_xfer_lat2x        (m_xfer_lat2x),
        .m_recal             (m_recal),
        .m_cfg_cr0           (m_cfg_cr0),
        .m_cfg_cr0_write     (m_cfg_cr0_write),
        .m_dly_force         (m_dly_force),
        .m_dly_force_code    (m_dly_force_code),
        .m_cr0_active        (m_cr0_active),
        .m_dly_code          (m_dly_code),
        .m_cal_tap           (m_cal_tap),
        .m_cal_map           (m_cal_map),
        .m_cal_ok            (m_cal_ok),
        .m_rd_arrival        (m_rd_arrival),
        .m_stat_xfers        (m_stat_xfers),
        .m_stat_lat2x        (m_stat_lat2x),
        .m_stat_rd_timeouts  (m_stat_rd_timeouts),
        .m_stat_lat_mismatch (m_stat_lat_mismatch)
    );

    // ------------------------------------------------------------------
    // Autostart: continuous PRBS / random length / random mask over
    // [0, 2M - 16) (the top 16 words are the calibration block)
    // ------------------------------------------------------------------
    localparam logic [31:0] AUTO_MODE = 32'h0000_00E1;   // PRBS, RUN=2 continuous, rand len + rand mask
    logic [2:0] auto_step = 3'd0;
    always_ff @(posedge clk) begin
        a_we <= 1'b0;
        if (!rst_n) begin
            auto_step   <= 3'd0;
            auto_active <= (AUTOSTART != 0);
        end else if (auto_active && m_init_done) begin
            auto_step <= auto_step + 3'd1;
            a_we      <= 1'b1;
            case (auto_step)
                3'd0: begin a_addr <= 8'h03; a_wdata <= AUTO_MODE; end
                3'd1: begin a_addr <= 8'h04; a_wdata <= 32'd0; end
                3'd2: begin a_addr <= 8'h05; a_wdata <= 32'(AUTO_LEN); end
                3'd3: begin a_addr <= 8'h07; a_wdata <= 32'h1234_5678; end
                3'd4: begin a_addr <= 8'h01; a_wdata <= 32'h0000_0005; end  // CLEAR + START
                default: begin a_we <= 1'b0; auto_active <= 1'b0; end
            endcase
        end
    end

    // ------------------------------------------------------------------
    // UART console
    // ------------------------------------------------------------------
    hr_uart_cmd #(
        .CLK_HZ (CLK_HZ),
        .BAUD   (UART_BAUD),
        .CK_MHZ (CK_MHZ),
        .STATUS_CLKS (STATUS_CLKS)
    ) i_cmd (
        .clk       (clk),
        .rst_n     (rst_n),
        .uart_rx   (uart_rx),
        .uart_tx   (uart_tx),
        .reg_addr  (c_addr),
        .reg_wdata (c_wdata),
        .reg_we    (c_we),
        .reg_rdata (reg_rdata),
        .cmd_seen  ()
    );

    // ------------------------------------------------------------------
    // LED
    // ------------------------------------------------------------------
    localparam int HB_SLOW = CLK_HZ / 3;     // toggle period -> 1.5 Hz
    localparam int HB_FAST = CLK_HZ / 12;    // -> 6 Hz
    logic [31:0] hb = '0;
    logic        errs_seen = 1'b0;
    initial led = 1'b0;
    wire         alarm = errs_seen || (m_init_done && !m_cal_ok);
    always_ff @(posedge clk) begin
        if (hb >= 32'(alarm ? HB_FAST : HB_SLOW) - 1) begin
            hb  <= '0;
            led <= ~led;
        end else begin
            hb <= hb + 32'd1;
        end
    end

    // Error flag for the LED: BIST STATUS bit4 (errors seen), captured
    // whenever STATUS is read — the console's periodic status line reads it
    // every second. reg_rdata is valid one clock after reg_addr.
    logic [7:0] addr_d;
    always_ff @(posedge clk) begin
        addr_d <= reg_addr;
        if (!rst_n)
            errs_seen <= 1'b0;
        else if (addr_d == 8'h02 && reg_addr == 8'h02)
            errs_seen <= reg_rdata[4];
    end

endmodule
