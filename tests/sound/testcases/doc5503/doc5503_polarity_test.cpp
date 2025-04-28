#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Simple test to investigate the polarity of DOC5503 audio output

// Required for Verilator
double sc_time_stamp() { return 0; }

// Helper function to write a register
void write_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t data) {
    // Pulse clock while holding write signals
    dut->clk_i = 0;
    dut->cs_n_i = 0;
    dut->we_n_i = 0;
    dut->addr_i = addr;
    dut->data_i = data;
    dut->eval();
    
    dut->clk_i = 1;
    dut->eval();
    
    dut->clk_i = 0;
    dut->eval();
    
    // Release signals
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->eval();
}

// Helper function to read a register
uint8_t read_register(Vdoc5503_harness* dut, uint8_t addr) {
    // Set up read signals
    dut->cs_n_i = 0;
    dut->we_n_i = 1; // READ
    dut->addr_i = addr;
    
    // Clock cycle to read
    dut->clk_i = 0;
    dut->eval();
    dut->clk_i = 1;
    dut->eval();
    
    // Get value
    uint8_t value = dut->data_o;
    
    // Release signals
    dut->cs_n_i = 1;
    dut->eval();
    
    return value;
}

int main(int argc, char** argv) {
    printf("=== DOC5503 POLARITY TEST ===\n");
    printf("This test examines the waveform polarity with different wave values\n\n");
    
    // Initialize model
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
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
    
    // Apply reset
    printf("Applying reset...\n");
    for (int i = 0; i < 20; i++) {
        dut->clk_i = !dut->clk_i;
        dut->eval();
    }
    dut->reset_n_i = 1;
    
    // Setup oscillator for fixed output
    printf("Configuring DOC5503...\n");
    write_register(dut, 0xE1, 0x02);    // Enable 1 oscillator
    write_register(dut, 0x00, 0xFF);    // Max frequency (low)
    write_register(dut, 0x20, 0x0F);    // Medium-high frequency (high)
    write_register(dut, 0x40, 0xFF);    // Maximum volume
    write_register(dut, 0x80, 0x00);    // Waveform pointer at page 0
    write_register(dut, 0xA0, 0x00);    // Channel 0 (right), free run mode
    write_register(dut, 0xC0, 0x00);    // 256-byte table, resolution 0
    
    // Define test wave values
    const uint8_t test_values[] = {
        0x01,  // Minimum value (very negative when signed)
        0x40,  // Negative value
        0x80,  // Zero
        0xC0,  // Positive value
        0xFF   // Maximum value (very positive when signed)
    };
    
    // Array to store results
    struct TestResult {
        uint8_t wave_value;
        int16_t left_output;
        int16_t right_output;
    };
    
    TestResult results[5];
    
    printf("\nTesting different wave values for polarity:\n");
    printf("  Waveform Value | Signed Value | Left Output | Right Output\n");
    printf("  ------------------------------------------------------\n");
    
    for (int i = 0; i < 5; i++) {
        uint8_t wave_value = test_values[i];
        
        // Reset the DOC for a clean test
        dut->reset_n_i = 0;
        for (int j = 0; j < 10; j++) {
            dut->clk_i = !dut->clk_i;
            dut->eval();
        }
        dut->reset_n_i = 1;
        
        // Reconfigure all settings
        write_register(dut, 0xE1, 0x02);    // Enable 1 oscillator
        write_register(dut, 0x00, 0xFF);    // Max frequency (low)
        write_register(dut, 0x20, 0x0F);    // Medium-high frequency (high)
        write_register(dut, 0x40, 0xFF);    // Maximum volume
        write_register(dut, 0x80, 0x00);    // Waveform pointer at page 0
        write_register(dut, 0xA0, 0x00);    // Channel 0 (right), free run mode
        write_register(dut, 0xC0, 0x00);    // 256-byte table, resolution 0
        
        // Run the test with a fixed wave value - regardless of address
        int audio_detected = 0;
        int16_t left_output = 0, right_output = 0;
        
        // Run enough cycles to get stable output
        for (int j = 0; j < 2000; j++) {
            dut->clk_i = !dut->clk_i;
            
            if (dut->clk_i) {
                dut->clk_en_i = ((j % 8) == 0);
                
                if (dut->wave_rd_o) {
                    // Always provide the same test value
                    dut->wave_data_i = wave_value;
                    dut->wave_data_ready_i = 1;
                } else {
                    dut->wave_data_ready_i = 0;
                }
                
                // Sample the audio output periodically
                if (j > 1000 && j % 50 == 0) {
                    left_output = dut->left_mix_o;
                    right_output = dut->right_mix_o;
                    
                    if (left_output != 0 || right_output != 0) {
                        audio_detected++;
                    }
                }
            }
            
            dut->eval();
        }
        
        // Store the results
        results[i].wave_value = wave_value;
        results[i].left_output = left_output;
        results[i].right_output = right_output;
        
        // Print the results
        int8_t signed_value = wave_value ^ 0x80;  // Convert to signed value as DOC does internally
        printf("  0x%02X (%3d)    | %4d        | %5d      | %5d\n", 
               wave_value, wave_value, signed_value, left_output, right_output);
    }
    
    // Analyze the results
    printf("\nAnalysis:\n");
    
    // Check for sign consistency - is negative wave data producing negative output?
    bool sign_consistent = true;
    for (int i = 0; i < 5; i++) {
        int8_t signed_value = results[i].wave_value ^ 0x80;  // DOC's internal conversion
        
        // Assuming right channel is being used (0xA0 = 0x00)
        // Check if sign is preserved in the output
        if ((signed_value > 0 && results[i].right_output < 0) ||
            (signed_value < 0 && results[i].right_output > 0)) {
            sign_consistent = false;
            printf("- Sign inconsistency for wave value 0x%02X: signed = %d, output = %d\n",
                   results[i].wave_value, signed_value, results[i].right_output);
        }
    }
    
    if (sign_consistent) {
        printf("- Sign is consistent between waveform data and audio output\n");
    } else {
        printf("- Sign inconsistency detected! DOC5503 is inverting the sign.\n");
    }
    
    // Check for zero crossing
    printf("- Wave value 0x80 (zero when signed) produces output: %d\n", 
           results[2].right_output);
    
    // Check polarity trend
    if (results[0].right_output > results[4].right_output) {
        printf("- Polarity appears INVERTED: lower wave values produce higher output\n");
    } else if (results[0].right_output < results[4].right_output) {
        printf("- Polarity appears CORRECT: higher wave values produce higher output\n");
    } else {
        printf("- Polarity trend unclear\n");
    }
    
    printf("\nPossible factors affecting polarity:\n");
    printf("1. XOR operation on waveform data (wds_w ^ 0x80) in doc5503.sv\n");
    printf("2. Multiplier implementation in doc5503.sv\n");
    printf("3. Channel routing in mixer logic\n");
    printf("4. Compressor polarity handling\n");
    
    delete dut;
    return 0;
}