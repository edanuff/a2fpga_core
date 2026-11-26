# Get current datetime in YYYYMMDDhhmmss format. Some gw_sh builds (e.g. the
# Linux Education 1.9.11.03 used in CI) ship a Tcl without the `clock`
# command; fall back to `exec date`, then a BUILD_DATETIME env var, then zeros.
if {[catch {clock format [clock seconds] -format "%Y%m%d%H%M%S"} now]} {
    if {[catch {string trim [exec date +%Y%m%d%H%M%S]} now]} {
        if {[info exists ::env(BUILD_DATETIME)]} {
            set now $::env(BUILD_DATETIME)
        } else {
            set now "00000000000000"
        }
    }
}

# Output to a Verilog header file
set out_file "hdl/datetime.svh"
set fp [open $out_file "w"]
puts $fp "// Generated build timestamp"
puts $fp "`define BUILD_DATETIME \"$now\""
close $fp
