module srff (
    input wire clk,
    input wire s,
    input wire r,
    output wire q
);

    reg prev_s_r;
    reg prev_r_r;
    reg q_r;
    
    // Edge detection signals
    wire edge_s_w = s & ~prev_s_r;
    wire edge_r_w = r & ~prev_r_r;
    
    // Purely sequential output
    assign q = q_r;

    always @(posedge clk) begin
        // Update edge detection registers
        prev_s_r <= s;
        prev_r_r <= r;
        
        // Update output register based on edges
        if (edge_r_w)
            q_r <= 1'b0;
        else if (edge_s_w)
            q_r <= 1'b1;
    end

endmodule