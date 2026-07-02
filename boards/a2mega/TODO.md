# A2Mega TODO

## Status

**Work In Progress** - Tang Mega 60K version, coming soon.

## High Priority

- [ ] Test the OSPI interface from the ESP32 to ensure it works
- [ ] Complete the OSPI control interface for:
  - [ ] ESP32 OSD display capability
  - [ ] Disk II emulation support

## Medium Priority

- [ ] Complete the FAT32 SD Card support
- [ ] Enable configuration through a web-based UX rather than OSD
- [ ] Investigate using the PPO interface to treat the FPGA as an LCD display
- [ ] Implement IIgs acceleration (similar to Transwarp GS) —
      see [docs/twgs_action_plan.md](docs/twgs_action_plan.md) for the phased plan.
      Status: Phase 0/1 gateware landed (GS pin constraints, `hdl/twgs/` bus engine
      + P65C816 core in the project, monitor-mode default). Next: verify a build
      with `gw_sh` (not available in the authoring environment), then Phase 1
      bench monitoring and Phase 2 bring-up with `TWGS_CPU_ENABLE=1`.
      Hardware: /VP drive path needs a board rev (vector-snapshot workaround
      in gateware covers current boards)
- [ ] Implement and test SDRAM support

## Low Priority / Future

- [ ] Document ESP32 to FPGA communication protocol
- [ ] Create web-based configuration UI

## Architecture Notes

- Uses ESP32 for control/configuration instead of internal PicoSoC
- OSPI interface between ESP32 and FPGA
- Tang Mega 60K provides significantly more FPGA resources

## Build Status

- Last verified build: Unknown
- Board design in progress
