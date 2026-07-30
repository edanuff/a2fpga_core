#!/bin/sh
# Differential simulation: baseline doc5503 vs doc5503_pipelined.
# Requires Icarus Verilog (iverilog/vvp) and python3.
set -e
cd "$(dirname "$0")"

iverilog -g2012 -o tb_doc5503_diff.vvp \
    ../../hdl/sound/doc5503.sv \
    ../../hdl/sound/doc5503_pipelined.sv \
    tb_doc5503_diff.sv

vvp tb_doc5503_diff.vvp

python3 compare.py
