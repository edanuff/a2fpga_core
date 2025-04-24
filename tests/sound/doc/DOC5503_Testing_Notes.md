# DOC5503 Testing Notes and Information

---

## Recent Test Results (April 23, 2025)

### Issues Diagnosed

After extensive testing of the DOC5503 Verilog implementation, we've identified and fixed several key issues with the test harness that were preventing proper audio output:

1. **Oscillator Control Register Understanding**
   - The control register bit 0 is the halt bit (0=run, 1=halt)
   - The Verilog implementation correctly processes the control register settings
   - Our test code properly sets this to 0 to allow oscillators to run

2. **Channel Mapping**
   - Control register bits 7-4 determine the output channel
   - 0x00 sets channel 0 (right output)
   - 0x10 sets channel 1 (left output)
   - In our successful test, we confirmed audio output on the right channel when bits 7-4 were set to 0x0

3. **Wavetable Data**
   - Ensure wavetable data never contains 0x00 values, as these will halt oscillators
   - Our sawtooth wave test used 0x81-0xFF values to ensure proper oscillation
   - The wavetable is properly read in the simulation

4. **Test Harness Configuration**
   - The simplified harness properly connects the DOC5503 signals
   - Wave address and read signals must be properly connected
   - Providing wave data immediately when requested is crucial

### Current Status

The DOC5503 Verilog implementation is correctly producing audio in the simulation environment.
Key findings include:

1. **Working Configuration:**
   - Oscillator channel set to 0 (right channel)
   - Control register: 0x00 (0000 0000) - Channel 0, not halted
   - Frequency registers: High 0x0F, Low 0xFF for clear audible output
   - Volume register: 0xFF (maximum)
   - Table size/resolution: 0x00 (256-byte table)

2. **Debug Traces Show:**
   - Wavetable addresses properly advancing
   - Correct wavetable data being read
   - Control register properly set (not halted)
   - Right channel audio output produced correctly

3. **Wave Output:**
   - Right channel produces proper non-zero audio output
   - Left channel is silent (as expected, since we routed to right channel)
   - The waveform reflects our sawtooth test pattern

### Future Testing Recommendations

1. **Test with Different Oscillator Modes:**
   - FREE RUN mode (0x00) - Current successful test
   - ONE SHOT mode (0x02)
   - SYNC/AM mode (0x04)
   - SWAP mode (0x06)

2. **Test Multiple Oscillators:**
   - Enable more than one oscillator (set 0xE1 to higher values)
   - Test with oscillators routed to different channels
   - Test paired oscillator modes (SYNC/AM and SWAP)

3. **Test Different Frequencies and Volumes:**
   - Very low frequencies
   - Very high frequencies
   - Different volume levels to test amplitude scaling

4. **Test Different Wavetables:**
   - Various waveform shapes (sine, square, etc.)
   - Different table sizes (512, 1024, etc.)
   - Different resolutions

### Comparison with C++ Model

When comparing the Verilog DOC5503 implementation with the C++ ES5503 model:

1. Both implementations correctly process control registers
2. Both implementations handle wavetable addressing correctly
3. Both implementations can produce audio output
4. Need to verify that the same control register settings produce identical audio output

### Conclusion

The DOC5503 Verilog implementation works correctly in simulation when properly configured. Future work should focus on:

1. Creating a comprehensive test suite to verify all oscillator modes and features
2. Enhancing the test harness to better expose internal state for debugging
3. Performing side-by-side testing with the C++ model to ensure identical audio output
4. Testing edge cases and different configurations

---

## Overview of the DOC5503 Sound Chip

The DOC5503 (Digital Oscillator Chip) is a 32-voice polyphonic sound engine used in the Apple IIgs and various Ensoniq synthesizers. It's a complex digital oscillator system that uses wavetable synthesis to generate sound.

### Key Features

1. **32 Digital Oscillators** - Each can be individually configured
2. **Wavetable Synthesis** - Uses memory-resident waveforms
3. **Multiple Oscillator Modes** - Free-running, one-shot, sync/AM, and swap modes
4. **Flexible Resolution** - Adjustable wavetable resolutions
5. **Variable Table Size** - Wavetable sizes from 256 bytes to 32KB
6. **Multi-Channel Output** - Can route oscillators to different output channels
7. **IRQ Generation** - For oscillator completion notification

## Register Map

The DOC5503 uses a set of 229 registers (8 bits each) for configuration:

### Per-Oscillator Registers (32 oscillators × 7 register types = 224 registers)

| Register Range | Description                 | Notes                                 |
|----------------|-----------------------------|---------------------------------------|
| $00-$1F        | Frequency Low               | Low 8 bits of frequency value         |
| $20-$3F        | Frequency High              | High 8 bits of frequency value        |
| $40-$5F        | Volume                      | 8-bit volume (0-255)                  |
| $60-$7F        | Waveform Data Sample (RO)   | Last read value from waveform table   |
| $80-$9F        | Waveform Table Pointer      | Starting page of waveform table       |
| $A0-$BF        | Control Register            | Mode, halt, channel assignment, IRQ   |
| $C0-$DF        | Resolution/Table Size       | Waveform resolution and table size    |

### Global Registers

| Register | Description              | Notes                                 |
|----------|--------------------------|---------------------------------------|
| $E0      | Oscillator Interrupt Reg | Interrupts for oscillator completion  |
| $E1      | Oscillator Enable Reg    | Number of active oscillators          |

### Control Register Format ($A0-$BF)

| Bits | Description      | Values                                    |
|------|------------------|-------------------------------------------|
| 0    | Halt bit         | 0=Running, 1=Halted                       |
| 1-2  | Mode             | 00=Free, 01=One-shot, 10=Sync/AM, 11=Swap |
| 3    | Interrupt Enable | 0=Disabled, 1=Enabled                     |
| 4-7  | Channel Assign   | Output channel assignment (0-15)          |

### Resolution/Table Size Register Format ($C0-$DF)

| Bits | Description  | Values                                    |
|------|--------------|-------------------------------------------|
| 0-2  | Resolution   | 000=Bits 1-16 through 111=Bits 8-23       |
| 3-5  | Table Size   | 000=256B through 111=32KB                 |
| 6    | Bank Select  | 0=Lower 64K, 1=Upper 64K                  |
| 7    | Reserved     | Should be 0                               |

## Oscillator Enable Register ($E1)

**NOTE**: The original documentation has a discrepancy here that caused an off-by-one error in our implementation.

**Correct Behavior**:
- Bits 5-1 represent (number of oscillators - 1)
- When set to 0, only 1 oscillator is enabled
- When set to 31 (all bits set), all 32 oscillators are enabled

**Setting the Register**:
- To enable N oscillators, set bits 5-1 to (N-1)
- For example, to enable all 32 oscillators, set bits 5-1 to 31 (value 0x3E)

## Oscillator Modes

### Free-running Mode (00)
The oscillator continuously loops through the waveform table until halted or a zero value is encountered.

### One-shot Mode (01)
The oscillator plays through the waveform once, then halts at the end.

### Sync/AM Mode (10)
Uses pairs of oscillators with special behaviors:
- **AM Mode**: When the lower oscillator is odd, it amplitude-modulates the even oscillator.
- **Sync Mode**: When the lower oscillator is even, it synchronizes the odd oscillator's phase.

### Swap Mode (11)
Uses pairs of oscillators where the lower is even, higher is odd:
- The oscillator runs in one-shot mode
- When completed, it resets and triggers the next oscillator
- Allows for complex chained sounds

## Timing and Processing

The DOC5503 divides the input clock by 8 for each oscillator cycle. In the Apple IIgs, the clock is 7.15909 MHz, giving an effective maximum frequency of 894,886.25 Hz per oscillator.

Each oscillator gets 8 clock cycles for processing, except the last oscillator which gets 24 cycles (related to RAM refresh in the original hardware).

## Important Testing Considerations

1. **Oscillator Enable Register ($E1) Correction**:
   - The implementation must handle bits 5-1 as the actual oscillator count minus one
   - This needs thorough testing with different oscillator counts

2. **Mixing Logic Synchronization**:
   - The mixer FSM should sync properly with the oscillator FSM
   - Test with different numbers of enabled oscillators (1, 16, 31, 32)

3. **Oscillator Modes**:
   - Test all four modes, especially the paired modes (Sync/AM and Swap)
   - Verify oscillator behavior when encountering zero in waveform

4. **Resolution and Table Size**:
   - Test different combinations of resolution and table size
   - Verify correct memory addressing for each combination

5. **Audio Output Quality**:
   - Test with various volumes and wavetables
   - Look for any timing-related audio artifacts

## Known Issues to Watch For

1. **Off-by-one Error in $E1 Register**:
   - When all 32 oscillators are enabled (value 0x3E), some implementations may only process 31
   - Test both 0x3E and 0x3D to ensure correct behavior

2. **Mixer Timing Issues**:
   - With 31+ oscillators enabled, there may be synchronization issues with the mixer
   - Watch for periodic dips or noise in the audio output

3. **Oscillator Counting Logic**:
   - The cycle_max_w calculation needs to correctly handle the full range of oscillator counts
   - Test edge cases (1 oscillator, 32 oscillators)

## Testing Goals

1. Verify correct implementation of all registers and modes
2. Ensure all 32 oscillators can be properly enabled and utilized
3. Validate audio output quality with different configurations
4. Test boundary conditions and timing edge cases
5. Verify proper handling of the $E1 register and oscillator counting