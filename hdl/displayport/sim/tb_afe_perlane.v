// tb_afe_perlane.v — M5 per-lane ADJUST_REQUEST (review item 2, 08-22).
//
// Policy under test (user decision): per-lane VALUES, ALL-LANE APPLICATION.
// Each lane carries its own requested VS/PE, its own DRP payload written to
// its own LANE_BASE, and its own TRAINING_LANEx_SET byte — but when ANY
// lane changes, EVERY lane is written, so the two lanes never end up in
// different FFE modes (manual vs the boot csr's Auto).
//
// Checks (APPLY_ON_TRAINING_START = 0, the production policy):
//   1. training start: both lanes declare INIT (0x0606), zero DRP writes.
//   2. A:0026 (lane0 VS2/PE1, lane1 VS2/PE0): 8 writes — lane 0 block at
//      0x8083xx carries C1=7, lane 1 block at 0x8084xx carries C1=0 — and
//      the declared bytes differ per lane: {lane1 0x06, lane0 0x2E}.
//   3. the same request again: nothing (per-lane debounce).
//   4. lane-1-only change: both lanes written, lane 1's payload changes.
//   5. per-lane clamp/sanitise: an illegal VS3/PE3 on lane 1 only.
//   6. commit discipline still holds: the declaration changes only when
//      the sequence completes.
//
// Run: iverilog -g2012 -o /tmp/tb_afe_perlane.vvp \
//        hdl/displayport/sim/tb_afe_perlane.v \
//        hdl/displayport/gowin/afe_adjust_seq.v && vvp /tmp/tb_afe_perlane.vvp
`timescale 1ns/1ps
module tb_afe_perlane;
    reg mgmt_clk = 0, drp_clk = 0;
    always #5   mgmt_clk = ~mgmt_clk;
    always #4.1 drp_clk  = ~drp_clk;

    // packed {lane1, lane0}
    reg  [3:0] vs_request = 0, pe_request = 0;
    reg        adjust_de = 0, training_active = 0;
    wire [15:0] byte_o; wire busy; wire [5:0] dbg; wire [3:0] dbg1;
    wire req; reg gnt = 0; wire [23:0] addr; wire [31:0] data; wire wren; reg ready = 0;

    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2),
        .LANE_BASE0(24'h808300), .LANE_BASE1(24'h808400),
        .INIT_VS(2'd2), .INIT_PE(2'd0), .APPLY_ON_TRAINING_START(0),
        .MAX_VS(2'd2), .MAX_PE(2'd3)) dut (
        .mgmt_clk(mgmt_clk), .vs_request(vs_request), .pe_request(pe_request),
        .adjust_de(adjust_de), .training_active(training_active), .phy_reinit(1'b0),
        .train_set_byte(byte_o), .afe_busy(busy), .dbg_afe(dbg), .dbg_afe1(dbg1),
        .drp_clk(drp_clk), .drp_req(req), .drp_gnt(gnt), .drp_addr(addr),
        .drp_wrdata(data), .drp_wren(wren), .drp_ready(ready));

    // mock DRP slave: grant after 3 cycles, ack each write after 6
    integer rd_cnt = 0, wr_cnt = 0; reg wren_d = 0;
    reg [23:0] wr_addr [0:255]; reg [31:0] wr_data [0:255];
    always @(posedge drp_clk) begin
        if (!req) begin gnt <= 0; rd_cnt <= 0; end
        else if (!gnt) begin rd_cnt <= rd_cnt + 1; if (rd_cnt >= 3) gnt <= 1; end
        ready <= 0; wren_d <= wren;
        if (wren && !wren_d) rd_cnt <= 0;
        if (wren && wren_d) begin rd_cnt <= rd_cnt + 1;
            if (rd_cnt == 6) begin ready <= 1; wr_addr[wr_cnt] <= addr;
                                   wr_data[wr_cnt] <= data; wr_cnt <= wr_cnt + 1; end end
    end

    task send_adjust(input [1:0] vs0, input [1:0] pe0, input [1:0] vs1, input [1:0] pe1);
        begin @(negedge mgmt_clk); vs_request = {vs1, vs0}; pe_request = {pe1, pe0};
              adjust_de = 1; @(negedge mgmt_clk); adjust_de = 0; end
    endtask
    task settle; begin #(30_000); end endtask

    integer errors = 0;
    // one lane's 4-write block: base+0x34 txlev, +0x38 C1, +0xd8 enable, +0xd8 strobe
    task check_block(input integer b, input [23:0] base, input [3:0] txlev,
                     input [4:0] c1, input [255:0] name);
        begin
            if (wr_addr[b+0] !== base + 24'h034 || wr_data[b+0] !== {16'd0, txlev, 12'd0}) begin
                errors = errors + 1;
                $display("FAIL %0s: swing write %06x=%08x (want %06x=%08x)", name,
                         wr_addr[b+0], wr_data[b+0], base + 24'h034, {16'd0, txlev, 12'd0});
            end
            if (wr_addr[b+1] !== base + 24'h038 || wr_data[b+1] !== {19'd0, c1, 8'd0}) begin
                errors = errors + 1;
                $display("FAIL %0s: FFE write %06x=%08x (want %06x=%08x)", name,
                         wr_addr[b+1], wr_data[b+1], base + 24'h038, {19'd0, c1, 8'd0});
            end
            if (wr_addr[b+2] !== base + 24'h0d8 || wr_data[b+2] !== 32'h10 ||
                wr_addr[b+3] !== base + 24'h0d8 || wr_data[b+3] !== 32'h110) begin
                errors = errors + 1; $display("FAIL %0s: enable/strobe pair wrong", name);
            end
        end
    endtask
    task check_bytes(input [15:0] want, input [255:0] name);
        begin
            if (byte_o !== want) begin errors = errors + 1;
                $display("FAIL %0s: bytes %04x want %04x", name, byte_o, want); end
            else $display("  ok: %0s -> {lane1 %02x, lane0 %02x}", name, byte_o[15:8], byte_o[7:0]);
        end
    endtask

    initial begin
        #100; training_active = 1; settle;
        // ---- 1. boot csr trusted: both lanes declare INIT, no writes ----
        if (wr_cnt != 0) begin errors = errors + 1;
            $display("FAIL: training start wrote %0d (want 0)", wr_cnt); end
        check_bytes(16'h0606, "training start declares INIT on both lanes");

        // ---- 2. A:0026 — lane 0 asks PE1, lane 1 stays at INIT ----------
        send_adjust(2'd2, 2'd1, 2'd2, 2'd0); settle;
        if (wr_cnt != 8) begin errors = errors + 1;
            $display("FAIL: asymmetric request wrote %0d (want 8 = both lanes)", wr_cnt); end
        check_block(0, 24'h808300, 4'd13, 5'd7, "lane 0 gets its own PE1 (C1=7)");
        check_block(4, 24'h808400, 4'd13, 5'd0, "lane 1 gets its own PE0 (C1=0), same mode");
        // VS2 is the ceiling -> MAX_SWING; VS2+PE1 = 3 -> MAX_PE too (0x2E)
        check_bytes(16'h062E, "A:0026 -> per-lane declaration");

        // ---- 3. same request again: per-lane debounce -------------------
        send_adjust(2'd2, 2'd1, 2'd2, 2'd0); settle;
        if (wr_cnt != 8) begin errors = errors + 1;
            $display("FAIL: repeat request wrote %0d more", wr_cnt - 8); end
        else $display("  ok: repeated request -> no writes");

        // ---- 4. lane-1-only change: both lanes written ------------------
        send_adjust(2'd2, 2'd1, 2'd1, 2'd0); settle;
        if (wr_cnt != 16) begin errors = errors + 1;
            $display("FAIL: lane-1 change wrote %0d (want 8 more)", wr_cnt - 8); end
        check_block(8,  24'h808300, 4'd13, 5'd7, "lane 0 re-written unchanged (same mode)");
        check_block(12, 24'h808400, 4'd9,  5'd0, "lane 1 moved to VS1");
        check_bytes(16'h012E, "lane-1-only change -> only lane 1's byte moves");

        // ---- 5. per-lane clamp + VS+PE<=3 sanitise on lane 1 only -------
        // lane1 asks VS3/PE3 -> clamp VS to 2, then PE yields to 1 => 0x2E
        send_adjust(2'd2, 2'd1, 2'd3, 2'd3); settle;
        if (wr_cnt != 24) begin errors = errors + 1;
            $display("FAIL: clamp case wrote %0d (want 8 more)", wr_cnt - 16); end
        check_block(20, 24'h808400, 4'd13, 5'd7, "lane 1 clamped+sanitised to VS2/PE1");
        check_bytes(16'h2E2E, "both lanes now VS2/PE1");

        // ---- 6. commit discipline: declaration waits for completion -----
        send_adjust(2'd0, 2'd0, 2'd0, 2'd0);
        #400;
        if (!busy || byte_o !== 16'h2E2E) begin errors = errors + 1;
            $display("FAIL: declaration moved before the sequence completed (busy=%0d bytes=%04x)",
                     busy, byte_o); end
        else $display("  ok: in-flight -> still declares the committed levels");
        settle;
        check_bytes(16'h0000, "VS0/PE0 committed on completion");
        if (dbg[3:0] !== 4'h0 || dbg1 !== 4'h0) begin errors = errors + 1;
            $display("FAIL: dbg lane0 %0h lane1 %0h (want 0/0)", dbg[3:0], dbg1); end
        else $display("  ok: dbg_afe/dbg_afe1 report their own lanes");

        if (errors == 0)
            $display("PASS: per-lane values with all-lane application — own request, own LANE_BASE payload, own declared byte, no FFE-mode split");
        else $display("FAIL: %0d error(s)", errors);
        $finish;
    end
    initial begin #3_000_000; $display("FAIL: TB watchdog timeout"); $finish; end
endmodule
