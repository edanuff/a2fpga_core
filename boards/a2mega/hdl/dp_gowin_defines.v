// Build-configuration defines for the Gowin flow. This file must be the
// FIRST entry in the .gprj file list so the defines apply to the whole
// compilation unit.
//
// GOWIN_SERDES_IP / GOWIN_PLL_IP are intentionally NOT defined yet: the
// timing-viability build uses the behavioural clock stubs (clocks come
// from input pins, constrained in the .sdc). Define them once the SerDes
// and PLL IP have been generated in the IP Core Generator.
`define DP_VENDOR_GOWIN
`define GOWIN_SERDES_IP
`define GOWIN_PLL_IP

// DP_SERDES_LANES_23 retired with the EDP PHY adoption: the die-lane
// 3+2 -> ML0/ML1 board remap is now hardcoded in transceiver_bank_gowin
// (logical ln1/ln0 of the 2-lane EDP PHY on Q0 lanes 2+3).
