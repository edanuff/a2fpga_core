#!/usr/bin/env python3
"""fpga_stream.py — flash a Gowin .bin to an a2mega board over WiFi.

Streams the binary bitstream to the board's fpgastream service (port
2323), which writes it into the FPGA's config flash via the ESP32's
native SPI-over-JTAG driver, verifies every page, reloads the FPGA, and
restarts itself. No USB, no replugs. Bench tool.

Usage:  python3 tools/fpga_stream.py <board-ip> <path/to/image.bin>

Use the .bin (binary bitstream, ~2.3 MB, emitted next to the .fs by the
Gowin tools). The .fs ASCII form is rejected by the board's validator.
On success the board reboots: telnet drops and the heartbeat confirms
the new image (~10 s).
"""
import socket
import sys
import time
import pathlib

PORT = 2323
CHUNK = 4096
TIMEOUT_S = 600


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    host, path = sys.argv[1], pathlib.Path(sys.argv[2])
    if path.suffix == ".fs":
        print("error: send the .bin (binary bitstream), not the .fs "
              "(ASCII); it sits next to the .fs in impl/pnr/")
        return 2
    data = path.read_bytes()
    print(f"{path.name}: {len(data)} bytes -> {host}:{PORT}")

    t0 = time.time()
    with socket.create_connection((host, PORT), timeout=30) as s:
        s.settimeout(TIMEOUT_S)
        s.sendall(f"FPGA {len(data)}\n".encode())
        # Stream; the board paces us via TCP flow control while it
        # programs/verifies pages.
        for off in range(0, len(data), CHUNK):
            s.sendall(data[off:off + CHUNK])
        # Read progress + final status lines.
        buf = b""
        while True:
            try:
                r = s.recv(256)
            except socket.timeout:
                print("timeout waiting for board status")
                return 1
            if not r:
                print("connection closed without final status")
                return 1
            buf += r
            while b"\n" in buf:
                line, buf = buf.split(b"\n", 1)
                text = line.decode(errors="replace").strip()
                if text == "E":
                    print(f"  erased ({time.time()-t0:.0f}s)")
                elif text.startswith("P "):
                    done = int(text[2:])
                    pct = 100 * done / len(data)
                    print(f"  {done//1024:5d} KB  {pct:5.1f}%  "
                          f"({time.time()-t0:.0f}s)", end="\r")
                elif text == "OK":
                    print(f"\nOK — verified, reloading FPGA + ESP32 "
                          f"restart ({time.time()-t0:.0f}s total)")
                    print("confirm: heartbeat LED, board back on "
                          "network in ~10 s")
                    return 0
                elif text.startswith("ERR"):
                    print(f"\nboard: {text}")
                    return 1


if __name__ == "__main__":
    sys.exit(main())
