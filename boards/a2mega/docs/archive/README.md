# boards/a2mega/docs/archive

Status: created 2026-10-25 by the documentation consolidation
(`docs/DOCS_AUDIT_2026-10.md`, a fork-only record).
Nothing here is maintained; each entry names the document that replaced it.
Files were moved with `git mv`, so `git log --follow` still finds their history.

## ddr3-framebuffer/ (2026-06 to 2026-07)

| File | Was | Superseded by |
|---|---|---|
| [A2FPGA_DDR3_Framebuffer_Refactor_Workplan.md](ddr3-framebuffer/A2FPGA_DDR3_Framebuffer_Refactor_Workplan.md) | 720p-to-480p DDR3 framebuffer workplan; all phases done | [docs/memory-system.md](../../../../docs/memory-system.md), the headers of `hdl/video/framebuffer_480p.sv` and `framebuffer_1080p.sv` |
| [ddr3_framebuffer_480p_design.md](ddr3-framebuffer/ddr3_framebuffer_480p_design.md) | Design of the retired monolithic `ddr3_framebuffer_480p.v` | `hdl/ddr3/ddr3_ports.sv` + `ddr3_port_cdc.sv` + `hdl/video/framebuffer_480p.sv`, described in `docs/memory-system.md` |

## ensoniq-ddr3/ (2026-08-02 to 2026-08-04)

| File | Was | Superseded by |
|---|---|---|
| [ensoniq_ddr3_postmortem.md](ensoniq-ddr3/ensoniq_ddr3_postmortem.md) | Post-mortem of the campaign that moved the DOC5503 wavetable to DDR3 | [ensoniq_ddr3_pipelined_design.md](../ensoniq_ddr3_pipelined_design.md) (the shipped design) and `sim/doc5503/README.md` |

## jtag/ (2026-08-04)

| File | Was | Superseded by |
|---|---|---|
| [jtag_bridge_research.md](jtag/jtag_bridge_research.md) | Vendor and upstream research during the flashing saga (openFPGALoader, Gowin UGs, ESP32 JTAG) | [jtag_flash_reliability.md](../jtag_flash_reliability.md), whose 2026-08-15 section has the true root cause (SecurityBit), plus the recovery recipe in `docs/gotchas.md` and the `/flash` skill |

## dp-bringup-2026-08/ (2026-08-17 to 2026-08-30)

The 1.0a3 USB-C DisplayPort bring-up. Ground truth for that campaign is
[test_log.md](../test_log.md); its surviving design documents are
[m5_afe_adjust_design.md](../m5_afe_adjust_design.md), [m5_runtime_afe.md](../m5_runtime_afe.md),
[acquisition_matrix_results.md](../acquisition_matrix_results.md),
[adapter_census_results.md](../adapter_census_results.md) and
[a2p25_carryover_2026-10-11.md](../a2p25_carryover_2026-10-11.md).

| File | Was | Superseded by |
|---|---|---|
| [week_plan_0817.md](dp-bringup-2026-08/week_plan_0817.md) | One week's plan (board-out week) | executed; `board_1_0a4_requirements.md` carries what survived |
| [four_lane_rbr_scope.md](dp-bringup-2026-08/four_lane_rbr_scope.md) | Scope for moving the a2mega to 4-lane RBR | direction not taken on a2mega (stayed 2-lane HBR); 4 lanes shipped on a2p25 through the shared `hdl/displayport` core (`a2p25_carryover_2026-10-11.md`) |
| [m5_review_0821.md](dp-bringup-2026-08/m5_review_0821.md) | Disposition of the six M5 review items, all closed | `m5_afe_adjust_design.md` sections 10 to 14 |
| [m5_perlane_proposal.md](dp-bringup-2026-08/m5_perlane_proposal.md) | Per-lane ADJUST_REQUEST proposal | applied 2026-08-22; `m5_afe_adjust_design.md` section 12 |
| [mac_anker_aux_answer_key.md](dp-bringup-2026-08/mac_anker_aux_answer_key.md) | AUX conformance answer key from a Mac-to-Anker capture, with three decoded transcripts ([mac_anker_bringup_annotated.txt](dp-bringup-2026-08/mac_anker_bringup_annotated.txt), [mac_anker_hdmireplug_annotated.txt](dp-bringup-2026-08/mac_anker_hdmireplug_annotated.txt), [our_board_bringup_annotated.txt](dp-bringup-2026-08/our_board_bringup_annotated.txt)) | the link policy that came out of it is in `acquisition_matrix_results.md` and `boards/a2p25/docs/DP_ALTMODE_DPHY_DESIGN.md` section 7a |

## Board-level (2026-07)

| File | Was | Superseded by |
|---|---|---|
| [todo_2026-07_enhanced_port_bringup.md](todo_2026-07_enhanced_port_bringup.md) | The board TODO during the 2026-07 ESP32 Enhanced-port bring-up on the previous board revision (micro-SD, USB-A) | the current `boards/a2mega/TODO.md`, `board_1_0a4_requirements.md` and the ESP32 feature port (`boards/a2p25/docs/ESP32_FEATURE_PORT.md`) |

## dp_gowin/ (2026-08, fork-only)

| File | Was | Superseded by |
|---|---|---|
| [WS4_GOWIN_STACK.md](dp_gowin/WS4_GOWIN_STACK.md) | Evaluation of DP output on Gowin's own EDP IP stack (`a2mega_dp_gowin.gprj`, since removed) | direction not taken; the CSR findings it produced live on in [SOM_138B_BALLMAP.md](../SOM_138B_BALLMAP.md) and [gowin_serdes_re_findings.md](../gowin_serdes_re_findings.md). Excluded from upstream by `UPSTREAM_EXCLUDE.txt` (vendor IP discussion). |

Deleted rather than archived on 2026-10-25: `offset_wrap_fix.patch`, a copy of
the fix that `hdl/displayport/video/video_stream_packer.v` already carries (the
two `OFFSET-WRAP FIX` blocks); `offset_wrap_investigation.md` records it.
