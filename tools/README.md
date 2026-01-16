# Gowin FPGA Tools

## Documentation

- Gowin Software EDA IDE User Guide: https://cdn.gowinsemi.com.cn/SUG100E.pdf
- Gowin Software Tcl Commands User Guide: https://cdn.gowinsemi.com.cn/SUG1220E.pdf
- Gowin Software Quick Start Guide: https://cdn.gowinsemi.com.cn/SUG918E.pdf

## Command Line Synthesis with gw_sh

The Gowin EDA includes a Tcl-based command line tool called `gw_sh` that can be used to run synthesis and place & route without opening the GUI.

### Location

**macOS:**
```
/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

**Windows:**
```
C:\Gowin\Gowin_V1.9.9\IDE\bin\gw_sh.exe
```

**Linux:**
```
/opt/gowin/IDE/bin/gw_sh
```

### macOS Setup

On macOS, the following script must be applied prior to installation of the Gowin IDE in order for the command line tools to execute correctly:
```
gowin_eda_mac_fixer.sh
```

### Running Synthesis

To run synthesis only:
```bash
cd boards/<board>
echo 'open_project <board>.gprj
run syn
exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

Example for a2mega:
```bash
cd boards/a2mega
echo 'open_project a2mega.gprj
run syn
exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

### Running Full Build (Synthesis + Place & Route)

To run the complete build flow including bitstream generation:
```bash
cd boards/<board>
echo 'open_project <board>.gprj
run all
exit' | /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh
```

### Key Tcl Commands

| Command | Description |
|---------|-------------|
| `open_project <file.gprj>` | Opens a Gowin project file |
| `run syn` | Runs synthesis only |
| `run pnr` | Runs place & route only (requires prior synthesis) |
| `run all` | Runs full flow: synthesis + place & route |
| `exit` | Exits gw_sh |

### Output Files

After synthesis:
- `impl/gwsynthesis/<board>.vg` - Synthesized netlist
- `impl/gwsynthesis/<board>_syn.rpt.html` - Synthesis report

After place & route:
- `impl/pnr/<board>.fs` - Bitstream file for programming
- `impl/pnr/<board>.rpt.html` - Place & route report
- `impl/pnr/<board>.tr.html` - Timing report

### Using a Tcl Script

For more complex builds, create a Tcl script file:
```tcl
# build.tcl
open_project a2mega.gprj
run all
exit
```

Then run it:
```bash
/Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/bin/gw_sh build.tcl
```

### Notes

- The project's `prebuild.tcl` script (if present) runs automatically when opening the project, generating files like `hdl/datetime.svh` with build timestamps.
- Warnings about OpenGL/Chromium on macOS can be safely ignored for command line builds.
- For the full Tcl command reference, see the Gowin Software Tcl Commands User Guide (SUG1220E.pdf).



