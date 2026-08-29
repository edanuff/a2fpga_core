///////////////////////////////////////////////////////////////////////////////
// dp_test_die_pkg — per-SOM-die feature/address constants for the shared
// a2mega_dp_test_top.sv. ONE of these files is in each dp_test project:
//   hdl/dp_test/dp_test_die_pkg_60b.sv        -> a2mega_dp_test.gprj (GW5AT-60B)
//   hdl/dp_test_138b/dp_test_die_pkg_138b.sv  -> a2mega_dp_test_138b.gprj (GW5AST-138B)
// Same package name, die-specific contents; the top reads
// dp_test_die_pkg::* so it never hardcodes one die's feature set.
//
// Why: the M5 AFE sequencer writes die-specific SERDES DRP addresses. The
// 138B lane bases are verified (m5_runtime_afe.md); the 60B's are NOT, so
// the feature must be OFF on the 60B until its DRP export is verified —
// a shared top enabling it with 138B addresses would write unverified
// registers on the 60B die (user review item 1, 08-21).
///////////////////////////////////////////////////////////////////////////////
package dp_test_die_pkg;
    // GW5AT-60B (SOM2 / board B1): M5 AFE adjust OFF — DRP lane bases for
    // this die are unverified. Bases below are placeholders (unused while
    // ENABLE_AFE_ADJUST = 0); fill from a verified 60B .csr before enabling.
    localparam int          ENABLE_AFE_ADJUST = 0;
    localparam logic [23:0] AFE_LANE_BASE0    = 24'h000000;
    localparam logic [23:0] AFE_LANE_BASE1    = 24'h000000;
endpackage
