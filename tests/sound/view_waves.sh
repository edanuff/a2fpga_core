#!/bin/bash
# Helper script to play and compare the generated audio waveforms

# Check if we're on macOS and have 'afplay'
if command -v afplay &> /dev/null; then
    PLAY_CMD="afplay"
elif command -v aplay &> /dev/null; then
    PLAY_CMD="aplay"
else
    echo "No audio player found. Please install 'afplay' (Mac) or 'aplay' (Linux)."
    exit 1
fi

echo "Audio Test File Player"
echo "====================="
echo ""

# Function to play a pair of files for comparison
play_comparison() {
    local file1=$1
    local file2=$2
    local description=$3
    
    echo "Playing: $description"
    echo "First: $file1"
    $PLAY_CMD "$file1"
    echo "Second: $file2"
    $PLAY_CMD "$file2"
    echo ""
}

# Main menu
while true; do
    echo "Choose a test to listen to:"
    echo "1. Square Wave Comparison (most obvious difference, 172 Hz)"
    echo "2. Square Wave Comparison (higher frequency, 344 Hz)"
    echo "3. Sawtooth Wave Comparison (172 Hz)"
    echo "4. Sawtooth Wave Comparison (344 Hz)"
    echo "5. Multiple Oscillator Mix Test"
    echo "q. Quit"
    echo ""
    read -p "Enter your choice: " choice
    
    case $choice in
        1)
            play_comparison "square_raw_freq512.wav" "square_filtered_freq512.wav" "Square wave at 172 Hz - Unfiltered vs. Filtered (0.75x)"
            ;;
        2)
            play_comparison "square_raw_freq1024.wav" "square_filtered_freq1024.wav" "Square wave at 344 Hz - Unfiltered vs. Filtered (0.75x)"
            ;;
        3)
            play_comparison "sawtooth_raw_freq512.wav" "sawtooth_filtered_freq512.wav" "Sawtooth wave at 172 Hz - Unfiltered vs. Filtered (0.75x)"
            ;;
        4)
            play_comparison "sawtooth_raw_freq1024.wav" "sawtooth_filtered_freq1024.wav" "Sawtooth wave at 344 Hz - Unfiltered vs. Filtered (0.75x)"
            ;;
        5)
            play_comparison "doc_output_8osc_scale_1.0000.wav" "doc_output_8osc_scale_0.5000.wav" "8 oscillators mixed - No filter vs. 0.5x scaling"
            ;;
        q|Q)
            echo "Exiting."
            exit 0
            ;;
        *)
            echo "Invalid choice. Please try again."
            ;;
    esac
done