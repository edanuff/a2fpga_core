# A2P25 TODO

## Status

**Work In Progress** - Tang Primer 25K version, coming soon.

## High Priority

- [ ] Fix the LCAM interface which is currently losing data during high speed transfers
- [ ] Complete the ES5503 code which runs on the ESP32-S3

## Medium Priority

- [ ] Complete the FAT32 SD Card support
- [ ] Enable configuration through a web-based UX rather than OSD

## Low Priority / Future

- [ ] Document ESP32-S3 to FPGA communication protocol
- [ ] Create web-based configuration UI

## Architecture Notes

- Uses ESP32-S3 for control/configuration
- LCAM interface between ESP32-S3 and FPGA
- ES5503 (Ensoniq DOC) emulation runs on ESP32-S3

## Known Issues

- LCAM interface losing data during high speed transfers (see High Priority)

## Build Status

- Last verified build: Unknown
- Board design in progress
