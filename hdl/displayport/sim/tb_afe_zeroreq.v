// tb_afe_zeroreq.v — IGNORE_ZERO_REQUEST=1, the per-sink COMPATIBILITY
// workaround (test log row 81). Not protocol semantics: 0x00 literally
// encodes VS0/PE0, so this is opt-in per board and OFF by default. The
// protocol-correct suppression is the phase_done gate (tb_afe_perlane).
//
// Run: iverilog -g2012 -o /tmp/tb_afe_zeroreq.vvp \
//        hdl/displayport/sim/tb_afe_zeroreq.v \
//        hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_afe_zeroreq.vvp
`timescale 1ns/1ps
module tb_afe_zeroreq;
    reg mgmt_clk = 0, drp_clk = 0;
    always #5   mgmt_clk = ~mgmt_clk;
    always #4.1 drp_clk  = ~drp_clk;
    reg [3:0] vs_request = 0, pe_request = 0;
    reg adjust_de = 0, training_active = 0, phase_done = 0;
    wire [15:0] byte_o; wire busy; wire [5:0] dbg; wire [3:0] dbg1;
    wire req; reg gnt = 0; wire [23:0] addr; wire [31:0] data; wire wren; reg ready = 0;
    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2),
        .LANE_BASE0(24'h808300), .LANE_BASE1(24'h808400),
        .INIT_VS(2'd2), .INIT_PE(2'd0), .APPLY_ON_TRAINING_START(0),
        .IGNORE_ZERO_REQUEST(1)) dut (
        .mgmt_clk(mgmt_clk), .vs_request(vs_request), .pe_request(pe_request),
        .adjust_de(adjust_de), .training_active(training_active),
        .phase_done(phase_done), .phy_reinit(1'b0),
        .train_set_byte(byte_o), .afe_busy(busy), .dbg_afe(dbg), .dbg_afe1(dbg1), .dbg_evt(dbg_evt),
        .drp_clk(drp_clk), .drp_req(req), .drp_gnt(gnt), .drp_addr(addr),
        .drp_wrdata(data), .drp_wren(wren), .drp_ready(ready));
    integer rd_cnt = 0, wr_cnt = 0; reg wren_d = 0;
    always @(posedge drp_clk) begin
        if (!req) begin gnt <= 0; rd_cnt <= 0; end
        else if (!gnt) begin rd_cnt <= rd_cnt + 1; if (rd_cnt >= 3) gnt <= 1; end
        ready <= 0; wren_d <= wren;
        if (wren && !wren_d) rd_cnt <= 0;
        if (wren && wren_d) begin rd_cnt <= rd_cnt + 1;
            if (rd_cnt == 6) begin ready <= 1; wr_cnt <= wr_cnt + 1; end end
    end
    task send_adjust(input [1:0] vs0, input [1:0] pe0, input [1:0] vs1, input [1:0] pe1);
        begin @(negedge mgmt_clk); vs_request = {vs1, vs0}; pe_request = {pe1, pe0};
              adjust_de = 1; @(negedge mgmt_clk); adjust_de = 0; end
    endtask
    integer errors = 0, wr_before = 0;

    wire [11:0] dbg_evt;
    initial begin
        #100; training_active = 1; #(30_000);
        // all-zero: suppressed by the workaround
        wr_before = wr_cnt;
        send_adjust(2'd0, 2'd0, 2'd0, 2'd0); #(30_000);
        if (wr_cnt != wr_before || byte_o !== 16'h0606) begin errors = errors + 1;
            $display("FAIL: all-zero not suppressed (wrote %0d, bytes %04x)",
                     wr_cnt - wr_before, byte_o); end
        else $display("  ok: all-zero suppressed by the workaround, levels stand (0x0606)");
        // zero on ONE lane only is a genuine asymmetric ask -> still honored
        send_adjust(2'd0, 2'd0, 2'd2, 2'd0); #(30_000);
        if (wr_cnt != wr_before + 8 || byte_o !== 16'h0600) begin errors = errors + 1;
            $display("FAIL: one-lane zero must be honored (wrote %0d, bytes %04x)",
                     wr_cnt - wr_before, byte_o); end
        else $display("  ok: zero on ONE lane still honored (bytes 0x0600)");
        if (errors == 0)
            $display("PASS: IGNORE_ZERO_REQUEST workaround — all-zero suppressed, one-lane zero honored");
        else $display("FAIL: %0d error(s)", errors);
        $finish;
    end
    initial begin #2_000_000; $display("FAIL: TB watchdog timeout"); $finish; end
endmodule
