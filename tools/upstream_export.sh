#!/usr/bin/env bash
# Stage a tree for the public upstream (a2fpga/a2fpga_core) with UPSTREAM_EXCLUDE.txt
# applied, then gate it: no '/Users/' path may remain in the documentation and every
# relative markdown link must resolve. Prints the removed-file list for review.
#
# Usage:
#   tools/upstream_export.sh                 # dry run from HEAD: build, gate, report, clean up
#   tools/upstream_export.sh --rev <commit>  # export a different commit (e.g. the squash commit)
#   tools/upstream_export.sh --keep          # keep the export worktree and commit it on an
#                                            # orphan branch (upstream-export) for review/push
#   tools/upstream_export.sh --strict        # also fail on '/Users/' outside the documentation
#
# Nothing is pushed. With --keep the result is a detached worktree whose index holds
# the filtered tree, committed on branch 'upstream-export'; push it yourself after review:
#   git push upstream upstream-export:<target-branch>
set -euo pipefail

REV=HEAD
KEEP=0
STRICT=0
while [ $# -gt 0 ]; do
  case "$1" in
    --rev) REV="$2"; shift 2 ;;
    --keep) KEEP=1; shift ;;
    --strict) STRICT=1; shift ;;
    -h|--help) sed -n '2,16p' "$0"; exit 0 ;;
    *) echo "unknown argument: $1" >&2; exit 2 ;;
  esac
done

REPO=$(git rev-parse --show-toplevel)
MANIFEST="$REPO/UPSTREAM_EXCLUDE.txt"
CHECKER="$REPO/tools/check_doc_links.py"
[ -f "$MANIFEST" ] || { echo "missing $MANIFEST" >&2; exit 2; }
[ -f "$CHECKER" ] || { echo "missing $CHECKER" >&2; exit 2; }

SHA=$(git rev-parse --verify "$REV^{commit}")
EXPORT=$(mktemp -d "${TMPDIR:-/tmp}/upstream-export.XXXXXX")
rmdir "$EXPORT"   # git worktree add wants to create it

cleanup() {
  if [ "$KEEP" -eq 0 ] && [ -d "$EXPORT" ]; then
    git -C "$REPO" worktree remove --force "$EXPORT" >/dev/null 2>&1 || rm -rf "$EXPORT"
    git -C "$REPO" worktree prune >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

echo "export: $SHA -> $EXPORT"
git -C "$REPO" worktree add --quiet --detach "$EXPORT" "$SHA"

# --- apply the manifest -------------------------------------------------------
# Each non-comment line is a pathspec with :(glob) semantics; a trailing slash
# means the whole directory. Patterns that match nothing are fine.
REMOVED=$(mktemp "${TMPDIR:-/tmp}/upstream-export-removed.XXXXXX")
: > "$REMOVED"
while IFS= read -r line; do
  line="${line%%#*}"; line="${line#"${line%%[![:space:]]*}"}"; line="${line%"${line##*[![:space:]]}"}"
  [ -z "$line" ] && continue
  case "$line" in */) line="${line}**" ;; esac
  git -C "$EXPORT" ls-files -z -- ":(glob)$line" | tr '\0' '\n' >> "$REMOVED" || true
done < "$MANIFEST"
sort -u -o "$REMOVED" "$REMOVED"

if [ -s "$REMOVED" ]; then
  # Remove from the index and from the worktree so the gates see what upstream would.
  tr '\n' '\0' < "$REMOVED" | xargs -0 git -C "$EXPORT" rm -r -q --cached --
  while IFS= read -r f; do rm -rf "$EXPORT/$f"; done < "$REMOVED"
  # Drop directories the removals emptied.
  find "$EXPORT" -mindepth 1 -type d -empty -not -path "$EXPORT/.git*" -delete 2>/dev/null || true
fi

echo
echo "removed by UPSTREAM_EXCLUDE.txt ($(wc -l < "$REMOVED" | tr -d ' ') files):"
sed 's/^/  - /' "$REMOVED"
echo

# --- gates --------------------------------------------------------------------
FAIL=0

echo "gate 1: '/Users/' in the staged documentation"
DOC_HITS=$(git -C "$EXPORT" grep -n '/Users/' -- '*.md' '*.txt' ':!*/impl/*' ':!impl/*' || true)
if [ -n "$DOC_HITS" ]; then
  echo "$DOC_HITS" | sed 's/^/  FAIL /'
  FAIL=1
else
  echo "  clean"
fi
OTHER_HITS=$(git -C "$EXPORT" grep -ln '/Users/' -- ':!*.md' ':!*.txt' ':!*.fs' ':!*/impl/*' ':!impl/*' ':!*.pyc' ':!tools/upstream_export.sh' || true)
if [ -n "$OTHER_HITS" ]; then
  if [ "$STRICT" -eq 1 ]; then
    echo "  FAIL (--strict): '/Users/' outside the documentation:"; FAIL=1
  else
    echo "  warning: '/Users/' outside the documentation (not gated; use --strict to fail):"
  fi
  echo "$OTHER_HITS" | sed 's/^/    /'
fi
IMPL_HITS=$( (git -C "$EXPORT" grep -l '/Users/' -- '*/impl/*' || true) | wc -l | tr -d ' ')
[ "$IMPL_HITS" != "0" ] && echo "  note: $IMPL_HITS tracked build reports under impl/ carry the builder's absolute paths (vendor output, not gated)"

echo
echo "gate 2: markdown links in the staged tree"
if python3 "$CHECKER" --root "$EXPORT" --quiet; then
  :
else
  FAIL=1
fi

echo
if [ "$FAIL" -ne 0 ]; then
  echo "EXPORT GATES FAILED - fix the findings above before exporting."
  KEEP=0
  exit 1
fi
echo "export gates passed."

if [ "$KEEP" -eq 1 ]; then
  git -C "$EXPORT" checkout --quiet --orphan upstream-export
  git -C "$EXPORT" commit --quiet -m "Upstream export of $SHA (UPSTREAM_EXCLUDE.txt applied)"
  echo "export worktree kept at $EXPORT on orphan branch 'upstream-export' ($(git -C "$EXPORT" rev-parse --short HEAD))."
  echo "review it, then push it yourself; remove with: git worktree remove --force $EXPORT"
else
  echo "dry run: export worktree removed. Re-run with --keep to retain it."
fi
rm -f "$REMOVED"
