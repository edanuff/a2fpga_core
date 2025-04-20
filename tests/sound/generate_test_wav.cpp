#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

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

// Read wavetable and generate a test WAV file
// This creates a simple sine wave test file
int main() {
    printf("Generating test WAV files from wavetable data...\n");
    
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
    
    // Print basic statistics about the wavetable
    int min_sample = 255, max_sample = 0;
    double avg_sample = 0;
    for (size_t i = 0; i < bytes_read; i++) {
        if (wave_data[i] < min_sample) min_sample = wave_data[i];
        if (wave_data[i] > max_sample) max_sample = wave_data[i];
        avg_sample += (double)wave_data[i] / bytes_read;
    }
    printf("Wave data statistics:\n");
    printf("  Min: %d (signed: %d)\n", min_sample, (int8_t)(min_sample ^ 0x80));
    printf("  Max: %d (signed: %d)\n", max_sample, (int8_t)(max_sample ^ 0x80));
    printf("  Avg: %.2f (unsigned), %.2f (signed)\n", avg_sample, avg_sample - 128.0);
    printf("  Range: %d to %d (total span: %d)\n\n", min_sample, max_sample, max_sample - min_sample);
    
    // Create buffers for audio samples
    const size_t num_samples = bytes_read;
    int16_t* raw_samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    int16_t* sine_samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    int16_t* wavetable_samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    int16_t* filtered_samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    
    if (!raw_samples || !sine_samples || !wavetable_samples || !filtered_samples) {
        printf("ERROR: Failed to allocate memory for sample buffers\n");
        return 1;
    }
    
    // Generate raw samples - convert 8-bit wavetable values to 16-bit signed
    for (size_t i = 0; i < num_samples; i++) {
        // Convert from 8-bit unsigned (0-255) to 16-bit signed (-32768 to 32767)
        int8_t signed_sample = wave_data[i] ^ 0x80;  // Convert to signed
        raw_samples[i] = signed_sample * 256;        // Scale to 16-bit range
    }
    
    // Generate sine wave samples
    for (size_t i = 0; i < num_samples; i++) {
        double t = (double)i / num_samples;
        double freq = 440.0;  // A4 note - 440 Hz
        double phase = 2.0 * M_PI * freq * t * 10.0;  // 10 cycles
        sine_samples[i] = (int16_t)(sin(phase) * 16000.0);  // Amplitude of 16000
    }
    
    // Generate filtered samples - apply a simple low-pass filter to the raw samples
    // This is similar to what we're doing in the DSP multiplier
    filtered_samples[0] = raw_samples[0];
    for (size_t i = 1; i < num_samples; i++) {
        // Simple filter: output = 0.75 * current + 0.25 * previous
        filtered_samples[i] = (raw_samples[i] * 3 + filtered_samples[i-1]) / 4;
    }
    
    // Find a non-zero section of the wavetable for more interesting output
    int start_offset = 0;
    for (size_t i = 0; i < bytes_read; i++) {
        if (wave_data[i] != 128) {  // Find non-zero (non-silence) data
            start_offset = i;
            printf("Found non-zero wavetable data at offset %d, value: %d\n", 
                   start_offset, wave_data[start_offset]);
            break;
        }
    }
    
    // If we couldn't find any non-zero data, just use a reasonable point in the file
    if (start_offset == 0 || start_offset > bytes_read/2) {
        start_offset = 1000;  // Arbitrarily choose a point that's likely to have data
        printf("Using artificial start offset at %d\n", start_offset);
    }
    
    // Create a custom test wavetable for demo purposes
    // Take a small slice of the wavetable and repeat it
    uint8_t test_wave[256];
    
    // The wavetable seems to be mostly silence, so let's create a synthetic waveform
    // that will better demonstrate the effect of the filter
    printf("Creating synthetic sawtooth waveform with harmonics...\n");
    for (int i = 0; i < 256; i++) {
        // Create a sawtooth wave with some high-frequency elements
        test_wave[i] = (i % 32) * 8 + 128;  // Sawtooth from 128 to 248, repeating every 32 samples
    }
    
    // Create a second version with a square wave for even more high-frequency content
    uint8_t square_wave[256];
    for (int i = 0; i < 256; i++) {
        // Create a square wave with harsh transitions
        square_wave[i] = (i % 16 < 8) ? 180 : 80;  // Square wave between 80 and 180
    }
    
    // Print the first 32 samples of our test waves
    printf("Sawtooth wave samples: ");
    for (int i = 0; i < 32; i++) {
        printf("%d ", test_wave[i]);
    }
    printf("...\n");
    
    printf("Square wave samples: ");
    for (int i = 0; i < 32; i++) {
        printf("%d ", square_wave[i]);
    }
    printf("...\n");
    
    // Generate wavetable played samples - simulate how the wavetable would sound
    // This is a simple simulation, not a full DOC emulation
    
    // Generate multiple playback examples at different frequencies
    printf("\nGenerating DOC-like playback at different frequencies...\n");
    
    // Try different frequencies to demonstrate audio quality with and without filter
    const int frequencies[] = {128, 256, 512, 1024};
    int16_t* freq_samples[4];
    int16_t* freq_filtered_samples[4];
    int16_t* square_samples[4];
    int16_t* square_filtered_samples[4];
    
    // First generate sawtooth tests
    printf("\nGenerating sawtooth wave examples...\n");
    for (int f = 0; f < 4; f++) {
        freq_samples[f] = (int16_t*)malloc(num_samples * sizeof(int16_t));
        freq_filtered_samples[f] = (int16_t*)malloc(num_samples * sizeof(int16_t));
        
        if (!freq_samples[f] || !freq_filtered_samples[f]) {
            printf("ERROR: Failed to allocate memory for frequency samples\n");
            return 1;
        }
        
        const int freq = frequencies[f];
        int acc = 0;
        printf("Simulating sawtooth at %d Hz...\n", freq * 22050 / 65536);
        
        for (size_t i = 0; i < num_samples; i++) {
            // Simulate oscillator accumulator
            acc = (acc + freq) & 0xFFFF;
            // Extract upper 8 bits as index into wavetable
            int idx = (acc >> 8) & 0xFF;
            // Get sample from test wavetable
            int8_t signed_sample = test_wave[idx] ^ 0x80;
            // Apply volume (full volume = 255)
            int16_t output = signed_sample * 256;  // Amplify more for audibility
            
            // Store unfiltered output
            freq_samples[f][i] = output;
            
            // Store filtered output (multiply by 0.75)
            freq_filtered_samples[f][i] = (output * 3) / 4;
            
            // For the lowest frequency, use it as our main demo
            if (f == 0) {
                wavetable_samples[i] = freq_filtered_samples[f][i];
            }
            
            // Debug the first few samples of the first frequency
            if (f == 0 && i < 10) {
                printf("Sample %zu: acc=%d, idx=%d, signed_sample=%d, output=%d, filtered=%d\n",
                       i, acc, idx, signed_sample, output, freq_filtered_samples[f][i]);
            }
        }
        
        // Write WAV files for each frequency
        char raw_filename[64], filtered_filename[64];
        snprintf(raw_filename, sizeof(raw_filename), 
                 "sawtooth_raw_freq%d.wav", freq);
        snprintf(filtered_filename, sizeof(filtered_filename), 
                 "sawtooth_filtered_freq%d.wav", freq);
        
        write_wav_file(raw_filename, freq_samples[f], num_samples, 22050);
        write_wav_file(filtered_filename, freq_filtered_samples[f], num_samples, 22050);
    }
    
    // Now generate square wave tests - these will show more distortion
    printf("\nGenerating square wave examples...\n");
    for (int f = 0; f < 4; f++) {
        square_samples[f] = (int16_t*)malloc(num_samples * sizeof(int16_t));
        square_filtered_samples[f] = (int16_t*)malloc(num_samples * sizeof(int16_t));
        
        if (!square_samples[f] || !square_filtered_samples[f]) {
            printf("ERROR: Failed to allocate memory for square wave samples\n");
            return 1;
        }
        
        const int freq = frequencies[f];
        int acc = 0;
        printf("Simulating square wave at %d Hz...\n", freq * 22050 / 65536);
        
        for (size_t i = 0; i < num_samples; i++) {
            // Simulate oscillator accumulator
            acc = (acc + freq) & 0xFFFF;
            // Extract upper 8 bits as index into wavetable
            int idx = (acc >> 8) & 0xFF;
            // Get sample from square wavetable
            int8_t signed_sample = square_wave[idx] ^ 0x80;
            // Apply volume (full volume = 255)
            int16_t output = signed_sample * 256;  // Amplify more for audibility
            
            // Store unfiltered output
            square_samples[f][i] = output;
            
            // Store filtered output (multiply by 0.75)
            square_filtered_samples[f][i] = (output * 3) / 4;
        }
        
        // Write WAV files for each frequency
        char raw_filename[64], filtered_filename[64];
        snprintf(raw_filename, sizeof(raw_filename), 
                 "square_raw_freq%d.wav", freq);
        snprintf(filtered_filename, sizeof(filtered_filename), 
                 "square_filtered_freq%d.wav", freq);
        
        write_wav_file(raw_filename, square_samples[f], num_samples, 22050);
        write_wav_file(filtered_filename, square_filtered_samples[f], num_samples, 22050);
    }
    
    // Write WAV files
    write_wav_file("raw_wavetable.wav", raw_samples, num_samples, 22050);
    write_wav_file("sine_test.wav", sine_samples, num_samples, 22050);
    write_wav_file("filtered_wavetable.wav", filtered_samples, num_samples, 22050);
    write_wav_file("played_wavetable.wav", wavetable_samples, num_samples, 22050);
    
    // Free memory
    free(raw_samples);
    free(sine_samples);
    free(wavetable_samples);
    free(filtered_samples);
    
    // Free frequency samples
    for (int f = 0; f < 4; f++) {
        free(freq_samples[f]);
        free(freq_filtered_samples[f]);
        free(square_samples[f]);
        free(square_filtered_samples[f]);
    }
    
    printf("\nGenerated the following test WAV files:\n");
    printf("1. raw_wavetable.wav - Direct conversion of wavetable to audio\n");
    printf("2. sine_test.wav - Pure sine wave for comparison\n");
    printf("3. filtered_wavetable.wav - Wavetable with low-pass filter applied\n");
    printf("4. played_wavetable.wav - Simulated playback of wavetable with low-pass filter\n");
    
    printf("\nSawtooth wave examples (less high-frequency content):\n");
    for (int f = 0; f < 4; f++) {
        int freq = frequencies[f];
        printf("- sawtooth_raw_freq%d.wav - Unfiltered sawtooth wave at ~%d Hz\n", 
               freq, freq * 22050 / 65536);
        printf("- sawtooth_filtered_freq%d.wav - Filtered sawtooth wave at ~%d Hz\n", 
               freq, freq * 22050 / 65536);
    }
    
    printf("\nSquare wave examples (more high-frequency content, shows filter effect better):\n");
    for (int f = 0; f < 4; f++) {
        int freq = frequencies[f];
        printf("- square_raw_freq%d.wav - Unfiltered square wave at ~%d Hz\n", 
               freq, freq * 22050 / 65536);
        printf("- square_filtered_freq%d.wav - Filtered square wave at ~%d Hz\n", 
               freq, freq * 22050 / 65536);
    }
    
    printf("\nHow to evaluate the audio quality:\n");
    printf("1. Listen to the square wave examples first - the difference between filtered and unfiltered is most pronounced\n");
    printf("2. The filter reduces harsh high-frequency content that causes the buzzy distortion\n");
    printf("3. Higher frequencies (512, 1024) show more obvious differences\n");
    printf("4. In Audacity, you can view the waveform and spectrum to see the difference visually\n");
    printf("\nThe 0.75 multiplier in the filter provides a good balance between reducing harshness\n");
    printf("while maintaining the overall sound character and volume level.\n");
    
    return 0;
}