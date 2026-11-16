// A2MEGA - IIgs CPU socket accelerator: when to run at the machine's pace
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
// The S7 accelerator's speed rules (docs/gs_socket_s7_accelerator_scoping.md
// 2.5; docs/gs_socket_s7_accel_only_build.md, section 3m).  Fast mode runs the
// code between two accesses to the machine at the card's rate, so code that
// counts cycles - a disk drive's read and write loops above all - must be run
// at the machine's pace instead: every step a socket cycle, timed by the
// machine's own PHI2, which the FPI already slows or not as its own rules say.
// slow_o asks for that (gs_accel: the broker's native mode, with the card's
// memory still the memory of record - what `fast` off has always been), while
//
//   * CYAREG ($C036) bit 7 is clear: the machine was asked for its normal
//     speed (the Control Panel's setting, or a program's), or
//   * the motor latch of slot 4, 5, 6 or 7 is set ($C0n9 on the bus since
//     $C0n8, n = 8 + slot) and CYAREG's motor-detect bit of that slot (bits
//     0 to 3) is set: the FPI's own rule for dropping to 1 MHz, or
//   * the motor latch of slot 6 is set and the 3.5 drive is selected
//     (DISKREG, $C031, bit 6): $C0E9 is then the enable of the IWM for a
//     drive the ROM serves at the machine's fast rate with loops timed for
//     it.  The ROM's own driver takes slot 6's detect bit out of CYAREG and
//     sets bit 7 before it selects the drive (ROM 01, FF/5DB3), so the FPI
//     does not slow for it and the first two rules do not see it.
//
// Slot 6's latch by itself is no rule: $C0E9 is whatever register 9 of a card
// in slot 6 is (the size register of this project's hard disk card, read in
// every ProDOS STATUS call), and the machine does not slow for it either.
//
// The latches follow the bus as the FPI's do: any cycle the socket shows at
// those addresses in banks 00, 01, E0 or E1, read or write - an I/O access is
// always a socket cycle, in every mode.  They clear at /RES.  With en_i low
// slow_o is low and nothing changes; the latches are kept in any case, so the
// rules can be switched on at any time.
//
// bus_evt_i comes a clk after the cycle was issued, with its address in a
// register (gs_accel's last_addr_o): nothing here hangs off the socket's
// request path.  slow_o is a register; the broker takes a change of mode at a
// step boundary, a step or two after the access that caused it - long before
// anything a drive does.
//
module gs_accel_rules (
    input  logic        clk,
    input  logic        rst_n,
    input  logic        cpu_rst_n_i,     // the core's reset (the machine's /RES)
    input  logic        en_i,            // follow the rules (quasi-static)
    input  logic        bus_evt_i,       // a cycle was issued on the socket a clk ago ...
    input  logic [23:0] bus_a_i,         // ... at this address
    input  logic [7:0]  cyareg_i,        // the replica's $C036
    input  logic        sel35_i,         // the replica's $C031 bit 6
    output logic [3:0]  motor_o,         // the motor latches of slots 4-7
    output logic        slow_o           // run at the machine's pace
);
    // $C0C8-$C0F9 with a low nibble of 8 or 9, in a bank where I/O is
    wire io_bank = (bus_a_i[23:17] == 7'h00) | (bus_a_i[23:17] == 7'h70);
    wire mot_hit = io_bank & (bus_a_i[15:6] == 10'b1100_0000_11) & (bus_a_i[3:1] == 3'b100);

    always_ff @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            motor_o <= '0; slow_o <= 1'b0;
        end else begin
            if (!cpu_rst_n_i)             motor_o <= '0;
            else if (bus_evt_i & mot_hit) motor_o[bus_a_i[5:4]] <= bus_a_i[0];
            slow_o <= en_i & (~cyareg_i[7] | (|(motor_o & cyareg_i[3:0])) | (motor_o[2] & sel35_i));
        end
    end
endmodule
