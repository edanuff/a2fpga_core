// Ultra-simple test stream for ESP32 communication debugging
// Just toggles pins with basic patterns - no complex logic

module test_stream (
    input  logic        clk,                // FPGA system clock
    input  logic        enable,             // Enable test stream
    
    // ESP32 Interface  
    output logic        esp_qclk,           // Quad clock to ESP32
    output logic        esp_frame,          // Frame signal to ESP32
    output logic [3:0]  esp_qdata           // 4-bit data to ESP32
);

    // Simple counter for all signals
    reg [23:0] counter = 24'h0;
    
    always @(posedge clk) begin
        if (enable) begin
            counter <= counter + 1;
        end else begin
            counter <= 24'h0;
        end
    end
    
    // Generate simple test patterns
    assign esp_qclk = counter[16];           // Slow clock ~400Hz at 54MHz
    assign esp_frame = counter[19];          // Even slower frame ~50Hz
    assign esp_qdata = counter[19:16];       // 4-bit counter pattern

endmodule