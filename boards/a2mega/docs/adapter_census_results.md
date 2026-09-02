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

Notes
- Anker link health at capture: HBR x2 trained (`C:0177`), sink streaming
  (`K:03`), symbol-error counters valid and zero (`SE0/SE1:8000`).
- The legacy lossy decode (`X:91`) read the Anker's HBR2 sink as "no
  supported rates" (it only recognizes 0x06/0x0A) — the raw tap is
  required for this census.
