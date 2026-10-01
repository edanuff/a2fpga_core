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
| A1 | `a2p25_dp_test`: 720p60 RBR×2 colorbars + tone, AUX telemetry | **built, 0 violations, TB-verified PHY** |
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
| B1 | `hyperram_ctrl` (custom, see §4) + Infineon-model TB | in progress |
| B2 | `hyperram_bist` + ESP32 register access + standalone `a2p25_psram_test` project, built at several CK rates | in progress |
| B3 | DOC5503: `sound_glu` + `doc5503_pipelined` on a HyperRAM port | todo |
| B4 | Framebuffer on PSRAM (the existing render/scanout mechanism) feeding DP | todo |
| B5 | Shadow memory in PSRAM (`apple_memory_sdram`, `VGC_IN_SDRAM`) | todo |

## 3. Clocking

All clocks come from the 50 MHz SOM oscillator on E2.

| Clock | Source | Use |
|---|---|---|
| 100 MHz | `gowin_mgmt_pll` (VCO 1200) | AUX bit timing, link policy |
| 54 MHz | system PLL | logic clock; D-PHY PLL reference |
| 108 MHz, plus a 90° copy | system PLL, retuned to VCO 1080 (B track) | HyperBus CK; `hyperram_ctrl` core |
| 810 MHz ×4 phases | D-PHY PLL (CLKIN 54 MHz, ×15) | serializer |
| 81 MHz | D-PHY PLL `clkout4` = VCO ÷ 10 | DP link-symbol clock |
| 101.25 MHz | D-PHY `TX_CLK_O` = VCO ÷ 8 | gearbox |
| 74.25 / 148.5 MHz | pixel PLL fed by 81 MHz (VCO 891, ÷12 or ÷6) | DP pixel clock, synchronous clocking |

The dp_test keeps the old system PLL (VCO 1350), because it only needs 54 MHz.
The PSRAM test and the full core move it to VCO 1080 (IDIV 5, MDIV 108). That
gives 27, 54 and 108 MHz plus a 108 MHz 90° copy from one VCO.

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

## 5. ⚠ HyperRAM CK# floats — on both boards

The S27KL0642 datasheet, Table 1 note 2, says to tie CK# to VCCQ or VSSQ when
it is not connected, and not to leave it floating. Both boards leave it open:

- **a2-p25 displayport1:** U6 ball B1 is no-connect.
- **a2-mega 1.0a5:** U17 ball B1 is `unconnected-(U17-~{CK}-PadB1)`.
  **Fix this before the a2mega turn: tie it to VSSQ.**

RESET# (A4) is unconnected on both boards, which is fine: it has an internal
pull-up. The a2p25 is already built, so the B2 standalone test measures whether
the floating CK# matters, and at which CK rates. Also check whether the ISSI or
Winbond equivalents document a floating CK# as allowed. If one does, that part
is a no-respin fix at assembly.

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
   census, both gates passing). Use the LED ladder:
   - slow blink: alive
   - fast blink: HPD
   - double blink: link up
   - solid: video

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
