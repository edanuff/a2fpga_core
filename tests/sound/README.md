# DOC5503 Verilator Test Harness

This directory contains a Verilator test harness for the DOC5503 sound module. It allows you to simulate the DOC's behavior and debug audio generation issues.

## Files

- `doc5503_harness.sv`: A wrapper module for the DOC5503 that simulates the register interface
- `doc5503_tb.cpp`: The C++ testbench that drives the simulation
- `Makefile`: Builds and runs the simulation
- `view_waves.sh`: A script to open the generated waveform file in gtkwave

## Running the Tests

1. Build and run the simulation:
```bash
make
make run
```

2. View the waveforms (if gtkwave is installed):
```bash
./view_waves.sh
```

## Modifying the Test

You can modify the test parameters in `doc5503_tb.cpp`:

- Change MAX_SIM_TIME to run longer or shorter simulations
- Modify the verbosity level (ERROR, WARNING, INFO, DEBUG)
- Adjust the register writes to test different DOC configurations

## Debugging Audio Issues

The test harness allows you to:

1. Set up DOC registers with known values
2. Provide controlled waveform data for the oscillators
3. Monitor the audio output signals (mono_mix_o, left_mix_o, right_mix_o)
4. Trace the internal signal values using the VCD output

This can help identify issues with:
- DOC register setup
- Waveform data handling
- Audio signal generation
- Oscillator mixing

## Tips

- Set the verbosity to DEBUG to see more details during simulation
- Look for the left_mix and right_mix outputs in the logs to see audio values
- Examine the VCD file in gtkwave to see all signals over time