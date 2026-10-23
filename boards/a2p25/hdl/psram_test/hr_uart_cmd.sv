//
// hr_uart_cmd — text register console for the a2p25 PSRAM bring-up bitstream
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
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
// Description:
//
// 8N1 UART console onto a 32-bit register port (hyperram_bist's map plus
// the local registers below). Deliberately independent of the ESP32 OPI
// link, which is itself unproven on the new board. The ESP32 firmware's
// dp_test mode passes the USB console straight through to this UART.
//
// Commands (one per line, CR or LF terminated, hex, case-insensitive,
// spaces optional between fields):
//   R aa              -> "aa=dddddddd"
//   W aa dddddddd     -> "OK"
//   anything else     -> "?"
// Periodic status (local reg 0xF2 bit0, default on), every ~1 s while no
// command is being answered:
//   "S 02=........ 12=........ 13=........ 1B=........ 21=........"
//   (STATUS, ERR_WORDS, ERR_BITS, PASS_COUNT, CAL_INFO)
//
// Local registers (not forwarded):
//   0xF0 BUILD   RO  [15:0] HyperBus CK in MHz, [31:16] 0x5053 ("PS")
//   0xF1 UPTIME  RO  seconds since configuration
//   0xF2 CONSOLE RW  bit0 periodic status enable
//   0xF3 SCRATCH RW  link test
//
// Register read timing: reg_rdata is sampled 3 clocks after reg_addr is
// presented (hyperram_bist: 1; EYE_DATA: 2 after EYE_IDX is written).
//

module hr_uart_cmd #(
    parameter CLK_HZ  = 54_000_000,
    parameter BAUD    = 115_200,
    parameter CK_MHZ  = 108,
    parameter STATUS_CLKS = CLK_HZ      // periodic status interval (1 s)
) (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        uart_rx,
    output reg         uart_tx,

    output reg  [7:0]  reg_addr,
    output reg  [31:0] reg_wdata,
    output reg         reg_we,
    input  wire [31:0] reg_rdata,

    output wire        cmd_seen          // pulses on every accepted command
);

    localparam CPB = CLK_HZ / BAUD;

    // ------------------------------------------------------------------
    // UART RX (2FF sync, mid-bit sampling)
    // ------------------------------------------------------------------
    reg [2:0]  rx_s = 3'b111;
    reg [15:0] rx_cnt = 16'd0;
    reg [3:0]  rx_bit = 4'd0;
    reg [7:0]  rx_sh = 8'd0;
    reg        rx_busy = 1'b0;
    reg        rx_valid = 1'b0;
    reg [7:0]  rx_byte = 8'd0;

    always @(posedge clk) begin
        rx_s     <= {rx_s[1:0], uart_rx};
        rx_valid <= 1'b0;
        if (!rx_busy) begin
            if (!rx_s[2]) begin                 // start bit edge
                rx_busy <= 1'b1;
                rx_cnt  <= 16'(CPB / 2);
                rx_bit  <= 4'd0;
            end
        end else if (rx_cnt != 16'd0) begin
            rx_cnt <= rx_cnt - 16'd1;
        end else begin
            rx_cnt <= 16'(CPB - 1);
            if (rx_bit == 4'd0) begin
                if (rx_s[2]) rx_busy <= 1'b0;   // false start
                rx_bit <= 4'd1;
            end else if (rx_bit <= 4'd8) begin
                rx_sh  <= {rx_s[2], rx_sh[7:1]};
                rx_bit <= rx_bit + 4'd1;
            end else begin
                rx_busy <= 1'b0;
                if (rx_s[2]) begin              // valid stop bit
                    rx_valid <= 1'b1;
                    rx_byte  <= rx_sh;
                end
            end
        end
        if (!rst_n) begin rx_busy <= 1'b0; end
    end

    // ------------------------------------------------------------------
    // UART TX: single byte holding register
    // ------------------------------------------------------------------
    reg  [7:0]  tx_byte;
    reg         tx_load = 1'b0;
    reg  [15:0] tx_cnt = 16'd0;
    reg  [3:0]  tx_bit = 4'd0;
    reg  [9:0]  tx_sh = 10'h3FF;
    wire        tx_idle = (tx_bit == 4'd0) && !tx_load;

    always @(posedge clk) begin
        if (tx_load) begin
            tx_sh   <= {1'b1, tx_byte, 1'b0};
            tx_bit  <= 4'd10;
            tx_cnt  <= 16'(CPB - 1);
            tx_load <= 1'b0;
        end else if (tx_bit != 4'd0) begin
            if (tx_cnt != 16'd0) begin
                tx_cnt <= tx_cnt - 16'd1;
            end else begin
                tx_cnt <= 16'(CPB - 1);
                tx_sh  <= {1'b1, tx_sh[9:1]};
                tx_bit <= tx_bit - 4'd1;
            end
        end
        uart_tx <= (tx_bit != 4'd0) ? tx_sh[0] : 1'b1;
        if (!rst_n) begin tx_bit <= 4'd0; uart_tx <= 1'b1; end
        if (send_go) begin tx_byte <= send_ch; tx_load <= 1'b1; end
    end

    // ------------------------------------------------------------------
    // Local registers
    // ------------------------------------------------------------------
    reg [31:0] uptime = 32'd0;
    reg [31:0] sec_cnt = 32'd0;
    reg [31:0] stat_cnt = 32'd0;
    reg        tick_1s = 1'b0;
    reg        status_en = 1'b1;
    reg [31:0] scratch = 32'd0;
    always @(posedge clk) begin
        tick_1s <= 1'b0;
        if (sec_cnt == CLK_HZ - 1) begin
            sec_cnt <= 32'd0;
            uptime  <= uptime + 32'd1;
        end else begin
            sec_cnt <= sec_cnt + 32'd1;
        end
        if (stat_cnt == STATUS_CLKS - 1) begin
            stat_cnt <= 32'd0;
            tick_1s  <= 1'b1;
        end else begin
            stat_cnt <= stat_cnt + 32'd1;
        end
    end

    // ------------------------------------------------------------------
    // Line buffer
    // ------------------------------------------------------------------
    reg [7:0] line [0:31];
    reg [5:0] line_len = 6'd0;
    reg       line_ready = 1'b0;

    function automatic [4:0] hexval(input [7:0] c);   // [4] = valid
        if (c >= "0" && c <= "9")      hexval = {1'b1, 4'(c - "0")};
        else if (c >= "a" && c <= "f") hexval = {1'b1, 4'(c - "a" + 10)};
        else if (c >= "A" && c <= "F") hexval = {1'b1, 4'(c - "A" + 10)};
        else                           hexval = 5'd0;
    endfunction

    function automatic [7:0] hexch(input [3:0] n);
        hexch = (n < 4'd10) ? (8'h30 + 8'(n)) : (8'h37 + 8'(n));
    endfunction

    // ------------------------------------------------------------------
    // Command / status engine
    // ------------------------------------------------------------------
    localparam E_IDLE   = 4'd0;
    localparam E_PARSE  = 4'd1;
    localparam E_FIELD  = 4'd2;
    localparam E_EXEC   = 4'd3;
    localparam E_RWAIT  = 4'd4;
    localparam E_EMIT   = 4'd5;
    localparam E_STAT   = 4'd6;

    reg [3:0]  st = E_IDLE;
    reg [5:0]  pi;                // parse index
    reg [7:0]  op;                // 'R' / 'W'
    reg [1:0]  field;             // 0 = addr, 1 = data
    reg [31:0] acc;
    reg [3:0]  ndig;
    reg [7:0]  a_addr;
    reg [31:0] a_data;
    reg [1:0]  wait_cnt;

    // Output string: up to 64 chars, assembled then streamed
    reg [7:0]  out [0:63];
    reg [6:0]  out_len, out_idx;
    reg        out_busy = 1'b0;
    reg        send_go;
    reg [7:0]  send_ch;

    // Status sequence
    reg [2:0]  stat_i;
    reg        stat_pending = 1'b0;
    function automatic [7:0] stat_addr(input [2:0] i);
        case (i)
            3'd0: stat_addr = 8'h02;
            3'd1: stat_addr = 8'h12;
            3'd2: stat_addr = 8'h13;
            3'd3: stat_addr = 8'h1B;
            default: stat_addr = 8'h21;
        endcase
    endfunction

    reg cmd_seen_r = 1'b0;
    assign cmd_seen = cmd_seen_r;

    // read value for a given address (local regs override)
    function automatic [31:0] rd_val(input [7:0] a, input [31:0] fwd);
        case (a)
            8'hF0: rd_val = {16'h5053, 16'(CK_MHZ)};
            8'hF1: rd_val = uptime;
            8'hF2: rd_val = {31'd0, status_en};
            8'hF3: rd_val = scratch;
            default: rd_val = fwd;
        endcase
    endfunction

    wire [4:0]  hv  = hexval(line[pi[4:0]]);
    wire [31:0] rv  = rd_val(a_addr, reg_rdata);
    wire [7:0]  sa  = stat_addr(stat_i);

    integer k;
    always @(posedge clk) begin
        reg_we     <= 1'b0;
        send_go    <= 1'b0;
        cmd_seen_r <= 1'b0;
        if (tick_1s && status_en) stat_pending <= 1'b1;

        // collect characters (dropped while a reply is being produced)
        if (rx_valid && !line_ready) begin
            if (rx_byte == 8'h0D || rx_byte == 8'h0A) begin
                if (line_len != 6'd0) line_ready <= 1'b1;
            end else if (line_len != 6'd32) begin
                line[line_len[4:0]] <= rx_byte;
                line_len <= line_len + 6'd1;
            end
        end

        // stream the output buffer
        if (out_busy && tx_idle && !send_go) begin
            if (out_idx == out_len) begin
                out_busy <= 1'b0;
            end else begin
                send_ch <= out[out_idx[5:0]];
                send_go <= 1'b1;
                out_idx <= out_idx + 7'd1;
            end
        end

        case (st)
        E_IDLE:
            if (!out_busy) begin
                if (line_ready) begin
                    pi    <= 6'd1;
                    op    <= line[0] & 8'hDF;     // upper-case
                    field <= 2'd0;
                    acc   <= 32'd0;
                    ndig  <= 4'd0;
                    st    <= E_PARSE;
                end else if (stat_pending) begin
                    stat_pending <= 1'b0;
                    stat_i  <= 3'd0;
                    out[0]  <= "S";
                    out_len <= 7'd1;
                    reg_addr <= stat_addr(3'd0);
                    wait_cnt <= 2'd3;
                    st      <= E_STAT;
                end
            end

        E_PARSE: begin
            // walk the line: fields are hex runs separated by spaces
            if (pi == line_len) begin
                st <= E_FIELD;                     // end of line closes field
            end else begin
                if (hv[4]) begin
                    acc  <= {acc[27:0], hv[3:0]};
                    ndig <= ndig + 4'd1;
                end else if (ndig != 4'd0) begin
                    if (field == 2'd0) a_addr <= acc[7:0];
                    else               a_data <= acc;
                    field <= field + 2'd1;
                    acc   <= 32'd0;
                    ndig  <= 4'd0;
                end
                pi <= pi + 6'd1;
            end
        end

        E_FIELD: begin
            if (ndig != 4'd0) begin
                if (field == 2'd0) a_addr <= acc[7:0];
                else               a_data <= acc;
            end
            field <= field + ((ndig != 4'd0) ? 2'd1 : 2'd0);
            st    <= E_EXEC;
        end

        E_EXEC: begin
            line_ready <= 1'b0;
            line_len   <= 6'd0;
            cmd_seen_r <= 1'b1;
            if (op == "R" && field == 2'd1) begin
                reg_addr <= a_addr;
                wait_cnt <= 2'd3;
                st       <= E_RWAIT;
            end else if (op == "W" && field == 2'd2) begin
                if (a_addr == 8'hF2)      status_en <= a_data[0];
                else if (a_addr == 8'hF3) scratch   <= a_data;
                else begin
                    reg_addr  <= a_addr;
                    reg_wdata <= a_data;
                    reg_we    <= 1'b1;
                end
                out[0] <= "O"; out[1] <= "K"; out[2] <= 8'h0D; out[3] <= 8'h0A;
                out_len <= 7'd4; out_idx <= 7'd0; out_busy <= 1'b1;
                st <= E_IDLE;
            end else begin
                out[0] <= "?"; out[1] <= 8'h0D; out[2] <= 8'h0A;
                out_len <= 7'd3; out_idx <= 7'd0; out_busy <= 1'b1;
                st <= E_IDLE;
            end
        end

        E_RWAIT:
            if (wait_cnt != 2'd0) begin
                wait_cnt <= wait_cnt - 2'd1;
            end else begin
                out[0] <= hexch(a_addr[7:4]); out[1] <= hexch(a_addr[3:0]);
                out[2] <= "=";
                for (k = 0; k < 8; k = k + 1)
                    out[3 + k] <= hexch(rv[31 - 4*k -: 4]);
                out[11] <= 8'h0D; out[12] <= 8'h0A;
                out_len <= 7'd13; out_idx <= 7'd0; out_busy <= 1'b1;
                st <= E_IDLE;
            end

        E_STAT:
            if (wait_cnt != 2'd0) begin
                wait_cnt <= wait_cnt - 2'd1;
            end else begin
                // append " aa=dddddddd" at out_len
                out[out_len[5:0] + 0] <= " ";
                out[out_len[5:0] + 1] <= hexch(sa[7:4]);
                out[out_len[5:0] + 2] <= hexch(sa[3:0]);
                out[out_len[5:0] + 3] <= "=";
                for (k = 0; k < 8; k = k + 1)
                    out[out_len[5:0] + 4 + k] <= hexch(reg_rdata[31 - 4*k -: 4]);
                if (stat_i == 3'd4) begin
                    out[out_len[5:0] + 12] <= 8'h0D;
                    out[out_len[5:0] + 13] <= 8'h0A;
                    out_len  <= out_len + 7'd14;
                    out_idx  <= 7'd0;
                    out_busy <= 1'b1;
                    st       <= E_IDLE;
                end else begin
                    out_len  <= out_len + 7'd12;
                    stat_i   <= stat_i + 3'd1;
                    reg_addr <= stat_addr(stat_i + 3'd1);
                    wait_cnt <= 2'd3;
                end
            end

        default: st <= E_IDLE;
        endcase

        if (!rst_n) begin
            st           <= E_IDLE;
            out_busy     <= 1'b0;
            line_ready   <= 1'b0;
            line_len     <= 6'd0;
            stat_pending <= 1'b0;
            status_en    <= 1'b1;
        end
    end

endmodule
