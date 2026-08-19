// top_define.v — Gowin EDP Encoder IP instance-name define.
//
// Hand-written equivalent of the IP Core Generator's emission (WS4: the
// generator GUI writes exactly `define module_name <ModuleName>`; template
// string recovered from libEDP_ENCODE.dylib). Module name kept at the
// GUI default so a later GUI regeneration is a drop-in replacement.
`define module_name EDP_Encoder_Top
