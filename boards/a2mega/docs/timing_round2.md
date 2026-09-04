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
| 60K roll 2 | — | (pending) |
| 60K roll 3 | — | (pending) |

## Remaining cones (candidates for #2..#7, each ≤ a day, most an hour)

- **AUX engine debug/ESI latches** (clk100): `tx_rd_ptr → dbg_esi2003/esi_2005/err_rxc CE` — register the byte strobe once, latch a cycle later from a registered data copy; no `next_state` change.
- **freq_ok meter** (clk_sym): 28-bit compare in the symbol clock for a 1 Hz instrument — pipeline the compare or move it to clk50.
- **sdp_engine wb mux** (clk_sym): third pipeline stage on the selected byte (pnr_structural_analysis F3).
- **sched-ROM address** (clk_sym): `line_cycle → AD` — pipeline the lookahead address one stage earlier (schedule is line-invariant; latency compensation as in e8da12a8).
- **debug_overlay** (clk_pix): instrument path into `r_o/SET` — register the set/reset decode.
- **framebuffer wr_fifo DI** (clk_logic): `wr_accum → BSRAM DI` — register the write data before the FIFO.
