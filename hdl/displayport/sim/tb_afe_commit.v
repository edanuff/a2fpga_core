// tb_afe_commit.v — M5 commit discipline + state retention (review items 3/4):
//   A. APPLY_ON_TRAINING_START=1: declaration stays 0x06 and afe_busy=1 until
//      the INIT DRP sequence COMPLETES; a request is declared only after its
//      sequence completes; a TIMED-OUT sequence does NOT commit (declaration
//      keeps the old levels, seq_err set).
//   B. training drop/rise RETAINS the committed state (no writes, same byte);
//      phy_reinit forgets it -> next training start re-baselines (INIT written).
//   Run: iverilog -g2012 -o /tmp/tb_afe_commit.vvp hdl/displayport/sim/tb_afe_commit.v \
//          hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_afe_commit.vvp
`timescale 1ns/1ps
module tb_afe_commit;
    reg mgmt_clk = 0, drp_clk = 0;
    always #5   mgmt_clk = ~mgmt_clk;
    always #4.1 drp_clk  = ~drp_clk;
    reg [1:0] vs_request = 0, pe_request = 0;
    reg adjust_de = 0, training_active = 0, phy_reinit = 0, slave_dead = 0;
    reg phase_done = 0;
    wire [15:0] byte_o; wire busy; wire [5:0] dbg;
    wire req; reg gnt = 0; wire [23:0] addr; wire [31:0] data; wire wren; reg ready = 0;
    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2),
        .LANE_BASE0(24'h808300), .LANE_BASE1(24'h808400),
        .INIT_VS(2'd2), .INIT_PE(2'd0), .APPLY_ON_TRAINING_START(1)) dut (
        .mgmt_clk(mgmt_clk), .vs_request({vs_request,vs_request}), .pe_request({pe_request,pe_request}),
        .adjust_de(adjust_de), .training_active(training_active), .phase_done(phase_done), .phy_reinit(phy_reinit),
        .train_set_byte(byte_o), .afe_busy(busy), .dbg_afe(dbg), .dbg_afe1(),
        .drp_clk(drp_clk), .drp_req(req), .drp_gnt(gnt), .drp_addr(addr),
        .drp_wrdata(data), .drp_wren(wren), .drp_ready(ready));
    integer rd_cnt = 0, wr_cnt = 0; reg wren_d = 0; reg [31:0] wr_data [0:255];
    always @(posedge drp_clk) begin
        if (!req) begin gnt <= 0; rd_cnt <= 0; end
        else if (!gnt) begin rd_cnt <= rd_cnt + 1; if (rd_cnt >= 3) gnt <= 1; end
        ready <= 0; wren_d <= wren;
        if (wren && !wren_d) rd_cnt <= 0;
        if (wren && wren_d) begin rd_cnt <= rd_cnt + 1;
            if (rd_cnt == 6 && !slave_dead) begin ready <= 1; wr_data[wr_cnt] <= data; wr_cnt <= wr_cnt + 1; end end
    end
    task send_adjust(input [1:0] vs, input [1:0] pe);
        begin @(negedge mgmt_clk); vs_request = vs; pe_request = pe; adjust_de = 1;
              @(negedge mgmt_clk); adjust_de = 0; end
    endtask
    integer errors = 0; reg saw_busy_idle = 0;
    initial begin
        #100; training_active = 1;
        // ---- A1: during the INIT write the declaration is legacy and busy is high
        #500;
        if (!busy || byte_o[7:0] !== 8'h06) begin errors = errors + 1; $display("FAIL A1: mid-INIT busy=%0d byte=%02x (want 1/0x06)", busy, byte_o[7:0]); end
        else $display("  ok A1: INIT in flight -> busy=1, declaration 0x06");
        #(20_000);
        if (busy || wr_cnt != 8 || byte_o[7:0] !== 8'h06 || !dbg[4]) begin errors = errors + 1; $display("FAIL A1b: after INIT busy=%0d wr=%0d byte=%02x known=%0d", busy, wr_cnt, byte_o[7:0], dbg[4]); end
        else $display("  ok A1b: INIT committed (8 writes), baseline known");
        // ---- A2: request VS1/PE0 — declaration must NOT change until the sequence completes
        send_adjust(2'd1, 2'd0); #300;
        if (!busy || byte_o[7:0] !== 8'h06) begin errors = errors + 1; $display("FAIL A2: in-flight declaration changed early (busy=%0d byte=%02x)", busy, byte_o[7:0]); end
        else $display("  ok A2: VS1 in flight -> still declares 0x06, busy=1");
        #(20_000);
        if (busy || byte_o[7:0] !== 8'h01 || wr_cnt != 16) begin errors = errors + 1; $display("FAIL A2b: after VS1 byte=%02x wr=%0d", byte_o[7:0], wr_cnt); end
        else $display("  ok A2b: VS1 committed on completion -> 0x01");
        // ---- A3: dead DRP slave: sequence times out -> no commit, seq_err set
        slave_dead = 1; send_adjust(2'd2, 2'd1);
        #(8 * 5000 * 8.2 + 40_000);   // 8 writes x 4096-cycle timeout at 8.2 ns + margin
        if (busy || byte_o[7:0] !== 8'h01 || !dbg[5]) begin errors = errors + 1; $display("FAIL A3: failed seq: busy=%0d byte=%02x err=%0d (want 0/0x01/1)", busy, byte_o[7:0], dbg[5]); end
        else $display("  ok A3: timed-out sequence NOT committed (still 0x01), seq_err flagged");
        slave_dead = 0;
        // ---- B1: training drop/rise retains state: no writes, same byte
        training_active = 0; #2000; training_active = 1; #(20_000);
        if (wr_cnt != 16 || byte_o[7:0] !== 8'h01) begin errors = errors + 1; $display("FAIL B1: retention: wr=%0d byte=%02x", wr_cnt, byte_o[7:0]); end
        else $display("  ok B1: training restart retained VS1 (no writes, 0x01)");
        // ---- B2: phy_reinit forgets -> re-baseline INIT written at next training
        phy_reinit = 1; #200; phy_reinit = 0;
        if (byte_o[7:0] !== 8'h06) begin errors = errors + 1; $display("FAIL B2: after reinit byte=%02x (want legacy 0x06)", byte_o[7:0]); end
        #(20_000);
        if (wr_cnt != 24 || wr_data[16] !== {16'd0, 4'd13, 12'd0} || byte_o[7:0] !== 8'h06) begin errors = errors + 1; $display("FAIL B2b: re-baseline: wr=%0d data16=%08x byte=%02x", wr_cnt, wr_data[16], byte_o[7:0]); end
        else $display("  ok B2: phy_reinit -> INIT re-written (txlev 13), declares 0x06");
        if (errors == 0) $display("PASS: commit-on-completion, no-commit-on-failure, retention across training, reinit re-baselines");
        else $display("FAIL: %0d error(s)", errors);
        $finish;
    end
endmodule
