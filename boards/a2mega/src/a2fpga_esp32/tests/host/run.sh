#!/usr/bin/env bash
# Host tests for the ESP32 USB-C port state machine:
#   test_stale_guard.c — stale-session guard
#   test_dr_swap.c     — sink+UFP -> DFP Data-Role Swap (powered adapters)
set -e
cd "$(dirname "$0")"
for t in test_stale_guard test_dr_swap; do
    cc -std=c11 -Wall -Wextra -I../.. -I. \
       -o /tmp/$t $t.c mock_fusb302.c ../../usbc_port.c
    /tmp/$t
done
