// A2MEGA - S7 accelerator: bus-cycle broker between the 65C816 core and the
// IIgs CPU-socket PHY (S7b)
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
// S2-S6 clock the core with PHI2: one core step per motherboard cycle.  Here
// the core lives in the accelerator's own clock domain (clk, the PHY's pin-
// sequencer clock) and is stepped with a clock enable; this module decides,
// step by step, where the step's bus cycle is performed
// (docs/gs_socket_s7_accelerator_scoping.md 2.1-2.4, 9):
//
//  * LOCAL: the memory-map replica (gs_mmu_replica) says the access belongs
//    to the card - banks 00/01, the ROM image, backed expansion banks.  The
//    local memory port is strobed ADDR_CLKS clocks into the step and the
//    core steps STEP_CLKS clocks after the previous step.  The socket shows
//    an idle cycle (below).
//  * SOCKET: everything else - I/O, slot space, banks E0/E1, unbacked banks.
//    The core is held (no clock enable), the cycle is handed to the PHY at
//    its next issue point, runs as exactly one PHI2 cycle with the PHY's
//    timing, and the core steps on the PHI2 fall that completes it, taking
//    the byte the socket pads held at that fall.  RDY low at the fall
//    repeats the cycle, as the PHY and a real 65816 do.
//  * POSTED WRITE: a local RAM write the machine shadows into E0/E1
//    (replica: forward_write) is stored locally at once AND queued in a
//    one-deep buffer to be run on the socket as an ordinary bank-00/01
//    write, so the FPI does its shadow copy.  The core does not wait for
//    it; a second forwarded write waits until the buffer is free.  Order on
//    the socket is program order: the buffer always goes out before any
//    later socket cycle of the core (which is necessarily younger, because
//    the core stops at the first one it needs).  A read of a just-posted
//    location is a local read of the byte just stored.
//  * IDLE: a PHI2 cycle with nothing to carry is a read of PARK_ADDR (a ROM
//    address in bank FF: a fast cycle, no refresh interplay, no side effect
//    anywhere in the machine) with VDA = VPA = 0.  Departure from the
//    scoping doc's "previous address repeated": repeating a previous I/O or
//    slot address would repeat its side effects (9).
//
// Internal cycles of the core (VDA = VPA = 0) carry no data.  In fast mode
// they reach the socket only when their address could matter to the machine
// (I/O or slot space), exactly as S6 would have shown them: an internal
// cycle aimed at $C0xx of banks 00/01/E0/E1 repeats the previous step's
// address as a read (the S5 phantom-$C028 rule, gs_socket_65816_scoping 4.5;
// the broker does the substitution itself, because the PHY's "previous
// address" would be the park address), elsewhere the core's own address.
//
// Modes (runtime, taken at step boundaries):
//  * native_i = 1: every step is a socket cycle, one per PHI2 cycle, issued
//    and completed exactly as S6 does it - pin for pin (the bench compares
//    against an S6 PHY + PHI2-clocked core running beside it).
//      - mem_bypass_i = 1: read data comes from the socket for everything,
//        i.e. S6.  Writes are still copied into the card's memory, so the
//        card's copy of banks 00/01 stays current.
//      - mem_bypass_i = 0: the card's memory is still the memory of record
//        (reads of local regions use it; every write also goes out on the
//        socket), so native and fast can be switched in either direction at
//        any time.  Each local read is also compared with the byte the
//        socket returned (verify_*: the scoping doc's "verify mode").  The
//        comparison means something only until fast mode first runs: after
//        that the motherboard's copy of banks 00/01 is stale by design.
//    Clearing mem_bypass_i is only coherent while the card's memory is a
//    complete copy; setting it is only coherent while the motherboard's is,
//    i.e. as long as fast mode has not run since power-up.
//  * native_i = 0: fast mode, as above.  RDY low at the last PHI2 fall also
//    holds local steps (the TransWarp's GS_PAUSE).
//
// Reset.  The core's reset is the PHY's (the machine's /RES, held until it
// has been high at two PHI2 falls), re-registered here.  While it is low the
// replica's switches take their reset values, the core is in reset, and the
// socket shows what S6 shows: the core's reset-state address as a read.  A
// posted write that is still in the buffer when /RES falls is dropped (the
// PHY cannot write during reset): at most one byte of a shadowed page, which
// the card's own memory still holds.  The card's memory is kept (warm reset).
//
// Clocking / CDC.  One domain: clk.  The socket pads (D0-7, RDY, /IRQ, /NMI,
// /ABORT) are sampled by flops clocked on the PHI2 fall (the instant a 65816
// samples them) and read here only at or after phy_fall_evt_i, at least one
// clk later - the same quasi-static crossing gs_socket_phy uses for the
// core's outputs.  A native step uses exactly those samples.  In fast mode
// the three interrupt lines go through ordinary two-flop synchronisers
// instead, so the core never acts on a level that is a PHI2 cycle old.  The
// PHY reads this module's phy_* outputs only in its issue clock
// (phy_issue_evt_i).  In native mode the next request must be presented by
// that clock: ADDR_CLKS <= OUT_DELAY + out_extra of the PHY.
//
// The core is taken through its port list (CLK = clk, RST_N, CE, RDY_IN tied
// high, IRQ/NMI/ABORT, D_IN, and its bus outputs), so P65C816 and F65C816
// are interchangeable.
//
// Local memory and the data-late step (DATA_LATE = 1, F65C816 only).  The
// memories are synchronous: a read is LAUNCHED (lm_ren_o registers the RAM
// and the ROM-image address, both memories read) and its data is used
// STEP_CLKS later.  Without DATA_LATE every local read is launched at the
// step's strobe from the core's address (two-edge step: ADDR_CLKS to form the
// address, the rest of the step for the data).  With DATA_LATE the broker also
// launches a read at the clock-enable edge of every local step, at the core's
// NEXT-step address (cpu_na_i = F65C816.NA_OUT, translated as a read by the
// replica's pf_ port), so that the following step finds its data already
// there and the whole step is one STEP_CLKS-long path.  The launch is a
// speculation and it is checked, not assumed: the broker remembers what the
// memories' output registers hold (ld_*), and when the step runs it compares
// that with the address the step really needs on the memory it really uses.
// A match uses the data; anything else - the step after a socket cycle (no
// launch: the socket data arrives too late to form the next address), a soft
// switch that moved the map, a vector pull, a local write since the launch -
// launches the read again, one clk after the strobe and from registers
// (ml_*), and waits a full STEP_CLKS for it.  The select of the core's data
// source (card RAM, card ROM, socket) is speculated and checked the same way:
// it is set at the launch from the replica's view of the next address, and a
// step that needs a different source sets it one clk after the strobe and
// waits.  Correctness therefore never rests on the guess, only speed.
// Timing shape this gives a build: core registers reach the memories'
// read-address pins and the data-source select only through the next-address
// path, once per step (a STEP_CLKS-cycle path, like memory data -> core and
// core -> core); everything else the broker registers from the core's outputs
// is captured at the strobe (an ADDR_CLKS-cycle path), and the select changes
// at least STEP_CLKS before the core takes the data.
// A write goes to the RAM at the strobe, always at least one clock before the
// next launch, so a launch never coincides with a write.
//
module gs_accel_broker #(
    parameter int          STEP_CLKS = 4,            // fast mode: clks per local step (>= ADDR_CLKS + 1)
    parameter int          ADDR_CLKS = 1,            // clks from the step edge to the memory / request strobe
    parameter int          SLOW_CLKS = 3,            // DATA_LATE: ... for a step whose decode was not registered ahead (>= 2, >= ADDR_CLKS)
    parameter logic [23:0] PARK_ADDR = 24'hFF0000,   // idle-cycle read address (must be ROM)
    parameter bit          DATA_LATE = 1'b0,         // 1 = launch the next step's read one step ahead (needs cpu_na_i)
    parameter bit          ROM3      = 1'b0          // 0 = ROM 01 machine, 1 = ROM 3
) (
    input  logic        clk,
    input  logic        rst_n,

    // control (quasi-static; taken at step boundaries)
    input  logic        native_i,        // 1 = PHI2-paced, every step a socket cycle (S6)
    input  logic        mem_bypass_i,    // native only: 1 = read data from the socket for everything
    input  logic        cxrom_local_i,   // replica: internal $C100-$CFFF firmware served locally
    input  logic [6:0]  exp_banks_i,     // replica: expansion banks backed by the card

    // PHY side (gs_socket_phy)
    input  logic        phy_enabled_i,   // enabled_o: the pins are ours
    input  logic        phy_running_i,   // running_o: /RES released (PH2 domain, changes at a fall)
    input  logic        phy_fall_evt_i,  // fall_evt_o
    input  logic        phy_issue_evt_i, // issue_evt_o: the PHY registers the cycle in this clk
    output logic [23:0] phy_a_o,         // -> cpu_a_i
    output logic [7:0]  phy_d_out_o,     // -> cpu_d_out_i
    output logic        phy_we_n_o,      // -> cpu_we_n_i (1 = read)
    output logic        phy_vpa_o,
    output logic        phy_vda_o,
    output logic        phy_vp_n_o,

    // socket pads, sampled here at the PHI2 fall
    input  logic        gs_ph2_i,
    input  logic [7:0]  gs_d_i,
    input  logic        gs_rdy_i,
    input  logic        gs_irq_n_i,
    input  logic        gs_nmi_n_i,
    input  logic        gs_abort_n_i,

    // core side (P65C816 / F65C816 port list; tie the core's RDY_IN high)
    output logic        cpu_rst_n_o,
    output logic        cpu_ce_o,
    output logic [7:0]  cpu_d_in_o,
    output logic        cpu_irq_n_o,
    output logic        cpu_nmi_n_o,
    output logic        cpu_abort_n_o,
    input  logic [23:0] cpu_a_i,
    input  logic [7:0]  cpu_d_out_i,
    input  logic        cpu_we_n_i,      // 1 = read
    input  logic        cpu_vpa_i,
    input  logic        cpu_vda_i,
    input  logic        cpu_vp_n_i,
    input  logic [23:0] cpu_na_i,        // F65C816.NA_OUT: address of the next step (tie 0 without DATA_LATE)

    // local memory, synchronous.  Read: lm_ren_o registers the addresses and
    // the memories read; the data is valid from the next clk and held until
    // the next lm_ren_o.  Write: RAM only, at lm_wen_o; never in the same clk
    // as a read launch.
    output logic        lm_ren_o,
    output logic [16:0] lm_ram_ra_o,     // read address of the bank-00/01 RAM
    output logic [17:0] lm_rom_ra_o,     // read address of the ROM image
    output logic [23:0] lm_raddr_o,      // RAM physical address, all banks (expansion memory)
    input  logic [7:0]  lm_ram_rdata_i,
    input  logic [7:0]  lm_rom_rdata_i,
    output logic        lm_wen_o,
    output logic [23:0] lm_waddr_o,
    output logic [7:0]  lm_wdata_o,

    // status / telemetry (clk domain)
    output logic        fast_o,          // the step in progress is a fast-mode step
    output logic        step_evt_o,      // = cpu_ce_o: one core step
    output logic        sock_evt_o,      // a non-idle socket cycle completed
    output logic        sock_idle_o,     // the cycle the PHY issued last is an idle cycle
    output logic        pw_busy_o,       // a posted write is waiting or on the bus
    output logic        verify_miss_o,   // native, card memory of record: local byte != socket byte
    output logic [23:0] verify_addr_o,   // address of the last mismatch
    output logic [15:0] verify_count_o,
    output logic [31:0] stat_steps_o,    // core steps
    output logic [31:0] stat_sock_o,     // socket cycles run for the core (reads, synchronous writes)
    output logic [31:0] stat_posted_o,   // posted writes completed
    output logic [31:0] stat_pwwait_o,   // clks a forwarded write waited for the buffer
    output logic [31:0] stat_rd_hit_o,   // local reads whose data and select were already in place
    output logic [31:0] stat_rd_miss_o,  // local reads that launched or re-selected at the strobe and waited
    output logic [7:0]  shadow_o,        // replica state
    output logic [7:0]  cyareg_o,
    output logic [7:0]  sltromsel_o,
    output logic [7:0]  statereg_o,
    output logic [7:0]  sw_misc_o
);

    //=========================================================================
    // PH2 domain: the pads at the fall
    //=========================================================================
    logic [7:0] d_f     = 8'h00;
    logic       rdy_f   = 1'b1;
    logic       irq_f   = 1'b1;
    logic       nmi_f   = 1'b1;
    logic       abort_f = 1'b1;

    always_ff @(negedge gs_ph2_i) begin
        d_f     <= gs_d_i;
        rdy_f   <= gs_rdy_i;
        irq_f   <= gs_irq_n_i;
        nmi_f   <= gs_nmi_n_i;
        abort_f <= gs_abort_n_i;
    end

    // clk-domain copies, loaded at the fall event; in the fall-event clk
    // itself the PH2-domain flops are read directly (they have been stable
    // for at least a clk), so a native step sees the pads of ITS fall.
    logic rdy_c, irq_c, nmi_c, abort_c;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rdy_c <= 1'b1; irq_c <= 1'b1; nmi_c <= 1'b1; abort_c <= 1'b1;
        end else if (phy_fall_evt_i) begin
            rdy_c <= rdy_f; irq_c <= irq_f; nmi_c <= nmi_f; abort_c <= abort_f;
        end
    end

    // Fast mode takes the interrupt lines through ordinary two-flop
    // synchronisers instead: a sample that is up to a whole PHI2 cycle old
    // would still show /IRQ low after the handler's acknowledging write has
    // released it, and a core that runs a dozen local steps per PHI2 cycle
    // would reach RTI and take the same interrupt again.
    logic [1:0] irq_s, nmi_s, abort_s;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            irq_s <= 2'b11; nmi_s <= 2'b11; abort_s <= 2'b11;
        end else begin
            irq_s   <= {irq_s[0],   gs_irq_n_i};
            nmi_s   <= {nmi_s[0],   gs_nmi_n_i};
            abort_s <= {abort_s[0], gs_abort_n_i};
        end
    end

    logic mode_native, mode_bypass;      // mode of the step in progress (below)
    assign cpu_irq_n_o   = !mode_native ? irq_s[1]   : phy_fall_evt_i ? irq_f   : irq_c;
    assign cpu_nmi_n_o   = !mode_native ? nmi_s[1]   : phy_fall_evt_i ? nmi_f   : nmi_c;
    assign cpu_abort_n_o = !mode_native ? abort_s[1] : phy_fall_evt_i ? abort_f : abort_c;

    //=========================================================================
    // Reset: follow the PHY's view of /RES, re-registered at the fall event
    //=========================================================================
    logic run_c;
    wire  run_now = phy_enabled_i & (phy_fall_evt_i ? phy_running_i : run_c);
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) run_c <= 1'b0;
        else        run_c <= run_now;
    end
    assign cpu_rst_n_o = run_c;

    // mode inputs: two flops, then taken at step boundaries
    logic [1:0] native_s, bypass_s;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            native_s <= 2'b11; bypass_s <= 2'b11;
        end else begin
            native_s <= {native_s[0], native_i};
            bypass_s <= {bypass_s[0], mem_bypass_i};
        end
    end

    //=========================================================================
    // The step in progress: effective access and its classification
    //=========================================================================
    typedef enum logic [1:0] { ST_RESET, ST_ADDR, ST_DATA, ST_SOCK } st_t;
    st_t         st;
    logic [3:0]  ph;                 // clks spent in ST_ADDR / left in ST_DATA
    logic [23:0] prev_addr;          // effective address of the previous step

    // internal cycle aimed at I/O space (the PHY's condition): repeat the
    // previous step's address as a read instead of presenting a new I/O
    // address the real chip would not have put on the bus
    wire        internal = ~cpu_vpa_i & ~cpu_vda_i;
    wire        io_int   = internal & (cpu_a_i[15:8] == 8'hC0) &
                           ((cpu_a_i[23:17] == 7'd0) | (cpu_a_i[23:17] == 7'h70));
    wire [23:0] eff_addr = io_int ? prev_addr : cpu_a_i;
    wire        eff_we   = ~io_int & ~cpu_we_n_i;            // 1 = write

    logic [2:0]  r_region;
    logic [23:0] r_phys;
    logic [17:0] r_rom_addr;
    logic        r_local, r_fwd, r_local_we, r_local_rom, r_side;
    logic [23:0] pf_phys;
    logic [17:0] pf_rom_addr;
    logic        pf_local, pf_local_rom, pf_side, pfw_local, pfw_local_we, pfw_fwd, sw_touch;
    logic [23:0] pfw_phys;
    logic [16:0] pf_ram_ra;
    logic [17:0] pf_rom_ra;
    logic        cyc_stb;
    logic [23:0] fl_addr;
    logic        fl_we;
    logic [7:0]  fl_data;

    gs_mmu_replica #(.ROM3(ROM3)) u_replica (
        .clk(clk), .rst_n(rst_n), .sw_reset_i(~run_c),
        .cxrom_local_i(cxrom_local_i), .exp_banks_i(exp_banks_i),
        .cyc_stb_i(cyc_stb), .cyc_addr_i(fl_addr), .cyc_we_i(fl_we), .cyc_wdata_i(fl_data),
        .acc_addr_i(eff_addr), .acc_we_i(eff_we), .acc_vpb_n_i(cpu_vp_n_i | io_int),
        .acc_region_o(r_region), .acc_phys_o(r_phys), .acc_local_o(r_local),
        .acc_forward_write_o(r_fwd), .acc_local_we_o(r_local_we), .acc_local_rom_o(r_local_rom),
        .acc_rom_addr_o(r_rom_addr), .acc_side_effect_o(r_side),
        .pf_addr_i(cpu_na_i), .pf_phys_o(pf_phys), .pf_rom_addr_o(pf_rom_addr),
        .pf_local_o(pf_local), .pf_local_rom_o(pf_local_rom), .pf_side_o(pf_side),
        .pf_ram_ra_o(pf_ram_ra), .pf_rom_ra_o(pf_rom_ra),
        .pfw_phys_o(pfw_phys), .pfw_local_o(pfw_local), .pfw_local_we_o(pfw_local_we), .pfw_forward_o(pfw_fwd),
        .sw_touch_o(sw_touch),
        .shadow_o(shadow_o), .cyareg_o(cyareg_o), .sltromsel_o(sltromsel_o),
        .statereg_o(statereg_o), .misc_o(sw_misc_o)
    );

    // ---- the decode view the step is run with
    // Two-edge step: the replica's combinational decode (r_*), used at the strobe.
    // Data-late step: nothing the sequencer registers hangs off that decode
    // directly.  A step that follows a local step uses the decode of its own
    // address made a step ahead (pl_*: the replica's pf_ views of the core's
    // next-step address, as a read and as a write, registered at the launch);
    // any other step uses the registered copy of r_* (aq_*), which it gives
    // SLOW_CLKS - 1 clks to settle.  pl_* is the same function of the same
    // address and switch state as r_* as long as the switches have not been
    // touched since (sw_touch), the step is not a vector pull and its address
    // is not substituted (io_int) - cross-checked in simulation below.
    logic        pl_fresh;
    logic        pl_r_local, pl_r_rom, pl_side, pl_w_local, pl_w_we, pl_w_fwd;
    logic [23:0] pl_w_phys;
    logic        aq_local, aq_fwd, aq_local_we, aq_local_rom, aq_side;
    logic [23:0] aq_phys;
    logic [17:0] aq_rom_addr;
    logic [7:0]  cfg_q;

    wire        fresh_ok    = DATA_LATE & pl_fresh & cpu_vp_n_i & ~io_int;
    wire        d_local     = !DATA_LATE ? r_local     : fresh_ok ? (eff_we ? pl_w_local : pl_r_local) : aq_local;
    wire        d_local_we  = !DATA_LATE ? r_local_we  : fresh_ok ? pl_w_we  : aq_local_we;
    wire        d_fwd       = !DATA_LATE ? r_fwd       : fresh_ok ? pl_w_fwd : aq_fwd;
    wire        d_local_rom = !DATA_LATE ? r_local_rom : fresh_ok ? pl_r_rom : aq_local_rom;
    wire        d_side      = !DATA_LATE ? r_side      : fresh_ok ? pl_side  : aq_side;
    wire [23:0] d_phys      = !DATA_LATE ? r_phys      : fresh_ok ? pl_w_phys : aq_phys;
    wire [17:0] d_rom_addr  = !DATA_LATE ? r_rom_addr  : aq_rom_addr;

    // what this step needs
    wire need_sock = mode_native | (internal ? d_side : ~d_local);
    wire lm_read   = ~internal & d_local & ~eff_we;
    wire lm_write  = ~internal & eff_we & d_local & d_local_we;
    wire post      = ~mode_native & ~internal & eff_we & d_fwd;
    wire use_lm    = lm_read & ~(mode_native & mode_bypass);

    //=========================================================================
    // Socket side: one request at a time, posted write first
    //=========================================================================
    typedef enum logic [1:0] { K_IDLE, K_SYNC, K_PW } kind_t;

    logic        sreq_valid;              // the step's own socket cycle
    logic        sreq_now;                // ... being requested in this clk (strobe & need_sock, below)
    logic [23:0] sreq_addr;
    logic        sreq_we, sreq_vpa, sreq_vda, sreq_vp_n;
    logic [7:0]  sreq_data;

    logic        pw_valid;                // one-deep posted-write buffer
    logic [23:0] pw_addr;
    logic [7:0]  pw_data;

    kind_t       fl_kind;                 // the cycle the PHY issued last
    logic        fl_busy;                 // ... and it has not completed
    logic        fl_run;                  // ... and the core was running when it was issued
                                          // (the PHY repeats a cycle on RDY low only then)

    kind_t sel;
    always_comb begin
        if (fl_busy)         sel = fl_kind;     // RDY held it: the PHY repeats it
        else if (pw_valid)   sel = K_PW;        // older than anything the core is waiting for
        else if (sreq_valid | sreq_now) sel = K_SYNC;
        else                 sel = K_IDLE;
    end

    always_comb begin
        unique case (sel)
            K_PW: begin
                phy_a_o = pw_addr;  phy_d_out_o = pw_data;  phy_we_n_o = 1'b0;
                phy_vpa_o = 1'b0;   phy_vda_o = 1'b1;       phy_vp_n_o = 1'b1;
            end
            K_SYNC: begin
                if (sreq_valid) begin
                    phy_a_o = sreq_addr; phy_d_out_o = sreq_data; phy_we_n_o = ~sreq_we;
                    phy_vpa_o = sreq_vpa; phy_vda_o = sreq_vda;   phy_vp_n_o = sreq_vp_n;
                end else begin
                    // the request being registered in this clk (same fields)
                    phy_a_o = eff_addr;  phy_d_out_o = cpu_d_out_i; phy_we_n_o = ~eff_we;
                    phy_vpa_o  = io_int ? 1'b0 : cpu_vpa_i;
                    phy_vda_o  = io_int ? 1'b1 : cpu_vda_i;
                    phy_vp_n_o = io_int ? 1'b1 : cpu_vp_n_i;
                end
            end
            default: begin
                // idle: park; while the core is in reset show its reset-state
                // address, as S6 does
                phy_a_o = run_c ? PARK_ADDR : cpu_a_i;
                phy_d_out_o = 8'h00; phy_we_n_o = 1'b1;
                phy_vpa_o = 1'b0;    phy_vda_o = 1'b0;      phy_vp_n_o = 1'b1;
            end
        endcase
    end

    wire cyc_done  = phy_fall_evt_i & fl_busy & (rdy_f | ~fl_run);   // the issued cycle completed
    wire sync_done = cyc_done & (fl_kind == K_SYNC) & (st == ST_SOCK);
    wire pw_done   = cyc_done & (fl_kind == K_PW);
    assign cyc_stb = cyc_done & (fl_kind != K_IDLE);

    //=========================================================================
    // Step sequencer
    //=========================================================================
    // strobe: the step's decision.  ADDR_CLKS into the step; in the data-late
    // step SLOW_CLKS for a fast-mode step whose decode was not registered
    // ahead (a native step needs no decode to go to the socket).  A forwarded
    // write waits for the buffer, a fast step waits while RDY is low.  Nothing
    // the broker or the PHY registers from the core's outputs is captured
    // before the strobe, so with ADDR_CLKS = 2 those are two-clk paths; the
    // PHY's issue point is two clks after the core steps, and a request made
    // in that very clk is presented live (sreq_now) as well as registered.
    localparam int SLOW  = (SLOW_CLKS > ADDR_CLKS) ? ((SLOW_CLKS > 2) ? SLOW_CLKS : 2) : ((ADDR_CLKS > 2) ? ADDR_CLKS : 2);
    localparam int PH_ON = (STEP_CLKS > ADDR_CLKS + 1) ? STEP_CLKS - ADDR_CLKS - 1 : 0;   // clks left after a strobe at ADDR_CLKS
    localparam int PH_SL = (STEP_CLKS > SLOW + 1)      ? STEP_CLKS - SLOW - 1      : 0;   // ... at SLOW
    wire [3:0] t_dec  = (!DATA_LATE | mode_native | fresh_ok) ? 4'(ADDR_CLKS - 1) : 4'(SLOW - 1);
    wire strobe = (st == ST_ADDR) & (ph >= t_dec) &
                  ~(post & pw_valid) & (mode_native | rdy_c);
    // (native mode only: there the strobe is exactly this clk and the request is certain;
    // a fast-mode request is registered and goes out at the next issue point)
    assign sreq_now = run_c & mode_native & (st == ST_ADDR) & (ph >= 4'(ADDR_CLKS - 1));

    // mem_evt: the step's memory actions (the write, a read that has to be
    // launched now, the verify flag).  With the strobe - except in the
    // data-late native step, whose strobe comes before the decode it needs.
    logic [3:0] sc;                                               // clks since the core stepped
    wire  mem_evt = (DATA_LATE & mode_native) ? (run_c & (st != ST_RESET) & (sc == 4'(SLOW - 1))) : strobe;

    logic use_lm_r, rom_r, vfy_r;

    // what the memories' output registers hold: the addresses of the last
    // launch, valid until a write (which may have changed that RAM byte)
    logic        ld_valid;
    logic [23:0] ld_phys;                 // expansion memory: physical address
    logic [16:0] ld_ram;                  // bank-00/01 RAM: the address it was read at
    logic [17:0] ld_rom;                  // ROM image: the address it was read at
    wire  ld_hit    = fresh_ok | (ld_valid & (d_local_rom       ? (ld_rom == d_rom_addr) :
                                              ~|d_phys[23:17]   ? (ld_ram == d_phys[16:0]) :
                                                                  (ld_phys == d_phys)));
    wire  rd_launch = lm_read & ~ld_hit;                          // this step's read is not there yet
    // data-late: the data-source select the step needs, against what was set
    // ahead.  It matters on steps whose data the core uses: reads.
    wire  rd_step   = ~internal & ~eff_we;
    wire  sel_ok    = fresh_ok | ((use_lm_r == use_lm) & (~use_lm | (rom_r == d_local_rom)));
    wire  sel_fix   = DATA_LATE & rd_step & ~sel_ok;
    wire  rd_slow   = DATA_LATE & (rd_launch | (lm_read & ~sel_ok));   // a local read that has to wait

    // The core's clock enable is a register: a local step's is known a clk
    // ahead, a socket step completes the clk AFTER the PHI2 fall that ends its
    // cycle (the data byte then has three clks from the fall to the core).
    logic ce_r;
    wire [3:0] ph_i = rd_slow ? 4'(STEP_CLKS + 1) : !DATA_LATE ? 4'(STEP_CLKS - ADDR_CLKS - 1) :
                      fresh_ok ? 4'(PH_ON) : 4'(PH_SL);           // clks a local step still takes after the strobe
    wire  local_ce  = ce_r & (st == ST_DATA);                     // a local step completes
    wire  pf_launch = DATA_LATE & local_ce;                       // launch the next step's read

    assign cpu_ce_o = ce_r;
    wire [7:0] lm_rdata = rom_r ? lm_rom_rdata_i : lm_ram_rdata_i;
    assign cpu_d_in_o = use_lm_r ? lm_rdata : d_f;

    // a missing read: two-edge step - launched at the strobe from the decode;
    // data-late - its addresses are registered at mem_evt, presented to the
    // memories for two clks (ml_sel_r) and launched in the second (ml_go).
    // The memories' read address is otherwise always the next-step address,
    // so nothing switches it in the clk of an ordinary launch.
    logic        ml_sel_r, ml_go, ml_apply, ml_use, ml_romsel;
    logic [23:0] ml_phys;
    logic [17:0] ml_rom;
    assign lm_ren_o    = pf_launch | (DATA_LATE ? ml_go : (strobe & rd_launch));
    assign lm_raddr_o  = !DATA_LATE ? r_phys            : ml_sel_r ? ml_phys       : pf_phys;
    assign lm_ram_ra_o = !DATA_LATE ? r_phys[16:0]      : ml_sel_r ? ml_phys[16:0] : pf_ram_ra;
    assign lm_rom_ra_o = !DATA_LATE ? r_rom_addr        : ml_sel_r ? ml_rom        : pf_rom_ra;
    assign lm_wen_o       = mem_evt & lm_write;
    assign lm_waddr_o     = d_phys;
    assign lm_wdata_o     = cpu_d_out_i;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            st <= ST_RESET; ph <= '0;
            mode_native <= 1'b1; mode_bypass <= 1'b1;
            prev_addr <= '0;
            sreq_valid <= 1'b0; sreq_addr <= '0; sreq_we <= 1'b0; sreq_data <= '0;
            sreq_vpa <= 1'b0; sreq_vda <= 1'b0; sreq_vp_n <= 1'b1;
            pw_valid <= 1'b0; pw_addr <= '0; pw_data <= '0;
            fl_kind <= K_IDLE; fl_busy <= 1'b0; fl_run <= 1'b0;
            fl_addr <= '0; fl_we <= 1'b0; fl_data <= '0;
            use_lm_r <= 1'b0; rom_r <= 1'b0; vfy_r <= 1'b0;
            ld_valid <= 1'b0; ld_phys <= '0; ld_rom <= '0;
            ml_go <= 1'b0; ml_sel_r <= 1'b0; ml_phys <= '0; ml_rom <= '0; ld_ram <= '0;
            ml_apply <= 1'b0; ml_use <= 1'b0; ml_romsel <= 1'b0;
            ce_r <= 1'b0; sc <= '0; pl_fresh <= 1'b0; cfg_q <= '0;
            pl_r_local <= 1'b0; pl_r_rom <= 1'b0; pl_side <= 1'b0;
            pl_w_local <= 1'b0; pl_w_we <= 1'b0; pl_w_fwd <= 1'b0; pl_w_phys <= '0;
            aq_local <= 1'b0; aq_fwd <= 1'b0; aq_local_we <= 1'b0; aq_local_rom <= 1'b0; aq_side <= 1'b0;
            aq_phys <= '0; aq_rom_addr <= '0;
        end else if (!run_now) begin
            // core in reset (or the socket is not ours): nothing pending
            st <= ST_RESET; ph <= '0;
            mode_native <= native_s[1]; mode_bypass <= bypass_s[1];
            prev_addr <= cpu_a_i;
            sreq_valid <= 1'b0; pw_valid <= 1'b0;
            use_lm_r <= 1'b0; vfy_r <= 1'b0;
            ld_valid <= 1'b0; ml_go <= 1'b0; ml_sel_r <= 1'b0; ml_apply <= 1'b0;
            ce_r <= 1'b0; sc <= '0; pl_fresh <= 1'b0;
            // the PHY keeps issuing (idle) cycles during reset
            if (phy_issue_evt_i) begin fl_kind <= K_IDLE; fl_busy <= 1'b1; fl_run <= 1'b0; end
            else if (phy_fall_evt_i) fl_busy <= 1'b0;
        end else begin
            // ---- socket bookkeeping
            if (phy_issue_evt_i) begin
                fl_kind <= sel;
                fl_busy <= 1'b1;
                fl_run  <= 1'b1;
                fl_addr <= phy_a_o;
                fl_we   <= ~phy_we_n_o;
                fl_data <= phy_d_out_o;
            end
            if (cyc_done) fl_busy <= 1'b0;
            if (pw_done)  pw_valid <= 1'b0;

            ml_go    <= ml_sel_r & ~ml_go;
            ml_sel_r <= ml_sel_r & ~ml_go;
            ml_apply <= 1'b0;
            sc       <= cpu_ce_o ? 4'd0 : (sc != 4'd15) ? sc + 4'd1 : sc;
            ce_r     <= sync_done | (strobe & ~need_sock & (ph_i == 4'd0)) | ((st == ST_DATA) & (ph == 4'd1));

            // data-late: at the launch of the next step's read, the decode of
            // that step and its data-source select are registered with it; the
            // select is otherwise corrected one clk after mem_evt
            if (pf_launch) begin
                use_lm_r   <= pf_local & ~(native_s[1] & bypass_s[1]);
                rom_r      <= pf_local_rom;
                pl_r_local <= pf_local;   pl_r_rom <= pf_local_rom;   pl_side  <= pf_side;
                pl_w_local <= pfw_local;  pl_w_we  <= pfw_local_we;   pl_w_fwd <= pfw_fwd;
                pl_w_phys  <= pfw_phys;
            end else if (ml_apply) begin
                use_lm_r <= ml_use;
                rom_r    <= ml_romsel;
            end
            // ... and is good for the one step that follows, unless the map is touched
            cfg_q <= {cxrom_local_i, exp_banks_i};
            if (sw_touch | (cfg_q != {cxrom_local_i, exp_banks_i})) pl_fresh <= 1'b0;
            else if (pf_launch)                                     pl_fresh <= 1'b1;
            else if (cpu_ce_o)                                      pl_fresh <= 1'b0;
            // the registered copy of the combinational decode
            if (DATA_LATE) begin
                aq_local <= r_local; aq_fwd <= r_fwd; aq_local_we <= r_local_we;
                aq_local_rom <= r_local_rom; aq_side <= r_side;
                aq_phys <= r_phys; aq_rom_addr <= r_rom_addr;
            end

            // ---- memory actions of the step
            if (mem_evt) begin
                vfy_r <= mode_native & ~mode_bypass & lm_read;
                if (DATA_LATE) begin
                    if (rd_launch) begin
                        ml_sel_r <= 1'b1;
                        ml_phys  <= d_phys;
                        ml_rom   <= d_rom_addr;
                    end
                    if (sel_fix) begin
                        ml_apply  <= 1'b1;
                        ml_use    <= use_lm;
                        ml_romsel <= d_local_rom;
                    end
                end
            end

            // ---- what the memory output registers hold
            if (lm_ren_o) begin
                ld_valid <= 1'b1;
                ld_phys  <= lm_raddr_o;
                ld_ram   <= lm_ram_ra_o;
                ld_rom   <= lm_rom_ra_o;
            end else if (lm_wen_o) begin
                ld_valid <= 1'b0;
            end

            // ---- step sequencer
            unique case (st)
                ST_RESET: begin
                    st <= ST_ADDR; ph <= '0;
                end

                ST_ADDR: begin
                    if (strobe) begin
                        if (!DATA_LATE) begin
                            use_lm_r <= use_lm;
                            rom_r    <= r_local_rom;
                        end
                        if (post) begin
                            pw_valid <= 1'b1;
                            pw_addr  <= eff_addr;
                            pw_data  <= cpu_d_out_i;
                        end
                        if (need_sock) begin
                            sreq_valid <= 1'b1;
                            sreq_addr  <= eff_addr;
                            sreq_we    <= eff_we;
                            sreq_data  <= cpu_d_out_i;
                            // a substituted internal I/O cycle goes out as a
                            // plain read (the PHY must not substitute again)
                            sreq_vpa   <= io_int ? 1'b0 : cpu_vpa_i;
                            sreq_vda   <= io_int ? 1'b1 : cpu_vda_i;
                            sreq_vp_n  <= io_int ? 1'b1 : cpu_vp_n_i;
                            st <= ST_SOCK;
                        end else begin
                            st <= ST_DATA;
                            // data-late: a read that has to be launched now
                            // (next clk) gets a whole STEP_CLKS for memory ->
                            // core; everything else ends the step STEP_CLKS
                            // after the previous one (or as soon as it is decided)
                            ph <= ph_i;
                        end
                    end else if (ph != 4'd15) begin
                        ph <= ph + 4'd1;
                    end
                end

                ST_DATA: begin
                    if (ph != 4'd0) ph <= ph - 4'd1;
                end

                default: ;   // ST_SOCK: wait for sync_done
            endcase

            // ---- the step completes
            if (cpu_ce_o) begin
                st          <= ST_ADDR;
                ph          <= '0;
                prev_addr   <= eff_addr;
                sreq_valid  <= 1'b0;
                mode_native <= native_s[1];
                mode_bypass <= bypass_s[1];
            end
        end
    end

    //=========================================================================
    // Verify (native, card memory of record) and statistics
    //=========================================================================
    wire vfy_miss = sync_done & vfy_r & (d_f != lm_rdata);

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            verify_miss_o <= 1'b0; verify_addr_o <= '0; verify_count_o <= '0;
            stat_steps_o <= '0; stat_sock_o <= '0; stat_posted_o <= '0; stat_pwwait_o <= '0;
            stat_rd_hit_o <= '0; stat_rd_miss_o <= '0;
        end else begin
            verify_miss_o <= vfy_miss;
            if (vfy_miss) begin
                verify_addr_o <= sreq_addr;
                if (verify_count_o != 16'hFFFF) verify_count_o <= verify_count_o + 16'd1;
            end
            if (cpu_ce_o)  stat_steps_o  <= stat_steps_o + 32'd1;
            if (sync_done) stat_sock_o   <= stat_sock_o + 32'd1;
            if (pw_done)   stat_posted_o <= stat_posted_o + 32'd1;
            if (mem_evt & lm_read & ~(DATA_LATE ? rd_slow : rd_launch)) stat_rd_hit_o  <= stat_rd_hit_o + 32'd1;
            if (mem_evt & lm_read &  (DATA_LATE ? rd_slow : rd_launch)) stat_rd_miss_o <= stat_rd_miss_o + 32'd1;
            if ((st == ST_ADDR) & (ph >= t_dec) & post & pw_valid)
                stat_pwwait_o <= stat_pwwait_o + 32'd1;
        end
    end

`ifdef VERILATOR
    // The decode registered a step ahead must be the decode of the step: every
    // time it is used, compare it with the replica's combinational view.
    always @(posedge clk) if (rst_n && run_c && fresh_ok && (strobe || mem_evt)) begin
        if (internal ? (pl_side !== r_side) :
            eff_we   ? (pl_w_local !== r_local || pl_w_we !== r_local_we || pl_w_fwd !== r_fwd ||
                        (r_local && pl_w_phys !== r_phys)) :
                       (pl_r_local !== r_local || (r_local && pl_r_rom !== r_local_rom) ||
                        (r_local && !ld_valid) ||
                        (r_local && (r_local_rom       ? (ld_rom !== r_rom_addr) :
                                     ~|r_phys[23:17]   ? (ld_ram !== r_phys[16:0]) : (ld_phys !== r_phys))))) begin
            $display("ERROR: broker step-ahead decode differs from the step's: a=%06h we=%b int=%b @%0t",
                     eff_addr, eff_we, internal, $time);
            $finish;
        end
    end
`endif

    assign fast_o      = run_c & ~mode_native;
    assign step_evt_o  = cpu_ce_o;
    assign sock_evt_o  = cyc_stb;
    assign sock_idle_o = fl_busy & (fl_kind == K_IDLE);
    assign pw_busy_o   = pw_valid;

endmodule
