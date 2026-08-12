#!/usr/bin/env python3
"""Decode DisplayPort AUX-channel captures (Manchester-II, 1 Mb/s) from an
Analog Discovery 2 export.

Input: CSV with two columns (time_seconds, level) — either a logic-analyzer
export or a scope export thresholded by --thresh. Extra columns/header rows
are tolerated. The AUX line is captured single-ended on AUX_P (SBU pin at a
USB-C breakout, or the DP connector's AUX+ on a DP breakout).

What it does:
  - splits the capture into bursts separated by idle gaps (> --gap us)
  - Manchester-decodes each burst, trying both phase conventions and both
    polarities; keeps the interpretation that yields a clean SYNC pattern
  - prints each burst's bytes and parses AUX request/reply framing:
      requests: cmd nibble (native/i2c, read/write), 20-bit address, length
      replies:  ACK / NACK / DEFER + data bytes
  - labels well-known DPCD addresses (LINK_BW_SET, LANE_COUNT_SET,
    TRAINING_PATTERN_SET, SET_POWER, LANE0_1_STATUS, ...)

Usage:
  aux_decode.py capture.csv [--thresh 0.7] [--gap 8] [--rate 1e6]

The point (bring-up log 2026-08-12): the monitor has been replying on the
wire all along — the TUSB1046A snooper proved our TX parses — and this
script is how we finally read the sink's side of the conversation without
a protocol analyzer.
"""
import sys, csv, argparse

DPCD_NAMES = {
    0x00000: "DPCD_REV", 0x00001: "MAX_LINK_RATE", 0x00002: "MAX_LANE_COUNT",
    0x00100: "LINK_BW_SET", 0x00101: "LANE_COUNT_SET",
    0x00102: "TRAINING_PATTERN_SET",
    0x00103: "TRAINING_LANE0_SET", 0x00104: "TRAINING_LANE1_SET",
    0x00107: "DOWNSPREAD_CTRL", 0x00108: "MAIN_LINK_CHANNEL_CODING_SET",
    0x00200: "SINK_COUNT", 0x00201: "DEVICE_SERVICE_IRQ_VECTOR",
    0x00202: "LANE0_1_STATUS", 0x00203: "LANE2_3_STATUS",
    0x00204: "LANE_ALIGN_STATUS_UPDATED", 0x00205: "SINK_STATUS",
    0x00206: "ADJUST_REQUEST_LANE0_1", 0x00207: "ADJUST_REQUEST_LANE2_3",
    0x00600: "SET_POWER",
}

def load_levels(path, thresh):
    pts = []
    with open(path) as f:
        for row in csv.reader(f):
            if len(row) < 2:
                continue
            try:
                t = float(row[0]); v = float(row[1])
            except ValueError:
                continue  # header
            pts.append((t, 1 if v >= thresh else 0))
    return pts

def edges_from_levels(pts):
    out = []
    last = None
    for t, v in pts:
        if last is None or v != last:
            out.append((t, v))
            last = v
    return out

def split_bursts(edges, gap_s):
    bursts, cur = [], []
    for i, (t, v) in enumerate(edges):
        if cur and t - cur[-1][0] > gap_s:
            bursts.append(cur); cur = []
        cur.append((t, v))
    if cur:
        bursts.append(cur)
    # a burst needs enough transitions to carry data
    return [b for b in bursts if len(b) > 20]

def manchester_bits(burst, bit_s, invert):
    """Sample the level at 1/4 and 3/4 of each bit cell; Manchester-II:
    bit value = level of the SECOND half (or first, per phase). We emit
    second-half convention; caller tries invert for the other polarity."""
    t0 = burst[0][0]
    t_end = burst[-1][0] + bit_s
    def level_at(t):
        lv = burst[0][1]
        for (te, ve) in burst:
            if te <= t:
                lv = ve
            else:
                break
        return lv
    bits = []
    t = t0
    while t < t_end:
        a = level_at(t + bit_s * 0.25)
        b = level_at(t + bit_s * 0.75)
        if a == b:
            bits.append(None)         # no mid-cell transition: sync/precharge edge zone
        else:
            v = b
            bits.append(v ^ invert)
        t += bit_s
    return bits

def bits_to_bytes(bits):
    # skip leading None/sync region: AUX bursts start with >=10 precharge
    # cells then a sync end; find the first run of 8 valid bits after the
    # last None gap near the start
    try:
        start = max(i for i, b in enumerate(bits[:40]) if b is None) + 1
    except ValueError:
        start = 0
    payload = [b for b in bits[start:] if b is not None]
    out = []
    for i in range(0, len(payload) - 7, 8):
        byte = 0
        for j in range(8):
            byte = (byte << 1) | payload[i + j]   # MSB first per DP AUX
        out.append(byte)
    return out

def parse_burst(bts, is_reply_hint):
    if not bts:
        return "(empty)"
    b0 = bts[0]
    cmd = (b0 >> 4) & 0xF
    if is_reply_hint or len(bts) <= 2:
        code = {0x0: "ACK", 0x1: "NACK", 0x2: "DEFER"}.get(cmd & 0x3, f"?{cmd:X}")
        return f"REPLY {code} data={' '.join(f'{x:02X}' for x in bts[1:])}"
    if len(bts) >= 4:
        addr = ((b0 & 0xF) << 16) | (bts[1] << 8) | bts[2]
        ln = bts[3] + 1
        name = DPCD_NAMES.get(addr, "")
        kind = "WR" if cmd in (0x8, 0x0) else "RD" if cmd in (0x9, 0x1) else f"C{cmd:X}"
        data = ' '.join(f'{x:02X}' for x in bts[4:])
        return f"REQ {kind} addr={addr:05X} {name} len={ln} data={data}"
    return "bytes=" + ' '.join(f'{x:02X}' for x in bts)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv")
    ap.add_argument("--thresh", type=float, default=0.7)
    ap.add_argument("--gap", type=float, default=8.0, help="idle gap, us")
    ap.add_argument("--rate", type=float, default=1e6)
    a = ap.parse_args()

    bit_s = 1.0 / a.rate
    pts = load_levels(a.csv, a.thresh)
    if not pts:
        sys.exit("no samples parsed — check CSV format/threshold")
    edges = edges_from_levels(pts)
    bursts = split_bursts(edges, a.gap * 1e-6)
    print(f"# {len(pts)} samples, {len(edges)} edges, {len(bursts)} bursts")
    prev_end = None
    for n, b in enumerate(bursts):
        best = None
        for inv in (0, 1):
            bts = bits_to_bytes(manchester_bits(b, bit_s, inv))
            if bts and (best is None or len(bts) > len(best[1])):
                best = (inv, bts)
        inv, bts = best
        # heuristics: a burst that follows another within ~300 us is a reply
        is_reply = prev_end is not None and (b[0][0] - prev_end) < 300e-6 and len(bts) <= 17
        t_ms = b[0][0] * 1e3
        print(f"[{n:03d}] t={t_ms:10.3f}ms inv={inv} {parse_burst(bts, is_reply)}")
        prev_end = b[-1][0]

if __name__ == "__main__":
    main()
