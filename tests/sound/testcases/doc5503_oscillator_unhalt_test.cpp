#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required time stamp function for Verilator
double sc_time_stamp() { return 0; }

// Helper function to write a register to the DOC
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

// WAV file output functions
void write_wav_file(const char* filename, std::vector<int16_t>& samples) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("ERROR: Could not open %s for writing\n", filename);
        return;
    }
    
    int sample_rate = 44100;
    size_t num_samples = samples.size();
    
    // WAV header
    const int header_size = 44;
    unsigned char header[header_size] = {
        'R', 'I', 'F', 'F',                                  // RIFF chunk
        0, 0, 0, 0,                                          // Chunk size (filled later)
        'W', 'A', 'V', 'E',                                  // Format
        'f', 'm', 't', ' ',                                  // Format chunk
        16, 0, 0, 0,                                         // Format chunk size
        1, 0,                                                // PCM format
        1, 0,                                                // Mono channel
        0, 0, 0, 0,                                          // Sample rate (filled later)
        0, 0, 0, 0,                                          // Byte rate (filled later)
        2, 0,                                                // Block align (2 bytes per sample)
        16, 0,                                               // Bits per sample
        'd', 'a', 't', 'a',                                  // Data chunk
        0, 0, 0, 0                                           // Data size (filled later)
    };
    
    // Fill in the sample rate
    header[24] = sample_rate & 0xff;
    header[25] = (sample_rate >> 8) & 0xff;
    header[26] = (sample_rate >> 16) & 0xff;
    header[27] = (sample_rate >> 24) & 0xff;
    
    // Fill in the byte rate (sample rate * block align)
    int byte_rate = sample_rate * 2; // 2 bytes per sample
    header[28] = byte_rate & 0xff;
    header[29] = (byte_rate >> 8) & 0xff;
    header[30] = (byte_rate >> 16) & 0xff;
    header[31] = (byte_rate >> 24) & 0xff;
    
    // Fill in the data size
    uint32_t data_size = (uint32_t)num_samples * 2; // 2 bytes per sample
    header[40] = data_size & 0xff;
    header[41] = (data_size >> 8) & 0xff;
    header[42] = (data_size >> 16) & 0xff;
    header[43] = (data_size >> 24) & 0xff;
    
    // Fill in the chunk size
    uint32_t chunk_size = 36 + data_size; // Header size - 8 + data size
    header[4] = chunk_size & 0xff;
    header[5] = (chunk_size >> 8) & 0xff;
    header[6] = (chunk_size >> 16) & 0xff;
    header[7] = (chunk_size >> 24) & 0xff;
    
    // Write header
    fwrite(header, 1, header_size, f);
    
    // Write data
    fwrite(samples.data(), sizeof(int16_t), num_samples, f);
    
    fclose(f);
    printf("Wrote %zu samples to %s\n", num_samples, filename);
}

// Oscillator debugging 
void print_oscillator_state(Vdoc5503_harness* dut, int osc_num) {
    uint8_t wds = read_register(dut, 0x60 + osc_num);
    uint8_t vol = read_register(dut, 0x40 + osc_num);
    uint8_t ctrl = read_register(dut, 0xA0 + osc_num);
    uint8_t freq_lo = read_register(dut, 0x00 + osc_num);
    uint8_t freq_hi = read_register(dut, 0x20 + osc_num);
    
    printf("Oscillator %d State:\n", osc_num);
    printf("  Waveform Data Sample: 0x%02X\n", wds);
    printf("  Volume: 0x%02X\n", vol);
    printf("  Control: 0x%02X (CA=%d, IE=%d, M1=%d, M0=%d, H=%d)\n", 
           ctrl, (ctrl >> 4) & 0x0F, (ctrl >> 3) & 0x01, (ctrl >> 2) & 0x01, (ctrl >> 1) & 0x01, ctrl & 0x01);
    printf("  Frequency: 0x%02X%02X\n", freq_hi, freq_lo);
}

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of DOC5503 model with harness
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
    
    printf("==== DOC5503 OSCILLATOR UNHALT TEST ====\n\n");
    printf("This test addresses the issue with oscillators getting halted by zero bytes\n");
    printf("and verifies that our workaround consistently produces audio output.\n\n");
    
    // Reset system
    for (int i = 0; i < 10; i++) {
        dut->clk_i = !dut->clk_i;
        dut->eval();
    }
    dut->reset_n_i = 1;
    
    // Configure the DOC5503
    printf("Configuring DOC5503...\n");
    
    // Enable one oscillator to simplify debugging
    write_register(dut, 0xE1, 0x02);  // Enable 1 oscillator
    
    // Define oscillator parameters
    uint8_t osc_num = 0;
    
    // Setup oscillator 0
    write_register(dut, 0x00, 0x10);  // Frequency Low
    write_register(dut, 0x20, 0x00);  // Frequency High
    write_register(dut, 0x40, 0xFF);  // Maximum volume
    write_register(dut, 0x80, 0x00);  // Wave table pointer = page 0
    
    // IMPORTANT: Use channel 0 (bit 4=0) to direct output to RIGHT channel
    // Free run mode (bits 2:1=00), not halted (bit 0=0)
    write_register(dut, 0xA0, 0x00);  // Control register
    
    write_register(dut, 0xC0, 0x00);  // 256-byte table, resolution 0
    
    printf("Initial state:\n");
    print_oscillator_state(dut, osc_num);
    
    // Generate sinewave data for testing
    const int wavetable_size = 256;
    uint8_t sinewave[wavetable_size];
    
    printf("\nGenerating proper bipolar sine waveform (centered at 0x80)...\n");
    for (int i = 0; i < wavetable_size; i++) {
        double angle = (double)i / wavetable_size * 2.0 * M_PI;
        double value = sin(angle);
        sinewave[i] = (uint8_t)(value * 127.0 + 128.0);
        
        // Ensure no zero bytes
        if (sinewave[i] == 0) sinewave[i] = 1;
    }
    
    // Print a few values to verify
    printf("Sample waveform values: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", sinewave[i]);
    }
    printf("...\n\n");
    
    // Run simulation and collect audio samples
    const int total_cycles = 10000;
    int clock_divider = 8;  // 8 cycles per DOC clock enable
    std::vector<int16_t> left_samples;
    std::vector<int16_t> right_samples;
    
    printf("Running simulation with workaround for zero byte detection:\n");
    
    // Collect 5 seconds worth of samples (assuming ~44.1kHz)
    for (int cycle = 0; cycle < total_cycles; cycle++) {
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        // On rising edge
        if (dut->clk_i) {
            // Generate DOC clock enable pulse every 8 cycles
            dut->clk_en_i = (cycle % clock_divider == 0);
            
            // When wave read is requested, provide non-zero value from sinewave table
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o & 0xFF;
                
                // CRITICAL: Always provide a non-zero value
                // This prevents the oscillator from halting
                uint8_t wave_val = sinewave[addr];
                if (wave_val == 0x00) wave_val = 0x01;
                
                dut->wave_data_i = wave_val;
                dut->wave_data_ready_i = 1;
                
                if (cycle < 200 || cycle % 1000 == 0) {
                    printf("Cycle %d: Wave read request at addr=0x%04X, providing value=0x%02X\n", 
                           cycle, addr, wave_val);
                }
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio samples periodically
            if (dut->clk_en_i && cycle >= 500 && cycle % 8 == 0) {
                left_samples.push_back(dut->left_mix_o);
                right_samples.push_back(dut->right_mix_o);
                
                // Print some samples for debugging
                if (left_samples.size() <= 10 || left_samples.size() % 500 == 0) {
                    printf("Sample %zu: left=%d, right=%d\n", 
                           left_samples.size(), dut->left_mix_o, dut->right_mix_o);
                }
            }
        }
        
        // Evaluate model
        dut->eval();
    }
    
    // Check oscillator state after run
    printf("\nFinal state:\n");
    print_oscillator_state(dut, osc_num);
    
    // Analyze collected samples
    int non_zero_left = 0;
    int non_zero_right = 0;
    int16_t max_left = 0, max_right = 0;
    
    for (auto sample : left_samples) {
        if (sample != 0) {
            non_zero_left++;
            if (abs(sample) > abs(max_left)) max_left = sample;
        }
    }
    
    for (auto sample : right_samples) {
        if (sample != 0) {
            non_zero_right++;
            if (abs(sample) > abs(max_right)) max_right = sample;
        }
    }
    
    printf("\nSample Analysis:\n");
    printf("  Left Channel: %d/%zu non-zero samples (%.1f%%), max amplitude: %d\n",
           non_zero_left, left_samples.size(), 
           (100.0 * non_zero_left) / left_samples.size(), max_left);
    printf("  Right Channel: %d/%zu non-zero samples (%.1f%%), max amplitude: %d\n",
           non_zero_right, right_samples.size(), 
           (100.0 * non_zero_right) / right_samples.size(), max_right);
    
    if (non_zero_left > 0 || non_zero_right > 0) {
        printf("\nSUCCESS: Audio output detected!\n");
        
        // Save WAV files if we have samples
        if (non_zero_right > 0) {
            write_wav_file("doc_right_output.wav", right_samples);
        }
        if (non_zero_left > 0) {
            write_wav_file("doc_left_output.wav", left_samples);
        }
    } else {
        printf("\nFAILURE: No audio output detected!\n");
        printf("Possible reasons:\n");
        printf("1. Oscillator halt issue is not fixed\n");
        printf("2. Channel routing is incorrect\n");
        printf("3. Output scaling is too low\n");
    }
    
    // Cleanup
    delete dut;
    return non_zero_left > 0 || non_zero_right > 0 ? 0 : 1;
}