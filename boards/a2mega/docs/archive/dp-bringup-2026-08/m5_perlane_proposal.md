# M5 per-lane ADJUST_REQUEST — proposal (review item 2)

Status: **REVIEWED AND APPLIED 08-22** — see `m5_afe_adjust_design.md` §12
for the version that actually landed. This document is kept as the
proposal record; two things changed in review:
1. **Apply policy**: the proposal wrote only the CHANGED lane. Applied
   version writes ALL lanes with their own per-lane values, because the
   proposal's policy would split the two lanes between manual FFE and the
   boot csr's FFE Auto under `APPLY_ON_TRAINING_START = 0` (unbenched
   analog asymmetry on a link that depends on lane matching). This also
   removed the lane mask and the `S_NEXT` walk entirely.
2. **Telemetry**: lane 1 is now in the UART/telnet message as a new `M1:`
   field (open question 3 resolved — without it the feature could not be
   validated on the bench).
The applied version is rebased onto the items-3/4/5 sequencer
(commit-on-ack, `phy_reinit`/retention, VS+PE<=3), which this proposal
predates (see §4).

Original status: PROPOSE-ONLY. Nothing under `hdl/displayport/` outside
`hdl/displayport/sim/m5_proposed/` was modified. Not built with gw_sh.
Baseline for every diff: commit `df553c79` (HEAD at the time of writing).

## 1. Problem

The committed M5 (`hdl/displayport/gowin/afe_adjust_seq.v`) consumes only
lane 0's nibble of DPCD 0x206 (`dp_transmitter.sv` wires
`debug_adjust[1:0]`/`[3:2]` = 0x206[1:0]/[3:2]) and drives BOTH SERDES lanes
with that one (vs, pe); msg 0x19 writes the same `train_set_byte` to
0x103..0x106. Sinks ask per lane (observed A:0026 = lane 0 VS2/PE1, lane 1
VS2/PE0), and the two physical lanes are known to be asymmetric (mux /
channel), so lane 1 can end up driven at a level it never asked for while
its TRAINING_LANE1_SET declares lane 0's level.

## 2. What changes

### 2a. Sequencer — `hdl/displayport/sim/m5_proposed/afe_adjust_seq_perlane.v`
Drop-in replacement (same module name `afe_adjust_seq`). Diff vs HEAD:
`hdl/displayport/sim/m5_proposed/perlane_afe_adjust_seq.diff` (whole-file).

| Port / behavior | HEAD | Proposed |
|---|---|---|
| `vs_request`, `pe_request` | `[1:0]` lane 0 only | `[2*NUM_LANES-1:0]` packed `{lane1, lane0}`; lane 0 = 0x206[1:0]/[3:2], lane 1 = 0x206[5:4]/[7:6] |
| `train_set_byte` | `[7:0]`, one byte for all lanes | `[8*NUM_LANES-1:0]`: `[7:0]` = TRAINING_LANE0_SET (0x103), `[15:8]` = LANE1_SET (0x104) |
| `dbg_afe[5:0]` | `{seq_err, known, pe, vs}` | unchanged meaning, reports LANE 0 (telemetry `M:` field unchanged) |
| `dbg_afe1[3:0]` | n/a | NEW: `{pe1, vs1}` for lane 1 |
| applied state | one (vs, pe) | per lane; clamp (MAX_VS/MAX_PE) and ceiling flags evaluated per lane |
| trigger | apply iff request != applied | apply iff ANY lane's clamped request != that lane's applied; only CHANGED lanes get their 4-write sequence (own LANE_BASE only), ascending lane order, one DRP grant for the whole sequence |
| APPLY_ON_TRAINING_START=1 | INIT to both lanes (8 writes) | INIT to every lane (NUM_LANES x 4 writes), lane 0 first |
| APPLY_ON_TRAINING_START=0 | declare INIT, write only on change | same, per lane: a lane whose request equals its applied state writes nothing |
| DRP port protocol | req/gnt/addr/wrdata/wren/ready | UNCHANGED (same write order 0x34, 0x38, 0xd8=0x010, 0xd8=0x110; same gap, timeout, sticky seq_err) |
| ENABLE_AFE_ADJUST=0 | constant tie-offs | same; `train_set_byte = {NUM_LANES{IDLE_SET_BYTE}}` = 0x0606 |

Implementation notes: the DRP-side player gained one state (`S_NEXT`) that
walks a lane mask latched with the toggle (`mask_lat`); the mgmt side latches
`mask_lat = changed` (or all-ones for INIT). The payload (`mask_lat`,
`vs_lat`, `pe_lat`) is quasi-static across the toggle CDC exactly like before.

### 2b. Shared RTL diffs (all against HEAD `df553c79`; produced with
`diff -u HEAD-copy edited-copy`; the edited copies live in
`hdl/displayport/sim/m5_proposed/perlane/`)

- `perlane_dp_aux_messages.diff` — `hdl/displayport/auxch/dp_aux_messages.v`
  - hunk @68: `train_set_byte` port `[7:0]` -> `[15:0]` (+comment)
  - hunk @200: msg 0x19 data bytes: `12'h194` (0x103) <= `train_set_byte[7:0]`
    (lane 0), `12'h195` (0x104) <= `[15:8]` (lane 1), `12'h196`/`12'h197`
    (0x105/0x106, unused lanes) mirror lanes 0/1. Message length stays 4 data
    bytes (header `0x80 0x01 0x03 0x03`), so AUX timing is identical to the
    current 0x19 and to legacy 0x18 — no change to `expected` counts in
    `aux_channel`.
- `perlane_aux_channel.diff` — `hdl/displayport/auxch/aux_channel.v`
  - hunk @86: `train_set_byte` port `[7:0]` -> `[15:0]`; `lane_set_msg`
    selection and the 12 set states untouched.
- `perlane_channel_managemnt.diff` — `hdl/displayport/auxch/channel_managemnt.v`
  - hunk @67: `train_set_byte` port `[7:0]` -> `[15:0]` (pass-through only;
    `adjust_evt = adjust_de & aux_addr==0` unchanged — the 0x206 byte
    carries BOTH lane nibbles, so one pulse per iteration is still right).
- `perlane_dp_transmitter.diff` — `hdl/displayport/dp_transmitter.sv`
  - hunk @128: new output `debug_afe1[3:0]` (lane 1 `{pe, vs}`); the board
    top may leave it unconnected.
  - hunk @487: `train_set_byte` `[7:0]` -> `[15:0]`.
  - hunk @507: sequencer wiring: `.vs_request({debug_adjust[5:4], debug_adjust[1:0]})`,
    `.pe_request({debug_adjust[7:6], debug_adjust[3:2]})`, `.dbg_afe1(debug_afe1)`.

Source of lane 1's nibble: no new decode is needed. `link_signal_mgmt.v`
already captures the whole 0x206 byte into `channel_adjust[7:0]` (and 0x207
into `[15:8]`) on `adjust_de` with `addr==0/1`, exported unchanged as
`debug_adjust`. `dp_register_decode.v` does NOT decode 0x206/0x207 at all
(it only handles DPCD 0x00-0x0B); the existing `debug_adjust` is the
cleanest and only source. Lanes 2/3 (0x207) are available the same way if a
4-lane build ever needs them.

## 3. Simulation results (iverilog 12, `-g2012`)

```
=== tb_afe_perlane (proposed sequencer)
  ok: training start declares INIT on both lanes -> bytes {lane1 06, lane0 06}, 0 DRP writes so far
  ok: A:0026 (lane0 VS2/PE1, lane1 VS2/PE0) -> lane0 only -> bytes {lane1 06, lane0 0e}, 4 DRP writes so far
  ok: repeat A:0026 -> no writes -> bytes {lane1 06, lane0 0e}, 4 DRP writes so far
  ok: lane1-only VS1/PE0 -> lane1 base only -> bytes {lane1 01, lane0 0e}, 8 DRP writes so far
  ok: both change (lane0 VS1/PE0, lane1 VS2/PE2) -> lane0 then lane1 -> bytes {lane1 16, lane0 01}, 16 DRP writes so far
  ok: per-lane clamp -> lane0 0x06 (MAX_SWING), lane1 0x3E (both flags) -> bytes {lane1 3e, lane0 06}, 24 DRP writes so far
  ok: clamped request equal to applied -> no writes -> bytes {lane1 3e, lane0 06}, 24 DRP writes so far
  ok: next training start -> re-baselined, no writes -> bytes {lane1 06, lane0 06}, 24 DRP writes so far
  ok: APPLY_ON_START=1 -> INIT to lane0 (0x8083xx) then lane1 (0x8084xx), 40 writes total
PASS: per-lane AFE adjust — own request, own LANE_BASE writes, own TRAINING_LANEx_SET byte, per-lane clamp
=== tb_afe_noinit_perlane (proposed sequencer)
PASS: APPLY_ON_TRAINING_START=0 — boot csr trusted, DRP only on real change
=== tb_afe_clamp_perlane (proposed sequencer)
PASS: declared-ceiling clamp (MAX_VS=2) — VS2 ceiling flagged, PE honored, below-ceiling honored
=== tb_afe_adjust_closedloop_perlane (proposed auxch copies + proposed sequencer, Manchester AUX sink)
  [sink] TRAINING_LANE_SET = 02 ... = 17 ... = 17
PASS: closed-loop — sink ADJUST_REQUEST applied over DRP once per change, TRAINING_LANE_SET on the wire reflects applied levels + caps, link trains
```

Every A:0026 check verifies all four writes (address AND data) of the lane 0
block: `0x808334=0x0000D000` (txlev 13), `0x808338=0x00000700` (C1=7),
`0x8083d8=0x010`, `0x8083d8=0x110`, and that NOTHING was written to
0x8084xx. Expected bytes follow the design-doc §4 encoding
`{00, MAX_PE, pe, MAX_SWING, vs}` with MAX_VS=2/MAX_PE=3: VS2/PE0=0x06,
VS2/PE1=0x0E, VS1/PE0=0x01, VS2/PE2=0x16, VS2/PE3=0x3E.

The existing benches are NOT port-compatible as-is (a `[1:0]` request into
the `[3:0]` port zero-extends, which would make lane 1 request VS0/PE0 and
write). `tb_afe_noinit_perlane.v`, `tb_afe_clamp_perlane.v` and
`tb_afe_adjust_closedloop_perlane.v` are sed-adapted copies of the HEAD
benches: request replicated to both lanes (`{vs_request,vs_request}`),
`train_set_byte[7:0]` checked, `.dbg_afe1()` added, closed-loop idle check
`16'h0606`. All original assertions (write counts of 8 per change, lane 0
block at b+0, lane 1 block at b+4, byte values) pass unchanged, i.e. with
symmetric requests the per-lane module behaves identically to HEAD.

Lint: the proposed auxch tree (`perlane/` copies + the untouched auxch files
+ the proposed sequencer) and the proposed `dp_transmitter.sv` with the stub
PHY (`core/`, `video/`, `audio/`) compile cleanly under iverilog. The Gowin
`transceiver_bank_gowin.v` path is untouched by this proposal (DRP port
protocol unchanged).

Run lines are in each bench header; e.g.
`iverilog -g2012 -o /tmp/p.vvp hdl/displayport/sim/m5_proposed/tb_afe_perlane.v hdl/displayport/sim/m5_proposed/afe_adjust_seq_perlane.v && vvp /tmp/p.vvp`.

## 4. IMPORTANT: concurrent in-flight edits (review items 3-5)

While this was being prepared, another session modified the working tree
(uncommitted at the time of writing): `afe_adjust_seq.v` (+105/-29: commit
applied-state only on successful DRP ack via `target_vs/pe` + `ack_evt &&
!fail_w`, new `phy_reinit` input, state retained across
`training_active` fall, VS+PE<=3 sanitise, `baseline` flag for the
declaration), `aux_channel.v` (`afe_busy` input + `afe_hold` of lane-set
states), `channel_managemnt.v` / `dp_transmitter.sv` (`afe_busy` plumbing,
`afe_phy_reinit = ~serdes_status[5] | ~serdes_status[2] | wdog_replay_req`),
and the four `tb_afe_*` benches. This proposal is deliberately against
HEAD `df553c79`, not against those edits. To land both:

- per-lane-ize the items-3-5 sequencer: `target_vs/pe` and `applied_vs/pe`
  become `[2*NUM_LANES-1:0]`, `mask_lat` is added, the commit-on-ack block
  commits only the masked lanes, the VS+PE<=3 sanitise and `max_pe` rule
  are applied per lane (the `g_lane` generate block here is the natural
  place), `phy_reinit` clears `applied_known`/`baseline` as it does now.
- the `afe_busy`/`afe_hold` plumbing is orthogonal (busy is already a
  single bit covering the whole multi-lane sequence here).
- the `perlane_*.diff` hunks for the three shared files do not overlap the
  items-3-5 hunks textually except the `train_set_byte` port lines in
  `aux_channel.v`/`channel_managemnt.v` (adjacent lines) — trivial merge.
- observation for that session: the HEAD `tb_afe_adjust_closedloop.v`
  compiled against the WORKING-TREE auxch files (which add a required
  `afe_busy` input the bench does not drive) ends in
  `FAIL: TB watchdog timeout (state=14 wr_cnt=8 ls_cnt=0)` — the undriven
  `afe_busy` holds the ladder at `clock_voltage_0p4`. Against the HEAD
  auxch copies the same bench passes. The bench needs `.afe_busy(afe_busy)`
  once that change lands.

## 5. Open questions

1. Lanes 2/3 bytes (0x105/0x106) in msg 0x19: this proposal mirrors lanes
   0/1 to keep the 4-byte write (identical AUX timing). A 2-lane link's sink
   ignores them per spec; alternatives are a 2-byte write (header
   `0x80 0x01 0x03 0x01`, one fewer AUX byte pair, and `expected` in
   `aux_channel` stays 1 for the ACK) or writing 0x00. No hardware evidence
   favors any of these; mirroring was chosen as the no-timing-change option.
2. `adjust_evt` fires on the 0x206 byte; lanes 2/3 (0x207) arrive one beat
   later. Irrelevant for the 2-lane link, but a 4-lane build should pulse
   on the 0x207 beat instead (or after both).
3. Telemetry: `M:` keeps lane 0 (`debug_afe`). Lane 1's applied levels
   (`debug_afe1`) are not in the 91-byte UART message; adding them means
   growing `MSG_LEN` in `boards/a2mega/hdl/dp_test/a2mega_dp_test_top.sv`
   (currently 91, `msg_idx` is `[6:0]`, so up to 127 is free).
4. `MAX_VS`/`MAX_PE` are common to both lanes. If the mux asymmetry ever
   warrants a different declared ceiling per lane, they would become
   per-lane parameters; not proposed here (no evidence yet).
5. The 4-write-per-changed-lane policy means a request that changes only
   lane 1 leaves lane 0's FFE untouched (still Auto if never written under
   APPLY_ON_TRAINING_START=0). That is intended (row 77: do not perturb a
   lane that did not ask), but it is a new state combination the bench has
   not seen: one lane manual-FFE, the other Auto.
6. Whether a sink that asked A:0026 actually trains better with lane 1 left
   at PE0 is a bench question; the proposal only makes the applied levels
   match the request per lane and the declaration truthful per lane.
