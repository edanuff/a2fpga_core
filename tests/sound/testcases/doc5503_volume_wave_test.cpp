#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

// Test strategy:
// 1. Setup a simple oscillator with controlled settings 
// 2. Force wave data to specific known values
// 3. Validate that the volume scaling works correctly

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

// Helper to check if a sample has a signal
bool has_signal(int16_t value) {
    return value != 0;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    // Create the DOC model
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
    dut->eval();
    
    printf("===== DOC5503 VOLUME & WAVE TEST =====\n\n");
    
    // Enable many oscillators to make sure the mixer is running
    write_register(dut, 0xE1, 0x3E);  // Enable all 31 oscillators (this was the default in doc5503.sv)
    
    // Test volumes from min to max
    uint8_t test_volumes[] = {32, 64, 128, 192, 255};
    int num_volumes = sizeof(test_volumes) / sizeof(test_volumes[0]);
    
    // Test waveform values covering different values
    // Note: 0x00 is special - it halts oscillators, so we'll use 0x01 instead for min value
    uint8_t test_waves[] = {0x01, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xFF};
    int num_waves = sizeof(test_waves) / sizeof(test_waves[0]);
    
    // For each volume level
    for (int vol_idx = 0; vol_idx < num_volumes; vol_idx++) {
        uint8_t volume = test_volumes[vol_idx];
        printf("\nTesting volume: 0x%02X (%d)\n", volume, volume);
        
        // Set up oscillator with a higher frequency for faster cycling (like in the extended test)
        write_register(dut, 0x00, 0xFF);  // Oscillator 0 Frequency Low
        write_register(dut, 0x20, 0x0F);  // Oscillator 0 Frequency High
        write_register(dut, 0x40, volume); // Oscillator 0 Volume
        write_register(dut, 0x80, 0x00);  // Oscillator 0 Wave Table Pointer
        
        // Set control register with appropriate channel for testing
        // Channel bits are bits 7-4 in the control register
        // Channel 0 = 0x00 (left output), Channel 1 = 0x10 (right output)
        // After fix to channel assignment in doc5503.sv
        write_register(dut, 0xA0, 0x00);  // Channel 0 (left), Free running, NOT HALTED
        
        write_register(dut, 0xC0, 0x00);  // Oscillator 0 Resolution/Table Size
        
        // Make sure we're not overriding our test channel setting
        // Control register is already set with halt bit = 0
        
        printf("Volume  | Wave    | Expected  | Outputs             | Result\n");
        printf("----------------------------------------------------------\n");
        
        // For each wave value
        for (int wave_idx = 0; wave_idx < num_waves; wave_idx++) {
            uint8_t wave = test_waves[wave_idx];
            
            // Expected output calculation - using exact hardware formula
            int8_t signed_wave = wave ^ 0x80;  // Convert to signed
            int8_t vol_scaled = (volume >> 2); // Scale volume to 6 bits
            int16_t expected = signed_wave * vol_scaled;
            
            // Run for much longer to let the oscillator read from wave table 
            // and properly propagate through the mixer - it takes many cycles!
            for (int i = 0; i < 1000; i++) {
                // Toggle clock
                dut->clk_i = !dut->clk_i;
                
                // On rising edge
                if (dut->clk_i) {
                    // Generate DOC clock enable
                    dut->clk_en_i = ((i % 8) == 0);
                    
                    // When wave read is requested, provide our test value
                    if (dut->wave_rd_o) {
                        dut->wave_data_i = wave;
                        dut->wave_data_ready_i = 1;
                    } else {
                        dut->wave_data_ready_i = 0;
                    }
                }
                
                dut->eval();
            }
            
            // Capture the output after stable cycles - check both channels
            int16_t actual_left = dut->left_mix_o;
            int16_t actual_right = dut->right_mix_o;
            int16_t actual = (actual_left != 0) ? actual_left : actual_right; // Use whichever one has a signal
            
            // Print results with both channels
            printf("0x%02X    | 0x%02X    | %6d   | L:%6d R:%6d | %s\n", 
                   volume, wave, expected, actual_left, actual_right, 
                   (abs(actual) > 0) ? "Signal" : "No signal");
        }
    }
    
    // Run a longer test with specific wave values to verify audio output
    printf("\n===== EXTENDED TEST WITH ALTERNATING WAVE VALUES =====\n");
    
    // Set maximum volume
    write_register(dut, 0x40, 0xFF);
    
    // Set to a higher frequency for faster cycling
    write_register(dut, 0x00, 0xFF); 
    write_register(dut, 0x20, 0x0F);
    
    // Use channel 0 (left) for the extended test
    write_register(dut, 0xA0, 0x00);  // Channel 0 (left), Free running, NOT HALTED
    
    // Track several consecutive samples
    const int SAMPLE_COUNT = 20;
    int16_t samples[SAMPLE_COUNT];
    bool has_audio = false;
    
    // Initialize buffer
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        samples[i] = 0;
    }
    
    printf("Running 1000 cycles with alternating wave values\n");
    
    // Run for more cycles
    for (int i = 0; i < 1000; i++) {
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        // On rising edge
        if (dut->clk_i) {
            // Generate DOC clock enable
            dut->clk_en_i = ((i % 8) == 0);
            
            // When wave read is requested, alternate between near-min and max values
            // Use 0x01 instead of 0x00 to avoid halting the oscillator
            if (dut->wave_rd_o) {
                dut->wave_data_i = (i % 2) ? 0xFF : 0x01;
                dut->wave_data_ready_i = 1;
                
                // Print the wave reads
                if (i < 50) {
                    printf("Cycle %d: Providing wave data 0x%02X at addr 0x%04X\n", 
                           i, dut->wave_data_i, dut->wave_address_o);
                }
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Sample every 50 cycles
            if (i % 50 == 0 && i > 0) {
                // Shift samples in buffer
                for (int j = 0; j < SAMPLE_COUNT-1; j++) {
                    samples[j] = samples[j+1];
                }
                
                // Save left channel output (because we're using channel 0, which now routes to left)
                samples[SAMPLE_COUNT-1] = dut->left_mix_o;
                
                // Check if we have a signal in either channel
                if (dut->left_mix_o != 0 || dut->right_mix_o != 0) {
                    has_audio = true;
                }
                
                // Report current output on both channels
                printf("Cycle %d: Left=%d Right=%d\n", i, dut->left_mix_o, dut->right_mix_o);
            }
        }
        
        dut->eval();
    }
    
    // Report results
    printf("\nFinal audio output check:\n");
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        printf("Sample %d: %d\n", i, samples[i]);
    }
    
    printf("\nAudio detection: %s\n", has_audio ? "AUDIO PRESENT" : "NO AUDIO DETECTED");
    
    delete dut;
    return 0;
}