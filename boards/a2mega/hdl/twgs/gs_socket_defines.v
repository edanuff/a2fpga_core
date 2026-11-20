// Enables the IIgs CPU-socket 65C816 in top.sv.  Listed in the 138B
// project (a2mega_138B.gprj) ahead of top.sv, like dp_gowin_defines.v;
// the 60K project does not list it, so the socket logic and its 38 pins
// stay out of that build (docs/gs_socket_65816_scoping.md: 138B only for
// the first iteration).
`define GS_SOCKET
