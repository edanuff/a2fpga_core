// Testbench for QSPI Serializer
// Tests the write-triggered QSPI implementation

`timescale 1ns / 1ps

module test_qspi_serializer();

    // Test signals
    reg clk = 0;
    reg rst_n = 0;
    reg wr = 0;
    reg [31:0] data = 32'h0;
    wire qspi_clk;
    wire qspi_cs;
    wire [3:0] qspi_data;
    
    // Clock generation - 54MHz
    parameter CLK_PERIOD = 18.5; // ns (54MHz = 18.5ns period)
    always #(CLK_PERIOD/2) clk = ~clk;
    
    // Instantiate DUT (using fixed version)
    qspi_serializer_fixed dut (
        .clk_i(clk),
        .rst_n(rst_n),
        .wr_i(wr),
        .data_i(data),
        .qspi_clk(qspi_clk),
        .qspi_cs(qspi_cs),
        .qspi_data(qspi_data)
    );
    
    // Transaction capture variables
    reg [3:0] captured_nibbles [0:15];
    reg [31:0] captured_packet;
    integer nibble_count = 0;
    integer transaction_count = 0;
    
    // CS edge detection for transaction boundaries
    reg qspi_cs_prev = 1;
    wire cs_falling = qspi_cs_prev && !qspi_cs;  // Start of transaction
    wire cs_rising = !qspi_cs_prev && qspi_cs;   // End of transaction
    
    always @(posedge clk) begin
        qspi_cs_prev <= qspi_cs;
    end
    
    // QSPI clock edge detection for data capture
    reg qspi_clk_prev = 0;
    wire qspi_clk_rising = !qspi_clk_prev && qspi_clk;
    wire qspi_clk_falling = qspi_clk_prev && !qspi_clk;
    
    always @(posedge clk) begin
        qspi_clk_prev <= qspi_clk;
    end
    
    // Transaction monitoring
    always @(posedge clk) begin
        // Detect start of transaction
        if (cs_falling) begin
            $display("=== TRANSACTION %0d STARTED at time %0t ===", transaction_count + 1, $time);
            $display("  Expected data: 0x%08X", data);
            nibble_count = 0;
        end
        
        // Capture data on QSPI clock rising edges (standard SPI timing)
        if (!qspi_cs && qspi_clk_rising) begin
            captured_nibbles[nibble_count] = qspi_data;
            $display("  Clock %0d: Data=0x%X (CS=%b, CLK=%b) at %0t", 
                    nibble_count, qspi_data, qspi_cs, qspi_clk, $time);
            nibble_count = nibble_count + 1;
        end
        
        // Detect end of transaction
        if (cs_rising) begin
            // Reconstruct 32-bit packet from captured nibbles (LSB first due to shift)
            if (nibble_count >= 8) begin
                captured_packet = {
                    captured_nibbles[7], captured_nibbles[6], 
                    captured_nibbles[5], captured_nibbles[4],
                    captured_nibbles[3], captured_nibbles[2], 
                    captured_nibbles[1], captured_nibbles[0]
                };
                
                $display("=== TRANSACTION %0d COMPLETE ===", transaction_count + 1);
                $display("  Nibbles captured: %0d", nibble_count);
                $display("  Raw sequence: %X%X%X%X%X%X%X%X", 
                        captured_nibbles[0], captured_nibbles[1], captured_nibbles[2], captured_nibbles[3],
                        captured_nibbles[4], captured_nibbles[5], captured_nibbles[6], captured_nibbles[7]);
                $display("  Reconstructed packet: 0x%08X", captured_packet);
                $display("  Expected packet:      0x%08X", data);
                
                // Check expected pattern
                if (captured_packet == data) begin
                    $display("  ✓ CORRECT PATTERN MATCH");
                end else begin
                    $display("  ✗ PATTERN ERROR: Expected 0x%08X, got 0x%08X", data, captured_packet);
                end
            end else begin
                $display("=== TRANSACTION %0d INCOMPLETE ===", transaction_count + 1);
                $display("  Only %0d nibbles captured (expected 8)", nibble_count);
            end
            
            transaction_count = transaction_count + 1;
            $display("");
        end
    end
    
    // Clock frequency measurement
    real qspi_clk_period = 0;
    real qspi_clk_freq = 0;
    time last_qspi_clk_rising = 0;
    time current_qspi_clk_rising = 0;
    
    always @(posedge qspi_clk) begin
        current_qspi_clk_rising = $time;
        if (last_qspi_clk_rising != 0) begin
            qspi_clk_period = current_qspi_clk_rising - last_qspi_clk_rising;
            qspi_clk_freq = 1000000000.0 / qspi_clk_period; // Convert to Hz
        end
        last_qspi_clk_rising = current_qspi_clk_rising;
    end
    
    // Write pulse generator
    task send_data(input [31:0] test_data);
        begin
            data = test_data;
            @(posedge clk);
            wr = 1;
            @(posedge clk);
            wr = 0;
            $display("Sent write command with data: 0x%08X at time %0t", test_data, $time);
        end
    endtask
    
    // Test sequence
    initial begin
        $display("Starting QSPI Serializer Test");
        $display("Testing write-triggered QSPI implementation");
        $display("Expected behavior:");
        $display("  - Write pulse triggers transaction");
        $display("  - CS goes LOW during transaction");
        $display("  - Data sent LSB nibble first");
        $display("  - 8 nibbles per 32-bit word");
        $display("");
        
        // Reset sequence
        rst_n = 0;
        wr = 0;
        data = 32'h0;
        #(CLK_PERIOD * 10);
        rst_n = 1;
        $display("Reset released at time %0t", $time);
        $display("");
        
        // Wait a bit after reset
        #(CLK_PERIOD * 20);
        
        // Test 1: Send 0xAABBCCDD
        $display("=== TEST 1: Sending 0xAABBCCDD ===");
        send_data(32'hAABBCCDD);
        #(CLK_PERIOD * 100); // Wait for transaction to complete
        
        // Test 2: Send 0x12345678
        $display("=== TEST 2: Sending 0x12345678 ===");
        send_data(32'h12345678);
        #(CLK_PERIOD * 100);
        
        // Test 3: Send 0xDEADBEEF
        $display("=== TEST 3: Sending 0xDEADBEEF ===");
        send_data(32'hDEADBEEF);
        #(CLK_PERIOD * 100);
        
        // Test 4: Back-to-back transactions
        $display("=== TEST 4: Back-to-back transactions ===");
        send_data(32'h00FF00FF);
        #(CLK_PERIOD * 10); // Short delay
        send_data(32'hFF00FF00);
        #(CLK_PERIOD * 200);
        
        // Test 5: Rapid writes (queue test)
        $display("=== TEST 5: Rapid write sequence ===");
        send_data(32'h11111111);
        #(CLK_PERIOD * 5);
        send_data(32'h22222222);
        #(CLK_PERIOD * 5);
        send_data(32'h33333333);
        #(CLK_PERIOD * 300); // Wait for all to complete
        
        // Report QSPI clock frequency if measured
        if (qspi_clk_period > 0) begin
            $display("QSPI Clock Analysis:");
            $display("  Period: %.2f ns", qspi_clk_period);
            $display("  Frequency: %.0f Hz (%.2f MHz)", qspi_clk_freq, qspi_clk_freq/1000000.0);
            $display("  Base clock: 54 MHz, Divider: /4 = 13.5 MHz expected");
        end
        
        $display("");
        $display("Test completed - sent %0d transactions", transaction_count);
        $finish;
    end
    
    // Generate VCD file for waveform analysis
    initial begin
        $dumpfile("qspi_serializer.vcd");
        $dumpvars(0, test_qspi_serializer);
    end
    
    // Safety timeout
    initial begin
        #(CLK_PERIOD * 2000); // 2000 clock cycles max
        $display("ERROR: Simulation timeout!");
        $finish;
    end

endmodule