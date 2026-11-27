// Bench of gs_accel_trace alone: a stream of socket cycles shaped like a program's
// (opcode fetches, operands, data reads and writes, internal cycles, jumps, vector
// pulls, I/O pages), with the bench's own model of what is kept, of the trigger and
// of the ring; every entry read back through the page and index and compared.
// Run: boards/a2mega/hdl/twgs/sim/trace/run_gs_trace_sim.sh
`timescale 1ns/1ps
module tb_gs_trace;
    localparam int AW = 13, N = 1 << AW;
    logic clk = 0, rst_n = 0;
    always #4.762 clk = ~clk;

    logic        run = 1, issue_evt = 0, fall_evt = 0;
    logic [23:0] a = 0;
    logic        we_n = 1, vp_n = 1, vpa = 0, vda = 0;
    logic [7:0]  d_f = 0;
    logic        irq_f = 1, shadow6 = 0, freeze = 0;
    logic [23:0] rq = 0;
    logic [4:0]  idx = 0;
    logic [31:0] word, s0, s1, s2;
    logic [4:0]  flags;
    logic        frozen, trigd;

    gs_accel_trace #(.AW(AW)) dut (
        .clk(clk), .rst_n(rst_n), .run_i(run), .issue_evt_i(issue_evt), .fall_evt_i(fall_evt),
        .a_i(a), .we_n_i(we_n), .vp_n_i(vp_n), .vpa_i(vpa), .vda_i(vda), .d_f_i(d_f), .irq_f_i(irq_f),
        .shadow6_i(shadow6), .freeze_i(freeze), .rq_i(rq), .idx_i(idx),
        .word_o(word), .flags_o(flags), .frozen_o(frozen), .trigd_o(trigd),
        .stat0_o(s0), .stat1_o(s1), .stat2_o(s2)
    );

    int errors = 0;
    task automatic err(input string s);
        errors++; if (errors <= 20) $display("ERROR @%0t: %s", $time, s);
    endtask

    // ---- the bench's model
    logic [36:0] m_mem [0:N-1];
    int          m_wptr = 0, m_count = 0;
    bit          m_wrapped = 0, m_trigd = 0, m_auto = 0, m_popv = 0;
    int          m_post = 0, m_quiet = 0;
    logic [23:0] m_pop = 0, m_taddr = 0;
    logic [7:0]  m_cfg = 0, m_page = 0;
    longint      n_cyc = 0, n_kept = 0, n_hits = 0;
    initial for (int i = 0; i < N; i++) m_mem[i] = '0;

    function automatic int post_n(input logic [1:0] c);
        return c == 0 ? 32 : c == 1 ? 1024 : c == 2 ? 4096 : N - 64;
    endfunction

    task automatic model_cycle(input logic [36:0] e, input bit sh6, input bit frz);
        logic [23:0] ea; bit op, pull, seq, io, keep, hit, frozen_m; int dif;
        ea = e[31:8]; op = e[35] & e[36]; pull = ~e[34];
        if (m_cfg[2]) return;
        dif = (int'(ea[15:0]) - int'(m_pop[15:0])) & 16'hFFFF;
        seq = m_popv && ea[23:16] == m_pop[23:16] && dif >= 1 && dif <= 4;
        io  = ea[15:8] == 8'hC0 && (ea[23:17] == 7'h70 || (ea[23:17] == 7'h00 && !sh6));
        case (m_cfg[1:0])
            0: keep = 1; 1: keep = op; 2: keep = pull; default: keep = pull || (op && !seq);
        endcase
        case (m_cfg[5:4])
            0: hit = op && ea == m_taddr;
            1: hit = pull && sh6;
            2: hit = !io && m_quiet == 131071;
            default: hit = ea == m_taddr;
        endcase
        if (op) begin m_pop = ea; m_popv = 1; end
        if (io) m_quiet = 0; else if (m_quiet < 131072) m_quiet++;
        frozen_m = frz || m_auto;
        if (!m_cfg[3]) begin m_trigd = 0; m_auto = 0; end
        else if (!frozen_m) begin
            if (!m_trigd) begin if (hit) begin m_trigd = 1; m_post = post_n(m_cfg[7:6]); n_hits++; end end
            else if (keep) begin if (m_post == 0) m_auto = 1; else m_post--; end
        end
        if (keep && !frozen_m) begin
            m_mem[m_wptr] = e; m_wptr = (m_wptr + 1) % N; m_count++; n_kept++;
            if (m_wptr == 0) m_wrapped = 1;
        end
    endtask

    // ---- one socket cycle: issue, a while, the fall
    task automatic cycle(input logic [23:0] ca, input bit c_we_n, input bit c_vp_n, input bit c_vpa, input bit c_vda,
                         input logic [7:0] cd, input bit c_irq);
        int gap;
        // the issue clk: the four are valid in it; the address register takes it at its end
        @(negedge clk); issue_evt = 1; we_n = c_we_n; vp_n = c_vp_n; vpa = c_vpa; vda = c_vda;
        @(posedge clk); #0.1; a = ca;
        @(negedge clk); issue_evt = 0; we_n = 1'bx; vp_n = 1'bx; vpa = 1'bx; vda = 1'bx;
        gap = 6 + $urandom_range(0, 6);
        repeat (gap) @(negedge clk);
        // the pads at the PHI2 fall, a clk and more before the fall event
        d_f = cd; irq_f = c_irq;
        repeat (2) @(negedge clk);
        fall_evt = 1;
        @(negedge clk); fall_evt = 0;
        if (run) begin
            model_cycle({c_vda, c_vpa, c_vp_n, c_irq, c_we_n, ca, cd}, shadow6, freeze);
            n_cyc++;
        end
        // the next issue is two clks after the fall at the earliest
        @(negedge clk);
    endtask

    // (the freeze is a register the firmware writes: it takes effect on the cycles whose
    // entry has not been written yet, so the bench moves it clear of one - four clks after
    // the last fall, and the next cycle after that)
    task automatic set_freeze(input bit v);
        repeat (4) @(negedge clk);
        freeze = v;
        @(negedge clk);
    endtask

    // ---- a program-shaped stream
    bit sh_toggle = 1;
    logic [23:0] pc = 24'h02_8000;
    int io_pct = 5, jump_pct = 15, int_pct = 2;
    task automatic instruction();
        int len, k;
        if ($urandom_range(0, 99) < int_pct) begin
            cycle(pc, 1, 1, 0, 0, 8'($urandom), $urandom_range(0, 3) == 0 ? 0 : 1);            // internal
            cycle(24'h00_01F0 + 24'($urandom_range(0, 15)), 0, 1, 0, 1, 8'($urandom), 0);  // push
            cycle(24'h00_FFEE, 1, 0, 0, 1, 8'h74, 0);
            cycle(24'h00_FFEF, 1, 0, 0, 1, 8'hC0, 0);
            pc = {8'($urandom_range(0, 3)), 16'($urandom)};
        end
        len = $urandom_range(1, 4);
        cycle(pc, 1, 1, 1, 1, 8'($urandom), 8'($urandom_range(0, 9)) != 0);
        for (k = 1; k < len; k++) cycle(pc + 24'(k), 1, 1, 1, 0, 8'($urandom), 1);
        if ($urandom_range(0, 2) == 0) begin
            logic [23:0] da;
            da = ($urandom_range(0, 99) < io_pct) ? {7'h70, 1'($urandom), 8'hC0, 8'($urandom)}
               : ($urandom_range(0, 99) < io_pct) ? {7'h00, 1'($urandom), 8'hC0, 8'($urandom)}
               : {8'($urandom_range(0, 3)), 16'($urandom)};
            cycle(da, 1'($urandom), 1, 0, 1, 8'($urandom), 1);
        end
        if ($urandom_range(0, 4) == 0) cycle(pc + 24'(len - 1), 1, 1, 0, 0, 8'($urandom), 1);
        if ($urandom_range(0, 99) < jump_pct)
            pc = ($urandom_range(0, 3) == 0) ? pc + 24'($urandom_range(0, 6)) - 24'd1 : {pc[23:16], 16'($urandom)};
        else
            pc = {pc[23:16], 16'(pc[15:0] + 16'(len))};
        if (sh_toggle && $urandom_range(0, 199) == 0) begin @(negedge clk); shadow6 = ~shadow6; repeat (3) @(negedge clk); end
    endtask

    // ---- the window registers, written a byte at a time as the firmware does
    task automatic set_reg(input int r, input logic [7:0] v);
        @(negedge clk);
        // (bits of one byte may arrive in different clks through the synchronisers)
        rq[8 * (r - 29) +: 4] = v[3:0];
        @(negedge clk);
        rq[8 * (r - 29) + 4 +: 4] = v[7:4];
        repeat ($urandom_range(2, 30)) @(negedge clk);
    endtask
    task automatic load_slot(input int n, input logic [15:0] v);
        set_reg(31, 8'h00); set_reg(29, v[7:0]); set_reg(30, v[15:8]); set_reg(31, 8'h50 + 8'(n));
        repeat (24) @(negedge clk);
        case (n)
            0: m_taddr[15:0] = v;
            1: begin m_cfg = v[15:8]; m_taddr[23:16] = v[7:0]; end
            2: m_page = v[7:0];
            default: ;
        endcase
    endtask
    task automatic configure(input logic [7:0] c, input logic [23:0] ta);
        load_slot(0, ta[15:0]);
        load_slot(1, {c | 8'h04, ta[23:16]});       // with the clear
        m_wptr = 0; m_count = 0; m_wrapped = 0; m_trigd = 0; m_auto = 0; m_popv = 0; m_quiet = 0; m_post = 0;
        load_slot(1, {c, ta[23:16]});
        set_reg(31, 8'h00);
    endtask

    // ---- read the whole ring back, or the pages asked
    task automatic check_state(input string why);
        @(negedge clk);
        if (s0 !== {freeze | m_auto, m_trigd, m_wrapped, 5'b0, m_cfg, 16'(m_wptr)})
            err($sformatf("%s: stat0 %08h, model %08h", why, s0, {freeze | m_auto, m_trigd, m_wrapped, 5'b0, m_cfg, 16'(m_wptr)}));
        if (s1 !== {m_page, m_taddr}) err($sformatf("%s: stat1 %08h, model %08h", why, s1, {m_page, m_taddr}));
        if (s2 !== 32'(m_count)) err($sformatf("%s: kept %0d, model %0d", why, s2, m_count));
        if (frozen !== (freeze | m_auto) || trigd !== m_trigd) err($sformatf("%s: frozen %b triggered %b, model %b %b", why, frozen, trigd, freeze | m_auto, m_trigd));
    endtask
    longint n_read = 0;
    task automatic read_back(input string why, input int page_step);
        int base, bad;
        logic [36:0] want;
        bad = 0;
        check_state(why);
        base = m_wrapped ? m_wptr : 0;
        set_reg(31, 8'h00); set_reg(30, 8'h00); set_reg(31, 8'h52);
        for (int p = 0; p < 256; p += page_step) begin
            set_reg(29, 8'(p)); m_page = 8'(p);
            repeat (20) @(negedge clk);
            for (int i = 0; i < 32; i++) begin
                idx = 5'(i);
                repeat (4) @(negedge clk);
                want = m_mem[(base + p * 32 + i) % N];
                if ({flags, word} !== want) begin
                    bad++;
                    if (bad <= 5) err($sformatf("%s: page %0d index %0d reads %010h, model %010h", why, p, i, {flags, word}, want));
                end
                n_read++;
            end
        end
        set_reg(31, 8'h00);
        if (bad != 0) err($sformatf("%s: %0d entries differ", why, bad));
    endtask

    task automatic run_cycles(input int n);
        longint stop;
        stop = n_cyc + n;
        while (n_cyc < stop) instruction();
    endtask
    task automatic run_until_frozen(input string why, input int max);
        longint stop;
        stop = n_cyc + max;
        while (!m_auto && n_cyc < stop) instruction();
        if (!m_auto) err($sformatf("%s: the model's trigger did not stop the ring in %0d cycles", why, max));
        repeat (8) @(negedge clk);
        if (!frozen) err($sformatf("%s: the ring did not stop", why));
        run_cycles(300);                              // nothing more may be kept
    endtask

    initial begin
        if ($test$plusargs("vcd")) begin $dumpfile("gs_trace.vcd"); $dumpvars(0, tb_gs_trace); end
        repeat (5) @(negedge clk); rst_n = 1; repeat (5) @(negedge clk);

        // 1. as it powers up: every cycle, no trigger; frozen by hand
        run_cycles(700);
        set_freeze(1); run_cycles(100); read_back("power-up, 700 cycles", 8);
        set_freeze(0);
        // 2. round the ring
        run_cycles(9500);
        set_freeze(1); run_cycles(50); read_back("round the ring", 1);
        set_freeze(0);
        // 3. the core not running: nothing kept
        run = 0; repeat (200) instruction(); run = 1;
        set_freeze(1); read_back("not running", 16); set_freeze(0);
        // 4. what is kept
        configure(8'h01, 24'h0); run_cycles(30000); set_freeze(1); read_back("opcode fetches", 1); set_freeze(0);
        int_pct = 6;
        configure(8'h02, 24'h0); run_cycles(40000); set_freeze(1); read_back("vector pulls", 4); set_freeze(0);
        configure(8'h03, 24'h0); run_cycles(60000); set_freeze(1); read_back("flow", 1); set_freeze(0);
        int_pct = 2;
        // 5. triggers
        //    an opcode fetch at an address (the stream is sent there by hand), 1024 after
        configure(8'h48, 24'h01_1234); run_cycles(12000);
        if (m_trigd) $display("note: trigger address reached by chance");
        pc = 24'h01_1234; run_until_frozen("opcode at address", 20000); read_back("opcode at address", 1);
        //    a data cycle at that address is not an opcode fetch
        configure(8'h08, 24'h03_0042); run_cycles(3000);
        cycle(24'h03_0042, 1, 1, 0, 1, 8'h11, 1); run_cycles(500);
        if (m_trigd || trigd) err("a data cycle fired the opcode trigger");
        //    any cycle at the address, 32 after, opcode fetches only kept
        configure(8'h39, 24'h03_0042); run_cycles(3000);
        cycle(24'h03_0042, 0, 1, 0, 1, 8'h22, 1); run_until_frozen("any cycle at address", 20000); read_back("any cycle at address", 4);
        //    a vector pull with shadow bit 6, 4096 after
        shadow6 = 0; int_pct = 0;
        configure(8'h98, 24'h0); run_cycles(9000);
        if (m_trigd) err("bench: a pull without bit 6 fired the model");
        shadow6 = 1; int_pct = 3; run_until_frozen("vector pull with bit 6", 60000); read_back("vector pull with bit 6", 1);
        int_pct = 2;
        //    no I/O page for 2**17 cycles, the ring less 64 after, flow kept
        io_pct = 5;
        configure(8'hEB, 24'h0); run_cycles(20000);
        if (m_trigd) err("bench: the quiet trigger fired with I/O in the stream");
        io_pct = 0; sh_toggle = 0; @(negedge clk); shadow6 = 1; repeat (3) @(negedge clk);   // (banks 00/01: page C0 is RAM)
        run_until_frozen("no I/O page", 700000); read_back("no I/O page", 1);
        //    ... and with every cycle kept and 32 after: the count to the cycle
        configure(8'h28, 24'h0); run_until_frozen("no I/O page, to the cycle", 200000); read_back("no I/O page, to the cycle", 1);
        io_pct = 5; sh_toggle = 1;
        //    disarmed: runs again without a clear
        load_slot(1, {8'h03, 8'h00}); set_reg(31, 8'h00);
        run_cycles(2000); check_state("disarmed");
        if (frozen) err("disarmed, still frozen");
        set_freeze(1); read_back("after disarm", 8); set_freeze(0);
        // 6. a slot's key seen for less than sixteen clks loads nothing
        @(negedge clk); rq = 24'h50_BE_EF; repeat (10) @(negedge clk); rq = 24'h00_00_00; repeat (30) @(negedge clk);
        check_state("short key");
        // 7. the accelerator's own key loads nothing
        rq = 24'hA5_00_3B; repeat (40) @(negedge clk); check_state("key A5"); rq = '0;

        $display("---- %0d cycles, %0d kept, %0d triggers, %0d entries read back", n_cyc, n_kept, n_hits, n_read);
        if (errors == 0) $display("PASS"); else $display("FAIL: %0d errors", errors);
        $finish;
    end
endmodule
