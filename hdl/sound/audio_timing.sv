
module audio_timing
#(
	parameter CLK_RATE = 24576000,
	parameter AUDIO_RATE = 48000
)
(
	input        reset,
	input        clk,

	output audio_clk,

	output i2s_bclk,
	output i2s_lrclk,
	output i2s_data_shift_strobe,
	output i2s_data_load_strobe
);
	// Must be as accurate as possible or HDMI audio will glitch
    localparam AUDIO_CLK_COUNT = CLK_RATE / AUDIO_RATE;
    logic [$clog2(AUDIO_CLK_COUNT)-1:0] audio_counter_r;

	localparam I2S_BCLK_COUNT = AUDIO_CLK_COUNT / 64;
    logic [$clog2(I2S_BCLK_COUNT)-1:0] i2s_bclk_counter_r;
	localparam I2S_ENABLE_WINDOW = I2S_BCLK_COUNT * 64;
	// Depending on clock division, an I2S sample period may complete before
	// the AUDIO_RATE sample period
	// Disable the I2S clock when full I2S sample period is reached
	wire i2s_enable = (AUDIO_CLK_COUNT > I2S_ENABLE_WINDOW) ? (audio_counter_r < I2S_ENABLE_WINDOW) : 1;

	localparam I2S_LRCLK_COUNT = I2S_BCLK_COUNT * 32; // 16 bits per channel * 2 clock toggles of bclk
    logic [$clog2(I2S_LRCLK_COUNT)-1:0] i2s_lrclk_counter_r;

	reg sample_ce;
	reg i2s_clk_r;
	reg prev_i2s_clk_r;
	reg i2s_lrclk_r;
	reg load_strobe_r;

	always_ff @(posedge clk, posedge reset)
    begin
		if (reset) begin
			i2s_bclk_counter_r <= 0;
			i2s_lrclk_counter_r <= 0;
			audio_counter_r <= 0;
			sample_ce <= 0;
			i2s_clk_r <= 0;
			prev_i2s_clk_r <= 0;
			i2s_lrclk_r <= 0;
			load_strobe_r <= 0;
		end
		else begin
			i2s_bclk_counter_r <= (i2s_bclk_counter_r == I2S_BCLK_COUNT-1) ? 1'd0 : i2s_bclk_counter_r + 1'd1;
			i2s_lrclk_counter_r <= (i2s_lrclk_counter_r == I2S_LRCLK_COUNT-1) ? 1'd0 : i2s_lrclk_counter_r + 1'd1;

			if (audio_counter_r == AUDIO_CLK_COUNT-1) begin
				audio_counter_r <= 0;
				i2s_bclk_counter_r <= 0;
				i2s_lrclk_counter_r <= 0;
			end else begin
				audio_counter_r <= audio_counter_r + 1'd1;
			end
			sample_ce <= audio_counter_r == AUDIO_CLK_COUNT-1;
			
			// Toggle i2s_clk and i2s_lrclk at the end of their respective counters
			i2s_clk_r <= i2s_bclk_counter_r == I2S_BCLK_COUNT-1 ? ~i2s_clk_r : i2s_clk_r;
			i2s_lrclk_r <= i2s_lrclk_counter_r == I2S_LRCLK_COUNT-1 ? ~i2s_lrclk_r : i2s_lrclk_r;
			if (!i2s_enable) begin
				i2s_clk_r <= 0;
				i2s_lrclk_r <= 0;
			end
			load_strobe_r <= i2s_enable & (i2s_lrclk_counter_r == (I2S_BCLK_COUNT * 2)) ? 1 : 0;
			prev_i2s_clk_r <= i2s_clk_r;
		end
    end
	assign audio_clk = sample_ce;
	assign i2s_bclk = i2s_clk_r;
	assign i2s_lrclk = i2s_lrclk_r;
	assign i2s_data_shift_strobe = !i2s_clk_r && prev_i2s_clk_r;
	assign i2s_data_load_strobe = load_strobe_r;

endmodule
