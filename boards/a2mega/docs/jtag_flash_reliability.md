# a2mega JTAG / bitstream-flash reliability

**Status:** analysis complete, **hardware verification pending** (board powered off and
unavailable at the time of writing). Everything below is labelled **VERIFIED** (read
directly out of a file in this repo, the carrier schematic, or the PnR report) or
**INFERRED** (reasoning on top of verified facts). Nothing here has been re-measured on
hardware.

---

> **CORRECTION 2 (2026-08-04, supersedes the power-topology framing):** two
> facts break the original conclusion. (a) Field failures occurred on
> **USB-only power with the machine off — including immediately after a fresh
> replug/cold boot** — so the wedge is intermittent and not gated by host
> power at all. (b) **USB power and slot power land on the same +5V net**
> (§3), so a power-source-based explanation of the machine-ON rule proves too
> much: it would forbid USB programming equally. What actually differs with
> the machine on is **bus signal ACTIVITY, not power** — which promotes §"pin
> sharing" from secondary finding to PRIME suspect for the machine-on rule:
> Apple II bus signals sit on GW5A configuration-function pins (SSPI_CS/CLK,
> SI/SO, D04–D08, EMCCLK), and a live toggling bus driving the config port
> during flash operations is a coherent mechanism. The machine-off
> intermittent failures remain UNEXPLAINED (bridge-side vs TAP-side — run
> `--detect --freq 500000` at next fresh-boot failure to discriminate before
> retrying). E4 (`--reset` after flash) remains worth one bench test. The
> machine-off flashing rule stands as validated operating procedure.
>
> **PRIME SUSPECT FOUND (2026-08-04): openFPGALoader v1.0.0 toggleClk bug.**
> The installed openFPGALoader was v1.0.0. Exactly ONE esp_usb_jtag change
> exists between v1.0.0 and v1.1.1: upstream commit `d654a9d`
> ("toggleClk: ignore tdi/tms and keep these pins to the current state").
> In v1.0.0, `toggleClk()` overwrote the driver's stored default TMS/TDI
> with the caller's values (typically 0) — so the long clock-toggle runs
> the Gowin flash protocol depends on (RTI clocking, SPI-over-JTAG waits)
> silently corrupted the TMS level of every subsequent buffered command,
> walking the TAP into a garbage state. Mechanism matches the field
> signature exactly: flash-phase operations complete but leave the chain
> dead ("TDO stuck at 0" / "no device found") until a cold boot re-syncs
> the TAP; only the handful of esp_usb_jtag boards in existence ever hit
> this path. **openFPGALoader upgraded to v1.1.1 (brew) 2026-08-04.**
> Validation: the next several flash cycles should no longer need the
> replug ritual. If any wedge still occurs, flash.sh's automatic low-speed
> detect probe classifies it, and the board's own ESP32 bridge sketch
> (boards/a2mega/src/esp32_usb_jtag/) becomes the next suspect.

## 1. Symptom

Flashing the a2mega bitstream with `tools/flash.sh a2mega`
(`openFPGALoader -c esp32s3 --bulk-erase -f --verify …`) fails in a host-power-dependent
pattern:

- With the host Apple II **powered ON**, JTAG operations essentially never succeed
  (known operating procedure — flash with the machine off).
- With the host Apple II **powered OFF**, flashing succeeds — typically first try
  after a cold boot; the recurring failure is the SECOND operation without one.
- Historically the ritual was "unplug and replug the USB cable, then it works again".
  Observed error messages, in varying order:
  - `Error: no device found` (bridge opens, chain scan finds nothing)
  - `JTAG init failed with: TDO is stuck at 0`
  - `fails to open esp_usb_jtag device`
  - `JTAG init failed with: std::exception`
- In every case the ESP32-S3 USB device itself enumerates fine
  (`Open Success with vid: 0x303a pid 0x1001`,
  `esp_usb_jtag: Device found. Base speed 40000 KHz`). It is the **FPGA-facing JTAG chain**
  that fails.

The single most important structural fact this document establishes is that
**"replug the USB" and "power-cycle the card" are the same action only when the Apple II is
powered off** — see §2.1. That one fact reorganises the whole hypothesis space.

---

## 2. What was verified

### 2.1 Power topology — the card is dual-fed, USB **or** slot

**VERIFIED** from `boards/a2mega/sch/a2-mega.pdf`:

| Source | Path to the board's `+5V` net | Sheet |
| --- | --- | --- |
| USB-C `J2` VBUS | → `F1` PTC RESET FUSE 6V 1.1A → `+5V` (**no series diode**) | p.3 "ESP32 & I/O" |
| Apple II slot `BUS_5V` | → `D1` SCHOTTKY 20V 2A (**anode = BUS_5V, cathode = +5V**) → `+5V` | p.2 "Apple II Bus Interface" |

and downstream:

```
+5V ──► U5  TLV1117-33 (LDO)  ──► D8 SCHOTTKY 20V 2A ──► +3V3        (p.3)
+5V ──► 74ALVC164245  VCC(B)  (Apple-II-side of the bus transceivers) (p.2)
+5V ──► Tang Mega SOM, BTB2 connector pins 93/94                        (p.6)
+5V ──► J1 USB-A VBUS         (the card *sources* 5 V to a downstream USB device)
+3V3 ─► 74ALVC164245 VCC(A), ESP32-S3-MINI-1-N8 pin 3, LSF0108PW VREF_A
+3V3 ─► Tang Mega SOM, BTB2 connector pins 45/46                        (p.6)
```

The SOM has no supply of its own — **all** FPGA power comes from the carrier's `+5V`/`+3V3`
nets, so the SOM is powered whenever either source is present.

**Consequences (INFERRED, but from an unambiguous topology):**

1. `+5V = max(USB VBUS, BUS_5V − V_f(D1))`. Either supply alone runs the whole card.
2. **Unplugging the USB cable does not remove power from anything while the Apple II is
   on.** The FPGA, the SOM, and the ESP32-S3 all stay alive on slot power through `D1`.
   The "replug the USB to reset the bridge" ritual is therefore a *no-op for the FPGA and a
   no-op for the ESP32* whenever the machine is powered.
3. Conversely, with the Apple II off, USB is the *only* supply, so a replug **is** a full
   cold power cycle of the card.
4. `D1` is oriented so the card can never back-feed the slot. The failure is not
   back-powering the Apple II.
5. Rail-margin note: `+3V3` sits behind a series Schottky (`D8`), so it is nominally
   ~3.3 − 0.3 ≈ **3.0 V**, and when the card runs on slot power only, `U5`'s input is
   `BUS_5V − V_f(D1)` ≈ 4.7 V against a TLV1117 dropout of ~1.2–1.3 V at high current.
   That is thin. (`D8`'s likely intent is to OR the carrier LDO with a 3.3 V source on the
   SOM; either way the carrier side is diode-dropped.)

### 2.2 JTAG topology — clean, no sharing (this *refutes* the original prime hypothesis)

**VERIFIED**, ESP32-S3 side, `boards/a2mega/sch/a2-mega.pdf` p.3 (module `U6`,
ESP32-S3-MINI-1-N8):

| ESP32 GPIO | Net |
| --- | --- |
| IO40 | `ESP32_JTAG_TCK` |
| IO41 | `ESP32_JTAG_TMS` |
| IO42 | `ESP32_JTAG_TDI` |
| IO45 | `ESP32_JTAG_TDO` (input to ESP32) |
| IO21 | `ESP32_OPI_CS` |
| IO47 | `ESP32_OPI_CLK` |
| IO0…IO10 (approx.) | `ESP32_OPI_D[0..7]` |
| IO11…IO18 | `ESP32_PPO_D[0..7]` |
| IO33 | `ESP32_PPO_SYNC` |
| IO48 | `FPGA_DONE` (input) |
| IO34–IO39 | SD card |

The OSPI runtime link and the JTAG bridge use **disjoint** ESP32 GPIOs. This matches
`boards/a2mega/src/a2fpga_esp32/fpga_jtag.c:4-6`.

**VERIFIED**, SOM side, schematic p.4 (`J4`, "CONN RCPT 80POS SMD GOLD"): `J4` pins
3/5/7/9 are the SOM's **dedicated** `JTAG.TMS` / `JTAG.TDO` / `JTAG.TCK` / `JTAG.TDI`
signals, wired straight to the four `ESP32_JTAG_*` nets. The physically adjacent
even-numbered pins — `BANK4_V19_IOB114B` (4), `BANK4_V18_IOB114A` (6),
`BANK4_U22_IOB104A_PUDC` (10), `BANK4_T18_IOB138B` (12), `BANK4_R18_IOB138A` (14) — are
all marked **no-connect** on the carrier, and none of them appears in
`boards/a2mega/hdl/a2mega.cst`.

**VERIFIED**, FPGA side, `boards/a2mega/impl/pnr/a2mega.rpt.txt:518-521`:

```
V12/12   | -  | in  | IOR1[A] | TCK | LVCMOS33 | OFF | UP | ... | SLOW | ... | Vccio 3.3
R13/12   | -  | in  | IOR1[B] | TDI | LVCMOS33 | OFF | UP | ... | SLOW | ... | Vccio 3.3
T13/12   | -  | in  | IOR3[A] | TMS | LVCMOS33 | OFF | UP | ... | SLOW | ... | Vccio 3.3
U13/12   | -  | out | IOR3[B] | TDO | LVCMOS33 |   8 | UP | ... | SLOW | ... | Vccio 3.3
```

and the bank-usage table (`a2mega.rpt.txt:75-91`) shows **`bank 12 | 0/4 | 0%`** — i.e. the
JTAG pins live in a dedicated bank with **no user I/O whatsoever**.

**VERIFIED**, `boards/a2mega/impl/a2mega_process_config.json`: `"JTAG" : false`, i.e. the
Gowin dual-purpose-pin option "use JTAG as regular I/O" is **not** enabled. The JTAG pins
stay JTAG.

⇒ **The "FPGA post-config drive contends with JTAG" hypothesis is refuted at the pin level.**
No FPGA-driven signal shares a net, a pin, or even a bank with TCK/TMS/TDI/TDO, and no
ESP32 runtime signal shares a GPIO with the bridge.

### 2.3 Apple II bus interface — what reaches the FPGA when it is unconfigured

**VERIFIED**, schematic p.2:

- `U1` (and siblings) = **74ALVC164245DGG**, dual-supply translating transceivers.
  `VCC(A)` (pins 31/42) = `+3V3` (FPGA side, `FPGA_A*` / `FPGA_D*`);
  `VCC(B)` (pins 7/18) = `+5V` (Apple II side, `BUS_A*` / `BUS_D*`).
  `~1OE`/`~2OE` (pins 48/25) = `FPGA_BUS_~OE`; `1DIR`/`2DIR` = `FPGA_A_DIR`.
- **`FPGA_BUS_~OE` has an external 4.7 kΩ pull-up to `+3V3` (`R22`).** So with the FPGA
  unconfigured (or held in reset), the address/data transceivers are **disabled** and the
  Apple II bus does *not* reach the FPGA. This matches the comment at
  `boards/a2mega/hdl/top.sv:225-228`.
- `U2` = **LSF0108PW**, a *passive* bidirectional translator with **no FPGA-controlled
  enable** (EN via `R1` 200 kΩ from `+5V`, the TI reference circuit). It carries the
  **control** lines: `RES`, `INH`, `IRQ`, `NMI`, `DMA`, `RDY`, `R/W`. On the FPGA side each
  has a 4.7 kΩ pull-up to `+3V3` (`R11`, `R54`–`R59`).
  ⇒ **These seven signals reach FPGA pins at all times, including while the FPGA is
  unconfigured.** None of them, however, is on a configuration-function pin (see §2.4).
- `FPGA_PHI1` / `FPGA_Q3` / `FPGA_7M` have 27 Ω series resistors (`R60`, `R62`, …).

`boards/a2mega/hdl/top.sv:221` — `wire system_reset_n_w = device_reset_n_w & a2_reset_n;`
⇒ **INFERRED:** with the Apple II powered off, `BUS_RES` is clamped low by the machine's
unpowered logic, the LSF passes that through, and the a2mega design is **held in reset**.
So "Apple II off" also means "the a2mega design is quiescent". (The ESP32 OSPI connector is
the exception — `top.sv:1894` resets it from `device_reset_n_w`, not `system_reset_n_w`, so
the ESP32 link stays alive either way.)

### 2.4 Apple II signals are sitting on GW5A **configuration** pins

**VERIFIED** from `boards/a2mega/impl/pnr/a2mega.rpt.txt` (lines 160-161, 234-235, 430,
509-577; extracted by cross-referencing the "Signal" and "CFG" columns):

| Design signal | Ball / bank | GW5AT configuration function |
| --- | --- | --- |
| `a2_a[0]` | T20 / 7 | `D08` |
| `a2_a[1]` | R19 / 7 | `D07` |
| `a2_a[2]` | P19 / 7 | `D06` |
| `a2_a[3]` | N22 / 5 | `SI/SSI0` |
| `a2_a[4]` | M22 / 5 | **`SSPI_CS_N`** |
| `a2_d[2]` | M18 / 5 | `SO/SSI1` |
| `a2_d[3]` | L18 / 5 | **`SSPI_CLK`** |
| `a2_a_dir` | T21 / 7 | `D04` |
| `a2_bus_oe` | U21 / 7 | `D05` |
| `clk` | V22 / 7 | `EMCCLK` |

This is *why* `a2mega_process_config.json` carries `"SSPI" : true` ("use SSPI as regular
I/O"). It is legal and it works once the device is configured. The concern is the window in
which the device is **not** configured — during `--bulk-erase`, during the flash write, and
during the reload afterwards — because in that window the configuration controller, not the
user design, owns those balls.

Note in particular that `a2_bus_oe` (the transceiver enable) is itself on `D05` and
`a2_a_dir` on `D04`. `R22` holds `FPGA_BUS_~OE` high externally, but if the configuration
controller ever *drives* `D05` low, the 4.7 kΩ pull-up loses and the transceivers turn on
mid-configuration — connecting a live Apple II bus straight onto `SSPI_CS_N`, `SSPI_CLK`,
`SI/SSI0`, `SO/SSI1` and `D06`–`D08`. Whether GW5A drives those balls during a JTAG flash
session is **not verified** and is the subject of experiment E5.

### 2.5 Gowin project settings (`boards/a2mega/impl/a2mega_process_config.json`)

**VERIFIED** current values, with assessment:

| Key | Value | Assessment |
| --- | --- | --- |
| `JTAG` | `false` | **Correct.** JTAG pins are *not* reused as regular I/O. Not a cause. |
| `SSPI` | `true` | **Required** — `a2_a[4]`/`a2_d[3]` are on `SSPI_CS_N`/`SSPI_CLK`. Only affects the *configured* state; see §2.4. |
| `MSPI` | `false` | Correct — MSPI must remain the config-flash port. |
| `Unused_Pin` | `As_input_tri_stated_with_pull_up` | Safest choice. Leave it. Would only matter if it were `As_open_drain_driving_ground`, which would fight `R22`. |
| `BACKGROUND_PROGRAMMING` | `"off"` | **Worth trying to change.** Background programming is the GW5A feature that lets the config flash be written while the device keeps running its design; with it off, the flash phase necessarily tears the design down. Speculative but cheap to test (E9). |
| `POWER_ON_RESET_MONITOR` | `true` | Means a supply dip re-triggers configuration. Relevant to H5 (rail margin) — a brownout during a flash would drop the chain mid-operation. |
| `SECURITY_BIT` | `true` | Blocks bitstream readback. Should not block IDCODE/flash access, but it is on for no stated reason; consider turning it off while debugging. |
| `CRC_CHECK` | `true` | Fine, and it is what makes the post-power-cycle heartbeat LED a real write verifier (per `tools/flash.sh:67-69`). |
| `DONE` / `READY` / `RECONFIG_N` / `MODE_IO` | `false` | Correct — those config pins are not stolen as user I/O. `RECONFIG_N` is on N12/bank 3 (`a2mega.rpt.txt:527`) and is unused by the design; see M6. |
| `DOWNLOAD_SPEED` | `"default"` | Only affects the Gowin Programmer, not openFPGALoader. Not a cause. |

### 2.6 ESP32-S3 firmware

**VERIFIED**:

- The **runtime firmware and the USB-JTAG bridge are the same binary**.
  `boards/a2mega/src/a2fpga_esp32/a2fpga_jtag.cpp` implements
  `route_usb_jtag_to_gpio()` / `unroute_usb_jtag_to_gpio()`, which set
  `USB_SERIAL_JTAG_USB_JTAG_BRIDGE_EN` and wire the USB-JTAG signals to GPIO 40/41/42/45
  through the GPIO matrix.
- The **only** caller is the Arduino loop, `a2fpga_esp32.ino:913-919`:

  ```c
  bool usb_is_connected = usb_serial_jtag_is_connected();
  if (usb_was_connected == false && usb_is_connected == true)
      route_usb_jtag_to_gpio();
  if (usb_was_connected == true && usb_is_connected == false)
      unroute_usb_jtag_to_gpio();
  usb_was_connected = usb_is_connected;
  ```

  This is **edge-triggered**, not level-maintained. If the "connected" signal ever glitches
  low→high faster than one `loop()` iteration, or if a route is undone by anything else, the
  bridge stays unrouted until the next genuine disconnect→connect transition. With the
  bridge disabled, the USB-JTAG endpoint talks to the **ESP32-S3's own internal TAP** rather
  than the FPGA — which would present to openFPGALoader as an unknown/absent device, i.e.
  exactly `no device found` or `TDO is stuck at 0`. (Mechanism INFERRED.)
- `PIN_SRST = 3` (`a2fpga_esp32.ino:64`, commented "unused and unconnected"). GPIO3 is a
  no-connect on the module in the schematic. **No conflict.** (The *standalone* sketch
  `boards/a2mega/src/esp32_usb_jtag/esp32_usb_jtag.ino:30` uses `PIN_SRST 21`, which on this
  carrier is `ESP32_OPI_CS` — a real conflict, but only if that sketch is the one flashed.
  Worth confirming which firmware is on the board.)
- There is **no software path to reset the bridge**: no CLI command, no menu item, no
  control request. `grep` over `boards/a2mega/src/a2fpga_esp32/` finds no other caller.
- The USB-JTAG endpoint shares the ESP32-S3's single `USB_SERIAL_JTAG` peripheral with the
  **CDC console**, which `loop()` uses to forward the FPGA's UART
  (`Serial1` ↔ `Serial`, `a2fpga_esp32.ino:939-948`). Heavy console traffic and JTAG traffic
  therefore share one USB device. (Interference is INFERRED, not measured; it is the most
  natural explanation for `fails to open esp_usb_jtag device`.)
- `fpga_jtag.c` documents the GW5A flash sequence mirrored from openFPGALoader
  (`fpga_jtag.c:13-24`): `gw5a_enable_spi()` → *"After this the TAP pins ARE the SPI bus:
  TCK=SCLK, TDI=MOSI, TDO=MISO, and the TLR<->RTI TMS moves frame CS."*
  ⇒ **Entering GW5A flash-programming mode stops the TAP being a TAP.** Until the device is
  reset or reconfigured, a normal chain scan on those pins cannot work. This is the
  mechanism behind the already-documented "the chain usually goes dark after any flash-phase
  operation".

### 2.7 The programming flow (`tools/flash.sh`)

**VERIFIED**:

- a2mega/a2p25 use `-c esp32s3`; for flash mode the command is
  `openFPGALoader -c esp32s3 --bulk-erase -f --verify <fs>` (`tools/flash.sh:56-62`).
- A 5-attempt retry loop with `sleep 3`, ending in
  *"try replugging the USB (bridge reset) and rerun"* (`tools/flash.sh:75-87`).
- Two a2mega-specific notes (`tools/flash.sh:64-70`): *"POWER THE HOST APPLE OFF before
  flashing — flashing with the machine live fails intermittently (verified on hardware)"*
  and *"power-cycle the board after flashing"*.
- **`--freq` is never passed.** So the flash phase runs at openFPGALoader's default
  (10 MHz through the esp_usb_jtag bridge).
- `openFPGALoader v1.0.0` (installed here) offers `-r/--reset` ("reset FPGA after
  operations"), `--skip-reset` ("skip resetting the device when in write-flash mode"),
  `--detect`, `--freq`, `--unprotect-flash`, `--skip-load-bridge`.

### 2.8 Prior art already in the repo (do not re-derive)

- **`docs/gotchas.md:101-131`** — "recovering from a corrupt/interrupted config-flash write
  (GW5A)". States that openFPGALoader's *"flash phase defaults to a 10 MHz JTAG clock
  through the ESP32 bridge, **which does not work on this board**"*; 500 kHz does (proven
  repeatedly for dumps, JEDEC ID, bulk erase). Recovery recipe (validated 2026-07-10):
  bulk-erase at `--freq 500000`, then *"Replug the board USB (**fresh power window**; the
  chain usually goes dark after any flash-phase operation)"*, then flash normally. Config
  flash is a Winbond W25Q64. `--sram` loads are *"first-try reliable in a fresh power
  window"*.
- **`docs/gotchas.md:56-57`** — power-cycle between flashes (framed as a DDR3-init issue).
- **`boards/a2mega/TODO.md:138-151`** — flash repaired 2026-07-10; the ESP32 bit-bang
  GW5A SPI-flash path is **not** silicon-validated (status reads returned busy/0xFF).
- Commit `3ed97d12` added the `--verify`, the retry loop, and the two hardware notes.
  Commit `906591d7` explicitly recorded that the "Read ID failed" spiral had *"a simpler root
  cause than bus contention"* — the 10 MHz default.
- **`.claude/skills/flash/SKILL.md:41-50`**, `docs/boards.md:42-47`,
  `boards/a2mega/README.md:7-17,43` restate the same procedure.
- **Not documented anywhere:** any *mechanism* for the host-power dependence; the strings
  `TDO is stuck at 0` and `fails to open esp_usb_jtag device`; anything about USB VBUS vs
  slot +5 V; any hazard from the ESP32's dual role; any link between `a2_bus_oe` and
  flashing. `boards/a2mega/hdl/a2mega_138b.cst` has never existed in any commit.
- **Board comparison:** `a2p25` uses the same `-c esp32s3` path and has **zero** recorded
  flashing-reliability issues. The a2n20v2 family uses a completely different programming
  path (Tang Nano 20K / BL616) and its only "no device found" lore is a BL616 bootloader
  procedure (`boards/a2n20v2-Enhanced/src/a2n20_bl616/README.md:270`) — unrelated.
  ⇒ The problem is a2mega-specific, which points at what is unique to a2mega: it is the only
  board that sits in a **powered Apple II slot that can keep it alive across a USB replug**.

---

## 3. Ranked candidate root causes

### H1 — The USB replug is not a power cycle while the Apple II is on, and a flash-phase operation leaves the TAP needing one — **most likely; confidence HIGH for the mechanism, MEDIUM-HIGH that it is the whole story**

**Claim.** Every `openFPGALoader` flash-phase operation puts the GW5A into a mode where the
JTAG pins are repurposed as the SPI bus to the config flash, and the device does not return
to a normal TAP until it is reset/reconfigured. On a2mega the only reset the user has is a
power cycle — and `D1` means slot power keeps the card alive through a USB replug. So with
the Apple II on, **no amount of replugging produces a fresh power window**, and every
subsequent JTAG operation fails. With the Apple II off, USB is the sole supply, the replug
*is* a cold boot, and flashing works.

**For:**
- §2.1 topology is unambiguous and verified from the schematic.
- `fpga_jtag.c:13-24` documents (from openFPGALoader's own GW5A path) that flash mode makes
  the TAP pins an SPI bus.
- `docs/gotchas.md:120-121` independently records *"the chain usually goes dark after any
  flash-phase operation"* and prescribes a **fresh power window**, not a bridge reset.
- `tools/flash.sh:67` independently prescribes a post-flash power cycle.
- Explains why the effect is a2mega-specific: a2p25 (same `-c esp32s3` bridge) is not in a
  powered slot.
- Explains all four error strings: with a wedged/SPI-mode TAP, a chain scan sees either
  nothing (`no device found`) or a constantly-low TDO (`TDO is stuck at 0`).

**Against / gaps:**
- It does not by itself explain a *first-attempt* failure on a card that has been sitting
  happily running a good bitstream, where the TAP should be healthy. In practice, though,
  the board is essentially never in a fresh power window when the machine is on, because you
  cannot create one without switching the Apple II off.
- The exact GW5A exit condition from flash-SPI mode is not verified against Gowin's
  documentation; openFPGALoader's default post-write reset (`--skip-reset` exists, implying a
  reset happens by default) *ought* to restore it. Why it apparently does not is the main
  open question — E8 targets it.

### H2 — The flash phase runs at 10 MHz, which this board's own docs say does not work — **confidence HIGH as a contributing cause; independently and cheaply fixable**

`tools/flash.sh` never passes `--freq`, so the write/erase phase runs at the 10 MHz default
that `docs/gotchas.md:103-107` explicitly states *does not work on this board*. That alone
produces the whole "`Read ID failed` / `TDO is stuck` / `no device found`" family. It is not
host-power-dependent by itself, but a live Apple II (32+ level-translated 5 V lines toggling
a few inches from the SOM) plausibly turns a marginal 10 MHz link into a dead one.

**For:** verified inconsistency between the tool and the project's own documented finding;
500 kHz is documented as working for every flash operation tried.
**Against:** does not explain the crisp ON/OFF host-power boundary on its own.

### H3 — Apple II bus traffic reaching the GW5A configuration interface during the unconfigured window — **confidence MEDIUM**

§2.4: `a2_a[3]`→`SI/SSI0`, `a2_a[4]`→`SSPI_CS_N`, `a2_d[2]`→`SO/SSI1`, `a2_d[3]`→`SSPI_CLK`,
`a2_a[0..2]`→`D06`–`D08`, `a2_a_dir`→`D04`, `a2_bus_oe`→`D05`, `clk`→`EMCCLK`.

**For:** if the transceivers are ever enabled while the device is unconfigured, a running
Apple II drives a slave-SPI configuration port at ~1 MHz for the entire duration of the
flash — a textbook way to wedge a configuration controller. It is exactly host-power-gated.
**Against:** `R22` (verified external 4.7 kΩ pull-up on `FPGA_BUS_~OE`) is specifically there
to keep the transceivers off in that state, and `top.sv:225-228` says so. The residual risk
is only that `a2_bus_oe` is *itself* `D05` and could be driven by the config controller.
**Discriminator:** E5 (scope `FPGA_BUS_~OE`).

### H4 — ESP32-side bridge state, not resettable in software and not reset by a replug while the slot is live — **confidence MEDIUM-LOW as a primary cause, HIGH as an aggravator**

The bridge route is edge-triggered on `usb_serial_jtag_is_connected()` (§2.6) with no
level re-assertion and no software reset path. The ESP32 is also kept alive by slot power
across a USB replug (§2.1), so any wedged firmware state persists too. The JTAG endpoint
shares one USB device with a CDC console that is actively forwarding the FPGA's UART.

**For:** the only hypothesis that naturally produces `fails to open esp_usb_jtag device`
(a USB-claim failure, before any JTAG happens).
**Against:** a genuine USB replug should still generate a false→true edge and re-route, even
without an ESP32 reset — so this cannot be the sole reason a replug fails to help.

### H5 — Rail margin / brownout re-triggering configuration — **confidence LOW-MEDIUM**

`+3V3` ≈ 3.0 V after `D8`; on slot-only power `U5`'s input is ~4.7 V against a ~1.2–1.3 V
TLV1117 dropout; `POWER_ON_RESET_MONITOR` is `true`, so a dip re-configures the device and
would drop the chain mid-operation. Card load is materially higher with the machine on (the
design is out of reset, DDR3/HDMI/OSPI/disk all active) and higher again during a flash
erase.
**Against:** with the Apple II on, the slot *adds* a supply; it should not make power worse.
Unless the extra load (design running) outweighs it.

### H6 — refuted / not supported

- **FPGA post-configuration drive contending with JTAG pins** — refuted (§2.2): JTAG is in
  bank 12 with zero user I/O; `"JTAG": false`; the SOM's JTAG-adjacent BANK4 pins are NC and
  unassigned in the CST.
- **ESP32 OSPI role stealing the JTAG GPIOs** — refuted (§2.2): disjoint GPIOs.
- **`PIN_SRST` clashing with `ESP32_OPI_CS`** — refuted for the runtime firmware
  (`PIN_SRST = 3`, a no-connect). *Only* applies if the standalone
  `esp32_usb_jtag.ino` (which uses GPIO21) is what is flashed — worth confirming.
- **Card back-powering the Apple II** — refuted: `D1` blocks that direction.

---

## 4. Experiments to run on hardware

Each states the action, and what each outcome implies. **Run E0 first** — it is the cheapest
and it decides the ranking.

### E0 — Fresh power window *with the machine on* (decides H1 vs everything else)

1. Apple II **off**. Unplug the card's USB-C. Wait 10 s (rails fully down — confirm with
   E2's meter or by the absence of any LED).
2. Plug USB-C back in. The card boots from USB alone.
3. **Now switch the Apple II on.**
4. Immediately run `tools/flash.sh a2mega` once.

- **Succeeds** → **H1 confirmed.** The variable is "fresh power window", not "machine is
  running". The rule becomes "power-cycle the card, which requires the Apple II off *at that
  moment*", not "keep the Apple II off during flashing". Big usability win.
- **Fails the same way** → H1 is not sufficient; promote H3 and H5, proceed to E5/E7.

### E1 — Prove the back-feed (confirms the §2.1 topology on this actual board)

With the Apple II **on**, unplug the USB-C and measure `+5V` and `+3V3` (or just watch the
heartbeat LED / HDMI output).

- **Rails stay up / LED keeps running** → verified: the replug is not a power cycle. This is
  the predicted result and is the factual foundation of H1.
- **Rails collapse** → §2.1 is wrong for this board spin (e.g. `D1` not fitted); re-rank.

### E2 — Where is the chain broken: bridge or TAP?

Immediately after a failure, with the machine on, run in order:

```
openFPGALoader -c esp32s3 --detect
openFPGALoader -c esp32s3 --freq 500000 --detect
```

- **Both say `fails to open esp_usb_jtag device`** → ESP32/USB layer (H4). The FPGA is not
  even being reached.
- **Device opens, no/garbled IDCODE, `TDO is stuck at 0`** → FPGA TAP layer (H1/H3/H5).
- **10 MHz fails, 500 kHz reads IDCODE `0x0001481B`** → **H2 dominates**; go straight to M1.

`0x0001481B` is the GW5AT-60 IDCODE per `fpga_jtag.c:25`.

### E3 — Frequency sweep with the machine ON (tests H2 in isolation)

```
FS=... openFPGALoader -c esp32s3 --freq 500000 --bulk-erase -f --verify <bitstream>
```
then repeat at `--freq 1000000`, `2000000`, `5000000`.

- **500 kHz succeeds with the machine on** → H2 is the dominant cause and the host-power
  rule can be relaxed to a frequency setting. Adopt M1 permanently.
- **All frequencies fail with the machine on but 10 MHz works with it off** → H2 is not the
  whole story; H1/H3 remain.

### E4 — Does an explicit reset restore the chain without a power cycle? (tests H1's exit path)

With the machine on and the chain wedged, try, in order:

```
openFPGALoader -c esp32s3 --freq 500000 --reset
openFPGALoader -c esp32s3 --freq 500000 --detect
```

and separately an SRAM load (`tools/flash.sh a2mega --sram`).

- **`--reset` restores IDCODE** → the fix is purely in the flash flow: always reset after
  flashing (M2), and the "power cycle" ritual becomes unnecessary.
- **Nothing but removing power restores it** → H1 confirmed *and* it is a hard property of
  the part; the fix must be a hardware/firmware reset path (M5/M6).

### E5 — Scope `FPGA_BUS_~OE` during a flash, machine ON (decides H3)

Probe the `R22` / transceiver `~1OE` node (also reachable at the `J4` pin-13 net,
`BANK4_U21_IOB97B`) while running a `--bulk-erase -f`.

- **Stays high (3.3 V) throughout** → H3 refuted; the transceivers never expose the config
  pins. Deprioritise.
- **Goes low at any point while the FPGA is unconfigured** → **H3 confirmed.** The live
  Apple II is then driving `SSPI_CS_N`/`SSPI_CLK`/`SI`/`SO` during configuration.
  Follow-up: probe `a2_a[4]` (M22) and `a2_d[3]` (L18) for ~1 MHz activity in that window.

### E6 — Bus-quiet-but-powered test (separates "machine powered" from "machine running")

With the Apple II **on**, hold it in a state with minimal bus activity — the cleanest is to
hold the Apple II's own RESET down (or, if the ESP32 reset-hold path is usable, assert the
`a2bus_control_if` reset hold via OSPI reg `0x2E`, per
`boards/a2mega/hdl/esp32/esp32_ospi_connector.sv:11`). Then flash.

- **Works with the machine powered but held in reset** → the *activity*, not the *power*, is
  what breaks it → H3 (or H5's load component). Strong result.
- **Still fails** → power/topology, i.e. H1. Combined with E0 this is close to decisive.

### E7 — Load-shedding test (tests H5)

Machine on. Unplug HDMI, unplug anything from the USB-A host port, disable WiFi if possible.
Flash.

- **Works** → rail margin (H5) matters; consider a better 3.3 V supply / removing `D8`.
- **No change** → H5 deprioritised.

### E8 — Two-supply test (isolates D1)

Feed the card from a bench supply on USB VBUS at 5.2 V (above `BUS_5V − V_f`) so `D1` is
firmly reverse-biased, machine on. Flash.

- **Works** → the slot's contribution to the rail (not the bus activity) is implicated —
  points at rail interaction/noise rather than pure logic.
- **No change** → the rail source is not the variable; the difference is bus activity or
  power *continuity* (H1/H3).

### E9 — `BACKGROUND_PROGRAMMING` (speculative, needs a rebuild)

Rebuild with `"BACKGROUND_PROGRAMMING"` set to the JTAG background-programming mode instead
of `"off"` and flash the resulting bitstream once by the normal (working) procedure; then
try re-flashing with the machine on.

- **Subsequent flashes stop wedging** → the tear-down of the running design was the problem;
  adopt permanently.
- **No change** → revert (it costs configuration robustness for nothing).

### E10 — Confirm which ESP32 firmware is on the board

Connect to the ESP32 console (115200) and read the banner
(`a2fpga_esp32.ino:886` prints `A2FPGA ESP32-S3 Firmware (<date> <time>)`).

- **Banner present** → the runtime firmware is loaded; `PIN_SRST = 3`, no OPI_CS conflict.
- **No banner / different sketch** → the standalone `esp32_usb_jtag.ino` may be loaded, which
  drives GPIO21 (`ESP32_OPI_CS`) as TRST. Not a JTAG-path conflict, but worth knowing.

---

## 5. Mitigations

### Adopt now — low risk

| # | Change | Status |
| --- | --- | --- |
| **M1** | `tools/flash.sh`: pass `--freq 500000` (or an overridable `FREQ=` env default) on the `esp32s3` path. | **Speculative as a fix for the host-power symptom**, but *proven* by `docs/gotchas.md:103-107` to be the right clock for this board. Fixes a verified inconsistency: the tool contradicts our own documentation. |
| **M2** | `tools/flash.sh`: after a successful write+verify, issue an explicit `openFPGALoader -c esp32s3 --freq 500000 --reset` (or add `-r`) so the TAP is returned to normal instead of being left in flash-SPI mode. | Speculative; validated by E4. |
| **M3** | Correct the operator guidance in `tools/flash.sh` and `docs/gotchas.md`: **a USB replug is only a power cycle when the Apple II is off** (schematic `D1`). Replace "try replugging the USB (bridge reset)" with "power the Apple II off *and* unplug USB for ~10 s — with the machine on, the card stays powered through the slot". | **Proven from the schematic** (§2.1). Documentation-only, zero risk. |
| **M4** | `tools/flash.sh`: run `--detect` first and abort with a clear message rather than burning 5 write attempts against a dead chain. | Zero risk. |
| **M5** | ESP32 firmware: (a) make the bridge route *level*-maintained, not edge-triggered — re-assert `route_usb_jtag_to_gpio()` whenever `usb_serial_jtag_is_connected()` is true and the route is not already active; (b) add a CLI/menu command that force-resets the bridge; (c) add a CLI/menu command that reconfigures the FPGA using the existing `fpga_jtag.c` sequencer, giving a software substitute for the power cycle. | Speculative; (a) is defensive and harmless. Note `boards/a2mega/TODO.md:143-146` records that the bit-bang path is not silicon-validated, so (c) needs care. |

### Board revision / larger changes — speculative

| # | Change |
| --- | --- |
| **M6** | Give the card a reset path that does not require touching the Apple II: bring `RECONFIG_N` (ball N12, currently unused — `a2mega.rpt.txt:527`) to an ESP32 GPIO, and/or put a load switch on the `+5V` rail (or a jumper in series with `D1`) so the card can be cold-booted in place. This directly removes the H1 failure mode. |
| **M7** | In a future pinout, move `a2_bus_oe` and `a2_a_dir` off `D05`/`D04`, and the low address/data bits off `SSPI_CS_N`/`SSPI_CLK`/`SI`/`SO`, so no Apple II signal can ever reach a configuration port (§2.4). |
| **M8** | Improve the 3.3 V rail: replace `U5` + `D8` with a buck or a low-dropout part that does not lose 0.3 V in a series Schottky, so the rail is a true 3.3 V and has margin when running on slot power (§2.1 note 5). |

### Do **not** change

- `"JTAG" : false` — correct as-is; enabling JTAG-as-regular-IO would break programming.
- `"Unused_Pin" : "As_input_tri_stated_with_pull_up"` — the safe setting; changing it to an
  open-drain mode would fight `R22` on `FPGA_BUS_~OE`.
- `"MSPI" : false` — must stay, the config flash needs it.

---

## 6. Summary

Most likely root cause: **the a2mega cannot be power-cycled while the Apple II is on**,
because slot `+5V` reaches the board's rail through `D1` (schematic p.2) and keeps the FPGA,
the SOM and the ESP32 alive across a USB replug — while every openFPGALoader flash-phase
operation leaves the GW5A's TAP in a state the project has already observed to require a
fresh power window. Confidence in the mechanism: **high**. Confidence that it is the *whole*
story: **medium-high**, pending E0.

Immediate mitigation with the best cost/benefit: **pass `--freq 500000` on the `esp32s3`
path in `tools/flash.sh`** (M1) — the tool currently contradicts `docs/gotchas.md` — plus a
post-flash `--reset` (M2) and a corrected operator note (M3).

Top experiments: **E0** (fresh power window, then switch the Apple II on, then flash),
**E2** (`--detect` at 10 MHz vs 500 kHz right after a failure), **E4** (does `--reset` alone
un-wedge the chain?).

---

## 2026-08-14 night session: fake-success bug + flash-boot failure forensics

**openFPGALoader FAKE-SUCCESS BUG (critical operational finding):** when the
JTAG chain scans `empty` (first line of output), flash operations DO NOT
ABORT — the tool parses the file, prints `Done`/`DONE`, runs the exit
sequence (status-register dump, "Erase SRAM DONE", reload attempt) and
looks success-shaped while having written NOTHING. A 7-18 MB write at
500 kHz takes minutes and emits thousands of `Writing:` progress lines;
a fake completes in ~1.4 s with zero. **Judge every op by elapsed time +
progress lines, never by `Done`.** (The "2-second bulk-erase" tonight was
a fake; so was every "verified write" whose log began with `empty`.)

**Boot-failure timeline (reconstructed):** flash held a known-good image
(booted from the slot minutes earlier). The first wedged flash session
wrote nothing (all attempts died at chain scan). Yet flash-boot never
succeeded again on the bench afterward — the failure PREDATES any real
write. The one live-chain probe that reached real flash logic read an
unrecognized JEDEC ID + block-protection bits set — either the chip is in
a corrupted persistent state, or the bridge garbles SPI-over-JTAG and the
readings are noise.

**Confirmed healthy:** FPGA + bitstream (SRAM load at 500 kHz ran to
100%, heartbeat up). Compressed bitstream now 7.2 MB (was 18.3 MB):
`set_option -bit_compress 1` — keep this on all a2mega builds regardless
(3x faster flash, dodges any >16 MB addressing questions).

**Discriminator queued (user):** direct JTAG programmer on the SOM's JTAG
header, bypassing ESP32 bridge + USB-C entirely. Programs fine -> bridge/
USB-C path at fault; fails identically -> SOM flash chip. **Do first
(zero cost): slot power-on test** — last confirmed flash-boot was IN THE
SLOT; if heartbeat appears there but not on the bench, flash is fine and
it's bench-environment interference on the shared config pins.

**Slot-boot test result (2026-08-14, late): NO heartbeat in the powered
GS either.** Bench-interference theory eliminated — the flash CONTENT is
corrupt (not merely unreadable-on-bench). Tomorrow's direct-JTAG
programmer session is now a clean two-way discriminator: writes+boots →
chip fine, bridge corrupted content (fix/work around the bridge);
fails → chip damaged → plan C (ESP32 SRAM-load at power-up) or SOM swap.

## ROOT CAUSE FOUND (2026-08-15 early AM): gowin.cpp hardcoded 10 MHz

`Gowin::prepare_flash_access()` (openFPGALoader src/gowin.cpp) ends with
`_jtag->setClkFreq(10000000);` — the GW5A SPI-over-JTAG external-flash
phase ALWAYS runs at 10 MHz, ignoring --freq. Through the esp_usb_jtag
bridge this is marginal: it held all week, tipped over tonight. A flash op
that dies mid-10 MHz-sequence WEDGES THE TAP for every tool (OpenOCD
included, "all ones") until USB cold boot. Wedged-state runs then
fake-success in ~1.4 s (chain scans empty, tool proceeds anyway).
Full causal chain: 10 MHz SPI phase crash → TAP wedge → fake-success
cascade → corrupt flash content → autoboot fail.

Why the layers confused us: SRAM loads never enter the flash-access phase
(stay at --freq) → always worked. Status-register reads happen BEFORE the
10 MHz switch → looked healthy. "flash chip unknown"/BP-bits/JEDEC garbage
were all 10 MHz misreads, not chip state.

**Fix (local build, scratchpad ofl/):** two patches —
1. jtag.cpp: detectChain retry x5 in constructor; empty chain after
   retries now surfaces "no device found" instead of fake-succeeding.
2. gowin.cpp: removed the setClkFreq(10000000) hardcode; SPI-over-JTAG
   flash access now honors the user's --freq.
Upstream issue/PR worth filing (also relates to trabucayre/openFPGALoader
issue #578 — esp_usb_jtag fragility).

Validation pending: fresh cold boot + patched flash at 2 MHz → expect
real multi-minute write with progress lines → power cycle → heartbeat.

## FINAL POST-MORTEM (2026-08-15): board #1 SOM flash retired

Direct-JTAG discriminator (user, Sipeed RV Debugger, SOM UNSEATED from
carrier): identical "Read ID failed" + wedge-until-power-cycle, JEDEC ID
reads 0x000000 (zeros, not FF — MISO flat, chip answers nothing). SRAM
config works perfectly through both transports. Revival sequence
(0xAB release-DPD + 0x66/0x99 software reset, patched into our
openFPGALoader build) changed nothing. FPGA's own boot master also gets
nothing (auto_boot_1st_fail at POR). Chip or its SOM-local connection is
dead; FPGA healthy.

REVISED root-cause ranking (user insight): flash trouble was CHRONIC —
replug rituals, ~50%/attempt failures, flash-phase-specific wedges
persisting after the toggleClk fix — consistent with a MARGINAL,
PROGRESSIVELY DEGRADING flash element all along, now fully failed. The
software bugs found tonight (10 MHz hardcode, fake-success on empty
chain, no chain-scan retry) are real and fixed in our local build, but
they were layered ON TOP of the failing hardware, not the cause of it.
Named mechanisms eliminated: SR-write brick (can't disable ID reads),
stuck DPD/QPI/continuous-read (reset didn't revive), transport layers
(two independent cables). Unresolved: defective part vs chronic
electrical stress. NOTE our master pin table is USE-FILTERED — "MSPI
unreachable from carrier" is NOT yet verified against the full BTB;
GW5A datasheet ball table vs BTB sheets check is on the board #2
pre-slot checklist.

Board #1: retired from flash-boot service (bench mule for ESP32/PD work;
SRAM-load still works when Mac-attached). Board #2 protections:
1. Patched openFPGALoader only (freq honored, honest empty-chain errors,
   flash revival preamble); judge every op by elapsed time + progress.
2. Machine-off flashing rule stays absolute (D05 transceiver-enable
   window).
3. Routine 5-second flash health check (--detect -f) after slot
   sessions; log results — degradation must show as a trend, not a
   surprise.
4. Pre-slot: verify MSPI balls vs BTB (datasheet cross-check).
5. 1.0a4 rev list: hard gate on bus-transceiver enable during config
   windows + AUX bias resistors.
