#!/bin/bash
# Check if gtkwave is available
if command -v gtkwave &> /dev/null; then
    gtkwave doc5503_trace.vcd &
    echo "Launched gtkwave"
else
    echo "gtkwave not found, please install it to visualize waveforms"
    echo "Waveform file: doc5503_trace.vcd"
fi