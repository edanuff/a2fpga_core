// board_pins.h — a2mega ESP32-S3 pin assignments, selected by board revision.
//
// The 1.0a3 board spin re-shuffled the ESP32 pinout almost everywhere and
// replaced HDMI + micro-SD + USB-A with a single USB-C port (DP Alt Mode +
// USB2 device + PD). Sources of truth: a2-mega repo `mcu.kicad_sch` /
// `usb_c.kicad_sch` on the matching branch, and
// boards/a2mega/docs/BRINGUP_1_0A3_PLAN.md.
//
// Select with -DA2MEGA_BOARD_REV=102 (1.0a2/1.0a2a boards), 103 (1.0a3) or
// 225 (a2-p25 "displayport1" rev: Tang Primer 25K + USB-C DP alt mode; see
// boards/a2p25/docs/ESP32_FW_A2P25.md). This branch defaults to 103. The
// Makefile forwards BOARD_REV=... (and A2P25 DP_TEST=...) if set.
#ifndef A2MEGA_BOARD_PINS_H
#define A2MEGA_BOARD_PINS_H

#ifndef A2MEGA_BOARD_REV
#define A2MEGA_BOARD_REV 103
#endif

// ---------------------------------------------------------------------------
// Common to all revisions
// ---------------------------------------------------------------------------
#define PIN_FPGA_DONE  48   // FPGA configuration DONE

#if A2MEGA_BOARD_REV == 102 || A2MEGA_BOARD_REV == 103
// a2mega (all spins): debug UART + JTAG on the same pads
#define A2_BOARD_NAME  "a2mega"
#define PIN_RXD        44   // UART0 from FPGA (ESP32_RX)
#define PIN_TXD        43   // UART0 to FPGA   (ESP32_TX)
#define PIN_TCK        40   // JTAG to FPGA (bit-banged self-update + USB bridge)
#define PIN_TMS        41
#define PIN_TDI        42
#endif

#if A2MEGA_BOARD_REV == 103
// ---------------------------------------------------------------------------
// 1.0a3 — USB-C DP Alt Mode board (no SD, no USB-A, no HDMI)
// Module: ESP32-S3-MINI-1-N4R2 (4 MB flash + 2 MB quad PSRAM, as on the
// a2p25). The footprint is pin-compatible with the 1.0a2x N8, so either
// variant can be fitted; the as-shipped 1.0a3 build is FlashSize=4M,
// PSRAM=enabled, PartitionScheme=no_ota (see Makefile).
// ---------------------------------------------------------------------------
#define A2MEGA_HAS_SD       0
#define A2MEGA_HAS_USBC_PD  1

#define PIN_TDO        39   // moved off IO45 (VDD_SPI strap) on 1.0a3
// The USB JTAG bridge needs a TRST GPIO but the FPGA has none; park it on
// ESP32_GPIO5 (J2.4), a spare software-assigned line the gateware ignores.
// Do NOT use IO45 (VDD_SPI strap: driven high across an ESP32 reset it would
// re-strap the flash rail) or IO3 (FUSB302B INT_N on this rev).
#define PIN_SRST       38

// Shared I2C bus: FUSB302B PD PHY (0x22) + TUSB1046A DP mux (0x12)
#define PIN_I2C_SCL     1
#define PIN_I2C_SDA     2
#define PIN_FUSB_INT    3   // FUSB302B INT_N, open-drain, R19 pull-up
                            // (ESP32 strapping pin — input only, never drive)

#define PIN_FPGA_INT_N  4   // ESP32_INT_N to the FPGA (J2.67), role TBD

// PPO bus (narrowed to 4 bits on 1.0a3; idle — reserved for DOC-on-ESP32)
#define PIN_PPO_SYNC    5
#define PIN_PPO_D0      6
#define PIN_PPO_D1      7
#define PIN_PPO_D2      8
#define PIN_PPO_D3      9
#define PIN_PPO_CLK    10

// Octal SPI link to the FPGA
#define PIN_OPI_D0     11
#define PIN_OPI_D1     12
#define PIN_OPI_D2     13
#define PIN_OPI_D3     14
#define PIN_OPI_D4     15
#define PIN_OPI_D5     16
#define PIN_OPI_D6     17
#define PIN_OPI_D7     18
#define PIN_OPI_CS     21   // net exists; protocol uses sync framing, unused
#define PIN_OPI_CLK    47

// Spare software-assigned lines to the FPGA (schematic ESP32_GPIO0..5,
// J2.17/13/11/7/5/4). Allocations below must match the gateware .cst.
#define PIN_ESP_GPIO0  33
#define PIN_ESP_GPIO1  34
#define PIN_ESP_GPIO2  35
#define PIN_ESP_GPIO3  36
#define PIN_ESP_GPIO4  37
#define PIN_ESP_GPIO5  38   // consumed as PIN_SRST dummy (see above)

// DP hot-plug to the FPGA: HPD is not a wire on 1.0a3 (TUSB1046A HPD pins
// are NC) — firmware reconstructs level + 0.5-1.0 ms IRQ pulses from PD
// Status/Attention VDMs and drives them here for auxch/hotplug_decode.v.
#define PIN_DP_HPD_OUT PIN_ESP_GPIO0

// VBUS source switch (TPS2553 EN, R29 pull-down). ESP32 strapping pin:
// keep low at boot; drive high only to source 5 V out the USB-C port.
#define PIN_VBUS_SRC_EN 46

#elif A2MEGA_BOARD_REV == 102
// ---------------------------------------------------------------------------
// 1.0a2 / 1.0a2a — HDMI + micro-SD + USB-A board
// ---------------------------------------------------------------------------
#define A2MEGA_HAS_SD       1
#define A2MEGA_HAS_USBC_PD  0

#define PIN_TDO        45
#define PIN_SRST        3   // unused and unconnected; the JTAG bridge needs a pin

#define PIN_SD_CLK     37
#define PIN_SD_CMD     36
#define PIN_SD_D0      38
#define PIN_SD_D1      39
#define PIN_SD_D2      35   // verify at bring-up (schematic pin 31 net inferred)
#define PIN_SD_D3      34
#define PIN_SD_DET     46   // low when a card is inserted

#define PIN_OPI_D0      1
#define PIN_OPI_D1      2
#define PIN_OPI_D2      4
#define PIN_OPI_D3      5
#define PIN_OPI_D4      6
#define PIN_OPI_D5      7
#define PIN_OPI_D6      8
#define PIN_OPI_D7      9
#define PIN_OPI_CS     21   // net exists; protocol uses sync framing, unused
#define PIN_OPI_CLK    47

#elif A2MEGA_BOARD_REV == 225
// ---------------------------------------------------------------------------
// a2-p25, branch "displayport1" — Tang Primer 25K SOM, 64 Mb HyperRAM,
// USB-C DP alt mode: FPGA MIPI D-PHY -> SN75DP130 redriver -> TUSB546A mux
// -> USB2 receptacle, FUSB302B PD PHY, TPS2553 VBUS source switch.
// Module: ESP32-S3-MINI-1-N4R2 (same as a2mega 1.0a3; same Makefile flash/
// PSRAM settings). Native USB (IO19/20) on its own USB-C (USB1), separate
// from the video port, so the CDC console stays usable during DP tests.
// Source of truth: kicad-cli netlist of the a2-p25 board repo @
// displayport1 (mcu.kicad_sch "ESP32 & IO" sheet). Full table + sources:
// boards/a2p25/docs/ESP32_FW_A2P25.md.
// ---------------------------------------------------------------------------
#define A2_BOARD_NAME       "a2p25"
#define A2MEGA_HAS_SD       0
#define A2MEGA_HAS_USBC_PD  1
#define A2MEGA_HAS_DP130    1   // SN75DP130 redriver on the shared I2C bus

// dp_test mode (default ON until a displayport1 production core exists):
// the bring-up bitstream (boards/a2p25/hdl/dp_test/a2p25_dp_test_top.sv)
// repurposes two ESP32-link balls — B2 (ESP32_INT_N) is its HPD INPUT and
// C2 (ESP32_OPI_D0) its 115200 8N1 telemetry TX. In this mode the OSPI link
// is never initialized; IO10 drives HPD and UART1 RX is matrix-routed to
// IO11, UART1 TX to IO12 (ESP32_OPI_D1 = ball F2: the PSRAM test
// bitstream's console RX — serial forwarding mode makes it the USB console). Build with DP_TEST=0 for a core that speaks OSPI (IO10 then stays
// an input: in production the FPGA drives ESP32_INT_N).
#ifndef A2P25_DP_TEST
#define A2P25_DP_TEST       1
#endif

// FPGA JTAG (dedicated balls C1/B1/A3/A2; also on header J3). NOTE the
// order differs from the a2mega (TCK is IO39 here, TDO IO42).
#define PIN_TCK        39   // ESP32_JTAG_TCK, R31 5.1k pull-down
#define PIN_TMS        40   // ESP32_JTAG_TMS
#define PIN_TDI        41   // ESP32_JTAG_TDI
#define PIN_TDO        42   // ESP32_JTAG_TDO
// USB JTAG bridge TRST dummy: IO7 is an unconnected module pad. Do NOT use
// IO38 here — on this board it is the FPGA's RECONFIG_N.
#define PIN_SRST        7

#define PIN_FPGA_RECONFIG_N 38   // ESP32_RECONFIG_N (R32 1k pull-up); leave
                                 // as input, pulse low only to reconfigure

// Shared I2C bus "DP_I2C": FUSB302BMPX (0x22) + TUSB546A-DCI (I2C_EN = 1k
// to 3V3 -> I2C mode; A0/A1 float -> 0x12 as on the a2mega's TUSB1046A,
// auto-detected at boot) + SN75DP130 (ADDR_EQ = GND -> 0x2C). 10k pull-ups.
#define PIN_I2C_SCL     4
#define PIN_I2C_SDA     3   // IO3 = JTAG-source strap, only read if the
                            // STRAP_JTAG_SEL eFuse is burned (it is not);
                            // R23 pull-up keeps it high at reset anyway
#define PIN_FUSB_INT    5   // FUSB302B INT_N, open-drain, R25 10k pull-up

// VBUS source switch (TPS2553 EN, R46 1k pull-down). Not a strapping pin
// on this board (the a2mega 1.0a3 used IO46).
#define PIN_VBUS_SRC_EN 6

#define PIN_FPGA_INT_N 10   // ESP32_INT_N <-> FPGA ball B2

// Octal SPI link to the FPGA (D0-D3 balls C2/F2/F1/A1, D4-D7 K1/K2/J4/K4)
#define PIN_OPI_D0     11
#define PIN_OPI_D1     12
#define PIN_OPI_D2     13
#define PIN_OPI_D3     14
#define PIN_OPI_D4     15
#define PIN_OPI_D5     16
#define PIN_OPI_D6     17
#define PIN_OPI_D7     18
#define PIN_OPI_CS     21   // ball E1; protocol uses sync framing, unused
#define PIN_OPI_CLK    47   // ball D1 (not a GCLK ball; fabric oversamples)

// Apple II /RES driver: IO33 -> U1 74LVC2G06 (open-drain INVERTER) ->
// slot pin 31. HIGH = /RES ASSERTED. R1 4.7k pulls IO33 high, so the
// Apple II is held in reset whenever the ESP32 is in reset, in download
// mode, or not driving the pin — firmware must drive it LOW at boot.
#define PIN_A2_RESET_OUT    33
#define A2_RESET_ASSERT_LEVEL 1

// Status LEDs (active high, 549R to GND) and the BOOT button (S2, IO0,
// strapping pin — read only, never drive). S1 = EN (CHIP_PU).
#define PIN_LED0        1   // D6: ESP32 heartbeat
#define PIN_LED1        2   // D7: HPD level sent to the FPGA
#define PIN_BOOT_BTN    0

// Not connected on this board: IO7 (TRST dummy above), IO8, IO9, IO26,
// IO34-IO37, IO45, IO46, TXD0 (IO43), RXD0 (IO44). No SD, no PPO bus.

#if A2P25_DP_TEST
#define PIN_DP_HPD_OUT       PIN_FPGA_INT_N  // dp_test: B2 = HPD input
#define A2MEGA_HAS_FPGA_UART 1
#define PIN_RXD              PIN_OPI_D0      // dp_test telemetry (ball C2)
#define PIN_TXD              PIN_OPI_D1      // ESP32 -> FPGA console (ball F2):
                                             // a2p25_psram_test's hr_uart_cmd;
                                             // ignored by the dp_test bitstream
#define A2MEGA_HAS_OSPI_LINK 0
#else
#define PIN_DP_HPD_OUT       -1   // HPD must travel as an OSPI register write
#define A2MEGA_HAS_FPGA_UART 0
#define A2MEGA_HAS_OSPI_LINK 1
#endif

#else
#error "Unknown A2MEGA_BOARD_REV (expected 102, 103 or 225)"
#endif

// ---------------------------------------------------------------------------
// Feature defaults (a2mega revisions keep their historical behavior)
// ---------------------------------------------------------------------------
#ifndef A2MEGA_HAS_FPGA_UART
#define A2MEGA_HAS_FPGA_UART 1    // FPGA debug UART on PIN_RXD/PIN_TXD
#endif
#ifndef A2MEGA_HAS_OSPI_LINK
#define A2MEGA_HAS_OSPI_LINK 1    // bring up the OSPI link + menu/disk tasks
#endif
#ifndef A2MEGA_HAS_DP130
#define A2MEGA_HAS_DP130 0
#endif
#ifndef PIN_A2_RESET_OUT
#define PIN_A2_RESET_OUT -1       // a2mega: the FPGA owns the Apple /RES driver
#endif
#ifndef PIN_LED0
#define PIN_LED0 -1
#endif
#ifndef PIN_LED1
#define PIN_LED1 -1
#endif

#endif // A2MEGA_BOARD_PINS_H
