# Ensoniq DOC5503 with DDR3-backed wavetable RAM: pipelined-fetch design

Status: **rev 3 — rev 2's word cache improved but did not fix the hardware
(stale storm + residual FB corruption); the real DDR3 port contract was
then derived from the RTL cycle-by-cycle (§12), revealing the failure is
lowest-priority service-TAIL latency plus a framebuffer CAPACITY margin
that DOC scheduling cannot create. Rev 3: 16-byte-line burst fetches
(~4× less arbiter occupancy again), two-line store with lookahead issue
(deadline ×16), and FB-aware issue gating. Re-validated against the
corrected contention model, which also reproduces the rev-2 field failure
— GO for re-integration**
Prototype RTL: `hdl/sound/doc5503_pipelined.sv`
Differential sim: `sim/doc5503/` (see its README for how to run)

## 1. Executive summary

The a2mega currently spends 32 BSRAM blocks (27% of the GW5AT-60) on the
64 KB Ensoniq wavetable RAM (`boards/a2mega/hdl/sound/ensoniq_bsram.sv`)
because the baseline `doc5503.sv` requires the waveform byte within its own
service slot (~978 ns budget), which DDR3 through the arbiter/CDC stack
cannot guarantee (worst-case tail ~1.0–1.2 µs).

The proposal — fetch the byte one service period ahead and consume it at the
oscillator's next service — **works**, and with one addition
(*prefetch-while-halted*) it works better than proposed: the pipelined
variant is **bit-exact and time-aligned** with the baseline in steady state,
one-shot termination, swap-mode looping (zero drift over 50+ loop
iterations), SYNC/AM operation, E1 changes, and FC/volume/halt register
writes. The one-service-period pipeline manifests **only as data staleness**
(one sample per event) in three narrow, benign cases, plus a graceful
stale-repeat fallback under memory overload. No register-write FIFO is
needed. The failure mode that would have been a real bug (a stale 0x00
terminator consumed at note-on, instantly re-halting the oscillator) was
identified analytically and is prevented structurally in the prototype;
simulation confirms zero unclassified deviations across ~52,000 service
slots including a forced-overload phase and its recovery.

Compared with the fallback plan (moving audio production to the ESP32,
which costs at least a service period of FPGA→ESP32→FPGA latency and
abandons cycle-locking), the pipelined DOC adds **zero** output latency and
stays fully cycle-locked.

**Hardware update (rev 2):** the rev-1 prototype was integrated and run on
a2mega hardware. DOC *semantics* held up (music plays correctly), but its
fetch-every-slot traffic policy saturated the serialized DDR3 arbiter and
starved the framebuffer reader — visible display corruption whenever music
played. §11 documents the field data, the root cause (grant alternation
against a CDC-throttled FB port — FB priority does not protect it), and the
rev-2 traffic policy: a per-oscillator 32-bit **word cache**
(issue-on-word-crossing, ~4× traffic cut), **prime-once** event-driven
priming for halted oscillators (near-zero idle traffic), and
**flush-driven invalidation** from the GLU write path. Rev 2 is
re-validated in simulation with an explicit arbiter-contention model and a
competing FB line-fetch client: all-32-oscillator traffic ≈ 277k fetches/s
(vs 895k), zero FB line-deadline misses outside forced overload, zero
fetch drops in all phases.

**Hardware update (rev 3):** rev 2 on hardware improved but did not fix the
failure (stale storm, drops, residual FB corruption). The DDR3 port
contract was then derived cycle-by-cycle from the CDC/arbiter/framebuffer
RTL (§12): the DOC is the lowest-priority client of a ~90%-utilized
serialized arbiter whose service tails reach tens of µs, and the FB's line
fetch consumes most of each line window — FB integrity is a CAPACITY
question that DOC scheduling cannot fix, only cheaper grants can. Rev 3:
**16-byte-line 4-beat burst fetches** (~4× occupancy cut again, to ~2.6%
with all 32 oscillators), a **two-line store with lookahead issue**
(deadline ×16, absorbing the tails), and **FB-aware issue gating**. The
corrected contention model reproduces the rev-2 field failure (2019/2337
FB line deadlines missed) and rev 3 passes it clean (0 missed, 0 drops).
Recommendation: **GO** — re-integrate with the rev-3 interface (§8),
reclaim the 32 BSRAM blocks, keep the BSRAM path behind a per-board
parameter until hardware confirms both audio and display.

## 2. Timing budget

All numbers at the IIgs 7.159 MHz DOC clock; service slot = 8 input clocks
= 1.117 µs; scan = (N_enabled + 2) slots.

| Quantity | Value |
|---|---|
| Baseline in-slot fetch deadline | ~7/8 slot = **978 ns** (hard, with 0x80 timeout) |
| DDR3 read, uncontended (through `ddr3_ports` + `ddr3_port_cdc`) | ~550 ns |
| DDR3 read, worst-case tail (burst grant drain + tRFC 260 ns + round trip) | ~1.0–1.2 µs → **violates baseline deadline** |
| Pipelined fetch deadline (one full service period) | (N+2) × 1.117 µs = **3.35 µs** (N=1) … **38 µs** (N=32) |
| Margin vs 1.2 µs worst tail | ≥ 2.8× (N=1), ~30× (N=32) |
| Fetch issue rate, rev 1 (one per slot — **caused the field failure, §11**) | ≤ 894,886 reads/s ≈ 50-90% arbiter occupancy |
| Fetch issue rate, rev 2 (word cache, §11.2) | ≤ ~224k reads/s for address steps ≤ 1 (measured 277k/s with a mixed-step all-32 population); ~zero when idle |
| Sustained throughput requirement (rev 2) | mean service < 4.5 µs per fetch at the ~0.25/slot rate; mean grant ~0.8 µs → **~7× margin**; internal 8-deep queue + 2-deep CDC absorb tails |
| DDR3 arbiter occupancy, rev 2, all 32 running | 13-28% of grant time depending on FC mix; simulated FB client loses no line deadlines (§11.3). Rev-1 occupancy was 50-90%, and priority did NOT protect the FB (CDC grant alternation, §11.1) |

The stale-repeat fallback only engages when a fetch outlives a *full
service period* — in simulation this required forcing 10% of fetches to
6.1 µs against a 4.47 µs (N=1-class) service period; it recovered to
bit-exactness immediately when the overload ended.

## 3. Architecture

NOTE (rev 2): §§3.1-3.4 describe the correctness core, which is unchanged.
Rev 2 generalizes the per-oscillator result store from one BYTE to one
32-bit WORD (tag = wave_addr[15:2], byte lane selected at consume), issues
fetches only on word-boundary crossings, and makes halted-oscillator
priming event-driven — see §11.2 for the traffic policy and the one
additional consume rule it requires (halt-on-zero suppression on stale
consumes, failure mode #14). "Result store entry" below reads as "cached
word" in rev 2.

### 3.1 What "one cycle delayed" actually needs to mean

The naive reading — "output the previously fetched byte, then fetch the
next one" — is **not** sufficient. Working through the baseline FSM
(`osc_request_data` → `osc_handle_data` → `osc_out` → `osc_mix` →
`osc_acc` → halt chain) shows two traps:

1. If the ACC += FC step runs at fetch time but the zero-byte halt decision
   runs at consume time, the machine performs an accumulator add the
   baseline never performs (the baseline skips the add on a zero byte).
   That changes overflow timing and, in swap mode, makes each loop span one
   slot longer than the baseline's — **an accumulating drift** of one slot
   per handoff.
2. If a freshly (re)started oscillator consumes whatever is in its buffer,
   it can consume the stale 0x00 terminator left from its previous note and
   **instantly re-halt** — a real, audible bug (silent note-ons).

The correct construction is a **delayed full machine**: at oscillator k's
service slot, run the *entire* baseline slot semantics (output, zero-halt,
AM volume write, ACC += FC with overflow check, swap/sync/retrigger
side-effects) using the byte fetched at k's previous service, and only then
issue the next fetch from the just-updated accumulator. Every architectural
decision is conditioned on the same data the baseline conditions it on, so
per-oscillator state trajectories are identical, merely fed by a fetch that
happened earlier. Cross-oscillator interactions (swap partner start, sync
restart, AM volume writes) execute inside the delayed machines of both
partners, so relative alignment is preserved exactly.

### 3.2 Prefetch-while-halted (removes the output delay entirely)

Halted oscillators still occupy scan slots. Every un-halt path in this DOC
implementation zeroes the accumulator (host control write with halt=0;
swap-mode partner start), so the first byte a restarting oscillator needs
is always `mem[wtp | 0]`. During halted service slots the pipelined variant
prefetches exactly that byte into the oscillator's result-store entry
(re-issuing only while the stored tag mismatches, so a parked oscillator
settles to a single read).

Consequence: when an oscillator starts, its first sample is already primed
and is consumed in its **first** service slot — the same slot in which the
baseline fetches and plays it. The pipelined DOC therefore produces every
sample in the same service slot as the baseline; the pipeline depth shows
up only as *which memory epoch* the byte was read in. This also makes swap
handoffs exact: while A plays, halted partner B sits with `mem[B.wtp|0]`
primed, and the handoff consumes it with no priming gap — verified over
50+ loop iterations with bit-identical handoff slots (zero drift).

### 3.3 Result store and tagging rules (the correctness core)

Per oscillator: `{data[7:0], tag[15:0] (wave address fetched), valid,
src_run}`. `src_run=1` = fetched by the oscillator's own running pipeline;
`src_run=0` = halted prefetch.

- **Running + src_run entry → consume unconditionally.** For a
  continuously-running oscillator the entry is by construction the byte the
  baseline fetches this slot (the address issued at slot n−1 used the
  post-add ACC, which equals the ACC read at slot n). If the tag mismatches
  it is either a late fetch (one-sample repeat, counted in
  `dbg_stale_fetch_o`) or an external ACC/WTP/RTS rewrite, for which the
  buffered byte is the correct delayed-stream sample. A consumed src_run
  byte can never be a spurious 0x00: had it been 0x00 it would have halted
  the oscillator when first consumed.
- **Running + prefetch entry → consume only on tag match** with the address
  the baseline would fetch this slot. This is what makes note-ons and swap
  starts sample-exact.
- **No valid/matching entry → output centerline 0x80 for one sample, and do
  not evaluate halt-on-zero.** The real 5503 emits exactly this centerline
  sample at a swap-mode switch (see §7), so the degraded case matches real
  silicon.
- **Every halted service slot clears src_run** for that oscillator, and a
  host control write that clears the halt bit does the same. After any
  halt, only tag-matched data may be consumed — this is the structural fix
  for the stale-0x00 bug, including the narrow double-toggle case where the
  host halts and un-halts within one service period while a fetch is in
  flight.

### 3.4 Fetch pipeline

An 8-deep internal FIFO of `{osc, addr, src}` with write/issue/retire
pointers. Issue is gated on a new `wave_available_i` input (wired to the
DDR3 port's `available`; tie high for BSRAM); the `ddr3_port_cdc` request
FIFO queues 2, responses return strictly in order, and each
`wave_data_ready_i` retires the oldest issued entry into the result store.
Enqueue points cover every path on which the oscillator remains running:
normal (post-ACC), free/sync overflow (in the halt chain, after the
halt-bit decision), and retrigger (using the consume-slot address on the
zero-byte path, since ACC did not advance). A full FIFO drops the newest
request (counted in `dbg_fetch_drop_o`; the oscillator self-heals at its
next service slot) — reachable only under sustained memory overload.

## 4. Register-write coherency — the 1-entry-FIFO question, answered

**Verdict: the register FIFO is not needed.** The baseline already applies
all host writes at service-slot granularity (each oscillator samples its
registers once per service, at `OSC_LOAD_REGISTERS`); the pipelined variant
samples them at the same slot boundaries. Per class:

| Register class | Effect point | Pipelined behavior vs baseline | Verdict |
|---|---|---|---|
| **FC** ($00-3F) | ACC += FC add | Adds happen in the same slots with the same values → identical ACC trajectories → identical fetch addresses. Sim: bit-exact through a 16-step FC sweep. | **Safe as-is** |
| **ACC** (internal; host cannot address it — control halt-clear zeroes it) | address generation | External ACC rewrites between fetch and consume yield the delayed-stream byte for one sample (see SYNC below); trajectory identical afterward. | **Safe as-is** |
| **Volume** ($40-5F) | output scaling at consume | Read from the register file in the same slot the baseline reads it; applied to the same byte. Sim: bit-exact through a 16-step ramp. AM-modulated volume writes also bit-exact. | **Safe as-is** |
| **Control** ($A0-BF): halt set | next service slot skips the oscillator | In-flight fetch lands harmlessly in the result store; no halt-on-zero is ever evaluated outside a consume. | **Safe as-is** |
| **Control: halt clear** (note-on) | ACC zeroed, oscillator restarts | Result store is primed with `mem[wtp|0]` by prefetch-while-halted → first sample identical to baseline. Without the src_run downgrade rules this class contained the one real bug (stale 0x00 → instant re-halt); the fix is structural, not a FIFO. | **Safe with tagging rules** (implemented) |
| **Control rewrite with halt=0 on a running oscillator** | this implementation resets ACC on *any* halt=0 control write (MAME resets only on the 1→0 key-on edge — pre-existing baseline deviation, out of scope) | ACC reset + src_run cleared → one 0x80 sample where the baseline plays `mem[0]`. Coincides with the phase-restart discontinuity the baseline itself produces. Sim: exactly 1 such event, as designed. | **Benign, documented** |
| **WTP** ($80-9F), **RTS** ($C0-DF) *as address-mapping inputs* | fetch address computed at issue time | The write's effect on the fetched **address** lands one sample late (the in-flight byte was addressed under the old mapping). RTS's other roles (overflow bit, ACC mask) act at the add, which is slot-aligned → no delay. Sim: 2 one-sample `ADDR_MAP_DELAY` events at WTP retargets, nothing else. | **Benign, documented** — a service-boundary write FIFO would *not* remove this (the delay is issue-to-consume, not write-to-slot); an exact fix would need re-fetch-on-WTP-write, complexity for zero audible benefit (retargets are normally done on halted oscillators) |
| **E1** ($E1) scan length | scan counter compare | Oscillators leaving the scan keep their (valid, tagged) entries and any in-flight fetch retires harmlessly; on re-entry the entry still matches (ACC unchanged) → bit-exact. Entering oscillators that were never scanned prime on their first halted slot. Sim: grow/shrink/restore all bit-exact. | **Safe as-is** |
| **$E0 / IRQ** | — | This RTL implements no IRQ generation (`osc_int_r` is host storage; control bit 3 unused). When IRQs are added, generate them from the same slot's halt events — the C-record streams (all halt events) proved bit-identical, so there is no pipelined-specific IRQ delta. | **N/A today** |
| **Data readback** ($60-7F) | wds register file | Written with the consumed byte in the same slot the baseline writes the fetched byte → readback identical (the W-record streams are the proof). | **Safe as-is** |

One inherent nit, common to any implementation: a host write landing within
a few clk_i of the slot's internal sampling point can apply one slot
earlier or later depending on ns-level timing. The pipelined variant moves
that knife edge by a few 54 MHz cycles; software cannot observe sub-slot
write timing, so this is not an artifact class.

## 5. Failure-mode table

| # | Failure mode | Class | Verdict / mitigation |
|---|---|---|---|
| 1 | Fetch outlives a full service period | stale repeat | **Benign, self-healing**: previous sample repeats for one slot, counted (`dbg_stale_fetch_o`); can never fake a 0x00 halt. Verified under forced overload (phase 8) with immediate recovery (phase 9: zero deviations). |
| 2 | Stale 0x00 consumed at note-on → spurious instant re-halt | **real bug (naive design)** | **Fixed structurally**: src_run downgrade on halted slots + on host un-halt; prefetch consume is tag-gated. Sim: 197 control events bit-identical, zero spurious halts anywhere. |
| 3 | Swap-loop handoff drift (one slot per handoff) | **real bug (naive design)** | **Fixed by the delayed-full-machine construction + prefetch-while-halted**. Sim: 50+ swap loops, handoff slots bit-identical. |
| 4 | Swap/note-on prime not yet landed → one 0x80 sample | graceful degradation | Matches real-silicon swap centerline glitch (§7). Not observed in sim (prefetch always landed in ≥1 halted slot + ~550 ns). |
| 5 | SYNC hard-restart lands between fetch and consume | benign spec deviation | One modulator sample plays the delayed-stream (old-phase) byte instead of the table-start byte, once per restart event; AM carrier volume differs for that one modulation period. Sim: 5 + 5 events, all classified. An exact fix (enqueue a partner re-prime at restart) exists but trades a plausible waveform byte for a possible 0x80 — not recommended. |
| 6 | GLU sound-RAM write vs in-flight fetch | benign | Write visibility is one service period late for that oscillator — the same class of race as CPU-vs-DOC DRAM access on real hardware. Sim (phase 7): every mismatch matched the pre-write byte. |
| 7 | WTP/RTS address-mapping change in flight | benign | One sample fetched under the old mapping (§4). |
| 8 | Internal fetch-FIFO overflow | overload only | Newest request dropped + counted; oscillator repeats one sample and reissues next service. Only reachable with sustained multi-µs memory stalls (forced in sim); real DDR3 tail is a 1.2 µs outlier, not sustained. |
| 9 | Result store cold at first-ever enable | same as baseline | If software enables an oscillator before loading the wavetable, both variants play whatever memory holds; prefetch may additionally be one GLU-write stale for the very first sample. |
| 10 | FC=0 oscillator (frozen ACC) | handled | Rev 2: a frozen walk never crosses a word boundary, so no re-fetch — GLU updates reach it via the flush path (issued_valid cleared → one refresh at its next service). |
| 11 | E1 shrink strands in-flight fetches | handled | Retires into the result store; entry remains tag-consistent for re-entry. |
| 12 | IRQ timing shift | N/A | No IRQ generation in this RTL (§4). |
| 13 | **Arbiter saturation / FB starvation** (rev-1 field failure) | **real system-level bug — mitigated in rev 2, FIXED in rev 3** | Per-slot single-word fetches (~895 kHz) occupied 50-90% of the serialized arbiter. Rev 2's word cache (~9%) was still on the FB's capacity knife edge and its 1-period deadline was inside the lowest-priority service tail (§12). Rev 3: 16-byte-line burst fetches (~2.6% occupancy), lookahead (deadline ×16), FB-aware gating — 0 FB misses / 0 drops in the corrected contention model that reproduces the rev-2 failure. |
| 16 | Jump fetch (table wrap / retarget) stale under gating at very small E1 | benign, counted | Jump fetches keep the 1-service-period path; with E1 tiny (deadline < gate escape) a wrap fetch can consume stale once per event (observed once at E1=4). One counted sample; unrealistic config (music + tiny E1 + SHR display). |
| 14 | Stale word served with an unvisited 0x00 lane (word-cache variant of #1) | handled | halt-on-zero is SUPPRESSED on any tag-miss consume — a stale lane can never fake a terminator. A genuinely missed terminator under overload degrades to a table-end halt (bounded; would surface as a C-record mismatch — none observed). |
| 15 | GLU write bursts during active playback | bounded | Each flush clears issue bookkeeping → running oscillators refresh once per service period during the burst (correctness kept); halted re-priming is deferred by a cooldown (`PRIME_COOLDOWN_SLOTS`) so uploads don't cause prime storms. Sustained upload+playback approaches rev-1 traffic for the RUNNING subset only — rare on real software (uploads precede playback). |

## 6. Simulation methodology and results

NOTE (rev 2): the numbers in this section are the rev-1 correctness run
(latency-only memory model). They remain valid for the DOC-semantics
claims and are superseded on the system side by the rev-2
contention-model run — coverage, traffic and FB-deadline results in §11.3
and `sim/doc5503/README.md`. Rev-2 correctness deltas: the same deviation
classes, plus word-granularity effects (mid-word terminators verified;
stale-consume zero-suppression, failure mode #14).

`sim/doc5503/tb_doc5503_diff.sv` runs the baseline (2-clk memory) and the
pipelined variant (serialized randomized-latency memory: uniform
200–1200 ns + 2% 2 µs outliers, ≤2 outstanding, in-order, data sampled at
completion time) side by side on the same clocks, host bus, and wavetable
contents, and logs four architectural event streams from each
(per-oscillator sample stream W, control writes C, volume writes V, per-scan
mixer outputs M). `compare.py` pairs records by service slot and classifies
every difference; anything unclassified fails the run.

Coverage: steady multi-oscillator free-run (three table sizes/resolutions);
one-shot with 0x00 terminator; swap pair over 50+ loops; SYNC/AM pair
(hard-sync restarts + AM modulation); FC sweep; volume ramp; WTP retarget;
RTS change; halt toggle; control rewrite on a running oscillator; E1
grow/shrink/restore; GLU writes into a playing table; forced-overload
stress (10% of fetches at 6.1 µs vs a 4.47 µs service period) plus
recovery.

Result (~52,000 slots, 18,527 paired samples): **PASS** —

- **Bit-exact and time-aligned** in phases 0–3 and 6 (steady state,
  one-shot, swap loops, E1 changes): zero deviations of any kind.
- C records (every halt/swap/retrigger/host-control event): **identical in
  all phases, including overload** — halt-on-zero timing, swap handoffs and
  the IIgs-verified even-oscillator retrigger are exact.
- Classified deviations, each one sample per triggering event: 2
  ADDR_MAP_DELAY (WTP retargets), 1 PRIME_MISS (control rewrite on running
  osc), 5 SYNC_RESTART + 5 SYNC_AM_VOL (hard-sync restarts). Overload phase
  only: 206 stale repeats, 138 catch-up-lag samples, 2 FIFO drops; recovery
  phase: zero.

Caveats: the RTS scenario changes size/mask but not the effective shift
(mechanism identical to WTP); $60-7F host data writes not exercised. The
memory-latency randomization is seedable (`+seed=N`); seeds 1, 42, and 7777
all pass, with the stress-phase deviation counts varying as expected and
the non-stress phases identical across seeds.

## 7. Prior art

- **MAME `es5503.cpp` (R. Belmont, v2.4)** — the reference software
  emulator, hardware-verified over two decades:
  - Register writes and reads take effect only at stream-update
    boundaries (`m_stream->update()` then mutate): accepted emulation
    applies register writes with *batch* granularity, far coarser than one
    service slot. The pipelined variant's slot-granular application is
    strictly tighter.
  - Header note: on real silicon, in swap mode "there's one cycle when the
    switch takes place where the oscillator's output is 0x80 (centerline)
    regardless of the sample data." Our prime-miss fallback reproduces real
    5503 behavior, and our primed handoff is better than real silicon.
  - Its inner loop computes the sample address from the pre-increment
    accumulator, then increments, then reads — batch-processing whole
    buffers per oscillator. Sub-slot fetch/consume ordering is not
    something accepted emulators preserve; per-slot ordering is.
  - `halt_osc()` confirms the halt/swap/retrigger semantics this RTL
    implements (including the even-oscillator retrigger when the partner is
    in swap), and MAME resets the accumulator only on the halt 1→0 key-on
    edge, whereas this RTL resets on any halt=0 control write — a
    pre-existing baseline deviation shared identically by both variants.
- **KEGS/GSplus** similarly generate DOC audio in batches driven by the
  sound engine's update cadence, not per-oscillator-slot.
- **Ensoniq 5503 datasheet timing** (transcribed in the `doc5503.sv`
  header) shows the real chip fetches and outputs within one slot
  (E-high fetch, E-low output). The pipelined variant preserves the
  observable per-slot sequence; only the memory epoch of the data moves.

## 8. Integration sketch (a2mega) — rev 3 interface

1. **`sound_glu.sv`** (`USE_DDR3_PIPELINED` branch — this REPLACES the
   rev-1/rev-2 wiring):
   - **Burst reads**: `doc_mem_if.addr = {7'b0, wave_addr_w[15:2]}` with
     `doc_mem_if.burst = 1'b1`. `wave_address_o` is now always 16-byte
     aligned (addr[3:0] = 0, so the port word address has addr[1:0] = 0,
     satisfying the CDC's burst-alignment requirement). Each request
     returns FOUR response beats.
   - **Forward the beats verbatim**: register `doc_mem_if.q` →
     `wave_data_word_i[31:0]` with one `wave_data_ready_i` pulse per beat
     (i.e. exactly the existing q/ready registering — four pulses arrive
     per request). Byte-lane selection happens INSIDE `doc5503_pipelined`;
     no offset tracking in sound_glu. Tie the legacy `wave_data_i` to
     8'h00.
   - **Flush hookup**: `cache_flush_i = glu_mem_wr_r` (one pulse per GLU
     sound-RAM write; holding high across a burst is safe).
   - **FB hint**: thread a new 1-bit input through sound_glu to
     `fb_fetch_active_i` (see item 1b). Tie 0 if unavailable (gating
     disabled; correctness unaffected).
     ```
     .wave_available_i (doc_mem_if.available),
     .wave_data_ready_i(doc_beat_ready_r),   // <= doc_mem_if.ready, registered
     .wave_data_i      (8'h00),
     .wave_data_word_i (doc_beat_r),         // <= doc_mem_if.q, registered
     .cache_flush_i    (glu_mem_wr_r),
     .fb_fetch_active_i(fb_fetch_active_i),  // new sound_glu input
     ```
   - Note the DDR3 copy of sound RAM is written via port 5 with a queue:
     a cache_flush at write-ISSUE time (glu_mem_wr_r) precedes the DDR3
     commit by up to the queue depth (~8 µs worst). The DOC's re-fetch
     after the flush could therefore still read pre-commit data if it
     lands inside that window — bounded by the same one-service-period
     staleness class, and only during active uploads. If ever audible,
     pulse the flush at queue-drain (gq_pop) instead; both are correct.

1b. **`framebuffer_480p.sv`**: add a 1-bit output
   `fetch_active_o = (fetch_state_r == FETCH_RUN)` (the state register at
   line ~505) and wire it in `top.sv` through sound_glu to the DOC's
   `fb_fetch_active_i`. One line in each file.
2. **`top.sv`**: reclaim arbiter port 4 (`DOC_MEM_PORT`) for the DOC
   (`PORT_BASE_ADDR[4] = ENSONIQ_WORD_BASE`), and move `ddr3_debug_reader`
   to a new port 6 (`NUM_DDR3_PORTS = 7`) — its traffic is manual/rare so
   lowest priority is fine. Keep the discrete-wire hookup style (Gowin
   interface-array flattening bug). GLU write path (port 5, write queue)
   is unchanged — it already writes the DDR3 copy.
3. **Arbiter priority**: DOC stays at 4 (below shadow video and both FB
   ports). Its deadline (≥3.35 µs) tolerates any realistic starvation;
   display clients see no new contention above them.
4. **Per-board parameter**: keep `USE_BSRAM=1` available on a2mega until
   hardware A/B listening confirms, then flip and delete
   `ensoniq_bsram.sv` from the build to reclaim 32 BSRAM blocks (~27% of
   the device). Tang Nano boards keep the baseline BSRAM path; the
   pipelined module also functions with BSRAM (`wave_available_i=1`) if
   unification is ever wanted.
5. **Diagnostics**: expose `dbg_prime_miss_o` / `dbg_stale_fetch_o` /
   `dbg_fetch_drop_o` and the new `dbg_fetch_count_o[15:0]` via the debug
   overlay scratch regs. Expected steady state on hardware: drops = 0
   always; prime/stale near-zero; fetch_count rate ≤ ~0.31/slot with
   everything running (compare against §11.2 — this is the traffic
   regression check for the field).
6. After burn-in, optionally fold the variant into `doc5503.sv` behind a
   `PIPELINED_FETCH` parameter; the files are kept separate for now so the
   baseline remains byte-for-byte reviewable.

## 9. Remaining work before (re-)bring-up

1. **Re-do the `sound_glu.sv` branch to the rev-3 interface** (§8.1):
   `doc_mem_if.burst = 1` with 16-byte-aligned addresses, forward all four
   response beats on `wave_data_word_i`/`wave_data_ready_i`,
   `cache_flush_i` ← `glu_mem_wr_r`, thread `fb_fetch_active_i` (new
   `fetch_active_o` from framebuffer_480p, §8.1b), and delete any
   byte-lane offset handling. `top.sv` port-4 wiring otherwise unchanged.
2. Gowin build + timing closure check (the new logic is small: ~1.9 kbit of
   FF/LUTRAM cache/bookkeeping + an 8-deep 22-bit FIFO; no new clock
   domains).
3. Hardware verification, BOTH axes this time: (a) audio A/B vs the BSRAM
   build (synthLAB, Music Construction Set, SoundSmith POLY.SYNTH for the
   swap/one-shot edge cases MAME's changelog calls out); (b) display
   integrity with music playing (the rev-1 failure mode) — Arkanoid title
   screen is the known reproducer; FB `line_not_ready` must stay 0 and
   `dbg_fetch_drop` must stay 0. Read `dbg_fetch_count_o` twice a second
   to confirm the ~0.1-0.3/slot field rate.
4. Optional sim hardening: an RTS scenario with a shift-changing value;
   host reads of $60-7F during playback; a combined upload-while-playing
   phase (failure mode #15).

## 10. Go/no-go

**GO** (reaffirmed for rev 2, with one lesson). The DOC-semantics analysis
survived hardware contact unchanged: the two candidate audio showstoppers
(stale-terminator re-halt, swap-loop drift) are eliminated structurally,
every remaining deviation is one sample per triggering event and matches or
beats real-silicon/emulator precedent, and no register-write FIFO is
needed. What rev 1 got wrong was SYSTEM-level: it validated latency
tolerance but never modeled shared-arbiter occupancy, and its fetch-every-
slot policy broke the display in the field. Rev 2 fixes the traffic policy
(word cache + prime-once + flush invalidation, §11), and the simulation now
co-validates the display: with all 32 oscillators running against a
priority-inverted-by-CDC framebuffer client, zero line-deadline misses and
zero fetch drops. The latency budget retains ≥2.8× worst-case margin; the
throughput margin improves from 2× to ~7×. The ESP32 alternative remains
strictly worse (≥1 service period of added latency, no cycle-locking, and
it would still need DDR3 or its own RAM for the wavetables).

## 11. Hardware finding (rev 1): DDR3 arbiter saturation, and the rev-2 fix

### 11.1 Field data and root cause

Rev 1 was integrated into the a2mega 60B build and run on hardware. DOC
semantics were correct (music played), but the debug overlay showed:

| State | prime_miss | stale_fetch | fetch_drop | FB line_not_ready | Display |
|---|---|---|---|---|---|
| Idle (Arkanoid GAME OVER) | 0x06 | 0x2B | 0x32 | 0x00 | clean |
| Music (Arkanoid title) | — | — | 0x9F | 0x6B | severe corruption |

Root cause: rev 1 issued a single-word DDR3 read nearly every oscillator
slot — ~895 kHz continuously, *including* re-checking halted-oscillator
primes every scan pass (dropped primes re-queued every scan, a positive
feedback loop under congestion, which is why even the "idle" screen showed
drops: running-but-silent oscillators plus prime retries). Each single-word
read holds the serialized `ddr3_ports` arbiter non-preemptibly for
~600-1000 ns (S_READ_CMD → S_READ_WAIT spans the full controller latency
per grant): 0.6-1.0 µs × 895k/s = **50-90% arbiter occupancy from the DOC
alone**. FB read priority (port 3 > DOC port 4) does not protect the
display: the CDC round trip means the FB port cannot stay continuously
pending, so with a DOC request pending nearly always, grants alternate
FB/DOC — halving FB line-fetch throughput → missed line deadlines →
corruption. The DOC also starved itself (FIFO overflow → drops → stale
repeats → prime retries).

The rev-1 simulation could not see this because it modeled memory as a
latency distribution, not as a shared serialized server with a competing
client. Rev 2's testbench adds exactly that (see §11.3).

### 11.2 Rev-2 traffic policy and math

Three mechanisms (implemented in `doc5503_pipelined.sv`, header section
"FETCH TRAFFIC POLICY"):

1. **Per-oscillator 32-bit word cache** (32 × {word, 14-bit tag, valid,
   src_run} ≈ 1.6 kbit LUTRAM/FF). Fetches return the full word
   (`wave_data_word_i`; byte lane selected internally); a running
   oscillator issues only when the post-ACC address leaves the last-issued
   word. Per-oscillator fetch rate = sample_rate × min(1, step/4) where
   step = FC >> (9 + RES − WTS) bytes/sample.
   - All 32 running, step ≤ 1 (typical FC ≤ 0x0200 at res 0):
     ≤ 894,886/4 ≈ **224k fetches/s** — at 0.6-1.0 µs/grant that is
     **13-22% arbiter occupancy** (the coordinator's ~15% target).
   - Measured in sim with a mixed population including steps up to 1.9:
     0.309 fetch/slot ≈ **277k/s** (17-28% occupancy) — still leaves the
     FB with every grant it asks for (0 line misses in ~640 line windows
     during the all-32 phase).
   - Honest worst case: an all-32 population with steps ≥ 4 (FC ≥ 0x800
     at res 0 — several octaves above normal playback for every voice at
     once) degenerates back to 1 fetch/slot. If that ever matters, a
     token-bucket cap on `wave_rd_o` is a 20-line addition; not needed for
     real software.
2. **Prime-once for halted oscillators.** Priming is event-driven via a
   per-oscillator `prime_pending` bit — set on halt entry, host WTP/RTS
   write, cache flush, and reset; cleared when the prime fetch is
   enqueued (FIFO-full leaves it pending for a bounded retry). No
   per-scan re-checking: an idle system generates **zero** steady-state
   DDR3 traffic (13 total fetches during the sim's entire config phase).
   This also removes the rev-1 congestion feedback loop.
3. **Flush-driven invalidation** (`cache_flush_i` from `glu_mem_wr_r`).
   A flush clears every oscillator's issue bookkeeping (one refresh per
   running oscillator at its next service slot) and sets all
   prime_pending bits, WITHOUT invalidating the cached data — the stale
   word stays consumable, so a mid-music GLU write costs at most ~one
   service period of stale samples instead of a centerline click. Halted
   re-priming is deferred until `PRIME_COOLDOWN_SLOTS` (64) slots after
   the last flush so bulk 1 MHz uploads coalesce into one re-prime per
   oscillator.

One semantic consequence of word granularity, handled explicitly: a
tag-miss (stale) consume must SUPPRESS halt-on-zero, because an unvisited
lane of a stale word may legitimately be 0x00 (rev 1's stale byte was
always previously-consumed, hence never 0x00). See failure mode #14.

### 11.3 Re-validation with contention

The testbench's pipelined-side memory model is now a serialized server:
600-1000 ns non-preemptible grants; a strictly-higher-priority FB client
needing 16 word grants per 31.7 µs line window that (like the real CDC)
keeps only one request pending with a turnaround gap — reproducing the
grant-alternation mechanism; DOC limited to 2 outstanding requests, word
data sampled at grant completion. Under this model the rev-1 policy's
arithmetic fails (16 × (grant + stolen DOC grant) ≈ 30-34 µs/line vs the
31.7 µs window — marginal-to-failing, matching the field), while rev 2
measured: **2536 FB line windows, 0 misses** outside the forced-overload
stress phase (1 miss during it, recovering to 0); **fetch_drop = 0 in all
phases including stress**; prime_miss = 1 (the designed
control-rewrite-on-running-osc case); stale fetches only under forced
6.1 µs grant outliers. Correctness results are unchanged from rev 1
(C-record streams bit-identical everywhere, including word-boundary-
crossing swap handoffs and mid-word 0x00 terminators; all sample
deviations classified). Fetch issue decisions are deterministic — the
total fetch count is identical across random seeds. Seeds 1, 42, 7777 all
PASS.

NOTE (rev 3): §11.3's re-validation used a contention model that later
proved optimistic against the real port contract (it modeled the FB as
unable to keep its port pending and used generic 600-1000 ns grants).
Rev 2 improved hardware behavior but did not fix it. §12 supersedes this
section's model and re-validation.

## 12. Hardware finding (rev 2 → rev 3): the real port contract, service
## tails, and the framebuffer capacity margin

### 12.1 Rev-2 field data

Arkanoid title, music playing, rev-2 build (timing-clean): stale_fetch =
0xDF and climbing (audible corruption — the stale-repeat fallback engaging
continuously), fetch_drop = 0x2D (the "must stay 0" invariant broken), FB
line_not_ready = 0x24 with visible display distortion (improved from rev
1's 0x6B, still failing). Idle traffic (prime-once) appeared fixed.

### 12.2 Exact round-trip accounting (from the RTL, not a model)

`hdl/ddr3/ddr3_port_cdc.sv`:

- The request FIFO is 2-entry (`REQ_PTR_WIDTH = 2`, `req_fifo_packed[0:1]`,
  lines 116-117/165); `client_available = init && !full` with full at
  occupancy 2 (lines 196-197). So the port is **2-outstanding**, not 1 —
  BUT requests are serviced strictly single-file: the DDR-side capture
  registers hold ONE request (`pending_r`), and the next queued request is
  captured only after the arbiter's `req_done` (lines 317-327), through a
  BSRAM-read/unpack/pending pipeline of ~3 ddr cycles plus arbiter
  re-scan.
- Request path, client fire → arbiter-visible pending: wr-pointer gray 2FF
  sync (lines 241-249, ~2-3 ddr) + BSRAM issue/unpack/capture/pending
  (comment lines 262-267: T..T+4) ≈ **7-8 ddr cycles** (~90-100 ns).
- Response path, `resp_valid` → `client_ready`: resp FIFO push, wr-gray
  2FF sync into clk_client (lines 414-423), then a 2-stage BSRAM-read
  drain (lines 436-456) ≈ **4-5 client cycles** (~80-90 ns).
  `client_ready` is a **single-cycle pulse per beat** (lines 443-459).
- `available` reassertion after `req_done`: rd-pointer gray 2FF back-sync
  (lines 175-183) ≈ 2-3 client cycles.

`hdl/ddr3/ddr3_ports.sv` (arbiter, all states non-preemptible per grant):

- Non-burst/4-beat read grant: S_IDLE pick (1) + S_LOAD (1) + S_READ_CMD
  (1) + S_READ_WAIT (controller latency K, calibrated ~20 ddr from the
  known ~550 ns uncontended round trip) + S_RESPOND (1 or 4 beats) +
  S_DONE (1): **~25-30 ddr ≈ 310-370 ns**; a 4-beat burst adds only ~3
  ddr — this is why rev 3 fetches 16 bytes per grant.
- FB burst8 grant (READ_BURST8_PORT): two CMD+WAIT rounds + 8 respond
  beats: **~52 ddr ≈ 640 ns**, and `framebuffer_480p.sv` issues them
  back-to-back (2-deep CDC pipelining, comment lines 493-496), so a
  640-wide line (320 words at 2 px/word, `packed_width_r` line 487) =
  **40 burst8 grants ≈ 1440+ clk_logic of the 1712-clk line window**.
- Static priority scan picks the lowest pending index (S_IDLE, lines
  363-374): shadow read (0), shadow write (1), FB write (2), FB read (3)
  all outrank the DOC (4).

Worst-case single-fetch tail behind one FB burst8 grant + a tRFC refresh
collision: ~8 (req path) + 52 (FB grant drain) + 30 (own grant) + 21
(tRFC 260 ns) ddr + response path ≈ **~1.6 µs**; behind FB catch-up plus
shadow/write traffic the tail is unbounded-ish: the DOC is the lowest-
priority client of an arbiter running at ~90%+ utilization during active
display, so its service tail scales like 1/(1-ρ) — tens of µs, past the
rev-2 one-service-period deadline. And every DOC grant taken during a
line fetch directly lengthens the FB's already-marginal line time.

### 12.3 The two real constraints (and why rev-2's fix was insufficient)

1. **DOC correctness is a LATENCY question**: the deadline must exceed
   the lowest-priority service tail (tens of µs). Rev 2's word cache cut
   traffic but kept a ~1-service-period deadline → stale storm + drops.
2. **FB integrity is a CAPACITY question**: with FB's 2-line prefetch
   cushion, what matters is total offered load vs serialized arbiter
   capacity. Word-granularity DOC traffic (~0.31 grants/slot × ~26 ddr)
   costs ~9% of the arbiter — right at the FB's thin margin. Scheduling
   (gating) only moves grants around; it cannot create capacity.
   **Quantified answer to "does (a)+(c) alone fix FB": no** — in the
   corrected model, word-granularity all-32 traffic missed ~580 of ~590
   line deadlines with lookahead and gating both active. The per-grant
   COST had to drop.

### 12.4 Rev-3 architecture

1. **16-byte cache lines via 4-beat burst reads** — a grant costs ~the
   same whether it returns 4 or 16 bytes, so occupancy drops ~4× again:
   all-32 measured 0.078 fetches/slot ≈ **70k grants/s ≈ 2.6%** of
   arbiter time (vs ~9% rev 2, 50-90% rev 1). Idle: 20 fetches total for
   a whole config phase (prime-once retained).
2. **Two-line store + lookahead issue** — slots direct-mapped by
   wave_addr[4] (consecutive lines alternate, prefetch never evicts the
   consumed line); the next line is issued when the walk address two
   samples ahead first lands in it. Deadline for step ≤ 1 walks:
   **~16 service periods** (~0.6 ms at 32 oscillators) — far beyond any
   arbiter tail. Jumps (wrap/retarget/SYNC restart) keep the 1-period
   crossing path: rare single-sample stales remain possible under
   extreme contention (observed once at a table wrap during an E1=4
   window where the gate escape exceeded the 5-slot deadline) — counted,
   benign.
3. **FB-aware issue gating** (`fb_fetch_active_i` from framebuffer_480p
   FETCH_RUN) with a drain-until-empty age escape
   (`FB_GATE_ESCAPE_SLOTS` = 8): DOC grants land in per-line residual
   and vblank; a naive single-issue escape was measured to cap gated
   throughput below demand and jam the queue — the hysteresis matters.
4. Fetch FIFO deepened to 16 so clustered crossings from many
   oscillators drain over a gated line harmlessly.
5. One new consume rule forced by line granularity: tag-miss (stale)
   consumes SUPPRESS halt-on-zero — an unvisited lane of a stale line
   may legitimately be 0x00 (failure mode #14 generalized).

### 12.5 Re-validation with the corrected model

The testbench's memory model now implements §12.2 directly: single-file
2-outstanding DOC port with the derived request/response/available
timings; FB engine with 40 back-to-back 35-clk burst8 grants per line,
2-line prefetch, continuous catch-up, 240-active/22-vblank line frames;
a ~6% background client above DOC priority; 3% tRFC grant extensions.
Aggregate high-priority utilization ~90% of active-display time.

- **Rev-2 reproduction** (`-DREV2_MODE +nogate`: lookahead off, gating
  off, synthetic word-granularity DOC-class load at rev-2's measured
  0.31 grants/slot): **2019 of 2337 FB line deadlines missed** — chronic
  display corruption, qualitatively matching the field failure. The
  DOC-side stale/drop storm follows from the same numbers for rev-2's
  1-period deadline (§12.3.1); the intermediate word-cache runs under
  this model showed the storm directly (drops > 130, stales > 180).
- **Rev 3** (seeds 1/42/7777, deterministic fetch counts): correctness
  PASS to the same standard as before — 36,336 paired samples, C-record
  streams (halt/swap/retrigger) bit-identical in every phase, all
  deviations in the documented classes (2 ADDR_MAP_DELAY, 1 PRIME_MISS,
  13 SYNC_RESTART incl. stress-phase wrap overlaps, 9 SYNC_AM_VOL,
  stress-only degradation) — now including line-crossing loop wraps,
  swap handoffs, and mid-line 0x00 terminators. **FB: 2337 line windows,
  0 misses in every phase including all-32 and forced overload.
  Counters: fetch_drop = 0 everywhere; prime_miss = 1 (designed);
  stales pre-stress ≤ 13, all classified.**

### 12.6 Residual risks / notes

- The controller read latency K (~20 ddr) is calibrated, not measured;
  the rev-3 margins (deadline ×16, occupancy 2.6%) do not depend on its
  exact value.
- Sustained GLU upload bursts during playback still cost one refresh per
  running oscillator per service period (failure mode #15) — now 16-byte
  refreshes, so ~4× cheaper than rev 2.
- If `fb_fetch_active_i` is tied off, correctness and traffic are
  unchanged; only the FB-crispness benefit of gating is lost — at 2.6%
  occupancy the impact is small, but wire it: it is one line per file.

### 12.7 Storage restructuring (rev 3.1 — Gowin placement fix)

Rev 3's line cache as a flat FF array (`reg [127:0] cache_line_r [64]` +
64x12 tags, read combinationally from the main FSM block) synthesized to
~9.7k registers and failed GW5AT-60B placement (`PR0003: 847 REG(s)
unPlaced`). Restructured:

- The {tag[11:0], line[127:0]} payload (64 x 140 = 8,960 bits) now lives
  in `doc_cache_ram`, a simple-dual-port synchronous-read memory using
  the repo's proven inference idiom (separate write/read always blocks,
  full-word writes, no reset, `syn_ramstyle="distributed_ram"`): ~140
  RAM16 CFUs as distributed SSRAM, or a single BSRAM if the tools prefer
  (8,960 < 18,432 bits — either way a2mega still nets ~27-31 freed BSRAM
  blocks vs `ensoniq_bsram`).
- Packing the TAG with the line keeps them atomically coherent across the
  1-cycle read latency: a retire landing on the read-sample edge can at
  worst pair one-cycle-newer valid/src FLAG bits with old {tag,line},
  which resolves as a benign counted stale/prime-miss — never a wrong
  tag-accept.
- No FSM change was needed: the read address depends only on
  curr_acc/wtp/rts, which settle at the end of OSC_LOAD_REGISTERS — two
  clk_i before OSC_CONSUME samples the read data.
- Remaining ADDED flip-flop cost vs baseline doc5503 ~= **1.54k FF**:
  per-slot issue bookkeeping 768 (last_issued_word, combinational on the
  issue path — kept in FFs deliberately) + valid/src/issued/prime flags
  224 + fetch FIFO 364 + beat assembly 98 + misc/debug ~90. Under the
  2k target.
- Differential suite re-run in full after the timing change: identical
  results (same deterministic fetch counts, same counters, PASS on seeds
  1/42/7777, rev-2 repro still fails as required).

### 12.8 Register diet (rev 3.2 — second GW5AT-60 placement fix)

Rev 3.1 still failed placement (847-1009 REG unplaced across attempts) at
88% CLS: doc_cache_ram extracted to RAM correctly, but ~1.5k added FFs of
bookkeeping could not be legalized at that slice pressure. Rev 3.2 sheds
~950 more FFs with no architecture or interface change:

- `last_issued_word_r` (64x12 = 768 FF) → two `doc_lu_ram` instances
  (32x12 each, one per cache-slot parity), synchronous read at address
  `curr_osc_r` — stable for the entire service slot, so the read needs no
  scheduling (data valid by the slot's 2nd clk_i; first use at OSC_ACC,
  ~7 clk_i in). Writes go through a registered strobe (+1 clk_i);
  verified safe because at most ONE issue/prime bookkeeping write fires
  per service slot and the next read of that oscillator's entry is a
  full scan later. `issued_valid_r` stays in FFs (a flush must clear all
  64 bits in one cycle).
- Fetch FIFO: 16 → 8 deep, and entries store 12-bit LINE addresses
  (the byte-lane bits were never consumed): 364 → 153 FF.
- prime/stale/drop debug counters: 8 → 4 bits, zero-padded to keep the
  8-bit ports (interface unchanged); `dbg_fetch_count_o` stays 16-bit
  (field traffic regression tool).

Added-FF audit (vs baseline doc5503), rev 3.2:

| Item | FF |
|---|---|
| RAM output registers (cache 140 + 2x lu 12) | 164 |
| Flag bits (valid/src/issued/prime) | 224 |
| Fetch FIFO (8 x 18 + pointers) | 153 |
| Burst-beat assembly | 98 |
| lu write staging (we/addr/data) | 19 |
| Working regs (issue/consume addr, cooldown, gate) | 44 |
| Debug counters | 28 |
| **Total added** | **~730 FF** (~9.7 kbit in inferred RAM) |

Full suite re-run: PASS on seeds 1/42/7777 to the same standards, FB
2337/2337 deadlines, drops 0, rev-2 repro gate intact. One deterministic
delta: 2 fewer total fetches (3152 vs 3154), isolated to the E1-change
phase — redundant re-issues deduped by the one-cycle bookkeeping
visibility; the comparator confirms zero consumed-sample impact.

### 12.9 RAM inference ground truth (rev 3.3): GW5A has NO SSRAM

Netlist analysis of the rev-3.1/3.2 placement failures produced two
findings, both resolved here. Verification methodology lesson (now the
standard): on Gowin, "Extracting RAM for identifier" is RECOGNITION, not
MAPPING — the only trustworthy checks are a DFF census of the synthesized
.vg netlist (count `^\s+DFF\w*` lines) and the SSRAM/BSRAM rows of the
resource summary.

1. `syn_ramstyle="distributed_ram"` on the 64/32-deep sync-read arrays
   silently fell back to flip-flops (doc_cache_ram alone = 9,036 DFFs in
   the failing netlists). Both arrays now carry
   `syn_ramstyle="block_ram"` (mirrored from the main tree).

2. The oscillator register banks were flip-flops too — and controlled
   synthesis-only experiments prove this is NOT caused by anything in
   doc5503_pipelined (the suspected extra read port does not exist; the
   pipelined module reads the bank outputs in exactly the same 34 places
   as the baseline):

   | Experiment | Device | RAM16 | BSRAM | DFF |
   |---|---|---|---|---|
   | baseline doc5503 | GW2AR-18 (control) | 40 | 0 | 1,125 |
   | baseline doc5503, UNMODIFIED | GW5AT-60B | **0** | 0 | **3,652** |
   | doc5503_pipelined (pre-fix) | GW5AT-60B | 0 | 6 | 4,218 |
   | direct RAM16SDP4 instantiation | GW5AT-60B | — | — | **ERROR RP0007: "There is no SSRAM resource in current device"** |

   The GW5A (Arora V) family has NO distributed-RAM resource at all: the
   a2n20v2 SSRAM contract is physically unsatisfiable on this device for
   ANY module, including the baseline. The banks can only be RAM as
   BSRAM (synchronous read).

Rev 3.3 restructure (doc5503_pipelined only; the baseline keeps its
proven GW2A idiom for the Tang Nano boards):

- New `osc_reg_ram_dp`: 32 x DATA_WIDTH dual-port BSRAM. Port A serves
  the FSM through the existing shared per-bank address register (write
  timing unchanged; sync-read is transparent because the RAM samples
  pre-edge register values). The control-read chain's three address
  launches each move ONE STATE EARLIER (partner -> OSC_START, next ->
  LOAD_REGISTERS, prev -> LOAD_PARTNER_CONTROL) so every datum lands in
  the state that already consumed it; the curr_osc default covers
  OSC_LOAD_REGISTERS' main-bank reads. Verified no consumer reads a bank
  the cycle immediately after writing the same address.
- Port B serves HOST reads with a dedicated persistent address register
  (host_raddr_r): data becomes valid two clk_i after the request and
  stays valid until the next host read, so the one-stage-delayed
  device_response can consume it at ANY later invocation — no staleness
  window, and zero interference with port-A addresses (the rev-3.2-era
  analysis showed a shared-port host-read scheme has an unavoidable
  address-clobber race with the load chain at slot boundaries).
- The differential testbench gained a host-readback phase (38 register
  reads compared between baseline and pipelined — the read path was
  previously unexercised) and the jump-stale classifier now covers wrap
  samples anywhere in the table's first line (failure mode #16's stale
  can land on offset 1+, not only offset 0).

Census after rev 3.3 (GW5AT-60B, synthesis-only):

- doc5503_pipelined: **1,621 DFF** (vs 4,218 pre-fix, and vs 3,652 for
  the BASELINE on this device — the pipelined module is now ~2,030 DFF
  smaller than the module it replaces) and **14 BSRAM** (8 bank DPBs +
  4 doc_cache_ram + 2 doc_lu_ram — true dual-port inference confirmed,
  no duplication). Combined with the a2mega wavetable moving to DDR3,
  total sound-subsystem BSRAM is 14 vs the 32 of ensoniq_bsram.
- Sim: full suite + rev-2 repro gate re-run, PASS on seeds 1/42/7777,
  host readback 38/38, FB 2339/2339 line deadlines, drops 0.

### 12.10 BSRAM write-mode legality (rev 3.4) + a boundary bug the audit caught

Rev 3.3 failed at PLACEMENT with the known project-wide Gowin gotcha
(`PA2122: Not support (SP) WRITE_MODE = 2'b10`): `osc_reg_ram_dp`'s port-A
read and write share one address net, so inference merged them into a
single port with Verilog read-old semantics = WRITE_MODE 2'b10
(read-before-write), which GW5A BSRAM rejects. Since the read-during-write
value is proven don't-care for every consumer, the port-A read register is
now gated `if (!a_we_i)` — output holds during writes, which is exactly
WRITE_MODE 2'b00. `doc_cache_ram` / `doc_lu_ram` (separate address nets,
the ensoniq_bsram SDPB idiom) were already legal.

Freeze-edge audit for the gated read (the output now also holds during
same-bank writes to OTHER addresses): harmless whenever the read address
has been stable ≥2 cycles (frozen value = previous sample of the same
address); the only dangerous edges are the service chain's
address-changing edges, and no bank write-enable can be high during those
states — EXCEPT one path the audit exposed as a REAL rev-3.3 bug
independent of write modes: a host WRITE processed on the last idle cycle
before a slot boundary leaves that bank's port-A address on the host
target during OSC_START, so the synchronous read sampled at the
START→LOAD_REGISTERS edge loads the WRONG oscillator's registers for one
slot (wrong FC/volume/control — potentially a spurious mode change). The
differential TB missed it because its host writes are slot-start
synchronized. Fixed: `osc_idle` never processes host requests on the
cycle_start cycle (the pending flag defers to the next idle window;
same-slot application semantics preserved since the service chain always
runs before the idle tail). This restores ≥1 cycle of address-stability
margin at every chain sampling edge.

VERIFICATION METHODOLOGY (upgraded, now the standard): synthesis PASS +
netlist DFF census is necessary but NOT sufficient — BSRAM write-mode
legality only surfaces at PnR. Every new RAM shape must survive a
PnR-THROUGH probe: a tiny top instantiating the RAM (serialized I/O keeps
pin count trivial), `run all` on the target part. The probe for all four
doc5503_pipelined shapes (bank 8-bit, bank 24-bit, cache 64x140, lu
32x12) passes placement + routing with zero PA2122: 12 BSRAM, only the
probe's own shift register in fabric.

Re-census after rev 3.4: unchanged — 1,621 DFF, 14 BSRAM. Full
differential suite + rev-2 repro gate re-run: PASS on seeds 1/42/7777,
host readback 38/38, FB 2339/2339, drops 0.

### 12.11 Targeted invalidation (rev 3.5): cache-flush storms during playback

Priority-swap hardware results (DOC port above FB, gating off, rev-3.2
module): DOC memory health fully restored (stale=0, drop=0, much-improved
audio) but FB line_not_ready WORSENED to 0x36 with corruption correlated
to multi-oscillator passages. Root cause: bulk invalidation. The
"sound-RAM writes only happen during loading" assumption is FALSE —
Arkanoid and many IIgs titles stream sound RAM DURING playback at up to
~1 MHz. Every write cleared all 64 issued_valid bits, so every running
oscillator refetched each service period — rev-1-class traffic, now
ABOVE the FB in priority. The priority swap had merely moved the
collateral damage from the DOC to the FB; bulk invalidation was the
shared root cause.

Rev-3.5 scheme (option b+c hybrid — smallest of the candidates):

- The GLU write's line address arrives on a NEW input,
  `cache_flush_addr_i[15:0]` (sound_glu wires
  `.cache_flush_addr_i({glu_mem_addr_r[13:0], 2'b00})` next to the
  existing `.cache_flush_i(glu_mem_wr_r)` — the only integration delta).
- Writes accumulate a [lo, hi] LINE range in generation A; once per scan
  A rotates into B (invariant: a write during scan n is in A for later
  services of scan n and in B throughout scan n+1, so every running
  oscillator checks it within one service period).
- At each running service slot, the oscillator's two cached/issued line
  addresses are ALREADY at hand (lu_rdata0/1_w, read every slot at
  curr_osc_r) — no CAM, no extra RAM port. On a range hit, only that
  slot's issued_valid clears; the refetch issues at the same slot's
  OSC_ACC. Halted oscillators keep the prime_pending + cooldown path
  (one re-prime per oscillator after writes quiesce).
- VISIBILITY SEMANTICS (documented class updated): dirty detection <= 1
  service period after the write; fresh data consumed <= 2 periods (was
  <= 1 with bulk flush — the line cache adds one period of lazy
  detection). The comparator's GLU-race window is 2 periods + margin.
- Cost: ~52 FF (two {lo, hi, valid} range generations) + eight 12-bit
  comparators. Census: 1,671 DFF / 14 BSRAM (rev 3.4: 1,621).
- KNOWN CONSERVATISM: the range is a superset — SCATTERED pokes to
  distant lines (e.g. alternating writes to two far-apart voices'
  tables) balloon the range and can regenerate refetch traffic up to the
  bulk-invalidation worst case (never worse than rev 3.4's shipped
  behavior, and it self-clears within 2 scans of the last write).
  Sequential streams — the realistic pattern, auto-increment pointer —
  keep the range a few lines wide. If hardware ever shows the scatter
  regime, the named follow-up is an exact 8-deep write-line FIFO checked
  alongside the ranges, overflowing into the range (adds ~100 FF).

Validation (corrected contention model, main-tree config: DOC priority
above FB, gating disabled):

- NEW TB phase 9: 900 sequential byte writes at ~1 write/us in bursts,
  arbitrary bus phase, walking through tables that playing oscillators
  are consuming, sustained ~35 scan rounds during all-32 playback.
  Results (seeds 1/42/7777, deterministic): storm-phase traffic
  0.086 fetch/slot vs 0.078 quiescent (+10%; bulk invalidation measured
  ~1/slot here — asserted bound 0.16); FB 2383/2383 line deadlines met
  in EVERY phase including the storm; drops 0; C-record streams
  bit-identical (900 mid-playback writes perturb no halt/swap event);
  4 GLU_RACE samples — the first live coverage of that class, all
  within the <= 2-period window; host readback 38/38.
- GATING VERDICT: stays OFF. With the DOC above the FB, gating no longer
  protects anything (the DOC wins arbitration anyway) and its convoy
  formation was harmful; with targeted invalidation the write-storm
  traffic collapses, so the FB holds its deadlines from below. The
  fb_fetch_active_i port remains (tie 0), harmless and available if a
  future board wants the old priority order.

### 12.12 Timing closure (rev 3.6): BSRAM-Q launch paths

Rev 3.5 failed 54 MHz closure (worst -0.277 ns, dominant family
`fl_ram/mem -> fq_src_r_*`). Netlist analysis showed the mechanism was
subtler than a missing pipeline stage: GowinSynthesis RETIMED the
LOAD-chain capture registers INTO the bank BSRAMs' output registers (the
fl_ram instance's output port literally became curr_fl_r), collapsing the
RTL's intended two-stage read path to one — so the ACC add, wave_addr_f,
lu compares and fetch-queue enable cones all launched from a slow BSRAM
clock-to-out (~2.5 ns) instead of a fabric FF.

Fix (no interface or scheduling change):

- `(* syn_preserve = 1 *)` on all eleven LOAD-chain capture registers
  (curr_fl/fh/vol/wds/wtp/control/rts/acc, partner/next/prev_control):
  these ARE the pipeline stage; the attribute stops the absorption.
  Verified in the re-synthesized netlist: the banks' outputs are module
  wires again and the capture registers are fabric DFFs.
- Fabric re-registration of the remaining BSRAM read outputs before any
  deep logic: cram_rdata_r (140 b) and lu_rdata0/1_r (12 b each),
  sampled every cycle. Functionally free by the margins already proven
  in §12.7/§12.8 scheduling analyses (cache data correct one state
  before OSC_CONSUME; lu data correct from the slot's 2nd cycle, first
  used ~7 cycles in). Hazard re-check: a retire landing in the extra
  cycle means the coherent one-cycle-older {tag, line} pair is consumed
  — the same benign counted classes; the lu write path already had no
  same-slot read-after-write (§12.8), and +1 cycle of read lag keeps
  that invariant (next read of an oscillator's bookkeeping is a full
  scan away).
- Result by construction: every BSRAM Q in the module feeds exactly one
  fabric FF (bank port A -> curr_*_r; cram/lu -> pipeline regs; bank
  port B -> the shallow host data_o mux, flagged as the only residual
  BSRAM-Q cone — trivial depth). All deep cones launch from fabric FFs
  with the full 18.5 ns.
- Census: 1,744 DFF (+73 for the pipeline FFs) / 14 BSRAM.
- On the secondary family (ddr3_port_cdc req FIFO, -0.258): concur it is
  congestion collateral — that path is pre-existing and outside this
  module; flag it back if it survives this fix.

Full suite + repro gate re-run: PASS on seeds 1/42/7777, results
bit-identical to rev 3.5 (same deterministic fetch counts and counters —
the re-registration consumes the same stable values one RAM-cycle
earlier). Host readback 38/38; FB 2383/2383; drops 0; lint clean.
