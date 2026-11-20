#!/usr/bin/env python3
"""Check that relative markdown links in the repository's documentation resolve.

Scope (default): every tracked *.md file outside impl/ output folders, plus the
.claude/skills/*/SKILL.md files. Only relative links are checked: a link whose
target has a URL scheme (http:, https:, mailto:, ...) or is a bare #anchor is
skipped. Fragments (#section) are stripped before the path is checked; heading
anchors themselves are not validated. Links inside fenced code blocks are ignored.

Links from files under an archive/ folder, and from the dated docs/DOCS_AUDIT_*.md
reports, are reported as warnings, not errors: archived documents describe removed
code by design and an audit quotes broken links as findings.

Usage:
    tools/check_doc_links.py            # whole repo, exit 1 on any error
    tools/check_doc_links.py docs AGENTS.md boards/a2mega/README.md
    tools/check_doc_links.py --root <dir>   # check a staged export tree instead

Exit status: 0 when every non-archive link resolves, 1 otherwise.
"""
import argparse
import os
import re
import subprocess
import sys

LINK_RE = re.compile(r'(?<!\\)!?\[[^\]]*\]\(\s*<?([^)\s>]+)>?(?:\s+"[^"]*")?\s*\)')
SCHEME_RE = re.compile(r'^[a-zA-Z][a-zA-Z0-9+.-]*:')
FENCE_RE = re.compile(r'^\s*(```|~~~)')


def tracked_markdown(root):
    out = subprocess.run(['git', '-C', root, 'ls-files', '-z', '--', '*.md', '**/*.md',
                          '.claude/skills/*/SKILL.md'],
                         check=True, capture_output=True).stdout
    files = sorted(set(p.decode() for p in out.split(b'\0') if p))
    return [f for f in files if '/impl/' not in f and not f.startswith('impl/')]


def expand_targets(root, targets):
    files = []
    for t in targets:
        full = os.path.join(root, t)
        if os.path.isdir(full):
            for d, _, names in os.walk(full):
                for n in names:
                    if n.endswith('.md'):
                        files.append(os.path.relpath(os.path.join(d, n), root))
        else:
            files.append(t)
    return sorted(set(files))


def check_file(root, rel):
    """Return a list of (lineno, target) for links in rel that do not resolve."""
    bad = []
    base = os.path.dirname(os.path.join(root, rel))
    in_fence = False
    with open(os.path.join(root, rel), encoding='utf-8', errors='replace') as fh:
        for lineno, line in enumerate(fh, 1):
            if FENCE_RE.match(line):
                in_fence = not in_fence
                continue
            if in_fence:
                continue
            for m in LINK_RE.finditer(line):
                target = m.group(1)
                if SCHEME_RE.match(target) or target.startswith('#'):
                    continue
                path = target.split('#', 1)[0]
                if not path:
                    continue
                path = path.replace('%20', ' ')
                if not os.path.exists(os.path.normpath(os.path.join(base, path))):
                    bad.append((lineno, target))
    return bad


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('targets', nargs='*', help='files or directories (default: all tracked markdown)')
    ap.add_argument('--root', default='.', help='repository or export tree to check')
    ap.add_argument('--quiet', action='store_true', help='print only the summary line')
    args = ap.parse_args()
    root = os.path.abspath(args.root)

    files = expand_targets(root, args.targets) if args.targets else tracked_markdown(root)
    errors = warnings = 0
    for rel in files:
        if not os.path.exists(os.path.join(root, rel)):
            continue
        archived = '/archive/' in ('/' + rel) or os.path.basename(rel).startswith('DOCS_AUDIT_')
        for lineno, target in check_file(root, rel):
            if archived:
                warnings += 1
                if not args.quiet:
                    print(f'warning: {rel}:{lineno}: {target} (archived document)')
            else:
                errors += 1
                print(f'error: {rel}:{lineno}: {target}')
    print(f'check_doc_links: {len(files)} files, {errors} broken links, {warnings} warnings in archive/')
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
