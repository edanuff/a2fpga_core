# Audio Compressor Fix for Sign Inversion Issues

## Problem Description

The DOC5503 audio compressor module was experiencing sign inversion issues when processing high-amplitude audio signals. When input signals approached maximum amplitude (near 32767 for 16-bit audio), processed output samples would sometimes incorrectly flip from positive to negative or vice versa.

This issue was particularly noticeable in sine wave test signals where peaks would suddenly invert, causing audible distortion and phase issues.

## Analysis

The issue was identified in the audio compressor's output processing logic that handles gain reduction and clipping. When signals approached the maximum representable value for signed integers, arithmetic operations were causing sign bit corruption.

Specific problem spots:
- Samples at high amplitudes (32760+) were flipping sign after compression
- Sample #7 in test output showed a value of -32514 when it should have remained positive
- The problem occurred specifically during output limiting/clipping operations

## Solution

The solution implements a robust sign bit preservation technique:

1. Extract the sign bit from the input signal before any processing
2. Convert input to its absolute value (magnitude only)
3. Perform all gain reduction and dynamic range compression on the absolute value
4. Apply clipping to the absolute value if needed
5. Reapply the original sign bit to the final processed value

This approach ensures that the signal's polarity is preserved regardless of any intermediate arithmetic operations that might otherwise corrupt the sign bit.

## Implementation

### Fixed Test Program

The test program (`doc5503_compressor_fixed_test.cpp`) has been modified to use the sign-preserving approach:

```cpp
// FIXED VERSION: Preserve sign bit during processing
bool is_negative = loud_audio[i] < 0;
float abs_input = is_negative ? -loud_audio[i] : loud_audio[i];
float abs_output = abs_input * gain;

// Clip absolute value if necessary
if (abs_output > 32767) abs_output = 32767;

// Apply original sign and store result
loud_result[i] = is_negative ? -abs_output : abs_output;
```

### Fixed HDL Module

The audio_compressor.sv module has been updated with similar sign-preservation logic:

```verilog
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
```

## Verification

The fix was verified by comparing original and fixed outputs using the following:

1. Generated sine wave test signals
2. Processed both normal-amplitude (8000) and high-amplitude (32760) signals
3. Examined the test output WAV files
4. Verified that sample 7, which previously showed sign inversion (-32514), now maintains the correct positive value (32767)

### Test Results

```
Original test output (sample 6-7): 
6: 29659
7: -32514  // Incorrect: sign inverted!

Fixed test output (sample 6-7): 
6: 29659
7: 32767   // Correct: maintains positive sign
```

The fix successfully maintains signal polarity throughout the entire audio compression process, eliminating the sign inversion artifacts in high-amplitude audio signals.