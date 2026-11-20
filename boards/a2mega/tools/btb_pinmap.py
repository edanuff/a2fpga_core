#!/usr/bin/env python3
"""Extract the a2mega BTB-connector -> FPGA-ball pin map from the a2-mega
KiCad hardware repo and emit docs/a2mega_pinmap_<rev>.csv.

Method (see docs/btb_pinout_1_0a3.md for the worked 1.0a3 result):
  - net names come from the pad records in a2-mega.kicad_pcb
    (`(pad "N" ... (net "name"))` — footprints J1/J2/J3 on 1.0a3);
  - FPGA pin names come from the free text annotations
    ("BANK2_D19_IOR29B", "Q0_L0.TXP", "JTAG.TMS", ...) placed beside each
    connector pin in fpga_som_btb0/1/2.kicad_sch. Connector-pin absolute
    positions are computed from the symbol-instance transform; texts are
    clustered into a left and a right column (odd pins label left, even
    pins label right) and matched by row (same y within 0.9 mm).

The a2-mega sheets use Tang Mega 138K pin names. Ball names carry over
1:1 to the Tang Mega 60K (same PG484 package and SOM PCB), but bank
numbering and IO names differ per die. Per-die names come from
docs/tang_mega_die_names.json, extracted from the Sipeed SOM schematics
(tang_mega_60k_30353_Schematics.pdf / tang_mega_138k_30354_Schematics.pdf
via `pdftotext -layout`; NB the 138K PDF's embedded font maps text
shifted by -0x1D — add 0x1D per character to decode). The Sipeed 138K
names agree with the a2-mega annotations on every overlapping ball
(0 mismatches, 135-row check); the 138K PDF labels fewer balls than the
60K one, so io_138k falls back to the a2-mega annotation.

SERDES caveat: the Sipeed *60K* sheet labels the BTB DP pins with
138K-style lane names (e.g. Q0_LN1 on the pins that are die-true lane 3
on the GW5AT-60) — exactly the "SOM PCB uses 138K net conventions"
premise of the die-true lane note on the a2-mega BTB1 sheet. The lane
columns below record the die-true mapping (timing-verified IP), not the
Sipeed sheet labels.

Validated against the working 1.0a2 boards/a2mega/hdl/a2mega.cst by
joining this ball table with the 1.0a2a-branch PCB netlist: 12/12
signals, counting the deliberate USB D+/D- swap.

Re-run this when the board revision changes (1.0a4 moves GS nets and adds
HyperRAM):  python3 btb_pinmap.py /path/to/a2-mega [rev-label]
Requires: pip install sexpdata. Connector refs (J1/J2/J3) and the SERDES
rows below are 1.0a3-specific — review both when re-targeting.
"""
import csv
import json
import math
import os
import re
import sys

import sexpdata


def load(fn):
    with open(fn) as f:
        return sexpdata.loads(f.read())


def walk(node, name):
    if isinstance(node, list):
        if node and isinstance(node[0], sexpdata.Symbol) and str(node[0]) == name:
            yield node
        for child in node:
            yield from walk(child, name)


def get(node, name):
    for child in node:
        if (isinstance(child, list) and child and
                isinstance(child[0], sexpdata.Symbol) and str(child[0]) == name):
            return child
    return None


def prop(node, key):
    for child in node:
        if (isinstance(child, list) and child and
                isinstance(child[0], sexpdata.Symbol) and
                str(child[0]) == 'property' and str(child[1]) == key):
            return str(child[2])
    return None


def extract_balls(fn, ref):
    """Connector pin number -> FPGA pin-name text for one BTB sheet."""
    doc = load(fn)
    libs = {}
    lib_symbols = next(walk(doc, 'lib_symbols'), None)
    for sym in lib_symbols[1:]:
        if not (isinstance(sym, list) and str(sym[0]) == 'symbol'):
            continue
        pins = {}
        for unit in walk(sym, 'symbol'):
            for pin in walk(unit, 'pin'):
                at = get(pin, 'at')
                num = get(pin, 'number')
                if at is None or num is None:
                    continue
                pins[str(num[1])] = (float(at[1]), float(at[2]))
        if pins:
            libs[str(sym[1])] = pins

    pin_pos = {}
    for inst in doc[1:]:
        if not (isinstance(inst, list) and str(inst[0]) == 'symbol'):
            continue
        lib_id = get(inst, 'lib_id')
        at = get(inst, 'at')
        if lib_id is None or at is None or str(lib_id[1]) not in libs:
            continue
        if prop(inst, 'Reference') != ref:
            continue
        ix, iy = float(at[1]), float(at[2])
        angle = float(at[3]) if len(at) > 3 else 0.0
        mirror = get(inst, 'mirror')
        mir = str(mirror[1]) if mirror else None
        a = math.radians(angle)
        ca, sa = math.cos(a), math.sin(a)
        for num, (px, py) in libs[str(lib_id[1])].items():
            x, y = px, py
            if mir == 'x':
                y = -y
            elif mir == 'y':
                x = -x
            pin_pos[num] = (ix + x * ca + y * sa, iy - (-x * sa + y * ca))

    texts = []
    for t in doc[1:]:
        if not (isinstance(t, list) and str(t[0]) == 'text'):
            continue
        at = get(t, 'at')
        s = str(t[1])
        if re.match(r'(BANK\d|Q0|VCCIO|GND|\+|,)', s) or \
                any(k in s for k in ('JTAG', 'MODE', 'RECONFIG', 'DONE')):
            texts.append((s, float(at[1]), float(at[2])))
    if not texts or not pin_pos:
        return {}

    xs = sorted(x for _, x, _ in texts)
    mid = (xs[0] + xs[-1]) / 2
    left = [(s, x, y) for s, x, y in texts if x < mid]
    right = [(s, x, y) for s, x, y in texts if x >= mid]

    out = {}
    for num, (ax, ay) in pin_pos.items():
        pool = left if int(num) % 2 else right
        cands = sorted((abs(ty - ay), s) for s, tx, ty in pool
                       if abs(ty - ay) < 0.9)
        out[num] = cands[0][1] if cands else None
    return out


def pcb_pads(fn, refs):
    """Footprint ref -> {pad_number: net_name}."""
    out = {r: {} for r in refs}
    for fp in open(fn).read().split('\n\t(footprint ')[1:]:
        m = re.search(r'\(property "Reference" "([^"]+)"', fp)
        if not m or m.group(1) not in refs:
            continue
        for pm in re.finditer(r'\(pad "([^"]+)"', fp):
            chunk = fp[pm.start():pm.start() + 600]
            nm = re.search(r'\(net (?:\d+ )?"([^"]*)"', chunk)
            if nm:
                out[m.group(1)].setdefault(pm.group(1), nm.group(1))
    return out


# net (short name) -> ESP32-S3 GPIO, from mcu.kicad_sch on the 1.0a3 branch
ESP32 = {
    'ESP32_I2C_SCL': 'IO1', 'ESP32_I2C_SDA': 'IO2', 'FUSB302B_INT_N': 'IO3',
    'ESP32_INT_N': 'IO4', 'ESP32_PPO_SYNC': 'IO5', 'ESP32_PPO_D0': 'IO6',
    'ESP32_PPO_D1': 'IO7', 'ESP32_PPO_D2': 'IO8', 'ESP32_PPO_D3': 'IO9',
    'ESP32_PPO_CLK': 'IO10', 'ESP32_OPI_D0': 'IO11', 'ESP32_OPI_D1': 'IO12',
    'ESP32_OPI_D2': 'IO13', 'ESP32_OPI_D3': 'IO14', 'ESP32_OPI_D4': 'IO15',
    'ESP32_OPI_D5': 'IO16', 'ESP32_OPI_D6': 'IO17', 'ESP32_OPI_D7': 'IO18',
    'ESP32_USB_D_N': 'IO19', 'ESP32_USB_D_P': 'IO20', 'ESP32_OPI_CS': 'IO21',
    'ESP32_GPIO0': 'IO33', 'ESP32_GPIO1': 'IO34', 'ESP32_GPIO2': 'IO35',
    'ESP32_GPIO3': 'IO36', 'ESP32_GPIO4': 'IO37', 'ESP32_GPIO5': 'IO38',
    'ESP32_JTAG_TDO': 'IO39', 'ESP32_JTAG_TCK': 'IO40',
    'ESP32_JTAG_TMS': 'IO41', 'ESP32_JTAG_TDI': 'IO42',
    'ESP32_TX': 'IO43 (TXD0)', 'ESP32_RX': 'IO44 (RXD0)', 'FPGA_DONE': 'IO48',
}

# Anonymous resistor nets that are really LEDs (1.0a3)
LED = {'J2.34': 'LED D1 (R23)', 'J2.36': 'LED D2 (R24)',
       'J3.55': 'LED D3 (R25)', 'J3.57': 'LED D4 (R26)'}

# SERDES dedicated bumps: die-true mapping from the BTB1 sheet note.
# Q0 lanes 1 and 3 swap ball positions between the GW5AT-60 and GW5AST-138.
SERDES_ROWS = [
    ('DP_REFCLK_P', 'J2.73', 'F10', 'Q0_REFCLK1', 'Q0_REFCLK1', 'Q0_REFCLK1',
     'DSC1103 135 MHz LVDS XO, AC-coupled'),
    ('DP_REFCLK_N', 'J2.71', 'E10', 'Q0_REFCLK1', 'Q0_REFCLK1', 'Q0_REFCLK1', ''),
    ('DP0_P', 'J2.77', 'C7', 'Q0 SERDES', 'Q0 lane 3 TXM', 'Q0 lane 1',
     'P/N swapped - tx_pol_invert (set in the 2-lane IP)'),
    ('DP0_N', 'J2.79', 'D7', 'Q0 SERDES', 'Q0 lane 3 TXP', 'Q0 lane 1', ''),
    ('DP1_P', 'J2.83', 'A6', 'Q0 SERDES', 'Q0 lane 2 TXM', 'Q0 lane 2',
     'P/N swapped - tx_pol_invert (set); bonding master (2-lane IP)'),
    ('DP1_N', 'J2.85', 'B6', 'Q0 SERDES', 'Q0 lane 2 TXP', 'Q0 lane 2', ''),
    ('DP2_P', 'J2.89', 'C5', 'Q0 SERDES', 'Q0 lane 1 TXM', 'Q0 lane 3',
     'P/N swapped - future 4-lane: tx_pol_invert'),
    ('DP2_N', 'J2.91', 'D5', 'Q0 SERDES', 'Q0 lane 1 TXP', 'Q0 lane 3', ''),
    ('DP3_P', 'J2.95', 'A4', 'Q0 SERDES', 'Q0 lane 0 TXM', 'Q0 lane 0',
     'P/N swapped - future 4-lane: tx_pol_invert. RESOLVED 2026-08-09: '
     'Sipeed 60K U1L (die-true, matches UG1222) + BTB nets give pin95=A4=TXM; '
     'ALL FOUR lanes are uniformly TXM-first. The a2-mega BTB1 note saying '
     'DP3 NOT swapped (and its per-pair ball orderings) needs correcting.'),
    ('DP3_N', 'J2.97', 'B4', 'Q0 SERDES', 'Q0 lane 0 TXP', 'Q0 lane 0', ''),
]


def main():
    hw_repo = sys.argv[1] if len(sys.argv) > 1 else '/Users/edanuff/GitHub/a2-mega'
    rev = sys.argv[2] if len(sys.argv) > 2 else '1_0a3'

    die_path = os.path.join(os.path.dirname(__file__), '..', 'docs',
                            'tang_mega_die_names.json')
    die = json.load(open(die_path)) if os.path.exists(die_path) else {'60k': {}, '138k': {}}

    balls = {
        'J1': extract_balls(os.path.join(hw_repo, 'fpga_som_btb0.kicad_sch'), 'J1'),
        'J2': extract_balls(os.path.join(hw_repo, 'fpga_som_btb1.kicad_sch'), 'J2'),
        'J3': extract_balls(os.path.join(hw_repo, 'fpga_som_btb2.kicad_sch'), 'J3'),
    }
    pads = pcb_pads(os.path.join(hw_repo, 'a2-mega.kicad_pcb'), ['J1', 'J2', 'J3'])

    def parse_ball(t):
        if not t:
            return ('', '', '')
        m = re.match(r'BANK(\d+)_([A-Z]+\d+)(?:_(\S+))?', t)
        if m:
            return (m.group(2), 'BANK' + m.group(1), m.group(3) or '')
        return ('', '', t)

    rows = []
    for ref in ('J1', 'J2', 'J3'):
        for pin, net in sorted(pads[ref].items(), key=lambda kv: int(kv[0])):
            if net in ('GND', '') or net.startswith('unconnected') or net.startswith('+'):
                continue
            short = net.split('/')[-1]
            if re.match(r'DP_REFCLK|DP[0-3]_[PN]$', short):
                continue    # SERDES dedicated bumps: curated SERDES_ROWS below
            key = f'{ref}.{pin}'
            if short.startswith('Net-('):
                short = LED.get(key, short)
            ball, bank, ioname = parse_ball(balls[ref].get(pin))
            ann138 = (bank + '_' + ioname) if ball else (balls[ref].get(pin) or '')
            rows.append({
                'signal': short,
                'esp32_s3': ESP32.get(short, ''),
                'btb': key,
                'ball': ball,
                'io_60k': die['60k'].get(ball, ''),
                'io_138k': die['138k'].get(ball, ann138.rstrip('_')),
                'lane_60k': '', 'lane_138k': '',
                'notes': '',
            })

    for sig, btb, ball, io, l60, l138, note in SERDES_ROWS:
        rows.append({'signal': sig, 'esp32_s3': '', 'btb': btb, 'ball': ball,
                     'io_60k': io, 'io_138k': io,
                     'lane_60k': l60, 'lane_138k': l138, 'notes': note})

    for sig, ball, note in (
            ('clk (50 MHz osc)', 'V22', 'SOM oscillator (EMCCLK)'),
            ('button', 'AB13', 'LVCMOS15 (1.5 V bank on both dies)')):
        rows.append({'signal': sig, 'esp32_s3': '', 'btb': 'SOM',
                     'ball': ball,
                     'io_60k': die['60k'].get(ball, ''),
                     'io_138k': die['138k'].get(ball, ''),
                     'lane_60k': '', 'lane_138k': '', 'notes': note})

    out = os.path.join(os.path.dirname(__file__), '..', 'docs',
                       f'a2mega_pinmap_{rev}.csv')
    with open(out, 'w', newline='') as f:
        f.write('# a2mega %s master pin map - signal / ESP32-S3 / BTB position / FPGA ball / per-die IO names\n' % rev)
        f.write('# Ball names are identical on Tang Mega 60K (GW5AT-60) and 138K (GW5AST-138): same PG484 package,\n')
        f.write('#  same SOM PCB. Bank numbering and IO names DIFFER per die - io_60k / io_138k columns from the\n')
        f.write('#  Sipeed SOM schematics (tang_mega_60k_30353 / tang_mega_138k_30354), see tang_mega_die_names.json.\n')
        f.write('#  The SERDES Q0 lanes 1 and 3 swap between the dies - lane columns give the die-true mapping\n')
        f.write('#  (NB the Sipeed 60K sheet itself labels the BTB DP pins with 138K-style lane names).\n')
        f.write('# NOTE: pins move again on board rev 1.0a4 (HyperRAM added, GS nets relocated) - re-extract with\n')
        f.write('#  boards/a2mega/tools/btb_pinmap.py before targeting 1.0a4.\n')
        f.write('# Sources: a2-mega repo (kicad_pcb pad nets + fpga_som_btb*.kicad_sch pin-name texts) + Sipeed SOM\n')
        f.write('#  schematics; validated against the working 1.0a2 a2mega.cst via the 1.0a2a netlist (12/12) and\n')
        f.write('#  0 mismatches between the a2-mega annotations and the Sipeed 138K PDF on overlapping balls.\n')
        w = csv.DictWriter(f, fieldnames=['signal', 'esp32_s3', 'btb', 'ball',
                                          'io_60k', 'io_138k', 'lane_60k',
                                          'lane_138k', 'notes'])
        w.writeheader()
        for r in rows:
            w.writerow(r)
    print(f'wrote {out} ({len(rows)} rows)')


if __name__ == '__main__':
    main()
