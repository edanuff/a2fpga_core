#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

// Write a WAV file from a buffer of samples
void write_wav_file(const char* filename, const int16_t* samples, int num_samples, int sample_rate = 44100) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("Error: couldn't open %s for writing\n", filename);
        return;
    }
    
    // WAV header
    const uint32_t data_size = num_samples * sizeof(int16_t);
    const uint32_t total_size = data_size + 36;
    
    // RIFF header
    fwrite("RIFF", 1, 4, fp);
    fwrite(&total_size, 4, 1, fp);
    fwrite("WAVE", 1, 4, fp);
    
    // Format chunk
    fwrite("fmt ", 1, 4, fp);
    uint32_t chunk_size = 16;
    fwrite(&chunk_size, 4, 1, fp);
    uint16_t audio_format = 1; // PCM
    fwrite(&audio_format, 2, 1, fp);
    uint16_t num_channels = 1; // Mono
    fwrite(&num_channels, 2, 1, fp);
    uint32_t sample_rate_value = sample_rate;
    fwrite(&sample_rate_value, 4, 1, fp);
    uint32_t byte_rate = sample_rate * num_channels * sizeof(int16_t);
    fwrite(&byte_rate, 4, 1, fp);
    uint16_t block_align = num_channels * sizeof(int16_t);
    fwrite(&block_align, 2, 1, fp);
    uint16_t bits_per_sample = 16;
    fwrite(&bits_per_sample, 2, 1, fp);
    
    // Data chunk
    fwrite("data", 1, 4, fp);
    fwrite(&data_size, 4, 1, fp);
    fwrite(samples, sizeof(int16_t), num_samples, fp);
    
    fclose(fp);
    printf("Wrote %d samples to %s\n", num_samples, filename);
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // Create an instance of our model
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
    // Always enable trace dump for this test
    VerilatedVcdC* tfp = nullptr;
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    dut->trace(tfp, 99); // Trace 99 levels of hierarchy
    tfp->open("doc5503_sine_test_fixed.vcd");
    
    printf("=== DOC5503 Sine Wave Test (FIXED) ===\n");
    printf("Generating a pure sine wave using a single oscillator\n\n");
    
    // Create a sine wave lookup table
    const int WAVE_TABLE_SIZE = 256;
    uint8_t wave_table[WAVE_TABLE_SIZE];
    
    // Generate a proper bipolar waveform centered around 0x80
    printf("Generating sine wave table with center point at 0x80...\n");
    for (int i = 0; i < WAVE_TABLE_SIZE; i++) {
        double angle = ((double)i / WAVE_TABLE_SIZE) * 2.0 * M_PI;
        double value = sin(angle);
        wave_table[i] = (uint8_t)(value * 127.0 + 128.0);
        
        // CRITICAL: Avoid 0x00 since it halts the oscillator
        if (wave_table[i] == 0) {
            wave_table[i] = 1;
        }
    }
    
    // Print the wave table
    printf("Wave table values:\n");
    for (int i = 0; i < WAVE_TABLE_SIZE; i++) {
        if (i % 16 == 0) printf("\n%3d: ", i);
        printf("%02X ", wave_table[i]);
    }
    printf("\n\n");
    
    // Initialize the DOC5503
    dut->reset_n_i = 0;
    dut->clk_i = 0;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Constants for timing
    const int DOC_CLK_DIVIDER = 8;   // DOC clock enable every 8 cycles
    
    // Reset for several cycles with clk_en_i pulses
    printf("Resetting DOC5503...\n");
    int cycle = 0;
    int rising_edges = 0;
    
    for (int i = 0; i < 50; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate DOC clock enable every 8 cycles (4 rising edges)
        if (dut->clk_i) { // On rising edges only
            rising_edges++;
            if (rising_edges == 4) { // Every 4th rising edge
                dut->clk_en_i = 1;
                rising_edges = 0;
            } else {
                dut->clk_en_i = 0;
            }
        } else {
            dut->clk_en_i = 0; // Always 0 on falling edges
        }
        
        dut->eval();
        tfp->dump(cycle++);
    }
    
    dut->reset_n_i = 1;
    
    // Wait for ready_o to go high before configuring registers
    printf("Waiting for DOC5503 to be ready...\n");
    int ready_wait = 0;
    
    while (!dut->ready_o && ready_wait < 1000) {
        dut->clk_i = !dut->clk_i;

        // Generate DOC clock enable every 8 cycles (4 rising edges)
        if (dut->clk_i) { // On rising edges only
            rising_edges++;
            if (rising_edges == 4) { // Every 4th rising edge
                dut->clk_en_i = 1;
                rising_edges = 0;
                printf("  clk_en_i pulse at cycle %d\n", ready_wait);
            } else {
                dut->clk_en_i = 0;
            }
        } else {
            dut->clk_en_i = 0; // Always 0 on falling edges
        }

        dut->eval();
        tfp->dump(cycle++);
        ready_wait++;
    }

    if (!dut->ready_o) {
        printf("ERROR: DOC5503.ready_o did not go high within timeout period!\n");
        return 1;
    }

    printf("DOC5503 is ready after %d cycles.\n", ready_wait);

    // Helper to write a register and verify it was written
    // Using the successful register write pattern from basic_reg_test (Attempt 3)
    auto write_reg = [&](uint8_t addr, uint8_t data) -> bool {
        printf("Writing register 0x%02X = 0x%02X\n", addr, data);

        bool verify_result = false;

        // 1. First set up address and data with CS=1, WE=1
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = addr;
        dut->data_i = data;

        // Run several clock cycles with clk_en_i pulses before starting write
        for (int i = 0; i < 16; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // 2. Assert CS with WE still high
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        
        // Run several clock cycles with clk_en_i pulses
        for (int i = 0; i < 8; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // 3. Assert WE (WE goes low)
        dut->cs_n_i = 0;
        dut->we_n_i = 0;
        
        // Run several clock cycles with clk_en_i pulses while WE is low
        for (int i = 0; i < 16; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses during write
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // 4. De-assert WE first
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        
        // Run several clock cycles with clk_en_i pulses
        for (int i = 0; i < 8; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // 5. De-assert CS
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        
        // Run more cycles with clk_en_i pulses for the write to be processed
        for (int i = 0; i < 20; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // Verify register write - read back the value
        printf("  Verifying register 0x%02X...\n", addr);

        // 1. First set up address with CS=1, WE=1
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = addr;
        
        // Run several clock cycles with clk_en_i pulses before starting read
        for (int i = 0; i < 8; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // 2. Assert CS with WE high for read
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        
        // Run several clock cycles with clk_en_i pulses during read
        for (int i = 0; i < 32; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }

        // Check read value
        printf("  Register 0x%02X, expected=0x%02X, actual=0x%02X %s\n",
               addr, data, dut->data_o,
               (dut->data_o == data) ? "[OK]" : "[FAILED]");
               
        verify_result = (dut->data_o == data);

        // De-assert CS
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        
        // Run more cycles with clk_en_i pulses after read
        for (int i = 0; i < 8; i++) {
            dut->clk_i = !dut->clk_i;
            
            // Keep generating clk_en_i pulses
            if (dut->clk_i) { // Only on rising edge
                rising_edges++;
                if (rising_edges == 4) { // Every 4th rising edge
                    dut->clk_en_i = 1;
                    rising_edges = 0;
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }
            
            dut->eval();
            tfp->dump(cycle++);
        }
        
        return verify_result;
    };
    
    printf("Configuring DOC5503 oscillators...\n");
    
    // Enable 1 oscillator (sets osc_max_w to 1)
    if (!write_reg(0xE1, 0x02)) {
        printf("ERROR: Failed to set oscillator enable register (0xE1)!\n");
        return 1;
    }
    
    // Setup oscillator 0
    uint8_t osc_num = 0;
    
    // For a clear sine wave, use a moderate frequency value
    // Frequency = 0x0100 (moderate value for clearer wave)
    if (!write_reg(0x00 + osc_num, 0x00)) {
        printf("WARNING: Failed to set frequency low register!\n");
    }
    
    if (!write_reg(0x20 + osc_num, 0x01)) {
        printf("WARNING: Failed to set frequency high register!\n");
    }
    
    // Maximum volume for clearer output
    if (!write_reg(0x40 + osc_num, 0xFF)) {
        printf("WARNING: Failed to set volume register!\n");
    }
    
    // Wave table pointer set to 0
    if (!write_reg(0x80 + osc_num, 0x00)) {
        printf("WARNING: Failed to set waveform table page register!\n");
    }
    
    // Control register: channel assignment (bits 7:4), mode (bits 2:1), halt (bit 0)
    // Send to channel 1 (left channel), free-run mode (00), not halted (0)
    if (!write_reg(0xA0 + osc_num, 0x10)) {
        printf("WARNING: Failed to set control register!\n");
    }
    
    // Resolution/Table size: 256 bytes table with standard resolution
    if (!write_reg(0xC0 + osc_num, 0x00)) {
        printf("WARNING: Failed to set table size/resolution register!\n");
    }
    
    printf("\nOscillator configuration complete.\n");
    printf("  Frequency: 0x0100 (moderate for clearer wave)\n");
    printf("  Volume: 0xFF (maximum)\n");
    printf("  Channel: Left (1)\n");
    printf("  Mode: Free-run\n");
    printf("  Table size: 256 bytes\n");
    
    // Run simulation
    const int MAX_CYCLES = 50000;  // Run longer to get a clean sine wave
    const int SAMPLES_TO_COLLECT = 10000;
    
    int16_t left_samples[SAMPLES_TO_COLLECT] = {0};
    int16_t right_samples[SAMPLES_TO_COLLECT] = {0};
    int sample_count = 0;
    
    printf("\nRunning simulation for %d cycles...\n", MAX_CYCLES);
    
    for (int i = 0; i < MAX_CYCLES && sample_count < SAMPLES_TO_COLLECT; i++) {
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        // Generate DOC clock enable every 8 cycles (4 rising edges)
        if (dut->clk_i) { // On rising edges only
            rising_edges++;
            if (rising_edges == 4) { // Every 4th rising edge
                dut->clk_en_i = 1;
                rising_edges = 0;
            } else {
                dut->clk_en_i = 0;
            }
            
            // When the DOC requests wave data, provide it from our table
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o & 0xFF;
                uint8_t wave_value = wave_table[addr];
                
                dut->wave_data_i = wave_value;
                dut->wave_data_ready_i = 1;
                
                // Print wave reads at regular intervals for debugging
                if (i < 1000 || i % 1000 == 0) {
                    printf("Cycle %d: Wave read at addr=0x%04X, value=0x%02X, osc_state=%d, curr_osc=%d\n",
                           i, addr, wave_value, dut->dbg_osc_state_o, dut->dbg_cycle_o);
                }
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio samples after some warmup time
            if (dut->clk_en_i && i > 1000 && sample_count < SAMPLES_TO_COLLECT) {
                left_samples[sample_count] = dut->left_mix_o;
                right_samples[sample_count] = dut->right_mix_o;
                
                // Print samples at regular intervals
                if (sample_count < 10 || sample_count % 1000 == 0) {
                    printf("Sample %d: left=%d, right=%d, osc_state=%d, curr_osc=%d\n",
                           sample_count, dut->left_mix_o, dut->right_mix_o, 
                           dut->dbg_osc_state_o, dut->dbg_cycle_o);
                }
                
                sample_count++;
            }
        } else {
            dut->clk_en_i = 0; // Always 0 on falling edges
            dut->wave_data_ready_i = 0;
        }
        
        // Evaluate model
        dut->eval();
        
        // Dump waveform
        tfp->dump(cycle++);
    }
    
    printf("\nCollected %d samples.\n", sample_count);
    
    // Analyze the samples
    int16_t left_min = 32767, left_max = -32768;
    int16_t right_min = 32767, right_max = -32768;
    int left_nonzero = 0, right_nonzero = 0;
    
    for (int i = 0; i < sample_count; i++) {
        // Left channel stats
        if (left_samples[i] < left_min) left_min = left_samples[i];
        if (left_samples[i] > left_max) left_max = left_samples[i];
        if (left_samples[i] != 0) left_nonzero++;
        
        // Right channel stats
        if (right_samples[i] < right_min) right_min = right_samples[i];
        if (right_samples[i] > right_max) right_max = right_samples[i];
        if (right_samples[i] != 0) right_nonzero++;
    }
    
    printf("\nSample Analysis:\n");
    printf("Left Channel: %d non-zero samples (%.1f%%)\n", 
           left_nonzero, (100.0 * left_nonzero) / sample_count);
    printf("  Range: %d to %d (span=%d)\n", left_min, left_max, left_max - left_min);
    
    printf("Right Channel: %d non-zero samples (%.1f%%)\n", 
           right_nonzero, (100.0 * right_nonzero) / sample_count);
    printf("  Range: %d to %d (span=%d)\n", right_min, right_max, right_max - right_min);
    
    // Count positive and negative samples to check polarity
    if (left_nonzero > 0) {
        int positive_samples = 0, negative_samples = 0;
        for (int i = 0; i < sample_count; i++) {
            if (left_samples[i] > 0) positive_samples++;
            else if (left_samples[i] < 0) negative_samples++;
        }
        
        printf("\nLeft Channel Polarity Analysis:\n");
        printf("  Positive samples: %d (%.1f%%)\n", 
               positive_samples, (100.0 * positive_samples) / left_nonzero);
        printf("  Negative samples: %d (%.1f%%)\n", 
               negative_samples, (100.0 * negative_samples) / left_nonzero);
        
        if (positive_samples > 0 && negative_samples > 0) {
            printf("  BIPOLAR AUDIO DETECTED (both positive and negative values)\n");
        } else if (positive_samples > 0) {
            printf("  UNIPOLAR POSITIVE AUDIO DETECTED\n");
        } else if (negative_samples > 0) {
            printf("  UNIPOLAR NEGATIVE AUDIO DETECTED\n");
        }
    }
    
    // Write WAV files if we have audio
    if (left_nonzero > 0) {
        write_wav_file("doc5503_sine_left_fixed.wav", left_samples, sample_count);
    }
    
    if (right_nonzero > 0) {
        write_wav_file("doc5503_sine_right_fixed.wav", right_samples, sample_count);
    }
    
    // Final result
    if (left_nonzero > 0 || right_nonzero > 0) {
        printf("\nSUCCESS: Audio output detected! Check the WAV files.\n");
    } else {
        printf("\nFAILURE: No audio output detected.\n");
    }
    
    // Clean up
    tfp->close();
    delete tfp;
    delete dut;
    
    return 0;
}