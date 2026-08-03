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
// read tail is ~1.0-1.2 us.
//
// This variant decouples fetch from consume:
//
//   * During oscillator k's service slot the FSM CONSUMES data that was
//     fetched for k earlier (held in a per-oscillator word cache), runs
//     the full baseline slot semantics on it (zero-byte halt, output, mix,
//     ACC += FC, overflow/one-shot/swap handling), and then — only when
//     the walk crosses a 32-bit word boundary — ISSUES the next fetch
//     fire-and-forget from the just-updated accumulator.
//   * A fetch has until k's NEXT service slot to land:
//     (N_enabled + 2) x 1.117 us >= 3.35 us (vs 1.2 us worst-case DDR3).
//   * A fetch that outlives a full service period degrades to serving the
//     cached (stale) word for one slot (counted in dbg_stale_fetch_o,
//     halt-on-zero suppressed for that byte) and self-heals on arrival.
//
// =============================================================================
// FETCH TRAFFIC POLICY (hardware-driven revision)
// =============================================================================
//
// The first prototype issued one single-word DDR3 read per service slot
// (~895 kHz continuously, including re-priming halted oscillators every
// scan). On a2mega hardware that occupied 50-90% of the serialized
// ddr3_ports arbiter (each grant blocks ~600-1000 ns non-preemptibly) and
// starved the framebuffer reader — FB priority does not help, because the
// CDC round trip prevents the FB port from staying continuously pending,
// so an always-pending DOC steals alternate grants. Visible result:
// missed FB line deadlines and display corruption whenever music played.
//
// Revised policy — three mechanisms cut traffic to ~1/4 fetch per slot
// with typical frequencies, and to near zero when idle:
//
//   1. PER-OSCILLATOR WORD CACHE. Fetches return full 32-bit words (see
//      interface notes below). Each oscillator caches its last word
//      {data[31:0], tag = wave_addr[15:2], valid, src_run}; the byte lane
//      (wave_addr[1:0]) is selected at consume time inside this module.
//      A running oscillator issues a fetch only when the post-ACC address
//      leaves the last-issued word — sequential table walks with address
//      steps <= 1 byte/sample fetch once per 4 samples (~224k reads/s
//      worst case with all 32 oscillators running, ~15-20% arbiter
//      occupancy at 600-1000 ns/grant, vs ~50-90% before).
//   2. PRIME-ONCE FOR HALTED OSCILLATORS. Every un-halt path in this DOC
//      zeroes the accumulator, so a restarting oscillator always needs
//      the word at mem[wtp | 0] first. That word is prefetched ONCE per
//      priming EVENT — halt entry, host WTP/RTS write, cache flush,
//      reset — via a per-oscillator prime_pending bit, not re-checked
//      every scan pass. An idle system generates no steady-state DDR3
//      traffic at all.
//   3. FLUSH-DRIVEN INVALIDATION. sound_glu pulses cache_flush_i on every
//      GLU sound-RAM write. A flush marks every oscillator's last-issued
//      word invalid (forcing a running oscillator to re-fetch at its next
//      service slot) and sets all prime_pending bits, WITHOUT clearing
//      the cached data: the stale word remains consumable, so a mid-music
//      GLU write costs at most ~one service period of stale samples (the
//      same class of race as CPU-vs-DOC DRAM access on real hardware)
//      rather than a centerline click. Re-priming of halted oscillators
//      is deferred until PRIME_COOLDOWN_SLOTS service slots after the
//      last flush, so bulk sample uploads (back-to-back 1 MHz writes) do
//      not cause a prime storm; running oscillators do refresh once per
//      service period during an upload burst, preserving correctness.
//
// PREFETCH PRIMING removes the output delay entirely: with the first word
// primed before an oscillator starts, its first sample is consumed in its
// FIRST service slot — the same slot in which the baseline fetches and
// plays it. The one-service-period pipeline manifests only as *data
// staleness* (GLU-write visibility, WTP/RTS address-mapping changes, and
// SYNC restarts landing between fetch and consume — one sample per event).
// See boards/a2mega/docs/ensoniq_ddr3_pipelined_design.md.
//
// =============================================================================
// SCHEDULING AROUND THE REAL ARBITER CONTRACT (rev 3, hardware-driven)
// =============================================================================
//
// Rev-2 hardware testing showed the true constraint is not average traffic
// but SERVICE-TAIL LATENCY: the DOC is the lowest-priority client of a
// statically-prioritized, serialized arbiter that runs at ~90%+ utilization
// during active display (shadow video reads/writes, framebuffer writes, and
// a framebuffer line fetch of ~40 non-preemptible burst8 grants per 31.7 us
// line). Port-4 service tails reach tens of microseconds — beyond the rev-2
// one-service-period deadline — and every DOC grant taken during a line
// fetch directly lengthens the FB's already-marginal line time. Field
// symptoms: stale_fetch storm + fetch drops + FB line_not_ready.
//
// Rev 3 addresses both directions:
//
//   1. 16-BYTE CACHE LINES FETCHED AS 4-BEAT BURSTS. A serialized-arbiter
//      grant costs ~the same whether it returns 4 bytes or 16 (the
//      controller round trip dominates; 3 extra response beats add ~3
//      cycles at 81 MHz), so each fetch now uses the mem-port's 4-beat
//      burst read (one aligned 128-bit DDR3 word) and fills a 16-byte
//      line. Sequential walks at address steps <= 1 byte/sample fetch
//      once per 16 samples: all 32 oscillators running costs ~56-70k
//      grants/s ~= 2-3% of arbiter time (vs ~9% for the rev-2 word cache
//      and 50-90% for rev 1) — decisively below the framebuffer's
//      capacity margin, which the corrected contention model showed is a
//      CAPACITY question that scheduling alone cannot fix.
//   2. TWO-LINE RESULT STORE + LOOKAHEAD ISSUE. Each oscillator's store
//      holds two 16-byte lines, direct-mapped by wave_addr[4]
//      (consecutive lines alternate slots, so prefetching the next line
//      never evicts the line being consumed). At each ACC update the
//      module computes the walk address two further samples ahead; when
//      that lands in the NEXT line (and only then — a +2-line skip would
//      collide with the current line's slot and is left to the normal
//      crossing path), the fetch is issued early. For address steps <= 1
//      byte/sample this converts the fetch deadline from ~1 service
//      period to ~16, absorbing the arbiter's multi-us lowest-priority
//      service tails. Jump fetches (table wrap, WTP retarget, SYNC
//      restart) still use the 1-period crossing path and may go stale
//      under extreme contention — one counted sample per event.
//   3. FB-AWARE ISSUE GATING (fb_fetch_active_i). While the framebuffer
//      line fetch is active, DOC issue is held so DOC grants land in the
//      per-line residual and vertical blanking instead of stretching the
//      line fetch. Once the oldest queued fetch has waited
//      FB_GATE_ESCAPE_SLOTS service slots, the gate is forced open until
//      the queue drains (a single-issue escape would cap gated
//      throughput below demand and jam the queue). Tie fb_fetch_active_i
//      low to disable gating.
//   4. The internal fetch FIFO (8 deep) lets a burst of near-simultaneous
//      line crossings from many oscillators drain over multiple slots
//      (or a whole gated display line) harmlessly under lookahead slack.
//
// STORAGE (GW5AT-60 placement, revs 3.1/3.2): all wide per-oscillator
// state lives in inferred RAM, not flip-flops — doc_cache_ram (64 x
// {tag, 128-bit line}) and two doc_lu_ram (32 x 12 last-issued-line
// bookkeeping, one per cache-slot parity, read at address curr_osc_r,
// which is stable all slot). Only flag bits, the 8-deep fetch FIFO, and
// narrow working registers remain as FFs (~0.7k added vs baseline). The
// flat-FF version of this state cost ~9.7k registers and failed
// placement at 88% CLS.
//
// WORD-CACHE CONSUME RULES (correctness-critical):
//
//   * Running oscillator, tag hit: consume the addressed byte lane;
//     halt-on-zero active. (Exact — the common case.)
//   * Running oscillator, src_run entry with tag MISS (late fetch, or an
//     external ACC rewrite such as a SYNC-mode partner restart): consume
//     the expected lane of the stale word for continuity, count it in
//     dbg_stale_fetch_o, and SUPPRESS halt-on-zero — an unvisited lane of
//     a stale word may legitimately contain 0x00 and must not fake a
//     terminator. (A genuinely missed 0x00 terminator under overload
//     degrades to halting at table end via the overflow path — bounded.)
//   * No valid entry / prefetch entry with tag miss: output centerline
//     0x80 for one sample, halt-on-zero suppressed. (The real 5503 emits
//     exactly this centerline sample at a swap-mode switch — see
//     R. Belmont's notes in MAME es5503.cpp.)
//   * SWAP-ON-ARRIVAL (rev 3.7): NO path may clear a cache entry's
//     validity or consume-eligibility while replacement data is still in
//     flight. Dirty-range hits, restart ACC resets and cooldown re-primes
//     all leave the entry valid and consumable (serving the documented
//     one-to-two-period-stale data) until retire atomically replaces
//     {tag, line}. The ONLY permitted 0x80 is a true cold start (no valid
//     data ever fetched, or a prefetch-sourced entry whose tag mismatches
//     after a table retarget). The stale-0x00 restart hazard is covered
//     structurally: halt-on-zero fires ONLY on tag-matched entries —
//     a match guarantees the byte is genuine memory content of the
//     expected address — and every mismatched consume is zero-suppressed
//     and counted in dbg_stale_fetch_o.
//
// INTERFACE (differences from baseline doc5503):
//
//   * wave_address_o is a BYTE address, always 16-byte aligned; the
//     memory client must issue a 4-beat BURST read of the 128-bit DDR3
//     word at wave_address_o[15:4] (mem_port_if: addr = {7'b0,
//     wave_address_o[15:2]}, burst = 1 — addr[1:0] is 0 by alignment)
//     and forward the four response beats UNMODIFIED on wave_data_word_i,
//     one wave_data_ready_i pulse per beat, strictly in order (beat 0 =
//     bytes 0-3). For the a2mega DDR3 path this is simply doc_mem_if.q /
//     doc_mem_if.ready — no byte-lane handling is needed in sound_glu
//     (lane selection happens here).
//   * wave_data_i (legacy byte return) is retained for port compatibility
//     but is UNUSED by this module; tie it off.
//   * wave_available_i: memory port can accept a request (ddr3_port_cdc
//     'available'; tie 1 for BSRAM).
//   * cache_flush_i: pulse (>= 1 clk_i) on any GLU sound-RAM write, with
//     the written BYTE address on cache_flush_addr_i (sound_glu:
//     .cache_flush_i(glu_mem_wr_r),
//     .cache_flush_addr_i({glu_mem_addr_r[13:0], 2'b00})). Rev 3.5:
//     invalidation is TARGETED via a dirty line range — bulk invalidation
//     stormed the arbiter on titles that stream sound RAM during playback.
//     A write is dirty-detected within one service period and consumed
//     fresh within two; holding the pulse across bursts is safe.
//
// Self-contained: the oscillator register banks use osc_reg_ram_dp
// (defined below) — synchronous-read dual-port BSRAM, because the GW5A
// family has NO SSRAM/distributed-RAM resource (GowinSynthesis RP0007;
// the baseline's async-read osc_reg_ram falls back to ~2.8k flip-flops
// there). Port A serves the FSM (shared read/write address, exactly the
// baseline access pattern with control-read addresses launched one state
// early); port B serves host register reads.
//
// =============================================================================

module doc5503_pipelined #(
    parameter int CLOCK_SPEED_HZ = 54_000_000,
    parameter int DOC_CLOCK_SPEED_HZ = 7_159_090, // 7.15909 MHz
    // Service slots of GLU-write silence required before halted-oscillator
    // re-priming resumes (coalesces bulk sample uploads into one re-prime).
    parameter int PRIME_COOLDOWN_SLOTS = 64,
    // 1 = issue next-word fetches two samples ahead of the walk (rev 3);
    // 0 = issue only at the actual word crossing (rev-2 behavior, kept for
    // the differential testbench's failure-reproduction mode).
    parameter bit FETCH_LOOKAHEAD = 1'b1,
    // Max service slots the oldest queued fetch may be held by the
    // framebuffer-activity gate before issuing anyway.
    parameter int FB_GATE_ESCAPE_SLOTS = 8,
    // 1 (default): oscillator register banks in dual-port BSRAM with the
    //   rev-3.3 shifted read chain and port-B host reads (GW5A has no
    //   SSRAM; FF banks cost ~2.8k flip-flops there).
    // 0: rev-3.2-style fallback — banks as async-read FF/LUT arrays
    //   (osc_reg_ram from doc5503.sv) with the ORIGINAL baseline read
    //   timing (chain launches and 1-cycle host reads). Hardware
    //   discrimination knob: register handling is then baseline-
    //   equivalent while all cache/invalidation/timing work is kept.
    parameter bit BANKS_IN_BSRAM = 1'b1
) (
    input clk_i,
    input reset_n_i,
    input clk_en_i,

    input cs_n_i,
    input we_n_i,

    input [7:0] addr_i,
    input [7:0] data_i,
    output reg [7:0] data_o,

    output reg [15:0] wave_address_o,   // BYTE address; client fetches word [15:2]
    output reg wave_rd_o,
    input wave_available_i,             // Memory port can accept a request (tie 1 for BSRAM)
    input wave_data_ready_i,
    input [7:0] wave_data_i,            // LEGACY, unused — lane select is internal
    input [31:0] wave_data_word_i,      // Full 32-bit word at wave_address_o[15:2]

    input cache_flush_i,                // Pulse on any GLU sound-RAM write
    input [15:0] cache_flush_addr_i,    // BYTE address of that write (bits [15:4] used)

    // Framebuffer line-fetch-active hint (framebuffer_480p fetch_state_r ==
    // FETCH_RUN). While high, fetch issue is deferred (with an age escape)
    // so DOC grants do not lengthen the marginal FB line fetch. Tie low to
    // disable gating.
    input fb_fetch_active_i,

    output signed [15:0] mono_mix_o,
    output signed [15:0] left_mix_o,
    output signed [15:0] right_mix_o,

    output signed [15:0] channel_o[16],

    output logic ready_o,          // Indicate when out of reset and ready to process

    output [7:0] debug_osc_en_o,   // Debug output for oscillator enable register
    output [1:0] debug_osc_mode_o[8], // Debug output for oscillator mode register;
    output [7:0] debug_osc_halt_o, // Debug output for oscillator halt register

    // Pipelined-fetch diagnostics (wrap-around counters)
    output [7:0]  dbg_prime_miss_o,  // Consumed 0x80 because no primed data was available
    output [7:0]  dbg_stale_fetch_o, // Consumed a src_run word whose tag mismatched (late fetch or ACC rewrite)
    output [7:0]  dbg_fetch_drop_o,  // Fetch request dropped: internal FIFO full (should stay 0)
    output [15:0] dbg_fetch_count_o  // Total fetches issued (traffic verification)
);

    // Unused legacy input (see interface notes)
    wire _unused_wave_data_w = &{1'b0, wave_data_i};

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

    reg [4:0] ram_fl_osc_r;
    reg [4:0] ram_fh_osc_r;
    reg [4:0] ram_vol_osc_r;
    reg [4:0] ram_wds_osc_r;
    reg [4:0] ram_wtp_osc_r;
    reg [4:0] ram_control_osc_r;
    reg [4:0] ram_rts_osc_r;
    reg [4:0] ram_acc_osc_r;

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

    // Host-read port (bank port B). host_raddr_r PERSISTS until the next
    // host read, so the synchronous port-B data is valid from two clk_i
    // after a host read request FOREVER — the delayed device_response can
    // consume it at any later invocation with no staleness window and no
    // interference with the FSM's port-A addresses.
    reg  [4:0]  host_raddr_r;
    reg         host_read_wait_r;
    wire [7:0]  ram_fl_hdout_w;
    wire [7:0]  ram_fh_hdout_w;
    wire [7:0]  ram_vol_hdout_w;
    wire [7:0]  ram_wds_hdout_w;
    wire [7:0]  ram_wtp_hdout_w;
    wire [7:0]  ram_control_hdout_w;
    wire [7:0]  ram_rts_hdout_w;
    wire [23:0] ram_acc_hdout_w;

    // Instantiate the oscillator register banks.
    // BANKS_IN_BSRAM=1: dual-port BSRAM (osc_reg_ram_dp, defined below).
    // BANKS_IN_BSRAM=0: async-read FF/LUT banks (osc_reg_ram — requires
    // doc5503.sv in the file list) with hdout aliased to the shared-port
    // dout, restoring baseline-identical register-read behavior.
    generate
    if (BANKS_IN_BSRAM) begin : gen_banks_bsram
        osc_reg_ram_dp fl_ram (
            .clk_i(clk_i), .a_addr_i(ram_fl_osc_r), .a_we_i(ram_fl_we_r),
            .a_din_i(ram_fl_din_r), .a_dout_o(ram_fl_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_fl_hdout_w));
        osc_reg_ram_dp fh_ram (
            .clk_i(clk_i), .a_addr_i(ram_fh_osc_r), .a_we_i(ram_fh_we_r),
            .a_din_i(ram_fh_din_r), .a_dout_o(ram_fh_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_fh_hdout_w));
        osc_reg_ram_dp vol_ram (
            .clk_i(clk_i), .a_addr_i(ram_vol_osc_r), .a_we_i(ram_vol_we_r),
            .a_din_i(ram_vol_din_r), .a_dout_o(ram_vol_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_vol_hdout_w));
        osc_reg_ram_dp wds_ram (
            .clk_i(clk_i), .a_addr_i(ram_wds_osc_r), .a_we_i(ram_wds_we_r),
            .a_din_i(ram_wds_din_r), .a_dout_o(ram_wds_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_wds_hdout_w));
        osc_reg_ram_dp wtp_ram (
            .clk_i(clk_i), .a_addr_i(ram_wtp_osc_r), .a_we_i(ram_wtp_we_r),
            .a_din_i(ram_wtp_din_r), .a_dout_o(ram_wtp_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_wtp_hdout_w));
        osc_reg_ram_dp control_ram (
            .clk_i(clk_i), .a_addr_i(ram_control_osc_r), .a_we_i(ram_control_we_r),
            .a_din_i(ram_control_din_r), .a_dout_o(ram_control_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_control_hdout_w));
        osc_reg_ram_dp rts_ram (
            .clk_i(clk_i), .a_addr_i(ram_rts_osc_r), .a_we_i(ram_rts_we_r),
            .a_din_i(ram_rts_din_r), .a_dout_o(ram_rts_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_rts_hdout_w));
        osc_reg_ram_dp #(.DATA_WIDTH(24)) acc_ram (
            .clk_i(clk_i), .a_addr_i(ram_acc_osc_r), .a_we_i(ram_acc_we_r),
            .a_din_i(ram_acc_din_r), .a_dout_o(ram_acc_dout_w),
            .b_addr_i(host_raddr_r), .b_dout_o(ram_acc_hdout_w));
    end else begin : gen_banks_ff
        osc_reg_ram fl_ram (
            .clk_i(clk_i), .osc_i(ram_fl_osc_r), .we_i(ram_fl_we_r),
            .data_i(ram_fl_din_r), .data_o(ram_fl_dout_w));
        osc_reg_ram fh_ram (
            .clk_i(clk_i), .osc_i(ram_fh_osc_r), .we_i(ram_fh_we_r),
            .data_i(ram_fh_din_r), .data_o(ram_fh_dout_w));
        osc_reg_ram vol_ram (
            .clk_i(clk_i), .osc_i(ram_vol_osc_r), .we_i(ram_vol_we_r),
            .data_i(ram_vol_din_r), .data_o(ram_vol_dout_w));
        osc_reg_ram wds_ram (
            .clk_i(clk_i), .osc_i(ram_wds_osc_r), .we_i(ram_wds_we_r),
            .data_i(ram_wds_din_r), .data_o(ram_wds_dout_w));
        osc_reg_ram wtp_ram (
            .clk_i(clk_i), .osc_i(ram_wtp_osc_r), .we_i(ram_wtp_we_r),
            .data_i(ram_wtp_din_r), .data_o(ram_wtp_dout_w));
        osc_reg_ram control_ram (
            .clk_i(clk_i), .osc_i(ram_control_osc_r), .we_i(ram_control_we_r),
            .data_i(ram_control_din_r), .data_o(ram_control_dout_w));
        osc_reg_ram rts_ram (
            .clk_i(clk_i), .osc_i(ram_rts_osc_r), .we_i(ram_rts_we_r),
            .data_i(ram_rts_din_r), .data_o(ram_rts_dout_w));
        osc_reg_ram #(.DATA_WIDTH(24)) acc_ram (
            .clk_i(clk_i), .osc_i(ram_acc_osc_r), .we_i(ram_acc_we_r),
            .data_i(ram_acc_din_r), .data_o(ram_acc_dout_w));
        assign ram_fl_hdout_w      = ram_fl_dout_w;
        assign ram_fh_hdout_w      = ram_fh_dout_w;
        assign ram_vol_hdout_w     = ram_vol_dout_w;
        assign ram_wds_hdout_w     = ram_wds_dout_w;
        assign ram_wtp_hdout_w     = ram_wtp_dout_w;
        assign ram_control_hdout_w = ram_control_dout_w;
        assign ram_rts_hdout_w     = ram_rts_dout_w;
        assign ram_acc_hdout_w     = ram_acc_dout_w;
    end
    endgenerate

    // Current oscillator state, copied from the register file at the start of each cycle.
    // syn_preserve: GowinSynthesis retimes these capture registers INTO the
    // bank BSRAMs' output registers (netlist showed fl_ram's output port
    // becoming curr_fl_r directly), which launches the ACC-add / issue-
    // decision cones from a slow BSRAM Q and fails 54 MHz closure
    // (-0.277 ns family fl_ram -> fq_*). These FFs ARE the intended
    // fabric pipeline stage — keep them out of the primitives.
    (* syn_preserve = 1 *) reg [7:0] curr_fl_r;
    (* syn_preserve = 1 *) reg [7:0] curr_fh_r;
    (* syn_preserve = 1 *) reg [7:0] curr_vol_r;
    (* syn_preserve = 1 *) reg [7:0] curr_wds_r;
    (* syn_preserve = 1 *) reg [7:0] curr_wtp_r;
    (* syn_preserve = 1 *) reg [7:0] curr_control_r;
    (* syn_preserve = 1 *) reg [7:0] curr_rts_r;
    (* syn_preserve = 1 *) reg [7:0] partner_control_r;
    (* syn_preserve = 1 *) reg [7:0] next_control_r;
    (* syn_preserve = 1 *) reg [7:0] prev_control_r;

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

    (* syn_preserve = 1 *) reg [23:0] curr_acc_r;
    reg signed [17:0] curr_output_r;

    // =========================================================================
    // Per-oscillator line cache — TWO 16-byte slots per oscillator (rev 3)
    // =========================================================================
    // 32 x 2 x {line[127:0], tag[11:0] = wave_addr[15:4], valid, src_run},
    // direct-mapped by wave_addr[4] (bit 0 of the line address), so
    // consecutive lines alternate slots and a next-line prefetch never
    // evicts the line currently being consumed. Entry index = {osc, tag[0]}.
    // Fetch-policy bookkeeping:
    //   prime_pending : this halted oscillator needs (re)priming with the
    //                   word at mem[wtp | 0] (set on halt entry, host
    //                   WTP/RTS write, cache flush, reset; cleared when
    //                   the prime fetch is enqueued)
    //   issued_valid / last_issued_word : PER CACHE SLOT (64 entries),
    //                   the word address of the most recent successfully
    //                   enqueued fetch targeting that slot — an issue
    //                   happens only when the wanted word differs from
    //                   what that slot already has in flight/cached (a
    //                   single per-oscillator record cannot represent
    //                   "issued one word ahead" without spuriously
    //                   re-issuing the current word). A cache flush
    //                   clears issued_valid to force refreshes.
    //
    // STORAGE (Gowin placement, rev 3.1): the {tag, line} payload — 64 x
    // 140 bits — lives in an inferred distributed RAM (doc_cache_ram,
    // separate write/read always blocks, synchronous read), NOT in flip-
    // flops: as a flat FF array it cost ~9k registers and failed GW5AT-60
    // placement. Packing the tag WITH the line keeps them atomically
    // coherent across the 1-cycle read latency, so a retire landing on
    // the read-sample edge can never pair a new tag with old data (the
    // valid/src flag bits, which may be one cycle newer, only ever cause
    // a benign counted stale/prime-miss, never a wrong tag-accept). The
    // read address depends only on curr_acc_r/curr_wtp_r/curr_rts_r,
    // which settle two clk_i before OSC_CONSUME samples the read data —
    // no extra FSM state is needed. Flag/bookkeeping bits stay in FFs
    // (combinational access on the issue path).

    reg [63:0]  cache_valid_r;
    reg [63:0]  cache_src_run_r;

    // {tag[11:0], line[127:0]} storage
    wire         cram_we_w;
    wire [5:0]   cram_waddr_w;
    wire [139:0] cram_wdata_w;
    wire [5:0]   cram_raddr_w;
    wire [139:0] cram_rdata_w;

    doc_cache_ram cache_ram (
        .clk_i(clk_i),
        .we_i(cram_we_w),
        .waddr_i(cram_waddr_w),
        .wdata_i(cram_wdata_w),
        .raddr_i(cram_raddr_w),
        .rdata_o(cram_rdata_w)
    );

    reg [31:0]  prime_pending_r;
    reg [63:0]  issued_valid_r;

    // last-issued line-address bookkeeping (rev 3.2): one 32x12 RAM per
    // cache-slot parity instead of a 64x12 FF array (768 FF -> RAM). Read
    // address is simply curr_osc_r — stable for the whole service slot,
    // so the synchronous read needs no scheduling at all (data valid from
    // the slot's second clk_i; first use is at OSC_ACC, ~7 clk_i in).
    // Single writer per clk_i: issue_running (OSC_ACC / halt chain /
    // retrigger) and the prime path (halted OSC_CONSUME) never coincide,
    // and at most one issue fires per slot, so the registered write
    // strobe (+1 clk_i) commits a full scan before that oscillator's
    // bookkeeping is read again. issued_valid_r stays in FFs (flush must
    // clear all 64 bits in one cycle).
    reg        lu_we0_r, lu_we1_r;
    reg [4:0]  lu_waddr_r;
    reg [11:0] lu_wdata_r;
    wire [11:0] lu_rdata0_w, lu_rdata1_w;

    doc_lu_ram lu_ram0 (
        .clk_i(clk_i),
        .we_i(lu_we0_r),
        .waddr_i(lu_waddr_r),
        .wdata_i(lu_wdata_r),
        .raddr_i(curr_osc_r),
        .rdata_o(lu_rdata0_w)
    );

    doc_lu_ram lu_ram1 (
        .clk_i(clk_i),
        .we_i(lu_we1_r),
        .waddr_i(lu_waddr_r),
        .wdata_i(lu_wdata_r),
        .raddr_i(curr_osc_r),
        .rdata_o(lu_rdata1_w)
    );

    // Last-issued line address for a given (this-oscillator) byte address
    function automatic logic [11:0] lu_sel_f(input logic [15:0] addr);
        return addr[4] ? lu_rdata1_r : lu_rdata0_r;
    endfunction

    // Burst-beat assembly: the memory client returns each 16-byte line as
    // four 32-bit beats (one wave_data_ready_i pulse each, in order).
    reg [1:0]   beat_cnt_r;
    reg [95:0]  beat_accum_r;

    // Slots of GLU-write silence before halted re-priming resumes
    localparam int COOLDOWN_W = (PRIME_COOLDOWN_SLOTS <= 1) ? 1
                                : $clog2(PRIME_COOLDOWN_SLOTS + 1);
    reg [COOLDOWN_W-1:0] flush_cooldown_r;

    // -----------------------------------------------------------------
    // Targeted invalidation: two-generation DIRTY LINE RANGE (rev 3.5).
    // Bulk invalidation on every GLU write caused fetch storms on titles
    // that stream sound RAM DURING playback (up to ~1 MHz): every write
    // forced every running oscillator to refetch each service period.
    // Instead, writes accumulate a [lo, hi] line-address range in
    // generation A; once per scan A rotates into B. At each running
    // service slot the oscillator's two cached/issued line addresses —
    // already at hand in lu_rdata0/1_w — are compared against A ∪ B;
    // only on a hit is that slot's issued_valid cleared (one refetch).
    // Invariant: a write during scan n is in A for services later in
    // scan n and in B throughout scan n+1, so every running oscillator
    // checks it within one service period. Visibility semantics: dirty
    // detection <= 1 service period after the write, fresh data consumed
    // <= 2 periods (the refetch issues in the detecting slot). The range
    // is conservative (a superset), which costs only occasional extra
    // refetches; sequential streaming writes keep it a few lines wide.
    // Halted oscillators are covered by prime_pending + cooldown as
    // before (one re-prime per oscillator after writes quiesce).
    // -----------------------------------------------------------------
    reg [11:0] dirty_a_lo_r, dirty_a_hi_r;
    reg        dirty_a_v_r;
    reg [11:0] dirty_b_lo_r, dirty_b_hi_r;
    reg        dirty_b_v_r;
    wire [11:0] flush_line_w = cache_flush_addr_i[15:4];

    // Fabric pipeline registers on the BSRAM read outputs (timing: BSRAM
    // clock-to-out must feed exactly one fabric FF, not the issue/consume
    // comb cones). Functionally free: both sources are stable at least
    // one cycle before their first consumer (see the rev-3.1 cache-read
    // and rev-3.2 lu-read scheduling analyses); a retire landing in the
    // extra cycle simply means the coherent {tag, line} pair from one
    // cycle earlier is consumed — the same benign-stale classes as before.
    reg [139:0] cram_rdata_r;
    reg [11:0]  lu_rdata0_r, lu_rdata1_r;
    always @(posedge clk_i) begin
        cram_rdata_r <= cram_rdata_w;
        lu_rdata0_r  <= lu_rdata0_w;
        lu_rdata1_r  <= lu_rdata1_w;
    end

    // Once-per-scan generation rotation point
    wire dirty_rotate_w = cycle_start_r && (cycle_state_r == CYCLE_REFRESH_1);

    function automatic logic dirty_hit_f(input logic [11:0] line);
        return (dirty_a_v_r && (line >= dirty_a_lo_r) && (line <= dirty_a_hi_r))
            || (dirty_b_v_r && (line >= dirty_b_lo_r) && (line <= dirty_b_hi_r));
    endfunction

    // Effective byte address of the most recently consumed sample
    // ({cache tag, lane}) — diagnostics / simulation logging only.
    reg [15:0] consume_addr_r;

    // =========================================================================
    // Fetch request FIFO (issue side of the fire-and-forget pipeline)
    // =========================================================================
    // Depth 8, three pointers: wr (enqueue by FSM), issue (sent to memory
    // port), ret (retired by wave_data_ready_i, in order).

    // Rev 3.2: 8 deep (lookahead + gating drain analysis keeps occupancy
    // shallow; the drop counter guards and drops self-heal) and 12-bit
    // LINE addresses per entry (byte-lane bits were never consumed).
    localparam int FQ_DEPTH = 8;
    reg [4:0]  fq_osc_r  [FQ_DEPTH];
    reg [11:0] fq_line_r [FQ_DEPTH];
    reg        fq_src_r  [FQ_DEPTH];
    reg [2:0]  fq_wr_ptr_r;
    reg [2:0]  fq_issue_ptr_r;
    reg [2:0]  fq_ret_ptr_r;

    wire fq_full_w = (3'(fq_wr_ptr_r + 3'd1) == fq_ret_ptr_r);

    // FB-gate age escape with drain hysteresis: count service slots the
    // queue has been held by the framebuffer gate; once the head ages past
    // the escape, FORCE the gate open until the queue fully drains (a
    // single-issue escape would cap gated throughput at 1/ESCAPE slots —
    // below steady demand — and jam the queue).
    localparam int GATE_W = (FB_GATE_ESCAPE_SLOTS <= 1) ? 1
                            : $clog2(FB_GATE_ESCAPE_SLOTS + 1);
    reg [GATE_W-1:0] issue_wait_r;
    reg gate_forced_r;
    wire gate_open_w = !fb_fetch_active_i || gate_forced_r;

    // 4-bit wrap counters (rev 3.2 register diet), zero-padded to keep
    // the 8-bit output ports (interface unchanged).
    reg [3:0]  dbg_prime_miss_r;
    reg [3:0]  dbg_stale_fetch_r;
    reg [3:0]  dbg_fetch_drop_r;
    reg [15:0] dbg_fetch_count_r;
    assign dbg_prime_miss_o  = {4'b0, dbg_prime_miss_r};
    assign dbg_stale_fetch_o = {4'b0, dbg_stale_fetch_r};
    assign dbg_fetch_drop_o  = {4'b0, dbg_fetch_drop_r};
    assign dbg_fetch_count_o = dbg_fetch_count_r;

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

    // ---------------------------------------------------------------------
    // Cache RAM wiring.
    // Write: fires on the retire of a request's 4th beat, same edge as the
    // valid/src flag updates (tag and line stay atomically coherent).
    // Read: the consume-side entry address depends only on curr_* register
    // state, which settles at the end of OSC_LOAD_REGISTERS — two clk_i
    // before OSC_CONSUME samples cram_rdata_w — so the synchronous read
    // needs no extra FSM state.
    // ---------------------------------------------------------------------
    wire retire_last_beat_w = wave_data_ready_i
                              && (fq_ret_ptr_r != fq_issue_ptr_r)
                              && (beat_cnt_r == 2'd3)
                              && reset_n_i;
    assign cram_we_w    = retire_last_beat_w;
    assign cram_waddr_w = {fq_osc_r[fq_ret_ptr_r], fq_line_r[fq_ret_ptr_r][0]};
    assign cram_wdata_w = {fq_line_r[fq_ret_ptr_r], wave_data_word_i, beat_accum_r};

    wire [15:0] consume_pre_addr_w = wave_addr_f(curr_acc_r);
    assign cram_raddr_w = {curr_osc_r, consume_pre_addr_w[4]};

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
            consume_addr_r <= '0;

            host_request_pending_r <= 1'b0;
            device_response_pending_r <= 1'b0;
            host_raddr_r <= '0;
            host_read_wait_r <= 1'b0;

            cache_valid_r <= '0;
            cache_src_run_r <= '0;
            prime_pending_r <= '1;      // every oscillator needs one prime
            issued_valid_r <= '0;
            flush_cooldown_r <= '0;
            dirty_a_v_r <= 1'b0;
            dirty_b_v_r <= 1'b0;
            dirty_a_lo_r <= '0;
            dirty_a_hi_r <= '0;
            dirty_b_lo_r <= '0;
            dirty_b_hi_r <= '0;
            fq_wr_ptr_r <= '0;
            fq_issue_ptr_r <= '0;
            fq_ret_ptr_r <= '0;
            issue_wait_r <= '0;
            gate_forced_r <= 1'b0;
            beat_cnt_r <= '0;
            beat_accum_r <= '0;
            lu_we0_r <= 1'b0;
            lu_we1_r <= 1'b0;
            lu_waddr_r <= '0;
            lu_wdata_r <= '0;
            dbg_prime_miss_r <= '0;
            dbg_stale_fetch_r <= '0;
            dbg_fetch_drop_r <= '0;
            dbg_fetch_count_r <= '0;

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
            lu_we0_r <= 1'b0;
            lu_we1_r <= 1'b0;

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

            // Host-read response delay stage (see host_request read branch)
            if (host_read_wait_r) begin
                host_read_wait_r <= 1'b0;
                device_response_pending_r <= 1'b1;
            end

            // -----------------------------------------------------------------
            // Cache flush (GLU sound-RAM write): accumulate the written
            // LINE into dirty range A (targeted invalidation happens at
            // each oscillator's service slot — see the dirty-range block
            // comment) and schedule halted re-primes (cooldown-deferred).
            // Cached lines stay CONSUMABLE; NO bulk issued_valid clear —
            // that caused rev-3.4's write-storm fetch traffic.
            // -----------------------------------------------------------------
            begin
                if (dirty_rotate_w) begin
                    dirty_b_lo_r <= dirty_a_lo_r;
                    dirty_b_hi_r <= dirty_a_hi_r;
                    dirty_b_v_r  <= dirty_a_v_r;
                    if (cache_flush_i) begin
                        dirty_a_lo_r <= flush_line_w;
                        dirty_a_hi_r <= flush_line_w;
                        dirty_a_v_r  <= 1'b1;
                    end else begin
                        dirty_a_v_r  <= 1'b0;
                    end
                end else if (cache_flush_i) begin
                    if (!dirty_a_v_r) begin
                        dirty_a_lo_r <= flush_line_w;
                        dirty_a_hi_r <= flush_line_w;
                        dirty_a_v_r  <= 1'b1;
                    end else begin
                        if (flush_line_w < dirty_a_lo_r) dirty_a_lo_r <= flush_line_w;
                        if (flush_line_w > dirty_a_hi_r) dirty_a_hi_r <= flush_line_w;
                    end
                end
            end
            if (cache_flush_i) begin
                prime_pending_r <= '1;
                flush_cooldown_r <= COOLDOWN_W'(PRIME_COOLDOWN_SLOTS);
            end else if (cycle_start_r && (flush_cooldown_r != 0)) begin
                flush_cooldown_r <= flush_cooldown_r - 1'd1;
            end

            // -----------------------------------------------------------------
            // Fetch retire: attribute the in-order memory response to the
            // oldest issued request. Runs every clk_i, independent of the
            // FSM state. Placed BEFORE the FSM case so that FSM writes to
            // the cache (src_run downgrade on a halted slot, host clear)
            // win same-cycle conflicts on the same entry.
            // -----------------------------------------------------------------
            if (wave_data_ready_i && (fq_ret_ptr_r != fq_issue_ptr_r)) begin
                if (beat_cnt_r != 2'd3) begin
                    beat_accum_r[32*beat_cnt_r +: 32] <= wave_data_word_i;
                    beat_cnt_r <= beat_cnt_r + 2'd1;
                end else begin
                    // {tag, line} written into doc_cache_ram this same edge
                    // via the combinational cram_* wires below; only the
                    // flag bits live here.
                    cache_valid_r[{fq_osc_r[fq_ret_ptr_r], fq_line_r[fq_ret_ptr_r][0]}]   <= 1'b1;
                    cache_src_run_r[{fq_osc_r[fq_ret_ptr_r], fq_line_r[fq_ret_ptr_r][0]}] <= fq_src_r[fq_ret_ptr_r];
                    fq_ret_ptr_r <= fq_ret_ptr_r + 3'd1;
                    beat_cnt_r <= 2'd0;
                end
            end

            // -----------------------------------------------------------------
            // Fetch issue: send the oldest unissued request to the memory
            // port whenever it can accept one AND the framebuffer gate is
            // open (not in a line fetch, or the head has aged past the
            // escape). One request per two clk_i max; the DDR3 CDC queues
            // up to two.
            // -----------------------------------------------------------------
            if ((fq_issue_ptr_r != fq_wr_ptr_r) && wave_available_i && !wave_rd_o
                && gate_open_w) begin
                wave_rd_o <= 1'b1;
                wave_address_o <= {fq_line_r[fq_issue_ptr_r], 4'b0};
                fq_issue_ptr_r <= fq_issue_ptr_r + 3'd1;
                dbg_fetch_count_r <= dbg_fetch_count_r + 16'd1;
            end
            if (fq_issue_ptr_r == fq_wr_ptr_r) begin
                issue_wait_r <= '0;
                gate_forced_r <= 1'b0;
            end else if (!gate_forced_r && fb_fetch_active_i && cycle_start_r) begin
                if (issue_wait_r >= GATE_W'(FB_GATE_ESCAPE_SLOTS)) begin
                    gate_forced_r <= 1'b1;
                    issue_wait_r <= '0;
                end else begin
                    issue_wait_r <= issue_wait_r + 1'd1;
                end
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
            fq_line_r[fq_wr_ptr_r] <= addr[15:4];
            fq_src_r[fq_wr_ptr_r]  <= src_run;
            fq_wr_ptr_r <= fq_wr_ptr_r + 3'd1;
        end else begin
            dbg_fetch_drop_r <= dbg_fetch_drop_r + 4'd1;
        end
    endtask: fetch_enqueue

    // Running-oscillator issue with word-granularity gating: enqueue only
    // when the target word differs from what its cache slot already has
    // issued/cached (or a flush invalidated the bookkeeping). On FIFO-full
    // the bookkeeping is NOT updated, so the request retries at the next
    // boundary check.
    task automatic issue_running(input logic [4:0] osc, input logic [15:0] addr);
        automatic logic [5:0] idx_w = {osc, addr[4]};
        if (!issued_valid_r[idx_w] || (lu_sel_f(addr) != addr[15:4])) begin
            if (!fq_full_w) begin
                fetch_enqueue(osc, addr, 1'b1);
                lu_we0_r <= ~addr[4];
                lu_we1_r <= addr[4];
                lu_waddr_r <= osc;
                lu_wdata_r <= addr[15:4];
                issued_valid_r[idx_w] <= 1'b1;
            end else begin
                dbg_fetch_drop_r <= dbg_fetch_drop_r + 4'd1;
            end
        end
    endtask: issue_running

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
                            // Pipelined variant: the halted-prime address
                            // depends on WTP — schedule a re-prime.
                            prime_pending_r[host_addr_r[4:0]] <= 1'b1;
                        end
                        3'b101: begin                               // $A0-BF
                            ram_control_osc_r <= host_addr_r[4:0];
                            ram_control_din_r <= host_data_r;
                            ram_control_we_r <= 1'b1;
                            if (!host_data_r[0]) begin
                                ram_acc_osc_r <= host_addr_r[4:0];
                                ram_acc_din_r <= '0; // Reset the accumulator if halt bit is cleared
                                ram_acc_we_r <= 1'b1;
                                // Rev 3.7: NO src_run invalidation here.
                                // Retriggers of RUNNING oscillators (per-
                                // frame on real titles) must consume the
                                // stale cached line until fresh f(0) data
                                // arrives — the old clear degraded every
                                // such write to an audible 0x80 prime miss
                                // (invisible on stale/drop counters). The
                                // rev-1 stale-0x00 hazard is structurally
                                // covered by the consume invariant:
                                // halt-on-zero fires ONLY on tag-matched
                                // entries; every mismatched consume is
                                // zero-suppressed and counted stale.
                            end else begin
                                // Host halt entry: schedule one prime so
                                // the next note-on starts sample-exact.
                                prime_pending_r[host_addr_r[4:0]] <= 1'b1;
                            end
                        end
                        3'b110: begin                               // $C0-DF
                            ram_rts_osc_r <= host_addr_r[4:0];
                            ram_rts_din_r <= host_data_r;
                            ram_rts_we_r <= 1'b1;
                            // RTS changes the prime address mapping too.
                            prime_pending_r[host_addr_r[4:0]] <= 1'b1;
                        end
                    endcase
                end
            end else begin
                if (BANKS_IN_BSRAM) begin
                    // Host read access: latch the port-B read address (it
                    // persists until the next host read) and arm the
                    // response one cycle later than the baseline so the
                    // synchronous port-B data is valid when
                    // device_response consumes it — at X+2 or ANY later
                    // invocation (no staleness window, and the FSM's
                    // port-A addresses are never touched).
                    host_raddr_r <= host_addr_r[4:0];
                    host_read_wait_r <= 1'b1;
                end else begin
                    // FF banks: baseline-original shared-port read with
                    // 1-cycle response.
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
                            data_o <= ram_fl_hdout_w;
                        end
                        3'b001: begin                               // $20-3F
                            data_o <= ram_fh_hdout_w;
                        end
                        3'b010: begin                               // $40-5F
                            data_o <= ram_vol_hdout_w;
                        end
                        3'b011: begin                               // $60-7F
                            data_o <= ram_wds_hdout_w;
                        end
                        3'b100: begin                               // $80-9F
                            data_o <= ram_wtp_hdout_w;
                        end
                        3'b101: begin                               // $A0-BF
                            data_o <= ram_control_hdout_w;
                        end
                        3'b110: begin                               // $C0-DF
                            data_o <= ram_rts_hdout_w;
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

        if (cycle_start_r) begin
            // Do NOT process host requests on the boundary cycle: a host
            // WRITE processed here would leave a bank's port-A address on
            // the host target during OSC_START, and the synchronous read
            // sampled at the START->LOAD_REGISTERS edge would load the
            // WRONG oscillator's registers for this slot. The pending
            // flag holds the request for the next idle window (same-slot
            // application semantics are preserved — the service chain
            // always runs before the idle tail).
            osc_state_r <= OSC_START;
        end else begin
            host_access();
        end

    endtask : osc_idle

    task automatic osc_start();
        device_response();

        // Init other working values
        curr_output_r <= '0;
        halt_zero_r <= 1'b0;
        halt_overflow_r <= 1'b0;

        // Synchronous-read banks (BSRAM mode): each control-bank read
        // address is launched ONE STATE EARLIER than in the baseline so
        // the registered-read data lands in the same state the baseline
        // consumed it. The RAM samples pre-edge register values, so the
        // curr_osc default (asserted since slot start) still yields the
        // CURRENT oscillator's registers during OSC_LOAD_REGISTERS.
        // FF mode (BANKS_IN_BSRAM=0): baseline-original launch points.
        if (BANKS_IN_BSRAM) begin
            ram_control_osc_r <= partner_osc_w;
        end

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

        // BSRAM mode: launch next-control read; FF mode: baseline partner
        ram_control_osc_r <= BANKS_IN_BSRAM ? (curr_osc_r + 1'b1) : partner_osc_w;

        osc_state_r <= OSC_LOAD_PARTNER_CONTROL;
    endtask: osc_load_registers

    task automatic osc_load_partner_control();
        partner_control_r <= ram_control_dout_w;
        // BSRAM mode: launch prev-control read; FF mode: baseline next
        ram_control_osc_r <= BANKS_IN_BSRAM ? (curr_osc_r - 1'b1) : (curr_osc_r + 1'b1);
        osc_state_r <= OSC_LOAD_NEXT_CONTROL;
    endtask: osc_load_partner_control

    task automatic osc_load_next_control();
        next_control_r <= ram_control_dout_w;
        // BSRAM mode: prev-control read already launched in
        // OSC_LOAD_PARTNER_CONTROL (data lands during OSC_CONSUME);
        // FF mode: baseline launches it here.
        if (!BANKS_IN_BSRAM) begin
            ram_control_osc_r <= curr_osc_r - 1'b1;
        end
        osc_state_r <= OSC_CONSUME;
    endtask: osc_load_next_control

    task automatic osc_consume();
        // Consume the addressed byte lane of this oscillator's cached
        // word, applying the consume rules from the header. Replaces the
        // baseline's in-slot fetch (OSC_REQUEST_DATA / OSC_HANDLE_DATA).

        // cram_rdata_w holds the {tag, line} payload for entry
        // {curr_osc_r, expected_line_w[0]} — the synchronous RAM read was
        // addressed via cram_raddr_w and sampled on the way into this
        // state (see the cache-RAM wiring comment).
        automatic logic [15:0] expected_addr_w = wave_addr_f(curr_acc_r);
        automatic logic [11:0] expected_line_w = expected_addr_w[15:4];
        automatic logic [3:0]  lane_w = expected_addr_w[3:0];
        automatic logic [5:0]  entry_idx_w = {curr_osc_r, expected_line_w[0]};
        automatic logic [11:0] entry_tag_w = cram_rdata_r[139:128];
        automatic logic entry_valid_w = cache_valid_r[entry_idx_w];
        automatic logic entry_run_w = cache_src_run_r[entry_idx_w];
        automatic logic entry_match_w = (entry_tag_w == expected_line_w);
        automatic logic [7:0] entry_data_w = cram_rdata_r[8*lane_w +: 8];

        if (!halt_w) begin
            // Default resume address for paths that skip OSC_ACC (zero-byte
            // consumption followed by a partner-swap retrigger).
            issue_addr_r <= expected_addr_w;

            // Targeted GLU-write invalidation: this oscillator's two
            // cached/issued line addresses are already being read from the
            // lu RAMs at curr_osc_r — compare against the dirty ranges and
            // clear only hit slots (the refetch issues at THIS slot's
            // OSC_ACC, so fresh data is consumed within <= 2 periods).
            if (dirty_hit_f(lu_rdata0_r))
                issued_valid_r[{curr_osc_r, 1'b0}] <= 1'b0;
            if (dirty_hit_f(lu_rdata1_r))
                issued_valid_r[{curr_osc_r, 1'b1}] <= 1'b0;

            if (entry_valid_w && (entry_match_w || entry_run_w)) begin
                consume_addr_r <= {entry_tag_w, lane_w};
                ram_wds_we_r <= 1'b1;
                ram_wds_din_r <= entry_data_w;
                curr_wds_r <= entry_data_w;
                if (entry_run_w && !entry_match_w) begin
                    // Stale consume (late fetch, or external ACC/WTP/RTS
                    // rewrite): keep continuity with a nearby old byte but
                    // never evaluate halt-on-zero on an unvisited lane.
                    dbg_stale_fetch_r <= dbg_stale_fetch_r + 4'd1;
                    osc_state_r <= OSC_OUT;
                end else if (entry_data_w == 8'h00) begin
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
                dbg_prime_miss_r <= dbg_prime_miss_r + 4'd1;
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
            // (Rev 3.7: the former RUN-source downgrade on halted slots is
            // REMOVED — a restart before the prime retires now consumes
            // the stale line, zero-suppressed on tag mismatch, instead of
            // emitting 0x80. Swap-on-arrival: cached data stays
            // consumable until its replacement actually lands.)
            // Prime-once: if a priming event is pending (halt entry,
            //    WTP/RTS write, cache flush, reset) and the post-flush
            //    cooldown has expired, fetch the word at mem[wtp | 0] —
            //    the first word every un-halt path will need (all un-halt
            //    paths zero the accumulator). ONE fetch per event; a
            //    parked oscillator generates no steady-state traffic.
            if (prime_pending_r[curr_osc_r] && (flush_cooldown_r == 0)
                && !fq_full_w) begin
                automatic logic [15:0] prime_addr_w = wave_addr_f(24'd0);
                fetch_enqueue(curr_osc_r, prime_addr_w, 1'b0);
                prime_pending_r[curr_osc_r] <= 1'b0;
                lu_we0_r <= ~prime_addr_w[4];
                lu_we1_r <= prime_addr_w[4];
                lu_waddr_r <= curr_osc_r;
                lu_wdata_r <= prime_addr_w[15:4];
                issued_valid_r[{curr_osc_r, prime_addr_w[4]}] <= 1'b1;
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
        // NEXT sample from the post-update accumulator and issue a fetch
        // only when it leaves the last-issued 32-bit word.

        automatic logic [24:0] temp_acc = curr_acc_r + {curr_fh_r, curr_fl_r};
        automatic int high_bit_w = 17 + curr_res_w;
        automatic logic overflow = temp_acc[high_bit_w];
        automatic logic [23:0] new_acc_w = temp_acc[23:0] & curr_acc_mask_w;
        automatic logic [15:0] new_addr_w = wave_addr_f(new_acc_w);
        // Lookahead: the walk address two further samples ahead. Masking
        // once after both adds is exact here (FC <= 0xFFFF, acc mask >=
        // 0x1FFFF, so the sum cannot cross the wrap boundary twice).
        automatic logic [23:0] la_acc_w =
            24'(new_acc_w + {curr_fh_r, curr_fl_r} + {curr_fh_r, curr_fl_r})
            & curr_acc_mask_w;
        automatic logic [15:0] la_addr_w = wave_addr_f(la_acc_w);
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
            automatic logic [5:0] w1_idx_w = {curr_osc_r, new_addr_w[4]};
            automatic logic w1_need_w = !issued_valid_r[w1_idx_w]
                || (lu_sel_f(new_addr_w) != new_addr_w[15:4]);
            if (w1_need_w) begin
                // Crossing (or jump/flush refresh) NOW: needed by the next
                // service slot — the 1-service-period path.
                issue_running(curr_osc_r, new_addr_w);
            end else if (FETCH_LOOKAHEAD
                         && (la_addr_w[15:4] == 12'(new_addr_w[15:4] + 12'd1))) begin
                // Next-line prefetch, two samples ahead of the crossing.
                // The +1-line guard keeps the target in the OPPOSITE cache
                // slot (never evicts the line being consumed); larger skips
                // fall back to the crossing path above. issue_running's
                // per-slot check makes this idempotent.
                issue_running(curr_osc_r, la_addr_w);
            end
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
        // free-run/sync overflow case where the oscillator keeps running,
        // and prime scheduling when the oscillator halts.

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
                // running from the wrapped accumulator — issue its fetch
                // (word-gated; a wrap almost always crosses words).
                issue_running(curr_osc_r, issue_addr_r);
            end
        end

        // Halt entry: schedule one prime so the next start (swap-back or
        // host note-on) finds mem[wtp | 0] cached.
        if (will_halt_w && !goto_retrigger_w) begin
            prime_pending_r[curr_osc_r] <= 1'b1;
        end
    endtask: osc_halt_one_shot_or_zero_byte

    task automatic osc_start_partner();
        // Identical to baseline. The partner's first word (mem[wtp | 0])
        // was primed into its cache when it halted; no fetch is issued
        // here.

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

        issue_running(curr_osc_r, issue_addr_r);

        osc_state_r <= OSC_IDLE;
    endtask: osc_retrigger

endmodule

// Line-cache payload storage for doc5503_pipelined: 64 x {tag[11:0],
// line[127:0]}, simple-dual-port, synchronous read. Uses the repo's proven
// Gowin-safe inference idiom (see hdl/support/sdpram32.sv /
// boards/a2mega/hdl/sound/ensoniq_bsram.sv): separate write and read
// always blocks, full-word writes (no read-modify-write), no reset on the
// array — infers distributed SSRAM (or BSRAM) instead of ~9k flip-flops,
// which failed GW5AT-60 placement when this lived in a flat FF array.
module doc_cache_ram (
    input  wire         clk_i,
    input  wire         we_i,
    input  wire [5:0]   waddr_i,
    input  wire [139:0] wdata_i,
    input  wire [5:0]   raddr_i,
    output wire [139:0] rdata_o
);
    reg [139:0] mem [0:63] /*synthesis syn_ramstyle="block_ram"*/;

    always @(posedge clk_i) begin
        if (we_i)
            mem[waddr_i] <= wdata_i;
    end

    reg [139:0] rdata_r;
    always @(posedge clk_i)
        rdata_r <= mem[raddr_i];

    assign rdata_o = rdata_r;

endmodule

// Last-issued-line bookkeeping storage for doc5503_pipelined: 32 x 12,
// simple-dual-port, synchronous read (one instance per cache-slot parity).
// Same Gowin-safe inference idiom as doc_cache_ram.
module doc_lu_ram (
    input  wire        clk_i,
    input  wire        we_i,
    input  wire [4:0]  waddr_i,
    input  wire [11:0] wdata_i,
    input  wire [4:0]  raddr_i,
    output wire [11:0] rdata_o
);
    reg [11:0] mem [0:31] /*synthesis syn_ramstyle="block_ram"*/;

    always @(posedge clk_i) begin
        if (we_i)
            mem[waddr_i] <= wdata_i;
    end

    reg [11:0] rdata_r;
    always @(posedge clk_i)
        rdata_r <= mem[raddr_i];

    assign rdata_o = rdata_r;

endmodule

// Oscillator register bank for doc5503_pipelined: 32 x DATA_WIDTH,
// dual-read-port with SYNCHRONOUS reads, inferred as BSRAM (block_ram) —
// the GW5A family has no SSRAM/distributed RAM (GowinSynthesis RP0007),
// so the baseline osc_reg_ram's async-read LUT-RAM idiom cannot map there.
// Port A: FSM read/write on the shared per-bank address register (write
// and read never need the same-edge new data — verified against every
// consumer). Port B: host register reads (read-only, persistent address).
// Same Gowin-safe pattern as doc_cache_ram: separate always blocks, no
// read-modify-write, no array reset.
module osc_reg_ram_dp #(
    parameter int DATA_WIDTH = 8
) (
    input  wire                  clk_i,
    input  wire [4:0]            a_addr_i,
    input  wire                  a_we_i,
    input  wire [DATA_WIDTH-1:0] a_din_i,
    output wire [DATA_WIDTH-1:0] a_dout_o,
    input  wire [4:0]            b_addr_i,
    output wire [DATA_WIDTH-1:0] b_dout_o
);
    reg [DATA_WIDTH-1:0] mem [0:31] /*synthesis syn_ramstyle="block_ram"*/;

    always @(posedge clk_i) begin
        if (a_we_i)
            mem[a_addr_i] <= a_din_i;
    end

    // Port-A read is DISABLED during a port-A write (the read-during-write
    // value is proven don't-care for every consumer): the output holds its
    // previous value on write cycles, which is exactly BSRAM WRITE_MODE
    // 2'b00 — the unguarded same-address read+write inferred an SP port
    // with WRITE_MODE 2'b10 (read-before-write), which GW5A rejects at
    // placement (PA2122). Freeze-edge audit: no bank's a_we_i is ever high
    // during a state whose exit edge samples a JUST-CHANGED read address
    // (chain states set no bank we; host processing is barred from the
    // cycle_start boundary cycle in osc_idle).
    reg [DATA_WIDTH-1:0] a_dout_r;
    always @(posedge clk_i)
        if (!a_we_i)
            a_dout_r <= mem[a_addr_i];
    assign a_dout_o = a_dout_r;

    reg [DATA_WIDTH-1:0] b_dout_r;
    always @(posedge clk_i)
        b_dout_r <= mem[b_addr_i];
    assign b_dout_o = b_dout_r;

endmodule
