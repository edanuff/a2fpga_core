#!/usr/bin/env python3
"""Capture the DP AUX channel with an Analog Discovery 2/3 (WaveForms SDK).

Why the ANALOG channels: AUX rides on DC common modes of ~0.3 V (AUX_P/SBU
side) and ~3.0 V (AUX_N) with <=1.38 Vpp swings — the AD's digital inputs
(threshold ~1.4 V) can read either line as constant. The 14-bit analog
inputs capture the real waveform; thresholding happens in software
(aux_decode.py, adaptive).

Why Record mode: a full attach conversation spans hundreds of ms; the
capture buffer holds only a few ms. Record streams to the host — 4 MS/s
gives 4 samples per 1 us Manchester bit cell, plenty for decode.

Usage:
  ad3_aux_capture.py out.csv [--seconds 5] [--rate 4e6] [--ch2]
                              [--range 5.0]

  --ch2      also record channel 2 (probe AUX_N/SBU2); CSV gains a third
             column and aux_decode.py will decode differentially.
  Channel 1+ probes AUX_P/SBU1 (and its ground to board GND).

Output CSV: time_s, ch1_volts[, ch2_volts] — feed to aux_decode.py.
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

def die(msg):
    sys.exit(f"ad3_aux_capture: {msg}")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("--seconds", type=float, default=5.0)
    ap.add_argument("--rate", type=float, default=4e6)
    ap.add_argument("--ch2", action="store_true")
    ap.add_argument("--range", type=float, default=5.0, dest="vrange")
    a = ap.parse_args()

    dwf = load_dwf()
    hdwf = ctypes.c_int()
    dwf.FDwfDeviceOpen(ctypes.c_int(-1), ctypes.byref(hdwf))
    if hdwf.value == 0:
        err = ctypes.create_string_buffer(512)
        dwf.FDwfGetLastErrorMsg(err)
        die(f"no device: {err.value.decode()}")

    nch = 2 if a.ch2 else 1
    acqmodeRecord = ctypes.c_int(3)
    for ch in range(nch):
        dwf.FDwfAnalogInChannelEnableSet(hdwf, ctypes.c_int(ch),
                                         ctypes.c_int(1))
        dwf.FDwfAnalogInChannelRangeSet(hdwf, ctypes.c_int(ch),
                                        ctypes.c_double(a.vrange))
    dwf.FDwfAnalogInAcquisitionModeSet(hdwf, acqmodeRecord)
    dwf.FDwfAnalogInFrequencySet(hdwf, ctypes.c_double(a.rate))
    dwf.FDwfAnalogInRecordLengthSet(hdwf, ctypes.c_double(a.seconds))
    time.sleep(0.5)                       # offset/range settle
    dwf.FDwfAnalogInConfigure(hdwf, ctypes.c_int(0), ctypes.c_int(1))

    total = int(a.seconds * a.rate)
    bufs = [(ctypes.c_double * 8192)() for _ in range(nch)]
    data = [[] for _ in range(nch)]
    lost = corrupted = 0
    sts = ctypes.c_ubyte()
    got = 0
    print(f"recording {a.seconds}s @ {a.rate/1e6:.1f} MS/s x{nch}ch ...",
          file=sys.stderr)
    while got < total:
        dwf.FDwfAnalogInStatus(hdwf, ctypes.c_int(1), ctypes.byref(sts))
        cAvail = ctypes.c_int(); cLost = ctypes.c_int(); cCorr = ctypes.c_int()
        dwf.FDwfAnalogInStatusRecord(hdwf, ctypes.byref(cAvail),
                                     ctypes.byref(cLost), ctypes.byref(cCorr))
        lost += cLost.value; corrupted += cCorr.value
        got += cLost.value                # keep the timeline honest
        n = cAvail.value
        while n > 0:
            chunk = min(n, 8192)
            for ch in range(nch):
                dwf.FDwfAnalogInStatusData(hdwf, ctypes.c_int(ch),
                                           bufs[ch], ctypes.c_int(chunk))
                data[ch].extend(bufs[ch][:chunk])
            got += chunk
            n -= chunk
        if sts.value == 2 and cAvail.value == 0:   # DwfStateDone
            break
    dwf.FDwfDeviceCloseAll()

    if lost or corrupted:
        print(f"WARNING: lost={lost} corrupted={corrupted} samples "
              f"(reduce --rate if severe)", file=sys.stderr)
    n = len(data[0])
    print(f"writing {n} samples -> {a.out}", file=sys.stderr)
    dt = 1.0 / a.rate
    with open(a.out, "w") as f:
        for i in range(n):
            row = [f"{i*dt:.9f}", f"{data[0][i]:.4f}"]
            if nch == 2 and i < len(data[1]):
                row.append(f"{data[1][i]:.4f}")
            f.write(",".join(row) + "\n")
    print("done. decode with: aux_decode.py", a.out, file=sys.stderr)

if __name__ == "__main__":
    main()
