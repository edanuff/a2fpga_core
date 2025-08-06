// Testbench for stream_serializer - test fixed timing packet transmission
`timescale 1ns / 1ps

module test_stream_serializer;

    // Clock and reset
    reg clk;
    reg rst_n;
    
    // DUT inputs
    reg wr_i;
    reg [31:0] data_i;
    
    // DUT outputs
    wire qclk;
    wire frame;
    wire [3:0] qdata;
    wire busy;
    
    // Test data
    reg [31:0] test_packets [0:3];
    integer packet_index;
    
    // DUT instantiation
    stream_serializer dut (
        .clk(clk),
        .rst_n(rst_n),
        .wr_i(wr_i),
        .data_i(data_i),
        .qclk(qclk),
        .frame(frame),
        .qdata(qdata),
        .busy(busy)
    );
    
    // Clock generation (54MHz)
    initial begin
        clk = 0;
        forever #9.26 clk = ~clk;  // 54MHz = 18.52ns period
    end
    
    
    // Safety timeout to prevent infinite simulation
    initial begin
        #10000;  // 10us timeout
        $display("ERROR: Simulation timeout after 10us");
        $finish;
    end
    
    // Test stimulus
    initial begin
        // VCD dump for waveform viewing
        $dumpfile("dump.vcd");
        $dumpvars(0, test_stream_serializer);
        $display("VCD file dump.vcd created");
        
        // Initialize
        rst_n = 0;
        wr_i = 0;
        data_i = 32'h0;
        packet_index = 0;
        
        // Test data patterns
        test_packets[0] = 32'hDEADBEEF;
        test_packets[1] = 32'hCAFEBABE;
        test_packets[2] = 32'h12345678;
        test_packets[3] = 32'hA5A5C3C3;
        
        // Release reset after 100ns
        #100;
        rst_n = 1;
        
        $display("=== Stream Serializer Test ===");
        $display("Time\t\tApple_Period\tPacket\t\t\tFrame\tQClk\tQData\tPhase");
        
        // Simple test: Send packets and observe transmission
        $display("\\n--- Basic Test: Fixed timing transmission ---");
        
        // Wait for a few clock cycles to settle
        repeat(10) @(posedge clk);
        
        // Test 1: Send first packet
        send_packet(test_packets[0]);
        $display("Sent packet: 0x%08x", test_packets[0]);
        wait_for_transmission_complete();
        
        // Wait between packets
        repeat(20) @(posedge clk);
        
        // Test 2: Send second packet
        send_packet(test_packets[1]);
        $display("Sent packet: 0x%08x", test_packets[1]);
        wait_for_transmission_complete();
        
        // Final wait
        repeat(10) @(posedge clk);
        
        $display("\\n=== Test Complete ===");
        $finish;
    end
    
    // Task to send a packet
    task send_packet(input [31:0] packet);
        begin
            @(posedge clk);
            wr_i = 1;
            data_i = packet;
            @(posedge clk);
            wr_i = 0;
        end
    endtask
    
    // Task to wait for transmission to complete
    task wait_for_transmission_complete;
        begin
            // Wait for frame to go high (start of transmission)
            @(posedge frame);
            $display("Frame start at time %0t", $time);
            
            // Wait for frame to go low (transmission active)
            @(negedge frame);
            
            // Wait for busy to go low (transmission complete)
            while (busy) begin
                @(posedge clk);
            end
            
            $display("Transmission complete at time %0t", $time);
        end
    endtask
    
    // Monitor key signals
    always @(posedge clk) begin
        if (frame || dut.ckl4_falling_w) begin
            $display("%0t: Frame=%b, QClk=%b, QData=0x%x, Busy=%b, Active=%b, Pending=%b, Nibble=%0d, Clk4=%b", 
                     $time, frame, qclk, qdata, busy, dut.packet_active_r, dut.packet_pending_r, 
                     dut.nibble_count_r, dut.clk4);
        end
    end
    
    // Check for obvious problems
    always @(posedge clk) begin
        // Check if nibble count is valid
        if (dut.nibble_count_r > 7) begin
            $error("ERROR: Invalid nibble count %0d at time %0t", dut.nibble_count_r, $time);
        end
        
        // Check for qclk only when packet is active
        if (qclk && !dut.packet_active_r) begin
            $error("ERROR: QClk active but packet not active at time %0t", $time);
        end
        
        // Check frame only high for first nibble
        if (frame && dut.nibble_count_r != 0) begin
            $error("ERROR: Frame high but nibble count is %0d at time %0t", dut.nibble_count_r, $time);
        end
    end

endmodule