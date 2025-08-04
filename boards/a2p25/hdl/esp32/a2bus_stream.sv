// -----------------------------------------------------
// Apple II Bus Capture - Proper Interface Usage
// -----------------------------------------------------
module a2bus_stream #(
    parameter bit ENABLE = 1'b1,
    parameter FIFO_DEPTH = 64
)(
    a2bus_if.slave a2bus_if,
    
    // ESP32 Interface  
    output logic        esp_qclk,
    output logic        esp_frame,
    output logic [3:0]  esp_qdata,
    
    // Control and Status
    input  logic        capture_enable,
    input  logic [2:0]  capture_mode,
    output logic        activity_led,
    output logic        overflow_led,
    output logic [7:0]  debug_status
);

    // Bus capture timing - use data_in_strobe for single capture per transaction
    // This ensures we only capture once per bus cycle, not repeatedly
    wire bus_cycle_w = ENABLE & capture_enable & a2bus_if.data_in_strobe & !a2bus_if.m2sel_n;
    
    // Address-based selection for filtering
    wire is_io_access_w = (a2bus_if.addr[15:12] == 4'hC);           // $C000-$CFFF
    wire is_zero_page_w = (a2bus_if.addr[15:8] == 8'h00);          // $0000-$00FF  
    wire is_stack_page_w = (a2bus_if.addr[15:8] == 8'h01);         // $0100-$01FF
    wire is_text_page_w = (a2bus_if.addr[15:11] == 5'b00100);      // $0400-$07FF
    wire is_hires_page_w = (a2bus_if.addr[15:13] == 3'b001);       // $2000-$3FFF
    wire is_rom_access_w = (a2bus_if.addr[15:12] >= 4'hD);         // $D000-$FFFF
    wire is_speaker_w = (a2bus_if.addr == 16'hC030);               // Speaker
    
    // Capture mode filtering
    reg capture_this_cycle;
    always @(*) begin
        case (capture_mode)
            3'b000: capture_this_cycle = 1'b1;                                    // Everything
            3'b001: capture_this_cycle = is_io_access_w;                          // I/O only
            3'b010: capture_this_cycle = is_zero_page_w | is_stack_page_w;        // System pages
            3'b011: capture_this_cycle = is_text_page_w | is_hires_page_w;        // Graphics pages
            3'b100: capture_this_cycle = is_rom_access_w;                         // ROM access
            3'b101: capture_this_cycle = !a2bus_if.rw_n;                         // Writes only
            3'b110: capture_this_cycle = a2bus_if.rw_n;                          // Reads only
            3'b111: capture_this_cycle = is_speaker_w;                           // Speaker only
        endcase
    end
    
    // Bus capture trigger
    wire capture_trigger_w = bus_cycle_w & capture_this_cycle;
    
    // Packet formation: [ADDR:16][DATA:8][FLAGS:8]
    // FLAGS: [7]=RW_N, [6]=M2SEL_N, [5]=M2B0, [4]=SW_GS, [3:0]=Reserved
    wire [31:0] packet_data_w = {
        a2bus_if.addr,           // [31:16] Address
        a2bus_if.data,           // [15:8]  Data  
        a2bus_if.rw_n,           // [7]     Read/Write
        a2bus_if.m2sel_n,        // [6]     M2SEL
        a2bus_if.m2b0,           // [5]     M2B0
        a2bus_if.sw_gs,          // [4]     IIgs mode
        4'b0000                  // [3:0]   Reserved
    };
    
    // Register the packet on the bus clock edge
    reg [31:0] packet_data_r;
    reg        packet_valid_r;
    
    always @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            packet_data_r <= 32'h0;
            packet_valid_r <= 1'b0;
        end else begin
            if (capture_trigger_w) begin
                packet_data_r <= packet_data_w;
                packet_valid_r <= 1'b1;
            end else if (packet_valid_r & !fifo_full) begin
                // Clear valid when packet is accepted by FIFO
                packet_valid_r <= 1'b0;
            end
        end
    end

    // I2S FIFO instance
    wire fifo_full, fifo_empty, fifo_almost_full;
    wire [6:0] fifo_count;  // 7-bit counter for 64-deep FIFO
    
    wire packet_accepted_w = packet_valid_r & !fifo_full;

    i2s_4bit_stream_fifo #(
        .FIFO_DEPTH(FIFO_DEPTH)
    ) i2s_stream (
        .clk(a2bus_if.clk_logic),
        .rst_n(a2bus_if.system_reset_n),
        .fifo_wr_en(packet_accepted_w),
        .fifo_data_in(packet_data_r),
        .fifo_full(fifo_full),
        .fifo_empty(fifo_empty),
        .fifo_almost_full(fifo_almost_full),
        .qclk(esp_qclk),
        .frame(esp_frame),
        .qdata(esp_qdata),
        .fifo_count(fifo_count)
    );

    // Activity detection
    reg bus_active_r;
    always @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            bus_active_r <= 1'b0;
        end else begin
            bus_active_r <= capture_trigger_w;
        end
    end

    // Status outputs
    assign activity_led = bus_active_r | !fifo_empty;
    assign overflow_led = packet_valid_r & fifo_full;
    assign debug_status = {
        fifo_full,              // [7]
        fifo_almost_full,       // [6] 
        capture_enable,         // [5]
        capture_mode            // [4:2]
        packet_valid_r,         // [1]
        bus_active_r            // [0]
    };

    // Performance counters for debugging
    reg [15:0] packets_captured_r = 16'h0;
    reg [15:0] packets_dropped_r = 16'h0;
    
    always @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            packets_captured_r <= 16'h0;
            packets_dropped_r <= 16'h0;
        end else begin
            if (packet_accepted_w)
                packets_captured_r <= packets_captured_r + 1;
            if (packet_valid_r & fifo_full)
                packets_dropped_r <= packets_dropped_r + 1;
        end
    end

endmodule

