#!/usr/bin/env python3
"""IIgs CPU-socket listen probe with an Analog Discovery 2/3 (digital inputs).

Wire the ribbon's IDC socket (socket end still in the GS, card end
unplugged) to the AD3 DIO pins, GND to IDC pin 40 (VSS), e.g.
    DIO0 = IDC 8 (PHI2)   DIO1 = IDC 7 (/IRQ)   DIO2 = IDC 3 (RDY)   DIO3 = IDC 10 (BE)
Captures DIO0-3 at 100 MS/s and reports per line: level / falling-edge
count / frequency / high duty.  A live IIgs socket PHI2 is ~1.02 MHz in
slow mode (2 ticks low / 12 high of 14.318 MHz) or ~2.86 MHz in fast mode.

Power note: on a weak USB port the AD3 opens fine but reports "The analog
circuit of the device is turned off ... needs at least 3W/600mA" and no
acquisition ever leaves the Ready state.  This script prints that message
and exits; use a direct port or the aux supply.

Usage: ad3_gs_socket_probe.py [--ms 0.16] [--rate 100e6] [--repeat N] [--names "PHI2,IRQ,RDY,BE"]
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

def analyze(bit, samples, rate):
    n = len(samples)
    ones = sum(1 for s in samples if (s >> bit) & 1)
    falls = 0
    prev = (samples[0] >> bit) & 1
    first = last = None
    for i in range(1, n):
        cur = (samples[i] >> bit) & 1
        if prev == 1 and cur == 0:
            falls += 1
            if first is None:
                first = i
            last = i
        prev = cur
    duty = ones / n
    freq = (falls - 1) * rate / (last - first) if falls >= 2 else None
    if falls == 0:
        level = "HIGH" if ones == n else ("LOW" if ones == 0 else "mixed")
    else:
        level = "toggling"
    return level, falls, freq, duty

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--ms", type=float, default=0.16, help="capture length in ms (buffer-limited)")
    ap.add_argument("--rate", type=float, default=100e6)
    ap.add_argument("--repeat", type=int, default=1)
    ap.add_argument("--names", default="DIO0,DIO1,DIO2,DIO3")
    a = ap.parse_args()
    names = a.names.split(",")

    dwf = load_dwf()
    hdwf = ctypes.c_int()
    ndev = ctypes.c_int()
    dwf.FDwfEnum(ctypes.c_int(0), ctypes.byref(ndev))
    if ndev.value == 0:
        print("no Digilent device found"); sys.exit(2)
    dwf.FDwfDeviceOpen(ctypes.c_int(-1), ctypes.byref(hdwf))
    err = ctypes.create_string_buffer(512); dwf.FDwfGetLastErrorMsg(err)
    msg = err.value.decode().strip()
    if hdwf.value == 0:
        print("open failed:", msg); sys.exit(2)
    if msg:
        print("device warning:", msg.splitlines()[0])
        if "turned off" in msg or "600mA" in msg:
            print("AD3 is under-powered: use a direct USB port or the aux supply."); dwf.FDwfDeviceCloseAll(); sys.exit(4)

    try:
        base = ctypes.c_double()
        dwf.FDwfDigitalInInternalClockInfo(hdwf, ctypes.byref(base))
        div = max(1, int(round(base.value / a.rate)))
        rate = base.value / div
        maxbuf = ctypes.c_int()
        dwf.FDwfDigitalInBufferSizeInfo(hdwf, ctypes.byref(maxbuf))
        nsamp = min(int(a.ms * 1e-3 * rate), maxbuf.value)
        dwf.FDwfDigitalInDividerSet(hdwf, ctypes.c_int(div))
        dwf.FDwfDigitalInSampleFormatSet(hdwf, ctypes.c_int(16))
        dwf.FDwfDigitalInBufferSizeSet(hdwf, ctypes.c_int(nsamp))
        dwf.FDwfDigitalInTriggerSourceSet(hdwf, ctypes.c_ubyte(0))
        print("AD3 digital in: %.1f MS/s, %d samples (%.0f us)" % (rate / 1e6, nsamp, nsamp / rate * 1e6))
        for rep in range(a.repeat):
            dwf.FDwfDigitalInConfigure(hdwf, ctypes.c_int(1), ctypes.c_int(1))
            sts = ctypes.c_ubyte()
            t0 = time.time()
            while True:
                dwf.FDwfDigitalInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
                if sts.value == 2:
                    break
                if time.time() - t0 > 5:
                    print("capture timeout (state %d)" % sts.value); sys.exit(3)
                time.sleep(0.01)
            buf = (ctypes.c_uint16 * nsamp)()
            dwf.FDwfDigitalInStatusData(hdwf, buf, ctypes.c_int(2 * nsamp))
            samples = list(buf)
            print("--- capture %d" % (rep + 1))
            for bit in range(4):
                nm = names[bit] if bit < len(names) else "DIO%d" % bit
                level, falls, freq, duty = analyze(bit, samples, rate)
                if freq:
                    print("  DIO%d %-5s %-8s  %6d falls  %8.3f MHz  high %.1f%%" % (bit, nm, level, falls, freq / 1e6, duty * 100))
                else:
                    print("  DIO%d %-5s %-8s  %6d falls  high %.1f%%" % (bit, nm, level, falls, duty * 100))
            # Phase of every other line's low pulses relative to DIO0's falling edge
            # (DIO0 = PHI2): where in the cycle does the FPI pulse it, and how wide.
            ref_falls = [i for i in range(1, nsamp) if ((samples[i-1] >> 0) & 1) and not ((samples[i] >> 0) & 1)]
            for bit in range(1, 4):
                falls = [i for i in range(1, nsamp) if ((samples[i-1] >> bit) & 1) and not ((samples[i] >> bit) & 1)]
                rises = [i for i in range(1, nsamp) if not ((samples[i-1] >> bit) & 1) and ((samples[i] >> bit) & 1)]
                if len(falls) < 3 or len(ref_falls) < 3:
                    continue
                widths = []
                for f in falls:
                    r = next((x for x in rises if x > f), None)
                    if r: widths.append((r - f) / rate * 1e9)
                delays = []
                for f in falls:
                    prev = max((x for x in ref_falls if x <= f), default=None)
                    if prev is not None: delays.append((f - prev) / rate * 1e9)
                nm = names[bit] if bit < len(names) else "DIO%d" % bit
                if widths and delays:
                    print("  %-5s low pulse %.0f-%.0f ns (mean %.0f); falls %.0f-%.0f ns after the PHI2 fall (mean %.0f)" % (
                        nm, min(widths), max(widths), sum(widths)/len(widths), min(delays), max(delays), sum(delays)/len(delays)))
            if rep + 1 < a.repeat:
                time.sleep(0.5)
    finally:
        dwf.FDwfDeviceCloseAll()

if __name__ == "__main__":
    main()
