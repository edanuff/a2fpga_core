#!/usr/bin/env python3
"""Edge-timing DP AUX Manchester decoder (v2) for AD2/AD3 analog captures.

v1 (aux_decode.py) sampled at fixed quarter-cell offsets, which fails at
4 MS/s where quantization is a full quarter cell. v2 works from edge
TIMES: hysteresis comparator on a high-passed differential, then a
half-cell grid fit per burst, then half-cell levels -> Manchester bits
under all four (polarity x phase) conventions, scored by DP AUX framing
(SYNC precharge + sync-end, plausible command nibble).

Prints, per burst: the winning convention and the decoded bytes. The
convention COLUMN is the point: if a known-good (GPU) capture decodes
with the opposite polarity from ours, our AUX pair is inverted on the
wire.

Usage: aux_decode2.py capture.csv [--rate-hint 1e6] [--max-bursts N]
CSV: time_s, ch1_V[, ch2_V]  (2-col: single-ended; 3-col: ch1-ch2)
"""
import sys, csv, argparse, statistics

def stream_edges(path, hp_n=25, hyst_frac=0.25):
    """Yield (t, level) transitions from hysteresis on high-passed diff.

    Edge TIMES are linearly interpolated at the zero crossing of the
    high-passed signal between adjacent samples: at 4 MS/s the raw sample
    quantization (+-0.25 us) is half a Manchester half-cell, which is
    fatal for an off-phase source clock; interpolation recovers ~tens of
    ns. Rows where every channel reads exactly 0.0000 are record-loss
    gap filler (ad3_aux_capture zero-fills lost samples) — the detector
    resets across them instead of fabricating edges."""
    from collections import deque
    win = deque(maxlen=hp_n)
    level = None
    # single online pass with fixed fallback hysteresis; adaptive would need
    # two passes over 60M rows — use 60 mV which sits below golden (300 mV
    # legs) and ours (1.1 V) but above noise (~10 mV)
    hyst = 0.06
    t_prev = hp_prev = None
    t_zc = None                      # most recent zero-crossing time of hp
    with open(path) as f:
        for row in csv.reader(f):
            t = float(row[0]); a = float(row[1])
            b = float(row[2]) if len(row) >= 3 else None
            if a == 0.0 and (b is None or b == 0.0):
                win.clear(); level = None
                t_prev = hp_prev = t_zc = None
                continue
            v = a - b if b is not None else a
            win.append(v)
            base = sum(win) / len(win)
            hp = v - base
            if level is None:
                level = 1 if hp > 0 else 0
                t_prev, hp_prev = t, hp
                continue
            if hp_prev is not None and hp != hp_prev and \
               (hp_prev <= 0.0 <= hp or hp_prev >= 0.0 >= hp):
                t_zc = t_prev + (0.0 - hp_prev) * (t - t_prev) / (hp - hp_prev)
            if level == 0 and hp > hyst:
                level = 1
                yield (t_zc if t_zc is not None else t, 1)
            elif level == 1 and hp < -hyst:
                level = 0
                yield (t_zc if t_zc is not None else t, 0)
            t_prev, hp_prev = t, hp

def group_bursts(edges, gap_s=8e-6, min_edges=16):
    cur = []
    for e in edges:
        if cur and e[0] - cur[-1][0] > gap_s:
            if len(cur) >= min_edges:
                yield cur
            cur = []
        cur.append(e)
    if len(cur) >= min_edges:
        yield cur

def halfcells(burst, half_s=0.5e-6):
    """Reconstruct half-cell levels with per-edge clock recovery: each
    inter-edge interval is 1 or 2 half-cells (Manchester property); the
    half-cell estimate tracks the sender's clock so long bursts from a
    +-5% off-nominal source (a real sink's reply) stay locked."""
    ivs = [burst[i+1][0] - burst[i][0] for i in range(len(burst) - 1)]
    # per-burst clock fit: the short-interval population IS the half-cell
    shorts = sorted(iv for iv in ivs if iv < 0.75 * 2 * half_s)
    half = shorts[len(shorts)//2] if shorts else half_s
    if not (0.3e-6 < half < 0.7e-6):
        half = half_s
    out = []
    for i, iv in enumerate(ivs):
        k = int(round(iv / half))
        if k < 1: k = 1
        if k > 8: k = 8            # sync-end runs span up to 4; clamp loosely
        out.extend([burst[i][1]] * k)
    out.extend([burst[-1][1]] * 2)
    return out

def bits_from_halfcells(h, phase, pol):
    """phase 0: pairs (h[0],h[1]); phase 1: skip one half-cell first.
    Manchester bit = SECOND half-cell value, XOR pol."""
    bits = []
    i = phase
    while i + 1 < len(h):
        a, b = h[i], h[i+1]
        if a == b:
            bits.append(None)          # no mid transition: sync-end zone
        else:
            bits.append(b ^ pol)
        i += 2
    return bits

def frame_score_and_bytes(bits):
    """DP AUX: >=8 precharge bits, then sync-end (>=2 None cells), then
    payload bytes MSB-first, ending near a trailing None zone (STOP)."""
    # find last None-run within the first ~20 cells
    idx = None
    run = 0
    for i, b in enumerate(bits[:48]):
        if b is None:
            run += 1
            if run >= 2:
                idx = i + 1
        else:
            run = 0
    if idx is None:
        return -1, []
    payload = []
    for b in bits[idx:]:
        if b is None:
            break
        payload.append(b)
    if len(payload) < 8:
        return -1, []
    nbytes = len(payload) // 8
    out = []
    for i in range(nbytes):
        byte = 0
        for j in range(8):
            byte = (byte << 1) | payload[i*8 + j]
        out.append(byte)
    # score: precharge before sync-end should alternate (decode to 0s or 1s
    # consistently); full bytes; more bytes = better anchored
    score = nbytes
    pre = [b for b in bits[:idx-2] if b is not None]
    if pre and len(set(pre)) == 1:
        score += 4
    return score, out

def frames_from_halfcells(h):
    """Anchor-based frame parser: find each frame's SYNC-END and parse the
    payload from that anchor, instead of pair-parsing the whole burst with
    one global phase (where a single odd-length run shifts everything
    after it).

    Structure exploited: precharge = alternating half-cells = a long
    stretch of length-1 runs; sync-end = the first pair of longer runs
    (nominally 4 high + 4 low half-cells); payload = Manchester pairs
    right after. The sync-end's first-run VALUE defines line polarity per
    frame (bit = 1 when the second half-cell of a pair matches it), so
    inverted probe orientation decodes identically. A +-1 half-cell
    boundary slip at the anchor is resolved by trying both offsets and
    keeping the parse with more whole bytes / fewer mid-payload slips.
    Yields byte lists; multiple frames per burst (request + fast reply)
    come out separately."""
    runs = []                              # [value, length, start]
    for i, v in enumerate(h):
        if runs and runs[-1][0] == v:
            runs[-1][1] += 1
        else:
            runs.append([v, 1, i])
    out = []
    nr = len(runs)
    i = 0
    while i < nr:
        # scan for anchor: >=8 consecutive len-1 runs then a len>=2 run
        cnt = 0
        j = i
        anchor = None
        while j < nr:
            if runs[j][1] == 1:
                cnt += 1
            else:
                if cnt >= 8 and runs[j][1] >= 2:
                    anchor = j
                    break
                cnt = 0
            j += 1
        if anchor is None or anchor + 1 >= nr:
            break
        r1, r2 = runs[anchor], runs[anchor + 1]
        if r2[1] < 2:                      # lone long run, not a sync-end
            i = anchor + 1
            continue
        v1 = r1[0]
        pstart = r2[2] + r2[1]
        best = None
        # off -1: the payload's first half-cell can merge into the sync-end's
        # second run when it has the same value — payload then starts one
        # half-cell INSIDE the anchor run
        for off in (-1, 0, 1):
            k = pstart + off
            bits = []
            slips = 0
            stopped = False
            end = k
            while k + 1 < len(h):
                a, b = h[k], h[k + 1]
                if a == b:
                    m = k
                    while m < len(h) and h[m] == a:
                        m += 1
                    if m - k >= 3:         # STOP (or inter-frame idle)
                        stopped = True
                        end = m
                        break
                    slips += 1             # boundary slip: realign
                    if slips > 4:
                        end = k
                        break
                    k += 1
                    continue
                # Manchester II per DP AUX: '0' rises mid-cell, '1' falls —
                # so bit 1 = second half-cell at the sync-end's FIRST-run
                # level being absent (b != v1, v1 = the high side)
                bits.append(0 if b == v1 else 1)
                k += 2
                end = k
            nbytes = len(bits) // 8
            score = nbytes * 10 - slips * 3 + (5 if stopped else 0) \
                    - (len(bits) % 8)
            if best is None or score > best[0]:
                best = (score, bits, end)
        _, bits, end = best
        if len(bits) >= 8:
            frame = []
            for k in range(len(bits) // 8):
                byte = 0
                for m in range(8):
                    byte = (byte << 1) | bits[k * 8 + m]
                frame.append(byte)
            out.append(frame)
        while i < nr and runs[i][2] + runs[i][1] <= end:
            i += 1
        if i <= anchor:                    # always make progress
            i = anchor + 1
    return out

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv")
    ap.add_argument("--max-bursts", type=int, default=40)
    a = ap.parse_args()

    n = 0
    for burst in group_bursts(stream_edges(a.csv)):
        n += 1
        if n > a.max_bursts:
            break
        h = halfcells(burst)
        best = None
        for pol in (0, 1):
            for phase in (0, 1):
                bits = bits_from_halfcells(h, phase, pol)
                sc, bts = frame_score_and_bytes(bits)
                if best is None or sc > best[0]:
                    best = (sc, bts, pol, phase)
        sc, bts, pol, phase = best
        t_ms = burst[0][0] * 1e3
        if sc < 0:
            print(f"[{n:03d}] t={t_ms:9.3f}ms  ({len(burst)} edges) no frame")
            continue
        bs = ' '.join(f'{x:02X}' for x in bts[:24])
        print(f"[{n:03d}] t={t_ms:9.3f}ms pol={pol} phase={phase} "
              f"bytes[{len(bts)}]: {bs}")

if __name__ == "__main__":
    main()
