#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// Read audio samples from a WAV file
// Returns the number of samples read, or -1 on error
int read_wav_samples(const char* filename, int16_t** samples, int* sample_rate, int* num_channels) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }
    
    // Read WAV header
    unsigned char header[44];
    if (fread(header, 1, 44, f) != 44) {
        printf("Error: Could not read WAV header\n");
        fclose(f);
        return -1;
    }
    
    // Check if it's a valid WAV file
    if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0) {
        printf("Error: Not a valid WAV file\n");
        fclose(f);
        return -1;
    }
    
    // Find 'fmt ' chunk
    unsigned char* fmt_ptr = header + 12; // Skip RIFF+size+WAVE
    while (memcmp(fmt_ptr, "fmt ", 4) != 0) {
        fmt_ptr += 8 + *(uint32_t*)(fmt_ptr + 4); // Skip to next chunk
        if (fmt_ptr >= header + 44) {
            printf("Error: fmt chunk not found\n");
            fclose(f);
            return -1;
        }
    }
    
    // Parse fmt chunk
    *num_channels = *(uint16_t*)(fmt_ptr + 10);
    *sample_rate = *(uint32_t*)(fmt_ptr + 12);
    uint16_t bits_per_sample = *(uint16_t*)(fmt_ptr + 22);
    
    // Find 'data' chunk
    unsigned char* data_ptr = fmt_ptr + 8 + *(uint32_t*)(fmt_ptr + 4);
    while (memcmp(data_ptr, "data", 4) != 0) {
        data_ptr += 8 + *(uint32_t*)(data_ptr + 4); // Skip to next chunk
        if (data_ptr >= header + 100) { // Allow for extra chunks
            printf("Error: data chunk not found\n");
            fclose(f);
            return -1;
        }
    }
    
    uint32_t data_size = *(uint32_t*)(data_ptr + 4);
    uint32_t sample_count = data_size / (*num_channels * bits_per_sample / 8);
    
    printf("WAV file: %s\n", filename);
    printf("Channels: %d\n", *num_channels);
    printf("Sample rate: %d Hz\n", *sample_rate);
    printf("Bits per sample: %d\n", bits_per_sample);
    printf("Sample count: %u (%0.2f seconds)\n", 
           sample_count, (float)sample_count / *sample_rate);
    
    // Allocate memory for samples (first channel only)
    *samples = (int16_t*)malloc(sample_count * sizeof(int16_t));
    if (!*samples) {
        printf("Error: Could not allocate memory for samples\n");
        fclose(f);
        return -1;
    }
    
    // Seek to start of data
    fseek(f, data_ptr + 8 - header, SEEK_SET);
    
    // Read samples from first channel
    for (uint32_t i = 0; i < sample_count; i++) {
        // Read one sample
        int16_t sample;
        if (fread(&sample, sizeof(int16_t), 1, f) != 1) {
            printf("Error: Could not read sample %u\n", i);
            free(*samples);
            fclose(f);
            return -1;
        }
        (*samples)[i] = sample;
        
        // Skip other channels
        if (*num_channels > 1) {
            fseek(f, (*num_channels - 1) * bits_per_sample / 8, SEEK_CUR);
        }
    }
    
    fclose(f);
    return sample_count;
}

// Find significant negative dips in the audio
void find_major_dips(int16_t* samples, int sample_count, int sample_rate) {
    printf("\nSearching for significant negative dips...\n");
    
    // First, calculate the standard deviation to detect significant outliers
    double sum = 0;
    double sum_squares = 0;
    int16_t min_val = 32767;
    int16_t max_val = -32768;
    
    for (int i = 0; i < sample_count; i++) {
        sum += samples[i];
        sum_squares += (double)samples[i] * samples[i];
        if (samples[i] < min_val) min_val = samples[i];
        if (samples[i] > max_val) max_val = samples[i];
    }
    
    double mean = sum / sample_count;
    double variance = (sum_squares / sample_count) - (mean * mean);
    double std_dev = sqrt(variance);
    
    printf("Signal statistics:\n");
    printf("  Mean: %.2f\n", mean);
    printf("  Standard deviation: %.2f\n", std_dev);
    printf("  Min value: %d\n", min_val);
    printf("  Max value: %d\n", max_val);
    
    // Use a longer window for detecting significant dips
    const int WINDOW_SIZE = 50;
    const double SIGNIFICANT_FACTOR = 2.0; // How many std devs below surrounding average
    
    int* dip_positions = (int*)malloc(100 * sizeof(int)); // Store up to 100 significant dip positions
    int dip_count = 0;
    
    // Find significant dips
    for (int i = WINDOW_SIZE; i < sample_count - WINDOW_SIZE; i++) {
        // Calculate average of surrounding samples
        double window_sum = 0;
        for (int j = -WINDOW_SIZE; j <= WINDOW_SIZE; j++) {
            if (j != 0) { // Skip the current sample
                window_sum += samples[i + j];
            }
        }
        double window_avg = window_sum / (2 * WINDOW_SIZE);
        
        // If current sample is significantly below the window average
        if (samples[i] < window_avg - SIGNIFICANT_FACTOR * std_dev) {
            // Only store if it's a local minimum
            bool is_local_min = true;
            for (int j = -5; j <= 5; j++) {
                if (j != 0 && i + j >= 0 && i + j < sample_count && samples[i + j] < samples[i]) {
                    is_local_min = false;
                    break;
                }
            }
            
            if (is_local_min) {
                // Don't add dips too close to previously detected ones
                bool too_close = false;
                for (int j = 0; j < dip_count; j++) {
                    if (abs(i - dip_positions[j]) < WINDOW_SIZE / 2) {
                        too_close = true;
                        break;
                    }
                }
                
                if (!too_close && dip_count < 100) {
                    dip_positions[dip_count++] = i;
                    printf("Significant dip at sample %d (%.3f sec): value=%d (%.2f std devs below surrounding avg)\n",
                           i, (float)i / sample_rate, samples[i], 
                           (window_avg - samples[i]) / std_dev);
                }
            }
        }
    }
    
    printf("\nFound %d significant dips\n", dip_count);
    
    // If no significant dips found, exit
    if (dip_count < 2) {
        free(dip_positions);
        return;
    }
    
    // Analyze spacing between dips
    int* spacings = (int*)malloc((dip_count-1) * sizeof(int));
    for (int i = 0; i < dip_count - 1; i++) {
        spacings[i] = dip_positions[i+1] - dip_positions[i];
    }
    
    // Calculate basic statistics on spacings
    int min_spacing = sample_count;
    int max_spacing = 0;
    double avg_spacing = 0;
    
    for (int i = 0; i < dip_count - 1; i++) {
        if (spacings[i] < min_spacing) min_spacing = spacings[i];
        if (spacings[i] > max_spacing) max_spacing = spacings[i];
        avg_spacing += spacings[i] / (double)(dip_count - 1);
    }
    
    printf("\nDip spacing analysis:\n");
    printf("  Minimum spacing: %d samples (%.2f ms)\n", 
           min_spacing, 1000.0 * min_spacing / sample_rate);
    printf("  Maximum spacing: %d samples (%.2f ms)\n", 
           max_spacing, 1000.0 * max_spacing / sample_rate);
    printf("  Average spacing: %.2f samples (%.2f ms)\n", 
           avg_spacing, 1000.0 * avg_spacing / sample_rate);
    
    // Calculate frequency of dips
    double dip_frequency = sample_rate / avg_spacing;
    printf("  Estimated dip frequency: %.2f Hz\n", dip_frequency);
    
    // Check for regularity of dip spacing
    double spacing_std_dev = 0;
    for (int i = 0; i < dip_count - 1; i++) {
        spacing_std_dev += pow(spacings[i] - avg_spacing, 2) / (dip_count - 1);
    }
    spacing_std_dev = sqrt(spacing_std_dev);
    
    double spacing_regularity = 1.0 - (spacing_std_dev / avg_spacing);
    printf("  Spacing regularity: %.2f (1.0 = perfectly regular, 0.0 = random)\n", 
           spacing_regularity);
    
    // Visualize waveform around each dip
    printf("\nWaveform visualization around dips:\n");
    for (int i = 0; i < dip_count && i < 5; i++) { // Show first 5 dips
        int pos = dip_positions[i];
        int start = pos - 10;
        if (start < 0) start = 0;
        int end = pos + 10;
        if (end >= sample_count) end = sample_count - 1;
        
        printf("\nDip %d at sample %d (%.3f sec):\n", i+1, pos, (float)pos / sample_rate);
        printf("Sample values around dip:\n");
        
        // Print sample values
        for (int j = start; j <= end; j++) {
            printf("%d%s", samples[j], j == pos ? "*" : " ");
            if ((j - start + 1) % 5 == 0) printf("\n");
        }
        if ((end - start + 1) % 5 != 0) printf("\n");
        
        // Simple ASCII visualization
        int height = 10;
        int16_t min_window = 32767;
        int16_t max_window = -32768;
        
        for (int j = start; j <= end; j++) {
            if (samples[j] < min_window) min_window = samples[j];
            if (samples[j] > max_window) max_window = samples[j];
        }
        
        // Make sure range isn't zero
        if (max_window == min_window) {
            max_window = min_window + 1;
        }
        
        printf("Visual representation [%d to %d]:\n", min_window, max_window);
        
        for (int row = 0; row < height; row++) {
            int16_t row_val = max_window - (row * (max_window - min_window) / (height - 1));
            
            for (int j = start; j <= end; j++) {
                if (j == pos) {
                    printf("*"); // Mark the dip position
                } else if (samples[j] >= row_val - (max_window - min_window) / (2 * height) && 
                          samples[j] <= row_val + (max_window - min_window) / (2 * height)) {
                    printf("o");
                } else if (samples[j] > row_val) {
                    printf(" ");
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }
    
    // Provide analysis of what might cause these dips
    if (spacing_regularity > 0.7) {
        printf("\nANALYSIS: Regular negative dips detected at approximately %.2f Hz\n", 
               dip_frequency);
        printf("This pattern suggests potential issues with:\n");
        printf("1. Buffer underruns in the audio processing chain\n");
        printf("2. Clock synchronization issues between oscillators\n");
        printf("3. Potential memory access conflicts when multiple oscillators are active\n");
    } else {
        printf("\nANALYSIS: Irregular but significant negative dips detected\n");
        printf("This pattern suggests potential issues with:\n");
        printf("1. Resource contention when processing multiple oscillators\n");
        printf("2. Occasional buffer underruns or processing delays\n");
        printf("3. Priority inversion in the audio processing chain\n");
    }
    
    free(dip_positions);
    free(spacings);
}

// Analyze the signal for quantization noise
void analyze_quantization(int16_t* samples, int sample_count) {
    printf("\nAnalyzing quantization effects:\n");
    
    // Count unique sample values
    const int MAX_UNIQUE = 65536;  // Maximum possible for 16-bit audio
    int* value_counts = (int*)calloc(MAX_UNIQUE, sizeof(int));
    
    for (int i = 0; i < sample_count; i++) {
        // Convert from signed to unsigned for array indexing
        unsigned int index = (unsigned int)(samples[i] + 32768);
        if (index < MAX_UNIQUE) {
            value_counts[index]++;
        }
    }
    
    // Count unique values and find most common values
    int unique_count = 0;
    int top_values[10] = {0};
    int top_counts[10] = {0};
    
    for (int i = 0; i < MAX_UNIQUE; i++) {
        if (value_counts[i] > 0) {
            unique_count++;
            
            // Check if this value should be in the top 10
            for (int j = 0; j < 10; j++) {
                if (value_counts[i] > top_counts[j]) {
                    // Shift everything down
                    for (int k = 9; k > j; k--) {
                        top_values[k] = top_values[k-1];
                        top_counts[k] = top_counts[k-1];
                    }
                    top_values[j] = i - 32768; // Convert back to signed
                    top_counts[j] = value_counts[i];
                    break;
                }
            }
        }
    }
    
    printf("Unique sample values: %d (%.2f%% of 16-bit range)\n",
           unique_count, 100.0 * unique_count / MAX_UNIQUE);
    
    printf("Most common sample values:\n");
    for (int i = 0; i < 10 && top_counts[i] > 0; i++) {
        printf("  %4d: %7d occurrences (%.2f%%)\n",
               top_values[i], top_counts[i], 100.0 * top_counts[i] / sample_count);
    }
    
    // Calculate approx effective bit depth
    double effective_bits = log2(unique_count);
    printf("Approximate effective bit depth: %.1f bits\n", effective_bits);
    
    if (effective_bits < 10) {
        printf("RESULT: Significant quantization effects detected (effective bit depth < 10)\n");
    } else if (effective_bits < 14) {
        printf("RESULT: Moderate quantization effects detected\n");
    } else {
        printf("RESULT: Minimal quantization effects\n");
    }
    
    free(value_counts);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <wav_file>\n", argv[0]);
        return 1;
    }
    
    int16_t* samples = NULL;
    int sample_rate = 0;
    int num_channels = 0;
    
    int sample_count = read_wav_samples(argv[1], &samples, &sample_rate, &num_channels);
    if (sample_count <= 0) {
        return 1;
    }
    
    printf("\n--- DISTORTION ANALYSIS ---\n");
    
    // Run analyses
    find_major_dips(samples, sample_count, sample_rate);
    analyze_quantization(samples, sample_count);
    
    // Free memory and exit
    free(samples);
    return 0;
}