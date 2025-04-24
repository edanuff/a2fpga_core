# ES5503/DOC5503 Sound Chip Testing

This directory contains tests for the Apple IIgs ES5503/DOC5503 sound chip implementation in FPGA.

## Test Infrastructure

### Standalone Wavetable Test

The `standalone_wavetable_test.cpp` program is a standalone C++ implementation of the core ES5503 wavetable playback functionality. It processes wavetable data files through the ES5503 emulator, generating 16-bit PCM WAV files that can be listened to.

#### Usage

```bash
g++ -std=c++17 -Wall -O3 standalone_wavetable_test.cpp -o standalone_wavetable_test
./standalone_wavetable_test
```

This generates several `.wav` files from the test wavetables:

1. **frontleft_correct.wav**: Generated from the frontleft.wavetable using the verified correct frequency value (0x01AD). 

2. **frontleft_freq_*.wav**: Generated from the stock frontleft.wavetable using different frequency values:
   - 128 (0x0080): Very slow frequency
   - 256 (0x0100): Slow frequency
   - 384 (0x0180): Medium frequency 
   - 429 (0x01AD): Correct frequency for this wavetable
   - 512 (0x0200): Medium-fast frequency
   - 640 (0x0280): Fast frequency
   - 768 (0x0300): Very fast frequency

3. **frontleft_vol_*.wav**: Tests different volume levels with the correct frequency:
   - 64 (0x40): Quarter volume
   - 128 (0x80): Half volume
   - 192 (0xC0): Three-quarter volume
   - 255 (0xFF): Full volume

4. **Generated test waveforms**:
   - sine_wave.wav: Smooth sine wave
   - square_wave.wav: Digital square wave (loud)
   - sawtooth_wave.wav: Linear sawtooth ramp
   - triangle_wave.wav: Triangle wave

5. **Resolution effects**:
   - sine_tableres_*.wav: Same sine wave with different resolution parameters (0-7)

#### ES5503 Behavior Notes

Important behavior characteristics of the ES5503/DOC5503 chip implemented in the tests:

1. **Wavetable Format**: The ES5503 uses 8-bit unsigned wavetable data where:
   - 0x80 = Zero (silence)
   - 0x01-0x7F = Negative values
   - 0x81-0xFF = Positive values
   - 0x00 = HALT signal that stops oscillator

2. **Accumulator/Addressing**: 
   - Uses a 24-bit accumulator that increments by the frequency value each sample
   - A shift value (depending on table size and resolution) determines which accumulator bits to use
   - Formula: `shift = 9 + resolution - tableSize`
   - For tableSize 7 (32KB) and resolution 0, shift = 2

3. **Output Conversion**:
   - Converts 8-bit wavetable values to 16-bit signed audio using XOR 0x80
   - Applies volume scaling (0-255)
   - Final output is scaled to 16-bit range (-32768 to 32767)

4. **Frequency Control**:
   - 16-bit value (0x0000 to 0xFFFF) that controls playback speed
   - Added to 24-bit accumulator each sample
   - Correct value for frontleft.wavetable is 0x01AD for normal playback

### Verilog Implementation Tests

The Verilog implementation is tested using the `doc5503_tb.cpp` program, which exercises the FPGA implementation through the Verilator testbench.

#### Running Verilog Tests

```bash
make run_doc
```

This runs the oscillator enable register test which validates the fix for the off-by-one error in the oscillator enable register.

## File Descriptions

- **frontleft.wavetable**: A 32KB wavetable file containing sample audio data with the correct frequency of 0x01AD
- **standalone_wavetable_test.cpp**: C++ implementation of ES5503 wavetable playback
- **doc5503_tb.cpp**: Verilator testbench for the DOC5503 Verilog implementation
- **doc5503_harness.sv**: SystemVerilog harness connecting the testbench to the Verilog module
- **doc_audio_test.cpp**: Test program for analyzing DOC audio output characteristics
- **Makefile**: Build rules for the tests

## Frontleft.wavetable Analysis

The `frontleft.wavetable` file is a raw 32KB data file that contains waveform data in the Ensoniq ES5503 format. Analysis shows:

- Size: 32,768 bytes
- Value range: 0 to 180 (decimal)
- Contains mostly silence (56.3% of values are 0x80)
- Contains 2 halt values (0x00) 
- Non-silent data starts around offset 0xA70 (2672)

The data is stored in 8-bit unsigned format where:
- 0x80 = Silence (zero amplitude)
- 0x01-0x7F = Negative amplitudes
- 0x81-0xFF = Positive amplitudes
- 0x00 = Halt signal (stops playback)

### Direct Playback vs. ES5503 Oscillator Mode

There are two distinct ways to use the wavetable data:

1. **Direct Playback**: 
   - Playing the file as a raw audio sample (similar to a PCM wave file)
   - Each byte directly becomes one audio sample
   - When converted to WAV this produces a very short sound (1.5 sec at 22050Hz)
   - No special processing needed, just a straight conversion

2. **ES5503 Oscillator Mode**:
   - Using the ES5503's oscillator and accumulator to address the wavetable
   - The wavetable becomes a source of waveform data (not a linear sample)
   - The frequency register determines how quickly we move through the wavetable
   - Table size and resolution affect which parts of the accumulator are used for addressing
   
## The ES5503 DOC Processing Technique

**How the ES5503 Accesses Wavetable Data:**

1. A 24-bit accumulator is incremented by the frequency value on each sample
2. The accumulator bits are shifted based on table size and resolution
3. The resulting address is used to index into the wavetable
4. The sample at that address becomes the audio output

**Key ES5503 Parameters:**
- Table size: 7 (for 32KB wavetable)
- Resolution: 0 (typical setting)
- Frequency: Controls the rate of movement through the wavetable
- Volume: 0xFF (full volume)

This means the wavetable is not played sequentially - it's accessed based on the accumulator value, which increases by the frequency value with each sample. Higher frequencies mean faster movement through the wavetable, but not in a linear way.

**Our test implementations:** We've created both:
1. Direct conversions (`frontleft_direct_*.wav`) - straight 1:1 conversion of the wavetable data
2. ES5503 processed versions - using the oscillator accumulator technique described above

## Oscillator Enable Register Fix

The oscillator enable register test in `doc5503_tb.cpp` validates the fix for the off-by-one error in the ES5503 implementation. The fix changes the calculation for `cycle_max_w` from:

```verilog
wire [4:0] cycle_max_w = osc_en_num_w == 0 ? 0 : 5'(osc_en_num_w - 1'b1);
```

to:

```verilog
wire [4:0] cycle_max_w = osc_en_r[5:1];
```

This corrects an off-by-one error that was causing incorrect oscillator timing and potential distortion.

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

### Technical Implementation Notes

The DOC5503 implementation details:

1. **DSP Pipeline**:
   ```
   Unsigned waveform data (0-255) 
   → XOR with 0x80 to convert to signed (-128 to 127)
   → Multiply by volume (0-255)
   → Apply 0.75x low-pass filter
   → Apply noise gate (if value < ±128)
   → Output to mixer
   ```

2. **Mixer Processing**:
   - Sums all active oscillator outputs
   - Applies channel assignment based on control registers
   - Applies noise gate to final output (if value < ±256)
   - Outputs signed 16-bit stereo audio

## Wavetable Testing Approaches

To fully understand how the wavetable data can be used in the ES5503/DOC chip, we've implemented and tested multiple approaches:

1. **Direct Conversion Testing**: 
   - Generated direct WAV conversions of the raw wavetable data
   - Created versions at different sample rates (8kHz, 11.025kHz, 22.05kHz)
   - Analyzed the content when played as a straight PCM sample
   - Verified that this approach produces a very short sound (~1.5 sec)

2. **ES5503 Oscillator Emulation**:
   - Implemented the full ES5503 oscillator mechanism with 24-bit accumulator
   - Created a parametric system that correctly emulates the wavetable access pattern 
   - Tested various frequency values to understand their effect on the wavetable addressing
   - Verified how changing table size and resolution affects addressing

3. **Parametric Testing**:
   - **Frequency**: Tested values from 0x00AD to 0x0300 
   - **Volume**: Tested amplitude scaling at levels 64, 128, 192, and 255
   - **Table Size/Resolution**: Measured effects on synthetic waveforms
   - **Sample Rate**: Generated output at various rates with correct scaling

4. **Synthetic Waveform Validation**: 
   - Created test waveforms (sine, square, sawtooth, triangle)
   - Processed them through the same ES5503 emulation
   - Compared behavior to ensure consistent implementation

The generated WAV files show the distinction between direct sample playback and ES5503 oscillator processing. This allows us to understand how the chip uses wavetable data and properly implement the wavetable playback mechanisms in the FPGA.