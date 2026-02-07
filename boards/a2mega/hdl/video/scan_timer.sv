module scan_timer (
    a2bus_if.slave a2bus_if,
	output [8:0] scanline_o,
    output hsync_o,
    output vsync_o,
    output [9:0] pixel_o
);
    reg [8:0] scanline_counter_r;
    reg vsync_r;
    reg hsync_r;
    reg [9:0] pixel_counter_r;

    always @(posedge a2bus_if.clk_logic or negedge a2bus_if.system_reset_n) begin
        if (!a2bus_if.system_reset_n) begin
            scanline_counter_r <= 9'd0;
            hsync_r <= 1'b0;
            vsync_r <= 1'b0;
            pixel_counter_r <= 10'd0;
        end else begin
            vsync_r <= 1'b0;
            hsync_r <= 1'b0;

            if (pixel_counter_r != 10'b1111111111) begin
                pixel_counter_r <= pixel_counter_r + 1'b1;  
            end

            if (a2bus_if.extended_cycle) begin
                hsync_r <= 1'b1;
                pixel_counter_r <= 10'd0;
                if (scanline_counter_r == 9'd261) begin
                    scanline_counter_r <= 9'd0;
                    vsync_r <= 1'b1;
                end else begin
                    scanline_counter_r <= scanline_counter_r + 1'b1;
                end
            end
        end
    end
    
    assign scanline_o = scanline_counter_r;
    assign hsync_o = hsync_r;
    assign vsync_o = vsync_r;
    assign pixel_o = pixel_counter_r;
	
endmodule
