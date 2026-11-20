# Big merge — conflict resolutions for review (2026-10-22)

**LANDED 2026-10-22 (later the same day, ed's OK):** `main` = 9332dab5, pushed
to origin. PR #34 retargeted to main and marked merged by GitHub. The
integration branch and all merged source branches are deleted (local + origin);
`claude/timing-round2(-linear)` tips archive-tagged. Still NOT built or flashed —
section 3 is the to-do list. ed accepted the register-map remap (2a) and the
H_TRIGGER_LEAD change (2b.2) as written.

Appendix A at the bottom holds the real diffs of every resolved region against
both parents.

## 1. What was merged, in order

| # | Merge commit | Source | Conflicts |
|---|--------------|--------|-----------|
| 1 | d0a90af2 | `origin/epic-merge-prep` (PRs #3–#29, #31–#33: census, timing round 2, gs-socket S1–S5) | 3 files, PD firmware only |
| 2 | c23e3a39 | `claude/stoic-sinoussi-294a3b` (a2p25 tip, build of record 92f164d6) | 5 files — the real merge |
| 3 | 3b74e76f | `claude/a2p25-modernization-8a97cc` (+2 docs) | none |
| 4 | 338676bc | `claude/gs-socket-s6-autoarm` (PR #34) | 3 files |
| 5 | 6ea11020 | `claude/hdd-boot-wait` | none |
| 6 | 6efa1d92 | `claude/gs-socket-s7-scoping` (docs) | none |
| 7 | 9332dab5 | `claude/a2mega-aux-stack-port` (built adc6da31, never flashed) | 3 files, all the a2p25 cherry-picks vs the unions below |

350 commits, 301 files. `claude/nostalgic-jackson-c9a80d` (WIP PD DR_Swap) is
deliberately NOT included.

## 2. Decisions you should look at

### 2a. Register-map collision in the ESP32 connector — **a design decision, please confirm**

The a2p25 branch and epic-merge-prep both squatted on the same holes of the
7-bit OSPI register space (only 0x7E was genuinely free):

| Address | epic-merge-prep (a2mega, on flashed 138B builds + PR #34 fw + census tooling) | a2p25 tip |
|---------|------------------------------------------------------------------------------|-----------|
| 0x3C/0x3D | DP readback: DPCD MAX_LINK_RATE / MAX_LANE_COUNT (census second gate) | DOC REQ/frame lo/hi |
| 0x3E | DP status {wedge, video_live, link_est} | DOC MEMRD/frame lo |
| 0x3F | SERDES status | **CORE_BUILD** (build-stamp digits, OSD FIRMWARE page) |
| 0x4F | GS window DATA | DOC LATE/frame hi |
| 0x5F | GS window SEL | DOC MEMRD/frame hi |
| 0x2F | — | DOC LATE/frame lo |

**Resolution taken:** the a2mega map stays. The a2p25 additions move:

- DOC per-frame telemetry → the existing indexed window (SEL 0x5F / DATA 0x4F),
  **indices 40–45** (REQ lo/hi, MEMRD lo/hi, LATE lo/hi). PR #34 had already
  widened the window select to 6 bits and uses 32–39 for its bring-up event
  log, so 40–45 was the first free block.
- CORE_BUILD → **0x7E** (same write-index / read-digit protocol). 0x7F is the
  extended-transfer opcode.
- 0x2F is freed.

Firmware follows in the same commits: `viddbg_log` (telnet `z`, the
"DOC/FRAME REQ MEMRD LATE" line — THE a2p25 audio metric) reads the window;
`menu.c` reads the stamp at 0x7E and still degrades to "(NO VERSION REG)" on
cores without it. The merged connector reads back `{2'b0, gs_sel_r}` on SEL.

Alternatives I did not take: moving the DP/GS regs instead (more consumers,
already on hardware), or a second window (no address for it).

### 2b. framebuffer_1080p.sv — three unions

1. **Vertical tracker.** Round 2 (g)+(o) added a registered `cy==0` flag and a
   decision stage; the a2p25 (d15af6fd) registered the same inputs more simply
   and, crucially, changed `at_border/at_lead` from the `cy+1` lookahead form
   to `== cy0_r`, because both sides now consume the flags one cycle after the
   change. The union keeps round 2's deeper pipeline running on the a2p25's
   `==` flags, and drops the a2p25's duplicate `cy_zero_r`.
   **Observation:** round 2 on its own (what the 138B bench ran) kept the `+1`
   form with the later consumption, which by my trace fires every vertical
   event one raster line early (picture 1 px high, inside the border). The
   union is exact. Worth a glance at vertical position on the a2mega after
   the rebuild.
2. **Scanout trigger.** The a2p25's modulo form (`h_trigger_x_px_r`, fixes
   black SHR when h_border = 0) is kept, with `H_TRIGGER_LEAD` **11 → 12**:
   round 2's cone (i) added pipeline stage 1b (`lb_rd_addr_q`), which is why
   round 2 itself went +5 → +6. **CHECK on hardware:** IIgs SHR left edge. One
   320-mode pixel (6 raster px) of border colour means the lead is off by one
   stage.
3. **Read stage.** Round 2's CE-free address copy feeds the a2p25's RGB565 line
   buffer: `rgb565_to_666(line_buf[lb_rd_addr_q])`, `in_active_s1b_r`.

### 2c. sdp_engine.v — round 2's 3-deep pipeline, 4 lanes

Round 2 (c/c2/c3) restructured the wire-byte precompute into stage A (16-entry
group lookups into flat vectors — flat because the arrays inferred RAMREGs with
a PLL-reset net on the 138B), stage B (4:1 group select), cycle-2 constant
registers and registered cycle selects. The a2p25 generalised the OLD 2-deep
form to 4 lanes. The union is round 2's structure over 4 lanes: stage A is
4 lanes × 4 groups (128-bit vectors), stage B / constant regs loop over 4
lanes, and the round-2 cycle-2 output branch drives lanes 2/3 like the other
branches. Lanes 2/3 prune on 1/2-lane builds. Parses under verilator for
LANE_COUNT = 2 and 4. The sim self-checks still cover lanes 0/1 only.

### 2d. Smaller ones

- **PD firmware (merge 1):** `telnetd.c`, `usbc_glue.cpp/.h` → epic-merge-prep
  side; main's PR #30 cherry-picks lacked the sink-caps census and the `:`
  cmdline key. Resolved files are byte-identical to epic-merge-prep's.
- **telnetd.c help line (merge 2):** a2p25's board-name/`z`/`i`/`n` variant
  plus epic-merge-prep's `:=cmdline`.
- **a2mega top.sv:** both port sets on the connector (DP readback + GS window
  from epic-merge-prep, DOC telemetry from the a2p25); both
  `debug_sink_caps` (census) and the aux-stack port's explicit empty
  `debug_obs/auxif/underrun/fifo` connections.
- **a2p25 top.sv:** new a2mega-only connector inputs tied off: DP readback and
  socket window inputs 0, `a2_alive_i(~sleep_w)` like the a2mega,
  `gs_res_n_i(1'b1)`. **Follow-up:** feed `dbg_dp_caps_i` from the
  transmitter's `debug_sink_caps` so the a2p25 gets the census second gate.
- **disk.c (merge 4):** reset release = `(media mounted && gs_socket_ready())
  || 10 s backstop` (a2p25's 10 s for the USB host role decision kept, PR #34's
  socket-arm gate added), released via `gs_socket_a2_release()` then
  `a2_reset_hold_release()` (a2p25 /RES pin). On the a2p25 the socket module
  sees status 0 and settles after 300 ms. `Makefile`: both source/header
  lists.

## 3. Still open after this

- **Build + timing** for a2mega 60K, a2mega 138B (gs socket), a2p25 — the
  no-skating rule applies; a2mega has not been attached since Oct 11.
- Hardware: SHR left edge (2b.2), vertical position (2b.1), telnet `z` DOC
  line on the a2p25, OSD FIRMWARE page CORE date, census `d` on the a2mega,
  PR #34 auto-arm on the 138B.
- `rescue/a2p25-console-hide-no-media-policy` edits the same disk.c release
  path; rebase it onto the integration branch (1 commit) and merge.
- `claude/nostalgic-jackson-c9a80d` WIP; `claude/apple-clock-sync-ed34ff`;
  `pr/fb-sdram-correctness`; `origin/claude/twgs-a2-mega-plan-iferin`.
- Pushing `integration/2026-10-22` (backup) and, after your OK, fast-forwarding
  main; then deleting the merged source branches and closing PR #34.

---

## Appendix A — resolved regions, as diffs against each parent


### A1. sdp_engine.v vs the a2p25 tip (what round 2 adds to the 4-lane engine)

```diff
diff --git a/hdl/displayport/audio/sdp_engine.v b/hdl/displayport/audio/sdp_engine.v
index b53619ae..52f1b184 100644
--- a/hdl/displayport/audio/sdp_engine.v
+++ b/hdl/displayport/audio/sdp_engine.v
@@ -357,7 +357,30 @@ module sdp_engine #(
     reg        ts_sent_once, stream_sent_once;
     reg        pkt_pending;
     reg        pkt_armed;                      // parity registers settled
-    reg [2:0]  armed_sh;                       // arming delay chain
+    reg [3:0]  armed_sh;                       // arming delay chain
+    // Timing campaign round 2, cone (c2): the packet load used to happen
+    // in the decision cycle, so the 256 db flops shared ONE enable net
+    // (!pkt_pending && !inserting && due) with fanout 256 — the clk_sym
+    // floor at +0.04 ns (dd 7.06, route-dominated) in a 60K roll. The
+    // load is now staged: the decision cycle marks pkt_pending and arms
+    // replicated one-hot load strobes (one copy per 64-bit db quarter,
+    // syn_keep'd so they stay separate), and the next cycle performs the
+    // load — flop -> data mux -> db, fanout 64 per strobe. Everything the
+    // load touched (hbv, ts_due/info_due clears, buffer_take, iec_cnt)
+    // moves with it; the arming chain is one stage longer so pkt_armed
+    // still follows the settled parity by the same margin.
+    (* syn_keep = 1, syn_preserve = 1 *) reg [3:0] ld_ts_r;
+    (* syn_keep = 1, syn_preserve = 1 *) reg [3:0] ld_st_r;
+    (* syn_keep = 1, syn_preserve = 1 *) reg [3:0] ld_in_r;
+    // Cone (c3): the insertion output mux selected on ins_cyc compares
+    // (== 1, == 2, == last) fanning into all 73 out_data bits ("ins_cyc ->
+    // out_data" +0.15 ns at 135 MHz). The three selects are registered
+    // one cycle ahead from ins_cyc's own next-value rule (exact; sim
+    // mirror check below).
+    wire [5:0] ins_cyc_nxt = reset ? 6'd0 :
+                             !inserting ? ((sdp_gap && pkt_pending && pkt_armed) ? 6'd1 : ins_cyc) :
+                             ((ins_cyc == INS_CYCLES[5:0]-1) ? 6'd0 : ins_cyc + 6'd1);
+    reg cyc1_r = 1'b0, cyc2_r = 1'b0, cycl_r = 1'b0;
     reg [5:0]  ins_cyc;                        // 0 = idle
     reg        inserting;
 
@@ -376,11 +399,9 @@ module sdp_engine #(
     // cone (which was on the clk_sym critical list in the full a2mega
     // core). wb0/wblast cover the constant-index first/last cycles.
     // ------------------------------------------------------------------
-    wire [5:0] wb_nxt = inserting ? ins_cyc + 6'd1 : 6'd1;
-    // b-index = 2*wb_nxt - 1 = {ins_cyc, 1} exactly (idle: ins_cyc=0 -> 1),
-    // avoiding a subtractor in series with the byte mux — the borrow chain
-    // was the design's final failing path at 135 MHz.
-    wire [5:0] wb_nxt_b = {ins_cyc[4:0], 1'b1};
+    // b-index = 2*n - 1 = {n-1, 1} exactly, avoiding a subtractor in
+    // series with the byte mux — the borrow chain was the design's final
+    // failing path at 135 MHz (retained below in wb_idx_b_r).
     // 08-30 structural timing: the index itself is REGISTERED before the
     // deep wire_byte mux, so the inserting/ins_cyc -> 256-bit-mux cone
     // (the residual clk_sym critical family in the full cores) starts
@@ -389,11 +410,45 @@ module sdp_engine #(
     // cycle k is registered, at k-1 the byte lands in wb_*_r, at k it is
     // consumed — same consumption timing as before. Cycles 0 and 1 use
     // the constant-index regs (wb0_r, wb1_r) exactly like wb0/wblast.
+    // Timing campaign round 2, cone (c): the registered index still fed
+    // the whole 36-way wire_byte mux in one cycle (wb_idx_b_r -> wb_b_r
+    // +0.21 ns at 135 MHz in the full core, the clk_sym floor once the
+    // packer cones were gone). The lookahead is now THREE deep: at
+    // insertion cycle k-3 the index for cycle k is registered; at k-2 the
+    // four 16-entry index groups (k[5:4] selects) are looked up into
+    // registers; at k-1 the group is picked (4:1) into wb_*_r; at k it is
+    // consumed. Same consumption timing as before; cycle 2 joins cycles 0
+    // and 1 on constant-index registers (wb2a/wb2b), and the sim
+    // self-check below covers it.
     wire [5:0] wb_nxt2   = inserting ? ins_cyc + 6'd2 : 6'd2;
+    wire [5:0] wb_nxt3   = inserting ? ins_cyc + 6'd3 : 6'd3;
     reg  [5:0] wb_idx_a_r = 6'd0, wb_idx_b_r = 6'd0;
     always @(posedge clk) begin
-        wb_idx_a_r <= {wb_nxt2[4:0], 1'b0};
-        wb_idx_b_r <= {wb_nxt[4:0],  1'b1};   // 2*(k+2)-1 = {k+1, 1}
+        wb_idx_a_r <= {wb_nxt3[4:0], 1'b0};   // 2*(k+3)
+        wb_idx_b_r <= {wb_nxt2[4:0], 1'b1};   // 2*(k+3)-1 = {k+2, 1}
+    end
+    // stage A: group lookups, flattened [lane*4 + group]
+    // Flat vectors, not arrays (138B durability): indexed with a variable,
+    // the 8x8 arrays were inferred as distributed RAM with a registered
+    // output ("RAMREG") whose RESET pin the tool tied to the pixel PLL's
+    // PLL_INIT enable net (fanout 163, 4.6 ns of route on the 138B:
+    // "u_pll_init/enable_r -> wb_gb_r RAMREG RESET" +0.10 ns). As flat
+    // vectors with an explicit part-select they are plain flops with no
+    // reset pin. Same values, same timing.
+    // 4 lanes x 4 groups (lanes 2/3 prune on 1- and 2-lane builds: their
+    // only consumers are under LANE_COUNT == 4).
+    reg [127:0] wb_ga_r = 128'd0;   // byte g of lane l at [8*(4*l+g) +: 8]
+    reg [127:0] wb_gb_r = 128'd0;
+    reg [1:0] wb_sa_r = 2'd0, wb_sb_r = 2'd0;
+    integer wgi, wgl;
+    always @(posedge clk) begin
+        for (wgl = 0; wgl < 4; wgl = wgl + 1)
+            for (wgi = 0; wgi < 4; wgi = wgi + 1) begin
+                wb_ga_r[8*(4*wgl+wgi) +: 8] <= wire_byte(wgl[1:0], {wgi[1:0], wb_idx_a_r[3:0]}, hbv, pbhv, pbdv, db);
+                wb_gb_r[8*(4*wgl+wgi) +: 8] <= wire_byte(wgl[1:0], {wgi[1:0], wb_idx_b_r[3:0]}, hbv, pbhv, pbdv, db);
+            end
+        wb_sa_r <= wb_idx_a_r[5:4];
+        wb_sb_r <= wb_idx_b_r[5:4];
     end
     // (lanes 2/3 are only consumed when LANE_COUNT == 4; otherwise pruned)
     reg [7:0] wb_a_r [0:3];      // byte index 2*ins_cyc   (slot 1)
@@ -401,12 +456,18 @@ module sdp_engine #(
     reg [7:0] wb0_r  [0:3];      // byte index 0 (start cycle)
     reg [7:0] wb1a_r [0:3];      // byte index 2 (cycle-1 slot 1)
     reg [7:0] wb1b_r [0:3];      // byte index 1 (cycle-1 slot 0)
+    reg [7:0] wb2a_r [0:3];      // byte index 4 (cycle-2 slot 1)
+    reg [7:0] wb2b_r [0:3];      // byte index 3 (cycle-2 slot 0)
     reg [7:0] wblast_r [0:3];    // byte index WB_PER_LANE-1 (last cycle)
     integer wl;
     always @(posedge clk) begin
         for (wl = 0; wl < 4; wl = wl + 1) begin
-            wb_a_r[wl]   <= wire_byte(wl[1:0], wb_idx_a_r,          hbv, pbhv, pbdv, db);
-            wb_b_r[wl]   <= wire_byte(wl[1:0], wb_idx_b_r,          hbv, pbhv, pbdv, db);
+            // stage B: group select (4:1 on the registered k[5:4])
+            wb_a_r[wl]   <= wb_ga_r[8*{wl[1:0], wb_sa_r} +: 8];
+            wb_b_r[wl]   <= wb_gb_r[8*{wl[1:0], wb_sb_r} +: 8];
+            // constant-index cycles 0, 1, 2 and last
+            wb2a_r[wl]   <= wire_byte(wl[1:0], 6'd4,                hbv, pbhv, pbdv, db);
+            wb2b_r[wl]   <= wire_byte(wl[1:0], 6'd3,                hbv, pbhv, pbdv, db);
             wb1a_r[wl]   <= wire_byte(wl[1:0], 6'd2,                hbv, pbhv, pbdv, db);
             wb1b_r[wl]   <= wire_byte(wl[1:0], 6'd1,                hbv, pbhv, pbdv, db);
             wb0_r[wl]    <= wire_byte(wl[1:0], 6'd0,                hbv, pbhv, pbdv, db);
@@ -422,7 +483,13 @@ module sdp_engine #(
             stream_sent_once <= 1'b0;
             pkt_pending      <= 1'b0;
             pkt_armed        <= 1'b0;
-            armed_sh         <= 3'b000;
+            armed_sh         <= 4'b0000;
+            ld_ts_r          <= 4'b0000;
+            ld_st_r          <= 4'b0000;
+            ld_in_r          <= 4'b0000;
+            cyc1_r           <= 1'b0;
+            cyc2_r           <= 1'b0;
+            cycl_r           <= 1'b0;
             inserting        <= 1'b0;
             ins_cyc          <= 6'd0;
             iec_cnt          <= 8'd0;
@@ -431,6 +498,9 @@ module sdp_engine #(
             out_data         <= 73'b0;
         end else begin
             buffer_take <= 1'b0;
+            cyc1_r <= (ins_cyc_nxt == 6'd1);
+            cyc2_r <= (ins_cyc_nxt == 6'd2);
+            cycl_r <= (ins_cyc_nxt == INS_CYCLES[5:0]-1);
             // armed = pending for >=4 CONSECUTIVE cycles (covers the split
             // parity pipeline + wb precompute). Must clear the instant
             // pending drops: a plain shift register retains stale armed
@@ -438,8 +508,10 @@ module sdp_engine #(
             // in one gap the next insertion would start on the PREVIOUS
             // packet's precomputed bytes (caught by the wire-byte shadow
             // assertion in sim - TimeStamp/Stream pair, frame 1).
-            armed_sh    <= pkt_pending ? {armed_sh[1:0], 1'b1} : 3'b000;
-            pkt_armed   <= pkt_pending ? armed_sh[2] : 1'b0;
+            // (one stage longer since the staged load, cone (c2): the
+            // payload lands one cycle after pkt_pending rises)
+            armed_sh    <= pkt_pending ? {armed_sh[2:0], 1'b1} : 4'b0000;
+            pkt_armed   <= pkt_pending ? armed_sh[3] : 1'b0;
 
             if (frame_pulse) begin
                 ts_due   <= 1'b1;
@@ -451,35 +523,59 @@ module sdp_engine #(
             // ----------------------------------------------------------
             // Load the next packet when idle
             // ----------------------------------------------------------
+            // Stage 1 (decision cycle): mark pending, arm the replicated
+            // per-quarter load strobes. Same priority as before.
+            ld_ts_r <= 4'b0000;
+            ld_st_r <= 4'b0000;
+            ld_in_r <= 4'b0000;
             if (!pkt_pending && !inserting) begin
                 if (ts_due) begin
-                    hbv <= {8'h44, 8'h17, 8'h01, 8'h00};
-                    for (si = 0; si < 4; si = si + 1) begin
-                        db[32*si     +: 32] <= {8'h00, maud[7:0], maud[15:8], maud[23:16]};
-                        db[128+32*si +: 32] <= {8'h00, NAUD[7:0], NAUD[15:8], NAUD[23:16]};
-                    end
                     pkt_pending <= 1'b1;
-                    ts_due      <= 1'b0;
+                    ld_ts_r     <= 4'b1111;
                 end else if (buffer_ready) begin
-                    hbv <= {8'h01, 8'h00, 8'h02, 8'h00};
-                    for (si = 0; si < 4; si = si + 1) begin
-                        sl  = buffer[si*2*AUDIO_BIT_WIDTH +: AUDIO_BIT_WIDTH];
-                        sr  = buffer[si*2*AUDIO_BIT_WIDTH + AUDIO_BIT_WIDTH +: AUDIO_BIT_WIDTH];
-                        csb = chan_status[iec_cnt + si[7:0]];
-                        pr_l = ((iec_cnt + si[7:0]) == 8'd0) ? 2'b00 : 2'b01;
-                        // DB(8i..8i+3) = ch1 subframe, DB(8i+4..8i+7) = ch2
-                        db[64*si    +: 32] <= {sf_b3(sl, pr_l,  csb), sl[15:8], sl[7:0], 8'h00};
-                        db[64*si+32 +: 32] <= {sf_b3(sr, 2'b10, csb), sr[15:8], sr[7:0], 8'h00};
-                    end
-                    iec_cnt     <= (iec_cnt >= 8'd188) ? 8'd0 : iec_cnt + 8'd4;
-                    buffer_take <= 1'b1;
                     pkt_pending <= 1'b1;
+                    ld_st_r     <= 4'b1111;
                 end else if (info_due) begin
-                    hbv <= {8'h44, 8'h1B, 8'h84, 8'h00};
-                    db          <= 256'h0;
-                    db[7:0]     <= 8'h01;          // CT=0 (refer to header), CC=1 (2ch)
                     pkt_pending <= 1'b1;
-                    info_due    <= 1'b0;
+                    ld_in_r     <= 4'b1111;
+                end
+            end
+            // Stage 2 (next cycle): header + side effects from copy 0 ...
+            if (ld_ts_r[0]) begin
+                hbv    <= {8'h44, 8'h17, 8'h01, 8'h00};
+                ts_due <= 1'b0;
+            end
+            if (ld_st_r[0]) begin
+                hbv         <= {8'h01, 8'h00, 8'h02, 8'h00};
+                iec_cnt     <= (iec_cnt >= 8'd188) ? 8'd0 : iec_cnt + 8'd4;
+                buffer_take <= 1'b1;   // buffer still holds the samples read below
+            end
+            if (ld_in_r[0]) begin
+                hbv      <= {8'h44, 8'h1B, 8'h84, 8'h00};
+                info_due <= 1'b0;
+            end
+            // ... and payload quarter si (64 bits) from strobe copy si.
+            // TimeStamp: quarters 0/1 = Maud x2, quarters 2/3 = Naud x2
+            // (was db[32*i] = Maud for i<4, db[128+32*i] = Naud).
+            // Stream: quarter i = sample pair i (was db[64*i]).
+            // Infoframe: all zero except DB0 = 0x01.
+            for (si = 0; si < 4; si = si + 1) begin
+                if (ld_ts_r[si]) begin
+                    db[64*si    +: 32] <= (si < 2) ? {8'h00, maud[7:0], maud[15:8], maud[23:16]}
+                                                   : {8'h00, NAUD[7:0], NAUD[15:8], NAUD[23:16]};
+                    db[64*si+32 +: 32] <= (si < 2) ? {8'h00, maud[7:0], maud[15:8], maud[23:16]}
+                                                   : {8'h00, NAUD[7:0], NAUD[15:8], NAUD[23:16]};
+                end else if (ld_st_r[si]) begin
+                    sl  = buffer[si*2*AUDIO_BIT_WIDTH +: AUDIO_BIT_WIDTH];
+                    sr  = buffer[si*2*AUDIO_BIT_WIDTH + AUDIO_BIT_WIDTH +: AUDIO_BIT_WIDTH];
+                    csb = chan_status[iec_cnt + si[7:0]];
+                    pr_l = ((iec_cnt + si[7:0]) == 8'd0) ? 2'b00 : 2'b01;
+                    // DB(8i..8i+3) = ch1 subframe, DB(8i+4..8i+7) = ch2
+                    db[64*si    +: 32] <= {sf_b3(sl, pr_l,  csb), sl[15:8], sl[7:0], 8'h00};
+                    db[64*si+32 +: 32] <= {sf_b3(sr, 2'b10, csb), sr[15:8], sr[7:0], 8'h00};
+                end else if (ld_in_r[si]) begin
+                    db[64*si +: 64] <= (si == 0) ? 64'h0000_0000_0000_0001   // CT=0 (refer to header), CC=1 (2ch)
+                                                 : 64'h0;
                 end
             end
 
@@ -502,7 +598,7 @@ module sdp_engine #(
                     end
                 end
             end else begin
-                if (ins_cyc == INS_CYCLES[5:0]-1) begin
+                if (cycl_r) begin                      // ins_cyc == INS_CYCLES-1
                     // last cycle: slot0 = final wire byte, slot1 = SE
                     out_data[17:0] <= {SE, 1'b0, wblast_r[0]};
                     if (LANE_COUNT >= 2)
@@ -516,10 +612,10 @@ module sdp_engine #(
                     pkt_pending <= 1'b0;
                     if (hbv[15:8] == 8'h01) ts_sent_once     <= 1'b1;
                     if (hbv[15:8] == 8'h02) stream_sent_once <= 1'b1;
-                end else if (ins_cyc == 6'd1) begin
-                    // cycle 1: constant-index regs (the 2-deep index
-                    // pipeline cannot serve it — its rolling regs first
-                    // become valid at cycle 2, seeded by the idle
+                end else if (cyc1_r) begin            // ins_cyc == 1
+                    // cycles 1 and 2: constant-index regs (the 3-deep
+                    // index pipeline cannot serve them — its rolling regs
+                    // first become valid at cycle 3, seeded by the idle
                     // default index)
                     out_data[17:0] <= {1'b0, wb1a_r[0], 1'b0, wb1b_r[0]};
                     if (LANE_COUNT >= 2)
@@ -529,10 +625,19 @@ module sdp_engine #(
                         out_data[71:54] <= {1'b0, wb1a_r[3], 1'b0, wb1b_r[3]};
                     end
                     ins_cyc <= ins_cyc + 1'b1;
+                end else if (cyc2_r) begin            // ins_cyc == 2
+                    out_data[17:0] <= {1'b0, wb2a_r[0], 1'b0, wb2b_r[0]};
+                    if (LANE_COUNT >= 2)
+                        out_data[35:18] <= {1'b0, wb2a_r[1], 1'b0, wb2b_r[1]};
+                    if (LANE_COUNT == 4) begin
+                        out_data[53:36] <= {1'b0, wb2a_r[2], 1'b0, wb2b_r[2]};
+                        out_data[71:54] <= {1'b0, wb2a_r[3], 1'b0, wb2b_r[3]};
+                    end
+                    ins_cyc <= ins_cyc + 1'b1;
                 end else begin
                     // slot0 = wire byte 2c-1, slot1 = wire byte 2c
-                    // (2-stage pipeline: index registered at c-2,
-                    // byte registered at c-1, consumed at c)
+                    // (3-stage pipeline: index registered at c-3, group
+                    // bytes at c-2, byte registered at c-1, consumed at c)
                     out_data[17:0] <= {1'b0, wb_a_r[0], 1'b0, wb_b_r[0]};
                     if (LANE_COUNT >= 2)
                         out_data[35:18] <= {1'b0, wb_a_r[1], 1'b0, wb_b_r[1]};
@@ -545,6 +650,12 @@ module sdp_engine #(
             end
 
             // synthesis translate_off
+            // cycle-select lookahead flags must mirror the counter (cone c3)
+            if (cyc1_r !== (ins_cyc == 6'd1) || cyc2_r !== (ins_cyc == 6'd2) ||
+                cycl_r !== (ins_cyc == INS_CYCLES[5:0]-1)) begin
+                $display("FATAL: sdp_engine cycle-select flags diverged at ins_cyc=%0d", ins_cyc);
+                $fatal(1);
+            end
             // precomputed bytes must equal the direct lookups they replace
             if (inserting && ins_cyc == 6'd1) begin
                 if ({wb1a_r[0], wb1b_r[0], wb1a_r[1], wb1b_r[1]} !==
@@ -556,7 +667,17 @@ module sdp_engine #(
                     $fatal(1);
                 end
             end
-            if (inserting && ins_cyc != INS_CYCLES[5:0]-1 && ins_cyc != 6'd1) begin
+            if (inserting && ins_cyc == 6'd2) begin
+                if ({wb2a_r[0], wb2b_r[0], wb2a_r[1], wb2b_r[1]} !==
+                    {wire_byte(2'd0, 6'd4, hbv, pbhv, pbdv, db),
+                     wire_byte(2'd0, 6'd3, hbv, pbhv, pbdv, db),
+                     wire_byte(2'd1, 6'd4, hbv, pbhv, pbdv, db),
+                     wire_byte(2'd1, 6'd3, hbv, pbhv, pbdv, db)}) begin
+                    $display("FATAL: sdp_engine cycle-2 precompute diverged");
+                    $fatal(1);
+                end
+            end
+            if (inserting && ins_cyc != INS_CYCLES[5:0]-1 && ins_cyc != 6'd1 && ins_cyc != 6'd2) begin
                 if ({wb_a_r[0], wb_b_r[0], wb_a_r[1], wb_b_r[1]} !==
                     {wire_byte(2'd0, {ins_cyc[4:0],1'b0},      hbv, pbhv, pbdv, db),
                      wire_byte(2'd0, {ins_cyc[4:0],1'b0}-6'd1, hbv, pbhv, pbdv, db),
```

### A2. sdp_engine.v vs epic-merge-prep (what the a2p25's 4-lane mapping adds to round 2)

```diff
diff --git a/hdl/displayport/audio/sdp_engine.v b/hdl/displayport/audio/sdp_engine.v
index 1c620b6a..52f1b184 100644
--- a/hdl/displayport/audio/sdp_engine.v
+++ b/hdl/displayport/audio/sdp_engine.v
@@ -15,12 +15,15 @@
 //  - GF(2^4) primitive polynomial x^4+x+1, generator (x-a^0)(x-a^1);
 //    parity computed by a 2-register nibble LFSR, fed low nibble first;
 //    parity byte = {x0, x1}
-//  - on the wire, paired lanes swap high nibbles byte-for-byte (2 lanes);
-//    on 1 lane consecutive codewords interleave per Figures 2-27/2-29
+//  - on the wire, paired lanes swap high nibbles byte-for-byte (lanes
+//    0<->1 and 2<->3 on 2- and 4-lane links, Figures 2-26/2-28); on 1
+//    lane consecutive codewords interleave per Figures 2-27/2-29
 //
 // All three packet types carry exactly 32 payload bytes (InfoFrame is 28
-// zero-padded to 32), so the wire length is constant: 24 bytes per lane
-// on 2 lanes, 48 bytes on 1 lane, plus SS/SE.
+// zero-padded to 32), so the wire length is constant: 12 bytes per lane
+// on 4 lanes, 24 on 2 lanes, 48 on 1 lane, plus SS/SE. 4-lane mapping
+// (DP 1.1a section 2.2.5.1 figure, page 72): lane n carries
+// HBn PBn DB(4n..4n+3) PB(4+n) DB(16+4n..16+4n+3) PB(8+n).
 //
 // Scheduling per gap: Audio_TimeStamp (once per frame) > Audio_Stream
 // (4-sample buffer ready) > Audio InfoFrame (once per frame). VB-ID
@@ -34,7 +37,7 @@
 `timescale 1ns / 1ps
 
 module sdp_engine #(
-    parameter LANE_COUNT      = 2,      // 1 or 2
+    parameter LANE_COUNT      = 2,      // 1, 2 or 4
     parameter AUDIO_BIT_WIDTH = 16,     // 16 only for now (subframe packing)
     parameter AUDIO_RATE      = 48000
 )(
@@ -61,7 +64,8 @@ module sdp_engine #(
     localparam [23:0] NAUD = 24'd32768;
 
     // wire bytes per lane between SS and SE
-    localparam integer WB_PER_LANE = (LANE_COUNT == 1) ? 48 : 24;
+    localparam integer WB_PER_LANE = (LANE_COUNT == 1) ? 48 :
+                                     (LANE_COUNT == 4) ? 12 : 24;
     localparam integer INS_CYCLES  = (WB_PER_LANE + 2) / 2;   // incl. SS/SE
 
     // ------------------------------------------------------------------
@@ -222,13 +226,45 @@ module sdp_engine #(
         end
     endfunction
 
+    //   4-lane: lane n = HBn PBhn [DBg(n) P(n)] [DBg(n+4) P(n+4)]
+    //           wire: lanes 0<->1 and 2<->3 swap high nibbles
+    function [7:0] pre_byte4(input [1:0] lane, input [5:0] k,
+                             input [31:0] fhb, input [31:0] fpbh,
+                             input [63:0] fpbd, input [255:0] fdb);
+        begin
+            // flat 12-way mux with constant slices (lane is a constant
+            // after the precompute loop unrolls); a k-dependent part-select
+            // here synthesized as a barrel shifter over the 256-bit payload
+            // (-6 ns at clk_sym)
+            case (k)
+                6'd0:  pre_byte4 = fhb[8*lane +: 8];
+                6'd1:  pre_byte4 = fpbh[8*lane +: 8];
+                6'd2:  pre_byte4 = fdb[32*lane      +: 8];
+                6'd3:  pre_byte4 = fdb[32*lane + 8  +: 8];
+                6'd4:  pre_byte4 = fdb[32*lane + 16 +: 8];
+                6'd5:  pre_byte4 = fdb[32*lane + 24 +: 8];
+                6'd6:  pre_byte4 = fpbd[8*lane +: 8];
+                6'd7:  pre_byte4 = fdb[128 + 32*lane      +: 8];
+                6'd8:  pre_byte4 = fdb[128 + 32*lane + 8  +: 8];
+                6'd9:  pre_byte4 = fdb[128 + 32*lane + 16 +: 8];
+                6'd10: pre_byte4 = fdb[128 + 32*lane + 24 +: 8];
+                6'd11: pre_byte4 = fpbd[32 + 8*lane +: 8];
+                default: pre_byte4 = 8'h00;
+            endcase
+        end
+    endfunction
+
     // wire byte for lane l, position k (0..WB_PER_LANE-1)
     function [7:0] wire_byte(input [1:0] l, input [5:0] k,
                              input [31:0] fhb, input [31:0] fpbh,
                              input [63:0] fpbd, input [255:0] fdb);
         reg [7:0] a, b;
         begin
-            if (LANE_COUNT == 2) begin
+            if (LANE_COUNT == 4) begin
+                a = pre_byte4(l, k, fhb, fpbh, fpbd, fdb);
+                b = pre_byte4({l[1], ~l[0]}, k, fhb, fpbh, fpbd, fdb);
+                wire_byte = {b[7:4], a[3:0]};
+            end else if (LANE_COUNT == 2) begin
                 a = pre_byte(l[0], k, fhb, fpbh, fpbd, fdb);
                 b = pre_byte(~l[0], k, fhb, fpbh, fpbd, fdb);
                 wire_byte = {b[7:4], a[3:0]};
@@ -399,46 +435,44 @@ module sdp_engine #(
     // "u_pll_init/enable_r -> wb_gb_r RAMREG RESET" +0.10 ns). As flat
     // vectors with an explicit part-select they are plain flops with no
     // reset pin. Same values, same timing.
-    reg [63:0] wb_ga_r = 64'd0;   // byte g of lane l at [8*(4*l+g) +: 8]
-    reg [63:0] wb_gb_r = 64'd0;
+    // 4 lanes x 4 groups (lanes 2/3 prune on 1- and 2-lane builds: their
+    // only consumers are under LANE_COUNT == 4).
+    reg [127:0] wb_ga_r = 128'd0;   // byte g of lane l at [8*(4*l+g) +: 8]
+    reg [127:0] wb_gb_r = 128'd0;
     reg [1:0] wb_sa_r = 2'd0, wb_sb_r = 2'd0;
-    integer wgi;
+    integer wgi, wgl;
     always @(posedge clk) begin
-        for (wgi = 0; wgi < 4; wgi = wgi + 1) begin
-            wb_ga_r[8*wgi      +: 8] <= wire_byte(2'd0, {wgi[1:0], wb_idx_a_r[3:0]}, hbv, pbhv, pbdv, db);
-            wb_ga_r[8*(4+wgi)  +: 8] <= wire_byte(2'd1, {wgi[1:0], wb_idx_a_r[3:0]}, hbv, pbhv, pbdv, db);
-            wb_gb_r[8*wgi      +: 8] <= wire_byte(2'd0, {wgi[1:0], wb_idx_b_r[3:0]}, hbv, pbhv, pbdv, db);
-            wb_gb_r[8*(4+wgi)  +: 8] <= wire_byte(2'd1, {wgi[1:0], wb_idx_b_r[3:0]}, hbv, pbhv, pbdv, db);
-        end
+        for (wgl = 0; wgl < 4; wgl = wgl + 1)
+            for (wgi = 0; wgi < 4; wgi = wgi + 1) begin
+                wb_ga_r[8*(4*wgl+wgi) +: 8] <= wire_byte(wgl[1:0], {wgi[1:0], wb_idx_a_r[3:0]}, hbv, pbhv, pbdv, db);
+                wb_gb_r[8*(4*wgl+wgi) +: 8] <= wire_byte(wgl[1:0], {wgi[1:0], wb_idx_b_r[3:0]}, hbv, pbhv, pbdv, db);
+            end
         wb_sa_r <= wb_idx_a_r[5:4];
         wb_sb_r <= wb_idx_b_r[5:4];
     end
-    reg [7:0] wb_a_r [0:1];      // byte index 2*ins_cyc   (slot 1)
-    reg [7:0] wb_b_r [0:1];      // byte index 2*ins_cyc-1 (slot 0)
-    reg [7:0] wb0_r  [0:1];      // byte index 0 (start cycle)
-    reg [7:0] wb1a_r [0:1];      // byte index 2 (cycle-1 slot 1)
-    reg [7:0] wb1b_r [0:1];      // byte index 1 (cycle-1 slot 0)
-    reg [7:0] wb2a_r [0:1];      // byte index 4 (cycle-2 slot 1)
-    reg [7:0] wb2b_r [0:1];      // byte index 3 (cycle-2 slot 0)
-    reg [7:0] wblast_r [0:1];    // byte index WB_PER_LANE-1 (last cycle)
+    // (lanes 2/3 are only consumed when LANE_COUNT == 4; otherwise pruned)
+    reg [7:0] wb_a_r [0:3];      // byte index 2*ins_cyc   (slot 1)
+    reg [7:0] wb_b_r [0:3];      // byte index 2*ins_cyc-1 (slot 0)
+    reg [7:0] wb0_r  [0:3];      // byte index 0 (start cycle)
+    reg [7:0] wb1a_r [0:3];      // byte index 2 (cycle-1 slot 1)
+    reg [7:0] wb1b_r [0:3];      // byte index 1 (cycle-1 slot 0)
+    reg [7:0] wb2a_r [0:3];      // byte index 4 (cycle-2 slot 1)
+    reg [7:0] wb2b_r [0:3];      // byte index 3 (cycle-2 slot 0)
+    reg [7:0] wblast_r [0:3];    // byte index WB_PER_LANE-1 (last cycle)
+    integer wl;
     always @(posedge clk) begin
-        // stage B: group select
-        wb_a_r[0]   <= wb_ga_r[8*{1'b0, wb_sa_r} +: 8];
-        wb_a_r[1]   <= wb_ga_r[8*{1'b1, wb_sa_r} +: 8];
-        wb_b_r[0]   <= wb_gb_r[8*{1'b0, wb_sb_r} +: 8];
-        wb_b_r[1]   <= wb_gb_r[8*{1'b1, wb_sb_r} +: 8];
-        wb2a_r[0]   <= wire_byte(2'd0, 6'd4,                hbv, pbhv, pbdv, db);
-        wb2a_r[1]   <= wire_byte(2'd1, 6'd4,                hbv, pbhv, pbdv, db);
-        wb2b_r[0]   <= wire_byte(2'd0, 6'd3,                hbv, pbhv, pbdv, db);
-        wb2b_r[1]   <= wire_byte(2'd1, 6'd3,                hbv, pbhv, pbdv, db);
-        wb1a_r[0]   <= wire_byte(2'd0, 6'd2,                hbv, pbhv, pbdv, db);
-        wb1a_r[1]   <= wire_byte(2'd1, 6'd2,                hbv, pbhv, pbdv, db);
-        wb1b_r[0]   <= wire_byte(2'd0, 6'd1,                hbv, pbhv, pbdv, db);
-        wb1b_r[1]   <= wire_byte(2'd1, 6'd1,                hbv, pbhv, pbdv, db);
-        wb0_r[0]    <= wire_byte(2'd0, 6'd0,                     hbv, pbhv, pbdv, db);
-        wb0_r[1]    <= wire_byte(2'd1, 6'd0,                     hbv, pbhv, pbdv, db);
-        wblast_r[0] <= wire_byte(2'd0, WB_PER_LANE[5:0]-6'd1,    hbv, pbhv, pbdv, db);
-        wblast_r[1] <= wire_byte(2'd1, WB_PER_LANE[5:0]-6'd1,    hbv, pbhv, pbdv, db);
+        for (wl = 0; wl < 4; wl = wl + 1) begin
+            // stage B: group select (4:1 on the registered k[5:4])
+            wb_a_r[wl]   <= wb_ga_r[8*{wl[1:0], wb_sa_r} +: 8];
+            wb_b_r[wl]   <= wb_gb_r[8*{wl[1:0], wb_sb_r} +: 8];
+            // constant-index cycles 0, 1, 2 and last
+            wb2a_r[wl]   <= wire_byte(wl[1:0], 6'd4,                hbv, pbhv, pbdv, db);
+            wb2b_r[wl]   <= wire_byte(wl[1:0], 6'd3,                hbv, pbhv, pbdv, db);
+            wb1a_r[wl]   <= wire_byte(wl[1:0], 6'd2,                hbv, pbhv, pbdv, db);
+            wb1b_r[wl]   <= wire_byte(wl[1:0], 6'd1,                hbv, pbhv, pbdv, db);
+            wb0_r[wl]    <= wire_byte(wl[1:0], 6'd0,                hbv, pbhv, pbdv, db);
+            wblast_r[wl] <= wire_byte(wl[1:0], WB_PER_LANE[5:0]-6'd1, hbv, pbhv, pbdv, db);
+        end
     end
 
     always @(posedge clk) begin
@@ -556,15 +590,23 @@ module sdp_engine #(
                     ins_cyc   <= 6'd1;
                     // slot0 = SS, slot1 = wire byte 0 (precomputed)
                     out_data[17:0] <= {1'b0, wb0_r[0], SS};
-                    if (LANE_COUNT == 2)
+                    if (LANE_COUNT >= 2)
                         out_data[35:18] <= {1'b0, wb0_r[1], SS};
+                    if (LANE_COUNT == 4) begin
+                        out_data[53:36] <= {1'b0, wb0_r[2], SS};
+                        out_data[71:54] <= {1'b0, wb0_r[3], SS};
+                    end
                 end
             end else begin
                 if (cycl_r) begin                      // ins_cyc == INS_CYCLES-1
                     // last cycle: slot0 = final wire byte, slot1 = SE
                     out_data[17:0] <= {SE, 1'b0, wblast_r[0]};
-                    if (LANE_COUNT == 2)
+                    if (LANE_COUNT >= 2)
                         out_data[35:18] <= {SE, 1'b0, wblast_r[1]};
+                    if (LANE_COUNT == 4) begin
+                        out_data[53:36] <= {SE, 1'b0, wblast_r[2]};
+                        out_data[71:54] <= {SE, 1'b0, wblast_r[3]};
+                    end
                     inserting   <= 1'b0;
                     ins_cyc     <= 6'd0;
                     pkt_pending <= 1'b0;
@@ -576,21 +618,33 @@ module sdp_engine #(
                     // first become valid at cycle 3, seeded by the idle
                     // default index)
                     out_data[17:0] <= {1'b0, wb1a_r[0], 1'b0, wb1b_r[0]};
-                    if (LANE_COUNT == 2)
+                    if (LANE_COUNT >= 2)
                         out_data[35:18] <= {1'b0, wb1a_r[1], 1'b0, wb1b_r[1]};
+                    if (LANE_COUNT == 4) begin
+                        out_data[53:36] <= {1'b0, wb1a_r[2], 1'b0, wb1b_r[2]};
+                        out_data[71:54] <= {1'b0, wb1a_r[3], 1'b0, wb1b_r[3]};
+                    end
                     ins_cyc <= ins_cyc + 1'b1;
                 end else if (cyc2_r) begin            // ins_cyc == 2
                     out_data[17:0] <= {1'b0, wb2a_r[0], 1'b0, wb2b_r[0]};
-                    if (LANE_COUNT == 2)
+                    if (LANE_COUNT >= 2)
                         out_data[35:18] <= {1'b0, wb2a_r[1], 1'b0, wb2b_r[1]};
+                    if (LANE_COUNT == 4) begin
+                        out_data[53:36] <= {1'b0, wb2a_r[2], 1'b0, wb2b_r[2]};
+                        out_data[71:54] <= {1'b0, wb2a_r[3], 1'b0, wb2b_r[3]};
+                    end
                     ins_cyc <= ins_cyc + 1'b1;
                 end else begin
                     // slot0 = wire byte 2c-1, slot1 = wire byte 2c
                     // (3-stage pipeline: index registered at c-3, group
                     // bytes at c-2, byte registered at c-1, consumed at c)
                     out_data[17:0] <= {1'b0, wb_a_r[0], 1'b0, wb_b_r[0]};
-                    if (LANE_COUNT == 2)
+                    if (LANE_COUNT >= 2)
                         out_data[35:18] <= {1'b0, wb_a_r[1], 1'b0, wb_b_r[1]};
+                    if (LANE_COUNT == 4) begin
+                        out_data[53:36] <= {1'b0, wb_a_r[2], 1'b0, wb_b_r[2]};
+                        out_data[71:54] <= {1'b0, wb_a_r[3], 1'b0, wb_b_r[3]};
+                    end
                     ins_cyc <= ins_cyc + 1'b1;
                 end
             end
```

### A3. framebuffer_1080p.sv vs the a2p25 tip

```diff
diff --git a/hdl/video/framebuffer_1080p.sv b/hdl/video/framebuffer_1080p.sv
index c7fc12cb..824818bf 100644
--- a/hdl/video/framebuffer_1080p.sv
+++ b/hdl/video/framebuffer_1080p.sv
@@ -140,7 +140,7 @@ module framebuffer_1080p #(
 
     localparam HDMI_WIDTH  = 1920;
     localparam HDMI_H_TOTAL = 2200;  // raster line length incl. blanking (cx wraps here)
-    localparam H_TRIGGER_LEAD = 11;  // see the h_trigger block in the output path
+    localparam H_TRIGGER_LEAD = 12;  // see the h_trigger block in the output path
     localparam HDMI_HEIGHT = 1080;
     localparam H_SCALE     = 3;    // 560->1680 / 640->1920
     localparam V_SCALE     = 5;    // 192->960  / 200->1000
@@ -521,38 +521,74 @@ module framebuffer_1080p #(
     // consumed a full line after fb_line_px_r settles.
     // (10-11, a2p25 GW5A-25: the change detect and the cy==0 compare still
     // fed the tracker's update directly — -0.75 ns at 148.5 MHz on the
-    // smaller die. Every input to the decision is now a registered 1-bit
-    // flag evaluated against the CURRENT cy0_r and consumed one cycle
-    // later, so the decision is a few gates. One more cycle of latency,
-    // still deep inside blanking; the export hold below moves with it.)
-    reg at_border_nxt_r, at_lead_nxt_r, fb_line_last_r, cy_zero_r;
+    // smaller die. The tracker now acts on the REGISTERED change flag, one
+    // cycle after cy0_r moved, so the border/lead flags are evaluated
+    // against the CURRENT cy0_r (== form) rather than cy+1: at the cycle
+    // the tracker reads them, cy0_r already holds the new line. Timing
+    // round 2 cones (g)/(o) below add the registered cy==0 flag and a
+    // decision stage on top; both read these flags one cycle after the
+    // change, which is exactly what the == form is for. 2026-10-22 merge:
+    // round 2 had kept the +1 form with the later consumption, which would
+    // have put every vertical event one raster line early.)
+    reg at_border_nxt_r, at_lead_nxt_r, fb_line_last_r;
     always @(posedge clk_pixel) begin
         at_border_nxt_r <= (cy0_r == v_border_px_r);
         at_lead_nxt_r   <= (cy0_r == v_border_px_r - 11'(PREFETCH_LEAD_LINES));
-        cy_zero_r       <= (cy0_r == 11'd0);
         fb_line_last_r  <= (fb_line_px_r == fb_height_px_r[8:0] - 9'd1);
     end
 
+    // TIMING ROUND 2 CONE (g): the tracker used the RAW 11-bit compares
+    // (cy0_r != cy_prev_px_r, cy0_r == 0) as its CE/reset cone — the
+    // clk_pix path "cy_prev_px_r -> v_approach_px_r/RESET" (-0.205 in a
+    // 60K roll). Both compares are registered (cy_changed_px_r already
+    // existed; cy_zero_px_r is new) and the tracker acts one cycle later.
+    // Exact: all tracker events fire in blanking with hundreds of cycles
+    // of slack, and the at_border/at_lead/fb_line_last flags are stable
+    // across the line, so they read the same value a cycle on.
+    reg cy_zero_px_r;
+    // Timing campaign round 2, cone (o): even with registered event flags
+    // the tracker's enables combined seven of them plus the phase compare
+    // in one cycle, and "v_active_px_r -> v_active_px_r/CE" (a flop
+    // clearing itself through that cone) landed at +0.00 ns in a 60K roll.
+    // One more stage: the decision (zero / border / lead / step, phase
+    // last, line last) is registered at the event cycle and applied the
+    // cycle after, so every tracker enable is a one-level function of
+    // flops. Exact by the same argument as above: every input is stable
+    // for the whole line, the tracker itself is the only writer of the
+    // state it reads, and events fire in blanking hundreds of cycles
+    // before any consumer. The export toggle keeps its original timing.
+    reg evt_q = 1'b0;
+    reg d_zero_q = 1'b0, d_border_q = 1'b0, d_lead_q = 1'b0, d_step_q = 1'b0;
+    reg d_phlast_q = 1'b0, d_linelast_q = 1'b0;
     always @(posedge clk_pixel) begin
         cy_prev_px_r    <= cy0_r;
         cy_changed_px_r <= (cy0_r != cy_prev_px_r);
-        if (cy_changed_px_r) begin
-            if (cy_zero_r) begin
+        cy_zero_px_r    <= (cy0_r == 11'd0);
+        // decision stage (values as the tracker used to read them)
+        evt_q        <= cy_changed_px_r;
+        d_zero_q     <= cy_zero_px_r;
+        d_border_q   <= !cy_zero_px_r && at_border_nxt_r;
+        d_lead_q     <= !cy_zero_px_r && !at_border_nxt_r && at_lead_nxt_r;
+        d_step_q     <= !cy_zero_px_r && !at_border_nxt_r && !at_lead_nxt_r && v_active_px_r;
+        d_phlast_q   <= (v_phase_px_r == 3'(V_SCALE-1));
+        d_linelast_q <= fb_line_last_r;
+        if (evt_q) begin
+            if (d_zero_q) begin
                 v_active_px_r   <= 1'b0;
                 v_approach_px_r <= 1'b0;
                 v_phase_px_r    <= 3'd0;
                 fb_line_px_r    <= 9'd0;
-            end else if (at_border_nxt_r) begin
+            end else if (d_border_q) begin
                 v_active_px_r   <= 1'b1;
                 v_approach_px_r <= 1'b0;
                 v_phase_px_r    <= 3'd0;
                 fb_line_px_r    <= 9'd0;
-            end else if (at_lead_nxt_r) begin
+            end else if (d_lead_q) begin
                 v_approach_px_r <= 1'b1;
-            end else if (v_active_px_r) begin
-                if (v_phase_px_r == 3'(V_SCALE-1)) begin
+            end else if (d_step_q) begin
+                if (d_phlast_q) begin
                     v_phase_px_r <= 3'd0;
-                    if (fb_line_last_r)
+                    if (d_linelast_q)
                         v_active_px_r <= 1'b0;
                     else
                         fb_line_px_r <= fb_line_px_r + 9'd1;
@@ -1284,28 +1320,32 @@ module framebuffer_1080p #(
     // Fix: explicitly register the BSRAM read output on clk_pixel, and delay
     // the control signals to match.
 
-    // Horizontal tracker for the pixel at cx+3 (the 3-cycle read pipeline
-    // lookahead): H_SCALE phase + source-x advance incrementally from the
+    // Horizontal tracker for the pixel at cx+4 (the 4-cycle read pipeline
+    // lookahead — timing round 2 cone (i) added the address-copy stage
+    // 1b below): H_SCALE phase + source-x advance incrementally from the
     // one-shot equality trigger at the left border — no divider, no wide
     // subtract in the address path.
-    // Trigger pre-registered one cycle earlier (+5 == border at cycle N
-    // implies +4 == border at N+1; cx is monotonic and the border is far
-    // from the line wrap, so the identity is exact). The 12-bit equality
-    // then never appears in the address/CE cone.
+    // Trigger pre-registered one cycle earlier (cx == border-LEAD at cycle N
+    // implies cx == border-(LEAD-1) at N+1; cx is monotonic). The 12-bit
+    // equality then never appears in the address/CE cone, and the
+    // compare target is a registered constant (h_trigger_x_px_r).
     // The trigger X is computed modulo the raster line: with a 640-wide
     // source the active window spans the full 1920 and h_border is 0, so
     // the +5 lookahead lands in the previous line's blanking (cx = 2195).
     // The earlier "cx0_r + 5 == h_border" form could never match there —
     // IIgs SHR frames scanned out black while the framebuffer held the
     // rendered image (a2p25 10-11; the a2mega shares this read side).
-    // Apple geometry unchanged: 120 - 5 = 115.
+    // Apple geometry: 120 - H_TRIGGER_LEAD.
     // H_TRIGGER_LEAD: raster cycles between the trigger and the first
     // active pixel reaching the DP transmitter's rgb input, less the
     // transmitter's RGB_LATENCY (2). Counted 10-11 on the a2p25 (identical
-    // chain on the a2mega): this module 5 (cx0_r, h_trigger_r, stage 1-3),
-    // then the top's composite 8 (fb_rgb_osd_r 1, osd_text_overlay 1,
-    // scanline_dim 0, DebugOverlay 6 — s0/sA/sB/s1/s2/output, ENABLE
-    // notwithstanding) = 13, minus 2 = 11. The value had been 5 since the
+    // chain on the a2mega): this module 6 (cx0_r, h_trigger_r, stage 1,
+    // 1b, 2, 3 — timing round 2 cone (i) added the 1b address copy, hence
+    // 12 rather than the 11 counted on 10-11), then the top's composite 8
+    // (fb_rgb_osd_r 1, osd_text_overlay 1, scanline_dim 0, DebugOverlay 6
+    // — s0/sA/sB/s1/s2/output, ENABLE notwithstanding) = 14, minus 2 = 12.
+    // UNVERIFIED on hardware after the merge: check the IIgs SHR left
+    // edge (one 320-mode pixel of border colour = off by 6). The value had been 5 since the
     // 08-30 stage-3 split, from before the OSD/overlay stages existed in
     // this path: every frame scanned out 6 raster pixels late — invisible
     // inside the Apple modes' 120-px borders, but the borderless 640-wide
@@ -1328,6 +1368,15 @@ module framebuffer_1080p #(
     reg [1:0]  h_phase_n;
     reg [9:0]  fb_x_n;
 
+    // Timing round 2 cone (i2): the end-of-line test "fb_x_r ==
+    // fb_width - 1" (subtract + 10-bit compare) sat inside lb_rd_addr's
+    // CE cone (fb_width_px_r -> lb_rd_addr CE, +0.45 ns at 148.5 MHz).
+    // fb_x_r takes fb_x_n every cycle, so the flag is registered from the
+    // next value and is exact; fb_width is quasi-static (mode change).
+    reg [9:0]  fb_width_m1_px_r = 10'd0;
+    reg [9:0]  fb_width_m2_px_r = 10'd0;
+    reg        x_last_r = 1'b0;
+
     always @* begin
         h_act_n   = h_act_r;
         h_phase_n = h_phase_r;
@@ -1339,7 +1388,7 @@ module framebuffer_1080p #(
         end else if (h_act_r) begin
             if (h_phase_r == 2'(H_SCALE-1)) begin
                 h_phase_n = 2'd0;
-                if (fb_x_r == fb_width_px_r[9:0] - 10'd1)
+                if (x_last_r)
                     h_act_n = 1'b0;
                 else
                     fb_x_n = fb_x_r + 10'd1;
@@ -1357,6 +1406,18 @@ module framebuffer_1080p #(
         h_act_r   <= h_act_n;
         h_phase_r <= h_phase_n;
         fb_x_r    <= fb_x_n;
+        fb_width_m1_px_r <= fb_width_px_r[9:0] - 10'd1;
+        fb_width_m2_px_r <= fb_width_px_r[9:0] - 10'd2;
+        // 138B durability: the same next-value test written on the flag's
+        // own algebra instead of on the fb_x_n mux — trigger: x becomes 0
+        // (last iff width == 1); advance: x becomes x+1 (last iff x ==
+        // width-2); otherwise x holds and so does the flag. Identical to
+        // (fb_x_n == width-1) except during the one line in which the
+        // width itself changes (mode switch), which the next trigger
+        // reloads. Removes the mux from the compare cone (+0.07 on 138B).
+        x_last_r <= h_trigger_w ? (fb_width_m1_px_r == 10'd0) :
+                    (h_act_r && (h_phase_r == 2'(H_SCALE-1)) && !x_last_r)
+                        ? (fb_x_r == fb_width_m2_px_r) : x_last_r;
         // No else-zero: the address is a don't-care outside the active
         // window (output muxes on in_active_px_r), and the zeroing put a
         // RESET arc on 13 flops at the end of the trigger cone.
@@ -1365,7 +1426,22 @@ module framebuffer_1080p #(
         in_active_s1_r <= h_act_n && in_v_active_px_w;
     end
 
-    // Pipeline stage 2: BSRAM read register + delayed control (posedge N+1)
+    // Pipeline stage 1b (timing round 2 cone (i)): a plain, CE-free copy of
+    // the read address right before the BSRAM. lb_rd_addr carries a CE cone
+    // (h_act_n && in_v_active), so the placer parks it by the walker logic —
+    // far from the several BSRAM blocks the 8192-entry line buffer spans —
+    // and "lb_rd_addr -> line_buf ADB" (routing + BSRAM address setup) sat
+    // at +0.004..+0.303 across 60K rolls. This copy has no logic on either
+    // side and can be placed at the blocks. +1 read latency, compensated by
+    // the cx+4 lookahead / H_TRIGGER_LEAD above and the extra active-flag stage.
+    (* syn_keep = 1, syn_preserve = 1 *) reg [12:0] lb_rd_addr_q;
+    reg in_active_s1b_r;
+    always @(posedge clk_pixel) begin
+        lb_rd_addr_q    <= lb_rd_addr;
+        in_active_s1b_r <= in_active_s1_r;
+    end
+
+    // Pipeline stage 2: BSRAM read register + delayed control (posedge N+2)
     // This gives the synthesizer a clear clk_pixel read clock for the BSRAM
     // and ensures deterministic 1-cycle read latency.
     reg [COLOR_BITS-1:0] lb_rd_data_r;
@@ -1373,8 +1449,8 @@ module framebuffer_1080p #(
 
     always @(posedge clk_pixel) begin
         lb_rd_data_r <= (TEST_PATTERN == 2) ?
-            test_pixel(lb_rd_addr[9:0]) : rgb565_to_666(line_buf[lb_rd_addr]);
-        in_active_px_r <= in_active_s1_r;
+            test_pixel(lb_rd_addr_q[9:0]) : rgb565_to_666(line_buf[lb_rd_addr_q]);
+        in_active_px_r <= in_active_s1b_r;
     end
 
     // Pipeline stage 3 (08-30 structural timing): register the BSRAM
```

### A4. framebuffer_1080p.sv vs epic-merge-prep

```diff
diff --git a/hdl/video/framebuffer_1080p.sv b/hdl/video/framebuffer_1080p.sv
index af7ea8d5..824818bf 100644
--- a/hdl/video/framebuffer_1080p.sv
+++ b/hdl/video/framebuffer_1080p.sv
@@ -139,6 +139,8 @@ module framebuffer_1080p #(
     // =========================================================================
 
     localparam HDMI_WIDTH  = 1920;
+    localparam HDMI_H_TOTAL = 2200;  // raster line length incl. blanking (cx wraps here)
+    localparam H_TRIGGER_LEAD = 12;  // see the h_trigger block in the output path
     localparam HDMI_HEIGHT = 1080;
     localparam H_SCALE     = 3;    // 560->1680 / 640->1920
     localparam V_SCALE     = 5;    // 192->960  / 200->1000
@@ -295,6 +297,7 @@ module framebuffer_1080p #(
     reg [15:0] wr_pixel_even_r;      // buffered even pixel (RGB565)
     reg        frame_pending_r;      // waiting for first fb_we after accepted frame start
     reg        frame_parity_r;      // toggles each frame for TEST_PATTERN==6
+    reg        frame_done_parity_r; // buffer the writer last COMPLETED (read side)
 
     // 4-pair accumulator: collects 4 × 32-bit pixel pairs before FIFO push
     reg [31:0] wr_accum_r [0:2];     // Slots 0-2; slot 3 comes from current pair
@@ -334,6 +337,7 @@ module framebuffer_1080p #(
             wr_pixel_even_r <= 16'd0;
             frame_pending_r <= 1'b0;
             frame_parity_r <= 1'b0;
+            frame_done_parity_r <= 1'b1;
             wr_accum_r[0] <= 32'd0;
             wr_accum_r[1] <= 32'd0;
             wr_accum_r[2] <= 32'd0;
@@ -341,6 +345,7 @@ module framebuffer_1080p #(
             wr_accum_addr_r <= 21'd0;
         end else begin
             if (frame_start_w) begin
+                frame_done_parity_r <= frame_parity_r;   // the frame just written is complete
                 frame_parity_r <= ~frame_parity_r;
                 // Arm reset, but align to the first pixel write pulse so
                 // wr_x/wr_y stay phase-locked to the producer's fb_we stream.
@@ -514,10 +519,21 @@ module framebuffer_1080p #(
     // line is 0, and 0 never equals v_border (60/40) or v_border-LEAD
     // (48/28), so the +1 form is exact there too. fb_line_last_r is
     // consumed a full line after fb_line_px_r settles.
+    // (10-11, a2p25 GW5A-25: the change detect and the cy==0 compare still
+    // fed the tracker's update directly — -0.75 ns at 148.5 MHz on the
+    // smaller die. The tracker now acts on the REGISTERED change flag, one
+    // cycle after cy0_r moved, so the border/lead flags are evaluated
+    // against the CURRENT cy0_r (== form) rather than cy+1: at the cycle
+    // the tracker reads them, cy0_r already holds the new line. Timing
+    // round 2 cones (g)/(o) below add the registered cy==0 flag and a
+    // decision stage on top; both read these flags one cycle after the
+    // change, which is exactly what the == form is for. 2026-10-22 merge:
+    // round 2 had kept the +1 form with the later consumption, which would
+    // have put every vertical event one raster line early.)
     reg at_border_nxt_r, at_lead_nxt_r, fb_line_last_r;
     always @(posedge clk_pixel) begin
-        at_border_nxt_r <= (cy0_r + 11'd1 == v_border_px_r);
-        at_lead_nxt_r   <= (cy0_r + 11'd1 == v_border_px_r - 11'(PREFETCH_LEAD_LINES));
+        at_border_nxt_r <= (cy0_r == v_border_px_r);
+        at_lead_nxt_r   <= (cy0_r == v_border_px_r - 11'(PREFETCH_LEAD_LINES));
         fb_line_last_r  <= (fb_line_px_r == fb_height_px_r[8:0] - 9'd1);
     end
 
@@ -601,7 +617,7 @@ module framebuffer_1080p #(
     reg [8:0] fb_line_hold_px_r;
     reg       v_active_hold_px_r, v_approach_hold_px_r;
     always @(posedge clk_pixel) begin
-        if (cy0_r != cy_prev_px_r) begin
+        if (cy_changed_px_r) begin
             fb_line_hold_px_r    <= fb_line_px_r;
             v_active_hold_px_r   <= v_active_px_r;
             v_approach_hold_px_r <= v_approach_px_r;
@@ -981,9 +997,14 @@ module framebuffer_1080p #(
                 // line and pixels are written to wrong line buffer positions.
                 if (fetch_start_line0_w) begin
                     // Approaching active area — prefetch line 0
-                    // Latch read buffer: read from the buffer the writer
-                    // just FINISHED (opposite of writer's current buffer).
-                    rd_buf_offset_r <= frame_parity_r ? 21'd0 : BUFFER_OFFSET;
+                    // Latch read buffer: the buffer the writer last
+                    // COMPLETED. (Was "opposite of the writer's current
+                    // buffer" — wrong whenever the writer is idle between
+                    // frames: the display then picked the buffer the
+                    // writer took next and tore one frame later; a2p25
+                    // bench 10-11, fixed-phase 60 Hz renderer, stripe
+                    // stepped at 1/8 down every frame.)
+                    rd_buf_offset_r <= frame_done_parity_r ? BUFFER_OFFSET : 21'd0;
                     last_fetched_line_r <= 9'd0;
                     line0_prefetched_r <= 1'b1;
                     fetch_bank_r <= 3'd0;
@@ -1142,7 +1163,9 @@ module framebuffer_1080p #(
     // Stores 1 pixel per entry, serializing the 2-pixel words through a
     // small buffer on the write side.
 
-    reg [COLOR_BITS-1:0] line_buf [0:8191] /* synthesis syn_ramstyle="block_ram" */;
+    // Stored as RGB565 (the memory format, converted back on the read side):
+    // 8192 x 16 = 8 BSRAM blocks; 8192 x 18 needed 9 (a2p25 GW5A-25 fit, 10-11).
+    reg [15:0] line_buf [0:8191] /* synthesis syn_ramstyle="block_ram" */;
 
     // ---- Write side (clk domain, 54 MHz) ----
     // Each 32-bit word contains 2 packed RGB565 pixels.
@@ -1200,9 +1223,9 @@ module framebuffer_1080p #(
         end else if (rd_pixel_active_r) begin
             // Extract pixel from latched word and write to line buffer
             line_buf[{fetch_bank_r, lb_wr_pixel_x[9:0]}] <=
-                (TEST_PATTERN == 1) ? test_pixel(lb_wr_pixel_x[9:0]) :
-                rd_pixel_idx_r ? rgb565_to_666(rd_word_latched_r[31:16]) :
-                                 rgb565_to_666(rd_word_latched_r[15:0]);
+                (TEST_PATTERN == 1) ? rgb666_to_565(test_pixel(lb_wr_pixel_x[9:0])) :
+                rd_pixel_idx_r ? rd_word_latched_r[31:16] :
+                                 rd_word_latched_r[15:0];
             lb_wr_pixel_x <= lb_wr_pixel_x + 11'd1;
             if (rd_pixel_idx_r)
                 rd_pixel_active_r <= 1'b0;
@@ -1243,7 +1266,7 @@ module framebuffer_1080p #(
                     bp_sync_pending_r <= 1'b1;
 
                 if (fb_we_r) begin
-                    line_buf[{bypass_y_r[2:0], bypass_x_r[9:0]}] <= fb_data_r;
+                    line_buf[{bypass_y_r[2:0], bypass_x_r[9:0]}] <= rgb666_to_565(fb_data_r);
                     if (bypass_x_r == fb_width - 11'd1) begin
                         bypass_x_r <= 11'd0;
                         // Apply frame sync at clean line boundary only
@@ -1302,13 +1325,40 @@ module framebuffer_1080p #(
     // 1b below): H_SCALE phase + source-x advance incrementally from the
     // one-shot equality trigger at the left border — no divider, no wide
     // subtract in the address path.
-    // Trigger pre-registered one cycle earlier (+6 == border at cycle N
-    // implies +5 == border at N+1; cx is monotonic and the border is far
-    // from the line wrap, so the identity is exact). The 12-bit equality
-    // then never appears in the address/CE cone.
+    // Trigger pre-registered one cycle earlier (cx == border-LEAD at cycle N
+    // implies cx == border-(LEAD-1) at N+1; cx is monotonic). The 12-bit
+    // equality then never appears in the address/CE cone, and the
+    // compare target is a registered constant (h_trigger_x_px_r).
+    // The trigger X is computed modulo the raster line: with a 640-wide
+    // source the active window spans the full 1920 and h_border is 0, so
+    // the +5 lookahead lands in the previous line's blanking (cx = 2195).
+    // The earlier "cx0_r + 5 == h_border" form could never match there —
+    // IIgs SHR frames scanned out black while the framebuffer held the
+    // rendered image (a2p25 10-11; the a2mega shares this read side).
+    // Apple geometry: 120 - H_TRIGGER_LEAD.
+    // H_TRIGGER_LEAD: raster cycles between the trigger and the first
+    // active pixel reaching the DP transmitter's rgb input, less the
+    // transmitter's RGB_LATENCY (2). Counted 10-11 on the a2p25 (identical
+    // chain on the a2mega): this module 6 (cx0_r, h_trigger_r, stage 1,
+    // 1b, 2, 3 — timing round 2 cone (i) added the 1b address copy, hence
+    // 12 rather than the 11 counted on 10-11), then the top's composite 8
+    // (fb_rgb_osd_r 1, osd_text_overlay 1, scanline_dim 0, DebugOverlay 6
+    // — s0/sA/sB/s1/s2/output, ENABLE notwithstanding) = 14, minus 2 = 12.
+    // UNVERIFIED on hardware after the merge: check the IIgs SHR left
+    // edge (one 320-mode pixel of border colour = off by 6). The value had been 5 since the
+    // 08-30 stage-3 split, from before the OSD/overlay stages existed in
+    // this path: every frame scanned out 6 raster pixels late — invisible
+    // inside the Apple modes' 120-px borders, but the borderless 640-wide
+    // IIgs SHR frame showed 6 px (one 320-mode pixel) of border colour at
+    // its left edge. Change the chain, change this number.
+    reg [11:0] h_trigger_x_px_r;
     reg h_trigger_r;
-    always @(posedge clk_pixel)
-        h_trigger_r <= (cx0_r + 12'd6 == h_border_px_r);
+    always @(posedge clk_pixel) begin
+        h_trigger_x_px_r <= (h_border_px_r < 12'(H_TRIGGER_LEAD))
+                              ? h_border_px_r + 12'(HDMI_H_TOTAL - H_TRIGGER_LEAD)
+                              : h_border_px_r - 12'(H_TRIGGER_LEAD);
+        h_trigger_r <= (cx0_r == h_trigger_x_px_r);
+    end
     wire        h_trigger_w = h_trigger_r;
 
     reg        h_act_r;
@@ -1383,7 +1433,7 @@ module framebuffer_1080p #(
     // and "lb_rd_addr -> line_buf ADB" (routing + BSRAM address setup) sat
     // at +0.004..+0.303 across 60K rolls. This copy has no logic on either
     // side and can be placed at the blocks. +1 read latency, compensated by
-    // the cx+4 lookahead / +6 trigger above and the extra active-flag stage.
+    // the cx+4 lookahead / H_TRIGGER_LEAD above and the extra active-flag stage.
     (* syn_keep = 1, syn_preserve = 1 *) reg [12:0] lb_rd_addr_q;
     reg in_active_s1b_r;
     always @(posedge clk_pixel) begin
@@ -1399,7 +1449,7 @@ module framebuffer_1080p #(
 
     always @(posedge clk_pixel) begin
         lb_rd_data_r <= (TEST_PATTERN == 2) ?
-            test_pixel(lb_rd_addr_q[9:0]) : line_buf[lb_rd_addr_q];
+            test_pixel(lb_rd_addr_q[9:0]) : rgb565_to_666(line_buf[lb_rd_addr_q]);
         in_active_px_r <= in_active_s1b_r;
     end
 
```

### A5. esp32_ospi_connector.sv vs PR #34 (the latest a2mega-side version)

```diff
diff --git a/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv b/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv
index 4c6d94db..6d511c38 100644
--- a/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv
+++ b/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv
@@ -22,6 +22,10 @@
 // the register map and protocol details.
 //
 module esp32_ospi_connector #(
+    // 14-char gateware build stamp (YYYYMMDDHHMMSS), read a digit at a time
+    // through reg 0x3F (write the index, read the ASCII digit) — the menu's
+    // CORE: line and the same convention as the BL616 connector.
+    parameter [8*14-1:0] VERSION_STR = "00000000000000",
     parameter USE_SYNC    = 1,
     parameter USE_CRC     = 0,
     parameter IDLE_TO_CYC = 5_400_000,
@@ -54,6 +58,7 @@ module esp32_ospi_connector #(
     input  wire        a2_reset_n_i,
     input  wire        a2_alive_i,         // slot PHI1 running (apple_bus sleep_o inverted): the machine is powered
     input  wire        gs_res_n_i,         // GS CPU-socket /RES pad, raw (synchronised here; bring-up event log only)
+    output wire        storage_settled_o,  // MCU finished its mount pass (0x2E release write), or no MCU came: HDD answers NO_DEVICE instead of NOT_READY
 
     // USB HID readback (already synchronized into clk domain)
     input  wire [1:0]  pad_typ_i,        // 0 none, 1 kbd, 2 mouse, 3 gamepad
@@ -99,6 +104,11 @@ module esp32_ospi_connector #(
     input  wire [47:0]  gs_trace_i,         // {frozen, triggered, wptr[5:0], trace_data[39:0]} (connector domain)
     input  wire [159:0] gs_tele_i,          // {last_addr[23:0], high[15:0], period[15:0], hold_samples[15:0],
                                             //  hold_mismatch[15:0], be[15:0], stall[15:0], cycle[31:0], status[7:0]}
+    // DOC fetch telemetry per frame (a2p25 HyperRAM DOC; read through the
+    // indexed window at REG_GS_SEL/REG_GS_DATA, indices 40-45)
+    input  wire [15:0] dbg_doc_reqf_i,      // DOC requests per frame
+    input  wire [15:0] dbg_doc_missf_i,     // DOC memory reads per frame
+    input  wire [15:0] dbg_doc_tof_i,       // DOC late fetches per frame
     output reg         ddr3_reinit_tgl_o,   // toggles on REG_DDR3_REINIT write (CDC as toggle)
 
     // DDR3 debug read window (ddr3_debug_reader on idle port 4)
@@ -276,6 +286,7 @@ module esp32_ospi_connector #(
     localparam REG_DBG_USB_PID_H = 7'h1F;
     localparam REG_DBG_USB_CLASS = 7'h7C;   // interface class
     localparam REG_DBG_USB_SUBCL = 7'h7D;   // interface subclass
+    // DOC fetch telemetry, latched per video frame in the top (16-bit, L/H):
     localparam REG_DBG_USB_FLAGS = 7'h20;   // {connerr,x_input,fs,connected,start,rdy,stb,busy}
     localparam REG_DBG_USB_TXNS  = 7'h21;   // transaction-start counter
     localparam REG_DBG_USB_DATA  = 7'h22;   // received-data-packet counter
@@ -299,6 +310,11 @@ module esp32_ospi_connector #(
     //  20-22 last issued address {lo, hi, bank}
     localparam REG_GS_DATA = 7'h4F;
     localparam REG_GS_SEL  = 7'h5F;
+    // Gateware build stamp: W = digit index 0-13, R = ASCII digit of
+    // VERSION_STR (the BL616 keeps its version at 0x3F; here 0x3F is the
+    // SERDES status, so the last plain register is used — 0x7F is the
+    // extended-transfer opcode).
+    localparam REG_CORE_BUILD = 7'h7E;
 
     // Memory spaces (XFER via reg 0x7F)
     localparam SPACE_TEST  = 3'd0;
@@ -334,7 +350,7 @@ module esp32_ospi_connector #(
 
     // System
     reg [7:0] scratch_r;
-    reg [5:0] gs_sel_r;
+    reg [5:0] gs_sel_r;    // 0-31: IIgs socket window; 32-39: event log; 40-45: DOC telemetry
     reg [7:0] gs_ctrl_r;
     reg       a2_rst_arm_early_r;  // 0x2E write bit 4 (read bit 6): hardware-arm the socket while we still hold
     reg [6:0] evt_idx_r;           // bring-up event log read index (window reg 33)
@@ -362,38 +378,38 @@ module esp32_ospi_connector #(
     reg [7:0] gs_rdata;
     always @* begin
         case (gs_sel_r)
-            5'd0:  gs_rdata = gs_ctrl_r | (gs_hw_arm_w ? 8'h05 : 8'h00);
-            5'd1:  gs_rdata = gs_tele_i[7:0];
-            5'd2:  gs_rdata = {4'b0, gs_out_extra_r};
-            5'd3:  gs_rdata = {3'b0, gs_hold_tap_r};
-            5'd4:  gs_rdata = gs_tele_i[15:8];
-            5'd5:  gs_rdata = gs_tele_i[23:16];
-            5'd6:  gs_rdata = gs_tele_i[31:24];
-            5'd7:  gs_rdata = gs_tele_i[39:32];
-            5'd8:  gs_rdata = gs_tele_i[47:40];
-            5'd9:  gs_rdata = gs_tele_i[55:48];
-            5'd10: gs_rdata = gs_tele_i[63:56];
-            5'd11: gs_rdata = gs_tele_i[71:64];
-            5'd12: gs_rdata = gs_tele_i[79:72];
-            5'd13: gs_rdata = gs_tele_i[87:80];
-            5'd14: gs_rdata = gs_tele_i[95:88];
-            5'd15: gs_rdata = gs_tele_i[103:96];
-            5'd16: gs_rdata = gs_tele_i[111:104];
-            5'd17: gs_rdata = gs_tele_i[119:112];
-            5'd18: gs_rdata = gs_tele_i[127:120];
-            5'd19: gs_rdata = gs_tele_i[135:128];
-            5'd20: gs_rdata = gs_tele_i[143:136];
-            5'd21: gs_rdata = gs_tele_i[151:144];
-            5'd22: gs_rdata = gs_tele_i[159:152];
-            5'd23: gs_rdata = gs_trace_i[47:40];          // {frozen, triggered, wptr}
-            5'd24: gs_rdata = gs_trace_i[15:8];           // addr lo   (entry = {flags, bank, addr, data})
-            5'd25: gs_rdata = gs_trace_i[23:16];          // addr hi
-            5'd26: gs_rdata = gs_trace_i[31:24];          // bank
-            5'd27: gs_rdata = gs_trace_i[7:0];            // data at the fall
-            5'd28: gs_rdata = gs_trace_i[39:32];          // {0,0,0, VDA, VPA, be_ok, rdy, rw}
-            5'd29: gs_rdata = gs_trig_addr_r[7:0];
-            5'd30: gs_rdata = gs_trig_addr_r[15:8];
-            5'd31: gs_rdata = gs_trig_addr_r[23:16];
+            6'd0:  gs_rdata = gs_ctrl_r | (gs_hw_arm_w ? 8'h05 : 8'h00);
+            6'd1:  gs_rdata = gs_tele_i[7:0];
+            6'd2:  gs_rdata = {4'b0, gs_out_extra_r};
+            6'd3:  gs_rdata = {3'b0, gs_hold_tap_r};
+            6'd4:  gs_rdata = gs_tele_i[15:8];
+            6'd5:  gs_rdata = gs_tele_i[23:16];
+            6'd6:  gs_rdata = gs_tele_i[31:24];
+            6'd7:  gs_rdata = gs_tele_i[39:32];
+            6'd8:  gs_rdata = gs_tele_i[47:40];
+            6'd9:  gs_rdata = gs_tele_i[55:48];
+            6'd10: gs_rdata = gs_tele_i[63:56];
+            6'd11: gs_rdata = gs_tele_i[71:64];
+            6'd12: gs_rdata = gs_tele_i[79:72];
+            6'd13: gs_rdata = gs_tele_i[87:80];
+            6'd14: gs_rdata = gs_tele_i[95:88];
+            6'd15: gs_rdata = gs_tele_i[103:96];
+            6'd16: gs_rdata = gs_tele_i[111:104];
+            6'd17: gs_rdata = gs_tele_i[119:112];
+            6'd18: gs_rdata = gs_tele_i[127:120];
+            6'd19: gs_rdata = gs_tele_i[135:128];
+            6'd20: gs_rdata = gs_tele_i[143:136];
+            6'd21: gs_rdata = gs_tele_i[151:144];
+            6'd22: gs_rdata = gs_tele_i[159:152];
+            6'd23: gs_rdata = gs_trace_i[47:40];          // {frozen, triggered, wptr}
+            6'd24: gs_rdata = gs_trace_i[15:8];           // addr lo   (entry = {flags, bank, addr, data})
+            6'd25: gs_rdata = gs_trace_i[23:16];          // addr hi
+            6'd26: gs_rdata = gs_trace_i[31:24];          // bank
+            6'd27: gs_rdata = gs_trace_i[7:0];            // data at the fall
+            6'd28: gs_rdata = gs_trace_i[39:32];          // {0,0,0, VDA, VPA, be_ok, rdy, rw}
+            6'd29: gs_rdata = gs_trig_addr_r[7:0];
+            6'd30: gs_rdata = gs_trig_addr_r[15:8];
+            6'd31: gs_rdata = gs_trig_addr_r[23:16];
             6'd32: gs_rdata = {evt_full_w, evt_wptr_w};     // event log: {full, count}; write 0x80 = clear
             6'd33: gs_rdata = {1'b0, evt_idx_r};            // event log read index
             6'd34: gs_rdata = evt_rd_w[23:16];              // time[7:0]   (clk ticks, 54 MHz)
@@ -402,9 +418,19 @@ module esp32_ospi_connector #(
             6'd37: gs_rdata = evt_rd_w[47:40];              // time[31:24]
             6'd38: gs_rdata = evt_rd_w[15:8];               // event code (a2_event_log.sv)
             6'd39: gs_rdata = evt_rd_w[7:0];                // context byte / register value
+            // DOC fetch telemetry per frame (a2p25; 2026-10-22 register-map
+            // merge: 0x3C-0x3F/0x2F/0x4F/0x5F were already the DP readback
+            // and this window on the a2mega side; 32-39 is PR #34's event log)
+            6'd40: gs_rdata = dbg_doc_reqf_i[7:0];     // wavetable requests
+            6'd41: gs_rdata = dbg_doc_reqf_i[15:8];
+            6'd42: gs_rdata = dbg_doc_missf_i[7:0];    // memory reads (cache misses)
+            6'd43: gs_rdata = dbg_doc_missf_i[15:8];
+            6'd44: gs_rdata = dbg_doc_tof_i[7:0];      // fetches past the slot deadline
+            6'd45: gs_rdata = dbg_doc_tof_i[15:8];
             default: gs_rdata = 8'h00;
         endcase
     end
+    reg [3:0] ver_idx_r;             // REG_CORE_BUILD digit index
     reg [7:0] scratch1_r, scratch2_r, scratch3_r, scratch4_r;
     reg [31:0] sys_time_r;
 
@@ -523,6 +549,10 @@ module esp32_ospi_connector #(
                  rst_hold_cnt_r >= RST_HOLD_BACKSTOP[RST_CW-1:0])
             rst_released_r <= 1'b1;
     end
+    // The same fact, for the HDD card: once storage bring-up is over an
+    // unmounted unit is a genuine "no device"; before that it is "not ready"
+    // and the boot ROM waits for it instead of the machine being held in reset.
+    assign storage_settled_o = rst_released_r;
     // --- POR probe -----------------------------------------------------------
     // The IIgs (ROM 01 bench, G31-G40) wedges if the socket is driven during
     // its own power-on reset, and again if it comes out of reset with no CPU
@@ -769,6 +799,7 @@ module esp32_ospi_connector #(
             REG_SCRATCH:      reg_rdata = scratch_r;
             REG_GS_SEL:       reg_rdata = {2'b0, gs_sel_r};
             REG_GS_DATA:      reg_rdata = gs_rdata;
+            REG_CORE_BUILD:   reg_rdata = VERSION_STR[8*(13-ver_idx_r) +: 8];
             REG_STATUS:       reg_rdata = status_w;
             REG_SYSTIME_0:    reg_rdata = sys_time_r[7:0];
             REG_SYSTIME_1:    reg_rdata = sys_time_r[15:8];
@@ -916,6 +947,7 @@ module esp32_ospi_connector #(
     always @(posedge clk or negedge rst_n) begin
         if (!rst_n) begin
             scratch_r <= 8'h00;
+            ver_idx_r <= 4'd0;
             scratch1_r <= 8'h00;
             scratch2_r <= 8'h00;
             scratch3_r <= 8'h00;
@@ -1002,6 +1034,7 @@ module esp32_ospi_connector #(
             if (reg_wr_req) begin
                 case (reg_idx)
                     REG_SCRATCH:      scratch_r <= reg_wdata;
+                    REG_CORE_BUILD:   ver_idx_r <= reg_wdata[3:0];
                     REG_DDR3_REINIT:  ddr3_reinit_tgl_o <= ~ddr3_reinit_tgl_o;
                     REG_SCRATCH1:     scratch1_r <= reg_wdata;
                     REG_SCRATCH2:     scratch2_r <= reg_wdata;
@@ -1010,13 +1043,13 @@ module esp32_ospi_connector #(
                     REG_GS_SEL:       gs_sel_r <= reg_wdata[5:0];
                     REG_GS_DATA: begin
                         case (gs_sel_r)
-                            5'd0: gs_ctrl_r      <= reg_wdata;
-                            5'd2: gs_out_extra_r <= reg_wdata[3:0];
-                            5'd3: gs_hold_tap_r  <= reg_wdata[4:0];
-                            5'd23: gs_trace_idx_r <= reg_wdata[5:0];
-                            5'd29: gs_trig_addr_r[7:0]   <= reg_wdata;
-                            5'd30: gs_trig_addr_r[15:8]  <= reg_wdata;
-                            5'd31: gs_trig_addr_r[23:16] <= reg_wdata;
+                            6'd0: gs_ctrl_r      <= reg_wdata;
+                            6'd2: gs_out_extra_r <= reg_wdata[3:0];
+                            6'd3: gs_hold_tap_r  <= reg_wdata[4:0];
+                            6'd23: gs_trace_idx_r <= reg_wdata[5:0];
+                            6'd29: gs_trig_addr_r[7:0]   <= reg_wdata;
+                            6'd30: gs_trig_addr_r[15:8]  <= reg_wdata;
+                            6'd31: gs_trig_addr_r[23:16] <= reg_wdata;
                             6'd33: evt_idx_r <= reg_wdata[6:0];
                             default: ;
                         endcase
```

### A6. esp32_ospi_connector.sv vs the a2p25 tip

```diff
diff --git a/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv b/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv
index f06e49ab..6d511c38 100644
--- a/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv
+++ b/boards/a2mega/hdl/esp32/esp32_ospi_connector.sv
@@ -56,6 +56,9 @@ module esp32_ospi_connector #(
     // System status inputs (clk domain)
     input  wire        ddr3_ready_i,
     input  wire        a2_reset_n_i,
+    input  wire        a2_alive_i,         // slot PHI1 running (apple_bus sleep_o inverted): the machine is powered
+    input  wire        gs_res_n_i,         // GS CPU-socket /RES pad, raw (synchronised here; bring-up event log only)
+    output wire        storage_settled_o,  // MCU finished its mount pass (0x2E release write), or no MCU came: HDD answers NO_DEVICE instead of NOT_READY
 
     // USB HID readback (already synchronized into clk domain)
     input  wire [1:0]  pad_typ_i,        // 0 none, 1 kbd, 2 mouse, 3 gamepad
@@ -85,6 +88,24 @@ module esp32_ospi_connector #(
     input  wire [7:0]  dbg_usb_cnt_rdy_i,   // received-data-packet counter (wraps)
     input  wire [7:0]  dbg_ddr3_retry_i,    // DDR3 calib watchdog retries (saturating)
     input  wire [7:0]  dbg_ddr3_seq_i,      // {3'b0,rst_n,pll_lock,calib,state[1:0]}
+    // DP link debug readback (regs 0x3C-0x3F; quasi-static, synced in top)
+    input  wire [15:0] dbg_dp_caps_i,       // raw {DPCD 0x001 rate, DPCD 0x002 lanes}
+    input  wire [7:0]  dbg_dp_status_i,     // {5'b0, wedge, video_live, link_est}
+    input  wire [7:0]  dbg_dp_serdes_i,     // SERDES bring-up status
+
+    // IIgs CPU-socket 65C816 control/telemetry window (regs 0x5F/0x4F;
+    // hdl/twgs/gs_socket_ctl.sv).  Quasi-static both ways; the top
+    // synchronises them across the socket sequencer clock.
+    output wire [7:0]   gs_ctrl_o,          // {clear,0,0,0,0,listen,sweep_en,arm}
+    output wire [3:0]   gs_out_extra_o,     // address-delay sweep (extra clks)
+    output wire [4:0]   gs_hold_tap_o,      // data-hold sweep tap (clks after the fall)
+    output wire [5:0]   gs_trace_idx_o,     // bus-trace read index (window reg 23)
+    output wire [23:0]  gs_trig_addr_o,     // trace trigger address (window regs 29-31: lo, hi, bank)
+    input  wire [47:0]  gs_trace_i,         // {frozen, triggered, wptr[5:0], trace_data[39:0]} (connector domain)
+    input  wire [159:0] gs_tele_i,          // {last_addr[23:0], high[15:0], period[15:0], hold_samples[15:0],
+                                            //  hold_mismatch[15:0], be[15:0], stall[15:0], cycle[31:0], status[7:0]}
+    // DOC fetch telemetry per frame (a2p25 HyperRAM DOC; read through the
+    // indexed window at REG_GS_SEL/REG_GS_DATA, indices 40-45)
     input  wire [15:0] dbg_doc_reqf_i,      // DOC requests per frame
     input  wire [15:0] dbg_doc_missf_i,     // DOC memory reads per frame
     input  wire [15:0] dbg_doc_tof_i,       // DOC late fetches per frame
@@ -266,13 +287,6 @@ module esp32_ospi_connector #(
     localparam REG_DBG_USB_CLASS = 7'h7C;   // interface class
     localparam REG_DBG_USB_SUBCL = 7'h7D;   // interface subclass
     // DOC fetch telemetry, latched per video frame in the top (16-bit, L/H):
-    localparam REG_DBG_DOC_REQF_L  = 7'h3C;  // wavetable requests per frame
-    localparam REG_DBG_DOC_REQF_H  = 7'h3D;
-    localparam REG_DBG_DOC_MISSF_L = 7'h3E;  // memory reads per frame (word-cache misses)
-    localparam REG_DBG_DOC_MISSF_H = 7'h5F;
-    localparam REG_CORE_BUILD      = 7'h3F;  // W: digit index 0-13; R: ASCII digit of VERSION_STR
-    localparam REG_DBG_DOC_TOF_L   = 7'h2F;  // fetches past the slot deadline per frame
-    localparam REG_DBG_DOC_TOF_H   = 7'h4F;
     localparam REG_DBG_USB_FLAGS = 7'h20;   // {connerr,x_input,fs,connected,start,rdy,stb,busy}
     localparam REG_DBG_USB_TXNS  = 7'h21;   // transaction-start counter
     localparam REG_DBG_USB_DATA  = 7'h22;   // received-data-packet counter
@@ -280,6 +294,28 @@ module esp32_ospi_connector #(
     localparam REG_DBG_DDR3_SEQ  = 7'h24;   // {3'b0,rst_n,pll_lock,calib,state[1:0]}
     localparam REG_DDR3_REINIT   = 7'h25;   // W: any value forces a DDR3 re-init (retry-path test)
 
+    // DP link debug readback (read-only). RATE/LANES = the sink's own DPCD
+    // capability bytes captured by the AUX engine each attach — the adapter
+    // census's second gate (telnet 'd'); 0x00 = no DPCD read this session.
+    localparam REG_DBG_DP_RATE   = 7'h3C;   // raw DPCD 0x001 MAX_LINK_RATE
+    localparam REG_DBG_DP_LANES  = 7'h3D;   // raw DPCD 0x002: [3:0] lanes, [7] enh framing
+    localparam REG_DBG_DP_STATUS = 7'h3E;   // {5'b0, wedge, video_live, link_established}
+    localparam REG_DBG_DP_SERDES = 7'h3F;   // SERDES bring-up status byte
+
+    // IIgs CPU-socket window: write the index to 0x5F, read/write 0x4F.
+    //  0 CTRL  (RW) {clear[7], .., listen[2], sweep_en[1], arm[0]}    1 STATUS (RO)
+    //  2 OUT_EXTRA (RW) [3:0]   3 HOLD_TAP (RW) [4:0]
+    //  4-7 cycle count   8-9 stalls   10-11 BE-low clks   12-13 hold mismatches
+    //  14-15 hold samples   16-17 PHI2 period (clks/256 cyc)   18-19 PHI2 high
+    //  20-22 last issued address {lo, hi, bank}
+    localparam REG_GS_DATA = 7'h4F;
+    localparam REG_GS_SEL  = 7'h5F;
+    // Gateware build stamp: W = digit index 0-13, R = ASCII digit of
+    // VERSION_STR (the BL616 keeps its version at 0x3F; here 0x3F is the
+    // SERDES status, so the last plain register is used — 0x7F is the
+    // extended-transfer opcode).
+    localparam REG_CORE_BUILD = 7'h7E;
+
     // Memory spaces (XFER via reg 0x7F)
     localparam SPACE_TEST  = 3'd0;
     localparam SPACE_OSD   = 3'd1;   // OSD text page (write-only from ESP32)
@@ -314,6 +350,86 @@ module esp32_ospi_connector #(
 
     // System
     reg [7:0] scratch_r;
+    reg [5:0] gs_sel_r;    // 0-31: IIgs socket window; 32-39: event log; 40-45: DOC telemetry
+    reg [7:0] gs_ctrl_r;
+    reg       a2_rst_arm_early_r;  // 0x2E write bit 4 (read bit 6): hardware-arm the socket while we still hold
+    reg [6:0] evt_idx_r;           // bring-up event log read index (window reg 33)
+    wire [47:0] evt_rd_w;
+    wire [6:0]  evt_wptr_w;
+    wire        evt_full_w;
+    wire evt_clear_w  = reg_wr_req && reg_idx == REG_GS_DATA && gs_sel_r == 6'd32 && reg_wdata[7];
+    wire gs_ctrl_wr_w = reg_wr_req && reg_idx == REG_GS_DATA && gs_sel_r == 6'd0;
+    wire rst_reg_wr_w = reg_wr_req && reg_idx == REG_A2_RST_RELEASE;
+    // hardware arm: at the machine's own reset end (por_done), or - ARM_EARLY - already
+    // while we hold after storage-ready, so the core sits in reset on a driven bus like
+    // a real 65816 and starts on the /RES rise (TransWarp model, scoping §8b, log 09-11)
+    wire gs_hw_arm_w = a2_rst_autoarm_r &
+                       (a2_por_done_r | (a2_rst_arm_early_r & a2_rst_assert_r & rst_released_r));
+    wire a2_rst_assert_set_w = reg_wr_req && reg_idx == REG_A2_RST_RELEASE && reg_wdata[1] && !a2_rst_assert_r;
+    reg [3:0] gs_out_extra_r;
+    reg [4:0] gs_hold_tap_r;
+    reg [5:0] gs_trace_idx_r;
+    assign gs_trace_idx_o = gs_trace_idx_r;
+    reg [23:0] gs_trig_addr_r;
+    assign gs_trig_addr_o = gs_trig_addr_r;
+    assign gs_ctrl_o      = gs_ctrl_r | (gs_hw_arm_w ? 8'h05 : 8'h00);   // hardware auto-arm
+    assign gs_out_extra_o = gs_out_extra_r;
+    assign gs_hold_tap_o  = gs_hold_tap_r;
+    reg [7:0] gs_rdata;
+    always @* begin
+        case (gs_sel_r)
+            6'd0:  gs_rdata = gs_ctrl_r | (gs_hw_arm_w ? 8'h05 : 8'h00);
+            6'd1:  gs_rdata = gs_tele_i[7:0];
+            6'd2:  gs_rdata = {4'b0, gs_out_extra_r};
+            6'd3:  gs_rdata = {3'b0, gs_hold_tap_r};
+            6'd4:  gs_rdata = gs_tele_i[15:8];
+            6'd5:  gs_rdata = gs_tele_i[23:16];
+            6'd6:  gs_rdata = gs_tele_i[31:24];
+            6'd7:  gs_rdata = gs_tele_i[39:32];
+            6'd8:  gs_rdata = gs_tele_i[47:40];
+            6'd9:  gs_rdata = gs_tele_i[55:48];
+            6'd10: gs_rdata = gs_tele_i[63:56];
+            6'd11: gs_rdata = gs_tele_i[71:64];
+            6'd12: gs_rdata = gs_tele_i[79:72];
+            6'd13: gs_rdata = gs_tele_i[87:80];
+            6'd14: gs_rdata = gs_tele_i[95:88];
+            6'd15: gs_rdata = gs_tele_i[103:96];
+            6'd16: gs_rdata = gs_tele_i[111:104];
+            6'd17: gs_rdata = gs_tele_i[119:112];
+            6'd18: gs_rdata = gs_tele_i[127:120];
+            6'd19: gs_rdata = gs_tele_i[135:128];
+            6'd20: gs_rdata = gs_tele_i[143:136];
+            6'd21: gs_rdata = gs_tele_i[151:144];
+            6'd22: gs_rdata = gs_tele_i[159:152];
+            6'd23: gs_rdata = gs_trace_i[47:40];          // {frozen, triggered, wptr}
+            6'd24: gs_rdata = gs_trace_i[15:8];           // addr lo   (entry = {flags, bank, addr, data})
+            6'd25: gs_rdata = gs_trace_i[23:16];          // addr hi
+            6'd26: gs_rdata = gs_trace_i[31:24];          // bank
+            6'd27: gs_rdata = gs_trace_i[7:0];            // data at the fall
+            6'd28: gs_rdata = gs_trace_i[39:32];          // {0,0,0, VDA, VPA, be_ok, rdy, rw}
+            6'd29: gs_rdata = gs_trig_addr_r[7:0];
+            6'd30: gs_rdata = gs_trig_addr_r[15:8];
+            6'd31: gs_rdata = gs_trig_addr_r[23:16];
+            6'd32: gs_rdata = {evt_full_w, evt_wptr_w};     // event log: {full, count}; write 0x80 = clear
+            6'd33: gs_rdata = {1'b0, evt_idx_r};            // event log read index
+            6'd34: gs_rdata = evt_rd_w[23:16];              // time[7:0]   (clk ticks, 54 MHz)
+            6'd35: gs_rdata = evt_rd_w[31:24];              // time[15:8]
+            6'd36: gs_rdata = evt_rd_w[39:32];              // time[23:16]
+            6'd37: gs_rdata = evt_rd_w[47:40];              // time[31:24]
+            6'd38: gs_rdata = evt_rd_w[15:8];               // event code (a2_event_log.sv)
+            6'd39: gs_rdata = evt_rd_w[7:0];                // context byte / register value
+            // DOC fetch telemetry per frame (a2p25; 2026-10-22 register-map
+            // merge: 0x3C-0x3F/0x2F/0x4F/0x5F were already the DP readback
+            // and this window on the a2mega side; 32-39 is PR #34's event log)
+            6'd40: gs_rdata = dbg_doc_reqf_i[7:0];     // wavetable requests
+            6'd41: gs_rdata = dbg_doc_reqf_i[15:8];
+            6'd42: gs_rdata = dbg_doc_missf_i[7:0];    // memory reads (cache misses)
+            6'd43: gs_rdata = dbg_doc_missf_i[15:8];
+            6'd44: gs_rdata = dbg_doc_tof_i[7:0];      // fetches past the slot deadline
+            6'd45: gs_rdata = dbg_doc_tof_i[15:8];
+            default: gs_rdata = 8'h00;
+        endcase
+    end
     reg [3:0] ver_idx_r;             // REG_CORE_BUILD digit index
     reg [7:0] scratch1_r, scratch2_r, scratch3_r, scratch4_r;
     reg [31:0] sys_time_r;
@@ -355,6 +471,12 @@ module esp32_ospi_connector #(
 
     // Apple II reset release
     reg        a2_rst_release_r;
+    reg        a2_rst_assert_r;    // 0x2E bit 1: hold the Apple II in reset while set (socket arm sequence)
+    reg        a2_rst_probe_r;     // 0x2E bit 2: while holding, let the line go for PROBE_WINDOW every
+                                   //   PROBE_PERIOD and sample it: the moment the machine's own
+                                   //   power-on reset stops pulling it low, stay released (por_done)
+    reg        a2_rst_autoarm_r;   // 0x2E bit 3: on por_done, arm the GS socket in the same clock
+    reg        a2_por_done_r;      // read 0x2E bit 5: machine's reset released by the probe
 
     // W5100 doorbell clear
     reg [3:0]  w5100_cmd_clr_r;
@@ -415,11 +537,133 @@ module esp32_ospi_connector #(
     end
     wire rst_mcu_absent_w = !mcu_ready_r &&
                             (rst_hold_cnt_r >= RST_MCU_ALIVE_WAIT[RST_CW-1:0]);
+    // Any release is final: once the hold has dropped (MCU write, no-MCU
+    // fallback, or backstop) it must never re-assert — the no-MCU fallback
+    // used to un-release when the MCU's first STATUS read arrived later,
+    // resetting a machine that had already booted (bench G32).
+    reg rst_released_r;
+    always @(posedge clk or negedge rst_n) begin
+        if (!rst_n)
+            rst_released_r <= 1'b0;
+        else if (a2_rst_release_r || rst_mcu_absent_w ||
+                 rst_hold_cnt_r >= RST_HOLD_BACKSTOP[RST_CW-1:0])
+            rst_released_r <= 1'b1;
+    end
+    // The same fact, for the HDD card: once storage bring-up is over an
+    // unmounted unit is a genuine "no device"; before that it is "not ready"
+    // and the boot ROM waits for it instead of the machine being held in reset.
+    assign storage_settled_o = rst_released_r;
+    // --- POR probe -----------------------------------------------------------
+    // The IIgs (ROM 01 bench, G31-G40) wedges if the socket is driven during
+    // its own power-on reset, and again if it comes out of reset with no CPU
+    // present for long. So: hold the slot reset from the card, probe the line
+    // every ms with a short release, and the first time it floats high keep
+    // it released and arm the socket at once (microseconds, not a timer).
+    // Only meaningful once the storage-ready release (rst_released_r) is in.
+    localparam PROBE_PERIOD = CLOCK_SPEED_HZ / 1000;        // 1 ms
+    localparam PROBE_WINDOW = CLOCK_SPEED_HZ / 25000;       // 40 us
+    reg [$clog2(PROBE_PERIOD+1)-1:0] probe_cnt_r;
+    reg        probe_window_r;
+    reg [1:0]  a2_reset_n_s;                                // sync
+    wire       probe_active = a2_rst_probe_r & a2_rst_assert_r & rst_released_r & ~a2_por_done_r;
+    wire       por_done_set = probe_active & probe_window_r &
+                              (probe_cnt_r == PROBE_PERIOD - 1) & a2_reset_n_s[1];
+    always @(posedge clk or negedge rst_n) begin
+        if (!rst_n) begin
+            probe_cnt_r    <= '0;
+            probe_window_r <= 1'b0;
+            a2_reset_n_s   <= 2'b00;
+            a2_por_done_r  <= 1'b0;
+        end else begin
+            a2_reset_n_s <= {a2_reset_n_s[0], a2_reset_n_i};
+            if (!probe_active) begin
+                probe_cnt_r    <= '0;
+                probe_window_r <= 1'b0;
+            end else begin
+                probe_cnt_r    <= (probe_cnt_r == PROBE_PERIOD - 1) ? '0 : probe_cnt_r + 1'b1;
+                probe_window_r <= (probe_cnt_r >= PROBE_PERIOD - PROBE_WINDOW - 1) &&
+                                  (probe_cnt_r != PROBE_PERIOD - 1);
+            end
+            if (por_done_set)
+                a2_por_done_r <= 1'b1;
+            else if (a2_rst_assert_set_w)
+                a2_por_done_r <= 1'b0;                      // a new hold starts a new sequence
+        end
+    end
+
     assign a2bus_control_if.reset_hold =
-        !(a2_rst_release_r || rst_mcu_absent_w ||
-          rst_hold_cnt_r >= RST_HOLD_BACKSTOP[RST_CW-1:0]);
+        (a2_rst_assert_r & ~probe_window_r & ~a2_por_done_r) | !rst_released_r;
     assign a2bus_control_if.ready = 1'b1;
 
+    // --- bring-up event log (window regs 32-39; hdl/esp32/a2_event_log.sv) ------
+    // Timestamped edges of everything that touches the Apple II reset and the
+    // GS socket enables, so a bench power-up can be reconstructed after the
+    // fact (which lever pulsed /RES, when the core started, when the machine
+    // let go). Codes: 1 slot /RESET fall, 2 rise, 3 socket /RES fall, 4 rise,
+    // 5 our hold on, 6 off (probe windows excluded), 7 0x2E write (data =
+    // value), 8 CTRL write (data = value), 9 por_done, 10 clock alive, 11 clock
+    // lost, 12 storage/backstop release, 13 MCU ready, 14 core running, 15 core
+    // reset, 16 trace trigger, 17 socket pins ours, 18 pins released, 19 probe
+    // start, 20 hardware arm. Data (unless a register value) = context
+    // {hold, released, por_done, alive, listen, arm, pins_ours, running}.
+    reg [1:0] gs_res_n_s;
+    reg a2_reset_n_d, gs_res_n_d, hold_core_d, alive_d, released_d, mcu_ready_d,
+        running_d, enabled_d, trigd_d, probe_d, hwarm_d, por_done_d;
+    wire hold_core_w = (a2_rst_assert_r & ~a2_por_done_r) | !rst_released_r;   // our hold, probe windows excluded
+    wire running_w = gs_tele_i[6];      // socket status bit 6 (top synchronises gs_tele)
+    wire enabled_w = gs_tele_i[5];      // socket status bit 5: pins are ours
+    wire trigd_w   = gs_trace_i[46];    // trace trigger fired
+    always @(posedge clk or negedge rst_n) begin
+        if (!rst_n) begin
+            gs_res_n_s <= 2'b11;
+            {a2_reset_n_d, gs_res_n_d, hold_core_d, alive_d, released_d, mcu_ready_d,
+             running_d, enabled_d, trigd_d, probe_d, hwarm_d, por_done_d} <= 12'b1100_0000_0000;
+        end else begin
+            gs_res_n_s   <= {gs_res_n_s[0], gs_res_n_i};
+            a2_reset_n_d <= a2_reset_n_s[1];
+            gs_res_n_d   <= gs_res_n_s[1];
+            hold_core_d  <= hold_core_w;
+            alive_d      <= a2_alive_f_r;
+            released_d   <= rst_released_r;
+            mcu_ready_d  <= mcu_ready_r;
+            running_d    <= running_w;
+            enabled_d    <= enabled_w;
+            trigd_d      <= trigd_w;
+            probe_d      <= probe_active;
+            hwarm_d      <= gs_hw_arm_w;
+            por_done_d   <= a2_por_done_r;
+        end
+    end
+    wire [23:0] evt_w = {
+        4'b0,
+        gs_hw_arm_w & ~hwarm_d,             // 20 hardware arm
+        probe_active & ~probe_d,            // 19 probe start
+        ~enabled_w & enabled_d,             // 18 socket pins released
+        enabled_w & ~enabled_d,             // 17 socket pins ours
+        trigd_w & ~trigd_d,                 // 16 trace trigger
+        ~running_w & running_d,             // 15 core reset
+        running_w & ~running_d,             // 14 core running
+        mcu_ready_r & ~mcu_ready_d,         // 13 MCU ready
+        rst_released_r & ~released_d,       // 12 storage/backstop release
+        ~a2_alive_f_r & alive_d,            // 11 clock lost
+        a2_alive_f_r & ~alive_d,            // 10 clock alive
+        a2_por_done_r & ~por_done_d,        //  9 por_done
+        gs_ctrl_wr_w,                       //  8 CTRL write
+        rst_reg_wr_w,                       //  7 0x2E write
+        ~hold_core_w & hold_core_d,         //  6 our hold off
+        hold_core_w & ~hold_core_d,         //  5 our hold on
+        gs_res_n_s[1] & ~gs_res_n_d,        //  4 socket /RES rise
+        ~gs_res_n_s[1] & gs_res_n_d,        //  3 socket /RES fall
+        a2_reset_n_s[1] & ~a2_reset_n_d,    //  2 slot /RESET rise
+        ~a2_reset_n_s[1] & a2_reset_n_d};   //  1 slot /RESET fall
+    wire [7:0] evt_ctx_w = {a2bus_control_if.reset_hold, rst_released_r, a2_por_done_r, a2_alive_f_r,
+                            gs_ctrl_o[2], gs_ctrl_o[0], enabled_w, running_w};
+    a2_event_log #(.DEPTH_LOG2(7)) i_evt_log (
+        .clk(clk), .rst_n(rst_n), .clear_i(evt_clear_w), .ev_i(evt_w), .ctx_i(evt_ctx_w),
+        .rst_wdata_i(reg_wdata), .ctrl_wdata_i(reg_wdata), .time_i(sys_time_r),
+        .rd_idx_i(evt_idx_r), .rd_data_o(evt_rd_w), .wptr_o(evt_wptr_w), .full_o(evt_full_w)
+    );
+
     // =========================================================================
     // Memory Spaces — using Gowin BSRAM inference pattern
     // =========================================================================
@@ -510,8 +754,27 @@ module esp32_ospi_connector #(
     wire vol_pending_w = volumes[0].rd | volumes[0].wr | volumes[1].rd | volumes[1].wr;
     wire hdd_pending_w = hdd_volumes[0].rd | hdd_volumes[0].wr |
                          hdd_volumes[1].rd | hdd_volumes[1].wr;
+    // Apple II alive = slot PHI1 running. sleep_o drops out between edges only
+    // if the clock stops for >1 us, so a short filter makes a clean level:
+    // alive after ~100 us of clock, dead as soon as sleep is reported.
+    reg [12:0] alive_cnt_r;
+    reg        a2_alive_f_r;
+    always @(posedge clk or negedge rst_n) begin
+        if (!rst_n) begin
+            alive_cnt_r  <= '0;
+            a2_alive_f_r <= 1'b0;
+        end else if (!a2_alive_i) begin
+            alive_cnt_r  <= '0;
+            a2_alive_f_r <= 1'b0;
+        end else if (&alive_cnt_r) begin
+            a2_alive_f_r <= 1'b1;
+        end else begin
+            alive_cnt_r  <= alive_cnt_r + 1'b1;
+        end
+    end
+
     wire [7:0] status_w = {
-        1'b0,
+        a2_alive_f_r,               // [7] Apple II alive (slot PHI1 running)
         pad_typ_i != 2'd0,          // [6] HID device present
         |w5100_cmd_pending,         // [5] W5100 doorbell pending
         hdd_pending_w,              // [4] HDD request pending
@@ -534,6 +797,8 @@ module esp32_ospi_connector #(
             REG_PROTO_VER:    reg_rdata = PROTO_VER;
             REG_CAPABILITIES: reg_rdata = CAP0;
             REG_SCRATCH:      reg_rdata = scratch_r;
+            REG_GS_SEL:       reg_rdata = {2'b0, gs_sel_r};
+            REG_GS_DATA:      reg_rdata = gs_rdata;
             REG_CORE_BUILD:   reg_rdata = VERSION_STR[8*(13-ver_idx_r) +: 8];
             REG_STATUS:       reg_rdata = status_w;
             REG_SYSTIME_0:    reg_rdata = sys_time_r[7:0];
@@ -594,12 +859,12 @@ module esp32_ospi_connector #(
             REG_DBG_USB_DATA:   reg_rdata = dbg_usb_cnt_rdy_i;
             REG_DBG_DDR3_RETRY: reg_rdata = dbg_ddr3_retry_i;
             REG_DBG_DDR3_SEQ:   reg_rdata = dbg_ddr3_seq_i;
-            REG_DBG_DOC_REQF_L:  reg_rdata = dbg_doc_reqf_i[7:0];
-            REG_DBG_DOC_REQF_H:  reg_rdata = dbg_doc_reqf_i[15:8];
-            REG_DBG_DOC_MISSF_L: reg_rdata = dbg_doc_missf_i[7:0];
-            REG_DBG_DOC_MISSF_H: reg_rdata = dbg_doc_missf_i[15:8];
-            REG_DBG_DOC_TOF_L:   reg_rdata = dbg_doc_tof_i[7:0];
-            REG_DBG_DOC_TOF_H:   reg_rdata = dbg_doc_tof_i[15:8];
+
+            // DP link debug readback
+            REG_DBG_DP_RATE:    reg_rdata = dbg_dp_caps_i[15:8];
+            REG_DBG_DP_LANES:   reg_rdata = dbg_dp_caps_i[7:0];
+            REG_DBG_DP_STATUS:  reg_rdata = dbg_dp_status_i;
+            REG_DBG_DP_SERDES:  reg_rdata = dbg_dp_serdes_i;
 
             // ProDOS HDD compact bank
             REG_HDD0_REQ_CTL: reg_rdata = {6'b0, hdd_volumes[0].wr, hdd_volumes[0].rd};
@@ -609,7 +874,7 @@ module esp32_ospi_connector #(
             REG_HDD1_LBA_L:   reg_rdata = hdd_volumes[1].lba[7:0];
             REG_HDD1_LBA_H:   reg_rdata = hdd_volumes[1].lba[15:8];
 
-            REG_A2_RST_RELEASE: reg_rdata = {7'b0, a2_rst_release_r};
+            REG_A2_RST_RELEASE: reg_rdata = {1'b0, a2_rst_arm_early_r, a2_por_done_r, a2bus_control_if.reset_hold, a2_rst_autoarm_r, a2_rst_probe_r, a2_rst_assert_r, a2_rst_release_r};
 
             // Slot configuration
             REG_SLOT_SELECT:  reg_rdata = {5'b0, slot_select_r};
@@ -687,6 +952,13 @@ module esp32_ospi_connector #(
             scratch2_r <= 8'h00;
             scratch3_r <= 8'h00;
             scratch4_r <= 8'h00;
+            gs_sel_r <= 6'd0;
+            evt_idx_r <= 7'd0;
+            gs_trace_idx_r <= 6'd0;
+            gs_trig_addr_r <= 24'd0;
+            gs_ctrl_r <= 8'h00;
+            gs_out_extra_r <= 4'd0;
+            gs_hold_tap_r <= 5'd0;
             video_enable_r <= 1'b0;
             video_mode_r <= 8'b00010001;  // TEXT_MODE=1, AN3=1
             text_color_r <= 4'd15;
@@ -718,6 +990,10 @@ module esp32_ospi_connector #(
             hdd_ack_r[0] <= 1'b0;
             hdd_ack_r[1] <= 1'b0;
             a2_rst_release_r <= 1'b0;
+            a2_rst_assert_r  <= 1'b0;
+            a2_rst_probe_r   <= 1'b0;
+            a2_rst_autoarm_r <= 1'b0;
+            a2_rst_arm_early_r <= 1'b0;
             w5100_cmd_clr_r <= 4'b0;
             gpu_trigger_r <= 1'b0;
             gpu_pause_r <= 1'b0;
@@ -764,6 +1040,20 @@ module esp32_ospi_connector #(
                     REG_SCRATCH2:     scratch2_r <= reg_wdata;
                     REG_SCRATCH3:     scratch3_r <= reg_wdata;
                     REG_SCRATCH4:     scratch4_r <= reg_wdata;
+                    REG_GS_SEL:       gs_sel_r <= reg_wdata[5:0];
+                    REG_GS_DATA: begin
+                        case (gs_sel_r)
+                            6'd0: gs_ctrl_r      <= reg_wdata;
+                            6'd2: gs_out_extra_r <= reg_wdata[3:0];
+                            6'd3: gs_hold_tap_r  <= reg_wdata[4:0];
+                            6'd23: gs_trace_idx_r <= reg_wdata[5:0];
+                            6'd29: gs_trig_addr_r[7:0]   <= reg_wdata;
+                            6'd30: gs_trig_addr_r[15:8]  <= reg_wdata;
+                            6'd31: gs_trig_addr_r[23:16] <= reg_wdata;
+                            6'd33: evt_idx_r <= reg_wdata[6:0];
+                            default: ;
+                        endcase
+                    end
 
                     REG_VIDEO_ENABLE: video_enable_r <= reg_wdata[0];
                     REG_VIDEO_MODE:   video_mode_r <= reg_wdata;
@@ -789,7 +1079,13 @@ module esp32_ospi_connector #(
                     REG_HDD1_LBA_H:   hdd_size_r[1][15:8] <= reg_wdata;
                     REG_HDD1_ACK:     hdd_ack_r[1] <= 1'b1;
 
-                    REG_A2_RST_RELEASE: a2_rst_release_r <= reg_wdata[0];
+                    REG_A2_RST_RELEASE: begin
+                        a2_rst_release_r <= reg_wdata[0];
+                        a2_rst_assert_r  <= reg_wdata[1];
+                        a2_rst_probe_r   <= reg_wdata[2];
+                        a2_rst_autoarm_r <= reg_wdata[3];
+                        a2_rst_arm_early_r <= reg_wdata[4];
+                    end
 
                     REG_SLOT_SELECT:  slot_select_r <= reg_wdata[2:0];
                     REG_SLOT_CARD: begin
```

### A7. a2mega top.sv vs PR #34

```diff
diff --git a/boards/a2mega/hdl/top.sv b/boards/a2mega/hdl/top.sv
index ae4ebb05..0ca26bba 100644
--- a/boards/a2mega/hdl/top.sv
+++ b/boards/a2mega/hdl/top.sv
@@ -977,6 +977,7 @@ module top #(
     wire [7:0] hdd_d_w;
     wire hdd_rd;
 
+    wire storage_settled_w;     // from the connector: MCU mount pass over (or no MCU)
     HDD #(
         .ENABLE(HDD_ENABLE),
         .ID(HDD_ID)
@@ -986,7 +987,8 @@ module top #(
         .data_o(hdd_d_w),
         .rd_en_o(hdd_rd),
         .ram_hdd_if(hdd_ram_if),
-        .volumes(hdd_volumes)
+        .volumes(hdd_volumes),
+        .storage_settled_i(storage_settled_w)
     );
 
     // Uthernet II (W5100) Ethernet card. The ESP32 services the MACRAW
@@ -1545,6 +1547,32 @@ module top #(
         .TRAIN_RECOVER    (1),          // paced retries, TX held
         .ERRCNT_READ      (1),          // 0x210-0x213 every check
         .HPD_DISCONNECT_RESETS(0),      // flap storms: no HPD teardown
+        // a2p25 AUX/training stack port (10-12, c7064c6e..e4f2e056 whole;
+        // see bringup_1_0a3_log.md). Every new knob is pinned EXPLICITLY at
+        // the value this board ran before, so the only behaviour change in
+        // this bitstream is the one the port carries at defaults: the EQ
+        // status-read dwell now follows the sink's DPCD 0x00E
+        // TRAINING_AUX_RD_INTERVAL (0 -> 400 us as before; n -> n x 4 ms,
+        // cap 16 ms) instead of a fixed 4 x 100 us, plus the 16-byte
+        // capability read that fetches it and the msg-0x00 engine fix.
+        .LINK_CHECK       (1),          // spec flow: periodic/hpd_irq status
+                                        // checks tear a failed link down
+                                        // (what the a2mega always ran;
+                                        // a2p25's 0/2 were AUX-RX bring-up aids)
+        .LINK_CHECK_CLKS  (27'd99999999), // ~1 s at 100 MHz, as before
+        .RETRY_WATCHDOG_CLKS(29'd49999999), // 0.5 s attach watchdog, as before
+        .EQ_WAIT_MULT     (6'd1),       // no extra EQ-dwell floor: the
+                                        // 0x00E-derived dwell alone
+        .TRAIN_ERRCNT     (0),          // no 0x210 reads inside the EQ loop
+        .LATE_REPLY_DRAIN (0),          // a2p25 drains converters' trailing
+                                        // DEFER bytes; NOT taken here unless
+                                        // an a2mega sink shows the need
+        // ECHO_SINK_ADJUST is an AFE-sequencer-OFF feature (a2p25: the
+        // SN75DP130 redriver snoops TRAINING_LANEx_SET). The a2mega's AFE
+        // policy is the M5 runtime sequencer below (ENABLE_AFE_ADJUST from
+        // the die pkg) which already declares the truthful lane-set byte,
+        // so the echo path stays off = byte-identical legacy.
+        .ECHO_SINK_ADJUST (0),
         // M5 runtime AFE: per-die verified bases from the die pkg
         .ENABLE_AFE_ADJUST(dp_test_die_pkg::ENABLE_AFE_ADJUST),
         .AFE_LANE_BASE0   (dp_test_die_pkg::AFE_LANE_BASE0),
@@ -1586,6 +1614,13 @@ module top #(
         .debug_errcnt0     (dp_errcnt0_w),
         .debug_errcnt1     (dp_errcnt1_w),
         .debug_sink_caps   (dp_sink_caps_w),
+        // a2p25-stack observe/liveness outputs (LINK_CHECK==2 counters are
+        // zero at LINK_CHECK=1; packer underrun/level unread here — the
+        // ESP32 telemetry map has no slot for them yet)
+        .debug_obs         (),
+        .debug_auxif       (),
+        .debug_underrun_cnt(),
+        .debug_fifo_level  (),
         // DRP register-readback debug bridge (used interactively by the
         // dp_test bitstream; idle here — index parked, outputs unread)
         .drp_dbg_idx       (5'd0),
@@ -1990,6 +2025,7 @@ module top #(
 
     // Octal SPI connector instance
     esp32_ospi_connector #(
+        .VERSION_STR(`BUILD_DATETIME),
         .USE_SYNC(1),
         .USE_CRC(0),
         .IDLE_TO_CYC(5_400_000),  // ~100ms at 54MHz
@@ -2016,6 +2052,7 @@ module top #(
         .a2_reset_n_i(a2_reset_n),
         .a2_alive_i(~sleep_w),
         .gs_res_n_i(gs_res_n),
+        .storage_settled_o(storage_settled_w),
 
         .pad_typ_i(hid_typ_sync1),
         .pad_connerr_i(hid_connerr_sync1),
@@ -2046,6 +2083,9 @@ module top #(
         .dbg_dp_caps_i(dp_caps_sync1),
         .dbg_dp_status_i({5'b0, dp_stat_sync1}),
         .dbg_dp_serdes_i(dp_serdes_sync1),
+        .dbg_doc_reqf_i(doc_dbg_req_frame_r),
+        .dbg_doc_missf_i(16'd0),
+        .dbg_doc_tof_i(16'd0),
         .ddr3_reinit_tgl_o(ddr3_reinit_tgl_w),
 
         .gs_ctrl_o(gs_ctrl_w),
```

### A8. a2p25 top.sv vs the a2p25 tip

```diff
diff --git a/boards/a2p25/hdl/top.sv b/boards/a2p25/hdl/top.sv
index 28ac92ae..9b509d0c 100644
--- a/boards/a2p25/hdl/top.sv
+++ b/boards/a2p25/hdl/top.sv
@@ -1900,6 +1900,22 @@ module top #(
         .dbg_doc_reqf_i(doc_dbg_req_frame_r),
         .dbg_doc_missf_i(doc_dbg_miss_frame_r),
         .dbg_doc_tof_i(doc_dbg_to_frame_r),
+        // a2mega-only debug sources, tied off (2026-10-22 merge): DP SERDES /
+        // link readback and the IIgs CPU-socket window. The a2p25 could
+        // feed dbg_dp_caps_i from dp_transmitter's debug_sink_caps (census
+        // second gate) the way the a2mega top does — follow-up.
+        .dbg_dp_caps_i(16'd0),
+        .dbg_dp_status_i(8'd0),
+        .dbg_dp_serdes_i(8'd0),
+        .gs_ctrl_o(),
+        .gs_out_extra_o(),
+        .gs_hold_tap_o(),
+        .gs_trace_idx_o(),
+        .gs_trig_addr_o(),
+        .gs_trace_i(48'd0),
+        .gs_tele_i(160'd0),
+        .a2_alive_i(~sleep_w),       // slot PHI1 running (same as the a2mega)
+        .gs_res_n_i(1'b1),           // no CPU socket on the a2p25
         .ddr3_reinit_tgl_o(ddr3_reinit_tgl_w),
 
         .dbg_mem_addr_o(dbg_mem_addr_w),
```

### A9. ESP32 firmware vs the a2p25 tip (disk.c, Makefile, telnetd.c, menu.c, viddbg)

```diff
diff --git a/boards/a2mega/src/a2fpga_esp32/Makefile b/boards/a2mega/src/a2fpga_esp32/Makefile
index a61e0e98..9ff844ad 100644
--- a/boards/a2mega/src/a2fpga_esp32/Makefile
+++ b/boards/a2mega/src/a2fpga_esp32/Makefile
@@ -51,13 +51,13 @@ CPP_FILES = a2fpga_jtag.cpp usbc_glue.cpp usbhost.cpp usbstorage.cpp
 C_FILES = fwupdate.c a2fpga_ospi_link.c a2fpga_spi_service.c fpga_link.c fpga_screen.c wedge_watch.c \
           osd_console.c menu.c settings.c disk.c gcr_dsk.c w5100.c \
           wifi_bridge.c fpga_jtag.c fpgaupdate.c fpgastream.c ftpd.c telnetd.c \
-          fusb302.c usbc_port.c esp32s3_integration.c sn75dp130.c
+          fusb302.c usbc_port.c esp32s3_integration.c sn75dp130.c gs_socket.c
 HEADER_FILES = a2fpga_jtag.h a2fpga_ospi_link.h a2fpga_spi_service.h wedge_watch.h \
                a2fpga_regs.h fpga_link.h fpga_screen.h osd_console.h menu.h \
                net_status.h settings.h disk.h gcr_dsk.h w5100.h \
                wifi_bridge.h fpga_jtag.h fpgaupdate.h fpgastream.h ftpd.h telnetd.h \
                board_pins.h fusb302.h usb_pd.h usbc_port.h usbc_glue.h \
-               esp32s3_integration.h sn75dp130.h usbhost.h usbstorage.h fwupdate.h
+               esp32s3_integration.h sn75dp130.h usbhost.h usbstorage.h fwupdate.h gs_socket.h
 ALL_SOURCES = $(SKETCH) $(CPP_FILES) $(C_FILES) $(HEADER_FILES)
 
 # Default target
diff --git a/boards/a2mega/src/a2fpga_esp32/disk.c b/boards/a2mega/src/a2fpga_esp32/disk.c
index 2bf6087d..08f01c82 100644
--- a/boards/a2mega/src/a2fpga_esp32/disk.c
+++ b/boards/a2mega/src/a2fpga_esp32/disk.c
@@ -44,6 +44,7 @@ void a2_reset_hold_release(const char *why);
 #include "usbstorage.h"   /* USB stick as the image volume (backend switch) */
 #include "settings.h"     /* persisted image overrides + slot map */
 #include "disk.h"
+#include "gs_socket.h"
 
 static const char *TAG = "disk";
 
@@ -766,8 +767,10 @@ void disk_poll(void)
             for (int v = 0; v < NDRV; v++) any = any || g_mounted[v];
             for (int u = 0; u < NHDD; u++) any = any || g_hdd_mounted[u];
             /* 10 s (was 7): the USB host role decision takes 5 s and a stick
-             * needs a moment to enumerate and mount before the Apple scans. */
-            if ((any && !g_remount_req) ||
+             * needs a moment to enumerate and mount before the Apple scans.
+             * gs_socket_ready(): the in-socket 65C816 auto-arm must have
+             * settled before /RES rises (bench G30; PR #34). */
+            if ((any && !g_remount_req && gs_socket_ready()) ||
                 esp_timer_get_time() > 10000000) {
                 /* Program the slot map JUST before the release — this late in
                  * boot the link is proven good (the mounts above ran over it),
@@ -775,7 +778,7 @@ void disk_poll(void)
                  * race-free. */
                 apply_slot_map();
 
-                fpga_reg_write(A2REG_A2_RST_RELEASE, 1);
+                gs_socket_a2_release();              /* 0x2E.0, via the socket module (keeps its assert bit) */
                 /* boards where the ESP32 owns the /RES pin (a2p25 IO33) */
                 a2_reset_hold_release(any ? "media mounted" : "no media");
                 s_released = true;
diff --git a/boards/a2mega/src/a2fpga_esp32/menu.c b/boards/a2mega/src/a2fpga_esp32/menu.c
index adbf91e6..cc7f15b0 100644
--- a/boards/a2mega/src/a2fpga_esp32/menu.c
+++ b/boards/a2mega/src/a2fpga_esp32/menu.c
@@ -30,6 +30,7 @@
 #include "osd_console.h"
 #include "settings.h"
 #include "disk.h"
+#include "gs_socket.h"
 #include "fpgaupdate.h"
 #include "net_status.h"
 #include "menu.h"
@@ -50,7 +51,7 @@ static const char *TAG = "menu";
 /* Gateware build stamp, indexed reg (write index, read ASCII digit). Not in
  * a2fpga_regs.h because older a2mega cores may not implement it — the reader
  * degrades to "(NO VERSION REG)" when the digits don't come back. */
-#define A2REG_CORE_BUILD  0x3Fu
+#define A2REG_CORE_BUILD  0x7Eu   /* was 0x3F until the 2026-10-22 merge: 0x3F is the a2mega SERDES status */
 
 /* ---- display modes ------------------------------------------------------ */
 typedef enum { VIEW_APPLE, VIEW_CONSOLE, VIEW_MENU } view_t;
@@ -256,7 +257,7 @@ static void save_settings_status(void)
 }
 
 /* ---- gateware build stamp ------------------------------------------------ */
-/* Read the gateware build stamp via indexed reg 0x3F (write index, read
+/* Read the gateware build stamp via indexed reg 0x7E (write index, read
  * ASCII digit). Cores predating the register return zeros -> unknown. */
 static bool fpga_core_version(char *out, size_t cap)
 {
@@ -955,8 +956,24 @@ static void settings_do_reset(int id)
     screen_refresh();
 }
 
+static void gs_socket_change(int id, int dir)
+{
+    (void)id; (void)dir;
+    settings()->gs_socket_off = !settings()->gs_socket_off;
+    save_settings_status();
+    gs_socket_apply();
+    screen_refresh();
+}
+
 static void settings_build(void)
 {
+    {
+        menu_item_t *g = mi_add(MI_TOGGLE, "GS SOCKET 65816",
+                                settings()->gs_socket_off ? "OFF" : "AUTO");
+        g->on_change = gs_socket_change;
+        mi_add(MI_INFO, gs_socket_state_str(), "");
+        mi_add(MI_INFO, "", "");
+    }
     mi_add(MI_INFO, "STORAGE", "SD CARD");
     mi_add(MI_INFO, "IMAGES LIVE ON THE MICRO-SD CARD", "");
     mi_add(MI_INFO, "", "");
diff --git a/boards/a2mega/src/a2fpga_esp32/telnetd.c b/boards/a2mega/src/a2fpga_esp32/telnetd.c
index a482b658..23de6497 100644
--- a/boards/a2mega/src/a2fpga_esp32/telnetd.c
+++ b/boards/a2mega/src/a2fpga_esp32/telnetd.c
@@ -23,6 +23,7 @@
  * On 1.0a3 this is the PRIMARY console: the USB-C port faces the DP
  *  monitor, so there is no PC serial link while the board is in service.
  */
+#include <stdarg.h>
 #include <stdbool.h>
 #include <stdint.h>
 #include <stdio.h>
@@ -36,6 +37,8 @@
 #include "lwip/netif.h"
 
 #include "fpga_screen.h"
+#include "fpga_link.h"
+#include "gs_socket.h"
 #include "fpga_jtag.h"
 #include "osd_console.h"
 #include "menu.h"
@@ -99,6 +102,267 @@ static void tn_puts(int fd, const char *s)
     tn_send(fd, s, (int)strlen(s));
 }
 
+static void tn_printf(int fd, const char *fmt, ...)
+{
+    char buf[200];
+    va_list ap;
+    va_start(ap, fmt);
+    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
+    va_end(ap);
+    if (n > 0)
+        tn_send(fd, buf, n < (int)sizeof(buf) ? n : (int)sizeof(buf) - 1);
+}
+
+/* ---- line mode (':' key): CLI-style register commands over telnet ---------
+ * The USB CLI lives in the sketch and prints to Serial; this is the subset
+ * the bench needs when the USB-C port is busy being the DP output:
+ *   spireg <reg> [val]      raw FPGA register read/write (dec or 0x..)
+ *   gs                      decoded GS-socket window (regs 0x5F/0x4F)
+ *   gs set <idx> <val>      write one window register
+ *   gs arm|listen|off|clear|freeze|run
+ *   gs trace                the 64-cycle bus trace (freezes it first)
+ *   gs mode <0-3> | gs hold <ms>   bring-up mode / timed hold (gs_socket.h)
+ *   gs events | gs evclear  the FPGA bring-up event log (window regs 32-39)
+ * Empty line or ESC leaves line mode. */
+#define GS_REG_SEL   0x5F
+#define GS_REG_DATA  0x4F
+
+static uint8_t gs_rd(uint8_t idx)
+{
+    uint8_t v;
+    fpga_link_lock();
+    fpga_reg_write(GS_REG_SEL, idx);
+    v = fpga_reg_read(GS_REG_DATA);
+    fpga_link_unlock();
+    return v;
+}
+
+static void gs_wr(uint8_t idx, uint8_t val)
+{
+    fpga_link_lock();
+    fpga_reg_write(GS_REG_SEL, idx);
+    fpga_reg_write(GS_REG_DATA, val);
+    fpga_link_unlock();
+}
+
+static const char *gs_mode_name(unsigned m);
+
+static void gs_dump(int fd)
+{
+    uint8_t r[32];
+    fpga_link_lock();
+    for (int i = 0; i < 32; i++) {
+        fpga_reg_write(GS_REG_SEL, (uint8_t)i);
+        r[i] = fpga_reg_read(GS_REG_DATA);
+    }
+    fpga_link_unlock();
+    #define U16(lo) ((unsigned)r[lo] | ((unsigned)r[(lo) + 1] << 8))
+    uint32_t cyc = (uint32_t)r[4] | ((uint32_t)r[5] << 8) | ((uint32_t)r[6] << 16) | ((uint32_t)r[7] << 24);
+    unsigned per = U16(16), hi = U16(18);
+    tn_printf(fd, "CTRL=0x%02X STATUS=0x%02X ph2_alive=%u running=%u enabled=%u be_ok=%u res_n=%u rdy=%u slot_dma_n=%u slot_rdy_n=%u\r\n",
+              r[0], r[1], (r[1] >> 7) & 1, (r[1] >> 6) & 1, (r[1] >> 5) & 1, (r[1] >> 4) & 1,
+              (r[1] >> 3) & 1, (r[1] >> 2) & 1, (r[1] >> 1) & 1, r[1] & 1);
+    if (per) {
+        /* sequencer clock 110 MHz: 9.091 ns per clk; counters are per 256 cycles */
+        unsigned per_ns10 = (unsigned)((uint64_t)per * 9091 / 256 / 100);   /* 0.1 ns units */
+        unsigned hi_ns10  = (unsigned)((uint64_t)hi  * 9091 / 256 / 100);
+        unsigned khz = (unsigned)(256000000ULL * 1000 / ((uint64_t)per * 9091));
+        tn_printf(fd, "PHI2: %u.%u ns/cycle (%u.%03u MHz), high %u.%u ns (%u%%)\r\n",
+                  per_ns10 / 10, per_ns10 % 10, khz / 1000, khz % 1000,
+                  hi_ns10 / 10, hi_ns10 % 10, per ? hi * 100 / per : 0);
+    }
+    tn_printf(fd, "cycles=%lu stalls=%u be_low_clks=%u hold_mismatch=%u hold_samples=%u out_extra=%u hold_tap=%u\r\n",
+              (unsigned long)cyc, U16(8), U16(10), U16(12), U16(14), r[2], r[3]);
+    tn_printf(fd, "last_addr=%02X:%02X%02X trace: %s%s wptr=%u trig_en=%u\r\n",
+              r[22], r[21], r[20], (r[23] & 0x80) ? "FROZEN" : "running",
+              (r[23] & 0x40) ? " TRIGGERED" : "", r[23] & 0x3F, (r[0] >> 4) & 1);
+    {
+        uint8_t st07 = fpga_reg_read(0x07), rst = fpga_reg_read(0x2E);
+        tn_printf(fd, "%s | a2: clock %s, reset_n=%u | 0x2E=%02X hold=%u por_done=%u (release=%u assert=%u probe=%u autoarm=%u early=%u) | mode %u %s, timed hold %u ms\r\n",
+                  gs_socket_state_str(), (st07 & 0x80) ? "RUNNING" : "STOPPED", (st07 >> 2) & 1, rst,
+                  (rst >> 4) & 1, (rst >> 5) & 1, rst & 1, (rst >> 1) & 1, (rst >> 2) & 1, (rst >> 3) & 1, (rst >> 6) & 1,
+                  gs_socket_get_mode(), gs_mode_name(gs_socket_get_mode()), gs_socket_get_hold_ms());
+        if (gs_socket_get_autotrig()) tn_puts(fd, "autotrig ON\r\n");
+        if (gs_socket_get_late_ms()) tn_printf(fd, "mode 2 late arm = %u ms\r\n", gs_socket_get_late_ms());
+    }
+    #undef U16
+}
+
+static void gs_trace(int fd)
+{
+    uint8_t ctrl = gs_rd(0);
+    if (!(ctrl & 0x08)) {
+        gs_wr(0, ctrl | 0x08);             /* freeze: the ring stops moving */
+        tn_puts(fd, "trace frozen (gs run to release)\r\n");
+    }
+    uint8_t st = gs_rd(23);
+    unsigned wptr = st & 0x3F;
+    tn_printf(fd, "%-3s %-9s %-4s %s   (oldest first; wptr=%u%s; OP=opcode fetch dat=data opr=operand int=internal)\r\n",
+              "#", "bank:addr", "data", "flags", wptr, (st & 0x40) ? "; TRIGGERED" : "");
+    fpga_link_lock();
+    for (unsigned n = 0; n < 64; n++) {
+        unsigned idx = (wptr + n) & 0x3F;  /* wptr = next write = oldest entry */
+        fpga_reg_write(GS_REG_SEL, 23);
+        fpga_reg_write(GS_REG_DATA, (uint8_t)idx);
+        uint8_t b[5];
+        for (int k = 0; k < 5; k++) {
+            fpga_reg_write(GS_REG_SEL, (uint8_t)(24 + k));
+            b[k] = fpga_reg_read(GS_REG_DATA);
+        }
+        fpga_link_unlock();
+        /* flags: bit0 R/W, bit1 RDY at the fall, bit2 BE ok, bit3 VPA, bit4 VDA */
+        const char *kind = (b[4] & 0x18) == 0x18 ? "OP " : (b[4] & 0x10) ? "dat" : (b[4] & 0x08) ? "opr" : "int";
+        tn_printf(fd, "%02u  %02X:%02X%02X   %02X   %c %s%s%s\r\n", n, b[2], b[1], b[0], b[3],
+                  (b[4] & 1) ? 'R' : 'W', kind, (b[4] & 2) ? "" : " STALL", (b[4] & 4) ? "" : " BE0");
+        fpga_link_lock();
+    }
+    fpga_link_unlock();
+}
+
+static const char *gs_mode_name(unsigned m)
+{
+    static const char *n[] = { "NATURAL (FPGA arms at the machine's release)", "EARLY (armed under our hold, TWGS model)",
+                               "TIMED (release socket-off, arm on rise)", "TIMED+ (armed under hold, timed release)",
+                               "AUTO (release socket-off, arm late; clock already up at start -> at once, clock later -> after a 300 ms hold)" };
+    return m < 5 ? n[m] : "?";
+}
+
+/* FPGA bring-up event log (window regs 32-39, hdl/esp32/a2_event_log.sv) */
+static void gs_events(int fd)
+{
+    static const char *name[] = {
+        "?", "SLOT /RESET fall", "SLOT /RESET rise", "SOCKET /RES fall", "SOCKET /RES rise",
+        "our hold ON", "our hold OFF", "0x2E write", "CTRL write", "POR DONE (machine let go)",
+        "clock alive", "clock lost", "release (storage/backstop)", "MCU ready", "core RUNNING",
+        "core in reset", "trace trigger", "socket pins OURS", "socket pins released", "probe start", "hardware arm" };
+    uint8_t st = gs_rd(32);
+    unsigned n = (st & 0x80) ? 128 : (st & 0x7F);
+    tn_printf(fd, "%u events%s; t = ms from the first; ctx letters: H our hold, R released, P por_done, A clock alive, L listen, M arm, O pins ours, X core running\r\n",
+              n, (st & 0x80) ? " (LOG FULL - gs evclear)" : "");
+    uint32_t t0 = 0, tp = 0;
+    for (unsigned i = 0; i < n; i++) {
+        uint8_t b[6];
+        fpga_link_lock();
+        fpga_reg_write(GS_REG_SEL, 33);
+        fpga_reg_write(GS_REG_DATA, (uint8_t)i);
+        for (int k = 0; k < 6; k++) {
+            fpga_reg_write(GS_REG_SEL, (uint8_t)(34 + k));
+            b[k] = fpga_reg_read(GS_REG_DATA);
+        }
+        fpga_link_unlock();
+        uint32_t t = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
+        if (i == 0) { t0 = t; tp = t; }
+        uint32_t dt = t - t0, dp = t - tp;            /* wrap-safe (32-bit ticks at 54 MHz, 79 s) */
+        tp = t;
+        unsigned code = b[4], d = b[5];
+        uint32_t us = dt / 54, dus = dp / 54;
+        char ctx[10];
+        snprintf(ctx, sizeof(ctx), "%c%c%c%c%c%c%c%c", (d & 0x80) ? 'H' : '.', (d & 0x40) ? 'R' : '.', (d & 0x20) ? 'P' : '.',
+                 (d & 0x10) ? 'A' : '.', (d & 0x08) ? 'L' : '.', (d & 0x04) ? 'M' : '.', (d & 0x02) ? 'O' : '.', (d & 0x01) ? 'X' : '.');
+        if (code == 7 || code == 8)
+            tn_printf(fd, "%3u %9lu.%03lu ms (+%lu.%03lu) %-28s value 0x%02X\r\n", i, (unsigned long)(us / 1000), (unsigned long)(us % 1000),
+                      (unsigned long)(dus / 1000), (unsigned long)(dus % 1000), code < 21 ? name[code] : "?", d);
+        else
+            tn_printf(fd, "%3u %9lu.%03lu ms (+%lu.%03lu) %-28s %s\r\n", i, (unsigned long)(us / 1000), (unsigned long)(us % 1000),
+                      (unsigned long)(dus / 1000), (unsigned long)(dus % 1000), code < 21 ? name[code] : "?", ctx);
+    }
+}
+
+static bool parse_num(const char *t, unsigned *out)
+{
+    if (!t || !*t) return false;
+    char *end;
+    unsigned long v = strtoul(t, &end, 0);
+    if (*end) return false;
+    *out = (unsigned)v;
+    return true;
+}
+
+static void tn_exec_line(int fd, char *line)
+{
+    char *tok[6];
+    int nt = 0;
+    for (char *p = strtok(line, " \t"); p && nt < 6; p = strtok(NULL, " \t"))
+        tok[nt++] = p;
+    if (nt == 0)
+        return;
+    if (!strcmp(tok[0], "help") || !strcmp(tok[0], "?")) {
+        tn_puts(fd, "spireg <reg> [val] | gs | gs set <idx> <val> | gs arm|listen|off|auto|mode <0-3>|hold <ms>|autotrig on|off|clear|freeze|run|trig|untrig | gs trace | gs events|evclear\r\n");
+        return;
+    }
+    if (!fpga_link_ok()) {
+        tn_puts(fd, "fpga link not up\r\n");
+        return;
+    }
+    if (!strcmp(tok[0], "spireg")) {
+        unsigned reg, val;
+        if (nt < 2 || !parse_num(tok[1], &reg) || reg > 126) {
+            tn_puts(fd, "usage: spireg <reg 0..126> [val]\r\n");
+        } else if (nt == 2) {
+            tn_printf(fd, "reg[0x%02X] -> 0x%02X\r\n", reg, fpga_reg_read((uint8_t)reg));
+        } else if (!parse_num(tok[2], &val) || val > 255) {
+            tn_puts(fd, "spireg: invalid value\r\n");
+        } else {
+            fpga_reg_write((uint8_t)reg, (uint8_t)val);
+            tn_printf(fd, "reg[0x%02X] <= 0x%02X\r\n", reg, val);
+        }
+        return;
+    }
+    if (!strcmp(tok[0], "gs")) {
+        unsigned idx, val;
+        if (nt == 1) {
+            gs_dump(fd);
+        } else if (!strcmp(tok[1], "set") && nt == 4 && parse_num(tok[2], &idx) && idx < 64 && parse_num(tok[3], &val) && val < 256) {
+            if (idx == 0) gs_socket_manual();
+            gs_wr((uint8_t)idx, (uint8_t)val);
+            tn_printf(fd, "gs[%u] <= 0x%02X\r\n", idx, val);
+        } else if (!strcmp(tok[1], "arm")) {
+            gs_socket_manual();
+            gs_wr(0, 0x05); tn_puts(fd, "CTRL=0x05 (arm+listen)\r\n");
+        } else if (!strcmp(tok[1], "listen")) {
+            gs_socket_manual();
+            gs_wr(0, 0x04); tn_puts(fd, "CTRL=0x04 (listen)\r\n");
+        } else if (!strcmp(tok[1], "off")) {
+            gs_socket_manual();
+            gs_wr(0, 0x00); tn_puts(fd, "CTRL=0x00\r\n");
+        } else if (!strcmp(tok[1], "clear")) {
+            uint8_t c = gs_rd(0); gs_wr(0, c | 0x80); gs_wr(0, c & 0x7F); tn_puts(fd, "counters cleared\r\n");
+        } else if (!strcmp(tok[1], "freeze")) {
+            gs_wr(0, gs_rd(0) | 0x08); tn_puts(fd, "trace frozen\r\n");
+        } else if (!strcmp(tok[1], "run")) {
+            gs_wr(0, gs_rd(0) & ~0x08); tn_puts(fd, "trace running\r\n");
+        } else if (!strcmp(tok[1], "trig")) {
+            /* arm the runaway trigger: clear pulse with trig_en set, freeze off */
+            uint8_t c = (gs_rd(0) & ~0x08) | 0x10;
+            gs_wr(0, c | 0x80); gs_wr(0, c);
+            tn_puts(fd, "trigger armed (opcode fetch from bank 0 < $0800; +32 cycles then freeze)\r\n");
+        } else if (!strcmp(tok[1], "untrig")) {
+            gs_wr(0, gs_rd(0) & ~0x10); tn_puts(fd, "trigger off\r\n");
+        } else if (!strcmp(tok[1], "hold") && nt == 3 && parse_num(tok[2], &val)) {
+            gs_socket_set_hold_ms(val); tn_printf(fd, "timed hold = %u ms (modes 2/3)\r\n", val);
+        } else if (!strcmp(tok[1], "mode") && nt == 3 && parse_num(tok[2], &val) && val < 5) {
+            gs_socket_set_mode(val); tn_printf(fd, "bring-up mode = %u (%s); takes effect at the next machine-off -> clock-up\r\n", val, gs_mode_name(val));
+        } else if (!strcmp(tok[1], "late") && nt == 3 && parse_num(tok[2], &val)) {
+            gs_socket_set_late_ms(val); tn_printf(fd, "mode 2: arm %u ms after the slot reset reads high\r\n", val);
+        } else if (!strcmp(tok[1], "autotrig") && nt == 3) {
+            bool on = !strcmp(tok[2], "on") || !strcmp(tok[2], "1");
+            gs_socket_set_autotrig(on); tn_printf(fd, "autotrig %s: the /RES-fall trace trigger is armed 20 ms after the socket inputs come on\r\n", on ? "ON" : "OFF");
+        } else if (!strcmp(tok[1], "events")) {
+            gs_events(fd);
+        } else if (!strcmp(tok[1], "evclear")) {
+            gs_wr(32, 0x80); tn_puts(fd, "event log cleared\r\n");
+        } else if (!strcmp(tok[1], "auto")) {
+            gs_socket_resume(); tn_puts(fd, "auto-arm resumed\r\n");
+        } else if (!strcmp(tok[1], "trace")) {
+            gs_trace(fd);
+        } else {
+            tn_puts(fd, "usage: gs | gs set <idx> <val> | gs arm|listen|off|auto|mode <0-3>|hold <ms>|clear|freeze|run|trig|untrig | gs trace | gs events|evclear\r\n");
+        }
+        return;
+    }
+    tn_printf(fd, "unknown: %s (help)\r\n", tok[0]);
+}
+
 /* Render one 40-char row of Apple II screen codes as ANSI. Inverse video
  * is codes $00-$3F ($00-$1F = '@'+c, $20-$3F = c); normal is ASCII+0x80. */
 static void render_row(int fd, const uint8_t *row)
@@ -176,12 +440,15 @@ static void session(int fd)
 #if A2MEGA_HAS_DP130
                 " n=dp130"
 #endif
-                " q=quit\r\n"
+                " :=cmdline q=quit\r\n"
                 "menu: up/down move, left/right change, enter/a=ok,\r\n"
                 "      esc/backspace/b=back, y=view, s/tab=select\r\n\r\n");
 
     bool menu_mode = false;
     int esc_st = 0, iac_st = 0;
+    bool line_mode = false;                /* ':' command line (tn_exec_line) */
+    char line[96];
+    int line_len = 0;
     uint32_t last_paint = 0;
 
     /* start in console mode: replay the on-screen backlog */
@@ -226,6 +493,38 @@ static void session(int fd)
                 iac_st = 1;
                 continue;
             }
+            if (line_mode) {
+                if (ch == 0x1b) {              /* ESC: leave line mode */
+                    line_mode = false;
+                    tn_puts(fd, "\r\n");
+                } else if (ch == '\r' || ch == '\n') {
+                    if (ch == '\n' && line_len == 0 && !line_mode)
+                        continue;
+                    tn_puts(fd, "\r\n");
+                    line[line_len] = 0;
+                    if (line_len == 0) {
+                        line_mode = false;     /* empty line: back to keys */
+                    } else {
+                        tn_exec_line(fd, line);
+                        line_len = 0;
+                        tn_puts(fd, ": ");
+                    }
+                } else if (ch == 0x7f || ch == 0x08) {
+                    if (line_len > 0) { line_len--; tn_puts(fd, "\b \b"); }
+                } else if (ch >= 0x20 && ch < 0x7f && line_len < (int)sizeof(line) - 1) {
+                    line[line_len++] = (char)ch;
+                    tn_send(fd, &ch, 1);
+                } else if (ch == 0) {
+                    /* telnet CR NUL */
+                }
+                continue;
+            }
+            if (esc_st == 0 && ch == ':' && !menu_mode) {
+                line_mode = true;
+                line_len = 0;
+                tn_puts(fd, "\r\n: ");
+                continue;
+            }
             if (esc_st == 0 && ch == 'q')
                 return;
             if (esc_st == 0 && ch == 'c' && menu_mode) {
diff --git a/boards/a2mega/src/a2fpga_esp32/a2fpga_esp32.ino b/boards/a2mega/src/a2fpga_esp32/a2fpga_esp32.ino
index 25111d73..8d3c1a66 100644
--- a/boards/a2mega/src/a2fpga_esp32/a2fpga_esp32.ino
+++ b/boards/a2mega/src/a2fpga_esp32/a2fpga_esp32.ino
@@ -49,6 +49,7 @@
@@ -1044,6 +1045,7 @@ static void load_wifi_credentials() {
@@ -1190,7 +1192,7 @@ volatile bool viddbg_request = false;   // telnet 'z' -> serviced in loop()
 extern "C" void viddbg_log(void)
-    uint8_t v[9], pf[6], st2 = 0;   // pf: 0x3C,0x3D,0x3E,0x5F,0x2F,0x4F
+    uint8_t v[9], pf[6], st2 = 0;   // pf: DOC REQ lo/hi, MEMRD lo/hi, LATE lo/hi
@@ -1204,8 +1206,15 @@ extern "C" void viddbg_log(void)
-    { const uint8_t pfr[6] = {0x3C, 0x3D, 0x3E, 0x5F, 0x2F, 0x4F};
-      for (int i = 0; i < 6; i++) a2spi_reg_read_status(pfr[i], &pf[i], &st2); }
+     * 0x5F, data 0x4F; indices 40-45 = REQ lo/hi, MEMRD lo/hi, LATE lo/hi;
+     * 32-39 is the socket module's event log).
+     * The plain registers it used until 2026-10-22 (0x3C-0x3F, 0x2F, 0x4F,
+     * 0x5F) are the DP readback and the GS-socket window on the a2mega. */
+        fpga_reg_write(0x5F, (uint8_t)(40 + i));
+        a2spi_reg_read_status(0x4F, &pf[i], &st2);
```

### A10. ESP32 firmware disk.c / Makefile vs PR #34

```diff
diff --git a/boards/a2mega/src/a2fpga_esp32/Makefile b/boards/a2mega/src/a2fpga_esp32/Makefile
index b6cad0f3..9ff844ad 100644
--- a/boards/a2mega/src/a2fpga_esp32/Makefile
+++ b/boards/a2mega/src/a2fpga_esp32/Makefile
@@ -1,9 +1,11 @@
 # ESP32-S3 Arduino CLI Makefile
-# A2FPGA ESP32 co-processor firmware (a2mega board)
+# A2FPGA ESP32 co-processor firmware (a2mega board; also the a2p25
+# "displayport1" rev — see boards/a2p25/docs/ESP32_FW_A2P25.md)
 
 # Module varies by board revision (BOARD_REV below):
 #   1.0a3   ESP32-S3-MINI-1-N4R2 — 4 MB flash + 2 MB quad PSRAM
 #   1.0a2x  ESP32-S3-MINI-1-N8   — 8 MB flash, no PSRAM
+#   a2p25   ESP32-S3-MINI-1-N4R2 (BOARD_REV=225, same settings as 1.0a3)
 BOARD = esp32s3
 PORT ?= /dev/cu.usbmodem*
 BAUD = 115200
@@ -13,12 +15,24 @@ PROJECT_NAME = a2fpga_esp32
 ARDUINO_CLI = arduino-cli
 
 # Board revision (see board_pins.h): 103 = 1.0a3 (USB-C DP, N4R2 module),
-# 102 = 1.0a2/1.0a2a (N8 module). Set BOARD_REV=102 to build for old boards.
+# 102 = 1.0a2/1.0a2a (N8 module), 225 = a2p25 displayport1 (N4R2).
+# Set BOARD_REV=102 to build for old boards.
+# a2p25 only: DP_TEST=1 (default) builds the dp_test-bitstream variant
+# (IO10 = HPD to FPGA ball B2, UART1 RX on IO11 = FPGA telemetry, no OSPI);
+# DP_TEST=0 builds for an OSPI-speaking core.
+# NOTE: all revs share ./build; arduino-cli rebuilds fully when the flags
+# change, but run 'make clean' if in doubt before switching BOARD_REV.
 BOARD_REV ?= 103
+DP_TEST ?= 1
 ifeq ($(BOARD_REV),102)
 FQBN = esp32:esp32:$(BOARD):USBMode=hwcdc,CDCOnBoot=cdc,CPUFreq=240,FlashSize=8M,PSRAM=disabled,PartitionScheme=default_8MB
 REV_FLAGS = --build-property compiler.c.extra_flags=-DA2MEGA_BOARD_REV=102 \
             --build-property compiler.cpp.extra_flags=-DA2MEGA_BOARD_REV=102
+else ifeq ($(BOARD_REV),225)
+# a2p25 displayport1: same N4R2 module + partition rationale as 1.0a3 below.
+FQBN = esp32:esp32:$(BOARD):USBMode=hwcdc,CDCOnBoot=cdc,CPUFreq=240,FlashSize=4M,PSRAM=enabled,PartitionScheme=default
+REV_FLAGS = --build-property "compiler.c.extra_flags=-DA2MEGA_BOARD_REV=225 -DA2P25_DP_TEST=$(DP_TEST)" \
+            --build-property "compiler.cpp.extra_flags=-DA2MEGA_BOARD_REV=225 -DA2P25_DP_TEST=$(DP_TEST)"
 else
 # no_ota partitions: 2 MB app + 1.875 MB "spiffs" (LittleFS) — the largest
 # FS a 4 MB part allows with app headroom. No OTA pair: the ESP32 reflashes
@@ -27,23 +41,23 @@ else
 # PC-driven JTAG via the bridge is unaffected. The board footprint accepts
 # either MINI variant — an N8-fitted 1.0a3 builds with BOARD_REV=103 plus
 # the 102 flash settings if that ever comes up.
-FQBN = esp32:esp32:$(BOARD):USBMode=hwcdc,CDCOnBoot=cdc,CPUFreq=240,FlashSize=4M,PSRAM=enabled,PartitionScheme=no_ota
+FQBN = esp32:esp32:$(BOARD):USBMode=hwcdc,CDCOnBoot=cdc,CPUFreq=240,FlashSize=4M,PSRAM=enabled,PartitionScheme=default
 REV_FLAGS =
 endif
 
 # Source files
 SKETCH = a2fpga_esp32.ino
-CPP_FILES = a2fpga_jtag.cpp usbc_glue.cpp
-C_FILES = a2fpga_ospi_link.c a2fpga_spi_service.c fpga_link.c fpga_screen.c wedge_watch.c \
+CPP_FILES = a2fpga_jtag.cpp usbc_glue.cpp usbhost.cpp usbstorage.cpp
+C_FILES = fwupdate.c a2fpga_ospi_link.c a2fpga_spi_service.c fpga_link.c fpga_screen.c wedge_watch.c \
           osd_console.c menu.c settings.c disk.c gcr_dsk.c w5100.c \
           wifi_bridge.c fpga_jtag.c fpgaupdate.c fpgastream.c ftpd.c telnetd.c \
-          fusb302.c usbc_port.c esp32s3_integration.c gs_socket.c
+          fusb302.c usbc_port.c esp32s3_integration.c sn75dp130.c gs_socket.c
 HEADER_FILES = a2fpga_jtag.h a2fpga_ospi_link.h a2fpga_spi_service.h wedge_watch.h \
                a2fpga_regs.h fpga_link.h fpga_screen.h osd_console.h menu.h \
                net_status.h settings.h disk.h gcr_dsk.h w5100.h \
                wifi_bridge.h fpga_jtag.h fpgaupdate.h fpgastream.h ftpd.h telnetd.h \
                board_pins.h fusb302.h usb_pd.h usbc_port.h usbc_glue.h \
-               esp32s3_integration.h gs_socket.h
+               esp32s3_integration.h sn75dp130.h usbhost.h usbstorage.h fwupdate.h gs_socket.h
 ALL_SOURCES = $(SKETCH) $(CPP_FILES) $(C_FILES) $(HEADER_FILES)
 
 # Default target
@@ -129,5 +143,7 @@ help:
 	@echo "Usage examples:"
 	@echo "  make init           # First time setup"
 	@echo "  make compile        # Build only"
+	@echo "  make BOARD_REV=225 compile          # a2p25 displayport1 (dp_test mode)"
+	@echo "  make BOARD_REV=225 DP_TEST=0 compile # a2p25, OSPI core"
 	@echo "  make PORT=/dev/ttyUSB0 upload  # Upload to specific port"
 	@echo "  make flash          # Upload and monitor"
diff --git a/boards/a2mega/src/a2fpga_esp32/disk.c b/boards/a2mega/src/a2fpga_esp32/disk.c
index 779061d7..08f01c82 100644
--- a/boards/a2mega/src/a2fpga_esp32/disk.c
+++ b/boards/a2mega/src/a2fpga_esp32/disk.c
@@ -38,6 +38,10 @@
 #include "a2fpga_regs.h"
 #include "fpga_link.h"
 #include "gcr_dsk.h"      /* on-the-fly .dsk/.do <-> 6-and-2 GCR nibble codec */
+
+/* .ino: releases the ESP32-driven Apple /RES (no-op where the FPGA owns it) */
+void a2_reset_hold_release(const char *why);
+#include "usbstorage.h"   /* USB stick as the image volume (backend switch) */
 #include "settings.h"     /* persisted image overrides + slot map */
 #include "disk.h"
 #include "gs_socket.h"
@@ -452,12 +456,10 @@ void disk_request_remount(void)
  * shown while we look for the image(s); on a successful mount we hide it so the
  * screen returns to the Apple II, and on failure we leave it up so the message
  * can be read. */
-static void disk_remount(void)
+/* Close every image and clear the FPGA mounted/ready flags (the disk task
+ * owns these files; the storage backend may be swapped afterwards). */
+static void disk_unmount_all(void)
 {
-    osd_console_show();
-    DLOGI("DISK II: SEARCHING FOR STORAGE...");
-
-    /* Tear down the previous mount. */
     for (int v = 0; v < NDRV; v++) {
         if (g_img[v]) {
             fclose(g_img[v]);
@@ -476,8 +478,16 @@ static void disk_remount(void)
         g_hdd_mounted[u] = false;
         fpga_reg_write(A2REG_HDD_CTL(u), 0);
     }
+}
+
+static const char *storage_name(void) { return usbstorage_is_usb() ? "USB STICK" : "FLASH FS"; }
 
-    /* SD card present? The VFS mount is owned by the integrator; probe it. */
+static void disk_remount(void)
+{
+    osd_console_show();
+    DLOGI("DISK II: SEARCHING FOR STORAGE...");
+    disk_unmount_all();
+    /* Volume present? The VFS mount is owned by the integrator; probe it. */
     DIR *root = opendir(SD_ROOT);
     if (!root) {
         DLOGW("DISK II: NO SD CARD (%s NOT MOUNTED)", SD_ROOT);
@@ -485,7 +495,7 @@ static void disk_remount(void)
         return;   /* leave the console up so the message is visible */
     }
     closedir(root);
-    DLOGI("DISK II: SD CARD");
+    DLOGI("DISK II: %s", storage_name());
 
     int n_mounted = 0;
     for (int v = 0; v < NDRV; v++) {
@@ -515,7 +525,7 @@ static void disk_remount(void)
          * seeks during the delay (observed on the BL616 as a ~1.54 s gap). */
         g_hide_console_at_us = esp_timer_get_time() + 1500000;
     } else {
-        DLOGI("DISK II: NO DISK IMAGES ON SD CARD");
+        DLOGI("DISK II: NO DISK IMAGES ON %s", storage_name());
         /* leave the console up so the message is visible */
     }
 }
@@ -719,6 +729,18 @@ static void apply_slot_map(void)
 
 void disk_poll(void)
 {
+    /* Storage backend change (USB stick attached/removed, reported by
+     * usbhost): close the images first — they live on the volume about to
+     * be swapped — then let the integrator swap /sdcard, then remount. */
+    {
+        bool usb = false;
+        if (usbstorage_take_pending(&usb)) {
+            disk_unmount_all();
+            storage_activate(usb);
+            DLOGI("STORAGE: %s", usb ? "USB STICK" : "FLASH FS");
+            g_remount_req = true;
+        }
+    }
     if (g_remount_req) {
         g_remount_req = false;
         g_remounting  = true;
@@ -744,8 +766,12 @@ void disk_poll(void)
             bool any = false;
             for (int v = 0; v < NDRV; v++) any = any || g_mounted[v];
             for (int u = 0; u < NHDD; u++) any = any || g_hdd_mounted[u];
+            /* 10 s (was 7): the USB host role decision takes 5 s and a stick
+             * needs a moment to enumerate and mount before the Apple scans.
+             * gs_socket_ready(): the in-socket 65C816 auto-arm must have
+             * settled before /RES rises (bench G30; PR #34). */
             if ((any && !g_remount_req && gs_socket_ready()) ||
-                esp_timer_get_time() > 7000000) {
+                esp_timer_get_time() > 10000000) {
                 /* Program the slot map JUST before the release — this late in
                  * boot the link is proven good (the mounts above ran over it),
                  * and the Apple II is still held in reset so the reconfig is
@@ -753,6 +779,8 @@ void disk_poll(void)
                 apply_slot_map();
 
                 gs_socket_a2_release();              /* 0x2E.0, via the socket module (keeps its assert bit) */
+                /* boards where the ESP32 owns the /RES pin (a2p25 IO33) */
+                a2_reset_hold_release(any ? "media mounted" : "no media");
                 s_released = true;
                 DLOGI("A2: RESET RELEASED%s", any ? "" : " (NO MEDIA)");
             }
@@ -852,7 +880,7 @@ void disk_get_hdd_info(int u, disk_info_t *out)
 
 bool disk_backend_is_usb(void)
 {
-    return false;   /* SD card is the only storage backend on the a2mega */
+    return usbstorage_is_usb();
 }
 
 bool disk_remount_pending(void)
```

## 4. Post-merge builds (2026-10-22/23) — all three clean after four fix commits

Built from this worktree (same tree as main 9332dab5 plus the fixes below),
Gowin V1.9.12.03 (gw_sh pipe method), gw_sh pipe method via tools/build.sh. Gowin IDE was open throughout
(license contention is the likely reason one 138B run hit the 45-min cap).

### Fix commits needed to build (on branch `claude/unmerged-worktrees-branches-0abece`, NOT yet on main)

| commit | what | why |
|--------|------|-----|
| 47c40112 | aux_channel.v: drop the direct `err_from/err_exp/err_rxc` writes at the four a2p25 observe-mode error sites | round 2 (PR #25) had deleted those registers for a centrally captured delayed copy; the a2p25 sites were added after and merged without a textual conflict → synthesis error in all three cores |
| 35aa83ba | a2p25.gprj lists `a2_event_log.sv` | PR #34 added the module to the shared connector and only the a2mega project files |
| 06bd70a2 | connector `EVT_LOG` parameter; a2mega top = 1 under `GS_SOCKET`, 0 otherwise; a2p25 = 0 | PR #34's 128×48 event log synthesised as ~5.8k flops + 3.3k LUTs in every core: 60K CLS 67%→84% and the first roll scattered the raster counter (clk_pix −1.09 ns); the GW5A-25 **failed to place** (2625 regs unplaced) |
| 759c3f53 | a2_event_log: `syn_ramstyle = "block_ram"` | same log on the 138B: registers 23.8k→31.4k and round 2's closed cones reopened (clk100 16 endpoints −0.62, clk_sym 7 −0.60, clk_pix 2 −0.45) with none of those modules changed since round 2; now 31 regs + 2 BSRAM |

### Final results (0 setup / 0 hold on all three)

a2mega reported slack = real slack − 0.5 ns on clk100/clk_sym/clk_pix/clk_logic
(and clk_gs on the 138B) from `set_clock_uncertainty`; the a2p25 SDC has no
margin, its slack is real.

**a2mega 60K** (a2mega.gprj) — 4 min — fs sha256 `f28e2e060492a86b524ff13a717034b8d450e7734cc3a159f6cfc80451a2aa03`
Logic 20792/59904 (35%), Reg 21416 (36%), CLS 67%, BSRAM 107/118.
| clock | constraint | Fmax | worst reported slack |
|---|---|---|---|
| clk100 | 100.000 | 105.317 | +0.505 (aux_channel train_inte→eq_dwell) |
| clk_sym | 135.007 | 142.374 | +0.383 (packer load0_r→data) |
| clk_pix | 148.500 | 176.717 | (no path within 25 listed) |
| clk_logic | 54.000 | 65.789 | +0.044 (framebuffer wr_accum→wr_fifo DI) |
| clk1x | 80.998 | 104.527 | +0.121 (vendor DDR3 PHY) |

**a2mega 138B** (a2mega_138B.gprj, GS_SOCKET) — 69 min — fs sha256 `4d799678136ccb42e3663f916db160edc501a7107561a8f9552aa29dc70cf34d`
Logic 29060/138240 (22%), Reg 25611 (19%), CLS 38%, BSRAM 134/340.
| clock | constraint | Fmax | worst reported slack |
|---|---|---|---|
| clk100 | 100.000 | 100.068 | **+0.007** (aux_channel expected→next_state) — at the +0.5 real policy line, ladder-limited as in round 2 |
| clk_sym | 135.007 | 135.927 | +0.050 (pixel_cdc_fifo rptr_bin→rlevel) |
| clk_pix | 148.500 | 149.686 | +0.053 (debug_overlay relx→sA_in_space) |
| clk_logic | 56.252 | 62.436 | +0.201 |
| clk1x | 80.998 | 115.531 | +0.141 (vendor DDR3 PHY) |
| clk_gs | 109.999 | 147.860 | — |

**a2p25** (a2p25.gprj, GW5A-25) — 4 min — fs sha256 `ce1a6b66efbf8e1fe6db06509569feefca435f2faa05cf80d6847081bc328dae`
Logic 15369/23040 (67%), Reg 13274 (58%), **CLS 11172/11520 (97%)**, BSRAM 52/56.
| clock | constraint | Fmax | worst slack (real) |
|---|---|---|---|
| clk100 | 100.000 | 103.906 | +0.376 (aux_channel train_inte→eq_dwell) |
| clk_sym | 80.998 | 109.069 | +0.240 (pixel_cdc_fifo rptr_bin→mem) |
| clk_pix | 148.500 | 166.061 | +0.201 (dp_video_timing fifo_wdata→cdc mem) |
| clk_hr | 53.999 | 64.688 | +0.201 (framebuffer wr_accum→wr_fifo) |
| clk_mem | 108.003 | 123.954 | +0.823 |
| clk_tx | 101.245 | 106.894 | +0.522 |

Single rolls each (no durability rolls yet). The 60K's earlier −1.09 clk_pix roll
had the event log in it; its three clk1x hold endpoints (−0.014 worst) were the
known vendor DDR3 PHY hold paths and did not recur.

### Not done / next
- Fix commits are not on main; bitstreams and reports are rebuilt (uncommitted)
  in this worktree's impl/ trees.
- Nothing flashed. Hardware checks from §3 remain (SHR left edge for
  H_TRIGGER_LEAD 12, vertical position, telnet `z` DOC line via window 40–45,
  OSD CORE date at 0x7E, census `d`, PR #34 auto-arm on the 138B).
- The a2p25 is at 97% CLS: the next feature will need the RESOURCE_AUDIT's
  bloat proposals.

**Landed 2026-10-23:** main fast-forwarded to 759c3f53 (the four fix commits) and pushed. Bitstreams left uncommitted in this worktree.

## 5. Verification pass (2026-10-23) — firmware, sims, the other four boards

### ESP32 firmware — compiles
Makefile `compile` target, BOARD_REV=103 (a2mega 1.0a3): 1,131,515 B (86%) of
program space, 107,064 B RAM. BOARD_REV=225 DP_TEST=0 (a2p25 OSPI core):
1,124,507 B (85%), 106,968 B. Deploy together with the new gateware (register remap).

### SDP engine wire-mapping testbench (`tb_sdp_4lane.v`, iverilog)
| engine version | 1 lane | 2 lanes | 4 lanes |
|---|---|---|---|
| merged main | FAIL (52 byte errors) | PASS | PASS |
| round 2 (epic-merge-prep) | FAIL (52) | PASS | no 4-lane support |
| a2p25 tip | FAIL (52) | PASS | PASS |
| base 9d95ad1d / main before merge | FAIL (52) | PASS | no 4-lane support |
The 1-lane failure is identical in every version (pre-existing; nothing ships
1-lane). The merged engine is the only one passing both 2 and 4 lanes.

### Full-chain DP frame + audio harness (`run_full_chain.sh both`, 720p60 RBR 2-lane, 48 kHz)
SCHED_ROM=0 and SCHED_ROM=1 (the a2mega core's packer): packer ready after
1407 cycles, no FIFO underrun over 3,037,500 cycles, frame reconstruction
720 rows pixel-exact, audio: 2 timestamp + 401 stream (1604 samples) + 2
infoframe SDPs, ALL AUDIO CHECKS PASSED. `check_dp_frame` also reports 6748
"Maud byte 06B (want 000)" errors: its `MAUD_BYTE_EXP` is hard-coded to 0
(no audio) while the testbench runs with audio, so every raster line trips
it. Pre-merge baseline run: identical verdict on the pre-merge tree (77bbb122) — packer ready 1407, no underrun, 720 rows pixel-exact, the same 6748 Maud-byte complaints, ALL AUDIO CHECKS PASSED. The merge changed nothing the harness can see.

### Tang Nano boards (not built before this pass)
First builds at main 759c3f53: all four 0 setup / 0 hold, but BSRAM grew on
every one (Enhanced classic 44→46/46, dualrate 28→36, fb 34→40, GS 16→24).
Cause: the a2p25 work added `BANKS_IN_BSRAM` to the baseline doc5503 and wired
sound_glu's `DOC_BANKS_IN_BSRAM` (default 1 since August, previously reaching
only the pipelined DOC) through to it; the Enhanced/GS tops pass nothing.
**Fix 56eeef50** (this worktree's branch, NOT on main): sound_glu default → 0,
matching doc5503's own default; a2mega (0) and a2p25 (1) pass it explicitly.
Rebuilt with the fix, all 0/0:
| config | Logic | Reg | CLS | BSRAM | worst reported slack |
|---|---|---|---|---|---|
| Enhanced classic | 13721 (67%) | 7164 | 86% | 46/46 | clk_logic +0.081 |
| Enhanced dualrate | 14839 (72%) | 8043 | 87% | 31/46 | clk_logic +0.074, clk_sdram +0.219 |
| Enhanced fb | 15759 (76%) | 8720 | 89% | 35/46 | clk_logic +0.074 |
| GS | 11617 (57%) | 6026 | 78% | 19/46 | clk_sdram +0.170 |
The classic's 46/46 is the same at the synthesis level before and after the
merge (46 both; the committed PnR report's "44" was packing). The remaining
+1..+3 PnR blocks on the others are packing too.

### Behaviour change on the Enhanced/GS audio path — YOUR DECISION
The pre-merge Enhanced fetched DOC wavetable bytes straight from SDRAM
(`sound_glu` generate branch `gen_doc_ddr3`). The a2p25 work replaced that
branch with `gen_doc_ext`: `doc_fetch_cache` (64-word cache, 1 BSRAM) in front
of the memory port plus the baseline DOC's stale-fetch filter / timeout /
hold-last-sample. It is selected by the same `USE_BSRAM=0, USE_DDR3_PIPELINED=0`
the Enhanced and GS already use, so they now run the a2p25's audio path,
verified on HyperRAM (Arkanoid LATE 0, Marble Madness no smear) but never on
SDRAM. Options: (a) accept and listen on Enhanced hardware (the overlay DOC
counters now read latmax / timeouts / filter drops); (b) resurrect
`gen_doc_ddr3` under a `DOC_FETCH_CACHE` parameter defaulting off for boards
that do not choose. I did not change anything here.

### Unit sims (2026-10-23)
`sim/doc5503/run_fetch_cache.sh`: real run 0 stale deliveries, hit rate 40%,
0 drops over 5768 consumed samples (PASS); the +noflush negative control
delivers 13 stale words as required. `sim/doc5503/run_banks_equiv.sh`: PASS on
all seeds.

**Decision (ed, 2026-10-23): keep the a2p25 DOC fetch-cache path on the
Enhanced/GS; validate by ear on hardware. sound_glu bank-default fix 56eeef50
landed on main and pushed.**

## 6. Resource report + build profiles (2026-10-24) — on branch, not on main

- `tools/resource_report.py` + `tools/build.sh` hook (runs before the timing
  verdict; `RESOURCE_BASELINE`, `RESOURCE_FAIL`); baseline snapshots for all
  seven builds committed in `boards/<board>/impl/<proj>_resources.json`.
- `boards/<board>/hdl/<board>_profile.vh` for a2mega, a2p25, Enhanced, GS,
  a2n20v2; tops read A2_FEATURE_* / A2_INSTR_* from it. Verified no behaviour
  change: six configurations 0/0 with identical module tables.
- **138B finding:** the same source rolled 22 clk100 setup violations in
  aux_channel (worst -0.92 reported) where the previous roll had +0.007. The
  AUX ladder grew with the a2p25 stack (observe mode, blind_dwell, EQ dwell,
  dp_reg_de_activ, aux_messages) and is no longer durable on the 138B under the
  three-roll / +0.5 ns rule. Needs a round-2-style structural fix (register the
  decode inputs, replicate next_state further); dp_test on the 138B is the
  cheap gate. Not started.
- Docs: `docs/build_profiles.md` (conventions, release plan, where the fabric
  is per board), index entry, tools/README, build skill step 4.
