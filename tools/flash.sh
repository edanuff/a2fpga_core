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

# Resolve the loader binary (Homebrew installs openFPGALoader; some setups use lowercase).
LOADER="$(command -v openFPGALoader || command -v openfpgaloader || true)"
[[ -n "$LOADER" ]] || { echo "openFPGALoader not found. Install it (macOS: brew install openfpgaloader)."; exit 3; }

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
    # The esp_usb_jtag bridge drops status reads intermittently during long
    # flash sessions (~50% per-attempt failure observed); retry a few times.
    for attempt in 1 2 3 4 5; do
        echo ">> flash attempt $attempt"
        if "${cmd[@]}"; then
            exit 0
        fi
        echo "!! attempt $attempt failed; retrying..."
        sleep 3
    done
    echo "!! all flash attempts failed."
    # Diagnostic snapshot: does the failure sit at the USB/ESP32 bridge layer
    # (open fails) or the FPGA TAP layer (bridge opens, no/garbage IDCODE)?
    # Discriminates the intermittent wedge classes — record with the failure.
    echo "!! diagnostic: low-speed detect probe:"
    openFPGALoader -c esp32s3 --freq 500000 --detect 2>&1 | tail -4 | sed 's/^/!!   /'
    echo "!! Flash with the Apple II POWERED OFF (validated procedure), replug"
    echo "!! USB (cold boot), and rerun."
    echo "!! See boards/a2mega/docs/jtag_flash_reliability.md"
    exit 1
else
    "${cmd[@]}"
fi
