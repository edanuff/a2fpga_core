# doc5503 pipelined-fetch differential simulation

Differential testbench proving that `hdl/sound/doc5503_pipelined.sv` (the
DDR3-latency-tolerant, pipelined-fetch DOC5503 with a per-oscillator word
cache) is architecturally equivalent to the baseline `hdl/sound/doc5503.sv`,
modulo a small set of documented, classified deviations — AND that its DDR3
fetch traffic no longer starves a competing framebuffer client on a
serialized arbiter (the failure the first prototype hit on a2mega hardware).

See `boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md` for the full design
analysis these results back.

## Running

Requires Icarus Verilog (`brew install icarus-verilog`) and python3:

```
./run.sh            # compile, simulate, classify
vvp tb_doc5503_diff.vvp +seed=N   # re-run with a different latency seed
```

`compare.py` exits nonzero if any difference between the two implementations
falls outside the expected deviation classes, if fetch traffic exceeds the
word-cache budget, or if the FB client misses a line deadline outside the
forced-overload stress phase.

## Structure

- `tb_doc5503_diff.sv` — instantiates BOTH DUTs on the same 54 MHz clock,
  7.159 MHz clock-enable DDS, host register bus, and shared 64 KB wavetable
  contents. The baseline gets a 2-clk BSRAM-class memory model. The
  pipelined DUT gets a **serialized-arbiter contention model** reflecting
  the a2mega `ddr3_ports` reality:
  - each grant occupies the server non-preemptibly for 600–1000 ns;
  - a competing FB client with strictly higher priority needs 16 word
    grants per 31.7 µs line window but — like the real CDC — can keep only
    one request pending at a time with a turnaround gap, which is exactly
    why a continuously-pending DOC steals alternate grants on hardware;
  - the DOC port accepts ≤2 outstanding requests (`ddr3_port_cdc`),
    responses in order, 32-bit **word** data sampled at grant completion
    (worst-case staleness). GLU writes pulse the DUT's `cache_flush_i`.
- `compare.py` — pairs the logged architectural event streams by service
  slot and classifies every difference; also asserts the traffic and
  FB-deadline policies (see its docstring).

Event streams logged from both DUTs (via hierarchical references to the
register-file write strobes, so no DUT modifications are needed): `W`
(per-oscillator sample stream: consumed byte, volume, effective address),
`C` (control writes: every halt, swap handoff, retrigger — must match
EXACTLY, this is the swap-drift check), `V` (volume writes incl. SYNC_AM
modulation), `M` (final mixes per scan).

## Test phases

| Phase | Scenario |
|-------|----------|
| 0 | Configuration writes, all oscillators halted (prime-once fetches) |
| 1 | Steady free-run, 3 oscillators, different FC/RTS/table sizes |
| 2 | One-shot sample with mid-word 0x00 terminator (halt-on-zero timing) |
| 3 | Swap-mode looped pair, 30,000 slots (~50+ loop iterations — drift check); mid-word terminators |
| 4 | SYNC/AM pair: hard-sync accumulator restarts + AM volume modulation |
| 5 | Mid-stream register writes: FC sweep, volume ramp, WTP retarget, RTS change, halt toggle, control rewrite on a running oscillator |
| 6 | E1 oscillator-count changes mid-playback (grow, shrink, restore) |
| 7 | GLU wavetable writes to a playing region, with cache_flush_i per write |
| 8 | **ALL 32 oscillators running against the FB client** — traffic + line-deadline assertion phase |
| 9 | Overload stress: 10% of DOC grants forced to ~6.1 µs against a 4.47 µs service period (stale-repeat fallback) |
| 10 | Recovery check after overload (must re-sync; FB misses must stop) |

## Result (as of this commit; seeds 1, 42, 7777 all PASS)

```
W records: 36336 paired, 90-103 differ (seed-dependent stress counts)
    ADDR_MAP_DELAY: 2        (WTP retarget: one sample fetched with old mapping)
    PRIME_MISS: 1            (control rewrite on running osc — designed)
    SYNC_RESTART: 9          (one modulator sample per hard-sync restart)
    SYNC_AM_VOL: 9           (AM fallout of SYNC_RESTART)
    STRESS_DEGRADED + LATE_FETCH_LAG + STALE_REPEAT: stress phase only
C records: IDENTICAL (122 events)  <- halt/swap/retrigger timing bit-exact,
                                      zero swap-loop drift, even under stress
V records: 1616 events, 9 sync-restart AM deviations, 0 unclassified
M records: 5562 scans; differing scans only at classified deviations and
           during forced overload; recovery phase clean

Traffic (fetch issues are DETERMINISTIC — identical across seeds):
    idle/config phase:      13 fetches total (prime-once; ~zero steady traffic)
    all-32 phase:           0.309 fetch/slot ≈ 277k fetches/s for a mixed-FC
                            population with steps up to 1.9 bytes/sample
                            (steps <=1 give <=0.25/slot ≈ 224k/s); the old
                            per-slot policy was 1.0/slot ≈ 895k/s
FB client:                  2536 line windows, 0 misses outside the forced
                            stress interval (1 miss during it, recovers)
DUT counters:               fetch_drop = 0 in ALL phases, including stress;
                            prime_miss = 1 (designed); stales stress-only
```

Outside the forced-overload phase, the pipelined variant is bit-exact AND
time-aligned with the baseline (not delayed): prefetch-priming means each
oscillator's first sample lands in the same service slot as the baseline's.
The pipeline manifests only as data staleness (GLU-write visibility,
WTP/RTS address-mapping changes, SYNC restarts between fetch and consume),
one sample per event. Notably, the flush-driven refresh closed the GLU-write
race window completely in these runs (no GLU_RACE samples observed — the
re-fetch lands before the stale lane is consumed); the class remains in the
classifier for latency mixes where the window stays open.

## Notes / coverage caveats

- The RTS-change scenario in phase 5 uses values with the same effective
  shift (9), so it exercises the mask/size change but not a shift change;
  the address-mapping-delay mechanism is identical to WTP (verified).
- Host writes are synchronized to service-slot starts by the TB. Writes
  landing within a few clk_i of the (different) internal fetch/consume
  points can apply one slot apart between the two implementations — an
  inherent ns-level knife edge, not a pipelined-specific artifact.
- Under forced overload a few lagged samples evade the exact-match lag
  classifier (the diagnostic address field can be refreshed by an in-flight
  retire between the consume decision and the log strobe) — those land in
  the stress-phase-only `STRESS_DEGRADED` catch-all; volume equality and
  C-record identity remain strictly enforced in every phase. A missed 0x00
  terminator under overload degrades to a table-end halt (bounded), which
  would surface as a C mismatch — none observed.
- The FB client model uses 16 single-word grants per line as specified for
  the contention study; the real a2mega FB reader uses 8-beat burst grants
  and is therefore strictly easier to satisfy than this model.
- Host writes to the $60-7F data registers are not exercised.
