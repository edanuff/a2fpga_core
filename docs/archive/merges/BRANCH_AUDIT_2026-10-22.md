# Branch and worktree audit — 2026-10-22

State at audit time: `main` = ce4a0595 (2026-09-05, "Merge PR #30"). 104 local
branches, 24 worktrees, 2 open PRs on the fork, 0 open upstream.
`upstream/main` is fully contained in `main`.

Method: ahead/behind vs `main`, `git cherry` (patch-id) vs `main` and vs
`origin/epic-merge-prep`, containment (`git branch --contains`), per-file
"is the branch's version of every file it touched already in main",
`merge-tree` trial merges, PR base refs on GitHub, dirty-tree scan of every
worktree.

---

## 1. The integration structure (why main looks stale)

```
main (Sep 5) ──────────────────────────────────────────────────────────┐
  │                                                                     │
  ├─ origin/epic-merge-prep (Sep 8, +137)   ← PRs #3–#29, #31–#33      │ CONFLICT 3 files
  │     └─ claude/gs-socket-s6-autoarm (+78)  ← PR #34 OPEN             │ (PD fw only)
  │           ├─ claude/gs-socket-s7-scoping (+1 docs)                  │
  │           └─ claude/hdd-boot-wait (+1)                               │
  │                                                                     │
  ├─ claude/a2p25-dp-over-dphy (Sep 5)                                   │ CLEAN vs main
  │   └─ claude/a2p25-console-hide-no-media (Oct 11)                     │ CONFLICT 7 files
  │        ├─ claude/stoic-sinoussi-294a3b (Oct 22, +115)  ← THE TIP    │   vs epic-merge-prep
  │        ├─ claude/a2p25-modernization-8a97cc (+2 docs beyond tip)     │
  │        └─ claude/nostalgic-jackson-c9a80d (+1 WIP beyond tip)        │
  │                                                                     │
  ├─ claude/quirky-hellman-d19038 ⊂ claude/a2mega-aux-stack-port (+9)   │ CLEAN vs a2p25 tip
  ├─ claude/74alvc164245-power-plane-9f9408 (+1 docs)                    │ CLEAN
  ├─ claude/a2p25-memory-bandwidth-a4dd1f (+3 docs)                      │ CLEAN
  └─ origin/claude/gamepad-apple2-joystick-ol6fyu (+4, remote only)      │ CLEAN
```

`origin/epic-merge-prep` is the integration branch every PR since #3 was merged
into. Only PR #30 (PD fixes) was carried to `main`. `main` has 12 commits not
in epic-merge-prep; 11 of them are patch-identical cherry-picks, which is where
the 3 PD-firmware conflicts come from.

The a2p25 October stack was built on `main`, NOT on epic-merge-prep, so it never
saw the timing-round-2 or gs-socket changes. 97 of epic-merge-prep's 137
commits are absent from the a2p25 tip; all 115 a2p25 commits are absent from
epic-merge-prep.

### ⚠ Unpushed branches (weeks of work exist only on this Mac)

- `claude/stoic-sinoussi-294a3b` — the a2p25 tip, includes today's verified
  DOC fix (92f164d6 build of record)
- `claude/a2p25-console-hide-no-media`, `claude/a2p25-dp-over-dphy`
- `claude/a2p25-modernization-8a97cc`, `claude/nostalgic-jackson-c9a80d`
- `claude/a2mega-aux-stack-port`, `claude/quirky-hellman-d19038`

---

## 2. Genuinely unmerged work (needs a landing decision)

| # | Branch | Size | What it is | Trial merge |
|---|--------|------|------------|-------------|
| U1 | `origin/epic-merge-prep` | +137 | timing round 2 (PRs 5–29), census (3–4), gs-socket S1–S5 (31–33), timing policy docs | vs main: 3 conflicts — `telnetd.c`, `usbc_glue.cpp/.h` (PD cherry-pick drift) |
| U2 | `claude/gs-socket-s6-autoarm` | +78 on U1 | PR #34 (open, MERGEABLE, 0 reviews): ESP32 auto-arm of the in-socket 65C816 | clean onto U1 |
| U3 | `claude/hdd-boot-wait` | +1 on U2 | HDD boot ROM polls STATUS instead of holding /RES — prerequisite for dropping the storage-ready reset hold | clean onto U2 |
| U4 | `claude/gs-socket-s7-scoping` | +1 on U2 | S7 accelerator scoping doc | clean onto U2 |
| U5 | `claude/stoic-sinoussi-294a3b` | +115 | a2p25: full core over DP, DOC on HyperRAM + word cache (verified 10-22), ESP32 fw P1–P4 (USB host, stick volume, OTA), build-stamp reg, RESOURCE_AUDIT | vs main: **clean**. vs U1: 7 conflicts — `framebuffer_1080p.sv`, `sdp_engine.v`, a2mega `top.sv`, `esp32_ospi_connector.sv`, `telnetd.c`, `usbc_glue.cpp/.h` |
| U6 | `claude/a2p25-modernization-8a97cc` | +2 on U5 | adapter census 10-13 docs (UGREEN pass) | clean onto U5 |
| U7 | `claude/nostalgic-jackson-c9a80d` | +1 on U5 | **WIP** PD DR_Swap for powered adapters | clean onto U5 — but it is a WIP commit |
| U8 | `claude/a2mega-aux-stack-port` | +9 | a2mega port of the a2p25 shared-HDL fixes + AUX/training stack + top wiring. Built (fs adc6da31), **never flashed** | clean onto U5; 6 conflicts vs U1 (same files as U5) |
| U9 | `claude/74alvc164245-power-plane-9f9408` | +1 | docs: Apple-off backfeed design note + SVG, 1.0a4 requirements item | clean |
| U10 | `claude/a2p25-memory-bandwidth-a4dd1f` | +3 | docs: HyperRAM bandwidth analysis §3 (+467 lines in `docs/memory_bandwidth_analysis.md`), RESET# retraction | clean vs main and vs U5 |
| U11 | `origin/claude/gamepad-apple2-joystick-ol6fyu` | +4 | RP2040 game-port dongle: KiCad project + design note (pure addition, Jul 5) | clean |
| U12 | `claude/apple-clock-sync-ed34ff` | 4 real commits | `hdl/bus/apple_timer.sv` predictive DPLL + testbench (opt-in `ENABLE_CLOCK_SYNC`, sim-verified, hw-verify pending). Branch is Aug 6 vintage | 148 conflicts (mostly artifacts/gprj/sdc) — needs a rebase or a fresh cherry-pick of 6d7d9ab7 |
| U13 | `pr/fb-sdram-correctness` | 1 | self-timed CDC request path in `mem_port_cdc.sv` + dualrate SDC. Upstream PR #48 was **CLOSED** unmerged; memory says CDC request tearing is still unfixed | parked — revive or drop is a decision |
| U14 | `origin/claude/twgs-a2-mega-plan-iferin` | +12 | TWGS action plan doc + 3 reference PDFs (SOM/TWGS schematics) + Phase 0/1 socket gateware (Jul). Superseded by gs-socket S1–S6 in U1/U2; PDFs fall under the pdf guardrail | archive, unless the plan doc is wanted |

### Recommended landing order

1. **Trivial docs/additions now:** U9, U10, U11 straight onto `main` (all
   clean, no HDL).
2. **main ← epic-merge-prep (U1).** Resolve the 3 PD-firmware conflicts
   (main's PR #30 copies vs epic-merge-prep's originals). Then U2/U3/U4 merge
   clean on top; PR #34 can be retargeted to `main` or merged as-is first.
3. **main ← a2p25 tip (U5 + U6).** This is the real merge: 7 files where
   timing-round-2 and the a2p25 fixes both edited `framebuffer_1080p.sv`,
   `sdp_engine.v` and the a2mega top/connector/PD firmware. Needs a2mega 60K +
   138B and a2p25 rebuilds with timing checked (no skating rule), and a2mega
   has not been attached since Oct 11.
4. **U8 (a2mega AUX-stack port)** rides with step 3 — it is the a2mega-side
   wiring for the shared-HDL interface changes U5 introduces. Unverified on
   hardware.
5. **U7** stays a WIP branch (or is squashed into step 3 if the DR_Swap work is
   wanted). **U12/U13/U14**: your call — see table.

Alternative: do steps 2+3 the other way round (a2p25 tip first, since it is
clean on main and is the live, hardware-verified work; then epic-merge-prep
with the same 7 conflicts).

---

## 3. Uncommitted work found in worktrees (exists nowhere else)

| Worktree | File(s) | What | Age |
|----------|---------|------|-----|
| `a2mega-ribbon-orientations-9eaded` (branch a2p25-console-hide-no-media) | `boards/a2mega/src/a2fpga_esp32/disk.c` +30, `boards/a2p25/docs/MODERNIZATION_PLAN.md` +15 | a2p25 (BOARD_REV 225) console-hide policy when no media: schedules the 1.5 s hide at reset-release. **Not in the a2p25 tip or main.** The memory note "OSD stays up w/o disks" is this fix, uncommitted | Oct 11 |
| `strange-poincare-0e20b4` (branch merged) | `firmware_host/disk.c` +91 | BL616: hot-plug watch for busy hubs (CherryUSB hub IRQ pipe goes silent once Ethernet streams; hw-verified 07-11) + rebuilt host .bin | Jul 12 |
| `beautiful-wilbur-572227` (branch happy-bartik, merged) | `bl616_spi_proto_proc.sv` +36/−11 | SPI MISO: sample `tx_byte` once per byte into an sclk-domain copy (`tx_sr`) — mid-byte update race fix | Jul 15 |
| `magical-allen-fde53a` (branch optimistic-ellis, merged) | `tools/bin/openFPGALoader-a2mega3` (3.4 MB binary), `tools/openfpgaloader-a2mega3.patch`, `jtag_flash_reliability.md` +78 | **The a2mega3 loader fix** (skip eraseSRAM / keep configured device) that root-caused the attempt-1 flash failure. Binary + patch never committed anywhere. Bench validation was pending | Aug 31 |
| `epic-lovelace-54ee14` (stale local epic-merge-prep) | `impl/pnr/flash_rescue_138b_built.fs` (35 MB), 10 archive `.fs` bitstreams, 8 AUX capture CSVs, 3 dump txt | **The 138B flash_rescue bitstream exists only on disk here** (memory: "disk-only; recipe in the answer key"). Archive .fs set = the Aug 30–31 A/B builds | Aug 25–31 |
| `apple-clock-sync-ed34ff` | `docs/2011-krue-fpi.pdf` | downloaded reference PDF | — |
| **main checkout** `/Users/edanuff/GitHub/a2fpga_core` | `boards/a2mega/docs/hyperram_scenarios.md` (27 KB, untracked), `a2mega_pinmap_1_0a3.csv` +39/−25, two `*_process_config.json` | The HyperRAM three-store placement doc (memory: "scenarios doc in boards/a2mega/docs/") is **untracked in main's working tree** | Aug 30 / Sep 3 |

Everything else dirty in worktrees is regenerated build output (`impl/pnr/*`,
`datetime.svh`, `device.cfg`, Gowin IP `_tmp.v`/`.vo`, pycache).

---

## 4. Already in main — safe to delete (archive-tag first)

Verified either by `git branch --contains`, or by the upstream PR that merged
the re-authored copy (`upstream/main` ⊂ `main`).

**Zero commits ahead of main (28):**
a2mega-usb-host, claude/a2mega-ribbon-orientations-9eaded,
claude/a2n20v2-firmware-verify-e31559, claude/apple-ii-inh-assertion-53a6ff,
claude/beautiful-wilbur-572227 (upstream #42), claude/cool-jang-9eb93d,
claude/dazzling-ramanujan-e51ed1, claude/dreamy-visvesvaraya-8b43a7,
claude/dual-rate-sdram (#43), claude/goofy-goldstine-24c0e5 (#40),
claude/happy-bartik-055ad3, claude/issue-54-review-602639,
claude/optimistic-ellis-519091, claude/strange-poincare-0e20b4,
claude/wonderful-hugle-ded783, claude/unmerged-worktrees-branches-0abece (this
session), dev, genlock2, license-issue-54 (#55), pr/enhanced-debug-toolkit
(#49), pr/enhanced-startup-sequencing (#50), pr/fb-storage-port-hardening
(#51), upstream-main (branch; keep the worktree if you use it),
claude/a2n20v2-pixel-stream-direct-display + backup/main-pre-attrib (pre-attribution-rewrite copies, patch-identical).
Remote-only: origin/claude/a2n20v2-enhanced-cdc-ecm-rx-investigation,
origin/claude/boot-polish (#46), josh/macos-toolchain-flash-fixes.

**Re-landed under another name (14):**
- claude/fw-picker-limits, claude/video-framebuffer, claude/nostalgic-lehmann-2ebdc1 → upstream #44, #45
- fix/fb-storage-port-hardening, experiment/native-short-reset-lc, fix/excise-cardrom-lc-contention, test/current-classic, test/jul3-baseline → re-authored as pr/* (#49–#51)
- a2n20v2-enhanced-disk-ii-volume → disk serving shipped (its private `apple_bus.sv` was replaced by the shared one in #40)
- claude/great-curran-5feb9d → squash 5f25839 (memory says keep the granular history: tag it)
- worktree-agent-aeda42e819774c3ad (doc5503 rev 3.10b → in main 5f01e42c), worktree-agent-a909c3ffa6cae8e62 (jtag doc, main has the Aug 28 version)
- claude/timing-round2, claude/timing-round2-linear → 0 patches missing from epic-merge-prep (delete after U1 lands)
- claude/gs-socket-65816-scoping, -s2-phy, -s4-integrate → in epic-merge-prep (PRs 31–33)

**a2mega 1.0a3 August epic (9) → squashed into main as 0a62d8ca (Aug 28):**
a2mega-1.0a3, claude/a2mega-1-0a3-prep-79601a (PR #2 OPEN+CONFLICTING →
**close as superseded**), claude/charming-meninsky-00995d,
claude/inspiring-hellman-22a2e1, claude/epic-lovelace-54ee14 (+origin),
worktree-agent-a05472d7877dddfd2, -a821af1176f38e602, -ad6d030eaf516667d,
-adfdc7882adccb2d9, -af35b40e04f94d890.
These are the ONLY copies of `boards/a2mega/hdl/dp_gowin/*` (WS4 Gowin DP-stack
evaluation), `gowin_outreach.md` and the vendor PDFs that the squash
deliberately excluded (pdf/outreach/dp_gowin guardrail). Tag
`claude/epic-lovelace-54ee14` (the superset) before deleting the rest.

**Diagnostic, "NOT FOR MERGE" by design (2):** diag/rom01-profiler, diag/rom01-bus-timing-profiler → tag + delete.

**Stale local copy:** `epic-merge-prep` (local, Sep 2) is 135 commits behind `origin/epic-merge-prep` and has `origin/main` as upstream. Reset to origin or delete.

---

## 5. Ancient 2025 experiments — archive, don't merge (30)

All ≥237 commits behind main, from before the current lineage; the shipped
designs superseded them. Their remotes are mostly already gone.

enhanced_v2, enhanced_v2_5503, enhanced_v2_5503_branch3, _branch_1,
_branch_1_working_1, _branch_2, _new, _new2, _working, enhanced_v2_wip,
enhanced_w2_ensoniq1, genlock, genlock3, signed-audio, wip, a2p25, a2p25-1,
a2p25_5503, boot, qspi, lcd_cam, claude/show-recent-branch-9cmp2, framebuffer,
ddr3_framebuffer, mystifying-bohr, claude/add-framebuffer-support-QQa6S,
claude/evaluate-ddr3-gowin-compatibility-GTnwX, pixel-stream-interface,
predictive-timing, new-timing-model (1 commit: 4 .gitignore lines),
backup/del-list-latest-branches, backup/del-working-build-ddr3,
a2n20v2-enhanced (2024 "initial commit").

Also: 5 stashes from 2024-12 … 2026-04 (GitHub Desktop autosaves). Not touched.

---

## 6. Worktrees

| Worktree | Branch | Verdict |
|----------|--------|---------|
| gs-socket | gs-socket-s6-autoarm | **keep** (PR #34) |
| stoic-sinoussi-294a3b | stoic-sinoussi | **keep** (a2p25 tip, unpushed) |
| a2mega-ribbon-orientations-9eaded | a2p25-console-hide-no-media | keep until the console-hide edit is committed |
| nostalgic-jackson-c9a80d | nostalgic-jackson | keep while the DR_Swap WIP is alive |
| eager-volhard-514c98 | a2p25-modernization | remove after the 2 docs commits are merged (101 dirty = dp_test build outputs) |
| dazzling-ramanujan-e51ed1 | a2mega-aux-stack-port | remove (clean; branch stays) |
| quirky-hellman-d19038 | quirky-hellman | remove (⊂ aux-stack-port; artifacts only) |
| brave-bassi-c4fc9a, a2p25-memory-bandwidth-a4dd1f | U9, U10 | remove after merge |
| apple-clock-sync-ed34ff | U12 | remove (move the PDF out first) |
| magical-allen-fde53a | optimistic-ellis | **rescue loader binary+patch+doc delta first**, then remove |
| epic-lovelace-54ee14 | stale epic-merge-prep | **rescue flash_rescue_138b_built.fs + archive .fs + CSVs first**, then remove |
| strange-poincare-0e20b4, beautiful-wilbur-572227 | merged | **rescue the two source edits** (commit to a branch), then remove |
| a2mega-1-0a3-prep-79601a, charming-meninsky-00995d, elated-mccarthy-0b684e, great-curran-5feb9d, inspiring-hellman-22a2e1, apple-ii-inh-assertion-53a6ff (detached) | superseded | remove (dirty = regenerable artifacts / IP regen) |
| unmerged-worktrees-branches-0abece | this session | remove when done |
| ../a2fpga_core.worktrees/upstream-main | upstream-main | yours; dirty with rebuilt a2n20v2 artifacts — left alone |

---

## 7. Safe pass executed (2026-10-22, same day)

- **Merged into main** (merge commits, not pushed): 74alvc backfeed note (34c9dfb7),
  HyperRAM bandwidth §3 docs (18af0bfa), game-port dongle KiCad + note (77bbb122).
  `main` is now 11 commits ahead of `origin/main`.
- **PR #2 closed** as superseded.
- **Rescue branches** created from the orphan uncommitted edits:
  `rescue/a2p25-console-hide-no-media-policy`, `rescue/bl616-hub-hotplug-watch`,
  `rescue/bl616-spi-tx-sr-sampling`, `rescue/a2mega3-loader-patch`.
- **Rescue folder** `/Users/edanuff/GitHub/a2fpga_core.rescue/2026-10-22/`:
  flash_rescue_138b_built.fs, 10 archive .fs, the 15 GB of AD2/AD3 CSV captures
  (moved, not copied — the copy attempt filled the disk), telnet dumps, the
  openFPGALoader-a2mega3 binary + patch, jtag doc diff, krue FPI pdf, a copy of
  hyperram_scenarios.md. README inside.
- **87 local branches deleted**, 65 of them first tagged `archive/2026-10-22/<name>`
  (tags are local only, not pushed). 23 merged branches deleted on origin.
- **16 worktrees removed.** Remaining: main, upstream-main, gs-socket,
  stoic-sinoussi-294a3b, nostalgic-jackson-c9a80d, eager-volhard-514c98
  (a2p25-modernization, 101 dirty build outputs), this session.
- **Not done / still open:** push main; push the unpushed a2p25 + aux-stack
  branches; the big merges (epic-merge-prep, a2p25 stack, PR #34, aux-stack-port);
  U7 WIP, U12 apple_timer, U13 mem_port_cdc, U14 twgs plan decisions; the two
  remote archive copies (origin/claude/epic-lovelace-54ee14,
  origin/claude/a2mega-1-0a3-prep-79601a) and origin/claude/74alvc +
  origin/claude/gamepad-apple2-joystick-ol6fyu can go once main is pushed;
  main checkout still has hyperram_scenarios.md untracked + 3 modified files.

## 8. Pushed (2026-10-22, later)

- `main` pushed: origin/main ce4a0595 → 77bbb122.
- Pushed with tracking: claude/stoic-sinoussi-294a3b, claude/a2p25-console-hide-no-media,
  claude/a2p25-dp-over-dphy, claude/a2p25-modernization-8a97cc,
  claude/nostalgic-jackson-c9a80d, claude/a2mega-aux-stack-port, and the four
  rescue/* branches.
- 64 of 65 `archive/2026-10-22/*` tags pushed. `archive/2026-10-22/diag/rom01-bus-timing-profiler`
  is LOCAL ONLY: its history carries `boards/a2mega/impl/pnr/a2mega_138B.pr`
  (138 MB), over GitHub's 100 MB limit. Its source (`bus_timing_profiler.sv`) is
  also in `archive/2026-10-22/diag/rom01-profiler`, which did push.
- Pre-existing local-only tags noticed, untouched: 08-17-2024, 10-12-2024,
  12-14-2024, a2mega-2lane-planc-4of5, rescue/diag-rom01-profiler,
  rescue/dropped-series-tip (0c5e1e5a, Aug 5 cleanup build).
- Now safe to delete on origin once you agree: claude/74alvc164245-power-plane-9f9408,
  claude/gamepad-apple2-joystick-ol6fyu (both in main), claude/epic-lovelace-54ee14,
  claude/a2mega-1-0a3-prep-79601a (both archive-tagged on origin).

## 9. Big merge landed (2026-10-22, later)

- `main` 77bbb122 → 9332dab5 (350 commits): epic-merge-prep, a2p25 tip +
  modernization docs, PR #34 (gs-socket S6), hdd-boot-wait, s7-scoping,
  a2mega-aux-stack-port. Pushed. Resolutions: `MERGE_RESOLUTIONS_2026-10-22.md`.
- PR #34 retargeted to main, auto-marked merged by GitHub. 0 open PRs.
- Deleted (local + origin): epic-merge-prep, gs-socket-s6-autoarm, s7-scoping,
  hdd-boot-wait, stoic-sinoussi, a2p25-console-hide-no-media, a2p25-dp-over-dphy,
  a2p25-modernization, a2mega-aux-stack-port, timing-round2(-linear, archive-
  tagged: patch-equivalent, not ancestors), 74alvc, gamepad dongle, and the two
  remote archive copies (epic-lovelace, a2mega-1-0a3-prep; tags are on origin).
- Worktrees gs-socket, stoic-sinoussi, eager-volhard removed (eager-volhard's 92
  untracked dp_test build outputs → rescue folder). 4 worktrees remain: main,
  upstream-main, nostalgic-jackson (WIP), this session.
- main checkout: the local a2p25_process_config.json edit (Sep 1 IDE reorder)
  was discarded for the ff — saved with the other two local mods in
  `a2fpga_core.rescue/2026-10-22/main-checkout/main_local_mods_2026-10-22.patch`.
- Remaining branches: nostalgic-jackson (U7 WIP), apple-clock-sync (U12),
  pr/fb-sdram-correctness (U13), origin twgs plan (U14), four rescue/*.
- Next: builds (a2mega 60K/138B, a2p25) + the hardware checks in
  MERGE_RESOLUTIONS §3; rescue/a2p25-console-hide-no-media-policy merges clean
  onto main (merge-tree) — merge it after the a2p25 build is good.
