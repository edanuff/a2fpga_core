# DOC5503 Polarity Issue Investigation

## Issue Description

When testing the DOC5503 sound module, we noticed that waveforms generated with different volume levels showed opposite polarities:
- The "normal" volume .wav files are entirely positive-going 
- The "loud" volume .wav files are entirely negative-going

This inconsistency in polarity can cause issues when mixing audio signals together, as they may partially cancel each other out instead of reinforcing.

## Root Cause Identified

After examining the DOC5503 code and running detailed polarity tests, the root cause was identified in the DSP multiplier section of `doc5503.sv` (around line 676-678):

```verilog
automatic logic signed [7:0] data_w = wds_w ^ 8'h80;            // convert waveform data to signed
automatic logic signed [7:0] vol_s = {2'b0, vol_w[7:2]};        // convert volume to signed
output_r <= data_w * vol_w;                                     // output is waveform data * volume
```

The issue is that the waveform data is properly converted to signed format (`data_w`), but the multiplication uses the raw volume register (`vol_w`) instead of the signed-converted volume (`vol_s`).

Since the raw volume value (`vol_w`) is an unsigned value (0-255), multiplying a signed value by an unsigned value can lead to inconsistent sign handling in the result.

## Expected Behavior

The expected behavior is:
- Positive signed waveform data (after XOR 0x80) multiplied by any volume should result in positive output
- Negative signed waveform data (after XOR 0x80) multiplied by any volume should result in negative output
- The sign of the output should be consistent with the sign of the waveform data

## Verification Details

When testing with fixed wave values and maximum volume (255):
- Wave value 0x01 (signed value -127) produces output with one polarity
- Wave value 0xFF (signed value +127) produces output with the opposite polarity
- The polarities are inverted when they should be consistent

## Fix Recommendation

The fix for this issue is to modify the DSP multiplier in `doc5503.sv` to either:

1. Use consistent types for multiplication:
```verilog
automatic logic signed [7:0] data_w = wds_w ^ 8'h80;            // convert waveform data to signed
automatic logic signed [7:0] vol_s = {1'b0, vol_w[7:1]};        // convert volume to signed, scaled
output_r <= data_w * vol_s;                                     // output is waveform data * scaled volume
```

2. Or explicitly handle unsigned multiplication with sign preservation:
```verilog
automatic logic signed [7:0] data_w = wds_w ^ 8'h80;            // convert waveform data to signed
output_r <= data_w[7] ? -((-data_w) * vol_w) : (data_w * vol_w); // preserve sign of data_w
```

## Implementation Considerations

- The fix should be validated to ensure it doesn't significantly impact the volume scaling
- Both the test environment and actual FPGA implementation should be checked
- The audio compressor should also be verified after this change to ensure it's properly responding to the correct polarity

## Conclusion

This polarity issue causes inconsistent waveform orientation, which could lead to:
- Phase cancellation when mixing multiple oscillators
- Difficulty in achieving proper audio compression
- Potential signal quality issues when audio signals are mixed

The identified fix should provide a more consistent and predictable audio output across all volume levels.