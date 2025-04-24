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

// Create a simple ASCII waveform visualization
void visualize_waveform_ascii(int16_t* samples, int sample_count, int window_size, int height) {
    printf("\nWaveform Visualization (ASCII):\n");
    
    // Display a segment of the waveform at a time
    for (int offset = 0; offset < sample_count; offset += window_size) {
        int end = offset + window_size;
        if (end > sample_count) end = sample_count;
        
        // Find min/max in this window for scaling
        int16_t min_val = 32767;
        int16_t max_val = -32768;
        for (int i = offset; i < end; i++) {
            if (samples[i] < min_val) min_val = samples[i];
            if (samples[i] > max_val) max_val = samples[i];
        }
        
        // Ensure we have a non-zero range to avoid division by zero
        if (max_val == min_val) {
            max_val = min_val + 1;
        }
        
        printf("\nSamples %d - %d: [Min: %d, Max: %d]\n", offset, end-1, min_val, max_val);
        
        // Print a line representing the zero level
        printf("0 |");
        for (int i = 0; i < window_size && offset + i < sample_count; i++) {
            printf("-");
        }
        printf("|\n");
        
        // Print the waveform rows from top to bottom
        for (int row = 0; row < height; row++) {
            // Calculate the sample value this row represents
            float row_val = max_val - (row * (max_val - min_val) / (float)(height - 1));
            
            // Print row label (approximate value)
            printf("%5d |", (int)row_val);
            
            // Print waveform for this row
            for (int i = 0; i < window_size && offset + i < sample_count; i++) {
                float scaled_val = samples[offset + i];
                
                // If this row is closest to the sample value, print a marker
                if ((row == 0 && scaled_val >= row_val) || 
                    (row == height-1 && scaled_val <= row_val) ||
                    (row > 0 && row < height-1 && 
                     fabs(scaled_val - row_val) <= (max_val - min_val) / (2.0 * height))) {
                    printf("*");
                } else {
                    printf(" ");
                }
            }
            printf("|\n");
        }
        
        // Ask if user wants to continue or stop
        printf("\nPress Enter to continue, q to quit: ");
        char input = getchar();
        if (input == 'q' || input == 'Q') {
            break;
        }
    }
}

// Find negative-going dips in the audio
void find_negative_dips(int16_t* samples, int sample_count, int sample_rate) {
    printf("\nSearching for negative-going dips...\n");
    
    const int MIN_DIP_DEPTH = 2;  // Minimum dip depth to consider
    const int WINDOW_SIZE = 10;   // Samples to consider for a dip
    
    int dip_count = 0;
    int* dip_positions = (int*)malloc(1000 * sizeof(int)); // Store up to 1000 dip positions
    
    // Find dips
    for (int i = WINDOW_SIZE; i < sample_count - WINDOW_SIZE; i++) {
        // Check if current sample is significantly below the average of surrounding samples
        int before_avg = 0;
        int after_avg = 0;
        
        for (int j = 1; j <= WINDOW_SIZE; j++) {
            before_avg += samples[i-j];
            after_avg += samples[i+j];
        }
        
        before_avg /= WINDOW_SIZE;
        after_avg /= WINDOW_SIZE;
        
        int surrounding_avg = (before_avg + after_avg) / 2;
        
        // If current sample is a significant negative dip
        if (samples[i] < surrounding_avg - MIN_DIP_DEPTH && 
            samples[i] < before_avg && samples[i] < after_avg) {
            // Store this dip position
            if (dip_count < 1000) {
                dip_positions[dip_count++] = i;
            }
        }
    }
    
    printf("Found %d negative dips\n", dip_count);
    
    // If no dips found, exit
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
    
    printf("Dip spacing analysis:\n");
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
    double std_dev = 0;
    for (int i = 0; i < dip_count - 1; i++) {
        std_dev += pow(spacings[i] - avg_spacing, 2) / (dip_count - 1);
    }
    std_dev = sqrt(std_dev);
    
    double spacing_regularity = 1.0 - (std_dev / avg_spacing);
    printf("  Spacing regularity: %.2f (1.0 = perfectly regular, 0.0 = random)\n", 
           spacing_regularity);
    
    // If dips are fairly regular, this could indicate a systematic issue
    if (spacing_regularity > 0.5) {
        printf("\nANALYSIS: Regular negative dips detected at approximately %.2f Hz\n", 
               dip_frequency);
        printf("This pattern suggests potential issues with:\n");
        printf("1. Buffer underruns/overruns in the audio processing chain\n");
        printf("2. Phase issues between oscillators (if frequency is related to audio frequency)\n");
        printf("3. Potential clock synchronization issues\n");
        
        // Display a sample region with dips to visualize
        if (dip_count > 0) {
            int dip_pos = dip_positions[0];
            int region_start = dip_pos - 50;
            if (region_start < 0) region_start = 0;
            
            printf("\nExample region with dips (samples %d-%d):\n", 
                   region_start, region_start + 100);
                   
            for (int i = 0; i < 20; i++) {
                printf("%5d: ", region_start + i*5);
                for (int j = 0; j < 5 && region_start + i*5 + j < sample_count; j++) {
                    printf("%4d ", samples[region_start + i*5 + j]);
                    if (region_start + i*5 + j == dip_pos) {
                        printf("*");
                    }
                }
                printf("\n");
            }
        }
    }
    
    free(dip_positions);
    free(spacings);
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
    
    // Find negative dips in the audio
    find_negative_dips(samples, sample_count, sample_rate);
    
    // Visualize a small segment of the waveform
    visualize_waveform_ascii(samples, sample_count, 100, 15);
    
    // Free memory and exit
    free(samples);
    return 0;
}