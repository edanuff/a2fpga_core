# Makefile for QSPI and stream_serializer testbenches
# Requires iverilog (can install with: brew install icarus-verilog)

# Default target - run QSPI test
all: qspi

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
	rm -f sim.out qspi_sim.out dump.vcd qspi_protocol_gen.vcd

# Help
help:
	@echo "Available targets:"
	@echo "  qspi  - Test QSPI protocol generator (default)"
	@echo "  stream- Test stream serializer"
	@echo "  sim   - Alias for stream test"
	@echo "  wave  - Alias for stream test"
	@echo "  clean - Clean generated files"
	@echo "  help  - Show this help"

.PHONY: all qspi stream sim wave clean help