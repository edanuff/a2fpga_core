# WS4 — DP output on Gowin's own IP stack (a2mega_dp_gowin)

**Status: hardware test 2026-08-19 FAILED (C:8011, CR-only) → root-caused
against Gowin's EDP reference design (§9: generator drops hardened-8b10b CSR
fields on lanes 2/3) → CSR fix VERIFIED IN SILICON (CR idx 0A = 0x00013110)
but exposed a second-layer defect: hardened 8b10b at 1:1 gear yields a broken
60.75 MHz word clock (§10) → MIGRATED to the reference 1:2/X40 geometry,
REBUILT timing-clean. Ready to re-flash; on-hardware checks: Q: ≈ 67,500,000,
D3 lit, CR idx 0A = 0x00013110, then C: past 0x8011.**

Workstream 4 of the a2mega 1.0a3 bring-up: an alternative DP-output bitstream that
replaces the hand-rolled DisplayPort core's main-link stream side and PHY mode with
**Gowin's supported IP stack** — EDP Encoder IP v2.7 + the EDP PHY SERDES protocol
module — to assess whether the vendor-supported path trains converter-class sinks
(DP→HDMI hubs) more robustly than our open-source core. Same hardware, same payload
(1080p 2-lane HBR colorbars), same AUX front end, same UART telemetry format —
a 1:1 A/B against `a2mega_dp_test`.

---

## 1. What Gowin's DP stack provides

Four pieces, all in IDE V1.9.12.03 (`ipcore/` in the app bundle):

| IP | Version | Role | Docs |
|---|---|---|---|
| **EDP Encoder** | 2.7 | Main-link *stream* side only: pixel steering (1/2/4 lanes), TU packing + stuffing, VB-ID/Mvid/Maud framing, MSA insertion, lane skewing (2 LS_Clk), scrambler (SR/0xFFFF or eDP 0xFFFE), TPS1–4 pattern generation. Outputs bytes+K flags per lane for a hardened-8b10b SERDES. **No AUX, no DPCD, no training policy, no audio/SDP.** | IPUG1179-2.6E (archived in `docs/`) |
| **EDP PHY** | 1.2 | A *protocol preset inside the SerDes IP generator* (Protocol → Video → EDP PHY), not fabric logic: emits a GTR12 config with hardened PCS 8b10b, native DP rates (1.62/2.7/5.4/8.1), per-lane TX/RX PN invert, AFE (swing 180–900 mV, FFE, RX EQ), optional DRP port, RX channel bonding. User face per lane: `txdata[15:0]`+`txk[1:0]` @ 1:20 (or 32/40, 64/80). | IPUG1043-2.2E (archived in `docs/`) |
| **DP Aux** | 1.0 | See §5 — source- and/or sink-side AUX channel + link policy engine. | No IPUG found (in-bundle HTML only; CN portal `gowinsemi.com.cn/ip/188`) |
| **EDP Decoder / RX Desteer** | 2.7 / — | Sink-side unpacking (not relevant to WS4). | IPUG1179 |

The intended composition (IPUG1043 Fig 3-1 and the DK_DP_GW5AT-LV60UG225 reference
design): video → **EDP Encoder** → **EDP PHY** → SERDES pads, with an **AUXTX**
block (the DP Aux IP) running training and driving the encoder's TPS enables.

## 2. Integration architecture chosen for WS4

```
 colorbars 2px/clk ──> Gowin EDP Encoder v2.7 ──bytes+K──> EDP PHY emission ──> pads
 (74.25 MHz strm)      (TU/MSA/scramble/TPS)   (135 MHz)   (GTR12 hardened 8b10b,
        ▲                    ▲ TPS1/TPS2/video_en           Q0 ln2/3, 2.7G, PN inv)
        │                    │
   gowin_pixel_pll      channel_management  ◄──AUX/HPD──  sink
   (÷ from clk_sym)     (OUR closed-loop ladder, unchanged)
```

- **Kept from our core:** the entire link policy (`hdl/displayport/auxch/*` —
  closed-loop AUX training, DPCD caps, retrain ladder, blind fallback), the proven
  TLVDS AUX front end, HPD, LEDs, UART telemetry. The policy's
  `tx_clock_train`/`tx_align_train`/`tx_link_established` map 1:1 onto the encoder's
  `I_tx_tps1_en`/`I_tx_tps2_en`/`I_tx_video_data_en`.
- **Replaced by Gowin IP:** dp_video_timing + pixel CDC FIFO + stream packer + MSA
  inserters + main_stream_processing + scrambler + skew + fabric 8b10b (→ EDP
  Encoder), and the raw-mode Customized-PHY emission (→ EDP PHY, hardened 8b10b).
- **Clocking:** synchronous — `clk_strm` = 135 MHz × 44/5 ÷ 16 = 74.25 MHz
  (2 px/clk = 148.5 Mpx/s) from `gowin_pixel_pll` off the PHY's `pcs_tx_clk_o`.
  MISC0=0x21 (RGB 8bpc, sync), MVID_CUSTOMER with the production M/N convention:
  Mvid=288358, Nvid=524288 (11/20 · 2^19, rounded — exact small rationals are
  rejected by some sinks, see the core's README discussion).
- The DP main-link *lane 0* is die lane 3, lane 1 is die lane 2 (board remap), both
  PN-inverted **statically in the PHY emission** — no fabric inverters, no DRP pokes.

## 3. What was built (all committed)

```
boards/a2mega/a2mega_dp_gowin.gprj                 project (SecurityBit OFF)
boards/a2mega/a2mega_dp_gowin.gprj.user            carries RES.serdes.csr (load-bearing!)
boards/a2mega/impl/a2mega_dp_gowin_process_config.json
boards/a2mega/hdl/dp_gowin/
  a2mega_dp_gowin_top.sv        top: clocks, AUX front end, colorbars/timing
                                (2 px/clk), UART telemetry (dp_test format), LEDs
  dp_gowin_transmitter.sv       channel_management + EDP Encoder + PHY bank + strm PLL
  edp_phy_bank.sv               EDP PHY wrapper: reset cadence, tx_vld write-gate,
                                read-only DRP register-dump bridge
  edp_encoder/                  EDP Encoder v2.7 "generated project" (headless — §4)
  serdes_edp/                   EDP PHY emission (recovered — §4)
  a2mega_dp_gowin.cst / .sdc    same pins as dp_test; clk anchors survive
```

### Headless IP generation — how the GUI was avoided entirely

**EDP Encoder v2.7:** the "generated project" is just (a) the IP's static source
files copied verbatim from the IDE bundle (`edp_encoder_top.v`,
`edp_encoder_wrapper.v` — encrypted, sha256 `b76ba6d5…`, GowinSynthesis decrypts it —
and `static_macro_define.v`), plus (b) two tiny macro files the GUI writes:
`top_define.v` (`` `define module_name EDP_Encoder_Top ``) and
`edp_encoder_defines.v`. The complete set of `define` templates the GUI can emit was
recovered from `libEDP_ENCODE.dylib` strings and cross-checked against IPUG1179-2.6E,
so the hand-written defines are generation-equivalent:

```
TX_2_LANES, TX_X20_MODE, DEF_TX_SYM_WIDTH 16, DEF_TX_BPP 24, DEF_TX_BPP_COEF 1,
DEF_TX_FIFO_DEPTH 256, MVID_CUSTOMER, TX_TPS1_ENABLE, TX_TPS2_ENABLE
```

(GUI equivalents: 1:20, 2 lanes, 24 bpp, coef 1, line buffer 256, Mvid Customer ✓,
TPS1 ✓, TPS2 ✓, TPS3/TPS4/Debug ✗.) Synthesis compiles the encrypted wrapper under
these defines to real logic (4 BSRAM line buffer, k-code/tx-data ports of the right
widths) — verified in the netlist.

**EDP PHY:** the SerDes-IP emission cannot be hand-written (encrypted
`upar_arbiter`, `.csr` CSR-init blob, `.mod`/`.toml` sidecars) — but it did not need
to be: **the exact emission for this board already existed in git history** (commit
`d535df5f`, "EDP PHY build #3", generated in the IDE on 2026-08-16 and later
reverted by plan C). Recovered verbatim into `serdes_edp/`:
Q0 lanes 2+3, REFCLK1 @ 135 MHz, QPLL0, 2.7 Gbps, 1:20, `encode_mode = "8b10b"`,
`width_mode = 20`, `tx_pol_invert = true` both lanes, `txlev = 13` (~804 mV), FFE
auto, DRP on — all verified in `serdes_tmp.toml` before adoption.

**gw_sh has no IP-generation TCL** (no `ip_*` commands; generation lives in the GUI
dylibs), so this copy+defines / recover-from-history approach *is* the headless
path. **No GUI session is required** — §6's checklist is only for optional
re-validation.

### Hard-won integration rules carried over (would have cost a board-week each)

- **Reset cadence:** release resets statically on a timer (never gate on a lock
  indicator): `fabric_rstn` at powerup, `tx_rst` ~21 ms later.
- **`tx_vld` must NOT be tied 1** (the Gowin reference design ties it 1 and it
  *failed on this hardware*): with `tx_rst` holding the PCS read side while the
  fabric writes, the TX FIFO parks almost-full and drops words forever → sink
  CR-locks but never symbol-locks. `edp_phy_bank` releases `tx_vld` a few fabric
  clocks after `tx_rst` so each session starts write/read lockstep on an empty FIFO.
- **DRP register-dump bridge** kept read-only (same CSR probe ROM as production) so
  the `CR ii aaaaaa dddddddd` UART lines still answer "did the CSR replay land".

## 4. Build provenance

| | |
|---|---|
| Base commit | `8280d7c1` (project sources; on `cfd720c9` = tip of `claude/epic-lovelace-54ee14`) |
| Toolchain | Gowin V1.9.12.03 (certified install; `gw_sh` via `/Users/edanuff/gowin-1.9.12-ide/bin/gw_sh`, pipe method through `tools/build.sh`, `GPRJ=a2mega_dp_gowin.gprj`) |
| Timing | **0 setup / 0 hold violations; TNS 0.000 all domains.** Fmax: clk_sym 135.491 ≥ 135.007, clk_strm 172.5 ≥ 74.25, clk100 122.5 ≥ 100, clk50 120.2 ≥ 50, cm_life 167.4 ≥ 100 |
| Resources | 3531 LUT / 2575 FF / 4 BSRAM / 2 PLLA (vs the full open core's stream path — the encoder is *smaller*) |
| `a2mega_dp_gowin.fs` sha256 | `43854f32e7289197b349e1f748e076efafe1fa94e7f9607e6b61bd2d2755bf87` |
| `a2mega_dp_gowin.bin` sha256 | `b2d91ffbe8f1e1715cd4ed4746b33025a6c6ba3ee2160270df51a2ff382ca236` (`.bin` is gitignored; regenerate = rebuild) |
| SECURITY_BIT | **False** (verified in `impl/a2mega_dp_gowin_process_config.json`) |

Netlist connectivity audited: encoder `O_tx_data/O_k_code` reach
`GTR12_QUADA.FABRIC_LN2/LN3_TXDATA_I` low 20 bits as `{0, txk[1], data[15:8], 0,
txk[0], data[7:0]}` (invalid bits grounded) — the hardened-8b10b format.

Flash with `GPRJ`/`FS` pointing at **a2mega_dp_gowin** explicitly (multi-gprj board —
see the wrong-gprj trap note). UART telemetry is line-compatible with dp_test
(`DP S:.. D:.. F:.. HLVC:.. P:. E:.. R:.. A:.. G:.. Y:.. C:.. Q:.. K:.. X:..` + `CR`
register lines); D4/led[3] = video_live, led[2] = 135 MHz line-rate check.

## 5. DP Aux IP v1.0 evaluation (read-only — not integrated)

From the bundled template (`dp_aux_top.v`) and `libDP_AUX.dylib` GUI surface (no
public IPUG found):

- **What it is:** a full AUX-channel engine with three modes (TX / RX / both).
  **TX mode is a source-side link-policy engine**: it owns the AUX Manchester pads
  (`IO_tx_man_p/n`, bidirectional half-duplex 1 Mbps), and its outputs map 1:1 onto
  the EDP Encoder's control inputs — `O_tx_tps1/2/3_en`, `O_tx_video_data_en`,
  `O_tx_frame_type`, `O_tx_scrm_value`, plus `O_tx_lane_num[2:0]` and
  `O_tx_aux_is_ok`. I.e. it runs DPCD discovery + link training autonomously.
- **DRP master port** (`O_drp_addr[23:0]`/`wrdata[31:0]`/…) matching the EDP PHY's
  DRP slave exactly — so it can retune the SERDES during training (rate,
  and presumably swing/pre-emphasis from ADJUST_REQUEST — the piece our stack
  handles by "static AFE + max-swing-reached replies").
- **APB slave** (10-bit addr) for a host CPU: register map undocumented (encrypted
  wrapper, no IPUG). Config knobs: Aux Port Enable (TX/RX/Both), RX APB Clock
  Frequency (`DEF_RX_FREQ`), Debug Enable (4×32-bit test taps each way).
- **What it would replace in our stack:** `channel_management` + `aux_channel` +
  `aux_interface` + `dp_aux_messages` + `dp_register_decode` + `edid_decode` +
  `hotplug_decode` + `link_signal_mgmt` — the whole `auxch/` tree — plus our AUX
  front-end bit-banging (it drives the pads itself).
- **Does the encoder require it?** No — the encoder is freestanding (WS4 proves it:
  our ladder drives the TPS enables). The Aux IP is the vendor's *optional* policy
  tier.
- **Why it is NOT in the WS4 build:** (1) no register map / IPUG — whether it needs
  APB servicing to start, and how HPD enters it (it may expect its own pad, but our
  HPD arrives via ESP32 GPIO reconstruction) is unknowable without the reference
  design; (2) its pad-level front end presumes the IPUG1179 bias network — our
  TUSB1046A/SBU path with the TLVDS park sequence is exactly the kind of front end
  a canned IP tends not to tolerate; (3) it would *reduce* observability (our ladder
  exports the full DPCD trace the bench playbooks key on). **Follow-up:** get the
  `DK_DP_GW5AT-LV60UG225` reference design + Aux IPUG from Gowin (support ticket /
  CN portal) before attempting it; it is the right experiment *after* the
  encoder+PHY pair proves itself, isolating policy-vs-stream variables.

## 6. Optional GUI validation checklist (~5 min; NOT required to build)

Only to cross-check the headless emissions against fresh GUI output — diff, don't
adopt blindly (pitfall: SERDES 1.2 regenerated *our raw config* with a hardware
regression; the EDP PHY preset itself has no 1.1 baseline to regress from).

1. Open `boards/a2mega/a2mega_dp_gowin.gprj` in the IDE (V1.9.12.03).
2. Tools → IP Core Generator → Multimedia → **EDP Encoder** (v2.7):
   File Name `edp_encoder`, Module Name `EDP_Encoder_Top`, Create In
   `…/boards/a2mega/hdl/dp_gowin/edp_encoder_gui` (a *scratch* dir — diff against
   `edp_encoder/`). Options: User Clock Ratio **1:20**, Lanes **2**, Bits Per Pixel
   **24**, Coefficient **1**, TX Line Buffer Depth **256**, **Mvid Customer ✓**,
   **TPS1 ✓**, **TPS2 ✓**, TPS3 ✗, TPS4 ✗, TX Debug ✗ → OK.
   Expect: identical `edp_encoder_top.v`/`edp_encoder_wrapper.v`/
   `static_macro_define.v`; `top_define.v` + `edp_encoder_defines.v` semantically
   equal to ours.
3. IP Core Generator → Serdes → **Serdes** → Protocol **EDP PHY** (1.2) → Create.
   General: File Name `dp_serdes`, Module Name `dp_serdes`, scratch Create-In.
   PHY tab: Quad **Q0**, Channel **PHY Lane2 ✓ + PHY Lane3 ✓** only, Loopback OFF,
   **DRP Port ✓**, TX Line Rate **2.7**, TX User Clock Ratio **1:20**,
   **TX PN Invert Lane2 ✓ Lane3 ✓**, RX side don't-care (2.7/1:20, no inverts).
   Refclk: Source **Q0 REFCLK1**, Frequency **135.000** ⚠ *the field resets to 125
   on every open — re-type 135 before OK*, PLL **QPLL0**. AFE: Differential Swing
   **804 mV** (txlev 13), FFE **Auto**. Channel Bonding: **None** (⚠ the TX-bonding
   checkbox emits a broken half-bond in both generator versions — never enable).
   → OK, OK. Diff the scratch `serdes_tmp.toml`/`dp_serdes.v`/`.csr` against
   `serdes_edp/` (expect: identical modulo timestamps; pay attention to per-lane
   `txlev`, `tx_pol_invert`, `encode_mode`).
4. Do **not** let the IDE re-save the project over our gprj/process config
   (SecurityBit would come back ON by Gowin default on a fresh save — verify
   `SECURITY_BIT: false` in `impl/a2mega_dp_gowin_process_config.json` afterwards).

## 7. Open questions & risks (with pitfall cross-checks)

1. **Hardened-8b10b history (pitfall 3).** Our core driving this same PHY mode
   never achieved sink symbol lock (polarity-independent, silicon-verified DRP
   experiment). The `tx_vld` FIFO-park failure explains *one* mechanism
   (and is fixed here); if symbol lock still fails with the matched encoder, the
   suspect ordering is: (a) some encoder-emission subtlety our fabric path lacked
   (disparity phasing, symbol order), now provided correctly by the matched pair —
   the hypothesis WS4 tests; (b) a genuine PHY-mode defect → strong Gowin-ticket
   material either way. The DRP dump lines give the evidence.
2. **SERDES 1.2 emission regression (pitfall 1).** The recovered emission *is* a
   current-generator (V1.9.12.01-header) EDP-preset emission — there is no 1.1
   EDP-preset baseline. The known regression was observed when regenerating the
   *raw-mode Customized-PHY* config; treat 20-s bring-up stalls or link degradation
   on hardware as this class and compare `CR` dumps against the production build's.
3. **Encoder idle behaviour is undocumented.** Between "TPS enables low" and
   "video_data_en high" the encoder's output contents are unspecified (our core
   inserts the DP idle pattern). The policy raises video_en within a few ls clocks
   of link_established, so the window is small; if strict sinks drop the link right
   after training, suspect this gap first.
4. **VB-ID input semantics.** `I_vb_id` is an 8-bit input tied to 0x00; assumed the
   IP maintains the VerticalBlanking_Flag internally. If sinks show a
   frame-cadence complaint (rolling/blanking), revisit (may need vs-gated VB-ID).
5. **No audio** on the Gowin encoder (no SDP inputs in v2.7). The A/B comparison is
   video-only; audio stays a reason our core remains production.
6. **AFE is static** (804 mV, FFE auto): sink ADJUST_REQUESTs are answered
   "max-swing-reached" by the ladder, as in the current production config — same
   behaviour, but the warm-EQ-sweep learnings (6.5 dB default) do not transfer to
   this PHY config; the mux-thermal margin axis is unchanged (board-level).
7. **Line-buffer depth 256** chosen (max of 64/128/256) — headroom over the GUI
   default 64; drain rate (540 MB/s) exceeds fill (445.5 MB/s), so any depth is
   rate-feasible; TU stuffing burstiness favors margin.
8. **Run-to-run PnR variance (pitfall 5):** this build is 0/0 first try; re-run
   before diagnosing any future violation as a regression.

## 8. Risk comparison vs our core (summary)

| Axis | Our core (a2mega_dp_test) | Gowin stack (a2mega_dp_gowin) |
|---|---|---|
| Stream correctness | Ours to debug (scrambler-reset high-half bug was real) | Vendor-encrypted; spec conformance is Gowin's problem — but unobservable |
| PHY mode | Raw 20-bit + fabric 8b10b (production-proven) | Hardened PCS 8b10b (never yet symbol-locked on this board — WS4's question) |
| Training policy | Closed-loop ladder, full DPCD telemetry | Same (reused) — variable isolated to stream+PHY |
| Audio | Yes | No |
| Observability | Full (pre/post scrambler taps possible) | Encoder is a black box; DRP + wire-level only |
| Vendor support | None (open source) | Ticketable with a supported-stack repro — the strategic value of this build even if it fails identically |

## 9. Forensics 2026-08-19 — CR-only stall root-caused via Gowin's EDP reference design

### Hardware result (bench session, Anker DP→HDMI hub — the sink our core trains GOLDEN on, same session)

`C:8011` on every attempt across 4 configuration draws: clock recovery locks on
BOTH lanes, symbol lock NEVER. AUX conversation healthy (sink ADJUST_REQUEST
0x22 = exactly our fixed drive), symbol clock +7 ppm. Deterministic — the exact
historical "hardened-8b10b never symbol-locks (CR fine)" dead-end, now
reproduced with Gowin's own encoder feeding Gowin's own PHY.

### The new oracle

Gowin's readable EDP reference design
(`/Volumes/Storage/Downloads/Gowin_EDP_RefDesign/`, variant
`Gowin_EDP_RefDesign_4ln_2560x1440_2p7_nodesteer` — same die GW5AT-60 rev B,
same 2.7 G line rate, hardened 8b10b **including lanes 2/3**, silicon-validated,
full RTL + serdes emission + CSR in the clear). Six generator emissions were
cross-diffed register-by-register (CSRs parsed to addr→value maps,
later-write-wins):

| tag | source | lanes | mode | gear |
|---|---|---|---|---|
| A | ours, `serdes_edp/` (recovered d535df5f) | 2/3 | 8b10b | 1:1 |
| B | ref 2560x1440_2p7_nodesteer | 0–3 | 8b10b | 1:2 |
| C | our scratch generation (commit 8dc076e2) | 0/1 | 8b10b | 1:1 |
| D | our 4-lane raw regen (commit 5986989d) | 0–3 | raw | 1:1 |
| E | production raw (`gowin/60B/dp_serdes`) | 2/3 | raw | 1:1 |
| — | ref 1920x1080_1p62 + 3840x2160_5p4 (rate-register control) | 0–3 | 8b10b | 1:2 |

C is the killer datapoint: **same IDE generator, same EDP preset, same toml
(2.7 G, 1:20, 8b10b, txlev 13, pol invert), only the lane pair differs** — and
its CSR carries hardened-mode fields that A's does not.

### Verdict — generator defect (half-bond class): two 8b10b-mode fields dropped on lanes 2/3

| register | A (ours, ln2/3) | B (ref, ALL lanes) | C (scratch, ln0/1) | raw (D/E) |
|---|---|---|---|---|
| `0xNN6C` PCS lane cfg | `0x00010110` | `0x00013110` | `0x00013110` | `0x00010110` |
| `0xNNa0` lane analog | `0x00005150` | `0x00005350` | `0x00005350` | `0x00005150` |

Bits[13:12] of `0xNN6C` and bit 9 of `0xNNa0` are set in **every** hardened
emission except the generator's lanes-2/3 output — our "hardened" lanes were
carrying the **raw-mode** PCS configuration. Wire consequence: 8b10b datapath
half-enabled → transition-rich non-8b10b serial content → CR locks (TPS1's
alternating pattern survives any symbol-level garbling), symbol lock impossible
(TPS2 needs valid symbols). Explains everything at once: deterministic,
polarity-independent (both DRP polarity experiments ran on garbled-symbol
streams), content-independent (fabric-8b10b era AND matched-encoder era failed
identically), and lane-specific (every historical hardened attempt was on lanes
2/3; the one lanes-0/1 generation was inspection-only and never flashed).

**Fix applied** (commit 77da2a41): `dp_serdes.csr` — all three config-phase
writes each of `0x80946c`/`0x80966c` → `0x00013110` (mid-sequence reset writes
`0x00000110` preserved), `0x8084a0`/`0x8085a0` → `0x00005350`. No RTL, no
gear, no interface change.

### Deltas examined and deliberately NOT adopted (6-emission classification)

- `0x809468` bit 6, `0x8086xx` per-lane-entry bit 8, `0x808930`
  (`0x41a2`→`0x42a2`): **tx_gear 1:2** markers (B-only). Gowin ships hardened
  8b10b only at 1:2 gear (all five ref variants), but C proves the generator
  intends 1:1 hardened on lanes 0/1 with our exact bits — gear migration is
  the documented fallback, not applied.
- `0x80943c` bit 10 (`0x408` vs ref `0x8`): **tx_pol_invert** — board P/N
  swap, keep. (Raw-mode pad probe 2026-08-13 proved the IP invert nets true
  end-to-end.)
- `0x808824`/`0x808838` (`0x1` vs `0x13`), `0x808888/8c`: lanes-0/1-enable
  arrays (D raw-4-lane also has `0x13`) — position, keep.
- `0x808760` bit 10, `0x808000`-vs-`0x808104` active-CMU hosting,
  `0x8007a6`/`0x8009a6`, `0x801800`-vs-`0x801880` blocks, `0x800400` lane
  codes: **refpad1/CMU position plumbing** — keep (production identical).
- `0x800b91` bit 9: `cmu_reset_by_fabric` toml choice (true in ours AND
  production) — keep.
- `pcs_tx_clk_src` 2-vs-1: position-dependent plumbing (pair 0/1 emits 2,
  pair 2/3 emits 1 under both IPs — 8b39a9ee correction) — keep.
- `0xNNdc = 0x31` family: strides 0x200/lane (`0x8086dc`/`0x8088dc` ARE our
  lanes 2/3) — already present, no action. (First-pass lane-block mapping got
  this wrong; presence re-verified per-family.)
- Drive delta vs production (txlev 13/FFE auto vs 15/manual 32:8, regs
  `0x808434/438`): sink requests 0x22 = our current 804 mV; deterministic
  0-for-N on the most tolerant sink ≠ the probabilistic margin signature;
  **secondary candidate only** — revisit if symbol lock still fails after the
  CSR fix.

### Ranked fix list (as applied)

1. **CSR 8b10b-enable patch (APPLIED)** — the four writes above. Confident:
   three independent hardened emissions agree, two of them on this exact die
   position or this exact board+toolchain.
2. **`I_vb_id` 0x00 → 0x10 (APPLIED)** — AudioMute_Flag for the no-audio
   stream; the reference `test_top.v` drives `8'h10`. Not symbol-lock
   related; contract correctness.
3. Drive matching production (900 mV + FFE 32:8) — NOT applied (see above).
4. Gear 1:2 / X40 migration to the reference geometry — NOT applied; fully
   mapped (encoder `TX_X40_MODE`/`SYM_WIDTH 32`/`COEF 2`, shim regen from
   `SERDES_IP/IPlib/EDPPHY/data` templates + defines
   `LINE2_EN/LINE3_EN/TX_X40_MODE/RX_X40_MODE/DRP`, CSR gear bits per table
   above, strm PLL ÷2). The nuclear option if 1 fails on hardware.

### Reference-design contract cross-checks (priorities 2–4, all clean)

- **pll_init.v**: the 398-line MDRP ceremony is `PLL_INIT` — the standard
  GW5A fabric-PLL init inside their `Gowin_PLL`/`pix_pll` IP wrappers (mdclk
  port). Our PLL emissions carry the same engine; no SERDES involvement.
- **Encoder driving contract** (`aux_tx.v`): tps1_en rises with the DPCD
  0x102=0x21 write, tps1→tps2 and tps2→tps3 switch on the same edge as the
  0x102 rewrite (no gaps, no overlap games), video_data_en rises when
  0x102=0x00 lands; ~1 ms dwell per AUX command, advance on ACK only, **no
  pass/fail gating** (open-loop walk). Our ladder's
  tx_clock_train/tx_align_train/tx_link_established mapping is
  contract-equivalent; nothing to change.
- **Reference reset shape**: encoder `I_rst_n` and PHY `tx_rst` released
  together (rst_n2), `fabric_rstn` 40 ms earlier, `tx_vld` tied 1 (their
  FIFO never sees a stuffed-while-held state because the encoder is in reset
  for the whole tx_rst hold). Our tx_vld write-gate achieves the same
  lockstep start differently; keep ours (their tie-1 idiom failed on this
  hardware in build #1).

### §5 addendum — DP Aux IP v1.0 unblocked: it is the reference `auxlink/` RTL, APB-wrapped

The reference design publishes the Aux engine **in the clear** and the port
correspondence is exact:

- IP TX face `O_tx_frame_type/O_tx_scrm_value/O_tx_tps1/2/3_en/
  O_tx_video_data_en/O_tx_end/O_tx_aux_is_ok` ≡ `aux_tx.v` outputs
  (name-for-name; IP adds `O_tx_lane_num[2:0]`). `IO_tx_man_p/n` +
  `O_tx_man_test` ≡ `manchester_encode.v` pads. `FREQ` parameter ≡ their
  aux-clock MHz parameter.
- IP RX face ≡ `aux_rx.v` + `manchester_decode.v` (wrapper port names
  `dec_frame_type/dec_scrm_value/dec_tps1..3_en` match aux_rx outputs), plus
  `O_rx_hpd` (internalizes test_top's reset-driven HPD) and `O_rx_irqn`.
- Generation defines (from the `dp_aux_top.v` template in the IDE bundle):
  `TX_AUX_ENABLE`, `RX_AUX_ENABLE`, `DEBUG_ENABLE` (4×32-bit taps each way),
  `DEF_RX_FREQ`, `module_name` — same encrypted-wrapper+defines pattern as
  the encoder, so headless generation is available.
- **What the "autonomous link policy" actually is** (from `aux_tx.v`): a
  76-entry fixed command ROM — 250 ms wait; DPCD 0x600=0x01; read 0x200,
  0x00000–0xF caps; EDID via I2C-over-AUX addr 0x50 in 16-byte MOT chunks;
  0x600=0x01; 0x100=0x0A (HBR); 0x101=0x84 (enhanced + 4 lanes,
  TPS3_SUPPORTED noted); 0x102=0x21→0x22→0x23→0x00 with 0x103–0x106 drive
  updates computed from 0x206/0x207 reads
  (`{2'b0, preemp[1:0], 1'b0, swing[1:0]}` per lane); `aux_is_ok` =
  0x202/0x203 == 0x77/0x77. **Open-loop**: it never checks CR/EQ pass/fail,
  never retries, never varies rate — an eDP-panel walk, not an adaptive
  ladder. The scrambler-seed choice `O_scrm_value` = DPCD cap 0x0000D bit 0.
- The DRP master port is the IP's addition (the readable reference never
  touches DRP), as is the APB slave. Given the engine runs the fixed walk
  autonomously, the APB map is most plausibly control/status/debug rather
  than required servicing.
- **Adoption verdict sharpened**: the IP's policy tier is *weaker* than our
  closed-loop ladder for converter-class sinks (no retrain, fixed
  4-lane/enhanced-framing constants in the reference ROM). Keep our ladder;
  the IP remains interesting only as a Gowin-supported repro vehicle for
  tickets.

### Rebuild provenance (fix build)

| | |
|---|---|
| Base | `c280bf5e` (tip of `claude/epic-lovelace-54ee14`) + fix commit `77da2a41` |
| Toolchain | Gowin V1.9.12.03, `gw_sh` pipe method, `GPRJ=a2mega_dp_gowin.gprj` |
| Timing | **0 setup / 0 hold; TNS 0.000 all domains.** Fmax: clk_sym 145.612 ≥ 135.007, clk_strm 160.463 ≥ 74.25, clk100 133.6 ≥ 100, clk50 103.2 ≥ 50, cm_life 147.7 ≥ 100 |
| `a2mega_dp_gowin.fs` sha256 | `6cfa7a4d2d1a7f06f0359e2e2cb99fb8af927c72d9d7afce37899626ad73eb1b` |
| `a2mega_dp_gowin.bin` sha256 | `c83456b8afbcf5475300e45d22588246f6d22339ef2fe9d69a682b585ab650e8` (gitignored) |
| SECURITY_BIT | OFF (`//SecurityBit: OFF` verified in the .fs) |
| Hardware check | `CR` idx 10 → `0x80946c` must read `0x00013110` (was `0x00010110`); then C: must progress past 0x8011 |

Gowin-ticket material (strengthened): the EDP PHY generator emits raw-mode PCS
lane configuration for lanes 2/3 under the EDP preset (this defect), on top of
the known half-bond emission defect — both reproducible from the shipped IDE.

## 10. Geometry migration 2026-08-19 — silicon falsified 1:1-gear hardened 8b10b; moved to the reference 1:2/X40

### Hardware result of the §9 CSR-fix build (bin `c83456b8`, board B1)

- **Fix verified in the quad**: `CR` idx 0A reads `0x80946C = 0x00013110`.
- **New failure mode**: fabric word clock measures **~60.75 MHz** (Q: samples
  60,747,983..60,761,166; rock-solid 135.001 M pre-fix). D3 (freq_ok) dark,
  `S:` mostly 0x38 / occasionally 0xBF, ladder cycles clock-training→error,
  C:8000 (no CR — line effectively broken).

**60.75 MHz = 135 × 9/20 = 67.5 × 9/10 exactly.** Reading: with the
8b10b-mode enables repaired but the gear registers still at 1:1, the PCS
divider tree is half-migrated — it lands at the 1:2-gear clock (67.5 M)
scaled by a spurious 9/10 (a divider pair programmed for one geometry driven
by the other; the exact register pair needs GTR12 documentation — ticket
question). The §9 inference error: the lanes-0/1 scratch emission (C) was
**inspection-only, never flashed** — it proved the generator *emits* 1:1
hardened configs, not that they *work*. They are broken by construction:
**Gowin ships hardened 8b10b only at 1:2 gear** (all five refdesign
variants), and that is load-bearing.

### Migration applied (commit 68a30cc4) — all headless, mirroring the reference arrangement

| piece | change |
|---|---|
| CSR gear registers | the 8 classified gear markers → reference values: `0x809468`/`0x809668` `0x133→0x173` (bit 6); entries `0x808610` `0x201A→0x211A`, `0x808618` `0x301A→0x311A`, `0x808630` `0x206→0x216`, `0x808638` `0x306→0x316` (bit 8); quad `0x808930` `0x41A2→0x42A2` + early `0x808918` `0x41FF→0x42FF`. Reset choreography, invert (`0x80943c` bit 10), refpad/CMU, FFE, lane-enable state untouched. |
| EDP PHY shim | regenerated headlessly: IDE template trio (`ipcore/SERDES_IP/IPlib/EDPPHY/data/`) + hand-written defines `LINE2_EN/LINE3_EN/TX_X40_MODE/RX_X40_MODE/DRP`, `module_name edp_phy` (same defines shape as the refdesign's own `temp/edp_phy` generation), scratch GowinSynthesis run, then 696 chip-top IBUF/OBUF mechanically rewritten to assigns (the IP generator's own emission form). Port names identical; faces now 32-bit+4K. |
| dp_serdes.v | user-face ports + edp_phy instance connections widened (16 substitutions); GTR12/upar_arbiter untouched. |
| Encoder | `TX_X40_MODE`, `DEF_TX_SYM_WIDTH 32`, `DEF_TX_BPP_COEF 2` (= refdesign encoder defines modulo 2-lane/TPS set). 48-bit pix ports = pixel pairs; `I_ls_clk` remains the pcs_tx fabric-clock loopback, now 67.5 MHz — exactly test_top.v's arrangement. |
| Clocks | word clock 67.5 MHz (2.7 G / 40); strm PLL 67.5 × 44/2 = 1485 MHz VCO / 40 = **37.125 MHz** (4 px/clk = 148.5 Mpx/s). First attempt 88/5÷32 tripped PA2078 — **PLLA PFD floor is 19 MHz**, 67.5/5 = 13.5 under it; 44/2 gives PFD 33.75. |
| Top | colorbars in pixel-quad units (550/11/48/528), ports {hi,lo} pairs; freq_ok window → 66.15..68.85 M; led[2] = clk_sym==67.5 M. |
| SDC | clk_sym 14.815 ns (same GTR12 LANE2 anchor), clk_strm 26.936 ns. |
| MVID/NVID | **unchanged** (288358/524288): the DP convention is pixel-rate / link-symbol-rate (270 M) — fabric gearing does not enter it. |
| toml sidecar | ln2/ln3 `tx_gear_rate`/`rx_gear_rate` → "1:2". |

X40 TX packing audited in the synthesis netlist: `FABRIC_LN2/LN3_TXDATA_I`
low 40 bits = `{0, k[i], byte[i]}` × 4 (upper 40 grounded) — the hardened
format, extending the audited X20 form; encoder lane 0 → die ln3.

### Rebuild provenance (geometry build)

| | |
|---|---|
| Base | `c280bf5e` + `77da2a41` (CSR 8b10b fix) + `68a30cc4` (geometry) |
| Toolchain | Gowin V1.9.12.03, `gw_sh` pipe method, `GPRJ=a2mega_dp_gowin.gprj` |
| Timing | **0 setup / 0 hold; TNS 0.000 all domains.** Fmax: clk_sym 164.868 ≥ 67.499, clk_strm 175.150 ≥ 37.125, clk100 114.6 ≥ 100, clk50 123.8 ≥ 50, cm_life 147.4 ≥ 100 |
| Resources | 3717 LUT / 2739 FF / 8 BSRAM / 2 PLLA |
| `a2mega_dp_gowin.fs` sha256 | `3f458b4d54194e4df15ec655f414ca08088ea399b9eb8675f668749ec7d47e07` |
| `a2mega_dp_gowin.bin` sha256 | `0f416d7b6228abbb626d587df83339203a45f6a6c932faad0bf55770ec92d2a5` (gitignored) |
| SECURITY_BIT | OFF (`//SecurityBit: OFF` verified in the .fs) |
| Hardware checks | Q: ≈ 67,500,000 ±2% (was 60.75 M broken / 135.0 M pre-fix); D3 lit; `CR` idx 0A still `0x00013110`; then C: must progress past 0x8011 |

Gowin ticket addendum: the EDP PHY generator offers TX User Clock Ratio 1:20
(1:1 gear) under the hardened-8b10b EDP preset for **both** lane pairs, but
the emitted configs produce a broken word clock once the (also mis-emitted,
§9) mode enables are corrected — 1:1 hardened appears unimplemented in
silicon or unreachable by the emitted divider programming; every shipped
reference design uses 1:2. Two stacked generator defects total.
