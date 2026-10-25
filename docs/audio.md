# Audio

> Status (2026-10-25): rewritten against the board tops. Every board now runs its IIgs
> sound in the fabric; the a2mega and a2p25 DOC wavetables live in DDR3 / HyperRAM behind the
> pipelined DOC, and the DisplayPort boards carry audio in DP secondary-data packets.

How the core's sound sources are mixed, filtered, crossed into the display clock domain, and
embedded in the HDMI or DisplayPort stream. Output is **16-bit stereo at 44.1 kHz**.

## The chain

```
sources (clk_logic 54 MHz)  →  mix (sum, signed 16-bit)  →  CDC (→ clk_pixel)
   →  filter (audio_out: IIR low-pass + DC block; optional)
   →  HDMI audio packets (Tang Nano boards)  |  DP SDP audio packets (a2mega, a2p25)
```

## Sources

Each source is produced on the logic clock and exposes a small audio output:

| Source | Module | Output | Width | Notes |
|---|---|---|---|---|
| Apple II speaker | [`apple_speaker`](../hdl/sound/apple_speaker.sv) | `speaker_o` | 1-bit | Toggled by `$C030`; decays |
| Mockingboard | [`mockingboard`](../hdl/mockingboard/mockingboard.sv) | `audio_l_o`/`audio_r_o` | ~10-bit, stereo | Two AY-3-8910/YM2149 PSGs |
| SuperSprite | [`supersprite`](../hdl/supersprite/supersprite.sv) | `ssp_audio_o` | ~10-bit, mono | AY-3-8910 on the sprite card |
| Ensoniq DOC 5503 | [`sound_glu`](../hdl/sound/sound_glu.sv) → [`doc5503`](../hdl/sound/doc5503.sv) or [`doc5503_pipelined`](../hdl/sound/doc5503_pipelined.sv) | `audio_l`/`audio_r` | signed 16-bit, stereo | IIgs 32-voice wavetable; see the board table |

The DOC's 64 KB wavetable is a memory client. `sound_glu` selects the implementation with
parameters: `USE_BSRAM` (wavetable in block RAM; the GS and Enhanced builds keep it in SDRAM
through the baseline DOC's memory port), or `USE_DDR3_PIPELINED` (the rev-3 pipelined DOC,
which issues each oscillator's fetch during its service slot and consumes it a slot later, so
DDR3 / HyperRAM tail latency never stalls the oscillator loop). The pipelined DOC is proven
equivalent to the baseline by the differential bench in [`sim/doc5503/`](../sim/doc5503/README.md);
its design and the a2mega port history are in
[`boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md`](../boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md).
On the a2p25 the DOC's HyperRAM port must be the arbiter's highest priority and runs through
[`doc_fetch_cache`](../hdl/sound/doc_fetch_cache.sv); the per-frame LATE counter the ESP32
firmware reads is the audio health metric (see
[`boards/a2p25/docs/RESOURCE_AUDIT.md`](../boards/a2p25/docs/RESOURCE_AUDIT.md)).

[`i2s_receiver`](../hdl/sound/i2s_receiver.sv) remains in `hdl/sound/` from the abandoned
ES5503-on-ESP32 path of the previous a2p25 revision; no current project builds it.

## Mixing

In `top.sv`, the unsigned sources are left-extended to a common width and summed with the signed
DOC output into a signed 16-bit stereo pair (the shape is the same on every board; from
[a2n20v2-Enhanced/hdl/top.sv](../boards/a2n20v2-Enhanced/hdl/top.sv)):

```systemverilog
wire [12:0] speaker_audio_ext_w = {speaker_audio_w, 12'b0};
wire [12:0] ssp_audio_ext_w     = {ssp_audio_w, 3'b0};
wire [12:0] mb_audio_l_ext_w    = {mb_audio_l, 3'b0};   // (and _r)
assign core_audio_l_w = sg_audio_l + ssp_audio_ext_w + mb_audio_l_ext_w + speaker_audio_ext_w;
assign core_audio_r_w = sg_audio_r + ssp_audio_ext_w + mb_audio_r_ext_w + speaker_audio_ext_w;
```

The sum can in principle overflow by a bit; the filter stage clamps (saturates) rather than
reserving headroom. Boards without a card simply omit its term (the profile switches the card
out; see [build_profiles.md](build_profiles.md)).

## Filtering

[`audio_out`](../hdl/sound/audio_out.v) (built on [`iir_filter`](../hdl/support/iir_filter.v),
from the MiSTer/Sorgelig audio lineage) band-limits the mixed signal before it's resampled to
44.1 kHz, then runs a **DC blocker** (high-pass) to remove offset. The filter is configured by
coefficients passed from `top.sv` (`flt_rate`, `cx`/`cx0..2`, `cy0..2`) on the Tang Nano boards;
treat those values as the tuned configuration. The a2mega and a2p25 tops carry
`ENABLE_FILTER = 0` and feed the mixed pair to the DP transmitter unfiltered.

## Clock-domain crossing & sample timing

- **CDC:** the mixed audio is resampled from `clk_logic` (54 MHz) into the display pixel clock
  (27 MHz HDMI, 148.5 MHz DP) by [`cdc_sampling`](../hdl/support/cdc_sampling.sv) (`WIDTH=16`,
  one instance per channel) or, on the DP boards, by a two-stage register on `clk_pixel`. Audio
  changes slowly relative to both clocks, so sampling the word across the boundary is safe.
- **Sample timing:** [`audio_timing`](../hdl/sound/audio_timing.sv) (HDMI boards) or a divider
  in the top (DP boards) generates the `clk_audio` sample strobe at 44100 Hz.

## Into the display stream

- **HDMI (Tang Nano boards):** the filtered `audio_sample_word[1:0]` (L/R, 16-bit) feeds the
  [`hdmi`](../hdl/hdmi/hdmi.sv) module (`AUDIO_RATE=44100`, `AUDIO_BIT_WIDTH=16`), which builds
  IEC-60958 audio sample packets ([`audio_sample_packet`](../hdl/hdmi/audio_sample_packet.sv))
  and the N/CTS clock-regeneration packets
  ([`audio_clock_regeneration_packet`](../hdl/hdmi/audio_clock_regeneration_packet.sv)),
  interleaved with video into the TMDS stream.
- **DisplayPort (a2mega, a2p25):** the same `clk_audio` + `audio_sample_word[1:0]` contract
  goes into [`dp_transmitter`](../hdl/displayport/dp_transmitter.sv), whose
  [`sdp_engine`](../hdl/displayport/audio/sdp_engine.v) emits audio secondary-data packets
  (with Maud measurement) between the video transfer units; it supports 1, 2 and 4-lane links.

## Board differences

| Board | Speaker | Mockingboard | SuperSprite | Ensoniq DOC | Output |
|---|---|---|---|---|---|
| a2mega 60K | ✓ | off (die pkg) | off (die pkg) | ✓ pipelined DOC, wavetable in **DDR3** | DP SDP |
| a2mega 138K | ✓ | ✓ | ✓ | ✓ pipelined DOC, wavetable in **DDR3** | DP SDP |
| a2p25 | ✓ | off (budget) | off (budget) | ✓ pipelined DOC, wavetable in **HyperRAM** (fetch cache, port priority 0) | DP SDP |
| a2n20v2-Enhanced | ✓ | ✓ | ✓ | ✓ baseline DOC, wavetable in SDRAM | HDMI |
| a2n20v2-GS | ✓ | ✓ | ✓ | ✓ baseline DOC, wavetable in SDRAM | HDMI |
| a2n20v2 | ✓ | ✓ | ✓ | ✗ | HDMI; production, no IIgs sound |
| a2n20v1 / a2n9 | ✓ | ✓ | ✓ (n9: no SSP) | ✗ | HDMI; deprecated |

The DOC path is switched by `A2_FEATURE_ENSONIQ` in each board's `*_profile.vh`.

## Adding a sound source

1. Produce a sample stream (signed preferred; unsigned is fine if you document the offset).
2. Add it to the `core_audio_l_w` / `core_audio_r_w` sum in `top.sv`, left-extending to match the
   mix width. Mind the overflow note above — scale so the combined sum stays in range.
3. If it has a memory-backed table, make it a `mem_port_if` client and read
   [memory-system.md](memory-system.md) on port priority first: the DOC's deadline is the
   hardest real-time constraint in the system.

## See also

- [gotchas.md](gotchas.md) — audio-clock driver pitfalls (a missing `clk_audio` driver sweeps the
  audio path away in synthesis — a real bug on a2n20v2-Enhanced, since fixed) and the DOC-on-DDR3
  history.
- [architecture.md](architecture.md) — where audio sits in the whole design.
- [memory-system.md](memory-system.md) — the DOC and GLU memory ports on each board.
