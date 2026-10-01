// Behavioural stand-in for Gowin_MIPI_DPHY_PLLA (simulation only).
//
// The Gowin PLLA simulation model, driven through PLL_INIT, does not
// reproduce the generated configuration (it ran at ~604 MHz instead of
// VCO 810 and its outputs were not in exact integer ratio), which turns
// the a2p25 D-PHY clock relationship — TX_CLK_O = VCO/8 and the 81 MHz
// symbol clock = VCO/10, exactly 5:4 — into a false rate mismatch. This
// model makes every output from one 810 MHz edge stream:
//   clkout0..3 = 810 MHz at 0/90/180/270 deg, clkout4 = 810 / 10 = 81 MHz
// lock rises 20 us after the first reference edge.
`timescale 1ps/1fs
module Gowin_MIPI_DPHY_PLLA (lock, clkout0, clkout1, clkout2, clkout3, clkout4, mdrdo, clkin, reset, mdclk, mdopc, mdainc, mdwdi);
    output reg lock = 1'b0;
    output reg clkout0 = 1'b0, clkout1 = 1'b0, clkout2 = 1'b1, clkout3 = 1'b1;
    output reg clkout4 = 1'b0;
    output [7:0] mdrdo;
    input clkin, reset, mdclk;
    input [1:0] mdopc;
    input mdainc;
    input [7:0] mdwdi;

    assign mdrdo = 8'h00;
    localparam real HALF = 1e6 / 810.0 / 2.0;   // ps
    reg started = 1'b0;
    integer div4 = 0;
    always @(posedge clkin) if (!started) begin
        started = 1'b1;
        #20_000_000 lock = 1'b1;
    end
    initial begin
        wait (started);
        forever begin
            #(HALF / 2.0) clkout1 = 1'b1; clkout3 = 1'b0;
            #(HALF / 2.0) clkout0 = 1'b0; clkout2 = 1'b1;
            #(HALF / 2.0) clkout1 = 1'b0; clkout3 = 1'b1;
            #(HALF / 2.0) clkout0 = 1'b1; clkout2 = 1'b0;
            div4 = div4 + 1;
            if (div4 == 5) begin div4 = 0; clkout4 = ~clkout4; end
        end
    end
endmodule
