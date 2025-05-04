#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <fstream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Simple sine wave test for DOC5503
// This test focuses on showing every state transition and output generation

// Required for Verilator
double sc_time_stamp() { return 0; }

// Convert enum value to string for nicer output
const char* osc_state_to_str(int state) {
    switch(state) {
        case 0: return "OSC_IDLE";
        case 1: return "OSC_START";
        case 2: return "OSC_LOAD_PARTNER_CONTROL";
        case 3: return "OSC_ACC";
        case 4: return "OSC_ADDR";
        case 5: return "OSC_READ_DATA";
        case 6: return "OSC_AM_SYNC_EVEN";
        case 7: return "OSC_HALT";
        case 8: return "OSC_HALT_PARTNER";
        case 9: return "OSC_OUT";
        case 10: return "OSC_MIX";
        default: return "UNKNOWN";
    }
}

const char* cycle_state_to_str(int state) {
    switch(state) {
        case 0: return "CYCLE_RESET";
        case 1: return "CYCLE_OSC";
        case 2: return "CYCLE_REFRESH_0";
        case 3: return "CYCLE_REFRESH_1";
        default: return "UNKNOWN";
    }
}

int main(int argc, char** argv) {
    printf("=== DOC5503 Debug Sine Wave Test ===\n");
    printf("This test creates a simple sine wave and shows all state transitions\n\n");
    
    // Create a log file
    std::ofstream logFile("doc5503_debug_sine.log");
    if (!logFile.is_open()) {
        fprintf(stderr, "Error: Unable to open log file for writing\n");
        return 1;
    }
    
    logFile << "=== DOC5503 Debug Sine Wave Test Log ===\n";
    logFile << "This log file contains detailed state transition and output information\n\n";
    
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the model
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
    // Enable VCD tracing for GTKWave
    bool trace_enabled = true;
    VerilatedVcdC* tfp = nullptr;
    
    if (trace_enabled) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);  // Trace 99 levels of hierarchy
        
        // Open VCD file in build directory for GTKWave
        tfp->open("doc5503_debug_sine.vcd");
        printf("GTKWave file: doc5503_debug_sine.vcd\n\n");
    }
    
    // Generate a sine wave lookup table
    const int wave_size = 256;
    uint8_t wave_table[wave_size];
    
    printf("Generating sine wave lookup table...\n");
    for (int i = 0; i < wave_size; i++) {
        double angle = (double)i / wave_size * 2.0 * M_PI;
        double value = sin(angle);
        // Scale to range 0x01-0xFF (avoid zero which can cause halting)
        int sample = (int)((value + 1.0) * 127.0) + 1;
        if (sample < 1) sample = 1;
        if (sample > 255) sample = 255;
        wave_table[i] = sample;
    }
    
    // Print first 16 values for debugging
    printf("Wave table sample values: ");
    logFile << "Wave table sample values: ";
    for (int i = 0; i < 16; i++) {
        printf("%02X ", wave_table[i]);
        logFile << std::hex << std::uppercase << (int)wave_table[i] << " ";
    }
    printf("...\n\n");
    logFile << "...\n\n";
    
    // Clock generation
    bool clk = false;
    int sim_time = 0;
    
    // Reset sequence
    dut->reset_n_i = 0;
    dut->clk_i = clk;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Reset for 10 cycles
    printf("Resetting for 10 cycles...\n");
    logFile << "Resetting for 10 cycles...\n";
    for (int i = 0; i < 20; i++) {
        clk = !clk;
        dut->clk_i = clk;
        dut->eval();
        if (trace_enabled) tfp->dump(sim_time);
        sim_time++;
    }
    
    // Release reset
    dut->reset_n_i = 1;
    logFile << "Reset released at cycle " << sim_time << "\n";
    
    // Helper function for register writes
    auto write_reg = [&](uint8_t addr, uint8_t data) {
        printf("Writing 0x%02X to register 0x%02X\n", data, addr);
        logFile << "REGISTER_WRITE: Cycle=" << sim_time << ", Addr=0x" 
                << std::hex << std::uppercase << (int)addr 
                << ", Data=0x" << (int)data << std::dec << "\n";
        
        // Apply write signals
        dut->cs_n_i = 0;
        dut->we_n_i = 0;
        dut->addr_i = addr;
        dut->data_i = data;
        
        // Clock for 2 cycles
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            dut->clk_i = clk;
            dut->eval();
            if (trace_enabled) tfp->dump(sim_time);
            sim_time++;
        }
        
        // Release signals
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        
        // Wait a few cycles
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            dut->clk_i = clk;
            dut->eval();
            if (trace_enabled) tfp->dump(sim_time);
            sim_time++;
        }
    };
    
    // Set up oscillator 0
    printf("\nSetting up oscillator 0...\n");
    logFile << "\nSetting up oscillator 0...\n";
    write_reg(0x00, 0x20);    // FL - Low frequency (0x20)
    write_reg(0x20, 0x01);    // FH - Moderate frequency (0x01)
    write_reg(0x40, 0xFF);    // Volume - Maximum (0xFF)
    write_reg(0x80, 0x00);    // WTP - Wavetable at page 0
    write_reg(0xA0, 0x00);    // Control - Free running, channel 0, not halted
    write_reg(0xC0, 0x00);    // RTS - 256 byte table, standard resolution
    
    // Enable one oscillator
    write_reg(0xE1, 0x02);    // Enable 1 oscillator (value 2 = 1 osc)
    
    // Run simulation
    int max_cycles = 3000; // Increased to see more state transitions
    printf("\nRunning simulation for %d cycles...\n", max_cycles);
    printf("Showing state transitions and wave reads/outputs...\n\n");
    logFile << "\nRunning simulation for " << max_cycles << " cycles...\n";
    logFile << "Detailed state transitions and wave reads/outputs:\n\n";
    
    int sample_counter = 0;
    int wave_read_counter = 0;
    int non_zero_outputs = 0;
    
    // Save last values to detect changes
    int last_osc_state = -1;
    int last_osc_num = -1;
    
    // Track all internal signal values for detailed logging
    uint8_t last_wds = 0;
    uint8_t last_vol = 0;
    
    for (int i = 0; i < max_cycles; i++) {
        clk = !clk;
        dut->clk_i = clk;
        
        // On rising edge, set clk_en_i pulse every 8 cycles
        if (clk) {
            // Generate DOC clock enable at 7.15909MHz/8
            dut->clk_en_i = ((sim_time % 8) == 0);
            
            // Provide wave data when requested
            if (dut->wave_rd_o) {
                wave_read_counter++;
                
                // Get wave address (masked to table size)
                uint16_t addr = dut->wave_address_o & 0xFF;
                
                // Get wave data from our sine table
                uint8_t value = wave_table[addr % wave_size];
                
                dut->wave_data_i = value;
                dut->wave_data_ready_i = 1;
                
                // Console output (brief)
                printf("WAVE_READ: Cycle=%d, Addr=0x%04X, Value=0x%02X\n", 
                       sim_time, dut->wave_address_o, value);
                
                // Log file output (detailed)
                logFile << "WAVE_READ: Cycle=" << sim_time 
                        << ", Addr=0x" << std::hex << dut->wave_address_o 
                        << ", Value=0x" << (int)value  
                        << ", Osc=" << std::dec << (int)dut->dbg_cycle_o
                        << ", State=" << osc_state_to_str(dut->dbg_osc_state_o)
                        << "\n";
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Output debug info on state changes
            int curr_osc_state = dut->dbg_osc_state_o;
            int curr_osc_num = dut->dbg_cycle_o;
            
            // Check for oscillator state change
            if (curr_osc_state != last_osc_state || curr_osc_num != last_osc_num) {
                // Log to console (less detailed)
                printf("OSC_STATE: Cycle=%d, Osc=%d, State=%s\n", 
                       sim_time, curr_osc_num, osc_state_to_str(curr_osc_state));
                
                // Log to file (more detailed)
                logFile << "OSC_STATE: Cycle=" << sim_time 
                        << ", Osc=" << curr_osc_num 
                        << ", State=" << osc_state_to_str(curr_osc_state) 
                        << ", WDS=0x" << std::hex << (int)dut->dbg_wds_o 
                        << ", VOL=0x" << (int)dut->dbg_vol_o << std::dec 
                        << ", CLK_EN=" << dut->clk_en_i
                        << "\n";
                
                last_osc_state = curr_osc_state;
                last_osc_num = curr_osc_num;
            }
            
            // Log output values when they change
            if ((sim_time % 32) == 0) { // Changed to every 32 cycles for less verbosity
                // Get current output values
                printf("OUTPUT: Cycle=%d, Left=%d, Right=%d, Mono=%d\n", 
                       sim_time, dut->left_mix_o, dut->right_mix_o, dut->mono_mix_o);
                
                // Print internal oscillator data 
                printf("  Osc=%d, State=%s, Vol=0x%02X, WDS=0x%02X\n",
                       dut->dbg_cycle_o, osc_state_to_str(dut->dbg_osc_state_o),
                       dut->dbg_vol_o, dut->dbg_wds_o);
                
                // Log to file (even more detailed)
                logFile << "OUTPUT: Cycle=" << sim_time
                        << ", Left=" << dut->left_mix_o 
                        << ", Right=" << dut->right_mix_o
                        << ", Mono=" << dut->mono_mix_o
                        << ", Osc=" << (int)dut->dbg_cycle_o
                        << ", State=" << osc_state_to_str(dut->dbg_osc_state_o)
                        << ", Vol=0x" << std::hex << (int)dut->dbg_vol_o
                        << ", WDS=0x" << (int)dut->dbg_wds_o << std::dec
                        << ", CLK_EN=" << dut->clk_en_i
                        << "\n";
                
                // Track non-zero outputs
                if (dut->left_mix_o != 0 || dut->right_mix_o != 0 || dut->mono_mix_o != 0) {
                    non_zero_outputs++;
                }
                
                // Sample counter - collect every 32nd cycle
                sample_counter++;
            }
        }
        
        // Evaluate model
        dut->eval();
        
        // Write to VCD
        if (trace_enabled) tfp->dump(sim_time);
        
        sim_time++;
    }
    
    // Summary
    printf("\n=== Summary ===\n");
    printf("Simulation time: %d cycles\n", sim_time);
    printf("Wave reads: %d\n", wave_read_counter);
    printf("Output samples: %d\n", sample_counter);
    printf("Non-zero outputs: %d\n", non_zero_outputs);
    
    // Log summary to file
    logFile << "\n=== Summary ===\n";
    logFile << "Simulation time: " << sim_time << " cycles\n";
    logFile << "Wave reads: " << wave_read_counter << "\n";
    logFile << "Output samples: " << sample_counter << "\n";
    logFile << "Non-zero outputs: " << non_zero_outputs << "\n";
    
    // Check if we had any non-zero outputs
    if (non_zero_outputs > 0) {
        printf("SUCCESS: Audio output was detected (%d non-zero samples)\n", non_zero_outputs);
        logFile << "SUCCESS: Audio output was detected\n";
    } else {
        printf("FAILURE: No audio output detected\n");
        logFile << "FAILURE: No audio output detected\n";
        
        // Additional debug info in log
        logFile << "\nProblem Analysis:\n";
        logFile << "1. Check state machine transitions - not reaching OSC_MIX state\n";
        logFile << "2. Verify volume & waveform data are correctly processed\n";
        logFile << "3. Examine oscillator control handling\n";
        logFile << "4. Consider potential timing issues in the output mixing logic\n";
    }
    
    // Cleanup
    if (trace_enabled) {
        tfp->close();
        delete tfp;
    }
    
    // Close log file
    logFile << "\nTest completed.\n";
    logFile.close();
    
    // Print log file location
    printf("\nDetailed log written to: doc5503_debug_sine.log\n");
    printf("VCD file for GTKWave: doc5503_debug_sine.vcd\n");
    
    delete dut;
    
    return 0;
}