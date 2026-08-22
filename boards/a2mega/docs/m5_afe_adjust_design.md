# M5 — runtime TX-AFE adjustment: design + sim results + proposed shared-RTL diffs

Status 2026-08-21: **DESIGNED AND SIM-VERIFIED** (unit + closed-loop, both
PASS under iverilog -g2012). New shared module + testbenches are COMMITTED;
modifications to existing shared production RTL are delivered here as
PROPOSE-ONLY diffs (per hdl/displayport/** policy) — the exact patched
texts live in `hdl/displayport/sim/m5_proposed/` and are what the
closed-loop sim ran against, so the diffs below are sim-verified by
construction. No hardware was touched; no Gowin builds run.

Companion: `m5_runtime_afe.md` (the verified register model, sequencing
law, and DP level mapping this design implements).

## 1. What the feature does

During link training, the sink's ADJUST_REQUEST (DPCD 0x206/0x207, parsed
by `link_signal_mgmt.v` into `channel_adjust` / exported as
`debug_adjust`) currently changes nothing — the TX drive is fixed by the
bitstream CSR config and TRAINING_LANE_SET truthfully declares that fixed
level (msg 0x18 = swing 2 + MAX_SWING_REACHED, the "truthful
declarations" policy). M5 closes the loop:

1. requested (vs, pe) levels are **applied** to the GTR12 TX AFE at
   runtime via DRP writes (the 4-write-per-lane sequence from
   m5_runtime_afe.md), and
2. TRAINING_LANE_SET then **reports the actually-applied** levels + cap
   flags, so the sink's calibration matches the wire.

Everything is behind `ENABLE_AFE_ADJUST` (default 0). Off = current
fixed-swing behavior, proven **byte-identical** on the AUX wire (see §6).

## 2. Architecture

```
                    mgmt_clk (clk100)                    drp_clk (arbiter drp_clk_o)
  aux_channel  --adjust_de/aux_addr--> channel_management
      |                                     |  adjust_evt (0x206 byte pulse)
      |  msg 0x19: TRAINING_LANE_SET        v
  dp_aux_messages <--train_set_byte-- afe_adjust_seq --drp_req/gnt,addr,data,wren/ready--+
      (dynamic byte)                   (NEW, committed)                                  |
                                            ^                                           v
  link_signal_mgmt --debug_adjust[3:0]------+                          transceiver_bank_gowin
                                                                        (existing read/replay DRP
  tx_clock_train | tx_align_train  --training_active--^                  engine + NEW grant mux)
                                                                              |
                                                                        dp_serdes dp_phy_drp_* port
```

- **`hdl/displayport/gowin/afe_adjust_seq.v`** (NEW, committed): trigger
  policy + bookkeeping in the mgmt domain, per-lane write-sequence player
  in the DRP domain, toggle-handshake CDC between them. Fully
  parameterized (lane bases, lane count, INIT levels, enable).
- The generated `dp_serdes` integration top exposes ONE fabric DRP
  channel (`dp_phy_drp_*`, behind the 8-channel `upar_arbiter`);
  `transceiver_bank_gowin.v` already time-shares it between the
  background register dump and the watchdog CSR replay. M5 adds a third
  client via a req/gnt pair: the AFE player owns the port for one whole
  8-write sequence (~µs), replay keeps priority for **new** grants
  (recovery trumps tuning), background reads fill the gaps.
- `aux_channel`/`dp_aux_messages` gain a **dynamic** TRAINING_LANE_SET
  message (0x19) whose 4 lane bytes come from `train_set_byte`; the
  legacy constant message 0x18 remains untouched and is still what ships
  when `AFE_ADJUST == 0`.

## 3. Trigger policy (sim-verified)

- Active only while a training pattern is on the wire:
  `training_active = tx_clock_train | tx_align_train` (the aux_channel
  ladder asserts these through the whole TPS1 clock_* / TPS2 align_*
  state families).
- **Training start**: on the first idle evaluation after
  `training_active` rises, the `INIT_VS/INIT_PE` baseline (default
  VS2/PE0 = 804 mV, 0 dB — today's proven analog point) is applied once.
  This makes the analog state deterministic per training run regardless
  of what a previous run left in the CSRs, and gives the early
  TRAINING_LANE_SET writes a truthful value (0x02) before the sink has
  asked for anything. `APPLY_ON_TRAINING_START=0` skips the write and
  just assumes the resident config equals INIT.
- **Per iteration**: evaluation happens ONLY on `adjust_evt` — the pulse
  accompanying each received DPCD 0x206 byte (one per
  clock_adjust/align_adjust read = one per training iteration). The
  request is applied iff it differs from the currently-applied (vs, pe).
  This is the hard once-per-iteration debounce, and it also keeps the
  powerup-cleared request register (0x0000 = "sink hasn't asked yet")
  from being mistaken for a VS0/PE0 request — a live-fire hazard found in
  sim: with plain level-comparison the INIT baseline was immediately
  dragged to 420 mV by the cleared register.
- Applications are atomic: a new request arriving while a sequence is in
  flight is latched and applied after (verified — no interleaving, none
  lost).
- On `training_active` falling, `applied_known` clears: the next training
  run re-baselines. Requests outside training are ignored.

Lane granularity: lane0's request (0x206[1:0]/[3:2]) is applied to BOTH
lanes — common-level tuning, matching the single train_set_byte
declaration. Per-lane differentiation (0x206[7:4] for lane 1) is a
straightforward extension (second payload pair + per-lane bytes in msg
0x19) left for a follow-up if the bench ever shows per-lane asymmetry.

## 4. Level mapping, cap semantics, reporting

Per m5_runtime_afe.md (swing law Vdiffpp = 180 + 48·txlev):

| DP VS req | txlev applied (mV) | reported |
|---|---|---|
| 0 | 5 (420) | vs=0 |
| 1 | 9 (612) | vs=1 |
| 2 | 13 (804) | vs=2 |
| 3 | 15 (900 = ceiling) | vs=3 + **MAX_SWING_REACHED** |

| DP PE req | C1 applied (dB) | reported |
|---|---|---|
| 0 | 0 (0) | pe=0 |
| 1 | 7 (3.7) | pe=1 |
| 2 | 10 (6.0) | pe=2 |
| 3 | 13 (9.1 = ceiling) | pe=3 + **MAX_PE_REACHED** |

CM stays 0. Writes are full-value (other bits 0), matching both the boot
.csr emission and the IDE dialog exports — the verified law.

`train_set_byte = {2'b00, MAX_PE, pe[1:0], MAX_SWING, vs[1:0]}` (the
DPCD TRAINING_LANEx_SET layout; the legacy constant 0x06 decodes as
vs=2+MAX_SWING in the same encoding, which is the byte the module emits
whenever the feature is off or no training run has begun).

Reporting decision — **echo the requested level, flag the cap**: a VS3
request applies txlev 15 (900 mV, our physical ceiling — a real increase
over VS2's 804 mV) and reports vs=3 + MAX_SWING. The alternative reading
of the spec ("set the max level you SUPPORT" → report vs=2 + MAX_SWING
while driving 900 mV) is defensible too; echo-with-flag was chosen
because sinks terminate their escalation on either form, but some bridge
implementations loop-detect on "request not honored" — and the flag
truthfully declares the ceiling either way. Flip = one line in
`afe_adjust_seq.v` (documented in the module header); worth an A/B on the
bench if a picky sink shows up.

## 5. The committed module: `hdl/displayport/gowin/afe_adjust_seq.v`

Parameters: `ENABLE_AFE_ADJUST` (0 = all outputs constant tie-offs,
train_set_byte = IDLE_SET_BYTE = 0x06 — byte-identical legacy),
`NUM_LANES` (1-4), `LANE_BASE0..3` (138B defaults 0x808300/0x808400 —
**per-die values; do NOT assume for the 60K**, see §8),
`INIT_VS/INIT_PE`, `APPLY_ON_TRAINING_START`, `IDLE_SET_BYTE`.

Interfaces:
- mgmt side: `vs_request/pe_request` (wire to `debug_adjust[1:0]/[3:2]`),
  `adjust_evt` pulse, `training_active`; outputs `train_set_byte`,
  `afe_busy`, `dbg_afe` = {seq_err, applied_known, pe[1:0], vs[1:0]}
  (telemetry-ready for the UART/telnet line).
- DRP side: `drp_req/drp_gnt` + `addr/wrdata/wren/ready`. Write
  choreography mirrors the bank's replay engine: wren held with
  addr/data until `ready` (12-bit timeout, sticky `seq_err`, sequence
  still completes — no wedge), one-cycle-plus gap between writes. The
  bank derives `strb = wren ? 8'hFF : 8'h00` at the port, identically to
  its existing writes.

CDC: `apply_tgl`/`ack_tgl` toggle handshake; `vs_lat/pe_lat` and the
debug fields are quasi-static (settled ≥ 2 destination clocks before the
toggle edge lands). Constrain the module's mgmt↔drp crossings as false
paths (same class as the bank's dbg_*/replay handshakes — see §9).

## 6. Simulation results (the deliverable gate)

`/usr/local/bin/iverilog -g2012`, both PASS:

**Unit — `hdl/displayport/sim/tb_afe_adjust.v`** (afe_adjust_seq + mock
DRP slave recording every write; async 100 MHz / 122 MHz clocks):
1. cold idle: zero DRP activity, byte 0x06;
2. training start applies INIT once — exactly 8 writes, per-lane
   complete, law order (+0x34 = txlev<<12, +0x38 = C1<<8|CM,
   +0xd8 = 0x010 enable, +0xd8 = 0x110 apply strobe), lane bases
   0x8083xx then 0x8084xx, byte → 0x02;
3. request == applied: no writes;
4. VS3/PE2 → one application (txlev 15 / C1 10), byte 0x17; repeated
   identical requests debounced (no further writes);
5. PE3 → C1 13, byte 0x3F (both cap flags);
6. requests with training_active=0 ignored;
7. re-training re-applies INIT, then VS1/PE1 → txlev 9 / C1 7, byte 0x09;
8. mid-flight request change: two atomic back-to-back sequences, final
   applied = latest request, no interleave/loss;
9. write timeout: sequence completes, sticky seq_err, no wedge;
10. ENABLE=0 twin under identical stimuli: zero DRP activity, drp_req
    never asserts, byte constant 0x06.

**Closed-loop — `hdl/displayport/sim/tb_afe_adjust_closedloop.v`**: the
REAL ladder (patched `channel_management`→`aux_channel`→`aux_interface`
from `sim/m5_proposed/`, AFE_ADJUST=1, BLIND_SINK=0) against a scripted
Manchester AUX sink model (decodes native DPCD requests from the wire,
ACKs, serves caps/status/adjust from a DPCD model, logs every
TRAINING_LANE_SET/PATTERN write) plus afe_adjust_seq + mock DRP slave:
- ladder trains to `link_established` (proposed changes don't break
  training; sink script: CR granted only after the 2nd AFE application,
  EQ after TPS2);
- INIT applied at training start and declared **on the wire** (0x02);
- sink ADJUST_REQUEST 0xBB (VS3/PE2 both lanes) → exactly ONE further
  application (16 DRP writes total), every subsequent wire lane-set =
  0x17 (vs3+MAX_SWING, pe2), align-phase repeat debounced;
- every 0x103 write carries identical bytes on all four lanes; no value
  other than 0x02/0x17 ever appears;
- after training completes the declaration reverts to idle 0x06 by
  design (no lane-set is sent in that window; a retrain re-baselines).

**Byte-identity of the legacy path**: a harness drove all 24 message
codes used by aux_channel through the production `dp_aux_messages` and
the patched one (train_set_byte tied 0x06); the logged
(msg, byte)-streams are **identical** (139-line diff = empty). Together
with `AFE_ADJUST=0` ⇒ `lane_set_msg = 0x18` (same constant path) and the
ENABLE=0 tie-offs, default-off is byte-identical end to end.

Run commands:
```
iverilog -g2012 -o /tmp/tb_afe_adjust.vvp \
  hdl/displayport/sim/tb_afe_adjust.v hdl/displayport/gowin/afe_adjust_seq.v \
  && vvp /tmp/tb_afe_adjust.vvp

iverilog -g2012 -o /tmp/tb_afe_cl.vvp \
  hdl/displayport/sim/tb_afe_adjust_closedloop.v \
  hdl/displayport/sim/m5_proposed/channel_managemnt.v \
  hdl/displayport/sim/m5_proposed/aux_channel.v \
  hdl/displayport/sim/m5_proposed/dp_aux_messages.v \
  hdl/displayport/auxch/aux_interface.v hdl/displayport/auxch/link_signal_mgmt.v \
  hdl/displayport/auxch/edid_decode.v hdl/displayport/auxch/dp_register_decode.v \
  hdl/displayport/auxch/hotplug_decode.v hdl/displayport/gowin/afe_adjust_seq.v \
  && vvp /tmp/tb_afe_cl.vvp        # add +verbose for wire/decode traces
```

## 7. Proposed shared-RTL diffs

### 7a. VERIFIED (exact texts in `hdl/displayport/sim/m5_proposed/`)

These three are the diffs the closed-loop sim ran against. Apply by
copying the m5_proposed file over the auxch one (or applying the diff —
identical result; regenerate any time with
`diff -u hdl/displayport/auxch/<f>.v hdl/displayport/sim/m5_proposed/<f>.v`).

**`hdl/displayport/auxch/dp_aux_messages.v`** — new input + message 0x19:

```diff
@@ -68,6 +68,11 @@
    input  [7:0] msg,
    output reg   busy,
 
+   // M5 runtime AFE adjust: the TRAINING_LANEx_SET value message 0x19
+   // declares — the ACTUALLY-APPLIED swing/pre-emphasis + cap flags from
+   // afe_adjust_seq. Tie to 8'h06 (legacy swing2+MAX_SWING) when unused.
+   input  [7:0] train_set_byte,
+
    // Interface to the AUX Channel
    output reg       aux_tx_wr_en,
    output reg [7:0] aux_tx_data
@@ -191,7 +196,20 @@
        12'h0F3: begin aux_tx_data <= 8'h00; aux_tx_wr_en <= 1'b1; end
        12'h0F4: begin aux_tx_data <= 8'h22; aux_tx_wr_en <= 1'b1; end
 
-       // Resd lane align status for all four lanes 
+       // M5: dynamic TRAINING_LANEx_SET (DPCD 0x103-0x106) — declares the
+       // levels the TX-AFE ACTUALLY drives (registered train_set_byte from
+       // afe_adjust_seq) instead of a fixed constant. Selected by
+       // aux_channel in every set state when AFE_ADJUST != 0.
+       12'h190: begin aux_tx_data <= 8'h80; aux_tx_wr_en <= 1'b1; end
+       12'h191: begin aux_tx_data <= 8'h01; aux_tx_wr_en <= 1'b1; end
+       12'h192: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
+       12'h193: begin aux_tx_data <= 8'h03; aux_tx_wr_en <= 1'b1; end
+       12'h194: begin aux_tx_data <= train_set_byte; aux_tx_wr_en <= 1'b1; end
+       12'h195: begin aux_tx_data <= train_set_byte; aux_tx_wr_en <= 1'b1; end
+       12'h196: begin aux_tx_data <= train_set_byte; aux_tx_wr_en <= 1'b1; end
+       12'h197: begin aux_tx_data <= train_set_byte; aux_tx_wr_en <= 1'b1; end
+
+       // Resd lane align status for all four lanes
        12'h100: begin aux_tx_data <= 8'h90; aux_tx_wr_en <= 1'b1; end
```

**`hdl/displayport/auxch/aux_channel.v`** — AFE_ADJUST param, message
select, pass-through:

```diff
@@ -70,9 +70,16 @@
 module aux_channel #(
     parameter LINK_RATE_MBPS = 2700,
-    parameter BLIND_SINK = 0
+    parameter BLIND_SINK = 0,
+    // M5: 1 = closed-loop TX-AFE adjust — TRAINING_LANEx_SET declares the
+    // registered train_set_byte (msg 0x19) instead of the fixed 0x06
+    // constant (msg 0x18). 0 = legacy, byte-identical.
+    parameter AFE_ADJUST = 0
 )(
         input        clk,
+        // ready-to-send TRAINING_LANEx_SET value from afe_adjust_seq
+        // (tie to 8'h06 when AFE_ADJUST == 0)
+        input  [7:0] train_set_byte,
         output [7:0] debug_pmod,
@@ -254,8 +261,12 @@
 end
 
+    // M5: which TRAINING_LANEx_SET message the set states send
+    wire [7:0] lane_set_msg = (AFE_ADJUST != 0) ? 8'h19 : 8'h18;
+
 dp_aux_messages #(.LINK_RATE_MBPS(LINK_RATE_MBPS)) i_aux_messages(
          .clk          (clk),
+         .train_set_byte (train_set_byte),
```
…and all 12 training-set states (`clock_voltage_0p4/0p6/0p8`,
`align_p0/p1/p2_V0p4/0p6/0p8`) change `msg <= 8'h18;` →
`msg <= lane_set_msg;` (the ladder keeps its state walk for pacing/retry
structure exactly as the truthful-declarations comment describes; with
AFE off the emitted bytes are unchanged).

**`hdl/displayport/auxch/channel_managemnt.v`** — param + plumbing +
adjust_evt export:

```diff
@@ -60,9 +60,15 @@
 module channel_management #(
     parameter LINK_RATE_MBPS = 2700,
-    parameter BLIND_SINK = 0   // open-loop link policy (see aux_channel.v)
+    parameter BLIND_SINK = 0,  // open-loop link policy (see aux_channel.v)
+    parameter AFE_ADJUST = 0   // M5: closed-loop TX-AFE adjust (aux_channel.v)
 )(
         input  clk100,
+        // M5 runtime AFE adjust: applied-level declaration in, one pulse
+        // out per received ADJUST_REQUEST 0x206 byte (afe_adjust_seq's
+        // evaluation/debounce tick). train_set_byte: tie 8'h06 when off.
+        input  [7:0] train_set_byte,
+        output       adjust_evt,
         output [7:0] debug,
@@ -204,10 +210,16 @@
     );
+
+    // M5: the ADJUST_REQUEST lane0/1 byte (DPCD 0x206) is the aux_addr==0
+    // beat of the adjust read — one pulse per training iteration.
+    assign adjust_evt = adjust_de & (aux_addr == 8'h00);
 
 aux_channel #(.LINK_RATE_MBPS(LINK_RATE_MBPS),
-              .BLIND_SINK(BLIND_SINK)) i_aux_channel(
+              .BLIND_SINK(BLIND_SINK),
+              .AFE_ADJUST(AFE_ADJUST)) i_aux_channel(
         .clk             (clk100),
+        .train_set_byte  (train_set_byte),
```

Compatibility: the other in-tree instantiator of `channel_management`
(board-local `boards/a2mega/hdl/dp_gowin/dp_gowin_transmitter.sv`) keeps
compiling unmodified — the new ports are simply unconnected and unused at
the default `AFE_ADJUST=0` — but should get explicit
`.train_set_byte(8'h06), .adjust_evt()` ties when the diffs land.

### 7b. DRAFT (same port protocol the mock DRP slave verified; not yet
### compiled against the generated IP — apply with the 7a set and build)

**`hdl/displayport/gowin/transceiver_bank_gowin.v`** — auxiliary DRP
write port + grant mux in the existing engine.

New ports (after `dbg_wrusewd`):
```verilog
    // M5 runtime AFE adjust (afe_adjust_seq): auxiliary DRP WRITE port,
    // arbitrated against the background register dump and the CSR replay.
    // Same clock domain as the engine (drp_clk_o, exported below); the
    // player owns the port for one whole sequence per grant (~us).
    output            afe_drp_clk,
    input             afe_drp_req,
    output            afe_drp_gnt,
    input      [23:0] afe_drp_addr,
    input      [31:0] afe_drp_wrdata,
    input             afe_drp_wren,
    output            afe_drp_ready
```

Inside the `GOWIN_SERDES_IP` section (once, above the lane-count
branches, next to the existing drp_* regs):
```verilog
    // M5 AFE port: pass-through mux; grant owned by the engine FSM below
    reg  afe_gnt_r = 1'b0;
    assign afe_drp_clk   = drp_clk_w;
    assign afe_drp_gnt   = afe_gnt_r;
    assign afe_drp_ready = drp_ready_w;
    wire        drp_wren_mux   = afe_gnt_r ? afe_drp_wren   : drp_wren_r;
    wire [23:0] drp_addr_mux   = afe_gnt_r ? afe_drp_addr   : drp_addr_r;
    wire [31:0] drp_wrdata_mux = afe_gnt_r ? afe_drp_wrdata : drp_wrdata_r;
```

In each of the three `dp_serdes` instantiations, the DRP port
connections become:
```verilog
        .dp_phy_drp_addr_i          (drp_addr_mux),
        .dp_phy_drp_wren_i          (drp_wren_mux),
        .dp_phy_drp_wrdata_i        (drp_wrdata_mux),
        .dp_phy_drp_strb_i          (drp_wren_mux ? 8'hFF : 8'b0),
```
(rden stays on `drp_rden_r`; the engine never reads while `afe_gnt_r` —
its FSM is parked in state 0 for the duration of the grant.)

Engine FSM, state `3'd0` becomes:
```verilog
            3'd0: begin
                drp_rden_r <= 1'b0;
                drp_wren_r <= 1'b0;
                if (afe_gnt_r) begin
                    // AFE player owns the port until its req falls
                    if (!afe_drp_req) begin
                        afe_gnt_r <= 1'b0;
                        rd_gap    <= 18'd0;
                    end
                end else if (afe_drp_req && !replay_pend) begin
                    // recovery replay outranks tuning for NEW grants
                    afe_gnt_r <= 1'b1;
                end else begin
                    rd_gap <= rd_gap + 18'd1;
                    if (replay_pend ? rd_gap[4] : (&rd_gap)) begin
                        ... (existing body unchanged) ...
                    end
                end
            end
```

Stub (`else`, non-IP) branch additions so full-design sims/lint work:
```verilog
    assign afe_drp_clk   = mgmt_clk;
    assign afe_drp_gnt   = afe_drp_req;
    assign afe_drp_ready = 1'b1;       // instant ack, no DRP in the stub
```
Also update the header's swing/pre-emphasis paragraph: the "future DRP
FSM" the `swing_sel/preemp_sel` comment anticipated is this feature —
those encodes remain available but the M5 path drives levels from
`afe_adjust_seq` via the new port instead.

**`hdl/displayport/dp_transmitter.sv`** — parameter, instance, wiring.

Parameters (after `TX_PROBE`):
```systemverilog
    parameter int ENABLE_AFE_ADJUST = 0,   // M5: honor ADJUST_REQUEST via DRP
    parameter [23:0] AFE_LANE_BASE0 = 24'h808300,  // 138B die values —
    parameter [23:0] AFE_LANE_BASE1 = 24'h808400,  //  re-verify per die!
    parameter [1:0]  AFE_INIT_VS    = 2'd2,
    parameter [1:0]  AFE_INIT_PE    = 2'd0,
```

Signals + instance (next to the channel_management instantiation):
```systemverilog
    logic [7:0]  train_set_byte;
    logic        adjust_evt;
    logic        afe_drp_clk, afe_drp_req, afe_drp_gnt;
    logic        afe_drp_wren, afe_drp_ready;
    logic [23:0] afe_drp_addr;
    logic [31:0] afe_drp_wrdata;

    afe_adjust_seq #(
        .ENABLE_AFE_ADJUST (ENABLE_AFE_ADJUST),
        .NUM_LANES         (2),                 // matches the 2-lane bank
        .LANE_BASE0        (AFE_LANE_BASE0),
        .LANE_BASE1        (AFE_LANE_BASE1),
        .INIT_VS           (AFE_INIT_VS),
        .INIT_PE           (AFE_INIT_PE)
    ) i_afe_adjust (
        .mgmt_clk        (clk100),
        .vs_request      (debug_adjust[1:0]),
        .pe_request      (debug_adjust[3:2]),
        .adjust_de       (adjust_evt),
        .training_active (tx_clock_train | tx_align_train),
        .train_set_byte  (train_set_byte),
        .afe_busy        (),
        .dbg_afe         (),                    // telemetry hook (UART A:-line)
        .drp_clk         (afe_drp_clk),
        .drp_req         (afe_drp_req),
        .drp_gnt         (afe_drp_gnt),
        .drp_addr        (afe_drp_addr),
        .drp_wrdata      (afe_drp_wrdata),
        .drp_wren        (afe_drp_wren),
        .drp_ready       (afe_drp_ready)
    );
```

channel_management instantiation gains:
```diff
     channel_management #(.LINK_RATE_MBPS(LINK_RATE_MBPS),
-                         .BLIND_SINK(BLIND_SINK)) i_channel_management(
+                         .BLIND_SINK(BLIND_SINK),
+                         .AFE_ADJUST(ENABLE_AFE_ADJUST)) i_channel_management(
         .clk100               (clk100),
+        .train_set_byte       (train_set_byte),
+        .adjust_evt           (adjust_evt),
```

transceiver_bank_gowin instantiation (DP_VENDOR_GOWIN branch) gains:
```diff
         .replay_req      (wdog_replay_req),
         .replay_ack      (wdog_replay_ack),
+        .afe_drp_clk     (afe_drp_clk),
+        .afe_drp_req     (afe_drp_req),
+        .afe_drp_gnt     (afe_drp_gnt),
+        .afe_drp_addr    (afe_drp_addr),
+        .afe_drp_wrdata  (afe_drp_wrdata),
+        .afe_drp_wren    (afe_drp_wren),
+        .afe_drp_ready   (afe_drp_ready),
         .dbg_wrusewd     (debug_wrusewd)
```

The Xilinx branch and the behavioural stub tie the AFE port off:
```systemverilog
    assign afe_drp_clk = clk100;
    assign afe_drp_gnt = afe_drp_req;
    assign afe_drp_ready = 1'b1;
```

Board top (`boards/a2mega/hdl/dp_test/a2mega_dp_test_top.sv`,
board-local, apply together with the diffs): add
`.ENABLE_AFE_ADJUST(1)` to the `dp_transmitter #(...)` params for the
experiment build (default stays 0 for cold-golden builds), and
optionally surface `dbg_afe` in a UART field for live telemetry.

## 8. Open items / next steps

1. **Apply + build — DONE 08-21 (c403e98f + 04f6a809)**: 7a 3-way-merged
   onto V2.1 (not copied — the m5_proposed files predate the HPD param),
   7b applied, afe_adjust_seq.v added to the gprj, debug_afe exported
   and serialized as telemetry field `M:` = {seq_err,known} {pe,vs}.
   Closed-loop tb re-run PASS against the real merged auxch files. 138B
   build bin 972110be: timing 0/0 (clk_sym Fmax 149.2), SecurityBit OFF.
   Existing set_clock_groups -asynchronous covers the new CDC pairs the
   same way as the wdog replay handshake (no explicit false paths were
   needed; timing clean). BENCH PENDING: predictions Anker M:12, Ugreen
   M:16 (first-ever pre-emphasis grant), M:2x/3x = seq_err.
2. **SDC**: add false-path (or set_max_delay) on
   `i_afe_adjust/apply_tgl -> tgl_sync`, `ack_tgl -> ack_sync`,
   `vs_lat/pe_lat -> vs_s/pe_s`, `seq_err -> err_sync` — same treatment
   as the existing wdog replay handshake.
3. **Bench validation** (m5_runtime_afe.md step 2): before trusting the
   loop on marginal sinks, fire stored sequences on a live link via the
   telnet hook and watch C:/Y:/A: telemetry; the per-level A/B is also
   the empirical check of the C0=40−C1 FFE model.
4. **60K equivalence**: LANE_BASE values are 138B-verified only. Repeat
   the dialog export from a 60K project before enabling there (per-die
   check — do NOT assume the register family/stride).
5. **INIT choice**: default INIT_VS=2 (804 mV) matches the proven
   baseline; the 900 mV cold-golden point is INIT_VS=3 (declares
   0x07 = vs3+MAX_SWING). The 804/900 cold A/B (test_log row 65) should
   settle the default.
6. **Reporting A/B** (§4): echo-with-flag vs clamp-to-2-with-flag on VS3
   if a sink misbehaves.
7. Per-lane request differentiation if ever needed (§3).

## 9. File inventory

Committed (new files only — no shared production RTL modified):
- `hdl/displayport/gowin/afe_adjust_seq.v` — the feature module
- `hdl/displayport/sim/tb_afe_adjust.v` — unit TB (PASS)
- `hdl/displayport/sim/tb_afe_adjust_closedloop.v` — closed-loop TB (PASS)
- `hdl/displayport/sim/m5_proposed/{dp_aux_messages,aux_channel,channel_managemnt}.v`
  — the exact proposed texts of the 7a diffs (what the closed-loop ran);
  not referenced by any .gprj
- this document

Propose-only (not applied): §7a diffs (verified), §7b drafts
(transceiver_bank_gowin.v, dp_transmitter.sv, board-top parameter).

## 10. Declared-ceiling clamp (adopted 08-21 after first hardware, test log row 75)

First hardware: truthful "VS2, more available" made the Ugreen escalate
to VS3 (A:0033); we applied 900 mV/C1=0 and every catch battled
(Y:77-AA) vs production's clean Y:11s. Our 900 mV hardware maximum sits
nearer DP nominal VS2 (800 mV) than VS3 (1200 mV), so declaring VS3
reachable overstated the PHY. Fix = the §4 reporting flip, made a
parameter: `MAX_VS` (default 2) / `MAX_PE` (default 3) — requests above
the ceiling are clamped and the MAX flag is raised AT the ceiling.
Expected sink behavior: settle at VS2 and spend the next request on
pre-emphasis (as it did under the legacy 0x06 declaration), which is
now GRANTED. Wire byte for VS2+PE1 = 0x0E; telemetry M:06 established
(M:16 while training, known=1). Verified: tb_afe_clamp (INIT at ceiling
= legacy 0x06; VS3 request clamps with no re-application; VS3/PE1 ->
txlev 13/C1 7 byte 0x0E; VS3/PE3 -> both flags 0x3E; VS1 honored 0x01);
existing unit/closed-loop benches pinned to MAX_VS=3 keep their VS3-echo
proofs. Build: see provenance commit. Also learned: 900 mV + C1=0 DOES
train the Ugreen — the old 900 build's 0-for-all was the 900 + C1=8
combination.

## 11. Apply-on-training-start is the wrong default (adopted 08-21, test log rows 76-77)

Row 76 (clamp build 84be7590) settled the sink at VS2/PE0 — production's
exact levels — yet every catch battled Y:77-99 where production 8db62fa3
catches Y:11-33.  The only remaining difference was the *act* of applying:
`APPLY_ON_TRAINING_START=1` fires the 8-write DRP sequence plus the APPLY
strobe at every training start, and converts the boot csr's FFE **Auto**
into manual/C1=0 — an analog change mid-training.

Row 77 (48576c12, `AFE_APPLY_ON_START=0`) is the A/B: identical RTL and
levels, no INIT writes.  Result: 8/8, **Y:11 flat on every v-draw** — the
cleanest readings the board has produced.  Hypothesis confirmed.

**Production policy (final):**
- The boot csr is the baseline (804 mV / FFE Auto on 138B).  The sequencer
  declares INIT (`0x06`: VS2 at the MAX_VS=2 ceiling) without writing.
- DRP writes happen only when a sink's ADJUST_REQUEST differs from what is
  applied; a request back to INIT re-applies (the PHY is manual by then).
- `tb_afe_noinit.v` locks this contract: zero writes at training start,
  zero writes for a request equal to INIT, 8 writes on a real change,
  re-apply on return, no writes on subsequent training starts.
- `dp_transmitter` default `AFE_APPLY_ON_START` stays 1 for Xilinx/stub
  parity; every Gowin top must set 0 (138B dp_test does).  TODO when M5
  reaches the 60K: confirm its boot csr carries the same base AFE writes.

Open question carried forward: the apply path itself (8 writes + strobe
while training runs) may cost margin when exercised — answering it needs
a sink that asks for ≠INIT (PE1 or VS1); the Ugreen and (predicted) Anker
only exercise the no-write path.

## 12. Per-lane ADJUST_REQUEST (review item 2, applied 08-22)

The sink asks per lane (DPCD 0x206: lane 0 = [3:0], lane 1 = [7:4]) and our
two physical lanes are asymmetric (mux / channel). Until now the sequencer
consumed only lane 0's nibble and drove BOTH lanes with it, while msg 0x19
wrote that one byte to 0x103-0x106 — so lane 1 could be driven at a level it
never asked for and its TRAINING_LANE1_SET declared lane 0's level.

**What changed.** Requests, applied/target state, the DRP payload and the
declared bytes are per lane, packed `{lane1, lane0}`:
- `afe_adjust_seq`: `vs_request`/`pe_request` `[2*NUM_LANES-1:0]`,
  `train_set_byte` `[8*NUM_LANES-1:0]`, new `dbg_afe1[3:0]` (lane 1
  `{pe, vs}`); clamp, VS+PE<=3 sanitising and the MAX flags are evaluated
  per lane in a `g_lane` generate block; the DRP player selects each
  write's payload from the lane the write index belongs to.
- `dp_aux_messages` msg 0x19: `0x103 <= [7:0]` (lane 0), `0x104 <= [15:8]`
  (lane 1); 0x105/0x106 (unused lanes on a 2-lane link) mirror them so the
  message keeps its 4-byte length — AUX timing is byte-identical to msg
  0x18 and to the previous 0x19, and `expected` in `aux_channel` is
  unchanged.
- `dp_transmitter`: lane 1's nibble needs no new decode —
  `link_signal_mgmt` already captures the whole 0x206 byte into
  `channel_adjust[7:0]`, exported as `debug_adjust`, so the wiring is
  `.vs_request({debug_adjust[5:4], debug_adjust[1:0]})` and
  `.pe_request({debug_adjust[7:6], debug_adjust[3:2]})`.

**Application policy — per-lane VALUES, ALL-LANE APPLICATION** (user
decision 08-22). When any lane's request changes, every lane is written
with its own levels. Writing only the changed lane (the original proposal)
would leave one lane in manual FFE while the other stayed in the boot csr's
FFE **Auto** under `APPLY_ON_TRAINING_START = 0` — two different analog
modes on a link that depends on lane matching, which is the very asymmetry
this feature exists to remove, and a PHY state nothing has ever benched.
Row 76/77 established that the Auto->manual flip is itself perturbing, so
doing it to one lane only is the worst of both worlds.

**CORRECTION (08-22, user review).** An earlier version of this section
claimed that re-writing a lane with the values it already holds is "a
no-op for the analog state". That is WRONG. The write sequence includes
the FFE manual-enable (base+0xd8 = 0x010) and the APPLY strobe, so a lane
that was still on the boot csr's FFE **Auto** transitions Auto->Manual even
though its numeric VS/PE are unchanged — and Auto->Manual is precisely the
perturbation rows 76/77 measured. So all-lane application is NOT free: it
trades "one lane perturbed, modes split" for "both lanes perturbed, modes
matched". Which is actually better on this hardware is an OPEN BENCH
QUESTION, not something settled by argument.

Partial evidence so far: row 81's A:0000 event was exactly an all-lane
apply mid-training (both lanes Auto->Manual plus new values) and the link
trained CLEAN (Y:11 flat) — so a SINGLE mid-training all-lane apply is not
catastrophic. Row 76's damage came from re-applying at EVERY training
start, which is a different exposure. Neither observation covers a genuine
asymmetric request.

**To settle it**, an actual asymmetric case (A:0026-like) must be forced on
the bench and the two policies compared; both hubs settle at symmetric
A:0022, so the sink will not produce it on its own. That needs a request
injection/override mechanism (build-time forced lane-1 nibble, or a runtime
telnet override) — NOT YET BUILT. Until then `ALL-LANE` is the working
default because it keeps the modes matched, not because it is proven best.

A useful consequence: "any lane changed" is just a whole-vector compare of
`{pe_clamped, vs_clamped}` against `{applied_pe, applied_vs}`, so no lane
mask and no mask-walking state are needed in the DRP player.

**Telemetry.** `M:` still carries lane 0 (unchanged, so the existing test-log
history and parsers stay valid); lane 1 is a NEW `M1:<hex>` field
(`{pe, vs}`), `MSG_LEN` 91 -> 96. Without it a per-lane divergence would be
invisible on the bench.

**Sims.** `tb_afe_perlane.v` (new): A:0026 writes lane 0's block at
0x8083xx with C1=7 and lane 1's at 0x8084xx with C1=0 in the same sequence,
declares `{lane1 0x06, lane0 0x2E}`, debounces a repeat, moves only lane 1's
byte on a lane-1-only change, clamps/sanitises per lane, and keeps the
commit-on-completion discipline. The existing benches drive symmetric
requests (replicated to both lanes) and pass unchanged — i.e. with a
symmetric sink the per-lane module behaves exactly as before.

**Still open.** Whether a sink that asks A:0026 actually trains better with
lane 1 left at PE0 is a bench question. Both our hubs settle at A:0022
(symmetric), and the one A:0026 observation (rows 74/75) was a mid-training
snapshot — so like item 6, this path is architecturally correct but not yet
exercised by our hardware. `MAX_VS`/`MAX_PE` remain common to both lanes;
per-lane ceilings would only be justified by evidence of a fixed mux
asymmetry.

## 13. An all-zero ADJUST_REQUEST is "no request" (row 81, 08-22)

**Observed.** On the per-lane build (`6b39e155`, Ugreen), one v-draw ended
with `M:10 M1:0` — both lanes applied at **VS0/PE0 = 420 mV**, down from the
INIT VS2/804 mV. The sink had sent `A:0000`; the sequencer honored it
literally, the DRP sequence fired mid-training, committed on ack, and the
link trained clean (Y:11 flat) — then **ran at minimum swing** until the
next retrain.

**Why it appeared now.** The behavior is not new, but its persistence is.
Before the item-4 retention change the applied state was cleared whenever
`training_active` fell, so a 420 mV excursion evaporated at the end of every
training run and was never visible in telemetry. Retention (correctly) makes
the applied state stick, which converted a transient into a lasting
condition — a latent policy bug surfaced by a correct fix.

**Why it matters.** Many sinks report `ADJUST_REQUEST = 0x00` to mean "no
adjustment needed" once locked, not "drive me at level 0". Telemetry also
shows `A:0000` during reset storms (row 79), so honoring it literally would
leave a *struggling* link at minimum drive — the exact intermittent class
these boards fail on with marginal sinks or longer cables. It trained fine
here, which is a useful margin data point for the Ugreen but not a defense
of the policy.

**Policy — REVISED 08-22 after user review.** The first fix suppressed any
all-zero request. That is a **compatibility heuristic, not DP semantics**:
0x00 literally encodes VS0/PE0 and is not a defined "no request" sentinel.
The same event also showed the Ugreen running CLEAN at 420 mV, which
refutes the earlier claim here that a genuine VS0 request is "implausible"
— that claim is RETRACTED.

The protocol-correct discriminator is the **immediately preceding link
status**, now implemented as the `phase_done` input (§14): if the phase
being trained already reports success, the ladder advances and the
accompanying request is dropped WHATEVER its value; if the applicable
status failed, the request is honored literally, including zero. Reset-storm
values are suppressed by `phy_reinit` — because the transaction is invalid
and the PHY is re-initialising, not because the value happens to be zero.

`IGNORE_ZERO_REQUEST` is retained as a per-sink workaround but now defaults
to **0 (OFF)**. It is not shared-default policy and not the final protocol
answer; enable it for a specific board only if a specific sink is shown to
need it.

**Sims.** `tb_afe_perlane` covers both the ignored all-zero case and the
one-lane-zero case. Two scenarios in `tb_afe_adjust` had been using all-zero
requests — including the "requests outside training are ignored" gate, which
would otherwise have kept passing for the wrong reason — and now use
non-zero requests.

**Note for item 6.** This event also answered the standing question of
whether the apply path costs margin when actually exercised: it fired for
real on hardware (mid-training DRP writes + APPLY strobe on both lanes) and
the link trained with the cleanest possible signature. The `AFE_INIT_VS=1`
experiment is no longer needed.


## 14. Status-gated adjustment (`phase_done`, 08-22)

DP link training says: read LANE_STATUS, then ADJUST_REQUEST; if the phase
under training already succeeded, ADVANCE — do not apply the request. The
sequencer now takes a `phase_done` level and samples it WITH each request
(`pend_done`), so the decision uses the status that actually accompanied
that request rather than a later one.

Wiring in `dp_transmitter` (no new plumbing — `debug_locks` already carries
`{clock, equ, symbol, align}_locked`, registered in `channel_managemnt`):

```
.phase_done (tx_clock_train ? debug_locks[3]                     // CR: clock lock
                            : (tx_align_train ? (&debug_locks[2:0])  // EQ: equ+sym+align
                                              : 1'b0))
```

Consequences:
- a locked sink that emits `A:0000` is ignored because its STATUS says the
  phase is done, not because the value is zero — the row-81 420 mV drop is
  prevented by the protocol-correct mechanism;
- a sink that genuinely wants VS0/PE0 while a phase is still FAILING gets
  it, literally;
- storms are handled by `phy_reinit` (PLL unlock / PCS reset / CSR replay).

`tb_afe_perlane` covers both halves: a request accompanied by `phase_done`
is dropped and the committed levels stand; the identical request with the
phase failing applies normally. `tb_afe_zeroreq` covers the opt-in
workaround separately.

Bench note: two benches had been connecting `phase_done` to an UNDECLARED
identifier, which Verilog makes an implicit wire at `z` — poisoning the
gate and silently disabling every apply. iverilog does not error on this.
Both now declare it explicitly; worth remembering when adding ports to
shared RTL that many benches instantiate.
