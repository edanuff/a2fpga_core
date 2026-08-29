#!/usr/bin/env python3
"""Compare a DRP register dump (debug-UART 'CR' lines) against the CSR file.

The dp_test bitstream's DRP reader dumps a curated register list over the
debug UART as lines:  CR ii aaaaaa dddddddd W1
Capture them from the telnet console into a text file, then:

  drp_dump_check.py capture.txt [--csr path/to/dp_serdes.csr]

For each dumped register, prints the silicon value next to the last value
the .csr replay should have written (later writes win), and a verdict:
  MATCH     silicon == csr intent (replay landed)
  MISMATCH  silicon != csr intent (replay dropped/overridden — smoking gun)
  NO-WRITE  address is not written by the csr at all (read-only/status reg)
  TIMEOUT   DRP read never returned (0xDEAD_xxxx sentinel)

The point (2026-08-13): FPGA TX pads are silent while all fabric status
reads healthy; this distinguishes "CSR replay never landed" from "replay
landed but the config itself is wrong".
"""
import sys, re, argparse

def parse_csr(path):
    intent = {}
    rx = re.compile(r'upar_write_driver\(0x([0-9a-fA-F]+),0x([0-9a-fA-F]+)\)')
    with open(path) as f:
        for line in f:
            m = rx.search(line)
            if m:
                intent[int(m.group(1), 16)] = int(m.group(2), 16)  # last wins
    return intent

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("capture")
    ap.add_argument("--csr", default="boards/a2mega/hdl/gowin/60B/dp_serdes/dp_serdes.csr")
    a = ap.parse_args()

    intent = parse_csr(a.csr)
    seen = {}
    rx = re.compile(r'CR ([0-9A-F]{2}) ([0-9A-F]{6}) ([0-9A-F]{8})')
    with open(a.capture, errors="replace") as f:
        for line in f:
            m = rx.search(line)
            if m:
                seen[int(m.group(2), 16)] = int(m.group(3), 16)  # latest wins

    if not seen:
        sys.exit("no 'CR ii aaaaaa dddddddd' lines found in capture")

    n_match = n_mis = 0
    for addr in sorted(seen):
        got = seen[addr]
        if (got >> 16) == 0xDEAD:
            verdict = "TIMEOUT (DRP read never returned)"
        elif addr not in intent:
            verdict = "NO-WRITE (not in csr; status/reserved)"
        elif got == intent[addr]:
            verdict = "MATCH"
            n_match += 1
        else:
            verdict = f"MISMATCH csr wrote 0x{intent[addr]:08X}"
            n_mis += 1
        print(f"0x{addr:06X}: silicon=0x{got:08X}  {verdict}")
    print(f"\n{len(seen)} regs dumped: {n_match} match, {n_mis} MISMATCH")

if __name__ == "__main__":
    main()
