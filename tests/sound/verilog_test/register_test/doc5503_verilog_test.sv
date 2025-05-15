// DOC5503 standalone test in Verilog for use with Verilator
// This test is designed to be compiled directly without requiring C++ code
`timescale 1ns / 1ps

module doc5503_verilog_test;
  // Testbench signals
  reg clk;
  reg reset_n;
  wire clk_en;
  reg cs_n;
  reg we_n;
  reg [7:0] addr;
  reg [7:0] data_in;
  wire [7:0] data_out;
  wire [15:0] wave_address;
  wire wave_rd;
  reg wave_data_ready;
  reg [7:0] wave_data;
  wire signed [15:0] mono_mix;
  wire signed [15:0] left_mix;
  wire signed [15:0] right_mix;
  wire ready;
  wire [7:0] osc_en;
  
  // For debug purposes
  reg [31:0] cycle_count;
  
  // Instantiate the DOC5503 module
  doc5503 #(
    .CLOCK_SPEED_HZ(54_000_000),
    .DOC_CLOCK_SPEED_HZ(7_159_090)
  ) dut (
    .clk_i(clk),
    .reset_n_i(reset_n),
    .clk_en_i(clk_en),
    .cs_n_i(cs_n),
    .we_n_i(we_n),
    .addr_i(addr),
    .data_i(data_in),
    .data_o(data_out),
    .wave_address_o(wave_address),
    .wave_rd_o(wave_rd),
    .wave_data_ready_i(wave_data_ready),
    .wave_data_i(wave_data),
    .mono_mix_o(mono_mix),
    .left_mix_o(left_mix),
    .right_mix_o(right_mix),
    .channel_o(),  // Not connected in test
    .ready_o(ready),
    .osc_en_o(osc_en)
  );
  
  // Clock generation (50MHz)
  initial begin
    clk = 0;
    forever #10 clk = ~clk; // 20ns period = 50MHz
  end

  always @(posedge clk) begin
    if (!reset_n) begin
      cycle_count <= 0;
    end else begin
      cycle_count <= cycle_count + 1;
    end
  end  

  assign clk_en = (cycle_count % 8 == 0);
  
  // Test sequence
  initial begin
    // For Verilator, add these to enable waveform dump
    if ($test$plusargs("trace")) begin
      $dumpfile("doc5503_verilog_test.vcd");
      $dumpvars(0, doc5503_verilog_test);
    end
    
    // Print header
    $display("=== DOC5503 Verilog Test ===");
    $display("Testing register access with detailed reporting");
    
    // Initialize signals
    reset_n = 0;
    cs_n = 1;
    we_n = 1;
    addr = 0;
    data_in = 0;
    wave_data_ready = 0;
    wave_data = 0;
    cycle_count = 0;
    
    // Reset sequence
    $display("Resetting DOC5503...");
    repeat (10) @(posedge clk);
    
    // Release reset
    reset_n = 1;
    $display("Reset released");
    
    // Wait for ready signal with clk_en pulses
    $display("Waiting for DOC5503 to be ready...");
    while (!ready) begin
      @(posedge clk);
    end
    
    $display("DOC5503 is ready after %0d cycles", cycle_count);
    
    // Test 1: Read default oscillator enable register (0xE1)
    $display("\nTest 1: Reading default oscillator enable register (0xE1)");
    read_register(8'hE1);
    
    // Test 2: Write oscillator enable register (0xE1) with value 0x02
    $display("\nTest 2: Writing oscillator enable register (0xE1) = 0x02");
    write_register(8'hE1, 8'h02);
    
    // Test 3: Read back oscillator enable register (0xE1)
    $display("\nTest 3: Reading back oscillator enable register (0xE1)");
    read_register(8'hE1);
    
    // Test 4: Write oscillator enable register (0xE1) with value 0x04
    $display("\nTest 4: Writing oscillator enable register (0xE1) = 0x04");
    write_register(8'hE1, 8'h04);
    
    // Test 5: Read back oscillator enable register (0xE1)
    $display("\nTest 5: Reading back oscillator enable register (0xE1)");
    read_register(8'hE1);
    
    // Test 6: Configure oscillator 0 registers
    $display("\nTest 6: Configuring oscillator 0 registers");
    
    // Frequency low
    write_register(8'h00, 8'h00);
    read_register(8'h00);
    
    // Frequency high
    write_register(8'h20, 8'h01);
    read_register(8'h20);
    
    // Volume
    write_register(8'h40, 8'hFF);
    read_register(8'h40);
    
    // Waveform table pointer
    write_register(8'h80, 8'h00);
    read_register(8'h80);
    
    // Control register (channel 1, free-run, not halted)
    write_register(8'hA0, 8'h10);
    read_register(8'hA0);
    
    // Resolution/Table size (256 bytes, standard resolution)
    write_register(8'hC0, 8'h00);
    read_register(8'hC0);
    
    $display("\nAll tests completed");
    
    // Run for a while to observe behavior
    repeat (1000) @(posedge clk);
    
    $finish;
  end
  
  // Track and display debug info
  always @(posedge clk) begin
    if (wave_rd) begin
      $display("Wave read at addr=0x%04X, providing data=0x%02X", 
               wave_address, wave_data);
      wave_data_ready = 1;
      wave_data = 8'h80; // Default to mid-level sample
    end else begin
      wave_data_ready = 0;
    end
  end
  
  // Task to write to a register
  task write_register;
    input [7:0] reg_addr;
    input [7:0] reg_data;
    
    begin
      $display("  Writing register 0x%02X = 0x%02X", reg_addr, reg_data);
      
      // Setup phase
      cs_n = 1;
      we_n = 1;
      addr = reg_addr;
      data_in = reg_data;
      
      // Run a few cycles with clk_en pulses
      repeat (8) begin
        @(posedge clk);
      end
      
      // WE assertion phase (write occurs here)
      cs_n = 0;
      we_n = 0;
      
      // Use continuous clk_en pulses during write
      repeat (16) begin
        @(posedge clk);
        $display("    Cycle %0d: CS=%0d WE=%0d CLK_EN=%0d OSC_EN=0x%02X", 
                cycle_count, cs_n, we_n, clk_en, osc_en);
      end
      
      // CS de-assertion phase
      cs_n = 1;
      we_n = 1;
      
      repeat (8) begin
        @(posedge clk);
        $display("    Cycle %0d: CS=%0d WE=%0d CLK_EN=%0d OSC_EN=0x%02X", 
                cycle_count, cs_n, we_n, clk_en, osc_en);
      end
    end
  endtask
  
  // Task to read from a register
  task read_register;
    input [7:0] reg_addr;
    
    begin
      $display("  Reading register 0x%02X", reg_addr);
      
      // Setup phase
      cs_n = 1;
      we_n = 1;
      addr = reg_addr;
      
      repeat (8) begin
        @(posedge clk);
      end
      
      // CS assertion phase (read occurs here)
      cs_n = 0;
      we_n = 1;
      
      // Use continuous clk_en pulses during read
      repeat (16) begin
        @(posedge clk);
        $display("    Cycle %0d: CS=%0d WE=%0d CLK_EN=%0d DATA=0x%02X", 
                cycle_count, cs_n, we_n, clk_en, data_out);
      end
      
      // CS de-assertion phase
      cs_n = 1;
      we_n = 1;
      
      repeat (8) begin
        @(posedge clk);
      end
      
      $display("  Read value: 0x%02X", data_out);
    end
  endtask
  
  // For waveform viewer: make sure we don't run forever
  initial begin
    #5000000; // 5ms max simulation time
    $display("Simulation timed out");
    $finish;
  end

endmodule