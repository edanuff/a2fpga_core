
module audio_out
#(
	parameter CLK_RATE = 24576000,
	parameter AUDIO_RATE = 48000
)
(
	input        reset,
	input        clk,

	input  [31:0] flt_rate,
	input  [39:0] cx,
	input   [7:0] cx0,
	input   [7:0] cx1,
	input   [7:0] cx2,
	input  [23:0] cy0,
	input  [23:0] cy1,
	input  [23:0] cy2,

	input        is_signed,
	input [15:0] core_l,
	input [15:0] core_r,

	output audio_clk,
	output [15:0] audio_l,
	output [15:0] audio_r,

	output i2s_bclk,
	output i2s_lrclk,
	input i2s_data,
	output [15:0] i2s_sample_word[1:0]
);

	wire sample_ce;
	wire i2s_data_shift_strobe;
	wire i2s_data_load_strobe;

	audio_timing #(
        .CLK_RATE(CLK_RATE),
        .AUDIO_RATE(AUDIO_RATE)
    ) audio_timing (
        .reset(reset),
        .clk(clk),
        .audio_clk(sample_ce),
        .i2s_bclk(i2s_bclk),
        .i2s_lrclk(i2s_lrclk),
        .i2s_data_shift_strobe(i2s_data_shift_strobe),
        .i2s_data_load_strobe(i2s_data_load_strobe)
    );
	assign audio_clk = sample_ce;

	reg [15:0] data_shift  = 16'h0000;
    reg [15:0] i2s_word_l = 16'h0000;
    reg [15:0] i2s_word_r = 16'h0000;

    always @(posedge clk) begin
        if (i2s_data_shift_strobe) begin
            data_shift <= {data_shift[14:0], i2s_data};
        end
    end

    always @(posedge clk) begin
        if (i2s_data_load_strobe) begin
			// lrclk has already toggled at this point, so use opposite of i2s_lrclk
			if (i2s_lrclk) i2s_word_l <= data_shift;
			else i2s_word_r <= data_shift;
        end
    end

	assign i2s_sample_word[0] = i2s_word_l;
	assign i2s_sample_word[1] = i2s_word_r;

	reg flt_ce;
	reg [31:0] cnt = 0;
	always @(posedge clk, posedge reset) begin
		if(reset) begin
			cnt <= 0;
			flt_ce <= 0;
		end
		else begin
			flt_ce <= 0;
			cnt <= cnt + {flt_rate[30:0],1'b0};
			if(cnt >= CLK_RATE) begin
				cnt <= cnt - CLK_RATE;
				flt_ce <= 1;
			end
		end
	end

	reg [15:0] cl,cr;
	reg [15:0] cl1,cl2;
	reg [15:0] cr1,cr2;
	always @(posedge clk) begin
		cl1 <= core_l; cl2 <= cl1;
		if(cl2 == cl1) cl <= cl2;

		cr1 <= core_r; cr2 <= cr1;
		if(cr2 == cr1) cr <= cr2;
	end

	reg a_en1 = 0, a_en2 = 0;
	reg  [1:0] dly1 = 0;
	reg [14:0] dly2 = 0;
	always @(posedge clk, posedge reset) begin

		if(reset) begin
			dly1 <= 0;
			dly2 <= 0;
			a_en1 <= 0;
			a_en2 <= 0;
		end
		else begin
			if(flt_ce) begin
				if(~&dly1) dly1 <= dly1 + 1'd1;
				else a_en1 <= 1;
			end

			if(sample_ce) begin
				if(!dly2[13]) dly2 <= dly2 + 1'd1;
				else a_en2 <= 1;
			end
		end
	end

	wire [15:0] acl, acr;
	IIR_filter #(.use_params(0)) IIR_filter
	(
		.clk(clk),
		.reset(reset),

		.ce(flt_ce & a_en1),
		.sample_ce(sample_ce),

		.cx(cx),
		.cx0(cx0),
		.cx1(cx1),
		.cx2(cx2),
		.cy0(cy0),
		.cy1(cy1),
		.cy2(cy2),

		.input_l({~is_signed ^ cl[15], cl[14:0]}),
		.input_r({~is_signed ^ cr[15], cr[14:0]}),
		.output_l(acl),
		.output_r(acr)
	);

	wire [15:0] adl;
	DC_blocker dcb_l
	(
		.clk(clk),
		.ce(sample_ce),
		.mute(~a_en2),
		.din(acl),
		.dout(adl)
	);

	wire [15:0] adr;
	DC_blocker dcb_r
	(
		.clk(clk),
		.ce(sample_ce),
		.mute(~a_en2),
		.din(acr),
		.dout(adr)
	);

	assign audio_l = adl;
	assign audio_r = adr;


endmodule
