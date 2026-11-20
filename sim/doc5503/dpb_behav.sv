// Behavioural model of the Gowin Arora V DPB primitive for simulators that do
// not load the vendor library (Verilator co-sim). Covers what doc5503's
// osc_reg_bank_dpb uses: BIT_WIDTH 16 on both ports, bypass read mode
// (registered one-cycle read), normal write mode (output holds on a write
// cycle), byte-lane enables on AD[1:0] with the word address in AD[13:4].
// Semantics follow IDE/simlib/gw5a/prim_sim.v (module DPB). Not for synthesis.
module DPB #(
    parameter READ_MODE0 = 1'b0, parameter READ_MODE1 = 1'b0,
    parameter WRITE_MODE0 = 2'b00, parameter WRITE_MODE1 = 2'b00,
    parameter BIT_WIDTH_0 = 16, parameter BIT_WIDTH_1 = 16,
    parameter BLK_SEL_0 = 3'b000, parameter BLK_SEL_1 = 3'b000,
    parameter RESET_MODE = "SYNC"
) (
    output reg  [15:0] DOA, output reg [15:0] DOB,
    input  wire [15:0] DIA, input  wire [15:0] DIB,
    input  wire [2:0]  BLKSELA, input wire [2:0] BLKSELB,
    input  wire [13:0] ADA, input  wire [13:0] ADB,
    input  wire        WREA, WREB, CLKA, CLKB, CEA, CEB, OCEA, OCEB, RESETA, RESETB
);
    initial if (BIT_WIDTH_0 != 16 || BIT_WIDTH_1 != 16 || READ_MODE0 || READ_MODE1 ||
                WRITE_MODE0 != 2'b00 || WRITE_MODE1 != 2'b00)
        $error("dpb_behav: only x16 / bypass read / normal write is modelled");
    reg [15:0] mem [0:1023];
    integer i;
    initial begin DOA = 0; DOB = 0; for (i = 0; i < 1024; i = i + 1) mem[i] = 16'h0000; end
    wire [9:0] wa = ADA[13:4];
    wire [9:0] wb = ADB[13:4];
    always @(posedge CLKA) if (CEA && BLKSELA == BLK_SEL_0) begin
        if (WREA) begin
            if (ADA[0]) mem[wa][7:0]  <= DIA[7:0];
            if (ADA[1]) mem[wa][15:8] <= DIA[15:8];
        end else if (OCEA) DOA <= mem[wa];
        if (RESETA) DOA <= 16'h0000;
    end
    always @(posedge CLKB) if (CEB && BLKSELB == BLK_SEL_1) begin
        if (WREB) begin
            if (ADB[0]) mem[wb][7:0]  <= DIB[7:0];
            if (ADB[1]) mem[wb][15:8] <= DIB[15:8];
        end else if (OCEB) DOB <= mem[wb];
        if (RESETB) DOB <= 16'h0000;
    end
endmodule

// Global set/reset net stub (the vendor library's DPB reads GSR.GSRO; the
// behavioural DPB above does not, but testbenches instantiate GSR).
module GSR (input wire GSRI);
    wire GSRO = GSRI;
endmodule
