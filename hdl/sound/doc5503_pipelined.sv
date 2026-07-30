// Ensoniq DOC5503 Sound Engine — pipelined-fetch variant for high-latency
// wavetable memory (DDR3)
//
// (c) 2023,2024,2026 Ed Anuff <ed@a2fpga.com>
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
// =============================================================================
// WHY THIS VARIANT EXISTS
// =============================================================================
//
// The baseline doc5503.sv fetches the waveform byte and consumes it within
// the SAME oscillator service slot: the memory round trip must complete
// before clk_count_r == 7, i.e. within ~7/8 of an oscillator cycle (~978 ns
// at 7.159 MHz). That budget is comfortable for BSRAM (1-2 clk) but is NOT
// safe for DDR3 through the a2mega arbiter/CDC stack, whose worst-case
// read tail (in-flight burst grant drain + tRFC refresh collision + CDC
// round trip) is ~1.0-1.2 us. A missed deadline in the baseline produces a
// 0x80 (silence) substitution — under DDR3 contention that would happen
// routinely, not exceptionally.
//
// This variant decouples fetch from consume:
//
//   * During oscillator k's service slot the FSM CONSUMES the byte that was
//     fetched for k during its PREVIOUS service slot (held in a 32-entry
//     result store), runs the full baseline semantics on it (zero-byte
//     halt, output, mix, ACC += FC, overflow/one-shot/swap handling), and
//     then ISSUES the next fetch fire-and-forget from the just-updated
//     accumulator.
//   * The fetch therefore has until k's NEXT service slot to land:
//     (N_enabled + 2) x 1.117 us  >=  3.35 us  (vs 1.2 us worst-case DDR3).
//   * A fetch that outlives a full service period degrades to repeating the
//     previous sample for one slot (counted in dbg_stale_fetch_o) and
//     self-heals when the data arrives.
//
// PREFETCH-WHILE-HALTED (the trick that removes the output delay):
//
//   Halted oscillators still occupy scan slots. During a halted service
//   slot this variant issues a low-rate prefetch of mem[wtp | 0] — the
//   byte the oscillator will need first whenever it is (re)started, since
//   every un-halt path in the DOC (host control write with halt=0,
//   swap-mode partner start) zeroes the accumulator. The prefetch is
//   re-issued only while the stored tag does not match, so a halted
//   oscillator converges to ONE extra memory read and then goes quiet.
//
//   With the result store primed before an oscillator starts, the first
//   consumed sample is available in the oscillator's FIRST service slot —
//   so the pipelined variant produces its samples in the SAME service slots
//   as the baseline, not one slot later. The one-service-period pipeline
//   manifests only as *data staleness*: a fetched byte reflects wavetable
//   memory contents (and the WTP/RTS address-mapping registers) as of one
//   service period before it is heard. See the design doc:
//   boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md
//
// RESULT-STORE TAGGING RULES (correctness-critical):
//
//   Each of the 32 result-store entries holds {data[7:0], tag[15:0] = wave
//   address fetched, valid, src_run}. src_run=1 marks data fetched by a
//   RUNNING oscillator's own pipeline; src_run=0 marks halted-prefetch data.
//
//   * Running oscillator, src_run entry: consume UNCONDITIONALLY. The entry
//     is the byte this oscillator's own previous slot issued; even if the
//     accumulator was rewritten in between (SYNC-mode partner restart, host
//     control write), that byte is the correct delayed-stream sample. A tag
//     mismatch here means either a late/dropped fetch (one-sample repeat,
//     counted) or an external ACC/WTP/RTS rewrite (benign, counted).
//   * Running oscillator, prefetch entry: consume ONLY on tag match with
//     the address the baseline would fetch this slot. This is what makes
//     note-on/swap starts sample-exact.
//   * No valid entry / tag mismatch on prefetch: output 0x80 (centerline
//     silence) for one sample and DO NOT evaluate halt-on-zero. (The real
//     5503 emits exactly this centerline sample at a swap-mode switch —
//     see R. Belmont's notes in MAME es5503.cpp — so this degraded case
//     matches real-silicon behavior.)
//   * Every halted service slot clears the entry's src_run bit. This
//     prevents a spuriously-consumed stale byte (worst case a stale 0x00 →
//     instant re-halt at note-on) when an oscillator is halted and later
//     restarted: after halting, only tag-matched data may be consumed.
//   * A host control write that clears the halt bit also clears src_run
//     (same reason; covers halt-set/halt-clear pairs inside one service
//     period where no halted slot intervenes).
//
// Requires osc_reg_ram from doc5503.sv (compile both files together).
//
// =============================================================================

module doc5503_pipelined #(
    parameter int CLOCK_SPEED_HZ = 54_000_000,
    parameter int DOC_CLOCK_SPEED_HZ = 7_159_090 // 7.15909 MHz
) (
    input clk_i,
    input reset_n_i,
    input clk_en_i,

    input cs_n_i,
    input we_n_i,

    input [7:0] addr_i,
    input [7:0] data_i,
    output reg [7:0] data_o,

    output reg [15:0] wave_address_o,
    output reg wave_rd_o,
    input wave_available_i,        // Memory port can accept a request (tie 1 for BSRAM)
    input wave_data_ready_i,
    input [7:0] wave_data_i,

    output signed [15:0] mono_mix_o,
    output signed [15:0] left_mix_o,
    output signed [15:0] right_mix_o,

    output signed [15:0] channel_o[16],

    output logic ready_o,          // Indicate when out of reset and ready to process

    output [7:0] debug_osc_en_o,   // Debug output for oscillator enable register
    output [1:0] debug_osc_mode_o[8], // Debug output for oscillator mode register;
    output [7:0] debug_osc_halt_o, // Debug output for oscillator halt register

    // Pipelined-fetch diagnostics (wrap-around counters)
    output [7:0] dbg_prime_miss_o,  // Consumed 0x80 because no primed data was available
    output [7:0] dbg_stale_fetch_o, // Consumed a src_run byte whose tag mismatched (late fetch or ACC rewrite)
    output [7:0] dbg_fetch_drop_o   // Fetch request dropped: internal FIFO full (should stay 0)
);

    reg [7:0] host_addr_r;         // Address register for host access
    reg [7:0] host_data_r;        // Data register for host access
    reg host_we_n_r;              // Write enable register for host access
    reg prev_cs_n_r;          // Previous chip select state
    reg prev_we_n_r;          // Previous write enable state
    reg host_access_r;         // Host access register

    always_ff @(posedge clk_i) begin
        if (!reset_n_i) begin
            host_addr_r <= '0;
            host_data_r <= '0;
            host_we_n_r <= 1'b1;
        end else begin
            prev_cs_n_r <= cs_n_i;
            prev_we_n_r <= we_n_i;
            host_access_r <= 1'b0;
            if (!cs_n_i) begin
                host_addr_r <= addr_i;
                host_data_r <= data_i;
                host_we_n_r <= we_n_i;

                // Pulse host_access_r on negative edge of cs_n_i
                // if cs_n_i is low and negative edge of we_n_i
                if (prev_cs_n_r || (!we_n_i && prev_we_n_r)) begin
                    host_access_r <= 1'b1;
                end
            end
        end
    end

    // Registers

    reg [7:0] osc_int_r;            // $E0    : Oscillator Interrupt Register
    reg [7:0] osc_en_r;             // $E1    : Oscillator Enable Register
    assign debug_osc_en_o = osc_en_r;

    reg [1:0] debug_osc_mode_r[8];
    assign debug_osc_mode_o = debug_osc_mode_r;
    reg [7:0] debug_osc_halt_r;
    assign debug_osc_halt_o = debug_osc_halt_r;

    // Oscillator registers as RAM (see doc5503.sv for discussion)

    reg [4:0] ram_fl_osc_r;         // RAM oscillator for frequency low operation
    reg [4:0] ram_fh_osc_r;         // RAM oscillator for frequency high operation
    reg [4:0] ram_vol_osc_r;        // RAM oscillator for volume operation
    reg [4:0] ram_wds_osc_r;        // RAM oscillator for waveform data sample operation
    reg [4:0] ram_wtp_osc_r;        // RAM oscillator for waveform table pointer operation
    reg [4:0] ram_control_osc_r;    // RAM oscillator for control operation
    reg [4:0] ram_rts_osc_r;        // RAM oscillator for resolution table size operation
    reg [4:0] ram_acc_osc_r;        // RAM oscillator for accumulator operation

    reg ram_fl_we_r;
    reg ram_fh_we_r;
    reg ram_vol_we_r;
    reg ram_wds_we_r;
    reg ram_wtp_we_r;
    reg ram_control_we_r;
    reg ram_rts_we_r;
    reg ram_acc_we_r;

    reg [7:0] ram_fl_din_r;
    reg [7:0] ram_fh_din_r;
    reg [7:0] ram_vol_din_r;
    reg [7:0] ram_wds_din_r;
    reg [7:0] ram_wtp_din_r;
    reg [7:0] ram_control_din_r;
    reg [7:0] ram_rts_din_r;
    reg [23:0] ram_acc_din_r;

    wire [7:0] ram_fl_dout_w;
    wire [7:0] ram_fh_dout_w;
    wire [7:0] ram_vol_dout_w;
    wire [7:0] ram_wds_dout_w;
    wire [7:0] ram_wtp_dout_w;
    wire [7:0] ram_control_dout_w;
    wire [7:0] ram_rts_dout_w;
    wire [23:0] ram_acc_dout_w;

    // Instantiate the RAMs for each oscillator register (shared with doc5503.sv)

    // $00-1F : Frequency Low Register
    osc_reg_ram fl_ram (
        .clk_i(clk_i),
        .osc_i(ram_fl_osc_r),
        .we_i(ram_fl_we_r),
        .data_i(ram_fl_din_r),
        .data_o(ram_fl_dout_w)
    );

    // $20-3F : Frequency High Register
    osc_reg_ram fh_ram (
        .clk_i(clk_i),
        .osc_i(ram_fh_osc_r),
        .we_i(ram_fh_we_r),
        .data_i(ram_fh_din_r),
        .data_o(ram_fh_dout_w)
    );

    // $40-5F : Volume Register
    osc_reg_ram vol_ram (
        .clk_i(clk_i),
        .osc_i(ram_vol_osc_r),
        .we_i(ram_vol_we_r),
        .data_i(ram_vol_din_r),
        .data_o(ram_vol_dout_w)
    );

    // $60-7F : Waveform Data Sample Register
    osc_reg_ram wds_ram (
        .clk_i(clk_i),
        .osc_i(ram_wds_osc_r),
        .we_i(ram_wds_we_r),
        .data_i(ram_wds_din_r),
        .data_o(ram_wds_dout_w)
    );

    // $80-9F : Waveform Table Pointer Register
    osc_reg_ram wtp_ram (
        .clk_i(clk_i),
        .osc_i(ram_wtp_osc_r),
        .we_i(ram_wtp_we_r),
        .data_i(ram_wtp_din_r),
        .data_o(ram_wtp_dout_w)
    );

    // $A0-BF : Control Register
    osc_reg_ram control_ram (
        .clk_i(clk_i),
        .osc_i(ram_control_osc_r),
        .we_i(ram_control_we_r),
        .data_i(ram_control_din_r),
        .data_o(ram_control_dout_w)
    );

    // $C0-DF : Resolution Table Size Register
    osc_reg_ram rts_ram (
        .clk_i(clk_i),
        .osc_i(ram_rts_osc_r),
        .we_i(ram_rts_we_r),
        .data_i(ram_rts_din_r),
        .data_o(ram_rts_dout_w)
    );

    // $E0-FF : Oscillator Accumulator Register
    osc_reg_ram #(.DATA_WIDTH(24)) acc_ram (
        .clk_i(clk_i),
        .osc_i(ram_acc_osc_r),
        .we_i(ram_acc_we_r),
        .data_i(ram_acc_din_r),
        .data_o(ram_acc_dout_w)
    );

    // Current oscillator state, copied from the register file at the start of each cycle
    reg [7:0] curr_fl_r;
    reg [7:0] curr_fh_r;
    reg [7:0] curr_vol_r;
    reg [7:0] curr_wds_r;
    reg [7:0] curr_wtp_r;
    reg [7:0] curr_control_r;
    reg [7:0] curr_rts_r;
    reg [7:0] partner_control_r;
    reg [7:0] next_control_r;
    reg [7:0] prev_control_r;

    reg [4:0] curr_osc_r;
    wire curr_osc_odd_w = curr_osc_r[0];
    wire curr_osc_even_w = ~curr_osc_odd_w;
    wire [4:0] partner_osc_w = curr_osc_r ^ 5'b1;

    wire [2:0] curr_wts_w = curr_rts_r[5:3];
    wire [2:0] curr_res_w = curr_rts_r[2:0];
    wire [4:0] curr_shift_w = 5'd9 + curr_res_w - curr_wts_w;
    wire [23:0] curr_acc_mask_w = {8'((1 << (1'd1 + curr_res_w)) - 1), 16'hFFFF};

    wire halt_w = curr_control_r[0];
    wire [1:0] curr_mode_w = curr_control_r[2:1];
    wire [3:0] curr_ca_w = curr_control_r[7:4];

    wire partner_halt_w = partner_control_r[0];
    wire [1:0] partner_mode_w = partner_control_r[2:1];

    wire next_halt_w = next_control_r[0];
    wire [1:0] next_mode_w = next_control_r[2:1];

    wire prev_halt_w = prev_control_r[0];
    wire [1:0] prev_mode_w = prev_control_r[2:1];

    reg [23:0] curr_acc_r;
    reg signed [17:0] curr_output_r;

    // =========================================================================
    // Per-oscillator fetch result store
    // =========================================================================
    // 32 x {valid, src_run, tag[15:0], data[7:0]} — infers as FFs/LUTRAM.

    reg [31:0]  fetch_valid_r;
    reg [31:0]  fetch_src_run_r;
    reg [15:0]  fetch_tag_r  [32];
    reg [7:0]   fetch_data_r [32];

    // =========================================================================
    // Fetch request FIFO (issue side of the fire-and-forget pipeline)
    // =========================================================================
    // Depth 8, three pointers: wr (enqueue by FSM), issue (sent to memory
    // port), ret (retired by wave_data_ready_i, in order). The memory port
    // (ddr3_port_cdc) queues up to 2 requests and returns responses in
    // order, so ret lags issue by at most 2 in practice.

    localparam int FQ_DEPTH = 8;
    reg [4:0]  fq_osc_r  [FQ_DEPTH];
    reg [15:0] fq_addr_r [FQ_DEPTH];
    reg        fq_src_r  [FQ_DEPTH];
    reg [2:0]  fq_wr_ptr_r;
    reg [2:0]  fq_issue_ptr_r;
    reg [2:0]  fq_ret_ptr_r;

    wire fq_full_w = (3'(fq_wr_ptr_r + 3'd1) == fq_ret_ptr_r);

    reg [7:0] dbg_prime_miss_r;
    reg [7:0] dbg_stale_fetch_r;
    reg [7:0] dbg_fetch_drop_r;
    assign dbg_prime_miss_o  = dbg_prime_miss_r;
    assign dbg_stale_fetch_o = dbg_stale_fetch_r;
    assign dbg_fetch_drop_o  = dbg_fetch_drop_r;

    // Address the baseline implementation would fetch for the current
    // oscillator this service slot: (ACC >> shift) OR'd with the masked
    // wavetable pointer. Also used (with the post-update ACC) to compute
    // the next issue address.
    function automatic logic [15:0] wave_addr_f(input logic [23:0] acc);
        logic [7:0] ptr_hi_mask;
        logic [15:0] ptr;
        ptr_hi_mask = 8'(8'hFF << curr_wts_w);
        ptr = {ptr_hi_mask & curr_wtp_r, 8'b0};
        return 16'(acc >> curr_shift_w) | ptr;
    endfunction

    // Use 24-bit accumulators for mixing to provide more headroom and prevent clipping
    localparam int MIXER_SUM_RESOLUTION = 24;

    localparam int TOP_BIT_OFFSET = 6;   // Skip this many bits from the top of the accumulator
    localparam int WINDOW_SIZE = 15;     // Use this many bits for magnitude

    reg signed [15:0] channel_r[16];
    assign channel_o = channel_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_channel_r[16];
    reg signed [MIXER_SUM_RESOLUTION-1:0] channel_sum_r;

    reg signed [15:0] mono_mix_r;
    assign mono_mix_o = mono_mix_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_mono_mix_r;

    reg signed [15:0] left_mix_r;
    assign left_mix_o = left_mix_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_left_mix_r;

    reg signed [15:0] right_mix_r;
    assign right_mix_o = right_mix_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_right_mix_r;

    localparam [1:0] MODE_FREE = 2'b00;
    localparam [1:0] MODE_ONE_SHOT = 2'b01;
    localparam [1:0] MODE_SYNC_AM = 2'b10;
    localparam [1:0] MODE_SWAP = 2'b11;

    // Use bits 5:1 to determine the number of oscillators enabled (divide by 2)
    wire [4:0] osc_max_w = osc_en_r[5:1];

    // Timing generator

    localparam int TICKS_PER_CYCLE = CLOCK_SPEED_HZ / (DOC_CLOCK_SPEED_HZ / 8);
    localparam int CYCLE_WIDTH = (TICKS_PER_CYCLE <= 1) ? 1 : $clog2(TICKS_PER_CYCLE + 1);
    logic [CYCLE_WIDTH-1:0] cycle_timer_r;

    localparam [1:0] CYCLE_RESET = 2'b00;
    localparam [1:0] CYCLE_OSC = 2'b01;
    localparam [1:0] CYCLE_REFRESH_0 = 2'b10;
    localparam [1:0] CYCLE_REFRESH_1 = 2'b11;

    localparam int OSC_STATE_COUNT = 13;
    typedef enum logic [$clog2(OSC_STATE_COUNT)-1:0] {
        OSC_IDLE,
        OSC_START,
        OSC_LOAD_REGISTERS,
        OSC_LOAD_PARTNER_CONTROL,
        OSC_LOAD_NEXT_CONTROL,
        OSC_CONSUME,               // replaces baseline OSC_REQUEST_DATA + OSC_HANDLE_DATA
        OSC_OUT,
        OSC_MIX,
        OSC_ACC,
        OSC_HALT,
        OSC_HALT_ONE_SHOT_OR_ZERO_BYTE,
        OSC_START_PARTNER,
        OSC_RETRIGGER
    } osc_state_e;
    osc_state_e osc_state_r;

    reg [1:0] cycle_state_r;
    wire ready_w = (cycle_state_r != CYCLE_RESET);
    assign ready_o = ready_w;
    reg cycle_start_r;

    reg [2:0] clk_count_r;
    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            clk_count_r <= '0;
            cycle_timer_r <= '0;
            cycle_state_r <= CYCLE_RESET;
            curr_osc_r <= '0;
            cycle_start_r <= 1'b0;
        end else begin
            cycle_timer_r <= cycle_timer_r + 1'd1;
            cycle_start_r <= 1'b0;
            if (clk_en_i) begin
                clk_count_r <= clk_count_r + 1'd1;
                if (clk_count_r == 3'b111) begin
                    cycle_timer_r <= '0;
                    cycle_start_r <= 1'b1;

                    if (cycle_state_r == CYCLE_RESET) begin
                        curr_osc_r <= '0;
                        cycle_state_r <= CYCLE_OSC;
                    end else if (cycle_state_r == CYCLE_OSC) begin
                        if (curr_osc_r == osc_max_w) begin
                            cycle_state_r <= CYCLE_REFRESH_0;
                        end else begin
                            curr_osc_r <= curr_osc_r + 1'd1;
                        end
                    end else if (cycle_state_r == CYCLE_REFRESH_0) begin
                        cycle_state_r <= CYCLE_REFRESH_1;
                    end else if (cycle_state_r == CYCLE_REFRESH_1) begin
                        curr_osc_r <= '0;
                        cycle_state_r <= CYCLE_OSC;
                    end
                end
            end
        end
    end

    reg halt_zero_r = 1'b0;
    reg halt_overflow_r = 1'b0;

    // Resume/issue address for the current oscillator. Defaulted at
    // OSC_CONSUME to the address of THIS slot's expected fetch (correct for
    // the zero-byte path, where ACC does not advance), overwritten at
    // OSC_ACC with the post-update ACC's address (the normal case).
    reg [15:0] issue_addr_r;

    reg host_request_pending_r = 1'b0;
    reg device_response_pending_r = 1'b0;

    always_ff @(posedge clk_i) begin
        if (!reset_n_i) begin
            osc_state_r <= OSC_IDLE;
            wave_address_o <= '0;
            wave_rd_o <= '0;
            halt_zero_r <= 1'b0;
            halt_overflow_r <= 1'b0;
            issue_addr_r <= '0;

            host_request_pending_r <= 1'b0;
            device_response_pending_r <= 1'b0;

            fetch_valid_r <= '0;
            fetch_src_run_r <= '0;
            fq_wr_ptr_r <= '0;
            fq_issue_ptr_r <= '0;
            fq_ret_ptr_r <= '0;
            dbg_prime_miss_r <= '0;
            dbg_stale_fetch_r <= '0;
            dbg_fetch_drop_r <= '0;

            // Reset all oscillator RAM control signals

            ram_fl_osc_r <= '0;
            ram_fh_osc_r <= '0;
            ram_vol_osc_r <= '0;
            ram_wds_osc_r <= '0;
            ram_wtp_osc_r <= '0;
            ram_control_osc_r <= '0;
            ram_rts_osc_r <= '0;
            ram_acc_osc_r <= '0;

            ram_fl_din_r <= '0;
            ram_fh_din_r <= '0;
            ram_vol_din_r <= '0;
            ram_wds_din_r <= '0;
            ram_wtp_din_r <= '0;
            ram_control_din_r <= '0;
            ram_rts_din_r <= '0;
            ram_acc_din_r <= '0;

            ram_fl_we_r <= 1'b0;
            ram_fh_we_r <= 1'b0;
            ram_vol_we_r <= 1'b0;
            ram_wds_we_r <= 1'b0;
            ram_wtp_we_r <= 1'b0;
            ram_control_we_r <= 1'b0;
            ram_rts_we_r <= 1'b0;
            ram_acc_we_r <= 1'b0;

        end else begin

            wave_rd_o <= '0;

            if (host_access_r) begin
                host_request_pending_r <= 1'b1;
            end
            device_response_pending_r <= 1'b0;

            // Default all target oscillators to the current oscillator
            ram_fl_osc_r <= curr_osc_r;
            ram_fh_osc_r <= curr_osc_r;
            ram_vol_osc_r <= curr_osc_r;
            ram_wds_osc_r <= curr_osc_r;
            ram_wtp_osc_r <= curr_osc_r;
            ram_control_osc_r <= curr_osc_r;
            ram_rts_osc_r <= curr_osc_r;
            ram_acc_osc_r <= curr_osc_r;

            // Default all target write enables to 0
            ram_fl_we_r <= 1'b0;
            ram_fh_we_r <= 1'b0;
            ram_vol_we_r <= 1'b0;
            ram_wds_we_r <= 1'b0;
            ram_wtp_we_r <= 1'b0;
            ram_control_we_r <= 1'b0;
            ram_rts_we_r <= 1'b0;
            ram_acc_we_r <= 1'b0;

            // -----------------------------------------------------------------
            // Fetch retire: attribute the in-order memory response to the
            // oldest issued request. Runs every clk_i, independent of the
            // FSM state (returns may land during refresh slots or while a
            // different oscillator is being serviced). Placed BEFORE the
            // FSM case so that FSM writes to the result store (src_run
            // downgrade on a halted slot, host clear) win same-cycle
            // conflicts on the same entry.
            // -----------------------------------------------------------------
            if (wave_data_ready_i && (fq_ret_ptr_r != fq_issue_ptr_r)) begin
                fetch_data_r[fq_osc_r[fq_ret_ptr_r]]    <= wave_data_i;
                fetch_tag_r[fq_osc_r[fq_ret_ptr_r]]     <= fq_addr_r[fq_ret_ptr_r];
                fetch_valid_r[fq_osc_r[fq_ret_ptr_r]]   <= 1'b1;
                fetch_src_run_r[fq_osc_r[fq_ret_ptr_r]] <= fq_src_r[fq_ret_ptr_r];
                fq_ret_ptr_r <= fq_ret_ptr_r + 3'd1;
            end

            // -----------------------------------------------------------------
            // Fetch issue: send the oldest unissued request to the memory
            // port whenever it can accept one. One request per two clk_i
            // max; the DDR3 CDC queues up to two.
            // -----------------------------------------------------------------
            if ((fq_issue_ptr_r != fq_wr_ptr_r) && wave_available_i && !wave_rd_o) begin
                wave_rd_o <= 1'b1;
                wave_address_o <= fq_addr_r[fq_issue_ptr_r];
                fq_issue_ptr_r <= fq_issue_ptr_r + 3'd1;
            end

            case (cycle_state_r)
                CYCLE_RESET: cycle_reset();
                CYCLE_OSC: cycle_osc();
                CYCLE_REFRESH_0: cycle_refresh0();
                CYCLE_REFRESH_1: cycle_refresh1();
            endcase // case (cycle_state_r)

        end // else: !if(!reset_n_i)

    end // always_ff

    task automatic fetch_enqueue(input logic [4:0] osc, input logic [15:0] addr,
                                 input logic src_run);
        if (!fq_full_w) begin
            fq_osc_r[fq_wr_ptr_r]  <= osc;
            fq_addr_r[fq_wr_ptr_r] <= addr;
            fq_src_r[fq_wr_ptr_r]  <= src_run;
            fq_wr_ptr_r <= fq_wr_ptr_r + 3'd1;
        end else begin
            dbg_fetch_drop_r <= dbg_fetch_drop_r + 8'd1;
        end
    endtask: fetch_enqueue

    task automatic host_request();
        // Handles CPU register read/write access to DOC registers

        if (host_request_pending_r) begin
            host_request_pending_r <= 1'b0;

            if (!host_we_n_r) begin
                data_o <= host_data_r;
                if (host_addr_r == 8'hE0) begin
                    // Oscillator Interrupt Register
                    osc_int_r <= host_data_r;
                end else if (host_addr_r == 8'hE1) begin
                    // Oscillator Enable Register
                    osc_en_r <= host_data_r;
                end else if (host_addr_r >= 8'h00 && host_addr_r <= 8'hDF) begin
                    // Oscillator Registers
                    case (host_addr_r[7:5])
                        3'b000: begin                               // $00-1F
                            ram_fl_osc_r <= host_addr_r[4:0];
                            ram_fl_din_r <= host_data_r;
                            ram_fl_we_r <= 1'b1;
                        end
                        3'b001: begin                               // $20-3F
                            ram_fh_osc_r <= host_addr_r[4:0];
                            ram_fh_din_r <= host_data_r;
                            ram_fh_we_r <= 1'b1;
                        end
                        3'b010: begin                               // $40-5F
                            ram_vol_osc_r <= host_addr_r[4:0];
                            ram_vol_din_r <= host_data_r;
                            ram_vol_we_r <= 1'b1;
                        end
                        3'b011: begin                               // $60-7F
                            ram_wds_osc_r <= host_addr_r[4:0];
                            ram_wds_din_r <= host_data_r;
                            ram_wds_we_r <= 1'b1;
                        end
                        3'b100: begin                               // $80-9F
                            ram_wtp_osc_r <= host_addr_r[4:0];
                            ram_wtp_din_r <= host_data_r;
                            ram_wtp_we_r <= 1'b1;
                        end
                        3'b101: begin                               // $A0-BF
                            ram_control_osc_r <= host_addr_r[4:0];
                            ram_control_din_r <= host_data_r;
                            ram_control_we_r <= 1'b1;
                            if (!host_data_r[0]) begin
                                ram_acc_osc_r <= host_addr_r[4:0];
                                ram_acc_din_r <= '0; // Reset the accumulator if halt bit is cleared
                                ram_acc_we_r <= 1'b1;
                                // Pipelined variant: any un-halting control
                                // write invalidates RUN-sourced data so a
                                // stale in-flight byte cannot be consumed
                                // (tag check governs from here on).
                                fetch_src_run_r[host_addr_r[4:0]] <= 1'b0;
                            end
                        end
                        3'b110: begin                               // $C0-DF
                            ram_rts_osc_r <= host_addr_r[4:0];
                            ram_rts_din_r <= host_data_r;
                            ram_rts_we_r <= 1'b1;
                        end
                    endcase
                end
            end else begin
                // Host read access to oscillator registers
                device_response_pending_r <= 1'b1;

                if (host_addr_r >= 8'h00 && host_addr_r <= 8'hDF) begin
                    case (host_addr_r[7:5])
                        3'b000: ram_fl_osc_r <= host_addr_r[4:0];
                        3'b001: ram_fh_osc_r <= host_addr_r[4:0];
                        3'b010: ram_vol_osc_r <= host_addr_r[4:0];
                        3'b011: ram_wds_osc_r <= host_addr_r[4:0];
                        3'b100: ram_wtp_osc_r <= host_addr_r[4:0];
                        3'b101: ram_control_osc_r <= host_addr_r[4:0];
                        3'b110: ram_rts_osc_r <= host_addr_r[4:0];
                    endcase
                end
            end

        end
    endtask: host_request

    task automatic device_response();
        if (device_response_pending_r) begin
            device_response_pending_r <= 1'b0;
            // Handle device response to host request

            if (host_we_n_r) begin
                if (host_addr_r >= 8'h00 && host_addr_r <= 8'hDF) begin
                    // Read from oscillator registers
                    case (host_addr_r[7:5])
                        3'b000: begin                               // $00-1F
                            data_o <= ram_fl_dout_w;
                        end
                        3'b001: begin                               // $20-3F
                            data_o <= ram_fh_dout_w;
                        end
                        3'b010: begin                               // $40-5F
                            data_o <= ram_vol_dout_w;
                        end
                        3'b011: begin                               // $60-7F
                            data_o <= ram_wds_dout_w;
                        end
                        3'b100: begin                               // $80-9F
                            data_o <= ram_wtp_dout_w;
                        end
                        3'b101: begin                               // $A0-BF
                            data_o <= ram_control_dout_w;
                        end
                        3'b110: begin                               // $C0-DF
                            data_o <= ram_rts_dout_w;
                        end
                    endcase
                end else if (host_addr_r == 8'hE0) begin
                    // Oscillator Interrupt Register
                    data_o <= osc_int_r;
                end else if (host_addr_r == 8'hE1) begin
                    // Oscillator Enable Register
                    data_o <= osc_en_r;
                end else begin
                    data_o <= '0; // Default value for unrecognized addresses
                end
            end
        end
    endtask: device_response

    task automatic host_access();
        host_request();
        device_response();
    endtask: host_access

    task automatic cycle_reset();
        // Initialize all oscillator registers during reset cycle

        ram_fl_osc_r <= cycle_timer_r[4:0];
        ram_fh_osc_r <= cycle_timer_r[4:0];
        ram_vol_osc_r <= cycle_timer_r[4:0];
        ram_wds_osc_r <= cycle_timer_r[4:0];
        ram_wtp_osc_r <= cycle_timer_r[4:0];
        ram_control_osc_r <= cycle_timer_r[4:0];
        ram_rts_osc_r <= cycle_timer_r[4:0];
        ram_acc_osc_r <= cycle_timer_r[4:0];

        ram_fl_we_r <= 1'b1;
        ram_fh_we_r <= 1'b1;
        ram_vol_we_r <= 1'b1;
        ram_wds_we_r <= 1'b1;
        ram_wtp_we_r <= 1'b1;
        ram_control_we_r <= 1'b1;
        ram_rts_we_r <= 1'b1;
        ram_acc_we_r <= 1'b1;

        ram_fl_din_r <= '0;
        ram_fh_din_r <= '0;
        ram_vol_din_r <= '0;
        ram_wds_din_r <= '0;
        ram_wtp_din_r <= '0;
        ram_control_din_r <= '1;
        ram_rts_din_r <= '0;
        ram_acc_din_r <= '0;

    endtask: cycle_reset

    task automatic cycle_refresh0();
        // First phase of output channel refresh cycle

        if (cycle_timer_r < 'd16) begin
            if (cycle_start_r) begin
                device_response();
            end

            channel_r[cycle_timer_r[3:0]] <= {
                next_channel_r[cycle_timer_r[3:0]][MIXER_SUM_RESOLUTION-1],
                next_channel_r[cycle_timer_r[3:0]][MIXER_SUM_RESOLUTION-1-TOP_BIT_OFFSET -: WINDOW_SIZE]
            };

            if (cycle_timer_r[3:0] == 4'hF) begin
                mono_mix_r <= {
                    next_mono_mix_r[MIXER_SUM_RESOLUTION-1],
                    next_mono_mix_r[MIXER_SUM_RESOLUTION-1-TOP_BIT_OFFSET -: WINDOW_SIZE]
                };
                left_mix_r <= {
                    next_left_mix_r[MIXER_SUM_RESOLUTION-1],
                    next_left_mix_r[MIXER_SUM_RESOLUTION-1-TOP_BIT_OFFSET -: WINDOW_SIZE]
                };
                right_mix_r <= {
                    next_right_mix_r[MIXER_SUM_RESOLUTION-1],
                    next_right_mix_r[MIXER_SUM_RESOLUTION-1-TOP_BIT_OFFSET -: WINDOW_SIZE]
                };
            end
        end else begin
            // use remaining time to process host access
            host_access();
        end

    endtask: cycle_refresh0

    task automatic cycle_refresh1();
        // Second phase of output channel refresh cycle

        if (cycle_timer_r < 'd16) begin
            if (cycle_start_r) begin
                device_response();
            end

            next_channel_r[cycle_timer_r[3:0]] <= '0;

            if (cycle_timer_r[3:0] == 4'hF) begin
                next_mono_mix_r <= '0;
                next_left_mix_r <= '0;
                next_right_mix_r <= '0;
        end
        end else begin
            // use remaining time to process host access
            host_access();
        end

    endtask: cycle_refresh1

    task automatic cycle_osc();
        // Force oscillator state machine to idle state if at the start of a cycle
        automatic osc_state_e osc_state_w;
        osc_state_w = osc_state_e'(cycle_start_r ? OSC_IDLE : osc_state_r);
        case (osc_state_w)
            OSC_IDLE: osc_idle();
            OSC_START: osc_start();
            OSC_LOAD_REGISTERS: osc_load_registers();
            OSC_LOAD_PARTNER_CONTROL: osc_load_partner_control();
            OSC_LOAD_NEXT_CONTROL: osc_load_next_control();
            OSC_CONSUME: osc_consume();
            OSC_OUT: osc_out();
            OSC_MIX: osc_mix();
            OSC_ACC: osc_acc();
            OSC_HALT: osc_halt();
            OSC_HALT_ONE_SHOT_OR_ZERO_BYTE: osc_halt_one_shot_or_zero_byte();
            OSC_START_PARTNER: osc_start_partner();
            OSC_RETRIGGER: osc_retrigger();
            default: osc_idle();
        endcase // case (osc_state_r)
    endtask: cycle_osc

    task automatic osc_idle();
        osc_state_r <= OSC_IDLE;
        host_access();

        if (cycle_start_r) begin
            osc_state_r <= OSC_START;
        end

    endtask : osc_idle

    task automatic osc_start();
        device_response();

        // Init other working values
        curr_output_r <= '0;
        halt_zero_r <= 1'b0;
        halt_overflow_r <= 1'b0;

        osc_state_r <= OSC_LOAD_REGISTERS;
    endtask: osc_start

    task automatic osc_load_registers();
        curr_fl_r <= ram_fl_dout_w;                  // Frequency low
        curr_fh_r <= ram_fh_dout_w;                  // Frequency high
        curr_vol_r <= ram_vol_dout_w;                // Volume
        curr_wds_r <= ram_wds_dout_w;                // Waveform data sample
        curr_wtp_r <= ram_wtp_dout_w;                // Waveform table pointer
        curr_control_r <= ram_control_dout_w;        // Control
        curr_rts_r <= ram_rts_dout_w;                // Resolution/table size
        curr_acc_r <= ram_acc_dout_w;                // Accumulator

        ram_control_osc_r <= partner_osc_w;

        osc_state_r <= OSC_LOAD_PARTNER_CONTROL;
    endtask: osc_load_registers

    task automatic osc_load_partner_control();
        partner_control_r <= ram_control_dout_w;
        ram_control_osc_r <= curr_osc_r + 1'b1;
        osc_state_r <= OSC_LOAD_NEXT_CONTROL;
    endtask: osc_load_partner_control

    task automatic osc_load_next_control();
        next_control_r <= ram_control_dout_w;
        ram_control_osc_r <= curr_osc_r - 1'b1;
        osc_state_r <= OSC_CONSUME;
    endtask: osc_load_next_control

    task automatic osc_consume();
        // Consume the byte fetched for this oscillator during its previous
        // service slot, applying the result-store tagging rules from the
        // header. Replaces the baseline's in-slot fetch (OSC_REQUEST_DATA /
        // OSC_HANDLE_DATA) — the memory round trip already happened.
        //
        // expected_addr_w is the address the BASELINE would fetch this
        // slot; for a continuously-running oscillator it equals the address
        // this oscillator's own previous slot issued (the ACC value written
        // at the end of slot n-1 is the ACC value read at slot n).

        automatic logic [15:0] expected_addr_w = wave_addr_f(curr_acc_r);
        automatic logic entry_valid_w = fetch_valid_r[curr_osc_r];
        automatic logic entry_run_w = fetch_src_run_r[curr_osc_r];
        automatic logic entry_match_w = (fetch_tag_r[curr_osc_r] == expected_addr_w);
        automatic logic [7:0] entry_data_w = fetch_data_r[curr_osc_r];

        if (!halt_w) begin
            // Default resume address for paths that skip OSC_ACC (zero-byte
            // consumption followed by a partner-swap retrigger).
            issue_addr_r <= expected_addr_w;

            if (entry_valid_w && (entry_run_w || entry_match_w)) begin
                // Consume. RUN-sourced data is consumed even on tag
                // mismatch (late fetch → one-sample repeat; external
                // ACC/WTP/RTS rewrite → the correct delayed-stream byte).
                if (entry_run_w && !entry_match_w) begin
                    dbg_stale_fetch_r <= dbg_stale_fetch_r + 8'd1;
                end
                ram_wds_we_r <= 1'b1;
                ram_wds_din_r <= entry_data_w;
                curr_wds_r <= entry_data_w;
                if (entry_data_w == 8'h00) begin
                    halt_zero_r <= 1'b1;                            // Set halt zero flag
                    osc_state_r <= OSC_HALT;
                end else begin
                    osc_state_r <= OSC_OUT;
                end
            end else begin
                // Prime miss: nothing trustworthy to play. Output centerline
                // silence for one sample and do NOT evaluate halt-on-zero.
                // (Matches the real 5503's centerline output at a swap-mode
                // switch.) Note: like the baseline's timeout path, the wds
                // register is not updated.
                curr_wds_r <= 8'h80;
                dbg_prime_miss_r <= dbg_prime_miss_r + 8'd1;
                osc_state_r <= OSC_OUT;
            end

        end else begin
            // Halted service slot.
            // Baseline behavior: when halted in one-shot/swap mode, clear
            // the accumulator.
            if (curr_mode_w[0]) begin
                ram_acc_we_r <= 1'b1;
                ram_acc_din_r <= '0;
            end

            // Pipelined additions:
            // 1. Downgrade RUN-sourced data — after a halt, only
            //    tag-matched data may be consumed (a stale byte, worst
            //    case a stale 0x00 terminator, must not play at restart).
            fetch_src_run_r[curr_osc_r] <= 1'b0;

            // 2. Prefetch priming: every un-halt path zeroes the
            //    accumulator, so the next byte this oscillator will need
            //    is mem[wtp | 0]. Keep the result store primed with it.
            //    Re-issue only while the stored tag mismatches, so a
            //    parked oscillator settles to a single memory read.
            begin
                automatic logic [15:0] prime_addr_w = wave_addr_f(24'd0);
                if (!(entry_valid_w && (fetch_tag_r[curr_osc_r] == prime_addr_w))) begin
                    fetch_enqueue(curr_osc_r, prime_addr_w, 1'b0);
                end
            end

            // When halted, skip OUT and return to IDLE state
            osc_state_r <= OSC_IDLE;
        end

        if (curr_osc_r < 5'd8) begin
            debug_osc_mode_r[curr_osc_r[2:0]] <= curr_mode_w;
            debug_osc_halt_r[3'd7 - curr_osc_r[2:0]] <= curr_control_r[0];
        end

        // load prev control register (needed later)
        prev_control_r <= ram_control_dout_w;
    endtask: osc_consume

    task automatic osc_out();
        // Identical to baseline.

        if ((curr_mode_w == MODE_SYNC_AM) & curr_osc_odd_w) begin           // Sync AM Mode, odd oscillator outputs nothing
            if ((curr_osc_r != 5'd31) & !next_halt_w) begin                     // if next oscillator is not halted
                ram_vol_osc_r <= curr_osc_r + 1'b1;                      // set target oscillator to next one
                ram_vol_we_r <= 1'b1;                                    // write to volume register
                ram_vol_din_r <= curr_wds_r;                             // set volume to waveform data
            end
            // Skip mixing for odd oscillators in SYNC_AM mode
            osc_state_r <= OSC_ACC;
        end else begin
            // Fixed waveform data polarity and volume calculation
            automatic logic signed [7:0] data_w = curr_wds_r ^ 8'h80;       // convert waveform data to signed (8'h80 = 0)
            automatic logic signed [8:0] vol_s = {1'b0, curr_vol_r};        // convert volume to signed (unsigned expanded)
            automatic logic signed [17:0] output_w = data_w * vol_s;        // output is waveform data * volume (signed * signed)
            curr_output_r <= output_w;                                      // store calculated output
            channel_sum_r <= next_channel_r[curr_ca_w];                     // load channel accumulator

            osc_state_r <= OSC_MIX;
        end
    endtask: osc_out

    task automatic osc_mix();
        // Identical to baseline.

        next_channel_r[curr_ca_w] <= channel_sum_r + curr_output_r;

        next_mono_mix_r <= next_mono_mix_r + curr_output_r;

        if (curr_ca_w[0])
            next_right_mix_r <= next_right_mix_r + curr_output_r;
        else
            next_left_mix_r <= next_left_mix_r + curr_output_r;

        osc_state_r <= OSC_ACC;
    endtask: osc_mix

    task automatic osc_acc();
        // Baseline accumulator update, plus: compute the address of the
        // NEXT fetch from the post-update accumulator and issue it
        // fire-and-forget when this oscillator keeps running.

        automatic logic [24:0] temp_acc = curr_acc_r + {curr_fh_r, curr_fl_r};
        automatic int high_bit_w = 17 + curr_res_w;
        automatic logic overflow = temp_acc[high_bit_w];
        automatic logic [23:0] new_acc_w = temp_acc[23:0] & curr_acc_mask_w;
        automatic logic [15:0] new_addr_w = wave_addr_f(new_acc_w);
        halt_overflow_r <= overflow;
        ram_acc_we_r <= 1'b1;
        ram_acc_din_r <= new_acc_w;                             // wrap around address
        issue_addr_r <= new_addr_w;

        if (overflow) begin
            // Whether the oscillator continues depends on the mode
            // (evaluated in OSC_HALT_ONE_SHOT_OR_ZERO_BYTE / OSC_RETRIGGER);
            // the fetch is issued there if it survives.
            osc_state_r <= OSC_HALT;
        end else begin
            fetch_enqueue(curr_osc_r, new_addr_w, 1'b1);
            osc_state_r <= OSC_IDLE;
        end
    endtask: osc_acc

    task automatic osc_halt();
        // Identical to baseline.

        if (curr_mode_w == MODE_SYNC_AM) begin
            if (curr_osc_even_w) begin
                // we're even, so if the odd oscillator 1 below us is playing,
                // restart it.
                if (!prev_halt_w) begin
                    ram_acc_osc_r <= curr_osc_r - 1'b1;                // set target oscillator to previous one
                    ram_acc_we_r <= 1'b1;                              // write to accumulator register
                    ram_acc_din_r <= '0;                               // set accumulator to zero
                end
            end
        end
        osc_state_r <= OSC_HALT_ONE_SHOT_OR_ZERO_BYTE;
    endtask: osc_halt

    task automatic osc_halt_one_shot_or_zero_byte();
        // Baseline halt-decision logic, plus fetch issue for the
        // free-run/sync overflow case where the oscillator keeps running.

        automatic logic will_halt_w = curr_mode_w[0] || halt_zero_r;
        automatic logic goto_retrigger_w =
            (partner_mode_w == MODE_SWAP) && curr_osc_even_w && (curr_mode_w != MODE_SWAP);

        if (will_halt_w) begin
            ram_control_we_r <= 1'b1;                                   // write to control register
            ram_control_din_r <= {curr_control_r[7:1], 1'b1};           // set halt bit
            curr_control_r <= {curr_control_r[7:1], 1'b1};              // set halt bit in working copy
        end

        osc_state_r <= OSC_IDLE;

        // if we're in swap mode, start the partner
        if (curr_mode_w == MODE_SWAP) begin                             // Swap Mode
            osc_state_r <= OSC_START_PARTNER;
        end else begin
            // if we're not swap and we're the even oscillator of the pair and the partner's swap
            // but we aren't, we retrigger (!!!)  Verified on IIgs hardware.
            if (goto_retrigger_w) begin
                osc_state_r <= OSC_RETRIGGER;
            end else if (!will_halt_w) begin
                // Free-run / sync-AM table-end overflow: oscillator keeps
                // running from the wrapped accumulator — issue its fetch.
                fetch_enqueue(curr_osc_r, issue_addr_r, 1'b1);
            end
        end
    endtask: osc_halt_one_shot_or_zero_byte

    task automatic osc_start_partner();
        // Identical to baseline. The partner's first byte (mem[wtp | 0])
        // is already primed in its result store by prefetch-while-halted;
        // no fetch is issued here.

        ram_control_osc_r <= partner_osc_w;                         // set target oscillator to partner
        ram_control_we_r <= 1'b1;                                   // write to control register
        ram_control_din_r <= {partner_control_r[7:1], 1'b0};        // set halt bit to zero

        ram_acc_osc_r <= partner_osc_w;                             // set target oscillator to partner
        ram_acc_we_r <= 1'b1;                                       // write to accumulator register
        ram_acc_din_r <= '0;                                        // set accumulator to zero

        // After halting partner, skip the current output
        osc_state_r <= OSC_IDLE;
    endtask: osc_start_partner

    task automatic osc_retrigger();
        // Baseline retrigger (halt bit cleared again), plus fetch issue —
        // the oscillator continues running, from the post-ACC accumulator
        // on the overflow path or the unchanged accumulator on the
        // zero-byte path (issue_addr_r holds the right address either way).

        ram_control_we_r <= 1'b1;                                   // write to control register
        ram_control_din_r <= {curr_control_r[7:1], 1'b0};           // set halt bit to zero

        fetch_enqueue(curr_osc_r, issue_addr_r, 1'b1);

        osc_state_r <= OSC_IDLE;
    endtask: osc_retrigger

endmodule
