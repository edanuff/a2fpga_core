// board_pins.h — a2mega ESP32-S3 pin assignments, selected by board revision.
//
// The 1.0a3 board spin re-shuffled the ESP32 pinout almost everywhere and
// replaced HDMI + micro-SD + USB-A with a single USB-C port (DP Alt Mode +
// USB2 device + PD). Sources of truth: a2-mega repo `mcu.kicad_sch` /
// `usb_c.kicad_sch` on the matching branch, and
// boards/a2mega/docs/BRINGUP_1_0A3_PLAN.md.
//
// Select with -DA2MEGA_BOARD_REV=102 (1.0a2/1.0a2a boards) or 103 (1.0a3).
// This branch defaults to 103. The Makefile forwards BOARD_REV=... if set.
#ifndef A2MEGA_BOARD_PINS_H
#define A2MEGA_BOARD_PINS_H

#ifndef A2MEGA_BOARD_REV
#define A2MEGA_BOARD_REV 103
#endif

// ---------------------------------------------------------------------------
// Common to all revisions
// ---------------------------------------------------------------------------
#define PIN_RXD        44   // UART0 from FPGA (ESP32_RX)
#define PIN_TXD        43   // UART0 to FPGA   (ESP32_TX)
#define PIN_FPGA_DONE  48   // FPGA configuration DONE
#define PIN_TCK        40   // JTAG to FPGA (bit-banged self-update + USB bridge)
#define PIN_TMS        41
#define PIN_TDI        42

#if A2MEGA_BOARD_REV == 103
// ---------------------------------------------------------------------------
// 1.0a3 — USB-C DP Alt Mode board (no SD, no USB-A, no HDMI)
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

#else
#error "Unknown A2MEGA_BOARD_REV (expected 102 or 103)"
#endif

#endif // A2MEGA_BOARD_PINS_H
