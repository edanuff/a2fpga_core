#!/usr/bin/env bash
# The bench of gs_accel_trace alone (tb_gs_trace.sv).
# Usage: boards/a2mega/hdl/twgs/sim/trace/run_gs_trace_sim.sh [+vcd]
# Env: OUT  work dir, default ${TMPDIR:-/tmp}/gs_trace_sim
# Requires Verilator >= 5 (timing support).
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
TWGS="$(cd "$HERE/../.." && pwd)"
OUT="${OUT:-${TMPDIR:-/tmp}/gs_trace_sim}"
mkdir -p "$OUT"
verilator --binary --timing -Wno-fatal -Wno-lint -Wno-style -O3 --top-module tb_gs_trace \
    --Mdir "$OUT/obj" -o Vtb_gs_trace "$TWGS/gs_accel_trace.sv" "$HERE/tb_gs_trace.sv" > "$OUT/verilate.log" 2>&1 \
    || { echo "verilator FAILED - see $OUT/verilate.log"; grep -E "%Error|error:" "$OUT/verilate.log" | head -20; exit 1; }
"$OUT/obj/Vtb_gs_trace" "$@" | tee "$OUT/run.log"
grep -q "^PASS" "$OUT/run.log"
