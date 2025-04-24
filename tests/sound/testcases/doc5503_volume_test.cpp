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

// Generate a sine wave wavetable
void generate_sine_wavetable(uint8_t* wavetable, size_t length) {
    for (size_t i = 0; i < length; i++) {
        double angle = (double)i / length * 2.0 * M_PI;
        double value = sin(angle);
        wavetable[i] = (uint8_t)(value * 127.0 + 128.0);
    }
}

// Generate audio statistics
void analyze_audio_stats(const std::vector<int16_t>& samples, const char* label) {
    int16_t min_val = 32767;
    int16_t max_val = -32768;
    double avg_val = 0.0;
    double rms_val = 0.0;
    int clipping_count = 0;
    int non_zero_count = 0;
    
    for (auto sample : samples) {
        if (sample < min_val) min_val = sample;
        if (sample > max_val) max_val = sample;
        avg_val += sample;
        
        if (sample != 0) non_zero_count++;
        if (sample == 32767 || sample == -32768) clipping_count++;
        
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
    printf("Non-zero samples: %d (%.1f%%)\n", 
        non_zero_count, (non_zero_count * 100.0) / samples.size());
    printf("Clipping: %d samples (%.4f%%)\n", 
        clipping_count, (clipping_count * 100.0) / samples.size());
    
    // Check for good audio levels
    if (max_val >= 32000 || min_val <= -32000) {
        printf("WARNING: Signal is close to clipping!\n");
    }
    
    double peak_level_db = 20.0 * log10((double)std::max(abs(min_val), abs(max_val)) / 32768.0);
    printf("Peak level: %.2f dB\n", peak_level_db);
    
    // Headroom calculation
    double headroom_db = 0.0 - peak_level_db;  // 0 dB is full scale
    printf("Headroom: %.2f dB\n", headroom_db);
    
    if (headroom_db < 3.0) {
        printf("WARNING: Less than 3dB headroom may lead to clipping on material peaks\n");
    } else if (headroom_db > 20.0) {
        printf("WARNING: More than 20dB headroom suggests signal may be too quiet\n");
    } else {
        printf("GOOD: Headroom is in an acceptable range (3-20dB)\n");
    }
}

// Main test function
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
        tfp->open("doc5503_volume_test.vcd");
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
    
    // Generate sine wavetable for testing
    const size_t WAVETABLE_SIZE = 256;
    uint8_t sine_wavetable[WAVETABLE_SIZE];
    generate_sine_wavetable(sine_wavetable, WAVETABLE_SIZE);
    
    printf("\n===== TESTING DOC5503 VOLUME SCALING =====\n");
    
    // Test with different volume levels
    const int NUM_VOLUME_TESTS = 5;
    uint8_t volume_levels[NUM_VOLUME_TESTS] = {
        32,     // Very low volume (12.5%)
        64,     // Low volume (25%)
        128,    // Medium volume (50%)
        192,    // High volume (75%)
        255     // Maximum volume (100%)
    };
    
    std::vector<std::vector<int16_t>> volume_test_samples(NUM_VOLUME_TESTS);
    
    for (int vol_test = 0; vol_test < NUM_VOLUME_TESTS; vol_test++) {
        uint8_t test_volume = volume_levels[vol_test];
        
        // Reset for a clean state
        reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
        
        printf("\nTesting volume level %d (0x%02X - %.1f%%)...\n", 
               test_volume, test_volume, (test_volume * 100.0) / 255.0);
        
        // Enable one oscillator
        write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
        
        // Setup oscillator parameters with test volume
        uint8_t osc_num = 0;
        uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
        uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
        uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
        uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
        uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
        uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
        
        // Use a higher frequency for faster wave table scanning to check if we get samples
        write_register(doc_harness, addr_freq_low, 0x00, sim_time, clk, tfp, dump_trace);
        write_register(doc_harness, addr_freq_high, 0x10, sim_time, clk, tfp, dump_trace);
        printf("Set frequency to 0x1000 for testing (high freq for fast wave reading)\n");
        
        // Set volume to the test value
        write_register(doc_harness, addr_volume, test_volume, sim_time, clk, tfp, dump_trace);
        printf("Set volume register (0x%02X) to 0x%02X\n", addr_volume, test_volume);
        
        // Other standard parameters
        write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);   // Start at page 0
        printf("Set wave table pointer (0x%02X) to 0x00\n", addr_wave_ptr);
        
        // Make sure control register has the oscillator running (not halted)
        // 0x00 = 00000000 (channel 0, free running, not halted)
        write_register(doc_harness, addr_control, 0x00, sim_time, clk, tfp, dump_trace);    // Free running, channel 0, not halted
        printf("Set control register (0x%02X) to 0x10 (free running, channel 1, not halted)\n", addr_control);
        
        write_register(doc_harness, addr_res_table, 0x00, sim_time, clk, tfp, dump_trace);  // 256-byte table, standard resolution
        printf("Set res/table register (0x%02X) to 0x00 (256-byte table, standard resolution)\n", addr_res_table);
        
        // Collect samples for this test
        std::vector<int16_t>& samples = volume_test_samples[vol_test];
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
                
                // When the DOC requests wave data, provide it from our sine wavetable
                if (doc_harness->wave_rd_o) {
                    uint16_t wave_addr = doc_harness->wave_address_o & 0xFF;
                    doc_harness->wave_data_i = sine_wavetable[wave_addr];
                    doc_harness->wave_data_ready_i = 1;
                    
                    // Always print wave read requests during debugging
                    printf("Cycle %d: Wave read request at addr=0x%04X, providing value=0x%02X\n", 
                           cycle, wave_addr, doc_harness->wave_data_i);
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                // Collect samples after warmup
                if (doc_clk_en && cycle >= 500 && samples.size() < SAMPLE_COUNT) {
                    samples.push_back(doc_harness->left_mix_o);
                    
                    // Log audio output values more frequently during debugging
                    if (samples.size() < 100 || samples.size() % 1000 == 0) {
                        printf("Sample %zu (cycle %d): left=%d, right=%d, mono=%d\n", 
                            samples.size(), cycle, 
                            doc_harness->left_mix_o, 
                            doc_harness->right_mix_o, 
                            doc_harness->mono_mix_o);
                    }
                    
                    // Print additional debug info every 500 samples
                    if (samples.size() % 500 == 0) {
                        printf("Mixer debug - Sample %zu\n", samples.size());
                    }
                }
                
                // Stop when we've collected enough samples
                if (samples.size() >= SAMPLE_COUNT) {
                    printf("Collected %zu samples with volume=%d\n", samples.size(), test_volume);
                    break;
                }
            }
            
            // Evaluate model
            doc_harness->eval();
            
            // Write trace if enabled
            if (dump_trace) tfp->dump(sim_time++);
        }
        
        // Analyze the audio stats for this volume level
        char label[64];
        snprintf(label, sizeof(label), "Volume %d (%.1f%%) Statistics", 
                test_volume, (test_volume * 100.0) / 255.0);
        analyze_audio_stats(samples, label);
        
        // Write a WAV file for this test
        char filename[64];
        snprintf(filename, sizeof(filename), "doc_output_vol_%d.wav", test_volume);
        write_wav_file(filename, samples, 22050);
    }
    
    // Analyze the scaling characteristics
    printf("\n===== VOLUME SCALING ANALYSIS =====\n");
    
    // Calculate the expected ratio between volume levels
    for (int i = 1; i < NUM_VOLUME_TESTS; i++) {
        int vol1 = volume_levels[i-1];
        int vol2 = volume_levels[i];
        
        // Find the peak values in each sample set
        int16_t peak1 = 0, peak2 = 0;
        for (const auto& sample : volume_test_samples[i-1]) {
            if (abs(sample) > abs(peak1)) peak1 = sample;
        }
        for (const auto& sample : volume_test_samples[i]) {
            if (abs(sample) > abs(peak2)) peak2 = sample;
        }
        
        // Calculate the actual ratio
        double expected_ratio = (double)vol2 / vol1;
        double actual_ratio = (double)abs(peak2) / abs(peak1);
        
        printf("Volume %d → %d: Expected ratio %.2f, Actual ratio %.2f, Difference %.2f%%\n",
            vol1, vol2, expected_ratio, actual_ratio, (actual_ratio/expected_ratio - 1.0) * 100.0);
    }
    
    // Find the peak value across all volume levels
    int16_t max_peak = 0;
    int max_peak_vol_index = 0;
    
    for (int i = 0; i < NUM_VOLUME_TESTS; i++) {
        for (const auto& sample : volume_test_samples[i]) {
            if (abs(sample) > abs(max_peak)) {
                max_peak = sample;
                max_peak_vol_index = i;
            }
        }
    }
    
    // Calculate what percentage of full scale we're achieving at max volume
    double percent_of_full_scale = (abs(max_peak) * 100.0) / 32768.0;
    printf("\nPeak sample value: %d (%.2f%% of full scale) at volume=%d\n",
        max_peak, percent_of_full_scale, volume_levels[max_peak_vol_index]);
    
    if (percent_of_full_scale < 70.0) {
        printf("WARNING: Maximum volume is using less than 70%% of available range.\n");
        printf("Consider increasing the gain in the volume scaling to utilize more dynamic range.\n");
    } else if (percent_of_full_scale > 95.0) {
        printf("WARNING: Maximum volume is very close to clipping!\n");
        printf("Consider reducing the gain in the volume scaling to avoid potential clipping.\n");
    } else {
        printf("GOOD: Maximum volume is utilizing dynamic range effectively without clipping.\n");
    }
    
    // Cleanup
    if (dump_trace) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    
    printf("\nTest complete. You can now analyze the volume scaling by examining the WAV files.\n");
    
    return 0;
}