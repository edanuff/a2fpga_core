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

// Debug - print oscillator state names for better debugging
const char* osc_state_names[] = {
    "OSC_IDLE",               // 0
    "OSC_START",              // 1
    "OSC_LOAD_PARTNER_CONTROL", // 2
    "OSC_ACC",                // 3
    "OSC_ADDR",               // 4
    "OSC_READ_DATA",          // 5
    "OSC_AM_SYNC_EVEN",       // 6
    "OSC_HALT",               // 7
    "OSC_HALT_PARTNER",       // 8
    "OSC_OUT",                // 9
    "OSC_MIX"                 // 10
};

// Required by Verilator
double sc_time_stamp() { return 0; }

// Configuration
const bool dump_vcd = true;
const int MAX_SIM_TIME = 100000;
const int CLOCK_SPEED_HZ = 54000000;    // 54 MHz main clock 
const int DOC_CLOCK_SPEED_HZ = 7159090; // 7.16 MHz DOC clock
const int DOC_CLK_CYCLES = CLOCK_SPEED_HZ / DOC_CLOCK_SPEED_HZ; // Approx 7-8 main clock cycles per DOC clock
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
        printf("VCD tracing enabled, writing to doc5503_debug_trace.vcd\n");
    } else {
        printf("VCD tracing disabled\n");
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
    
    // Create variables for 7.16MHz clock edge detection
    int clk_counter = 0;
    bool clk7_16M = false;
    int wait_cycles = 0;
    
    // Reset for a few cycles
    int sim_time = 0;
    printf("Starting reset sequence...\n");
    for (int i = 0; i < 20; i++) {
        clk = !clk;
        dut->clk_i = clk;
        dut->eval();
        if (tfp) tfp->dump(sim_time++);
    }
    
    // Release reset
    dut->reset_n_i = 1;
    printf("Reset released. Waiting for DOC5503 to indicate ready...\n");
    
    // Wait for module to indicate it's ready before attempting to write registers
    bool module_ready = false;
    int ready_wait_cycles = 0;
    const int MAX_READY_WAIT = 2000; // Increased wait time
    
    printf("Applying clk_en_i pulses to advance the DOC5503 state machine...\n");
    
    // Make sure we give the DOC5503 module enough clock cycles with proper clk_en_i pulses
    // to ensure it gets out of reset state and starts functioning properly.
    // We'll use proper DOC_CLK_CYCLES timing for the pulses
    printf("Applying properly timed clk_en_i pulses to ensure module initialization...\n");
    clk_counter = 0;
    clk7_16M = false;
    for (int i = 0; i < 200; i++) {
        clk = !clk;
        dut->clk_i = clk;
        
        // Only on rising edge
        if (clk) {
            // Generate clk_en_i pulses with proper DOC_CLK_CYCLES timing
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                clk7_16M = !clk7_16M;
                dut->clk_en_i = 1;
                printf("Pulse clk_en_i at cycle %d\n", i);
            } else {
                dut->clk_en_i = 0;
            }
        } else {
            // Always low on falling edge
            dut->clk_en_i = 0;
        }
        
        dut->eval();
        if (tfp) tfp->dump(sim_time++);
        
        // Print status periodically
        if (i % 20 == 0) {
            printf("Initialization cycle %d: E=%d, clk_count=%d, ready=%d\n", 
                   i, dut->dbg_E_o, dut->dbg_clk_count_o, dut->ready_o);
        }
    }
    
    // Now set clk_en_i low and continue with normal timing
    dut->clk_en_i = 0;
    dut->eval();
    
    printf("Forced clk_en_i high for 100 cycles, now waiting for ready signal...\n");
    printf("Current state: E=%d, clk_count=%d, ready=%d\n", 
           dut->dbg_E_o, dut->dbg_clk_count_o, dut->ready_o);
    
    // Now wait for ready signal with normal clock timing
    while (!module_ready && ready_wait_cycles < MAX_READY_WAIT) {
        clk = !clk;
        dut->clk_i = clk;
        
        // Only check on rising edge
        if (clk) {
            // Apply clock enable with proper DOC_CLK_CYCLES timing
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                dut->clk_en_i = 1;
                
                // Debug periodically
                if (ready_wait_cycles % 50 == 0) {
                    printf("Pulse clk_en_i at cycle %d: E=%d, count=%d, ready=%d\n",
                           ready_wait_cycles, dut->dbg_E_o, dut->dbg_clk_count_o, dut->ready_o);
                }
            } else {
                dut->clk_en_i = 0;
            }
            
            // Check if module is ready or if E has gone high
            if (dut->ready_o || dut->dbg_E_o) {
                module_ready = true;
                printf("DOC5503 module responded after %d cycles! E=%d, clk_count=%d, ready=%d\n", 
                       ready_wait_cycles, dut->dbg_E_o, dut->dbg_clk_count_o, dut->ready_o);
            }
        }
        
        dut->eval();
        if (tfp) tfp->dump(sim_time++);
        ready_wait_cycles++;
        
        // Print status periodically
        if (ready_wait_cycles % 200 == 0) {
            printf("Still waiting... cycle=%d, E=%d, clk_count=%d, ready=%d\n",
                  ready_wait_cycles, dut->dbg_E_o, dut->dbg_clk_count_o, dut->ready_o);
        }
    }
    
    if (!module_ready) {
        printf("WARNING: DOC5503 module did not indicate ready within %d cycles!\n", MAX_READY_WAIT);
    }
    
    printf("Configuring oscillator...\n");
    
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
    write_register(dut, 0x40, 0xFF, &sim_time, &clk, tfp); // Volume - absolute maximum
    
    // Set oscillator to use page 0 of waveform
    write_register(dut, 0x80, 0x00, &sim_time, &clk, tfp); // Waveform pointer
    
    // Set control register - channel 1 (left), no halt, free-running mode
    // 0x10 = 0001 0000 = Channel 1 (left), oscillator running
    // IMPORTANT: bit 0 is the halt bit (0=run, 1=halt)
    // Bits 7:4 = Channel assignment (0-15)
    // Bits 2:1 = Mode (00=free run, 01=one-shot, 10=sync AM, 11=swap)
    // Bit 0 = Halt (0=run, 1=halt)
    write_register(dut, 0xA0, 0x10, &sim_time, &clk, tfp);
    
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
    
    // Enable 1 oscillator (0x02 means osc 0 is enabled since it's 2 >> 1 = 1)
    // Bits 7:1 determine how many oscillators are enabled (divided by 2)
    // This value (0x02) enables only oscillator 0
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
    printf("Using DOC_CLK_CYCLES = %d (approx cycles between 7.16MHz edges)\n", DOC_CLK_CYCLES);
    
    // Reset existing clock variables 
    clk7_16M = false;
    clk_counter = 0;
    
    // Track E state for better debug visibility
    bool last_e_state = false;
    int e_high_count = 0;
    int e_low_count = 0;
    
    printf("ENHANCED: Main simulation starting with improved clock handling\n");
    
    for (int i = 0; i < MAX_SIM_TIME; i++) {
        clk = !clk;
        dut->clk_i = clk;
        
        // Only execute logic on rising edge of main clock
        if (clk) {
            clk_global = clk;
            
            // Improved approach with better clk_en_i timing:
            // 1. Generate clk_en_i pulses exactly once per DOC_CLK_CYCLES
            // 2. Better align with the module's expected timing
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                // Toggle the 7.16MHz clock for visibility in debugging
                clk7_16M = !clk7_16M;
                
                // Generate a pulse for EXACTLY one clock cycle
                dut->clk_en_i = 1;
            } else {
                dut->clk_en_i = 0;
            }
            
            // Track E signal transitions to better understand timing issues
            if (dut->dbg_E_o != last_e_state) {
                if (dut->dbg_E_o) {
                    printf("DEBUG E TRANSITION: E went HIGH at sim_time=%d (was low for %d cycles)\n", 
                           sim_time, e_low_count);
                    e_low_count = 0;
                    e_high_count = 1;
                } else {
                    printf("DEBUG E TRANSITION: E went LOW at sim_time=%d (was high for %d cycles)\n", 
                           sim_time, e_high_count);
                    e_high_count = 0;
                    e_low_count = 1;
                }
                last_e_state = dut->dbg_E_o;
            } else {
                // Count how long E stays in each state
                if (dut->dbg_E_o) {
                    e_high_count++;
                } else {
                    e_low_count++;
                }
            }
            
            // Occasionally log clock state
            if (sim_time % 100 == 0) {
                printf("DEBUG CLOCK: sim_time=%d, clk_counter=%d, clk7_16M=%d, clk_en_i=%d, E=%d\n", 
                       sim_time, clk_counter, clk7_16M, dut->clk_en_i, dut->dbg_E_o);
            }
            
            // Enhanced wave data handling with immediate response
            if (dut->wave_rd_o) {
                uint16_t addr = dut->wave_address_o % WAVETABLE_SIZE;
                // Ensure we're sending a non-zero value - not returning 0x80 (silence)
                // Let's use some waveform values that are easier to spot in the output
                // Send a much stronger test signal with non-zero values for alternating samples
                // Avoid 0x00 (halt byte) and 0x80 (silence)
                uint8_t value;
                if (addr % 2 == 0) {
                    value = 0xFF; // Max positive
                } else {
                    value = 0x40; // Negative value, but not 0x00
                }
                printf("Selected wave data 0x%02X for addr 0x%04X at sim_time=%d\n", 
                       value, addr, sim_time);
                
                // Immediately make data available - don't wait for next cycle
                dut->wave_data_i = value;
                dut->wave_data_ready_i = 1;
                
                // Print the address and data with simulation time for tracking
                printf("Time %d: Wave read at 0x%04X, providing 0x%02X immediately\n", 
                       sim_time, addr, value);
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Collect audio samples periodically
            if ((sim_time % SAMPLE_COLLECT_INTERVAL) == 0) {
                // Add debug for audio samples
                printf("DEBUG AUDIO: Time %d, Left=%d, Right=%d\n", 
                       sim_time, dut->left_mix_o, dut->right_mix_o);
                       
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
        if (tfp) tfp->dump(sim_time);
        
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
    // ENHANCED: More direct register writing approach that doesn't depend on E signal
    
    printf("Writing 0x%02X to register 0x%02X...\n", data, addr);
    
    // Set up the register write - hold control signals steady
    dut->cs_n_i = 0;     // Select the chip
    dut->we_n_i = 0;     // Write mode
    dut->addr_i = addr;  // Address to write
    dut->data_i = data;  // Data to write
    
    // First evaluate with these settings
    dut->eval();
    
    // Clock generation variables
    int clk_counter = 0;
    bool clk7_16M = false;
    int wait_cycles = 0;
    const int MAX_WAIT = 100; // Maximum cycles to wait for E transitions
    bool found_e_high = false;
    
    // Hold signals stable for many clock cycles to ensure they are sampled
    const int WRITE_CYCLES = DOC_CLK_CYCLES * 8; // Hold for 8 complete DOC clock cycles
    
    printf("Holding cs_n_i=0, we_n_i=0, addr=0x%02X, data=0x%02X for %d cycles\n", 
           addr, data, WRITE_CYCLES);
    
    // First, wait for E to go low, then high to catch a complete E cycle
    bool found_e_low = false;
    
    // Wait for E to go low first
    while (!found_e_low && wait_cycles < MAX_WAIT) {
        dut->eval();
        
        if (!dut->dbg_E_o) {
            found_e_low = true;
            printf("Found E low at cycle %d, now waiting for E high for clean register write\n", wait_cycles);
        } else {
            // Advance simulation time
            *clk = !(*clk);
            dut->clk_i = *clk;
            
            // Only on rising edge of main clock
            if (*clk) {
                // Maintain clock enable signal
                if (++clk_counter >= DOC_CLK_CYCLES) {
                    clk_counter = 0;
                    clk7_16M = !clk7_16M;
                    dut->clk_en_i = 1;
                } else {
                    dut->clk_en_i = 0;
                }
            }
            
            dut->eval();
            if (tfp) tfp->dump(*sim_time);
            (*sim_time)++;
            wait_cycles++;
        }
    }
    
    // Then wait for E to go high
    while (found_e_low && !found_e_high && wait_cycles < MAX_WAIT*2) {
        dut->eval();
        
        if (dut->dbg_E_o) {
            found_e_high = true;
            printf("Found E high at cycle %d (clk_count=%d), good timing for register write\n", 
                   wait_cycles, dut->dbg_clk_count_o);
        } else {
            // Advance simulation time
            *clk = !(*clk);
            dut->clk_i = *clk;
            
            // Only on rising edge of main clock
            if (*clk) {
                // Maintain clock enable signal
                if (++clk_counter >= DOC_CLK_CYCLES) {
                    clk_counter = 0;
                    clk7_16M = !clk7_16M;
                    dut->clk_en_i = 1;
                } else {
                    dut->clk_en_i = 0;
                }
            }
            
            dut->eval();
            if (tfp) tfp->dump(*sim_time);
            (*sim_time)++;
            wait_cycles++;
        }
    }
    
    if (!found_e_high) {
        printf("WARNING: Could not find proper E high after %d cycles! Will try anyway.\n", wait_cycles);
    } else {
        printf("Successfully synchronized to E high at %d, clk_count=%d\n", 
               *sim_time, dut->dbg_clk_count_o);
    }
    
    // IMPROVED: Make sure E is definitely high before setting control signals
    if (!dut->dbg_E_o) {
        printf("WARNING: E is not high before write! This write may fail.\n");
    }
    
    // Set up the register write - hold address, data and control signals stable
    // IMPORTANT: Set these signals while E is high
    dut->cs_n_i = 0;     // Select the chip
    dut->we_n_i = 0;     // Write mode
    dut->addr_i = addr;  // Address to write
    dut->data_i = data;  // Data to write
    
    printf("Setting cs_n_i=0, we_n_i=0, addr=0x%02X, data=0x%02X with E=%d\n", 
           addr, data, dut->dbg_E_o);
    
    // Hold signals stable for longer to ensure they are sampled correctly
    // We need to keep them stable for at least 4 full E cycles
    const int STABLE_CYCLES = DOC_CLK_CYCLES * 4;
    
    for (int i = 0; i < STABLE_CYCLES; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        // Only on rising edge of main clock
        if (*clk) {
            // Generate the 7.16 MHz clock
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                clk7_16M = !clk7_16M;  // Toggle the 7.16MHz clock
                dut->clk_en_i = 1;
            } else {
                dut->clk_en_i = 0;
            }
        }
        
        dut->eval();
        
        // Debug: Print when E goes high or low during this sequence
        static bool last_e = false;
        if (dut->dbg_E_o != last_e) {
            printf("Time %d: E changed from %d to %d during register write\n", 
                   *sim_time, last_e, dut->dbg_E_o);
            last_e = dut->dbg_E_o;
        }
        
        if (tfp) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Reset control signals and keep them deasserted for the same 
    // number of cycles to ensure clean separation between commands
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    
    printf("Write complete: Resetting cs_n_i=1, we_n_i=1\n");
    
    for (int i = 0; i < 16; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        // Only on rising edge of main clock
        if (*clk) {
            // Generate the 7.16 MHz clock
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                clk7_16M = !clk7_16M;  // Toggle the 7.16MHz clock
                dut->clk_en_i = 1;
            } else {
                dut->clk_en_i = 0;
            }
        }
        
        dut->eval();
        if (tfp) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Verify the write worked by reading back the value (optional but helpful)
    uint8_t read_value;
    read_register(dut, addr, &read_value, sim_time, clk, tfp);
    
    // Print for confirmation
    printf("Wrote 0x%02X to register 0x%02X at time %d, readback value is 0x%02X%s\n", 
           data, addr, *sim_time, read_value,
           (read_value == data) ? " (SUCCESS)" : " (MISMATCH!)");
}

// Helper function to read a register
void read_register(Vdoc5503_harness* dut, uint8_t addr, uint8_t* data, int* sim_time, bool* clk, VerilatedVcdC* tfp) {
    // ENHANCED: More robust E signal synchronization for register reads
    
    // First, check if the module is ready to accept register reads
    if (!dut->ready_o) {
        printf("ERROR: Module is not ready for register reads yet! Aborting read from 0x%02X\n", addr);
        *data = 0; // Set default value
        return;
    }
    
    // Wait for E to be high, so we catch the right timing window
    bool found_e_high = false;
    int wait_cycles = 0;
    const int MAX_WAIT = 100;  // Max cycles to wait for E to be high
    
    // Clock generation variables
    int clk_counter = 0;
    bool clk7_16M = false;
    
    printf("Waiting for E to be high before reading from register 0x%02X...\n", addr);
    
    // First, wait for E to go low, then high to catch a complete E cycle
    bool found_e_low = false;
    
    // Wait for E to go low first
    while (!found_e_low && wait_cycles < MAX_WAIT) {
        dut->eval();
        
        if (!dut->dbg_E_o) {
            found_e_low = true;
            printf("Found E low at cycle %d, now waiting for E high for clean register read\n", wait_cycles);
        } else {
            // Advance simulation time
            *clk = !(*clk);
            dut->clk_i = *clk;
            
            // Only on rising edge of main clock
            if (*clk) {
                // Maintain clock enable signal
                if (++clk_counter >= DOC_CLK_CYCLES) {
                    clk_counter = 0;
                    clk7_16M = !clk7_16M;
                    dut->clk_en_i = 1;
                } else {
                    dut->clk_en_i = 0;
                }
            }
            
            dut->eval();
            if (tfp) tfp->dump(*sim_time);
            (*sim_time)++;
            wait_cycles++;
        }
    }
    
    // Then wait for E to go high
    while (found_e_low && !found_e_high && wait_cycles < MAX_WAIT*2) {
        dut->eval();
        
        if (dut->dbg_E_o) {
            found_e_high = true;
            printf("Found E high at cycle %d (clk_count=%d), good timing for register read\n", 
                   wait_cycles, dut->dbg_clk_count_o);
        } else {
            // Advance simulation time
            *clk = !(*clk);
            dut->clk_i = *clk;
            
            // Only on rising edge of main clock
            if (*clk) {
                // Maintain clock enable signal
                if (++clk_counter >= DOC_CLK_CYCLES) {
                    clk_counter = 0;
                    clk7_16M = !clk7_16M;
                    dut->clk_en_i = 1;
                } else {
                    dut->clk_en_i = 0;
                }
            }
            
            dut->eval();
            if (tfp) tfp->dump(*sim_time);
            (*sim_time)++;
            wait_cycles++;
        }
    }
    
    if (!found_e_high) {
        printf("WARNING: Could not find proper E high after %d cycles! Will try anyway.\n", wait_cycles);
    } else {
        printf("Successfully synchronized to E high at %d, clk_count=%d\n", 
               *sim_time, dut->dbg_clk_count_o);
    }
    
    // IMPROVED: Make sure E is definitely high before setting control signals
    if (!dut->dbg_E_o) {
        printf("WARNING: E is not high before read! This read may fail.\n");
    }
    
    // Set up the register read - hold address and control signals stable
    // IMPORTANT: Set these signals while E is high
    dut->cs_n_i = 0;     // Select the chip
    dut->we_n_i = 1;     // Read mode
    dut->addr_i = addr;  // Address to read
    
    printf("Setting cs_n_i=0, we_n_i=1, addr=0x%02X with E=%d\n", 
           addr, dut->dbg_E_o);
    
    // Hold signals stable for longer to ensure they are sampled correctly
    // We need to keep them stable for at least 4 full E cycles
    const int STABLE_CYCLES = DOC_CLK_CYCLES * 4;
    
    // Track when we've captured data
    bool captured_data = false;
    
    for (int i = 0; i < STABLE_CYCLES; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        // Only on rising edge of main clock
        if (*clk) {
            // Generate the 7.16 MHz clock
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                clk7_16M = !clk7_16M;  // Toggle the 7.16MHz clock
                dut->clk_en_i = 1;
            } else {
                dut->clk_en_i = 0;
            }
        }
        
        dut->eval();
        
        // Track E transitions for debugging and data capture
        static bool last_e = false;
        if (dut->dbg_E_o != last_e) {
            printf("Time %d: E changed from %d to %d during register read\n", 
                   *sim_time, last_e, dut->dbg_E_o);
                   
            // If we see E go high after the initial setup, capture data
            if (dut->dbg_E_o && last_e == false && !captured_data) {
                *data = dut->data_o;
                captured_data = true;
                printf("Captured data (0x%02X) when E went high during read cycle\n", *data);
            }
            
            last_e = dut->dbg_E_o;
        }
        
        if (tfp) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // If we haven't captured data yet, do it now
    if (!captured_data) {
        if (dut->dbg_E_o) {
            *data = dut->data_o;
            printf("Capturing data (0x%02X) at end of read cycle with E high\n", *data);
        } else {
            // Try to find E high in the next few cycles to capture data
            int extra_cycles = 0;
            const int MAX_EXTRA = 16;
            bool found_new_e_high = false;
            
            printf("E is not high at end of read cycle, searching for next E high...\n");
            
            while (!found_new_e_high && extra_cycles < MAX_EXTRA) {
                *clk = !(*clk);
                dut->clk_i = *clk;
                
                // Only on rising edge of main clock
                if (*clk) {
                    // Maintain clock enable signal
                    if (++clk_counter >= DOC_CLK_CYCLES) {
                        clk_counter = 0;
                        clk7_16M = !clk7_16M;
                        dut->clk_en_i = 1;
                    } else {
                        dut->clk_en_i = 0;
                    }
                }
                
                dut->eval();
                
                if (dut->dbg_E_o) {
                    found_new_e_high = true;
                    *data = dut->data_o;
                    printf("Found E high after %d extra cycles, captured data (0x%02X)\n", 
                           extra_cycles, *data);
                }
                
                if (tfp) tfp->dump(*sim_time);
                (*sim_time)++;
                extra_cycles++;
            }
            
            if (!found_new_e_high) {
                printf("WARNING: Could not find E high to capture data, using last value\n");
                *data = dut->data_o;
            }
        }
    }
    
    // Reset control signals and keep them deasserted for clean separation
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    
    printf("Read complete: Resetting cs_n_i=1, we_n_i=1\n");
    
    for (int i = 0; i < 16; i++) {
        *clk = !(*clk);
        dut->clk_i = *clk;
        
        // Only on rising edge of main clock
        if (*clk) {
            // Generate the 7.16 MHz clock
            if (++clk_counter >= DOC_CLK_CYCLES) {
                clk_counter = 0;
                clk7_16M = !clk7_16M;  // Toggle the 7.16MHz clock
                dut->clk_en_i = 1;
            } else {
                dut->clk_en_i = 0;
            }
        }
        
        dut->eval();
        if (tfp) tfp->dump(*sim_time);
        (*sim_time)++;
    }
    
    // Print for confirmation
    printf("Read 0x%02X from register 0x%02X at time %d\n", *data, addr, *sim_time);
}

// Helper function to print debug info
void print_debug_info(Vdoc5503_harness* dut, int sim_time) {
    printf("\n--- Debug Trace at sim_time=%d ---\n", sim_time);
    
    // Clock status and debug signals
    printf("Clock status: E=%d, clk_count=%d, ready=%d\n", 
           dut->dbg_E_o, dut->dbg_clk_count_o, dut->ready_o);
    
    // Audio outputs
    printf("Audio outputs: left=%d, right=%d, mono=%d\n", 
           dut->left_mix_o, dut->right_mix_o, dut->mono_mix_o);
    
    // Oscillator state machine
    int osc_state = dut->dbg_osc_state_o;
    printf("Current oscillator: %d, State: %s (%d)\n",
           dut->dbg_cycle_o, 
           (osc_state >= 0 && osc_state <= 10) ? osc_state_names[osc_state] : "UNKNOWN",
           osc_state);
    
    // Debug values from oscillator
    printf("Wave data: 0x%02X, Volume: 0x%02X, Current output: %d\n",
           dut->dbg_wds_o, dut->dbg_vol_o, dut->dbg_output_o);
    
    // Read key registers to see current state
    uint8_t reg_data;
    
    // Check control register - is the oscillator halted?
    read_register(dut, 0xA0, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Control register (0xA0): 0x%02X (Halted: %s, Mode: %d, Channel: %d)\n", 
           reg_data, (reg_data & 0x01) ? "YES" : "NO", 
           (reg_data >> 1) & 0x03, (reg_data >> 4) & 0x0F);
    
    // Check volume register
    read_register(dut, 0x40, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Volume register (0x40): 0x%02X\n", reg_data);
    
    // Check waveform data sample register - what's the current sample?
    read_register(dut, 0x60, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Waveform Data Sample (0x60): 0x%02X\n", reg_data);
    
    // Check oscillator enable register
    read_register(dut, 0xE1, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Oscillator Enable (0xE1): 0x%02X (# oscillators: %d)\n", 
           reg_data, reg_data >> 1);
    
    // Check frequency registers
    read_register(dut, 0x00, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Frequency Low (0x00): 0x%02X\n", reg_data);
    
    read_register(dut, 0x20, &reg_data, &sim_time, &clk_global, tfp_global);
    printf("Frequency High (0x20): 0x%02X\n", reg_data);
    
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