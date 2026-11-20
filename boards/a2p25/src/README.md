# boards/a2p25/src

> Status (2026-10-25): this directory holds no firmware. The a2p25 runs the ESP32-S3 firmware
> shared with the a2mega.

**Use the firmware in [`boards/a2mega/src/`](../../a2mega/src/).** The a2p25 displayport1 board
and the a2mega 1.0a3 carry the same ESP32-S3-MINI-1-N4R2 module and the same 8-bit OSPI link
to the FPGA, so one firmware tree serves both, selected by `BOARD_REV`:

```sh
cd boards/a2mega/src/a2fpga_esp32
make BOARD_REV=225 compile                      # a2p25 (103 = a2mega 1.0a3)
make BOARD_REV=225 PORT=/dev/cu.usbmodemXXXX upload
```

The USB JTAG bridge that `openFPGALoader -c esp32s3` talks to is part of that same firmware;
the standalone sketch is `boards/a2mega/src/esp32_usb_jtag/`.

Build details, the upload port, and what the shared firmware does on this board are in
[docs/ESP32_FW_A2P25.md](../docs/ESP32_FW_A2P25.md); the status of the feature port is in
[docs/ESP32_FEATURE_PORT.md](../docs/ESP32_FEATURE_PORT.md).

The previous a2p25 revision's firmware (LCAM capture link, ES5503 on the ESP32, 3-wire SPI)
was deleted from this directory on 2026-10-25; it is in git history before that date, and its
notes are archived at [docs/archive/](../docs/archive/README.md). The ES5503-on-ESP32 path is
abandoned; the LCAM capture link is the part worth revisiting if a low-latency FPGA-to-ESP32
transport is ever needed.
