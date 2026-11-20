///////////////////////////////////////////////////////////////////////////////
// dp_test_die_pkg — per-SOM-die feature/address constants for the shared
// a2mega_dp_test_top.sv. ONE of these files is in each dp_test project:
//   hdl/dp_test/dp_test_die_pkg_60b.sv        -> a2mega_dp_test.gprj (GW5AT-60B)
//   hdl/dp_test_138b/dp_test_die_pkg_138b.sv  -> a2mega_dp_test_138b.gprj (GW5AST-138B)
// Same package name, die-specific contents; the top reads
// dp_test_die_pkg::* so it never hardcodes one die's feature set.
//
// Why: the M5 AFE sequencer writes die-specific SERDES DRP addresses, in
// LOGICAL lane order. BOTH dies are dual-source verified (boot-emission
// diff + IDE dialog exports; m5_runtime_afe.md): 60B ML0 -> die lane 3
// (0x8085xx), ML1 -> die lane 2 (0x8084xx); 138B ML0 -> die lane 1
// (0x8083xx), ML1 -> die lane 2 (0x8084xx). The 08-21 "60B unverified,
// keep OFF" rule is SUPERSEDED (08-28 verification + review).
///////////////////////////////////////////////////////////////////////////////
package dp_test_die_pkg;
    // GW5AST-138B (SOM / board B3): M5 AFE adjust ON (rows 75-78); lane
    // bases = die lanes 1 (ML0) and 2 (ML1), verified two ways 08-21.
    localparam int          ENABLE_AFE_ADJUST = 1;
    localparam logic [23:0] AFE_LANE_BASE0    = 24'h808300;
    localparam logic [23:0] AFE_LANE_BASE1    = 24'h808400;

    // FULL-CORE FEATURE SET: the 138K die has room (CLS 46%) — full set
    localparam bit FEATURE_SUPERSPRITE  = 1'b1;
    localparam bit FEATURE_MOCKINGBOARD = 1'b1;
    localparam bit FEATURE_SUPERSERIAL  = 1'b1;
endpackage
