# M5 — runtime TX AFE control via DRP (honor ADJUST_REQUEST)

Goal (PRE_REV_EXERCISE_PLAN V2.4 / M5): make the sink's ADJUST_REQUEST a
real control loop — change TX swing / pre-emphasis at runtime through
the SERDES DRP/UPAR port during link training, instead of the fixed
max-swing "truthful declarations" policy.

## Register model (GW5AST-138B, Customized PHY; VERIFIED 08-21 from two
independent sources that agree exactly: the 804-vs-900 boot-emission
CSR diff, and the IDE Reconfiguration-dialog exports in
`hdl/gowin/138B/dp_serdes/txafe_*.csr`)

Per-lane base: lane1 = 0x8083xx, lane2 = 0x8084xx (stride 0x100; the
same stride family as the WS4 CR-replay addresses).

| register | field | encoding |
|---|---|---|
| base+0x34 | TX swing | txlev << 12 (txlev 0-15; 13=804 mV, 15=900 mV) |
| base+0x38 | FFE C1   | bits [12:8] (0-19) |
| base+0x38 | FFE CM   | bits [4:0] (0-19) |
| base+0xd8 | control  | bit4 = FFE manual enable; bit8 = APPLY strobe |

Runtime application ORDER (from the dialog export — this is the
sequencing knowledge the boot emission cannot provide):
1. write base+0x34 (swing)
2. write base+0x38 (C1|CM)
3. write base+0xd8 = 0x010   (manual enable)
4. write base+0xd8 = 0x110   (apply strobe on top of enable)
(repeat per lane)

The boot emission writes only steps 1-3 — values latch during init, so
POR-time config never needed the strobe. A RUNNING transmitter needs
write 4 to latch new values.

## Exporter findings

- The Reconfiguration dialog (Export button) emits the guide's bare
  format: 24-bit address ++ 32-bit data per line. The boot `.csr` uses
  the `upar_write_driver(addr,data)` wrapper — same payload, two
  syntaxes.
- **FFE Mode = Auto exports are always EMPTY** (even with non-default
  swing). The tool cannot express auto-mode or swing-only-in-auto
  sequences. Design consequence: the runtime path enters FFE MANUAL at
  training start and stays there — which matches DP semantics anyway
  (the sink dictates levels; nothing is "auto").
- Reference exports in dp_serdes/: sw15_c1_8 (matches resident 900 mV
  build exactly + strobe), sw13_manual_c1_0, sw15_cm5_c1_8 (located the
  CM field), and the empty auto variants kept as negative evidence.

## DP level mapping (derived 08-21 — engineering model, refine on bench)

Swing law (IPUG1024 range 180-900 mV + our anchors txlev 13=804 /
15=900): **Vdiffpp = 180 + 48·txlev mV**, linear, all 16 steps.

FFE model (observed GUI behavior C1=8→C0=32, C1=0→C0=40 ⇒ C0 = 40−C1,
constant total drive): de-emphasis dB = 20·log10(40/(40−2·C1)).

| DP VS level | nominal | txlev (actual mV) |
|---|---|---|
| 0 | 400 mV | 5 (420) |
| 1 | 600 mV | 9 (612) |
| 2 | 800 mV | 13 (804) |
| 3 | 1200 mV | 15 (900 = our max; declare MAX_SWING_REACHED) |

| DP PE level | nominal dB | C1 (actual dB) |
|---|---|---|
| 0 | 0 | 0 (0) |
| 1 | 3.5 | 7 (3.7) |
| 2 | 6.0 | 10 (6.0 exact) |
| 3 | 9.5 | 13 (9.1; declare MAX_PE at 3) |

CM stays 0 (no pre-shoot tap in DP 1.x training). Note the resident
900 mV build's C1=8 ≈ 4.4 dB sits between PE1 and PE2. CAVEATS: the
swing law is exact by construction; the C0=40−C1 rule rests on two GUI
observations, and the dB formula is the standard FFE de-emphasis model
— both flagged for bench refinement (the per-level A/B in step 3 is
also the empirical check).

## Boot-path vs reconfig-path encoding quirk (08-21, ticket note)

The BOOT csr always contains a base AFE write block (0x334=0xE000,
0x338=0x0B00, FFE-manual bit unset so the coefficients are inert in
Auto); non-default GUI choices APPEND override writes at the csr tail
(the 900 build = 804 csr + 6 lines). Discrepancy: boot path writes
0xE000 for toml txlev=13 where the Reconfiguration-dialog export writes
0xD000 for the same level (both agree on 0xF000 at 15) — an
off-by-one-with-clamp between the two generator paths. Resolution =
the DRP peek reading the live register (queued with the config-path
audit). Also recorded 08-21: the fabric csr_replay ROM is ON-DEMAND
only (replay_req handshake; CR telemetry lines are the read-back dump
display) — normal boots run pure vendor config-time values, so the
804/900 hardware A/B was clean; the previously-stale shared ROM
(60K-era in all 138B builds) was a dormant trap only, now regenerated
per-build via gen_csr_replay.py <csr-path> (60B copy preserved as
csr_replay_rom_60b.svh).

## Next steps
1. ~~Fabric FSM (sim-first)~~ — **DONE 08-21**: `afe_adjust_seq.v` +
   unit & closed-loop TBs all PASS; design, sim results, and the
   propose-only shared-RTL diffs live in `m5_afe_adjust_design.md`.
2. Bench validation before automation: gateware hook to fire a stored
   sequence on command (telnet-triggered via ESP32/OSPI register) —
   apply swing changes on a live link, watch C:/Y:/A: telemetry.
3. 60K equivalence: repeat the dialog exports from a 60K project
   session; verify the same register family/stride (per-die check —
   do NOT assume).

## Provenance

- Boot-diff method: `git show 26f6c427:...toml` (804) through
  serdes_toml_to_csr_138k.bin vs resident 900 mV dp_serdes.csr — the
  entire diff is the six value-writes above (no strobe).
- Dialog: IDE V1.9.12.03, 138B project, Channel = Q0 Lane1+Lane2, only
  TX AFE enabled.
