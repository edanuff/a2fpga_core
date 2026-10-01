<!-- Netlist-derived 2026-10-01 from the a2-p25 board repo @ displayport1 7c47fa0. -->
> **Resolved since extraction (2026-10-01, a2p25_dp_test build):**
> - §4.4 AUX: `IO_TYPE=LVDS25` on L2/L1 is accepted with `BANK_VCCIO=3.3`
>   (same as the a2mega's proven closed-loop AUX). Pin `BANK_VCCIO=3.3`
>   explicitly — without it the tool silently configured bank 2 for 2.5 V.
> - §4.6 D-PHY: the M0 pads need no `.cst` lines; the IP binds A5–B9 itself
>   (verified in the pin report). Lane reversal lives in
>   `hdl/dp/transceiver_bank_dphy.sv` (`REVERSE_LANES`).
> - §4.2 OPI_CLK on a non-GCLK ball: the a2mega OSPI connector already
>   oversamples SCLK on the 54 MHz fabric clock, so this costs nothing at the
>   ≤10 MHz link rate.
> - ⚠ HyperRAM CK# (U6 ball B1) is left floating; the Infineon S27KL0642
>   datasheet says tie it to VCCQ or VSSQ when unused. See
>   [MODERNIZATION_PLAN.md](MODERNIZATION_PLAN.md) §5.

# a2-p25 new board revision (branch `displayport1`) FPGA pinout

## Sources and how the balls were established

- **New board:** netlist exported with kicad-cli from `the a2-p25 board repo` @ `displayport1` (HEAD 7c47fa0; the working tree had no uncommitted schematic edits). Board connectors: J1 = BTB0 and J2 = BTB1 (DF40C-60DS). The old revisions used refs J4 and J5 for the same connectors.
- **BTB pin to SOM IO name:** Sipeed `Tang_Primer_25K_52300_Schematic.pdf` (in /Volumes/Storage/Downloads), page 5, SOM J1 and J2. Board BTBn pin N mates to SOM Jn pin N with no mirroring.
- **IO name to ball:** Gowin TN725-1.2 (Arora V package delay table), sheet GW5A-25, column `GW5A-LV25MG121N`. Cross-checked against the SOM schematic page 3, which also gives the bank grouping.
- **Verification:** I also exported netlists for old revisions `main`, `rev2`, `rev3` and `rev4`. The old `a2p25.cst` matches **rev3** exactly: ESP32 LCAM/parallel, I2S, UART, HDMI. For every one of the 75 signals in the old .cst, the derived ball equals the old .cst ball. That validates the BTB-to-ball method end to end. The two docs agree with the derivation too:
  - memory_bandwidth §3 (DQ L4 L3 J1 J2 G4 H4 H1 H2, RWDS E3, CS# C3, CK J11)
  - the DP doc's M0 ball table
- **Banks:** all user-IO banks are 3.3 V. The board feeds VCCIO0/1, VCCIO2/3 and VCCIO6/7 from +3V3 through FB1, FB3 and FB2. The SOM ties VCCIO4 and VCCIO5/10 to its own VDD_3V3.
- **Clock:** the SOM 50 MHz oscillator `clk` stays on **E2** (IOB12A/GCLKT_10B, bank 5) on the SOM. No board net is involved, so it is unchanged.

## 1. Full table (every BTB pin with an FPGA IO behind it)

Status key: unchanged = same signal on the same ball; moved = signal kept but on a new ball; new = a new function on a ball that had another use or none; NC = SOM pin left unconnected on the board.

### BTB0 (board J1, SOM J1)

| Net | BTB pin | Ball | IO name | Bank | Dir / role | Old .cst name on this ball | Status |
|---|---|---|---|---|---|---|---|
| FPGA_D0 | BTB0.3 | L9 | IOT31A | 0 | bidir, U4 1A0 | a2_d[0] | unchanged |
| FPGA_BUS_~OE | BTB0.4 | H5 | IOT61A/GCLKT_2 | 1 | out, ~OE of U2+U4 (all 4 halves); through U1 it also gates LSF0108 EN; R3 4.7k pull-up | a2_bus_oe | unchanged |
| FPGA_D1 | BTB0.5 | K9 | IOT31B | 0 | bidir | a2_d[1] | unchanged |
| FPGA_A_DIR | BTB0.6 | J5 | IOT61B | 1 | out, U2 1DIR+2DIR | a2_a_dir | unchanged |
| FPGA_D2 | BTB0.7 | J8 | IOT56A | 0 | bidir | a2_d[2] | unchanged |
| FPGA_A0 | BTB0.8 | L5 | IOT63A | 1 | bidir (U2) | a2_a[0] | unchanged |
| FPGA_D3 | BTB0.9 | K8 | IOT56B | 0 | bidir | a2_d[3] | unchanged |
| FPGA_A1 | BTB0.10 | K5 | IOT63B | 1 | bidir | a2_a[1] | unchanged |
| FPGA_D4 | BTB0.11 | F7 | IOT58A | 0 | bidir | a2_d[4] | unchanged |
| FPGA_A2 | BTB0.12 | H8 | IOT66A | 1 | bidir | a2_a[2] | unchanged |
| FPGA_D5 | BTB0.13 | F6 | IOT58B | 0 | bidir | a2_d[5] | unchanged |
| FPGA_A3 | BTB0.14 | H7 | IOT66B | 1 | bidir | a2_a[3] | unchanged |
| FPGA_A4 | BTB0.16 | G7 | IOT68A | 1 | bidir | a2_a[4] | unchanged |
| FPGA_D6 | BTB0.17 | E8 | IOB37A/READY | 4 | bidir; dual-purpose READY pin; R28 1k pull-up | a2_d[6] | unchanged |
| FPGA_A5 | BTB0.18 | G8 | IOT68B | 1 | bidir | a2_a[5] | unchanged |
| **FPGA_D7** | BTB0.19 | **B3** | IOB56A | 4 | bidir, U4 1A7 | uart_tx | **moved** (a2_d[7] was E3) |
| FPGA_A6 | BTB0.20 | F5 | IOT72A | 1 | bidir | a2_a[6] | unchanged |
| **PSRAM_~CS** | BTB0.21 | **C3** | IOB56B | 4 | out, HyperRAM CS# | uart_rx | **new** |
| FPGA_A7 | BTB0.22 | G5 | IOT72B | 1 | bidir | a2_a[7] | unchanged |
| **PSRAM_RWDS** | BTB0.23 | **E3** | IOB60A | 4 | bidir, HyperRAM RWDS | a2_d[7] | **new** |
| FPGA_DONE | BTB0.25 | D7 | IOB64A/DONE | 4 | dedicated config DONE; goes to ESP32 IO48 with R29 1k pull-up | (not in .cst) | unchanged, not a user IO |
| (NC) | BTB0.30 | L6 | IOT23A | 7 | unconnected (was USB_PHY_P in rev3; never in .cst) | none | NC |
| (NC) | BTB0.32 | K6 | IOT23B | 7 | unconnected (was USB_PHY_N) | none | NC |
| **PSRAM_CK** | BTB0.33 | **J11** | IOT1A/GCLKT_15 | 7 | out, HyperRAM CK, single-ended (CK# B1 on U6 is NC) | esp32_spi_sclk | **new** |
| FPGA_A8 | BTB0.34 | K7 | IOT21A | 7 | bidir | a2_a[8] | unchanged |
| FPGA_~M2SEL | BTB0.35 | J10 | IOT1B | 7 | in (U4 2A0, 2DIR=GND, so B to A only) | a2_m2sel_n | unchanged |
| FPGA_A9 | BTB0.36 | J7 | IOT21B | 7 | bidir | a2_a[9] | unchanged |
| FPGA_PHI1 | BTB0.37 | H11 | IOT3A/GCLKT_16 | 7 | in, through U4 2A1 + 27 Ω | a2_phi1 | unchanged |
| FPGA_A10 | BTB0.38 | L7 | IOT19A | 7 | bidir | a2_a[10] | unchanged |
| FPGA_Q3 | BTB0.39 | H10 | IOT3B | 7 | in, U4 2A2 + 27 Ω | a2_q3 | unchanged |
| FPGA_A11 | BTB0.40 | L8 | IOT19B | 7 | bidir | a2_a[11] | unchanged |
| FPGA_7M | BTB0.41 | G11 | IOT7A | 7 | in, U4 2A3 + 27 Ω | a2_7M | unchanged |
| FPGA_A12 | BTB0.42 | L10 | IOT15A | 7 | bidir | a2_a[12] | unchanged |
| FPGA_M2B0 | BTB0.43 | G10 | IOT7B | 7 | in, U4 2A4 (bus side D2 + R18 4.7k pulldown) | a2_mb20 | unchanged (name only) |
| FPGA_A13 | BTB0.44 | K10 | IOT15B | 7 | bidir | a2_a[13] | unchanged |
| FPGA_A14 | BTB0.46 | K11 | IOT11A | 7 | bidir | a2_a[14] | unchanged |
| FPGA_~RDY | BTB0.47 | D11 | IOL9A | 6 | through LSF0108; 4.7k pull-up | a2_rdy_n | unchanged |
| FPGA_A15 | BTB0.48 | L11 | IOT11B | 7 | bidir | a2_a[15] | unchanged |
| FPGA_~IRQ | BTB0.49 | D10 | IOL9B | 6 | LSF0108; 4.7k pull-up | a2_irq_n | unchanged |
| FPGA_~DMA | BTB0.51 | C11 | IOL5A | 6 | LSF0108; 4.7k pull-up | a2_dma_n | unchanged |
| FPGA_D_DIR | BTB0.52 | E11 | IOL3A | 6 | out, U4 1DIR | a2_d_dir | unchanged |
| FPGA_~NMI | BTB0.53 | C10 | IOL5B | 6 | LSF0108; 4.7k pull-up | a2_nmi_n | unchanged |
| FPGA_R/~W | BTB0.54 | E10 | IOL3B | 6 | LSF0108; 4.7k pull-up | a2_rw_n | unchanged |
| FPGA_SYNC | BTB0.55 | B11 | IOL12A | 6 | in, U4 2A5 | a2_sync_n | unchanged |
| FPGA_~INH | BTB0.56 | A11 | IOL14A | 6 | LSF0108; 4.7k pull-up | a2_inh_n | unchanged |
| **FPGA_LED** | BTB0.57 | **B10** | IOL12B | 6 | out, active-high LED D8 (549 Ω to GND) | a2_res_out_n | **new** |
| FPGA_~RES | BTB0.58 | A10 | IOL14B | 6 | LSF0108; 4.7k pull-up (bus /RES sense) | a2_reset_n | unchanged |

Power and ground on BTB0: pins 24/26 = VCCIO0/1 and 29/31 = VCCIO6/7 (both from +3V3 through ferrite beads); the rest are GND.

### BTB1 (board J2, SOM J2)

| Net | BTB pin | Ball | IO name | Bank | Dir / role | Old .cst name on this ball | Status |
|---|---|---|---|---|---|---|---|
| **ESP32_INT_N** | BTB1.3 | **B2** | IOB4A | 5 | interrupt line to ESP32 IO10; FPGA-to-ESP32 inferred from naming, see §4 | esp32_parl_clk | **new** |
| **ESP32_OPI_D0** | BTB1.5 | **C2** | IOB4B | 5 | bidir, ESP32 IO11 | esp32_parl_d[0] | **new** |
| **ESP32_OPI_D1** | BTB1.7 | **F2** | IOB26A/GCLKT_9B | 5 | bidir, ESP32 IO12 | esp32_parl_d[1] | **new** |
| **ESP32_OPI_D2** | BTB1.9 | **F1** | IOB26B | 5 | bidir, ESP32 IO13 | esp32_parl_d[2] | **new** |
| **ESP32_OPI_D3** | BTB1.11 | **A1** | IOB24A | 5 | bidir, ESP32 IO14 | esp32_parl_d[3] | **new** |
| ESP32_RECONFIG_N | BTB1.13 | D8 | IOB1A/RECONFIG_N | 5 | dedicated config input from ESP32 IO38; R32 1k pull-up | (not in .cst) | unchanged, not a user IO |
| **ESP32_OPI_CS** | BTB1.15 | **E1** | IOB12B/GCLKC_10B | 5 | in (ESP32 IO21 is master) | esp32_i2s_lrclk | **new** |
| **ESP32_OPI_CLK** | BTB1.17 | **D1** | IOB14B (not a GCLK pin) | 5 | in, clock from ESP32 IO47 | esp32_i2s_data | **new** |
| **FPGA_AUX_SRC_P** | BTB1.6 | **L2** | IOR18A | 2 | DP AUX + (bidir pseudo-diff) | tmds_d_p[2] (2nd ball) | **new** |
| **FPGA_AUX_SRC_N** | BTB1.8 | **L1** | IOR18B | 2 | DP AUX − | tmds_d_p[2] (1st ball) | **new** |
| **ESP32_OPI_D4** | BTB1.10 | **K1** | IOR20A | 2 | bidir, ESP32 IO15 | tmds_d_p[1] (2nd ball) | **new** |
| **ESP32_OPI_D5** | BTB1.12 | **K2** | IOR20B | 2 | bidir, ESP32 IO16 | tmds_d_p[1] (1st ball) | **new** |
| **ESP32_OPI_D6** | BTB1.14 | **J4** | IOR22A | 2 | bidir, ESP32 IO17 | tmds_d_p[0] (2nd ball) | **new** |
| **ESP32_OPI_D7** | BTB1.16 | **K4** | IOR22B | 2 | bidir, ESP32 IO18 | tmds_d_p[0] (1st ball) | **new** |
| (NC) | BTB1.18 | G2 | IOR24A | 2 | unconnected | tmds_clk_p (2nd ball) | removed / NC |
| (NC) | BTB1.20 | G1 | IOR24B | 2 | unconnected | tmds_clk_p (1st ball) | removed / NC |
| ESP32_JTAG_TCK | BTB1.21 | C1 | IOR1A/TCK | 5/10 | dedicated JTAG; ESP32 IO39 + header J3.4; R31 5.1k pulldown | (not in .cst) | unchanged, keep JTAG enabled |
| **PSRAM_DQ0** | BTB1.22 | **L4** | IOR31A/GCLKT_4 | 2 | bidir | esp32_parl_frame | **new** |
| ESP32_JTAG_TMS | BTB1.23 | B1 | IOR3A/TMS | 5/10 | dedicated JTAG; ESP32 IO40, J3.5 | none | unchanged |
| **PSRAM_DQ1** | BTB1.24 | **L3** | IOR31B | 2 | bidir | esp32_i2s_bclk | **new** |
| ESP32_JTAG_TDO | BTB1.25 | A2 | IOR3B/TDO | 5/10 | dedicated JTAG; ESP32 IO42, J3.3 | none | unchanged |
| **PSRAM_DQ2** | BTB1.26 | **J1** | IOR33A/GCLKT_5 | 2 | bidir | esp32_spi_miso | **new** |
| ESP32_JTAG_TDI | BTB1.27 | A3 | IOR1B/TDI | 5/10 | dedicated JTAG; ESP32 IO41, J3.6 | none | unchanged |
| **PSRAM_DQ3** | BTB1.28 | **J2** | IOR33B | 2 | bidir | esp32_spi_mosi | **new** |
| **PSRAM_DQ4** | BTB1.30 | **G4** | IOB89A/GCLKT_7 | 3 | bidir | a2_int_out_n | **new** |
| **M0_D3_P / M0_D3_N** | BTB1.31 / BTB1.33 | **A5 / B5** | MIPI M0_D3 | MIPI | D-PHY HS TX, AC-coupled into DP130 IN0, which feeds **DP lane 0** | none | **new** |
| **PSRAM_DQ5** | BTB1.32 | **H4** | IOB89B | 3 | bidir | a2_int_in_n | **new** |
| **PSRAM_DQ6** | BTB1.34 | **H1** | IOB91A/GCLKT_6A | 3 | bidir | a2_dma_out_n | **new** |
| **PSRAM_DQ7** | BTB1.36 | **H2** | IOB91B | 3 | bidir | a2_dma_in_n | **new** |
| **M0_D2_P / M0_D2_N** | BTB1.37 / BTB1.39 | **A6 / B6** | MIPI M0_D2 | MIPI | to DP130 IN1, which feeds **DP lane 1** | none | **new** |
| (NC) | BTB1.43 / BTB1.45 | A7 / B7 | MIPI M0_CK | MIPI | clock lane not connected on the board | none | NC |
| **M0_D1_P / M0_D1_N** | BTB1.49 / BTB1.51 | **A8 / B8** | MIPI M0_D1 | MIPI | to DP130 IN2, which feeds **DP lane 2** | none | **new** |
| **M0_D0_P / M0_D0_N** | BTB1.55 / BTB1.57 | **A9 / B9** | MIPI M0_D0 | MIPI | to DP130 IN3, which feeds **DP lane 3** | none | **new** |

Power and ground on BTB1: pins 2/4 = VCCIO2/3 (+3V3 through FB3). Pins 38/40 (VDD_1V8) and 42/44 (VDD_2V5) are SOM outputs and go nowhere on the board. Pins 46/48/50 = +3V3 in, 52–60 even = +5V in, and the rest are GND.

## 2. Functional groups

### Apple II bus
- **Address A0–A15:** L5 K5 H8 H7 G7 G8 F5 G5 K7 J7 L7 L8 L10 K10 K11 L11. Unchanged. They pass through U2 74ALVC164245 with both halves on FPGA_A_DIR.
- **Data D0–D7:** L9 K9 J8 K8 F7 F6 E8 **B3**. Only **D7 moved, from E3 to B3**. They pass through U4 half 1, with DIR = FPGA_D_DIR (E11).
- **Transceiver control:** BUS_~OE on H5 drives all four '245 ~OE pins. Through U1 (74LVC2G06 open-drain) it also controls the LSF0108 EN and VREF_B, so the LSF is enabled when OE is low. A_DIR is on J5 and D_DIR on E11.
- **LSF0108 bidirectional open-drain lines** (each has a 4.7k pull-up to 3V3 on the FPGA side): R/~W E10, ~INH A11, ~RES A10, ~RDY D11, ~IRQ D10, ~DMA C11, ~NMI C10.
- **Inputs through U4 half 2** (2DIR = GND, so these are inputs only): ~M2SEL J10, PHI1 H11, Q3 H10, 7M G11, M2B0 G10, SYNC B11. PHI1, Q3 and 7M also have 27 Ω series resistors on both sides.
- **Reset output:** FPGA_~RES_OUT is now driven by the **ESP32 (IO33)** through U1 open-drain onto bus /RES. The FPGA no longer has a reset-out pin; the old B10 is now the LED.
- **INT/DMA daisy chain:** INTIN is strapped to INTOUT and DMAIN to DMAOUT on the card (P1.28 to P1.23, P1.27 to P1.24). The FPGA no longer sees these signals.
- **Unused slot pins:** PHI0, ~IOSEL, ~DEVSEL and ~IOSTRB are not routed. That is the same as before; they were never in the .cst.

### HyperRAM (U6 S27KL0642, 64 Mb, 3.0 V part on 3.3 V)
- **DQ0–7:** L4 L3 J1 J2 G4 H4 H1 H2. DQ0–3 are in bank 2 and DQ4–7 in bank 3.
- **RWDS:** E3. **CS#:** C3 (both bank 4).
- **CK:** J11 (bank 7, a GCLKT pin). It is single-ended; CK# is not connected, and neither is RESET#, which is correct per memory notes.
- This matches memory_bandwidth_analysis.md §3 exactly.

### DisplayPort (USB-C alt mode)
- **Main link:** the M0 D-PHY data lanes run through 100 nF caps into the SN75DP130 (U7), then through more caps into TUSB546A (U8) DP0–3, then to USB2 (the USB-C video receptacle). **The lane order is reversed and the polarity is straight:**
  - DP lane 0 = M0_D3 (A5 P / B5 N)
  - DP lane 1 = M0_D2 (A6 / B6)
  - DP lane 2 = M0_D1 (A8 / B8)
  - DP lane 3 = M0_D0 (A9 / B9)
  - P goes to P on every lane: DP130 INxp → OUTxp → TUSB DPxP.
  - The gateware must assign DP lane n to D-PHY data lane (3−n). For 2-lane modes (pin assignment D), the lanes used are M0_D3 and M0_D2.
  - The M0 clock lane (A7/B7) is not connected.
- **AUX:**
  - FPGA_AUX_SRC_P is on L2 (IOR18A) and _N on L1 (IOR18B), a true A/B pair in bank 2 at 3.3 V.
  - Each leg has a 100 Ω series resistor (R34 on P, R33 on N). After it is a bias divider: P is 100k to 3V3 and 43k to GND (≈0.99 V); N is 100k to 3V3 and 47k to GND (≈1.06 V). Then a 100 nF cap, then DP130 AUX_SRCp/n.
  - From there the DP130 AUX_SNK goes to DPAUX_P/N (100k to GND on P, 100k to 3V3 on N), then TUSB546A AUXP/N, then SBU1/SBU2.
- **HPD: there is no FPGA HPD pin.**
  - DP130 HPD_SNK is tied high (10k to 3V3); its HPD_SRC and CAD_SRC are NC. TUSB546A HPDIN is NC.
  - HPD is virtual: it arrives as a PD VDM at the FUSB302B, the ESP32 reads it, and the ESP32 passes it to the FPGA in a register. This is the "Option A" from the DP design doc.
- **I2C (DP_I2C_SCL/SDA):** the bus connects to SN75DP130, TUSB546A (I2C_EN pulled high, so it is in I2C mode) and FUSB302B. **It is mastered by the ESP32** (IO4 = SCL, IO3 = SDA, R24/R23 pull-ups). **The FPGA is not on this bus.**
  - FUSB302B INT_N goes to ESP32 IO5.
  - The TPS2553 VBUS-source enable is driven by ESP32 IO6.
  - DP130 RSTN is an RC to GND (C29) and its EN is NC.

### ESP32-S3 link (ESP32-S3-MINI-1-N4R2)
- **8-bit "OPI" bus**, with the ESP32 as master:
  - D0–D3: C2 F2 F1 A1 (bank 5)
  - D4–D7: K1 K2 J4 K4 (bank 2)
  - CS: E1
  - CLK: D1
  - The ESP32 side is IO11–IO18 for D0–D7, IO21 for CS and IO47 for CLK.
- **ESP32_INT_N:** B2 (ESP32 IO10).
- **RECONFIG_N:** ESP32 IO38 drives the dedicated D8 pin.
- **DONE:** the dedicated D7 pin is read by ESP32 IO48.
- **FPGA JTAG:** ESP32 IO39–42 drive TCK/TMS/TDI/TDO on the dedicated C1/B1/A3/A2, which are also on header J3. Keep the JTAG dual-purpose option **off** (the current gprj has JTAG=false, which is correct).
- The old UART (B3/C3), SPI (J11/J1/J2), LCAM/parallel (L4, B2, C2, F2, F1, A1) and I2S (L3, D1, E1) are all gone. Their balls were reused for the functions above.

### Audio
- **There is no audio hardware.** No I2S, DAC or codec: the old esp32_i2s_* is removed, and PCM1809 is in kicad-libs but not placed. Audio would have to travel in-band over DP.

### SD card / USB
- **No SD card on this revision.**
- USB1 is a USB-C port for the ESP32's native USB (D+/D−) and is also a 5 V power input (CC 5.1k Rd, PTC F1).
- USB2 is the DP alt-mode port with PD (FUSB302B, LM66100 VBUS-in, TPS2553 VBUS-out).
- The FPGA's old USB_PHY pins (BTB0.30/32, L6/K6) are now NC.

### LEDs / buttons
- **FPGA:** FPGA_LED on B10, active high (D8, 549 Ω).
- **ESP32:** LED0 on IO1 (D6) and LED1 on IO2 (D7). S1 = ESP32 EN/reset. S2 = ESP32 IO0/BOOT.
- **No buttons on FPGA pins.**

### Clocks
- `clk` on E2 is the 50 MHz SOM oscillator.
- The Apple clocks PHI1 (H11, GCLKT_16), Q3 (H10) and 7M (G11) are unchanged.
- HyperRAM CK is an output on J11.
- The ESP32 OPI CLK is an **input on D1, which is not a GCLK pin** (see §4).

### Config / dual-purpose pins used as IO
- **E8 (READY):** D6.
- **E1 (RDWR_B/GCLKC):** OPI_CS.
- **D1 (CLKHOLD_N/SSPI_HOLDN):** OPI_CLK.
- **B2 / C2 (D08 / D09):** INT_N and OPI_D0.
- The existing `a2p25_process_config.json` already sets READY, CPU, SSPI and I2C as regular IO, and the old design used these same balls. Keep those settings, and keep JTAG, RECONFIG_N and DONE dedicated.

### Spare FPGA balls reachable on the carrier
- These SOM IOs reach a BTB pin, but the new board leaves them unconnected: L6, K6 (BTB0.30/32), G2, G1 (BTB1.18/20) and A7/B7 (M0_CK).

## 3. Old .cst signals that no longer exist

| Old signal | Old ball | New use of that ball |
|---|---|---|
| uart_tx | B3 | FPGA_D7 (a2_d[7]) |
| uart_rx | C3 | PSRAM_CS# |
| tmds_clk_p (G1,G2) | G1/G2 | NC |
| tmds_d_p[0] (K4,J4) | K4/J4 | ESP32_OPI_D7 / ESP32_OPI_D6 |
| tmds_d_p[1] (K2,K1) | K2/K1 | ESP32_OPI_D5 / ESP32_OPI_D4 |
| tmds_d_p[2] (L1,L2) | L1/L2 | AUX_SRC_N / AUX_SRC_P |
| a2_res_out_n | B10 | FPGA_LED (bus reset-out is now driven by the ESP32 IO33) |
| a2_int_out_n | G4 | PSRAM_DQ4 |
| a2_int_in_n | H4 | PSRAM_DQ5 |
| a2_dma_out_n | H1 | PSRAM_DQ6 |
| a2_dma_in_n | H2 | PSRAM_DQ7 |
| esp32_spi_sclk | J11 | PSRAM_CK |
| esp32_spi_miso | J1 | PSRAM_DQ2 |
| esp32_spi_mosi | J2 | PSRAM_DQ3 |
| esp32_parl_frame | L4 | PSRAM_DQ0 |
| esp32_parl_clk | B2 | ESP32_INT_N |
| esp32_parl_d[0..3] | C2 F2 F1 A1 | ESP32_OPI_D0..D3 |
| esp32_i2s_bclk | L3 | PSRAM_DQ1 |
| esp32_i2s_data | D1 | ESP32_OPI_CLK |
| esp32_i2s_lrclk | E1 | ESP32_OPI_CS |

`a2_d[7]` survives but moves from E3 to B3. All other a2_* signals and `clk` keep their balls.

## 4. Ambiguities and open points

1. **ESP32_INT_N direction.** The netlist only shows a point-to-point wire from B2 to ESP32 IO10. The name suggests "interrupt to the ESP32", so the FPGA drives it (probably open-drain or active-low), but nothing in the schematic forces a direction. This needs to be confirmed against the firmware plan.
2. **OPI_CLK on D1 (IOB14B) is not clock-capable.** It cannot use a dedicated GCLK route. If the ESP32 drives the bus synchronously at a meaningful rate, the gateware must either oversample it with a fabric clock or accept the fabric clock-routing penalty. The only GCLK-capable pins in the OPI group are F2 (GCLKT_9B, OPI_D1) and E1 (GCLKC_10B, the complement pin, used for OPI_CS). This is a board-level concern; flag it if a respin is still possible.
3. **The OPI bus is split across banks.** D0–D3, CS and CLK are in bank 5; D4–D7 are in bank 2, on the far side of the die. Both banks are 3.3 V, so this is electrically fine, but expect skew between nibbles at high OPI clock rates.
4. **AUX I/O standard.** The a2mega used "LVDS25 + pulldowns". Here AUX is in bank 2 at VCCIO = 3.3 V. Whether a GW5A-25 bank at 3.3 V permits an LVDS25 / true-LVDS input and the emulated output for a bidirectional AUX (or whether it needs LVCMOS33D/ELVDS-style TX plus an LVDS RX) is **not verified**; check UG289/DS1103. The external bias (N ≈ 1.06 V, P ≈ 0.99 V at idle, AC-coupled) is consistent with a source-side AUX front end. The balls themselves (P = L2 IOR18A, N = L1 IOR18B) are verified.
5. **P/N ball order in the old HDMI lines.** The old .cst listed the B (complement) ball first, e.g. `tmds_d_p[2] L1,L2`. Per the SOM schematic and TN725, L2 = IOR18A is the true/P ball, and the rev3 schematic routed HDMI_TX2.P there; other 25K .cst files (tangcore) put P on the A ball. For the new .cst, put each diff P on the A ball: AUX P = L2. This only matters as a caution against copying the old line format.
6. **D-PHY ball constraints.** The A5/B5…A9/B9 balls are fixed by the hard MIPI block. Whether the Gowin D-PHY TX IP needs explicit IO_LOC lines or binds them implicitly should be checked against the IP's generated example. **The lane reversal (DP lane n = M0_D(3−n)) must be handled in RTL or the IP lane mapping, not in the .cst.**
7. **Bank numbers come from the SOM schematic's symbol grouping.** That grouping shows bank 3 = IOB89/91 and bank 7 = IOT1..23. I did not cross-check it against the Gowin UG pinout xlsx for GW5A-25 MG121, which is not available locally. The balls are verified two ways; the bank labels come from one source.
8. **DP130 auto-AUX-snoop vs I2C.** This is out of .cst scope. The ESP32 owns the DP130 over I2C; the FPGA has no strap or enable pins on the DP130 or TUSB546A.
