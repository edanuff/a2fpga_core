#!/usr/bin/env bash
# Host tests for the ESP32 USB-C stale-session guard.
set -e
cd "$(dirname "$0")"
cc -std=c11 -Wall -Wextra -I../.. -I. \
   -o /tmp/test_stale_guard \
   test_stale_guard.c mock_fusb302.c ../../usbc_port.c
/tmp/test_stale_guard
