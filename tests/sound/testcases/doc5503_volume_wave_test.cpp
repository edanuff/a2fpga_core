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

// Helper to check if a sample has a signal
bool has_signal(int16_t value) {
    return value != 0;
}

// Helper to dump oscillator state
void dump_oscillator_state(Vdoc5503_harness* dut, int osc_num) {
    printf("\n--- OSCILLATOR %d STATE ---\n", osc_num);
    
    // Read frequency registers
    uint8_t freq_low = read_register(dut, osc_num);
    uint8_t freq_high = read_register(dut, 0x20 + osc_num);
    uint16_t freq = (freq_high << 8) | freq_low;
    
    // Read volume register
    uint8_t volume = read_register(dut, 0x40 + osc_num);
    
    // Read waveform data sample
    uint8_t wds = read_register(dut, 0x60 + osc_num);
    
    // Read waveform table pointer
    uint8_t wtp = read_register(dut, 0x80 + osc_num);
    
    // Read control register
    uint8_t control = read_register(dut, 0xA0 + osc_num);
    uint8_t channel = (control >> 4) & 0x0F;
    uint8_t mode = (control >> 1) & 0x03;
    uint8_t halt = control & 0x01;
    
    // Read resolution/table size register
    uint8_t rts = read_register(dut, 0xC0 + osc_num);
    uint8_t table_size = (rts >> 3) & 0x07;
    uint8_t resolution = rts & 0x07;
    
    // Read oscillator enable register
    uint8_t osc_en = read_register(dut, 0xE1);
    uint8_t enabled_oscs = osc_en >> 1;
    
    printf("Frequency: 0x%04X (%d)\n", freq, freq);
    printf("Volume: 0x%02X (%d)\n", volume, volume);
    printf("Waveform Data Sample: 0x%02X\n", wds);
    printf("Waveform Table Pointer: 0x%02X\n", wtp);
    printf("Control Register: 0x%02X (Channel=%d, Mode=%d, Halt=%d)\n", 
           control, channel, mode, halt);
    printf("Resolution/Table Size: 0x%02X (TableSize=%d, Resolution=%d)\n", 
           rts, table_size, resolution);
    printf("Oscillator Enable: 0x%02X (Enabled oscillators=%d)\n", 
           osc_en, enabled_oscs);
    
    // Interpret oscillator mode
    const char* mode_str;
    switch (mode) {
        case 0: mode_str = "Free Run"; break;
        case 1: mode_str = "One Shot"; break;
        case 2: mode_str = "Sync/AM"; break;
        case 3: mode_str = "Swap"; break;
        default: mode_str = "Unknown"; break;
    }
    
    // Interpret table size
    int table_size_bytes;
    switch (table_size) {
        case 0: table_size_bytes = 256; break;
        case 1: table_size_bytes = 512; break;
        case 2: table_size_bytes = 1024; break;
        case 3: table_size_bytes = 2048; break;
        case 4: table_size_bytes = 4096; break;
        case 5: table_size_bytes = 8192; break;
        case 6: table_size_bytes = 16384; break;
        case 7: table_size_bytes = 32768; break;
        default: table_size_bytes = 0; break;
    }
    
    printf("Oscillator Mode: %s\n", mode_str);
    printf("Table Size: %d bytes\n", table_size_bytes);
    printf("Halt Status: %s\n", halt ? "HALTED" : "RUNNING");
    printf("----------------------------\n");
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
    
    // Check the oscillator enable register default value
    uint8_t osc_en = read_register(dut, 0xE1);
    printf("Default Oscillator Enable: 0x%02X\n", osc_en);
    
    // Enable oscillators - correctly set according to 5503 Reference doc
    // Value should be 2 × N where N is the desired oscillator count
    // For a single oscillator, we use 0x02 (2×1)
    write_register(dut, 0xE1, 0x02);  // Enable 1 oscillator for better tracking of output
    
    // Verify oscillator enable setting
    osc_en = read_register(dut, 0xE1);
    printf("After setting, Oscillator Enable: 0x%02X\n", osc_en);
    
    // Set up oscillator with a higher frequency for faster cycling
    write_register(dut, 0x00, 0xFF);  // Oscillator 0 Frequency Low
    write_register(dut, 0x20, 0x0F);  // Oscillator 0 Frequency High
    
    // Set volume register and verify
    write_register(dut, 0x40, 0xFF);  // Oscillator 0 Volume (maximum)
    uint8_t volume_verify = read_register(dut, 0x40);
    printf("Volume Register after write: 0x%02X (should be 0xFF)\n", volume_verify);
    
    write_register(dut, 0x80, 0x00);  // Oscillator 0 Wave Table Pointer
    
    // Write to control register and explicitly verify it
    write_register(dut, 0xA0, 0x00);  // Control: Channel 0 (left), Free running, NOT HALTED
    uint8_t control_verify = read_register(dut, 0xA0);
    printf("Control Register after write: 0x%02X (Halt bit = %d)\n", 
           control_verify, control_verify & 0x01);
    
    write_register(dut, 0xC0, 0x00);  // Resolution/Table Size: 256-byte table, Resolution 0
    
    // Dump initial oscillator state
    printf("\n=== INITIAL OSCILLATOR STATE ===\n");
    dump_oscillator_state(dut, 0);
    
    // Run some clock cycles to allow oscillator to start
    for (int i = 0; i < 100; i++) {
        dut->clk_i = !dut->clk_i;
        
        if (dut->clk_i) {
            // Generate DOC clock enable every 8 cycles
            dut->clk_en_i = ((i % 8) == 0);
        }
        
        dut->eval();
    }
    
    // Check oscillator state after initialization
    printf("\n=== OSCILLATOR STATE AFTER INIT ===\n");
    dump_oscillator_state(dut, 0);
    
    // If oscillator is halted, force unhalt
    uint8_t control = read_register(dut, 0xA0);
    if (control & 0x01) {
        printf("\n*** OSCILLATOR IS HALTED - FORCING UNHALT ***\n");
        // Write the control register again with halt bit cleared
        for (int i = 0; i < 5; i++) {  // Try multiple times
            write_register(dut, 0xA0, 0x00);  // Force free-running, not halted
            control = read_register(dut, 0xA0);
            printf("Attempt %d - Control Register: 0x%02X (Halt=%d)\n", 
                   i+1, control, control & 0x01);
            if (!(control & 0x01)) break;  // Break if successfully unhalted
        }
    }
    
    // Setup for wave data test
    printf("\n=== WAVE DATA TEST ===\n");
    
    // Define test values
    int16_t expected_outputs[3] = {0}; // Store expected values for comparison
    uint8_t test_wave_values[] = {0x01, 0x80, 0xFF}; // Min, Zero, Max
    int num_test_waves = sizeof(test_wave_values) / sizeof(test_wave_values[0]);
    
    // Calculate expected outputs for each wave value (using DOC formula)
    for (int i = 0; i < num_test_waves; i++) {
        int8_t signed_wave = test_wave_values[i] ^ 0x80;  // Convert to signed
        int8_t vol_scaled = 0xFF >> 2;                    // Maximum volume scaled to 6 bits
        expected_outputs[i] = signed_wave * vol_scaled;
        printf("Wave value 0x%02X (signed %d) with volume 0xFF should produce output: %d\n", 
               test_wave_values[i], signed_wave, expected_outputs[i]);
    }
    
    // Track wave reads and audio outputs
    int wave_read_count = 0;
    int audio_output_samples = 0;
    uint16_t last_wave_address = 0;
    
    // Map to track wave address locations where we've provided data
    bool wave_data_provided[65536] = {false};
    
    // Run detailed test
    printf("\n=== DETAILED OSCILLATOR ANALYSIS ===\n");
    printf("Cycle | DOC Cycle | Address  | Wave Value | Left Out | Right Out | Notes\n");
    printf("------------------------------------------------------------------\n");
    
    for (int i = 0; i < 2000; i++) {
        // Toggle the clock
        dut->clk_i = !dut->clk_i;
        
        // On rising edge
        if (dut->clk_i) {
            // Generate DOC clock enable every 8 cycles
            bool clk_en = ((i % 8) == 0);
            dut->clk_en_i = clk_en;
            
            // When wave read is requested, provide one of our test values
            // Use different values based on address to see if they're properly processed
            if (dut->wave_rd_o) {
                wave_read_count++;
                last_wave_address = dut->wave_address_o;
                
                // Choose test value based on wave address
                // IMPORTANT: NEVER use 0x00 as wave value - it will halt the oscillator
                uint8_t wave_value = test_wave_values[i % num_test_waves];
                if (wave_value == 0x00) wave_value = 0x01;  // Prevent oscillator halt
                
                dut->wave_data_i = wave_value;
                dut->wave_data_ready_i = 1;
                wave_data_provided[last_wave_address] = true;
                
                // Print detailed debug for every wave read, including control register status
                uint8_t current_control = read_register(dut, 0xA0);
                uint8_t waveform_data = read_register(dut, 0x60); // Read the waveform data register
                printf("%5d | %9s | 0x%04X   | 0x%02X       | %8d | %9d | WAVE READ (Control=0x%02X Halt=%d WDS=0x%02X)\n", 
                       i, clk_en ? "YES" : "NO", last_wave_address, wave_value, 
                       dut->left_mix_o, dut->right_mix_o,
                       current_control, current_control & 0x01, waveform_data);
            } else {
                dut->wave_data_ready_i = 0;
                
                // Print output every 10 cycles to see if it changes
                if (i % 10 == 0) {
                    bool has_output = (dut->left_mix_o != 0 || dut->right_mix_o != 0);
                    if (has_output) audio_output_samples++;
                    
                    // Get control register and check channel assignment bits (4-7)
                    uint8_t current_control = read_register(dut, 0xA0);
                    uint8_t channel = (current_control >> 4) & 0x0F;
                    
                    printf("%5d | %9s | -------- | ----      | %8d | %9d | %s (Ctrl=0x%02X Ch=%d)\n", 
                           i, clk_en ? "YES" : "NO", dut->left_mix_o, dut->right_mix_o,
                           has_output ? "OUTPUT" : "", current_control, channel);
                }
            }
        }
        
        dut->eval();
    }
    
    // Check oscillator state after test
    printf("\n=== OSCILLATOR STATE AFTER TEST ===\n");
    dump_oscillator_state(dut, 0);
    
    // Check for signs of audio output
    printf("\n=== TEST SUMMARY ===\n");
    printf("Total wave reads: %d\n", wave_read_count);
    printf("Unique wave addresses accessed: %d\n", wave_read_count); // Simplified
    printf("Audio output samples detected: %d\n", audio_output_samples);
    
    // Try a focused test with the DSP multiplier
    printf("\n=== DSP MULTIPLIER TEST ===\n");
    
    // Reset all registers to default values
    dut->reset_n_i = 0;
    for (int i = 0; i < 10; i++) {
        dut->clk_i = !dut->clk_i;
        dut->eval();
    }
    dut->reset_n_i = 1;
    dut->eval();
    
    // Setup for direct multiplication test
    write_register(dut, 0xE1, 0x02);    // Enable 1 oscillator
    write_register(dut, 0x00, 0xFF);    // Maximum frequency (low)
    write_register(dut, 0x20, 0xFF);    // Maximum frequency (high)
    write_register(dut, 0x40, 0xFF);    // Maximum volume
    write_register(dut, 0xA0, 0x00);    // Channel 0, Free run
    write_register(dut, 0xC0, 0x00);    // 256-byte table, Resolution 0
    
    printf("Testing direct DSP multiplication:\n");
    printf("Wave | Volume | Expected | Left     | Right    | Result\n");
    printf("-------------------------------------------------------\n");
    
    // Test with specific known values
    uint8_t test_waves[] = {0x01, 0x40, 0x80, 0xA0, 0xFF};
    int num_waves = sizeof(test_waves) / sizeof(test_waves[0]);
    
    for (int wave_idx = 0; wave_idx < num_waves; wave_idx++) {
        uint8_t wave = test_waves[wave_idx];
        uint8_t volume = 0xFF;  // Max volume
        
        // Expected output calculation - using exact hardware formula
        int8_t signed_wave = wave ^ 0x80;  // Convert to signed
        // We need to figure out the correct formula based on the actual multiplier logic
        int16_t expected = signed_wave * volume;
        
        // Run enough cycles to let the value propagate
        for (int i = 0; i < 500; i++) {
            dut->clk_i = !dut->clk_i;
            
            if (dut->clk_i) {
                dut->clk_en_i = ((i % 8) == 0);
                
                if (dut->wave_rd_o) {
                    dut->wave_data_i = wave;
                    dut->wave_data_ready_i = 1;
                } else {
                    dut->wave_data_ready_i = 0;
                }
            }
            
            dut->eval();
        }
        
        // Check the output
        printf("0x%02X | 0x%02X   | %8d | %8d | %8d | %s\n", 
               wave, volume, expected, dut->left_mix_o, dut->right_mix_o,
               (dut->left_mix_o != 0 || dut->right_mix_o != 0) ? "SIGNAL" : "NO SIGNAL");
    }
    
    // Final analysis
    printf("\n=== FINAL ANALYSIS ===\n");
    if (audio_output_samples > 0) {
        printf("AUDIO DETECTED: The DOC5503 is producing audio output.\n");
    } else {
        printf("NO AUDIO DETECTED: Troubleshooting suggestions:\n");
        printf("1. Check the DSP multiplier in doc5503.sv - ensure it's converting waveform data to signed correctly\n");
        printf("2. Verify the mixer is receiving and processing oscillator outputs\n");
        printf("3. Confirm that channel assignment is working correctly\n");
        printf("4. Check that the Oscillator accumulator is updating as expected\n");
        printf("5. Ensure the wave data is being stored in the waveform data register\n");
    }
    
    delete dut;
    return 0;
}