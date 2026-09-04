# A2P25 Board Rev: USB-C DisplayPort Output via Hardened MIPI D-PHY

**Status:** Proposed design for next a2p25 board rev
**Date:** 2026-09-01
**Depends on:** a2mega DP TX gateware (60K/138K SERDES implementation), a2mega USB-C alt-mode support-IC stack

## 1. Problem Statement

The a2p25 uses the Tang Primer 25K core board (GW5A-LV25MG121C1/I0). Unlike the
a2mega's GW5AT-60/GW5AST-138 SOMs, the GW5A-25 has **no SERDES transceivers**, so the
a2mega DP main-link PHY cannot be ported directly. The options considered:

1. **ANX7625** (MIPI DSI/DPI → USB-C DP alt mode, purpose-built, I2S audio in) —
   the ideal single-chip answer, but procurement is unreliable. Rejected on
   sourcing risk.
2. **SN65DSI86** (DSI → eDP) — investigated and rejected: no audio engine (its only
   SDP path is a software-paced DSI generic-long-write tunnel unsuited to
   isochronous audio), and it only supports ASSR-scrambling sinks, which many DP
   monitors/converters are not. See §10 ref [3].
3. **Soft SERDES on generic I/O** — DP RBR is 1.62 Gbps/lane; GW5A generic true-LVDS
   I/O tops out around 1.25 Gbps. A ~30% out-of-spec overclock with no dedicated
   PLL. Rejected on timing risk.
4. **Hardened MIPI D-PHY TX as a raw 1.62 Gbps serializer** + external swing
   regeneration — **selected**, detailed below.

## 2. Key Insight

Gowin's Arora V hardened D-PHY TX (UG296E) is **protocol-free**. The `MIPI_DPHY`
primitive is an 8:1/16:1 gearbox serializer plus pad drivers; every byte on the
wire — including D-PHY's own SoT/sync/EoT sequences in normal MIPI use — is
supplied by the fabric, and the low-power (LP) states are separate bit-banged pad
ports (`do_lptx*`) that we simply never drive. Hold `HSTXD_VLD` and the lane
enables high and it emits a continuous, arbitrary NRZ bitstream at up to
2.5 Gbps/lane (UG296E §4.1.2, Figure 4-1; §2.1).

Therefore the FPGA can transmit **native DisplayPort RBR**, reusing the entire
a2mega DP link layer (link training, scrambler, 8b/10b, MSA, **audio SDPs**),
with the D-PHY hard macro replacing the SERDES at the bottom of the stack. No
protocol bridge chip is needed; the external BOM is signal conditioning only.

## 3. Silicon Facts (verified against vendor docs)

| Fact | Value | Source |
|---|---|---|
| D-PHY TX support | GW5A 15K/25K/60K (75K/138K are RX-only) | UG296E §2 note 1 |
| Max HS rate | 2.5 Gbps/lane (IP dialog accepts 200–2500) | UG296E §2.1, §4.1.3 |
| DP RBR 1.62 Gbps | in spec, ~35% headroom | — |
| DP HBR 2.7 Gbps | **out of spec** — not planned | — |
| Gearbox | 8:1 or 16:1, fabric byte clock = rate/8 or /16 | UG296E §2.2.2 |
| Fabric clock @ RBR, 16-bit | **101.25 MHz** (`tx_clk_o`) | UG296E §4.1.1 |
| TX PLL | dedicated D-PHY PLL; CLKIN 19–800 MHz (27/54 MHz usable) | UG296E §4.1.3 |
| Continuous HS | fabric-controlled; no forced LP return | UG296E Fig 4-1 |
| MG121N MIPI quad | M0: 4 data lanes + clock lane bonded | UG985E pin list |
| MG121N MIPI rails | VDDAM (C8), VDDDM (D9) 0.87–1.0 V; VDD12M (K3) 1.14–1.32 V | UG985E |
| Tang Primer 25K module | MIPI quad routed to 2×60P BTB ("MIPI IO: 4lane Data"); Sipeed Dock does *not* route it — our carrier does | Sipeed wiki |

**MG121N ball map (M0 quad):**

| Lane | P ball | N ball |
|---|---|---|
| M0_D0 | A9 | B9 |
| M0_D1 | A8 | B8 |
| M0_CK | A7 | B7 |
| M0_D2 | A6 | B6 |
| M0_D3 | A5 | B5 |

The clock lane is unused for DP (embedded clock); it may be left disabled or
reserved as a debug output.

### Lanes and rates: keeping 1080p without HBR

The a2mega runs **HBR ×2 lanes** for 1080p60. HBR (2.7 Gbps) exceeds the D-PHY's
2.5 Gbps rating, so the a2p25 recovers the bandwidth with lane count instead —
**plan of record is RBR ×4**:

| Config | Payload (post-8b/10b) | 1080p60 @24bpp (3.56 Gbps) | 720p60 @24bpp (1.78 Gbps) |
|---|---|---|---|
| a2mega: HBR ×2 | 4.32 Gbps | ✓ 82% util | ✓ |
| a2p25: RBR ×2 | 2.59 Gbps | ✗ | ✓ 69% util |
| **a2p25: RBR ×4** | **5.18 Gbps** | **✓ 69% util** | ✓ |

Everything in the chain is 4-lane capable: the M0 quad bonds 4 data lanes, the
SN75DP130 is a 4-lane part, and 4-lane DP is the TUSB1046's native alt-mode
config (**pin assignment C** — carries USB 2.0 but *not* USB 3 alongside video;
the port is display-out on a2mega 1.0a3, so likely a non-issue, but confirm as a
product decision). Gateware delta: the a2mega core stripes 2 lanes today —
extending symbol striping + per-lane regearing to 4 lanes is modest new work.

Fallbacks if 2-lane were forced: 1080p60 @6bpc/CVT-RB fits at ~96% utilization
(**do not ship this** — no SDP headroom, strict-converter bait); 1080p30 fits
honestly; 720p60 fits at 69%. HBR ×2 (an 8% D-PHY overclock; the IP dialog
accepts up to 3.2 G) is a bench curiosity for the interposer, not a design basis.

**Pixel format note (framebuffer is RGB565):** DP has NO 16bpp wire format —
the link carries RGB at 6/8/10/12/16 bpc, so 565 expands at MSA/packing time
regardless. Plan of record: **8bpc/24bpp** (pad/bit-replicate LSBs) — the DP
mandatory baseline every sink and converter accepts; all tables above assume
it, so the 565 framebuffer changes nothing. **6bpc/18bpp carries RGB565
losslessly** (G6 exact, R5/B5 pad 1 bit) and buys margin (720p60×2: 69%→52%)
but does NOT rescue 2-lane 1080p60 (148.5M×18 = 2.67 G > 2.59 G payload;
CVT-RB = 96%, still no-ship) — and HDMI has no 6bpc RGB, so converters must
expand it with uneven support. 6bpc = per-sink optimization on known-good
DP-native monitors only, never the compatibility plan.

## 4. Signal Chain

```
GW5A-25 fabric                         (carrier board)
┌─────────────────────────┐
│ a2mega DP link layer    │
│  LT / scrambler / 8b10b │
│  MSA / audio SDPs       │
│  10b→16b symbol regear  │
└────────┬────────────────┘
         │ 16-bit @ 101.25 MHz per lane
┌────────▼────────────────┐
│ MIPI_DPHY hard TX (M0)  │  1.62 Gbps NRZ, ~280–540 mVpp diff (SLVS)
└────────┬────────────────┘
         │ BTB (2×60P)
       ══╪══ 100 nF AC caps (×2 per lane)
┌────────▼────────────────┐
│ SN75DP130               │  swing/pre-emphasis REGENERATION
│  VOD 340/510/690/1000mV │  + link-training response (AUX auto or I2C)
└────────┬────────────────┘
┌────────▼────────────────┐
│ TUSB1046-DCI            │  USB-C orientation mux + linear EQ
└────────┬────────────────┘
      USB-C receptacle  (FUSB302B: CC/PD, alt-mode entry — as on a2mega)

AUX  : FPGA LVDS25 + pulldown front-end (a2mega circuit) → routed THROUGH
       DP130 AUX pass-through (keeps auto-snoop option) → TUSB1046 → connector
HPD  : connector → TUSB1046 → DP130 HPD pass → ESP32 GPIO (a2mega model)
```

### AUX and HPD

AUX never touches the D-PHY — only the main link moved. AUX is 1 Mbps
Manchester, half-duplex, on **ordinary FPGA I/O** (any of the ~75 general BTB
pins), reusing the a2mega stack verbatim:

- **Front-end**: LVDS25 buffers + the proven a2mega bias/pulldown network —
  incl. the learned traps: NO RX inverter; DP-spec asymmetric bias for source
  detection. Do not re-derive; copy the circuit.
- **Fabric AUX engine unchanged**: Manchester framing, DPCD r/w, I2C-over-AUX
  EDID — incl. converter policies (native-DPCD presence, EDID-skip for
  converters that defer DDC forever).
- **Routed THROUGH the DP130** AUX_SRC→AUX_SNK pass-through (analog switch,
  ~5–10 Ω, spec'd to 360 MHz — invisible at 1 Mbps). Fabric owns the protocol;
  the DP130 only *snoops* TRAINING_LANE_SET writes in auto mode to set its own
  VOD/PE. Routing around it would forfeit auto mode; through keeps both.
  Config note: DP130 AUX pins double as DDC in its DP++/TMDS mode (CAD_SNK
  pin) — strap for DP mode permanently, we can never do DP++.
- **HPD**: connector → (TUSB1046, Option A) → DP130 HPD pass (~50 ns, handles
  termination) → ESP32 GPIO → fabric (a2mega model). Flap-tolerant handling
  required — sleeping-monitor flap storms reproduce identically here; IRQ_HPD
  (0.5–1 ms) vs disconnect (>2 ms) discrimination lives in firmware.
- **Per option**: A — AUX rides USB-C SBU1/SBU2, TUSB1046 flips SBU with
  orientation; PD layer never carries AUX. B — DP130 pass straight to jack
  pins 15/17. Passive-DP++ detection = AUX transactions time out (no DPCD
  sink) → OSD "active converter required".

### Why the SN75DP130 (and why the TUSB1046 alone is not enough)

The TUSB1046 is a **linear** redriver: output amplitude tracks input. Its EQ
ladder (the 16-step DPEQ machinery from a2mega firmware carries over) fixes
channel ISI, not source swing. The D-PHY pads emit MIPI SLVS levels —
|VOD| 140–270 mV, i.e. ~280–540 mVpp differential, ~400 nominal — which is
approximately DP swing level 0. Legal, but with zero headroom and no ability to
honor sink swing/pre-emphasis requests during link training (the strict-converter
class routinely asks for level 1–2).

The SN75DP130 (DP 1.1a/1.2, HBR2, 4-lane, VQFN-48) **regenerates** the link
rather than amplifying it:

- Input: AC-coupled, self-biased, 50 Ω terminated; squelch threshold ~80 mVpp
  default — D-PHY swing is 4–6× above it. Input EQ to 9 dB available at RBR.
- Output: programmable VOD 340/510/690/1000 mVpp (typ) + pre-emphasis
  0/3.5/6/9.5 dB + fine boost — the full DP level set, independent of input
  amplitude.
- Training response: monitors AUX and auto-adjusts output per sink requests, or
  fully manual over I2C (ESP32). **Board routes AUX through the DP130 pass-through
  so both modes remain available**; mode choice is a firmware decision (note the
  existing no-AUX-snoop policy on the TUSB1146/a2mega was mux-specific — evaluate
  DP130 auto mode on its own merits, fall back to I2C manual).
- Squelch gives correct semantics for free: quiet D-PHY lanes before training →
  main-link outputs held off → sink sees a proper dead link during bring-up.
- Adds only ~15 ps residual jitter. Mature, cheap (~$1–2), widely stocked
  (graphics-card heritage) — the anti-ANX7625 on procurement.

Ruled out: additional linear parts (PI3DPX1203/TUSB546/PS8330 — duplicate the
TUSB1046), SN75DP141 (linear, no regeneration), full retimers (MCDP2900 class —
solves a jitter problem not yet measured, at ANX7625-grade procurement/config
cost).

## 5. Gateware Plan

Port of the a2mega DP core with the PHY layer swapped:

**Reused unchanged:** link policy/training state machine (TPS1/TPS2), scrambler,
8b/10b encoder, BS/BE framing, MSA, Mvid/Nvid, audio SDP + timestamp generation,
AUX channel engine (1 Mbps Manchester on LVDS25 + pulldowns, native-DPCD
presence), HPD handling, FUSB302B PD/alt-mode firmware, TUSB1046 config incl.
DPEQ ladder.

**New/changed:**

1. **PHY shim** — replace SERDES TX wrapper with `MIPI_DPHY` primitive (or the
   UG296E TX IP): per-lane 16-bit `HSTXD` feed, `HSTXD_VLD`/`HSTXEN_LNx` held
   asserted while the main link is active, LP/ODT ports tied off, clock lane
   disabled.
   **IP catalog note (verified in IDE on GW5A-LV25MG121NC1/I0, 2026-09-01):**
   use the **`MIPI_DPHY`** IP (module `Gowin_MIPI_DPHY`) — NOT "MIPI TX
   Advance", which is the higher-level protocol wrapper (per-lane ELVDS IO TYPE
   = the rejected soft path). MIPI_DPHY dialog confirmed: 16-bit mode,
   TX ENABLE (this part has TX), per-lane enables, "Include PLL" → CLKIN
   19–800 MHz (default 50 = module osc), mDRP 1–100 MHz, Data Rate
   **200–2500 Mbps** field, VCO 700–1400 MHz, CLKOUT4 divider 1–128
   (÷8 @ VCO 810 = a second 101.25 MHz byte-rate output). Config: rate=1620,
   CLKIN=50, lanes 0–3, clock lane off → Calculate should yield VCO=810.
   After generation, grep RTL: fabric bytes must wire straight to
   `DxLN_HSTXD` with nothing in between.
2. **Symbol regearing** — 10-bit 8b/10b symbols packed into the 16-bit gearbox
   words (LCM 80-bit rolling buffer per lane; standard gearbox, LSB-first per
   D-PHY byte ordering — verify bit order in sim against a captured RBR stream).
   **IMPLEMENTED + SIM-VERIFIED** (`hdl/dp/dp_dphy_tx.sv`, TB
   `tests/test_dp_dphy_tx.sv`, `make -C boards/a2p25/tests dp_dphy`): shared
   lockstep gearbox (one bit-count, 4 lane buffers), pulls one 20-bit symbol
   pair per lane in 4 of 5 clocks via a registered req→vld contract (⚠ req
   pipeline is 2 cycles deep — the request rule must count the in-flight pull
   or the gearbox starves every 7th cycle); enable FSM sequences txdpen →
   settle → hstxen → settle → stream (settle timers because the generated
   wrapper leaves PLL `lock` dangling — re-expose it at bring-up). TB
   serializes words bit-0-first per TX_WORD_LITTLE_ENDIAN and reassembles
   symbols: 40k symbols/lane bit-exact, continuous vld, 4/2/1-lane modes,
   inactive lanes quiet.
3. **Clocking/CDC** — `tx_clk_o` (101.25 MHz) is the link-layer clock domain;
   CDC to the 54 MHz video/logic domain via existing FIFO patterns. D-PHY TX PLL
   fed from 27 or 54 MHz refclk (CLKIN range 19–800 MHz).
4. **Rate/lane policy** — RBR only; advertise 4 lanes in DPCD caps (1080p60
   plan of record, see §3). Extend a2mega 2-lane symbol striping to 4 lanes.
   TX swing/pre-emphasis DPCD handling: report max-swing-reached at level 0 if
   DP130 runs manual, or let DP130 auto-mode track requests.
5. **Link-off behavior** — deassert `HSTXEN`/`HSTXD_VLD` when ML is off; DP130
   squelch propagates this to the sink.

### Clocking — no new oscillators

The a2mega's 135 MHz oscillator is a SERDES-specific refclk requirement
(135×12=RBR / ×20=HBR on dedicated pins) and does **not** carry over. The D-PHY
TX PLL takes `clkin` (19–800 MHz) from the fabric clock network:

- **`clkin` = 54 MHz — RESOLVED (IP GUI Calculate verified 2026-09-01)**:
  rate=1620 with CLKIN=54 closes at **VCO=810** (×15). CLKIN=50 crystal-direct
  does NOT close ("No eligible Fvco"), while 50 @ rate 1500 DOES (VCO 750=50×15)
  — confirming the divider rule: **VCO must be an integer multiple of CLKIN**
  (810/50=16.2 impossible; no useful fractional path). Any integer divisor of
  810 works as CLKIN (54/27/81/135…). Note 50@1500 is diagnostic-only — 1500 is
  not a legal DP link rate; the link must be exactly 1620. Source: 54 MHz from fabric PLL (module 50 MHz osc ×27÷25, or
  tap the existing 54 MHz logic-clock tree). Cascade topology accepted pending
  interposer SER soak; break-glass crystal-direct upgrades if soak implicates
  reference jitter: 27 MHz ×30 or the a2mega's 135 MHz part ×6. The IP's
  internal PLL does all high-speed multiplication — never inject a fabric-
  synthesized 810 MHz. DP needs only ±300 ppm (any crystal clears 6×).
  Full IP config: 16-bit, TX ENABLE, lanes 0–3, clock lane off, rate 1620,
  CLKIN 54.000, mDRP 50, CLKOUT4 div 8 (= spare 101.25 MHz output).
- **`mdclk`**: any 1–100 MHz (54 MHz logic clock fine).
- **Link-layer domain**: `tx_clk_o` (101.25 MHz) is a PLL *output* — link layer
  runs on it, nothing to source.
- **Pixel clock** (148.5 MHz @1080p60): fabric PLL, async to link (measured
  Mvid, as on a2mega).
- **DP130 / TUSB1046 / FUSB302B**: clockless (FUSB302B internal osc).
- **No SSC** — D-PHY PLL has none; SSC is optional in DP (a2mega ships without).
- Regearing detail (Table 4-1): in 16-bit mode the **lower 8 bits are sent to
  the line first** — bit-order anchor for the 10→16 gearbox.

## 6. Board Design Requirements

1. **BTB breakout** — route M0 quad (A5–A9/B5–B9) from the 2×60P BTB as 100 Ω
   differential pairs, intra-pair skew tight; keep total FPGA→DP130 route short
   (the segment running at SLVS swing is the fragile one).
   **Length matching: intra-pair ONLY. Budget, not folklore: the DP130 input
   intra-pair skew spec is 20 ps (at HBR2!) ≈ 3 mm — actual layout achieved
   0.57–0.76 mm (≈3.7–5 ps, 0.008 UI at RBR), a 4× margin: ACCEPTED, route
   as-is.** (The N legs ran systematically ~0.7 mm long — single geometric
   cause at the BTB escape; a per-P bump there halves it if ever wanted.
   Skew doesn't accumulate downstream — the DP130 re-drives clean.)
   **Do NOT lane-to-lane match** — the 12.5–20 mm spread ≈ 45–50 ps ≈ 0.08 UI
   at RBR, vs multi-UI sink deskew (DP even permits 2 UI deliberate TX lane
   skew); the module's internal ball→BTB routing is unmatched anyway.
   Serpentines would add loss/discontinuities to the thinnest-margin segment
   for zero benefit. Applies to all three main-link segments.
2. **Polarity and lane order are layout-owned.** No inversion or remap exists
   anywhere in the chain (D-PHY TX has no invert; DP training does not fix
   polarity). Triple-check P/N against the module schematic before fab.
3. **AC coupling — full main-link cap map** (one 75–265 nF cap per leg, 100 nF
   X7R 0402 throughout; TUSB1046A-DCI DS Figure 27 is the authority for the
   mux side):
   - FPGA D-PHY → DP130 inputs: **8 caps** (blocks SLVS CM; DP130 input is
     50 Ω self-biased — no external termination or bias anywhere on this
     segment).
   - DP130 outputs → TUSB1046 DP0–DP3 inputs: **8 caps** (TI "system side").
   - TUSB1046 TX1/TX2 → receptacle: **4 caps**. TUSB1046 RX1/RX2 →
     receptacle: **NONE** — USB-C rule: each pair coupled exactly once;
     RX-role pins are coupled by the far-end device's TX caps. Caps on RX1/RX2
     double-couple; missing TX1/TX2 caps leave those lanes uncoupled.
   - Per-pair cap placement symmetric (same position along the route) in all
     segments to avoid P/N skew. **Position along the route: place caps near
     the RECEIVING device** (main-link caps near DP130 inputs; DP130→mux caps
     near TUSB inputs). The FPGA end is the BTB — caps there would stack the
     cap and connector discontinuities and crowd the fanout. Neither DS
     mandates a position; symmetry matters more than location at 1.62 G.
   - ⚠ **Amplitude budget on FPGA→DP130**: DP130 min input VID = 0.3 Vpp diff;
     D-PHY worst-case swing ≈ 0.28 Vpp (0.4 nominal). Nominal clears, worst
     case is thin — keep this segment as short/low-loss as the layout allows;
     DP130 input EQ (to 9 dB) is the recovery knob if needed.
   - **AUX passive placement — CORRECTED per SN75DP130 DS Fig 22 ("DP Only
     Configuration with AUX Pass Through"), which is exactly our topology:**
     ① a2mega FPGA-side network VERBATIM at the FPGA pins: series 100 Ω on
     TX/RX + bias dividers 100k/47k on AUX_N (~1.06 V) and 100k/43k on AUX_P
     (~0.99 V) — the ~65 mV N-over-P offset is load-bearing (idle polarity +
     sink detect); keep which side gets 47k vs 43k.
     ② **AC caps (a2mega's C46/C47, 0.1 µF) MOVE UPSTREAM: between the FPGA
     network and DP130 AUX_SRC.** Mandatory, not stylistic — DP130 AUX pins
     have a DC-input spec in DP mode (p: −0.5..0.4 V, n: 2.0..3.6 V) that the
     FPGA's ~1 V dividers would violate if DC-connected. No bias parts at
     AUX_SRC (pass-FET carries SNK-side bias through).
     ③ DP130 AUX_SNK → TUSB AUXp/n: **DC segment**, biased by a SINGLE 100 k
     pair at the TUSB pins (a2mega's R31 100k→GND on AUXp / R32 100k→3V3 on
     AUXn, unchanged) — this pair double-duties as the SNK-side bias TI
     mandates ("100 kΩ shall be placed on the AUXSNK side", Fig 22 note).
     ⚠ Do NOT add a second pair: parallel 100k pairs = 50k effective, shifts
     the sink-detect divider (~1.65 V → ~1.1 V) and eats detect margin.
     Net vs a2mega: same AUX BOM, caps relocated, nothing else new.
     - DP130 straps/placeholders (DS §10.2.2): CAD_SNK strapped V_IL (DP-only)
       with DNP 2 k pullup placeholder (mode override); DNP 2 k pullup
       placeholder on HPD_SNK (force sink presence on bench); DDC_SRC open.
     Option B (native jack): ①② identical; ③'s 100 k pair goes at the
     connector (pulldown AUX+ / pullup-3.3 V AUX−). SBU1/2 → receptacle DC
     with 2 MΩ (R33/R34), no caps. HPD: DC logic, no extra passives.
4. **MIPI rails** — confirm on the Sipeed 52300 core-board schematic that VDDAM/
   VDDDM/VDD12M are powered on the module (advertised MIPI support implies yes;
   **verify before layout** — download is captcha-gated, grab manually).
5. **SN75DP130** placement between BTB and TUSB1046; I2C to ESP32 (shared bus
   with TUSB1046/FUSB302B); AUX and HPD routed through its pass-through pins.
6. **Carry over from a2mega 1.0a3**: TUSB1046-DCI, FUSB302B, USB-C connector,
   HPD-via-ESP32, AUX front-end component values.
7. **HDMI** — existing a2p25 HDMI path is unaffected by this subsystem (board
   space/connector decisions out of scope here).

## 7. Risks and Open Questions

| # | Risk | Assessment | Mitigation |
|---|---|---|---|
| 1 | D-PHY TX jitter vs DP RBR budget | TX PLL characterized for D-PHY masks, not DP TJ. RBR budget is generous; unproven. | Measure with soak/SER instrumentation (guernok methodology). DP130 does NOT clean jitter (redriver, no CDR). Retimer is the escape hatch if it fails — not expected. |
| 2 | Continuous-HS assumption | Strongly implied by UG296E Fig 4-1 (all framing fabric-supplied); not an explicitly documented use case. | Bench-verify early: scope an unbroken PRBS/8b10b stream for LP excursions or gearbox hiccups. First gateware milestone. |
| 3 | Module doesn't power MIPI rails | Would kill the plan. Low likelihood (Sipeed advertises MIPI IO). | Verify schematic before any layout (§6.4). |
| 4 | DP130 auto-snoop misbehavior | TUSB1146 snoop history suggests caution; DP130's snoop is its core design center, different beast. | AUX routed through pass-through keeps both modes; I2C manual is the fallback. |
| 5 | Strict converters at RBR | Same population issues as a2mega Stage 5. | DP130 gives real swing/PE levels — strictly better equipped than a2mega's fixed-swing ladder era. Reuse 'r'/'v' instruments + converter zoo. |
| 6 | D-PHY bit/byte ordering vs 8b/10b symbol order | Silent-garbage class bug. | Sim against golden RBR capture; loopback via a DP-capable protocol analyzer or the a2mega RX leg if available. |

### Adapter 4-lane support: verification, not guesswork

Pin-assignment support is declared in the adapter's **Discover Modes ACK VDO**
(pin-assignment bitmask: C/E = 4-lane, D/F = 2-lane + USB3) — the DFP selects
from what's offered. Two consequences:

- **Adapter census tool (a2mega, today):** extend the FUSB302B alt-mode firmware
  to log/decode the mode VDO pin-assignment bits (telnet command, 'v'/'r'
  style). Works on a2mega despite its 2-lane wiring — the VDM is PD-layer only.
  Second gate: read DPCD 0x00002 MAX_LANE_COUNT over AUX post-entry (the
  converter chip's own lane cap). Census the converter zoo before the board rev.
- **Firmware fallback policy:** offer-driven, not hard-required. Adapter offers
  C/E → 4-lane RBR, 1080p60. Offers only D → 2-lane RBR, 720p60 (or 1080p30).
  "Wrong adapter" degrades resolution instead of failing.

**USB-C→HDMI docks specifically** have TWO lane gates, and the second bites:
the mux pin assignment (VDM) *and* the DP→HDMI converter chip's own
**DPCD MAX_LANE_COUNT** — not implied by the mux. Cheap 1080p dongles often use
2-lane converters (2-lane HBR = 4.32 Gbps covers 1080p60 for normal sources) —
**for our RBR-only source these are traps**: 2×RBR = 2.59 Gbps → no 1080p60,
despite the dongle legitimately "supporting 1080p."

**FIRST CENSUS RESULTS (2026-09-01, a2mega fw e3c41cbe, 4 hubs — answer key
in boards/a2mega/docs/mac_anker_aux_answer_key.md @ 0eefc9c5):** the C
advertisement is **near-universal** (Anker + Ugreen both advertise C,D), so
**the PD census alone cannot qualify a hub for ×4** — the Anker is the
documented advertise-C-but-DPCD-2 counterexample (mux passes 4 pairs, the
converter trains 2). The converter DPCD gate is the BINDING gate;
MAX_LANE_COUNT readout (gateware register, in flight) is REQUIRED for
qualification. Three-rung ladder: PD census → DPCD readout → actual ×4
training. Census-empty is itself a verdict (hub #3: PD-incompatible,
DR_Swap unanswered — distinguishes PD-dead from lane-limited). And the
**DP-pass-through class** (docks with a native DP port, no converter chip)
inverts the ranking: no second gate exists, the monitor's own DPCD applies
— prime RBR×4 sinks, currently gated on our own monitor-direct AUX RX
margin TODO, not on the hubs.

**Adapter (dongle) class beats hub class**: video-only USB-C→HDMI adapters
("unidirectional", no USB ports) are pin-assignment C/E by construction —
no USB3 to preserve, so the mux gate can't fail. The 8K30/4K144-rated tier
(HDMI 2.1 FRL converter silicon, RTD2173-class) is both DSC-capable (4K144
≈ 28.7G exceeds even HBR3×4 uncompressed — DSC proven by the spec sheet)
AND built on HBR3 ×4-lane front ends — strongest shelf candidates for
RBR×4, but per the Anker lesson still census-then-DPCD before recommending.

Shelf heuristics for HDMI docks (BOM/docs, not qualification):
- **4K60 / HDMI 2.0 rated ⇒ safe** — 4K60 ≈ 14.3 Gbps needs HBR2×4, so the
  converter must be 4-lane and the mux C/E. RBR×4 through it → 1080p60 works.
- **"4K" / 4K30 / HDMI 1.4 ⇒ ambiguous** — 7.1 Gbps fits 4-lane HBR *or*
  2-lane HBR2. Census required.
- **"1080p" dongles ⇒ assume 2-lane** until the census proves otherwise.
- Docks with USB-A 3.x ports + video = assignment D = 2-lane, regardless of
  HDMI rating. Passive USB-C→DP cables are virtually always C/E.

Lane count and rate are independent at the mux — any 4-lane path carries RBR×4.

## 7b. Connector Strategy: USB-C Alt Mode vs Native DP Port

Two connector options share the identical FPGA → AC caps → SN75DP130 core:

**Option A — USB-C alt mode** (as specified above): TUSB1046-DCI + FUSB302B +
PD firmware. Single modern port, a2mega parity. Costs: two extra chips, PD/
alt-mode firmware, mux pin-assignment gate (§7a census), PD-layer flap exposure.

**Option B — native full-size DP receptacle + separate USB port**: chain ends
DP130 → jack (the DP130's original graphics-card application). Deletes both
support chips, all PD firmware, and the *entire mux pin-assignment gate* — a
full-size DP jack always wires 4 lanes. Cheapest, lowest-risk variant.
Option B specifics:

- **DP_PWR pin required** — 3.3 V / 500 mA (fused/current-limited) on the
  connector; this powers most active DP→HDMI converters. Omitting it silently
  breaks half the dongle market.
- **DP++ is physically unsupported** (source cannot emit TMDS/DDC — D-PHY is
  DP-only). Passive DP++ adapters dominate the cheap DP→HDMI shelf and will
  black-screen. Docs must say "ACTIVE converter" loudly; firmware should detect
  the no-DPCD-sink signature (AUX timeouts) and surface "passive adapter not
  supported" on the OSD.
- The **converter-chip lane gate persists**: active DP→HDMI dongles still have
  their own DPCD MAX_LANE_COUNT (the 2-lane 1080p-dongle trap exists in DP-plug
  form). Customer guidance unchanged: active, 4K60/HDMI 2.0-rated. Census tool
  simplifies to a DPCD read.
- Gives up: single-cable USB-C monitors (DP→USB-C display cables are rare/
  flaky — effectively unsupported), one-port cleanliness, a2mega alt-mode
  firmware/test commonality.

**Decision aid:** validation-ladder step 2's interposer (BTB → DP130 → DP jack)
*is* Option B's rev 0. Build it first regardless — it de-risks the D-PHY/DP130
chain for both options and defers the connector decision until bench data
exists. Recommendation lean: Option B for the low-cost positioning; Option A
remains the premium/a2mega-parity story.

### Product-level connector options (2026-09-02 analysis)

Three product shapes considered:
1. **Single USB-C (video + data)** — one-cable mini-dock UX.
2. **Dual USB-C (video + USB2 data)** — better variant: **video USB-C +
   data USB-A** (unambiguous ports; gamepads are USB-A anyway).
3. **DP jack + USB port** (= Option B above) — cheap, simple, honest.

**Load-bearing fact for option 1: pin assignment C keeps USB 2.0** — D+/D−
is separate from the four SS pairs, WE (the DFP) select the assignment, the
firmware already selects C exclusively, and both census hubs offer C+D.
Selecting C costs the user nothing: our USB host is the ESP32-S3's
full-speed 12 Mbps port, so the hub's USB3 is unusable by us in ANY
assignment. Keyboards/gamepads/USB2 ethernet ride D+/D−. Option 1's real
limits: hub converter gate (VIA-class = 720p floor), the PD-dead tail
(hub #3 class), and video-only DONGLES kill USB entirely (no D+/D−
passthrough) — the accessory guaranteeing 1080p60 removes the gamepad.

**The deciding question: is a 720p60 base experience acceptable** (Enhanced
boards ship 720p today)? If "Enhanced-parity resolution, 1080p when the
accessory allows" is an honest product line, option 1 works with the
offer-driven fallback. If 1080p60 + USB accessories must be guaranteed
simultaneously, one connector cannot do it → options 2b/3.

| | 1: single USB-C | 2b: USB-C video + USB-A | 3: DP + USB |
|---|---|---|---|
| 1080p60 + gamepad guaranteed | ✗ hub-dependent (720p floor) | ✓ dongle class | ✓ |
| One-cable dock UX | ✓ | ✗ | ✗ |
| Worst trap | 2-lane hubs → 720p (benign); PD-dead hubs | minor port confusion | DP++ passive adapter (detectable, doc-able) |
| BOM/firmware | full PD stack | full PD stack + 2nd conn | **neither** |
| Support burden | qualified-hub treadmill | low | low |
| USB-C monitors | ✓ | ✓ | ✗ |

Leanings: option 2-as-specified (dual USB-C) is dominated — full PD cost +
second connector + port confusion for a guarantee 3 delivers cheaper.
For low-cost positioning: **option 3** (DP++ OSD detection + DP_PWR +
recommended-adapter SKU). If the one-cable dock vision IS the product:
option 1, boxed as 720p-minimum/1080p-capable. Census pass-through-class
finding tilts slightly DP-ward (zero converter-gate exposure there).

### DECISION (ed, 2026-09-02): dual USB-C — one video, one data

DPCD gate tests came back: **every on-hand mini-dock reads MAX_LANE_COUNT=2**
— hub makers solve for fast-enough-USB + good-enough-HDMI with 2-lane
converters, so the single-port shape is quietly capped at 720p for most
real accessories. The "dominated" verdict above assumed hubs might pass the
converter gate; the bench removed that premise. Product shape:
- **Video USB-C**: alt-mode DP (DP130 + TUSB546A + FUSB302B chain as
  specified) — pairs with the dongle class (C/E by construction) and USB-C
  monitors for guaranteed 1080p60.
- **Data USB-C**: plain USB2 host port — Rp pull-ups on CC1/CC2, VBUS out,
  D+/D− to the ESP32-FS host. No PD controller, no mux.
- Port-confusion mitigation: label the video port; the FUSB302B sees every
  attach, so a partner with no DP sink mode → OSD hint ("display is the
  other port"). The census machinery doubles as the wrong-port detector.

### Power architecture (vs a2mega's LM66100 + TPS2553)

- **VBUS-out current limiting: REQUIRED, one switch per port — NOT a fuse.**
  A polyfuse trips in seconds at 2× rating, and for those seconds the fault
  drags the Apple II slot 5 V rail (the measured 4.75 V rail = Disk II
  failures lesson); a shorted accessory must not crash the host machine.
  Electronic switches fold back in µs, control dongle inrush (video port
  must source VBUS by spec — FRL dongles pull 1 W+), and auto-recover.
  **Part: TPS2553, one per port** (video ~1.5 A budget, data ~0.5–1 A) —
  a2mega-proven AND cheap via PCBWay ($0.18 sourced, 2026-09-02), so no
  substitution needed. Optional PTC upstream as backstop; fuse-ONLY is the
  false economy regardless of price.
- **LM66100 (VBUS-in ideal diode): KEPT, on the DATA port** ($0.21 via
  PCBWay). The data port is the programming path (its D+/D− are the ESP32's
  USB pins), so bench power belongs there: VBUS-in → PTC → LM66100 → rail,
  a2mega model minus video. **This placement de-fangs the backfeed class**:
  the a2mega's phantom-VBUS pain was the leak confusing the PD stack
  (VBUSOK=1 empty → attach fails as no-HPD); the data port has NO PD
  controller to confuse, and the VIDEO port (where the FUSB302B decides off
  VBUS state) becomes strictly source-only with no power-in path at all —
  the failure class is architecturally separated from the function it broke.
  Strings: (1) **C-to-C cables won't bench-power an Rp host port** — the
  data port presents Rp for gamepads, so a laptop on C-to-C won't drive
  VBUS; A-to-C cables hard-wire VBUS and work (document it; Tang module's
  own USB-C is the C-to-C-only fallback). (2) Copy the a2mega's TPS2553
  enable policy along with the parts — source and sink paths share the
  connector and the anti-fight logic is part of the proven design.
  **OPEN SUB-QUESTION — bench-power scope (load budget):** programming
  needs <500 mA, but the whole carrier through a whole-rail LM66100 is
  ~0.8–1.5 A (Tang module 300–500 + ESP32 100–240 + DP130 130–230 + mux) —
  a PC port can't run it. Fork: **(a) scoped feed** — VBUS → diode/switch →
  ESP32 regulator input ONLY (programming works on any PC port, rest of
  board stays dark, smallest backfeed surface; whole-board bench = add the
  Tang module's own USB) — best fit if programming is the requirement;
  **(b) whole-rail LM66100** (a2mega precedent) — one-cable full-board
  bench but REQUIRES a 5 V/2 A supply, PC-port flashing only marginally
  works. Leaning (a).

**720p floor status for the census hubs**: bandwidth-assured (RBR×2 =
2.59 G vs 1.78 G needed, 69% util) and 2-lane-proven at length (the whole
Stage-5 campaign is 2-lane through Anker/Ugreen) — but ALL history is at
HBR (0x0A). RBR is spec-mandatory for every sink, but per project ethos:
**pending one bench run forcing the a2mega ladder to 0x100=0x06 (RBR) ×2
and confirming colorbars on Anker + Ugreen** — ten minutes that make the
720p floor a measured fact at the only rate the a2p25 will ever speak.
1080p60 remains pending a converter-gate-passing dock (or the pass-through
class once the monitor-direct AUX RX margin work lands).

## 8. Validation Ladder

0. **Adapter census** (a2mega, before hardware): VDM pin-assignment dump +
   DPCD MAX_LANE_COUNT across the converter zoo.
1. **Rail + pin verify** (schematic, DMM on module): MIPI rails live, BTB map.
2. **Interposer spin** (cheap, before the real rev): BTB → AC caps → DP130 →
   full-size DP receptacle + AUX/HPD wiring. No USB-C, no mux. Answers risks
   1/2/6 with the existing soak station before the a2p25 rev commits.
3. **Gateware bring-up** on interposer: continuous-HS scope check → link training
   against a forgiving monitor → colorbars → audio SDPs → SER soak (target the
   a2mega bar: measured SER, zero teardowns, overnight).
4. **Converter zoo** pass with DPEQ ladder + DP130 level sweeps.
5. **a2p25 rev layout** with TUSB1046/FUSB302B integrated; alt-mode entry and
   orientation flip regression (a2mega test plans apply).

## 9. BOM Delta (video output subsystem)

| Part | Role | Notes |
|---|---|---|
| SN75DP130 | swing/PE regeneration, LT response | ~$1–2, VQFN-48, stocked |
| **TUSB546A-DCI** (was TUSB1046) | USB-C alt-mode mux + linear EQ | **PLANNED PART (price/stock checked 2026-09-02)**: pin-compatible sibling of the a2mega's TUSB1046A (same WQFN-40 4×6, pinout, register structure, EQ class, DP 1.4 HBR3); only delta is USB3 Gen1-vs-Gen2, unused here (assignment C, USB2 on D+/D−, FS host). LCSC $3.39–3.60 in stock vs 1046A $5.61–5.97 **OOS at LCSC**; DK $6.72/738pc vs $7.58. Both TI-ACTIVE. 1046A = drop-in fallback if sourcing flips. Prefer the "A" rev (plain TUSB546 exists cheaper at DK — errata-check before considering). Verify before layout: full pin diff, EQ step table vs fw DPEQ ladder, I2C address. |
| FUSB302B | CC/PD | carryover |
| AC caps 100 nF ×(2×lanes) | DP coupling | passives |
| — | | No bridge chip. No ANX7625. |

## 10. References

1. Gowin **UG296E** — Arora V Hardened MIPI D-PHY User Guide (v1.1.2E, 07/2025):
   primitive ports §3.1, TX IP §4.1, timing Fig 4-1, rates §2.1/App A.
2. Gowin **UG985E** — GW5A-25 Pinout (MG121N pin list: M0 quad, MIPI rails).
3. TI **SN65DSI86** datasheet (SLLSEH2C) — rejected-option analysis: SDP tunnel
   §8.4.4.3.3, supported DSI data types Table 8-2, ASSR-only §8.3.2.
4. TI **SN75DP130** datasheet (SLLSE57E) — VOD/PE tables §7.6, squelch, AUX
   auto-config, switching §7.7.
5. Sipeed Tang Primer 25K wiki — core board 2×60P BTB, "MIPI IO: 4lane Data";
   Dock omits MIPI pins.
6. a2mega references: DP TX gateware (60K/138K), TUSB1146 DPEQ ladder work
   (SLLA404/365), 1.0a3 USB-C support-IC stack, Stage 5 converter-class findings.
