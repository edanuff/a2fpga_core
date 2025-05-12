#!/bin/bash

# Script to build and run DOC5503 Verilog tests

# Default test type (reg, sine, multi, or all)
TEST_TYPE="reg"

# Function to display usage information
function show_usage {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -t, --test TYPE    Test type to run (reg, sine, multi, or all)"
    echo "  -v, --view         View waveforms after running tests"
    echo "  -p, --plot         Plot audio output data"
    echo "  -c, --clean        Clean build files first"
    echo "  -h, --help         Show this help message"
    echo ""
    echo "Example:"
    echo "  $0 -t sine -p      # Run sine test and plot results"
    echo "  $0 -t all -v       # Run all tests and view waveforms"
    echo ""
}

# Parse command line options
CLEAN=0
VIEW=0
PLOT=0

while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--test)
            TEST_TYPE="$2"
            shift 2
            ;;
        -v|--view)
            VIEW=1
            shift
            ;;
        -p|--plot)
            PLOT=1
            shift
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            echo "Error: Unknown option $1"
            show_usage
            exit 1
            ;;
    esac
done

# Validate test type
if [[ "$TEST_TYPE" != "reg" && "$TEST_TYPE" != "sine" && "$TEST_TYPE" != "multi" && "$TEST_TYPE" != "all" ]]; then
    echo "Error: Invalid test type '$TEST_TYPE'. Must be 'reg', 'sine', 'multi', or 'all'."
    show_usage
    exit 1
fi

# Clean if requested
if [[ $CLEAN -eq 1 ]]; then
    echo "Cleaning build files..."
    make clean
fi

# Run the specified test
case $TEST_TYPE in
    reg)
        echo "Running register test..."
        make run-reg
        if [[ $VIEW -eq 1 ]]; then
            make view-reg
        fi
        ;;
    sine)
        echo "Running sine wave test..."
        make run-sine
        if [[ $VIEW -eq 1 ]]; then
            make view-sine
        fi
        if [[ $PLOT -eq 1 ]]; then
            make plot-sine
        fi
        ;;
    multi)
        echo "Running multiple oscillators test..."
        make run-multi
        if [[ $VIEW -eq 1 ]]; then
            make view-multi
        fi
        if [[ $PLOT -eq 1 ]]; then
            make plot-multi
        fi
        ;;
    all)
        echo "Running all tests..."
        make test-all
        if [[ $VIEW -eq 1 ]]; then
            echo "Viewing waveforms is not supported for 'all' test mode."
        fi
        if [[ $PLOT -eq 1 ]]; then
            make plot-sine
            make plot-multi
        fi
        ;;
esac

echo "Done!"
exit 0