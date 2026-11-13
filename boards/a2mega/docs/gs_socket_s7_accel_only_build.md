# S7 — the accelerator-only build

Status: built and simulated, 2026-11-12. **Not flashed, never on hardware.**
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
- No framebuffer, DDR3, DisplayPort, slot cards, slot ROM or I/O decode, or
  OSD. The DDR3 and DisplayPort balls are not ports of this top; they take
  the bitstream's unused-pin default (input with pull-up), as in the DP test
  builds.

Clocks: the 50 MHz crystal; `clk_logic` (the board PLL's 54 MHz) for the
connector; `clk_gs` for the accelerator, from `gowin_gs_pll`, **the S6 socket
PLL, unchanged: 110 MHz**, so the core steps at **55 MHz**.

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
| hold samples | `{mode, 5A}`; mode: bit 7 ROM image valid, 6 copy busy, 5 core held, 4 fast, 3 card memory, 2 cxrom |

`gs trace` prints 64 lines; here line *n* is telemetry word *n* (read
`bank:addr data` as one 32-bit number): 0 core steps, 1 socket cycles for
the core, 2 posted writes, 3 clks waited for the write buffer, 4 local
reads in place, 5 local reads that waited, 6 `{verify count, 00, mode}`,
7 address of the last verify mismatch, 8 `{copies, 00, sum of the image's
bytes mod 65536}`, 9 `{shadow, CYAREG, SLTROMSEL, STATEREG}`, 10 the other
switches, 11 PHY bus cycles. LEDs (active low): heartbeat, socket ours,
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

After fast mode has run, register 29 must not go back to 00 without a
reset: the machine's copy of banks 00/01 is stale by design. `0x0A` is the
way back to PHI2 pacing, and its verify count then means nothing.

## 4. What is verified, and what is not

Verified in simulation (the broker suite, `hdl/twgs/sim/broker`):

- `gs_accel` runs beside the bench's hand-wired design on the same socket
  inputs, and **every output pin is compared in every clk** (`ACCEL_SHADOW`),
  in native, paced, fast and toggling modes, with interrupts, RDY stalls, a
  BE pause, warm resets, at 100, 108 and 110 MHz. ROM 01 boot at 110 MHz:
  50,850,536 clks, no difference; over the suite's 35 runs with the module beside the design, 290,331,590 clks.
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

With the early socket request (§10.5 of the scoping doc), not flashed:

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
  against this build (DDR3 ready is tied high; volumes, video, USB and DP
  status read as nothing).
- **The build with the early socket request has not run on hardware.**
- No expansion RAM (S7d), no speed control or Zip / TransWarp registers
  (S7e), no ROM 3.
- The reset rule's gateware sequencer (`gs_socket_arm_seq`) is not in this
  build; arming is timed by the firmware, as in S6.
- The timing probes were not rebuilt after the broker gained the hold
  (they tie it off); their recorded results are those of commit `caa48d98`.
- Open from before: the write of a step completing in the clk /RES is
  seen is not exercised by the bench.

## 5. Building

    GPRJ=a2mega_accel_138B.gprj RESOURCE_BASELINE=none tools/build.sh a2mega all

The a2mega directory holds many projects: always name the project, and
check the build stamp (`CORE:` line) before trusting a hardware test.
Regenerate the constraints with `python3 hdl/accel_probe_138b/gen_sdc.py`
and `python3 hdl/accel_probe_138b/gen_floorplan.py accel`.

Provenance of fx2 (the early socket request, not flashed): sources = the
commit that adds this paragraph; the same tool and device; build stamp
`20261112184610`; bitstream sha256 `cc7d3cb5a33f90d8…2476b571f5`.

Provenance of aoz: sources = the commit that adds this page; Gowin
V1.9.12.01, GW5AST-LV138PG484AC1/I0 device version B; build stamp
`20261112104329`; bitstream sha256 `48c06bbc72bca6da…7cb8c04baa`. The
bitstream options are the 138B full build's (`impl/a2mega_138B_process_config.json`
copied, with the probe's place and route options).
