///////////////////////////////////////////////////////////////////////////////
// a2mega_dp_gowin_top.sv — 1.0a3 DisplayPort colorbars via GOWIN'S IP STACK
//
// WS4 of the a2mega bring-up: the same 1080p 2-lane HBR colorbars payload
// as a2mega_dp_test_top.sv, but with the main-link stream side and PHY
// replaced by Gowin's own supported IP (EDP Encoder v2.7 + EDP PHY
// emission, hardened PCS 8b10b) — see dp_gowin_transmitter.sv. The AUX
// front end, HPD, LEDs, and the 115200-baud debug UART telemetry are
// carried over from the dp_test top so bench A/B comparison is 1:1.
//
//   led[0]  heartbeat (~1.5 Hz)      — bitstream alive
//   led[1]  HPD level                — PD negotiation delivered a sink
//   led[2]  clk_sym == 135 MHz       — QPLL/line rate verified
//   led[3]  video_live               — main stream is being transmitted
//
// Differences vs the dp_test build (accepted for this evaluation):
//   - no audio (the Gowin encoder has no audio/SDP inputs)
//   - video is generated push-style at 2 px/clk in a 74.25 MHz stream
//     domain (the encoder's line buffer owns the strm->ls CDC)
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module a2mega_dp_gowin_top (
    input  logic clk50_in,          // 50 MHz SOM oscillator (V22)
    input  logic button,            // SOM pushbutton, active low (AB13)

    // DP AUX channel: true-LVDS front end (proven closed-loop config)
    inout  wire  dp_aux_p,          // G15 (DPAUX_P, J2.64)
    inout  wire  dp_aux_n,          // G16 (DPAUX_N, J2.62)
    input  logic dp_hpd,            // A19 (ESP32_GPIO0, J2.17)

    output logic [3:0] led,         // D15 D14 R14 P14, active low

    // Debug UART to the ESP32 (H13 -> ESP32 Serial1 -> telnet tee)
    output logic uart_tx
);

    // ------------------------------------------------------------------
    // 100 MHz management/AUX clock from the board oscillator
    // ------------------------------------------------------------------
    logic clk100;
    gowin_mgmt_pll i_mgmt_pll (.lock(), .clkout(clk100), .clkin(clk50_in));

    // ------------------------------------------------------------------
    // AUX analog front end — the proven closed-loop configuration from
    // a2mega_dp_test_top.sv (TLVDS_IOBUF, no RX inversion, counter-park
    // after each burst). See that file's header for the full history.
    // ------------------------------------------------------------------
    logic auxch_in, auxch_out, auxch_tri;

    logic       tri_d = 1'b1;
    logic       out_last = 1'b0;
    logic [8:0] park_cnt = 9'd0;
    always_ff @(posedge clk100) begin
        tri_d <= auxch_tri;
        if (!auxch_tri)
            out_last <= auxch_out;
        if (auxch_tri && !tri_d)
            park_cnt <= 9'd250;          // 2.5 us @ 100 MHz
        else if (park_cnt != 9'd0)
            park_cnt <= park_cnt - 9'd1;
    end
    wire parking = (park_cnt != 9'd0);

    TLVDS_IOBUF i_aux_diff (
        .O   (auxch_in),
        .IO  (dp_aux_p),
        .IOB (dp_aux_n),
        .I   (parking ? ~out_last : auxch_out),
        .OEN (auxch_tri && !parking)
    );

    // ------------------------------------------------------------------
    // Video timing + colorbars, strm domain (74.25 MHz, 2 px/clk).
    // 1080p: H 1920/2200/44/192, V 1080/1125/5/41, positive syncs.
    // Line layout (pair-of-pixels units, 1100/line): hsync [0,22),
    // active [96,1056). Frame: vsync lines [0,5), active [41,1121).
    // Pattern: eight 240-px vertical bars + 2-px white frame border.
    // ------------------------------------------------------------------
    logic clk_strm;
    logic [10:0] hx = '0;           // 0..1099 pixel pairs
    logic [10:0] vy = '0;           // 0..1124 lines
    logic        pix_vs, pix_hs, pix_de;
    logic [23:0] pix_data0, pix_data1;

    localparam int HP_TOTAL     = 1100;  // 2200 / 2
    localparam int HP_SYNC      = 22;    // 44 / 2
    localparam int HP_ACT_START = 96;    // 192 / 2
    localparam int HP_ACT_END   = 1056;  // (192 + 1920) / 2
    localparam int V_TOTAL_L    = 1125;
    localparam int V_SYNC_L     = 5;
    localparam int V_ACT_START  = 41;
    localparam int V_ACT_END    = 1121;

    function automatic [23:0] bar24(input [11:0] x);
        if      (x < 12'd240)  bar24 = 24'hFFFFFF;   // white
        else if (x < 12'd480)  bar24 = 24'hFFFF00;   // yellow
        else if (x < 12'd720)  bar24 = 24'h00FFFF;   // cyan
        else if (x < 12'd960)  bar24 = 24'h00FF00;   // green
        else if (x < 12'd1200) bar24 = 24'hFF00FF;   // magenta
        else if (x < 12'd1440) bar24 = 24'hFF0000;   // red
        else if (x < 12'd1680) bar24 = 24'h0000FF;   // blue
        else                   bar24 = 24'h000000;   // black
    endfunction

    function automatic [23:0] pixel24(input [11:0] x, input [10:0] y);
        if (x < 12'd2 || x >= 12'd1918 || y < 11'd2 || y >= 11'd1078)
            pixel24 = 24'hFFFFFF;                    // frame border
        else
            pixel24 = bar24(x);
    endfunction

    wire [11:0] act_x0 = {(hx - 11'(HP_ACT_START)), 1'b0};  // (hx-96)*2
    wire [10:0] act_y  = vy - 11'(V_ACT_START);

    always_ff @(posedge clk_strm) begin
        if (hx == 11'(HP_TOTAL - 1)) begin
            hx <= '0;
            vy <= (vy == 11'(V_TOTAL_L - 1)) ? '0 : vy + 11'd1;
        end else
            hx <= hx + 11'd1;

        pix_hs    <= (hx < 11'(HP_SYNC));
        pix_vs    <= (vy < 11'(V_SYNC_L));
        pix_de    <= (hx >= 11'(HP_ACT_START)) && (hx < 11'(HP_ACT_END))
                  && (vy >= 11'(V_ACT_START))  && (vy < 11'(V_ACT_END));
        pix_data0 <= pixel24(act_x0,          act_y);
        pix_data1 <= pixel24(act_x0 | 12'd1,  act_y);
    end

    // ------------------------------------------------------------------
    // DisplayPort transmitter on the Gowin IP stack
    // ------------------------------------------------------------------
    logic link_established, video_live;
    logic [7:0] debug;
    logic [7:0] serdes_status;
    logic [15:0] aux_dbg_rx;
    logic [7:0]  aux_dbg_gate;
    logic [15:0] aux_dbg_adjust;
    logic [15:0] aux_dbg_chstate;
    logic [7:0]  aux_dbg_sink;
    logic [7:0]  aux_dbg_caps;
    logic       hpd_present_w;
    logic [4:0]  drp_idx;
    logic [31:0] drp_data;
    logic [23:0] drp_addr;
    logic        drp_done;
    logic        clk_sym_w;

    dp_gowin_transmitter #(
        .LINK_RATE_MBPS (2700),
        .BLIND_SINK     (0),
        .H_VISIBLE (1920), .H_TOTAL (2200), .H_SYNC_WIDTH (44), .H_START (192),
        .V_VISIBLE (1080), .V_TOTAL (1125), .V_SYNC_WIDTH (5),  .V_START (41)
    ) i_dp (
        .clk100          (clk100),
        .clk_strm        (clk_strm),
        .pix_vs          (pix_vs),
        .pix_hs          (pix_hs),
        .pix_de          (pix_de),
        .pix_data0       (pix_data0),
        .pix_data1       (pix_data1),
        .hpd             (dp_hpd),
        .auxch_in        (auxch_in),
        .auxch_out       (auxch_out),
        .auxch_tri       (auxch_tri),
        .link_established(link_established),
        .video_live      (video_live),
        .debug           (debug),
        .debug_rx        (aux_dbg_rx),
        .debug_gate      (aux_dbg_gate),
        .debug_sink      (aux_dbg_sink),
        .debug_adjust    (aux_dbg_adjust),
        .debug_chstate   (aux_dbg_chstate),
        .debug_caps      (aux_dbg_caps),
        .clk_symbol_out  (clk_sym_w),
        .serdes_status   (serdes_status),
        .hpd_present_out (hpd_present_w),
        .drp_dbg_idx     (drp_idx),
        .drp_dbg_data    (drp_data),
        .drp_dbg_addr    (drp_addr),
        .drp_dbg_done    (drp_done)
    );

    // ------------------------------------------------------------------
    // Debug UART: same format as a2mega_dp_test_top.sv ("DP S:.. D:..
    // F:.. HLVC:... P:. E:.. R:.. A:.... G:.. Y:.. C:.... Q:....... K:..
    // X:..") interleaved with CR register-dump lines, so the ESP32
    // telnet tee and the bench playbooks read identically.
    // ------------------------------------------------------------------
    logic [26:0] c100_cnt = '0;
    always_ff @(posedge clk100)
        c100_cnt <= c100_cnt + 27'd1;

    logic [7:0] frame_cnt = '0;
    logic       vs_d = 1'b0;
    always_ff @(posedge clk_strm) begin
        vs_d <= pix_vs;
        if (pix_vs && !vs_d)             // one frame per vsync rise
            frame_cnt <= frame_cnt + 8'd1;
    end

    // loose 2FF samples into clk50 (quasi-static / diagnostic only)
    logic [7:0] st_s0, st_s;
    logic [7:0] dbg_s0, dbg_s, frm_s0, frm_s;
    logic [2:0] flg_s0, flg_s;
    logic       c100_s0, c100_s;
    logic [16:0] hp_s0, hp_s;
    logic [7:0]  gate_s0, gate_s;
    logic [15:0] adj_s0, adj_s;
    logic [15:0] chst_s0, chst_s;
    logic [27:0] symd_s0, symd_s;
    logic [7:0] snk_s0, snk_s, cap_s0, cap_s;
    always_ff @(posedge clk50_in) begin
        st_s0  <= serdes_status;  st_s  <= st_s0;
        dbg_s0 <= debug;          dbg_s <= dbg_s0;
        frm_s0 <= frame_cnt;      frm_s <= frm_s0;
        flg_s0 <= {dp_hpd, link_established, video_live};
        c100_s0 <= c100_cnt[26];  c100_s <= c100_s0;
        hp_s0 <= {hpd_present_w, aux_dbg_rx};  hp_s <= hp_s0;
        flg_s  <= flg_s0;
        gate_s0 <= aux_dbg_gate;   gate_s <= gate_s0;
        adj_s0  <= aux_dbg_adjust; adj_s  <= adj_s0;
        chst_s0 <= aux_dbg_chstate; chst_s <= chst_s0;
        symd_s0 <= sym_delta;       symd_s <= symd_s0;
        snk_s0 <= aux_dbg_sink;     snk_s <= snk_s0;
        cap_s0 <= aux_dbg_caps;     cap_s <= cap_s0;
    end

    // Y: link/video rise odometer (4-bit saturating each)
    logic [3:0] link_rises = '0, vid_rises = '0;
    logic       link_d = 1'b0, vid_d = 1'b0;
    always_ff @(posedge clk50_in) begin
        link_d <= flg_s[1];
        vid_d  <= flg_s[0];
        if (flg_s[1] && !link_d && link_rises != 4'hF)
            link_rises <= link_rises + 4'd1;
        if (flg_s[0] && !vid_d && vid_rises != 4'hF)
            vid_rises <= vid_rises + 4'd1;
    end

    function automatic [7:0] hexch(input [3:0] n);
        hexch = (n < 4'd10) ? (8'h30 + 8'(n)) : (8'h37 + 8'(n));
    endfunction

    localparam int MSG_LEN = 86;   // msg_idx is [6:0]
    logic [7:0] msg [0:MSG_LEN-1];
    logic        line_is_reg = 1'b0;
    logic [4:0]  drp_idx_q = 5'd0;
    logic [31:0] reg_data_l = '0;
    logic [23:0] reg_addr_l = '0;
    logic        reg_done_l = 1'b0;
    assign drp_idx = drp_idx_q;
    always_comb begin
        if (line_is_reg) begin
            msg[0]="C"; msg[1]="R"; msg[2]=" ";
            msg[3]=hexch({3'b0, drp_idx_q[4]}); msg[4]=hexch(drp_idx_q[3:0]);
            msg[5]=" ";
            msg[6]=hexch(reg_addr_l[23:20]); msg[7]=hexch(reg_addr_l[19:16]);
            msg[8]=hexch(reg_addr_l[15:12]); msg[9]=hexch(reg_addr_l[11:8]);
            msg[10]=hexch(reg_addr_l[7:4]);  msg[11]=hexch(reg_addr_l[3:0]);
            msg[12]=" ";
            msg[13]=hexch(reg_data_l[31:28]); msg[14]=hexch(reg_data_l[27:24]);
            msg[15]=hexch(reg_data_l[23:20]); msg[16]=hexch(reg_data_l[19:16]);
            msg[17]=hexch(reg_data_l[15:12]); msg[18]=hexch(reg_data_l[11:8]);
            msg[19]=hexch(reg_data_l[7:4]);   msg[20]=hexch(reg_data_l[3:0]);
            msg[21]=" "; msg[22]="W";
            msg[23]=8'h30 + 8'(reg_done_l);
            for (int i = 24; i < 85; i++) msg[i] = " ";
            msg[85]=8'h0A;
        end else begin
        msg[0]="D"; msg[1]="P"; msg[2]=" "; msg[3]="S"; msg[4]=":";
        msg[5]=hexch(st_s[7:4]); msg[6]=hexch(st_s[3:0]);
        msg[7]=" "; msg[8]="D"; msg[9]=":";
        msg[10]=hexch(dbg_s[7:4]); msg[11]=hexch(dbg_s[3:0]);
        msg[12]=" "; msg[13]="F"; msg[14]=":";
        msg[15]=hexch(frm_s[7:4]); msg[16]=hexch(frm_s[3:0]);
        msg[17]=" "; msg[18]="H"; msg[19]="L"; msg[20]="V"; msg[21]="C";
        msg[22]=":";
        msg[23]=8'h30 + 8'(flg_s[2]);
        msg[24]=8'h30 + 8'(flg_s[1]);
        msg[25]=8'h30 + 8'(flg_s[0]);
        msg[26]=8'h30 + 8'(c100_s);
        msg[27]=" "; msg[28]="P"; msg[29]=":";
        msg[30]=8'h30 + 8'(hp_s[16]);
        msg[31]=" "; msg[32]="E"; msg[33]=":";
        msg[34]=hexch(hp_s[7:4]);
        msg[35]=hexch(hp_s[3:0]);
        msg[36]=" "; msg[37]="R"; msg[38]=":";
        msg[39]=hexch(hp_s[15:12]);
        msg[40]=hexch(hp_s[11:8]);
        msg[41]=" "; msg[42]="A"; msg[43]=":";
        msg[44]=hexch(adj_s[15:12]);
        msg[45]=hexch(adj_s[11:8]);
        msg[46]=hexch(adj_s[7:4]);
        msg[47]=hexch(adj_s[3:0]);
        msg[48]=" "; msg[49]="G"; msg[50]=":";
        msg[51]=hexch(gate_s[7:4]);
        msg[52]=hexch(gate_s[3:0]);
        msg[53]=" "; msg[54]="Y"; msg[55]=":";
        msg[56]=hexch(link_rises);
        msg[57]=hexch(vid_rises);
        msg[58]=" "; msg[59]="C"; msg[60]=":";
        msg[61]=hexch(chst_s[15:12]);
        msg[62]=hexch(chst_s[11:8]);
        msg[63]=hexch(chst_s[7:4]);
        msg[64]=hexch(chst_s[3:0]);
        msg[65]=" "; msg[66]="Q"; msg[67]=":";
        msg[68]=hexch(symd_s[27:24]);
        msg[69]=hexch(symd_s[23:20]);
        msg[70]=hexch(symd_s[19:16]);
        msg[71]=hexch(symd_s[15:12]);
        msg[72]=hexch(symd_s[11:8]);
        msg[73]=hexch(symd_s[7:4]);
        msg[74]=hexch(symd_s[3:0]);
        msg[75]=" "; msg[76]="K"; msg[77]=":";
        msg[78]=hexch(snk_s[7:4]);
        msg[79]=hexch(snk_s[3:0]);
        msg[80]=" "; msg[81]="X"; msg[82]=":";
        msg[83]=hexch(cap_s[7:4]);
        msg[84]=hexch(cap_s[3:0]);
        msg[85]=8'h0A;
        end
    end

    // 115200 baud from 50 MHz (divisor 434); one message per ~0.5 s
    logic [8:0]  baud_cnt = '0;
    logic        baud_tick;
    logic [24:0] msg_timer = '0;
    logic [6:0]  msg_idx = MSG_LEN[6:0];   // idle when == MSG_LEN
    logic [3:0]  bit_idx = '0;
    logic [9:0]  shifter = 10'h3FF;
    always_ff @(posedge clk50_in) begin
        baud_tick <= 1'b0;
        if (baud_cnt == 9'd433) begin baud_cnt <= '0; baud_tick <= 1'b1; end
        else baud_cnt <= baud_cnt + 9'd1;

        msg_timer <= msg_timer + 25'd1;
        if (msg_timer[23:0] == 24'd0 && msg_idx == MSG_LEN[6:0]) begin
            msg_idx <= '0;                 // start a new message
            bit_idx <= 4'd10;              // force reload on next tick
            line_is_reg <= ~line_is_reg;   // alternate status / register
            if (line_is_reg)               // last line was a reg: advance
                drp_idx_q <= (drp_idx_q == 5'd23) ? 5'd0 : drp_idx_q + 5'd1;
            reg_data_l <= drp_data;        // latch (quasi-static source)
            reg_addr_l <= drp_addr;
            reg_done_l <= drp_done;
        end

        if (baud_tick && msg_idx != MSG_LEN[6:0]) begin
            if (bit_idx >= 4'd10) begin    // load next char: start+8+stop
                shifter <= {1'b1, msg[msg_idx], 1'b0};
                bit_idx <= 4'd0;
            end else begin
                shifter <= {1'b1, shifter[9:1]};
                bit_idx <= bit_idx + 4'd1;
                if (bit_idx == 4'd9)
                    msg_idx <= msg_idx + 6'd1;
            end
        end
    end
    assign uart_tx = (msg_idx == MSG_LEN[6:0]) ? 1'b1 : shifter[0];

    // ------------------------------------------------------------------
    // Line-rate verification: count clk_sym over a 1 s crystal window.
    // 2.7 Gb/s / 20 = 135 M +/- tol -> led[2].
    // ------------------------------------------------------------------
    logic [25:0] win_cnt = '0;          // 0..49,999,999 @ 50 MHz
    logic        win_tgl = 1'b0;
    always_ff @(posedge clk50_in) begin
        if (win_cnt == 26'd49_999_999) begin
            win_cnt <= '0;
            win_tgl <= ~win_tgl;
        end else
            win_cnt <= win_cnt + 26'd1;
    end

    logic [27:0] sym_cnt = '0, sym_last = '0, sym_delta = '0;
    logic [2:0]  tgl_sync = '0;
    logic        freq_ok = 1'b0;
    always_ff @(posedge clk_sym_w) begin
        sym_cnt  <= sym_cnt + 28'd1;
        tgl_sync <= {tgl_sync[1:0], win_tgl};
        if (tgl_sync[2] != tgl_sync[1]) begin
            sym_delta <= sym_cnt - sym_last;   // registered snapshot
            sym_last  <= sym_cnt;
        end
        // 135 M +/- ~2%: 132.3M .. 137.7M (pipelined vs the snapshot)
        freq_ok <= (sym_delta > 28'd132_300_000) &&
                   (sym_delta < 28'd137_700_000);
    end

    // ------------------------------------------------------------------
    // Bring-up ladder on the LEDs (active low)
    // ------------------------------------------------------------------
    logic [24:0] hb_cnt;
    always_ff @(posedge clk50_in)
        hb_cnt <= hb_cnt + 25'd1;

    assign led[0] = ~hb_cnt[24];        // heartbeat
    assign led[1] = ~dp_hpd;            // HPD from the ESP32
    assign led[2] = ~freq_ok;           // clk_sym == 135 MHz (line rate OK)
    assign led[3] = ~video_live;        // pixels flowing

endmodule
