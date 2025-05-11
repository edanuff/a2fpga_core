#include <stdio.h>
#include <stdlib.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_reg_harness.h"

// Time stamp function required by Verilator
double sc_time_stamp() { return 0; }

// Maximum simulation time
#define MAX_SIM_TIME 50000

// Debug level
const bool dump_vcd = true;

// Main simulation loop
int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the top-level model
    Vdoc5503_reg_harness* doc_harness = new Vdoc5503_reg_harness;
    
    // Create VCD trace file if required
    VerilatedVcdC* tfp = nullptr;
    if (dump_vcd) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        doc_harness->trace(tfp, 99);
        tfp->open("doc5503_reg_test.vcd");
    }

    // Clock generation
    bool clk = false;
    int sim_time = 0;
    
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
    for (int i = 0; i < 20; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_vcd) tfp->dump(sim_time++);
    }
    
    // Release reset
    doc_harness->reset_n_i = 1;
    
    // Wait for device to be ready
    // Make sure we have enough cycles to complete initialization
    printf("Waiting for initialization...\n");

    // Simpler initialization - just run for 300 cycles with proper clk_en_i
    for (int i = 0; i < 300; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;

        // Generate a DOC clk_en_i pulse every 8 cycles
        if (clk) {
            doc_harness->clk_en_i = ((sim_time % 8) == 0);

            // Print progress
            if (i % 100 == 0) {
                printf("Cycle %d\n", i);
            }
        }

        doc_harness->eval();
        if (dump_vcd) tfp->dump(sim_time++);
    }
    printf("Initialization complete\n");

    // Set up oscillator registers
    printf("=== Testing register access ===\n");
    
    // Test registers: write and read back
    struct TestReg {
        uint8_t addr;
        uint8_t value;
        const char* name;
    };
    
    TestReg test_regs[] = {
        {0xE1, 0x3E, "Oscillator Enable Register"},
        {0x00, 0x20, "Frequency Low Register (osc 0)"},
        {0x20, 0x01, "Frequency High Register (osc 0)"},
        {0x40, 0x7F, "Volume Register (osc 0)"},
        {0x80, 0x55, "Waveform Table Pointer Register (osc 0)"},
        {0xA0, 0x10, "Control Register (osc 0)"},
        {0xC0, 0x18, "Resolution Table Size Register (osc 0)"}
    };
    
    for (const auto& reg : test_regs) {
        printf("Testing %s (addr 0x%02X)...\n", reg.name, reg.addr);
        
        // Write register
        uint8_t value = reg.value;
        printf("  Writing value 0x%02X\n", value);
        
        // First set up address and data (CS, WE inactive)
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 1;
                doc_harness->we_n_i = 1;
                doc_harness->addr_i = reg.addr;
                doc_harness->data_i = value;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }

        // Then activate CS (keeping WE inactive)
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 1;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }

        // Then activate WE (write operation happens)
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 0;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }

        // Finally deactivate both CS and WE
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 1;
                doc_harness->we_n_i = 1;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }
        
        // Wait for 8 clk_en_i pulses (64 clock cycles) to ensure proper synchronization
        int wait_cycles = 64; // 8 clk_en_i pulses (every 8 cycles)
        int clk_en_count = 0;
        for (int i = 0; i < wait_cycles; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                bool clk_en = ((sim_time % 8) == 0);
                doc_harness->clk_en_i = clk_en;
                if (clk_en) clk_en_count++;

                // Debug output for every clk_en_i pulse
                if (clk_en) {
                    printf("  Waiting cycle %d, clk_en_i pulse %d\n", i, clk_en_count);
                }
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }
        printf("  Waited for %d clk_en_i pulses\n", clk_en_count);
        
        // Read register back - First set address
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 1;
                doc_harness->we_n_i = 1;
                doc_harness->addr_i = reg.addr;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }

        // Then assert CS for read (WE remains deasserted)
        for (int i = 0; i < 8; i++) {  // Increased to 8 cycles for more reliable read
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 0;
                doc_harness->we_n_i = 1;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);

                // Let at least one clk_en_i pulse happen while CS is asserted
                if (((sim_time % 8) == 0)) {
                    printf("  Read cycle with clk_en_i pulse\n");
                }
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }

        // Check the value after enough cycles for data to be valid
        printf("  Read back value 0x%02X\n", doc_harness->data_o);
        if (doc_harness->data_o != value) {
            printf("  ERROR: Register verification failed! Address: 0x%02X, Expected: 0x%02X, Got: 0x%02X\n",
                   reg.addr, value, doc_harness->data_o);
        } else {
            printf("  SUCCESS: Register verification passed!\n");
        }

        // Finally deassert CS
        for (int i = 0; i < 4; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;

            if (clk) {
                doc_harness->cs_n_i = 1;
                doc_harness->we_n_i = 1;
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }

            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
        }
        
        // Extra delay between register tests
        for (int i = 0; i < 16; i++) {
            clk = !clk;
            doc_harness->clk_i = clk;
            
            if (clk) {
                doc_harness->clk_en_i = ((sim_time % 8) == 0);
            }
            
            doc_harness->eval();
            if (dump_vcd) tfp->dump(sim_time++);
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