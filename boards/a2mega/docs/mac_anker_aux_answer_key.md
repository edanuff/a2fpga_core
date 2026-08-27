# Mac ↔ Anker AUX capture: the conformance answer key

2026-08-26. AD3 on a USB-C breakout inline between a MacBook Pro and the
Anker hub (Sceptre on the hub's HDMI). CH1 = SBU1 (A8), CH2 = SBU2 (B8;
turned out flat — decode is effectively single-ended on SBU1, which is
sufficient). 30 s window, 4 MS/s, plug-in inside the window. Raw CSV =
`mac_aux_bringup.csv` (~3 GB, not committed); full annotated transcript =
`mac_anker_bringup_annotated.txt` (committed alongside this doc).

Decoder chain fixed in the same session (see tools):
- `ad3_aux_capture.py`: streams raw samples to disk (long windows no
  longer exhaust RAM); zero-fills lost samples to keep the timeline.
- `aux_decode2.py`: interpolated edge timing (was ±0.25 µs quantized =
  half a Manchester half-cell); NEW anchor-based frame parser — finds
  each frame's SYNC-END (first ≥2-run pair after a stretch of length-1
  runs) and derives polarity from the anchor itself, instead of
  pair-parsing the whole burst with one global phase. Manchester
  convention corrected ('1' = second half-cell low); the old selftest
  passed only because its synthesizer had the same inversion.
- `aux_annotate.py`: request/reply direction comes from alternation
  state (headers overlap: ACK=0x00 collides with the I2C_WR nibble),
  with an exact-length native-cmd escape hatch for lost replies.

Internal consistency checks that certify the decode: the ESI block read
at 0x2003 (13 B) shows `77 00 01 03` at offsets 0x200C-0x200F — the
exact mirror of the separately-read 0x202-0x204 lane status; the EDID
blocks reassemble into a valid Sceptre F22 EDID ("Sceptre F22" string,
checksum-plausible structure, 0x02 0x03 CTA extension).

## Timeline (capture t=0 at record start; Mac plugged in ~t=0-2 s)

- **t=11.72 s**: first AUX transaction. Everything before is macOS
  display-stack latency — the >10 s to picture is the Mac's, not the
  link's. The DP conversation itself is FAST.
- 11.72-11.83 s: capability phase (~100 ms).
- 11.86-11.90 s: EDID (both blocks, ~50 ms).
- 11.91-11.95 s: link training — **complete in ~30 ms**.
- 12.4 s onward: steady-state polling + vendor writes; HDCP at 13.8 s.

## The conversation, phase by phase

### 1. Capabilities
- `RD 0x0000 len16` → `12 14 C2 81 01 1D 01 81 02 00 04 04 3E 00 81 00`
  - DPCD rev **1.2**, max rate **0x14 = HBR2**, `0xC2` = 2 lanes +
    TPS3 + **enhanced framing**; 0x00E byte = 0x81: extended-caps
    present, EQ interval 4 ms.
- `RD 0x2200 len16` (extended caps; first byte 0x14 = rev 1.4 there).
- `RD 0x0080 len4` (sink-specific caps), `RD 0x0500` branch OUI →
  `00 10 FA` + "cHDMIb" (DP→HDMI branch identity), hw/fw rev bytes.
- **`WR 0x0300` = `00 10 FA 41 41 50 4C 00 00 10 01 00`** — the Mac
  writes its SOURCE OUI: Apple + "AAPL" + rev. Read back at 0x300 to
  verify. Branch firmware can (and often does) key quirks off this.
- Reads of 0x310, 0x400/0x403 (sink OUI), 0x503, 0x090, 0x0B0, 0x060,
  0xE000D, HDCP2.2 RxCaps region (0x6921D), 0x2210/0x2214.

### 2. Stale-IRQ servicing BEFORE anything else (the big one)
At 11.857 s, before EDID and training:
- `RD 0x0200 len6` → `41 ...` (sink count 1 + CP_READY)
- `RD 0x0202 len3` → `00 00 80` — **0x204 bit7 LINK_STATUS_UPDATED set**
- `RD 0x2003 len13` (whole ESI0 block 0x2003-0x200F) → shows
  **0x2005 = 0x02** (link-service IRQ pending) and 0x200E = 0x80.
- **`WR 0x2005 = 02`** — write-1-to-clear, ACKed.
- Subsequent status polls read `77 00 01` — **the 0x80 latch is GONE**.

The Mac services sink IRQs through the **ESI space** (0x2003/0x2005),
not legacy 0x201 — 0x201 is never touched in the whole capture. And the
0x204-bit7 latch that appears in EVERY one of our wedge snapshots (Z:)
clears exactly when the ESI vector is acknowledged. This is the answer
key confirming the unserviced-IRQ diagnosis, with the addendum that the
reference implementation clears via ESI. Our 8acfe351 build clears
legacy 0x201; if bench shows 0x204 bit7 still sticking, the move is to
mirror the Mac: read 0x2002-0x200F as a block, write-1-clear 0x2004/0x2005.

### 3. EDID — the Anker does NOT defer forever
Pattern per 16-byte block, every block, metronomic:
`I2C_RD_MOT len16` → **DEFER** → retry ~0.4 ms → **DEFER** → retry
~0.6 ms → **ACK + 16 bytes**. Segment-pointer write 0x00 first, address
write 0x80 for the extension block, closing `I2C_RD` (len 0, MOT end).
Both 128-byte blocks read in ~50 ms total.

This kills the "converters DEFER all DDC forever" belief (that
observation likely came from a monitor-absent or wedged state): with a
monitor present the Anker serves EDID after exactly two DEFERs per
transaction. A tolerant EDID read = retry on DEFER with ~0.4 ms spacing,
budget ~3-5 tries per block.

### 4. Power + link training (30 ms, first try, no ladder)
- `RD 0x600` → 01, `WR 0x600 = 01` (D0).
- `WR 0x107 = 00` (no downspread), `WR 0x100 = 0x14` (**HBR2**),
  `WR 0x101 = 02` then **`= 82`** (2 lanes + ENHANCED_FRAME_EN,
  read-back between writes).
- `WR 0x102 = 21` (TPS1 + scrambling disable) → `RD 0x202 len6` →
  CR done `11` per lane in ONE poll → `WR 0x103 = 01 01` (lane set) →
  `WR 0x102` = EQ pattern → `RD 0x202 len6` → `77 00 81 ...` EQ+align
  done → `WR 0x102 = 00` (training off).
- `RD 0x210 len8` (symbol error counters) → clean.

Notable: status reads during training use len6 (0x202-0x207 in one
transaction — status AND adjust requests together), and the pre-training
status read tolerated the regressed `00 00 80` without drama.

### 5. Steady state
Polling cluster roughly every 0.3-1 s (relaxing to ~2.8 s):
- `RD 0x600` (power), `RD 0x200 len6` (sink count + status block),
  `RD 0x202 len3`.
- **`WR 0x3050 = 01`** every cycle, plus `RD/WR 0x3051` (= 02 then 06
  pre-training, re-confirmed 06 afterwards): 0x3000-0x30FF is branch
  vendor space — a VIA/ITE vendor handshake, possibly a keepalive.
  Candidate explanation for why known-good sources never see the hub
  MCU freeze. Experiment-worthy, clearly labeled cargo-cult risk.
- Second ESI service cycle at 16.66 s (RD 0x2003 len13, WR 0x2005=02).
- HDCP 1.x auth at 13.8 s (An/Aksv writes, Bksv read, R0' read) —
  optional for us; picture does not depend on it here.

## Conformance gaps ranked (ours vs the answer key)

1. **IRQ servicing via ESI** — Mac reads the ESI block and
   write-1-clears 0x2005; never touches 0x201. Our IRQ_SERVICE=1 build
   (8acfe351) uses legacy 0x201 — bench will show whether the IT6563
   honors the legacy path (watch whether 0x204 bit7 clears after a
   J: irq_service). If not: switch clear target to 0x2004/0x2005.
2. **Source OUI write (0x300)** — we never announce ourselves; the Mac
   writes AAPL. Cheap to add; branch quirk-gating is real.
3. **EDID with DEFER-tolerant retry** — 2 DEFERs then ACK is the hub's
   documented-by-capture behavior; our blanket skip forfeits mode info
   and diverges from what the branch expects a source to do.
4. **Status reads as multi-byte blocks** (0x200 len6 / 0x202 len6) —
   we read registers singly; blocks are atomic snapshots (status +
   adjust in one transaction) and fewer AUX round-trips.
5. **Vendor 0x3050/0x3051 writes** — unknown semantics; note for a
   controlled experiment only after 1-4 are graded.
6. Enhanced framing (`0x101 = 0x82`) — verify what we set.

## Pass A2: HDMI unplug → replug (captured same session)

`mac_aux_hdmireplug.csv` → `mac_anker_hdmireplug_annotated.txt`. HDMI
was pulled before/at record start (AUX silent 0-5 s — the Mac had
already torn down on the virtualized HPD drop); replug at ~5.03 s.

- **HDMI replug = a full fresh hot-plug to the source.** The hub
  virtualizes the IT6563's DP-HPD re-assert into a complete HPD cycle,
  and the Mac re-runs its ENTIRE bring-up script from `RD 0x0000` —
  byte-identical caps phase, EDID with the same 2-DEFER cadence,
  training first-try. No incremental retrain path, no IRQ-driven
  recovery: attach handling IS the recovery mechanism.
- **Warm re-attach is fast**: first AUX → training complete in ~230 ms;
  HDCP done ~2.2 s. The 10+ s cold-plug delay is macOS display-stack
  latency, confirmed.
- **The `WR 0x2005 = 02` is UNCONDITIONAL.** Here the ESI block read
  showed 0x2005 = 00 (no IRQ pending) and the Mac wrote the clear
  anyway. It's a scripted ack in the attach sequence, not a
  conditional response to a pending vector.
- **Branch DPCD state survives the HDMI cycle**: LANE_COUNT_SET read
  back 0x82 (previous session's value) before being rewritten; 0x3051
  already 06. The hub does not reset its DPCD on HDMI unplug — matches
  the stale-but-served physiology (autonomous AUX hardware, MCU-owned
  state).

### Correction to §2 (both captures agree)
The 0x204 bit7 LINK_STATUS_UPDATED latch does NOT clear at the ESI ack:
it still reads 0x80 during training (re-set by every status change:
00→11→77) and only reads clear in steady state after training completes
— consistent with clear-on-read re-armed by changes. So the latch in
our wedge snapshots is a *status engine frozen mid-change* signature,
not directly an unserviced-IRQ flag. The ESI ack write remains part of
the reference attach script and IRQ servicing remains conformance-
required (capture 1's 0x2005=02 pending bit was real), but "clearing
0x204 bit7" is not the observable to grade IRQ servicing by.

## Still wanted

- Pass C: identical capture of OUR board's bring-up (breakout inline
  board↔hub) for a line-by-line diff against this transcript.
- Pass B: CC1/CC2 (A5/B5) PD capture; needs a BMC decoder.
