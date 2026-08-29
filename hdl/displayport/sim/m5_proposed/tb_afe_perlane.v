// tb_afe_perlane.v — PROPOSED per-lane afe_adjust_seq: each lane gets its
// own request nibble, its own DRP payload (own LANE_BASE only) and its own
// TRAINING_LANEx_SET byte. Mock DRP slave as in tb_afe_noinit.v.
//   Run: iverilog -g2012 -o /tmp/tb_afe_perlane.vvp \
//          hdl/displayport/sim/m5_proposed/tb_afe_perlane.v \
//          hdl/displayport/sim/m5_proposed/afe_adjust_seq_perlane.v && vvp /tmp/tb_afe_perlane.vvp
// Byte encoding (design doc §4): {2'b00, MAX_PE, pe[1:0], MAX_SWING, vs[1:0]},
// MAX_VS=2 / MAX_PE=3 defaults -> VS2/PE0 = 0x06, VS2/PE1 = 0x0E, VS1/PE0 = 0x01,
// VS2/PE2 = 0x16, VS2/PE3 = 0x3E.
`timescale 1ns/1ps
module tb_afe_perlane;
    reg mgmt_clk = 0, drp_clk = 0;
    always #5   mgmt_clk = ~mgmt_clk;
    always #4.1 drp_clk  = ~drp_clk;
    reg [3:0] vs_request = 0, pe_request = 0;   // {lane1, lane0}
    reg adjust_de = 0, training_active = 0;

    localparam [23:0] B0 = 24'h808300, B1 = 24'h808400;

    // ---- DUT A: production policy (APPLY_ON_TRAINING_START = 0) ----
    wire [15:0] byte_a; wire busy_a; wire [5:0] dbg_a; wire [3:0] dbg1_a;
    wire req_a; reg gnt_a = 0; wire [23:0] addr_a; wire [31:0] data_a; wire wren_a; reg ready_a = 0;
    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2), .LANE_BASE0(B0), .LANE_BASE1(B1),
        .INIT_VS(2'd2), .INIT_PE(2'd0), .APPLY_ON_TRAINING_START(0)) dut_a (
        .mgmt_clk(mgmt_clk), .vs_request(vs_request), .pe_request(pe_request),
        .adjust_de(adjust_de), .training_active(training_active),
        .train_set_byte(byte_a), .afe_busy(busy_a), .dbg_afe(dbg_a), .dbg_afe1(dbg1_a),
        .drp_clk(drp_clk), .drp_req(req_a), .drp_gnt(gnt_a), .drp_addr(addr_a),
        .drp_wrdata(data_a), .drp_wren(wren_a), .drp_ready(ready_a));
    integer rc_a = 0, wc_a = 0; reg wd_a = 0; reg [23:0] wa_a [0:255]; reg [31:0] wv_a [0:255];
    always @(posedge drp_clk) begin
        if (!req_a) begin gnt_a <= 0; rc_a <= 0; end
        else if (!gnt_a) begin rc_a <= rc_a + 1; if (rc_a >= 3) gnt_a <= 1; end
        ready_a <= 0; wd_a <= wren_a;
        if (wren_a && !wd_a) rc_a <= 0;
        if (wren_a && wd_a) begin rc_a <= rc_a + 1;
            if (rc_a == 6) begin ready_a <= 1; wa_a[wc_a] <= addr_a; wv_a[wc_a] <= data_a; wc_a <= wc_a + 1; end end
    end

    // ---- DUT B: APPLY_ON_TRAINING_START = 1 (INIT on every lane) ----
    wire [15:0] byte_b; wire busy_b; wire [5:0] dbg_b; wire [3:0] dbg1_b;
    wire req_b; reg gnt_b = 0; wire [23:0] addr_b; wire [31:0] data_b; wire wren_b; reg ready_b = 0;
    afe_adjust_seq #(.ENABLE_AFE_ADJUST(1), .NUM_LANES(2), .LANE_BASE0(B0), .LANE_BASE1(B1),
        .INIT_VS(2'd2), .INIT_PE(2'd0), .APPLY_ON_TRAINING_START(1)) dut_b (
        .mgmt_clk(mgmt_clk), .vs_request(vs_request), .pe_request(pe_request),
        .adjust_de(adjust_de), .training_active(training_active),
        .train_set_byte(byte_b), .afe_busy(busy_b), .dbg_afe(dbg_b), .dbg_afe1(dbg1_b),
        .drp_clk(drp_clk), .drp_req(req_b), .drp_gnt(gnt_b), .drp_addr(addr_b),
        .drp_wrdata(data_b), .drp_wren(wren_b), .drp_ready(ready_b));
    integer rc_b = 0, wc_b = 0; reg wd_b = 0; reg [23:0] wa_b [0:255]; reg [31:0] wv_b [0:255];
    always @(posedge drp_clk) begin
        if (!req_b) begin gnt_b <= 0; rc_b <= 0; end
        else if (!gnt_b) begin rc_b <= rc_b + 1; if (rc_b >= 3) gnt_b <= 1; end
        ready_b <= 0; wd_b <= wren_b;
        if (wren_b && !wd_b) rc_b <= 0;
        if (wren_b && wd_b) begin rc_b <= rc_b + 1;
            if (rc_b == 6) begin ready_b <= 1; wa_b[wc_b] <= addr_b; wv_b[wc_b] <= data_b; wc_b <= wc_b + 1; end end
    end

    // send one ADJUST_REQUEST 0x206 byte: {pe1,vs1,pe0,vs0}
    task send_adjust(input [1:0] vs0, input [1:0] pe0, input [1:0] vs1, input [1:0] pe1);
        begin @(negedge mgmt_clk); vs_request = {vs1, vs0}; pe_request = {pe1, pe0}; adjust_de = 1;
              @(negedge mgmt_clk); adjust_de = 0; end
    endtask
    task settle; begin #(30_000); end endtask

    integer errors = 0;
    // check one lane's 4-write block at index b of DUT A
    task check_block(input integer b, input [23:0] base, input [3:0] txlev, input [4:0] c1, input [639:0] name);
        begin
            if (wa_a[b+0] !== base + 24'h034 || wv_a[b+0] !== {16'd0, txlev, 12'd0}) begin errors = errors + 1;
                $display("FAIL %0s: w%0d addr %06x data %08x (want %06x txlev %0d)", name, b, wa_a[b+0], wv_a[b+0], base + 24'h034, txlev); end
            if (wa_a[b+1] !== base + 24'h038 || wv_a[b+1] !== {19'd0, c1, 8'd0}) begin errors = errors + 1;
                $display("FAIL %0s: w%0d addr %06x data %08x (want %06x C1 %0d)", name, b+1, wa_a[b+1], wv_a[b+1], base + 24'h038, c1); end
            if (wa_a[b+2] !== base + 24'h0d8 || wv_a[b+2] !== 32'h10) begin errors = errors + 1;
                $display("FAIL %0s: w%0d addr %06x data %08x (want %06x 0x010)", name, b+2, wa_a[b+2], wv_a[b+2], base + 24'h0d8); end
            if (wa_a[b+3] !== base + 24'h0d8 || wv_a[b+3] !== 32'h110) begin errors = errors + 1;
                $display("FAIL %0s: w%0d addr %06x data %08x (want %06x 0x110)", name, b+3, wa_a[b+3], wv_a[b+3], base + 24'h0d8); end
        end
    endtask
    task check_count(input integer want, input [639:0] name);
        begin if (wc_a != want) begin errors = errors + 1; $display("FAIL %0s: %0d writes total (want %0d)", name, wc_a, want); end end
    endtask
    task check_bytes(input [15:0] want, input [639:0] name);
        begin if (byte_a !== want) begin errors = errors + 1; $display("FAIL %0s: bytes %04x (want %04x)", name, byte_a, want); end
              else $display("  ok: %0s -> bytes {lane1 %02x, lane0 %02x}, %0d DRP writes so far", name, byte_a[15:8], byte_a[7:0], wc_a); end
    endtask

    initial begin
        #100; training_active = 1; settle;
        // 1. training start, no-init policy
        check_count(0, "training start (APPLY_ON_START=0)");
        check_bytes(16'h0606, "training start declares INIT on both lanes");
        if (dbg_a[3:0] !== 4'b0010 || dbg1_a !== 4'b0010) begin errors = errors + 1; $display("FAIL: dbg %b / dbg1 %b", dbg_a, dbg1_a); end

        // 2. A:0026 = lane0 VS2/PE1, lane1 VS2/PE0 -> lane 0 only, C1=7
        send_adjust(2'd2, 2'd1, 2'd2, 2'd0); settle;
        check_count(4, "A:0026");
        check_block(0, B0, 4'd13, 5'd7, "A:0026 lane0 block");
        check_bytes(16'h060E, "A:0026 (lane0 VS2/PE1, lane1 VS2/PE0) -> lane0 only");

        // 3. same request again -> nothing
        send_adjust(2'd2, 2'd1, 2'd2, 2'd0); settle;
        check_count(4, "repeat A:0026");
        check_bytes(16'h060E, "repeat A:0026 -> no writes");

        // 4. lane1-only change: lane1 VS1/PE0, lane0 unchanged
        send_adjust(2'd2, 2'd1, 2'd1, 2'd0); settle;
        check_count(8, "lane1-only");
        check_block(4, B1, 4'd9, 5'd0, "lane1-only block");
        check_bytes(16'h010E, "lane1-only VS1/PE0 -> lane1 base only");

        // 5. both change: lane0 VS1/PE0, lane1 VS2/PE2 -> lane0 block then lane1 block
        send_adjust(2'd1, 2'd0, 2'd2, 2'd2); settle;
        check_count(16, "both change");
        check_block(8,  B0, 4'd9,  5'd0,  "both: lane0 block");
        check_block(12, B1, 4'd13, 5'd10, "both: lane1 block");
        check_bytes(16'h1601, "both change (lane0 VS1/PE0, lane1 VS2/PE2) -> lane0 then lane1");

        // 6. per-lane clamp: lane0 VS3/PE0 -> VS2/PE0 (changed from VS1), lane1 VS3/PE3 -> VS2/PE3 (changed PE)
        send_adjust(2'd3, 2'd0, 2'd3, 2'd3); settle;
        check_count(24, "clamp both");
        check_block(16, B0, 4'd13, 5'd0,  "clamp: lane0 at VS2 ceiling");
        check_block(20, B1, 4'd13, 5'd13, "clamp: lane1 VS2 + PE3 ceiling");
        check_bytes(16'h3E06, "per-lane clamp -> lane0 0x06 (MAX_SWING), lane1 0x3E (both flags)");

        // 7. clamped request equal to applied -> nothing
        send_adjust(2'd3, 2'd0, 2'd3, 2'd3); settle;
        check_count(24, "clamped == applied");
        check_bytes(16'h3E06, "clamped request equal to applied -> no writes");

        // 8. training restart -> no INIT writes, declares INIT again
        training_active = 0; #(2_000); training_active = 1; settle;
        check_count(24, "next training start");
        check_bytes(16'h0606, "next training start -> re-baselined, no writes");
        if (dbg_a[5]) begin errors = errors + 1; $display("FAIL: DUT A seq_err"); end

        // ---- DUT B: APPLY_ON_TRAINING_START=1 wrote INIT to BOTH lanes, lane0 first ----
        // (training started twice => 16 writes; plus the same 24 change writes = 40)
        if (wc_b != 40) begin errors = errors + 1; $display("FAIL DUT B: %0d writes (want 40)", wc_b); end
        if (wa_b[0] !== B0 + 24'h034 || wv_b[0] !== {16'd0, 4'd13, 12'd0} ||
            wa_b[4] !== B1 + 24'h034 || wv_b[4] !== {16'd0, 4'd13, 12'd0} ||
            wa_b[3] !== B0 + 24'h0d8 || wv_b[3] !== 32'h110 || wa_b[7] !== B1 + 24'h0d8 || wv_b[7] !== 32'h110) begin
            errors = errors + 1; $display("FAIL DUT B INIT: w0 %06x/%08x w3 %06x/%08x w4 %06x/%08x w7 %06x/%08x",
                wa_b[0], wv_b[0], wa_b[3], wv_b[3], wa_b[4], wv_b[4], wa_b[7], wv_b[7]); end
        else $display("  ok: APPLY_ON_START=1 -> INIT to lane0 (0x8083xx) then lane1 (0x8084xx), %0d writes total", wc_b);
        if (byte_b !== 16'h0606) begin errors = errors + 1; $display("FAIL DUT B bytes %04x", byte_b); end
        if (dbg_b[5]) begin errors = errors + 1; $display("FAIL: DUT B seq_err"); end

        if (errors == 0) $display("PASS: per-lane AFE adjust — own request, own LANE_BASE writes, own TRAINING_LANEx_SET byte, per-lane clamp");
        else $display("FAIL: %0d error(s)", errors);
        $finish;
    end
endmodule
