#!/usr/bin/env python3
"""Mitsubishi M50740/M50741 (MELPS 740) disassembler — for the Apple IIgs ADB
microcontroller ROM (341s0345 = ROM 01 M50740, 3 KB at $1400; 341s0632 = ROM 3
M50741, 4 KB at $1000). Opcode table after Neil Parker's "Inside the IIGS ADB
Controller" (llx.com/Neil/a2/adb.html).

usage: dis740.py <rom.bin> <base_hex> [start_hex] [end_hex]
Linear sweep; prints address, bytes, mnemonic; annotates the IIgs port/timer
addresses and the reset/interrupt vectors at $1FF4-$1FFF.
"""
import sys

# mode codes: imp, A, imm, zp, zpx, zpy, indx, indy, zpi, sp, rel, abs, absx, absy, absi,
#             bAr (bit,A,rel: 2 bytes), bzr (bit,zp,rel: 3 bytes), bA (bit,A: 1), bz (bit,zp: 2), immzp (LDM: 3)
T = {}
def row(hi, cells):
    for lo, cell in enumerate(cells.split("|")):
        cell = cell.strip()
        if cell == "-":
            continue
        mn, md = cell.split()
        T[hi * 16 + lo] = (mn, md)
row(0x0, "BRK imp|ORA indx|JSR zpi|BBS bAr|-|ORA zp|ASL zp|BBS bzr|PHP imp|ORA imm|ASL A|SEB bA|-|ORA abs|ASL abs|SEB bz")
row(0x1, "BPL rel|ORA indy|CLT imp|BBC bAr|-|ORA zpx|ASL zpx|BBC bzr|CLC imp|ORA absy|DEC A|CLB bA|-|ORA absx|ASL absx|CLB bz")
row(0x2, "JSR abs|AND indx|JSR sp|BBS bAr|BIT zp|AND zp|ROL zp|BBS bzr|PLP imp|AND imm|ROL A|SEB bA|BIT abs|AND abs|ROL abs|SEB bz")
row(0x3, "BMI rel|AND indy|SET imp|BBC bAr|-|AND zpx|ROL zpx|BBC bzr|SEC imp|AND absy|INC A|CLB bA|LDM immzp|AND absx|ROL absx|CLB bz")
row(0x4, "RTI imp|EOR indx|STP imp|BBS bAr|COM zp|EOR zp|LSR zp|BBS bzr|PHA imp|EOR imm|LSR A|SEB bA|JMP abs|EOR abs|LSR abs|SEB bz")
row(0x5, "BVC rel|EOR indy|-|BBC bAr|-|EOR zpx|LSR zpx|BBC bzr|CLI imp|EOR absy|-|CLB bA|-|EOR absx|LSR absx|CLB bz")
row(0x6, "RTS imp|ADC indx|-|BBS bAr|TST zp|ADC zp|ROR zp|BBS bzr|PLA imp|ADC imm|ROR A|SEB bA|JMP absi|ADC abs|ROR abs|SEB bz")
row(0x7, "BVS rel|ADC indy|-|BBC bAr|-|ADC zpx|ROR zpx|BBC bzr|SEI imp|ADC absy|-|CLB bA|-|ADC absx|ROR absx|CLB bz")
row(0x8, "BRA rel|STA indx|RRF zp|BBS bAr|STY zp|STA zp|STX zp|BBS bzr|DEY imp|-|TXA imp|SEB bA|STY abs|STA abs|STX abs|SEB bz")
row(0x9, "BCC rel|STA indy|-|BBC bAr|STY zpx|STA zpx|STX zpy|BBC bzr|TYA imp|STA absy|TXS imp|CLB bA|-|STA absx|-|CLB bz")
row(0xA, "LDY imm|LDA indx|LDX imm|BBS bAr|LDY zp|LDA zp|LDX zp|BBS bzr|TAY imp|LDA imm|TAX imp|SEB bA|LDY abs|LDA abs|LDX abs|SEB bz")
row(0xB, "BCS rel|LDA indy|JMP zpi|BBC bAr|LDY zpx|LDA zpx|LDX zpy|BBC bzr|CLV imp|LDA absy|TSX imp|CLB bA|LDY absx|LDA absx|LDX absy|CLB bz")
row(0xC, "CPY imm|CMP indx|SLW imp|BBS bAr|CPY zp|CMP zp|DEC zp|BBS bzr|INY imp|CMP imm|DEX imp|SEB bA|CPY abs|CMP abs|DEC abs|SEB bz")
row(0xD, "BNE rel|CMP indy|-|BBC bAr|-|CMP zpx|DEC zpx|BBC bzr|CLD imp|CMP absy|-|CLB bA|-|CMP absx|DEC absx|CLB bz")
row(0xE, "CPX imm|SBC indx|FST imp|BBS bAr|CPX zp|SBC zp|INC zp|BBS bzr|INX imp|SBC imm|NOP imp|SEB bA|CPX abs|SBC abs|INC abs|SEB bz")
row(0xF, "BEQ rel|SBC indy|-|BBC bAr|-|SBC zpx|INC zpx|BBC bzr|SED imp|SBC absy|-|CLB bA|-|SBC absx|INC absx|CLB bz")

LEN = {"imp": 1, "A": 1, "bA": 1, "imm": 2, "zp": 2, "zpx": 2, "zpy": 2, "indx": 2, "indy": 2, "zpi": 2, "sp": 2,
       "rel": 2, "bAr": 2, "bz": 2, "abs": 3, "absx": 3, "absy": 3, "absi": 3, "bzr": 3, "immzp": 3}

NAMES = {0xE0: "P0", 0xE1: "P0DIR", 0xE2: "P1", 0xE3: "P1DIR", 0xE4: "P2", 0xE5: "P2DIR", 0xE8: "P3", 0xE9: "P3DIR",
         0xF9: "T12PRE", 0xFA: "T1", 0xFB: "T2", 0xFC: "TXPRE", 0xFD: "TX", 0xFE: "INTCTL", 0xFF: "TIMCTL"}
P2BITS = {7: "ADBDATA_IN", 6: "KRESET", 5: "SYS_RESET", 4: "GLU_STB", 3: "SEL3", 2: "SEL2", 1: "SEL1", 0: "SEL0"}
P3BITS = {7: "KSW0", 6: "KSW1", 5: "BUTTON0", 4: "BUTTON1", 3: "ADBDATA_OUT", 2: "CAPLOCK", 1: "CNTRL", 0: "SHIFT"}

def zpname(a):
    return NAMES.get(a, "$%02X" % a)

def fmt(op, md, b, pc, ln):
    mn = T[op][0]
    bit = (op >> 5) & 7
    if md in ("imp",): return mn
    if md == "A": return mn + " A"
    if md == "imm": return "%s #$%02X" % (mn, b[1])
    if md == "zp": return "%s %s" % (mn, zpname(b[1]))
    if md == "zpx": return "%s %s,X" % (mn, zpname(b[1]))
    if md == "zpy": return "%s %s,Y" % (mn, zpname(b[1]))
    if md == "indx": return "%s ($%02X,X)" % (mn, b[1])
    if md == "indy": return "%s ($%02X),Y" % (mn, b[1])
    if md == "zpi": return "%s ($%02X)" % (mn, b[1])
    if md == "sp": return "%s \\$1F%02X" % (mn, b[1])
    if md == "rel":
        d = b[1] - 256 if b[1] > 127 else b[1]
        return "%s $%04X" % (mn, (pc + ln + d) & 0x1FFF)
    if md == "abs": return "%s $%04X" % (mn, b[1] | (b[2] << 8))
    if md == "absx": return "%s $%04X,X" % (mn, b[1] | (b[2] << 8))
    if md == "absy": return "%s $%04X,Y" % (mn, b[1] | (b[2] << 8))
    if md == "absi": return "%s ($%04X)" % (mn, b[1] | (b[2] << 8))
    if md == "bA": return "%s %d,A" % (mn, bit)
    if md == "bz":
        note = ""
        if b[1] == 0xE4 and bit in P2BITS: note = "   ; P2." + P2BITS[bit]
        if b[1] == 0xE8 and bit in P3BITS: note = "   ; P3." + P3BITS[bit]
        return "%s %d,%s%s" % (mn, bit, zpname(b[1]), note)
    if md == "bAr":
        d = b[1] - 256 if b[1] > 127 else b[1]
        return "%s %d,A,$%04X" % (mn, bit, (pc + ln + d) & 0x1FFF)
    if md == "bzr":
        d = b[2] - 256 if b[2] > 127 else b[2]
        note = ""
        if b[1] == 0xE4 and bit in P2BITS: note = "   ; P2." + P2BITS[bit]
        if b[1] == 0xE8 and bit in P3BITS: note = "   ; P3." + P3BITS[bit]
        return "%s %d,%s,$%04X%s" % (mn, bit, zpname(b[1]), (pc + ln + d) & 0x1FFF, note)
    if md == "immzp": return "%s #$%02X,%s" % (mn, b[1], zpname(b[2]))
    return mn

def main():
    rom = open(sys.argv[1], "rb").read()
    base = int(sys.argv[2], 16)
    start = int(sys.argv[3], 16) if len(sys.argv) > 3 else base
    end = int(sys.argv[4], 16) if len(sys.argv) > 4 else base + len(rom)
    top = base + len(rom)
    vecs = {0x1FF4: "INT (VBL)", 0x1FF6: "TIMER2", 0x1FF8: "TIMER1", 0x1FFA: "TIMERX", 0x1FFC: "CNTR (ADB line)", 0x1FFE: "RESET"}
    if top == 0x2000:
        print("; vectors:")
        for a, n in vecs.items():
            lo, hi = rom[a - base], rom[a + 1 - base]
            print(";   $%04X %-16s -> $%04X" % (a, n, lo | (hi << 8)))
    pc = start
    while pc < min(end, top):
        op = rom[pc - base]
        if op not in T:
            print("%04X  %02X        ???" % (pc, op)); pc += 1; continue
        mn, md = T[op]
        ln = LEN[md]
        b = rom[pc - base: pc - base + ln]
        if len(b) < ln:
            break
        print("%04X  %-9s %s" % (pc, b.hex(" ").upper(), fmt(op, md, b, pc, ln)))
        pc += ln

if __name__ == "__main__":
    main()
