#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm> // for std::max

// Direct simulation of the DOC5503 DSP multiplier to test polarity behavior with different volumes

// This test directly simulates the volume multiplication stage in doc5503.sv:
// automatic logic signed [7:0] data_w = wds_w ^ 8'h80;       // convert waveform data to signed
// automatic logic signed [8:0] vol_s = {1'b0, vol_w};        // convert volume to signed
// output_r <= data_w * vol_s;                                // output is waveform data * volume

// Function to run the DSP multiplication using the current implementation
int16_t simulate_dsp_multiply(uint8_t wave_data, uint8_t volume) {
    // Convert wave data from unsigned (0-255) to signed (-128 to 127)
    int8_t signed_data = wave_data ^ 0x80;
    
    // Convert volume to 9-bit signed with high bit forced to 0
    int16_t signed_vol = (int16_t)((uint16_t)volume | 0x0000);  // Ensure 9-bit representation with high bit 0
    
    // Perform signed multiplication
    int16_t result = signed_data * signed_vol;
    
    return result;
}

// WAV file output functions - simplified for direct calculation test
void write_wav_file(const char* filename, int16_t* samples, int sample_count, int sample_rate) {
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
    uint32_t data_size = (uint32_t)sample_count * 2; // 2 bytes per sample
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
    fwrite(samples, sizeof(int16_t), sample_count, f);
    
    fclose(f);
    printf("Wrote %d samples to %s\n", sample_count, filename);
}

// Generate a sine wave table
void generate_sine_wavetable(uint8_t* wavetable, int length) {
    for (int i = 0; i < length; i++) {
        double angle = (double)i / length * 2.0 * M_PI;
        double value = sin(angle);
        // Map [-1, 1] to [0, 255] with 128 as the center point
        wavetable[i] = (uint8_t)(value * 127.0 + 128.0);
    }
}

// Analyze audio statistics with polarity information
void analyze_audio_stats(int16_t* samples, int sample_count, const char* label) {
    int16_t min_val = 32767;
    int16_t max_val = -32768;
    double avg_val = 0.0;
    double rms_val = 0.0;
    int positive_count = 0;
    int negative_count = 0;
    int zero_count = 0;
    
    for (int i = 0; i < sample_count; i++) {
        int16_t sample = samples[i];
        if (sample < min_val) min_val = sample;
        if (sample > max_val) max_val = sample;
        avg_val += sample;
        
        if (sample > 0) positive_count++;
        else if (sample < 0) negative_count++;
        else zero_count++;
        
        double sample_normalized = sample / 32768.0;
        rms_val += sample_normalized * sample_normalized;
    }
    
    avg_val /= sample_count;
    rms_val = sqrt(rms_val / sample_count);
    
    printf("\n=== %s ===\n", label);
    printf("Total samples: %d\n", sample_count);
    printf("Range: min=%d, max=%d (span=%d)\n", min_val, max_val, max_val - min_val);
    printf("Average value: %.2f\n", avg_val);
    printf("Polarity count: positive=%d (%.1f%%), negative=%d (%.1f%%), zero=%d (%.1f%%)\n",
        positive_count, (positive_count * 100.0) / sample_count,
        negative_count, (negative_count * 100.0) / sample_count,
        zero_count, (zero_count * 100.0) / sample_count);
    
    // Polarity analysis
    if (positive_count > 0 && negative_count > 0) {
        printf("BIPOLAR WAVEFORM: Both positive and negative samples detected\n");
    } else if (positive_count > 0) {
        printf("POSITIVE-ONLY WAVEFORM: Only positive samples detected\n");
    } else if (negative_count > 0) {
        printf("NEGATIVE-ONLY WAVEFORM: Only negative samples detected\n");
    } else {
        printf("SILENT WAVEFORM: Only zero samples detected\n");
    }
    
    // RMS and peak statistics
    printf("RMS level: %.4f (%.2f dB)\n", rms_val, 20.0 * log10(rms_val));
    double peak_level_db = 20.0 * log10((double)std::max(abs(min_val), abs(max_val)) / 32768.0);
    printf("Peak level: %.2f dB\n", peak_level_db);
}

int main() {
    const int NUM_SAMPLES = 1000;
    const int SAMPLE_RATE = 44100;
    const int WAVETABLE_SIZE = 256;
    
    // Create a sine wavetable
    uint8_t sine_wavetable[WAVETABLE_SIZE];
    generate_sine_wavetable(sine_wavetable, WAVETABLE_SIZE);
    
    printf("Sine wavetable values (first and key points):\n");
    printf("  0: 0x%02X (center)\n", sine_wavetable[0]);
    printf("  64: 0x%02X (peak)\n", sine_wavetable[64]);
    printf("  128: 0x%02X (center)\n", sine_wavetable[128]);
    printf("  192: 0x%02X (valley)\n", sine_wavetable[192]);
    
    // Test with two different volume levels
    const uint8_t LOW_VOLUME = 64;    // Below 128 threshold
    const uint8_t HIGH_VOLUME = 200;  // Above 128 threshold
    
    // Generate output samples at low volume
    int16_t low_volume_samples[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++) {
        int index = (i * WAVETABLE_SIZE) / NUM_SAMPLES;
        low_volume_samples[i] = simulate_dsp_multiply(sine_wavetable[index], LOW_VOLUME);
    }
    
    // Generate output samples at high volume
    int16_t high_volume_samples[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++) {
        int index = (i * WAVETABLE_SIZE) / NUM_SAMPLES;
        high_volume_samples[i] = simulate_dsp_multiply(sine_wavetable[index], HIGH_VOLUME);
    }
    
    // Analyze and write the output
    analyze_audio_stats(low_volume_samples, NUM_SAMPLES, "Low Volume (64) Waveform");
    write_wav_file("doc_output_vol_64.wav", low_volume_samples, NUM_SAMPLES, SAMPLE_RATE);
    
    analyze_audio_stats(high_volume_samples, NUM_SAMPLES, "High Volume (200) Waveform");
    write_wav_file("doc_output_vol_200.wav", high_volume_samples, NUM_SAMPLES, SAMPLE_RATE);
    
    // Compare polarities
    printf("\n===== POLARITY COMPARISON =====\n");
    
    // Find peak values in each sample set for comparison
    int16_t peak1_pos = 0, peak1_neg = 0;
    int16_t peak2_pos = 0, peak2_neg = 0;
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (low_volume_samples[i] > peak1_pos) peak1_pos = low_volume_samples[i];
        if (low_volume_samples[i] < peak1_neg) peak1_neg = low_volume_samples[i];
        if (high_volume_samples[i] > peak2_pos) peak2_pos = high_volume_samples[i];
        if (high_volume_samples[i] < peak2_neg) peak2_neg = high_volume_samples[i];
    }
    
    printf("Low volume peaks: positive=%d, negative=%d\n", peak1_pos, peak1_neg);
    printf("High volume peaks: positive=%d, negative=%d\n", peak2_pos, peak2_neg);
    
    // Determine if the two waveforms have consistent polarity
    bool low_vol_bipolar = (peak1_pos > 10 && peak1_neg < -10);
    bool high_vol_bipolar = (peak2_pos > 10 && peak2_neg < -10);
    
    if (low_vol_bipolar && high_vol_bipolar) {
        printf("CONSISTENT BIPOLAR BEHAVIOR: Both volume levels produce bipolar waveforms\n");
        printf("The polarity fix appears to be working correctly!\n");
    } else if (!low_vol_bipolar && !high_vol_bipolar) {
        if ((peak1_pos > 0 && peak2_pos > 0) || (peak1_neg < 0 && peak2_neg < 0)) {
            printf("CONSISTENT UNIPOLAR BEHAVIOR: Both volume levels produce similar unipolar waveforms\n");
            printf("The polarity fix appears to be working correctly, but waveforms aren't bipolar\n");
        } else {
            printf("INCONSISTENT POLARITY: Volume levels produce opposite unipolar waveforms\n");
            printf("The polarity fix may not be working correctly.\n");
        }
    } else {
        printf("MIXED POLARITY BEHAVIOR: One volume level is bipolar, the other is unipolar\n");
        printf("The polarity fix may be partially working, but there are still issues.\n");
    }
    
    return 0;
}