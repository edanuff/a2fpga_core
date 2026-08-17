// fpgastream.h — network bitstream flashing (bench tool).
//
// TCP server (port 2323) that streams a Gowin .bin straight into the
// GW5A's external config flash via the native SPI-over-JTAG driver —
// no USB, no staging (LittleFS cannot hold an image; TCP flow control
// paces the sender). Client: tools/fpga_stream.py.
//
// Protocol:  client sends  "FPGA <size>\n"  + exactly <size> raw bytes.
//            server sends  "E\n" after erase, "P <bytes>\n" every 64 KB,
//                          then "OK\n" or "ERR <message>\n".
// Success reloads the FPGA from the new image and restarts the ESP32
// (same semantics as the menu updater).
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void fpgastream_start(void);

#ifdef __cplusplus
}
#endif
