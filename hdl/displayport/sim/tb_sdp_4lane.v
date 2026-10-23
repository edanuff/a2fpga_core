// tb_sdp_4lane.v : sdp_engine wire-mapping self-check for 1/2/4 lanes.
//
// Captures the bytes each lane carries between SS and SE, undoes the
// nibble interleaving (pairs 0<->1, 2<->3; 1-lane: consecutive codewords),
// and compares against the DP 1.1a packet-to-lane mapping built here
// from the header/payload the engine was fed (RS(15,13) parity recomputed
// independently in the TB).
//
//   iverilog -g2012 -DLANES=4 -o /tmp/tb_sdp4.vvp hdl/displayport/sim/tb_sdp_4lane.v hdl/displayport/audio/sdp_engine.v && vvp /tmp/tb_sdp4.vvp
`timescale 1ns/1ps
module tb_sdp_4lane;
`ifndef LANES
`define LANES 4
`endif
    localparam L = `LANES;
    localparam WB = (L == 1) ? 48 : (L == 4) ? 12 : 24;
    reg clk = 0; always #5 clk = ~clk;
    reg reset = 1;
    reg [72:0] in_data = 73'h0;
    reg sdp_gap = 0, frame_pulse = 0;
    wire [72:0] out_data;
    reg [4*2*16-1:0] buffer = 0;
    reg buffer_ready = 0;
    wire buffer_take;
    reg [23:0] maud = 24'h0123AB;
    wire audio_mute;

    sdp_engine #(.LANE_COUNT(L), .AUDIO_BIT_WIDTH(16), .AUDIO_RATE(48000)) dut (
        .clk(clk), .reset(reset), .in_data(in_data), .sdp_gap(sdp_gap),
        .frame_pulse(frame_pulse), .out_data(out_data), .buffer(buffer),
        .buffer_ready(buffer_ready), .buffer_take(buffer_take), .maud(maud),
        .audio_mute(audio_mute));

    // ---- independent RS(15,13) parity (spec 2.2.6, same algebra) ----
    function [3:0] g1m(input [3:0] c); g1m = {c[3]^c[2], c[2]^c[1], c[3]^c[1]^c[0], c[3]^c[0]}; endfunction
    function [3:0] g0m(input [3:0] c); g0m = {c[2], c[1], c[3]^c[0], c[3]}; endfunction
    function [7:0] lstep(input [7:0] x, input [3:0] n); reg [3:0] fb; begin fb = n ^ x[7:4]; lstep = {x[3:0] ^ g1m(fb), g0m(fb)}; end endfunction
    function [7:0] par1(input [7:0] b); reg [7:0] st; begin st = lstep(8'h00, b[3:0]); st = lstep(st, b[7:4]); par1 = {st[3:0], st[7:4]}; end endfunction
    function [7:0] par4(input [31:0] g); reg [7:0] st; integer i; begin st = 0; for (i = 0; i < 4; i = i + 1) begin st = lstep(st, g[8*i +: 4]); st = lstep(st, g[8*i+4 +: 4]); end par4 = {st[3:0], st[7:4]}; end endfunction

    // expected per-lane pre-interleave byte sequence
    reg [7:0] exp_b [0:3][0:47];
    task build_expected(input [31:0] hb, input [255:0] d);
        integer n, g, k, j;
        reg [7:0] pbh [0:3]; reg [7:0] pbd [0:7];
        begin
            for (n = 0; n < 4; n = n + 1) pbh[n] = par1(hb[8*n +: 8]);
            for (g = 0; g < 8; g = g + 1) pbd[g] = par4(d[32*g +: 32]);
            if (L == 4) begin
                for (n = 0; n < 4; n = n + 1) begin
                    exp_b[n][0] = hb[8*n +: 8]; exp_b[n][1] = pbh[n];
                    for (j = 0; j < 4; j = j + 1) exp_b[n][2+j] = d[32*n + 8*j +: 8];
                    exp_b[n][6] = pbd[n];
                    for (j = 0; j < 4; j = j + 1) exp_b[n][7+j] = d[128 + 32*n + 8*j +: 8];
                    exp_b[n][11] = pbd[4+n];
                end
            end else if (L == 2) begin
                for (n = 0; n < 2; n = n + 1) begin
                    exp_b[n][0] = hb[8*n +: 8];     exp_b[n][1] = pbh[n];
                    exp_b[n][2] = hb[8*(n+2) +: 8]; exp_b[n][3] = pbh[n+2];
                    for (g = 0; g < 4; g = g + 1) begin
                        for (j = 0; j < 4; j = j + 1) exp_b[n][4+5*g+j] = d[32*(2*g+n) + 8*j +: 8];
                        exp_b[n][4+5*g+4] = pbd[2*g+n];
                    end
                end
            end else begin
                for (n = 0; n < 4; n = n + 1) begin exp_b[0][2*n] = hb[8*n +: 8]; exp_b[0][2*n+1] = pbh[n]; end
                for (g = 0; g < 8; g = g + 1) begin
                    for (j = 0; j < 4; j = j + 1) exp_b[0][8+5*g+j] = d[32*g + 8*j +: 8];
                    exp_b[0][8+5*g+4] = pbd[g];
                end
            end
        end
    endtask

    // capture
    reg [7:0] got [0:3][0:47];
    integer cnt [0:3];
    reg capturing = 0;
    integer errors = 0, packets = 0;
    wire [8:0] sym [0:7];
    assign sym[0] = out_data[8:0];   assign sym[1] = out_data[17:9];
    assign sym[2] = out_data[26:18]; assign sym[3] = out_data[35:27];
    assign sym[4] = out_data[44:36]; assign sym[5] = out_data[53:45];
    assign sym[6] = out_data[62:54]; assign sym[7] = out_data[71:63];
    localparam [8:0] SS = 9'b101011100, SE = 9'b111111101;
    integer ln, sl, k, m;
    reg [7:0] a, b, ua, ub;
    reg [31:0] hb_sent; reg [255:0] db_sent;
    always @(posedge clk) begin
        for (ln = 0; ln < L; ln = ln + 1) begin
            for (sl = 0; sl < 2; sl = sl + 1) begin
                if (sym[2*ln+sl] == SS) begin cnt[ln] = 0; if (ln == 0) begin capturing = 1; $display("t=%0t SS (hb=%08x)", $time, dut.hbv); end end
                else if (sym[2*ln+sl] == SE) begin
                    if (ln == L-1 && capturing) begin
                        $display("t=%0t SE", $time);
                        // check this packet
                        packets = packets + 1;
                        build_expected(hb_sent, db_sent);
                        for (m = 0; m < L; m = m + 1) if (cnt[m] != WB) begin errors = errors + 1; $display("lane %0d: %0d bytes, expected %0d", m, cnt[m], WB); end
                        if (L == 1) begin
                            // consecutive codewords: bytes 2i,2i+1 swap high nibbles
                            for (k = 0; k < WB; k = k + 2) begin
                                a = got[0][k]; b = got[0][k+1];
                                ua = {b[7:4], a[3:0]}; ub = {a[7:4], b[3:0]};
                                // engine's 1-lane table: byte k carries {hi of k+1?}; compare both orders
                                if (!((ua == exp_b[0][k] && ub == exp_b[0][k+1]) || (a == exp_b[0][k] && b == exp_b[0][k+1]))) begin
                                    errors = errors + 1; $display("1-lane mismatch at %0d: got %02x %02x exp %02x %02x", k, a, b, exp_b[0][k], exp_b[0][k+1]);
                                end
                            end
                        end else begin
                            for (m = 0; m < L; m = m + 2) for (k = 0; k < WB; k = k + 1) begin
                                a = got[m][k]; b = got[m+1][k];
                                ua = {b[7:4], a[3:0]}; ub = {a[7:4], b[3:0]};
                                if (ua !== exp_b[m][k] || ub !== exp_b[m+1][k]) begin
                                    errors = errors + 1;
                                    $display("lane pair %0d/%0d byte %0d: got %02x/%02x -> %02x/%02x, exp %02x/%02x", m, m+1, k, a, b, ua, ub, exp_b[m][k], exp_b[m+1][k]);
                                end
                            end
                        end
                        capturing = 0;
                    end
                end else if (capturing && cnt[ln] < 48) begin
                    got[ln][cnt[ln]] = sym[2*ln+sl][7:0]; cnt[ln] = cnt[ln] + 1;
                end
            end
        end
    end

    // remember what the engine loaded (snoop its registers at load)
    always @(posedge clk) if (dut.pkt_pending && !dut.inserting) begin hb_sent = dut.hbv; db_sent = dut.db; end

    integer i;
    initial begin
        for (i = 0; i < 8; i = i + 1) buffer[16*i +: 16] = 16'h1234 + 16'h1111*i;
        // all stimulus on negedge: no race with the DUT's posedge sampling
        repeat (4) @(negedge clk); reset = 0;
        repeat (4) @(negedge clk);
        // packet 1: Audio_TimeStamp (frame_pulse), 2: Audio_Stream, 3: InfoFrame
        frame_pulse = 1; @(negedge clk); frame_pulse = 0;
        repeat (8) @(negedge clk); sdp_gap = 1; repeat (40) @(negedge clk); sdp_gap = 0;
        buffer_ready = 1; @(negedge clk); while (!buffer_take) @(negedge clk); buffer_ready = 0;
        repeat (8) @(negedge clk); sdp_gap = 1; repeat (40) @(negedge clk); sdp_gap = 0;
        repeat (8) @(negedge clk); sdp_gap = 1; repeat (40) @(negedge clk); sdp_gap = 0;
        repeat (10) @(negedge clk);
        if (packets < 3) begin errors = errors + 1; $display("only %0d packets seen", packets); end
        $display("%s: LANES=%0d packets=%0d errors=%0d", errors ? "FAIL" : "PASS", L, packets, errors);
        $finish;
    end
endmodule
