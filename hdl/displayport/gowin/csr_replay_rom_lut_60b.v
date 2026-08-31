// Per-die CSR replay ROM, GW5AT-60B. Same module name as the 138B
// variant; each a2mega gprj lists exactly ONE of these — the same
// per-project selection pattern as dp_serdes.v. This module form
// replaced the shared `include after the 08-27 miswire: the include
// had silently become 138B-flavored and the 60B build replayed 138B
// CSR writes into the 60K quad (wrong lane bases/PLL config -> S:04
// PLL never locks, found on the good 60K SOM's first polite-era
// attach). A missing module now fails the build loudly.
//
// BSRAM form (08-31, PnR structural campaign): the combinational
// case-function ROM was ~1K LUTs of hard-to-place mass inside the
// transceiver bank (right in the congested GTR12 corner) and needed
// cm_life multicycle crutches on the replay_idx->drp_addr cone. Now a
// sync-read block ROM: contents from csr_replay_rom_60b.hex (generated
// alongside the .svh by boards/a2mega/tools/gen_csr_replay.py — always
// regenerate BOTH on IP regen), length still from the .svh localparam.
// The +1 read latency is absorbed by the replay FSM's inter-op gap:
// replay_idx settles >=17 drp_clk cycles before csr_rom_data is
// consumed (rd_gap[4] wait in state 0) — proven in tb_csr_rom_bsram.
module csr_replay_rom_lut(
    input  wire        clk,
    input  wire [9:0]  idx,
    output reg  [55:0] data,
    output wire [9:0]  len
);
    `include "csr_replay_rom_60b.svh"

    (* syn_romstyle = "block_rom" *) reg [55:0] rom_r [0:511];
    initial $readmemh("csr_replay_rom_60b.hex", rom_r, 0);

    always @(posedge clk) data <= rom_r[idx[8:0]];

    assign len = CSR_REPLAY_LEN[9:0];
endmodule
