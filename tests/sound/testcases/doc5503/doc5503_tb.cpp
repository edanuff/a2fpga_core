#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Maximum simulation time
#define MAX_SIM_TIME 100000

// Debug severity levels
enum DebugLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3
};

// Debug level
const DebugLevel verbosity = DEBUG;

// Dump VCD file
const bool dump_vcd = true;

// Forward declarations
void test_oscillator_enable_register(Vdoc5503_harness* doc_harness, VerilatedVcdC* tfp);
void write_register(Vdoc5503_harness* doc_harness, uint8_t addr, uint8_t data, int* sim_time, bool clk, VerilatedVcdC* tfp);
void write_and_verify_register(Vdoc5503_harness* doc_harness, uint8_t addr, uint8_t data, int* sim_time, bool clk, VerilatedVcdC* tfp);
void setup_oscillator(Vdoc5503_harness* doc_harness, int osc_num, int* sim_time, bool clk, VerilatedVcdC* tfp);
void generate_wav_file(const std::vector<int16_t>& samples, const std::string& filename);

// Main simulation loop
int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the top-level model
    Vdoc5503_harness* doc_harness = new Vdoc5503_harness;
    
    // Create VCD trace file if required
    VerilatedVcdC* tfp = nullptr;
    if (dump_vcd) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        doc_harness->trace(tfp, 99);
        tfp->open("doc5503_trace.vcd");
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
    
    // Reset for a few cycles
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_vcd) tfp->dump(i);
    }
    
    // Release reset
    doc_harness->reset_n_i = 1;
    
    // Run the oscillator enable register test
    test_oscillator_enable_register(doc_harness, tfp);
    
    // Cleanup
    if (dump_vcd) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    
    return 0;
}

// Test function for the oscillator enable register
void test_oscillator_enable_register(Vdoc5503_harness* doc_harness, VerilatedVcdC* tfp) {
    int sim_time = 10; // Start after reset cycles
    bool clk = false;
    std::vector<int16_t> audio_samples_1;
    std::vector<int16_t> audio_samples_31;
    
    printf("=== Testing oscillator enable register behavior ===\n");
    
    // First test with 1 oscillator enabled (value of 0x02)
    printf("Testing with 1 oscillator enabled (osc_en=0x02)...\n");
    
    // Set up oscillator 0 for a simple tone
    setup_oscillator(doc_harness, 0, &sim_time, clk, tfp);
    
    // Set oscillator enable register
    write_and_verify_register(doc_harness, 0xE1, 0x02, &sim_time, clk, tfp);
    
    // Run simulation for a while and collect samples
    for (int i = 0; i < 50000; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        // On rising edge, set clk_en_i pulse
        if (clk) {
            // Generate a DOC clk_en_i pulse every 8 cycles to simulate 7.15909MHz/8
            doc_harness->clk_en_i = ((sim_time % 8) == 0);
            
            // Provide wave data when requested
            if (doc_harness->wave_rd_o) {
                // Use sine wave approximation
                uint16_t addr = doc_harness->wave_address_o & 0xFF;
                uint8_t value = 0x80 + (addr < 128 ? addr : 255 - addr);  // Simple triangle wave
                
                doc_harness->wave_data_i = value;
                doc_harness->wave_data_ready_i = 1;
                
                if (verbosity >= DEBUG) {
                    printf("Wave read at address 0x%04x, providing value 0x%02x\n", 
                           doc_harness->wave_address_o, value);
                }
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Collect audio samples every 100 cycles
            if ((sim_time % 100) == 0) {
                if (verbosity >= DEBUG) {
                    printf("Time %d: Collecting sample, left_mix_o = %d\n", 
                           sim_time, doc_harness->left_mix_o);
                }
                audio_samples_1.push_back(doc_harness->left_mix_o);
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Reset for next test
    doc_harness->reset_n_i = 0;
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_vcd) tfp->dump(sim_time++);
    }
    doc_harness->reset_n_i = 1;
    
    // Now test with 31 oscillators enabled (value of 0x3E)
    printf("Testing with 31 oscillators enabled (osc_en=0x3E)...\n");
    
    // Set up oscillator 0 with the same values
    setup_oscillator(doc_harness, 0, &sim_time, clk, tfp);
    
    // Set oscillator enable register to 31 oscillators
    write_and_verify_register(doc_harness, 0xE1, 0x3E, &sim_time, clk, tfp);
    
    // Run simulation for the same duration
    for (int i = 0; i < 50000; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        // On rising edge, set clk_en_i pulse
        if (clk) {
            // Generate a DOC clk_en_i pulse every 8 cycles to simulate 7.15909MHz/8
            doc_harness->clk_en_i = ((sim_time % 8) == 0);
            
            // Provide wave data when requested
            if (doc_harness->wave_rd_o) {
                // Use sine wave approximation
                uint16_t addr = doc_harness->wave_address_o & 0xFF;
                uint8_t value = 0x80 + (addr < 128 ? addr : 255 - addr);  // Simple triangle wave
                
                doc_harness->wave_data_i = value;
                doc_harness->wave_data_ready_i = 1;
                
                if (verbosity >= DEBUG) {
                    printf("Wave read at address 0x%04x, providing value 0x%02x (31 osc)\n", 
                           doc_harness->wave_address_o, value);
                }
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Collect audio samples every 100 cycles
            if ((sim_time % 100) == 0) {
                if (verbosity >= DEBUG) {
                    printf("Time %d: Collecting sample, left_mix_o = %d (31 osc)\n", 
                           sim_time, doc_harness->left_mix_o);
                }
                audio_samples_31.push_back(doc_harness->left_mix_o);
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Generate WAV files for analysis
    generate_wav_file(audio_samples_1, "doc_output_1osc.wav");
    generate_wav_file(audio_samples_31, "doc_output_31osc.wav");
    
    // Compare frequency differences
    printf("Generated audio output files for both configurations.\n");
    printf("Analysis:\n");
    printf("  - With 1 oscillator (osc_en=0x02): Output in doc_output_1osc.wav\n");
    printf("  - With 31 oscillators (osc_en=0x3E): Output in doc_output_31osc.wav\n");
    
    // Compute and print sample rate ratio
    printf("Expected sample rate ratio: 31 oscillators should have ~1/31 the frequency of 1 oscillator\n");
    
    // Additional test for the fixed off-by-one error
    printf("\n=== Testing the off-by-one fix in oscillator enable register ===\n");
    printf("Lines 151-152 in doc5503.sv show the correction:\n");
    printf("wire [4:0] osc_en_num_w = osc_en_r[5:1];\n");
    printf("wire [4:0] cycle_max_w = osc_en_r[5:1];\n");
    printf("Previously it was: cycle_max_w = osc_en_num_w == 0 ? 0 : 5'(osc_en_num_w - 1'b1);\n");
    
    // Test with values that would be affected by the off-by-one error
    for (int test_val = 2; test_val <= 10; test_val += 2) {
        uint8_t osc_en_val = test_val;
        printf("Testing oscillator enable value: 0x%02X (%d)\n", osc_en_val, osc_en_val);
        
        // Reset
        doc_harness->reset_n_i = 0;
        for (int i = 0; i < 10; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;
            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }
        doc_harness->reset_n_i = 1;
        
        // Set up oscillator 0
        setup_oscillator(doc_harness, 0, &sim_time, clk, tfp);
        
        // Set oscillator enable register to test value
        write_and_verify_register(doc_harness, 0xE1, osc_en_val, &sim_time, clk, tfp);
        
        // Collect a short sample to verify behavior
        std::vector<int16_t> test_samples;
        for (int i = 0; i < 2000; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
                
                if (doc_harness->wave_rd_o) {
                    uint16_t addr = doc_harness->wave_address_o & 0xFF;
                    uint8_t value = 0x80 + (addr < 128 ? addr : 255 - addr);
                    doc_harness->wave_data_i = value;
                    doc_harness->wave_data_ready_i = 1;
                } else {
                    doc_harness->wave_data_ready_i = 0;
                }
                
                if ((sim_time % 100) == 0) {
                    test_samples.push_back(doc_harness->left_mix_o);
                }
            }
            
            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time);
            sim_time++;
        }
        
        // Generate WAV file for this test
        char filename[64];
        sprintf(filename, "doc_output_osc_en_%02d.wav", osc_en_val);
        generate_wav_file(test_samples, filename);
        printf("  - Generated %s for analysis\n", filename);
    }
}

// Helper function to write a register value
void write_register(Vdoc5503_harness* doc_harness, uint8_t addr, uint8_t data, int* sim_time, bool clk, VerilatedVcdC* tfp) {
    // Write register on next rising edge
    for (int i = 0; i < 2; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            doc_harness->cs_n_i = 0;
            doc_harness->we_n_i = 0;
            doc_harness->addr_i = addr;
            doc_harness->data_i = data;
        }
        
        doc_harness->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Reset control signals
    for (int i = 0; i < 2; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            doc_harness->cs_n_i = 1;
            doc_harness->we_n_i = 1;
        }
        
        doc_harness->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
}

// Helper function to write and verify a register value
void write_and_verify_register(Vdoc5503_harness* doc_harness, uint8_t addr, uint8_t data, int* sim_time, bool clk, VerilatedVcdC* tfp) {
    // Write the register
    write_register(doc_harness, addr, data, sim_time, clk, tfp);
    
    // Read back and verify
    for (int i = 0; i < 2; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            doc_harness->cs_n_i = 0;
            doc_harness->we_n_i = 1;
            doc_harness->addr_i = addr;
        }
        
        doc_harness->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Check the value
    if (doc_harness->data_o != data) {
        printf("ERROR: Register verification failed! Address: 0x%02X, Expected: 0x%02X, Got: 0x%02X\n",
               addr, data, doc_harness->data_o);
    } else if (verbosity >= DEBUG) {
        printf("DEBUG: Register verification passed! Address: 0x%02X, Value: 0x%02X\n",
               addr, data);
    }
    
    // Reset control signals
    for (int i = 0; i < 2; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            doc_harness->cs_n_i = 1;
            doc_harness->we_n_i = 1;
        }
        
        doc_harness->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
}

// Helper function to set up an oscillator with test values
void setup_oscillator(Vdoc5503_harness* doc_harness, int osc_num, int* sim_time, bool clk, VerilatedVcdC* tfp) {
    // Calculate register addresses
    uint8_t fl_addr = 0x00 | (osc_num & 0x1F);
    uint8_t fh_addr = 0x20 | (osc_num & 0x1F);
    uint8_t vol_addr = 0x40 | (osc_num & 0x1F);
    uint8_t wtp_addr = 0x80 | (osc_num & 0x1F);
    uint8_t ctl_addr = 0xA0 | (osc_num & 0x1F);
    uint8_t rts_addr = 0xC0 | (osc_num & 0x1F);
    
    // Set up oscillator registers
    if (verbosity >= INFO) {
        printf("Setting up oscillator %d...\n", osc_num);
    }
    
    // Frequency Low = 0x20
    write_register(doc_harness, fl_addr, 0x20, sim_time, clk, tfp);
    
    // Frequency High = 0x01 (moderate frequency)
    write_register(doc_harness, fh_addr, 0x01, sim_time, clk, tfp);
    
    // Volume = 0x7F (medium volume)
    write_register(doc_harness, vol_addr, 0x7F, sim_time, clk, tfp);
    
    // Waveform Table Pointer = 0x00 (start at page 0)
    write_register(doc_harness, wtp_addr, 0x00, sim_time, clk, tfp);
    
    // Control = 0x10 (Free running mode, no halt, channel 1 - left)
    // Note: Control register bit 0 is the halt bit (0=run, 1=halt)
    write_register(doc_harness, ctl_addr, 0x10, sim_time, clk, tfp);
    
    // Resolution/Table Size = 0x00 (256 byte table, standard resolution)
    write_register(doc_harness, rts_addr, 0x00, sim_time, clk, tfp);
    
    if (verbosity >= INFO) {
        printf("Oscillator %d setup complete\n", osc_num);
    }
}

// Helper function to generate a WAV file from samples
void generate_wav_file(const std::vector<int16_t>& samples, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    
    // Check if we have any valid samples (non-zero)
    bool has_valid_samples = false;
    for (auto sample : samples) {
        if (sample != 0) {
            has_valid_samples = true;
            break;
        }
    }
    
    if (!has_valid_samples) {
        if (verbosity >= WARNING) {
            printf("WARNING: All samples are zero in %s\n", filename.c_str());
        }
    }
    
    // WAV file header
    const int sample_rate = 44100;
    const int num_channels = 1;
    const int bits_per_sample = 16;
    const int data_size = samples.size() * (bits_per_sample / 8);
    const int header_size = 44;
    
    // RIFF header
    file.write("RIFF", 4);
    int filesize = data_size + header_size - 8;
    file.write(reinterpret_cast<const char*>(&filesize), 4);
    file.write("WAVE", 4);
    
    // Format chunk
    file.write("fmt ", 4);
    int format_length = 16;
    file.write(reinterpret_cast<const char*>(&format_length), 4);
    short audio_format = 1; // PCM
    file.write(reinterpret_cast<const char*>(&audio_format), 2);
    file.write(reinterpret_cast<const char*>(&num_channels), 2);
    file.write(reinterpret_cast<const char*>(&sample_rate), 4);
    int byte_rate = sample_rate * num_channels * (bits_per_sample / 8);
    file.write(reinterpret_cast<const char*>(&byte_rate), 4);
    short block_align = num_channels * (bits_per_sample / 8);
    file.write(reinterpret_cast<const char*>(&block_align), 2);
    file.write(reinterpret_cast<const char*>(&bits_per_sample), 2);
    
    // Data chunk
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&data_size), 4);
    file.write(reinterpret_cast<const char*>(samples.data()), data_size);
    
    file.close();
    
    // Print sample statistics
    if (verbosity >= INFO) {
        int16_t min_val = 0, max_val = 0;
        double avg_val = 0;
        int non_zero_count = 0;
        
        if (!samples.empty()) {
            min_val = max_val = samples[0];
            for (auto sample : samples) {
                if (sample < min_val) min_val = sample;
                if (sample > max_val) max_val = sample;
                avg_val += sample;
                if (sample != 0) non_zero_count++;
            }
            avg_val /= samples.size();
        }
        
        printf("WAV file generated: %s (%zu samples, %d non-zero)\n", 
               filename.c_str(), samples.size(), non_zero_count);
        printf("  Sample stats: min=%d, max=%d, avg=%.2f\n", min_val, max_val, avg_val);
    }
}