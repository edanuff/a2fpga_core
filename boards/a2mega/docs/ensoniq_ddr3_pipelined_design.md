# Ensoniq DOC5503 with DDR3-backed wavetable RAM: pipelined-fetch design

Status: **rev 2 — DOC semantics hardware-proven; fetch-traffic policy
revised after the rev-1 hardware run saturated the DDR3 arbiter and starved
the framebuffer (see §11); rev 2 re-validated in differential simulation
including an arbiter-contention + FB-deadline model — GO for
re-integration**
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
fetch drops in all phases. Recommendation: **GO** — re-integrate on
a2mega with the rev-2 interface (§8), reclaim the 32 BSRAM blocks, keep
the BSRAM path available behind a per-board parameter until hardware A/B
confirms both audio and display.

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
| 13 | **Arbiter saturation / FB starvation** (rev-1 field failure) | **real system-level bug — fixed in rev 2** | Per-slot single-word fetches (~895 kHz) occupied 50-90% of the serialized arbiter; FB priority did not protect it because the CDC round trip lets an always-pending DOC steal alternate grants. Fixed by word cache + prime-once + flush invalidation (§11); re-validated against an explicit contention + FB-deadline model: 0 FB misses outside forced overload, 0 drops anywhere. |
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

## 8. Integration sketch (a2mega) — rev 2 interface

1. **`sound_glu.sv`** (`USE_DDR3_PIPELINED` branch — this REPLACES the
   rev-1 wiring):
   - Address mapping unchanged: `doc_mem_if.addr = {7'b0,
     wave_addr_w[15:2]}` (the DOC emits a byte address; the port fetches
     the containing 32-bit word), `doc_mem_if.rd = doc_mem_rd_w`,
     `wave_available_i = doc_mem_if.available`.
   - **Return the full word**: register `doc_mem_if.q` →
     `wave_data_word_i[31:0]` with a 1-clk `wave_data_ready_i` pulse on
     `doc_mem_if.ready`. Byte-lane selection now happens INSIDE
     `doc5503_pipelined` (it knows `addr[1:0]` per request), so the rev-1
     **4-deep byte-lane offset FIFO is DELETED** — no offset tracking in
     sound_glu at all. Tie the legacy `wave_data_i` byte input to 8'h00.
   - **Flush hookup**: `cache_flush_i = glu_mem_wr_r` (the registered
     sound-RAM write strobe; one pulse per GLU write, before/while the
     write enters the port-5 write queue). Holding it high across a write
     burst is safe. Exact port diff for the branch:
     ```
     .wave_available_i (doc_mem_if.available),
     .wave_data_ready_i(doc_word_ready_r),   // <= doc_mem_if.ready, registered
     .wave_data_i      (8'h00),
     .wave_data_word_i (doc_word_r),         // <= doc_mem_if.q, registered
     .cache_flush_i    (glu_mem_wr_r),
     ```
   - Note the DDR3 copy of sound RAM is written via port 5 with a queue:
     a cache_flush at write-ISSUE time (glu_mem_wr_r) precedes the DDR3
     commit by up to the queue depth (~8 µs worst). The DOC's re-fetch
     after the flush could therefore still read pre-commit data if it
     lands inside that window — bounded by the same one-service-period
     staleness class, and only during active uploads. If ever audible,
     pulse the flush at queue-drain (gq_pop) instead; both are correct.
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

1. **Re-do the `sound_glu.sv` branch to the rev-2 interface** (§8.1): word
   return (`wave_data_word_i` ← `doc_mem_if.q`), `cache_flush_i` ←
   `glu_mem_wr_r`, and DELETE the rev-1 byte-lane offset FIFO (lane
   selection moved into the DOC). `top.sv` port-4 wiring is unchanged from
   rev 1.
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
