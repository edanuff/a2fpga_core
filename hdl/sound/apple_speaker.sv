module apple_speaker (
    a2bus_if.slave a2bus_if,
    input enable,                        // Global enable for speaker audio
    output reg speaker_o,               // Optional logic-level speaker out (still useful)
    output logic signed [13:0] pcm_o    // Signed 14-bit PCM output (centered, gated)
);

    // Speaker toggle bit: changes on writes to $C030
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

    localparam COUNTDOWN_WIDTH = 20;
    reg [COUNTDOWN_WIDTH-1:0] countdown;
    reg prev_speaker_bit;

    always_ff @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            countdown <= '0;
            prev_speaker_bit <= 0;
            speaker_o <= 0;
            pcm_o <= 14'sd0;
        end else begin
            // Detect change in speaker toggle
            if (speaker_bit != prev_speaker_bit) begin
                countdown <= '1;  // Reset countdown to max
            end else if (countdown != 0) begin
                countdown <= COUNTDOWN_WIDTH'(countdown - 1);
            end

            prev_speaker_bit <= speaker_bit;
            speaker_o <= speaker_bit;

            // Gate the PCM output based on countdown and global enable
            if (enable && (countdown != 0)) begin
                pcm_o <= speaker_bit ? 14'sd8191 : -14'sd8192;
            end else begin
                pcm_o <= 14'sd0;
            end
        end
    end

endmodule