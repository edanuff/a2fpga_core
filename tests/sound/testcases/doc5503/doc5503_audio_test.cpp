#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required for linking - provides simulation time to verilator
double sc_time_stamp() { return 0; }

// Maximum simulation time
#define MAX_SIM_TIME 300000

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

// Clock handling structure to keep state together
struct ClockHandler {
    bool clk;               // Current clock state
    int clk_div8_counter;   // Counter for clk_en_i generation (0-7)
    int sim_time;           // Current simulation time
    
    // Constructor to initialize state
    ClockHandler() : clk(false), clk_div8_counter(0), sim_time(0) {}
    
    // Advance clock by one edge and handle clk_en_i generation
    void tick(Vdoc5503_harness* dut, VerilatedVcdC* tfp) {
        // Toggle clock
        clk = !clk;
        dut->clk_i = clk;
        
        // Only on rising edge, update counter and set clk_en_i
        if (clk) {
            clk_div8_counter = (clk_div8_counter + 1) % 8;
            dut->clk_en_i = (clk_div8_counter == 0) ? 1 : 0;
        }
        
        // Evaluate model and dump VCD if needed
        dut->eval();
        if (tfp) tfp->dump(sim_time);
        sim_time++;
    }
    
    // Advance clock by N cycles (2N edges)
    void cycles(Vdoc5503_harness* dut, int n, VerilatedVcdC* tfp) {
        for (int i = 0; i < n * 2; i++) {
            tick(dut, tfp);
        }
    }
};

// Forward declarations for register access
void write_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t data, ClockHandler& clk, VerilatedVcdC* tfp);
uint8_t read_register(Vdoc5503_harness* dut, uint8_t addr, ClockHandler& clk, VerilatedVcdC* tfp);
bool wait_for_ready(Vdoc5503_harness* dut, ClockHandler& clk, VerilatedVcdC* tfp, int timeout_cycles = 1000);
void setup_oscillator(Vdoc5503_harness* dut, int osc_num, ClockHandler& clk, VerilatedVcdC* tfp);
void provide_wave_data(Vdoc5503_harness* dut, uint16_t addr, uint8_t data);
void generate_wav_file(const std::vector<int16_t>& samples, const std::string& filename);

// Main simulation loop
int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the top-level model
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
    // Create VCD trace file if required
    VerilatedVcdC* tfp = nullptr;
    if (dump_vcd) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);
        tfp->open("doc5503_audio_test.vcd");
    }
    
    // Initialize our clock handler
    ClockHandler clk;

    // Initialize inputs
    dut->reset_n_i = 0;
    dut->clk_i = clk.clk;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Reset for 10 cycles (20 edges)
    clk.cycles(dut, 10, tfp);
    
    // Release reset
    dut->reset_n_i = 1;
    
    // Wait for the ready_o signal to go high
    printf("Waiting for DOC5503 module to be ready...\n");
    if (!wait_for_ready(dut, clk, tfp)) {
        printf("ERROR: Timeout waiting for ready_o signal!\n");
        delete dut;
        if (tfp) delete tfp;
        return 1;
    }
    printf("DOC5503 module is ready, starting tests.\n\n");
    
    // Give a few more cycles as buffer
    clk.cycles(dut, 16, tfp);
    
    printf("=== DOC5503 Simple Audio Generation Test ===\n\n");
    
    // Set oscillator enable register to enable only oscillator 0
    printf("Setting oscillator enable register to enable oscillator 0...\n");
    write_register(dut, 0xE1, 0x02, clk, tfp); // Value 0x02 enables oscillator 0

    // Read back the oscillator enable register to make sure it was set
    uint8_t osc_en_val = read_register(dut, 0xE1, clk, tfp);
    printf("Oscillator enable register after write: 0x%02X (should be 0x02)\n", osc_en_val);

    // Setup oscillator 0 for a simple triangle waveform
    printf("Setting up oscillator 0 for audio generation...\n");

    // Print debugging info about the oscillator state before setup
    printf("Oscillator state before setup:\n");
    printf("- Enable register: 0x%02X\n", osc_en_val);

    // Make sure the DOC is really enabled
    for (int i = 0; i < 1000; i++) {
        clk.tick(dut, tfp);

        // Check if the DOC is requesting wave data every tick
        if (dut->wave_rd_o) {
            printf("!!! WAVE_RD_O HIGH during DOC init at time %d, addr=0x%04x !!!\n",
                  clk.sim_time, dut->wave_address_o);
        }
    }

    setup_oscillator(dut, 0, clk, tfp);

    // After setup, check if the oscillator is still properly configured
    printf("Checking oscillator state after setup...\n");
    uint8_t ctl_val_after = read_register(dut, 0xA0, clk, tfp);
    printf("Control register AFTER cycles: 0x%02X (halt bit: %d)\n",
           ctl_val_after, ctl_val_after & 0x01);

    // Monitor debug registers for oscillator state
    printf("DEBUG SIGNALS: Oscillator=%d, State=%d, Volume=0x%02X, WDS=0x%02X\n",
           dut->dbg_cycle_o, dut->dbg_osc_state_o, dut->dbg_vol_o, dut->dbg_wds_o);

    if (ctl_val_after & 0x01) {
        printf("WARNING: Oscillator appears to be HALTED despite our attempt to enable it!\n");
        printf("This matches the known issue in DOC5503_Oscillator_Halting_Issue.md\n");

        // Attempt to fix the issue by writing to WDS register first with non-zero value
        printf("Attempting fix: Setting WDS to non-zero value (0x80 for zero crossing)...\n");
        write_register(dut, 0xC0, 0x80, clk, tfp);
        clk.cycles(dut, 8, tfp);

        // Then clear the halt bit
        printf("Clearing the halt bit in control register...\n");
        write_register(dut, 0xA0, 0x10, clk, tfp);
        clk.cycles(dut, 8, tfp);

        // Check if the fix worked
        uint8_t ctl_val_fixed = read_register(dut, 0xA0, clk, tfp);
        printf("Control register after fix: 0x%02X (halt bit: %d)\n",
               ctl_val_fixed, ctl_val_fixed & 0x01);
    }

    // Wait for oscillator to start with extra debugging
    printf("Giving extra time for oscillator to start...\n");
    for (int i = 0; i < 1000; i++) {
        // Evaluate the model BEFORE checking signals to catch any changes
        dut->eval();

        // Check if the DOC is requesting wave data - check at the start of each tick
        if (dut->wave_rd_o) {
            printf("!!! WAVE_RD_O HIGH after oscillator setup at time %d, addr=0x%04x !!!\n",
                  clk.sim_time, dut->wave_address_o);

            // If we see a wave_rd_o signal, respond immediately with wave data
            // to avoid missing the short pulse
            uint16_t addr = dut->wave_address_o & 0xFF;
            uint8_t value = (addr < 128) ? (0x80 + addr) : (0xFF - addr + 0x80); // Triangle wave

            // Provide wave data
            dut->wave_data_i = value;
            dut->wave_data_ready_i = 1;
            dut->eval();  // Process the response

            printf("Provided wave data 0x%02X in response to wave_rd_o\n", value);
        }

        // Advance the clock, checking for wave_rd_o after the clock edge too
        clk.tick(dut, tfp);

        // Check again after the clock edge
        if (dut->wave_rd_o) {
            printf("!!! WAVE_RD_O HIGH AFTER CLOCK EDGE at time %d, addr=0x%04x !!!\n",
                  clk.sim_time, dut->wave_address_o);

            uint16_t addr = dut->wave_address_o & 0xFF;
            uint8_t value = (addr < 128) ? (0x80 + addr) : (0xFF - addr + 0x80);

            dut->wave_data_i = value;
            dut->wave_data_ready_i = 1;
            dut->eval();

            printf("Provided wave data 0x%02X in response to wave_rd_o\n", value);
        } else {
            // Make sure to deassert wave_data_ready_i when wave_rd_o is not active
            dut->wave_data_ready_i = 0;
            dut->eval();
        }

        // Check every 100 cycles for a change in the control register's halt bit
        if (i % 100 == 0) {
            uint8_t curr_ctl = read_register(dut, 0xA0, clk, tfp);
            printf("Control register at cycle %d: 0x%02X (halt bit: %d)\n",
                   i, curr_ctl, curr_ctl & 0x01);
        }

        // Specifically check around time points where wave_rd_o signals might be active
        if (clk.sim_time >= 148 && clk.sim_time <= 158) {
            printf("FOCUS TIME %d: wave_rd_o=%d, wave_address_o=0x%04x, left=%d, right=%d, mono=%d\n",
                   clk.sim_time, dut->wave_rd_o, dut->wave_address_o,
                   dut->left_mix_o, dut->right_mix_o, dut->mono_mix_o);
        }

        // Also check at specific oscillator cycle boundaries (every 32 clocks)
        if (clk.sim_time % 32 == 0 || clk.sim_time % 32 == 1) {
            printf("OSC CYCLE %d: wave_rd_o=%d, wave_address_o=0x%04x\n",
                   clk.sim_time, dut->wave_rd_o, dut->wave_address_o);
        }
    }
    
    // Run simulation to generate audio
    printf("Running simulation to generate audio...\n");
    std::vector<int16_t> audio_samples;
    
    // Duration of audio to generate (cycles) - longer to give more time for oscillators to run
    const int sim_duration = 200000;
    
    // Collect audio samples periodically
    for (int i = 0; i < sim_duration; i++) {
        // Evaluate the model before checking the wave_rd_o signal to catch it in the current state
        dut->eval();

        // Check if the DOC is requesting wave data - IMPORTANT: Check BEFORE clocking
        if (dut->wave_rd_o) {
            uint16_t addr = dut->wave_address_o & 0xFF; // Get low 8 bits as sample index

            // Create a triangle wave pattern based on address
            uint8_t value;
            if (addr < 128) {
                value = 0x80 + addr; // Rising part
            } else {
                value = 0xFF - addr + 0x80; // Falling part
            }

            printf("!!! WAVE_RD_O HIGH at time %d, addr=0x%04x !!!\n", clk.sim_time, dut->wave_address_o);

            // Provide the wave data immediately
            dut->wave_data_i = value;
            dut->wave_data_ready_i = 1;
            dut->eval();

            printf("DEBUG: Provided wave data for address 0x%04x: 0x%02x (wave_rd_o active!)\n",
                  dut->wave_address_o, value);

            // Don't deassert data_ready yet - continue with regular clocking
        } else {
            // Make sure to deassert data_ready when read is not active
            dut->wave_data_ready_i = 0;
        }

        // Advance the clock
        clk.tick(dut, tfp);

        // Check again AFTER clock edge for wave_rd_o
        if (dut->wave_rd_o) {
            uint16_t addr = dut->wave_address_o & 0xFF;
            uint8_t value = (addr < 128) ? (0x80 + addr) : (0xFF - addr + 0x80);

            printf("!!! WAVE_RD_O HIGH after clock at time %d, addr=0x%04x !!!\n", clk.sim_time, dut->wave_address_o);

            dut->wave_data_i = value;
            dut->wave_data_ready_i = 1;
            dut->eval();

            printf("DEBUG: Provided wave data after clock for address 0x%04x: 0x%02x\n",
                  dut->wave_address_o, value);
        } else {
            // Make sure to deassert data_ready when wave_rd_o is not active
            dut->wave_data_ready_i = 0;
            dut->eval();
        }

        // Check for wave_rd_o at specific simulation times and cycle boundaries
        if ((clk.sim_time >= 148 && clk.sim_time <= 158) ||
            (clk.sim_time % 32 == 0 || clk.sim_time % 32 == 1)) {
            printf("SIM TIME %d: wave_rd_o=%d, wave_address_o=0x%04x, left=%d, right=%d\n",
                   clk.sim_time, dut->wave_rd_o, dut->wave_address_o,
                   dut->left_mix_o, dut->right_mix_o);
        }

        // Debug audio output periodically
        if (i % 100 == 0) {
            printf("DEBUG: At time %d: left_mix_o = %d, right_mix_o = %d, mono_mix_o = %d, wave_rd_o = %d, wave_address_o = 0x%04x\n",
                   i, dut->left_mix_o, dut->right_mix_o, dut->mono_mix_o, dut->wave_rd_o, dut->wave_address_o);
        }
        
        // Collect audio samples (every 128 cycles to get a reasonable sized output)
        if (i % 128 == 0) {
            audio_samples.push_back(dut->left_mix_o);
            
            if (verbosity >= DEBUG && (i % 1024 == 0)) {
                printf("DEBUG: Sample %d: left_mix_o = %d\n", 
                       (int)audio_samples.size() - 1, dut->left_mix_o);
            }
        }
    }
    
    // Generate WAV file from samples
    generate_wav_file(audio_samples, "doc5503_audio_test.wav");
    printf("Generated audio file: doc5503_audio_test.wav\n");
    
    // Cleanup
    if (tfp) {
        tfp->close();
        delete tfp;
    }
    
    delete dut;
    
    return 0;
}

// Function to wait for the ready_o signal to go high
bool wait_for_ready(Vdoc5503_harness* dut, ClockHandler& clk, VerilatedVcdC* tfp, int timeout_cycles) {
    bool ready = false;
    int timeout = 0;
    
    while (!ready && timeout < timeout_cycles) {
        clk.tick(dut, tfp);
        
        // Check ready signal only on rising edge
        if (clk.clk) {
            ready = dut->ready_o;
            
            if (ready && verbosity >= INFO) {
                printf("DEBUG: ready_o went high at sim_time=%d\n", clk.sim_time);
            }
        }
        
        // Only count full cycles
        if (!clk.clk) {
            timeout++;
        }
    }
    
    return ready;
}

// Helper function to setup an oscillator for audio generation
void setup_oscillator(Vdoc5503_harness* dut, int osc_num, ClockHandler& clk, VerilatedVcdC* tfp) {
    // Calculate register addresses
    uint8_t fl_addr = 0x00 | (osc_num & 0x1F);  // Frequency Low Byte
    uint8_t fh_addr = 0x20 | (osc_num & 0x1F);  // Frequency High Byte
    uint8_t vol_addr = 0x40 | (osc_num & 0x1F); // Volume
    uint8_t wtp_addr = 0x80 | (osc_num & 0x1F); // Waveform Table Pointer
    uint8_t ctl_addr = 0xA0 | (osc_num & 0x1F); // Control
    uint8_t rts_addr = 0xC0 | (osc_num & 0x1F); // Resolution/Table Size

    printf("Setting up oscillator %d registers:\n", osc_num);
    printf("- FL_ADDR = 0x%02X\n", fl_addr);
    printf("- FH_ADDR = 0x%02X\n", fh_addr);
    printf("- VOL_ADDR = 0x%02X\n", vol_addr);
    printf("- WTP_ADDR = 0x%02X\n", wtp_addr);
    printf("- CTL_ADDR = 0x%02X\n", ctl_addr);
    printf("- RTS_ADDR = 0x%02X\n", rts_addr);

    // Set frequency to maximum value to quickly overflow accumulator and try to get wave data
    // Freq = 0xFFFF (fastest possible frequency value)
    printf("Setting frequency low byte to 0xFF...\n");
    write_register(dut, fl_addr, 0xFF, clk, tfp);
    clk.cycles(dut, 4, tfp);  // Give more cycles between operations

    printf("Setting frequency high byte to 0xFF...\n");
    write_register(dut, fh_addr, 0xFF, clk, tfp);
    clk.cycles(dut, 4, tfp);

    // Set volume (maximum volume for testing)
    printf("Setting volume to maximum (0xFF)...\n");
    write_register(dut, vol_addr, 0xFF, clk, tfp);  // Maximum volume
    clk.cycles(dut, 4, tfp);

    // Set waveform table pointer (page 0)
    printf("Setting waveform table pointer to page 0...\n");
    write_register(dut, wtp_addr, 0x00, clk, tfp);
    clk.cycles(dut, 4, tfp);

    // Set resolution/table size first (some DOCs need this set before control)
    printf("Setting resolution/table size...\n");
    // - Bits 2-0: 000 = 256-byte wavetable (standard resolution)
    // - Bits 5-3: 000 = Standard table size
    write_register(dut, rts_addr, 0x00, clk, tfp);
    clk.cycles(dut, 4, tfp);

    // IMPORTANT: Apply the fix recommended in DOC5503_Oscillator_Halting_Issue.md
    // Set wave data sample register to non-zero value (0x80) to avoid zero-detection halt
    printf("Setting WDS to non-zero value (0x80) to prevent halting...\n");
    // Access the WDS register directly - not a register in the normal address space
    // Let's try writing to oscillator 0's internal RAM directly
    write_register(dut, 0xE0, 0x80, clk, tfp);  // Set all oscillators' WDS to 0x80
    clk.cycles(dut, 4, tfp);

    // Set control register:
    // - Bits 1-0: 00 = Free running mode (not halt/one-shot/sync/swap)
    // - Bits 3-2: 00 = Reserved
    // - Bits 7-4: 0001 = Channel 1 (left)
    // IMPORTANT: Bit 0 = 0 means NOT halted (oscillator is enabled to run)
    printf("Setting control register to 0x10 (free running, channel 1)...\n");
    write_register(dut, ctl_addr, 0x10, clk, tfp);
    clk.cycles(dut, 4, tfp);

    // Double-check all registers were set correctly
    uint8_t fl_val = read_register(dut, fl_addr, clk, tfp);
    uint8_t fh_val = read_register(dut, fh_addr, clk, tfp);
    uint8_t vol_val = read_register(dut, vol_addr, clk, tfp);
    uint8_t wtp_val = read_register(dut, wtp_addr, clk, tfp);
    uint8_t ctl_val = read_register(dut, ctl_addr, clk, tfp);
    uint8_t rts_val = read_register(dut, rts_addr, clk, tfp);

    printf("Register values after setup:\n");
    printf("- FL = 0x%02X (should be 0xFF)\n", fl_val);
    printf("- FH = 0x%02X (should be 0xFF)\n", fh_val);
    printf("- VOL = 0x%02X (should be 0xFF)\n", vol_val);
    printf("- WTP = 0x%02X (should be 0x00)\n", wtp_val);
    printf("- CTL = 0x%02X (should be 0x10, halt bit: %d)\n", ctl_val, ctl_val & 0x01);
    printf("- RTS = 0x%02X (should be 0x00)\n", rts_val);

    clk.cycles(dut, 8, tfp);  // Give more cycles for changes to take effect

    printf("Oscillator %d configured with:\n", osc_num);
    printf("  Frequency: 0x%02X%02X (maximum for fast testing)\n", fh_val, fl_val);
    printf("  Volume: 0x%02X (maximum)\n", vol_val);
    printf("  Control: 0x%02X (free running, channel 1)\n", ctl_val);
    printf("  Res/Table Size: 0x%02X (256-byte table)\n", rts_val);
}

// Helper function to provide wave data when requested
void provide_wave_data(Vdoc5503_harness* dut, uint16_t addr, uint8_t data) {
    // Set up the wave data response
    dut->wave_data_i = data;
    dut->wave_data_ready_i = 1;

    // Update module state with new inputs
    dut->eval();

    // IMPORTANT: The DOC5503 needs to see data_ready high for at least one full clock cycle
    // The wave_data_ready_i will be deasserted by the caller after appropriate timing

    // Print detailed debug information
    printf("DEBUG: Wave data response setup - data_i=0x%02x, data_ready_i=1\n", data);
}

// Helper function to write a register value
void write_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t data, ClockHandler& clk, VerilatedVcdC* tfp) {
    // Debug print before operation
    if (verbosity >= DEBUG) {
        printf("DEBUG: Starting write to register 0x%02X: 0x%02X\n", addr, data);
    }
    
    // Assert chip select and write enable
    dut->cs_n_i = 0;
    dut->we_n_i = 0;
    dut->addr_i = addr;
    dut->data_i = data;
    
    // Give this setup time to propagate before clock edge
    dut->eval();
    
    // Advance the clock for 2 full cycles
    for (int i = 0; i < 4; i++) {
        clk.tick(dut, tfp);
    }
    
    // Deassert signals
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->eval();
    
    // Advance the clock for 1 more cycle
    for (int i = 0; i < 2; i++) {
        clk.tick(dut, tfp);
    }
    
    if (verbosity >= DEBUG) {
        printf("DEBUG: Completed write to register 0x%02X: 0x%02X\n", addr, data);
    }
}

// Helper function to read a register value
uint8_t read_register(Vdoc5503_harness* dut, uint8_t addr, ClockHandler& clk, VerilatedVcdC* tfp) {
    uint8_t read_val = 0;
    
    // Debug print before operation
    if (verbosity >= DEBUG) {
        printf("DEBUG: Starting read from register 0x%02X\n", addr);
    }
    
    // Set up the read cycle
    dut->cs_n_i = 0;
    dut->we_n_i = 1;  // Read operation
    dut->addr_i = addr;
    
    // Give this setup time to propagate
    dut->eval();
    
    // Advance the clock and capture data from each rising edge
    for (int i = 0; i < 8; i++) {
        clk.tick(dut, tfp);
        
        if (clk.clk) {  // On rising edge
            read_val = dut->data_o;
            
            if (verbosity >= DEBUG && i == 0) {
                printf("DEBUG: First rising edge, data_o=0x%02X\n", read_val);
            }
        }
    }
    
    // Deassert signals
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->eval();
    
    // Advance the clock for 1 more cycle
    for (int i = 0; i < 2; i++) {
        clk.tick(dut, tfp);
    }
    
    if (verbosity >= DEBUG) {
        printf("DEBUG: Completed read from register 0x%02X: 0x%02X\n", addr, read_val);
    }
    
    return read_val;
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
    int min_val = 0, max_val = 0;
    double avg_val = 0.0;
    int non_zero_count = 0;
    
    for (auto sample : samples) {
        if (sample != 0) {
            has_valid_samples = true;
            non_zero_count++;
            
            // Update min/max/avg statistics
            if (min_val == 0 || sample < min_val) min_val = sample;
            if (max_val == 0 || sample > max_val) max_val = sample;
            avg_val += sample;
        }
    }
    
    if (samples.size() > 0) {
        avg_val /= samples.size();
    }
    
    if (!has_valid_samples) {
        printf("WARNING: All samples are zero in %s\n", filename.c_str());
    } else {
        printf("Sample statistics: min=%d, max=%d, avg=%.2f, non_zero=%d (%.1f%%)\n", 
               min_val, max_val, avg_val, non_zero_count, 
               (float)non_zero_count / samples.size() * 100);
    }
    
    // WAV file header
    const int sample_rate = 22050;
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
    
    printf("WAV file generated: %s (%zu samples)\n", filename.c_str(), samples.size());
}