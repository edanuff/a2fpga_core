///////////////////////////////////////////////////////////////////////////////
// dp_serdes_lane_shim.sv — 60B-named `dp_serdes` face over the 138B emission
//
// WHY THIS EXISTS
// ---------------
// The a2mega carrier's DP main-link pairs land on DIFFERENT Q0 lane numbers
// on the two SOM dies — Q0 lanes 1 and 3 are transposed between them
// (docs/SOM_138B_BALLMAP.md §2, verified against the Gowin IDE package
// databases for both dies):
//
//     carrier net   balls     GW5AT-60B      GW5AST-138B
//     DP0 (ML0)     C7/D7     Q0 lane 3      Q0 lane 1     <- moves
//     DP1 (ML1)     A6/B6     Q0 lane 2      Q0 lane 2     <- same
//
// So the 138B SERDES emission is generated for **Q0 lanes 1 + 2** (master
// lane 2 — unchanged, which is why the .sdc clock anchor
// `LANE2_PCS_TX_O_FABRIC_CLK` is identical on both dies) and its module ports
// are named `dp_phy_q0_ln1_*` / `dp_phy_q0_ln2_*`.
//
// The SHARED transceiver bank (hdl/displayport/gowin/transceiver_bank_gowin.v)
// has `DP_SERDES_LANES_4` / `_23` / else-(0/1) branches but no lanes-1/2
// branch, and it is not this project's to modify. Its `_23` branch drives
// ML0 through ports named `..._ln3_*` and ML1 through `..._ln2_*`.
//
// This shim therefore presents exactly the 60B port face (`ln2`/`ln3`) and
// re-labels ML0's half onto die lane 1:
//
//     shared RTL port      this shim          138B emission port
//     dp_phy_q0_ln3_*  ->  ML0 (carrier DP0)  dp_phy_q0_ln1_*
//     dp_phy_q0_ln2_*  ->  ML1 (carrier DP1)  dp_phy_q0_ln2_*
//
// Net effect: DP main-link lane 0 leaves the package on C7/D7 and lane 1 on
// A6/B6 — the same two carrier pairs, in the same order, as on the 60B.
//
// The alternative (cleaner, but a shared-RTL change) is a
// `DP_SERDES_LANES_12` branch in transceiver_bank_gowin.v naming the ports
// directly. Recorded as the preferred long-term fix; deliberately not done
// here to keep the shared DP core byte-identical between SOM variants.
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
///////////////////////////////////////////////////////////////////////////////
`timescale 100 ps / 100 ps

module dp_serdes (
    input  wire        por_n_i,

    // ---- "lane 2" face = DP main-link lane 1 = carrier DP1 = die lane 2 ----
    output wire        dp_phy_q0_ln2_rx_pcs_clkout_o,
    output wire [87:0] dp_phy_q0_ln2_rx_data_o,
    output wire [4:0]  dp_phy_q0_ln2_rx_fifo_rdusewd_o,
    output wire        dp_phy_q0_ln2_rx_fifo_aempty_o,
    output wire        dp_phy_q0_ln2_rx_fifo_empty_o,
    output wire        dp_phy_q0_ln2_rx_valid_o,
    output wire        dp_phy_q0_ln2_tx_pcs_clkout_o,
    output wire [4:0]  dp_phy_q0_ln2_tx_fifo_wrusewd_o,
    output wire        dp_phy_q0_ln2_tx_fifo_afull_o,
    output wire        dp_phy_q0_ln2_tx_fifo_full_o,
    output wire        dp_phy_q0_ln2_refclk_o,
    output wire        dp_phy_q0_ln2_signal_detect_o,
    output wire        dp_phy_q0_ln2_rx_cdr_lock_o,
    output wire        dp_phy_q0_ln2_pll_lock_o,
    output wire        dp_phy_q0_ln2_ready_o,
    input  wire        dp_phy_q0_ln2_rx_clk_i,
    input  wire        dp_phy_q0_ln2_rx_fifo_rden_i,
    input  wire        dp_phy_q0_ln2_tx_clk_i,
    input  wire [79:0] dp_phy_q0_ln2_tx_data_i,
    input  wire        dp_phy_q0_ln2_tx_fifo_wren_i,
    input  wire        dp_phy_q0_ln2_pma_rstn_i,
    input  wire        dp_phy_q0_ln2_pcs_rx_rst_i,
    input  wire        dp_phy_q0_ln2_pcs_tx_rst_i,

    // ---- "lane 3" face = DP main-link lane 0 = carrier DP0 = die lane 1 ----
    output wire        dp_phy_q0_ln3_rx_pcs_clkout_o,
    output wire [87:0] dp_phy_q0_ln3_rx_data_o,
    output wire [4:0]  dp_phy_q0_ln3_rx_fifo_rdusewd_o,
    output wire        dp_phy_q0_ln3_rx_fifo_aempty_o,
    output wire        dp_phy_q0_ln3_rx_fifo_empty_o,
    output wire        dp_phy_q0_ln3_rx_valid_o,
    output wire        dp_phy_q0_ln3_tx_pcs_clkout_o,
    output wire [4:0]  dp_phy_q0_ln3_tx_fifo_wrusewd_o,
    output wire        dp_phy_q0_ln3_tx_fifo_afull_o,
    output wire        dp_phy_q0_ln3_tx_fifo_full_o,
    output wire        dp_phy_q0_ln3_refclk_o,
    output wire        dp_phy_q0_ln3_signal_detect_o,
    output wire        dp_phy_q0_ln3_rx_cdr_lock_o,
    output wire        dp_phy_q0_ln3_pll_lock_o,
    output wire        dp_phy_q0_ln3_ready_o,
    input  wire        dp_phy_q0_ln3_rx_clk_i,
    input  wire        dp_phy_q0_ln3_rx_fifo_rden_i,
    input  wire        dp_phy_q0_ln3_tx_clk_i,
    input  wire [79:0] dp_phy_q0_ln3_tx_data_i,
    input  wire        dp_phy_q0_ln3_tx_fifo_wren_i,
    input  wire        dp_phy_q0_ln3_pma_rstn_i,
    input  wire        dp_phy_q0_ln3_pcs_rx_rst_i,
    input  wire        dp_phy_q0_ln3_pcs_tx_rst_i,

    // ---- DRP (register-dump bridge; identical on both dies) --------------
    output wire        dp_phy_drp_clk_o,
    output wire        dp_phy_drp_ready_o,
    output wire        dp_phy_drp_rdvld_o,
    output wire [31:0] dp_phy_drp_rddata_o,
    output wire        dp_phy_drp_resp_o,
    input  wire [23:0] dp_phy_drp_addr_i,
    input  wire        dp_phy_drp_wren_i,
    input  wire [31:0] dp_phy_drp_wrdata_i,
    input  wire [7:0]  dp_phy_drp_strb_i,
    input  wire        dp_phy_drp_rden_i
);

    dp_serdes_138b i_dp_serdes_138b (
        .por_n_i                          (por_n_i),

        // die lane 2  <- "ln2" face (ML1, carrier DP1)
        .dp_phy_q0_ln2_rx_pcs_clkout_o    (dp_phy_q0_ln2_rx_pcs_clkout_o),
        .dp_phy_q0_ln2_rx_data_o          (dp_phy_q0_ln2_rx_data_o),
        .dp_phy_q0_ln2_rx_fifo_rdusewd_o  (dp_phy_q0_ln2_rx_fifo_rdusewd_o),
        .dp_phy_q0_ln2_rx_fifo_aempty_o   (dp_phy_q0_ln2_rx_fifo_aempty_o),
        .dp_phy_q0_ln2_rx_fifo_empty_o    (dp_phy_q0_ln2_rx_fifo_empty_o),
        .dp_phy_q0_ln2_rx_valid_o         (dp_phy_q0_ln2_rx_valid_o),
        .dp_phy_q0_ln2_tx_pcs_clkout_o    (dp_phy_q0_ln2_tx_pcs_clkout_o),
        .dp_phy_q0_ln2_tx_fifo_wrusewd_o  (dp_phy_q0_ln2_tx_fifo_wrusewd_o),
        .dp_phy_q0_ln2_tx_fifo_afull_o    (dp_phy_q0_ln2_tx_fifo_afull_o),
        .dp_phy_q0_ln2_tx_fifo_full_o     (dp_phy_q0_ln2_tx_fifo_full_o),
        .dp_phy_q0_ln2_refclk_o           (dp_phy_q0_ln2_refclk_o),
        .dp_phy_q0_ln2_signal_detect_o    (dp_phy_q0_ln2_signal_detect_o),
        .dp_phy_q0_ln2_rx_cdr_lock_o      (dp_phy_q0_ln2_rx_cdr_lock_o),
        .dp_phy_q0_ln2_pll_lock_o         (dp_phy_q0_ln2_pll_lock_o),
        .dp_phy_q0_ln2_ready_o            (dp_phy_q0_ln2_ready_o),
        .dp_phy_q0_ln2_rx_clk_i           (dp_phy_q0_ln2_rx_clk_i),
        .dp_phy_q0_ln2_rx_fifo_rden_i     (dp_phy_q0_ln2_rx_fifo_rden_i),
        .dp_phy_q0_ln2_tx_clk_i           (dp_phy_q0_ln2_tx_clk_i),
        .dp_phy_q0_ln2_tx_data_i          (dp_phy_q0_ln2_tx_data_i),
        .dp_phy_q0_ln2_tx_fifo_wren_i     (dp_phy_q0_ln2_tx_fifo_wren_i),
        .dp_phy_q0_ln2_pma_rstn_i         (dp_phy_q0_ln2_pma_rstn_i),
        .dp_phy_q0_ln2_pcs_rx_rst_i       (dp_phy_q0_ln2_pcs_rx_rst_i),
        .dp_phy_q0_ln2_pcs_tx_rst_i       (dp_phy_q0_ln2_pcs_tx_rst_i),

        // die lane 1  <- "ln3" face (ML0, carrier DP0)  *** the remap ***
        .dp_phy_q0_ln1_rx_pcs_clkout_o    (dp_phy_q0_ln3_rx_pcs_clkout_o),
        .dp_phy_q0_ln1_rx_data_o          (dp_phy_q0_ln3_rx_data_o),
        .dp_phy_q0_ln1_rx_fifo_rdusewd_o  (dp_phy_q0_ln3_rx_fifo_rdusewd_o),
        .dp_phy_q0_ln1_rx_fifo_aempty_o   (dp_phy_q0_ln3_rx_fifo_aempty_o),
        .dp_phy_q0_ln1_rx_fifo_empty_o    (dp_phy_q0_ln3_rx_fifo_empty_o),
        .dp_phy_q0_ln1_rx_valid_o         (dp_phy_q0_ln3_rx_valid_o),
        .dp_phy_q0_ln1_tx_pcs_clkout_o    (dp_phy_q0_ln3_tx_pcs_clkout_o),
        .dp_phy_q0_ln1_tx_fifo_wrusewd_o  (dp_phy_q0_ln3_tx_fifo_wrusewd_o),
        .dp_phy_q0_ln1_tx_fifo_afull_o    (dp_phy_q0_ln3_tx_fifo_afull_o),
        .dp_phy_q0_ln1_tx_fifo_full_o     (dp_phy_q0_ln3_tx_fifo_full_o),
        .dp_phy_q0_ln1_refclk_o           (dp_phy_q0_ln3_refclk_o),
        .dp_phy_q0_ln1_signal_detect_o    (dp_phy_q0_ln3_signal_detect_o),
        .dp_phy_q0_ln1_rx_cdr_lock_o      (dp_phy_q0_ln3_rx_cdr_lock_o),
        .dp_phy_q0_ln1_pll_lock_o         (dp_phy_q0_ln3_pll_lock_o),
        .dp_phy_q0_ln1_ready_o            (dp_phy_q0_ln3_ready_o),
        .dp_phy_q0_ln1_rx_clk_i           (dp_phy_q0_ln3_rx_clk_i),
        .dp_phy_q0_ln1_rx_fifo_rden_i     (dp_phy_q0_ln3_rx_fifo_rden_i),
        .dp_phy_q0_ln1_tx_clk_i           (dp_phy_q0_ln3_tx_clk_i),
        .dp_phy_q0_ln1_tx_data_i          (dp_phy_q0_ln3_tx_data_i),
        .dp_phy_q0_ln1_tx_fifo_wren_i     (dp_phy_q0_ln3_tx_fifo_wren_i),
        .dp_phy_q0_ln1_pma_rstn_i         (dp_phy_q0_ln3_pma_rstn_i),
        .dp_phy_q0_ln1_pcs_rx_rst_i       (dp_phy_q0_ln3_pcs_rx_rst_i),
        .dp_phy_q0_ln1_pcs_tx_rst_i       (dp_phy_q0_ln3_pcs_tx_rst_i),

        .dp_phy_drp_clk_o                 (dp_phy_drp_clk_o),
        .dp_phy_drp_ready_o               (dp_phy_drp_ready_o),
        .dp_phy_drp_rdvld_o               (dp_phy_drp_rdvld_o),
        .dp_phy_drp_rddata_o              (dp_phy_drp_rddata_o),
        .dp_phy_drp_resp_o                (dp_phy_drp_resp_o),
        .dp_phy_drp_addr_i                (dp_phy_drp_addr_i),
        .dp_phy_drp_wren_i                (dp_phy_drp_wren_i),
        .dp_phy_drp_wrdata_i              (dp_phy_drp_wrdata_i),
        .dp_phy_drp_strb_i                (dp_phy_drp_strb_i),
        .dp_phy_drp_rden_i                (dp_phy_drp_rden_i)
    );

endmodule
