# Apple-off backfeed through the bus level shifters

**Design note, 2026-09-14.** Applies to a2mega 1.0a3 (and the 1.0a4 rev),
a2p25, and by reference a2n9. Question: when the card is powered from USB
(monitor or hub VBUS) while the Apple II is switched off, what current does
the card source into the unpowered motherboard, and should the Apple-facing
side of the level shifters be powered from `BUS_5V` (the raw slot rail) instead
of the board `+5V` plane so that it is unpowered whenever the Apple is?

Everything below comes from the KiCad netlists (`a2-mega`: `a2bus.kicad_sch`
and `gs.kicad_sch`; `a2-p25`: `a2bus.kicad_sch`; `a2-n9`), the vendor
datasheets named in §7, and `boards/a2mega/hdl/top.sv` on the main branch.
Numbers are calculated, not measured; §6 lists the bench checks.

---

## 1. Summary

- The transceiver question is vendor-specific. The **Nexperia 74ALVC164245**
  documents a suspend mode (either supply at 0 V → both ports high-impedance,
  no current from the live supply into the dead one). The **TI SN74ALVC164245**
  documents nothing of the kind and its absolute-maximum B-port voltage is
  `VCCB + 0.5 V`, so with `VCCB = 0` any Apple-side pin above 0.5 V is out of
  rating. The TI part must not be used with `VCC(B)` on `BUS_5V`.
- The **TI SN74LVC16T245** is pin-identical in the same DGG package and
  documents the isolation TI's ALVC part lacks ("if either VCC is at GND, both
  ports are high-impedance", Ioff ±2 µA). It is TI-only; Nexperia has no
  LVC16T245 and its 74AVC16T245 ports stop at 3.6 V.
- The **LSF0108** is the easy part. It has no supply pin; its enable is the
  pass-FET gate, biased through the 200 kΩ resistor. Moving that resistor to
  `BUS_5V` puts the part in its documented EN-low high-impedance state
  (≤3 µA per channel) whenever the Apple is off. No sequencing requirements.
- **With no changes**, the only path that is both live today and large enough to
  damage a specific part is the 549 Ω pull-up on the socket `VP` line, which
  sources about 8 mA into the unpowered FPI (CYA) on the IIgs motherboard.
  The LSF control lines add a few milliamps spread over seven lines. Everything
  else is microamps unless gateware misbehaves.
- The gateware on both a2mega and a2p25 ties the shifter output-enable low
  permanently after configuration. No Apple-alive gating exists anywhere.

Recommended rev change: move `VCC(B)` of the transceivers, the LSF bias
resistors, and the socket-sheet 5 V references to `BUS_5V`; adopt the BOM rule
in §5; give the FPGA a `BUS_5V`-present input and gate the output enables on it.

---

## 2. Current topology

### 2.1 a2mega 1.0a3

Power ingress: slot `BUS_5V` → Q1 (CSD17571Q2) + U3 LM74700 ideal-diode
controller → `+5V` plane; USB-C VBUS → PTC → U9 LM66100 → `+5V`; SOM JTAG 5 V
via the BTB. `+5V` feeds the SY8089 buck for `+3V3`. The LM74700 blocks reverse
current, so with the Apple off `BUS_5V` sits near 0 V while `+5V` and `+3V3`
are up. `BUS_5V` also feeds the GS interposer header (J5 pin 15).

Apple-side parts and what powers them (`+5V` = board plane, not the slot):

| Ref | Part | Apple-side supply / reference | Enable | Role |
|---|---|---|---|---|
| U1, U2 | 74ALVC164245 | `VCC(B)` pins 7/18 = `+5V` | `FPGA_BUS_~OE`, 4.7 kΩ to `+3V3` (R7) | slot address (input only, DIR = GND) and data (DIR = `FPGA_D_DIR`) |
| U4 | LSF0108PW | `Vref_B`/EN = R8 200 kΩ to `+5V`, C17 0.1 µF | EN pulled low by U5 gate when `FPGA_BUS_~OE` is high | IRQ, INH, DMA, RDY, NMI, RES, R/W |
| U5 | 74LVC2G06 | `+3V3` | — | open-drain: LSF EN override, `BUS_~RES` |
| U13 | 74ALVC164245 | `VCC(B)` = `+5V` | `FPGA_GS_~OE`, `FPGA_GS_DATA_~OE` (4.7 kΩ to `+3V3`) | socket control-in (DIR = GND) and data |
| U14 | 74ALVC164245 | `VCC(B)` = `+5V` | `FPGA_GS_ADDR_~OE` (4.7 kΩ to `+3V3`) | socket address out (DIR = `+3V3`, A→B only), 75 Ω series |
| U16 | SN74LV1T125 | `VCC` = `+5V` | `FPGA_GS_ADDR_~OE` | socket R/W out |
| U15 | 74LVC2G07 | `+3V3` | — | open-drain: `GS_RDY` wire-OR, `GS_VP` |
| R65 | 549 Ω | `+5V` → `GS_VP` | none | pull-up for the open-drain VP driver |

FPGA-side control lines (`FPGA_~IRQ`, `~INH`, `~DMA`, `~RDY`, `~NMI`, `~RES`,
`R/~W`) each carry a 4.7 kΩ pull-up to `+3V3` (R9–R15).

### 2.2 a2p25

Same bus sheet, with a Schottky (D1, SOD-323) in place of the LM74700 between
`BUS_5V` and `+5V`. U2 and U4 `VCC(B)` = `+5V`; LSF bias R2 200 kΩ = `+5V`;
U1 74LVC2G06 supplied from `+5V` (open-drain, sinks only). No socket sheet.

### 2.3 a2n9 (existing counter-example)

The a2n9 has no separate `BUS_5V` net: the slot 5 V pin **is** the card's
`+5V`, which supplies `VCC(B)` of U4 (74ALVC164245) and U7 (SN74LVC8T245)
directly, and reaches the Tang Nano 9K only through D2. `VCC(A)` comes from
the module's own 3.3 V regulator, which the module's USB port can power. So the
a2n9 already runs the "`VCC(B)` on the slot rail, `VCC(A)` live" configuration
whenever it is USB-powered in an unpowered Apple. Its BOM MPN is the Nexperia
part (74ALVC164245DGG).

### 2.4 Gateware

`boards/a2mega/hdl/top.sv` and `boards/a2p25/hdl/top.sv` both contain
`assign a2_bus_oe = 1'b0;` — the transceivers (and, through U5, the LSF) are
enabled for as long as the FPGA is configured. The address port is input-only
(`a2_a_dir = 0`). The data direction follows the decoded read strobes
(`data_out_en_w = ssp_rd || mb_rd || ...`). The main branch does not drive the
socket-sheet enables at all; they rest on their pull-ups, disabled. The
gs-socket branch must be checked separately (§6).

---

## 3. Datasheet facts

### 3.1 TI SN74ALVC164245 (SCAS416Q, rev. Sept 2016)

- Features: Widebus, tpd, ±24 mA at 3.3 V, control inputs referenced to
  `VCCA`, latch-up. **No Ioff, no VCC-isolation feature.**
- Absolute maximum, I/O port B: `–0.5 V to VCCB + 0.5 V`. With `VCCB = 0 V`
  the B pins are limited to 0.5 V.
- Only power-sequencing guidance: "To ensure the high-impedance state during
  power up or power down, OE should be tied to VCC through a pullup resistor."
- Static: IOZ ±10 µA. Behavior with one supply at 0 V is not specified.

### 3.2 Nexperia 74ALVC164245 (Rev. 13, 24 April 2024)

- Features include "IOFF circuitry provides partial Power-down mode operation"
  and "High-impedance outputs when VCC(A) or VCC(B) = 0 V".
- General description: "In suspend mode, when one of the supply voltages is
  zero, there will be no current flow from the non-zero supply towards the zero
  supply." (The sentence that follows, requiring the A outputs 3-state and the
  A bus below a diode drop, describes the `VCC(A) = 0` case, the inverse of
  ours.)
- Limiting values: 3-state I/O voltage max 6.0 V regardless of VCC
  (overvoltage tolerant); `VCC(B)` and `VCC(A)` both carry the condition
  **`VCC(B) ≥ VCC(A)`**, and the description adds "(except in suspend mode)".
  The Apple's rail passes through `0 < VCC(B) < 3.3 V` on every power-up ramp
  (tens of ms) and for much longer on power-down decay. That window is neither
  suspend mode nor the stated operating condition.
- **No numeric Ioff row** in the static characteristics; the suspend-mode
  guarantee is qualitative. IOZ ±10 µA (±20 µA at 125 °C).

### 3.3 TI SN74LVC16T245 (SCES636B, rev. April 2015)

- "VCC Isolation Feature – If Either VCC Input is at GND, Both Ports are in the
  High-Impedance State"; "Ioff Supports Partial-Power-Down Mode Operation";
  overvoltage-tolerant inputs and outputs; each port 1.65–5.5 V independently,
  no `VCCB ≥ VCCA` constraint.
- Ioff: ±1 µA (±2 µA over temperature) with either VCC at 0 V and the pin at
  0–5.5 V.
- Drive: ±24 mA at 3.3 V, ±32 mA at 4.5 V (B port at 5 V improves on ALVC).
- Requirement: "the OE input pin must be tied to VCCA through a pullup resistor
  and must not be enabled until VCCA and VCCB are fully ramped and stable."
- Pinout, DGG-48: 1DIR 1, 1OE 48, 2DIR 24, 2OE 25, `VCCB` 7/18, `VCCA` 31/42,
  A pins 47,46,44,43,41,40,38,37 / 36,35,33,32,30,29,27,26, B pins
  2,3,5,6,8,9,11,12 / 13,14,16,17,19,20,22,23, GND 4,10,15,21,28,34,39,45.
  **Identical to both ALVC164245 parts**; the KiCad symbol and footprint carry
  over unchanged.
- Sourcing: TI only. Nexperia's 16-bit dual-supply parts are the 74ALVC164245
  and the 74AVC16T245 (ports ≤3.6 V, unusable on the 5 V bus). Chinese clones
  (e.g. Wuxi I-CORE AiP74LVC16T245) exist but are not a qualified second source.

### 3.4 TI LSF0108 (SNLS381)

- "The LSF family has an EN input that is used to disable the device by setting
  EN LOW, placing all I/Os in the high-impedance state."
- "To ensure the high impedance state during power-up, power-down, or during
  operation, the EN pin must be LOW. The EN pin should always be tied directly
  to the Vref_B pin and is recommended to be disabled by an open-drain driver
  without a pullup resistor." (This is exactly the U5 arrangement.)
- "There are no power sequence requirements for the LSF family."
- Leakage with `VEN = 0 V`, 5 V across the switch: 0.5 µA typ, 3 µA max per
  channel. Recommended `VI(EN)` ≥ `Vref_A + 0.8 V`.
- Mechanism: EN is the gate of every pass FET. The internal reference
  transistor conducts only from EN into `Vref_A`, so nothing on the 3.3 V side
  can hold EN up. If the rail feeding the 200 kΩ bias resistor collapses, EN
  discharges through that resistor (τ = 200 kΩ × 0.1 µF = 20 ms) and every
  channel is off. On the way up, a channel conducts only while its gate exceeds
  the pin by a threshold, so the FPGA-side 3.3 V pull-ups can never lift a bus
  line above roughly `BUS_5V` minus a threshold during the Apple's ramp.

---

## 4. Leak inventory with the design as built

Card on USB power, Apple off, `BUS_5V` ≈ 0 V, `+5V` and `+3V3` up, FPGA
configured and running main-branch gateware. Note that in the socket
configuration the 65C816 is removed and the ribbon lands in the empty socket,
so every socket line goes straight to motherboard silicon (FPI/CYA, Mega II,
RAM, ROM).

| # | Path | Boards | Estimated current | Condition |
|---|---|---|---|---|
| 1 | R65 549 Ω from `+5V` to `GS_VP`; on the ROM 3 motherboard the socket VP pin is a plain input, **FPI (CYA) pin 25** | a2mega with interposer | (5 − 0.7 V) / 549 Ω ≈ **8 mA** into one ESD diode of an unpowered, irreplaceable Apple VLSI | live today |
| 2 | FPGA-side 4.7 kΩ pull-ups to `+3V3` through the enabled LSF into the motherboard's own pull-ups and inputs | a2mega, a2p25 | ≈ 0.4–0.6 mA per line × 7 ≈ **3–4 mA** total | live today |
| 3 | Slot data bus, U1/U2 B outputs, if a read strobe stays decoded on a dead bus | a2mega, a2p25 | up to 24 mA per line | gateware-dependent, not traced |
| 4 | Socket address (U14) and R/W (U16) into FPI/Mega II/RAM/ROM inputs through 75 Ω | a2mega with interposer | tens of mA per line | only if gateware enables `FPGA_GS_ADDR_~OE` with the Apple off; never on main |
| 5 | Transceiver IOZ (≤10 µA/pin), LSF off-leakage, LM74700 reverse leakage, open-drain gates against a 0 V line | all | µA | always |

Path 1 is the one to act on: it is live, concentrated on a single pin, and the
victim is the FPI. Path 2 is small per pin and stresses nothing, but it
continuously biases a dead CMOS rail, the "partial power" condition that
produces odd power-up states. Paths 3 and 4 are the defense-in-depth argument
for unpowering the Apple side rather than trusting the enables.

---

## 5. Options and recommendation

**A. Keep `VCC(B)` on `+5V`, add an Apple-alive interlock on the enables.**
Vendor-independent: both ALVC vendors and the LSF specify the disabled-state
leakage. Requires a `BUS_5V`-present signal. In hardware, one logic-level
N-FET per OE net (source on the OE net, drain on the FPGA pin, gate from a
`BUS_5V` divider) forces the net high through the existing 4.7 kΩ pull-up
whenever the rail is absent, regardless of gateware; the body diode then only
ever pulls the net toward disabled. Does nothing for R65 by itself.

**B. Move `VCC(B)`, the LSF bias resistors and the socket-sheet 5 V references
to `BUS_5V`.** The Apple-facing side is then unpowered whenever the Apple is,
by construction. Constraints:

- BOM rule for the transceivers: **Nexperia 74ALVC164245, or TI SN74LVC16T245.
  Never TI SN74ALVC164245.** Each alternate is single-sourced.
- Both acceptable parts still want OE high while `VCC(B)` ramps (TI states it
  as a requirement for the LVC16T245; Nexperia's `VCC(B) ≥ VCC(A)` condition is
  violated during the ramp). With the FPGA already running under USB power
  when the Apple is switched on, this needs the same `BUS_5V`-present input as
  option A, at least in gateware.
- The Nexperia guarantee has no numeric Ioff; §6 lists the bench measurement.

**Recommendation: B plus the interlock input from A.** Concretely, for 1.0a4:

| Board | Change |
|---|---|
| a2mega | U1, U2, U13, U14 `VCC(B)` (pins 7/18) → `BUS_5V` |
| a2mega | R8 200 kΩ LSF bias → `BUS_5V` |
| a2mega | R65 549 Ω → `BUS_5V` |
| a2mega | U16 SN74LV1T125 `VCC` → `BUS_5V` (confirm the LV1T125 Ioff / partial-power-down rating first; otherwise keep on `+5V` and rely on its OE) |
| a2mega, a2p25 | `BUS_5V`-present divider into a 3.3 V FPGA input; gateware holds `a2_bus_oe` (and the socket enables) inactive until present, optionally the hardware FET interlock on `FPGA_BUS_~OE` |
| a2p25 | U2, U4 `VCC(B)` → `BUS_5V`; R2 200 kΩ → `BUS_5V` |
| a2p25 | U1 74LVC2G06 may stay on `+5V` (open-drain, sinks only) |
| BOM | transceivers: Nexperia 74ALVC164245DGG or TI SN74LVC16T245DGGR; strike TI SN74ALVC164245 as an alternate |

The FPGA-side 4.7 kΩ control pull-ups and the U5/U15 open-drain drivers are
unchanged: with the LSF off they are isolated, and an open-drain output cannot
source current into a line sitting at 0 V.

If only one change can be made, move R65. If two, move the LSF bias resistors.

---

## 5a. The `BUS_PWR_ACTIVE` sense line

Status of the 1.0a4 schematic (netlist re-checked 09-15): transceiver
`VCC(B)`, R8, R65, U16 `VCC` and their decoupling are on `BUS_5V`; nothing
Apple-facing remains on `+5V`. U18 TLV803EA43DBZR (VDD = `BUS_5V`, RESET →
node), R76 22 kΩ / R77 33 kΩ, node → J1-4 → ball C20 (bank 2, same bank as
the bus I/O), Q2 BSS138 gate on the node. Q2 verified 09-15: drain (pin 3) on
`FPGA_BUS_~OE`, source (pin 2) on J1-13; ngspice on both orientations with
the rail off and the FPGA driving low gives 3.30 V (correct) vs 0.52 V
(wrong, reads as enabled). Socket enables are gateware-only.

Schematic of the supervisor + interlock arrangement:
`bus_pwr_active_interlock.svg` (same directory).

**Circuit (baseline): a resistor divider.** 22 kΩ from `BUS_5V`, 33 kΩ to
GND, node = `BUS_PWR_ACTIVE` to one 3.3 V FPGA input (LVCMOS33, no internal
pull, HYSTERESIS attribute set in the .cst). 0.6 × rail: 3.0 V at 5 V,
3.15 V at 5.25 V, 3.3 V at a 5.5 V rail (input limit 3.6 V); ≈90 µA slot
load; 0 V with the Apple off, no dead zone. Optional 1 nF across the 33 kΩ
against rail ripple.

Semantics are "rail present", not "rail in regulation": LVCMOS33 guarantees
high above 2.0 V and low below 0.8 V at the pin, so the trip point lies
somewhere between 2.2 and 3.3 V of rail and drifts with PVT. The Apple's
rail ramps slowly (ms up, hundreds of ms down), so the input can bounce in
its threshold region. The gateware therefore (a) debounces, (b) holds the
enables off for 50–100 ms of stable high, and (c) treats 7M activity as the
real "Apple is running" qualifier; 7M stops well before the rail is out of
spec.

**Circuit (optional upgrade): a supervisor.** `TLV803EA43DBZR` (SOT-23-3
standard pinout 1 GND / 2 RESET / 3 VDD; open-drain active-low; 4.38 V
falling threshold, ±0.5 % typ, ~1.2 % hysteresis; 200 ms release delay;
250 nA), `VDD` = `BUS_5V` with 0.1 µF, `RESET` = the node, with the same
22 kΩ/33 kΩ pair as its pull-up. The pull-up must come from `BUS_5V`
through the divider, not from `+3V3`: below its power-on-reset voltage
(0.7 V) the output is undefined, and a `+3V3` pull-up could read "active"
with the Apple fully off. With the divider the node is 0 V at 0 V rail, at
most 0.42 V while the supervisor is below VPOR, and 3.0 V only after
200 ms above the rising threshold (~4.43 V). Buys a precise threshold and
the delay in hardware, independent of gateware.

Orderables that actually exist per the TLV803E datasheet addendum
(SLVSES2J, May 2021): at 4.38 V, `TLV803EA43DBZR` (200 ms) and
`TLV803EC43DBZR` (10 ms) in the standard pinout, plus R/V-pinout and SC-70
variants of the 200 ms part; at 4.2 V, only `TLV803EA42RDBZR` (200 ms, R
pinout: 1 RESET, 2 GND) and `TLV803EB42VDBZR` (40 µs, V pinout). No 50 ms
or 100 ms part exists at either threshold. Trade-off of the 4.38 V trip:
~0.3 V margin on a IIgs with a sagging 4.75 V rail, and any dip below
threshold drops the bus for the delay time. The older `TLV803M` (4.38 V,
200 ms, output valid to 1.1 V) works identically.

**Choosing the hold time.** Two timelines:

- *Apple powers the card* (normal): the FPGA spends hundreds of ms
  configuring after the rail is up, so any hardware delay ≤200 ms has
  expired before gateware runs; the existing reset-hold contract does the
  work. The delay is invisible here.
- *Card already on USB power, Apple switched on*: the FPGA is waiting with
  the re-armed hold asserted. The reset path (`FPGA_~RES_OUT` → U5
  74LVC2G06 on `+3V3` → open-drain onto `BUS_~RES`) bypasses the
  transceivers and the LSF, so /RESET is already held low when the Apple's
  rail rises and the CPU never fetches the reset vector. The supervisor
  delay only orders the bus enable; reset release follows it. The Apple
  boots once, with the card already on the bus, whatever the delay is.

10 ms is electrically sufficient: release happens only after the rail has
been above ~4.43 V for the hold, and at that voltage no transceiver or LSF
condition is undefined. A dip below 4.38 V drops the bus within ~20 µs
regardless of the hold; the threshold, not the delay, sets the brownout
exposure (≈0.3 V on a IIgs with a sagging 4.75 V rail; the 4.2 V R-pinout
part gives 0.5 V). The only argument for 200 ms is riding through
load-induced rail chatter while the card's own load ramps, which happens
while the FPGA is unconfigured anyway.

Decision (09-15): **`TLV803EA43DBZR`** (200 ms). With the hold asserted
across power-up the delay has no boot-visible effect in either case; the
200 ms rides through load-induced rail chatter without gateware help, and
it is the stocked, cheap, standard-pinout part. The A43 and C43 (10 ms)
parts share the footprint.

Clone tolerance: the divider pull-up from `BUS_5V` makes the circuit
indifferent to the supervisor's VPOR and pull-up-above-VDD rating, and the
0.23 mA sink makes drive irrelevant. A pin-compatible clone (e.g. Tech
Public `TPTLV803EA43DBZR`, datasheet not located online as of 09-15) must
be checked only on: falling threshold 4.25–4.5 V, nonzero hysteresis,
release delay 50–400 ms, VDD rating ≥ 5.5 V, released output leakage
< a few µA, and the DBZ pin order (1 GND, 2 RESET, 3 VDD). List TI as
primary and the clone as alternate.

**Gateware contract** (`a2_bus_pwr_active` top-level input). The mechanism
as built is one-shot from FPGA configuration: in `esp32_ospi_connector.sv`
the hold counter only counts up, release (ESP32 write of A2_RST_RELEASE,
3 s no-MCU timeout, or 15 s backstop) is latched for the life of the
configuration, and `a2_bus_oe` is a constant. In case B the card has been on
USB for minutes, so the Apple comes up to a card already on the bus with no
hold and, after a brownout, with stale bus-side state. The contract must
re-arm:

1. Two-flop synchronize the input into the FPGA's own clock domain (not the
   Apple's 7M, which is dead when the rail is). Define
   `bus_alive = bus_pwr_active_sync && a2_7m_alive`.
2. Output enables (`a2_bus_oe`, and the socket enables) active only after
   `bus_alive` has been stable for the gateware hold (50–100 ms); dropped
   the cycle it falls. Through U5 this also drops the LSF enable.
3. On `bus_alive` falling: zero `rst_hold_cnt_r`, clear the release latch
   (`a2_rst_release_r`), and reset the Apple-facing state (data-out enable,
   DMA, RDY, IRQ, latched address/data, disk/Uthernet/DOC transaction state).
   Do not reset the ESP32 link, DDR3, video, or the mounts.
4. The re-armed hold is asserted while `bus_alive` is low, not only after
   the bus enables: U5 sinks the (dead) /RESET line continuously, so the
   Apple's rail rises into a held reset and the CPU never starts. Holding
   /RESET from before the rail rises is the auto-arm case characterized in
   the S6 reset-sequencing work (IIgs ADB micro shares the line); that is
   the reference. On `bus_alive` rising (after the gateware hold): enable
   the bus, then release. Release path after a re-arm: the ESP32 will not
   rewrite A2_RST_RELEASE on its own, so either latch "mounts done" from
   the first release and use a short fixed hold (~100 ms after enable)
   when it is set, or raise a "bus power cycled" status bit and have the
   firmware rewrite the register. Recommended: the former, with the status
   bit for logging.
5. Report `bus_alive` and re-arm events on the debug overlay / status LED.

**Optional hardware interlock.** The same node can gate a BSS138 in series
with `FPGA_BUS_~OE`: **drain (pin 3) on the OE net, source (pin 2) on the
FPGA's OE ball, gate (pin 1) on `BUS_PWR_ACTIVE`**. With the rail absent
the FET is off and the existing 4.7 kΩ pull-up disables the bus regardless
of gateware. Orientation matters: the body diode conducts source→drain,
i.e. from the FPGA pin into the OE net, which can only pull the net up
(disabled). The other way round (source on the OE net) the diode would
pass an FPGA low into the net with the FET off and defeat the interlock.
With the FET on and the FPGA driving low, Vgs is the full ~3 V. Repeat for
the three socket enables if desired. Schematic:
`bus_pwr_active_interlock.svg`.

---

## 6. Open items and bench checks

1. **Measure, do not assume.** Card on USB, Apple off, DMM in series with the
   slot 5 V pin: total current into the motherboard as built, then after each
   change. Expected: ~10 mA as built with the interposer, ~4 mA without, µA
   after the rev. Also measure `BUS_5V` at the connector with the Apple off to
   confirm the LM74700 (a2mega) and D1 (a2p25) actually hold it near 0 V.
2. **Nexperia suspend-mode current.** `VCC(A)` = 3.3 V, `VCC(B)` = 0 V, A port
   toggling, B pins at 0 V: measure current out of the `VCC(B)` pins and into
   the B pins. The datasheet gives no number.
3. **SN74LV1T125 on `BUS_5V` — CLOSED 09-15.** SCLS745D: A and OE inputs
   are rated to 7.0 V independent of VCC (5.5 V tolerant input structure,
   no diode to VCC), so 3.3 V from the FPGA with VCC = 0 backfeeds nothing.
   Output rating is VCC + 0.5 V, satisfied structurally: GS_RW in the empty
   socket is pulled up only to the motherboard rail, which is U16's VCC.
   OE (R37 to +3V3) is a valid high at VCC = 5 V (VIH(min) 2.0 V). No Ioff
   row published; the case rests on the ratings, as for the Nexperia part.
4. **gs-socket branch gateware.** Confirm `FPGA_GS_ADDR_~OE` and the data port
   enable are held inactive when no Apple clock is present.
5. **Main-branch data direction on a dead bus.** Trace whether any `*_rd` term
   in `data_out_en_w` can stay asserted when the bus clocks stop.
6. **a2n9 field evidence.** Whether an a2n9 has sat USB-powered in an
   unpowered Apple for long periods (with the Nexperia part) is the only
   long-duration data point on the suspend-mode configuration.
7. **1.0a4 requirements cross-reference.** Fold the change list into
   `board_1_0a4_requirements.md` item 4b (power-source architecture) at the rev.

---

## 7. Sources

- TI, *SN74ALVC164245 16-Bit 2.5-V to 3.3-V or 3.3-V to 5-V Level-Shifting
  Transceiver*, SCAS416Q (1994, rev. 2016).
- Nexperia, *74ALVC164245 16-bit dual supply translating transceiver; 3-state*,
  Rev. 13, 24 April 2024.
- TI, *SN74LVC16T245 16-bit Dual-Supply Bus Transceiver With Configurable
  Level-Shifting*, SCES636B (2005, rev. 2015).
- TI, *LSF0108 8-Bit Bidirectional Multi-Voltage Level Translator*, SNLS381.
- a2-mega KiCad project (`a2bus.kicad_sch`, `gs.kicad_sch`), a2-p25 and a2-n9
  KiCad projects, netlists exported with `kicad-cli sch export netlist`.
- Apple IIgs ROM 3 redrawn schematic (bit-preserve), `CYA-Mem-1.kicad_sch`:
  socket VP → CYA pin 25.
- `boards/a2mega/hdl/top.sv`, `boards/a2p25/hdl/top.sv` (main).
