# Ensoniq-on-DDR3 Postmortem — a2mega

*Campaign: 2026-08-02 → 2026-08-04. Outcome: the DOC5503 runs its 64KB
wavetable RAM from DDR3 on the a2mega (GW5AT-60), hardware-verified with
clean audio, clean video, and instrumentation matching the design math.
Companion documents: `ensoniq_ddr3_pipelined_design.md` (the design and its
revision history), `jtag_flash_reliability.md` / `jtag_bridge_research.md`
(the flashing saga that ran interleaved with this one).*

---

## 1. Executive summary

Ensoniq-on-DDR3 had been analyzed and **parked as infeasible**: the DOC5503
requires its sample fetch within ~978 ns of computing the address, and the
shared DDR3 stack's worst-case round trip is ~1.0–1.2 µs. That analysis was
correct — and irrelevant. The feature shipped because of one architectural
insight (delay consumption by one service cycle, converting a ~1 µs
deadline into a ≥3.3 µs one) and then survived **eight distinct defects**
discovered across five hardware iterations, none of which were the parked
analysis's concern, and the last of which — the actual cause of weeks of
"distortion with clean counters" — was on the **write path**, in code that
predates the campaign and had simply never executed before.

The single sentence version: **the read side was over-analyzed and turned
out fine; the write side was never analyzed and turned out broken; and the
diagnostic instruments were all pointed at the read side.**

Final verified numbers (Arkanoid title, 2026-08-04):

| Metric | Predicted | Measured |
|---|---|---|
| DOC DDR3 requests/frame | ~1,200 | ~1,300 (Hex 2 = 05) |
| Min inter-request gap | ~1.1 µs (1/slot) | 1.06 µs (Hex 4 = 0x39) |
| Write drops / fetch drops / late FB lines | 0 / 0 / 0 | 0 / 0 / 0 |
| Arbiter occupancy (all-32 case, sim) | — | ~2.6% |
| BSRAM freed vs `ensoniq_bsram` | 32 | 28 net (cache uses some) |

## 2. Why it was parked, and why that was the wrong reason

The original feasibility analysis modeled a single dependent random read
per oscillator slot: address born mid-slot (`ACC += FC`), data due before
`clk_count == 7` — ~978 ns — against a measured-in-RTL worst-case DDR3
tail of ~1.0–1.2 µs (in-flight burst grant + tRFC + own round trip). No
margin, audio fails loudly on every miss: parked. The alternatives priced
at the time (per-oscillator wavetable cache with stochastic miss
artifacts; a dedicated QSPI PSRAM chip; ESP32 audio offload) all carried
real costs.

The unlock was the observation that **the 5503 is already half-pipelined**
("output old WDS → fetch new WDS → ACC += FC") and that extending it by
one stage — issue the fetch at oscillator *k*'s service slot, consume at
*k*'s **next** service — multiplies the deadline by the scan length:
minimum ~3.3 µs, typically ~38 µs. Better still, because halted
oscillators still occupy scan slots and un-halt resets the accumulator,
prefetch-while-halted makes the pipelined DOC **time-aligned** with the
baseline, not merely one period behind. Differential simulation confirmed
bit-identical control streams. The read-side architecture that ultimately
shipped (rev 3.8) is this idea plus three hardware-taught refinements:
16-byte line fetches, lookahead issue, and targeted invalidation.

**Lesson zero: the parked analysis answered "can a naive client meet the
deadline" (no) and was silently treated as answering "can any client"
(also no — wrongly).** The park was rational; the failure was never
re-opening it when an architectural degree of freedom (the consume delay)
was pointed out to be available all along.

## 3. Iteration timeline

| Rev | Change | Hardware result | What it taught |
|---|---|---|---|
| 1 | Pipelined fetch, per-slot issue incl. halted refresh | Display corruption during music; DOC self-starved (drops 0x32 even idle) | Fetch *traffic*, not latency, was the binding constraint: ~895 kHz of serialized single-word round trips ≈ 50–90% arbiter occupancy. The sim had modeled a private memory, not a shared arbiter. |
| 2 | Word cache (4× cut), prime-once, flush-on-write | Better; stale=DF storms, drops, FB lines still missed | The sim's port model (≤2 outstanding, 600–1000 ns) was optimistic vs the real CDC (single-file service, ~1.6 µs tails behind FB line trains at ~90% utilization). Models must be derived from RTL, not assumed. |
| 3 | 16-byte line bursts, two-line/osc cache, lookahead (deadline ≈16 periods), FB gating | (via 3.x fixes) corruption moved to frame bottom, 9 late lines | Corrected model *reproduced* rev-2's field failure (2019/2337 lines missed) — the first time sim and silicon agreed. Gating created convoys: deferred fetches discharged exactly at line-fetch start. |
| 3.1–3.2 | Storage/FF diet | Placement kept failing | See defects #4–5: the RAM that wasn't. |
| 3.3–3.4 | Register banks → dual-port BSRAM; write-mode idiom; boundary-write bug fix | — | GW5A has **no SSRAM at all** (RP0008/RP0007, verified empirically); the baseline DOC's distributed-RAM contract was silently costing ~2.8k FF on this die since day one. |
| 3.5 | Targeted dirty-range invalidation (~52 FF) replacing bulk flush | — | Bulk flush on every GLU write ≈ rev-1 traffic during play-while-write titles. |
| 3.6 | `syn_preserve` + fabric re-registration of all BSRAM outputs | Priority-swap build: **DOC counters finally all clean**; FB still 0x36 late lines; audio still distorted | Synthesis had *retimed the existing pipeline registers into the BSRAM output registers*, silently converting two-stage paths into launch-from-BSRAM-Q. |
| — | DOC priority above FB, gating deleted | Video clean at last (Hex 7=00), audio still distorted, **all counters clean** | Convoy diagnosis confirmed. And the stage was set for the real lesson: every remaining instrument said the memory system was healthy. |
| 3.7–3.8 | src_run clear paths deleted (swap-on-arrival); issue-cone timing staged | Not flashed in distorted era (timing) — later folded in | The 0x80-pop mechanism was real but was NOT the field bug (hardware prime_miss read 00 during distortion). |
| **GLU fix** | Write-ack contract + priority + queue depth + drops counter wired | **Clean. Everything.** | See §5. |

## 4. The defect catalog

Eight distinct defects (excluding the flashing saga), by where they hid:

1. **Fetch-policy saturation** (rev 1, design): per-slot fetches including
   halted-refresh at ~895 kHz through a serialized arbiter. *Hid in:* a sim
   memory model with no contention.
2. **Convoy formation from FB gating** (rev 3, design): polite deferral +
   drain-until-empty discharged queued fetches exactly when the next line
   fetch began; cumulative slip visible as bottom-of-frame corruption.
   *Hid in:* a gating heuristic designed against the wrong port model.
3. **Bulk cache flush on every sound-RAM write** (rev 2–3.4, design):
   play-while-write titles (Arkanoid does this) regenerated near-rev-1
   traffic. *Hid in:* the assumption "writes only happen at load time."
4. **`syn_ramstyle="distributed_ram"` silently produces flip-flops on
   GW5A** (rev 3.0–3.2, toolchain trap): "Extracting RAM" in the log means
   *recognition, not mapping*; 9,036 FFs shipped as "RAM." Only a netlist
   DFF census revealed it. Root fact: **the GW5A family has no distributed
   RAM**, verified by direct RAM16SDP4 instantiation → `RP0007`.
5. **The baseline DOC's SSRAM contract was already broken on a2mega**
   (pre-existing): the pains-taken register-bank inference was physically
   unsatisfiable on Arora V; ~2.8k FF silently, in every prior a2mega
   build. The campaign's DPB rework made the DOC *smaller* than baseline.
6. **BSRAM WRITE_MODE=2'b10 rejection at PnR** (rev 3.3): a
   placement-phase-only error invisible to synthesis + census checks;
   fixed with the repo's separate-block idiom; methodology now mandates a
   PnR-through probe per new RAM shape. The audit also caught a real
   slot-boundary host-write bug (wrong oscillator's registers loaded for
   one slot).
7. **Synthesis retiming absorbed pipeline registers into BSRAM output
   registers** (rev 3.5→3.6): the RTL had the two-stage path; the netlist
   didn't. 97→132 shallow violations that were structural, not variance.
8. **The terminal bug — GLU write-ack** (§5).

Plus two instrumentation defects that mattered: the drops counter left
unconnected (masked defect #8 for the entire campaign), and the
implicit-net trap (EX3638) that made the first ground-truth instrumentation
build read all zeros.

## 5. The terminal bug, in full

`sound_glu.sv`'s sound-RAM write queue advanced on `glu_mem_if.ready`:

- On the **SDRAM** port (a2n20v2-GS), `ready` pulses on write completion.
  The code is correct there and field-proven.
- On the **DDR3** port, the arbiter's write path is `S_WRITE → S_DONE` and
  never asserts `resp_valid`; the CDC generates `client_ready` *only* from
  response beats. **`ready` never pulses for a write.**

Consequences: the queue's `wr` stuck high forever; the CDC's
level-sensitive accept re-fired the same write indefinitely; the queue
never drained; every subsequent sample byte was dropped (counted by a
counter wired to nothing); **the wavetable in DDR3 was essentially never
written**. The DOC then did exactly what it was designed to do — fetch, on
time, with perfect bookkeeping — from memory containing garbage. Hence the
maddening signature: correct rhythm and note timing (register-driven),
garbage timbre, and every DOC-side counter pristine.

Why it survived so long:

- **It had never executed.** a2mega always shipped `USE_BSRAM=1`; the DDR3
  GLU branch was dead code until this campaign enabled it.
- **Its own comment described the symptom** ("a slow write dropped the
  next sample byte, audible as corrupted waveforms") — written for the
  SDRAM contention case, priming every reader to see a congestion issue,
  not a protocol mismatch.
- **The diagnostic asymmetry**: months of read-side instrumentation
  (stale/prime/drop/fetch counters, tags, classifiers) versus zero
  write-side instrumentation and one unconnected drops port that Gowin
  warned about (EX3073) in every single build log.
- **A masking interaction**: at its original priority *below* the FB, the
  permanently-pending port only stole spare grants — invisible. The
  campaign's own priority fix promoted it above the FB, converting the
  silent wedge into 180 late lines/frame — which is what finally made
  Hex 1's constant `08` (port-pending bitmask) undeniable.

The fix is three lines of substance: acknowledge writes by the `available`
handshake on DDR3 ports (`GLU_WR_ACK_AVAIL` parameter, so the GS keeps its
correct `ready` semantics), queue deepened 8→32, and the drops counter
finally on the overlay.

## 6. What found it — and what didn't

**Didn't:** three revisions of read-side hardening, ~40k paired-sample
differential simulation across two bank implementations and three seeds,
an adversarially-corrected contention model, exhaustive host-write phase
sweeps, netlist censuses, and five flash-test cycles. All of it was
looking at the reader.

**Did:** the user's escalating redirections, each of which removed a wrong
assumption of the analysis:

1. *"Does the delayed design actually meet the real timing demands?"* —
   forced the ground-truth instrumentation of the **working BSRAM build**
   (requests/frame, min-gap), which calibrated reality instead of models.
2. *"We've been burned simulating the DOC without the GLU"* — the co-sim
   directive put `sound_glu` and the real CDC/arbiter RTL in the loop for
   the first time; the seam it targeted is exactly where the bug lived.
3. The A/B discipline (BSRAM-perfect vs DDR3-distorted on identical
   builds) that kept eliminating hypotheses — prime-miss storms, register
   banks, cadence — until "the data is wrong at rest" was the only
   survivor, at which point reading the write path took minutes.

The overlay counters deserve credit too: once the write port was promoted,
**Hex 1 = 08, constant** was a one-glance conviction, and the final clean
run's numbers (05 / 39 / 00s) quantitatively matched the design math —
the difference between "seems to work" and "works for the reason we
designed."

## 7. Sim blind-spot catalog (now closed or documented)

1. Private-memory model with no shared-arbiter contention → rev 1.
2. Port contract assumed, not derived from CDC/arbiter RTL → rev 2.
3. `sound_glu`, the GLU write path, and the real CDC never instantiated in
   the TB → terminal bug invisible. (Co-sim rig now specified.)
4. Stream comparison aligned by service index — blind to absolute cadence
   (assertion class added even though cadence was ultimately not the bug).
5. Write-side data integrity never asserted — the golden-reference compare
   was of DOC outputs given *assumed-correct* memory.
6. Toolchain gaps: synthesis-passing ≠ mapped-as-intended (DFF census
   required); synthesis+census ≠ placeable (PnR-through probe required);
   RTL structure ≠ netlist structure (retiming; `syn_preserve`).

## 8. Collateral gains

- 28 BSRAM blocks freed on the 60K; the DOC subsystem is now *smaller in
  FFs than the baseline* (banks in DPB — a fix the baseline needed anyway
  on this die).
- The GW5A-has-no-SSRAM fact, which affects every module ported from
  GW2A boards and explains chronic CLS pressure (the framebuffer's
  intentional 7k-FF `wr_fifo` is now a known, documented cost).
- A differential + contention sim rig with hard assertions
  (prime_miss==0, FB deadlines, fetch budgets) and a rev-2 failure
  reproduction gate.
- Overlay instrumentation for DDR3 demand (requests/frame, min-gap) that
  works identically across BSRAM/DDR3 configs.
- The write-ack contract made explicit (`GLU_WR_ACK_AVAIL`) instead of
  implicit-and-wrong.
- Separately: the flashing saga fixes (openFPGALoader toggleClk floor
  v1.1.0+, sketch debounce, `--bulk-erase -f` no-op) — see the JTAG docs.

## 9. Transferable lessons

1. **When outputs are wrong and the reader's instruments are clean,
   suspect the data at rest — instrument the writer.** Corollary: an
   unconnected diagnostic port (EX3073) can be load-bearing evidence,
   silently discarded.
2. **A parked "infeasible" verdict is a claim about one design, not the
   feature.** Record what degree of freedom would un-park it.
3. **Never trust "Extracting RAM," synthesis success, or RTL structure**
   on Gowin: verify with a netlist DFF census, a PnR-through probe, and
   (for pipeline registers near BSRAM) `syn_preserve`.
4. **Derive port contracts from the RTL, cycle by cycle, before modeling
   them** — and simulate the integration seam (the module you hand-wrote
   between two validated things is the least-tested code in the system).
5. **Priority is also a diagnostic instrument**: promoting the wedged port
   is what made it visible. Silent-below-threshold failures deserve
   deliberate exposure.
6. **Dead code enabled is new code.** `USE_BSRAM=0` didn't flip a tested
   alternative on; it executed a branch for the first time, comments and
   all.
7. **The person with field history outranks the model.** Every major
   redirection in this campaign came from prior experience ("we've been
   burned simulating DOC without GLU", "harmonic distortion means memory
   time") — the analysis's job was to catch up honestly rather than defend
   its instruments.

## 10. Residual work

- Deep listening pass: synthLAB, Music Construction Set, POLY.SYNTH
  (swap-mode/one-shot edge cases) — the last acceptance box.
- Percussive/short-sound check (the old "DOC timeout" complaint predates
  this work; the timeout mechanism no longer exists on a2mega).
- 138B variant build with the same stack (fabric headroom makes it
  trivial); GS/Enhanced boards intentionally unchanged.
- Restore the displaced FB debug-overlay slots once burn-in is done;
  re-deploy the debounced ESP32 bridge sketch at next convenience.
- Commit the campaign (main tree is entirely uncommitted on top of
  `676a90a8`; agent worktree branches hold the module history).
- Upstream: the ESP32 sketch debounce may be worth offering to
  emard/esp32s3-jtag; the write-ack asymmetry note may help the next
  mem_port_if client author.
