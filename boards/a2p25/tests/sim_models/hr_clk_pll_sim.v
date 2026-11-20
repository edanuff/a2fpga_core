// Behavioural stand-in for hr_clk_pll (simulation only). Gowin's PLLA model
// neither applies the static phase (PE_COARSE/FINE) nor aligns outputs of
// different dividers, so it cannot exercise hyperram_ctrl's 2:1 + 90 deg
// clocking. Same exact timebase as test_hyperram_harness: clk_mem 108 MHz,
// clk_mem_90 = +90 deg, clk 54 MHz with rising edges on clk_mem rising
// edges (CK 108 variant only).
`timescale 1ns/1ps
module hr_clk_pll #(
    parameter IDIV = 5, parameter MDIV = 108, parameter ODIV_LOGIC = 20,
    parameter ODIV_MEM = 10, parameter PE90_COARSE = 2, parameter PE90_FINE = 4,
    parameter FCLKIN = "50"
) (
    output reg lock = 1'b0,
    output clk, output clk_mem, output clk_mem_90,
    input  clkin
);
    reg [2:0] ph = 3'd0;
    always #2.315 ph = ph + 3'd1;
    assign clk_mem    = (ph[1] == 1'b0);
    assign clk_mem_90 = ph[1] ^ ph[0];
    assign clk        = (ph[2] == 1'b0);
    initial #1000 lock = 1'b1;
endmodule
