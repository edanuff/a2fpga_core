#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required time stamp function for Verilator
double sc_time_stamp() { return 0; }

// Helper function to write a register to the DOC
void write_register(Vdoc5503_harness* doc_harness, uint8_t addr, uint8_t data, int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    // Write register on rising edge
    clk = true;
    doc_harness->clk_i = clk;
    doc_harness->cs_n_i = 0;
    doc_harness->we_n_i = 0;
    doc_harness->addr_i = addr;
    doc_harness->data_i = data;
    doc_harness->eval();
    if (dump_trace) tfp->dump(sim_time++);
    
    // Change clock to falling edge
    clk = false;
    doc_harness->clk_i = clk;
    doc_harness->eval();
    if (dump_trace) tfp->dump(sim_time++);
    
    // Reset control signals
    doc_harness->cs_n_i = 1;
    doc_harness->we_n_i = 1;
}

// Helper function to read a register from the DOC
uint8_t read_register(Vdoc5503_harness* doc_harness, uint8_t addr, int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    // Set up read on rising edge
    clk = true;
    doc_harness->clk_i = clk;
    doc_harness->cs_n_i = 0;
    doc_harness->we_n_i = 1;
    doc_harness->addr_i = addr;
    doc_harness->eval();
    if (dump_trace) tfp->dump(sim_time++);
    
    // Read data on falling edge
    clk = false;
    doc_harness->clk_i = clk;
    doc_harness->eval();
    uint8_t data = doc_harness->data_o;
    if (dump_trace) tfp->dump(sim_time++);
    
    // Reset control signals
    doc_harness->cs_n_i = 1;
    doc_harness->we_n_i = 1;
    
    return data;
}

// Configure a DOC oscillator
void setup_oscillator(Vdoc5503_harness* doc_harness, uint8_t osc_num, uint16_t freq, uint8_t vol, uint8_t control, 
                     int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    const uint8_t addr_freq_low = osc_num;                  // Frequency Low (0x00-0x1F)
    const uint8_t addr_freq_high = 0x20 + osc_num;          // Frequency High (0x20-0x3F)
    const uint8_t addr_volume = 0x40 + osc_num;             // Volume (0x40-0x5F)
    const uint8_t addr_wave_ptr = 0x80 + osc_num;           // Waveform table pointer (0x80-0x9F)
    const uint8_t addr_control = 0xA0 + osc_num;            // Control (0xA0-0xBF)
    const uint8_t addr_res_table = 0xC0 + osc_num;          // Resolution/Table Size (0xC0-0xDF)
    
    printf("Setting up oscillator %d...\n", osc_num);
    
    // Frequency Low
    write_register(doc_harness, addr_freq_low, freq & 0xFF, sim_time, clk, tfp, dump_trace);
    
    // Frequency High
    write_register(doc_harness, addr_freq_high, (freq >> 8) & 0xFF, sim_time, clk, tfp, dump_trace);
    
    // Volume
    write_register(doc_harness, addr_volume, vol, sim_time, clk, tfp, dump_trace);
    
    // Wave Table Pointer (start at page 0)
    write_register(doc_harness, addr_wave_ptr, 0x00, sim_time, clk, tfp, dump_trace);
    
    // Control register 
    // - Bits 7-4: Channel assignment (0x10 = channel 1)
    // - Bit 3: Interrupt enable
    // - Bits 2-1: Mode (0=Free Run, 1=One Shot, 2=Sync/AM, 3=Swap)
    // - Bit 0: Halt bit (0=run, 1=halt)
    write_register(doc_harness, addr_control, control, sim_time, clk, tfp, dump_trace);
    
    // Resolution/Table Size 
    // - Bits 5-3: Table size (0-7 = 256,512,1024,2048,4096,8192,16384,32768 bytes)
    // - Bits 2-0: Resolution (0-7 = accumulator bits 1-16 through 8-23)
    write_register(doc_harness, addr_res_table, 0x00, sim_time, clk, tfp, dump_trace);
}

// Reset the DOC
void reset_doc(Vdoc5503_harness* doc_harness, int& sim_time, bool& clk, VerilatedVcdC* tfp, bool dump_trace) {
    doc_harness->reset_n_i = 0;
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_trace) tfp->dump(sim_time++);
    }
    
    doc_harness->reset_n_i = 1;
}

// Generate a test wavetable with a zero at the end to test oscillator halting
void generate_test_wavetable_with_zero(uint8_t* wavetable, size_t length) {
    for (size_t i = 0; i < length - 8; i++) {
        // Generate rising sawtooth wave 
        wavetable[i] = (uint8_t)(((i % 64) * 2) + 128);
    }
    
    // Put zeros at the end to halt the oscillator
    for (size_t i = length - 8; i < length; i++) {
        wavetable[i] = 0;
    }
}

// Main function to test the DOC5503 interrupt functionality
int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the DOC5503 model
    Vdoc5503_harness* doc_harness = new Vdoc5503_harness;
    
    // Set up tracing
    VerilatedVcdC* tfp = nullptr;
    bool dump_trace = true;
    
    if (dump_trace) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        doc_harness->trace(tfp, 99);
        tfp->open("doc5503_interrupt_test.vcd");
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
    
    // Reset for initial state
    int sim_time = 0;
    reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
    
    // Generate test wavetable with zeros at end
    const size_t WAVETABLE_SIZE = 256;
    uint8_t test_wavetable[WAVETABLE_SIZE];
    generate_test_wavetable_with_zero(test_wavetable, WAVETABLE_SIZE);
    
    printf("\n===== TESTING DOC5503 INTERRUPT FUNCTIONALITY =====\n");
    
    // === TEST 1: Verify interrupt generation for one-shot mode ===
    printf("\n--- TEST 1: Interrupt on oscillator halt in one-shot mode ---\n");
    
    // Enable one oscillator
    write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
    
    // Setup oscillator 0 in one-shot mode with interrupt enabled
    // 0x1A = 0001 1010
    //   Bits 7-4: 0001 = Channel 1
    //   Bit 3: 1 = Interrupt enabled
    //   Bits 2-1: 01 = One-shot mode
    //   Bit 0: 0 = Not halted
    uint8_t osc0_control = 0x1A;  // Channel 1, Interrupt enabled, One-shot mode
    setup_oscillator(doc_harness, 0, 0x0200, 0xFF, osc0_control, sim_time, clk, tfp, dump_trace);
    
    // Clear interrupt register before starting
    write_register(doc_harness, 0xE0, 0x00, sim_time, clk, tfp, dump_trace);
    
    // Track whether interrupt fired
    bool interrupt_fired = false;
    int interrupt_cycle = 0;
    int cycles_since_zero = 0;
    bool zero_encountered = false;
    
    printf("Running simulation waiting for interrupt...\n");
    
    // Run for enough cycles to execute one-shot
    for (int cycle = 0; cycle < 20000; cycle++) {
        // Toggle clock
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            // Generate DOC clk_en_i pulse every 8 cycles
            bool doc_clk_en = ((cycle % 8) == 0);
            doc_harness->clk_en_i = doc_clk_en;
            
            // When the DOC requests wave data, provide it from our wavetable
            if (doc_harness->wave_rd_o) {
                uint16_t wave_addr = doc_harness->wave_address_o & 0xFF;
                doc_harness->wave_data_i = test_wavetable[wave_addr];
                doc_harness->wave_data_ready_i = 1;
                
                // Debug: track when we hit the zeros at the end
                if (doc_harness->wave_data_i == 0 && !zero_encountered) {
                    zero_encountered = true;
                    printf("Cycle %d: Zero value encountered in wavetable at address 0x%04x\n",
                        cycle, wave_addr);
                }
                
                if (zero_encountered) {
                    cycles_since_zero++;
                }
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Check if interrupt line goes low (active)
            if (!doc_harness->irq_n_o && !interrupt_fired) {
                interrupt_fired = true;
                interrupt_cycle = cycle;
                printf("Cycle %d: INTERRUPT DETECTED (irq_n_o = %d)\n", 
                    cycle, doc_harness->irq_n_o);
                
                // Read the interrupt register to see which oscillator triggered it
                uint8_t interrupt_reg = read_register(doc_harness, 0xE0, sim_time, clk, tfp, dump_trace);
                printf("Interrupt register value: 0x%02X\n", interrupt_reg);
                
                // Check control register to verify oscillator was halted
                uint8_t control_reg = read_register(doc_harness, 0xA0, sim_time, clk, tfp, dump_trace);
                printf("Control register value: 0x%02X (halt bit = %d)\n", 
                    control_reg, control_reg & 0x01);
            }
            
            // Debug output for first few cycles
            if (cycle < 100 && cycle % 20 == 0) {
                printf("Cycle %d: IRQ line = %d\n", cycle, doc_harness->irq_n_o);
            }
            
            // If we found interrupt and enough cycles passed, we can stop
            if (interrupt_fired && cycle > (interrupt_cycle + 100)) {
                printf("Stopping test after interrupt + 100 cycles\n");
                break;
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write trace if enabled
        if (dump_trace) tfp->dump(sim_time++);
    }
    
    // Reset DOC for next test
    reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
    
    // === TEST 2: Verify clearing of interrupt ===
    printf("\n--- TEST 2: Clearing interrupt by writing to interrupt register ---\n");
    
    // Enable one oscillator
    write_register(doc_harness, 0xE1, 0x02, sim_time, clk, tfp, dump_trace);
    
    // Setup oscillator 0 again with interrupt enabled
    setup_oscillator(doc_harness, 0, 0x0200, 0xFF, 0x1A, sim_time, clk, tfp, dump_trace);
    
    // Clear interrupt register before starting
    write_register(doc_harness, 0xE0, 0x00, sim_time, clk, tfp, dump_trace);
    
    // Variables for tracking test state
    interrupt_fired = false;
    int interrupt_clear_cycle = 0;
    bool interrupt_cleared = false;
    
    printf("Running simulation for interrupt clear test...\n");
    
    // Run simulation until interrupt fires and then clear it
    for (int cycle = 0; cycle < 20000; cycle++) {
        // Toggle clock
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            // Generate DOC clk_en_i pulse
            bool doc_clk_en = ((cycle % 8) == 0);
            doc_harness->clk_en_i = doc_clk_en;
            
            // When the DOC requests wave data, provide it from our wavetable
            if (doc_harness->wave_rd_o) {
                uint16_t wave_addr = doc_harness->wave_address_o & 0xFF;
                doc_harness->wave_data_i = test_wavetable[wave_addr];
                doc_harness->wave_data_ready_i = 1;
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Check if interrupt line goes low (active)
            if (!doc_harness->irq_n_o && !interrupt_fired) {
                interrupt_fired = true;
                printf("Cycle %d: INTERRUPT DETECTED (irq_n_o = %d)\n", 
                    cycle, doc_harness->irq_n_o);
                
                // Read the interrupt register
                uint8_t interrupt_reg = read_register(doc_harness, 0xE0, sim_time, clk, tfp, dump_trace);
                printf("Interrupt register value: 0x%02X\n", interrupt_reg);
                
                // Wait 50 cycles before clearing the interrupt
                interrupt_clear_cycle = cycle + 50;
            }
            
            // Clear the interrupt after waiting a bit
            if (interrupt_fired && cycle == interrupt_clear_cycle && !interrupt_cleared) {
                printf("Cycle %d: Clearing interrupt by writing to interrupt register\n", cycle);
                
                // Writing to the interrupt register clears the interrupt
                write_register(doc_harness, 0xE0, 0x00, sim_time, clk, tfp, dump_trace);
                interrupt_cleared = true;
            }
            
            // Check if interrupt cleared successfully
            if (interrupt_cleared && doc_harness->irq_n_o == 1) {
                printf("Cycle %d: INTERRUPT CLEARED SUCCESSFULLY (irq_n_o = %d)\n", 
                    cycle, doc_harness->irq_n_o);
                
                // Wait a bit more, then end the test
                if (cycle > (interrupt_clear_cycle + 50)) {
                    printf("Stopping test after interrupt cleared + 50 cycles\n");
                    break;
                }
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write trace if enabled
        if (dump_trace) tfp->dump(sim_time++);
    }
    
    // Reset DOC for next test
    reset_doc(doc_harness, sim_time, clk, tfp, dump_trace);
    
    // === TEST 3: Multiple oscillators with interrupts enabled ===
    printf("\n--- TEST 3: Multiple oscillators with interrupts ---\n");
    
    // Enable two oscillators
    write_register(doc_harness, 0xE1, 0x04, sim_time, clk, tfp, dump_trace);
    
    // Setup oscillator 0 with interrupt enabled, one-shot mode
    setup_oscillator(doc_harness, 0, 0x0100, 0xFF, 0x1A, sim_time, clk, tfp, dump_trace);
    
    // Setup oscillator 1 with interrupt enabled, one-shot mode but higher frequency
    // This should finish first and trigger the interrupt first
    setup_oscillator(doc_harness, 1, 0x0300, 0xFF, 0x1A, sim_time, clk, tfp, dump_trace);
    
    // Clear interrupt register before starting
    write_register(doc_harness, 0xE0, 0x00, sim_time, clk, tfp, dump_trace);
    
    printf("Running simulation with multiple oscillators...\n");
    
    // Track which oscillator triggered the interrupt
    int interrupting_osc = -1;
    
    // Run simulation until we see which oscillator triggers first
    for (int cycle = 0; cycle < 20000; cycle++) {
        // Toggle clock
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            // Generate DOC clk_en_i pulse
            bool doc_clk_en = ((cycle % 8) == 0);
            doc_harness->clk_en_i = doc_clk_en;
            
            // When the DOC requests wave data, provide it from our wavetable
            if (doc_harness->wave_rd_o) {
                uint16_t wave_addr = doc_harness->wave_address_o & 0xFF;
                doc_harness->wave_data_i = test_wavetable[wave_addr];
                doc_harness->wave_data_ready_i = 1;
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Check if interrupt line goes low (active)
            if (!doc_harness->irq_n_o && interrupting_osc == -1) {
                printf("Cycle %d: INTERRUPT DETECTED (irq_n_o = %d)\n", 
                    cycle, doc_harness->irq_n_o);
                
                // Read the interrupt register to see which oscillator triggered it
                uint8_t interrupt_reg = read_register(doc_harness, 0xE0, sim_time, clk, tfp, dump_trace);
                printf("Interrupt register value: 0x%02X\n", interrupt_reg);
                
                // Determine which oscillator triggered the interrupt
                if (interrupt_reg & 0x01) {
                    interrupting_osc = 0;
                    printf("Oscillator 0 triggered the interrupt\n");
                } else if (interrupt_reg & 0x02) {
                    interrupting_osc = 1;
                    printf("Oscillator 1 triggered the interrupt\n");
                } else {
                    printf("Unknown oscillator triggered the interrupt!\n");
                }
                
                // Check control registers to verify oscillators were halted
                uint8_t control_reg0 = read_register(doc_harness, 0xA0, sim_time, clk, tfp, dump_trace);
                uint8_t control_reg1 = read_register(doc_harness, 0xA1, sim_time, clk, tfp, dump_trace);
                printf("Control register 0: 0x%02X (halt bit = %d)\n", 
                    control_reg0, control_reg0 & 0x01);
                printf("Control register 1: 0x%02X (halt bit = %d)\n", 
                    control_reg1, control_reg1 & 0x01);
                
                // Wait a bit more, then end the test
                if (cycle > 100) {
                    printf("Stopping test after interrupt detected\n");
                    break;
                }
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write trace if enabled
        if (dump_trace) tfp->dump(sim_time++);
    }
    
    // Print overall results
    printf("\n===== DOC5503 INTERRUPT TEST RESULTS =====\n");
    
    printf("TEST 1: Interrupt on oscillator halt in one-shot mode: %s\n", 
        interrupt_fired ? "PASSED" : "FAILED");
    
    printf("TEST 2: Clearing interrupt by writing to interrupt register: %s\n", 
        (interrupt_fired && interrupt_cleared && doc_harness->irq_n_o == 1) ? "PASSED" : "FAILED");
    
    printf("TEST 3: Multiple oscillators with interrupts: %s\n",
        (interrupting_osc >= 0) ? "PASSED" : "FAILED");
    
    if (interrupting_osc == 1) {
        printf("  Verified that oscillator 1 (higher frequency) triggered the interrupt first\n");
    }
    
    // Cleanup
    if (dump_trace) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    
    printf("\nTest complete. You can examine doc5503_interrupt_test.vcd for detailed signal traces.\n");
    
    return 0;
}