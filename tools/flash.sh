#!/usr/bin/env bash
#
# flash.sh — Flash an A2FPGA board's bitstream to the device with openFPGALoader.
#
# Usage:
#   tools/flash.sh <board> [--sram]
#     <board>  board directory under boards/ (e.g. a2mega, a2n20v2)
#     --sram   load to volatile SRAM (lost on power-down) instead of SPI flash
#
# Environment:
#   FS=<path> override the bitstream path (default: boards/<board>/impl/pnr/<proj>.fs)
#   GPRJ=<file.gprj> select the project when a board has several (same as build.sh);
#             REQUIRED for multi-gprj boards unless FS= is given — flashing the
#             alphabetically-first project by accident cost a full debug day (2026-08-13)
#   DRY_RUN=1 print the openFPGALoader command without running it
#
# Per-board programming config (the error-prone knowledge this encapsulates):
#   a2n20v1 / a2n20v2 / a2n20v2-GS / a2n20v2-Enhanced  -> -b tangnano20k   (Tang Nano 20K)
#   a2n9                                               -> -b tangnano9k    (Tang Nano 9K)
#   a2mega (Tang Mega 60K) / a2p25 (Tang Primer 25K)   -> -c esp32s3       (on-board ESP32S3 programmer)
#
set -uo pipefail

REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

board="${1:-}"
mode="flash"
[[ "${2:-}" == "--sram" ]] && mode="sram"

if [[ -z "$board" ]]; then
    echo "Usage: tools/flash.sh <board> [--sram]"
    echo "Boards:"; (cd "$REPO/boards" && ls -d */ 2>/dev/null | tr -d '/')
    exit 2
fi

bdir="$REPO/boards/$board"
[[ -d "$bdir" ]] || { echo "No such board: '$board' (see boards/)"; exit 2; }

gprj="${GPRJ:-$(cd "$bdir" && ls *.gprj 2>/dev/null | head -1)}"
nprj="$(cd "$bdir" && ls *.gprj 2>/dev/null | wc -l | tr -d ' ')"
if [[ -z "${GPRJ:-}" && -z "${FS:-}" && "$nprj" -gt 1 ]]; then
    echo "!! Board '$board' has $nprj projects: $(cd "$bdir" && ls *.gprj | tr '\n' ' ')"
    echo "!! Refusing to guess. Set GPRJ=<file.gprj> or FS=<path>."
    exit 2
fi
[[ -n "$gprj" ]] || { echo "No .gprj found in $bdir"; exit 2; }
proj="${gprj%.gprj}"
fs="${FS:-$bdir/impl/pnr/${proj}.fs}"
[[ -f "$fs" ]] || { echo "No bitstream at: $fs"; echo "Build it first:  tools/build.sh $board"; exit 2; }

# Resolve the loader binary. For GW5A boards prefer the repo's patched
# build (tools/bin/openFPGALoader-a2mega2): honest empty-chain errors,
# freq-honored flash phase, and the auto-boot-race mitigations
# (OFL_GW5A_SETTLE_MS wait — measured 5/6 first-attempt on populated
# flash vs ~50% stock — and opt-in SRAM-preload OFL_GW5A_SRAM_FIRST).
repo_root="$(cd "$(dirname "$0")/.." && pwd)"
LOADER="${LOADER:-}"
if [[ -z "$LOADER" && -x "$repo_root/tools/bin/openFPGALoader-a2mega2" ]]; then
    LOADER="$repo_root/tools/bin/openFPGALoader-a2mega2"
fi
[[ -n "$LOADER" ]] || LOADER="$(command -v openFPGALoader || command -v openfpgaloader || true)"
[[ -n "$LOADER" ]] || { echo "openFPGALoader not found. Install it (macOS: brew install openfpgaloader)."; exit 3; }
# GW5A boot-scanner settle (wait-only default; preload is opt-in recovery)
export OFL_GW5A_NO_SRAM_FIRST="${OFL_GW5A_SRAM_FIRST:+0}"
export OFL_GW5A_NO_SRAM_FIRST="${OFL_GW5A_NO_SRAM_FIRST:-1}"
export OFL_GW5A_SETTLE_MS="${OFL_GW5A_SETTLE_MS:-3500}"

esp32s3=0
case "$board" in
    a2n20v1|a2n20v2|a2n20v2-GS|a2n20v2-Enhanced) tag=(-b tangnano20k) ;;
    a2n9)                                        tag=(-b tangnano9k) ;;
    a2mega|a2p25)                                tag=(-c esp32s3); esp32s3=1 ;;  # on-board ESP32S3
    *) echo "No flashing config for board '$board'. Add one to tools/flash.sh."; exit 2 ;;
esac

cmd=("$LOADER" "${tag[@]}")
if [[ "$mode" == "flash" ]]; then
    if [[ "$esp32s3" == "1" ]]; then
        # NOTE: --bulk-erase is a NO-OP when combined with -f (openFPGALoader
        # guards it with bit_file.empty(); main.cpp:650). Sector erases still
        # happen inside erase_and_prog. Run --bulk-erase as a SEPARATE
        # invocation (no -f) when a true full-chip erase is needed.
        cmd+=(-f --verify)  # ESP32S3 bridge: write + read-back verify
    else
        cmd+=(-f)                  # write to SPI flash (persistent)
    fi
fi
cmd+=("$fs")

if [[ "$board" == "a2mega" ]]; then
    echo "NOTE (a2mega): POWER THE HOST APPLE OFF before flashing — flashing with the"
    echo "               machine live fails intermittently (verified on hardware)."
    echo "NOTE (a2mega): power-cycle the board after flashing. The config-load CRC makes"
    echo "               the heartbeat LED after a power cycle the real write verifier;"
    echo "               a 'Done' without --verify has produced corrupt flash."
fi

echo ">> ${cmd[*]}"
[[ "${DRY_RUN:-}" == "1" ]] && { echo "-- DRY RUN -- not executed."; exit 0; }

if [[ "$esp32s3" == "1" && "$mode" == "flash" ]]; then
    # Protocol (validated 2026-08-17, test log #10/#11): single attempt;
    # on failure, verify the chain is still healthy (failures are benign
    # since the settle fix — no wedge) and retry EXACTLY ONCE. Never
    # hammer: repeated blind retries were implicated in flash corruption.
    for attempt in 1 2; do
        echo ">> flash attempt $attempt"
        if "${cmd[@]}"; then
            exit 0
        fi
        if [[ "$attempt" == "1" ]]; then
            echo "!! attempt 1 failed; probing chain before the single retry..."
            if ! "$LOADER" -c esp32s3 --detect >/dev/null 2>&1; then
                echo "!! chain unhealthy — NOT retrying (wedge suspected)."
                break
            fi
            sleep 2
        fi
    done
    echo "!! flash failed."
    # Diagnostic snapshot: does the failure sit at the USB/ESP32 bridge layer
    # (open fails) or the FPGA TAP layer (bridge opens, no/garbage IDCODE)?
    # Discriminates the intermittent wedge classes — record with the failure.
    echo "!! diagnostic: low-speed detect probe:"
    openFPGALoader -c esp32s3 --freq 500000 --detect 2>&1 | tail -4 | sed 's/^/!!   /'
    echo "!! Recovery options (see boards/a2mega/docs/jtag_flash_reliability.md):"
    echo "!!   1. OFL_GW5A_SRAM_FIRST=1 tools/flash.sh ...   (preload variant)"
    echo "!!   2. Corrupt/wedged flash: replug, load flash_rescue.fs to SRAM,"
    echo "!!      wait for E:D on telnet, replug, flash onto the blank chip."
    echo "!! Always: Apple II POWERED OFF while flashing."
    exit 1
else
    "${cmd[@]}"
fi
