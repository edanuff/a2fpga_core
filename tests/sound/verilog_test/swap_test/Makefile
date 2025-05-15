# Makefile for DOC5503 Swap Mode Test

# Verilator configuration
VERILATOR = verilator
VERILATOR_FLAGS = -Wall -Wno-fatal --trace --trace-structs --timing
VERILATOR_FLAGS += --top-module doc5503_verilog_test

# Path to DOC5503 implementation
DOC_PATH = ../../../../hdl/sound/doc5503.sv
SRFF_PATH = ../../../../hdl/support/srff.v

# Build directory
BUILD_DIR = ./obj_dir

# Check if build directory exists
$(shell mkdir -p $(BUILD_DIR))

# Main target
all: swap_test

# Compile the test
swap_test: doc5503_verilog_swap_test.sv $(DOC_PATH) $(SRFF_PATH)
	$(VERILATOR) $(VERILATOR_FLAGS) --binary $^ -o $@
	@echo "Build complete. Executable is at $(BUILD_DIR)/Vdoc5503_verilog_test"

# Run the test
run: swap_test
	cd $(BUILD_DIR) && ./swap_test +trace
	@echo "Test complete. VCD file is at $(BUILD_DIR)/doc5503_verilog_swap_test.vcd"

# Clean up
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean