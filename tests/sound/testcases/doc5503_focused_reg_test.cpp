#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // Create an instance of our model
    Vdoc5503_harness* dut = new Vdoc5503_harness;
    
    // Always enable trace dump for this test
    VerilatedVcdC* tfp = nullptr;
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    dut->trace(tfp, 99); // Trace 99 levels of hierarchy
    tfp->open("doc5503_focused_reg_test.vcd");
    
    printf("=== DOC5503 Focused Register Test ===\n");
    printf("Testing basic register access with very detailed debug output\n\n");
    
    // Initialize the DOC5503
    dut->reset_n_i = 0;
    dut->clk_i = 0;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Reset sequence
    printf("Resetting DOC5503...\n");
    int cycle = 0;
    
    // Reset with detailed clk_en_i timing
    printf("Reset phase:\n");
    for (int i = 0; i < 100; i++) {
        // Toggle clock
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
            printf("  Cycle %3d: clk=%d clk_en=1 reset=%d\n", 
                   i, dut->clk_i, !dut->reset_n_i);
        } else {
            dut->clk_en_i = 0;
            
            // Print only for state changes
            if (i < 10 || (i % 8 == 0) || (i % 8 == 1)) {
                printf("  Cycle %3d: clk=%d clk_en=0 reset=%d\n", 
                       i, dut->clk_i, !dut->reset_n_i);
            }
        }
        
        // Release reset halfway through
        if (i == 50) {
            dut->reset_n_i = 1;
            printf("  Releasing reset at cycle %d\n", i);
        }
        
        dut->eval();
        tfp->dump(cycle++);
    }
    
    // Wait for ready signal
    printf("\nWaiting for DOC5503 to be ready...\n");
    int ready_wait = 0;
    
    while (!dut->ready_o && ready_wait < 1000) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (ready_wait % 8 == 0)) {
            dut->clk_en_i = 1;
            printf("  Cycle %3d: clk=%d clk_en=1 ready=%d\n", 
                   ready_wait + 100, dut->clk_i, dut->ready_o);
        } else {
            dut->clk_en_i = 0;
        }
        
        dut->eval();
        tfp->dump(cycle++);
        ready_wait++;
        
        // Print ready signal when it changes
        if (dut->ready_o) {
            printf("  DOC5503 ready_o went HIGH at cycle %d\n", ready_wait + 100);
        }
    }
    
    if (!dut->ready_o) {
        printf("ERROR: DOC5503.ready_o did not go high within timeout period!\n");
        return 1;
    }
    
    printf("DOC5503 is ready.\n\n");
    
    // Test register write with careful clk_en_i pulse timing and full debug output
    printf("Testing register write to Oscillator Enable Register (0xE1)...\n");
    
    // Define a debug print function
    auto print_state = [&](const char* step, int current_cycle) {
        printf("  [%3d] %-25s: cs=%d we=%d addr=0x%02X data=0x%02X | clk_en=%d dbg_host_acc=%d ready=%d data_out=0x%02X\n",
               current_cycle, step, 
               dut->cs_n_i, dut->we_n_i, dut->addr_i, dut->data_i,
               dut->clk_en_i, 0, dut->ready_o, dut->data_o);
    };
    
    // Perform the write with detailed debug and very precise control of signals
    
    // Setup phase - prepare address and data with CS=1, WE=1
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0xE1;
    dut->data_i = 0x02;  // Enable 1 oscillator
    
    // Run multiple cycles with clk_en_i pulses every 8 clock transitions
    int current_cycle = cycle;
    printf("\nSetup phase (address/data setup):\n");
    for (int i = 0; i < 16; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("Setup", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // CS assertion phase - Assert CS with WE still high
    printf("\nCS assertion phase (CS goes low, WE still high):\n");
    dut->cs_n_i = 0;
    dut->we_n_i = 1;
    
    for (int i = 0; i < 16; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("CS Assertion", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // WE assertion phase - Assert WE while CS is low
    printf("\nWE assertion phase (WE goes low while CS is low):\n");
    dut->cs_n_i = 0;
    dut->we_n_i = 0;
    
    for (int i = 0; i < 24; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate continuous clk_en_i pulses on rising clock edges during write
        if (dut->clk_i) {
            dut->clk_en_i = 1;
            print_state("WE Assertion (clk_en=1)", current_cycle - cycle + i);
        } else {
            dut->clk_en_i = 0;
            print_state("WE Assertion (clk_en=0)", current_cycle - cycle + i);
        }
        
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // WE de-assertion phase - De-assert WE while CS is still low
    printf("\nWE de-assertion phase (WE goes high, CS still low):\n");
    dut->cs_n_i = 0;
    dut->we_n_i = 1;
    
    for (int i = 0; i < 16; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("WE De-assertion", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // CS de-assertion phase - De-assert CS
    printf("\nCS de-assertion phase (CS goes high):\n");
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    
    for (int i = 0; i < 16; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("CS De-assertion", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // Wait phase - Allow time for the write to propagate
    printf("\nWait phase (allowing time for write to complete):\n");
    for (int i = 0; i < 32; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("Wait After Write", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // Verify the write by reading back the register
    printf("\nVerification phase (reading back register):\n");
    
    // 1. Setup address for read with CS=1, WE=1
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0xE1;
    
    for (int i = 0; i < 16; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("Read Setup", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // 2. Assert CS with WE still high for read
    dut->cs_n_i = 0;
    dut->we_n_i = 1;
    
    for (int i = 0; i < 32; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate continuous clk_en_i pulses on rising clock edges during read
        if (dut->clk_i) {
            dut->clk_en_i = 1;
            print_state("Read CS Assertion (clk_en=1)", current_cycle - cycle + i);
        } else {
            dut->clk_en_i = 0;
            print_state("Read CS Assertion (clk_en=0)", current_cycle - cycle + i);
        }
        
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // 3. De-assert CS
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    
    for (int i = 0; i < 16; i++) {
        dut->clk_i = !dut->clk_i;
        
        // Generate clk_en_i pulse every 8 clock transitions (4 clock cycles)
        if (dut->clk_i && (i % 8 == 0)) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        print_state("Read CS De-assertion", current_cycle - cycle + i);
        dut->eval();
        tfp->dump(current_cycle++);
    }
    
    // Final result
    printf("\nRegister verification: expected=0x02, actual=0x%02X %s\n", 
           dut->data_o, (dut->data_o == 0x02) ? "[SUCCESS]" : "[FAILED]");
    
    // Clean up
    tfp->close();
    delete tfp;
    delete dut;
    
    return 0;
}