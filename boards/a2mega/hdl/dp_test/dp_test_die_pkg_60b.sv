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
    // GW5AT-60B: M5 AFE adjust ON (08-28) — lane bases DUAL-SOURCE
    // VERIFIED to the 138B standard: (1) boot-emission generational diff
    // in dp_serdes.csr (F000/0800/0110 block at 8084xx/8085xx), and
    // (2) IDE Reconfiguration-dialog exports txafe_q0l23_*.csr — both
    // agree exactly (incl. the known boot-vs-dialog txlev-13 E000/D000
    // off-by-one reproducing on this die). Bases = die lanes 2 (ML0)
    // and 3 (ML1), stride 0x100, offsets +0x34/+0x38/+0xd8 identical
    // to the 138B M5 model (m5_runtime_afe.md).
    // LANE ORDERING (08-28 review correction): the bank's LANES_23 shim
    // maps fabric word lane 0 (ML0) -> die lane 3 and ML1 -> die lane 2
    // (transceiver_bank_gowin.v, board routing: DP0=ln3, DP1=ln2). The
    // AFE bases are LOGICAL-lane-indexed, so BASE0 = die lane 3's block.
    // (First cut had them reversed — harmless for symmetric requests
    // like A:0022, wrong lane for asymmetric ones. Board-level
    // asymmetric-request verification still pending.)
    localparam int          ENABLE_AFE_ADJUST = 1;
    localparam logic [23:0] AFE_LANE_BASE0    = 24'h808500;  // ML0 -> die lane 3
    localparam logic [23:0] AFE_LANE_BASE1    = 24'h808400;  // ML1 -> die lane 2

    // FULL-CORE FEATURE SET (ed, 08-30): the 60K die is capacity-bound
    // (CLS 92%, BSRAM 92% with everything on — timing closure was a
    // per-draw lottery). Supersprite/Mockingboard/SuperSerial are OFF on
    // the 60K full core to buy placement freedom; the 138K carries the
    // full feature set (CLS 46%).
    localparam bit FEATURE_SUPERSPRITE  = 1'b0;
    localparam bit FEATURE_MOCKINGBOARD = 1'b0;
    localparam bit FEATURE_SUPERSERIAL  = 1'b0;
endpackage
