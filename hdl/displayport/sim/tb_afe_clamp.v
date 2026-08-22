// tb_afe_clamp.v — M5 declared-ceiling clamp (row 75): with the default
// MAX_VS=2, a sink request above VS2 is applied AT VS2 (txlev 13) and the
// wire byte raises MAX_SWING there; pre-emphasis requests are still
// honored. MAX_PE=3 still echoes/flags at PE3.
//   Run: iverilog -g2012 -o /tmp/tb_afe_clamp.vvp \
//          hdl/displayport/sim/tb_afe_clamp.v hdl/displayport/gowin/afe_adjust_seq.v \
//        && vvp /tmp/tb_afe_clamp.vvp
`timescale 1ns/1ps
module tb_afe_clamp;
    reg mgmt_clk = 0, drp_clk = 0;
    always #5   mgmt_clk = ~mgmt_clk;
    always #4.1 drp_clk  = ~drp_clk;
    reg [1:0] vs_request = 0, pe_request = 0;
    reg adjust_de = 0, training_active = 0;
    wire [7:0] byte_o; wire busy; wire [5:0] dbg;
    wire req; reg gnt = 0; wire [23:0] addr; wire [31:0] data; wire wren; reg ready = 0;

    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2),
        .LANE_BASE0(24'h808300), .LANE_BASE1(24'h808400),
        .INIT_VS(2'd2), .INIT_PE(2'd0)) dut (   // MAX_VS/MAX_PE at defaults (2 / 3)
        .mgmt_clk(mgmt_clk), .vs_request(vs_request), .pe_request(pe_request),
        .adjust_de(adjust_de), .training_active(training_active), .phy_reinit(1'b0),
        .train_set_byte(byte_o), .afe_busy(busy), .dbg_afe(dbg),
        .drp_clk(drp_clk), .drp_req(req), .drp_gnt(gnt), .drp_addr(addr),
        .drp_wrdata(data), .drp_wren(wren), .drp_ready(ready));

    // mock DRP slave: grant after 3 cycles; ack each write after 6
    integer rd_cnt = 0, wr_cnt = 0; reg wren_d = 0;
    reg [23:0] wr_addr [0:255]; reg [31:0] wr_data [0:255];
    always @(posedge drp_clk) begin
        if (!req) begin gnt <= 0; rd_cnt <= 0; end
        else if (!gnt) begin rd_cnt <= rd_cnt + 1; if (rd_cnt >= 3) gnt <= 1; end
        ready <= 0; wren_d <= wren;
        if (wren && !wren_d) rd_cnt <= 0;
        if (wren && wren_d) begin
            rd_cnt <= rd_cnt + 1;
            if (rd_cnt == 6) begin ready <= 1; wr_addr[wr_cnt] <= addr; wr_data[wr_cnt] <= data; wr_cnt <= wr_cnt + 1; end
        end
    end

    task send_adjust(input [1:0] vs, input [1:0] pe);
        begin @(negedge mgmt_clk); vs_request = vs; pe_request = pe; adjust_de = 1;
              @(negedge mgmt_clk); adjust_de = 0; end
    endtask
    task settle; begin #(20_000); end endtask   // 20 us: sequence completes

    integer errors = 0, base;
    task check_app(input integer b, input [3:0] txlev, input [4:0] c1, input [7:0] exp_byte, input [255:0] name);
        begin
            // write b+0 = lane0 swing, b+1 = lane0 FFE, b+4 = lane1 swing, b+5 = lane1 FFE
            if (wr_cnt != b + 8) begin errors = errors + 1; $display("FAIL %0s: expected %0d writes, got %0d", name, b + 8, wr_cnt); end
            if (wr_data[b+0] !== {16'd0, txlev, 12'd0} || wr_data[b+4] !== {16'd0, txlev, 12'd0}) begin
                errors = errors + 1; $display("FAIL %0s: swing data %08x/%08x (want txlev %0d)", name, wr_data[b+0], wr_data[b+4], txlev); end
            if (wr_data[b+1] !== {19'd0, c1, 8'd0}) begin
                errors = errors + 1; $display("FAIL %0s: FFE data %08x (want C1 %0d)", name, wr_data[b+1], c1); end
            if (byte_o !== exp_byte) begin
                errors = errors + 1; $display("FAIL %0s: byte %02x (want %02x)", name, byte_o, exp_byte); end
            else $display("  ok: %0s -> txlev %0d, C1 %0d, byte 0x%02x", name, txlev, c1, byte_o);
        end
    endtask

    initial begin
        #100; training_active = 1; settle;
        // INIT VS2/PE0: at the ceiling already => MAX_SWING flagged = legacy 0x06
        check_app(0, 4'd13, 5'd0, 8'h06, "INIT VS2/PE0 (ceiling)");
        // the Ugreen's escalation: VS3/PE0 -> clamped to VS2, NO new application
        send_adjust(2'd3, 2'd0); settle;
        if (wr_cnt != 8) begin errors = errors + 1; $display("FAIL: VS3/PE0 must clamp to the already-applied VS2 (no writes), got %0d", wr_cnt); end
        else $display("  ok: VS3/PE0 request clamps to VS2 -> no re-application, byte 0x%02x", byte_o);
        // the hoped-for request: VS3/PE1 -> VS2 + PE1 = txlev 13 / C1 7, byte 0x0E (telemetry M:06 established / M:16 in training)
        send_adjust(2'd3, 2'd1); settle;
        check_app(8, 4'd13, 5'd7, 8'h2E, "VS3/PE1 -> clamped VS2 + PE1");  // {00,maxpe1(VS2+PE1=3),pe01,maxsw1,vs10}
        // VS+PE<=3 sanitising: VS3/PE3 -> VS2 (ceiling) + PE1 (3-2) = already applied -> NO writes, byte 0x2E
        send_adjust(2'd3, 2'd3); settle;
        if (wr_cnt != 16 || byte_o !== 8'h2E) begin errors = errors + 1; $display("FAIL: VS3/PE3 must sanitise to VS2/PE1 (no writes, 0x2E); writes %0d byte %02x", wr_cnt, byte_o); end
        else $display("  ok: VS3/PE3 -> sanitised to VS2/PE1 (already applied), no writes, byte 0x2E");
        // VS0/PE3 is a legal combination (sum 3): txlev 5 / C1 13, MAX_PE flagged, byte 0x2C
        send_adjust(2'd0, 2'd3); settle;
        check_app(16, 4'd5, 5'd13, 8'h38, "VS0/PE3 -> legal, honored");  // {00,maxpe1,pe11,maxsw0,vs00}
        // below ceiling still honored: VS1/PE0 -> txlev 9 / C1 0, byte 0x01
        send_adjust(2'd1, 2'd0); settle;
        check_app(24, 4'd9, 5'd0, 8'h01, "VS1/PE0 -> honored, no flags");
        if (dbg[5]) begin errors = errors + 1; $display("FAIL: seq_err set"); end
        if (errors == 0) $display("PASS: declared-ceiling clamp (MAX_VS=2) + VS+PE<=3 sanitising");
        else $display("FAIL: %0d error(s)", errors);
        $finish;
    end
endmodule
