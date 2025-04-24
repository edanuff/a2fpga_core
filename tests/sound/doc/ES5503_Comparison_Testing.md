# ES5503/DOC5503 Comparison Testing

This document describes the standalone ES5503 emulator and comparison testing infrastructure for the DOC5503 SystemVerilog implementation.

## Overview

The comparison test infrastructure allows validating the FPGA implementation of the DOC5503 chip against a software reference implementation based on the MAME emulator's ES5503 code. The comparison helps identify potential behavioral differences and bugs in the FPGA implementation.

## Components

The comparison testing system consists of the following components:

1. **es5503_standalone.h/cpp**: A standalone C++ implementation of the ES5503 chip derived from the MAME emulator code. This implementation can be used independently of the MAME framework.

2. **doc5503_comparison_test.cpp**: A test program that configures both the FPGA implementation (via Verilator) and the standalone ES5503 emulator with identical settings, runs a simulation, and compares the audio outputs.

## Comparison Testing

The comparison test works as follows:

1. It initializes both the FPGA DOC5503 implementation and the software ES5503 emulator with identical settings (oscillator count, frequency, volume, wavetable size/resolution, etc.).

2. It generates a synthetic wavetable (sine wave by default) shared by both implementations.

3. It runs a simulation for a specified duration, collecting audio samples from both implementations.

4. It writes the audio outputs to separate WAV files for manual inspection.

5. It compares the audio outputs and reports statistics on the differences (maximum error, average error, percentage of samples exceeding an error threshold).

## Running the Tests

To build and run the comparison test:

```bash
# Build the test
make comparison_tests

# Run the test
make run_comparison_test
```

### Command-Line Options

The comparison test accepts the following command-line options:

- `--oscs <num>`: Number of oscillators to enable (default: 31)
- `--mode <mode>`: Oscillator mode (0=free, 1=oneshot, 2=syncam, 3=swap, default: 0)
- `--size <size>`: Wavetable size (0-7 corresponding to 256-32768 samples, default: 3 = 2048)
- `--res <res>`: Wavetable resolution (0-7 corresponding to 8-15 bits, default: 0 = 8-bit)
- `--freq <freq>`: Oscillator frequency value (default: 1024)
- `--vol <vol>`: Volume (0-255, default: 255)
- `--duration <ms>`: Test duration in milliseconds (default: 1000)
- `--nocompare`: Disable output comparison
- `--threshold <val>`: Error threshold for comparison (default: 0.05)

Example:
```bash
./doc5503_comparison_test --oscs 16 --mode 0 --freq 2048 --duration 2000
```

## Using the ES5503 Standalone Implementation

The ES5503 standalone implementation can be used independently for other testing or integration purposes:

```cpp
#include "es5503_standalone.h"

// Create a 64K wave memory
std::vector<uint8_t> wave_memory(65536, 0);

// Fill wave memory with your wavetable data
// ...

// Create an ES5503 instance with a 14.31818 MHz clock
ES5503 es5503(14318180, wave_memory.data(), wave_memory.size());

// Configure the chip
es5503.set_channels(2);  // Stereo output
es5503.reset();

// Configure oscillators
es5503.write(0xE1, 62);  // Enable all 32 oscillators

// Configure oscillator 0
es5503.write(0x00, 0x80);          // Frequency low byte
es5503.write(0x20, 0x01);          // Frequency high byte
es5503.write(0x40, 0xFF);          // Volume
es5503.write(0x80, 0x00);          // Wavetable pointer
es5503.write(0xC0, 0x18);          // Wavetable size (3 = 2048) and resolution (0 = 8-bit)
es5503.write(0xA0, 0x00);          // Control register (free-running mode, channel 0)

// Generate audio samples
std::vector<int16_t> output_buffer(1024);
es5503.generate_audio(output_buffer.data(), output_buffer.size());
```

## Known Issues and Differences

The ES5503 implementation in the MAME emulator documents several specific behaviors of the original hardware:

1. The uppermost enabled oscillator contributes to the output 3 times. This is why the Apple IIgs system software doesn't use oscillators 30 and 31.

2. In "swap" mode, there's one cycle when the swap takes place where the oscillator's output is 0x80 (centerline) regardless of the sample data.

3. There's an off-by-one error in the oscillator enable register handling that was fixed in MAME version 2.1.3.

These behaviors are accurately modeled in the standalone ES5503 implementation, allowing them to be compared against the FPGA implementation.

## Debugging Tips

1. Examine the generated WAV files to visually inspect differences in audio output.

2. Use a spectral analyzer to identify frequency-domain differences that may not be apparent in the time domain.

3. Test with different oscillator counts, especially around the known problematic values (30 and 31).

4. Test with different oscillator modes, particularly the swap and sync modes which have more complex interactions.

5. If you find differences, try simplifying the test case (reducing oscillator count, using simpler wavetables) to isolate the issue.

## Future Improvements

Potential enhancements to the comparison testing infrastructure:

1. Implement more detailed register-level state comparison between the implementations.

2. Add support for testing IRQ generation timing and accuracy.

3. Add automated regression testing with a suite of test cases covering all operational modes.

4. Extend the test to support real wavetable data from the Apple IIgs.

5. Add visualization tools to better illustrate differences between implementations.