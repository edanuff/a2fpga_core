# a2p25 standalone PSRAM (HyperRAM) test

Bring-up and error-rate measurement for the S27KL0642 on the displayport1
board. It does not depend on the Apple bus, DP or the ESP32 OPI link. Plan
context: [MODERNIZATION_PLAN.md](MODERNIZATION_PLAN.md) §2 (track B) and §4–5.

## Pieces

| Piece | Where |
|---|---|
| Controller | `hdl/hyperram/hyperram_ctrl.sv` + `hyperram_phy_gw5a.sv` |
| Self-test engine + register map | `hdl/hyperram/hyperram_bist.sv` (map in its header) |
| Console (UART ↔ registers) | `boards/a2p25/hdl/psram_test/hr_uart_cmd.sv` |
| Top | `boards/a2p25/hdl/psram_test/a2p25_psram_test_top.sv` |
| Projects | `a2p25_psram_test_ck108.gprj`, `_ck81`, `_ck54` (HyperBus CK in MHz; logic = CK/2) |
| Simulation | `make -C boards/a2p25/tests psram_test_top` (whole bitstream over the UART against the Infineon model), plus `hyperram` / `hyperram_bist` |

The console runs over the two ESP32 link lines the bitstream borrows. The
FPGA's TX on C2 goes to ESP32 IO11, and ESP32 IO12 goes to the FPGA's RX on
F2. This needs the ESP32 firmware a2p25 variant (`make BOARD_REV=225`, the
default `DP_TEST=1` mode). In its serial **forwarding mode** (the default; the
CLI is entered with the escape sequence) the USB console *is* the FPGA
console. FPGA lines are also teed to telnet.

## What it does on its own

1. It waits tVCS, writes CR0 (`0x8F17`: 34 Ω, latency 6, variable latency),
   and calibrates the input delay.
2. It then autostarts a continuous test over the whole device, except the top
   16 words, which are the calibration block. Each pass:
   - writes a PRBS pattern with random burst lengths of 1–16 words;
   - overwrites it with the inverted pattern under random byte masks;
   - reads back and compares.
   The seed changes every pass.
3. Every second it prints a status line:
   ```
   S 02=STATUS 12=ERR_WORDS 13=ERR_BITS 1B=PASS_COUNT 21=CAL_INFO
   ```
   - `CAL_INFO` = `[7:0]` live tap, `[15:8]` calibrated tap, `[20:16]` read
     arrival, `[21]` cal_ok, `[22]` init_done.

LED (B10) is a heartbeat that toggles in every state. It is clocked from the
cascaded HyperRAM PLL, so blinking also proves both PLLs locked. A dark or
solid LED means the bitstream or its clocks are not running.

| LED | Meaning |
|---|---|
| ~1.5 Hz | calibrating, or running with no errors |
| ~6 Hz | errors seen or calibration failed |

The errors flag is updated from the status line, so it needs `F2` = 1.

## Console commands

Hex, case-insensitive, one per line:

- `R aa` replies `aa=dddddddd`.
- `W aa dddddddd` replies `OK`.
- Anything else replies `?`.

Local registers:

| Reg | Meaning |
|---|---|
| `F0` | `0x5053`·CK MHz |
| `F1` | uptime in seconds |
| `F2` | bit 0 = periodic status on/off |
| `F3` | scratch |

Everything else is the BIST map; see the `hyperram_bist.sv` header.

**Before a manual run, stop the autostart and quiet the console:**
```
W F2 0          status line off
W 01 2          STOP (wait for 02 bit0 = 0)
```

### Error-rate run (fixed number of passes)
```
W 03 E1         PRBS, RUN=2 continuous, random length + random mask
W 03 D1         ... or RUN=1 (N passes): PRBS, rand len + mask
W 06 64         100 passes
W 04 0          start word
W 05 1FFFF0     length = whole device minus the calibration block
W 07 1          seed
W 01 5          CLEAR + START
R 02            bit0 busy, bit1 done, bit4 errors seen
R 11 / R 12 / R 13    words compared / error words / error bits
R 14 / R 15 / R 16    first error: address (bit31 = valid), expected, actual
R 19 / R 1A           2x-latency transactions / read timeouts
```
At CK 108 one full pass over 8 MB takes well under a second. Leave
continuous mode running for a soak and read `11/12/13`; the
bit error rate is `ERR_BITS / (32 × WORDS_RD)`.

### Eye scan (input delay margin)
```
W 03 F0101      PRBS, BURST=16, EYE
W 05 1000       4096-word working range
W 08 400008     step 8 taps, first tap 0, 64 reads per tap
W 01 5          CLEAR + START; wait for 02 bit1 (done)
R 02            [31:16] = number of eye entries
W 30 0 / R 31   entry 0: [31:24] tap, [23:0] error words; repeat for 1, 2, ...
```
- **Taps:** each tap is about 12.5 ps (Gowin model figure; confirm on
  silicon). The 256 taps cover about 3.2 ns of a 4.63 ns half-period at 108 MHz.
- **Window at the end of the range:** if the error-free window touches tap 0
  or tap 255, the capture phase is near an edge. Note it; the fix is a
  phase-shifted `clk_cap`.
- **Cross-check:** `R 20` is the boot calibration's pass map (bin = 8 taps).
  It should agree with the eye scan.

### Drive strength / latency sweep (CR0)
CR0 = `{1, DRIVE[2:0], 1111, LAT[3:0], FIXED, 1, 11}`.

- **High byte, drive strength:**

  | Drive | 34 Ω (default) | 115 Ω | 67 Ω | 46 Ω | 27 Ω | 22 Ω | 19 Ω |
  |---|---|---|---|---|---|---|---|
  | High byte | `8F` | `9F` | `AF` | `BF` | `DF` | `EF` | `FF` |

- **Low byte, latency:**

  | Latency | Max CK | Variable | Fixed |
  |---|---|---|---|
  | 5 | 133 MHz | `07` | `0F` |
  | 6 | 166 MHz | `17` | `1F` |
  | 7 | 200 MHz | `27` | `2F` |
  | 3 | 85 MHz | `E7` | — |
  | 4 | 104 MHz | `F7` | — |

  Latencies 3 and 4 are simulation-untested and only legal at the lower CK
  variants.

```
W 01 2          STOP
W 09 8F07       e.g. 34 ohm, latency 5, variable
W 01 10         CR0_WRITE
R 22            CR0_ACTIVE (should read back the new value)
W 01 8          RECAL — re-run delay calibration
R 21 / R 20     calibration result
W 01 5          restart the test
```
⚠ Never touch CR1. The board leaves CK# floating, which is only in spec with
CR1[6] at its default (single-ended). The controller has no CR1 path.

### Manual tap override
```
W 0A 1A0        force tap 0xA0 (bit8 = force)
W 0A 0          release (restores the calibrated tap)
```

## Bench matrix to record

Run it for each CK variant (108, then 81 and 54 if 108 shows errors):

| CK | CR0 | cal_map | tap | eye window (taps) | passes | words | err words | err bits | 2x count |
|---|---|---|---|---|---|---|---|---|---|
| 108 | 8F17 | | | | | | | | |

**Also check on the scope:**
- CK vs DQ phase. CK edges should sit centred in the DQ eye on writes; the
  +90° clock comes from PLL phase parameters the Gowin simulation model
  ignores.
- The CK duty cycle.
- Ringing on CK at J11.

## Build provenance

See [MODERNIZATION_PLAN.md](MODERNIZATION_PLAN.md) §8.
