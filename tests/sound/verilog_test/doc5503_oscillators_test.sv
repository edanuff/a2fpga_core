// DOC5503 multiple oscillators test in Verilog for use with Verilator
// Tests multiple oscillator configurations and mixing
`timescale 1ns / 1ps

module doc5503_oscillators_test;
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
  reg [7:0] sine_table [0:255];
  reg [7:0] square_table [0:255];
  reg [7:0] triangle_table [0:255];
  reg [7:0] sawtooth_table [0:255];
  integer audio_file;
  integer i;
  
  // For tracking audio output
  reg [15:0] audio_samples_left [0:1999];
  reg [15:0] audio_samples_right [0:1999];
  integer sample_count;
  
  // For tracking oscillators
  reg [4:0] active_osc; // The oscillator currently being accessed
  
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
    .channel_o(active_osc),  // Connect to track the active oscillator
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
  
  // Initialize waveform tables
  initial begin
    // Generate sine wave
    for (i = 0; i < 256; i = i + 1) begin
      sine_table[i] = 128 + $rtoi(127.0 * $sin(2.0 * 3.14159 * i / 256.0));
    end
    
    // Generate square wave
    for (i = 0; i < 128; i = i + 1) begin
      square_table[i] = 230;
    end
    for (i = 128; i < 256; i = i + 1) begin
      square_table[i] = 30;
    end
    
    // Generate triangle wave
    for (i = 0; i < 64; i = i + 1) begin
      triangle_table[i] = 128 + i * 2;
    end
    for (i = 64; i < 192; i = i + 1) begin
      triangle_table[i] = 255 - (i - 64) * 2;
    end
    for (i = 192; i < 256; i = i + 1) begin
      triangle_table[i] = 0 + (i - 192) * 2;
    end
    
    // Generate sawtooth wave
    for (i = 0; i < 256; i = i + 1) begin
      sawtooth_table[i] = i;
    end
  end
  
  // Test sequence
  initial begin
    // For Verilator, add these to enable waveform dump
    if ($test$plusargs("trace")) begin
      $dumpfile("doc5503_oscillators_test.vcd");
      $dumpvars(0, doc5503_oscillators_test);
    end
    
    // Open file for audio data
    audio_file = $fopen("multi_osc_output.txt", "w");
    if (audio_file == 0) begin
      $display("Error: Could not open file for writing");
      $finish;
    end
    
    // Print header
    $display("=== DOC5503 Multiple Oscillators Test ===");
    $display("Testing multiple oscillator configurations and mixing");
    
    // Initialize signals
    reset_n = 0;
    cs_n = 1;
    we_n = 1;
    addr = 0;
    data_in = 0;
    wave_data_ready = 0;
    wave_data = 0;
    cycle_count = 0;
    sample_count = 0;
    
    // Reset sequence
    $display("Resetting DOC5503...");
    repeat (10) @(posedge clk);
    
    // Release reset
    reset_n = 1;
    $display("Reset released");
    
    // Wait for ready signal
    $display("Waiting for DOC5503 to be ready...");
    while (!ready) begin
      @(posedge clk);
    end
    
    $display("DOC5503 is ready after %0d cycles", cycle_count);
    
    // Configure oscillator 0 for sine wave (left channel)
    $display("\nConfiguring oscillator 0 for sine wave (left channel)");
    configure_oscillator(0, 8'h8D, 8'h0F, 8'hC0, 8'h00, 8'h00, 8'h00);
    
    // Configure oscillator 1 for square wave (right channel)
    $display("\nConfiguring oscillator 1 for square wave (right channel)");
    configure_oscillator(1, 8'h46, 8'h0A, 8'hC0, 8'h01, 8'h20, 8'h00);
    
    // Configure oscillator 2 for triangle wave (both channels)
    $display("\nConfiguring oscillator 2 for triangle wave (both channels)");
    configure_oscillator(2, 8'h23, 8'h05, 8'h80, 8'h02, 8'h30, 8'h00);
    
    // Configure oscillator 3 for sawtooth wave (both channels)
    $display("\nConfiguring oscillator 3 for sawtooth wave (both channels)");
    configure_oscillator(3, 8'h11, 8'h03, 8'h80, 8'h03, 8'h30, 8'h00);
    
    // Enable oscillators 0, 1, 2, and 3
    $display("\nEnabling oscillators 0, 1, 2, and 3");
    write_register(8'hE1, 8'h0F);
    read_register(8'hE1);
    
    $display("\nOscillators configured for mixed waveform generation");
    $display("Running and collecting audio samples...");
    
    // Run for a while to collect audio samples
    for (i = 0; i < 2000; i = i + 1) begin
      // Wait for a clk_en pulse to capture audio in sync with DOC clock
      while (!clk_en) @(posedge clk);
      @(posedge clk);
      
      // Store the audio sample
      if (sample_count < 2000) begin
        audio_samples_left[sample_count] = left_mix;
        audio_samples_right[sample_count] = right_mix;
        // Write to file
        $fdisplay(audio_file, "%d, %d, %d", sample_count, $signed(left_mix), $signed(right_mix));
        sample_count = sample_count + 1;
      end
    end
    
    $display("\nCollected %0d audio samples", sample_count);
    $display("Audio samples saved to 'multi_osc_output.txt'");
    $fclose(audio_file);
    
    $display("\nTest completed");
    $finish;
  end
  
  // Handle wave data requests
  always @(posedge clk) begin
    if (wave_rd) begin
      $display("Wave read for oscillator %d at addr=0x%04X", active_osc, wave_address);
      
      // Provide wave data based on oscillator and address
      case (active_osc)
        0: wave_data = sine_table[wave_address[7:0]];
        1: wave_data = square_table[wave_address[7:0]];
        2: wave_data = triangle_table[wave_address[7:0]];
        3: wave_data = sawtooth_table[wave_address[7:0]];
        default: wave_data = sine_table[wave_address[7:0]];
      endcase
      
      wave_data_ready = 1;
      $display("  Providing wave data=0x%02X", wave_data);
    end else begin
      wave_data_ready = 0;
    end
  end
  
  // Task to configure an oscillator
  task configure_oscillator;
    input [4:0] osc_num;
    input [7:0] freq_low;
    input [7:0] freq_high;
    input [7:0] volume;
    input [7:0] wave_ptr;
    input [7:0] control;
    input [7:0] resolution;
    
    begin
      $display("  Configuring oscillator %0d", osc_num);
      
      // Frequency low register (0x00-0x1F)
      write_register(8'h00 + osc_num, freq_low);
      read_register(8'h00 + osc_num);
      
      // Frequency high register (0x20-0x3F)
      write_register(8'h20 + osc_num, freq_high);
      read_register(8'h20 + osc_num);
      
      // Volume register (0x40-0x5F)
      write_register(8'h40 + osc_num, volume);
      read_register(8'h40 + osc_num);
      
      // Waveform table pointer register (0x80-0x9F)
      write_register(8'h80 + osc_num, wave_ptr);
      read_register(8'h80 + osc_num);
      
      // Control register (0xA0-0xBF)
      write_register(8'hA0 + osc_num, control);
      read_register(8'hA0 + osc_num);
      
      // Resolution table size register (0xC0-0xDF)
      write_register(8'hC0 + osc_num, resolution);
      read_register(8'hC0 + osc_num);
    end
  endtask
  
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
      end
      
      // CS de-assertion phase
      cs_n = 1;
      we_n = 1;
      
      repeat (8) begin
        @(posedge clk);
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
    #10000000; // 10ms max simulation time
    $display("Simulation timed out");
    $finish;
  end

endmodule