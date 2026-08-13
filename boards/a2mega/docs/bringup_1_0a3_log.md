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
