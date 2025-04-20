#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
// Use explicit paths for Verilator includes
#include "/opt/homebrew/Cellar/verilator/5.034/share/verilator/include/verilated.h"
#include "/opt/homebrew/Cellar/verilator/5.034/share/verilator/include/verilated_vcd_c.h"
#include "obj_dir/Vdoc5503_harness.h"

// Required time stamp function for Verilator
double sc_time_stamp() { return 0; }

// WAV file output functions - borrowed from doc_audio_test.cpp
void write_wav_file(const char* filename, int16_t* samples, size_t num_samples, int sample_rate) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("ERROR: Could not open %s for writing\n", filename);
        return;
    }
    
    // WAV header
    const int header_size = 44;
    unsigned char header[header_size] = {
        'R', 'I', 'F', 'F',                                  // RIFF chunk
        0, 0, 0, 0,                                          // Chunk size (filled later)
        'W', 'A', 'V', 'E',                                  // Format
        'f', 'm', 't', ' ',                                  // Format chunk
        16, 0, 0, 0,                                         // Format chunk size
        1, 0,                                                // PCM format
        1, 0,                                                // Mono channel
        0, 0, 0, 0,                                          // Sample rate (filled later)
        0, 0, 0, 0,                                          // Byte rate (filled later)
        2, 0,                                                // Block align (2 bytes per sample)
        16, 0,                                               // Bits per sample
        'd', 'a', 't', 'a',                                  // Data chunk
        0, 0, 0, 0                                           // Data size (filled later)
    };
    
    // Fill in the sample rate
    header[24] = sample_rate & 0xff;
    header[25] = (sample_rate >> 8) & 0xff;
    header[26] = (sample_rate >> 16) & 0xff;
    header[27] = (sample_rate >> 24) & 0xff;
    
    // Fill in the byte rate (sample rate * block align)
    int byte_rate = sample_rate * 2; // 2 bytes per sample
    header[28] = byte_rate & 0xff;
    header[29] = (byte_rate >> 8) & 0xff;
    header[30] = (byte_rate >> 16) & 0xff;
    header[31] = (byte_rate >> 24) & 0xff;
    
    // Fill in the data size
    uint32_t data_size = num_samples * 2; // 2 bytes per sample
    header[40] = data_size & 0xff;
    header[41] = (data_size >> 8) & 0xff;
    header[42] = (data_size >> 16) & 0xff;
    header[43] = (data_size >> 24) & 0xff;
    
    // Fill in the chunk size
    uint32_t chunk_size = 36 + data_size; // Header size - 8 + data size
    header[4] = chunk_size & 0xff;
    header[5] = (chunk_size >> 8) & 0xff;
    header[6] = (chunk_size >> 16) & 0xff;
    header[7] = (chunk_size >> 24) & 0xff;
    
    // Write header
    fwrite(header, 1, header_size, f);
    
    // Write data
    fwrite(samples, 2, num_samples, f);
    
    fclose(f);
    printf("Wrote %zu samples to %s\n", num_samples, filename);
}

int main(int argc, char** argv) {
    // Initialize Verilated library
    Verilated::commandArgs(argc, argv);
    
    // Load wavetable file
    FILE* wave_file = fopen("frontleft.wavetable", "rb");
    if (!wave_file) {
        printf("ERROR: Could not open frontleft.wavetable\n");
        return 1;
    }
    
    // Read the wave data
    uint8_t wave_data[32768]; // Max DOC wave table size
    size_t bytes_read = fread(wave_data, 1, sizeof(wave_data), wave_file);
    fclose(wave_file);
    printf("Loaded %ld bytes of wave data\n", bytes_read);
    
    // Create an array to hold the processed audio
    int16_t* output_samples = (int16_t*)malloc(bytes_read * sizeof(int16_t));
    if (!output_samples) {
        printf("ERROR: Failed to allocate memory for output samples\n");
        return 1;
    }
    memset(output_samples, 0, bytes_read * sizeof(int16_t));
    
    // Create instance of the top-level model
    Vdoc5503_harness* doc_harness = new Vdoc5503_harness;
    
    // Setup for tracing if needed
    VerilatedVcdC* tfp = nullptr;
    bool dump_trace = false;
    
    if (dump_trace) {
        Verilated::traceEverOn(true);
        tfp = new VerilatedVcdC;
        doc_harness->trace(tfp, 99);
        tfp->open("doc_wavetable_test.vcd");
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
        if (dump_trace) tfp->dump(i);
    }
    
    // Release reset
    doc_harness->reset_n_i = 1;
    
    // Setup DOC registers for wavetable playback
    const uint8_t oscillator_num = 0;
    const uint8_t addr_freq_low = oscillator_num;             // Frequency Low (0x00-0x1F)
    const uint8_t addr_freq_high = 0x20 + oscillator_num;     // Frequency High (0x20-0x3F)
    const uint8_t addr_volume = 0x40 + oscillator_num;        // Volume (0x40-0x5F)
    const uint8_t addr_wave_ptr = 0x80 + oscillator_num;      // Waveform table pointer (0x80-0x9F)
    const uint8_t addr_control = 0xA0 + oscillator_num;       // Control (0xA0-0xBF)
    const uint8_t addr_res_table = 0xC0 + oscillator_num;     // Resolution/Table Size (0xC0-0xDF)
    const uint8_t addr_osc_enable = 0xE1;                     // Oscillator enable register (0xE1)
    
    // First write cycle - register setup
    printf("Setting up DOC5503 registers...\n");
    
    // We'll use these settings:
    // - Frequency = 0x0800 (medium frequency for clear audio)
    // - Volume = 0xFF (maximum)
    // - Wave pointer = 0 (start of table)
    // - Control = 0x10 (free running mode, channel = 0 (mono))
    // - Res/Table Size = 0x00 (table size = 0 (256 bytes), resolution = 0)
    
    typedef struct {
        uint8_t addr;
        uint8_t data;
        const char* desc;
    } RegWrite;
    
    RegWrite reg_writes[] = {
        {addr_freq_low, 0x00, "FL=0x00 (medium frequency)"},
        {addr_freq_high, 0x08, "FH=0x08 (medium frequency)"},
        {addr_volume, 0xFF, "VOL=0xFF (maximum volume)"},
        {addr_wave_ptr, 0x00, "WTP=0x00 (start at page 0)"},
        {addr_control, 0x10, "CTL=0x10 (free run, channel 0 (mono))"},
        {addr_res_table, 0x00, "RTS=0x00 (256-byte table, res=0)"},
        {addr_osc_enable, 0x02, "OSC_EN=0x02 (enable oscillator 0)"}
    };
    
    // Setup oscillator registers
    int sim_time = 10; // Start after reset
    
    for (size_t i = 0; i < sizeof(reg_writes)/sizeof(reg_writes[0]); i++) {
        // Rising edge
        clk = true;
        doc_harness->clk_i = clk;
        doc_harness->cs_n_i = 0;
        doc_harness->we_n_i = 0;
        doc_harness->addr_i = reg_writes[i].addr;
        doc_harness->data_i = reg_writes[i].data;
        doc_harness->eval();
        if (dump_trace) tfp->dump(sim_time++);
        
        // Falling edge
        clk = false;
        doc_harness->clk_i = clk;
        doc_harness->eval();
        if (dump_trace) tfp->dump(sim_time++);
        
        printf("Writing %s\n", reg_writes[i].desc);
    }
    
    // Done with register setup
    doc_harness->cs_n_i = 1;
    doc_harness->we_n_i = 1;
    
    // Run the DOC for enough cycles to process the entire wavetable
    // We'll run it for at least as many cycles as there are bytes in the wavetable
    printf("Processing wavetable through DOC5503...\n");
    
    size_t current_wave_addr = 0;
    size_t sample_index = 0;
    const int max_cycles = bytes_read * 20; // Extra cycles to ensure processing completes
    bool doc_clk_en = false;
    
    for (int cycle = 0; cycle < max_cycles; cycle++) {
        // Toggle clock
        clk = !clk;
        doc_harness->clk_i = clk;
        
        if (clk) {
            // Generate DOC clk_en_i pulse every 8 cycles to simulate 7.15909MHz/8
            doc_clk_en = ((cycle % 8) == 0);
            doc_harness->clk_en_i = doc_clk_en;
            
            // When the DOC requests wave data, provide it from our wavetable
            if (doc_harness->wave_rd_o) {
                // Calculate index into wavetable based on wave address
                // Mask the lower bits of the address based on table size (8K = 0x1FFF mask)
                current_wave_addr = doc_harness->wave_address_o & 0x1FFF;
                
                // Provide a constant test wave pattern rather than from the wavetable
                // 0xC0 is a positive value (0x80 is zero)
                doc_harness->wave_data_i = 0xC0;
                
                doc_harness->wave_data_ready_i = 1;
                
                // Debug: Print every wave read request to see if the DOC is reading data
                printf("Wave rd at cycle %d: addr=0x%04x, providing data=0xC0\n", 
                    cycle, 
                    (unsigned int)doc_harness->wave_address_o);
            } else {
                doc_harness->wave_data_ready_i = 0;
            }
            
            // Debug: Print oscillator output periodically
            if (doc_clk_en && cycle % 500 == 0) {
                printf("Cycle: %d, left_mix: %d, right_mix: %d, mono_mix: %d\n",
                    cycle,
                    doc_harness->left_mix_o,
                    doc_harness->right_mix_o,
                    doc_harness->mono_mix_o);
            }
            
            // Capture the output every 8 cycles (on DOC clock) if we've processed at least some data
            if (doc_clk_en && cycle > 200 && sample_index < bytes_read) {
                // Capture mono mix output (we set channel 0 (mono) in the control register)
                output_samples[sample_index++] = doc_harness->mono_mix_o;
                
                if (sample_index % 1000 == 0) {
                    printf("Captured %lu samples, current value: %d\n", 
                        (unsigned long)sample_index, 
                        doc_harness->mono_mix_o);
                }
                
                // Stop when we've captured enough samples
                if (sample_index >= bytes_read) {
                    printf("Finished capturing %lu samples\n", (unsigned long)sample_index);
                    break;
                }
            }
        }
        
        // Evaluate model
        doc_harness->eval();
        
        // Write trace if enabled
        if (dump_trace) tfp->dump(sim_time++);
    }
    
    // Write output to WAV file at 22050Hz sample rate
    // This matches what the IIgs would typically use
    printf("Writing output to WAV file...\n");
    write_wav_file("doc_wavetable_output.wav", output_samples, sample_index, 22050);
    
    // Print some simple statistics about the output
    int16_t min_val = 32767;
    int16_t max_val = -32768;
    double avg_val = 0.0;
    double rms_val = 0.0;
    int clipping_count = 0;
    
    for (size_t i = 0; i < sample_index; i++) {
        int16_t sample = output_samples[i];
        if (sample < min_val) min_val = sample;
        if (sample > max_val) max_val = sample;
        avg_val += (double)sample / sample_index;
        
        if (sample == 32767 || sample == -32768) {
            clipping_count++;
        }
    }
    
    for (size_t i = 0; i < sample_index; i++) {
        double deviation = output_samples[i] - avg_val;
        rms_val += deviation * deviation;
    }
    rms_val = sqrt(rms_val / sample_index);
    
    printf("\nOutput statistics:\n");
    printf("Samples: %lu\n", (unsigned long)sample_index);
    printf("Min: %d, Max: %d (range: %d)\n", min_val, max_val, max_val - min_val);
    printf("Average: %.2f\n", avg_val);
    printf("RMS: %.2f\n", rms_val);
    printf("Clipping: %d samples (%.2f%%)\n", 
        clipping_count, 
        (clipping_count * 100.0) / sample_index);
    
    // Cleanup
    if (dump_trace) {
        tfp->close();
        delete tfp;
    }
    
    delete doc_harness;
    free(output_samples);
    
    printf("\nTest complete. You can now load doc_wavetable_output.wav in Audacity or any audio player.\n");
    
    return 0;
}