// edp_encoder_defines.v — Gowin EDP Encoder IP v2.7 configuration defines.
//
// Hand-written equivalent of the IP Core Generator's emission (WS4). The
// GUI writes this file from the Options tab; the complete set of `define
// templates was recovered from libEDP_ENCODE.dylib (IDE V1.9.12.03) and
// cross-checked against IPUG1179-2.6E Table 3-6 and the bundled
// edp_encoder_top.v v2.7 template.
//
// GEOMETRY MIGRATION 2026-08-19 (WS4 forensics §10): moved from 1:20/X20
// (SYM_WIDTH 16, coef 1) to the reference design's 1:40/X40 geometry
// (SYM_WIDTH 32, coef 2) after the repaired hardened-8b10b CSR at 1:1
// gear produced a broken 60.75 MHz fabric clock on silicon — Gowin ships
// hardened 8b10b ONLY at 1:2 gear (all five EDP refdesign variants; cf.
// Gowin_EDP_RefDesign temp/EDP_Encoder/edp_encoder_defines.v: TX_X40_MODE,
// SYM_WIDTH 32, BPP_COEF 2). GUI-equivalent settings now:
//
//   User Clock Ratio:            1:40   -> TX_X40_MODE, SYM_WIDTH 32
//                                          (ls_clk = 67.5 MHz at 2.7 G)
//   Lanes Number:                2      -> TX_2_LANES
//   Bits Per Pixel:              24
//   Bits Per Pixel Coefficient:  2      -> 2 pixels (48 bits) per pix_data
//                                          port = 4 px per strm_clk
//   TX Line Buffer Depth:        256    (range 64/128/256; max for margin)
//   Mvid Customer:               Checked (sync clocking: exact M/N supplied,
//                                         matching the production core's
//                                         Nvid=0x80000 convention)
//   Training Pattern TPS1:       Checked
//   Training Pattern TPS2:       Checked
//   Training Pattern TPS3/TPS4:  Unchecked (HBR needs TPS1/2 only)
//   TX Debug Enable:             Unchecked
`define TX_2_LANES
`define TX_X40_MODE
`define DEF_TX_SYM_WIDTH 32
`define DEF_TX_BPP 24
`define DEF_TX_BPP_COEF 2
`define DEF_TX_FIFO_DEPTH 256
`define MVID_CUSTOMER
`define TX_TPS1_ENABLE
`define TX_TPS2_ENABLE
