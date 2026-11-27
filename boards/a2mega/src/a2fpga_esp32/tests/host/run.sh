#!/usr/bin/env bash
# Host tests for the ESP32 firmware's pure (no ESP-IDF) modules:
#   - the USB-C stale-session guard (usbc_port.c)
#   - the S7 accelerator bring-up ladder against a model of the gateware's
#     register window (gs_socket.c)
#   - the USB-Ethernet protocol helpers (usbeth_proto.c: CDC descriptors,
#     ECM MAC string, NCM NTB16)
#   - the hub-port enumeration retry ladder (usbhub_retry.c)
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
cc -std=c11 -Wall -Wextra -I../.. -I. \
   -o "$OUT/test_usbeth_proto" \
   test_usbeth_proto.c ../../usbeth_proto.c
"$OUT/test_usbeth_proto"
cc -std=c11 -Wall -Wextra -I../.. -I. \
   -o "$OUT/test_usbhub_retry" \
   test_usbhub_retry.c ../../usbhub_retry.c
"$OUT/test_usbhub_retry"
