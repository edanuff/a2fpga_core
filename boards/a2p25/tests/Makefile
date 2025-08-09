# Makefile for QSPI and stream_serializer testbenches
# Requires iverilog (can install with: brew install icarus-verilog)

# Default target - run QSPI serializer test
all: qspi_serializer

# QSPI Protocol Generator Test
QSPI_FILES = qspi_protocol_gen.sv test_qspi_protocol_gen.sv
qspi: $(QSPI_FILES)
	@echo "=== Compiling QSPI Protocol Generator Test ==="
	iverilog -g2012 -o qspi_sim.out $(QSPI_FILES)
	@echo "=== Running QSPI Simulation ==="
	./qspi_sim.out
	@if [ -f qspi_protocol_gen.vcd ]; then echo "=== VCD file generated: qspi_protocol_gen.vcd ==="; echo "Open with: gtkwave qspi_protocol_gen.vcd"; fi

# Stream Serializer Test (original)
STREAM_FILES = ../hdl/esp32/stream_serializer.sv test_stream_serializer.sv
stream: $(STREAM_FILES)
	@echo "=== Compiling Stream Serializer Test ==="
	iverilog -g2012 -o sim.out $(STREAM_FILES)
	@echo "=== Running Stream Simulation ==="
	./sim.out
	@if [ -f dump.vcd ]; then echo "=== VCD file generated: dump.vcd ==="; echo "Open with: gtkwave dump.vcd"; fi

# Alias for legacy compatibility
sim: stream
wave: stream

# Clean generated files
clean:
	rm -f sim.out qspi_sim.out qspi_serializer.out dump.vcd qspi_protocol_gen.vcd qspi_serializer.vcd

# QSPI Serializer Test (new implementation)
QSPI_SERIALIZER_FILES = qspi_serializer_fixed.sv test_qspi_serializer.sv
qspi_serializer: $(QSPI_SERIALIZER_FILES)
	@echo "=== Compiling QSPI Serializer Test ==="
	iverilog -g2012 -o qspi_serializer.out $(QSPI_SERIALIZER_FILES)
	@echo "=== Running QSPI Serializer Simulation ==="
	./qspi_serializer.out
	@if [ -f qspi_serializer.vcd ]; then echo "=== VCD file generated: qspi_serializer.vcd ==="; echo "Open with: gtkwave qspi_serializer.vcd"; fi

# Help
help:
	@echo "Available targets:"
	@echo "  qspi_serializer - Test new QSPI serializer implementation (default)"
	@echo "  qspi  - Test QSPI protocol generator"
	@echo "  stream- Test stream serializer"
	@echo "  sim   - Alias for stream test"
	@echo "  wave  - Alias for stream test"
	@echo "  clean - Clean generated files"
	@echo "  help  - Show this help"

.PHONY: all qspi qspi_serializer stream sim wave clean help