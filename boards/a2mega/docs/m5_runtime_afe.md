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

## Next steps

1. DP level mapping table: VS levels 0-3 → txlev values via the guide's
   swing-mV table (anchors: 13=804 mV, 15=900 mV); PE levels 0-3
   (0/3.5/6/9.5 dB) → C1 values. Deliverable: (VS,PE) → (txlev,CM,C1)
   for the legal DP combos.
2. Fabric FSM (sim-first): small sequence ROM + player driving the
   existing upar_arbiter DRP port; triggered by link_signal_mgmt when
   ADJUST_REQUEST differs from current levels, during TPS phases;
   TRAINING_LANE_SET then reports the actually-applied levels.
3. Bench validation before automation: gateware hook to fire a stored
   sequence on command (telnet-triggered via ESP32/OSPI register) —
   apply swing changes on a live link, watch C:/Y:/A: telemetry.
4. 60K equivalence: repeat the dialog exports from a 60K project
   session; verify the same register family/stride (per-die check —
   do NOT assume).

## Provenance

- Boot-diff method: `git show 26f6c427:...toml` (804) through
  serdes_toml_to_csr_138k.bin vs resident 900 mV dp_serdes.csr — the
  entire diff is the six value-writes above (no strobe).
- Dialog: IDE V1.9.12.03, 138B project, Channel = Q0 Lane1+Lane2, only
  TX AFE enabled.
