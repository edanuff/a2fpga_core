# SOM ball-map compatibility audit — GW5AT-60B ("60B") vs GW5AST-138B ("138B")

**Deliverable for the 1.0a4 board-rev freeze (2026-08-21).** Question: the
a2mega carrier is designed to accept either Tang Mega SOM —
`GW5AT-LV60PG484AC1/I0` or `GW5AST-LV138PG484AC1/I0`, same PG484 package.
Does the 138 die map the *same functions* to the balls the carrier uses?

---

## FREEZE VERDICT

> **CARRIER COMPATIBLE AS-ROUTED.** Every one of the 180 carrier-used balls
> (135 BTB signals from the 1.0a3 pin map + the SOM-internal DDR3/clock/button
> set) carries a functionally equivalent pin on both dies: **176 MATCH,
> 4 DIFFER — and all four DIFFERs are SERDES *lane-number* relabelling of the
> same physical transceiver pads, not a change of function.** No carrier trace
> lands on a pin that is I/O on one die and power/NC/dedicated on the other; no
> bank-VCCIO conflict exists on the 138B (the 60B's only conflict, `button` on
> bank 9, actually *disappears* on the 138B). **No board change is required for
> 1.0a4 to accept both SOMs.**
>
> The one thing that is **not** interchangeable is the **bitstream**: the
> DP SERDES IP must be regenerated for the 138's lane numbering (see §2), the
> DDR3/PLL IP must be regenerated per die (already known), and the `.cst` needs
> the 138B IO-type/bank deltas already captured in `hdl/a2mega_138b.cst`.
> Bitstreams were never interchangeable across dies anyway.

---

## 1. Method and sources

| Source | Used for | Trust |
|---|---|---|
| `IDE/data/device/GW5AT-60B/PBGA484A.json` and `.../GW5AST-138B/PBGA484A.json` (Gowin IDE V1.9.12.03 bundle) | **Primary.** Per-ball `NAME` / `TYPE` / `BANK` / `DIFF` / `PAIR` / `DQS` / `TRUELVDS` for all 484 balls of both dies. | Vendor package database — the same data PnR itself uses. |
| `docs/a2mega_pinmap_1_0a3.csv` | Which balls the carrier actually uses (135 BTB signals), and the previously-recorded per-die lane columns. | Machine-extracted from the a2-mega KiCad repo; validated 12/12 against the working 1.0a2 `.cst`. |
| `hdl/a2mega.cst`, `hdl/a2mega_138b.cst`, `hdl/dp_test/a2mega_dp_test.cst` | The SOM-internal set (DDR3, 50 MHz osc, button, reset) and the known 138B deltas. | In-repo, hardware-proven on the 60B. |
| `docs/tang_mega_138k_analysis.md`, `docs/btb_pinout_1_0a3.md` | Prior 138B findings (IO types, `button` bank move, DDR3 ball identity). | Cross-checked here against the package DB — **all confirmed**. |

The audit is a programmatic join: every carrier-used ball → the two dies'
package entries → per-ball verdict. Nothing in the table below is transcribed
by hand.

---

## 2. The DP main link — the one real DIFFER (quad index and lane numbers)

**Quad index: MATCH.** Only **Q0** bonds out on PG484 on *both* dies (the
package DBs contain `Q0_*` pads only; no `Q1_*` pad exists on either). The
carrier's four DP pairs and the REFCLK pair all belong to Q0 on both.

**Lane numbering: DIFFER — Q0 lanes 1 and 3 are transposed between the dies.**

| Carrier net | J2 pins | Balls (P/N) | 60B lane | 138B lane | Verdict |
|---|---|---|---|---|---|
| **DP0** | 77 / 79 | C7 / D7 | `Q0_LN3_TXM_O` / `Q0_LN3_TXP_O` | `Q0_LN1_TXM_O` / `Q0_LN1_TXP_O` | **DIFFER (lane 3 → lane 1)** |
| **DP1** | 83 / 85 | A6 / B6 | `Q0_LN2_TXM_O` / `Q0_LN2_TXP_O` | `Q0_LN2_TXM_O` / `Q0_LN2_TXP_O` | MATCH |
| **DP2** | 89 / 91 | C5 / D5 | `Q0_LN1_*` | `Q0_LN3_*` | **DIFFER (lane 1 → lane 3)** |
| **DP3** | 95 / 97 | A4 / B4 | `Q0_LN0_*` | `Q0_LN0_*` | MATCH |
| **DP_REFCLK** | 73 / 71 | F10 / E10 | `Q0_REFCLKP_1` / `Q0_REFCLKM_1` | **identical** | **MATCH** |

Consequences, in order of importance:

1. **The two pairs we actually use become Q0 lanes 1 + 2 on the 138B**
   (they are lanes 2 + 3 on the 60B). Production drives DP0 = DP main-link
   lane 0 and DP1 = main-link lane 1, so on the 138B: **ML0 → die lane 1,
   ML1 → die lane 2**.
2. **The clock/bonding master does not move.** DP1 is die lane 2 on *both*
   dies, and DP1 is the master. The `.sdc` anchor pin
   `.../gtr12_quad_inst0/LANE2_PCS_TX_O_FABRIC_CLK` is therefore
   **unchanged** on the 138B — a genuinely convenient accident.
3. **TXM/TXP ordering is identical on both dies** (TXM on the lower-numbered
   ball of every pair), so the carrier's uniform P/N swap is compensated the
   same way: `tx_pol_invert` on both used lanes. No polarity rework.
4. **Ticket relevance (Gowin generator defect).** `WS4_GOWIN_STACK.md` §9/§11
   convicts the EDP-PHY/Customized-PHY generator of emitting **raw-mode** PCS/PMA
   mode fields (`0xNN6C[13:12]`, `0xNNa0[9]`, `0xNNa6[17:16]`) when the *hardened
   8b10b* preset targets **lanes 2/3**, while emitting them correctly for lanes
   0/1. On the 138B our pair **straddles the two halves** — die lane 1 is in the
   known-good half, die lane 2 is in the defective half. Two implications:
   - Our **production** DP path is *raw 20-bit + fabric 8b10b* and is
     **unaffected** on either die (the defect is specific to the hardened-8b10b
     configuration).
   - The 138B is a **better ticket vehicle**: the same board, same carrier, same
     hardened preset, with one used lane on each side of the defect boundary —
     a single-build A/B that no 60B configuration can produce.
5. **4-lane future (RBR scope).** All four pairs remain Q0 lanes 0-3 on both
   dies; only the *labels* permute. A 4-lane build's ML→die-lane map becomes
   ML0→ln1, ML1→ln2, ML2→ln3, ML3→ln0 on the 138B (vs ln3/ln2/ln1/ln0 on the
   60B). No routing consequence.

**REFCLK1 ball: MATCH, no action.** F10/E10 are `Q0_REFCLKP_1`/`Q0_REFCLKM_1`
on both dies, so the DSC1103 135 MHz LVDS oscillator feeds the same dedicated
reference input, and `REFERENCE_CLOCK_SOURCE = Q0 REFCLK1` carries over
verbatim.

---

## 3. Fabric pins — all MATCH as *pins*; bank membership and IO names move

Every fabric ball the carrier uses is a general-purpose `I/O` pad on **both**
dies. What changes is cosmetic-but-load-bearing metadata:

| What changes | Detail | Action |
|---|---|---|
| **IO name** | Bank-relative names differ die to die (`G15` = `IOR24A` on the 60B, `IOR105A` on the 138B). | None — `.cst` constrains by *ball*, not IO name. |
| **Bank number** | Nearly every ball changes bank (the 60B's 12 banks vs the 138B's different partition). E.g. LEDs bank 1→2, `V22` bank 7→4, `AB13` bank **9→5**. | Only matters through `BANK_VCCIO` — see below. |
| **DQS group** | Group labels move (`DQ4`→`DQ16` etc.). | None for our uses (no source-synchronous carrier interface). |
| **True-LVDS pairing** | Preserved on every differential use. Five *single-ended* balls lose their pair partner on the 138B (`P20` FPGA_D_DIR, `F21` FPGA_~RES_OUT, `F15` SW4, `N15` FPGA_GS_D0, `Y17` FPGA_GS_IRQ). | None — all five are single-ended signals. |

### 3.1 The dp_test pin set specifically

| Signal | Ball | 60B | 138B | Verdict | Note |
|---|---|---|---|---|---|
| `dp_aux_p` / `dp_aux_n` | **G15 / G16** | `IOR24A/B`, bank 4, true pair | `IOR105A/B`, bank 3, **still a true pair** | MATCH | The `LVDS25`-out AUX front end (`AUX_TLVDS`) stays legal — G15/G16 remain a true differential pair with `TRUELVDS=true` on the 138B. |
| `dp_hpd` | **A19** | `IOT113B`, bank 2 | `IOR38B`, bank 2 | MATCH | Same bank number by coincidence. |
| `uart_tx` (1.0a3) | **H13** | `IOR20A`, bank 4 | `IOR98A`, bank 3 | MATCH | |
| `uart_rx` (1.0a3) | **G13** | `IOR20B`, bank 4 | `IOR98B`, bank 3 | MATCH | |
| `led[0]` / `led[1]` | **D15 / D14** | `IOT82B/A`, bank 1 | `IOR4B/A`, bank 2 | MATCH | |
| `led[2]` / `led[3]` | **R14 / P14** | `IOB146B/A`, bank 6 | `IOB133B/A`, bank 4 | MATCH | |
| `clk50_in` | **V22** | `IOB117B`, bank 7 | `IOB104B`, bank 4 | MATCH | 50 MHz SOM oscillator. |
| `button` | **AB13** | `IOB54A`, bank **9** (1.5 V with DDR3) | `IOB89B`, bank **5** (3.3 V group) | MATCH (pin) / **bank move** | Must be `LVCMOS33` on the 138B — already handled in `a2mega_138b.cst`. |

### 3.2 OSPI / ESP32 link, Apple bus, GS bus

All present and I/O on both dies — see the full table in §6. Both the
1.0a2x OSPI ball set (`E13 E14 C14 C15 C13 B13 A13 A14`, `B20`) and the
1.0a3 set (`A14 B15 B16 A15 A16 D17 C17 B17`, `A18`, `B18`) are I/O on both
dies, as are `ESP32_GPIO0..5` (`A19 B20 A20 B21 A21 C20`), the whole J1 Apple
bus, and the whole J3 GS bus.

### 3.3 Bank-VCCIO check (the failure mode that actually bites)

Grouping every carrier-used ball by its **138B** bank and the voltage its
signal needs:

| 138B bank | Content | Verdict |
|---|---|---|
| 2 | 41 signals, all 3.3 V | ok |
| 3 | 44 signals, all 3.3 V | ok |
| 4 | 34 signals, all 3.3 V | ok |
| 5 | 13 signals, all 3.3 V (GS control + `button`) | ok |
| 6 | 22 signals, all 1.5 V (DDR3 DQ/DQS/DM) | ok |
| 7 | 28 signals, all 1.5 V (DDR3 addr/cmd + `rst`) | ok |
| 10 | `FPGA_DONE` (G11), 3.3 V | ok |

**Zero conflicts on the 138B.** For contrast, the same signal set on the 60B
puts `button` (3.3 V-tolerant input) in bank **9** alongside the 1.5 V DDR3
byte lane — which is precisely why `a2mega.cst` constrains `button` as
`LVCMOS15`. The 138B partition is the friendlier of the two.

---

## 4. Non-ball differences that still gate a 138B bitstream

These are *not* carrier issues (nothing to change on the board), but they are
what a 138B build must handle. Recorded here so the freeze decision is not
mistaken for "the same bitstream runs".

1. **DP SERDES IP**: regenerate for **Q0 lanes 1 + 2**, master lane 2
   (§2). Everything else in the config is die-independent: REFCLK1 @ 135 MHz,
   QPLL0, 2.7 Gbps, width 20 raw, `tx_pol_invert` on both used lanes, DRP on.
   ⚠ **This regeneration cannot be done by hand or headlessly** — it is not a
   lane rename of the 60B emission. The dies use *different silicon
   primitives*: the 60B emission instantiates `GTR12_QUADA`, and synthesising
   it for the 138B fails with `ERROR (RP0008): There is no GTR12_QUADA
   resource in current device`. The 138 die uses the multi-quad
   **`GTR12_QUAD`** (parameter `POSITION = "Q0"|"Q1"`), whose port set differs
   by 43 ports — the extra ones being inter-quad interconnect and
   reference-clock steering (`INET_Q0_Q1`, `INET_Q_UPAR`, `INET_Q_PMAC`,
   `FABRIC_REFCLK_OE_L_I/R_I`, `FABRIC_REFCLK_OUTPUT_SEL_I[4:0]`).
   Consistently, the vendor's per-die CSR generator demands quad-topology
   inputs the 60 flow lacks (`refomux0_sel`, `ref_prop_dir`, a full `[q1]`
   block). Generation must run in the IDE IP Core Generator; the exact field
   list is `hdl/gowin/138B/dp_serdes/README.md`.
   *(Note this affects the die, not the package: only Q0 bonds out on PG484 on
   both dies — the 138's second quad exists on the die but has no pads here.)*
2. **DDR3 IO types**: the GW5AST die has no `SSTL15_I`/`SSTL15D_I` (PnR CT1109)
   — use `SSTL15`/`SSTL15D`, `DRIVE=8`. Already in `a2mega_138b.cst`.
3. **PLLs**: the GW5AST die has **no PLLA resource at all** (PnR RP0008); every
   PLL must be the PLL + `PLL_INIT` style, and exact 54 MHz needs the ODIV0
   fractional divider. Handled by `hdl/clocks_138b.sv` +
   `hdl/gowin/138B/gowin_pll/`. (dp_test uses its own PLLs — see the build notes
   in the 138B project.)
4. **`button` IO type** `LVCMOS15` → `LVCMOS33` (§3.1).
5. **Die revisions are not interchangeable** with each other either: the 138K
   exists in B and C revisions and each needs its own `.gprj` and released `.fs`.

---

## 5. What was checked and found clean (negative results worth recording)

- **No carrier ball is power/GND/NC on one die and I/O on the other.** Checked
  all 180; zero `TYPE` mismatches.
- **No second SERDES quad is bonded out** on PG484 on either die (the 138 die
  *has* a Q1 — see §4 — but none of its pads reach this package), so nothing
  can be gained or lost by quad reassignment on the carrier.
- **Rotating the carrier's DP0..DP3 → TUSB1046A assignment cannot make the two
  SOMs lane-identical**, so it is not a 1.0a4 option worth spending routing on.
  Because the dies transpose lanes 1↔3, *any* choice of two adjacent pairs
  lands on different die-lane pairs: DP0+DP1 gives {3,2} on the 60B and {1,2}
  on the 138B; DP1+DP2 gives {2,1} and {2,3}. A per-die IP regeneration is
  required under every routing, so the current routing is as good as any.
- **The DP pads' TXM/TXP polarity order is identical**, so the "all four pairs
  P/N swapped" carrier fact — and its `tx_pol_invert` compensation — is
  die-independent.
- **The prior recorded lane mapping was right.** `a2mega_pinmap_1_0a3.csv`'s
  `lane_60k`/`lane_138k` columns (extracted from the Sipeed SOM schematics)
  agree with the Gowin package database on all eight DP balls and both REFCLK
  balls. Two independent sources, no discrepancy.
- **`docs/tang_mega_138k_analysis.md`'s claims re-verified**: DDR3 balls
  identical across dies ✔; `button` AB13 bank 9 → 5 ✔.

---

## 6. Full per-ball table (180 balls)

Legend: **MATCH** = same function class on both dies (I/O pad, or the same
dedicated pad). **DIFFER** = the die presents a different function — here, only
SERDES lane relabelling. Bank numbers are informational; they never break
compatibility on their own, only through `BANK_VCCIO` grouping (§3.3, clean).

| Net / signal | BTB pin | Ball | 60B function (GW5AT-60B) | 138B function (GW5AST-138B) | Verdict |
|---|---|---|---|---|---|
| `FPGA_BUS_~{OE}` | J1.13 | **U21** | IOB115B (bank 7) | IOB97B (bank 4) | MATCH |
| `FPGA_A_DIR` | J1.15 | **T21** | IOB115A (bank 7) | IOB97A (bank 4) | MATCH |
| `FPGA_A0` | J1.19 | **T20** | IOB111B (bank 7) | IOB102B (bank 4) | MATCH |
| `FPGA_A1` | J1.23 | **R19** | IOB113B (bank 7) | IOB99B (bank 4) | MATCH |
| `FPGA_A2` | J1.25 | **P19** | IOB113A (bank 7) | IOB99A (bank 4) | MATCH |
| `FPGA_A3` | J1.31 | **N22** | IOR63A (bank 5) | IOR89A (bank 3) | MATCH |
| `FPGA_A4` | J1.33 | **M22** | IOR63B (bank 5) | IOR89B (bank 3) | MATCH |
| `FPGA_D_DIR` | J1.36 | **P20** | IOB124A (bank 7) | IOB92A (bank 4) | MATCH |
| `FPGA_A5` | J1.37 | **M21** | IOR68A (bank 5) | IOR71A (bank 3) | MATCH |
| `FPGA_A6` | J1.39 | **L21** | IOR68B (bank 5) | IOR71B (bank 3) | MATCH |
| `FPGA_D0` | J1.40 | **N18** | IOR72A (bank 5) | IOR92A (bank 3) | MATCH |
| `FPGA_A7` | J1.41 | **L19** | IOR61A (bank 5) | IOR85A (bank 3) | MATCH |
| `FPGA_D1` | J1.42 | **N19** | IOR72B (bank 5) | IOR92B (bank 3) | MATCH |
| `FPGA_A8` | J1.43 | **L20** | IOR61B (bank 5) | IOR85B (bank 3) | MATCH |
| `FPGA_D2` | J1.44 | **M18** | IOR66A (bank 5) | IOR87A (bank 3) | MATCH |
| `FPGA_A9` | J1.45 | **K21** | IOR59A (bank 5) | IOR76A (bank 3) | MATCH |
| `FPGA_D3` | J1.46 | **L18** | IOR66B (bank 5) | IOR87B (bank 3) | MATCH |
| `FPGA_A10` | J1.47 | **K22** | IOR59B (bank 5) | IOR76B (bank 3) | MATCH |
| `FPGA_D4` | J1.48 | **K18** | IOR57A (bank 5) | IOR83A (bank 3) | MATCH |
| `FPGA_A11` | J1.49 | **J22** | IOR45A (bank 5) | IOR74A (bank 3) | MATCH |
| `FPGA_D5` | J1.50 | **K19** | IOR57B (bank 5) | IOR83B (bank 3) | MATCH |
| `FPGA_A12` | J1.51 | **H22** | IOR45B (bank 5) | IOR74B (bank 3) | MATCH |
| `FPGA_D6` | J1.52 | **H17** | IOR32A (bank 4) | IOR96A (bank 3) | MATCH |
| `FPGA_A13` | J1.53 | **J20** | IOR54A (bank 5) | IOR78A (bank 3) | MATCH |
| `FPGA_D7` | J1.54 | **H18** | IOR32B (bank 4) | IOR96B (bank 3) | MATCH |
| `FPGA_A14` | J1.55 | **J21** | IOR54B (bank 5) | IOR78B (bank 3) | MATCH |
| `FPGA_~{M2SEL}` | J1.56 | **J19** | IOR41A (bank 5) | IOR80A (bank 3) | MATCH |
| `FPGA_A15` | J1.57 | **G17** | IOR30A (bank 4) | IOR107A (bank 3) | MATCH |
| `FPGA_PHI1` | J1.58 | **H19** | IOR41B (bank 5) | IOR80B (bank 3) | MATCH |
| `FPGA_R{slash}~{W}` | J1.59 | **G18** | IOR30B (bank 4) | IOR107B (bank 3) | MATCH |
| `FPGA_Q3` | J1.62 | **H20** | IOR43A (bank 5) | IOR69A (bank 3) | MATCH |
| `FPGA_7M` | J1.64 | **G20** | IOR43B (bank 5) | IOR69B (bank 3) | MATCH |
| `FPGA_~{INH}` | J1.65 | **G21** | IOT144A (bank 2) | IOR53A (bank 2) | MATCH |
| `FPGA_~{RES}` | J1.67 | **G22** | IOT144B (bank 2) | IOR53B (bank 2) | MATCH |
| `FPGA_M2B0` | J1.68 | **F19** | IOT124A (bank 2) | IOR33A (bank 2) | MATCH |
| `FPGA_~{RDY}` | J1.69 | **F18** | IOT117A (bank 2) | IOR35A (bank 2) | MATCH |
| `FPGA_SYNC` | J1.70 | **F20** | IOT124B (bank 2) | IOR33B (bank 2) | MATCH |
| `FPGA_~{IRQ}` | J1.71 | **E18** | IOT117B (bank 2) | IOR35B (bank 2) | MATCH |
| `FPGA_~{RES}_OUT` | J1.72 | **F21** | IOT142A (bank 2) | IOR55A (bank 2) | MATCH |
| `FPGA_~{DMA}` | J1.73 | **C22** | IOT133A (bank 2) | IOR42A (bank 2) | MATCH |
| `FPGA_~{INTOUT}` | J1.74 | **E22** | IOT140A (bank 2) | IOR49A (bank 2) | MATCH |
| `FPGA_~{NMI}` | J1.75 | **B22** | IOT133B (bank 2) | IOR42B (bank 2) | MATCH |
| `FPGA_~{INTIN}` | J1.76 | **D22** | IOT140B (bank 2) | IOR49B (bank 2) | MATCH |
| `FPGA_DONE` | J1.77 | **G11** | IOR9A (bank 3) | IOB171B (bank 10) | MATCH |
| `FPGA_~{DMAOUT}` | J1.78 | **E21** | IOT138A (bank 2) | IOR51A (bank 2) | MATCH |
| `FPGA_~{DMAIN}` | J1.80 | **D21** | IOT138B (bank 2) | IOR51B (bank 2) | MATCH |
| `ESP32_GPIO5` | J2.4 | **C20** | IOT131B (bank 2) | IOR47B (bank 2) | MATCH |
| `ESP32_GPIO4` | J2.5 | **A21** | IOT135B (bank 2) | IOR44B (bank 2) | MATCH |
| `ESP32_GPIO3` | J2.7 | **B21** | IOT135A (bank 2) | IOR44A (bank 2) | MATCH |
| `ESP32_GPIO2` | J2.11 | **A20** | IOT122B (bank 2) | IOR40B (bank 2) | MATCH |
| `ESP32_GPIO1` | J2.13 | **B20** | IOT122A (bank 2) | IOR40A (bank 2) | MATCH |
| `ESP32_GPIO0` | J2.17 | **A19** | IOT113B (bank 2) | IOR38B (bank 2) | MATCH |
| `ESP32_OPI_CLK` | J2.19 | **A18** | IOT113A (bank 2) | IOR38A (bank 2) | MATCH |
| `ESP32_OPI_CS` | J2.23 | **B18** | IOT104B (bank 1) | IOR24B (bank 2) | MATCH |
| `ESP32_OPI_D7` | J2.25 | **B17** | IOT104A (bank 1) | IOR24A (bank 2) | MATCH |
| `ESP32_OPI_D6` | J2.29 | **C17** | IOT106B (bank 1) | IOR26B (bank 2) | MATCH |
| `ESP32_OPI_D5` | J2.31 | **D17** | IOT106A (bank 1) | IOR26A (bank 2) | MATCH |
| `LED D1 (R23)` | J2.34 | **D15** | IOT82B (bank 1) | IOR4B (bank 2) | MATCH |
| `ESP32_OPI_D4` | J2.35 | **A16** | IOT99B (bank 1) | IOR20B (bank 2) | MATCH |
| `LED D2 (R24)` | J2.36 | **D14** | IOT82A (bank 1) | IOR4A (bank 2) | MATCH |
| `ESP32_OPI_D3` | J2.37 | **A15** | IOT99A (bank 1) | IOR20A (bank 2) | MATCH |
| `SW4` | J2.40 | **F15** | IOT91A (bank 1) | IOR1A (bank 2) | MATCH |
| `ESP32_OPI_D2` | J2.41 | **B16** | IOT97B (bank 1) | IOR22B (bank 2) | MATCH |
| `SW3` | J2.42 | **F14** | IOT80B (bank 1) | IOR11B (bank 2) | MATCH |
| `ESP32_OPI_D1` | J2.43 | **B15** | IOT97A (bank 1) | IOR22A (bank 2) | MATCH |
| `SW2` | J2.44 | **F13** | IOT80A (bank 1) | IOR11A (bank 2) | MATCH |
| `ESP32_OPI_D0` | J2.47 | **A14** | IOT95B (bank 1) | IOR15B (bank 2) | MATCH |
| `SW1` | J2.48 | **J16** | IOR47A (bank 5) | IOR109A (bank 3) | MATCH |
| `ESP32_PPO_CLK` | J2.49 | **A13** | IOT95A (bank 1) | IOR15A (bank 2) | MATCH |
| `ESP32_PPO_D3` | J2.53 | **B13** | IOT93B (bank 1) | IOR17B (bank 2) | MATCH |
| `ESP32_PPO_D2` | J2.55 | **C13** | IOT93A (bank 1) | IOR17A (bank 2) | MATCH |
| `ESP32_PPO_D1` | J2.59 | **C15** | IOT85B (bank 1) | IOR8B (bank 2) | MATCH |
| `ESP32_PPO_D0` | J2.61 | **C14** | IOT85A (bank 1) | IOR8A (bank 2) | MATCH |
| `DPAUX_N` | J2.62 | **G16** | IOR24B (bank 4) | IOR105B (bank 3) | MATCH |
| `DPAUX_P` | J2.64 | **G15** | IOR24A (bank 4) | IOR105A (bank 3) | MATCH |
| `ESP32_PPO_SYNC` | J2.65 | **E14** | IOT78B (bank 1) | IOR2B (bank 2) | MATCH |
| `ESP32_TX` | J2.66 | **G13** | IOR20B (bank 4) | IOR98B (bank 3) | MATCH |
| `ESP32_INT_N` | J2.67 | **E13** | IOT78A (bank 1) | IOR2A (bank 2) | MATCH |
| `ESP32_RX` | J2.68 | **H13** | IOR20A (bank 4) | IOR98A (bank 3) | MATCH |
| `FPGA_GS_A11` | J3.3 | **Y22** | IOB99B (bank 8) | IOB131B (bank 4) | MATCH |
| `FPGA_GS_D_DIR` | J3.4 | **W21** | IOB104A (bank 7) | IOB124A (bank 4) | MATCH |
| `FPGA_GS_A12` | J3.5 | **Y21** | IOB99A (bank 8) | IOB131A (bank 4) | MATCH |
| `FPGA_GS_D7` | J3.6 | **V20** | IOB95B (bank 8) | IOB120B (bank 4) | MATCH |
| `FPGA_GS_A10` | J3.7 | **AB22** | IOB97B (bank 8) | IOB129B (bank 4) | MATCH |
| `FPGA_GS_D6` | J3.8 | **U20** | IOB95A (bank 8) | IOB120A (bank 4) | MATCH |
| `FPGA_GS_A13` | J3.9 | **AB21** | IOB97A (bank 8) | IOB129A (bank 4) | MATCH |
| `FPGA_GS_D5` | J3.10 | **M17** | IOR56A (bank 5) | IOR73A (bank 3) | MATCH |
| `FPGA_GS_A9` | J3.11 | **AA21** | IOB102B (bank 7) | IOB126B (bank 4) | MATCH |
| `FPGA_GS_D4` | J3.12 | **P17** | IOB140B (bank 6) | IOB135B (bank 4) | MATCH |
| `FPGA_GS_A14` | J3.13 | **AA20** | IOB102A (bank 7) | IOB126A (bank 4) | MATCH |
| `FPGA_GS_D3` | J3.14 | **N17** | IOB140A (bank 6) | IOB135A (bank 4) | MATCH |
| `FPGA_GS_A8` | J3.15 | **AB20** | IOB85B (bank 8) | IOB110B (bank 4) | MATCH |
| `FPGA_GS_D2` | J3.16 | **M16** | IOR52B (bank 5) | IOR67B (bank 3) | MATCH |
| `FPGA_GS_A15` | J3.17 | **AA19** | IOB85A (bank 8) | IOB110A (bank 4) | MATCH |
| `FPGA_GS_D1` | J3.18 | **M15** | IOR52A (bank 5) | IOR67A (bank 3) | MATCH |
| `FPGA_GS_A7` | J3.19 | **W20** | IOB93B (bank 8) | IOB122B (bank 4) | MATCH |
| `FPGA_GS_D0` | J3.20 | **N15** | IOB134B (bank 6) | IOB146A (bank 4) | MATCH |
| `FPGA_GS_A6` | J3.21 | **W19** | IOB93A (bank 8) | IOB122A (bank 4) | MATCH |
| `FPGA_GS_NMI` | J3.22 | **N13** | IOB138A (bank 6) | IOB142A (bank 4) | MATCH |
| `FPGA_GS_A5` | J3.23 | **AB18** | IOB77B (bank 8) | IOB108B (bank 4) | MATCH |
| `FPGA_GS_BE` | J3.24 | **N14** | IOB138B (bank 6) | IOB142B (bank 4) | MATCH |
| `FPGA_GS_A4` | J3.25 | **AA18** | IOB77A (bank 8) | IOB108A (bank 4) | MATCH |
| `FPGA_GS_IRQ` | J3.26 | **Y17** | IOB122A (bank 7) | IOB91A (bank 5) | MATCH |
| `FPGA_GS_A3` | J3.27 | **Y19** | IOB87B (bank 8) | IOB116B (bank 4) | MATCH |
| `FPGA_GS_PH2` | J3.28 | **AB17** | IOB108A (bank 7) | IOB80B (bank 5) | MATCH |
| `FPGA_GS_A2` | J3.29 | **Y18** | IOB87A (bank 8) | IOB116A (bank 4) | MATCH |
| `FPGA_GS_ABORT` | J3.30 | **AA16** | IOB106A (bank 7) | IOB78B (bank 5) | MATCH |
| `FPGA_GS_A1` | J3.31 | **W17** | IOB79B (bank 8) | IOB106B (bank 4) | MATCH |
| `FPGA_GS_A0` | J3.33 | **V17** | IOB79A (bank 8) | IOB106A (bank 4) | MATCH |
| `FPGA_GS_RW` | J3.35 | **U18** | IOB75B (bank 8) | IOB112B (bank 4) | MATCH |
| `FPGA_GS_RDY` | J3.36 | **AB16** | IOB108B (bank 7) | IOB80A (bank 5) | MATCH |
| `FPGA_GS_RESET` | J3.38 | **AA15** | IOB106B (bank 7) | IOB83A (bank 5) | MATCH |
| `FPGA_GS_DATA_~{OE}` | J3.40 | **AB15** | IOB89A (bank 8) | IOB83B (bank 5) | MATCH |
| `FPGA_GS_ADDR_~{OE}` | J3.42 | **Y16** | IOB122B (bank 7) | IOB78A (bank 5) | MATCH |
| `FPGA_GS_~{OE}` | J3.44 | **W15** | IOB126B (bank 7) | IOB72A (bank 5) | MATCH |
| `FPGA_GS_RDY_OUT` | J3.46 | **V15** | IOB130A (bank 6) | IOB68B (bank 5) | MATCH |
| `FPGA_GS_VP` | J3.48 | **W14** | IOB91A (bank 8) | IOB85A (bank 5) | MATCH |
| `LED D3 (R25)` | J3.55 | **R14** | IOB146B (bank 6) | IOB133B (bank 4) | MATCH |
| `LED D4 (R26)` | J3.57 | **P14** | IOB146A (bank 6) | IOB133A (bank 4) | MATCH |
| `DP_REFCLK_P` | J2.73 | **F10** | Q0_REFCLKP_1 | Q0_REFCLKP_1 | MATCH |
| `DP_REFCLK_N` | J2.71 | **E10** | Q0_REFCLKM_1 | Q0_REFCLKM_1 | MATCH |
| `DP0_P` | J2.77 | **C7** | Q0_LN3_TXM_O | Q0_LN1_TXM_O | **DIFFER** |
| `DP0_N` | J2.79 | **D7** | Q0_LN3_TXP_O | Q0_LN1_TXP_O | **DIFFER** |
| `DP1_P` | J2.83 | **A6** | Q0_LN2_TXM_O | Q0_LN2_TXM_O | MATCH |
| `DP1_N` | J2.85 | **B6** | Q0_LN2_TXP_O | Q0_LN2_TXP_O | MATCH |
| `DP2_P` | J2.89 | **C5** | Q0_LN1_TXM_O | Q0_LN3_TXM_O | **DIFFER** |
| `DP2_N` | J2.91 | **D5** | Q0_LN1_TXP_O | Q0_LN3_TXP_O | **DIFFER** |
| `DP3_P` | J2.95 | **A4** | Q0_LN0_TXM_O | Q0_LN0_TXM_O | MATCH |
| `DP3_N` | J2.97 | **B4** | Q0_LN0_TXP_O | Q0_LN0_TXP_O | MATCH |
| `clk (50 MHz osc)` | SOM | **V22** | IOB117B (bank 7) | IOB104B (bank 4) | MATCH |
| `button` | SOM | **AB13** | IOB54A (bank 9) | IOB89B (bank 5) | MATCH |
| `ddr_bank[2]` | SOM (DDR3) | **M6** | IOL70A (bank 10) | IOL51A (bank 7) | MATCH |
| `ddr_bank[1]` | SOM (DDR3) | **P2** | IOL68A (bank 10) | IOL47A (bank 7) | MATCH |
| `ddr_bank[0]` | SOM (DDR3) | **P5** | IOL66A (bank 10) | IOL49A (bank 7) | MATCH |
| `ddr_addr[14]` | SOM (DDR3) | **D1** | IOL27B (bank 11) | IOL4B (bank 7) | MATCH |
| `ddr_addr[13]` | SOM (DDR3) | **K1** | IOL35A (bank 11) | IOL15A (bank 7) | MATCH |
| `ddr_addr[12]` | SOM (DDR3) | **K4** | IOL48A (bank 10) | IOL29A (bank 7) | MATCH |
| `ddr_addr[11]` | SOM (DDR3) | **H3** | IOL43A (bank 11) | IOL26A (bank 7) | MATCH |
| `ddr_addr[10]` | SOM (DDR3) | **L1** | IOL52B (bank 10) | IOL35B (bank 7) | MATCH |
| `ddr_addr[9]` | SOM (DDR3) | **H5** | IOL41B (bank 11) | IOL20B (bank 7) | MATCH |
| `ddr_addr[8]` | SOM (DDR3) | **J5** | IOL41A (bank 11) | IOL20A (bank 7) | MATCH |
| `ddr_addr[7]` | SOM (DDR3) | **J1** | IOL35B (bank 11) | IOL15B (bank 7) | MATCH |
| `ddr_addr[6]` | SOM (DDR3) | **G3** | IOL43B (bank 11) | IOL26B (bank 7) | MATCH |
| `ddr_addr[5]` | SOM (DDR3) | **H2** | IOL37A (bank 11) | IOL17A (bank 7) | MATCH |
| `ddr_addr[4]` | SOM (DDR3) | **J2** | IOL39B (bank 11) | IOL22B (bank 7) | MATCH |
| `ddr_addr[3]` | SOM (DDR3) | **J4** | IOL48B (bank 10) | IOL29B (bank 7) | MATCH |
| `ddr_addr[2]` | SOM (DDR3) | **G2** | IOL37B (bank 11) | IOL17B (bank 7) | MATCH |
| `ddr_addr[1]` | SOM (DDR3) | **K2** | IOL39A (bank 11) | IOL22A (bank 7) | MATCH |
| `ddr_addr[0]` | SOM (DDR3) | **M1** | IOL52A (bank 10) | IOL35A (bank 7) | MATCH |
| `ddr_odt` | SOM (DDR3) | **M2** | IOL54B (bank 10) | IOL33B (bank 7) | MATCH |
| `ddr_cke` | SOM (DDR3) | **K6** | IOL57A (bank 10) | IOL38A (bank 7) | MATCH |
| `ddr_we` | SOM (DDR3) | **M5** | IOL70B (bank 10) | IOL51B (bank 7) | MATCH |
| `ddr_cas` | SOM (DDR3) | **L4** | IOL59B (bank 10) | IOL40B (bank 7) | MATCH |
| `ddr_ras` | SOM (DDR3) | **L5** | IOL59A (bank 10) | IOL40A (bank 7) | MATCH |
| `ddr_cs` | SOM (DDR3) | **P4** | IOL66B (bank 10) | IOL49B (bank 7) | MATCH |
| `ddr_reset_n` | SOM (DDR3) | **L6** | IOL65A (bank 10) | IOL55A (bank 7) | MATCH |
| `ddr_ck` | SOM (DDR3) | **L3** | IOL50A (bank 10) | IOL31A (bank 7) | MATCH |
| `ddr_ck` | SOM (DDR3) | **K3** | IOL50B (bank 10) | IOL31B (bank 7) | MATCH |
| `ddr_dm[1]` | SOM (DDR3) | **V7** | IOB58A (bank 9) | IOL107A (bank 6) | MATCH |
| `ddr_dm[0]` | SOM (DDR3) | **AA4** | IOB31B (bank 9) | IOL78B (bank 6) | MATCH |
| `ddr_dq[15]` | SOM (DDR3) | **Y9** | IOB69B (bank 9) | IOL105B (bank 6) | MATCH |
| `ddr_dq[14]` | SOM (DDR3) | **AB6** | IOB60B (bank 9) | IOL98B (bank 6) | MATCH |
| `ddr_dq[13]` | SOM (DDR3) | **W9** | IOB69A (bank 9) | IOL105A (bank 6) | MATCH |
| `ddr_dq[12]` | SOM (DDR3) | **AB8** | IOB65B (bank 9) | IOL101B (bank 6) | MATCH |
| `ddr_dq[11]` | SOM (DDR3) | **Y7** | IOB67B (bank 9) | IOL103B (bank 6) | MATCH |
| `ddr_dq[10]` | SOM (DDR3) | **AB7** | IOB60A (bank 9) | IOL98A (bank 6) | MATCH |
| `ddr_dq[9]` | SOM (DDR3) | **Y8** | IOB67A (bank 9) | IOL103A (bank 6) | MATCH |
| `ddr_dq[8]` | SOM (DDR3) | **AA8** | IOB65A (bank 9) | IOL101A (bank 6) | MATCH |
| `ddr_dq[7]` | SOM (DDR3) | **AB1** | IOB22B (bank 9) | IOL71B (bank 6) | MATCH |
| `ddr_dq[6]` | SOM (DDR3) | **AB5** | IOB29B (bank 9) | IOL76B (bank 6) | MATCH |
| `ddr_dq[5]` | SOM (DDR3) | **AB2** | IOB24B (bank 9) | IOL74B (bank 6) | MATCH |
| `ddr_dq[4]` | SOM (DDR3) | **AA1** | IOB22A (bank 9) | IOL71A (bank 6) | MATCH |
| `ddr_dq[3]` | SOM (DDR3) | **V4** | IOB33A (bank 9) | IOL80A (bank 6) | MATCH |
| `ddr_dq[2]` | SOM (DDR3) | **AA5** | IOB29A (bank 9) | IOL76A (bank 6) | MATCH |
| `ddr_dq[1]` | SOM (DDR3) | **AB3** | IOB24A (bank 9) | IOL74A (bank 6) | MATCH |
| `ddr_dq[0]` | SOM (DDR3) | **Y4** | IOB31A (bank 9) | IOL78A (bank 6) | MATCH |
| `ddr_dqs[1]` | SOM (DDR3) | **V9** | IOB62A (bank 9) | IOL96A (bank 6) | MATCH |
| `ddr_dqs[1]` | SOM (DDR3) | **V8** | IOB62B (bank 9) | IOL96B (bank 6) | MATCH |
| `ddr_dqs[0]` | SOM (DDR3) | **Y3** | IOB26A (bank 9) | IOL69A (bank 6) | MATCH |
| `ddr_dqs[0]` | SOM (DDR3) | **AA3** | IOB26B (bank 9) | IOL69B (bank 6) | MATCH |

---

## Provenance

| | |
|---|---|
| Base commit | `a72d0fe9` (tip of `claude/epic-lovelace-54ee14`) |
| Package data | Gowin IDE **V1.9.12.03** bundle, `IDE/data/device/{GW5AT-60B,GW5AST-138B}/PBGA484A.json` |
| Carrier pin data | `docs/a2mega_pinmap_1_0a3.csv` (a2-mega repo branch `1.0a3`, HEAD `273d673`) + `hdl/a2mega.cst` / `hdl/a2mega_138b.cst` |
| Audit date | 2026-08-20 |
| Scope | 1.0a3 routing. **Re-run for 1.0a4** once HyperRAM lands and the GS nets move — regenerate the pin map with `boards/a2mega/tools/btb_pinmap.py`, then re-run this join. The die-vs-die conclusions (§2, §3) are properties of the two silicon dies and do not change with board revision. |
