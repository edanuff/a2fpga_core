# Documentation audit, 2026-10

Analysis only. Nothing was moved, deleted or committed. Worktree
`claude/unmerged-worktrees-branches-0abece` at main f0d93bb9 (HEAD 105e56b5,
2026-10-24), audited 2026-10-25. Method: `git log --follow` first/last commit per
file, the file's own status lines, an inbound-reference map (`git grep` of each
basename across the tree, excluding `impl/`), a link checker over markdown links
and backticked repo paths, and a read of every file's head, section list and
status markers. Where a doc contradicts the code the code was checked
(`top.sv`, `.gprj`, `hdl/`).

## 1. Executive summary

1. 101 files are in scope (5 root, 21 `docs/`, 47 `boards/a2mega/docs/`, 3 Enhanced docs, 4 BL616 docs, 1 GS doc, 10 `boards/a2p25/docs/`, 7 board READMEs, 3 READMEs under `hdl/`, `sim/`, `tools/`); 14 adjacent documentation files (board `TODO.md`s, sub-project READMEs) are tabled separately.
2. By stage: 67 current, 22 reference-keep, 3 superseded, 7 obsolete, 2 direction-not-taken.
3. By action: 48 keep, 33 update, 18 archive, 1 delete, 1 ask-ed (plus 1 ask-ed in the adjacent set).
4. By upstream suitability: 89 public, 4 fork-only (the two merge reports, `WS4_GOWIN_STACK.md`, `gowin_serdes_re_findings.md`; three of these also carry absolute paths), 8 needs-redaction (absolute `/Users/` paths; 14 such lines in tracked docs, listed in 6.5). No personal data and no third-party documents were found in the tree; two public files carry harmless `/Volumes/` paths.
5. The June 2026 wiki (`docs/`) is structurally sound but 15 of its 21 pages describe the June state: a2mega as a 60K HDMI board with BSRAM Ensoniq, a2p25 as a no-memory LCAM board with the ES5503 on the ESP32, Disk II "not built", Enhanced without a framebuffer. All of that is now wrong in code.
6. `boards/a2mega/docs/` is 47 files from one four-month bring-up (DDR3 refactor, Ensoniq-on-DDR3, JTAG flash saga, 1.0a3 USB-C DP bring-up, M5 AFE, census, timing round 2, GS socket). 26 are still live reference and cited from code comments; 13 are campaign history that should move to an archive; the rest need status headers.
7. The a2p25 docs are the most current set in the repo (October), but the board README still says "Coming soon" and links three LCAM-era files.
8. Three doc families have a clear survivor: DDR3 framebuffer (`memory-system.md` + code; the 480p design and workplan are history), Ensoniq-on-DDR3 (`ensoniq_ddr3_pipelined_design.md`; the postmortem is history), M5 AFE (`m5_afe_adjust_design.md` + `m5_runtime_afe.md`; review and proposal are history).
9. Twelve broken cross-references were found, including `README.md -> src/`, `peripheral-cards.md -> hdl/cardrom/cardrom.sv`, `memory-system.md -> boards/a2n20v2-GS/hdl/video/sdram_framebuffer.sv`, and `gs_socket_65816_scoping.md -> hyperram_scenarios.md` (a file that was never committed).
10. Upstream exclusion today is a checklist in memory; this audit proposes a committed `UPSTREAM_EXCLUDE.txt` honoured by an export script, plus a `/Users/` grep gate.

## 2. Proposed target structure

### 2.1 `docs/` as the living wiki

Keep `docs/` flat (ADR-0001) but make the index group the pages:

| Group | Pages |
|---|---|
| Start here | `README.md`, `getting-started.md`, `setup-gowin-cli.md`, `build_profiles.md`, `conventions.md`, `gotchas.md` |
| Architecture | `architecture.md`, `video-pipeline.md`, `memory-system.md`, `audio.md`, `bus-interface.md`, `coprocessor-interface.md`, `peripheral-cards.md` |
| Boards and direction | `boards.md` (matrix plus a per-board doc index), `ROADMAP.md` |
| Decisions | `adr/` |
| Hardware notes | `gameport-dongle.md` (+ `images/`) |
| Deep references | `memory_bandwidth_analysis.md` |
| Archive | `archive/` (see 2.2) |

Gaps the wiki does not cover and that currently live only in board docs (recommended new pages, not written by this audit): a `displayport.md` (the shared `hdl/displayport` core, its two PHY backends GW5A SERDES and GW5A-25 D-PHY, link policy, AUX front end, the census verdicts), a `testing.md` (`sim/doc5503`, `hdl/displayport/sim/run_full_chain.sh`, `boards/a2p25/tests`, the dp_test gate), and an `upstream-release.md` (the squash checklist that today lives in agent memory, pointing at `UPSTREAM_EXCLUDE.txt`). Candidate ADRs implied by decisions already recorded in logs: timing policy (no re-rolls, 0.5 ns uncertainty), `bit_security 0` everywhere, DOC on memory ports behind a pipelined fetch, per-board `*_profile.vh`, `Convert_SDP32_36_to_SDP16_18` left on.

### 2.2 Where archives go: one `archive/` per folder

Recommendation: `docs/archive/`, `boards/a2mega/docs/archive/`, `boards/a2p25/docs/archive/`, `boards/a2n20v2-GS/docs/archive/`, each with a one-page `README.md` saying what lives there, which document superseded each file, and the date range it covers.

Why per folder rather than a single `docs/archive/<topic>/`:

- `conventions.md` already fixes the discoverability chain `docs/README.md -> boards.md -> board README -> boards/<board>/docs/`. Board history stays reachable through the same chain; a central archive would need a second index.
- Many archived files are cited from code comments and sibling docs with board-relative paths (`docs/foo.md` from `boards/a2mega/hdl/*.sv`). A one-level move keeps those edits to a path prefix.
- Board history is board-scoped by nature (the a2mega DP campaign means nothing to the Tang Nano boards).

Inside `boards/a2mega/docs/archive/` group by topic because 13 files land there: `ddr3-framebuffer/`, `ensoniq-ddr3/`, `jtag/`, `dp-bringup-2026-08/`, `dp_gowin/` (fork-only). The other archives stay flat. `docs/archive/merges/` holds the two 2026-10-22 merge reports.

### 2.3 Upstream exclusion mechanism

Add `UPSTREAM_EXCLUDE.txt` at the repo root: one gitignore-style pathspec per line, comments allowed. The upstream squash step (proposed `tools/upstream_export.sh`) does:

1. `git worktree add` an orphan export branch from the squash commit.
2. `git rm -r -q --cached --pathspec-from-file=UPSTREAM_EXCLUDE.txt --pathspec-file-nul=false` with `:(glob)` magic, then `git clean` the removed paths from the export tree.
3. Gate: `git grep -n '/Users/' -- ':!*.fs' ':!impl'` must be empty, and every markdown link must resolve (reuse the link checker from this audit); fail the export otherwise.
4. Print the removed-file list for ed's review before the push.

Initial contents:

```
# UPSTREAM_EXCLUDE.txt - paths that never leave the edanuff fork.
# Honoured by tools/upstream_export.sh. One pathspec per line, :(glob) semantics.

# Private records of the fork's own history
BRANCH_AUDIT_*.md
MERGE_RESOLUTIONS_*.md
docs/archive/merges/
docs/DOCS_AUDIT_*.md

# Gowin vendor-IP evaluation and reverse-engineering notes (third-party IP)
boards/a2mega/docs/archive/dp_gowin/
boards/a2mega/docs/WS4_GOWIN_STACK.md
boards/a2mega/docs/gowin_serdes_re_findings.md
boards/a2mega/a2mega_dp_gowin.gprj
boards/a2mega/hdl/dp_gowin/

# Vendor documents, if any ever reappear (board schematics under boards/*/sch are ours and stay)
docs/**/*.pdf
boards/*/docs/**/*.pdf

# Outreach and correspondence drafts
**/outreach/

# Agent tooling
.claude/
```

Two lines are policy questions for ed (section 5): whether the bench journals (`test_log.md`, `bringup_1_0a3_log.md`, the acquisition and AUX captures) go upstream, and whether `.claude/` does.

## 3. Per-folder tables

Columns: file, last commit (`git log --follow`, `%cs`), stage, upstream, action, note. "Code-cited" means a comment in `hdl/`, `boards/*/hdl`, `src`, `tools` or a skill names the file; those stay in place.

### 3.1 Repository root

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `AGENTS.md` | 2026-09-05 | current | public | update | Add rows for build profiles, the archive convention and `UPSTREAM_EXCLUDE.txt`; the "find work" row points at TODOs that are stale (3.9). |
| `CLAUDE.md` | 2026-06-14 | current | public | keep | Stub per ADR-0001. |
| `README.md` | 2026-08-27 | current | public | update | Broken link `src/` and a PicoSoC paragraph (removed path); a2mega described as Tang Mega 60K only (138K SOM, USB-C DP since 1.0a3). |
| `BRANCH_AUDIT_2026-10-22.md` | untracked | reference-keep | fork-only, needs-redaction | archive | Record of the branch census; `/Users/` on lines 110 and 214. To `docs/archive/merges/`. |
| `MERGE_RESOLUTIONS_2026-10-22.md` | untracked | reference-keep | fork-only | archive | 2748 lines of conflict diffs; sections 3 and 6 list still-open work (138B aux_channel regression). To `docs/archive/merges/`. |

### 3.2 `docs/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `README.md` | 2026-10-24 | current | public | update | Add the groups in 2.1, an archive section and a pointer to each board's doc index. |
| `ROADMAP.md` | 2026-06-14 | obsolete | public | update | Themes 3 to 5 describe June: a2p25 ES5503 on the ESP32, a2mega "60K bring-up", Enhanced "audio-clock fix, OSD, Disk II" (all shipped). Rewrite against current TODOs. |
| `adr/0001-agents-md-as-router.md` | 2026-06-14 | current | public | keep | |
| `adr/README.md` | 2026-06-14 | current | public | update | Only one ADR exists; add the candidates listed in 2.1. |
| `adr/template.md` | 2026-06-14 | current | public | keep | |
| `architecture.md` | 2026-06-14 | current | public | update | Layout table lacks `hdl/displayport`, `hdl/hyperram`, `hdl/disk`, `hdl/uthernet2`; "renders to 720x480 HDMI" (DP 1080p on a2mega/a2p25). |
| `audio.md` | 2026-06-14 | current | public | update | Contradicts code: a2mega DOC RAM "in BSRAM" (DDR3 pipelined DOC since 08-04, `top.sv:652`); a2p25 "ES5503 on the ESP32 via I2S" (fabric `doc5503_pipelined` on HyperRAM, no I2S); HDMI-only chain (DP SDP audio). |
| `boards.md` | 2026-07-04 | current | public | update | a2p25 row "no off-chip memory, no FB possible" (HyperRAM framebuffer live); a2mega row 60K only, no `a2mega_138B.gprj`, HDMI implied; Enhanced `_dualrate`/`_fb` variants missing; a2mega flash recipe lacks the 500 kHz bulk-erase rule; per-board docs table lists 6 of 47 a2mega docs. |
| `build_profiles.md` | 2026-10-24 | current | public | keep | |
| `bus-interface.md` | 2026-07-05 | current | public | update | Data-mux example predates Disk II, HDD and Uthernet II; add the GS-socket mode and IIgs autodetect pointer. |
| `conventions.md` | 2026-06-14 | current | public | update | Add the `*_profile.vh` rule and the archive convention; "feature toggles are `top.sv` parameters" is now half true. |
| `coprocessor-interface.md` | 2026-06-14 | current | public | update | Contradicts code: a2p25 listed with `esp32_spi_connector` (3-wire); `a2p25.gprj` builds `../a2mega/hdl/esp32/esp32_ospi_connector.sv`. The 3-wire path survives only in `legacy_a2p25_rev0.gprj`. |
| `gameport-dongle.md` | 2026-07-05 | current | public | keep | Rev 0 design note, matches `hardware/gameport-dongle/`. |
| `getting-started.md` | 2026-06-14 | current | public | update | ESP32 toolchain link points at the LCAM-era `boards/a2p25/src/a2fpga_esp32/README.md`; should point at `boards/a2mega/src/a2fpga_esp32/` and `ESP32_FW_A2P25.md`. |
| `gotchas.md` | 2026-09-05 | current | public | update | One contradiction: "the Ensoniq wavetable is NOT simply movable to DDR3" (the pipelined DOC shipped on DDR3 08-04 and on HyperRAM 10-11). Everything else current. |
| `images/gameport-dongle-schematic.svg` | 2026-07-05 | current | public | keep | Used by `gameport-dongle.md`. |
| `memory-system.md` | 2026-06-14 | current | public | update | Broken link to `boards/a2n20v2-GS/hdl/video/sdram_framebuffer.sv` (now `hdl/video/`); "Enhanced has no framebuffer" (fb and dual-rate variants); a2mega ports 4-5 "BSRAM-backed sound"; no HyperRAM backend (`hdl/hyperram`, `hyperram_ports`). |
| `memory_bandwidth_analysis.md` | 2026-08-23 | reference-keep | needs-redaction | update | Own note says sections 1-2 are superseded by `VGC_IN_SDRAM`; section 3 is revised by `MODERNIZATION_PLAN.md` section 4. `/Users/` on lines 699 and 963. Add a header, redact, keep in `docs/`. |
| `peripheral-cards.md` | 2026-06-20 | current | public | update | Contradicts code: Disk II "not instantiated on any current board" (slot 5 on Enhanced, a2mega, a2p25); no ProDOS HDD card (`hdl/disk/hdd.sv`, slot 6); CardROM link to `hdl/cardrom/cardrom.sv` is broken (only `src/cardrom/*.s`). |
| `setup-gowin-cli.md` | 2026-09-05 | current | public | keep | |
| `video-pipeline.md` | 2026-06-19 | current | public | update | a2p25 listed under legacy raster boards (HyperRAM framebuffer + DP 1080p); "HDMI" used where a2mega/a2p25 drive `dp_transmitter` through `framebuffer_1080p`. DebugOverlay section is current. |

### 3.3 `boards/a2mega/docs/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `A2FPGA_DDR3_Framebuffer_Refactor_Workplan.md` | 2026-06-14 | obsolete | public | archive | 720p-to-480p workplan, phases done; names the retired `ddr3_framebuffer_480p.v`. To `archive/ddr3-framebuffer/`. |
| `BRINGUP_1_0A3_PLAN.md` | 2026-08-28 | reference-keep | needs-redaction | keep | Executed plan; code-cited (`a2mega_dp_test_top.sv`, `board_pins.h`). `/Users/` on lines 8-9. Add "executed" header. |
| `ESP32_ENHANCED_PORT.md` | 2026-09-02 | current | public | update | Register table predates the 2026-10-22 remap of 0x3C-0x3F/0x4F (MERGE_RESOLUTIONS 2a); re-sync with `a2fpga_regs.h`. Code-cited. |
| `ESP32_OSPI_DESIGN.md` | 2026-06-14 | current | public | update | Base OSPI protocol, now shared with a2p25; "similar to the PicoSoC module" wording and the register map need the same remap pass. Code-cited. |
| `PRE_REV_EXERCISE_PLAN.md` | 2026-09-05 | reference-keep | public | ask-ed | Gate plan for the 1.0a4 send. Phase C (socket) done S2-S6; Phases U (USB 2.0 through hub) and P (power DMM) unresolved in any doc. Question in section 5. |
| `SOM_138B_BALLMAP.md` | 2026-08-28 | current | public | keep | Code-cited from the 138B `.cst`/`.sdc`/shim. Links to `WS4_GOWIN_STACK.md` need the archive path. |
| `SOURCES.md` | 2026-08-30 | current | public | keep | Record of vendor PDFs removed 08-30; the model for the manifest comment. |
| `WS4_GOWIN_STACK.md` | 2026-08-28 | direction-not-taken | fork-only, needs-redaction | archive | Gowin EDP IP stack evaluation (upstream checklist already excludes `dp_gowin`); refs `a2mega_dp_gowin.gprj`, which no longer exists; `/Users/` line 133, `/Volumes/` line 276. To `archive/dp_gowin/`; update two code citations. |
| `a2mega_pinmap_1_0a3.csv` | 2026-08-28 | current | public | keep | Code-cited from both `.cst`s and `transceiver_bank_gowin.v`; header says re-extract for 1.0a4. |
| `a2p25_carryover_2026-10-11.md` | 2026-10-11 | current | public | keep | Feeds `board_1_0a4_requirements.md`. |
| `acquisition_matrix_protocol.md` | 2026-08-28 | reference-keep | public | keep | Pair with the results file (code-cited); keep together. |
| `acquisition_matrix_results.md` | 2026-08-28 | reference-keep | public | keep | Campaign log, root-caused (telemetry transport, teardown); cited from `aux_channel.v` and `a2mega_dp_test_top.sv`. |
| `adapter_census_results.md` | 2026-09-03 | current | public | keep | Both-gate census; input to the a2p25 DP design section 7a. |
| `adb_micro_rom01_notes.md` | 2026-09-13 | reference-keep | needs-redaction | keep | Open reset-re-assert investigation; disassembly notes only (Apple ROM not in tree). `/Users/` line 133. |
| `apple_off_backfeed_level_shifters.md` | 2026-09-15 | current | public | keep | 1.0a4 design note. |
| `board_1_0a4_requirements.md` | 2026-10-11 | current | needs-redaction | keep | The rev gate; `/Volumes/` on line 273. |
| `bringup_1_0a3_log.md` | 2026-10-12 | reference-keep | needs-redaction | keep | 94 KB narrative journal; code-cited from `top.sv`; `/Volumes/` line 290. Publication policy question in section 5. |
| `btb_pinout_1_0a3.md` | 2026-08-28 | current | public | keep | Code-cited (`a2mega_dp_test.cst`, `btb_pinmap.py`). |
| `bus_pwr_active_interlock.svg` | 2026-09-15 | current | public | keep | Figure for the backfeed note. |
| `cdc_constraint_audit.md` | 2026-08-28 | reference-keep | public | keep | Hypothesis doc; recommendations landed in timing round 2. Code-cited from `transceiver_bank_gowin.v`. Add pointer to `timing_round2.md`. |
| `ddr3_framebuffer_480p_design.md` | 2026-07-05 | superseded-by `docs/memory-system.md` + `hdl/video/framebuffer_480p.sv` | public | archive | Self-declared historical (retired monolithic module). To `archive/ddr3-framebuffer/`. |
| `ensoniq_ddr3_pipelined_design.md` | 2026-08-05 | current | public | keep | Design of the shipped `doc5503_pipelined.sv`; cited from both board tops, `sound_glu.sv`, the sim README. |
| `ensoniq_ddr3_postmortem.md` | 2026-08-05 | reference-keep | public | archive | Campaign post-mortem; uncited. To `archive/ensoniq-ddr3/`. |
| `four_lane_rbr_scope.md` | 2026-08-28 | direction-not-taken | public | archive | a2mega stayed 2-lane HBR; the 4-lane RBR idea shipped on a2p25 through the shared core. To `archive/dp-bringup-2026-08/`. |
| `gowin_serdes_re_findings.md` | 2026-08-31 | reference-keep | fork-only, needs-redaction | keep | CSR-map decode behind the replay ROM; cited from `a2mega_138b.sdc`. Discusses decrypted vendor IP; `/Users/` line 3. Stays in place, excluded upstream. |
| `gs_socket_65816_scoping.md` | 2026-09-13 | current | public | update | Status line "No HDL written yet" vs S2-S6 merged; section 8b is the S6 state. Broken ref to `hyperram_scenarios.md` (never committed). Heavily code-cited. |
| `gs_socket_s7_accelerator_scoping.md` | 2026-09-13 | current | public | keep | Open scoping with questions for ed in section 8. |
| `jtag_bridge_research.md` | 2026-08-04 | superseded-by `jtag_flash_reliability.md` (08-15 root cause) | public, `/Volumes/` line 99 | archive | To `archive/jtag/`. |
| `jtag_flash_reliability.md` | 2026-08-28 | current | public | update | Cited by the flash skill and `tools/flash.sh`; 600 lines of superseded verdicts precede the 08-15 TRUE ROOT CAUSE. Add a top summary; the a2mega3-loader diff (+78 lines) exists only in the rescue folder (section 5). |
| `m5_afe_adjust_design.md` | 2026-08-29 | current | public | keep | M5 survivor (sections 10-14 are the applied policy). Ref to `boards/a2mega/hdl/dp_gowin/dp_gowin_transmitter.sv` is dead. |
| `m5_perlane_proposal.md` | 2026-08-28 | superseded-by `m5_afe_adjust_design.md` section 12 | public | archive | Self-declared. To `archive/dp-bringup-2026-08/`. |
| `m5_review_0821.md` | 2026-08-29 | reference-keep | public | archive | Review disposition, all items closed. To `archive/dp-bringup-2026-08/`. |
| `m5_runtime_afe.md` | 2026-08-28 | current | public | keep | Verified register model; cited from `afe_adjust_seq.v`, `dp_transmitter.sv`, die packages. |
| `mac_anker_aux_answer_key.md` | 2026-08-30 | reference-keep | public | archive | AUX conformance answer key; cited only from the a2p25 DP design. To `archive/dp-bringup-2026-08/` with its three captures. |
| `mac_anker_bringup_annotated.txt` | 2026-08-28 | reference-keep | public | archive | Decoded AUX transcript (Mac to Anker). Moves with the answer key. |
| `mac_anker_hdmireplug_annotated.txt` | 2026-08-28 | reference-keep | public | archive | Decoded AUX transcript (HDMI replug). Moves with the answer key. |
| `offset_wrap_fix.patch` | 2026-08-28 | obsolete | public | delete | Applied: `video_stream_packer.v` lines 526 and 903 carry the OFFSET-WRAP FIX blocks. Pure duplicate of code. |
| `offset_wrap_investigation.md` | 2026-08-28 | reference-keep | public | update | Status says "fix not applied"; it is. Cited from `tb_offset_wrap.v`. |
| `our_board_bringup_annotated.txt` | 2026-08-28 | reference-keep | public | archive | 179 KB decoded AUX transcript of our board. Moves with the answer key. |
| `pnr_structural_analysis.md` | 2026-08-31 | reference-keep | public | keep | Campaign plan partly executed (wr_fifo to BSRAM shipped); cited from `framebuffer_1080p.sv`. Add a status header pointing to `timing_round2.md`. |
| `scan_timer_design.md` | 2026-06-14 | current | public | keep | Cited from `hdl/video/scan_timer.sv`; says "DDR3 framebuffer", module is now shared. |
| `tang_mega_138k_analysis.md` | 2026-08-04 | reference-keep | public | keep | Pre-silicon analysis (138B verified 08-20); cited from `clocks_138b.sv`, `framebuffer_1080p.sv`. Add header. |
| `tang_mega_die_names.json` | 2026-08-28 | current | public | keep | Input to `btb_pinmap.py`. |
| `test_log.md` | 2026-09-13 | current | needs-redaction | keep | Bench ground truth (164 KB, two tables). `/Users/` on lines 269, 272, 278; `/Volumes/` line 272. |
| `timing_round2.md` | 2026-09-05 | current | public | update | Method and log; cited from `gotchas.md`, `setup-gowin-cli.md`, `gs_socket.sdc`. Add the 2026-10-24 138B `aux_channel` regression (MERGE_RESOLUTIONS section 6). |
| `twgs_reference.md` | 2026-06-14 | reference-keep | public (already upstream) | keep | Synthesis of the 1989 AE manual; input to S7 scoping. |
| `week_plan_0817.md` | 2026-08-28 | obsolete | public | archive | One week's plan; uncited. To `archive/dp-bringup-2026-08/`. |

### 3.4 `boards/a2n20v2-Enhanced/docs/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `UTHERNET2.md` | 2026-06-20 | current | public | update | Status "hardware validation pending" vs shipped and HW-verified; add the W5100/LAN DHCP note. |
| `UTHERNET2_PROTOCOL.md` | 2026-06-20 | current | public | update | Broken link `BL616_SPI_PROTOCOL.md` (lives in `../src/a2n20_bl616/docs/`). |
| `disk_image_serving_design.md` | 2026-07-02 | current | public | update | Design dated 06-23, implemented since (plus .dsk GCR nibblize, HDD, .2mg); add an "implemented" header pointing at the firmware. |

### 3.5 `boards/a2n20v2-Enhanced/src/a2n20_bl616/docs/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `BL616_SPI_PROTOCOL.md` | 2026-07-02 | current | public | keep | Authoritative register map; cited from `fpga_spi.h`, `main.c`. Verify 0x7A and HDD regs are present. |
| `BL616_USB.md` | 2026-06-19 | current | public | update | "Working today: XInput gamepad" predates MSC, CDC-ECM, HID remotes, the QH-pool fix. |
| `bl616_ecosystem.md` | 2026-07-04 | reference-keep | public | keep | Sipeed two-stage boot, board variants. |
| `macos_build_setup.md` | 2026-07-05 | current | public | keep | Linked from the setup skill. |

### 3.6 `boards/a2n20v2-GS/docs/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `WORKPLAN.md` | 2026-06-14 | obsolete | public | archive | Implemented; 11 refs to `boards/a2n20v2-GS/hdl/video/*` and `boards/a2mega/hdl/video/*` are dead (moved to `hdl/video/`). To `archive/`; update GS README and `memory-system.md`. |

### 3.7 `boards/a2p25/docs/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `DP_ALTMODE_DPHY_DESIGN.md` | 2026-09-05 | current | public | update | "Status: Proposed" vs built and working 10-11; the design basis for `transceiver_bank_dphy.sv` (code-cited). |
| `ESP32_FEATURE_PORT.md` | 2026-10-12 | current | public | keep | P1-P4 verified, P5 next. |
| `ESP32_FW_A2P25.md` | 2026-10-12 | current | public (`/Volumes/` lines 222-223) | keep | Code-cited from the shared ESP32 firmware. |
| `ESP32_SPI_PROTOCOL.md` | 2026-06-14 | obsolete | public | archive | 3-wire protocol used only by `legacy_a2p25_rev0.gprj`; `esp32_ospi_proto_proc.sv` says "based on". To `archive/`; update `coprocessor-interface.md` and the README. |
| `LCAM_SESSION_NOTES.md` | 2026-06-14 | obsolete | public | archive | LCAM, I2S and the ESP32 ES5503 are gone on displayport1 (TODO says superseded). To `archive/`. |
| `MODERNIZATION_PLAN.md` | 2026-10-13 | current | public | keep | Living plan plus bench log; section 4 revises `memory_bandwidth_analysis.md` section 3. |
| `PINOUT_DISPLAYPORT1.md` | 2026-10-01 | current | needs-redaction | keep | Code-cited from every a2p25 `.cst`; `/Volumes/` line 21. |
| `PSRAM_TEST.md` | 2026-10-11 | current | public | keep | |
| `RESOURCE_AUDIT.md` | 2026-10-22 | current | public | keep | Cited from `a2p25_profile.vh` and `build_profiles.md`; refs to `doc_fetch_filter.sv` and `tb_fetch_late.sv` are proposals, not files. |
| `TOP_INTEGRATION_PLAN.md` | 2026-10-11 | current | public | update | Status "B4 first light" predates the IIgs booting on the new top (10-11); fold section 6 decisions into the plan, then archive. |

### 3.8 Board READMEs

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `boards/a2mega/README.md` | 2026-07-04 | current | public | update | Describes 1.0a2a (USB-A gamepad, micro-SD, Tang Mega 60K only); 1.0a3 removed HDMI, micro-SD and USB-A for USB-C DP; add the 138K SOM and a full doc index (6 of 47 listed). |
| `boards/a2n20v1/README.md` | 2026-06-14 | current | public | keep | Deprecated board. |
| `boards/a2n20v2-Enhanced/README.md` | 2026-07-05 | current | public | keep | Beta guide; telnet, FTP, modem, self-update sections present. |
| `boards/a2n20v2-GS/README.md` | 2026-06-14 | current | public | update | Only the WORKPLAN link changes. |
| `boards/a2n20v2/README.md` | 2026-06-14 | current | public | keep | |
| `boards/a2n9/README.md` | 2026-06-14 | current | public | keep | |
| `boards/a2p25/README.md` | 2026-08-23 | current | public | update | "Coming soon" vs IIgs booting over DP 1080p (10-11); links LCAM notes, the 3-wire protocol and the LCAM firmware README; no displayport1 docs. Rewrite. |

### 3.9 `hdl/`, `sim/`, `tools/`

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `hdl/displayport/README.md` | 2026-08-28 | current | needs-redaction | update | "Vendored at 87cd538, develop upstream" vs two months of in-repo development (timing round 2, M5, AUX fixes, MSA 4ch); "4-lane NOT wired" vs a2p25 1080p x4; a2mega-only specifics (D-PHY backend exists); `/Users/` line 4. Source-of-truth question in section 5. |
| `sim/doc5503/README.md` | 2026-08-01 | current | public | keep | Add a line that the a2p25 HyperRAM port runs the same model. |
| `tools/README.md` | 2026-10-24 | current | public | keep | Download links dated 6/12/26; resource report documented. |

`tests/` does not exist at the repo root (a2p25 tests live in `boards/a2p25/tests/`, no markdown).

### 3.10 Adjacent documentation (outside the strict scope, load-bearing for the above)

| File | Last commit | Stage | Upstream | Action | Note |
|---|---|---|---|---|---|
| `boards/a2mega/TODO.md` | 2026-07-12 | obsolete | public | update | "All code in place, untested on hardware" and "Ensoniq NOT movable to DDR3" contradict shipped state; rewrite from `board_1_0a4_requirements.md` and MERGE_RESOLUTIONS section 3. |
| `boards/a2p25/TODO.md` | 2026-10-01 | current header, obsolete body | public | update | Body is the ES5503-on-ESP32 plan (2026-01-28); keep the header, replace the body with the MODERNIZATION_PLAN open items. |
| `boards/a2n20v2-Enhanced/TODO.md` | 2026-06-14 | obsolete | public | update | PicoSoC items, "fix audio clk" (fixed), "complete Disk II" (shipped); rewrite. |
| `boards/a2n20v2-GS/TODO.md` | 2026-06-14 | current | public | update | "Last verified build 2026-01-16". |
| `boards/a2n20v2/TODO.md` | 2026-06-14 | current | public | update | Same text as GS; same date. |
| `boards/a2n9/TODO.md` | 2026-06-14 | current | public | keep | |
| `boards/a2n20v1/TODO.md` | 2026-06-14 | current | public | keep | |
| `boards/a2mega/hdl/dp_test_138b/README.md` | 2026-08-28 | reference-keep | needs-redaction | update | "Blocked, no bitstream" vs 138B silicon-verified 08-20; `/Users/` line 85. |
| `boards/a2mega/hdl/gowin/138B/dp_serdes/README.md` | 2026-08-28 | reference-keep | needs-redaction | update | GUI-generation checklist; emission now present; `/Users/` line 121; links `WS4_GOWIN_STACK.md`. |
| `boards/a2mega/hdl/twgs/65C816/README.md` | 2026-08-27 | current | public | keep | Provenance and license of the P65C816 core. |
| `boards/a2n20v2-Enhanced/src/README.md` | 2026-06-14 | current | public | keep | |
| `boards/a2n20v2-Enhanced/src/a2n20_bl616/README.md` | 2026-07-11 | current | needs-redaction | update | `/Users/` line 109; "replaces the PicoRV32" is history now. |
| `boards/a2p25/src/a2fpga_esp32/README.md` | 2026-06-14 | obsolete | public | ask-ed | LCAM-era firmware that `ESP32_FW_A2P25.md` says is not used on this board; the whole `boards/a2p25/src/` tree is the question (section 5). |
| `hardware/gameport-dongle/README.md` | 2026-07-05 | current | public | keep | |

Also present and fine as-is: `boards/{a2n9,a2n20v1,a2n20v2,a2mega,a2p25}/sch/*.pdf` (our schematics, public), `boards/*/photos/*.png`, `releases/README.pdf` (2024-08-28, release packaging note).

## 4. Doc families across time: survivor and history

| Topic | Survivor | History (archive) |
|---|---|---|
| DDR3 framebuffer | `docs/memory-system.md`, `hdl/video/framebuffer_480p.sv`, `framebuffer_1080p.sv` headers, `scan_timer_design.md` | `A2FPGA_DDR3_Framebuffer_Refactor_Workplan.md`, `ddr3_framebuffer_480p_design.md`, `tang_mega_138k_analysis.md` (keep, cited) |
| Ensoniq on external memory | `ensoniq_ddr3_pipelined_design.md`, `sim/doc5503/README.md` | `ensoniq_ddr3_postmortem.md` |
| JTAG and flashing | `jtag_flash_reliability.md` (08-15 section), `gotchas.md` recovery recipe, flash skill | `jtag_bridge_research.md` |
| 1.0a3 DP bring-up | `test_log.md` (ground truth), `board_1_0a4_requirements.md`, `a2p25_carryover_2026-10-11.md` | `bringup_1_0a3_log.md` (keep, cited), `week_plan_0817.md`, `BRINGUP_1_0A3_PLAN.md` (keep, cited) |
| AUX / link policy | `acquisition_matrix_results.md`, `adapter_census_results.md`, `a2p25_carryover` | `mac_anker_aux_answer_key.md` + three `.txt` captures, `four_lane_rbr_scope.md` |
| M5 runtime AFE | `m5_afe_adjust_design.md`, `m5_runtime_afe.md` | `m5_review_0821.md`, `m5_perlane_proposal.md` |
| Timing closure | `timing_round2.md`, `gotchas.md` policy paragraph | `pnr_structural_analysis.md`, `cdc_constraint_audit.md` (both keep, cited) |
| Gowin DP alternatives | `SOM_138B_BALLMAP.md`, `gowin_serdes_re_findings.md` (fork-only) | `WS4_GOWIN_STACK.md` (fork-only) |
| GS socket | `gs_socket_65816_scoping.md` section 8b, `gs_socket_s7_accelerator_scoping.md`, `adb_micro_rom01_notes.md`, `twgs_reference.md` | none yet |
| ESP32 firmware port | `ESP32_FEATURE_PORT.md`, `ESP32_FW_A2P25.md`, `ESP32_ENHANCED_PORT.md`, `ESP32_OSPI_DESIGN.md` | `ESP32_SPI_PROTOCOL.md`, `LCAM_SESSION_NOTES.md`, `boards/a2p25/src/a2fpga_esp32/README.md` |
| a2p25 memory | `MODERNIZATION_PLAN.md` section 4, `PSRAM_TEST.md`, `RESOURCE_AUDIT.md` | `memory_bandwidth_analysis.md` section 3 (keep, linked) |
| SDRAM framebuffer (GS) | `docs/memory-system.md`, `hdl/video/sdram_framebuffer.sv` | `boards/a2n20v2-GS/docs/WORKPLAN.md` |

## 5. Ask ed

1. `PRE_REV_EXERCISE_PLAN.md`: is the 1.0a4 send still gated on Phase U (USB 2.0 through the hub) and Phase P (power DMM, item 4b), or did the a2p25 displayport1 results retire this plan? If retired, archive it; if not, it is the only place those two gates are written down and should be updated, not archived.
2. `boards/a2p25/src/a2fpga_esp32/` (LCAM-era firmware, 2026-06-14, unused per `ESP32_FW_A2P25.md`): delete the tree, or keep it as `boards/a2p25/src/legacy_rev0/` beside `hdl/legacy/` and `legacy_a2p25_rev0.gprj`?
3. Bench journals upstream policy: do `test_log.md`, `bringup_1_0a3_log.md`, `acquisition_matrix_results.md` and the AUX capture transcripts go to the public repo (after `/Users/` redaction), or are logs fork-only as a class? This decides two lines of `UPSTREAM_EXCLUDE.txt`.
4. `hyperram_scenarios.md`: `gs_socket_65816_scoping.md` cites it and memory calls it the TransWarp endgame, but it was never committed (BRANCH_AUDIT found it untracked in the main checkout and copied it to the rescue folder). Commit it to `boards/a2mega/docs/`, or drop the citation?
5. `hdl/displayport/README.md`: is `edanuff/DisplayPort_Verilog` still the development home, with this directory a vendored copy to re-sync, or has the in-repo copy become the source of truth since August? The README has to say one or the other.
6. The a2mega3 openFPGALoader fix (`tools/openfpgaloader-a2mega3.patch`, binary, and a +78 line addition to `jtag_flash_reliability.md`) exists only in `a2fpga_core.rescue/2026-10-22/`. Land it, or record it as not taken?
7. `.claude/` (skills, settings): excluded from upstream, or part of the public contributor tooling? The `/setup`, `/build`, `/flash` skills are referenced from `AGENTS.md` and `getting-started.md`, which do go upstream.
8. `twgs_reference.md` and `adb_micro_rom01_notes.md` are derived from third-party material (a 1989 user manual; Apple's ADB micro ROM, disassembled). Both are the author's synthesis and the first is already upstream. Confirm both stay public.

## 6. Mechanical follow-up

### 6.1 `git mv` list implied by the archive recommendations

Root and `docs/`:

```
mkdir -p docs/archive/merges
git mv BRANCH_AUDIT_2026-10-22.md      docs/archive/merges/     # untracked today: mv + git add
git mv MERGE_RESOLUTIONS_2026-10-22.md docs/archive/merges/     # untracked today: mv + git add
# write docs/archive/README.md and docs/archive/merges/README.md
```

`boards/a2mega/docs/`:

```
mkdir -p boards/a2mega/docs/archive/{ddr3-framebuffer,ensoniq-ddr3,jtag,dp-bringup-2026-08,dp_gowin}
git mv boards/a2mega/docs/A2FPGA_DDR3_Framebuffer_Refactor_Workplan.md boards/a2mega/docs/archive/ddr3-framebuffer/
git mv boards/a2mega/docs/ddr3_framebuffer_480p_design.md             boards/a2mega/docs/archive/ddr3-framebuffer/
git mv boards/a2mega/docs/ensoniq_ddr3_postmortem.md                  boards/a2mega/docs/archive/ensoniq-ddr3/
git mv boards/a2mega/docs/jtag_bridge_research.md                     boards/a2mega/docs/archive/jtag/
git mv boards/a2mega/docs/week_plan_0817.md                           boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/four_lane_rbr_scope.md                      boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/m5_review_0821.md                           boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/m5_perlane_proposal.md                      boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/mac_anker_aux_answer_key.md                 boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/mac_anker_bringup_annotated.txt             boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/mac_anker_hdmireplug_annotated.txt          boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/our_board_bringup_annotated.txt             boards/a2mega/docs/archive/dp-bringup-2026-08/
git mv boards/a2mega/docs/WS4_GOWIN_STACK.md                          boards/a2mega/docs/archive/dp_gowin/
git rm boards/a2mega/docs/offset_wrap_fix.patch
# write boards/a2mega/docs/archive/README.md
```

`boards/a2p25/docs/` and `boards/a2n20v2-GS/docs/`:

```
mkdir -p boards/a2p25/docs/archive boards/a2n20v2-GS/docs/archive
git mv boards/a2p25/docs/LCAM_SESSION_NOTES.md  boards/a2p25/docs/archive/
git mv boards/a2p25/docs/ESP32_SPI_PROTOCOL.md  boards/a2p25/docs/archive/
git mv boards/a2n20v2-GS/docs/WORKPLAN.md       boards/a2n20v2-GS/docs/archive/
# write both archive/README.md files
```

Conditional on section 5 answers: `PRE_REV_EXERCISE_PLAN.md` to `archive/dp-bringup-2026-08/` (Q1); `boards/a2p25/src/a2fpga_esp32/` to `boards/a2p25/src/legacy_rev0/` or removed (Q2); `TOP_INTEGRATION_PLAN.md` to `boards/a2p25/docs/archive/` once its section 6 decisions are folded into `MODERNIZATION_PLAN.md`.

### 6.2 References that the moves break (edit alongside each `git mv`)

| Moved file | Files that name it |
|---|---|
| `A2FPGA_DDR3_Framebuffer_Refactor_Workplan.md` | `boards/a2mega/README.md`, `docs/ROADMAP.md` |
| `ddr3_framebuffer_480p_design.md` | `boards/a2mega/README.md`, `docs/memory-system.md` |
| `ensoniq_ddr3_postmortem.md` | none inbound; its own links to `ensoniq_ddr3_pipelined_design.md`, `jtag_*` need `../../` |
| `jtag_bridge_research.md` | `ensoniq_ddr3_postmortem.md` (moves too), `jtag_flash_reliability.md` ("companion") |
| `four_lane_rbr_scope.md` | `boards/a2mega/docs/a2p25_carryover_2026-10-11.md` |
| `m5_review_0821.md` | `boards/a2mega/docs/PRE_REV_EXERCISE_PLAN.md` |
| `m5_perlane_proposal.md` | `m5_review_0821.md` (moves too) |
| `mac_anker_aux_answer_key.md` and the three `.txt` | `boards/a2p25/docs/DP_ALTMODE_DPHY_DESIGN.md`; the key's own links to the `.txt` files stay relative |
| `WS4_GOWIN_STACK.md` | `boards/a2mega/docs/SOM_138B_BALLMAP.md`, `boards/a2mega/hdl/dp_test_138b/gowin_plls_138b.sv` (comment), `boards/a2mega/hdl/gowin/138B/dp_serdes/README.md` |
| `offset_wrap_fix.patch` | `boards/a2mega/docs/offset_wrap_investigation.md` (status paragraph) |
| `LCAM_SESSION_NOTES.md` | `boards/a2p25/README.md`, `boards/a2p25/TODO.md`, `boards/a2p25/src/a2fpga_esp32/README.md`, `docs/ROADMAP.md`, `docs/audio.md` |
| `ESP32_SPI_PROTOCOL.md` | `docs/coprocessor-interface.md`, `boards/a2p25/README.md`, `boards/a2mega/hdl/esp32/esp32_ospi_proto_proc.sv` (comment), `boards/a2p25/hdl/esp32/esp32_spi_connector.sv` and `esp32_spi_proto_proc.sv` (comments) |
| `WORKPLAN.md` (GS) | `boards/a2n20v2-GS/README.md`, `docs/memory-system.md` |
| `BRANCH_AUDIT_*`, `MERGE_RESOLUTIONS_*` | none in tree (agent memory only) |

### 6.3 Index and router files to touch regardless of the moves

- `docs/README.md`: groups from 2.1, archive section, per-board doc index pointer, `DOCS_AUDIT_2026-10.md` link.
- `AGENTS.md`: routing rows for build profiles, archive convention, `UPSTREAM_EXCLUDE.txt`; the "Starting a session" step 2 should warn that `TODO.md` currency varies by board.
- `CLAUDE.md`: no change.
- `docs/boards.md`: matrix rows (a2mega 60K/138B, DP; a2p25 HyperRAM; Enhanced variants) and the per-board docs table rebuilt from the board READMEs.
- `docs/conventions.md`: add the archive rule ("a doc that no longer describes the code moves to the folder's `archive/` with a one-line entry in that archive's README; code citations are updated in the same commit") and the `*_profile.vh` rule.
- Board READMEs: `boards/a2mega/README.md` (rewrite for 1.0a3 and a grouped doc index), `boards/a2p25/README.md` (rewrite), `boards/a2n20v2-GS/README.md` (link).
- `.claude/skills/flash/SKILL.md`: cites `boards/a2mega/docs/jtag_flash_reliability.md` (stays in place, no change) and `docs/gotchas.md`. `.claude/skills/setup/SKILL.md`: cites `boards/a2p25/src/a2fpga_esp32/README.md` (changes with Q2) and `boards/a2n20v2-Enhanced/src/a2n20_bl616/docs/macos_build_setup.md` (no change). `.claude/skills/build/SKILL.md`: `docs/build_profiles.md` (no change). `.claude/skills/flash-mcu/SKILL.md`: BL616 README (no change).

### 6.4 Broken links and dead references found (fix in the update pass)

| File | Reference | Problem |
|---|---|---|
| `README.md` | `[src/](src/)` | directory does not exist; paragraph describes the removed PicoSoC firmware |
| `docs/peripheral-cards.md` | `../hdl/cardrom/cardrom.sv` | no such file; CardROM source is `boards/a2n20v2-Enhanced/src/cardrom/cardrom.s` |
| `docs/memory-system.md` | `../boards/a2n20v2-GS/hdl/video/sdram_framebuffer.sv` | moved to `hdl/video/sdram_framebuffer.sv` |
| `boards/a2n20v2-Enhanced/docs/UTHERNET2_PROTOCOL.md` | `BL616_SPI_PROTOCOL.md` | lives in `../src/a2n20_bl616/docs/` |
| `boards/a2n20v2-GS/docs/WORKPLAN.md` | `boards/a2n20v2-GS/hdl/video/{scan_timer,apple_video_fb,vgc_fb,sdram_framebuffer}.sv`, `boards/a2mega/hdl/video/*` | all moved to `hdl/video/` |
| `boards/a2mega/docs/WS4_GOWIN_STACK.md` | `boards/a2mega/a2mega_dp_gowin.gprj` | removed |
| `boards/a2mega/docs/m5_afe_adjust_design.md` | `boards/a2mega/hdl/dp_gowin/dp_gowin_transmitter.sv` | removed |
| `boards/a2mega/docs/gs_socket_65816_scoping.md` | `hyperram_scenarios.md` | never committed (section 5, Q4) |
| `boards/a2mega/docs/gs_socket_65816_scoping.md` | `hdl/twgs/iigs_65816_wrapper.sv` | present but the doc itself marks it superseded; fine |
| `boards/a2p25/docs/RESOURCE_AUDIT.md` | `hdl/sound/doc_fetch_filter.sv`, `sim/doc5503/tb_fetch_late.sv` | forward references to proposals; label them as such |
| `docs/conventions.md`, `docs/setup-gowin-cli.md`, `tools/README.md` | `hdl/datetime.svh` | generic path; the file is per board (`boards/<board>/hdl/datetime.svh`) |
| `BRANCH_AUDIT_2026-10-22.md` | `docs/2011-krue-fpi.pdf`, `tools/openfpgaloader-a2mega3.patch`, `hdl/bus/apple_timer.sv`, `boards/a2mega/docs/hyperram_scenarios.md` | rescue-folder items, not in tree; correct as written once the file is in `archive/merges/` |

### 6.5 `git grep '/Users/'` over the documentation folders (tracked files)

Each line needs a generic path or a "local checkout of repo X" phrase before the file goes upstream.

| File:line | Text |
|---|---|
| `boards/a2mega/docs/BRINGUP_1_0A3_PLAN.md:8` | `/Users/edanuff/GitHub/a2-mega` branch `1.0a3` |
| `boards/a2mega/docs/BRINGUP_1_0A3_PLAN.md:9` | `/Users/edanuff/GitHub/DisplayPort_Verilog` |
| `boards/a2mega/docs/WS4_GOWIN_STACK.md:133` | `gw_sh` via `/Users/edanuff/gowin-1.9.12-ide/bin/gw_sh` (fork-only file) |
| `boards/a2mega/docs/adb_micro_rom01_notes.md:133` | section heading names `/Users/edanuff/GitHub/Mega-IIe` |
| `boards/a2mega/docs/gowin_serdes_re_findings.md:3` | source repo `/Users/edanuff/GitHub/gowin-serdes` (fork-only file) |
| `boards/a2mega/docs/test_log.md:269` | `/Users/edanuff/GitHub/Mega-IIe` |
| `boards/a2mega/docs/test_log.md:272` | `/Users/edanuff/GitHub/TransWarpGS-ROM` (also `/Volumes/Storage/Downloads`) |
| `boards/a2mega/docs/test_log.md:278` | `/Users/edanuff/GitHub/TransWarpGS-ROM` |
| `boards/a2mega/hdl/dp_test_138b/README.md:85` | `GW_SH=/Users/edanuff/gowin-1.9.12-ide/bin/gw_sh` |
| `boards/a2mega/hdl/gowin/138B/dp_serdes/README.md:121` | `GW_SH=/Users/edanuff/gowin-1.9.12-ide/bin/gw_sh` |
| `boards/a2n20v2-Enhanced/src/a2n20_bl616/README.md:109` | a2fpga_core location `/Users/edanuff/GitHub/a2fpga_core/boards/a2n20v2/` |
| `docs/memory_bandwidth_analysis.md:699` | repo `/Users/edanuff/GitHub/a2-p25`, commit 800cd75 |
| `docs/memory_bandwidth_analysis.md:963` | verified against `/Users/edanuff/GitHub/a2-p25` at 6971f51 |
| `hdl/displayport/README.md:4` | local dev sandbox `/Users/edanuff/GitHub/DisplayPort_Verilog` |

Untracked: `BRANCH_AUDIT_2026-10-22.md:110` (main checkout path) and `:214` (rescue folder path); `MERGE_RESOLUTIONS_2026-10-22.md` has none.

Secondary class, `/Volumes/` (a NAS and a Downloads folder; harmless but machine-specific): `WS4_GOWIN_STACK.md:276`, `board_1_0a4_requirements.md:273`, `bringup_1_0a3_log.md:290`, `jtag_bridge_research.md:99`, `test_log.md:272`, `boards/a2p25/docs/ESP32_FW_A2P25.md:222-223`, `boards/a2p25/docs/PINOUT_DISPLAYPORT1.md:21`. No e-mail addresses or other personal strings were found in any documentation folder.

### 6.6 Doc-versus-code contradictions to resolve in the update pass

1. `docs/gotchas.md` and `boards/a2mega/TODO.md`: Ensoniq wavetable "not movable to DDR3" versus `boards/a2mega/hdl/top.sv:652` (DDR3-backed pipelined DOC) and `boards/a2p25/hdl/top.sv:632` (same on HyperRAM).
2. `docs/audio.md`: a2mega DOC in BSRAM; a2p25 ES5503 on the ESP32 over I2S. Neither is true.
3. `docs/peripheral-cards.md`: Disk II not built; no HDD card. Both ship (`hdl/disk/drive_ii.sv`, `hdl/disk/hdd.sv`, slot 5 and 6 on Enhanced).
4. `docs/memory-system.md`: Enhanced has no framebuffer (`a2n20v2_enhanced_fb.gprj`); a2mega sound BSRAM-backed; no HyperRAM backend.
5. `docs/coprocessor-interface.md`: a2p25 uses the 3-wire `esp32_spi_connector`; `a2p25.gprj` lists `../a2mega/hdl/esp32/esp32_ospi_connector.sv`.
6. `docs/boards.md`: a2p25 "no off-chip memory, no FB possible"; a2mega 60K only and HDMI.
7. `hdl/displayport/README.md`: "4-lane NOT wired" versus `LANE_COUNT=4` 1080p on a2p25 since 2026-10-11 and the MSA 4ch fix.
8. `boards/a2mega/docs/offset_wrap_investigation.md`: "fix not applied" versus `video_stream_packer.v:526,903`.
9. `boards/a2mega/docs/gs_socket_65816_scoping.md`: "No HDL written yet" versus S2-S6 merged (PRs 31-34).
10. `boards/a2n20v2-Enhanced/docs/UTHERNET2.md`: "hardware validation pending" versus shipped and bench-verified.
11. `boards/a2mega/README.md`: USB-A gamepad port and micro-SD versus 1.0a3 (`BRINGUP_1_0A3_PLAN.md` section 1 and `ESP32_FEATURE_PORT.md` section 1: no SD, no USB host on either ESP32 board).
12. `boards/a2p25/README.md`: "Coming soon" versus the IIgs booting on the new core over DP 1080p (`MODERNIZATION_PLAN.md` section 6f).
13. `boards/a2p25/docs/DP_ALTMODE_DPHY_DESIGN.md`: "Status: Proposed" versus built and working.
14. `boards/a2n20v2-Enhanced/TODO.md`: audio clock bug and PicoSoC interface as open high-priority items.
15. `boards/a2mega/hdl/dp_test_138b/README.md`: "blocked, no bitstream to flash" versus the 138B DP builds in `test_log.md` from 08-20 on.
16. `docs/ROADMAP.md` theme 3: a2p25 ES5503 on the ESP32 "largely working".
