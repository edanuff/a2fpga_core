#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// Audio statistics structure
struct AudioStats {
    int16_t min;
    int16_t max;
    double average;
    double rms;
    int clipping_count;
    int total_samples;
    double peak_to_rms;  // Crest factor
    double zero_crossings_per_second;
};

// Calculate statistics for a set of audio samples
AudioStats calculate_stats(int16_t* samples, int count, int sample_rate) {
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
    
    // Calculate peak-to-RMS ratio (crest factor)
    double peak_value = fmax(fabs(stats.min), fabs(stats.max));
    stats.peak_to_rms = peak_value / stats.rms;
    
    // Count zero crossings
    int zero_crossings = 0;
    for (int i = 1; i < count; i++) {
        if ((samples[i-1] >= 0 && samples[i] < 0) || 
            (samples[i-1] < 0 && samples[i] >= 0)) {
            zero_crossings++;
        }
    }
    stats.zero_crossings_per_second = (double)zero_crossings * sample_rate / count;
    
    return stats;
}

// Analyze segments of audio for patterns that might indicate distortion
void analyze_segments(int16_t* samples, int total_samples, int sample_rate) {
    const int SEGMENT_SIZE = sample_rate / 10; // 100ms segments
    
    printf("\nSegment analysis (100ms windows):\n");
    printf("Segment | Min     | Max     | RMS     | Peak/RMS | Zeros/s  | Description\n");
    printf("--------|---------|---------|---------|---------|---------|--------------\n");
    
    for (int i = 0; i < total_samples; i += SEGMENT_SIZE) {
        int segment_size = (i + SEGMENT_SIZE > total_samples) ? (total_samples - i) : SEGMENT_SIZE;
        if (segment_size < 100) continue; // Skip tiny segments
        
        AudioStats segment_stats = calculate_stats(samples + i, segment_size, sample_rate);
        
        // Describe the segment based on its characteristics
        const char* description = "Normal";
        if (segment_stats.peak_to_rms > 10) {
            description = "High dynamic range";
        }
        if (segment_stats.peak_to_rms < 2) {
            description = "Compressed/clipped";
        }
        if (segment_stats.clipping_count > 0) {
            description = "CLIPPING";
        }
        if (segment_stats.zero_crossings_per_second > 5000) {
            description = "High frequency content";
        }
        if (fabs(segment_stats.average) > 1000) {
            description = "DC offset";
        }
        if (segment_stats.rms < 100) {
            description = "Silence/Low level";
        }
        
        printf("%-7d | %-7d | %-7d | %-7.1f | %-7.1f | %-7.1f | %s\n",
               i/SEGMENT_SIZE, 
               segment_stats.min, 
               segment_stats.max, 
               segment_stats.rms,
               segment_stats.peak_to_rms,
               segment_stats.zero_crossings_per_second,
               description);
               
        // Only analyze first 50 segments (5 seconds)
        if (i/SEGMENT_SIZE >= 49) break;
    }
}

// Analyze frequency content using primitive methods
void analyze_frequency(int16_t* samples, int total_samples, int sample_rate) {
    // Use zero-crossing rate as a simple frequency estimator for different window sizes
    const int window_sizes[] = {64, 128, 256, 512, 1024, 2048, 4096, 8192};
    const int num_windows = sizeof(window_sizes) / sizeof(window_sizes[0]);
    
    printf("\nFrequency content analysis (zero-crossing method):\n");
    printf("Window size | Avg Est. Freq (Hz) | Description\n");
    printf("------------|-------------------|-------------\n");
    
    for (int w = 0; w < num_windows; w++) {
        int window_size = window_sizes[w];
        int num_windows = total_samples / window_size;
        if (num_windows == 0) continue;
        
        double total_zero_crossings = 0;
        
        for (int i = 0; i < num_windows && i < 100; i++) { // Limit to 100 windows
            int start = i * window_size;
            int zero_crossings = 0;
            
            for (int j = start + 1; j < start + window_size; j++) {
                if ((samples[j-1] >= 0 && samples[j] < 0) || 
                    (samples[j-1] < 0 && samples[j] >= 0)) {
                    zero_crossings++;
                }
            }
            
            total_zero_crossings += zero_crossings;
        }
        
        double avg_crossings = total_zero_crossings / num_windows;
        double est_freq = avg_crossings * sample_rate / (2 * window_size);
        
        const char* description = "Normal";
        if (est_freq > 10000) description = "Very high freq content";
        else if (est_freq > 5000) description = "High freq content";
        else if (est_freq > 1000) description = "Mid-high freq content";
        else if (est_freq > 500) description = "Mid freq content";
        else if (est_freq > 100) description = "Low-mid freq content";
        else description = "Low freq content";
        
        printf("%-11d | %-17.1f | %s\n", window_size, est_freq, description);
    }
}

// Check for repeating patterns that could indicate aliasing or quantization issues
void analyze_patterns(int16_t* samples, int total_samples) {
    printf("\nPattern analysis (searching for digital artifacts):\n");
    
    // Search for repeating patterns of various sizes
    const int pattern_sizes[] = {2, 3, 4, 8, 16, 32, 64, 128, 256};
    const int num_sizes = sizeof(pattern_sizes) / sizeof(pattern_sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int pattern_size = pattern_sizes[s];
        int matches = 0;
        int strongest_match_pos = 0;
        int strongest_match_count = 0;
        
        // Only check first 10000 samples for patterns
        int check_samples = (total_samples < 10000) ? total_samples : 10000;
        
        for (int i = 0; i < check_samples - pattern_size * 2; i++) {
            int consecutive_matches = 0;
            
            // See if we have a repeating pattern of length pattern_size
            for (int j = 0; j < pattern_size; j++) {
                if (samples[i+j] == samples[i+j+pattern_size]) {
                    consecutive_matches++;
                }
            }
            
            if (consecutive_matches > pattern_size * 0.9) { // 90% match
                matches++;
                
                // Count how many times this pattern repeats consecutively
                int repeat_count = 1;
                for (int k = 1; i + (k+1)*pattern_size < check_samples; k++) {
                    int matches_next = 0;
                    for (int j = 0; j < pattern_size; j++) {
                        if (samples[i+j] == samples[i+j+k*pattern_size]) {
                            matches_next++;
                        }
                    }
                    if (matches_next > pattern_size * 0.9) {
                        repeat_count++;
                    } else {
                        break;
                    }
                }
                
                if (repeat_count > strongest_match_count) {
                    strongest_match_count = repeat_count;
                    strongest_match_pos = i;
                }
            }
        }
        
        if (matches > 0) {
            printf("Found %d potential repeating patterns of size %d\n", matches, pattern_size);
            printf("Strongest pattern repeats %d times starting at sample %d\n", 
                   strongest_match_count, strongest_match_pos);
                   
            // Analyze the pattern itself
            if (strongest_match_count >= 3) {
                printf("Pattern values: ");
                for (int j = 0; j < pattern_size && j < 16; j++) { // Print first 16 values max
                    printf("%d ", samples[strongest_match_pos + j]);
                }
                if (pattern_size > 16) printf("...");
                printf("\n");
            }
        }
    }
}

// Look for signs of digital aliasing
void check_for_aliasing(int16_t* samples, int total_samples) {
    printf("\nAliasing detection:\n");
    
    // Look for sharp transitions followed by ripples
    int sharp_transitions = 0;
    int ripple_patterns = 0;
    
    for (int i = 2; i < total_samples - 5; i++) {
        // Look for a large change between consecutive samples
        int delta1 = abs(samples[i] - samples[i-1]);
        
        if (delta1 > 5000) { // Threshold for "sharp" transition
            sharp_transitions++;
            
            // Check for oscillating pattern after the transition
            int sign_changes = 0;
            for (int j = i+1; j < i+5; j++) {
                if ((samples[j] - samples[j-1]) * (samples[j-1] - samples[j-2]) < 0) {
                    sign_changes++;
                }
            }
            
            if (sign_changes >= 3) {
                ripple_patterns++;
            }
        }
    }
    
    printf("Found %d sharp transitions\n", sharp_transitions);
    printf("Found %d potential aliasing patterns (ripples after transitions)\n", ripple_patterns);
    
    if (ripple_patterns > total_samples / 1000) {
        printf("ANALYSIS: High number of aliasing artifacts detected\n");
    } else if (ripple_patterns > 0) {
        printf("ANALYSIS: Some aliasing artifacts detected\n");
    } else {
        printf("ANALYSIS: No obvious aliasing artifacts detected\n");
    }
}

// Main function: read the WAV file and analyze it
int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <wav_file>\n", argv[0]);
        return 1;
    }
    
    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }
    
    // Read WAV header
    char header[44];
    if (fread(header, 1, 44, f) != 44) {
        printf("Error: Could not read WAV header\n");
        fclose(f);
        return 1;
    }
    
    // Check if it's a valid WAV file
    if (header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F' ||
        header[8] != 'W' || header[9] != 'A' || header[10] != 'V' || header[11] != 'E') {
        printf("Error: Not a valid WAV file\n");
        fclose(f);
        return 1;
    }
    
    // Parse header
    int num_channels = header[22] | (header[23] << 8);
    int sample_rate = header[24] | (header[25] << 8) | (header[26] << 16) | (header[27] << 24);
    int bits_per_sample = header[34] | (header[35] << 8);
    
    printf("WAV file: %s\n", argv[1]);
    printf("Channels: %d\n", num_channels);
    printf("Sample rate: %d Hz\n", sample_rate);
    printf("Bits per sample: %d\n", bits_per_sample);
    
    // Calculate file size from header
    uint32_t data_size = header[40] | (header[41] << 8) | (header[42] << 16) | (header[43] << 24);
    uint32_t num_samples = data_size / (num_channels * (bits_per_sample / 8));
    
    printf("Data size: %u bytes\n", data_size);
    printf("Number of samples: %u (%0.2f seconds)\n", num_samples, (float)num_samples / sample_rate);
    
    // For this analysis, we'll use just the first channel if stereo
    int16_t* samples = (int16_t*)malloc(num_samples * sizeof(int16_t));
    if (!samples) {
        printf("Error: Could not allocate memory for samples\n");
        fclose(f);
        return 1;
    }
    
    // Read the samples
    for (uint32_t i = 0; i < num_samples; i++) {
        int16_t sample;
        if (fread(&sample, sizeof(int16_t), 1, f) != 1) {
            printf("Error: Could not read sample %u\n", i);
            fclose(f);
            free(samples);
            return 1;
        }
        samples[i] = sample;
        
        // Skip other channels
        if (num_channels > 1) {
            fseek(f, (num_channels - 1) * (bits_per_sample / 8), SEEK_CUR);
        }
    }
    
    fclose(f);
    
    // Calculate overall statistics
    AudioStats stats = calculate_stats(samples, num_samples, sample_rate);
    
    printf("\nOverall audio statistics:\n");
    printf("Min: %d\n", stats.min);
    printf("Max: %d\n", stats.max);
    printf("Average: %.2f\n", stats.average);
    printf("RMS: %.2f\n", stats.rms);
    printf("Peak-to-RMS ratio: %.2f\n", stats.peak_to_rms);
    printf("Zero crossings per second: %.2f\n", stats.zero_crossings_per_second);
    printf("Clipping: %d samples (%.2f%%)\n", stats.clipping_count, 
           (stats.clipping_count * 100.0) / stats.total_samples);
    
    // Additional analysis
    analyze_segments(samples, num_samples, sample_rate);
    analyze_frequency(samples, num_samples, sample_rate);
    analyze_patterns(samples, num_samples);
    check_for_aliasing(samples, num_samples);
    
    // Summary of findings
    printf("\nSUMMARY OF DISTORTION ANALYSIS:\n");
    printf("------------------------------\n");
    
    // Clipping
    if (stats.clipping_count > 0) {
        printf("- CLIPPING DETECTED: %d samples are clipped (%.2f%%)\n", 
               stats.clipping_count, (stats.clipping_count * 100.0) / stats.total_samples);
    }
    
    // Dynamic range assessment
    if (stats.peak_to_rms < 3) {
        printf("- LOW DYNAMIC RANGE: Peak-to-RMS ratio is %.2f (typical for heavily compressed/limited audio)\n", 
               stats.peak_to_rms);
    } else if (stats.peak_to_rms > 12) {
        printf("- VERY HIGH DYNAMIC RANGE: Peak-to-RMS ratio is %.2f (unusual, may indicate transient spikes)\n", 
               stats.peak_to_rms);
    }
    
    // DC offset
    if (fabs(stats.average) > 500) {
        printf("- DC OFFSET DETECTED: Average value is %.2f (should be close to zero)\n", stats.average);
    }
    
    // Frequency content
    if (stats.zero_crossings_per_second > 10000) {
        printf("- HIGH FREQUENCY CONTENT: %.2f zero-crossings/second indicates significant high-frequency content\n", 
               stats.zero_crossings_per_second);
    }
    
    free(samples);
    return 0;
}