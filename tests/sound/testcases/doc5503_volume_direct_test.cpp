#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Direct simulation of the volume scaling calculation

int main() {
    printf("Testing DOC5503 Volume Scaling Algorithm\n\n");
    
    // Test with different waveform values (centered around 0x80)
    const uint8_t test_wave_values[] = {
        0x00,  // Minimum
        0x40,  // Low negative
        0x80,  // Zero level
        0xC0,  // Low positive
        0xFF   // Maximum
    };
    
    // Test with different volume levels
    const uint8_t test_volumes[] = {
        32,    // Low 
        64,    // Medium-low
        128,   // Medium
        192,   // Medium-high
        255    // Maximum
    };
    
    // Print header
    printf("%-10s | ", "Wave\\Vol");
    for (int v = 0; v < sizeof(test_volumes)/sizeof(test_volumes[0]); v++) {
        printf("%-7d | ", test_volumes[v]);
    }
    printf("\n");
    printf("-----------+");
    for (int v = 0; v < sizeof(test_volumes)/sizeof(test_volumes[0]); v++) {
        printf("---------+");
    }
    printf("\n");
    
    // Previous algorithm (using XOR 0x80 and right-shifting by variable amounts)
    printf("\nPrevious Algorithm (XOR 0x80, Variable Shift):\n");
    
    for (int w = 0; w < sizeof(test_wave_values)/sizeof(test_wave_values[0]); w++) {
        uint8_t wave_val = test_wave_values[w];
        printf("0x%-8X | ", wave_val);
        
        for (int v = 0; v < sizeof(test_volumes)/sizeof(test_volumes[0]); v++) {
            uint8_t vol = test_volumes[v];
            
            // Convert waveform to signed (-128 to 127)
            int8_t data_signed = wave_val ^ 0x80;
            
            // Old scaling with right-shift by 2
            uint8_t vol_scaled = (vol >> 2);
            
            // Signed multiplication
            int16_t output = data_signed * vol_scaled;
            
            printf("%-7d | ", output);
        }
        printf("\n");
    }
    
    // New algorithm (full 8-bit volume, consistent scaling with different shift values) 
    printf("\nNew Algorithm (XOR 0x80, Full Range, Shift 2):\n");
    
    for (int w = 0; w < sizeof(test_wave_values)/sizeof(test_wave_values[0]); w++) {
        uint8_t wave_val = test_wave_values[w];
        printf("0x%-8X | ", wave_val);
        
        for (int v = 0; v < sizeof(test_volumes)/sizeof(test_volumes[0]); v++) {
            uint8_t vol = test_volumes[v];
            
            // Convert waveform to signed (-128 to 127)
            int8_t data_signed = wave_val ^ 0x80;
            
            // Signed multiplication with full 8-bit volume
            int16_t product = data_signed * vol;
            
            // Scaling with right shift of 2 bits (divide by 4)
            int16_t output = product >> 2;
            
            printf("%-7d | ", output);
        }
        printf("\n");
    }
    
    // Range analysis for shift by 2
    printf("\nOutput Range Analysis (Shift 2):\n");
    printf("Maximum positive data (0xFF) at full volume (255): %d\n", 
           (int16_t)((0xFF ^ 0x80) * 255) >> 2);
    printf("Minimum negative data (0x00) at full volume (255): %d\n",
           (int16_t)((0x00 ^ 0x80) * 255) >> 2);
    
    // Calculate percentage of full scale (-32768 to 32767)
    int16_t max_pos = (int16_t)((0xFF ^ 0x80) * 255) >> 2;
    int16_t max_neg = (int16_t)((0x00 ^ 0x80) * 255) >> 2;
    
    printf("Percentage of full scale (positive): %.2f%%\n", (double)max_pos / 32767.0 * 100.0);
    printf("Percentage of full scale (negative): %.2f%%\n", (double)max_neg / -32768.0 * 100.0);
    
    return 0;
}