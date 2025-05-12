#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_reg_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    
    // Create an instance of our model
    Vdoc5503_reg_harness* dut = new Vdoc5503_reg_harness;
    
    // Enable trace dump
    VerilatedVcdC* tfp = nullptr;
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("doc5503_basic_reg_test.vcd");
    
    printf("=== DOC5503 Basic Register Test ===\n");
    
    // Clock generation
    bool clk = false;
    int sim_time = 0;
    
    // Initialize signals
    dut->reset_n_i = 0;
    dut->clk_i = clk;
    dut->clk_en_i = 0;
    dut->cs_n_i = 1;
    dut->we_n_i = 1;
    dut->addr_i = 0;
    dut->data_i = 0;
    dut->wave_data_ready_i = 0;
    dut->wave_data_i = 0;
    
    // Reset pulse
    printf("Applying reset...\n");
    for (int i = 0; i < 10; i++) {
        clk = !clk;
        dut->clk_i = clk;
        dut->eval();
        tfp->dump(sim_time++);
    }
    dut->reset_n_i = 1;
    printf("Reset released\n");
    
    // Continue with CONSISTENT clk_en_i pulses until ready
    // DOC5503 requires a clk_en_i pulse every 8 clk_i cycles (every 4 rising edges)
    printf("Waiting for DOC5503 to be ready with proper clk_en_i timing...\n");
    int ready_wait = 0;
    int clk_en_counter = 0;
    int rising_edges = 0;

    while (!dut->ready_o && ready_wait < 1000) {
        clk = !clk;
        dut->clk_i = clk;

        // Ensure one clk_en_i pulse every 8 clock cycles (4 rising edges)
        if (clk) { // Only on rising edge
            rising_edges++;
            if (rising_edges == 4) { // Every 4th rising edge (8 clock transitions)
                dut->clk_en_i = 1;
                rising_edges = 0;
                printf("  clk_en_i pulse at cycle %d\n", ready_wait);
            } else {
                dut->clk_en_i = 0;
            }
        } else {
            dut->clk_en_i = 0; // Always off on falling edge
        }

        dut->eval();
        tfp->dump(sim_time++);
        ready_wait++;
    }
    
    if (!dut->ready_o) {
        printf("ERROR: DOC5503 never became ready!\n");
        return 1;
    }
    
    printf("DOC5503 ready after %d cycles\n", ready_wait);
    
    // Register test sequence
    const int DOC_CLK_DIVIDER = 8;  // clk_en_i pulse every 8 cycles
    
    // Test registers - focusing only on the global register first
    struct RegTest {
        uint8_t addr;
        uint8_t value;
        const char* name;
    };

    RegTest tests[] = {
        {0xE1, 0x02, "Oscillator Enable"} // Focus only on global register for now
    };
    
    // Reset the clk_en_counter for consistent timing
    clk_en_counter = 0;

    // Separate helper function for register write with proper clk_en_i pulses
    auto write_register = [&](uint8_t addr, uint8_t data) {
        printf("  Writing 0x%02X to register 0x%02X\n", data, addr);

        // Step 1: Setup address and data with CS=1, WE=1
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = addr;
        dut->data_i = data;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 2; i++) { // Two full clock cycles
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse during setup\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // Step 2: Assert CS first (CS negative edge)
        dut->cs_n_i = 0;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 4; i++) { // Four full clock cycles with CS asserted
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse with CS low\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // Step 3: Assert WE (WE negative edge) for write operation
        dut->cs_n_i = 0;
        dut->we_n_i = 0;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 8; i++) { // Eight full clock cycles for write operation
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse during write operation\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // Step 4: Release WE first, keeping CS asserted
        dut->we_n_i = 1;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 4; i++) { // Four full clock cycles
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse after releasing WE\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // Step 5: Release CS to complete the write cycle
        dut->cs_n_i = 1;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 4; i++) { // Four full clock cycles
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse after releasing CS\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }
    };
    
    // Separate helper function for register read with proper clk_en_i pulses
    auto read_register = [&](uint8_t addr) -> uint8_t {
        printf("  Reading from register 0x%02X\n", addr);

        // Step 1: Setup address with CS=1, WE=1
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = addr;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 2; i++) { // Two full clock cycles
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse during read setup\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // Step 2: Assert CS for read (CS negative edge, WE stays high)
        dut->cs_n_i = 0;

        // Pulse clock with proper clk_en_i for read operation
        for (int i = 0; i < 16; i++) { // Run several cycles with CS asserted for read
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse during read operation (cycle %d)\n", i/2);
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // Capture data
        uint8_t result = dut->data_o;
        printf("    Read value: 0x%02X\n", result);

        // Release CS
        dut->cs_n_i = 1;

        // Clock cycle with proper clk_en_i
        for (int i = 0; i < 4; i++) { // Four full clock cycles
            clk = 0;
            dut->clk_i = clk;
            dut->clk_en_i = 0; // Always off on falling edge
            dut->eval();
            tfp->dump(sim_time++);

            clk = 1;
            dut->clk_i = clk;

            if (++clk_en_counter == 8) {
                dut->clk_en_i = 1;
                clk_en_counter = 0;
                printf("    clk_en_i pulse after releasing CS\n");
            } else {
                dut->clk_en_i = 0;
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        return result;
    };
    
    // Run through the register tests
    printf("\nStarting register tests...\n");

    for (const auto& test : tests) {
        printf("\nTesting %s register (0x%02X):\n", test.name, test.addr);

        // 1. Normal register write
        printf("  Attempting standard register write...\n");
        write_register(test.addr, test.value);

        // Wait with proper clk_en_i pulses to ensure write is processed
        printf("  Waiting for write to complete...\n");
        for (int i = 0; i < 40; i++) {
            clk = !clk;
            dut->clk_i = clk;

            if (clk) { // Only on rising edge
                if (++clk_en_counter == 8) {
                    dut->clk_en_i = 1;
                    clk_en_counter = 0;
                    printf("    clk_en_i pulse while waiting (cycle %d)\n", i);
                } else {
                    dut->clk_en_i = 0;
                }
            } else {
                dut->clk_en_i = 0; // Always off on falling edge
            }

            dut->eval();
            tfp->dump(sim_time++);
        }

        // 2. Read the register back
        uint8_t read_value = read_register(test.addr);

        // 3. Verify the result
        if (read_value == test.value) {
            printf("  SUCCESS: Register verified 0x%02X == 0x%02X\n", read_value, test.value);
        } else {
            printf("  FAILURE: Register verification failed, expected 0x%02X, got 0x%02X\n",
                   test.value, read_value);

            // 4. Try writing the register multiple times with different timing
            printf("\n  Trying alternative write approach...\n");

            // Multiple attempts with different timing parameters
            for (int attempt = 0; attempt < 5; attempt++) {
                printf("  Attempt %d: Writing register with different timing...\n", attempt + 1);

                // Reset CS and WE
                dut->cs_n_i = 1;
                dut->we_n_i = 1;

                // Run several clock cycles with clk_en_i pulses between attempts
                for (int i = 0; i < 20; i++) {
                    clk = !clk;
                    dut->clk_i = clk;

                    if (clk) { // Only on rising edge
                        if (++clk_en_counter == 8) {
                            dut->clk_en_i = 1;
                            clk_en_counter = 0;
                        } else {
                            dut->clk_en_i = 0;
                        }
                    } else {
                        dut->clk_en_i = 0;
                    }

                    dut->eval();
                    tfp->dump(sim_time++);
                }

                // Different CS/WE sequence for each attempt
                if (attempt == 0) {
                    // CS first, then WE
                    dut->addr_i = test.addr;
                    dut->data_i = test.value;

                    // Assert CS
                    dut->cs_n_i = 0;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);

                    // Assert WE
                    dut->we_n_i = 0;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->clk_en_i = 1; // Force clk_en_i pulse
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_en_i = 0;

                    // Release WE and CS
                    dut->we_n_i = 1;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);

                    dut->cs_n_i = 1;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                }
                else if (attempt == 1) {
                    // WE and CS together
                    dut->addr_i = test.addr;
                    dut->data_i = test.value;

                    // Assert CS and WE simultaneously
                    dut->cs_n_i = 0;
                    dut->we_n_i = 0;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->clk_en_i = 1;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_en_i = 0;

                    // Release CS and WE
                    dut->cs_n_i = 1;
                    dut->we_n_i = 1;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                }
                else if (attempt == 2) {
                    // Multiple clk_en_i pulses during write
                    dut->addr_i = test.addr;
                    dut->data_i = test.value;

                    // Assert CS and WE
                    dut->cs_n_i = 0;
                    dut->we_n_i = 0;

                    // Run several clock cycles with clk_en_i pulses
                    for (int i = 0; i < 20; i++) {
                        clk = !clk;
                        dut->clk_i = clk;

                        if (clk) {
                            dut->clk_en_i = 1; // Force clk_en_i pulse on each rising edge
                        } else {
                            dut->clk_en_i = 0;
                        }

                        dut->eval();
                        tfp->dump(sim_time++);
                    }

                    // Release WE and CS
                    dut->we_n_i = 1;
                    dut->cs_n_i = 1;
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                    dut->clk_i = !dut->clk_i;
                    dut->eval();
                    tfp->dump(sim_time++);
                }

                // Wait with clk_en_i pulses
                for (int i = 0; i < 20; i++) {
                    clk = !clk;
                    dut->clk_i = clk;

                    if (clk) {
                        if (++clk_en_counter == 8) {
                            dut->clk_en_i = 1;
                            clk_en_counter = 0;
                        } else {
                            dut->clk_en_i = 0;
                        }
                    } else {
                        dut->clk_en_i = 0;
                    }

                    dut->eval();
                    tfp->dump(sim_time++);
                }

                // Read and check the register again
                read_value = read_register(test.addr);
                if (read_value == test.value) {
                    printf("  SUCCESS on attempt %d: Register verified 0x%02X == 0x%02X\n",
                           attempt + 1, read_value, test.value);
                    break;
                } else {
                    printf("  FAILURE on attempt %d: Register verification failed, expected 0x%02X, got 0x%02X\n",
                           attempt + 1, test.value, read_value);
                }
            }
        }
    }
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete dut;
    
    return 0;
}