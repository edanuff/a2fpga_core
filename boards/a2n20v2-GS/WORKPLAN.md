# A2N20v2-GS SDRAM Framebuffer Workplan

## Goal

Add SDRAM-based framebuffer support to the a2n20v2-GS board (Tang Nano 20K), enabling beam-racing game effects (e.g. Arkanoid, Bugz) to display correctly via HDMI. This combines:

- **SDRAM infrastructure** from the a2n20v2-Enhanced board
- **Framebuffer architecture** from the a2mega DDR3 framebuffer
- Adapted for SDRAM's simpler timing (single logic clock domain, no async FIFO needed)

## Current State

The a2n20v2-GS board currently has:
- Basic video pipeline: `apple_video` -> `vgc` -> `SuperSprite` -> `DebugOverlay` -> HDMI
- 480p HDMI output (720x480 @ 59.94 Hz, VIDEO_ID_CODE=2)
- No SDRAM — all memory is BRAM-only
- No framebuffer — video is rendered in real-time (beam racing doesn't work)

## Architecture Overview

### Why a Framebuffer?

The current `apple_video` module renders pixels in the HDMI pixel clock domain (27 MHz), synchronized to HDMI scan position. Apple II software that relies on precise CPU-to-beam timing ("beam racing") fails because the FPGA's HDMI timing doesn't match the Apple II's native video timing.

A framebuffer solves this by:
1. Rendering video in the **Apple II native timing domain** (54 MHz logic clock, driven by `scan_timer`)
2. Storing rendered pixels in **SDRAM**
3. Reading pixels back at **HDMI pixel clock rate** for display

### Clock Domains (Simplified vs DDR3)

The SDRAM approach is significantly simpler than the a2mega's DDR3 approach:

| Domain | DDR3 (a2mega) | SDRAM (a2n20v2-GS) |
|--------|---------------|---------------------|
| Logic clock | 54 MHz | 54 MHz |
| Memory controller | 74.25 MHz (separate PLL) | 54 MHz (same as logic) |
| Memory PHY | 297 MHz (DDR3 PLL) | 54 MHz phase-shifted |
| Pixel clock | 27 MHz | 27 MHz |
| TMDS clock | 135 MHz | 135 MHz |
| **Total domains** | **4** | **2** (logic + pixel) |

Since the SDRAM controller runs on the same 54 MHz logic clock, **no async FIFO** is needed between the framebuffer write path and memory — writes go directly through a `mem_port_if` port.

### Data Flow

```
Apple II VRAM (BRAM, 54 MHz logic clock)
    |
    v
[apple_video_fb / vgc_fb] -- renders pixels using scan_timer timing
    |
    v  fb_we + fb_data (RGB666, 18-bit)
[sdram_framebuffer] -- pixel write accumulator
    |
    v  mem_port_if.wr + .addr + .data
[sdram_ports] -- SDRAM controller with multi-port arbitration
    |
    v  (SDRAM at 54 MHz, 32-bit data bus)
[Tang Nano 20K on-chip SDRAM, 8MB]
    |
    ^  mem_port_if.rd + .q (read back)
[sdram_framebuffer] -- line fetch FSM
    |
    v  line_buf write (54 MHz)
[Line Buffer BRAM] -- true dual-port, hardware CDC
    |
    v  line_buf read (27 MHz pixel clock)
[480p output logic] -- borders, 2x vertical scaling, scanline dimming
    |
    v  RGB888
[HDMI encoder] -- 720x480 @ 59.94 Hz
```

### Display Layout

```
720 pixels wide:  [80 border] [560 active] [80 border]   (Apple II modes)
                  [40 border] [640 active] [40 border]   (IIgs SHR modes)
480 lines tall:   [48 border] [384 active = 192x2] [48 border]   (Apple II)
                  [40 border] [400 active = 200x2] [40 border]   (IIgs SHR)
```

- 2x integer vertical scaling (each framebuffer line -> 2 HDMI lines)
- Odd HDMI lines dimmed to 50% for CRT scanline effect (when enabled)
- Border color from Apple II/IIgs palette

### SDRAM Memory Layout

```
Address space: 21-bit (2MB)
Framebuffer: 640 * 200 * 4 bytes = 512,000 bytes (~500KB at top of address space)
  Base address: 0x180000 (1.5MB offset, leaving lower 1.5MB for system use)
  Each pixel: 32-bit word (18-bit RGB666, 14 bits unused)
  Row stride: 640 words (2560 bytes) -- always 640 wide for uniform addressing

Remaining SDRAM (0x000000 - 0x17FFFF):
  System memory, Ensoniq DOC/GLU, etc.
```

### SDRAM Port Allocation

```
Port 0: FB_READ_PORT    -- Framebuffer line reads (highest priority, display-critical)
Port 1: VIDEO_MEM_PORT  -- Apple II VRAM reads for rendering
Port 2: MAIN_MEM_PORT   -- CPU memory (main + aux banks)
Port 3: FB_WRITE_PORT   -- Framebuffer pixel writes (lowest among active ports)
Port 4: DOC_MEM_PORT    -- Ensoniq DOC (if enabled)
Port 5: GLU_MEM_PORT    -- Ensoniq GLU (if enabled)
```

Framebuffer reads get highest priority since display stalls cause visible artifacts. Framebuffer writes are lower priority since the write accumulator can buffer pixels.

### SDRAM Bandwidth Budget

At 54 MHz with 32-bit bus, single-word access:
- Theoretical max: ~54M words/sec, but with overhead (activate, precharge, refresh): ~10-15M words/sec effective
- **FB read (per line)**: 640 reads per 2 scanlines (~127 µs) = ~5,040 reads/sec per line x 200 lines = ~1M reads/sec
- **FB write (per frame)**: 560-640 writes x 192-200 lines x 60 fps = ~7.7M writes/sec
- **Video VRAM reads**: ~2,250 reads per scanline x 262 lines x 60 fps = ~35M reads/sec (but shares with existing BRAM path)
- Total FB bandwidth: ~8.7M accesses/sec — well within SDRAM capacity

**Note**: Video VRAM reads are from BRAM (like current design), not SDRAM. Only the framebuffer pixels go through SDRAM. This keeps bandwidth manageable.

---

## Implementation Phases

### Phase 1: Add SDRAM Infrastructure

**Goal**: Get SDRAM controller running on a2n20v2-GS with basic port access.

**Files to create/modify**:

1. **`boards/a2n20v2-GS/hdl/top.sv`** — Add SDRAM port declarations and `sdram_ports` instantiation
   - Add SDRAM I/O ports (O_sdram_clk, O_sdram_addr, IO_sdram_dq, etc.)
   - Add `mem_port_if` array instantiation
   - Instantiate `sdram_ports` controller
   - Wire phase-shifted clock from PLL to SDRAM

2. **`boards/a2n20v2-GS/hdl/a2n20v2_gs.cst`** — No SDRAM pin changes needed (Gowin auto-manages on-chip SDRAM via "magic" signal names)

3. **`boards/a2n20v2-GS/hdl/gowin/clk_logic/clk_logic.v`** — Verify PLL outputs include phase-shifted clock for SDRAM (already has `clkoutp`)

4. **`boards/a2n20v2-GS/a2n20v2_gs.gprj`** — Add SDRAM source files to project:
   - `hdl/sdram/sdram.sv`
   - `hdl/sdram/sdram_ports.sv`
   - `hdl/memory/mem_port_if.sv`

**Validation**: SDRAM initialization completes (LED indicator or debug overlay)

---

### Phase 2: Add Scan Timer and Framebuffer Renderers

**Goal**: Replace `apple_video` with framebuffer-aware renderers driven by `scan_timer`.

**Files to copy from a2mega**:

1. **`boards/a2n20v2-GS/hdl/video/scan_timer.sv`** — Copy from `boards/a2mega/hdl/video/scan_timer.sv`
   - Provides `scanline_o`, `hsync_o`, `vsync_o` driven by Apple II bus timing
   - No modifications needed — it reads from `a2bus_if` directly

2. **`boards/a2n20v2-GS/hdl/video/apple_video_fb.sv`** — Copy from `boards/a2mega/hdl/video/apple_video_fb.sv`
   - Framebuffer-aware Apple II video renderer
   - Outputs `fb_we_o`, `fb_data_o` (RGB666) instead of direct RGB888
   - May need minor adjustments if it references a2mega-specific interfaces

3. **`boards/a2n20v2-GS/hdl/video/vgc_fb.sv`** — Copy from `boards/a2mega/hdl/video/vgc_fb.sv`
   - Framebuffer-aware IIgs Super Hi-Res renderer
   - Same `fb_we_o`, `fb_data_o` interface

**Files to modify**:

4. **`boards/a2n20v2-GS/hdl/top.sv`** — Wire scan_timer and swap video modules
   - Instantiate `scan_timer` connected to `a2bus_if`
   - Replace `apple_video` with `apple_video_fb`
   - Replace `vgc` with `vgc_fb`
   - Add framebuffer mux (selects between apple_video_fb and vgc_fb based on SHRG_MODE)

5. **`boards/a2n20v2-GS/a2n20v2_gs.gprj`** — Add new source files

**Validation**: Framebuffer renderers produce `fb_we`/`fb_data` pulses (observable via debug overlay counters)

---

### Phase 3: Create SDRAM Framebuffer Module

**Goal**: New module that bridges framebuffer pixel writes to SDRAM and reads scanlines back for display.

**New file**: **`boards/a2n20v2-GS/hdl/video/sdram_framebuffer.sv`**

This is the core new module. It replaces `ddr3_framebuffer_480p.v` with an SDRAM-specific implementation that is simpler due to the single memory clock domain.

**Module interface**:
```systemverilog
module sdram_framebuffer #(
    parameter COLOR_BITS = 18    // RGB666
) (
    // Clocks and reset
    input  logic clk,            // 54 MHz logic clock (also SDRAM clock)
    input  logic clk_pixel,      // 27 MHz pixel clock
    input  logic rst_n,

    // Framebuffer write interface (from apple_video_fb / vgc_fb)
    input  logic fb_vsync,       // Frame start
    input  logic fb_we,          // Pixel write enable
    input  logic [COLOR_BITS-1:0] fb_data,  // RGB666 pixel
    input  logic [10:0] fb_width,   // 560 or 640
    input  logic [9:0]  fb_height,  // 192 or 200

    // SDRAM port interfaces (directly use mem_port_if)
    mem_port_if.client fb_write_port,   // For writing pixels to SDRAM
    mem_port_if.client fb_read_port,    // For reading scanlines from SDRAM

    // HDMI scan position (from HDMI encoder, pixel clock domain)
    input  logic [10:0] hdmi_cx,
    input  logic [9:0]  hdmi_cy,

    // Video output (pixel clock domain)
    output logic [23:0] rgb_o,          // RGB888 output

    // Configuration
    input  logic [COLOR_BITS-1:0] border_color,
    input  logic scanline_en            // Enable CRT scanline dimming
);
```

**Internal architecture**:

1. **Write Path** (all in 54 MHz logic clock domain — no FIFO needed):
   - Accept `fb_we`/`fb_data` from renderer
   - Maintain write position counters (`wr_x`, `wr_y`)
   - On `fb_vsync`: reset to (0, 0)
   - On `fb_we`: write pixel to SDRAM via `fb_write_port`
     - Address = `FB_BASE + wr_y * fb_width + wr_x`
     - Data = `{14'b0, fb_data}` (zero-pad to 32 bits)
   - Increment `wr_x`, wrap at `fb_width` and increment `wr_y`
   - Handle `available` signal — if port busy, buffer one pixel

2. **Read Path / Line Fetch FSM** (54 MHz logic clock domain):
   - Monitor HDMI `cy` (pixel clock) via gray-code CDC to get current display line
   - Calculate which framebuffer line to prefetch: `fb_line = (cy - v_border) >> 1`
   - When display line changes (new `fb_line` detected):
     - Fetch entire scanline from SDRAM into line buffer BRAM
     - Issue sequential reads: `fb_read_port.addr = FB_BASE + fb_line * fb_width + x` for x = 0..fb_width-1
     - Write returned data into line buffer BRAM at position `x`

3. **Line Buffer** (true dual-port BRAM, hardware CDC):
   - 1024 entries x 18 bits (2 banks x 512)
   - Write port: 54 MHz (SDRAM read responses)
   - Read port: 27 MHz (HDMI pixel output)
   - Bank select by line parity — prevents read/write collision

4. **Output Path** (27 MHz pixel clock domain):
   - Calculate border regions from `fb_width`, `fb_height`
   - Read line buffer at `hdmi_cx - h_border` offset
   - Expand RGB666 to RGB888 via `torgb()` function
   - Apply scanline dimming on odd lines: `{1'b0, R[7:1], 1'b0, G[7:1], 1'b0, B[7:1]}`
   - Output border color for non-active pixels

**Validation**: Static color fill test — write solid color to SDRAM, verify display output

---

### Phase 4: Integration and Top-Level Wiring

**Goal**: Connect everything in `top.sv` and get end-to-end video working.

**Modify**: **`boards/a2n20v2-GS/hdl/top.sv`**

1. Remove direct `apple_video` → `vgc` → `SuperSprite` → HDMI chain
2. Add framebuffer mux:
   ```
   fb_we = vgc_fb_active ? vgc_fb_we : apple_video_fb_we
   fb_data = vgc_fb_active ? vgc_fb_data : apple_video_fb_data
   ```
3. Instantiate `sdram_framebuffer` with:
   - `fb_write_port` → `mem_ports[FB_WRITE_PORT]`
   - `fb_read_port` → `mem_ports[FB_READ_PORT]`
   - RGB output → SuperSprite → DebugOverlay → HDMI
4. Wire HDMI `cx`/`cy` back to `sdram_framebuffer`
5. Update `DebugOverlay` with framebuffer debug values (write count, line fetch status, etc.)
6. Connect `border_color` from `a2mem_if` soft switches

**Modify**: **`boards/a2n20v2-GS/hdl/a2n20v2_gs.sdc`**
- Add timing constraints for SDRAM ↔ pixel clock CDC paths
- Verify false paths between 54 MHz and 27 MHz domains through line buffer

**Modify**: **`boards/a2n20v2-GS/a2n20v2_gs.gprj`**
- Add all new source files

**Validation**: Full video output — Apple II text mode, graphics modes, IIgs SHR all display correctly via framebuffer

---

### Phase 5: SuperSprite Integration

**Goal**: Ensure VDP overlay compositing works with framebuffer output.

The a2mega's framebuffer renders SuperSprite compositing **into** the framebuffer during `apple_video_fb` rendering. We need to verify this path works:

1. `apple_video_fb` outputs Apple II RGB to SuperSprite module
2. SuperSprite composites VDP graphics
3. Composited output feeds back as `fb_data` for framebuffer writes

**If SuperSprite compositing is NOT in the framebuffer path** (current a2n20v2-GS has it post-framebuffer):
- Option A: Move SuperSprite compositing into the framebuffer write path (matches a2mega)
- Option B: Keep SuperSprite post-framebuffer (simpler, but beam racing won't apply to VDP)

**Validation**: F-18 Interceptor or other SuperSprite-using software displays correctly

---

### Phase 6: Ensoniq DOC Sound Integration

**Goal**: Add Ensoniq DOC/GLU support using SDRAM for 128K sound memory.

This mirrors the a2n20v2-Enhanced implementation:

1. Add Ensoniq DOC and GLU modules
2. Connect to `mem_ports[DOC_MEM_PORT]` and `mem_ports[GLU_MEM_PORT]`
3. Mix Ensoniq audio into the existing audio chain
4. Verify SDRAM bandwidth is sufficient with framebuffer + Ensoniq concurrent access

**Validation**: IIgs system sounds, music playback through HDMI audio

---

### Phase 7: Testing and Optimization

**Goal**: Verify all video modes and optimize SDRAM access patterns.

**Test matrix**:
- [ ] Apple II Text 40-column
- [ ] Apple II Text 80-column
- [ ] Apple II Lo-Res
- [ ] Apple II Hi-Res
- [ ] Apple II Double Hi-Res
- [ ] IIgs Super Hi-Res 320 mode
- [ ] IIgs Super Hi-Res 640 mode
- [ ] Beam racing games (Arkanoid, Bugz)
- [ ] SuperSprite VDP overlay
- [ ] Ensoniq audio playback
- [ ] Scanline effect (DIP switch toggle)
- [ ] Debug overlay toggle (S2 button)
- [ ] Sleep mode (HDMI blank when CPU stopped)

**Optimization opportunities**:
- SDRAM burst mode for sequential line reads (if timing permits)
- Write coalescing (batch multiple pixels per SDRAM transaction)
- Prefetch multiple lines ahead during VBlank

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| SDRAM bandwidth insufficient for FB + system | Low | High | FB reads highest priority; video VRAM stays in BRAM |
| Line fetch doesn't complete within 2-scanline window | Low | Medium | 640 reads at 54 MHz with ~8 cycle overhead = ~95µs, window is ~127µs |
| SDRAM port starvation under heavy FB write load | Medium | Medium | Write accumulator buffers pixels; writes are lower priority |
| apple_video_fb pixel timing doesn't match SDRAM available cycles | Medium | Medium | Buffer 1-2 pixels; stall renderer if needed |
| FPGA resource utilization too high | Low | Medium | Tang Nano 20K GW2A has 20K LUTs; current design uses ~60% |

## Files Summary

### New files
- `boards/a2n20v2-GS/hdl/video/sdram_framebuffer.sv` — Core framebuffer module
- `boards/a2n20v2-GS/hdl/video/scan_timer.sv` — Copy from a2mega
- `boards/a2n20v2-GS/hdl/video/apple_video_fb.sv` — Copy from a2mega
- `boards/a2n20v2-GS/hdl/video/vgc_fb.sv` — Copy from a2mega

### Modified files
- `boards/a2n20v2-GS/hdl/top.sv` — Major restructure for SDRAM + framebuffer
- `boards/a2n20v2-GS/a2n20v2_gs.gprj` — Add source files
- `boards/a2n20v2-GS/hdl/a2n20v2_gs.sdc` — Timing constraints for CDC paths

### Reused shared files (already in repo)
- `hdl/sdram/sdram.sv` — SDRAM controller
- `hdl/sdram/sdram_ports.sv` — Multi-port wrapper
- `hdl/memory/mem_port_if.sv` — Port interface definition
