# DOC5503 Testing Notes and Information (Updated)

## Latest Findings (April 2025)

We've identified a critical issue with the DOC5503 oscillator implementation that prevents all test code from producing audio output. This document provides a summary of our findings and recommended solutions.

### Critical Issue: Oscillator Halting During Initialization

After extensive debugging, we've discovered that all oscillators are being halted during initialization due to a specific condition in the `doc5503.sv` module:

1. During initialization, the waveform data sample register (`wds_w`) starts with a value of `0x00`.

2. The oscillator state machine detects this as a "zero byte" which triggers a halt condition:
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

3. Once the oscillator enters the `OSC_HALT` state, the halt bit in the control register is set, and no amount of writing to this register can clear it because the internal FSM keeps re-asserting the halt:
   ```verilog
   current_osc_halt_req_r <= 1;  // halt current oscillator
   ```

### Diagnostic Evidence

Through systematic testing, we've confirmed:

1. **Initial Control Register State**: After writing `0x00` to the control register (`0xA0`), it correctly reads back as `0x00` (not halted).

2. **Post-Initialization State**: After initialization cycles, the control register changes to `0x01` (halted) without explicit request.

3. **Unable to Un-halt**: Repeated attempts to write `0x00` to the control register fail to clear the halt bit.

4. **No Wave Reads**: Due to the halted state, no wave read requests (`wave_rd_o`) are ever generated.

5. **No Audio Output**: With no wave reads occurring, no audio is produced in any of the output channels.

### Detailed Analysis

The issue is in the oscillator state machine's detection of zero bytes in the `OSC_READ_DATA` state:

```verilog
// Around line 880-896 in doc5503.sv
automatic logic zero_byte_w = (wds_w == 8'h00);

osc_state_r <= OSC_OUT;
// ...
if (overflow | zero_byte_w) begin
    current_acc_reset_req_r <= 1;
    if (zero_byte_w | mode_w[0]) begin  // zero byte halts oscillator
        osc_state_r <= OSC_HALT;
    end  
    // ...
end
```

During the first initialization cycle, the waveform data sample register (`wds_w`) has a value of `0x00`, which triggers the halt condition. The oscillator enters the `OSC_HALT` state and sets the halt bit in the control register, effectively stopping all oscillator activity.

### Recommended Solutions

Based on our analysis, we recommend one of the following fixes:

#### Option 1: Initialize WDS to Non-Zero Value

Change the initialization value in the `doc5503_register_ram` module for the waveform data sample register:

```verilog
// In doc5503_register_ram for wds_w (around line 350)
.priority_write_data_i('{
    8'h80,  // Change from 8'h00 to 8'h80 for initial value
    8'h80,
    wave_data_i,
    data_i
}),
```

This ensures that the initial value is the zero-crossing point for signed audio (`0x80`) rather than a zero value that triggers the halt condition.

#### Option 2: Add Initialization Phase Flag

Add logic to only halt the oscillator if a zero byte is encountered during normal running, not during initialization:

```verilog
// Add new register
reg init_phase_r = 1;

// In initialization section
if (ready_r) init_phase_r <= 0;

// Modify halt condition (around line 895)
if ((zero_byte_w & !init_phase_r) | mode_w[0]) begin
    osc_state_r <= OSC_HALT;
end
```

#### Option 3: Add External Control Override

For testing purposes, add a simulation-only register that allows test code to forcibly unhalt oscillators:

```verilog
// Add to module parameters
parameter bit ENABLE_FORCE_UNHALT = 0,

// Add new register
reg force_unhalt_r = 0;

// In register handling
if (ENABLE_FORCE_UNHALT && addr_i == 8'hFF && !cs_n_i && !we_n_i) begin
    force_unhalt_r <= data_i[0];
end

// Modify halt condition
if ((zero_byte_w & !force_unhalt_r) | mode_w[0]) begin
    osc_state_r <= OSC_HALT;
end
```

### Impact on Testing

This issue explains why all previous test code was unable to produce audio output, including:

1. The volume scaling tests
2. Wavetable resolution tests
3. Oscillator mode tests
4. The newly implemented audio compressor tests

Once this issue is fixed, all these tests should be able to produce audio output, allowing proper validation of the DOC5503 implementation and the audio compressor.

### Workaround for Testing

We've created a workaround test (`doc5503_volume_wave_test_workaround.cpp`) that attempts to bypass the halt issue by:

1. Setting non-zero wave values for all addresses
2. Periodically attempting to unhalt the oscillator
3. Providing detailed diagnostics on oscillator state

However, even this workaround cannot produce audio because the oscillator is completely halted at the FSM level and never issues wave read requests.

### Relevant Documentation

For more detailed information, please see:
- `DOC5503_Oscillator_Halting_Issue.md` - Comprehensive analysis of the halt issue
- Previous `DOC5503_Testing_Notes.md` - General DOC5503 testing information

## Conclusion

The DOC5503 oscillator implementation has a specific issue in the initialization sequence that causes all oscillators to remain halted. This is a fundamental issue that must be fixed before any audio output can be tested or verified. We recommend implementing one of the suggested solutions to allow oscillators to run properly.