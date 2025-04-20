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
    echo "1. Square Wave Comparison (144 Hz - matches IIgs frequency offset 429)"
    echo "2. Square Wave Comparison (higher frequency, 344 Hz)"
    echo "3. Sawtooth Wave Comparison (144 Hz - matches IIgs frequency offset 429)"
    echo "4. Sawtooth Wave Comparison (higher frequency, 344 Hz)"
    echo "5. Multiple Oscillator Mix Test"
    echo "6. Volume Comparison (vol=192 vs vol=255)"
    echo "7. Scaling Comparison (0.5x vs 1.0x)"
    echo "q. Quit"
    echo ""
    read -p "Enter your choice: " choice
    
    case $choice in
        1)
            play_comparison "square_raw_freq429.wav" "square_filtered_freq429.wav" "Square wave at 144 Hz (IIgs freq) - Unfiltered vs. Filtered (0.75x)"
            ;;
        2)
            play_comparison "square_raw_freq1024.wav" "square_filtered_freq1024.wav" "Square wave at 344 Hz - Unfiltered vs. Filtered (0.75x)"
            ;;
        3)
            play_comparison "sawtooth_raw_freq429.wav" "sawtooth_filtered_freq429.wav" "Sawtooth wave at 144 Hz (IIgs freq) - Unfiltered vs. Filtered (0.75x)"
            ;;
        4)
            play_comparison "sawtooth_raw_freq1024.wav" "sawtooth_filtered_freq1024.wav" "Sawtooth wave at 344 Hz - Unfiltered vs. Filtered (0.75x)"
            ;;
        5)
            play_comparison "doc_output_8osc_scale_1.0000.wav" "doc_output_8osc_scale_0.5000.wav" "8 oscillators mixed - No filter vs. 0.5x scaling"
            ;;
        6)
            play_comparison "doc_output_vol_192.wav" "doc_output_vol_255.wav" "DOC volume comparison - 192 vs. 255 (maximum)"
            ;;
        7)
            play_comparison "doc_output_scale_0.5000.wav" "doc_output_scale_1.0000.wav" "DOC scaling comparison - 0.5x vs. 1.0x"
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