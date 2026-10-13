# a2mega 1.0a3 first-article bring-up log

Per-unit results for the 5 PCBWay units, following
`BRINGUP_1_0A3_PLAN.md` §5. Binary hashes are ground truth (provenance
rule). Boards labeled #1–#5; #1 = first article, #5 = untouched
reference.

## Board #1

### Stage 1 — electrical — PASS (2026-08-11)

**Correction: the SOM was fitted during these measurements** (deviation
from the runbook's no-SOM sequence; harmless in hindsight — the FPGA was
unconfigured, see Stage 3).

- Visual + unpowered short check: clean.
- Powered via USB-C from PC:
  - VBUS: **5.06 V**
  - +5V (post PTC → LM66100): **4.92 V** (~140 mV drop — nominal)
  - +3V3 (SY8089): **3.31 V**
  - Idle current: **38 mA** — initially alarming with a SOM fitted, but
    consistent with ESP32 + *unconfigured* GW5AT-60 (factory-blank SOM
    flash). Re-measure with a bitstream loaded.
- VBUS_SRC_EN (IO46): low — board not sourcing.

### Stage 2 — ESP32 / WiFi / telnet — PASS (2026-08-11)

- Firmware flashed over USB-C; no BOOT-button intervention needed on
  this unit (IO3 strap: no adverse effect observed on boot/flash so far).
- I2C: both PD chips ack (no `[usbc]` init warnings) — TUSB1046A (0x12)
  + FUSB302B (0x22) alive on IO1/IO2.
- **Two firmware bugs found and fixed on this boot** (see commits
  5c1cfc1a, 2fb9de1c): (1) WiFi/telnet was FPGA-gated in
  start_subsystems() — never came up without an A2FP link; now
  network-first. (2) `status` printed floating-bus junk as DDR3
  telemetry over a dead link ("CALIBRATED retries=254 seq=0xFE") —
  now gated on `fpga_link_ok()`.
- WiFi joined 'edhomelab' from NVS (`wifi` CLI). Telnet: pending
  explicit check (use `net` for the IP, port 23).
- Note: FPGA DONE read HIGH with an unconfigured FPGA — the ESP32
  pull-up, not the FPGA. Don't trust DONE without a link or JTAG check.

### Stage 3 — SOM + JTAG — detect + SRAM load PASS (2026-08-11)

- `openFPGALoader -c esp32s3 --detect` through the ESP32 bridge:
  **GW5AT-60, IDCODE 0x1481b** — proves TCK/TMS/TDI + relocated TDO
  (IO39) end-to-end.
- Colorbars `a2mega_dp_test.fs` SRAM-loaded via the bridge: DONE.
- Current with colorbars configured: **0.40 A** @ VBUS (~2 W) — expected
  band for ESP32 + GW5AT-60 with SERDES quad + PLLs up. Confirms the
  38 mA Stage-1 reading was the unconfigured-FPGA state; power path
  fully coherent (unconfigured 38 mA → configured 400 mA).
- led[0] heartbeat: confirmed blinking. Telnet: confirmed (console
  mirror answers on port 23, boot backlog replays).
- **Flash-write saga (~2 h, all root-caused).** First flash write used
  bare `openFPGALoader -f` instead of the validated `tools/flash.sh`
  recipe (bulk-erase + verify) → unverified corrupt image → GW5A MSPI
  boot-retry loop owned the bus: FPGA DONE stuck LOW, JTAG chain dead
  ("no device found") even across cold boots — openFPGALoader's own
  flash entry resets the fabric, re-arming the loop (the 1.0a2a-era
  `fpgaflash keepsram` comment describes this exact trap; the tooling
  for it already existed and should have been consulted FIRST).
  Recovery that worked: race an **SRAM load** against the boot window
  at replug (openFPGALoader retry loop, ~0.2 s spacing; captured on
  attempts 115/15/1) → fabric satisfied, bus free → `fpgaerase` CLI
  (header-block erase via keepsram SPI) → flash boots as blank →
  validated flash.sh write with verify → **heartbeat from flash after
  power cycle: PASS.**
- Two NEW firmware bugs found and fixed during the rescue:
  (1) `fpgaerase` bit-banged floating pins — after any USB JTAG bridge
  session the disconnect handler leaves TCK/TMS/TDI as INPUTs and the
  inline command never re-initialized them ("block 0x00000 TIMEOUT");
  (2) worse and general: while the USB-JTAG bridge is matrix-routed,
  `gpio_config()` does NOT reclaim the pads, so ALL bit-bang JTAG
  (fpgaerase, menu self-update) wiggled nothing. Fix: fpga_jtag
  init/release_pins now explicitly hand the pads between the USB-JTAG
  peripheral and GPIO (bridge released during bit-bang, restored after).
- Firmware also gained: early JTAG bridge routing at top of setup()
  (rescue window), telnet `p` key = PD status via the console tee.

### Stage 4 — PD / Alt-Mode — PASS after sink-path implementation (2026-08-11)

- First monitor attach stalled in PD state `device`: the vendored driver
  only implemented the source role (the SPEC deliberately scoped out
  monitors that present Rp — i.e. every real charging-capable USB-C
  monitor). Implemented the sink path per the SPEC's own sketch: sink
  PD contract → DR_Swap → DFP → existing VDM ladder (752b2d33; also
  fixed: FUSB302 BMC TX never enabled in sink role, hardcoded TX header
  roles, GoodCRC DATAROLE after swap).
- **PASS: `DP-ACTIVE SNK/DFP`, both cable orientations (CC1+CC2), mux
  configured (reg 0x0A=0x0A verified in-chip), HPD delivered, led[1].**

### Stage 5 — link training — IN PROGRESS (2026-08-11)

- **AUX receive is electrically unreachable on 1.0a3** — board matches
  the TI TUSB1046A reference (Fig 28) + datasheet-mandated AUX CM bias
  exactly; the reference assumes a GPU-class AUX PHY (internal RX bias
  behind own caps), which an FPGA LVCMOS pin is not. No field mod
  possible. → DP core gained BLIND_SINK open-loop link policy
  (5f83f4e3): writes transmit, replies assumed, HPD-gated, D0 wake
  added (stock core never woke the sink).
- **AUX transmit proven end-to-end by TI silicon**: mux DCI snooper
  reg 0x12 read back LANE_COUNT_SET=2 — our DPCD write, decoded off
  the wire by the TUSB1046A.
- Current state: all LEDs (HPD + link_established + video_live)
  assert on monitor attach; **no picture**. led2/3 are now blind-mode
  self-report, not sink truth. Verified correct by inspection: fabric
  lane crossover (lane0→LN3, lane1→LN2), .ipc lane/invert config
  (TXBITPOLARITYINVERT=true on Q0 L2+L3, QPLL0, REFCLK1).
- Monitor behavior: **"no signal" then sleeps** — sink never locks the
  main link (not a stream/MSA rejection). Control test: **MacBook +
  same cable + same monitor works** — monitor/cable/Alt-Mode path all
  good; fault is board-side in the main-link signal.

#### ROOT-CAUSE CANDIDATE (overnight I2C investigation, 2026-08-12)

**TUSB1046A DP receiver EQ is latched at 12.3 dB from floating straps.**
Evidence chain, each link verified:
1. PCB netlist: U11 SSEQ0/A0 (pad 11) and DPEQ0/A1 (pad 14) are
   explicitly unconnected — floated per SPEC for I2C address 0x12.
2. Datasheet Table 1: floating = 4-level "F"; the 4-level pins latch at
   reset and the strap resistors then disconnect.
3. Datasheet Table 7: DPEQ1/DPEQ0 = F/F → **EQ setting 10 = 12.3 dB at
   4.05 GHz, all DP lanes**.
4. Live register dump ('x'): 0x10 = 0x11 = 0xAA (setting 10 ×4 lanes),
   General EQ_OVERRIDE = 0 (strap-sampled EQ active).
5. Channel needs ~1 dB (few cm of PCB): ~11 dB over-equalization; the
   linear redriver re-drives a destroyed eye at 2.7 Gb/s → sink cannot
   lock — while the MacBook path (no mux) drives the monitor fine.
6. SPEC.md anticipated this: "Receiver EQ is register-settable
   (DPxEQ_SEL) instead of strap resistors" — the firmware just never
   programmed it (wrote CTLSEL/FLIPSEL/HPDIN_OVRRIDE only).

Fix committed (f5abb424, NOT yet on the board): program DPxEQ_SEL=0
(1.0 dB) + EQ_OVERRIDE before lane enable; telnet 'e' cycles EQ
0/3/6/10 live for A/B; I2C mutex (kills the 0xEE read races); PD
discovery auto-retry after VDM flakes (no more replug-to-retry).

#### ROOT CAUSE FOUND (2026-08-12, debug-plan Phase A2)

**The GTR12 transmitter was held in reset the entire bring-up.** First
live status readback over the new FPGA-debug-UART→telnet channel:
`DP S:20` = pll_lock=1, **lane_ready=00, pcs_tx_rst asserted,
tx_running=00** — the lanes never carried a single bit. The reset
sequencer in `transceiver_bank_gowin.v` waited for `pll_lock &&
lane_ready` before releasing `pcs_tx_rst`, but the GTR12's `ready_o`
does not assert while PCS is held in reset: a deadlock. The working
Sipeed SFP+ example releases both resets statically and treats
`ready_o` as status only.

Consequences: every prior content-side "elimination" (word modes 0-3,
polarity both ways, EQ effect on lock, D0, dwell) tested a dead line
and is VOID — exactly as the debug plan's "critical logical
consequence" warned. The EQ fix remains real and necessary (register
evidence), just not sufficient alone.

Fix: static reset release (~330 µs after powerup, ungated) + TX FIFO
wren gated on ~afull (working-example idiom, was constant-1) + tx_data
replicated 4× across the 80-bit bus (slice question still open until
lanes run — replication covers all cases).

#### MORNING RUNBOOK (one PC visit)
1. Board → PC. `make -C boards/a2mega/src/a2fpga_esp32 upload
   PORT=/dev/cu.usbmodem5101` (serial; JTAG uninvolved).
2. `FS=boards/a2mega/impl/pnr/a2mega_dp_test.fs tools/flash.sh a2mega`
   — flash holds the mode-1 bit-reversal experiment; this restores the
   canonical mode-0 build (already rebuilt, timing-clean).
3. Board → monitor. Expected: heartbeat + led1 (HPD) + led2 (135 MHz
   check) + led3, and — if the EQ chain is the root cause — colorbars.
4. Verify over telnet regardless: 'x' must now show 0x10=0x11=0x00 and
   General bit4 set (clean reads — mutex). If no picture: 'e' cycles EQ
   presets live; also confirm D0 snoop (0x12[6:5], anomaly still open).
- Open suspects (ranked): actual line rate out of the QPLL (verify
  clk_sym=135 MHz in-fabric vs the 50 MHz crystal — no external gear
  needed), GTR12 serialization bit-order/polarity semantics (CSR blob
  not inspectable — never verified on silicon), training dwell too
  short for slow-adapting sinks, swing/EQ/SI. Next: D0 build test →
  instrumented build (freq-check LED + longer dwells) → A/B bit-order
  and polarity builds.

### 2026-08-13 — AD3 physical-layer session, part 1: THE MUX WAS MUTING EVERY LANE / then the real split

Toolkit sanity: AD3 on the Mac; `ad3_lane_probe.py` correctly reads DEAD
on an open input (6 mV floor); Record-mode streamer pulled 2 s @ 4 MS/s
with zero lost samples; W1→CH1 loopback self-test returns the generated
1 MHz 75%-duty square at 513 mVpp/76% — the whole capture chain is proven.
(Lesson: AD3 range-relay needs ~3 s settle before capture, now in the
script; also closing the dwf handle stops the wavegen, so self-tests must
run in one session.)

Flash detour (morning): first flash attempt hit the wedged-JTAG state
(garbage IDCODE 0x120034e5), and its early attempts corrupted the flash
header → MSPI wedge, DONE low, TAP reading 0xFFFFFFFF. Recovery per the
recipe: SRAM-load race won on iteration 129 → `fpgaerase` (now requires a
live fabric; runs keepsram) → chain healthy at full speed → flash + verify
100%. Also learned: serial console boots in FORWARDING mode — '+++' to
reach the CLI; telnet console keys are single-key hotkeys (a pasted string
gets eaten as commands — undo with e/e/f).

**FINDING 1 (root-cause class, fixed): TUSB1046A AUX-snoop lane gating.**
Datasheet 8.3.2: with AUX snoop enabled (reset default), the redriver
DISABLES every DP lane until it snoops a LANE_COUNT_SET write on AUX;
unused lanes stay off to save power. Reg 0x12 read 0x00 this attach (the
TX_PROBE build runs no AUX ladder) → all four lanes muted INSIDE the mux
regardless of what the FPGA drives. Fix (firmware, register-only):
usbc_glue.cpp now writes reg 0x13=0x80 (AUX_SNOOP_DISABLE; DPx_DISABLE
defaults = all lanes enabled) right after the EQ programming on DP-mode
entry. Verified live: MUX 10-13 = 00 00 00 80, DP-ACTIVE, CC2, FLIPSEL=1.
Note: this does NOT retro-explain yesterday's certified runs (snooper had
LANE_COUNT_SET=2 then, lanes 0/1 enabled), but it retires a whole failure
class — any bitstream that doesn't complete the AUX ladder transmits into
a disabled mux; also interacts with the per-boot D:00 ladder race.

**FINDING 2 (the decisive split): mux TX alive, FPGA lane silent.**
With snoop defeated and all lanes force-enabled, full SS-pin sweep at the
USB-C breakout (both legs, 3 s settle, twice): A2/A3, A11/A10, B2/B3,
B11/B10 all ~10 mV Vpp — no probe pattern anywhere. Then the DC
measurement that splits the world: B11/B10 (= DP0 out under FLIPSEL=1 per
Table 4) sit at 1.83 V DC — the TUSB's ~1.75 V TX common-mode bias —
with 6 mV of data. An enabled linear redriver driving CM with zero data
means its INPUT is silent: DP0 ← die L3 ← GTR12 TX pads. (B2/B3 = DP2,
never connected, correctly sit at 0 V.)

Caveat being closed right now: the AD3 (~30 MHz BW) cannot see a
2.7 Gb/s stream, so the verdict requires certainty that the flashed .fs
really transmits the 1 MHz TX_PROBE pattern. The stashed
dp_lane_probe_v2.fs matched by hash but its build parameters are not
provable from the artifact (and the build's UART telemetry never
appeared — unexplained). Rebuilding TX_PROBE=1 from visible source,
reflash, re-probe B11/B10. If still silent under 1.83 V of live mux CM:
U1 closed — the GTR12 has never driven its pads, and the IP-config/CSR
path (task C, IDE regen) is the critical path.

**VERDICT (2026-08-13 afternoon): U1 CLOSED — GTR12 TX pads have never
driven.** Fresh a2mega_dp_test build (TX_PROBE=1 verified in source,
timing-clean 0/0, flash verified 100%, heartbeat confirmed): B11/B10
show DC=1.836 V (mux TX bias, alive) with only ~35 mV ripple @ ~14 MHz —
the 1 MHz/75% probe pattern is absent. An enabled linear redriver driving
CM with no data = silent input = DP0 ← L3 ← GTR12. All prior content-side
A/Bs ran on a dead line. Critical path is now the SERDES IP config
itself: task C (clean IDE regen from dp_serdes.ipc, + generate a 1.62G
RBR variant while in the GUI), plus desk-diff of our CSR vs the Sipeed
SFP+ working example for TX power/enable bits.

Open anomaly, possibly related: UART telemetry (clk50-only, pin placed,
H13 confirmed in pin report) is silent on BOTH TX_PROBE=1 builds but
streamed on all TX_PROBE=0 mode builds across two firmware versions —
a perfect correlation with no innocent mechanism identified yet. If the
TX_PROBE forced-powerup path wedges more than the SERDES (e.g. via CSR
interaction), both symptoms may share a root.

### 2026-08-13 evening — CSR bit test negative; provenance settled; IPUG1024 acquired

**Path A result: NEGATIVE.** 0x809468/0x809668 bit 6 (0x1BF->0x1FF,
matching tang_mega's active lanes) rebuilt/flashed/probed: B11/B10
unchanged (1.84 V CM, no pattern). Bit 6 is not a TX kill switch.
CSR reverted to generator-authentic 0x1BF.

**Provenance settled (user + DisplayPort_Verilog project memory):** the
whole DP stack incl. both example SERDES configs was authored by a prior
Claude instance driven by Ed; NOTHING has ever run on silicon (tang_mega
dock has no DP connector — this board is the first hardware). a2_mega
IP was genuinely GUI-generated 2026-07-19 (GUI can't reload .ipc;
from-scratch redo drifted 3 defaults, caught by sidecar diff — ALWAYS
diff after regen). Our .csr is byte-identical to that generator output.
tang-vs-a2mega CSR deltas beyond mechanical lane transposition:
0x809x68 bit6 (tested, not it), 0x808760 bit 0x400 + 0x800b91 F3->FD
(both plausibly REFCLK1 config; QPLL locks so likely correct).

**IPUG1024 read (now in boards/a2mega/docs/):** TOML loopBack="TX_ONLY"
= Operation Mode "TX Only" (legit lane-direction mode, §3.4) — CLEARED
as suspect. Loopback proper is separate (OFF/LB_NES/LB_FES/LB_ENC), ours
effectively OFF. ready_o = "TX channel status indicator" — asserted in
post-reset-fix mode builds (certified runs), so PCS layer reports alive;
break is in the PMA serializer/driver layer, CSR-configured. por_n
contract (§3.2): low after config until refclks stable, then high —
ours releases after one mgmt_clk edge (compliant-ish; margin unknown).
DRP read/write timing fully documented (§3.11, p.35) — a runtime CSR
readback/poke bridge is buildable to verify the replay actually landed.

**Next-step menu:** (B) GUI matrix session — from-scratch regen + single
-knob variants for register attribution + generate an EDPPHY (eDP TX
PHY, electrically DP) instance at 2.7G as a Gowin-blessed CSR reference;
(D) DRP readback bridge in the dp_test top (verify replay, then live
poke candidate driver bits). UART-dead-in-TX_PROBE-builds anomaly still
open (perfect correlation, no mechanism).

**Path A round 2 (magic-write ordering): NEGATIVE.** Gowin's own
Customized PHY reference design (GW5AST-138 PRBS7 demo,
/Volumes/Storage/Downloads/Gowin_Customized_PHY_RefDesign) puts the
0xb00000=0x00FFAA55 write FIRST in its CSR; our generator emits 7 writes
before it. Reordered ours unlock-first: built clean, flashed, B11/B10
unchanged (1.84 V CM, no pattern). Keeping the Gowin-blessed order (no
observed downside). Bonus from the ref design: its fabric idiom (static
pma_rstn=1/pcs_tx_rst=0, wren=~afull, clkout->tx_clk loopback) matches
our post-reset-fix code EXACTLY — the reset fix is validated against
vendor practice. Note: ref design zero-pads unused upper 80-bit-bus
words where we replicate; content-identical for probe, revisit for data.

DONE-LED flashing during problem periods (user observation): ESP32 side
is clean (GPIO48 INPUT_PULLUP, read-only) — adds ~45k parallel pull-up
to the DONE net. flash.sh retries wiggle JTAG which legitimately
resets/reconfigures (DONE drops/rises per attempt). UG718 config guide
now in boards/a2mega/docs/ for the net's pull-up requirements. Watch.

**Decision: stop bit-guessing. Next = observability.** (1) DRP readback
bridge in dp_test top — IPUG1024 §3.11 timing, dump curated CSR window
over the debug UART: answers "did the replay land" and enables live
pokes. (2) GUI session: from-scratch regen + single-knob attribution +
EDPPHY generation for a Gowin-blessed DP-rate CSR reference.

### 2026-08-13 late — ⚠️ MAJOR CORRECTION: wrong bitstream flashed ALL DAY

`tools/flash.sh a2mega` without FS= auto-picks the alphabetically-first
project — **a2mega.gprj (the full core)** — and flashed impl/pnr/a2mega.fs
on every plain invocation today. Yesterday's runbook used the FS=
override; today it was dropped. Discovered when ESP32 `status` on a
supposedly-dp_test board reported "FPGA link: UP (A2FP), DDR3 CALIBRATED,
disk slots" — none of which exist in dp_test. The only correctly-flashed
image today was mode3_inst (via FS=) — exactly the one whose UART
"mysteriously" worked. The whole UART anomaly = full core has no debug
UART on H13.

**RETRACTED:** "U1 CLOSED — GTR12 pads never driven" (the probe pattern
never reached silicon; the full core's 2.7 Gb/s traffic reads as ~35 mV
ripple in the AD3's ~30 MHz bandwidth — THE LANES MAY BE ALIVE); both CSR
experiments (bit6 0x1FF, unlock-first ordering) — never on hardware,
results VOID; the "TX_PROBE kills UART" correlation.

**STILL VALID:** TUSB1046A snoop-gating fix (verified via I2C readback),
VBUS-sourcing brownout-loop fix, AD3 toolkit + self-tests (the 1.83 V mux
CM measurement stands as a MUX-level fact), flash-corruption recovery
recipe, DRP readback bridge code, mode-build S:20-at-idle telemetry.

**Guardrail:** flash.sh now takes GPRJ= (like build.sh) and REFUSES to
guess when a board has multiple .gprj and neither GPRJ= nor FS= is given.

Reset test matrix (all on correctly-flashed dp_test builds): (1) TX_PROBE
lane probe at the breakout — pads-alive question REOPENED, now with
snoop-disabled mux; (2) DRP register dump; (3) mode build S: status with
monitor attached.

### 2026-08-13 late — ✅ PHYSICAL LAYER SIGNED OFF (first correctly-flashed probe run)

DRP dump: **24/24 registers MATCH the CSR** — replay lands perfectly
(incl. pre-magic writes; later-write-wins ordering observed working).
Lane probe at the breakout, TX_PROBE build, snoop-disabled mux, FLIPSEL=1:
- DP0 → B11/B10: 226/215 mVpp, 75%/25% duty, 0.948 µs — EXACT pattern
- DP1 → A2/A3:   228/211 mVpp, 75%/25% duty, 0.948 µs — EXACT pattern
GTR12 TX transmits; crosspoint mapping exactly per datasheet Table 4;
polarity nets TRUE end-to-end (IP tx_pol_invert correctly compensates
the board P/N swap); amplitude healthy through the redriver.

The transmitter has likely worked since the GTR12 reset fix. Yesterday's
certified no-lock runs (valid dp_test runs via FS=) therefore failed at
the CONTENT layer: word packing / 8b10b framing / scrambling / training
sequence. Word-mode 2 (symbol swap) was never hardware-tested.

Next: canonical mode-0 colorbars with snoop-disabled mux; if no image,
AUX capture on SBU (A8/B8) for the monitor's LANE0_1_STATUS CR_DONE
verdict, then word-mode 2.

### 2026-08-13 night — colorbars still dark BUT the AUX capture found a smoking gun

Colorbars attempt (correctly-flashed, certified D:2E, HLVC:111x, P:1,
lanes proven alive, snoop-disabled mux): monitor stays asleep. Fault is
at the content/protocol layer as predicted.

**B1 AUX capture (SBU1/SBU2 differential, 12 s spanning a monitor
replug): TWO findings.**
1. Our AUX TX Manchester is MALFORMED: bit cells exactly 1.0 us (rate
   correct) but the mid-cell transition sits at ~75% (0.75/0.25 us
   intervals) instead of 50%. TI's snooper tolerates it (that's how our
   writes were ever snooped); a strict sink receiver may not.
2. THE MONITOR NEVER REPLIES: 12 identical 1 Hz poll bursts (ours),
   zero reply bursts, zero HPD-IRQ pulses (E:00). Prior belief "the
   monitor has been replying all along" was an inference from the
   snooper seeing OUR writes — never actually evidence of replies.
   If the sink rejects malformed Manchester it never received ANY DPCD
   write: no LINK_BW_SET, no TRAINING_PATTERN_SET — dark screen with
   perfect lanes exactly as observed.

NEXT (morning): read hdl/displayport/auxch/aux_interface.v TX
bit-shaping, fix the 50/50 duty; recapture (expect monitor ACKs to
appear); then colorbars again. Decoder TODO: edge-timing-based
Manchester decode (quarter/three-quarter sampling fails on asymmetric
cells); UNDECODABLE guard added tonight.

Flash-ritual note: tonight's corruption loop was cleared by race →
openFPGALoader --freq 500000 --bulk-erase (clean JEDEC read, full-chip
erase) → replug → flash; serial-CLI fpgaerase was unavailable (ESP32
CDC wedge after enumeration storms — telnet stays up).

CAVEAT (same night, added after reading aux_interface.v): the 75/25
"malformed Manchester" measurement is at 4 MS/s (0.25 us grid) — a true
0.5/0.5 stream can alias to 0.75/0.25 under that quantization, and the
TX shaping RTL (50-cycle half-bit counter on clk100) looks correct by
inspection. The asymmetry claim is UNCONFIRMED until a 50 MS/s buffer
capture of one burst (5-min task, probes already on A8/B8). What stands
regardless: 12 polls, ZERO monitor replies, E:00 — the sink is not
answering, and finding out why is the morning's first job. Candidates:
AUX timing (pending precise capture), TX amplitude/CM at the monitor
side of the mux, or request framing.

### 2026-08-13 midnight — ROOT CAUSE #4 FOUND & FIXED: AUX-SBU crossbar was OPEN

50 MS/s capture: our AUX TX timing is CLEAN (1.0 us cells, ~50/50
Manchester, normal AC wander) — the 75/25 reading was 4 MS/s
quantization. TX exonerated.

THE find: SBU idle DC read 0.43/0.61 V — the AUXN 100k-to-3V3 bias never
crossed the chip. The TUSB1046A's AUX<->SBU crossbar was effectively
OPEN despite CTLSEL1=1/FLIPSEL auto-mapping; our AUX reached the monitor
only through ~5 pF of off-capacitance. The monitor never received a
single DPCD write across the entire bring-up — dark screen with perfect
lanes, zero replies, zero HPD-IRQ, all explained.

FIX (firmware): AUX_SBU_OVR forced per orientation (reg 0x13 = 0xA0 for
FLIPSEL=1: AUXp->SBU2/AUXn->SBU1) alongside snoop-disable. VERIFIED: SBU
idle DC snapped to 1.67 V (= source 100k-up ∥ sink 100k-down, the
textbook two-network midpoint — the MONITOR'S OWN BIAS is now visible)
and 2.59 V. The sink's AUX front-end is electrically attached for the
first time.

REMAINING: sink still does not ACK the 1 Hz check_link poll (msg 0x0D,
a mandatory-reply DPCD status read). Next-tier suspects: TX differential
over-swing (~±2 V measured vs DP AUX 1.38 Vpp max), differential SYNC
polarity convention, request framing. Decisive next test: B2 — MacBook
golden-reference AUX capture through the same breakout, electrical +
byte-level diff vs ours. Also: whether ladder re-ran post-fix at a fresh
attach is unconfirmed (capture window showed steady-state polls only).

**POLICY (2026-08-13, user-ratified): TUSB1046A AUX snooping stays
permanently disabled** (reg 0x13 bit7, written with AUX_SBU_OVR on every
DP-mode entry). Rationale: we own the link state machine; the snooper is
a second silent FSM inferring lane state from AUX traffic it may or may
not catch (per-attach 0x12 reset, the never-snooped D0 write, AUX-less
diagnostic bitstreams). Its only benefit — auto power savings — is
irrelevant on a slot-powered card. Future lane trimming, if wanted,
belongs in the ESP32 driving DPx_DISABLE from explicitly-communicated
link state.

### 2026-08-13/14 overnight — B2 GOLDEN CAPTURE IN HAND; analysis deferred

MacBook -> breakout -> monitor: PICTURE WORKS through the breakout (the
breakout passes a full DP link incl. SS lanes at rate — important
control). Golden AUX capture landed: 60 M samples, ~492k edges, ~2,675
bursts at the replug — the complete attach negotiation with constant
sink replies. Fragmentary live decode shows plausible ACK/NACK framing;
full byte-level + electrical diff vs our bursts DEFERRED to careful
offline analysis (late-night quick-look stats were internally
inconsistent — probe-placement doubt on the differential pair + a
windowing bug in the ad-hoc slicer; single-ended data looks rich).

Solid numeric from OUR side (aux_attach2, earlier, differential probes
verified): our AUX TX = 1.99 Vpp differential — ABOVE the DP AUX spec
ceiling (0.29-1.38 Vpp). Over-swing is now the leading suspect for the
sink ignoring us (spec-fearing receivers may squelch it), ahead of SYNC
polarity and framing. The FPGA drives AUX at LVCMOS33 full swing
pseudo-diff; attenuation options are firmware-free but fabric-side
(drive strength / IO standard on G15/G16 in the cst - e.g. lower
DRIVE, or series impedance already on board?) — design in the morning
against the golden capture's measured amplitude.

Captures preserved: boards/a2mega/captures/{aux_golden2,aux_attach2,
aux_attach}.csv.gz (gitignored, ~170 MB each). Decoder needs an
edge-timing-based Manchester pass for 4 MS/s data (quarter-cell
sampling is marginal); write offline against the golden file.

### 2026-08-14 — AUX FULLY BIDIRECTIONAL; failure isolated to CR training; in-slot source attach = separate broken workstream

**Decoder v2** (edge-timing, per-burst clock fit, tools/aux_decode2.py):
our 1 Hz poll decodes EXACTLY as designed — REQ `90 02 00 07` (native
read, DPCD 0x200, 8 bytes) — and THE MONITOR REPLIES 260 us later:
`00 41 00 00 00 00 00 00 00` = ACK, SINK_COUNT=0x41 (present),
**LANE0_1_STATUS=0x00 — CR_DONE=0**. Conclusions:
- AUX works BOTH directions through mux+breakout; v1 decoder's "sink
  never replies" was tooling blindness (its replies use a ~31-cell
  precharge, longer than ours). Amplitude fear retired (sink ACKs our
  2 Vpp fine). Polarity consistent (pol=0 decodes both parties).
- THE remaining fault: main-link training never achieves clock
  recovery. Prime suspect: blind ladder's ~1.3 ms dwells rush the sink
  (writes all land, but TPS1 hold time may be far too short before we
  advance to scrambled video). Fix candidate: lengthen blind dwells
  massively (parameter). Full closed-loop visibility now exists via
  AD3 capture incl. ADJUST_REQUEST once in training.

**In-slot experiments:** source-role attach NEVER works (user corrected:
the DP monitor's earlier "no signal" OSD was its own power cycle, not
our HPD). Observed: phantom Rd attach on empty breakout -> parked
forever in SOURCE-WAIT-REQUEST (no timeout); after unstick, no attach
against USB-C monitor (dual-source deadlock) nor DP-cable adapter (Rd
not detected — check VBUS-backfeed vs the new source guard). Firmware
backlog: Rd debounce/vRd check, SOURCE-WAIT-REQUEST timeout->retoggle,
raw FUSB302B status telnet key, **telnet HPD-pulse key** (on-demand
ladder restart = no more cable dances for retrain captures).

Captures: aux_training{,2}.csv in scratchpad; golden set preserved in
boards/a2mega/captures/.

### 2026-08-14 — ROOT CAUSE #5, IN THE MONITOR'S OWN WORDS: TX SWING TOO LOW

Dwell fix (1.3 ms -> 84 ms/state) exposed a second bug: the 0.5 s
retry_now watchdog reset the slowed ladder forever (D cycling 02..06)
-> watchdog now 4 s in blind mode. With both fixed the ladder walks
clean (D:2E, ~2.1 s) — screen still dark, CR_DONE still 0.

THE self-triggered retrain capture (new telnet 'r' key + AD3) decoded
the ENTIRE ladder conversation:
- EDID reads: monitor serves full EDID (ASCII name visible in replies)
- D0 power write (0x600): **ACKed** — the "never-snooped D0" was a TUSB
  snooper quirk, not a transmit bug (U6 closed)
- During TPS1: LANE_ALIGN_STATUS=0x80 (LINK_STATUS_UPDATED) and
  **ADJUST_REQUEST_LANE0_1 = 0x22 — the monitor requests VOLTAGE SWING
  LEVEL 2 (800 mV) on both lanes, repeatedly, all training long**
- Our ladder then writes TRAINING_PATTERN_SET=0 (training over) at the
  same 420 mV -> monitor never locks -> dark, CR_DONE=0.

Compounding gaps: blind mode's forced clock_locked_i skips the ladder's
own swing-escalation states, and the swing_0p4/0p6/0p8 outputs are not
wired to silicon anyway (GTR12 swing static in CSR: txlev=5 = 420 mV).

FIX ROUTES (both ours, no hardware): (1) STATIC — IDE regen with TX
Swing = 800 mV (DP level 2); (2) DYNAMIC — IPUG 3.11.2: the IP GUI's
"Reconfiguration" button EXPORTS a .csr of DRP writes for TX-AFE swing;
replay over our already-wired DRP port = real closed-... loop swing
control for the ladder. GUI session agenda: regen @800 mV + export
swing-level reconfig .csr set + RBR variant.

Session tally: FIVE root causes found+fixed/identified (mux snoop
gating; VBUS DRP-race brownout; wrong-gprj flash trap; open AUX-SBU
crossbar; TX swing below sink's requested level) + physical layer fully
signed off + AUX proven bidirectional with complete protocol
visibility (decoder v2 + retrain key = closed-loop debugging without a
protocol analyzer).

### 2026-08-14 — ✅✅ COLORBARS + AUDIO. STAGE 5 CORE GOAL ACHIEVED.

The swing fix landed in three parts: (1) IP regen at 804 mV (txlev 13);
(2) blind ladder DECLARES the driven level — TRAINING_LANEx_SET=0x06
(swing 2 + MAX_SWING_REACHED) in all blind voltage writes; (3) dwell
84 ms + 4 s blind watchdog. First attach: COLORBARS ON SCREEN, test
tone playing (DP audio path working end-to-end).

The monitor's own certificate (AUX poll reply, decoded):
`00 41 00 77 00 01 03 22 22` — LANE0_1_STATUS=0x77 (CR_DONE +
CHANNEL_EQ_DONE + SYMBOL_LOCKED, both lanes), LANE_ALIGN=0x01 (aligned),
SINK_STATUS=0x03 (both ports in sync), adjust satisfied at level 2.

First light for a2mega 1.0a3 and first-ever silicon success for the
DisplayPort_Verilog transmitter + blind-sink architecture. Remaining
for Stage 5 sign-off: 30-min soak, second-monitor cross-check, then
Stage 6 (full core).

**SOAK PASSED (2026-08-14 morning): 30 min, 60/60 telemetry samples,
ZERO anomalies** — D:2E pinned, HLVC:111x, frames counting, E:00 (not
one HPD edge). Monitor certificate identical at T0 and T30:
`00 41 00 77 00 01 03 22 22` (0x77 both lanes, aligned, in sync,
level 2). User-observed: colorbars rock-solid, audio playing.
**Stage 5 core: SIGNED OFF.** Remaining Stage 5 extras: second-monitor
cross-check; then Stage 6 (full core: DDR3 + 1080p scan-out — NOTE:
full-core top.sv needs the new dp_transmitter drp_dbg_* ports connected
and picks up all aux_channel fixes automatically).

**Bonus compatibility test — powered USB-C hub + DP->HDMI adapter:
BLACK SCREEN, logged as backlog.** Positives: first live validation of
the CC1/FLIPSEL=0 path end-to-end (crosspoint + straight AUX_SBU_OVR=01
mapping both correct; our ladder trains to D:2E), AUX electrically
healthy with heavy bidirectional traffic (DEFER-rich EDID relay pattern
typical of converters, incl. 2352-edge reply bursts). Unknowns: the
converter's LANE0_1_STATUS is unreadable — its burst format defeats
aux_decode2 in this orientation (170-edge frames, asymmetric leg
levels 2.35/1.53 Vpp). DP->HDMI bridges are the strictest sink class
(some require true closed-loop training). Needs its own session:
decoder work (triggered 50 MS/s captures, per-orientation thresholds)
before diagnosis. Not a Stage 5 blocker — real-monitor path signed off.

**ORIENTATION COVERAGE COMPLETE (user-tested both plug orientations on
the USB-C monitor): COLORBARS IN BOTH.** CC1/FLIPSEL=0 and CC2/FLIPSEL=1
each produce a trained link + video on the known-good sink (current
telemetry: CC1, D:2E, HLVC:111x). Crosspoint, AUX-crossbar override,
polarity compensation, and training all orientation-clean.
**Stage 5: FULLY SIGNED OFF.** Next: Stage 6 full core.

**FOLLOW-UP (user insight, 2026-08-14): differential AUX RX may be
possible on UNMODIFIED 1.0a3.** PULL_MODE alone can't fix RX (0.7 Vpp
per leg can never span LVCMOS33's 0.8->2.0 V threshold window at any
bias) — but G15/G16 are the old TMDS pins = a true die diff pair. A
TLVDS/ELVDS INPUT buffer (mV-class differential sensitivity) + weak
internal pulls (P up / N down: classic AC-coupled LVDS biasing) could
receive AUX replies for real; ELVDS_IOBUF would ALSO fix TX over-swing
(~350 mV diff, in-spec — likely what the picky DP->HDMI converter
wants). Would retire BLIND_SINK on this board rev entirely. Verify
first: (1) G15/G16 pair as a TLVDS input pair on PG484/GW5AT-60B,
(2) ELVDS/TLVDS input legality in a 3.3 V VCCIO bank on GW5A.

### 2026-08-14 — ✅ STAGE 6 BENCH MILESTONE: FULL CORE DISPLAYS OVER DP

Full core (with drp_dbg ports, OSD input reg, debug-overlay stage-B
pipeline) built at 2 setup / 0 hold (down from 21 — the two remaining
are -0.157/-0.109 routing-margin paths: cy self-increment + one packer
hop; flashed for bench with that caveat, clean-roll queued). Bench:
DDR3-backed Apple II framebuffer + OSD visible on the USB-C monitor
over the certified DP link, PD DP-ACTIVE. KNOWN GAP (deferred polish):
frame renders native-size in the 1080p raster — the integer scaler
(Phase 3b intent) isn't in the scan-out path; image + OSD small.

Build-time note: full-core PnR now takes ~35 min (register-richer
netlist) — BUILD_TIMEOUT=1800 kills healthy runs; use 3600 for the
full core. The earlier "pathological PnR / license contention" reads
were wrong: the runs were just slow. (Monitor-phase streaming instead
of tail-piped invocations makes this visible.)

Remaining Stage 6/7: one more PnR roll for 0/0; integer scaling; IIgs
slot regression — BLOCKED on the in-slot source-role PD fix (now the
critical path); 30-min full-core soak.

### 2026-08-14 — ✅ STAGE 7 CORE MOMENT: IIgs DISPLAYS FROM THE SLOT

In-slot attach fixed via four changes: 1.5 A Rp advertisement (phantom
killed — empty-connector attach gone), Source_Caps give-up (12 tries),
VCONN sourcing on source attach, VBUS-fallback sink attach (CC-measured
orientation), and REMOVAL of the VBUS source veto (slot backfeed made
it block everything; TOGSS=2 proved the toggle worked all along).
Result: board in IIgs slot, USB-C monitor through breakout —
DP-ACTIVE SNK/DFP on CC1, HPD=1, VIDEO UP. The monitor chose to source
power; our port took sink+DFP (bench-identical roles). Slot findings
recorded: VBUS node permanently backfed by slot power (VBUSOK always
1); dead-battery Rd was historically the only working attach persona —
now the toggle+fallback paths work too.

Remaining: DP-cable adapter test (true source role e2e with VCONN);
IIgs functional regression (boot, input, disk); integer scaling; one
timing-clean PnR roll; full-core soak.

**STAGE 7 DISPLAY REGRESSION: PASSED.** IIgs running (ctrl-reset beep),
OSD toggled off via telnet menu (SELECT='s') — APPLE II DISPLAY UP AND
CORRECTLY SCALED on the DP monitor from the slot. Scaling mystery
resolved: the Apple II fb rides the Phase 3b 1080p scan-out scaler
(working as designed); only the OSD draws native-size (known polish
item). Full chain live: IIgs bus -> capture -> DDR3 fb -> scaler ->
DP core -> GTR12 -> TUSB1046A -> USB-C Alt Mode -> monitor.

Remaining regression items: disk boot via ESP32 serving, keyboard/
input, slot soak, DP-cable adapter (true source role), OSD scaling
polish, timing-clean PnR roll, board #2 (Stage 8).

## AUX closed-loop RX: FINAL VERDICT — electrically unreachable on 1.0a3 (2026-08-14)

Six rounds. TX is fully proven (monitor ACKs every DPCD write; EDID I2C
request flow decoded on the wire; replies arrive ~300 us after each
request). RX is proven impossible with this board + FPGA IO:

- Root cause (circuit analysis): every Manchester burst ends with the pads
  driven to opposite rails; the 100 nF AC caps STORE that differential.
  The FPGA-side pair idles with a large locked-in offset (no DC path on
  the FPGA side to discharge it — PULL_MODE pulls are far too weak within
  the 300 us reply window). The sink's ±0.35 V differential reply never
  crosses the comparator threshold.
- Fix would be "park both legs at the same level before release" —
  requires independent per-leg drivers plus a differential receiver:
  - MIPI_IBUF has exactly this, but its HS receiver requires bank VCCIO
    1.2/1.5/1.8 V (CT1136); bank 4 is hard-wired 3.3 V. Dead.
  - Hand-composition (TLVDS_IBUF + 2x TBUF on shared pads) rejected by
    PnR (CV0013: IBUF must connect directly to a port). Dead.
  - ELVDS_IOBUF (only remaining diff-RX personality) forces P=~N. Dead.
- Empirical confirmation (round 6 instrumentation): listen-window edge
  counter (counts auxch_in transitions only >10 us after tri-state, so
  own TX can never register) stayed at 00 across hundreds of
  request/reply exchanges. Counter path itself validated in round 3
  (free-running variant churned on TX edges). Receiver output is DEAD
  QUIET while the sink talks: comparator pinned. No RTL fix exists.

Consequences:
1. Blind-sink ladder (84 ms dwell + declared 804 mV swing + watchdog)
   REMAINS the production AUX strategy for 1.0a3. Stage 5 sign-off stands.
2. 1.0a4 board rev item: add an FPGA-side AUX bias/termination network
   (e.g. 100 k divider bias per leg to ~1.5 V) so the receiver idles in
   range with zero stored differential — two resistors buy closed-loop
   training, EDID read, and link-status feedback.
3. USB-C hub / DP->HDMI converter path must be attacked blind: config
   sweep (lane count / link rate / mode) with HPD-IRQ pulses from the
   sink as the only feedback channel. Converter still receives all our
   DPCD writes (TX works); we just can't read its capabilities.

## 2026-08-15 late: PD attach regression (OPEN) — parks the AUX counter experiment

After the day's heavy attach cycling, DP alt-mode entry stopped completing.
Signature: attach detection + (sometimes) contract/DR_Swap still work
(one attach reached USB-ONLY SNK/DFP), but VDM-phase messaging ALWAYS
fails — "DP ALT MODE RESPONSE TIMEOUT" xN then "PD TRANSMISSION FAILED
DURING DP" (no GoodCRC on our own TX). Eliminated: breakout (direct cable
same), cable orientation (flip same), monitor state (power-cycled; also
HUB shows identical timeouts -> BOARD SIDE), board+monitor state machines
(full both-end power cycle same), FPGA image (VDMs don't involve FPGA).
FUSB dump with hub attached: 08:02 3C:0C 3D:01 3E:00 3F:00 40:9.
Suspects for next session: FUSB302B TX path degradation, board USB-C
connector CC contacts (exceptional insertion count today), subtle
firmware regression (3 uploads today — diff is textually benign: 'j'
key, fline[96], TEE_COLS 80). Next: register-level VDM/GoodCRC logging
in usbc_port.c; try known-good firmware build from git if in doubt.

Blocked by this: the attach-counter experiment (E: should walk +1 per
fresh attach if the virgin-caps theory holds). Round-11 evidence stands:
ONE fully-decoded reply with valid ACK header (E:11 R:00) — decoder +
polarity PROVEN; only reply visibility vs stored line offset remains.

## 2026-08-15: CLOSED-LOOP DP TRAINING WORKS — hub DP->HDMI converter shows colorbars + audio

Round 12 (native-DPCD presence + EDID skip) trained the link CLOSED-LOOP
end-to-end on the USB-C hub's DP->HDMI converter: D:2E (established),
HLVC:1111 (HPD/link/video/clk), E: counting continuously with R:00 —
live DPCD status polling, every reply decoded, every reply an ACK.
Colorbars + audio on the HDMI display behind the hub. THE HUB PATH
(previous total black-screen blocker, critical path to disk booting) IS
OPEN.

The path here, compressed:
- Rounds 1-6: receiver electrically blind (stored cap differential).
- Round 7 (user's BLVDS question): LVDS25 + pull-downs -> first edges.
- Round 9-10: decoder counters proved sync-polarity inverted; inverter
  removed -> first decoded byte.
- Round 11: R: field showed reply headers. Monitor: one ACK at attach
  then offset-blind. (Monitor path remains offset-marginal — blind
  ladder still available for it via BLIND_SINK.)
- Hub converter discovery: its properly-terminated AUX front end gives
  clean full-swing replies -> RX works CONTINUOUSLY. R:40 = I2C NACK
  (no HDMI display), then R:20 = DEFER on all DDC-class traffic.
- Round 12 root insight: check_presence was an I2C address-phase to 0x50
  (DDC-class!) — converters DEFER DDC indefinitely; ladder never passed
  step 1 ALL SESSION. Fix: presence = native DPCD read (msg 0x03 sink
  count), skip EDID states entirely (we output fixed 1080p).

Also this session: PD hot-attach regression remains OPEN (boot-with-
partner-present works — that's the workaround; VDM diagnostics now in
firmware for the hot-attach case). AUX RX FIFO noise ingestion at
zero-offset idle documented (squelch = gate sync acceptance on
awaiting-reply; future hardening).

**Round-12 stability addendum:** CC2 orientation = rock solid (continuous
D:2E, live ACKed polling, colorbars+audio). CC1 orientation = trains
closed-loop successfully but link drops/retrains ~1 Hz (converter
HPD-IRQ/lock-loss) — orientation-dependent main-link margin; add to lane
EQ tuning list (mux EQ was tuned on the monitor path). Display-asleep
behavior identified: converter cycles HPD when its HDMI sink sleeps —
ladder correctly retrains each wake (closed-loop doing its job). PD
hot-attach worked twice this session (hub attach + live flip) — the
hot-attach regression is intermittent/partner-dependent, not absolute;
VDM diagnostics remain in firmware to catch the failing case.

## 2026-08-15 end of session: hub link-stability regression (OPEN)

After the HDMI-replug experiments, the hub/converter path never returned
to the stable state of the first round-12 session (which held D:2E for
minutes with video+audio). Current behavior on BOTH orientations, through
round 13 (IRQ-vector clear — didn't cure it), hub power cycle, display
wake: ladder trains closed-loop to D:2E, holds 1-2 s, sink status shows
loss, correct retrain, repeat ~1 Hz. Screen dark — converter's HDMI TX
plausibly never starts because the link never stays up long enough.
One EQ preset step tried live ('e') — no change.

Hypotheses for next session, in order:
1. Main-link SI/EQ through the hub path (sweep all mux EQ presets
   systematically while watching stability; the working first session may
   have been on a luckier EQ/orientation combination).
2. Converter HDCP attempts with this display churning its DP side.
3. Converter state damaged by the hotplug churn (try a different
   DP->HDMI adapter or the hub's other ports).
Required instrumentation: lock-bits telemetry (clock/equ/symbol/align
from the DECODED sink status) to see WHICH condition the sink reports
losing at each drop — closed-loop gives us this for free now.

Milestone unaffected: closed-loop training + live monitoring PROVEN
(trains in ms, notices loss, retrains autonomously — all visible in
telemetry). The instability is a link-quality tuning problem, not an
AUX/protocol one.

**Round 14 + revert findings (late night):** round-12 revert cycles
IDENTICALLY -> round 13 exonerated. Squelch added to aux_interface (sync
acceptance gated on `busy`; NB rx_reset already flushes the FIFO per
transaction — noise-ingestion math can't explain every-poll failures).
Lock-bits L: field added but reads 0 ALWAYS incl. during established —
the lock signals are pulses, sampled wrong; needs latch-at-check_wait +
timeout-vs-gate-fail event counters (next instrument build).
NEW PRIME SUSPECT: THERMAL — nothing else differs from the stable
morning sessions except ~12 h of continuous powered operation; SERDES
margin drift fits trains-then-drops + morning-stable/evening-cycling.
ZERO-BUILD DISCRIMINATOR QUEUED: stone-cold board, exact boot recipe,
first thing next session. Monitor-direct non-working is EXPECTED on
closed-loop builds (offset-invisible replies) — production wants
closed-loop with blind-ladder fallback on presence timeout.

## 2026-08-15 evening: stability SOLVED as a model; two concrete findings

**The lottery model (confirmed):** link training is an EQ-convergence
lottery; a converged link is stable INDEFINITELY (15-min hands-off soak:
zero drops). "Cycling" = losing streaks of the retrain lottery;
perturbations (HDMI hotplug etc.) just force new draws. Nothing was
thermal, nothing was sticky state, no component was bad.

**Finding 1 — EQ preset odds (hard re-acquisition via mux flip-kill,
4 trials/preset):** 6.5dB=4/4, 1.0dB=3/4, 12.3dB=2/4, 9.5dB=1/4.
Firmware default now 6.5 dB (was 1.0). Soft retrains ('r' on a settled
link) converge 20/20 at ANY preset and don't even flicker the screen —
the lottery only exists at true re-acquisition.

**Finding 2 — video-restart-after-retrain BUG (blocks in-session
recovery):** any in-session retrain leaves the link perfect (D:2E, L:F,
G: clean) but the sink permanently reports SINK_STATUS=0 (K:00, "no
valid stream") until a full board reboot; boots deliver K:03 + picture.
Explains ALL "attach luck": boots whose first training attempt converged
show video; boots that retrained even once come up dark. Suspects: VB-ID
/MSA sequencing or scrambler-reset behavior on re-establishment in the
stream engine (idle_pattern_inserter already does a 64k-symbol idle dwell
and resets on channel drop — not the naive cause). NEXT: reproduce in
SIMULATION (core has testbenches; replay a mid-stream retrain, inspect
idle->video transition, VB-ID, MSA emission) — no hardware needed.

**Golden signature (healthy streaming link): L:F G:F1 K:03.**
Telemetry fields now: D: ladder state, HLVC, E:{sync,bytes} R:last-byte
L:live-locks G:{gate-locks,fails,timeouts} K:SINK_STATUS(0x205).

## Video-restart bug: elimination complete on source side (2026-08-15 night)

Round 19 (AUDIO_ENABLE=0, zero SDP packets) reproduced K:00 after
flip-kill on a pure-video stream -> SDP/audio path exonerated. Full
elimination table for the source: stream content (bit-exact sim),
idle-switch (real cadences), PHY (re-resets every pass via powerup drop),
SDP (hardware-discriminated), D3 bounce (no effect + reverted), mux-EN
bounce (no effect — and RETROSPECTIVELY INVALID: the converter sits
behind the hub's internal wiring and never saw a detach).

Remaining theory: the CONVERTER's stream pipeline wedges and only its
own power cycle clears it ("first-pass magic" = hub was freshly powered
at our boots). The clean test (K:00 -> hub-only power cycle -> video?)
was CONFOUNDED: after the hub reboot the re-attach came up on CC1 with
AUX DEAD (E: frozen, transactions timing out; flip-convention probe no
help) — a NEW sub-failure: either converter AUX silent post-reboot or a
CC1-specific crossbar/orientation gap on our side. Needs its own
session: 'x' mux reg dump on CC1 attach, PD/VDM logs, compare CC1 vs CC2
attach flows.

Round 19 also showed: audio-off boot converged FIRST-PASS (G:F1) with
colorbars — consistent with prior good boots; no evidence audio affects
convergence.

**Round 20 falsified + reverted:** DRP readback in the live K:00 state
showed ALL 24 CSR registers INTACT (24/24 match incl. the 804 mV swing)
— PMA resets do NOT wipe the UPAR/CSR analog settings; the one-way
pma_rstn release fixed nothing and made retrains jitterier. Long-dead-
window test (45 s lanes dead, then restore) also K:00 — signal-loss-
timeout reset theory dead. Video-restart elimination is now TOTAL on the
source side including analog state (verified by silicon readback, not
inference). Sole remaining axis: sink class. Discriminator queued:
BLIND_SINK=1 build + direct monitor (Stage-5 recipe) -> flip-kill ->
does MONITOR video return post-retrain? Recovers = converter-specific
quirk (document, hub boot-discipline, unblock port); dark = shared-layer
bug with a narrowed search space. Practical risk note: with EQ 6.5 dB,
boots converge first-pass ~always (G:F1); settled links soak clean —
full-core port viable under boot discipline in parallel.

## 2026-08-15 end: ROOT CAUSE CANDIDATE FOUND — TX Channel Bonding disabled

Honest boot-convergence measurement (reverted build, 5 full fresh
cycles): 2/5 lit; PERFECT correlation lit<->G:F1 (first-pass) and
dark<->retried(doom loop). No boot discipline suffices.

Unifying mechanism, backed by config evidence: **chbond_enable = false**
in dp_serdes (serdes_tmp.toml) — TX channel bonding DISABLED. The two TX
lanes' buffers free-run at arbitrary relative read phase per PCS
release: ~40% of releases land within the sink's deskew window (golden
boots — genuinely clean links, 15-min soaks pass), the rest land outside
(marginal locks, flapping polls, SINK_STATUS=0 doom loop). In-session
re-releases land correlated with the first roll -> retrains never
escape. Explains: boot lottery, doom loop, cycling, immunity to every
protocol/analog/stream fix (all exonerated: stream bit-exact sim, CSR
24/24 silicon readback, PHY sequencing both ways, audio off, D3/EN
bounces, dead-window). chbond_trigger_by_fabric=true exists — a fabric
trigger input never driven.

FIX (next session): guided GUI IP regen (same procedure as the 804 mV
swing session) enabling Channel Bonding (master channel + Read Start
Depth per IPUG1024 p.21), then drive the bonding trigger after lane
release. Acceptance: 5/5 fresh-boot cycles lit + flip-kill recovery.
Current flash: reverted round-19-equivalent (stable retrains, AUDIO OFF —
re-enable AUDIO_ENABLE=1 after the bonding fix validates).

## 2026-08-16 — EDP PHY adoption build #1

- Base commit: 8b39a9ee (+ this log entry); toolchain gw_sh V1.9.12.01
- `impl/pnr/a2mega_dp_test.fs` sha256 `878e5498704b412f0e2dae2e9cc36da5e8e7e24fd1ae27a669623f3e3b5de248`
  (18,275,231 bytes, Compress OFF, SecurityBit OFF, CRC ON)
- Timing: 0 setup / 0 hold violated endpoints (9010 analyzed);
  clk_sym 143.4 MHz actual vs 135.0 constraint; clk50/clk100/clk_pix/cm_life all pass
- Netlist audit: `edp_phy_inst` "swept" warning = benign flattening of the
  pure-wiring interposer; GTR12_QUADA FABRIC_LN2/LN3_TXDATA_I confirmed driven
  by tx_symbols word lanes 1/0 with K flags at bits 8/18, 62 constant bits each
  (60 unused + 2 invalid-bit slots) — exact expected packing
- Config swap deltas vs Customized PHY (same physical lanes 2/3): ln3 tx_if FIFO
  un-chained (self-mastered; half-bond defect gone), hardened 8b10b + RX word
  align, loopback TX_ONLY->OFF, rd_start_depth 16->8. pcs_tx_clk_src proved to
  be position-dependent plumbing (pair 0/1 emits 2, pair 2/3 emits 1 under both
  IPs) — NOT the shared-clock flag of the earlier root-cause note; the fix is
  the un-chained FIFOs + single-fabric-clock architecture + reference reset
  cadence (fabric_rstn -> tx_rst ~21 ms, tx_vld tied 1)
- Semantic delta on the wire: TPS2 initial K28.5 disparity phase no longer
  forced to RD- (period is disparity-self-sustaining; Gowin's own encoder
  ships without disparity control)
- Next: flash (IIgs OFF, board on Mac), then acceptance: 5x fresh power
  cycles, flip-kill recovery, 15-min soak; then AUDIO_ENABLE=1

## 2026-08-16 — EDP PHY build #1 slot test: FAIL (deterministic) -> build #2

- Build #1 on hardware: no colorbars; telemetry S:BF while running = TX FIFO
  almost-full asserted on EVERY running sample, across boots AND flip-kill
  retrains; sink reaches CR lock (L:8) but never symbol lock; ladder cycles.
  Deterministic — NOT the old per-boot lottery.
- Mechanism: tx_vld tied 1 (mirroring the Gowin reference) lets the fabric
  write side stuff the TX FIFO for the full 21 ms tx_rst hold (PCS read side
  stopped) -> FIFO parks at full -> continuous word drops -> undecodable
  symbol stream at the correct bit rate. Matches CR-without-symbol-lock.
- Build #2 fix: tx_vld = 3-stage sync of ~tx_rst in the fabric clock domain
  (write gate off during reset; session starts with empty FIFO in lockstep).
- Build #2: 0/0 timing violations; netlist confirms vld_sync[2] ->
  FABRIC_LN2/LN3_TX_VLD_IN. fs sha256 d263d0b0fb3538709efd74556eca5fecd0ff
  99aa36f1affbf26cc2befd709117 (SecurityBit OFF).
- Fallback if afull persists (would mean tx_vld gates the PCS read side
  instead): reference-exact reset choreography (tx_rst deasserted at
  power-up, brief pulse after fabric_rstn release, falling edge last).
- Primitive-level find: GTR12 quad exposes FABRIC_LNx_TX_DISPARITY_I —
  per-lane TX disparity control exists below the EDP PHY abstraction if the
  TPS2 initial-phase delta ever matters.

## 2026-08-16 — flash saga: content-present race pattern NAILED

Today's sequence (patched loader, single-attempt discipline, --verify always):
fail@1:22 (old image present) -> success@1:45 (chip erased by failed attempt)
-> fail@1:22 (image present) -> success@1:46 -> verify-FAIL@0:58 mid-write
(hole at byte 173919; bridge dropped a write op) -> wedge (Read ID fail 1.3s)
-> replug -> partial JEDEC ef 40 00 -> full wedge (chain empty, honest
report) -> replug -> flash_rescue SRAM erase (J:EF4017, E:B->E:D) -> replug
-> success@1:44.9 on blank chip.

- PATTERN: every failure-first had CONTENT in flash; every attempt on an
  erased chip succeeded (4/4 today incl. post-rescue). The auto-boot re-arm
  retry loop races the JTAG-SPI passthrough only when a (partial or whole)
  image gives it something to chew on. SecurityBit=OFF made this survivable,
  not gone. The 1:22 "timeout" failures are this race, not chip/bridge decay.
- Success profile is deterministic: 1:44.9-1:45.9. Fail profiles: 1:22.0x
  (race), <5s (wedge/chain), ~0:58 verify-fail (mid-write drop).
- flash_rescue erase path: deterministic, ~3 min end to end incl. replugs.
- Loader fix list (upstream + local): (1) erase-whole-image first or kill
  auto-boot before write session; (2) stretch erase-phase wait; (3) chunked
  write with readback-retry; (4) keep honest empty-chain reporting.

## 2026-08-16 — plan C acceptance: 4/5 boots, frozen-draw model refined

Un-chained Customized PHY (build d8c22f27), hub powered throughout, cable
orientation fixed logo-up, boot-with-partner-attached:
- 15-min soak on converged link: 10/10 golden (D:2E L:F G:F1 K:03). Every
  soak ever run has been clean — steady-state SI is NOT the problem.
- 5-cycle boot test: PASS/FAIL/PASS/PASS/PASS = 4/5 (baseline chained
  config measured 2/5 yesterday; n too small to call the odds improved,
  but the FAILURE MODE transformed: bad boot now trains fully and holds a
  STABLE link with clean K:00 (sink: no valid stream) instead of
  doom-loop churn — cleanly detectable).
- Bad-draw remedies tested IN-SESSION, all negative: 5x flip-kill retrain,
  mux EN bounce, hub-only power cycle (hub re-attach changed expression
  stable-dark->flap but no recovery). The frozen variable survives PMA/PCS
  resets, por_n, mux re-init, partner re-attach; re-rolls ONLY on board
  power cycle -> cornered to the GTR12 QUAD COMMON BLOCK (QPLL/divider
  init state), drawn once per powered session.
- Un-chaining did NOT remove the draw; it decoupled the doom-loop
  (retrains no longer correlated-WORSE) and made bad draws observable.
  The old chained "winning streak" on the USB-C monitor is attributed to
  that sink's generous deskew tolerance + blind mode never tearing down.
- Capture-tooling note: telnet reconnects can replay a stale buffer —
  ALWAYS verify F: (frame counter) progresses across two reads before
  trusting a "same state" conclusion.

NEXT (decision pending): (a) auto-recovery build — fabric self-reconfig
hook (Gowin CONFIG/reload primitive) commanded via ESP32 scratch reg +
both-lane txfifo wrusewd telemetry; firmware loop: K:00 after training ->
trigger reconfig (~2-3 s) -> re-check; at 80%/draw converges in ~1.25
draws. (b) closed-loop-with-blind-fallback ladder for full core (serves
monitors' invisible replies too). (c) Gowin ticket: half-bond emission,
hardened-8b10b no-symbol-lock forensics, common-block draw.

## 2026-08-16 — VERDICT: common-block draw is CONFIG-TIME-ONLY

Watchdog v2 on hardware (bad-draw boot, flap expression): W climbed 3..7 —
seven genuine 2ms held por_n resets (CMU0/1 + all CPLLs + quad POR, wiring
verified in dp_serdes.v:496-533) each followed by full retrain — K:00
through ALL seven. The quad common-block init draw does NOT re-roll under
any fabric-reachable reset; it is fixed at device configuration/wakeup.
Corroborates Gowin's own reference design tying every common-block reset
port to gw_gnd and never touching them at runtime (plaintext refdesign
audit, serdes.v:526-563; GTR12_QUADA prim_sim model is an empty stub —
silicon was the only oracle, and it has now spoken).

Watchdog v1->v2 lesson: bad draws express two ways (stable-dark K:00 with
checks passing, OR flap with ~5s teardowns); grace must accumulate across
link drops (cleared only by streaming), else the flap starves it.

RE-ROLL LEVER (the only one left in software reach): full bitstream reload.
RECONFIG_N = ball N12, NOT routed on 1.0a3 (1.0a4 candidate: route to
ESP32 alongside AUX bias resistors). Available NOW: ESP32 bit-bangs JTAG
(it is the programming bridge) — SRAM-erase re-arms auto-boot = reload
from flash (~2-3s), proven behavior from the flash saga. Architecture:
FPGA watchdog exhausts (W:7 K:00, harmless placebo pulses) -> ESP32 parses
its own UART tee -> JTAG reload -> fresh draw -> recheck; ~1.25 reloads
expected at measured 4/5 pass rate.

## 2026-08-16 — CSR replay: placebo #2. Draw is config-DOMAIN state.

Watchdog v3 on a bad-draw boot: W 1->7, each attempt = 2ms reset + full
357-write CSR replay over DRP (verified engine; same one that landed the
polarity-experiment writes) + 2ms reset + bring-up. K:00 through all
seven. VERDICT: the common-block draw is not in the registers and not
reset-reachable — it is created by the configuration wakeup process
itself. Software re-roll levers exhausted at the fabric level; only full
reconfiguration (power / RECONFIG_N / JTAG reload) re-rolls.

Scenario menu going forward:
A. Instrument the draw: both lanes' tx_if FIFO wrusewd + CMU_OK etc. in
   telemetry; correlate good/bad boots. If the bad draw shows a WORD-level
   signature, a targeted per-lane rd_start_depth DRP adjust + PCS reset
   could CANCEL it (real fabric fix, no re-roll needed). If sub-word,
   fabric is out of levers.
B. ESP32-JTAG reload firmware (pragmatic hammer): dp_test/bench contexts
   unrestricted; full core = at-boot only, bounded to ONE reload inside
   the reset-hold (~3s extra, Enhanced-precedent scale) -> 20% dark boots
   become ~4%; residual = "power cycle" guidance. NMOS reset-hold concern
   bounded by the single-reload cap.
C. Gowin support ticket (parallel): half-bond emission, hardened-8b10b
   no-symbol-lock, config-domain draw — ask for the sanctioned runtime
   re-init (a PLL recalibrate strobe may exist outside our CSR list).
D. 4-lane: ALL FOUR lanes routed on 1.0a3 (DP2=die ln1, DP3=die ln0,
   uniform P/N swap). Hub will negotiate 2-lane+USB3 (pin asgn D/F), so
   4-lane mainly serves monitor-direct (blind-ladder path) — pairs with
   the closed-loop-with-blind-fallback ladder work.
E. 1.0a4: RECONFIG_N -> ESP32 + AUX bias resistors (both queued).

## 2026-08-16 — Scenario A closes NEGATIVE: no word-level draw signature

Instrument build (U:xxyy = both lanes' tx_if FIFO wrusewd): 5 boots
sampled — 3 good (K:03, colorbars) + 2 bad (K:00) — U:0A0A on EVERY one,
rock-stable across all telemetry rows. Both lanes park at 10 words by
rd_start_depth design; fill levels carry zero draw information. The
config-time draw is SUB-WORD (serializer/divider phase below FIFO/word
granularity) — no fabric-reachable measurement or cancellation target.
(Also burned one flash cycle on a self-inflicted UART bug: MSG_LEN 67
overflowed 6-bit msg_idx arithmetic — widened to 7 bits.)

Remaining scenarios, re-weighted: B (ESP32-JTAG reload firmware) is now
the main line for resilience; C (outreach: ask Gowin for a divider-sync/
recalibration strobe — exactly the thing only they would know); D
(4-lane: different sink deskew posture may TOLERATE all draws); E
(1.0a4: RECONFIG_N + AUX bias). U: field stays in the build (harmless,
one line of decode).

## 2026-08-16 evening — 4-lane RBR detour: systematic failure; REVERT to 2-lane

- Closed-loop 4-lane vs the Anker A8365: core's power-mask correctly
  refused (sink DPCD=2 lanes). Teardown confirms hub architecture: IT6563
  (4-lane-capable silicon) fed only 2 lanes; USB3 (VL817) runs alongside;
  4K30 max. Target hub class is architecturally 2-lane -> 2-LANE HBR IS
  THE PRODUCTION CONFIG (2-lane RBR can't carry 1080p).
- Blind-4 v1 invalidated by our own watchdog (blind sinks never show
  K!=0 -> 8s teardown loop; W cycling). Fixed: watchdog gated on
  BLIND_SINK (link_established alone = healthy when blind).
- Blind-4 v2 monitor-direct: TX side perfect every boot (D:2E, 4 lanes
  streaming, W:0) — screen dark 0/4 (p~0.2% if lottery) AND dark on a
  native-DP USB2 adapter. SYSTEMATIC: our 4-lane datapath implicated
  (maiden flight of interleave/MSA4/skew; no sim coverage). Margin thesis
  remains UNTESTED. If revisited: sim-audit 4-lane stream first.
- DECISIONS OF RECORD: (1) FPGA reconfiguration is NEVER a user-level
  fix — not JTAG, not RECONFIG_N; bench acceleration only. Shipping bar =
  5/5 by construction. (2) Target hardware = commonplace USB3+2-lane-HDMI
  hubs. (3) Escalation: FFE/swing sweep at 2-lane HBR (bench-reload
  tooling first) -> full Gowin stack (Encoder+EDP PHY) on UPGRADED IDE
  (newer encoder IP; old-IDE artifacts documented) if tuning can't reach
  5/5. (4) Firmware must gain pin-assignment-D support (D-only hubs fail
  find_dp_mode today).
- Reverted: IP/defines/SDC/top byte-for-byte from tag
  a2mega-2lane-planc-4of5 (no IDE session needed); replay ROM 357; kept
  blind-gated watchdog + parameterized PLL (identical defaults) + dormant
  4-lane branch.

## 2026-08-16 late — CONTAMINATED CONCLUSION RETRACTED (firmware confound)

The monitor-direct dark-screen gauntlet tonight (blind 2-lane, faithful
pseudo-diff pad, chained IP, 1.0dB EQ, even Friday's EXACT .fs 5d8e15f0)
all stayed dark and drove toward "SOM#2 silicon variation / monitor never
lit on SOM#2." **That conclusion is not supportable** — the ESP32 firmware
was NEVER reverted. FPGA .fs was byte-identical Fri AM -> Fri PM
(5d8e15f0); only FIRMWARE changed across the good period and all weekend.
SOM#2 came into use ~same time as the Saturday firmware, so silicon and
firmware are fully confounded. Tonight's "Friday binary" test flashed only
the FPGA half; the ESP32 ran TODAY's firmware.

Firmware diff since Fri 07:46 (a77e2422) = 235 insertions across 7 files,
incl. the PD SOURCE-ROLE REWRITE (prime suspects for monitor alt-mode):
- 29f65c19 Fri PM: Rp advert 1.5A/180uA, VCONN sourcing, Source_Caps giveup
- ac183774 Fri PM: VBUS-fallback sink attach
- 98461af3 Fri PM: remove VBUS source veto
- ad471835 Sat: EQ default -> 6.5dB (tried reverting live to 1.0, no change)
- Saturday: JTAG-bridge toggle, tee width, VDM diagnostics, mux-EN bounce

FAITHFUL TEST (next session): full end-of-Friday snapshot 98461af3 —
its .fs is 5d8e15f0 (ALREADY on the chip), so ONLY need to rebuild+upload
98461af3's firmware (loses current telemetry width temporarily; restore
after). Monitor lights -> regression is Saturday firmware (bisect the PD
rework); stays dark -> then and only then suspect hardware (SOM#1 A/B).
Until this runs, make NO hardware-variation claims.

## 2026-08-16 NIGHT — BREAKTHROUGH: hardware exonerated, regression is Saturday firmware

FAITHFUL FRIDAY SNAPSHOT (FPGA .fs 5d8e15f0 already on chip + end-of-Friday
firmware 98461af3 rebuilt & uploaded, EQ back to 1.0dB default) →
**COLORBARS on the USB-C monitor, SOM#2.** Hardware fully exonerated:
SOM#2 good, monitor good, pad good, silicon-variation hypothesis DEAD.
The monitor-direct regression is entirely in the SATURDAY ESP32 FIRMWARE.

BIG CLUE: works in only ONE plug orientation. Points at CC-orientation /
FLIPSEL / mux-flip handling. Friday = works one orientation; Saturday
rework = dark both. Bisection targets (all Saturday, the good snapshot
98461af3 already includes the Friday-PM PD source-role rework):
- 881fff61 jtag-bridge toggle (JTAG pad hi-Z routing — could touch mux/flip pins?)
- d5accd46 / 7c26f650 tee width (unlikely)
- 94f28b6e VDM-phase diagnostics + FUSB dump status-only (touches FUSB reads)
- ad471835 EQ default -> 6.5dB (live-revert to 1.0 earlier didn't fix, but
  that was on today's full firmware; re-evaluate in clean bisect)
- bb11e4f3 mux-EN bounce telnet key
Plus the full-firmware commits after 98461af3 up to HEAD.

METHOD next session: git-bisect the ESP32 firmware between 98461af3 (GOOD)
and HEAD (BAD), rebuild+upload each candidate, monitor-direct one-orientation
test. FPGA untouched (5d8e15f0 stays). Board currently RUNS Friday firmware;
working tree firmware restored to HEAD (source), board fw independent.

## 2026-08-16 NIGHT #2 — blind monitor-direct is 5/5 (streak was REAL, not luck)

Friday snapshot (fs 5d8e15f0 + fw 98461af3), monitor-direct, board
MONITOR-POWERED (out of slot): 5 unplug/replug cycles → colorbars EVERY
time. Board is monitor-powered, so each cable pull = full board power-down
= FPGA RECONFIGURATION = fresh config draw. Therefore **5/5 cold config
draws all lit.** The blind-era "winning streak" was REAL (user's hypothesis
confirmed); blind 2-lane HBR monitor-direct is reliable, NOT a lottery.

REFRAMES THE BOOT-LOTTERY: the 4/5 (un-chained) / 2/5 (chained) "lottery"
was measured on the HUB path with SATURDAY firmware. This 5/5 is the
MONITOR path with FRIDAY firmware. Three variables changed at once (sink:
monitor-tolerant vs converter-strict; power: monitor-VBUS vs slot;
firmware: Fri vs Sat) — can't yet attribute. But it means the boot draw
may be HUB/sink/firmware-specific, NOT a fixed silicon tax. The all-day
"config-domain draw" silicon experiments (reset/CSR-replay immune) remain
valid AS MECHANISM, but the RATE-that-matters is sink-dependent and the
monitor tolerates it (generous deskew window, as hypothesized).

NEXT-SESSION disambiguation (besides the orientation-regression bisect):
- Re-test the HUB with FRIDAY firmware (does the hub lottery improve?)
- Boot-count the monitor path more (confirm 5/5 holds over 10+)
- If hub also improves on Friday fw → the "lottery" was partly a Saturday
  firmware artifact, and production 2-lane HBR is in far better shape than
  the afternoon's 4/5 suggested.

## 2026-08-16 NIGHT #3 — bisection is TWO personas, not one revert (user note)

User: end-of-Friday fw (98461af3) does NOT support in-slot hotplug; recalls
that working as a later iteration. Key reframe — there are TWO distinct PD
personas, separate lineages, BOTH must work:
- MONITOR-DIRECT / bench = SINK-attach (monitor powers board). Lineage:
  752b2d33(Tue, sink-attach path) -> f6f3a6d4(Thu, force AUX_SBU_OVR PER
  ORIENTATION = monitor AUX first attached; ROOT CAUSE #4). Tonight's
  orientation-dependence (one flip works) almost certainly rides on this
  per-orientation AUX_SBU_OVR — a Saturday change likely disturbed it.
- IN-SLOT HOTPLUG / product = SOURCE-attach (board powers sink in slot).
  Lineage: 29f65c19/ac183774/98461af3 (Fri PM source-role PD). 98461af3
  may predate a later (early-Sat?) refinement that made slot hotplug solid.

⇒ TRADE-OFF hypothesis: a Saturday commit FIXED in-slot source-attach while
BREAKING monitor-direct per-orientation AUX. Goal is NOT "revert to Friday"
— it's a fw state (or small fix) satisfying BOTH. Bisect watching both
signals per candidate. Files: usbc_glue.cpp (AUX_SBU_OVR/FLIPSEL/mux),
usbc_port.c (attach state machines). Board currently on 98461af3 (monitor
5/5, slot hotplug NO).

## 2026-08-16 NIGHT #4 — CORRECTION: TWO regressions, first is FRIDAY-PM (user)

My "monitor-direct regression is Saturday firmware" was WRONG/incomplete.
Coherent Friday timeline (all commits) shows a TRADE-OFF then a regression:
- 07:52 6e1d40c6: COLORBARS+AUDIO first light (our .fs 5d8e15f0 era)
- 11:52 5513d728: colorbars BOTH orientations (FLIPSEL 0 AND 1) — monitor
  fully working both flips, but IN-SLOT not yet.
- 16:47-17:15 29f65c19/ac183774/98461af3: in-slot SOURCE-ROLE PD rework
  (VCONN sourcing, Rp 1.5A, VBUS-fallback, veto removal) — the user's
  "VConn/VBUS hotplug commits."
- 17:17 15086180: "IIgs DISPLAYS FROM THE SLOT — in-slot attach fixed."
  => the Friday-PM rework FIXED in-slot but TRADED AWAY one monitor
     orientation (both->one). We run 98461af3 = one-orientation + in-slot.
- Saturday HEAD: broke the REMAINING orientation (one->zero) = all dark.

So TWO regressions: (R1) Fri-PM source-role rework, monitor 2-orient->1
(coupled to the in-slot fix, likely via AUX_SBU_OVR per-orientation vs
source-attach CC/VCONN handling); (R2) Saturday, monitor 1-orient->0.
Goal: BOTH monitor orientations AND in-slot hotplug simultaneously.
Note: .fs 5d8e15f0 is EARLY-Friday dp_test gw (pre-TLVDS, pseudo-diff) —
correct for the pseudo-diff fw path; Apple II FULL-CORE gw is a separate
bitstream (a2mega.gprj) with its own Friday fixes, not this dp_test .fs.

TOMORROW candidates: (a) build+upload ~11:52 fw (pre-16:47, e.g. tree at
5513d728) → confirm BOTH orientations reproduce (isolates R1 to the Fri-PM
rework); (b) then find minimal R1 change that keeps in-slot; (c) bisect R2
across Saturday. FPGA stays 5d8e15f0 throughout.

## 2026-08-17 morning — ROOT CAUSE: AUX overdrive needs attenuation (breakout was load-bearing)

Direct experiments (user): "dead" cable 3 + BREAKOUT inline -> COLORBARS,
BOTH plug orientations. Confirms: the 2 Vpp pseudo-diff AUX overdrive
(spec <=1.38 Vpp) is only decodable by the monitor through sufficient
channel attenuation. The USB-C breakout board was inline for ALL Friday
tests = silent inline attenuator; it was removed Saturday.

ONE cause explains the entire monitor-direct saga:
- 3 ft cable dead: shortest/least loss = worst for overdrive.
- Premium 40G 6 ft cable dead both flips: best build = least loss.
- "Good" 6 ft cable 4/5 cold: barely inside the window.
- Friday both-orientations + streak reliability: breakout padding.
- One-orientation-only without breakout: the two orientations route AUX
  through different mux crossbar paths with different loss — marginal
  signal passes one path, fails the other. NOT a firmware bug.
- Treedix all-green: continuity testers cannot see this.

RECORD CORRECTIONS:
- "Saturday firmware broke monitor orientation" (R2): UNPROVEN and likely
  FALSE — every Saturday+ monitor test changed pad (TLVDS) AND breakout
  AND firmware together. Clean test pending: HEAD fw + pseudo-diff pad +
  breakout (or cable 1) -> if lit, firmware fully exonerated, NO BISECT.
- R1 (Fri-PM "traded away one orientation"): also likely analog — Friday
  PM tests may have varied the breakout/cable, not the firmware.
- EQ was never the morning's variable (full 1.0-12.3 sweep dark on the
  low-loss cable; mux EQ is main-link RX, not AUX).
- In-slot hotplug gap on 98461af3 (user observation) REMAINS a real
  firmware question — separate from AUX, still to investigate.

FIX DIRECTION (until 1.0a4 bias network, which this PROMOTES to critical):
- Candidate: TLVDS pad (spec ~350 mV differential) — never cleanly tested
  (all TLVDS monitor tests were confounded by fw/breakout). Test TLVDS +
  known fw + no breakout + cable 3.
- Hub path tolerates the overdrive (all Saturday hub work was
  breakout-free) — product hub path less exposed, field cable variance
  still a risk until AUX is spec-compliant.
- Bench rule: cable ID + orientation + breakout-presence + EQ logged on
  EVERY test (user instituting).

## 2026-08-17 PM — WS1b COMPLETE: network flashing + telnet FPGA reload

The replug tax is gone. Full loop verified twice on B2, zero USB after a
one-time ESP32 firmware upload:

- **fpgastream** (ESP32 TCP service, port 2323): streams a Gowin .bin
  over WiFi straight into config flash via the native SPI-over-JTAG
  driver. Validates A5C3 sync + GW5AT-60 IDCODE before erasing; page
  program + readback verify + 1 retry; on OK, reloads the FPGA and
  restarts the ESP32. Client: `tools/fpga_stream.py <ip> <bin>`.
  Measured: 2,281,894 bytes in **186 s** (both runs identical), full
  page verify included.
- **telnet 'g' key**: reconfigures the FPGA from flash in ~2-3 s;
  telemetry restarts from CR 00 (fresh config-domain state — a true
  boot-draw re-roll); telnet session survives. This is the primitive for
  task #3's high-n hub boot statistics. NEVER a consumer feature
  (decision of record).
- Bug caught during bring-up: fpgastream was initially started inside
  start_subsystems(), which early-returns when the FPGA has no OSPI link
  — i.e. dead exactly when running bring-up bitstreams. Moved to setup()
  next to start_network(), unconditional (commit 928df697).
- End-to-end validation payloads: TLVDS build (re-flash of running
  image), then the **pseudo-diff bench baseline rebuild** (lottery-check
  v3 config restored: LVCMOS33 2Vpp AUX + chained IP + blind ladder;
  timing 0/0, SecurityBit OFF, bin sha256 6b97bafa, commit eea49ef0).
  B2 now runs the useful bench config.

ESP32 firmware updates still need USB (no OTA partition). Gateware
iteration is now: edit -> build -> `fpga_stream.py` -> auto-reload.

## 2026-08-17 evening — hub re-baseline VERDICT + in-slot attach root-cause leads

**Hub-path boot statistic (task #3), by construction:** 0/9 true power-on
draws on the bench (6 capture-sink + 3 monitor-sink) + 1 in-slot = **0/10
power-on draws, K:00 D:2A channel-EQ stall every time**. Saturday's "4/5"
does not replicate under ANY tested condition; every software variable was
eliminated (exact Sat gateware binary via .fs->.bin conversion, both fw
vintages incl. 98461af3 hot-attach, EQ 1.0-12.3, attach flow, HDMI sink,
reload-vs-power-cycle). User corrected the record: Sat hub tests were
bench-powered (row 2 fixed), hub cable is captive, board externally
powered via JTAG-connector 5V throughout. Honest current hub number:
**~0/10, not 4/5**; Sat count likely optimistic/miscounted (conditions
were unlogged). Harness lesson: quad-common-block draw re-rolls ONLY on
power cycle — the 30+ 'g' reload cycles confirmed the draw survives
reconfiguration (matches memory), so reload loops CANNOT measure this.

**In-slot attach failures root-cause leads (task #1), instrumented:**
- FUSB302 STATUS0=0x8x with port EMPTY = **VBUSOK asserted with no
  partner: phantom VBUS backfeed** onto the connector rail from the
  shared 5V plane (the vSafe0V precondition can never be met). Measured,
  not inferred.
- Hub attach in-slot **killed WiFi for ~1 min** (ESP32 did NOT reset —
  log backlog intact): attach-inrush rail sag degrading RF. Bench supply
  never glitched (stiffer feed). In-slot attach: 1/3 vs bench 9/9.
- 1.0a4 implications: VBUS source-path isolation (load switch/ideal
  diode, discharge FET) + local bulk cap for hub inrush.
- FW to-do (task #1): attach-state logging, backfeed-tolerant source
  attach (98461af3's approach), telnet-reachable PD restart; telnet
  console mode does NOT feed CLI input (restart cmd inert) — QoL gap.

Tooling shipped today and load-bearing throughout: fpgastream WiFi
flashing, telnet 'g' reload, .fs->.bin converter (validated byte-exact),
HDMI capture eyes (imagesnap loop in user Terminal; Claude Code app
lacks camera TCC — workaround documented), automated boot classifier.

## 2026-10-11 night — 60K regression build with the a2p25's two shared-HDL fixes (SHR scanout trigger + sdp_engine 4-lane)

Context: the a2p25 bring-up (branch `claude/a2p25-modernization-8a97cc`,
commit 24df5029) found two bugs in shared HDL that the a2mega inherits:

- `hdl/video/framebuffer_1080p.sv` — the left-border scanout trigger
  `cx0_r + 5 == h_border_px_r` can never match for a 640-wide IIgs SHR
  frame (h_border = 0, cx wraps at 2200). SHR scans out black while the
  framebuffer holds the image. The `+5` form came in with the 1.0a3 epic
  squash 0a62d8ca (08-28) ⇒ **a2mega SHR has very likely been black since
  08-28**. Fix: trigger X computed modulo the raster line (2195 for SHR,
  115 for 560-wide Apple modes — unchanged for the modes we have been
  testing, which is why nothing noticed).
- `hdl/displayport/audio/sdp_engine.v` — no 4-lane mode existed (with
  LANE_COUNT=4 it emitted the 1-lane byte table on lane 0 only). DP 1.1a
  4-lane packet-to-lane mapping added; `hdl/displayport/sim/tb_sdp_4lane.v`
  self-check PASS for 4 and 2 lanes (re-run here under iverilog: PASS/PASS).
  **a2mega note:** `boards/a2mega/hdl/top.sv` instantiates `dp_transmitter`
  with `LANE_COUNT(2)` (two die lanes on the 60K SOM), so the 4-lane path is
  pruned in this build. A direct USB-C→DP cable to a 4-lane-capable monitor
  (census second gate, 09-02) still trains as a 2-lane link on the a2mega
  side; the "4-lane audio was silent" finding is an a2p25 fact, carried here
  for parity only. 2-lane audio behaviour should be unchanged.

Cherry-pick: the three files only (`git format-patch -1 24df5029 -- <3 files>`
applied onto main ce4a0595), commit **ad88e341** on
`claude/quirky-hellman-d19038`. No a2p25 files touched.

**Build provenance (the binary hash is ground truth):**
- Base: main ce4a0595 + ad88e341 (the cherry-pick); worktree clean.
- Toolchain: gw_sh V1.9.12.01 (84072), pipe method via `tools/build.sh`,
  `GPRJ=a2mega.gprj` (60K SOM, GW5AT-LV60PG484AC1/I0), Gowin IDE GUI not
  running; waited for another session's a2p25 gw_sh to exit first (license).
- `impl/pnr/a2mega.fs` sha256 **77dcb9dfd0f9e05539577b8ae187eaa005bb41b55d753c3b4778f45dd3831d37**
  (20,723,670 bytes; header: SecurityBit OFF, CRCCheck ON, Created Sun Oct 11 22:07:51 2026)
- `impl/pnr/a2mega.bin` sha256 **d0b15355eca734fa294eb704c0ef128f8f0b0f035d416e74b0cd7c57013349be** (2,586,632 bytes)
- Resources: Logic 20304/59904 (34%), Register 20714/60780 (35%), BSRAM 108/118 (92%), DSP 2/118.
- Build wall time 14 min 52 s (21:53:10 → 22:08:02).

**Timing (single roll of the committed source — not yet "durable" per the
no-skating rule, which wants ≥3 rolls of the final source):** 0 setup / 0
hold violated endpoints, TNS 0.000 on every clock, Actual Fmax ≥ constraint
on every clock:

- clk 50.000 → 158.306 MHz
- clk4x 324.044 → 2016.128 MHz
- clk1x 80.998 → 118.888 MHz
- clk100 100.000 → 103.855 MHz
- clk_sym 135.007 → 135.869 MHz (worst slack 0.047 ns)
- clk_pix 148.500 → 151.185 MHz
- cm_life 66.667 → 154.114 MHz
- clk_logic 54.000 → 54.928 MHz
- clk_pixel: no timing paths reported (as before)

Worst setup paths (margins are the a2mega's usual knife-edge, not new):
1. 0.047 ns clk_sym `i_pixel_cdc_fifo mem DO → video_stream_packer data` (the
   known FIFO-DO cone, cf. timing_round2.md)
2. 0.051 ns clk_sym `i_sdp_engine wb_idx_b_r → wb_b_r[1]/wb_b_r[0]` (x3 in the
   top 11) — the sdp wire-byte precompute. This cone exists in the 2-lane
   form too (the 4-lane case prunes), but the loop rewrite is the change
   under test; no pre-change baseline roll was done tonight, so whether it
   tightened is unknown. Watch item.
3. 0.120 ns clk_pix `debug_overlay s2_x_bit → r_o/g_o/b_o SET` (overlay decodes)
4. 0.150 ns clk_sym `video_stream_packer line_cycle → pf_vb0` (packer fanout)
5. 0.313 ns clk_logic DOC pipelined `curr_rts_r → ram_wds_din_r`
6. 0.371 ns clk100 AUX `tx_rd_ptr → esi_off CE`

No `framebuffer_1080p` / `h_trigger` path appears in the 25 worst setup paths
(the trigger compare is now a registered 12-bit equality, as intended).

Archived copy: `a2mega_60k_ad88e341_77dcb9df.fs` (session scratchpad; the
`.fs`/`.bin` are gitignored — rebuild from ad88e341 reproduces, cf. the
08-22 determinism finding).

**Hardware status at time of writing:** NOT flashed. The ESP32-S3 JTAG chain
on this Mac reports a GW5A-25 (the a2p25), i.e. the a2mega is not attached.
Pending (needs the bench): attach the a2mega to the Mac (Apple II OFF) →
`GPRJ=a2mega.gprj tools/flash.sh a2mega` → power-cycle → on the IIgs:
(a) Arkanoid (SHR) shows its SHR screen (expected: was black on every
build since 08-28); (b) hub (2-lane) DOC/speaker audio still works;
(c) direct USB-C→DP cable to a DP monitor: audio over the 2-lane link the
a2mega trains there (the 4-lane case does not arise on this SOM).

### 10-11 night, addendum — trigger lead 5 → 11 (a9d19c90) + vertical-tracker pipelining (d15af6fd); builds 2 and 3

The a2p25 session reported a second shared-HDL fix after the entry above:
**a9d19c90** — `H_TRIGGER_LEAD` 5 → 11. The lead must cover every register
between hdmi_cx and the DP transmitter's rgb input less RGB_LATENCY (2):
framebuffer 5 + the top's composite chain 8 = 13 → 11. With 5, every 1080p
frame scanned out 6 raster px late — hidden inside the Apple modes' 120-px
borders, but on the borderless 640-wide SHR frame it is one 320-mode pixel
of border colour at the left edge (visible until the game sets the border
black). The a2mega top's chain was counted here and is identical to the
a2p25's (`fb_rgb_osd_r` 1, `osd_text_overlay` 1, `scanline_dim` 0,
`DebugOverlay` 6 with ENABLE=1 — its stages are unconditional, ENABLE only
gates the pixel colour; `overlay_en_w = 1`), so 11 holds. Apple modes move
6 px left to true centre; the ESP32 OSD text keeps its own coordinate path.

- **Build 2 = 9c703ba0** (ad88e341 + lead 11): **NOT CLEAN** — 4 setup /
  1 hold. clk_pix 145.878 < 148.5, clk_logic 53.897 < 54.0, clk1x hold
  -0.009. Violators: framebuffer vertical tracker `cy0_r → v_approach_px_r
  RESET` (-0.121) and `→ v_active_px_r CE` (-0.027), `ovl_cy_q →
  scanline_dim phase_r RESET` (-0.057), DOC `curr_rts_r → osc_state_r`
  (-0.035), DDR3 PHY cmd oserdes hold (-0.009). None is in the changed
  logic (a constant in the h_trigger subtractor) — the a2mega 60K
  knife-edge re-rolling under a changed `BUILD_DATETIME`. fs sha256
  225d019f4884f49a790af34977e3be64366136cbb889a2544574dc82e879ab46
  (archived, NOT for flashing).
- Rather than re-roll, took the a2p25's structural fix for exactly the
  worst cone: **d15af6fd** registers the vertical tracker's decision inputs
  (line-changed / cy==0 / at-border / at-lead flags), one more
  blanking-time cycle; its commit message already said "a2mega inherits
  (regression build pending)". Applied in isolation (19 lines; the module's
  port list is unchanged between main and the a2p25 tip). = **50eae999**.
- **Build 3 = 50eae999** (main ce4a0595 + ad88e341 + 9c703ba0 + 50eae999):
  **CLEAN** — 0 setup / 0 hold, TNS 0.000 every clock, Fmax ≥ constraint
  every clock. gw_sh V1.9.12.01 (84072), `GPRJ=a2mega.gprj`, SecurityBit OFF.
  - `impl/pnr/a2mega.fs` sha256 **32b860082d1469d62e18d3b814d2ad8c5d5e525eb3b071cc0a524b50014e7a5f**
  - `impl/pnr/a2mega.bin` sha256 **a43dfe4cc6ff8aafd65cd83e6db472f657f3f1632213bd1c9dc5b806ee23e846**
  - Logic 20179/59904, Register 20718/60780, BSRAM 108/118.
  - Fmax: clk 123.105/50; clk4x 2016/324; clk1x 123.058/80.998; clk100
    100.559/100; clk_sym 136.752/135.007; clk_pix 149.896/148.5; cm_life
    160.114/66.667; clk_logic 54.033/54.0.
  - Worst setup: 0.012 clk_logic DOC `curr_rts_r → ram_wds_din_r` (×2);
    0.056 clk100 AUX `next_state`; 0.063 clk_pix framebuffer `fb_width_px_r
    → lb_rd_addr CE` (×3); **0.086 clk_pix `cx0_r → h_trigger_r`** (the new
    registered 12-bit equality — now in the top 10, was absent in build 1);
    0.095 clk_sym packer `line_cycle → pf_vb0`; 0.105 clk_sym FIFO-DO →
    packer `ready`. Worst hold 0.052 clk_logic (fb wr_accum → wr_fifo).
  - Still ONE roll of the final source — same caveat as the first entry.

**Flash candidate = build 3 (50eae999, fs 32b86008…).** Still UNFLASHED:
the a2mega has not been on the Mac's JTAG chain tonight. Pending bench
list unchanged, plus: during the transition INTO SHR (before the game
blacks the border) the left edge should be flush with no border-colour
stripe; Apple modes sit 6 px further left than before.

### 10-12 — FOLLOW-UP (not in build 3): a2p25 AUX/training stack port

The a2p25 session asked for e4f2e056 (EQ status-read dwell from DPCD 0x00E
TRAINING_AUX_RD_INTERVAL: 0 → 400 µs as before, n → n × 4 ms, cap 16 ms, as
a registered advance; 16-byte capability read; default-off
RETRY_WATCHDOG_CLKS / EQ_WAIT_MULT / TRAIN_ERRCNT) plus the shared parts of
999a7492. **Declined for build 3** — it is not a cherry-pick on main:

- main's `hdl/displayport/auxch/aux_channel.v`, `channel_managemnt.v`,
  `dp_aux_messages.v`, `dp_transmitter.sv` are SEVEN commits behind the
  a2p25 branch (6d0b148d, c7064c6e, 84b65c72, 77cc1b60, 1ea23fc7,
  999a7492, e4f2e056). e4f2e056 alone does not apply.
- The stack changes `dp_transmitter`'s interface: new params FIFO_LINES,
  PREFILL_WORDS, RETRY_WATCHDOG_CLKS, EQ_WAIT_MULT, TRAIN_ERRCNT,
  LINK_CHECK, LINK_CHECK_CLKS, ECHO_SINK_ADJUST; new ports `debug_obs[103:0]`,
  `debug_auxif[23:0]`, `debug_underrun_cnt`, `debug_fifo_level`,
  `ext_symbol_clk` (in), `ext_tx_running[3:0]` (in), `ext_tx_symbols[79:0]`,
  `ext_powerup_channel[3:0]` (the D-PHY external-SerDes hooks). The a2mega
  top would need wiring for the inputs at minimum.
- It touches the closed-loop training ladder that was soak-proven on both
  dies (08-30 port) and census-validated (09-02). Changing it in the same
  bitstream as the SHR/audio regression check would confound the hub-audio
  result.

If wanted: a separate a2mega branch porting c7064c6e..e4f2e056 whole, top
wired for the new ports (a2p25's `LATE_REPLY_DRAIN=1` NOT taken unless on
the a2mega's own merits), built and A/B'd against build 3 on both hubs and
the direct cable. Expected delta per the a2p25 session: sinks with 0x00E =
0x81 (4 ms, e.g. the Sceptre) wait 16 ms in EQ instead of 1.6 ms (~15 ms
longer, still single-pass); hubs unchanged; the registered dwell form closed
at 0 violations on the GW5A-25 (the nested form was -1.2 ns at clk100 — the
a2mega's clk100 margin tonight is 0.056 ns, so watch it).
Correction from the a2p25 session (10-12): "defaults byte-identical" holds
for the PARAMETERS only — the 0x00E-derived EQ dwell is a behaviour change
even at defaults, so the port's A/B must RE-MEASURE first-draw /
time-to-picture rows (blinks-to-lock, L: counter, n ≥ 5 per build per the
08-22 lesson), not assume them from the parameter defaults.
