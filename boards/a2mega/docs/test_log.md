# a2mega DP test log

Structured record of every hardware test. **Protocol: no test result is
reported or acted on unless it has a row here.** The bringup log
(`bringup_1_0a3_log.md`) stays the narrative journal; this file is the
scannable ground truth.

## Column key

- **#**: test id (monotonic)
- **GW**: FPGA bitstream — sha256 prefix of the flashed `.fs` (git-committed)
- **FW**: ESP32 firmware — commit hash it was built from
- **LEDs** (dp_test builds; silkscreen D1-D4, all active-low; user-verified
  08-18): D1/D15=led0 heartbeat (flashing = configured). D2/D14=led1 HPD
  (lit = attach+orientation+cable good; seldom dark). D3/R14=led2 freq_ok
  135MHz (solid = bitstream running; lags D4 ~0.3-0.5s = counter window
  latency, NOT clock latency; header comment in top.sv saying
  'link_established' is STALE). D4/P14=led3 video_live — THE draw verdict,
  delivered instantly: good draw = solid right away (training completes
  <1s, before D3's window fills); bad draw = flashing (link-flap mode) or
  dark (never-trains mode). No trains-after-retries mode exists at boot;
  D4 at power-on is a complete instant classifier.
- **Board** (carrier+SOM pairs — SOMs migrated during the flash saga):
  `B1` = carrier #1 (original) + SOM #2, in service from Sat 08-16 9am
  (all Sat hub successes incl. first light 15:20 / 39ada3ca);
  `pre-switch` = carrier #1 + SOM #1 (through Fri + Sat early AM);
  `B2` = carrier #2 (fresh) + SOM #1 (revived), in service 08-17
  (all 08-17 rows; 0/12+ hub draws).
- **Sink**: `mon` (USB-C monitor direct) / `hubA` (Anker A8365 → HDMI) /
  other (add to roster below)
- **Cable**: id from the cable roster below
- **Orient**: plug orientation as *detected* (`CC1`/`CC2` from PD telemetry),
  or `?` if not captured. "logo-up" style notes go in Notes.
- **Brk**: USB-C breakout inline? `Y`/`N` — **load-bearing variable** (AUX
  attenuation), never omit
- **Pwr**: board power source — `mon` (monitor VBUS, replugs = power cycle)
  / `slot` (IIgs) / `mac` (USB bench)
- **EQ**: mux DP RX EQ in dB at test time (fw default unless cycled;
  RESETS to fw default on every board power-up)
- **Kind**: `cold` (power-on with partner attached) / `hot` (partner
  attached after boot) / `retrain` (in-session) / `soak`
- **Result**: `LIT` / `DARK` / partial; add golden-signature state for
  closed-loop builds (`K:03` etc.)

## Cable roster

| id | len | markings | Treedix CC | notes |
|----|-----|----------|-----------|-------|
| C1 | 6ft | USB3.2/USB4 Gen2 20G, 50V5A | CC1=1K, CC2=open | the "good" cable |
| C2 | 3ft | USB3.2/USB4 Gen2 20G, 20V5A | CC1=open, CC2=1K | low loss (short) |
| C3 | 6ft | USB4 Gen3 40G EPR, 20V5A | CC1=open, CC2=1K | premium/low loss |

Treedix = continuity/pin-map only; it CANNOT see channel loss or SI.
All three cables: all pins green incl. SBU; duplicate D+/D- red (normal).

## Build roster (flashed images referenced below)

| GW sha | built | config |
|--------|-------|--------|
| 5d8e15f0 | Fri 08-14 07:46 (a77e2422) | dp_test 2-lane HBR, CHAINED IP, pseudo-diff AUX, blind, 804mV. SecurityBit ON (pre-discovery) |
| d8c22f27 | Sat 08-16 | dp_test 2-lane HBR "plan C" un-chained, TLVDS AUX, closed-loop |
| 55571daf | Sun 08-16 | plan-C 2-lane + TLVDS + blind forced (lottery-check v2) |
| 74759364 | B2 | Sun 08-16 | CHAINED IP + pseudo-diff + blind (lottery-check v3 = streak replica) |

## Firmware roster

| FW | date | key properties |
|----|------|----------------|
| 98461af3 | Fri 08-14 17:15 | end-of-Friday. EQ default 1.0dB. Fri-PM source-role PD (VCONN/Rp/VBUS). Telemetry: short DP line (S/D/F/HLVC/P/E). NO: R/L/G/K/W/U fields, 'j','b' keys. In-slot hotplug: reported NOT working (user) |
| HEAD (10a4bf80+) | Sun 08-16 | EQ default 6.5dB. Full telemetry (R/L/G/K/W/U), VDM diag, jtag-toggle, muxbounce. Monitor-direct guilt UNPROVEN (confounded tests only) |

## Test table

Weekend backfill: only rows with certain provenance; `?` = not recorded
(the confounds that cost us two days). Rigor starts at #10.

| # | Board | date | GW | FW | Sink | Cable | Orient | Brk | Pwr | EQ | Kind | Result | Notes |
|---|------|-------|----|----|------|-------|--------|-----|-----|----|------|--------|-------|
| 1 | pre-switch | 08-14 AM | 5d8e15f0 | (Fri AM) | mon | ? | both | **Y** | mon? | 1.0 | cold+hot | LIT both orientations | "streak" era; 30-min soak passed; breakout inline ALL Friday |
| 2 | B1 | 08-15→16 | various | Sat/Sun | hubA | captive | n/a | N | bench (user 08-17: almost none in slot; 'slot' was mis-recorded) | 1.0→6.5 | many | mixed (2/5, 4/5, soaks clean) | closed-loop era; boot-draw model built here; conditions unlogged; 4/5 NOT reproducible 08-17 eve (row 18: 0/9 same bench) |
| 3 | B1 | 08-16 night | 55571daf | HEAD | mon | ? | ? | N | mon | 6.5 | cold ×2 | DARK 0/2 | TLVDS pad confound |
| 4 | B1 | 08-16 night | 74759364 | HEAD | ? | ? | ? | N | mon | 6.5→1.0 live | cold ×3 + EQ | DARK 0/3 | streak-replica GW but TODAY's fw |
| 5 | B1 | 08-16 night | 5d8e15f0 | HEAD | mon | ? | ? | N | mon | 6.5 | cold | DARK | "Friday binary" test — fw not reverted (invalid as Friday repro) |
| 6 | B1 | 08-16 night | 5d8e15f0 | 98461af3 | mon | C1? | one | N | mon | 1.0 | cold ×1 + replug ×5 | LIT 5/5 | BREAKTHROUGH row; one orientation only |
| 7 | B1 | 08-17 AM | 5d8e15f0 | 98461af3 | mon | C1 | ? | N | mon | 1.0 | cold ×5 | LIT 4/5 | cold morning |
| 8 | B1 | 08-17 AM | 5d8e15f0 | 98461af3 | mon | C2,C3 | both | N | mon | 1.0/6.5/9.5/12.3 | cold+EQ steps | DARK all | full EQ range dark on C3 both orientations |
| 9 | B1 | 08-17 AM | 5d8e15f0 | 98461af3 | mon | C3 | **both** | **Y** | mon | 1.0 | cold | **LIT both orientations** | ROOT CAUSE row: AUX overdrive needs attenuation |
| 10 | B1 | 08-17 | 5d8e15f0 (rewrite) | — | flash-tooling | — | — | — | mac | — | 3× flash populated | 2/3 first-attempt | loader a2mega2 SRAM-preload path; cycle-3 fail benign (no wedge) |
| 11 | B1 | 08-17 | 5d8e15f0 (rewrite) | — | flash-tooling | — | — | — | mac | — | 6× flash populated | 5/6 first-attempt | wait-only (OFL_GW5A_SETTLE_MS=3500); cycle-4 fail benign; ZERO replugs/wedges across all 9 cycles (#10+#11) |

| 12 | B2 | 08-17 | 74759364 | HEAD | flash-recovery | — | — | — | mac | — | SOM#1 revival | RECOVERED | fresh carrier (board B2) + SOM#1: ESP32 fw uploaded + WiFi provisioned (IP .116); preload-variant flash FAILED on corrupt flash (CRC err during JTAG load, ReadID fail — scanner interferes even with preload on pathological content); classic rescue path clean (JEDEC EF4017 = chip healthy, erase E:D, blank write 2:12 verify OK, auto-boot heartbeat). Saturday "SOM dead" verdict formally overturned |

| 13 | B2 | 08-17 | 74759364 | HEAD | mon | C3 | ? | **Y** | mon | 6.5 | cold | **LIT** | B2/SOM#1 first light. CLOSES: silicon-variation (dead — SOM#1 lights identical image), fresh-carrier bring-up (pass), Saturday-fw monitor guilt (exonerated — HEAD fw lights with AUX attenuation present) |

| 14 | B2 | 08-17 | c85d3afc | HEAD | mon | C3 | both | Y and N | mon | 6.5 | cold ×4 | DARK all 4 | TLVDS clean test NEGATIVE: spec ~350mV doesn't light monitor direct OR breakout (attenuating weak signal only hurts). Hub ACKed TLVDS all Saturday => sink-dependent AUX RX thresholds. Neither 1.0a3 drive is in the 0.4-1.38Vpp spec window at the connector. CLOSES 1.0a4 open item A: resistive level/bias network on the LVCMOS path (≈1V diff at connector + defined CM + RX bias), NOT the TLVDS pad |
| 15 | B2 | 08-17 PM | 74759364 | HEAD+net | hubA | ? | ? | N | hub | 6.5 | reload ×11 | K:00 all 11 | chained config on hub: AUX fine (D:2E) but ZERO symbol lock, deterministic — hub rejects chained outright (sink-dependence confirmed, high n). Via new telnet 'g' re-roll |
| 16 | B2 | 08-17 PM | d8c22f27 | HEAD+net | hubA | ? | ? | N | hub | 6.5 | reload ×10 + retrain | K:00 all | plan-C via fpgastream (exact Sat binary, .fs→.bin converted). 'g' reload alone leaves ladder idle (no HPD re-delivery — closed-loop builds need 'r' HPD pulse after reload); with 'r': training starts, collapses pre-CR |
| 17 | B2 | 08-17 PM | d8c22f27 | HEAD+net | hubA | ? | ? | N | hub | 1.0-12.3 | cold ×1 + EQ sweep | DARK, K:00 | fresh power-cycle of full chain: HPD present, training reaches channel-EQ (D:2A, L:8, G cycles) but NEVER completes, at ALL 4 EQ settings. NOTE: Sat 4/5 (row 2) was SLOT-powered — today's hub-powered bench persona was never in that dataset; persona or fw vintage = remaining deltas |
| 18 | B2 | 08-17 eve | d8c22f27 | HEAD+net | hubA | captive | n/a | N | bench 5V (JTAG conn) | 6.5 | power-cycle ×9 | BAD 0/9 (K:00, D:2A stall) | TRUE power-on draw series (quad common block re-rolls ONLY on power cycle — reload cycles in rows 15-16 re-rolled nothing). 6 cycles w/ capture-device HDMI sink + 3 w/ monitor HDMI sink: identical stall => HDMI sink exonerated. vs Sat 4/5: P(0/9|p=0.8) ~ 5e-7 => NOT the same process. Every sw variable eliminated (gw binary exact, fw both vintages, EQ all, attach flow). Remaining: slot power (supply ramp?) or Sat 4/5 miscounted. NOTE rows 15-17 'persona' framing WRONG: board was never hub-powered — external 5V via JTAG conn throughout; hub is bus-powered FROM board; hub cable is captive (C1-C3 are monitor-path only) |
| 19 | 08-18 AM | B1 | b9a908fb (39ada3ca first-success, streamed) | HEAD+net | hubA | captive | CC1 | N | hub-fed | 6.5 | power-cycle ×3 | **GOLDEN 3/3** (D:2E sustained, colorbars) | B1 cold overnight; SAME binary that went 0/12 on B2. Stall follows the ASSEMBLY (or warm/cold state — B2-cold control queued). Note B1 attaches CC1 vs B2 CC2 |
| 20 | 08-18 AM | B2 | b9a908fb (same, page-verified at write) | HEAD+net | hubA | captive | CC2 | N | hub-fed | 6.5 | power-cycle ×1 COLD | BAD (D:2A stall) | Cold control: B2 overnight-cold fails identically => thermal story DEAD. With row 19: **ASSEMBLY-DEPENDENCE CONFIRMED** (B1 3/3 golden, B2 0/13, same bits). Suspects: SOM TX margin / carrier channel+mux / BTB seating. Localizers: FFE-swing sweep on B2 (sw), cross-swap SOM1->carrier1 (hw), AD2 AUX amplitude compare |
| 21 | 08-18 AM | B2 | b9a908fb (FULL re-prep: fresh fw flash + fresh page-verified stream) | HEAD+net | hubA | captive | **CC1** | N | hub-fed | 6.5 | power-cycle ×1 | BAD (D:2A) | B2 fails on CC1 — the orientation B1 succeeds on. Re-prep rules out procedure |
| 22 | 08-18 AM | B2 | b9a908fb | HEAD+net | hubA | captive | CC2 | N | hub-fed | 6.5 | power-cycle ×1 | BAD | Orientation matrix complete: B1/CC1 3/3 golden; B2 dark on BOTH CC paths (0/16 cumulative). Crossbar hypothesis DEAD. Assembly hardware confirmed: SOM / carrier / seating. Next: FFE-swing sweep + SOM1->spare-carrier localizer |
| 23 | 08-18 AM | B1 | b9a908fb | HEAD+net | hubA | captive | CC1 | N | hub-fed | 6.5 | replug + 2 cycles | flap (9 trans/25s) -> golden-but-DARK -> hub power cycle -> **COLORBARS** | **HUB WEDGE FOUND**: hub externally powered since last night's oops = never reset; accumulates state that degrades DP->HDMI (flap, then golden-link-no-picture). B1 board side was golden throughout (38/38 D:2E stable while screen dark). Yesterday's bench series immune (bus-powered hub = auto-reset each cycle). NEW PROTOCOL: hub power cycle before every test block, or run hub bus-powered. Today's B2 rows 21-22 need fresh-hub retest |
| 24 | 08-18 AM | B2 | b9a908fb | HEAD+net | hubA | captive | ? | N | hub-fed | 6.5 | power-cycle ×1, FRESH hub | BAD (D:2A; led3/video_live dead, fabric alive+attached) | Fresh-hub control: B2 unchanged => assembly-dependence SURVIVES the hub-wedge confound. Final matrix: B1 golden+colorbars (fresh hub), B2 D:2A (fresh hub), same verified bits. Proceed to FFE/swing sweep |
| 25 | 08-18 | B2 | 04643444 (900mV) | HEAD+net | hubA | captive | ? | N | hub-fed | 6.5 | cycle ×1 | BAD D:2A | swing 804->900mV (txlev 15): NULL |
| 26 | 08-18 | B2 | 31b7ccfb (FFE 4.4dB + truthful decl + trunc fix) | HEAD+net | hubA | captive | ? | N | hub-fed | 6.5 | cycle ×1 | BAD, C:8011 | FFE pre-emphasis: NULL. But truthful TRAINING_LANE_SET declarations WOKE the hub's adjust dialogue (A:0000 -> A:0022 = swing2/preemp0 = exactly what we drive). preemp_level truncation bug found+fixed ([3:0]->[1:0] read swing bits; harmless once declarations truthful) |
| 27 | 08-18 | B2 | d377f594/e030f96a (per-lane C: + ppm Q:) | HEAD+net | hubA | captive | ? | N | hub-fed | 6.5 | cycles | BAD, C:8011 always | **VERDICT: BOTH lanes CR-only, symmetric = common-mode.** Refclk exonerated: Q = +7.5ppm (pristine). Every sw/config variable now nulled with data |
| 28 | 08-18 | B1 | e030f96a (same) | HEAD+net | hubA | captive | CC1 | N | hub-fed | 6.5 | replug attach | **GOLDEN: C:0177, G:F1, Y:11** | Reference trace: same hub request (A:0022), single-shot train, both lanes full lock, +5.2ppm crystal (2.3ppm from B2). FINAL: B2 hub stall = carrier-2 common-mode analog deficit FPGA->IT6563 (mux unit / passives / layout tolerance; refclk jitter unmeasured runner-up). NOT SOM, NOT software. Post-restart attach gap seen on B1 too (P:0 until hub replug) — task #1 |
| 29 | 08-18 | B1+B2 | e030f96a | HEAD+net | Ugreen hub, UtechSmart hub, USB->HDMI dongle, Anker | captive | - | N | JTAG 5V | 6.5 | multi-sink sweep | Anker: B1 **95%+ default-works** (rare dropouts) / B2 never; Ugreen: neither (LAYER UNKNOWN — HPD not checked); UtechSmart: neither + NO HPD (pin-assign-D suspect); dongle: neither (layer unknown) | **TWO findings, both real: (1) B1↔B2 assembly delta is LARGE (95% vs 0% on Anker — carrier-2 analog deficit stands); (2) other converters fail at a layer NOT YET CLASSIFIED — Ugreen/dongle could be PD/alt-mode (firmware) not training. NEXT: 2-min telemetry per sink on B1 classifies the layer (P:/HPD vs C:-stall) Mechanism (short reflective captive channels + bridge-grade RX vs long lossy cable + monitor RX) fits ALL data. UtechSmart no-HPD = likely the known pin-assignment-D firmware gap (find_dp_mode). WS4 (Gowin supported stack) = THE road for hub viability |
| 30 | 08-18 eve | B1 | a1a3a5a6+ | HEAD+net | Cable Matters USB->HDMI dongle | captive | CC1 | N | JTAG 5V | 6.5 | attach | CLASS 2: PD+HPD OK, ladder loops ERROR state (D:00) at DPCD presence read; AUX reply traffic present | Suspect: DEFER handling — ladder may treat legal DEFER replies as failure. Firmware/RTL fix candidate |
| 31 | 08-18 eve | B1 | a1a3a5a6 (check non-fatal) | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | attach | CLASS 4 verdict: GENUINE post-train loss, not phantom teardown (non-fatal held stream; C:8000 = sink reports ZERO locks; monitor 'no signal'). Locks TPS (C:8177) -> dies at scrambled normal mode | Old flap loop = correct ladder response |
| 32 | 08-18 eve | B1 | 36f44072 SCRAMBLER FIX | HEAD+net | Anker, then Ugreen | captive | - | N | hub-fed | 6.5 | regression + retest | Anker GOLDEN (C:0177 Y:11 — no regression); Ugreen STILL C:8000 | High-half SR fix real+sim-proven (NO K28.0 ever transmitted in 1080p builds before) but not Ugreen's killer. X:91 both hubs: ext-framing capable, downspread capable |
| 33 | 08-18 eve | B1 | 8166b65e downspread-truth | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | retest | STILL C:8000 | Truthful SSC declaration also null. Class-4 remaining: enhanced-framing-required (RTL project: 4-symbol sequences not implemented) / broadband margin (WS4) / deeper content — end-to-end post-scrambler verification harness is the systematic road |
| 34 | 08-18 night | B1 | 61ab95d6 (NEW toolchain 1.9.12 + SERDES 1.2 emission) | HEAD+net | Anker | captive | - | N | hub-fed | 6.5 | attach | **REGRESSION**: D3+D4 dark 20+s (symbol clock stalled at bring-up), then trains once, sink degrades to C:8001 (was C:0177) | Two-variable confound — split below |
| 35 | 08-18 night | B1 | 5b4c8e04 (NEW toolchain + 1.1 emission restored) | HEAD+net | Anker | captive | - | N | hub-fed | 6.5 | attach (hub repower needed) | **GOLDEN C:0177 ×37, G:F1, Y:11** | VERDICT: toolchain 1.9.12 CERTIFIED (clean timing, better Fmax margin, hw parity); **SERDES 1.2 emission = hardware regression** (20s bring-up stall + post-train degradation; likely its por_toggle_by_fabric flip + 0xc00008 3->1). Production stance: 1.1 emission + either toolchain. 1.2's fabric-POR lead needs surgical extraction, not wholesale adoption. Half-bond unfixed in 1.2 (Gowin ticket, 2 versions) |
| 36 | 08-18 late | B1 | 0d893690 prod-candidate, then ARCHIVED 5b4c8e04 (new TC) + 36f44072 (old TC) via git .fs->.bin | HEAD+net | Anker | captive | - | N | hub-fed | 6.5 | many replugs + power cycles | ALL THREE DEAD on late-night bench state (trains to C:8177 every round, sink drops ALL locks at video start; D3 blink = teardown echo). Same 5b4c8e04 bits were GOLDEN C:0177x37 ~1h earlier | **Software 100% exonerated by construction** (both toolchains, three binaries). Bench state drifted over hours of operation: prime suspect = un-heatsinked CARRIER MUX (TUSB1046A) thermal drift (SOM heatsink present on B1 all along); also hub warmth, seating wear. MORNING PROTOCOL: cold-boot the same 36f44072 bits untouched — colorbars=thermal (then localize w/ selective cooling), dead=seating. Retrain-restored build 0d893690 shows battle-loop on bad instances (correct behavior); non-fatal diagnostic reverted for production |
| 37 | 08-18 late | B1 | 36f44072 | HEAD+net | Anker | captive | - | N | hub-fed | 6.5 | **TUSB1046A cooled w/ compressed air** + replug | **GOLDEN C:0177 G:F1 Y:11 + COLORBARS** | **MUX THERMAL MARGIN CONFIRMED AT COMPONENT LEVEL**: one chip cooled = triply-dead bench -> golden. Reframes: (a) hub-path 'boot lottery' substantially = mux temperature at attach; (b) B1-vs-B2 delta plausibly per-unit mux thermal margin (B2 below line even cold, B1 above-cold/below-hot); (c) sink-strictness ladder rides on top (stricter sink = less thermal headroom). 1.0a4 CRITICAL: mux thermal design (thermal pad/via farm/airflow). Bench: heatsink both muxes NOW. QUEUED: Ugreen + cooled mux (class 4 may be thermal-conditional!), B2 + cooled mux (0/16 may flip!) |
| 38 | 08-18 late | B1 | 36f44072 | HEAD+net | **Ugreen** | captive | - | N | hub-fed | 6.5 | mux cooled + attach | **GOLDEN C:0177 G:F1 Y:11 + COLORBARS — CLASS 4 SOLVED** | The all-day 0-for-everything sink works on a cooled mux. Class 4 = class 1 with a stricter RX (less thermal headroom). Taxonomy collapses to: ONE analog axis (per-unit mux thermal margin × temperature) × per-sink RX strictness, PLUS two separate firmware gaps (Cable Matters AUX-DEFER, UtechSmart pin-assign-D). Scrambler/downspread fixes remain correct (spec compliance) but were not the blocker. NEXT: B2 + cooled mux |
| 39 | 08-18 late | B2 | e030f96a (in flash) | HEAD+net | Anker | captive | - | N | hub-fed | 6.5 | cold board, mux COOL to touch | BAD C:8011 (classic B2 stall) | **B2 deficit is temperature-INDEPENDENT** — fails with verifiably cool mux. Final model: B1 variance = mux-region thermal axis (cool=golden incl Ugreen; warm=video-start death); B2 = deeper per-unit deficit, same symptom family, bigger magnitude (suspects: voided thermal pad [die hot despite cool case — discriminate via rail current + warm-up rate vs B1], weak mux unit, caps, BTB seating). B1 mux was VERY HOT before air blast — abnormal for ~400mW part: voiding or electrical stress on B1 too. Morning: rail-current compare B1 vs B2, mux warm-up race, selective heating (mux vs caps) |
| 40 | 08-18 late | **B3 = carrier#3 + SOM#1** | 0d893690 prod-candidate | HEAD+net (crash-fix 1f56345e) | Anker | captive | - | N | hub-fed | 6.5 | first attach, factory-cold mux | BAD C:8011 (the SOM#1 signature, exactly) | **CROSS-SWAP VERDICT: DEFICIT FOLLOWS THE SOM.** Carrier 2 exonerated. SOM#1 = per-unit SERDES TX margin deficit (EQ-stall vs bridge sinks; monitors' tolerant RX unaffected) — silicon corner or wear from the flash-chaos/reseat saga. Final taxonomy: (1) per-SOM TX margin [SOM1 bad, SOM2 good]; (2) per-carrier mux-region thermal axis [carrier1 mux ran VERY HOT = voiding suspect] modulating good SOMs; (3) sink strictness ladder multiplies both; (4) fw gaps: DEFER (Cable Matters), pin-D (UtechSmart). SOM1 = quarantine from hub work (still fine for monitor-path/dev). New 60K SOMs ~2 weeks. 1.0a4/production: add per-unit hub-path screening test |
| 41 | 08-19 | B1 | 36f44072 | HEAD+net (1f56345e) | Anker | captive | - | N | hub-fed | 6.5 | SUPER-HOT mux: warm replug + 5x 'g' full-reconfig cycles (user-verified 5 blank->colorbars) | **6/6 WARM ACQUISITION GOLDEN** (C:0177 every cycle) | Same build/board/hub that failed repeatedly warm LAST NIGHT => **MARGIN-STACK MODEL**: acquisition margin consumed ADDITIVELY by mux temp (proven: cooling flip) + sink/hub fatigue (proven: row 23 wedge; last night = hot mux x cycled-to-death hub; today = hot mux x fresh hub) + per-SOM TX quality (row 40). Qualification = acquisition-rate MATRIX across {mux temp} x {hub freshness} x {SINK} — this row is the ANKER (most tolerant converter) cell only; Ugreen warm-acquisition UNTESTED (its only golden ever was cooled-mux). 'g'-harness VALIDATED as the instrument (acks + visible + hands-free). ALSO: 'r' HPD retrain proven inert (250ms pulse swallowed by fabric hotplug debounce — has never worked on any build; fw fix queued; prior 'r'-based conclusions re-read as ladder self-restarts) |
| 42 | 08-19 | B1 | 36f44072 | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | warm attach + 5 'g'; then COOLED attach + 5 'g' | warm: attach FAIL, g 0/5 (mix C:8011 EQ-stall / C:8000 — good SOM shows the SOM#1 signature when far below threshold: failure signature = depth gauge). **Cooled: physical attach = COLORBARS (2/2 lifetime)**, but g 0/5 even cold | **HARNESS SCOPING DISCOVERY**: 'g' reconfig = unannounced mid-stream link death; Anker re-acquires (6/6 warm), Ugreen NEVER re-engages without full renegotiation (PD/mux/HPD ceremony) even cold. g-cycles valid for Anker-class only; strict-sink attach statistics need renegotiation-inclusive events ⇒ FW FEATURE QUEUED: 'virtual replug' (PD alt-mode exit/re-enter). Matrix so far: Anker{warm:6/6}, Ugreen{warm-attach:0/1, cool-attach:2/2 lifetime, reconfig:0/5 any temp} |
| 43 | 08-19 | B1 | 36f44072 | HEAD+net | Anker | captive | - | N | hub-fed | 6.5->sweep | WARM EQ SWEEP: 5 g-acquisitions per setting | **6.5: 5/5, 9.5: 5/5, 12.3: 5/5, 1.0: 4/5** | Current 6.5 default VALIDATED (mid/high EQ all perfect at n=5; headroom exists). Low end cracks first — and 1.0dB-warm = 4/5 = EXACTLY the legendary Saturday figure (Friday-era fw defaulted EQ 1.0): the era's 'boot lottery' was plausibly EQ-setting x warm bench, now reproduced mechanistically. Deeper 6.5-vs-9.5-vs-12.3 differentiation needs n=20+ or enclosure-hot conditions |
| 44 | 08-19 | B1 | f70af283 HYBRID | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | WARM attach, NO cooling | **COLORBARS after visible 11-round battle (Y:BB), then stable C:0177 x30, mature** | First-ever warm-Ugreen success. Hybrid acquisition phase (retrain-until-caught) converged where the non-fatal build held dark after 1 attempt (row 42). Maturity ride-through now armed; soak running to observe advisory-mode behavior (G: low nibble counts wobbles without teardown) |
| 45 | 08-19 | B1 | f70af283 HYBRID | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | 90+ min WARM soak | **ZERO drops (Y:BB unchanged), C:0177 every sample, G:F5 static (no advisory events even), Q +9ppm fully heat-soaked** | Strongest qualification evidence in the log: strictest sink, warm, 1.5h perfect hold on the production-candidate policy. Tracking margin confirmed far above the stack; hybrid's acquisition win at attach was the entire battle |
| 46 | 08-19 | B1 | f70af283 HYBRID | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | POWER-CYCLE fresh boot, mux IR-measured 30C | **~20s battle (Y:FF, 15+ rounds) then stable C:0177** — user-observed 'came up after about 20 seconds' | Even cool, Ugreen acquisition margin is thin — but hybrid converts thin margin into a bounded delay instead of permanent dark. Product framing: worst case = picture in ~20s. NOTE: IR reading CORRECTED to ~46C (gun's laser misaligned with sense spot — aiming 'north' of the inverted chip's position reads the die area) — consistent with the 50-60C dissipation model and the very-hot-to-touch reports; thermal model stands. Measurement protocol note: calibrate the gun's laser offset before trusting spot readings on dense boards |
| 47 | 08-19 | B1 | f70af283 HYBRID | HEAD+net | Ugreen | captive | - | N | hub-fed | 6.5 | deep-warm bench (hours + battle-storm self-heat): power cycle -> 4+min no catch; g-rolls degrade 8177->8011; AIR: catches WHILE cooled, LOST on re-warm | **UGREEN OPERATING ENVELOPE MAPPED**: below acquisition line at deep-warm steady state (transient cooling = transient catch); catchable fresh/ambient (rows 44/46); holds forever once mature at any temp (row 45). Signature deepens with temp (8177-drop -> 8011-stall = the margin depth gauge live). NEXT: the 6x6 heatsink = sustained-cooling experiment; 1.0a4 margin items = durable fix |
| 48 | 08-19 eve | B1 | Gowin stack rounds 1-3 (b2d91ffb / c83456b8 / 0f416d7b / 75f0dabf) | HEAD+net | Anker | captive | - | N | hub-fed | 6.5 | 4 hardware cycles | Round 1: CR-only (C:8011). Rounds 2-3 fixes ALL silicon-verified (CR replay: 0x13110, 0x173, 0x20000 x2) but CR LOST (C:8000) and never returned | **WS4 HARDWARE CHAPTER CLOSED**: 3 generator-dropped hardened-mode fields found+fixed (mode enables 0xNN6C[13:12], analog 0xNNa0[9], PMA path 0xNNa6[17:16] — one coherent lanes-2/3 EDP-preset defect) + gear-1:1 hardened configs broken by construction + clock-meter duty artifact documented. Stack still dark post-fixes (NOT bench-confounded: hybrid achieved 8177 trains same bench/minutes). Residuals ranked in WS4 doc §11 (invert-in-hardened discriminator first). STRATEGIC VERDICT STANDS: our core = production; Gowin stack = ticket-enrichment only |

<!-- new rows below; never reuse ids -->
