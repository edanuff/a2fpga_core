#!/usr/bin/env bash
# Full-chain DP frame+audio sim against THIS repo's hdl/displayport (the
# canonical, sched-ROM packer), using the DisplayPort_Verilog harness:
#   tb_dp_frame_audio.v -> symbol dump -> misc/check_dp_frame.c (pixel-exact)
#                                       -> misc/check_dp_audio.c (SDP ECC + PCM)
# Usage: hdl/displayport/sim/run_full_chain.sh [0|1|both]
#   0    = legacy Bresenham walk packer
#   1    = schedule-ROM packer (+ its shadow-walk $fatal assertions)
#   both = default
# Env: DPV (harness repo, default ~/GitHub/DisplayPort_Verilog), OUT (work dir).
# Requires iverilog/vvp and a C compiler.
set -uo pipefail
REPO="$(cd "$(dirname "$0")/../../.." && pwd)"
DPV="${DPV:-$HOME/GitHub/DisplayPort_Verilog}"
OUT="${OUT:-${TMPDIR:-/tmp}/dp_full_chain}"
mkdir -p "$OUT"
SRC="$REPO/hdl/displayport"
FILES=( "$SRC/video/dp_video_timing.v" "$SRC/video/pixel_cdc_fifo.v"
        "$SRC/video/video_stream_packer.v" "$SRC/video/msa_inserter_2ch.v"
        "$SRC/audio/audio_sample_buffer.v" "$SRC/audio/maud_measure.v"
        "$SRC/audio/sdp_engine.v" )

cc -O2 -o "$OUT/check_dp_frame" "$DPV/misc/check_dp_frame.c"
cc -O2 -o "$OUT/check_dp_audio" "$DPV/misc/check_dp_audio.c"

run_cfg() {
  local sched=$1
  local dump="$OUT/dump_sched$sched.txt"
  local extra=()
  if [[ "$sched" == "1" ]]; then
    printf 'module dp_fc_defparams; defparam tb_dp_frame_audio.i_packer.SCHED_ROM = 1; endmodule\n' > "$OUT/defparams.v"
    extra=( "$OUT/defparams.v" )
  fi
  echo "== SCHED_ROM=$sched =="
  iverilog -g2012 -o "$OUT/tb_sched$sched.vvp" -DDUMP_FILE="\"$dump\"" \
      "$DPV/test_benches/tb_dp_frame_audio.v" "${FILES[@]}" ${extra[@]+"${extra[@]}"}
  # run from video/ so the packer's $readmemh finds video_sched_720p_2l.hex
  ( cd "$SRC/video" && vvp "$OUT/tb_sched$sched.vvp" ) | grep -E "PASS|FAIL|ready after|fatal|rror" | head -8
  echo "-- check_dp_frame:"; "$OUT/check_dp_frame" "$dump" 2>&1 | tail -2 || true
  echo "-- check_dp_audio:"; "$OUT/check_dp_audio" "$dump" 2>&1 | tail -3 || true
}

case "${1:-both}" in
  0)    run_cfg 0 ;;
  1)    run_cfg 1 ;;
  both) run_cfg 0; run_cfg 1 ;;
  *)    echo "usage: $0 [0|1|both]"; exit 2 ;;
esac
