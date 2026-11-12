// S7 accelerator timing probe (accel_probe_top): the accelerator domain at the
// 108 MHz pin-sequencer clock, the core stepping every STEP_CLKS = 3 clks
// (36 MHz).  The exceptions below are the timing contract gs_accel_broker
// documents and tb_gs_broker.sv asserts in clks (step spacing, read launch ->
// use, select steady, no decision before the second clk of a step); everything
// not listed is an ordinary one-clk path.
// See boards/a2mega/docs/gs_socket_s7_fast_core_exploration.md.
create_clock -name clk108 -period 9.259 -waveform {0 4.630} [get_ports {clk50_in}]
set_clock_uncertainty 0.5 -setup -from [get_clocks {clk108}]

// gs_ph2 and the sequencer clock are asynchronous (hdl/twgs/gs_socket.sdc).  The
// socket data byte (u_brk/d_f, PHI2-fall flops) is taken by the core the clk after
// the synchronised fall event: at least three clks after the fall.
create_clock -name gs_ph2 -period 349.0 -waveform {209.4 349.0} [get_ports {gs_ph2}]
set_clock_groups -asynchronous -group [get_clocks {gs_ph2}] -group [get_clocks {clk108}]

// ---- STEP_CLKS-cycle paths (three clks): launched at a core step, captured at the next.
// Startpoints: the core's registers; the memories' read ports (and the fabric
// registers the synthesizer keeps the upper read-address bits in, ram_/rom_ADBREG_G,
// which select the output mux); the data-source select.
// Endpoints: the core's registers; the next read address (pins and ADBREG);
// what the broker registers at the launch (addresses held, select, the next
// step's decode).
set_multicycle_path 3 -setup -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 3 -setup -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 2 -hold  -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 3 -setup -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 2 -hold  -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 3 -setup -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 2 -hold  -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 3 -setup -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 2 -hold  -end -from [get_pins {ram_ram_*/CLKB rom_rom_*/CLKB}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 3 -setup -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 2 -hold  -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 3 -setup -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 2 -hold  -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 3 -setup -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 2 -hold  -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 3 -setup -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 2 -hold  -end -from [get_regs {ram_ADBREG_G* rom_ADBREG_G*}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 3 -setup -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]
set_multicycle_path 2 -hold  -end -from [get_regs {u_brk/use_lm_r* u_brk/rom_r*}] -to [get_regs {u_brk/ld_phys* u_brk/ld_ram* u_brk/ld_rom* u_brk/use_lm_r* u_brk/rom_r* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* lm_raddr_q*}]

// ---- ADDR_CLKS-cycle paths (two clks): everything else that is registered from
// the core's outputs, and from the broker registers that change only when the core
// steps (the step-ahead decode, the mode).  The broker's strobe (and its memory
// actions, later still) is never before the second clk of a step, the PHY takes a
// request at its issue point two clks after the core steps, and the registered copy
// of the decode (aq_*) is looked at only in the third clk.  The RAM write port is
// written at the strobe.
set_multicycle_path 2 -setup -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {u_brk/aq_* u_brk/sreq_* u_brk/pw_* u_brk/ml_* u_brk/st_* u_brk/ph_* u_brk/ce_r* u_brk/vfy_r* u_brk/ld_valid* u_brk/fl_* u_brk/prev_addr* u_brk/stat_* u_phy/*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_regs {u_brk/aq_* u_brk/sreq_* u_brk/pw_* u_brk/ml_* u_brk/st_* u_brk/ph_* u_brk/ce_r* u_brk/vfy_r* u_brk/ld_valid* u_brk/fl_* u_brk/prev_addr* u_brk/stat_* u_phy/*}]
set_multicycle_path 2 -setup -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_pins {ram_ram_*/DI* ram_ram_*/CEA ram_ram_*/ADA*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_cpu/* u_cpu/MCode/* u_cpu/AddrGen/*}] -to [get_pins {ram_ram_*/DI* ram_ram_*/CEA ram_ram_*/ADA*}]
set_multicycle_path 2 -setup -end -from [get_regs {u_brk/pl_fresh* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* u_brk/mode_native* u_brk/mode_bypass*}] -to [get_regs {u_brk/aq_* u_brk/sreq_* u_brk/pw_* u_brk/ml_* u_brk/st_* u_brk/ph_* u_brk/ce_r* u_brk/vfy_r* u_brk/ld_valid* u_brk/fl_* u_brk/prev_addr* u_brk/stat_* u_phy/*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_brk/pl_fresh* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* u_brk/mode_native* u_brk/mode_bypass*}] -to [get_regs {u_brk/aq_* u_brk/sreq_* u_brk/pw_* u_brk/ml_* u_brk/st_* u_brk/ph_* u_brk/ce_r* u_brk/vfy_r* u_brk/ld_valid* u_brk/fl_* u_brk/prev_addr* u_brk/stat_* u_phy/*}]
set_multicycle_path 2 -setup -end -from [get_regs {u_brk/pl_fresh* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* u_brk/mode_native* u_brk/mode_bypass*}] -to [get_pins {ram_ram_*/DI* ram_ram_*/CEA ram_ram_*/ADA*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_brk/pl_fresh* u_brk/pl_r_* u_brk/pl_w_* u_brk/pl_side* u_brk/mode_native* u_brk/mode_bypass*}] -to [get_pins {ram_ram_*/DI* ram_ram_*/CEA ram_ram_*/ADA*}]

// ---- a read that was not launched ahead: its registered address is presented to
// the memories for two clks and launched in the second
set_multicycle_path 2 -setup -end -from [get_regs {u_brk/ml_sel_r* u_brk/ml_phys* u_brk/ml_rom*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_brk/ml_sel_r* u_brk/ml_phys* u_brk/ml_rom*}] -to [get_pins {ram_ram_*/ADB* rom_rom_*/ADB*}]
set_multicycle_path 2 -setup -end -from [get_regs {u_brk/ml_sel_r* u_brk/ml_phys* u_brk/ml_rom*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_brk/ml_sel_r* u_brk/ml_phys* u_brk/ml_rom*}] -to [get_regs {ram_ADBREG_G* rom_ADBREG_G*}]

// ---- the broker's view of the core's reset changes at a PHI2-fall event; the PHY
// takes a cycle at its issue point, never sooner than two clks after one
set_multicycle_path 2 -setup -end -from [get_regs {u_brk/run_c*}] -to [get_regs {u_phy/*}]
set_multicycle_path 1 -hold  -end -from [get_regs {u_brk/run_c*}] -to [get_regs {u_phy/*}]
