# M5 review (08-21 night) — disposition of the six review items

User review of the M5 runtime-AFE implementation after rows 75-78. Acted on
overnight per "act immediately on what makes sense; backlog the rest".
Everything below is on branch `claude/epic-lovelace-54ee14`; **nothing was
flashed** — the 138B bin built tonight is a review candidate.

| # | Item | Disposition |
|---|------|-------------|
| 1 | 60K projects broken/unsafe (shared top enables AFE with 138K addresses; `afe_adjust_seq.v` missing from two gprjs) | **FIXED** (`2e6013f9`): per-project `dp_test_die_pkg` (60B = OFF + zero bases, 138B = ON + verified bases); shared top reads the package; file added to `a2mega_dp_test.gprj` and `a2mega.gprj`. Build verification: see §Builds. |
| 2 | Per-lane requests ignored (lane 0's nibble applied to both lanes; one byte for every lane) | **PROPOSED, not applied** — propose-only diffs + sims in `hdl/displayport/sim/m5_proposed/` and `m5_perlane_proposal.md` (agent). Diffs are against `df553c79`; must be rebased onto the commit-on-ack sequencer (§2 below). Needs your review (shared `hdl/displayport/**`). |
| 3 | Declares a setting before DRP applied it; `afe_busy` ignored | **FIXED** (`2e6013f9`): commit on successful completion only; timeout never commits; `afe_busy` holds the ladder's next lane-set message. |
| 4 | No-INIT policy forgets state at every training end; assumes POR | **FIXED** (`2e6013f9`): state retained across training transitions; forgotten only on `phy_reinit` = PLL unlock \| PCS TX reset \| watchdog CSR replay. |
| 5 | Clamp permits VS+PE>3 | **FIXED** (`2e6013f9`): sanitised after the ceiling clamp; MAX_PE declared truthfully at the applied swing. |
| 6 | Production policy never exercised the apply path on hardware | **BACKLOG** — options in §6. |

## 1. Die gating (item 1)

- `boards/a2mega/hdl/dp_test/dp_test_die_pkg_60b.sv` and
  `boards/a2mega/hdl/dp_test_138b/dp_test_die_pkg_138b.sv` declare the same
  `package dp_test_die_pkg` with `ENABLE_AFE_ADJUST` / `AFE_LANE_BASE0/1`;
  exactly one is in each dp_test project (listed first in the file list).
- `a2mega_dp_test_top.sv` passes `dp_test_die_pkg::*` to `dp_transmitter`.
- `a2mega.gprj` (full core, 60K): `hdl/top.sv` does not set
  `ENABLE_AFE_ADJUST`, so the default 0 applies; only the missing file was
  the defect there.
- To enable on the 60B later: verify the 60B DRP lane bases from a 60B
  Reconfiguration-dialog export (the 138B were verified two ways —
  `m5_runtime_afe.md`), fill the package, build, bench.

## 2. Sequencer contract after items 3/4/5 (`afe_adjust_seq.v`)

- `applied_*` = committed levels; `target_*` = levels of the sequence in
  flight; `busy_r` set at launch, cleared on ack; the DRP side carries a
  per-sequence `seq_fail` (settled before `ack_tgl`), so a timed-out
  sequence completes (never wedges) but does not commit; `seq_err` stays
  the sticky telemetry flag (`M:2x/3x`).
- `afe_busy = busy_r | toggle-compare | adjust_de | adj_d | eval_pend` —
  covers the 3-clock evaluation latency after the 0x206 byte so the
  ladder's hold is already up before the lane-set message that follows
  (adjust reads expect 3 bytes, so there is ≥1 AUX byte-time of slack
  anyway; the cover makes it independent of message shape).
- `aux_channel.v`: `afe_hold` gates the single state-change site
  (`if(next_state != state && !afe_hold)` and `if(!afe_hold) state <=
  next_state`) only when the pending state is one of the 12 lane-set
  states. No other FSM path is touched; the 100 µs timer re-arms each held
  cycle; the 'retry_now' watchdog still applies.
- `phy_reinit` (new input) clears `applied_known`/`baseline`; wired in
  `dp_transmitter` as `~serdes_status[5] | ~serdes_status[2] |
  wdog_replay_req` (pll_lock, ~pcs_tx_rst, CSR replay). Stubs: 0.
- `APPLY_ON_TRAINING_START` now means "how the baseline is established
  after a reinit": 1 = write INIT (declaration stays the legacy 0x06 until
  it completes), 0 = assume the boot config. Neither re-applies at later
  training starts. The 138B top keeps 0 (row 77 verdict).
- Sanitising: `pe = (vs+pe > 3) ? 3-vs : pe` after the ceiling clamp;
  `MAX_PE = pe >= MAX_PE || vs+pe >= 3`. Consequence: at our VS2 ceiling
  the most PE a sink can get is PE1 (byte 0x2E declares MAX_PE there) —
  which is exactly the Ugreen's A:0026 ask.
- Sims: `tb_afe_commit.v` (new: commit-on-completion, no-commit-on-
  failure, retention, reinit re-baseline), `tb_afe_adjust.v` (rewritten
  expectations; edge-sampled `wait_idle` — a level `wait()` on a
  multi-input combinational signal wakes on NBA-region intermediates),
  `tb_afe_clamp.v`, `tb_afe_noinit.v`, `tb_afe_adjust_closedloop.v` (real
  auxch files, `.afe_busy` wired), `tb_hpd_disconnect_reset.v`,
  `tb_aux_idle_bias.v` — all PASS.

## Per-lane proposal (item 2) — rebase notes

The agent's proposal (`m5_perlane_proposal.md`) pre-dates `2e6013f9`.
When applying: per-lane-ize `applied_*`/`target_*`/`busy` bookkeeping,
commit only the lanes in the completed sequence, sanitise per lane, keep
`afe_busy` as the OR over lanes, and extend `M:` telemetry (MSG_LEN can
grow to 127). Open questions the agent raised: 0x105/0x106 mirror vs
2-byte write; `adjust_evt` on the 0x206 beat only (4-lane would need
0x207); `MAX_VS/MAX_PE` common to both lanes; one lane manual-FFE while
the other stays boot-Auto is a PHY state never benched.

## 6. Exercising the apply path on hardware (item 6) — options

Both tested hubs request exactly INIT (A:0022), so `48576c12` and
tonight's candidate take the no-write path. To validate a real
sink-requested change on the final configuration:

1. **A sink that asks for ≠INIT.** The Ugreen produced A:0026 mid-training
   (row 74/75); with per-lane (item 2) that becomes a lane-0 PE1 apply.
   Cheapest if it recurs; not controllable.
2. **Bench-only INIT offset** (build-time): set `AFE_INIT_VS=1` in the 138B
   top for one build — the boot csr still drives 804, the sequencer
   believes VS1 is resident, so the first A:0022 forces a real VS2 apply
   (8 writes + strobe mid-training) on the production-identical target.
   This measures exactly the open question from row 77 (does the apply
   path cost margin when exercised) with zero new RTL. Recommended first.
3. **Telnet-injected request override** (gateware debug knob): a scratch
   register forcing vs/pe_request — more RTL, more review; only if 2 is
   inconclusive.

## Builds

See the tail of this file (appended when the overnight chain finishes).

- 60K `a2mega_dp_test.gprj` (item 1 verification): synthesis + PnR PASS,
  Setup 0 / Hold 0, ~1 min (21:14-21:15); package resolved, AFE
  elaborated OFF; //SecurityBit: OFF; bin sha 5eddf85b. Not flashed (B1 not on the bench).
