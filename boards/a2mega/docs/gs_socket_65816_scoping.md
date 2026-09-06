# 65C816 in-socket replacement — scoping (a2mega 1.0a3, GW5AST-138B)

**Status: scoping document, 2026-09-05. No HDL written yet.** This is the
Phase C material of `PRE_REV_EXERCISE_PLAN.md` (C0 circuit review + C1 RTL
inventory + the sim/bench plan), gathered so the first iteration can be
decided before any RTL work starts.

**Ask (ed, 09-05):** start the 65816 scoping; 138B only until the logic and
BSRAM footprint is known; this board rev has no HyperRAM (the next rev
will); the first iteration is a *drop-in replacement* for the IIgs's own
65C816, to prove the socket circuit is electrically correct. Acceleration,
cache and the HyperRAM backing store are explicitly out of scope here (they
are the TransWarp endgame in `hyperram_scenarios.md`).

## 0. Summary

- **The socket circuit exists on 1.0a3 and is complete for a drop-in.** All
  38 `FPGA_GS_*` nets are on the J3 board-to-board connector, on balls the
  138B design does not use, behind proper dual-supply level shifters
  (74ALVC164245, 3.3 V ↔ 5 V) with the correct directions and with every
  enable pulled to "disabled" while the FPGA is unconfigured. Signal-by-
  signal review in §2. The ribbon interposer exists and its 1:1 mapping
  checks out pin for pin (§2.1). One optional note for 1.0a4 (§6).
- **The CPU core exists and is verified elsewhere.** `hdl/twgs/65C816/` is
  the P65C816 from Apple-IIgs_MiSTer (srg320's SNES core, IIgs-adapted by
  Alan Steremberg), byte-identical to the MiSTer tree apart from the license
  header; that project runs the IIgs ROM self-tests and boots GS/OS on it.
  Our `iigs_65816_wrapper.sv` is a first-draft phase machine that has never
  been built; it needs the redesign in §4 (it is clocked at 54 MHz through
  a synchronizer, which cannot meet the socket timing).
- **Footprint, measured (GowinSynthesis, 138B, all ports exposed so nothing
  is pruned):**

  | Resource | P65C816 alone | 138B full core today | After (est.) |
  |---|---|---|---|
  | Logic (LUT+ALU) | 1 912 / 138 240 (1.4 %) | 25 774 (19 %) | ≈ 28 500 (21 %) |
  | Registers | 262 | 23 813 | ≈ 24 500 |
  | BSRAM | 6 / 340 (microcode ROM, pROMX9) | 121 / 340 | ≈ 127–131 |

  The socket PHY, CE generator and telemetry add an estimated 500–800 LUT
  and 0–4 BSRAM (a bus-trace ring buffer is the only BSRAM consumer, and it
  is optional). This is small on either die: on paper it also fits the 60K
  (BSRAM 108 → 114 of 118, no trace buffer), so the 138B-only decision is a
  focus decision, not a capacity one. The core's synthesized single-cycle
  Fmax is 52 MHz (18 logic levels on the microcode→P path) — relevant to
  the clock choice in §4.
- **The hard part is timing, not size.** The IIgs FPI gives the CPU a
  ~140 ns PHI1 (clock-low) phase in fast mode; the address and the bank byte
  must be on the bus well inside it, and read data must be captured within
  ~20 ns of the PHI2 falling edge. That forces two design rules (§4):
  the bus pins run in a **PH2-derived clock domain** (input capture on the
  PH2 falling edge, output enables driven from PH2 directly), and the core
  runs on a **~100 MHz clock with a clock enable**, not on clk_logic 54.
  `FPGA_GS_PH2` sits on a non-clock ball (AB17, plain I/O on both dies),
  so on 1.0a3 PH2 reaches the clock network through general routing; the
  1.0a4 fix is a carrier net swap onto a clock-capable ball on the same
  connector, W19/W20 preferred since it is a clock pair on both dies (§6).
- **Recommendation:** proceed with iteration 1 as a pure drop-in — no
  cache, no speed control, no bus mastering — on the architecture in §4,
  sim-first per §7, then the C3→C5 bench ladder. Open questions that need
  ed before RTL: §8.

## 1. What "drop-in" means here

The card is in a slot AND plugged into the empty CPU socket via a ribbon
(there is never a socketed CPU; ed, 08-20). The motherboard therefore sees:

- our address (A0–15), bank byte (D0–7 during PHI1), R/W, data on writes,
  VP — exactly as a 65C816 would drive them, at the FPI's PHI2 pace,
  including the FPI's stretched (sync) cycles and refresh cycles;
- us honoring its inputs: PHI2 (the only clock), RDY, /IRQ, /NMI, /RES,
  /ABORT, BE.

Everything else the card already does on the slot side (DP video of the
Apple II screen, Ensoniq, OSD, disk/HDD, Uthernet) keeps working unchanged:
our own CPU's slow-side cycles appear on the slot bus like any CPU's.

Out of scope for iteration 1: TransWarp acceleration, caching, `$C074` /
ZipGS registers, DMA snooping beyond honoring BE, E/M/X/VDA/VPA outputs
(not wired on the card, see §2), HyperRAM.

## 2. Hardware inventory — the GS socket circuit on 1.0a3 (C0 review)

Source of truth: the a2-mega KiCad netlist (sheet "GS 65816 Connector",
`gs.kicad_sch`; pad-to-net extracted from the production PCB file for the
2026-07-26 order). Refdes below are the schematic's (U13/U14/U15/U16; the
older PDF export shows U8/U10/U11 for the same parts).

### 2.1 Header J5 (40-pin IDC, 2.54 mm) — what the ribbon carries

**Interposer (ed, 09-05): exists.** It is an off-the-shelf 40-way ribbon
with a crimp IDC header on the card end and a crimp DIP-40 plug on the
motherboard end, deliberately a plain 1:1 cable. A standard IDC→DIP ribbon
maps header odd pins down one side of the DIP and even pins up the other
(IDC 2k−1 → DIP k, IDC 2k → DIP 41−k). Applied to J5 that lands every net
on its W65C816 pin — J5 was laid out for exactly this cable:

| J5 pin | DIP-40 pin | 65C816 signal | Net | Card side | Notes |
|---|---|---|---|---|---|
| 1 | 1 | VPB | GS_VP | U15 open-drain out + 549 Ω to +5 V | output to motherboard |
| 2 | 40 | RESB | GS_RESET | U13 port-1 input | motherboard /RES, input to us |
| 3 | 2 | RDY | GS_RDY | U13 port-1 input **and** U15 open-drain out | bidirectional per 65816 |
| 4 | 39 | VDA | — | **not connected** | |
| 5 | 3 | ABORTB | GS_ABORT | U13 port-1 input | |
| 6 | 38 | M/X | — | **not connected** | |
| 7 | 4 | IRQB | GS_IRQ | U13 port-1 input | |
| 8 | 37 | PHI2 | GS_PH2 | 27 Ω → U13 port-1 → 27 Ω → FPGA | the bus clock |
| 9 | 5 | MLB | — | **not connected** | |
| 10 | 36 | BE | GS_BE | U13 port-1 input | |
| 11 | 6 | NMIB | GS_NMI | U13 port-1 input | |
| 12 | 35 | E | — | **not connected** | |
| 13 | 7 | VPA | — | **not connected** | |
| 14 | 34 | RWB | GS_RW | U16 SN74LV1T125 output | 3.3→5 V buffer, OE = ADDR_~OE |
| 15 | 8 | VDD | BUS_5V | slot +5 V rail, upstream of the LM74700 | socket VDD = the motherboard's own 5 V rail; see F2 |
| 16,18,…,30 | 33,32,…,26 | D0–D7 | GS_D0–D7 | 47 Ω series → U13 port-2 | bidirectional |
| 17,19,…,39 | 9,10,…,20 | A0–A11 | GS_A0–A11 | 75 Ω series → U14 | output only |
| 32,34,36,38 | 25,24,23,22 | A15,A14,A13,A12 | GS_A15–A12 | 75 Ω series → U14 | output only |
| 40 | 21 | VSS | GND | | the ribbon's only ground return |

Signal-integrity note: one ground conductor for 39 signals is the classic
ribbon crosstalk case, but it is also exactly what the TransWarp GS cable
was, the edges are series-terminated (75/47/27 Ω), and the bus is
2.86 MHz. Keep the cable short; C3/C4 look at PH2 and D0 with a scope.

### 2.2 Level shifters and enables

- **U13 (74ALVC164245, VCCA 3.3 V / VCCB 5 V)**
  - Port 1 = control inputs, `1DIR` = GND → 5 V side → 3.3 V side only.
    1B0 GS_VP (its A-side pin 47 is left unconnected — VP is not read
    back), 1B1 RESET, 1B2 RDY, 1B3 ABORT, 1B4 PH2, 1B5 IRQ, 1B6 BE, 1B7 NMI
    → `FPGA_GS_RESET/RDY/ABORT/PH2/IRQ/BE/NMI`. Enable `~1OE` =
    `FPGA_GS_~OE`, **pulled up 4.7 kΩ** (disabled when unconfigured).
  - Port 2 = data D0–7, `2DIR` = `FPGA_GS_D_DIR` (1 = we drive the bus),
    `~2OE` = `FPGA_GS_DATA_~OE`, **pulled up 4.7 kΩ**.
- **U14 (74ALVC164245)** = address A0–15, both `DIR` pins tied to 3.3 V
  (FPGA → bus only), both `~OE` = `FPGA_GS_ADDR_~OE`, **pulled up 4.7 kΩ**.
- **U16 (SN74LV1T125, VCC 5 V)** = R/W up-translator; `~OE` =
  `FPGA_GS_ADDR_~OE`, so BE-style tri-state of A0–15 and R/W is one signal,
  matching the 65816's own BE grouping ("RWB and address buffers").
- **U15 (74LVC2G07 dual open-drain, VCC 3.3 V, 5.5 V-tolerant outputs)**
  - 1A `FPGA_GS_RDY_OUT` → 1Y GS_RDY: wired-OR onto the 5 V RDY line, as the
    65816 datasheet allows ("RDY can be wire-ORed"). `FPGA_GS_RDY_OUT`
    **pulled up 4.7 kΩ** → released when unconfigured.
  - 2A `FPGA_GS_VP` → 2Y GS_VP with 549 Ω to +5 V (≈9 mA sink when low,
    τ ≈ 30 ns with ~50 pF). Not pulled on the FPGA side: during
    configuration VP follows the GW5A's default weak pull-up (high =
    inactive). Harmless either way with no CPU running.
- **Series terminations:** 75 Ω on every address line, 47 Ω on data, 27 Ω at
  each end of PH2. Fine at 2.8 MHz edges over a short ribbon.

### 2.3 Unconfigured / configuring / configured-but-idle states

All three transceiver enables and RDY_OUT are pulled to their inactive
state, so an unconfigured or half-alive card drives nothing into the
socket; the motherboard sees a floating CPU (harmless: the Mega II keeps
generating video and refresh from a garbage screen). Once configured, the
gateware must keep the enables inactive until the socket core is armed
(a DIP/telnet switch; default OFF so a card without a ribbon behaves as
today). The existing slot-side reset choreography already gives the start
sequence we need: the ESP32 holds the Apple II /RES via
`a2_res_out_n` until its mounts are done (OSPI reg 0x2E, backstop
counter), then releases — our core then takes its reset vector cleanly.
Nothing new is required on the reset path.

### 2.4 Balls on the 138B

All 38 `FPGA_GS_*` balls are free in `a2mega_138b.cst` (checked ball by
ball). Address on bank 4 (LVCMOS33 like the slot bus), data mixed bank
3/4, control on bank 5 — all 3.3 V banks already in use. I/O count after
adding them: 158 of 297.

`FPGA_GS_PH2` = **AB17 = IOB80B, bank 5, plain I/O — not a clock-capable
ball** (per the Gowin PG484 pin table for the GW5AST-138B). Several other
GS nets landed on clock-capable balls by accident: `FPGA_GS_RDY_OUT` V15
(SGCLKC_7), `FPGA_GS_D6/D7` U20/V20 (SGCLK_5), `FPGA_GS_A6/A7` W19/W20
(MGCLK_5), `FPGA_GS_A2/A3` Y18/Y19 (MGCLK_4). See §6 for the 1.0a4 fix and
§4.4 for what we do on 1.0a3.

## 3. The electrical contract

### 3.1 W65C816S timing (datasheet, 5 V column and the 4 MHz-grade column)

The IIgs runs its CPU at 2.86 MHz from a 4 MHz-grade part. The motherboard
was therefore designed to tolerate at least the 4 MHz-grade output delays
(right column); the 14 MHz-grade numbers (left) are what a modern socketed
W65C816S-14 would deliver.

| Symbol | Parameter | 14 MHz grade | 4 MHz grade | Who it binds |
|---|---|---|---|---|
| tPWL / tPWH | PHI2 low / high, min | 35 / 35 ns | 125 / 125 ns | FPI (2 / 3 ticks of 14.318 MHz in fast mode ≈ 140 / 210 ns) |
| tADS | A0–15 valid after PHI2 fall, max | 30 ns | 75 ns | **us → FPI** |
| tBAS | bank byte on D0–7 valid after fall, max | 33 ns | 75 ns | **us → FPI** |
| tAH | A0–15 hold after fall, min | 10 ns | 20 ns | us |
| tBH | bank byte hold after PHI2 rise, min | 10 ns | 20 ns | us (FPI latches bank on the rise) |
| tMDS | write data valid after rise, max | 30 ns | 70 ns | us |
| tDHW | write data hold after fall, min | 10 ns | 20 ns | us |
| tDSR / tDHR | read data setup before / hold after fall, min | 10 / 10 ns | 30 / 20 ns | **motherboard → us** |
| tPCS / tPCH | IRQ/NMI/RES/RDY/ABORT setup before / hold after fall | 10 / 10 ns | 30 / 20 ns | motherboard → us (sample at the fall) |
| tBVD | BE to valid data / Hi-Z | 25 ns | 60 ns | us |

Datasheet notes that matter: RDY is sampled such that a low level "halts
the microprocessor in its current state" and execution resumes "following
the next PHI2 negative transition"; while RDY is low the data bus stays in
the data-transfer state and the external bank latch must be latched on the
PHI2 rising edge (§7.6); the W65C816S **does** stop during writes (§7.17,
unlike the NMOS 6502); BE is asynchronous.

### 3.2 IIgs specifics (Hardware Reference + the MiSTer FPI model)

- **PHI2 comes from the FPI** in every mode; the CPU never sees a free-
  running clock. Fast cycle = 5 ticks of 14.318 MHz (349 ns). Every 9th
  fast RAM cycle is a 10-tick refresh cycle (ROM accesses hide refresh).
  Every 65th cycle carries the NTSC line stretch (+2 ticks).
- **Slow / sync cycles are made by holding PHI2 HIGH:** "the FPI holds the
  02 clock high until it synchronizes itself with the Mega II" (HW Ref).
  So the low phase is always the short one (~140 ns), the high phase is
  what stretches (up to ~27 ticks). Consequence for us: the falling edge is
  the unpredictable "commit" event, the rising edge is regular.
- The FPI decides slow-vs-fast per cycle from the address and bank byte it
  sees during PHI1 — which is why the bank byte must be on D0–7 early in
  the low phase, not just "before the rise".
- Reset clears the Speed register: the reset-vector fetch runs at 1 MHz.
- RDY: honoured by the FPI only at Normal speed per the TWGS manual; we
  honor it always (safe superset). We do **not** need to drive RDY_OUT in
  iteration 1 (WAI can be internal); recommendation: leave it released.
- E, M/X, VDA, VPA, MLB: not connected on the card. Whether the FPI uses
  VDA/VPA is not stated in the Hardware Reference text; the motherboard
  works with a real 65C816 whose VDA/VPA are wired somewhere, so C3 should
  look at the socket pins 7/39 with a meter (open = confirmed unused).
  VP **is** wired and driven by us.

## 4. Timing budget → architecture (as built in S2)

### 4.1 The two critical chains

**(a) PHI2 fall → next address + bank byte on the bus.** Path: U13 port-1
(ALVC164245 tpd ≈ 4–6 ns) → FPGA input (≈2 ns) → the core computes the
next address → output register → FPGA output (≈3 ns) → U14 (≈4–6 ns) →
75 Ω into the bus (≈4 ns). Budget: 4 MHz-grade tADS/tBAS = 75 ns; the
FPI's low phase is ~140 ns.

**(b) Read data capture at the PHI2 fall.** The motherboard guarantees
data only until tDHR after the fall (20 ns at the 4 MHz grade; possibly
much more in practice, unknown). Anything that first synchronises the
edge into a sampling clock and then captures is already late.

**What S2 found.** The first draft (core in a 108 MHz clock-enable domain,
fall synchronised over, then CE → microcode ROM → address logic → output
register) measured 85–100 ns fall-to-address on the bench: the two-flop
edge synchroniser plus three serialized core clocks eat the budget
regardless of how fast the sequencer clock is. So the S2 design does what
the real part does:

- **The core is clocked by PHI2.** Its clock is the inverted PH2 pin, so
  its active edge is the PHI2 falling edge. It samples D0–7, RDY, /IRQ,
  /NMI and /ABORT straight from the pads at the 65816's own sample
  instant (no capture registers, no synchroniser), and its state — hence
  A0–23, RWB, VPB, D_OUT — is settled ~20 ns later. CE is tied high: the
  core takes one clock per bus cycle (its microcode ROM is read from the
  next state), and RDY_IN from the pad freezes it at the fall exactly as
  the datasheet says. Every core path has a 349 ns period, so there is
  nothing to close and no multicycle constraint at all.
- **The pins are sequenced in a ~108 MHz domain** (the PLL output ed
  chose; it is now the *sequencer* clock, the core no longer needs it). A
  toggle written at the fall is two-flop synchronised across; two clocks
  later the core's settled outputs are registered out (A0–15, RWB, VPB,
  bank byte on D0–7), and the rise (two-flop synchronised) drives the
  bank-hold / write-data / bus-release sequence of §4.2. The core's
  outputs are quasi-static for a whole cycle, so this crossing is safe by
  construction provided they have settled before the sample: the design
  samples ≥ 37 ns after the pad fall against a ~20 ns settle.

Measured at the socket side by the Sim 1 bench (transceiver delays 5 ns
each way, enables 6 ns), worst case over ~6 000 cycles, fast and 1 MHz
modes identical:

| Parameter | Limit (4 MHz grade) | Measured |
|---|---|---|
| tADS address valid after fall | ≤ 75 ns | 66 ns |
| tBAS bank byte valid after fall | ≤ 75 ns | 67 ns |
| tAH address hold after fall | ≥ 20 ns | 56 ns |
| tBH bank hold after rise | ≥ 20 ns | 40 ns |
| tMDS write data valid after rise | ≤ 70 ns | 56 ns |
| tDHW write data hold after fall | ≥ 20 ns | 56 ns |
| read: bus released after rise | before any memory drive | 57 ns |
| read data hold needed after fall | ≥ 20 ns available | sampled at the fall (+5 ns shifter) |

tADS is the one with the least margin; it is set by the two-flop
synchroniser (18–28 ns) plus three sequencer clocks (28 ns) plus the two
shifters. It can be pulled in ~9 ns by sampling one clock earlier
(OUT_DELAY 2 → 1) at the cost of settle margin, or by a faster sequencer
clock; the address-delay sweep instrument in C4 will tell whether the FPI
cares before anything is changed.

### 4.2 The turnaround at the PHI2 rise

At the rise the bank byte must stay ≥ tBH (10–20 ns) and then, for a read,
we must get off the bus before the FPI/RAM/ROM drives it; for a write we
must present data within tMDS (70 ns). The sequencer sees the rise through
a two-flop sync (~10–20 ns), holds one more clock, then: writes switch the
byte from bank to data (56 ns after the rise measured); reads release the
bus — shifter off, pads tri-state, direction turned, receive on — done
~57 ns after the rise, long before any memory can drive (the model drives
at 100 ns). Write data holds until the next cycle's bank byte (~56 ns
after the next fall), satisfying tDHW. A cycle whose RDY was low at the
fall is repeated with the data bus left in its data-transfer state and no
bank byte, per W65C816S §7.6.

### 4.3 What the socket PHY looks like (`hdl/twgs/gs_socket_phy.sv`)

```
PH2 pin ──┬─► ~PH2 = core clock: P65C816 (unchanged, CE=1, RDY_IN/D_IN/IRQ/NMI/ABORT from the pads)
          ├─► negedge regs: /RES sample (two falls high → core reset released), RDY sample, cycle toggle
          └─► two-flop syncs into the sequencer: fall toggle, rise level, BE, armed
~108 MHz sequencer ──► fall + 2 clk: register A[15:0], RWB, VPB, bank byte; DIR/OE for the low phase
                   ──► rise + hold: write data or bus release (OE off → tri-state → DIR → OE on)
                   ──► enables: ADDR_~OE (armed & alive & BE), DATA_~OE, CTL_~OE (armed), RDY_OUT released
                   ──► telemetry: PH2 alive, running, cycle / stall / BE counters
```

Gating: `armed` (DIP / telnet) ANDed into every enable; nothing is driven
until PH2 has been seen toggling; BE low takes the address/RWB/data
shifters and the D pads off within tBVD (measured well inside 60 ns) while
the core keeps running, as a real 65816 does; /RES low holds the core in
reset with the address bus still driven and RWB high.

The socket SDC declares `gs_ph2` as a 349 ns clock (fast cycle; sync and
slow cycles only add slack), the sequencer clock as a separate
asynchronous group, and the 0.5 ns uncertainty policy on the sequencer
clock. The core-output → sequencer sampling margin is a designed CDC that
STA cannot see; it is stated in the module header and in the SDC comments.

**138B PnR probe (S2, 2026-09-05).** gs_socket_phy + P65C816 placed on
the real GS balls with `hdl/twgs/gs_socket.sdc` and
`hdl/twgs/gs_socket_pins_138b.cst` (sequencer clock taken from a pin for
the probe): 0 setup / 0 hold violations; sequencer clock +1.47 ns
reported under the 0.5 ns policy (Fmax 128 MHz, 9 levels); gs_ph2 domain
+322 ns (Fmax 37 MHz, 20 levels — that figure also bounds the core's
output settle at ~27 ns, which is the number the OUT_DELAY = 2 sample
point at ≥ 37 ns is measured against). Footprint 2 095 logic, 398
registers, 6 BSRAM, 44 I/O. PnR notes, as predicted, "Generic routing
resource will be used to clock signal gs_ph2". Synthesis also reports a
latch inferred in the core's BCD adder (`sum2low[8]`, inherited from the
MiSTer source) — harmless at 2.86 MHz but worth cleaning before the
timing campaign bar is applied to this domain (follow-up).

### 4.4 PH2 on a non-clock ball (1.0a3)

Gowin PnR routes a general I/O onto the clock network through the fabric
("clock routed via general routing"). At 2.86 MHz the added insertion
delay and skew (a few ns) are irrelevant to the core; they slightly delay
the sample instant, which only helps read-data hold. This is the 1.0a3
approach; the optional 1.0a4 note is §6 F1.

## 5. RTL inventory (C1)

- `hdl/twgs/65C816/` — P65C816 (GPL-3.0-only, srg320 / Alan Steremberg),
  identical to Apple-IIgs_MiSTer `rtl/65C816/` except the license header.
  Interface: `CLK`, `CE` (one pulse per bus cycle), `RDY_IN` (stall),
  `NMI_N/IRQ_N/ABORT_N`, `D_IN` sampled on the CE edge, `A_OUT[23:0]`,
  `D_OUT`, `WE`, `RDY_OUT (=EN)`, `VPA/VDA/MLB/VPB`, `I_FLAG`. All state
  advances on `EN = RDY_IN & CE & ~WAI & ~STP`; microcode is a synchronous
  ROM (6 BSRAM). The MiSTer top drives it with `CE = ph2_en` (one 14 MHz
  tick per cycle) and `RDY_IN = ~dma & ~stall` — exactly the contract we
  need, so the core is used **unchanged**.
- `hdl/twgs/iigs_65816_wrapper.sv` — draft, never built, superseded by §4
  (keep the port shape; replace the state machine). Its bank-byte-on-D0–7
  idea and RDY wait state are right; its clocking is not.
- License: the core is GPL-3.0-only. Already accounted for — the root
  README lists the 65C816 core among the GPL cores and states that every
  board's bitstream is a combined work conveyed under GPL v3 (issue #54);
  the per-directory LICENSE is in place. No change.

## 6. Findings for `board_1_0a4_requirements.md`

- **F1 (optional) — move `FPGA_GS_PH2` to a clock-capable ball** (carrier
  routing only; the SOM is untouched). **Not required** (ed, 09-05): the
  socket never runs faster than the FPI's 2.86 MHz PHI2, so the fabric-
  routed clock on 1.0a3 is adequate; do this only if 1.0a4 is reshuffling
  GS nets anyway. AB17 is a plain I/O on both dies. Options, checked
  against the Gowin pin tables for both dies:
  - swap nets with `FPGA_GS_RDY_OUT` (J3-46 → V15): SGCLKC_7 on the 138B
    but plain I/O on the 60K — helps one die only;
  - swap nets with `FPGA_GS_A6` or `A7` (J3-21/19 → W19/W20): a global
    clock pair on **both** dies (MGCLK_5 + BPLL2/3 input on the 138B,
    GCLK_9 on the 60K), and PLL-input capable should a PLL ever lock to a
    PH2-derived clock — **preferred**. The displaced address bit goes to
    AB17; address lines have no use for a clock ball.
  RDY_OUT is a slow open-drain output and A6/A7 are plain outputs, so
  neither swap costs anything. Decide at the 1.0a4 pin shuffle ("1.0a4
  moves GS nets again", btb pinout notes).
- **F2 — resolved, no change.** J5 pin 15 is DIP pin 8 = VDD, i.e. the
  motherboard's own 5 V at the socket, tied to BUS_5V which is that same
  rail arriving via the slot fingers, upstream of the LM74700. No current
  flows in normal use; with the card USB-powered and the machine off the
  ideal diode still blocks any backfeed into the socket. Correct as built.
- **F3 (optional)** — bring VDA/VPA to the FPGA if C3 shows the motherboard
  actually loads them; today they are header-only.

## 7. Verification plan (sim first, then C3→C5)

**Sim 1 — socket PHY against a behavioural IIgs bus — DONE in S2
(`hdl/twgs/sim/tb_gs_socket.sv`, `run_gs_socket_sim.sh`, Verilator 5
with `--timing`; Icarus cannot parse the core's typed localparams).**
The bench models, at the socket side: the FPI clock (2 ticks low at
14.318 MHz; 5-tick fast cycle; every 9th RAM cycle a 10-tick refresh, ROM
hides refresh; 65th PH0 stretch; slow-class accesses — bank E0/E1, $C0xx —
become sync cycles held high to the PH0 grid; Speed register $C036 bit 7
= 0 runs on the PH0 grid at 1.023 MHz; power-up in slow mode), a 16 MB
memory that latches the bank at the rise, drives read data 100 ns after
the rise, holds it 20 ns after the fall and then drives garbage for
15 ns (a late sampler is caught), latches writes at the fall, and the
level shifters (5 ns data, 6 ns enable, both ways). Checks against the
4 MHz-grade W65C816S limits every cycle: tAH, tADS, tBAS, tBH, tMDS, tDHW,
no bus contention, the bank/address/RWB the FPI latched equal the core's,
read data at the core's sample instant equals what memory drove, RDY-low
cycles repeat the same cycle with no advance, BE-low tri-states within
tBVD, VP low on vector fetches; plus a hand-assembled program (writes,
RMW, JSR/RTS, an I/O read and a long write/read to bank E1 to force sync
cycles, a RAM-resident loop to force refresh cycles, CLI + IRQ handler)
whose results are checked in memory. Two configurations pass — fast mode
(6 000 cycles, 40 RDY stalls, 3 sync, 657 refresh, IRQ vector fetch, BE
test) and `+noturbo` 1 MHz throughout — with the socket timing in §4.1.
Run: `boards/a2mega/hdl/twgs/sim/run_gs_socket_sim.sh [+noturbo]
[+trace] [+vcd]`.

**Sim 2 — execution (S3) — DONE 2026-09-05, as a mode of the same bench
(`ROM=<boot1.rom> run_gs_socket_sim.sh`).** The plan was a trace match
against the MiSTer Verilator build; that cannot work past the first I/O
read, because MiSTer models the whole machine and this bench does not. The
bench instead carries a **lockstep reference**: a second instance of the
same core, same clock/reset/RDY/interrupts, fed from an ideal copy of the
memory model directly. Every cycle it compares address, RWB, VPB, write
data and the read byte the socket-side core actually sampled against the
reference, so any byte or address the PHY gets wrong is a divergence
within one cycle — over real ROM code with its real instruction mix, bank
FF/E1 accesses and sync cycles interleaved with fast ones. The ROM 01
image (built locally by the MiSTer `roms/Makefile` from a MAME
`apple2gs.zip`; Apple ROM content is never committed) sits in banks FE/FF
behind the FPI's reset-state map (banks 00/01/E0/E1 $C100–$FFFF read the
bank-FF image; $C0xx is a write-back register file with a minimal
handshake so the ROM's init code progresses). Periodic RDY stalls and one
DMA-style BE+RDY pause are applied during the run. Result: **2 000 000 socket cycles, zero divergences** (2 006 RDY
stalls, 220 951 sync cycles, 1 047 distinct opcode addresses, 1 156
text-page writes — the ROM initialised the screen — before parking in a
hardware wait loop at FF:A5A0 — `LDA $C019 / BPL`, the vertical-blank poll, which needs the VGC); socket timing identical to §4.1.
The ROM eventually parks in a hardware wait (no Mega II / ADB / VGC
behind it); how far it gets is reported as distinct opcode addresses and
text-page writes, which is also a preview of what C4 will show before the
real hardware answers.

**Bench (Phase C ladder, unchanged from the plan):**
- C3 listen-only bitstream: enables inactive, telemetry of PH2 frequency
  and duty (expect 2.86 MHz, 40 % low), RDY/IRQ/NMI/RES levels and edges,
  BE level; meter check of socket pins 7/39 (VDA/VPA) and the RDY drive
  topology (pull-up vs push-pull — decides whether RDY_OUT may ever be
  used).
- C4 first drive, with two instruments built in: a programmable **address
  delay sweep** (issue A/bank N × 9 ns late and find the FPI's real
  tolerance — the only way to learn the motherboard's true tADS budget)
  and a **data-hold sweep** (re-sample D0–7 at +9 ns steps after the fall
  and log where it changes). Then vector fetch → ROM startup → self-test →
  chime and text → ProDOS/Finder, with the DP video + telnet trace ring as
  the debug surface.
- C5 soak at stock timing with IRQ/VBL/ADB alive and disk boot; the
  board-turn bar is "every socket signal exercised at speed, both
  directions" — not compatibility polish.

Every hardware result gets a `test_log.md` row; builds carry provenance.

## 8. Open questions for ed (before RTL)

1. ~~**Interposer hardware**~~ — **answered 09-05:** exists, plain 1:1
   IDC→DIP ribbon; mapping verified in §2.1.
2. ~~**Test machine**~~ — **decided 09-05: plan with the ROM 01 machine.**
   Sim 2 therefore uses the ROM 01 image (MiSTer `boot1.rom`, 128 KB);
   the bench ladder runs on the ROM 01 GS at stock (any TWGS out of the
   socket).
3. ~~**Core clock**~~ — **decided 09-05: a new ~108 MHz PLL output** (also
   the future HyperRAM spine). The CE generator asserts CE at most every
   other clock so the core's ~52 MHz single-cycle paths are 2-clock paths
   by construction; that is stated as a multicycle constraint on the
   CE-gated registers.
4. ~~**GPL-3** consequence~~ — **closed 09-05:** already the project's
   stated position (README licensing section); nothing new.
5. ~~**1.0a4:** put F1 on the list now?~~ — **closed 09-05:** optional,
   only if 1.0a4 reshuffles GS nets anyway.

## 8b. S4 — integration into the 138B build (2026-09-05)

What the 138B full core now contains (`a2mega_138B.gprj` only; the 60K
project does not list `hdl/twgs/gs_socket_defines.v`, so `GS_SOCKET` is
undefined there and neither the logic nor the 38 pins exist in that build):

- `hdl/twgs/gs_socket_ctl.sv` — PHY + core + telemetry wrapper, instantiated
  in `top.sv` under `` `ifdef GS_SOCKET `` with the `gs_*` ports; the D bus
  is a plain `assign gs_d = oe ? d : 8'bz` inout.
- **Sequencer clock: a PLL of its own, `gowin_gs_pll` at 110 MHz** (VCO
  50 × 22 = 1100 MHz, ODIV 10; 108 is not reachable from the 50 MHz
  crystal inside the GW5AST PLL's 19–81 MHz PFD window without a
  fractional divider — 110 keeps the proven management-PLL recipe and the
  exact rate is immaterial, the socket runs at PHI2). It costs the die's
  8th and last PRIMARY clock net, so the fabric-routed `gs_ph2` lands on a
  long-wire net; that is acceptable at 2.86 MHz. The alternative, sharing
  `clk100`, was tried and rejected: it crowds the DP AUX ladder, which
  lives on the knife edge in that domain (one roll failed it at −1.07 ns).
- **The routing thrash and its real cause.** The first integration rolls
  routed erratically: 12 min, 31 min, and four kills at the 40-minute cap,
  while the same source with the socket compiled out routed in 17 min.
  Single-variable diagnostics: socket without the core = 15 min clean;
  core on a legitimate PRIMARY 27 MHz clock = still thrashed. So the
  core's own logic was the problem, and GowinSynthesis had said what:
  "Latch inferred for net xhdl0.sum2low[8]" — branch-scoped temporaries
  in the MiSTer core's ADDR_BUS block (its author had waived the same
  Verilator warning). Latch loops make a timing-driven router iterate
  without converging. Hoisting those temporaries (`sum2low/sum2car/sum1/
  off/sum9/useWrap`) to continuous assignments — identical values, both
  benches unchanged, Verilator `-Wall` latch-clean — brought every roll
  back to finishing. Lesson recorded in the core README: treat EX2420 as
  a build blocker on this design.
- **Arming and telemetry over the existing ESP32 register path** (no
  firmware change needed: `spireg` reads/writes any register). A two-register
  window: write the index to `0x5F`, then read/write `0x4F`:

  | index | name | R/W | meaning |
  |---|---|---|---|
  | 0 | CTRL | RW | bit 0 arm (take the socket), bit 1 data-hold sweep on, bit 2 listen (enable the control-input shifter only: PHI2/RDY//RES/IRQ/NMI/ABORT/BE become visible, nothing is driven — the C3 step), bit 7 clear counters |
  | 1 | STATUS | R | {PH2 alive, core running, enabled, BE pad, /RES pad, RDY pad, slot /DMA, slot /RDY} |
  | 2 | OUT_EXTRA | RW | address-delay sweep: extra sequencer clocks before the cycle is issued (0–15, 9.1 ns each) |
  | 3 | HOLD_TAP | RW | data-hold sweep: clocks after the synchronised fall at which D0–7 is re-sampled (0–31) |
  | 4–7 | CYCLES | R | bus cycles the core advanced through (32-bit, little-endian) |
  | 8–9 | STALLS | R | cycles repeated because RDY was low |
  | 10–11 | BE_LOW | R | sequencer clocks spent with BE low (saturating) |
  | 12–13 | HOLD_MISMATCH | R | sweep: re-sampled byte ≠ the byte taken at the fall |
  | 14–15 | HOLD_SAMPLES | R | sweep: read cycles sampled |
  | 16–17 | PH2_PERIOD | R | sequencer clocks per 256 PHI2 cycles (fast mode ≈ 9 830; 1 MHz ≈ 27 500) |
  | 18–19 | PH2_HIGH | R | sequencer clocks PHI2 high per 256 cycles (duty) |
  | 20–22 | LAST_ADDR | R | {lo, hi, bank} of the last cycle issued |

  Telemetry crosses from the sequencer clock to the connector clock with
  two flops per bit, like the other debug counters (a multi-byte counter
  can tear between reads; read twice).

- **Bench procedure this enables (C3/C4):** power up with the ribbon in and
  CTRL = 4 (listen) — only the control-input shifter is enabled, nothing is
  driven; STATUS shows PH2 alive and the pad levels, and PH2_PERIOD/
  PH2_HIGH give the FPI clock's period and duty (C3 listen-only). (With
  CTRL = 0 that shifter is off too and PHI2 is invisible — found on the
  first power-up.) Then CTRL = 1 to arm: the core starts at the next
  /RES release (or immediately if /RES is already high), CYCLES and
  LAST_ADDR show it running. C4 instruments: raise OUT_EXTRA until the
  machine misbehaves (that is the FPI's real tADS tolerance, in 9.1 ns
  steps above the ~60 ns baseline); enable the sweep and step HOLD_TAP
  from 0 upward, watching HOLD_MISMATCH/HOLD_SAMPLES — the first tap with
  mismatches is where the motherboard stops holding read data after the
  fall.

- **Build record (final S4 source, commit e8631053 HDL; Gowin V1.9.12.01;
  138B).** Five rolls, all that finished were clean; one was killed by the
  old 30/40-minute guard at 40 min and its two 60-minute-guard successors
  finished at 23 and 41 min. The socket build takes 21–41 min against a
  15–17 min baseline for the same source with the socket compiled out
  (the fabric-routed PHI2 clock into ~300 core flops is the extra routing
  effort); `tools/build.sh` now gives the `a2mega_138B` project a
  60-minute guard for that reason, everything else keeps 30. Reported
  slack under the 0.5 ns policy (real = reported + 0.5):

  | roll | wall | clk_gs | gs_ph2 | clk100 | clk_sym | clk_pix | clk_logic | clk_x1 | bitstream sha256 |
  |---|---|---|---|---|---|---|---|---|---|
  | 1 | 32 min | +2.22 | +324.7 | +0.11 | +0.06 | +0.13 | +1.64 | +3.22 | 20aee44d7a6899b7… |
  | 2 | 21 min | +2.40 | +325.5 | +0.04 | +0.09 | +0.35 | +2.94 | +2.40 | 60d7e6a70eff724b… |
  | 3 | 23 min | +1.83 | +324.9 | +0.01 | +0.07 | +0.21 | +1.97 | +2.21 | 75616e6be5cdf99e… |
  | 4 | 41 min | +2.02 | +324.9 | +0.05 | +0.02 | +0.05 | +2.38 | +2.92 | 8e4e77da3aa172f8… |

  0 setup / 0 hold on every roll; 28 5xx logic (21 %), 127 BSRAM, 158 I/O,
  5 PLL. The dp_test gate is untouched (no socket there). The clk100 /
  clk_sym / clk_pix minima are the DP block's pre-existing knife edge
  (the durability campaign's 138B rolls sat at +0.01…+0.29 there too).

## 9. Proposed work plan

| Step | Deliverable | Gate |
|---|---|---|
| S1 | This document reviewed; answers to §8 — **closed 2026-09-05** (interposer exists, ROM 01 machine, 108 MHz PLL core clock, GPL already covered, PH2 swap optional) | ed ✔ |
| S2 | Socket PHY (`gs_socket_phy.sv`, core clocked by PHI2) + socket SDC + Sim 1 — **DONE 2026-09-05**, both configurations pass; 138B PnR probe on the real GS balls 0/0 (§4.3) | assertions clean ✔ |
| S3 | Sim 2: ROM 01 through the socket path with a lockstep reference core — **DONE 2026-09-05** (see §7) | zero divergences ✔ |
| S4 | Integration into the 138B full core behind CTRL.arm; register window + sweeps (§8b); 138B build under the margin policy — **DONE 2026-09-06**, four clean rolls (§8b) | 0/0 + 3 rolls ✔ |
| S5 | Bench C3 listen-only | test-log rows |
| S6 | Bench C4 (sweeps → vector fetch → boot) | boot chime |
| S7 | Bench C5 soak | board-turn bar |

S1 closed 2026-09-05; S2 (socket PHY + CE generator + SDC + Sim 1) is
the next step.
