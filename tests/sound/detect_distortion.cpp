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

// Calculate the harmonic distortion by examining the ratio between 
// fundamental frequency and harmonics
double estimate_thd(int16_t* samples, int sample_count, int sample_rate) {
    // This is a simplified approach without FFT:
    // 1. Look for outlier samples that exceed typical signal levels
    // 2. Measure spacing between outliers to estimate distortion pattern
    
    printf("\nAnalyzing harmonic distortion (simplified):\n");
    
    // Calculate signal statistics
    double mean = 0;
    double rms = 0;
    int16_t min = 32767;
    int16_t max = -32768;
    
    for (int i = 0; i < sample_count; i++) {
        mean += samples[i] / (double)sample_count;
        rms += samples[i] * samples[i] / (double)sample_count;
        if (samples[i] < min) min = samples[i];
        if (samples[i] > max) max = samples[i];
    }
    
    rms = sqrt(rms);
    double peak_to_rms = fmax(fabs(min), fabs(max)) / rms;
    
    printf("Mean: %.2f\n", mean);
    printf("RMS: %.2f\n", rms);
    printf("Min: %d\n", min);
    printf("Max: %d\n", max);
    printf("Peak-to-RMS ratio: %.2f\n", peak_to_rms);
    
    // Count outliers (samples that deviate greatly from mean)
    double threshold = 3.0 * rms; // Common threshold for outliers
    int outlier_count = 0;
    
    for (int i = 0; i < sample_count; i++) {
        if (fabs(samples[i] - mean) > threshold) {
            outlier_count++;
        }
    }
    
    printf("Outlier count (>3*RMS): %d (%.2f%%)\n", 
           outlier_count, (100.0 * outlier_count) / sample_count);
    
    // Check for clipping
    int clipping_count = 0;
    for (int i = 0; i < sample_count; i++) {
        if (samples[i] >= 32767 || samples[i] <= -32768) {
            clipping_count++;
        }
    }
    
    printf("Clipping count: %d (%.2f%%)\n", 
           clipping_count, (100.0 * clipping_count) / sample_count);
    
    // Look for regular patterns of transients/outliers that could indicate digital distortion
    int pattern_count = 0;
    int last_outlier_pos = -1;
    int min_spacing = sample_count;
    int max_spacing = 0;
    int total_spacing = 0;
    int spacing_count = 0;
    
    for (int i = 0; i < sample_count; i++) {
        if (fabs(samples[i] - mean) > threshold) {
            if (last_outlier_pos >= 0) {
                int spacing = i - last_outlier_pos;
                total_spacing += spacing;
                spacing_count++;
                
                if (spacing < min_spacing) min_spacing = spacing;
                if (spacing > max_spacing) max_spacing = spacing;
            }
            last_outlier_pos = i;
        }
    }
    
    if (spacing_count > 0) {
        double avg_spacing = total_spacing / (double)spacing_count;
        printf("Outlier spacing analysis:\n");
        printf("  Minimum spacing: %d samples (%.2f ms)\n", 
               min_spacing, 1000.0 * min_spacing / sample_rate);
        printf("  Maximum spacing: %d samples (%.2f ms)\n", 
               max_spacing, 1000.0 * max_spacing / sample_rate);
        printf("  Average spacing: %.2f samples (%.2f ms)\n", 
               avg_spacing, 1000.0 * avg_spacing / sample_rate);
        
        // Calculate uniformity of spacing (as a form of regularity)
        double std_dev = 0;
        for (int i = 1; i < sample_count; i++) {
            if (fabs(samples[i] - mean) > threshold && 
                fabs(samples[i-1] - mean) <= threshold) {
                for (int j = i + 1; j < sample_count; j++) {
                    if (fabs(samples[j] - mean) > threshold && 
                        fabs(samples[j-1] - mean) <= threshold) {
                        int spacing = j - i;
                        std_dev += pow(spacing - avg_spacing, 2) / spacing_count;
                        break;
                    }
                }
            }
        }
        std_dev = sqrt(std_dev);
        
        double uniformity = 1.0 - (std_dev / avg_spacing);
        printf("  Spacing uniformity: %.2f (1.0 = perfectly regular, 0.0 = random)\n", uniformity);
        
        // Estimate frequency of distortion
        double estimated_freq = sample_rate / avg_spacing;
        printf("  Estimated distortion frequency: %.2f Hz\n", estimated_freq);
        
        // Return a rough estimate of total harmonic distortion based on uniformity and outlier percentage
        double estimated_thd = (outlier_count / (double)sample_count) * uniformity * 100.0;
        printf("  Estimated THD: %.2f%%\n", estimated_thd);
        return estimated_thd;
    }
    
    return 0.0;
}

// Analyze discontinuities in the signal
void analyze_discontinuities(int16_t* samples, int sample_count, int sample_rate) {
    printf("\nAnalyzing signal discontinuities:\n");
    
    const int WINDOW_SIZE = 5; // Look at 5 samples before and after each point
    int strong_jumps = 0;
    
    // Calculate average delta between adjacent samples
    double avg_delta = 0;
    for (int i = 1; i < sample_count; i++) {
        avg_delta += fabs(samples[i] - samples[i-1]) / (sample_count - 1);
    }
    printf("Average sample-to-sample delta: %.2f\n", avg_delta);
    
    // Find sudden large jumps (discontinuities)
    for (int i = WINDOW_SIZE; i < sample_count - WINDOW_SIZE; i++) {
        // Calculate average deltas in windows before and after current point
        double before_avg = 0;
        double after_avg = 0;
        
        for (int j = 1; j <= WINDOW_SIZE; j++) {
            before_avg += fabs(samples[i-j] - samples[i-j-1]) / WINDOW_SIZE;
            after_avg += fabs(samples[i+j] - samples[i+j-1]) / WINDOW_SIZE;
        }
        
        // Calculate the jump at this point
        double jump = fabs(samples[i] - samples[i-1]);
        
        // If the jump is much larger than the average deltas before and after,
        // it's a potential discontinuity
        if (jump > 5.0 * fmax(before_avg, after_avg) && jump > 5.0 * avg_delta) {
            strong_jumps++;
            
            // Only print first 10 discontinuities
            if (strong_jumps <= 10) {
                printf("Discontinuity at sample %d (%.2f ms): delta=%d (%.2fx avg)\n",
                       i, i * 1000.0 / sample_rate, samples[i] - samples[i-1],
                       jump / avg_delta);
            }
        }
    }
    
    printf("Total strong discontinuities: %d (%.2f per second)\n",
           strong_jumps, strong_jumps * sample_rate / (float)sample_count);
    
    if (strong_jumps > sample_count / 1000) {
        printf("RESULT: High number of discontinuities detected - likely digital distortion\n");
    } else if (strong_jumps > 0) {
        printf("RESULT: Some discontinuities detected - possible mild distortion\n");
    } else {
        printf("RESULT: No significant discontinuities detected\n");
    }
}

// Analyze zero-crossings to detect frequency anomalies
void analyze_zero_crossings(int16_t* samples, int sample_count, int sample_rate) {
    printf("\nAnalyzing zero-crossings:\n");
    
    int zero_crossings = 0;
    const int WINDOW_SIZE = sample_rate / 100; // 10ms windows
    
    // Count overall zero crossings
    for (int i = 1; i < sample_count; i++) {
        if ((samples[i-1] < 0 && samples[i] >= 0) ||
            (samples[i-1] >= 0 && samples[i] < 0)) {
            zero_crossings++;
        }
    }
    
    double zc_per_second = zero_crossings * sample_rate / (double)sample_count;
    printf("Overall zero-crossings per second: %.2f\n", zc_per_second);
    
    // Analyze zero-crossing rate (ZCR) in small windows to detect anomalies
    int window_count = sample_count / WINDOW_SIZE;
    int* window_zc = (int*)calloc(window_count, sizeof(int));
    
    for (int w = 0; w < window_count; w++) {
        int start = w * WINDOW_SIZE;
        int end = start + WINDOW_SIZE;
        if (end > sample_count) end = sample_count;
        
        for (int i = start + 1; i < end; i++) {
            if ((samples[i-1] < 0 && samples[i] >= 0) ||
                (samples[i-1] >= 0 && samples[i] < 0)) {
                window_zc[w]++;
            }
        }
    }
    
    // Calculate statistics on window ZCR
    int min_zc = sample_count;
    int max_zc = 0;
    double avg_zc = 0;
    
    for (int w = 0; w < window_count; w++) {
        if (window_zc[w] < min_zc) min_zc = window_zc[w];
        if (window_zc[w] > max_zc) max_zc = window_zc[w];
        avg_zc += window_zc[w] / (double)window_count;
    }
    
    printf("Zero-crossing rate per 10ms window:\n");
    printf("  Minimum: %d (%.2f Hz equivalent)\n", min_zc, min_zc * 100);
    printf("  Maximum: %d (%.2f Hz equivalent)\n", max_zc, max_zc * 100);
    printf("  Average: %.2f (%.2f Hz equivalent)\n", avg_zc, avg_zc * 100);
    
    // Calculate standard deviation of ZCR
    double zc_std_dev = 0;
    for (int w = 0; w < window_count; w++) {
        zc_std_dev += pow(window_zc[w] - avg_zc, 2) / window_count;
    }
    zc_std_dev = sqrt(zc_std_dev);
    printf("  Standard deviation: %.2f (%.2f%% of mean)\n", 
           zc_std_dev, 100.0 * zc_std_dev / avg_zc);
    
    // Look for outlier windows with unusually high ZCR
    int high_zcr_windows = 0;
    for (int w = 0; w < window_count; w++) {
        if (window_zc[w] > avg_zc + 2.0 * zc_std_dev) {
            high_zcr_windows++;
        }
    }
    
    printf("Windows with unusually high ZCR: %d (%.2f%%)\n",
           high_zcr_windows, 100.0 * high_zcr_windows / window_count);
    
    if (high_zcr_windows > window_count / 10) {
        printf("RESULT: Significant high-frequency anomalies detected\n");
    } else if (high_zcr_windows > 0) {
        printf("RESULT: Some high-frequency anomalies detected\n");
    } else {
        printf("RESULT: No significant high-frequency anomalies detected\n");
    }
    
    free(window_zc);
}

// Analyze the signal for quantization noise and aliasing
void analyze_quantization(int16_t* samples, int sample_count) {
    printf("\nAnalyzing quantization effects:\n");
    
    // Count unique sample values
    const int MAX_UNIQUE = 65536;  // Maximum possible for 16-bit audio
    int unique_count = 0;
    int* value_counts = (int*)calloc(MAX_UNIQUE, sizeof(int));
    
    for (int i = 0; i < sample_count; i++) {
        // Convert from signed to unsigned for array indexing
        unsigned int index = (unsigned int)(samples[i] + 32768);
        if (index < MAX_UNIQUE) {
            if (value_counts[index] == 0) {
                unique_count++;
            }
            value_counts[index]++;
        }
    }
    
    printf("Unique sample values: %d (%.2f%% of 16-bit range)\n",
           unique_count, 100.0 * unique_count / MAX_UNIQUE);
    
    // Find most common values
    int top_values[5] = {0};
    int top_counts[5] = {0};
    
    for (int i = 0; i < MAX_UNIQUE; i++) {
        for (int j = 0; j < 5; j++) {
            if (value_counts[i] > top_counts[j]) {
                // Shift everything down
                for (int k = 4; k > j; k--) {
                    top_values[k] = top_values[k-1];
                    top_counts[k] = top_counts[k-1];
                }
                top_values[j] = i - 32768; // Convert back to signed
                top_counts[j] = value_counts[i];
                break;
            }
        }
    }
    
    printf("Most common sample values:\n");
    for (int i = 0; i < 5; i++) {
        printf("  %d: %d occurrences (%.2f%%)\n",
               top_values[i], top_counts[i], 100.0 * top_counts[i] / sample_count);
    }
    
    // Check for quantization patterns
    int step_counts[256] = {0}; // Count occurrences of each step size
    for (int i = 1; i < sample_count; i++) {
        int step = abs(samples[i] - samples[i-1]);
        if (step < 256) {
            step_counts[step]++;
        }
    }
    
    printf("Most common step sizes between adjacent samples:\n");
    int total_steps = 0;
    for (int i = 0; i < 256; i++) {
        total_steps += step_counts[i];
    }
    
    int top_steps[5] = {0};
    int top_step_counts[5] = {0};
    
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 5; j++) {
            if (step_counts[i] > top_step_counts[j]) {
                // Shift everything down
                for (int k = 4; k > j; k--) {
                    top_steps[k] = top_steps[k-1];
                    top_step_counts[k] = top_step_counts[k-1];
                }
                top_steps[j] = i;
                top_step_counts[j] = step_counts[i];
                break;
            }
        }
    }
    
    for (int i = 0; i < 5; i++) {
        printf("  Step size %d: %d occurrences (%.2f%%)\n",
               top_steps[i], top_step_counts[i], 100.0 * top_step_counts[i] / total_steps);
    }
    
    // Analyze distribution of values for patterns indicating quantization distortion
    int small_values = 0;
    int medium_values = 0;
    int large_values = 0;
    
    for (int i = 0; i < sample_count; i++) {
        int abs_val = abs(samples[i]);
        if (abs_val < 100) small_values++;
        else if (abs_val < 1000) medium_values++;
        else large_values++;
    }
    
    printf("Value distribution:\n");
    printf("  Small values (0-99): %d (%.2f%%)\n", 
           small_values, 100.0 * small_values / sample_count);
    printf("  Medium values (100-999): %d (%.2f%%)\n", 
           medium_values, 100.0 * medium_values / sample_count);
    printf("  Large values (1000+): %d (%.2f%%)\n", 
           large_values, 100.0 * large_values / sample_count);
    
    // Calculate approximate effective bit depth based on unique values
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
    
    // Run various analyses
    double thd = estimate_thd(samples, sample_count, sample_rate);
    analyze_discontinuities(samples, sample_count, sample_rate);
    analyze_zero_crossings(samples, sample_count, sample_rate);
    analyze_quantization(samples, sample_count);
    
    // Overall assessment
    printf("\nOVERALL DISTORTION ASSESSMENT:\n");
    
    if (thd > 10.0) {
        printf("HIGH DISTORTION DETECTED: Estimated THD > 10%%\n");
    } else if (thd > 3.0) {
        printf("MODERATE DISTORTION DETECTED: Estimated THD between 3-10%%\n");
    } else {
        printf("LOW DISTORTION: Estimated THD < 3%%\n");
    }
    
    // Free memory and exit
    free(samples);
    return 0;
}