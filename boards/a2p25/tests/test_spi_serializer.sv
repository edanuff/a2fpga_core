// Testbench for SPI serializer (1-bit serial output)
`timescale 1ps/1ps

module test_spi_serializer;
    parameter CLK_PERIOD = 18.5; // 54MHz clock period in ps
    
    // Test signals
    reg clk = 0;
    reg rst_n = 0;
    reg wr = 0;
    reg [31:0] data = 0;
    
    wire spi_clk, spi_cs, spi_mosi;
    
    // Instantiate DUT with slower clock for easier analysis
    qspi_to_spi_serializer #(.COUNT_WIDTH(4)) dut (
        .clk_i(clk),
        .rst_n(rst_n),
        .wr_i(wr),
        .data_i(data),
        .spi_clk(spi_clk),
        .spi_cs(spi_cs),
        .spi_mosi(spi_mosi)
    );
    
    // Clock generation
    always #(CLK_PERIOD/2) clk = ~clk;
    
    // Edge detection for analysis
    reg spi_clk_prev = 0, spi_cs_prev = 1;
    wire spi_clk_rising = spi_clk && !spi_clk_prev;
    wire spi_clk_falling = !spi_clk && spi_clk_prev;
    wire cs_falling = !spi_cs && spi_cs_prev;
    wire cs_rising = spi_cs && !spi_cs_prev;
    
    always @(posedge clk) begin
        spi_clk_prev <= spi_clk;
        spi_cs_prev <= spi_cs;
    end
    
    // Track received data
    reg [31:0] captured_data = 0;
    integer bit_count = 0;
    integer transaction_count = 0;
    
    // Capture data on SPI clock rising edges (standard SPI Mode 0)
    always @(posedge clk) begin
        if (cs_falling) begin
            $display("Time %0t: SPI Transaction %0d START", $time, transaction_count);
            captured_data = 0;
            bit_count = 0;
        end
        
        if (!spi_cs && spi_clk_rising) begin
            captured_data = {captured_data[30:0], spi_mosi};
            bit_count = bit_count + 1;
            $display("Time %0t: Bit %0d: %b (Data so far: %08X)", 
                    $time, bit_count-1, spi_mosi, captured_data);
        end
        
        if (cs_rising && bit_count > 0) begin
            $display("Time %0t: SPI Transaction %0d COMPLETE - Received %0d bits: 0x%08X", 
                    $time, transaction_count, bit_count, captured_data);
            transaction_count = transaction_count + 1;
            
            // Verify against expected data
            if (captured_data == data && bit_count == 32) begin
                $display("✓ PASS: Data matches expected 0x%08X", data);
            end else begin
                $display("✗ FAIL: Expected 0x%08X, got 0x%08X (%0d bits)", 
                        data, captured_data, bit_count);
            end
            $display("");
        end
    end
    
    // Test task
    task send_data(input [31:0] test_data);
        begin
            data = test_data;
            $display("=== Sending 0x%08X ===", test_data);
            @(posedge clk);
            wr = 1;
            @(posedge clk);
            wr = 0;
            
            // Wait for transaction to complete
            wait(cs_rising);
            repeat(10) @(posedge clk);
        end
    endtask
    
    // Measure timing
    real spi_clk_freq;
    real spi_clk_period;
    reg [63:0] last_spi_clk_rising = 0;
    reg [63:0] current_spi_clk_rising = 0;
    
    always @(posedge clk) begin
        if (spi_clk_rising) begin
            current_spi_clk_rising = $time;
            if (last_spi_clk_rising != 0) begin
                spi_clk_period = current_spi_clk_rising - last_spi_clk_rising;
                spi_clk_freq = 1.0e12 / spi_clk_period; // Hz (period in ps)
                $display("SPI Clock: %.1f Hz (period: %.0f ps)", spi_clk_freq, spi_clk_period);
            end
            last_spi_clk_rising = current_spi_clk_rising;
        end
    end
    
    initial begin
        $dumpfile("spi_serializer.vcd");
        $dumpvars(0, test_spi_serializer);
        
        $display("=== SPI Serializer Test ===");
        $display("Clock Period: %.1f ps", CLK_PERIOD);
        
        // Reset
        #(10 * CLK_PERIOD);
        rst_n = 1;
        #(10 * CLK_PERIOD);
        
        // Test different patterns
        send_data(32'hABCDEF01);
        send_data(32'h12345678);  
        send_data(32'hFFFFFFFF);
        send_data(32'h00000000);
        send_data(32'hA5A5A5A5);
        
        // Test rapid back-to-back transactions
        $display("=== Testing Back-to-Back Transactions ===");
        send_data(32'h11111111);
        send_data(32'h22222222);
        
        #(100 * CLK_PERIOD);
        
        $display("=== Test Complete ===");
        $display("Total transactions: %0d", transaction_count);
        $finish;
    end
    
endmodule