// a2fpga_jtag.h
// USB JTAG bridge for FPGA programming
#ifndef A2FPGA_JTAG_H
#define A2FPGA_JTAG_H

#include <Arduino.h>
#include "board_pins.h"

// Function declarations
void route_usb_jtag_to_gpio();
void unroute_usb_jtag_to_gpio();

#endif // A2FPGA_JTAG_H
