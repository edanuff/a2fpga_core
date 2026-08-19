// edp_encoder_defines.v — Gowin EDP Encoder IP v2.7 configuration defines.
//
// Hand-written equivalent of the IP Core Generator's emission (WS4). The
// GUI writes this file from the Options tab; the complete set of `define
// templates was recovered from libEDP_ENCODE.dylib (IDE V1.9.12.03) and
// cross-checked against IPUG1179-2.6E Table 3-6 and the bundled
// edp_encoder_top.v v2.7 template. GUI-equivalent settings:
//
//   User Clock Ratio:            1:20   -> TX_X20_MODE, SYM_WIDTH 16
//   Lanes Number:                2      -> TX_2_LANES
//   Bits Per Pixel:              24
//   Bits Per Pixel Coefficient:  1      -> 2 pixels (48 bits) per strm_clk
//   TX Line Buffer Depth:        256    (range 64/128/256; max for margin)
//   Mvid Customer:               Checked (sync clocking: exact M/N supplied,
//                                         matching the production core's
//                                         Nvid=0x80000 convention)
//   Training Pattern TPS1:       Checked
//   Training Pattern TPS2:       Checked
//   Training Pattern TPS3/TPS4:  Unchecked (HBR needs TPS1/2 only)
//   TX Debug Enable:             Unchecked
`define TX_2_LANES
`define TX_X20_MODE
`define DEF_TX_SYM_WIDTH 16
`define DEF_TX_BPP 24
`define DEF_TX_BPP_COEF 1
`define DEF_TX_FIFO_DEPTH 256
`define MVID_CUSTOMER
`define TX_TPS1_ENABLE
`define TX_TPS2_ENABLE
