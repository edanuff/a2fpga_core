# a2p25 displayport1 — full-core integration plan (track B3 + B5)

Status 2026-10-11: B4 (framebuffer on PSRAM) has first light. `sound_glu` /
`doc5503_pipelined` (B3) and the shadow memory (B5) both hang off the Apple
bus, so neither has a standalone bench: they arrive with the new `top.sv`.
This document is the port plan for that top. Parent plan:
[MODERNIZATION_PLAN.md](MODERNIZATION_PLAN.md); ball map:
[PINOUT_DISPLAYPORT1.md](PINOUT_DISPLAYPORT1.md).

## 1. Approach: derive the new top from the a2mega top

The a2mega 1.0a3 top (`boards/a2mega/hdl/top.sv`, 2042 lines) already has the
architecture this board needs: ESP32 OSPI link with the Enhanced feature set
(disk/HDD images, Uthernet, OSD, gamepad), `apple_bus` with IIgs autodetect,
shadow memory on memory ports, `framebuffer_1080p`, DP output, DOC on memory
ports. The current `boards/a2p25/hdl/top.sv` is the *previous* board rev's
core (HDMI, ESP32 I²S audio, QSPI link, BSRAM-only memory, no DP, no DOC, no
GS autodetect) and is retired by this work.

The new top = a2mega top with these substitutions:

| Area | a2mega 1.0a3 | a2p25 displayport1 |
|---|---|---|
| Memory | DDR3 IP + `ddr3_ports` (7 ports, CDC to 81 MHz) | `hyperram_ctrl` + `hyperram_ports_if` (same 7 ports, no CDC, all on 54 MHz) — **proven in B4** |
| Clocks | `board_plls` (DDR3 324 MHz, pixel, logic, USB) | `Gowin_PLL` (54 MHz logic) + `gowin_mgmt_pll` (100 MHz DP mgmt) + `hr_clk_pll` cascade (54/108/108+90°) + D-PHY PLLA (81 MHz symbol) + `gowin_pixel_pll` (148.5 MHz) = 5 of 6 PLLAs |
| DP PHY | GW5AT SERDES, 2 lanes HBR, schedule ROM + CSR replay | hardened D-PHY via `transceiver_bank_dphy`, **4 lanes RBR 1080p60** (`dp_transmitter` DP_VENDOR_DPHY, `ECHO_SINK_ADJUST=1`, `LINK_CHECK_CLKS` 200 ms, `HPD_DISCONNECT_RESETS=1`) — proven in `a2p25_dp_test_4l` |
| HPD | FPGA pin | ESP32 IO10 → FPGA B2 (PD-derived) — proven |
| AUX | `dp_aux_pad` on the a2mega pair | same `dp_aux_pad`, L2/L1 LVDS25 PULL_MODE=DOWN BANK_VCCIO=3.3 — proven |
| ESP32 link | `esp32_ospi_connector` on `esp_sclk` + `esp_data[7:0]` | same module; CLK D1, D0–D7 = C2 F2 F1 A1 K1 K2 J4 K4. (CS on E1 and INT_N on B2 are not used by the a2mega connector; keep them as inputs/HPD.) |
| Apple bus | a2mega `apple_bus` (`GS_AUTODETECT=1`, `sw_gs_i` = DIP, `reset_hold_i`) | same module, `sw_gs_i = 0` (no DIP), `a2_res_out_n` **not an FPGA pin** on this rev (ESP32 IO33 drives /RES) — leave the port unconnected; the ESP32 owns reset hold. D7 is on **B3** |
| Audio | DP in-band (`audio_sample_word` from `apple_speaker` + `sound_glu`) | identical — no I²S hardware on this rev |
| LEDs / DIP / button / UART | 4 LEDs, 4 DIPs, button, UART | 1 LED (B10 heartbeat); no DIP (scanlines/speaker/slot7 become OSD settings or parameters); no button; no UART pins (telemetry UART of the bench builds went out on the OPI D0 ball — not available once the OPI link is live) |
| F18A / SuperSprite, Mockingboard, SSC, DiskII, HDD, Uthernet2, OSD | all present | **see §3 budget** |

## 2. Memory map (8 MB HyperRAM, 32-bit word addresses) and ports

| Port | Client | Base (words) | Burst | Notes |
|---|---|---|---|---|
| 0 | shadow read (`apple_memory`) | `SHADOW_WORD_BASE` 0x050000 | 4 | highest priority, as a2mega |
| 1 | shadow write | 0x050000 | — | |
| 2 | DOC wavetable read (`sound_glu`) | `ENSONIQ_WORD_BASE` 0x080000 | 4 | above FB: tiny traffic |
| 3 | GLU sound-RAM write | 0x080000 | — | above FB: a dropped write corrupts the wavetable |
| 4 | FB write (wide, 4 words) | `FB_WORD_BASE` 0x000000 | — | `WIDE_WR_PORT` |
| 5 | FB read | 0x000000 | 8 | |
| 6 | debug reader (ESP32) | 0 | — | lowest |

Same layout as the a2mega so the ESP32 firmware's debug reader and the
documentation stay valid. `hyperram_ports_if` takes the bases and burst
lengths as packed tables (port 0 in the low field). The controller's
calibration block is the top 64 B of the device (`CAL_ADDR`), far above any
client region.

Bandwidth: the B4 bench measured the FB alone at a write-FIFO high-water of
11/256 and a read-FIFO peak of 5/16 with zero stalls; shadow and DOC traffic
is small and short. The arbiter is one-transaction-at-a-time (14–29 clocks
per transaction), so if the shadow port ever shows drops the fix is the
pipelined-port follow-on noted in `hyperram_ctrl.sv`, not a bigger buffer.

## 3. Resource budget — the scoping decision

GW5A-25: 23,040 logic cells (11,520 CLS), 56 BSRAM (18 Kb), 28 DSP.

| Design | Logic | CLS | BSRAM |
|---|---|---|---|
| a2mega 1.0a3 full core (GW5AT-60) | 20,311 | 19,959 / 29,952 | 108 / 118 |
| a2p25 B4 bench (DP 1080p×4 + HyperRAM FB + telemetry) | 10,358 | 8,824 / 11,520 (77%) | 38 / 56 |
| old a2p25 core (prev rev: HDMI, SuperSprite/F18A, Mockingboard, SSC, BSRAM shadow) | 8,069 | 6,216 / 11,520 | 45 / 56 |

The a2mega feature set does not fit the 25K as-is (CLS alone is 1.7× the
device). The B4 bench already packs 77% of the CLS, though ~1.5k of that is
bench-only (telemetry UART lines, AUX capture, synthetic renderer, colour
bars), and the DP test's `transceiver_bank_dphy` + `dp_transmitter` at 4
lanes is the fixed cost of video on this board.

Rough additive estimate for the core on top of DP + FB + HyperRAM (~9k logic):

| Block | Logic (est.) | BSRAM |
|---|---|---|
| `apple_bus` + `apple_memory` with shadow in PSRAM (`VGC_IN_SDRAM`) | ~2.5k | ~8 (ROM/soft-switch state; the shadow leaves BSRAM) |
| `apple_video_gen` + `vgc_gen` + `framebuffer_writer`×2 + `scan_timer` | ~3k | ~6 |
| `sound_glu` + `doc5503_pipelined` (FF banks) + `apple_speaker` | ~3k | 0–1 |
| `esp32_ospi_connector` + proto + OSD overlay + `DebugOverlay` | ~2.5k | ~4 |
| DiskII + HDD + Uthernet2 (ESP32-served) | ~2.5k | ~6 |
| Mockingboard (2× 6522 + 2× YM2149) | ~2k | 0 |
| SuperSprite (F18A VDP) | ~5k | ~16 |
| SuperSerial | ~0.8k | ~1 |

Everything except SuperSprite ≈ 25k logic: **over the 23k device before
placement margin**, and CLS packing on the 25K has been the limiter (the B4
bench is at 77% CLS with 45% logic). A realistic first full core for this
board is therefore:

- **In:** Apple II/IIe/IIgs video (incl. SHR, shadow in PSRAM), DP 1080p×4
  with audio, DOC/Ensoniq on PSRAM, ESP32 link (disk/HDD/Uthernet/OSD/
  gamepad), Mockingboard, SSC.
- **Out, first pass:** SuperSprite/F18A (the single largest block, 16 BSRAM
  + ~5k logic). Re-evaluate once the real numbers are in; the 25K may take it
  if the ESP32-served peripherals are trimmed instead (e.g. Uthernet2).

This is a product decision — see "Decisions" below.

**Measured 2026-10-11 (first full-core builds, SuperSprite out):**

| Build | Logic | CLS | BSRAM | Result |
|---|---|---|---|---|
| everything else in | — | — | 67 | BSRAM over (Uthernet2 10, DOC 8, FB line buffer 9 + write FIFO 5, DP pixel FIFO 11, ESP32 disk lanes 8 + HDD/OSD 3, D-PHY bank FIFO 3, ROMs/PLL-init 6) |
| Uthernet2 off (RAM now generate-guarded), line buffer stored as RGB565 (−1), DP pixel FIFO 1 line / half-line prefill (−8, new `FIFO_LINES`/`PREFILL_WORDS` params, a2mega unchanged), bank FIFO in registers (−3) | 20,344 (89%) | 11,092 (97%) | 56 (100%) | placement failed (95 regs unplaced) |
| + Mockingboard off | 18,472 (81%) | 11,448 (99%) | 56 | routed, 20 setup violations (worst −0.34: DebugOverlay, DOC, HyperRAM ck_en, framebuffer, scanline dim, AUX) |
| + DebugOverlay off (ESP32 OSD stays) | 17,991 (79%) | 11,397 (99%) | 54 | **0 violations, worst +0.09 ns** — first core bitstream `0c651f95…` (thin; the +0.5 ns durability bar is still ahead) |

Note (ed): CLS % this high is the placer spreading cells to close timing, not design logic; with logic at ~80% the remaining work is the timing cones themselves (DOC, HyperRAM ck_en half-cycle, framebuffer, scanline dim, AUX), not more feature removal.

So the 25K takes video + DP + DOC + ESP32 (disk/HDD/OSD) + shadow, but not Mockingboard and Uthernet2 on top of that. Uthernet2 is BSRAM-bound (10 blocks), Mockingboard logic-bound (~2k). Candidates to win them back later: DOC register banks into BSRAM (`DOC_BANKS_IN_BSRAM=1`, −2k logic / +6 BSRAM — only if BSRAM is freed elsewhere), the ESP32 disk lanes 4→2 (−4 BSRAM, firmware-visible), the DebugOverlay (bring-up aid).

## 4. Build/bench sequence

1. **New top, video only** — the B4 bench with the real renderers: `apple_bus`
   (GS autodetect, `sw_gs_i=0`), `apple_memory` with shadow on ports 0/1,
   `apple_video_gen`/`vgc_gen` → `framebuffer_writer` → FB ports 4/5,
   `dp_transmitter` 1080p×4, `esp32_ospi_connector` (so the ESP32 can hold
   reset and serve the OSD). Bench: Apple IIgs on the bench → picture.
   Keep the `PC`-style status on the OSD/telemetry path the ESP32 already
   has (`dbg_fb_flags_i`, `dbg_shadow_drop_i`, ...).
2. **+ DOC/GLU on ports 2/3** (B3) + `apple_speaker` → DP audio.
   Bench: IIgs system beep, then a DOC title (POLY.SYNTH per the a2mega
   listening notes).
3. **+ peripherals**: DiskII/HDD/Uthernet2 (ESP32-served), Mockingboard, SSC.
   Bench: ProDOS boot from an image served by the ESP32.
4. **Fit check** after each step (CLS %, BSRAM); timing bar: 0 violations,
   and ≥ +0.5 ns on clk_mem/clk_hr before calling a build durable (the
   HyperRAM half-cycle `ck_en` stage sits at +0.03…+0.3 across rolls today;
   a structural fix in `hyperram_phy_gw5a` is on the list if it keeps
   flapping).

## 5. Files to create / change

- `boards/a2p25/hdl/top.sv` — new (derived from a2mega); the old one moves to
  `boards/a2p25/hdl/legacy/top_rev0.sv` with its gprj, for reference.
- `boards/a2p25/hdl/a2p25.cst` — new pin list per PINOUT_DISPLAYPORT1 §2
  (Apple bus with D7 on B3, HyperRAM, DP AUX/HPD, OPI, LED, clocks); process
  config keeps READY/SSPI/I2C as regular IO, JTAG/RECONFIG/DONE dedicated.
- `boards/a2p25/hdl/a2p25.sdc` — union of `a2p25_dp_test_fb.sdc` and the Apple
  clock constraints from the old sdc.
- `boards/a2p25/a2p25.gprj` — a2mega file list minus DDR3/SERDES/board_plls,
  plus `hdl/dp/*`, `hdl/gowin/*` (PLLs, D-PHY), `hdl/hyperram/*`,
  `hdl/psram_test/hr_clk_pll.v`, `../a2mega/hdl/dp_aux_pad.sv`,
  `../a2mega/hdl/esp32/*`, `../a2mega/hdl/bus/apple_bus.sv`,
  `../a2mega/hdl/memory/apple_memory.sv`.
- ESP32 firmware: `BOARD_REV=225` without `DP_TEST` (the production mode of the
  a2mega firmware: OSPI link, HPD on IO10, I2C mux/PD/DP130 unchanged).

## 6. Decisions (ed, 2026-10-11)

**Scope confirmed:** first core as recommended — SuperSprite/F18A deferred; Uthernet2 decided after the first fit check. Scanlines/speaker/slot-7 as build parameters for now. Double-buffering off.

Original questions:

1. **Feature scope for the first full core** (§3): drop SuperSprite/F18A
   first, or trim ESP32-served peripherals instead? (Recommendation: start
   without SuperSprite; it is the one block that moves the budget by itself.)
2. **Scanlines / Apple speaker / slot-7** were DIP switches on the a2mega.
   On this board: build parameters for now, OSD settings later?
3. Double-buffering stays off (a2mega behaviour, see MODERNIZATION_PLAN §6e).
