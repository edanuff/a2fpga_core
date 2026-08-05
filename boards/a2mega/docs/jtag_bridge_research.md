# a2mega JTAG bridge — vendor/upstream research reference

Condensed from a deep research pass over openFPGALoader source (master +
tagged releases), Gowin UG290/UG718/UG983/UG984/SUG100, the ESP32-S3 TRM,
ESP-IDF, and Sipeed wiki/examples. Companion to
`jtag_flash_reliability.md` (field investigation + corrections).

## Empirical status (2026-08-04)

- **Root cause of the post-flash wedge: openFPGALoader v1.0.0 toggleClk bug**
  (upstream `d654a9d`, fixed in v1.1.x): `toggleClk()` clobbered the stored
  TMS/TDI defaults, so the Gowin flash protocol's long clock runs corrupted
  the TMS level of subsequent commands, leaving the TAP wedged after every
  flash-phase operation. **Validated on hardware**: after upgrading to
  v1.1.1, flash + immediate `--detect` with NO replug returned a clean
  IDCODE — the sequence that failed on every cycle under v1.0.0.
  **Version floor: never run < v1.1.0 with GW5A over esp_usb_jtag.**
- **Secondary live mechanism (intermittent, now mitigated)**: our
  `esp32_usb_jtag.ino` un-routed all JTAG pins to Hi-Z on a single false
  USB-disconnect reading from ESP-IDF's SOF watchdog (3 ms tolerance,
  documented to flap on healthy links). Debounced to 500 ms in the sketch
  (redeploy the ESP32 firmware to take effect).

## openFPGALoader facts worth keeping

- **Error origins**: `TDO is stuck at 0` = first 32-bit DR read was
  0x00000000 (net held low); `no device found` = read was 0xFFFFFFFF (net
  high/floating). `fails to open esp_usb_jtag device` and `JTAG init failed
  with: std::exception` are ONE event (open/claim failure; the preceding
  stderr line identifies the step). "TDO stuck at 1" is NOT an
  openFPGALoader message — if seen, it came from another tool.
- **`--bulk-erase` is a NO-OP when combined with `-f`** (guarded by
  `bit_file.empty()`). Run it as a separate invocation. flash.sh fixed
  accordingly.
- **GW5A flash programming runs at a hard-coded 10 MHz TCK** —
  `prepare_flash_access()` sets it and `--freq` cannot lower it (no
  `restore_flash_access_frequency` override in the Gowin driver). Chain
  DETECTION honors `--freq`. Within Gowin's spec (UG984: JTAG config
  ≤100 MHz), so failures there are signal-integrity, not device limits.
- **No bridge recovery**: openFPGALoader has no `libusb_reset_device`/retry
  (openocd-esp32 has `esp_usb_jtag_revive_device`, 5 retries). A transient
  bridge wedge that OpenOCD hides is a hard failure here.
- **Open upstream bugs to watch**: PR #644 (GW5A Reinit 0x3F triggers an
  auto bulk-erase the code doesn't wait for — signature: flash+verify pass
  but the bitstream never loads after power cycle); issue #544 (GW5AT-LV60:
  first flash fails, second succeeds — flashing only works from the
  unconfigured state).
- Watch for `No speed caps found` in logs — the fallback assumes a 1 MHz
  base and makes every frequency 40x off (healthy log says
  `Base speed 40000 KHz, div range 1 to 255`).

## Gowin (Arora V) facts

- **JTAG pins are dedicated by default**; the IDE's "Use JTAG as regular
  IO" (dual-purpose pin dialog) reassigns them to GPIO after configuration
  — the classic self-inflicted JTAG lockout (field report on GW5A-25 with
  our exact symptoms). Recovery: hold RECONFIG_N low at power-up (>=25 ns
  pulse), SRAM-load a good design, then flash. The a2mega carrier must
  provide RECONFIG_N access — the Reconfig button is a Sipeed DOCK
  feature; the SOM has none. Sipeed's own example READMEs tell users to
  tick dual-purpose boxes on placement errors — the trap is documented
  reflex.
- **PUDC_B** (ball IOB104A on our CST): input sampled DURING configuration;
  low = all GPIOs weakly pulled up during config, high = Hi-Z. Must not
  float (tie to VCCIO or GND via >=1 kOhm). No post-config role. The
  "Unused Pin" project option is disjoint (acts after config; excludes
  dual-purpose pins). "As open drain driving ground" for unused pins is
  wrong for this carrier — it actively sinks nets the bus may drive.
- SECURITY_BIT blocks readback only (status/IDCODE/reprogram unaffected);
  CRC_CHECK failure leaves DONE low; BACKGROUND_PROGRAMMING option does not
  apply to GW5A (Arora V background upgrade is a different mechanism).
- JTAG is live in user mode by design (IDCODE 0x0001481B for GW5AT-60).

## ESP32-S3 bridge facts

- Bridge mode = `USB_SERIAL_JTAG_CONF0_REG` bit 16 (plain register, no
  eFuse); JTAG signals route through the GPIO matrix to ANY GPIOs the
  firmware picks. Nothing locks the routing — any later gpio_config on
  those pins silently steals them.
- **Never** put JTAG on GPIO45/46 (VDD_SPI / boot straps: a pull-up into
  GPIO45 makes the module boot its flash rail at 1.8 V and not come up),
  or touch GPIO19/20 (USB pads). Safe set on MINI-1-N8: 4-7, 21, 33-38.
  No Octal-SPI conflict on -N8 (quad flash; octal pins 33-37 only apply to
  octal-equipped parts).
- Documented wedge: CMD_REP applied to CMD_FLUSH can leave the USB device
  unresponsive until USB reset. Keep power management/DFS off in bridge
  firmware (USJ needs APB >= 40 MHz); never link TinyUSB into it.
- The device is composite (CDC-ACM + vendor JTAG interface); an open
  serial monitor does not block JTAG. macOS needs no driver work for the
  vendor interface.

## Still unobtained

Tang Mega 60K SOM schematic-level JTAG details (BTB pin numbers, JST
pinout, series/pull resistors, JTAG bank VCCIO) — Sipeed's file host
requires a manual browser download:
`dl.sipeed.com/shareURL/TANG/Mega_138K_60K/02_Schematic`. The GW5AT-60
PG484A ball map for PUDC_B/JTAGSEL_N ships as a Gowin spreadsheet, not in
the UG983 PDF. Check `/Volumes/Public/Datasheets/Gowin` first.
