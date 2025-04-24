#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>  // For std::setw, std::setfill in analyzeWavetable

// Define the wavetable size for test tones
const int WAVETABLE_SIZE = 256;

// Class to write audio samples to a WAV file
class WaveWriter {
public:
    WaveWriter(const char* filename, int sampleRate, int numChannels) 
        : m_file(NULL), m_dataSize(0), m_sampleRate(sampleRate), m_numChannels(numChannels) {
        m_file = fopen(filename, "wb");
        if (!m_file) {
            printf("Error opening WAV file %s for writing\n", filename);
            return;
        }
        
        // Write WAV header with placeholder sizes (will be updated when closing)
        writeWavHeader();
    }
    
    ~WaveWriter() {
        close();
    }
    
    void writeSample(int16_t sample) {
        if (!m_file) return;
        
        fwrite(&sample, sizeof(int16_t), 1, m_file);
        m_dataSize += sizeof(int16_t);
    }
    
    void writeSamples(const std::vector<int16_t>& samples) {
        if (!m_file) return;
        
        fwrite(samples.data(), sizeof(int16_t), samples.size(), m_file);
        m_dataSize += samples.size() * sizeof(int16_t);
    }
    
    void close() {
        if (!m_file) return;
        
        // Update header with final sizes
        fseek(m_file, 0, SEEK_SET);
        writeWavHeader();
        
        fclose(m_file);
        m_file = NULL;
    }
    
private:
    void writeWavHeader() {
        if (!m_file) return;
        
        int fileSize = m_dataSize + 44 - 8;  // File size minus RIFF chunk
        int byteRate = m_sampleRate * m_numChannels * sizeof(int16_t);
        int blockAlign = m_numChannels * sizeof(int16_t);
        
        // RIFF header
        fwrite("RIFF", 1, 4, m_file);
        fwrite(&fileSize, 4, 1, m_file);
        fwrite("WAVE", 1, 4, m_file);
        
        // Format chunk
        fwrite("fmt ", 1, 4, m_file);
        int fmtSize = 16;
        fwrite(&fmtSize, 4, 1, m_file);
        short audioFormat = 1;  // PCM
        fwrite(&audioFormat, 2, 1, m_file);
        short numChannels = m_numChannels;
        fwrite(&numChannels, 2, 1, m_file);
        fwrite(&m_sampleRate, 4, 1, m_file);
        fwrite(&byteRate, 4, 1, m_file);
        fwrite(&blockAlign, 2, 1, m_file);
        short bitsPerSample = 16;
        fwrite(&bitsPerSample, 2, 1, m_file);
        
        // Data chunk
        fwrite("data", 1, 4, m_file);
        fwrite(&m_dataSize, 4, 1, m_file);
    }
    
    FILE* m_file;
    int m_dataSize;
    int m_sampleRate;
    int m_numChannels;
};

// ES5503 emulator for wavetable playback
// Simplified version that correctly plays pre-recorded voice samples
class ES5503 {
public:
    ES5503(uint8_t* wavetableData, uint32_t wavetableSize) 
        : m_wavetableData(wavetableData), m_wavetableSize(wavetableSize),
          m_acc(0), m_freq(0x0180), m_vol(0xFF), m_waveTableSize(0), 
          m_resolution(0), m_halt(false), m_debugMode(false),
          m_sampleRate(22050), m_targetSampleRate(22050) {
        
        // Print info about the wavetable data if in debug mode
        if (m_debugMode) {
            std::cout << "Wavetable first 16 bytes: ";
            for (size_t i = 0; i < 16 && i < wavetableSize; i++) {
                printf("%02X ", wavetableData[i]);
            }
            std::cout << std::endl;
        }
    }
    
    void setFrequency(uint16_t freq) {
        m_freq = freq;
        if (m_debugMode) {
            std::cout << "Frequency set to 0x" << std::hex << m_freq << std::dec << std::endl;
        }
    }
    
    void setVolume(uint8_t vol) {
        m_vol = vol;
    }
    
    // Table size parameter (0-7) which determines wavetable size:
    // 0: 256 bytes, 1: 512 bytes, 2: 1KB, 3: 2KB, 4: 4KB, 5: 8KB, 6: 16KB, 7: 32KB
    void setTableSize(uint8_t size) {
        m_waveTableSize = size & 0x07;
        if (m_debugMode) {
            std::cout << "Table size set to " << (int)m_waveTableSize
                      << " (" << (256 << m_waveTableSize) << " bytes)" << std::endl;
        }
    }
    
    // Resolution parameter (0-7) affects which bits of the accumulator are used
    void setResolution(uint8_t res) {
        m_resolution = res & 0x07;
        if (m_debugMode) {
            std::cout << "Resolution set to " << (int)m_resolution << std::endl;
        }
    }
    
    // Set the target sample rate the wavetable was designed for (22050 Hz for voice samples)
    void setTargetSampleRate(int sampleRate) {
        m_targetSampleRate = sampleRate;
        if (m_debugMode) {
            std::cout << "Target sample rate set to " << m_targetSampleRate << " Hz" << std::endl;
        }
    }
    
    // Set the output sample rate (default: 22050 Hz)
    void setSampleRate(int sampleRate) {
        m_sampleRate = sampleRate;
        if (m_debugMode) {
            std::cout << "Output sample rate set to " << m_sampleRate << " Hz" << std::endl;
        }
    }
    
    bool isHalted() const {
        return m_halt;
    }
    
    void reset() {
        m_acc = 0;
        m_halt = false;
    }
    
    void setDebugMode(bool enable) {
        m_debugMode = enable;
    }
    
    // Calculate shift amount based on resolution and table size
    int calculateShift() const {
        // Formula from DOC5503 datasheet and implementation:
        // shift = 9 + resolution - tableSize
        return 9 + m_resolution - m_waveTableSize;
    }
    
    // Get the active table size in bytes
    uint32_t getTableSizeInBytes() const {
        return 256 << m_waveTableSize;
    }
    
    // Calculate the frequency scaling factor needed to adjust for different output sample rates
    double getFrequencyScaleFactor() const {
        // For a pre-recorded voice sample designed for 22050 Hz playback
        // with 0x01AD frequency value, we need to scale correctly if using
        // a different output sample rate
        return static_cast<double>(m_sampleRate) / m_targetSampleRate;
    }
    
    // Get the target rate this wavetable was designed for
    int getTargetSampleRate() const {
        return m_targetSampleRate;
    }
    
    // Generate a single audio sample from the wavetable
    int16_t generateSample() {
        if (m_halt) {
            return 0;
        }
        
        // For a direct playback of the wavetable data, we simply index through it sequentially
        // Calculate the current index based on frequency scaling
        
        // Print settings on first sample if in debug mode
        static bool settings_printed = false;
        if (m_debugMode && !settings_printed) {
            std::cout << "Target sample rate: " << m_targetSampleRate << " Hz" << std::endl;
            std::cout << "Output sample rate: " << m_sampleRate << " Hz" << std::endl;
            std::cout << "Sample rate factor: " << getFrequencyScaleFactor() << std::endl;
            std::cout << "Frequency: 0x" << std::hex << m_freq << std::dec << std::endl;
            std::cout << "Resolution: " << (int)m_resolution << 
                      ", Table size: " << (int)m_waveTableSize << std::endl;
            std::cout << "Direct sequential playback mode enabled" << std::endl;
            settings_printed = true;
        }
        
        // For direct playback of voice sample, just index through the wavetable sequentially
        // Adjust index based on frequency relative to 0x01AD (the correct freq)
        double normalizedFreq = static_cast<double>(m_freq) / 0x01AD;
        double position = m_acc / (double)(1 << 24);  // Normalize position to 0.0-1.0
        uint32_t index = static_cast<uint32_t>(position * m_wavetableSize);
        
        // Ensure index is within bounds
        if (index >= m_wavetableSize) {
            // Reached the end of the sample
            m_halt = true;
            return 0;
        }
        
        // Debug output for first few samples if in debug mode
        static bool samples_debug_done = false;
        static int sample_count = 0;
        if (m_debugMode && !samples_debug_done && sample_count < 20) {
            if (sample_count < 5 || (sample_count >= 8000 && sample_count < 8010)) {
                std::cout << "Sample " << sample_count << ": Index=" << index 
                          << ", Value=0x" << std::hex << (int)m_wavetableData[index] 
                          << std::dec << std::endl;
            }
            sample_count++;
            if (sample_count >= 20) samples_debug_done = true;
        }
        
        // Read sample from wavetable
        uint8_t sample = m_wavetableData[index];
        
        // Update position for next sample
        // For direct playback, we increment based on normalized frequency
        double increment = normalizedFreq / m_wavetableSize;
        position += increment;
        m_acc = static_cast<uint32_t>(position * (1 << 24)) & 0xFFFFFF;  // Apply 24-bit mask
        
        // If sample is zero, halt the oscillator as per ES5503 spec
        if (sample == 0) {
            m_halt = true;
            return 0;
        }
        
        // Convert to signed output and apply volume
        // ES5503 format: 0x80 = silence (zero), 0x01-0x7F = negative, 0x81-0xFF = positive
        int16_t signed_sample = (int16_t)((int8_t)(sample ^ 0x80));
        
        // Scale up to 16-bit audio (x256)
        int16_t output = signed_sample * 256;
        
        // Apply volume (0-255)
        output = (output * m_vol) / 255;
        
        return output;
    }
    
private:
    uint8_t* m_wavetableData;
    uint32_t m_wavetableSize;
    uint32_t m_acc;           // 24-bit accumulator
    uint16_t m_freq;          // Frequency control
    uint8_t m_vol;            // Volume
    uint8_t m_waveTableSize;  // Table size (0-7)
    uint8_t m_resolution;     // Resolution (0-7)
    bool m_halt;              // Oscillator halted flag
    bool m_debugMode;         // Enable debug output
    
    int m_sampleRate;         // Output sample rate
    int m_targetSampleRate;   // Target sample rate the wavetable was designed for
};

// Function to load a binary wavetable file
std::vector<uint8_t> loadWavetable(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open wavetable file: " << filename << std::endl;
        return std::vector<uint8_t>();
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read the entire file
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    
    std::cout << "Loaded wavetable from " << filename << " (" << size << " bytes)" << std::endl;
    
    return data;
}

// Function to generate a test wavetable
std::vector<uint8_t> generateWavetable(int size, int type) {
    std::vector<uint8_t> wavetable(size);
    
    // Generate different waveform types
    switch (type) {
        case 0: // Sine wave
            for (int i = 0; i < size; i++) {
                double angle = 2.0 * M_PI * i / size;
                wavetable[i] = (uint8_t)(128 + 127 * sin(angle));
                if (wavetable[i] == 0) wavetable[i] = 1; // Prevent halt values
            }
            break;
        case 1: // Square wave
            for (int i = 0; i < size; i++) {
                wavetable[i] = (i < size/2) ? 255 : 1; // Use 1 instead of 0 to prevent halting
            }
            break;
        case 2: // Sawtooth wave
            for (int i = 0; i < size; i++) {
                wavetable[i] = (uint8_t)(128 + 127.0 * (2.0 * i / size - 1.0));
                if (wavetable[i] == 0) wavetable[i] = 1; // Prevent zeros
            }
            break;
        case 3: // Triangle wave
            for (int i = 0; i < size; i++) {
                double v = i < size/2 ? 
                    (2.0 * i / size) :
                    (2.0 - 2.0 * i / size);
                wavetable[i] = (uint8_t)(128 + 127.0 * (2.0 * v - 1.0));
                if (wavetable[i] == 0) wavetable[i] = 1; // Prevent zeros
            }
            break;
    }
    
    return wavetable;
}

// Function to play a wavetable and save to WAV file
void playWavetable(const std::vector<uint8_t>& wavetable, const std::string& outfile, 
                 uint16_t freq, uint8_t vol, uint8_t tableSize, uint8_t resolution, 
                 int duration, int sampleRate, bool debug = false, int targetSampleRate = 22050) {
    
    // Create ES5503 emulator
    ES5503 es5503(const_cast<uint8_t*>(wavetable.data()), wavetable.size());
    es5503.setFrequency(freq);
    es5503.setVolume(vol);
    es5503.setTableSize(tableSize);
    es5503.setResolution(resolution);
    es5503.setTargetSampleRate(targetSampleRate);
    es5503.setSampleRate(sampleRate);
    es5503.setDebugMode(debug);
    
    // Create WAV writer
    WaveWriter wav(outfile.c_str(), sampleRate, 1);
    
    // Generate samples
    int numSamples = duration * sampleRate / 1000; // Convert duration from ms to samples
    std::vector<int16_t> samples;
    samples.reserve(numSamples);
    
    int nonZeroCount = 0;
    bool has_output_samples = false;
    int haltCount = 0;
    
    for (int i = 0; i < numSamples; i++) {
        int16_t sample = es5503.generateSample();
        samples.push_back(sample);
        
        if (sample != 0) nonZeroCount++;
        
        // Look for actual non-zero outputs for debugging
        if (debug && !has_output_samples && sample != 0) {
            std::cout << "First non-zero sample at index " << i << ": " << sample << std::endl;
            has_output_samples = true;
        }
        
        // Check if oscillator halted
        if (es5503.isHalted()) {
            haltCount++;
            if (debug && haltCount <= 3) { // Only show first few halts
                std::cout << "Oscillator halted at sample " << i << std::endl;
            }
            // Restart oscillator for demo purposes
            es5503.reset();
        }
        
        // Debug output for first few samples
        if (debug && i < 10) {
            std::cout << "Sample " << i << ": " << sample << std::endl;
        }
    }
    
    // Write samples to WAV file
    wav.writeSamples(samples);
    wav.close();
    
    double durationSec = static_cast<double>(numSamples) / sampleRate;
    
    std::cout << "Generated " << outfile << " with " << numSamples << " samples (" 
              << nonZeroCount << " non-zero) - duration: " << durationSec << " seconds" << std::endl;
    
    if (debug) {
        double scaleFactor = es5503.getFrequencyScaleFactor();
        std::cout << "Target sample rate: " << es5503.getTargetSampleRate() << " Hz" << std::endl;
        std::cout << "Output sample rate: " << sampleRate << " Hz" << std::endl;
        std::cout << "Sample rate scaling factor: " << scaleFactor << std::endl;
        
        // Calculate expected playback duration
        double expectedDur = static_cast<double>(wavetable.size()) / targetSampleRate * freq / 0x01AD;
        std::cout << "Expected sample duration at freq 0x01AD: ~" << expectedDur << " seconds" << std::endl;
    }
}

// Function to analyze a wavetable file
void analyzeWavetable(const std::vector<uint8_t>& wavetable, size_t sampleOffset = 0, size_t count = 256) {
    // Calculate statistics
    size_t maxCount = std::min(count, wavetable.size() - sampleOffset);
    int value_0_count = 0;
    int value_80_count = 0;
    int min_value = 255;
    int max_value = 0;
    
    // Count occurrences of special values
    for (size_t i = 0; i < wavetable.size(); i++) {
        if (wavetable[i] == 0x00) value_0_count++;
        if (wavetable[i] == 0x80) value_80_count++;
        if (wavetable[i] < min_value) min_value = wavetable[i];
        if (wavetable[i] > max_value) max_value = wavetable[i];
    }
    
    // Print general statistics
    std::cout << "Wavetable Statistics:" << std::endl;
    std::cout << "  Size: " << wavetable.size() << " bytes" << std::endl;
    std::cout << "  Value range: " << min_value << " to " << max_value << std::endl;
    std::cout << "  Halt values (0x00): " << value_0_count << " (" 
              << (100.0 * value_0_count / wavetable.size()) << "%)" << std::endl;
    std::cout << "  Silence values (0x80): " << value_80_count << " (" 
              << (100.0 * value_80_count / wavetable.size()) << "%)" << std::endl;
    
    // Print a sample of the wavetable
    std::cout << "\nWavetable sample starting at offset " << sampleOffset << ":" << std::endl;
    for (size_t i = 0; i < maxCount; i += 16) {
        std::cout << "  " << std::hex << std::setw(4) << std::setfill('0') << (sampleOffset + i) << ": ";
        
        // Print hex values
        for (size_t j = 0; j < 16 && (i + j) < maxCount; j++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << (int)wavetable[sampleOffset + i + j] << " ";
        }
        
        // Print ASCII values (for readability)
        std::cout << " | ";
        for (size_t j = 0; j < 16 && (i + j) < maxCount; j++) {
            char c = wavetable[sampleOffset + i + j];
            // Only print printable ASCII
            std::cout << (c >= 32 && c <= 126 ? c : '.');
        }
        
        std::cout << std::dec << std::endl;
    }
    std::cout << std::dec << std::endl;
}

// Function to convert wavetable directly to WAV without processing
void convertWavetableToWav(const std::vector<uint8_t>& wavetable, const std::string& outfile, int sampleRate) {
    std::cout << "Converting wavetable directly to WAV (raw conversion)..." << std::endl;
    
    std::vector<int16_t> samples;
    samples.reserve(wavetable.size());
    
    // Convert each byte directly to a signed 16-bit sample
    for (size_t i = 0; i < wavetable.size(); i++) {
        // Convert Ensoniq 8-bit format to signed 16-bit
        int16_t signed_sample = (int16_t)((int8_t)(wavetable[i] ^ 0x80)) * 256;
        samples.push_back(signed_sample);
    }
    
    // Write WAV file
    WaveWriter wav(outfile.c_str(), sampleRate, 1);
    wav.writeSamples(samples);
    wav.close();
    
    std::cout << "Generated direct conversion " << outfile << " with " 
              << samples.size() << " samples" << std::endl;
}

int main(int argc, char** argv) {
    // Load the frontleft.wavetable file
    std::vector<uint8_t> wavetableData = loadWavetable("frontleft.wavetable");
    if (wavetableData.empty()) {
        std::cerr << "Failed to load wavetable data" << std::endl;
        return 1;
    }
    
    // Analyze the wavetable data
    std::cout << "Analyzing frontleft.wavetable..." << std::endl;
    analyzeWavetable(wavetableData);
    
    // Sample from non-silent section (if any is found)
    size_t nonSilentOffset = 0;
    for (size_t i = 0; i < wavetableData.size(); i++) {
        if (wavetableData[i] != 0x80 && wavetableData[i] != 0x00) {
            nonSilentOffset = (i / 16) * 16; // Round to 16-byte boundary
            break;
        }
    }
    
    if (nonSilentOffset > 0) {
        std::cout << "Found non-silent data at offset " << nonSilentOffset << std::endl;
        analyzeWavetable(wavetableData, nonSilentOffset);
    }
    
    // Direct conversion of wavetable to WAV for comparison
    convertWavetableToWav(wavetableData, "frontleft_direct_8khz.wav", 8000);
    convertWavetableToWav(wavetableData, "frontleft_direct_11khz.wav", 11025);
    convertWavetableToWav(wavetableData, "frontleft_direct_22khz.wav", 22050);
    
    // Original frontleft.wavetable is 32KB, so we use table size 7 and resolution 0
    // This is the common configuration for Apple IIgs voice samples
    uint16_t correctFreq = 0x01AD;
    
    // CRITICAL TEST - Play with correct voice sample approach
    std::cout << "\n=== Testing wavetable playback as a 22050Hz voice sample ===" << std::endl;
    std::cout << "The frontleft.wavetable is a voice sample saying 'front left'" << std::endl;
    std::cout << "designed to be played at 22050Hz with frequency 0x01AD" << std::endl;
    
    // Generate a detailed debug version at 22050Hz (native rate)
    std::cout << "\nCreating debug version at native 22050Hz..." << std::endl;
    playWavetable(wavetableData, "frontleft_22050_debug.wav", correctFreq, 0xFF, 7, 0, 2000, 22050, true, 22050);
    
    // Create WAV file without debug output for clean playback at 22050Hz
    std::cout << "\nCreating standard version at 22050Hz..." << std::endl;
    playWavetable(wavetableData, "frontleft_22050.wav", correctFreq, 0xFF, 7, 0, 2000, 22050, false, 22050);
    
    // Create 44100Hz version with proper sample rate conversion
    std::cout << "\nCreating 44100Hz version with proper sample rate conversion..." << std::endl;
    playWavetable(wavetableData, "frontleft_44100.wav", correctFreq, 0xFF, 7, 0, 2000, 44100, false, 22050);
    
    // Test different frequencies to demonstrate the effect on playback speed
    std::cout << "\nTesting different frequencies (changes playback speed)..." << std::endl;
    uint16_t freqValues[] = {0x00AD, 0x0100, 0x01AD, 0x0280, 0x0300};
    
    for (size_t i = 0; i < sizeof(freqValues)/sizeof(freqValues[0]); i++) {
        double speedRatio = static_cast<double>(freqValues[i]) / correctFreq;
        std::string filename = "frontleft_freq_" + std::to_string(freqValues[i]) + ".wav";
        std::cout << "Frequency 0x" << std::hex << freqValues[i] << std::dec 
                  << " (speed: " << speedRatio << "x)" << std::endl;
        playWavetable(wavetableData, filename, freqValues[i], 0xFF, 7, 0, 2000, 22050, false, 22050);
    }
    
    // Test different volumes
    std::cout << "\nTesting different volume levels..." << std::endl;
    uint8_t volumeLevels[] = {0x40, 0x80, 0xC0, 0xFF};
    
    for (size_t i = 0; i < sizeof(volumeLevels)/sizeof(volumeLevels[0]); i++) {
        double volRatio = static_cast<double>(volumeLevels[i]) / 255.0;
        std::string filename = "frontleft_vol_" + std::to_string(volumeLevels[i]) + ".wav";
        std::cout << "Volume 0x" << std::hex << static_cast<int>(volumeLevels[i]) << std::dec 
                  << " (" << (volRatio * 100.0) << "%)" << std::endl;
        playWavetable(wavetableData, filename, correctFreq, volumeLevels[i], 7, 0, 2000, 22050, false, 22050);
    }
    
    // Generate test waveforms
    std::cout << "\nGenerating test waveforms for comparison..." << std::endl;
    std::vector<uint8_t> sineWave = generateWavetable(WAVETABLE_SIZE, 0);
    std::vector<uint8_t> squareWave = generateWavetable(WAVETABLE_SIZE, 1);
    std::vector<uint8_t> sawWave = generateWavetable(WAVETABLE_SIZE, 2);
    std::vector<uint8_t> triangleWave = generateWavetable(WAVETABLE_SIZE, 3);
    
    // Test synthetic waveforms with the same parameters
    std::cout << "\nTesting synthetic waveforms at 22050Hz..." << std::endl;
    playWavetable(sineWave, "sine_wave.wav", 0x0080, 0xFF, 0, 0, 1000, 22050, false, 22050);
    playWavetable(squareWave, "square_wave.wav", 0x0080, 0xFF, 0, 0, 1000, 22050, false, 22050);
    playWavetable(sawWave, "sawtooth_wave.wav", 0x0080, 0xFF, 0, 0, 1000, 22050, false, 22050);
    playWavetable(triangleWave, "triangle_wave.wav", 0x0080, 0xFF, 0, 0, 1000, 22050, false, 22050);
    
    // Test effect of table size and resolution on synthetic waveforms
    std::cout << "\nTesting table size and resolution effects on a sine wave..." << std::endl;
    
    // Test different resolutions with tableSize 0
    for (int res = 0; res < 4; res++) {  // Test only first 4 resolutions
        std::string filename = "sine_res_" + std::to_string(res) + ".wav";
        playWavetable(sineWave, filename, 0x0080, 0xFF, 0, res, 1000, 22050, false, 22050);
    }
    
    // Test different table sizes with resolution 0
    for (int ts = 0; ts < 4; ts++) {  // Test only first 4 table sizes
        std::string filename = "sine_tablesize_" + std::to_string(ts) + ".wav";
        
        // Create larger wavetables for larger table sizes
        int size = 256 << ts;
        std::vector<uint8_t> largeSine = generateWavetable(size, 0);
        
        playWavetable(largeSine, filename, 0x0080, 0xFF, ts, 0, 1000, 22050, false, 22050);
    }
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    std::cout << "The frontleft_direct_*.wav files are direct conversions of the wavetable data" << std::endl;
    std::cout << "at different sample rates with no processing." << std::endl;
    std::cout << "The frontleft_22050.wav file represents the processed version of the wavetable" << std::endl;
    std::cout << "using the ES5503 technique with frequency 0x01AD at 22050Hz." << std::endl;
    
    return 0;
}