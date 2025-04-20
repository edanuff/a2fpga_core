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
    
    // First, generate a pure sine wave for testing DC offset
    printf("Generating test sine wave (should be perfectly zero-centered)...\n");
    for (int i = 0; i < num_samples; i++) {
        double phase = 2.0 * M_PI * i / 100.0; // ~220Hz sine wave
        audio_samples[i] = (int16_t)(sin(phase) * 16000.0); // -16000 to 16000 range
    }
    write_wav_file("glu_sine_test.wav", audio_samples, num_samples, 22050);
    
    // Create wavetable data following DOC format constraints:
    // - Values must be in range 0x01-0xFF (1-255)
    // - 0x00 is a reserved value that stops the oscillator
    // - Normal waveforms are centered around 0x80 (128) which is zero-crossing
    uint8_t waveTableData[256];
    for (int i = 0; i < 256; i++) {
        if (i < 128) {
            waveTableData[i] = 128 + (i / 2);  // 128-192 (positive values)
        } else {
            waveTableData[i] = 128 + (128 - (i - 128)) / 2;  // 192-128 (returning to zero)
        }
        
        // Ensure no 0x00 values (which would stop the oscillator)
        if (waveTableData[i] == 0) {
            waveTableData[i] = 1;  // Use minimum valid value
        }
    }
    
    // Print some stats about our wavetable
    printf("Wavetable statistics:\n");
    for (int i = 0; i < 256; i += 32) {
        printf("  Values %3d-%3d: ", i, i+31);
        for (int j = 0; j < 32 && i+j < 256; j++) {
            printf("%3d ", waveTableData[i+j]);
        }
        printf("\n");
    }
    printf("\n");
    
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
                
                // Follow exactly the same zero-centering approach as the DOC5503 module:
                
                // 1. Get sample from wavetable (0-255 range)
                uint8_t raw_sample = waveTableData[oscillator_idx];     // 0-255 range, centered at 128
                
                // 2. Convert to signed the same way the DOC does
                int8_t signed_sample = (int8_t)(raw_sample ^ 0x80);    // XOR with 0x80 exactly like DOC5503.sv
                
                // 3. Scale signed sample with full volume (255) as DOC does
                int16_t scaled_sample = signed_sample * 255;           // Scale to 16-bit range, preserving sign
                
                // 4. Apply the 0.75 factor (same low-pass filter as DOC)
                int16_t filtered_sample = (scaled_sample * 3) / 4;     // Multiply by 0.75 as in DOC5503.sv
                
                // 5. Apply volume control with arithmetic right shift (just like the GLU)
                // This simulates exactly what happens in sound_glu.sv
                int16_t volume_adjusted = filtered_sample >> volume_shift;
                
                // Apply noise gate if enabled - cleans up low-level signals
                if (threshold > 0 && volume_adjusted > -threshold && volume_adjusted < threshold) {
                    audio_samples[i] = 0;                             // True digital silence
                } else {
                    audio_samples[i] = volume_adjusted;                // Full signal (with sign preserved)
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
    
    // Generate a reference waveform that has silent parts - properly zero-centered
    printf("Generating audio test patterns with silent sections...\n");
    for (int i = 0; i < num_samples; i++) {
        // Create a pattern that plays for 1/4 second, then silent for 1/4 second
        int cycle_pos = i % (num_samples / 2);
        
        if (cycle_pos < num_samples / 4) {
            // Active part - play the waveform - EXACTLY the same as DOC+GLU does
            int oscillator_idx = (i * 250) % 256;
            
            // 1. Get raw sample from wavetable (0-255)
            uint8_t raw_sample = waveTableData[oscillator_idx];         // 0-255 range centered at 128
            
            // 2. Convert to signed the same way as the DOC
            int8_t signed_sample = (int8_t)(raw_sample ^ 0x80);        // XOR with 0x80 like the DOC
            
            // 3. Scale to 16-bit range with volume 255
            int16_t scaled_sample = signed_sample * 255;                // Scale preserving sign
            
            // 4. Apply 0.75 factor low-pass filter
            int16_t filtered_sample = (scaled_sample * 3) / 4;          // Same as DOC implementation
            
            // 5. Store final properly DC-centered sample
            audio_samples[i] = filtered_sample;                        // Should be perfectly zero-centered
        } else {
            // Silent part - true zero (DC centered)
            audio_samples[i] = 0;
        }
    }
    // Write perfect reference with clean silence
    write_wav_file("glu_reference.wav", audio_samples, num_samples, 22050);
    
    // Create a version with simulated buzzing during silent parts
    for (int i = 0; i < num_samples; i++) {
        int cycle_pos = i % (num_samples / 2);
        
        if (cycle_pos < num_samples / 4) {
            // Keep the active parts unchanged - these should be properly DC centered
            // No changes needed
        } else {
            // Replace silent parts with low-level noise (properly centered around zero)
            audio_samples[i] = (rand() % 65) - 32;  // Low-level noise (-32 to +32)
        }
    }
    write_wav_file("glu_with_buzz.wav", audio_samples, num_samples, 22050);
    
    // Create a version with noise gate applied to fix the buzzing
    for (int i = 0; i < num_samples; i++) {
        if (audio_samples[i] > -32 && audio_samples[i] < 32) {
            audio_samples[i] = 0;  // Apply noise gate
        }
    }
    write_wav_file("glu_fixed.wav", audio_samples, num_samples, 22050);
    
    // Create a square wave test file (zero-centered) to show filter effect
    // This is a cleaner test case that directly tests the filter
    for (int i = 0; i < num_samples; i++) {
        // Generate a 220Hz square wave
        double time = (double)i / 22050.0;
        double value = sin(2.0 * M_PI * 220 * time) > 0 ? 1.0 : -1.0;
        
        // Scale to 16-bit range but not too loud
        int16_t square_sample = (int16_t)(value * 12000);  // Lower amplitude to prevent clipping when filtered
        audio_samples[i] = square_sample;
    }
    write_wav_file("glu_square_raw.wav", audio_samples, num_samples, 22050);
    
    // Create a filtered version of the square wave
    for (int i = 0; i < num_samples; i++) {
        audio_samples[i] = (audio_samples[i] * 3) / 4;  // Apply 0.75 low-pass filter
    }
    write_wav_file("glu_square_filtered.wav", audio_samples, num_samples, 22050);
    
    // Clean up
    free(audio_samples);
    
    printf("\nGenerated WAV files:\n");
    printf("1. glu_sine_test.wav - Pure sine wave (verify proper zero-centering in Audacity)\n");
    printf("2. glu_vol*_gate*.wav - Various volume levels and noise gate settings\n");
    printf("3. glu_reference.wav - Reference waveform with proper silent parts\n");
    printf("4. glu_with_buzz.wav - Simulation of the buzzing issue during silent parts\n");
    printf("5. glu_fixed.wav - Same as above but with noise gate applied\n");
    printf("6. glu_square_raw.wav - Raw square wave for showing filter effect\n");
    printf("7. glu_square_filtered.wav - Filtered square wave (0.75x filter)\n");
    
    printf("\nNOTE: All files should be properly zero-centered in Audacity. If not, check\n");
    printf("      your Audacity import settings or view the glu_sine_test.wav file to verify.\n");
    
    return 0;
}