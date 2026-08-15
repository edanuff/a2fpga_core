// flash_rescue_top.sv — autonomous MSPI flash recovery/probe core.
//
// SRAM-loaded via JTAG with "MSPI": true in the process config, so the
// fabric owns the config-flash pads after configuration. Bypasses the
// GW5A config controller's JTAG-to-SPI passthrough entirely — no
// security latch, no auto-boot contention, no 10 MHz marginality.
//
// Autonomous flow (no inbound comms needed):
//   1. settle ~50 ms after config
//   2. if ERASE_OP: Write Enable (0x06) + Chip Erase (0xC7), then poll
//      SR1.BUSY; report progress
//   3. forever: read JEDEC (0x9F) + SR1/SR2/SR3 (0x05/0x35/0x15),
//      stream one ASCII line over the debug UART (H13 -> ESP32 telnet
//      tee) every ~500 ms:  "FR J:xxxxxx 1:xx 2:xx 3:xx E:c\r\n"
//      E: '-' = no erase requested, 'B' = erase busy, 'D' = erase done
//
// SPI: mode 0, ~1 MHz, single-IO. WP#/SIO2 and HOLD#/SIO3 driven high.
// Chip: Winbond W25Q64 (expect J:EF4017).

module flash_rescue_top #(
    parameter ERASE_OP = 1
) (
    input  logic clk50_in,
    output logic uart_tx,
    output logic [2:0] led,
    output logic flash_cs,
    output logic flash_clk,
    output logic flash_mosi,
    input  logic flash_miso,
    output logic flash_wp,
    output logic flash_hold
);
    assign flash_wp   = 1'b1;
    assign flash_hold = 1'b1;

    // heartbeat + status LEDs
    logic [25:0] hb = '0;
    always_ff @(posedge clk50_in) hb <= hb + 26'd1;
    logic erase_done = 1'b0;
    assign led = {hb[24], (ERASE_OP != 0), erase_done};

    // ------------------------------------------------------------------
    // SPI byte engine: mode 0, clk50/50 = 1 MHz
    // ------------------------------------------------------------------
    logic [7:0]  spi_tx, spi_rx;
    logic        spi_go = 1'b0, spi_busy = 1'b0;
    logic [4:0]  spi_div = '0;
    logic [3:0]  spi_bit = '0;
    logic        sclk_r  = 1'b0;
    logic [7:0]  sh_tx, sh_rx;
    assign flash_clk  = sclk_r;
    assign flash_mosi = sh_tx[7];

    always_ff @(posedge clk50_in) begin
        if (!spi_busy) begin
            if (spi_go) begin
                spi_busy <= 1'b1;
                sh_tx    <= spi_tx;
                spi_bit  <= 4'd0;
                spi_div  <= 5'd0;
                sclk_r   <= 1'b0;
            end
        end else begin
            spi_div <= spi_div + 5'd1;
            if (spi_div == 5'd24) begin        // half-period @1 MHz
                spi_div <= 5'd0;
                if (!sclk_r) begin
                    sclk_r <= 1'b1;            // rising edge: sample MISO
                    sh_rx  <= {sh_rx[6:0], flash_miso};
                end else begin
                    sclk_r <= 1'b0;            // falling edge: shift MOSI
                    if (spi_bit == 4'd7) begin
                        spi_busy <= 1'b0;
                        spi_rx   <= sh_rx;
                    end else begin
                        spi_bit <= spi_bit + 4'd1;
                        sh_tx   <= {sh_tx[6:0], 1'b0};
                    end
                end
            end
        end
    end

    // ------------------------------------------------------------------
    // UART TX: 115200 8N1 @ clk50 (434 cycles/bit)
    // ------------------------------------------------------------------
    localparam int MSG_LEN = 32;
    logic [7:0]  msg [0:MSG_LEN-1];
    logic [5:0]  u_idx = MSG_LEN[5:0];         // idle when == MSG_LEN
    logic [8:0]  u_div = '0;
    logic [3:0]  u_bit = '0;
    logic [9:0]  u_sh  = 10'h3FF;
    logic        u_start = 1'b0;
    assign uart_tx = u_sh[0];

    always_ff @(posedge clk50_in) begin
        if (u_idx == MSG_LEN[5:0]) begin
            u_sh <= 10'h3FF;
            if (u_start) begin
                u_idx <= 6'd0;
                u_bit <= 4'd0;
                u_div <= 9'd0;
                u_sh  <= {1'b1, msg[0], 1'b0};
            end
        end else begin
            u_div <= u_div + 9'd1;
            if (u_div == 9'd433) begin
                u_div <= 9'd0;
                if (u_bit == 4'd9) begin
                    u_bit <= 4'd0;
                    if (u_idx == MSG_LEN[5:0] - 6'd1)
                        u_idx <= MSG_LEN[5:0];
                    else begin
                        u_idx <= u_idx + 6'd1;
                        u_sh  <= {1'b1, msg[u_idx + 6'd1], 1'b0};
                    end
                end else begin
                    u_bit <= u_bit + 4'd1;
                    u_sh  <= {1'b1, u_sh[9:1]};
                end
            end
        end
    end

    function automatic [7:0] hexch(input [3:0] n);
        hexch = (n < 4'd10) ? (8'h30 + 8'(n)) : (8'h37 + 8'(n));
    endfunction

    // ------------------------------------------------------------------
    // Command sequencer
    // ------------------------------------------------------------------
    typedef enum logic [4:0] {
        S_SETTLE, S_WE_CS, S_WE, S_WE_END,
        S_ER_CS, S_ER, S_ER_END,
        S_POLL_CS, S_POLL_CMD, S_POLL_RD, S_POLL_END,
        S_J_CS, S_J_CMD, S_J_B0, S_J_B1, S_J_B2, S_J_END,
        S_SR_CS, S_SR_CMD, S_SR_RD, S_SR_END,
        S_FMT, S_WAIT
    } st_t;
    st_t st = S_SETTLE;

    logic [23:0] jedec = '0;
    logic [7:0]  sr [0:2];
    logic [1:0]  sr_idx = 2'd0;
    logic [7:0]  sr_cmd;
    logic [25:0] t = '0;
    logic        cs_r = 1'b1;
    logic        erase_busy = 1'b0;
    logic [7:0]  echar;
    assign flash_cs = cs_r;
    assign echar = (ERASE_OP == 0) ? 8'h2D : (erase_busy ? 8'h42 : (erase_done ? 8'h44 : 8'h2E));

    always_comb begin
        case (sr_idx)
            2'd0: sr_cmd = 8'h05;   // SR1
            2'd1: sr_cmd = 8'h35;   // SR2
            default: sr_cmd = 8'h15; // SR3
        endcase
    end

    // gap timer between CS edges (>= 1 us)
    logic [6:0] gap = '0;

    always_ff @(posedge clk50_in) begin
        spi_go  <= 1'b0;
        u_start <= 1'b0;
        if (gap != 7'd0) gap <= gap - 7'd1;
        else case (st)
            S_SETTLE: begin
                t <= t + 26'd1;
                if (t[21]) begin                 // ~42 ms
                    t  <= '0;
                    st <= (ERASE_OP != 0) ? S_WE_CS : S_J_CS;
                end
            end
            // ---- write enable ----
            S_WE_CS:  begin cs_r <= 1'b0; gap <= 7'd100; st <= S_WE; end
            S_WE:     if (!spi_busy && !spi_go) begin spi_tx <= 8'h06; spi_go <= 1'b1; st <= S_WE_END; end
            S_WE_END: if (!spi_busy && !spi_go) begin cs_r <= 1'b1; gap <= 7'd100; st <= S_ER_CS; end
            // ---- chip erase ----
            S_ER_CS:  begin cs_r <= 1'b0; gap <= 7'd100; st <= S_ER; end
            S_ER:     if (!spi_busy && !spi_go) begin spi_tx <= 8'hC7; spi_go <= 1'b1; st <= S_ER_END; end
            S_ER_END: if (!spi_busy && !spi_go) begin cs_r <= 1'b1; gap <= 7'd100; erase_busy <= 1'b1; st <= S_J_CS; end
            // ---- JEDEC ----
            S_J_CS:  begin cs_r <= 1'b0; gap <= 7'd100; st <= S_J_CMD; end
            S_J_CMD: if (!spi_busy && !spi_go) begin spi_tx <= 8'h9F; spi_go <= 1'b1; st <= S_J_B0; end
            S_J_B0:  if (!spi_busy && !spi_go) begin spi_tx <= 8'h00; spi_go <= 1'b1; st <= S_J_B1; end
            S_J_B1:  if (!spi_busy && !spi_go) begin jedec[23:16] <= spi_rx; spi_tx <= 8'h00; spi_go <= 1'b1; st <= S_J_B2; end
            S_J_B2:  if (!spi_busy && !spi_go) begin jedec[15:8]  <= spi_rx; spi_tx <= 8'h00; spi_go <= 1'b1; st <= S_J_END; end
            S_J_END: if (!spi_busy && !spi_go) begin jedec[7:0]   <= spi_rx; cs_r <= 1'b1; gap <= 7'd100; sr_idx <= 2'd0; st <= S_SR_CS; end
            // ---- status registers (x3) ----
            S_SR_CS:  begin cs_r <= 1'b0; gap <= 7'd100; st <= S_SR_CMD; end
            S_SR_CMD: if (!spi_busy && !spi_go) begin spi_tx <= sr_cmd; spi_go <= 1'b1; st <= S_SR_RD; end
            S_SR_RD:  if (!spi_busy && !spi_go) begin spi_tx <= 8'h00; spi_go <= 1'b1; st <= S_SR_END; end
            S_SR_END: if (!spi_busy && !spi_go) begin
                sr[sr_idx] <= spi_rx;
                cs_r <= 1'b1; gap <= 7'd100;
                if (sr_idx == 2'd2) st <= S_FMT;
                else begin sr_idx <= sr_idx + 2'd1; st <= S_SR_CS; end
            end
            // ---- format + send ----
            S_FMT: begin
                if (erase_busy && !sr[0][0]) begin erase_busy <= 1'b0; erase_done <= 1'b1; end
                msg[0]<="F"; msg[1]<="R"; msg[2]<=" "; msg[3]<="J"; msg[4]<=":";
                msg[5]<=hexch(jedec[23:20]); msg[6]<=hexch(jedec[19:16]);
                msg[7]<=hexch(jedec[15:12]); msg[8]<=hexch(jedec[11:8]);
                msg[9]<=hexch(jedec[7:4]);   msg[10]<=hexch(jedec[3:0]);
                msg[11]<=" "; msg[12]<="1"; msg[13]<=":";
                msg[14]<=hexch(sr[0][7:4]); msg[15]<=hexch(sr[0][3:0]);
                msg[16]<=" "; msg[17]<="2"; msg[18]<=":";
                msg[19]<=hexch(sr[1][7:4]); msg[20]<=hexch(sr[1][3:0]);
                msg[21]<=" "; msg[22]<="3"; msg[23]<=":";
                msg[24]<=hexch(sr[2][7:4]); msg[25]<=hexch(sr[2][3:0]);
                msg[26]<=" "; msg[27]<="E"; msg[28]<=":";
                msg[29]<=echar; msg[30]<=8'h0D; msg[31]<=8'h0A;
                u_start <= 1'b1;
                t  <= '0;
                st <= S_WAIT;
            end
            S_WAIT: begin
                t <= t + 26'd1;
                if (t[24]) begin t <= '0; st <= S_J_CS; end   // ~335 ms
            end
            default: st <= S_SETTLE;
        endcase
    end
endmodule
