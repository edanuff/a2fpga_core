# DOC5503 Verilog Test Environment

This folder contains a pure Verilog testbench environment for the DOC5503 sound module. These tests are designed to be compiled directly to binary executables using Verilator's binary compilation mode, allowing you to run tests without writing any C++ code.

## Available Tests

1. **Register Test (`doc5503_verilog_test.sv`)**
   - Tests basic register read/write functionality
   - Verifies global and oscillator-specific register access
   - Useful for debugging register access issues

2. **Sine Wave Test (`doc5503_sine_test.sv`)**
   - Configures a single oscillator for sine wave generation
   - Collects audio output samples for analysis
   - Provides visualization of audio waveform

3. **Multiple Oscillators Test (`doc5503_oscillators_test.sv`)**
   - Tests multiple oscillators with different waveforms
   - Tests stereo channel mixing and routing
   - Tests different frequency and volume settings

## Features

- Written entirely in SystemVerilog (no C++ code needed)
- Detailed debug output to console for diagnostics
- Waveform generation for signal timing analysis
- Automatic plotting of audio output with Python scripts
- Makefile-based build system for easy test execution

## Quick Start

### Building and Running Tests

```bash
# Run the default register test
make

# Run the sine wave test
make run-sine

# Run the multiple oscillators test
make run-multi

# Run all tests
make test-all

# Clean up build artifacts
make clean
```

### Using the Helper Script

A helper script is provided for convenience:

```bash
# Run the sine wave test and plot results
./build_and_run.sh -t sine -p

# Run all tests
./build_and_run.sh -t all

# Show help
./build_and_run.sh -h
```

### Visualizing Results

```bash
# View register test waveforms in GTKWave
make view-reg

# Generate and view sine wave audio plot
make plot-sine

# Generate and view multi-oscillator audio plot
make plot-multi
```

## Understanding the Tests

### Register Test

The register test verifies that the DOC5503 module can correctly handle register read and write operations. It tests:

- Global registers (0xE0-0xE1)
- Oscillator-specific registers (0x00-0xDF)
- Proper timing of chip select (CS) and write enable (WE) signals
- Clock enable (clk_en) timing

### Sine Wave Test

The sine wave test configures a single oscillator to generate a sine wave and captures the audio output. It:

- Sets up oscillator 0 with sine wave parameters
- Provides sine wave data when requested by DOC5503
- Captures audio output for analysis
- Creates a text file of audio samples for plotting

### Multiple Oscillators Test

The multiple oscillators test configures several oscillators with different waveforms, frequencies, and channel settings. It:

- Configures oscillator 0 for sine wave on left channel
- Configures oscillator 1 for square wave on right channel
- Configures oscillator 2 for triangle wave on both channels
- Configures oscillator 3 for sawtooth wave on both channels
- Captures stereo audio output for analysis

## Modifying the Tests

### Adding a New Test

1. Create a new test file based on the existing templates
2. Add it to the Makefile with appropriate build/run/view targets
3. Update the build_and_run.sh script if needed

### Testing Different Configurations

To test different DOC5503 configurations:

1. Modify register settings in the test files
2. Adjust waveform tables for different audio outputs
3. Change oscillator frequencies, volumes, or modes

## DOC5503 Register Map

For reference, here's the DOC5503 register map:

- **0x00-1F**: Frequency Low Register (oscillators 0-31)
- **0x20-3F**: Frequency High Register (oscillators 0-31)
- **0x40-5F**: Volume Register (oscillators 0-31)
- **0x60-7F**: Waveform Data Sample Register (oscillators 0-31)
- **0x80-9F**: Waveform Table Pointer Register (oscillators 0-31)
- **0xA0-BF**: Control Register (oscillators 0-31)
- **0xC0-DF**: Resolution/Table Size Register (oscillators 0-31)
- **0xE0**: Oscillator Interrupt Register (global)
- **0xE1**: Oscillator Enable Register (global)

## Requirements

- Verilator (for compiling SystemVerilog to executable)
- Python 3 with matplotlib and numpy (for plotting)
- GTKWave (optional, for viewing waveforms)

## Troubleshooting

If you encounter issues:

1. Check the console output for error messages
2. Verify that clk_en timing is correct (every 8 clock cycles)
3. Ensure proper register access timing with sufficient cycles
4. Check for timing issues in the waveform viewer
5. Verify that wave data is being provided when requested