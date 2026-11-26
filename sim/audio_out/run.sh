#!/bin/sh
# hdl/support/iir_filter_gw5a.v (Arora V arithmetic) against hdl/support/iir_filter.v
# (the GW2A file), both under hdl/sound/audio_out.v, cycle for cycle; plus the
# frequency response of the a2p25 configuration (54 MHz, flt_rate 5,594,066)
# beside the Tang Nano one (27 MHz, 7,056,000). Requires Verilator.
set -e
cd "$(dirname "$0")"
mkdir -p obj_dir
# The reference copy gets _ref module names so both can live in one simulation.
perl -pe 's/\bIIR_filter\b/IIR_filter_ref/g; s/\biir_filter_tap\b/iir_filter_tap_ref/g; s/\bDC_blocker\b/DC_blocker_ref/g' \
    ../../hdl/support/iir_filter.v > obj_dir/iir_filter_ref.v
perl -pe 's/\bmodule audio_out\b/module audio_out_ref/; s/\bIIR_filter\b/IIR_filter_ref/g; s/\bDC_blocker\b/DC_blocker_ref/g' \
    ../../hdl/sound/audio_out.v > obj_dir/audio_out_ref.v
# iir_filter_gw5a.v instantiates Gowin's MULT12X12 primitive by name (so a2p25.cst
# can pin the cells across GowinSynthesis versions); its simulation model lives in
# the Gowin install. GOWIN_SIMLIB overrides the search; the model needs the
# top-level GSR instance that the bench provides.
if [ -z "${GOWIN_SIMLIB:-}" ]; then
    for app in /Applications/GowinIDE.app /Applications/GowinIDE*.app /opt/gowin /opt/Gowin* "$HOME"/gowin/*; do
        f="$app/Contents/Resources/Gowin_EDA/IDE/simlib/gw5a/prim_sim.v"
        [ -f "$f" ] || f="$app/IDE/simlib/gw5a/prim_sim.v"
        [ -f "$f" ] && GOWIN_SIMLIB="$f" && break
    done
fi
[ -f "${GOWIN_SIMLIB:-}" ] || { echo "GW5A prim_sim.v not found; set GOWIN_SIMLIB=/path/to/IDE/simlib/gw5a/prim_sim.v" >&2; exit 1; }
# only the two modules needed: the whole library does not lint under Verilator
awk '/^module GSR /{p=1} p{print; if(/^endmodule/) exit}' "$GOWIN_SIMLIB" > obj_dir/gw5a_prims.v
awk '/^module MULT12X12 /{p=1} p{print; if(/^endmodule/) exit}' "$GOWIN_SIMLIB" >> obj_dir/gw5a_prims.v
grep -q "^module MULT12X12" obj_dir/gw5a_prims.v || { echo "MULT12X12 not found in $GOWIN_SIMLIB" >&2; exit 1; }
verilator --binary --timing -Wno-WIDTHTRUNC -Wno-WIDTHEXPAND -Wno-UNSIGNED -Wno-REALCVT -Wno-TIMESCALEMOD --timescale 1ns/1ps -Wno-fatal -j 4 \
    --top-module tb_audio_out_gw5a -o tb_audio_out_gw5a \
    obj_dir/gw5a_prims.v \
    ../../hdl/support/iir_filter_gw5a.v ../../hdl/sound/audio_out.v \
    obj_dir/iir_filter_ref.v obj_dir/audio_out_ref.v tb_audio_out_gw5a.sv
./obj_dir/tb_audio_out_gw5a "$@" | tee audio_out_run.log
grep -q "AUDIO OUT RESULT: PASS" audio_out_run.log
