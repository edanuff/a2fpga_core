//
// IIgs GLU
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
// The GLU interfaces the Ensoniq DOC5503 sound chip to the Apple II bus
//

module sound_glu #(
    parameter bit ENABLE = 1'b1,
    parameter bit NOISE_GATE_ENABLE = 1'b0,  // Temporarily disable noise gate for testing
    parameter int NOISE_GATE_THRESHOLD = 16, // Lower threshold for noise gate (0-128)
    parameter bit DEBUG_MODE = 1'b1,        // Enable debug mode to bypass DOC and inject test signal
    parameter bit TEST_TONE_ENABLE = 1'b1   // Keep test tone for debugging
) (
    a2bus_if.slave a2bus_if,

    output [7:0] data_o,
    output rd_en_o,
    output irq_n_o,

    output signed [15:0] audio_l_o,
    output signed [15:0] audio_r_o,

    // Debug outputs for external monitoring (LEDs, etc.)
    output debug_doc_active_o,     // DOC is active and configured
    output debug_osc_enabled_o,    // Oscillator 0 is enabled
    output debug_wave_reads_o,     // Wavetable memory reads occurring
    output [7:0] debug_doc_access_count_o,  // Count of DOC accesses
    
    sdram_port_if.client glu_mem_if,
    sdram_port_if.client doc_mem_if
    
);

    reg [7:0] sound_control_r;      // Sound Control Register
    reg [7:0] sound_data_r;         // Sound Data Register 
    reg [7:0] sound_ptr_lo_r;       // Sound Pointer Lo Register
    reg [7:0] sound_ptr_hi_r;       // Sound Pointer Hi Register

    localparam [15:0] SOUND_CONTROL_ADDR = 16'hC03C;
    localparam [15:0] SOUND_DATA_ADDR = 16'hC03D;
    localparam [15:0] SOUND_PTR_LO_ADDR = 16'hC03E;
    localparam [15:0] SOUND_PTR_HI_ADDR = 16'hC03F;

    // address in GLU range, $C03C-$C03F, during phi0 and m2sel_n asserted
    wire glu_sel_w = ENABLE & a2bus_if.phi0 & (a2bus_if.addr[15:2] == SOUND_CONTROL_ADDR[15:2]) & !a2bus_if.m2sel_n;
    // sound control register is at $C03C
    wire sc_sel_w = glu_sel_w & (a2bus_if.addr[1:0] == 2'b00);
    // sound data register is at $C03D
    wire sd_sel_w = glu_sel_w & (a2bus_if.addr[1:0] == 2'b01);
    // address pointer lo register is at $C03E
    wire spl_sel_w = glu_sel_w & (a2bus_if.addr[1:0] == 2'b10);
    // address pointer hi register is at $C03F
    wire sph_sel_w = glu_sel_w & (a2bus_if.addr[1:0] == 2'b11);

    // all accesses are to the dedicated 64K sound RAM
    wire access_ram_w = sound_control_r[6];
    // all accesses are to the DOC
    wire access_doc_w = ~access_ram_w;
    // auto increment address pointer
    wire auto_inc_w = sound_control_r[5];
    // volume control, 0 is lowest, 15 is highest
    wire [3:0] volume_w = sound_control_r[3:0];

    //assign rd_en_o = glu_sel_w & a2bus_if.rw_n;
    assign rd_en_o = 1'b0;

    logic [7:0] doc_data_o_w;

    assign data_o = a2bus_if.addr[1:0] == 2'b00 ? sound_control_r :
        (a2bus_if.addr[1:0] == 2'b01) & access_ram_w ? sound_data_r :
        (a2bus_if.addr[1:0] == 2'b01) & access_doc_w ? doc_data_o_w :
        a2bus_if.addr[1:0] == 2'b10 ? sound_ptr_lo_r :
        sound_ptr_hi_r;

    // write only for a2fpga, will need to implement reads at alternate address
    // and for future standalone IIgs core
    assign glu_mem_if.rd = '0;
    // DOC memory is at 0x4_0000/8 or 0x1_0000/32
    assign glu_mem_if.addr = {4'b0, 1'b1, 2'b0, sound_ptr_hi_r, sound_ptr_lo_r[7:2]};
    assign glu_mem_if.wr = ENABLE && glu_sel_w && access_ram_w && !a2bus_if.rw_n && a2bus_if.data_in_strobe;
    assign glu_mem_if.byte_en = 1'b1 << sound_ptr_lo_r[1:0];
    assign glu_mem_if.data = {a2bus_if.data, a2bus_if.data, a2bus_if.data, a2bus_if.data};

    always_ff @(posedge a2bus_if.clk_logic) begin

        if (!a2bus_if.system_reset_n) begin
            sound_control_r <= 8'h0F;
            sound_data_r <= 8'h00;
            sound_ptr_lo_r <= 8'h00;
            sound_ptr_hi_r <= 8'h00;
        end else begin

            if (ENABLE && glu_sel_w && !a2bus_if.rw_n && a2bus_if.data_in_strobe) begin
                case (a2bus_if.addr[1:0])
                    2'b00: sound_control_r <= a2bus_if.data;
                    2'b01: begin
                        sound_data_r <= a2bus_if.data;
                        if (auto_inc_w) begin
                            {sound_ptr_hi_r, sound_ptr_lo_r} <= {sound_ptr_hi_r, sound_ptr_lo_r} + 1'd1;
                        end
                    end
                    2'b10: sound_ptr_lo_r <= a2bus_if.data;
                    2'b11: sound_ptr_hi_r <= a2bus_if.data;
                endcase
            end

        end

    end

    // DOC Memory Interface
    // All DOC logic in the sound module is clocked by clk_logic

    wire [15:0] wave_addr_w;
    wire doc_mem_rd_w;

    assign doc_mem_if.wr = '0;
    assign doc_mem_if.data = '0;
    assign doc_mem_if.byte_en = 4'b1111;
    assign doc_mem_if.addr = {4'b0, 1'b1, 2'b0, wave_addr_w[15:2]};
    assign doc_mem_if.rd = ENABLE && doc_mem_rd_w;

    reg [1:0] doc_mem_offset_r;

    always_ff @(posedge a2bus_if.clk_logic) begin
        if (doc_mem_rd_w) begin
            doc_mem_offset_r <= wave_addr_w[1:0];
        end
    end

    reg [7:0] wave_data_r;
    reg wave_data_ready_r;
    always_ff @(posedge a2bus_if.clk_logic) begin
        wave_data_ready_r <= 1'b0;
        if (doc_mem_if.ready) begin
            wave_data_r <= doc_mem_if.q[8*doc_mem_offset_r +: 8];
            wave_data_ready_r <= 1'b1;
        end
    end

    //wire signed [15:0] mono_mix_w;
    wire signed [15:0] left_mix_w;
    wire signed [15:0] right_mix_w;
    //wire signed [15:0] channel_w[15:0]; 

    // Debug signals for DOC access - allows monitoring key registers and events
    reg [7:0] debug_osc_enable_r = 8'h00;
    reg [7:0] debug_osc_control_r = 8'h00;
    reg [7:0] debug_osc_volume_r = 8'h00;
    reg [7:0] debug_wtp_r = 8'h00;        // Wave table pointer for first oscillator
    reg [15:0] debug_freq_r = 16'h0000;   // Frequency for first oscillator
    
    // Additional event counters for debugging
    reg [7:0] debug_doc_access_count = 8'h00;  // Count DOC accesses
    reg [7:0] debug_wave_read_count = 8'h00;   // Count wavetable reads
    
    // Flag indicating DOC is being accessed and should be producing sound
    reg debug_doc_active = 1'b0;
    
    // Logic to capture key register writes and events for debugging
    always_ff @(posedge a2bus_if.clk_logic) begin
        if (!a2bus_if.system_reset_n) begin
            debug_osc_enable_r <= 8'h00;
            debug_osc_control_r <= 8'h00;
            debug_osc_volume_r <= 8'h00;
            debug_wtp_r <= 8'h00;
            debug_freq_r <= 16'h0000;
            debug_doc_access_count <= 8'h00;
            debug_wave_read_count <= 8'h00;
            debug_doc_active <= 1'b0;
        end else begin
            // Count wavetable memory reads (indicates oscillator is running)
            if (doc_mem_rd_w) begin
                debug_wave_read_count <= debug_wave_read_count + 8'd1;
                // If we're reading from the wavetable memory, set active flag
                debug_doc_active <= 1'b1;
            end
            
            // Capture DOC register writes
            if (ENABLE && sd_sel_w && access_doc_w && !a2bus_if.rw_n && a2bus_if.data_in_strobe) begin
                // Increment access counter on any DOC write
                debug_doc_access_count <= debug_doc_access_count + 8'd1;
                
                // Capture oscillator enable writes (0xE1)
                if (sound_ptr_lo_r == 8'hE1) begin
                    debug_osc_enable_r <= a2bus_if.data;
                end
                // Capture first oscillator's control register (0xA0)
                else if (sound_ptr_lo_r == 8'hA0) begin
                    debug_osc_control_r <= a2bus_if.data;
                    // Check if oscillator is being enabled (halt bit cleared)
                    if ((a2bus_if.data & 8'h01) == 8'h00) begin
                        debug_doc_active <= 1'b1;
                    end
                end
                // Capture first oscillator's volume register (0x40)
                else if (sound_ptr_lo_r == 8'h40) begin
                    debug_osc_volume_r <= a2bus_if.data;
                end
                // Capture first oscillator's wavetable pointer (0x80)
                else if (sound_ptr_lo_r == 8'h80) begin
                    debug_wtp_r <= a2bus_if.data;
                end
                // Capture first oscillator's frequency registers
                else if (sound_ptr_lo_r == 8'h00) begin
                    debug_freq_r[7:0] <= a2bus_if.data;
                end
                else if (sound_ptr_lo_r == 8'h20) begin
                    debug_freq_r[15:8] <= a2bus_if.data;
                end
            end
        end
    end

    doc5503 #(
        .OUTPUT_CHANNEL_MIX(0),
        .OUTPUT_MONO_MIX(0),
        .OUTPUT_STEREO_MIX(1)
    ) doc5503 (
        .clk_i(a2bus_if.clk_logic),
        .reset_n_i(a2bus_if.system_reset_n),
        .clk_en_i(a2bus_if.clk_7m_posedge),
        .irq_n_o(irq_n_o),
        .cs_n_i(~(sd_sel_w & access_doc_w & !a2bus_if.rw_n & a2bus_if.data_in_strobe)),
        .we_n_i(1'b0),  // DOC must be write-only as it shadows the real DOC chip on motherboard
        .addr_i(sound_ptr_lo_r),
        .data_i(a2bus_if.data),
        .data_o(doc_data_o_w),
        .wave_address_o(wave_addr_w),
        .wave_rd_o(doc_mem_rd_w),
        .wave_data_ready_i(wave_data_ready_r),
        .wave_data_i(wave_data_r),
        .left_mix_o(left_mix_w),
        .right_mix_o(right_mix_w),
        .mono_mix_o(),
        .channel_o()
    );

    // Volume is inverted for right shift (0 is min volume, 15 is max volume)
    // IIgs volume control ranges from 0-15, invert for right shift (0=lots of shift, 15=no shift)
    logic [3:0] volume_shift_w = volume_w < 12 ? 4'd4 - {2'b0, volume_w[3:2]} : 4'd0;
    
    // Output registers for audio with zero-centering preserved
    reg signed [15:0] audio_l_reg;
    reg signed [15:0] audio_r_reg;
    
    // Test signal generator - add a simple square wave for debugging
    // Using a slower frequency that will be clearly audible (around a440Hz)
    reg [18:0] test_counter = 0;
    reg test_tone = 0;
    
    always_ff @(posedge a2bus_if.clk_logic) begin
        // Divide clock to create audible frequency
        // For ~50MHz clock, counting to ~60000 gives about 440Hz
        if (test_counter == 19'd60000) begin
            test_counter <= 0;
            test_tone <= ~test_tone;
        end else begin
            test_counter <= test_counter + 19'd1;
        end
    end
    
    // Create square wave test signals at different frequencies to distinguish between normal and debug modes
    // Normal test tone is 440Hz (A note)
    wire signed [15:0] test_signal = test_tone ? 16'h1000 : -16'h1000;
    
    // Debug signal is 880Hz (A one octave higher) - will sound different to help identify
    reg debug_tone = 0;
    reg [17:0] debug_counter = 0;
    
    always_ff @(posedge a2bus_if.clk_logic) begin
        // Debug tone at 880Hz (twice the frequency of the regular test tone)
        if (debug_counter == 18'd30000) begin
            debug_counter <= 0;
            debug_tone <= ~debug_tone;
        end else begin
            debug_counter <= debug_counter + 18'd1;
        end
    end
    
    wire signed [15:0] debug_signal = debug_tone ? 16'h1000 : -16'h1000;
    
    // Create tone pattern sequences based on debug counters for direct audio feedback
    reg pulse_tone = 0;
    reg [23:0] pulse_counter = 0;
    reg [3:0] pulse_pattern_counter = 0;
    
    // Generate complex pulse patterns that represent debug states
    always_ff @(posedge a2bus_if.clk_logic) begin
        // Create a pulse pattern generator
        if (pulse_counter >= 24'd500000) begin
            pulse_counter <= 0;
            pulse_pattern_counter <= pulse_pattern_counter + 4'd1;
            if (pulse_pattern_counter >= 4'd10) begin
                pulse_pattern_counter <= 0;
            end
        end else begin
            pulse_counter <= pulse_counter + 24'd1;
        end
        
        // Create different pulse patterns based on debug state
        case (pulse_pattern_counter)
            0: pulse_tone <= 1'b1;  // constant tone for reference
            1: pulse_tone <= debug_doc_active;  // pulse if DOC is active
            2: pulse_tone <= debug_osc_enable_r[1];  // Oscillator 0 enabled?
            3: pulse_tone <= (debug_osc_control_r & 8'h01) == 0;  // Oscillator 0 not halted?
            4: pulse_tone <= debug_osc_volume_r > 0;  // Oscillator 0 has volume?
            5: pulse_tone <= debug_wave_read_count > 0;  // Any wavetable reads?
            6: pulse_tone <= debug_doc_access_count > 0;  // Any DOC accesses?
            7: pulse_tone <= (debug_freq_r > 0);  // Oscillator frequency set?
            default: pulse_tone <= 0;
        endcase
    end
    
    // Create audio pulses for debug output
    wire signed [15:0] pulse_signal = pulse_tone ? 16'h1000 : -16'h1000;
    
    // Also create pure debug tones for key state values
    wire signed [15:0] osc_enable_signal = debug_osc_enable_r[1] ? 16'h0800 : -16'h0800;
    wire signed [15:0] osc_active_signal = debug_doc_active ? 16'h0800 : -16'h0800;
    wire signed [15:0] osc_halt_signal = (debug_osc_control_r & 8'h01) == 0 ? 16'h0800 : -16'h0800;
    wire signed [15:0] osc_volume_signal = (debug_osc_volume_r > 0) ? 16'h0800 : -16'h0800;
    
    // Connect debug signals to outputs for LED monitoring
    assign debug_doc_active_o = debug_doc_active;
    assign debug_osc_enabled_o = debug_osc_enable_r[1];
    assign debug_wave_reads_o = (debug_wave_read_count > 0);
    assign debug_doc_access_count_o = debug_doc_access_count;
    
    // In debug mode, output debug signals, otherwise normal audio path
    // Mix with a test signal to ensure something is coming out (can be disabled with TEST_TONE_ENABLE)
    generate
        if (DEBUG_MODE) begin: debug_audio
            // Output debug info as audio in different channels
            // Left channel: DOC state summary as pulse patterns
            // Right channel: Key register values mixed with debug tone
            assign audio_l_o = pulse_signal;
            assign audio_r_o = osc_volume_signal + osc_enable_signal + 
                              (TEST_TONE_ENABLE ? debug_signal : 16'h0);
        end else begin: normal_audio
            // Normal operation - mix DOC output with test tone if enabled
            assign audio_l_o = audio_l_reg + (TEST_TONE_ENABLE ? test_signal : 16'h0);
            assign audio_r_o = audio_r_reg + (TEST_TONE_ENABLE ? test_signal : 16'h0);
        end
    endgenerate
    
    // Apply volume control and noise gate while preserving zero-centering
    always_ff @(posedge a2bus_if.clk_logic) begin
        // Apply volume control using arithmetic right shift (>>>)
        // This preserves the sign bit, maintaining zero-centered audio
        automatic logic signed [15:0] vol_adjusted_l = left_mix_w >>> volume_shift_w;
        automatic logic signed [15:0] vol_adjusted_r = right_mix_w >>> volume_shift_w;
        
        // Apply noise gate to remove buzzing during silent periods
        // Only apply when signal level is very low, preserving zero-centering
        if (NOISE_GATE_ENABLE && 
            vol_adjusted_l < NOISE_GATE_THRESHOLD && 
            vol_adjusted_l > -NOISE_GATE_THRESHOLD) begin
            audio_l_reg <= '0;  // Digital silence
        end else begin
            audio_l_reg <= vol_adjusted_l;  // Normal output
        end
        
        if (NOISE_GATE_ENABLE && 
            vol_adjusted_r < NOISE_GATE_THRESHOLD && 
            vol_adjusted_r > -NOISE_GATE_THRESHOLD) begin
            audio_r_reg <= '0;  // Digital silence
        end else begin
            audio_r_reg <= vol_adjusted_r;  // Normal output
        end
    end
    //assign audio_l_o = channel_w[0] >>> (4'd15 - volume_w);
    //assign audio_r_o = channel_w[0] >>> (4'd15 - volume_w);
    //assign audio_l_o = channel_w[1] >>> (4'd15 - volume_w);
    //assign audio_r_o = channel_w[2] >>> (4'd15 - volume_w);

endmodule