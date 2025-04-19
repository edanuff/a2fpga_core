#include <stdio.h>
#include <stdlib.h>
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
const DebugLevel verbosity = INFO;

// Dump VCD file
const bool dump_vcd = true;

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
    
    // Setup oscillator 0 registers with test values
    // We'll setup a simple oscillator with basic parameters
    
    // Simulation main loop
    int sim_time = 10; // Start after reset cycles
    bool doc_clk_en = false;
    
    for (; sim_time < MAX_SIM_TIME; sim_time++) {
        // Toggle clock
        clk = !clk;
        doc_harness->clk_i = clk;
        
        // On rising edge, potentially set clk_en_i
        if (clk) {
            // Generate a DOC clk_en_i pulse every 8 cycles to simulate 7.15909MHz/8
            doc_clk_en = ((sim_time % 8) == 0);
            doc_harness->clk_en_i = doc_clk_en;
            
            // Every so often, write test data to oscillator registers
            if (sim_time == 20) {
                if (verbosity >= INFO) printf("Writing FL=0x20 to oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0x00; // Frequency Low for oscillator 0
                doc_harness->data_i = 0x20; // Test value
            }
            else if (sim_time == 30) {
                if (verbosity >= INFO) printf("Writing FH=0x01 to oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0x20; // Frequency High for oscillator 0
                doc_harness->data_i = 0x01; // Test value
            }
            else if (sim_time == 40) {
                if (verbosity >= INFO) printf("Writing VOL=0x7F to oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0x40; // Volume for oscillator 0
                doc_harness->data_i = 0x7F; // Medium volume
            }
            else if (sim_time == 50) {
                if (verbosity >= INFO) printf("Writing WTP=0x00 to oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0x80; // Waveform table pointer for oscillator 0
                doc_harness->data_i = 0x00; // Start at page 0
            }
            else if (sim_time == 60) {
                if (verbosity >= INFO) printf("Writing CTL=0x10 to oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0xA0; // Control for oscillator 0
                doc_harness->data_i = 0x10; // Free running mode, no halt, channel 1 (left)
            }
            else if (sim_time == 70) {
                if (verbosity >= INFO) printf("Writing RTS=0x18 to oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0xC0; // Resolution/Table Size for oscillator 0
                doc_harness->data_i = 0x18; // Res=0, TableSize=3 (8k)
            }
            else if (sim_time == 80) {
                if (verbosity >= INFO) printf("Enabling oscillator 0\n");
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->addr_i = 0xE1; // Oscillator enable register
                doc_harness->data_i = 0x02; // Enable oscillator 0
            }
            else {
                doc_harness->cs_n_i = 1;
                doc_harness->we_n_i = 1;
            }
            
            // Provide wave data when requested - using simple test pattern
            if (doc_harness->wave_rd_o) {
                if (verbosity >= DEBUG) printf("Wave read request at address 0x%04x\n", doc_harness->wave_address_o);
                
                // Use simple test pattern for debugging - alternating values
                // 0xC0 is a positive value (0x80 is zero)
                doc_harness->wave_data_i = 0xC0;
                doc_harness->wave_data_ready_i = 1;
                
                if (verbosity >= DEBUG && (sim_time % 100 == 0)) {
                    printf("Wave data at 0x%04x = 0x%02x\n", doc_harness->wave_address_o, doc_harness->wave_data_i);
                }
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write to VCD
        if (dump_vcd) tfp->dump(sim_time);
        
        // Monitor outputs
        if (clk && (sim_time % 100 == 0)) {
            if (verbosity >= INFO) {
                printf("Time: %d, left_mix: %d, right_mix: %d\n", 
                       sim_time, doc_harness->mono_mix_o, doc_harness->left_mix_o);
            }
        }
    }
    
    // Cleanup
    if (dump_vcd) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    
    return 0;
}