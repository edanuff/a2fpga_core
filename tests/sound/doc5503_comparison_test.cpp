#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"
#include "es5503_standalone.h"

// Required by Verilator
double sc_time_stamp() { return 0; }

// Debug severity levels
enum DebugLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3
};

// Configuration
const DebugLevel verbosity = INFO;
const bool dump_vcd = true;
const int MAX_SIM_TIME = 100000;
const int DOC_CLK_DIV = 8;          // 7.15909MHz / 8
const int SAMPLE_COLLECT_INTERVAL = 100;
const int AUDIO_SAMPLE_RATE = 22050;

// Helper functions
void write_register(Vdoc5503_harness* doc_harness, ES5503* es5503, uint8_t addr, uint8_t data, int* sim_time, bool* clk, VerilatedVcdC* tfp);
void generate_wav_file(const std::vector<int16_t>& samples, const std::string& filename);
void compare_audio_outputs(const std::vector<int16_t>& verilog_samples, const std::vector<int16_t>& cpp_samples, const std::string& test_name);

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create the wavetable memory
    const int wavetable_size = 32768;
    uint8_t* wavetable_mem = new uint8_t[wavetable_size];
    
    // Initialize wavetable with a simple triangle wave (for initial testing)
    for (int i = 0; i < 256; i++) {
        wavetable_mem[i] = 0x80 + (i < 128 ? i : 255 - i);
    }
    
    // Fill the rest with silence
    for (int i = 256; i < wavetable_size; i++) {
        wavetable_mem[i] = 0x80;
    }
    
    // Create instance of the ES5503 C++ reference model
    ES5503* es5503 = new ES5503(7159090, wavetable_mem, wavetable_size);
    es5503->set_channels(2);  // Default to stereo
    
    // Create instance of the top-level Verilog model
    Vdoc5503_harness* doc_harness = new Vdoc5503_harness;
    
    // Create VCD trace file if required
    VerilatedVcdC* tfp = nullptr;
    if (dump_vcd) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        doc_harness->trace(tfp, 99);
        tfp->open("doc5503_compare_trace.vcd");
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
    int sim_time = 0;
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_vcd) tfp->dump(sim_time++);
    }
    
    // Reset both implementations
    doc_harness->reset_n_i = 1;
    es5503->reset();
    
    // Variables to store audio samples
    std::vector<int16_t> verilog_samples_left;
    std::vector<int16_t> verilog_samples_right;
    std::vector<int16_t> cpp_samples_left;
    std::vector<int16_t> cpp_samples_right;
    
    // Buffer for C++ audio output
    const int audio_buffer_size = 1024;
    int16_t audio_buffer[audio_buffer_size * 2]; // * 2 for stereo
    int audio_buffer_pos = 0;
    
    // Test case 1: Basic oscillator setup
    printf("=== Test Case 1: Basic Oscillator Setup ===\n");
    
    // Set up a simpler test case
    // Register addresses:
    // 0x00-0x1F: Frequency Low
    // 0x20-0x3F: Frequency High
    // 0x40-0x5F: Volume
    // 0x60-0x7F: Data (read-only)
    // 0x80-0x9F: Wavetable Pointer
    // 0xA0-0xBF: Control
    // 0xC0-0xDF: Resolution/Table Size
    // 0xE0: Interrupt Status
    // 0xE1: Oscillator Enable
    
    // Set up a single oscillator with maximum frequency and volume
    // for easier detection
    
    // Set frequency (0x1000 = higher frequency for clearer test)
    write_register(doc_harness, es5503, 0x00, 0x00, &sim_time, &clk, tfp);
    write_register(doc_harness, es5503, 0x20, 0x10, &sim_time, &clk, tfp);
    
    // Set volume (0xFF = full volume)
    write_register(doc_harness, es5503, 0x40, 0xFF, &sim_time, &clk, tfp);
    
    // Set wavetable pointer (0x00 = start at page 0)
    write_register(doc_harness, es5503, 0x80, 0x00, &sim_time, &clk, tfp);
    
    // Set control (0x00 = Free running mode, no halt, channel 0)
    // Bit 0 = 0 means NOT halted
    write_register(doc_harness, es5503, 0xA0, 0x00, &sim_time, &clk, tfp);
    
    // Set resolution/table size (0x00 = 256 byte table, standard resolution)
    write_register(doc_harness, es5503, 0xC0, 0x00, &sim_time, &clk, tfp);
    
    // Set oscillator enable (0x02 = 1 oscillator enabled)
    write_register(doc_harness, es5503, 0xE1, 0x02, &sim_time, &clk, tfp);
    
    // Run simulation for a while and collect samples
    for (int i = 0; i < MAX_SIM_TIME; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        
        // On rising edge
        if (clk) {
            // Generate a DOC clk_en_i pulse every DOC_CLK_DIV cycles
            doc_harness->clk_en_i = ((sim_time % DOC_CLK_DIV) == 0);
            
            // Provide wave data when requested
            if (doc_harness->wave_rd_o) {
                uint16_t addr = doc_harness->wave_address_o;
                uint8_t value = wavetable_mem[addr % wavetable_size];
                
                doc_harness->wave_data_i = value;
                doc_harness->wave_data_ready_i = 1;
                
                if (verbosity >= DEBUG) {
                    printf("Wave read at address 0x%04x, providing value 0x%02x\n", 
                           addr, value);
                }
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Collect audio samples at regular intervals
            if ((sim_time % SAMPLE_COLLECT_INTERVAL) == 0) {
                // Collect Verilog output
                verilog_samples_left.push_back(doc_harness->left_mix_o);
                verilog_samples_right.push_back(doc_harness->right_mix_o);
                
                // Generate samples for the C++ implementation periodically
                // We'll generate one buffer at a time
                if (audio_buffer_pos == 0 || audio_buffer_pos >= audio_buffer_size) {
                    es5503->generate_audio(audio_buffer, audio_buffer_size);
                    audio_buffer_pos = 0;
                }
                
                // Add the current sample from the buffer
                cpp_samples_left.push_back(audio_buffer[audio_buffer_pos * 2]);
                cpp_samples_right.push_back(audio_buffer[audio_buffer_pos * 2 + 1]);
                
                // Move to next position in the buffer
                audio_buffer_pos++;
                
                if (verbosity >= DEBUG) {
                    printf("Time %d: Verilog L=%d, R=%d | C++ L=%d, R=%d\n", 
                           sim_time, 
                           verilog_samples_left.back(), 
                           verilog_samples_right.back(),
                           cpp_samples_left.back(), 
                           cpp_samples_right.back());
                }
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Generate and compare WAV files
    generate_wav_file(verilog_samples_left, "doc5503_verilog_left.wav");
    generate_wav_file(verilog_samples_right, "doc5503_verilog_right.wav");
    generate_wav_file(cpp_samples_left, "doc5503_cpp_left.wav");
    generate_wav_file(cpp_samples_right, "doc5503_cpp_right.wav");
    
    // Compare outputs
    compare_audio_outputs(verilog_samples_left, cpp_samples_left, "left channel");
    compare_audio_outputs(verilog_samples_right, cpp_samples_right, "right channel");
    
    // Cleanup
    if (dump_vcd) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    delete es5503;
    delete[] wavetable_mem;
    
    return 0;
}

// Helper function to write a register to both implementations
void write_register(Vdoc5503_harness* doc_harness, ES5503* es5503, uint8_t addr, uint8_t data, int* sim_time, bool* clk, VerilatedVcdC* tfp) {
    // Write to C++ model
    es5503->write(addr, data);
    
    // Write to Verilog model
    for (int i = 0; i < 2; i++) {
        *clk = !(*clk);
        doc_harness->clk_i = *clk;
        
        if (*clk) {
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
        *clk = !(*clk);
        doc_harness->clk_i = *clk;
        
        if (*clk) {
            doc_harness->cs_n_i = 1;
            doc_harness->we_n_i = 1;
        }
        
        doc_harness->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    if (verbosity >= INFO) {
        printf("Wrote 0x%02X to register 0x%02X\n", data, addr);
    }
}

// Helper function to generate a WAV file from samples
void generate_wav_file(const std::vector<int16_t>& samples, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    
    // WAV file header
    const int sample_rate = AUDIO_SAMPLE_RATE;
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
    
    printf("Generated %s with %zu samples\n", filename.c_str(), samples.size());
}

// Helper function to compare audio outputs
void compare_audio_outputs(const std::vector<int16_t>& verilog_samples, const std::vector<int16_t>& cpp_samples, const std::string& test_name) {
    printf("\n=== Comparing %s audio outputs ===\n", test_name.c_str());
    
    // Ensure we have samples to compare
    if (verilog_samples.empty() || cpp_samples.empty()) {
        printf("ERROR: Empty sample array, cannot compare\n");
        return;
    }
    
    // Calculate basic statistics
    int min_size = std::min(verilog_samples.size(), cpp_samples.size());
    double total_diff = 0.0;
    double max_diff = 0.0;
    double avg_v = 0.0;
    double avg_c = 0.0;
    double rms_diff = 0.0;
    
    for (int i = 0; i < min_size; i++) {
        double diff = std::abs((double)verilog_samples[i] - (double)cpp_samples[i]);
        total_diff += diff;
        rms_diff += diff * diff;
        max_diff = std::max(max_diff, diff);
        avg_v += verilog_samples[i];
        avg_c += cpp_samples[i];
    }
    
    avg_v /= min_size;
    avg_c /= min_size;
    double avg_diff = total_diff / min_size;
    rms_diff = std::sqrt(rms_diff / min_size);
    
    // Calculate normalized differences
    double max_signal = 0.0;
    for (int i = 0; i < min_size; i++) {
        max_signal = std::max(max_signal, std::abs((double)verilog_samples[i]));
        max_signal = std::max(max_signal, std::abs((double)cpp_samples[i]));
    }
    
    double norm_avg_diff = (max_signal > 0) ? avg_diff / max_signal * 100.0 : 0.0;
    double norm_max_diff = (max_signal > 0) ? max_diff / max_signal * 100.0 : 0.0;
    double norm_rms_diff = (max_signal > 0) ? rms_diff / max_signal * 100.0 : 0.0;
    
    // Print comparison results
    printf("Sample count: Verilog=%zu, C++=%zu\n", verilog_samples.size(), cpp_samples.size());
    printf("Average values: Verilog=%.2f, C++=%.2f\n", avg_v, avg_c);
    printf("Average difference: %.2f (%.2f%% of max signal)\n", avg_diff, norm_avg_diff);
    printf("Maximum difference: %.2f (%.2f%% of max signal)\n", max_diff, norm_max_diff);
    printf("RMS difference: %.2f (%.2f%% of max signal)\n", rms_diff, norm_rms_diff);
    
    // Threshold for acceptable difference (1% of max signal)
    const double threshold = 1.0;
    
    if (norm_rms_diff <= threshold) {
        printf("PASS: RMS difference is below %g%% threshold\n", threshold);
    } else {
        printf("FAIL: RMS difference exceeds %g%% threshold\n", threshold);
    }
}