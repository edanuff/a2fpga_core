# Sound Module Testing Suite

This directory contains a comprehensive testing suite for the Apple II FPGA sound components, with a focus on the DOC5503 (Ensoniq Digital Oscillator Chip) implementation.

## Directory Structure

- `build/` - Build artifacts and compiled binaries
- `doc/` - Documentation and test plans
- `harness/` - Test harnesses for Verilog modules
- `output/` - Test output files (WAV files, logs)
- `reference/` - Reference implementations (C++ ES5503)
- `testcases/` - Test case implementations
  - `doc5503/` - DOC5503 specific tests
  - `glu/` - Sound GLU specific tests
- `tools/` - Utility tools for analysis and visualization

## Main Components

### DOC5503 Testing

The DOC5503 (Digital Oscillator Chip) is the main sound component for the Apple IIgs, providing 32-voice polyphonic sound. Tests include:

- Basic oscillator functionality
- Multi-oscillator configurations
- Wavetable synthesis validation
- Different oscillator modes (Free Run, One Shot, Sync/AM, Swap)
- Volume and frequency testing

### Sound GLU Testing

Tests for the Sound GLU (Glue Logic Unit) that interfaces between the sound chip and system bus.

## Running Tests

Use the provided Makefile targets to run specific tests:

```bash
# Basic DOC tests
make run_doc

# Run audio test
make run_audio_test

# Run wavetable test
make run_wavetable_test

# Run debug test for DOC5503
make run_debug_test

# Run the unhalt test for DOC5503
make run_unhalt_test

# Run comparison test between Verilog and C++ implementations
make run_comparison_test

# Run standalone ES5503 C++ test
make run_standalone_test

# Build and run all utility tools
make utilities
```

## Key Documents

- `doc/DOC5503_Testing_Notes.md` - Testing notes and observations
- `doc/doc5503_verification_plan.md` - Comprehensive verification plan
- `doc/ES5503_Comparison.md` - Comparison between ES5503 and DOC5503 implementations

## Recent Updates

The DOC5503 implementation has been successfully validated. Key findings:

1. The oscillator control register bit 0 is the halt bit (0=run, 1=halt)
2. Channel mapping works correctly (0x00 routes to right channel, 0x10 to left channel)
3. Properly implemented with wavetable-based synthesis
4. Matches expected behavior when synthesized to the FPGA

## Running a Complete Test

To run a full test suite:

```bash
# Clean the build directory
make clean

# Build and run all tests
make all

# Run the comparison test
make run_comparison_test
```

## Development

When adding new tests:

1. Place test code in the appropriate subdirectory
2. Update the Makefile to include new test targets
3. Document test purpose and results in the relevant documentation