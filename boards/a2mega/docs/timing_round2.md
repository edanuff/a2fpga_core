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
- **(i) framebuffer line-buffer read address** (clk_pix): `lb_rd_addr → line_buf ADB` — register (pending ed's approval).
- **framebuffer wr_fifo DI** (clk_logic): `wr_accum → BSRAM DI` — register the write data before the FIFO.
