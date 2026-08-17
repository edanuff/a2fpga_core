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

## STRONG candidates (cheap, would have saved days)

4. **AUX/SBU test points** (2 pads near the caps) — the entire AUX saga
   was fought blind; two pads make it an oscilloscope problem.
5. **DP main-link ground-referenced test structure** if layout allows
   (even one lane's P/N stubs) — boot-draw/eye debugging.
6. **Confirm SecurityBit=OFF is process default** in all shipping build
   configs (process config item, not board — listed so it ships in the
   same review).

## OPEN questions that could still change the rev (close before Friday)

- **A. AUX network topology/values** ← TLVDS clean test (+AD2 capture).
  This is the only item where the schematic genuinely waits on data.
- **B. Anything from the hub-path clean re-baseline** (task #3) that
  implicates board-level SI (e.g., if FFE/swing tuning shows the channel
  into the mux needs help). Expected: no board change; verify.
- **C. Gowin IP assessment fallout** (new-IDE EDP PHY / encoder): if the
  supported stack imposes pin/clock constraints we don't meet. Expected:
  none (same quad, same refclk); verify during assessment.

## Explicitly NOT in this rev

- Any user-facing reset/reconfig affordance (decision of record).
- HyperRAM/TransWarp endgame items (separate planned rev; see
  a2mega-hyperram-transwarp-plan).
