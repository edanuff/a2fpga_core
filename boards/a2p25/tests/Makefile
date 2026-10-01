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
	rm -f sim.out qspi_sim.out qspi_serializer.out dp_dphy.out dump.vcd qspi_protocol_gen.vcd qspi_serializer.vcd dp_dphy_tx.vcd

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
	@echo "  spi_connector - Test 3-wire SPI protocol (connector + proto proc)"
	@echo "  sim   - Alias for stream test"
	@echo "  wave  - Alias for stream test"
	@echo "  clean - Clean generated files"
	@echo "  help  - Show this help"

# DP-over-D-PHY main-link shim test (10->16 gearbox + enable sequencing)
DP_DPHY_FILES = ../hdl/dp/dp_dphy_tx.sv test_dp_dphy_tx.sv
dp_dphy: $(DP_DPHY_FILES)
	@echo "=== Compiling DP D-PHY TX Shim Test ==="
	iverilog -g2012 -o dp_dphy.out $(DP_DPHY_FILES)
	@echo "=== Running DP D-PHY TX Shim Simulation ==="
	./dp_dphy.out
	@if [ -f dp_dphy_tx.vcd ]; then echo "=== VCD file generated: dp_dphy_tx.vcd ==="; echo "Open with: gtkwave dp_dphy_tx.vcd"; fi

# DP PHY integration test: transceiver_bank_dphy + generated Gowin_MIPI_DPHY
# wrapper (PLL_INIT etc.) + behavioural PLL and D-PHY hard-macro stand-ins
GOWIN_SIMLIB ?= /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/simlib/gw5a/prim_sim.v
DPHY_BANK_FILES = ../hdl/dp/transceiver_bank_dphy.sv ../hdl/dp/dp_dphy_tx.sv \
	../hdl/gowin/gowin_mipi_dphy/gowin_mipi_dphy.v sim_models/gowin_mipi_dphy_mipi_dphy_sim.v \
	sim_models/gowin_mipi_dphy_plla_sim.v ../hdl/gowin/pll_init.v \
	../../../hdl/displayport/gowin/lane_encoder_8b10b.v ../../../hdl/displayport/gowin/enc_8b10b.v \
	../../../hdl/displayport/gowin/benz_encode.v test_transceiver_bank_dphy.sv
dphy_bank: $(DPHY_BANK_FILES)
	@echo "=== Compiling DP PHY bank integration test ==="
	iverilog -g2012 -s test_transceiver_bank_dphy -o dphy_bank.out $(DPHY_BANK_FILES) $(GOWIN_SIMLIB)
	@echo "=== Running ==="
	vvp dphy_bank.out

SPI_CONNECTOR_FILES = ../hdl/esp32/esp32_spi_proto_proc.sv ../hdl/esp32/esp32_spi_connector.sv test_esp32_spi_connector.sv
spi_connector: $(SPI_CONNECTOR_FILES)
	@echo "=== Compiling ESP32 SPI Connector Test ==="
	iverilog -g2012 -DTB_DEBUG -o spi_connector.out $(SPI_CONNECTOR_FILES)
	@echo "=== Running ESP32 SPI Connector Simulation ==="
	./spi_connector.out
	@if [ -f esp32_spi_connector.vcd ]; then echo "=== VCD file generated: esp32_spi_connector.vcd ==="; echo "Open with: gtkwave esp32_spi_connector.vcd"; fi
.PHONY: all qspi qspi_serializer stream sim wave clean help

-include hyperram.mk
