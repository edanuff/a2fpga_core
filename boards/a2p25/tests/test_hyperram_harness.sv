//
// HyperRAM simulation harness: clocks, Infineon S27KL0642 model, board
// delay and pin-level timing checks. Shared by test_hyperram_ctrl.sv and
// test_hyperram_bist.sv.
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES.
//
// Clocks come from one 2.315 ns quarter-period timebase so clk (54 MHz),
// clk_mem (108 MHz) and clk_mem_90 are exactly aligned (rising edges of
// clk coincide with rising edges of clk_mem).
//
// Board delay (+BOARD_DLY_PS=<ps>, default 0) is a transport delay applied
// to every signal in both directions: CK, CS# and the FPGA-driven DQ/RWDS
// (taken from the PHY's ODDR data/tristate outputs) towards the RAM, and
// the model's DQ/RWDS drivers back towards the FPGA pads. The two sides
// are separate nets, so a turnaround conflict shows up as a contention
// error rather than being hidden.
//
// Checks at the RAM pins (166 MHz / 3.0 V grade): tCSS >= 3 ns, tCSH >= 0
// (CK low when CS# rises), tCSHI >= 6 ns, tRWR >= 36 ns (CS# rise to CK#2
// fall of the next transaction — the model's $recovery definition),
// tCSM <= 4 us, CK high/low >= 3 ns, CK period >= 6 ns, tIS/tIH >= 0.6 ns
// for DQ and RWDS while the FPGA drives them, and bus contention on
// either side. iverilog ignores the model's specify block, hence these.
//
// inj_xor: XORed into the RAM->FPGA DQ path (error injection).
//

`timescale 1ns / 1ps

module test_hyperram_harness (
    output logic        clk,
    output logic        clk_mem,
    output logic        clk_mem_90,

    // FPGA pads
    input  wire         hr_ck,
    input  wire         hr_cs_n,
    input  wire  [7:0]  f_dq_drv,     // FPGA DQ driver (z when tristated)
    input  wire         f_rwds_drv,
    output wire  [7:0]  hr_dq_in,     // RAM -> FPGA (z when the RAM is not driving)
    output wire         hr_rwds_in,

    input  wire  [7:0]  inj_xor,
    output integer      timing_errors,
    output integer      contention_errors
);

    // ---------------------------------------------------------------------
    // Clocks
    // ---------------------------------------------------------------------
    logic [2:0] ph = 3'd0;
    always #2.315 ph = ph + 3'd1;
    assign clk_mem    = (ph[1] == 1'b0);
    assign clk_mem_90 = ph[1] ^ ph[0];
    assign clk        = (ph[2] == 1'b0);

    // ---------------------------------------------------------------------
    // Board delay
    // ---------------------------------------------------------------------
    real dly;
    integer dly_ps;
    initial begin
        if (!$value$plusargs("BOARD_DLY_PS=%d", dly_ps)) dly_ps = 0;
        dly = dly_ps / 1000.0;
    end

    logic       ram_ck = 1'b0;
    logic       ram_cs_n = 1'b1;
    logic [7:0] f2r_dq = 8'hzz;
    logic       f2r_rwds = 1'bz;
    logic [7:0] r2f_dq = 8'hzz;
    logic       r2f_rwds = 1'bz;

    always @(hr_ck)      ram_ck   <= #(dly) hr_ck;
    always @(hr_cs_n)    ram_cs_n <= #(dly) hr_cs_n;
    always @(f_dq_drv)   f2r_dq   <= #(dly) f_dq_drv;
    always @(f_rwds_drv) f2r_rwds <= #(dly) f_rwds_drv;

    wire  [7:0] ram_dq;
    wire        ram_rwds;
    assign ram_dq   = f2r_dq;
    assign ram_rwds = f2r_rwds;

    s27kl0642 u_ram (
        .DQ7      (ram_dq[7]),
        .DQ6      (ram_dq[6]),
        .DQ5      (ram_dq[5]),
        .DQ4      (ram_dq[4]),
        .DQ3      (ram_dq[3]),
        .DQ2      (ram_dq[2]),
        .DQ1      (ram_dq[1]),
        .DQ0      (ram_dq[0]),
        .RWDS     (ram_rwds),
        .CSNeg    (ram_cs_n),
        .CK       (ram_ck),
        .CKn      (~ram_ck),
        .RESETNeg (1'b1)
    );

    // RAM drivers, delayed back to the FPGA
    wire [7:0] m_dq   = u_ram.Dout_zd;
    wire       m_rwds = u_ram.RWDSout_zd;
    always @(m_dq)   r2f_dq   <= #(dly) m_dq;
    always @(m_rwds) r2f_rwds <= #(dly) m_rwds;

    genvar gi;
    generate
        for (gi = 0; gi < 8; gi = gi + 1) begin : g_inj
            assign hr_dq_in[gi] = (r2f_dq[gi] === 1'bz) ? 1'bz : (r2f_dq[gi] ^ inj_xor[gi]);
        end
    endgenerate
    assign hr_rwds_in = r2f_rwds;

    // ---------------------------------------------------------------------
    // Timing checks at the RAM pins
    // ---------------------------------------------------------------------
    localparam real T_CSS  = 3.0;
    localparam real T_CSHI = 6.0;
    localparam real T_RWR  = 36.0;
    localparam real T_CSM  = 4000.0;
    localparam real T_CHL  = 3.0;
    localparam real T_CK   = 6.0;
    localparam real T_IS   = 0.6;
    localparam real T_IH   = 0.6;

    initial begin
        timing_errors     = 0;
        contention_errors = 0;
    end

    task automatic terr(input string what, input real val);
        begin
            timing_errors = timing_errors + 1;
            if (timing_errors <= 20)
                $display("[%0t] TIMING %s violated (%0.3f ns)", $time, what, val);
        end
    endtask

    // Observed minimum / maximum margins, for the report
    real min_tcss = 1.0e9, min_trwr = 1.0e9, min_tcshi = 1.0e9, max_tcsm = 0.0;
    real min_tis = 1.0e9, min_tih = 1.0e9;

    real t_cs_fall = -1.0e9, t_cs_rise = -1.0e9;
    real t_ck_rise = -1.0e9, t_ck_fall = -1.0e9, t_ck_edge = -1.0e9;
    real t_dq_chg = -1.0e9, t_rwds_chg = -1.0e9;
    integer ck_rises_in_cs = 0, ck_falls_in_cs = 0;
    logic   first_cs = 1'b1;

    always @(negedge ram_cs_n) begin
        if (!first_cs && ($realtime - t_cs_rise) < T_CSHI)
            terr("tCSHI", $realtime - t_cs_rise);
        if (!first_cs && ($realtime - t_cs_rise) < min_tcshi)
            min_tcshi = $realtime - t_cs_rise;
        t_cs_fall      = $realtime;
        ck_rises_in_cs = 0;
        ck_falls_in_cs = 0;
    end

    always @(posedge ram_cs_n) begin
        if (ram_ck !== 1'b0)
            terr("tCSH (CK not low at CS# rise)", 0.0);
        if (($realtime - t_cs_fall) > T_CSM && t_cs_fall > 0.0)
            terr("tCSM", $realtime - t_cs_fall);
        if (t_cs_fall > 0.0 && ($realtime - t_cs_fall) > max_tcsm)
            max_tcsm = $realtime - t_cs_fall;
        t_cs_rise = $realtime;
        first_cs  = 1'b0;
    end

    always @(posedge ram_ck) begin
        if (($realtime - t_ck_fall) < T_CHL) terr("tCL", $realtime - t_ck_fall);
        if (($realtime - t_ck_rise) < T_CK)  terr("tCK", $realtime - t_ck_rise);
        if (ram_cs_n === 1'b0) begin
            if (ck_rises_in_cs == 0 && ($realtime - t_cs_fall) < T_CSS)
                terr("tCSS", $realtime - t_cs_fall);
            if (ck_rises_in_cs == 0 && ($realtime - t_cs_fall) < min_tcss)
                min_tcss = $realtime - t_cs_fall;
            ck_rises_in_cs = ck_rises_in_cs + 1;
            if (f2r_dq !== 8'hzz && ($realtime - t_dq_chg) < T_IS)
                terr("tIS DQ", $realtime - t_dq_chg);
            if (f2r_rwds !== 1'bz && ($realtime - t_rwds_chg) < T_IS)
                terr("tIS RWDS", $realtime - t_rwds_chg);
            if (f2r_dq !== 8'hzz && ($realtime - t_dq_chg) < min_tis)
                min_tis = $realtime - t_dq_chg;
        end
        t_ck_rise = $realtime;
        t_ck_edge = $realtime;
    end

    always @(negedge ram_ck) begin
        if (($realtime - t_ck_rise) < T_CHL) terr("tCH", $realtime - t_ck_rise);
        if (ram_cs_n === 1'b0) begin
            ck_falls_in_cs = ck_falls_in_cs + 1;
            if (ck_falls_in_cs == 2 && !first_cs && ($realtime - t_cs_rise) < T_RWR)
                terr("tRWR", $realtime - t_cs_rise);
            if (ck_falls_in_cs == 2 && !first_cs && ($realtime - t_cs_rise) < min_trwr)
                min_trwr = $realtime - t_cs_rise;
            if (f2r_dq !== 8'hzz && ($realtime - t_dq_chg) < T_IS)
                terr("tIS DQ", $realtime - t_dq_chg);
            if (f2r_rwds !== 1'bz && ($realtime - t_rwds_chg) < T_IS)
                terr("tIS RWDS", $realtime - t_rwds_chg);
        end
        t_ck_fall = $realtime;
        t_ck_edge = $realtime;
    end

    logic [7:0] f2r_dq_prev = 8'hzz;
    logic       f2r_rwds_prev = 1'bz;
    always @(f2r_dq) begin
        if (ram_cs_n === 1'b0 && f2r_dq_prev !== 8'hzz && ($realtime - t_ck_edge) < T_IH)
            terr("tIH DQ", $realtime - t_ck_edge);
        if (ram_cs_n === 1'b0 && f2r_dq_prev !== 8'hzz && ($realtime - t_ck_edge) < min_tih)
            min_tih = $realtime - t_ck_edge;
        t_dq_chg    = $realtime;
        f2r_dq_prev = f2r_dq;
    end
    always @(f2r_rwds) begin
        if (ram_cs_n === 1'b0 && f2r_rwds_prev !== 1'bz && ($realtime - t_ck_edge) < T_IH)
            terr("tIH RWDS", $realtime - t_ck_edge);
        t_rwds_chg    = $realtime;
        f2r_rwds_prev = f2r_rwds;
    end

    // ---------------------------------------------------------------------
    // Contention (both sides drive for > 1 ps)
    // ---------------------------------------------------------------------
    function automatic logic any_drv8(input logic [7:0] v);
        integer k;
        any_drv8 = 1'b0;
        for (k = 0; k < 8; k = k + 1)
            if (v[k] !== 1'bz) any_drv8 = 1'b1;
    endfunction

    wire ram_side_conflict  = (any_drv8(f2r_dq) && any_drv8(m_dq)) ||
                              (f2r_rwds !== 1'bz && m_rwds !== 1'bz);
    wire fpga_side_conflict = (any_drv8(f_dq_drv) && any_drv8(r2f_dq)) ||
                              (f_rwds_drv !== 1'bz && r2f_rwds !== 1'bz);

    always @(posedge ram_side_conflict) begin
        #0.001;
        if (ram_side_conflict) begin
            contention_errors = contention_errors + 1;
            if (contention_errors <= 10)
                $display("[%0t] CONTENTION at RAM pins", $time);
        end
    end
    always @(posedge fpga_side_conflict) begin
        #0.001;
        if (fpga_side_conflict) begin
            contention_errors = contention_errors + 1;
            if (contention_errors <= 10)
                $display("[%0t] CONTENTION at FPGA pins", $time);
        end
    end

endmodule
