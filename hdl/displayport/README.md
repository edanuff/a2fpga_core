# DisplayPort transmitter core

> Status (2026-10-25): the core has been developed **in this repository** since 2026-08
> (AUX front end and link policy from the a2mega census, M5 runtime AFE, timing round 2,
> the 4-lane MSA and SDP audio path, the a2p25 AUX/training stack) and runs on two boards:
> the a2mega (2-lane HBR, GW5A SERDES backend below) and the a2p25 (4-lane RBR through the
> GW5A-25 D-PHY, `boards/a2p25/hdl/dp/transceiver_bank_dphy.sv`). The vendoring paragraph
> that follows describes the August origin; whether `edanuff/DisplayPort_Verilog` is still
> the development home to re-sync with, or this directory is now the source of truth, is an
> open maintainer question (documentation audit, section 5). Until it is answered, treat the
> tree here as what the boards build.

Vendored from https://github.com/edanuff/DisplayPort_Verilog
(local dev sandbox: a checkout of that repository outside this one), branch
`master`, commit `87cd538` ("Add examples/a2_mega: board-specific SERDES
config, timing-verified"). MIT license; originally derived from Mike Field
(hamster)'s Artix-7 DisplayPort source, restructured into an instantiable
`dp_transmitter` with a Gowin GW5A SERDES backend.

**Upstream is the place to develop the core** (it has the Icarus
testbenches under `test_benches/` and golden-model checkers under `misc/`
that are not vendored here). Re-vendor by copying `src/**` over this
directory and updating the commit hash above.

## Layout

- `dp_transmitter.sv` — the single integration point. Pull-style video
  front end: the core *owns* `clk_pixel` (generated from the SERDES symbol
  clock via `gowin_pixel_pll`) and outputs `cx`/`cy`; you return
  `rgb[23:0]` for the previous cycle's coordinates, registered on
  `clk_pixel`. Audio: `clk_audio` strobe + `audio_sample_word[1:0]`
  (same contract as the vendored HDMI core). AUX is raw digital
  (`auxch_in/out/tri`) — the pseudo-differential tri-state buffer idiom
  lives in the board top. Link training / DPCD / EDID are handled
  entirely in fabric by `auxch/channel_managemnt.v`.
- `core/` — main stream processing, scrambler, training/idle patterns.
- `video/` — internal timing gen, pixel CDC FIFO, TU packer, MSA
  (1, 2 and 4-channel inserters all wired; `LANE_COUNT=4` is what the a2p25
  builds at 1080p since 2026-10-11, with the 4-lane SDP audio fix of the
  same day).
- `audio/` — SDP engine, Maud measurement, sample buffer.
- `auxch/` — AUX channel, hotplug decode (0.5 ms IRQ / 2 ms disconnect
  thresholds), link policy.
- `gowin/` — GW5A SERDES bank wrapper (`transceiver_bank_gowin.v`,
  fabric 8b10b because the GTR12 hard encoder can't force disparity for
  TPS2) and hand-written PLLA wrappers (`gowin_plls.v`).

Not vendored: upstream `src/artix7/` (Xilinx GTX backend),
`src/test_streams/` (legacy 73-bit stream test sources, incompatible with
`dp_transmitter`'s front end), `src/debug/` (7-segment driver).

## a2mega specifics

Compile-time defines (see `boards/a2mega/hdl/dp_gowin_defines.v` — must be
**first** in the `.gprj` file list): `DP_VENDOR_GOWIN`, `GOWIN_SERDES_IP`,
`GOWIN_PLL_IP`, `DP_SERDES_LANES_23`.

The generated Customized PHY IP for the a2mega board lives in
`boards/a2mega/hdl/gowin/60B/dp_serdes/` (gw5at60b-002, TX-only, QPLL0,
**Q0 lanes 2+3 bonded, master lane 2, refclk 135 MHz on Q0_REFCLK1,
2.7 Gb/s HBR, 20-bit fabric width, hard 8b10b OFF, `tx_pol_invert` on both
lanes**). The `dp_serdes.csr` sidecar must be registered in the board's
`.gprj.user` (`RES.serdes.csr = ...`) or PnR fails with CM2031.

Die-true lane mapping on the 1.0a3 board: DP0←L3, DP1←L2, DP2←L1,
DP3←L0, and **all four pairs are P/N swapped** (the SOM presents TXM on
the lower BTB pin of every pair — resolved 2026-08-09 from the Sipeed
60K schematic's U1L die symbol, which matches Gowin UG1222 exactly,
joined with the BTB-page nets; see boards/a2mega/docs/
a2mega_pinmap_1_0a3.csv). The 2-lane IP's `tx_pol_invert` on lanes 2+3
is correct as generated; a future 4-lane IP needs it on lanes 0 and 1
as well, plus a new `DP_SERDES_LANES_*` branch in
`transceiver_bank_gowin.v`.

Status of the a2mega backend: hardware-proven on both SOMs (60K from 2026-08,
138K from 2026-08-20; the 138K has its own IDE-generated SERDES IP under
`boards/a2mega/hdl/gowin/138B/dp_serdes/` and die lanes 1+2 instead of 2+3),
through hubs, converters and direct DP monitors (`boards/a2mega/docs/
adapter_census_results.md`). The executed bring-up plan:
`boards/a2mega/docs/BRINGUP_1_0A3_PLAN.md`; the bench log: `test_log.md`.
Timing: `boards/a2mega/docs/timing_round2.md` (every change to the AUX
ladder runs `sim/shadow_diff.py` against a git revision).
