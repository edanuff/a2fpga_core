#!/usr/bin/env python3
"""Transaction-level annotator for DP AUX captures (AD2/AD3 analog CSV).

Builds on aux_decode2's edge/burst/Manchester machinery and turns raw
byte bursts into a legible protocol log:

  - splits each burst into FRAMES (a request and its reply can share one
    burst group when the sink answers fast) by re-scanning for the
    precharge+sync pattern after each frame's stop;
  - classifies frames as REQUEST (cmd nibble + 20-bit address + length)
    or REPLY (ACK/NACK/DEFER + data), pairs them in order;
  - names DPCD registers (link config, status, IRQ vector, ESI space,
    SET_POWER, HDCP, ...) and I2C targets (0x50 = EDID);
  - prints per-transaction: time, delta, direction, command, register,
    data, reply disposition;
  - ends with the REGISTER FOOTPRINT (every address touched, read/write
    counts) and reply statistics (ACK/DEFER/NACK per target) — the two
    tables that make a known-good source's behavior diffable against
    ours.

Usage: aux_annotate.py capture.csv [--max-tx N] [--selftest]
CSV: time_s, ch1_V[, ch2_V] (same as aux_decode2).
"""
import sys, os, argparse, importlib.util

_here = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location("aux_decode2",
                                               os.path.join(_here, "aux_decode2.py"))
d2 = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(d2)

# --------------------------------------------------------------------------
# DPCD register naming
# --------------------------------------------------------------------------
def dpcd_name(a):
    named = {
        0x000: "DPCD_REV", 0x001: "MAX_LINK_RATE", 0x002: "MAX_LANE_COUNT",
        0x003: "MAX_DOWNSPREAD", 0x004: "NORP", 0x005: "DOWNSTREAMPORT_PRESENT",
        0x006: "MAIN_LINK_CH_CODING", 0x007: "DOWN_STREAM_PORT_COUNT",
        0x008: "RECEIVE_PORT0_CAP_0", 0x00E: "TRAINING_AUX_RD_INTERVAL",
        0x021: "MSTM_CAP",
        0x100: "LINK_BW_SET", 0x101: "LANE_COUNT_SET", 0x102: "TRAINING_PATTERN_SET",
        0x103: "TRAINING_LANE0_SET", 0x104: "TRAINING_LANE1_SET",
        0x105: "TRAINING_LANE2_SET", 0x106: "TRAINING_LANE3_SET",
        0x107: "DOWNSPREAD_CTRL", 0x108: "MAIN_LINK_CH_CODING_SET",
        0x111: "MSTM_CTRL",
        0x200: "SINK_COUNT", 0x201: "DEVICE_SERVICE_IRQ_VECTOR",
        0x202: "LANE0_1_STATUS", 0x203: "LANE2_3_STATUS",
        0x204: "LANE_ALIGN_STATUS_UPDATED", 0x205: "SINK_STATUS",
        0x206: "ADJUST_REQUEST_LANE0_1", 0x207: "ADJUST_REQUEST_LANE2_3",
        0x260: "TEST_RESPONSE", 0x261: "TEST_EDID_CHECKSUM",
        0x600: "SET_POWER",
        0x2002: "SINK_COUNT_ESI", 0x2003: "DEVICE_SERVICE_IRQ_VECTOR_ESI0",
        0x2004: "DEVICE_SERVICE_IRQ_VECTOR_ESI1", 0x2005: "LINK_SERVICE_IRQ_VECTOR_ESI0",
        0x200C: "LANE0_1_STATUS_ESI", 0x200D: "LANE2_3_STATUS_ESI",
        0x200E: "LANE_ALIGN_STATUS_UPDATED_ESI", 0x200F: "SINK_STATUS_ESI",
    }
    if a in named: return named[a]
    if 0x68000 <= a <= 0x68FFF: return "HDCP"
    if 0x00500 <= a <= 0x005FF: return "VENDOR/OUI"
    if 0x02200 <= a <= 0x022FF: return "EXT_RECEIVER_CAP"
    return ""

CMD = {0x8: "NAT_WR", 0x9: "NAT_RD", 0x0: "I2C_WR", 0x1: "I2C_RD",
       0x4: "I2C_WR_MOT", 0x5: "I2C_RD_MOT", 0x2: "I2C_STATUS", 0x6: "I2C_STATUS_MOT"}
REPLY = {0x0: "ACK", 0x1: "NACK", 0x2: "DEFER",
         0x4: "I2C_NACK", 0x8: "I2C_DEFER"}

# --------------------------------------------------------------------------
# multi-frame extraction: re-scan for sync after each frame's stop
# --------------------------------------------------------------------------
def frames_from_bits(bits):
    """Yield byte-lists for every frame in a half-cell bit stream."""
    i = 0
    n = len(bits)
    while i < n:
        # find a sync-end: a run of >=2 None cells after >=4 decodable
        # precharge cells
        run = 0; idx = None; pre = 0
        j = i
        while j < n:
            b = bits[j]
            if b is None:
                run += 1
                if run >= 2 and pre >= 4:
                    idx = j + 1
                    break
            else:
                if run >= 2 and pre >= 4:
                    idx = j
                    break
                if run > 0: pre = 0
                run = 0; pre += 1
            j += 1
        if idx is None: return
        payload = []
        j = idx
        while j < n and bits[j] is not None:
            payload.append(bits[j]); j += 1
        if len(payload) >= 8:
            out = []
            for k in range(len(payload) // 8):
                byte = 0
                for m in range(8):
                    byte = (byte << 1) | payload[k*8 + m]
                out.append(byte)
            yield out
        i = j + 1

def classify(bts):
    """Return ('req', cmd, addr, length, data) or ('rep', code, data)."""
    if not bts: return None
    c = bts[0] >> 4
    if c in CMD and len(bts) >= 3:
        addr = ((bts[0] & 0xF) << 16) | (bts[1] << 8) | bts[2]
        if c in (0x8, 0x0, 0x4):          # writes: header(3)+len+data
            if len(bts) >= 4:
                ln = bts[3] + 1
                return ("req", c, addr, ln, bts[4:4+ln])
        elif c in (0x9, 0x1, 0x5):        # reads: header(3)+len
            ln = bts[3] + 1 if len(bts) >= 4 else 0
            return ("req", c, addr, ln, [])
        elif c in (0x2, 0x6):
            return ("req", c, addr, 0, [])
    code = (bts[0] >> 4) & 0x3
    i2c  = (bts[0] >> 6) & 0x3
    key = code if i2c == 0 else (0x4 if code == 1 else (0x8 if code == 2 else code))
    return ("rep", key, bts[1:])

# --------------------------------------------------------------------------
def annotate(path, max_tx=0):
    txlog = []       # (t, kind-tuple)
    pending = None   # last unanswered request
    footprint = {}   # (rw, addr) -> count
    replies = {}     # (target, disposition) -> count
    n_frames = 0
    for burst in d2.group_bursts(d2.stream_edges(path)):
        h = d2.halfcells(burst)
        best = None
        for pol in (0, 1):
            for phase in (0, 1):
                bits = d2.bits_from_halfcells(h, phase, pol)
                fr = list(frames_from_bits(bits))
                score = sum(len(f) for f in fr)
                if best is None or score > best[0]:
                    best = (score, fr)
        t0 = burst[0][0]
        for f in best[1]:
            n_frames += 1
            c = classify(f)
            if c is None: continue
            txlog.append((t0, c, f))
            if max_tx and len(txlog) >= max_tx * 2:
                break
        if max_tx and len(txlog) >= max_tx * 2:
            break

    # ---- pass 2: pair and print ------------------------------------------
    prev_t = None
    pend = None
    print(f"# {n_frames} frames decoded")
    print(f"{'time_ms':>10} {'d_ms':>8}  line")
    for (t, c, raw) in txlog:
        dt = 0 if prev_t is None else (t - prev_t) * 1e3
        prev_t = t
        if c[0] == "req":
            _, cmd, addr, ln, data = c
            is_i2c = cmd not in (0x8, 0x9)
            tgt = (f"I2C[{addr & 0xFF:02X}]" + (" EDID" if (addr & 0xFF) == 0x50 else "")) \
                  if is_i2c else f"DPCD {addr:05X} {dpcd_name(addr)}"
            rw = "WR" if cmd in (0x8, 0x0, 0x4) else "RD"
            footprint[(rw, tgt)] = footprint.get((rw, tgt), 0) + 1
            ds = (" = " + " ".join(f"{x:02X}" for x in data)) if data else f" len {ln}"
            print(f"{t*1e3:10.3f} {dt:8.3f}  > {CMD[cmd]:12s} {tgt}{ds}")
            pend = ("REQ", rw, tgt)
        else:
            _, code, data = c
            disp = REPLY.get(code, f"?{code:X}")
            tgt = pend[2] if pend else "?"
            replies[(tgt, disp)] = replies.get((tgt, disp), 0) + 1
            ds = (" = " + " ".join(f"{x:02X}" for x in data[:16])) if data else ""
            print(f"{t*1e3:10.3f} {dt:8.3f}  <   {disp:10s} {tgt}{ds}")
            pend = None

    print("\n# ============ REGISTER FOOTPRINT ============")
    for (rw, tgt), cnt in sorted(footprint.items(), key=lambda kv: -kv[1]):
        print(f"  {cnt:5d}x  {rw}  {tgt}")
    print("\n# ============ REPLY DISPOSITIONS ============")
    for (tgt, disp), cnt in sorted(replies.items(), key=lambda kv: -kv[1]):
        print(f"  {cnt:5d}x  {disp:8s} {tgt}")

# --------------------------------------------------------------------------
# self-test: synthesize a capture (Manchester at 1 Mbps, 4 MS/s) with a
# known request+reply and check the annotator reads it back
# --------------------------------------------------------------------------
def _selftest():
    import tempfile
    RATE = 4e6; BIT = 1e-6
    def man_bits(byte_list):
        bits = []
        for b in byte_list:
            for i in range(7, -1, -1):
                bits.append((b >> i) & 1)
        return bits
    def emit(f, t, bits_pre, payload_bytes):
        # precharge: 16 zeros; sync-end: 4 half-cells low then high edge
        # pattern approximated as two constant cells; payload Manchester
        wave = []
        for b in ([0]*bits_pre):
            wave += [0.3, -0.3] if b == 0 else [-0.3, 0.3]
        wave += [0.3, 0.3, -0.3, -0.3]           # sync-end (2 const cells)
        for b in man_bits(payload_bytes):
            wave += [0.3, -0.3] if b == 0 else [-0.3, 0.3]
        wave += [0.3, 0.3, 0.3, 0.3]             # stop-ish
        for i, v in enumerate(wave):
            n = int((BIT/2) * RATE)
            for k in range(n):
                f.write(f"{t + (i*(BIT/2) + k/RATE):.9f},{v:.3f}\n")
        return t + len(wave) * (BIT/2)
    with tempfile.NamedTemporaryFile("w", suffix=".csv", delete=False) as f:
        t = 1e-3
        # native read DPCD 0x00201 len 1  (9 0 2 01 00)
        t = emit(f, t, 16, [0x90, 0x02, 0x01, 0x00])
        t += 30e-6
        # ACK + 0x40
        t = emit(f, t, 16, [0x00, 0x40])
        t += 100e-6
        # native write 0x00201 = 0x40 (80 02 01 00 40)
        t = emit(f, t, 16, [0x80, 0x02, 0x01, 0x00, 0x40])
        t += 30e-6
        t = emit(f, t, 16, [0x00])
        path = f.name
    print("== selftest capture:", path)
    annotate(path)
    print("== selftest done (expect: RD DEVICE_SERVICE_IRQ_VECTOR, ACK 40, "
          "WR DEVICE_SERVICE_IRQ_VECTOR = 40, ACK)")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv", nargs="?")
    ap.add_argument("--max-tx", type=int, default=0)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        _selftest(); return
    if not a.csv:
        ap.error("csv required (or --selftest)")
    annotate(a.csv, a.max_tx)

if __name__ == "__main__":
    main()
