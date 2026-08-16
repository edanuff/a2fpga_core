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

// a2-mega carrier lane selection (board remap; see transceiver_bank_gowin):
//   DP_SERDES_LANES_4  = 4-lane RBR, die ln3/2/1/0 <- ML0/1/2/3
//   DP_SERDES_LANES_23 = 2-lane HBR, die ln3/2     <- ML0/ML1
// Only one may be defined. 4-lane RBR is the wider-UI configuration under
// evaluation against the ~20% boot marginality; tag
// a2mega-2lane-planc-4of5 is the 2-lane revert point.
`define DP_SERDES_LANES_4
