#!/bin/sh
# Differential simulation: baseline doc5503 vs doc5503_pipelined (rev 3).
# Requires Icarus Verilog (iverilog/vvp) and python3.
#
# 1. Rev-2 failure reproduction: lookahead off, FB gating off, plus the
#    testbench's synthetic word-granularity load emulating rev-2's fetch
#    rate. Must show chronic FB line-deadline misses (the field failure).
# 2. Rev-3 suite: full differential correctness + traffic + FB-deadline
#    assertions via compare.py.
set -e
cd "$(dirname "$0")"

SRCS="../../hdl/sound/doc5503.sv ../../hdl/sound/doc5503_pipelined.sv tb_doc5503_diff.sv"

echo "=== rev-2 failure reproduction (REV2_MODE + nogate) ==="
iverilog -g2012 -DREV2_MODE -o tb_rev2repro.vvp $SRCS
vvp tb_rev2repro.vvp +nogate | tee repro_run.log | grep -E "FINAL|PHASE 10|PHASE 11"
python3 - <<'EOF'
miss = 0
for ln in open("repro_run.log"):
    if ln.startswith("FINAL"):
        for tok in ln.split():
            if tok.startswith("fb_miss="):
                miss = int(tok.split("=")[1])
if miss < 200:
    print(f"REPRO FAILED: expected chronic FB misses at rev-2 traffic, got {miss}")
    raise SystemExit(1)
print(f"REPRO OK: rev-2-class traffic causes {miss} FB line-deadline misses")
EOF

echo "=== rev-3 differential suite ==="
iverilog -g2012 -o tb_doc5503_diff.vvp $SRCS
vvp tb_doc5503_diff.vvp

python3 compare.py
