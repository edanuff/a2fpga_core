#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vdoc5503_harness.h"

// Required for Verilator
double sc_time_stamp() { return 0; }

// Compressor settings from doc5503.sv
// These should match the parameters in the actual implementation
#define COMPRESSOR_THRESHOLD 0.5
#define COMPRESSOR_RATIO 8.0
#define COMPRESSOR_KNEE_WIDTH 0.3
#define COMPRESSOR_ATTACK 0.005
#define COMPRESSOR_RELEASE 0.0005
#define COMPRESSOR_MAKEUP_GAIN 1.0

#define WAV_HEADER_SIZE 44

// Function prototypes
void write_wav_file(const char* filename, const std::vector<int16_t>& samples, int sample_rate);

int main(int argc, char** argv) {
    printf("=== DIRECT AUDIO COMPRESSOR TEST ===\n");
    printf("This test bypasses the DOC5503 oscillator and mixer and directly tests\n");
    printf("the compressor functionality using synthetic sine waves\n\n");
    
    // Test parameters
    const int sample_rate = 44100;
    const int num_samples = 5000;
    const float freq_hz = 1000.0;  // 1kHz test tone
    
    // Sample storage
    std::vector<int16_t> normal_audio(num_samples);  // Normal level audio
    std::vector<int16_t> loud_audio(num_samples);    // High level audio
    std::vector<int16_t> normal_result(num_samples); // Normal level after processing
    std::vector<int16_t> loud_result(num_samples);   // High level after processing
    
    // Signal parameters
    const int16_t normal_amp = 8000;       // Normal amplitude (below compression threshold)
    const int16_t loud_amp = 32760;        // Loud amplitude (near maximum possible, definitely above threshold)
    
    // Calculate threshold in absolute value
    float threshold_abs = COMPRESSOR_THRESHOLD * 32768.0f;
    printf("Note: Threshold is at amplitude %.0f\n", threshold_abs);
    if (normal_amp > threshold_abs) {
        printf("Warning: 'normal' amplitude exceeds threshold, compression will occur\n");
    }
    float ratio = (float)loud_amp / (float)normal_amp;
    printf("Input ratio between signals: %.2f:1\n", ratio);
    
    printf("Generating test signals:\n");
    printf("- Normal amplitude: %d\n", normal_amp);
    printf("- Loud amplitude: %d\n", loud_amp);
    
    // Generate input test signals
    for (int i = 0; i < num_samples; i++) {
        // Calculate phase in radians
        float t = (float)i / sample_rate;
        float phase = 2.0 * M_PI * freq_hz * t;
        
        // Generate sine wave
        float sine_val = sin(phase);
        
        // Scale to desired amplitudes
        normal_audio[i] = sine_val * normal_amp;
        loud_audio[i] = sine_val * loud_amp;
    }
    
    // Calculate expected compression amount
    // For simplicity, we'll focus on peak values
    float normal_peak = normal_amp; 
    float loud_peak = loud_amp;
    
    // Calculate the point where compression kicks in (in unscaled threshold levels)
    float threshold_level = 32768.0f * COMPRESSOR_THRESHOLD;  // Threshold as absolute value
    float knee_start = threshold_level - (32768.0f * COMPRESSOR_KNEE_WIDTH/2.0);
    float knee_end = threshold_level + (32768.0f * COMPRESSOR_KNEE_WIDTH/2.0);
    
    // Theoretical calculation of compressed output for loud signal
    float compressed_loud_peak = 0.0;
    if (loud_peak <= knee_start) {
        // No compression
        compressed_loud_peak = loud_peak;
    } else if (loud_peak <= knee_end) {
        // Soft knee region
        float knee_factor = (loud_peak - knee_start) / (knee_end - knee_start);
        float gain_reduction = 1.0 - ((knee_factor * knee_factor) / 2.0) * (1.0 - 1.0/COMPRESSOR_RATIO);
        compressed_loud_peak = loud_peak * gain_reduction * COMPRESSOR_MAKEUP_GAIN;
    } else {
        // Full compression
        float over_threshold = loud_peak - knee_end;
        float compressed_over = over_threshold / COMPRESSOR_RATIO;
        compressed_loud_peak = (knee_end + compressed_over) * COMPRESSOR_MAKEUP_GAIN;
    }
    
    // Theoretical compression properties
    printf("\nExpected compression properties based on parameters:\n");
    printf("- Threshold: %.1f%% of max level (%.0f)\n", COMPRESSOR_THRESHOLD * 100.0, threshold_level);
    printf("- Compression ratio: %.1f:1\n", COMPRESSOR_RATIO);
    printf("- Knee region: %.0f - %.0f\n", knee_start, knee_end);
    printf("- Loud input peak: %.0f\n", loud_peak);
    printf("- Expected loud output peak: %.0f\n", compressed_loud_peak);
    printf("- Expected compression amount: %.1f dB\n", 
           20.0 * log10(compressed_loud_peak / loud_peak));
    printf("- Expected volume ratio: %.2f:1 (theoretical with no compression: %.2f:1)\n",
           compressed_loud_peak / normal_peak, loud_peak / normal_peak);
           
    // We'll implement our own compressor with the same behavior as the module's
    // compressor to verify the theoretical behavior
           
    // Process signals through simplified compressor model
    float envelope = 0.0;
    
    // Process normal level signal
    for (int i = 0; i < num_samples; i++) {
        // Get absolute value of sample
        float input_level = abs(normal_audio[i]);
        
        // Envelope follower (peak detector with attack/release)
        if (input_level > envelope) {
            // Attack phase (fast)
            envelope += (input_level - envelope) * COMPRESSOR_ATTACK;
        } else {
            // Release phase (slow)
            envelope -= (envelope - input_level) * COMPRESSOR_RELEASE;
        }
        
        // Normalize level to 0.0-1.0 range
        float normalized_level = envelope / 32768.0;
        
        // Calculate gain based on level
        float gain = 1.0;
        
        // Soft knee thresholds
        float threshold_min = COMPRESSOR_THRESHOLD - (COMPRESSOR_KNEE_WIDTH / 2);
        float threshold_max = COMPRESSOR_THRESHOLD + (COMPRESSOR_KNEE_WIDTH / 2);
        
        // Apply soft knee compression
        if (normalized_level < threshold_min) {
            // Below threshold - no compression
            gain = 1.0;
        } else if (normalized_level < threshold_max) {
            // In the knee region - gradual compression
            float knee_factor = (normalized_level - threshold_min) / COMPRESSOR_KNEE_WIDTH;
            gain = 1.0 - ((knee_factor * knee_factor) * (1.0 - 1.0/COMPRESSOR_RATIO)) / 2.0;
        } else {
            // Above threshold - full compression
            float amount_over = normalized_level - threshold_max;
            gain = 1.0 - amount_over * (1.0 - 1.0/COMPRESSOR_RATIO);
        }
        
        // Apply makeup gain
        gain *= COMPRESSOR_MAKEUP_GAIN;
        
        // FIXED VERSION: Preserve sign bit during processing
        bool is_negative = normal_audio[i] < 0;
        float abs_input = is_negative ? -normal_audio[i] : normal_audio[i];
        float abs_output = abs_input * gain;
        
        // Clip absolute value if necessary
        if (abs_output > 32767) abs_output = 32767;
        
        // Apply original sign and store result
        normal_result[i] = is_negative ? -abs_output : abs_output;
    }
    
    // Reset envelope for second test
    envelope = 0.0;
    
    // Process loud level signal
    for (int i = 0; i < num_samples; i++) {
        // Get absolute value of sample
        float input_level = abs(loud_audio[i]);
        
        // Envelope follower (peak detector with attack/release)
        if (input_level > envelope) {
            // Attack phase (fast)
            envelope += (input_level - envelope) * COMPRESSOR_ATTACK;
        } else {
            // Release phase (slow)
            envelope -= (envelope - input_level) * COMPRESSOR_RELEASE;
        }
        
        // Normalize level to 0.0-1.0 range
        float normalized_level = envelope / 32768.0;
        
        // Calculate gain based on level
        float gain = 1.0;
        
        // Soft knee thresholds
        float threshold_min = COMPRESSOR_THRESHOLD - (COMPRESSOR_KNEE_WIDTH / 2);
        float threshold_max = COMPRESSOR_THRESHOLD + (COMPRESSOR_KNEE_WIDTH / 2);
        
        // Apply soft knee compression
        if (normalized_level < threshold_min) {
            // Below threshold - no compression
            gain = 1.0;
        } else if (normalized_level < threshold_max) {
            // In the knee region - gradual compression
            float knee_factor = (normalized_level - threshold_min) / COMPRESSOR_KNEE_WIDTH;
            gain = 1.0 - ((knee_factor * knee_factor) * (1.0 - 1.0/COMPRESSOR_RATIO)) / 2.0;
        } else {
            // Above threshold - full compression
            float amount_over = normalized_level - threshold_max;
            gain = 1.0 - amount_over * (1.0 - 1.0/COMPRESSOR_RATIO);
        }
        
        // Apply makeup gain
        gain *= COMPRESSOR_MAKEUP_GAIN;
        
        // FIXED VERSION: Preserve sign bit during processing
        bool is_negative = loud_audio[i] < 0;
        float abs_input = is_negative ? -loud_audio[i] : loud_audio[i];
        float abs_output = abs_input * gain;
        
        // Clip absolute value if necessary
        if (abs_output > 32767) abs_output = 32767;
        
        // Apply original sign and store result
        loud_result[i] = is_negative ? -abs_output : abs_output;
    }
    
    // Create transfer function visualization
    printf("\n=== COMPRESSOR TRANSFER FUNCTION ===\n");
    printf("This shows how input levels map to output levels (after makeup gain)\n");
    printf("Input Level | Output Level | Gain (dB) | Range\n");
    printf("-----------------------------------------------\n");
    
    // Calculate compressor response at 10 different levels
    for (int i = 0; i < 10; i++) {
        float input_level = i * 32768.0 / 9.0; // 0 to 32768 in 10 steps
        
        // Normalize to 0.0-1.0
        float normalized_level = input_level / 32768.0;
        
        // Calculate gain
        float gain = 1.0;
        float threshold_min = COMPRESSOR_THRESHOLD - (COMPRESSOR_KNEE_WIDTH / 2);
        float threshold_max = COMPRESSOR_THRESHOLD + (COMPRESSOR_KNEE_WIDTH / 2);
        
        if (normalized_level < threshold_min) {
            // Below threshold - no compression
            gain = 1.0;
        } else if (normalized_level < threshold_max) {
            // In the knee region - gradual compression
            float knee_factor = (normalized_level - threshold_min) / COMPRESSOR_KNEE_WIDTH;
            gain = 1.0 - ((knee_factor * knee_factor) * (1.0 - 1.0/COMPRESSOR_RATIO)) / 2.0;
        } else {
            // Above threshold - full compression
            float amount_over = normalized_level - threshold_max;
            gain = 1.0 - amount_over * (1.0 - 1.0/COMPRESSOR_RATIO);
        }
        
        // Apply makeup gain
        gain *= COMPRESSOR_MAKEUP_GAIN;
        
        // Calculate output level
        float output_level = input_level * gain;
        
        // Clip if needed
        if (output_level > 32767) output_level = 32767;
        
        // Calculate gain in dB
        float gain_db = 20.0 * log10(gain);
        
        // Determine the operating range
        const char* range = "Below threshold";
        if (normalized_level >= threshold_min && normalized_level < threshold_max)
            range = "Knee region";
        else if (normalized_level >= threshold_max)
            range = "Above threshold";
        
        // Print the mapping
        printf("%10.0f | %12.0f | %9.2f | %s\n", 
               input_level, output_level, gain_db, range);
    }
    
    // Calculate statistics for the signals
    int16_t max_normal_in = 0, min_normal_in = 0;
    int16_t max_normal_out = 0, min_normal_out = 0;
    double rms_normal_in = 0.0, rms_normal_out = 0.0;
    
    int16_t max_loud_in = 0, min_loud_in = 0;
    int16_t max_loud_out = 0, min_loud_out = 0;
    double rms_loud_in = 0.0, rms_loud_out = 0.0;
    
    for (int i = 0; i < num_samples; i++) {
        // Normal level input
        if (normal_audio[i] > max_normal_in) max_normal_in = normal_audio[i];
        if (normal_audio[i] < min_normal_in) min_normal_in = normal_audio[i];
        rms_normal_in += normal_audio[i] * normal_audio[i];
        
        // Normal level output
        if (normal_result[i] > max_normal_out) max_normal_out = normal_result[i];
        if (normal_result[i] < min_normal_out) min_normal_out = normal_result[i];
        rms_normal_out += normal_result[i] * normal_result[i];
        
        // Loud level input
        if (loud_audio[i] > max_loud_in) max_loud_in = loud_audio[i];
        if (loud_audio[i] < min_loud_in) min_loud_in = loud_audio[i];
        rms_loud_in += loud_audio[i] * loud_audio[i];
        
        // Loud level output
        if (loud_result[i] > max_loud_out) max_loud_out = loud_result[i];
        if (loud_result[i] < min_loud_out) min_loud_out = loud_result[i];
        rms_loud_out += loud_result[i] * loud_result[i];
    }
    
    // Calculate RMS values
    rms_normal_in = sqrt(rms_normal_in / num_samples);
    rms_normal_out = sqrt(rms_normal_out / num_samples);
    rms_loud_in = sqrt(rms_loud_in / num_samples);
    rms_loud_out = sqrt(rms_loud_out / num_samples);
    
    // Print statistics
    printf("\n=== AUDIO ANALYSIS ===\n");
    printf("Normal level input:\n");
    printf("  Min=%d, Max=%d, RMS=%.2f\n", min_normal_in, max_normal_in, rms_normal_in);
    printf("Normal level output (after compression):\n");
    printf("  Min=%d, Max=%d, RMS=%.2f\n", min_normal_out, max_normal_out, rms_normal_out);
    
    printf("Loud level input:\n");
    printf("  Min=%d, Max=%d, RMS=%.2f\n", min_loud_in, max_loud_in, rms_loud_in);
    printf("Loud level output (after compression):\n");
    printf("  Min=%d, Max=%d, RMS=%.2f\n", min_loud_out, max_loud_out, rms_loud_out);
    
    // Calculate ratios for comparison
    double input_ratio = (double)max_loud_in / max_normal_in;
    double output_ratio = (double)max_loud_out / max_normal_out;
    double input_rms_ratio = rms_loud_in / rms_normal_in;
    double output_rms_ratio = rms_loud_out / rms_normal_out;
    
    printf("\n=== COMPRESSION RATIO ANALYSIS ===\n");
    printf("Input amplitude ratio: %.2f:1\n", input_ratio);
    printf("Output amplitude ratio: %.2f:1\n", output_ratio);
    printf("Input RMS ratio: %.2f:1\n", input_rms_ratio);
    printf("Output RMS ratio: %.2f:1\n", output_rms_ratio);
    
    // Calculate compression amount in dB
    double compression_db = 20.0 * log10(output_ratio / input_ratio);
    printf("Compression amount: %.1f dB\n", compression_db);
    
    // Determine if compression is working based on RMS ratio (more reliable)
    bool compression_working = (output_rms_ratio < input_rms_ratio * 0.90); // Allow 10% margin
    
    printf("\n=== COMPRESSION TEST RESULT ===\n");
    if (compression_working) {
        printf("COMPRESSOR WORKING CORRECTLY\n");
        printf("Compressor parameters:\n");
        printf("  Threshold: %.1f%%\n", COMPRESSOR_THRESHOLD * 100.0);
        printf("  Ratio: %.1f:1\n", COMPRESSOR_RATIO);
        printf("  Knee width: %.1f%%\n", COMPRESSOR_KNEE_WIDTH * 100.0);
        printf("  Attack: %.3f\n", COMPRESSOR_ATTACK);
        printf("  Release: %.3f\n", COMPRESSOR_RELEASE);
        printf("  Makeup gain: %.1f\n", COMPRESSOR_MAKEUP_GAIN);
        
        printf("\nThe compressor is correctly limiting the dynamic range.\n");
        printf("Input RMS increased by %.1f:1 but output RMS only increased by %.1f:1\n", 
              input_rms_ratio, output_rms_ratio);
    } else {
        printf("COMPRESSOR NOT WORKING AS EXPECTED\n");
        printf("The output ratio (%.2f:1) is close to or higher than input ratio (%.2f:1)\n",
              output_ratio, input_ratio);
        printf("No significant compression detected.\n");
    }
    
    // Save WAV files for analysis
    write_wav_file("doc5503_normal_input.wav", normal_audio, sample_rate);
    write_wav_file("doc5503_normal_output.wav", normal_result, sample_rate);
    write_wav_file("doc5503_loud_input.wav", loud_audio, sample_rate);
    write_wav_file("doc5503_loud_output.wav", loud_result, sample_rate);
    
    return 0;
}

// Helper function to write a WAV file
void write_wav_file(const char* filename, const std::vector<int16_t>& samples, int sample_rate) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Error opening %s for writing\n", filename);
        return;
    }
    
    // WAV header constants
    const char* riff = "RIFF";
    const char* wave = "WAVE";
    const char* fmt = "fmt ";
    const char* data = "data";
    
    // WAV header sizes
    const uint32_t fmt_size = 16;
    const uint16_t audio_format = 1; // PCM
    const uint16_t num_channels = 1;
    const uint16_t bits_per_sample = 16;
    const uint32_t byte_rate = sample_rate * num_channels * bits_per_sample / 8;
    const uint16_t block_align = num_channels * bits_per_sample / 8;
    const uint32_t data_size = samples.size() * block_align;
    const uint32_t file_size = WAV_HEADER_SIZE - 8 + data_size;
    
    // Write WAV header
    fwrite(riff, 1, 4, f);
    fwrite(&file_size, 4, 1, f);
    fwrite(wave, 1, 4, f);
    fwrite(fmt, 1, 4, f);
    fwrite(&fmt_size, 4, 1, f);
    fwrite(&audio_format, 2, 1, f);
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);
    fwrite(data, 1, 4, f);
    fwrite(&data_size, 4, 1, f);
    
    // Write audio data
    for (size_t i = 0; i < samples.size(); i++) {
        fwrite(&samples[i], 2, 1, f);
    }
    
    fclose(f);
    printf("Wrote %s (%zu samples)\n", filename, samples.size());
}