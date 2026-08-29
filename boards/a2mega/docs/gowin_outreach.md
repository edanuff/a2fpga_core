# Draft: Gowin GTR12 SERDES questions (GW5AT-60, DisplayPort TX)

Draft outreach for a Gowin FAE / engineer (forum DM, r/FPGA, electronix.ru,
or ticket). Written to be self-contained and reproducible. Trim to taste.

---

**Subject: GW5AT-60 GTR12 quad — three findings from a DisplayPort TX
bring-up, one question about common-block initialization**

Hi — I'm bringing up a 2-lane DisplayPort 1.1 transmitter (2.7 Gbps, Q0
lanes 2/3, QPLL0, REFCLK1 = 135 MHz) on a GW5AT-LV60PG484AC1/I0 (Tang Mega
60K SOM, IDE V1.9.12.01). The link trains and streams correctly, and I've
isolated a per-power-up behavior I'd like Gowin's guidance on. Three
findings, one question.

**1. Per-power-up "draw" in the quad common block (the question).**
On ~1 boot in 5, the link trains completely — sink reports clock/EQ/symbol/
inter-lane-align lock via DPCD 0x202, our periodic link checks pass — but
DPCD 0x205 SINK_STATUS stays 0 (no valid stream) and the display is dark.
The other ~4/5 of boots, the identical bitstream streams perfectly (15-min
soaks, zero errors). The outcome is decided once per powered session:

- Reproducible re-training, PMA reset (`FABRIC_LNx_RSTN`), PCS TX reset,
  and a held (2 ms) assertion of `FABRIC_CMU0/1_RESETN` + all
  `FABRIC_LNx_CPLL_RESETN` + `FABRIC_POR_N` (all wired to fabric, config
  has cmu_reset_by_fabric = true) do NOT change the outcome — tried 7×
  per session by an automated watchdog.
- Replaying the ENTIRE generator-emitted CSR write sequence (all 357
  `upar_write_driver` writes from the .csr sidecar) over the DRP port at
  runtime — verified writes land via DRP read-back — also does NOT change
  the outcome.
- Only a full reconfiguration (power cycle / bitstream reload) re-rolls it.

So some common-block state (my guess: word-boundary phase of the per-lane
20:1 serializer dividers relative to each other) is established during
configuration wakeup and is unreachable from fabric. Question: **is there a
sanctioned runtime sequence to re-initialize or re-align the quad common
block / lane dividers without a full reconfiguration** — e.g., a PLL
recalibration strobe, a divider sync pulse, or a documented TX lane-to-lane
alignment procedure? (I note the EDP PHY reference design ties all
common-block resets to gw_gnd, which suggests fabric resets of that block
are not expected to be useful.)

**2. Customized PHY "TX Channel Bonding" emits half a bond.**
With TX Channel Bonding checked (master Q0 Lane2, read start depth 16), the
emission chains the slave's tx_if FIFO to the master
(`tx_if_cfg_mst_sel = "q0.ln2"` on ln3) but `chbond_enable` remains false
and no other bond machinery is configured. On hardware this produced a
severe per-boot inter-lane phase lottery (~2/5 boots usable) plus a "doom
loop" where in-session retrains stayed correlated-bad. Un-chaining (bonding
unchecked) improved boots to ~4/5 and removed the doom loop. Is the
half-chained emission intentional, and is a full TX bond available for the
Customized PHY on GW5AT-60?

**3. Hardened 8b10b (EDP PHY / encode_mode=8b10b): no sink symbol lock.**
Same board, same lanes, same rates/AFE (CSR-verified identical analog
config): with the hardened encoder, feeding bytes+K per the generated
wrapper's packing (byte in bits [7:0]/[17:10], K at [8]/[18] of the 80-bit
fabric word), every sink reaches clock-recovery lock on TPS1 but NEVER
symbol lock on TPS2 — the signature of comma-free garbage on the wire. This
is polarity-independent: I alternated the per-lane `+0x3c` register between
0x408 and 0x008 at runtime via DRP (read-back verified) with no change.
Raw mode (encode OFF) with fabric-side 8b10b on the identical configuration
trains and streams. Is there an additional mode/CSR requirement for
fabric-fed hardened 8b10b on GTR12 beyond `encode_mode = 8b10b`, word
alignment, and the documented txdata/txk packing?

Happy to share full forensics: CSR diffs, DRP read-back logs, telemetry
traces, and minimal reproduction bitstreams.

---

Venue notes (not part of the message): r/FPGA has occasional Gowin
employee/FAE activity; electronix.ru has a long-running Gowin thread with
vendor-adjacent posters; the Sipeed community (Tang Mega) sometimes relays
to Gowin. Findings 2 and 3 are gifts that establish credibility; finding 1
is the ask that matters.
