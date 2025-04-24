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
void write_wav_file(const char* filename, int16_t* samples, size_t num_samples, int sample_rate) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("ERROR: Could not open %s for writing\n", filename);
        return;
    }
    
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
    uint32_t data_size = num_samples * 2; // 2 bytes per sample
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
    fwrite(samples, 2, num_samples, f);
    
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

// Helper function to read a register from the DOC
uint8_t read_register(Vdoc5503_harness* doc_harness, uint8_t addr, int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    // Set up read on rising edge
    clk = true;
    doc_harness->clk_i = clk;
    doc_harness->cs_n_i = 0;
    doc_harness->we_n_i = 1;
    doc_harness->addr_i = addr;
    doc_harness->eval();
    if (dump_trace) tfp->dump(sim_time++);
    
    // Read data on falling edge
    clk = false;
    doc_harness->clk_i = clk;
    doc_harness->eval();
    uint8_t data = doc_harness->data_o;
    if (dump_trace) tfp->dump(sim_time++);
    
    // Reset control signals
    doc_harness->cs_n_i = 1;
    doc_harness->we_n_i = 1;
    
    return data;
}

// Configure a DOC oscillator
void setup_oscillator(Vdoc5503_harness* doc_harness, uint8_t osc_num, uint16_t freq, uint8_t vol, uint8_t control, 
                     int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    const uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
    const uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
    const uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
    const uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
    const uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
    const uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
    
    printf("Setting up oscillator %d...\n", osc_num);
    
    // Frequency Low
    write_register(doc_harness, addr_freq_low, freq & 0xFF, sim_time, clk, tfp, dump_trace);
    
    // Frequency High
    write_register(doc_harness, addr_freq_high, (freq >> 8) & 0xFF, sim_time, clk, tfp, dump_trace);
    
    // Volume
    write_register(doc_harness, addr_volume, vol, sim_time, clk, tfp, dump_trace);
    
    // Wave Table Pointer (start at page 0)
    write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);
    
    // Control register 
    // - Bits 7-4: Channel assignment (0x10 = channel 1)
    // - Bit 3: Interrupt enable
    // - Bits 2-1: Mode (0=Free Run, 1=One Shot, 2=Sync/AM, 3=Swap)
    // - Bit 0: Halt bit (0=run, 1=halt)
    write_register(doc_harness, addr_control, control, sim_time, clk, tfp, dump_trace);
    
    // Resolution/Table Size 
    // - Bits 5-3: Table size (0-7 = 256,512,1024,2048,4096,8192,16384,32768 bytes)
    // - Bits 2-0: Resolution (0-7 = accumulator bits 1-16 through 8-23)
    write_register(doc_harness, addr_res_table, 0x00, sim_time, clk, tfp, dump_trace);
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

// Generate a test wavetable (sine wave)
void generate_sine_wavetable(uint8_t* wavetable, size_t length) {
    for (size_t i = 0; i < length; i++) {
        // Generate sine wave centered at 0x80
        double angle = (double)i / length * 2.0 * M_PI;
        double value = sin(angle);
        wavetable[i] = (uint8_t)(value * 127.0 + 128.0);
    }
}

// Generate a test wavetable with a zero at the end to test oscillator halting
void generate_test_wavetable_with_zero(uint8_t* wavetable, size_t length) {
    for (size_t i = 0; i < length - 8; i++) {
        // Generate rising sawtooth wave 
        wavetable[i] = (uint8_t)(((i % 64) * 2) + 128);
    }
    
    // Put zeros at the end to halt the oscillator
    for (size_t i = length - 8; i < length; i++) {
        wavetable[i] = 0;
    }
}

// Main function to run oscillator mode tests
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
        tfp->open("doc5503_mode_test.vcd");
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
    
    // Generate test wavetables
    const size_t WAVETABLE_SIZE = 256;
    uint8_t sine_wavetable[WAVETABLE_SIZE];
    uint8_t test_wavetable[WAVETABLE_SIZE];
    
    generate_sine_wavetable(sine_wavetable, WAVETABLE_SIZE);
    generate_test_wavetable_with_zero(test_wavetable, WAVETABLE_SIZE);
    
    // Store output from each mode for comparison
    const size_t SAMPLE_SIZE = 10000;
    std::vector<int16_t> free_run_samples;
    std::vector<int16_t> one_shot_samples;
    std::vector<int16_t> sync_am_samples;
    std::vector<int16_t> swap_samples;
    
    const int MODES_TO_TEST = 4;
    
    // Parameters for each test
    uint16_t freq = 0x0200;  // Moderate frequency for clear output
    uint8_t volume = 0xFF;   // Maximum volume
    bool use_zero_end_wavetable = false;  // Whether to use the wavetable with zero at end
    
    // Test each oscillator mode
    for (int mode = 0; mode < MODES_TO_TEST; mode++) {
        // For swap and sync/AM modes, we need to set up oscillator pairs
        bool is_pair_mode = (mode >= 2);  // Sync/AM and Swap modes need pairs
        
        printf("\n===== Starting test for mode %d =====\n", mode);
        
        // Reset DOC for a clean state
        reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
        
        // Set oscillator enable register based on test
        if (is_pair_mode) {
            // Enable two oscillators (pair) for sync/AM and swap modes
            write_register(doc_harness, 0xE1, 0x04, sim_time, clk, tfp, dump_trace);
        } else {
            // Enable one oscillator for free run and one shot
            write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
        }
        
        // Setup first oscillator
        uint8_t osc0_control;
        
        switch (mode) {
            case 0:  // Free Run mode
                osc0_control = 0x10;  // Channel 1, Free Run
                break;
            case 1:  // One Shot mode
                osc0_control = 0x12;  // Channel 1, One Shot
                break;
            case 2:  // Sync/AM mode
                osc0_control = 0x14;  // Channel 1, Sync/AM
                break;
            case 3:  // Swap mode
                osc0_control = 0x16;  // Channel 1, Swap
                break;
        }
        
        // Use the wavetable with zero at end for testing one-shot/swap modes
        use_zero_end_wavetable = (mode == 1 || mode == 3);
        
        // Set up oscillator 0
        setup_oscillator(doc_harness, 0, freq, volume, osc0_control, sim_time, clk, tfp, dump_trace);
        
        // For pair modes, set up oscillator 1
        if (is_pair_mode) {
            uint8_t osc1_control;
            
            if (mode == 2) {  // Sync/AM mode
                osc1_control = 0x14;  // Channel 1, Sync/AM for osc 1
            } else {  // Swap mode
                osc1_control = 0x17;  // Channel 1, Swap for osc 1, Halted to start
            }
            
            setup_oscillator(doc_harness, 1, freq, volume, osc1_control, sim_time, clk, tfp, dump_trace);
        }
        
        // Run the simulation and collect samples
        std::vector<int16_t>* current_samples;
        
        switch (mode) {
            case 0: current_samples = &free_run_samples; break;
            case 1: current_samples = &one_shot_samples; break;
            case 2: current_samples = &sync_am_samples; break;
            case 3: current_samples = &swap_samples; break;
        }
        
        printf("Running simulation for mode %d...\n", mode);
        
        // Run for enough cycles to generate adequate samples
        for (int cycle = 0; cycle < SAMPLE_SIZE * 10; cycle++) {
            // Toggle clock
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                // Generate DOC clk_en_i pulse every 8 cycles (simulates 7.15909MHz/8)
                bool doc_clk_en = ((cycle % 8) == 0);
                doc_harness->clk_en_i = doc_clk_en;
                
                // When the DOC requests wave data, provide it from our wavetable
                if (doc_harness->wave_rd_o) {
                    uint16_t wave_addr = doc_harness->wave_address_o & 0xFF;
                    
                    // Select wavetable based on test
                    if (use_zero_end_wavetable) {
                        doc_harness->wave_data_i = test_wavetable[wave_addr];
                    } else {
                        doc_harness->wave_data_i = sine_wavetable[wave_addr];
                    }
                    
                    doc_harness->wave_data_ready_i = 1;
                    
                    // Debug output for first few wave reads
                    if (cycle < 100) {
                        printf("Wave read at addr=0x%04x, data=0x%02x\n", 
                            wave_addr, doc_harness->wave_data_i);
                    }
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                // Collect samples every N cycles after warmup
                if (doc_clk_en && cycle >= 800 && cycle % 8 == 0 && current_samples->size() < SAMPLE_SIZE) {
                    current_samples->push_back(doc_harness->left_mix_o);
                    
                    // Debug output every 1000 samples
                    if (current_samples->size() % 1000 == 0) {
                        printf("Captured %zu samples, current value: %d\n", 
                            current_samples->size(), doc_harness->left_mix_o);
                    }
                }
                
                // Debug: print register state for first few cycles
                if (cycle < 10 && doc_clk_en) {
                    uint8_t control = read_register(doc_harness, 0xA0, sim_time, clk, tfp, dump_trace);
                    uint8_t wave_data = read_register(doc_harness, 0x40, sim_time, clk, tfp, dump_trace);
                    
                    printf("Cycle %d: Control=0x%02x, Wave data=0x%02x, Output=%d\n",
                        cycle, control, wave_data, doc_harness->left_mix_o);
                }
                
                // Stop when we've collected enough samples
                if (current_samples->size() >= SAMPLE_SIZE) {
                    printf("Collected %zu samples for mode %d\n", current_samples->size(), mode);
                    break;
                }
            }
            
            // Evaluate model
            doc_harness->eval();
            
            // Write trace if enabled
            if (dump_trace) tfp->dump(sim_time++);
        }
        
        printf("Test for mode %d complete.\n", mode);
    }
    
    // Write output to WAV files at 22050Hz sample rate
    printf("\nWriting output WAV files...\n");
    
    // Pad sample vectors to ensure they're all the same length
    size_t max_samples = SAMPLE_SIZE;
    free_run_samples.resize(max_samples, 0);
    one_shot_samples.resize(max_samples, 0);
    sync_am_samples.resize(max_samples, 0);
    swap_samples.resize(max_samples, 0);
    
    // Write WAV files
    write_wav_file("doc_mode_free_run.wav", free_run_samples.data(), free_run_samples.size(), 22050);
    write_wav_file("doc_mode_one_shot.wav", one_shot_samples.data(), one_shot_samples.size(), 22050);
    write_wav_file("doc_mode_sync_am.wav", sync_am_samples.data(), sync_am_samples.size(), 22050);
    write_wav_file("doc_mode_swap.wav", swap_samples.data(), swap_samples.size(), 22050);
    
    // Print a summary
    printf("\nTest Results Summary:\n");
    printf("1. FREE RUN MODE: Should play continuously - check doc_mode_free_run.wav\n");
    printf("2. ONE SHOT MODE: Should play once and halt - check doc_mode_one_shot.wav\n");
    printf("3. SYNC/AM MODE: Should have amplitude modulation - check doc_mode_sync_am.wav\n");
    printf("4. SWAP MODE: Should transition between oscillators - check doc_mode_swap.wav\n");
    
    // Calculate some basic stats for each mode's output
    auto calculate_stats = [](const std::vector<int16_t>& samples, const char* mode_name) {
        int16_t min_val = 32767, max_val = -32768;
        double avg = 0.0;
        int non_zero_count = 0;
        
        for (auto sample : samples) {
            if (sample < min_val) min_val = sample;
            if (sample > max_val) max_val = sample;
            avg += sample;
            if (sample != 0) non_zero_count++;
        }
        
        avg /= samples.size();
        
        printf("%s Mode: min=%d, max=%d, range=%d, avg=%.2f, non-zero=%d (%.1f%%)\n",
            mode_name, min_val, max_val, max_val - min_val, avg, 
            non_zero_count, (non_zero_count * 100.0) / samples.size());
    };
    
    printf("\nOutput Statistics:\n");
    calculate_stats(free_run_samples, "Free Run");
    calculate_stats(one_shot_samples, "One Shot");
    calculate_stats(sync_am_samples, "Sync/AM");
    calculate_stats(swap_samples, "Swap");
    
    // Cleanup
    if (dump_trace) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    
    printf("\nTest complete. You can now load the WAV files to analyze the different oscillator modes.\n");
    
    return 0;
}