# S7 — the accelerator-only build

Status: on hardware since 2026-11-12 (section 3). The DDR3 build of
section 3b ran on 2026-11-13 and runs the benchmark 12 to 14 times faster
than the machine (section 3c). That run found a defect in the near
memory's empty entry; the build with the fix (m0) is on the card since
18:21 the same day and ran the same benchmark without it (section 3c).
Built since (t0, section 3e): the near memory as lines of sixteen bytes, a
DDR3 read filling a whole line, and every step in the expansion banks two
clks on a hit; three placements at 105 MHz with no violation. **Flashed
2026-11-14 08:53 and run the same morning (section 3f): card memory
verifies with no mismatch, and the benchmark's average is 44.02 MHz where
m0 had 33.34; the sieve and the sort are above 48, the floating point
tests 36 to 38.**
Built since and not on the card (section 3h): a copy of the Mega II's RAM
below `$C000` (banks E0/E1) kept in the DDR3, read in place of the socket
when a request bit asks, switchable at any step. Simulated in full; **at
105 MHz five placements of ten have no violation, which is not closure.**
One clean placement (x0) is on the card since 2026-11-14 15:39 as an
experiment, by Ed's decision: the floating point tests go from 37.81 and
35.76 to 48.14 and 44.51 over ten runs, the average from 44.24 to 47.48;
the switch works while the machine runs.
Built since and not on the card (section 3j): the first-clk write rule
reads the shadow register, so the benchmark's stack and direct page
writes (section 3i) are two clks. Simulated in full; **at 105 MHz two
placements of five have no violation: not closed.** One clean placement
(q1) is on the card since 2026-11-14 19:20 as an experiment, by Ed's
decision: a whole base pass in paced verify shows no mismatch, and the
benchmark's average over ten runs is **50.76 MHz** (x0: 47.48), the
sieve, the sort, Fibonacci and integer math at 52.7 to 52.9; the sum of
its times is 7 % under the AppleSqueezer GS Turbo's published ones.
**Closed since (section 3k): with `Place_Option` 1 and the PHI2 period
counter without its adder, three placements of three at 105 MHz have no
violation (p0, p1, p2). p1 flashed 2026-11-15 and brought up to paced
card memory with the copy, no mismatch, then fast (`1B`): five runs of
the benchmark average 50.75 MHz, q1's figure.** **Since: the firmware
brings it to fast mode by itself (section 3l, on the card), and speed
control is built - paced mode at the machine's pace and the machine's
speed rules (section 3m: simulated, three placements with no violation;
sc1 and its firmware flashed 2026-11-15 15:43; one power-up on the
machine: fast by itself, and a stretch of the boot with the speed
register at normal seen paced by the rules; the a2p25's emulated
Disk II booted and read through them, and the motor rule seen pacing by
itself; a fault of the slot 6 rule seen and the rule changed in the
source, with the repair of a ROM copy that begins under a RDY stall
(simulated; **three placements with no violation, se0 to se2**; se2
flashed 2026-11-16 07:26; **on a power-up that evening the bring-up
stopped on a verify mismatch in bank 01 and left the card on socket
bytes, and Arkanoid locked up in that session: cause not known,
section 3m; **2026-11-21: the lock happens with the accelerator off by
its setting, mode 00, and on p1 built again - it is not of the speed
control work; a trace build to read it is built, three placements with
no violation, not flashed: section 3n**); no write, no real drive, no benchmark yet). **Found with it, and not of this build: the copy of E0/E1 has
only what the machine was given since the card began to mirror, and
RAM that nothing has written since power-up reads differently from the
copy than from the machine - about 23,000 bytes on this power-up, and
the Control Panel reads one such line (section 3m, "The cause"). x0,
q1 and p1 have it too. Not fixed.**
Parents: [`gs_socket_s7_accelerator_scoping.md`](gs_socket_s7_accelerator_scoping.md)
(what the accelerator is) and
[`gs_socket_s7_fast_core_exploration.md`](gs_socket_s7_fast_core_exploration.md)
(§2d: why an accelerator-only card beside an a2p25; "Rung 5": the timing).

## 1. What it is

`a2mega_accel_138B.gprj` builds the a2mega 1.0a3 with the 138B SOM as a CPU
accelerator and nothing else:

- `hdl/twgs/gs_accel.sv`, the accelerator domain as one module: the socket
  PHY, the broker with the near memory and the two-clk step, the `F65C816`
  core with its microcode in block RAM, banks 00/01 and the ROM image in
  block RAM (128 KB each), and `gs_accel_romcopy`, which reads the ROM image
  through the socket.
- The ESP32 connector, for control and telemetry, with everything else it
  serves in the full build tied off.
- The slot side passive: level shifters on as inputs, the data bus never
  turned round, no /IRQ, /INH, /DMA or /RDY, the two daisy chains passed
  through. The one slot line the card can drive is /RESET, through the
  ESP32's reset hold, as in the full build.
- No framebuffer, DisplayPort, slot cards, slot ROM or I/O decode, or OSD.
  The DisplayPort balls are not ports of this top; they take the
  bitstream's unused-pin default (input with pull-up), as in the DP test
  builds. The DDR3 was left out the same way until section 3b, which gives
  it to the accelerator.

Clocks: the 50 MHz crystal; `clk_logic` (the board PLL's 54 MHz) for the
connector; `clk_gs` for the accelerator, from `gowin_gs_pll`, **the S6 socket
PLL, unchanged: 110 MHz**, so the core steps at **55 MHz**. (With the DDR3,
section 3b, the build is at 105 MHz and 52.5 MHz steps.)

> The decision recorded on 2026-11-12 was "108 MHz", made on the statement
> that S6's PHY had run on hardware at 108 MHz. It ran on this PLL, which is
> 110 MHz (50 MHz × 22 / 10; `hdl/a2mega_138b.sdc` constrains it at
> 9.091 ns); "108" is the nominal figure in the PHY's comments and the clock
> of the simulation bench. The build keeps the hardware-proven PLL and PHY
> rather than introduce a new PLL recipe for 2 %. Say so if a true 108 MHz
> is wanted; the same design closes there as well (probe `_n8`).

## 2. Control and telemetry

Through the connector's GS window, with the firmware the S6 build already
has (`gs`, `gs set <idx> <val>`, `gs trace` on telnet). Nothing in the
firmware was changed for this build.

| window register | S6 meaning | here |
|---|---|---|
| 0 CTRL | arm, sweep, listen, trace bits, clear | bit 0 arm, bit 2 listen, bit 7 clear, as S6 (the firmware's auto-arm drives it); the other bits do nothing |
| 2 | address-delay sweep | the same |
| 3 | data-hold sweep tap | nothing |
| 29 | trace trigger address, low | the accelerator's requests (below) |
| 30 | … high | nothing |
| 31 | … bank | **must hold `A5`**, or register 29 is ignored |

Requests in register 29:

| bit | name | effect |
|---|---|---|
| 0 | FAST | local steps at 55 MHz. Needs CARDMEM. |
| 1 | CARDMEM | the card's memory is the memory of record. In native mode its byte goes to the core and is compared with the socket's (the verify count). |
| 2 | CXROM | internal `$C100–$CFFF` firmware served from the image. Taken when the core is in reset or held (so: at /RES, or set it with COPY). |
| 3 | COPY | 0 → 1: read the ROM image through the socket. The core is held between two steps for about 46 ms. |
| 4 | SLOW | (section 3h; not in the build on the card) reads of the Mega II's RAM below `$C000` in banks E0/E1 come from the card's copy. Needs CARDMEM. May change at any step. |
| 5 | RULES | (section 3m; in the builds on the card since sc1) fast mode gives way to the machine's pace where the speed rules say so. May change at any step. |

Bits 0–2 take effect only once an image has been copied since power-up.
**With register 29 at 00, or without the key, the card is S6 on the new
core: every step a socket cycle, every byte from the socket.** That is the
state after power-up, and after `gs trig` (which writes `FF FF FF` to
29–31: no key).

Telemetry is the S6 window with two fields carrying something else:

| field in `gs` | here |
|---|---|
| status, cycles, stalls, BE-low, PHI2 period and high time, last address | the same |
| hold mismatches | verify count |
| hold samples | `{mode, 5A}`; mode: bit 7 ROM image valid, 6 copy busy, 5 core held, 4 fast, 3 card memory, 2 cxrom, 1 E0/E1 reads from the copy (section 3h) |

`gs trace` prints 64 lines; here line *n* is telemetry word *n* (read
`bank:addr data` as one 32-bit number): 0 core steps, 1 socket cycles for
the core, 2 posted writes, 3 clks waited for the write buffer, 4 local
reads in place, 5 local reads that waited, 6 `{verify count, 00, mode}`,
7 address of the last verify mismatch, 8 `{copies, 00, sum of the image's
bytes mod 65536}`, 9 `{shadow, CYAREG, SLTROMSEL, STATEREG}`, 10 the other
switches, 11 PHY bus cycles; with section 3h's build 13 `{E0/E1 verify
count, 00, 0000, built with the copy, the copy whole, SLOW requested, SLOW
in effect}` and 14 the address of the last E0/E1 mismatch. LEDs (active low): heartbeat, socket ours,
core running, fast.

## 3. A bring-up ladder (proposal)

Each step is observable before the next is taken. All four steps were run
once on 2026-11-12 (below), without video and with the machine idling.

1. **S6 equivalence.** Power up beside the a2p25; let the firmware arm as
   it does for S6; register 29 untouched. The machine should behave exactly
   as with the S6 bitstream. This is already new on hardware: the
   `F65C816` core, the broker's native mode and the 110 MHz core clock
   domain (S6's core ran on PHI2).
2. **Key and copy.** `gs set 31 0xA5`, `gs set 29 0x08`. Mode shows copy
   busy, then ROM image valid; word 8 gives the image's byte sum, to be
   compared with the sum of a dump of that machine's ROM. The machine
   pauses for about 46 ms.
3. **Card memory, native.** `gs set 29 0x0A`. The core now takes the card's
   bytes; the verify count must stay 0. A count that moves gives the
   address in word 7.
4. **Fast.** `gs set 29 0x0B`. LED 3 on; words 0 and 1 show steps running
   ahead of socket cycles.

Things to know before step 3:

- Banks 00/01 on the card hold what the core has **written since the
  socket was armed**. Anything else differs from the machine's RAM: RAM
  written before a re-arm without a power cycle, RAM written by slot DMA.
  Step 3 is where that shows.
- /RES during a copy cuts it short (image not valid; copy again). A warm
  reset keeps a valid image and the card's RAM.
- The step that was held and every byte in the near memory are not trusted
  after a copy: the near memory is emptied before the core goes on.

### First hardware run, 2026-11-12

ROM 01 IIgs, the card alone (no a2p25: no video, no boot device), slot
powered, firmware of Sep 13 2026 arming as for S6 (mode 4). Bitstream sha256
`48c06bbc…c8c04baa`, source 857d136c, flashed over USB with read-back verify.
Read over telnet with `gs`, `gs events` and `gs trace`.

| step | what was done | result |
|---|---|---|
| 1 | power-on, register 29 untouched | startup beep, then the ROM's no-device beep. Signature `5A`, mode 00. The core ran the ROM to the "No device connected!" screen and sat in its sliding-apple loop (FF:A55E–A5AA, polling `$C019`); 280 M steps, one socket cycle each, 0 stalls. Event log: release, arm 36 ms later, no /RESET fall afterwards |
| 2 | `gs set 31 0xA5`, `gs set 29 0x08` | mode 80, 1 copy, image sum `7EE8` = the sum of a ROM 01 image; the machine carried on |
| 3 | `gs set 29 0x0A` | mode 88; verify count 0 after 76 M cycles |
| 4 | `gs set 29 0x0B` | mode 98; rates over 12.7 s of wall clock: 3.566 M steps/s, 0.510 M socket cycles/s for the core, 1.020 M PHY cycles/s, 2.55 M local read hits/s, 0 misses, 119 posted writes/s (the loop's two text-screen writes per frame); 0 stalls; the core stayed in the same loop |

What this does and does not show:

- The loop is bound by the bus, not by the core: each pass reads `$C019`,
  a slow soft switch, so it runs 7 steps per socket cycle and one socket
  read per two Mega II cycles (a native pass takes four). It is no measure
  of the 55 MHz step rate: the longest local run in it is six steps.
- Fast mode has no byte-for-byte check, and there was no video. The
  evidence that the core stayed right is that it stayed in the loop, with
  the expected two forwarded writes per frame.
- Nothing but the idle loop has run: no program, no disk, no interrupts
  beyond what the ROM enables at that screen, no reset while fast.
- The telemetry words are read a byte at a time and are not latched: a
  word can be off by `0x100` or `0x10000` when a carry lands mid-read. Read
  twice before trusting a difference.

### With the a2p25 in slot 3, the same day

The same card and bitstream, an a2p25 in slot 3 for video and storage (a
USB stick on its hub), Marble Madness booting from the stick.

| step | result |
|---|---|
| power-on, two cards holding /RESET | the a2mega let go at 450 ms, the a2p25 held the line until 8.49 s (waiting for its stick), the firmware armed 26 ms after the rise; no /RESET fall afterwards. The game boots and plays on the new core: 2.07 M steps/s, one socket cycle each, 0 stalls |
| `gs set 29 0x08` with the game running | 1 copy, sum `7EE8` again |
| `gs set 29 0x0A` | verify count 0 after 95 M cycles, about 3.3 M bytes served by the card and compared |
| `gs set 29 0x0B` | the game keeps playing, with no artifacts and its music at tempo, but **slower**: 1.449 M steps/s against 2.077 M native. 1.184 M socket cycles/s for the core in 2.367 M PHY cycles/s: one idle cycle behind every cycle of the core |

Two things this showed:

- **The game runs from banks 02 and 03.** The card holds banks 00/01 and
  the ROM; expansion RAM on the card is S7d. Only about 3 % of the game's
  cycles are reads the card can serve, so fast mode has little to give it.
  Speed shows on software that lives in banks 00/01: eight-bit programs.
- **A defect: in fast mode a socket step behind a socket step waited a
  whole PHI2 cycle behind an idle one.** The sequencer decides a fast-mode
  step in its third clk; the PHY issues in the second. Fixed in the source
  after this run (scoping doc §10.5); the card still carries the build
  without the fix (aoz). The idle loop's "one socket read per two Mega II
  cycles" above is a different thing and stands: there the core runs six
  local steps between two reads, and an idle cycle is already on the bus
  when the next read is known.

A benchmark, the same evening, the same build (aoz, without the fix): a
TML Pascal program that times six tests at the machine's speed, waits for
the speed to be changed, and times them again. Times in 60ths of a second;
fast mode was switched on at its prompt (copy, card memory with a verify
count of 0, then fast).

| test | base | fast mode | ratio |
|---|---|---|---|
| 10 iterations of the sieve | 413 | 437 | 0.95 |
| selection sort of 500 strings | 1159 | 1060 | 1.09 |
| 100 iterations of a floating-point test | 475 | 49 | 9.7 |
| 15,000 Gamm units (floating-point mix) | 1545 | 314 | 4.9 |
| 10 iterations of Fibonacci calls | 2021 | 1794 | 1.13 |
| 5,000 iterations of integer math | 1564 | 1083 | 1.44 |

The program rates the floating-point test at 26 MHz and the Gamm mix at
13 MHz against a base it takes as about 2.5 MHz; its average is 8.5 MHz.
The split is the memory map again. The floating-point work is done by
SANE in the ROM, on a stack and direct page in bank 00: all the card's.
The sieve, the sort and the calls are the program's own code and data in
expansion RAM: every step a socket cycle, and with this build's defect a
socket step behind a socket step waits a cycle, so the sieve came out
slower than base.

**The same benchmark on fx2, the build with the fix** (flashed 19:38 the
same evening; the same machine, cards and firmware; the base pass run
again and within one tick of the first on every test):

| test | base | aoz, fast | fx2, fast | fx2 ratio | the program's MHz |
|---|---|---|---|---|---|
| 10 iterations of the sieve | 413 | 437 | 322 | 1.28 | 3.24 |
| selection sort of 500 strings | 1159 | 1060 | 805 | 1.44 | 3.63 |
| 100 iterations of a floating-point test | 476 | 49 | 41 | 11.6 | 31.35 |
| 15,000 Gamm units | 1546 | 314 | 253 | 6.1 | 16.25 |
| 10 iterations of Fibonacci calls | 2020 | 1794 | 1417 | 1.43 | 3.59 |
| 5,000 iterations of integer math | 1564 | 1083 | 863 | 1.81 | 4.57 |

The program's average went from 8.49 to 10.44 MHz. Every test is faster
than on aoz, by 1.20 to 1.36, and none is below base. At the program's
prompt the core takes 0.844 M socket cycles a second of the same 1.86 M
(aoz: 0.668 M), and steps 12.4 M times a second (aoz: 9.7 M). The build
ran the boot, the ROM copy (sum `7EE8`) and card memory (verify count 0)
as aoz did.

After fast mode has run, register 29 must not go back to 00 without a
reset: the machine's copy of banks 00/01 is stale by design. `0x0A` is the
way back to PHI2 pacing, and its verify count then means nothing.

## 3a. Banks 02/03 in block RAM (2026-11-12, not flashed)

`gs_accel` has a parameter `EXP_BANKS`, 0 or 2, and the board top sets 2:
expansion banks 02 and 03 in block RAM, beside banks 00/01 and on the same
terms. They follow every write of the core from the moment the socket is
armed; with card memory on they are the memory of record, and in native
mode each byte they give the core is compared with the socket's (the same
verify count). A stopgap for S7d's DDR3: 64 more blocks, 243 of 340.

- **Two banks, not four**, because the near memory's tag (`NEAR_KW = 4`:
  A14–A17) reaches bank 03 and no further. Four would want a wider tag
  and a new floorplan for the near blocks.
- **The machine must have banks 02/03 itself** (a memory card in a ROM 01
  machine). The card does not add memory the machine lacks.
- **A step in banks 02/03 is three or four clks, not two**: the first-clk
  cheap view covers banks 00/01 and the ROM only, so these steps are
  decided from the full decode. About 27 to 37 MHz against 55.
- Telemetry word 12 is `EXP_BANKS` (2 on this build, 0 on aoz and fx2).

**The far memories now have two clks in each direction.** The first build
with the 64 extra blocks failed 44 paths by up to 0.482 ns, everywhere:
the banks and the ROM image are 192 blocks all over the die, and three of
their paths were still single-clk, pulling the broker and the core apart.

| path | before | now |
|---|---|---|
| read data, blocks → the broker's `alt_q` | one clk | two: the broker waits a clk longer on a far read (`FAR_CLKS = 2`) and does not look at the data in the clk after its launch |
| RAM write, the broker's registers → blocks | one clk | through one more register per data bit in `gs_accel`: the write lands a clk later |
| read and write enables | one register to every block | a copy per data bit (`RA_COPIES = 8`) |

A step served by the near memory is unchanged. The late write is safe
because the broker cannot launch a far read until three clks and more
after it makes a write; the bench stops any run in which a read is
launched within two. In simulation the far data reads wrong in the clk it
is not yet valid, in the bench's model and in `gs_accel`, so a broker that
looked early would show. Cost: 60,000 steps of the test program take
9.455 ms against 9.444.

Verification: the suite is 102 runs, 14 of them with `gs_accel` holding two
banks and compared pin for pin with the bench's design in every clk
(native, paced, fast, toggling, with internal slot firmware, warm resets, a
machine with six expansion banks behind it, dense RDY stalls, and a ROM 01
boot): 247,013,471 clks; over all 49 runs with the module beside the
design, 535,896,565 clks. The test program runs code from bank 02 and, because
everything it wrote there stayed in the near memory, pushes those entries
out and reads the bytes back from the expansion blocks themselves, onto
the socket. Six mutations of the new logic, all detected (three were not
until that test existed). A bench fault found on the way: the final
comparison of the card's RAM could be taken in the clk the last step's
write was landing.

Timing, three placements of the committed source:

| build | failing | worst slack |
|---|---|---|
| fe1 (the far paths as they were) | 44 | −0.482 |
| **fe2, the committed source** | **0** | **+0.007** |
| fep1 (a telemetry constant changed) | 0 | +0.003 |
| fep2 (changed again) | 0 | +0.012 |

In fe2 the far data has 8.96 ns of slack on its two-clk path; the step
paths are where they were (core → core +0.21, near memory → core +0.08).
Size: 6,764 logic cells, 3,468 FF, 243 block RAMs. Build stamp
`20261112202241`, bitstream sha256 `6761a20a0f4d85f3…974a5dfb27`.

**On hardware** (flashed 21:10 the same evening; telemetry word 12 reads 2;
ROM copy sum `7EE8`; card memory at PHI2 pace for 17 s, about 24 M bytes
served and compared, verify count 0; then fast). The same benchmark:

| test | base | fx2, fast | fe2, fast | fe2 ratio | the program's MHz |
|---|---|---|---|---|---|
| 10 iterations of the sieve | 413 | 322 | 318 | 1.30 | 3.28 |
| selection sort of 500 strings | 1159 | 805 | 795 | 1.46 | 3.68 |
| 100 iterations of a floating-point test | 476 | 41 | 38 | 12.5 | 33.83 |
| 15,000 Gamm units | 1546 | 253 | 240 | 6.4 | 17.14 |
| 10 iterations of Fibonacci calls | 2020 | 1417 | 1401 | 1.44 | 3.64 |
| 5,000 iterations of integer math | 1564 | 863 | 853 | 1.83 | 4.63 |

The program's average went from 10.44 to 11.03 MHz: one to seven percent
per test. The two banks work (nothing wrong in 24 M compared bytes, a
clean run), and they are not where this program lives: with the program
waiting at its prompt the socket shows accesses to bank 04, twice in some
fifty samples, and none above it. Where a program is loaded on this
machine was the thing not known, and two banks do not reach it. Not
sampled while a test was running.

## 3b. Expansion memory on the DDR3, at 105 MHz (2026-11-13)

The benchmark of section 3a runs from bank 04: sampled at PHI2 pace (every
step on the socket), 70 % of the steps of its sieve, sort, call and integer
tests are in bank 04 and 30 % in bank 00; four of 1,062 samples are in bank
7F. Two banks of block RAM stop one short. This build puts every expansion
bank, 02 to 7F, in the board's DDR3 - S7d as the scoping doc has it.

**What is in the build.** The full build's DDR3: its PLL block
(`board_plls`, the clock chain calibration was proven with), its reset
sequencer and calibration watchdog, `DDR3_Memory_Interface_Top`,
`ddr3_ports` and `ddr3_port_cdc`, with the accelerator as the only client
(port 1 is tied off). In the accelerator's clock: `gs_accel_exp`, one queue
of writes and reads in program order, and in the top a byte-to-word
adapter (the arbiter's words are 32 bits; a read's byte lane is kept until
its word comes back). `gs_accel` is built with `EXP_DDR = 1`,
`EXP_BANKS = 126`, `NEAR_KW = 9`.

**The near memory tags every bank.** Its entry is the byte, the tag as it
was (two bits that say ROM and "A12 moved", and A14–A17), and A18–A22. The
first fourteen bits are read with the core's next address, in the same
fourteen blocks as every build before this one, and they are all the
first-clk decision needs. RAM above bank 03 is marked in the two bits that
were {ROM, A12 moved}: both set, a pair nothing else uses (ROM is {1, 0},
and nothing moves A12 above bank 03). A step of the cheap view is never in
those banks, so its entry matches exactly when the old fourteen bits match.
The five high address bits are in five blocks of their own, read a clk
later at the step's address - a register - and written a clk after the
rest of the entry; only the registered full compare uses them, and it has
the time. A first version spent a fifteenth bit ("an address bit above A17
is set") in the first-clk compare; this one costs that compare nothing.

**Waiting for the memory.** A near miss in an expansion bank queues a read
and waits for it: a fast step in place (the broker's count stands while
`lm_rwait_i`), about 25 clks in the bench's model. A PHI2-paced step cannot
stand still, so its socket cycle is not taken as complete until the byte is
there; the cycle - a read of RAM - is issued again meanwhile. It may not
take the socket's byte instead: after fast mode the machine's copy is
stale (a first version did, and a toggling run stopped at once). Writes go
into the same queue; when it is within six entries of full every fast step
that could write there waits at its strobe.

**Granted only when it is safe.** Card memory needs the DDR3 calibrated and
a queue that has never overflowed, as well as a ROM image. If the DDR3
does not come up, the card is S6 on the new core and stays so. Telemetry
word 12 is `{EXP_DDR, queue overflowed, memory ready, EXP_BANKS}`: `057E`
on a good start. The DDR3's sequencer state and retry count are in the
firmware's debug registers, as in the full build.

**The card's copy is what the core wrote since the socket was armed**, as
for banks 00/01. The DDR3 does not start at zero. In PHI2-paced card-memory
mode every byte the near memory gives is compared with the socket's, and
so is every byte fetched from the DDR3 (the verify count).

**A defect found on the way, never built into a bitstream that ran.** When
the queue was nearly full the broker held a fast step at its strobe. A
step whose socket request had been raised ahead (the early request of
section 3) was held too, its socket cycle completed with the step still
undecided, and the cycle would have been issued twice. The bench did not
fill the queue often enough to show it. It does now: `+wbforce` forces
"nearly full" in long bursts, and a check stops the run if a step's socket
cycle completes before the step is decided. Six of six such runs failed
before the fix and pass after it: a step whose request is already at the
socket is not held by the queue.

**Verification (simulation).** The broker suite, 140 of 140 runs;
the decode sweep (981,101,952 vectors), the replica benches and both
socket benches pass on the same source. The bench's
memory answers reads 8 to 39 clks late and is not always ready
(`gs_exp_mem_sim`; `+xlat`, `+xgap` make it slower than the core writes);
both the bench's design and `gs_accel` beside it have one, and the pins
are compared in every clk. New in the test program: banks 04 and 08
against bank 00 at one near-memory index; a 96-byte block move into bank
02; a reset dropped on a step that is waiting for its byte (`+resetx`).
Seven mutations of the new logic: six detected (the marking of high banks
among them); the seventh (a byte that comes back to a step a reset removed
is not dropped) changes nothing at any latency the DDR3 could have - the
byte is back before the reset is over - and stays as a safeguard. The S6
pin comparison is off in PHI2-paced runs of this configuration: a cycle
issued again is not S6. Nine runs are at 105 MHz (`b105_2s`).

**Timing: 110 MHz did not hold, 105 MHz does.** The rule is three
placements of one source with no failing path, against a clock uncertainty
of 0.5 ns (so "+0.03" below is 0.53 ns of real margin, and a small negative
number is a path that works with less than the margin asked for). Seven
rounds of the source:

| round | what changed | failing paths (worst slack, ns) in each placement |
|---|---|---|
| 1 | first build | 560 (−2.529) |
| 2 | the queue's "nearly full" and "waiting" as registers | 0 (+0.019), 97 (−0.281), 20 (−0.092) |
| 3 | the queue registered at both ends; high tag bits written a clk late | 0 (+0.018), 0 (+0.028), 88 (−0.986) |
| 4 | the {1, 1} marking in place of a fifteenth tag bit | 43 (−0.391), 26 (−0.152), 98 (−0.753) |
| 5 | the PHY's enable as a register; the queue in one block RAM | 11 (−0.031); the others not built |
| 6 | one write-enable register for the far memories; dead register copies removed | 80 (−0.513), 8 (−0.131), 0 (+0.038) |
| 7, 110 MHz | the defect's fix; cycle completion from registers a clk ahead | 140 (−1.788), 138 (−1.497), 1 (−0.075) |
| **7, 105 MHz** | **the same source, `ACC_CLK_105`** | **0 (+0.029), 0 (+0.037), 0 (+0.028)** |

At 110 MHz the same source lands anywhere from one path 0.075 ns short of
the margin to 140 paths 1.8 ns short, by placement. The DDR3's controller
sits on the left edge of the device and the socket on the bottom and lower
right; the accelerator is pulled between them, and its paths are mostly
wire (5 to 7 ns of 8.5). The failing families differ from placement to
placement (the core's two-clk loop through the near memory, the far
memories' write side, the PHY's enable to the broker), which is the sign
of a design at the edge and not of one bad path. So the build as committed
runs the accelerator at **105 MHz: 52.5 MHz steps**, from a second PLL
recipe (`gowin_acc_pll`, 50 MHz × 21 / 10), selected by `ACC_CLK_105` in
`accel_only_defines.v`; `gen_sdc.py` reads the same line for the period.
Comment the line out and regenerate the constraints for 110 MHz. The
block-RAM build of section 3a (on the card today) closed at 110 MHz and
is unchanged by this.

Changes made for timing that are in every build from now on, each exact
and checked in simulation where it is a register standing for a function:
the PHY's `enabled` is a register (asserted equal to the old function in
every clk); the ROM copy's two terminal comparisons are registered flags;
the broker's cycle-completion conditions are registered a clk ahead of the
PHI2 fall event (asserted equal at every fall event); the far memories'
write enables come from one register; the queue is one block RAM (this
device has no distributed RAM, so a small memory is otherwise built from
registers).

Size: 11,131 logic cells, 11,416 FF, 192 of 340 block RAMs (the two banks
of section 3a are gone; the near memory is 14 + 5 blocks; the queue is
one). Build k0: stamp `20261113103322`, bitstream sha256
`4aad21337ff394c4…b620fb41072f`.

**Not known, not done.** Whether the DDR3 calibrates in this build on the
card: nothing but the full build has run it. The socket PHY has run on
hardware only at 110 MHz; at 105 MHz its points are the same counts of a
clk 4.8 % longer (in simulation at 100, 105, 108 and 110 MHz). The round trip's real length.
What a program's working set does to a 16K-entry cache of single bytes when
a miss costs 25 clks and brings one byte (the arbiter returns a 32-bit
word, of which three bytes are dropped). Not run on hardware. (It has
been since: section 3c.)

## 3c. The DDR3 build on hardware: a crash, its cause, the benchmark (2026-11-13)

k0 was flashed at 14:14 with the IIgs off (first attempt, verified) and
the card power-cycled. ROM 01 machine, the a2p25 beside it, as section 3.

**What worked at once.** Telemetry word 12 reads `057E`: the DDR3
calibrated, the queue never overflowed, 126 banks. The socket PHY at
105 MHz: armed 22 ms after the slot's /RESET rose, no stall. Key and copy:
one copy, sum `7EE8`.

**The first switch to card memory crashed the machine.** GS/OS was up and
the benchmark at its prompt. Register 29 = `0x0A` (card memory at PHI2
pace, verify on): inside two seconds the verify count was at its ceiling
(65,535), the core was executing in bank 67, the keyboard dead. Back to
`0x08` (socket bytes; fast mode had not run) and a reset.

**What the monitor cleared.** From the IIgs monitor, with card memory on
for the reads: a 64K fill of bank 02 and its dump; single bytes in every
lane of the DDR3's word in banks 02, 04 and 08; a bank of `EA` executed
end to end (65,535 steps) and a bank of `AD` (21,845 instructions) in bank
04. Nothing wrong. Then GS/OS again with the benchmark at its prompt, in
card memory at PHI2 pace: eleven short pulses, then 7 s, 30 s and 240 s
without a mismatch. So the DDR3, the queue, the byte lanes and the tagging
of the high banks are good on hardware, and the crash did not come back.

**The cause: an entry that held nothing carried the tag of RAM at
7F/C000-FFFF.** An empty near-memory entry was marked by a tag of all
ones. That was no byte's tag while the tag held A14-A17: RAM has a zero in
the first of its two leading bits, ROM in the second. Section 3b marks RAM
above bank 03 with both set, and all ones became a real tag: {1, 1} and
A14-A22 all ones is the top 16K of bank 7F. The entries are empty at
power-up (the block RAM's zeros, stored inverted) and after a ROM copy,
which empties all 16,384. A read of 7F/C000-FFFF then found its index
empty and took the empty entry's byte for the memory's. GS/OS keeps code
and data at the top of the last bank (the socket showed `7F/C363` in use),
so the first card-memory run after the copy went wrong at once. The
monitor's fills had rewritten every one of the 16,384 indices with real
entries, which is why nothing failed afterwards: the fault needs an entry
that is still empty.

The bench did not have it. Its expansion memory stopped at 16 banks, its
runs used 8, and an empty entry's byte was 00, the same as memory nothing
had written.

**The fix.** The empty entry has a tag of its own (`NM_EMPTY` in
`gs_accel_broker`): with the split tag, {1, 1} and A18-A22 all zero, a
combination no byte has, because {1, 1} is only ever written for RAM with
one of those bits set. (Without the split tag it stays all ones.) The
broker writes it on a flush and for a byte the tag cannot name. In
`gs_accel` the five high address bits are now stored as they are and the
fourteen others inverted as before, so a block RAM's power-up zeros read
as exactly that tag. No logic is added; the first-clk compare is as it
was.

In the bench: the expansion memory reaches bank 7F (`EXP_MAX = 126`); an
empty entry's byte is `EE`; one byte of bank 7F holds `D9` in every memory
model before anything runs. The test program has T24: it reads that byte
(nothing has written it), writes and reads 7F/E100 against bank 00's
6100 at the same index, and reads both again on every pass of its loop,
so a ROM copy in the middle of the run (`+recopy`) empties the near memory
under them. Two builds have all 126 banks at 105 MHz (`b105_2t`, and
`b105_2k` with the ROM image empty for `+mode=copy`), `gs_accel` beside
the design in both. And in every simulation build the broker checks at
elaboration that the empty tag is no RAM page's and no ROM page's, for the
parameters it is built with.

Before the fix, on those builds: PHI2-paced, fast and toggling runs fail
at the read of the unwritten byte (the broker delivers the empty entry's
`EE`), and the copy-again run fails at 7F/E100 in the first loop pass
after the second copy ("the broker delivered 00, the ideal machine holds
7e") - the crash of this section. With the broker fixed and `gs_accel`'s storage left as it
was, the pin comparison fails (`gs_accel` answers 00 out of an entry its
block RAM powered up with). With the fix, the broker suite passes 152 of
152 runs (the 140 of section 3b, eight of T24's on the two new builds, and
a ROM 01 boot with 126 banks in native, paced, fast and copy mode: the
step hash `6012b76169ecc820` and the 128,393-write stream of every other
boot). The decode sweep (981,101,952 vectors), the replica benches and
both socket benches pass.

**The benchmark on k0** (the bitstream with the defect, after the
monitor's fills, so with no empty entry left; fast mode on at 15:10:41).
The program was run twice, both passes in fast mode. 60ths of a second;
"machine" is the base pass of section 3, "fe2" the block-RAM build of
section 3a:

| test | machine | fe2, fast | k0, first pass | k0, second pass | against the machine | against fe2 | the program's MHz |
|---|---|---|---|---|---|---|---|
| 10 iterations of the sieve | 413 | 318 | 33 | 33 | 12.5 | 9.6 | 31.59 |
| selection sort of 500 strings | 1159 | 795 | 88 | 88 | 13.2 | 9.0 | 33.22 |
| 100 iterations of a floating-point test | 476 | 38 | 35 | 35 | 13.6 | 1.09 | 36.73 |
| 15,000 Gamm units | 1546 | 240 | 130 | 130 | 11.9 | 1.85 | 31.63 |
| 10 iterations of Fibonacci calls | 2020 | 1401 | 158 | 158 | 12.8 | 8.9 | 32.24 |
| 5,000 iterations of integer math | 1564 | 853 | 115 | 114 | 13.6 | 7.4 | 34.61 |

The program's average is 33.34 MHz (fe2: 11.03). The four tests that run
the program's own code in bank 04 are nine times faster than on fe2, which
took every one of those steps from the socket; they are now within ten
percent of the floating-point test, whose work is in the ROM and bank 00
and was the card's already. One tick is three percent of the short tests.
At the program's prompt the core steps 15.4 M times a second and takes
0.45 M socket cycles a second of the machine's 1.68 M; 9.5 M reads a
second are answered by the near memory and 0.5 M are not. The queue in
front of the DDR3 did not overflow (word 12 still `057E`).

**The fixed source, built.** Three placements at 105 MHz, no setup or hold
violation in any (worst slack +0.024, +0.002 and +0.018 ns; the tool
stops when the constraint is met). Size as k0 (11,050 logic cells, 11,416
FF, 192 block RAMs). Build m0: stamp `20261113175022`,
bitstream sha256 `aaa7ca899b7fcd5a…bdfa95db0a7fa73711`. (On k0 a ROM
copy or a power cycle leaves empty entries again; a 64K fill from the
monitor after the copy and before card memory goes on,
`00<02/0000.FFFFZ`, rewrites every index - the card follows every write -
and is what the run above rested on.)

**The fixed build on hardware** (m0 flashed 18:17 to 18:21 with the IIgs
off, first attempt, verified; the card power-cycled). Word 12 `057E`, no
stall, copy sum `7EE8`. Then the step that crashed k0, with nothing
written from the monitor: GS/OS up, the benchmark at its first prompt,
card memory at PHI2 pace straight after the copy for 5 s and for 60 s -
verify count 0, no mismatch address. The benchmark's base pass was run on
socket bytes (register 29 = `0x08`), so it is the machine's; at the
program's prompt for the speed, card memory for 20 s more (verify count
still 0) and fast mode at 18:34:35:

| test | base | m0, fast | ratio | the program's MHz |
|---|---|---|---|---|
| 10 iterations of the sieve | 413 | 33 | 12.5 | 31.59 |
| selection sort of 500 strings | 1159 | 88 | 13.2 | 33.22 |
| 100 iterations of a floating-point test | 476 | 35 | 13.6 | 36.73 |
| 15,000 Gamm units | 1546 | 130 | 11.9 | 31.63 |
| 10 iterations of Fibonacci calls | 2020 | 158 | 12.8 | 32.24 |
| 5,000 iterations of integer math | 1564 | 114 | 13.7 | 34.61 |

The program's average is 33.34 MHz, and every time is k0's: the fix adds
no logic and costs nothing. The base pass is the base pass of section 3
to the tick. Rates at the prompt as on k0 (15.4 M steps a second, 0.45 M
socket cycles). The build has no stamp that can be read over telnet: that
m0 is what runs rests on the verified flash, the power cycle (the verify
count, which only a reconfiguration clears, was back at 0) and the step
that no longer fails. No access to bank 7F was among the socket samples
taken during the 65 s, so the bench (T24) and the benchmark are the
evidence for that RAM, not those seconds alone.

Entered as 52.5 MHz, the program expects the sieve in 19.9 ticks and gets
33; its average of 33.34 MHz is 64 % of the step rate. What the rest is
spent on (socket cycles, near misses that bring one byte from the DDR3)
is not measured yet.

**Not known, not done.** A run longer than
the benchmark. The round trip's real length, and what a larger working
set does to the 16K entries. The DDR3's power-on self-test writes sixteen
bytes of its pattern at 40/0000 before the card is armed; nothing reads
them back before writing, as far as is known, and they are still there.

## 3d. Where the step rate goes (2026-11-13, m0 on the card)

The benchmark's 33 MHz against 52.5 MHz steps, measured: the card's
counters (telemetry words 0 to 5 and 11) logged over the console 2.4 times
a second while the program ran ten rounds twice, both passes in fast mode
(20 rounds, 567 samples inside the run). The tests show as plateaus; the
two shortest (sieve, 0.55 s; floating point, 0.58 s) are a sample or two
each and the sieve is not resolved.

| test | steps, M/s | the program's MHz | socket cycles, M/s | reads not in the near memory, M/s | clks per step |
|---|---|---|---|---|---|
| selection sort | 32.7 | 33.22 | 0.008 | 0.007 | 3.21 |
| Fibonacci calls | 31.8 | 32.24 | 0.008 | 0.003 | 3.30 |
| integer math | 33.9 | 34.61 | 0.008 | 0.002 | 3.09 |
| floating point (10 samples) | 35.4 | 36.73 | 0.204 | 0.376 | 2.97 |
| Gamm units | 31.1 | 31.63 | 0.193 | 0.579 | 3.38 |
| at the program's prompt | 15.4 | | 0.453 | 0.498 | 6.83 |

**The program's MHz is the step rate**, test for test, within four
percent. To pass 48 on this program the core has to step 48 M times a
second while it runs.

**In the three tests that only compute, the loss is one thing: a step in
banks 04 to 7F takes four clks, not two.** They take 8,000 socket cycles a
second (about one percent of the time) and miss the near memory a few
thousand times a second, and still average 3.1 to 3.3 clks a step. The
bench says why (`---- step lengths`, new in `tb_gs_broker`, with code and
data in bank 04 called from the test program's loop): a read or a write in
banks 04-7F that hits the near memory completes in exactly 4 clks, every
time, where a step of the cheap view completes in 2. Section 3b chose
that: the five high tag bits are read a clk after the rest and only the
registered compare sees them, so no step in those banks is the cheap
view's. With 2 clks for a step elsewhere and 4 there, 3.1 to 3.3 clks a
step is 55 to 65 % of the steps in bank 04 and above - the program's code
and data (section 3b sampled 70 % of its steps in bank 04). Of the time:
62 % is the two clks every step has, 37 % the two more on steps in the
high banks, 1 % socket cycles.

**In the two floating-point tests** 41 % (Gamm) and 33 % of the time is
not a two-clk step: 0.2 M socket cycles a second and 0.4 to 0.6 M reads a
second that are not in the near memory. The counters cannot split the two
(no counter of clks, only of events; a fit over the two tests gives about
80 clks for a socket cycle and 50 for a miss, from ten samples of one of
them). What the socket cycles are is not known; a toolbox call goes
through bank E1, the machine's, and that is the first guess.

**At the prompt** the program polls the keyboard: 0.45 M socket cycles a
second, each a cycle at the machine's slow rate.

**What follows.** A two-clk step for a near-memory hit in banks 04-7F
would take the three computing tests from 3.2 to about 2.05 clks a step:
some 51 M steps a second, 51 on the program's scale. It needs the high
tag bits in the first-clk decision (or one bit that stands for them), and
that compare is the path the 105 MHz build closes by 0.02 ns; a three-clk
step there (35 MHz in those banks, about 40 M steps a second on these
tests) is the smaller change. For the floating-point tests: counters of
clks by kind of step, to split socket cycles from misses before choosing.
The first is written (next paragraph); the rest is not done.

**The two-clk step in the expansion banks: written, verified in
simulation, not durable in timing (2026-11-13, evening).** The tag is
whole again (`NEAR_SPLIT = 0`, now a parameter of `gs_accel`; the split
tag of section 3b is kept as the other value): data, {ROM, A12 moved} and
A14-A22, nineteen blocks, all read with the core's next address. The
cheap view (`gs_mmu_decode` `fast_exp`) takes a read or a write in any
bank the card backs while only banks 00/01 shadow (CYAREG bit 4 clear):
plain RAM at its own address. The five tag bits this adds to the first-clk
compare are compared against the address register directly, gated by
A23, not through the cheap view: a RAM step's high bits are the
address's, a ROM step's are zero and its bank has A23 set or is 00/01.
Where expansion steps are the cheap view's, every cheap step waits for
room in the expansion memory's queue. The empty entry is all ones again.

In the bench a hit in banks 04-7F is two clks (the test program's bank 04
code: 51,528 two-clk steps where the split tag had 40,093, no four-clk
read left). 155 runs pass, the ROM 01 boot with 126 banks at the same step
hash; the decode sweep has 85,994,305 expansion vectors (the physical
address, no ROM, no fold, the bank's own bit 0) and no failure; replica
and socket benches pass. Seven mutations of the new logic, all detected -
one (the bank bit taken from RAMRD in an expansion bank) only after the
sweep gained a check and the test program a write in bank 04 with RAMRD
and RAMWRT set.

Timing, with all nineteen blocks placed (R46 / R64, sites 16 to 22): the
rule is three placements with no violation, and it is not met.

| clock | placement 0 | placement 1 | placement 2 |
|---|---|---|---|
| 105 MHz | +0.002 | 73 paths, -1.069 ns | +0.002 |
| 100 MHz (`ACC_CLK_100`, new) | +0.012 | 25 paths, -0.743 ns | +0.034 |

The failing placement is the same perturbation at both clocks, and what
fails in it is not the new logic alone: three-level paths in the PHY and
the ROM copy with 7 ns of wire, the near memory to the core, the core to
the near address. A first build with three of the blocks left to the
placer (it put them in R28) and the high tag bits through the cheap view
failed by 0.79 ns on 95 paths. So: the design was at the edge with
fourteen near blocks, and nineteen tip some placements over; 5 % of clock
does not buy it back. Not flashed. Ideas not tried: a ten-bit tag (ROM
coded as "A12 moved" with A22, eighteen blocks, no site 16); a three-clk
step for these hits on the split tag's fourteen blocks (about 40 M steps a
second on the computing tests where two clks would give 51 M).

**A defect found on the way, in the source only so far.** The bank 04
code changed which step a second ROM copy lands on in the bench's
copy-again run, and that run failed: the step that was held for the copy
read its byte from the socket. `gs_accel_romcopy`'s `valid_o` drops when
a copy starts, the grants of fast mode and card memory dropped with it,
and the one step held across the copy ran as a socket step - of a machine
whose RAM is stale once the card's memory has been the memory of record.
A first copy is not affected (nothing has been granted yet), and neither
is anything this page's ladder does; it takes a second copy asked for in
fast mode. Now the grants stand on `usable_o`: a complete image, or a
complete image being copied again (nothing is written before the core is
held, and it is held until the copy is complete). In the bench the second
copy can be asked for at any step (`+recopyat`): 45 points on two builds
pass, two of them fail without the change, and three are in the suite
(155 runs, all passing). **m0, on the card, has the defect; the change is
not built.**

## 3e. The near memory as lines of sixteen bytes (2026-11-13/14)

The DDR3 answers every read with sixteen bytes, and the near memory of
sections 3b to 3d kept one of them. It also kept an 11-bit tag beside
every byte: 19 bits a byte, nineteen blocks for 16 KB, and nineteen blocks
on the core's next address are what the two-clk step of section 3d could
not carry on every placement. This build (`NEAR_LINE` in the broker and
`gs_accel`; the default of the board top, `ACC_BYTE_CACHE` in the defines
goes back) makes the near memory lines.

**What it is.** 1,024 lines of 16 bytes, one tag for a line: eight blocks
of 1K x 16 for the eight words of a line, one block of tags. Nine blocks,
with ten or eleven address bits each where the entries had nineteen with
fourteen. A hit is as it was: the tag read with the core's next address,
compared whole in the step's first clk. Each data block's read port is a
byte wide (the primitive's ports need not be one width) and gives its
word's byte; which word is picked by address bits 3 to 1, a register.

**A miss brings the line.** Every far read is of sixteen bytes:

- the DDR3's through the arbiter's burst read, which is one read of the
  memory handed over as its four words (the top collects them; the queue,
  `gs_accel_exp` with `LINE`, is otherwise as it was, writes one byte each);
- the block memories' (banks 00/01, the ROM image) because they are now
  sixteen bytes wide: for each word of a line as many blocks of 1K x 16
  as there were blocks of 16K x 1 for a bit, a byte write being one byte
  of one of them. The same 64 + 64 blocks.

The broker takes its byte as before and the line is written into the near
memory in one clk, the clk before the step ends, through a register (the
far memories' two thousand outputs are muxed where they are; 128 bits go
to the blocks beside the core).

**A line is there whole or not at all, and always equal to the far
memory.** A write always goes to the far memory. In the near memory it
changes its byte if the line is there - the tag read at the launch of the
write's own step says so - and does nothing if it is not: there is no
write that installs. Two cases empty the line instead: the tag read at the
launch is not to be trusted (it was written at or since the launch), and
the byte lives under another index ($Cxxx of banks 00/01, kept under the
$Dxxx index as before). A read launched in the clk of, or before, any
write to its line is not used. The ROM copy empties 1,024 tags.

**`gs_bram16be`.** Written as an array with two write enables, a 1K x 16
memory became two blocks under GowinSynthesis, one a byte: the first build
used 318 of the 340. The primitive (SDPB) has the byte enables in the low
address bits of its write port; the module instantiates it and simulation
has the array.

**In the bench** (the test program, 126 banks, fast mode, against the
whole-tag byte memory of section 3d): 545 far reads where there were
4,213; 249 reads of the expansion memory where there were 2,338; 98.6 %
of local steps two clks. The ROM 01 boot fills 225 lines where it filled
2,433 bytes and reaches the same step hash with the identical write
stream, on the DDR3 build and on two builds with block RAM alone.
176 runs pass (21 new), the decode sweep and the other benches are as they
were. Eight mutations of the line logic, all detected - one (a write made
in the very clk the next read of its byte is launched) after the test
program gained self-modifying code in a page whose writes are not the
cheap view's ($6200).

**Not in any bench:** the top's collecting of the four words (the order of
the arbiter's beats is read from its source, `ddr3_ports`). On hardware
the PHI2-paced card memory of the ladder checks it: every byte the DDR3
returns is compared with the machine's. A PHI2-paced step fills no line
(a line is filled by a fast step), so in that mode every read of an
expansion bank goes to the DDR3 - slower, and a longer test of it.

**Timing: three placements at 105 MHz with no violation, after six
rounds.** The array form (318 blocks, no floorplan) failed by 7.6 ns.
With the primitive and the nine blocks placed (R46 / R64 sites 18 and 19,
the tags in R46[20]) 34 paths failed by up to 0.18 ns and the build took
half an hour. Then the fill register, the byte-wide read port and a copy
of the far write data for each word: 14 paths, 0.18 ns. Then the first-clk
compare in two registers (`fh_q`: the tag's address bits against the
address register; `fx_q`: the four bits the cheap view makes) and the
verify copy of the card's byte declared the two-clk path it is: two
placements of three clean, the third 1.31 ns short on 97 paths; the same
source at 100 MHz (`ACC_CLK_100`), two of three, the third 0.22 ns short.
That one named the cause: its worst path, the tag block to the compare,
was 2.0 ns of logic and 5.6 ns of wire - the tags were in R46 beside the
core and the broker is placed below it, twenty rows away. With the tag
block in R64[20], on the core's second copy of its next address like the
odd words there:

| placement | 0 | 1 | 2 |
|---|---|---|---|
| worst slack, ns | +0.000 | +0.002 | +0.006 |

(The tool stops when the constraint is met.) 13,088 logic cells, 11,842
FF, 182 of 340 block RAMs. Build t0: stamp `20261114020946`, bitstream
sha256 `dd5d16229b2438a8…6339edea96909e7720`. **Flashed 2026-11-14
08:50 to 08:53, IIgs off, first attempt, write verified; section 3f is
its run on the machine.** It carries
everything since m0: the line memory, the two-clk step in every bank the
card holds, and the ROM copy's grant across a second copy (section 3d).
What to expect of it on the benchmark is section 3d's arithmetic - about
2.05 clks a step where the computing tests have 3.2; section 3f has what
the machine gave.

## 3f. The line memory on the machine (2026-11-14, t0 on the card)

Flashed 08:50 to 08:53 with the IIgs off, the card power cycled, ROM 01.
The ladder of section 3, in order:

| step | result |
|---|---|
| DDR3 | word 12 = `057E` |
| ROM copy (register 29 = `08`) | one copy, sum `7EE8` |
| card memory, paced, verify (`0A`), 5 s | no mismatch |
| the same, 60 s | no mismatch; the last bus address seen in banks 00, 02, 03, 04, E0, E1, FE, FF |
| the same, 4 s, then fast (`0B`) at the benchmark's prompt | no mismatch; the machine runs on |

Paced card memory fills no line, so every read of bank 04 in those holds
went to the DDR3 through the top's four-beat line assembly and was
compared with the machine's byte. That assembly has no bench; this is
its first test, and it is a narrow one (what the machine read in 70 s at
a prompt and through one base pass's start).

The benchmark's fast pass (one round, 52.5 entered) ran to its report.
Counters over the pass, sampled each 0.42 s:

| | |
|---|---|
| the pass, first test to report | 8.74 s |
| core steps | 315.1 M = 36.05 M a second = 2.91 clks a step |
| socket cycles for the core | 1.564 M |
| near memory hits / misses | 202.3 M / 0.359 M |
| best samples (no socket cycles) | 45 to 49 M steps a second = 2.15 to 2.33 clks a step |
| samples with socket cycles (0.2 M a second) | 30 to 37 M steps a second |
| verify mismatches, word 7 | 0 |

So the computing stretches run at 45 to 49 M steps a second where m0 had
31 to 34 (section 3d), and the stretches with socket cycles are where m0
had them, about 35. One round is too short to give each test its own
rate: a test lasts one to six samples. At the prompt the rate is 15.6 M a
second as before (the prompt's loop is socket cycles).

The program's report for the same pass (TML Pascal benchmark, one run,
times in its ticks; the base pass with register 29 = `08`, the fast pass
with 52.5 entered; m0's column is section 3c's):

| test | base | t0 | m0 | t0 against base | "Actual MHz" t0 | m0 |
|---|---|---|---|---|---|---|
| sieve, 10 iterations | 413 | 21 | 33 | 19.7 | 49.65 | 31.6 |
| selection sort, 500 strings | 1159 | 60 | 88 | 19.3 | 48.72 | 33.2 |
| floating point, 100 iterations | 476 | 34 | 35 | 14.0 | 37.81 | 36.7 |
| Gamm, 15000 units | 1546 | 115 | 130 | 13.4 | 35.76 | 31.6 |
| Fibonacci calls, 10 iterations | 2020 | 110 | 158 | 18.4 | 46.31 | 32.2 |
| integer math, 5000 iterations | 1564 | 86 | 114 | 18.2 | 45.88 | 34.6 |
| the program's average | | | | | **44.02** | 33.34 |

(m0's "Actual MHz" here is 52.5 x expected / measured from its times, as
the program computes it.) Two tests are above 48 and the average is 84 %
of the 52.5 MHz step rate, where m0 had 64 %. The two floating point
tests are the ones with socket cycles in the counters above, and they
hold the average down: 34 and 115 ticks where two clks a step would give
24 and 78.

## 3g. What the floating point tests wait for (2026-11-14, t0 on the card)

A second run, ten rounds, the card in fast mode throughout. The program's
times: sieve 20.8, sort 59.4, floating point 33.9, Gamm 115.1, Fibonacci
109.8, integer 85.6; average 44.24 MHz. (Its base column in that run is
not a pass the counters saw - one pass of 88 s is all that ran - and
means nothing.)

During the run the console read the address of the last cycle the card
put on the socket about ten times a second (`last_addr` in `gs`), with
the telemetry words between. A park cycle is a read of `FF/0000`, so
bank FF in a sample is an idle socket and any other bank is a cycle of
the core's. The three address bytes are read one after another and come
from different cycles: the bank is a fair sample of where the socket's
time goes, a whole address is not.

| stretch | steps, M/s | socket cycles, M/s | misses, M/s | PHY cycles, M/s | bank of the sample |
|---|---|---|---|---|---|
| Fibonacci, integer (18 s, 168 samples) | 45.8 | 0.008 | 0.001 | 2.83 | FF 100 % |
| floating point, Gamm (13 s, 120 samples) | 34.9 | 0.219 | 0.132 | 2.31 | FF 74 %, E1 21 %, E0 5 % |
| between tests (15 s, 136 samples) | 28.4 | 0.288 | 0.014 | 2.11 | FF 60 %, E1 38 %, E0 2 % |
| the prompt (220 s, 2,040 samples) | 15.6 | 0.461 | 0.183 | 1.63 | FF 41 %, E1 31 %, E0 26 % |

What it says:

- **The core's socket cycles in the floating point tests are in banks E1
  and E0, and nowhere else.** Those are the machine's slow memory: the
  machine stretches each such cycle to its 1 MHz side. From the PHY
  count, an idle cycle is 351 ns and a cycle of the core's in these
  tests 1.2 us.
- **They are the whole of what the floating point tests lose.** The
  socket is the core's for 26 % of the time (the samples), 0.26 s in a
  second (the counts), and the core also waits for the idle cycle in
  progress before each one. 105 M clks less 30 % leaves 2.1 clks a step
  for the 34.9 M steps: the same step the computing tests have.
- The page and low bytes that are not a park cycle's fall in page 00
  (`00` to `03`, `64` to `6A`, `C0`, `C1`, `DE`, `EF`, `FF`), page 03
  (`C0`, `C8`, `CA`) and page 1D. That is the look of the tool
  dispatcher: `JSL $E10000`, the vector there, the tool locator's
  pointers at `E1/03C0` and up, a table in `E1/1Dxx`. SANE is a tool
  set, and every floating point operation is a tool call. It is a
  reading of torn samples, not a trace.
- Between tests the cycles are spread over `E1/2000` to `9FFF`: the
  program drawing on the super hi-res screen. Not in any test's time.
- The computing tests take 2.29 clks a step with no socket cycle and no
  miss to speak of: one step in seven or so is longer than two clks, and
  the counters do not say which.

Two things would raise the average, then:

1. The card holding a copy of the RAM of banks E0/E1 and answering
   reads from it, writes still going to the machine. The replica already
   has the Mega II's side of the map (`gs_mmu_decode`, `slowram_*`), the
   posted write already exists, and paced card memory would verify the
   copy as it verifies banks 00/01. It makes memory fast that is slow on
   the machine and on a TransWarp GS: a decision, so a request bit. By
   the arithmetic above the two floating point tests would come to 24
   to 26 and 81 to 85 ticks and the average to about 49.
2. Finding the steps of plain computing code that take more than two
   clks (the bench has a histogram of step lengths).

### A second benchmark: A2Desktop's (the same day)

A2Desktop's benchmark desk accessory (`src/desk_acc/benchmark.s` in
a2stuff/a2d) reads the card at about 36 MHz. Its loop counts passes in
one video frame, and one pass is 176 cycles of plain code (a counter and
a 32-turn spin, put there so that a IIgs's slow read is a small part)
and **one read of `$C019`**, with 97 passes a frame taken as 1 MHz. On
the card that is 176 local steps and one socket cycle in I/O space,
which the machine fits to its 1 MHz side. So a pass can only be a whole
number of the Mega II's cycles (0.98 us): 176 steps at 52.5 MHz are
3.35 us, the read takes the rest of five cycles, 4.90 us, and the frame
holds 3,406 passes: 35.1. Four cycles would read 43.9 and want the 176
steps and the read inside 3.92 us, about 62 MHz of steps; six would
read 29.3. The reading therefore says nothing fine: a band of step
rates some 15 MHz wide reads 35.1, from the mid or high 40s to about 60
(where the band's ends are depends on the fixed part of the read, which
is not measured: 0.6 to 1.4 us is the range considered). This is arithmetic
from the source and the counters above, not a measurement of the loop:
with the accessory open the card shows the core's socket time in bank
00's I/O page (`C019` the most common low byte), each such cycle about
1.8 us of socket time.

### What the machine itself offers for slow memory (the same day)

Looked up after the two benchmarks, in the Apple IIGS Hardware Reference
(second edition, chapter 2) and Apple IIGS Technical Notes #70 and #72:

- **No switch makes banks E0/E1 fast.** "Because the memory controlled
  by the Mega II contains the display buffers, it always runs at the
  1.024-MHz speed." A Mega II cycle is taken for "all external and most
  internal I/O operations", shadowed video writes, inhibited memory
  accesses and "Mega II memory accesses to banks $E0 and $E1".
- **The machine's own answer is shadowing**: a fast copy in banks 00/01
  that is read at full speed, with each write repeated into E0/E1. The
  Shadow register (`$C035`) chooses the regions - the text pages, the
  hi-res pages, super hi-res, the auxiliary hi-res pages, and I/O with
  the language card - and bit 4 of the Speed register (`$C036`) extends
  it to every RAM bank ("for proper operation of the Apple IIGS
  operating system, this bit must always be set to 0"). QuickDraw II
  uses it when asked: bit 15 of the MasterSCB given to `QDStartUp`
  "requests that QuickDraw II use the memory at $012000 to shadow onto
  the real screen memory at $E12000", which "speeds up QuickDraw II,
  since memory in bank $01 can be read at full speed" (TN #72). The
  regions are video and I/O only: nothing shadows `E1/0000` to `1FFF`,
  where the tool dispatcher's vectors and tables are.
- **Some I/O is fast by design**: "Certain registers internal to the FPI
  (the DMA register, the Speed register, and the Shadow register) are
  read and written at high speed. Similarly, reading the interrupt ROM
  addresses ($C071 through $C07F) does not slow the system. In addition,
  two registers (the State register and the Slot ROM Select register)
  ... are written at 1.024 MHz and read at 2.8 MHz". `$C019` is not
  among them. On the card these all go to the socket, where the machine
  gives them a fast cycle; the replica holds the same registers, so
  their reads could be answered on the card.
- The cost of one slow cycle is the manual's: up to 1 us of waiting
  (0.5 average) for a single Mega II cycle, and consecutive ones at
  1.024 MHz. TN #70 describes the notches of the A2Desktop reading:
  "one loop writing data to the graphics memory may run at the same
  speed as a second loop with a higher cycle count".
- The ZipGS's registers (`$C058` to `$C05F`, from the 6502bench symbol
  file) have a language card cache bit, delays for slots, speaker,
  paddles and counters, and nothing for E0/E1. What the TransWarp GS
  does there was not found in a document that could be read (its
  manuals are scans, its programmer's reference did not load).

So a card copy of E0/E1 RAM for reads is the machine's shadowing idea
carried to memory Apple did not shadow. It has no official switch.

## 3h. A copy of the Mega II's RAM on the card (2026-11-14, built and simulated; timing not closed; not flashed)

Section 3g: what the floating point tests lose is socket cycles in banks
E0/E1, the machine's slow memory, most of them the tool dispatcher's
reads. The machine's own answer to slow memory is a fast copy that is
read while writes go to both (shadowing, section 3g), and this is that,
for the RAM of E0/E1 itself. Ed's decisions: the copy in the DDR3 behind
the ordinary near memory, not in block RAM; and a switch that can be
turned on and off while the machine runs, to be a setting of the OSD.

**What it is.** The card keeps a copy of the Mega II's RAM below `$C000`
of banks E0/E1 - 96K: no I/O, no slot space and no language card are
there - in the DDR3's first 128K, which the 126 expansion banks leave
free (the card's banks 00/01 are block RAM). To the far path, the fills
and the near memory it is two more banks of expansion memory; its lines
have tags of their own, taken from codes the ROM's tags leave unused, so
the tag is no wider and the step's first clk has nothing new in it but
one term of the socket-only test.

**The copy follows the machine in every mode, whatever the switch says:**

- a write to E0/E1 is the socket cycle it always was, and its byte goes
  to the copy when that cycle completes (as a native step's write does:
  a step that /RESET cuts short writes neither);
- a write the FPI shadows from a bank the card holds - stored there and
  posted to the socket, as before - is repeated in the copy at E0 + the
  low 17 bits of its address, a clk later (a step is two clks and no
  read is launched within two clks of a write, so that clk is free even
  when the write is the expansion memory's own: a shadowed write in an
  expansion bank, `CYAREG[4]`). If the near memory holds that line of
  the copy it gets the byte too: the same index, the same byte of the
  line;
- a write the FPI shadows from a bank the card does not hold is a socket
  cycle and a write of the copy alone.

**The switch only says where a read comes from** (request bit 4, SLOW):
the socket, as before, or the copy - a local read like any other, a near
hit (four clks: not the cheap view's two) or a far read that brings its
line from the DDR3. The broker takes it when the core steps. Neither
the machine's RAM nor the copy is ever stale, so it may change at any
step, in either direction, any number of times. That is unlike FAST,
which leaves the machine's banks 00/01 behind and cannot be taken back
without a reset.

It is granted when card memory is in effect, the DDR3 was ready at the
machine's last reset, and nothing has been lost from the DDR3's queue
since (telemetry word 13). What the machine's RAM held from before that
reset the copy does not have: after a power-up that is RAM nothing has
written.

**Checking it on the machine** is paced mode with the switch on
(register 29 = `1A`): every read of that RAM is then the card's and is
compared with the byte the socket returns, as banks 00/01 are in `0A`.
Mismatches in E0/E1 have a count of their own (word 13) and the address
of the last (word 14). Unlike the count for banks 00/01 it means
something at any time, also after fast mode has run. While such a step
waits for the DDR3 its socket cycle, a read of RAM, is issued again, as
an expansion bank's is.

What is not in it, and was said to be when the switch was proposed: a
comparison while the switch is off in fast mode. It would need a far
read beside each socket cycle, in the sequencer's tightest part.

**Departures from the machine, all with the switch on:** reads of E0/E1
below `$C000` take four clks or a DDR3 read where the machine takes a
cycle of its 1 MHz side - software that times itself by the slow memory
sees it; and the machine no longer sees those reads at all (park cycles
in their place). Writes, `$C000` and up, and everything with the switch
off are as before, cycle for cycle. One exposure is the posted write's:
a shadowed write that /RESET drops from the buffer is in the copy, as it
is in the card's bank 00/01, and not in the machine.

**The model it rests on.** Which byte of the Mega II's RAM an access to
E0/E1 or a shadowed write reaches is the replica's decode
(`gs_mmu_decode`, the MiSTer core's), with the bank latch and the IIe
switches. Nothing on the card depended on that part of it before. It
agrees with the bench's machine, which is the same model; the machine
itself checks it in paced mode. One case where the Hardware Reference
reads differently from the model: a shadowed write to bank 01 with the
bank latch off, where the model has the Mega II take the bank bit and
the manual has it ignore it.

**Simulation.** The board-configuration builds of the broker bench
(`b105_2t`, `b105_2k`) keep the copy in every run and compare it with
the ideal machine's Mega II RAM at the end, and gs_accel's with the
design's; the `k_`, `j_` and `m_*_c` runs read from it (`+slow=1`) or
switch the request through the run (`+slow=2`): every mode, resets,
stalls, the DDR3 slow and its queue held, the ROM image read through the
socket, the card backing no bank, two and 126. The test program has a
test of every way into that RAM (T25: a shadowed write with the copy's
line in the near memory and without, a read-modify-write in the bank,
the bank latch off with the IIe switches, ALTZP, a block move over
itself). As everywhere in this bench, every byte delivered to the core
is compared with the ideal machine's and every socket cycle with the
cycles the program owes.

Results, on the source of the last timing round (y, below): the broker
suite 210 of 210 runs; in the ROM 01 boot of 3,000,000 steps the runs
with the copy read (`m_fast_c`), switched through the run (`m_fast_sw`:
the request changed 6,507 times), paced with it (`m_paced_c`) and with
the ROM copied (`m_copy_c`) reach the native run's point (step hash
`6012b76169ecc820`) with its stream of 128,393 writes; the test program's
six `j_` runs likewise. `gs_accel` beside the bench's design in 143 runs,
2,038,229,843 clks of pin comparison, no difference. The decode sweep
runs every Mega II and expansion vector three times (no copy, the copy
with the switch off, and on): 1,875,164,248 tests, no failure. Replica,
socket, arming and the ten core benches pass. Twenty-six mutations of
the new logic (the decode's rules, the store at the cycle's end, the
repeat of a forwarded write, the twin update of a cached line, the tags,
the address map, the grant), each run through thirteen runs: all
detected.

What the switch changes in simulation: the test program, which spends
much of its time in E0/E1, averages 20.49 MHz with it and 7.04 without
(4,123 reads of that RAM, 313 of them far reads). The ROM 01 boot hardly
reads that RAM: 6.58 MHz with it, 6.52 without.

**Timing and size.** **Three placements with no violation at 105 MHz were not
reached, so by this project's rule the build does not count and nothing
was flashed.** Ten placements over four rounds, five clean:

| round | source | placements |
|---|---|---|
| v | an earlier source | +0.006 (one build) |
| w | the first complete source | 13 paths, -0.212; +0.008; +0.001 |
| x | the write registers' two clks stated (below) | +0.000; 24 paths, -0.201; +0.002 |
| y | x and the registered tag (below) | 48 paths, -0.348; 52 paths, -0.354; stopped at 40 min in routing |

Round w's failure was the new logic: the step's address through the full
decode into the write registers (`whit_r`), nine levels. Those registers
are loaded with a value that is used no sooner than the third clk of a
step, so the paths into them are two-clk paths; that is now said in the
constraints (`gen_sdc.py`) and held to in simulation (`wr_bad`: what
they load earlier reads wrong, and a write made with it shows). In
round x that path has 6 to 8 ns to spare, and what failed in x1 is what
was at the edge before this section: the tag block to the first-clk
compare, the compare to the core's clock enable, in a placement that
spread the core over thirty rows. Round y took the two later compares
off the tag block's outputs (`rtag_q`, a registered copy of the tag for
the full decode's compare and the write's): no help - both placements
that finished failed across the design (the PHY to the ROM copy, the
DDR3 controller's own write buffer, the core's two-clk paths), with the
worst path the same tag compare at 2.0 ns out of the block RAM, 1.6 ns
of logic and 5.7 ns of wire.

So: t0 closed three of three with +0.000 to +0.006, which is no margin,
and about 150 more logic cells beside the core make it a coin toss per
placement. The new logic adds one term to one first-clk signal
(`fast_sock`); the rest of what fails is not its logic but its
neighbours. Section 3e has the same history: there the cure was a
floorplan change, and 100 MHz did not buy the failing placement back.
Not tried: more floorplan for the first-clk compare and its register;
100 MHz for this build; the first-clk test without its new term (reads
of E0/E1 then start from the registered decode with the switch on).

The source in the tree is round y's (the regression and the mutations
above are of it). `ACC_NO_SLOW_COPY` in the defines builds without the
feature; that build has not been placed since `rtag_q` and the new
constraints went in. (The card ran t0 until the experiment below.)

Size: 13,236 logic cells, 11,929 FF, 182 of 340 block RAMs (t0: 13,088
logic cells, the same 182).

**Not known.** What the benchmark gives with it (by section 3g's
arithmetic the two floating point tests come to 24 to 26 and 81 to 85
ticks and the average to about 49: an estimate from 120 samples). How
the copy compares with the real machine's Mega II RAM: the paced verify
has not been run. Whether anything the machine does writes that RAM
behind the processor's back (a card that does DMA would).

### On the machine: x0, as an experiment (2026-11-14 afternoon)

Ed's decision: flash one placement that has no violation, to learn what
the copy is worth before more time goes into the timing. x0 is round x's
first placement (+0.000 ns; that source closed two of three), so **this
is a build that does not meet the three-placement rule, on the card as a
measurement and nothing more.** Its source is the tree's without
`rtag_q` (the full regression passed on it too). Flashed 15:39 with the
machine off.

Ladder: word 13 reads `0C` (built with the copy, the copy whole); ROM
copy, sum `7EE8`; paced verify of banks 00/01 for 40 s, no mismatch;
then paced verify with the copy (`1A`): 60 s at the benchmark's prompt
and through part of a base pass, word 13's count 0 and word 14 0, with
E0 and E1 among the sampled socket addresses. (Part, because the card
was put in fast mode for a minute in the middle of that base pass by
mistake - told the prompt was up when it was not. A whole base pass in
`1A` is still owed, and that run's base times are not the machine's.)

Fast with the copy (`1B`), 52.5 entered, one run, against section 3f's
one run without it:

| test | ticks without | ticks with | Actual MHz without | with |
|---|---|---|---|---|
| sieve | 21 | 21 | 49.65 | 49.65 |
| string sort | 60 | 59 | 48.72 | 49.55 |
| floating point, 100 iterations | 34 | 28 | 37.81 | 45.91 |
| 15,000 Gamm units | 115 | 93 | 35.76 | 44.22 |
| Fibonacci | 110 | 110 | 46.31 | 46.31 |
| integer math | 86 | 86 | 45.88 | 45.88 |
| average | | | 44.02 | 46.92 |

Ten runs, the same evening, against section 3f's ten without it:

| test | ticks without | ticks with | Actual MHz without | with |
|---|---|---|---|---|
| sieve | 20.80 | 20.90 | 50.12 | 49.89 |
| string sort | 59.40 | 59.10 | 49.21 | 49.46 |
| floating point, 100 iterations | 33.90 | 26.70 | 37.92 | 48.14 |
| 15,000 Gamm units | 115.10 | 92.40 | 35.73 | 44.51 |
| Fibonacci | 109.80 | 109.40 | 46.39 | 46.56 |
| integer math | 85.60 | 85.20 | 46.09 | 46.31 |
| average | | | 44.24 | 47.48 |

The two tests that wait for the slow memory gain 27 and 25 % over ten runs; the others
do not move. Less than section 3g's arithmetic gave (24 to 26 and 81 to
85 ticks): that took every socket cycle in E0/E1 away, and what the
telemetry shows in the Gamm stretch is 43.5 M steps a second where it
was 34.9, socket cycles 0.052 M a second where they were 0.219 (so about
a fifth of them remain: writes, and whatever is at `$C000` and up), and
read misses up from 3.8 to 5.3 for each thousand steps: about three in
ten of the reads that moved to the copy go to the DDR3 for their line.

The switch while the machine runs, at the report's keypress prompt,
register 29 changed `1B`, `0B`, `1B` with eight seconds in each:

| | copy read | not | copy read again |
|---|---|---|---|
| core steps, M/s | 30.93 | 15.64 | 30.93 |
| socket cycles for the core, M/s | 0.151 | 0.460 | 0.151 |
| read misses, M/s | 0.459 | 0.183 | 0.459 |

That loop runs twice as fast with it. The E0/E1 mismatch count is still
0 after it.

Not done: a whole base pass in paced verify with the copy; what the remaining socket cycles of the floating point tests
are; and the timing, which is where section 3h left it.

## 3i. The computing tests' other clks: writes to the program's stack (2026-11-14, x0 on the card)

Against the AppleSqueezer GS Turbo's published times for this benchmark
(22, 62, 27, 91, 107, 82 ticks; stock 410, 1151, 472, 1535, 2006, 1553)
section 3h's ten runs (20.9, 59.1, 26.7, 92.4, 109.4, 85.2) are level in
total - 18.23 times the stock machine for both - ahead on the sieve, the
sort and the first floating point test, and behind on Gamm, Fibonacci
and integer math, by 1.5 to 4 %. Those last are the tests section 3g
left at 2.29 clks a step with almost no socket cycle and no miss: in the
Fibonacci stretch of section 3h's log, 46.0 M steps a second, 0.004 M
socket cycles, 0.001 M misses - 2.28 clks a step, which is one step in
seven at four clks if that is what they are.

**They are writes the cheap view does not take.** A step is two clks
when the first-clk view (`gs_mmu_decode` `fast_rw`) says it is the
card's alone. For a write in banks 00/01 that view excludes every page
that CAN be shadowed - `$0400-$07FF` and all of `$2000-$9FFF` - without
looking at the shadow register, and such a write waits for the full
decode: four clks (the bench's "local write" class). On the machine:

- the shadow register reads `1E` with the benchmark up (telemetry word
  9): hi-res 1 and 2, super hi-res and the auxiliary hi-res pages all
  inhibited. Nothing in `$2000-$9FFF` is shadowed;
- the program's direct page and stack are in that range. In paced mode
  (every step a socket cycle, so the last socket address is the last
  step's) 342 samples at the report's prompt have 94 in bank 00: pages
  38 (18 samples), 3C (6) and 66 (4) beside the zero page, the system's
  stack in BE/BF and the firmware in C3 to C5. The first mismatch of
  the old verify count after fast mode was at 00/3BEC.

So every push, every store to a local and every store through the
direct page in this program is a four-clk step. Recursion and integer
arithmetic on stack frames are mostly that; the sieve and the sort
write to arrays in the program's own bank (an expansion bank: two clks)
and lose less. It is an inference from the rule, the register and the
addresses - the telemetry does not count writes - but nothing else in a
stretch without socket cycles or misses takes more than two clks except
an internal cycle at `$Cxxx` and a write at `$D000` and up.

**What would take it away:** the cheap view asking two things the
replica already holds - main `$2000-$5FFF` is shadowed only if
`shadow[1]` or `[2]` is clear, main `$6000-$9FFF` never, auxiliary
`$2000-$9FFF` only if `shadow[3]` is clear or an auxiliary hi-res page
is shadowed (`shadow[4]` clear with `[1]` or `[2]`). Two terms of the
shadow register and the bank bit the view already makes for the tag.
The decode sweep proves a cheap view a subset of the full decode on
every vector, so the rule cannot be wrong unnoticed. The bound is the
program's own "expected time" column for 52.5 MHz: 19.9, 55.7, 24.5,
78.3, 97.0, 75.2 ticks. Not built: it is logic in the step's first clk,
where this design has no margin (section 3h).

## 3j. The write rule with the shadow register in it (2026-11-14 evening; simulated in full; timing not closed; one clean placement, q1, on the card as an experiment)

Section 3i's rule, in its smallest form, is in the source:

```
wire f_hgrq = shadow[1] & shadow[2] & shadow[3];
```

and the cheap view's write rule leaves `$2000-$9FFF` of banks 00/01 to
the full decode only when `f_hgrq` is 0. With hi-res 1, hi-res 2 and
super hi-res all three inhibited nothing in that range of either bank
can be shadowed (the auxiliary hi-res pages need bit 1 or 2 clear as
well as bit 4), so the write is the card's alone and the step is two
clks. That is the benchmark's case: its shadow register is `1E`. It is
not the fuller rule of section 3i: a program with super hi-res
shadowing on (the desktop: `shadow[3]` clear) still pays four clks for
a write to main `$6000-$9FFF`, which that rule would take. The text
pages are as before: never cheap. The same commit takes `rtag_q` of
round y out again (section 3h: it did not help), so the source is
round x's with this rule; x0 on the card does not have it.

**Simulation.**

| check | result |
|---|---|
| decode sweep, 1,875,164,248 vectors, with the rule checked for equality (a write in the range is cheap exactly when the register's bits 1 to 3 are all set) | 0 failures; 991,042 such writes taken in the first clk, none of them shadowed by the full decode; 2,644,827 more are shadowed nowhere and stay at four clks (the fuller rule's share) |
| new test T26 in the bench's program: writes at 00/3BEC, 6100, 9FFF and 01/3BEC under `1E`, then one write right after each change of `$C035` to `16`, `0E`, `1C`, `1E`, `1A` (hi-res 1, hi-res 2 and super hi-res each enabled alone, main and auxiliary, RAMWRT on for one), and the twelve bytes the Mega II's banks and the card's must hold after | in every run of the regression |
| the bench counts the range's writes in fast mode and fails a run in which fewer than 5 are two-clk steps | 8 of 28 in the plain fast runs (the others are T26's writes under the settings where the rule must not take them, and earlier tests') |
| full regression: 210 bench runs, both replicas, socket and arm benches, 10 core benches | all pass |
| planted faults: the 26 of section 3h and six in the new rule (ignores bit 3, bit 1, bit 2; always takes; never takes; polarity) | 32 of 32 detected, each of the six by the sweep and by the bench |

**Timing at 105 MHz: not closed.** Three placements of this source
(z0, z1, z2):

| placement | worst slack | violations | the worst paths |
|---|---|---|---|
| z0 | -0.033 | 5 | the PHI2 period counter into its telemetry register; the tag hit into the core's X; the address register into the first-clk read decision |
| z1 | +0.006 | 0 | |
| z2 | -0.045 | 4 | the address register into the first-clk write decision (eight levels of logic, and 2.6 ns of wire to one of its four copies); the registered decode into the core's enable; a line's data into the line register |

One of three. The misses are a tenth of rounds w to y's (-0.2 to
-0.35), and no two placements fail on the same path, but that is what
"no margin" looks like and the rule is three of three. z2's worst path
is the decision this rule is in. z0's worst is not the accelerator's at
all: the period counter is a diagnostic and can take a register stage.
Size: 12,900 logic cells of 69,120, 11,918 flip-flops, 182 block RAMs.
None of the three is on the card. z1's bitstream was not kept (the roll
script keeps only the first; the build stamp is in the netlist, so it
cannot be made again). A fourth build would be a new placement.

### On the machine: q1, as an experiment (2026-11-14 night)

Ed's decision, as for x0: measure what the rule is worth before more
time goes into the timing. The committed source was built until a
placement had no violation, every bitstream kept: q0 -0.080 (10
violations), **q1 +0.002, none**. Of this source's five placements two
are clean; q1 is not a closed build. Flashed 19:16 to 19:20 with the
machine off.

| step | result |
|---|---|
| key, `gs set 29 0x08` | mode 80, 1 copy, sum `7EE8` |
| `0x0A`, 15 s | mode 88, verify count 0 |
| `0x1A` (paced verify with the copy), then **the benchmark's whole base pass of ten runs in that mode** | mode 8A, word 13 `0F`; verify count 0, copy verify count 0 at the end. This is the pass section 3h's experiment did not complete |
| the program idle at its prompt (the address samples match the prompt's of section 3i), then `0x1B` | mode 9A; 33.24 M steps a second at the prompt (x0: 30.93) |
| the accelerated pass, ten runs | below; verify words unchanged after it |

The program's report, ten runs, 52.5 entered, beside x0's ten runs
(section 3h) and the AppleSqueezer GS Turbo's published ticks:

| test | x0 ticks | **q1 ticks** | Squeezer | x0 MHz | **q1 MHz** |
|---|---|---|---|---|---|
| sieve | 20.9 | **19.8** | 22 | 49.89 | **52.66** |
| string sort | 59.1 | **55.3** | 62 | 49.46 | **52.86** |
| floating point | 26.7 | **27.1** | 27 | 48.14 | **47.43** |
| Gamm | 92.4 | **89.8** | 91 | 44.51 | **45.80** |
| Fibonacci | 109.4 | **96.3** | 107 | 46.56 | **52.90** |
| integer math | 85.2 | **74.6** | 82 | 46.31 | **52.89** |
| sum / average | 393.7 | **362.9** | 391 | 47.48 | **50.76** |

The sum is 7.2 % under the Squeezer's and 19.64 times the stock
machine's 7,127 (the Squeezer: 18.23). Five tests are ahead of it; the
first floating point test is level (27.1 against a published 27) and
is 0.4 tick slower than x0's ten runs, which this build gives no reason
for and ten runs at one tick's grain do not settle.

Section 3i's inference holds. The log of the pass (telemetry every 0.4
s) has the stretches with no socket cycle and no miss at 52.0 to 52.7 M
steps a second: 2.00 to 2.02 clks a step, where x0 had 46.0 (2.28). Four
tests now read above the 52.5 entered because the program's scale is
the stock machine's nominal 2.8 MHz and those tests are at this
design's bound: every step two clks. What is left is in the two
floating point tests: their stretches run 2.3 to 3.4 clks a step with
0.05 to 0.29 M socket cycles and up to 0.24 M misses a second; the
whole pass averages 44.98 M steps a second.

The report's base column (673, 1857, 487, 1679, 3066, 2211) is the pass
in paced verify, not the stock machine's, so its improvement column
means nothing here; the MHz column does not use it. Every earlier base
pass was run on socket bytes (register 29 = `08`: the machine's own
memory at the machine's pace) and gave 413, 1159, 476, 1546, 2020,
1564. Paced card memory is slower than the machine by design since the
DDR3 build (section 3b, "Waiting for the memory"; section 3e): a paced
step fills no line, so every read of an expansion bank goes to the
DDR3, and its socket cycle is issued again until the byte is there. The
ratios say the same: 1.64, 1.60, 1.53 and 1.42 on the four tests that
run in the program's own bank, 1.02 and 1.09 on the two that run in ROM
and the tools. Checked on q1 the same night: after a power cycle, with
the card at its power-up state (mode 00, no copy; PHI2 averaging 2.66
MHz), the first round of a base pass was back in the machine's range on
every test (Ed's reading of the screen; the pass was not run to its
report). A second report followed from that state (ten runs; the base pass on
socket bytes before the card was brought up at the program's speed
prompt: copy sum `7EE8`, 10 s of each verify with no mismatch, `1B`):
base 413, 1159, 476, 1545, 2021, 1564 - the machine's, as on every
earlier build - and accelerated 19.8, 55.2, 27.1, 89.8, 96.3, 74.6:
52.66, 52.96, 47.43, 45.80, 52.90, 52.89, average **50.77 MHz**. The
first report repeats to a tenth of a tick on five tests, so the
floating point test's 27.1 against x0's 26.7 is this build's and not
chance; its cause is not known. Against its own base pass q1 is 20.9,
21.0, 17.6, 17.2, 21.0 and 21.0 times the machine, 19.8 on the sum.

It matters beyond the benchmark: paced
card memory is the only way back from fast mode without a reset, so a
speed setting of "off" would today be slower than the machine for a
program in expansion RAM.

## 3k. Closing the timing (2026-11-14 night; closed: three placements at 105 MHz with no violation; p1 flashed 2026-11-15)

### The first round: a guard band, not kept

**What the kept placements say.** The tightest paths of the thirteen
placements kept since the copy of section 3h, grouped by the registers
they start and end at: the placements that fail do so in more than
twelve different groups, and no two in the same one - the near memory
back into the near memory through the core, the instruction register
into the near address, the address register into each of the first-clk
decisions, the tag block into the hit copy, the hit copy into the core's
A, X and Y, the line index into the line register, the copy machine's
state, the PHI2 period counter. Every placement without a violation ends
between +0.000 and +0.03 ns (the tool stops when the constraint is met),
and the failing ones of section 3j's source end 0.03 to 0.08 ns short.
So there is no one path to shorten. A registered copy of the shadow
bits' AND was not built: the path it would shorten starts at the bank
bits of the address register, and that path was already at +0.006 in x0,
before the rule existed.

**Two changes, built together (round g).**

- The PHI2 period counter of the telemetry counts from 1, so the period
  is the register itself and the adder into `ph2_period_o` (z0's worst
  path) is gone. Checked against the old form in a bench of its own:
  40,000,000 clks, 2,314 captures (4 after a wrap of the count), the two
  outputs equal in every clk. The full regression passes on this source
  (210 bench runs, both replicas, socket and arm benches, 10 core
  benches). The 32 planted faults were not run again; the change is in
  the telemetry only.
- The place and route was asked for 0.1 ns more than the rule on the
  accelerator's clock (0.6 ns of setup uncertainty where the rule is
  0.5), so that it stops later. A report made that way shows every slack
  0.1 ns less than the rule's.

| placement | in the report (0.6) | by the rule (0.5) | |
|---|---|---|---|
| g0 | 0 violations, +0.020 | +0.120 | the first placement of this design with margin beyond the rule |
| g1 | 93 violations, -0.669 | -0.569, 65 of the listed paths short | the whole broker: tag block to the hit copy, the address register to every first-clk copy, the hit to the broker's state |
| g2 | none | none | the router did not finish in 40 minutes and the build was stopped |

One of three, and the two that fail do so by far more than any placement
at 0.5 did. In g1 the first-clk copies and the core's enable copies sit
beside the core (rows 47 to 53) while the logic that makes them is
twenty rows away in the broker (rows 67 to 75): each of those paths ends
in 2.1 to 2.9 ns of wire, and one net of fan-out 20 crosses the broker
twice (2.8 and 3.8 ns). In g0 two of the four copies are beside the core
and two beside the broker, and the enable copies are in the broker. The
0.1 ns did what was asked of it once and cost two placements; it is
taken out again (the constraints are the committed ones, 0.5). The
period counter change stays.

Kept: g0 (stamp `20261114200313`, sha256 `a423257a45663c0c…514dae15251e8`,
no violation at 0.6) and g1 (`20261114202509`, `d231c5143ada1a7c…c3c037f520b11c`,
failing). Neither is on the card; the card runs q1.

### The second round: the placement option, closed

The same source with the 0.5 ns rule as committed and one change in the
place and route: `Place_Option` 1 in
`impl/a2mega_accel_138B_process_config.json` (it was 0; `Route_Option`
stays 0). The tool's command file of the last build shows
`-place_option 1 -route_option 0`. Three placements, p0 to p2, each with
its own build stamp in the netlist:

| placement | setup violations | hold violations | worst slack beyond the 0.5 ns | the tightest path | built in |
|---|---|---|---|---|---|
| p0 | 0 | 0 | +0.010 | a line's data into the core's P | 13 min |
| p1 | 0 | 0 | +0.009 | the broker's run copy into the last-address telemetry register | 21 min |
| p2 | 0 | 0 | +0.004 | the PHY's enable into the copy machine's sum | 18 min |

**Three of three: the rule is met.** Every clock of the design is at
zero in each (the accelerator's, the logic clock, the DDR3's two, the
50 MHz input, PHI2). What that does and does not say: the slack beyond
the 0.5 ns is still a hundredth, as in every clean placement of this
design, because the tool stops when the constraint is met; the margin is
the 0.5 ns of the rule. Section 3j's source closed two of five with the
old option; this source differs from it in the period counter (one of
the failing groups) and the option, and which of the two did more is not
separated. A source change from here is a new closure. A round with
`Route_Option` 1 was prepared and not needed.

Size: 13,215 logic cells, 11,918 flip-flops, 12,644 of 69,120 logic
slices, 182 of 340 block RAMs (p1). The logic is the simulated logic of
section 3j with the period counter change: the regression above is this
source's.

Kept: p1 (stamp `20261114213411`) and p2 (`20261114215549`); section 5
has the sums. p0's bitstream was not kept: an older file of that name
in the scratch directory was read-only, the copy failed, and the next
build replaced the file. Its timing report is kept.

### p1 on the card (2026-11-15)

p1 was flashed over USB with the IIgs off (read-back verify, first
attempt, 10:40), the board power-cycled, the IIgs switched on. That p1
is what runs rests on the checked sum of the file, the verified flash
and the power cycle. The ladder, at a prompt:

| step | result |
|---|---|
| before anything | no stall, PHI2 386.5 ns a cycle (the machine's range; the counter's change is a 256th of a clk a cycle and cannot be seen here, the bench comparison of the first round is its proof) |
| DDR3 | word 12 = `057E` |
| ROM copy (register 29 = `08`) | mode 80, one copy, sum `7EE8` |
| card memory, paced, verify (`0A`), 12 s | mode 88, verify count 0, no mismatch address |
| the same with the copy of E0/E1 (`1A`), 15 s | mode 8A, both verify counts 0, word 13 = `0F` (the copy whole, SLOW in effect) |

Then fast (`1B`) with the machine idle at a prompt (Ed's word, and the
bus addresses of six seconds were a prompt's: the ROM's banks, 00, the
program's bank 04, E0/E1): mode 9A, no stall, the machine runs on. Over
10.6 s at that prompt: 33.3 M core steps a second, 0.162 M socket cycles
a second for the core, 21.95 M line hits and 0.49 M misses a second.

The benchmark on p1, five runs averaged (Ed, the same morning):

| test | accelerated, ticks | MHz | q1, ten runs: ticks | q1 MHz |
|---|---|---|---|---|
| sieve | 19.80 | 52.66 | 19.8 | 52.66 |
| string sort | 55.20 | 52.96 | 55.2 | 52.96 |
| floating point | 27.20 | 47.26 | 27.1 | 47.43 |
| Gamm | 89.80 | 45.80 | 89.8 | 45.80 |
| Fibonacci | 96.20 | 52.95 | 96.3 | 52.90 |
| integer | 74.60 | 52.89 | 74.6 | 52.89 |
| sum, average | 362.8 | **50.75** | 362.9 | 50.77 |

p1 is q1 to the report's resolution: an average of five runs moves in
steps of 0.2 tick, and the two tests that differ differ by 0.1. No base
pass was run: the program was told to use its stored times for an 8 MHz
TransWarp GS (153.4, 422.6, 168.2, 560.4, 733.4, 517.2), so the base and
improvement columns of this report are against that card and not this
machine. The MHz column does not use them: on every report it is a
constant of the test divided by the accelerated time. So the build that
meets the timing rule on three placements measures what the experiment
measured, past the Squeezer's 391 ticks by the same margin.

## 3l. Fast by itself: the bring-up in the firmware (2026-11-15; tested against a model; flashed; one power-up on the card reached fast mode by itself)

Until now fast mode was reached by hand over telnet, one register write
at a time. The ESP32 firmware now runs the same ladder after every arm
(`gs_socket.c`, `gs_accel_*`), each step checked before the next:

| step | register 29 | goes on when | if not |
|---|---|---|---|
| wait | untouched | the gateware shows its signature and the socket's /RES has been high for 2 s | waits |
| ROM copy | `00`, `08` | the image is valid | copies again (a /RES cuts a copy short), three tries, then stops on `00` |
| card memory at the machine's pace | `0A` | 1 s with the verify count unmoved | back to `08`, and it says where the mismatch was |
| the same with the copy of E0/E1 | `1A` | 1 s with the copy's verify count unmoved | fast without the copy (`0B`) |
| fast | `1B` | | |

So about 4.5 s after the machine's reset ends it is in fast mode, with a
second or two at the paced rate on the way (slower than the machine for a
program in expansion RAM; section 3j). Rules it keeps:

- **The wait is there for the ROM's cold or warm start test**, which
  must read the machine's own RAM: the card's memory holds only what the
  core has written since the socket was armed, and after a machine power
  cycle with the card still powered it holds the last session's bytes.
- **Machine off clears register 29**, so every power-up starts on the
  machine's own memory and copies the ROM again.
- **After fast mode has run, register 29 never goes back to `00` or
  `08`** while the machine is on. "Off" is paced card memory.
- A build without the copy of E0/E1, or a copy that is not whole, goes
  fast without it. A DDR3 that is not ready is "card memory not
  granted": it stops on `08`.
- An ESP32 restart under a machine in fast mode adopts what it finds and
  writes nothing. Gateware without the signature (S6, the full core) is
  never written to.
- A hand write of register 29 or 31 (`gs set`) stops the ladder until
  `gs accel go`.

The setting is one byte of the saved settings, `gs_accel_off`, 0 by
default: **the accelerator is on unless it is turned off**, as a
TransWarp GS is. Telnet (`:` for the command line):

| command | |
|---|---|
| `gs accel` | state, mode in effect decoded, ROM copies and sum, both verify counts and their last addresses, the expansion memory |
| `gs accel on` / `off` | the saved setting. `off` with fast mode in effect goes to paced card memory at once; from the next power-up the machine runs at its own pace |
| `gs accel slow` / `fast` | paced card memory and back, at any time, setting unchanged |
| `gs accel go` | run the ladder now (also after a failure, a hand write, or with the setting off) |
| `gs accel delay <ms>`, `verify <ms>` | the wait and the length of each verify step (not saved) |

`gs` gains one line with the state, and each step is logged on the
console (`ACCEL: ROM IMAGE VALID, SUM 7EE8` ... `ACCEL: FAST`).

**Tested** on the host against a model of the register window
(`tests/host/test_gs_accel.c`, run by `tests/host/run.sh`): twelve
scenarios - a normal start, the setting off, a verify mismatch, a
mismatch in the copy, a build without the copy, a /RES in the ROM copy,
machine off and on again while fast, an ESP32 restart under fast mode,
slow and fast and the setting at run time, a hand write, S6 gateware, a
DDR3 that is not ready - each with the writes to register 29 compared
and the rule about `00` after fast mode checked on every write. The
firmware builds for the 1.0a3 board (1,136,343 bytes, 86 % of the slot).

**On the card (2026-11-15, 12:57), and what exactly.** The card's ESP32
ran the firmware of 2026-09-13 (its banner; commit `e96b40cf`, the S6
auto-arm). The repository's firmware has moved two months on since - the
a2p25 variant, the USB host and the stick as the volume, the reset hold
until media is mounted, self-update with a different partition table -
and none of that has run against this gateware. So the card was not
given the repository's firmware: it was given **its own source,
`e96b40cf`, with this section's change to four files applied** (the diff
of `gs_socket.c`, `gs_socket.h`, `settings.h`, `telnetd.c` in the commit
that adds this section; those files but `telnetd.c` are the same in both
trees, and the patch applies to all four as it is), built with that
tree's Makefile (BOARD_REV 103, the `no_ota` partition table the card
has): 1,046,971 bytes, sha256 `d1533014030e9934…591574e8fb77c1f10`.
Uploaded over the ESP32's USB with the IIgs off, the USB serial number
checked against the card's network address first. It came up
(`A2MEGA ESP32 NOV 15 2026 12:55:17`), joined the network with its saved
settings, and waits for the machine's clock with the socket off;
`gs accel` answers, setting ON. Moving this card to the repository's
firmware is a step of its own.

**The first power-up with it (2026-11-15, about 13:05).** The IIgs was
switched on with nothing typed at the card. Its console, in order:
machine clock up, reset released, armed 20 ms after the slot reset read
high; `ACCEL: WAITING FOR /RES HIGH 2000 MS`, `COPYING THE ROM (TRY 1)`,
`ROM IMAGE VALID, SUM 7EE8`, card memory at the machine's pace, the same
with the copy of E0/E1, `ACCEL: FAST`. `gs accel` afterwards: mode 9A,
one copy, both verify counts 0 with no mismatch address, the DDR3 ready
with 126 banks; no stall. Counters over 8.6 s a minute later: 33.2 M
core steps a second, 0.162 M socket cycles a second for the core, 21.95
M line hits and 0.49 M misses a second - this morning's figures at a
prompt in fast mode. So the ladder ran inside the boot, where it had
never run, and the two verify seconds saw no mismatch there. On the
screen the machine booted normally (Ed).

**A control-reset in fast mode (the same session).** The first reset
taken in fast mode on hardware. The card's event log has it whole: the
slot's /RESET and the socket's /RES fell together, the core went into
reset in the same 2 us, the lines rose 179.2 ms later and the core ran
again 2 us after that. Afterwards: still `ACCEL: FAST`, mode 9A, the
ROM image still the one copy (no second copy, as meant: a warm reset
keeps the image and the card's memory), no stall, both verify counts 0,
and the idle rates as before the reset (33.2 M core steps and 0.162 M
socket cycles a second), the bus in the ROM's banks and E0. The
firmware wrote nothing: there is nothing for it to do at a reset.

**Not done, not known.**

- One power-up and one control-reset. The model is mine, and only the card can say where it is
  wrong; this run agreed with it on the normal path. None of the other
  paths (a mismatch, a /RES in the copy, machine off and on with the
  card powered, an ESP32 restart, `slow` and `fast`, the setting off)
  has run on the card.
- **No speed control (S7e).** Fast mode does not follow the machine's
  speed register or slow down for a disk drive: code between accesses to
  a drive runs at the card's rate. The a2p25's disk cards are polled and
  do not care; a real 5.25 or 3.5 drive does, and a write to one in fast
  mode could damage the disk's data. `gs accel slow` before using one,
  until the card does this itself.
- "Off" is slower than the machine once fast mode has run (section 3j).
- A restart through Control-Open-Apple-Reset in fast mode has not been
  taken on hardware (a control-reset has, above).
- The `gs` line for PHI2 still assumes a 110 MHz clock; on this 105 MHz
  build its periods read 4.5 % short.

## 3m. Speed control: paced at the machine's pace, and the machine's rules (2026-11-15; simulated; three placements with no violation; sc1 flashed 15:43; the first and the motor rule seen at work with an emulated Disk II; a fault of the slot 6 rule seen and the rule changed, three placements with no violation, se2 flashed 2026-11-16; a power-up with it stopped on a verify mismatch and Arkanoid locked up, cause not known)

Section 3l makes fast mode the card's normal state, and that leaves two
things wrong. "Off" is slower than the machine (section 3j: paced card
memory is the only way back from fast mode, and a paced program in
expansion RAM read every byte from the DDR3). And fast mode runs the
code between two accesses to the machine at the card's rate, whatever
that code is: a disk drive's read and write loops count cycles.

### A paced step fills its line

The near memory's lines were filled by fast steps alone: the fill hung
on the fast step's own count. A paced (native, card memory of record)
step's far read now fills its line too (`gs_accel_broker`, `nf*`): once
the data has been steady for two clks after the expansion memory lets
go, once, and not in the clk the step ends. Fast mode is untouched - the
new registers are never set in it - and so is paced mode's rule that a
socket cycle is not taken as complete until the byte is there; only the
first read of a line waits now. Every byte a line gives a paced step is
compared with the socket's, as before, so the ladder's verify seconds
now check line contents and not single bytes.

In the bench, the test program paced with 126 expansion banks:

| | before | now |
|---|---|---|
| reads that went to the expansion memory | 9,026 | 289 |
| socket cycles issued again while a byte was on its way | 4,626 | 139 |
| line fills | 0 | 614 |
| time for the 60,000 steps (native: 26.85 ms) | 28.63 ms, 6.6 % over | 26.92 ms, 0.3 % over |

And the ROM 01 boot paced (3 M steps): 2,144,414 reads, each a far read
before; now 225 line fills and 2,144,189 reads out of lines, every one
compared with the machine's byte, none different.

### The machine's speed rules

`gs_accel_rules.sv`, asked for by **register 29 bit 5**. With the bit
clear nothing changes. With it set, fast mode gives way to paced card
memory - every step a socket cycle, timed by the machine's own PHI2,
which the FPI slows or not by its own rules - while:

| | |
|---|---|
| the speed register (`$C036`) has bit 7 clear | the machine was asked for its normal speed: the Control Panel's setting, or a program's |
| the motor latch of slot 4, 5, 6 or 7 is set (`$C0n9` on the bus since `$C0n8`) and that slot's detect bit in the speed register is set | the FPI's own rule for dropping to 1 MHz |
| slot 6's motor latch is set and the 3.5 drive is selected (`$C031` bit 6) | the IWM's enable for a drive the ROM serves at the machine's fast rate by loops timed for it; the machine does not slow for it, so the first two rows do not see it |

(As changed on 2026-11-15, "The slot 6 rule changed" below. **sc0 to
sc2, and so the card, have the rule as first built:** slot 6's latch
paces by itself whatever its detect bit says, and `$C031` is not
looked at.)

The latches follow the bus as the FPI's do (any cycle the socket shows
at those addresses in banks 00, 01, E0, E1; an I/O access is a socket
cycle in every mode) and clear at /RES. The change of mode is the
broker's own, at a step boundary, a step or two after the access - the
switch the bench has always made at random points (`+mode=toggle`).
Telemetry word 10 is now `{16'h0, motor latches of slots 7-4, 00, rules
asked, slow by them, the other switches}`; the mode byte's fast bit
drops while the rules hold. (From the changed rule on, bit 7 of the
other switches is `$C031` bit 6.)

**Not among the rules:** the speaker (`$C030`: a program that times
sound with loops plays it too high in fast mode, as on any accelerator
with that option off), a linger after the motor goes off, slot DMA,
interrupts. And nothing on the machine's side can ask for a speed yet:
no Zip or TransWarp registers, no desk accessory (scoping 2.5).

**The firmware** asks for the rules with fast mode (`3B` where section
3l wrote `1B`; gateware without them ignores the bit); `gs accel rules
on|off` at run time, and `gs accel` shows the rules, the speed register
and the four latches.

### Simulated

The test program has a T27: a stretch of local code under each cause,
and one under slot 5's latch with its detect bit clear, which must stay
fast. The bench has its own instance of the rules for the hand-wired
design and compares gs_accel beside it pin for pin (so the two must
change mode at the same step), checks that a step is paced within three
steps of the rules' asking and fast again within three of their letting
go, that each cause paced a stretch, and that the fourth stretch ran
fast (260 fast steps). Eleven new runs (`q_*`, `j_rules`, `m_rules`):
with and without the copy of E0/E1, toggling, the FPI held at 1 MHz, a
warm reset, RDY stalls, `+mode=copy`, the line memory on block RAM, the
write stream compared with the other modes', and the ROM 01 boot - where
the rules paced 275,398 of the 3 M steps (the ROM runs that much of its
start at normal speed) and the run reached the same point with the same
writes as every other.

**The broker bench: 221 runs, all pass** (210 before), on the final
source. Three faults planted in copies of the source:

| planted | |
|---|---|
| a paced fill that does not wait for the expansion memory's data | 17 runs fail (a stale byte delivered) |
| the rules forget slot 6's motor | 9 runs fail |
| any motor latch paces, whatever its detect bit | **passed at first**: the check asked for one fast step in that stretch, and the first step or two after the access are fast before the mode changes. The check counts now (100 or more); 9 runs fail |

Not run again: the decode sweep, the replica, socket, arm and core
benches (nothing they build was changed), and the 32 earlier faults.

### Timing

Three placements of the final source, the committed options
(`Place_Option` 1), 105 MHz, the 0.5 ns rule:

| | setup / hold violations | worst slack beyond the rule | on |
|---|---|---|---|
| sc0 | 0 / 0 | +0.031 | a line's data into the core's P |
| sc1 | 0 / 0 | +0.035 | the near memory's index into its hit register |
| sc2 | 0 / 0 | +0.014 | the PHY's enable into the copy machine's sum |

**Three of three.** Size (sc1): 13,249 logic cells, 11,931 flip-flops,
12,755 of 69,120 logic slices, 182 of 340 block RAMs - 34 cells and 13
flip-flops more than p1.

### Flashed (2026-11-15 15:43)

sc1 written to the card's flash with the machine off, 15:39 to 15:43,
the first attempt, verified by the loader's read back. Then the
firmware of section 5 (`4691e43a…`) over the same USB port, the port's
serial number checked against the test card's first. The firmware
answers on the network with the machine off: `gs accel` reads "no
accelerator gateware seen, or the socket is not armed | setting ON",
and its usage line has `rules on|off`. The gateware has not been loaded
yet: that wants the board's power off and on.

### The first power-up (2026-11-15 15:48)

The board's power off and on, then the machine on. The console: ROM
copy on the first try, sum 7EE8, paced card memory, the same with the
copy of E0/E1, FAST. No command was typed. Three readings after it:

| when | mode | speed register | what |
|---|---|---|---|
| 15:48:36 | - | `82` | the ladder's last line is FAST |
| a few seconds on | `8A` | `02` | `gs accel`: "paced by them now"; last address 00:C0E1; fast asked, not in effect |
| 15:49:42 | `9A` | `82` | fast in effect; 33.3 M steps/s, 0.162 M socket cycles/s, 21.96 M hits/s, 0.493 M misses/s |

So something in the start-up cleared bit 7 for a stretch with slot 6's
soft switches in use, and the card ran that stretch at the machine's
pace and went back to fast when the bit was set again: the first rule,
on the machine, with nothing typed. Which code it was has not been
looked up. The last line's rates are those of section 3l's idle prompt.
No motor latch was set in any reading. The copy of E0/E1: built, whole,
verify count 0. The card memory's verify count reads 65535 (last
7F:F50F, later 00:AE16): once fast mode has run the machine's RAM is
stale by design and a step paced by the rules is compared with it, so
that count says nothing after the first fast step; before the rules it
could not move in fast mode at all. The count for E0/E1 is the one that
still means something.

### The Control Panel's speed at Normal (15:53), and fourteen mismatches not explained

Ed set the Control Panel's system speed to Normal. Read at 15:53:55 and
again at 15:55:21, the machine idle:

| | fast, 15:49 | Normal, 15:53 |
|---|---|---|
| mode | `9A` | `8A`, "paced by them now" |
| speed register | `82` | `02` |
| core steps | 33.3 M/s | 1.085 M/s |
| socket cycles for the core | 0.162 M/s | 1.016 M/s |
| PHI2 cycles | 2.449 M/s | 1.023 M/s |
| line misses | 0.493 M/s | 0.017 M/s |

The rule does what it was built to do: the card runs at the machine's
1 MHz. About 6 % of the steps are not socket cycles; a guess, not
checked, is the firmware's interrupt handlers setting the speed bit for
their own length, which the rule follows both ways.

**Not explained: the count of mismatches in the copy of E0/E1 reads 14
(last address E1:1D6B), where it read 0 at 15:49:42.** It did not move
between the two readings ninety seconds apart. The count only moves in
paced steps, so the fourteen fell between the speed bit clearing and
15:53:55: the rest of the Control Panel's session, its exit, or the
first of the idle after it. The bench holds this count at zero in every
run, the rules' runs too, and a paced pass of the whole benchmark held
it at zero on q1. ROM 01 has no long reference to E1:1D6B itself (it
has many to E1:1DB0-1DD6). What it could be, none of it checked: a line
of the copy stale after the new paced fill; a write in fast mode that
reached the copy and not the machine, or the reverse; a comparison
taken before a slow DDR3 read had answered (a false count: the step
itself waits). The core uses the copy's byte, and the machine went on
running. Until it is explained, the copy of E0/E1 with the rules is not
known to be right.

### The fourteen, narrowed (16:04 to 16:12)

A log of the counters about once a second while Ed worked the Control
Panel, and then four changes of mode by command with the machine idle
at Normal:

| what | count for E0/E1 |
|---|---|
| speed set back to Fast (Control Panel opened while paced) | 14, unchanged |
| speed set to Normal, 16:05:41 | 28 within a second |
| ... again, 16:06:44 | 42 |
| ... again, 16:07:06 | 56 |
| ... again, 16:08:01 | 70 |
| a second or less at Normal, twice (16:06:10, 16:07:03) | unchanged |
| `gs accel rules off`, then fast and `gs accel slow` four times at the idle prompt, 2 to 16 s of fast between | 70, unchanged |

Fourteen each time the Control Panel sets the speed to Normal and
leaves it there, the last address E1:1D6B each time, and none from a
change from fast to paced by itself. So it is not the change of mode
and it is not chance: some code that runs when that option is set reads
E1:1D6B, or bytes that end there, and the copy's byte is not the
machine's. The simplest reading is one byte that differs and stays
different, read fourteen times at each change of the option; opening
the Control Panel while paced and setting Fast would not show it,
because nothing is compared once the card is fast. ROM 01 has no
absolute or long reference to E1:1D6B (the Desk Manager's own variables
are at E0:1D61 to 1D74, through its data bank), so the reader has not
been found. Fast mode with the speed register at Normal (the rules not
asked) ran here for the first time, sixteen seconds at the longest,
with nothing seen wrong. Next: the monitor's verify command over E0 and
E1 with the card paced reads every byte through the comparison, which
counts the bytes that differ and so audits the whole copy.

### The audit, and the cause: RAM the copy never had (16:20 to 16:51)

Ed restarted the machine to BASIC at 16:20 (the card paced, `1A`) and
ran the monitor's dump and verify with the card paced:

| command | mismatched reads | bytes, about |
|---|---|---|
| `E1/1D60.1D6F` | 32 | all 16; the screen shows FF for each |
| `E1/0000<E1/0000.BFFFV` | 37,044 | 18,500; none above E1:9F0F |
| `E0/0000<E0/0000.BFFFV` | 9,976 | 5,000; nearly all of 0000-02FF; none above 7FFF |

Then the same lines read three ways:

| line | the machine (`0A`, mode 88) | the copy, paced (`1A`) | the copy, fast (`1B`) |
|---|---|---|---|
| E1:1D60-1D6F | 00, all | FF, all | FF, all |
| E0:0000-000F | 00, all | (counted as different) | `96 D5 8A C8 B4 AD 83 58`, twice |
| E1:0000-000F | - | - | `5C AF 00 FE 5C A3 00 FE 5C 75 00 FE 5C 69 00 FE`, the machine's vectors |

The fast path, which this build did not change, reads the same FF from
the DDR3 as the paced path: the paced fill is not at fault (a suspicion
of an hour, written to Ed, and wrong). The copy is as section 3h built
it: **there is no pass that reads the machine's RAM into it.** It holds
the writes the card has mirrored, and section 3h says so ("what the
machine's RAM held from before that reset the copy does not have: after
a power-up that is RAM nothing has written"). Where nothing has written
since the card began to mirror, the machine's RAM holds what it held
(00 in every byte looked at) and the DDR3 holds what it powered up
with (FF, or a pattern). Section 3h took it that nothing reads such
RAM. The Control Panel does: fourteen reads of E1:1D60-1D6F each time
the speed is set (which of its code, not found), and they get FF where
the machine gives 00. Whether that changed anything it did is not
known; nothing was seen.

So the 47,122 are a difference that is real and old, not a fault of the
rules or of the paced fill: x0, q1 and p1 (on main) have it in fast
mode, where nothing compares. It is a fault of fidelity all the same -
a program that reads slow RAM it has not written, and there is at least
one in the ROM, gets a byte the machine would not give. What would
close it: a pass at bring-up that reads E0/E1 below $C000 through the
socket into the copy with the core held, as the ROM image is copied
(96K slow cycles, about a tenth of a second), "whole" then meaning that
pass has run; and a bench whose machine RAM and expansion memory do not
start out equal, which is why no run has ever shown this. Not built.

Ed's question, and it sharpens this: nothing but this card's core can
write the machine's RAM, so nothing was "set" there before the
bring-up, and nothing goes wrong at a change of speed either (four
changes by command made no mismatch; the change is only when the
Control Panel reads such a byte with the comparison on). The two
memories start out different and only written bytes are made equal.
The bench's ROM 01 boot agrees that these bytes are not written: in
its three million steps the ROM writes six pages of E1 directly (00-03,
0F, 15), none of E0, and never 1Dxx. What the DDR3 holds there is
whatever it held before: the card was without power for a few seconds
only between the flash and this power-up, and the mismatches in E1 end
at 9F0F, the end of the super hi-res screen, so much of it is probably
the copy of the session before (not checked). On the machine itself
such RAM is whatever its chips came up with, which no program can count
on either; so this is a narrow difference - a program that reads slow
RAM nothing has written gets other bytes than this machine's - and not
a corruption of anything written.

The card was left at `3B` by `gs set 29 59` (the firmware's ladder in
its manual state until the next power-up), the Control Panel's speed at
Normal.

### The a2p25's Disk II in slot 5: a boot and a catalog (17:57 to 18:01)

The first drive through the rules is the a2p25's own (slot 5 by the
default map; the detect bit of slot 5 is set on this machine, the speed
register reads `82`). The Control Panel's speed at Fast, the counters
read about twice a second:

| what | mode | speed register | slot 5's latch | steps |
|---|---|---|---|---|
| the prompt, before | `9A` | `82` | 0 | 33.5 M/s |
| `PR#5`, twenty seconds of drive | `8A` | `02` | 1 (0 in four readings between) | 1.0 M/s |
| after it; it booted | `9A` | `82` | 0 | 33.5 M/s |
| `CATALOG`, a second and a half | `8A` | `02` | 1 | 1.0 M/s |
| after it; it listed | `9A` | `82` | 0 | 33.5 M/s |

Both read correctly, paced at the machine's 1 MHz for as long as the
drive ran and fast again by themselves. **But neither isolates the
motor rule:** the speed register read normal in every reading with the
motor on. The machine's software clears the bit itself. ROM 01 does it
before it enters the firmware of a slot set to "Your Card" (FE/9C9x:
the slot's bit of SLTROMSEL, then `TRB` of `$80` on `$C036`; the boot
path the same at FF/BACD, set again at FF/BA4E), and something did it
again for the catalog (which code, not looked for; the Disk II firmware
of `hdl/disk/diskii.hex` has no reference to `$C036`). So for a Disk II
in a slot the first rule already covers what was run, and that the ROM
uses the bit on purpose to run card firmware at 1 MHz is an argument
for the first rule as built. (At the BASIC prompt with the speed at
Normal the bit reads set in about a third of the readings: the ROM's
text firmware saves it, sets it and restores it, FF/CB8C.) A boot the
same way with the speed still at Normal, earlier, also read correctly:
the disk's timing holds in paced mode.

**A fault of the slot 6 rule, found by reading (seen on the machine
at 20:05, next section).** The rule paces on slot 6's latch whatever the detect bit
says, for the 3.5 drive. The a2p25's hard disk card sits in slot 6 by
the default map and its register 9 is the unit's size
(`hdl/disk/hdd.sv`, read by the card's ROM in a ProDOS STATUS call):
a read of `$C0E9`. That sets the latch, and the card is then paced until
something touches `$C0E8`, the data register - the next block read.
The latch for slot 6 has not read set in any log of today. What the
rule should ask instead: slot 6 like the other slots, by its detect
bit, and the 3.5 drive by what selects it (`$C031` bit 6), which the
replica does not track yet. Not built.

### Four reads from the monitor: the motor rule alone, and the slot 6 fault seen (20:05)

To take the speed bit out of it, the latches were set by hand from the
monitor at the DOS prompt of the same boot, the Control Panel's speed
still at Fast, the counters read about twice a second:

| read | what it is | mode | speed register | latches (slots 7-4) | steps |
|---|---|---|---|---|---|
| before | | `9A` | `82` | 0 | 33.6 M/s |
| `C0D9` at 20:05:09 | slot 5's motor on | `8A` | `82` | 2 | 1.02 M/s |
| `C0D8` at 20:05:16 | slot 5's motor off | `9A` | `82` | 0 | 33.6 M/s |
| `C0E9` at 20:05:27 | the hard disk card's size register | `8A` | `82` | 4 | 2.6 M/s |
| `C0E8` at 20:05:35 | its data register | `9A` | `82` | 0 | 33.7 M/s |

**The motor rule works by itself.** With the speed register at `82` in
every reading, slot 5's latch paced the card for the seven seconds it
was set (fifteen readings) and the card was fast again within half a
second of the read that cleared it.

**The slot 6 fault is real.** One read of `$C0E9` set slot 6's latch and
the card stayed paced for the eight seconds until `$C0E8` was read
(sixteen readings). Nothing else on this machine would have cleared it:
after a ProDOS STATUS call to the hard disk the card is paced until the
next block read. The fix is as written above and is not built.

The two paced stretches ran at different rates, 1.02 M and 2.6 M steps
a second. That fits the machine's own pace in each: the machine slows
itself to 1 MHz for slot 5's motor (its detect bit is set) and does not
for slot 6's latch (its detect bit is clear), so the second stretch was
paced against a machine still at its fast speed. Socket cycles were not
counted in this log, so that is a reading of the step rate and not a
measurement of the machine. The count of E0/E1 mismatches did not move
(47122, section above).

### The slot 6 rule changed (2026-11-15, 20:10 to 23:00): simulated; the first round two placements of three, the round with the copy's repair three of three; se2 flashed 2026-11-16

**The change.** Slot 6's latch is one of four now: it paces with its
detect bit (`$C036` bit 2), as the FPI has it, and with the 3.5 drive
selected (`$C031` bit 6), and not by itself. The replica tracks
`$C031` bit 6 (written in an I/O bank, cleared at /RES) and shows it as
bit 7 of its switch byte, so nothing has a new port. Three source
files: `gs_accel_rules.sv`, `gs_mmu_replica.sv`, one line of
`gs_accel.sv`.

**Why the 3.5 drive by its select bit.** ROM 01's 3.5 driver, on entry
(FF/5DB3): `LDA $C036`, `AND #$FB`, `ORA #$80`, `STA $C036`, then `LDA
#$40`, `STA $C031`; the IWM's enable (`$C0E9`) is read a few
instructions later in the same routine. On the way out (FF/5644): `BIT
$C0E8`, `STZ $C031`. So the ROM itself takes slot 6's detect bit away
and asks for the fast speed before it touches the drive - the machine
does not slow for a 3.5 drive and the first two rules cannot see one -
and the select bit is set for exactly the call. Read from the ROM
image; **no 3.5 drive has been run**, and a driver that is not the
ROM's (GS/OS's) has not been read.

**Simulated.** T27 has six stretches now: the speed register normal;
slot 6's latch with nothing else (must stay fast: the hard disk's
case); the same with `$C031` = `$40` (paced); `$C031` cleared again
(fast); slot 6's detect bit (paced); slot 5's latch without and with
its detect bit. The bench asks that each of the four causes paced a
stretch as the only cause, and counts the fast steps under a latch with
no rule (slot 5's 260, slot 6's 518; 100 or more wanted of each). The
replica's bench has the bit in its reference model, with directed
facts and in the random sequences (6 M checks for each ROM, none
failed).

**The broker bench: 221 runs, all pass**, on the committed source
(`ba398b71`). The first run of it had one failure, `c_recopy`, and not
of the rules: the longer test program moved the second ROM copy so
that a RDY stall fell on the last idle cycle of the hold. The card
then issues the park address (`FF0000`) while the stalled cycle
(`FE0000`) repeats on the bus with the bank byte not driven again -
right, and as it always was - and the bench's FPI, which keeps the
bank through a stall, called the next cycle `FE0000` and found nobody
owed it. The bench knows that case now.

Five faults planted in copies of the source, the program suite and
the replica's bench on each:

| planted | |
|---|---|
| the rule as first built (slot 6's latch paces by itself) | 9 runs fail: 1 fast step in the hard disk's stretch |
| the 3.5 select ignored by the rule | 11 runs fail |
| slot 6's detect bit ignored | 9 runs fail |
| `$C031` never tracked | 9 runs fail; the replica's bench fails (1.5 M checks) |
| `$C031` tracked from bit 7 | 11 runs fail; the replica's bench fails |

**Timing, the first round: not closed.** Three placements of `ba398b71`, the committed
options, 105 MHz, the 0.5 ns rule:

| | setup / hold violations | worst slack beyond the rule | on |
|---|---|---|---|
| sd0 | 0 / 0 | +0.024 | the near memory's tag into its hit register |
| sd1 | **19** / 0 | **-0.231** | the same path; the other eighteen are the line memory's and the broker's usual ones |
| sd2 | 0 / 0 | +0.003 | a line's data into the line memory's address |

**Two of three.** None of the nineteen paths is of the rules or of the
new bit; they are the paths every placement of this build has been
measured on. What it says is what the margins of sc0 to sc2 (+0.031,
+0.035, +0.014) already said: this build closes by hundredths of a
nanosecond, and a placement can land on the other side. With
`Place_Option` 1 that is eight clean placements of nine (p0 to p2, sc0
to sc2, sd0 to sd2). By the rule this page keeps - three of three of
the final source - sd is not a closed build, and another draw of three
would not make it one. sd0 and sd2 are each a placement with no
violation and are kept; neither is flashed.

**Found beside it, older than it, not fixed: a ROM copy that begins
under a RDY stall takes its first byte from the wrong bank.** The hold
begins at a step's end; in fast mode an idle cycle (a read of
`FF0000`) is on the bus then. If RDY is low at that cycle's fall the
PHY repeats it, with the address lines as the broker now gives them
(`FE0000`: the same low sixteen bits) and the bank byte not driven
again, so the bus reads `FF0000` once more - and the broker, which has
marked the repeat as the copy's first read, hands that byte to the
copy as the byte of `FE0000`. Made to happen in a scratch copy of
`ba398b71` (the bench stalls the cycle the second copy is asked in):
six runs of eight end with one byte of the image different and the
sum one off; the two others began their hold after a cycle of the
core's own. The suite never does it (its stalls and its copy request
come from one loop, never together: sixteen copies under stalls every
2 to 7 cycles, none wrong). It needs a copy asked for from fast mode -
the power-up copy is asked for in native mode, where the cycle before
the hold is the core's own and has completed - and RDY low on that one
cycle, which on this machine is a slot card's doing; and the
firmware's check of the sum would see it. In every build with the ROM
copy up to sd.

**Repaired (`42b046a0`).** The broker's mark of a cycle as the hold's
is not taken again when the PHY repeats a stalled cycle: the repeat
stays the park read it was, its byte goes to nobody, and the copy's
first read is the next cycle, with its own bank byte. One line of
`gs_accel_broker.sv`. The bench has the stall as an option
(`+holdstall`: RDY low on the cycle the second copy is asked in), four
runs with it - two on the line memory, the board's configuration - and
each must count a hold that began under a stalled idle cycle (of ten
start points tried on the line memory seven are one; the suite uses
two). The bench also takes a stalled cycle's repeat for the cycle it
was, which replaces the correction for `c_recopy` above; the idle
check is as strict as it was.

**The broker bench: 225 runs, all pass**, on `42b046a0`; the four
runs each with one such hold and the image identical. Three faults
planted in copies of it:

| planted | |
|---|---|
| the repair taken out | 4 runs fail: the four with the stall, and no other |
| the mark dropped at every repeat (a stalled read in the middle of a copy too) | 22 runs fail |
| the slot 6 rule as first built, again | 9 runs fail |

**Timing, the round of the final source: three of three.** Three
placements of `42b046a0`, the committed options, 105 MHz, the 0.5 ns
rule:

| | setup / hold violations | worst slack beyond the rule | on |
|---|---|---|---|
| se0 | 0 / 0 | +0.022 | the PHY's clock toggle into the broker's far-bank copy |
| se1 | 0 / 0 | +0.029 | a line's data into the line memory's address |
| se2 | 0 / 0 | +0.039 | the PHY's clock toggle into the record of the issued address |

Closed by the rule of this page, and by no more than the others: the
margins are hundredths of a nanosecond, and sd1 of the round before -
the same logic but for one line - missed by 0.231. With `Place_Option`
1 that is eleven clean placements of twelve. Size (se2): 13,255 logic
cells, 11,932 flip-flops, 12,666 of 69,120 logic slices, 182 of 340
block RAMs - 6 cells and 1 flip-flop more than sc1.

### se2 flashed (2026-11-16 07:22 to 07:26)

se2 written to the card's flash over USB with the machine off, the
first attempt, verified by the loader's read back. The one probe on
USB had the test card's serial number, and the file written had se2's
sha256 (section 5). The firmware is as it was (`4691e43a…`): the
change needs none. **The gateware has not been loaded yet, and nothing
of this build has run on the machine:** that wants the board's power
off and on, then the ladder to fast mode, and the four reads of the
section above again - `C0E9` must leave the card fast now, `C0D9` must
still pace it.

### A power-up with se2: the bring-up stopped on a verify mismatch, and Arkanoid locked up (2026-11-16, read 19:57 to 20:03)

Ed, at the machine without a log running: Arkanoid, launched by
accident, locks up "with the accelerator on"; the machine was left in
that state. What the card said, read over telnet and changing nothing:

- **The card was not accelerating.** The firmware's console for this
  power-up: ROM image valid, sum 7EE8; card memory at the machine's
  pace; the same with the copy of E0/E1; then `ACCEL: VERIFY MISMATCH
  AT 01:FBC3 (6) - ON SOCKET BYTES`. Mode 80 since: every step a socket
  cycle, every byte the machine's (2.54 M steps and 2.54 M socket
  cycles a second). The first second of card memory passed; the count
  moved in the second with the copy of E0/E1, and it is the main count
  (banks 00/01 and expansion RAM), not the copy's (0).
- So six reads in that second gave the core the card's byte where the
  machine's differed, the last at `01:FBC3`. Which of the two was right
  is not known, nor the other five addresses, nor the bytes: the
  telemetry keeps the last address and the count.
- The event log has one power-up and no reset after it. If the step
  counter has not wrapped, the core began to run at about 19:54:40,
  three minutes before Ed's message; the counter wraps every 28
  minutes, so that is not certain.
- At 20:03 the program is not stopped: 120 samples of the bus address
  are 81 in bank 02 and 37 in bank 01, about half of them in
  `02:8000-81FF` and `01:8000-81FF`, the rest spread over both banks;
  none in E0/E1 or at `$C0xx`. The replica's switches then: shadow 57
  (I/O and language card shadowing inhibited in banks 00/01), speed
  register 92, state register 0C, all four motor latches set.

Not known: whether Arkanoid was running when the count moved (it would
have to be the boot disk, or started within seconds); whether the lock
up comes from those six bytes, or happens on socket bytes with no card
memory at all (which would make it older than this section); whether
sc1 or p1 does the same. A mismatch of the main count in a bring-up is
new on this card: every earlier power-up with the ladder read 0, but
none of them is known to have had this program in the two verify
seconds. The source differs from sc1's in the speed rule, one tracked
bit of `$C031` and the copy's first byte under a stall; none of that is
in the path of a read in paced card memory, and that is reading, not a
test.

**Two more runs, with a log (20:08 and 20:26).** The build stamp read
from the card is `20261115224949`: se2 runs. The disk in use boots
straight into Arkanoid (Ed; the first run had started it later, from
something else).

- *Power cycle, 20:08.* The core runs from 20:08:14 and the game is
  loading within two seconds (shadow `1E` by 20:08:16.5). Copy done
  20:08:17, sum 7EE8; card memory on at 20:08:18.08; by 20:08:18.52 the
  count is 4, last `02:000B`, and the firmware is back on socket bytes.
  Not the first run's address. The game loads on socket bytes for 70 s
  (the speed bit and the language card switching as a loader's would),
  sets shadow `57` and speed register `92` at 20:09:30, all four motor
  latches read set at 20:09:31, and nothing changes after that: locked,
  in its attract mode (Ed). The bus profile is the first lock's.
- *Control-Open-Apple-Reset, 20:26:50* (the firmware does nothing at a
  reset: mode 80 throughout, the count still 4). The whole load again
  on socket bytes, shadow `57` / `92` at 20:28:06.9, the latches set by
  20:28:08.2, locked at the same place (Ed).

**So the lock does not need card memory.** In the second of these runs
no byte of the card's reached the core between the reset and the lock
(the machine's RAM was not cleared between them, but the game loads
itself again). What is left: the core, the broker's native mode and the
PHY as this build has them; this placement of them; or something
outside the card. Not separated. The verify mismatches are a second
fact, not explained either: two power-ups of se2, two addresses
(`01:FBC3`, `02:000B`), where every power-up of the earlier builds read
0 - none of them with this disk.

**A gap in the speed rules, seen in passing.** The motor latches take
`$C0n8/9` in banks 00 and 01 whatever the shadow register says. With
its bit 6 set there is no I/O in those banks, only RAM, and Arkanoid
sets the four latches that way; with a detect bit set in the speed
register (it leaves `92`: slot 5's) that would pace the card in fast
mode until the same RAM at `$C0n8` is touched. Not fixed, and not the
lock: the rules do nothing on socket bytes.

**Left for the night (20:41).** se2 on the card. **The card's saved
setting is OFF** (`gs accel off`, set at 20:10 for a run that was not
made as planned): the next power-up stays on the machine's own bytes
with register 29 never written. `gs accel on` with the machine running
and the ladder stopped runs the ladder at once, so it was not set back
under Ed's hands; set it back with the machine off. The beside-card
a2p25 has the hard disk fix (Ed), not the Mockingboard build. Two
tests are open, neither made: Arkanoid started with the card in paced
card memory by hand (`gs set 29 10` at an idle prompt) and the verify
count logged up to the lock - 0 clears the socket's data path, a moving
count names the RAM that differs from what the core wrote; and sc1
flashed back with the same disk, then se0, to tell this build from the
ones before and source from placement.

**A fourth lock, with the setting off (2026-11-21, read 08:30 to
08:40).** Ed booted the same disk with the card as it was left and
Arkanoid hung as before. Read without changing anything: `ACCEL: OFF BY
SETTING`, **mode 00** (no ROM image, no ROM copy made, sum 0000), every
verify count 0, the socket armed by the automatic sequence as in every
run. Register 29 was never written in this power-up. The lock is the
same one: shadow 57, speed register 92, state 0C, all four motor
latches set, 2.546 M socket cycles a second, and of 200 bus samples 64
in `02:8000-81FF` and 34 in `01:8000-81FF`, the rest spread over banks
01 and 02, none in E0/E1 and none at `$C0xx`. So the lock needs nothing
of the accelerator beyond what every power-up of this build does: the
F65C816 core stepping on socket cycles through the broker and the PHY.
The first open test above (paced card memory with the verify count)
no longer bears on the lock; it still bears on the two verify
mismatches. `01:8000-81FF` is super hi-res pixel memory (lines 153 to
156) and super hi-res shadowing is on in shadow 57, so the program may
be drawing there from bank 02; that is a reading of the addresses, the
program has not been read. What it is waiting for is not known. Still
not known: whether this disk runs on this machine with its own CPU, and
whether any earlier build of the card runs it.

**The scratch directory was emptied by the system between 11-16 and
11-21** (macOS clears files under `/tmp` that have not been read for
three days; every directory is there, no file is). Gone: the kept
bitstreams of every round (p1, p2, sc0 to sc2, sd0, sd2, se0, se1), the
test card's firmware binary, the logs named in this page, and the
telnet helpers. se2 is on the card and in `impl/pnr`. A build before
se2 for the second open test has to be made again from its commit, and
its placement will not be the one that was measured. Bitstreams to keep
go beside the others in `boards/a2mega/impl/pnr` from here on, as
`a2mega_accel_138B_<round>.fs` (that pattern is ignored by git).

**p1 locks too (2026-11-21, 16:40 to 17:00).** p1 was built again from
its commit (p1r, section 5: the report repeats p1's to the last figure)
and flashed with the machine off; the stamp read from the running card
is `20261114213411`. Ed booted the same disk with the setting still
off: Arkanoid hung at the same place. Read: mode 00, verify 0, shadow
57, speed register 92, state 0C, 2.542 M socket cycles a second, and of
200 bus samples 69 in `02:8000-81FF` and 36 in `01:8000-81FF`, none in
E0/E1 or at `$C0xx` - the lock of se2 to the sample counts. (The motor
latches read 0: p1 has no speed rules.) So the lock is not of the speed
control, the slot 6 rule, the copy's repair or se2's placement: the
build that was benchmarked and merged has it. What is left is what p1
and se2 share - the F65C816 core, the broker's socket steps, the PHY -
or something outside the card. One thing not yet tried on hardware and
in Arkanoid's path: the program inhibits I/O and language card
shadowing (shadow bit 6), and a machine in that state gives a vector
pull ROM only if its FPI reads the socket's VP pin, which the card
drives through U15 (`gs_socket_65816_scoping.md` section 3.3 took the
motherboard not to use VP; the S7 scoping page, decision 13, left it to
be checked). That is a suspicion; no vector pull of this program has
been seen. Not known still: whether the disk runs on this machine with
its own CPU. p1r is on the card, the setting off; se2 is in
`impl/pnr` as the default file and by name.

### Not done, not known

- **No write, no real drive.** The drive read through the rules is the
  a2p25's emulated Disk II (a boot, a catalog, and the latches set by
  hand); nothing has been written through them, and
  the bench's machine has no drive. The latches and the detect bits are
  as the MiSTer core's model of the FPI has them (`clock_divider.v`:
  tracked whatever the detect bits say, gated by them); slot 6 holding
  without its detect bit is this card's own, for the 3.5 drive. A real
  5.25 or 3.5 drive is the test, with a disk that can be lost.
- **The changed slot 6 rule and the copy's repair have not been seen
  at work on the machine** (se2 is in the card's flash since 2026-11-16
  07:26; the one power-up read with it never reached fast mode, the
  subsection above). With sc1, a ProDOS STATUS call to a hard disk
  card in slot 6 left the card paced until the next block read; that
  se2 does not is simulated, not seen. The 3.5 drive's rule is from
  reading the ROM: no 3.5 drive has been run, with either rule.
- **An open choice: the first rule.** Scoping 2.5 says the speed
  register's normal setting paces the card ("the Control Panel's
  Normal/Fast setting keeps meaning what it means"), and that is what is
  built. The MiSTer core's accelerator model says the opposite of the
  real cards: a ZipGS or TransWarp GS stays fast with bit 7 clear and is
  slowed through its own registers, and the Zip's desk accessory clears
  the bit in its speed test expecting the card to stay fast. As built,
  the Control Panel's speed setting is an off switch on the machine's
  side (Normal: 1 MHz, as on a stock machine) and code that drops to 1
  MHz to count cycles gets what it asked for; a program that clears the
  bit and expects an accelerator to ignore it loses its speed until it
  sets the bit again. Which a TransWarp GS does is Ed's to say; making
  the first rule a request bit of its own is a small change and a new
  timing closure.
- Paced mode at the machine's pace is the bench's figure; the benchmark's
  base pass in paced mode on the card is the measure (section 3j's was
  1.4 to 1.6 times the machine's on four tests).
- A ROM 3 machine; the speaker; anything the machine itself can set.

## 3n. A trace of the socket's cycles, to read Arkanoid's lock (2026-11-21; simulated; three placements with no violation; not flashed)

Section 3m ends with the lock on two builds, in mode 00, and nothing
that shows what the program is doing when it stops. The telemetry has
one bus address at a time. This build puts the S6 bench's bus trace
back, deeper, in the accelerator's clock domain.

**What it is.** `hdl/twgs/gs_accel_trace.sv`, in `gs_accel` under its
`TRACE` parameter, on in a project of its own:
`a2mega_accel_138B_trace.gprj` (the accelerator-only project's sources
with `hdl/accel_only/accel_trace_defines.v`; `Place_Option` 1 as that
project). A ring of 8,192 entries in block RAM, one for a cycle the core
had on the socket: bank and address as the broker issued them, R/W, VP,
VPA and VDA from the issue, and the data pads and /IRQ as the broker's
PHI2-fall flops had them at the fall that ended the cycle. Local steps
of fast mode are not socket cycles and are not in it. It only reads:
the one change in the design it sits in is two outputs on the broker,
the PHI2-fall flops of the data pads and /IRQ, so that no second set of
flops hangs on the pads. The source of se2 is otherwise as it was; the
project without `ACC_TRACE` synthesises as before (checked; not placed
again).

**What is kept and when it stops** is set through window registers 29
to 31 under keys of their own (31 = 50, 51, 52; the accelerator's
requests are the same registers under A5, so the setup is for a card
that is not asked for card memory - mode 00, as the lock is): every
cycle, opcode fetches, vector pulls, or vector pulls and the opcode
fetches that do not follow the one before; a trigger that stops the
ring a chosen number of kept entries later - an opcode fetch at an
address, any cycle at an address, a vector pull with shadow bit 6 set,
or 131,072 cycles with none in an I/O page (what the lock looks like
from outside). CTRL bit 3 (`gs freeze`, `gs run`; `gs trace` sets it)
stops it by hand. At power-up it keeps every cycle and has no trigger,
so a machine that has locked needs only `gs freeze`.

**Read** with the firmware the card has: `gs trace` shows telemetry
words at lines 0 to 31 as before (15 to 17 are the ring's state) and a
page of 32 entries at lines 32 to 63, oldest first, in the S6 trace's
form; its STALL column reads "/IRQ low" and BE0 "vector pull". `gs`
shows the ring standing still and the trigger fired where the S6 trace
showed them. `boards/a2mega/tools/gs_trace.py` does the setup, reads
the whole ring over telnet (256 pages) and lists it as 65C816
instructions with the data cycles beside them.

**Simulated.** The module's own bench (`hdl/twgs/sim/trace`): a
program-shaped stream of 593,357 cycles against the bench's model of
the filter, the triggers and the ring, 64,000 entries read back through
the page and index, no difference; eight planted faults (the flow test's
window, the count after the trigger, the quiet count by one, the oldest
entry's place, the entry taken two clks late, the slot key without its
sixteen clks, the I/O test without bit 6, /IRQ stuck) each fail it. The
broker bench with `gs_accel` and its trace beside the hand-wired design
(`b105_2a`, six runs: native, paced, fast, toggling, a reset in a
write, RDY stalls): about 350,000 entries compared with the cycle the
machine's side of the bench latched - no difference - and gs_accel's
pins equal to the design's in every clk as in every such build. The
whole regression with the six: **231 runs of 231 pass**, the ROM 01
boots and the test program at the same points with the same write
streams.

**Timing: three placements of three with no violation** on any clock
(105 MHz, the 0.5 ns rule as committed):

| placement | setup | hold | worst slack beyond the 0.5 ns | the tightest path |
|---|---|---|---|---|
| tr0 | 0 | 0 | +0.011 | the microcode row into the broker's internal-I/O flag |
| tr1 | 0 | 0 | +0.010 | the near memory's index into the broker's `nh_dq` |
| tr2 | 0 | 0 | +0.017 | the PHI2 high-time counter into its telemetry register |

No tight path is the trace's. 13,587 logic cells, 12,267 flip-flops,
201 of 340 block RAMs (tr2): 19 blocks are the ring. The socket's pin
flops are where they were (61 I/O logic cells, as p1 and se2).

**What it has not done.** It has not run on the card. The trace shows
what the core issued and what the pads read at the fall; it is not an
instrument on the pins themselves, and a fault between the pin flops
and the motherboard would show only by what the machine answers.

### tr2 on the card: what the lock is (2026-11-21, 18:10 to 18:50)

tr2 flashed 18:08, its stamp read from the card; Arkanoid hung on it as
on se2 and p1r, the setting off, mode 00. Five captures, each after a
Control-Apple-Reset, in `boards/a2mega/captures/arkanoid_1121/` (not
committed: the program's code is in them).

**The lock is one loop, and a real CPU would not leave it either.** At
`02:80C8`, data bank 01, direct page 1B00: ten pairs of `LDA $93A0,X` /
`STA $93A0,X` (rows `$93A0` to `$9940`, 160 apart: super hi-res lines
185 to 194 of bank 01, each word read and written in place so that the
shadowing just switched on puts it on the screen), then `DEX`, `DEX`,
`CPX $B0`, `BCS $80C8`. It is entered with X = `$B0` + `$BA` - 2 and
runs while X >= `$B0`. `$B0` holds 0000: the carry is then always set, X
passes zero and the loop sweeps `01:93A0+X` with the 24-bit carry into
bank 02 for ever (the samples "in banks 01 and 02" of section 3m). The
strip is the paddle's: 18 bytes wide (`$BA` = 12), its left edge taken
from `00:1BB2` by `LDA $B2` / `STA $B0` at `02:6EC9`.

**What the captures show of the way in.**
- The frame that locks begins normally: the game waits for the vertical
  blank by polling `$E1C019`, sets the border, runs its objects, draws
  the ball (new column 38, old column 08, line B5) and refreshes two
  strips through its routines at `02:6D52` and `02:6D6C`, which write
  the shadow register 1E or 57 and the speed register 82 or 92 around
  each. Every I/O cycle in it is one of those, with the values the
  program wrote: no soft switch answers wrongly in that frame.
- `00:1BB2` is 0000 from the first instruction of that frame.
- About half a second earlier (the first access to `00:1BB2` after the
  game starts; the ring stopped on it) the game's own start-of-round
  code at `02:D88B` writes 0030 there and draws the paddle at column
  30. The ball's new column 38 in the frame that locks is that paddle's.
  So between that frame and the one that locks something writes zero
  to the paddle's column, or the code that keeps it is not run as on
  the machine's own CPU. Not yet seen.
- Interrupts are taken and the machine answers them rightly: 11,721
  vector pulls in one boot and game, every one `00:FFEE` or `00:FFFE`
  answered `C074`; the reset's `00:FFFC` answered `FA62`; the vector
  pull flag low on each. One interrupt read in full: taken at the
  vertical blank inside the polling loop, through `E1:0010` into the
  ROM at `FF:B7CC` and back by RTI to the instruction it left. (The
  suspicion of section 3m about VP is withdrawn.) In the frame that
  locks /IRQ goes low at the vertical blank and is not taken: the game
  has patched `SEI`, `RTS` into the end of its shadow routine
  (`LDA #$6078` / `STA $026D66`), so that is the I flag, not the core.

**What the trace turned out not to have.** VPA and VDA at the socket
are both 1 for every cycle that is not internal (the broker issues
them so; only the substituted I/O cycle differs): the trace cannot
tell an opcode fetch from data, its "opcode" filter keeps every
non-internal cycle, and its opcode trigger is the any-cycle one.
`tools/gs_trace.py` lists by the order of the addresses instead, with
the register widths followed from REP, SEP and pulled P bytes; the
8,192 cycles of each capture list with no instruction out of step. The
VP flag, R/W, the address, the data and /IRQ are as the bench has them.
And `gs`'s `last_addr` is three bytes read at different times: its bank
and address can be of different cycles ("01:8000-81FF" in section 3m
was that, and "lines 153 to 156" there is wrong).

## 4. What is verified, and what is not

Verified in simulation (the broker suite, `hdl/twgs/sim/broker`):

- `gs_accel` runs beside the bench's hand-wired design on the same socket
  inputs, and **every output pin is compared in every clk** (`ACCEL_SHADOW`),
  in native, paced, fast and toggling modes, with interrupts, RDY stalls, a
  BE pause, warm resets, at 100, 105, 108 and 110 MHz. ROM 01 boot at 110 MHz:
  50,850,536 clks, no difference; over the suite's 79 runs with the module
  beside the design (2026-11-13), 814,174,211 clks.
- The ROM copy (`+mode=copy`): the image starts empty, the design runs
  native from the socket, copies 131,072 bytes through the socket, empties
  the near memory and goes on in fast mode. The image is compared with the
  machine's ROM byte for byte; the test program gives its results; the ROM
  01 boot reaches the same step hash (`6012b76169ecc820`) and the identical
  128,393-write stream as every other mode.
- Fault injection on the new logic (the hold, the copy, the flush, the
  module's memories and grants): 12 of 16 detected. The four that are not:
  the write enable's timing and which copy of the address a near-memory
  block reads (nothing a zero-delay simulation can see), the bank test on
  RAM writes (no expansion memory here), and the tag the flush writes when
  the held step's address is in ROM (it is then the same tag; the bench
  checks it directly when it is not). Four of the twelve needed two things
  the bench gained for them: a second copy made from fast mode, mid-cycle,
  and checks that a copy ends with the near memory empty and the held step
  not trusting its byte. That run also showed the flush is needed: 207
  entries read from the empty image were in the near memory at the first
  copy.
- The suite is 88 runs, all passing; the CPU lockstep bench is unchanged by
  this build.
- The empty near-memory entry (section 3c, found on hardware): all 126
  banks in two builds at 105 MHz, a byte at the top of bank 7F read before
  anything wrote it and again after a ROM copy; the suite is 152 runs.
- The early socket request (scoping doc §10.5): a test of code and 16-bit
  data in a Mega II bank, also called from the interrupt-enabled loop; a
  bench property that no idle cycle is issued in front of a socket-only
  step; two runs with a RDY stall every second and every fifth PHI2 cycle.

Timing (138B, 0.5 ns setup uncertainty; constraints generated by
`hdl/accel_probe_138b/gen_sdc.py`, floorplan and pins by `gen_floorplan.py
accel`):

| build | failing | worst slack | `clk_gs` (110 MHz) reaches | `clk_logic` (56.25 MHz) reaches |
|---|---|---|---|---|
| ao3 | 0 | +0.012 | 110.07 MHz | 71.8 MHz |
| aop1 (a telemetry constant changed) | 0 | +0.016 | 110.19 MHz | 73.9 MHz |
| aop2 (changed again) | 0 | +0.030 | 110.18 MHz | 79.1 MHz |
| **aoz, the source of 857d136c (on the card)** | **0** | **+0.002** | **110.02 MHz** | 72.8 MHz |

With the early socket request (§10.5 of the scoping doc); fx2 is on the card:

| build | failing | worst slack |
|---|---|---|
| fx1 (the request with two RDY terms it did not need) | 6 | −0.062 |
| **fx2, the committed source** | **0** | **+0.008** |
| fxp1 (a telemetry constant changed) | 0 | +0.047 |
| fxp2 (changed again) | 0 | +0.006 |

fx1's six failures were spread over paths that were already at the edge
(none in the new logic); taking the two terms out, which simulation showed
change nothing, took their load off the sequencer's registers.

No setup or hold violation in any of the four, and they are four
placements: every build of this project carries its own build stamp, so
no two netlists are the same. Step paths of aoz: core → core +0.22, core →
near address +0.04, near memory → core +0.07, clock enable into the core
+0.39. As in the probe, the slacks are small because the tool stops when
the constraint is met; the probe's 116 MHz build (114.3 MHz reached) is the
measure of margin, and it has not been repeated on this build. A first
build without the near memory's floorplan failed by 0.8 ns: the floorplan
is part of the design.

Size: 6,591 logic cells (5,737 LUT, 854 ALU), 3,177 FF, 179 of 340 block
RAMs (fx2: 6,571 logic cells, 3,178 FF, 179 block RAMs). At synthesis the accelerator is about 4,100 of the LUTs and the
connector about 1,500.

Not verified, not done:

- **The top level is not simulated.** `accel_only_top.sv` is wiring: PLLs,
  the connector with its tie-offs, two-flop crossings, the key compare,
  the slot pins. The slot side's passivity is by inspection of that file.
- The firmware (Sep 13 2026) arms and controls it, alone and beside an
  a2p25 (section 3); nothing else of the firmware has been looked at
  against this build (its DDR3-ready input is tied high; volumes, video, USB and DP
  status read as nothing).
- The build with the early socket request has run on hardware for one
  boot and one benchmark (section 3); nothing longer.
- No speed control or Zip / TransWarp registers (S7e), no ROM 3.
- The reset rule's gateware sequencer (`gs_socket_arm_seq`) is not in this
  build; arming is timed by the firmware, as in S6.
- The timing probes were not rebuilt after the broker gained the hold
  (they tie it off); their recorded results are those of commit `caa48d98`.
- Open from before: the write of a step completing in the clk /RES is
  seen is not exercised by the bench.

## 5. Building

    GPRJ=a2mega_accel_138B.gprj RESOURCE_BASELINE=none tools/build.sh a2mega all
    GPRJ=a2mega_accel_138B_trace.gprj RESOURCE_BASELINE=none tools/build.sh a2mega all   (section 3n)

The a2mega directory holds many projects: always name the project, and
check the build stamp (`CORE:` line) before trusting a hardware test.
Regenerate the constraints with `python3 hdl/accel_probe_138b/gen_sdc.py`
and `python3 hdl/accel_probe_138b/gen_floorplan.py accel`.

Provenance of tr0, tr1 and tr2 (the trace build of section 3n,
`a2mega_accel_138B_trace.gprj`; three placements with no violation;
tr2 flashed 2026-11-21): sources = the commit that adds this paragraph; tool
V1.9.12.01, the same device. tr0: build stamp `20261121170651`,
bitstream sha256 `beed63d597b3af4d…39c5e123d4ecb81f`, of the lines that
are not header `738cf8356621cc10…ec856ac257004b89`. tr1:
`20261121172603`, `7d42b80e79f2585e…f51c52cacb5c0fb5`,
`6dea409032bff951…130768bc8d5b7f6a`. tr2: `20261121173753`,
`bbd358d2c686487a…2ed38234c5db4dca`, `06b286dcd9d43f9a…ce9a4f2396f43757`.
Kept as `impl/pnr/a2mega_accel_138B_tr0.fs` to `_tr2.fs` with their
reports.
**tr2 flashed 2026-11-21** over USB with the machine off, first
attempt, read-back verified, the card checked by its serial number
before it. Not yet loaded.

Provenance of p1r (p1 built again on 2026-11-21, after the scratch
directory with the kept bitstreams was emptied; for the comparison of
section 3m; flashed the same day): sources = commit `43cf0c96` in a separate
tree, with `prebuild.tcl` writing p1's own build stamp
`20261114213411` in place of the time of day, so the netlist is the one
p1 was placed from; tool V1.9.12.01, the same device. The report
repeats p1's to the last figure: no violation on any clock, worst slack
beyond the 0.5 ns +0.009 on the same path (the broker's run copy into
the last-address telemetry register), 13,215 logic cells, 11,918
flip-flops, 12,644 slices, 182 block RAMs; built in 4 minutes. The
file's sha256 is not p1's (`0be57353dadfe56f…0d5eddbc84d62e9e` against
`53133d79…`): the bitstream's header carries its time of creation, and
only the whole file's sum of p1 was recorded, so that the bits are p1's
is likely and not proven. The sum of the lines that are not header:
`4eb46b8e6092f363…31b4cbdd4c63c32b` (se2's, for the next time:
`f00f0e2151d1e4a6…bf3d103d2c3fad86`). Kept as
`impl/pnr/a2mega_accel_138B_p1r.fs`, with its timing report beside it;
se2 as `a2mega_accel_138B_se2.fs`.
**p1r flashed 2026-11-21** over USB with the machine off, first
attempt, read-back verified, after the card on the cable was checked
by its serial number (the first cable offered was the a2p25's). Not
yet loaded: the board has to be power-cycled.

Provenance of se0, se1 and se2 (the changed slot 6 rule and the
repair of a ROM copy begun under a RDY stall, section 3m; three
placements with no violation; se2 flashed 2026-11-16 07:26): sources = commit
`42b046a0`; the same tool and device. se0: build stamp
`20261115215920`, bitstream sha256
`175290c2fff44805…0feb6c19749ec7ee`. se1: `20261115222151`,
`8e7462c044c1a56a…fd8e056c7468c6f0`. se2: `20261115224949`,
`6053309260dd25bf…eff81916f3c07492`.

Provenance of sd0, sd1 and sd2 (the changed slot 6 rule, section 3m;
**two placements of three with no violation: not a closed build**; none
flashed): sources = commit `ba398b71`; the same tool and device. sd0:
build stamp `20261115204328`, bitstream sha256
`fbd43ef7d8b0c212…c45017b7a0aa4cc0`. sd1 (19 setup violations):
`20261115210940`, `a7563e040fda2d73…f33399564d4e0aa8`. sd2:
`20261115211624`, `95917a397d038a0f…2c6af4644759ac24`.

Provenance of sc0, sc1 and sc2 (speed control, section 3m; three
placements with no violation; sc1 flashed 2026-11-15 15:43): sources = commit `8df36beb`
(the bench alone changed after it); the same tool and device. sc0: build
stamp `20261115133453`, bitstream sha256
`40adcdbdd4aba2eb…0ddc1e832d552f46`. sc1: `20261115134447`,
`05d20b619356a6e0…7407890b74997d20`. sc2: `20261115140000`,
`a68adb5bf2c634a4…073399cfa4044519`. The firmware to go with it on the
test card: its own source `e96b40cf` with the four-file change of
sections 3l and 3m, sha256 `4691e43a65f1207f…3646f055191d74be`
(flashed 2026-11-15 15:43).

Provenance of p1 and p2 (the closed build of section 3k: section 3j's
logic, the period counter change, `Place_Option` 1; three placements
with no violation; p1 flashed 2026-11-15): sources = the commit that adds this
paragraph; the same tool and device. p1: build stamp `20261114213411`,
bitstream sha256 `53133d7907a9ee8a…bb40bcf65b6bc376`. p2: build stamp
`20261114215549`, bitstream sha256 `777cd2829cf717a9…80f3d3cb912e0ba9`.

Provenance of q1 (the write rule with the shadow register, section 3j;
an experiment, one clean placement of a source that closed two of five;
flashed 2026-11-14 19:20): sources = commit `b1aad584`; the same tool
and device; build stamp `20261114181935`; bitstream sha256
`2511dfa52b681192…31edcf9d3ad3`.

Provenance of x0 (the copy of the Mega II's RAM, section 3h; an
experiment, one clean placement of a source that closed two of three;
flashed 2026-11-14 15:39): sources = commit `7ab26835` without `rtag_q`
and with the `mc(2, NMQ, nh_dq)` line in `gen_sdc.py`; the same tool and
device; build stamp `20261114124800`; bitstream sha256
`308790d3bf5d88f2…6e5418a29d98`.

Provenance of t0 (the line memory, section 3e; flashed 2026-11-14 08:53):
sources = the
commit that adds this paragraph; the same tool and device; build stamp
`20261114020946`; bitstream sha256
`dd5d16229b2438a8…6339edea96909e7720`.

Provenance of m0 (the empty-entry fix, section 3c; on the card): sources
= the commit that adds this paragraph; the same tool and device; build
stamp `20261113175022`; bitstream sha256
`aaa7ca899b7fcd5a…bdfa95db0a7fa73711`.

Provenance of k0 (the DDR3 build at 105 MHz, section 3b): sources = the
commit that adds this paragraph; the same tool and device; build stamp
`20261113103322`; bitstream sha256 `4aad21337ff394c4…b620fb41072f`.

Provenance of fx2 (the early socket request, on the card): sources = the
commit that adds this paragraph; the same tool and device; build stamp
`20261112184610`; bitstream sha256 `cc7d3cb5a33f90d8…2476b571f5`.

Provenance of aoz: sources = the commit that adds this page; Gowin
V1.9.12.01, GW5AST-LV138PG484AC1/I0 device version B; build stamp
`20261112104329`; bitstream sha256 `48c06bbc72bca6da…7cb8c04baa`. The
bitstream options are the 138B full build's (`impl/a2mega_138B_process_config.json`
copied, with the probe's place and route options).
