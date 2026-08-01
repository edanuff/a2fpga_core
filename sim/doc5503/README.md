# doc5503 pipelined-fetch differential simulation

Differential testbench proving that `hdl/sound/doc5503_pipelined.sv` (rev
3: DDR3-latency-tolerant DOC5503 with per-oscillator 16-byte line cache,
lookahead issue, and FB-aware gating) is architecturally equivalent to the
baseline `hdl/sound/doc5503.sv`, modulo a small set of documented,
classified deviations — AND that it coexists with the framebuffer on the
REAL serialized DDR3 arbiter contract (which broke revs 1 and 2 on a2mega
hardware).

See `boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md` (§11-§12) for
the hardware history and the line-cited port-contract accounting this
model implements.

## Running

Requires Icarus Verilog (`brew install icarus-verilog`) and python3:

```
./run.sh                            # repro check + rev-3 suite + classifier
vvp tb_doc5503_diff.vvp +seed=N     # rev-3 suite with a different seed
```

`run.sh` first compiles `-DREV2_MODE` and runs with `+nogate` — lookahead
off, FB gating off, plus a synthetic word-granularity DOC-class load at
rev-2's measured fetch rate — and REQUIRES chronic FB line-deadline misses
(reproducing the rev-2 field failure). It then runs the rev-3 build, which
must PASS `compare.py`: bit-exact-modulo-classified-deviations correctness,
fetch-traffic bounds, zero FB misses outside forced overload, zero drops
before the stress phase.

## The memory model (corrected, rev 3)

Derived cycle-by-cycle from `hdl/ddr3/ddr3_port_cdc.sv`,
`hdl/ddr3/ddr3_ports.sv`, and `hdl/video/framebuffer_480p.sv`, calibrated
with controller read latency K≈20 ddr cycles (matches the known ~550 ns
uncontended round trip):

- DOC port: 2-entry CDC request FIFO (available deasserts at occupancy 2),
  strictly single-file service; request arbiter-visible ~6 clk after the
  rd pulse; next queued request visible ~4 clk after req_done; 4-beat
  burst grant of ~29 ddr ≈ 20 clk_logic, non-preemptible; four response
  beats delivered one/clk starting ~3 clk after completion.
- FB client (priority above DOC): 640-wide line = 320 words = 40 burst8
  grants of ~35 clk each, issued BACK-TO-BACK (the 2-deep CDC keeps the
  port pending ~3 clk after each completion — the real reason FB priority
  alone never protected the display from a chatty DOC). 2-line prefetch,
  continuous catch-up when behind, 240 active + 22 vblank lines per frame.
- Background client (shadow reads/writes + FB writes): 12-clk grants,
  ~6% duty, above DOC priority.
- 3% of FB/DOC grants +14 clk (tRFC refresh collision).
- GLU writes pulse `cache_flush_i`; the TB drives `fb_fetch_active_i`
  from the FB engine's fetch-in-progress state.

Aggregate high-priority utilization is ~90% of active-display time: the
DOC lives on the leftovers, exactly as on hardware.

## Test phases

| Phase | Scenario |
|-------|----------|
| 0 | Configuration writes, all oscillators halted (prime-once fetches) |
| 1 | Steady free-run, 3 oscillators, different FC/RTS/table sizes |
| 2 | One-shot sample with mid-line 0x00 terminator (halt-on-zero timing) |
| 3 | Swap-mode looped pair, 30,000 slots (~50+ loop iterations — drift check) |
| 4 | SYNC/AM pair: hard-sync accumulator restarts + AM volume modulation |
| 5 | Mid-stream register writes: FC sweep, volume ramp, WTP retarget, RTS change, halt toggle, control rewrite on a running oscillator |
| 6 | E1 oscillator-count changes mid-playback (grow, shrink, restore) |
| 7 | GLU wavetable writes to a playing region, with cache_flush_i per write |
| 8 | **ALL 32 oscillators running against the FB engine** — traffic + line-deadline assertion phase |
| 9 | Overload stress: 10% of DOC grants forced to ~6.1 µs against a 4.47 µs service period (stale-repeat fallback) |
| 10 | Recovery check after overload (must re-sync) |

## Result (as of this commit; seeds 1, 42, 7777 all PASS)

```
Rev-2 reproduction (REV2_MODE + nogate):  2019 / 2337 FB line deadlines
    missed — chronic display corruption, matching the field failure.

Rev 3:
W records: 36336 paired, ~50 differ
    ADDR_MAP_DELAY: 2      (WTP retarget: one sample fetched with old mapping)
    PRIME_MISS: 1          (control rewrite on running osc — designed)
    SYNC_RESTART: 13       (one modulator sample per hard-sync restart, plus
                            stress/small-E1 wrap-fetch overlaps — see doc)
    SYNC_AM_VOL: 9         (AM fallout of SYNC_RESTART)
    STRESS_DEGRADED: ~26   (forced-overload phase only)
C records: IDENTICAL (122 events)  <- halt/swap/retrigger timing bit-exact,
                                      zero swap-loop drift, even under stress
V records: 1616 events, 9 sync-restart AM deviations, 0 unclassified

Traffic (fetch issues DETERMINISTIC — identical across seeds, 3154 total):
    idle/config phase:  20 fetches total (prime-once; ~zero steady traffic)
    all-32 phase:       0.078 fetch/slot ≈ 70k grants/s ≈ 2.6% arbiter
                        (rev 2 word cache: ~0.31/slot ≈ 9%; rev 1: 1.0/slot)
FB: 2337 line windows, 0 missed — in EVERY phase, including all-32 and
    the forced-overload stress phase.
Counters: fetch_drop = 0 everywhere; prime_miss = 1; stales pre-stress <= 13.
```

Outside forced overload the pipelined variant remains bit-exact AND
time-aligned with the baseline (prefetch priming puts each oscillator's
first sample in the same service slot as the baseline's); the pipeline
manifests only as one-sample data staleness at the documented event
classes.

## Notes / coverage caveats

- The RTS-change scenario keeps the effective shift constant (mask/size
  change only); the address-mapping-delay mechanism is identical to WTP.
- Host writes are TB-synchronized to service-slot starts (sub-slot
  register-application knife edge is inherent to any implementation).
- Under forced overload a few lagged samples land in the stress-only
  `STRESS_DEGRADED` catch-all (diagnostic address field can be refreshed
  by an in-flight retire between consume and log strobe); volume equality
  and C-record identity are strictly enforced in every phase.
- Wrap/retarget jump fetches keep a 1-service-period deadline; at very
  small E1 with gating active a wrap fetch can go stale once per event
  (classified; unrealistic config).
- The controller read latency K is calibrated, not measured; rev-3
  margins (deadline ×16, ~2.6% occupancy) are insensitive to it.
- Host writes to the $60-7F data registers are not exercised.
- Rev 3.1: the {tag, line} cache payload moved from flip-flops into an
  inferred synchronous-read RAM (`doc_cache_ram`) after a GW5AT-60
  placement failure; the suite was re-run in full and is result-identical
  (design doc §12.7).
