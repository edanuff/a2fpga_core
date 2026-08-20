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
   as the driver and bias for it). *(Historical note: the original
   design sketch said "zero standing differential" — superseded by the
   RESOLVED DESIGN below, which deliberately parks a standing idle
   offset; the old OPEN sub-question on values/topology is closed.)*
   **VENDOR PRECEDENT (08-19, Gowin EDP refdesign cst + DK_DP/DK_VIDEO
   schematics; CORRECTED reading — the cst's tx/rx pairs are TWO
   different DP PORTS' AUX channels, source and sink, each a single
   bidirectional pair like ours)**: (a) Gowin's SOURCE port AUX =
   LVCMOS18D pair, PULL_MODE=NONE, DRIVE=4, no FPGA-side bias — viable
   only because their reference source policy is OPEN-LOOP (writes
   blind, never gates on replies); (b) their SINK port — the one that
   MUST receive — is the template that matters: D-type differential
   LVCMOS pair + PULL_MODE=DOWN, i.e. ON-DIE pulls as the bias behind
   the AC caps, no external bias resistors on either dev board. This is
   the same family as our TLVDS/LVCMOS33D+pulldown receiver — vendor
   precedent for a receivable single-pair pad. 1.0a4 design: single
   bidirectional pair, D-type IO (verify DK_VIDEO's 3.3V Bank-4 AUX
   port role before citing it as the LVCMOS33D-at-3.3V precedent),
   internal pulls as RX bias, board network per the DK schematic values
   sized for 0.4-1.38Vpp TX at the connector; keep our shared-pair park
   discipline (their source never receives so never faced it).
   Generational corroboration: Gowin's older (2024, 138K-kit) refdesign
   used LVCMOS18D with NO pulls on either port; the current one added
   PULL_MODE=DOWN on the receiving port — they learned the same
   bias-behind-the-caps lesson our AUX saga did. DK_VIDEO hardware puts
   source AUX on a 3.3V bank (G12/H12) so 3.3V AUX is vendor-shipped;
   the matching software project (expected LVCMOS33D) not in our
   archive — request alongside the ticket, or rely on our own round-6
   LVCMOS33D input results.

   **RESOLVED DESIGN (08-20, freeze decision — FINAL VALUES, verified
   by analysis and sim BEFORE fab; no rework or restuffing plan, and no
   0201 anywhere in the AUX network — 0402 minimum):** the 1.0a3
   network is correct in kind and stays put; the delta is purely
   additive. Existing (all stay as-is): C46/C47 0.1 µF AC caps + R31
   100k→GND (AUX_P) / R32 100k→+3V3 (AUX_N) = the DP-spec source
   presence bias, correctly on the LINE side of the caps, placed near
   the mux; R33/R34 2 M→GND on SBU1/2 (TI bleed). Measured: BTB→mux AUX
   run = **1.75 inches** — past critical length for LVCMOS-class edges,
   i.e. today's undamped line genuinely rings (why external attenuation
   "fixed" it).

   Additions, final values:
   (a) **Series 100 Ω per leg AT THE BTB PINS** (position is
   load-bearing: source termination sits at the driver end; the caps'
   position is a don't-care at 1 Mbps once the source is damped).
   CORRECTED 08-20 from an earlier 47 Ω draft — 47 Ω is NOT spec-legal
   for the LVCMOS33D fallback driver (3.3 V into 2·Rs+100 Ω gives
   1.70 Vpp > the 1.38 Vpp limit); 100 Ω gives 1.10 Vpp mid-window for
   LVCMOS and is swing-transparent to the current-mode TLVDS driver, so
   100 Ω is the one value legal for BOTH drivers with no change ever
   needed. Driver commitment: TLVDS (the configuration that reads
   converter replies today).
   (b) **Receiver-bias divider on the PAD side** (between series R and
   cap), fitted always — this is THE critical fix; the bare on-die-pull
   configuration is the one proven offset-marginal for monitor-class
   replies. Final values: **AUX_P: 100 k→3V3 / 43 k→GND (parks
   ≈0.99 V); AUX_N: 100 k→3V3 / 47 k→GND (parks ≈1.05 V)** ⇒ common
   mode ≈1.0 V (mid-window of the Gowin TLVDS input CM range — confirm
   the DS window at layout review as a check), per-leg Thevenin ≈30 k
   (<1% signal loading vs the ~100-200 Ω source impedance, stiff vs
   leakage, 100 nF cap-node recharge τ ≈ 3 ms — harmless for
   DC-balanced Manchester bursts), and a **≈65 mV standing idle offset
   with P BELOW N — sign fixed by design analysis**: the receiver reads
   logic 0 at idle, matching the DP line-idle convention (AUX_P low /
   AUX_N high via the 100 k pair) and keeping the RX shift register
   benign for the sync detector. Corroboration: the aux_interface.v
   squelch comment (08-15) documents that today's zero-differential
   idle makes the comparator CHATTER — this offset is the analog
   squelch that comment says real AUX PHYs have; the digital squelch
   demotes to belt-and-braces. The sign and levels are verified in the
   AUX sim harness pre-fab as a CHECK, and the AD2 on the item-6 test
   pads validates the built board — neither is a tuning step.
   (c) **Test pads on the LINE-side node** (item 6), one per leg — for
   validation measurement (the node the sink sees).
   (d) **Companion cst change (gateware, lands with the board): AUX
   pair PULL_MODE=NONE.** The on-die pulldown hack that biases 1.0a3
   would sit in parallel with the divider (tens of kΩ to ground),
   dragging both park levels down and shrinking the idle offset — the
   board network replaces the pulls entirely.
   Schematic review 08-20 (BTB sheet): series 100 Ω R66/R67 correct;
   pin mapping correct (G15/IOR105A→DPAUX_P, G16/IOR105B→DPAUX_N);
   ⚠ first draft had the 43 k/47 k lower resistors SWAPPED (parked P
   above N = receiver idles 1) — 43 k belongs on the DPAUX_P leg, 47 k
   on DPAUX_N. Caught against tb_aux_idle_bias's verified convention.
   This closes OPEN A — no remaining data dependency for the schematic.

2. **RECONFIG_N (ball N12) routed to an ESP32 GPIO.** Bench/dev recovery
   and automated test-reroll. NOT a consumer-facing mechanism (decision
   of record 2026-08-16: no user-level reconfiguration ever). Weak
   pull-up; ESP32 pin must be Hi-Z/open-drain at power-on (config guide:
   keep high during power-on/config; ≥25 ns low pulse = reload).

3. **Keep all four SERDES lanes routed to the mux** (as 1.0a3; uniform
   P/N swap is fine — tx_pol_invert handles it). 4-lane remains a future
   option; no change needed, just don't lose it.

3b. **138K SOM compatibility: CLOSED, SILICON-VERIFIED, NO COPPER CHANGE
   (08-20).** Ball-map audit (`SOM_138B_BALLMAP.md`): 176/180 carrier
   balls MATCH; the 4 DIFFERs are SERDES lane relabelling of the same
   pads (Q0 lanes 1↔3 transposed between dies — our pairs are die lanes
   2+3 on the 60B, 1+2 on the 138B; TX master = die lane 2 on both;
   REFCLK1 exact match; zero bank-VCCIO conflicts, the 60B's AB13 one
   disappears). Verified end-to-end same day on B3 (first 138K article):
   golden C:0177 + colorbars through the Anker hub (test log rows 54-55).
   Cost of the die difference: one per-die SERDES IP regeneration,
   config-only. Do not attempt to "fix" lane numbering in copper — the
   1↔3 transposition means every routing needs per-die IP regen anyway.

4. **VBUS source-path isolation + discharge** (evidence 08-17 eve,
   instrumented): FUSB302 reads VBUSOK=1 with the port EMPTY — the 5V
   plane backfeeds the connector VBUS rail, so the vSafe0V source-attach
   precondition can never be met honestly (root of the in-slot attach
   flakiness; 98461af3 had to remove the safety veto to attach at all).
   Fix: real load switch/ideal diode between plane and VBUS pin +
   discharge path, so VBUS is genuinely 0 V until sourced.
   **Part-requirements spec (08-20):** eFuse-class device preferred —
   one package gives all four needs: reverse blocking (kills the
   backfeed that makes VBUSOK lie), output discharge (honest vSafe0V),
   adjustable current limit ~2-3 A (bus-powered hub + downstream), and
   programmable soft-start slew (which is also half of item 5's inrush
   fix). Shortlist by stock at layout time.
5. **Local bulk capacitance on the VBUS source rail** for bus-powered
   sink inrush: hub cold-boot inrush through the source switch sagged
   the shared rail hard enough to kill ESP32 WiFi for ~1 min in-slot
   (bench supply masked it). Size for a bus-powered hub + downstream
   devices; consider soft-start on the switch. With item 4's eFuse
   slew-limiting the start, sizing is forgiving: ~330-470 µF bulk on the
   VBUS source side + local ceramics, placed so hub-side sag cannot
   couple into the ESP32's rail.

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
   (c) best-fit mechanism REFINED (08-18/19): the thermal sensitivity is
   at the sink's ACQUISITION threshold, not tracking — 1-hour warm soak
   holds rock-solid once established (receiver hysteresis: CDR pull-in/
   EQ-adapt/descrambler-sync need more eye than tracking); AUX, HPD,
   refclk all exonerated at temp. User-visible failure mode = "attach
   fails when warm", never "picture drops". Qualification metric =
   WARM-ATTACH success rate (heat system THEN attach xN), not warm
   retention. Note: the scrambler-reset fix (K28.0 per 512 BS) likely
   strengthens retention further (periodic descrambler re-anchor).
   Component-level confirmation via selective heating (mux vs cap bank)
   pending; (d) firmware lever: sweep mux RX
   EQ at WARM temperature on a good SOM (prior sweeps were on the
   SOM-limited unit) — a hotter-compensating EQ setting may buy margin
   in firmware. IR-measured package temperature to be added when
   available.
   **08-19/20 data + REFRAMING:** heatsink on B1's mux = 46→40 °C, which
   clears nothing new (Anker's acquisition bar >40 °C, Ugreen's <40 °C)
   — copper/heatsinks alone cannot save the strict-sink class. And B3
   (138K, heatsinked FPGA+mux, morning-ambient, ~90 min runtime) walked
   the full depth-gauge arc anyway at the 804 mV baseline (row 57).
   **TOLERANCE VERDICT: small thermal deltas flipping outcomes on a
   cool, heatsinked board mean the operating point sits ON the margin
   line — this is a link-budget/tolerance problem wearing a thermal
   costume.** Board actions stay (pad/via farm, copper, heatsink as
   standard fitment, keep-out) but the margin itself must come from
   drive/EQ (804→900 mV A/B in flight) + item 5a SI. Qualification must
   be margin-based (distance from the line), not pass/fail.

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
9. **Per-unit production screening test**: warm-attach against a strict
   converter sink (the margin-stack model's qualification metric). This
   is the test that would have caught SOM#1's TX deficit before it cost
   bench days. Cheap: it's the existing telemetry harness + a heat gun.
10. **Mux-region thermal probe point** (defined bare-copper spot or pad)
   — IR guns have been fighting heatsinks and laser offset all week.
11. **Recovery-ladder reality (process/firmware note, informs margin
   priority):** 08-20 evidence — a wedged hub survives QUICK power
   cycles (full PD renegotiation included) and clears only on a ~30 s
   drain. So the planned virtual-replug firmware feature (PD
   exit/re-enter) may NOT clear the worst converter wedge class; the
   only real fix is not entering marginal territory — which is items
   5a/5b's margin work. Also characterized: marginal acquisitions can
   latch a rotated frame (offset+wraparound, both axes, sink-independent
   — test log rows 49/53/56); clean catches always render true.

## OPEN questions that could still change the rev (close before Friday)

- **A. AUX network topology/values: CLOSED 08-20** — footprint-superset
  resolution written into item 1. The AD2 capture stays useful for
  choosing fitted values at bring-up but no longer gates the schematic.
- **D. DP redriver footprint: CLOSED 08-20 — NO (withdrawn).** The mux
  IS a linear redriver; no reference design cascades a second redriver
  into a redriving mux, for cause (series EQ stages amplify each other's
  ISI/noise and the settings interact), and a bypassable DNP footprint
  would add unpopulated-pad/jumper discontinuities on the main link —
  the exact defect class item 5a exists to remove. Escalation path if
  margin work ever proves insufficient: substitute a stronger SINGLE
  stage — the natural candidate is **TUSB1146** (same 4x6 WQFN-40,
  pin-compatible per family positioning ⇒ a BOM-line swap, zero copper;
  newer DP2.0/10G-rated part, so our 2.7G sits further inside its linear
  region). Caveats verified 08-20 from TI docs: its headline ADAPTIVE EQ
  is USB-path only — DP lanes get a conventional ladder ("up to 12dB @
  5GHz", same range family as the 1046A's 1.0-12.3dB); it drops alt-mode
  config F (unused by us). DP-path margin benefit is plausible but
  unproven — full-datasheet check before counting on it. Never cascade
  redrivers. First exhaust: 804→900 mV/FFE A/B (in flight) + mux EQ
  characterization at warm temperature on a good SOM.
- **E. RECONFIG_N GPIO: CLOSED 08-20 — ESP32 IO38** (non-strapping,
  Hi-Z at ESP32 boot), open-drain low-only, never push-pull high.
  **Board pull-up REQUIRED** (decision: on-board resistor, NOT
  internal-pulls-only): 10 k (4.7 k acceptable) from RECONFIG_N (ball
  N12) to the FPGA's CONFIG-BANK VCCIO (the bank rail, not blindly
  3.3 V), placed at the FPGA/BTB end of the trace. Rationale: the 25 ns
  reconfig trigger makes a weak-pull-only line a spurious-reconfig
  antenna, and the ESP32's pull is absent during its boot/reset/reflash/
  crash windows — exactly when the FPGA must ride through undisturbed.
  Optional 100 pF-1 nF at the pin for glitch filtering (1-10 µs rise is
  harmless vs the ms-scale POR timeline). Firmware: pin stays
  unconfigured until well after boot; reconfig pulse deliberately long
  (~1 ms), never near the 25 ns threshold.
- **B. Hub-path re-baseline: CLOSED 08-17 eve** — verdict 0/10 power-on
  draws (D:2A channel-EQ stall, all sw variables eliminated; Sat '4/5'
  not replicable). No SI data implicating the board channel yet — the
  stall is at training, upstream of any margin question. Items 4/5 above
  (VBUS isolation + bulk cap) are the board-level fallout. Hub-viability
  hope now rides on WS4 (item C).
- **C. Gowin IP assessment fallout: CLOSED** (WS4 + the 08-20 parity
  and 138B builds) — no pin/clock constraints we don't already meet;
  our core remains production, the Gowin stack is ticket material.

## Explicitly NOT in this rev

- Any user-facing reset/reconfig affordance (decision of record).
- HyperRAM/TransWarp endgame items (separate planned rev; see
  a2mega-hyperram-transwarp-plan).
