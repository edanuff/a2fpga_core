# a2mega 1.0a3 first-article bring-up log

Per-unit results for the 5 PCBWay units, following
`BRINGUP_1_0A3_PLAN.md` §5. Binary hashes are ground truth (provenance
rule). Boards labeled #1–#5; #1 = first article, #5 = untouched
reference.

## Board #1

### Stage 1 — electrical — PASS (2026-08-11)

**Correction: the SOM was fitted during these measurements** (deviation
from the runbook's no-SOM sequence; harmless in hindsight — the FPGA was
unconfigured, see Stage 3).

- Visual + unpowered short check: clean.
- Powered via USB-C from PC:
  - VBUS: **5.06 V**
  - +5V (post PTC → LM66100): **4.92 V** (~140 mV drop — nominal)
  - +3V3 (SY8089): **3.31 V**
  - Idle current: **38 mA** — initially alarming with a SOM fitted, but
    consistent with ESP32 + *unconfigured* GW5AT-60 (factory-blank SOM
    flash). Re-measure with a bitstream loaded.
- VBUS_SRC_EN (IO46): low — board not sourcing.

### Stage 2 — ESP32 / WiFi / telnet — PASS (2026-08-11)

- Firmware flashed over USB-C; no BOOT-button intervention needed on
  this unit (IO3 strap: no adverse effect observed on boot/flash so far).
- I2C: both PD chips ack (no `[usbc]` init warnings) — TUSB1046A (0x12)
  + FUSB302B (0x22) alive on IO1/IO2.
- **Two firmware bugs found and fixed on this boot** (see commits
  5c1cfc1a, 2fb9de1c): (1) WiFi/telnet was FPGA-gated in
  start_subsystems() — never came up without an A2FP link; now
  network-first. (2) `status` printed floating-bus junk as DDR3
  telemetry over a dead link ("CALIBRATED retries=254 seq=0xFE") —
  now gated on `fpga_link_ok()`.
- WiFi joined 'edhomelab' from NVS (`wifi` CLI). Telnet: pending
  explicit check (use `net` for the IP, port 23).
- Note: FPGA DONE read HIGH with an unconfigured FPGA — the ESP32
  pull-up, not the FPGA. Don't trust DONE without a link or JTAG check.

### Stage 3 — SOM + JTAG — detect + SRAM load PASS (2026-08-11)

- `openFPGALoader -c esp32s3 --detect` through the ESP32 bridge:
  **GW5AT-60, IDCODE 0x1481b** — proves TCK/TMS/TDI + relocated TDO
  (IO39) end-to-end.
- Colorbars `a2mega_dp_test.fs` SRAM-loaded via the bridge: DONE.
- Current with colorbars configured: **0.40 A** @ VBUS (~2 W) — expected
  band for ESP32 + GW5AT-60 with SERDES quad + PLLs up. Confirms the
  38 mA Stage-1 reading was the unconfigured-FPGA state; power path
  fully coherent (unconfigured 38 mA → configured 400 mA).
- led[0] heartbeat: confirmed blinking. Telnet: confirmed (console
  mirror answers on port 23, boot backlog replays).
- **Flash-write saga (~2 h, all root-caused).** First flash write used
  bare `openFPGALoader -f` instead of the validated `tools/flash.sh`
  recipe (bulk-erase + verify) → unverified corrupt image → GW5A MSPI
  boot-retry loop owned the bus: FPGA DONE stuck LOW, JTAG chain dead
  ("no device found") even across cold boots — openFPGALoader's own
  flash entry resets the fabric, re-arming the loop (the 1.0a2a-era
  `fpgaflash keepsram` comment describes this exact trap; the tooling
  for it already existed and should have been consulted FIRST).
  Recovery that worked: race an **SRAM load** against the boot window
  at replug (openFPGALoader retry loop, ~0.2 s spacing; captured on
  attempts 115/15/1) → fabric satisfied, bus free → `fpgaerase` CLI
  (header-block erase via keepsram SPI) → flash boots as blank →
  validated flash.sh write with verify → **heartbeat from flash after
  power cycle: PASS.**
- Two NEW firmware bugs found and fixed during the rescue:
  (1) `fpgaerase` bit-banged floating pins — after any USB JTAG bridge
  session the disconnect handler leaves TCK/TMS/TDI as INPUTs and the
  inline command never re-initialized them ("block 0x00000 TIMEOUT");
  (2) worse and general: while the USB-JTAG bridge is matrix-routed,
  `gpio_config()` does NOT reclaim the pads, so ALL bit-bang JTAG
  (fpgaerase, menu self-update) wiggled nothing. Fix: fpga_jtag
  init/release_pins now explicitly hand the pads between the USB-JTAG
  peripheral and GPIO (bridge released during bit-bang, restored after).
- Firmware also gained: early JTAG bridge routing at top of setup()
  (rescue window), telnet `p` key = PD status via the console tee.

### Stage 4 — PD / Alt-Mode — (next: swap to monitor)
