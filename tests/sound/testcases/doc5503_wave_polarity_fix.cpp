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

// Generate a proper balanced sine wavetable for the DOC5503
void generate_centered_sine_wavetable(uint8_t* wavetable, size_t length) {
    printf("Generating properly balanced sine wavetable...\n");
    
    // First, calculate the values properly centered around 0x80
    for (size_t i = 0; i < length; i++) {
        double angle = (double)i / length * 2.0 * M_PI;
        double value = sin(angle);
        // Scale to fit in 8-bit range centered at 0x80 (0x01-0xFF)
        wavetable[i] = (uint8_t)(value * 127.0 + 128.0);
        
        // Avoid 0x00 which would halt the oscillator
        if (wavetable[i] == 0x00) wavetable[i] = 0x01;
    }
    
    // Verify the waveform is correct and doesn't contain 0x00
    int min_val = 255, max_val = 0;
    for (size_t i = 0; i < length; i++) {
        if (wavetable[i] < min_val) min_val = wavetable[i];
        if (wavetable[i] > max_val) max_val = wavetable[i];
    }
    
    printf("Wavetable stats: min=%d, max=%d, range=%d\n", min_val, max_val, max_val - min_val);
    printf("Wavetable center: %.1f (ideal is 128.0)\n", (min_val + max_val) / 2.0);
    
    // Print some sample values
    printf("Sample values: [");
    for (size_t i = 0; i < length; i += length/16) {
        printf("%d", wavetable[i]);
        if (i < length - length/16) printf(", ");
    }
    printf("]\n");
}

// Generate audio statistics
void analyze_audio_stats(const std::vector<int16_t>& samples, const char* label) {
    int16_t min_val = 32767;
    int16_t max_val = -32768;
    double avg_val = 0.0;
    double rms_val = 0.0;
    int positive_count = 0;
    int negative_count = 0;
    int zero_count = 0;
    
    for (auto sample : samples) {
        if (sample < min_val) min_val = sample;
        if (sample > max_val) max_val = sample;
        avg_val += sample;
        
        if (sample > 0) positive_count++;
        else if (sample < 0) negative_count++;
        else zero_count++;
        
        double sample_normalized = sample / 32768.0;
        rms_val += sample_normalized * sample_normalized;
    }
    
    avg_val /= samples.size();
    rms_val = sqrt(rms_val / samples.size());
    
    printf("\n=== %s ===\n", label);
    printf("Total samples: %zu\n", samples.size());
    printf("Range: min=%d, max=%d (span=%d)\n", min_val, max_val, max_val - min_val);
    printf("Average value: %.2f\n", avg_val);
    printf("RMS level: %.4f (%.2f dB)\n", rms_val, 20.0 * log10(rms_val));
    printf("Positive samples: %d (%.1f%%)\n", 
        positive_count, (positive_count * 100.0) / samples.size());
    printf("Negative samples: %d (%.1f%%)\n", 
        negative_count, (negative_count * 100.0) / samples.size());
    printf("Zero samples: %d (%.1f%%)\n", 
        zero_count, (zero_count * 100.0) / samples.size());
    
    // Analyze polarity
    if (positive_count > 0 && negative_count > 0) {
        printf("POLARITY: Bipolar signal (both positive and negative values)\n");
        printf("Ratio of positive to negative: %.2f:1\n", 
               (double)positive_count / negative_count);
    } else if (positive_count > 0) {
        printf("POLARITY: Unipolar POSITIVE signal\n");
    } else if (negative_count > 0) {
        printf("POLARITY: Unipolar NEGATIVE signal\n");
    } else {
        printf("POLARITY: No signal (all zeros)\n");
    }
    
    // Check for good audio levels
    double peak_level_db = 20.0 * log10((double)std::max(abs(min_val), abs(max_val)) / 32768.0);
    printf("Peak level: %.2f dB\n", peak_level_db);
}

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
        tfp->open("doc5503_wave_polarity_fix.vcd");
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
    
    printf("\n===== TESTING DOC5503 POLARITY FIX =====\n");
    
    // Generate proper balanced sine wavetable
    const size_t WAVETABLE_SIZE = 256;
    uint8_t sine_wavetable[WAVETABLE_SIZE];
    generate_centered_sine_wavetable(sine_wavetable, WAVETABLE_SIZE);
    
    // Test with different oscillator and mixer phase combinations
    const int NUM_PHASE_TESTS = 8;
    std::vector<std::vector<int16_t>> phase_test_samples(NUM_PHASE_TESTS);
    
    for (int phase_test = 0; phase_test < NUM_PHASE_TESTS; phase_test++) {
        // Use different starting phases in the wavetable
        uint16_t start_offset = (WAVETABLE_SIZE * phase_test) / NUM_PHASE_TESTS;
        
        // Reset for a clean state
        reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
        
        printf("\nPhase test %d - Starting at wavetable offset 0x%02X...\n", 
               phase_test, start_offset);
        
        // Enable one oscillator
        write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
        
        // Channel assignment - alternate between left and right
        uint8_t channel_assignment = (phase_test % 2) << 4;  // 0x00 = right, 0x10 = left
        
        // Setup oscillator parameters
        uint8_t osc_num = 0;
        uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
        uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
        uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
        uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
        uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
        uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
        
        // Use a moderate frequency
        write_register(doc_harness, addr_freq_low, 0x80, sim_time, clk, tfp, dump_trace);
        write_register(doc_harness, addr_freq_high, 0x01, sim_time, clk, tfp, dump_trace);
        
        // Alternate between normal volume (64) and high volume (255)
        uint8_t test_volume = (phase_test % 4 < 2) ? 64 : 255;
        write_register(doc_harness, addr_volume, test_volume, sim_time, clk, tfp, dump_trace);
        
        // Standard parameters
        write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);   // Page 0
        write_register(doc_harness, addr_control, channel_assignment, sim_time, clk, tfp, dump_trace);
        write_register(doc_harness, addr_res_table, 0x00, sim_time, clk, tfp, dump_trace);  // 256-byte table
        
        // Collect samples for this test
        std::vector<int16_t>& samples = phase_test_samples[phase_test];
        const size_t SAMPLE_COUNT = 5000;  // Number of samples to collect
        
        // Keep track of current wave address to properly sequence the wavetable
        uint16_t current_wave_addr = start_offset;
        
        // Run simulation and collect samples
        for (int cycle = 0; cycle < SAMPLE_COUNT * 10; cycle++) {
            // Toggle clock
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                // Generate DOC clk_en_i pulse every 8 cycles
                bool doc_clk_en = ((cycle % 8) == 0);
                doc_harness->clk_en_i = doc_clk_en;
                
                // When the DOC requests wave data, provide it from our sine wavetable
                if (doc_harness->wave_rd_o) {
                    // Use the wave address to calculate the index, but start at our offset
                    uint16_t wave_addr = doc_harness->wave_address_o & 0xFF;
                    current_wave_addr = (wave_addr + start_offset) % WAVETABLE_SIZE;
                    
                    doc_harness->wave_data_i = sine_wavetable[current_wave_addr];
                    doc_harness->wave_data_ready_i = 1;
                    
                    // Debug print first few reads
                    if (cycle < 100 || cycle % 1000 == 0) {
                        printf("Cycle %d: Wave read request at addr=0x%04X, provided value=0x%02X\n", 
                               cycle, wave_addr, doc_harness->wave_data_i);
                    }
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                // Collect samples after warmup
                if (doc_clk_en && cycle >= 500 && samples.size() < SAMPLE_COUNT) {
                    // Collect from the appropriate channel based on test configuration
                    if (channel_assignment == 0x00) {
                        // Right channel
                        samples.push_back(doc_harness->right_mix_o);
                    } else {
                        // Left channel
                        samples.push_back(doc_harness->left_mix_o);
                    }
                    
                    // Log audio output periodically
                    if (samples.size() < 10 || samples.size() % 1000 == 0) {
                        printf("Sample %zu (cycle %d): left=%d, right=%d\n", 
                            samples.size(), cycle, 
                            doc_harness->left_mix_o, 
                            doc_harness->right_mix_o);
                    }
                }
                
                // Stop when we've collected enough samples
                if (samples.size() >= SAMPLE_COUNT) {
                    break;
                }
            }
            
            // Evaluate model
            doc_harness->eval();
            
            // Write trace if enabled
            if (dump_trace) tfp->dump(sim_time++);
        }
        
        // Analyze the audio stats for this test
        char label[64];
        
        const char* channel_str = (channel_assignment == 0x00) ? "Right" : "Left";
        const char* volume_str = (test_volume == 64) ? "Normal" : "High";
        snprintf(label, sizeof(label), "Phase %d - %s Channel, %s Volume (0x%02X), Offset 0x%02X", 
                 phase_test, channel_str, volume_str, test_volume, start_offset);
        
        analyze_audio_stats(samples, label);
        
        // Write a WAV file for this test
        char filename[64];
        snprintf(filename, sizeof(filename), "doc_phase%d_ch%s_vol%s.wav", 
                 phase_test, channel_str, volume_str);
        write_wav_file(filename, samples, 22050);
    }
    
    // Cleanup
    if (dump_trace) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    
    printf("\nPhase tests complete. Check the WAV files to see if our new properly centered waveform\n");
    printf("and phase/channel combinations resolve the polarity issue.\n");
    
    return 0;
}