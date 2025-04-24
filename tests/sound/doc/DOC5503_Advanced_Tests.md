# DOC5503 Advanced Testing Plan

Based on the analysis of the NINJATRACKER.S player, we should implement the following test cases to more thoroughly verify the DOC5503 implementation.

## 1. Complex Oscillator Control Register Tests

The NinjaTracker code demonstrates sophisticated use of the control register (lines 1414-1422, 1188-1189), showing we should test:

- **Interrupts Handling**
  - Verify that bit 3 (Interrupt Enable) works correctly
  - Test that when a wavetable ends, interrupts are generated properly
  - Confirm that the interrupt register (0xE0) correctly shows which oscillator triggered the interrupt

- **Channel Assignment**
  - Test all 16 possible channel assignments (bits 7-4)
  - Verify correct stereo output based on channel assignment
  - Particularly test using the right/left output routing (channel 0 → right, channel 1 → left)

- **Halt Bit Behavior**
  - Verify that setting bit 0 immediately halts oscillator
  - Test that clearing bit 0 correctly restarts oscillator
  - Verify that oscillator halts when encountering 0x00 in the wavetable

## 2. Oscillator Mode Testing

The NinjaTracker code has specific implementations for all oscillator modes (lines 1288-1294, etc.). We should test:

- **Free Run Mode (M1=0, M0=0)**
  - Verify oscillator loops continuously
  - Test that it stops when encountering 0x00 in the wavetable or when halted by software

- **One Shot Mode (M1=0, M0=1)**
  - Verify oscillator plays through wavetable once and then halts
  - Test with various wavetable sizes

- **Sync/AM Mode (M1=1, M0=0)**
  - For odd/even pairs:
    - Test AM (Amplitude Modulation) when lower oscillator is odd
    - Verify volume register is ignored for even oscillator in AM mode
  - For even/odd pairs:
    - Test Sync mode when lower oscillator is even
    - Verify that when first oscillator wraps, second oscillator is reset

- **Swap Mode (M1=1, M0=1)**
  - Test that when oscillator completes, it halts and starts its partner
  - Verify the partner oscillator's accumulator is reset to zero
  - Test with multiple oscillator pairs

## 3. Wave Data Access Tests

The NinjaTracker code shows detailed wave pointer management (lines 1349-1394):

- **Wavetable Addressing**
  - Test all wavetable sizes (256, 512, 1024, 2048, 4096, 8192, 16384, 32768 bytes)
  - Verify address wrapping behavior at different resolutions
  - Test bank select bit behavior (bit 6 of resolution register)

- **Resolution Settings**
  - Test all resolution settings (0-7)
  - Verify frequency scaling with different resolution settings
  - Test interaction between table size and resolution

## 4. Advanced Effects Tests

The NinjaTracker code implements several audio effects that stress the DOC chip:

- **Portamento (Frequency Sliding)**
  - Test smooth sliding between notes/frequencies
  - Verify slide up/down behavior matches expected curve

- **Vibrato**
  - Test oscillating frequency variations
  - Verify different vibrato depths and speeds

- **Volume Sliding**
  - Test smooth volume transitions up/down
  - Verify volume scale is accurate

- **Arpeggios**
  - Test rapid note switching that creates chord effects
  - Verify timing accuracy

## 5. Multiple Oscillator Tests

Based on the NinjaTracker's use of multiple oscillators:

- **Oscillator Enable Register (0xE1)**
  - Test enabling 1, 2, 4, 8, 16, and 32 oscillators
  - Verify timing and cycle handling
  - Test the fix for the off-by-one error in the oscillator count

- **Mixer Load Testing**
  - Test all 32 oscillators running simultaneously
  - Verify audio mixing with multiple channels
  - Test output level when all oscillators are at maximum volume

## 6. Cycle Timing Tests

The NinjaTracker code shows critical timing considerations:

- **Cycle Accuracy**
  - Verify 8 cycle steps per oscillator (except last oscillator with 24 steps)
  - Test interrupt timing when multiple oscillators are enabled
  - Verify clock division behavior (7.15909MHz / 8 = 894,886Hz)

- **Sample Rate Effects**
  - Test the effect of oscillator count on effective sample rate
  - Verify frequency calculation accuracy across different oscillator counts

## 7. Edge Case Tests

Based on the code's error handling:

- **Boundary Conditions**
  - Test what happens when frequency values are at minimum/maximum
  - Verify wavetable wrapping at page boundaries
  - Test accumulator overflow conditions

- **Special Values**
  - Test behavior with 0x00 (halt) values in different positions in wavetable
  - Verify 0x80 (silence) handling
  - Test behavior with extreme volume settings

## Implementation Plan

For each of these test categories:

1. Start with simple test cases that verify basic functionality
2. Add more complex test cases that exercise edge cases
3. Create tests that closely mimic real-world music playback

The test harness should be enhanced to:

1. Record internal state at each step
2. Support side-by-side comparison with C++ model
3. Generate waveform visualizations for audio output
4. Validate correct timing of state transitions

The NINJATRACKER code provides valuable insights into real-world usage patterns that we should replicate in our test suite to ensure the DOC5503 implementation is robust for all applications.