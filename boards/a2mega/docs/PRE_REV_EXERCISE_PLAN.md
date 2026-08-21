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

- V1. **Offset/wrap fix.** Sim agent (in flight) delivers mechanism +
  reproducer + proposed diff; user reviews; apply; re-run reproducer +
  tb_sr_highhalf + video_restart tbs; hardware-verify on B3 (marginal
  catches must render centered — or video starts made phase-deterministic
  so marginal catches cannot latch a rotated frame).
- V2. **Fold Stage-5 DP into the full core.** dp_test carries the
  production link policy (v3: plain retrain, scrambler high-half fix,
  truthful declarations, honest teardown) — the full `a2mega.gprj` core
  predates it. Port + build + verify Apple II video over DP in-slot.
- V3. **Repower reliability stats, defined bar:** ≥19/20 cold power-on
  catches to picture (Anker + monitor, ambient bench) on the full core.
  Log as test rows; this number is the go/no-go for Phase C.
- V4 (ergonomics, strongly recommended before C): fw recovery ladder —
  fix 'r' (HPD pulse > 2 ms), attempt virtual replug (PD exit/re-enter;
  may not clear the drain-only wedge — row 57 — but cheap to try), and
  document the 30 s full-drain as the last rung so a mid-session wedge
  costs seconds-to-a-minute, not a debugging detour.
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
  tolerance/level shifting — and CRITICALLY the unconfigured/config-time
  board-level defaults, which must guarantee BOTH: (a) **GS /RES held
  asserted while the FPGA is unconfigured or reconfiguring** (the
  fabric cannot drive it during config — this hold must come from
  pulls/buffer-enable defaults; it is the load-bearing element of the
  power-up choreography) and (b) every bus transceiver DISABLED so a
  half-alive card cannot fight motherboard drivers. Also: physical
  interposer hardware status (ribbon cable + socket plug — exists?
  needs fabrication?). Deliverable: `GS_SOCKET_INTERFACE.md`
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
- C4. **First drive:** real CPU out, our core in. Milestone ladder:
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
