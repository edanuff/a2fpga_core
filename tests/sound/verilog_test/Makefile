# Makefile for DOC5503 Verilog test
# Supports multiple test modules and visualization

# Source file paths
HDLDIR = ../../../hdl/sound
SUPPORTDIR = ../../../hdl/support
BUSDIR = ../../../hdl/bus
MEMORYDIR = ../../../hdl/memory
SDRAMDIR = ../../../hdl/sdram

# Verilator flags
VERILATOR_FLAGS = -Wall -Wno-fatal --trace --trace-structs -CFLAGS "-O3" 
VERILATOR_FLAGS += --binary
VERILATOR_FLAGS += -I$(HDLDIR) -I$(SUPPORTDIR) -I$(BUSDIR) -I$(MEMORYDIR) -I$(SDRAMDIR)

# Test modules (add new test modules here)
REG_TEST = doc5503_verilog_test
SINE_TEST = doc5503_sine_test
MULTI_OSC_TEST = doc5503_oscillators_test

# Build directory
BUILD_DIR = build

# Default target builds and runs the register test
all: build-reg run-reg

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Register test targets
build-reg: $(BUILD_DIR)
	verilator $(VERILATOR_FLAGS) -o $(BUILD_DIR)/$(REG_TEST) $(REG_TEST).sv $(HDLDIR)/doc5503.sv $(SUPPORTDIR)/srff.v

run-reg: build-reg
	cd $(BUILD_DIR) && ./$(REG_TEST) +trace

view-reg: run-reg
	@if command -v gtkwave > /dev/null; then \
		gtkwave $(BUILD_DIR)/$(REG_TEST).vcd; \
	else \
		echo "GTKWave not found. Please open the VCD file manually."; \
	fi

# Sine wave test targets
build-sine: $(BUILD_DIR)
	verilator $(VERILATOR_FLAGS) -o $(BUILD_DIR)/$(SINE_TEST) $(SINE_TEST).sv $(HDLDIR)/doc5503.sv $(SUPPORTDIR)/srff.v

run-sine: build-sine
	cd $(BUILD_DIR) && ./$(SINE_TEST) +trace

view-sine: run-sine
	@if command -v gtkwave > /dev/null; then \
		gtkwave $(BUILD_DIR)/$(SINE_TEST).vcd; \
	else \
		echo "GTKWave not found. Please open the VCD file manually."; \
	fi

plot-sine: run-sine
	cd $(BUILD_DIR) && ../sine_plot.py --input audio_output.txt --output sine_waveform.png --analyze

# Multiple oscillator test targets
build-multi: $(BUILD_DIR)
	verilator $(VERILATOR_FLAGS) -o $(BUILD_DIR)/$(MULTI_OSC_TEST) $(MULTI_OSC_TEST).sv $(HDLDIR)/doc5503.sv $(SUPPORTDIR)/srff.v

run-multi: build-multi
	cd $(BUILD_DIR) && ./$(MULTI_OSC_TEST) +trace

view-multi: run-multi
	@if command -v gtkwave > /dev/null; then \
		gtkwave $(BUILD_DIR)/$(MULTI_OSC_TEST).vcd; \
	else \
		echo "GTKWave not found. Please open the VCD file manually."; \
	fi

plot-multi: run-multi
	cd $(BUILD_DIR) && ../multi_osc_plot.py --input multi_osc_output.txt --output multi_osc_waveform.png --analyze

# Run all tests
test-all: run-reg run-sine run-multi

# Clean target
clean:
	rm -rf obj_dir
	rm -f $(BUILD_DIR)/$(REG_TEST)
	rm -f $(BUILD_DIR)/$(SINE_TEST)
	rm -f $(BUILD_DIR)/$(MULTI_OSC_TEST)
	rm -f $(BUILD_DIR)/*.vcd
	rm -f $(BUILD_DIR)/*.txt
	rm -f $(BUILD_DIR)/*.png

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build and run the register test (default)"
	@echo "  build-reg    - Build the register test"
	@echo "  run-reg      - Run the register test"
	@echo "  view-reg     - View register test waveforms"
	@echo "  build-sine   - Build the sine wave test"
	@echo "  run-sine     - Run the sine wave test"
	@echo "  view-sine    - View sine test waveforms"
	@echo "  plot-sine    - Plot sine wave audio output"
	@echo "  build-multi  - Build the multiple oscillators test"
	@echo "  run-multi    - Run the multiple oscillators test"
	@echo "  view-multi   - View multiple oscillators test waveforms"
	@echo "  plot-multi   - Plot multiple oscillators audio output"
	@echo "  test-all     - Run all tests"
	@echo "  clean        - Clean build files"
	@echo ""
	@echo "Examples:"
	@echo "  make         - Build and run register test"
	@echo "  make run-sine - Run sine wave test"
	@echo "  make plot-multi - Run multiple oscillators test and plot results"

.PHONY: all build-reg run-reg view-reg build-sine run-sine view-sine plot-sine \
        build-multi run-multi view-multi plot-multi test-all clean help