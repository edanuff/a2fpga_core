// tb_aux_idle_bias.v
//
// Pre-fab verification for the 1.0a4 AUX bias network's idle-parking
// choice (board_1_0a4_requirements.md item 1b): the divider parks the
// pair with P below N, so the TLVDS receiver reads a constant logic 0
// whenever nothing drives the line.
//
// Checks, in order:
//   1. A long idle-0 line produces zero RX bytes and never asserts busy
//      (no chatter-class acceptance from the parked level).
//   2. After a request is transmitted, the un-driven wait window
//      (idle 0, busy=1, holdoff expired) still produces zero RX bytes.
//   3. A Manchester reply arriving over the idle-0 line is decoded:
//      sync detects, exactly the sent bytes arrive intact, busy drops.
//   4. Post-reply idle-0 produces no further bytes.
//
// Run: iverilog -g2012 -o /tmp/tb_aux_idle_bias.vvp \
//        hdl/displayport/sim/tb_aux_idle_bias.v \
//        hdl/displayport/auxch/aux_interface.v  && vvp /tmp/tb_aux_idle_bias.vvp

`timescale 1ns/1ps

module tb_aux_idle_bias;

    localparam HALF_BIT_NS = 500;      // 1 Mbps Manchester, 100 MHz clk

    reg         clk = 1'b0;
    reg         aux_in = 1'b0;         // the parked idle level under test
    wire        aux_out;
    wire        aux_tri;
    reg         tx_wr_en = 1'b0;
    reg  [7:0]  tx_data = 8'h00;
    wire        tx_full;
    reg         rx_rd_en = 1'b0;
    wire [7:0]  rx_data;
    wire        rx_empty;
    wire        busy;
    reg         abort = 1'b0;
    wire        timeout;
    wire [15:0] debug_pmod;

    integer     rx_byte_events = 0;
    integer     errors = 0;
    integer     i;

    aux_interface dut (
        .clk        (clk),
        .debug_pmod (debug_pmod),
        .aux_in     (aux_in),
        .aux_out    (aux_out),
        .aux_tri    (aux_tri),
        .tx_wr_en   (tx_wr_en),
        .tx_data    (tx_data),
        .tx_full    (tx_full),
        .rx_rd_en   (rx_rd_en),
        .rx_data    (rx_data),
        .rx_empty   (rx_empty),
        .busy       (busy),
        .abort      (abort),
        .timeout    (timeout)
    );

    always #5 clk = ~clk;

    // Count every byte the DUT commits to its RX FIFO.
    always @(posedge clk)
        if (dut.rx_wr_en) begin
            rx_byte_events = rx_byte_events + 1;
            $display("  [probe] DUT committed RX byte %02x (event %0d, wr_ptr %0d rd_ptr %0d)",
                     dut.rx_wr_data, rx_byte_events, dut.rx_wr_ptr, dut.rx_rd_ptr);
        end

    task send_half_bit(input value);
        begin
            aux_in = value;
            #(HALF_BIT_NS);
        end
    endtask

    task send_bit(input value);           // Manchester: bit b -> {b, ~b}
        begin
            send_half_bit(value);
            send_half_bit(~value);
        end
    endtask

    task send_raw8(input [7:0] halves);   // raw half-bit pattern, MSB first
        begin
            for (i = 7; i >= 0; i = i - 1)
                send_half_bit(halves[i]);
        end
    endtask

    task send_byte(input [7:0] b);
        begin
            for (i = 7; i >= 0; i = i - 1)
                send_bit(b[i]);
        end
    endtask

    task enqueue_tx(input [7:0] b);
        begin
            @(negedge clk);
            tx_data  = b;
            tx_wr_en = 1'b1;
            @(negedge clk);
            tx_wr_en = 1'b0;
        end
    endtask

    task read_rx_byte(input [7:0] expected);
        begin
            if (rx_empty) begin
                errors = errors + 1;
                $display("FAIL: expected RX byte %02x but FIFO empty", expected);
            end else begin
                @(negedge clk); rx_rd_en = 1'b1;
                @(negedge clk); rx_rd_en = 1'b0;
                @(negedge clk);
                if (rx_data !== expected) begin
                    errors = errors + 1;
                    $display("FAIL: RX byte %02x, expected %02x", rx_data, expected);
                end else
                    $display("  ok: RX byte %02x", rx_data);
            end
        end
    endtask

    initial begin
        // ---- Phase 1: long cold idle at parked 0 ------------------------
        aux_in = 1'b0;
        #(400_000);                       // 400 us of parked line
        if (rx_byte_events != 0 || busy !== 1'b0) begin
            errors = errors + 1;
            $display("FAIL: cold idle produced bytes=%0d busy=%b",
                     rx_byte_events, busy);
        end else
            $display("  ok: 400us cold idle-0, zero bytes, busy low");

        // ---- Phase 2: transmit a request, wait in idle ------------------
        enqueue_tx(8'h09);                // arbitrary 3-byte request
        enqueue_tx(8'h02);
        enqueue_tx(8'h04);
        // 3 request bytes + sync/start/stop overhead ~= 56 us on the wire;
        // wait 90 us so TX is done and the 5 us RX holdoff has expired.
        #(90_000);
        if (busy !== 1'b1) begin
            errors = errors + 1;
            $display("FAIL: busy not held while awaiting reply");
        end
        if (rx_byte_events != 0) begin
            errors = errors + 1;
            $display("FAIL: reply-wait idle produced %0d bytes", rx_byte_events);
        end else
            $display("  ok: reply-wait window idle-0, zero bytes, busy high");

        // ---- Phase 3: reply arrives over the idle-0 line ----------------
        begin : reply
            integer k;
            for (k = 0; k < 16; k = k + 1)
                send_bit(1'b0);           // sync preamble: 16 zero bits
        end
        send_raw8(8'b1111_0000);          // START
        send_byte(8'hE0);                 // reply command byte
        send_byte(8'h5A);                 // payload byte
        send_raw8(8'b1111_0000);          // STOP
        aux_in = 1'b0;                    // back to parked idle
        #(20_000);

        if (rx_byte_events != 2) begin
            errors = errors + 1;
            $display("FAIL: expected 2 RX bytes, got %0d", rx_byte_events);
        end
        read_rx_byte(8'hE0);
        read_rx_byte(8'h5A);
        if (busy !== 1'b0) begin
            errors = errors + 1;
            $display("FAIL: busy did not release after reply");
        end else
            $display("  ok: reply decoded from idle-0 line, busy released");

        // ---- Phase 4: post-reply idle ----------------------------------
        #(400_000);
        if (rx_byte_events != 2) begin
            errors = errors + 1;
            $display("FAIL: post-reply idle produced extra bytes (%0d total)",
                     rx_byte_events);
        end else
            $display("  ok: 400us post-reply idle-0, no further bytes");

        if (errors == 0)
            $display("PASS: idle-0 parking (P below N) verified — quiet idle, clean reply decode");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

endmodule
