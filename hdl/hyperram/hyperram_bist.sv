//
// HyperRAM built-in self test — bring-up error-rate measurement
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
//
// Description:
//
// Sits between the normal client and hyperram_ctrl's user port (pass-
// through while idle) and, when started from the register interface
// (ESP32 link register file), drives write / masked-overwrite / read-
// compare passes over an address range and counts errors. An eye-scan mode
// forces the controller's input delay across the tap range and records
// the read error count per tap. Testbench: boards/a2p25/tests/
// test_hyperram_bist.sv (`make hyperram_bist`).
//
// Test pass (shadow-free — the expected value of every word is a pure
// function of its address and the pass seed, so nothing is stored):
//   W  write the pattern over [START, START+LENGTH), full byte enables
//   M  (MODE.RAND_MASK) overwrite the range with the INVERTED pattern
//      under a per-word random byte mask m(addr) = hash(addr ^ seed)[3:0]
//   R  read back and compare against pat(addr) ^ bytemask(m(addr))
// Bursts are MODE.BURST words, or random 1..16 (MODE.RAND_LEN), clipped
// to the end of the range. Passes: single / N (PASSES) / continuous until
// STOP. Each pass uses seed + pass index (PRBS data and masks change
// every pass); MODE.INV_ALT also inverts the pattern on odd passes.
//
// Patterns (MODE.PATTERN):
//   0 address-in-address (word address)    4 checkerboard  55AA55AA/AA55AA55
//   1 PRBS32 (xorshift32, seeded)          5 inverse checkerboard
//   2 walking 1 (bit = addr[4:0])          6 all 0
//   3 walking 0                            7 all 1
// PRBS32 is generated sequentially (one step per word, restarted at the
// start of each phase), so every phase walks the range in address order.
//
// Eye scan (MODE.EYE): one W (+M) pass at the calibrated delay, then for
// tap = EYE_CFG.FIRST, +STEP ... <= 255: force the controller's input
// delay to tap, perform EYE_CFG.READS read bursts of MODE.BURST words
// (wrapping over the range) and store {tap, error words} in the eye table
// (index 0, 1, ...; EYE_IDX selects the entry read at EYE_DATA). The force
// is released at the end. Error words include padded words of timed-out
// reads.
//
// ---------------------------------------------------------------------------
// Register map (32-bit registers, word address reg_addr[7:0]).
// reg_rdata is registered: valid the clk after reg_addr is presented.
// ---------------------------------------------------------------------------
//   0x00 ID          RO  0x48524231 ("HRB1")
//   0x01 CTRL        WO  bit0 START  bit1 STOP  bit2 CLEAR (counters, eye
//                        count, error capture)  bit3 RECAL (controller
//                        re-calibration)  bit4 CR0_WRITE (program CR0 reg)
//                        RECAL / CR0_WRITE are accepted only while idle.
//   0x02 STATUS      RO  bit0 busy  bit1 done (sticky until START/CLEAR)
//                        bit2 ctrl init_done  bit3 ctrl cal_ok  bit4 errors
//                        seen  bit5 stopped by STOP  bits[10:8] phase
//                        (0 idle 1 write 2 mask 3 read 4 eye-set 5 eye-read)
//                        bits[31:16] eye entries recorded
//   0x03 MODE        RW  [2:0] PATTERN  [5:4] RUN (0 single, 1 PASSES
//                        passes, 2/3 continuous)  [6] RAND_LEN  [7] RAND_MASK
//                        [8] EYE  [9] READ_ONLY (skip W/M: verify existing
//                        data)  [10] INV_ALT  [19:16] BURST-1
//   0x04 START_ADDR  RW  first word address
//   0x05 LENGTH      RW  words (0 = nothing)
//   0x06 PASSES      RW  pass count for RUN = 1
//   0x07 SEED        RW  PRBS / mask / random-length seed
//   0x08 EYE_CFG     RW  [7:0] STEP (0 = 1)  [15:8] FIRST tap  [31:16] READS
//                        per tap (0 = 1)
//   0x09 CR0         RW  value written by CTRL.CR0_WRITE
//   0x0A DLY_FORCE   RW  [8] force enable  [7:0] code (manual tap override;
//                        the eye scan overrides it while running)
//   0x10 WORDS_WR    RO  words written (W and M phases)
//   0x11 WORDS_RD    RO  words read and compared
//   0x12 ERR_WORDS   RO  words that mismatched
//   0x13 ERR_BITS    RO  total mismatching bits (popcount)
//   0x14 FIRST_ERR_ADDR RO [31] valid, [ADDR_WIDTH-1:0] address
//   0x15 FIRST_ERR_EXP  RO expected data of the first error
//   0x16 FIRST_ERR_ACT  RO actual data of the first error
//   0x17 LAST_ERR_ADDR  RO [31] valid, address of the most recent error
//   0x18 XFERS       RO  transactions issued by the BIST
//   0x19 LAT2X       RO  of which 2x initial latency
//   0x1A RD_TIMEOUTS RO  reads that timed out (rd_err)
//   0x1B PASS_COUNT  RO  completed passes
//   0x1C CUR_ADDR    RO  current address (progress)
//   0x20 CAL_MAP     RO  controller calibration pass map (bin b = tap b*8)
//   0x21 CAL_INFO    RO  [7:0] live delay code  [15:8] calibrated tap
//                        [20:16] read arrival {cyc, half}  [21] cal_ok
//                        [22] init_done
//   0x22 CR0_ACTIVE  RO  [15:0] CR0 the device holds
//   0x23 CTL_XFERS   RO  controller transaction count (all clients)
//   0x24 CTL_LAT2X   RO  controller 2x-latency count
//   0x25 CTL_RD_TMO  RO  controller read timeouts
//   0x26 CTL_LAT_MISMATCH RO reads whose CA latency sample disagreed with
//                        the observed data arrival (should stay 0)
//   0x30 EYE_IDX     RW  [7:0] eye table index
//   0x31 EYE_DATA    RO  [31:24] tap  [23:0] error words (saturating)
//                        (two clks after EYE_IDX is written)
//

module hyperram_bist #(
    parameter ADDR_WIDTH = 21             // bursts are 1..16 words (MODE.BURST)
) (
    input  wire                     clk,
    input  wire                     rst_n,

    // Register interface
    input  wire [7:0]               reg_addr,
    input  wire [31:0]              reg_wdata,
    input  wire                     reg_we,
    output logic [31:0]             reg_rdata,
    output wire                     active,        // BIST owns the controller

    // Client side (same signals as hyperram_ctrl's user port)
    output wire                     u_ready,
    input  wire                     u_req,
    input  wire                     u_we,
    input  wire [ADDR_WIDTH-1:0]    u_addr,
    input  wire [4:0]               u_len,
    input  wire [31:0]              u_wdata,
    input  wire [3:0]               u_wbe,
    input  wire                     u_wbuf_we,
    input  wire [3:0]               u_wbuf_idx,
    input  wire [31:0]              u_wbuf_data,
    input  wire [3:0]               u_wbuf_be,
    output wire                     u_rvalid,
    output wire [31:0]              u_rdata,
    output wire                     u_done,
    output wire                     u_rd_err,

    // Controller side
    input  wire                     m_ready,
    input  wire                     m_init_done,
    output wire                     m_req,
    output wire                     m_we,
    output wire [ADDR_WIDTH-1:0]    m_addr,
    output wire [4:0]               m_len,
    output wire [31:0]              m_wdata,
    output wire [3:0]               m_wbe,
    output wire                     m_wbuf_we,
    output wire [3:0]               m_wbuf_idx,
    output wire [31:0]              m_wbuf_data,
    output wire [3:0]               m_wbuf_be,
    input  wire                     m_rvalid,
    input  wire [31:0]              m_rdata,
    input  wire                     m_done,
    input  wire                     m_rd_err,
    input  wire                     m_xfer_lat2x,
    output logic                    m_recal,
    output wire [15:0]              m_cfg_cr0,
    output logic                    m_cfg_cr0_write,
    output wire                     m_dly_force,
    output wire [7:0]               m_dly_force_code,
    input  wire [15:0]              m_cr0_active,
    input  wire [7:0]               m_dly_code,
    input  wire [7:0]               m_cal_tap,
    input  wire [31:0]              m_cal_map,
    input  wire                     m_cal_ok,
    input  wire [4:0]               m_rd_arrival,
    input  wire [31:0]              m_stat_xfers,
    input  wire [31:0]              m_stat_lat2x,
    input  wire [31:0]              m_stat_rd_timeouts,
    input  wire [31:0]              m_stat_lat_mismatch
);

    // =====================================================================
    // Registers
    // =====================================================================
    logic [31:0]           r_mode;
    logic [ADDR_WIDTH-1:0] r_start;
    logic [31:0]           r_length;
    logic [31:0]           r_passes;
    logic [31:0]           r_seed;
    logic [31:0]           r_eye_cfg;
    logic [15:0]           r_cr0;
    logic [8:0]            r_dly_force;
    logic [7:0]            r_eye_idx;

    wire  [2:0]  mode_pat      = r_mode[2:0];
    wire  [1:0]  mode_run      = r_mode[5:4];
    wire         mode_rand_len = r_mode[6];
    wire         mode_rand_msk = r_mode[7];
    wire         mode_eye      = r_mode[8];
    wire         mode_ro       = r_mode[9];
    wire         mode_inv_alt  = r_mode[10];
    wire  [4:0]  mode_burst    = {1'b0, r_mode[19:16]} + 5'd1;
    wire  [7:0]  eye_step      = (r_eye_cfg[7:0] == 8'd0) ? 8'd1 : r_eye_cfg[7:0];
    wire  [7:0]  eye_first     = r_eye_cfg[15:8];
    wire  [15:0] eye_reads     = (r_eye_cfg[31:16] == 16'd0) ? 16'd1 : r_eye_cfg[31:16];

    // Counters / capture
    logic [31:0] c_words_wr, c_words_rd, c_err_words, c_err_bits;
    logic [31:0] c_xfers, c_lat2x, c_rd_tmo, c_passes;
    logic        e_first_v, e_last_v;
    logic [ADDR_WIDTH-1:0] e_first_addr, e_last_addr;
    logic [31:0] e_first_exp, e_first_act;

    // Status
    logic        busy, done_sticky, stopped, stop_req;
    logic [2:0]  phase;
    logic        pend_recal, pend_cr0;

    // Eye table
    logic [31:0] eye_ram [0:255];
    logic [31:0] eye_q;
    logic [8:0]  eye_n;            // entries recorded (0..256)

    // =====================================================================
    // Pattern generation
    // =====================================================================
    function automatic [31:0] xs32(input [31:0] x);
        logic [31:0] y;
        begin
            y = x ^ (x << 13);
            y = y ^ (y >> 17);
            xs32 = y ^ (y << 5);
        end
    endfunction

    function automatic [31:0] popcount32(input [31:0] v);
        integer k;
        begin
            popcount32 = 32'd0;
            for (k = 0; k < 32; k = k + 1)
                popcount32 = popcount32 + {31'd0, v[k]};
        end
    endfunction

    logic [ADDR_WIDTH-1:0] pg_addr;     // address of the word being generated
    logic [31:0]           pg_lfsr;     // PRBS state for pg_addr
    logic [31:0]           pass_seed;
    logic                  pass_inv;

    wire  [31:0] pg_a32  = {{(32-ADDR_WIDTH){1'b0}}, pg_addr};
    wire  [31:0] pg_walk = 32'd1 << pg_addr[4:0];
    wire         pg_odd  = pg_addr[0];
    logic [31:0] pg_pat;
    always_comb begin
        case (mode_pat)
            3'd0: pg_pat = pg_a32;
            3'd1: pg_pat = pg_lfsr;
            3'd2: pg_pat = pg_walk;
            3'd3: pg_pat = ~pg_walk;
            3'd4: pg_pat = pg_odd ? 32'hAA55AA55 : 32'h55AA55AA;
            3'd5: pg_pat = pg_odd ? 32'h55AA55AA : 32'hAA55AA55;
            3'd6: pg_pat = 32'h00000000;
            default: pg_pat = 32'hFFFFFFFF;
        endcase
        if (pass_inv) pg_pat = ~pg_pat;
    end
    wire  [31:0] pg_hash  = xs32(pg_a32 ^ pass_seed ^ 32'h9E3779B9);
    wire  [3:0]  pg_mask  = mode_rand_msk ? pg_hash[3:0] : 4'h0;
    wire  [31:0] pg_bmask = {{8{pg_mask[3]}}, {8{pg_mask[2]}}, {8{pg_mask[1]}}, {8{pg_mask[0]}}};
    wire  [31:0] pg_exp   = pg_pat ^ pg_bmask;          // expected after W (+M)
    wire  [31:0] seed_nz  = (pass_seed == 32'd0) ? 32'h1 : pass_seed;

    // =====================================================================
    // Engine
    // =====================================================================
    localparam [3:0] E_IDLE   = 4'd0;
    localparam [3:0] E_WAITRDY= 4'd1;
    localparam [3:0] E_PASS   = 4'd2;
    localparam [3:0] E_PHASE  = 4'd3;
    localparam [3:0] E_BPREP  = 4'd4;
    localparam [3:0] E_LOAD   = 4'd5;
    localparam [3:0] E_REQ    = 4'd6;
    localparam [3:0] E_WAIT   = 4'd7;
    localparam [3:0] E_PEND   = 4'd8;
    localparam [3:0] E_EYESET = 4'd9;
    localparam [3:0] E_EYEDLY = 4'd10;
    localparam [3:0] E_EYEREC = 4'd11;
    localparam [3:0] E_FINISH = 4'd12;

    localparam [2:0] P_IDLE = 3'd0, P_W = 3'd1, P_M = 3'd2, P_R = 3'd3, P_ES = 3'd4, P_ER = 3'd5;

    logic [3:0]            est;
    logic [ADDR_WIDTH-1:0] cur;          // first address of the next burst
    logic [31:0]           remain;       // words left in this phase
    logic [4:0]            blen;         // current burst length
    logic [4:0]            k;            // word index while loading
    logic [31:0]           ctl_lfsr;     // random lengths
    logic                  e_req, e_we, e_wbuf_we;
    logic [ADDR_WIDTH-1:0] e_addr;
    logic [4:0]            e_len;
    logic [31:0]           e_wdata, e_wbuf_data;
    logic [3:0]            e_wbe, e_wbuf_be, e_wbuf_idx;
    logic                  own;          // in-flight transaction is the BIST's
    logic [8:0]            eye_tap;      // 9 bits: loop end detection
    logic [15:0]           eye_cnt_rd;
    logic [23:0]           eye_err;
    logic [3:0]            settle;
    logic                  eye_force;

    wire  [4:0]  rand_len = {1'b0, ctl_lfsr[3:0]} + 5'd1;
    wire  [4:0]  want_len = mode_rand_len ? rand_len : mode_burst;
    wire  [4:0]  clip_len = (remain < {27'd0, want_len}) ? remain[4:0] : want_len;
    wire         phase_wr = (phase == P_W) || (phase == P_M);

    // Write data for the word being generated
    wire  [31:0] gen_wdata = (phase == P_M) ? ~pg_pat : pg_pat;
    wire  [3:0]  gen_wbe   = (phase == P_M) ? pg_mask : 4'hF;

    // Read compare
    wire         bist_rvalid = m_rvalid && own;
    wire  [31:0] diff        = m_rdata ^ pg_exp;
    logic        mis;
    logic [31:0] mis_bits;
    always_comb begin
        mis      = (diff != 32'd0);
        mis_bits = popcount32(diff);
        // synthesis translate_off
        if (^diff === 1'bx) begin            // X/Z read: every bit is an error
            mis      = 1'b1;
            mis_bits = 32'd32;
        end
        // synthesis translate_on
    end

    // Restart the pattern generator at the start of a phase / range
    task automatic pg_restart();
        begin
            pg_addr <= r_start;
            pg_lfsr <= seed_nz;
        end
    endtask

    task automatic pg_advance();
        begin
            pg_addr <= pg_addr + 1'b1;
            pg_lfsr <= xs32(pg_lfsr);
        end
    endtask

    assign active = busy;

    // ---- Pass-through mux ----
    assign u_ready     = m_ready && !busy && !pend_recal && !pend_cr0;
    assign m_req       = busy ? e_req       : (u_req && u_ready);
    assign m_we        = busy ? e_we        : u_we;
    assign m_addr      = busy ? e_addr      : u_addr;
    assign m_len       = busy ? e_len       : u_len;
    assign m_wdata     = busy ? e_wdata     : u_wdata;
    assign m_wbe       = busy ? e_wbe       : u_wbe;
    assign m_wbuf_we   = busy ? e_wbuf_we   : u_wbuf_we;
    assign m_wbuf_idx  = busy ? e_wbuf_idx  : u_wbuf_idx;
    assign m_wbuf_data = busy ? e_wbuf_data : u_wbuf_data;
    assign m_wbuf_be   = busy ? e_wbuf_be   : u_wbuf_be;
    assign u_rvalid    = m_rvalid && !own;
    assign u_rdata     = m_rdata;
    assign u_done      = m_done && !own;
    assign u_rd_err    = m_rd_err;
    assign m_cfg_cr0   = r_cr0;
    assign m_dly_force      = eye_force || r_dly_force[8];
    assign m_dly_force_code = eye_force ? eye_tap[7:0] : r_dly_force[7:0];

    wire ctrl_start = reg_we && reg_addr == 8'h01 && reg_wdata[0];
    wire ctrl_stop  = reg_we && reg_addr == 8'h01 && reg_wdata[1];
    wire ctrl_clear = reg_we && reg_addr == 8'h01 && reg_wdata[2];

    always_ff @(posedge clk) begin
        e_req           <= 1'b0;
        e_wbuf_we       <= 1'b0;
        m_recal         <= 1'b0;
        m_cfg_cr0_write <= 1'b0;

        // ---- register writes ----
        if (reg_we) begin
            case (reg_addr)
                8'h03: r_mode      <= reg_wdata;
                8'h04: r_start     <= reg_wdata[ADDR_WIDTH-1:0];
                8'h05: r_length    <= reg_wdata;
                8'h06: r_passes    <= reg_wdata;
                8'h07: r_seed      <= reg_wdata;
                8'h08: r_eye_cfg   <= reg_wdata;
                8'h09: r_cr0       <= reg_wdata[15:0];
                8'h0A: r_dly_force <= reg_wdata[8:0];
                8'h30: r_eye_idx   <= reg_wdata[7:0];
                8'h01: begin
                    if (reg_wdata[3] && !busy) pend_recal <= 1'b1;
                    if (reg_wdata[4] && !busy) pend_cr0   <= 1'b1;
                end
                default: ;
            endcase
        end
        if (ctrl_stop && busy) stop_req <= 1'b1;

        // ---- controller configuration commands (idle only) ----
        if (!busy && m_ready && !u_req) begin
            if (pend_recal) begin
                m_recal    <= 1'b1;
                pend_recal <= 1'b0;
            end else if (pend_cr0) begin
                m_cfg_cr0_write <= 1'b1;
                pend_cr0        <= 1'b0;
            end
        end

        // ---- transaction completion bookkeeping ----
        if (m_req && busy) own <= 1'b1;
        if (m_done && own) begin
            own     <= 1'b0;
            c_xfers <= c_xfers + 32'd1;
            if (m_xfer_lat2x) c_lat2x  <= c_lat2x + 32'd1;
            if (m_rd_err)     c_rd_tmo <= c_rd_tmo + 32'd1;
        end

        // ---- read compare ----
        if (bist_rvalid) begin
            c_words_rd <= c_words_rd + 32'd1;
            if (mis) begin
                c_err_words <= c_err_words + 32'd1;
                c_err_bits  <= c_err_bits + mis_bits;
                if (eye_err != 24'hFFFFFF) eye_err <= eye_err + 24'd1;
                if (!e_first_v) begin
                    e_first_v    <= 1'b1;
                    e_first_addr <= pg_addr;
                    e_first_exp  <= pg_exp;
                    e_first_act  <= m_rdata;
                end
                e_last_v    <= 1'b1;
                e_last_addr <= pg_addr;
            end
            pg_advance();
        end

        case (est)
        E_IDLE: begin
            phase <= P_IDLE;
            if (ctrl_start) begin
                busy        <= 1'b1;
                done_sticky <= 1'b0;
                stopped     <= 1'b0;
                stop_req    <= 1'b0;
                c_passes    <= 32'd0;
                eye_n       <= 9'd0;
                est         <= E_WAITRDY;
            end
        end

        E_WAITRDY:
            // let a client transaction in flight finish first
            if (m_ready && m_init_done && !own) begin
                pass_seed <= r_seed;
                pass_inv  <= 1'b0;
                ctl_lfsr  <= (r_seed == 32'd0) ? 32'hACE1ACE1 : r_seed ^ 32'h5A5A5A5A;
                est       <= (r_length == 32'd0) ? E_FINISH : E_PASS;
            end

        E_PASS: begin
            // start of a pass: first phase
            phase <= (mode_ro && !mode_eye) ? P_R :
                     (mode_ro && mode_eye)  ? P_ES : P_W;
            est   <= E_PHASE;
        end

        E_PHASE: begin
            // start of a phase
            cur    <= r_start;
            remain <= r_length;
            pg_restart();
            est    <= (phase == P_ES) ? E_EYESET : E_BPREP;
            if (phase == P_ES) begin
                eye_tap <= {1'b0, eye_first};
                eye_n   <= 9'd0;
            end
        end

        E_BPREP: begin
            if (stop_req) begin
                stopped <= 1'b1;
                est     <= E_FINISH;
            end else if (m_ready) begin
                if (phase == P_ER && remain == 32'd0) begin
                    // eye read: wrap over the range
                    cur    <= r_start;
                    remain <= r_length;
                    pg_restart();
                end else begin
                    blen     <= clip_len;
                    ctl_lfsr <= xs32(ctl_lfsr);
                    k        <= 5'd0;
                    est      <= phase_wr ? E_LOAD : E_REQ;
                end
            end
        end

        E_LOAD: begin
            // word k of the burst: word 0 rides with req, 1..n-1 via wbuf
            if (k == 5'd0) begin
                e_wdata <= gen_wdata;
                e_wbe   <= gen_wbe;
            end else begin
                e_wbuf_we   <= 1'b1;
                e_wbuf_idx  <= k[3:0];
                e_wbuf_data <= gen_wdata;
                e_wbuf_be   <= gen_wbe;
            end
            pg_advance();
            k <= k + 5'd1;
            if (k + 5'd1 == blen) est <= E_REQ;
        end

        E_REQ: begin
            if (m_ready && !e_wbuf_we) begin
                e_req  <= 1'b1;
                e_we   <= phase_wr;
                e_addr <= cur;
                e_len  <= blen;
                est    <= E_WAIT;
            end
        end

        E_WAIT: begin
            if (m_done && own) begin
                if (phase_wr) c_words_wr <= c_words_wr + {27'd0, blen};
                cur    <= cur + {{(ADDR_WIDTH-5){1'b0}}, blen};
                remain <= remain - {27'd0, blen};
                if (phase == P_ER) begin
                    eye_cnt_rd <= eye_cnt_rd + 16'd1;
                    est        <= (eye_cnt_rd + 16'd1 == eye_reads) ? E_EYEREC : E_BPREP;
                end else if (remain == {27'd0, blen}) begin
                    // end of phase
                    if (phase == P_W && mode_rand_msk) begin
                        phase <= P_M;
                        est   <= E_PHASE;
                    end else if (phase_wr) begin
                        phase <= mode_eye ? P_ES : P_R;
                        est   <= E_PHASE;
                    end else begin
                        est   <= E_PEND;
                    end
                end else begin
                    est <= E_BPREP;
                end
            end
        end

        E_PEND: begin
            c_passes <= c_passes + 32'd1;
            if (stop_req) begin
                stopped <= 1'b1;
                est     <= E_FINISH;
            end else if (mode_run[1] || (mode_run == 2'd1 && c_passes + 32'd1 < r_passes)) begin
                pass_seed <= pass_seed + 32'd1;
                pass_inv  <= mode_inv_alt && !pass_inv;
                est       <= E_PASS;
            end else begin
                est <= E_FINISH;
            end
        end

        // ---- eye scan ----
        E_EYESET: begin
            if (stop_req || eye_tap[8]) begin
                stopped <= stop_req;
                est     <= E_FINISH;
            end else begin
                eye_force <= 1'b1;
                settle    <= 4'd8;
                est       <= E_EYEDLY;
            end
        end

        E_EYEDLY:
            // wait until the controller has loaded the forced code
            if (m_dly_code == eye_tap[7:0] && m_ready) begin
                if (settle == 4'd0) begin
                    phase      <= P_ER;
                    eye_cnt_rd <= 16'd0;
                    eye_err    <= 24'd0;
                    cur        <= r_start;
                    remain     <= r_length;
                    pg_restart();
                    est        <= E_BPREP;
                end else begin
                    settle <= settle - 4'd1;
                end
            end

        E_EYEREC: begin
            eye_ram[eye_n[7:0]] <= {eye_tap[7:0], eye_err};
            eye_n   <= eye_n + 9'd1;
            eye_tap <= eye_tap + {1'b0, eye_step};
            phase   <= P_ES;
            est     <= E_EYESET;
        end

        E_FINISH: begin
            eye_force   <= 1'b0;
            if (!own) begin
                busy        <= 1'b0;
                done_sticky <= 1'b1;
                phase       <= P_IDLE;
                est         <= E_IDLE;
            end
        end

        default: est <= E_IDLE;
        endcase

        if (ctrl_clear) begin
            c_words_wr  <= 32'd0;
            c_words_rd  <= 32'd0;
            c_err_words <= 32'd0;
            c_err_bits  <= 32'd0;
            c_xfers     <= 32'd0;
            c_lat2x     <= 32'd0;
            c_rd_tmo    <= 32'd0;
            c_passes    <= 32'd0;
            e_first_v   <= 1'b0;
            e_last_v    <= 1'b0;
            done_sticky <= 1'b0;
            if (!busy) eye_n <= 9'd0;
        end

        if (!rst_n) begin
            est         <= E_IDLE;
            busy        <= 1'b0;
            done_sticky <= 1'b0;
            stopped     <= 1'b0;
            stop_req    <= 1'b0;
            own         <= 1'b0;
            eye_force   <= 1'b0;
            eye_n       <= 9'd0;
            pend_recal  <= 1'b0;
            pend_cr0    <= 1'b0;
            phase       <= P_IDLE;
            r_mode      <= 32'd0;
            r_start     <= '0;
            r_length    <= 32'd0;
            r_passes    <= 32'd1;
            r_seed      <= 32'd1;
            r_eye_cfg   <= 32'h0004_0008;
            r_cr0       <= 16'h8F1F;
            r_dly_force <= 9'd0;
            r_eye_idx   <= 8'd0;
            c_words_wr  <= 32'd0;
            c_words_rd  <= 32'd0;
            c_err_words <= 32'd0;
            c_err_bits  <= 32'd0;
            c_xfers     <= 32'd0;
            c_lat2x     <= 32'd0;
            c_rd_tmo    <= 32'd0;
            c_passes    <= 32'd0;
            e_first_v   <= 1'b0;
            e_last_v    <= 1'b0;
            e_first_addr <= '0;
            e_last_addr  <= '0;
            e_first_exp  <= 32'd0;
            e_first_act  <= 32'd0;
        end
    end

    // Eye table read port (registered -> BSRAM)
    always_ff @(posedge clk)
        eye_q <= eye_ram[r_eye_idx];

    // =====================================================================
    // Register read (registered)
    // =====================================================================
    wire [31:0] a32_first = {{(32-ADDR_WIDTH){1'b0}}, e_first_addr};
    wire [31:0] a32_last  = {{(32-ADDR_WIDTH){1'b0}}, e_last_addr};
    wire [31:0] a32_cur   = {{(32-ADDR_WIDTH){1'b0}}, cur};
    wire [31:0] a32_start = {{(32-ADDR_WIDTH){1'b0}}, r_start};

    always_ff @(posedge clk) begin
        case (reg_addr)
            8'h00: reg_rdata <= 32'h48524231;
            8'h02: reg_rdata <= {7'd0, eye_n, 5'd0, phase, 2'd0, stopped,
                                 (c_err_words != 32'd0), m_cal_ok, m_init_done,
                                 done_sticky, busy};
            8'h03: reg_rdata <= r_mode;
            8'h04: reg_rdata <= a32_start;
            8'h05: reg_rdata <= r_length;
            8'h06: reg_rdata <= r_passes;
            8'h07: reg_rdata <= r_seed;
            8'h08: reg_rdata <= r_eye_cfg;
            8'h09: reg_rdata <= {16'd0, r_cr0};
            8'h0A: reg_rdata <= {23'd0, r_dly_force};
            8'h10: reg_rdata <= c_words_wr;
            8'h11: reg_rdata <= c_words_rd;
            8'h12: reg_rdata <= c_err_words;
            8'h13: reg_rdata <= c_err_bits;
            8'h14: reg_rdata <= {e_first_v, a32_first[30:0]};
            8'h15: reg_rdata <= e_first_exp;
            8'h16: reg_rdata <= e_first_act;
            8'h17: reg_rdata <= {e_last_v, a32_last[30:0]};
            8'h18: reg_rdata <= c_xfers;
            8'h19: reg_rdata <= c_lat2x;
            8'h1A: reg_rdata <= c_rd_tmo;
            8'h1B: reg_rdata <= c_passes;
            8'h1C: reg_rdata <= a32_cur;
            8'h20: reg_rdata <= m_cal_map;
            8'h21: reg_rdata <= {9'd0, m_init_done, m_cal_ok, m_rd_arrival, m_cal_tap, m_dly_code};
            8'h22: reg_rdata <= {16'd0, m_cr0_active};
            8'h23: reg_rdata <= m_stat_xfers;
            8'h24: reg_rdata <= m_stat_lat2x;
            8'h25: reg_rdata <= m_stat_rd_timeouts;
            8'h26: reg_rdata <= m_stat_lat_mismatch;
            8'h30: reg_rdata <= {24'd0, r_eye_idx};
            8'h31: reg_rdata <= eye_q;
            default: reg_rdata <= 32'd0;
        endcase
    end

endmodule
