# Pre-revision exercise plan — prove every 1.0a3 circuit before the 1.0a4 send

**Decision of record (2026-08-20):** the 1.0a4 revision does NOT ship until
every unverified circuit on 1.0a3 has been exercised. The freeze document
(`board_1_0a4_requirements.md`) is complete and parked; it gains findings
from this plan but its send date is gated on Phase C and Phase P below.

Verified so far: DP main link + AUX (exhaustively, test log rows 1-61,
both dies), ESP32/OSPI/WiFi/PD/mux, slot-bus Apple II operation (full
core ran in the IIgs; Apple II video shown over DP — that era pivoted to
the HDMI-adapter path to pursue USB 2.0 through the hub, still pending).
UNVERIFIED: (1) USB 2.0 data through the hub, (2) the 65C816 in-circuit
emulation socket interface (`FPGA_GS_*` — the ribbon/interposer circuit),
(3) the non-VBUS power-path anomaly (item 4b DMM plan).

Ordering rationale (user): steady video FIRST (CPU bring-up means endless
repowers; chasing video and CPU simultaneously is untenable), then USB 2.0
through the hub, then the 65C816. Sim-first discipline throughout.

---

## Phase V — video steadiness (the precondition)

Bar: **power-on → picture, reliably, in the full-core in-slot
configuration** — because that is the configuration the CPU work lives in.

**REORDERED 08-20 (user):** the full core takes a very long time to
build; dp_test rebuilds in ~10 min with full telemetry. So EXHAUST the
reliability exploration on the fast build first, port once, validate
once. The Phase M firmware/gateware items are absorbed here in priority
order (their full descriptions remain in Phase M below as reference).

- V1. **Offset/wrap fix — ✅ DONE 08-20** (applied 6a0eaa3f, sim-proven,
  hardware-confirmed row 62).
- V2. **Fast-build reliability program (on dp_test), in order:**
  1. Recovery-ergonomics fw (was V4): fix 'r' (HPD pulse > 2 ms) +
     virtual replug (PD exit/re-enter) — these UNLOCK the per-setting
     scans below (a fresh attach per data point stops costing a 30 s
     cable ceremony); 30 s full-drain stays documented as last rung.
  2. M1 completion: equilibrium thermal measurement + warm-attach
     count → decide default-on for the lane disable.
  3. M2: acquisition-per-setting EQ scan (full 16-step ladder,
     physical/virtual reattach per setting).
  4. M5: runtime swing/FFE via DRP — honor ADJUST_REQUEST. **BENCHED
     08-21, rows 75-77: works mechanically (975110be), escalation fixed
     by the declared-ceiling clamp (84be7590), and the INIT-apply
     perturbation fixed by trusting the boot csr (48576c12: 8/8, Y:11
     flat — cleaner than production). Production policy settled
     (design doc §11). Anker cross-check row 78 PASS (no-write path).
     USER REVIEW 08-21 night (m5_review_0821.md): items 1 (60K gprjs
     broken/unsafe → die package), 3 (commit-on-DRP-ack + ladder hold),
     4 (state retention + phy_reinit), 5 (VS+PE<=3) FIXED in 2e6013f9,
     sim-proven, builds queued, NOT flashed; item 2 per-lane = proposal
     awaiting review; item 6 apply-path validation = backlog (option:
     one-build AFE_INIT_VS=1 offset to force a real apply on A:0022).**
  5. M6: POR correctness (refclk-stable gating; reconnect fabric POR
     on the 60K emission and retest draw re-rolls).
  6. M7: bonding / inter-lane startup skew (genuinely-bonded emission
     via toml edit + CLI csr; power-up skew stats).
  7. M3: pin-assignment D/F + mux DP2 mode (closes failure class 3;
     testable on dp_test against the D/F-only hubs).
  (M4 layout audit = desk work, runs in parallel with any of these.
  Cold 804/900 A/B folds into whichever bench morning comes first.)
- V3. **Fold Stage-5 DP into the full core (was V2) — AFTER the
  reliability program**, carrying everything proven above in one port:
  link policy, TLVDS AUX front-end + cst, telemetry serializer, LED
  semantics, plus whatever V2 adds (ADJUST_REQUEST loop, POR fix,
  bonding config, lane-disable policy). Build on 1.9.12.03,
  SecurityBit OFF, timing-close the big design once.
- V4. **Repower reliability bar (was V3):** ≥19/20 cold power-on catches
  to picture (Anker + monitor, ambient) on the FULL CORE in-slot.
  Go/no-go for Phase C.
- Board roles: **B1 (60K, production v3) = the in-slot GS article**;
  B3 (138K) stays the DP margin/A/B testbed. 804/900 cold A/B continues
  opportunistically on B3 — it informs 1.0a4 margins, not this plan's
  critical path.

## Phase U — USB 2.0 through the hub (before the CPU, per decision)

Context: gamepad already works direct-attached (a2mega USB bring-up,
memory: 4 stacked causes fixed). The gap is HUB enumeration: our port is
DFP/source in 4-lane DP alt mode; D+/D- pass through to the hub's
upstream port; devices hang off the hub's downstream ports.

- U1. **Stack inventory:** ESP32-S3 host stack hub support (ESP-IDF hub
  feature / Arduino core IDF version; external-hub class driver status).
  Deliverable: what enumerates a hub on our SDK, or what must be added.
- U2. **Bench bring-up:** self-powered Anker + DP video live; milestones:
  hub descriptor read → downstream device enumerated → HID reports
  flowing (keyboard + gamepad) → wired into existing menu/gamepad paths.
- U3. **Coexistence soak:** USB2 traffic concurrent with DP video;
  watch link stability (D+/D- share the connector with the SS pairs —
  any interference shows up as C:/Y: wobble in telemetry).
- U4. **In-slot repeat** on B1. Gate: keyboard usable through the hub
  while Apple II video runs, sustained.

## Phase C — 65C816 in-circuit emulation (the board-turn gate)

Inventory (verified in-repo): pinmap `FPGA_GS_*` = A0-15, D0-7 +
GS_D_DIR, GS_PH2, GS_RW, GS_RDY + GS_RDY_OUT, GS_RESET, GS_IRQ, GS_NMI,
GS_ABORT, GS_BE, GS_VP, transceiver controls GS_DATA_~OE / GS_ADDR_~OE /
GS_~OE. RTL: `boards/a2mega/hdl/twgs/65C816/P65C816.sv` (+ALU etc.) and
`iigs_65816_wrapper.sv` (phi-phase state machine, CDC). Authority:
`twgs_reference.md` (TWGS = the architecture template).

**Topology (user-corrected 08-20): there is NO socketed CPU, ever.**
The ribbon cable plugs into the EMPTY IIgs CPU socket and carries all
socket signals to the card in the slot. Expected operation: machine
powers up → **the card holds the GS in reset while the FPGA
configures** → reset released → our core executes at the socket pins.
So CPU-vs-card contention does not exist as a category; the contention
analysis is card-vs-MOTHERBOARD (data bus during reads; PH2/RDY/IRQ
are motherboard-driven inputs — the FPI supplies PH2 into the socket
regardless of CPU presence).

- C0. **Circuit design review (paper, before power):** schematic walk of
  the GS_* transceiver chain — directions, OE polarities, 5 V
  tolerance/level shifting — and the unconfigured/config-time
  transceiver-disable defaults (a half-alive card must not fight
  motherboard drivers). The reset-hold-during-config is a WORKING card
  design (user, 08-20) — no verification needed, it is simply the
  choreography the bring-up relies on. Also: physical interposer
  hardware status (ribbon cable + socket plug — exists? needs
  fabrication?). Deliverable: `GS_SOCKET_INTERFACE.md`
  signal-by-signal.
- C1. **RTL inventory:** P65C816 core provenance/completeness;
  wrapper review against real 65816 bus timing — bank byte multiplexed
  on D0-7 during PHI1 (wrapper today shows 16-bit addr_out + dbg_addr
  [23:0]; verify the bank-byte drive path exists), RDY single-cycle
  semantics, ABORT, BE, VP. Fix list before any sim.
- C2. **Sim first:** testbench = core+wrapper against a IIgs bus model
  (PHI2 ~2.8 MHz with 1 MHz Mega-II stretch cycles, ROM image, minimal
  memory map). Milestones: reset-vector fetch ($00FFFC) → first N
  instructions match a golden trace from a software emulator → IRQ/RDY/
  stretch behavior correct. No hardware until this passes.
- C3. **Reset-parked listen first (safest circuit exercise — no CPU
  exists to snoop):** ribbon in, listen-only bitstream — all GS_* OEs
  inactive, /RES kept ASSERTED — observe PH2 (frequency/duty; the FPI
  drives it into the socket even in reset), input levels (IRQ/NMI/RDY),
  and stream them via telemetry. This exercises every level shifter
  receive path with the machine safely parked, validating the C0 paper
  review on silicon. Optional half-step before driving: release /RES
  with the bus still undriven — the motherboard fetches floating
  garbage harmlessly; confirms reset-release behavior in isolation.
- C4. **First drive:** our core takes the socket. Milestone ladder:
  (a) sane vector fetch on the bus analyzer, (b) ROM startup executes
  (border/beep activity), (c) self-test progresses, (d) boot chime +
  GS text screen (GS video is the motherboard's own — it renders if the
  CPU sets up softswitches correctly), (e) boots to ProDOS/Finder.
  Instrumentation: our DP video + telemetry as the debug surface (the
  Phase V payoff), bus-trace ring buffer readable over telnet.
- C5. **Robustness for the gate:** sustained operation at stock GS
  timing (Fast + Mega II stretch), IRQ/VBL/ADB alive, disk boot.
  **Board-turn bar = the socket CIRCUIT is proven** (every signal path
  exercised at speed, both directions, no integrity issues) — NOT full
  compatibility polish and NOT TransWarp acceleration (that is the
  HyperRAM endgame, explicitly out of scope here).

## Phase M — margin & compatibility work items (user review 08-20;
all firmware/gateware, no board change). **ABSORBED into Phase V2's
priority order 08-20** — kept here as the reference descriptions.

- M1. **Disable the two unused mux DP lanes** (DPx_DISABLE bits, reg
  0x13, after orientation known — pick the right pair per FLIP). All
  four linear channels are biased today (634 mW assumes DP4); halving
  the active DP path may cut self-heating on the very chip whose
  temperature sets acquisition margin. Measure: IR temp before/after at
  steady state + warm-attach reliability counts. One register.
- M2. **EQ acquisition-per-setting scan** (full 16-step ladder, per-lane
  where useful) using PHYSICAL reconnects per setting — the hold-sweep
  (row 63) proved tracking spans the ladder; acquisition is the
  discriminating axis. Reconcile the default-EQ history first (fw boot
  default is setting 0 = 1.0 dB; rows 55-62 corrected; check what B1's
  fw era actually defaulted to before trusting old sweep narratives).
  The 1.0-vs-6.5 empirical gap on a ~1.5 dB channel = the equalizer is
  compensating launch artifacts, not FR-4 loss (feeds 5a). Ceremony
  cost argues for doing V4's virtual-replug first.
- M3. **Pin-assignment D/F negotiation + mux DP2 mode** — firmware today
  accepts only assignment C and always sets CTLSEL=DP4; hubs offering
  only D/F never get Configure (= failure class 3, UtechSmart). Add:
  accept D/F in PD, set the mux "USB3 + 2-lane DP" General mode, FPGA
  already 2-lane. Also the doorway to Phase U USB3 coexistence.
- M4. **Layout audit vs TI rules (folds into 5a review):** pre-mux
  100 Ω / post-mux 90 Ω impedance boundary (suspect 1.0a3 routed
  uniformly), AC-cap placement/symmetry (observed: all caps sit near
  the mux — check against SLLA404 placement guidance), GND stitching
  ≤200 mil at transitions, ≤2 vias/pair, no-stub rule for any test
  structure.
- M5. **Runtime TX swing/FFE via DRP — honor ADJUST_REQUEST (the big
  one).** Gowin documents runtime txlev/FFE/Cm/C1 reconfiguration via
  DRP on the 138 (ordered .csr sequences; GUI exports them). Our design
  already has DRP machinery (upar_arbiter + CR-replay path). Concrete
  register-extraction method: DIFF the 804 mV and 900 mV .csr emissions
  (both tomls in git; serdes_toml_to_csr_138k regenerates either) — the
  differing writes ARE the swing/FFE sequence. Then the sink's
  ADJUST_REQUEST (A: field) becomes a real control loop instead of
  fixed-max-swing truthful declarations — directly attacks failure
  class 1 (EQ-never/analog margin). SIM-FIRST; test 60K equivalence
  after.

- M6. **POR correctness (user, 08-20).** (a) The Customized PHY guide
  requires refclk STABLE before releasing global POR; our logic releases
  por_n from the link power request — gate it on a verified
  refclk-stable condition instead (the freq counter / freq_ok already
  exists in fabric). (b) The 60K emission has por_toggle_by_fabric=FALSE
  (WS4: fabric POR never connected — the historical por_n "placebo
  resets") while the 138K has TRUE ⇒ every POR/retry experiment on the
  60K is suspect: the documented global SERDES reset never happened.
  Regenerate the 60K with fabric POR connected (or CSR-poke) and retest
  whether a REAL POR re-rolls boot draws — candidate contributor to the
  power-up/configuration lottery.
- M7. **TX bonding / inter-lane startup skew (user, 08-20).** Both
  active lanes run the half-bond (chbond_enable=false) — inter-lane
  startup skew is nondeterministic per power-up (independent FIFO
  read-starts), and strict converter RXes may be intolerant of the bad
  draws. Warranted: repeated-power-up lane-to-lane skew measurement,
  and/or comparison against a GENUINELY bonded emission — now feasible
  without the GUI's broken checkbox: hand-edit chbond_enable=true in
  the toml and generate the CSR via serdes_toml_to_csr_<die>k.bin
  (flag as generator-unverified; sim/bench validate). Common master +
  controlled FIFO read-start depth per the guide.

## Phase P — remaining board-turn gates

- P1. Item 4b DMM power-path characterization (slot/LM74700 and
  JTAG-5V-via-BTB with port empty; identify the leaking element) —
  gates the 1.0a4 power page.
- P2. Any schematic deltas discovered by C0 (socket-interface fixes
  would be exactly the kind of thing this plan exists to catch) get
  folded into `board_1_0a4_requirements.md`.
- P3. THEN the 1.0a4 send.

## Standing rules for this plan

One variable at a time; every hardware result gets a test-log row;
sim-first for all new RTL; no shared-RTL changes without review; builds
carry provenance; B1 = slot article, B3 = DP testbed; the 30 s hub drain
is the known recovery for wedged converter sinks.
