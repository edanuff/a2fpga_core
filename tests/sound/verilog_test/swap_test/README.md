# DOC5503 Swap Mode Test

This directory contains a test for the DOC5503 sound chip's swap mode functionality.

## What is Swap Mode?

In the DOC5503, swap mode (mode=11 or 0x3) is a special oscillator mode where oscillator pairs take turns playing when one overflows. Oscillator pairs are even-odd pairs (0-1, 2-3, etc.) that alternate in swap mode.

When an oscillator's accumulator overflows, it triggers a swap - the current oscillator halts, and its partner activates. This continues back and forth as each accumulator overflows.

## Test Details

This test configures:
- Oscillators 0 and 1 in swap mode
- Oscillator 0 initially active, oscillator 1 initially halted
- Both oscillators with the same frequency (0x0100)
- Wavetable resolution and size set to 000 (standard resolution, 256 bytes)
- Oscillator wave data is always 0x80 (mid-level) to avoid zero byte halt

## Running the Test

To run the test:

```bash
make
make run
```

This will build and execute the test, generating a VCD file that can be viewed in GTKWave.

## Analyzing Results

To analyze the swap behavior, use the provided script which automatically loads the important signals in GTKWave:

```bash
./view_swap_mode.sh
```

Or you can manually open the VCD file and select signals:

```bash
gtkwave ./obj_dir/doc5503_verilog_swap_test.vcd
```

When viewing the waveform, you should look for these key signals:

1. **Accumulator values**:
   - `dut.osc_acc_r[0]` - Oscillator 0's accumulator
   - `dut.osc_acc_r[1]` - Oscillator 1's accumulator

2. **Halt status**:
   - `dut.osc_halt_o[0]` - Oscillator 0's halt status
   - `dut.osc_halt_o[1]` - Oscillator 1's halt status

3. **Mode and control signals**:
   - `dut.curr_mode_w` - Should be 2'b11 for SWAP mode
   - `dut.swap_w` - Swap activation signal
   - `dut.overflow_w` - Accumulator overflow indicator
   - `dut.curr_halt_r` and `dut.partner_halt_r` - Current and partner halt status

In GTKWave, you should see an alternating pattern of oscillator activations as the swap occurs. Specifically, when one oscillator's accumulator overflows, its halt status should change from 0 to 1, and its partner's halt status should change from 1 to 0.

## Expected Swap Behavior

The expected behavior is:

1. Oscillator 0 starts active, increments its accumulator
2. When oscillator 0's accumulator overflows, it triggers a swap:
   - Oscillator 0 halts (halt status = 1)
   - Oscillator 1 activates (halt status = 0)
3. Oscillator 1 now increments its accumulator
4. When oscillator 1's accumulator overflows, it triggers a swap back:
   - Oscillator 1 halts (halt status = 1)
   - Oscillator 0 activates (halt status = 0)
5. This pattern continues indefinitely

The swap behavior is a key feature for certain sound effects in the Apple IIgs, allowing for more complex sounds with the limited oscillator resources.