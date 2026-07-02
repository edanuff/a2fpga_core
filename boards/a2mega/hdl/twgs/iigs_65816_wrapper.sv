//
// IIgs 65816 CPU socket interposer - Phase 2 bus engine
//
// (c) 2023-2026 Ed Anuff <ed@a2fpga.com>
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
// Drives the Apple IIgs 65816 CPU socket through the A2Mega GS connector
// (J8 / U8 / U10 / U11), replacing the removed 65C816 with the P65C816
// soft core, CE-locked 1:1 to the motherboard PH2 clock. No acceleration
// in this phase; the design goal is to be indistinguishable from a stock
// CPU. See boards/a2mega/docs/twgs_action_plan.md (Phase 2).
//
// Bus cycle model (verified against the TWGS schematic and Apple IIGS
// Tech Note #68):
//   - PH2 low (PHI1): drive address, R/W, and the bank byte on D0-7.
//     The FPI's bank latch is transparent while PH2 is low and closes at
//     PH2 rise, so bank+address must be stable before the rise.
//   - PH2 high: writes keep driving D0-7 with write data; reads release
//     D0-7 and sample near the PH2 fall.
//   - RDY low (sampled while PH2 high, per the FPI rule): repeat the
//     cycle without advancing the core; keep all outputs stable.
//   - BE low: release address/data/RW drivers (DMA-style bus grant).
//
// /VP workaround (action plan 2.8a): this board revision cannot drive
// /VP to the motherboard, so the FPI cannot redirect interrupt-vector
// fetches to ROM. After each reset this engine performs 32 read cycles
// of bank $00 $FFE0-$FFFF (ROM is banked in at reset) and thereafter
// serves vector-pull reads (core VPB asserted) from the snapshot,
// replicating the FPI's "vectors always from ROM" behavior internally.
//
// Bus parking: when the core reports an internal-operation cycle
// (VDA=VPA=0), the last valid address is re-presented with R/W=read,
// imitating a real 65816 internal cycle (safe: the socket carries no
// VDA/VPA, so the FPI must tolerate exactly these cycles).
//
// P65C816 core port semantics (srg320, VHDL — see 65C816/P65C816.vhd):
//   WE is active-low (1 = read, 0 = write, i.e. RWB); VPB/MLB active-low;
//   VDA/VPA active-high; the core advances one cycle per CE=1 clock and
//   samples D_IN on that same edge; RDY_OUT is low while WAI/STP idle.
//

module iigs_65816_wrapper #(
    parameter bit VECTOR_SNAPSHOT_ENABLE = 1'b1,
    parameter int READ_TAP = 1              // gs_d pipeline tap used at PH2 fall
) (
    input  logic        clk_i,              // 54 MHz clk_logic
    input  logic        device_reset_n_i,   // FPGA power-on reset

    input  logic        monitor_enable_i,   // enable U8 port 1 (control inputs only)
    input  logic        cpu_enable_i,       // drive the socket as the CPU

    // GS socket, FPGA side of the level shifters
    input  logic        gs_ph2_i,
    input  logic        gs_reset_n_i,
    input  logic        gs_irq_n_i,
    input  logic        gs_nmi_n_i,
    input  logic        gs_abort_n_i,
    input  logic        gs_rdy_i,
    input  logic        gs_be_i,

    output logic [15:0] gs_a_o,
    input  logic [7:0]  gs_d_i,
    output logic [7:0]  gs_d_o,
    output logic        gs_d_drive_o,       // FPGA pin output enable for gs_d
    output logic        gs_rw_o,            // 1 = read, 0 = write

    // Level-shifter controls (see a2mega.cst for hardware semantics)
    output logic        gs_oe_n_o,          // U8 port 1 enable (control inputs)
    output logic        gs_data_oe_n_o,     // U8 port 2 enable (D0-7)
    output logic        gs_addr_oe_n_o,     // U11 + U10 enable (A0-15, R/W)
    output logic        gs_d_dir_o,         // 1 = FPGA drives D0-7 toward bus

    // Debug / monitor
    output logic [31:0] dbg_ph2_count_o,    // free-running PH2 rise counter
    output logic [23:0] dbg_addr_o,         // last bus address {bank, addr}
    output logic [3:0]  dbg_state_o,
    output logic        dbg_veccap_done_o
);

    //=========================================================================
    // Input synchronization
    //=========================================================================

    logic [2:0] ph2_sync;
    logic [1:0] reset_sync, irq_sync, nmi_sync, abort_sync, rdy_sync, be_sync;

    always_ff @(posedge clk_i or negedge device_reset_n_i) begin
        if (!device_reset_n_i) begin
            ph2_sync   <= '0;
            reset_sync <= 2'b00;    // treat bus as in-reset until proven otherwise
            irq_sync   <= 2'b11;
            nmi_sync   <= 2'b11;
            abort_sync <= 2'b11;
            rdy_sync   <= 2'b11;
            be_sync    <= 2'b11;
        end else begin
            ph2_sync   <= {ph2_sync[1:0], gs_ph2_i};
            reset_sync <= {reset_sync[0], gs_reset_n_i};
            irq_sync   <= {irq_sync[0], gs_irq_n_i};
            nmi_sync   <= {nmi_sync[0], gs_nmi_n_i};
            abort_sync <= {abort_sync[0], gs_abort_n_i};
            rdy_sync   <= {rdy_sync[0], gs_rdy_i};
            be_sync    <= {be_sync[0], gs_be_i};
        end
    end

    wire ph2_w         = ph2_sync[2];
    wire ph2_rise_w    = ph2_sync[1] && !ph2_sync[2];
    wire ph2_fall_w    = !ph2_sync[1] && ph2_sync[2];
    wire bus_reset_n_w = reset_sync[1];
    wire be_w          = be_sync[1];

    // Data-bus sample pipeline: at the synchronized PH2 fall, the wire fell
    // ~2-3 clocks earlier, so the freshest sample may sit in the bus
    // data-hold window. READ_TAP selects an earlier sample taken while PH2
    // was still high (read data is valid well before the fall per 65816
    // bus timing). Tuning hook for Phase 2.2 timing analysis.
    logic [7:0] d_pipe [0:3];
    always_ff @(posedge clk_i) begin
        d_pipe[0] <= gs_d_i;
        d_pipe[1] <= d_pipe[0];
        d_pipe[2] <= d_pipe[1];
        d_pipe[3] <= d_pipe[2];
    end

    // RDY may only change while PH2 is high (FPI rule); capture the value
    // seen during the high phase for the end-of-cycle decision.
    logic rdy_seen_r;
    always_ff @(posedge clk_i) begin
        if (ph2_rise_w)
            rdy_seen_r <= rdy_sync[1];
        else if (ph2_w && !rdy_sync[1])
            rdy_seen_r <= 1'b0;
    end

    //=========================================================================
    // P65C816 core
    //=========================================================================

    logic        cpu_ce;
    logic [7:0]  cpu_din;
    logic [7:0]  cpu_dout;
    logic [23:0] cpu_addr;
    logic        cpu_we_n;       // core WE: 1 = read, 0 = write (RWB)
    logic        cpu_rdy_out;    // low while WAI/STP idle (unused: bus keeps parking)
    logic        cpu_vpa, cpu_vda;
    logic        cpu_vpb_n;      // 0 = vector pull cycle

    wire core_rst_n_w = device_reset_n_i && bus_reset_n_w && cpu_enable_i;

    P65C816 cpu_core (
        .CLK     (clk_i),
        .RST_N   (core_rst_n_w),
        .CE      (cpu_ce),

        .RDY_IN  (1'b1),          // bus stalls are applied by withholding CE
        .NMI_N   (nmi_sync[1]),
        .IRQ_N   (irq_sync[1]),
        .ABORT_N (abort_sync[1]),

        .D_IN    (cpu_din),
        .D_OUT   (cpu_dout),
        .A_OUT   (cpu_addr),
        .WE      (cpu_we_n),
        .RDY_OUT (cpu_rdy_out),

        .VPA     (cpu_vpa),
        .VDA     (cpu_vda),
        .MLB     (),
        .VPB     (cpu_vpb_n)
    );

    //=========================================================================
    // Vector snapshot ($00FFE0-$00FFFF, captured post-reset while ROM is in)
    //=========================================================================

    logic [7:0] vec_mem [0:31];
    logic       veccap_done_r;
    logic [4:0] veccap_idx_r;

    // $00FFE0-$00FFFF: 24-bit address with bits [23:5] == 19'h007FF
    wire vec_addr_hit_w = (cpu_addr[23:5] == 19'h007FF);
    wire vec_serve_w    = VECTOR_SNAPSHOT_ENABLE && veccap_done_r &&
                          !cpu_vpb_n && vec_addr_hit_w && cpu_we_n;

    //=========================================================================
    // Bus engine
    //=========================================================================

    typedef enum logic [3:0] {
        ST_DISABLED,     // drivers released, engine idle
        ST_RESET,        // bus reset asserted: park, hold core in reset
        ST_VECCAP_PHI1,  // vector snapshot read cycle: PH2-low phase
        ST_VECCAP_PHI2,  // vector snapshot read cycle: PH2-high phase
        ST_ADVANCE,      // post-fall: core CE issued, latch next cycle
        ST_PHI1,         // normal cycle: PH2 low, address/bank driven
        ST_PHI2,         // normal cycle: PH2 high
        ST_RELEASED      // BE low: drivers released, engine frozen
    } state_t;

    state_t state_r;

    // Registered bus-cycle outputs
    logic [15:0] bus_addr_r;
    logic [7:0]  bus_bank_r;
    logic [7:0]  bus_wdata_r;
    logic        bus_write_r;     // 1 = write cycle
    logic        bus_vec_r;       // serve read data from vec_mem
    logic [23:0] park_addr_r;     // last valid (VDA|VPA) address

    logic        drive_bus_r;     // engine owns the addr/RW/data drivers
    logic        d_drive_r;       // FPGA currently drives the gs_d pins
    logic        d_dir_r;

    wire engine_on_w = cpu_enable_i && device_reset_n_i;

    always_ff @(posedge clk_i or negedge device_reset_n_i) begin
        if (!device_reset_n_i) begin
            state_r       <= ST_DISABLED;
            bus_addr_r    <= 16'hFFFF;
            bus_bank_r    <= 8'h00;
            bus_wdata_r   <= 8'h00;
            bus_write_r   <= 1'b0;
            bus_vec_r     <= 1'b0;
            park_addr_r   <= 24'h00FFFF;
            drive_bus_r   <= 1'b0;
            d_drive_r     <= 1'b0;
            d_dir_r       <= 1'b0;
            cpu_ce        <= 1'b0;
            cpu_din       <= 8'h00;
            veccap_done_r <= 1'b0;
            veccap_idx_r  <= '0;
        end else begin
            cpu_ce <= 1'b0;

            unique case (state_r)

                ST_DISABLED: begin
                    drive_bus_r <= 1'b0;
                    d_drive_r   <= 1'b0;
                    d_dir_r     <= 1'b0;
                    if (engine_on_w)
                        state_r <= ST_RESET;
                end

                // Bus reset asserted: drive parked read cycles so the FPI
                // sees a live CPU, hold the core in reset (core_rst_n_w),
                // schedule a fresh vector capture for when reset releases.
                ST_RESET: begin
                    drive_bus_r   <= 1'b1;
                    veccap_done_r <= 1'b0;
                    veccap_idx_r  <= '0;
                    bus_addr_r    <= 16'hFFFF;
                    bus_bank_r    <= 8'h00;
                    bus_write_r   <= 1'b0;
                    bus_vec_r     <= 1'b0;
                    if (ph2_fall_w) begin
                        d_dir_r   <= 1'b1;      // drive bank during PHI1
                        d_drive_r <= 1'b1;
                    end else if (ph2_rise_w) begin
                        d_drive_r <= 1'b0;      // read cycle: release for PH2
                        d_dir_r   <= 1'b0;
                    end
                    if (!engine_on_w)
                        state_r <= ST_DISABLED;
                    else if (bus_reset_n_w && ph2_fall_w) begin
                        d_dir_r   <= 1'b1;
                        d_drive_r <= 1'b1;
                        state_r   <= VECTOR_SNAPSHOT_ENABLE ? ST_VECCAP_PHI1
                                                            : ST_ADVANCE;
                    end
                end

                // Vector snapshot: 32 read cycles of $00FFE0+idx. ROM is
                // banked into bank-0 $E000-$FFFF at reset, and these
                // locations have no read side effects.
                ST_VECCAP_PHI1: begin
                    bus_addr_r  <= {11'b1111_1111_111, veccap_idx_r};
                    bus_bank_r  <= 8'h00;
                    bus_write_r <= 1'b0;
                    if (!bus_reset_n_w)
                        state_r <= ST_RESET;
                    else if (ph2_rise_w) begin
                        d_drive_r <= 1'b0;
                        d_dir_r   <= 1'b0;
                        state_r   <= ST_VECCAP_PHI2;
                    end
                end

                ST_VECCAP_PHI2: begin
                    if (!bus_reset_n_w) begin
                        state_r <= ST_RESET;
                    end else if (ph2_fall_w) begin
                        d_dir_r   <= 1'b1;      // turn around for next PHI1
                        d_drive_r <= 1'b1;
                        if (rdy_seen_r) begin
                            vec_mem[veccap_idx_r] <= d_pipe[READ_TAP];
                            veccap_idx_r <= veccap_idx_r + 1'b1;
                            if (veccap_idx_r == 5'd31) begin
                                veccap_done_r <= 1'b1;
                                state_r       <= ST_ADVANCE;
                            end else begin
                                state_r <= ST_VECCAP_PHI1;
                            end
                        end else begin
                            state_r <= ST_VECCAP_PHI1;  // RDY stall: repeat
                        end
                    end
                end

                // One clock after the core CE (or after reset/veccap): core
                // outputs for the next cycle are valid; latch and drive PHI1.
                ST_ADVANCE: begin
                    if (cpu_vda || cpu_vpa) begin
                        bus_addr_r  <= cpu_addr[15:0];
                        bus_bank_r  <= cpu_addr[23:16];
                        park_addr_r <= cpu_addr;
                        bus_write_r <= !cpu_we_n;
                    end else begin
                        // Internal-operation cycle: park on last valid address
                        bus_addr_r  <= park_addr_r[15:0];
                        bus_bank_r  <= park_addr_r[23:16];
                        bus_write_r <= 1'b0;
                    end
                    bus_wdata_r <= cpu_dout;
                    bus_vec_r   <= vec_serve_w;
                    d_dir_r     <= 1'b1;
                    d_drive_r   <= 1'b1;
                    state_r     <= ST_PHI1;
                end

                ST_PHI1: begin
                    if (!engine_on_w) begin
                        state_r <= ST_DISABLED;
                    end else if (!bus_reset_n_w) begin
                        state_r <= ST_RESET;
                    end else if (!be_w) begin
                        drive_bus_r <= 1'b0;
                        d_drive_r   <= 1'b0;
                        d_dir_r     <= 1'b0;
                        state_r     <= ST_RELEASED;
                    end else if (ph2_rise_w) begin
                        if (!bus_write_r) begin
                            d_drive_r <= 1'b0;  // read: release for PH2 high
                            d_dir_r   <= 1'b0;
                        end
                        // writes keep driving; gs_d_o switches to write data
                        state_r <= ST_PHI2;
                    end
                end

                ST_PHI2: begin
                    if (!bus_reset_n_w) begin
                        state_r <= ST_RESET;
                    end else if (ph2_fall_w) begin
                        d_dir_r   <= 1'b1;      // turn around for next PHI1
                        d_drive_r <= 1'b1;
                        if (rdy_seen_r) begin
                            // Cycle completes: feed the core and advance.
                            // Writes echo the write data; vector-pull reads
                            // come from the snapshot instead of the bus.
                            if (bus_write_r)
                                cpu_din <= bus_wdata_r;
                            else if (bus_vec_r)
                                cpu_din <= vec_mem[bus_addr_r[4:0]];
                            else
                                cpu_din <= d_pipe[READ_TAP];
                            cpu_ce  <= 1'b1;
                            state_r <= ST_ADVANCE;
                        end else begin
                            state_r <= ST_PHI1;  // RDY stall: repeat cycle
                        end
                    end
                end

                ST_RELEASED: begin
                    if (!engine_on_w) begin
                        state_r <= ST_DISABLED;
                    end else if (!bus_reset_n_w) begin
                        state_r <= ST_RESET;
                    end else if (be_w && ph2_fall_w) begin
                        // Re-acquire the bus at a cycle boundary
                        drive_bus_r <= 1'b1;
                        d_dir_r     <= 1'b1;
                        d_drive_r   <= 1'b1;
                        state_r     <= ST_PHI1;
                    end
                end

                default: state_r <= ST_DISABLED;
            endcase
        end
    end

    //=========================================================================
    // Pin-level outputs
    //=========================================================================

    always_comb begin
        gs_a_o  = bus_addr_r;
        gs_rw_o = !bus_write_r;
        // PHI1: bank byte; PH2 of a write cycle: write data
        gs_d_o  = (ph2_w && bus_write_r) ? bus_wdata_r : bus_bank_r;
        gs_d_drive_o = d_drive_r;
        gs_d_dir_o   = d_dir_r;

        // U8 port 1 (control inputs toward the FPGA) never drives the bus;
        // enable it whenever monitoring or driving.
        gs_oe_n_o      = !(monitor_enable_i || cpu_enable_i);
        gs_addr_oe_n_o = !(drive_bus_r && engine_on_w);
        gs_data_oe_n_o = !(drive_bus_r && engine_on_w);
    end

    //=========================================================================
    // Debug / monitor
    //=========================================================================

    logic [31:0] ph2_count_r;
    always_ff @(posedge clk_i or negedge device_reset_n_i) begin
        if (!device_reset_n_i)
            ph2_count_r <= '0;
        else if (ph2_rise_w)
            ph2_count_r <= ph2_count_r + 1'b1;
    end

    assign dbg_ph2_count_o   = ph2_count_r;
    assign dbg_addr_o        = {bus_bank_r, bus_addr_r};
    assign dbg_state_o       = state_r;
    assign dbg_veccap_done_o = veccap_done_r;

endmodule
