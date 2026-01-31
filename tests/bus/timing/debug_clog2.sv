`timescale 1ns / 1ps
module debug_clog2;
    localparam int unsigned CLOCK_SPEED_HZ = 54_000_000;
    localparam int unsigned TICK_RATE = 14_318_181;
    localparam int unsigned ACC_WIDTH = $clog2(CLOCK_SPEED_HZ);
    
    initial begin
        $display("CLOCK_SPEED_HZ = %0d", CLOCK_SPEED_HZ);
        $display("TICK_RATE = %0d", TICK_RATE);
        $display("ACC_WIDTH = %0d", ACC_WIDTH);
        $display("ACC_WIDTH+1 bits max = %0d", (1 << (ACC_WIDTH+1)) - 1);
        $display("Ratio = %f", real'(CLOCK_SPEED_HZ) / real'(TICK_RATE));
        
        // Simulate a few accumulator iterations
        begin
            reg [ACC_WIDTH:0] acc;
            integer tick_count;
            integer cycle_count;
            acc = 0;
            tick_count = 0;
            cycle_count = 0;
            
            for (cycle_count = 0; cycle_count < 200; cycle_count = cycle_count + 1) begin
                acc = acc + TICK_RATE;
                if (acc >= CLOCK_SPEED_HZ) begin
                    acc = acc - CLOCK_SPEED_HZ;
                    tick_count = tick_count + 1;
                end
            end
            $display("After %0d clock cycles: %0d ticks (expected ~%f)", 
                cycle_count, tick_count, 200.0 * TICK_RATE / CLOCK_SPEED_HZ);
        end
        
        $finish;
    end
endmodule
