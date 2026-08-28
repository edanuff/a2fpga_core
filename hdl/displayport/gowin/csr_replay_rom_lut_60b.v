// Per-die CSR replay ROM, GW5AT-60B. Same module name as the 138B
// variant; each a2mega gprj lists exactly ONE of these — the same
// per-project selection pattern as dp_serdes.v. This module form
// replaced the shared `include after the 08-27 miswire: the include
// had silently become 138B-flavored and the 60B build replayed 138B
// CSR writes into the 60K quad (wrong lane bases/PLL config -> S:04
// PLL never locks, found on the good 60K SOM's first polite-era
// attach). A missing module now fails the build loudly.
module csr_replay_rom_lut(
    input  wire [9:0]  idx,
    output wire [55:0] data,
    output wire [9:0]  len
);
    `include "csr_replay_rom_60b.svh"
    assign data = csr_replay_rom(idx);
    assign len  = CSR_REPLAY_LEN[9:0];
endmodule
