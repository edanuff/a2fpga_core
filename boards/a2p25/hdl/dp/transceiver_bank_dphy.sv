// transceiver_bank_dphy.sv - DP main-link PHY for the a2p25: GW5A-25
// hardened MIPI D-PHY TX used as a raw 1.62 Gbps serializer.
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
// Description:
//
// Plays the role transceiver_bank_gowin plays on a2mega (dp_transmitter's
// DP_VENDOR_DPHY branch exports the symbol stream; the board top hosts
// this module). Design: boards/a2p25/docs/DP_ALTMODE_DPHY_DESIGN.md.
//
//   tx_symbols (81 MHz, 4 lanes x {force1,K1,d1,force0,K0,d0})
//     -> lane_encoder_8b10b x4 (81 MHz)
//     -> async FIFO 81 MHz -> 101.25 MHz (same VCO, fixed ratio 4:5)
//     -> lane reversal (board routing)
//     -> dp_dphy_tx (10 -> 16 gearbox, continuous HS)
//     -> Gowin_MIPI_DPHY (16-bit mode, 1620 Mbps, CLKIN 54 MHz, VCO 810)
//
// Clocks: the D-PHY IP's own PLL (VCO 810 MHz) makes the 4 serializer
// phases on clkout0-3 and, on the spare clkout4 (ODIV4 = 10), the 81 MHz
// link-symbol clock returned as tx_symbol_clk. tx_clk_o (101.25 MHz) is
// generated inside the D-PHY. Both come from one VCO, so the FIFO only
// absorbs phase and the 4-of-5 pull pattern; its level is constant in
// steady state.
//
// FIFO start-up: the writer pushes every symbol-clock cycle (never drops
// once streaming). Before dp_dphy_tx starts pulling, the reader discards
// entries to hold the level at half, so the first pull finds ~8 entries
// and the steady-state level never reaches full or empty. gb_underflow /
// fifo_overflow are sticky diagnostics for the telemetry line.
//
// Lane map: on the a2-p25 board DP lane n is routed from D-PHY data lane
// (3 - n) (M0_D3 -> DP130 IN0 -> DP0, ..., M0_D0 -> DP3); polarity is
// straight. REVERSE_LANES = 1 encodes that. All four D-PHY lanes always
// run (unused DP lanes carry whatever the link layer emits there); a
// 2-lane link uses DP lanes 0/1 = D-PHY lanes 3/2.
//
// PROBE = 1: replaces the symbol stream with a raw 16-pairs-high /
// 16-pairs-low pattern (1.62 Gbps / 640 = 2.53 MHz square wave) on every
// lane, link-policy independent - answers "are the lanes alive, on which
// pins, which polarity" with a basic scope, no DP sink needed.
//

module transceiver_bank_dphy #(
    parameter REVERSE_LANES     = 1,
    parameter PROBE             = 0,
    parameter DRV_SETTLE_CYCLES = 20'd10125,   // ~100 us @ 101.25 MHz
    parameter HS_SETTLE_CYCLES  = 20'd10125
) (
    input  wire        mgmt_clk,           // 100 MHz (link policy domain)
    input  wire        clk_dphy_ref,       // 54 MHz D-PHY PLL reference
    input  wire        clk_md,             // 50 MHz PLL_INIT / DRP clock
    input  wire        reset,              // async, active high

    input  wire [3:0]  powerup_channel,    // mgmt_clk domain
    output wire [3:0]  tx_running,         // mgmt_clk domain
    output wire        tx_symbol_clk,      // 81 MHz
    input  wire [79:0] tx_symbols,         // tx_symbol_clk domain

    // Status (quasi-static; sample with a synchronizer)
    output wire [7:0]  status,             // {drst_n, ovf, unf, phy_active, link_en, enc_run, tx_clk_alive, 0}
    output wire        tx_clk_out,         // 101.25 MHz, diagnostics

    // D-PHY pads (M0 quad)
    inout  wire        dphy_ck_p,
    inout  wire        dphy_ck_n,
    inout  wire [3:0]  dphy_d_p,
    inout  wire [3:0]  dphy_d_n
);

    // ------------------------------------------------------------------
    // D-PHY IP (PLL + serializer)
    // ------------------------------------------------------------------
    wire        tx_clk;
    wire        clk_sym;
    wire        shim_drst_n, txhclk_en, hstxd_vld;
    reg         dphy_drst_n = 1'b0;     // D-PHY TX datapath reset (see below)
    wire [15:0] d0ln, d1ln, d2ln, d3ln;
    wire [3:0]  hstxen, txdpen, lptxen, do_lptx_n, do_lptx_p, lprx_en;

    assign tx_symbol_clk = clk_sym;
    assign tx_clk_out    = tx_clk;

    Gowin_MIPI_DPHY u_dphy (
        .ck_n        (dphy_ck_n),
        .ck_p        (dphy_ck_p),
        .d0_n        (dphy_d_n[0]),
        .d0_p        (dphy_d_p[0]),
        .d1_n        (dphy_d_n[1]),
        .d1_p        (dphy_d_p[1]),
        .d2_n        (dphy_d_n[2]),
        .d2_p        (dphy_d_p[2]),
        .d3_n        (dphy_d_n[3]),
        .d3_p        (dphy_d_p[3]),
        .clkin       (clk_dphy_ref),
        .mdclk       (clk_md),
        .clkout0     (),
        .clkout1     (),
        .clkout2     (),
        .clkout3     (),
        .clkout4     (clk_sym),
        .tx_drst_n   (dphy_drst_n),
        .tx_clk_o    (tx_clk),
        .txhclk_en   (txhclk_en),
        .hstxd_vld   (hstxd_vld),
        .hstxen_ln0  (hstxen[0]),
        .txdpen_ln0  (txdpen[0]),
        .d0ln_hstxd  (d0ln),
        .lprx_en_d0  (lprx_en[0]),
        .di_lprx0_n  (),
        .di_lprx0_p  (),
        .lptxen_ln0  (lptxen[0]),
        .do_lptx0_n  (do_lptx_n[0]),
        .do_lptx0_p  (do_lptx_p[0]),
        .hstxen_ln1  (hstxen[1]),
        .txdpen_ln1  (txdpen[1]),
        .d1ln_hstxd  (d1ln),
        .lprx_en_d1  (lprx_en[1]),
        .di_lprx1_n  (),
        .di_lprx1_p  (),
        .lptxen_ln1  (lptxen[1]),
        .do_lptx1_n  (do_lptx_n[1]),
        .do_lptx1_p  (do_lptx_p[1]),
        .hstxen_ln2  (hstxen[2]),
        .txdpen_ln2  (txdpen[2]),
        .d2ln_hstxd  (d2ln),
        .lprx_en_d2  (lprx_en[2]),
        .di_lprx2_n  (),
        .di_lprx2_p  (),
        .lptxen_ln2  (lptxen[2]),
        .do_lptx2_n  (do_lptx_n[2]),
        .do_lptx2_p  (do_lptx_p[2]),
        .hstxen_ln3  (hstxen[3]),
        .txdpen_ln3  (txdpen[3]),
        .d3ln_hstxd  (d3ln),
        .lprx_en_d3  (lprx_en[3]),
        .di_lprx3_n  (),
        .di_lprx3_p  (),
        .lptxen_ln3  (lptxen[3]),
        .do_lptx3_n  (do_lptx_n[3]),
        .do_lptx3_p  (do_lptx_p[3])
    );

    // ------------------------------------------------------------------
    // Link enable: any lane powered by the link policy (or PROBE)
    // ------------------------------------------------------------------
    wire link_req_mgmt = PROBE ? 1'b1 : (powerup_channel != 4'b0000);

    reg [2:0] link_sync_sym = 3'b000;
    reg [2:0] link_sync_tx  = 3'b000;
    always @(posedge clk_sym) link_sync_sym <= {link_sync_sym[1:0], link_req_mgmt};
    always @(posedge tx_clk)  link_sync_tx  <= {link_sync_tx[1:0],  link_req_mgmt};
    wire link_en_sym = link_sync_sym[2];
    wire link_en_tx  = link_sync_tx[2];

    // ------------------------------------------------------------------
    // Reset sequencing in each domain (PLL lock is not exported by the
    // generated wrapper; run on settle counters)
    // ------------------------------------------------------------------
    reg [2:0]  rst_sym_s = 3'b111;
    reg [2:0]  rst_tx_s  = 3'b111;
    reg [15:0] sym_settle = 16'd0;
    reg        enc_run = 1'b0;
    always @(posedge clk_sym or posedge reset) begin
        if (reset) begin
            rst_sym_s  <= 3'b111;
            sym_settle <= 16'd0;
            enc_run    <= 1'b0;
        end else begin
            rst_sym_s <= {rst_sym_s[1:0], 1'b0};
            if (!rst_sym_s[2]) begin
                if (sym_settle != 16'hFFFF)
                    sym_settle <= sym_settle + 16'd1;
                else
                    enc_run <= 1'b1;
            end
        end
    end
    // The D-PHY TX datapath reset is released from the PLL-derived symbol
    // clock, NOT from tx_clk: TX_CLK_O is produced by the hard PHY and may
    // be held while TX_DRST_N is low, so a release sequenced on tx_clk
    // could deadlock. Release once the PLL has run ~0.4 ms (sym_settle
    // MSB), after which tx_clk is expected to run.
    always @(posedge clk_sym or posedge reset) begin
        if (reset) dphy_drst_n <= 1'b0;
        else if (sym_settle[15]) dphy_drst_n <= 1'b1;
    end

    always @(posedge tx_clk or posedge reset) begin
        if (reset) rst_tx_s <= 3'b111;
        else       rst_tx_s <= {rst_tx_s[1:0], !dphy_drst_n};
    end
    wire rst_tx_n = !rst_tx_s[2];

    // ------------------------------------------------------------------
    // 8b/10b (81 MHz)
    // ------------------------------------------------------------------
    wire [19:0] code [0:3];
    genvar gi;
    generate
        for (gi = 0; gi < 4; gi = gi + 1) begin : g_enc
            lane_encoder_8b10b u_enc (
                .clk       (clk_sym),
                .reset     (!enc_run),
                .tx_symbol (tx_symbols[gi*20 +: 20]),
                .tx_code   (code[gi])
            );
        end
    endgenerate

    // PROBE pattern: 16 pairs of all-ones then 16 of all-zeros
    reg [4:0] probe_cnt = 5'd0;
    always @(posedge clk_sym) probe_cnt <= probe_cnt + 5'd1;
    wire [19:0] probe_pair = {20{probe_cnt[4]}};

    wire [79:0] wr_pair = PROBE ? {4{probe_pair}} : {code[3], code[2], code[1], code[0]};

    // ------------------------------------------------------------------
    // Async FIFO, 16 x 80, gray pointers
    // ------------------------------------------------------------------
    localparam AW = 4;
    reg  [79:0]  mem [0:(1<<AW)-1];
    reg  [AW:0]  wp_bin = '0, wp_gray = '0;
    reg  [AW:0]  rp_bin = '0, rp_gray = '0;
    reg  [AW:0]  rp_gray_w1 = '0, rp_gray_w2 = '0;   // rp gray in write domain
    reg  [AW:0]  wp_gray_r1 = '0, wp_gray_r2 = '0;   // wp gray in read domain
    reg          ovf_sticky = 1'b0;
    wire         phy_active;
    reg  [2:0]   act_sync_sym = 3'b000;
    always @(posedge clk_sym) act_sync_sym <= {act_sync_sym[1:0], phy_active};
    wire         phy_active_sym = act_sync_sym[2];

    function automatic [AW:0] g2b(input [AW:0] g);
        integer i;
        begin
            g2b[AW] = g[AW];
            for (i = AW - 1; i >= 0; i = i - 1)
                g2b[i] = g2b[i+1] ^ g[i];
        end
    endfunction

    wire [AW:0] wp_bin_next = wp_bin + 1'b1;
    wire        w_full = (wp_gray ^ rp_gray_w2) == {2'b11, {(AW-1){1'b0}}};
    wire        w_en   = enc_run;

    always @(posedge clk_sym) begin
        rp_gray_w1 <= rp_gray;
        rp_gray_w2 <= rp_gray_w1;
        if (w_en) begin
            if (!w_full) begin
                mem[wp_bin[AW-1:0]] <= wr_pair;
                wp_bin  <= wp_bin_next;
                wp_gray <= wp_bin_next ^ (wp_bin_next >> 1);
            end else if (phy_active_sym) begin
                ovf_sticky <= 1'b1;                 // only an error once streaming
            end
        end
    end

    // Read side (101.25 MHz)
    wire        sym_req;
    reg         sym_vld = 1'b0;
    reg  [79:0] rd_pair = '0;
    reg         unf_sticky = 1'b0;
    reg         started = 1'b0;         // shim has issued its first pull

    always @(posedge tx_clk) begin
        wp_gray_r1 <= wp_gray;
        wp_gray_r2 <= wp_gray_r1;
    end
    wire [AW:0] level_r = g2b(wp_gray_r2) - rp_bin;
    wire        r_empty = (level_r == '0);

    // The shim may only start once the writer runs and the FIFO has
    // reached half: its first request must find data, and from then on
    // the equal 81 M/s rates hold the level there.
    reg [2:0] enc_run_tx_s = 3'b000;
    reg       primed = 1'b0;
    always @(posedge tx_clk) begin
        enc_run_tx_s <= {enc_run_tx_s[1:0], enc_run};
        if (!link_en_tx || !enc_run_tx_s[2])
            primed <= 1'b0;
        else if (level_r >= (1 << (AW - 1)))
            primed <= 1'b1;
    end
    wire shim_link_en = link_en_tx && primed;

    // Before the shim's first pull, drain to half so the stream starts with
    // headroom both ways; from the first pull on, read exactly on request.
    wire pulling = started || sym_req;
    wire rd_en   = pulling ? sym_req : (level_r > (1 << (AW - 1)));
    wire [AW:0] rp_bin_next = rp_bin + 1'b1;

    always @(posedge tx_clk) begin
        sym_vld <= 1'b0;
        if (!shim_link_en)
            started <= 1'b0;
        else if (sym_req)
            started <= 1'b1;
        if (rd_en && !r_empty) begin
            rd_pair <= mem[rp_bin[AW-1:0]];
            sym_vld <= pulling;
            rp_bin  <= rp_bin_next;
            rp_gray <= rp_bin_next ^ (rp_bin_next >> 1);
        end
        if (sym_req && r_empty)
            unf_sticky <= 1'b1;
    end

    // ------------------------------------------------------------------
    // Lane reversal + shim
    // ------------------------------------------------------------------
    wire [79:0] shim_pair;
    generate
        for (gi = 0; gi < 4; gi = gi + 1) begin : g_lane
            assign shim_pair[gi*20 +: 20] = REVERSE_LANES ? rd_pair[(3-gi)*20 +: 20]
                                                          : rd_pair[gi*20 +: 20];
        end
    endgenerate

    wire gb_underflow;

    dp_dphy_tx #(
        .DRV_SETTLE_CYCLES (DRV_SETTLE_CYCLES),
        .HS_SETTLE_CYCLES  (HS_SETTLE_CYCLES)
    ) u_shim (
        .tx_clk_i       (tx_clk),
        .rst_n_i        (rst_tx_n),
        .link_en_i      (shim_link_en),
        .lane_count_i   (3'd4),
        .sym_req_o      (sym_req),
        .sym_vld_i      (sym_vld),
        .sym_pair_i     (shim_pair),
        .phy_active_o   (phy_active),
        .gb_underflow_o (gb_underflow),
        .tx_drst_n_o    (shim_drst_n),          // = rst_n_i; unused (see dphy_drst_n)
        .txhclk_en_o    (txhclk_en),
        .hstxd_vld_o    (hstxd_vld),
        .d0ln_hstxd_o   (d0ln),
        .d1ln_hstxd_o   (d1ln),
        .d2ln_hstxd_o   (d2ln),
        .d3ln_hstxd_o   (d3ln),
        .hstxen_ln_o    (hstxen),
        .txdpen_ln_o    (txdpen),
        .lptxen_ln_o    (lptxen),
        .do_lptx_n_o    (do_lptx_n),
        .do_lptx_p_o    (do_lptx_p),
        .lprx_en_d_o    (lprx_en)
    );

    // ------------------------------------------------------------------
    // Status back to the link policy (mgmt_clk)
    // ------------------------------------------------------------------
    reg [2:0] run_sync = 3'b000;
    always @(posedge mgmt_clk) run_sync <= {run_sync[1:0], phy_active};
    assign tx_running = powerup_channel & {4{run_sync[2]}};

    // tx_clk liveness: toggles while the hard PHY produces TX_CLK_O
    reg tx_clk_tgl = 1'b0;
    always @(posedge tx_clk) tx_clk_tgl <= ~tx_clk_tgl;
    reg [2:0] tx_alive_s = 3'b000;
    reg [7:0] tx_alive_cnt = 8'd0;
    always @(posedge clk_sym) begin
        tx_alive_s <= {tx_alive_s[1:0], tx_clk_tgl};
        if (tx_alive_s[2] != tx_alive_s[1]) tx_alive_cnt <= 8'd255;
        else if (tx_alive_cnt != 8'd0)      tx_alive_cnt <= tx_alive_cnt - 8'd1;
    end

    assign status = {dphy_drst_n, ovf_sticky, unf_sticky | gb_underflow, phy_active,
                     link_en_tx, enc_run, (tx_alive_cnt != 8'd0), 1'b0};

endmodule
