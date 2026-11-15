#!/usr/bin/env bash
# Host tests: the ESP32 USB-C stale-session guard, and the S7 accelerator
# bring-up ladder against a model of the gateware's register window.
set -e
cd "$(dirname "$0")"
OUT="${TMPDIR:-/tmp}"
cc -std=c11 -Wall -Wextra -I../.. -I. \
   -o "$OUT/test_stale_guard" \
   test_stale_guard.c mock_fusb302.c ../../usbc_port.c
"$OUT/test_stale_guard"
cc -std=c11 -Wall -Wextra -Istubs -I../.. -I. \
   -o "$OUT/test_gs_accel" \
   test_gs_accel.c ../../gs_socket.c
"$OUT/test_gs_accel"
