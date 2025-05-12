#!/usr/bin/env python3

"""
Script to visualize the audio output from the DOC5503 multiple oscillators test.
Reads the multi_osc_output.txt file created by the test and plots both channels.
"""

import matplotlib.pyplot as plt
import numpy as np
import argparse
import os
from scipy import signal
from scipy.fft import fft, fftfreq

def load_audio_data(filename):
    """Load stereo audio sample data from file."""
    if not os.path.exists(filename):
        print(f"Error: File {filename} not found.")
        return None, None
    
    left_samples = []
    right_samples = []
    
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split(',')
            if len(parts) >= 3:
                try:
                    # Get the sample values (second and third columns)
                    left = int(parts[1].strip())
                    right = int(parts[2].strip())
                    left_samples.append(left)
                    right_samples.append(right)
                except ValueError:
                    print(f"Warning: Couldn't parse line: {line}")
    
    return np.array(left_samples), np.array(right_samples)

def plot_waveforms(left_samples, right_samples, filename, title="DOC5503 Multiple Oscillators Output"):
    """Plot the stereo audio waveforms."""
    if len(left_samples) == 0 or len(right_samples) == 0:
        print("No data to plot")
        return
    
    plt.figure(figsize=(12, 8))
    
    # Create time axis (sample number)
    time = np.arange(len(left_samples))
    
    # Plot left channel
    plt.subplot(2, 1, 1)
    plt.plot(time, left_samples, color='blue')
    plt.title(f"{title} - Left Channel")
    plt.xlabel("Sample Number")
    plt.ylabel("Amplitude")
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # Plot right channel
    plt.subplot(2, 1, 2)
    plt.plot(time, right_samples, color='red')
    plt.title(f"{title} - Right Channel")
    plt.xlabel("Sample Number")
    plt.ylabel("Amplitude")
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # Adjust layout
    plt.tight_layout()
    
    # Save figure
    plt.savefig(filename)
    print(f"Saved waveform plot to {filename}")
    
    # Show plot
    plt.show()

def plot_spectrum(left_samples, right_samples, filename, sample_rate=894880):
    """Plot the frequency spectrum of the audio signals."""
    if len(left_samples) == 0 or len(right_samples) == 0:
        print("No data to plot spectrum")
        return
    
    plt.figure(figsize=(12, 8))
    
    # Compute FFT of left channel
    left_fft = fft(left_samples)
    left_fft_mag = np.abs(left_fft)
    
    # Compute FFT of right channel
    right_fft = fft(right_samples)
    right_fft_mag = np.abs(right_fft)
    
    # Create frequency axis
    n = len(left_samples)
    freqs = fftfreq(n, 1/sample_rate)[:n//2]
    
    # Plot left channel spectrum
    plt.subplot(2, 1, 1)
    plt.plot(freqs, left_fft_mag[:n//2], color='blue')
    plt.title("Frequency Spectrum - Left Channel")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude")
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.xscale('log')
    plt.xlim([10, sample_rate/2])
    
    # Plot right channel spectrum
    plt.subplot(2, 1, 2)
    plt.plot(freqs, right_fft_mag[:n//2], color='red')
    plt.title("Frequency Spectrum - Right Channel")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude")
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.xscale('log')
    plt.xlim([10, sample_rate/2])
    
    # Adjust layout
    plt.tight_layout()
    
    # Save figure
    plt.savefig(filename)
    print(f"Saved spectrum plot to {filename}")
    
    # Show plot
    plt.show()

def analyze_audio(left_samples, right_samples, sample_rate=894880):
    """Analyze the audio data."""
    if len(left_samples) == 0 or len(right_samples) == 0:
        print("No data to analyze")
        return
    
    print("\nAudio Analysis:")
    
    # Basic statistics for left channel
    print("Left Channel:")
    min_left = np.min(left_samples)
    max_left = np.max(left_samples)
    mean_left = np.mean(left_samples)
    peak_to_peak_left = max_left - min_left
    
    print(f"  Min value: {min_left}")
    print(f"  Max value: {max_left}")
    print(f"  Mean value: {mean_left:.2f}")
    print(f"  Peak-to-peak amplitude: {peak_to_peak_left}")
    
    # Basic statistics for right channel
    print("\nRight Channel:")
    min_right = np.min(right_samples)
    max_right = np.max(right_samples)
    mean_right = np.mean(right_samples)
    peak_to_peak_right = max_right - min_right
    
    print(f"  Min value: {min_right}")
    print(f"  Max value: {max_right}")
    print(f"  Mean value: {mean_right:.2f}")
    print(f"  Peak-to-peak amplitude: {peak_to_peak_right}")
    
    # Cross-correlation between channels
    corr = np.corrcoef(left_samples, right_samples)[0, 1]
    print(f"\nCorrelation between channels: {corr:.4f}")
    
    # Phase relationship (very simplified)
    if corr < 0:
        print("Channels appear to be out of phase")
    elif corr > 0.9:
        print("Channels appear to be in phase")
    else:
        print("Channels have complex phase relationship")

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description="Plot DOC5503 multi-oscillator audio output")
    parser.add_argument("--input", "-i", default="multi_osc_output.txt", 
                        help="Input file with audio samples (default: multi_osc_output.txt)")
    parser.add_argument("--output", "-o", default="multi_osc_waveform.png",
                        help="Output waveform image file (default: multi_osc_waveform.png)")
    parser.add_argument("--spectrum", "-s", default="multi_osc_spectrum.png",
                        help="Output spectrum image file (default: multi_osc_spectrum.png)")
    parser.add_argument("--title", "-t", default="DOC5503 Multiple Oscillators Output",
                        help="Plot title (default: 'DOC5503 Multiple Oscillators Output')")
    parser.add_argument("--analyze", "-a", action="store_true",
                        help="Perform basic audio analysis")
    parser.add_argument("--sample-rate", "-r", type=int, default=894880,
                        help="Sample rate in Hz (default: 894880)")
    
    args = parser.parse_args()
    
    # Load the audio data
    left_samples, right_samples = load_audio_data(args.input)
    
    if left_samples is not None and right_samples is not None:
        if len(left_samples) > 0 and len(right_samples) > 0:
            print(f"Loaded {len(left_samples)} stereo audio samples")
            
            # Plot the waveforms
            plot_waveforms(left_samples, right_samples, args.output, args.title)
            
            # Plot the spectrum
            plot_spectrum(left_samples, right_samples, args.spectrum, args.sample_rate)
            
            # Analyze the audio if requested
            if args.analyze:
                analyze_audio(left_samples, right_samples, args.sample_rate)
        else:
            print("Empty data arrays")
    else:
        print("Failed to load audio data")

if __name__ == "__main__":
    main()