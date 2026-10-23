# A2FPGA USB Game-Port Dongle — KiCad project (rev 0)

KiCad project for the gamepad→Apple II game-port dongle. Electrical rationale, BOM
and firmware protocol live in the design note:
**[docs/gameport-dongle.md](../../docs/gameport-dongle.md)**.

Written in KiCad 7 file format; opens cleanly in KiCad 7/8/9.

| File | Contents |
|---|---|
| `gameport-dongle.kicad_sch` | Full schematic (single A3 sheet, label-style wiring). All symbols are embedded — no external symbol libs needed. |
| `gameport-dongle.kicad_pcb` | Board outline only (64×40 mm). Populate with **Tools → Update PCB from Schematic (F8)**, then place & route. |
| `dongle.pretty/RP2040-Zero.kicad_mod` | Custom footprint for the Waveshare RP2040-Zero module (2×9 THT edge pins, 2.54 mm pitch, 15.24 mm row spacing). |
| `fp-lib-table` | Project-local footprint lib mapping (`dongle` → `dongle.pretty`). |

All other footprints reference the standard KiCad libraries (TSSOP-24, SOIC-14,
0603/0805, pin header, DSUB-9, solder jumper).

The netlist has been machine-verified against the intended connectivity
(all 21 nets, pin-by-pin, plus no-connect coverage) — but see the checklist
below for the items that could **not** be verified from the design environment.

## ⚠ Before layout — mandatory checklist

1. **Renumber U2 (AD5263).** The symbol's pin *numbers* are its logical pin
   *names* (`A1`, `W1`, `VDD`, …) because the ADI datasheet was unreachable when
   this was generated. Open the schematic, edit the embedded `dongle:AD5263`
   symbol, and replace each pin number with the real TSSOP-24 number from the
   [AD5263 datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/AD5263.pdf)
   pin-configuration table. Until then the TSSOP footprint pads will not
   connect (deliberately obvious, rather than silently wrong). If the device
   has additional pins not in the symbol (e.g. logic outputs O1/O2), add them
   as no-connects.
2. **DIS strap (JP1).** JP1 center pad goes to U2 DIS; pad 1 = GND, pad 3 =
   +5V_A2. Solder-bridge the side the datasheet specifies for **SPI mode**.
3. **RP2040-Zero footprint.** Verify pad positions against a physical module
   (assumed: 2×9 pins, 2.54 mm pitch, 15.24 mm between rows, 18×23.5 mm body,
   USB at the pad-1 end). Bottom castellations (GP9–GP13) are unused.
4. **AD5263 supply grade.** Confirm minimum single-supply V<sub>DD</sub> for
   the -200 grade vs. a worst-case Apple +5 V rail (~4.75 V). If marginal,
   re-power U2/U3 from USB 5 V (and confirm USB 5 V ≥ port 5 V).
5. **J1 cable strategy.** J1 is a plain 1×16 header; if you switch to a 2×8
   IDC ribbon with a DIP-16 plug, remap the pin order for the ribbon twist.

## ERC notes

- `PWR_FLAG` symbols are placed on `+5V_A2` and `GND` (both nets are sourced by
  connectors). `3V3` is driven by U1's pin (declared power output).
- U1's `5V` pin is intentionally no-connected — do **not** tie it to +5V_A2
  (it would back-feed the BL616's USB hub port).

## Layout guidance

- Keep the four `PDLn_W` wiper nets short and away from the SPI lines — they
  are high-impedance analog nodes (up to 200 kΩ source).
- R9–R11 (560 Ω pull-downs) go near the connector; R1–R4 (100 Ω) near U2.
- Single ground pour; the USB (U1) and game-port grounds join on this board.
- No length/impedance constraints anywhere; 2-layer, 0.25 mm traces are fine.
