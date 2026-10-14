#!/bin/sh
# Late-fetch guard check for the baseline doc5503 (variable-latency memory).
set -e
cd "$(dirname "$0")"
GOWIN_SIMLIB="${GOWIN_SIMLIB:-/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/simlib}"
iverilog -g2012 -DDOC_SIM_CHECKS -s tb_fetch_late -o tb_fetch_late.vvp \
    ../../hdl/sound/doc5503.sv ../../hdl/sound/doc_fetch_filter.sv \
    "$GOWIN_SIMLIB/gw5a/prim_sim.v" tb_fetch_late.sv
vvp -n tb_fetch_late.vvp | tee fetch_late.log | grep -E "STALE|FINAL|RESULT" | head -20
grep -q "FETCH LATE RESULT: PASS" fetch_late.log || { echo "FAILED"; exit 1; }
echo "=== negative control (+noguard): stale samples expected"
vvp -n tb_fetch_late.vvp +noguard | tee fetch_late_noguard.log | grep -E "NEGATIVE|FINAL|RESULT" | head -5
grep -q "FETCH LATE RESULT: FAIL" fetch_late_noguard.log || { echo "NEGATIVE CONTROL DID NOT FAIL"; exit 1; }
echo "FETCH LATE OK"
