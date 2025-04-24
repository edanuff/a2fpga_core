#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void dump_header_info(const unsigned char* header) {
    printf("RIFF header: %.4s\n", header);
    printf("WAVE header: %.4s\n", header + 8);
    
    // Extract file size
    uint32_t file_size = *(uint32_t*)(header + 4);
    printf("File size: %u bytes\n", file_size);
    
    // Find 'fmt ' chunk
    const unsigned char* fmt_ptr = header + 12; // Skip RIFF+size+WAVE
    while (strncmp((const char*)fmt_ptr, "fmt ", 4) != 0) {
        fmt_ptr += 8 + *(uint32_t*)(fmt_ptr + 4); // Skip to next chunk
        if (fmt_ptr >= header + 44) {
            printf("Error: fmt chunk not found\n");
            return;
        }
    }
    
    // Parse fmt chunk
    uint16_t format = *(uint16_t*)(fmt_ptr + 8);
    uint16_t channels = *(uint16_t*)(fmt_ptr + 10);
    uint32_t sample_rate = *(uint32_t*)(fmt_ptr + 12);
    uint32_t byte_rate = *(uint32_t*)(fmt_ptr + 16);
    uint16_t block_align = *(uint16_t*)(fmt_ptr + 20);
    uint16_t bits_per_sample = *(uint16_t*)(fmt_ptr + 22);
    
    printf("Format: %u (1=PCM)\n", format);
    printf("Channels: %u\n", channels);
    printf("Sample rate: %u Hz\n", sample_rate);
    printf("Byte rate: %u bytes/sec\n", byte_rate);
    printf("Block align: %u bytes\n", block_align);
    printf("Bits per sample: %u\n", bits_per_sample);
    
    // Find 'data' chunk
    const unsigned char* data_ptr = fmt_ptr + 8 + *(uint32_t*)(fmt_ptr + 4);
    while (strncmp((const char*)data_ptr, "data", 4) != 0) {
        data_ptr += 8 + *(uint32_t*)(data_ptr + 4); // Skip to next chunk
        if (data_ptr >= header + 100) { // Allow for extra chunks
            printf("Error: data chunk not found\n");
            return;
        }
    }
    
    uint32_t data_size = *(uint32_t*)(data_ptr + 4);
    printf("Data chunk size: %u bytes\n", data_size);
    printf("Sample count: %u (%0.2f seconds)\n", 
           data_size / (channels * bits_per_sample / 8),
           (float)(data_size / (channels * bits_per_sample / 8)) / sample_rate);
}

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
    
    // Read first 1KB of the file
    unsigned char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), f);
    printf("Read %zu bytes from file\n", bytes_read);
    
    // Display file header info
    if (bytes_read >= 44) {
        dump_header_info(buffer);
    } else {
        printf("Error: File too small to be a valid WAV\n");
    }
    
    // Show first few sample values
    if (bytes_read > 44) {
        printf("\nFirst 20 samples (16-bit):\n");
        for (int i = 0; i < 20 && (44 + i*2 + 1 < bytes_read); i++) {
            int16_t sample = buffer[44 + i*2] | (buffer[44 + i*2 + 1] << 8);
            printf("%d: %d\n", i, sample);
        }
    }
    
    // Read and show samples from middle of file
    fseek(f, 1000, SEEK_SET);
    bytes_read = fread(buffer, 1, sizeof(buffer), f);
    
    if (bytes_read > 0) {
        printf("\nSamples from middle of file (offset 1000):\n");
        for (int i = 0; i < 20 && (i*2 + 1 < bytes_read); i++) {
            int16_t sample = buffer[i*2] | (buffer[i*2 + 1] << 8);
            printf("%d: %d\n", i, sample);
        }
    }
    
    // Close file and exit
    fclose(f);
    return 0;
}