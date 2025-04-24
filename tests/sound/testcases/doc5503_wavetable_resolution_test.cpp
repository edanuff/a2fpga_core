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

// WAV file output functions
void write_wav_file(const char* filename, const std::vector<int16_t>& samples, int sample_rate) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("ERROR: Could not open %s for writing\n", filename);
        return;
    }
    
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

// Helper function to write a register to the DOC
void write_register(Vdoc5503_harness* doc_harness, uint8_t addr, uint8_t data, int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    // Write register on rising edge
    clk = true;
    doc_harness->clk_i = clk;
    doc_harness->cs_n_i = 0;
    doc_harness->we_n_i = 0;
    doc_harness->addr_i = addr;
    doc_harness->data_i = data;
    doc_harness->eval();
    if (dump_trace) tfp->dump(sim_time++);
    
    // Change clock to falling edge
    clk = false;
    doc_harness->clk_i = clk;
    doc_harness->eval();
    if (dump_trace) tfp->dump(sim_time++);
    
    // Reset control signals
    doc_harness->cs_n_i = 1;
    doc_harness->we_n_i = 1;
}

// Reset the DOC
void reset_doc(Vdoc5503_harness* doc_harness, int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    doc_harness->reset_n_i = 0;
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_trace) tfp->dump(sim_time++);
    }
    
    doc_harness->reset_n_i = 1;
}

// Generate a wavetable with different patterns for testing
void generate_test_wavetable(uint8_t* wavetable, size_t length, int pattern_type) {
    switch (pattern_type) {
        case 0: // Sine wave
            for (size_t i = 0; i < length; i++) {
                double angle = (double)i / length * 2.0 * M_PI;
                double value = sin(angle);
                wavetable[i] = (uint8_t)(value * 127.0 + 128.0);
            }
            break;
            
        case 1: // Sawtooth wave
            for (size_t i = 0; i < length; i++) {
                wavetable[i] = (uint8_t)((i * 256) / length);
            }
            break;
            
        case 2: // Square wave
            for (size_t i = 0; i < length; i++) {
                wavetable[i] = (i < length / 2) ? 255 : 0;
            }
            break;
            
        case 3: // Triangle wave
            for (size_t i = 0; i < length; i++) {
                double position = (double)i / length;
                double value = (position < 0.5) ? 
                    (position * 2.0) : (1.0 - (position - 0.5) * 2.0);
                wavetable[i] = (uint8_t)(value * 255.0);
            }
            break;
            
        case 4: // Ramp pattern (for resolution testing)
            for (size_t i = 0; i < length; i++) {
                // Create a pattern that's easy to see in different resolutions
                // For each 256-byte page, we'll have a different constant value
                wavetable[i] = 0x80 + ((i / 256) * 16);
            }
            break;
    }
}

// Main function to test different wavetable sizes and resolutions
int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the DOC5503 model
    Vdoc5503_harness* doc_harness = new Vdoc5503_harness;
    
    // Set up tracing
    VerilatedVcdC* tfp = nullptr;
    bool dump_trace = false;
    
    if (dump_trace) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        doc_harness->trace(tfp, 99);
        tfp->open("doc5503_wavetable_test.vcd");
    }
    
    // Clock generation
    bool clk = false;
    
    // Initialize inputs
    doc_harness->reset_n_i = 0;
    doc_harness->clk_i = clk;
    doc_harness->clk_en_i = 0;
    doc_harness->cs_n_i = 1;
    doc_harness->we_n_i = 1;
    doc_harness->addr_i = 0;
    doc_harness->data_i = 0;
    doc_harness->wave_data_ready_i = 0;
    doc_harness->wave_data_i = 0;
    
    // Reset for initial state
    int sim_time = 0;
    reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
    
    // Generate test wavetable
    const size_t MAX_WAVETABLE_SIZE = 32768;  // Maximum DOC wavetable size
    uint8_t* test_wavetable = new uint8_t[MAX_WAVETABLE_SIZE];
    
    // Build a special pattern that makes it easy to see resolution changes
    generate_test_wavetable(test_wavetable, MAX_WAVETABLE_SIZE, 4);
    
    printf("\n===== TESTING DOC5503 WAVETABLE SIZE AND RESOLUTION =====\n");
    
    // Test different table sizes and resolutions
    const int NUM_TABLE_SIZES = 8;  // 8 possible table sizes (3 bits)
    const int NUM_RESOLUTIONS = 8;  // 8 possible resolutions (3 bits)
    
    // We'll run 3 series of tests:
    // 1. Fixed resolution (0), variable table size
    // 2. Fixed table size (3 = 2048 bytes), variable resolution
    // 3. Matching table size and resolution (recommended usage according to docs)
    
    // Store samples for generating WAV files
    std::vector<std::vector<int16_t>> size_test_samples(NUM_TABLE_SIZES);
    std::vector<std::vector<int16_t>> res_test_samples(NUM_RESOLUTIONS);
    std::vector<std::vector<int16_t>> matched_test_samples(NUM_TABLE_SIZES);
    
    // === TEST 1: Different table sizes with fixed resolution ===
    printf("\n--- TEST 1: Different table sizes with fixed resolution 0 ---\n");
    
    for (int table_size = 0; table_size < NUM_TABLE_SIZES; table_size++) {
        // Reset for a clean state
        reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
        
        printf("\nTesting table size %d (", table_size);
        
        // Calculate actual size in bytes
        int actual_size_bytes = 256 << table_size;  // 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
        printf("%d bytes)...\n", actual_size_bytes);
        
        // Enable one oscillator
        write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
        
        // Setup oscillator parameters
        uint8_t osc_num = 0;
        uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
        uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
        uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
        uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
        uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
        uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
        
        // Configure with moderate frequency, maximum volume, free running
        write_register(doc_harness, addr_freq_low, 0x40, sim_time, clk, tfp, dump_trace);   // Low frequency for clear output
        write_register(doc_harness, addr_freq_high, 0x00, sim_time, clk, tfp, dump_trace);  // ... across all table sizes
        write_register(doc_harness, addr_volume, 0xFF, sim_time, clk, tfp, dump_trace);     // Maximum volume
        write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);   // Start at page 0
        write_register(doc_harness, addr_control, 0x10, sim_time, clk, tfp, dump_trace);    // Free running, channel 1
        
        // Set resolution to 0 (standard) and table size based on test
        uint8_t res_table_val = (table_size << 3) | 0;  // Table size in bits 5-3, resolution in bits 2-0
        write_register(doc_harness, addr_res_table, res_table_val, sim_time, clk, tfp, dump_trace);
        
        printf("Running with table size bits=%d (res_table=0x%02X)...\n", table_size, res_table_val);
        
        // Collect samples for this test
        std::vector<int16_t>& samples = size_test_samples[table_size];
        const size_t SAMPLE_COUNT = 5000;  // Number of samples to collect
        
        // Run simulation and collect samples
        for (int cycle = 0; cycle < SAMPLE_COUNT * 10; cycle++) {
            // Toggle clock
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                // Generate DOC clk_en_i pulse every 8 cycles
                bool doc_clk_en = ((cycle % 8) == 0);
                doc_harness->clk_en_i = doc_clk_en;
                
                // When the DOC requests wave data, provide it from our wavetable
                if (doc_harness->wave_rd_o) {
                    // Calculate correct address based on table size
                    uint32_t addr_mask = actual_size_bytes - 1;
                    uint16_t wave_addr = doc_harness->wave_address_o & addr_mask;
                    
                    doc_harness->wave_data_i = test_wavetable[wave_addr];
                    doc_harness->wave_data_ready_i = 1;
                    
                    // Log a sample of the addresses being accessed
                    if (cycle < 200 && cycle % 50 == 0) {
                        printf("Cycle %d: Wave read from address 0x%04X, value=0x%02X\n", 
                            cycle, wave_addr, doc_harness->wave_data_i);
                    }
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                // Collect samples after warmup
                if (doc_clk_en && cycle >= 500 && samples.size() < SAMPLE_COUNT) {
                    samples.push_back(doc_harness->left_mix_o);
                    
                    // Log some sample values
                    if (samples.size() % 1000 == 0) {
                        printf("Collected %zu samples, last value: %d\n", 
                            samples.size(), doc_harness->left_mix_o);
                    }
                }
                
                // Stop when we've collected enough samples
                if (samples.size() >= SAMPLE_COUNT) {
                    printf("Collected %zu samples with table size %d\n", 
                        samples.size(), table_size);
                    break;
                }
            }
            
            // Evaluate model
            doc_harness->eval();
            
            // Write trace if enabled
            if (dump_trace) tfp->dump(sim_time++);
        }
        
        // Write a WAV file for this test
        char filename[64];
        sprintf(filename, "doc_tsize_%d.wav", table_size);
        write_wav_file(filename, samples, 22050);
    }
    
    // === TEST 2: Different resolutions with fixed table size ===
    printf("\n--- TEST 2: Different resolutions with fixed table size 3 (2048 bytes) ---\n");
    
    for (int resolution = 0; resolution < NUM_RESOLUTIONS; resolution++) {
        // Reset for a clean state
        reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
        
        printf("\nTesting resolution %d (accum bits %d-%d)...\n", 
            resolution, 1 + resolution, 16 + resolution);
        
        // Enable one oscillator
        write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
        
        // Setup oscillator parameters
        uint8_t osc_num = 0;
        uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
        uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
        uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
        uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
        uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
        uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
        
        // Fixed frequency across all tests (adjusted for resolution)
        uint16_t freq_value = 0x80 << resolution;  // Scale frequency based on resolution
        
        write_register(doc_harness, addr_freq_low, freq_value & 0xFF, sim_time, clk, tfp, dump_trace);
        write_register(doc_harness, addr_freq_high, (freq_value >> 8) & 0xFF, sim_time, clk, tfp, dump_trace);
        write_register(doc_harness, addr_volume, 0xFF, sim_time, clk, tfp, dump_trace);     // Maximum volume
        write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);   // Start at page 0
        write_register(doc_harness, addr_control, 0x10, sim_time, clk, tfp, dump_trace);    // Free running, channel 1
        
        // Fixed table size of 3 (2048 bytes), variable resolution
        uint8_t res_table_val = (3 << 3) | resolution;  // Table size 3 in bits 5-3, variable resolution in bits 2-0
        write_register(doc_harness, addr_res_table, res_table_val, sim_time, clk, tfp, dump_trace);
        
        printf("Running with freq=0x%04X, res_table=0x%02X...\n", freq_value, res_table_val);
        
        // Collect samples for this test
        std::vector<int16_t>& samples = res_test_samples[resolution];
        const size_t SAMPLE_COUNT = 5000;  // Number of samples to collect
        
        // Run simulation and collect samples
        for (int cycle = 0; cycle < SAMPLE_COUNT * 10; cycle++) {
            // Toggle clock
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                // Generate DOC clk_en_i pulse every 8 cycles
                bool doc_clk_en = ((cycle % 8) == 0);
                doc_harness->clk_en_i = doc_clk_en;
                
                // When the DOC requests wave data, provide it from our wavetable
                if (doc_harness->wave_rd_o) {
                    // Fixed table size of 2048 bytes
                    uint16_t wave_addr = doc_harness->wave_address_o & 0x7FF;
                    
                    doc_harness->wave_data_i = test_wavetable[wave_addr];
                    doc_harness->wave_data_ready_i = 1;
                    
                    // Log a sample of the addresses being accessed
                    if (cycle < 200 && cycle % 50 == 0) {
                        printf("Cycle %d: Wave read from address 0x%04X, value=0x%02X\n", 
                            cycle, wave_addr, doc_harness->wave_data_i);
                    }
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                // Collect samples after warmup
                if (doc_clk_en && cycle >= 500 && samples.size() < SAMPLE_COUNT) {
                    samples.push_back(doc_harness->left_mix_o);
                    
                    // Log some sample values
                    if (samples.size() % 1000 == 0) {
                        printf("Collected %zu samples, last value: %d\n", 
                            samples.size(), doc_harness->left_mix_o);
                    }
                }
                
                // Stop when we've collected enough samples
                if (samples.size() >= SAMPLE_COUNT) {
                    printf("Collected %zu samples with resolution %d\n", 
                        samples.size(), resolution);
                    break;
                }
            }
            
            // Evaluate model
            doc_harness->eval();
            
            // Write trace if enabled
            if (dump_trace) tfp->dump(sim_time++);
        }
        
        // Write a WAV file for this test
        char filename[64];
        sprintf(filename, "doc_res_%d.wav", resolution);
        write_wav_file(filename, samples, 22050);
    }
    
    // === TEST 3: Matching table size and resolution (recommended usage) ===
    printf("\n--- TEST 3: Matching table size and resolution settings ---\n");
    
    for (int size_res = 0; size_res < NUM_TABLE_SIZES; size_res++) {
        // Reset for a clean state
        reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
        
        // Calculate actual size in bytes
        int actual_size_bytes = 256 << size_res;  // 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
        
        printf("\nTesting matched table size and resolution %d (%d bytes)...\n", 
            size_res, actual_size_bytes);
        
        // Enable one oscillator
        write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
        
        // Setup oscillator parameters
        uint8_t osc_num = 0;
        uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
        uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
        uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
        uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
        uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
        uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
        
        // Use a fixed frequency value that works well with all sizes
        write_register(doc_harness, addr_freq_low, 0x40, sim_time, clk, tfp, dump_trace);   // Low byte
        write_register(doc_harness, addr_freq_high, 0x00, sim_time, clk, tfp, dump_trace);  // High byte
        write_register(doc_harness, addr_volume, 0xFF, sim_time, clk, tfp, dump_trace);     // Maximum volume
        write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);   // Start at page 0
        write_register(doc_harness, addr_control, 0x10, sim_time, clk, tfp, dump_trace);    // Free running, channel 1
        
        // Match table size and resolution as recommended
        uint8_t res_table_val = (size_res << 3) | size_res;  // Same value for both fields
        write_register(doc_harness, addr_res_table, res_table_val, sim_time, clk, tfp, dump_trace);
        
        printf("Running with table size and resolution both=%d (res_table=0x%02X)...\n", 
            size_res, res_table_val);
        
        // Collect samples for this test
        std::vector<int16_t>& samples = matched_test_samples[size_res];
        const size_t SAMPLE_COUNT = 5000;  // Number of samples to collect
        
        // Run simulation and collect samples
        for (int cycle = 0; cycle < SAMPLE_COUNT * 10; cycle++) {
            // Toggle clock
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                // Generate DOC clk_en_i pulse every 8 cycles
                bool doc_clk_en = ((cycle % 8) == 0);
                doc_harness->clk_en_i = doc_clk_en;
                
                // When the DOC requests wave data, provide it from our wavetable
                if (doc_harness->wave_rd_o) {
                    // Calculate correct address based on table size
                    uint32_t addr_mask = actual_size_bytes - 1;
                    uint16_t wave_addr = doc_harness->wave_address_o & addr_mask;
                    
                    doc_harness->wave_data_i = test_wavetable[wave_addr];
                    doc_harness->wave_data_ready_i = 1;
                    
                    // Log a sample of the addresses being accessed
                    if (cycle < 200 && cycle % 50 == 0) {
                        printf("Cycle %d: Wave read from address 0x%04X, value=0x%02X\n", 
                            cycle, wave_addr, doc_harness->wave_data_i);
                    }
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                // Collect samples after warmup
                if (doc_clk_en && cycle >= 500 && samples.size() < SAMPLE_COUNT) {
                    samples.push_back(doc_harness->left_mix_o);
                    
                    // Log some sample values
                    if (samples.size() % 1000 == 0) {
                        printf("Collected %zu samples, last value: %d\n", 
                            samples.size(), doc_harness->left_mix_o);
                    }
                }
                
                // Stop when we've collected enough samples
                if (samples.size() >= SAMPLE_COUNT) {
                    printf("Collected %zu samples with matched size/res %d\n", 
                        samples.size(), size_res);
                    break;
                }
            }
            
            // Evaluate model
            doc_harness->eval();
            
            // Write trace if enabled
            if (dump_trace) tfp->dump(sim_time++);
        }
        
        // Write a WAV file for this test
        char filename[64];
        sprintf(filename, "doc_matched_%d.wav", size_res);
        write_wav_file(filename, samples, 22050);
    }
    
    // Print summary
    printf("\n===== WAVETABLE SIZE AND RESOLUTION TEST SUMMARY =====\n\n");
    
    printf("TEST 1: Different table sizes with fixed resolution 0\n");
    for (int i = 0; i < NUM_TABLE_SIZES; i++) {
        int size_bytes = 256 << i;
        printf("  Table size %d (%d bytes): %zu samples in doc_tsize_%d.wav\n", 
            i, size_bytes, size_test_samples[i].size(), i);
    }
    
    printf("\nTEST 2: Different resolutions with fixed table size 3 (2048 bytes)\n");
    for (int i = 0; i < NUM_RESOLUTIONS; i++) {
        printf("  Resolution %d (bits %d-%d): %zu samples in doc_res_%d.wav\n", 
            i, 1 + i, 16 + i, res_test_samples[i].size(), i);
    }
    
    printf("\nTEST 3: Matching table size and resolution settings\n");
    for (int i = 0; i < NUM_TABLE_SIZES; i++) {
        int size_bytes = 256 << i;
        printf("  Matched size/res %d (%d bytes): %zu samples in doc_matched_%d.wav\n", 
            i, size_bytes, matched_test_samples[i].size(), i);
    }
    
    // Cleanup
    if (dump_trace) {
        tfp->close();
        delete tfp;
    }
    
    delete[] test_wavetable;
    delete doc_harness;
    
    printf("\nTest complete. You can now analyze the WAV files to see how different table sizes\n");
    printf("and resolutions affect the audio output.\n");
    
    return 0;
}