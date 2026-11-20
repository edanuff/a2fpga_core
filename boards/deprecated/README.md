# boards/deprecated

> Status (2026-10-25): created when these three projects were moved out of the active board
> list. Nothing here is under development; the shared HDL under `hdl/` may still change
> underneath them, and nobody rebuilds them to check.

These are historical development boards that are not active, or builds for existing boards
that have been superseded. They are kept so the projects still build and the hardware that
exists can still be programmed, not because any of them is a release target.

| Directory | What it is | Why it is here |
|---|---|---|
| [a2n9/](a2n9/README.md) | A2FPGA card on the Tang Nano 9K with discrete level-shifting; Apple II and //e only, no IIgs, no SuperSprite | A development board that was never released beyond a limited set of testers. Superseded by the A2N20v2. |
| [a2n20v1/](a2n20v1/README.md) | The first Tang Nano 20K card, with the 64-pin a2bridge CPLD that does not carry the IIgs M2SEL/M2B0 lines | A development board that was never released beyond a limited set of testers. Superseded by the A2N20v2, whose 100-pin a2bridge adds the IIgs lines. |
| [a2n20v2-GS/](a2n20v2-GS/README.md) | A build for the production A2N20v2 card: Ensoniq DOC audio with its wavetable in SDRAM and an SDRAM framebuffer that keeps beam-racing software correct | A build, not a board, for hardware that is still current. It has been superseded by the [a2n20v2-Enhanced](../a2n20v2-Enhanced/README.md) build, which carries the GS-specific build features (the dual-rate SDRAM path with the SHR aux banks in SDRAM, and the `_fb` framebuffer variant) alongside the BL616 coprocessor features. |

## Building and flashing

The tools find these boards by name, exactly as before the move:

```bash
tools/build.sh a2n20v2-GS            # boards/deprecated/a2n20v2-GS/a2n20v2_gs.gprj
tools/flash.sh a2n20v2-GS            # -b tangnano20k
tools/flash.sh a2n9                  # -b tangnano9k
```

The project files reference the shared HDL as `../../../hdl/...` (one level deeper than the
active boards). The last committed bitstreams are under each `impl/pnr/`; the a2n9 and a2n20v1
ones predate the 2026 work on the shared HDL and have not been rebuilt since.

## What stays current

The shared modules these projects use are maintained for the active boards (`a2n20v2`,
`a2n20v2-Enhanced`, `a2mega`, `a2p25`); a change there is not tested against the deprecated
projects. The GS build's design record (the SDRAM framebuffer workplan) is in
[a2n20v2-GS/docs/archive/](a2n20v2-GS/docs/archive/README.md); the SDRAM framebuffer itself
lives on in `hdl/video/sdram_framebuffer.sv` and the Enhanced `_fb` build.
