// tb_sr_highhalf.v — verify scrambler-reset path for BS in the HIGH symbol
// (odd BS_POS case, e.g. 1080p 2-lane HBR BS_POS=3549).
//
// Drives the inserter -> scrambler chain the way main_stream does:
// BS (K28.5) appears in the high half of all four 18-bit lane slots,
// data words (00 payload) between. Checks:
//   1. exactly one SR (K28.0) per 512 BS events, high half, ALL lanes
//   2. the scrambler LFSR resets when the SR passes through: the first
//      scrambled 00-data byte after SR must be FF (then 17, C0 per the
//      PCIe/DP LFSR reference table quoted in scrambler_all_channels.v)
`timescale 1ns / 1ps
module tb_sr_highhalf;

    reg clk = 0;
    always #5 clk = ~clk;

    localparam [8:0] BS = 9'b110111100;  // K28.5
    localparam [8:0] SR = 9'b100011100;  // K28.0

    reg  [71:0] ins_in = 72'b0;
    wire [71:0] ins_out;
    wire [71:0] scr_out;

    scrambler_reset_inserter i_ins (.clk(clk), .in_data(ins_in), .out_data(ins_out));
    scrambler_all_channels   i_scr (.clk(clk), .bypass0(1'b0), .bypass1(1'b0),
                                    .in_data(ins_out), .out_data(scr_out));

    // data word: all-zero data symbols (control bit 0) in both halves
    wire [71:0] DATA_WORD = 72'b0;
    // BS word: BS in HIGH half of each lane, zero data in low half
    wire [71:0] BS_WORD = {BS, 9'b0, BS, 9'b0, BS, 9'b0, BS, 9'b0};

    integer bs_events = 0;
    integer sr_events = 0;
    integer errors = 0;
    integer i, k;
    reg [71:0] ins_out_q;
    reg        saw_sr_last = 0;

    // monitor inserter output
    always @(posedge clk) begin
        ins_out_q <= ins_out;
        if (ins_out[17:9] == SR) begin
            sr_events = sr_events + 1;
            // all four lanes must carry SR in the high half
            if (ins_out[35:27] != SR || ins_out[53:45] != SR || ins_out[71:63] != SR) begin
                $display("FAIL: SR not on all lanes at event %0d", sr_events);
                errors = errors + 1;
            end
            if (ins_out[8:0] == SR) begin
                $display("FAIL: unexpected SR in low half");
                errors = errors + 1;
            end
            saw_sr_last <= 1;
        end else
            saw_sr_last <= 0;
    end

    // scrambler reset check: first data word AFTER the SR word must
    // scramble 00 -> FF (low symbol of lane 0, fresh LFSR)
    reg check_next = 0;
    always @(posedge clk) begin
        check_next <= saw_sr_last;
        if (check_next) begin
            if (scr_out[7:0] !== 8'hFF) begin
                $display("FAIL: post-SR scramble of 00 = %02x (want FF) — LFSR did not reset", scr_out[7:0]);
                errors = errors + 1;
            end else
                $display("PASS: LFSR reset confirmed after SR (00 -> FF)");
        end
    end

    initial begin
        // 1200 BS events with data words between (2 data words per BS)
        for (i = 0; i < 1200; i = i + 1) begin
            @(negedge clk); ins_in = BS_WORD; bs_events = bs_events + 1;
            @(negedge clk); ins_in = DATA_WORD;
            @(negedge clk); ins_in = DATA_WORD;
        end
        @(negedge clk); ins_in = DATA_WORD;
        repeat (4) @(posedge clk);

        $display("bs_events=%0d sr_events=%0d errors=%0d", bs_events, sr_events, errors);
        // 1200 BS -> substitution on events 513 and 1025 => 2 SRs
        if (sr_events == 2 && errors == 0)
            $display("ALL CHECKS PASS");
        else
            $display("TEST FAILED");
        $finish;
    end
endmodule
