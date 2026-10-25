# boards/a2p25/docs/archive

Status: created 2026-10-25 by the documentation consolidation
(`docs/DOCS_AUDIT_2026-10.md`, a fork-only record).
Nothing here is maintained. These describe the rev-0 a2p25 (Tang Primer 25K
plus an ESP32-S3 running the ES5503 over an LCD_CAM capture link); the
displayport1 board revision replaced all of it with a fabric DOC5503 on
HyperRAM and the 8-bit OSPI ESP32 link shared with the a2mega. The rev-0
gateware survives as `boards/a2p25/hdl/legacy/` built by
`legacy_a2p25_rev0.gprj`.

| File | Was | Superseded by | Period |
|---|---|---|---|
| [LCAM_SESSION_NOTES.md](LCAM_SESSION_NOTES.md) | Investigation and playbook for lossless ES5503 burst capture on the ESP32 LCD_CAM peripheral | [MODERNIZATION_PLAN.md](../MODERNIZATION_PLAN.md) (fabric DOC on HyperRAM, no capture link) and [ESP32_FW_A2P25.md](../ESP32_FW_A2P25.md) | 2026-01 to 2026-06 |
| [ESP32_SPI_PROTOCOL.md](ESP32_SPI_PROTOCOL.md) | The 3-wire SPI protocol between the ESP32 and `esp32_spi_connector.sv` / `esp32_spi_proto_proc.sv` (still built by `legacy_a2p25_rev0.gprj` only) | [boards/a2mega/docs/ESP32_OSPI_DESIGN.md](../../../a2mega/docs/ESP32_OSPI_DESIGN.md), the 8-bit OSPI link `a2p25.gprj` builds; `docs/coprocessor-interface.md` | 2026-06 |
