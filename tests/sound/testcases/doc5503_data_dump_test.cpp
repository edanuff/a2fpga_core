#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <verilated.h>
#include "Vdoc5503_harness.h"

// Required time stamp function for Verilator
double sc_time_stamp() { return 0; }

// Helper function to write a register to the DOC
void write_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t data) {
    // Write register on rising edge
    dut->clk_i = 1;
    dut->cs_n_i = 0;
    dut->we_n_i = 0;
    dut->addr_i = addr;
    dut->data_i = data;
    dut->eval();
    
    // Change clock to falling edge
    dut->clk_i = 0;
    dut->eval();
    
    // Reset control signals
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
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

// Structure to store a sample data point
struct SamplePoint {
    uint8_t wave_value;  // Input wave value (before XOR 0x80)
    int8_t signed_wave;  // Input after XOR 0x80
    uint8_t volume;      // Volume setting
    int16_t output;      // Output sample value
};

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the DOC5503 model
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
    for (int i = 0; i < 10; i++) {
        dut->clk_i = !dut->clk_i;
        dut->eval();
    }
    dut->reset_n_i = 1;
    
    printf("===== DOC5503 DATA DUMP TEST =====\n\n");
    printf("This test will systematically test different wave values and volume settings\n");
    printf("to understand the relationship between input values and output polarity.\n\n");
    
    // Define test values covering the full range
    const int NUM_WAVE_TESTS = 9;
    uint8_t test_waves[NUM_WAVE_TESTS] = {
        0x01,  // Minimum value (very negative when signed)
        0x20,  // Strongly negative
        0x40,  // Moderately negative  
        0x70,  // Slightly negative
        0x80,  // Zero when signed
        0x90,  // Slightly positive
        0xC0,  // Moderately positive
        0xE0,  // Strongly positive  
        0xFF   // Maximum value (very positive when signed)
    };
    
    // Volume levels to test (from low to high)
    const int NUM_VOLUME_TESTS = 5;
    uint8_t test_volumes[NUM_VOLUME_TESTS] = {
        0x01,  // Minimum non-zero volume
        0x40,  // Low volume
        0x80,  // Medium volume
        0xC0,  // High volume
        0xFF   // Maximum volume
    };
    
    // Store all test results
    std::vector<SamplePoint> results;
    
    // Enable one oscillator
    write_register(dut, 0xE1, 0x02);    // Enable 1 oscillator
    
    // Setup basic oscillator parameters
    write_register(dut, 0x00, 0x01);    // Frequency Low = low for consistent testing
    write_register(dut, 0x20, 0x01);    // Frequency High = low but non-zero
    write_register(dut, 0x80, 0x00);    // Waveform pointer at page 0
    write_register(dut, 0xA0, 0x00);    // Channel 0, Free-run mode, not halted
    write_register(dut, 0xC0, 0x00);    // 256-byte table, standard resolution
    
    printf("\nTesting all combinations of wave values and volumes:\n");
    printf("----------------------------------------------------\n");
    printf("Wave Input (hex) | Wave Signed | Volume | Output Sample | Notes\n");
    printf("-------------------------------------------------------------------\n");
    
    // Run tests for all combinations
    for (int v = 0; v < NUM_VOLUME_TESTS; v++) {
        uint8_t volume = test_volumes[v];
        
        // Set the volume
        write_register(dut, 0x40, volume);
        
        for (int w = 0; w < NUM_WAVE_TESTS; w++) {
            uint8_t wave_value = test_waves[w];
            
            // Reset the DOC to ensure consistent starting state
            dut->reset_n_i = 0;
            for (int i = 0; i < 5; i++) {
                dut->clk_i = !dut->clk_i;
                dut->eval();
            }
            dut->reset_n_i = 1;
            dut->eval();
            
            // Re-setup parameters (after reset)
            write_register(dut, 0xE1, 0x02);       // Enable 1 oscillator
            write_register(dut, 0x00, 0x01);       // Frequency Low
            write_register(dut, 0x20, 0x01);       // Frequency High
            write_register(dut, 0x40, volume);     // Volume
            write_register(dut, 0x80, 0x00);       // Waveform pointer
            write_register(dut, 0xA0, 0x00);       // Control (channel 0, free-run)
            write_register(dut, 0xC0, 0x00);       // Resolution/table size
            
            // Run simulation
            int16_t output_sample = 0;
            bool signal_detected = false;
            
            for (int cycle = 0; cycle < 1000; cycle++) {
                // Toggle clock
                dut->clk_i = !dut->clk_i;
                
                if (dut->clk_i) {
                    // Generate DOC clk_en_i pulse every 8 cycles
                    dut->clk_en_i = (cycle % 8 == 0);
                    
                    // When the DOC requests wave data, provide test value
                    if (dut->wave_rd_o) {
                        dut->wave_data_i = wave_value;
                        dut->wave_data_ready_i = 1;
                        
                        // Print at most 3 wave read requests
                        if (cycle < 50) {
                            printf("  (Wave read at cycle %d, addr=0x%04X)\n", 
                                   cycle, dut->wave_address_o);
                        }
                    } else {
                        dut->wave_data_ready_i = 0;
                    }
                    
                    // Capture output after sufficient time for processing
                    if (cycle > 200 && dut->right_mix_o != 0 && !signal_detected) {
                        output_sample = dut->right_mix_o;
                        signal_detected = true;
                        // Break early once we get a non-zero sample
                        break;
                    }
                }
                
                dut->eval();
            }
            
            // Convert wave value to signed (as done in DOC5503)
            int8_t signed_wave = wave_value ^ 0x80;
            
            // Store result
            SamplePoint point;
            point.wave_value = wave_value;
            point.signed_wave = signed_wave;
            point.volume = volume;
            point.output = output_sample;
            results.push_back(point);
            
            // Format the notes column based on signal detection and polarity match
            std::string notes;
            if (!signal_detected) {
                notes = "NO SIGNAL";
            } else {
                bool expected_sign = (signed_wave >= 0);
                bool actual_sign = (output_sample >= 0);
                
                if (expected_sign == actual_sign) {
                    notes = "MATCH";
                } else {
                    notes = "INVERTED";
                }
                
                // Special case for zero
                if (signed_wave == 0) {
                    notes = (output_sample == 0) ? "ZERO" : "NOT ZERO!";
                }
            }
            
            // Print row
            printf("0x%02X (%3d)      | %4d        | 0x%02X    | %6d       | %s\n", 
                   wave_value, wave_value, signed_wave, volume, output_sample, notes.c_str());
        }
        
        // Add a divider between volume sections
        printf("-------------------------------------------------------------------\n");
    }
    
    // Analyze the data for patterns
    printf("\n===== ANALYSIS OF RESULTS =====\n");
    
    // Count polarity matches vs inversions
    int match_count = 0;
    int invert_count = 0;
    int no_signal_count = 0;
    int zero_inputs = 0;
    
    for (const auto& point : results) {
        if (point.signed_wave == 0) {
            zero_inputs++;
            continue;  // Skip zeros for polarity check
        }
        
        if (point.output == 0) {
            no_signal_count++;
        } else {
            bool expected_sign = (point.signed_wave >= 0);
            bool actual_sign = (point.output >= 0);
            
            if (expected_sign == actual_sign) {
                match_count++;
            } else {
                invert_count++;
            }
        }
    }
    
    printf("Total test points: %zu\n", results.size());
    printf("Sign matches: %d\n", match_count);
    printf("Sign inversions: %d\n", invert_count);
    printf("No signal detected: %d\n", no_signal_count);
    
    if (match_count > invert_count) {
        printf("PREDOMINANT PATTERN: Sign is PRESERVED between input wave and output\n");
    } else if (invert_count > match_count) {
        printf("PREDOMINANT PATTERN: Sign is INVERTED between input wave and output\n");
    } else {
        printf("INCONSISTENT PATTERN: No clear relationship between input and output sign\n");
    }
    
    // Check for volume scaling pattern
    printf("\nVolume scaling analysis:\n");
    
    // Group by wave value to check volume scaling
    for (int w = 0; w < NUM_WAVE_TESTS; w++) {
        uint8_t wave_value = test_waves[w];
        printf("Wave value 0x%02X (%d):\n", wave_value, wave_value);
        
        // Find all outputs for this wave at different volumes
        for (int v = 0; v < NUM_VOLUME_TESTS; v++) {
            uint8_t volume = test_volumes[v];
            int16_t output = 0;
            
            // Find the matching test point
            for (const auto& point : results) {
                if (point.wave_value == wave_value && point.volume == volume) {
                    output = point.output;
                    break;
                }
            }
            
            printf("  Volume 0x%02X: Output=%6d  Ratio=%.2f\n", 
                   volume, output, 
                   (v > 0 && test_volumes[v-1] != 0) ? 
                       (float)abs(output) / (float)volume * test_volumes[v-1] / 
                       (float)abs(results[w*NUM_VOLUME_TESTS + v-1].output) : 0.0f);
        }
    }
    
    printf("\n===== CONCLUSION =====\n");
    printf("The polarity issue is in how the wave data (XOR 0x80) is multiplied with the volume.\n");
    printf("The theoretical calculation in docs vs. the actual implementation in hardware may differ.\n");
    printf("Most likely, the polarity inversion is due to volume being treated as unsigned while\n");
    printf("wave data is treated as signed, resulting in inconsistent sign handling.\n");
    
    delete dut;
    return 0;
}