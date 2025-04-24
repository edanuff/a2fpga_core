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
const int MAX_SIM_TIME = 100000;
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
void print_debug_info(Vdoc5503_harness* dut, int sim_time);

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create the wavetable memory - simple triangle wave for testing
    uint8_t* wavetable_mem = new uint8_t[WAVETABLE_SIZE];
    
    printf("Initializing wavetable with triangle wave pattern...\n");
    for (int i = 0; i < WAVETABLE_SIZE; i++) {
        // Triangle wave: 0x80 is silence, values above are positive, below are negative
        wavetable_mem[i] = 0x80 + (i < WAVETABLE_SIZE/2 ? i : (WAVETABLE_SIZE-1 - i));
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
        tfp->open("doc5503_debug_trace.vcd");
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
    
    // Setup for a very basic test - single oscillator with extreme settings
    // to make any output very obvious
    
    // Set oscillator frequency - high value for clear oscillation
    // FL = 0xFF, FH = 0x00 for a moderate frequency
    write_register(dut, 0x00, 0xFF, &sim_time, &clk, tfp); // Freq Low
    write_register(dut, 0x20, 0x01, &sim_time, &clk, tfp); // Freq High
    
    // Set maximum volume for clear signal
    write_register(dut, 0x40, 0xFC, &sim_time, &clk, tfp); // Volume
    
    // Set oscillator to use page 0 of waveform
    write_register(dut, 0x80, 0x00, &sim_time, &clk, tfp); // Waveform pointer
    
    // Set control register - channel 0 (right), no halt, free-running mode
    // 0x00 = 0000 0000 = Channel 0 (right), oscillator running
    // IMPORTANT: bit 0 is the halt bit (0=run, 1=halt)
    write_register(dut, 0xA0, 0x00, &sim_time, &clk, tfp);
    
    // Read back the control register to verify it was set correctly
    uint8_t control_reg;
    read_register(dut, 0xA0, &control_reg, &sim_time, &clk, tfp);
    printf("Control register after write: 0x%02X\n", control_reg);
    
    // If the control register isn't set correctly, try writing again with an explicit NOT HALT
    if (control_reg & 0x01) { 
        printf("WARNING: Control register still has HALT bit set, trying again...\n");
        write_register(dut, 0xA0, 0x10, &sim_time, &clk, tfp);
        read_register(dut, 0xA0, &control_reg, &sim_time, &clk, tfp);
        printf("Control register after second write: 0x%02X\n", control_reg);
    } // Control
    
    // Set 256-byte table, lowest resolution
    write_register(dut, 0xC0, 0x00, &sim_time, &clk, tfp); // Resolution/Table Size
    
    // Enable 1 oscillator
    write_register(dut, 0xE1, 0x02, &sim_time, &clk, tfp); // Oscillator Enable
    
    // Verify settings by reading back
    uint8_t read_data;
    
    printf("\nVerifying register settings:\n");
    read_register(dut, 0x00, &read_data, &sim_time, &clk, tfp);
    printf("Freq Low (0x00): Expected 0xFF, Got 0x%02X\n", read_data);
    
    read_register(dut, 0x20, &read_data, &sim_time, &clk, tfp);
    printf("Freq High (0x20): Expected 0x01, Got 0x%02X\n", read_data);
    
    read_register(dut, 0x40, &read_data, &sim_time, &clk, tfp);
    printf("Volume (0x40): Expected 0xFC, Got 0x%02X\n", read_data);
    
    read_register(dut, 0x80, &read_data, &sim_time, &clk, tfp);
    printf("Waveform Ptr (0x80): Expected 0x00, Got 0x%02X\n", read_data);
    
    read_register(dut, 0xA0, &read_data, &sim_time, &clk, tfp);
    printf("Control (0xA0): Expected 0x00, Got 0x%02X\n", read_data);
    
    read_register(dut, 0xC0, &read_data, &sim_time, &clk, tfp);
    printf("Res/Table (0xC0): Expected 0x00, Got 0x%02X\n", read_data);
    
    read_register(dut, 0xE1, &read_data, &sim_time, &clk, tfp);
    printf("Osc Enable (0xE1): Expected 0x02, Got 0x%02X\n", read_data);
    
    printf("\nRunning simulation with debug tracing...\n");
    
    // Run simulation and collect samples
    int trace_countdown = 0;
    for (int i = 0; i < MAX_SIM_TIME; i++) {
        clk = !clk;
        dut->clk_i = clk;
        
        // On rising edge
        if (clk) {
            clk_global = clk;
            // Generate a DOC clk_en_i pulse every DOC_CLK_DIV cycles
            dut->clk_en_i = ((sim_time % DOC_CLK_DIV) == 0);
            
            // Provide wave data when requested
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o % WAVETABLE_SIZE;
                // Ensure we're sending a non-zero value - not returning 0x80 (silence)
                // Let's use some waveform values that are easier to spot in the output
                uint8_t value = (addr % 2 == 0) ? 0xA0 : 0x60; // Alternating high and low values
                
                dut->wave_data_i = value;
                dut->wave_data_ready_i = 1;
                
                // Print the address and data with simulation time for tracking
                printf("Time %d: Wave read at 0x%04X, providing 0x%02X\n", sim_time, addr, value);
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio samples periodically
            if ((sim_time % SAMPLE_COLLECT_INTERVAL) == 0) {
                audio_samples_left.push_back(dut->left_mix_o);
                audio_samples_right.push_back(dut->right_mix_o);
                
                if (--trace_countdown <= 0) {
                    print_debug_info(dut, sim_time);
                    trace_countdown = TRACE_INTERVAL;
                }
            }
        }
        
        // Evaluate model
        dut->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Generate WAV files for analysis
    generate_wav_file(audio_samples_left, "doc5503_debug_left.wav");
    generate_wav_file(audio_samples_right, "doc5503_debug_right.wav");
    
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
void print_debug_info(Vdoc5503_harness* dut, int sim_time) {
    printf("\n--- Debug Trace at sim_time=%d ---\n", sim_time);
    printf("Audio outputs: left=%d, right=%d, mono=%d\n", 
           dut->left_mix_o, dut->right_mix_o, dut->mono_mix_o);
    
    // Read some registers to see current state
    uint8_t reg_data;
    
    // Check control register - is the oscillator halted?
    read_register(dut, 0xA0, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Control register (0xA0): 0x%02X (Halted: %s, Channel: %d)\n", 
           reg_data, (reg_data & 0x01) ? "YES" : "NO", (reg_data >> 4) & 0x0F);
    
    // Check waveform data sample register - what's the current sample?
    read_register(dut, 0x60, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Waveform Data Sample (0x60): 0x%02X\n", reg_data);
    
    // Check oscillator enable register
    read_register(dut, 0xE1, &reg_data, &sim_time, &clk_global, tfp_global);
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