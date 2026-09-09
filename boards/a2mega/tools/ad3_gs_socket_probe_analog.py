#!/usr/bin/env python3
"""IIgs CPU-socket listen probe with an Analog Discovery 2/3 — analog inputs.

Wire the ribbon's IDC socket (socket end still in the GS, card end
unplugged) to the AD3 scope inputs, minus leads to IDC pin 40 (VSS):
    1+ = the line under test (e.g. IDC 8, PHI2)   2+ = a second line (e.g. IDC 7)
Captures both channels at 50 MS/s (16k samples = 328 us, ~330 PHI2 cycles)
and reports per channel: DC level, Vpp, edge count, frequency and high
duty against a 2.5 V threshold.  A live IIgs socket PHI2 is ~1.02 MHz in
slow mode (2 ticks low / 12 high of 14.318 MHz) or ~2.86 MHz in fast
mode (2 low / 3 high).

(The AD3's digital-in never left the Ready state under dwf 3.25.1 on
macOS, so this uses the analog path the DP lane probe already proved.)

Usage: ad3_gs_socket_probe.py [--rate 50e6] [--repeat N] [--names "PHI2,IRQ"]
"""
import sys, argparse, ctypes, time

def load_dwf():
    import platform
    if platform.system() == "Darwin":
        for cand in ("/Library/Frameworks/dwf.framework/dwf",
                     "/Applications/WaveForms.app/Contents/Frameworks/dwf.framework/Versions/A/dwf"):
            try:
                return ctypes.cdll.LoadLibrary(cand)
            except OSError:
                continue
        raise OSError("dwf library not found — install WaveForms")
    return ctypes.cdll.LoadLibrary("libdwf.so")

NBUF = 16384
THRESH = 2.5

def analyze(name, vs, rate):
    n = len(vs)
    vmin, vmax = min(vs), max(vs)
    mean = sum(vs) / n
    hi = [v > THRESH for v in vs]
    duty = sum(hi) / n
    falls = 0
    first = last = None
    for i in range(1, n):
        if hi[i - 1] and not hi[i]:
            falls += 1
            if first is None:
                first = i
            last = i
    freq = (falls - 1) * rate / (last - first) if falls >= 2 else None
    if falls == 0:
        level = "HIGH" if duty > 0.99 else ("LOW" if duty < 0.01 else "mixed")
    else:
        level = "toggling"
    s = "%-6s DC=%.2fV  min=%.2f max=%.2f Vpp=%.2f  %-8s  %5d falls" % (name, mean, vmin, vmax, vmax - vmin, level, falls)
    if freq:
        s += "  %.3f MHz  high %.1f%%" % (freq / 1e6, duty * 100)
    return s

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--rate", type=float, default=50e6)
    ap.add_argument("--repeat", type=int, default=1)
    ap.add_argument("--names", default="CH1,CH2")
    a = ap.parse_args()
    names = a.names.split(",")

    dwf = load_dwf()
    hdwf = ctypes.c_int()
    dwf.FDwfDeviceOpen(ctypes.c_int(-1), ctypes.byref(hdwf))
    if hdwf.value == 0:
        err = ctypes.create_string_buffer(512); dwf.FDwfGetLastErrorMsg(err)
        print("open failed:", err.value.decode()); sys.exit(2)
    try:
        for ch in (0, 1):
            dwf.FDwfAnalogInChannelEnableSet(hdwf, ctypes.c_int(ch), ctypes.c_int(1))
            # 10 V range (the AD3 picks its high range): 0-5 V logic fits with no
            # offset; setting an offset made the acquisition never complete.
            dwf.FDwfAnalogInChannelRangeSet(hdwf, ctypes.c_int(ch), ctypes.c_double(10.0))
        dwf.FDwfAnalogInFrequencySet(hdwf, ctypes.c_double(a.rate))
        dwf.FDwfAnalogInBufferSizeSet(hdwf, ctypes.c_int(NBUF))
        rate = ctypes.c_double(); dwf.FDwfAnalogInFrequencyGet(hdwf, ctypes.byref(rate))
        print("AD3 analog in: %.1f MS/s, %d samples (%.0f us), threshold %.1f V" % (rate.value / 1e6, NBUF, NBUF / rate.value * 1e6, THRESH))
        time.sleep(0.5)
        for rep in range(a.repeat):
            dwf.FDwfAnalogInConfigure(hdwf, ctypes.c_int(0), ctypes.c_int(1))
            sts = ctypes.c_ubyte()
            t0 = time.time()
            while True:
                dwf.FDwfAnalogInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
                if sts.value == 2:
                    break
                if time.time() - t0 > 5:
                    print("capture timeout"); sys.exit(3)
                time.sleep(0.01)
            print("--- capture %d" % (rep + 1))
            for ch in (0, 1):
                buf = (ctypes.c_double * NBUF)()
                dwf.FDwfAnalogInStatusData(hdwf, ctypes.c_int(ch), buf, ctypes.c_int(NBUF))
                print("  " + analyze(names[ch] if ch < len(names) else "CH%d" % (ch + 1), list(buf), rate.value))
            if rep + 1 < a.repeat:
                time.sleep(0.5)
    finally:
        dwf.FDwfDeviceCloseAll()

if __name__ == "__main__":
    main()
