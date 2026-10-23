# S7 — Accelerating the in-socket 65C816 (scoping)

Status: scoping, 2026-09-13. Follows S6 (`gs_socket_65816_scoping.md` §8b:
auto-arm, reset sequencing). Nothing here is built; the numbers are from the
S4–S6 builds and the bench.

## 0. Summary

S2–S6 put a 65C816 core in the IIgs CPU socket and made it a transparent
replacement: the core is clocked by PHI2 from the socket pin, one core step
per motherboard cycle, 2.86 MHz on fast cycles and 1 MHz on Mega II cycles.
S7 makes it an accelerator that is faster than every card that has been sold
for the machine.

- **Target: ≥ 1.5× the AppleSqueezer GS, i.e. ≈ 9–10× a stock 2.8 MHz IIgs**
  on the classic Applesoft benchmarks, and GS/OS, games and disk I/O working
  as they do behind a TransWarp.
- **Means: the core runs on its own clock (28 MHz planned, 40 MHz ceiling)
  from memory on the card, and the socket is used only for I/O, the Mega II
  banks and video shadowing.** The speed comes from owning the memory, not
  from the clock. That is what the benchmark table says about the existing
  cards.
- **Fits the 138B**: banks $00/$01 and a ROM 01 image are 256 KB of the
  418 KB of block RAM that is free today; expansion RAM comes from the DDR3.
- **The work is the memory-map replica** (language card, ALTZP, RAMRD/RAMWRT,
  80STORE, shadowing, ROM select) and a two-clock-domain bridge in the socket
  PHY. The MiSTer IIgs core we already borrowed the CPU from has both as
  separable, unit-tested modules.

## 1. What the benchmarks say

Times in seconds (ed, 2026-09-13), speedup = stock ÷ card:

| CPU | clock | Sieve | String | Float 1 | Float 2 | Fibonacci | Integer |
|---|---|---|---|---|---|---|---|
| Stock | 2.8 | 410 | 1151.2 | 472 | 1535 | 2006 | 1553.4 |
| TWGS | 15 | 99 (4.1×) | 270 (4.3×) | 92 (5.1×) | 317 (4.8×) | 605 (3.3×) | 307 (5.1×) |
| TWGS | 13.75 | 108 | 292 | 87 | 394 | 634 | 330 |
| TWGS | 12.5 | 117 | 304 | 111 | 382 | 645 | 347 |
| ZipGS | 16 | 98 (4.2×) | 259 (4.4×) | 123 (3.8×) | 395 (3.9×) | 507 (4.0×) | 420 (3.7×) |
| ZipGS | 15 | 99 | 262 | 128 | 415 | 523 | 431 |
| ZipGS | 14 | 110 | 282 | 135 | 432 | 548 | 443 |
| AppleSqueezer GS | | 69 (5.9×) | 193 (6.0×) | 89 (5.3×) | 291 (5.3×) | 339 (5.9×) | 263 (5.9×) |

Three facts fall out:

1. A 15 MHz CPU with perfect memory would be 15 ÷ 2.8 = 5.4× stock. TransWarp
   and Zip reach 3.3–5.1× at 15–16 MHz. Their caches are write-through and
   small (8–32 KB), so **every store and every miss costs a full 350 ns
   motherboard cycle** — 5–6 of their own cycles. Fibonacci, the most
   stack- and store-heavy test, is where they lose most (3.3×).
2. The AppleSqueezer is a flat 5.9× on every test, which is the signature of
   a core (≈ 16.5 MHz equivalent) whose reads *and writes* never leave the
   card except for I/O and shadowed video. **It owns banks $00/$01, the ROM
   and its own 8 MB of expansion.**
3. Nobody accelerates $E0/$E1 or I/O: those are Mega II cycles at 1 MHz for
   every card, ours included. Code that draws SHR by writing $E1/2000 directly
   is bus-bound on every accelerator.

So the design target is Squeezer's memory model with a faster core.

## 2. Architecture

### 2.1 Two clock domains, one socket

Today (`hdl/twgs/gs_socket_phy.sv`): the core's clock **is** PHI2 (inverted
pin, CE = 1); the 110 MHz `clk_gs` PLL clock only runs the pin sequencer that
places address/data inside each PHI2 cycle.

S7: the core runs on **`clk_core`**, and the PHY becomes a **bus-cycle
broker** between the core and the socket:

- **Local hit** (see §2.2): the memory answers in one `clk_core` cycle; the
  core never waits.
- **External cycle** (I/O, $E0/$E1, shadow write, slot space): the broker
  stalls the core (CE low or RDY_IN low — the core already honours RDY_IN),
  waits for the next PHI2 cycle boundary, performs exactly one socket cycle
  with the timing the PHY already produces, then releases the core with the
  data. Stretched Mega II cycles and the FPI's RDY are followed as today.
- **Every PHI2 cycle still shows a legal 65816 cycle on the socket.** While
  the core is busy locally the socket presents an *internal* cycle: VDA = VPA
  = 0, the previous address repeated, R/W high, data bus released. This is
  the behaviour the phantom-`$C028` fix (S5) already gave the PHY, and it is
  what a real 65816 does during its own internal cycles, so the FPI is
  satisfied. E and M/X keep reflecting the core's real mode.
- Interrupts, NMI, ABORT, /RES and RDY are sampled per PHI2 cycle by the PHY
  (as now) and crossed into `clk_core` with two-flop synchronisers; the core
  sees them at instruction boundaries as before.

**Clock choice.** Simplest: run the core *in the `clk_gs` (110 MHz) domain
with a clock enable* every 4th cycle → 27.5 MHz effective, a single clock
domain with the pin sequencer, no CDC inside the PHY, and the speed step is a
CE ratio (÷4 = 27.5, ÷8 = 13.75, ÷16 = 6.9 MHz, native = PHI2-paced). The
alternative, a dedicated PLL output, buys nothing until the core needs more
than 27.5 MHz. Ceiling: the core's PHI2 domain closes at 40.4 MHz in builds
13/14 with no constraint pressure; S2 synthesis estimated ~52 MHz single-
cycle. Plan for 27.5 MHz; treat 40 as the stretch.

### 2.2 Memory of record on the card

| region | where | policy |
|---|---|---|
| banks $00/$01 (128 KB) | BSRAM, 64 blocks | reads local; writes local **and** forwarded to the bus when the address is in a shadowed video range (§2.4) |
| ROM ($FE/$FF on ROM 01 = 128 KB; $FC–$FF on ROM 3 = 256 KB) | BSRAM, 64 blocks for ROM 01 (ROM 3: 128 blocks, or DDR3 + cache) | read-only local; loaded once from the motherboard at bring-up (§2.6) |
| expansion banks $02–$7F (up to 8 MB) | DDR3 behind a BSRAM cache (16–32 blocks) | local; nothing on the motherboard is involved (the machine's own memory card, if any, is bypassed) |
| $E0/$E1 (Mega II slow RAM) | motherboard | every access is a socket cycle |
| I/O $C000–$C0FF, slot ROM $C100–$CFFF that is not internal, $C800 space of slot cards | motherboard | every access is a socket cycle; the soft-switch writes are *also* decoded locally to keep the map replica current |
| language-card RAM in $00/$01 $D000–$FFFF | part of banks $00/$01 above | local |

Why banks $00/$01 can be owned outright: on the IIgs nothing but the CPU
reaches fast RAM — the Mega II cannot, video shadowing is performed by the
FPI on the CPU's *write cycles*, and slot DMA into fast RAM is exotic
(the TransWarp has exactly the same blind spot and nobody notices). The one
thing we must preserve is the shadowing side effect, hence §2.4.

### 2.3 The memory-map replica

Everything in §2.2 depends on knowing, for each core access, which physical
region the machine would have used. That is the FPI's and the Mega II's
decode, driven by soft switches the core itself writes. The replica tracks
them from the core's own bus cycles (they all pass through the broker):

| state | set by | matters for |
|---|---|---|
| LCRAM / LCBNK2 / RDROM / LC write-enable | reads of $C080–$C08F (the two-read protocol) | $D000–$FFFF in $00/$01: ROM image vs LC RAM bank 1/2 |
| ALTZP | $C008/$C009 | zero page, stack and LC RAM: main vs aux (bank 0 vs 1) |
| RAMRD / RAMWRT | $C002–$C005 | $0200–$BFFF reads/writes: bank 0 vs 1 |
| 80STORE, PAGE2, HIRES | $C000/$C001, $C054/$C055, $C056/$C057 | text/hires page aux switching overrides RAMRD/RAMWRT |
| INTCXROM, SLOTC3ROM, INTC8ROM | $C006/$C007, $C00A/$C00B, $CFFF | $C100–$CFFF: internal ROM (local) vs slot card (bus) |
| SHADOW ($C035) | write | which video ranges of $00/$01 must be forwarded (bits are inhibits: text1, hgr1, hgr2, SHR, aux hgr, text2 on ROM 3, IOLC) |
| CYAREG ($C036) | write | bit 7 fast/slow (native pacing), bit 4 shadow-all-banks, bits 0–3 disk-slot motor detectors (slowdown, §2.5) |
| STATEREG ($C068) | write | all of the above bits in one register (GS/OS uses it) |
| ROMBANK ($C028, ROM 01) | write | which half of the ROM image |

`rtl/mmu.sv` in the MiSTer Apple IIgs core is exactly this function: pure
combinational, `{bank, addr, we}` + register state → physical region and chip
enables, with a C++ golden model and a sweep test (`vsim/mmu_tb.cpp`). It is
GPL-3 like the CPU core we already ship. Port it, keep its tests, and add
our two outputs: *local?* and *forward-write?*.

### 2.4 Write forwarding (shadowing)

For a write into banks $00/$01 the replica decides three things:

1. update the local copy (always, unless the address is I/O);
2. **forward to the socket** if the address is in a range the machine shadows
   into $E0/$E1 and the corresponding SHADOW inhibit bit is clear — text page
   1 ($0400–$07FF), hires 1/2 ($2000–$5FFF), double-hires (bank 1 $2000–$5FFF),
   SHR (bank 1 $2000–$9FFF), text page 2 on ROM 3 — or if it is I/O or
   language-card-inhibited space (SHADOW bit 6);
3. otherwise stay local.

The forwarded write is an ordinary bank-$00/$01 write on the socket, so the
FPI writes the motherboard's fast RAM *and* performs its shadow copy exactly
as it would for a real CPU; the Mega II's display stays right and the
motherboard's copy of those pages stays coherent. Forwarded writes are posted
(the core continues; the broker owns a one-deep write buffer and stalls only
if a second external cycle arrives before the first completes), which hides
most of their cost behind local work.

### 2.5 Speed control and slowdown rules

Every accelerator has to drop to native speed around things that count
cycles. The rules the existing cards use, which we adopt:

- **CYAREG bit 7 = 0 (the machine's own "slow" mode)**: run PHI2-paced,
  i.e. today's S6 behaviour. The Control Panel's Normal/Fast setting keeps
  meaning what it means.
- **Disk slots**: while a slot's motor-detect bit in CYAREG is set and its
  $C0n0 I/O range has been touched, run at native for ≈ 300 ms after the
  last touch (the FPI does the same for its 2.8 → 1 MHz step). Configurable
  per slot like the TransWarp's slot table.
- **Speaker and timing loops**: the Zip/TransWarp approach — slow down on
  $C030 access for a few ms — is available and configurable; default on.
- **Interrupts**: TransWarp's optional "slow on interrupt" is advisory; not
  needed by GS/OS. Off by default.

Software-visible control: emulate the **ZipGS $C058–$C05F unlock protocol**
and the **TransWarp $BC0000 latch**, so existing control panels, CDAs and
games can detect an accelerator and set its speed. Both exist as small
modules in the MiSTer core (`zipgs_regs.sv`, `twgs_regs.sv`, verified
against KEGS/GSplus and the TWGS ROM). The card's own menu/telnet gets a
speed selector (native, 6.9, 13.75, 27.5 MHz) and cache/forwarding toggles,
which is also the bring-up ladder.

### 2.6 Bring-up sequence (adds to S6)

S6's sequencing is unchanged: hold, release with the socket off, arm 20 ms
later, never inside the ROM 01's re-assert window. S7 adds, after arming and
before the core leaves reset:

1. Read the ROM image through the socket into BSRAM at native speed (128 KB
   at 2.8 MHz ≈ 50 ms; done once per power-up, or cached in the ESP32's flash
   and verified against a few probe reads).
2. Banks $00/$01 start empty; RAM is not assumed to hold anything at power-on.
   (Warm resets keep the local copy, as the machine keeps its RAM.)
3. Soft-switch state starts from the reset defaults the FPI/Mega II apply;
   the replica mirrors them.

### 2.7 A card-only opportunity (noted, not S7)

Our own emulated slot cards (HDD block device, Disk II, Uthernet, Mockingboard)
live in the same FPGA as the core. Their I/O and ROM space could be served
inside the chip with no socket cycle at all, and the HDD block transfer
could run at core speed. No other accelerator can do that. It changes the
$C0n0/$Cn00 decode for our slots only and is a natural S8.

## 3. Performance model

Per core cycle at 27.5 MHz (36 ns): local hit = 1 cycle; forwarded write =
posted, usually free; external cycle = wait for a PHI2 slot ≈ 350 ns fast /
980 ns slow ≈ 10 / 27 core cycles.

| workload | external fraction | expected speedup vs stock |
|---|---|---|
| Applesoft benchmarks (bank 0 + ROM; text writes forwarded) | ≈ 1–3 % | **≈ 9–10×** (Squeezer 5.9×, TWGS 4–5×) |
| GS/OS desktop (fast RAM + SHR writes via bank 1 shadow) | 5–15 % | 4–7× |
| SHR code writing $E1 directly | 30–60 % | 1.5–2.5×, same as every card |
| disk I/O windows | native by rule | 1× while active |

At the 40 MHz ceiling the compute-bound figure becomes ≈ 14×. The
benchmark set in §1 is the acceptance test: **all six under Squeezer's
times ÷ 1.5**.

## 4. Resources and timing (138B)

| item | BSRAM blocks | notes |
|---|---|---|
| build 14 today | 131 / 340 | socket, event log, HDD, video, DOC |
| banks $00/$01 | 64 | 2 KB per 18 Kbit block at byte width |
| ROM 01 image | 64 | ROM 3: 128, or DDR3 + cache |
| expansion cache | 16–32 | 32–64 KB direct-mapped, write-back to DDR3 |
| **total** | **275–291 / 340** | ROM 3 in BSRAM: 339–355 → ROM 3 goes to DDR3 |

Logic: the replica and broker are small (the MiSTer MMU is 400 lines of
combinational decode); the 64:1 and 128:1 BSRAM read muxes are the widest
structures and are trivial at 27.5 MHz. Timing risk is the usual 138B one:
the DP block's knife edge (clk100/clk_sym/clk_pix at +0.01…+0.3 ns); the S7
logic stays in `clk_gs`/`clk_core` and does not touch it. Socket builds take
21–41 min (S4 record); expect the same.

DDR3: 512 MB on the 138K SOM, seven ports already arbitrated; one more
client for the expansion backing store.

## 5. Risks and open questions

1. **Fidelity of the replica.** A wrong decode is a crash, not a slowdown.
   Mitigations: the MiSTer golden-model sweep; a "native" mode that bypasses
   the local memory entirely (today's S6 behaviour) selectable at runtime; a
   "verify" mode for the bench that performs every local read *also* on the
   bus and logs mismatches into the event log (the trace ring exists).
2. **ROM 01 vs ROM 3.** Different ROM size, TEXT2 shadow bit, $C028 ROMBANK,
   a few soft-switch details. Scope S7 to ROM 01 (the bench machine) with
   the ROM 3 differences kept as parameters, not forgotten.
3. **Coherency with anything else that touches fast RAM.** Slot DMA cards
   (rare); the a2mega's own emulated cards do not DMA. Document it, provide
   the native-mode escape.
4. **The machine's own expansion card.** If one is fitted, GS/OS will size
   memory from *our* responses, and the motherboard card is simply idle.
   Fine, but the sizing must be consistent (a menu setting: 1/4/8 MB).
5. **Reset and the 10 ms window** (S6): unchanged; the core is still armed
   20 ms after the edge. The ROM copy (§2.6) happens after arming.
6. **Timing-sensitive software** beyond disks: the standard accelerator
   slowdown set (§2.5) has covered the TWGS/Zip installed base for 35 years;
   we ship the same knobs.
7. **ADB micro/SYNC timing**: the ROM's first ADB exchange happens at native
   speed on the bus regardless; no change.

## 6. Verification plan

1. **Replica unit tests**: port `mmu.sv` + its C++ golden model; extend the
   model with *local?* / *forward?* and sweep all soft-switch combinations
   over both banks. Pure simulation, no hardware.
2. **Broker simulation**: the existing PHY bench against the FPI clock model
   (`clock_divider.v`) with the core on `clk_gs`/CE; assertions that every
   PHI2 cycle carries a legal cycle and that external cycles meet the S2
   timing rules; posted-write ordering.
3. **Bench ladder** (each step a runtime toggle, native mode always one
   command away):
   1. native mode = S6 (regression);
   2. local ROM reads only, RAM still on the bus;
   3. banks $00/$01 local with forwarding, still PHI2-paced (verify mode
      compares against the bus);
   4. speed steps 6.9 → 13.75 → 27.5 MHz;
   5. expansion RAM from DDR3;
   6. the six benchmarks, GS/OS boot from the HDD device, a disk-image boot
      through the Disk II device, a few timing-sensitive games; Zip and
      TWGS control panels.

## 7. Work plan

| step | content | deliverable |
|---|---|---|
| S7a | memory-map replica (port MiSTer `mmu.sv` + tests, add local/forward outputs) | sim-tested module, PR |
| S7b | broker: core on `clk_gs`/CE, external-cycle stall/resume, internal cycles on the socket, posted write | sim against the FPI model; bench step 1–2 at native pacing |
| S7c | banks $00/$01 + ROM image in BSRAM, write forwarding, verify mode, ROM copy at arm | bench steps 2–3 at native pacing, then speed steps |
| S7d | expansion RAM: DDR3 client + BSRAM cache | bench step 5 |
| S7e | speed control: CYAREG pacing, slot/speaker slowdown, Zip + TWGS register models, menu/telnet | compatibility runs |
| S7f | benchmarks and acceptance | test_log rows, numbers against §1 |

Each step is a PR against `epic-merge-prep` on top of S6, per the project
rule; S7a and S7b can proceed in parallel and neither needs the bench.

## 8. Questions for ed

1. Speed steps to expose (proposal: native, 6.9, 13.75, 27.5 MHz; 40 as a
   bench-only experiment).
2. ROM 3 in scope for S7 or parameterised only?
3. Expansion RAM default size (proposal 8 MB) and whether to hide the
   motherboard's memory card entirely.
4. Software interface: Zip protocol, TransWarp latch, both, or only our own
   menu? (Proposal: both models, they are small and existing software expects
   one of them.)
5. Is the "serve our own slot cards inside the chip" S8 worth moving up —
   it is the one thing that makes disk-bound work faster than any card.
