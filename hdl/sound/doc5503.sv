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
// during synthesis. This is implemented in the companion doc5503_register_ram
// module. The use of the doc5503_register_ram module for the control registers
// greatly simplifies the logic in the main FSM since the control registers
// can be easily read and modified asynchronously to the FSM's state changes
// on clk_en_i pulses.
//

module doc5503 #(
    parameter int OUTPUT_CHANNEL_MIX = 1,
    parameter int NUM_CHANNELS = 16,
    parameter int OUTPUT_MONO_MIX = 1,
    parameter int OUTPUT_STEREO_MIX = 1,
    parameter int OUTPUT_OSC_DIRECT = 1, // Output raw oscillator data
    parameter int DIRECT_OUTPUT_OSC_NUM = 0, // # of Oscillator to direct to audio output
    parameter int CHANNEL_MAX = OUTPUT_CHANNEL_MIX && (NUM_CHANNELS > 1) ? NUM_CHANNELS - 1 : 1
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

    output signed [15:0] channel_o[CHANNEL_MAX:0],
    
    output [7:0] osc_en_o   // Debug output for oscillator enable register

);

    reg [7:0] osc_int_r;            // Oscillator Interrupt Register
    reg [7:0] osc_en_r;             // Oscillator Enable Register
    
    // Expose the oscillator enable register for debugging
    assign osc_en_o = osc_en_r;

    wire [15:0] f_w;
    wire [7:0] vol_w;
    wire [7:0] wds_w;
    wire [7:0] wtp_w;
    wire [7:0] rts_w;
    wire [7:0] control_w;
    wire [7:0] partner_control_w;
    wire [7:0] next_control_w;

    wire [7:0] fl_o_w;
    wire [7:0] fh_o_w;
    wire [7:0] vol_o_w;
    wire [7:0] wds_o_w;
    wire [7:0] wtp_o_w;
    wire [7:0] control_o_w;
    wire [7:0] rts_o_w;

    assign data_o = addr_i[7:5] == 3'b000 ? fl_o_w :
        addr_i[7:5] == 3'b001 ? fh_o_w :
        addr_i[7:5] == 3'b010 ? vol_o_w :
        addr_i[7:5] == 3'b011 ? wds_o_w :
        addr_i[7:5] == 3'b100 ? wtp_o_w :
        addr_i[7:5] == 3'b101 ? control_o_w :
        addr_i[7:5] == 3'b110 ? rts_o_w :
        addr_i == 8'hE0 ? osc_int_r :
        addr_i == 8'hE1 ? osc_en_r :
        8'b0;

    wire odd_osc_w = cycle_r[0];
    wire even_osc_w = ~odd_osc_w;
    wire [4:0] partner_w = 5'(cycle_r^1);

    wire [2:0] wts_w = rts_w[5:3];
    wire [2:0] res_w = rts_w[2:0];

    wire halt_w = control_w[0];
    wire [1:0] mode_w = control_w[2:1];

    wire partner_halt_w = partner_control_w[0];
    wire [1:0] partner_mode_w = partner_control_w[2:1];

    wire next_halt_w = next_control_w[0];
    wire [1:0] next_mode_w = next_control_w[2:1];

    wire [23:0] acc_w;
    reg [15:0] wave_addr_r;

    localparam [1:0] MODE_FREE = 2'b00;
	localparam [1:0] MODE_ONESHOT = 2'b01;
    localparam [1:0] MODE_SYNC_AM = 2'b10;
    localparam [1:0] MODE_SWAP = 2'b11;

    reg [4:0] cycle_r;
    reg [4:0] cycle_step_r;
    wire [4:0] osc_en_num_w = osc_en_r[5:1];
    //wire [4:0] cycle_max_w = osc_en_num_w == 0 ? 0 : 5'(osc_en_num_w - 1'b1);
    wire [4:0] cycle_max_w = osc_en_r[5:1];

    reg [5:0] mixer_cycle_r;
    wire [7:0] mixer_control_w;
    wire signed [15:0] mixer_output_w;

    reg [4:0] last_osc_r;

    reg ready_r;
    reg [4:0] init_cycle_r;

    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            ready_r <= 1'b0;
            init_cycle_r <= '0;
        end else begin
            if (!ready_r) begin
                init_cycle_r <= init_cycle_r + 1'd1;
                if (init_cycle_r == 5'd31) begin
                    ready_r <= 1'b1;
                end
            end
        end
    end

    // cycles are 8 steps long except for last cycle is 24 steps long, corresponds to RAM refresh
    wire last_cycle_w = cycle_r == cycle_max_w;
    wire last_cycle_step_w = last_cycle_w ? (cycle_step_r == 5'd23) : (cycle_step_r == 5'd7);

    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            cycle_r <= '0;
            cycle_step_r <= '0;
        end else begin
           if (ready_r & clk_en_i) begin
                cycle_step_r <= last_cycle_step_w ? '0 : cycle_step_r + 1'd1;
                cycle_r <= last_cycle_step_w ? (last_cycle_w ? '0 : cycle_r + 1'd1) : cycle_r;
           end
        end
    end

    reg inhibit_host_writes_r;

    // FREQUENCY CONTROL LOW AND HIGH ($OO·$lF, $20·$3F)

    // This group of registers form, in part the actual frequency in which
    // the DOC steps through the waveform table. The FREQUENCY LOW and
    // the FREQUENCY HIGH registers are concatenated to form a 16-bit
    // incremental value for the 24 bit linear accumulator. Each time the
    // oscillator is updated the value of the high/low FREQUENCY CONTROL
    // REGISTER is added to the current value stored in the accumulator.
    // The following equations can be used to determine the final output
    // frequency.
    // SR = CLK / ((OSC + 2) • 8)
    // Fundamental Output Frequency = [ SR / 2^(17 + RES)] • FC
    // Where: SR = sample rate, CLK = input clock, OSC = enabled
    // oscillators, RES = Resolution register, FC is the Frequency high
    // and Frequency Low registers concatenated, and the waveform
    // fundamental frequency length is equal to one page of memory.
    //
    // For values stored in the Frequency Control registers, the
    // effective output sample rate can be calculated as follows
    // freqOffset = ((32 x Output sample rate in hertz)/1645) 


    // Oscillator Frequency Lo

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(1),
        .PRIORITY_READ_PORTS(1)
    ) osc_fl_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b000)
        }),
        .priority_write_addr_i('{
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b000)
        }),
        .priority_read_addr_i('{
            addr_i[4:0]
        }),
        .priority_read_data_o('{
            fl_o_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(f_w[7:0]),
        .addr_b_i('0),
        .data_b_o()
    );

    // Oscillator Frequency Hi

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(1),
        .PRIORITY_READ_PORTS(1)
    ) osc_fh_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b001)
        }),
        .priority_write_addr_i('{
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b001)
        }),
        .priority_read_addr_i('{
            addr_i[4:0]
        }),
        .priority_read_data_o('{
            fh_o_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(f_w[15:8]),
        .addr_b_i('0),
        .data_b_o()
    );

    // Oscillator Volume

    // This set of registers is used to control the
    // volume level of the waveform data. The current value read by the
    // associated oscillator from the waveform table is multiplied by the
    // eight bit volume register to obtain the final output level for the
    // oscillator. It should be noted that volume register is ignored when
    // the associated oscillator is running in self enveloping mode.

    reg next_am_req_r;

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(2),
        .PRIORITY_READ_PORTS(1),
        .LAST_PRIORITY_WRITE_PORT_INHIBITABLE(1)
    ) osc_vol_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            next_am_req_r,
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b010)
        }),
        .priority_write_addr_i('{
            cycle_r + 1'b1,
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            wds_w,
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b010)
        }),
        .priority_read_addr_i('{
            addr_i[4:0]
        }),
        .priority_read_data_o('{
            vol_o_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(vol_w),
        .addr_b_i('0),
        .data_b_o()
    );

    // Oscillator Waveform Data Sample

    // This set of read only registers contain
    // last value read from the waveform table for the corresponding
    // oscillator.
    //
    // The waveform data format is an 8 bit unsigned with 0x80 being zero level
    // and 0x01 and 0xFF being the lowest and highest levels
    // respectively. A value of 0x00 is reserved for the end of the
    // waveform table and will cause the oscillator to halt.

    reg current_wds_reset_req_r;

    doc5503_register_ram #(
        .FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED(1),
        .PRIORITY_WRITE_PORTS(4),
        .PRIORITY_READ_PORTS(1),
        .LAST_PRIORITY_WRITE_PORT_INHIBITABLE(1)
    ) osc_wds_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !ready_r,
            current_wds_reset_req_r,
            wave_data_ready_i,
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b011)
        }),
        .priority_write_addr_i('{
            init_cycle_r,
            cycle_r,
            last_osc_r,
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            8'h80,
            8'h80,
            wave_data_i,
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b011)
        }),
        .priority_read_addr_i('{
            addr_i[4:0]
        }),
        .priority_read_data_o('{
            wds_o_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(wds_w),
        .addr_b_i('0),
        .data_b_o()
    );

    // Oscillator Waveform Table Pointer

    // This set of registers contain the
    // beginning page number of the waveform table address and are used
    // to determine the final ram address. All waveform tables must begin
    // at the first address of a page. Also, a waveform table cannot wrap
    // around from upper to lower memory. For ex.ample, an 8k table
    // cannot start at the highest page of memory and continue through
    // lower memory. Therefore, the larger the table size, the fewer of the
    // pointer bits are actually used. 

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(1),
        .PRIORITY_READ_PORTS(1)
    ) osc_wtp_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b100)
        }),
        .priority_write_addr_i('{
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b100)
        }),
        .priority_read_addr_i('{
            addr_i[4:0]
        }),
        .priority_read_data_o('{
            wtp_o_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(wtp_w),
        .addr_b_i('0),
        .data_b_o()
    );    

    // Oscillator Control

    // The control register determines the channel assignment, oscillator
    // mode, and the Halt bit. The following is the control register bit
    // positions:
    // | D7 | D6 | D5 | D4 | D3 | D2 | D1 | DO |
    // | CA3| CA2| CA1| CA0| IE | M1 | MO | H  |
    // CA3-CA0 = Channel Assignment bits, used to assign the oscillator
    //           to a channel. The channel assignment bits are used to
    //           determine which channel the oscillator output is sent to.
    // IE = Interrupt Enable bit, used to enable the oscillator interrupt.
    // M1 = Mode bit 1, used to determine the oscillator mode.
    // M0 = Mode bit 0, used to determine the oscillator mode.
    // H = Halt bit, used to halt the oscillator.
    //
    // The oscillator mode bits determine the oscillator's behavior when
    // the oscillator is running. The following is the oscillator mode
    // bit definitions:
    // | M1 | M0 | Description |
    // | 0  | 0  | Free Run    |
    // | 0  | 1  | One Shot    |
    // | 1  | 0  | Sync/AM     |
    // | 1  | 1  | Swap        |
    //
    // Free Run Mode - In this mode the oscillator runs in a continuous
    // loop, repeating the same waveform until it is halted by the
    // controlling microprocessor, or encounters a zero in the waveform
    // table.
    //
    // One-Shot Mode - In this mode the oscillator cycles through the
    // waveform table once, halting at the end of the table.
    // Sync/AM Mode This mode uses pairs of adjacent-numbered
    // oscillators and depending on whether the lower oscillator is odd or
    // even determines the mode. To select AM mode use an odd/even
    // pair, to select sync mode use an even/odd pair.
    //
    // AM Mode - In this mode, the odd oscillator is used to amplitude
    // modulate the even oscillator. When in this mode, the volume
    // register is ignored for the pair of registers.
    //
    // Sync Mode - When in this mode the higher odd oscillator will sync
    // on to the lower even oscillator. When the fIrst oscillator wraps
    // around to the beginning of its table, the second oscillator is also reset
    // to the beginning of its table.
    //
    // Swap Mode - This mode uses pairs of oscillators where the lower
    // oscillator is even, the higher is odd. The oscillator runs in a one-shot
    // mode, when it completes its cycle it resets its accumulator to zero
    // and clears the halt bit of the next oscillator.
    //
    // H (Halt Bit) This bit indicates when an oscillator has been halted by
    // either the DOC or the micro-processor. When MO - 1 III H - 1, the
    // oscillators accumulator will be reset to zero. An oscillator will halt
    // when a zero is encountered in its waveform table. Because some
    // memory locations maybe skipped do to the frequency the oscillator
    // is set to, eight consecutive zeros must be in memory to guarantee the
    // oscillator will halt.

    reg current_osc_halt_req_r;
    reg partner_unhalt_req_r;
    reg partner_control_load_req_r;
    reg next_control_load_req_r;

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(4),
        .PRIORITY_READ_PORTS(3),
        .FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED(1),
        .LAST_PRIORITY_WRITE_PORT_INHIBITABLE(1),
        .PORT_B_ENABLE(1)
    ) osc_control_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !ready_r,
            current_osc_halt_req_r,
            partner_unhalt_req_r,
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b101)
        }),
        .priority_write_addr_i('{
            init_cycle_r,
            cycle_r,
            partner_w,
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            8'h01,
            control_w | 1'b1,
            partner_control_w & 8'b11111110,
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b101),
            partner_control_load_req_r,
            next_control_load_req_r
        }),
        .priority_read_addr_i('{
            addr_i[4:0],
            partner_w,
            cycle_r + 1'b1
        }),
        .priority_read_data_o('{
            control_o_w,
            partner_control_w,
            next_control_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(control_w),
        .addr_b_i(5'(mixer_cycle_r + 1'b1)),
        .data_b_o(mixer_control_w)
    );

    // Oscillator Resolution Table Size

    // This group of registers is used to control three oscillator functions.
    // The following chart indicates the bit positions within the register:
    // | D7 | D6 | D5 | D4 | D3 | D2 | D1 | DO |
    // | X  | BS | T2 | T1 | T0 | R2 | R1 | RO |
    //
    // X - (Reserved) This bit is reserved and should be set to zero.
    //
    // BS- (Bank Select) This bit is used to extend the addressing range of
    // the DOC. If BS - 0 then the DOC .address range is 0 to 64k, if BS = 1
    // then the DOC address range is 65k to 128k.
    //
    // T2-TO (Table size) These bits are used to specify the size of the
    // waveform table addressed by the oscillator and are used to
    // determine the final ram address. The maximum addressable
    // memory for an oscillator is 32k. The following table indicates there
    // usage:
    //
    // | T2 | T1 | T0 | Table Size in bytes
    // | 0  | 0  | 0  | 256
    // | 0  | 0  | 1  | 512
    // | 0  | 1  | 0  | 1024
    // | 0  | 1  | 1  | 2048
    // | 1  | 0  | 0  | 4096
    // | 1  | 0  | 1  | 8192
    // | 1  | 1  | 0  | 16384
    // | 1  | 1  | 1  | 32768
    //
    // R2-RO (Resolution) These bits determine which16 of the 24-bits of
    // the oscillator accumulator used to determine the final address into
    // the waveform table. Typically the Resolution register would be
    // loaded with the same value that is in the Table Size register
    // otherwise the Resolution register affects the fundamental frequency
    // by powers of 2.
    //
    // | R2 | R1 | R0 | Accumulator bits
    // | 0  | 0  | 0  |  1 through 16
    // | 0  | 0  | 1  |  2 through 17
    // | 0  | 1  | 0  |  3 through 18
    // | 0  | 1  | 1  |  4 through 19
    // | 1  | 0  | 0  |  5 through 20
    // | 1  | 0  | 1  |  6 through 21
    // | 1  | 1  | 0  |  7 through 22
    // | 1  | 1  | 1  |  8 through 23

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(1),
        .PRIORITY_READ_PORTS(1)
    ) osc_rts_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !cs_n_i & !we_n_i & (addr_i[7:5] == 3'b110)
        }),
        .priority_write_addr_i('{
            addr_i[4:0]
        }),
        .priority_write_data_i('{
            data_i
        }),
        .priority_read_req_i('{
            !cs_n_i & we_n_i & (addr_i[7:5] == 3'b110)
        }),
        .priority_read_addr_i('{
            addr_i[4:0]
        }),
        .priority_read_data_o('{
            rts_o_w
        }),
        .addr_a_i(cycle_r),
        .data_a_o(rts_w),
        .addr_b_i('0),
        .data_b_o()
    );

    // Oscillator Accumulator

    reg current_acc_add_req_r;
    reg current_acc_reset_req_r;
    reg partner_acc_reset_req_r;

    doc5503_register_ram #(
        .DATA_WIDTH(24),
        .PRIORITY_WRITE_PORTS(3),
        .PRIORITY_READ_PORTS(0)
    ) osc_acc_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            current_acc_add_req_r,
            current_acc_reset_req_r,
            partner_acc_reset_req_r
        }),
        .priority_write_addr_i('{
            cycle_r,
            cycle_r,
            partner_w
        }),
        .priority_write_data_i('{
            acc_w + f_w,
            24'h000000,
            24'h000000
        }),
        .priority_read_req_i('{0}),
        .priority_read_addr_i('{0}),
        .priority_read_data_o(),
        .addr_a_i(cycle_r),
        .data_a_o(acc_w),
        .addr_b_i('0),
        .data_b_o()
    );

    // Oscillator Output

    // We are handling the multiplier stage for each oscillator output
    // in a pipelined fashion, so the output is not available until the next
    // cycle. The output is stored in a register and then written to the
    // mixer output RAM in the next cycle. The output is scaled by the
    // volume register and the waveform data. The waveform data is converted
    // to signed by XORing with 0x80, and the volume is converted to signed

    reg signed [15:0] output_r;                                         // Current scaled oscillator output        

    // DSP Multiplier optimized for MULT9x9 hardware inference
    always @(posedge clk_i) begin
        /*
        // Use full 8-bit precision for better volume resolution
        automatic logic signed [8:0] data_w = {1'b0, wds_w} ^ 9'h080;   // convert waveform data to signed with 9-bit width
        
        // Scale volume to prevent clipping
        // Use 7 bits instead of 8 bits (divide by 2) for safety margin
        automatic logic signed [8:0] vol_s = {2'b0, vol_w[7:1]};        // scale volume by 1/2 to prevent clipping
        
        // 9x9 multiplication will be inferred as hardware multiplier
        automatic logic signed [17:0] mult_result = data_w * vol_s;
        
        // Scale down the result to prevent clipping and preserve sign
        // Use bits [17:2], preserving sign bit and scaling by another 1/4 for headroom
        output_r <= mult_result[17:2];
        */

        automatic logic signed [7:0] data_w = wds_w ^ 8'h80;            // convert waveform data to signed
        automatic logic signed [7:0] vol_s = {2'b0, vol_w[7:2]};        // convert volume to signed
        output_r <= data_w * vol_w;                                     // output is waveform data * volume
    end

    // Scaled oscillator output register
    // This register holds the scaled output of the oscillator
    // The main FSM will signal when to update the register with the
    // current output value from the DSP multiplier
    // The register is continuously read by the mixer FSM
    // based on the value of the mixer_cycle_r register.
    // To account for the pipelining, the output register uses 
    // mixer_cycle_r + 1'b1 as the address for the mixer output RAM.
    // This ensures that the output is available at the correct cycle
    // for the mixer to read.  

    reg output_reset_req;
    reg output_update_req;

    doc5503_register_ram #(
        .PRIORITY_WRITE_PORTS(3),
        .PRIORITY_READ_PORTS(0),
        .FIRST_PRIORITY_WRITE_PORT_LEVEL_TRIGGERED(1),
        .PORT_A_ENABLE(0),
        .PORT_B_ENABLE(1),
        .DATA_WIDTH(16)
    ) osc_output_r_inst (
        .clk_i(clk_i),
        .inhibit_i(inhibit_host_writes_r),
        .priority_write_req_i('{
            !ready_r,
            output_reset_req,
            output_update_req
        }),
        .priority_write_addr_i('{
            init_cycle_r,
            cycle_r,
            cycle_r
        }),
        .priority_write_data_i('{
            16'h0000,
            16'h0000,
            output_r
        }),
        .priority_read_req_i('{0}),
        .priority_read_addr_i('{0}),
        .priority_read_data_o(),
        .addr_a_i('0),
        .data_a_o(),
        .addr_b_i(5'(mixer_cycle_r + 1'b1)),
        .data_b_o(mixer_output_w)
    );

    // Host Interface

    // Controls setting of registers $E0 and $E1
    //
    // $E0 - Oscillator Interrupt Register
    // The OIR register is not used in this
    // implementation, but is included for completeness.
    //
    // $E1 - Oscillator Enable Register
    // This register controls the number of
    // active oscillators in the DOC. A minimum of one oscillator is always
    // selected, which is also the reset default. To enable oscillators
    // multiply the desired number by 2, Le. ·to enable all 32 oscillators
    // load the register with 64.

    reg prev_cs_n_r;
    wire req_cs_w = !cs_n_i & prev_cs_n_r;

    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            osc_int_r <= '0;
            osc_en_r <= 8'h3E;
        end else begin
            prev_cs_n_r <= cs_n_i;

            if (req_cs_w & !we_n_i) begin
                if (addr_i[7:5] == 3'b111) begin
                    if (addr_i[4:0] == 5'd0) osc_int_r <= data_i;
                    else if (addr_i[4:0] == 5'd1) osc_en_r <= data_i;
                end
            end
        end
    end

    // Wave Processor FSM

    localparam [2:0] OSC_IDLE = 3'd0;
    localparam [2:0] OSC_START = 3'd1;
    localparam [2:0] OSC_ACC = 3'd2;
    localparam [2:0] OSC_ADDR = 3'd3;
    localparam [2:0] OSC_READ_DATA = 3'd4;
    localparam [2:0] OSC_HALT = 3'd5;
    localparam [2:0] OSC_OUT = 3'd6;

    reg [2:0] osc_state_r;

    always @(posedge clk_i) begin
        if (!reset_n_i) begin

            irq_n_o <= '1;

            wave_address_o <= '0;
            wave_rd_o <= '0;

            next_am_req_r <= 0;

            current_wds_reset_req_r <= 0;

            current_acc_add_req_r <= 0;
            current_acc_reset_req_r <= 0;
            partner_acc_reset_req_r <= 0;

            partner_control_load_req_r <= 0;
            next_control_load_req_r <= 0;
            current_osc_halt_req_r <= 0;
            partner_unhalt_req_r <= 0;

            output_reset_req <= 0;
            output_update_req <= 0;

            inhibit_host_writes_r <= 0;

        end else begin
            wave_rd_o <= '0;

            next_am_req_r <= 0;

            current_wds_reset_req_r <= 0;

            current_acc_add_req_r <= 0;
            current_acc_reset_req_r <= 0;
            partner_acc_reset_req_r <= 0;

            partner_control_load_req_r <= 0;
            next_control_load_req_r <= 0;
            current_osc_halt_req_r <= 0;
            partner_unhalt_req_r <= 0;

            output_reset_req <= 0;
            output_update_req <= 0;
            
            if (clk_en_i) begin
                if (ready_r) begin
                    unique case (osc_state_r)
                        OSC_START: begin
                            // Start Oscillator
                            // Cycle Step 0

                            partner_control_load_req_r <= 1;
                            next_control_load_req_r <= 1;

                            osc_state_r <= OSC_ACC;
                        end
                        OSC_ACC: begin
                            // Accumulator 
                            // Cycle Step 1

                            if (!halt_w) begin
                                current_acc_add_req_r <= 1;
                                osc_state_r <= OSC_ADDR;
                            end else begin
                                if (mode_w[0]) begin
                                    current_acc_reset_req_r <= 1;
                                end
                                output_reset_req <= 1;
                                osc_state_r <= OSC_IDLE;
                            end
                        end
                        OSC_ADDR: begin
                            // Address Pointer (big barrel shifter)
                            // Cycle Step 2

                            automatic logic [4:0] shift_w = 5'd9 + res_w - wts_w;
                            wave_addr_r <= 16'(acc_w >> shift_w);
                            osc_state_r <= OSC_READ_DATA;
                        end
                        OSC_READ_DATA: begin
                            // Read Waveform Data
                            // Cycle Step 3

                            // High bit depends on the table size
                            // For 32K tables (wts_w == 3'b111), use bit 15
                            automatic int high_bit_w = (wts_w == 3'b111) ? 15 : {1'b1, wts_w};
                            automatic logic overflow = wave_addr_r[high_bit_w];
                            automatic logic zero_byte_w = (wds_w == 8'h00);

                            osc_state_r <= OSC_OUT;
                            if (!(overflow & mode_w[0]) & !zero_byte_w) begin
                                // Read next byte from SDRAM
                                automatic logic [7:0] ptr_hi_mask_w = 8'hFF << wts_w;
                                automatic logic [15:0] ptr_w = {ptr_hi_mask_w & wtp_w, 8'b0};
                                automatic logic [15:0] addr_w = (overflow ? 16'b0 : wave_addr_r) | ptr_w;
                                wave_rd_o <= 1'b1; 
                                wave_address_o <= addr_w;
                                last_osc_r <= cycle_r;
                            end 
                            if (overflow | zero_byte_w) begin
                                current_acc_reset_req_r <= 1;                                   // reset accumulator
                                if (zero_byte_w | mode_w[0]) begin                              // zero byte halts oscillator
                                    osc_state_r <= OSC_HALT;
                                end  
                                if (mode_w == MODE_SYNC_AM) begin                               // Sync AM Mode
                                    if (even_osc_w) begin                                       // Sync Mode, even oscillator
                                        partner_acc_reset_req_r <= 1;                           // reset partner oscillator
                                    end
                                end
                            end
                            // reload control registers from RAM
                            partner_control_load_req_r <= 1;
                            next_control_load_req_r <= 1;
                            inhibit_host_writes_r <= 1;
                        end
                        OSC_HALT: begin
                            // Halt Oscillator
                            // Cycle Step 4

                            current_wds_reset_req_r <= 1;                                       // silence output

                            current_osc_halt_req_r <= 1;                                        // halt current oscillator
                            
                            if (mode_w == MODE_SWAP) begin                                      // Swap Mode
                                partner_unhalt_req_r <= 1;                                      // unhalt partner oscillator
                                partner_acc_reset_req_r <= 1;                                   // reset partner accumulator
                            //end else if ((partner_mode_w == MODE_SWAP) && even_osc_w) begin     // Partner Swap Mode, even oscillator
                            //    current_osc_halt_req_r <= 0;                                    // belay the halt request
                            end
                            
                            osc_state_r <= OSC_IDLE;
                        end
                        OSC_OUT: begin
                            // Oscillator Output
                            // Cycle Step 4

                            if ((mode_w == MODE_SYNC_AM) & odd_osc_w) begin                     // Sync AM Mode, odd oscillator outputs nothing
                                output_reset_req <= 1;                                          // silence output
                                if ((cycle_r < 30) & !next_halt_w) begin                        // if next oscillator is not halted
                                    next_am_req_r <= 1;                                         // it's volume is set to current oscillator's waveform data
                                end
                            end else begin
                                output_update_req <= 1;                                         // output is waveform data * volume
                            end
                            
                            osc_state_r <= OSC_IDLE;
                        end
                        default: begin
                            // Idle
                            // Cycle Step 2 or 5 on enter

                            inhibit_host_writes_r <= 0;

                            if (last_cycle_step_w) begin
                                osc_state_r <= OSC_START;
                            end
                        end
                    endcase
                end
            end
        end

    end

    // Mixer

    localparam int MIXER_SUM_RESOLUTION = 16;

    reg signed [15:0] osc_out_r;

    reg signed [15:0] mono_mix_r;
    assign mono_mix_o = OUTPUT_OSC_DIRECT ? osc_out_r : mono_mix_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_mono_mix_r;

    reg signed [15:0] left_mix_r;
    assign left_mix_o = OUTPUT_OSC_DIRECT ? osc_out_r : left_mix_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_left_mix_r;

    reg signed [15:0] right_mix_r;
    assign right_mix_o = OUTPUT_OSC_DIRECT ? osc_out_r : right_mix_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_right_mix_r;

    reg signed [15:0] channel_r[CHANNEL_MAX:0]; 
    assign channel_o = channel_r;
    reg signed [MIXER_SUM_RESOLUTION-1:0] next_channel_r[CHANNEL_MAX:0]; 


    localparam [1:0] MIXER_INIT = 2'd0;
    localparam [1:0] MIXER_ZERO = 2'd1;
    localparam [1:0] MIXER_ADD = 2'd2;
    localparam [1:0] MIXER_OUTPUT = 2'd3;

    reg [1:0] mixer_state_r;
    reg [3:0] mixer_channel_r;

    always @(posedge clk_i) begin
        if (!reset_n_i) begin
            mixer_state_r <= MIXER_INIT;
            mixer_channel_r <= '0;
            
            mono_mix_r <= '0;
            left_mix_r <= '0;
            right_mix_r <= '0;

        end else begin
            case (mixer_state_r)
                MIXER_INIT: begin
                    channel_r[mixer_channel_r] <= '0;

                    mixer_channel_r <= mixer_channel_r + 1'd1;
                    if (mixer_channel_r == CHANNEL_MAX) mixer_state_r <= MIXER_ZERO;
                end
                MIXER_ZERO: begin
                    mixer_cycle_r <= 6'd31;

                    next_channel_r[mixer_channel_r] <= '0;

                    next_mono_mix_r <= '0;
                    next_left_mix_r <= '0;
                    next_right_mix_r <= '0;

                    mixer_channel_r <= mixer_channel_r + 1'd1;
                    if (mixer_channel_r == CHANNEL_MAX) begin
                        mixer_state_r <= MIXER_ADD;
                        mixer_cycle_r <= '0;
                    end
                end
                MIXER_ADD: begin
                    automatic logic [3:0] ca = mixer_control_w[7:4];

                    if (mixer_cycle_r == DIRECT_OUTPUT_OSC_NUM) begin
                        osc_out_r <= mixer_output_w;
                    end

                    if (OUTPUT_CHANNEL_MIX) next_channel_r[ca] <= next_channel_r[ca] + mixer_output_w;

                    if (OUTPUT_MONO_MIX) next_mono_mix_r <= next_mono_mix_r + mixer_output_w;

                    if (OUTPUT_STEREO_MIX) begin
                        if (ca[0]) next_left_mix_r <= next_left_mix_r + mixer_output_w;
                        else next_right_mix_r <= next_right_mix_r + mixer_output_w;
                    end

                    mixer_cycle_r <= (mixer_cycle_r == cycle_max_w) ? '0 : mixer_cycle_r + 1'd1;
                    if (mixer_cycle_r == cycle_max_w) mixer_state_r <= MIXER_OUTPUT;
                end
                MIXER_OUTPUT: begin                    
                    channel_r[mixer_channel_r] <= next_channel_r[mixer_channel_r][MIXER_SUM_RESOLUTION-1:MIXER_SUM_RESOLUTION-16];

                    mono_mix_r <= next_mono_mix_r[MIXER_SUM_RESOLUTION-1:MIXER_SUM_RESOLUTION-16];
                    left_mix_r <= next_left_mix_r[MIXER_SUM_RESOLUTION-1:MIXER_SUM_RESOLUTION-16];
                    right_mix_r <= next_right_mix_r[MIXER_SUM_RESOLUTION-1:MIXER_SUM_RESOLUTION-16];

                    mixer_channel_r <= mixer_channel_r + 1'd1;
                    if (mixer_channel_r == CHANNEL_MAX) mixer_state_r <= MIXER_ZERO;
                end
            endcase
        end
    end

    
endmodule