# boards/a2p25/docs/archive

Status: created 2026-10-25 by the documentation consolidation
(`docs/DOCS_AUDIT_2026-10.md`, a fork-only record).
Nothing here is maintained. These describe the previous a2p25 revision (Tang
Primer 25K plus an ESP32-S3 running the ES5503 over an LCD_CAM capture link).
The displayport1 revision replaced it with a fabric DOC5503 on HyperRAM and the
8-bit OSPI ESP32 link shared with the a2mega; the board is unreleased and the
previous revision is not maintained. **The ES5503-on-ESP32 path is abandoned.
The LCAM capture link itself is kept as useful work:** it reached lossless
32,768-packet bursts at 13.5 MB/s and is a candidate low-latency FPGA-to-ESP32
transport, possibly on the a2mega (ed, 2026-10-25). The previous revision's
gateware is `boards/a2p25/hdl/legacy/` (`legacy_a2p25_rev0.gprj`); its
firmware tree was deleted from `boards/a2p25/src/` on 2026-10-25 (in git
history before that date).

| File | Was | Superseded by | Period |
|---|---|---|---|
| [LCAM_SESSION_NOTES.md](LCAM_SESSION_NOTES.md) | Investigation and playbook for lossless burst capture on the ESP32 LCD_CAM peripheral (the working link; read it before reusing LCAM for any transfer) | for the DOC: [MODERNIZATION_PLAN.md](../MODERNIZATION_PLAN.md) (fabric DOC on HyperRAM); for the link itself: nothing yet, reuse candidate | 2026-01 to 2026-06 |
| [ESP32_SPI_PROTOCOL.md](ESP32_SPI_PROTOCOL.md) | The 3-wire SPI protocol between the ESP32 and `esp32_spi_connector.sv` / `esp32_spi_proto_proc.sv` (still built by `legacy_a2p25_rev0.gprj` only) | [boards/a2mega/docs/ESP32_OSPI_DESIGN.md](../../../a2mega/docs/ESP32_OSPI_DESIGN.md), the 8-bit OSPI link `a2p25.gprj` builds; `docs/coprocessor-interface.md` | 2026-06 |
