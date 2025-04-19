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
    parameter bit ENABLE = 1'b1
) (
    a2bus_if.slave a2bus_if,

    output [7:0] data_o,
    output rd_en_o,
    output irq_n_o,

    output [15:0] audio_l_o,
    output [15:0] audio_r_o,

    sdram_port_if.client glu_mem_if,
    sdram_port_if.client doc_mem_if,
    
    // Debug outputs
    output wave_rd_o,
    output wave_data_ready_o,
    output cs_active_o,
    output signed [15:0] left_mix_debug_o,
    output signed [15:0] right_mix_debug_o
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
    
    // Flag to detect if wave data is being read and is non-zero
    reg wave_data_nonzero = 0;
    // Counter to track number of doc_mem_rd_w assertions
    reg [15:0] wave_read_counter = 0;
    
    always_ff @(posedge a2bus_if.clk_logic) begin
        wave_data_ready_r <= 1'b0;
        
        // Count wave read requests
        if (doc_mem_rd_w)
            wave_read_counter <= wave_read_counter + 1;
            
        if (doc_mem_if.ready) begin
            // Extract byte from 32-bit word based on offset
            case (doc_mem_offset_r)
                2'b00: wave_data_r <= doc_mem_if.q[7:0];
                2'b01: wave_data_r <= doc_mem_if.q[15:8];
                2'b10: wave_data_r <= doc_mem_if.q[23:16];
                2'b11: wave_data_r <= doc_mem_if.q[31:24];
            endcase
            
            // Set flag if we ever read non-zero waveform data
            if (doc_mem_if.q != 32'h0)
                wave_data_nonzero <= 1'b1;
                
            wave_data_ready_r <= 1'b1;
        end
    end

    //wire signed [15:0] mono_mix_w;
    wire signed [15:0] left_mix_w;
    wire signed [15:0] right_mix_w;
    //wire signed [15:0] channel_w[15:0]; 

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
        .we_n_i(1'b0),
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
    
    // Monitor DOC register writes to help debug
    reg [7:0] doc_reg_monitor[0:31];  // Store the last values written to DOC registers
    always @(posedge a2bus_if.clk_logic) begin
        if (!a2bus_if.system_reset_n) begin
            for (int i = 0; i < 32; i++) begin
                doc_reg_monitor[i] <= 8'h00;
            end
        end else if (sd_sel_w & access_doc_w & !a2bus_if.rw_n & a2bus_if.data_in_strobe) begin
            // Log every write to the DOC
            doc_reg_monitor[sound_ptr_lo_r[4:0]] <= a2bus_if.data;
        end
    end

    logic [3:0] volume_shift_w = volume_w ^ 4'hF;
    
    // Simple 440Hz tone generator for testing
    reg [15:0] test_tone_counter = 0;
    reg signed [15:0] test_tone = 0;
    
    // Division factor for different frequencies at 54MHz
    // 440Hz: 54MHz / 440Hz / 2 = 61363
    // (we divide by 2 because each toggle generates half a cycle)
    localparam TEST_TONE_DIVISOR = 16'd61363;  // For 440Hz at 54MHz
    
    always @(posedge a2bus_if.clk_logic) begin
        // Simple counter-based tone generator
        test_tone_counter <= test_tone_counter + 1'd1;
        if (test_tone_counter >= TEST_TONE_DIVISOR) begin
            test_tone_counter <= 16'd0;
            test_tone <= ~test_tone;  // Toggle between positive and negative
        end
    end
    
    // DOC is now producing audible output - disable test tone and reduce amplification
    
    // DOC output - direct connection with no additional amplification
    // The DOC5503 module is already applying 64x amplification
    assign audio_l_o = left_mix_w;
    assign audio_r_o = right_mix_w;
    
    // Alternative audio paths with volume controls
    //assign audio_l_o = left_mix_w >>> volume_shift_w[3:2];
    //assign audio_r_o = right_mix_w >>> volume_shift_w[3:2];

    // Connect debug outputs
    assign wave_rd_o = doc_mem_rd_w;
    assign wave_data_ready_o = wave_data_ready_r;
    assign cs_active_o = (wave_read_counter > 0);  // Lights up if any wave reads have occurred
    assign left_mix_debug_o = left_mix_w;
    assign right_mix_debug_o = right_mix_w;

endmodule