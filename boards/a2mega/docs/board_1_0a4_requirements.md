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

5b. **TUSB1046A operating-temperature margin (REFRAMED 08-18 night:
   normal heat, insufficient system margin)** — the mux runs "very hot"
   on EVERY carrier: 634 mW (datasheet, DP4 mode, all four linear
   channels always biased) in a 4x6 mm WQFN ≈ 2.6 W/cm² ⇒ case 50-60 °C
   at bench ambient with correct pad/vias. NOT a defect (voiding theory
   retired — both carriers identical); TI rates Tj to 105 °C. The defect
   is SYSTEM margin: link outcomes demonstrably flip with mux-region
   temperature (compressed-air experiment, rows 37-38), and the product
   lives in an enclosed Apple II case at 35-40 °C ambient ⇒ die 60-75 °C
   — the bench is the EASY case. Requirements: (a) maximize thermal
   pad/via/copper coupling and treat a 6x6 heatsink as standard fitment;
   (b) characterize and spec the margin-vs-temperature curve (soak test
   IN AN ENCLOSURE or with pre-warmed mux = the product condition);
   (c) best-fit mechanism = mux linear-path bandwidth/jitter degrading
   with die temp (narrow-spectrum TPS survives, broadband scrambled
   data dies — ISI-class); component-level confirmation via selective
   heating (mux vs cap bank) pending; (d) firmware lever: sweep mux RX
   EQ at WARM temperature on a good SOM (prior sweeps were on the
   SOM-limited unit) — a hotter-compensating EQ setting may buy margin
   in firmware. IR-measured package temperature to be added when
   available.

5a. **DP main-link channel/launch review — DESIGN-WIDE, not unit** —
   evidence 08-18: ALL converter-class sinks (3 hubs + a dongle, short
   captive cables, bridge-grade RX) fail BOTH carriers; monitors (long
   cables, display-controller RX) work on both; B1/B2 differ only as
   unit variance straddling one hub's bar. All sw/config variables
   nulled with instrumented data (C: per-lane, A: adjust, Q: ppm —
   crystals <10ppm both). Best-fit mechanism: board-side impedance
   discontinuity whose reflections short captive channels preserve and
   long lossy cables dissipate. Review for 1.0a4: connector launch,
   AC-cap selection/placement on the main-link pairs, mux output
   routing, ground return at the BTB; tighter-tolerance passives;
   jitter-specified 135M oscillator. NOTE: source-side viability on
   converter sinks may ultimately come from the supported Gowin IP
   stack (WS4) rather than board changes — treat this item as SI
   hygiene, not the sole fix.

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
