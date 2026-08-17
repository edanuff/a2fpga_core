# Week plan: Mon 2026-08-17 → board-out Friday

Organizing principle: every task either **de-risks 1.0a4** or **keeps the
current board productive while waiting**. Video reliability is the
imperative; HDMI-converter (hub) viability needs a hard, data-backed
verdict this week.

## Workstream 1 — kill the replug tax (Mon/Tue) [productivity multiplier]

- **1a. openFPGALoader root fix attempt: SRAM-load-first.** Mechanism of
  the recurring failures: with content in flash, GW5A auto-boot retry
  races the JTAG-SPI passthrough (blank chip = deterministic = why
  rescue-first works). Hypothesis: loading ANY design to SRAM first
  quiesces the auto-boot scanner; then `-f` write proceeds without the
  race. One bench session to test; if it works, fold into flash.sh and
  the erase-replug ritual mostly dies. Also try lower `--freq` for the
  mid-write-drop class (verify-fail at ~60 s).
- **1b. Telnet-commanded FPGA reconfigure (bench tool).** ESP32 bit-bangs
  reload over its existing JTAG wiring; telnet key. Kills replugs for
  boot statistics; enables automated sweeps. (Consumer use: never —
  decision of record.) On 1.0a4 this becomes a RECONFIG_N pulse.

## Workstream 2 — AUX verdict → 1.0a4 schematic freeze (Mon/Tue) [rev-gating]

- **2a. TLVDS clean test** (task #2): TLVDS pad + known fw + NO breakout
  + low-loss cable C3 + monitor. Decides whether spec-drive works
  without attenuation → sets the 1.0a4 AUX network topology.
- **2b. Optional AD2 capture** of working (attenuated) vs failing AUX
  waveforms at the breakout — numbers for the 1.0a4 design margins.
- **2c. Decide the 1.0a3 bench standard**: breakout-inline (proven) or
  TLVDS (if 2a passes). Document in test log; stop varying it.
- **2d. Saturday-fw exoneration** (task #1 step 1): HEAD fw + breakout +
  C3. Expected LIT both orientations → firmware exonerated for monitor
  path; closes the false regression thread for good.

## Workstream 3 — hub viability verdict (Tue/Wed) [the hard assessment]

- **3a. Clean hub re-baseline** (task #3): plan-C GW + chosen fw + logged
  cable/EQ/orientation; high-n boot stats via 1b's reload tool.
  Question: does the config-draw lottery exist on the hub path under
  clean conditions, and at what rate? (Monitor path measured 5/5;
  the 4/5 / 2/5 hub numbers were confounded.)
- **3b. If a real draw rate remains:** FFE/pre-emphasis + swing sweep
  scored by boot stats (channel into the IT6563 has never seen
  pre-emphasis). Target: 5/5 by construction.
- **3c. Verdict artifact:** hub-viability assessment written into the
  bringup log: P(good boot) best-config, trend under tuning, and the
  explicit call on whether 1.0a3 hub support ships, defers, or fails.

## Workstream 4 — Gowin IP hard assessment (Wed/Thu) [tier-2 decision]

- **4a. IDE dual-install:** new IDE as a renamed app bundle alongside
  `/Applications/GowinIDE.app`; add `GW_SH=` override to build flow so
  existing builds stay pinned. Verify node-locked license against the
  new version FIRST.
- **4b. Assessment protocol (pass/fail):** on the new IDE — (i) does
  Customized PHY "TX bonding" emit a FULL bond (chbond machinery, not
  the half-bond)? (ii) does EDP PHY hardened-8b10b achieve sink symbol
  lock on hw? (iii) does the Gowin Encoder+PHY reference stack light the
  hub? (iv) boot statistics vs ours. Any (i)-(iii) pass reshapes tier 2;
  all-fail = documented dead end + outreach material.

## Workstream 5 — housekeeping (continuous)

- Test log discipline (every result = a row in test_log.md).
- Firmware roster kept current; converge telemetry to one superset
  format carried by ALL builds (spec: current HEAD field set is the
  superset — S/D/F/HLVC/P/E/R/L/G/K/W/U + PD lines).
- 1.0a4 requirements doc updated as evidence lands; freeze Thu, out Fri.
- Gowin outreach (docs/gowin_outreach.md) — user sends when ready;
  update with the new-IDE assessment results if they land first.

## Standing rules

- No user-facing FPGA reconfiguration, ever.
- 2-lane HBR is the production config (hub class is 2-lane).
- Every test logs: GW sha, FW commit, sink, cable id, orientation
  (detected CC), breakout Y/N, power source, EQ, cold/hot.
- Flash discipline: rescue-first until 1a lands; single attempts; judge
  by elapsed time + verify; heartbeat is the only true confirmation.
