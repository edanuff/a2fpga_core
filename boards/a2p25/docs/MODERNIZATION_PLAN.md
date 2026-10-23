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
| A1b | `a2p25_dp_test_4l`: **1080p60 RBR×4** colorbars | **🏆 COLORBARS 2026-10-11, Sceptre OSD 1920×1080@60** (§6d) — the a2mega-unifying mode |
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
| B3 | DOC5503: `sound_glu` + `doc5503_pipelined` on a HyperRAM port | needs the Apple bus → part of the full-core integration, see [TOP_INTEGRATION_PLAN.md](TOP_INTEGRATION_PLAN.md) |
| B4 | Framebuffer on PSRAM (the existing render/scanout mechanism) feeding DP | **🏆 FIRST LIGHT 2026-10-11** — `a2p25_dp_test_fb`: `hyperram_ports` (new mem_port_if arbiter over `hyperram_ctrl`, same client contract as `ddr3_ports`) + unchanged `framebuffer_1080p` + synthetic 560×192 renderer → colorbars + sweeping stripe on the monitor at 1080p×4; HyperRAM 0 read errors under the real load. Fixed on the way: framebuffer vertical-tracker timing (−0.75 ns on the 25K) ; the visible 1-px step is the shipped single-buffer tear made visible by a 1 px/frame stripe (§6e) |
| B5 | Shadow memory in PSRAM (`apple_memory_sdram`, `VGC_IN_SDRAM`) | same: [TOP_INTEGRATION_PLAN.md](TOP_INTEGRATION_PLAN.md) — **resource scoping decision pending** (a2mega feature set does not fit the 25K; §3 there) |

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

The Sceptre's lock-gate failures are the "bad training" from 10-10: the sink drops CR shortly after training and waits for the source to retrain (spec flow). `LINK_CHECK=0` never looks, so a sink that does not recover on its own stays dark. `LINK_CHECK=1` (spec flow, built first as `_lc1`, now the baseline: periodic checks + IRQ_HPD service + teardown/retrain on failure) on the Sceptre: gate failed at +1.2 s after load, torn down and retrained once (sink asked VS2/PE1 on the retrain), aligned, then held 15 min with 0 AUX failures. **Sceptre replug run under LINK_CHECK=1: 20 attaches (10 flips, CC1/CC2 alternating), 20/20 ended with an aligned, streaming link (`C:0177`, `K:03`)**, 8–10 s from alt-mode entry to aligned (one at 20 s). Every attach follows the same script: train (sink asks VS2/PE0 `A:0022`, established within 0.4 s) → ~2 s later the sink drops CR on both lanes → teardown/retrain (same request) → ~5 s later drops again → third retrain, sink now asks **VS2/PE1 (`A:6666`)** → aligned and stable. Correction (same day): with the AFE sequencer off, a2p25 writes a constant TRAINING_LANEx_SET of 0x06 (VS2 + MAX_SWING, PE0) on every pass — the sink's requests are read but never echoed, so the DP130 (auto mode, snooping those writes) drives the same VOD/PE on all three passes. The third pass holding is therefore the Sceptre's own re-lock schedule (two CR drops ~2 s and ~5 s after each establish, consistent with its 5–15 s input-detect seen on 10-10), not anything we changed. What `LINK_CHECK=1` fixes is that we now notice each drop and retrain; the old hold stopped after pass 1 and ~1/6 attaches stayed dark. Not an amplitude-margin problem. Tried (bdca565f…, now baseline): `ECHO_SINK_ADJUST=1` (declare the sink's request in the lane-set writes) + `LINK_CHECK_CLKS` 200 ms. Sceptre 8/8 attaches aligned; each retrain now follows the drop within 0.2–0.4 s, but the drops themselves still come at ~2 s and ~6–8 s after attach, so time-to-picture stays 8–10 s: it is the monitor's re-lock schedule. Side effect: with the request echoed the Sceptre settles on VS2/PE0 (`A:2222`) instead of asking VS2/PE1. Kept both changes (spec-correct, no regression); FANGOR/UFYQL unchanged at ~2 s.

**Baseline policy since 10-11: `LINK_CHECK=1`** (spec flow). `a2p25_dp_test_hold.gprj` keeps the first-light hold-link policy for the bench; `_auxobs` is the observe/count build.

Gateware policy at first light (10-10): `LINK_CHECK=0` (established-state AUX reads intermittently fail and used to tear the link down), watchdog exemption, `HPD_DISCONNECT_RESETS=1`.

DP130 note: in auto mode it sets its *input* EQ from the pre-emphasis we declare (8 dB at PE0). `05h ← 0x80` (I2C EQ, 0 dB) was tried live; it did not change the bad-training rate. The CLI parses decimal unless `0x` is given.

Next steps (DP):
1. ~~Bad attach self-heal~~ — done: LINK_CHECK=1 baseline (spec retrain).
2. ~~Sceptre time-to-picture~~ — it is the monitor's own re-lock schedule (see above); nothing more to gain source-side.
2b. ~~4-lane 1080p RBR~~ — working (§6d).
3. ~~Established-state AUX RX reliability~~ — done, clean on three sinks (§6b soak).
4. ~~UFYQL~~ — colorbars.
5. 4-lane 1080p (saturated symbol errors).

## 6c. Bench results — PSRAM first run (2026-10-11)

`a2p25_psram_test_ck108` (sha `ee1dfc31…`), console through the ESP32 forwarding mode, no monitor needed.

| Item | Result |
|---|---|
| Init / calibration | CR0 0x8F17 accepted, cal ok, calibrated tap 0xB0, arrival 17, cal map `FFFFE07F` |
| Continuous test, CK 108 MHz | 102 full-device passes in 36 s, 207 M words compared, **0 error words, 0 error bits**, 0 read timeouts; 0x9522 refresh-collision (2x latency) transactions |
| Eye scan (4096-word range, 64 reads/tap) | errors only at taps 0x40–0x68 (~0.5 ns band); clean from 0x70 up through 0xFF and from 0x00 to 0x38, i.e. the eye runs off both ends of the 3.2 ns delay range; the calibrated tap sits ≥0.9 ns from the nearest edge |

So the floating CK# is a non-issue on the Infineon part (CR1 default single-ended, as §5 predicted), and 108 MHz works with margin on the first try. The 81/54 MHz variants are not needed for bring-up. Remaining PSRAM items: longer soak, CR0 drive-strength sweep (34 Ω default is fine), and the scope checks in PSRAM_TEST.md if anything ever looks marginal.

## 6d. Bench results — 4-lane 1080p (2026-10-11)

Symptom: `_4l` trained and aligned on all four lanes (new P6 `3:` field = DPCD 0x203 = 0x77), sink reported receiving, then dropped every few seconds with saturated symbol-error counters; the Sceptre stayed at "no signal". Same at 4 lanes/720p (`_4l720`), so the 4-lane path, not the pixel rate. Two bugs, both in shared `hdl/displayport`:

1. **AUX dropped-request hang** (commit 84b65c72): a status read issued within 15 clocks of entering `link_established` (pending IRQ_HPD after a retrain) was dropped by the message engine, which was still "busy" with the empty link_established message; the ladder then waited forever (`PB` line: tx idle, 0 bytes, 9 expected). Fix: msg 0x00 no longer occupies the engine; check_link entry waits for it. Affects a2mega too (candidate explanation for its old "checks not running for minutes" anomaly).
2. **`msa_inserter_4ch` width bugs** (same commit): lane-2 Vheight rows were 22-bit concatenations into 18 bits (720p announced as 8400 lines, 1080p as 16440), and the lane-2/lane-3 pass-through rows were 19/16 bits wide (symbols shifted, lane 3 lost its K bits). Synthesis had warned (EX3791) all along. 2-lane inserter unaffected, so a2mega never saw it.

After the fixes (Sceptre via USB-C→DP cable): `_4l720` holds, aligned ×4, 0 symbol errors, colorbars, OSD 1280×720. `_4l` (1080p60): link up in 0.8 s, aligned ×4, 0 symbol errors, pixel clock 148,500,000 exact, 0 underruns, **colorbars, OSD 1920×1080@60**. a2p25 and a2mega now share the same 1080p video path. Also colorbars at 1080p on the FANGOR (native USB-C, 4 lanes).

## 6e. Bench results — framebuffer on PSRAM (2026-10-11)

`a2p25_dp_test_fb` = the DP test with `DP_TEST_FB`: `clk54 → hr_clk_pll` cascade, `hyperram_ctrl` (108 MHz CK), `hyperram_ports_if` (port 0 = FB write, 4-word wide; port 1 = FB read, 8-word bursts), `framebuffer_1080p` (560×192 RGB666 source, ×3/×5 into 1920×1080) and a renderer that draws eight colour bars, a white frame and an 8-px stripe sweeping 1 px/frame. Telemetry line `PC`.

| Item | Result |
|---|---|
| HyperRAM under load | init + cal OK, 0 read timeouts, write FIFO never overflowed |
| Scanout | ~192 line fetches/frame, pixel clock 148.5 MHz exact, 0 underruns |
| Picture | dark-blue field, colour bars, moving stripe — on the first load that reached a monitor |
| Artifact | stripe stepped 1 px at 1/8 down, every frame, fixed position. Not a data-path fault (beat/FIFO counters all zero): `framebuffer_1080p` is deliberately **single-buffered** (`BUFFER_OFFSET = 0`, comment: double-buffering made Apple II updates feel slow), so the renderer overtakes the scanout at a fixed line — both run at exactly 60.000 Hz. Invisible with static content, same on the a2mega. The read side now latches the writer's last *completed* buffer (a no-op at offset 0, correct if double-buffering is ever re-enabled). The vertical-tracker pipeline (−0.75 ns on the 25K) is the one functional change the a2mega inherits — regression build pending |

`hyperram_ports` simulation: `make -C boards/a2p25/tests hyperram_ports` — random traffic on three ports (single r/w with byte enables, wide writes, burst-8 reads, priority contention) against the Infineon model, PASS. The arbiter bug it caught (re-picking a port in the cycle its pop landed) would have been a hardware mystery.

## 6f. 🏆 Apple IIgs boots on the new core (2026-10-11)

`a2p25.fs` `0c651f95…` (commit 391abc13) in the FPGA flash, ESP32 fw rev 225 production variant (OSPI link, HPD on IO10): card in the IIgs, monitor on the card's USB-C → **Apple II boot screen** over DP 1080p×4. Path: Apple bus → `apple_bus` (IIgs autodetect, no DIP) → shadow memory in HyperRAM (`apple_memory` on `hyperram_ports` 0/1) → `apple_video_gen`/`vgc_gen` → `framebuffer_1080p` on HyperRAM (ports 4/5) → `dp_transmitter` → D-PHY → SN75DP130 → TUSB546A → USB-C. Firmware status: FPGA DONE, OSPI link UP (A2FP), HyperRAM calibrated, 0 retries. One replug of the monitor was needed after the firmware reflash (the known stale-attach case). Exercised: keyboard echo, IIgs control panel, system beep over DP audio — all OK; video-pipeline counters over 5 min: 0 shadow-write drops, 0 VGC missed hsyncs, 0 stale-word swaps. No scanline effect: the first build bypassed the composite (scanline_dim + OSD) when the DebugOverlay was disabled — fixed in the next build (the OSD text overlay from the ESP32 would have been hidden too). Tracks A and B of this plan are now both on hardware; what remains is characterization (shadow-read latency under load, DP audio, DOC listening, disk images over the ESP32 link) and winning back the deferred features (TOP_INTEGRATION_PLAN.md §3).

## 6g. SHR black / DOC silent on the new core — SHR root-caused (2026-10-11)

Symptoms on the IIgs (ROM 03, slot 4): text, 80-col, hires and keyboard
fine; IIgs autodetect fires (reg 0x79 = 0x53); Arkanoid goes black when it
switches to SHR; no DOC audio from the monitor (the dp_test tone WAS heard
earlier, so DP audio itself works).

Bus side cleared first (the board rev changed the M2B0/M2SEL/SYNC
termination, so that was the obvious suspect):

- Bus-phase probe build (sha 7562b5f0, SRAM only): sticky masks of which
  `apple_bus` phase counts (18.5 ns) see M2B0 high / M2SEL low, split by
  PHI1 half. Result: M2B0 high at every count of the PHI1-high half and
  counts 2-3 after PHI0 rises; M2SEL low throughout. The sampler (count 12
  of the 2-FF-synced line, ~185 ns after PH0 falls) sits inside the
  140-280 ns window Apple's tech note gives for M2B0 (latch at Q3 falling).
  Consistent with the note; the union mask is wider than 140 ns because
  the tri-stated line decays slowly through the pulldown.
- HyperRAM debug reader (regs 0x34-0x3B over the ESP32 CLI, scratch
  `memdump.py`): with Arkanoid on the black screen the SHR aux words
  (unified words 2/3 at 0x060000+) hold live pixel data, i.e. bank-$E1
  writes are captured and land correctly; AND the framebuffer region
  (0x000000+, 320 words/line) holds rendered RGB565 SHR pixels
  (0x4208 / 0xC002 brick patterns at line 100) versus the all-0x211E
  (IIgs blue) text-mode reference. VGC fetch + renderer + FB write all work.
- `viddbg` in that state: mode 0xF1 (use_vgc=1 SHRG=1 LINEAR=1), shadow rd
  fsm is_vgc=1 cache_valid=1, no missed hsync, no drops.

Root cause — scanout, `hdl/video/framebuffer_1080p.sv`: the left-border
trigger `h_trigger_r <= (cx0_r + 5 == h_border_px_r)` (introduced with the
a2mega 1.0a3 epic squash 0a62d8ca, 08-28). A 640-wide frame scales to the
full 1920 so `h_border = 0`; `cx` runs 0..2199 with active at 0, so the
lookahead must fire at cx = 2195 in the previous line's blanking — the
`+5 == 0` compare never matches, the horizontal tracker never starts, the
frame scans out black. Fix: trigger X computed modulo the raster
(`h_border < 5 ? h_border + 2200 - 5 : h_border - 5`); Apple geometry
unchanged (115). **The a2mega shares this read side — SHR there has very
likely been black since 08-28 and needs a retest after the fix.**

SHR fix verified on hardware: build d08273f0 (0 violations) — Arkanoid
title screen in SHR.

DOC silence root-caused the same evening — not the DOC: with Arkanoid's
music playing the counters showed 62 oscillators enabled, the wavetable
fetch counter running, 0 prime misses / stale fetches / GLU write drops.
The audio mix and 48 kHz strobe are wired exactly as on the a2mega. What
had never been exercised anywhere is **audio on a 4-lane link**: the a2mega
plays audio on 2-lane links only (hubs are HBR2x2, its 1080p is 2-lane),
and the dp_test tone heard on this board was the 720p 2-lane build.
`hdl/displayport/audio/sdp_engine.v` was written for 1 or 2 lanes;
instantiated with LANE_COUNT=4 it fell into the 1-lane byte table,
truncated to 24 bytes, on lane 0 only — lanes 1-3 never carried the
packet, so every audio SDP was malformed and dropped by the sink. Fix: a
4-lane mode per the DP 1.1a packet-to-lane figure (page 72: lane n =
HBn PBn DB(4n..4n+3) PB(4+n) DB(16+4n..) PB(8+n), 12 wire bytes per
lane) with the spec's nibble interleaving between lanes 0<->1 and 2<->3
(section 2.2.6.3). New self-check `hdl/displayport/sim/tb_sdp_4lane.v`
captures all lanes between SS/SE, un-interleaves and compares against an
independent model incl. RS(15,13) parity: PASS for 4 and 2 lanes (1-lane
not covered by the TB; untouched code path). The a2mega inherits this
fix for its direct-cable HBR2x4 case.

**Verified on hardware (evening 10-11):** build 6dfd236e (SDP 4-lane fix;
11 knife-edge violations, SRAM only) — Arkanoid title music through the
monitor. First 4-lane DP audio anywhere in the project. The first
attempt (be07dfb9) had -6 ns at clk_sym: a k-dependent part-select in
the 4-lane byte lookup became a barrel shifter — rewritten as a flat
12-way case (TB unchanged, PASS). Clean build for the flash follows with
M2B0 sampling returned to the shared Q3-falling latch (tech-note edge;
the rev-0 phase sampler was a red herring during the SHR hunt).

Console lessons (cost an hour): opening the USB console with DTR/RTS
asserted resets the ESP32 → it re-holds Apple RESET (beep) and drops HPD
(dark monitor, alt-mode session lost → replug). Use one persistent session
with dtr=rts=False (`scratchpad/console.py`), `+++` enters the CLI. With no
disk images the firmware deliberately left the OSD console up
(`disk.c`: hide only scheduled after a good mount) — `spireg 0x10 0`
hides it by hand.

**Console policy changed (2026-10-11, `disk.c`):** on BOARD_REV 225 the
"A2: RESET RELEASED (NO MEDIA)" path in `disk_poll()` now schedules the
same non-blocking console hide the good-mount path uses (1.5 s readable
delay via `g_hide_console_at_us`; the 2 ms disk task never sleeps), so an
image-less power-on hands the screen to the Apple II ~8.5 s after boot
(7 s release deadline + 1.5 s). Gated by `DISK_HIDE_CONSOLE_ON_NO_MEDIA`,
which is 1 only for 225 — the a2mega keeps the console up on a missing /
empty SD card, where the message is actionable. The menu's manual console
view is unaffected (re-shows on demand; a scheduled hide never fires while
the menu holds the osd_console lockout). When images arrive over the
ESP32 link, the resulting remount schedules its own hide as before.

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
| 2026-10-11 | ESP32 fw `make BOARD_REV=225 DP_TEST=0` (production, OSPI core) — **no-media console hide** | 24df5029 + disk.c policy change (this commit) | arduino-cli 1.2.2 / esp32 3.3.5 | builds; 1025539 B app (48 %) | `.ino.bin` `5f749c971b45c7f53706504a044ab0a029cdc912ab3d7057ceef5c4df1638450`; merged `e53a4b874d0c501bb0f6b48f7cfdf15a9cd0b7fb3105007a3e4c6a05adbf8d02` |
| 2026-10-10 | **`a2p25_dp_test` — FIRST COLORBARS** (hold-link, P7/P8 telemetry) | 6d0b148d | Gowin V1.9.12.01 | 0/0 | `4257f701389f46809ab3f27eab356b95223a39344fed51d0ef4df28b0e09a373` |
| 2026-10-11 | `a2p25_dp_test_auxobs` (LINK_CHECK=2 observe; P9/PA) — **AUX soak clean on 3 sinks** | c7064c6e | Gowin V1.9.12.01 | 0/0; worst setup +0.33 | `d758d1c8b1edc3690a0738b19900e4501165c260b35c5723a1b91932498ce9ec` |
| 2026-10-11 | **`a2p25_dp_test_4l` 1080p ×4 — COLORBARS, OSD 1920×1080@60** (MSA 4ch + AUX hang fixes) | 84b65c72 | Gowin V1.9.12.01 | 0/0 | `b0d9b84f7f666f5a530c48672838c58cf3a6765e3d8a94be75c325cd750f1885` |
| 2026-10-11 | `a2p25_dp_test_4l720` 4 lanes at 720p (discriminator; colorbars) | 84b65c72 | Gowin V1.9.12.01 | 0/0 | `bb41780cbd8dc1b27b10b560cb256dac3afc98e3e3b2324ba2fb60ab340d2bc1` |
| 2026-10-11 | `a2p25_dp_test` 2-lane baseline (same fixes, ERRCNT_READ=1, PB line) | 84b65c72 | Gowin V1.9.12.01 | 0/0 | `ed988c98de513c39c9dd2989953ccac005cae78b65772c53668d12105572f3d3` |
| 2026-10-11 | `a2p25_dp_test_4l` 1080p ×4 (rebuilt on the LINK_CHECK=1 baseline) | this commit | Gowin V1.9.12.01 | 0/0; worst setup +0.22 | `dbf6006bd57e8df194736a49e5f6531788c39f795f29f5ad5b1cb7ee3c94f40d` |
| 2026-10-11 | `a2p25_dp_test` + ECHO_SINK_ADJUST + 200 ms poll (**baseline**) | this commit | Gowin V1.9.12.01 | 0/0; worst setup +0.63 | `bdca565fb27954fdaf856b47d4f7ef6513b29d0b6a3e2d8ff1ef1202fc1d4070` |
| 2026-10-11 | **`a2p25_dp_test` — LINK_CHECK=1 BASELINE** (spec flow; Sceptre 20/20 attaches as `_lc1`, sha a21f23c8…) | this commit | Gowin V1.9.12.01 | 0/0; worst setup +0.16 (thin) | `6ec36c790f12e8d349c2a4935ee6ed8b6ae9808a465922f0d2f72e4a5023bf17` |
| 2026-10-11 | `a2p25_dp_test_hold` (LINK_CHECK=0, the 10-10 first-light policy, bench only) | this commit | Gowin V1.9.12.01 | 0/0; worst setup +0.29 | `4eb4705442788e960f2c8cf3772b379ea95f4830925df963f9f7f15fd71eb6ef` |
| 2026-10-10 | `a2p25_dp_test` + HPD_DISCONNECT_RESETS=1 (**replug recovery verified**: Y 11→22→33, bars return) | this commit | Gowin V1.9.12.01 | 0/0 | `bc08d218a69cf396d0b618d27176b416c6d7a92a0cbf4be69d36ca5dd076a9c2` |
