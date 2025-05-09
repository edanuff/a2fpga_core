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

    output signed [15:0] channel_o[16],
    
    output logic ready_o,           // Indicate when out of reset and ready to process
    output [2:0] clk_count_o        // Expose the full clk_count for debugging
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
    reg [7:0] prev_control_r;

    wire zero_byte_w = (curr_wds_r == 8'h00);
    
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

    wire prev_halt_w = prev_control_r[0];
    wire [1:0] prev_mode_w = prev_control_r[2:1];

   reg [23:0] curr_acc_r;
    reg [15:0] curr_wave_addr_r;
    reg [15:0] curr_output_r;

    // Incoming wave date requested by the FSM

    reg [7:0] loaded_wds_r;;

    always_ff @(posedge clk_i) begin
        if (!reset_n_i) begin
            loaded_wds_r; <= 8'h80;
        end else begin
            if (wave_data_ready_i) begin
                loaded_wds_r; <= wave_data_i;
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
    logic [CYCLE_WIDTH-1:0] cycle_timer_r;
    logic cycle_start_w = (cycle_timer_r == '0);  // Start new oscillator cycle when timer is zero

    localparam [1:0] CYCLE_RESET = 2'b00;
    localparam [1:0] CYCLE_OSC = 2'b01;
    localparam [1:0] CYCLE_REFRESH_0 = 2'b10;
    localparam [1:0] CYCLE_REFRESH_1 = 2'b11;

    reg [1:0] cycle_state_r;
    wire ready_w = (cycle_state_r != CYCLE_RESET);
    assign ready_o = ready_w;

    reg [2:0] clk_count_r;
    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            clk_count_r <= '0;
            cycle_timer_r <= '0;
            cycle_state_r <= CYCLE_RESET;
            curr_osc_r <= '0;
        end else begin
            cycle_timer_r <= cycle_timer_r + 1'd1;
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
            end
        end
    end

   localparam int OSC_STATE_COUNT = 11;
    typedef enum logic [$clog2(OSC_STATE_COUNT)-1:0] {
        OSC_IDLE = 0,
        OSC_START = 1,
        OSC_LOAD_PARTNER_CONTROL = 2,
        OSC_ACC = 3,
        OSC_ADDR = 4,
        OSC_READ_DATA = 5,
        OSC_AM_SYNC_EVEN = 6,
        OSC_HALT = 7,
        OSC_HALT_PARTNER = 8,
        OSC_OUT = 9,
        OSC_MIX = 10
    } osc_state_e;
    osc_state_e osc_state_r; 
    
    reg loaded_wds_pending_r;

    reg host_access_r = 1'b0;

    reg halt_zero_r = 1'b0;
    reg halt_acc_overflow_r = 1'b0;

    always_ff @(posedge clk_i) begin
        if (!reset_n_i) begin
            osc_state_r <= OSC_IDLE;
            wave_address_o <= '0;
            wave_rd_o <= '0;
            loaded_wds_pending_r <= '0;
            host_access_r <= 1'b0;
            halt_zero_r <= 1'b0;
            halt_acc_overflow_r <= 1'b0;
        end else begin
            host_access_r <= ~host_access_r;
            wave_rd_o <= '0;
            if (wave_data_ready_i) begin
                loaded_wds_pending_r <= 1'b1;
            end
            if (host_access_r & ready_w) begin
                host_access();
            end else begin
                // Handle the oscillator state machine
                case (cycle_state_r)
                    CYCLE_RESET: cycle_reset();
                    CYCLE_OSC: cycle_osc();
                    CYCLE_REFRESH_0: cycle_refresh0();
                    CYCLE_REFRESH_1: cycle_refresh1();
                endcase // case (cycle_state_r)
            end
        end // else: !if(!reset_n_i)

    end // always_ff

    task host_access();
        // Handle host access to the registers
        if (!cs_n_i) begin
            if (addr_i == 8'hE0) begin
                // Oscillator Interrupt Register
                if (!we_n_i) osc_int_r <= data_i; 
                else data_o <= osc_int_r;
            end else if (addr_i == 8'hE1) begin
                // Oscillator Enable Register
                if (!we_n_i) osc_en_r <= data_i;
                else data_o <= osc_en_r;
            end else if (addr_i >= 8'h00 && addr_i <= 8'hDF) begin
                // Oscillator Registers
                if (!we_n_i) begin
                    case (addr_i[7:5])
                        3'b000: fl_r[addr_i[4:0]] <= data_i;        // $00-1F
                        3'b001: fh_r[addr_i[4:0]] <= data_i;        // $20-3F
                        3'b010: vol_r[addr_i[4:0]] <= data_i;       // $40-5F
                        3'b011: wds_r[addr_i[4:0]] <= data_i;       // $60-7F
                        3'b100: wtp_r[addr_i[4:0]] <= data_i;       // $80-9F
                        3'b101: control_r[addr_i[4:0]] <= data_i;   // $A0-BF
                        3'b110: rts_r[addr_i[4:0]] <= data_i;       // $C0-DF
                    endcase
                end else begin
                    case (addr_i[7:5])
                        3'b000: data_o <= fl_r[addr_i[4:0]];        // $00-1F
                        3'b001: data_o <= fh_r[addr_i[4:0]];        // $20-3F
                        3'b010: data_o <= vol_r[addr_i[4:0]];       // $40-5F
                        3'b011: data_o <= wds_r[addr_i[4:0]];       // $60-7F
                        3'b100: data_o <= wtp_r[addr_i[4:0]];       // $80-9F
                        3'b101: data_o <= control_r[addr_i[4:0]];   // $A0-BF
                        3'b110: data_o <= rts_r[addr_i[4:0]];       // $C0-DF
                    endcase
                end
            end
        end
    endtask: host_access

    task cycle_reset();
        fl_r[cycle_timer_r[4:0]] <= '0;
        fh_r[cycle_timer_r[4:0]] <= '0;
        vol_r[cycle_timer_r[4:0]] <= '0;
        wds_r[cycle_timer_r[4:0]] <= '0;
        wtp_r[cycle_timer_r[4:0]] <= '0;
        rts_r[cycle_timer_r[4:0]] <= '0;
        control_r[cycle_timer_r[4:0]] <= '0; // Ensure oscillators start not halted
        acc_r[cycle_timer_r[4:0]] <= '0;
    endtask: cycle_reset

    task cycle_refresh0();
        // Use this cycle to copy the channel sums to the output registers
        channel_r[cycle_timer_r[3:0]] <= next_channel_r[cycle_timer_r[3:0]];
        
        // Only copy the final values when processing the last channel (15)
        if (cycle_timer_r[3:0] == 4'hF) begin
            mono_mix_r <= next_mono_mix_r;
            left_mix_r <= next_left_mix_r;
            right_mix_r <= next_right_mix_r;
            // Debug: Print the final mix values
        end
    endtask: cycle_refresh0

    task cycle_refresh1();
        // Use this cycle to zero the channel sums for the next cycle
        next_channel_r[cycle_timer_r[3:0]] <= '0;
        
        // Only reset the mix values when processing the last channel (15)
        if (cycle_timer_r[3:0] == 4'hF) begin
            next_mono_mix_r <= '0;
            next_left_mix_r <= '0;
            next_right_mix_r <= '0;
        end
    endtask: cycle_refresh1

    task cycle_osc();
        case (osc_state_r)
            OSC_IDLE: osc_idle();
            OSC_REQUEST_DATA: osc_request_data();
            OSC_HANDLE_DATA: osc_handle_data();
            OSC_OUT: osc_out();
            OSC_MIX : osc_mix();
            OSC_ACC : osc_acc();
            OSC_HALT: osc_halt();
            OSC_HALT2: osc_halt2();
            OSC_HALT_PARTNER: osc_halt_partner();
            OSC_AM_SYNC_EVEN: osc_am_sync_even();
        endcase // case (osc_state_r)
    endtask: cycle_osc
    
    task osc_idle();
        if (cycle_start_w) begin

            // Load all current oscillator registers
            curr_fl_r <= fl_r[curr_osc_r];                  // Frequency low
            curr_fh_r <= fh_r[curr_osc_r];                  // Frequency high
            curr_vol_r <= vol_r[curr_osc_r];                // Volume
            curr_wds_r <= wds_r[curr_osc_r];                // Waveform data sample
            curr_wtp_r <= wtp_r[curr_osc_r];                // Waveform table pointer
            curr_control_r <= control_r[curr_osc_r];        // Control
            curr_rts_r <= rts_r[curr_osc_r];                // Resolution/table size
            curr_acc_r <= acc_r[curr_osc_r];                // Accumulator
            
            // Init other working values
            curr_wave_addr_r <= '0;
            curr_output_r <= '0;
            halt_zero_r <= 1'b0;
            halt_acc_overflow_r <= 1'b0;

            osc_state_r <= OSC_REQUEST_DATA;
        end
    endtask : osc_idle

    task osc_request_data();
        // Request data from SDRAM if not halted
        // If halted, clear accumulator if in one-shot mode and skip to OSC_IDLE
        // Needs the following registers to be loaded:
        //   curr_control_r, curr_acc_r, curr_rts_r, curr_wtp_r
        // Accesses or modifies the following oscillator registers:
        //   acc_r[curr_osc_r], control_r[partner_osc_w]
        // Modifies the following registers:
        //   partner_control_r, wave_address_o, wave_rd_o

        if (!halt_w) begin
            // Address Output Multiplexer - create wave table address from accumulator
            automatic logic [15:0] curr_wave_addr_w <= 16'(curr_acc_r >> curr_shift_w);
            // Create mask for wave table pointer based on wave table size
            automatic logic [7:0] ptr_hi_mask_w = 8'hFF << curr_wts_w;
            // Create pointer to wave table with ignored bits masked out
            automatic logic [15:0] ptr_w = {ptr_hi_mask_w & curr_wtp_r, 8'b0};
            // 
            automatic logic [15:0] addr_w = curr_wave_addr_w | ptr_w;

            // Read next byte from SDRAM
            wave_rd_o <= 1'b1; 
            wave_address_o <= addr_w;
                                                
            osc_state_r <= OSC_HANDLE_DATA;


        end else begin
            // When halted but in one-shot mode, clear accumulator
            if (curr_mode_w[0]) begin
                acc_r[curr_osc_r] <= '0;
            end
            // When halted, skip OUT and return to IDLE state
            osc_state_r <= OSC_IDLE;
        end                                

        // load partner control register (needed later)
        partner_control_r <= control_r[partner_osc_w];
    endtask: osc_request_data

    task osc_handle_data();
        // Handle the data returned from SDRAM
        // If the data is zero, set the halt flag and skip to OSC_IDLE
        // Otherwise proceed to OSC_OUT
        // Needs the following registers to be loaded:
        //   --
        // Accesses or modifies the following oscillator registers:
        //   control_r[curr_osc_r + 1], wds_r[curr_osc_r]
        // Modifies the following registers:
        //   curr_wds_r, halt_zero_r

        if (loaded_wds_pending_r) begin
            loaded_wds_pending_r <= 1'b0;
            wds_r[curr_osc_r] <= loaded_wds_r;;
            curr_wds_r <= loaded_wds_r;
            if (loaded_wds_r; == 8'h00) begin
                halt_zero_r <= 1'b1;                                    // Set halt zero flag
                osc_state_r <= OSC_HALT;
            end
            else osc_state_r <= OSC_OUT;
        end

        // load next control register (needed later)
        next_control_r <= control_r[curr_osc_r + 1'b1];
    endtask: osc_handle_data

    task osc_out();
        // Oscillator Output
        // If in SYNC_AM mode, odd oscillator outputs nothing
        // but it's value is used to set the volume of the next
        // even oscillator
        // Needs the following registers to be loaded:
        //   curr_wds_r, curr_vol_r, curr_control_r
        // Accesses or modifies the following oscillator registers:
        //   vol_r[curr_osc_r + 1], next_channel_r[curr_ca_w]
        // Modifies the following registers:
        //   curr_output_r, channel_sum_r, next_channel_r[curr_ca_w] 

        if ((curr_mode_w == MODE_SYNC_AM) & curr_osc_odd_w) begin           // Sync AM Mode, odd oscillator outputs nothing
            if ((curr_osc_r < 30) & !next_halt_w) begin                     // if next oscillator is not halted
                vol_r[curr_osc_r + 1'b1] <= curr_wds_r;                     // it's volume is set to current oscillator's waveform data
            end
            // Skip mixing for odd oscillators in SYNC_AM mode
            osc_state_r <= OSC_ACC;
        end else begin
            // Fixed waveform data polarity and volume calculation
            automatic logic signed [7:0] data_w = curr_wds_r ^ 8'h80;       // convert waveform data to signed (8'h80 = 0)
            automatic logic signed [8:0] vol_s = {1'b0, curr_vol_r};        // convert volume to signed (unsigned expanded)
            automatic logic signed [15:0] output_w = data_w * vol_s;        // output is waveform data * volume (signed * signed)
            curr_output_r <= output_w;                                      // store calculated output
            channel_sum_r <= next_channel_r[curr_ca_w];                     // load channel accumulator
            
            // Always go to OSC_MIX from here except in the special case above
            osc_state_r <= OSC_MIX;
        end
    endtask: osc_out

    task osc_mix();
        // Add current oscillator output to channel accumulator
        next_channel_r[curr_ca_w] <= channel_sum_r + curr_output_r;
        
        // Add to mono mix (all channels)
        next_mono_mix_r <= next_mono_mix_r + curr_output_r;
        
        // Add to left/right mix based on channel number
        // Channel 0,2,4,etc. go to right, 1,3,5,etc. go to left
        if (curr_ca_w[0]) 
            next_left_mix_r <= next_left_mix_r + curr_output_r;
        else 
            next_right_mix_r <= next_right_mix_r + curr_output_r;
        
        osc_state_r <= OSC_ACC;
    endtask: osc_mix

    task osc_acc();
        // Accumulator
        automatic logic [24:0] temp_acc <= curr_acc_r + {curr_fh_r, curr_fl_r};
        automatic logic [23:0] curr_acc_mask_w = {16'((1 << (5'd9 + curr_res_w)) - 1), 8'hFF};
        automatic int high_bit_w = 17 + curr_res_w 
        automatic logic overflow = temp_acc[high_bit_w];
        curr_acc_overflow_r <= overflow;
        curr_acc_r <= curr_wave_addr_w[23:0] & curr_acc_mask_w;             // wrap around address

        osc_state_r <= OSC_IDLE;
        if (overflow) begin
            curr_acc_overflow_r <= 1'b1;                                    // Set overflow flag
            osc_state <= OSC_HALT;
        end else begin
            // If not in one-shot mode, just set the accumulator
            acc_r[curr_osc_r] <= curr_acc_r;
        end
    endtask: osc_acc

    task osc_halt();
        // Halt Oscillator
        if (mode == MODE_SYNC_AM) begin
            if (curr_osc_even_w) begin
                // we're even, so if the odd oscillator 1 below us is playing,
                // restart it.
                if (!partner_halt_w) begin
                    acc_r[partner_osc_w] <= '0;                                    // reset partner accumulator
                end
            end
        end
    endtask: osc_halt

    task osc_halt2();
        // MODE_ONESHOT, MODE_SWAP, or zero byte
        if (curr_mode_w[0] || halt_zero_r) begin
            control_r[curr_osc_r] <= curr_control_r | 1'b1;                     // halt current oscillator
        end

        if (curr_mode_w == MODE_SWAP) begin                                 // Swap Mode
            osc_state_r <= OSC_START_PARTNER;
        end else begin
            osc_state_r <= OSC_PARTNER_SWAP;
        end
    endtask: osc_halt2

    task osc_halt_partner();
        control_r[partner_osc_w] <= partner_control_r & 8'b11111110;
        acc_r[partner_osc_w] <= '0;                                        // reset partner accumulator
        // After halting partner, skip the current output
        osc_state_r <= OSC_IDLE;
    endtask: osc_halt_partner

    task osc_am_sync_even();
        // Sync AM Mode, even oscillator
        acc_r[partner_osc_w] <= '0;                                        // reset partner accumulator
        osc_state_r <= OSC_OUT;
    endtask: osc_am_sync_even

endmodule
