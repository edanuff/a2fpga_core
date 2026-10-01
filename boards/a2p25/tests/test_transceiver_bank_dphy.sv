// test_transceiver_bank_dphy.sv — integration TB for the a2p25 DP PHY:
// transceiver_bank_dphy + generated Gowin_MIPI_DPHY wrapper + Gowin PLLA
// simulation model (real clocks: 54 MHz -> VCO 810 -> 81 MHz symbol clock)
// + behavioural D-PHY hard-macro stand-in (sim_models/).
//
// Checks, per D-PHY data pin:
//   1. the serial stream is bit-exact with the 8b/10b codes the encoders
//      produced for the DP lane routed there (DP lane n -> D-PHY lane 3-n)
//   2. no FIFO overflow / gearbox underflow once streaming
//   3. TX_CLK_O comes up only after the reset release driven from the
//      symbol-clock domain (no tx_clk-dependent reset deadlock)
`timescale 1ps/1ps

module test_transceiver_bank_dphy;
    GSR GSR(.GSRI(1'b1));

    reg clk50 = 0, clk54 = 0, clk100 = 0;
    always #10000 clk50  = ~clk50;
    always #9259  clk54  = ~clk54;
    always #5000  clk100 = ~clk100;

    reg         reset = 1'b1;
    reg  [3:0]  powerup = 4'h0;
    wire [3:0]  running;
    wire        clk_sym;
    reg  [79:0] tx_symbols = 80'd0;
    wire [7:0]  status;
    wire        ckp, ckn;
    wire [3:0]  dp, dn;

    transceiver_bank_dphy #(
        .REVERSE_LANES     (1),
        .PROBE             (0),
        .DRV_SETTLE_CYCLES (20'd200),
        .HS_SETTLE_CYCLES  (20'd200)
    ) dut (
        .mgmt_clk        (clk100),
        .clk_dphy_ref    (clk54),
        .clk_md          (clk50),
        .reset           (reset),
        .powerup_channel (powerup),
        .tx_running      (running),
        .tx_symbol_clk   (clk_sym),
        .tx_symbols      (tx_symbols),
        .status          (status),
        .tx_clk_out      (),
        .dphy_ck_p       (ckp),
        .dphy_ck_n       (ckn),
        .dphy_d_p        (dp),
        .dphy_d_n        (dn)
    );

    // ---------------- stimulus: identifiable stream per DP lane ---------
    // sym = {force, K, d}; every 32nd pair carries K28.5 (0x1BC) in slot 0
    reg [7:0] cnt = 8'd0;
    integer l;
    always @(posedge clk_sym) begin
        cnt <= cnt + 8'd1;
        for (l = 0; l < 4; l = l + 1) begin
            tx_symbols[l*20 +: 10]      <= (cnt[4:0] == 5'd0) ? {1'b0, 9'h1BC}
                                                              : {1'b0, 1'b0, 8'(cnt * 3 + l * 64)};
            tx_symbols[l*20 + 10 +: 10] <= {1'b0, 1'b0, 8'(cnt ^ (8'h5A + l))};
        end
    end

    // ---------------- expected: encoder outputs per DP lane -------------
    localparam int EXP_MAX = 400000;
    localparam int OBS_MAX = 200000;
    reg  exp_bits [0:3][0:EXP_MAX-1];
    integer exp_n [0:3];
    reg  obs_bits [0:3][0:OBS_MAX-1];
    integer obs_n [0:3];
    integer b, j;
    initial for (j = 0; j < 4; j = j + 1) begin exp_n[j] = 0; obs_n[j] = 0; end

    always @(posedge clk_sym) begin
        if (dut.enc_run) begin
            for (l = 0; l < 4; l = l + 1)
                for (b = 0; b < 20; b = b + 1)
                    if (exp_n[l] < EXP_MAX) begin
                        exp_bits[l][exp_n[l]] = dut.code[l][b];
                        exp_n[l] = exp_n[l] + 1;
                    end
        end
    end

    // ---------------- observed: sample each pin mid-bit -----------------
    // Bits change on CK0 edges inside the model; sample 300 ps after each
    // edge (bit period 617 ps).
    wire ck0 = dut.u_dphy.clkout0_o;
    always @(ck0) begin
        #300;
        if (dut.phy_active)
            for (j = 0; j < 4; j = j + 1)
                if (obs_n[j] < OBS_MAX) begin
                    obs_bits[j][obs_n[j]] = dp[j];
                    obs_n[j] = obs_n[j] + 1;
                end
    end

    // ---------------- checks --------------------------------------------
    integer errors = 0;
    always @(posedge dut.ovf_sticky) $display("[%0t] overflow set, level=%0d", $time, dut.level_r);
    always @(posedge dut.unf_sticky) $display("[%0t] underflow set, level=%0d", $time, dut.level_r);
    always @(posedge dut.gb_underflow) $display("[%0t] shim gb_underflow set", $time);
    always @(posedge dut.primed) $display("[%0t] primed, level=%0d", $time, dut.level_r);
    reg     txclk_seen_before_drst = 1'b0;
    always @(posedge dut.tx_clk) if (!dut.dphy_drst_n) txclk_seen_before_drst = 1'b1;

    function automatic integer find_offset(input integer pin, input integer lane);
        integer o, k, ok;
        begin
            find_offset = -1;
            for (o = 0; o + 200 < exp_n[lane] && find_offset < 0; o = o + 1) begin
                ok = 1;
                for (k = 0; k < 200 && ok; k = k + 1)
                    if (obs_bits[pin][1000 + k] !== exp_bits[lane][o + k]) ok = 0;
                if (ok) find_offset = o - 1000;
            end
        end
    endfunction

    integer pin, lane, off, k, cmp, mism;
    initial begin
        #200_000;
        reset = 1'b0;
        #100_000;
        powerup = 4'hF;

        wait (dut.phy_active);
        $display("[%0t] phy_active; status=%b running=%b", $time, status, running);
        #600_000_000;   // 600 us of streaming

        $display("status=%b (drst_n ovf unf active link enc alive 0)", status);
        if (status[6]) begin $display("FAIL: FIFO overflow"); errors++; end
        if (status[5]) begin $display("FAIL: gearbox/FIFO underflow"); errors++; end
        if (txclk_seen_before_drst) begin $display("FAIL: model tx_clk ran before drst"); errors++; end
        if (running !== 4'hF) begin $display("FAIL: tx_running=%b", running); errors++; end

        for (pin = 0; pin < 4; pin = pin + 1) begin
            lane = 3 - pin;
            off  = find_offset(pin, lane);
            if (off < 0) begin
                $display("FAIL: pin d%0d: no alignment with DP lane %0d (obs %0d bits)", pin, lane, obs_n[pin]);
                errors++;
            end else begin
                mism = 0; cmp = 0;
                for (k = 16; k < obs_n[pin] - 40 && off + k < exp_n[lane]; k = k + 1) begin
                    cmp = cmp + 1;
                    if (obs_bits[pin][k] !== exp_bits[lane][off + k]) begin
                        if (mism < 4) $display("  mismatch pin d%0d at obs bit %0d", pin, k);
                        mism = mism + 1;
                    end
                end
                $display("pin d%0d <- DP lane %0d: offset %0d, %0d bits compared, %0d mismatches",
                         pin, lane, off, cmp, mism);
                if (mism != 0 || cmp < 50000) errors++;
            end
        end

        if (errors == 0) $display("PASS: all 4 lanes bit-exact, lane map reversed, no FIFO events");
        else             $display("FAIL: %0d error(s)", errors);
        $finish;
    end

    initial begin
        #5_000_000_000;
        $display("FAIL: timeout (phy never became active)");
        $finish;
    end
endmodule
