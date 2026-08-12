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

### Stage 4 — PD / Alt-Mode — PASS after sink-path implementation (2026-08-11)

- First monitor attach stalled in PD state `device`: the vendored driver
  only implemented the source role (the SPEC deliberately scoped out
  monitors that present Rp — i.e. every real charging-capable USB-C
  monitor). Implemented the sink path per the SPEC's own sketch: sink
  PD contract → DR_Swap → DFP → existing VDM ladder (752b2d33; also
  fixed: FUSB302 BMC TX never enabled in sink role, hardcoded TX header
  roles, GoodCRC DATAROLE after swap).
- **PASS: `DP-ACTIVE SNK/DFP`, both cable orientations (CC1+CC2), mux
  configured (reg 0x0A=0x0A verified in-chip), HPD delivered, led[1].**

### Stage 5 — link training — IN PROGRESS (2026-08-11)

- **AUX receive is electrically unreachable on 1.0a3** — board matches
  the TI TUSB1046A reference (Fig 28) + datasheet-mandated AUX CM bias
  exactly; the reference assumes a GPU-class AUX PHY (internal RX bias
  behind own caps), which an FPGA LVCMOS pin is not. No field mod
  possible. → DP core gained BLIND_SINK open-loop link policy
  (5f83f4e3): writes transmit, replies assumed, HPD-gated, D0 wake
  added (stock core never woke the sink).
- **AUX transmit proven end-to-end by TI silicon**: mux DCI snooper
  reg 0x12 read back LANE_COUNT_SET=2 — our DPCD write, decoded off
  the wire by the TUSB1046A.
- Current state: all LEDs (HPD + link_established + video_live)
  assert on monitor attach; **no picture**. led2/3 are now blind-mode
  self-report, not sink truth. Verified correct by inspection: fabric
  lane crossover (lane0→LN3, lane1→LN2), .ipc lane/invert config
  (TXBITPOLARITYINVERT=true on Q0 L2+L3, QPLL0, REFCLK1).
- Monitor behavior: **"no signal" then sleeps** — sink never locks the
  main link (not a stream/MSA rejection). Control test: **MacBook +
  same cable + same monitor works** — monitor/cable/Alt-Mode path all
  good; fault is board-side in the main-link signal.

#### ROOT-CAUSE CANDIDATE (overnight I2C investigation, 2026-08-12)

**TUSB1046A DP receiver EQ is latched at 12.3 dB from floating straps.**
Evidence chain, each link verified:
1. PCB netlist: U11 SSEQ0/A0 (pad 11) and DPEQ0/A1 (pad 14) are
   explicitly unconnected — floated per SPEC for I2C address 0x12.
2. Datasheet Table 1: floating = 4-level "F"; the 4-level pins latch at
   reset and the strap resistors then disconnect.
3. Datasheet Table 7: DPEQ1/DPEQ0 = F/F → **EQ setting 10 = 12.3 dB at
   4.05 GHz, all DP lanes**.
4. Live register dump ('x'): 0x10 = 0x11 = 0xAA (setting 10 ×4 lanes),
   General EQ_OVERRIDE = 0 (strap-sampled EQ active).
5. Channel needs ~1 dB (few cm of PCB): ~11 dB over-equalization; the
   linear redriver re-drives a destroyed eye at 2.7 Gb/s → sink cannot
   lock — while the MacBook path (no mux) drives the monitor fine.
6. SPEC.md anticipated this: "Receiver EQ is register-settable
   (DPxEQ_SEL) instead of strap resistors" — the firmware just never
   programmed it (wrote CTLSEL/FLIPSEL/HPDIN_OVRRIDE only).

Fix committed (f5abb424, NOT yet on the board): program DPxEQ_SEL=0
(1.0 dB) + EQ_OVERRIDE before lane enable; telnet 'e' cycles EQ
0/3/6/10 live for A/B; I2C mutex (kills the 0xEE read races); PD
discovery auto-retry after VDM flakes (no more replug-to-retry).

#### MORNING RUNBOOK (one PC visit)
1. Board → PC. `make -C boards/a2mega/src/a2fpga_esp32 upload
   PORT=/dev/cu.usbmodem5101` (serial; JTAG uninvolved).
2. `FS=boards/a2mega/impl/pnr/a2mega_dp_test.fs tools/flash.sh a2mega`
   — flash holds the mode-1 bit-reversal experiment; this restores the
   canonical mode-0 build (already rebuilt, timing-clean).
3. Board → monitor. Expected: heartbeat + led1 (HPD) + led2 (135 MHz
   check) + led3, and — if the EQ chain is the root cause — colorbars.
4. Verify over telnet regardless: 'x' must now show 0x10=0x11=0x00 and
   General bit4 set (clean reads — mutex). If no picture: 'e' cycles EQ
   presets live; also confirm D0 snoop (0x12[6:5], anomaly still open).
- Open suspects (ranked): actual line rate out of the QPLL (verify
  clk_sym=135 MHz in-fabric vs the 50 MHz crystal — no external gear
  needed), GTR12 serialization bit-order/polarity semantics (CSR blob
  not inspectable — never verified on silicon), training dwell too
  short for slow-adapting sinks, swing/EQ/SI. Next: D0 build test →
  instrumented build (freq-check LED + longer dwells) → A/B bit-order
  and polarity builds.
