#!/usr/bin/env python3
"""Host side of the accelerator-only card's trace build (hdl/twgs/gs_accel_trace.sv;
docs/gs_socket_s7_accel_only_build.md, section 3n): sets the ring up, reads it out
through the firmware's telnet command line, and lists what it holds.

  gs_trace.py --host <card> status
  gs_trace.py --host <card> arm [--keep all|op|pull|flow] [--trig op|pull6|quiet|any]
                                [--addr BB:AAAA] [--post 32|1024|4096|max] [--when-speed HH]
  gs_trace.py --host <card> freeze | run
  gs_trace.py --host <card> dump [-o file] [--last N]      (freezes the ring; N = newest entries)
  gs_trace.py list <file> [--raw]                           (a dump as instructions)

arm --when-speed HH stages the setup with the trigger off, waits until the machine's speed
register (CYAREG, from `gs accel`) has been something else and then reads HH, arms the
trigger with one register write, and waits for it to fire: for a trigger that a program's
loader would fire, armed when the program itself starts.

The ring's setup goes through window registers 29 to 31 under the keys 50 to 52, where
the accelerator's requests live under the key A5: the tool refuses unless the card is
in mode 00 (the accelerator not asked for), and it never sends a bare key to the telnet
console (g there reloads the FPGA).  The host may also come from GS_TRACE_HOST.
"""
import argparse, os, re, socket, sys, time

KEEP = {"all": 0, "op": 1, "pull": 2, "flow": 3}
TRIG = {"op": 0, "pull6": 1, "quiet": 2, "any": 3}
POST = {"32": 0, "1024": 1, "4096": 2, "max": 3}
RING = 8192

class Card:
    def __init__(self, host, port=23):
        self.s = socket.create_connection((host, port), timeout=5)
        self.s.settimeout(0.3)
        self._until(None, 1.5)
        self.s.sendall(b":")
        if not self._until(b": ", 3).endswith(b": "):
            raise SystemExit("no command prompt from the card: nothing sent")

    @staticmethod
    def _strip(b):
        out, i = bytearray(), 0
        while i < len(b):
            if b[i] == 255:
                i += 3 if i + 1 < len(b) and 251 <= b[i + 1] <= 254 else 2
            else:
                out.append(b[i]); i += 1
        return bytes(out)

    def _until(self, tok, timeout):
        buf, end = b"", time.time() + timeout
        while time.time() < end:
            try:
                d = self.s.recv(8192)
                if not d:
                    break
                buf += d
                if tok and self._strip(buf).endswith(tok):
                    break
            except socket.timeout:
                pass
        return self._strip(buf)

    def cmd(self, line, timeout=15):
        self.s.sendall(line.encode() + b"\r")
        out = self._until(b"\r\n: ", timeout)
        if not out.endswith(b": "):
            raise SystemExit(f"no prompt after '{line}': stopping")
        return out.decode("latin1").replace("\r", "")

    def close(self):
        self.s.close()

LINE = re.compile(r"^(\d\d)  ([0-9A-F]{2}):([0-9A-F]{4})   ([0-9A-F]{2})   ([RW]) (OP |dat|opr|int)( STALL)?( BE0)?")

def read_window(card):
    """One `gs trace`: {index: (word, flags text)}; freezes the ring."""
    rows = {}
    for l in card.cmd("gs trace", 30).split("\n"):
        m = LINE.match(l)
        if m:
            rows[int(m.group(1))] = (int(m.group(2) + m.group(3) + m.group(4), 16),
                                     (m.group(5), m.group(6).strip(), bool(m.group(7)), bool(m.group(8))))
    if len(rows) != 64:
        raise SystemExit(f"gs trace gave {len(rows)} lines, not 64")
    return rows

def state(rows):
    s0, s1, s2 = rows[15][0], rows[16][0], rows[17][0]
    return {"frozen": bool(s0 >> 31), "triggered": bool((s0 >> 30) & 1), "round": bool((s0 >> 29) & 1),
            "cfg": (s0 >> 16) & 0xFF, "wptr": s0 & 0xFFFF, "page": s1 >> 24, "taddr": s1 & 0xFFFFFF,
            "kept": s2, "mode": rows[6][0] & 0xFF}

def show(st):
    c = st["cfg"]
    keep = [k for k, v in KEEP.items() if v == (c & 3)][0]
    trig = [k for k, v in TRIG.items() if v == ((c >> 4) & 3)][0]
    post = [k for k, v in POST.items() if v == (c >> 6)][0]
    print(f"ring: {'stands still' if st['frozen'] else 'running'}{', TRIGGERED' if st['triggered'] else ''}"
          f"{', gone round' if st['round'] else ''}; write pointer {st['wptr']}; {st['kept']} kept since the clear")
    print(f"setup {c:02X}: keeps {keep}; trigger {'armed' if c & 8 else 'off'}: {trig} at "
          f"{st['taddr'] >> 16:02X}:{st['taddr'] & 0xFFFF:04X}, {post} after; clear {'ON' if c & 4 else 'off'}; "
          f"page {st['page']}; card mode {st['mode']:02X}")

def need_mode00(st, force):
    if st["mode"] != 0 and not force:
        raise SystemExit(f"card mode is {st['mode']:02X}, not 00: registers 29-31 hold the accelerator's requests. "
                         "Not touched (--force to do it anyway).")

def slot(card, n, v):
    for line in ("gs set 31 0", f"gs set 29 {v & 0xFF}", f"gs set 30 {(v >> 8) & 0xFF}", f"gs set 31 {0x50 + n}"):
        card.cmd(line)
    time.sleep(0.05)

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--host", default=os.environ.get("GS_TRACE_HOST"))
    ap.add_argument("--force", action="store_true")
    sub = ap.add_subparsers(dest="what", required=True)
    sub.add_parser("status"); sub.add_parser("freeze"); sub.add_parser("run")
    a = sub.add_parser("arm")
    a.add_argument("--keep", choices=KEEP, default="all")
    a.add_argument("--trig", choices=TRIG)
    a.add_argument("--addr", default="00:0000")
    a.add_argument("--post", choices=POST, default="1024")
    a.add_argument("--when-speed", help="arm the trigger when the speed register reads this (hex), after it has read otherwise")
    a.add_argument("--again", type=int, default=0, help="with --when-speed: when the trigger has fired, arm it again at once, this many times (the access after the first)")
    a.add_argument("--wait", type=int, default=900, help="seconds to wait for --when-speed and then for the trigger")
    d = sub.add_parser("dump"); d.add_argument("-o", "--out"); d.add_argument("--last", type=int)
    l = sub.add_parser("list"); l.add_argument("file"); l.add_argument("--raw", action="store_true")
    args = ap.parse_args()

    if args.what == "list":
        return listing(args.file, args.raw)
    if not args.host:
        raise SystemExit("--host (or GS_TRACE_HOST) is needed")
    card = Card(args.host)
    try:
        if args.what == "freeze":
            print(card.cmd("gs freeze").strip().split("\n")[-2]); return
        if args.what == "run":
            print(card.cmd("gs run").strip().split("\n")[-2]); return
        if args.what == "status":
            # (`gs trace` freezes: put the ring back as it was)
            was = "FROZEN" in card.cmd("gs")
            st = state(read_window(card))
            if not was:
                card.cmd("gs run"); st["frozen"] = False
            show(st); return
        if args.what == "arm":
            was = "FROZEN" in card.cmd("gs")
            st = state(read_window(card)); need_mode00(st, args.force)
            bank, addr = (int(x, 16) for x in args.addr.split(":"))
            cfg = KEEP[args.keep] | (POST[args.post] << 6) | ((TRIG[args.trig] << 4) | 8 if args.trig else 0)
            if args.when_speed:
                return arm_when(card, args, cfg, bank, addr)
            slot(card, 0, addr)
            slot(card, 1, ((cfg | 4) << 8) | bank)
            slot(card, 1, (cfg << 8) | bank)
            card.cmd("gs set 31 0")
            card.cmd("gs run")
            st = state(read_window(card)); card.cmd("gs run"); st["frozen"] = False
            show(st)
            if st["cfg"] != cfg or st["taddr"] != (bank << 16 | addr):
                raise SystemExit("the card did not take the setup")
            return
        if args.what == "dump":
            rows = read_window(card); st = state(rows); need_mode00(st, args.force); show(st)
            valid = RING if st["round"] else st["wptr"]
            first = 0 if not args.last else max(0, valid - args.last)
            pages = range(first // 32, (valid + 31) // 32)
            out = open(args.out, "w") if args.out else sys.stdout
            print(f"# gs_trace dump {time.strftime('%Y-%m-%d %H:%M:%S')} valid {valid} cfg {st['cfg']:02X} "
                  f"triggered {int(st['triggered'])} kept {st['kept']} entries {pages.start * 32}..{min(valid, pages.stop * 32) - 1}",
                  file=out)
            card.cmd("gs set 31 0"); card.cmd("gs set 30 0"); card.cmd("gs set 29 0"); card.cmd(f"gs set 31 {0x52}")
            t0 = time.time()
            for p in pages:
                for attempt in range(3):
                    card.cmd(f"gs set 29 {p}")
                    rows = read_window(card)
                    if state(rows)["page"] == p:
                        break
                else:
                    raise SystemExit(f"page {p} did not load")
                for i in range(32):
                    n = p * 32 + i
                    if n >= valid:
                        break
                    w, (rw, kind, irq_low, pull) = rows[32 + i]
                    print(f"{n:5d} {w >> 24:02X}:{(w >> 8) & 0xFFFF:04X} {w & 0xFF:02X} {rw} {kind:3s}"
                          f"{' VP' if pull else ''}{' irq' if irq_low else ''}", file=out)
                if args.out and (p - pages.start) % 16 == 15:
                    print(f"  page {p - pages.start + 1} of {len(pages)}, {time.time() - t0:.0f} s", file=sys.stderr)
            card.cmd("gs set 31 0")
            if args.out:
                out.close(); print(f"written: {args.out} (the ring still stands still: `run` to start it)")
            return
    finally:
        card.close()

def speed_reg(card):
    m = re.search(r"speed register ([0-9A-F]{2})", card.cmd("gs accel"))
    return int(m.group(1), 16) if m else None

def arm_when(card, args, cfg, bank, addr):
    want = int(args.when_speed, 16)
    slot(card, 0, addr)
    slot(card, 1, ((cfg | 4) << 8) | bank)
    slot(card, 1, ((cfg & ~8) << 8) | bank)          # running, the trigger off
    for line in ("gs set 31 0", f"gs set 29 {bank}", f"gs set 30 {cfg}", "gs run"):
        card.cmd(line)                               # staged: the key alone is left to write
    t0, seen_other, n = time.time(), False, 0
    print(f"staged; waiting for the speed register to leave {want:02X} and come back to it", flush=True)
    while True:
        v = speed_reg(card); n += 1
        if v is not None and v != want and not seen_other:
            seen_other = True; print(f"  {time.time() - t0:6.1f} s: speed register {v:02X}", flush=True)
        if seen_other and v == want:
            card.cmd(f"gs set 31 {0x51}")
            ta = time.time(); print(f"  {ta - t0:6.1f} s: speed register {want:02X}: trigger armed", flush=True)
            break
        if time.time() - t0 > args.wait:
            card.cmd("gs set 31 0"); raise SystemExit("gave up waiting; the trigger is not armed")
    again = args.again
    if not again:
        time.sleep(0.1); card.cmd("gs set 31 0")
    while time.time() - ta < args.wait:
        g = card.cmd("gs")
        if "TRIGGERED" in g and "FROZEN" in g:
            print(f"  {time.time() - t0:6.1f} s: the trigger fired and the ring stands still ({time.time() - ta:.1f} s after arming)", flush=True)
            if again:
                # (the slot's key is still there: the trigger off and on again is two writes)
                card.cmd(f"gs set 30 {cfg & ~8}"); card.cmd(f"gs set 30 {cfg}")
                again -= 1
                print(f"  {time.time() - t0:6.1f} s: armed again", flush=True)
                if not again:
                    time.sleep(0.1); card.cmd("gs set 31 0")
                continue
            show(state(read_window(card))); return
        if not again:
            time.sleep(0.2)
    raise SystemExit("armed, not fired in the time given")

# ---------------------------------------------------------------- listing
M = """BRK s|ORA dxi|COP s|ORA sr|TSB d|ORA d|ASL d|ORA dl|PHP|ORA #|ASL A|PHD|TSB a|ORA a|ASL a|ORA al
BPL r|ORA diy|ORA di|ORA sry|TRB d|ORA dx|ASL dx|ORA dly|CLC|ORA ay|INC A|TCS|TRB a|ORA ax|ASL ax|ORA alx
JSR a|AND dxi|JSL al|AND sr|BIT d|AND d|ROL d|AND dl|PLP|AND #|ROL A|PLD|BIT a|AND a|ROL a|AND al
BMI r|AND diy|AND di|AND sry|BIT dx|AND dx|ROL dx|AND dly|SEC|AND ay|DEC A|TSC|BIT ax|AND ax|ROL ax|AND alx
RTI|EOR dxi|WDM s|EOR sr|MVP mv|EOR d|LSR d|EOR dl|PHA|EOR #|LSR A|PHK|JMP a|EOR a|LSR a|EOR al
BVC r|EOR diy|EOR di|EOR sry|MVN mv|EOR dx|LSR dx|EOR dly|CLI|EOR ay|PHY|TCD|JML al|EOR ax|LSR ax|EOR alx
RTS|ADC dxi|PER rl|ADC sr|STZ d|ADC d|ROR d|ADC dl|PLA|ADC #|ROR A|RTL|JMP ai|ADC a|ROR a|ADC al
BVS r|ADC diy|ADC di|ADC sry|STZ dx|ADC dx|ROR dx|ADC dly|SEI|ADC ay|PLY|TDC|JMP axi|ADC ax|ROR ax|ADC alx
BRA r|STA dxi|BRL rl|STA sr|STY d|STA d|STX d|STA dl|DEY|BIT #|TXA|PHB|STY a|STA a|STX a|STA al
BCC r|STA diy|STA di|STA sry|STY dx|STA dx|STX dy|STA dly|TYA|STA ay|TXS|TXY|STZ a|STA ax|STZ ax|STA alx
LDY #|LDA dxi|LDX #|LDA sr|LDY d|LDA d|LDX d|LDA dl|TAY|LDA #|TAX|PLB|LDY a|LDA a|LDX a|LDA al
BCS r|LDA diy|LDA di|LDA sry|LDY dx|LDA dx|LDX dy|LDA dly|CLV|LDA ay|TSX|TYX|LDY ax|LDA ax|LDX ay|LDA alx
CPY #|CMP dxi|REP #|CMP sr|CPY d|CMP d|DEC d|CMP dl|INY|CMP #|DEX|WAI|CPY a|CMP a|DEC a|CMP al
BNE r|CMP diy|CMP di|CMP sry|PEI di|CMP dx|DEC dx|CMP dly|CLD|CMP ay|PHX|STP|JML ail|CMP ax|DEC ax|CMP alx
CPX #|SBC dxi|SEP #|SBC sr|CPX d|SBC d|INC d|SBC dl|INX|SBC #|NOP|XBA|CPX a|SBC a|INC a|SBC al
BEQ r|SBC diy|SBC di|SBC sry|PEA a|SBC dx|INC dx|SBC dly|SED|SBC ay|PLX|XCE|JSR axi|SBC ax|INC ax|SBC alx"""
OPS = [x.split() for x in M.replace("\n", "|").split("|")]
assert len(OPS) == 256
FMT = {"d": "${0:02X}", "dx": "${0:02X},X", "dy": "${0:02X},Y", "di": "(${0:02X})", "dxi": "(${0:02X},X)",
       "diy": "(${0:02X}),Y", "dl": "[${0:02X}]", "dly": "[${0:02X}],Y", "sr": "${0:02X},S", "sry": "(${0:02X},S),Y",
       "a": "${0:04X}", "ax": "${0:04X},X", "ay": "${0:04X},Y", "ai": "(${0:04X})", "axi": "(${0:04X},X)",
       "ail": "[${0:04X}]", "al": "${0:06X}", "alx": "${0:06X},X", "s": "${0:02X}"}

def dis(pc, op, opr):
    name, mode = OPS[op][0], (OPS[op][1] if len(OPS[op]) > 1 else "")
    v = sum(b << (8 * i) for i, b in enumerate(opr))
    if mode in ("", "A") or not opr:
        return name + (" A" if mode == "A" else "") + ("" if not mode or mode == "A" or opr else "  ; operand not seen")
    if mode == "#":
        return f"{name} #${v:0{2 * len(opr)}X}"
    if mode == "r":
        return f"{name} ${(pc + 2 + (v - 256 if v > 127 else v)) & 0xFFFF:04X}"
    if mode == "rl":
        return f"{name} ${(pc + 3 + (v - 65536 if v > 32767 else v)) & 0xFFFF:04X}"
    if mode == "mv":
        return f"{name} ${opr[-1]:02X},${opr[0]:02X}" if len(opr) > 1 else f"{name} ?"
    return f"{name} {FMT[mode].format(v)}"

def listing(path, raw):
    rows = []
    for l in open(path):
        if l.startswith("#"):
            print(l.rstrip()); continue
        f = l.split()
        rows.append((int(f[0]), int(f[1][:2], 16), int(f[1][3:], 16), int(f[2], 16), f[3], f[4], "VP" in f[5:], "irq" in f[5:]))
    if not raw and not any(r[5] == "opr" for r in rows):
        return listing_by_order(rows)      # (the socket's VPA/VDA do not tell an opcode from data: by address order)
    if raw:
        for r in rows:
            print(f"{r[0]:5d} {r[1]:02X}:{r[2]:04X} {r[3]:02X} {r[4]} {r[5]:3s}{' VP' if r[6] else ''}{' irq' if r[7] else ''}")
        return
    i = 0
    while i < len(rows):
        n, b, a, d, rw, kind, vp, irq = rows[i]
        if kind != "OP":
            what = "VECTOR PULL" if vp else {"dat": "data", "opr": "operand", "int": "internal"}[kind]
            print(f"{n:5d}                               {what:8s} {rw} {b:02X}:{a:04X} = {d:02X}{'   /IRQ low' if irq else ''}")
            i += 1; continue
        opr, j = [], i + 1
        while j < len(rows) and rows[j][5] == "opr" and rows[j][1] == b and rows[j][2] == ((a + 1 + len(opr)) & 0xFFFF) and len(opr) < 3:
            opr.append(rows[j][3]); j += 1
        data = []
        while j < len(rows) and rows[j][5] != "OP":
            r = rows[j]
            if r[6]:
                data.append(f"VP {r[1]:02X}:{r[2]:04X}={r[3]:02X}")
            elif r[5] == "dat":
                data.append(f"{r[4]} {r[1]:02X}:{r[2]:04X}={r[3]:02X}")
            elif r[5] == "opr":
                data.append(f"opr {r[1]:02X}:{r[2]:04X}={r[3]:02X}")
            j += 1
        low = any(r[7] for r in rows[i:j])
        code = " ".join(f"{x:02X}" for x in [d] + opr)
        print(f"{n:5d} {b:02X}:{a:04X}  {code:11s}  {dis(a, d, opr):22s} {'  '.join(data)}{'   /IRQ low' if low else ''}")
        i = j

A_OPS = {"ORA", "AND", "EOR", "ADC", "STA", "LDA", "CMP", "SBC", "BIT", "STZ", "INC", "DEC", "ASL", "LSR", "ROL", "ROR", "TSB", "TRB"}
X_OPS = {"LDX", "LDY", "STX", "STY", "CPX", "CPY"}
RMW = {"INC", "DEC", "ASL", "LSR", "ROL", "ROR", "TSB", "TRB"}
OPRS = {"": 0, "A": 0, "d": 1, "dx": 1, "dy": 1, "di": 1, "dxi": 1, "diy": 1, "dl": 1, "dly": 1, "sr": 1, "sry": 1, "a": 2,
        "ax": 2, "ay": 2, "ai": 2, "axi": 2, "ail": 2, "al": 3, "alx": 3, "r": 1, "rl": 2, "mv": 2, "s": 1}
PTR = {"di": 2, "dxi": 2, "diy": 2, "sry": 2, "dl": 3, "dly": 3}
FIXED = {"PHP": 1, "PHB": 1, "PHK": 1, "PHD": 2, "PLP": 1, "PLB": 1, "PLD": 2, "RTS": 2, "RTL": 3, "RTI": 4, "PER": 2,
         "BRK": 6, "COP": 6}
FLOW = {"JMP", "JML", "JSR", "JSL", "RTS", "RTL", "RTI", "BRK", "COP", "BRA", "BRL", "BPL", "BMI", "BVC", "BVS", "BCC", "BCS",
        "BNE", "BEQ", "MVN", "MVP", "WAI", "STP"}

def listing_by_order(rows):
    """Instructions from the order of the addresses.  The widths of A and of X, Y follow the
    REP, SEP and pulled P bytes seen; where an instruction does not end at the next fetch
    with the widths as they stand, the other widths are tried and kept if they fit.
    '!' = no widths fit (an interrupt, or a cycle the decoder does not know)."""
    n = len(rows)
    def nxt(j):                       # the next cycle that is not internal
        while j < n and rows[j][5] == "int":
            j += 1
        return j
    def decode(i, m, x):
        num, b, a, d, rw, kind, vp, irq = rows[i]
        name, mode = OPS[d][0], (OPS[d][1] if len(OPS[d]) > 1 else "")
        no = (1 if name in ("REP", "SEP") else 2 - m if name in A_OPS else 2 - x) if mode == "#" else OPRS[mode]
        if name == "PEA": no = 2
        j, opr, data = i + 1, [], []
        def take(j, k, what):
            for _ in range(k):
                j = nxt(j)
                if j >= n: break
                what.append(rows[j]); j += 1
            return j
        if name == "JSL":
            seq = []; j = take(j, 6, seq)
            opr = [r for k, r in enumerate(seq) if k in (0, 1, 3)]; data = [r for k, r in enumerate(seq) if k in (2, 4, 5)]
        else:
            j = take(j, no, opr)
            w = 2 - m if name in A_OPS else 2 - x
            if name in FIXED: nd = FIXED[name]
            elif name in ("PHA", "PLA"): nd = 2 - m
            elif name in ("PHX", "PHY", "PLX", "PLY"): nd = 2 - x
            elif name == "PEA": nd = 2
            elif name == "PEI": nd = 4
            elif name == "JSR": nd = 2 + (2 if mode == "axi" else 0)
            elif name in ("JMP", "JML"): nd = {"ai": 2, "axi": 2, "ail": 3}.get(mode, 0)
            elif name in ("MVN", "MVP"): nd = 2
            elif mode in ("", "A", "#", "r", "rl", "s"): nd = 0
            else: nd = PTR.get(mode, 0) + (2 * w if name in RMW else w)
            j = take(j, nd, data)
        ok = name == "JSL" or all((r[1], r[2]) == (b, (a + 1 + k) & 0xFFFF) for k, r in enumerate(opr))
        k = nxt(j)
        if ok and name not in FLOW and k < n and (rows[k][1], rows[k][2]) != (b, (a + 1 + len(opr)) & 0xFFFF):
            ok = False
        # (a data cycle is not a fetch from the program: its address is not the next of this instruction's)
        if ok and any((r[1], r[2]) == (b, (a + 1 + len(opr)) & 0xFFFF) for r in data) and name not in FLOW:
            ok = False
        return ok, j, name, opr, data
    m = x = 0
    i = 0
    while True:
        i = nxt(i)
        if i >= n:
            break
        num, b, a, d, rw, kind, vp, irq = rows[i]
        if vp:
            print(f"{num:5d}                               VECTOR PULL {b:02X}:{a:04X} = {d:02X}"); i += 1; continue
        for mm, xx in ((m, x), (m ^ 1, x), (m, x ^ 1), (m ^ 1, x ^ 1)):
            ok, j, name, opr, data = decode(i, mm, xx)
            if ok:
                m, x = mm, xx; break
        else:
            ok, j, name, opr, data = decode(i, m, x)
        ob = [r[3] for r in opr]
        if name == "REP" and ob: m, x = m & ~(ob[0] >> 5) & 1, x & ~(ob[0] >> 4) & 1
        if name == "SEP" and ob: m, x = m | (ob[0] >> 5) & 1, x | (ob[0] >> 4) & 1
        if name in ("PLP", "RTI") and data: m, x = (data[0][3] >> 5) & 1, (data[0][3] >> 4) & 1
        dd = "  ".join(f"{'VP ' if r[6] else ''}{r[4]} {r[1]:02X}:{r[2]:04X}={r[3]:02X}" for r in data)
        low = any(r[7] for r in rows[i:j])
        code = " ".join(f"{v:02X}" for v in [d] + ob)
        print(f"{num:5d} {b:02X}:{a:04X}  {code:11s}  {dis(a, d, ob):22s} {dd}{'' if ok else '  !'}{'   irq' if low else ''}")
        i = j

if __name__ == "__main__":
    main()
