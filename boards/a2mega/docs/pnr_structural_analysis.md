# a2mega Full-Core PnR Structural Analysis — Why Closure Is a Lottery, and How to End It

**Analysis date:** 2026-08-31.
**Timing snapshots** are the build artifacts present in `boards/a2mega/impl/pnr/` at analysis
time (copied to a scratch dir before analysis; the tree is being rebuilt by the bench session):

| Project | Report created | .fs sha256 (first 8) | STA result |
|---|---|---|---|
| a2mega (GW5AT-60, "60K") | Sun Aug 30 23:58 | `04a08f3b` (the current timing-clean champion) | **0 setup / 0 hold** violated endpoints |
| a2mega_138B (GW5AST-138, "138B") | Sun Aug 30 22:04 | `2f10816d` | **71 setup violated endpoints**, TNS −33.4 ns (clk_sym −25.40/51 ep, clk_pix −4.55/15, clk100 −1.92/4, cm_life −1.52/1) |

Tooling: Gowin V1.9.12.01 (`gw_sh`), options per `impl/*_process_config.json`
(60K: Place 2 / Route 1 / SerDes_retiming **false**; 138B as built: Place 2 / Route 1 /
SerDes_retiming **true**; both Route_Maxfan 23, Run_Timing_Driven on).
Bench-reported closure history for **identical 60K sources**: draws of 0, 1, 21, 23, 33, 48,
69, 82 setup violations across recent runs.

---

## (a) Executive summary — verdict on the placement-luck thesis

**The thesis is confirmed, with one sharpening and one big surprise.**

1. **The 60K closes only by luck, and the luck is thin.** In the passing champion build the
   worst path meets by **+0.026 ns** — 0.35% of the 7.407 ns clk_sym period — and 14 of the
   top 25 paths are the same two `video_stream_packer` cones. Per-clock actual Fmax vs
   constraint: clk_sym **135.478 vs 135.007** (+0.3%), clk_pix 151.8 vs 148.5 (+2%),
   clk100 106.8 vs 100 (+7%), clk_logic 57.0 vs 54.0 (+5.5%, at **17 logic levels**).
   gw_sh exposes no placement seed; the only "seed" is netlist perturbation, which is why
   identical sources draw 0–82 violations. A flow whose success criterion is "roll until the
   dice land on 0" has no margin to absorb the next feature.

2. **Sharpening: the marginal families are logic-depth-limited at the source, and the
   placer's job is already near-impossible.** The clk_sym survivors carry 4.0–5.0 ns of pure
   cell delay (tC2Q+LUT) in a 7.407 ns period — 55–67% of the budget in 8–9 LUT levels —
   leaving **< 0.4 ns per routing hop** when a normal GW5A hop is 0.4–0.8 ns. The winning
   draws are the ones where the placer packs 10-node cones into a few-CLS-wide box
   (observed: data nodes of Path1 span only R54–58 × C93–98). That is not a robust
   operating point; it is the definition of luck.

3. **The 138B refutes the "utilization causes failure" reading directly.** The 138B is at
   **CLS 46% / logic 23%** (vs 60K CLS 85% / logic 44%) and times far *worse*. Its failing
   paths are **routing-dominated**: net share 55–92% (60K passing paths: 47–65%). Examples:
   `maud_measure window_cnt→strobe_cnt` fails −0.599 with only **4 logic levels** and
   **8.95 ns of net delay (77%)**; the cm_life `upar_arbiter` state loop fails −1.524 with
   2 levels and **15.24 ns of net (92%)** — a single LUT of that loop was placed at R61C107
   while its flops sit at R2C158, a ~90-column round trip. Both dies report 100%-congested
   routing tiles **only in the top-edge strip near the GTR12 quad** (60K: R2C75–97;
   138B: R2C2–5, R2C125–127) while the rest of the die is quiet. Diagnosis: the DP symbol
   domain is anchored to a hot corner (the quad's fabric clock pin), the placer smears the
   rest of the design across a die 2.3× larger, and the symbol-domain cones pay for the
   distance. **CLS% is a symptom.** Reducing fill helps the 60K only because it lets the
   packer cones collapse into a tight box more often — it does not fix the cones.

4. **Surprise (blocking, invisible until now): the 138B SDC constrains the Apple II logic
   domain at half rate.** `hdl/gowin/138B/gowin_pll/clk_pll.v` deliberately **cross-wires
   the physical PLL outputs** ("MOD clkout0=54 … clkout2=27; the swap below restores the
   shared port order": physical `PLL_inst/CLKOUT0` = 54 MHz fractional, `CLKOUT2` = 27 MHz).
   But `a2mega_138b.sdc` puts `create_generated_clock` on the **physical pins**: clk_pixel
   (27 MHz constraint) lands on the 54 MHz output and clk_logic (54 MHz constraint) lands on
   the 27 MHz output. The report confirms it: *"No timing paths to get frequency of
   clk_logic!"* while the "clk_pixel" domain shows Apple II/DOC endpoints
   (`sg/gen_doc_pipelined…`, `esp32_ospi/hdd_mem…`) with **actual Fmax 47.888 MHz at 9 logic
   levels against a 27 MHz constraint**. The clocks_138b.sv header itself says this domain
   must be constrained at **56.25 MHz (17.77 ns)** — the fractional /12/13 divider's short
   period. So the 138B's real Apple II domain has *never been timed*, is currently
   **~15% short of its true requirement as built**, and the "28–190 violations across option
   combos" understate the problem. **No amount of option scanning can close a design whose
   SDC is timing the wrong netlist.** (The 60K SDC/PLL pair is self-consistent: its report
   shows clk_logic timed at 17 levels and clk_pixel with no fabric paths, as expected.)

5. **Second structural cause, uncovered by the resource audit (section g): the design
   carries ~11–12 K flip-flops and ~6 K LUTs of storage that belongs in BSRAM.** The single
   worst offender is the framebuffer's DDR3 write FIFO — `reg [148:0] wr_fifo [0:63]` with
   an **explicit `syn_ramstyle="registers"`** — ≈ 9.5 K bits of FF plus a 149-wide 64:1
   read mux, whose read-pointer bits are the #3 and #4 highest-fanout nets in the whole
   design (`u_framebuffer/fifo_rd_ptr_r[5]` fanout **3747**, `[4]` fanout 1875, max net
   delay 5.8–6.9 ns; they even get promoted onto LW global-clock spines — and the 60K is at
   **PRIMARY 8/8 and LW 8/8, 100% clock-spine utilization**). `u_framebuffer` alone is
   8,546 FF / 4,763 LUT — 30% of all FFs in the design. This register farm is a direct,
   quantifiable cause of the CLS 85% symptom, the placement spread, and the congested
   corner competition.

**Bottom line:** stop scanning options. Fix the SDC lie on the 138B, move the misplaced
storage into BSRAM, retire the last two deep clk_sym cones structurally, and both dies
should close deterministically — with meaningful margin and materially shorter builds.

---

## (b) Ranked path families (evidence table)

Slack/level/net% from the snapshot reports (`report_timing -setup -max_paths 25`; per-path
net vs logic split summed from the arrival-path rows). "60K" column = passing champion
build; "138B" = failing snapshot. Endpoint counts from the top-25 tables and TNS summary.

| # | Family (owning module → endpoints) | Clock | 60K slack | 138B slack | Levels | Net share | Character |
|---|---|---|---|---|---|---|---|
| **F1** | `i_dp/i_video_stream_packer` flag-prefetch cone: `line_cycle_0_s1` / `line_num_7_s1` → `pf_vb0/pf_vb1/pf_ftu/pf_fnl` | clk_sym | **+0.026** (paths 1,2,5,6,7,12–15) | −0.865…−0.595 | 8–9 | 49–62% | Logic-depth-limited; double counter-increment (nc→nc2) + `>=DATA_START / <BS_POS` range compares + `vsel()` 12-way constant compare, all in one cone |
| **F2** | `i_video_stream_packer` walk cone: `running_s2` / `e_tu_pos*` → `fetch_r, dec_px/fs/fe/ph_r, e_phase, load0_r` | clk_sym | +0.101…+0.813 (paths 3,10,18,23–25) | **−1.829…−0.752** (worst on die: `e_tu_pos_1→dec_fe_r_1`) | 7–11 | 47–69% | The registered early-walk (Bresenham + phase + fetch decisions) is still one cone from state flops to decision flops |
| **F3** | `i_sdp_engine` wire-byte mux: `wb_idx_b_r` → `wb_b_r` | clk_sym | +0.285 (path 8) | (below top-25 cut) | 6 | 62% | 64-way byte select over {256b fdb, 32b fhb/fpbh, 64b fpbd}; span R2–73 × C2–101 — placement-stretch victim with a wide mux underneath |
| **F4** | `i_maud_measure`: `window_cnt` → `strobe_cnt/maud` CEs | clk_sym | (not in top 25) | **−0.599 × 6 endpoints** | **4** | **77%** | 24-bit terminal-count compare into CE. Pure placement victim on the big die; trivially fixable in RTL |
| **F5** | `i_aux_channel` FSM: `i_aux_interface/tx_wr_ptr` and `count_100us` → `next_state` | clk100 | +0.636/+0.779 (paths 17,22) | −0.663/−0.589/−0.485 | 11–12 | 59–63% | ~100-state 8-bit-encoded FSM; FIFO-pointer compare + 15-bit timer compare feed the giant next_state mux (history: this family has been shaved repeatedly — c98cbbdf, cc0fa914) |
| **F6** | `debug_overlay` pipeline: `s0_x_r→sA_in_space/is_hex`; `s1_rom_addr→s2_font_row` (the `n1080_s26xx` mux = **char_rom in LUTs**) | clk_pix | +0.147…+0.592 (paths 4,9,11,16) | −0.423 | 7–11 | 50–58% | 128×8 `char_rom` has no romstyle attribute → synthesized as LUT mux; plus serial `rel_x` subtract/compare chain |
| **F7** | `u_framebuffer` h-tracker: `fb_width_px_r` → `lb_rd_addr[…]` | clk_pix | +0.740 ×3 (paths 19–21) | **−0.698** | 5–10 | 65–71% | Quasi-static config value (`fb_width_px_r[9:0]−1` compare) inside the per-pixel address CE cone |
| **F8** | `upar_arbiter` state loop (138B only) | cm_life | (clean, +6.5 recovery margin) | **−1.524**, net **92%** | 2 | 92% | One LUT of a 3-element loop placed 90 columns from its flops; characteristic register-move smear — prime suspect: `SerDes_retiming: true` on the 138B build (60K champion has it off) |
| **F9** | **Hidden**: 138B Apple II domain mis-labeled "clk_pixel" | (should be 56.25 MHz) | n/a | **Fmax 47.888 MHz, 9 levels, 0 reported violations** | 9 | — | Constrained at 27 MHz due to the SDC/PLL pin swap (section a.4). Real, currently invisible, ~15% short |
| **F10** | 60K clk_logic domain | clk_logic | Fmax 56.998 vs 54.0, **17 levels** | (mis-constrained, see F9) | 17 | — | Passing, but the deepest cone in the design; 5.5% margin. Watch-list, not urgent |

DDR3 PHY hold paths (`u_ddr_phy_cmd_io/oserdes_*`, +0.084 worst) are vendor-IP and healthy.
`pixel_cdc_fifo` appears only as a comfortable hold path (BSRAM `DI` pins) in these
snapshots — its storage is correctly in BSRAM; earlier-draw appearances of
`i_pixel_cdc_fifo` on the setup list were the F1/F2 cones reaching the FIFO's registered
interfaces, not the FIFO itself.

---

## (c) Per-family structural fixes

**F1 + F2 — video_stream_packer (the design's #1 family).**
The module is already heavily hand-pipelined (shadow-model-verified early walk, flag
pre-registration). What remains is structural, and there are two tiers:

* *Tier 1 (mechanical, low risk):* the F1 cone starts with a **double combinational
  counter advance** (`line_cycle → nc → nc2 → s0n2/s1n2 → range compares`). Maintain a
  registered `line_cycle_next` (and `line_num_next`) so the two-step basis starts from
  flops — removes 2–3 levels from every pf_* path. Similarly split the F2 walk by
  pre-computing `tu_valid`/`bres_err` **one TU ahead**: the Bresenham update only fires at
  `tu_pos==0`, once per 32 cycles — it has 31 spare cycles and does not belong in the
  per-cycle cone. Effort: ~1–2 days incl. extending the existing shadow-model assertions.
  Expected: 2+ levels off both cones; on current evidence that converts +0.026 into real margin.
* *Tier 2 (the endgame, kills both families outright):* the per-line symbol schedule
  (pixel/FS/FE/BS/VB-ID classification per slot, `tu_valid` sequence) is **identical for
  every line** — the Bresenham walk restarts from zero each line. Precompute the schedule
  once into a small BSRAM (SYMS_PER_LINE/2 = 1800 entries × ~8 flag bits ≈ one 18 Kb block,
  fillable by a boot-time walker or `initial`-computed) and replace the entire runtime walk
  with a table read addressed by `line_cycle`. The clk_sym datapath collapses to
  BSRAM-read → registered flags → the existing mux. Effort: ~3–5 days with the existing
  TBs (tb_dp_frame_audio pixel-exact harness already exists). Expected: family eliminated;
  clk_sym worst path moves to the 8b10b/lane encoders.

**F3 — sdp_engine.** Emission order through `wire_byte` is a fixed sequence (indices come
from a counter, already 2-deep pipelined by 0ce3b316). Replace random-access byte selection
with **shift-register serialization** (load `db/fhb/fpbh/fpbd` wide, shift 8/16 bits per
cycle) — the 64-way mux disappears; or, cheaper, add a third pipeline stage on the selected
byte. Effort: 1–2 days (audio checker exists). Expected: family off the list on both dies.

**F4 — maud_measure.** Convert to a down-counter with a **registered terminal flag**
(`window_last_r <= (window_cnt == WINDOW_CYCLES-2)`), use the flag as the CE. 3-line
change, no behavior change (window is 2^23 cycles). Kills 6 of the 138B's 51 clk_sym
endpoints for an afternoon's work.

**F5 — aux_channel / aux_interface.** This FSM makes millisecond-scale policy decisions
and does not need single-cycle 100 MHz turnarounds. Options, cheapest first:
(1) extend the proven registered-decode pattern (c98cbbdf) to the timer and pointer terms —
pre-register `count_100us_done` and the tx/rx-pointer equality flags; (2) re-encode the
8-bit state one-hot (`syn_encoding`) to flatten the range compares (`next_state >=
clock_training && <= align_wait_after`); (3) if it ever resurfaces: run the whole
channel-management block on a divided enable with a multicycle. Expected: clk100 family
retired; also relieves the 138B AUX cluster (placed R86–89 × C105–116, 8.7 ns of net on the
worst path — a smaller cone tolerates the stretch).

**F6 — debug_overlay.** Mirror the documented in-repo fix (`osd_text_overlay.sv:75-84`):
pure sync read + `syn_romstyle="block_rom"` for `char_rom` (mind the "`~rom[addr]` blocks
block-ROM mapping" trap recorded there). Also pre-register the `rel_x` window compares
(the `rel_x_5..9` serial chain visible in paths 4/11). Effort: hours. Note DebugOverlay is
diagnostic equipment — the alternative is compiling it out of release builds.

**F7 — framebuffer h-tracker.** `fb_width_px_r` is quasi-static config: register
`fb_width_last_r <= fb_width_px_r[9:0] - 1` and compare against that (same idiom as the
already-fixed `h_trigger_r` pre-registration two lines above it). Effort: minutes.

**F8 — 138B upar_arbiter smear.** First lever is **SerDes_retiming → false** on the 138B
(the 60K champion runs without it; the single-LUT-90-columns-away pathology is what
register retiming plus a big empty die produces). If it persists, `syn_keep`/`syn_preserve`
the arbiter state registers. Zero RTL risk.

**F9 — the 138B SDC swap (do first, see campaign).** Re-point the two
`create_generated_clock` statements so the *logical* taps are constrained (per
`clk_pll.v`'s cross-wiring: 54 MHz is physical `CLKOUT0`, 27 MHz is physical `CLKOUT2`),
and set the logic-domain period to **17.777 ns / 56.25 MHz** as the clocks_138b.sv header
already instructs (fractional /12/13 divider — the short period is the constraint). Then
rebuild and expect *new, honest* violations in the Apple II domain (as-built Fmax 47.9 MHz);
they are currently shipping untimed.

**SDC hygiene beyond F9.** The clock-group matrix is sound (all exchanges audited async —
see `cdc_constraint_audit.md`). Two crutches to retire with their causes: the cm_life
CSR-ROM multicycles (both SDCs, deleted when the ROM moves to BSRAM, section g) — and note
there are **no input/output delay constraints at all**; acceptable while all external
interfaces are IP-managed (DDR3) or slow (ESP32 OSPI, Apple bus), but worth one deliberate
pass, especially for the OSPI pins.

---

## (g) Resource-mapping audit — storage in the wrong fabric

Method: `Extracting RAM` messages in `impl/gwsynthesis/a2mega.log` (48 arrays) cross-checked
against the **per-module** rows of `a2mega_syn_resource.html` (REG/LUT/BSRAM/SSRAM/ROM16
columns) and the PnR resource summary. Key lesson baked into the table: an "Extracting RAM"
log line does **not** mean BSRAM — `char_rom` is "extracted" and still lands as LUTs
(module row: BSRAM 0, ROM16 0).

60K totals for scale: 28,204 REG / 23,238 LUT + 2,590 ALU / **CLS 25,336/29,952 = 85%** /
BSRAM 95/118 (23 free) / PRIMARY & LW clock spines 8/8 = 100%.

| Array (file:line) | Size | Current mapping (evidence) | Correct mapping | Recoverable mass |
|---|---|---|---|---|
| **`wr_fifo`** (`framebuffer_1080p.sv:257`) | 149 b × 64 | **FF farm by explicit `syn_ramstyle="registers"`** + combinational show-ahead read (`fifo_head_w = wr_fifo[rd_ptr]`); no recorded rationale for the attribute. `u_framebuffer` = 8,546 FF / 4,763 LUT; `fifo_rd_ptr_r[5]/[4]/[3]` = fanout 3747/1875/939, promoted to LW spines | 5 × SDPB (×36) BSRAM, 1-cycle prefetched head (small FSM change; same-clock, no CDC involved) | **≈ 8–9.5 K FF + ≈ 3 K LUT** — the single biggest placement mass in the design |
| `rd_fifo` (`framebuffer_1080p.sv:1073`) | 32 b × 16 | FF + 16:1 mux | 1 × SDPB, or leave (borderline size) | ≈ 0.5 K FF + ~0.2 K LUT |
| **`u_csr_rom`** (`transceiver_bank_gowin.v:699`, per-die `csr_replay_rom_lut_*.v`) | 56 b × 399 (138B) / smaller (60B) | Pure combinational case → **1,048 LUT (60K) / 756 LUT (138B)**, 0 FF; papered over by the replay_idx→drp_* multicycles in *both* full-core SDCs | pROM (2 blocks ×36), registered read — the FSM already holds `replay_idx` ≥16 cycles | ≈ 1.0 K LUT (60K), **plus deletion of the multicycle crutches** and the recurring cm_life knife-edge (100.03–101.3 Fmax history in the archive log) |
| **DOC oscillator banks** (`doc5503_pipelined.sv`, `gen_banks_ff.*`) | 7 × 256 b + 768 b acc | FF banks — deliberately: `top.sv:661` sets `DOC_BANKS_IN_BSRAM(0)` ("FF register banks: 0 BSRAM vs ~2k FF … GW5A has no distributed RAM") | Flip the existing, co-validated `BANKS_IN_BSRAM=1` path (6 DPB), or the 1-block scheduled-SDPB endpoint already designed in `ensoniq_ddr3_pipelined_design.md` | ≈ 2.5 K FF + ≈ 1.5 K LUT for 6 (or 1) of the 23 free BSRAMs. The 44%-regs rationale predates this audit; at CLS 85% the trade has flipped |
| `tx_fifo`/`rx_fifo` (`aux_interface.v:102/126`) | 2 × 32×8 | FF farms (module: 675 FF/500 LUT); pointer compares feed the F5 clk100 family | Could share one 18 Kb BSRAM; or keep FF but pre-register the pointer-equality flags (F5 fix) | ≈ 0.5 K FF (minor; do with F5) |
| `char_rom` (`debugoverlay.sv:43`) | 128×8 | "Extracted" then implemented as **LUT mux** (module BSRAM 0, ROM16 0 → the `n1080_s26xx` timing cone, family F6) | sync read + `syn_romstyle="block_rom"` (or ROM16) | ~0.15–0.2 K LUT + retires F6's deep half |
| `db`/`db_d` + `wire_byte` (`sdp_engine.v`) | 2×256 b + headers | Registers + 884 LUT of byte-select mux | Registers are fine; the *mux* is the problem → shift-based serializer (F3) | ~0.5 K LUT |
| Verified **correct**: | | `pixel_cdc_fifo` mem (BSRAM, DI pins in hold report); `osd_vram`/`lane_mem`/`hdd_mem` (`block_ram` attrs, esp32_ospi = 11 BSRAM); OSD `viderom_r` (`block_rom`, with the documented `~rom[addr]` fix); `apple_video_gen` (24 ROM16 + 1 BSRAM); DDR3/f18a/uthernet2 vendor+library RAMs | | |

**Aggregate recoverable: ≈ 11.5–12.5 K FF (~41% of current FF usage) and ≈ 5.5–6 K LUT
(~23%) for ≈ 8–14 of the 23 free BSRAMs.** Projected 60K CLS: from 85% to the low-to-mid
60s — *with the three dieted features (SuperSprite/Mockingboard/SuperSerial, ae19b863) still
off; this recovery is what buys them back.* On the 138B the same conversions shrink the
placement problem the placer is currently failing to solve by spreading.

**GW5A idioms (the team's operating rules going forward, per this toolchain's observed
behavior + the repo's own findings):**
1. GW5A has **no writable distributed RAM** (SSRAM row reads 0 in every build; the
   `top.sv:661` analysis is correct) — every array ≥ 64 bits is a *decision*: BSRAM, or a
   deliberately-justified FF farm with a comment. Default BSRAM.
2. BSRAM inference needs a **pure registered read** (`q <= mem[addr]` alone in a clocked
   block — no inversion/logic inside the index or read expression; the `~rom[addr]`
   blocker is documented at `osd_text_overlay.sv:75`), write and read in separate
   processes, no same-block read-modify-write.
3. **Attribute explicitly** (`syn_ramstyle="block_ram"` / `syn_romstyle="block_rom"`) and
   **verify in `*_syn_resource.html`** — the module row must show the BSRAM count.
   "Extracting RAM" in the log is not confirmation.
4. Constant tables ≤ ~16 deep: ROM16 is fine (LUT-cost, no block).
5. Any FF array whose read index has > ~16 consumers will put its index bits on global
   routing — check the High Fanout Nets report after adding one.

---

## (d) Prioritized campaign plan

Order of operations. Each step ends with one build per die and a recorded Fmax table +
top-25; **success criterion is worst setup slack ≥ +0.3 ns (≈4% of the clk_sym period)
sustained across 3 consecutive draws** — not a lucky 0/0.

1. **138B SDC swap fix (F9).** One-file change, no RTL. Until this lands, every 138B
   timing number is partially fiction and every option-scan result is noise. Also flip
   **SerDes_retiming → false** on the 138B in the same build (F8; matches the 60K champion
   and isolates the retiming variable). Expect the violation count to *change character*,
   not necessarily improve — that's the point: it becomes honest.
2. **`wr_fifo` → BSRAM (+ optionally `rd_fifo`).** Biggest single de-bloat; removes the
   3.7 K-fanout selects, frees LW spines, drops ~30% of FF placement mass. This is the step
   most likely to turn the 60K lottery into routine closure on its own.
3. **`u_csr_rom` → pROM, both dies; delete the replay multicycles from both SDCs.** Ends
   the recurring cm_life knife-edge permanently.
4. **DOC banks → `DOC_BANKS_IN_BSRAM(1)`** on both a2mega tops (both modes co-validated by
   the existing suites; run the pending listening pass on the next hardware session).
5. **The small-RTL batch (F4, F6, F7 + F5's registered compares).** Four independent,
   hours-scale edits; land together, one build.
6. **Reassess.** If clk_sym still owns the worst paths, do **F1 Tier 1** (registered
   counter basis + TU-ahead Bresenham). Only reach for **Tier 2 (schedule table)** and the
   **F3 serializer** if the +0.3 ns criterion still isn't met — on current numbers, steps
   2–5 plus Tier 1 very likely get there.
7. **Only now** re-approach the 138B seriously: with honest SDC, de-bloated netlist,
   retiming off. If it still misses, the remaining tool is *placement locality for the DP
   corner* — but see the do-not-do list: region constraints are booby-trapped on this
   toolchain; the working substitute is shrinking the cones (already done) and the mass
   (step 2) so the placer's default clustering suffices.
8. **Re-enable the dieted features on the 60K** (ae19b863) once margin is banked, feature
   by feature, gated on the same +0.3 ns criterion.

---

## (e) Build-time findings

From the PnR reports (elapsed):

| Phase | 60K | 138B |
|---|---|---|
| Placement Phase 0–1 | 0:49 | 1:06 |
| Placement Phase 2 | 4:29 | 7:06 |
| **Placement Phase 3** (timing-driven detail) | **11:04** | 5:14 |
| **Routing Phase 1** | **7:24** | **13:04** |
| Output generation | 0:18 | 0:29 |
| **Total PnR** | **24:06** | **27:04** |

Synthesis adds ~3–5 min (log mtime 23:33 vs PnR start ≈23:34). Peak memory 2.0–2.3 GB.

* Where the time goes tracks the disease: on the 60K, **timing-driven detailed placement**
  (Phase 3, 11 min) is the placer fighting to pack knife-edge cones at 85% fill; on the
  138B, **routing** doubles (13 min) because placement spread makes every net long. Both
  shrink when mass drops and cones shorten — the previous board's 5-minute experience is
  the same causality. **Treat any >~5-minute build as a symptom to investigate, not a cost
  to accept.**
* This toolchain **does expose incremental flows**: `INCREMENTAL_PLACE_AND_ROUTING` /
  `INCREMENTAL_PLACE_ONLY` (currently "0") and an `Incremental_Compile` slot in the process
  config. Worth a bench trial *for iteration builds only* — never for release/archive
  bitstreams (provenance rule: hash-logged full builds).
* Keep the Gowin IDE closed during batch builds — the 2026-08-14 "multicycle stall" was
  actually gw_sh license contention with an open IDE (memory note); it inflates wall time
  nondeterministically.
* For DP-family iteration, the `a2mega_dp_test*` projects build a much smaller netlist —
  use them as the fast loop where the change is DP-local, then confirm on the full core.
* Option levels are *not* a runtime lever worth mining: recorded history shows the same
  option changing sign between builds (Place 1 = 26 viol in one run; Place 2 = 130 viol on
  dp_test in another era, later the champion). Their runtime differences are minutes;
  their timing differences are noise around the structural signal.

---

## (f) Do-NOT-do list

1. **No more option scanning as a primary strategy.** Identical sources drew 0→82
   violations; gw_sh has no seed control, so a scan is sampling the same lottery with
   relabeled tickets. History already shows option effects reversing between netlists
   (c98cbbdf vs 0ce3b316 champion notes). Scan only *after* the structural steps, once, to
   pick a default.
2. **No `SerDes_retiming` on the 60K** (champion is `false`; the 138B's smeared
   single-LUT cm_life loop is the signature failure mode of retiming + empty die — turn it
   off there too, step 1).
3. **No GROUP/GRP_LOC region floorplanning on GW5A with V1.9.12.x.** Tried twice, with and
   without the CDC FIFO BSRAMs in the group: "pathological legalization — 50 minutes and
   9 HOURS of runtime respectively, both killed" (`a2mega.cst:365` note). Re-evaluate only
   on a new toolchain version, on the dp_test project first.
4. **No new multicycle/false-path crutches on real paths.** The cm_life multicycles are a
   documented stopgap scheduled for deletion with the ROM conversion — that pattern
   (constrain around a structure instead of fixing it) is what left the 138B's swapped
   domain undetected. Every SDC exception must name the *structural invariant* that makes
   it safe (the replay-ROM one does; imitate the standard, not the shortcut).
5. **Do not trust "Extracting RAM" log lines** — verify the per-module BSRAM column in
   `*_syn_resource.html` (char_rom is the counterexample).
6. **Do not re-enable SuperSprite/Mockingboard/SuperSerial on the 60K before step 2 lands**
   — they were removed because closure was "a per-draw lottery" at CLS 92%; adding them
   back into an 85% design reruns the same experiment.
7. **Do not touch the 138B option matrix before its SDC is fixed** (step 1) — every result
   gathered against the swapped constraints is unusable, including the existing
   "28–190 violations across combos" dataset.
8. **Do not archive/flash any 138B full-core bitstream as "timing clean"** until F9 is
   fixed — current clean-looking clk_logic rows are vacuous (zero paths timed), and the
   real domain is ~15% short as built.

---

*Method note: all numbers quoted are from the snapshot artifacts listed in the header
(`a2mega{,_138B}.rpt.txt`, `*_tr_content.html` parsed per-path with net/logic/fanout/LOC
extraction, `*.timing_paths`, `*_syn_resource.html`, `impl/gwsynthesis/a2mega*.log`) and
from the named RTL/SDC/CST files at the working-tree state of 2026-08-31. The tree is
actively rebuilt by the bench session; re-run the extraction against fresh artifacts before
citing these numbers for a build other than `04a08f3b` / `2f10816d`.*
