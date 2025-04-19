# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands
- Test DOC5503 sound module: `cd tests/sound && make && make run`
- View sound waveforms: `cd tests/sound && ./view_waves.sh`
- Build PicoSoC firmware: `cd src/picosoc/firmware && make`
- Build boot kernel: `cd src/picosoc/boot && make`
- Program Tang Nano 20K: `openfpgaloader -b tangnano20k -f boards/a2n20v2/impl/pnr/a2n20v2.fs`

## Project Structure
- Each board type has its own directory under `boards/` (a2n20v1, a2n20v2, a2n20v2-Enhanced, a2n9)
- Each board directory contains a `hdl/` subdirectory with its own `top.sv` file as the top-level module
- Board-specific HDL files are in the board's `hdl/` directory
- Common HDL files shared across boards are in the top-level `hdl/` directory
- **Important**: Always examine code in the context of a specific board's `top.sv` file
- All functionality is configured within each board's top module (slots, peripherals, etc.)

## Code Style Guidelines
- Use SystemVerilog (.sv) for HDL source files
- Signal naming: lowercase with underscores, append _i for inputs, _o for outputs
- Maintain clear module interfaces with proper port declarations
- Follow existing code indentation style (2-space)
- Keep modules focused on single responsibilities
- Reuse existing components when possible
- Document interfaces and non-obvious behavior
- Maintain compatibility with hardware constraints of target FPGAs
- Respect pin assignments in .cst constraint files
- Test changes using appropriate testbenches before committing