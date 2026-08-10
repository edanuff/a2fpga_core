# a2mega 1.0a3 — BTB connector → FPGA ball map

Extracted 2026-08-09 from the a2-mega hardware repo, branch `1.0a3`
(HEAD `273d673`): net names from `a2-mega.kicad_pcb` pad records, FPGA pin
names from the text annotations in `fpga_som_btb1/2.kicad_sch` matched to
connector pins by row coordinate (side-aware: odd pins label left, even
pins label right).

**Validation:** joining the same ball table with the *1.0a2a* PCB nets
reproduces the known-good working `boards/a2mega/hdl/a2mega.cst` exactly —
12/12 signals, counting the deliberate USB D+/D− swap (the schematic calls
J.66/68 `USB_PHY_N/P` = G13/H13; the working cst crossed them, which was
one of the four gamepad bring-up root causes). The SOM sheets use Tang
Mega **138K** pin names; the PG484 package/ball grid is common to the
GW5AT-60, so GPIO balls carry over 1:1 (per `tang_mega_138k_analysis.md`).
Only the SERDES quad lanes differ between dies — covered by the die-true
lane note on the BTB1 sheet.

## ⚠ The FPGA-side pinout MOVED on 1.0a3

1.0a3 re-assigned nets across BTB pins, so the fabric `.cst` changes for
the OSPI link, UART, and PPO — not just the removed HDMI/USB/SD:

| Signal | 1.0a2x ball (a2mega.cst) | 1.0a3 ball |
|---|---|---|
| ESP32_OPI_D0..D7 | E13 E14 C14 C15 C13 B13 A13 A14 | **A14 B15 B16 A15 A16 D17 C17 B17** |
| ESP32_OPI_CLK | B20 | **A18** |
| ESP32_OPI_CS | A19 | **B18** |
| ESP32_TX (FPGA rx) | V14 (`uart_rx`)¹ | **G13** (old usb_dp ball) |
| ESP32_RX (FPGA tx) | U15 (`uart_tx`)¹ | **H13** (old usb_dm ball) |
| ESP32_PPO_D0..D3 | (8-bit, elsewhere) | **C14 C15 C13 B13** (4-bit) |
| ESP32_PPO_CLK / SYNC | — | **A13 / E14** |
| ESP32_INT_N (new) | — | **E13** (old OPI_D0 ball) |
| ESP32_GPIO0..5 (new) | — | **A19 B20 A20 B21 A21 C20** |
| dip_switches_n / SW1-4 | J16 F13 F14 F15 | J16 F13 F14 F15 (unchanged) |
| led[0] / led[1] | D15 / D14 | D15 / D14 (unchanged) |
| led[2] / led[3] (new, J3.55/57) | — | **R14 / P14** |
| tmds_* (HDMI) | G16,G15 / H14.. | gone — G15/G16 are now DP AUX |

¹ cst names on 1.0a2x follow FPGA direction: `uart_rx` = data *from* the
ESP32 (net ESP32_TX), `uart_tx` = data *to* it (net ESP32_RX).

## DP-specific pins (BTB1 / J2)

| Net | J2 pin | FPGA ball | Note |
|---|---|---|---|
| DPAUX_P | 64 | **G15** (BANK3, IOR105A) | old tmds clk pair — true diff pair; drive as two LVCMOS33 |
| DPAUX_N | 62 | **G16** (BANK3, IOR105B) | pseudo-diff idiom in the top |
| ESP32_GPIO0 = DP HPD | 17 | **A19** (BANK2, IOR38B) | level + 0.75 ms IRQ pulses from the ESP32 |
| DP_REFCLK_P/N | 73/71 | Q0_REFCLK1 pads (E10/F10) | dedicated bumps — no cst entry |
| DP0..DP3 P/N | 77/79 83/85 89/91 95/97 | Q0 SERDES pads | dedicated bumps — no cst entry; die-true map DP0←L3 DP1←L2 DP2←L1 DP3←L0 |

## Full 1.0a3 J2 (BTB1) signal table

| J2 | Net | Ball (IO) |
|---|---|---|
| 4 | ESP32_GPIO5 | C20 (IOR47B) |
| 5 | ESP32_GPIO4 | A21 (IOR44B) |
| 7 | ESP32_GPIO3 | B21 (IOR44A) |
| 11 | ESP32_GPIO2 | A20 (IOR40B) |
| 13 | ESP32_GPIO1 | B20 (IOR40A) |
| 17 | ESP32_GPIO0 | A19 (IOR38B) |
| 19 | ESP32_OPI_CLK | A18 (IOR38A) |
| 23 | ESP32_OPI_CS | B18 (IOR24B) |
| 25 | ESP32_OPI_D7 | B17 (IOR24A) |
| 29 | ESP32_OPI_D6 | C17 (IOR26B) |
| 31 | ESP32_OPI_D5 | D17 (IOR26A) |
| 34 | LED D1 (via R23) | D15 (IOR4B) |
| 35 | ESP32_OPI_D4 | A16 (IOR20B) |
| 36 | LED D2 (via R24) | D14 (IOR4A) |
| 37 | ESP32_OPI_D3 | A15 (IOR20A) |
| 40 | SW4 | F15 (IOR1A) |
| 41 | ESP32_OPI_D2 | B16 (IOR22B) |
| 42 | SW3 | F14 (IOR11B) |
| 43 | ESP32_OPI_D1 | B15 (IOR22A) |
| 44 | SW2 | F13 (IOR11A) |
| 47 | ESP32_OPI_D0 | A14 (IOR15B) |
| 48 | SW1 | J16 (IOR109A) |
| 49 | ESP32_PPO_CLK | A13 (IOR15A) |
| 53 | ESP32_PPO_D3 | B13 (IOR17B) |
| 55 | ESP32_PPO_D2 | C13 (IOR17A) |
| 59 | ESP32_PPO_D1 | C15 (IOR8B) |
| 61 | ESP32_PPO_D0 | C14 (IOR8A) |
| 62 | DPAUX_N | G16 (IOR105B) |
| 64 | DPAUX_P | G15 (IOR105A) |
| 65 | ESP32_PPO_SYNC | E14 (IOR2B) |
| 66 | ESP32_TX | G13 (IOR98B) |
| 67 | ESP32_INT_N | E13 (IOR2A) |
| 68 | ESP32_RX | H13 (IOR98A) |
| 71/73 | DP_REFCLK_N/P | Q0_REFCLK1 (dedicated) |
| 77…97 | DP0..DP3 lanes | Q0 SERDES (dedicated) |

## Master table

The complete machine-generated table for **all three connectors** (J1
Apple bus, J2 ESP32/DP, J3 GS), including ESP32-S3 GPIO numbers and the
60K/138K SERDES lane naming, lives in
**[`a2mega_pinmap_1_0a3.csv`](a2mega_pinmap_1_0a3.csv)** — regenerate
with `boards/a2mega/tools/btb_pinmap.py` (this is required for 1.0a4,
which moves GS nets and adds HyperRAM). Extra cross-validation: the four
J1 Apple-bus spot checks (a2_phi1 H19, a2_d[0] N18, a2_a[0] T20,
a2_a[15] G17) match the working `a2mega.cst` exactly.

J3 highlights for the Phase 3 port: GS address bus on BANK4 (A0=V17 …
A15=AA19); data bus D0-D7 = N15 M15 M16 N17 P17 M17 U20 V20 (mixed
BANK3/BANK4); control mostly on BANK5 (`FPGA_GS_RDY` = AB16 in,
**`FPGA_GS_RDY_OUT` = V15** out via the new open-drain buffer,
`FPGA_GS_VP` = W14, PH2 = AB17, RESET = AA15); LEDs D3/D4 = R14/P14.

SOM-internal pins (unchanged from a2mega.cst): 50 MHz osc `clk` = V22,
`button` = AB13, DDR3 bank 1. The old USB-A pins G13/H13 are repurposed
as the UART — see above.
