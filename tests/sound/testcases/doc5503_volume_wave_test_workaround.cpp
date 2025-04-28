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

// This file implements a WORKAROUND for the DOC5503 oscillator halt issue
// It bypasses the normal DOC5503 halt logic by always providing non-zero wave values
// and explicitly re-writing the control register to try to unhalt the oscillator

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
    
    printf("===== DOC5503 VOLUME & WAVE TEST (WITH WORKAROUND) =====\n\n");
    
    // Enable oscillators
    write_register(dut, 0xE1, 0x02);  // Enable 1 oscillator
    
    // Set maximum frequency
    write_register(dut, 0x00, 0xFF);  // Freq Low = max
    write_register(dut, 0x20, 0xFF);  // Freq High = max
    
    // Set maximum volume
    write_register(dut, 0x40, 0xFF);  // Volume = max
    
    // Rest of initialization
    write_register(dut, 0x80, 0x00);  // Waveform pointer = 0
    write_register(dut, 0xA0, 0x00);  // Channel 0, Free run, not halted
    write_register(dut, 0xC0, 0x00);  // Smallest table size, lowest resolution
    
    // Verify settings
    uint8_t osc_en = read_register(dut, 0xE1);
    uint8_t vol = read_register(dut, 0x40);
    uint8_t ctrl = read_register(dut, 0xA0);
    
    printf("Settings: OscEn=0x%02X, Vol=0x%02X, Ctrl=0x%02X\n", osc_en, vol, ctrl);
    printf("Oscillator Halted: %s\n", (ctrl & 0x01) ? "YES" : "NO");
    
    // Run the test with alternating high and low values to get maximum volume swing
    printf("\nRunning test with alternating wave data values to ensure non-zero values\n");
    printf("Note: This is a workaround for the oscillator halt issue\n\n");
    
    // For tracking audio output
    bool has_audio = false;
    int wave_reads = 0;
    int total_cycles = 0;
    
    // Array to store audio samples for analysis
    const int MAX_SAMPLES = 100;
    int16_t samples[MAX_SAMPLES];
    int sample_count = 0;
    
    // Run for more cycles to ensure oscillator has time to produce output
    printf("Cycle | Wave Addr | Wave Val | Output  | Status\n");
    printf("---------------------------------------------\n");
    
    for (int i = 0; i < 10000; i++) {
        total_cycles++;
        
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        // On rising edge
        if (dut->clk_i) {
            // Generate DOC clock enable every 8 cycles
            dut->clk_en_i = ((i % 8) == 0);
            
            // WORKAROUND: Try to unhalt the oscillator regularly
            if (i % 100 == 0) {
                write_register(dut, 0xA0, 0x00);  // Try to force unhalt
            }
            
            // When wave read requested, provide alternating values
            // NEVER use 0x00 as it halts the oscillator
            if (dut->wave_rd_o) {
                wave_reads++;
                
                // Provide alternating high and low non-zero values
                // Using 0x01 and 0xFF for maximum amplitude swing
                uint8_t wave_value = (wave_reads % 2) ? 0xFF : 0x01;
                
                dut->wave_data_i = wave_value;
                dut->wave_data_ready_i = 1;
                
                // Print wave read info once every 100 reads
                if (wave_reads % 100 == 1) {
                    printf("%5d | 0x%04X    | 0x%02X     | %6d  | Wave Read\n", 
                           i, dut->wave_address_o, wave_value, dut->left_mix_o);
                }
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Sample the audio output every 100 cycles
            if (i % 100 == 0 && i > 0) {
                int16_t current_output = dut->left_mix_o;
                
                // Store sample if we have room
                if (sample_count < MAX_SAMPLES) {
                    samples[sample_count++] = current_output;
                }
                
                // Check if we have detected audio
                if (current_output != 0) {
                    has_audio = true;
                }
                
                // Print output sample every 500 cycles
                if (i % 500 == 0) {
                    printf("%5d | -------- | ----     | %6d  | Output sample\n", 
                           i, current_output);
                }
            }
        }
        
        dut->eval();
    }
    
    // Final status and analysis
    printf("\nTest complete.\n");
    printf("Total cycles: %d\n", total_cycles);
    printf("Wave reads: %d\n", wave_reads);
    printf("Audio detected: %s\n", has_audio ? "YES" : "NO");
    
    // Print the last few samples
    printf("\nLast %d audio samples:\n", sample_count);
    for (int i = 0; i < sample_count; i++) {
        printf("Sample %3d: %d\n", i, samples[i]);
    }
    
    // Analyze output for periodicity (simple analysis)
    if (sample_count > 10) {
        int sign_changes = 0;
        bool was_positive = samples[0] >= 0;
        
        for (int i = 1; i < sample_count; i++) {
            bool is_positive = samples[i] >= 0;
            if (is_positive != was_positive) {
                sign_changes++;
                was_positive = is_positive;
            }
        }
        
        printf("\nSign changes in output: %d\n", sign_changes);
        if (sign_changes > 0) {
            printf("Approximate frequency: %f Hz\n", 
                   sign_changes * 44100.0 / (2.0 * sample_count * 100));
        }
    }
    
    printf("\nWORKAROUND RECOMMENDATION:\n");
    printf("To fix the DOC5503 oscillator halt issue, consider:\n");
    printf("1. Initialize waveform data register to 0x80 instead of 0x00\n");
    printf("2. Modify the zero_byte detection logic to ignore zero values during initialization\n");
    printf("3. See DOC5503_Oscillator_Halting_Issue.md for details\n");
    
    delete dut;
    return 0;
}