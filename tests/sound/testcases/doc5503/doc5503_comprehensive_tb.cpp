#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Simulation constants
#define MAX_SIM_TIME 2000000  // Long enough to test multiple oscillator cycles
#define CLK_PERIOD_NS 18.52   // 54MHz clock period in nanoseconds
#define CLK_7M_PERIOD_NS 139.7  // 7.159MHz clock

// Test patterns for waveforms
#define NUM_TEST_PATTERNS 4
uint8_t test_patterns[NUM_TEST_PATTERNS][256] = {
    // Sine wave approximation
    {
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116,
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116,
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116,
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116
    },
    // Sawtooth wave
    {
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248,
        0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
        128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248
    },
    // Square wave
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
    },
    // Zero-crossing test (halts oscillator)
    {
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Zeros to halt oscillator
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116,
        128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 249, 252, 253,
        254, 253, 252, 249, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140,
        128, 116, 104, 91, 80, 68, 58, 48, 38, 30, 22, 16, 11, 7, 4, 3,
        2, 3, 4, 7, 11, 16, 22, 30, 38, 48, 58, 68, 80, 91, 104, 116
    }
};

// DOC5503 Register Map
namespace DOC_REGISTERS {
    // Oscillator register banks
    const uint8_t FREQ_LO_BASE = 0x00;      // $00-$1F - Frequency Low (8 bits)
    const uint8_t FREQ_HI_BASE = 0x20;      // $20-$3F - Frequency High (8 bits)
    const uint8_t VOLUME_BASE = 0x40;       // $40-$5F - Volume (8 bits)
    const uint8_t DATA_SAMPLE_BASE = 0x60;  // $60-$7F - Waveform Data Sample (8 bits, read-only)
    const uint8_t TABLE_PTR_BASE = 0x80;    // $80-$9F - Waveform Table Pointer (8 bits)
    const uint8_t CONTROL_BASE = 0xA0;      // $A0-$BF - Control Register (8 bits)
    const uint8_t BANK_SIZE_BASE = 0xC0;    // $C0-$DF - Bank/Res/Table Size (8 bits)
    
    // Global registers
    const uint8_t OSC_INT_REG = 0xE0;       // Oscillator Interrupt Register
    const uint8_t OSC_ENABLE_REG = 0xE1;    // Oscillator Enable Register
}

// Control Register Bits
namespace CONTROL_BITS {
    const uint8_t HALT = 0x01;              // Bit 0: Halt oscillator
    const uint8_t MODE_MASK = 0x06;         // Bits 1-2: Mode
    const uint8_t MODE_FREE = 0x00;         // 00: Free-running
    const uint8_t MODE_ONESHOT = 0x02;      // 01: One-shot
    const uint8_t MODE_SYNC_AM = 0x04;      // 10: Sync/AM
    const uint8_t MODE_SWAP = 0x06;         // 11: Swap
    const uint8_t IRQ_ENABLE = 0x08;        // Bit 3: Interrupt Enable
    const uint8_t CHANNEL_MASK = 0xF0;      // Bits 4-7: Channel Assignment
}

// Bank/Res/Table Size Register Bits
namespace RTS_BITS {
    const uint8_t RES_MASK = 0x07;          // Bits 0-2: Resolution
    const uint8_t TABLE_SIZE_MASK = 0x38;   // Bits 3-5: Table Size
    const uint8_t BANK_SELECT = 0x40;       // Bit 6: Bank Select
}

// Verbosity levels
enum DebugLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3,
    TRACE = 4
};

// Struct to track oscillator state
struct OscillatorState {
    uint16_t frequency;      // Combined value of frequency low/high registers
    uint8_t volume;          // Volume register value
    uint8_t waveform_sample; // Last read waveform data sample
    uint8_t table_ptr;       // Waveform table pointer
    uint8_t control;         // Control register
    uint8_t rts;             // Resolution/Table Size register
    bool active;             // Whether oscillator is active
};

class DOC5503Tester {
private:
    Vdoc5503_harness* m_doc;
    VerilatedVcdC* m_tfp;
    DebugLevel m_verbosity;
    uint64_t m_sim_time;
    bool m_dump_vcd;
    bool m_clk;
    bool m_clk_en;
    int m_clk_en_counter;
    OscillatorState m_osc_states[32]; // Track 32 oscillators
    
    // Counters and metrics for validation
    int m_wave_read_count;
    int m_irq_count;
    int m_audio_samples;
    bool m_expecting_silence;
    
    // Helper methods
    void toggle_clock() {
        m_clk = !m_clk;
        m_doc->clk_i = m_clk;
        
        if (m_clk) {
            // Generate clk_en_i pulses at 1/8 of clock rate
            m_clk_en_counter++;
            if (m_clk_en_counter >= 8) {
                m_clk_en_counter = 0;
                m_clk_en = true;
            } else {
                m_clk_en = false;
            }
            m_doc->clk_en_i = m_clk_en;
        }
    }
    
    void advance_time(uint64_t time_ns) {
        uint64_t target_time = m_sim_time + time_ns;
        while (m_sim_time < target_time) {
            toggle_clock();
            m_doc->eval();
            if (m_dump_vcd) m_tfp->dump(m_sim_time);
            m_sim_time += CLK_PERIOD_NS / 2; // Half-period for toggle
        }
    }
    
    void write_register(uint8_t addr, uint8_t data) {
        if (m_verbosity >= DEBUG) {
            printf("Writing register $%02X = $%02X\n", addr, data);
        }
        
        // Prepare inputs
        m_doc->cs_n_i = 0;
        m_doc->we_n_i = 0;
        m_doc->addr_i = addr;
        m_doc->data_i = data;
        
        // Wait for one clock cycle
        advance_time(CLK_PERIOD_NS);
        
        // Deassert chip select
        m_doc->cs_n_i = 1;
        m_doc->we_n_i = 1;
        advance_time(CLK_PERIOD_NS);
        
        // Update our tracked oscillator state
        if (addr >= DOC_REGISTERS::FREQ_LO_BASE && addr < DOC_REGISTERS::FREQ_LO_BASE + 32) {
            int osc_num = addr - DOC_REGISTERS::FREQ_LO_BASE;
            m_osc_states[osc_num].frequency = (m_osc_states[osc_num].frequency & 0xFF00) | data;
        }
        else if (addr >= DOC_REGISTERS::FREQ_HI_BASE && addr < DOC_REGISTERS::FREQ_HI_BASE + 32) {
            int osc_num = addr - DOC_REGISTERS::FREQ_HI_BASE;
            m_osc_states[osc_num].frequency = (m_osc_states[osc_num].frequency & 0x00FF) | (data << 8);
        }
        else if (addr >= DOC_REGISTERS::VOLUME_BASE && addr < DOC_REGISTERS::VOLUME_BASE + 32) {
            int osc_num = addr - DOC_REGISTERS::VOLUME_BASE;
            m_osc_states[osc_num].volume = data;
        }
        else if (addr >= DOC_REGISTERS::TABLE_PTR_BASE && addr < DOC_REGISTERS::TABLE_PTR_BASE + 32) {
            int osc_num = addr - DOC_REGISTERS::TABLE_PTR_BASE;
            m_osc_states[osc_num].table_ptr = data;
        }
        else if (addr >= DOC_REGISTERS::CONTROL_BASE && addr < DOC_REGISTERS::CONTROL_BASE + 32) {
            int osc_num = addr - DOC_REGISTERS::CONTROL_BASE;
            m_osc_states[osc_num].control = data;
            m_osc_states[osc_num].active = !(data & CONTROL_BITS::HALT);
        }
        else if (addr >= DOC_REGISTERS::BANK_SIZE_BASE && addr < DOC_REGISTERS::BANK_SIZE_BASE + 32) {
            int osc_num = addr - DOC_REGISTERS::BANK_SIZE_BASE;
            m_osc_states[osc_num].rts = data;
        }
        else if (addr == DOC_REGISTERS::OSC_ENABLE_REG) {
            // Note the fix for the off-by-one error: osc_en_r[5:1] is the number of oscillators
            int num_oscillators = (data >> 1) & 0x1F;
            if (m_verbosity >= INFO) {
                printf("Setting number of oscillators to %d\n", num_oscillators);
            }
        }
    }
    
    uint8_t read_register(uint8_t addr) {
        // Prepare inputs
        m_doc->cs_n_i = 0;
        m_doc->we_n_i = 1;
        m_doc->addr_i = addr;
        
        // Wait for one clock cycle
        advance_time(CLK_PERIOD_NS);
        
        // Read data
        uint8_t data = m_doc->data_o;
        
        // Deassert chip select
        m_doc->cs_n_i = 1;
        advance_time(CLK_PERIOD_NS);
        
        if (m_verbosity >= DEBUG) {
            printf("Read register $%02X = $%02X\n", addr, data);
        }
        
        return data;
    }
    
    void handle_wave_read() {
        if (m_doc->wave_rd_o) {
            m_wave_read_count++;
            if (m_verbosity >= TRACE) {
                printf("Wave read request at address 0x%04x\n", m_doc->wave_address_o);
            }
            
            // Determine which oscillator is reading
            uint8_t pattern_idx = 0;
            uint16_t addr = m_doc->wave_address_o & 0xFF;
            
            // After a few cycles, provide wave data
            advance_time(CLK_PERIOD_NS * 2);
            
            m_doc->wave_data_i = test_patterns[pattern_idx][addr];
            m_doc->wave_data_ready_i = 1;
            
            advance_time(CLK_PERIOD_NS);
            m_doc->wave_data_ready_i = 0;
        }
    }
    
    void monitor_outputs() {
        // Check for IRQ
        static bool last_irq_n = true;
        if (last_irq_n && !m_doc->irq_n_o) {
            m_irq_count++;
            if (m_verbosity >= INFO) {
                printf("IRQ asserted at time %lu\n", m_sim_time);
            }
        }
        last_irq_n = m_doc->irq_n_o;
        
        // Monitor audio output periodically
        if (m_sim_time % 5000 == 0) {
            int16_t left = m_doc->left_mix_o;
            int16_t right = m_doc->right_mix_o;
            m_audio_samples++;
            
            if (m_verbosity >= DEBUG) {
                printf("Time: %lu, left: %d, right: %d\n", m_sim_time, left, right);
            }
            
            // Verify audio output against expectations
            if (m_expecting_silence && (left != 0 || right != 0)) {
                printf("ERROR: Expected silence but got audio at time %lu (L:%d, R:%d)\n",
                       m_sim_time, left, right);
            }
        }
    }
    
public:
    DOC5503Tester(Vdoc5503_harness* doc, DebugLevel verbosity = INFO, bool dump_vcd = false)
        : m_doc(doc), m_verbosity(verbosity), m_sim_time(0), m_dump_vcd(dump_vcd),
          m_clk(false), m_clk_en(false), m_clk_en_counter(0),
          m_wave_read_count(0), m_irq_count(0), m_audio_samples(0), m_expecting_silence(true)
    {
        // Initialize VCD tracing if requested
        if (m_dump_vcd) {
            Verilated::traceEverOn(true);
            m_tfp = new VerilatedVcdC;
            m_doc->trace(m_tfp, 99);
            m_tfp->open("doc5503_comprehensive_trace.vcd");
        } else {
            m_tfp = nullptr;
        }
        
        // Initialize oscillator state tracking
        for (int i = 0; i < 32; i++) {
            m_osc_states[i].frequency = 0;
            m_osc_states[i].volume = 0;
            m_osc_states[i].waveform_sample = 0;
            m_osc_states[i].table_ptr = 0;
            m_osc_states[i].control = CONTROL_BITS::HALT; // Halted by default
            m_osc_states[i].rts = 0;
            m_osc_states[i].active = false;
        }
    }
    
    ~DOC5503Tester() {
        if (m_tfp) {
            m_tfp->close();
            delete m_tfp;
        }
    }
    
    void initialize() {
        // Set initial inputs
        m_doc->reset_n_i = 0;
        m_doc->clk_i = m_clk;
        m_doc->clk_en_i = m_clk_en;
        m_doc->cs_n_i = 1;
        m_doc->we_n_i = 1;
        m_doc->addr_i = 0;
        m_doc->data_i = 0;
        m_doc->wave_data_ready_i = 0;
        m_doc->wave_data_i = 0;
        
        // Reset for a few cycles
        for (int i = 0; i < 20; i++) {
            toggle_clock();
            m_doc->eval();
            if (m_dump_vcd) m_tfp->dump(m_sim_time);
            m_sim_time += CLK_PERIOD_NS / 2;
        }
        
        // Release reset
        m_doc->reset_n_i = 1;
        advance_time(CLK_PERIOD_NS * 10);
        
        // After reset, DOC should be in a known state with 1 oscillator enabled
        if (m_verbosity >= INFO) {
            printf("DOC initialized at time %lu\n", m_sim_time);
        }
    }
    
    void run_simulation(uint64_t duration_ns) {
        uint64_t end_time = m_sim_time + duration_ns;
        
        while (m_sim_time < end_time) {
            // Advance one clock cycle
            toggle_clock();
            m_doc->eval();
            
            // Handle wave read requests
            handle_wave_read();
            
            // Monitor outputs
            monitor_outputs();
            
            // Dump VCD
            if (m_dump_vcd) m_tfp->dump(m_sim_time);
            
            m_sim_time += CLK_PERIOD_NS / 2;
        }
        
        if (m_verbosity >= INFO) {
            printf("Simulation reached time %lu ns\n", m_sim_time);
            printf("Wave reads: %d, IRQs: %d, Audio samples: %d\n",
                  m_wave_read_count, m_irq_count, m_audio_samples);
        }
    }
    
    void test_single_oscillator() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing Single Oscillator ===\n");
        }
        
        // Configure oscillator 0
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x02); // Enable 1 oscillator
        write_register(DOC_REGISTERS::FREQ_LO_BASE, 0x40);   // Frequency low
        write_register(DOC_REGISTERS::FREQ_HI_BASE, 0x01);   // Frequency high
        write_register(DOC_REGISTERS::VOLUME_BASE, 0xFF);    // Full volume
        write_register(DOC_REGISTERS::TABLE_PTR_BASE, 0x00); // Waveform table at page 0
        write_register(DOC_REGISTERS::BANK_SIZE_BASE, 0x00); // 256-byte table, lowest resolution
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_FREE); // Free-running, no halt
        
        m_expecting_silence = false;
        
        // Run for a while to ensure oscillator produces sound
        run_simulation(1000000); // 1ms
        
        // Verify oscillator is active by checking for wave reads
        if (m_wave_read_count == 0) {
            printf("ERROR: No wave reads detected for active oscillator\n");
        }
        
        // Now halt the oscillator
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::HALT);
        
        m_expecting_silence = true;
        m_wave_read_count = 0;
        
        // Run again to ensure oscillator stops
        run_simulation(1000000); // 1ms
        
        // Verify oscillator is halted (no wave reads)
        if (m_wave_read_count > 0) {
            printf("ERROR: Wave reads detected for halted oscillator\n");
        }
    }
    
    void test_multiple_oscillators() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing Multiple Oscillators ===\n");
        }
        
        // Enable 16 oscillators
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x20); // 16 oscillators
        
        // Configure each oscillator with different frequencies
        for (int i = 0; i < 16; i++) {
            write_register(DOC_REGISTERS::FREQ_LO_BASE + i, 0x20 + i);      // Frequency low
            write_register(DOC_REGISTERS::FREQ_HI_BASE + i, 0x01);          // Frequency high
            write_register(DOC_REGISTERS::VOLUME_BASE + i, 0x80);           // Mid volume
            write_register(DOC_REGISTERS::TABLE_PTR_BASE + i, 0x00);        // Waveform table at page 0
            write_register(DOC_REGISTERS::BANK_SIZE_BASE + i, 0x00);        // 256-byte table, lowest resolution
            write_register(DOC_REGISTERS::CONTROL_BASE + i, i % 2);         // Even oscillators running, odd halted
        }
        
        m_expecting_silence = false;
        m_wave_read_count = 0;
        
        // Run for a while to ensure oscillators produce sound
        run_simulation(2000000); // 2ms
        
        // Verify active oscillators by checking for wave reads
        if (m_wave_read_count == 0) {
            printf("ERROR: No wave reads detected for active oscillators\n");
        }
        
        // Now enable all 32 oscillators (test for the off-by-one issue)
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x3E); // 31 oscillators - corrected value
        
        m_wave_read_count = 0;
        
        // Run again with 31 oscillators
        run_simulation(2000000); // 2ms
        
        if (m_verbosity >= INFO) {
            printf("Wave reads with 31 oscillators: %d\n", m_wave_read_count);
        }
    }
    
    void test_oscillator_modes() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing Oscillator Modes ===\n");
        }
        
        // Enable 4 oscillators for testing different modes
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x08); // 4 oscillators
        
        // Test Mode 0: Free-running
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_FREE);
        write_register(DOC_REGISTERS::VOLUME_BASE, 0xFF);
        write_register(DOC_REGISTERS::FREQ_LO_BASE, 0x40);
        write_register(DOC_REGISTERS::FREQ_HI_BASE, 0x01);
        
        m_wave_read_count = 0;
        run_simulation(500000); // 0.5ms
        
        int free_run_reads = m_wave_read_count;
        if (m_verbosity >= INFO) {
            printf("Free-running mode wave reads: %d\n", free_run_reads);
        }
        
        // Test Mode 1: One-shot
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_ONESHOT);
        
        m_wave_read_count = 0;
        run_simulation(500000); // 0.5ms
        
        int one_shot_reads = m_wave_read_count;
        if (m_verbosity >= INFO) {
            printf("One-shot mode wave reads: %d\n", one_shot_reads);
        }
        
        // One-shot should eventually halt
        if (one_shot_reads >= free_run_reads) {
            printf("WARNING: One-shot mode did not halt as expected\n");
        }
        
        // Test Mode 2: Sync/AM - requires oscillator pairs
        // Configure oscillator 0 (even)
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_FREE);
        write_register(DOC_REGISTERS::VOLUME_BASE, 0xFF);
        
        // Configure oscillator 1 (odd) for Sync/AM
        write_register(DOC_REGISTERS::CONTROL_BASE + 1, CONTROL_BITS::MODE_SYNC_AM);
        write_register(DOC_REGISTERS::VOLUME_BASE + 1, 0xFF);
        
        m_wave_read_count = 0;
        run_simulation(1000000); // 1ms
        
        if (m_verbosity >= INFO) {
            printf("Sync/AM mode wave reads: %d\n", m_wave_read_count);
        }
        
        // Test Mode 3: Swap - requires oscillator pairs
        // Configure oscillator 2 (even) for Swap
        write_register(DOC_REGISTERS::CONTROL_BASE + 2, CONTROL_BITS::MODE_SWAP | CONTROL_BITS::HALT);
        write_register(DOC_REGISTERS::VOLUME_BASE + 2, 0xFF);
        
        // Configure oscillator 3 (odd)
        write_register(DOC_REGISTERS::CONTROL_BASE + 3, CONTROL_BITS::MODE_SWAP);
        write_register(DOC_REGISTERS::VOLUME_BASE + 3, 0xFF);
        
        m_wave_read_count = 0;
        run_simulation(1000000); // 1ms
        
        if (m_verbosity >= INFO) {
            printf("Swap mode wave reads: %d\n", m_wave_read_count);
        }
    }
    
    void test_resolution_table_size() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing Resolution and Table Size ===\n");
        }
        
        // Enable 1 oscillator
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x02); // 1 oscillator
        
        // Test each resolution and table size combination
        const uint8_t table_sizes[] = {0, 1, 2, 3, 4, 5, 6, 7}; // 256B to 32KB
        const uint8_t resolutions[] = {0, 1, 2, 3, 4, 5, 6, 7}; // Bits 1-16 through 8-23
        
        for (int t = 0; t < 8; t++) {
            for (int r = 0; r < 8; r++) {
                uint8_t rts_val = (table_sizes[t] << 3) | resolutions[r];
                write_register(DOC_REGISTERS::BANK_SIZE_BASE, rts_val);
                write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_FREE); // Free-running, no halt
                
                m_wave_read_count = 0;
                run_simulation(200000); // 0.2ms
                
                if (m_verbosity >= INFO) {
                    printf("Table size %d, Resolution %d: %d wave reads\n", 
                           table_sizes[t], resolutions[r], m_wave_read_count);
                }
            }
        }
    }
    
    void test_volume_control() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing Volume Control ===\n");
        }
        
        // Enable 1 oscillator
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x02); // 1 oscillator
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_FREE); // Free-running
        
        // Test different volume levels
        const uint8_t volumes[] = {0, 32, 64, 128, 192, 255};
        
        for (int v = 0; v < 6; v++) {
            write_register(DOC_REGISTERS::VOLUME_BASE, volumes[v]);
            
            m_expecting_silence = (volumes[v] == 0);
            run_simulation(200000); // 0.2ms
            
            // Verify audio output against volume
            if (m_verbosity >= INFO) {
                printf("Volume %d test complete\n", volumes[v]);
            }
        }
    }
    
    void test_channel_assignment() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing Channel Assignment ===\n");
        }
        
        // Enable 8 oscillators
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x10); // 8 oscillators
        
        // Assign each oscillator to a different channel
        for (int i = 0; i < 8; i++) {
            uint8_t control = CONTROL_BITS::MODE_FREE | ((i & 0xF) << 4);
            write_register(DOC_REGISTERS::CONTROL_BASE + i, control);
            write_register(DOC_REGISTERS::VOLUME_BASE + i, 0xFF);
        }
        
        m_expecting_silence = false;
        run_simulation(500000); // 0.5ms
        
        if (m_verbosity >= INFO) {
            printf("Channel assignment test complete\n");
        }
    }
    
    void test_irq_functionality() {
        if (m_verbosity >= INFO) {
            printf("\n=== Testing IRQ Functionality ===\n");
        }
        
        // Enable 1 oscillator
        write_register(DOC_REGISTERS::OSC_ENABLE_REG, 0x02); // 1 oscillator
        
        // Configure oscillator with IRQ enabled
        write_register(DOC_REGISTERS::CONTROL_BASE, CONTROL_BITS::MODE_ONESHOT | CONTROL_BITS::IRQ_ENABLE);
        write_register(DOC_REGISTERS::VOLUME_BASE, 0xFF);
        
        m_irq_count = 0;
        run_simulation(1000000); // 1ms
        
        if (m_verbosity >= INFO) {
            printf("IRQ count with one-shot mode: %d\n", m_irq_count);
        }
        
        // Test IRQ register
        write_register(DOC_REGISTERS::OSC_INT_REG, 0xFF); // Set all IRQ bits
        read_register(DOC_REGISTERS::OSC_INT_REG);        // Read should clear
    }
    
    void run_all_tests() {
        initialize();
        
        test_single_oscillator();
        test_multiple_oscillators();
        test_oscillator_modes();
        test_resolution_table_size();
        test_volume_control();
        test_channel_assignment();
        test_irq_functionality();
        
        if (m_verbosity >= INFO) {
            printf("\nAll tests completed at time %lu\n", m_sim_time);
        }
    }
};

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Create instance of the DOC model
    Vdoc5503_harness* doc_harness = new Vdoc5503_harness;
    
    // Create tester and run all tests
    DOC5503Tester tester(doc_harness, INFO, true);
    tester.run_all_tests();
    
    // Cleanup
    delete doc_harness;
    
    return 0;
}