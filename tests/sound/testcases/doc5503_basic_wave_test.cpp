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
    
    // Enable trace dump if debugging
    VerilatedVcdC* tfp = nullptr;
    bool dump_traces = false;
    
    if (dump_traces) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);
        tfp->open("waveform.vcd");
    }
    
    printf("=== DOC5503 Basic Wave Test ===\n");
    printf("Testing if the DOC5503 produces audio output with the fixed DSP multiplier.\n\n");
    
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
    
    // Show a few values from the wave table
    printf("Wave table sample values: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", wave_table[i]);
    }
    printf("...\n\n");
    
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
    
    // Reset for several cycles
    for (int i = 0; i < 10; i++) {
        dut->clk_i = !dut->clk_i;
        dut->eval();
        if (dump_traces) tfp->dump(i);
    }
    
    dut->reset_n_i = 1;
    
    // Setup DOC registers
    int cycle = 10;
    
    // Helper to write a register
    auto write_reg = [&](uint8_t addr, uint8_t data) {
        // Start write cycle
        dut->clk_i = 0;
        dut->eval();
        if (dump_traces) tfp->dump(cycle++);
        
        // Setup signals
        dut->cs_n_i = 0;
        dut->we_n_i = 0;
        dut->addr_i = addr;
        dut->data_i = data;
        
        // Clock high
        dut->clk_i = 1;
        dut->eval();
        if (dump_traces) tfp->dump(cycle++);
        
        // Clock low
        dut->clk_i = 0;
        dut->eval();
        if (dump_traces) tfp->dump(cycle++);
        
        // Release
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->eval();
        if (dump_traces) tfp->dump(cycle++);
    };
    
    printf("Configuring DOC5503...\n");
    
    // Enable 1 oscillator
    write_reg(0xE1, 0x02);
    
    // Setup oscillator 0
    uint8_t osc_num = 0;
    write_reg(0x00 + osc_num, 0x10);      // Frequency low
    write_reg(0x20 + osc_num, 0x01);      // Frequency high = 0x0110
    write_reg(0x40 + osc_num, 0xFF);      // Volume = maximum
    write_reg(0x80 + osc_num, 0x00);      // Waveform table page = 0
    write_reg(0xA0 + osc_num, 0x00);      // Control = channel 0, free-run, not halted
    write_reg(0xC0 + osc_num, 0x00);      // Table size/resolution = 256 bytes
    
    printf("Setup oscillator with:\n");
    printf("  Frequency: 0x0110\n");
    printf("  Volume: 0xFF (maximum)\n");
    printf("  Channel: Right (0)\n");
    
    // Run simulation
    const int MAX_CYCLES = 20000;
    const int SAMPLES_TO_COLLECT = 5000;
    const int DOC_CLK_DIVIDER = 8;   // DOC clock enable every 8 cycles
    
    int16_t left_samples[SAMPLES_TO_COLLECT] = {0};
    int16_t right_samples[SAMPLES_TO_COLLECT] = {0};
    int sample_count = 0;
    
    printf("\nRunning simulation for %d cycles...\n", MAX_CYCLES);
    
    for (int i = 0; i < MAX_CYCLES && sample_count < SAMPLES_TO_COLLECT; i++) {
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        if (dut->clk_i) { // Rising edge
            // Generate DOC clock enable every 8 cycles
            dut->clk_en_i = (i % DOC_CLK_DIVIDER == 0);
            
            // When the DOC requests wave data, provide it from our table
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o & 0xFF;
                uint8_t wave_value = wave_table[addr];
                
                // Ensure we NEVER provide a zero byte
                if (wave_value == 0) wave_value = 1;
                
                dut->wave_data_i = wave_value;
                dut->wave_data_ready_i = 1;
                
                // Print some wave reads for debugging
                if (i < 100 || i % 1000 == 0) {
                    printf("Cycle %d: Wave read at addr=0x%04X, providing value=0x%02X\n", 
                           i, addr, wave_value);
                }
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio samples after some warmup time
            if (dut->clk_en_i && i > 500 && sample_count < SAMPLES_TO_COLLECT) {
                left_samples[sample_count] = dut->left_mix_o;
                right_samples[sample_count] = dut->right_mix_o;
                
                // Print some samples for debugging
                if (sample_count < 10 || sample_count % 500 == 0) {
                    printf("Sample %d: left=%d, right=%d\n",
                           sample_count, dut->left_mix_o, dut->right_mix_o);
                }
                
                sample_count++;
            }
        }
        
        // Evaluate model
        dut->eval();
        
        // Dump waveform if enabled
        if (dump_traces) tfp->dump(cycle++);
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
    if (right_nonzero > 0) {
        int positive_samples = 0, negative_samples = 0;
        for (int i = 0; i < sample_count; i++) {
            if (right_samples[i] > 0) positive_samples++;
            else if (right_samples[i] < 0) negative_samples++;
        }
        
        printf("\nRight Channel Polarity Analysis:\n");
        printf("  Positive samples: %d (%.1f%%)\n", 
               positive_samples, (100.0 * positive_samples) / right_nonzero);
        printf("  Negative samples: %d (%.1f%%)\n", 
               negative_samples, (100.0 * negative_samples) / right_nonzero);
        
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
        write_wav_file("doc_left_output.wav", left_samples, sample_count);
    }
    
    if (right_nonzero > 0) {
        write_wav_file("doc_right_output.wav", right_samples, sample_count);
    }
    
    // Final result
    if (left_nonzero > 0 || right_nonzero > 0) {
        printf("\nSUCCESS: Audio output detected! Check the WAV files.\n");
    } else {
        printf("\nFAILURE: No audio output detected.\n");
    }
    
    // Clean up
    if (dump_traces) {
        tfp->close();
        delete tfp;
    }
    
    delete dut;
    return 0;
}