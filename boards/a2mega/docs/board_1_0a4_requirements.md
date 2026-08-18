# a2mega 1.0a4 — board revision requirements (rev gate)

**Ship gate: end of this week (target Fri 2026-08-22).** Every item is
EVIDENCED (links to test log / bringup log) or marked OPEN with the
experiment that closes it. Nothing goes on the board without a reason;
no rev until the OPEN items that could change the netlist are closed.

## CONFIRMED changes (evidence in hand)

1. **AUX front-end: proper bias + level network on DPAUX_P/N (SBU path).**
   THE critical fix. 1.0a3 has AC caps with no FPGA-side bias; the
   LVCMOS pseudo-diff drive is ~2 Vpp (spec ≤1.38 Vpp) and only decodes
   through channel attenuation (test log #9: breakout-as-attenuator).
   RX side: replies electrically unreachable at LVCMOS thresholds;
   TLVDS+pulldown hack reads converter-class sinks only (offset-marginal
   for monitors). Design: bias divider network setting known common mode
   + zero standing differential at the receiver, and TX levels within
   spec (resistor divider/series attenuation, or commit to the TLVDS pad
   as the driver and bias for it). **OPEN sub-question gating the exact
   values/topology: TLVDS clean test (task #2) + optional AD2 waveform
   of the working attenuated path — DO THIS EARLY THIS WEEK.**

2. **RECONFIG_N (ball N12) routed to an ESP32 GPIO.** Bench/dev recovery
   and automated test-reroll. NOT a consumer-facing mechanism (decision
   of record 2026-08-16: no user-level reconfiguration ever). Weak
   pull-up; ESP32 pin must be Hi-Z/open-drain at power-on (config guide:
   keep high during power-on/config; ≥25 ns low pulse = reload).

3. **Keep all four SERDES lanes routed to the mux** (as 1.0a3; uniform
   P/N swap is fine — tx_pol_invert handles it). 4-lane remains a future
   option; no change needed, just don't lose it.

4. **VBUS source-path isolation + discharge** (evidence 08-17 eve,
   instrumented): FUSB302 reads VBUSOK=1 with the port EMPTY — the 5V
   plane backfeeds the connector VBUS rail, so the vSafe0V source-attach
   precondition can never be met honestly (root of the in-slot attach
   flakiness; 98461af3 had to remove the safety veto to attach at all).
   Fix: real load switch/ideal diode between plane and VBUS pin +
   discharge path, so VBUS is genuinely 0 V until sourced.
5. **Local bulk capacitance on the VBUS source rail** for bus-powered
   sink inrush: hub cold-boot inrush through the source switch sagged
   the shared rail hard enough to kill ESP32 WiFi for ~1 min in-slot
   (bench supply masked it). Size for a bus-powered hub + downstream
   devices; consider soft-start on the switch.

## STRONG candidates (cheap, would have saved days)

6. **AUX/SBU test points** (2 pads near the caps) — the entire AUX saga
   was fought blind; two pads make it an oscilloscope problem.
7. **DP main-link ground-referenced test structure** if layout allows
   (even one lane's P/N stubs) — boot-draw/eye debugging.
8. **Confirm SecurityBit=OFF is process default** in all shipping build
   configs (process config item, not board — listed so it ships in the
   same review).

## OPEN questions that could still change the rev (close before Friday)

- **A. AUX network topology/values** ← TLVDS clean test (+AD2 capture).
  This is the only item where the schematic genuinely waits on data.
- **B. Hub-path re-baseline: CLOSED 08-17 eve** — verdict 0/10 power-on
  draws (D:2A channel-EQ stall, all sw variables eliminated; Sat '4/5'
  not replicable). No SI data implicating the board channel yet — the
  stall is at training, upstream of any margin question. Items 4/5 above
  (VBUS isolation + bulk cap) are the board-level fallout. Hub-viability
  hope now rides on WS4 (item C).
- **C. Gowin IP assessment fallout** (new-IDE EDP PHY / encoder): if the
  supported stack imposes pin/clock constraints we don't meet. Expected:
  none (same quad, same refclk); verify during assessment.

## Explicitly NOT in this rev

- Any user-facing reset/reconfig affordance (decision of record).
- HyperRAM/TransWarp endgame items (separate planned rev; see
  a2mega-hyperram-transwarp-plan).
