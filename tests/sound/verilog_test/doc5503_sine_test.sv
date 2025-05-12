// DOC5503 standalone sine wave test in Verilog for use with Verilator
// Tests oscillator configuration and sine wave generation
`timescale 1ns / 1ps

module doc5503_sine_test;
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
  integer audio_file;
  integer i;
  
  // For tracking audio output
  reg [15:0] audio_samples [0:999];
  integer sample_count;
  
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
  
  // Initialize sine wave table
  initial begin
    for (i = 0; i < 256; i = i + 1) begin
      // Generate a sine wave from 0-255
      sine_table[i] = 128 + $rtoi(127.0 * $sin(2.0 * 3.14159 * i / 256.0));
    end
  end
  
  // Test sequence
  initial begin
    // For Verilator, add these to enable waveform dump
    if ($test$plusargs("trace")) begin
      $dumpfile("doc5503_sine_test.vcd");
      $dumpvars(0, doc5503_sine_test);
    end
    
    // Open file for audio data
    audio_file = $fopen("audio_output.txt", "w");
    if (audio_file == 0) begin
      $display("Error: Could not open file for writing");
      $finish;
    end
    
    // Print header
    $display("=== DOC5503 Sine Wave Test ===");
    $display("Testing sine wave generation");
    
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
    
    // Configure oscillator 0 for sine wave generation
    $display("\nConfiguring oscillator 0 for sine wave generation");
    
    // Enable oscillator 0
    $display("1. Enabling oscillator 0");
    write_register(8'hE1, 8'h01);
    read_register(8'hE1);
    
    // Set frequency (for audible sine wave - around 440Hz)
    // Frequency calculation: F = Fosc * FreqValue / 2^24
    // Where Fosc = 7.159MHz, so for 440Hz: FreqValue = 440 * 2^24 / 7159090 = 1,027,293
    // This is 0x0FAD8D in hex, so freq_low = 0x8D, freq_mid = 0xAD, freq_high = 0x0F
    $display("2. Setting frequency to ~440Hz");
    write_register(8'h00, 8'h8D); // Freq low
    read_register(8'h00);
    
    write_register(8'h20, 8'h0F); // Freq high (using only 8 bits here, simplified)
    read_register(8'h20);
    
    // Set volume to maximum
    $display("3. Setting volume to maximum");
    write_register(8'h40, 8'hFF);
    read_register(8'h40);
    
    // Set waveform table pointer to 0
    $display("4. Setting waveform table pointer to 0");
    write_register(8'h80, 8'h00);
    read_register(8'h80);
    
    // Set control register (channel 1, free-run)
    $display("5. Setting control register to channel 1, free-run");
    write_register(8'hA0, 8'h10);
    read_register(8'hA0);
    
    // Set resolution to 8-bit, 256 samples
    $display("6. Setting resolution to 8-bit, 256 samples");
    write_register(8'hC0, 8'h00);
    read_register(8'hC0);
    
    $display("\nOscillator 0 configured for sine wave generation");
    $display("Running and collecting audio samples...");
    
    // Run for a while to collect audio samples
    for (i = 0; i < 1000; i = i + 1) begin
      // Wait for a clk_en pulse to capture audio in sync with DOC clock
      while (!clk_en) @(posedge clk);
      @(posedge clk);
      
      // Store the audio sample
      if (sample_count < 1000) begin
        audio_samples[sample_count] = left_mix;
        // Write to file
        $fdisplay(audio_file, "%d, %d", sample_count, $signed(left_mix));
        sample_count = sample_count + 1;
      end
    end
    
    $display("\nCollected %0d audio samples", sample_count);
    $display("Audio samples saved to 'audio_output.txt'");
    $fclose(audio_file);
    
    $display("\nTest completed");
    $finish;
  end
  
  // Handle wave data requests
  always @(posedge clk) begin
    if (wave_rd) begin
      $display("Wave read at addr=0x%04X", wave_address);
      
      // Provide sine wave data from table based on address
      wave_data = sine_table[wave_address[7:0]];
      wave_data_ready = 1;
      
      $display("  Providing sine wave data=0x%02X", wave_data);
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
    #5000000; // 5ms max simulation time
    $display("Simulation timed out");
    $finish;
  end

endmodule