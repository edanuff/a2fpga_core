#!/bin/bash

# Script to open the swap mode test VCD file in GTKWave with specific signals loaded

VCD_FILE="./obj_dir/doc5503_verilog_swap_test.vcd"

if [ ! -f "$VCD_FILE" ]; then
    echo "Error: VCD file not found at $VCD_FILE"
    echo "Run 'make run' to generate the VCD file first."
    exit 1
fi

# Create a temporary save file for GTKWave to load the specific signals
TMP_SAVEFILE="$(mktemp -t gtkwave_swap_XXXXXX.gtkw)"

# Write a GTKWave save file that highlights the swap-related signals
cat > "$TMP_SAVEFILE" <<EOF
[*]
[*] GTKWave Analyzer v3.3.104 (w)1999-2020 BSI
[*]
[dumpfile] "$VCD_FILE"
[dumpfile_mtime] "$(date +%s)"
[dumpfile_size] $(stat -f%z "$VCD_FILE")
[savefile] "$TMP_SAVEFILE"
[timestart] 0
[size] 1440 900
[pos] -1 -1
*-11.000000 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
[treeopen] TOP.
[treeopen] TOP.doc5503_verilog_test.
[treeopen] TOP.doc5503_verilog_test.dut.
[sst_width] 290
[signals_width] 366
[sst_expanded] 1
[sst_vpaned_height] 321
@28
TOP.doc5503_verilog_test.dut.clk_i
TOP.doc5503_verilog_test.dut.reset_n_i
TOP.doc5503_verilog_test.dut.clk_en_i
@22
TOP.doc5503_verilog_test.osc_en[7:0]
@420
TOP.doc5503_verilog_test.cycle_count[31:0]
@28
TOP.doc5503_verilog_test.dut.curr_mode_w
@22
TOP.doc5503_verilog_test.dut.curr_osc_r[3:0]
@28
TOP.doc5503_verilog_test.dut.curr_halt_r
TOP.doc5503_verilog_test.dut.partner_halt_r
TOP.doc5503_verilog_test.dut.overflow_w
TOP.doc5503_verilog_test.dut.swap_w
@22
TOP.doc5503_verilog_test.dut.osc_acc_r[0][23:0]
TOP.doc5503_verilog_test.dut.osc_acc_r[1][23:0]
@28
TOP.doc5503_verilog_test.wave_rd
@22
TOP.doc5503_verilog_test.wave_address[15:0]
TOP.doc5503_verilog_test.wave_data[7:0]
@28
TOP.doc5503_verilog_test.wave_data_ready
@22
+{Oscillator 0 Halt} TOP.doc5503_verilog_test.dut.osc_halt_r[0]
+{Oscillator 1 Halt} TOP.doc5503_verilog_test.dut.osc_halt_r[1]
@420
+{Oscillator 0 Accumulator} TOP.doc5503_verilog_test.dut.osc_acc_r[0][23:0]
+{Oscillator 1 Accumulator} TOP.doc5503_verilog_test.dut.osc_acc_r[1][23:0]
[pattern_trace] 1
[pattern_trace] 0
EOF

# Launch GTKWave with our temporary save file
gtkwave -a "$TMP_SAVEFILE" "$VCD_FILE" || {
    echo "Error: Failed to launch GTKWave."
    echo "Make sure GTKWave is installed and in your PATH."
    echo ""
    echo "If GTKWave is not available, you can examine the VCD file with any other waveform viewer."
    echo "The VCD file is located at: $VCD_FILE"
    exit 1
}

# Clean up temporary save file
rm -f "$TMP_SAVEFILE"