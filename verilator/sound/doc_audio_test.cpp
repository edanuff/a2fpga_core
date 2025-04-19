#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Test DOC audio calculations with the real frontleft.wavetable file
// Focus on raw output monitoring to better understand the issue

// Write audio samples to a WAV file for direct listening
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

// Exact version of our current DOC oscillator calculation (from SVerilog)
int16_t doc_output_current(uint8_t waveform, uint8_t volume) {
    // Convert waveform to signed (-128 to 127)
    int16_t signed_wave = (int8_t)(waveform ^ 0x80);
    
    // Calculate raw product
    int32_t product = signed_wave * volume;
    
    // Apply 1.5x amplification as in our current implementation
    product = product + (product >> 1);  // product * 1.5
    
    // Clamp to 16-bit range
    if (product > 32767) return 32767;
    if (product < -32768) return -32768;
    
    return (int16_t)product;
}

// Version with NO amplification - raw product only
int16_t doc_output_raw(uint8_t waveform, uint8_t volume) {
    // Convert waveform to signed (-128 to 127)
    int16_t signed_wave = (int8_t)(waveform ^ 0x80);
    
    // Calculate raw product only
    int32_t product = signed_wave * volume;
    
    // Clamp to 16-bit range (shouldn't be needed for raw product)
    if (product > 32767) return 32767;
    if (product < -32768) return -32768;
    
    return (int16_t)product;
}

// Audio statistics structure
struct AudioStats {
    int16_t min;
    int16_t max;
    double average;
    double rms;
    int clipping_count;
    int total_samples;
};

// Calculate statistics for a set of audio samples
AudioStats calculate_stats(int16_t* samples, int count) {
    AudioStats stats;
    stats.min = 32767;
    stats.max = -32768;
    stats.average = 0;
    stats.rms = 0;
    stats.clipping_count = 0;
    stats.total_samples = count;
    
    // First pass - min, max, average
    for (int i = 0; i < count; i++) {
        if (samples[i] < stats.min) stats.min = samples[i];
        if (samples[i] > stats.max) stats.max = samples[i];
        stats.average += (double)samples[i] / count;
        
        // Check for clipping
        if (samples[i] == 32767 || samples[i] == -32768) {
            stats.clipping_count++;
        }
    }
    
    // Second pass - RMS
    double sum_squares = 0;
    for (int i = 0; i < count; i++) {
        double deviation = samples[i] - stats.average;
        sum_squares += deviation * deviation;
    }
    stats.rms = sqrt(sum_squares / count);
    
    return stats;
}

// Apply mixer scaling to simulate oscillator mix
void apply_mixer_scaling(int16_t* src, int16_t* dst, size_t count, double scale) {
    for (size_t i = 0; i < count; i++) {
        int32_t scaled = (int32_t)(src[i] * scale);
        // Clamp to 16-bit range
        if (scaled > 32767) scaled = 32767;
        if (scaled < -32768) scaled = -32768;
        dst[i] = (int16_t)scaled;
    }
}

int main() {
    printf("DOC Audio Output Analysis\n");
    printf("========================\n\n");
    
    // Load the wave table data
    FILE* wave_file = fopen("frontleft.wavetable", "rb");
    if (!wave_file) {
        printf("ERROR: Could not open frontleft.wavetable\n");
        return 1;
    }
    
    // Read the wave data
    uint8_t wave_data[32768]; // Max DOC wave table size
    size_t bytes_read = fread(wave_data, 1, sizeof(wave_data), wave_file);
    fclose(wave_file);
    printf("Loaded %ld bytes of wave data\n\n", bytes_read);
    
    // Basic statistics on the waveform
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
    
    // Generate audio samples with our current implementation
    int16_t* current_impl_samples = (int16_t*)malloc(bytes_read * sizeof(int16_t));
    int16_t* raw_impl_samples = (int16_t*)malloc(bytes_read * sizeof(int16_t));
    
    // Test with volume = 255 (maximum)
    for (size_t i = 0; i < bytes_read; i++) {
        current_impl_samples[i] = doc_output_current(wave_data[i], 255);
        raw_impl_samples[i] = doc_output_raw(wave_data[i], 255);
    }
    
    // Calculate statistics
    AudioStats current_stats = calculate_stats(current_impl_samples, bytes_read);
    AudioStats raw_stats = calculate_stats(raw_impl_samples, bytes_read);
    
    printf("Audio output statistics (vol=255):\n");
    printf("Current impl (1.5x): Min=%d, Max=%d, Avg=%.1f, RMS=%.1f, Clipping=%d (%.1f%%)\n", 
           current_stats.min, current_stats.max, current_stats.average, current_stats.rms,
           current_stats.clipping_count, (current_stats.clipping_count * 100.0) / current_stats.total_samples);
    printf("Raw product: Min=%d, Max=%d, Avg=%.1f, RMS=%.1f, Clipping=%d (%.1f%%)\n\n", 
           raw_stats.min, raw_stats.max, raw_stats.average, raw_stats.rms,
           raw_stats.clipping_count, (raw_stats.clipping_count * 100.0) / raw_stats.total_samples);
    
    // Now test mixer scaling (using our current implementation)
    int16_t* mixer_scaled_samples = (int16_t*)malloc(bytes_read * sizeof(int16_t));
    
    printf("Mixer scaling with current implementation:\n");
    const double scales[] = {1.0, 0.5, 0.25, 0.125, 0.0625};
    const int num_scales = sizeof(scales) / sizeof(scales[0]);
    
    for (int s = 0; s < num_scales; s++) {
        double scale = scales[s];
        apply_mixer_scaling(current_impl_samples, mixer_scaled_samples, bytes_read, scale);
        AudioStats mixer_stats = calculate_stats(mixer_scaled_samples, bytes_read);
        
        printf("Scale %.4f: Min=%d, Max=%d, Avg=%.1f, RMS=%.1f, Clipping=%d (%.1f%%)\n", 
               scale, mixer_stats.min, mixer_stats.max, mixer_stats.average, mixer_stats.rms,
               mixer_stats.clipping_count, (mixer_stats.clipping_count * 100.0) / mixer_stats.total_samples);
        
        // Write out WAV file for this scaling
        char filename[100];
        sprintf(filename, "doc_output_scale_%.4f.wav", scale);
        write_wav_file(filename, mixer_scaled_samples, bytes_read, 22050);
    }
    
    // Test with no amplification but varying volumes
    printf("\nScaling via volume control (no amplification):\n");
    const uint8_t volumes[] = {32, 64, 128, 192, 255};
    const int num_volumes = sizeof(volumes) / sizeof(volumes[0]);
    
    for (int v = 0; v < num_volumes; v++) {
        uint8_t volume = volumes[v];
        
        for (size_t i = 0; i < bytes_read; i++) {
            raw_impl_samples[i] = doc_output_raw(wave_data[i], volume);
        }
        
        AudioStats vol_stats = calculate_stats(raw_impl_samples, bytes_read);
        
        printf("Volume %3d: Min=%d, Max=%d, Avg=%.1f, RMS=%.1f, Clipping=%d (%.1f%%)\n", 
               volume, vol_stats.min, vol_stats.max, vol_stats.average, vol_stats.rms,
               vol_stats.clipping_count, (vol_stats.clipping_count * 100.0) / vol_stats.total_samples);
               
        // Write out WAV file for this volume
        char filename[100];
        sprintf(filename, "doc_output_vol_%d.wav", volume);
        write_wav_file(filename, raw_impl_samples, bytes_read, 22050);
    }
    
    // Generate multiple oscillator simulation (8 oscillators)
    printf("\nMultiple oscillator simulation (8 oscillators, raw output):\n");
    
    int32_t* mixed_samples = (int32_t*)calloc(bytes_read, sizeof(int32_t));
    if (!mixed_samples) {
        printf("ERROR: Could not allocate memory for mixed samples\n");
        return 1;
    }
    
    // Create 8 oscillators with offsets
    const int NUM_OSCILLATORS = 8;
    const int OFFSETS[NUM_OSCILLATORS] = {0, 128, 256, 384, 512, 640, 768, 896};
    
    // Mix the oscillators (raw output, no amplification)
    for (int osc = 0; osc < NUM_OSCILLATORS; osc++) {
        int offset = OFFSETS[osc] % bytes_read;
        
        for (size_t i = 0; i < bytes_read; i++) {
            size_t idx = (i + offset) % bytes_read;
            int16_t sample = doc_output_raw(wave_data[idx], 255); // Max volume
            mixed_samples[i] += sample;
        }
    }
    
    // Convert to 16-bit with different scaling
    for (int s = 0; s < num_scales; s++) {
        double scale = scales[s];
        
        // Apply scaling
        for (size_t i = 0; i < bytes_read; i++) {
            int32_t scaled = (int32_t)(mixed_samples[i] * scale);
            // Clamp to 16-bit range
            if (scaled > 32767) scaled = 32767;
            if (scaled < -32768) scaled = -32768;
            mixer_scaled_samples[i] = (int16_t)scaled;
        }
        
        // Calculate statistics
        AudioStats mix_stats = calculate_stats(mixer_scaled_samples, bytes_read);
        
        printf("8 osc + Scale %.4f: Min=%d, Max=%d, Avg=%.1f, RMS=%.1f, Clipping=%d (%.1f%%)\n", 
               scale, mix_stats.min, mix_stats.max, mix_stats.average, mix_stats.rms,
               mix_stats.clipping_count, (mix_stats.clipping_count * 100.0) / mix_stats.total_samples);
        
        // Write WAV for this multiple oscillator mix
        char filename[100];
        sprintf(filename, "doc_output_8osc_scale_%.4f.wav", scale);
        write_wav_file(filename, mixer_scaled_samples, bytes_read, 22050);
    }
    
    // Cleanup
    free(current_impl_samples);
    free(raw_impl_samples);
    free(mixer_scaled_samples);
    free(mixed_samples);
    
    printf("\nKey insights and recommendations:\n");
    printf("1. Generated WAV files allow direct listening to check for distortion\n");
    printf("2. Try using NO amplification in the oscillator output - just raw product\n");
    printf("3. Apply appropriate mixer scaling based on oscillator count\n");
    printf("4. Consider that distortion might be from elsewhere in the audio chain\n");
    
    return 0;
}