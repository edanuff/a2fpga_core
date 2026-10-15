#!/bin/sh
set -e
cd "$(dirname "$0")"
GOWIN_SIMLIB="${GOWIN_SIMLIB:-/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/simlib}"
iverilog -g2012 -DDOC_SIM_CHECKS -s tb_fetch_cache -o tb_fetch_cache.vvp \
    ../../hdl/sound/doc5503.sv ../../hdl/sound/doc_fetch_cache.sv "$GOWIN_SIMLIB/gw5a/prim_sim.v" tb_fetch_cache.sv
vvp -n tb_fetch_cache.vvp | tee fetch_cache.log | grep -E "STALE|P1|FINAL|RESULT" | head -20
grep -q "FETCH CACHE RESULT: PASS" fetch_cache.log || { echo "FAILED"; exit 1; }
echo "=== negative control (+noflush): stale cached words expected"
vvp -n tb_fetch_cache.vvp +noflush | tee fetch_cache_noflush.log | grep -E "NEGATIVE|FINAL|RESULT" | head -4
grep -q "FETCH CACHE RESULT: FAIL" fetch_cache_noflush.log || { echo "NEGATIVE CONTROL DID NOT FAIL"; exit 1; }
echo "FETCH CACHE OK"
