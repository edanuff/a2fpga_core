//
// Behavioural model of the AP Memory APS6408L-3OBMx (64 Mbit, x8, 3.0 V)
// Xccela / OPI DDR PSRAM, written from the Rev 4.0 (2023-06-27) datasheet.
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES.
//
// Not a vendor model. It implements what hyperram_ctrl relies on and checks
// what the datasheet says the host must respect:
//
//   - Command/address: INST on the 1st CLK rising edge (held for the whole
//     clock), A3/A2 on the 2nd rising/falling edge, A1/A0 on the 3rd
//     (datasheet §8.3, Table "Command Truth Table"). Commands: 00h sync
//     read, 80h sync write, 20h linear read, A0h linear write, 40h MRR,
//     C0h MRW, FFh Global Reset. Any other INST is counted in undef_inst
//     and ignored.
//   - Read: DQS Hi-Z through CA, driven low tCQLZ after the 3rd rising
//     edge (preamble), first data on the rising edge LC clocks after that
//     edge, i.e. CLK#(3+LC); DQS toggles with the data, rising = even byte.
//     Variable latency (MR0[5]=0): a read whose CE# falls inside a refresh
//     window is pushed out by 1..LC extra clocks (PUSHOUT_HALF=1: in
//     half-clocks). Fixed latency (MR0[5]=1): always 2*LC. tDQSCK after
//     the CLK edge, DQ tDQSQ after DQS (§8.5, Figures 5-9, Table 29).
//   - Write: first data byte sampled on CLK#(3+WLC) (MR4[7:5]); no
//     pushout; DQS/DM = 1 masks the byte (§8.6, Figures 10-11, Table 6).
//   - MRW: MA on the 3rd falling edge, data byte on the 4th rising edge
//     ("latency 1"); MRR: MA on the 3rd falling edge, returns MR[MA],
//     MR[next] alternating after LC (§8.7, Figures 12-13).
//   - Bursts wrap inside the 1 KB column page for linear commands
//     (Table 18). Wrapped commands wrap per MR8[1:0] (hybrid bit
//     ignored). RBX is not modelled. page_wraps counts linear WRITE bursts
//     that wrapped (the controller must never do that); read_overruns
//     counts reads clocked past the page end (the controller over-clocks
//     reads on purpose, so that is expected).
//   - Global Reset: INST FFh, CE# low for >= 4 clocks; registers reset,
//     tRST 2 us before the next command. Power-up: tPU 150 us; with
//     REQUIRE_RESET (parameter or +XC_COLD) the device ignores every
//     command until a RESET# pulse or a Global Reset (datasheet §7
//     "Phase 2").
//   - Timing checks at the pins (-7 grade, Table 28/29): tCLK >= 7.5 ns,
//     tCSP >= 2.5 ns, tCHD >= 2.5 ns, tCSP2 >= 1.5 ns, tCPH >= 18 ns,
//     tRC >= 60 ns, tCEM <= 8 us, CE# low >= 3 clocks, tSP/tHD and
//     tDS/tDH >= 1.1 ns, A[0] = 0, write burst >= 2 bytes.
//
// Hierarchical hooks for a harness: Dout_zd / DQSout_zd (the device's
// drivers, z when idle), Mem (bytes), mr0..mr8, refresh_pulse,
// timing_errors, undef_inst, page_wraps, ignored_cmds, reset_count.
//

`timescale 1ns / 1ps

module aps6408l_model #(
    parameter         REQUIRE_RESET     = 0,
    parameter integer MEM_BYTES         = 8 * 1024 * 1024,
    parameter real    T_PU_NS           = 150_000.0,
    parameter real    T_RST_NS          = 2_000.0,
    parameter real    T_DQSCK           = 4.0,     // 2 .. 5.5 ns
    parameter real    T_DQSQ            = 0.3,     // <= 0.6 ns
    parameter real    T_CQLZ            = 3.0,     // 1 .. 6 ns
    parameter real    T_HZ              = 4.0,     // <= 6 ns
    parameter real    REFRESH_PERIOD_NS = 4036.0,  // same cadence as the S27KL model
    parameter real    REFRESH_WIN_NS    = 160.0,   // reads whose CE# falls in here collide
    parameter         PUSHOUT_HALF      = 0        // 1: pushout in half-clocks
) (
    inout  wire [7:0] DQ,
    inout  wire       DQS,
    input  wire       CE_n,
    input  wire       CLK,
    input  wire       RESET_n
);

    // ---------------------------------------------------------------------
    // Drivers and storage
    // ---------------------------------------------------------------------
    reg  [7:0] Dout_zd   = 8'hzz;
    reg        DQSout_zd = 1'bz;
    assign DQ  = Dout_zd;
    assign DQS = DQSout_zd;

    reg  [7:0] Mem [0:MEM_BYTES-1];

    reg  [7:0] mr0, mr1, mr2, mr3, mr4, mr8;

    integer timing_errors = 0;
    integer undef_inst    = 0;
    integer page_wraps    = 0;             // write bursts that wrapped inside the 1 KB page
    integer read_overruns = 0;             // read bursts clocked past the page end (harmless)
    integer ignored_cmds  = 0;
    integer reset_count   = 0;
    integer n_reads = 0, n_writes = 0, n_pushout = 0, n_mrw = 0, n_mrr = 0;

    logic   require_reset;
    logic   powered    = 1'b0;
    logic   reset_done = 1'b0;
    real    t_ready    = 0.0;      // commands accepted from here on (tRST)
    logic   refresh_pulse = 1'b0;
    real    t_refresh  = -1.0e9;

    task automatic reset_regs;
        begin
            mr0 = 8'h09;   // LT variable, RLC 5, drive 1/4 (100 ohm)
            mr1 = 8'h0D;   // vendor APM
            mr2 = 8'h93;   // good die, generation 3, 64 Mb
            mr3 = 8'hC0;   // RBX supported, 3 V
            mr4 = 8'h40;   // WLC 5, fast refresh, full array
            mr8 = 8'h05;   // 32 B hybrid wrap
        end
    endtask

    initial begin
        require_reset = REQUIRE_RESET || $test$plusargs("XC_COLD");
        reset_regs();
        #(T_PU_NS);
        powered = 1'b1;
        if (!require_reset) begin
            reset_done = 1'b1;
            t_ready    = $realtime;
        end
    end

    // Refresh cadence (hook for collision hunting)
    always begin
        #(REFRESH_PERIOD_NS - REFRESH_WIN_NS);
        refresh_pulse = 1'b1;
        t_refresh = $realtime;
        #(REFRESH_WIN_NS);
        refresh_pulse = 1'b0;
    end

    // RESET# pin
    real t_reset_fall = -1.0e9;
    always @(negedge RESET_n) t_reset_fall = $realtime;
    always @(posedge RESET_n) begin
        if (powered && ($realtime - t_reset_fall) >= 1000.0) begin
            reset_regs();
            reset_done  = 1'b1;
            reset_count = reset_count + 1;
            t_ready     = $realtime + T_RST_NS;
        end
    end

    // ---------------------------------------------------------------------
    // Latency decode
    // ---------------------------------------------------------------------
    function automatic integer lc_of(input [2:0] code);
        case (code)
            3'b000: lc_of = 3;
            3'b001: lc_of = 4;
            3'b010: lc_of = 5;
            3'b011: lc_of = 6;
            3'b100: lc_of = 7;
            default: lc_of = 5;
        endcase
    endfunction

    function automatic integer wlc_of(input [2:0] code);
        case (code)
            3'b000: wlc_of = 3;
            3'b100: wlc_of = 4;
            3'b010: wlc_of = 5;
            3'b110: wlc_of = 6;
            3'b001: wlc_of = 7;
            default: wlc_of = 5;
        endcase
    endfunction

    function automatic [7:0] mr_read(input [7:0] ma);
        case (ma)
            8'h00: mr_read = mr0;
            8'h01: mr_read = mr1;
            8'h02: mr_read = mr2;
            8'h03: mr_read = mr3;
            8'h04: mr_read = mr4;
            8'h08: mr_read = mr8;
            default: mr_read = 8'h00;
        endcase
    endfunction

    // MRR returns MR[MA] then the next register (Figure 12)
    function automatic [7:0] mr_next(input [7:0] ma);
        case (ma)
            8'h00: mr_next = 8'h01;
            8'h01: mr_next = 8'h02;
            8'h02: mr_next = 8'h03;
            8'h03: mr_next = 8'h04;
            8'h04: mr_next = 8'h08;
            default: mr_next = 8'h00;
        endcase
    endfunction

    // ---------------------------------------------------------------------
    // Timing checks
    // ---------------------------------------------------------------------
    localparam real T_CLK  = 7.5;
    localparam real T_CSP  = 2.5;
    localparam real T_CHD  = 2.5;
    localparam real T_CSP2 = 1.5;
    localparam real T_CPH  = 18.0;
    localparam real T_RC   = 60.0;
    localparam real T_CEM  = 8000.0;
    localparam real T_SP   = 1.1;
    localparam real T_DS   = 1.1;

    task automatic terr(input string what, input real val);
        begin
            timing_errors = timing_errors + 1;
            if (timing_errors <= 20)
                $display("[%0t] APS6408L TIMING %s violated (%0.3f ns)", $time, what, val);
        end
    endtask

    real min_tcsp = 1.0e9, min_tchd = 1.0e9, min_tcph = 1.0e9, min_trc = 1.0e9, max_tcem = 0.0;
    real min_tds = 1.0e9, min_tdh = 1.0e9;

    // ---------------------------------------------------------------------
    // Transaction state
    // ---------------------------------------------------------------------
    typedef enum integer { K_NONE, K_READ, K_WRITE, K_MRR, K_MRW, K_GBRST, K_IGN } kind_t;

    kind_t   kind = K_NONE;
    integer  e = 0;                    // CLK edges since CE# fell (1 = first rising)
    integer  tid = 0;                  // transaction id (invalidates scheduled outputs)
    integer  lc, wlc;
    integer  e_data;                   // edge index of the first data byte
    integer  bytes;                    // bytes transferred
    logic [7:0] inst;
    logic [31:0] addr, cur_addr;
    logic [7:0]  ma;
    logic        linear;
    integer      wrap_len;
    logic        fixed_lat;
    logic        pushout;
    real     t_ce_fall = -1.0e9, t_ce_rise = -1.0e9, t_last_fall = -1.0e9, t_last_rise = -1.0e9;
    real     t_dq_chg = -1.0e9, t_dqs_chg = -1.0e9;
    logic    first_ce = 1'b1;
    logic    collide;                  // refresh in progress when CE# fell
    logic    wrap_pending;             // burst address wrapped; counts if another byte follows
    logic    accepted;                 // device is executing this command

    wire [7:0] din  = DQ;
    wire       dmin = DQS;

    always @(DQ)  t_dq_chg  = $realtime;
    always @(DQS) t_dqs_chg = $realtime;

    // Scheduled output: DQS at +T_DQSCK, DQ T_DQSQ later. Plain delayed
    // nonblocking assignments: every data event lands within ~5 ns of its
    // CLK edge, long before CE# can legally rise (tCHD + the controller's
    // CK-low cycle), so no event outlives its transaction.
    task automatic out_byte(input [7:0] d, input logic s);
        begin
            DQSout_zd <= #(T_DQSCK) s;
            Dout_zd   <= #(T_DQSCK + T_DQSQ) d;
        end
    endtask

    task automatic release_outputs;
        begin
            Dout_zd   <= #(T_HZ) 8'hzz;
            DQSout_zd <= #(T_HZ) 1'bz;
        end
    endtask

    function automatic [31:0] next_addr(input [31:0] a);
        logic [31:0] n;
        begin
            if (linear) begin
                n = {a[31:10], a[9:0] + 10'd1};
            end else begin
                case (wrap_len)
                    16:   n = {a[31:4],  a[3:0] + 4'd1};
                    32:   n = {a[31:5],  a[4:0] + 5'd1};
                    64:   n = {a[31:6],  a[5:0] + 6'd1};
                    default: n = {a[31:10], a[9:0] + 10'd1};
                endcase
            end
            next_addr = n;
        end
    endfunction

    // ---- CE# ----
    always @(negedge CE_n) begin
        if (!first_ce) begin
            if (($realtime - t_ce_rise) < T_CPH) terr("tCPH", $realtime - t_ce_rise);
            if (($realtime - t_ce_rise) < min_tcph) min_tcph = $realtime - t_ce_rise;
            if (($realtime - t_ce_fall) < T_RC)  terr("tRC", $realtime - t_ce_fall);
            if (($realtime - t_ce_fall) < min_trc) min_trc = $realtime - t_ce_fall;
        end
        if (CLK !== 1'b0) terr("CLK not low at CE# fall", 0.0);
        t_ce_fall = $realtime;
        tid       = tid + 1;
        e         = 0;
        kind      = K_NONE;
        bytes     = 0;
        e_data    = 0;
        pushout   = 1'b0;
        collide   = refresh_pulse || (($realtime - t_refresh) < REFRESH_WIN_NS);
        wrap_pending = 1'b0;
        accepted  = powered && ($realtime >= t_ready);   // reset_done checked per INST
    end

    always @(posedge CE_n) begin
        if (!first_ce || t_ce_fall > 0.0) begin
            if (($realtime - t_last_fall) < T_CHD && t_last_fall > t_ce_fall)
                terr($sformatf("tCHD (kind %0d, %0d edges, %0d bytes)", kind, e, bytes), $realtime - t_last_fall);
            if (t_last_fall > t_ce_fall && ($realtime - t_last_fall) < min_tchd)
                min_tchd = $realtime - t_last_fall;
            if (($realtime - t_ce_fall) > T_CEM) terr("tCEM", $realtime - t_ce_fall);
            if (($realtime - t_ce_fall) > max_tcem) max_tcem = $realtime - t_ce_fall;
            if (e < 6 && accepted && kind != K_NONE && kind != K_IGN)
                terr("CE# low < 3 clocks", e / 2.0);
            if (CLK !== 1'b0) terr("CLK not low at CE# rise", 0.0);
        end
        // finish the command
        if (accepted) begin
            case (kind)
                K_WRITE: begin
                    if (bytes < 2) terr("write burst < 2 bytes", bytes);
                    n_writes = n_writes + 1;
                end
                K_READ:  n_reads = n_reads + 1;
                K_MRR:   n_mrr = n_mrr + 1;
                K_MRW:   n_mrw = n_mrw + 1;
                K_GBRST: begin
                    if (e >= 7) begin
                        reset_regs();
                        reset_done  = 1'b1;
                        reset_count = reset_count + 1;
                        t_ready     = $realtime + T_RST_NS;
                    end else begin
                        terr("Global Reset frame < 4 clocks", e / 2.0);
                    end
                end
                default: ;
            endcase
        end
        t_ce_rise = $realtime;
        first_ce  = 1'b0;
        kind      = K_NONE;
        release_outputs();
    end

    // ---- CLK edges ----
    task automatic input_checks(input logic data_phase);
        begin
            if (($realtime - t_dq_chg) < (data_phase ? T_DS : T_SP))
                terr(data_phase ? "tDS DQ" : "tSP", $realtime - t_dq_chg);
            if (data_phase && ($realtime - t_dq_chg) < min_tds)
                min_tds = $realtime - t_dq_chg;
            if (data_phase && ($realtime - t_dqs_chg) < T_DS)
                terr("tDS DM", $realtime - t_dqs_chg);
        end
    endtask

    task automatic hold_check(input real t_edge, input logic data_phase);
        begin
            // called on the next DQ change: hold from the previous edge
            if ((t_dq_chg - t_edge) < 1.1 && (t_dq_chg - t_edge) >= 0.0)
                terr(data_phase ? "tDH DQ" : "tHD", t_dq_chg - t_edge);
        end
    endtask

    real t_edge_prev = -1.0e9;
    logic in_input_phase = 1'b0;      // DQ is an input the device samples
    always @(DQ) begin
        if (CE_n === 1'b0 && in_input_phase && t_edge_prev > t_ce_fall &&
            ($realtime - t_edge_prev) < 1.1 && ($realtime - t_edge_prev) >= 0.0 &&
            kind != K_READ && kind != K_MRR)
            terr("tHD/tDH DQ", $realtime - t_edge_prev);
        if (CE_n === 1'b0 && in_input_phase && t_edge_prev > t_ce_fall &&
            ($realtime - t_edge_prev) >= 0.0 && ($realtime - t_edge_prev) < min_tdh &&
            kind != K_READ && kind != K_MRR)
            min_tdh = $realtime - t_edge_prev;
    end

    task automatic handle_edge(input logic rising);
        logic [7:0] b;
        integer     k;
        begin
            e = e + 1;
            t_edge_prev = $realtime;
            if (!accepted) begin
                if (e == 1) begin
                    ignored_cmds = ignored_cmds + 1;
                    kind = K_IGN;
                end
                return;
            end
            case (e)
                1: begin
                    input_checks(1'b0);
                    in_input_phase = 1'b1;
                    inst = din;
                    if (CE_n === 1'b0 && ($realtime - t_ce_fall) < T_CSP) terr("tCSP", $realtime - t_ce_fall);
                    if (($realtime - t_ce_fall) < min_tcsp) min_tcsp = $realtime - t_ce_fall;
                    lc  = lc_of(mr0[4:2]);
                    wlc = wlc_of(mr4[7:5]);
                    fixed_lat = mr0[5];
                    linear    = inst[5];
                    case (mr8[1:0])
                        2'b00: wrap_len = 16;
                        2'b01: wrap_len = 32;
                        2'b10: wrap_len = 64;
                        default: wrap_len = 1024;
                    endcase
                    if (!reset_done && inst != 8'hFF) begin
                        // Phase 2 pending: only Global Reset (or RESET#) is honoured
                        accepted = 1'b0;
                        ignored_cmds = ignored_cmds + 1;
                        kind = K_IGN;
                        return;
                    end
                    case (inst)
                        8'h00, 8'h20: kind = K_READ;
                        8'h80, 8'hA0: kind = K_WRITE;
                        8'h40: kind = K_MRR;
                        8'hC0: kind = K_MRW;
                        8'hFF: kind = K_GBRST;
                        default: begin
                            kind = K_IGN;
                            undef_inst = undef_inst + 1;
                            $display("[%0t] APS6408L: undefined INST %02h ignored", $time, inst);
                        end
                    endcase
                end
                3: begin input_checks(1'b0); addr[31:24] = din; end
                4: begin input_checks(1'b0); addr[23:16] = din; end
                5: begin
                    input_checks(1'b0);
                    addr[15:8] = din;
                    if (kind == K_READ || kind == K_MRR) begin
                        // preamble: DQS driven low tCQLZ after the 3rd rising edge
                        DQSout_zd <= #(T_CQLZ) 1'b0;
                        // data edge: rising edge number 3 + LC  -> e = 2*(3+LC) - 1
                        e_data = 2 * (3 + lc) - 1;
                        if (fixed_lat) begin
                            e_data = e_data + 2 * lc;
                        end else if (collide) begin
                            pushout = 1'b1;
                            n_pushout = n_pushout + 1;
                            if (PUSHOUT_HALF) e_data = e_data + 1 + ($urandom % (2 * lc));
                            else              e_data = e_data + 2 * (1 + ($urandom % lc));
                        end
                    end else if (kind == K_WRITE) begin
                        e_data = 2 * (3 + wlc) - 1;
                    end
                end
                6: begin
                    input_checks(1'b0);
                    addr[7:0] = din;
                    ma = din;
                    cur_addr = addr;
                    in_input_phase = (kind == K_WRITE || kind == K_MRW);
                    if ((kind == K_READ || kind == K_WRITE) && addr[0] !== 1'b0)
                        terr("A[0] != 0", 0.0);
                    if ((kind == K_READ || kind == K_WRITE) && addr[22:1] >= (MEM_BYTES / 2))
                        $display("[%0t] APS6408L: address %08h beyond the array", $time, addr);
                end
                7: begin
                    if (kind == K_MRW) begin
                        input_checks(1'b1);
                        case (ma)
                            8'h00: mr0 = din & 8'h3F;
                            8'h04: mr4 = din & 8'hEF;
                            8'h08: mr8 = din & 8'h7F;
                            default: $display("[%0t] APS6408L: MRW to read-only/unknown MR %02h ignored", $time, ma);
                        endcase
                        in_input_phase = 1'b0;
                    end
                end
                default: ;
            endcase

            // ---- data phase ----
            if (e >= e_data && e_data > 6) begin
                case (kind)
                    K_READ: begin
                        if (wrap_pending) begin read_overruns = read_overruns + 1; wrap_pending = 1'b0; end
                        b = Mem[cur_addr[22:0]];
                        out_byte(b, (bytes[0] == 1'b0));
                        cur_addr = next_addr(cur_addr);
                        if (linear && cur_addr[9:0] == 10'd0) wrap_pending = 1'b1;
                        bytes = bytes + 1;
                    end
                    K_MRR: begin
                        out_byte(bytes[0] ? mr_read(mr_next(ma)) : mr_read(ma), (bytes[0] == 1'b0));
                        bytes = bytes + 1;
                    end
                    K_WRITE: begin
                        input_checks(1'b1);
                        if (wrap_pending) begin page_wraps = page_wraps + 1; wrap_pending = 1'b0; end
                        if (dmin !== 1'b1) begin
                            if (dmin !== 1'b0) terr("DM is X/Z during write data", 0.0);
                            Mem[cur_addr[22:0]] = din;
                        end
                        cur_addr = next_addr(cur_addr);
                        if (linear && cur_addr[9:0] == 10'd0) wrap_pending = 1'b1;
                        bytes = bytes + 1;
                    end
                    default: ;
                endcase
            end
        end
    endtask

    always @(posedge CLK) begin
        if (($realtime - t_last_rise) < T_CLK && t_last_rise > 0.0 && CE_n === 1'b0)
            terr("tCLK", $realtime - t_last_rise);
        if (CE_n === 1'b0) handle_edge(1'b1);
        t_last_rise = $realtime;
    end

    always @(negedge CLK) begin
        if (CE_n === 1'b0) handle_edge(1'b0);
        else if (($realtime - t_ce_rise) < T_CSP2 && ($realtime - t_ce_rise) >= 0.0)
            terr("tCSP2", $realtime - t_ce_rise);
        t_last_fall = $realtime;
    end

    initial begin : init_mem
        integer i;
        for (i = 0; i < MEM_BYTES; i = i + 1) Mem[i] = 8'hFF;
    end

endmodule
