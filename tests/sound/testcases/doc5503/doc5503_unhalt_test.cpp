#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <unistd.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required by Verilator
double sc_time_stamp() { return 0; }

// Configuration
const bool dump_vcd = true;
const int MAX_SIM_TIME = 50000;
const int DOC_CLK_DIV = 8;          // 7.15909MHz / 8
const int SAMPLE_COLLECT_INTERVAL = 100;
const int TRACE_INTERVAL = 1000;    // How often to print debug trace

// Global variables for debug function
bool clk_global = false;
VerilatedVcdC* tfp_global = nullptr;

// Wavetable configuration
const int WAVETABLE_SIZE = 256;

// Helper functions
void write_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t data, int* sim_time, bool* clk, VerilatedVcdC* tfp);
void read_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t* data, int* sim_time, bool* clk, VerilatedVcdC* tfp);
void generate_wav_file(const std::vector<int16_t>& samples, const std::string& filename);
void print_debug_info(Vdoc5503_harness* dut, int* sim_time, bool* clk);

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create the wavetable memory - use a sawtooth wave for more distinct audio
    uint8_t* wavetable_mem = new uint8_t[WAVETABLE_SIZE];
    
    printf("Initializing wavetable with sawtooth wave pattern...\n");
    for (int i = 0; i < WAVETABLE_SIZE; i++) {
        // Sawtooth wave from 0x81 to 0xFF (positive values)
        wavetable_mem[i] = 0x80 + ((i % 127) + 1); // Ensure no zeros in the wavetable
        
        if (i < 16) {
            printf("Wavetable[%3d] = 0x%02X\n", i, wavetable_mem[i]);
        }
    }
    printf("... (continued)\n");
    
    // Create instance of the Verilog model
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
    // Create VCD trace file if required
    VerilatedVcdC* tfp = nullptr;
    if (dump_vcd) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);
        tfp->open("doc5503_unhalt_trace.vcd");
    }
    tfp_global = tfp;

    // Clock generation
    bool clk = false;
    clk_global = clk;
    
    // Initialize inputs
    dut->reset_n_i = 0;
    dut->clk_i = clk;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Reset for a few cycles
    int sim_time = 0;
    printf("Starting reset sequence...\n");
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        dut->clk_i = clk;
        dut->eval();
        if (dump_vcd) tfp->dump(sim_time++);
    }
    
    // Release reset
    dut->reset_n_i = 1;
    printf("Reset complete. Configuring oscillator...\n");
    
    // Variables to store audio samples
    std::vector<int16_t> audio_samples_left;
    std::vector<int16_t> audio_samples_right;
    
    // UNHALT TEST: Try different ways to un-halt the oscillator
    
    // Strategy 1: Set oscillator to channel 0 (right) and explicitly clear halt bit
    printf("\n--- STRATEGY 1: Set Channel 0 (right), explicitly clear halt bit ---\n");
    
    // Step 1: Set up oscillator with very clear parameters
    // Set high frequency to see movement faster
    write_register(dut, 0x00, 0xFF, &sim_time, &clk, tfp); // Freq Low = 0xFF (max)
    write_register(dut, 0x20, 0x0F, &sim_time, &clk, tfp); // Freq High = 0x0F (fairly high)
    
    // Set maximum volume
    write_register(dut, 0x40, 0xFF, &sim_time, &clk, tfp); // Volume = 0xFF (max)
    
    // Set waveform to page 0
    write_register(dut, 0x80, 0x00, &sim_time, &clk, tfp); // Waveform pointer = 0
    
    // Set 256-byte table, lowest resolution for simplicity
    write_register(dut, 0xC0, 0x00, &sim_time, &clk, tfp); // Resolution/Table Size = 0
    
    // Enable 1 oscillator
    write_register(dut, 0xE1, 0x02, &sim_time, &clk, tfp); // Oscillator Enable = 2 (1 oscillator)
    
    // Check initial control register value (expect halt bit set)
    uint8_t control_reg;
    read_register(dut, 0xA0, &control_reg, &sim_time, &clk, tfp);
    printf("Control register BEFORE: 0x%02X (Halted: %s, Channel: %d)\n", 
           control_reg, (control_reg & 0x01) ? "YES" : "NO", (control_reg >> 4) & 0x0F);
    
    // Step 2: Set control register with channel 0, un-halted (0x00)
    printf("Setting control register to 0x00 (Channel 0, not halted)...\n");
    write_register(dut, 0xA0, 0x00, &sim_time, &clk, tfp); // Control = 0x00 (right channel, not halted)
    
    // Check if successful
    read_register(dut, 0xA0, &control_reg, &sim_time, &clk, tfp);
    printf("Control register AFTER: 0x%02X (Halted: %s, Channel: %d)\n", 
           control_reg, (control_reg & 0x01) ? "YES" : "NO", (control_reg >> 4) & 0x0F);
    
    // Run simulation for a short while to see if it produces output
    for (int i = 0; i < 5000; i++) {
        clk = !clk;
        dut->clk_i = clk;
        clk_global = clk;
        
        // On rising edge
        if (clk) {
            // Generate a DOC clk_en_i pulse every DOC_CLK_DIV cycles
            dut->clk_en_i = ((sim_time % DOC_CLK_DIV) == 0);
            
            // Provide wave data when requested
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o % WAVETABLE_SIZE;
                uint8_t value = wavetable_mem[addr];
                
                dut->wave_data_i = value;
                dut->wave_data_ready_i = 1;
                
                printf("Time %d: Wave read at 0x%04X, providing 0x%02X\n", sim_time, addr, value);
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio sample on every 100th cycle
            if ((sim_time % SAMPLE_COLLECT_INTERVAL) == 0) {
                audio_samples_left.push_back(dut->left_mix_o);
                audio_samples_right.push_back(dut->right_mix_o);
                
                if ((sim_time % TRACE_INTERVAL) == 0) {
                    print_debug_info(dut, &sim_time, &clk);
                }
            }
        }
        
        // Evaluate model
        dut->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Strategy 2: Try setting up 8 oscillators in sync mode
    printf("\n--- STRATEGY 2: Set up oscillator 0 in free run mode, explicit channel ---\n");
    
    // Reset everything 
    dut->reset_n_i = 0;
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        dut->clk_i = clk;
        dut->eval();
        if (dump_vcd) tfp->dump(sim_time++);
    }
    dut->reset_n_i = 1;
    
    // Set oscillator 0 frequency registers 
    write_register(dut, 0x00, 0xFF, &sim_time, &clk, tfp); // Freq Low (osc 0)
    write_register(dut, 0x20, 0x0F, &sim_time, &clk, tfp); // Freq High (osc 0)
    
    // Set oscillator 0 volume to maximum
    write_register(dut, 0x40, 0xFF, &sim_time, &clk, tfp); // Volume (osc 0)
    
    // Set oscillator 0 waveform pointer
    write_register(dut, 0x80, 0x00, &sim_time, &clk, tfp); // Waveform Pointer (osc 0)
    
    // Set oscillator 0 control register - explicitly channel 0, free run, NOT halted
    write_register(dut, 0xA0, 0x00, &sim_time, &clk, tfp); // Control (osc 0)
    
    // Set oscillator 0 resolution/table size (256 byte table)
    write_register(dut, 0xC0, 0x00, &sim_time, &clk, tfp); // Resolution/Table Size (osc 0)
    
    // Enable just 1 oscillator for simplicity
    write_register(dut, 0xE1, 0x02, &sim_time, &clk, tfp); // Oscillator Enable (1 oscillator)
    
    // Verify settings
    printf("Verifying oscillator 0 settings:\n");
    uint8_t reg_val;
    
    read_register(dut, 0x00, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Freq Low: 0x%02X\n", reg_val);
    
    read_register(dut, 0x20, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Freq High: 0x%02X\n", reg_val);
    
    read_register(dut, 0x40, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Volume: 0x%02X\n", reg_val);
    
    read_register(dut, 0x80, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Waveform Pointer: 0x%02X\n", reg_val);
    
    read_register(dut, 0xA0, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Control: 0x%02X\n", reg_val);
    
    read_register(dut, 0xC0, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Resolution/Table Size: 0x%02X\n", reg_val);
    
    read_register(dut, 0xE1, &reg_val, &sim_time, &clk, tfp);
    printf("Oscillator Enable: 0x%02X\n", reg_val);
    
    // Try again to write the control register to un-halt it
    printf("Trying to clear halt bit again...\n");
    write_register(dut, 0xA0, 0x00, &sim_time, &clk, tfp);
    read_register(dut, 0xA0, &reg_val, &sim_time, &clk, tfp);
    printf("Osc 0 Control after second write: 0x%02X\n", reg_val);
    
    // Run simulation for a while and check for output
    printf("Running simulation for oscillator 0...\n");
    
    for (int i = 0; i < 10000; i++) {
        clk = !clk;
        dut->clk_i = clk;
        clk_global = clk;
        
        // On rising edge
        if (clk) {
            // Generate a DOC clk_en_i pulse every DOC_CLK_DIV cycles
            dut->clk_en_i = ((sim_time % DOC_CLK_DIV) == 0);
            
            // Provide wave data when requested
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o % WAVETABLE_SIZE;
                uint8_t value = wavetable_mem[addr];
                
                dut->wave_data_i = value;
                dut->wave_data_ready_i = 1;
                
                printf("Time %d: Wave read at 0x%04X, providing 0x%02X\n", sim_time, addr, value);
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio sample periodically
            if ((sim_time % SAMPLE_COLLECT_INTERVAL) == 0) {
                audio_samples_left.push_back(dut->left_mix_o);
                audio_samples_right.push_back(dut->right_mix_o);
                
                if ((sim_time % TRACE_INTERVAL) == 0) {
                    print_debug_info(dut, &sim_time, &clk);
                }
            }
            
            // Attempt to continuously un-halt the oscillator
            if ((sim_time % 500) == 0 && sim_time < 5000) {
                printf("Time %d: Attempting to clear halt bit\n", sim_time);
                write_register(dut, 0xA0, 0x00, &sim_time, &clk, tfp);
                read_register(dut, 0xA0, &reg_val, &sim_time, &clk, tfp);
                printf("Control register after write: 0x%02X (Halted: %s)\n", 
                       reg_val, (reg_val & 0x01) ? "YES" : "NO");
            }
        }
        
        // Evaluate model
        dut->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Generate WAV files for analysis
    generate_wav_file(audio_samples_left, "doc5503_unhalt_left.wav");
    generate_wav_file(audio_samples_right, "doc5503_unhalt_right.wav");
    
    // Check if audio contains non-zero values
    bool has_audio_left = false;
    bool has_audio_right = false;
    
    for (size_t i = 0; i < audio_samples_left.size(); i++) {
        if (audio_samples_left[i] != 0) has_audio_left = true;
        if (audio_samples_right[i] != 0) has_audio_right = true;
    }
    
    printf("\nAudio output analysis:\n");
    printf("Left channel: %s audio\n", has_audio_left ? "Has" : "No");
    printf("Right channel: %s audio\n", has_audio_right ? "Has" : "No");
    printf("Samples collected: %zu\n", audio_samples_left.size());
    
    // Cleanup
    if (dump_vcd) {
        tfp->close();
        delete tfp;
    }
    
    delete dut;
    delete[] wavetable_mem;
    
    return 0;
}

// Helper function to write a register
void write_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t data, int* sim_time, bool* clk, VerilatedVcdC* tfp) {
    // Write to Verilog model
    for (int i = 0; i < 2; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        if (*clk) {
            dut->cs_n_i = 0;
            dut->we_n_i = 0;
            dut->addr_i = addr;
            dut->data_i = data;
        }
        
        dut->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Reset control signals
    for (int i = 0; i < 2; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        if (*clk) {
            dut->cs_n_i = 1;
            dut->we_n_i = 1;
        }
        
        dut->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
}

// Helper function to read a register
void read_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t* data, int* sim_time, bool* clk, VerilatedVcdC* tfp) {
    // Read from Verilog model
    for (int i = 0; i < 2; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        if (*clk) {
            dut->cs_n_i = 0;
            dut->we_n_i = 1;
            dut->addr_i = addr;
        }
        
        dut->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Capture the data
    *data = dut->data_o;
    
    // Reset control signals
    for (int i = 0; i < 2; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        if (*clk) {
            dut->cs_n_i = 1;
        }
        
        dut->eval();
        if (dump_vcd) tfp->dump(*sim_time);
        (*sim_time)++;
    }
}

// Helper function to print debug info
void print_debug_info(Vdoc5503_harness* dut, int* sim_time, bool* clk) {
    printf("\n--- Debug Trace at sim_time=%d ---\n", *sim_time);
    printf("Audio outputs: left=%d, right=%d, mono=%d\n", 
           dut->left_mix_o, dut->right_mix_o, dut->mono_mix_o);
    
    // Read some registers to see current state
    uint8_t reg_data;
    
    // Check control register - is the oscillator halted?
    read_register(dut, 0xA0, &reg_data, sim_time, clk, tfp_global);
    printf("Control register (0xA0): 0x%02X (Halted: %s, Channel: %d)\n", 
           reg_data, (reg_data & 0x01) ? "YES" : "NO", (reg_data >> 4) & 0x0F);
    
    // Check waveform data sample register - what's the current sample?
    read_register(dut, 0x60, &reg_data, sim_time, clk, tfp_global);
    printf("Waveform Data Sample (0x60): 0x%02X\n", reg_data);
    
    // Check oscillator enable register
    read_register(dut, 0xE1, &reg_data, sim_time, clk, tfp_global);
    printf("Oscillator Enable (0xE1): 0x%02X (# oscillators: %d)\n", 
           reg_data, reg_data >> 1);
    
    printf("----------------------------------------\n");
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
    
    if (has_valid_samples) {
        printf("Generated %s with %zu samples (contains non-zero values)\n", 
               filename.c_str(), samples.size());
    } else {
        printf("WARNING: Generated %s with %zu samples (ALL ZEROS!)\n", 
               filename.c_str(), samples.size());
    }
}