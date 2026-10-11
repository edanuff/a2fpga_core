# a2p25 modernization — displayport1 board rev

Gateware/firmware plan for the a2-p25 board revision on branch `displayport1`
of the board repo (`the a2-p25 board repo`): Tang Primer 25K
(GW5A-LV25MG121), Infineon S27KL0642 HyperRAM, ESP32-S3 on an 8-bit link,
USB-C DisplayPort through the GW5A's hardened MIPI D-PHY.

Started 2026-10-01 on branch `claude/a2p25-modernization-8a97cc`.

Companion docs:

- [PINOUT_DISPLAYPORT1.md](PINOUT_DISPLAYPORT1.md) — netlist-derived ball map
- [DP_ALTMODE_DPHY_DESIGN.md](DP_ALTMODE_DPHY_DESIGN.md) — why DP over D-PHY, the analog chain
- `docs/memory_bandwidth_analysis.md` §3 on branch `claude/a2p25-memory-bandwidth-a4dd1f`
  — the HyperRAM budget, which §4 below revises

## 1. Priorities

Set by ed on 2026-10-01, in order:

1. **Colorbars over the D-PHY → DP chain.** No one has driven DP from a MIPI
   D-PHY used as a raw serializer, in any setting. Prove it first.
2. **The AUX path.** The a2mega's AUX receive path was broken and was worked
   around. The a2p25 uses the corrected circuit: the a2mega network with the
   AC caps moved in front of the SN75DP130. It has to be validated here before
   the a2mega board turn, which carries the same fix.
3. **A standalone PSRAM test with error rates.** The HyperRAM CK# ball floats
   (§5). We expect the part to work, but possibly only with some error rate or
   at a slower clock. The test has to measure that, independently of
   everything else.
4. Then:
   - the Ensoniq DOC5503 in fabric with its wave RAM in PSRAM, replacing the
     ESP32-generated audio of the previous rev;
   - the existing framebuffer mechanism (render pass into a framebuffer, then a
     scanout pass, as on a2mega and a2n20) retargeted to PSRAM;
   - shadow memory in PSRAM;
   - the ESP32 link and the full core.

A board turn takes about 30 days, so the plan is split. Everything that does
not need the PSRAM can be brought up even if the PSRAM turns out to be
unusable.

## 2. Two tracks

**Track A — no PSRAM needed.** Each item stands alone.

| Step | What | Status |
|---|---|---|
| A1 | `a2p25_dp_test`: 720p60 RBR×2 colorbars + tone, AUX telemetry | **🏆 COLORBARS + DP AUDIO ON HARDWARE 2026-10-10/11** on all three monitors (Sceptre via USB-C→DP cable; FANGOR and UFYQL native USB-C); replugs + both orientations OK; established-state AUX soak clean (§6b) |
| A2 | ESP32 firmware a2p25 variant (`make BOARD_REV=225`, see [ESP32_FW_A2P25.md](ESP32_FW_A2P25.md)): PD (FUSB302B) → TUSB546A → SN75DP130 probe, HPD to FPGA, telemetry tee | **builds**; not yet run on hardware |
| A3 | AUX validation matrix across our sink census (§6) | bench |
| A4 | 4-lane RBR 1080p60 variant of A1 (`a2p25_dp_test_4l.gprj`) | **built, 0 violations (thin: clk100 +0.02 ns, pix +0.17 ns)**; bench after A3 |
| A5 | ESP32 OPI link + OSD (reuse a2mega `esp32_ospi_connector`; the pinout is the same family) | todo |
| A6 | Apple bus core + real-time `apple_video` at 480p over DP, shadow memory in BSRAM as today | todo |

A6 is the fallback product if the PSRAM is unusable. 480p fits because 27 MHz
is exactly the 81 MHz RBR symbol clock ÷ 3. Speaker and Mockingboard audio go
over DP. The Ensoniq does not fit without PSRAM: 64 KB of wave RAM is too much
BSRAM, and this rev has no I2S path back from the ESP32.

**Track B — PSRAM.**

| Step | What | Status |
|---|---|---|
| B1 | `hyperram_ctrl` (custom, see §4) + Infineon-model TB (`make hyperram`) | **sim PASS at 0–4 ns board delay**; standalone PnR clk_mem 116.8 MHz |
| B2 | `a2p25_psram_test_ck{108,81,54}`: BIST + UART console (C2/F2 to ESP32 IO11/IO12, firmware passthrough); procedure in [PSRAM_TEST.md](PSRAM_TEST.md) | **all three built, 0/0; system sim PASS** (`make psram_test_top`); bench next |
| B3 | DOC5503: `sound_glu` + `doc5503_pipelined` on a HyperRAM port | todo |
| B4 | Framebuffer on PSRAM (the existing render/scanout mechanism) feeding DP | todo |
| B5 | Shadow memory in PSRAM (`apple_memory_sdram`, `VGC_IN_SDRAM`) | todo |

## 3. Clocking

All clocks come from the 50 MHz SOM oscillator on E2.

| Clock | Source | Use |
|---|---|---|
| 100 MHz | `gowin_mgmt_pll` (VCO 1200) | AUX bit timing, link policy |
| 54 MHz | system PLL | logic clock; D-PHY PLL reference |
| 108 MHz + 90° copy, and the logic 54 MHz | HyperRAM PLL fed by the board PLL's 54 MHz: VCO 1080 (×20), ÷10 / ÷10 +90° / ÷20 | HyperBus CK; `hyperram_ctrl` core; all logic in the full core |
| 810 MHz ×4 phases | D-PHY PLL (CLKIN 54 MHz, ×15) | serializer |
| 81 MHz | D-PHY PLL `clkout4` = VCO ÷ 10 | DP link-symbol clock |
| 101.25 MHz | D-PHY `TX_CLK_O` = VCO ÷ 8 | gearbox |
| 74.25 / 148.5 MHz | pixel PLL fed by 81 MHz (VCO 891, ÷12 or ÷6) | DP pixel clock, synchronous clocking |

The board PLL (VCO 1350, 50 MHz in) makes 54 MHz. Everything that needs 108
MHz or the HyperRAM phase relationships cascades from that 54 MHz into a
second PLLA.

**Why the cascade.** GW5A PLLA requires a PFD (FCLKIN / IDIV) of
19–87.5 MHz; Gowin reports PA2078 when it is out of range. From 50 MHz that
allows only IDIV 1 or 2, so the plan's "VCO 1080 = 50 ÷ 5 × 108" was illegal.

A single PLL *can* make 108 MHz: the board PLL's VCO 1350 with CLKOUT0's
**fractional** divider ÷12.5 (12 + 4/8), confirmed by ed in the IDE. That
is not suitable for HyperRAM:

- **No 90° copy.** Only CLKOUT0 has a fractional divider, and 1350 / 108 is
  not an integer, so no other output can produce the +90° CK clock.
- **Jitter.** ÷12.5 alternates ÷12 / ÷13, so successive periods are 8.9 and
  9.6 ns (about ±0.37 ns of deterministic jitter). That eats directly into the
  ~4.6 ns per-byte capture window.

`hr_clk_pll` therefore takes 54 MHz: PFD 54, VCO 1080 (108 MHz) or 810
(81 MHz), with integer dividers and a +90° output. In the full core, all
logic clocks from this PLL's 54 MHz so it stays phase-related to `clk_mem`.
PLL budget: board, HyperRAM, AUX 100 MHz, D-PHY and pixel = 5 of 6 PLLA.
A plain, jitter-tolerant 108 MHz elsewhere can still use the single-PLL
fractional CLKOUT0.


## 4. PSRAM

### 4.1 Why a custom controller, not the Gowin IP

IPUG944 (Table 4-2) requires a minimum command interval of 15 user clocks even
for a 16-byte burst. At a 54 MHz user clock, every 4-byte Apple shadow access
or CPU write would cost 15 cycles. That is about 3× what the original budget
assumed (~5.5 cycles), and it pushes worst-case bus load to about 90%.

`hdl/hyperram/hyperram_ctrl.sv` starts from nand2mario's psram-tang-nano-9k
controller (`ed's psram-tang-nano-9k fork`: ODDR/IDDR and a
phase-shifted CK) and changes it:

- **Clocking:** 2:1 synchronous. CK runs on `clk_mem` = 2× `clk`, the user
  side runs on `clk`, and there are no async FIFOs.
- **Latency:** variable latency, decided by sampling RWDS during CA.
- **Bursts:** linear bursts of 1–16 words.
- **Read capture:** framed by RWDS, through an IODELAY that is calibrated at
  boot.
- **Runtime knobs:** CR0 can be changed at runtime (latency, fixed/variable,
  drive strength).

At CK = 108 MHz the projected worst case is about 45% of the bus, against about
90% with the IP.

Measured in simulation, at L=6, in clk cycles per transaction including the
user handshake:

| Transaction | 1 word | 4 words | 16 words |
|---|---|---|---|
| Read | 14 | 17 | 29 |
| Write | 10 | 13 | 25 |

Writes and bursts beat the IP. Single-word random reads only roughly match the
IP's 15-clock floor, because the next request waits for read data to drain.
If the shadow-read budget needs it, the next step is a pipelined user port.

Timing needs `set_false_path -from [get_regs {<inst>/dly_code*}]`, since the
IODELAY code is quasi-static.

### 4.2 Clock rate is an open question

108 MHz is the goal. The S27KL0642 is rated for 166/200 MHz at 3.0 V, and the
grade on the BOM is DPBHI020, the 166 MHz part. The psram-tang-nano-9k numbers
(81–96 MHz) came from a much slower GW1N and say nothing about this part on
this board.

The standalone test is built at several CK rates (108, 81 and 54 MHz) and
reports error rates. It also does an IODELAY eye scan and a drive-strength
sweep. The design is meant to be vendor-generic HyperBus, because parts will
be multi-sourced: Infineon, ISSI and Winbond use the same CR0 encoding.

### 4.3 Memory map

8 MB. Revisit when B3–B5 land.

| Region | Byte address |
|---|---|
| Framebuffer | 0x000000 |
| Shadow + `VGC_IN_SDRAM` pre-interleaved | 0x100000 |
| Ensoniq wave RAM (64 KB) | 0x200000 |
| BIST / calibration scratch (the controller's `CAL_ADDR` is the last 64 B) | top of memory |

## 5. HyperRAM CK# floats — OK on Infineon, check per vendor

Both boards leave CK# unconnected:

- **a2-p25 displayport1:** U6 ball B1 is no-connect.
- **a2-mega 1.0a5:** U17 ball B1 is `unconnected-(U17-~{CK}-PadB1)`.

The pin descriptions in the Infineon, ISSI and Winbond datasheets all say to tie
an unused CK# to VSS or VCC. However, the Infineon S27KL0642 datasheet
(002-24692 Rev *I) has a more specific statement, found by ed on 2026-10-01:

- **§6.3.3.1:** "In the single ended clock mode (by default), CK# input is not
  enabled; hence it may be left either floating or biased to HIGH or LOW."
- **Table 12:** CR1[6] master clock type resets to 1, which is single-ended.

So on the fitted Infineon part a floating CK# is within spec, as long as
nothing ever writes CR1[6] = 0. `hyperram_ctrl` only ever writes CR0, and its
header records this as a hard rule.

Second sources, checked on 2026-10-01. All three parts share the same CR1[6]
"Master Clock Type" bit, which resets to 1 (single-ended):

| Vendor | Pin-table note on CK# | CR1 section | Fit on boards with a floating CK#? |
|---|---|---|---|
| Infineon S27KL0642 (002-24692 Rev *I) | tie to VCCQ/VSSQ, do not float | §6.3.3.1: single-ended default, CK# input **not enabled, may float** | **yes** (explicitly allowed) |
| Winbond W956x8MBYA (A01-003) | no tie-off instruction; "CK# is not used" in single-ended mode | Table 13 / §9.5.1: same bit and default | **yes** (not forbidden) |
| ISSI IS66/67WVH8M8FALL/BLL | Table note 1: tie to VccQ/VssQ, "do not leave it floating" | Table 5.7: same bit and default, but no "may float" statement | **avoid** until a respin ties B1 to C1 |

Open items:

- **Next respins:** tying B1 to the adjacent C1 (VSSQ) is still the cleanest
  fix and makes the board vendor-agnostic. It is a 1 mm top-layer trace on
  both boards.
- **Assembly:** no rework instruction is needed for the a2p25 run with
  Infineon parts. A paste bridge would not work anyway: there is no copper
  between B1 and C1, and B1 sits next to the CK pad.

RESET# (A4) is unconnected on both boards, which is fine: it has an internal
pull-up.

## 6. Day-one bench ladder (DP)

1. **ESP32 firmware first.** Flash the a2p25 variant (see `ESP32_FW_A2P25.md`
   once it lands). It does the PD contract, the alt-mode entry, the TUSB546A
   lanes, the DP130 probe, HPD on IO10, and the FPGA telemetry on IO11.
2. **Lane-probe bitstream.** Flash `a2p25_dp_test_probe` (`PROBE=1`) and scope
   each lane at the DP130 inputs. Expect a 2.53 MHz square wave on all four.
   This proves the D-PHY HS driver runs continuously with no LP excursions,
   and shows the lane order and polarity. Telemetry `S:` bit 1 = `TX_CLK_O`
   alive, bit 7 = D-PHY reset released.
3. **Colorbars.** Build `a2p25_dp_test` with `PROBE=0` and use the best sink
   class first: a direct USB-C→DP cable into a DP monitor (HBR2 ×4 in the
   census, both gates passing).

   LEDs:
   - **FPGA LED (B10):** a ~1.5 Hz heartbeat in every state. Dark or solid
     means the bitstream is not running.
   - **ESP32 LED0:** ESP32 heartbeat.
   - **ESP32 LED1:** the HPD level being sent to the FPGA.

   The link ladder is in telemetry `P1 HLV:` (HPD / link / video).

   In telemetry, `Q:` must read `04D3F640` (81,000,000 symbol clocks per
   second). `P2 C:` shows lane lock state, and `P6` shows the symbol error
   counters.
4. **AUX matrix.** Run the census devices: the Anker / Mindorlan / Ugreen hubs
   and dongles, the direct cable, and the DP monitor. Record AUX RX health:
   - `E:` / `R:` reply counters
   - `J:` error classes
   - `U:` defers
   - `T:` teardowns

   Compare with the a2mega closed-loop results. The `dp_aux_pad` counter-park
   (2.5 µs) was tuned for the a2mega's cap placement and may need retuning
   now that the caps sit in front of the DP130.

## 6b. Bench results — DP first light (2026-10-10)

Setup: Sceptre DP monitor via USB-C→DP cable. FANGOR USB-C portable: see the 2026-10-11 row.

| Result | Detail |
|---|---|
| Video | 1280×720@60 (monitor OSD), colorbars |
| Audio | 1 kHz SDP test tone heard |
| Clocks | symbol 81,000,000 / pixel 74,250,000 exact, 0 packer underruns |
| Replug | recovers every time (`HPD_DISCONNECT_RESETS=1`) |
| Orientation | both CC1 and CC2 work with the default FLIPSEL convention (7/7 tagged attaches); `muxdump`/`muxflip` serial commands for A/B |
| Time to picture | ~0.5 s to trained link; Sceptre displays 5–15 s after attach (normal input detect) |
| FANGOR (USB-C native) | **Colorbars 2026-10-11** with `bc08d218`, direct USB-C. Earlier "never wakes" was a **bad USB-C cable**: a MacBook Air gave no video through it either. Good-cable signature: `Y:11` (first try), `C:8177`, `A:0022`, symbol errors 0, PD `dp-active`, FANGOR is the power source. Note: the bad-cable sessions also showed `C:8177` with zero errors, so a trained/aligned status does not prove the main link reaches the panel; cross-check suspect cables on a known-good source. Replug run 10-11: 6 flips (3 per orientation), **6/6 pictures ~2 s**, every attach trained first try (`Y` 22→77), `A:0022`/`C:8177`, 0 symbol errors; 0/7 bad trainings vs ~1/6 on the Sceptre+cable path. Firmware missed 3/6 detaches (stale-session guard recovered each) |
| UFYQL (USB-C native) | **Colorbars 2026-10-11**, `bc08d218`: 6 flips → 6 pictures once its own power supply was connected correctly. With its power mis-plugged it tried to draw from our port: PD churn (`no PD Request after 12 Source_Caps`, source/sink role flapping), no stable DP. Final link `A:0022`/`C:8177`, 0 symbol errors (this run was not logged per-attach) |
| Occasional bad training | ~1 in 6 attaches: sink asks swing 2 (+PE 1), lanes lock but no interlane alignment → dark. Training-time only: once a picture appears it is perfect and stable (no sparkle, zero symbol errors). Treat as a handshake/ladder problem first (cf. a2mega, where similar symptoms were protocol, not swing); amplitude margin only if new evidence points there |

**Established-state AUX soak 2026-10-11** (`a2p25_dp_test_auxobs`, `LINK_CHECK=2`: periodic status reads run, failures only counted, link never torn down; P9 counters, PA = capture of the failing reply):

| Monitor | Path | Status reads | short | NACK | DEFER | timeout | lock-gate fails |
|---|---|---|---|---|---|---|---|
| FANGOR | direct USB-C | ~1,000 (17 min) | 0 | 0 | 0 | 0 | 0 |
| UFYQL | direct USB-C | ~300 (5 min) | 0 | 0 | 0 | 0 | 0 |
| Sceptre | USB-C→DP cable | ~300 (5 min) | 0 | 0 | 0 | 0 | **10** (first 0.3 s after established: 0x202=0x66 both lanes = CR lost + align lost; sink re-locked by itself ~10 s later) |

Verdict: **the a2p25 AUX circuit + .cst (LVDS25, BANK_VCCIO 3.3, pulldowns) and the FPGA turnaround logic are clean** — ~1,800 established-state transactions, zero AUX-level failures, across three sinks. Yesterday's "intermittent NACK/timeout" that forced `LINK_CHECK=0` was the bad USB-C cable. This is the AUX validation the a2mega board turn was waiting on.

The Sceptre's lock-gate failures are the "bad training" from 10-10: the sink drops CR shortly after training and waits for the source to retrain (spec flow). `LINK_CHECK=0` never looks, so a sink that does not recover on its own stays dark. `a2p25_dp_test_lc1` (`LINK_CHECK=1`, spec flow: periodic checks + IRQ_HPD service + teardown/retrain on failure) on the Sceptre: gate failed at +1.2 s after load, torn down and retrained once (sink asked VS2/PE1 on the retrain), aligned, then held 4 min with 0 AUX failures and 2 IRQ services. Replug tally pending.

Gateware policy at first light (10-10): `LINK_CHECK=0` (established-state AUX reads intermittently fail and used to tear the link down), watchdog exemption, `HPD_DISCONNECT_RESETS=1`.

DP130 note: in auto mode it sets its *input* EQ from the pre-emphasis we declare (8 dB at PE0). `05h ← 0x80` (I2C EQ, 0 dB) was tried live; it did not change the bad-training rate. The CLI parses decimal unless `0x` is given.

Next steps (DP):
1. Make a bad attach self-heal: after "established", do one evidence-only status read. Retrain if lanes or alignment are lost; ignore AUX errors.
2. Debug the bad-training ladder path (training sequence/timing, the retrain that declared established without alignment).
3. Established-state AUX RX reliability (P7 capture) — this is the a2mega-turn gate. The AUX board circuit is simple; suspect .cst (IO standard, pulls, VCCIO) and FPGA-side direction-turnaround logic first.
4. UFYQL USB-C monitor (untried).
5. 4-lane 1080p (saturated symbol errors).

## 7. Known risks

- **D-PHY amplitude vs the DP130 input.** The DP130's minimum VID is
  0.3 Vpp; the worst-case D-PHY swing is about 0.28 Vpp. The DP130 EQ
  (I2C register 05.7 override) is the recovery knob.
- **`TX_CLK_O` gating.** The D-PHY reset is now released from the symbol-clock
  domain, so the start-up has no dependency on `TX_CLK_O`. Bench-verify via
  telemetry `S:` bit 1.
- **PLL cascade.** The D-PHY PLL reference is a fabric PLL output. This was
  accepted at design time pending a soak; a crystal-direct break-glass option
  is in the design doc.
- **clk100 slack.** It is +0.1 ns in the dp_test (shared AUX/link-policy
  logic). That passes but is thin; watch it as the core grows.
- **CK# floating** (§5): within spec on Infineon parts. Never write CR1.
  Re-check before using ISSI or Winbond.
- **IODELAY dynamic-load semantics.** These are taken from the GW5A sim model
  and still need confirming against UG289. The PSRAM eye scan exercises them.
- **OPI_CLK lands on a non-GCLK ball.** The link oversamples it on the 54 MHz
  clock anyway, so it is fine at ≤10 MHz.

## 8. Build provenance

| Date | Project | Base | Toolchain | Result | `.fs` sha256 |
|---|---|---|---|---|---|
| 2026-10-01 | `a2p25_dp_test` (superseded) | c56c691a | Gowin V1.9.12.01 | 0/0 violations | `9af2ff657ca33c833e2dca44f34547ab9287a67af189b711fc2f458c4ab81bb6` |
| 2026-10-01 | `a2p25_dp_test` 720p ×2 | this commit | Gowin V1.9.12.01 | 0/0; clk100 101.1, pix 130.5 MHz | `2fb2d479954d09aa5c29371279e9e2145b1fed5c1555b28605c97e967b1bbc6f` |
| 2026-10-01 | `a2p25_dp_test_4l` 1080p ×4 | this commit | Gowin V1.9.12.01 | 0/0; clk100 100.24 (+0.02 ns), pix 152.25 vs 148.5 (+0.17 ns) — thin | `4b1d2e74bd52abc95118dd487e0c9776c902b040c51986beefd20e45bcc2d798` |
| 2026-10-01 | `a2p25_dp_test_probe` (PROBE=1, 720p) | this commit | Gowin V1.9.12.01 | 0/0 | `bbe086b98ece01a3f273a9ae04bd1d52e5e7bb8e712ca4fa14401a0c0118f429` |
| 2026-10-01 | `a2p25_psram_test_ck108` | this commit | Gowin V1.9.12.01 | 0/0; clk_mem 112.0 vs 108 (+0.31 ns — below the +0.5 durability bar) | `c78ffe7423efa974d0ef524d44ae2d0cf0e32f58d9fedcbedd6d5841999b9a4b` |
| 2026-10-01 | `a2p25_psram_test_ck81` | this commit | Gowin V1.9.12.01 | 0/0; clk_mem 118.2 vs 81 | `16a5c29f7fbe63d67f07b224a785a33ce03eedbce2c7278b5c3f1ead55966c71` |
| 2026-10-01 | `a2p25_psram_test_ck54` | this commit | Gowin V1.9.12.01 | 0/0; clk_mem 110.9 vs 54 | `4cb6a3908ea4e18e5f6801f955b13b5dc0d661a51f851a6470216e3d37478fb2` |
| 2026-10-10 | **heartbeat LED rebuild (current bench set)** — `a2p25_dp_test` 720p ×2 | this commit | Gowin V1.9.12.01 | 0/0; clk100 102.8, pix 104.7 | `224aa743e16d21a2b27a0438b109ce4381aa7e73ac8c8af833d1caddf4f135a4` |
| 2026-10-10 | `a2p25_dp_test_probe` | this commit | Gowin V1.9.12.01 | 0/0 | `dc172e27fe2f0e1bf3116d4e92b071c72fe1459a313c1e56a050a9111cc06f69` |
| 2026-10-10 | `a2p25_dp_test_4l` 1080p ×4 | this commit | Gowin V1.9.12.01 | 0/0; clk100 100.43 (+0.04 ns), pix 156.9 vs 148.5 | `b6a56dbd484e6695cf9772f68f834c7b876e2c0ed01ee3a9bec2f8552d399af4` |
| 2026-10-10 | `a2p25_psram_test_ck108` | this commit | Gowin V1.9.12.01 | 0/0; clk_mem 109.7 vs 108 (+0.15 ns, thin) | `ee1dfc317c94b35f8fcda796fed9a0415bbbeaf33ce4e9a18c12e2c709fecf0f` |
| 2026-10-10 | `a2p25_psram_test_ck81` | this commit | Gowin V1.9.12.01 | 0/0; clk_mem 107.6 vs 81 | `386e1a36924867884278341b70459381f03216e377b205644c500647bc673851` |
| 2026-10-10 | `a2p25_psram_test_ck54` | this commit | Gowin V1.9.12.01 | 0/0; clk_mem 85.6 vs 54 | `1ba81136659674a716ca84982b8feac7a21754972bcfddbdf0d5b5c2f0ac272e` |
| 2026-10-10 | ESP32 fw `make BOARD_REV=225` (DP_TEST=1) | this commit | arduino-cli 1.2.2 / esp32 3.3.5 | builds | `1ea60d8aa9ba5ef93ae1f339f1db6ad32acfb9ea8075a6520f9612ce4e1e5816` |
| 2026-10-10 | **`a2p25_dp_test` — FIRST COLORBARS** (hold-link, P7/P8 telemetry) | 6d0b148d | Gowin V1.9.12.01 | 0/0 | `4257f701389f46809ab3f27eab356b95223a39344fed51d0ef4df28b0e09a373` |
| 2026-10-10 | `a2p25_dp_test` + HPD_DISCONNECT_RESETS=1 (**replug recovery verified**: Y 11→22→33, bars return) | this commit | Gowin V1.9.12.01 | 0/0 | `bc08d218a69cf396d0b618d27176b416c6d7a92a0cbf4be69d36ca5dd076a9c2` |
