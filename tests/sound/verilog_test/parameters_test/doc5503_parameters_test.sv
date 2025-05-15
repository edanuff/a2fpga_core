// DOC5503 Parameters Test
// Print out key parameters for all combinations of wavetable size and resolution

`timescale 1ns / 1ps

module doc5503_parameters_test;

  // Test variables
  reg [2:0] curr_res;      // Resolution setting (0-7)
  reg [2:0] curr_wts;      // Wavetable size setting (0-7)
  
  // Calculated parameters
  reg [4:0] curr_shift;    // Calculated shift value
  integer high_bit;        // High bit position for overflow detection
  reg [23:0] acc_mask;     // Accumulator mask
  
  // Test output file
  integer file;
  
  initial begin
    // Open output file
    file = $fopen("parameters_test_results.txt", "w");
    if (file == 0) begin
      $display("Error opening file for writing");
      $finish;
    end
    
    $fdisplay(file, "DOC5503 Parameters Test");
    $fdisplay(file, "====================\n");
    $fdisplay(file, "Testing all combinations of wavetable size (WTS) and resolution (RES)");
    $fdisplay(file, "----------------------------------------------------------------\n");
    
    $fdisplay(file, "| WTS | RES | SHIFT | HIGH_BIT | ACC_MASK (hex)   | ACC_MASK (binary)                 |");
    $fdisplay(file, "|-----|-----|-------|----------|------------------|-----------------------------------|");
    
    // Fixed loop to ensure all combinations are tested correctly
    for (integer wts = 0; wts < 8; wts = wts + 1) begin
      for (integer res = 0; res < 8; res = res + 1) begin
        // Set the values for calculations
        curr_wts = wts[2:0];
        curr_res = res[2:0];
        
        // Calculate parameters exactly as in doc5503.sv
        curr_shift = 5'd9 + curr_res - curr_wts;
        high_bit = 17 + curr_res;
        acc_mask = {16'((1 << (9 + curr_res)) - 1), 8'hFF};
        
        // Print results
        $fdisplay(file, "| %3d | %3d | %5d | %8d | 0x%016x | %b |", 
                 curr_wts, curr_res, curr_shift, high_bit, acc_mask, acc_mask);
      end
    end
    
    // Extra information
    $fdisplay(file, "\nWavetable Size Constants (from MAME):");
    $fdisplay(file, "------------------------------------");
    $fdisplay(file, "wavesizes[] = {256, 512, 1024, 2048, 4096, 8192, 16384, 32768}");
    
    $fdisplay(file, "\nShift Formula:");
    $fdisplay(file, "-------------");
    $fdisplay(file, "curr_shift_w = 5'd9 + curr_res_w - curr_wts_w;");
    
    $fdisplay(file, "\nHigh Bit Formula (for overflow detection):");
    $fdisplay(file, "---------------------------------------");
    $fdisplay(file, "high_bit_w = 17 + resolution;");
    
    $fdisplay(file, "\nAccumulator Mask Formula:");
    $fdisplay(file, "------------------------");
    $fdisplay(file, "curr_acc_mask_w = {16'((1 << (9 + resolution)) - 1), 8'hFF};");
    
    $fdisplay(file, "\nOverflow Detection in MAME vs Verilog:");
    $fdisplay(file, "-----------------------------------");
    $fdisplay(file, "Verilog: overflow = temp_acc[high_bit_w];");
    $fdisplay(file, "MAME:    overflow = (altram >= wtsize);  // where altram = acc >> resshift");
    
    $fclose(file);
    $display("Test completed, results written to parameters_test_results.txt");
    $finish;
  end

endmodule