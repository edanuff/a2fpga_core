# DOC5503 Swap Mode Test Results

## Test Configuration

This test has been set up to demonstrate the DOC5503's swap mode functionality with the following parameters:

1. **Oscillator Configuration:**
   - Oscillator 0: Started in active mode (not halted)
   - Oscillator 1: Started in halted mode
   - Both oscillators configured with:
     - Mode = 11 (SWAP mode)
     - Frequency = 0x0100 (1 byte from high register, 0 from low)
     - Resolution = 000 (standard resolution)
     - Table size = 000 (256 bytes)
     - Volume = 0xFF (maximum volume)

2. **Wavetable Data:**
   - All wavetable reads return 0x80 (mid-level, signed 0)
   - This prevents any zero-byte halt conditions

## What to Look For in the Waveform

The test runs for 40,000 clock cycles, which should be long enough to observe multiple swap events. When analyzing the VCD file, focus on:

### Primary Signals for Swap Behavior

1. **Halt Status:**
   - `dut.osc_halt_r[0]` - Oscillator 0's halt status
   - `dut.osc_halt_r[1]` - Oscillator 1's halt status
   - In swap mode, you should see these alternate between 0 (active) and 1 (halted)

2. **Accumulators:**
   - `dut.osc_acc_r[0]` - Oscillator 0's accumulator
   - `dut.osc_acc_r[1]` - Oscillator 1's accumulator
   - The active oscillator's accumulator should increment, while the halted one remains static

3. **Trigger Signals:**
   - `dut.overflow_w` - Indicates an accumulator overflow
   - `dut.swap_w` - Indicates a swap event triggered

4. **Current Oscillator:**
   - `dut.curr_osc_r` - The current oscillator being processed
   - `dut.curr_halt_r` - The halt status of the current oscillator
   - `dut.partner_halt_r` - The halt status of the partner oscillator

### Expected Sequence of Events

1. **Initial State:** 
   - Oscillator 0: Active (`osc_halt_r[0] = 0`) 
   - Oscillator 1: Halted (`osc_halt_r[1] = 1`)

2. **Oscillator 0 Running:**
   - `osc_acc_r[0]` increments on each cycle when `clk_en_i` is high
   - `osc_acc_r[1]` stays constant

3. **First Swap Event (First Overflow):**
   - When `osc_acc_r[0]` overflows, `overflow_w` should go high
   - `swap_w` should go high
   - `osc_halt_r[0]` should change from 0 to 1 (halting osc 0)
   - `osc_halt_r[1]` should change from 1 to 0 (activating osc 1)

4. **Oscillator 1 Running:**
   - `osc_acc_r[1]` increments on each cycle when `clk_en_i` is high
   - `osc_acc_r[0]` stays constant

5. **Second Swap Event:**
   - When `osc_acc_r[1]` overflows, the process repeats in reverse

This cycle of swapping should continue throughout the simulation, with each oscillator taking turns playing.

## Notes on DOC5503 Swap Mode Implementation

The DOC5503 swap mode functionality is implemented in the DOC5503 module with these key components:

1. **Partner Selection:**
   - Even oscillators (0,2,4,6) pair with odd oscillators (1,3,5,7)
   - The partner is always the oscillator with the opposite LSB

2. **Swap Triggering:**
   - Swaps occur when an accumulator overflows (typically at the end of a wavetable)
   - The overflow is detected when comparing with the `curr_acc_mask_w` based on resolution

3. **Swap Logic:**
   - Located in the DOC5503 module's oscillator update block around line 960-970
   - When a swap occurs, the current oscillator is halted and its partner is activated

## Understanding Resolution and Overflow

In the DOC5503, the accumulator overflow point depends on the resolution setting:

- Resolution 000: Overflows after 65,536 samples (16-bit)
- Resolution 001: Overflows after 131,072 samples (17-bit)
- Resolution 010: Overflows after 262,144 samples (18-bit)
- And so on...

Since we're using resolution 000, the accumulator should overflow when it reaches 0x10000 (65,536).

## Troubleshooting

If swap behavior is not visible:

1. Ensure the simulation ran long enough (the default of 40,000 cycles should be sufficient)
2. Verify that both oscillators are enabled in the OSC_EN register (0xE1 = 0x02)
3. Confirm that both oscillators are set to SWAP mode (0xA0 = 0x06, 0xA1 = 0x07)
4. Check the frequency settings - higher values will cause overflow to happen sooner