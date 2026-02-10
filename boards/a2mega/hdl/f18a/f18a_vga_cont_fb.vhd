--
-- F18A VGA Controller — Framebuffer variant for A2Mega (DDR3)
--
-- Based on f18a_vga_cont_640_60.vhd by Matthew Hagerty (3-Clause BSD License)
-- Modified for DDR3 framebuffer pipeline: 560x192 visible area at 1:1 pixel scale,
-- driven by external raster counter synced to Apple II scan_timer.
--
-- Both vga_clk and clk_logic are the same clock (54 MHz) in this variant.
-- Internal counters remain commented out — raster_x/raster_y are external inputs.
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity f18a_vga_cont_640_60 is
   port(
      vga_clk  : in std_logic;
      rst_n    : in std_logic;
      hsync    : out std_logic;
      vsync    : out std_logic;
      raster_x : in unsigned(0 to 9);
      raster_y : in unsigned(0 to 9);
      y_tick   : out std_logic;
      y_max    : out std_logic;
      blank    : out std_logic
   );
end f18a_vga_cont_640_60;

architecture rtl of f18a_vga_cont_640_60 is

   -- Framebuffer timing constants for 560x192 at 1:1 pixel scale.
   -- External raster counter runs 0–1023 per scanline (clamped at HMAX),
   -- 0–261 per frame (262 Apple II scanlines).
   constant HMAX  : integer := 1023;   -- Max horizontal counter (10-bit)
   constant VMAX  : integer := 261;    -- Apple II total scanlines

   -- Visible area matches apple_video_fb output.
   constant HSIZE : integer := 560;    -- Visible pixels per line
   constant VSIZE : integer := 192;    -- Visible lines

   -- Sync pulse placement in blanking region (not used by HDMI output,
   -- but needed for F18A internal logic that checks hsync/vsync).
   constant HFP   : integer := 600;    -- Horz front porch start
   constant HSP   : integer := 700;    -- Horz sync pulse end
   constant VFP   : integer := 193;    -- Vert front porch start
   constant VSP   : integer := 196;    -- Vert sync pulse end

   -- Polarity of sync pulses
   constant SPP   : std_logic := '0';

   -- Horizontal and vertical counters (driven externally)
   signal hcounter : unsigned(0 to 9);
   signal vcounter : unsigned(0 to 9);

   signal blank_reg : std_logic;

begin

   -- External raster input
   hcounter <= raster_x;
   vcounter <= raster_y;

   -- Generate horizontal sync pulse
   do_hs: process (vga_clk)
   begin
      if rising_edge(vga_clk) then
         if hcounter >= HFP and hcounter < HSP then
            hsync <= SPP;
         else
            hsync <= not SPP;
         end if;
      end if;
   end process do_hs;

   -- Generate vertical sync pulse
   do_vs: process (vga_clk)
   begin
      if rising_edge(vga_clk) then
         if vcounter >= VFP and vcounter < VSP then
            vsync <= SPP;
         else
            vsync <= not SPP;
         end if;
      end if;
   end process do_vs;

   -- y_tick fires when hcounter reaches HMAX (once per scanline, briefly)
   y_tick <= '1' when hcounter = HMAX else '0';

   -- y_max signals end of visible area
   y_max <= '1' when vcounter = VSIZE else '0';

   -- Blank is active when the raster is outside visible screen area.
   process (vga_clk) begin if rising_edge(vga_clk) then
      if (hcounter < HSIZE and vcounter < VSIZE) then
         blank_reg <= '0';
      else
         blank_reg <= '1';
      end if;
   end if; end process;
   blank <= blank_reg;

end rtl;
