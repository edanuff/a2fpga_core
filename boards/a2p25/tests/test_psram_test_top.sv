// test_psram_test_top.sv — system TB for the a2p25 standalone HyperRAM
// bitstream: a2p25_psram_test_top (CK 108 variant) against the Infineon
// S27KL0642 model through test_hyperram_harness, driven exclusively over
// the console UART exactly as the ESP32 passthrough would.
//
// Checks: autostart reaches running passes with zero errors, the periodic
// status line appears, R/W/local registers answer correctly, STOP works.
`timescale 1ns / 1ps

module test_psram_test_top;
    GSR GSR(.GSRI(1'b1));

    localparam int BAUD = 2_000_000;           // sim speed; hardware = 115200
    localparam real BIT_NS = 1.0e9 / BAUD;

    logic clk50 = 1'b0;
    always #10 clk50 = ~clk50;

    wire        hr_ck, hr_cs_n, hr_rwds;
    wire [7:0]  hr_dq;
    wire [7:0]  f_dq_drv;
    wire        f_rwds_drv;
    wire [7:0]  h_dq_in;
    wire        h_rwds_in;
    integer     timing_errors, contention_errors;
    logic       uart_to_fpga = 1'b1;
    wire        uart_from_fpga, led, oe_n;

    test_hyperram_harness u_h (
        .clk               (),
        .clk_mem           (),
        .clk_mem_90        (),
        .hr_ck             (hr_ck),
        .hr_cs_n           (hr_cs_n),
        .f_dq_drv          (f_dq_drv),
        .f_rwds_drv        (f_rwds_drv),
        .hr_dq_in          (h_dq_in),
        .hr_rwds_in        (h_rwds_in),
        .inj_xor           (8'h00),
        .timing_errors     (timing_errors),
        .contention_errors (contention_errors)
    );
    assign hr_dq   = h_dq_in;
    assign hr_rwds = h_rwds_in;
    genvar gi;
    generate
        for (gi = 0; gi < 8; gi = gi + 1) begin : g_drv
            assign f_dq_drv[gi] = dut.i_ctrl.u_phy.dq_tri[gi] ? 1'bz : dut.i_ctrl.u_phy.dq_out[gi];
        end
    endgenerate
    assign f_rwds_drv = dut.i_ctrl.u_phy.rwds_tri ? 1'bz : dut.i_ctrl.u_phy.rwds_out;

    a2p25_psram_test_top #(
        .AUTO_LEN    (2048),
        .UART_BAUD   (BAUD),
        .STATUS_CLKS (54_000 * 2)        // every 2 ms
    ) dut (
        .clk50_in    (clk50),
        .hr_ck       (hr_ck),
        .hr_cs_n     (hr_cs_n),
        .hr_rwds     (hr_rwds),
        .hr_dq       (hr_dq),
        .uart_rx     (uart_to_fpga),
        .uart_tx     (uart_from_fpga),
        .led         (led),
        .a2_bus_oe_n (oe_n)
    );

    // ---------------- UART host side ------------------------------------
    task automatic send_byte(input [7:0] b);
        integer i;
        begin
            uart_to_fpga = 1'b0; #(BIT_NS);
            for (i = 0; i < 8; i = i + 1) begin uart_to_fpga = b[i]; #(BIT_NS); end
            uart_to_fpga = 1'b1; #(BIT_NS);
        end
    endtask

    task automatic send_line(input string s);
        integer i;
        begin
            for (i = 0; i < s.len(); i = i + 1) send_byte(s[i]);
            send_byte(8'h0D);
        end
    endtask

    string rx_line = "";
    string last_line = "";
    integer lines = 0, status_lines = 0;
    initial begin : rx
        reg [7:0] b;
        integer i;
        forever begin
            @(negedge uart_from_fpga);
            #(BIT_NS * 1.5);
            for (i = 0; i < 8; i = i + 1) begin b[i] = uart_from_fpga; #(BIT_NS); end
            if (b == 8'h0A) begin
                last_line = rx_line;
                lines = lines + 1;
                if (rx_line.len() > 0 && rx_line[0] == "S") status_lines = status_lines + 1;
                $display("[%0t ns] FPGA> %s", $time, rx_line);
                rx_line = "";
            end else if (b != 8'h0D) begin
                rx_line = {rx_line, string'(b)};
            end
        end
    end

    // send a command and wait for its (non-status) reply line
    task automatic cmd(input string c, output string reply);
        integer n0;
        begin
            n0 = lines;
            send_line(c);
            wait (lines > n0 && last_line[0] != "S");
            reply = last_line;
        end
    endtask

    function automatic [31:0] hexval32(input string s, input integer from);
        integer i;
        reg [31:0] v;
        begin
            v = 0;
            for (i = from; i < from + 8; i = i + 1)
                v = {v[27:0], (s[i] >= "A") ? 4'(s[i] - "A" + 10) : 4'(s[i] - "0")};
            hexval32 = v;
        end
    endfunction

    integer errors = 0;
    string r;
    reg [31:0] v;
    initial begin
        wait (dut.m_init_done);
        $display("[%0t ns] init_done cal_ok=%b cal_map=%h tap=%0d", $time,
                 dut.m_cal_ok, dut.m_cal_map, dut.m_dly_code);
        if (!dut.m_cal_ok) begin $display("FAIL: calibration"); errors++; end

        // let autostart run and the periodic status line appear
        wait (status_lines >= 2);
        cmd("W F2 0", r);          // quiet the status line
        if (r != "OK") begin $display("FAIL: W F2 -> %s", r); errors++; end
        cmd("R 00", r);
        if (r != "00=48524231") begin $display("FAIL: ID -> %s", r); errors++; end
        cmd("w f3 deadbeef", r);
        cmd("r f3", r);
        if (r != "F3=DEADBEEF") begin $display("FAIL: scratch -> %s", r); errors++; end
        cmd("R F0", r);
        if (r != "F0=5053006C") begin $display("FAIL: build -> %s", r); errors++; end
        cmd("bogus", r);
        if (r != "?") begin $display("FAIL: bad cmd -> %s", r); errors++; end

        // wait for >= 3 completed passes (PASS_COUNT 0x1B)
        v = 0;
        while (v < 3) begin
            #200_000;
            cmd("R 1B", r);
            v = hexval32(r, 3);
        end
        cmd("R 11", r);
        $display("words read: %s", r);
        if (hexval32(r, 3) < 3 * 2048) begin $display("FAIL: too few words read"); errors++; end
        cmd("R 12", r);
        if (hexval32(r, 3) != 0) begin $display("FAIL: error words %s", r); errors++; end
        cmd("R 19", r);
        $display("BIST 2x-latency transactions: %s", r);

        cmd("W 01 2", r);          // STOP
        #50_000;
        cmd("R 02", r);
        v = hexval32(r, 3);
        if (v[0]) begin $display("FAIL: still busy after STOP (%s)", r); errors++; end

        if (timing_errors != 0 || contention_errors != 0) begin
            $display("FAIL: pin timing %0d, contention %0d", timing_errors, contention_errors);
            errors++;
        end
        if (errors == 0) $display("PSRAM_TEST_TOP PASS (status lines %0d)", status_lines);
        else             $display("PSRAM_TEST_TOP FAIL (%0d)", errors);
        $finish;
    end

    initial begin
        #60_000_000;
        $display("PSRAM_TEST_TOP FAIL: timeout");
        $finish;
    end
endmodule
