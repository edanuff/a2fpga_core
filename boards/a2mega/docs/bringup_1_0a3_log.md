# a2mega 1.0a3 first-article bring-up log

Per-unit results for the 5 PCBWay units, following
`BRINGUP_1_0A3_PLAN.md` §5. Binary hashes are ground truth (provenance
rule). Boards labeled #1–#5; #1 = first article, #5 = untouched
reference.

## Board #1

### Stage 1 — electrical (no SOM) — PASS (2026-08-11)

- Visual + unpowered short check: clean.
- Powered via USB-C from PC:
  - VBUS: **5.06 V**
  - +5V (post PTC → LM66100): **4.92 V** (~140 mV drop — nominal)
  - +3V3 (SY8089): **3.31 V**
  - Idle current: **38 mA** (ESP32 module only — in expected band)
- VBUS_SRC_EN (IO46): low — board not sourcing.

### Stage 2 — ESP32 / WiFi / telnet — (in progress)
