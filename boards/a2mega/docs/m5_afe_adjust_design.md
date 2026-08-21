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

1. **Apply + build**: land 7a (copy from m5_proposed), apply 7b, build
   the 138B dp_test target, confirm timing (the AFE player is tiny; the
   only new cross-domain paths are the toggle handshake + quasi-static
   payloads).
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
