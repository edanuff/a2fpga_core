// A2MEGA - IIgs CPU-socket 65C816: PHY + core + telemetry wrapper
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
// Everything the a2mega top needs for the in-socket 65C816 (drop-in
// iteration, docs/gs_socket_65816_scoping.md): the P65C816 core clocked by
// PHI2, gs_socket_phy sequencing the pins on clk, and the telemetry the
// bench ladder (Phase C3/C4) reads over the ESP32 register window:
//
//  * status / counters: PH2 alive, core running, bus cycles, RDY stalls,
//    BE-low time, PHI2 period and high time (clk ticks per 256 cycles - the
//    C3 "frequency and duty" measurement), the last address issued;
//  * address-delay sweep: out_extra_i adds clks before the cycle is put on
//    the bus (C4 instrument: find the FPI's real tADS tolerance);
//  * data-hold sweep: hold_tap_i clks after the synchronised fall the D pads
//    are re-sampled and compared with the byte captured AT the fall (what
//    the core took); mismatches vs samples say how long the motherboard
//    really holds read data after the fall (C4 instrument).
//
// All control inputs are quasi-static and already in the clk domain (the
// top synchronises them); telemetry outputs are clk-domain registers that
// the top samples into the ESP32 connector's domain (tearing on a wide
// counter is tolerated, as for the other debug counters).
//
module gs_socket_ctl (
    input  logic        clk,            // sequencer clock (~110 MHz PLL output)
    input  logic        rst_n,

    // control (clk domain, quasi-static)
    input  logic        arm_i,          // take the socket
    input  logic        listen_i,       // enable the control-input shifter only (C3)
    input  logic        sweep_en_i,     // data-hold sweep on
    input  logic [3:0]  out_extra_i,    // address-delay sweep: extra clks before issue
    input  logic [4:0]  hold_tap_i,     // data-hold sweep: clks after the fall event
    input  logic        clear_i,        // hold counters at zero while high

    // socket pins (FPGA_GS_*)
    input  logic        gs_ph2_i,
    input  logic        gs_rdy_i,
    input  logic        gs_irq_n_i,
    input  logic        gs_nmi_n_i,
    input  logic        gs_res_n_i,
    input  logic        gs_abort_n_i,
    input  logic        gs_be_i,
    input  logic [7:0]  gs_d_i,
    output logic [7:0]  gs_d_o,
    output logic        gs_d_oe_o,
    output logic [15:0] gs_a_o,
    output logic        gs_rw_o,
    output logic        gs_vp_o,
    output logic        gs_rdy_out_o,
    output logic        gs_d_dir_o,
    output logic        gs_data_oe_n_o,
    output logic        gs_addr_oe_n_o,
    output logic        gs_ctl_oe_n_o,

    // telemetry (clk domain)
    output logic [7:0]  status_o,       // {ph2_alive, running, enabled, be_ok, res_n, rdy, 0, 0}
    output logic [31:0] cycle_count_o,
    output logic [15:0] stall_count_o,
    output logic [15:0] be_count_o,
    output logic [15:0] hold_mismatch_o,
    output logic [15:0] hold_samples_o,
    output logic [15:0] ph2_period_o,   // clk ticks per 256 PHI2 cycles
    output logic [15:0] ph2_high_o,     // clk ticks PHI2 high per 256 cycles
    output logic [23:0] last_addr_o     // {bank, address} of the last issued cycle
);

    //=========================================================================
    // Core + PHY
    //=========================================================================
    logic        cpu_clk, cpu_rst_n, cpu_we_n, cpu_vp_n;
    logic [7:0]  cpu_d_out;
    logic [23:0] cpu_a;
    logic        ph2_alive, running, enabled, be_ok, fall_evt, issue_evt, ended_read;
    logic [31:0] cycle_count;
    logic [15:0] stall_count, be_count;

    gs_socket_phy u_phy (
        .clk(clk), .rst_n(rst_n), .armed_i(arm_i), .listen_i(listen_i), .out_extra_i(out_extra_i),
        .cpu_clk_o(cpu_clk), .cpu_rst_n_o(cpu_rst_n),
        .cpu_a_i(cpu_a), .cpu_d_out_i(cpu_d_out), .cpu_we_n_i(cpu_we_n), .cpu_vp_n_i(cpu_vp_n),
        .gs_ph2_i(gs_ph2_i), .gs_rdy_i(gs_rdy_i), .gs_res_n_i(gs_res_n_i), .gs_be_i(gs_be_i),
        .gs_d_o(gs_d_o), .gs_d_oe_o(gs_d_oe_o),
        .gs_a_o(gs_a_o), .gs_rw_o(gs_rw_o), .gs_vp_o(gs_vp_o), .gs_rdy_out_o(gs_rdy_out_o),
        .gs_d_dir_o(gs_d_dir_o), .gs_data_oe_n_o(gs_data_oe_n_o), .gs_addr_oe_n_o(gs_addr_oe_n_o),
        .gs_ctl_oe_n_o(gs_ctl_oe_n_o),
        .ph2_alive_o(ph2_alive), .running_o(running), .enabled_o(enabled), .be_ok_o(be_ok),
        .fall_evt_o(fall_evt), .issue_evt_o(issue_evt), .ended_read_o(ended_read),
        .cycle_count_o(cycle_count), .stall_count_o(stall_count), .be_count_o(be_count)
    );

    // The core is clocked by PHI2 (inverted: active edge = the fall) and
    // samples the pads directly - see gs_socket_phy.
    P65C816 u_cpu (
        .CLK(cpu_clk), .RST_N(cpu_rst_n), .CE(1'b1), .RDY_IN(gs_rdy_i),
        .NMI_N(gs_nmi_n_i), .IRQ_N(gs_irq_n_i), .ABORT_N(gs_abort_n_i),
        .D_IN(gs_d_i), .D_OUT(cpu_d_out), .A_OUT(cpu_a), .WE(cpu_we_n), .RDY_OUT(),
        .VPA(), .VDA(), .MLB(), .VPB(cpu_vp_n), .I_FLAG()
    );

    //=========================================================================
    // Data-hold sweep: what the core took at the fall vs the pads N clks later
    //=========================================================================
    logic [7:0] d_fall = 8'h00;                 // PH2 domain: the byte at the fall
    always_ff @(negedge gs_ph2_i) d_fall <= gs_d_i;

    logic [4:0]  tap_cnt;
    logic        tap_arm;
    logic [15:0] hold_mismatch, hold_samples;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tap_cnt <= '0; tap_arm <= 1'b0;
            hold_mismatch <= '0; hold_samples <= '0;
        end else if (clear_i) begin
            tap_arm <= 1'b0;
            hold_mismatch <= '0; hold_samples <= '0;
        end else begin
            if (fall_evt && sweep_en_i && ended_read && be_ok) begin
                tap_arm <= 1'b1;
                tap_cnt <= hold_tap_i;
            end else if (tap_arm) begin
                if (tap_cnt == 5'd0) begin
                    tap_arm <= 1'b0;
                    if (hold_samples != 16'hFFFF) hold_samples <= hold_samples + 1'b1;
                    if (gs_d_i != d_fall && hold_mismatch != 16'hFFFF) hold_mismatch <= hold_mismatch + 1'b1;
                end else begin
                    tap_cnt <= tap_cnt - 1'b1;
                end
            end
        end
    end

    //=========================================================================
    // PHI2 period / duty: clk ticks per 256 cycles (period, high time)
    //=========================================================================
    logic [1:0]  ph2_s;
    logic [15:0] per_acc, hi_acc, per_out, hi_out;
    logic [7:0]  cyc256;

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ph2_s <= '0; per_acc <= '0; hi_acc <= '0; per_out <= '0; hi_out <= '0; cyc256 <= '0;
        end else begin
            ph2_s <= {ph2_s[0], gs_ph2_i};
            if (fall_evt) begin
                if (cyc256 == 8'hFF) begin
                    per_out <= per_acc + 1'b1;
                    hi_out  <= hi_acc + {15'd0, ph2_s[1]};
                    per_acc <= '0;
                    hi_acc  <= '0;
                end else begin
                    per_acc <= per_acc + 1'b1;
                    hi_acc  <= hi_acc + {15'd0, ph2_s[1]};
                end
                cyc256 <= cyc256 + 1'b1;
            end else begin
                per_acc <= per_acc + 1'b1;
                hi_acc  <= hi_acc + {15'd0, ph2_s[1]};
            end
        end
    end

    //=========================================================================
    // Last issued address, status
    //=========================================================================
    logic [23:0] last_addr;
    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n)          last_addr <= '0;
        else if (issue_evt)  last_addr <= cpu_a;
    end

    assign status_o        = {ph2_alive, running, enabled, be_ok, gs_res_n_i, gs_rdy_i, 2'b00};
    assign cycle_count_o   = cycle_count;
    assign stall_count_o   = stall_count;
    assign be_count_o      = be_count;
    assign hold_mismatch_o = hold_mismatch;
    assign hold_samples_o  = hold_samples;
    assign ph2_period_o    = per_out;
    assign ph2_high_o      = hi_out;
    assign last_addr_o     = last_addr;

endmodule
