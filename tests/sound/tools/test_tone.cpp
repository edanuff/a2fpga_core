#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// Generate a test square wave similar to what sound_glu.sv does
// This mimics the behavior of our 440Hz test tone

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

// Check DC offset of a WAV file
void check_dc_offset(const char* filename, int16_t* samples, size_t num_samples) {
    // Calculate average (DC offset)
    double sum = 0.0;
    for (size_t i = 0; i < num_samples; i++) {
        sum += samples[i];
    }
    double average = sum / num_samples;
    
    // Calculate RMS
    double sum_squared = 0.0;
    for (size_t i = 0; i < num_samples; i++) {
        double deviation = samples[i] - average;
        sum_squared += deviation * deviation;
    }
    double rms = sqrt(sum_squared / num_samples);
    
    // Calculate DC offset as percentage of full range
    double dc_offset_percentage = (average / 32768.0) * 100.0;
    
    printf("DC Offset Check for %s:\n", filename);
    printf("  Average sample value: %.2f\n", average);
    printf("  RMS value: %.2f\n", rms);
    printf("  DC offset percentage: %.4f%%\n", dc_offset_percentage);
    printf("  DC offset / RMS ratio: %.4f%%\n\n", (fabs(average) / rms) * 100.0);
}

int main() {
    // Create test file similar to the one we expect to see from the FPGA
    const int sample_rate = 48000;
    const int num_samples = sample_rate * 2; // 2 seconds
    
    // Allocate memory for samples
    int16_t* audio_samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    if (!audio_samples) {
        printf("Error: Unable to allocate memory for audio samples\n");
        return 1;
    }
    
    // Create a test tone similar to what we're implementing in sound_glu.sv
    // This tone will be a 440Hz square wave (A note)
    
    // Calculate samples needed per cycle for 440Hz at 48kHz sample rate
    const int samples_per_cycle = sample_rate / 440;
    
    // Create square waves with different amplitudes
    const int num_amplitudes = 4;
    const int amplitudes[num_amplitudes] = {4096, 2048, 1024, 512}; // 1/8, 1/16, 1/32, 1/64 of full scale
    
    for (int a = 0; a < num_amplitudes; a++) {
        int amplitude = amplitudes[a];
        
        // Create the square wave
        for (int i = 0; i < num_samples; i++) {
            if ((i % samples_per_cycle) < (samples_per_cycle / 2)) {
                audio_samples[i] = amplitude;      // Positive amplitude
            } else {
                audio_samples[i] = -amplitude;     // Negative amplitude
            }
        }
        
        // Create filename
        char filename[64];
        sprintf(filename, "test_tone_440hz_amp%d.wav", amplitude);
        
        // Write the wave file
        write_wav_file(filename, audio_samples, num_samples, sample_rate);
        
        // Check DC offset
        check_dc_offset(filename, audio_samples, num_samples);
    }
    
    // Create a sine wave version for comparison (smoother sound)
    for (int i = 0; i < num_samples; i++) {
        double angle = 2.0 * M_PI * 440.0 * i / sample_rate;
        audio_samples[i] = (int16_t)(4096.0 * sin(angle));
    }
    
    // Write sine wave
    write_wav_file("test_tone_440hz_sine.wav", audio_samples, num_samples, sample_rate);
    check_dc_offset("test_tone_440hz_sine.wav", audio_samples, num_samples);
    
    // Clean up
    free(audio_samples);
    
    printf("Test tone generation complete\n");
    return 0;
}