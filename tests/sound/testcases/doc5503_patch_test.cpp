#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_patch_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // Create an instance of our model
    Vdoc5503_patch_harness* dut = new Vdoc5503_patch_harness;
    
    // Always enable trace dump for this test
    VerilatedVcdC* tfp = nullptr;
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    dut->trace(tfp, 99); // Trace 99 levels of hierarchy
    tfp->open("doc5503_patch_test.vcd");
    
    printf("=== DOC5503 Patch Test ===\n");
    printf("Testing register access with the patched DOC5503 module\n\n");
    
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
    printf("Resetting DOC5503 patch...\n");
    int cycle = 0;
    
    for (int i = 0; i < 20; i++) {
        dut->clk_i = !dut->clk_i;
        
        if (dut->clk_i && i % 8 == 0) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        dut->eval();
        tfp->dump(cycle++);
    }
    
    dut->reset_n_i = 1;
    
    for (int i = 0; i < 20; i++) {
        dut->clk_i = !dut->clk_i;
        
        if (dut->clk_i && i % 8 == 0) {
            dut->clk_en_i = 1;
        } else {
            dut->clk_en_i = 0;
        }
        
        dut->eval();
        tfp->dump(cycle++);
    }
    
    printf("DOC5503 patch ready.\n\n");
    
    // Test writing to the oscillator enable register (0xE1)
    printf("Test 1: Writing oscillator enable register (0xE1)...\n");
    
    // Register write sequence with osc_en debugging
    auto write_reg = [&](uint8_t addr, uint8_t data) {
        printf("  Writing register 0x%02X = 0x%02X\n", addr, data);
        
        // First set up address and data with CS=1, WE=1
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = addr;
        dut->data_i = data;
        
        for (int i = 0; i < 4; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            dut->eval();
            tfp->dump(cycle++);
        }
        
        // Assert CS with WE still high
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        
        for (int i = 0; i < 4; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            dut->eval();
            tfp->dump(cycle++);
        }
        
        // Assert WE (WE goes low)
        dut->cs_n_i = 0;
        dut->we_n_i = 0;
        
        for (int i = 0; i < 8; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            printf("    Cycle %3d: CS=%d WE=%d clk_en=%d osc_en=0x%02X\n", 
                  cycle, dut->cs_n_i, dut->we_n_i, dut->clk_en_i, dut->dbg_osc_en_o);
            dut->eval();
            tfp->dump(cycle++);
        }
        
        // De-assert WE
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        
        for (int i = 0; i < 4; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            dut->eval();
            tfp->dump(cycle++);
        }
        
        // De-assert CS
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        
        for (int i = 0; i < 8; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            printf("    Cycle %3d: CS=%d WE=%d clk_en=%d osc_en=0x%02X\n", 
                  cycle, dut->cs_n_i, dut->we_n_i, dut->clk_en_i, dut->dbg_osc_en_o);
            dut->eval();
            tfp->dump(cycle++);
        }
        
        printf("  Oscillator enable register after write: 0x%02X\n\n", dut->dbg_osc_en_o);
    };
    
    // Read register sequence
    auto read_reg = [&](uint8_t addr) -> uint8_t {
        printf("  Reading register 0x%02X\n", addr);
        
        // First set up address with CS=1, WE=1
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = addr;
        
        for (int i = 0; i < 4; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            dut->eval();
            tfp->dump(cycle++);
        }
        
        // Assert CS with WE high for read
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        
        for (int i = 0; i < 16; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            printf("    Cycle %3d: CS=%d WE=%d clk_en=%d data_out=0x%02X\n", 
                  cycle, dut->cs_n_i, dut->we_n_i, dut->clk_en_i, dut->data_o);
            dut->eval();
            tfp->dump(cycle++);
        }
        
        // De-assert CS
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        
        for (int i = 0; i < 4; i++) {
            dut->clk_i = !dut->clk_i;
            dut->clk_en_i = dut->clk_i ? 1 : 0;  // clk_en_i on every rising edge for testing
            dut->eval();
            tfp->dump(cycle++);
        }
        
        printf("  Read value: 0x%02X\n\n", dut->data_o);
        return dut->data_o;
    };
    
    // Initial read of osc_en register should be 0x00
    uint8_t initial_value = read_reg(0xE1);
    
    // Write 0x02 to osc_en register (enable 1 oscillator)
    write_reg(0xE1, 0x02);
    
    // Read back the value to verify
    uint8_t read_value = read_reg(0xE1);
    
    // Write 0x04 to osc_en register (enable 2 oscillators)
    write_reg(0xE1, 0x04);
    
    // Read back the value to verify
    uint8_t read_value2 = read_reg(0xE1);
    
    // Final result
    printf("Test Results:\n");
    printf("  Initial osc_en value: 0x%02X\n", initial_value);
    printf("  After writing 0x02: 0x%02X %s\n", read_value, 
           (read_value == 0x02) ? "[SUCCESS]" : "[FAILED]");
    printf("  After writing 0x04: 0x%02X %s\n", read_value2, 
           (read_value2 == 0x04) ? "[SUCCESS]" : "[FAILED]");
    
    // Clean up
    tfp->close();
    delete tfp;
    delete dut;
    
    return 0;
}