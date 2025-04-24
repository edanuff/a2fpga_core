# Verilator Makefile for DOC5503 and GLU testing

# Directory paths
HDLDIR = ../../hdl/sound
SUPPORTDIR = ../../hdl/support
BUSDIR = ../../hdl/bus
MEMORYDIR = ../../hdl/memory
SDRAMDIR = ../../hdl/sdram
OBJDIR = obj_dir
OBJDIR_GLU = obj_dir_glu

# Verilator flags
VERILATOR_FLAGS = -Wall -Wno-fatal --trace -CFLAGS "-O3 -g3" --cc --exe
VERILATOR_FLAGS += -I$(HDLDIR) -I$(SUPPORTDIR) -I$(BUSDIR) -I$(MEMORYDIR) -I$(SDRAMDIR)

# C++ compiler flags
CXXFLAGS = -std=c++17 -Wall -O3 -g
VERILATOR_INCLUDE = /opt/homebrew/Cellar/verilator/5.034/share/verilator/include

# Source files for DOC testing
DOC_MODULE_SV = doc5503_harness.sv
DOC_MODULE_CPP = doc5503_tb.cpp
DOC_HDLFILES = $(HDLDIR)/doc5503.sv $(HDLDIR)/doc5503_register_ram.sv $(SUPPORTDIR)/srff.v

# Source files for GLU testing
GLU_MODULE_SV = glu_harness.sv
GLU_MODULE_CPP = glu_tb.cpp
GLU_HDLFILES = $(HDLDIR)/sound_glu.sv $(HDLDIR)/doc5503.sv $(HDLDIR)/doc5503_register_ram.sv \
               $(SUPPORTDIR)/srff.v

# Targets
all: doc_tests glu_tests comparison_tests

doc_tests: $(OBJDIR)/Vdoc5503_harness doc_audio_test doc_wavetable_test generate_test_wav

glu_tests: $(OBJDIR_GLU)/Vglu_harness glu_wav_test

comparison_tests: es5503_standalone.o standalone_es5503_test doc5503_comparison_test

# DOC test targets
$(OBJDIR)/Vdoc5503_harness: $(DOC_MODULE_SV) $(DOC_MODULE_CPP) $(DOC_HDLFILES)
	verilator $(VERILATOR_FLAGS) $(DOC_MODULE_SV) $(DOC_HDLFILES) $(DOC_MODULE_CPP)
	make -C $(OBJDIR) -f Vdoc5503_harness.mk

doc_audio_test: doc_audio_test.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

doc_wavetable_test: doc_wavetable_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

generate_test_wav: generate_test_wav.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

# GLU test targets
$(OBJDIR_GLU)/Vglu_harness: $(GLU_MODULE_SV) $(GLU_MODULE_CPP) $(GLU_HDLFILES)
	verilator $(VERILATOR_FLAGS) $(GLU_MODULE_SV) $(GLU_HDLFILES) $(GLU_MODULE_CPP) --top-module glu_harness -Mdir $(OBJDIR_GLU)
	make -C $(OBJDIR_GLU) -f Vglu_harness.mk

glu_wav_test: glu_wav_test.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

# Run targets
run_doc: $(OBJDIR)/Vdoc5503_harness
	$(OBJDIR)/Vdoc5503_harness

run_glu: $(OBJDIR_GLU)/Vglu_harness
	$(OBJDIR_GLU)/Vglu_harness

run_audio_test: doc_audio_test
	./doc_audio_test

run_wavetable_test: doc_wavetable_test
	./doc_wavetable_test
	
run_generate_test: generate_test_wav
	./generate_test_wav

run_glu_wav_test: glu_wav_test
	./glu_wav_test

# ES5503 standalone and comparison test targets
es5503_standalone.o: es5503_standalone.cpp es5503_standalone.h
	g++ $(CXXFLAGS) -c -o $@ es5503_standalone.cpp

standalone_es5503_test: standalone_es5503_test.cpp es5503_standalone.o
	g++ $(CXXFLAGS) -o $@ $< es5503_standalone.o -lm

doc5503_comparison_test: doc5503_comparison_test.cpp es5503_standalone.o $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< es5503_standalone.o $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

run_comparison_test: doc5503_comparison_test
	./doc5503_comparison_test
	
run_standalone_test: standalone_es5503_test
	./standalone_es5503_test

# Clean targets
clean:
	rm -rf $(OBJDIR) $(OBJDIR_GLU) doc_audio_test doc_wavetable_test generate_test_wav glu_wav_test doc5503_comparison_test es5503_standalone.o *.wav

clean_doc:
	rm -rf $(OBJDIR) doc_audio_test doc_wavetable_test generate_test_wav

clean_glu:
	rm -rf $(OBJDIR_GLU) glu_wav_test

clean_comparison:
	rm -f doc5503_comparison_test es5503_standalone.o doc5503_*_output.wav

.PHONY: all doc_tests glu_tests comparison_tests run_doc run_glu run_audio_test run_wavetable_test run_generate_test run_glu_wav_test run_comparison_test clean clean_doc clean_glu clean_comparison