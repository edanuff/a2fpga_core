#!/usr/bin/env python3
"""Lane presence + polarity verdict with an Analog Discovery 2/3.

Pairs with the dp_lane_probe bitstream (TX_PROBE=1): the SERDES transmits
a slow asymmetric square (~1 MHz, 75% high duty in the LANE's P-polarity
convention) on both DP lanes continuously. Probe one leg of one pair at
the USB-C breakout SS pins (through the connector AC caps the DC is
blocked, but duty asymmetry survives as an asymmetric AC waveform).

Buffer capture at 50 MS/s on channels 1 (and 2 if connected), then per
channel report:
  - Vpp: > ~80 mV  => lane present and driven ("ALIVE")
  - dominant period ~0.95 us  => it is our probe pattern, not noise
  - duty of the positive half (time above midpoint):
      ~75% => this leg carries P-convention TRUE polarity
      ~25% => this leg is the INVERTED polarity (or you probed N — swap
              legs to confirm; P and N must read opposite duties)

Usage: ad3_lane_probe.py [--rate 50e6] [--range 1.0] [--ch2]
"""
import sys, argparse, ctypes, time

def load_dwf():
    import platform
    if platform.system() == "Darwin":
        for cand in ("/Library/Frameworks/dwf.framework/dwf",
                     "/Applications/WaveForms.app/Contents/Frameworks/"
                     "dwf.framework/Versions/A/dwf"):
            try:
                return ctypes.cdll.LoadLibrary(cand)
            except OSError:
                continue
        raise OSError("dwf library not found — install WaveForms")
    return ctypes.cdll.LoadLibrary("libdwf.so")

def analyze(name, vs, rate):
    n = len(vs)
    vmin, vmax = min(vs), max(vs)
    vpp = vmax - vmin
    mean = sum(vs) / n
    mid = (vmax + vmin) / 2.0
    above = sum(1 for v in vs if v > mid)
    duty = above / n
    # crude period: count falling crossings of mid
    crossings = 0
    prev = vs[0] > mid
    first = last = None
    for i in range(1, n):
        cur = vs[i] > mid
        if prev and not cur:
            crossings += 1
            if first is None: first = i
            last = i
        prev = cur
    period_us = None
    if crossings >= 2:
        period_us = (last - first) / (crossings - 1) / rate * 1e6
    verdict = "DEAD (no signal)" if vpp < 0.08 else "ALIVE"
    pol = ""
    if vpp >= 0.08 and period_us:
        if 0.6 < period_us < 1.6:
            pol = ("P-convention TRUE (duty~%.0f%%)" % (duty*100)
                   if duty > 0.6 else
                   "INVERTED or N-leg (duty~%.0f%%)" % (duty*100)
                   if duty < 0.4 else
                   "ambiguous duty %.0f%% (16/16 build? probe placement?)"
                   % (duty*100))
        else:
            pol = f"period {period_us:.2f}us not the probe pattern"
    # DC mean is diagnostic gold at the connector: an ENABLED TUSB1046A DP
    # output drives ~1.75 V common mode even with a silent input, so
    # mean~1.8V + tiny Vpp = mux alive, FPGA lane dead; mean~0V = that mux
    # output pair not driving (disabled/unused lane). Use --range 5.
    print(f"{name}: Vpp={vpp*1000:.0f}mV  DC={mean:.2f}V  {verdict}"
          + (f"  period={period_us:.2f}us  {pol}" if period_us else ""))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--rate", type=float, default=50e6)
    ap.add_argument("--range", type=float, default=1.0, dest="vrange")
    ap.add_argument("--ch2", action="store_true")
    a = ap.parse_args()

    dwf = load_dwf()
    hdwf = ctypes.c_int()
    dwf.FDwfDeviceOpen(ctypes.c_int(-1), ctypes.byref(hdwf))
    if hdwf.value == 0:
        sys.exit("no AD device found")
    nch = 2 if a.ch2 else 1
    NBUF = 16384
    for ch in range(nch):
        dwf.FDwfAnalogInChannelEnableSet(hdwf, ctypes.c_int(ch), ctypes.c_int(1))
        dwf.FDwfAnalogInChannelRangeSet(hdwf, ctypes.c_int(ch),
                                        ctypes.c_double(a.vrange))
    dwf.FDwfAnalogInFrequencySet(hdwf, ctypes.c_double(a.rate))
    dwf.FDwfAnalogInBufferSizeSet(hdwf, ctypes.c_int(NBUF))
    time.sleep(0.4)
    dwf.FDwfAnalogInConfigure(hdwf, ctypes.c_int(0), ctypes.c_int(1))
    sts = ctypes.c_ubyte()
    for _ in range(200):
        dwf.FDwfAnalogInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
        if sts.value == 2:                    # done
            break
        time.sleep(0.02)
    buf = (ctypes.c_double * NBUF)()
    for ch in range(nch):
        dwf.FDwfAnalogInStatusData(hdwf, ctypes.c_int(ch), buf,
                                   ctypes.c_int(NBUF))
        analyze(f"CH{ch+1}", list(buf), a.rate)
    dwf.FDwfDeviceCloseAll()

if __name__ == "__main__":
    main()
