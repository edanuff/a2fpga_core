// A2MEGA - IIgs CPU-socket PHY for the in-socket 65C816 (drop-in iteration)
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
// Description
// -----------
// Sits between the P65C816 core (the MiSTer IIgs core, unchanged) and the
// FPGA_GS_* pins that reach the IIgs CPU socket through the 1.0a3 level
// shifters (docs/gs_socket_65816_scoping.md: §2 circuit, §4 timing).
//
// The socket never runs faster than the FPI's PHI2 (2.86 MHz fast cycles,
// stretched-high sync cycles, 1 MHz slow mode).  The design makes the
// FPGA behave like a 65C816 at the socket pins:
//
//  * The core is CLOCKED BY PHI2, like the real part.  cpu_clk_o is the
//    inverted PH2 pin, so the core's active edge is the PHI2 falling edge:
//    it samples D0-7, RDY, /IRQ, /NMI and /ABORT straight from the pads at
//    the instant a 65816 would (tDSR/tDHR are measured to that edge), and
//    its state - hence A0-23, RWB, VPB, D_OUT - is settled ~20 ns later.
//    The core takes one clock per bus cycle (CE tied high; its microcode
//    ROM is read from the NEXT state, so no extra clocks are needed) and
//    RDY_IN comes straight from the pad, so a low RDY at the fall freezes
//    it exactly as the datasheet describes.  On 1.0a3 the PH2 ball is not
//    a dedicated clock input; the clock reaches the network through
//    general routing, which at 2.86 MHz is not a concern.  Every core path
//    has a 349 ns period - no multicycle constraints, nothing to close.
//  * The pins are sequenced in the clk (~108 MHz) domain.  A toggle
//    written at the fall is synchronised across (two flops); OUT_DELAY
//    clocks after that the core's outputs are registered out: A0-15, RWB,
//    VPB, and the bank byte on D0-7.  Fall -> address on the socket is
//    ~45-55 ns including both transceivers (4 MHz-grade tADS/tBAS is
//    75 ns; the FPI's low phase is ~140 ns) and never earlier than ~40 ns
//    (tAH).  The core's outputs are quasi-static for a whole cycle, so
//    this crossing is safe by construction as long as they have settled
//    before the sample: OUT_DELAY = 2 gives >= 37 ns after the pad fall
//    against a ~20 ns settle - a designed margin, stated in the SDC.
//  * At the rise (two-flop sync, ~10-20 ns) the bank byte is held
//    RISE_HOLD more clocks (tBH >= 20 ns), then a write cycle switches the
//    byte to write data (tMDS <= 70 ns) and a read cycle releases the bus:
//    shifter off, pads tri-state, direction turned, receive on - done
//    ~40 ns after the rise, long before any memory can drive.  Write data
//    holds until the next cycle's bank byte (~45 ns after the fall, tDHW).
//  * RDY low at the fall: the core does not advance; per W65C816S §7.6 the
//    data bus stays in the data-transfer state (write data kept driven, or
//    bus kept in receive) and no bank byte is driven in the repeated
//    cycle's low phase.
//  * BE (asynchronous, synchronised here).  MEASURED on a ROM 01 IIgs
//    (2026-09-06, AD3 at the socket): the FPI pulses BE low for ~66 ns
//    EVERY cycle, from ~32 ns before the PHI2 fall to ~34 ns after it -
//    a bus-turnaround pulse around the sample edge, not a DMA-only level.
//    A 4 MHz-grade 65816 (tBVD 60 ns) drives straight through it and the
//    FPI is built for that, so the PHY ignores BE lows shorter than
//    BE_MIN_CLKS (~110 ns).  A sustained BE low tri-states only what a
//    65816 tri-states: the address/RWB shifters and our data DRIVE; the
//    receive path is never gated (the core samples at the fall).  The
//    core keeps running, as a real 65816 does.
//  * /RES: sampled at the fall; the core is held in reset until /RES has
//    been high at two consecutive falls, and is released synchronously to
//    its own clock.  While in reset the address bus stays driven with the
//    core's reset-state address, RWB is forced high and no data phase is
//    driven.
//  * RDY_OUT is never asserted (WAI stays internal); VP is driven.
//
// Nothing is driven into the socket unless armed_i is high AND PH2 has
// been seen toggling (ph2_alive_o).  Unarmed, every enable is inactive -
// the card behaves exactly as it does today.
//
// PH2_SAMPLED = 1 (the accelerator, whose core runs on clk): no flop is
// clocked by the pin.  PH2 is a signal sampled in every clk, and the pads
// that were taken at its fall (RDY and /RES here; D0-7, RDY and the
// interrupt lines in gs_accel_broker) are sampled in every clk as well.
//  * A fall is a low sample after FALL_HIGH and more high ones in a row; a
//    rise is two high samples in a row after a low one, and the bank byte
//    is then held one clk less, so the pins move in the clks they did.  A
//    pulse narrower than a clk is one sample at most and is neither.  The
//    fall event is in the clk it was: the second clk edge after the first
//    low sample.
//  * The pads "at the fall" are the samples of the last clk edge at which
//    the pin still read high - 0 to 1 clk before the fall, where the pads
//    have been measured to hold the byte from 9 clks before it to 5 after
//    (2026-11-24, ROM, fast RAM and the Mega II's side alike).  They are
//    read where the pin-clocked flops were: in the clk of the fall event
//    and, by the broker, in the clk before it.
// Why: the pin was seen to take a falling edge that is no cycle's end - a
// pulse narrower than a clk in the low phase, in the clk after the card
// has put a cycle on the bus (2026-11-24, with the card's USB fed: 472 on
// one start, 1 on two others, each of them after a store of zero to
// E1:00FF with the fetch at FF:BFxx next, some twenty pads rising at once;
// none on the slot's supply).  Every flop the pin clocked was clocked by
// it: the broker ended the cycle in flight and took the bank byte for its
// read.  The flop that toggles at the pin's falls is kept for one thing,
// the count of them that are no fall (xfall_evt_o, an instrument).
//
module gs_socket_phy #(
    parameter int OUT_DELAY  = 2,   // clks after the synchronised fall before core outputs are sampled
    parameter int RISE_HOLD  = 1,   // extra clks the bank byte is held after the synchronised rise
    parameter int BE_MIN_CLKS = 12, // BE low shorter than this (~110 ns) is the FPI's per-cycle turnaround pulse: ignored
    parameter int ALIVE_BITS = 17,  // no PH2 fall for 2^ALIVE_BITS clks => not alive
    parameter bit CORE_SUBST = 1'b1,// 1 = substitute internal cycles aimed at I/O space here (S6: the core's
                                    // outputs come straight in); 0 = the caller already did (gs_accel_broker)
    parameter bit WDATA_LATE = 1'b0,// 1 = take the write data again the clk after the issue (it is not driven
                                    // before the PHI2 rise, so the caller gets one more clk for it)
    parameter bit PH2_SAMPLED = 1'b0,// 1 = no flop clocked by the PH2 pin (see above); cpu_clk_o / cpu_rst_n_o
                                    // are then not for a core clocked by the pin
    parameter int FALL_HIGH  = 8    // PH2_SAMPLED: high samples in a row before a low one is a fall (PHI2 is
                                    // high for 200 ns and more; the pads move in the first four clks of it)
) (
    input  logic        clk,            // pin-sequencer clock (~108 MHz)
    input  logic        rst_n,          // system reset, active low
    input  logic        armed_i,        // 1 = take the socket (asynchronous ok)
    input  logic        listen_i,       // 1 = enable the control-input shifter only (C3 listen-only: PH2/RDY/RES/IRQ/NMI/ABORT/BE visible, nothing driven)
    input  logic [3:0]  out_extra_i,    // extra clks before issue (address-delay sweep instrument)

    // P65C816 side.  D_IN / RDY_IN / IRQ_N / NMI_N / ABORT_N connect straight
    // from the pads to the core in the top level.
    output logic        cpu_clk_o,      // = ~gs_ph2_i (core samples at the PHI2 fall)
    output logic        cpu_rst_n_o,    // core reset (async assert, released on cpu_clk)
    input  logic [23:0] cpu_a_i,        // core A_OUT
    input  logic [7:0]  cpu_d_out_i,    // core D_OUT
    input  logic        cpu_we_n_i,     // core WE: 1 = read, 0 = write (RWB polarity)
    input  logic        cpu_vpa_i,      // core VPA/VDA: both low = internal cycle. The socket has no
    input  logic        cpu_vda_i,      // VDA/VPA pins, so the FPI performs whatever address is on the bus;
                                        // the real chip holds the operand address through an R-M-W internal
                                        // cycle, the MiSTer core presents operand+1 (TSB $C027 -> $C028 =
                                        // ROMBANK, bench G19). Internal cycles repeat the previous address as a read.
    input  logic        force_slow_i,   // DIAGNOSTIC: clear bit 7 on writes to $C036 (any I/O bank)
                                        // so the FPI never leaves 1 MHz - isolates fast-mode bus timing
    input  logic        cpu_vp_n_i,     // core VPB (active low)

    // Socket side: FPGA_GS_* pins
    input  logic        gs_ph2_i,
    input  logic        gs_rdy_i,
    input  logic        gs_res_n_i,
    input  logic        gs_be_i,
    output logic [7:0]  gs_d_o,         // FPGA D pad outputs ...
    output logic        gs_d_oe_o,      // ... driven when 1 (top does the inout)
    output logic [15:0] gs_a_o,
    output logic        gs_rw_o,        // 1 = read
    output logic        gs_vp_o,        // 1 = VP inactive (open-drain released)
    output logic        gs_rdy_out_o,   // 1 = RDY released (never asserted here)
    output logic        gs_d_dir_o,     // U13 port 2 DIR: 1 = FPGA -> bus
    output logic        gs_data_oe_n_o, // U13 port 2 ~OE
    output logic        gs_addr_oe_n_o, // U14 ~OE (A0-15) and U16 ~OE (RWB)
    output logic        gs_ctl_oe_n_o,  // U13 port 1 ~OE (control inputs)

    // Status / telemetry (clk domain)
    output logic        ph2_alive_o,
    output logic        running_o,      // core out of reset
    output logic        enabled_o,      // armed & alive: pins are ours
    output logic        be_ok_o,        // synchronised BE
    output logic        fall_evt_o,     // one clk per PHI2 fall (synchronised)
    output logic        issue_evt_o,    // one clk when a cycle is put on the bus
    output logic        ended_read_o,   // valid with fall_evt_o: the cycle that ended was a running read
    output logic        xfall_evt_o,    // a falling edge of the pin with the pins ours that is no cycle's end:
                                        // the sequencer not waiting for one (an instrument; see gs_accel)
    output logic [2:0]  state_o,        // the sequencer's state (as xfall_evt_o: for an instrument)
    output logic [31:0] cycle_count_o,  // bus cycles the core advanced through
    output logic [15:0] stall_count_o,  // cycles repeated because RDY was low
    output logic [15:0] be_count_o      // clk cycles spent with BE low
);

    //=========================================================================
    // clk domain: synchronisers, edge events, liveness, enable
    //=========================================================================
    logic [2:0] tog_s;
    logic [3:0] ph2_s;
    logic [1:0] be_s;
    logic [1:0] armed_s;
    logic [1:0] listen_s;
    logic       cap_tog = 1'b0;         // PH2 domain, toggles at every fall

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tog_s   <= '0;
            ph2_s   <= '0;
            be_s    <= 2'b11;
            armed_s <= 2'b00;
            listen_s <= 2'b00;
        end else begin
            tog_s   <= {tog_s[1:0], cap_tog};
            ph2_s   <= {ph2_s[2:0], gs_ph2_i};
            be_s    <= {be_s[0], gs_be_i};
            armed_s <= {armed_s[0], armed_i};
            listen_s <= {listen_s[0], listen_i};
        end
    end

    // PH2_SAMPLED: the run of high samples behind the newest one (ph2_s[1])
    logic [3:0] hi_cnt;
    logic       hi_ok;                         // FALL_HIGH and more of them
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n)          begin hi_cnt <= '0; hi_ok <= 1'b0; end
        else if (!ph2_s[1])  begin hi_cnt <= '0; hi_ok <= 1'b0; end
        else begin
            if (hi_cnt != 4'd15) hi_cnt <= hi_cnt + 4'd1;
            hi_ok <= (hi_cnt >= 4'(FALL_HIGH - 1));
        end
    end
    initial if (PH2_SAMPLED && (FALL_HIGH < 2 || FALL_HIGH > 15 || RISE_HOLD < 1))
        $fatal(1, "gs_socket_phy: PH2_SAMPLED needs 2 <= FALL_HIGH <= 15 and RISE_HOLD >= 1");

    wire tog_evt  = tog_s[2] ^ tog_s[1];       // the pin took a falling edge
    wire fall_evt = PH2_SAMPLED ? (ph2_s[2] & ~ph2_s[1] & hi_ok)      // a PH2 falling edge happened
                                : tog_evt;
    wire rise_evt = PH2_SAMPLED ? (ph2_s[1] & ph2_s[2] & ~ph2_s[3])   // PH2 rising edge
                                : (ph2_s[1] & ~ph2_s[2]);
    localparam int RISE_WAIT = PH2_SAMPLED ? RISE_HOLD - 1 : RISE_HOLD;
    wire armed    = armed_s[1];

    // BE filter.  Measured on a ROM 01 IIgs: the FPI pulses BE low ~66 ns
    // around EVERY PHI2 fall (from ~32 ns before to ~34 ns after).  A
    // 4 MHz-grade 65816 (tBVD 60 ns) keeps driving through that pulse and
    // the FPI is built for it, so we do the same: a BE low shorter than
    // BE_MIN_CLKS is ignored; a sustained BE low (a real bus request, e.g.
    // DMA) tri-states our drivers ~BE_MIN_CLKS + 2 clks after it starts.
    logic [4:0] be_low_cnt;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n)          be_low_cnt <= '0;
        else if (be_s[1])    be_low_cnt <= '0;
        else if (be_low_cnt != 5'd31) be_low_cnt <= be_low_cnt + 1'b1;
    end
    wire be_ok = be_s[1] | (be_low_cnt < 5'(BE_MIN_CLKS));

    logic [ALIVE_BITS-1:0] alive_cnt;
    logic [1:0]            falls_seen;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            alive_cnt   <= '0;
            ph2_alive_o <= 1'b0;
            falls_seen  <= 2'd0;
        end else if (fall_evt) begin
            alive_cnt   <= '0;
            ph2_alive_o <= 1'b1;
            if (falls_seen != 2'd3) falls_seen <= falls_seen + 2'd1;
        end else if (alive_cnt == {ALIVE_BITS{1'b1}}) begin
            ph2_alive_o <= 1'b0;
            falls_seen  <= 2'd0;
        end else begin
            alive_cnt <= alive_cnt + 1'b1;
        end
    end

    // enabled = armed & ph2_alive_o & (falls_seen >= 2), as a register: made from
    // the next values of the three, so it is that function in every clk and what
    // reads it (the pin enables, the broker's sequencer, the PH2 domain) reads a
    // register.
    logic enabled;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) enabled <= 1'b0;
        else        enabled <= armed_s[0] & (fall_evt ? (falls_seen != 2'd0)
                                                      : ((alive_cnt != {ALIVE_BITS{1'b1}}) & ph2_alive_o & (falls_seen >= 2'd2)));
    end
`ifdef VERILATOR
    always @(posedge clk) if (rst_n && enabled !== (armed & ph2_alive_o & (falls_seen >= 2'd2))) begin
        $display("ERROR: gs_socket_phy: the enable register is not armed & alive & two falls seen"); $finish;
    end
`endif

    //=========================================================================
    // PH2 domain (the core's clock domain): sample points at the fall
    //=========================================================================
    logic       rdy_q   = 1'b1;    // RDY at the last fall
    logic       res_q   = 1'b0;    // /RES at the last fall
    logic       res_ok  = 1'b0;    // /RES high at the last two falls
    logic [1:0] en_s    = 2'b00;   // 'enabled' brought into this domain

    logic       stall_last;        // the cycle that just ended did not advance the core
    logic       stall_evt;         // the same, as read in the clk of the fall event

    if (!PH2_SAMPLED) begin : g_pin
        always_ff @(negedge gs_ph2_i) begin
            cap_tog <= ~cap_tog;
            rdy_q   <= gs_rdy_i;
            res_q   <= gs_res_n_i;
            res_ok  <= res_q & gs_res_n_i;
            en_s    <= {en_s[0], enabled};
        end
        assign cpu_rst_n_o = rst_n & en_s[1] & res_ok;
        // Views for the clk domain (quasi-static; read >= 2 clks after the fall)
        assign stall_last  = ~rdy_q;
        assign stall_evt   = ~rdy_q;
    end else begin : g_smp
        // the pin's falls, for the count of those that are no fall and nothing else
        always_ff @(negedge gs_ph2_i) cap_tog <= ~cap_tog;
        // The same four, in clk: loaded at the fall event with the pads of the last
        // clk edge at which the pin read high ([2] in that clk: [0] is the edge that
        // made the event, [1] the first low sample's), and read through in that clk
        // as the pin-clocked ones were, which had them from the fall itself.
        // (run_nx: what "the core runs" becomes at the next fall event, made a clk ahead
        // from [1] - nothing else in it moves between falls - so that the flag is one of
        // two registers by the event and no deeper than it was)
        logic [2:0] rdy_p, res_p;
        logic       run_nx, run_q;
        always_ff @(posedge clk or negedge rst_n) begin
            if (!rst_n) begin
                rdy_p <= 3'b111; res_p <= 3'b000;
                rdy_q <= 1'b1; res_q <= 1'b0; res_ok <= 1'b0; en_s <= 2'b00;
                run_nx <= 1'b0; run_q <= 1'b0;
            end else begin
                rdy_p  <= {rdy_p[1:0], gs_rdy_i};
                res_p  <= {res_p[1:0], gs_res_n_i};
                run_nx <= en_s[0] & res_q & res_p[1];
                if (fall_evt) begin
                    rdy_q  <= rdy_p[2];
                    res_q  <= res_p[2];
                    res_ok <= res_q & res_p[2];
                    en_s   <= {en_s[0], enabled};
                    run_q  <= run_nx;
                end
            end
        end
        assign cpu_rst_n_o = fall_evt ? run_nx : run_q;
        // (two names, so that nothing has the event in front of its RDY that need not)
        assign stall_last  = ~rdy_q;
        assign stall_evt   = ~rdy_p[2];
`ifdef VERILATOR
        always @(posedge clk) if (rst_n && (run_q !== (en_s[1] & res_ok) ||
                                            (fall_evt && run_nx !== (en_s[0] & res_q & res_p[2])))) begin
            $display("ERROR: gs_socket_phy: the run flag's registers are not /RES high at two falls with the pins ours"); $finish;
        end
`endif
    end

    assign cpu_clk_o   = ~gs_ph2_i;
    wire core_run   = cpu_rst_n_o;

    //=========================================================================
    // Pin sequencer (clk domain)
    //=========================================================================
    typedef enum logic [2:0] {
        S_OFF,        // not enabled: nothing driven
        S_WAIT_FALL,  // cycle on the bus, waiting for the falling edge
        S_OUT_WAIT,   // fall seen: waiting for the core's outputs to settle
        S_ISSUE,      // register the cycle out (address, RWB, VP, bank byte)
        S_LOW,        // low phase, waiting for the rise
        S_RISEH,      // rise seen: hold the bank byte (tBH)
        S_REL1,       // read: bus released, turn the shifter around
        S_REL2        // read: enable receive
    } state_t;

    state_t     state;
    logic [3:0] wait_cnt;
    logic       write_q;         // cycle on the bus is a write
    logic [7:0] bank_q;          // bank byte of the last non-internal cycle
    // internal cycle whose address falls in I/O space ($C000-$C0FF of banks 00/01/E0/E1): the only
    // case where a phantom address has side effects; elsewhere the core's own address is presented
    wire        internal_cyc = CORE_SUBST & core_run & ~cpu_vpa_i & ~cpu_vda_i & (cpu_a_i[15:8] == 8'hC0) &
                               ((cpu_a_i[23:17] == 7'd0) | (cpu_a_i[23:17] == 7'h70));
    logic [7:0] wdata_q;
    logic       wd_again = 1'b0; // WDATA_LATE: the clk after a normal issue
    logic       d_oe_seq;        // sequencer wants the D pads driven
    logic       data_oe_n_seq;   // sequencer's U13 port-2 enable
    logic       run_q;           // core was running when the cycle was issued

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state         <= S_OFF;
            wait_cnt      <= '0;
            write_q       <= 1'b0;
            bank_q        <= 8'h00;
            wdata_q       <= 8'h00;
            run_q         <= 1'b0;
            d_oe_seq      <= 1'b0;
            data_oe_n_seq <= 1'b1;
            gs_d_dir_o    <= 1'b0;
            gs_a_o        <= 16'h0000;
            gs_rw_o       <= 1'b1;
            gs_vp_o       <= 1'b1;
            gs_d_o        <= 8'h00;
            cycle_count_o <= '0;
            stall_count_o <= '0;
        end else if (!enabled) begin
            state         <= S_OFF;
            write_q       <= 1'b0;
            run_q         <= 1'b0;
            d_oe_seq      <= 1'b0;
            data_oe_n_seq <= 1'b1;
            gs_d_dir_o    <= 1'b0;
            gs_rw_o       <= 1'b1;
            gs_vp_o       <= 1'b1;
        end else begin
            unique case (state)
                S_OFF, S_WAIT_FALL: begin
                    if (fall_evt) begin
                        // Book-keeping for the cycle that just ended
                        if (state == S_WAIT_FALL && run_q) begin
                            if (stall_evt)  stall_count_o <= stall_count_o + 1'b1;
                            else            cycle_count_o <= cycle_count_o + 1'b1;
                        end
                        // A completed read leaves the shifter in receive;
                        // take it off the bus ahead of the bank byte.
                        if (!write_q && !(stall_evt && run_q)) data_oe_n_seq <= 1'b1;
                        wait_cnt <= 4'(OUT_DELAY - 1) + out_extra_i;
                        state    <= S_OUT_WAIT;
                    end
                end

                S_OUT_WAIT: begin
                    if (wait_cnt == 4'd0) state <= S_ISSUE;
                    else                  wait_cnt <= wait_cnt - 1'b1;
                end

                S_ISSUE: begin
                    // The core's outputs have settled: put the cycle on the bus.
                    if (internal_cyc) begin
                        // internal cycle: repeat the previous address/bank as a read
                        // (gs_a_o unchanged) - never a new address on the bus
                        gs_rw_o <= 1'b1;
                        gs_vp_o <= 1'b1;
                    end else begin
                        gs_a_o  <= cpu_a_i[15:0];
                        gs_rw_o <= core_run ? cpu_we_n_i : 1'b1;
                        gs_vp_o <= core_run ? cpu_vp_n_i : 1'b1;
                    end
                    if (!(stall_last && run_q)) begin
                        // normal cycle: new write flag/data, bank byte during the low phase
                        write_q       <= core_run & ~cpu_we_n_i & ~internal_cyc;
                        wdata_q       <= (force_slow_i && cpu_a_i[15:0] == 16'hC036 &&
                                          (cpu_a_i[23:17] == 7'd0 || cpu_a_i[23:17] == 7'h70))
                                         ? {1'b0, cpu_d_out_i[6:0]} : cpu_d_out_i;   // banks 00/01, E0/E1
                        if (internal_cyc) begin
                            gs_d_o    <= bank_q;
                        end else begin
                            gs_d_o    <= cpu_a_i[23:16];
                            bank_q    <= cpu_a_i[23:16];
                        end
                        d_oe_seq      <= 1'b1;
                        gs_d_dir_o    <= 1'b1;
                        data_oe_n_seq <= 1'b0;
                    end
                    // else repeated cycle: keep the data-transfer state as is
                    run_q <= core_run;
                    state <= S_LOW;
                end

                S_LOW: begin
                    if (rise_evt) begin
                        wait_cnt <= 4'(RISE_WAIT);
                        state    <= S_RISEH;
                    end
                end

                S_RISEH: begin
                    if (wait_cnt != 4'd0) wait_cnt <= wait_cnt - 1'b1;
                    else if (write_q) begin
                        gs_d_o <= wdata_q;                // bank -> write data
                        state  <= S_WAIT_FALL;
                    end else if (d_oe_seq) begin
                        data_oe_n_seq <= 1'b1;            // release the bus
                        d_oe_seq      <= 1'b0;
                        state         <= S_REL1;
                    end else begin
                        state <= S_WAIT_FALL;             // already in receive (repeat)
                    end
                end

                S_REL1: begin
                    gs_d_dir_o <= 1'b0;                   // turn around
                    state      <= S_REL2;
                end

                S_REL2: begin
                    data_oe_n_seq <= 1'b0;                // receive
                    state         <= S_WAIT_FALL;
                end

                default: state <= S_OFF;
            endcase
            // WDATA_LATE: the write data, taken again the clk after a normal issue
            wd_again <= WDATA_LATE & (state == S_ISSUE) & ~(stall_last & run_q);
            if (wd_again)
                wdata_q <= (force_slow_i && cpu_a_i[15:0] == 16'hC036 &&
                            (cpu_a_i[23:17] == 7'd0 || cpu_a_i[23:17] == 7'h70))
                           ? {1'b0, cpu_d_out_i[6:0]} : cpu_d_out_i;
        end
    end

    //=========================================================================
    // Enables and status
    //=========================================================================
    assign gs_ctl_oe_n_o  = ~(armed | listen_s[1]);   // inputs only; harmless to enable without arming
    assign gs_addr_oe_n_o = ~(enabled & be_ok);
    // BE low takes our data DRIVE off the bus (the pads, and the shifter only
    // while we are driving); the receive direction is never gated by BE - the
    // FPI holds BE low across the PHI2 fall, exactly where the core samples.
    assign gs_data_oe_n_o = data_oe_n_seq | ~enabled | (d_oe_seq & ~be_ok);
    assign gs_d_oe_o      = d_oe_seq & be_ok & enabled;
    assign gs_rdy_out_o   = 1'b1;
    assign running_o      = core_run;
    assign enabled_o      = enabled;
    assign be_ok_o        = be_ok;
    assign fall_evt_o     = fall_evt;
    assign issue_evt_o    = (state == S_ISSUE);
    assign ended_read_o   = ~write_q & run_q & ~stall_evt;
    // (PH2_SAMPLED: the pin's own falling edge reaches here through another flop than the
    // sample does and may be a clk to either side of the fall event; it is taken a clk
    // late and is a cycle's end if the event is in that clk or one of the two before it)
    logic       tog_q;
    logic [1:0] fall_q;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin tog_q <= 1'b0; fall_q <= 2'b00; end
        else        begin tog_q <= tog_evt; fall_q <= {fall_q[0], fall_evt}; end
    end
    assign xfall_evt_o    = PH2_SAMPLED ? (tog_q & enabled & ~(fall_evt | fall_q[0] | fall_q[1]))
                                        : (fall_evt & enabled & (state != S_WAIT_FALL) & (state != S_OFF));
    assign state_o        = state;

    logic [15:0] be_cnt;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) be_cnt <= '0;
        else if (!be_ok && be_cnt != 16'hFFFF) be_cnt <= be_cnt + 1'b1;
    end
    assign be_count_o = be_cnt;

endmodule
