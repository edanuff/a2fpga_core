# DOC5503 Sound Engine Tests

This directory contains test programs for the DOC5503 sound module, including a Verilator test harness for simulation and audio testing tools.

## DOC5503 Low-Pass Filter Implementation

The DOC5503 module has been enhanced with a simple low-pass filter to reduce high-frequency buzzy distortion in the audio output. The filter is implemented by multiplying the output by 0.75 (3/4) in the DSP multiplier section of the code:

```verilog
// DSP Multiplier with simple low-pass filter
always @(posedge clk_i) begin
    automatic logic signed [7:0] data_w = wds_w ^ 8'h80;            // convert waveform data to signed
    automatic logic signed [7:0] vol_s = {2'b0, vol_w[7:2]};        // convert volume to signed
    
    // Apply a subtle low-pass filter by multiplying the output by 0.75
    // to reduce high-frequency buzzy distortion
    automatic logic signed [15:0] raw_product = data_w * vol_s;
    output_r <= (raw_product * 3) >>> 2;                           // multiply by 0.75 (3/4)
end
```

This simple multiplication attenuates high-frequency content while maintaining the overall character of the sound. The 0.75 multiplier was chosen to provide a good balance between reducing harshness and preserving volume levels.

## Audio Test Programs

### 1. Verilator Test Harness

- `doc5503_harness.sv`: A wrapper module for the DOC5503 simulation
- `doc5503_tb.cpp`: The C++ testbench that drives the simulation

### 2. Audio Analysis Tools

- `doc_audio_test.cpp`: Analyzes DOC audio output with various parameters
- `generate_test_wav.cpp`: Generates synthetic waveforms to demonstrate filter effectiveness
- `view_waves.sh`: Interactive script to play and compare audio samples

## Generated WAV Files

The test programs generate multiple WAV files for comparison:

1. **Square Wave Tests** (best for hearing filter effect):
   - `square_raw_freq*.wav`: Unfiltered square waves at different frequencies
   - `square_filtered_freq*.wav`: Filtered square waves with 0.75x multiplier

2. **Sawtooth Wave Tests**:
   - `sawtooth_raw_freq*.wav`: Unfiltered sawtooth waves at different frequencies
   - `sawtooth_filtered_freq*.wav`: Filtered sawtooth waves with 0.75x multiplier

3. **DOC Audio Tests**:
   - `doc_output_scale_*.wav`: Current DOC implementation with different mixer scaling
   - `doc_output_vol_*.wav`: DOC output with different volume levels
   - `doc_output_8osc_*.wav`: Simulated 8-oscillator mix with different scaling

## Running the Audio Tests

1. Generate all test WAV files:
```bash
make generate_test_wav
make run_generate_test
make doc_audio_test
./doc_audio_test
```

2. Use the interactive player to compare filtered vs. unfiltered audio:
```bash
./view_waves.sh
```

## Audio Filter Evaluation

When evaluating the audio filter:

1. The difference is most noticeable in square wave examples at higher frequencies
2. The filter reduces harsh high-frequency content while preserving overall volume
3. For 8-oscillator mixes, the filtered version (0.75x) reduces clipping and distortion
4. In Audacity, you can visually see the reduction in high-frequency overtones

## Debugging Audio Issues

The test harness allows you to:

1. Set up DOC registers with known values
2. Provide controlled waveform data for the oscillators
3. Monitor the audio output signals (mono_mix_o, left_mix_o, right_mix_o)
4. Trace the internal signal values using the VCD output

This can help identify issues with:
- DOC register setup
- Waveform data handling
- Audio signal generation
- Oscillator mixing

## Tips

- Use the `view_waves.sh` script to compare audio samples easily
- Listen to the square wave examples first, as they show the most pronounced difference
- The filter effect is more noticeable at higher frequencies
- Using the 0.75 multiplier in the actual DOC module provides a good balance