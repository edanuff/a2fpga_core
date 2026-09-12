#!/usr/bin/env python3
"""Long single-shot record of the IIgs /RESET line (or any slow line) with an
Analog Discovery 3 — bench tool for the a2mega GS-socket power-on work.

    ad3_reset_record.py levels                      # DC level of both scope channels now
    ad3_reset_record.py record --seconds 8 [--rate 1e6] [--ch 1] [--out reset.bin]

record: streams CH<ch> in Record mode for --seconds at --rate (default 1 MS/s,
so a 40 us probe window is 40 samples), then prints every level transition
(thresholds 0.8 V / 2.0 V with hysteresis) as time-from-first-rise, level and
duration, plus min/max/mean, and saves the raw float32 samples to --out.
Start it, then power the machine on: the record covers the whole ramp,
the 2G06 hold, the machine's own release and anything after it.
"""
import sys, argparse, ctypes, time, struct

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

acqmodeRecord = ctypes.c_int(3)
DwfStateDone = 2

def open_dev(dwf):
    hdwf = ctypes.c_int()
    dwf.FDwfDeviceOpen(ctypes.c_int(-1), ctypes.byref(hdwf))
    if hdwf.value == 0:
        err = ctypes.create_string_buffer(512)
        dwf.FDwfGetLastErrorMsg(err)
        sys.exit("AD3 open failed: %s" % err.value.decode(errors="replace"))
    return hdwf

def cmd_levels(dwf, args):
    hdwf = open_dev(dwf)
    for ch in (0, 1):
        dwf.FDwfAnalogInChannelEnableSet(hdwf, ctypes.c_int(ch), ctypes.c_int(1))
        dwf.FDwfAnalogInChannelRangeSet(hdwf, ctypes.c_int(ch), ctypes.c_double(10.0))
    dwf.FDwfAnalogInFrequencySet(hdwf, ctypes.c_double(1e6))
    dwf.FDwfAnalogInBufferSizeSet(hdwf, ctypes.c_int(8192))
    dwf.FDwfAnalogInConfigure(hdwf, ctypes.c_int(1), ctypes.c_int(1))
    sts = ctypes.c_byte()
    for _ in range(200):
        dwf.FDwfAnalogInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
        if sts.value == DwfStateDone:
            break
        time.sleep(0.01)
    buf = (ctypes.c_double * 8192)()
    for ch in (0, 1):
        dwf.FDwfAnalogInStatusData(hdwf, ctypes.c_int(ch), buf, ctypes.c_int(8192))
        vs = list(buf)
        print("CH%d: mean %.3f V  min %.3f  max %.3f" % (ch + 1, sum(vs) / len(vs), min(vs), max(vs)))
    dwf.FDwfDeviceCloseAll()

def cmd_record(dwf, args):
    hdwf = open_dev(dwf)
    ch = args.ch - 1
    rate = float(args.rate)
    n_total = int(args.seconds * rate)
    dwf.FDwfAnalogInChannelEnableSet(hdwf, ctypes.c_int(ch), ctypes.c_int(1))
    dwf.FDwfAnalogInChannelRangeSet(hdwf, ctypes.c_int(ch), ctypes.c_double(10.0))
    dwf.FDwfAnalogInAcquisitionModeSet(hdwf, acqmodeRecord)
    dwf.FDwfAnalogInFrequencySet(hdwf, ctypes.c_double(rate))
    dwf.FDwfAnalogInRecordLengthSet(hdwf, ctypes.c_double(args.seconds))
    time.sleep(1.0)   # let the offsets settle
    dwf.FDwfAnalogInConfigure(hdwf, ctypes.c_int(0), ctypes.c_int(1))
    print("recording CH%d for %.1f s at %.0f S/s — power the machine on now" % (args.ch, args.seconds, rate), flush=True)
    sts = ctypes.c_byte()
    avail = ctypes.c_int(); lost = ctypes.c_int(); corrupt = ctypes.c_int()
    chunk = (ctypes.c_double * 1000000)()
    samples = []
    n_lost = n_corrupt = 0
    t0 = time.time()
    while len(samples) < n_total:
        dwf.FDwfAnalogInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
        dwf.FDwfAnalogInStatusRecord(hdwf, ctypes.byref(avail), ctypes.byref(lost), ctypes.byref(corrupt))
        n_lost += lost.value; n_corrupt += corrupt.value
        if avail.value > 0:
            n = min(avail.value, 1000000)
            dwf.FDwfAnalogInStatusData(hdwf, ctypes.c_int(ch), chunk, ctypes.c_int(n))
            samples.extend(chunk[:n])
        elif sts.value == DwfStateDone:
            break
        else:
            time.sleep(0.002)
        if time.time() - t0 > args.seconds + 15:
            print("timeout waiting for samples (got %d)" % len(samples)); break
    dwf.FDwfDeviceCloseAll()
    print("got %d samples (%.2f s), lost %d, corrupt %d" % (len(samples), len(samples) / rate, n_lost, n_corrupt))
    if args.out:
        with open(args.out, "wb") as f:
            f.write(struct.pack("<%df" % len(samples), *samples))
        print("raw saved to", args.out)
    analyze(samples, rate)

def analyze(vs, rate):
    n = len(vs)
    if n == 0:
        return
    print("min %.3f  max %.3f  mean %.3f V" % (min(vs), max(vs), sum(vs) / n))
    HI, LO = 2.0, 0.8
    level = 1 if vs[0] > HI else 0
    edges = []   # (index, new_level)
    for i in range(1, n):
        v = vs[i]
        if level == 0 and v > HI:
            level = 1; edges.append((i, 1))
        elif level == 1 and v < LO:
            level = 0; edges.append((i, 0))
    print("%d transitions (0.8/2.0 V hysteresis); initial level %s" % (len(edges), "HIGH" if (vs[0] > HI) else "LOW"))
    first_rise = next((i for i, l in edges if l == 1), None)
    ref = first_rise if first_rise is not None else 0
    print("t = ms from the first rise (index %s); per edge: time, new level, and how long the previous level lasted" % ref)
    prev_i = 0
    shown = 0
    for i, l in edges:
        if shown < 400:
            # local level detail: max/min of the 200 us after the edge
            seg = vs[i:i + int(200e-6 * rate)]
            print("  %10.3f ms  -> %s  (prev level %.3f ms)  next 200us: min %.2f max %.2f" %
                  ((i - ref) * 1000.0 / rate, "HIGH" if l else "LOW", (i - prev_i) * 1000.0 / rate,
                   min(seg) if seg else 0, max(seg) if seg else 0))
        prev_i = i; shown += 1
    if shown > 400:
        print("  ... %d more" % (shown - 400))
    # final level and time held
    print("final level %s for the last %.3f ms" % ("HIGH" if level else "LOW", (n - prev_i) * 1000.0 / rate))
    # rise time of the first rise (10%-90% of 5 V)
    if first_rise is not None:
        i0 = max(first_rise - int(50e-6 * rate), 0)
        seg = vs[i0:first_rise + int(50e-6 * rate)]
        t10 = next((k for k, v in enumerate(seg) if v > 0.5), None)
        t90 = next((k for k, v in enumerate(seg) if v > 4.5), None)
        if t10 is not None and t90 is not None:
            print("first rise 0.5->4.5 V: %.1f us" % ((t90 - t10) * 1e6 / rate))


def cmd_drecord(dwf, args):
    """DigitalIn record of one DIO bit: run-length list of levels (1 MS/s default)."""
    import numpy as np
    hdwf = open_dev(dwf)
    rate = float(args.rate)
    div = int(round(100e6 / rate))
    n_total = int(args.seconds * rate)
    dwf.FDwfDigitalInDividerSet(hdwf, ctypes.c_int(div))
    dwf.FDwfDigitalInSampleFormatSet(hdwf, ctypes.c_int(16))
    dwf.FDwfDigitalInAcquisitionModeSet(hdwf, acqmodeRecord)
    dwf.FDwfDigitalInTriggerPositionSet(hdwf, ctypes.c_int(n_total))
    dwf.FDwfDigitalInConfigure(hdwf, ctypes.c_int(0), ctypes.c_int(1))
    print("recording DIO%d for %.1f s at %.0f S/s - cycle the machine now" % (args.dio, args.seconds, rate), flush=True)
    sts = ctypes.c_byte(); avail = ctypes.c_int(); lost = ctypes.c_int(); corrupt = ctypes.c_int()
    buf = (ctypes.c_uint16 * 1000000)()
    runs = []          # (level, length) run-length encoded
    got = 0; n_lost = n_corrupt = 0; t0 = time.time()
    while got < n_total and time.time() - t0 < args.seconds + 20:
        dwf.FDwfDigitalInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
        dwf.FDwfDigitalInStatusRecord(hdwf, ctypes.byref(avail), ctypes.byref(lost), ctypes.byref(corrupt))
        n_lost += lost.value; n_corrupt += corrupt.value
        if avail.value > 0:
            n = min(avail.value, 1000000)
            dwf.FDwfDigitalInStatusData(hdwf, buf, ctypes.c_int(2 * n))
            bits = (np.frombuffer(bytes(bytearray(buf)[:2 * n]), dtype=np.uint16) >> args.dio) & 1
            # RLE this chunk and merge with the previous run
            change = np.flatnonzero(np.diff(bits)) + 1
            starts = np.concatenate(([0], change)); ends = np.concatenate((change, [n]))
            for s, e in zip(starts, ends):
                lvl = int(bits[s]); ln = int(e - s)
                if runs and runs[-1][0] == lvl: runs[-1] = (lvl, runs[-1][1] + ln)
                else: runs.append((lvl, ln))
            got += n
        elif sts.value == DwfStateDone:
            break
        else:
            time.sleep(0.002)
    dwf.FDwfDeviceCloseAll()
    print("got %d samples (%.2f s), lost %d, corrupt %d, %d runs" % (got, got / rate, n_lost, n_corrupt, len(runs)))
    # print runs with absolute time; suppress runs shorter than --min-us unless --all
    t = 0
    first_rise = None
    acc = 0
    for lvl, ln in runs:
        if lvl == 1 and first_rise is None and acc > 0: first_rise = acc
        acc += ln
    ref = first_rise or 0
    print("t = ms from the first rise after start (or from start); runs shorter than %.0f us are marked *" % args.min_us)
    shown = 0
    for lvl, ln in runs:
        dur_us = ln * 1e6 / rate
        if dur_us >= args.min_us or args.all:
            if shown < 500:
                print("  %10.3f ms  %-4s for %12.3f ms%s" % ((t - ref) * 1000.0 / rate, "HIGH" if lvl else "LOW", dur_us / 1000.0, "" if dur_us >= args.min_us else " *"))
            shown += 1
        t += ln
    if shown >= 500: print("  ... (%d runs shown of %d)" % (500, shown))
    short = sum(1 for lvl, ln in runs if ln * 1e6 / rate < args.min_us)
    print("%d runs shorter than %.0f us not shown (use --all)" % (short, args.min_us) if not args.all else "")

def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("levels")
    r = sub.add_parser("record")
    r.add_argument("--seconds", type=float, default=8.0)
    r.add_argument("--rate", type=float, default=1e6)
    r.add_argument("--ch", type=int, default=1)
    r.add_argument("--out", default="")
    d = sub.add_parser("drecord")
    d.add_argument("--seconds", type=float, default=30.0)
    d.add_argument("--rate", type=float, default=1e6)
    d.add_argument("--dio", type=int, default=0)
    d.add_argument("--min-us", type=float, default=5.0)
    d.add_argument("--all", action="store_true")
    args = ap.parse_args()
    dwf = load_dwf()
    if args.cmd == "levels":
        cmd_levels(dwf, args)
    elif args.cmd == "drecord":
        cmd_drecord(dwf, args)
    else:
        cmd_record(dwf, args)

if __name__ == "__main__":
    main()
