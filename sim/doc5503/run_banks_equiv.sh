#!/bin/sh
# Equivalence check: doc5503 FF/SSRAM banks vs BANKS_IN_BSRAM=1 (Gowin DPB).
# Needs iverilog and the Gowin IDE simulation library (gw5a prim_sim.v).
set -e
cd "$(dirname "$0")"
GOWIN_SIMLIB="${GOWIN_SIMLIB:-/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/simlib}"
iverilog -g2012 -DDOC_SIM_CHECKS -s tb_banks_equiv -o tb_banks_equiv.vvp \
    ../../hdl/sound/doc5503.sv "$GOWIN_SIMLIB/gw5a/prim_sim.v" tb_banks_equiv.sv
for seed in ${SEEDS:-1 2 3}; do
    vvp -n tb_banks_equiv.vvp +seed=$seed | tee banks_equiv_$seed.log | grep -E "MISMATCH|ERROR|FINAL|RESULT" | head -40
    grep -q "BANKS EQUIV RESULT: PASS" banks_equiv_$seed.log || { echo "FAILED seed $seed"; exit 1; }
done
echo "ALL SEEDS PASS"
