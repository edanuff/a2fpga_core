# DOC5503 Oscillator Halting Issue

## Problem Summary

The DOC5503 sound module oscillators appear to become halted during initialization and cannot be unhalted through normal register writes. This is preventing any audio output from being generated during testing.

## Diagnostic Evidence

Through extensive testing, we've observed the following behavior:

1. **Initial Control Register State**: After writing 0x00 to the control register (0xA0), it correctly reads back as 0x00 (not halted).

2. **Post-Initialization State**: After initialization cycles, the control register changes to 0x01 (halted) without explicit request.

3. **Unable to Un-halt**: Repeated attempts to write 0x00 to the control register fail to clear the halt bit.

4. **No Wave Reads**: Due to the halted state, no wave read requests (wave_rd_o) are ever generated.

5. **Affected Registers**: The oscillator's volume register initially reads as 0x00 despite being set to 0xFF, though this appears to correct itself later.

## Root Cause Identified

After extensive debugging, we've identified the specific cause of the oscillator halting issue:

1. During initialization, the waveform data sample register (`wds_w`) starts with a value of 0x00.

2. In the `OSC_READ_DATA` state, there's a halt condition (lines 880-896):
   ```verilog
   automatic logic zero_byte_w = (wds_w == 8'h00);
   
   // ...
   
   if (overflow | zero_byte_w) begin
       current_acc_reset_req_r <= 1;                                   // reset accumulator
       if (zero_byte_w | mode_w[0]) begin                              // zero byte halts oscillator
           osc_state_r <= OSC_HALT;
       end  
       // ...
   end
   ```

3. This code is causing the oscillator to enter the `OSC_HALT` state during the first cycle because the initial `wds_w` value of 0x00 triggers the `zero_byte_w` condition.

4. In the `OSC_HALT` state, the oscillator is halted with:
   ```verilog
   current_osc_halt_req_r <= 1;  // halt current oscillator
   ```

5. Once halted, the oscillator's control register has its halt bit set, and subsequent write attempts are ineffective at clearing it because the internal FSM keeps re-asserting the halt request.

## Verification Methods

1. Created an enhanced diagnostic test that:
   - Explicitly tries to un-halt the oscillator multiple times
   - Verifies register values after each write
   - Tracks control register state through the execution cycle
   - Follows oscillator state through initialization

2. Verified that no audio output occurs because no wave reads are requested.

## Recommended Solution

Based on our root cause analysis, we recommend implementing one of the following fixes:

### Option 1: Initialize WDS register to non-zero value

In the initialization section (around line 165-177), change:
```verilog
// In the doc5503_register_ram module for the wds register
.priority_write_data_i('{
    8'h80,                  // Change this value from 8'h00 to 8'h80
    8'h80,
    wave_data_i,
    data_i
}),
```

This ensures that the initial waveform data sample value is 0x80 (zero crossing for signed audio) instead of 0x00, which triggers a halt.

### Option 2: Modify Zero Byte Detection Logic

Modify the zero byte condition (around line 893-897) to only halt the oscillator if in a specific state:
```verilog
if (overflow | zero_byte_w) begin
    current_acc_reset_req_r <= 1;                                   // reset accumulator
    if ((zero_byte_w & !reset_phase_r) | mode_w[0]) begin           // only halt if zero byte encountered during normal running
        osc_state_r <= OSC_HALT;
    end  
    // ...
end
```

Add a new register `reset_phase_r` that is set during initialization and cleared after first successful wave read.

### Option 3: Add External Control to Override Halt

Implement a simulation-only register that allows test code to forcibly unhalt the oscillator regardless of internal conditions:
```verilog
// In the module parameter list
parameter bit ENABLE_FORCE_UNHALT = 0,

// In the register handling section
if (ENABLE_FORCE_UNHALT && !cs_n_i && !we_n_i && addr_i == 8'hFF) begin
    force_unhalt_r <= data_i[0];
end

// In the halt condition
if ((zero_byte_w & !force_unhalt_r) | mode_w[0]) begin
    osc_state_r <= OSC_HALT;
end
```

This implementation change keeps the normal DOC5503 behavior intact while adding a special test mode to override the halt state.

## Related Tests

- `doc5503_unhalt_test.cpp`: An existing test that specifically tries to un-halt oscillators
- `doc5503_volume_wave_test.cpp`: Our enhanced diagnostic test that verifies volume and waveform handling
- `doc5503_debug_test.cpp`: May provide additional insight into the initialization flow

## Impact

This issue is preventing any audio output from being produced during testing, making it impossible to verify other aspects of the audio pipeline, including the recently implemented audio compressor.