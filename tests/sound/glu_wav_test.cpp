#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

// WAV file generation function
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

// Simulate GLU audio processing with various volume levels
int main(int argc, char** argv) {
    printf("GLU Audio Test - Generating WAV files with different volume settings\n");
    printf("=================================================================\n\n");
    
    const size_t num_samples = 22050; // 1 second @ 22.05kHz
    int16_t* audio_samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    
    if (!audio_samples) {
        printf("ERROR: Could not allocate memory for audio samples\n");
        return 1;
    }
    
    // Create wavetable data (same as in glu_tb.cpp)
    uint8_t waveTableData[256];
    for (int i = 0; i < 256; i++) {
        if (i < 128) {
            waveTableData[i] = 128 + (i / 2);
        } else {
            waveTableData[i] = 128 + (128 - (i - 128)) / 2;
        }
    }
    
    // Generate audio samples using similar math to what's in the DOC5503 module
    // This simulates a complete path from DOC to GLU to audio output
    printf("Generating audio samples with various volume and noise gate settings...\n");
    
    // Test with different volume levels and noise gate thresholds
    const int num_volumes = 5;
    const int volumes[num_volumes] = {15, 10, 5, 2, 0}; // 15=max, 0=min
    
    const int num_thresholds = 3;
    const int thresholds[num_thresholds] = {0, 32, 128}; // 0=no gate, 128=strong gate
    
    for (int v = 0; v < num_volumes; v++) {
        int volume = volumes[v];
        for (int t = 0; t < num_thresholds; t++) {
            int threshold = thresholds[t];
            
            // Compute volume right shift amount (invert for right shift)
            int volume_shift = (volume == 15) ? 0 : (15 - volume) / 4;
            
            // Generate samples
            for (int i = 0; i < num_samples; i++) {
                // Simulate DOC oscillator playback
                int oscillator_idx = (i * 250) % 256;  // Similar to frequency setting in test
                int8_t sample_value = waveTableData[oscillator_idx] ^ 0x80;  // Convert to signed
                
                // Apply DOC volume (maximum = 255)
                int16_t doc_output = sample_value * 255;
                
                // Apply 0.75 factor (low-pass filter in DSP multiplier)
                int16_t filtered_output = (doc_output * 3) / 4;
                
                // Apply GLU volume control
                int16_t volume_adjusted = filtered_output >> volume_shift;
                
                // Apply noise gate if enabled
                if (threshold > 0 && volume_adjusted > -threshold && volume_adjusted < threshold) {
                    audio_samples[i] = 0;  // Silence
                } else {
                    audio_samples[i] = volume_adjusted;
                }
            }
            
            // Write WAV file
            char filename[100];
            snprintf(filename, sizeof(filename), "glu_vol%d_gate%d.wav", volume, threshold);
            write_wav_file(filename, audio_samples, num_samples, 22050);
        }
    }
    
    // Generate combined test that demonstrates the buzzing issue and fix
    printf("\nGenerating test files for buzzing issue...\n");
    
    // Generate a reference waveform that has silent parts
    for (int i = 0; i < num_samples; i++) {
        // Create a pattern that plays for 1/4 second, then silent for 1/4 second
        int cycle_pos = i % (num_samples / 2);
        
        if (cycle_pos < num_samples / 4) {
            // Active part - play the waveform
            int oscillator_idx = (i * 250) % 256;
            int8_t sample_value = waveTableData[oscillator_idx] ^ 0x80;
            audio_samples[i] = sample_value * 255;
        } else {
            // Silent part
            audio_samples[i] = 0;
        }
    }
    write_wav_file("glu_reference.wav", audio_samples, num_samples, 22050);
    
    // Now add simulated buzzing noise during silent parts
    for (int i = 0; i < num_samples; i++) {
        int cycle_pos = i % (num_samples / 2);
        
        if (cycle_pos < num_samples / 4) {
            // Active part - play the waveform
            int oscillator_idx = (i * 250) % 256;
            int8_t sample_value = waveTableData[oscillator_idx] ^ 0x80;
            audio_samples[i] = sample_value * 255;
        } else {
            // Silent part with noise
            audio_samples[i] = (rand() % 33) - 16;  // Low-level noise (-16 to +16)
        }
    }
    write_wav_file("glu_with_buzz.wav", audio_samples, num_samples, 22050);
    
    // Apply noise gate to fix buzzing
    for (int i = 0; i < num_samples; i++) {
        if (audio_samples[i] > -32 && audio_samples[i] < 32) {
            audio_samples[i] = 0;  // Apply noise gate
        }
    }
    write_wav_file("glu_fixed.wav", audio_samples, num_samples, 22050);
    
    // Clean up
    free(audio_samples);
    
    printf("\nGenerated WAV files:\n");
    printf("1. glu_vol*_gate*.wav - Various volume levels and noise gate settings\n");
    printf("2. glu_reference.wav - Reference waveform with proper silent parts\n");
    printf("3. glu_with_buzz.wav - Simulation of the buzzing issue during silent parts\n");
    printf("4. glu_fixed.wav - Same as above but with noise gate applied\n");
    
    return 0;
}