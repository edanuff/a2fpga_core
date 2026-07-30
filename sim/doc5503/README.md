# doc5503 pipelined-fetch differential simulation

Differential testbench proving that `hdl/sound/doc5503_pipelined.sv` (the
DDR3-latency-tolerant, pipelined-fetch DOC5503) is architecturally equivalent
to the baseline `hdl/sound/doc5503.sv`, modulo a small set of documented,
classified deviations.

See `boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md` for the full design
analysis these results back.

## Running

Requires Icarus Verilog (`brew install icarus-verilog`) and python3:

```
./run.sh
```

This compiles both DUTs plus `tb_doc5503_diff.sv`, runs ~58 ms of simulated
time (~52,000 oscillator service slots), and then runs `compare.py`, which
exits nonzero if any difference between the two implementations falls outside
the expected deviation classes.

## Structure

- `tb_doc5503_diff.sv` — instantiates BOTH DUTs on the same 54 MHz clock,
  7.159 MHz clock-enable DDS, host register bus, and shared 64 KB wavetable
  contents. The baseline gets a 2-clk BSRAM-class memory model; the pipelined
  DUT gets a serialized, randomized-latency DDR3-class model (uniform
  200–1200 ns, 2% 2 µs outliers, in-order responses, ≤2 outstanding like
  `ddr3_port_cdc`, data sampled at completion time for worst-case staleness).
- `compare.py` — pairs the logged architectural event streams by service slot
  and classifies every difference (see its docstring for the class
  definitions and tolerance policy).

Event streams logged from both DUTs (via hierarchical references to the
register-file write strobes, so no DUT modifications are needed):

- `W` — waveform-data-sample register writes: the consumed/fetched byte,
  the volume it is scaled by, and the wavetable address it came from. One
  per running-oscillator service slot; this is the per-oscillator sample
  stream.
- `C` — control register writes: every halt, swap handoff, retrigger, and
  host control write. Must match EXACTLY (this is the swap-drift check).
- `V` — volume register writes (host writes + SYNC_AM modulation writes).
- `M` — final mono/left/right mixer outputs, once per scan.

## Test phases

| Phase | Scenario |
|-------|----------|
| 0 | Configuration writes, all oscillators halted (prefetch priming) |
| 1 | Steady free-run, 3 oscillators, different FC/RTS/table sizes |
| 2 | One-shot sample with 0x00 terminator (halt-on-zero timing) |
| 3 | Swap-mode looped pair, 30,000 slots (~50+ loop iterations — drift check) |
| 4 | SYNC/AM pair: hard-sync accumulator restarts + AM volume modulation |
| 5 | Mid-stream register writes: FC sweep, volume ramp, WTP retarget, RTS change, halt toggle, control rewrite on a running oscillator |
| 6 | E1 oscillator-count changes mid-playback (grow, shrink, restore) |
| 7 | GLU wavetable writes to a region being played |
| 8 | Overload stress: 10% of fetches forced to ~6.1 µs against a 4.47 µs service period (stale-repeat fallback + FIFO overflow) |
| 9 | Recovery check after overload (must re-sync with zero deviations) |

## Result (as of this commit)

```
W records: 18527 paired, 357 differ
    ADDR_MAP_DELAY: 2       (WTP retarget: one sample fetched with old mapping)
    LATE_FETCH_LAG: 138     (stress phase only)
    PRIME_MISS: 1           (control rewrite on running osc — designed)
    STALE_REPEAT: 206       (stress phase only)
    SYNC_AM_VOL: 5          (AM fallout of SYNC_RESTART)
    SYNC_RESTART: 5         (one modulator sample per hard-sync restart)
C records: IDENTICAL (197 events)   <- halt/swap/retrigger timing bit-exact,
                                       zero swap-loop drift, even under stress
V records: 988 events, 5 sync-restart AM deviations, 0 unclassified
M records: 4963 scans, 206 differ (phases 4/5/6: 8 scans traceable to the
                                   classified deviations; phase 8: 198 under
                                   forced overload; phase 9 recovery: 0)
RESULT: PASS
```

Outside the forced-overload phase, the pipelined variant is bit-exact AND
time-aligned with the baseline (not delayed): the prefetch-while-halted
priming means each oscillator's first sample lands in the same service slot
as the baseline's. The one-service-period pipeline manifests only as data
staleness (GLU-write visibility, WTP/RTS address-mapping changes, SYNC
restarts landing between fetch and consume), one sample per event.

## Notes / coverage caveats

- The RTS-change scenario in phase 5 uses values with the same effective
  shift (9), so it exercises the mask/size change but not a shift change;
  the address-mapping-delay mechanism is identical to WTP (verified).
- Host writes are synchronized to service-slot starts by the TB. Writes
  landing within a few clk_i of the (different) internal fetch/consume
  points can apply one slot apart between the two implementations — an
  inherent ns-level knife edge, not a pipelined-specific artifact.
- The DDR3-model latency randomization is seedable: `vvp tb_doc5503_diff.vvp
  +seed=N`. Seeds 1, 42, and 7777 all PASS. Under forced overload a few
  lagged samples can evade the exact-match lag classifier (the diagnostic
  tag field can be refreshed by an in-flight retire between the consume
  decision and the log strobe) — those land in the stress-phase-only
  `STRESS_DEGRADED` catch-all; volume equality and C-record identity remain
  strictly enforced in every phase.
- Host writes to the $60-7F data registers are not exercised (they would
  appear in the W stream); neither implementation treats them specially.
