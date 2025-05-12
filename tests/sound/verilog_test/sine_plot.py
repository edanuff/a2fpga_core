#!/usr/bin/env python3

"""
Simple script to visualize the audio output from the DOC5503 sine test.
Reads the audio_output.txt file created by the test and plots the audio waveform.
"""

import matplotlib.pyplot as plt
import numpy as np
import argparse
import os

def load_audio_data(filename):
    """Load audio sample data from file."""
    if not os.path.exists(filename):
        print(f"Error: File {filename} not found.")
        return None
    
    samples = []
    
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split(',')
            if len(parts) >= 2:
                try:
                    # Get the sample value (second column)
                    sample = int(parts[1].strip())
                    samples.append(sample)
                except ValueError:
                    print(f"Warning: Couldn't parse line: {line}")
    
    return np.array(samples)

def plot_waveform(samples, filename, title="DOC5503 Sine Wave Output"):
    """Plot the audio waveform."""
    plt.figure(figsize=(12, 6))
    
    # Plot samples
    plt.plot(samples)
    
    # Add title and labels
    plt.title(title)
    plt.xlabel("Sample Number")
    plt.ylabel("Amplitude")
    
    # Add grid
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # Save figure
    plt.savefig(filename)
    print(f"Saved plot to {filename}")
    
    # Show plot
    plt.show()

def analyze_waveform(samples):
    """Analyze the waveform for basic properties."""
    if len(samples) == 0:
        print("No samples to analyze")
        return
    
    # Basic statistics
    min_val = np.min(samples)
    max_val = np.max(samples)
    mean_val = np.mean(samples)
    peak_to_peak = max_val - min_val
    
    print("\nWaveform Analysis:")
    print(f"  Number of samples: {len(samples)}")
    print(f"  Min value: {min_val}")
    print(f"  Max value: {max_val}")
    print(f"  Mean value: {mean_val:.2f}")
    print(f"  Peak-to-peak amplitude: {peak_to_peak}")
    
    # Simple frequency estimation (very basic)
    # Count zero crossings
    zero_crossings = np.where(np.diff(np.signbit(samples - mean_val)))[0]
    if len(zero_crossings) > 1:
        # Average distance between zero crossings
        avg_period_samples = (2 * len(samples)) / len(zero_crossings)
        # Assuming sample rate of DOC5503 is 7.159MHz / 8 ≈ 894.88kHz
        sample_rate = 894880
        estimated_freq = sample_rate / avg_period_samples
        print(f"  Estimated frequency: {estimated_freq:.2f} Hz (approximate)")
    else:
        print("  Not enough zero crossings to estimate frequency")

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description="Plot DOC5503 audio output waveform")
    parser.add_argument("--input", "-i", default="audio_output.txt", 
                        help="Input file with audio samples (default: audio_output.txt)")
    parser.add_argument("--output", "-o", default="waveform_plot.png",
                        help="Output image file (default: waveform_plot.png)")
    parser.add_argument("--title", "-t", default="DOC5503 Sine Wave Output",
                        help="Plot title (default: 'DOC5503 Sine Wave Output')")
    parser.add_argument("--analyze", "-a", action="store_true",
                        help="Perform basic waveform analysis")
    
    args = parser.parse_args()
    
    # Load the audio data
    samples = load_audio_data(args.input)
    
    if samples is not None and len(samples) > 0:
        print(f"Loaded {len(samples)} audio samples")
        
        # Plot the waveform
        plot_waveform(samples, args.output, args.title)
        
        # Analyze the waveform if requested
        if args.analyze:
            analyze_waveform(samples)
    else:
        print("No data to plot")

if __name__ == "__main__":
    main()