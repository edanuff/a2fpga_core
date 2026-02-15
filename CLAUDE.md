# Claude Code Instructions for a2fpga_core

## FPGA Build Instructions (MUST FOLLOW)

The Gowin FPGA toolchain is used for synthesis and place & route. The command
line tool is `gw_sh` and its path on this Mac is:

```
/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

### Full Build (Synthesis + Place & Route)

```bash
cd /Users/edanuff/GitHub/a2fpga_core/boards/a2mega
echo 'open_project a2mega.gprj
run all
exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

### Synthesis Only

```bash
cd /Users/edanuff/GitHub/a2fpga_core/boards/a2mega
echo 'open_project a2mega.gprj
run syn
exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

### Post-Build Timing Check

Always check for timing violations after a build:

```bash
grep -E "Violated|Fmax" boards/a2mega/impl/pnr/a2mega_tr_content.html | head -20
```

A healthy build has:
- Zero violated endpoints (setup and hold)
- Actual Fmax >= Constraint for all clocks
- TNS of 0.000 for all clock domains

### Key Output Files

- `impl/pnr/a2mega.fs` - Bitstream for programming
- `impl/pnr/a2mega.rpt.txt` - Resource usage summary
- `impl/pnr/a2mega_tr_content.html` - Timing report
- `impl/gwsynthesis/a2mega.log` - Synthesis log with warnings/errors

### Important Notes

- NEVER use `gw_sh` with `-exit -e` flags or try to run it from `~/bin/`. Use
  the pipe method shown above with the full application path.
- The project's `prebuild.tcl` runs automatically when opening the project,
  updating `hdl/datetime.svh` with the build timestamp.
- Warnings about OpenGL/Chromium on macOS are safe to ignore.
- See `tools/README.md` for additional documentation and platform-specific paths.

## Project Structure

- `boards/a2mega/` - a2mega board (Gowin GW5AT-LV60PG484AC1/I0)
- `boards/a2mega/hdl/` - Board-specific HDL (top.sv, bus/, video/)
- `hdl/` - Shared HDL modules (bus interfaces, debug overlay, etc.)
- `tools/` - Build tools and documentation

## Current Branch

Work is on the `ddr3_framebuffer` branch.
