# Verilator Makefile for DOC5503 and GLU testing

# Directory paths
HDLDIR = ../../hdl/sound
SUPPORTDIR = ../../hdl/support
BUSDIR = ../../hdl/bus
MEMORYDIR = ../../hdl/memory
SDRAMDIR = ../../hdl/sdram

# Build output paths
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj_dir
OBJDIR_GLU = $(BUILDDIR)/obj_dir_glu
OUTPUTDIR = output

# Local directory structure
HARNESSDIR = harness
TESTCASESDIR = testcases
DOC5503DIR = $(TESTCASESDIR)/doc5503
GLUDIR = $(TESTCASESDIR)/glu
TOOLSDIR = tools
REFDIR = reference
DOCDIR = doc

# Make sure build and output directories exist
$(shell mkdir -p $(BUILDDIR) $(OUTPUTDIR))

# Verilator flags
VERILATOR_FLAGS = -Wall -Wno-fatal --trace -CFLAGS "-O3 -g3" --cc --exe
VERILATOR_FLAGS += -I$(HDLDIR) -I$(SUPPORTDIR) -I$(BUSDIR) -I$(MEMORYDIR) -I$(SDRAMDIR)

# C++ compiler flags
CXXFLAGS = -std=c++17 -Wall -O3 -g
VERILATOR_INCLUDE = /opt/homebrew/Cellar/verilator/5.034/share/verilator/include

# Source files for DOC testing
DOC_MODULE_SV = $(HARNESSDIR)/doc5503_harness.sv
DOC_MODULE_CPP = $(DOC5503DIR)/doc5503_tb.cpp
DOC_HDLFILES = $(HDLDIR)/doc5503.sv $(SUPPORTDIR)/srff.v

# Create build directory structure before building
$(shell mkdir -p $(OBJDIR) $(OBJDIR_GLU))

# Source files for GLU testing
GLU_MODULE_SV = $(HARNESSDIR)/glu_harness.sv
GLU_MODULE_CPP = $(GLUDIR)/glu_tb.cpp
GLU_HDLFILES = $(HDLDIR)/sound_glu.sv $(HDLDIR)/doc5503.sv $(SUPPORTDIR)/srff.v

# Targets
all: doc_tests glu_tests comparison_tests

doc_tests: $(OBJDIR)/Vdoc5503_harness $(BUILDDIR)/doc_audio_test $(BUILDDIR)/doc_wavetable_test $(BUILDDIR)/generate_test_wav $(BUILDDIR)/doc5503_debug_test $(BUILDDIR)/doc5503_unhalt_test $(BUILDDIR)/doc5503_oscillator_unhalt_test $(BUILDDIR)/doc5503_basic_wave_test $(BUILDDIR)/doc5503_core_state_dump $(BUILDDIR)/doc5503_mode_test $(BUILDDIR)/doc5503_interrupt_test $(BUILDDIR)/doc5503_wavetable_resolution_test $(BUILDDIR)/doc5503_volume_test $(BUILDDIR)/doc5503_volume_direct_test $(BUILDDIR)/doc5503_volume_wave_test $(BUILDDIR)/doc5503_volume_wave_test_workaround $(BUILDDIR)/doc5503_tb_fixed $(BUILDDIR)/doc5503_compressor_test $(BUILDDIR)/doc5503_compressor_fixed_test $(BUILDDIR)/doc5503_wave_polarity_fix $(BUILDDIR)/doc5503_data_dump_test $(BUILDDIR)/doc5503_debug_sine_test $(BUILDDIR)/doc5503_audio_test $(BUILDDIR)/doc5503_reg_test

glu_tests: $(OBJDIR_GLU)/Vglu_harness $(BUILDDIR)/glu_wav_test

comparison_tests: $(BUILDDIR)/es5503_standalone.o $(BUILDDIR)/standalone_es5503_test $(BUILDDIR)/doc5503_comparison_test

# DOC test targets
$(OBJDIR)/Vdoc5503_harness: $(DOC_MODULE_SV) $(DOC_MODULE_CPP) $(DOC_HDLFILES)
	verilator $(VERILATOR_FLAGS) $(DOC_MODULE_SV) $(DOC_HDLFILES) $(DOC_MODULE_CPP) -Mdir $(OBJDIR)
	make -C $(OBJDIR) -f Vdoc5503_harness.mk

$(BUILDDIR)/doc_audio_test: $(DOC5503DIR)/doc_audio_test.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

$(BUILDDIR)/doc_wavetable_test: $(DOC5503DIR)/doc_wavetable_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_debug_test: $(DOC5503DIR)/doc5503_debug_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_unhalt_test: $(DOC5503DIR)/doc5503_unhalt_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_oscillator_unhalt_test: $(TESTCASESDIR)/doc5503_oscillator_unhalt_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_basic_wave_test: $(TESTCASESDIR)/doc5503_basic_wave_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_core_state_dump: $(TESTCASESDIR)/doc5503_core_state_dump.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_mode_test: $(TESTCASESDIR)/doc5503_mode_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_interrupt_test: $(TESTCASESDIR)/doc5503_interrupt_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_wavetable_resolution_test: $(TESTCASESDIR)/doc5503_wavetable_resolution_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_volume_test: $(TESTCASESDIR)/doc5503_volume_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_volume_wave_test: $(TESTCASESDIR)/doc5503_volume_wave_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_volume_wave_test_workaround: $(TESTCASESDIR)/doc5503_volume_wave_test_workaround.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_volume_direct_test: $(TESTCASESDIR)/doc5503_volume_direct_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_tb_fixed: $(DOC5503DIR)/doc5503_tb_fixed.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_compressor_test: $(DOC5503DIR)/doc5503_compressor_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_compressor_fixed_test: $(DOC5503DIR)/doc5503_compressor_fixed_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_wave_polarity_fix: $(TESTCASESDIR)/doc5503_wave_polarity_fix.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread
	
$(BUILDDIR)/doc5503_data_dump_test: $(TESTCASESDIR)/doc5503_data_dump_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_debug_sine_test: $(TESTCASESDIR)/doc5503_debug_sine_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_audio_test: $(DOC5503DIR)/doc5503_audio_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/doc5503_reg_test: $(DOC5503DIR)/doc5503_reg_test.cpp $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

$(BUILDDIR)/generate_test_wav: $(TOOLSDIR)/generate_test_wav.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

# GLU test targets
$(OBJDIR_GLU)/Vglu_harness: $(GLU_MODULE_SV) $(GLU_MODULE_CPP) $(GLU_HDLFILES)
	verilator $(VERILATOR_FLAGS) $(GLU_MODULE_SV) $(GLU_HDLFILES) $(GLU_MODULE_CPP) --top-module glu_harness -Mdir $(OBJDIR_GLU)
	make -C $(OBJDIR_GLU) -f Vglu_harness.mk

$(BUILDDIR)/glu_wav_test: $(GLUDIR)/glu_wav_test.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

# Run targets
run_doc: $(OBJDIR)/Vdoc5503_harness
	cd $(BUILDDIR) && ./$(notdir $(OBJDIR))/Vdoc5503_harness

run_glu: $(OBJDIR_GLU)/Vglu_harness
	cd $(BUILDDIR) && ./$(notdir $(OBJDIR_GLU))/Vglu_harness

run_audio_test: $(BUILDDIR)/doc_audio_test
	cd $(BUILDDIR) && ./doc_audio_test

run_wavetable_test: $(BUILDDIR)/doc_wavetable_test
	cd $(BUILDDIR) && ./doc_wavetable_test
	
run_debug_test: $(BUILDDIR)/doc5503_debug_test
	cd $(BUILDDIR) && ./doc5503_debug_test
	
run_unhalt_test: $(BUILDDIR)/doc5503_unhalt_test
	cd $(BUILDDIR) && ./doc5503_unhalt_test
	
run_oscillator_unhalt_test: $(BUILDDIR)/doc5503_oscillator_unhalt_test
	cd $(BUILDDIR) && ./doc5503_oscillator_unhalt_test
	
run_basic_wave_test: $(BUILDDIR)/doc5503_basic_wave_test
	cd $(BUILDDIR) && ./doc5503_basic_wave_test
	
run_core_state_dump: $(BUILDDIR)/doc5503_core_state_dump
	cd $(BUILDDIR) && ./doc5503_core_state_dump

run_mode_test: $(BUILDDIR)/doc5503_mode_test
	cd $(BUILDDIR) && ./doc5503_mode_test

run_interrupt_test: $(BUILDDIR)/doc5503_interrupt_test
	cd $(BUILDDIR) && ./doc5503_interrupt_test

run_wavetable_resolution_test: $(BUILDDIR)/doc5503_wavetable_resolution_test
	cd $(BUILDDIR) && ./doc5503_wavetable_resolution_test
	
run_volume_test: $(BUILDDIR)/doc5503_volume_test
	cd $(BUILDDIR) && ./doc5503_volume_test
	
run_volume_direct_test: $(BUILDDIR)/doc5503_volume_direct_test
	cd $(BUILDDIR) && ./doc5503_volume_direct_test
	
run_volume_wave_test: $(BUILDDIR)/doc5503_volume_wave_test
	cd $(BUILDDIR) && ./doc5503_volume_wave_test
	
run_volume_wave_test_workaround: $(BUILDDIR)/doc5503_volume_wave_test_workaround
	cd $(BUILDDIR) && ./doc5503_volume_wave_test_workaround
	
run_tb_fixed: $(BUILDDIR)/doc5503_tb_fixed
	cd $(BUILDDIR) && ./doc5503_tb_fixed
	
run_compressor_test: $(BUILDDIR)/doc5503_compressor_test
	cd $(BUILDDIR) && ./doc5503_compressor_test
	
run_compressor_fixed_test: $(BUILDDIR)/doc5503_compressor_fixed_test
	cd $(BUILDDIR) && ./doc5503_compressor_fixed_test
	
run_wave_polarity_fix: $(BUILDDIR)/doc5503_wave_polarity_fix
	cd $(BUILDDIR) && ./doc5503_wave_polarity_fix
	
run_data_dump_test: $(BUILDDIR)/doc5503_data_dump_test
	cd $(BUILDDIR) && ./doc5503_data_dump_test

run_debug_sine_test: $(BUILDDIR)/doc5503_debug_sine_test
	cd $(BUILDDIR) && ./doc5503_debug_sine_test

run_reg_test: $(BUILDDIR)/doc5503_reg_test
	cd $(BUILDDIR) && ./doc5503_reg_test

run_audio_test: $(BUILDDIR)/doc5503_audio_test
	cd $(BUILDDIR) && ./doc5503_audio_test

run_generate_test: $(BUILDDIR)/generate_test_wav
	cd $(BUILDDIR) && ./generate_test_wav

run_glu_wav_test: $(BUILDDIR)/glu_wav_test
	cd $(BUILDDIR) && ./glu_wav_test

# ES5503 standalone and comparison test targets
$(BUILDDIR)/es5503_standalone.o: $(REFDIR)/es5503_standalone.cpp $(REFDIR)/es5503_standalone.h
	g++ $(CXXFLAGS) -c -o $@ $(REFDIR)/es5503_standalone.cpp

$(BUILDDIR)/standalone_es5503_test: $(DOC5503DIR)/standalone_es5503_test.cpp $(BUILDDIR)/es5503_standalone.o
	g++ $(CXXFLAGS) -o $@ $< $(BUILDDIR)/es5503_standalone.o -lm

$(BUILDDIR)/doc5503_comparison_test: $(DOC5503DIR)/doc5503_comparison_test.cpp $(BUILDDIR)/es5503_standalone.o $(OBJDIR)/Vdoc5503_harness__ALL.a
	g++ $(CXXFLAGS) -o $@ $< $(BUILDDIR)/es5503_standalone.o $(OBJDIR)/Vdoc5503_harness__ALL.a $(OBJDIR)/verilated.o $(OBJDIR)/verilated_vcd_c.o $(OBJDIR)/verilated_threads.o -I$(OBJDIR) -I$(VERILATOR_INCLUDE) -lm -lpthread

run_comparison_test: $(BUILDDIR)/doc5503_comparison_test
	cd $(BUILDDIR) && ./doc5503_comparison_test
	
run_standalone_test: $(BUILDDIR)/standalone_es5503_test
	cd $(BUILDDIR) && ./standalone_es5503_test

# Utility targets
$(BUILDDIR)/analyze_distortion: $(TOOLSDIR)/analyze_distortion.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

$(BUILDDIR)/detect_distortion: $(TOOLSDIR)/detect_distortion.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

$(BUILDDIR)/detect_major_dips: $(TOOLSDIR)/detect_major_dips.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

$(BUILDDIR)/examine_wav: $(TOOLSDIR)/examine_wav.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

$(BUILDDIR)/test_tone: $(TOOLSDIR)/test_tone.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

$(BUILDDIR)/visualize_waveform: $(TOOLSDIR)/visualize_waveform.cpp
	g++ $(CXXFLAGS) -o $@ $< -lm

utilities: $(BUILDDIR)/analyze_distortion $(BUILDDIR)/detect_distortion $(BUILDDIR)/detect_major_dips $(BUILDDIR)/examine_wav $(BUILDDIR)/test_tone $(BUILDDIR)/visualize_waveform

# Clean targets
clean:
	rm -rf $(BUILDDIR) $(OUTPUTDIR)

clean_doc:
	rm -rf $(OBJDIR) $(BUILDDIR)/doc_audio_test $(BUILDDIR)/doc_wavetable_test $(BUILDDIR)/generate_test_wav $(BUILDDIR)/doc5503_debug_test $(BUILDDIR)/doc5503_unhalt_test $(BUILDDIR)/doc5503_mode_test $(BUILDDIR)/doc5503_interrupt_test $(BUILDDIR)/doc5503_wavetable_resolution_test $(BUILDDIR)/doc5503_volume_test $(BUILDDIR)/doc5503_volume_direct_test $(BUILDDIR)/doc5503_volume_wave_test $(BUILDDIR)/doc5503_volume_wave_test_workaround $(BUILDDIR)/doc5503_tb_fixed $(BUILDDIR)/doc5503_compressor_test

clean_glu:
	rm -rf $(OBJDIR_GLU) $(BUILDDIR)/glu_wav_test

clean_comparison:
	rm -f $(BUILDDIR)/doc5503_comparison_test $(BUILDDIR)/es5503_standalone.o $(OUTPUTDIR)/doc5503_*_output.wav

clean_utilities:
	rm -f $(BUILDDIR)/analyze_distortion $(BUILDDIR)/detect_distortion $(BUILDDIR)/detect_major_dips $(BUILDDIR)/examine_wav $(BUILDDIR)/test_tone $(BUILDDIR)/visualize_waveform

.PHONY: all doc_tests glu_tests comparison_tests utilities run_doc run_glu run_audio_test run_wavetable_test run_debug_test run_unhalt_test run_oscillator_unhalt_test run_basic_wave_test run_core_state_dump run_mode_test run_interrupt_test run_wavetable_resolution_test run_volume_test run_volume_direct_test run_volume_wave_test run_compressor_test run_compressor_fixed_test run_wave_polarity_fix run_data_dump_test run_debug_sine_test run_audio_test run_generate_test run_glu_wav_test run_comparison_test run_standalone_test clean clean_doc clean_glu clean_comparison clean_utilities