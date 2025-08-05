# ESP32-S3 LCD_CAM Peripheral Research Notes

## Problem Summary
- Goal: Use ESP32-S3's LCD_CAM peripheral for 4-bit parallel data capture from FPGA (no physical camera sensor)
- Issue: `esp_camera_init()` causes LoadProhibited exception when no camera sensor connected
- Current Status: JTAG and Serial working, but LCD_CAM DMA functionality not operational

## Root Cause Analysis
The LoadProhibited crash occurs because:
1. **Hardware Detection Failure**: Camera library expects to communicate with physical sensor over I2C during initialization
2. **Sensor Communication**: Uses SIOD/SIOC pins (I2C) to detect and configure camera sensor
3. **No Bypass**: Current ESP32 camera library doesn't provide clean way to skip sensor detection

## ESP32-S3 LCD_CAM Peripheral Key Facts
1. **Completely Redesigned**: ESP32-S3 uses new LCD_CAM peripheral (separate from older I2S-based approach in ESP32/ESP32-S2)
2. **Documentation Gap**: Limited tutorials/examples available, sparse official documentation
3. **Capabilities**: 
   - PCLK frequencies up to 40MHz
   - 8-bit data bus support
   - DMA integration with GDMA
   - Can theoretically work without camera sensor for parallel data capture

## Potential Solutions

### Option 1: Camera Library Bypass Methods
- Try different pixel formats (PIXFORMAT_RAW, PIXFORMAT_RGB565) that might skip sensor config
- Experiment with minimal frame sizes (FRAMESIZE_96X96, custom sizes)
- Test setting all sensor-related pins to -1
- Look for undocumented camera library flags that disable sensor detection

### Option 2: Direct LCD_CAM Register Access
- Use `soc/lcd_cam_reg.h` for direct register manipulation
- Configure LCD_CAM peripheral manually without camera library
- Set up DMA transfers directly using GDMA
- Reference: Adafruit blog posts on "hacking the ESP32-S3 LCD peripheral"
- Potential code location: ESP-IDF `peripherals/lcd` directory

### Option 3: Alternative Libraries/Approaches
- Look for community LCD_CAM libraries that don't assume camera sensor
- Check if newer ESP-IDF versions have better LCD_CAM support
- Investigate using LCD mode instead of camera mode

## Pin Configuration (Current)
```c
// 4-bit Parallel Data Interface from FPGA
#define PIN_PARL_D0 14          // Data bit 0 (LSB)
#define PIN_PARL_D1 15          // Data bit 1
#define PIN_PARL_D2 16          // Data bit 2  
#define PIN_PARL_D3 17          // Data bit 3 (MSB)
#define PIN_PARL_CLK 13         // Clock signal from FPGA
#define PIN_PARL_FRAME 12       // Frame/Valid signal for packet boundaries
```

## Technical Requirements
- Capture 4-bit parallel data from FPGA at ~6.75MHz
- Process Apple II bus packets (32-bit: address+data+flags)
- Use DMA for zero-copy high-performance capture
- Maintain JTAG functionality (already working)

## Next Steps Priority
1. **Research direct LCD_CAM register approach** - most likely to succeed
2. **Try alternative camera configurations** - quick to test
3. **Look for ESP-IDF LCD examples** - might provide working code patterns
4. **Check ESP32-Arduino-lib updates** - newer versions might have better support

## Useful Resources Found
- ESP-FAQ Camera Application documentation
- Adafruit ESP32-S3 LCD peripheral hacking blog posts
- ESP32 Forum discussions on LCD_CAM peripheral
- ESP-IDF peripherals/lcd directory examples
- `soc/lcd_cam_reg.h` register definitions

## Status
- ✅ JTAG bridge working with openFPGALoader
- ✅ Serial communication working  
- ✅ GPIO pins configured for parallel data
- ❌ LCD_CAM DMA functionality not operational (crashes on init)
- 🔄 Need to implement direct register access or find camera library workaround