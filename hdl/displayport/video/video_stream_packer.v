///////////////////////////////////////////////////////////////////////////////
// video_stream_packer.v : Generic pixel-to-main-link symbol stream packer
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Symbol-domain timing master. Consumes packed pixel words from the CDC
// FIFO (LANE_COUNT pixels per word, lane 0 in the low 24 bits) and emits
// the 73-bit main-link word stream (4 lanes x 2 symbols, bit 72 = safe
// switch point), replicating the line structure of the proven hand-coded
// test sources:
//
//   symbol 0 .. TU_SIZE-2   : dummy (start-of-line blank TU)
//   symbol TU_SIZE-1        : BE (active lines only)
//   symbol TU_SIZE ..       : transfer units - 'valid' data symbols then
//                             FS <fill> FE padding to TU_SIZE, valid count
//                             per TU from a Bresenham accumulator
//   symbol BS_POS           : BS  (same fixed position every line)
//   BS_POS+1 .. +3*sets     : VB-ID / Mvid / Maud, repeated 4/LANE_COUNT x
//   remainder               : dummy; on vblank lines the region before
//                             BS_POS allows idle-pattern switchover, the
//                             region after BS is flagged for SDP insertion
//
// Lane data is pixel-interleaved: lane N carries pixels N, N+LANE_COUNT, ..
// each as three symbols R, G, B (rgb[23:16] first).
//
// The FIFO pop (fifo_rd) is combinational: pops can occur on consecutive
// cycles, and the show-ahead head must advance between them.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module video_stream_packer #(
    parameter LANE_COUNT    = 2,     // 1, 2 or 4
    parameter H_VISIBLE     = 1280,
    parameter V_VISIBLE     = 720,
    parameter V_TOTAL       = 750,
    parameter TU_SIZE       = 64,    // symbols per transfer unit (even)
    parameter SYMS_PER_LINE = 3600,  // link symbols per lane per video line (even)
    parameter VALID_NUM     = 2112,  // Bresenham: valid syms/TU = VALID_NUM/VALID_DEN
    parameter VALID_DEN     = 48,    //   = TU_SIZE*3*PIXEL_CLK_MULT / (2*PIXEL_CLK_DIV*LANE_COUNT)
    parameter PREFILL       = 640    // FIFO words buffered before starting a frame
)(
    input             clk,           // tx_symbol_clk
    input             reset,
    // Per-line M values (Mvid low byte constant in sync-clock mode)
    input       [7:0] mvid_byte,     // = M_value[7:0]
    input       [7:0] maud_byte,     // audio M value low byte (Phase 3, else 0)
    input             audio_mute,    // VB-ID bit 4 (Phase 3, else 0)
    // Pixel FIFO read side (show-ahead)
    input [24*LANE_COUNT-1:0] fifo_rdata,
    input             fifo_rsof,     // head word is first of a frame
    input             fifo_rvalid,
    output            fifo_rd,
    input      [15:0] fifo_rlevel,
    output reg        capture_arm,   // tell pixel domain to start at next frame
    // Main link stream out
    output reg        ready,         // source_ready to the idle inserter
    output reg [72:0] data,
    // Sidebands for the SDP inserter (Phase 3)
    output reg        sdp_gap,       // safe hblank/tail region for secondary packets
    output reg        frame_pulse,   // one-cycle pulse entering the vertical blank
    output reg        underrun       // sticky error flag (FIFO ran dry mid-line)
);

    // ------------------------------------------------------------------
    // Symbols (9-bit: bit 8 = K-code flag)
    // ------------------------------------------------------------------
    localparam [8:0] DUMMY = 9'b000000011;  // 0x03
    localparam [8:0] BE    = 9'b111111011;  // K27.7 Blank End
    localparam [8:0] BS    = 9'b110111100;  // K28.5 Blank Start
    localparam [8:0] FS    = 9'b111111110;  // K30.7 Fill Start
    localparam [8:0] FE    = 9'b111110111;  // K23.7 Fill End

    // ------------------------------------------------------------------
    // Derived layout constants
    // ------------------------------------------------------------------
    localparam integer BYTES_PER_LANE  = H_VISIBLE * 3 / LANE_COUNT;  // per line
    localparam integer DATA_START      = TU_SIZE;                    // BE at DATA_START-1
    localparam integer VBID_SETS       = 4 / LANE_COUNT;             // VB-ID/Mvid/Maud repeats
    localparam integer CYCLES_PER_LINE = SYMS_PER_LINE / 2;

    // BS position: end of the (possibly partial) final TU - fixed per line
    function integer calc_bs_pos;
        input integer unused;
        integer bl, er, v, pos;
        begin
            bl = BYTES_PER_LANE; er = 0; pos = DATA_START;
            while (bl > 0) begin
                er = er + VALID_NUM;
                v  = er / VALID_DEN;
                er = er % VALID_DEN;
                if (v >= bl) begin
                    pos = pos + bl;      // partial final TU: BS follows last byte
                    bl  = 0;
                end else begin
                    pos = pos + TU_SIZE; // full TU incl. fill
                    bl  = bl - v;
                end
            end
            calc_bs_pos = pos;
        end
    endfunction

    localparam integer BS_POS    = calc_bs_pos(0);
    localparam integer VBID_END  = BS_POS + 3*VBID_SETS;             // last VB-ID seq symbol
    localparam integer GAP_START = BS_POS + 30;                      // clear of MSA window
    localparam integer GAP_END   = SYMS_PER_LINE - 2;

    // Bresenham strength reduction: valid/TU = VQUOT or VQUOT+1, chosen by
    // the error accumulator - no runtime divider needed
    localparam integer VQUOT = VALID_NUM / VALID_DEN;
    localparam integer VREM  = VALID_NUM % VALID_DEN;

    // ------------------------------------------------------------------
    // Registered state
    // ------------------------------------------------------------------
    reg [$clog2(CYCLES_PER_LINE)-1:0] line_cycle;
    reg [$clog2(V_TOTAL)-1:0]         line_num;
    reg                               running;

    reg [$clog2(TU_SIZE)-1:0]   tu_pos;
    reg [$clog2(TU_SIZE+1)-1:0] tu_valid;
    reg [$clog2(VALID_DEN)-1:0] bres_err;
    reg [1:0]                   phase;      // 0=R,1=G,2=B of current pixel word
    reg [24*LANE_COUNT-1:0]     cur_word;

    wire vb_flag        = (line_num >= V_VISIBLE-1);
    wire is_active_line = (line_num < V_VISIBLE);
    wire [7:0] vbid_byte = {3'b000, audio_mute, 3'b000, vb_flag};

    // ------------------------------------------------------------------
    // Region decode, precomputed one cycle early and registered so the
    // per-slot logic muxes on single flag bits instead of chaining wide
    // line_cycle comparisons (timing: the slot chain is the critical
    // path at HBR rates)
    // ------------------------------------------------------------------
    wire at_eol = (line_cycle == CYCLES_PER_LINE-1);
    wire [$clog2(CYCLES_PER_LINE)-1:0] nc =
        at_eol ? {$clog2(CYCLES_PER_LINE){1'b0}} : line_cycle + 1'b1;
    wire nl_active = at_eol ? ((line_num == V_TOTAL-1) || (line_num < V_VISIBLE-1))
                            : is_active_line;
    wire [13:0] s0n = {nc, 1'b0};
    wire [13:0] s1n = s0n + 1'b1;

    // VB-ID sequence position for a symbol index (registered, so the
    // constant-compare depth here is off the critical path)
    function [1:0] vsel(input [13:0] s);
        begin
            if (s > BS_POS && s <= VBID_END) begin
                if      (s == BS_POS+1 || s == BS_POS+4 ||
                         s == BS_POS+7 || s == BS_POS+10) vsel = 2'd1; // VB-ID
                else if (s == BS_POS+2 || s == BS_POS+5 ||
                         s == BS_POS+8 || s == BS_POS+11) vsel = 2'd2; // Mvid
                else                                      vsel = 2'd3; // Maud
            end else
                vsel = 2'd0;
        end
    endfunction

    reg [1:0] f_be, f_tu, f_bs;   // bit k = slot k
    reg [1:0] f_nl;               // slot is NOT the line's last data byte
    reg [1:0] f_vb [0:1];
    reg       f_prime;
    reg       start_ok;   // registered start decision (BSRAM sof output
                          // -> wide level compare is the slowest path in
                          // the device; it only matters once, at startup)

    // ------------------------------------------------------------------
    // Combinational slot logic: two symbols per lane per cycle
    // ------------------------------------------------------------------
    integer l, k;
    reg [13:0] s;
    reg [8:0]  sym [0:3];
    reg [8:0]  slot_syms [0:1][0:3];
    reg        c_fetch;
    reg        c_underrun;
    reg [1:0]  c_phase;
    reg [$clog2(TU_SIZE)-1:0]   c_tu_pos;
    reg [$clog2(TU_SIZE+1)-1:0] c_tu_valid;
    reg [$clog2(VALID_DEN)-1:0] c_err;
    reg [24*LANE_COUNT-1:0] c_word;
    reg [23:0] lane_pix;
    reg [$clog2(VALID_DEN)+1:0] err_sum;

    always @* begin
        c_fetch      = 1'b0;
        c_underrun   = 1'b0;
        c_phase      = phase;
        c_tu_pos     = tu_pos;
        c_tu_valid   = tu_valid;
        c_err        = bres_err;
        c_word       = cur_word;
        for (k = 0; k < 2; k = k + 1) begin
            slot_syms[k][0] = DUMMY; slot_syms[k][1] = DUMMY;
            slot_syms[k][2] = DUMMY; slot_syms[k][3] = DUMMY;
        end

        if (running) begin
            // Prime the first pixel word of each active line from the FIFO
            // head one cycle before the data region (slot 1 carries BE).
            if (f_prime) begin
                c_word  = fifo_rdata;
                c_fetch = 1'b1;
                if (!fifo_rvalid)
                    c_underrun = 1'b1;
            end

            for (k = 0; k < 2; k = k + 1) begin
                for (l = 0; l < 4; l = l + 1)
                    sym[l] = DUMMY;

                if (f_be[k]) begin
                    for (l = 0; l < LANE_COUNT; l = l + 1)
                        sym[l] = BE;
                end else if (f_tu[k]) begin
                    // ---- transfer-unit region ----
                    if (c_tu_pos == 0) begin
                        // acc += NUM; valid = acc/DEN; acc %= DEN, with
                        // NUM = VQUOT*DEN + VREM and acc < DEN, reduces to:
                        err_sum = c_err + VREM;
                        if (err_sum >= VALID_DEN) begin
                            c_err      = err_sum - VALID_DEN;
                            c_tu_valid = (VQUOT+1 > TU_SIZE) ? TU_SIZE[$clog2(TU_SIZE+1)-1:0]
                                                             : (VQUOT+1);
                        end else begin
                            c_err      = err_sum[$clog2(VALID_DEN)-1:0];
                            c_tu_valid = VQUOT[$clog2(TU_SIZE+1)-1:0];
                        end
                    end
                    if (c_tu_pos < c_tu_valid) begin
                        for (l = 0; l < LANE_COUNT; l = l + 1) begin
                            lane_pix = c_word[24*l +: 24];
                            case (c_phase)
                                2'd0:    sym[l] = {1'b0, lane_pix[23:16]};
                                2'd1:    sym[l] = {1'b0, lane_pix[15:8]};
                                default: sym[l] = {1'b0, lane_pix[7:0]};
                            endcase
                        end
                        if (c_phase == 2'd2) begin
                            c_phase = 2'd0;
                            if (f_nl[k]) begin
                                c_word  = fifo_rdata;
                                c_fetch = 1'b1;
                                if (!fifo_rvalid)
                                    c_underrun = 1'b1;
                            end
                        end else begin
                            c_phase = c_phase + 1'b1;
                        end
                    end else if (c_tu_pos == c_tu_valid && c_tu_valid != TU_SIZE) begin
                        for (l = 0; l < LANE_COUNT; l = l + 1)
                            sym[l] = FS;
                    end else if (c_tu_pos == TU_SIZE-1 && c_tu_valid != TU_SIZE) begin
                        for (l = 0; l < LANE_COUNT; l = l + 1)
                            sym[l] = FE;
                    end
                    // else: dummy fill inside the TU
                    c_tu_pos = (c_tu_pos == TU_SIZE-1) ? {$clog2(TU_SIZE){1'b0}}
                                                       : c_tu_pos + 1'b1;
                end else if (f_bs[k]) begin
                    for (l = 0; l < LANE_COUNT; l = l + 1)
                        sym[l] = BS;
                end else if (f_vb[k] == 2'd1) begin
                    for (l = 0; l < LANE_COUNT; l = l + 1) sym[l] = {1'b0, vbid_byte};
                end else if (f_vb[k] == 2'd2) begin
                    for (l = 0; l < LANE_COUNT; l = l + 1) sym[l] = {1'b0, mvid_byte};
                end else if (f_vb[k] == 2'd3) begin
                    for (l = 0; l < LANE_COUNT; l = l + 1) sym[l] = {1'b0, maud_byte};
                end
                // else: dummy (line-start blank TU, post-VBID tail)

                slot_syms[k][0] = sym[0];
                slot_syms[k][1] = sym[1];
                slot_syms[k][2] = sym[2];
                slot_syms[k][3] = sym[3];
            end
        end
    end

    // Pop combinationally so back-to-back fetches see an advanced head
    assign fifo_rd = !reset && (running ? c_fetch
                     : (fifo_rvalid && !fifo_rsof));  // pre-start: discard non-SOF words

    // ------------------------------------------------------------------
    // Sequential state
    // ------------------------------------------------------------------
    initial begin
        ready       = 1'b0;
        data        = 73'b0;
        capture_arm = 1'b0;
        sdp_gap     = 1'b0;
        underrun    = 1'b0;
        running     = 1'b0;
        line_cycle  = 0;
        line_num    = 0;
        phase       = 2'd0;
        tu_pos      = 0;
        tu_valid    = 0;
        bres_err    = 0;
    end

    always @(posedge clk) begin
        if (reset) begin
            running     <= 1'b0;
            ready       <= 1'b0;
            capture_arm <= 1'b0;
            underrun    <= 1'b0;
            data        <= 73'b0;
            sdp_gap     <= 1'b0;
            frame_pulse <= 1'b0;
            line_cycle  <= 0;
            line_num    <= 0;
            start_ok    <= 1'b0;
            f_be        <= 2'b00;
            f_tu        <= 2'b00;
            f_bs        <= 2'b00;
            f_nl        <= 2'b00;
            f_vb[0]     <= 2'd0;
            f_vb[1]     <= 2'd0;
            f_prime     <= 1'b0;
        end else begin
            capture_arm <= 1'b1;   // pixel domain may start at its next frame
            frame_pulse <= running && (line_cycle == CYCLES_PER_LINE-1) &&
                           (line_num == V_VISIBLE-1);

            // region flags for the NEXT cycle's two symbol slots
            f_be[0] <= nl_active && ((DATA_START-1) % 2 == 0) &&
                       (s0n == DATA_START-1);
            f_be[1] <= nl_active && ((DATA_START-1) % 2 == 1) &&
                       (s1n == DATA_START-1);
            f_tu[0] <= nl_active && (s0n >= DATA_START) && (s0n < BS_POS);
            f_tu[1] <= nl_active && (s1n >= DATA_START) && (s1n < BS_POS);
            f_bs[0] <= (s0n == BS_POS);
            f_bs[1] <= (s1n == BS_POS);
            f_nl[0] <= (s0n != BS_POS-1);
            f_nl[1] <= (s1n != BS_POS-1);
            f_vb[0] <= vsel(s0n);
            f_vb[1] <= vsel(s1n);
            f_prime <= running && nl_active && (nc == (DATA_START/2)-1);

            if (!running) begin
                data    <= {1'b0, {4{DUMMY, DUMMY}}};
                ready   <= 1'b0;
                sdp_gap <= 1'b0;
                // conditions are stable until the head word is popped, so
                // acting one cycle late keeps frame alignment intact
                start_ok <= fifo_rvalid && fifo_rsof && (fifo_rlevel >= PREFILL);
                if (start_ok) begin
                    running    <= 1'b1;
                    line_cycle <= 0;
                    line_num   <= 0;
                    tu_pos     <= 0;
                    tu_valid   <= 0;
                    bres_err   <= 0;
                    phase      <= 2'd0;
                end
            end else begin
                for (l = 0; l < 4; l = l + 1)
                    data[18*l +: 18] <= {slot_syms[1][l], slot_syms[0][l]};

                // switch point: vblank lines, in the dummy region before BS
                data[72] <= (!is_active_line) &&
                            ({line_cycle, 1'b0} > 8) &&
                            ({line_cycle, 1'b0} < BS_POS - 4);

                // SDP-safe gap (Phase 3 hook)
                sdp_gap <= ({line_cycle, 1'b0} >= GAP_START) &&
                           ({line_cycle, 1'b0} <  GAP_END);

                ready <= 1'b1;
                if (c_underrun)
                    underrun <= 1'b1;

                phase      <= c_phase;
                tu_pos     <= c_tu_pos;
                tu_valid   <= c_tu_valid;
                bres_err   <= c_err;
                cur_word   <= c_word;

                if (line_cycle == CYCLES_PER_LINE-1) begin
                    line_cycle <= 0;
                    line_num   <= (line_num == V_TOTAL-1) ? {$clog2(V_TOTAL){1'b0}}
                                                          : line_num + 1'b1;
                    tu_pos     <= 0;
                    tu_valid   <= 0;
                    bres_err   <= 0;
                    phase      <= 2'd0;
                end else begin
                    line_cycle <= line_cycle + 1'b1;
                end
            end
        end
    end

endmodule
