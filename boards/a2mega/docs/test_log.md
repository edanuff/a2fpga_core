# a2mega DP test log

Structured record of every hardware test. **Protocol: no test result is
reported or acted on unless it has a row here.** The bringup log
(`bringup_1_0a3_log.md`) stays the narrative journal; this file is the
scannable ground truth.

## Column key

- **#**: test id (monotonic)
- **GW**: FPGA bitstream — sha256 prefix of the flashed `.fs` (git-committed)
- **FW**: ESP32 firmware — commit hash it was built from
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

<!-- new rows below; never reuse ids -->
