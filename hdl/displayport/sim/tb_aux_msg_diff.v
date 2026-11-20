// tb_aux_msg_diff.v — cycle-exact differential check of dp_aux_messages
// against its pre-2026-10-24 table (the 12-bit `case (counter)` evaluated
// every cycle) after the message-engine change for the 138B clk100 ladder
// (byte table looked up once at load into a slot shift register, live
// bytes tagged). Random traffic: message numbers 0x00..0x3F (used and
// unused), msg_de pulses at random including while busy and in the load
// cycle, train_set_byte / irq_clear_byte changing every cycle. Every output
// of the two engines must match on every clock.
//
// Reference module: the old file with its module renamed —
//   git show 56eeef50:hdl/displayport/auxch/dp_aux_messages.v \
//     | sed 's/^module dp_aux_messages /module dp_aux_messages_ref /' > /tmp/dp_aux_messages_ref.v
// Run (repo root):
//   iverilog -g2012 -o /tmp/tb_msgdiff.vvp hdl/displayport/sim/tb_aux_msg_diff.v \
//        hdl/displayport/auxch/dp_aux_messages.v /tmp/dp_aux_messages_ref.v && vvp /tmp/tb_msgdiff.vvp
`timescale 1ns / 1ps
module tb_aux_msg_diff;
    reg clk = 1'b0;
    always #5 clk = ~clk;

    reg        msg_de = 1'b0;
    reg [7:0]  msg = 8'h00;
    reg [15:0] train_set_byte = 16'h0606;
    reg [7:0]  irq_clear_byte = 8'h00;

    wire       busy_n, we_n;  wire [7:0] data_n;
    wire       busy_r, we_r;  wire [7:0] data_r;

    dp_aux_messages     #(.LINK_RATE_MBPS(2700)) dut (.clk(clk), .msg_de(msg_de), .msg(msg), .busy(busy_n),
        .train_set_byte(train_set_byte), .irq_clear_byte(irq_clear_byte), .aux_tx_wr_en(we_n), .aux_tx_data(data_n));
    dp_aux_messages_ref #(.LINK_RATE_MBPS(2700)) ref0 (.clk(clk), .msg_de(msg_de), .msg(msg), .busy(busy_r),
        .train_set_byte(train_set_byte), .irq_clear_byte(irq_clear_byte), .aux_tx_wr_en(we_r), .aux_tx_data(data_r));

    integer cyc = 0, errors = 0, bytes = 0, loads = 0, live = 0;
    integer seed = 32'h5EED_0001;
    integer r;
    always @(negedge clk) begin
        // stimulus for the coming edge
        r = $random(seed);
        // ~1 in 6 cycles a request; messages 0x00..0x3F
        msg_de <= (r[2:0] == 3'd0);
        msg    <= {2'b00, r[8:3]};
        train_set_byte <= $random(seed);
        irq_clear_byte <= $random(seed);
    end
    always @(posedge clk) begin
        cyc = cyc + 1;
        if (we_r) bytes = bytes + 1;
        if (msg_de && dut.counter[3:0] == 4'hF && msg != 8'h00) loads = loads + 1;
        if (we_r && (data_r !== data_n)) live = live;    // placeholder for readability
        if ({busy_n, we_n, data_n} !== {busy_r, we_r, data_r}) begin
            errors = errors + 1;
            if (errors <= 10)
                $display("DIFF at cycle %0d: new busy=%b we=%b data=%02x  ref busy=%b we=%b data=%02x  (ref counter=%03x)",
                         cyc, busy_n, we_n, data_n, busy_r, we_r, data_r, ref0.counter);
        end
        if (cyc == 3_000_000) begin
            $display("%0d cycles, %0d message loads, %0d bytes emitted, %0d mismatches",
                     cyc, loads, bytes, errors);
            if (errors == 0 && loads > 10000 && bytes > 40000) $display("PASS: dp_aux_messages cycle-exact vs reference");
            else $display("FAIL");
            $finish;
        end
    end
endmodule
