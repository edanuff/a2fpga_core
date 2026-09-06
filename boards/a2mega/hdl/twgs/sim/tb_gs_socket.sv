// Sim 1 — in-socket 65C816: gs_socket_phy + P65C816 against a behavioural
// IIgs FPI bus.  Verilator --timing bench (see run_gs_socket_sim.sh).
//
// What is modelled at the SOCKET side (after the 1.0a3 level shifters):
//  * FPI PHI2: 2 ticks low / rest high at 14.318 MHz; fast cycle = 5 ticks,
//    every 9th RAM cycle = 10-tick refresh cycle (ROM hides refresh), 65th
//    PH0 of a line stretches the next fast cycle by 2 ticks; slow-class
//    accesses (bank E0/E1, $C0xx in bank 0/1) become sync cycles: PHI2 held
//    high until a PH0 boundary with >= 14 ticks total; Speed register
//    $C036 bit 7 = 0 runs every cycle on the PH0 grid (1.023 MHz).  The
//    machine comes out of reset in slow mode, like the real one.
//  * Memory: 16 MB, latches bank at the PHI2 rise (74x245-latch style),
//    drives read data T_ACC after the rise, holds it T_DHR after the fall,
//    then drives GARBAGE for a while (so a late sampler is caught), then
//    floats.  Writes are latched at the fall.
//  * Transceivers: T_XCVR data delay, T_XEN enable delay, both directions.
//  * ROM mode (+rom=<boot1.rom>): the ROM 01 image in banks FE/FF behind the
//    FPI's reset-state map (banks 00/01/E0/E1 $C100-$FFFF read the bank-FF
//    image, $C0xx is a write-back register file), and a REFERENCE core
//    instance fed by an ideal memory path, compared with the socket-side
//    core every cycle (address, RWB, VPB, write data, read data).  Any byte
//    the PHY gets wrong shows up as a divergence within one cycle.  The
//    reference is active in program mode too.
//  * Checks (4 MHz-grade W65C816S limits): tAH, tADS, tBAS, tBH, tMDS,
//    tDHW, no bus contention, bank/address/RWB latched by the FPI equal the
//    core's, read data delivered to the core equals what memory drove,
//    RDY-low cycles repeat with no CE, BE-low tri-states within tBVD,
//    VP low on vector fetches; and the program's results in memory.
//
`timescale 1ns/1ps

module tb_gs_socket;

    //---------------------------------------------------------------- params
    localparam real T14    = 69.841;   // 14.31818 MHz
    localparam real TCLK   = 9.259;    // 108 MHz
    localparam real T_XCVR = 5.0;      // 74ALVC164245 / LV1T125 tpd
    localparam real T_XEN  = 6.0;      // enable / disable
    // W65C816S, 4 MHz grade (datasheet table 4-2)
    localparam real T_ADS = 75.0, T_BAS = 75.0, T_AH = 20.0, T_BH = 20.0;
    localparam real T_MDS = 70.0, T_DHW = 20.0, T_DHR = 20.0, T_BVD = 60.0;
    // memory behaviour
    localparam real T_ACC   = 100.0;   // read data driven this long after the rise
    localparam real T_GARB  = 35.0;    // garbage driven until fall + T_GARB
    localparam int  IO_READ = 8'hA0;   // what $C0xx reads return

    //---------------------------------------------------------------- clocks
    logic clk = 1'b0;
    always #(TCLK/2.0) clk = ~clk;
    logic rst_n = 1'b0;

    logic tick = 1'b0;
    always #(T14/2.0) tick = ~tick;

    //---------------------------------------------------------------- DUT pins
    logic        gs_ph2_f, gs_rdy_f, gs_irq_n_f, gs_nmi_n_f, gs_res_n_f, gs_abort_n_f, gs_be_f;
    logic [7:0]  gs_d_i_f, gs_d_o_f;
    logic        gs_d_oe_f;
    logic [15:0] gs_a_f;
    logic        gs_rw_f, gs_vp_f, gs_rdy_out_f, gs_d_dir_f, gs_data_oe_n_f, gs_addr_oe_n_f, gs_ctl_oe_n_f;
    logic        armed = 1'b0;
    logic        ph2_alive, running;
    logic [31:0] cycle_count;
    logic [15:0] stall_count, be_count;

    logic        cpu_clk, cpu_rst_n, cpu_we_n, cpu_vp_n, cpu_rdy_out, cpu_vpa, cpu_vda, cpu_mlb, cpu_iflag;
    logic [7:0]  cpu_d_out;
    logic [23:0] cpu_a;

    gs_socket_phy u_phy (
        .clk(clk), .rst_n(rst_n), .armed_i(armed), .out_extra_i(4'd0),
        .cpu_clk_o(cpu_clk), .cpu_rst_n_o(cpu_rst_n),
        .cpu_a_i(cpu_a), .cpu_d_out_i(cpu_d_out), .cpu_we_n_i(cpu_we_n), .cpu_vp_n_i(cpu_vp_n),
        .gs_ph2_i(gs_ph2_f), .gs_rdy_i(gs_rdy_f), .gs_res_n_i(gs_res_n_f), .gs_be_i(gs_be_f),
        .gs_d_o(gs_d_o_f), .gs_d_oe_o(gs_d_oe_f),
        .gs_a_o(gs_a_f), .gs_rw_o(gs_rw_f), .gs_vp_o(gs_vp_f), .gs_rdy_out_o(gs_rdy_out_f),
        .gs_d_dir_o(gs_d_dir_f), .gs_data_oe_n_o(gs_data_oe_n_f), .gs_addr_oe_n_o(gs_addr_oe_n_f),
        .gs_ctl_oe_n_o(gs_ctl_oe_n_f),
        .ph2_alive_o(ph2_alive), .running_o(running), .enabled_o(), .be_ok_o(),
        .fall_evt_o(), .issue_evt_o(), .ended_read_o(),
        .cycle_count_o(cycle_count), .stall_count_o(stall_count), .be_count_o(be_count)
    );

    // The core is clocked by PHI2 (inverted: its active edge is the fall) and
    // samples the pads directly, as the real part does.
    P65C816 u_cpu (
        .CLK(cpu_clk), .RST_N(cpu_rst_n), .CE(1'b1), .RDY_IN(gs_rdy_f),
        .NMI_N(gs_nmi_n_f), .IRQ_N(gs_irq_n_f), .ABORT_N(gs_abort_n_f),
        .D_IN(gs_d_i_f), .D_OUT(cpu_d_out), .A_OUT(cpu_a), .WE(cpu_we_n), .RDY_OUT(cpu_rdy_out),
        .VPA(cpu_vpa), .VDA(cpu_vda), .MLB(cpu_mlb), .VPB(cpu_vp_n), .I_FLAG(cpu_iflag)
    );

    //---------------------------------------------------------------- reference core
    // Same core, same clock/reset/RDY/interrupts, but fed from the reference
    // memory copy directly.  The socket-side core must never differ from it.
    logic [7:0]  ref_din, ref_dout;
    logic [23:0] ref_a;
    logic        ref_we_n, ref_vp_n, ref_rdy_out, ref_vpa, ref_vda, ref_mlb, ref_iflag;

    P65C816 u_ref (
        .CLK(cpu_clk), .RST_N(cpu_rst_n), .CE(1'b1), .RDY_IN(gs_rdy_f),
        .NMI_N(gs_nmi_n_f), .IRQ_N(gs_irq_n_f), .ABORT_N(gs_abort_n_f),
        .D_IN(ref_din), .D_OUT(ref_dout), .A_OUT(ref_a), .WE(ref_we_n), .RDY_OUT(ref_rdy_out),
        .VPA(ref_vpa), .VDA(ref_vda), .MLB(ref_mlb), .VPB(ref_vp_n), .I_FLAG(ref_iflag)
    );
    always_comb ref_din = model_read(1, ref_a);

    //---------------------------------------------------------------- socket nets
    // Motherboard-driven inputs (socket side)
    logic ph2_bus = 1'b0;
    logic rdy_bus = 1'b1, irq_n_bus = 1'b1, nmi_n_bus = 1'b1, res_n_bus = 1'b0, abort_n_bus = 1'b1, be_bus = 1'b1;

    // Card -> socket through the shifters
    logic        addr_en;       // U14/U16 enabled
    logic [15:0] a_bus;
    logic        rw_bus, vp_bus;
    logic        cpu_drv;       // U13 port 2 driving the socket data bus
    logic [7:0]  cpu_val;
    logic        rx_en;         // U13 port 2 in receive mode
    assign #(T_XEN)  addr_en = ~gs_addr_oe_n_f;
    assign #(T_XCVR) a_bus   = gs_a_f;
    assign #(T_XCVR) rw_bus  = gs_rw_f;
    assign #(T_XCVR) vp_bus  = gs_vp_f;
    assign #(T_XEN)  cpu_drv = ~gs_data_oe_n_f & gs_d_dir_f & gs_d_oe_f;
    assign #(T_XCVR) cpu_val = gs_d_o_f;
    assign #(T_XEN)  rx_en   = ~gs_data_oe_n_f & ~gs_d_dir_f;

    // Memory -> socket data bus
    logic       mem_drv = 1'b0;
    logic [7:0] mem_val = 8'h00;

    logic [7:0] d_bus;
    logic       d_float;
    always_comb begin
        d_float = ~(cpu_drv | mem_drv);
        d_bus   = cpu_drv ? cpu_val : (mem_drv ? mem_val : 8'hFF);
    end

    // Socket -> card through the shifters
    assign #(T_XCVR) gs_ph2_f     = ph2_bus;
    assign #(T_XCVR) gs_rdy_f     = rdy_bus;
    assign #(T_XCVR) gs_irq_n_f   = irq_n_bus;
    assign #(T_XCVR) gs_nmi_n_f   = nmi_n_bus;
    assign #(T_XCVR) gs_res_n_f   = res_n_bus;
    assign #(T_XCVR) gs_abort_n_f = abort_n_bus;
    assign #(T_XCVR) gs_be_f      = be_bus;
    // FPGA D pad input: the bus when receiving, own output when driving, junk otherwise
    assign #(T_XCVR) gs_d_i_f = rx_en ? d_bus : (gs_d_oe_f ? gs_d_o_f : 8'h5A);

    //---------------------------------------------------------------- memory
    bit         noturbo = 0;            // +noturbo: ignore $C036 (1 MHz throughout)
    int         text_writes = 0, io_writes = 0;
    logic [7:0] mem   [0:(1<<24)-1];    // socket-side (DUT) copy
    logic [7:0] mem_r [0:(1<<24)-1];    // reference copy
    logic [7:0] rom   [0:(1<<17)-1];    // ROM 01 image: bank FE then FF
    logic [7:0] io_regs   [0:255];      // $C0xx write-back register file (DUT)
    logic [7:0] io_regs_r [0:255];      // ... reference copy
    bit         rom_loaded = 0;
    logic       slow_mode = 1'b1;       // $C036 bit 7 = 0 after reset

    // $C000-$C0FF in banks 00/01/E0/E1
    function automatic bit is_io(input logic [23:0] a);
        return ((a[23:17] == 7'b0000000) || (a[23:17] == 7'b1110000)) && (a[15:8] == 8'hC0);
    endfunction
    // ROM image offset for a read, or -1.  FPI reset state: banks FE/FF are the
    // image; banks 00/01/E0/E1 $C100-$FFFF read the bank-FF half (internal
    // slot ROM, language card in ROM-read state).
    function automatic int rom_ofs(input logic [23:0] a);
        if (!rom_loaded) return -1;
        if (a[23:17] == 7'b1111111) return int'(a[16:0]);
        if (((a[23:17] == 7'b0000000) || (a[23:17] == 7'b1110000)) && (a[15:8] >= 8'hC1)) return int'({1'b1, a[15:0]});
        return -1;
    endfunction
    function automatic logic is_rom(input logic [23:0] a);
        if (rom_loaded) return rom_ofs(a) >= 0;
        return (a[23:16] == 8'h00) && (a[15:12] >= 4'hD);
    endfunction
    function automatic logic [7:0] model_read(input bit r, input logic [23:0] a);
        int ofs;
        if (is_io(a)) begin
            logic [7:0] v;
            if (!rom_loaded) return (a == 24'h00C030) ? IO_READ[7:0] : 8'h00;
            v = r ? io_regs_r[a[7:0]] : io_regs[a[7:0]];
            // minimal hardware handshakes so the ROM's init code progresses:
            case (a[7:0])
                8'h34: v[7] = 1'b0;        // clock chip: transaction always complete
                default: ;
            endcase
            return v;
        end
        ofs = rom_ofs(a);
        if (ofs >= 0) return rom[ofs];
        return r ? mem_r[a] : mem[a];
    endfunction
    task automatic model_write(input bit r, input logic [23:0] a, input logic [7:0] v);
        if (is_io(a)) begin
            if (a[15:0] == 16'hC036 && !r && !noturbo) slow_mode = ~v[7];
            if (rom_loaded) begin
                if (r) io_regs_r[a[7:0]] = v; else io_regs[a[7:0]] = v;
            end
        end else begin
            if (r) mem_r[a] = v; else mem[a] = v;     // LC RAM under ROM-read is writable
        end
    endtask
    function automatic logic is_slow_class(input logic [23:0] a, input logic wr);
        return (a[23:17] == 7'b1110000) ||                       // E0/E1
               ((a[23:17] == 7'b0000000) && (a[15:8] == 8'hC0)); // I/O in 00/01
    endfunction

    //---------------------------------------------------------------- FPI clock
    int  ph2_ctr = 0;           // ticks since the fall
    int  ph0_ctr = 0;           // 0..13 (0..15 on the 65th)
    int  line_ctr = 0;          // PH0 cycles in the line, 0..64
    int  refresh_ctr = 0;
    bit  cycle_is_refresh = 0;
    bit  fast_stretch = 0;
    bit  this_slow = 0;         // current cycle classified slow at the rise
    bit  this_rom  = 0;
    longint cyc_no = 0;         // socket cycles (falls)
    bit  ph0_wrap;

    // Latched at the rise (the FPI's view of the cycle)
    logic [7:0]  bank_l;
    logic [15:0] addr_l;
    logic        rw_l, vp_l;
    real         t_fall = 0, t_rise = 0, t_prev_fall = 0;

    always @(posedge tick) begin
        // PH0 grid
        ph0_wrap = (ph0_ctr == ((line_ctr == 64) ? 15 : 13));
        if (ph0_wrap) begin
            ph0_ctr <= 0;
            if (line_ctr == 64) begin
                line_ctr <= 0;
                if (!slow_mode) fast_stretch <= 1'b1;
            end else line_ctr <= line_ctr + 1;
        end else ph0_ctr <= ph0_ctr + 1;

        ph2_ctr <= ph2_ctr + 1;

        if (ph2_ctr == 1) begin
            // rise (2 ticks low)
            ph2_bus <= 1'b1;
        end

        // end-of-cycle decision (evaluated on the tick where the fall happens)
        begin
            bit end_now = 0;
            if (slow_mode) begin
                end_now = ph0_wrap && (ph2_ctr >= 4);
            end else if (this_slow && ph2_ctr >= 3) begin
                end_now = ph0_wrap && (ph2_ctr >= 13);
            end else if (cycle_is_refresh) begin
                end_now = (ph2_ctr >= (fast_stretch ? 11 : 9));
            end else begin
                end_now = (ph2_ctr >= (fast_stretch ? 6 : 4));
            end
            if (end_now && ph2_bus) begin
                ph2_bus <= 1'b0;
                ph2_ctr <= 0;
                if (!(this_slow || slow_mode)) fast_stretch <= 1'b0;
                // refresh cadence for the NEXT cycle
                if (this_rom) begin
                    refresh_ctr <= 0; cycle_is_refresh <= 1'b0;
                end else if (this_slow || slow_mode) begin
                    cycle_is_refresh <= 1'b0;
                    refresh_ctr <= (refresh_ctr >= 8) ? 0 : refresh_ctr + 1;
                end else if (refresh_ctr >= 8) begin
                    cycle_is_refresh <= 1'b1; refresh_ctr <= 0;
                end else begin
                    cycle_is_refresh <= 1'b0; refresh_ctr <= refresh_ctr + 1;
                end
            end
        end
    end

    //---------------------------------------------------------------- checkers
    int errors = 0;
    int warnings = 0;
    bit trace = 0;
    longint max_cycles = 6000;
    // timing statistics at the socket (ns)
    real st_max_tads = 0, st_max_tbas = 0, st_min_tah = 1e9, st_min_tbh = 1e9, st_max_tmds = 0, st_min_tdhw = 1e9, st_max_rel = 0;
    bit  bh_pending = 0, dhw_pending = 0;

    task automatic err(input string s);
        errors++;
        $display("[%0t] ERROR cyc=%0d: %s", $realtime, cyc_no, s);
        if (errors > 40) begin $display("too many errors"); $finish; end
    endtask

    // shared state declared ahead of the processes that use it
    bit          wr_l_valid = 0;   // previous cycle was a write (for tDHW)
    bit          stall_req = 0;    // RDY was low at the last fall => this cycle repeats
    bit          prev_stall_req = 0;

    // address / RWB change tracking (socket side)
    real t_addr_change = -1;
    logic [15:0] a_bus_p; logic rw_bus_p; logic addr_en_p;
    always @(a_bus or rw_bus or addr_en) begin
        if (addr_en && addr_en_p && ((a_bus != a_bus_p) || (rw_bus != rw_bus_p))) begin
            t_addr_change = $realtime;
            if (ph2_bus) err($sformatf("address/RWB changed while PHI2 high (%04h rw=%b)", a_bus, rw_bus));
            else begin
                if ($realtime - t_fall < T_AH) err($sformatf("tAH: address changed %.1f ns after the fall", $realtime - t_fall));
                if (running && ($realtime - t_fall) < st_min_tah) st_min_tah = $realtime - t_fall;
            end
        end
        a_bus_p = a_bus; rw_bus_p = rw_bus; addr_en_p = addr_en;
    end

    // data-bus driver tracking
    real t_cpu_d_change = -1;    // last change of (cpu_drv, cpu_val)
    real t_cpu_release = -1;     // cpu_drv 1->0
    logic cpu_drv_p; logic [7:0] cpu_val_p;
    always @(cpu_drv or cpu_val or mem_drv) begin
        if (cpu_drv && mem_drv) err($sformatf("BUS CONTENTION: card and memory both driving D0-7 (%.1f ns after fall, %.1f ns after rise, ph2=%b)", $realtime - t_fall, $realtime - t_rise, ph2_bus));
        if ((cpu_drv != cpu_drv_p) || (cpu_drv && cpu_val != cpu_val_p)) begin
            t_cpu_d_change = $realtime;
            if (cpu_drv_p && !cpu_drv) t_cpu_release = $realtime;
            // hold checks against the most recent edges
            if (ph2_bus && ($realtime - t_rise) < T_BH && t_rise > t_fall)
                err($sformatf("tBH: bank byte changed/released %.1f ns after the rise", $realtime - t_rise));
            if (!ph2_bus && ($realtime - t_fall) < T_DHW && wr_l_valid)
                err($sformatf("tDHW: write data changed %.1f ns after the fall", $realtime - t_fall));
            if (running) begin
                if (ph2_bus && bh_pending && t_rise > t_fall) begin
                    if (($realtime - t_rise) < st_min_tbh) st_min_tbh = $realtime - t_rise;
                    bh_pending = 0;
                end
                if (!ph2_bus && dhw_pending && wr_l_valid) begin
                    if (($realtime - t_fall) < st_min_tdhw) st_min_tdhw = $realtime - t_fall;
                    dhw_pending = 0;
                end
            end
        end
        cpu_drv_p = cpu_drv; cpu_val_p = cpu_val;
    end
    // expected read data for the cycle in flight
    logic [7:0] exp_rd;
    bit         exp_rd_valid = 0;
    logic [15:0] prev_addr_l; logic [7:0] prev_bank_l; logic prev_rw_l;
    int         stall_checks = 0, be_checks = 0, sync_cycles = 0, refresh_cycles = 0, vp_fetches = 0;

    // the FPI at the rise: latch bank/address/RWB, classify, start memory
    always @(posedge ph2_bus) begin
        t_rise = $realtime;
        bh_pending = 1;
        bank_l = d_bus; addr_l = a_bus; rw_l = rw_bus; vp_l = vp_bus;
        // A repeated cycle (RDY was low at the last fall) carries no bank byte
        // (W65C816S 7.6); the FPI keeps the bank it latched for the original
        // cycle - also across a BE-low (DMA) stall, when the bus is not ours.
        if (stall_req) bank_l = prev_bank_l;
        if (running && be_bus) begin
            if (!addr_en) err("address shifters not enabled at the rise");
            if (t_addr_change > t_fall && (t_addr_change - t_fall) > T_ADS)
                err($sformatf("tADS: address settled %.1f ns after the fall", t_addr_change - t_fall));
            if (t_addr_change > t_fall && (t_addr_change - t_fall) > st_max_tads) st_max_tads = t_addr_change - t_fall;
            if (addr_l != cpu_a[15:0]) err($sformatf("latched address %04h != core %06h", addr_l, cpu_a));
            if (rw_l != cpu_we_n) err($sformatf("latched RWB %b != core %b", rw_l, cpu_we_n));
            if (vp_l != cpu_vp_n) err($sformatf("VP %b != core VPB %b", vp_l, cpu_vp_n));
            if (!vp_l) vp_fetches++;
            if (!stall_req) begin
                // a normal cycle: the bank byte must be there and be the core's
                if (!cpu_drv) err("bank byte not driven at the rise");
                if (t_cpu_d_change > t_fall && (t_cpu_d_change - t_fall) > T_BAS)
                    err($sformatf("tBAS: bank byte settled %.1f ns after the fall", t_cpu_d_change - t_fall));
                if (t_cpu_d_change > t_fall && (t_cpu_d_change - t_fall) > st_max_tbas) st_max_tbas = t_cpu_d_change - t_fall;
                if (bank_l != cpu_a[23:16]) err($sformatf("latched bank %02h != core %06h", bank_l, cpu_a));
            end
            // (repeated cycle: bank handled above)
        end
        this_slow = is_slow_class({bank_l, addr_l}, ~rw_l);
        this_rom  = is_rom({bank_l, addr_l});
        if (this_slow && !slow_mode) sync_cycles++;
        if (cycle_is_refresh) refresh_cycles++;
        // memory: reads drive after T_ACC (any read cycle, running or not)
        exp_rd_valid = 0;
        if (rw_l) begin
            exp_rd = model_read(0, {bank_l, addr_l});
            exp_rd_valid = 1;
            #(T_ACC);
            if (ph2_bus) begin
                mem_val = exp_rd;
                mem_drv = 1'b1;
            end
        end
    end

    // the fall: writes latch, read hold/garbage, checks that close the cycle
    always @(negedge ph2_bus) begin
        t_prev_fall = t_fall;
        t_fall = $realtime;
        cyc_no++;
        if (running && be_bus) begin
            if (!rw_l) begin
                // write: data must have been stable since rise + T_MDS
                if (!cpu_drv) err("write cycle: data not driven at the fall");
                if (t_cpu_d_change > t_rise + T_MDS)
                    err($sformatf("tMDS: write data settled %.1f ns after the rise", t_cpu_d_change - t_rise));
                if (t_cpu_d_change > t_rise && (t_cpu_d_change - t_rise) > st_max_tmds) st_max_tmds = t_cpu_d_change - t_rise;
                dhw_pending = 1;
                if (cpu_val != cpu_d_out) err($sformatf("write data %02h != core D_OUT %02h", cpu_val, cpu_d_out));
                if (!(stall_req)) model_write(0, {bank_l, addr_l}, cpu_val);
                if (bank_l == 8'h00 && addr_l[15:10] == 6'b000001) text_writes++;   // $0400-$07FF
                if (is_io({bank_l, addr_l})) io_writes++;
            end else begin
                if (cpu_drv) err("read cycle: card still driving D0-7 at the fall");
                if (!rx_en) err("read cycle: transceiver not in receive at the fall");
                if (!mem_drv) err("bench: memory not driving at a read fall");
                if (t_cpu_release > t_rise && (t_cpu_release - t_rise) > st_max_rel) st_max_rel = t_cpu_release - t_rise;
            end
            if (trace) $display("[%0t] cyc %0d %s %02h:%04h %s%02h%s%s%s", $realtime, cyc_no,
                                rw_l ? "R" : "W", bank_l, addr_l, rw_l ? "->" : "<-",
                                rw_l ? (mem_drv ? mem_val : 8'hxx) : cpu_val,
                                this_slow ? " sync" : "", cycle_is_refresh ? " refresh" : "",
                                stall_req ? " STALL" : "");
        end
        wr_l_valid = running && !rw_l;
        prev_stall_req = stall_req;
        stall_req = ~rdy_bus;
        prev_addr_l = addr_l; prev_bank_l = bank_l; prev_rw_l = rw_l;
    end

    // read data hold, then garbage, then float
    always @(negedge ph2_bus) begin
        if (mem_drv) begin
            #(T_DHR);
            mem_val = ~mem_val;          // garbage
            #(T_GARB - T_DHR);
            mem_drv = 1'b0;
        end
    end

    // The core samples D_IN on its clock edge = the pad-side fall.  What it
    // sees there must be what memory drove for this cycle.
    longint ce_count = 0;
    always @(posedge cpu_clk) begin
        if (cpu_rst_n && be_bus) begin
            if (rw_l && exp_rd_valid && gs_d_i_f != exp_rd)
                err($sformatf("read data at the core %02h != memory %02h at %02h:%04h", gs_d_i_f, exp_rd, bank_l, addr_l));
            if (gs_rdy_f) ce_count++;
        end
        // lockstep compare with the reference (pre-edge values = the cycle ending now)
        if (cpu_rst_n) begin
            if (cpu_a != ref_a || cpu_we_n != ref_we_n || cpu_vp_n != ref_vp_n || (!cpu_we_n && cpu_d_out != ref_dout))
                err($sformatf("DIVERGENCE from reference: dut %06h rw%b vp%b d%02h  ref %06h rw%b vp%b d%02h",
                              cpu_a, cpu_we_n, cpu_vp_n, cpu_d_out, ref_a, ref_we_n, ref_vp_n, ref_dout));
            if (ref_we_n && be_bus && gs_d_i_f != ref_din)
                err($sformatf("read data mismatch vs reference at %06h: dut %02h ref %02h", ref_a, gs_d_i_f, ref_din));
            if (!ref_we_n && gs_rdy_f) model_write(1, ref_a, ref_dout);
            // boot-progress bookkeeping (opcode fetches)
            if (ref_vpa && ref_vda && ref_we_n && gs_rdy_f) begin
                if (!seen_pc[ref_a]) begin seen_pc[ref_a] = 1; distinct_pcs++; end
                last_pcs[last_pcs_i] = ref_a; last_pcs_i = (last_pcs_i + 1) % 16;
            end
        end
    end
    bit     seen_pc [0:(1<<24)-1];
    int     distinct_pcs = 0;
    logic [23:0] last_pcs [0:15];
    int     last_pcs_i = 0;

    // Stalled cycle must repeat the same bus cycle
    always @(posedge ph2_bus) if (running && be_bus && stall_req) begin
        stall_checks++;
        if (addr_l != prev_addr_l || bank_l != prev_bank_l || rw_l != prev_rw_l)
            err($sformatf("stalled cycle did not repeat: %02h:%04h rw%b vs %02h:%04h rw%b", bank_l, addr_l, rw_l, prev_bank_l, prev_addr_l, prev_rw_l));
    end

    //---------------------------------------------------------------- program
    task automatic poke(input logic [23:0] a, input logic [7:0] v);
        mem[a] = v; mem_r[a] = v;
    endtask
    task automatic load_program();
        // bank 0, ROM at $F000
        poke(24'h00F000, 8'h78);                                   // SEI
        poke(24'h00F001, 8'hA9); poke(24'h00F002, 8'h80);          // LDA #$80
        poke(24'h00F003, 8'h8D); poke(24'h00F004, 8'h36); poke(24'h00F005, 8'hC0); // STA $C036 (fast)
        poke(24'h00F006, 8'hA9); poke(24'h00F007, 8'h55);          // LDA #$55
        poke(24'h00F008, 8'h8D); poke(24'h00F009, 8'h00); poke(24'h00F00A, 8'h03); // STA $0300
        poke(24'h00F00B, 8'hA9); poke(24'h00F00C, 8'hAA);          // LDA #$AA
        poke(24'h00F00D, 8'h8D); poke(24'h00F00E, 8'h01); poke(24'h00F00F, 8'h03); // STA $0301
        poke(24'h00F010, 8'hAD); poke(24'h00F011, 8'h00); poke(24'h00F012, 8'h03); // LDA $0300
        poke(24'h00F013, 8'h18);                                   // CLC
        poke(24'h00F014, 8'h6D); poke(24'h00F015, 8'h01); poke(24'h00F016, 8'h03); // ADC $0301
        poke(24'h00F017, 8'h8D); poke(24'h00F018, 8'h02); poke(24'h00F019, 8'h03); // STA $0302
        poke(24'h00F01A, 8'hEE); poke(24'h00F01B, 8'h02); poke(24'h00F01C, 8'h03); // INC $0302
        poke(24'h00F01D, 8'h20); poke(24'h00F01E, 8'h40); poke(24'h00F01F, 8'hF0); // JSR $F040
        poke(24'h00F020, 8'hA9); poke(24'h00F021, 8'h01);          // LDA #$01
        poke(24'h00F022, 8'h8D); poke(24'h00F023, 8'h03); poke(24'h00F024, 8'h03); // STA $0303
        poke(24'h00F025, 8'hAD); poke(24'h00F026, 8'h30); poke(24'h00F027, 8'hC0); // LDA $C030
        poke(24'h00F028, 8'h8F); poke(24'h00F029, 8'h00); poke(24'h00F02A, 8'h20); poke(24'h00F02B, 8'hE1); // STA $E12000
        poke(24'h00F02C, 8'hAF); poke(24'h00F02D, 8'h00); poke(24'h00F02E, 8'h20); poke(24'h00F02F, 8'hE1); // LDA $E12000
        poke(24'h00F030, 8'h8D); poke(24'h00F031, 8'h07); poke(24'h00F032, 8'h03); // STA $0307
        poke(24'h00F033, 8'h58);                                   // CLI
        poke(24'h00F034, 8'h4C); poke(24'h00F035, 8'h00); poke(24'h00F036, 8'h04); // JMP $0400 (loop lives in RAM)
        // RAM-resident main loop (RAM accesses are what the FPI refreshes around)
        poke(24'h000400, 8'hEE); poke(24'h000401, 8'h06); poke(24'h000402, 8'h03); // INC $0306
        poke(24'h000403, 8'hAD); poke(24'h000404, 8'h00); poke(24'h000405, 8'h03); // LDA $0300
        poke(24'h000406, 8'h8D); poke(24'h000407, 8'h08); poke(24'h000408, 8'h03); // STA $0308
        poke(24'h000409, 8'h4C); poke(24'h00040A, 8'h00); poke(24'h00040B, 8'h04); // JMP $0400
        poke(24'h00F040, 8'hA2); poke(24'h00F041, 8'h42);          // LDX #$42
        poke(24'h00F042, 8'h8E); poke(24'h00F043, 8'h04); poke(24'h00F044, 8'h03); // STX $0304
        poke(24'h00F045, 8'h60);                                   // RTS
        poke(24'h00F050, 8'hEE); poke(24'h00F051, 8'h05); poke(24'h00F052, 8'h03); // INC $0305
        poke(24'h00F053, 8'h40);                                   // RTI
        poke(24'h00FFFC, 8'h00); poke(24'h00FFFD, 8'hF0);          // RESET vector
        poke(24'h00FFFE, 8'h50); poke(24'h00FFFF, 8'hF0);          // IRQ/BRK vector
    endtask

    //---------------------------------------------------------------- stimulus
    task automatic wait_cycles(input int n);
        repeat (n) @(negedge ph2_bus);
    endtask

    // pull RDY low for exactly one cycle, changed at the rise (tPCS-safe)
    task automatic stall_one();
        @(posedge ph2_bus); #20; rdy_bus = 1'b0;
        @(posedge ph2_bus); #20; rdy_bus = 1'b1;
    endtask

    initial begin
        int i;
        longint c;
        if ($test$plusargs("trace")) trace = 1;
        if ($test$plusargs("noturbo")) noturbo = 1;
        if (!$value$plusargs("cycles=%d", max_cycles)) max_cycles = 6000;
        if ($test$plusargs("vcd")) begin $dumpfile("gs_socket.vcd"); $dumpvars(0, tb_gs_socket); end
        begin
            string rom_path;
            if ($value$plusargs("rom=%s", rom_path)) begin
                int fd, n;
                fd = $fopen(rom_path, "rb");
                if (fd == 0) begin $display("cannot open ROM %s", rom_path); $finish; end
                n = $fread(rom, fd);
                $fclose(fd);
                if (n != (1<<17)) begin $display("ROM %s: read %0d bytes, want 131072", rom_path, n); $finish; end
                rom_loaded = 1;
                if (!$value$plusargs("cycles=%d", max_cycles) || max_cycles == 6000) max_cycles = 200000;
                $display("---- ROM mode: %s loaded, reset vector %02h%02h", rom_path, rom[17'h1FFFD], rom[17'h1FFFC]);
            end
        end

        if (!rom_loaded) load_program();
        cpu_drv_p = 0; cpu_val_p = 0; a_bus_p = 0; rw_bus_p = 1; addr_en_p = 0;

        #200 rst_n = 1'b1;
        // machine powered, PH2 running, /RES held low; card not armed yet
        wait_cycles(20);
        if (addr_en || cpu_drv) err("unarmed card is driving the socket");
        armed = 1'b1;
        wait_cycles(10);
        if (!ph2_alive) err("PH2 not detected alive");
        if (cpu_drv) err("card drove data while /RES low");
        // release reset at a rise (tPCS-safe)
        @(posedge ph2_bus); #30; res_n_bus = 1'b1;

        if (rom_loaded) begin
            // ROM run: periodic RDY stalls, one DMA-style BE+RDY pause, then run out
            longint next_stall = 997;
            while (cyc_no < max_cycles) begin
                wait_cycles(1);
                if (cyc_no >= next_stall) begin stall_one(); next_stall += 997; end
                if (cyc_no == 50000) begin
                    @(posedge ph2_bus); #20; rdy_bus = 1'b0; be_bus = 1'b0;
                    #(T_BVD); be_checks++;
                    if (addr_en) err("BE low: address shifters still enabled after tBVD");
                    if (cpu_drv) err("BE low: data still driven after tBVD");
                    wait_cycles(5);
                    @(posedge ph2_bus); #20; be_bus = 1'b1; rdy_bus = 1'b1;
                end
            end
            $display("---- ROM run: %0d cycles, %0d distinct opcode addresses, %0d text-page writes, %0d I/O writes, %s mode",
                     cyc_no, distinct_pcs, text_writes, io_writes, slow_mode ? "slow" : "fast");
            $display("---- last opcode fetches: %06h %06h %06h %06h %06h %06h %06h %06h",
                     last_pcs[(last_pcs_i+8)%16], last_pcs[(last_pcs_i+9)%16], last_pcs[(last_pcs_i+10)%16], last_pcs[(last_pcs_i+11)%16],
                     last_pcs[(last_pcs_i+12)%16], last_pcs[(last_pcs_i+13)%16], last_pcs[(last_pcs_i+14)%16], last_pcs[(last_pcs_i+15)%16]);
            if (distinct_pcs < 100) err("ROM run: fewer than 100 distinct opcode addresses");
        end else begin
        // let the program run; sprinkle RDY stalls
        for (i = 0; i < 40; i++) begin
            wait_cycles(37);
            stall_one();
        end
        if (mem[24'h000300] != 8'h55 || mem[24'h000301] != 8'hAA) err("program: $0300/$0301 wrong");
        if (mem[24'h000302] != 8'h00) err($sformatf("program: $0302 = %02h (want 00)", mem[24'h000302]));
        if (mem[24'h000303] != 8'h01) err("program: $0303 wrong (JSR/RTS path)");
        if (mem[24'h000304] != 8'h42) err("program: $0304 wrong (subroutine)");
        if (mem[24'h000307] != IO_READ[7:0]) err($sformatf("program: $0307 = %02h (want %02h via $C030/$E12000)", mem[24'h000307], IO_READ[7:0]));
        if (mem[24'hE12000] != IO_READ[7:0]) err("program: long write to $E12000 missing");
        if (slow_mode && !noturbo) err("program: $C036 write did not switch to fast mode");
        if (mem[24'h000306] == 8'h00) err("program: main loop not running");

        // IRQ: assert at a rise, expect the vector fetch and the handler
        begin
            int b4 = mem[24'h000305];
            longint vp_before = vp_fetches;
            @(posedge ph2_bus); #20; irq_n_bus = 1'b0;
            c = 0;
            while (mem[24'h000305] == b4 && c < 200) begin wait_cycles(1); c++; end
            if (mem[24'h000305] == b4) err("IRQ handler did not run");
            if (vp_fetches == vp_before) err("no VP-low vector fetch seen for the IRQ");
            @(posedge ph2_bus); #20; irq_n_bus = 1'b1;
            wait_cycles(20);
            if (mem[24'h000305] != b4 + 1) err($sformatf("IRQ handler ran %0d times", mem[24'h000305] - b4));
        end

        // BE + RDY low (what a DMA would do): bus must float within tBVD
        begin
            @(posedge ph2_bus); #20; rdy_bus = 1'b0; be_bus = 1'b0;
            #(T_BVD);
            be_checks++;
            if (addr_en) err("BE low: address shifters still enabled after tBVD");
            if (cpu_drv) err("BE low: data still driven after tBVD");
            wait_cycles(5);
            @(posedge ph2_bus); #20; be_bus = 1'b1; rdy_bus = 1'b1;
            wait_cycles(10);
        end
        begin
            logic [7:0] v = mem[24'h000306];
            wait_cycles(60);
            if (mem[24'h000306] == v) err("main loop stopped after BE/RDY release");
        end
        end // program mode

        // run out the clock
        while (cyc_no < max_cycles) wait_cycles(100);

        $display("---- gs_socket sim: %0d socket cycles, %0d CE, %0d stalls checked, %0d sync, %0d refresh, %0d VP fetches, %0d BE checks",
                 cyc_no, ce_count, stall_checks, sync_cycles, refresh_cycles, vp_fetches, be_checks);
        $display("---- telemetry: cycle_count=%0d stall_count=%0d be_count=%0d", cycle_count, stall_count, be_count);
        if (stall_checks == 0) err("no stall cycles were checked");
        if (sync_cycles == 0 && !noturbo) err("no sync cycles occurred");
        if (refresh_cycles == 0 && !noturbo && !rom_loaded) err("no refresh cycles occurred");
        $display("---- socket timing (ns): tADS max %.1f  tBAS max %.1f  tAH min %.1f  tBH min %.1f  tMDS max %.1f  tDHW min %.1f  read release after rise max %.1f",
                 st_max_tads, st_max_tbas, st_min_tah, st_min_tbh, st_max_tmds, st_min_tdhw, st_max_rel);
        if (errors == 0) $display("PASS");
        else $display("FAIL: %0d errors", errors);
        $finish;
    end

    // global timeout
    initial begin
        #1;                                   // after plusargs are read
        #(1_000_000 + max_cycles * 1200);     // ns: worst case ~1 us per cycle in slow mode
        $display("TIMEOUT"); $finish;
    end

endmodule
