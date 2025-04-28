/*
 * audio_compressor.sv - Dynamic audio compressor for the A2FPGA project
 * 
 * This module implements a dynamic range compressor with configurable parameters
 * including attack/release rates, threshold, ratio, knee width, and makeup gain.
 * It takes high-resolution input (e.g., 24-bit) and produces lower-resolution
 * output (e.g., 16-bit) with controlled dynamics to prevent clipping while
 * maintaining good audio levels.
 */
`timescale 1 ns / 1 ps

module audio_compressor #(
  parameter int INPUT_WIDTH = 24,           // Input audio width in bits
  parameter int OUTPUT_WIDTH = 16,          // Output audio width in bits
  parameter real ATTACK_RATE = 0.01,        // Attack rate (0.0-1.0), lower is slower
  parameter real RELEASE_RATE = 0.001,      // Release rate (0.0-1.0), lower is slower
  parameter real THRESHOLD = 0.7,           // Threshold level (0.0-1.0), level where compression begins
  parameter real RATIO = 4.0,               // Compression ratio (1.0-inf), higher is more compression
  parameter real KNEE_WIDTH = 0.2,          // Soft knee width (0.0-1.0), higher is softer
  parameter real MAKEUP_GAIN = 1.2,         // Makeup gain (0.0-10.0), to compensate for reduced volume
  parameter bit PROTECT_SIGN_BIT = 1        // Enable sign bit protection to prevent polarity inversion
) (
  input  logic        clk_i,                // Clock
  input  logic        reset_i,              // Reset (active high)
  input  logic        enable_i,             // Enable (active high)
  input  logic signed [INPUT_WIDTH-1:0] audio_in_l,   // Left channel input
  input  logic signed [INPUT_WIDTH-1:0] audio_in_r,   // Right channel input
  output logic signed [OUTPUT_WIDTH-1:0] audio_out_l, // Left channel output
  output logic signed [OUTPUT_WIDTH-1:0] audio_out_r  // Right channel output
);
  // Internal parameters scaled to fixed point representation
  localparam int FP_BITS = 16;
  localparam int FP_ONE = (1 << FP_BITS);
  localparam int FP_ATTACK = int'(ATTACK_RATE * FP_ONE);
  localparam int FP_RELEASE = int'(RELEASE_RATE * FP_ONE);
  localparam int FP_THRESHOLD = int'(THRESHOLD * FP_ONE);
  localparam int FP_KNEE_WIDTH = int'(KNEE_WIDTH * FP_ONE);
  localparam int FP_MAKEUP_GAIN = int'(MAKEUP_GAIN * FP_ONE);
  localparam int FP_RATIO = int'(RATIO * FP_ONE);
  localparam int FP_RATIO_MINUS_ONE = FP_RATIO - FP_ONE;
  
  // Constants for calculating the envelope
  localparam int MAX_INPUT_VALUE = (1 << (INPUT_WIDTH-1)) - 1;
  localparam int MAX_OUTPUT_VALUE = (1 << (OUTPUT_WIDTH-1)) - 1;
  
  // State variables
  logic signed [FP_BITS+INPUT_WIDTH-1:0] envelope_r;       // Current envelope value (fixed point)
  logic signed [FP_BITS+INPUT_WIDTH-1:0] gain_reduction_r; // Current gain reduction (fixed point)
  
  // Scaling factor from input to output bitwidth
  localparam int SCALING_SHIFT = INPUT_WIDTH - OUTPUT_WIDTH;
  
  // Temporary variables for calculation
  logic [INPUT_WIDTH-1:0] abs_left_r;
  logic [INPUT_WIDTH-1:0] abs_right_r;
  logic [INPUT_WIDTH-1:0] max_abs_r;
  
  // Intermediate variables for envelope tracking and gain calculation
  logic signed [FP_BITS+INPUT_WIDTH-1:0] level_fp;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] gain_reduction;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] level_normalized;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] threshold_min;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] threshold_max;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] knee_factor;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] amount_over;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] left_reduced;
  logic signed [FP_BITS+INPUT_WIDTH-1:0] right_reduced;
  
  // Helper functions for proper signed arithmetic
  
  // Function to perform signed fixed-point multiplication preserving sign
  function automatic logic signed [FP_BITS+INPUT_WIDTH-1:0] signed_mult(
    input logic signed [INPUT_WIDTH-1:0] value,
    input logic signed [FP_BITS-1:0] fixed_factor
  );
    // Extract sign bits
    logic sign_value, sign_factor, result_sign;
    logic [INPUT_WIDTH-2:0] abs_value;
    logic [FP_BITS-2:0] abs_factor;
    logic [FP_BITS+INPUT_WIDTH-2:0] abs_result;
    
    sign_value = value[INPUT_WIDTH-1];
    sign_factor = fixed_factor[FP_BITS-1];
    result_sign = sign_value ^ sign_factor; // XOR to determine result sign
    
    // Get absolute values
    abs_value = sign_value ? (~value[INPUT_WIDTH-2:0] + 1'b1) : value[INPUT_WIDTH-2:0];
    abs_factor = sign_factor ? (~fixed_factor[FP_BITS-2:0] + 1'b1) : fixed_factor[FP_BITS-2:0];
    
    // Multiply absolute values
    abs_result = abs_value * abs_factor;
    
    // Apply resulting sign
    return result_sign ? {1'b1, ~abs_result + 1'b1} : {1'b0, abs_result};
  endfunction
  
  // Function to properly scale down a signed fixed-point value
  function automatic logic signed [OUTPUT_WIDTH-1:0] scale_to_output(
    input logic signed [FP_BITS+INPUT_WIDTH-1:0] value
  );
    logic sign;
    logic [FP_BITS+INPUT_WIDTH-2:0] abs_value;
    logic [OUTPUT_WIDTH-2:0] scaled_abs;
    
    // Extract sign
    sign = value[FP_BITS+INPUT_WIDTH-1];
    
    // Get absolute value
    abs_value = sign ? (~value[FP_BITS+INPUT_WIDTH-2:0] + 1'b1) : value[FP_BITS+INPUT_WIDTH-2:0];
    
    // Scale down (shift right by FP_BITS)
    scaled_abs = abs_value >> FP_BITS;
    
    // Clip to output range if needed
    if (scaled_abs > (1 << (OUTPUT_WIDTH-1)) - 1)
      scaled_abs = (1 << (OUTPUT_WIDTH-1)) - 1;
    
    // Reapply sign and return
    return sign ? {1'b1, ~scaled_abs + 1'b1} : {1'b0, scaled_abs};
  endfunction
  
  always_ff @(posedge clk_i) begin
    if (reset_i) begin
      envelope_r <= '0;
      gain_reduction_r <= FP_ONE; // No reduction
      audio_out_l <= '0;
      audio_out_r <= '0;
      abs_left_r <= '0;
      abs_right_r <= '0;
      max_abs_r <= '0;
    end else if (enable_i) begin
      // Calculate absolute values
      abs_left_r <= (audio_in_l < 0) ? -audio_in_l : audio_in_l;
      abs_right_r <= (audio_in_r < 0) ? -audio_in_r : audio_in_r;
      
      // Get the maximum absolute value
      max_abs_r <= (abs_left_r > abs_right_r) ? abs_left_r : abs_right_r;
      
      // Update envelope (peak detector with attack/release)
      if (max_abs_r > envelope_r[FP_BITS+INPUT_WIDTH-1:FP_BITS]) begin
        // Attack phase (fast)
        envelope_r <= envelope_r + ((((max_abs_r << FP_BITS) - envelope_r) * FP_ATTACK) >> FP_BITS);
      end else begin
        // Release phase (slow)
        envelope_r <= envelope_r - ((((envelope_r - (max_abs_r << FP_BITS))) * FP_RELEASE) >> FP_BITS);
      end
      
      // Calculate the level in fixed point
      level_fp = envelope_r;
      
      // Normalize level to 0.0-1.0 range in fixed point
      level_normalized = (level_fp * FP_ONE) / (MAX_INPUT_VALUE << FP_BITS);
      
      // Calculate soft knee thresholds
      threshold_min = FP_THRESHOLD - (FP_KNEE_WIDTH / 2);
      threshold_max = FP_THRESHOLD + (FP_KNEE_WIDTH / 2);
      
      // Apply soft knee compression
      if (level_normalized < threshold_min) begin
        // Below threshold - no compression
        gain_reduction = FP_ONE;
      end else if (level_normalized < threshold_max) begin
        // In the knee region - gradual compression
        knee_factor = ((level_normalized - threshold_min) * FP_ONE) / FP_KNEE_WIDTH;
        gain_reduction = FP_ONE - (((knee_factor * knee_factor) * FP_RATIO_MINUS_ONE) / (2 * FP_ONE));
      end else begin
        // Above threshold - full compression
        amount_over = level_normalized - threshold_max;
        gain_reduction = FP_ONE - ((amount_over * FP_RATIO_MINUS_ONE) / FP_ONE);
      end
      
      // Ensure gain reduction factor is never negative to prevent polarity inversion
      if (gain_reduction < 0) gain_reduction = 0;
      
      // Apply makeup gain
      gain_reduction = (gain_reduction * FP_MAKEUP_GAIN) >> FP_BITS;
      
      // Store gain reduction
      gain_reduction_r <= gain_reduction;
      
      // Very simple direct approach to prevent sign inversion
      
      // LEFT CHANNEL - COMPLETELY REWRITTEN FOR MAXIMUM ROBUSTNESS
      
      // Special handling to ensure sign bits are preserved correctly
      // Solution: Extract sign bit from input before any processing, 
      // and reapply it at the very end after all processing and clipping is done
      
      // Process left channel with sign bit preservation 
      begin
        logic sign_bit_l;
        logic [31:0] abs_input_l, abs_scaled_l;
        logic [OUTPUT_WIDTH-1:0] final_l;
        
        // Extract sign bit first - we'll preserve this throughout
        sign_bit_l = audio_in_l[INPUT_WIDTH-1];
        
        // Get absolute value of input
        abs_input_l = sign_bit_l ? -audio_in_l : audio_in_l;
        
        // Apply gain reduction to absolute value only
        abs_scaled_l = (abs_input_l * gain_reduction) >>> FP_BITS;
        
        // Limit to maximum output value, but working only with positive values
        if (abs_scaled_l > MAX_OUTPUT_VALUE) begin
            final_l = MAX_OUTPUT_VALUE;
        end else begin
            final_l = abs_scaled_l[OUTPUT_WIDTH-1:0];
        end
        
        // Reapply the original sign bit to the final value
        audio_out_l <= sign_bit_l ? -final_l : final_l;
      end
      
      // RIGHT CHANNEL - COMPLETELY REWRITTEN FOR MAXIMUM ROBUSTNESS
      
      // Special handling to ensure sign bits are preserved correctly
      // Solution: Extract sign bit from input before any processing, 
      // and reapply it at the very end after all processing and clipping is done
      
      // Process right channel with sign bit preservation
      begin
        logic sign_bit_r;
        logic [31:0] abs_input_r, abs_scaled_r;
        logic [OUTPUT_WIDTH-1:0] final_r;
        
        // Extract sign bit first - we'll preserve this throughout
        sign_bit_r = audio_in_r[INPUT_WIDTH-1];
        
        // Get absolute value of input
        abs_input_r = sign_bit_r ? -audio_in_r : audio_in_r;
        
        // Apply gain reduction to absolute value only
        abs_scaled_r = (abs_input_r * gain_reduction) >>> FP_BITS;
        
        // Limit to maximum output value, but working only with positive values
        if (abs_scaled_r > MAX_OUTPUT_VALUE) begin
            final_r = MAX_OUTPUT_VALUE;
        end else begin
            final_r = abs_scaled_r[OUTPUT_WIDTH-1:0];
        end
        
        // Reapply the original sign bit to the final value
        audio_out_r <= sign_bit_r ? -final_r : final_r;
      end
      
      // We now explicitly preserve the sign bit during processing
      // So we don't need this additional check anymore
      
      // We already clip in the sign-preserving code above
    end
  end
endmodule
