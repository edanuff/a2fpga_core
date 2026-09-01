// test_dp_dphy_tx.sv - testbench for the DP-over-D-PHY main-link shim.
//
// Validates the 10->16 gearbox against a model of the D-PHY serializer:
// the DUT's 16-bit words are serialized bit 0 first (matching the hard
// macro's 16-bit little-endian, LSB-first-per-byte behavior, UG296E
// Table 4-1), the resulting serial stream is regrouped into 10-bit
// symbols (bit 0 = first on wire), and compared against the exact symbol
// sequence the TB supplied. Also checks:
//   - the supply contract (req -> vld next cycle) never underflows
//   - the stream is continuous once active (hstxd_vld never drops)
//   - enable sequencing order (txdpen before hstxen)
//   - inactive lanes stay quiet in 1-lane mode
//
// Run: make -C boards/a2p25/tests dp_dphy

`timescale 1ns/1ps

module test_dp_dphy_tx;

    localparam int MAX_SYMS = 40000;      // per lane recording depth

    logic        clk;
    logic        rst_n;
    logic        link_en;
    logic [2:0]  lane_count;
    logic        sym_req;
    logic        sym_vld;
    logic [79:0] sym_pair;
    logic        phy_active;
    logic        gb_underflow;
    logic        tx_drst_n;
    logic        txhclk_en;
    logic        hstxd_vld;
    logic [15:0] dln_hstxd [0:3];
    logic [3:0]  hstxen_ln;
    logic [3:0]  txdpen_ln;
    logic [3:0]  lptxen_ln, do_lptx_n, do_lptx_p, lprx_en_d;

    dp_dphy_tx #(
        .DRV_SETTLE_CYCLES (20'd8),
        .HS_SETTLE_CYCLES  (20'd8)
    ) dut (
        .tx_clk_i     (clk),
        .rst_n_i      (rst_n),
        .link_en_i    (link_en),
        .lane_count_i (lane_count),
        .sym_req_o    (sym_req),
        .sym_vld_i    (sym_vld),
        .sym_pair_i   (sym_pair),
        .phy_active_o (phy_active),
        .gb_underflow_o (gb_underflow),
        .tx_drst_n_o  (tx_drst_n),
        .txhclk_en_o  (txhclk_en),
        .hstxd_vld_o  (hstxd_vld),
        .d0ln_hstxd_o (dln_hstxd[0]),
        .d1ln_hstxd_o (dln_hstxd[1]),
        .d2ln_hstxd_o (dln_hstxd[2]),
        .d3ln_hstxd_o (dln_hstxd[3]),
        .hstxen_ln_o  (hstxen_ln),
        .txdpen_ln_o  (txdpen_ln),
        .lptxen_ln_o  (lptxen_ln),
        .do_lptx_n_o  (do_lptx_n),
        .do_lptx_p_o  (do_lptx_p),
        .lprx_en_d_o  (lprx_en_d)
    );

    // 101.25 MHz-ish; content is timing-agnostic
    initial clk = 1'b0;
    always #4.938 clk = ~clk;

    integer errors;

    // ------------------------------------------------------------------
    // Symbol source: per-lane 10-bit LFSR, recorded in supply order
    // ------------------------------------------------------------------
    logic [9:0] lfsr [0:3];
    logic [9:0] exp_sym [0:3][0:MAX_SYMS-1];
    integer     exp_cnt [0:3];

    function automatic [9:0] lfsr_next(input [9:0] s);
        // x^10 + x^7 + 1, never all-zero
        lfsr_next = {s[8:0], s[9] ^ s[6]};
    endfunction

    logic [3:0] active_mask;
    always_comb begin
        case (lane_count)
            3'd1:    active_mask = 4'b0001;
            3'd2:    active_mask = 4'b0011;
            default: active_mask = 4'b1111;
        endcase
    end

    // honor the supply contract: req in cycle N -> vld + data in N+1
    integer sl;
    logic [9:0] s0, s1;
    always @(posedge clk) begin
        if (!rst_n) begin
            sym_vld  <= 1'b0;
            sym_pair <= 80'd0;
        end else begin
            sym_vld <= sym_req;
            if (sym_req) begin
                for (sl = 0; sl < 4; sl = sl + 1) begin
                    s0 = lfsr_next(lfsr[sl]);
                    s1 = lfsr_next(s0);
                    lfsr[sl] <= s1;
                    sym_pair[sl*20 +: 20] <= {s1, s0};
                    if (active_mask[sl] && exp_cnt[sl] < MAX_SYMS - 2) begin
                        exp_sym[sl][exp_cnt[sl]]     = s0;
                        exp_sym[sl][exp_cnt[sl] + 1] = s1;
                        exp_cnt[sl] = exp_cnt[sl] + 2;
                    end
                end
            end
        end
    end

    // ------------------------------------------------------------------
    // Serializer model + symbol reassembly + on-the-fly compare
    // ------------------------------------------------------------------
    logic [9:0] asm_sym  [0:3];   // symbol being assembled from serial bits
    integer     asm_pos  [0:3];   // bits collected so far
    integer     chk_cnt  [0:3];   // symbols compared so far
    integer     word_cnt;         // words captured (lane 0 reference)
    logic       was_active;       // continuity check

    integer cl, cb;
    always @(posedge clk) begin
        if (rst_n && link_en) begin
            // continuity: once streaming, hstxd_vld must never drop
            if (was_active && !hstxd_vld) begin
                $display("FAIL: hstxd_vld dropped mid-stream at %0t", $time);
                errors = errors + 1;
            end
            was_active <= hstxd_vld;

            if (hstxd_vld) begin
                word_cnt = word_cnt + 1;
                for (cl = 0; cl < 4; cl = cl + 1) begin
                    if (active_mask[cl]) begin
                        // serialize bit 0 first, regroup into 10-bit symbols
                        for (cb = 0; cb < 16; cb = cb + 1) begin
                            asm_sym[cl][asm_pos[cl]] = dln_hstxd[cl][cb];
                            asm_pos[cl] = asm_pos[cl] + 1;
                            if (asm_pos[cl] == 10) begin
                                if (chk_cnt[cl] < exp_cnt[cl]) begin
                                    if (asm_sym[cl] !== exp_sym[cl][chk_cnt[cl]]) begin
                                        if (errors < 20)
                                            $display("FAIL: lane %0d sym %0d got %b exp %b",
                                                     cl, chk_cnt[cl], asm_sym[cl],
                                                     exp_sym[cl][chk_cnt[cl]]);
                                        errors = errors + 1;
                                    end
                                end
                                chk_cnt[cl] = chk_cnt[cl] + 1;
                                asm_pos[cl] = 0;
                            end
                        end
                    end else begin
                        if (dln_hstxd[cl] !== 16'h0) begin
                            $display("FAIL: inactive lane %0d not quiet (%h) at %0t",
                                     cl, dln_hstxd[cl], $time);
                            errors = errors + 1;
                        end
                    end
                end
            end
        end else begin
            was_active <= 1'b0;
        end
    end

    // enable sequencing: txdpen must be up before hstxen rises
    logic [3:0] hstxen_q;
    always @(posedge clk) begin
        hstxen_q <= hstxen_ln;
        for (cl = 0; cl < 4; cl = cl + 1)
            if (hstxen_ln[cl] && !hstxen_q[cl] && !txdpen_ln[cl]) begin
                $display("FAIL: hstxen_ln[%0d] rose before txdpen at %0t", cl, $time);
                errors = errors + 1;
            end
    end

    // ------------------------------------------------------------------
    // Test phases
    // ------------------------------------------------------------------
    task automatic reset_scoreboard;
        integer i;
        begin
            for (i = 0; i < 4; i = i + 1) begin
                exp_cnt[i] = 0;
                chk_cnt[i] = 0;
                asm_pos[i] = 0;
                lfsr[i]    = 10'h155 ^ i[9:0];
            end
            word_cnt = 0;
        end
    endtask

    task automatic run_phase(input [2:0] lanes, input integer cycles,
                             input integer min_syms, input string name);
        integer i, checked_ok;
        logic [3:0] mask;
        begin
            mask = active_mask_of(lanes);
            reset_scoreboard;
            lane_count = lanes;
            @(posedge clk);
            link_en = 1'b1;
            repeat (cycles) @(posedge clk);
            link_en = 1'b0;
            repeat (10) @(posedge clk);

            checked_ok = 1;
            for (i = 0; i < 4; i = i + 1) begin
                if (mask[i]) begin
                    if (chk_cnt[i] < min_syms) begin
                        $display("FAIL(%s): lane %0d only %0d symbols checked (< %0d)",
                                 name, i, chk_cnt[i], min_syms);
                        errors = errors + 1;
                        checked_ok = 0;
                    end
                end
            end
            if (gb_underflow) begin
                $display("FAIL(%s): gearbox underflow flagged", name);
                errors = errors + 1;
            end
            $display("phase %-8s: %0d words, lane0 %0d symbols verified%s",
                     name, word_cnt, chk_cnt[0],
                     (errors == 0 && checked_ok) ? "" : "  (with errors)");
        end
    endtask

    function automatic [3:0] active_mask_of(input [2:0] lanes);
        case (lanes)
            3'd1:    active_mask_of = 4'b0001;
            3'd2:    active_mask_of = 4'b0011;
            default: active_mask_of = 4'b1111;
        endcase
    endfunction

    initial begin
        $dumpfile("dp_dphy_tx.vcd");
        $dumpvars(0, test_dp_dphy_tx);

        errors     = 0;
        link_en    = 1'b0;
        lane_count = 3'd4;
        rst_n      = 1'b0;
        reset_scoreboard;
        repeat (5) @(posedge clk);
        rst_n = 1'b1;
        repeat (3) @(posedge clk);

        // Phase 1: 4-lane, long continuous run
        run_phase(3'd4, 25000, 20000, "4-lane");

        // Phase 2: restart in 1-lane mode (lanes 1-3 must stay quiet)
        run_phase(3'd1, 8000, 6000, "1-lane");

        // Phase 3: 2-lane
        run_phase(3'd2, 8000, 6000, "2-lane");

        if (errors == 0)
            $display("PASS: all phases clean (continuous stream, symbols intact, no underflow)");
        else
            $display("FAIL: %0d error(s)", errors);
        $finish;
    end

endmodule
