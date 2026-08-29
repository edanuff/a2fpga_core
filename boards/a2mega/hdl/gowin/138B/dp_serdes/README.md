# 138B DP SERDES emission — **must be generated in the Gowin IDE GUI**

This directory is **intentionally missing the emission itself**
(`dp_serdes.v`, `dp_phy/`, `upar_arbiter/`, `dp_serdes.csr`). It holds only
three `*.reference` sidecars: the 60B production configuration translated to
the 138B's lane numbering, so the GUI dialog can be filled in without
re-deriving anything. `boards/a2mega/a2mega_dp_test_138b.gprj` expects the
real files to appear here; **the project does not build until they do.**

## Why headless generation is impossible here (evidence, not caution)

Four independent, hard blockers were found — each verified, not assumed:

1. **Different silicon primitive.** The 60B emission instantiates
   `GTR12_QUADA`. Synthesising that netlist against `GW5AST-138B` fails with
   `ERROR (RP0008): There is no GTR12_QUADA resource in current device`.
   The 138 die uses **`GTR12_QUAD`** — a *parameterised, multi-quad* variant
   (`POSITION = "Q0"|"Q1"`) whose port set differs by 43 ports (258 vs 279;
   247 common). The 138-only ports are the inter-quad interconnect and
   reference-clock steering nets — `INET_Q0_Q1`, `INET_Q_UPAR`, `INET_Q_PMAC`,
   `FABRIC_REFCLK_OE_L_I/R_I`, `FABRIC_REFCLK_OUTPUT_SEL_I[4:0]`,
   `FABRIC_CM0_RXCLK_OE_L_I/R_I` — i.e. exactly the machinery a two-quad die
   needs and a one-quad die does not. (Ports compared in
   `simlib/gw5a/prim_sim.v`.)
2. **Different CSR schema.** The vendor's per-die CSR generator
   (`bin/serdes_toml_to_csr.dist/serdes_toml_to_csr_138k.bin`) rejects a
   60-die `.toml` with `KeyError: 'refomux0_sel'`, then `KeyError:
   'ref_prop_dir'`, then `KeyError: 'q1'` — it requires quad-topology inputs
   the 60 flow has no concept of: a reference **output** mux select
   (legal values 0/1 — 2 and 3 are rejected), a **reference propagation
   direction** (legal values 0/1/2), and a complete second-quad `[q1]` block.
   Those three choices decide whether the 135 MHz reference ever reaches
   QPLL0. Guessing them is exactly the class of error that cost a week of
   silicon forensics in `docs/WS4_GOWIN_STACK.md` §§9-11.
3. **`gw_sh` has no IP-generation TCL** (no `ip_*` commands) — re-verified.
4. **`GowinModgen` cannot do it either.** `GowinModgen -do serdes.mod` exits 1
   with no diagnostic on the SERDES `.mod`; and in this environment it exits 1
   even on an *untouched, known-good vendor* PLL `.mod`, so it is not a usable
   headless generator here at all.

**What IS reusable headlessly** (and was validated in passing, recorded here
so nobody re-derives it): `serdes_toml_to_csr_<die>k.bin` reproduces a GUI
`.csr` from its `.toml` *exactly*, except for one systematic omission — for
every enabled lane it drops four writes at `0x800224 + 0x200·lane` (values
`0x00000041 / 0x00004100 / 0x00410000 / 0x41000000`) that the GUI emits right
after that lane's `0x8003a7 + 0x200·lane` write. Confirmed identical on three
independent GUI emissions (production raw ln2/3, EDP ln2/3, 4-lane raw
ln0-3); the lane stride is pinned by the 4-lane and lanes-0/1 emissions. So
the CLI tool is a good *verifier* of a GUI `.csr`, not a substitute for one.

## The generation checklist (~5 minutes in the IDE)

Gowin IDE **V1.9.12.03** (the certified install). Values below come from the
`*.reference` sidecars in this directory, which are the hardware-proven 60B
production configuration with **only the lane numbers changed**.

1. Open `boards/a2mega/a2mega_dp_test_138b.gprj`. Confirm the device reads
   **GW5AST-138B**, part **GW5AST-LV138PG484AC1/I0** (`gw5ast138b-011`).
2. **Tools → IP Core Generator → Serdes → Serdes.**
   *General tab:* File Name **`dp_serdes`**, Module Name **`dp_serdes_138b`**
   ⚠ (not `dp_serdes` — that name belongs to the lane shim,
   `hdl/dp_test_138b/dp_serdes_lane_shim.sv`), Create In
   **`boards/a2mega/hdl/gowin/138B/dp_serdes`**, Synthesis tool
   **GowinSynthesis**, Global Reset **enabled**.
3. Protocol → **Customized PHY**, protocol module name **`dp_phy`**.
4. **PHY tab** — the whole point of this exercise is the lane pair:
   - Quad **Q0**; Channels: **PHY Lane1 ✓ and PHY Lane2 ✓** — *and nothing
     else*. ⚠ This is the ONE substantive difference from the 60B build,
     which uses lanes 2+3. Rationale: carrier **DP0** (balls C7/D7) is
     `Q0_LN3` on the GW5AT-60B but **`Q0_LN1`** on the GW5AST-138B, while
     carrier **DP1** (A6/B6) is `Q0_LN2` on **both** dies. See
     `docs/SOM_138B_BALLMAP.md` §2.
   - Operation Mode **TX Only**; Loopback **OFF**; Word Alignment **off**;
     Clock Correction **None**; RX Channel Bonding **None**.
   - **DRP Port ✓** (the telemetry `CR` register-dump bridge depends on it).
   - TX Line Rate **2.7**, TX User Clock Ratio **1:1**, TX Internal Data
     Width **20**, TX Encoding **OFF** (raw — the fabric does 8b10b).
   - **TX PN Invert: Lane1 ✓ Lane2 ✓** (all four carrier pairs are P/N
     swapped at the BTB on both dies — verified, TXM/TXP ordering is
     die-independent).
   - TX Master Channel Selection **Q0 Lane2** (unchanged from the 60B — this
     is why the `.sdc` clock anchor `LANE2_PCS_TX_O_FABRIC_CLK` is identical
     on both dies; do not change it).
   - RX side is don't-care (TX Only), leave 2.7 / 1:1 / no inverts.
5. **Refclk:** Source **Q0 REFCLK1**, Frequency **135.000**
   ⚠ *this field resets to 125 every time the dialog opens — re-type 135
   before OK*, PLL **QPLL0**. (Balls E10/F10 = `Q0_REFCLKP_1/M_1` on both
   dies — MATCH, nothing to decide.)
6. **AFE:** Differential Swing **804 mV** (`txlev = 13`), FFE **Auto**,
   Equalization **Auto**, SD Threshold **100 mV**, ATT 7 / BOOST 9.
   *(The 60B production build later moved to 900 mV + FFE Manual C0=32/C1=8 as
   a drive experiment; start the 138B from the plain baseline.)*
7. **TX Channel Bonding: CHECKED** (Master Q0 Lane2, Read Start Depth 16) —
   exactly as the sidecar prefills it and exactly as the hardware-proven 60B
   production `.ipc` has it (`TX_CHANNEL_BONDING=true`). ⚠ Nuance from WS4
   §9: the checkbox emits a *non-functional* bond (`chbond_enable=false` in
   the toml — the "half-bond"), so bonding can never be *relied on*; but our
   design doesn't rely on it — this group's real effect in our config is the
   master-channel word-clock selection, which the shim and `.sdc` depend on.
   Do not uncheck it. **RX Channel Bonding = None** (separate field) is the
   one that stays off.
8. OK, OK. Then **verify the emission before building**:
   - `serdes_tmp.toml` — `[q0.ln1]` and `[q0.ln2]` have `enable = true`,
     `tx_data_rate = "2.7G"`, `width_mode = 20`, `encode_mode = "OFF"`,
     `tx_pol_invert = true`, `txlev = 13`, `ffe_manual = false`;
     `ref_pad1_freq = "135M"`; ln0/ln3 disabled.
   - `dp_serdes.v` — instantiates **`GTR12_QUAD`** (with `POSITION`), not
     `GTR12_QUADA`, and the module is named `dp_serdes_138b`.
   - `dp_serdes.csr` — optional cross-check: run
     `serdes_toml_to_csr_138k.bin -o /tmp/x.csr serdes_tmp.toml` and diff; the
     only differences should be the eight `0x41`-pattern lines described
     above (four per enabled lane, at `0x800424..7` and `0x800624..7`).
9. Do **not** let the IDE re-save the project over `a2mega_dp_test_138b.gprj`
   or `impl/a2mega_dp_test_138b_process_config.json` — a fresh IDE save turns
   **SecurityBit back ON** by Gowin default. Afterwards confirm
   `"SECURITY_BIT": false` in the process config, and `//SecurityBit: OFF` in
   the produced `.fs`.
10. Add the emitted files to `a2mega_dp_test_138b.gprj` if the IDE did not
    (`dp_phy/dp_phy.v`, `dp_serdes.v`, `upar_arbiter/upar_arbiter.v`), then
    build headlessly:
    `GW_SH=/Users/edanuff/gowin-1.9.12-ide/bin/gw_sh GPRJ=a2mega_dp_test_138b.gprj tools/build.sh a2mega`

## If the emitted module face differs from the shim's expectation

`hdl/dp_test_138b/dp_serdes_lane_shim.sv` assumes the generator names the user
face `dp_phy_q0_ln1_*` / `dp_phy_q0_ln2_*` (the pattern every 60B emission
follows). If the 138 generator names them differently, adjust **the shim
only** — never the shared `hdl/displayport/**`.

## 900 mV / FFE-manual variant (the drive-margin A/B, queued 08-20)

Same dialog as the baseline generation — load `dp_serdes.ipc` (now the
generated 804 mV config), then change ONLY the AFE tab:

- Differential Swing **804mV → 900mV**
- FFE **Auto → Manual**, then set **C1 = 8** (C0 auto-adjusts 40 → 32 —
  expected, matches the 60B production emission)

Re-verify before OK (both fields reset-prone): Module Name
`dp_serdes_138b`, refclk **135.000**, Internal Data Width **20**, TX
bonding group unchanged. After OK the enabled lanes' toml must show
`txlev = 15`, `ffe_manual = true`, `ffe_c0 = 32`, `ffe_c1 = 8` (the 60B
production emission's exact 900 mV values); then the CSR cross-check
(`serdes_toml_to_csr_138k.bin`) and the standard build. Keep
the 804 mV emission's `.bin` archived first (bisect discipline): both
variants must be flashable for the warm A/B.
