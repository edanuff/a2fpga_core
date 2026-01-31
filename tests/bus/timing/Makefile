HDL_DIR = ../../../hdl

SOURCES = \
	$(HDL_DIR)/support/cdc.sv \
	$(HDL_DIR)/support/cdc_denoise.sv \
	$(HDL_DIR)/bus/a2bus_timing.sv \
	a2bus_timing_tb.sv

.PHONY: all run clean view

all: run

a2bus_timing_tb: $(SOURCES)
	iverilog -g2012 -o $@ $(SOURCES)

run: a2bus_timing_tb
	vvp a2bus_timing_tb
	@if [ -f a2bus_timing_tb.vcd ]; then echo "=== VCD file generated: a2bus_timing_tb.vcd ==="; fi

view: a2bus_timing_tb.vcd
	gtkwave a2bus_timing_tb.vcd &

clean:
	rm -f a2bus_timing_tb a2bus_timing_tb.vcd
