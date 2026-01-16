# Repository Guidelines

## Project Structure & Module Organization
- boards/: Per-board projects (e.g., `a2n20v2`, `a2n9`, `a2p25`). Each contains a Gowin `.gprj`, `hdl/` (board wiring/top), `impl/pnr/` (bitstreams), and optional `src/` (firmware/ROM tools).
- hdl/: Shared HDL for all boards (SystemVerilog/Verilog/VHDL).
- releases/: Prebuilt artifacts for users.
- tests/: Targeted HDL test assets (e.g., sound VCDs) for manual inspection.

## Build, Test, and Development Commands
- Build (Gowin IDE): Open `boards/<board>/<board>.gprj`, then synthesize, PnR, and generate bitstream. Output: `boards/<board>/impl/pnr/<board>.fs`.
- Build (Command Line): Use `gw_sh` for headless builds. See `tools/README.md` for full details.
  ```bash
  # macOS path to gw_sh:
  /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh

  # Synthesis only:
  cd boards/<board>
  echo 'open_project <board>.gprj
  run syn
  exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh

  # Full build (synthesis + place & route):
  echo 'open_project <board>.gprj
  run all
  exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
  ```
- Flash (examples):
  - A2N20v2: `openfpgaloader -b tangnano20k -f boards/a2n20v2/impl/pnr/a2n20v2.fs`
  - A2N9: `openfpgaloader -b tangnano9k -f boards/a2n9/impl/pnr/a2n9.fs`
- Enhanced firmware (RISC‑V, optional): `make -C boards/a2n20v2-Enhanced/src/firmware` (updates `hdl/picosoc/firmware.hex`). Toolchain: `riscv*-unknown-elf-gcc` per Makefile auto-detection.
- Card ROM (optional): `boards/a2n20v2-Enhanced/src/cardrom/build.sh` (requires `merlin32` and Python; copies `cardrom.hex` to `hdl/cardrom/`).

## Coding Style & Naming Conventions
- HDL: 4‑space indentation, no tabs; use `logic` for SV nets; prefer explicit widths.
- Files/modules: lower_snake_case (e.g., `picosoc_sdram.sv`, `a2bus_if.sv`). Parameters/constants in UPPER_SNAKE_CASE.
- Interfaces: keep common logic in `hdl/`; per‑board `top.sv` lives under `boards/<board>/hdl/`.

## Testing Guidelines
- No global CI tests; targeted artifacts live under `tests/` (e.g., `tests/sound/...`). Provide small benches/VCDs when adding new HDL blocks.
- Verify synthesis/PnR in Gowin for impacted boards and, when possible, sanity‑check on hardware. Include reproduction steps in PRs.

## Commit & Pull Request Guidelines
- Commits: concise, imperative (“Fix HDMI clock mux”); group related changes; reference issues (`#123`) when relevant.
- PRs: describe motivation, affected boards, build steps, and tool versions. Attach logs or screenshots for visual/audio changes. Keep diffs scoped.
- Gowin note: do not add/remove files via IDE—this rewrites relative paths to absolute. Update the project by editing sources in repo, not by changing the `.gprj` file set.

## Security & Configuration Tips
- Board toggles live in `top.sv` parameters; document any default changes.
- Prebuild steps may generate `hdl/datetime.svh` via `prebuild.tcl`; avoid committing machine‑local paths.
