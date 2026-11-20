# 4-lane RBR scope (a2mega DP)

Thesis: move off our bespoke 2-lane HBR raw/unbonded mode to the
configuration the Gowin reference designs are validated in — 4 lanes,
lower per-lane rate (RBR 1.62 G, 1.67× wider UI), canonical framing —
to see whether the ~20% boot marginality dissolves rather than needs a
reload workaround. Revert anchor: tag `a2mega-2lane-planc-4of5`.

## Bandwidth / feasibility
- 4×1.62 = 6.48 Gb/s raw, ×8/10 = 648 MB/s usable. 1080p60 = 148.5M px
  × 3 B = 445.5 MB/s. Fits with margin. (2-lane HBR was 5.4/432 usable —
  we GAIN headroom.)
- Per-lane UI: 617 ps (RBR) vs 370 ps (HBR) → 1.67× more timing margin
  against skew/jitter/startup-phase, the suspected marginality source.

## USB-C / PD / mux: ZERO CHANGES ✅
- Firmware already negotiates **Pin Assignment C** (4-lane DP-only):
  `usb_pd_dp_configure_vdo()` → USB_PD_DP_PIN_C; `find_dp_mode` requires
  PIN_C. (usb_pd.h:177, usbc_port.c:307)
- Mux already **CTLSEL_DP4** (four-lane) when DP active. (usbc_glue.cpp:214)
- HPD, flip, EQ override all unchanged.

## DP core RTL: parameter-only + one wiring gap found ✅
The vendored core is genuinely 1/2/4-lane and rate-parameterized:
- `video_stream_packer` LANE_COUNT ∈ {1,2,4} (pixel-interleaved), VBID
  repeats = 4/LANE_COUNT — generic. (video_stream_packer.v:35)
- `msa_inserter_4ch` present; `skew_channels` present.
- Ladder already has `set_link_count_4`; DPCD LINK_BW_SET byte is emitted
  as `LINK_RATE_MBPS/270` → 1620/270 = 0x06 (RBR) automatically.
  (dp_aux_messages.v:129)
- So: `dp_transmitter #(.LANE_COUNT(4), .LINK_RATE_MBPS(1620))`.
- **GAP FOUND & FIXED**: the MSA generate was `if (LANE_COUNT==1) 1ch
  else 2ch` — no 4-lane branch, so LANE_COUNT=4 would have silently used
  the 2-channel inserter. `msa_inserter_4ch` existed but was never wired.
  Added as the first branch.
- The 80-bit tx_symbols bus is 8 x 10-bit symbol slots and was always
  4-lane-wide; 2-lane only ever filled the low half. Lane N = bits
  [20N +: 20]. Encoders are now a generate loop over ENC_LANES.

## transceiver_bank_gowin.v: the real RTL work (mechanical)
Currently hardwired 2-lane (ln2/ln3) under `DP_SERDES_LANES_23`. Extend to
4-lane:
- 4 lane encoders (have 2), 4 tx_if FIFO connections, 4 pma/pcs resets.
- Lane map (die-true, from pinmap): ML0→ln3, ML1→ln2, ML2→ln1, ML3→ln0;
  all four TXM-first, all P/N-swapped (tx_pol_invert set in IP).
- Symbol clock from the bond/clock master lane's PCS clkout (one lane
  feeds all four fabric_tx_clk).
- DRP reader, CSR-replay FSM, watchdog plumbing: unchanged (they're
  quad-level, not per-lane-count).

## PHY IP regen (user, IDE) — the experiment knob
Customized PHY, Q0 **all 4 lanes**, **RBR 1.62 G**, width 20, encode OFF,
tx_pol_invert all 4, DRP on, swing TBD (804 mV likely still fine; RBR may
want less pre-emphasis). **Bonding decision (the crux):**
- Option 1 (recommended first): un-bonded 4-lane RBR — isolates the
  rate/margin variable alone. Simplest delta from known-good.
- Option 2 (canonical): FULL 4-lane channel bonding (chbond_enable=true,
  master lane, align pattern) — the true reference-design framing. This
  is what the thesis really tests, but the half-bond burned us, so it must
  be a COMPLETE bond, verified in emission (chbond_enable=true, not the
  mst_sel-only half-bond).
- Plan: try Option 1 first (does lower rate alone fix it?); if still
  marginal, Option 2 (does proper framing fix it?).

## Clocking (CORRECTED — earlier draft of this doc was wrong)
- Symbol clock 135 → **81 MHz** (1.62 G / 20). SDC `clk_sym` period
  7.407 → **12.346 ns**. Clock master stays die lane 2, so the SDC anchor
  pin `LANE2_PCS_TX_O_FABRIC_CLK` is UNCHANGED — only the period moves.
- Pixel PLL: an earlier draft said to re-source it off a stable reference
  instead of the symbol clock. **That was wrong** — this is DP
  *synchronous-clock mode*: clk_pixel must stay locked to the symbol
  clock (the MSA M/N ratio encodes exactly that relationship). The real
  change is the PLL RATIO:
    2-lane HBR: 135 MHz in, IDIV 5 / MDIV 44 / ODIV0 8 → VCO 1188 → 148.5
    4-lane RBR:  81 MHz in, IDIV 1 / MDIV 11 / ODIV0 6 → VCO  891 → 148.5
  `gowin_pixel_pll` now takes IDIV/MDIV/FCLKIN as parameters (defaults =
  the old hardcoded values, so 2-lane builds are unchanged), and
  dp_transmitter asserts at elaboration that the divider triple actually
  reproduces F_PIXEL_HZ from F_SYMBOL_HZ.
- Derived params all check out at LANE_COUNT=4 / MULT 11 / DIV 6:
  SYMS_PER_LINE 2400, TU fill 44/64 (= 445.5/648 MB/s), M_VALUE 480597
  (=148.5/162, 0.7 ppm rounding), capacity assert passes.

## Test protocol (unchanged rig)
Flash (rescue-first), 5+ cold boots logo-up, count K:03/colorbars vs K:00.
Success = the 20% marginality gone (target 5/5, several rounds). Soak +
flip-kill recovery as before. Telemetry unchanged (U: now 4 lanes' worth —
widen field or keep master pair).

## Cost estimate
- transceiver_bank 4-lane extension: ~1 hr careful RTL (mechanical).
- Pixel-PLL re-source: ~30 min + care.
- SDC/params: minutes.
- User IDE regen: ~10 min.
- Build + rescue-flash + boot test: ~30 min/round.
Total to first hardware result: ~half a day. Revert is one `git checkout`.
