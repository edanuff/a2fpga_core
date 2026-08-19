# WS4 — DP output on Gowin's own IP stack (a2mega_dp_gowin)

**Status: BUILT, timing-clean, ready to flash. Not yet hardware-tested** (bench owned
by another session at build time).

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
