module apple_speaker (
    a2bus_if.slave a2bus_if,
    input enable,
    output reg speaker_o,
    output logic signed [13:0] pcm_o
);

    reg speaker_bit;

    always @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            speaker_bit <= 1'b0;
        end else if (a2bus_if.phi1_posedge &&
                     (a2bus_if.addr[15:0] == 16'hC030) &&
                     !a2bus_if.m2sel_n) begin
            speaker_bit <= !speaker_bit;
        end
    end

    localparam signed [13:0] POS_LEVEL = 14'sd8191;
    localparam signed [13:0] NEG_LEVEL = -14'sd8192;
    localparam signed [13:0] FADE_STEP = 14'sd64;

    // Clock divider to reduce fade frequency
    localparam integer FADE_DIV_BITS = 10;  // 2^10 = 1024
    reg [FADE_DIV_BITS-1:0] fade_clk_div;

    wire fade_tick = (fade_clk_div == 0);

    reg prev_speaker_bit;

    always_ff @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            pcm_o <= 0;
            prev_speaker_bit <= 0;
            speaker_o <= 0;
            fade_clk_div <= 0;
        end else begin
            fade_clk_div <= fade_clk_div + 1;

            if (speaker_bit != prev_speaker_bit) begin
                pcm_o <= speaker_bit ? POS_LEVEL : NEG_LEVEL;
            end else if (fade_tick && enable) begin
                if (pcm_o > FADE_STEP)
                    pcm_o <= pcm_o - FADE_STEP;
                else if (pcm_o < -FADE_STEP)
                    pcm_o <= pcm_o + FADE_STEP;
                else
                    pcm_o <= 14'sd0;
            end else if (!enable) begin
                pcm_o <= 14'sd0;
            end

            prev_speaker_bit <= speaker_bit;
            speaker_o <= speaker_bit;
        end
    end

endmodule