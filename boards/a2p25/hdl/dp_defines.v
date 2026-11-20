// Build-configuration defines for the a2p25 core. Must be the FIRST entry in
// the .gprj file list so the defines apply to the whole compilation unit.
//
// DP_VENDOR_DPHY: dp_transmitter exports the symbol stream to the board's
// transceiver_bank_dphy (GW5A-25 hardened MIPI D-PHY as a raw serializer).
// GOWIN_PLL_IP:   the pixel clock comes from gowin_pixel_pll, fed by the
// 81 MHz symbol clock (DP synchronous clocking).
`define DP_VENDOR_DPHY
`define GOWIN_PLL_IP
