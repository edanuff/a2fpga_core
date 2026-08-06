// Bus Timing Profiler
//
// (c) 2026 Ed Anuff <ed@a2fpga.com>  — same license as apple_bus.sv
//
// Cross-machine bus-timing instrumentation for the a2n20v2-family boards.
//
// Motivation: on some hosts (notably ROM 00/01 Apple IIgs) the FPI's
// SYNC/STRETCH cycle shaping produces a phi0 edge stream that the FPGA's
// phi0-counted logic (Disk II nibble advance = 32 x phi0_posedge; W5100
// auto-increment) can mis-count, while static-register cards are unaffected.
// This block MEASURES the raw phi0 / cycle timing the FPGA actually sees so
// that identical builds on different machines yield comparable numbers.
//
// It watches the reconstructed phi0/phi1 edges (from apple_bus / a2bus_timing)
// at clk_logic (54 MHz, ~18.5 ns) and accumulates saturating statistics per
// Apple CPU cycle (one phi1_posedge -> next phi1_posedge window):
//   - cycle length in 54 MHz ticks (nominal ~52; SYNC/STRETCH cycles longer)
//   - phi0_posedge count within the cycle (MUST be exactly 1; !=1 is the bug)
//   - phi0 high-phase width in ticks
//   - the same, filtered to I/O ($C0xx) cycles (Disk II $C0Dx is a subset)
//   - per-cycle M2SEL-asserted / M2B0-set counts
//   - a coarse cycle-length histogram (runt / nominal / stretch1 / stretch2)
//
// NOTE ON WHAT IS OBSERVABLE: on the Enhanced board only PH1 and 7M are direct
// FPGA clock pins; M2SEL/M2B0 arrive as CPLD-latched per-cycle logical values
// (Q3 is tied off). So this measures phi0/cycle timing at full 54 MHz
// resolution, plus the LOGICAL M2SEL/M2B0 state per cycle — not their
// sub-cycle edge timing (that needs a scope or a CPLD change).
//
// Read-out: a single indexed byte port. stat_index_i selects a byte;
// stat_byte_o returns it (combinational). Multi-byte values are little-endian
// across consecutive indices. See the index map below (mirrored in telnetd.c).

module bus_timing_profiler (
    input  wire        clk_i,            // clk_logic (54 MHz)
    input  wire        rst_n_i,

    // reconstructed bus timing (from a2bus_if)
    input  wire        phi0_i,           // phi0 level (high = phase 2 / data)
    input  wire        phi0_posedge_i,   // rising edge of phi0 (read-window start)
    input  wire        phi1_posedge_i,   // rising edge of phi1 (cycle boundary)
    input  wire [15:0] addr_i,           // per-cycle latched address
    input  wire        rw_n_i,           // 1 = read
    input  wire        m2sel_n_i,        // 0 = valid IIgs slow access (SYNC)
    input  wire        m2b0_i,           // Mega II bank-0 (IIgs)

    // control (from the SPI connector, reg 0x6F)
    input  wire        clear_i,          // 1-cycle strobe: zero all stats
    input  wire        armed_i,          // level: accumulate while high

    // indexed read-out
    input  wire [6:0]  stat_index_i,
    output reg  [7:0]  stat_byte_o
);

    // ---- cycle-length histogram thresholds (54 MHz ticks; nominal ~52) ----
    localparam [15:0] TH_RUNT    = 16'd48;   // < 48        = runt / missed-edge short
    localparam [15:0] TH_NOM_HI  = 16'd55;   // 48..54      = nominal
    localparam [15:0] TH_S1_HI   = 16'd65;   // 55..64      = stretch-1
                                             // >= 65       = stretch-2+

    // ---- per-cycle live counters (reset each phi1_posedge) ----
    reg [15:0] tick_r;        // 54 MHz ticks since last cycle boundary
    reg [15:0] ph0high_r;     // ticks phi0 high this cycle
    reg [7:0]  ph0cnt_r;      // phi0_posedge count this cycle (saturating)

    // ---- accumulated statistics (all saturating) ----
    reg [31:0] total_cycles_r;
    reg [31:0] phi0_anom_r;    // cycles with ph0cnt != 1
    reg [15:0] clen_min_r, clen_max_r, clen_last_r;
    reg [15:0] ph0h_min_r, ph0h_max_r;
    reg [7:0]  last_ph0cnt_r;
    reg [31:0] io_cycles_r;    // addr[15:12]==C
    reg [31:0] io_phi0_anom_r;
    reg [31:0] m2sel_cnt_r;    // m2sel_n==0
    reg [31:0] m2b0_cnt_r;     // m2b0==1
    reg [31:0] hist_runt_r, hist_nom_r, hist_s1_r, hist_s2_r;
    reg        overflow_r;     // any 32-bit counter saturated

    wire sat32 = (total_cycles_r == 32'hFFFFFFFF);

    // saturating +1 helpers via function
    function [31:0] inc32(input [31:0] v);
        inc32 = (v == 32'hFFFFFFFF) ? v : v + 32'd1;
    endfunction

    always @(posedge clk_i or negedge rst_n_i) begin
        if (!rst_n_i) begin
            tick_r <= 16'd0; ph0high_r <= 16'd0; ph0cnt_r <= 8'd0;
            total_cycles_r <= 0; phi0_anom_r <= 0;
            clen_min_r <= 16'hFFFF; clen_max_r <= 0; clen_last_r <= 0;
            ph0h_min_r <= 16'hFFFF; ph0h_max_r <= 0;
            last_ph0cnt_r <= 0;
            io_cycles_r <= 0; io_phi0_anom_r <= 0;
            m2sel_cnt_r <= 0; m2b0_cnt_r <= 0;
            hist_runt_r <= 0; hist_nom_r <= 0; hist_s1_r <= 0; hist_s2_r <= 0;
            overflow_r <= 0;
        end else if (clear_i) begin
            tick_r <= 16'd0; ph0high_r <= 16'd0; ph0cnt_r <= 8'd0;
            total_cycles_r <= 0; phi0_anom_r <= 0;
            clen_min_r <= 16'hFFFF; clen_max_r <= 0; clen_last_r <= 0;
            ph0h_min_r <= 16'hFFFF; ph0h_max_r <= 0;
            last_ph0cnt_r <= 0;
            io_cycles_r <= 0; io_phi0_anom_r <= 0;
            m2sel_cnt_r <= 0; m2b0_cnt_r <= 0;
            hist_runt_r <= 0; hist_nom_r <= 0; hist_s1_r <= 0; hist_s2_r <= 0;
            overflow_r <= 0;
        end else begin
            // ---- live per-cycle accumulation ----
            if (tick_r != 16'hFFFF) tick_r <= tick_r + 16'd1;
            if (phi0_i && ph0high_r != 16'hFFFF) ph0high_r <= ph0high_r + 16'd1;
            if (phi0_posedge_i && ph0cnt_r != 8'hFF) ph0cnt_r <= ph0cnt_r + 8'd1;

            // ---- cycle boundary: fold the just-completed cycle into stats ----
            if (phi1_posedge_i) begin
                // restart per-cycle live counters; count the current tick.
                tick_r    <= 16'd1;
                ph0high_r <= (phi0_i ? 16'd1 : 16'd0);
                ph0cnt_r  <= (phi0_posedge_i ? 8'd1 : 8'd0);

                if (armed_i) begin
                    // total + last
                    total_cycles_r <= inc32(total_cycles_r);
                    clen_last_r    <= tick_r;
                    last_ph0cnt_r  <= ph0cnt_r;

                    // min/max cycle length
                    if (tick_r < clen_min_r) clen_min_r <= tick_r;
                    if (tick_r > clen_max_r) clen_max_r <= tick_r;
                    // min/max phi0 high
                    if (ph0high_r < ph0h_min_r) ph0h_min_r <= ph0high_r;
                    if (ph0high_r > ph0h_max_r) ph0h_max_r <= ph0high_r;

                    // phi0 anomaly (should be exactly 1 edge per cycle)
                    if (ph0cnt_r != 8'd1) phi0_anom_r <= inc32(phi0_anom_r);

                    // cycle-length histogram
                    if      (tick_r <  TH_RUNT)   hist_runt_r <= inc32(hist_runt_r);
                    else if (tick_r <  TH_NOM_HI) hist_nom_r  <= inc32(hist_nom_r);
                    else if (tick_r <  TH_S1_HI)  hist_s1_r   <= inc32(hist_s1_r);
                    else                          hist_s2_r   <= inc32(hist_s2_r);

                    // I/O ($C0xx) subset — Disk II $C0Dx reads live here
                    if (addr_i[15:12] == 4'hC) begin
                        io_cycles_r <= inc32(io_cycles_r);
                        if (ph0cnt_r != 8'd1) io_phi0_anom_r <= inc32(io_phi0_anom_r);
                    end

                    // per-cycle M2SEL / M2B0 logical state
                    if (!m2sel_n_i) m2sel_cnt_r <= inc32(m2sel_cnt_r);
                    if (m2b0_i)     m2b0_cnt_r  <= inc32(m2b0_cnt_r);

                    if (sat32) overflow_r <= 1'b1;
                end
            end
        end
    end

    // ---- indexed read-out mux (combinational) ----
    // Index map (little-endian multibyte). Mirror in telnetd.c 'm' command.
    always @(*) begin
        case (stat_index_i)
            7'h00: stat_byte_o = {6'b0, overflow_r, armed_i};        // STATUS
            7'h01: stat_byte_o = total_cycles_r[7:0];
            7'h02: stat_byte_o = total_cycles_r[15:8];
            7'h03: stat_byte_o = total_cycles_r[23:16];
            7'h04: stat_byte_o = total_cycles_r[31:24];
            7'h05: stat_byte_o = phi0_anom_r[7:0];
            7'h06: stat_byte_o = phi0_anom_r[15:8];
            7'h07: stat_byte_o = phi0_anom_r[23:16];
            7'h08: stat_byte_o = phi0_anom_r[31:24];
            7'h09: stat_byte_o = clen_min_r[7:0];
            7'h0A: stat_byte_o = clen_min_r[15:8];
            7'h0B: stat_byte_o = clen_max_r[7:0];
            7'h0C: stat_byte_o = clen_max_r[15:8];
            7'h0D: stat_byte_o = ph0h_min_r[7:0];
            7'h0E: stat_byte_o = ph0h_min_r[15:8];
            7'h0F: stat_byte_o = ph0h_max_r[7:0];
            7'h10: stat_byte_o = ph0h_max_r[15:8];
            7'h11: stat_byte_o = io_cycles_r[7:0];
            7'h12: stat_byte_o = io_cycles_r[15:8];
            7'h13: stat_byte_o = io_cycles_r[23:16];
            7'h14: stat_byte_o = io_cycles_r[31:24];
            7'h15: stat_byte_o = io_phi0_anom_r[7:0];
            7'h16: stat_byte_o = io_phi0_anom_r[15:8];
            7'h17: stat_byte_o = io_phi0_anom_r[23:16];
            7'h18: stat_byte_o = io_phi0_anom_r[31:24];
            7'h19: stat_byte_o = m2sel_cnt_r[7:0];
            7'h1A: stat_byte_o = m2sel_cnt_r[15:8];
            7'h1B: stat_byte_o = m2sel_cnt_r[23:16];
            7'h1C: stat_byte_o = m2sel_cnt_r[31:24];
            7'h1D: stat_byte_o = m2b0_cnt_r[7:0];
            7'h1E: stat_byte_o = m2b0_cnt_r[15:8];
            7'h1F: stat_byte_o = m2b0_cnt_r[23:16];
            7'h20: stat_byte_o = m2b0_cnt_r[31:24];
            7'h21: stat_byte_o = last_ph0cnt_r;
            7'h22: stat_byte_o = clen_last_r[7:0];
            7'h23: stat_byte_o = clen_last_r[15:8];
            7'h24: stat_byte_o = hist_runt_r[7:0];
            7'h25: stat_byte_o = hist_runt_r[15:8];
            7'h26: stat_byte_o = hist_runt_r[23:16];
            7'h27: stat_byte_o = hist_runt_r[31:24];
            7'h28: stat_byte_o = hist_nom_r[7:0];
            7'h29: stat_byte_o = hist_nom_r[15:8];
            7'h2A: stat_byte_o = hist_nom_r[23:16];
            7'h2B: stat_byte_o = hist_nom_r[31:24];
            7'h2C: stat_byte_o = hist_s1_r[7:0];
            7'h2D: stat_byte_o = hist_s1_r[15:8];
            7'h2E: stat_byte_o = hist_s1_r[23:16];
            7'h2F: stat_byte_o = hist_s1_r[31:24];
            7'h30: stat_byte_o = hist_s2_r[7:0];
            7'h31: stat_byte_o = hist_s2_r[15:8];
            7'h32: stat_byte_o = hist_s2_r[23:16];
            7'h33: stat_byte_o = hist_s2_r[31:24];
            default: stat_byte_o = 8'h00;
        endcase
    end

endmodule
