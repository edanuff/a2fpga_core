# DOC5503 Audio Troubleshooting Guide

## IMPORTANT: DO NOT MODIFY THE DSP MULTIPLIER FIRST

When encountering audio output issues or when making changes to the audio path, follow this systematic approach:

## Troubleshooting Procedure

### Step 1: Focus on the Most Recent Changes
- First examine the most recent code changes you made to the audio path
- Verify that those changes are working as expected
- Check for unintended side effects like signal disconnections or sign bit issues

### Step 2: Verify Test Inputs
Common issues with test code:
- **Zero detection**: Check if the test code uses `0x00` values in waveforms (these will HALT the oscillator!)
- **Oscillator halting**: Verify the oscillator isn't being halted by control register settings
- **Wave data initialization**: Make sure wavetables are properly initialized with non-zero values
- **Test frequency**: Ensure the frequency is set high enough for samples to be generated in the test window

### Step 3: Check Signal Flow
Before modifying DSP code, check these areas:
- **Input signals** to modules are correctly connected and have valid values
- **Control registers** are set to appropriate values (verify in the test code)
- **Signal modifications** at each stage of the pipeline 
- **Output connections** are properly connected

### Step 4: Specific Checks for Common DOC5503 Issues
- **Oscillator enable register (0xE1)**: Is it set correctly? This controls the NUMBER of enabled oscillators, not a bitmap. Write 2×N where N is the desired oscillator count (e.g., 0x02 for 1 oscillator, 0x40 for 32 oscillators).
- **Control register**: 
  - Is the halt bit (bit 0) cleared? This is common source of silent outputs.
  - Channel routing: Channel bits [7:4] determine whether audio goes to left/right channels:
    ```
    if (ca[0]) next_left_mix_r += output; else next_right_mix_r += output;
    ```
  - Always check both left_mix_o and right_mix_o in your tests - output may be routed to only one!
- **Wave data**: VERY IMPORTANT - 0x00 values in waveform data will HALT the oscillator! Use non-zero values (0x01 or higher).
- **Mode settings**: Is the oscillator in the correct mode (free-running, one-shot, etc.)?
- **Clock enable**: Is the doc's clock enable signal being pulsed properly?

### Step 5: Check Integration and Signal Path
For complex changes like adding compressors:
- **Parameter consistency**: Ensure bit widths match between modules
- **Enable signals**: Is the new module properly enabled?
- **Input connections**: Are signals properly connected to the input of the module?
- **Output connections**: Is the output from the module properly routed to the next stage?
- **Multiplexing**: If there are conditional paths, are the conditions evaluating as expected?

### Step 6: Trace Signal Values
Use the simulator to trace values through the pipeline:
- Look at waveform reads (are they happening?)
- Examine oscillator output values (are they non-zero?)
- Check intermediate values at each pipeline stage
- Look at the final mixer output values

## ONLY AFTER all of the above steps should you consider modifying the DSP multiplier or other core audio processing code.

## Common Issues and Solutions

### Issue 1: No Audio Output from DOC5503 Tests

**Symptoms:**
- Test completes successfully but produces no audio output
- No wave reads are reported during execution
- Oscillator appears to be halted in control register (`0xA0` bit 0 = 1)

**Root Cause:**  
The DOC5503 oscillator is automatically halted during initialization due to a detection of a zero byte (`0x00`) in the waveform data sample register (`wds_w`).

**Technical Explanation:**  
In `doc5503.sv`, the oscillator state machine checks for zeros in the waveform data:

```verilog
automatic logic zero_byte_w = (wds_w == 8'h00);

// ...

if (overflow | zero_byte_w) begin
    current_acc_reset_req_r <= 1;
    if (zero_byte_w | mode_w[0]) begin  // zero byte halts oscillator
        osc_state_r <= OSC_HALT;
    end  
    // ...
end
```

During initialization, `wds_w` is 0x00, which triggers this halt condition.

**Solution:**  
One of the following approaches can fix this issue:

1. Initialize the waveform data sample register to `0x80` instead of `0x00`
2. Add initialization phase detection to ignore zeros during startup
3. Add a force-unhalt mechanism for testing

See `DOC5503_Oscillator_Halting_Issue.md` for detailed fix recommendations.

### Issue 2: Incorrect Oscillator Enable Register Setting

**Symptoms:**
- Not all expected oscillators are running
- Oscillator settings don't match actual behavior

**Root Cause:**  
The Oscillator Enable Register (`0xE1`) has a specific format that's easy to misinterpret.

**Technical Explanation:**  
Register `0xE1` bits 5-1 set the number of active oscillators minus 1. The formula is:
- For N oscillators, set bits 5-1 to (N-1)
- Value = ((N-1) << 1) | 0x0

**Solution:**  
Use the correct formula to set the oscillator count:
- For 1 oscillator: `0x02`
- For 16 oscillators: `0x1E`
- For all 32 oscillators: `0x3E`

### Issue 3: Zero Values in Wavetable Halting Oscillators

**Symptoms:**
- Oscillator unexpectedly halts during playback
- Audio output suddenly stops

**Root Cause:**  
Any `0x00` values in the wavetable will cause the oscillator to halt.

**Technical Explanation:**  
The DOC5503 design halts oscillators when encountering a zero in the wavetable. This is intentional behavior but can cause problems with test patterns.

**Solution:**  
Ensure all wavetable data provided to the DOC has no zero values:
- Replace `0x00` with `0x01` for minimal amplitude
- Use `0x80` for zero-crossing points in signed audio
- For test patterns, use values between `0x01` and `0xFF`

### Issue 4: Channel Assignment Confusion

**Symptoms:**
- Audio appears in unexpected channel
- No audio in expected channel

**Root Cause:**  
Misunderstanding of how control register bits map to channels.

**Technical Explanation:**  
The control register (`0xA0`-`0xBF`) bits 7-4 determine the channel assignment:
- `0x00`: Channel 0 (mapped to right output)
- `0x10`: Channel 1 (mapped to left output)
- And so on, up to channel 15

**Solution:**  
Set control register bits 7-4 correctly:
- For right channel: `0x00` in bits 7-4
- For left channel: `0x10` in bits 7-4
- Ensure halt bit (bit 0) is cleared (`0`)

### Issue 5: Incorrect Volume Scaling

**Symptoms:**
- Audio too quiet or too loud
- Distortion in audio output

**Root Cause:**  
Misunderstanding how the multiplier in doc5503.sv scales volume.

**Technical Explanation:**  
The volume register (`0x40`-`0x5F`) provides an 8-bit value multiplied by the waveform sample. The multiplication is performed in hardware:

```verilog
automatic logic signed [7:0] data_w = wds_w ^ 8'h80;  // Convert to signed
automatic logic signed [7:0] vol_s = {2'b0, vol_w[7:2]}; // Scale volume 
output_r <= data_w * vol_w;  // Multiply
```

**Solution:**  
Ensure volume register is set appropriately:
- For maximum volume: `0xFF`
- For half volume: `0x80`
- For quarter volume: `0x40`

### Issue 6: DSP Multiplier and Polarity Issues

**Symptoms:**
- Inconsistent polarity between different volume levels
- Waveforms at low volume are positive-going while high volume are negative-going (or vice versa)

**Root Cause:**  
Sign bit handling in the DSP multiplier when using unsigned volume values > 127.

**Technical Explanation:**  
When an unsigned 8-bit volume value ≥ 128 is used directly in multiplication with signed data, the high bit can be interpreted as a sign bit, causing polarity inversion.

**Solution:**  
There are several approaches:
1. Use a 9-bit signed volume value with the high bit guaranteed to be 0
2. Ensure test cases use volume values < 128 for consistent polarity
3. Post-process audio to normalize polarity when comparing waveforms

## Common Pitfalls to Avoid
1. Assuming the DSP multiplier is the source of the problem when silent output occurs
2. Using test vectors with 0x00 values which will halt the oscillator
3. Forgetting to properly connect new modules in the signal chain
4. Not accounting for signal width differences when adding new processing stages
5. Changing core logic without verifying the test inputs are valid

## Debugging Tools

1. **Register Dump:**  
   Check key registers before and after initialization:
   ```c
   uint8_t control = read_register(dut, 0xA0);
   uint8_t volume = read_register(dut, 0x40);
   uint8_t osc_en = read_register(dut, 0xE1);
   printf("Control: 0x%02X, Volume: 0x%02X, OscEn: 0x%02X\n", 
          control, volume, osc_en);
   ```

2. **Wave Read Monitoring:**  
   Print when wave reads occur and what values are provided:
   ```c
   if (dut->wave_rd_o) {
       printf("Wave read at addr 0x%04X, providing 0x%02X\n", 
              dut->wave_address_o, wave_value);
   }
   ```

3. **Audio Output Sampling:**  
   Periodically check if there's any audio signal:
   ```c
   if (i % 100 == 0) {
       printf("Left: %d, Right: %d\n", dut->left_mix_o, dut->right_mix_o);
   }
   ```

## Test Methodology

When developing tests for the DOC5503:

1. Always check register values after writing them
2. Never use zero values in wavetables
3. Verify oscillator is not halted (control register bit 0 = 0)
4. Use simple, predictable waveforms (sawtooth, square)
5. Start with a single oscillator before testing multiple oscillators
6. Test each mode separately (free-run, one-shot, sync/AM, swap)

## Recommended Test Setup

```c
// Basic DOC5503 Test Setup
// 1. Reset the DOC
dut->reset_n_i = 0;
// Apply reset cycles
dut->reset_n_i = 1;

// 2. Configure a single oscillator
write_register(dut, 0xE1, 0x02);       // Enable 1 oscillator
write_register(dut, 0x00, 0xFF);       // Freq Low
write_register(dut, 0x20, 0x0F);       // Freq High
write_register(dut, 0x40, 0xFF);       // Volume
write_register(dut, 0x80, 0x00);       // Waveform Pointer
write_register(dut, 0xA0, 0x00);       // Control: Ch0, Free Run
write_register(dut, 0xC0, 0x00);       // 256-byte table

// 3. Verify settings are correctly applied
uint8_t control = read_register(dut, 0xA0);
printf("Control: 0x%02X (Halt: %d)\n", control, control & 0x01);

// 4. Run test with non-zero wave values
if (dut->wave_rd_o) {
    // Provide non-zero wave data (avoid 0x00)
    dut->wave_data_i = 0x80;  // Or other test pattern
    dut->wave_data_ready_i = 1;
}
```

## Further Resources

For detailed information on the DOC5503 and its testing, refer to:

- `DOC5503_Testing_Notes_Updated.md` - Latest findings and recommendations
- `DOC5503_Oscillator_Halting_Issue.md` - Analysis of the oscillator halt issue
- `DOC5503_Polarity_Issue_Summary.md` - Analysis of the polarity inversion issue
- `doc5503_volume_wave_test_workaround.cpp` - Example test with diagnostic code

Remember: The way you test the audio path is crucial. If the test is not properly set up, it's nearly impossible to diagnose issues in the audio processing code.