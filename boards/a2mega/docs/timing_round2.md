# Timing campaign, round 2 — durable closure (2026-09-02 →)

Rule (ed): re-rolls are not closure. A build is **durable** only when worst
setup slack is positive with margin (target ≥ +0.5 ns, the observed roll
spread) on every clock, on the deterministic dp_test gate AND across ≥3
full-core rolls. Fix cones structurally; never by re-rolling, Place_Option
roulette, or unproven SDC exceptions.

Verification harness: `hdl/displayport/sim/run_full_chain.sh` (DisplayPort_
Verilog `tb_dp_frame_audio` + `check_dp_frame`/`check_dp_audio` against this
repo's `hdl/displayport`; both packer configs). Baseline and every change:
3,037,500 cycles no underrun, 720 rows pixel-exact, ALL AUDIO CHECKS PASSED.

## Starting point (epic-merge-prep @ dff28ad4)

| Build | Result | Dominant cone |
|---|---|---|
| 60K full core, 5 rolls of identical source | 1 / 1 / 4 / 9 / 1 setup viols | `pixel_cdc_fifo` BSRAM DO → `video_stream_packer/ready_s0` (clk_sym 7.407 ns), −0.447 worst; archived production draw was 0/0 with **+0.097** |
| dp_test (deterministic; no datetime in netlist) | 5 viols (35 at the pre-census tip; stale since 08-29) | `sym_delta→freq_ok` instrument, `sdp wb` mux, AUX `err_rxc` CE |
| dp_test_blind | 23 viols | clk100 inside i_dp, `freq_ok` |

## #1 — pixel_cdc_fifo two-stage output skid (991158e1)

Cause: the FIFO's single output register was absorbed into the BSRAM's own
output register, so the packer ate the block RAM's ~2.5-3 ns clock-to-out
plus its ready/data muxes in one clk_sym cycle. Fix: stage A = BSRAM read
register, stage B = fabric head register; contract unchanged (+1 cycle
initial latency; `rlevel` counts stage A). Harness: identical to baseline,
both packer configs.

| Build | Before | After |
|---|---|---|
| dp_test gate | 5 viols; FIFO-DO path present | **FIFO-DO path gone**; 20 viols, now all instrument/debug-latch/sdp class: `sym_delta→freq_ok` −1.620 (clk_sym), AUX `tx_rd_ptr→dbg_esi2003/esi_2005 CE` −0.480 ×16 (clk100), `sdp wb_idx→wb_b` −0.266 |
| 60K roll 1 | 1–9 viols, FIFO-DO worst | **FIFO-DO path gone**; 2 viols: `line_cycle→sched_rom AD[13]` −0.060 (clk_sym), `debug_overlay s2_x_bit→r_o/SET` −0.045 (clk_pix); next: `wr_accum→wr_fifo DI` +0.044 (clk_logic) |
| 60K roll 2 | — | **0/0**; worst +0.016 clk_sym `line_cycle→pf_vb1/RESET`, +0.085 clk_pix `ovl_cy_q→phase_r/RESET` |
| 60K roll 3 | — | **0/0**; worst +0.004 clk_pix `ovl_cx_q→glyph_line_r/CE`, +0.074 clk_sym |

Verdict on #1: the −0.45 ns cone is gone from every report (FIFO-DO startpoints 0/0/0); the count distribution moved from 1–9 to 0–2 — but worst margins of −0.060 / +0.016 / +0.004 are still inside the ±0.5 roll spread, so the build is NOT yet durable. The remaining knife-edge is now split across the packer's `line_cycle` fanout (sched-ROM address + `pf_vb` resets, clk_sym) and the DebugOverlay/`ovl_c*` decode paths (clk_pix), plus the dp_test-only instrument/AUX-latch/sdp families.

## #2 — packer registered two-step lookahead (nc2_r counter)

Cause: `nc2/s0n2/s1n2` (two adders + two wrap muxes off `line_cycle`) fed the
schedule-ROM address and the `pf_*` compares directly. Fix: `nc2_r` is a
counter sharing `line_cycle`'s zero/increment control; `s0n2/s1n2` are wiring
off it. (A first formulation that recomputed nc2 from line_cycle's next value
put a five-way mux + two adders on the flop's reset pin and drew −0.055 —
replaced.) Sim-only `$fatal` proves `nc2_r == nc2` every clock; harness
identical to baseline, both packer configs.

| Build | Result |
|---|---|
| dp_test gate | **0/0** — first clean deterministic dp_test since 08-29 (with cone (b) in tree); worst +0.156 cm_life / +0.166 clk_pix / +0.197 clk_sym / +0.208 clk100 |
| 60K roll 1 | **0/0**; clk_sym +0.361; floor clk_logic +0.048 `wr_accum→wr_fifo DI` |
| 60K roll 2 | **0/0**; clk_sym +0.406, clk_pix +0.303; floor clk_logic +0.048 |
| 60K roll 3 | **0/0**; clk_sym +0.101, clk_pix +0.107; floor clk_logic +0.044 |

Verdict: clk_sym retired as the knife-edge (no sched-ROM/pf_vb/nc2_r
endpoints in any report). Three clean rolls, but margins still below +0.5:
the floor is now `wr_accum → wr_fifo DI` (clk_logic, cone f) with the
clk_pix framebuffer/overlay decodes next; vendor DDR3 PHY `cmd_oserdes`
(clk1x) hovers +0.07–0.14 and is left alone.

## #b — dp_test freq_ok instrument split into registered half-compares

Two 28-bit magnitude compares in one clk_sym cycle on a value static for a
second; now 14-bit halves registered and combined a cycle later. dp_test-only;
verified by the deterministic gate above (0 `freq_ok` endpoints).

## Remaining cones (approved by ed 2026-09-02)

- **AUX engine debug/ESI latches** (clk100): `tx_rd_ptr → dbg_esi2003/esi_2005/err_rxc CE` — register the byte strobe once, latch a cycle later from a registered data copy; no `next_state` change.
- **sdp_engine wb mux** (clk_sym): third pipeline stage on the selected byte (pnr_structural_analysis F3).
- **(e) overlays** (clk_pix): OSD `screen_x==X_START` / `screen_y==Y_OFFSET` tracker gates → pre-registered `x+1` flags (exact, no offset shift); DebugOverlay stage-2 output mux → extra stage with offset compensation. Visual check on the 60K.
- **(g) framebuffer trackers** (clk_pix): `cy_changed && cy==0` reset cone on `v_approach_px_r` → register the flags and act a cycle later (events are in blanking).
- **(h) pipelined DOC** (clk_logic): `curr_rts_r → ram_wds_din_r` consume path → pipeline; proven by `sim/doc5503/tb_doc5503_diff.sv`, then a listening pass.
- **(i) framebuffer line-buffer read address** (clk_pix): `lb_rd_addr → line_buf ADB` — register (approved).
- Vendor DDR3 PHY `cmd_oserdes` (clk1x, +0.07–0.14): left alone by decision.
- Verification standard (ed): framebuffer/overlay cones (e)(f)(g)(i) = visual on the IIgs after clean rolls, one bench flash after the clk_pix set lands.
- **framebuffer wr_fifo DI** (clk_logic): `wr_accum → BSRAM DI` — register the write data before the FIFO.

## Session 2026-09-03 (evening) — clk_pix / clk_sym / clk100 cone set

Method unchanged: one structural fix per cone, dp_test gate + full-chain
harness / AUX benches as the proof, full-core rolls only to read the next
floor. Per-clock "floor" below = worst setup slack from the report's
Max Frequency Summary (`1/f_constraint - 1/f_actual`).

### Baseline spread this session (nine prior rolls)

| clock | worst … best |
|---|---|
| clk100 | +0.07 … +1.12 |
| clk_sym | -0.06 … +0.41 |
| clk_pix | -0.21 … +0.30 |
| clk_logic | -0.07 … +2.22 |

### Cones fixed (in order of discovery)

- **(e) overlays** (794028a8): DebugOverlay `OUT_PIPE` output stage (a2mega only, X_OFFSET compensates); OSD tracker `screen_x+1` / `screen_y` pre-registered flags (exact).
- **(g) framebuffer vertical tracker** (67c4c65c): `cy_zero_px_r` registered alongside `cy_changed_px_r`; trackers act register-to-register.
- **(i) line-buffer read address** (f3a3f098): CE-free `syn_keep` copy `lb_rd_addr_q` before the BSRAM, lookahead cx+4 / trigger +6.
- **(j) packer `ready` / idle inserter** (10a7dc9f): the persistent clk_sym knife-edge was ONE register — `video_stream_packer.ready` (D: the `sof_mismatch`/`fetch_starved` cone, `line_num_1 → ready/D` -0.107; Q: replicated fanout into `idle_pattern_inserter.count_to_switch` CEs, 17 paths at +0.03..+0.08, dd 7.5 ns of a 7.407 ns period). `ready <= running` (rise identical, drop one cycle later) and the inserter samples `source_ready` once. Invisible downstream (ready already leads the data by the sdp/msa depth). Proof: `run_full_chain.sh` both packer configs — 720 rows pixel-exact, ALL AUDIO CHECKS PASSED, no underrun over 3,037,500 cycles.
- **(k) DebugOverlay font ROM** (430bbffa): the 128-entry LUT-ROM read `s1_rom_addr → s2_font_row` (-0.007, dd 6.68) split into glyph select (16:1) + row select (8:1) under `OUT_PIPE`.
- **(b) second cut** (0e4d9241): dp_test `freq_ok` 14-bit slices → 7-bit slices (the 14-bit compare landed on a flop RESET at +0.074).
- **(i2) end-of-line test** (70c63afc): `fb_x_r == fb_width-1` (subtract + compare inside `lb_rd_addr`'s CE) → exact one-cycle lookahead `x_last_r` from `fb_x_n`.
- **(c)+(c2) sdp_engine** (d74ca22c): (c) wire-byte lookahead three deep — index at k-3, four 16-entry groups registered at k-2, 4:1 at k-1; insertion cycle 2 joins 0/1 on constant-index regs; sim self-check extended. (c2) the 256-bit packet load shared one fanout-256 enable (`pkt_pending → db CE`, +0.04 in counted roll 2) → load staged one cycle behind replicated per-quarter `syn_keep` strobes, side effects moved with it, arming chain +1 (same parity margin). Proof: full-chain harness (both configs, audio ECC+PCM), dp_test gate.
- **(b3) dp_test colorbar generator** (72aafc9a): the gate's own `cx → rgb/SET` compares (+0.34 once the core cones were gone) → local `syn_keep` cx copy, bar/x-border decisions registered with a two-pixel lookahead (constants−2, wrap folded), rgb a 2:1 mux of flops; y-border lag masked by the x border. Gate: clk_pix +0.34 → +1.16.
- **(a) AUX clk100 cone** (200b4c82): two legs, ladder `next_state` untouched. (1) `aux_interface` tx/rx empty flags were combinational pointer compares (5-stage carry chain off `tx_rd_ptr`, fanout 131) heading every reply-byte latch CE → registered flags from the same push/pop strobes (bit-identical). (2) `channel_managemnt`'s `adjust_evt = adjust_de & (aux_addr==0)` fed `afe_adjust_seq`'s live busy term and came straight back into `afe_hold` (fanout 40) → registered in `aux_channel` at the same edge (same cycle, same value). Proof: 13 AUX benches PASS (train_recover ×3, hpd_disconnect, afe_adjust, polite_attach ×2, afe_perlane/closedloop/zeroreq/commit/clamp/noinit, gate_fail_counters); gate 0/0, clk100 +0.19 → +0.42.
- **(k2) DebugOverlay offset subtracts** (ff3060c4): stage 0b registers `x-X_OFFSET`/`y-Y_OFFSET` ahead of the region-compare tree (`s0_x_r → sA_in_space/sA_is_hex`, +0.46 = the clk_pix floor after the rest). X_OFFSET 16 → 13 total for the three `OUT_PIPE` stages.
- **dp_test gate configuration** (339fc032): the gate built the packer's legacy walk (`SCHED_ROM` default 0) while the core builds `SCHED_ROM=1`; its walk-only knife-edge (`pf_ftu → dec_fe_r/RESET`, +0.11..+0.57 across gate builds) was in a path the core never uses. Aligned to `SCHED_ROM=1`.

- **(c3) sdp_engine insertion selects** (in the sdp commit): `ins_cyc` compares (==1/==2/==last) fanning into all 73 `out_data` bits (+0.15 in the diagnostic) → one-cycle-ahead flags from the counter's own next-value rule, sim-mirrored.
- **(h) pipelined DOC** (1c22215c): the clk_logic floor (diagnostic: `curr_rts_r → ram_wds_din_r` at 17 ns of 18.5; +0.29 in counted roll 2) was OSC_CONSUME recomputing `wave_addr_f(curr_acc_r)` in series with the lane mux / tag compare / consume rules → registered at OSC_LOAD_NEXT_CONTROL (`exp_addr_r`, exact, sim-mirrored). Proof: `sim/doc5503/run.sh` (rev-2 repro + two rev-3 differential suites, bit-identical). **Listening pass on the IIgs before merge.**

- **(l) pixel_cdc_fifo write full flag** (db8e1afb): the gate's clk_pix floor after (b3) was `wptr_bin → mem CEA` (+0.15): the full test (subtract + compare) on the BSRAM write enable → `wfull_r` registered from the next write pointer and the registered read pointer (one cycle staler, conservative-safe by the module's own argument). Proof: full-chain harness, gate.

- **(n) pixel_cdc_fifo read empty flag** (db8e1afb, with (l)): `wptr_gray_r2 → mem CEB` (+0.43 in the gate, +0.37..+0.73 in rolls) → `ram_empty` registered from the synchroniser's first stage and the read pointer's next value (exact); refined twice from the rolls that followed: a registered `bin2gray(rptr_bin+1)` lookahead, and the two candidate compares evaluated in parallel and selected by fetch (on GW5A the 11-bit equality is a carry chain; behind fetch it reached +0.00). Residual: the fetch LUT still reads a BSRAM data output (block 12 = the sof bit) even with the valid flags `syn_preserve`d — +0.62..+1.17 in the final builds, above the bar, noted.
- **(m) scanline_dim trigger/lock** (dcfb0d1f): `ovl_cy_q → u_scanline_dim/phase_r RESET` (+0.51) → x-trigger and window-top compares pre-registered (exact: x sequential, y constant across the line at the trigger column).
- **(o) framebuffer vertical tracker, second stage** (009e2c98): `v_active_px_r → v_active_px_r/CE` at +0.00 in the first counted roll of the final source (seven flags + the phase compare in one enable) → decision flags registered at the event and applied a cycle later; every tracker enable is a one-level function of flops; export toggle timing unchanged. Visual with the clk_pix set.

Retired: **(f) framebuffer `wr_fifo` DI** — the "+0.048 clk_logic floor" in the earlier log was the HOLD slack of `wr_accum_r → wr_fifo DI` (dd 0.28 ns), not a setup path; setup floors on clk_logic were +0.85..+2.22 until (h) surfaced. Vendor DDR3 PHY `cmd_oserdes` (clk1x setup +0.41, hold −0.010/−0.014 in two rolls) left alone by decision.

**Ladder finding:** with every other cone gone, clk100's floor in the diagnostic placement is the AUX ladder's own `next_state` cone (+0.20; +0.26..+1.9 across rolls) — excluded from the campaign by rule; clk100 is reported as ladder-limited, positive on every roll.

### Results

| Build | viols | clk100 | clk_sym | clk_pix | clk_logic | note |
|---|---|---|---|---|---|---|
| roll (e,g,i) #1 | 0/0 | +0.67 | +0.03 | +0.68 | +2.22 | clk_sym = cone (j) whole top-17 |
| roll (e,g,i) #2 | 2/0 | +0.40 | -0.11 | -0.01 | +1.43 | (j) D-side; (k) font ROM |
| roll (e,g,i,j,k) | 0/0 | +0.26 | +0.21 | +0.45 | +0.85 | (c) and (i2) next |
| roll (+c,i2) | 0/0 | +1.00 | +0.73 | +0.46 | +1.30 | (k2) next; sources before (a) |
| gate (b2,c,j) | 0/0 | +0.19 | +0.51 | +0.70 | — | (a) next |
| gate (+a) | 0/0 | +0.42 | +0.11 | +0.61 | — | walk-only packer path → cfg fix |
| gate (+cfg) | 0/0 | +0.52 | +0.61 | +0.34 | — | (b3) next |
| roll (+cfg,+b3,+c2..) | 0/0 | +0.51 | +1.57 | +0.73 | +1.96 | first pass of the whole set; (c3),(h),(l),(m),(n),(o) followed |
| **final roll 1** | 0 setup, 1 hold (vendor DDR3 −0.010) | +1.51 | +1.17 | +1.06 | +4.18 | clk1x +2.10 |
| **final roll 2** | 0/0 | +1.57 | +0.67 | +0.98 | +2.87 | clk1x +1.29 |
| **final roll 3** | 0/0 | +0.43 (ladder) | +1.74 | +1.05 | +4.91 | clk1x +2.03 |
| **final gate** | 0/0 | +1.34 | +0.62 | +1.18 | — | SCHED_ROM=1, all instruments fixed |

**Verdict.** Three consecutive full-core rolls of the final source and the
deterministic gate are clean on setup; every clock the campaign was
allowed to touch sits above +0.5 ns on all three rolls (minima: clk_sym
+0.67, clk_pix +0.98, clk_logic +2.87, clk1x +1.29). The two exclusions
are explicit: clk100 is bounded by the AUX ladder's own `next_state` cone
(+0.43 … +1.57 across the three rolls; +1.34 in the gate), and the vendor
DDR3 PHY command serializer shows −0.010 ns hold in one of the three
rolls. The (b)/(b3) instrument fixes and the SCHED_ROM alignment make the
gate a faithful, deterministic proxy for the core's DP path for the first
time.

Verification standard for the bench (ed): one flash + visual on the IIgs
after the clk_pix set lands (DebugOverlay / OSD position, border, first
active pixel, 80-column text); no further bench checks needed for (a),
(c), (j) beyond the benches and the gate.

**Bench check 2026-09-04 (ed, 60K, bitstream 7576d91b = final roll 3):** IIgs hi-res
test pattern with all four corner markers, colours, even scanline bands, DebugOverlay
and OSD in place — clk_pix set PASS; Ensoniq PASS on the 4soniq demo disk (larger
games not yet runnable on this bench) — DOC cone (h) merged. PRs #8–#23 merged to
epic-merge-prep in order; the two exclusions stand as documented above.

## 138B follow-up (2026-09-04, branch claude/timing-r2-138b)

The 138B core built 0/0 on the final 60K source but not durably (clk_sym +0.02 on the 8b/10b encoder input,
clk_pix +0.08 on the scanline lock; its gate +0.06 on the watchdog counter, +0.03 on the dp_test tone accumulator).
The 138B routes run roughly 0.5 ns longer than the 60K for the same cones and its DP lanes sit on other die lanes.
Six exact fixes followed (encoder input register, watchdog 1 ms prescaler + ms counter with identical trip cycles,
scanline lock via v_border-1 + wrap term, FIFO write-full input as a registered pointer difference + carry bits,
tone accumulator in the reduced fraction 4/12375, AUX error-detail captured centrally from delayed copies).
Proofs: tb_gowin_lane (encoder + main stream processing + idle inserter vs the independent decoder, reference
delayed two cycles for the new latency) PASS; polite_attach x2, train_recover x3, gate_fail_counters PASS;
full-chain harness both configs; both gates 0/0; 60K roll 0/0 with clk100 +1.01 / clk_sym +1.19 / clk_pix +1.67 /
clk_logic +4.40 (its best roll); 138B roll 0/0 with clk100 +0.08 / clk_sym +0.10 / clk_pix +0.07 / clk_logic +1.95.

Verdict for the 138B: the fixed cones are gone but the next ones land at the same +0.1 level on every clock
(ladder self-loop, pixel-PLL init enable into the SDP group RAM resets, packer line_cycle -> nc2_r reset,
fb_x_r -> x_last_r). That is a die-level routing margin for this design at these clocks, not a cone list;
per-cone fixing does not converge on it. Options: (A) treat the 138B as builds-clean-not-durable while the 60K
ships; (B) a physical floorplan/placement study for the 138B (DP core near the GTR bank); (C) a broader 138B
pipelining pass across the DP core (parallel-LFSR scrambler, packer schedule, SDP), each with DisplayPort_Verilog
benches; (D) the AUX ladder one-hot re-encode, which now bounds clk100 on both dies.

Correction to the session log above: the full-chain harness compiles only the packer/FIFO/MSA/SDP files, so
cone (j)'s idle-inserter half was bench-proven on the IIgs, not harness-proven; tb_gowin_lane now covers it.

## 138B durability (2026-09-05, branch claude/timing-r2-138b-durable)

ed: the 65816 work will likely be 138B-only, so the 138B build must be
durable, not just clean. Per-cone fixing had stopped converging on that
die (every fix exposed the next cone at the same +0.1 level), so this
pass changed method.

### Diagnostic: what the placer can do when asked

One clock tightened by 1.0 ns per build, 138B full core:

| Build | tightened | setup viols | meaning |
|---|---|---|---|
| D1 | clk_sym 7.407 → 6.407 | 0 | every clk_sym path ≥ +1.0 ns real |
| D2 | clk_pix 6.734 → 5.734 | 0 | every clk_pix path ≥ +1.0 ns real |
| D3 | clk100 10.0 → 9.0 | 0 | every clk100 path ≥ +1.0 ns real (ladder self-loop +1.54 real) |

Conclusion: the 138B's marginality at the nominal constraints is placer
effort, not intrinsic logic depth — timing-driven PnR stops as soon as
slack is barely positive. The near-critical populations the diagnostics
listed (packer `line_cycle`/`line_num` decodes into the `nc2_r` set/reset
arcs, SDP parity stage 1, the PLL_INIT enable net into inferred-RAM output
registers, the chained 8b/10b lookups, the framebuffer end-of-line flag,
two 29-bit audio-strobe accumulators) were removed structurally anyway.

### Structural set (all exact unless stated)

- **AUX ladder fanout** — `syn_maxfan = 12` on `state`/`next_state`; GowinSynthesis replicates the registers (netlist shows several copies per bit). No logic change. 138B clk100 at nominal: +0.00/+0.08 → +1.00.
- **8b/10b encoder carry-select** — second symbol encoded for both running disparities, selected by the first's output; the lookups no longer chain. `tb_gowin_lane` (2-cycle ref) PASS.
- **Packer `at_eol` and `line_num == 0` lookaheads** — registered from the same `lc_zero`/`lc_inc` control as the `nc2_r` counter, each with a sim mirror check. Full-chain harness both configs.
- **SDP group registers as flat vectors** — as arrays they were inferred as distributed RAM whose output-register RESET the tool tied to the pixel PLL's PLL_INIT enable net (fanout 163).
- **Framebuffer `x_last_r` on its own algebra** — registered width−2; identical except during the one line a width change lands in.
- **Audio strobe accumulators** (a2mega top and dp_test) — exact reduced fraction 4/12375, 15-bit.

### Policy: the bar as a constraint

`set_clock_uncertainty 0.5 -setup` on clk100 / clk_sym / clk_pix (and
clk_logic in the core SDCs) in all five a2mega SDCs. "0 setup violations"
now means ≥ 0.5 ns real margin on every path, and the placer optimises
against it on every build. Reported slack is after the uncertainty. A
stricter requirement, not an exception. Separate commit so it can be
discussed on its own.

### Results (reported slack = real − 0.5 ns)

| Build | viols | clk100 | clk_sym | clk_pix | clk_logic | clk1x |
|---|---|---|---|---|---|---|
| 138B gate | 0/0 | +0.02 | +0.11 | +0.05 | — | — |
| 138B roll 1 | 0/0 | +0.29 | +0.04 | +0.55 | +2.36 | +2.62 |
| 138B roll 2 | 0/0 | +0.15 | +0.01 | +0.13 | +0.97 | +1.55 |
| 138B roll 3 | 0/0 | +0.01 | +0.18 | +0.00 | +1.84 | +2.39 |
| 60K gate | 0/0 | +0.05 | +0.76 | +1.72 | — | — |
| 60K roll | 0/0 | +0.26 | +0.45 | +0.19 | +3.56 | +1.93 |

Verdict: three consecutive 138B rolls of one source, the 138B gate, the
60K gate and a 60K roll all close with zero setup violations under the
0.5 ns margin constraint, i.e. every fabric-clock path on both dies has
at least 0.5 ns of real margin (138B minima across the three rolls: clk100
+0.51, clk_sym +0.51, clk_pix +0.50, clk_logic +1.47; vendor DDR3 clk1x
+2.05). No hold violations in any of these builds. The AUX ladder is no
longer the clk100 floor on either die after the fanout replication. The
bar is now self-enforcing: a roll that fell short would report a
violation instead of a small positive number.

**Bench check 2026-09-05 (ed, 138B, bitstream fd064d24 = margin-policy roll 1 of the final source):** DP link
trained through the carry-select encoder and replicated ladder, Apple II picture up, Ensoniq PASS. The 138B is
now durable and bench-verified; this is the build to base the 65816 work on.

## 138B clk100 AUX ladder after the 2026-10-22 merge (2026-10-24/25, branch claude/nostalgic-shamir-4b4f64)

The a2p25 AUX/training stack (c7064c6e..e4f2e056, ported to the a2mega in
c478d26d) grew the ladder that the 2026-09-05 pass had left as the clk100 floor
(+1.00 after the fanout replication; "ladder-limited" before it). On main
56eeef50 + the profile commits, two consecutive 138B rolls of one source went
+0.007 reported (clean) and then **−0.922 with 22 violated endpoints**, all in
`i_aux_channel`:

| cone | endpoints | worst | shape |
|---|---|---|---|
| `next_state → afe_hold → transition CE` | 16 (`dp_reg_de_active`, `tx_clock_train`, `msg`, `train_st`, `tx_powerup`, `esi_*_st`, `errcnt_st`, … CE) | −0.922 | 8-bit inequality + 12-way decode + hold in front of ~40 registers, 8 cells |
| `blind_dwell[15] → next_state D` | 5 | −0.882 | the POLITE CR-dwell term of the 100 µs advance, then 10 series mux levels of the next_state priority chain (14 cells) |
| `i_aux_messages counter → aux_tx_data` | 1 | −0.862 | the 12-bit message table decoded live every cycle, 13 LUT levels |
| `wedge_timer` 32-bit increment | 3 | −0.582 | 12 LUT cells |

The deterministic gate (`a2mega_dp_test_138b`, same ladder parameters as the
core) reproduced the knife-edge on the same source (0/0, clk100 **+0.002**,
`gate_fail_evt → snapshot CE` with 6.7 ns of route; `state → next_state`
+0.07), so it was the iteration vehicle (8 min/build) and the full core was
rolled on the final source only. Every path on this die is route-dominated
(single hops of 1.5–2.9 ns between a replica and its loads), so each fix below
cuts cells AND nets, and every one is exact: same values on every cycle.

### Fixes (`hdl/displayport/auxch/`, `hdl/displayport/video/`)

- **afe_hold without the transition term** — `(next_state != state)` was
  redundant at every consumer (the transition block ANDs it in; `state <=
  next_state` is a no-op when equal; the 100 µs reload sits inside the
  unequal branch). Then the 12-way lane-set decode became a maintained flag
  (`ns_laneset_r`, written by the same combine that writes next_state from
  the same group's value; `sos_laneset_r` beside state_on_success, `st_ls_r`
  beside state) so afe_hold is `afe_busy & one flop`; and the AND is three
  `syn_keep` copies, one per consumer group (transition block, pure-function
  block, state/count), after roll 2 drew 2.6 ns on the single ~75-load net.
- **Pure-function transition registers off the transition enable** — `msg`,
  `expected`, the `*_de_active` routing flags, `reset_addr_on_change`, the
  `tx_*` flags and the `esi_*_st` / `train_st` / `errcnt_st` decodes are
  functions of next_state alone; they are written on every un-held cycle
  from the same case statements (moved verbatim) instead of only at
  `next_state != state`. Between transitions `next_state == state`, so the
  case re-selects what the transition wrote; the hold gates both versions;
  `expected` keeps its hold on `error`/unlisted values. 16 of the 22.
- **Wait advances merged and pre-decoded** — the five `next_state <=
  state_on_success` wait sites are one site (same priority position, same
  value); their state/`expected` decodes are registered flags. Exact because
  `count_100us` reloads on every transition and its bit 14 is clear for the
  first 10,000 cycles of a state, `blind_dwell` resets on every transition
  and bits 16/21 are clear for the first 65,536, and `expected` is written
  only at a transition — wherever a dwell bit is set the one-cycle-late
  flags equal the live decodes (the `is_align_wait_r` argument, extended).
  `blind_dwell[15]` stays live: it is only read ANDed with `count_100us[14]`,
  whose first set cycle (10,000) never coincides with the dwell bit's rise
  (32,768).
- **next_state priority chain regrouped** — ~15 independent `next_state <=`
  sites (synthesised as one series 2:1 mux each, lowest priority deepest)
  now record requests in per-group blocking temporaries (last writer within
  a group wins, as before) and one statement applies the groups in the
  original order: kick > hpd > link check > timeouts > reply processing >
  wait advances. Same winner, same value, same cycle.
- **State-class flags maintained at every write of `state`** — the
  established-set membership (9 values), `state == error` and `state ==
  reset` are written by the macro that writes `state`, from the value being
  written, so each equals its decode on every cycle including the force
  sites. Retires the 9-way decode in series with the mux (the gate's floor
  after the first set) and the error-detail safety-net enable.
- **Counters** — `wedge_timer` (32) and `blind_dwell` (24) are two halves
  with a maintained "low half all ones" flag (preload: constant compare;
  hold: unchanged; count: lo+1 == FFFF ⇔ lo == FFFE); `retry_count` (29),
  `link_check_count` (27) and channel_management's `tsl_tick` (24) reload
  off a maintained zero flag (reload → reload-value == 0; count → count ==
  1 ⇔ next == 0). Bit-identical values and pulses. `TRAIN_ERRCNT == 0`
  (every board) now folds `errcnt_train` out of its decodes.
- **Message engine byte table at load time** (`dp_aux_messages.v`) — the
  187-entry `case (counter)` is a function evaluated once at message load
  into eight {we, tag, data} slots that shift out one per cycle; the three
  live bytes (`train_set_byte` lanes 0/1, `irq_clear_byte`) are tags
  multiplexed in at the output register, so they are still sampled in the
  cycle they are emitted. Cycle-exact by construction (slot k reaches the
  head when `counter[3:0] == k`; zeros shift in for the old `default`).
  Gate: LUT 230 → 128, FF 22 → 83.
- **Reply-byte branch prefix** — `rxc_nz_r == (rx_byte_count != 0)` and
  `rxc_p1_r == rx_byte_count + 1 (mod 256)` maintained at both writes of
  `rx_byte_count`, so `rx_byte_count == expected-1` is the equality
  `rxc_p1_r == expected` (the same modular identity) and the branch prefix
  is one flop; `syn_maxfan = 12` on `rx_byte_count`, `expected`,
  `just_read_from_rx` (16 on `msg`, which the engine now decodes ~90 ways at
  load).
- **AUX interface FIFO full flags registered** (`aux_interface.v`) — like
  round 2's empty flags: from the next pointer values, equal to the compare
  of the registered pointers on every cycle (rx: the rx_reset branch copies
  rd into wr, so the next value is `rd+1 == rd` = 0). The live `wr_ptr+1 ==
  rd_ptr` had headed the push strobe, the 256 FIFO write enables and the
  empty flag's adders (`tx_wr_ptr → tx_empty`, 9 cells, +0.01 on the gate
  once the ladder cones were gone).
- **channel_management snapshot** — `gate_fail_evt & !snapped_r` ran 6.7 ns
  across the die into 32 CEs. The event is registered beside a one-cycle
  delayed copy of the data and the snapshot latches from those: the same
  value is captured, visible one cycle later.
- **Two clk_sym cones outside the ladder** (roll 2 of the first final set
  drew −0.214 / −0.204 on them; both had sat at +0.00…+0.18 since 09-05):
  `pixel_cdc_fifo.rlevel` was a three-operand sum `(wptr − rptr) + rvalid +
  a_valid` (14 cells); `rptr − a_valid − rvalid` is the number of words the
  consumer has popped, so a pop counter holds that difference exactly and
  rlevel is one subtract of two registers (mirror-checked). The packer's
  two-step flag `nl2` compared `line_num(+1)` and `nc` live (9 cells in
  front of `pf_prime`/`pf_ftu`); three line_num class flags now follow
  line_num's writes exactly as `ln0_r` does, and `eol_m1_r` follows
  line_cycle's exactly as `at_eol` does, one step earlier; `nl2` is a mux
  of flops (every flag and `nl2` itself mirror-checked against the live
  expression).

Proofs. `hdl/displayport/sim/tb_aux_msg_diff.v` (new): the new engine against
the 56eeef50 table, cycle-exact over 3,000,000 random cycles / 129,719 loads.
`hdl/displayport/sim/shadow_diff.py` (new): rewrites tb_train_recover (×3
modes), tb_polite_attach (×2) and tb_link_observe so a second
channel_management built from the 56eeef50 modules runs beside the DUT on the
same inputs (forced pulses mirrored) with every output compared on every clock
— 0 mismatches over 45.5 M compared cycles on the final source (the AFE hold is
live in tb_polite_attach). The 16-run AUX suite PASS (train_recover ×3,
polite_attach ×2, gate_fail_counters, hpd_disconnect_reset, link_observe,
afe ×7, aux_idle_bias). `run_full_chain.sh` both packer configs: no underrun
over 3,037,500 cycles, 720 rows pixel-exact, ALL AUDIO CHECKS PASSED, no
mirror `$fatal`, and the 121.5 MB symbol dump is **byte-identical** to the
same harness run on 56eeef50 (the harness's 6748 Maud/VBID "ERRORS" are its
no-audio expectation and are identical on both).

### Results (reported slack = real − 0.5 ns)

| Build | viols | clk100 | clk_sym | clk_pix | clk_logic | note |
|---|---|---|---|---|---|---|
| gate, before | 0/0 | +0.002 | +0.080 | +0.658 | — | snapshot CE route; `state → next_state` +0.07 |
| gate, set 1 (hold term, pure-function regs, advances, ROM, wedge, snapshot) | 0/0 | +0.033 | +0.060 | +0.196 | — | floor: `retry_count` reload; `in_established_set → next_state` |
| gate, set 2 (+ state flags, chain regroup, counters) | 0/0 | +0.121 | +0.065 | +0.617 | — | floor: reply-byte latch CEs |
| 138B diagnostic roll (set 2) | 0/0 | +0.001 | +0.305 | +0.016 | +1.771 | `expected → ==expected−1 → next_state`; `next_state → afe_hold` |
| gate, set 3+4 (+ rxc flags, maxfan, lane-set flags) | 0/0 | +0.010 | — | — | — | floor: FIFO full/push, `tx_wr_ptr → tx_empty` |
| gate, set 5 (+ registered full flags) | 0/0 | +0.025 | — | — | — | floor: `next_state → msg D`, 6 cells / 6.4 ns route |
| 138B roll, set 5, #1 | 0/0 | +0.024 | +0.002 | +0.059 | +1.670 | clk100 floor `afe busy → train_retry_cnt CE` |
| 138B roll, set 5, #2 | **7/0** | −0.149 | −0.214 | +0.217 | +1.115 | afe_hold net (2.6 ns); packer `pf_prime`; FIFO `rlevel` |
| 138B roll, set 5, #3 | 0/0 | +0.329 | +0.107 | +0.188 | +1.554 | |
| gate, set 6 (+ hold copies, rlevel, nl2) | 0/0 | +0.078 | +0.057 | +0.527 | — | clk_sym = FIFO BSRAM DO → CEB (round-2 residual) |
| **138B final roll 1** (319d28c4) | 0/0 | **+0.772** | +0.105 | +0.045 | +0.527 | clk1x +2.76, clk_gs +1.34 |
| **138B final roll 2** (8bd8bcb9) | 0/0 | **+0.448** | +0.100 | +0.830 | +1.244 | clk100 floor = `train_interval_r → eq_dwell_cycles_r` (the ×625 multiply) |
| **138B final roll 3** (def55e2d) | 0/0 | **+0.687** | +0.046 | +0.090 | +0.637 | clk1x +0.66; clk_sym = `wptr_gray_r2 → wptr_bin_r` (gray→bin) |
| 60K roll (set 6, d4826cf7) | 0/0 | +1.373 | +0.676 | +1.146 | +2.695 | clk1x +3.06 (the doc5503 LUT +154 vs its snapshot is synthesis jitter of an untouched module) |
| a2p25 (set 6, c5373024) | 0/0 | +1.200 | +3.088 | +0.721 | clk_hr +2.912 | clk_tx +1.73, clk_mem +0.72 |

Verdict: three consecutive 138B rolls of the final source are clean on every
clock; clk100 sits at +0.45 … +0.77 reported (+0.95 … +1.27 real) with the AUX
ladder no longer the floor on any roll, against +0.007 / −0.922 before and
+0.01 … +0.29 in the 09-05 set. Resources: `i_aux_messages` LUT −60 / REG +61,
`i_aux_channel` LUT −28…−37 / REG +40, nothing else moved (the doc5503 LUT
jitter of ±76 in some rolls is synthesis seeding; the snapshots are from the
final rolls). The clk_sym floor on this die is now the pixel FIFO's block-RAM
data-out family and the gray→bin synchroniser (+0.05 … +0.11 across the three
rolls) — the "above the bar, noted" residual of cone (n), unchanged by this
pass and the next thing to pipeline if a roll ever lands below it. Not flashed:
the bitstreams of the three rolls are in `impl/pnr` history (roll 3 = the
committed `a2mega_138B.fs`).
