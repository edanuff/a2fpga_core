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
    // GW5AT-60B: M5 AFE adjust ON (08-28) — lane bases DUAL-SOURCE
    // VERIFIED to the 138B standard: (1) boot-emission generational diff
    // in dp_serdes.csr (F000/0800/0110 block at 8084xx/8085xx), and
    // (2) IDE Reconfiguration-dialog exports txafe_q0l23_*.csr — both
    // agree exactly (incl. the known boot-vs-dialog txlev-13 E000/D000
    // off-by-one reproducing on this die). Bases = die lanes 2 (ML0)
    // and 3 (ML1), stride 0x100, offsets +0x34/+0x38/+0xd8 identical
    // to the 138B M5 model (m5_runtime_afe.md).
    localparam int          ENABLE_AFE_ADJUST = 1;
    localparam logic [23:0] AFE_LANE_BASE0    = 24'h808400;
    localparam logic [23:0] AFE_LANE_BASE1    = 24'h808500;
endpackage
