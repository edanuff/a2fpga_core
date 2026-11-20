// GS socket clocking — constraints the a2mega SDCs gain when the in-socket
// 65C816 is integrated (S4).  Validated on a 138B PnR probe of
// gs_socket_phy + P65C816 on the real GS balls (2026-09-05: 0/0, clk108
// +1.47 ns reported under the 0.5 ns policy, gs_ph2 +322 ns).
//
// gs_ph2: the IIgs FPI's PHI2 at the socket.  2.86 MHz fast cycle, ~140 ns
// low / ~209 ns high.  The 65C816 core is clocked by its inversion (active
// edge = PHI2 fall); the tool derives that from the inverter.  The period
// is the FAST cycle; sync/slow cycles are longer (only ever more slack).
// On 1.0a3 the PH2 ball (AB17) is not a dedicated clock input, so PnR
// reports "Generic routing resource will be used to clock signal" — that
// is expected and harmless at 2.86 MHz.
create_clock -name gs_ph2 -period 349.0 -waveform {209.4 349.0} [get_ports {gs_ph2}]

// The pin sequencer runs on the ~108 MHz PLL output (named per the top
// level's create_clock).  The two domains are asynchronous: the only
// crossings are the per-cycle toggle (two-flop synchronised in
// gs_socket_phy) and the core's quasi-static outputs, sampled >= 37 ns
// after the fall by design (OUT_DELAY = 2).  STA cannot see that margin:
// the core's longest internal path bounds its output settle at ~27 ns
// (Fmax 37 MHz on the probe), so ~10 ns of margin remains.  Keep
// OUT_DELAY >= 2 unless that bound is re-measured.
set_clock_groups -asynchronous -group [get_clocks {gs_ph2}] -group [get_clocks {clk108}]

// Durability margin policy (boards/a2mega/docs/timing_round2.md) on the
// sequencer clock is applied where clk108 is created.
