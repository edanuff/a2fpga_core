# IIgs ADB microcontroller firmware (ROM 01, M50740, 341s0345) — reading notes

Why: the a2mega socket bench (test_log.md G45–G63) shows this IIgs re-asserting
its /RESET 9.95 ms after its own power-on release, permanently, on about half
of power-ups — only when the a2mega is in the slot and no real 65816 is
executing. The two drivers on the RESET.L net are the M50740 (P2 bit 5) and
the Mega II. These notes are what the micro's firmware says about that.

Source: MAME ROM set `apple2gsr1`, file `341s0345.bin` (3 KB, SHA1
97e421f5…), loaded at $1400–$1FFF (MAME `ROM_REGION(0xc00, "adbmicro")`).
Disassembled with `tools/dis740.py` (MELPS 740 table after Neil Parker's
page, https://llx.com/Neil/a2/adb.html). The ROM image and the listing are
Apple's; they stay out of the repo — regenerate with the tool.

Port map (Parker): P0 = GLU data; P2: bit 7 ADB data in, bit 6 IIe-keyboard
/KRESET (ROM 0/1), **bit 5 = IIgs /RESET**, bit 4 GLU strobe, bits 3–0 GLU
register select; P3: bit 5/4 button 0/1 lines, bit 3 ADB data out, bit 2/1/0
IIe-keyboard CAPLOCK/CNTRL/SHIFT. Vectors $1FF4–$1FFF: **every interrupt
(INT/VBL, Timer 1, Timer 2, Timer X, CNTR) → $14DC; RESET → $1400.**

## What the code does with the reset line

1. **At power-on the micro does not hold /RESET.** `$1433` (port init, run
   from the reset vector) sets `P2 = $14`, `P2DIR = $1F` — bit 5 is an
   **input**. The only way the firmware drives the line is the routine at
   `$147E`: `LDM #$3F,P2DIR` (make bit 5 an output) with `P2 = $DF` (bit 5
   low) — open-drain style; `$148B` `LDM #$1F,P2DIR` releases it (back to
   input). A second copy of assert/(delay $1F5F)/release lives at
   `$15BC–$15C7` (the ADB "reset" command path, `$154E` dispatcher, command
   ≥ $10 → `$15BA`).
   → **The machine's 333 ms power-on release cannot be the micro letting go
   of P25; it is the Mega II's internal power-on reset ending.** The micro
   is not driving the line at that time at all.

2. **Reset entry `$1400`**: SEI, port init, then a wait loop
   `$140A–$1413`: `INY; BEQ $1457 (cold init); JSR \$1F3A (wait ≤ 210 polls
   for a CPU command via the GLU); BCC $140A (timeout → INY, retry); CMP
   #$07; BNE $140B (a non-SYNC command: consume it, retry WITHOUT INY);
   JSR $15D3 (SYNC: read 3 parameter bytes, `$141A` clears vars $07–$0B and
   programs the timers, `$16C9` ADB bus init); BRA $1469`. Cold init
   `$1457` (Y wrapped after ~238 timeouts ≈ 2 s): defaults into $0C/$01/
   $00/$03, `$141A`, `$16C9`. Both paths → `$1469`: **`$144A` arms Timer X
   as the watchdog** (TXPRE = TX = $FA, TIMCTL = $40 → period 252×252 /
   223.72 kHz = **283.8 ms**), CLI, then the main loop `$146C`.

3. **Main loop reset decision `$146C`**:
   `BBS 2,$0A → ASSERT ($147E)`; else `BBC 6,P2 (IIe /KRESET low) → $1475`
   else `BBC 3,$0A → RELEASE ($148B)`; `$1475: BBS 7,$03 → ASSERT; BBC 1,P3
   (IIe CNTRL low) → ASSERT; BBC 0,$0B → RELEASE; else ASSERT`.
   Flag sources: `$0A.2` set at `$1524` in the ADB-keyboard data handler
   (`$14FF`: after a Talk to the keyboard, `LDA #$10 [ORA #$08 unless
   $03.7]; AND $44; BNE $153E (CLB 3,$0A); SEB 2,$0A` — $44 = keyboard
   register 2 high byte, bit 4 = Reset key, bit 3 = Control, 0 = pressed:
   **Control-Reset from the ADB keyboard → assert**), cleared at `$1502`
   when that handler runs again. `$0A.3` set/cleared at `$1536/$153E` on
   $44 == $7F / $FF (keyboard reset-key states). `$03` = SYNC parameter
   byte 1 (`$15FC`) or a mode command. `$0B` written at `$1A8D/$1F8D`
   (keyboard/mouse processing). So **every assert path is keyboard-state
   driven**: the ADB keyboard's reset/control bits ($44), or the IIe
   keyboard connector's /KRESET (P2.6) and CNTRL (P3.1) inputs read low.

4. **Watchdog / interrupts `$14DC`**: `LDY #$40; JSR \$1F47 (offer byte $40
   to the CPU through the GLU data register, ≤ 200 polls); BCC $14DE
   (repeat); JMP $1400` — a Timer X expiry (or any enabled interrupt)
   **restarts the firmware from $1400**, which *releases* the reset line
   (port init → P2DIR = $1F) and goes back to waiting for a SYNC. **A
   watchdog restart does not assert /RESET.** The main loop re-arms Timer X
   (`$14A9`: `LDA TX; CMP #$F4; BCS skip; STA TIMCTL #0; JSR $144A`) each
   pass. Timers 1/2 (`T12PRE = $33`, `T1 = T2 = $24` → 4221 Hz prescaler,
   9.0 ms period) are read as a software timebase in the release path
   (`$148E`) and reloaded (`$149C` T2 = $A3) for the ADB poll cadence;
   their interrupts are off (INTCTL = 0).

## What this means for the bench observation

- The 333 ms release = Mega II POR end. The 9.95 ms pull is either the
  Mega II re-asserting or the micro's `$147E`. The micro reaches `$147E`
  only through the keyboard-state paths above; the fastest plausible route
  after a release is: the micro leaves its SYNC-wait (a SYNC from the ROM
  ~10 ms after a real CPU starts, or a spurious command when the bus is
  driven by nobody), initialises the ADB bus (`$16C9`, ~ms), polls the
  keyboard (Talk R2, ~3.5 ms), and evaluates $44 — a phantom "Reset key
  down" (bits 4/3 clear) would assert and hold until a later poll returns
  them set. Timing-wise that lands near 10 ms; it is not proven.
- A parked line with no CPU present fits either party: the micro holding
  `$147E` waiting for keyboard state that never changes, or the Mega II.
- The "watchdog race" reading is not supported: Timer X only restarts the
  firmware into the released state.

## Discriminating measurements (no board handling)

1. **AD3 on the ADB data line at the keyboard port** (DIN pin 1 vs GND),
   alongside slot /RESET: if the micro issues ADB traffic (attention +
   Talk) in the 10 ms before the pull, the micro's keyboard path is live;
   if the bus is silent, look at the Mega II.
2. **AD3 on M50740 pin 49 (P25)** directly: low at the pull = micro; high
   while RESET.L is low = Mega II. Needs a probe on the motherboard.
3. MAME (`apple2gsr1` is on disk) with the debugger on the `adbmicro` CPU:
   breakpoint at `$147E` and `$1400`, trace the normal boot's timeline
   (when the main loop starts, when the first keyboard poll happens) to
   calibrate the 10 ms.

## Bench specifics (ed, 2026-09-12)

- **This ROM 01 machine uses a classic IIe keyboard on J13, not ADB.** So the
  live inputs to the reset logic are the IIe-connector lines: P2.6 /KRESET
  (the keyboard's RESET key line) and P3.1 CNTRL (the Control key line),
  plus the matrix scan through P1 / the GLU select lines (`$18AC`). On the
  ROM 3 netlist those port pins are grounded (Parker: "always 0 on ROM 3");
  on ROM 01 they come from J13. The Control-Reset path in the main loop is
  therefore: `/KRESET low` and (`CNTRL low` or `$03.7` or `$0B.0`) → assert
  at `$147E`; release when /KRESET reads high again. **A /KRESET that reads
  low persistently parks the machine in reset** — the same symptom as the
  bench wedge. The ADB-keyboard paths (`$0A.2/$0A.3` via `$44`) are inert
  without ADB devices: a Talk with no response leaves them clear.
- **The ADB GLU (KEYGLU, UI12) sits on RESET.L too**, as an input (pin 33):
  it is the register file between the CPU ($C000/$C010/$C024–$C027) and the
  micro (P0 data, P2.0–3 select, P2.4 strobe), reset by the line, with no
  reason or means to drive it. It matters only as the path by which
  spurious bus writes could reach the micro's command register.

Measurements that follow from this (all digital, AD3, no board handling
beyond a clip on the keyboard cable):
1. **J13 /KRESET and CNTRL** alongside slot /RESET: if either reads low
   at, or in the 10 ms before, the pull, the micro is asserting via its
   Control-Reset logic and the question becomes why the line dips
   (pull-up value on this board, keyboard cable, coupling from the bus
   starting up).
2. **ADB data line at the ADB port**: traffic before the pull = the micro
   is out of its SYNC-wait and polling; silence = still waiting.
3. Micro P25 (pin 49) vs the line, if the above are inconclusive.

## Mega II reset behaviour — what the Mega-IIe project shows (rev3b, `/Users/edanuff/GitHub/Mega-IIe`)

A working Apple II built around an extracted Mega II (no FPI, no ADB micro,
a 65C02 and an RP2040 keyboard/power controller). Netlist (kicad-cli):
- `/IWM/~{RESET}`: Mega II pin 35, the 65C02 RESB, the IWM, the Slotmaker,
  the slot connector pin 31, **two 4k7 pull-ups**, the front-panel reset
  switch to ground, an **NPN open-collector driver (Q3 BC817) from the RP2040's
  RESET_CTL**, and two read-backs into the RP2040s (74LVC2G04 → GPIO11
  `RESET_STATUS`, 74HCT245 → the video RP2040). Nothing drives the line
  push-pull: they treat it as a shared open-drain net, i.e. consistent with
  the Mega II being able to drive it itself.
- Firmware `power_sequence.c`: on power-on the RP2040 **asserts reset before
  enabling the supplies, waits 50 ms after they are on, then releases**;
  its power-cycle and Control-Reset paths hold for 250 ms. It never waits on
  or relies on the Mega II's own power-on reset; `RESET_STATUS` is only read
  once at start-up (no monitoring for a Mega II-originated reset).
- No comment or code in the project describes the Mega II re-asserting
  reset after release. Their design choice — always give the Mega II an
  externally-held reset that outlasts the supply ramp by 50 ms — is the same
  posture the a2mega's 2G06 hold gives it in slot-powered use.

## Emulator / FPGA-core survey (2026-09-12; clones under the session scratchpad)

MAME (`apple2gs.cpp` + `m5074x.cpp`), KEGS (a2kegs mirror), GSplus, Clemens,
MiSTer `Apple-IIgs_MiSTer`. **None models the Mega II as a reset source,
holder or watchdog; none has a "Mega II re-resets when pulled low" comment;
none has a power-on timing constant.** Only MAME models the real topology:
the micro runs its real firmware, `set_pullups<2>(0x20)` puts a pull-up on
P25, the M5074x ports reset to inputs, so the line is released until the
firmware drives it; a P25 1→0 asserts the 65816 reset and resets the bus
and the Mega II soft-switch state as a side effect (`adbmicro_p2_out`,
3424–3461). MAME's only ordering note: "the 65816 loses a race to the
microcontroller on reset" (modifier snapshot frozen for two $C061 reads).
KEGS/GSplus: `do_reset()` at start, on host Ctrl-Reset, and on ADB command
$10 — instantaneous. Clemens: RESB held 3 CPU cycles at power-on, 2 on
Ctrl-Reset; its GLU is explicitly "not an accurate emulation". MiSTer:
reset = OR of PLL-lock/buttons/host events, CPU `.RST_N(~reset)`; the ADB
micro is behavioural; the only micro-power note is that the real micro
keeps its RAM power-up flag ($51 = $A5) across a Ctrl-Reset. So the
10 ms re-assert and the Mega II's power-on behaviour are not documented
anywhere in software; they have to come from the bench.

