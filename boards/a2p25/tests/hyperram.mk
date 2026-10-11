# HyperRAM controller / BIST simulation (included from Makefile)
#
#   make hyperram              controller TB, board-delay sweep (HR_DLYS, ps)
#   make hyperram_bist         BIST TB, board-delay sweep (HR_BIST_DLYS, ps):
#                              zero errors + error injection detected
#   make hyperram_clean
#
# Needs iverilog, the Infineon S27KL0642 Verilog model (Cypress/Infineon
# copyright — NOT in this repo; download 002-26518 from infineon.com or use
# nand2mario's psram-tang-nano-9k checkout) and the Gowin GW5A simulation
# primitives. Override the paths on the command line if they live
# elsewhere:
#   make hyperram HR_MODEL=/path/s27kl0642.v GOWIN_SIMLIB=/path/prim_sim.v
#
# The model is copied into $(HR_BUILD) (git-ignored) and patched there:
#   - part select S27KS0642GABHI -> S27KL0642DPBHI (3.0 V, 166 MHz grade)
#   - variable-latency refresh collision: the stock model drives RWDS high
#     during CA when refresh_pulse is set but then still uses 1x latency
#     (REFCOLL_ACTIV is only armed in fixed-latency mode); patched to apply
#     the 2x latency it signals, as the datasheet requires. (Its 2x count
#     itself is datasheet-correct: 1x + L clocks.)

HR_MODEL     ?= $(HOME)/GitHub/psram-tang-nano-9k/library/3rdparty/002-26518/s27kl0642.v
GOWIN_SIMLIB ?= /Applications/GowinIDE.app/Contents/Resources/Gowin_EDA/IDE/simlib/gw5a/prim_sim.v
HR_BUILD     ?= temp/hyperram
HR_DLYS      ?= 0 1000 2000 3000 4000
HR_SEED      ?= 1
HR_NTRANS    ?= 450

HR_RTL       = ../../../hdl/hyperram/hyperram_ctrl.sv ../../../hdl/hyperram/hyperram_phy_gw5a.sv
HR_BIST_RTL  = ../../../hdl/hyperram/hyperram_bist.sv
HR_TB_COMMON = test_hyperram_harness.sv
HR_PMODEL    = $(HR_BUILD)/s27kl0642_patched.v
HR_IVFLAGS   = -g2012 -Wno-timescale -Wno-implicit-dimensions
# iverilog does not support the model's specify timing checks (the
# harness checks timing instead); drop that warning flood
HR_IVFILTER  = 2>&1 | grep -v 'Timing checks are not supported' || true

$(HR_BUILD):
	@mkdir -p $(HR_BUILD)

$(HR_PMODEL): $(HR_MODEL) | $(HR_BUILD)
	@test -f "$(HR_MODEL)" || { echo "HR_MODEL not found: $(HR_MODEL)"; exit 1; }
	@sed -e 's/^`define S27KS0642GABHI/`define S27KL0642DPBHI/' "$(HR_MODEL)" | \
	  perl -0pe "s/(REFCOLL = 1'b1;\s*\n\s*if \(Config_reg0\[3\] == 1'b1)\)/\$$1 || RWDSout_zd_tmp === 1'b1) \/\/ a2fpga patch: variable-latency collision/" \
	  > $@.tmp
	@grep -q '^`define S27KL0642DPBHI' $@.tmp || { echo "model part patch failed"; exit 1; }
	@grep -q 'a2fpga patch: variable-latency' $@.tmp || { echo "model collision patch failed"; exit 1; }
	@mv $@.tmp $@

# Compile order: Gowin primitives first (they carry `timescale 1ns/1ps for
# the RTL that follows), then RTL, model, harness, testbench.
$(HR_BUILD)/hyperram_ctrl.vvp: $(HR_RTL) $(HR_TB_COMMON) test_hyperram_ctrl.sv $(HR_PMODEL)
	iverilog $(HR_IVFLAGS) -s test_hyperram_ctrl -o $@ \
	  "$(GOWIN_SIMLIB)" $(HR_RTL) $(HR_PMODEL) $(HR_TB_COMMON) test_hyperram_ctrl.sv $(HR_IVFILTER)
	@test -f $@

hyperram: $(HR_BUILD)/hyperram_ctrl.vvp
	@echo "=== HyperRAM controller: board delay sweep ($(HR_DLYS) ps) ==="
	@rm -f $(HR_BUILD)/summary.txt; fail=0; \
	for d in $(HR_DLYS); do \
	  vvp -n $< +BOARD_DLY_PS=$$d +SEED=$(HR_SEED) +NTRANS=$(HR_NTRANS) > $(HR_BUILD)/ctrl_dly$$d.log 2>&1; \
	  grep -E 'init_done' $(HR_BUILD)/ctrl_dly$$d.log | sed 's/^/  /'; \
	  line=$$(grep -E 'HYPERRAM_CTRL (PASS|FAIL)' $(HR_BUILD)/ctrl_dly$$d.log | tail -1); \
	  [ -n "$$line" ] || line="HYPERRAM_CTRL FAIL dly=$${d}ps (no result, see $(HR_BUILD)/ctrl_dly$$d.log)"; \
	  echo "$$line" | tee -a $(HR_BUILD)/summary.txt; \
	  echo "$$line" | grep -q PASS || fail=1; \
	done; \
	if [ $$fail = 0 ]; then echo "=== HYPERRAM SWEEP PASS ==="; else echo "=== HYPERRAM SWEEP FAIL (logs in $(HR_BUILD)) ==="; exit 1; fi

# BIST
$(HR_BUILD)/hyperram_bist.vvp: $(HR_RTL) $(HR_BIST_RTL) $(HR_TB_COMMON) test_hyperram_bist.sv $(HR_PMODEL)
	iverilog $(HR_IVFLAGS) -s test_hyperram_bist -o $@ \
	  "$(GOWIN_SIMLIB)" $(HR_RTL) $(HR_BIST_RTL) $(HR_PMODEL) $(HR_TB_COMMON) test_hyperram_bist.sv $(HR_IVFILTER)
	@test -f $@

HR_BIST_DLYS ?= 0 2000 4000
hyperram_bist: $(HR_BUILD)/hyperram_bist.vvp
	@echo "=== HyperRAM BIST: board delay sweep ($(HR_BIST_DLYS) ps) ==="
	@fail=0; \
	for d in $(HR_BIST_DLYS); do \
	  vvp -n $< +BOARD_DLY_PS=$$d +SEED=$(HR_SEED) > $(HR_BUILD)/bist_dly$$d.log 2>&1; \
	  grep -E 'init_done|^\[[0-9]+\] [1-8] |eye:|ERROR' $(HR_BUILD)/bist_dly$$d.log | sed 's/^/  /'; \
	  line=$$(grep -E 'HYPERRAM_BIST (PASS|FAIL)' $(HR_BUILD)/bist_dly$$d.log | tail -1); \
	  [ -n "$$line" ] || line="HYPERRAM_BIST FAIL dly=$${d}ps (no result, see $(HR_BUILD)/bist_dly$$d.log)"; \
	  echo "$$line"; \
	  echo "$$line" | grep -q PASS || fail=1; \
	done; \
	if [ $$fail = 0 ]; then echo "=== HYPERRAM BIST PASS ==="; else echo "=== HYPERRAM BIST FAIL (logs in $(HR_BUILD)) ==="; exit 1; fi

# mem_port_if arbiter (hyperram_ports) over the controller + model
#   make hyperram_ports
HR_PORTS_RTL = ../../../hdl/hyperram/hyperram_ports.sv
$(HR_BUILD)/hyperram_ports.vvp: $(HR_RTL) $(HR_PORTS_RTL) $(HR_TB_COMMON) test_hyperram_ports.sv $(HR_PMODEL)
	iverilog $(HR_IVFLAGS) -s test_hyperram_ports -o $@ \
	  "$(GOWIN_SIMLIB)" $(HR_RTL) $(HR_PORTS_RTL) $(HR_PMODEL) $(HR_TB_COMMON) test_hyperram_ports.sv $(HR_IVFILTER)
	@test -f $@

hyperram_ports: $(HR_BUILD)/hyperram_ports.vvp
	vvp -n $(HR_BUILD)/hyperram_ports.vvp | grep -v 'Timing checks'

hyperram_clean:
	rm -rf $(HR_BUILD)

.PHONY: hyperram hyperram_bist hyperram_clean

# Standalone PSRAM bitstream system test: a2p25_psram_test_top (CK 108)
# driven over its console UART against the vendor model.
#   make psram_test_top
PT_RTL = ../hdl/psram_test/psram_test_defines_ck108.v sim_models/hr_clk_pll_sim.v \
         ../hdl/gowin/gowin_pll/gowin_pll.v ../hdl/gowin/gowin_pll/gowin_pll_mod.v ../hdl/gowin/pll_init.v \
         ../hdl/psram_test/hr_uart_cmd.sv ../hdl/psram_test/a2p25_psram_test_top.sv
$(HR_BUILD)/psram_test_top.vvp: $(HR_RTL) $(HR_BIST_RTL) $(PT_RTL) $(HR_TB_COMMON) test_psram_test_top.sv $(HR_PMODEL)
	iverilog $(HR_IVFLAGS) -s test_psram_test_top -o $@ \
	  "$(GOWIN_SIMLIB)" $(HR_RTL) $(HR_BIST_RTL) $(PT_RTL) $(HR_PMODEL) $(HR_TB_COMMON) test_psram_test_top.sv $(HR_IVFILTER)
	@test -f $@

psram_test_top: $(HR_BUILD)/psram_test_top.vvp
	vvp $(HR_BUILD)/psram_test_top.vvp | grep -v 'Timing checks'
