// tb_afe_noinit.v — M5 APPLY_ON_TRAINING_START=0 (row 76 isolation): trust
// the boot csr. Training start must produce ZERO DRP writes while still
// declaring the INIT levels (0x06 = VS2 at the MAX_VS=2 ceiling); a real
// sink change (VS1/PE0) applies once (8 writes, byte 0x01); a request
// back to INIT re-applies (the PHY is now manual, so it must be written).
//   Run: iverilog -g2012 -o /tmp/tb_afe_noinit.vvp \
//          hdl/displayport/sim/tb_afe_noinit.v hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_afe_noinit.vvp
`timescale 1ns/1ps
module tb_afe_noinit;
    reg mgmt_clk = 0, drp_clk = 0;
    always #5   mgmt_clk = ~mgmt_clk;
    always #4.1 drp_clk  = ~drp_clk;
    reg [1:0] vs_request = 0, pe_request = 0;
    reg adjust_de = 0, training_active = 0;
    wire [7:0] byte_o; wire busy; wire [5:0] dbg;
    wire req; reg gnt = 0; wire [23:0] addr; wire [31:0] data; wire wren; reg ready = 0;
    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2),
        .LANE_BASE0(24'h808300), .LANE_BASE1(24'h808400),
        .INIT_VS(2'd2), .INIT_PE(2'd0), .APPLY_ON_TRAINING_START(0)) dut (
        .mgmt_clk(mgmt_clk), .vs_request(vs_request), .pe_request(pe_request),
        .adjust_de(adjust_de), .training_active(training_active),
        .train_set_byte(byte_o), .afe_busy(busy), .dbg_afe(dbg),
        .drp_clk(drp_clk), .drp_req(req), .drp_gnt(gnt), .drp_addr(addr),
        .drp_wrdata(data), .drp_wren(wren), .drp_ready(ready));
    integer rd_cnt = 0, wr_cnt = 0; reg wren_d = 0; reg [31:0] wr_data [0:255];
    always @(posedge drp_clk) begin
        if (!req) begin gnt <= 0; rd_cnt <= 0; end
        else if (!gnt) begin rd_cnt <= rd_cnt + 1; if (rd_cnt >= 3) gnt <= 1; end
        ready <= 0; wren_d <= wren;
        if (wren && !wren_d) rd_cnt <= 0;
        if (wren && wren_d) begin rd_cnt <= rd_cnt + 1;
            if (rd_cnt == 6) begin ready <= 1; wr_data[wr_cnt] <= data; wr_cnt <= wr_cnt + 1; end end
    end
    task send_adjust(input [1:0] vs, input [1:0] pe);
        begin @(negedge mgmt_clk); vs_request = vs; pe_request = pe; adjust_de = 1;
              @(negedge mgmt_clk); adjust_de = 0; end
    endtask
    integer errors = 0;
    initial begin
        #100; training_active = 1; #(20_000);
        if (wr_cnt != 0) begin errors = errors + 1; $display("FAIL: INIT produced %0d DRP writes (want 0)", wr_cnt); end
        else $display("  ok: training start -> zero DRP writes");
        if (byte_o !== 8'h06) begin errors = errors + 1; $display("FAIL: declared byte %02x (want 0x06)", byte_o); end
        else $display("  ok: declares 0x06 (VS2 at ceiling) without writing");
        send_adjust(2'd2, 2'd0); #(20_000);   // same as INIT -> nothing
        if (wr_cnt != 0) begin errors = errors + 1; $display("FAIL: VS2/PE0 request (== INIT) wrote %0d", wr_cnt); end
        else $display("  ok: request equal to INIT -> no writes");
        send_adjust(2'd1, 2'd0); #(20_000);   // real change -> 8 writes, txlev 9
        if (wr_cnt != 8 || wr_data[0] !== {16'd0, 4'd9, 12'd0} || byte_o !== 8'h01) begin
            errors = errors + 1; $display("FAIL: VS1 apply: writes %0d data0 %08x byte %02x", wr_cnt, wr_data[0], byte_o); end
        else $display("  ok: VS1/PE0 -> 8 writes, txlev 9, byte 0x01");
        send_adjust(2'd2, 2'd0); #(20_000);   // back to INIT -> must re-apply (PHY is manual now)
        if (wr_cnt != 16 || wr_data[8] !== {16'd0, 4'd13, 12'd0} || byte_o !== 8'h06) begin
            errors = errors + 1; $display("FAIL: back-to-INIT: writes %0d data8 %08x byte %02x", wr_cnt, wr_data[8], byte_o); end
        else $display("  ok: back to VS2 -> re-applied (txlev 13), byte 0x06");
        training_active = 0; #(2_000); training_active = 1; #(20_000);
        if (wr_cnt != 16) begin errors = errors + 1; $display("FAIL: next training start wrote %0d more", wr_cnt - 16); end
        else $display("  ok: next training start -> still no INIT writes");
        if (errors == 0) $display("PASS: APPLY_ON_TRAINING_START=0 — boot csr trusted, DRP only on real change");
        else $display("FAIL: %0d error(s)", errors);
        $finish;
    end
endmodule
