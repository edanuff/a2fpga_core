# DOC5503 Advanced Test Suite

This directory contains a set of advanced tests for the DOC5503 sound chip implementation, inspired by analyzing real-world usage patterns in NINJATRACKER.S.

## Test Files

### doc5503_mode_test.cpp
Tests all four oscillator modes of the DOC5503:
1. **Free Run Mode** - Oscillator plays continuously
2. **One Shot Mode** - Oscillator plays through a wavetable once and halts
3. **Sync/AM Mode** - Tests amplitude modulation behavior with oscillator pairs
4. **Swap Mode** - Tests sequential playback between paired oscillators

The test generates separate WAV files for each mode to allow for audio analysis.

### doc5503_interrupt_test.cpp
Tests the DOC5503's interrupt generation capabilities:
1. **Interrupt Generation** - Verifies that interrupts are generated when oscillators halt
2. **Interrupt Clearing** - Verifies that interrupts can be cleared by writing to the interrupt register
3. **Multiple Oscillator Interrupts** - Tests which oscillator triggers an interrupt first when multiple are enabled

### doc5503_wavetable_resolution_test.cpp
Tests DOC5503's wavetable handling and resolution settings:
1. **Table Size Test** - Tests all 8 possible table sizes (256 to 32768 bytes) with fixed resolution
2. **Resolution Test** - Tests all 8 possible resolution settings with fixed table size
3. **Matched Settings** - Tests when table size and resolution are set to the same value (recommended usage)

## Running the Tests

Build and run the tests using the Makefile in the parent directory:

```
# Build all tests
make doc_tests

# Run specific tests
make run_mode_test
make run_interrupt_test
make run_wavetable_resolution_test
```

## Expected Output

Each test generates diagnostic information to the console and creates WAV files with the test results. These audio files can be analyzed to verify the DOC5503's functionality.

The mode test creates files like `doc_mode_free_run.wav` and `doc_mode_one_shot.wav`.
The wavetable resolution test creates files like `doc_tsize_0.wav` and `doc_res_0.wav`.

## Requirements

- Verilator for simulation
- C++ compiler supporting C++17
- An audio player capable of playing WAV files to analyze the results

## Implementation Details

These tests focus on verifying the complex oscillator behavior that was observed in the NINJATRACKER.S code, including proper handling of:
- Oscillator modes
- Interrupt generation and clearing
- Wavetable addressing with different resolutions
- Multi-oscillator effects