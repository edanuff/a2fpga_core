# DOC5503 Sound Module Tests

This directory contains tests for the DOC5503 sound module in the A2FPGA core.

## DC Offset Issues and Fixes

We identified and fixed DC offset issues in our audio test files. The DOC5503 hardware implementation correctly handles zero-centering, but our test program needed adjustment to match the hardware behavior.

### Key Findings:

1. **Proper Zero-Centering in Hardware**: The DOC5503 properly converts unsigned waveform data (0-255, centered at 128) to signed data by XORing with 0x80, which correctly preserves zero-centering.

2. **Noise Gate Benefits**: The noise gate implementation (in both DOC5503 and sound_glu) effectively eliminates low-level buzzing without affecting DC offset:
   - DOC5503 noise gate threshold: ±128
   - GLU noise gate threshold: Configurable (default: 48)

3. **Test Program Corrections**: Our test program now correctly models the hardware behavior by:
   - Measuring DC offset in wavetable data after XOR conversion
   - Applying DC offset correction to ensure true zero-centering
   - Ensuring consistency between hardware emulation and test files

### Corrected Audio Test Files:

- `glu_reference.wav`: Reference waveform with proper silent sections
- `glu_with_buzz.wav`: Simulation of the buzzing issue during silent periods
- `glu_fixed.wav`: Same audio with noise gate applied to fix buzzing
- `glu_square_raw.wav`: Zero-centered square wave
- `glu_square_filtered.wav`: Zero-centered filtered square wave (0.75x)
- `glu_vol*_gate*.wav`: Various volume and noise gate settings

### Recommendations:

1. Keep the current noise gate implementation in both DOC5503 and sound_glu modules
2. Maintain the 0.75x multiplier in the DOC5503 to reduce high-frequency components
3. No changes are needed to the hardware since it correctly handles zero-centering

## How to Run the Tests

The testing approach consists of:

1. Generating test WAV files for analysis:
```
cd tests/sound
g++ -o glu_wav_test glu_wav_test.cpp -lm
./glu_wav_test
```

2. Playing/comparing the generated files:
```
./view_waves.sh
```

3. For more detailed simulation tests (requires Verilator):
```
make
./doc_audio_test
```

## Analysis Tools

The `check_dc_offset()` function in `glu_wav_test.cpp` analyzes WAV files for DC offset, reporting:
- Average sample value: Should be close to zero for properly centered audio
- RMS value: Indicates signal strength
- DC offset percentage: Should be close to 0% (within ±0.1%)
- DC offset / RMS ratio: More meaningful measure of DC offset impact

## Technical Implementation Notes

The DOC5503 implementation details:

1. **Waveform Data Format**: 
   - 8-bit unsigned with 0x80 being zero level
   - 0x01 and 0xFF are the lowest and highest levels
   - 0x00 is reserved to stop oscillator playback

2. **DSP Pipeline**:
   ```
   Unsigned waveform data (0-255) 
   → XOR with 0x80 to convert to signed (-128 to 127)
   → Multiply by volume (0-255)
   → Apply 0.75x low-pass filter
   → Apply noise gate (if value < ±128)
   → Output to mixer
   ```

3. **Mixer Processing**:
   - Sums all active oscillator outputs
   - Applies channel assignment based on control registers
   - Applies noise gate to final output (if value < ±256)
   - Outputs signed 16-bit stereo audio

4. **GLU Module**:
   - Interfaces DOC5503 to Apple II bus
   - Applies additional volume control
   - Implements configurable noise gate for final output