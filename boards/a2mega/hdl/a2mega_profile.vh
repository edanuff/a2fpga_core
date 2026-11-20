// a2mega_profile.vh — build profile for boards/a2mega (included by top.sv).
//
// ONE place for the switches that decide what this board's core carries:
// features (slot cards, audio, video options) and instrumentation (overlays,
// event logs, telemetry). The top's parameter defaults and instrument
// instances read these macros; nothing else should set them. Values below are
// the "dev" profile — exactly what this board built before this file existed.
//
// Release profile (planned): define A2_PROFILE_RELEASE (a *_defines.v listed
// first in a release .gprj, or a build.sh PROFILE=release switch once it can
// inject one) and fill the `ifdef A2_PROFILE_RELEASE block below with what
// release drops. Until then that block is empty on purpose. See
// docs/build_profiles.md for the conventions and the per-module cost table.
`ifndef A2MEGA_PROFILE_VH
`define A2MEGA_PROFILE_VH

// ---- features (slot cards; the per-die ones live in dp_test_die_pkg) ----
`define A2_FEATURE_DISK_II        1
`define A2_FEATURE_UTHERNET2      1
`define A2_FEATURE_HDD            1
`define A2_FEATURE_ENSONIQ        1
// SUPERSPRITE / MOCKINGBOARD / SUPERSERIAL: dp_test_die_pkg::FEATURE_* (60B vs 138B)

// ---- instrumentation ----
`define A2_INSTR_DEBUG_OVERLAY    1   // DebugOverlay in the clk_pix chain (OUT_PIPE stage; see framebuffer H_TRIGGER_LEAD before removing)
`ifdef GS_SOCKET
`define A2_INSTR_EVENT_LOG        1   // socket bring-up event log (window regs 32-39; 2 BSRAM on the 138B)
`else
`define A2_INSTR_EVENT_LOG        0
`endif

`ifdef A2_PROFILE_RELEASE
// Release overrides go here (undef + redefine). Empty: release == dev today.
`endif
`endif
