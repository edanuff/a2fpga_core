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

// a2-mega carrier: die lanes 3+2 drive ML0/ML1 (board remap)
`define DP_SERDES_LANES_23
