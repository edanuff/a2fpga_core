# docs/archive

Status: created 2026-10-25 by the documentation consolidation (see
`docs/DOCS_AUDIT_2026-10.md`, a fork-only record).

What lives here: documents from the repository-wide (not board-specific)
record that no longer describe the code or the current plan but are kept as
history. Nothing in `archive/` is maintained; read the surviving document named
in each entry for the current state.

The archive convention (also in [conventions.md](../conventions.md)): every
documentation folder has at most one `archive/` beside it. A document that no
longer describes the code moves there with `git mv` (history preserved), gets a
one-line entry in that archive's README naming what superseded it, and every
code comment or document that cites it is updated in the same commit.

| Folder | Contents | Period | Upstream |
|---|---|---|---|
| `merges/` | Records of the 2026-10-22 branch census and the merge that landed the a2p25 stack on `main` | 2026-10-22 to 2026-10-24 | fork-only (`UPSTREAM_EXCLUDE.txt`) |

Board history lives beside each board: [boards/a2mega/docs/archive/](../../boards/a2mega/docs/archive/README.md),
[boards/a2p25/docs/archive/](../../boards/a2p25/docs/archive/README.md),
[boards/deprecated/a2n20v2-GS/docs/archive/](../../boards/deprecated/a2n20v2-GS/docs/archive/README.md).
