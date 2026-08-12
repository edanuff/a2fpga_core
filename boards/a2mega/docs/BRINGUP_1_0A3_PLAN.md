# a2mega 1.0a3 Bring-Up Plan — USB-C DisplayPort

Status: **HARDWARE IN HAND (2026-08-11).** All pre-hardware software is done
and committed: Phases 0–1 (vendoring + ESP32 firmware), Phase 2 step 1
(colorbars bitstream `a2mega_dp_test.fs`, timing-clean), Phase 3a (full core
480p-over-DP) and 3b (full core 1080p, `a2mega.fs` sha `2270857e…`,
timing-clean). **Execute Section 5 (hardware runbook) next.**
Hardware source of truth: `/Users/edanuff/GitHub/a2-mega` branch `1.0a3`
(HEAD `273d673 "Final Board"`). DP core: `/Users/edanuff/GitHub/DisplayPort_Verilog`
(`examples/a2_mega` is the board-locked, timing-closed SERDES recipe; `usb-c/`
holds the FUSB302B/TUSB1046A spec and the portable PD driver in C).

## 1. What changed in hardware (1.0a2a → 1.0a3)

- **HDMI, microSD, and USB-A are gone.** One USB-C port (J4) replaces them:
  4-lane DP Alt Mode (pin assignment C/E — all four SS pairs are DP, there is
  **no SuperSpeed USB ever**) + USB2 device (ESP32 IO19/20 wired straight to
  the connector, orientation-agnostic) + USB-PD.
- **DP path:** FPGA SERDES Q0 → BTB1 (J2) → TUSB1046A (U11, linear
  redriver/crosspoint, **I2C mode addr 0x12**) → J4. AUX (`DPAUX_P/N`,
  J2.64/62) comes from FPGA fabric through the mux onto SBU1/2 — **link
  training, DPCD, EDID all stay in the FPGA** (`dp_transmitter` does this).
- **PD/Type-C:** FUSB302B (U12, I2C addr 0x22) sees CC; the ESP32 owns the
  entire PD/Alt-Mode policy in firmware. Shared I2C bus: IO1=SCL, IO2=SDA;
  FUSB INT_N on IO3.
- **HPD is not a wire.** Both TUSB1046A HPD pins are NC. The ESP32
  reconstructs HPD level+IRQ from DP Status/Attention VDMs, sets
  `HPDIN_OVRRIDE` (TUSB1046A reg 0x0A bit 3), and must deliver HPD to the
  FPGA itself. Plan: dedicate one of the six spare `ESP32_GPIO0..5` lines
  (IO33-38 → J2.17/13/11/7/5/4) as an HPD level/pulse wire (0.5–1 ms IRQ
  pulse per `hotplug_decode.v`; `usb-c/rtl/usbc_dp_control.sv` generates it).
- **SERDES facts (die-true, from the BTB1 schematic note):** refclk =
  DSC1103 **135 MHz** LVDS XO on **Q0_REFCLK1**, QPLL0. Lane map is
  non-identity: DP0←L3 (P/N swapped), DP1←L2 (swapped), DP2←L1 (swapped),
  DP3←L0 (NOT swapped — *unverified*; Sipeed sheet disagrees with the die
  pinout; resolve on hardware before enabling 4-lane). The 2-lane IP
  (L2+L3 bonded, master L2, `tx_pol_invert` both, 2.7G HBR, fabric 8b10b)
  is already generated and timing-closed in `examples/a2_mega`
  (+0.020 ns WNS, TNS 0, ~2.9k LUT / 13 BSRAM / 2 PLLA).
- **ESP32-S3 pinout reshuffled almost everywhere:** OPI D0-7 = IO11-18
  (CS=IO21, CLK=IO47); PPO narrowed 8→4 bit (D0-3=IO6-9, CLK=IO10,
  SYNC=IO5); JTAG TDO moved IO45→**IO39** (TCK/TMS/TDI unchanged IO40/41/42);
  `ESP32_INT_N`=IO4 (new, to J2.67); `VBUS_SRC_EN`=IO46 (strapping pin,
  pull-down — board can *source* 5 V out the port via TPS2553, 1 A limit);
  UART0 TX/RX still to FPGA (J2.66/68). IO3 (strap: JTAG source select) now
  carries FUSB INT_N with a pull-up — verify boot behavior on first power-up.
- **Power:** slot 5 V (LM74700 ideal diode) OR USB-C VBUS (PTC → LM66100)
  → +5V; SY8089 buck → +3V3 (LDO gone). Dead-battery attach works.
- **GS:** RDY split into `FPGA_GS_RDY` (in, J3.36) + `FPGA_GS_RDY_OUT`
  (out via 74LVC2G07 open-drain U15, J3.46); `FPGA_GS_VP` now output-only.
- **Apple II bus sheet: unchanged.** a2bus gateware ports over as-is.
- LEDs D1-D4 → J2.34/36 + J3.55/57 (active-low); DIP SW1 → J2.48/44/42/40.
- SI caveat: no controlled-impedance/length-match rules exist for the DP
  pairs in the 4-layer stack. **HBR (2.7 Gb/s) first**; HBR2 only if the
  first article proves clean.
- Heads-up: branch `1.0a4` adds HyperRAM and moves GS nets again.

## 2. Consequences for the current software stack

| Today (main) | 1.0a3 impact |
|---|---|
| `hdmi` encoder + OSER10 serializer + ELVDS pins | Dead. Replace with `dp_transmitter` (pull-style: it *owns* `clk_pixel` and asks for `rgb` at `(cx,cy)` — same shape as the HDMI core's cx/cy, clean swap at the `framebuffer_480p` seam). |
| `pll_ddr3` clocked from 27 MHz `clk_pixel` | Must re-source (50 MHz crystal) — in DP the pixel clock derives from the 135 MHz symbol clock and will change with link mode. Do this **before** touching the pixel domain (DDR3 calibration fragility is documented). |
| FPGA-fabric `usb_hid_host` + `pll_usb` (USB-A gamepad) | Dead — no USB-A. Menu loses the gamepad. Near-term replacement: telnet ANSI menu mirror (BL616 precedent `firmware_host/telnetd.c`, "works padless"). |
| micro-SD storage (`/sdcard`), `wifi.txt` config | Gone. WiFi config → NVS (settings v4 already supports it). Disk images → LittleFS (~1.9 MB `no_ota` partition; .dsk = 140 KB, ~a dozen fit) uploaded via FTP/telnet, or USB-MSC from a PC later. FPGA update-from-file will **stream to JTAG** (USB stick / network source) rather than staging in MCU flash — staging a ~2.6 MB image in flash is wasteful regardless of module. JTAG from a PC is unaffected. |
| ESP32 module | 1.0a3 ships with the **ESP32-S3-MINI-1-N4R2** (4 MB flash + **2 MB quad PSRAM**, same module as the a2p25); the footprint accepts either MINI variant (pin-compatible), so the N8 (8 MB, no PSRAM) can also be fitted. N4R2 was picked when DOC-on-ESP32 was anticipated — now known unnecessary — but the PSRAM may still be needed for the dramatically larger buffers WiFi throughput wants. Either module supports the current bring-up path. Build FQBN differs per rev — the Makefile selects it from `BOARD_REV`. |
| ESP32 pin map in firmware | Full rewrite behind a board-rev header (keep 1.0a2a buildable). |
| Serial console via USB-C to a PC | Only available when the port is *not* driving a monitor → **WiFi telnet console is a prerequisite for DP debugging**, exactly as anticipated. |

## 3. Phases

### Phase 0 — Branch + vendoring (now, no hardware)
1. Create the long-lived tracking branch `a2mega-1.0a3` from `main` in
   a2fpga_core (this worktree branch is the prep area; PR pieces into the
   tracking branch per the AGENTS.md worktree policy).
2. Vendor the DP core: copy `DisplayPort_Verilog/src/**` →
   `hdl/displayport/` with a provenance README (same pattern as the vendored
   `hdl/hdmi` core), and `examples/a2_mega/src/serdes/**` (generated PHY IP,
   incl. the `.csr` sidecar — must be registered in `.gprj.user` or PnR
   fails CM2031) → `boards/a2mega/hdl/gowin/60B/dp_serdes/`. Keep
   DisplayPort_Verilog as the upstream dev sandbox.
3. Vendor the PD driver: `usb-c/src|include/**` + `examples/esp32s3_integration.*`
   → `boards/a2mega/src/a2fpga_esp32/` (it's portable C with an integration
   contract; `make -C usb-c check` runs its host-side tests).

### Phase 1 — ESP32 firmware for 1.0a3 (pre-hardware, finish on arrival)
1. **Pin-map port** behind `BOARD_REV_1_0A3`: OPI, PPO(4-bit), JTAG TDO
   IO39, INT_N IO4, I2C IO1/2, FUSB INT IO3, VBUS_SRC_EN IO46 (never drive
   high before straps latch; leave low unless sourcing).
2. **WiFi + telnet first**: config from NVS (no SD), port the BL616
   telnetd + ANSI menu mirror onto ESP-IDF sockets. Follow the BL616 lwIP
   lessons (no blocking in service tasks). This becomes the primary console
   the moment the USB-C port is occupied by a monitor.
3. **PD stack**: integrate `usbc_port.c`/`fusb302.c` (≥1 kHz task, INT_N
   edge wake), TUSB1046A I2C driver (FLIPSEL by CC orientation,
   `CTLSEL=10`, `HPDIN_OVRRIDE`, EQ regs; AUX snooping trims lanes
   autonomously). Gate the VDM ladder on FPGA-ready. Emit HPD level+IRQ on
   the chosen `ESP32_GPIO*` pin. Source PDO stays 5 V/1 A default.
4. **JTAG bridge**: retarget TDO to IO39 so `openFPGALoader -c esp32s3`
   and `fpgaupdate` keep working — this is the only FPGA programming path.
5. Storage: LittleFS partition + FTP upload path for disk images (existing
   ftpd); disk.c backend swap SD→VFS. (Can trail the display work.)

### Phase 2 — Colorbars over DP (first hardware milestone)

> **Status 2026-08-09: bitstream BUILT & timing-clean** (step 1 done
> pre-hardware): `boards/a2mega/a2mega_dp_test.gprj` →
> `impl/pnr/a2mega_dp_test.fs`; 0 violations, TNS 0.000, all Fmax ≥
> constraint (clk_sym 135.352 vs 135.007). Pins from
> `docs/a2mega_pinmap_1_0a3.csv`. Gotchas encountered: Gowin needs
> `impl/<proj>_process_config.json` with `Verilog_Standard: Sysv2017`,
> `TopModule`, and a distinct `OUTPUT_BASE_NAME` (else it silently
> overwrites the main project's outputs). The bring-up ladder (step 2)
> awaits hardware.

1. New minimal bring-up project `boards/a2mega/a2mega_dp_test.gprj`:
   `dp_transmitter` (2-lane HBR, 1080p59.94 148.5 MHz — the exact
   timing-closed config), a trivial `cx/cy → rgb` colorbar generator (none
   exists upstream; the examples use an XOR gradient), AUX tri-state idiom
   from `top_tang_mega.sv`, HPD in from the ESP32 GPIO, 50 MHz osc →
   `clk100` mgmt PLL. Real `.cst` for AUX/HPD/LED/clk pins (SERDES pads and
   refclk are dedicated bumps — no constraint needed; derive BTB→ball for
   the rest from the SOM pinout tables; `examples/a2_mega` deliberately has
   no .cst). SDC per the example, incl. the LANE2 `FABRIC_CLK` anchor.
2. Bring-up ladder, each observable via telnet/LED (`debug[7:0]`, `link_established`,
   `video_live` wired to OSPI-readable regs + LEDs):
   a. ESP32 flashes via USB-C from a PC; I2C scan sees 0x22 + 0x12.
   b. WiFi + telnet up.
   c. FPGA programs over the ESP32 JTAG bridge (SRAM loads first).
   d. Plug USB-C monitor: FUSB302 attach → Discover/Enter DP mode →
      mux configured → HPD asserted to FPGA.
   e. AUX traffic / link training completes (`link_established`).
   f. **Colorbars on screen.** Soak for link stability (SI check at HBR).
3. Only after 2-lane is solid: probe DP3/L0 polarity question (needs the
   4-lane IP regenerated; not on the critical path).

### Phase 3 — Apple II core over DP
1. Re-source `pll_ddr3` off the pixel clock (→ 50 MHz crystal input) as an
   isolated, separately-verified commit (DDR3 calibration is fragile).
2. Swap the output stage in `top.sv`: delete hdmi/serializer/ELVDS, wire
   `dp_transmitter`; `framebuffer_480p`'s pixel-side interface
   (`clk_pixel, cx, cy, r/g/b`) is the seam. Two sub-steps:
   - **3a (quick):** run DP at 720×480 (pixel clock 27 MHz = 135 × 2/10;
     bandwidth trivially fits) so `framebuffer_480p`, OSD, and DebugOverlay
     port unchanged. Risk: some USB-C monitors reject 480p — acceptable as
     a lab step, not the product.
   - **3b (target):** 1080p per `tang_mega_138k_analysis.md` — scan-out
     scaler in the 148.5 MHz pixel domain (integer ratios: 560×3=1680 wide
     + borders, ×… vertical; FB still stores source-res), bigger ping-pong
     line buffer, timing closure at 148.5 MHz. This is the main new HDL work.
3. Audio: `dp_transmitter` takes the same 48 kHz strobe + stereo samples as
   the HDMI core — port directly.
4. GS RDY split + LED/DIP pin moves in `.cst`/`top.sv`; delete
   `usb_hid_host`/`pll_usb`.
5. Regression on a real IIgs: boot, OSD/menu via telnet mirror, disk serve
   from flash FS, Ensoniq audio, W5100-over-WiFi.

### Phase 4 — Later / after DP is reliable
- Introduce a USB-C dock/hub scenario. Note the constraint baked into
  1.0a3: the port is always 4-lane-DP + USB2; a dock's USB path is USB2
  only, and the ESP32-S3 is wired as a *device* — hosting Ethernet or HID
  through a hub would need ESP32 OTG host mode (FS, 12 Mb/s) and a role
  strategy. Ethernet-over-WiFi (existing MAC-NAT bridge) already covers
  networking meanwhile.
- 4-lane DP / HBR2 (after L0 polarity + SI verification), 138K variant,
  HyperRAM (1.0a4).

## 4. Risks / open questions

1. **DP3/L0 polarity** unresolved by design docs — hardware experiment.
2. **DP pair SI** — no impedance-controlled routing rules were set; HBR
   first, scope/soak before HBR2 ambitions.
3. **IO3 strap** (JTAG select) now pulled up by FUSB INT_N — confirm the
   ESP32 still boots/flashes normally on first power-up.
4. **Monitor tolerance**: some USB-C monitors demand specific PDOs or
   refuse 5 V/1 A sources oddly; test across ≥2 monitors/dongles early.
5. **480p-over-DP acceptance** (Phase 3a) is monitor-dependent — treat as
   optional scaffolding; 1080p (3b) is the real deliverable.
6. **No hardware bring-up has ever been done on the DP core** — it is
   simulation-verified only. Budget time at step 2e (link training) —
   that's where fabric-8b10b/scrambler/SERDES assumptions meet reality;
   the `misc/` golden-model checkers and `tb_*` benches are the reference
   when AUX traces look wrong.
7. **First-article checklist before any slot insertion**: visual inspect,
   bench-power via USB-C (current-limited), rail check (+5V/+3V3), then
   ESP32 flash — before the board ever sees an Apple II.

## 5. Hardware bring-up runbook (boards in hand, 2026-08-11)

### 5.0 Read this first: the one-port problem

Everything on this board funnels through the single USB-C port (J4):

- **PC connection** = ESP32 flashing (native USB-Serial/JTAG) **and** FPGA
  programming (`openFPGALoader -c esp32s3` through the ESP32's USB-JTAG
  bridge, `route_usb_jtag_to_gpio()`).
- **Monitor connection** = DP Alt Mode — the thing we're bringing up.
- **On the bench, J4 is also the board's only power** (monitor or PC
  sources VBUS; dead-battery attach works).

Consequences that shape the whole sequence:

1. You can never program and watch the monitor at the same time.
2. On the bench, an FPGA **SRAM load dies at every cable swap** (power
   drops). Bench iteration therefore writes to **SPI flash** (survives the
   swap) — slower per cycle but stateless.
3. **In an Apple II slot, the board keeps slot 5 V across cable swaps** —
   SRAM loads survive PC↔monitor replug. That is the fast iteration loop;
   move to it as soon as the board is trusted enough to slot.
4. Telnet (port 23: `status`, `wifi`, `pd`, `restart`, `help`, ANSI menu
   mirror) is the only console while a monitor occupies J4 — bringing WiFi
   up is a *gate*, not a nicety.

### 5.1 Stage 0 — bench prep (before powering anything)

- Label the 5 boards `#1`–`#5`. **#1 = first article** (takes all the
  risk). Keep **#5 untouched** as a known-virgin reference for "is this
  fault board-specific?" questions (see the second-machine-repro rule).
- Equipment: USB-C cable to PC; inline USB-C power meter (or
  current-limited supply feeding a C breakout); multimeter; **two or more
  USB-C DP monitors/dongles** (monitor tolerance is a listed risk);
  Tang Mega 60K SOM(s); an Apple IIgs for Stage 7.
- Software staged on the PC, hashes logged (provenance rule — binary hash
  is ground truth):
  - ESP32 firmware: `make -C boards/a2mega/src/a2fpga_esp32 compile`
    (BOARD_REV defaults to 103/N4R2).
  - `boards/a2mega/impl/pnr/a2mega_dp_test.fs` (colorbars) and
    `impl/pnr/a2mega.fs` (full core 1080p, sha256 `2270857e…`).
  - `arduino-cli` + `openFPGALoader` installed and in PATH.

### 5.2 Stage 1 — first-article electrical (board #1, **no SOM fitted**)

1. Visual inspection under magnification: J4 solder, the two QFNs
   (TUSB1046A U11, FUSB302B U12), SY8089 buck, BTB connectors clean.
2. Unpowered: ohmmeter +5V→GND and +3V3→GND — no shorts.
3. Power via USB-C from PC/power-meter. Check **+5V** (after PTC →
   LM66100) and **+3V3** (SY8089). Idle current: expect low tens of mA
   (ESP32 module only). Anything ≥250 mA without the SOM → stop, thermal
   hunt.
4. Verify **VBUS_SRC_EN behavior**: the board must NOT attempt to source
   5 V back out the port (IO46 has R29 pull-down; firmware leaves it low).

**Gate:** rails good, current sane → Stage 2.

### 5.3 Stage 2 — ESP32 alive, WiFi/telnet up (still no SOM)

1. Flash: `make -C boards/a2mega/src/a2fpga_esp32 upload PORT=/dev/cu.usbmodem*`.
   A blank module should enumerate the ROM USB-Serial/JTAG automatically;
   if not, hold BOOT + tap RESET (both buttons are on the board).
   ⚠ **IO3 strap check (risk #3):** IO3 (JTAG-source-select strap) is
   pulled up by the FUSB INT_N pull-up. Confirm normal boot + normal
   reflashing. If flashing is flaky only on first power-up, this strap is
   the suspect — document the workaround (BOOT-button entry always wins).
2. Serial monitor (`make monitor`): boot banner, then confirm **no**
   `[usbc] warning: TUSB1046A (0x12) not responding` and **no**
   `[usbc] FUSB302B (0x22) init FAILED` — that pair of checks validates
   the shared I2C bus (IO1/IO2) and both PD chips in one shot.
3. `wifi <ssid> <psk>` (persists to NVS) → `status` shows an IP →
   **telnet to port 23** from the PC; verify `status`, `pd`, and the ANSI
   menu mirror render.
4. Sanity: `pd` with nothing attached reports detached/idle.

**Gate:** telnet console works → you can afford to lose the USB console.

### 5.4 Stage 3 — SOM + JTAG path (FPGA enters the picture)

1. Power off. Mount the Tang Mega 60K SOM on BTB0/1/2 (inspect keying —
   a misaligned BTB can short rails). Power back up **watching the
   current meter**; re-verify rails. GW5A + DDR3 idle draw will jump the
   total substantially; a hot SOM or >1 A sustained at idle → stop.
2. `openFPGALoader -c esp32s3 --detect` through the bridge → GW5AT-60
   IDCODE. This proves TCK/TMS/TDI + the **relocated TDO (IO39)**.
3. SRAM-load colorbars: `openFPGALoader -c esp32s3 a2mega_dp_test.fs`.
   **led[0] blinks ~1.5 Hz** (heartbeat) = bitstream alive. (LEDs are
   active-low; D15/D14/R14/P14.)
4. Write the same bitstream to **SPI flash** (`openFPGALoader -c esp32s3
   -f a2mega_dp_test.fs`) and power-cycle: heartbeat returns on its own.
   Flash-boot is what makes bench cable-swaps tolerable.

**Gate:** heartbeat from flash after power cycle → Stage 4.

### 5.5 Stage 4 — PD negotiation / DP Alt Mode entry

Colorbars bitstream LED ladder: `led[0]` heartbeat → `led[1]` HPD →
`led[2]` link_established → `led[3]` video_live.

1. With colorbars in flash: swap J4 from PC to the **USB-C monitor**
   (board reboots on monitor power — fine, it flash-boots). Watch over
   **telnet**: `pd` should walk attach → orientation → Discover
   Identity/SVIDs/Modes → Enter DP → Status/Config VDMs → mux configured
   (CTLSEL=10 + FLIPSEL + HPDIN_OVRRIDE) → **HPD asserted to the FPGA**.
2. **led[1] on = HPD delivered.** This checkpoint is pure ESP32+PD — no
   SERDES involved yet.
3. Flip the cable (both orientations must work — FLIPSEL path).
4. Repeat on the second monitor/dongle before declaring PD good.

Failure play: `pd` trace stalls at Discover → CC/FUSB problem (check
INT_N on IO3); enters DP mode but no HPD → VDM Attention/Status parsing
or the HPD GPIO wire; mode entry refused → monitor wants different PDOs
(risk #4 — try the other monitor first).

### 5.6 Stage 5 — link training + colorbars (the moment of truth)

This is the **first-ever hardware run of the DP core** (risk #6). Budget
time here.

1. After HPD: fabric AUX churn → **led[2] = link training done** →
   **led[3] = video live** → colorbars at 1080p59.94 on screen.
2. Soak ≥30 min (SI at HBR, no impedance-controlled routing — risk #2):
   watch for dropouts/retrains. Power-cycle ×5: colorbars must return
   unattended every time.
3. led[2] never lights: AUX-layer problem — polarity/tri-state idiom,
   DPCD handshake. The `misc/` golden-model checkers and `tb_*` benches
   in DisplayPort_Verilog are the reference; compare a `pd`-observed AUX
   timeline against sim. led[2] on but led[3] off / black screen: main
   stream path (MSA/VB-ID/scrambler assumptions). Unstable image or
   intermittent retrain: SI — try the shortest cable, the other monitor,
   and only then consider dropping to RBR (needs SERDES IP regen; note it
   in the log, don't improvise it at the bench).
4. Log everything per unit in the provenance format (build sha + what was
   observed).

**Gate:** colorbars stable on 2 monitors, both orientations, across
power cycles → the DP path is real. Commit any fixes; then Stage 6.

### 5.7 Stage 6 — full core on the bench (no Apple II yet)

Full-core LED ladder differs: `led[0]` heartbeat, **`led[1]` = DDR3
calibration FAILED (lit = bad)**, `led[2]` link_established,
`led[3]` video_live.

1. Flash `a2mega.fs` (1080p full core). Swap to monitor.
2. Expect: led[1] **off** (DDR3 calibrated), link up, and a mostly-black
   1080p frame with borders + **DebugOverlay** hex/status and the OSD —
   no Apple II bus activity is fine, scan-out doesn't need it.
3. Verify the monitor actually reports 1920×1080@59.94; check overlay
   text legibility (OSD renders native-scale at 1080p — small text is a
   known cosmetic, not a bug).
4. Telnet menu mirror should drive the OSD.

**Gate:** full core scans out 1080p with DDR3 calibrated → slot time.

### 5.8 Stage 7 — Apple II slot (IIgs first)

Pre-insertion checklist: machine off; inspect card-edge for solder
bridges; card seats fully; nothing conductive under the board.

1. First power-on in slot with **no USB-C attached**: machine must boot
   normally (bus transceivers behave, no /INH mischief). Then attach the
   monitor.
2. **Fast iteration loop unlocked:** in-slot the board holds slot 5 V, so
   PC↔monitor cable swaps no longer reboot it — SRAM loads
   (`openFPGALoader -c esp32s3 file.fs`, no `-f`) now survive the swap.
   Bench = flash writes; slot = SRAM iterate.
3. Regression sweep (the Phase 3.5 list): IIgs boots; Apple video correct
   at 1080p (40/80-col text, hires, DHGR, SHR); OSD/menu via telnet
   mirror; disk serve from LittleFS (FTP a .dsk up first — ~a dozen fit);
   Ensoniq audio over DP audio (48 kHz SDP path — first hardware test of
   sdp_engine too); W5100-over-WiFi; GS RDY split (`FPGA_GS_RDY_OUT` via
   the new open-drain driver — watch for bus contention symptoms).
4. Soak: leave a GS demo/game running ≥1 hr.

### 5.9 Stage 8 — fleet + wrap-up

- Repeat Stages 1–3 + colorbars on board **#2** to separate "design
  works" from "unit #1 works" before deep-diving any anomaly.
- Log per-unit results + build hashes; update this doc's risk list with
  what reality said (esp. IO3 strap, monitor matrix, SI margin).
- Then the deferred queue, in order of value: DP3/L0 polarity probe
  (needs 4-lane IP regen — now safe to schedule); stream-to-JTAG FPGA
  update (removes the PC from the loop); 3× OSD text scaling; HBR2
  experiment only if the SI soak was spotless.
