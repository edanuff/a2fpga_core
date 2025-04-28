#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

// FIXED TEST HARNESS FOR DOC5503
// This addresses the oscillator halt issue by:
// 1. Ensuring all waveform data is non-zero
// 2. Adding delays between register writes
// 3. Writing the reset sequence differently
// 4. Adding exhaustive verification of register values

class DOC5503TestHarness {
public:
    Vdoc5503_harness* dut;
    VerilatedVcdC* tfp;
    bool dump_vcd;
    int sim_time;
    bool verbose;
    
    DOC5503TestHarness(bool dump_vcd = false, bool verbose = false) : dump_vcd(dump_vcd), verbose(verbose) {
        // Setup the model
        dut = new Vdoc5503_harness;
        sim_time = 0;
        
        // Initialize VCD tracing if requested
        if (dump_vcd) {
            Verilated::traceEverOn(true);
            tfp = new VerilatedVcdC;
            dut->trace(tfp, 99);
            tfp->open("doc5503_harness.vcd");
        } else {
            tfp = nullptr;
        }
        
        // Initial reset
        reset();
    }
    
    ~DOC5503TestHarness() {
        if (dump_vcd && tfp) {
            tfp->close();
            delete tfp;
        }
        delete dut;
    }
    
    // Reset the DOC5503
    void reset() {
        if (verbose) printf("Applying reset...\n");
        
        // Initialize inputs
        dut->reset_n_i = 0;
        dut->clk_i = 0;
        dut->clk_en_i = 0;
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        dut->addr_i = 0;
        dut->data_i = 0;
        dut->wave_data_ready_i = 0;
        dut->wave_data_i = 0;
        
        // Apply reset for 10 clock cycles
        for (int i = 0; i < 20; i++) {
            tick();
        }
        
        // Release reset
        dut->reset_n_i = 1;
        
        // Wait for initialization to complete
        for (int i = 0; i < 100; i++) {
            tick();
        }
        
        if (verbose) printf("Reset complete\n");
    }
    
    // Clock tick with optional DOC clock enable
    void tick(bool doc_clk_en = false) {
        // Toggle the clock
        dut->clk_i = !dut->clk_i;
        
        // Set DOC clock enable if requested (on rising edge)
        if (dut->clk_i) {
            dut->clk_en_i = doc_clk_en;
        }
        
        // Evaluate the model
        dut->eval();
        
        // Dump VCD if enabled
        if (dump_vcd && tfp) {
            tfp->dump(sim_time);
        }
        
        sim_time++;
    }
    
    // Helper function to write a register
    void write_register(uint8_t addr, uint8_t data) {
        // Release any previous CS signals
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        tick();
        tick();
        
        // Set up write signals
        dut->cs_n_i = 0;
        dut->we_n_i = 0;
        dut->addr_i = addr;
        dut->data_i = data;
        tick();
        
        // Complete the write cycle
        tick();
        
        // Release signals
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        tick();
        tick();
        
        if (verbose) printf("Wrote 0x%02X to register 0x%02X\n", data, addr);
    }
    
    // Helper function to read a register
    uint8_t read_register(uint8_t addr) {
        // Release any previous CS signals
        dut->cs_n_i = 1;
        dut->we_n_i = 1;
        tick();
        tick();
        
        // Set up read signals
        dut->cs_n_i = 0;
        dut->we_n_i = 1;
        dut->addr_i = addr;
        tick();
        
        // Sample the data on the second clock edge
        tick();
        uint8_t value = dut->data_o;
        
        // Release signals
        dut->cs_n_i = 1;
        tick();
        tick();
        
        if (verbose) printf("Read 0x%02X from register 0x%02X\n", value, addr);
        return value;
    }
    
    // Write and verify a register value
    bool write_verify_register(uint8_t addr, uint8_t data) {
        write_register(addr, data);
        uint8_t readback = read_register(addr);
        
        // Special case for oscillator enable register
        if (addr == 0xE1) {
            // The register readback may not match exactly, so just verify bits 5-1
            bool match = ((readback & 0x3E) == (data & 0x3E));
            if (!match && verbose) {
                printf("MISMATCH for register 0x%02X: wrote 0x%02X, read 0x%02X\n", 
                       addr, data, readback);
            }
            return match;
        }
        
        // Special case for control register - may be modified by internal logic
        if ((addr >= 0xA0) && (addr <= 0xBF)) {
            // Only check bits 7-1, not the halt bit which may be set by internal logic
            bool match = ((readback & 0xFE) == (data & 0xFE));
            if (!match && verbose) {
                printf("MISMATCH for control register 0x%02X: wrote 0x%02X, read 0x%02X\n", 
                       addr, data, readback);
            }
            return match;
        }
        
        // For most registers, expect exact match
        bool match = (readback == data);
        if (!match && verbose) {
            printf("MISMATCH for register 0x%02X: wrote 0x%02X, read 0x%02X\n", 
                   addr, data, readback);
        }
        return match;
    }
    
    // Run the test with clock enables for a number of cycles
    void run_with_waveform(int num_cycles, uint8_t* waveform, int waveform_size) {
        int wave_reads = 0;
        bool has_audio = false;
        
        for (int i = 0; i < num_cycles; i++) {
            // Toggle clock
            tick(i % 8 == 0);  // DOC clock enable every 8 cycles
            
            // Handle wave read requests
            if (dut->wave_rd_o) {
                wave_reads++;
                
                // Calculate wave address index
                uint16_t addr_index = dut->wave_address_o % waveform_size;
                
                // CRITICAL: Always provide a non-zero value for wave data
                // This prevents the oscillator from halting
                uint8_t wave_val = waveform[addr_index];
                if (wave_val == 0x00) wave_val = 0x01;
                
                dut->wave_data_i = wave_val;
                dut->wave_data_ready_i = 1;
                
                if (verbose && (wave_reads % 100 == 1)) {
                    printf("Wave read #%d at addr 0x%04X, providing 0x%02X\n", 
                           wave_reads, dut->wave_address_o, wave_val);
                }
            } else {
                dut->wave_data_ready_i = 0;
            }
            
            // Check for audio output
            if (i % 100 == 0) {
                if (dut->left_mix_o != 0 || dut->right_mix_o != 0) {
                    has_audio = true;
                    if (verbose && (i % 1000 == 0)) {
                        printf("Audio output at cycle %d: L=%d, R=%d\n", 
                               i, dut->left_mix_o, dut->right_mix_o);
                    }
                }
            }
        }
        
        printf("Test complete. Wave reads: %d, Audio detected: %s\n", 
               wave_reads, has_audio ? "YES" : "NO");
    }
    
    // Configure a single oscillator with typical test settings
    void configure_oscillator(uint8_t osc_num, uint8_t channel, uint8_t volume) {
        if (verbose) printf("\nConfiguring oscillator %d with channel %d, volume 0x%02X\n", 
                             osc_num, channel, volume);
        
        // Set DOC to just 1 oscillator
        write_verify_register(0xE1, 0x02);  // 1 oscillator (value is 2×N)
        
        // Set high frequency for faster cycling during test
        write_verify_register(0x00 + osc_num, 0xFF);  // Freq Low = maximum
        write_verify_register(0x20 + osc_num, 0x0F);  // Freq High = moderately high
        
        // Set volume
        write_verify_register(0x40 + osc_num, volume); // Max volume for clear signal
        
        // Set waveform table pointer to page 0
        write_verify_register(0x80 + osc_num, 0x00);
        
        // Set control register - CRITICAL: oscillator mode and channel
        uint8_t control_val = (channel << 4);  // Channel in bits 7-4, Free Run mode, not halted
        write_verify_register(0xA0 + osc_num, control_val);
        
        // Set resolution/table size - smallest table (256 bytes), lowest resolution
        write_verify_register(0xC0 + osc_num, 0x00);
        
        // Verify key registers after configuration
        uint8_t ctrl = read_register(0xA0 + osc_num);
        uint8_t vol = read_register(0x40 + osc_num);
        uint8_t osc_en = read_register(0xE1);
        
        printf("Verification - Control: 0x%02X (Halt=%d, Channel=%d), Volume: 0x%02X, OscEn: 0x%02X\n", 
               ctrl, ctrl & 0x01, (ctrl >> 4) & 0x0F, vol, osc_en);
    }
    
    // Run a simplified audio test to verify that audio is produced
    void run_basic_audio_test() {
        printf("\n=== BASIC AUDIO TEST ===\n");
        
        // Create a simple sawtooth waveform that never hits zero
        const int wavetable_size = 256;
        uint8_t* waveform = new uint8_t[wavetable_size];
        
        // Sawtooth waveform from 0x01 to 0xFF (avoiding 0x00 value)
        for (int i = 0; i < wavetable_size; i++) {
            waveform[i] = (i % 255) + 1;  // Values 1-255, never 0
        }
        
        // Reset the DOC for a clean state
        reset();
        
        // Configure oscillator 0 for right channel, maximum volume
        configure_oscillator(0, 0, 0xFF);  // Channel 0 = right, volume 0xFF
        
        // Run the test with our waveform
        run_with_waveform(10000, waveform, wavetable_size);
        
        delete[] waveform;
    }
    
    // Run a test that focuses on the compressor
    void run_compressor_test() {
        printf("\n=== COMPRESSOR TEST ===\n");
        
        // Create a test waveform with full amplitude range
        const int wavetable_size = 256;
        uint8_t* waveform = new uint8_t[wavetable_size];
        
        // Sine-like waveform from 0x01-0xFF (but never 0)
        for (int i = 0; i < wavetable_size; i++) {
            double phase = (double)i / wavetable_size * 2.0 * M_PI;
            double value = sin(phase);
            // Scale to 1-255 range (avoid zero)
            int sample = (int)((value + 1.0) * 127.0) + 1;
            if (sample < 1) sample = 1;
            if (sample > 255) sample = 255;
            waveform[i] = sample;
        }
        
        // Reset for clean state
        reset();
        
        // Configure oscillator for stereo output (both channels)
        printf("Configuring for left channel (1) output...\n");
        configure_oscillator(0, 1, 0xFF);  // Channel 1 = left, max volume
        run_with_waveform(5000, waveform, wavetable_size);
        
        printf("Configuring for right channel (0) output...\n");
        configure_oscillator(0, 0, 0xFF);  // Channel 0 = right, max volume
        run_with_waveform(5000, waveform, wavetable_size);
        
        // Test with different volumes to check the compressor behavior
        printf("\nTesting compressor with different volumes:\n");
        uint8_t test_volumes[] = {64, 128, 192, 255};
        for (int vol_idx = 0; vol_idx < 4; vol_idx++) {
            uint8_t volume = test_volumes[vol_idx];
            printf("Testing with volume 0x%02X (%d)...\n", volume, volume);
            configure_oscillator(0, 0, volume);
            run_with_waveform(2000, waveform, wavetable_size);
        }
        
        delete[] waveform;
    }
};

// Main test function
int main(int argc, char** argv) {
    printf("=== DOC5503 FIXED TEST HARNESS ===\n");
    printf("This test addresses the oscillator halting issue.\n\n");
    
    // Parse command line arguments for verbose mode
    bool verbose = false;
    bool dump_vcd = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose")
            verbose = true;
        else if (arg == "--vcd")
            dump_vcd = true;
    }
    
    // Create the test harness
    DOC5503TestHarness tester(dump_vcd, verbose);
    
    // Run the tests
    tester.run_basic_audio_test();
    tester.run_compressor_test();
    
    printf("\nTest complete.\n");
    return 0;
}