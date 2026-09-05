#!/usr/bin/env bash
# Sim 1 for the in-socket 65C816: gs_socket_phy + P65C816 against a
# behavioural IIgs FPI bus model with W65C816S timing checks at the socket.
# See boards/a2mega/docs/gs_socket_65816_scoping.md §7.
#
# Usage: boards/a2mega/hdl/twgs/sim/run_gs_socket_sim.sh [extra +plusargs]
#   +cycles=N       bus cycles to run (default 6000; 200000 in ROM mode)
#   +noturbo        ignore the $C036 speed write: 1 MHz throughout
#   +trace          print every bus cycle
#   +vcd            write gs_socket.vcd in OUT
#   +rom=<file>     ROM mode: run a ROM 01 image (128 KB, banks FE/FF) with a
#                   lockstep reference core instead of the built-in program
# Env: OUT (work dir, default ${TMPDIR:-/tmp}/gs_socket_sim)
#      ROM (path to boot1.rom; same as +rom=, e.g. the MiSTer vsim/boot1.rom
#           built by Apple-IIgs_MiSTer/roms/Makefile from a MAME apple2gs.zip;
#           Apple ROM content is never committed)
# Requires Verilator >= 5 (timing support).
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
TWGS="$(cd "$HERE/.." && pwd)"
CORE="$TWGS/65C816"
OUT="${OUT:-${TMPDIR:-/tmp}/gs_socket_sim}"
mkdir -p "$OUT"

FILES=( "$CORE/P65C816_pkg.sv" "$CORE/mcode.sv" "$CORE/bit_adder.v" "$CORE/adder4.v"
        "$CORE/BCDAdder.v" "$CORE/AddSubBCD.sv" "$CORE/ALU.sv" "$CORE/AddrGen.sv"
        "$CORE/P65C816.sv" "$TWGS/gs_socket_phy.sv" "$HERE/tb_gs_socket.sv" )

verilator --binary --timing -Wno-fatal -Wno-lint -Wno-style -Wno-MULTIDRIVEN \
    --top-module tb_gs_socket --Mdir "$OUT/obj" -o Vtb_gs_socket \
    --trace --trace-depth 4 \
    "${FILES[@]}" > "$OUT/verilate.log" 2>&1
rc=$?
if [[ $rc -ne 0 ]]; then
    echo "verilator FAILED (rc=$rc) — see $OUT/verilate.log"; grep -E "%Error" "$OUT/verilate.log" | head -20; exit $rc
fi
ROMARG=()
[[ -n "${ROM:-}" ]] && ROMARG=( "+rom=$ROM" )
( cd "$OUT" && ./obj/Vtb_gs_socket ${ROMARG[@]+"${ROMARG[@]}"} "$@" ) | tee "$OUT/sim.log"
exit "${PIPESTATUS[0]}"
