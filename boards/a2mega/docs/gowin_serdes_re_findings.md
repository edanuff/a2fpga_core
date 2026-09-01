# gowin-serdes (key2) reverse-engineering findings — cross-referenced for a2mega DP

**Source repo:** `/Users/edanuff/GitHub/gowin-serdes` — key2's pure-Amaranth reimplementation
of the Gowin GTR12 SerDes IP (GW5AT-15/60, GW5A(S)T-138), built from decrypted vendor IP
(`gw/` reference tree, IEEE P1735 decryptor) and byte-exact TOML/CSR reproduction of the
Gowin USB3.1 reference design. Their arbiter is claimed cycle-exact against the decrypted
vendor `upar_arbiter.v` under cocotb (`example/gw5ast-138/usb31-enum/README.md:18`).

**Caveat:** the local clone does **not** contain the `gw/` (decrypted vendor IP), `gen/`
(IDE-generated references), or `tests/` directories that `ARCHITECTURE.md:560-580` describes
— only the Python package, examples, and docs. All "cycle-exact"/"byte-identical" claims are
theirs, not locally re-verifiable. Everything below cites what is actually in the clone.

**Our context:** GW5AT-60 (Q0 lanes 2+3) / GW5AST-138 (Q0 lanes 1+2), 2.7 Gbps ×2 DP
source, raw-20, 135 MHz refclk on Q0 REFPAD1, driven through the vendor `dp_serdes`
wrapper (`boards/a2mega/hdl/gowin/{60B,138B}/dp_serdes/dp_serdes.v`) with a fabric CSR
replay ROM (349/339 writes) and M5 runtime AFE writes.

---

## 1. cm_life (FABRIC_CM_LIFE_CLK_O) derivation — VERDICT

**Verdict: free-running on-die oscillator, NOT refclk-divided, NOT a fixed VCO/N.
No CSR register controlling it is identified anywhere in their RE material. Frequency is
config- and die-dependent within a measured 56–118 MHz envelope. Our measured-honest
15.0 ns constraint for our config is the right method; their/vendor blanket 10 ns has no
provenance and does not even bound their own 118 MHz measurement (8.47 ns).**

Evidence, in order of authority:

1. **Their hardware measurement** — `gowin_serdes/bench.py:745-747` (ClockFreqProbe
   docstring): *"the upar/DRP 'life clock' of the GTR12 was measured to be a free-running
   ring oscillator (~56..118 MHz); this probe measured it"*. Same wording in the SDC
   comment `gowin_serdes/dkusb_gw5at60.py:459-461`: *"upar = GTR12 life clock, a
   free-running ring oscillator measured 56..118 MHz -- 10 ns bound"*. This is a real
   frequency-counter measurement (request/ack snapshot CDC, §5.5) on GW5AT-60 hardware
   across their configurations.

2. **The competing "refclk/2" claim is vendor folklore, not measurement** —
   `dkusb_gw5at60.py:399`: *"upar = quad life clock = refclk/2 = 100 MHz on this board
   (200 MHz refclk; the vendor top.sdc also uses 10 ns)"*. This annotates the **vendor**
   usb31_enum config and simply repeats the vendor's top.sdc assumption. Their own probe
   (item 1) postdates and supersedes it. Note 200/2 = 100 MHz is not even inside their
   measured 56..118 span's endpoints for the two boot trims they ran.

3. **Their 138K examples assume ~62.5 MHz and the UART works** — `example/gw5ast-138/
   eidle/top.py:62` (`UPAR_FREQ = 62_500_000`, divisor for a working 115200 UART bridge),
   `usb31-enum/usb_debug.py:3-5`. Async UART tolerates ≈±4.5% → the 138 die's life clock
   under their configs is 62.5 ±3 MHz. With a 125 MHz refclk that coincidentally equals
   refclk/2 — but our board refutes the /2 rule (next item).

4. **Our measurement refutes refclk/2 and confirms oscillator behavior** — our refclk is
   135 MHz → /2 would be 67.5 MHz; we measured 60.3-60.4 MHz cooling to 60.1 warm
   (−1.7% thermal drift). A crystal-refclk-derived clock cannot drift with temperature;
   a ring/RC oscillator must. Our SDC already records this
   (`boards/a2mega/hdl/a2mega_138b.sdc:48-56`). Their 138K ≈62.5 and our ≈60.3 with
   *different* refclks (125 vs 135) is exactly what a refclk-independent ~60 MHz
   oscillator class looks like.

5. **Config-dependence is real but unexplained** — their 56 vs 118 MHz endpoints were on
   the *same* DK60 board (fixed 200 MHz refclk) across boot trims (5G Gen1 vs 10.3G Gen2
   class), a ≈2× ratio. Nothing in `csr_map.py`, `toml_gen.py`, or `config.py` names a
   life-clock trim/divider CSR. Hypothesis (unproven): an internal ÷2 engages per
   rate-class config. Our 2.7G/raw-20 config lands in the "slow" class (60.3), consistent
   with their Gen1-class 56-62 observations and with our replay writing the 5G-class PLL
   bandwidth value (0x808120 = 0x1A, see §2).

6. **A fabric divider input exists on the 138 die (untested lever)** —
   `gowin_serdes/primitives.py:184`: `("FABRIC_CLK_LIFE_DIV_I", "i", 2)` in
   `QUAD_PORTS_QUAD_ONLY` (GW5A(S)T-138 only). Both their builder and our vendor wrapper
   tie it to 0 (`quad.py` default-GND path; our `138B/dp_serdes/dp_serdes.v:397`
   `.FABRIC_CLK_LIFE_DIV_I({gw_gnd,gw_gnd})`). The name strongly suggests a fabric-
   selectable life-clock divider. If cm_life timing ever becomes the limiter, driving this
   nonzero is a one-wire experiment that could halve/quarter the domain clock — flag as
   **unverified**; nobody (them or us) has exercised it. A second life clock also exists:
   `FABRIC_CM1_LIFE_CLK_O` (CMU1's, `primitives.py:112`), unused by both stacks.

**Constraint strategy conclusion:** keep the measured 15.0 ns (66 MHz = 60 MHz +10%
guard) on our shipped configs. Add a standing rule: **re-measure cm_life whenever the
SerDes IP is regenerated or the rate/PLL configuration changes** — their data proves the
frequency can move ×2 with configuration, and in the fast class (≈118 MHz) even the
vendor's 10 ns would under-constrain. Their common alternative — blanket
`set_false_path -from/-to life_clk` (`example/gw5ast-138/eidle/gw5ast_dvk.py:394-397` and
every raw/csr_tuner variant) — is only sound for designs whose life-domain logic is
trivial and fully handshake-decoupled; **not** for us, because our CSR replay engine and
AFE player *run* in the cm_life domain and need real intra-domain analysis.

---

## 2. CSR map cross-reference — decoding our replay ROMs

### 2.1 Address model (their `gowin_serdes/csr_map.py:14-71, 872-877`)

| Bits/base | Meaning | Cite |
|---|---|---|
| `0x80xxxx` / `0x90xxxx` | Quad selector: Q0 / Q1 | csr_map.py:14-19 |
| `0xC00000` (+`0xC1` for Q1) | UPAR controller block | csr_map.py:21-26, 35 |
| `0xB00000` | GLOBAL block | csr_map.py:34 |
| `q<<16 \| 0x8000` | QUAD_CTRL (quad-common control) | csr_map.py:36 |
| `q<<16 \| 0x8100` | QPLL0 / CMU block | csr_map.py:36 |
| `q<<16 \| 0x8200 + lane*0x100` | **AFE** per-lane (lane0=0x808200 … lane3=0x808500) | csr_map.py:62 |
| `q<<16 \| 0x0200 + lane*0x200` | **CDR** per-lane (0x800200/400/600/800) | csr_map.py:65 |
| `q<<16 \| 0x9000 + lane*0x200` | **PCS** per-lane (0x809000/200/400/600) | csr_map.py:68 |
| `q<<16 \| 0x0300 + lane*0x200` | **LN_CTRL** per-lane (0x800300/500/700/900) | csr_map.py:71 |
| `q<<16 \| 0xA020 + lane*0x200` | **CPLL** per-lane divider (0x80A020/220/420/620) | csr_map.py:872 |
| `q<<16 \| 0x8B34 + lane*0x100` | RXDET result read | csr_map.py:105 |
| `q<<16 \| 0x8600 + lane*0x8` | TX clock source select | csr_map.py:874 |
| `q<<16 \| 0x8620 + lane*0x8` | RX clock source select | csr_map.py:875 |

This model instantly explains our per-lane families: 138B ROM's `0x8083xx/0x8084xx` =
AFE lanes 1+2 (our 138B DP pair), 60B ROM's `0x8084xx/0x8085xx` = AFE lanes 2+3 (our 60K
pair); `0x8092xx/0x8094xx` = PCS lanes 1/2; `0x80a2xx/0x80a4xx` = CPLL lanes 1/2, etc.

### 2.2 The AFE/FFE trio — their names CONFIRM our reverse-engineered bit meanings

| Addr (AFE base+) | Their name | Their bit decode | Our decode | Match |
|---|---|---|---|---|
| +0x34 | `CSR_TX_FFE_0` (csr_map.py:75-78) | txlev in [15:12] of 0x808234 (config.py:120-121) | txlev<<12 | ✅ register+field (encoding caveat below) |
| +0x38 | `CSR_TX_FFE_1` (csr_map.py:81-84) | "cm in [4:0] and c1 in [12:8]"; c0 implicit = 40−cm−c1 (config.py:114-121) | C1[12:8] \| CM[4:0] | ✅ exact |
| +0xD8 | `CSR_TX_FFE_2` / `TX_FFE_VDDT` (csr_map.py:85-88, 759) | "always reloaded with 0x00000110 to latch the new coefficients" (csr_map.py:829-830); toggled 0 → 0x110 in `ffe_sequence` (csr_map.py:404-424) | bit4 manual-enable, bit8 strobe | ✅ 0x110 = bit8\|bit4; they treat it as the commit/latch register — same mechanism, coarser model |

Additional confirmations and cautions:

- **FFE coefficient constraint**: cm + c0 + c1 = 40, c0 ≥ 20, cm/c1 ≤ 19
  (config.py:142-157). Full swing = 40 units; de-emphasis dB = 20·log10((c0−…)/40)-style —
  useful for converting our DPEQ ladder steps to coefficient space.
- **Their proven USB3 de-emphasis point is FFE_1 = 0x0805** (c1=8, cm=5)
  (csr_map.py:421) — exactly our `sw15_cm5_c1_8` sweep point. Their LFPS/full-swing
  point is FFE_1 = 0 (csr_map.py:812-848).
- **txlev encoding caveat**: our 138B TOML has `txlev = 13` for the enabled lanes
  (`serdes_tmp.toml:182,264`), yet the generator emits `0x808334/0x808434 = 0x0000E000`
  ([15:12] = 14) and `FFE_1 = 0x0B00` (c1=11, cm=0) in auto mode
  (`138B/dp_serdes/dp_serdes.csr:254-259`). So the [15:12] field is *not* literally
  txlev (13 → 0xE): either the tool maps txlev→drive-code +1, or auto mode overrides.
  Our hardware-verified M5 rule (txlev<<12) still stands for *runtime* writes — but when
  comparing against generator output, expect the off-by-one. Also: TOML `vddt` is **not
  mapped to any CSR by the Gowin tool** (config.py:122-123) — metadata only; the
  registers are the only real lever, confirming our M5-writes approach.
- Their USB3 flow's extra boot write `AFE+0xF8 = 0x00000A02` is named "lane TX-AFE
  tuning" (usb3.py:112-124; `example/gw5at-60-dkusb/README.md` "0x8083F8 = 0xA02") —
  an AFE register we've never touched; candidate for the strict-converter campaign if
  txlev/FFE alone tops out.

### 2.3 Decode of the specific replay entries asked about

| Our entry (ROM) | Block per their map | Decode / evidence |
|---|---|---|
| `0xb00000 = 0x00ffaa55` (first write, both dies) | GLOBAL (csr_map.py:34) | No field detail in their RE. Position (always first) + value pattern (…AA55) → boot unlock/magic for the CSR sequencer. Keep first in any replay. |
| `0xc00008 = 3 → 2 → 1` (138B idx 9/168/283; 60B idx 17/176/279) | UPAR_CTRL (csr_map.py:35) | Unnamed by them. Written three times with descending values at phase boundaries of the blob → boot-sequencer phase/bank select. **Order-sensitive; never reorder across these writes.** |
| `0x800b91`, `0x800bfa` | *Unmapped* — above LN_CTRL lane3 (0x800900) and below QUAD_CTRL (0x808000) | Not in their model at all. Values are die-specific (138B: f700/20000; 60B: fd00/20000) → per-die analog trim words. Treat as opaque, replay verbatim. |
| `0x8081a4/a8, 0x8081b0/b4/b8` (60B only: idx 0-1, 14-16) | QPLL0/CMU block (csr_map.py:36) | 60-die-only entries. The GW5AT-60 QUADA has 4 ref pads + refimux0..3 (primitives.py:169-176 `QUAD_PORTS_QUADA_60_ONLY`; toml_gen.py:164-173 extra `ref_pad2/3`, `refimux2/3`) — these are the extra refclk-path/CMU config the 138 die doesn't have. Quad-common (CMU), not per-lane. |
| `0x808120 = 0x1A` (138B idx 153) | QPLL0/CMU | **Named by them: "PLL bandwidth (5G)"** — identical value in their 10G→5G rate-switch table (csr_map.py:362) and as the Gen1 CPLL divider value (csr_map.py:880). Confirms our 2.7/5.4G-class config runs the 5G-class PLL setting → consistent with cm_life landing in their Gen1-class 56-62 MHz band. |
| `0x808758 = 0xc0` (60B only), `0x808760/764` (both dies), `0x908760/764` (138B) | QUAD_CTRL | Refclk input-mux configuration. Evidence: values track refimux settings — 60B `808760=0x00015410` with `refimux0_sel=2`; 138B Q0 `808760=0x0001d410` with `refimux0_sel=3`; 138B Q1 (disabled) `908760=0x00015010` with `refimux0_sel=0`. `0x808758` exists only on the 60 die (extra refimux2/3). The Q1 (`0x90876x`) writes on 138B are the generator emitting the disabled quad's mux defaults — harmless, keep them. Quad-common. |
| `0x808x84 = 0x100` (60B idx 3-6, all four lanes) | AFE per-lane (+0x84) | Unnamed in their map. Written for **all** lanes on 60B; absent on 138B. AFE housekeeping default for the QUADA die. |
| `0x808x98 = 0x1` (138B idx 12-13 → lanes 1,2; 60B idx 20-21 → lanes 2,3) | AFE per-lane (+0x98) | Unnamed. Written **only for the active lanes**, matching each die's DP pair exactly → per-lane analog/driver enable. If we ever add a lane, this needs a matching write. |
| `0x808900…0x80891c` (8-word table, identical both dies) | QUAD_CTRL region (between their `ln_ctrl_shared` 0x8830 and `rxdet` 0x8B34) | Unnamed. Die-independent constant table (1, 50ff, 5f00, 6080, 5800, 6001, 41ff, a000) → quad-common calibration/sequencer table. |
| `0x80882c/840/854/868 = 0x00707120` (stride 0x14, 4 lanes, 138B tail) | QUAD_CTRL, adjacent to their `LN_CTRL_shared` `0x808830 = 0xFFFFF9FF` (csr_map.py:167, 338) | Per-lane control words in the shared quad-ctrl area. |
| `0x8003a9/0x8005a9/0x8007a9 = 0x8000` | LN_CTRL +0xA9, lanes 0-2 | Unnamed neighbors of their named EIDLE register (LN_CTRL+0xA4). |
| `0x808610 = 0x201a`, `0x808630/634` (138B tail) | TX/RX clock source select, lane 2 (csr_map.py:874-885: gen1 TX=0x11A, gen2 TX=0x21A at lane 0) | Our lane-2 TX clk select carries the same 0x?1A pattern family. |
| `0x809x0c/…/0x809x70` families | PCS per-lane | Their named PCS registers: base+0x00 = PCS rate mode (1=Gen1, 3=Gen2, csr_map.py:886); 0x809000-word = width/gearbox (5G/20-bit/1:2 = 0x511; 10G/16-bit/1:4 = 0xF11, csr_map.py:366,381); +0x08 = RX polarity (csr_map.py:114-117); +0x68 = 8b10b bypass (csr_map.py:110-112). |
| `0x808104/0x808000/0x80a204/0x80a404 = 0x00022322` (138B idx 1-4) | CMU +0x04, QUAD_CTRL +0x00, CPLL lanes 1+2 +0x04 | Same value into every PLL block's +0x04 → common PLL/refclk configuration word (refclk divider class). 60B writes CPLL lanes 2+3 (0x80a404/0x80a604) instead — per-die lane substitution, as expected. |

### 2.4 Runtime registers we are not using yet (from their PIPE layer)

| Register | Address | Values | Cite |
|---|---|---|---|
| TX electrical idle | LN_CTRL+0xA4 (`0x8003A4+lane*0x200`) | 1 = idle, 7 = active | csr_map.py:93-95, 797-798 |
| LFPS pulse / RXDET pulse | LN_CTRL+0x3F (`0x80033F+lane*0x200`) | 0x03000000 pulse on, 0 off | csr_map.py:97-100, 800-801 |
| RXDET result (read) | `0x808B34+lane*0x100` | receiver-detect status | csr_map.py:102-105 |
| Loopback mode | CDR+0x56 | PCS/PMA/NEAREND | csr_map.py:153-156, 199 |
| CDR loop filter init | CDR+0x53/5E/5F/54/60/61 | 0x7F000000 / 0x007F0000 / 0x7F000000 / 0x4F / 0x4F / 0x4F00 | csr_map.py:317-346 |
| Quad-shared CDR / lane ctrl | `q\|0x83F8` = 0x00038002, `q\|0x8830` = 0xFFFFF9FF | init values | csr_map.py:160-168, 338-339 |

The USB3 PHY's own 11-write power-on init (FFE trio + CDR set, csr_map.py:317-346) is a
strict subset of what our full-blob replay already covers.

---

## 3. UPAR protocol + arbiter — and replacing the vendor `upar_arbiter_wrap`

### 3.1 Protocol (client/DRP side)

From their cycle-exact reimplementation (`gowin_serdes/upar_arbiter.py`) and bridge FSM
(`example/gw5ast-138/eidle/top.py:276-328`):

- All DRP clients are clocked by the life clock: `drp_clk[j] = upar_clk` fanned out
  combinationally (upar_arbiter.py:75-77; identical `assign drp_clk_o[n] = upar_clk_i`
  in our plaintext wrap, `138B/dp_serdes/upar_arbiter/upar_arbiter.v:3572-3579`).
- **Request**: drive `addr` + (`wren`+`wrdata`+`strb=0xFF`) or `rden`, and **hold them
  asserted** until the response — the arbiter samples in IDLE and then spends **3 cycles
  in JUDG_ADDR** re-latching the winner's address/data (upar_arbiter.py:224-231; their
  bridge comment top.py:294-296: "Keep wren/rden asserted so the arbiter can latch them
  during the JUDG_ADDR phase (3 cycles)"). Our engine already holds — good.
- **Response**: `ready` (write) / `rdvld`+`rddata` (read) pulse for one cycle,
  registered; `resp` is always 0 (address validation never fires,
  upar_arbiter.py:249-253; our wrap hard-grounds `drp_resp_o`, upar_arbiter.v:3580-3588).
- **FSM**: `IDLE → JUDG_ADDR (3 cyc) → UPAR_EN (until ready/rdvld) → WAIT (1) → IDLE`
  (ARCHITECTURE.md:289-297). Minimum ≈6 life-clk cycles per write → our 339-write replay
  ≈ 2-3 k cycles ≈ 40 µs at 60 MHz, matching observed behavior.
- **Macro-side bus**: `UPAR_ADDR_S[24]/WRDATA_S[32]/STRB_S[8]/WREN_S` held until
  `UPAR_READY_S`; `RDEN_S` until `RDVLD_S`; constants `UPAR_RST = 0`,
  `UPAR_BUS_WIDTH_S = 0` (= 32-bit) (upar_arbiter.py:309-310; our wrap
  upar_arbiter.v:3589-3590 ties the same).
- **Arbiter POR**: internal 16-cycle counter after GSR before any arbitration
  (upar_arbiter.py:99-106) — no fabric reset needed or provided.
- **Their addition**: a 2^20-cycle (≈16 ms) UPAR timeout that abandons a hung
  transaction (upar_arbiter.py:166-169, 207-215) — the vendor arbiter can hang forever.
  Worth copying into anything we build.

### 3.2 Can we replace the vendor arbiter? — YES, two tiers

**Key structural fact from our own tree:** the outer
`\~upar_arbiter_wrap.dp_serdes` module is **plaintext** in our vendor file
(`boards/a2mega/hdl/gowin/138B/dp_serdes/upar_arbiter/upar_arbiter.v:3497-3590`) — it is
pure flat-bus↔array unpacking plus the clk/resp/rst/bus_width ties; only the inner
`\~upar_arbiter.dp_serdes` FSM is encrypted (lines 10-3496). The full interface is
therefore known, and their repo documents the FSM's exact behavior. Our top-level
`dp_serdes.v` grounds 7 of 8 DRP slots and connects our single fabric client to **slot 1**
(`drp_addr_i[47:24]`, `drp_wren_i[1]` — dp_serdes.v:640-646), matching their
`arbiter_slot = quad*4 + first_lane` = 0·4+1 rule (group.py:62). The 60B uses slot 2
(quad*4+2) by the same rule.

**Tier 1 — SDC-only, vendor-sanctioned (recommended first): the vendor's own multicycle
set for these exact paths.** Their `gw5ast-138` platform quotes, with attribution
*"exact pin names from working PCIe project"* (Gowin's own PCIe reference design), the
following (`example/gw5ast-138/eidle/gw5ast_dvk.py:348-379`; identical block in
`usb31-enum/gw5ast_dvk.py:377-408`):

```
set_multicycle_path 2 -setup -from [get_pins {…/u_upar_arbiter/upar_wren_o_s1/Q}]
set_multicycle_path 1 -hold  -from [get_pins {…/u_upar_arbiter/upar_wren_o_s1/Q}]
set_multicycle_path 2 -setup -from [get_pins {…/u_upar_arbiter/upar_rden_o_s1/Q}]
set_multicycle_path 1 -hold  -from [get_pins {…/u_upar_arbiter/upar_rden_o_s1/Q}]
set_multicycle_path 2 -setup -from [get_pins {…/gtr12_upar_inst/UPAR_READY_S}]
set_multicycle_path 1 -hold  -from [get_pins {…/gtr12_upar_inst/UPAR_READY_S}]
set_multicycle_path 2 -setup -to   [get_pins {…/u_upar_arbiter/upar_addr_o_*_s1/D}]
set_multicycle_path 1 -hold  -to   [get_pins {…/u_upar_arbiter/upar_addr_o_*_s1/D}]
set_multicycle_path 2 -setup -to   [get_pins {…/u_upar_arbiter/upar_strb_o_*_s1/D}]
set_multicycle_path 1 -hold  -to   [get_pins {…/u_upar_arbiter/upar_strb_o_*_s1/D}]
set_multicycle_path 2 -setup -to   [get_pins {…/u_upar_arbiter/upar_wrdata_o_*_s1/D}]
set_multicycle_path 1 -hold  -to   [get_pins {…/u_upar_arbiter/upar_wrdata_o_*_s1/D}]
```

This is **exactly our 138B cm_life wren→addr-CE path family**, blessed as 2-cycle by
Gowin's own PCIe reference project. Semantics justify it: addr/wrdata/strb are latched at
the end of JUDG_ADDR and then held stable through the whole UPAR_EN handshake, and wren is
registered — data at the `*_s1` capture registers is multi-cycle-stable by construction
(upar_arbiter.py:255-297). For our netlists the hierarchy prefix becomes
`i_dp/i_transceiver_bank/i_dp_serdes[/i_dp_serdes_138b]/upar_arbiter_wrap_dp_serdes_inst/u_upar_arbiter/`
and `…/gtr12_upar_inst/UPAR_READY_S` — verify the `_s1` register names survive in our
PnR netlist (encrypted-source register names do appear in Gowin timing reports). Effort:
hours, zero RTL risk. This likely retires the 138B cm_life timing family outright.

**Tier 2 — replace the arbiter with fabric RTL we control.** Feasible and de-risked by
their work:

- *What exists*: a complete, documented reimplementation (`upar_arbiter.py`, 359 lines of
  Amaranth ≈ 200 lines of SystemVerilog), claimed cycle-exact vs the decrypted vendor RTL
  under cocotb (usb31-enum/README.md:18), and running on their real GW5AST-138 hardware in
  the eidle/csr_rw/usb31-enum examples.
- *Wiring*: copy our generated `dp_serdes.v` (plaintext), delete the
  `\~upar_arbiter_wrap.dp_serdes` instance (dp_serdes.v:575-648) and the encrypted
  `upar_arbiter.v` from the project, and drop in our own module with the same ports.
  Everything it touches is already visible in our file: UPAR side to
  `gtr12_upar_inst` (`UPAR_CLK = q0_fabric_cm_life_clk_o`, dp_serdes.v:540;
  `CSR_MODE = {vcc,gnd,vcc,gnd,gnd}` = **0b10100**, dp_serdes.v:551 — identical constant
  in their builder, quad.py:190; `AHB_CLK_O`/`AHB_RSTN_O` outputs, with `AHB_RSTN_O`
  feeding the quad's `AHB_RSTN`, dp_serdes.v:513/531 — same as their serdes.py:245-258).
  Since we have exactly one client, the round-robin can degenerate to a single-master
  UPAR FSM (latch → drive → wait-ready → done) — 60 lines — with every stage registered
  and pipelineable at will; the protocol is level/hold + ready-handshake, so **there is
  no combinational deadline anywhere**: extra pipeline stages only add life-clk cycles
  to a 40 µs replay.
- *Boot safety*: the `.csr` boot blob is executed by a sequencer **inside the UPAR
  macro** (the `set_csr` flow works with zero fabric logic; the fabric `UPAR_*_S` port is
  a separate slave port). Replacing the fabric arbiter cannot break device-config boot.
  Our replay ROM re-drives the same port the vendor arbiter used.
- *Risks*: (1) unknown quirks inside the encrypted FSM not captured by their model —
  mitigated by their cocotb equivalence claim, but that test isn't in our clone, so
  hardware-verify on the dp_test bitstream first; (2) `CSR_MODE` bit semantics unknown —
  keep 0b10100 verbatim; (3) copy their 2^20 timeout so a wedged UPAR can't hang the
  replay engine; (4) keep `UPAR_RST=0`, `UPAR_BUS_WIDTH_S=0`, `strb=0xFF` on writes.
- *Effort*: ~1-2 days RTL + a dp_test soak. Do it only if Tier 1 doesn't close timing or
  if we want the timeout/observability anyway.

---

## 4. POR / reset sequencing — theirs vs ours

- **CPLL resets**: their builder ties `FABRIC_POR_N_I`, `FABRIC_CMU0/1_RESETN_I`, and
  **all four** `FABRIC_LN{0-3}_CPLL_RESETN_I` to the single `por_n`
  (quad.py:97-113), with the load-bearing comment: *"ALL lane CPLL resets must be tied to
  por_n, not just active lanes. Leaving unused CPLL in permanent reset can block the
  entire PLL tree"* (quad.py:110-112). **Our vendor wrapper already does exactly this**
  (`138B/dp_serdes/dp_serdes.v:473-489,510`: CMU0/CMU1/LN0-LN3 CPLL_RESETN and POR_N all
  = `por_n_i`). **No hazard in our 2-lane wiring** — the generator got this right.
- **TOML reset ownership**: our TOMLs already match their proven USB3 overrides —
  `cmu0/1_reset_by_fabric = true`, `por_toggle_by_fabric = true`,
  `cpll_reset_by_fabric = true`, `lane_reset_by_fabric = true`
  (our `138B/serdes_tmp.toml:12-14,60`; their `USB3_QUAD_OVERRIDES`, usb3.py:54-59).
- **Sequencing**: their working 138K enum design (usb31-enum/top.py:128-144) uses a
  reset-less oscillator domain with one-way counters: PHY/controller reset released after
  256 cycles (~5 µs @50 MHz), SerDes `por_n` released when an 18-bit counter saturates
  (~5.2 ms) — "mirrors the reference top.sv: release the controller reset first (~5 us),
  then the SerDes POR" — one-way, never re-toggled. Our ~2.6 ms one-way boot-settle POR
  is the same shape; their reference releases POR *later* (≈5 ms) than we do, so if we
  ever chase a power-on-marginality, lengthening toward 5 ms is precedented.
- **DRP without POR edges**: their DRP bridge ties `por_n = 1` permanently
  (eidle/top.py:112) and CSR access works — the UPAR/CSR path needs no POR edge, only
  the macro-internal 16-cycle arbiter settle.
- **Lane resets** (their USB3-PHY-matching static values, eidle/top.py:119-126):
  `pma_rstn = 1`, `pcs_rx_rst = 0`, `pcs_tx_rst = 0`; "PMA must be out of reset for the
  TX analog driver to power up."

---

## 5. Other findings of value

### 5.1 RX squelch ingredients (for our future RX work)
- Per-lane `FABRIC_LN{n}_RXELECIDLE_O` (+`_H`) pins on the quad (primitives.py:36-37);
  their PHY consumes it directly as `status.rx_elecidle` (signature.py:62-66).
- `signal_detect = ASTAT_O[5]` (ARCHITECTURE.md:257) with TOML threshold `sr_sd_thsel`
  (0-7, csr_map.py:211) and idle filters `idle_high_filter`/`idle_low_filter`
  (toml_gen.py:241-242); 60-die extra `rxsd_use_dlogic` (toml_gen.py:272).
- Receiver detect (far-end termination present): write `0x03000000` to LN_CTRL+0x3F,
  read result at `0x808B34+lane*0x100` (csr_map.py:97-105, 800-801). This is a
  DP-relevant "is a sink attached" probe independent of AUX/HPD.

### 5.2 Error counters / BIST / eye
- Per-lane 8b10b error outputs on the quad: `LANE{n}_DISP_ERR_O[2]`,
  `LANE{n}_DEC_ERR_O[2]`, `LANE{n}_CUR_DISP_O[2]` (primitives.py:44-46) — free error
  telemetry pins (we run raw-20 so these are dormant unless we enable 8b10b).
- Hard PRBS BIST exists: TOML `txPattern`/`rxPattern = "PRBS31"` +
  `txBistInv`/`rxBistInv` (toml_gen.py:197-200) — an in-silicon link tester we could
  drive over DRP for lane qualification without the DP source.
- **No eye-margining registers surfaced anywhere in their RE.** No spread-spectrum
  fields either (nothing matching ssc/spread in the repo).

### 5.3 Power-down controls
Per-lane `FABRIC_LN{n}_PD_I[3]`, `IDDQ_I`, `CPLL_PD_I/IDDQ_I`; quad `CMU0/1_PD_I/IDDQ_I`
(primitives.py:61-63, 79-81, 127-133); TOML `pd_toggle_by_fabric` (default false,
toml_gen.py:137). Unused lanes are content with GND-tied inputs (their `_add_default_port`,
quad.py:221-227) — matching our wrapper.

### 5.4 138 vs 60 die cross-check (against our wrappers)
- Primitives: 138 = `GTR12_QUAD` + `GTR12_UPAR`; 60 = `GTR12_QUADA` + `GTR12_UPARA`
  (config.py:209-221) — our wrappers agree (`138B/dp_serdes.v:260,523`;
  `60B/dp_serdes.v:269,551`).
- 138-only: `p_POSITION = "Q0"/"Q1"` defparam (quad.py:89-91; ours dp_serdes.v:521 ✅);
  INET buses `INET_Q0_Q1[92]`, `INET_Q_PMAC[532]`, `INET_Q_TEST[228]`,
  `INET_Q_UPAR[421]` from each quad into the UPAR's `INET_UPAR_Q0/Q1[421]`,
  `INET_UPAR_PMAC[5467]`, `INET_UPAR_TEST[1329]`, `TL_CLKP_I` (primitives.py:179-191,
  251-257; wired in serdes.py:200-217) — our vendor wrapper carries the same buses;
  plus `FABRIC_CLK_LIFE_DIV_I[2]`, `FABRIC_REFCLK_OE_L/R_I`,
  `FABRIC_REFCLK_OUTPUT_SEL_I[5]` (all GND'd in ours).
- 60-only: `REFCLKP/M2/3` extra ref pads (primitives.py:169-176) — explains the 60B-only
  replay entries (§2.3).
- GW5AST-138 vs GW5AT-138: treated as a pure package variant, same `138k` CSR tool
  (config.py:17, 31).
- Refclk routing: our 138B TOML (`ref_pad1_freq=135M`, `refimux0_sel=3`,
  `ref_prop_dir=1`) matches their computed multi-quad `Q0_REFCLK1` route
  (toml_gen.py:485-492); our 60B (`refimux0_sel=2`) matches their single-quad
  `Q0_REFCLK1` route (toml_gen.py:527-529). Independent confirmation both our
  generator-emitted routings are canonical.

### 5.5 Their measurement instrument (pattern worth stealing)
`ClockFreqProbe` (bench.py:734-868): free-running counter per measured clock; a request
toggle from the reference domain makes each source domain latch its counter into a holding
register and answer with an ack toggle; the reference domain then reads a **stable**
snapshot — no gray coding, no decode logic at speed, dead clocks read 0 via a pending
flag. `f = delta × f_ref / 2^gate_bits`. Cleaner than gray-counter frequency meters for
hostile/unknown clocks; relevant if we instrument cm_life on more boards.

### 5.6 CSR file format confirmation
The `.csr` is literally a text list of `upar_write_driver(0xADDR,0xDATA)` lines — their
`generate_csr` appends extra boot writes in that exact syntax (toml_gen.py:1012-1015),
and our `dp_serdes.csr` is 339 such lines matching our ROM 1:1. The boot sequencer and
our replay execute the identical program; anything we learn to append (their USB3 flow
appends AFE tuning + EIDLE state) can go either in the blob or the ROM.

### 5.7 Gowin toolchain corroboration
Their platform files independently arrive at several of our hard-won settings:
`-bit_security 0` (dkusb_gw5at60.py:327 — our SecurityBit verdict), `-serdesRetiming 0` /
`"SerDes_retiming": false` process config (gw5ast_dvk.py:282, dkusb:294), and a
GowinSynthesis mis-inferred-BSRAM bug forcing `syn_romstyle="logic"` on 8b10b tables
(dkusb_gw5at60.py:498-510) — same class as our $readmemh zero-ROM and DPB traps.

---

## Actions for the campaign

1. **(High, cheap)** Apply the vendor-PCIe multicycle set (§3.2 Tier 1) to the 138B SDCs,
   name-adapted to our hierarchy; confirm the cm_life CSR-ROM family disappears from the
   timing report. Keep the 15.0 ns cm_life period.
2. **(Standing rule)** Re-measure cm_life after any SerDes IP regen or rate/PLL change —
   their data shows ×2 config swings (56-118 MHz); a config in the fast class breaks both
   our 15 ns and the vendor's 10 ns.
3. **(Medium)** Hold Tier 2 (fabric UPAR master replacing the encrypted arbiter, §3.2) in
   reserve; the interface and semantics are now fully documented and the wrap shell in our
   tree is plaintext. Pull their 2^20 UPAR timeout into whatever we build.
4. **(Campaign / AFE)** Try `AFE+0xF8` (their "TX AFE tuning", USB3 value 0x00000A02) in
   the strict-converter overdrive experiments; note the txlev generator encoding
   off-by-one (§2.2) when comparing M5 writes against generator output; their proven
   de-emphasis point cm=5/c1=8 matches our sweep grid.
5. **(RX groundwork)** RX squelch = RXELECIDLE_O pin + `sr_sd_thsel`/idle filters +
   RXDET pulse/result registers (§5.1); PRBS31 BIST exists in-silicon for lane
   qualification (§5.2).
6. **(Curiosity, guarded)** `FABRIC_CLK_LIFE_DIV_I` (138-only, currently 00) may divide
   the life clock from fabric — a possible future timing relief valve; strictly a
   dp_test-first experiment.
7. **(Never do)** Reorder replay writes across the `0xb00000` magic or the
   `0xc00008 = 3→2→1` phase writes; skip the per-active-lane `AFE+0x98 = 1` writes when
   changing lane maps.
