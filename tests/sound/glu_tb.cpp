#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vglu_harness.h"

// Log levels
enum LogLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3
};

// Current log level
LogLevel currentLogLevel = INFO;

// Logging macros
#define LOG_ERROR(fmt, ...) if (currentLogLevel >= ERROR) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) if (currentLogLevel >= WARNING) printf("[WARNING] " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) if (currentLogLevel >= INFO) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) if (currentLogLevel >= DEBUG) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

// Simulation parameters
#define MAX_SIM_TIME 5000000 // Increased for longer simulation
#define CLOCK_PERIOD 20 // 50 MHz clock

// GLU register addresses (as seen by Apple II)
#define GLU_SOUND_CONTROL 0xC03C
#define GLU_SOUND_DATA    0xC03D
#define GLU_SOUND_PTR_LO  0xC03E
#define GLU_SOUND_PTR_HI  0xC03F

// IIgs GLU control register bits
#define GLU_CONTROL_ACCESS_RAM  0x40  // 1 = Access RAM, 0 = Access DOC
#define GLU_CONTROL_AUTO_INC    0x20  // 1 = Auto-increment address after data access
#define GLU_CONTROL_VOL_MASK    0x0F  // Volume mask (bits 0-3)

// DOC Oscillator register offsets
#define DOC_FREQ_LO    0x00  // Oscillator frequency lo ($00-$1F)
#define DOC_FREQ_HI    0x20  // Oscillator frequency hi ($20-$3F)
#define DOC_VOL        0x40  // Oscillator volume ($40-$5F)
#define DOC_DATA       0x60  // Oscillator waveform data ($60-$7F)
#define DOC_WAVEPTR    0x80  // Oscillator wave pointer ($80-$9F)
#define DOC_CONTROL    0xA0  // Oscillator control ($A0-$BF)
#define DOC_TABLE_RES  0xC0  // Oscillator table/resolution ($C0-$DF)
#define DOC_OSC_ENABLE 0xE1  // Oscillator enable (global)

// Wavetable data to be returned for read operations
uint8_t waveTableData[65536];

// Main test function
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    // Enable trace output
    Verilated::traceEverOn(true);
    
    // Create an instance of the model
    Vglu_harness* glu = new Vglu_harness;
    
    // Setup trace dump
    VerilatedVcdC* tfp = new VerilatedVcdC;
    glu->trace(tfp, 99);
    tfp->open("glu_trace.vcd");
    
    // Initialize wavetable data with a simple sine-like pattern
    for (int i = 0; i < 256; i++) {
        // Create a pattern that rises from 128 to 200 and back to 128
        if (i < 128) {
            waveTableData[i] = 128 + (i / 2);
        } else {
            waveTableData[i] = 128 + (128 - (i - 128)) / 2;
        }
    }
    
    // Fill the rest with zeros (silent)
    for (int i = 256; i < 65536; i++) {
        waveTableData[i] = 128;  // 128 = zero/silence in DOC format
    }
    
    LOG_INFO("Starting GLU test simulation");
    
    // Simulation variables
    int clockCounter = 0;
    bool clock7m = false;
    
    // Reset the model
    LOG_INFO("Resetting the model");
    glu->reset_n_i = 0;
    
    // Initial state for other inputs
    glu->clk_i = 0;
    glu->reg_cs_n_i = 1;  // Not selected
    glu->reg_we_n_i = 1;  // Not writing
    glu->reg_addr_i = 0;
    glu->reg_data_i = 0;
    glu->clk_7m_posedge_i = 0;
    glu->sdram_ready_i = 0;
    glu->sdram_data_i = 0;
    
    // Run a few cycles with reset
    for (int i = 0; i < 10; i++) {
        glu->clk_i = !glu->clk_i;
        glu->eval();
        tfp->dump(clockCounter * CLOCK_PERIOD / 2);
        clockCounter++;
    }
    
    // Release reset
    glu->reset_n_i = 1;
    
    // Run a few cycles after reset
    for (int i = 0; i < 10; i++) {
        glu->clk_i = !glu->clk_i;
        glu->eval();
        tfp->dump(clockCounter * CLOCK_PERIOD / 2);
        clockCounter++;
    }
    
    // Helper function to write to GLU registers
    auto writeGLU = [&](uint16_t addr, uint8_t data) {
        glu->reg_addr_i = addr;
        glu->reg_data_i = data;
        glu->reg_cs_n_i = 0;  // Select the chip
        glu->reg_we_n_i = 0;  // Write mode
        
        // Clock for 2 cycles to ensure write happens
        for (int i = 0; i < 4; i++) {
            glu->clk_i = !glu->clk_i;
            glu->eval();
            tfp->dump(clockCounter * CLOCK_PERIOD / 2);
            clockCounter++;
        }
        
        // Deselect
        glu->reg_cs_n_i = 1;
        glu->reg_we_n_i = 1;
        
        LOG_INFO("Wrote 0x%02X to GLU register 0x%04X", data, addr);
    };
    
    // Configure the GLU control register - set to max volume and auto-increment
    LOG_INFO("Configuring GLU control register");
    writeGLU(GLU_SOUND_CONTROL, GLU_CONTROL_AUTO_INC | 0x0F);  // Max volume (0x0F) and auto-increment
    
    // Point to DOC oscillator register
    LOG_INFO("Setting DOC pointer to oscillator 0 frequency register");
    writeGLU(GLU_SOUND_PTR_LO, 0x00);  // Frequency Low byte for oscillator 0
    writeGLU(GLU_SOUND_PTR_HI, 0x00);  // High byte = 0
    
    // Configure the first oscillator
    // Write Frequency Low (250)
    LOG_INFO("Writing oscillator 0 frequency (low)");
    writeGLU(GLU_SOUND_DATA, 250);  // Frequency low byte
    
    // Frequency High (1) - auto-increment should move to this register
    LOG_INFO("Writing oscillator 0 frequency (high)");
    writeGLU(GLU_SOUND_DATA, 1);    // Frequency high byte
    
    // Now write to volume register ($40) - need to set pointer
    LOG_INFO("Setting DOC pointer to oscillator 0 volume register");
    writeGLU(GLU_SOUND_PTR_LO, DOC_VOL);
    
    // Set oscillator 0 volume to maximum
    LOG_INFO("Writing oscillator 0 volume (max)");
    writeGLU(GLU_SOUND_DATA, 255);  // Maximum volume
    
    // Now write to control register ($A0) - need to set pointer
    LOG_INFO("Setting DOC pointer to oscillator 0 control register");
    writeGLU(GLU_SOUND_PTR_LO, DOC_CONTROL);
    
    // Set oscillator 0 control - Free running mode (00), no halt (0), channel 0 (0000)
    LOG_INFO("Writing oscillator 0 control (free running, channel 0)");
    writeGLU(GLU_SOUND_DATA, 0x00);  // Free run on channel 0
    
    // Set table/resolution register ($C0)
    LOG_INFO("Setting DOC pointer to oscillator 0 table/resolution register");
    writeGLU(GLU_SOUND_PTR_LO, DOC_TABLE_RES);
    
    // Set table size to 256 bytes (000), resolution to match (000)
    LOG_INFO("Writing oscillator 0 table size and resolution");
    writeGLU(GLU_SOUND_DATA, 0x00);  // 256 byte table, resolution 0
    
    // Set pointer register for wave table start
    LOG_INFO("Setting DOC pointer to oscillator 0 wave pointer");
    writeGLU(GLU_SOUND_PTR_LO, DOC_WAVEPTR);
    
    // Set wave pointer to page 0
    LOG_INFO("Writing oscillator 0 wave pointer");
    writeGLU(GLU_SOUND_DATA, 0x00);  // Start at page 0
    
    // Enable oscillators
    LOG_INFO("Setting DOC pointer to oscillator enable register");
    writeGLU(GLU_SOUND_PTR_LO, DOC_OSC_ENABLE);
    
    // Enable 1 oscillator (value = 2)
    LOG_INFO("Enabling 1 oscillator");
    writeGLU(GLU_SOUND_DATA, 0x02);  // Enable 1 oscillator
    
    // Main simulation loop
    LOG_INFO("Starting main simulation loop");
    for (int simTime = 0; simTime < MAX_SIM_TIME; simTime++) {
        // Toggle clock
        glu->clk_i = !glu->clk_i;
        
        // Generate 7MHz clock enable pulse (approximately 1/7 of the main clock)
        if (simTime % 14 == 0 && glu->clk_i) {
            glu->clk_7m_posedge_i = 1;
            clock7m = true;
        } else {
            glu->clk_7m_posedge_i = 0;
        }
        
        // Process SDRAM memory requests
        if (glu->sdram_rd_o) {
            // Memory is being read, prepare data
            uint32_t address = glu->sdram_addr_o & 0xFFFF;  // Use lower 16 bits
            
            // For this test, we always return the wavetable data
            uint32_t data = 0;
            for (int i = 0; i < 4; i++) {
                data |= (uint32_t)waveTableData[(address * 4 + i) & 0xFFFF] << (i * 8);
            }
            
            // Set the memory data and ready signal
            glu->sdram_data_i = data;
            glu->sdram_ready_i = 1;
            
            LOG_DEBUG("SDRAM read from address 0x%06X, returning 0x%08X", glu->sdram_addr_o, data);
        } else {
            glu->sdram_ready_i = 0;
        }
        
        // Evaluate model
        glu->eval();
        
        // Dump variables to trace file
        tfp->dump(simTime * CLOCK_PERIOD / 2);
        
        // Print audio output every 10000 cycles
        if (simTime % 10000 == 0) {
            // Handle signed values correctly by casting to int16_t
            int16_t audio_l = static_cast<int16_t>(glu->audio_l_o);
            int16_t audio_r = static_cast<int16_t>(glu->audio_r_o);
            LOG_INFO("Time %d: Audio L: %d, Audio R: %d", 
                     simTime, audio_l, audio_r);
        }
        
        // Increment the counter
        clockCounter++;
        
        // Check if we're exceeding simulation time
        if (Verilated::gotFinish()) {
            break;
        }
    }
    
    // Clean up
    LOG_INFO("Cleaning up");
    tfp->close();
    delete tfp;
    delete glu;
    
    LOG_INFO("Simulation complete");
    return 0;
}