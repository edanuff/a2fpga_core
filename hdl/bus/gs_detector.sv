//
// Apple IIgs runtime detector
//
// (c) 2023,2024,2025,2026 Ed Anuff <ed@a2fpga.com>
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
// Detects whether the host machine is an Apple IIgs by watching for
// read-modify-write access pairs to GS-only I/O registers on the slot bus.
//
// The IIgs ROM reset handler at $FA62 begins with TSB $C029 (NEWVIDEO),
// which the slot sees as a read of $C029 immediately followed by a write
// to $C029 — the first slot-visible CPU activity after /RES deasserts.
// Beyond reset, the monitor's GOTOBANKFF dispatcher (used by essentially
// all firmware console I/O) brackets every call with LDA $C036 / STA $C036
// (CYAREG speed save/restore), so a running GS emits a stream of $C036
// read/write pairs whenever firmware text output occurs.
//
// No IIe/II+ firmware or software generates a read of one of these
// registers followed within a few cycles by a write to the same address:
// on an 8-bit machine $C029/$C036/$C068 are unassigned locations that read
// floating bus, and the paired-access qualifier rejects both stray reads
// and blind writes. The comparator deliberately excludes $C100-$C7FF,
// which the slot ID boot scan touches on all machines.
//
// Detection is sticky until FPGA reset (the host machine cannot change
// while powered). Sampling follows the soft-switch capture idiom in
// apple_memory.sv: at phi1_posedge, addr/rw_n describe the bus cycle that
// just completed. The detector must see the bus unqualified by M2SEL
// (m2sel_n gating is itself enabled by the sw_gs this module helps derive),
// so it takes the raw latched address/rw lines from the bus module.
//

module gs_detector #(
    // Max bus cycles between the read and the write of an R-M-W pair.
    // On the GS both halves of TSB/LDA-STA sync to consecutive 1MHz
    // Mega II cycles, but allow slack for interleaved non-CPU cycles.
    parameter int PAIR_WINDOW = 4
) (
    input clk_i,
    input reset_n_i,

    input phi1_posedge_i,     // sample strobe: addr/rw reflect completed cycle
    input [15:0] addr_i,
    input rw_n_i,

    output reg gs_detected_o, // sticky: IIgs R-M-W signature seen
    output reg gs_event_o     // single-clock strobe per detected pair (debug)
);

    // GS-only register band: NEWVIDEO, CYAREG, STATEREG
    wire addr_is_gs_reg = (addr_i == 16'hC029) ||
                          (addr_i == 16'hC036) ||
                          (addr_i == 16'hC068);

    reg [15:0] pending_addr_r;
    reg [$clog2(PAIR_WINDOW+1)-1:0] window_r;

    always @(posedge clk_i or negedge reset_n_i) begin
        if (!reset_n_i) begin
            gs_detected_o <= 1'b0;
            gs_event_o <= 1'b0;
            pending_addr_r <= 16'h0000;
            window_r <= '0;
        end else begin
            gs_event_o <= 1'b0;
            if (phi1_posedge_i) begin
                if (rw_n_i && addr_is_gs_reg) begin
                    // read of a GS-only register arms (or re-arms) the window
                    pending_addr_r <= addr_i;
                    window_r <= PAIR_WINDOW[$clog2(PAIR_WINDOW+1)-1:0];
                end else begin
                    if (window_r != '0) begin
                        window_r <= window_r - 1'b1;
                        if (!rw_n_i && (addr_i == pending_addr_r)) begin
                            gs_detected_o <= 1'b1;
                            gs_event_o <= 1'b1;
                            window_r <= '0;
                        end
                    end
                end
            end
        end
    end

endmodule
