# docs/archive/merges

Status: created 2026-10-25. Fork-only: these files are private records of the
`edanuff` fork's own history and are excluded from the public upstream by
`UPSTREAM_EXCLUDE.txt` at the repo root.

| File | What it is | Date |
|---|---|---|
| [BRANCH_AUDIT_2026-10-22.md](BRANCH_AUDIT_2026-10-22.md) | Census of every branch and worktree before the big merge: what was merged, deleted or archive-tagged, and which orphan work was copied to the out-of-tree rescue folder (`a2fpga_core.rescue/2026-10-22/`, outside this repository by design; paths in it are left as written). | 2026-10-22 |
| [MERGE_RESOLUTIONS_2026-10-22.md](MERGE_RESOLUTIONS_2026-10-22.md) | Conflict-by-conflict record of the merge that brought the a2p25 displayport1 stack onto `main` (register remap, profile switches, DOC port priority). Sections 3 and 6 list the work that was still open at the time; the 138B `aux_channel` timing regression in section 6 was closed on 2026-10-25 (see `boards/a2mega/docs/timing_round2.md`). | 2026-10-22 to 2026-10-24 |

Both files were written outside git (in a worktree) and committed here
unchanged on 2026-10-25; the absolute paths they mention are the author's
machine at the time and are not meant to resolve.
