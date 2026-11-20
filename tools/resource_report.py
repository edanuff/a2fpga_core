#!/usr/bin/env python3
"""resource_report.py — per-module FPGA resource accounting for A2FPGA builds.

Reads the Gowin synthesis hierarchy report (impl/gwsynthesis/<proj>_syn_resource.html)
and the place-and-route totals (impl/pnr/<proj>.rpt.txt), writes a compact JSON
snapshot next to the project's process_config (boards/<board>/impl/<proj>_resources.json,
tracked in git), and diffs it against a baseline: the committed snapshot
(`git show <ref>:<path>`, default HEAD) or any snapshot file.

The point is the diff. Two silent regressions this project has already paid for
— a 128x48 "distributed_ram" event log that synthesised as 5.8k flops in every
core, and a parameter default that moved the DOC register banks into BSRAM on
boards that never asked — are each a one-line entry in this report.

Usage:
  tools/resource_report.py <board> [<proj>] [--baseline <ref|file|none>]
                           [--top N] [--fail-on bsram|lut|reg|none]... [--no-write]
Examples:
  tools/resource_report.py a2mega a2mega_138B            # diff vs HEAD's snapshot
  tools/resource_report.py a2p25 --baseline main          # diff vs main's snapshot
  tools/resource_report.py a2n20v2-Enhanced a2n20v2_enhanced --fail-on bsram

Exit status: 0 ok; 2 when a --fail-on resource increased versus the baseline;
3 when the reports are missing.
"""
import argparse, html, json, os, re, subprocess, sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COLS = ("reg", "alu", "lut", "dsp", "bsram", "ssram", "rom16")

def parse_hierarchy(path):
    """Gowin's syn_resource.html is one table: MODULE NAME | REG | ALU | LUT | DSP | BSRAM | SSRAM | ROM16.
    Each row's name cell carries the hierarchy as a '|--' prefix per level plus the source file in
    parentheses. Returns {instance_path: {col: int}} with paths like 'top/u_framebuffer'."""
    t = open(path, encoding="utf-8", errors="ignore").read()
    t = re.sub(r"<script.*?</script>|<style.*?</style>", "", t, flags=re.S)
    t = re.sub(r"</t[dh]>", "\x1f", t)
    t = re.sub(r"<[^>]+>", " ", t)
    t = html.unescape(t)
    toks = [re.sub(r"\s+", " ", x).strip() for x in t.split("\x1f")]
    toks = [x for x in toks if x]
    if "ROM16 NUMBER" not in toks:
        raise ValueError(f"unrecognised hierarchy report: {path}")
    toks = toks[toks.index("ROM16 NUMBER") + 1:]
    out, stack = {}, []
    for k in range(0, len(toks) - 7, 8):
        name_cell, vals = toks[k], toks[k + 1:k + 8]
        if not all(re.fullmatch(r"\d+|-", v) for v in vals):
            continue
        depth = name_cell.count("|--")
        name = re.sub(r"\(.*?\)", "", name_cell.replace("|--", "")).strip()
        stack = stack[:depth] + [name]
        out["/".join(stack)] = {c: (int(v) if v.isdigit() else 0) for c, v in zip(COLS, vals)}
    return out

def parse_pnr_totals(path):
    """Device-level utilisation from the PnR text report: {'Logic': (used, avail), ...}."""
    tot = {}
    if not os.path.exists(path):
        return tot
    for line in open(path, encoding="utf-8", errors="ignore"):
        m = re.match(r"^\s*(Logic|Register|BSRAM|CLS|DSP|PLL|DCS|DQCE|OSC)\s*\|\s*(\d+)/(\d+)", line)
        if m:
            tot[m.group(1)] = (int(m.group(2)), int(m.group(3)))
    return tot

def load_baseline(spec, snap_rel):
    """spec: 'none', a file path, or a git ref whose tree holds snap_rel."""
    if spec == "none":
        return None, "none"
    if os.path.exists(spec):
        return json.load(open(spec)), spec
    r = subprocess.run(["git", "-C", REPO, "show", f"{spec}:{snap_rel}"], capture_output=True, text=True)
    if r.returncode != 0:
        return None, f"{spec}:{snap_rel} (not present)"
    return json.loads(r.stdout), f"{spec}:{snap_rel}"

def fmt_delta(n):
    return f"{n:+d}" if n else ""

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("board")
    ap.add_argument("proj", nargs="?")
    ap.add_argument("--baseline", default="HEAD", help="git ref, snapshot file, or 'none' (default HEAD)")
    ap.add_argument("--top", type=int, default=25, help="rows in the module table")
    ap.add_argument("--fail-on", action="append", choices=["bsram", "lut", "reg", "none"], default=None,
                    help="exit 2 if this resource grew at the device level versus the baseline (repeatable)")
    ap.add_argument("--no-write", action="store_true", help="do not write the snapshot")
    a = ap.parse_args()

    brel = f"boards/{a.board}"
    if not os.path.isdir(os.path.join(REPO, brel)):
        brel = f"boards/deprecated/{a.board}"
    bdir = os.path.join(REPO, brel)
    if not os.path.isdir(bdir):
        sys.exit(f"no such board dir: boards/{a.board} (or boards/deprecated/{a.board})")
    proj = a.proj or sorted(f[:-5] for f in os.listdir(bdir) if f.endswith(".gprj"))[0]
    syn = os.path.join(bdir, "impl", "gwsynthesis", f"{proj}_syn_resource.html")
    rpt = os.path.join(bdir, "impl", "pnr", f"{proj}.rpt.txt")
    snap_rel = f"{brel}/impl/{proj}_resources.json"
    snap_abs = os.path.join(REPO, snap_rel)
    if not os.path.exists(syn):
        print(f"!! no synthesis hierarchy report at {os.path.relpath(syn, REPO)} — run a build first", file=sys.stderr)
        sys.exit(3)

    mods = parse_hierarchy(syn)
    totals = parse_pnr_totals(rpt)
    snap = {"board": a.board, "project": proj, "modules": mods, "pnr_totals": {k: list(v) for k, v in totals.items()}}
    base, base_desc = load_baseline(a.baseline, snap_rel)
    bmods = (base or {}).get("modules", {})
    btot = {k: tuple(v) for k, v in (base or {}).get("pnr_totals", {}).items()}

    print(f">> Resources: {brel} {proj}   (baseline: {base_desc})")
    print("   device (place & route):")
    for k in ("Logic", "Register", "CLS", "BSRAM", "DSP"):
        if k in totals:
            u, av = totals[k]
            d = f"  {fmt_delta(u - btot[k][0]):>6}" if k in btot else ""
            print(f"     {k:<9} {u:>7}/{av:<7} {100*u//av:>3}%{d}")

    # module table: top-level and second-level instances, by LUT; show deltas
    rows = []
    for p, v in mods.items():
        if p.count("/") <= 2:
            b = bmods.get(p)
            rows.append((p, v, b))
    rows.sort(key=lambda r: -(r[1]["lut"] + r[1]["reg"]))
    print(f"   modules (synthesis; top {a.top} by LUT+REG; deltas vs baseline):")
    print(f"     {'instance':<46} {'LUT':>6} {'':>6} {'REG':>6} {'':>6} {'BSRAM':>5} {'':>4}")
    for p, v, b in rows[: a.top]:
        dl = fmt_delta(v["lut"] - b["lut"]) if b else ("new" if base else "")
        dr = fmt_delta(v["reg"] - b["reg"]) if b else ""
        db = fmt_delta(v["bsram"] - b["bsram"]) if b else ""
        print(f"     {p[-46:]:<46} {v['lut']:>6} {dl:>6} {v['reg']:>6} {dr:>6} {v['bsram']:>5} {db:>4}")
    if base:
        changed = [(p, v, bmods.get(p)) for p, v in mods.items()
                   if p.count("/") <= 3 and bmods.get(p) != v]
        gone = [p for p in bmods if p.count("/") <= 3 and p not in mods]
        big = [(p, v, b) for p, v, b in changed
               if b is None or abs(v["lut"] - b["lut"]) + abs(v["reg"] - b["reg"]) >= 50 or v["bsram"] != b["bsram"]]
        if big or gone:
            print("   notable module changes (|LUT|+|REG| >= 50, or any BSRAM):")
            for p, v, b in sorted(big, key=lambda r: -(abs(r[1]["lut"] - (r[2] or {}).get("lut", 0)) + abs(r[1]["reg"] - (r[2] or {}).get("reg", 0)))):
                if b is None:
                    print(f"     + {p}: LUT {v['lut']} REG {v['reg']} BSRAM {v['bsram']} (new)")
                else:
                    print(f"     ~ {p}: LUT {fmt_delta(v['lut']-b['lut']) or '0'} REG {fmt_delta(v['reg']-b['reg']) or '0'} BSRAM {fmt_delta(v['bsram']-b['bsram']) or '0'}")
            for p in gone:
                print(f"     - {p}: removed")
        else:
            print("   no notable module changes versus the baseline.")

    if not a.no_write:
        os.makedirs(os.path.dirname(snap_abs), exist_ok=True)
        json.dump(snap, open(snap_abs, "w"), indent=0, sort_keys=True)
        print(f"   snapshot written: {snap_rel}")

    rc = 0
    for res in (a.fail_on or []):
        key = {"bsram": "BSRAM", "lut": "Logic", "reg": "Register"}.get(res)
        if key and key in totals and key in btot and totals[key][0] > btot[key][0]:
            print(f"   !! {key} grew: {btot[key][0]} -> {totals[key][0]} (--fail-on {res})")
            rc = 2
    sys.exit(rc)

if __name__ == "__main__":
    main()
