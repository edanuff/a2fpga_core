#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "es5503_standalone.h"

// Generate a WAV file from audio samples
void write_wav_file(const char* filename, int16_t* samples, size_t num_samples, int num_channels, int sample_rate) {
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
        0, 0,                                                // Channels (filled later)
        0, 0, 0, 0,                                          // Sample rate (filled later)
        0, 0, 0, 0,                                          // Byte rate (filled later)
        0, 0,                                                // Block align (filled later)
        16, 0,                                               // Bits per sample
        'd', 'a', 't', 'a',                                  // Data chunk
        0, 0, 0, 0                                           // Data size (filled later)
    };
    
    // Fill in the channels
    header[22] = num_channels & 0xff;
    header[23] = (num_channels >> 8) & 0xff;
    
    // Fill in the sample rate
    header[24] = sample_rate & 0xff;
    header[25] = (sample_rate >> 8) & 0xff;
    header[26] = (sample_rate >> 16) & 0xff;
    header[27] = (sample_rate >> 24) & 0xff;
    
    // Fill in the byte rate (sample rate * block align)
    int byte_rate = sample_rate * num_channels * 2; // 2 bytes per sample
    header[28] = byte_rate & 0xff;
    header[29] = (byte_rate >> 8) & 0xff;
    header[30] = (byte_rate >> 16) & 0xff;
    header[31] = (byte_rate >> 24) & 0xff;
    
    // Fill in the block align (bytes per sample * channels)
    int block_align = num_channels * 2;
    header[32] = block_align & 0xff;
    header[33] = (block_align >> 8) & 0xff;
    
    // Fill in the data size
    uint32_t data_size = num_samples * num_channels * 2; // 2 bytes per sample
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
    fwrite(samples, 2, num_samples * num_channels, f);
    
    fclose(f);
    printf("Wrote %zu samples (%d channels) to %s\n", num_samples, num_channels, filename);
}

// Set up an oscillator with the specified parameters
void setup_oscillator(ES5503* es5503, int osc_num, uint16_t freq, uint8_t vol, 
                     uint8_t wtp, uint8_t control, uint8_t rts) {
    // Calculate register addresses
    uint16_t fl_addr = 0x00 | (osc_num & 0x1F);  // Frequency Low
    uint16_t fh_addr = 0x20 | (osc_num & 0x1F);  // Frequency High
    uint16_t vol_addr = 0x40 | (osc_num & 0x1F); // Volume
    uint16_t wtp_addr = 0x80 | (osc_num & 0x1F); // Wavetable Pointer
    uint16_t ctl_addr = 0xA0 | (osc_num & 0x1F); // Control
    uint16_t rts_addr = 0xC0 | (osc_num & 0x1F); // Resolution/Table Size
    
    // Write to the registers
    es5503->write(fl_addr, freq & 0xFF);         // Frequency Low
    es5503->write(fh_addr, (freq >> 8) & 0xFF);  // Frequency High
    es5503->write(vol_addr, vol);                // Volume
    es5503->write(wtp_addr, wtp);                // Wavetable Pointer
    es5503->write(ctl_addr, control);            // Control
    es5503->write(rts_addr, rts);                // Resolution/Table Size
    
    printf("Set up oscillator %d: Freq=0x%04X, Vol=0x%02X, WTP=0x%02X, Ctrl=0x%02X, RTS=0x%02X\n",
           osc_num, freq, vol, wtp, control, rts);
}

// Main test function
int main(int argc, char** argv) {
    std::cout << "ES5503 Standalone Test" << std::endl;
    std::cout << "======================" << std::endl << std::endl;
    
    // Test parameters
    const int wavetable_size = 32768;
    const int sample_rate = 22050;
    const int output_duration_sec = 3;
    const int output_samples = sample_rate * output_duration_sec;
    const int channels = 2;
    const int buffer_size = 1024;
    
    // Allocate memory for wavetable
    uint8_t* wavetable = new uint8_t[wavetable_size];
    
    // Generate a simple triangle wave for testing
    std::cout << "Generating test waveforms..." << std::endl;
    
    // Initialize the wavetable with a triangle wave at the beginning
    for (int i = 0; i < 256; i++) {
        wavetable[i] = 0x80 + (i < 128 ? i : 255 - i);
    }
    
    // Add a simple sine wave after the triangle
    for (int i = 0; i < 256; i++) {
        wavetable[256 + i] = 0x80 + (int)(127.0 * sin(i * 2.0 * M_PI / 256.0));
    }
    
    // Add a square wave
    for (int i = 0; i < 256; i++) {
        wavetable[512 + i] = (i < 128) ? 0xFF : 0x01;
    }
    
    // Add a sawtooth wave
    for (int i = 0; i < 256; i++) {
        wavetable[768 + i] = 0x80 + i / 2;
    }
    
    // Fill the rest with silence
    for (int i = 1024; i < wavetable_size; i++) {
        wavetable[i] = 0x80;
    }
    
    // Add a HALT (0x00) at the end of each waveform
    wavetable[255] = 0x00;
    wavetable[511] = 0x00;
    wavetable[767] = 0x00;
    wavetable[1023] = 0x00;
    
    // Try to load the frontleft.wavetable if available
    FILE* frontleft_file = fopen("frontleft.wavetable", "rb");
    if (frontleft_file) {
        size_t bytes_read = fread(wavetable, 1, wavetable_size, frontleft_file);
        fclose(frontleft_file);
        std::cout << "Loaded " << bytes_read << " bytes from frontleft.wavetable" << std::endl;
    } else {
        std::cout << "Could not load frontleft.wavetable, using generated waves" << std::endl;
    }
    
    // Create a standalone ES5503 instance
    ES5503* es5503 = new ES5503(7159090, wavetable, wavetable_size);
    es5503->set_channels(channels);
    es5503->reset();
    
    // Allocate buffers for audio output
    int16_t* output_buffer = new int16_t[output_samples * channels];
    int16_t* temp_buffer = new int16_t[buffer_size * channels];
    
    std::cout << "Testing different oscillator configurations..." << std::endl;
    
    // Test 1: Basic triangle wave
    {
        std::cout << "Test 1: Basic triangle wave (Free Run Mode)" << std::endl;
        es5503->reset();
        
        // Set up oscillator 0 to play the triangle wave
        setup_oscillator(es5503, 0, 0x0120, 0x7F, 0x00, 0x10, 0x00);
        
        // Enable 1 oscillator
        es5503->write(0xE1, 0x02);
        
        // Generate audio
        for (int i = 0; i < output_samples; i += buffer_size) {
            int samples_to_generate = std::min(buffer_size, output_samples - i);
            es5503->generate_audio(temp_buffer, samples_to_generate);
            
            // Copy to output buffer
            memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
        }
        
        // Write to WAV file
        write_wav_file("es5503_triangle_free.wav", output_buffer, output_samples, channels, sample_rate);
    }
    
    // Test 2: Sine wave with One Shot mode
    {
        std::cout << "Test 2: Sine wave (One Shot Mode)" << std::endl;
        es5503->reset();
        
        // Set up oscillator 0 to play the sine wave in one-shot mode
        // Control register: 0x12 = One Shot mode (bit 1), channel 1 (bits 4-7 = 0001)
        setup_oscillator(es5503, 0, 0x0120, 0x7F, 0x01, 0x12, 0x00);
        
        // Enable 1 oscillator
        es5503->write(0xE1, 0x02);
        
        // Generate audio
        for (int i = 0; i < output_samples; i += buffer_size) {
            int samples_to_generate = std::min(buffer_size, output_samples - i);
            es5503->generate_audio(temp_buffer, samples_to_generate);
            
            // Copy to output buffer
            memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
        }
        
        // Write to WAV file
        write_wav_file("es5503_sine_oneshot.wav", output_buffer, output_samples, channels, sample_rate);
    }
    
    // Test 3: Swap mode between oscillators
    {
        std::cout << "Test 3: Swap mode between square and sawtooth" << std::endl;
        es5503->reset();
        
        // Set up oscillator 0 (square wave) in swap mode
        // Control register: 0x16 = Swap mode (bits 1-2 = 11), channel 1 (bits 4-7 = 0001)
        setup_oscillator(es5503, 0, 0x0120, 0x7F, 0x02, 0x16, 0x00);
        
        // Set up oscillator 1 (sawtooth wave) in swap mode
        // Control register: 0x16 = Swap mode (bits 1-2 = 11), channel 1 (bits 4-7 = 0001)
        setup_oscillator(es5503, 1, 0x0120, 0x7F, 0x03, 0x16, 0x00);
        
        // Halt oscillator 1 initially
        es5503->write(0xA1, 0x17);  // Control with halt bit set
        
        // Enable 2 oscillators
        es5503->write(0xE1, 0x04);
        
        // Generate audio
        for (int i = 0; i < output_samples; i += buffer_size) {
            int samples_to_generate = std::min(buffer_size, output_samples - i);
            es5503->generate_audio(temp_buffer, samples_to_generate);
            
            // Copy to output buffer
            memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
        }
        
        // Write to WAV file
        write_wav_file("es5503_swap_mode.wav", output_buffer, output_samples, channels, sample_rate);
    }
    
    // Test 4: Different frequency values
    {
        std::cout << "Test 4: Different frequency values" << std::endl;
        
        const uint16_t test_freqs[] = {0x0080, 0x0100, 0x0180, 0x0200, 0x0300};
        const int num_freqs = sizeof(test_freqs) / sizeof(test_freqs[0]);
        
        for (int f = 0; f < num_freqs; f++) {
            uint16_t freq = test_freqs[f];
            std::cout << "  Testing frequency 0x" << std::hex << freq << std::dec << std::endl;
            
            es5503->reset();
            
            // Set up oscillator 0 to play the triangle wave
            setup_oscillator(es5503, 0, freq, 0x7F, 0x00, 0x10, 0x00);
            
            // Enable 1 oscillator
            es5503->write(0xE1, 0x02);
            
            // Generate audio
            for (int i = 0; i < output_samples; i += buffer_size) {
                int samples_to_generate = std::min(buffer_size, output_samples - i);
                es5503->generate_audio(temp_buffer, samples_to_generate);
                
                // Copy to output buffer
                memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
            }
            
            // Write to WAV file
            char filename[100];
            sprintf(filename, "es5503_freq_%04X.wav", freq);
            write_wav_file(filename, output_buffer, output_samples, channels, sample_rate);
        }
    }
    
    // Test 5: Different volume values
    {
        std::cout << "Test 5: Different volume values" << std::endl;
        
        const uint8_t test_vols[] = {0x20, 0x40, 0x80, 0xC0, 0xFF};
        const int num_vols = sizeof(test_vols) / sizeof(test_vols[0]);
        
        for (int v = 0; v < num_vols; v++) {
            uint8_t vol = test_vols[v];
            std::cout << "  Testing volume 0x" << std::hex << (int)vol << std::dec << std::endl;
            
            es5503->reset();
            
            // Set up oscillator 0 to play the triangle wave
            setup_oscillator(es5503, 0, 0x0120, vol, 0x00, 0x10, 0x00);
            
            // Enable 1 oscillator
            es5503->write(0xE1, 0x02);
            
            // Generate audio
            for (int i = 0; i < output_samples; i += buffer_size) {
                int samples_to_generate = std::min(buffer_size, output_samples - i);
                es5503->generate_audio(temp_buffer, samples_to_generate);
                
                // Copy to output buffer
                memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
            }
            
            // Write to WAV file
            char filename[100];
            sprintf(filename, "es5503_vol_%02X.wav", vol);
            write_wav_file(filename, output_buffer, output_samples, channels, sample_rate);
        }
    }
    
    // Test 6: Different wavetable sizes and resolutions
    {
        std::cout << "Test 6: Different wavetable sizes and resolutions" << std::endl;
        
        // Test each table size with matching resolution
        for (int size = 0; size <= 7; size++) {
            uint8_t rts = (size << 3) | size;  // Set both table size and resolution
            std::cout << "  Testing table size/resolution " << size << " (RTS=0x" << std::hex << (int)rts << std::dec << ")" << std::endl;
            
            es5503->reset();
            
            // Set up oscillator 0 to play the triangle wave
            setup_oscillator(es5503, 0, 0x0120, 0x7F, 0x00, 0x10, rts);
            
            // Enable 1 oscillator
            es5503->write(0xE1, 0x02);
            
            // Generate audio
            for (int i = 0; i < output_samples; i += buffer_size) {
                int samples_to_generate = std::min(buffer_size, output_samples - i);
                es5503->generate_audio(temp_buffer, samples_to_generate);
                
                // Copy to output buffer
                memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
            }
            
            // Write to WAV file
            char filename[100];
            sprintf(filename, "es5503_tablesize_%d.wav", size);
            write_wav_file(filename, output_buffer, output_samples, channels, sample_rate);
        }
    }
    
    // Test 7: Frontleft.wavetable (if loaded)
    if (frontleft_file) {
        std::cout << "Test 7: Playing frontleft.wavetable" << std::endl;
        es5503->reset();
        
        // The frontleft.wavetable is a 32KB wavetable (size=7)
        // Known to work well with frequency 0x01AD, table size 7, resolution 0
        setup_oscillator(es5503, 0, 0x01AD, 0xFF, 0x00, 0x10, 0x38);
        
        // Enable 1 oscillator
        es5503->write(0xE1, 0x02);
        
        // Generate audio
        for (int i = 0; i < output_samples; i += buffer_size) {
            int samples_to_generate = std::min(buffer_size, output_samples - i);
            es5503->generate_audio(temp_buffer, samples_to_generate);
            
            // Copy to output buffer
            memcpy(output_buffer + (i * channels), temp_buffer, samples_to_generate * channels * sizeof(int16_t));
        }
        
        // Write to WAV file
        write_wav_file("es5503_frontleft.wav", output_buffer, output_samples, channels, sample_rate);
    }
    
    // Cleanup
    delete[] output_buffer;
    delete[] temp_buffer;
    delete[] wavetable;
    delete es5503;
    
    std::cout << std::endl << "Testing complete. WAV files have been generated for analysis." << std::endl;
    return 0;
}