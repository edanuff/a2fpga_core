#!/usr/bin/env bash
#
# build.sh — Build an A2FPGA board with the Gowin gw_sh CLI and report timing.
#
# Usage:
#   tools/build.sh <board> [syn|all]
#     <board>  board directory under boards/ (e.g. a2mega, a2n20v2-GS)
#     stage    "all" (default: synthesis + place & route + bitstream) or
#              "syn" (synthesis only)
#
# Environment:
#   GW_SH     path to gw_sh (default: macOS GowinIDE location)
#   GPRJ      project file name to use when a board dir has more than one
#             .gprj (e.g. GPRJ=a2n20v2_enhanced_dualrate.gprj); default is
#             the first one alphabetically
#   BUILD_TIMEOUT  wall-time cap in seconds (default 1800). A healthy full
#             build is ~15 min; a placer that runs past 30 min is thrashing
#             (region constraints have produced 9-HOUR runs) and gets
#             killed with a loud failure instead of eating the day.
#   DRY_RUN=1 print what would run without invoking gw_sh
#
# Notes:
#   - Uses the pipe method (the only supported way; never `gw_sh -exit -e`).
#   - Auto-discovers the board's .gprj (filenames don't always match the dir,
#     e.g. a2n20v2-GS uses a2n20v2_gs.gprj).
#   - See docs/setup-gowin-cli.md and tools/README.md for toolchain setup.
#
set -uo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
GW_SH="${GW_SH:-/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh}"

board="${1:-}"
stage="${2:-all}"

if [[ -z "$board" ]]; then
    echo "Usage: tools/build.sh <board> [syn|all]"
    echo "Boards:"; (cd "$REPO/boards" && ls -d */ 2>/dev/null | tr -d '/')
    exit 2
fi

bdir="$REPO/boards/$board"
[[ -d "$bdir" ]] || { echo "No such board: '$board' (see boards/)"; exit 2; }

gprj="${GPRJ:-$(cd "$bdir" && ls *.gprj 2>/dev/null | head -1)}"
[[ -n "$gprj" ]] || { echo "No .gprj found in $bdir"; exit 2; }
[[ -f "$bdir/$gprj" ]] || { echo "No such project file: $bdir/$gprj"; exit 2; }
proj="${gprj%.gprj}"

case "$stage" in
    syn) run="run syn" ;;
    all) run="run all" ;;
    *)   echo "stage must be 'syn' or 'all' (got '$stage')"; exit 2 ;;
esac

echo ">> Board:   $board"
echo ">> Project: $gprj"
echo ">> Stage:   $stage"

if [[ "${DRY_RUN:-}" == "1" ]]; then
    echo "-- DRY RUN -- would run from $bdir:"
    echo "   printf 'open_project %s\\n%s\\nexit\\n' '$gprj' '$run' | '$GW_SH'"
    exit 0
fi

[[ -x "$GW_SH" ]] || {
    echo "gw_sh not found/executable at: $GW_SH"
    echo "Install Gowin and (macOS) run tools/gowin_cli_fix_macos.sh; or set GW_SH=/path/to/gw_sh."
    echo "See docs/setup-gowin-cli.md."
    exit 3
}

start_epoch="$(date +%s)"
buildlog="$(mktemp "${TMPDIR:-/tmp}/a2fpga_build.XXXXXX")"
BUILD_TIMEOUT="${BUILD_TIMEOUT:-1800}"

# Run gw_sh under a wall-time cap (macOS has no coreutils `timeout`).
CAFF=""
[[ "$(uname)" == "Darwin" ]] && command -v caffeinate >/dev/null && CAFF="caffeinate -i"
( cd "$bdir" && printf 'open_project %s\n%s\nexit\n' "$gprj" "$run" | $CAFF "$GW_SH" ) | tee "$buildlog" &
build_pid=$!
(
    slept=0
    while kill -0 "$build_pid" 2>/dev/null && [[ $slept -lt $BUILD_TIMEOUT ]]; do
        sleep 10; slept=$((slept+10))
    done
    if kill -0 "$build_pid" 2>/dev/null; then
        echo "!! BUILD_TIMEOUT: exceeded ${BUILD_TIMEOUT}s wall time — killing gw_sh (placer thrash?)" | tee -a "$buildlog"
        pkill -P "$build_pid" 2>/dev/null
        kill "$build_pid" 2>/dev/null
        # gw_sh detaches from the subshell pipeline; make sure it dies too
        pkill -x gw_sh 2>/dev/null
    fi
) &
watchdog_pid=$!
wait "$build_pid" 2>/dev/null
kill "$watchdog_pid" 2>/dev/null; wait "$watchdog_pid" 2>/dev/null

if grep -q 'BUILD_TIMEOUT' "$buildlog"; then
    echo
    echo "!! Build KILLED after exceeding BUILD_TIMEOUT=${BUILD_TIMEOUT}s."
    echo "   A healthy full build is ~15 min. Do not just raise the timeout:"
    echo "   look for what made PnR pathological (e.g. GROUP/GRP_LOC region"
    echo "   constraints — see the note in boards/a2mega/hdl/a2mega.cst)."
    rm -f "$buildlog"
    exit 5
fi

# gw_sh exits 0 even when synthesis/PnR fail — detect ERROR lines ourselves.
if grep -qE '^ERROR' "$buildlog"; then
    echo
    echo "!! gw_sh reported errors — build FAILED (stale artifacts in impl/ do not count):"
    grep -E '^ERROR' "$buildlog" | head -5
    rm -f "$buildlog"
    exit 4
fi
rm -f "$buildlog"

# ---- Post-build timing check (required after place & route) ----
if [[ "$stage" == "all" ]]; then
    fs="$bdir/impl/pnr/${proj}.fs"
    tr="$bdir/impl/pnr/${proj}_tr_content.html"
    echo
    if [[ ! -f "$fs" ]]; then
        echo "!! No bitstream produced ($fs). Check the synthesis/PnR log for errors."
        exit 4
    fi
    # A pre-existing bitstream from an earlier build must not pass as success.
    fs_mtime="$(stat -f %m "$fs" 2>/dev/null || stat -c %Y "$fs" 2>/dev/null)"
    if [[ -n "$fs_mtime" && "$fs_mtime" -lt "$start_epoch" ]]; then
        echo "!! Bitstream is STALE (predates this build) — the run did not produce a new one."
        exit 4
    fi
    echo ">> Bitstream: boards/$board/impl/pnr/${proj}.fs"
    echo ">> Timing summary:"
    if [[ -f "$tr" ]]; then
        sv="$(grep -A1 "Setup Violated Endpoints" "$tr" | sed -E 's/<[^>]+>//g' | grep -Eo '[0-9]+' | head -1)"
        hv="$(grep -A1 "Hold Violated Endpoints"  "$tr" | sed -E 's/<[^>]+>//g' | grep -Eo '[0-9]+' | head -1)"
        echo "   Setup violations: ${sv:-?}    Hold violations: ${hv:-?}"
        echo "   Full report: boards/$board/impl/pnr/${proj}_tr_content.html"
        if [[ "${sv:-1}" != "0" || "${hv:-1}" != "0" ]]; then
            echo "   !! TIMING VIOLATIONS PRESENT — build is NOT clean. Inspect the report before using this bitstream."
            exit 4
        fi
        echo "   OK: 0 violations. (Confirm Fmax >= constraint for each clock in the report.)"
    else
        echo "   Timing report not found at $tr — verify the PnR step completed."
        exit 4
    fi
fi
