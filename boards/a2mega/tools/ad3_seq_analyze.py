#!/usr/bin/env python3
"""Offline analysis of a `ad3_reset_record.py drecord --save` capture of the IIgs
power-up sequence taken on the slot 7 breakout:

  DIO0 /RESET (pin 31)   DIO1 CREF (pin 35)   DIO2 +5V (pin 25)
  DIO3 PH0 (pin 40)      DIO4 7M (pin 36)     DIO5 SYNC.L (pin 19)

Reports, all relative to the first /RESET rise (t = 0):
  . rail up, first clock activity (7M/PH0/CREF), every /RESET edge
  . the composite-sync line train: line period, the vertical-sync intervals
    (lines whose low time is far longer than the ~4 us horizontal pulse),
    the line count from the clock start to each vertical sync, and the
    frame phase (lines since the last vertical sync) of every /RESET edge.

usage: ad3_seq_analyze.py capture.npy [--rate 500000]
"""
import sys, argparse
import numpy as np

def edges(bits):
    d = np.diff(bits.astype(np.int8))
    return np.flatnonzero(d > 0) + 1, np.flatnonzero(d < 0) + 1   # rises, falls

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("npy")
    ap.add_argument("--rate", type=float, default=500e3)
    ap.add_argument("--vsync-us", type=float, default=15.0, help="a low longer than this inside the sync train = vertical sync")
    a = ap.parse_args()
    w = np.load(a.npy)
    rate = a.rate
    us = 1e6 / rate
    b = {n: ((w >> n) & 1).astype(np.uint8) for n in range(6)}

    r_rise, r_fall = edges(b[0])
    if len(r_rise) == 0:
        print("no /RESET rise in the capture"); return
    # per power cycle (each +5V rise): clock start = first 7M edge after it, then every
    # /RESET edge of that cycle relative to the clock start
    v5r, _ = edges(b[2])
    m7r, m7f = edges(b[4]); m7 = np.sort(np.concatenate((m7r, m7f)))
    rall = np.sort(np.concatenate((r_rise, r_fall)))
    if len(v5r):
        print("power cycles (t from the clock start of each):")
        for c, t5 in enumerate(v5r):
            nxt = v5r[c + 1] if c + 1 < len(v5r) else len(w)
            e = m7[m7 > t5]
            if len(e) == 0: print("  cycle %d: +5V at %.3f s, no clock" % (c + 1, t5 / rate)); continue
            clk = int(e[0])
            ev = rall[(rall > clk) & (rall < nxt)]
            parts = ["%s %+.3f ms" % ("rise" if i in r_rise else "FALL", (i - clk) * 1000.0 / rate) for i in ev[:6]]
            print("  cycle %d: +5V at %.3f s, clock %+.1f ms after it; /RESET: %s" % (c + 1, t5 / rate, (clk - t5) * 1000.0 / rate, ", ".join(parts) if parts else "no edge"))
    t0 = int(r_rise[0])
    ms = lambda i: (i - t0) * 1000.0 / rate

    v5_rise, _ = edges(b[2])
    print("t = 0 at the first /RESET rise (sample %d)" % t0)
    if len(v5_rise): print("  +5V up            %12.3f ms" % ms(v5_rise[0]))
    for n, name in ((4, "7M"), (3, "PH0"), (1, "CREF")):
        rs, fs = edges(b[n])
        act = np.sort(np.concatenate((rs, fs)))
        # first sustained activity: first edge followed by >= 20 edges within 1 ms
        # (a clock above half the sample rate aliases, so the edge density is irregular)
        first = None
        for k in range(0, len(act) - 20):
            if act[k + 20] - act[k] < rate / 1000.0:
                first = act[k]; break
        print("  %-5s first active %12.3f ms" % (name, ms(first) if first is not None else float("nan")))
    print("  /RESET edges:")
    for i in sorted(np.concatenate((r_rise, r_fall)))[:12]:
        print("    %12.3f ms  %s" % (ms(i), "rise" if i in r_rise else "FALL"))

    # composite sync: lows are sync pulses; measure each low's length
    s_rise, s_fall = edges(b[5])
    if len(s_fall) == 0 or len(s_rise) == 0:
        print("no sync activity"); return
    if s_rise[0] < s_fall[0]: s_rise = s_rise[1:]
    n = min(len(s_fall), len(s_rise))
    lows_start = s_fall[:n]; lows_len = (s_rise[:n] - s_fall[:n]) * us
    # horizontal pulses: the common short low; line period from consecutive short pulses
    short = lows_len < a.vsync_us
    hs = lows_start[short]
    per = np.diff(hs)
    per = per[(per > 20 / us) & (per < 120 / us)]
    line_us = np.median(per) * us if len(per) else float("nan")
    print("sync: %d pulses, horizontal period %.3f us (median), %d long lows (>%.0f us)" % (n, line_us, int((~short).sum()), a.vsync_us))
    # vertical sync intervals: groups of long lows; take the first long low of each group
    longs = lows_start[~short]
    vs = []
    for i in longs:
        if not vs or (i - vs[-1]) > rate * 0.005:   # new group if > 5 ms after the previous
            vs.append(int(i))
    vs = np.array(vs)
    if len(vs) == 0:
        print("no vertical sync found (try --vsync-us)"); return
    fr = np.diff(vs) / rate * 1000.0
    print("vertical sync: %d found, frame period %.3f ms (median), first at %12.3f ms" % (len(vs), np.median(fr), ms(vs[0])))
    # clock start reference = first PH0 activity
    rs3, fs3 = edges(b[4]); act3 = np.sort(np.concatenate((rs3, fs3)))   # 7M: the densest clock line
    clk0 = None
    for k in range(0, len(act3) - 20):
        if act3[k + 20] - act3[k] < rate / 1000.0: clk0 = act3[k]; break
    if clk0 is not None and line_us == line_us:
        print("  lines from clock start to first vsync: %.2f" % ((vs[0] - clk0) * us / line_us))
    def phase(i):
        k = np.searchsorted(vs, i) - 1
        if k < 0: return None
        return k + 1, (i - vs[k]) * us / line_us, (i - vs[k]) * 1000.0 / rate
    for i in sorted(np.concatenate((r_rise, r_fall)))[:6]:
        p = phase(i)
        if p:
            print("  /RESET %s at %10.3f ms: vsync #%d + %.2f lines (%.3f ms into the frame)" % ("rise" if i in r_rise else "FALL", ms(i), p[0], p[1], p[2]))
        if clk0 is not None:
            print("      = %.2f lines after the clock start" % ((i - clk0) * us / line_us))

if __name__ == "__main__":
    main()
