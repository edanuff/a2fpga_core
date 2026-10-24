// a2n20v2_gs_profile.vh — build profile for boards/a2n20v2-GS (included by top.sv).
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
`ifndef A2N20V2_GS_PROFILE_VH
`define A2N20V2_GS_PROFILE_VH

// ---- features ----
`define A2_FEATURE_SUPERSPRITE    1
`define A2_FEATURE_MOCKINGBOARD   1
`define A2_FEATURE_SUPERSERIAL    1
`define A2_FEATURE_ENSONIQ        1

// ---- instrumentation ----
`define A2_INSTR_DEBUG_OVERLAY    1

`ifdef A2_PROFILE_RELEASE
// Release overrides go here (undef + redefine). Empty: release == dev today.
`endif
`endif
