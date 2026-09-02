# USB-C adapter census — both gates (a2p25 RBR-x4 qualification)

Two independent gates decide whether an adapter can deliver 1080p60 from an
RBR-only source (the a2p25 D-PHY-as-SerDes plan needs 4 lanes at RBR):

1. **PD/mux gate** — DP alt-mode VDO pin assignments from Discover Modes
   (`telnet 'd'`, ESP32 census, e3c41cbe). C/E = the mux path carries 4
   lanes; D/F = 2 lanes + USB3.
2. **DPCD gate** — the sink/converter chip's OWN `MAX_LINK_RATE` (0x001) and
   `MAX_LANE_COUNT` (0x002), invisible to PD. Captured by the AUX engine's
   attach-time caps read (68ec8bab): dp_test telemetry line D6 `O:rrll`
   (rr = 0x001 raw, ll = 0x002 raw: low nibble lanes, bit7 enhanced
   framing, bit6 TPS3); full core: OSPI regs 0x3C/0x3D, `d` prints
   `sink caps:`.

Verdict for RBR-x4 = BOTH gates must show 4 lanes. A 2-lane DPCD cap means
2 x 1.62 Gbps = 3.24 Gbps, short of 1080p60's ~4.46 Gbps -> 720p ceiling.

Method: 60K card, dp_test bitstream 232c81f1 (colorbars + telemetry),
census firmware d7b34197, readout over the WiFi telnet tee (USB-C is the
DP port, so serial is unavailable during the test).

| Date | Adapter | PD VDO | PD pins / verdict | DPCD O: | Rate max | Lanes max | RBR-x4 verdict |
|------|---------|--------|-------------------|---------|----------|-----------|----------------|
| 2026-09-02 | Anker hub (IT6563 DP->HDMI) | 00000C05 | C+D / "4-LANE OK" | 14C2 | HBR2 | **2** (+ef +tps3) | **FAIL — 720p ceiling** (PD gate alone would have passed it) |
| 2026-09-02 | Mindorlan USB-C hub/adapter | 00000C05 | C+D / "4-LANE OK" | 14C2 | HBR2 | **2** (+ef +tps3) | **FAIL — 720p ceiling** (identical caps to the Anker; EDID given up after 41 DEFERs, `U:69`) |
| 2026-09-02 | Ugreen hub (the strict-converter/overdrive-class unit) | 00000C05 | C+D / "4-LANE OK" | 14C2 | HBR2 | **2** (+ef +tps3) | **FAIL — 720p ceiling** (third identical reading; trained clean at 804 mV, `G:F1 U:08`) |
| 2026-09-02 | Generic USB-C->HDMI 4K dongle with 100 W PD passthrough (charger on its PD-in sourced VBUS -> card negotiated as power SNK) | 00000C05 | C+D / "4-LANE OK" | 0000 (never read) | ? | ? | **UNMEASURED — AUX dead**: PD/mux/HPD all good (`DP-ACTIVE`, `HPD=1`, `MUX=1`, FPGA `HLVC:1000`) but the ladder sits at `check_presence` with ZERO AUX reply edges (`E:00`) in BOTH flip polarities and after an HPD-pulse retrain; timeouts saturate (`T:000F`); no training, no colorbars. Sink DPCD unreadable -> separate AUX-electrical investigation (candidate: spec AUX DC-bias presence detect vs our pulldown-both pad config). Physical plug flip: no change. Device CONFIRMED GOOD on Mac and iPhone (HDMI out on both) -> a2mega-side. CLASSIFIED: the documented 1.0a3 AUX-RX limitation (aux-rx findings / board_1_0a4_requirements.md): AC-coupled AUX with no FPGA-side DC bias -> TLVDS receiver stores an offset and decodes converter/hub-class replies only; monitor/dongle-class sinks are offset-blind (E:00). **Second gate is unmeasurable for this sink class on 1.0a3 until the 1.0a4 receiver-bias divider.** Blind ladder (BLIND_SINK=1) would still drive video through it open-loop. NOTE: its 4K claim implies an HBR2 x4-class converter — the very 4-lane sink the RBR-x4 plan wants to find, and the class this board rev is blind to |
| 2026-09-02 | Cable Matters 48 Gbps unidirectional USB-C->HDMI 2.1 adapter (slot-powered card; card sources VBUS, adapter LED lit) | — (no Discover Modes) | — | — | ? | ? | **UNMEASURED — PD-SILENT**: FUSB302 attach OK on CC1 (SRC/DFP, VCONN sourced on Ra pin), one hard reset at attach, then parked in `SOURCE-WAIT-REQUEST`: our 5 V/1 A Source_Caps (150 ms cadence) never draw a Request; no alt-mode entry, `MUX=0 HPD=0`; no new PD events over 25 s (not a VBUS-collapse livelock). Both gates blocked at the PD layer. Follow-up: FUSB302 RX log / PD analyzer; plug-flip + Mac known-good check pending |
| 2026-09-02 | **USB-C->DP cable + DP monitor (model TBD)** — slot-powered card, fw 79538724 (pin-E fix) | 00001005 (plug) | E / "4-LANE OK" | **14C4** | HBR2 | **4** (+ef +tps3) | **PASS — RBR-x4 viable.** Both gates 4-lane. Closed-loop link trained HBR x2 (`C:0177`), streaming (`K:03`), SE 0/0, EDID answered (`U:00`), video live. ⚠ BEFORE fw 79538724 this cable was refused by our PD stack (`no DP sink mode with pin C` -> USB-only): plugs offer pin E, and a2mega had never negotiated E |
| 2026-09-02 | Fangor USB-C portable monitor (USB-C input; monitor sources VBUS -> card SNK; first CC2-orientation device) | 00000405 | C / "4-LANE OK" | 0000 (never read) | ? | ? | **UNMEASURED — AUX offset-blind**: PD `DP-ACTIVE`, `MUX=1 HPD=1`, mux regs consistent for CC2 (`0x0A=1E`, `0x13=A0`), FPGA sees HPD, but ZERO AUX reply edges (`E:00`) in both crossbar states (`f`) and after retrain. Physical plug flip: no change. KNOWN sink: this monitor has NEVER lit via alt-mode on 1.0a3 except under BLIND_SINK — it is the campaign's blind-ladder monitor. Lane telemetry is therefore unobtainable on this board rev by construction (blind mode reads no replies); only its PD gate (pin C, 4-lane mux path) is known. 1.0a4 receiver bias. (One `DP ALT MODE RESPONSE TIMEOUT` during PD, recovered.) |

Board-rev caveat: on 1.0a3 the DPCD gate is readable only from sinks whose AUX replies the offset-marginal receiver can decode. Hub/converter class reads; the generic 4K dongle did NOT (E:00); the DP monitor DID read cleanly (E:18-EC, O:14C4), so "monitor-class = blind" from the earlier bring-up rounds is not universal — it is per-sink. Offset-blind sinks read O:0000 with E:00 regardless of their real caps — that is the 1.0a3 AUX-RX limitation, not a census result. The 1.0a4 receiver-bias fix lifts it.

Result (n=7 devices): the only 4-lane sink at both gates is a REAL DP MONITOR over a direct USB-C->DP cable (PASS). Every hub-class adapter is a 2-lane HBR2 converter behind a 4-lane PD advertisement (720p ceiling for RBR-only). Three devices are unmeasured: two AUX-offset-blind on 1.0a3 (generic 4K dongle, Fangor USB-C monitor) and one PD-silent (Cable Matters). Readability is per-sink: the direct DP monitor and all hubs read; the Fangor and the dongle do not. Side finding with product impact: our PD stack rejected pin assignment E, so direct USB-C->DP cables never worked on a2mega until fw 79538724.

Emerging pattern (n=3): every hub-class adapter so far advertises C/D pin
assignments (4-lane mux path) yet carries a 2-lane HBR2 DP->HDMI converter
(IT6563 class). For an RBR-only source that is a 720p ceiling on hubs
regardless of the mux; the open question is a DIRECT USB-C->DP path to a
real DP monitor, where the monitor's own DPCD is the sink.

Notes
- Anker link health at capture: HBR x2 trained (`C:0177`), sink streaming
  (`K:03`), symbol-error counters valid and zero (`SE0/SE1:8000`).
- The legacy lossy decode (`X:91`) read the Anker's HBR2 sink as "no
  supported rates" (it only recognizes 0x06/0x0A) — the raw tap is
  required for this census.
