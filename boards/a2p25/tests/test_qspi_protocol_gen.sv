// Testbench for QSPI Protocol Generator
// Tests the QSPI timing and protocol implementation

`timescale 1ns / 1ps

module test_qspi_protocol_gen();

    // Test signals
    reg clk = 0;
    reg rst_n = 0;
    wire qspi_cs;
    wire qspi_clk;
    wire [3:0] qspi_data;
    
    // Clock generation - 54MHz
    parameter CLK_PERIOD = 18.5; // ns (54MHz = 18.5ns period)
    always #(CLK_PERIOD/2) clk = ~clk;
    
    // Instantiate DUT
    qspi_protocol_gen #(
        .CLOCK_SPEED_HZ(54_000_000)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .qspi_cs(qspi_cs),
        .qspi_clk(qspi_clk),
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
            nibble_count = 0;
        end
        
        // Capture data on QSPI clock edges (try both edges to see which works)
        if (!qspi_cs && qspi_clk_rising) begin
            captured_nibbles[nibble_count] = qspi_data;
            $display("  Clock %0d: Data=0x%X (CS=%b, CLK=%b) at %0t", 
                    nibble_count, qspi_data, qspi_cs, qspi_clk, $time);
            nibble_count = nibble_count + 1;
        end
        
        // Detect end of transaction
        if (cs_rising) begin
            // Reconstruct 32-bit packet from captured nibbles
            if (nibble_count >= 8) begin
                captured_packet = {
                    captured_nibbles[0], captured_nibbles[1], 
                    captured_nibbles[2], captured_nibbles[3],
                    captured_nibbles[4], captured_nibbles[5], 
                    captured_nibbles[6], captured_nibbles[7]
                };
                
                $display("=== TRANSACTION %0d COMPLETE ===", transaction_count + 1);
                $display("  Nibbles captured: %0d", nibble_count);
                $display("  Raw sequence: %X%X%X%X%X%X%X%X", 
                        captured_nibbles[0], captured_nibbles[1], captured_nibbles[2], captured_nibbles[3],
                        captured_nibbles[4], captured_nibbles[5], captured_nibbles[6], captured_nibbles[7]);
                $display("  Reconstructed packet: 0x%08X", captured_packet);
                
                // Check expected pattern
                if (captured_packet == 32'hAABBCCDD) begin
                    $display("  ✓ CORRECT PATTERN: 0xAABBCCDD");
                end else begin
                    $display("  ✗ PATTERN ERROR: Expected 0xAABBCCDD, got 0x%08X", captured_packet);
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
    
    // Test sequence
    initial begin
        $display("Starting QSPI Protocol Generator Test");
        $display("Expected behavior:");
        $display("  - CS starts HIGH, goes LOW during transaction");
        $display("  - SCLK should toggle at ~844kHz during transaction");
        $display("  - 8 nibbles per transaction: A-A-B-B-C-C-D-D");
        $display("  - 32-bit packet: 0xAABBCCDD");
        $display("");
        
        // Reset sequence
        rst_n = 0;
        #(CLK_PERIOD * 10);
        rst_n = 1;
        $display("Reset released at time %0t", $time);
        $display("");
        
        // Wait for multiple transactions
        #(CLK_PERIOD * 50000); // Wait for several transactions
        
        // Report QSPI clock frequency if measured
        if (qspi_clk_period > 0) begin
            $display("QSPI Clock Analysis:");
            $display("  Period: %.2f ns", qspi_clk_period);
            $display("  Frequency: %.0f Hz (%.2f kHz)", qspi_clk_freq, qspi_clk_freq/1000.0);
            $display("  Expected: ~844 kHz");
        end
        
        $display("");
        $display("Test completed - captured %0d transactions", transaction_count);
        $finish;
    end
    
    // Generate VCD file for waveform analysis
    initial begin
        $dumpfile("qspi_protocol_gen.vcd");
        $dumpvars(0, test_qspi_protocol_gen);
    end
    
    // Safety timeout
    initial begin
        #(CLK_PERIOD * 100000); // 100k clock cycles max
        $display("ERROR: Simulation timeout!");
        $finish;
    end

endmodule