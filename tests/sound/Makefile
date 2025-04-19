# Verilator Makefile for DOC5503 testing

# Directory paths
HDLDIR = ../../hdl/sound
SUPPORTDIR = ../../hdl/support
OBJDIR = obj_dir

# Verilator flags
VERILATOR_FLAGS = -Wall -Wno-fatal --trace -CFLAGS "-O3 -g3" --cc --exe
VERILATOR_FLAGS += -I$(HDLDIR) -I$(SUPPORTDIR)

# C++ compiler flags
CXXFLAGS = -std=c++11 -Wall -O3 -g

# Source files
MODULE_SV = doc5503_harness.sv
MODULE_CPP = doc5503_tb.cpp
HDLFILES = $(HDLDIR)/doc5503.sv $(HDLDIR)/doc5503_register_ram.sv $(SUPPORTDIR)/srff.v

# Targets
all: $(OBJDIR)/Vdoc5503_harness doc_audio_test

$(OBJDIR)/Vdoc5503_harness: $(MODULE_SV) $(MODULE_CPP) $(HDLFILES)
	verilator $(VERILATOR_FLAGS) $(MODULE_SV) $(HDLFILES) $(MODULE_CPP)
	make -C $(OBJDIR) -f Vdoc5503_harness.mk

doc_audio_test: doc_audio_test.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

run: $(OBJDIR)/Vdoc5503_harness
	$(OBJDIR)/Vdoc5503_harness

run_audio_test: doc_audio_test
	./doc_audio_test

clean:
	rm -rf $(OBJDIR) doc_audio_test

.PHONY: all run run_audio_test clean