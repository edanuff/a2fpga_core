// Ensoniq DOC5503 Sound Engine
//
// (c) 2023,2024 Ed Anuff <ed@a2fpga.com> 
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
// The DOC5503 is a 32 voice polyphonic sound engine used in the Apple IIgs and
// a number of Ensoniq synthesizers. It is a digital sound engine that uses
// wavetable synthesis to produce sound.
//
// The DOC5503 consists of 32 time-multiplexed digital oscillators that are
// mixed together to produce the final output. Each oscillator has a 24-bit
// accumulator that is used to address a 256 to 32768 byte wavetable in the
// system's RAM. The accumulator is incremented by a frequency value that is
// loaded from a 16-bit frequency register. The output of the wavetable is
// scaled by a 8-bit volume register and then mixed with the outputs of the
// other oscillators.
//
// In the IIgs, the DOC5503 is clocked using the Apple II 7.15909 MHz clock
// rather than its intended 8 MHz clock. The 7.15909 MHz clock is derived from
// the 14.31818 MHz clock that is used to drive the video system. The DOC5503
// divdes this clock by 8 for every oscillator cycle, so the effective
// oscillator maximum frequency is 894886.25 Hz. This module is assumed to be
// clocked at at least 50 MHz on clk_i with a single clk pulse on clk_en_i
// on 7.15909 MHz clock timing.  Although the main DOC5503 FSM operates on the
// clk_en_i pulse, register memory and the oscillator outputs are updated on
// the rising edge of clk_i. At 50 MHz, there are aproximately 5 clk_i pulses
// in between each clk_en_i pulse that can be used to update the control
// registers before the next FSM state change.
//
// The core logic of the DOC5503 is not particularly complex, but the large
// number of oscillators and the control registers for each oscillator make
// for a complex memory structure that is difficult to implement as a typical
// register file. This module is designed to provide the memory access logic
// for the DOC5503 and to infer the memory as block RAMs or distributed RAMs
// during synthesis. 
//

module doc5503 #(
    parameter int CLOCK_SPEED_HZ = 54_000_000,
    parameter int DOC_CLOCK_SPEED_HZ = 7_159_090 // 7.15909 MHz
) (
    input clk_i,
    input reset_n_i,
    input clk_en_i,

    output reg irq_n_o,

    input cs_n_i,
    input we_n_i,

    input [7:0] addr_i,
    input [7:0] data_i,
    output reg [7:0] data_o,

    output reg [15:0] wave_address_o,
    output reg wave_rd_o,
    input wave_data_ready_i,
    input [7:0] wave_data_i,

    output signed [15:0] mono_mix_o,
    output signed [15:0] left_mix_o,
    output signed [15:0] right_mix_o,

    output signed [15:0] channel_o[16]

);

    // Registers

    reg [7:0] osc_int_r;            // $E0    : Oscillator Interrupt Register
    reg [7:0] osc_en_r;             // $E1    : Oscillator Enable Register

    // In order for these to be safely synthesized as ram, we need to never access them
    // multiple times in the same clock cycle

    reg [7:0] fl_r[32];             // $00-1F : Frequency Low Register
    reg [7:0] fh_r[32];             // $20-3F : Frequency High Register
    reg [7:0] vol_r[32];            // $40-5F : Volume Register
    reg [7:0] wds_r[32];            // $60-7F : Waveform Data Sample Register
    reg [7:0] wtp_r[32];            // $80-9F : Waveform Table Pointer Register
    reg [7:0] control_r[32];        // $A0-BF : Control Register
    reg [7:0] rts_r[32];            // $C0-DF : Resolution Table Size Register

    reg [23:0] acc_r[32];           // Oscillator Accumulator Register

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
    
    reg [5:0] curr_osc_r;
    wire curr_osc_odd_w = curr_osc_r[0];
    wire curr_osc_even_w = ~curr_osc_odd_w;
    wire [4:0] partner_osc_w = 5'(curr_osc_r^1);

    wire [2:0] curr_wts_w = curr_rts_r[5:3];
    wire [2:0] curr_res_w = curr_rts_r[2:0];
    wire [4:0] curr_shift_w = 5'd9 + curr_res_w - curr_wts_w;

    wire halt_w = curr_control_r[0];
    wire [1:0] curr_mode_w = curr_control_r[2:1];
    wire [3:0] curr_ca_w = curr_control_r[7:4];

    wire partner_halt_w = partner_control_r[0];
    wire [1:0] partner_mode_w = partner_control_r[2:1];

    wire next_halt_w = next_control_r[0];
    wire [1:0] next_mode_w = next_control_r[2:1];

    reg [23:0] curr_acc_r;
    reg [15:0] curr_wave_addr_r;
    reg [15:0] curr_output_r;

    // Incoming wave date requested by the FSM

    reg [4:0] last_osc_r;
    reg [7:0] last_wave_data_r;

    always_ff @(posedge clk_i) begin
        if (!reset_n_i) begin
            last_wave_data_r <= 8'h80;
        end else begin
            if (wave_data_ready_i) begin
                last_wave_data_r <= wave_data_i;
            end
        end
    end

    // Use 24-bit accumulators for mixing to provide more headroom and prevent clipping
    localparam int MIXER_SUM_RESOLUTION = 24;

    // Adjustable parameters for output scaling
    // TOP_BIT_OFFSET: How many bits from the top to start the window (0 = full amplitude)
    // WINDOW_SIZE: Number of bits to use for magnitude portion (should be 15 for 16-bit output with sign bit)
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
	localparam [1:0] MODE_ONESHOT = 2'b01;
    localparam [1:0] MODE_SYNC_AM = 2'b10;
    localparam [1:0] MODE_SWAP = 2'b11;

    // Use bits 5:1 to determine the number of oscillators enabled (divide by 2)
    wire [4:0] osc_max_w = osc_en_r[5:1];

    // Timing generator

    localparam int TICKS_PER_CYCLE = CLOCK_SPEED_HZ / (DOC_CLOCK_SPEED_HZ / 8);
    localparam int CYCLE_WIDTH = (TICKS_PER_CYCLE <= 1) ? 1 : $clog2(TICKS_PER_CYCLE + 1);
    localparam int TICKS_PER_PHASE = TICKS_PER_CYCLE / 2;
    localparam int PHASE_WIDTH = (TICKS_PER_PHASE <= 1) ? 1 : $clog2(TICKS_PER_PHASE + 1);
    logic [CYCLE_WIDTH-1:0] cycle_timer_r;
    logic [PHASE_WIDTH-1:0] phase_timer_w;
    logic cycle_start_w = (cycle_timer_r == '0);

    localparam [1:0] CYCLE_RESET = 2'b00;
    localparam [1:0] CYCLE_OSC = 2'b01;
    localparam [1:0] CYCLE_REFRESH_0 = 2'b10;
    localparam [1:0] CYCLE_REFRESH_1 = 2'b11;

    reg [1:0] cycle_state_r;

    reg [2:0] clk_count_r;
    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            clk_count_r <= '0;
            cycle_timer_r <= '0;
            phase_timer_w <= '0;
            cycle_state_r <= CYCLE_RESET;
            curr_osc_r <= '0;
        end else begin
            cycle_timer_r <= cycle_timer_r + 1'd1;
            phase_timer_w <= phase_timer_w + 1'd1;
            if (clk_en_i) begin
                clk_count_r <= clk_count_r + 1'd1;
                if (clk_count_r == 3'b111) begin
                    cycle_timer_r <= '0;

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
                if (clk_count_r[1:0] == 2'b11) begin
                    phase_timer_w <= '0;
                end
            end
        end
    end

    logic E;
    logic Q;
    always_comb begin
            E = clk_count_r[2];
            Q = clk_count_r[2] ^ clk_count_r[1];
    end

    localparam int OSC_STATE_COUNT = 11;
    typedef enum logic [$clog2(OSC_STATE_COUNT)-1:0] {
        OSC_IDLE,
        OSC_START,
        OSC_LOAD_PARTNER_CONTROL,
        OSC_ACC,
        OSC_ADDR,
        OSC_READ_DATA,
        OSC_AM_SYNC_EVEN,
        OSC_HALT,
        OSC_HALT_PARTNER,
        OSC_OUT,
        OSC_MIX
    } osc_state_e;
    osc_state_e osc_state_r; 
    
    reg data_pending_r;

    always_ff @(posedge clk_i) begin
        if (!reset_n_i) begin
            osc_state_r <= OSC_IDLE;
            wave_address_o <= '0;
            wave_rd_o <= '0;
            data_pending_r <= '0;
        end else begin
            wave_rd_o <= '0;
            if (wave_data_ready_i) begin
                data_pending_r <= 1'b1;
            end
            if (E & (cycle_state_r != CYCLE_RESET)) begin
                // Handle host access to the registers
                if (!cs_n_i) begin
                    if (addr_i == 8'hE0) begin
                        // Oscillator Interrupt Register
                        if (!we_n_i) begin
                            osc_int_r <= data_i;
                        end else begin
                            data_o <= osc_int_r;
                        end
                    end else if (addr_i == 8'hE1) begin
                        // Oscillator Enable Register
                        if (!we_n_i) begin
                            osc_en_r <= data_i;
                        end else begin
                            data_o <= osc_en_r;
                        end
                    end else if (addr_i >= 8'h00 && addr_i <= 8'hDF) begin
                        // Oscillator Registers
                        if (!we_n_i) begin
                            case (addr_i[7:5])
                                3'b000: fl_r[addr_i[4:0]] <= data_i;     // $00-1F
                                3'b001: fh_r[addr_i[4:0]] <= data_i;     // $20-3F
                                3'b010: vol_r[addr_i[4:0]] <= data_i;    // $40-5F
                                3'b011: wds_r[addr_i[4:0]] <= data_i;    // $60-7F
                                3'b100: wtp_r[addr_i[4:0]] <= data_i;    // $80-9F
                                3'b101: control_r[addr_i[4:0]] <= data_i; // $A0-BF
                                3'b110: rts_r[addr_i[4:0]] <= data_i;    // $C0-DF
                            endcase
                        end else begin
                            case (addr_i[7:5])
                                3'b000: data_o <= fl_r[addr_i[4:0]];      // $00-1F
                                3'b001: data_o <= fh_r[addr_i[4:0]];      // $20-3F
                                3'b010: data_o <= vol_r[addr_i[4:0]];     // $40-5F
                                3'b011: data_o <= wds_r[addr_i[4:0]];     // $60-7F
                                3'b100: data_o <= wtp_r[addr_i[4:0]];     // $80-9F
                                3'b101: data_o <= control_r[addr_i[4:0]]; // $A0-BF
                                3'b110: data_o <= rts_r[addr_i[4:0]];     // $C0-DF
                            endcase
                        end
                    end
                end

            end else begin
                // Handle the oscillator state machine
                case (cycle_state_r)
                    CYCLE_RESET: begin
                        fl_r[cycle_timer_r[4:0]] <= '0;
                        fh_r[cycle_timer_r[4:0]] <= '0;
                        vol_r[cycle_timer_r[4:0]] <= '0;
                        wds_r[cycle_timer_r[4:0]] <= '0;
                        wtp_r[cycle_timer_r[4:0]] <= '0;
                        rts_r[cycle_timer_r[4:0]] <= '0;
                        control_r[cycle_timer_r[4:0]] <= '0;
                        acc_r[cycle_timer_r[4:0]] <= '0;
                    end
                    CYCLE_OSC: begin
                        case (osc_state_r)
                            OSC_IDLE: begin
                                if (data_pending_r) begin
                                    wds_r[last_osc_r] <= last_wave_data_r;
                                    data_pending_r <= 1'b0;
                                end
                                if (cycle_start_w) begin
                                    osc_state_r <= OSC_START;
                                end
                            end
                            OSC_START: begin
                                curr_fl_r <= fl_r[curr_osc_r];
                                curr_fh_r <= fh_r[curr_osc_r];
                                curr_vol_r <= vol_r[curr_osc_r];
                                curr_wds_r <= wds_r[curr_osc_r];
                                curr_wtp_r <= wtp_r[curr_osc_r];
                                curr_control_r <= control_r[curr_osc_r];
                                curr_rts_r <= rts_r[curr_osc_r];
                                curr_acc_r <= acc_r[curr_osc_r];

                                curr_wave_addr_r <= '0;
                                curr_output_r <= '0;

                                osc_state_r <= OSC_LOAD_PARTNER_CONTROL;
                            end
                            OSC_LOAD_PARTNER_CONTROL: begin
                                partner_control_r <= control_r[partner_osc_w];
                                
                                osc_state_r <= OSC_ACC;
                            end
                            OSC_ACC: begin
                                next_control_r <= control_r[curr_osc_r + 1'b1];

                                // Accumulator 
                                if (!halt_w) begin
                                    curr_acc_r <= curr_acc_r + {curr_fh_r, curr_fl_r};
                                    osc_state_r <= OSC_ADDR;
                                end else begin
                                    if (curr_mode_w[0]) begin
                                        acc_r[curr_osc_r] <= '0;
                                    end
                                    osc_state_r <= OSC_IDLE;
                                end                                
                            end
                            OSC_ADDR: begin
                                acc_r[curr_osc_r] <= curr_acc_r; // Store the updated accumulator value

                                // Address Pointer (big barrel shifter)
                                curr_wave_addr_r <= 16'(curr_acc_r >> curr_shift_w);

                                osc_state_r <= OSC_READ_DATA;
                            end
                            OSC_READ_DATA: begin
                                // Read Waveform Data

                                // High bit depends on the table size
                                // For 32K tables (wts_w == 3'b111), use bit 15
                                automatic int high_bit_w = (curr_wts_w == 3'b111) ? 15 : {1'b1, curr_wts_w};
                                automatic logic overflow = curr_wave_addr_r[high_bit_w];
                                automatic logic zero_byte_w = (curr_wds_r == 8'h00);

                                if (overflow | zero_byte_w) begin
                                    acc_r[curr_osc_r] <= '0;                                        // reset accumulator
                                    if (zero_byte_w & curr_mode_w[0]) begin                              // only halt in one-shot mode or when explicit zero byte during run
                                        osc_state_r <= OSC_HALT;
                                    end else if (curr_mode_w == MODE_SYNC_AM && curr_osc_even_w) begin   // Sync AM Mode, even oscillator
                                        osc_state_r <= OSC_AM_SYNC_EVEN;
                                    end else begin
                                        osc_state_r <= OSC_OUT;
                                    end
                                end else begin
                                    // Read next byte from SDRAM
                                    automatic logic [7:0] ptr_hi_mask_w = 8'hFF << curr_wts_w;
                                    automatic logic [15:0] ptr_w = {ptr_hi_mask_w & curr_wtp_r, 8'b0};
                                    automatic logic [15:0] addr_w = curr_wave_addr_r | ptr_w;
                                    wave_rd_o <= 1'b1; 
                                    wave_address_o <= addr_w;
                                    last_osc_r <= curr_osc_r;
                                    osc_state_r <= OSC_OUT;
                                end
                            end
                            OSC_AM_SYNC_EVEN: begin
                                // Sync AM Mode, even oscillator
                                acc_r[partner_osc_w] <= '0;                                        // reset partner accumulator
                                osc_state_r <= OSC_OUT;
                            end
                            OSC_HALT: begin
                                // Halt Oscillator

                                curr_wds_r <= '0;                                                   // silence output

                                control_r[curr_osc_r] <= curr_control_r | 1'b1;                     // halt current oscillator
                                
                                if (curr_mode_w == MODE_SWAP) begin                                 // Swap Mode
                                    osc_state_r <= OSC_HALT_PARTNER;
                                end else begin
                                    osc_state_r <= OSC_IDLE;
                                end
                            end
                            OSC_HALT_PARTNER: begin
                                control_r[partner_osc_w] <= partner_control_r & 8'b11111110;
                                acc_r[partner_osc_w] <= '0;                                        // reset partner accumulator
                                osc_state_r <= OSC_IDLE;
                            end
                            OSC_OUT: begin
                                // Oscillator Output

                                if ((curr_mode_w == MODE_SYNC_AM) & curr_osc_odd_w) begin           // Sync AM Mode, odd oscillator outputs nothing
                                    if ((curr_osc_r < 30) & !next_halt_w) begin                     // if next oscillator is not halted
                                        vol_r[curr_osc_r + 1'b1] <= curr_wds_r;                     // it's volume is set to current oscillator's waveform data
                                    end
                                    osc_state_r <= OSC_IDLE;
                                end else begin
                                    automatic logic signed [7:0] data_w = curr_wds_r ^ 8'h80;       // convert waveform data to signed
                                    automatic logic signed [8:0] vol_s = {1'b0, curr_vol_r};        // convert volume to signed
                                    curr_output_r <= data_w * vol_s;                                // output is waveform data * volume (signed * signed)
                                    channel_sum_r <= next_channel_r[curr_ca_w];
                                    osc_state_r <= OSC_MIX;
                                end
                            end
                            OSC_MIX : begin
                                next_channel_r[curr_ca_w] <= channel_sum_r + curr_output_r;
                                next_mono_mix_r <= next_mono_mix_r + curr_output_r;
                                if (curr_ca_w[0]) next_left_mix_r <= next_left_mix_r + curr_output_r;
                                else next_right_mix_r <= next_right_mix_r + curr_output_r;
                                osc_state_r <= OSC_IDLE;
                            end

                        endcase // case (osc_state_r)

                    end
                    CYCLE_REFRESH_0: begin
                        // Use this cycle to copy the channel sums to the output registers
                        channel_r[cycle_timer_r[3:0]] <= next_channel_r[cycle_timer_r[3:0]];
                        mono_mix_r <= next_mono_mix_r;
                        left_mix_r <= next_left_mix_r;
                        right_mix_r <= next_right_mix_r;
                    end
                    CYCLE_REFRESH_1: begin
                        // Use this cycle to zero the channel sums for the next cycle
                        next_channel_r[cycle_timer_r[3:0]] <= '0;
                        next_mono_mix_r <= '0;
                        next_left_mix_r <= '0;
                        next_right_mix_r <= '0;
                    end
                endcase // case (cycle_state_r)
            end
        end // else: !if(!reset_n_i)

    end // always_ff
    
endmodule