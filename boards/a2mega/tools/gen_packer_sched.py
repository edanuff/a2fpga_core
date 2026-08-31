#!/usr/bin/env python3
"""Generate the video_stream_packer per-line symbol-schedule ROM.

The packer's two-slot Bresenham TU walk is line-invariant by construction
(the RTL zeroes tu_pos/tu_valid/bres_err/phase at every line end), so the
per-cycle walk decisions for an ACTIVE line are a pure function of
line_cycle. This script replays the walk exactly as the RTL computes it
(video_stream_packer.v, the always@* "registered early walk") and emits a
hex image consumed by the SCHED_ROM generate branch. Blanking lines are
all-zero decisions — the RTL gates the ROM output with the live
active-line flag instead of storing a second page.

Entry layout (12 bits, one per line_cycle, addr = cycle):
  [0]  px slot 0        [1]  px slot 1
  [3:2]  phase slot 0   [5:4]  phase slot 1
  [6]  fs slot 0        [7]  fs slot 1
  [8]  fe slot 0        [9]  fe slot 1
  [10] fetch (walk part; the prime fetch stays live RTL)
  [11] load0

Usage: gen_packer_sched.py  (writes hdl/displayport/video/<name>.hex)
"""
from pathlib import Path

# ---- geometries ----
CONFIGS = [
    # (name, lanes, h_vis, h_tot, tu, mult, div)
    ("video_sched_1080p_2l", 2, 1920, 2200, 64, 11, 10),  # full core / dp_test
    ("video_sched_720p_2l",  2, 1280, 1650, 64, 11, 12),  # sim harness geometry
]
import sys
for (NAME, LANE_COUNT, H_VISIBLE, H_TOTAL, TU_SIZE,
     PIXEL_CLK_MULT, PIXEL_CLK_DIV) in CONFIGS:

    SYMS_PER_LINE = H_TOTAL * 2 * PIXEL_CLK_DIV // PIXEL_CLK_MULT
    assert H_TOTAL * 2 * PIXEL_CLK_DIV % PIXEL_CLK_MULT == 0
    VALID_NUM = TU_SIZE * 3 * PIXEL_CLK_MULT
    VALID_DEN = 2 * PIXEL_CLK_DIV * LANE_COUNT
    BYTES_PER_LANE = H_VISIBLE * 3 // LANE_COUNT
    DATA_START = TU_SIZE
    CYCLES_PER_LINE = SYMS_PER_LINE // 2

    # BS_POS exactly as calc_bs_pos() in the RTL
    def calc_bs_pos():
        bl, er, pos = BYTES_PER_LANE, 0, DATA_START
        while bl > 0:
            er += VALID_NUM
            v = er // VALID_DEN
            er = er % VALID_DEN
            if v >= bl:
                pos += bl
                bl = 0
            else:
                pos += TU_SIZE
                bl -= v
        return pos

    BS_POS = calc_bs_pos()
    VQUOT = VALID_NUM // VALID_DEN
    VREM = VALID_NUM % VALID_DEN

    # ---- replay the RTL walk for one active line ----
    entries = [0] * (1 << (CYCLES_PER_LINE - 1).bit_length())
    phase, tu_pos, tu_valid, err = 0, 0, 0, 0
    px_count = 0
    for c in range(CYCLES_PER_LINE):
        e = 0
        fetch = 0
        load0 = 0
        for pk in range(2):
            s = 2 * c + pk
            ftu = DATA_START <= s < BS_POS
            fnl = (s != BS_POS - 1)
            if ftu:
                if tu_pos == 0:
                    es = err + VREM
                    if es >= VALID_DEN:
                        err = es - VALID_DEN
                        tu_valid = min(VQUOT + 1, TU_SIZE)
                    else:
                        err = es
                        tu_valid = VQUOT
                if tu_pos < tu_valid:
                    e |= 1 << pk                    # px
                    e |= (phase & 3) << (2 + 2*pk)  # ph
                    px_count += 1
                    if phase == 2:
                        phase = 0
                        if fnl:
                            fetch = 1
                            if pk == 0:
                                load0 = 1
                    else:
                        phase += 1
                elif tu_pos == tu_valid and tu_valid != TU_SIZE:
                    e |= 1 << (6 + pk)              # fs
                elif tu_pos == TU_SIZE - 1 and tu_valid != TU_SIZE:
                    e |= 1 << (8 + pk)              # fe
                tu_pos = 0 if tu_pos == TU_SIZE - 1 else tu_pos + 1
        e |= fetch << 10
        e |= load0 << 11
        entries[c] = e

    # ---- invariant self-checks ----
    assert px_count == BYTES_PER_LANE, (px_count, BYTES_PER_LANE)
    assert phase == 0, phase                       # phase returns each line
    assert BS_POS < SYMS_PER_LINE - 2, BS_POS      # BS fits before line end
    print(f"SYMS_PER_LINE={SYMS_PER_LINE} CYCLES={CYCLES_PER_LINE} "
          f"VALID={VALID_NUM}/{VALID_DEN} (={VQUOT} rem {VREM}) "
          f"BS_POS={BS_POS} px/line={px_count} rom_depth={len(entries)}")

    out = Path(__file__).resolve().parents[3] / "hdl/displayport/video" / f"{NAME}.hex"
    with out.open("w") as f:
        for e in entries:
            f.write(f"{e:03x}\n")
    print(f"{out} written")
