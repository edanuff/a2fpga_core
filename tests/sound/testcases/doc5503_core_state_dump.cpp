#include <stdio.h>
#include <stdint.h>
#include <verilated.h>
#include "Vdoc5503_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

// Helper function to write a register
void write_reg(Vdoc5503_harness* dut, uint8_t addr, uint8_t data) {
    // Setup
    dut->cs_n_i = 0;
    dut->we_n_i = 0;
    dut->addr_i = addr;
    dut->data_i = data;
    
    // Clock cycle
    dut->clk_i = 0;
    dut->eval();
    dut->clk_i = 1;
    dut->eval();
    dut->clk_i = 0;
    dut->eval();
    
    // Release
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->eval();
}

// Helper function to read a register
uint8_t read_reg(Vdoc5503_harness* dut, uint8_t addr) {
    // Setup
    dut->cs_n_i = 0;
    dut->we_n_i = 1;
    dut->addr_i = addr;
    
    // Clock cycle
    dut->clk_i = 0;
    dut->eval();
    dut->clk_i = 1;
    dut->eval();
    
    // Get data
    uint8_t value = dut->data_o;
    
    // Release
    dut->cs_n_i = 1;
    dut->eval();
    
    return value;
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // Create DUT
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
    printf("=== DOC5503 State Dump Test ===\n");
    printf("This test examines the internal state of the DOC5503 module step by step,\n");
    printf("focusing on the data path from oscillator to audio output.\n\n");
    
    // Initialize inputs
    dut->reset_n_i = 0;
    dut->clk_i = 0;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Reset
    printf("Applying reset...\n");
    for (int i = 0; i < 10; i++) {
        dut->clk_i = !dut->clk_i;
        dut->eval();
    }
    dut->reset_n_i = 1;
    dut->eval();
    
    // Configure DOC
    printf("\nConfiguring DOC...\n");
    
    // Enable one oscillator
    write_reg(dut, 0xE1, 0x02);
    printf("Oscillator enable register: 0x%02X\n", read_reg(dut, 0xE1));
    
    // Setup oscillator 0
    uint8_t osc = 0;
    
    // Set frequency to a moderate value
    write_reg(dut, 0x00 + osc, 0x00);
    write_reg(dut, 0x20 + osc, 0x01);
    printf("Frequency: 0x%02X%02X\n", read_reg(dut, 0x20 + osc), read_reg(dut, 0x00 + osc));
    
    // Set volume to maximum
    write_reg(dut, 0x40 + osc, 0xFF);
    printf("Volume: 0x%02X\n", read_reg(dut, 0x40 + osc));
    
    // Set waveform table pointer to page 0
    write_reg(dut, 0x80 + osc, 0x00);
    printf("Waveform table pointer: 0x%02X\n", read_reg(dut, 0x80 + osc));
    
    // Set control register: channel 0, free run mode, not halted
    write_reg(dut, 0xA0 + osc, 0x00);
    printf("Control: 0x%02X\n", read_reg(dut, 0xA0 + osc));
    
    // Set resolution/table size: 256 bytes, standard resolution
    write_reg(dut, 0xC0 + osc, 0x00);
    printf("Resolution/Table Size: 0x%02X\n", read_reg(dut, 0xC0 + osc));
    
    // Fixed test waveform values that span the full range
    uint8_t test_values[] = {
        0x01, 0x40, 0x80, 0xC0, 0xFF
    };
    int num_test_values = sizeof(test_values) / sizeof(test_values[0]);
    int test_index = 0;
    
    printf("\nRunning simulation with wave data tracing for 200 cycles...\n");
    printf("%-5s | %-10s | %-10s | %-10s | %-15s | %-15s | %-15s\n", 
           "Cycle", "CLK_EN", "Wave Read", "Wave Data", "Left Output", "Right Output", "Notes");
    printf("--------------------------------------------------------------------------------\n");
    
    // Capture DOC state cycle by cycle
    for (int cycle = 0; cycle < 200; cycle++) {
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        // Rising edge operations
        if (dut->clk_i) {
            // Generate DOC clock enable every 8 cycles
            dut->clk_en_i = (cycle % 8 == 0);
            
            // When wave read is requested, provide the next test value
            if (dut->wave_rd_o) {
                // Use test values in sequence
                uint8_t wave_data = test_values[test_index];
                test_index = (test_index + 1) % num_test_values;
                
                dut->wave_data_i = wave_data;
                dut->wave_data_ready_i = 1;
                
                printf("%-5d | %-10s | %-10s | 0x%-8X | %-15d | %-15d | Provided wave data\n", 
                       cycle, 
                       dut->clk_en_i ? "YES" : "NO", 
                       "YES", 
                       wave_data, 
                       dut->left_mix_o, 
                       dut->right_mix_o);
            } else {
                dut->wave_data_ready_i = 0;
                
                // Only print every 8th cycle to reduce output volume
                if (cycle % 8 == 0 || dut->left_mix_o != 0 || dut->right_mix_o != 0) {
                    printf("%-5d | %-10s | %-10s | %-10s | %-15d | %-15d | %s\n", 
                           cycle, 
                           dut->clk_en_i ? "YES" : "NO", 
                           "NO", 
                           "-", 
                           dut->left_mix_o, 
                           dut->right_mix_o,
                           (dut->left_mix_o != 0 || dut->right_mix_o != 0) ? "AUDIO OUTPUT!" : "");
                }
            }
        }
        
        // Evaluate model
        dut->eval();
    }
    
    // Final register state
    printf("\nFinal register state (oscillator 0):\n");
    printf("Waveform Data: 0x%02X\n", read_reg(dut, 0x60));
    printf("Volume: 0x%02X\n", read_reg(dut, 0x40));
    printf("Control: 0x%02X\n", read_reg(dut, 0xA0));
    
    // Analyze DSP multiplier based on code
    printf("\nDSP Multiplier Analysis:\n");
    printf("From doc5503.sv (line ~676-678):\n");
    printf("  automatic logic signed [7:0] data_w = wds_w ^ 8'h80;       // convert waveform data to signed\n");
    printf("  automatic logic signed [8:0] vol_s = {1'b0, vol_w};        // convert volume to signed\n");
    printf("  output_r <= data_w * vol_s;                                // output is waveform data * volume\n\n");
    
    // For a specific test case, calculate expected output
    uint8_t wds = 0xC0;  // Example waveform data
    uint8_t vol = 0xFF;  // Example volume
    
    int8_t data_signed = wds ^ 0x80;               // Convert to signed (-128 to 127)
    int16_t output_expected = data_signed * vol;   // Expected multiplication result
    
    printf("Test case calculation:\n");
    printf("Waveform value: 0x%02X\n", wds);
    printf("Volume value: 0x%02X\n", vol);
    printf("Signed waveform (after XOR 0x80): %d\n", data_signed);
    printf("Expected DSP output: %d\n", output_expected);
    
    // Output path analysis - can't directly access module parameters
    printf("\nOutput path analysis:\n");
    printf("Note: Can't directly check parameters in the test but DOC5503 is configured with:\n");
    printf("- OUTPUT_OSC_DIRECT=1 (typically)\n");
    printf("- OUTPUT_MONO_MIX=1 (typically)\n");
    printf("- OUTPUT_STEREO_MIX=1 (typically)\n");
    
    // Summary
    printf("\nSummary:\n");
    if (dut->left_mix_o == 0 && dut->right_mix_o == 0) {
        printf("No audio output detected in 200 cycles. Possible issues:\n");
        printf("1. Oscillator is halting due to zero byte detection\n");
        printf("2. Channel routing is incorrect\n");
        printf("3. DSP multiplier not producing output\n");
        printf("4. Mixer not passing audio to output\n");
    } else {
        printf("Audio output detected! The fixed DSP multiplier is working.\n");
    }
    
    // Clean up
    delete dut;
    return 0;
}