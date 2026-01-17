# Repository Guidelines

## Starting a New Session

At the start of each session, check the `TODO.md` file in the relevant board directory (`boards/<board>/TODO.md`) to:
- Understand current priorities and known issues
- See what tasks are in progress or blocked
- Find context about the board's status (deprecated, WIP, stable)

Active boards:
- `boards/a2n20v2/` - Stable production version (Tang Nano 20K, supports IIgs)
- `boards/a2n20v2-Enhanced/` - Experimental with PicoSoC/SDRAM/SD Card
- `boards/a2mega/` - Work in progress (Tang Mega 60K)
- `boards/a2p25/` - Work in progress (Tang Primer 25K)

Deprecated boards (reference only):
- `boards/a2n20v1/` - Superseded by a2n20v2
- `boards/a2n9/` - Limited resources, planned for retirement

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
- Build Output Files: After builds, review these files in `boards/<board>/impl/`:
  - `gwsynthesis/<board>.log` - Synthesis log (warnings/errors)
  - `pnr/<board>.rpt.txt` - **Key report**: resource usage, I/O banks, clocks, pinout
  - `pnr/<board>.log` - Place & route log
  - `pnr/<board>.power.html` - Power analysis
  - `pnr/<board>_tr_content.html` - **Timing report** (critical to check!)
- **Timing Verification (Required after P&R)**: Always check `pnr/<board>_tr_content.html` for timing violations:
  - Look for `Numbers of Setup Violated Endpoints` and `Numbers of Hold Violated Endpoints` - both must be **0**
  - Check `Max Frequency Summary` - Actual Fmax must be >= Constraint for all clocks
  - Check `Total Negative Slack` - TNS must be **0.000** for all clocks
  - Report any timing violations to the user before considering a build successful
- Flash (examples):
  - A2N20v2: `openfpgaloader -b tangnano20k -f boards/a2n20v2/impl/pnr/a2n20v2.fs`
  - A2N9: `openfpgaloader -b tangnano9k -f boards/a2n9/impl/pnr/a2n9.fs`
- Enhanced firmware (RISC‑V, optional): `make -C boards/a2n20v2-Enhanced/src/firmware` (updates `hdl/picosoc/firmware.hex`). Toolchain: `riscv*-unknown-elf-gcc` per Makefile auto-detection.
- Card ROM (optional): `boards/a2n20v2-Enhanced/src/cardrom/build.sh` (requires `merlin32` and Python; copies `cardrom.hex` to `hdl/cardrom/`).

## Coding Style & Naming Conventions
- **SystemVerilog is required** for all new modules. Use `.sv` extension (not `.v`).
- HDL: 4‑space indentation, no tabs; use `logic` for SV nets; prefer explicit widths.
- Files/modules: lower_snake_case (e.g., `picosoc_sdram.sv`, `a2bus_if.sv`). Parameters/constants in UPPER_SNAKE_CASE.
- Interfaces: keep common logic in `hdl/`; per‑board `top.sv` lives under `boards/<board>/hdl/`.

## Adding New Files to a Project
- New HDL files **must be added** to the appropriate `.gprj` file for the board(s) that use them.
- Paths in `.gprj` files are **relative to the `.gprj` file location** (e.g., `hdl/top.sv` or `../../hdl/sound/audio_out.v`).
- **Never use absolute paths** in `.gprj` files. The Gowin IDE will insert absolute paths when adding files via the GUI - these must be converted to relative paths before committing.
- Shared modules go in `hdl/` and are referenced from board projects as `../../hdl/<path>`.
- Board-specific modules go in `boards/<board>/hdl/` and are referenced as `hdl/<filename>`.

## Testing Guidelines
- No global CI tests; targeted artifacts live under `tests/` (e.g., `tests/sound/...`). Provide small benches/VCDs when adding new HDL blocks.
- Verify synthesis/PnR in Gowin for impacted boards and, when possible, sanity‑check on hardware. Include reproduction steps in PRs.

## Commit & Pull Request Guidelines
- Commits: concise, imperative ("Fix HDMI clock mux"); group related changes; reference issues (`#123`) when relevant.
- PRs: describe motivation, affected boards, build steps, and tool versions. Attach logs or screenshots for visual/audio changes. Keep diffs scoped.
- Before committing `.gprj` changes, verify all paths are relative (see "Adding New Files to a Project" above).

## Security & Configuration Tips
- Board toggles live in `top.sv` parameters; document any default changes.
- Prebuild steps may generate `hdl/datetime.svh` via `prebuild.tcl`; avoid committing machine‑local paths.
