// QSPI Protocol Generator Module
// Extracted from top.sv for standalone testing

module qspi_protocol_gen #(
    parameter CLOCK_SPEED_HZ = 54_000_000
) (
    input wire clk,           // 54MHz input clock
    input wire rst_n,         // Active low reset
    
    // QSPI outputs
    output reg qspi_cs,       // Chip Select (active low)
    output reg qspi_clk,      // SCLK
    output reg [3:0] qspi_data // 4-bit parallel data
);

    // QSPI transaction state machine
    typedef enum logic [2:0] {
        IDLE,
        CS_SETUP,
        TRANSMIT,
        CS_HOLD,
        CS_RELEASE
    } qspi_state_t;
    
    qspi_state_t qspi_state_r = IDLE;
    
    // Timing and data registers
    reg [7:0] timer_r = 8'h0;           // General purpose timer
    reg [3:0] bit_counter_r = 4'h0;     // Counts 8 nibbles per transaction
    reg [31:0] packet_data_r = 32'h0;   // 32-bit packet to transmit
    reg [7:0] packet_counter_r = 8'h0;  // Packet sequence number
    
    // QSPI output signals
    reg qspi_cs_r = 1'b1;      // Chip Select (active low)
    reg qspi_clk_r = 1'b0;     // SCLK 
    reg [3:0] qspi_data_r = 4'h0; // 4-bit parallel data
    
    // Clock divider for QSPI timing
    reg [4:0] clk_div_r = '0;  // 5-bit counter for divide-by-32  
    wire qspi_clk_en_w = (clk_div_r == '0);  // Enable every 32 logic clocks
    
    // Clock calculation: 54MHz / 32 / 2 = 844kHz SCLK
    // This is close to 1MHz and good for reliable testing
    
    always @(posedge clk) begin
        if (!rst_n) begin
            clk_div_r <= '0;
            qspi_state_r <= IDLE;
            timer_r <= 8'h0;
            bit_counter_r <= 4'h0;
            packet_data_r <= 32'h0;
            packet_counter_r <= 8'h0;
            qspi_cs_r <= 1'b1;
            qspi_clk_r <= 1'b0;
            qspi_data_r <= 4'h0;
        end else begin
            clk_div_r <= clk_div_r + 1;
            
            // Clock always runs - independent of state machine
            if (qspi_clk_en_w) begin
                qspi_clk_r <= ~qspi_clk_r;
            end
            
            // State machine controls CS and data
            if (qspi_clk_en_w) begin
                case (qspi_state_r)
                    IDLE: begin
                        // Wait before starting next transaction
                        timer_r <= timer_r + 1;
                        qspi_cs_r <= 1'b1;
                        
                        if (timer_r >= 8'd100) begin  // Wait between transactions for ESP32
                            timer_r <= 8'h0;
                            bit_counter_r <= 4'h0;
                            
                            // Prepare simple test packet: AA BB CC DD  
                            packet_data_r <= 32'hAABBCCDD;  // Fixed pattern for easy debugging
                            packet_counter_r <= packet_counter_r + 1;
                            
                            qspi_state_r <= CS_SETUP;
                        end
                    end
                    
                    CS_SETUP: begin
                        // Assert CS and setup first data
                        qspi_cs_r <= 1'b0;  // CS active low
                        qspi_data_r <= packet_data_r[31:28];  // First nibble (MSB first)
                        timer_r <= 8'h0;
                        qspi_state_r <= TRANSMIT;
                    end
                    
                    TRANSMIT: begin
                        // Keep CS LOW during transmission
                        qspi_cs_r <= 1'b0;
                        
                        // Count clock edges and update data
                        if (!qspi_clk_r) begin  // On rising edge, advance to next nibble
                            bit_counter_r <= bit_counter_r + 1;
                            
                            // After 8 clock edges, we've sent all nibbles
                            if (bit_counter_r >= 4'h7) begin  
                                qspi_state_r <= CS_HOLD;
                                timer_r <= 8'h0;
                            end
                        end
                        
                        // Always update data based on current counter
                        case (bit_counter_r[2:0])  // 8 nibbles total
                            3'h0: qspi_data_r <= packet_data_r[31:28];  // 1st nibble: A
                            3'h1: qspi_data_r <= packet_data_r[27:24];  // 2nd nibble: A
                            3'h2: qspi_data_r <= packet_data_r[23:20];  // 3rd nibble: B  
                            3'h3: qspi_data_r <= packet_data_r[19:16];  // 4th nibble: B
                            3'h4: qspi_data_r <= packet_data_r[15:12];  // 5th nibble: C
                            3'h5: qspi_data_r <= packet_data_r[11:8];   // 6th nibble: C
                            3'h6: qspi_data_r <= packet_data_r[7:4];    // 7th nibble: D
                            3'h7: qspi_data_r <= packet_data_r[3:0];    // 8th nibble: D
                        endcase
                    end
                    
                    CS_HOLD: begin
                        // Hold final data stable with CS still active
                        qspi_cs_r <= 1'b0;      // Keep CS LOW during hold
                        timer_r <= timer_r + 1;
                        
                        if (timer_r >= 8'd5) begin  // Hold for ESP32 to process
                            qspi_state_r <= CS_RELEASE;
                        end
                    end
                    
                    CS_RELEASE: begin
                        // Release CS to end transaction
                        qspi_cs_r <= 1'b1;
                        qspi_data_r <= 4'h0;
                        timer_r <= 8'h0;
                        qspi_state_r <= IDLE;
                    end
                    
                    default: qspi_state_r <= IDLE;
                endcase
            end
        end
    end
    
    // Connect outputs
    assign qspi_cs = qspi_cs_r;
    assign qspi_clk = qspi_clk_r;
    assign qspi_data = qspi_data_r;

endmodule