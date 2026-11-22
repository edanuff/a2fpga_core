// A2MEGA - IIgs CPU socket accelerator: a trace of the socket's cycles
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// Description
// -----------
// A diagnostic, in the trace build only (gs_accel TRACE; a2mega_accel_138B_trace.gprj;
// docs/gs_socket_s7_accel_only_build.md, section 3n).  A ring of 2**AW entries in block
// RAM, one entry for a cycle the core had on the socket:
//
//   {VDA, VPA, VP, /IRQ, R/W, bank, address, data}
//
// - the cycle as the broker issued it (the address gs_accel registers for its telemetry,
// R/W, VP, VPA and VDA taken in the issue clk), and the data pads and /IRQ as the broker's
// PHI2-fall flops had them at the fall that ended it (a write's byte is the card's own,
// read back through the pad).  Local steps of fast mode are not socket cycles and are not
// here.  Nothing in this module reaches back into the design: it only reads.
//
// Every tap goes through a register that copies it in every clk before the entry's own
// register takes it, a clk after the fall (the address and the pads hold from before the
// fall until the issue, two clks after it at the earliest): two hops for the placer.
//
// Control is by the socket window's registers the firmware of the accelerator-only build
// already writes (`gs set <idx> <val>`, `gs freeze`, `gs run`, `gs trace`):
//
//   CTRL bit 3 (`gs freeze` / `gs run`; `gs trace` sets it)   the ring stands still
//   registers 31, 30, 29 with 31 = 5n: slot n is loaded from {30, 29} once the three
//   have stood still for sixteen clks.  (The accelerator's requests need 31 = A5: with a
//   slot's key there they read as none.  So the slots are for a card that is not asked
//   for card memory - mode 00 - and are left alone otherwise.)
//     slot 0  the trigger's address, low sixteen bits
//     slot 1  30 = configuration, 29 = the trigger's bank
//               bits 1:0  what is kept: 0 every cycle, 1 opcode fetches, 2 vector pulls,
//                         3 vector pulls and the opcode fetches that do not follow the
//                           one before (not 1 to 4 bytes after it in its bank)
//               bit 2     clear: the ring empty, the trigger not fired, nothing kept
//               bit 3     trigger armed: the ring stops by itself after the trigger and
//                         a number of kept entries more
//               bits 5:4  the trigger: 0 an opcode fetch at the address, 1 a vector pull
//                         with I/O and language card shadowing inhibited (shadow bit 6),
//                         2 no cycle in an I/O page ($C0xx of banks E0/E1, or of 00/01
//                         where shadowed) for 2**17 cycles, 3 any cycle at the address
//               bits 7:6  kept entries after the trigger: 32, 1024, 4096, the ring less 64
//     slot 2  29 = the page read at indices 32 to 63
//   At reset: every cycle kept, no trigger, page 0.
//
// Read: entry {page, idx_i} counted from the oldest (from entry 0 until the ring has
// gone round), as the telemetry word's "bank:addr data" and five flag bits in the
// order of the S6 trace's (R/W, RDY -> /IRQ, BE ok -> VP, VPA, VDA), so the firmware's
// `gs trace` prints lines 32 to 63 as cycles: STALL there reads "/IRQ low" and BE0
// "vector pull".  Meaningful while the ring stands still.
//
module gs_accel_trace #(
    parameter int AW = 13
) (
    input  logic        clk,
    input  logic        rst_n,

    // the socket's cycle (clk domain but for the two PHI2-fall flops)
    input  logic        run_i,           // the socket is the card's and the core is running on it
    input  logic        issue_evt_i,     // the PHY registers a cycle in this clk
    input  logic        fall_evt_i,      // a PHI2 fall: the cycle on the bus has ended
    input  logic [23:0] a_i,             // its address, registered at the issue
    input  logic        we_n_i,          // (these four: valid in the issue clk)
    input  logic        vp_n_i,
    input  logic        vpa_i,
    input  logic        vda_i,
    input  logic [7:0]  d_f_i,           // PHI2-fall flops: the data pads
    input  logic        irq_f_i,         //                  /IRQ
    input  logic        shadow6_i,       // the replica's shadow register, bit 6

    // control (clk domain, quasi-static)
    input  logic        freeze_i,
    input  logic [23:0] rq_i,            // window registers {31, 30, 29}
    input  logic [4:0]  idx_i,

    // read (clk domain)
    output logic [31:0] word_o,          // {bank, address, data}
    output logic [4:0]  flags_o,         // {VDA, VPA, VP, /IRQ, R/W}
    output logic        frozen_o,
    output logic        trigd_o,
    output logic [31:0] stat0_o,         // {frozen, triggered, gone round, 5'b0, configuration, write pointer}
    output logic [31:0] stat1_o,         // {page, trigger bank, trigger address}
    output logic [31:0] stat2_o          // entries kept since the clear
);
    localparam int EW = 37;

    // ---- the slots
    logic [23:0] rq_q;
    logic [3:0]  stab;
    logic [23:0] taddr;
    logic [7:0]  cfg;
    logic [7:0]  page;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rq_q <= '0; stab <= '0; taddr <= '0; cfg <= '0; page <= '0;
        end else begin
            rq_q <= rq_i;
            if (rq_i != rq_q)      stab <= '0;
            else if (stab != 4'hF) stab <= stab + 1'b1;
            if (stab == 4'hF && rq_q[23:20] == 4'h5) begin
                unique case (rq_q[19:16])
                    4'd0:    taddr[15:0]         <= rq_q[15:0];
                    4'd1:    {cfg, taddr[23:16]} <= rq_q[15:0];
                    4'd2:    page                <= rq_q[7:0];
                    default: ;
                endcase
            end
        end
    end
    wire [1:0] c_filt  = cfg[1:0];
    wire       c_clear = cfg[2];
    wire       c_trig  = cfg[3];
    wire [1:0] c_type  = cfg[5:4];
    wire [1:0] c_post  = cfg[7:6];

    // ---- the cycle: taps, then the entry
    logic          we_n_q, vp_n_q, vpa_q, vda_q;
    logic [EW-1:0] rec_p, rec_q, rec_w;
    logic          f1, f2, f3;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            {vda_q, vpa_q, vp_n_q, we_n_q} <= 4'b0011;
            rec_p <= '0; rec_q <= '0; f1 <= 1'b0;
        end else begin
            if (issue_evt_i) {vda_q, vpa_q, vp_n_q, we_n_q} <= {vda_i, vpa_i, vp_n_i, we_n_i};
            rec_p <= {vda_q, vpa_q, vp_n_q, irq_f_i, we_n_q, a_i, d_f_i};
            f1    <= fall_evt_i & run_i;
            if (f1) rec_q <= rec_p;
        end
    end

    // ---- what is kept, and the trigger
    wire [23:0] b_a    = rec_q[31:8];
    wire        b_pull = ~rec_q[34];
    wire        b_op   = rec_q[35] & rec_q[36];
    logic [23:0] pop;                    // the opcode fetch before this one
    logic        pop_v;
    wire [15:0] b_dif  = b_a[15:0] - pop[15:0];
    wire        b_seq  = pop_v & (b_a[23:16] == pop[23:16]) & (b_dif != 16'd0) & (b_dif <= 16'd4);
    wire        b_io   = (b_a[15:8] == 8'hC0) &
                         ((b_a[23:17] == 7'h70) | ((b_a[23:17] == 7'h00) & ~shadow6_i));
    wire        b_keep = (c_filt == 2'd0) ? 1'b1
                       : (c_filt == 2'd1) ? b_op
                       : (c_filt == 2'd2) ? b_pull
                                          : (b_pull | (b_op & ~b_seq));
    logic [17:0] quiet;                  // cycles since one in an I/O page (stops at 2**17)
    wire        b_hit  = (c_type == 2'd0) ? (b_op & (b_a == taddr))
                       : (c_type == 2'd1) ? (b_pull & shadow6_i)
                       : (c_type == 2'd2) ? (~b_io & (quiet == 18'h1FFFF))
                                          : (b_a == taddr);
    logic        keep_q, hit_q;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            f2 <= 1'b0; f3 <= 1'b0; pop <= '0; pop_v <= 1'b0; quiet <= '0;
            keep_q <= 1'b0; hit_q <= 1'b0; rec_w <= '0;
        end else begin
            f2 <= f1;
            f3 <= f2;
            if (c_clear) begin
                pop_v <= 1'b0; quiet <= '0;
            end else if (f2) begin
                if (b_op) begin pop <= b_a; pop_v <= 1'b1; end
                if (b_io)             quiet <= '0;
                else if (!quiet[17])  quiet <= quiet + 1'b1;
            end
            if (f2) begin keep_q <= b_keep; hit_q <= b_hit; rec_w <= rec_q; end
        end
    end

    // ---- the ring
    logic [AW-1:0] wptr;
    logic          wrapped, trigd, auto_frozen;
    logic [AW-1:0] post;
    logic [31:0]   count;
    wire           frozen = freeze_i | auto_frozen;
    wire           wr     = f3 & keep_q & ~frozen & ~c_clear;
    wire [AW-1:0]  post_n = (c_post == 2'd0) ? AW'(32)
                          : (c_post == 2'd1) ? AW'(1024)
                          : (c_post == 2'd2) ? AW'(4096)
                                             : AW'((1 << AW) - 64);
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wptr <= '0; wrapped <= 1'b0; trigd <= 1'b0; auto_frozen <= 1'b0; post <= '0; count <= '0;
        end else if (c_clear) begin
            wptr <= '0; wrapped <= 1'b0; trigd <= 1'b0; auto_frozen <= 1'b0; post <= '0; count <= '0;
        end else begin
            if (!c_trig) begin
                trigd <= 1'b0; auto_frozen <= 1'b0;
            end else if (f3 && !frozen) begin
                if (!trigd) begin
                    if (hit_q) begin trigd <= 1'b1; post <= post_n; end
                end else if (keep_q) begin
                    if (post == '0) auto_frozen <= 1'b1;
                    else            post <= post - 1'b1;
                end
            end
            if (wr) begin
                wptr  <= wptr + 1'b1;
                count <= count + 1'b1;
                if (&wptr) wrapped <= 1'b1;
            end
        end
    end

    logic [EW-1:0] mem [0:(1<<AW)-1] /* synthesis syn_ramstyle = "block_ram" */;
`ifdef VERILATOR
    initial for (int i = 0; i < (1 << AW); i++) mem[i] = '0;
`endif
    logic [AW-1:0] ra;
    logic [EW-1:0] rd;
    always_ff @(posedge clk) begin
        if (wr) mem[wptr] <= rec_w;
        ra <= (wrapped ? wptr : '0) + AW'({page, idx_i});
        rd <= mem[ra];
    end

    assign word_o   = rd[31:0];
    assign flags_o  = rd[36:32];
    assign frozen_o = frozen;
    assign trigd_o  = trigd;
    assign stat0_o  = {frozen, trigd, wrapped, 5'b0, cfg, 16'(wptr)};
    assign stat1_o  = {page, taddr};
    assign stat2_o  = count;
endmodule
