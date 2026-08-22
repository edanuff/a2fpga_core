# CDC / timing-constraint audit (a2mega DP, 08-22)

**Why.** Test log row 85: the SAME RTL, with only placement perturbed
(`set_option -place_option 1`), went from 6 blinks-to-lock to ZERO. Every
build reports 0 setup/hold violations and TNS 0.000, and `Fmax` has no
predictive power (the tightest `cm_life`, 100.091 MHz, gave the BEST
behavior). When STA says everything passes and behavior still tracks
placement, the paths that vary are ones STA is not looking at.

## 1. The blanket exemption

`boards/a2mega/hdl/dp_test_138b/a2mega_dp_test_138b.sdc` (and the 60B twin)
ends with:

```
set_clock_groups -asynchronous -group {clk50} -group {clk100} \
                               -group {clk_sym} -group {clk_pix} -group {cm_life}
```

This makes **every** path between the five domains unanalyzed — not
"relaxed", but excluded from timing entirely. Their delays are whatever
placement gives them, which is exactly the observed symptom.

The accompanying comment claims:

> Domains exchange data only through the gray-coded CDC FIFO and 2FF
> synchronisers; no synchronous cross-domain paths exist.

**That claim is not accurate.** Findings below.

## 2. What is actually fine

- `hdl/displayport/video/pixel_cdc_fifo.v` — genuine gray-coded dual-clock
  FIFO, pointers crossed through `(* ASYNC_REG = "TRUE" *)` 2FF pairs.
  Correct by construction.
- `enc_rst_sync` in `transceiver_bank_gowin.v` — `pcs_tx_rst` crossed into
  `tx_symbol_clk` through a proper 2FF before use.
- Telemetry sampling in `a2mega_dp_test_top.sv` (`st_s0/st_s`, `afe_s0/afe_s`,
  `afe1_s0/afe1_s`, `evt_s0/evt_s`) — 2FF, display-only.
- `afe_adjust_seq` `apply_tgl`/`ack_tgl` — toggle handshake with a
  quasi-static payload, the standard pattern. **But see §4: the payload is
  unbounded, so "quasi-static" is an assumption placement can break.**

## 3. What is NOT fine (unsynchronized cross-domain reads)

1. **`lane_ready` into `mgmt_clk`** — `transceiver_bank_gowin.v`:
   `assign lane_ready = {rdy_ln2 & rdy_ln0, rdy_ln3 & rdy_ln1};` where
   `rdy_ln*` are SERDES IP outputs asynchronous to `mgmt_clk`, consumed
   directly in the `always @(posedge mgmt_clk)` block:
   `tx_running <= {2{!pcs_tx_rst}} & powerup_eff & lane_ready;`
   No synchronizer. Two bits, so they can also be captured skewed.
2. **`pll_lock` into `mgmt_clk`** — same origin, no synchronizer, published
   in `serdes_status[5]`.
3. **`serdes_status` is a MIXED-DOMAIN vector**: `{fifo_afull_used,
   fifo_full_used, pll_lock, lane_ready, ~pcs_tx_rst, tx_running}` combines
   `tx_symbol_clk` signals (the FIFO flags), asynchronous SERDES outputs
   (`pll_lock`, `lane_ready`) and `mgmt_clk` registers. Any consumer sampling
   it as a unit gets bits from three domains.
4. **`phy_reinit` (introduced 08-22, MINE)** — `dp_transmitter.sv`:
   `assign afe_phy_reinit = ~serdes_status[5] | ~serdes_status[2] | wdog_replay_req;`
   feeds the AFE sequencer's `mgmt_clk` state machine directly from the
   unsynchronized `pll_lock`. It can glitch or capture metastable, spuriously
   clearing the applied-state baseline. Low impact today (`applies=0` on both
   hubs, so the AFE state is inert) but it is a real defect I added.

## 4. Why this plausibly explains the lottery

With the blanket `-asynchronous`, none of §3 is bounded, and the
"quasi-static payload" assumptions in the handshakes are unverified by the
tool. A placement that routes a payload bit slower than its toggle, or that
lengthens an unsynchronized status path, changes behavior with no timing
report ever moving. That matches: identical RTL, 0 violations everywhere,
behavior swinging 0 <-> 6 blinks with placement.

**Status: hypothesis, not proof.** No specific net has been tied to the
acquisition symptom yet. What is proven is (a) placement changes behavior
and (b) STA is blind to the cross-domain paths.

## 5. Recommended work, in order

1. **Fix the unsynchronized reads (RTL).** 2FF-synchronize `pll_lock` and
   `lane_ready` into `mgmt_clk` inside the bank before any use; synchronize
   the `phy_reinit` sources into `mgmt_clk` in `dp_transmitter`. These are
   correctness fixes independent of the lottery question.
2. **Bound the crossings (SDC).** Replace the blanket grouping with explicit
   per-crossing bounds so placement cannot stretch them, e.g.
   `set_max_delay -from <src reg> -to <dst sync FF> <period>` on each
   handshake payload and each synchronizer input, keeping the async grouping
   only for genuinely unrelated clocks. **Verify Gowin SDC support for
   `set_max_delay` / `-datapath_only` in this toolchain version before
   relying on it** — if unsupported, the fallback is to keep the payload
   registers physically grouped and rely on RTL discipline.
3. **Re-measure the lottery.** Build the same source several times with
   different `-place_option` values and count blinks on each. If the spread
   collapses after 1+2, the hypothesis is confirmed and build quality stops
   being luck.

## 6. Bench-process consequence (already adopted)

Every behavioral comparison must hold placement constant or be repeated
across placements. Rows 76/77 and 81 attributed behavior to RTL semantics
from single builds; those attributions are confounded and are flagged in
the test log as needing re-validation.
