///////////////////////////////////////////////////////////////////////////////
// sdp_engine.v : Audio secondary-data-packet scheduler, builder and inserter
//
// Part of the DisplayPort_Verilog project - an open implementation of the
// DisplayPort protocol for FPGA boards.
//
// Inserts Audio_TimeStamp (HB1=0x01), Audio_Stream (HB1=0x02) and Audio
// InfoFrame (HB1=0x84) secondary data packets into the hblank/vblank gaps
// flagged by video_stream_packer (sdp_gap), framed SS ... SE per DP 1.1a
// section 2.2.6:
//
//  - every header byte HB0..HB3 gets its own RS(15,13)/GF(16) parity byte
//  - every 4 payload bytes get one parity byte (packets here always carry
//    32 payload bytes -> 8 groups, PB4..PB11)
//  - GF(2^4) primitive polynomial x^4+x+1, generator (x-a^0)(x-a^1);
//    parity computed by a 2-register nibble LFSR, fed low nibble first;
//    parity byte = {x0, x1}
//  - on the wire, paired lanes swap high nibbles byte-for-byte (2 lanes);
//    on 1 lane consecutive codewords interleave per Figures 2-27/2-29
//
// All three packet types carry exactly 32 payload bytes (InfoFrame is 28
// zero-padded to 32), so the wire length is constant: 24 bytes per lane
// on 2 lanes, 48 bytes on 1 lane, plus SS/SE.
//
// Scheduling per gap: Audio_TimeStamp (once per frame) > Audio_Stream
// (4-sample buffer ready) > Audio InfoFrame (once per frame). VB-ID
// AudioMute is held until the first TimeStamp and Stream packets have
// been sent.
//
// Verified against the DP 1.1a spec test vectors and misc/dp_sdp_golden.py.
//
// MIT License - part of work derived from Copyright (c) 2019 Mike Field
///////////////////////////////////////////////////////////////////////////////
`timescale 1ns / 1ps

module sdp_engine #(
    parameter LANE_COUNT      = 2,      // 1 or 2
    parameter AUDIO_BIT_WIDTH = 16,     // 16 only for now (subframe packing)
    parameter AUDIO_RATE      = 48000
)(
    input             clk,              // tx_symbol_clk
    input             reset,
    // main link stream (one cycle latency through this module)
    input      [72:0] in_data,
    input             sdp_gap,          // safe insertion region, from packer
    input             frame_pulse,      // one-cycle pulse entering vblank
    output reg [72:0] out_data,
    // audio sample buffer (audio_sample_buffer.v)
    input [4*2*AUDIO_BIT_WIDTH-1:0] buffer,
    input             buffer_ready,
    output reg        buffer_take,
    // audio clock recovery
    input      [23:0] maud,
    // VB-ID audio mute (to packer)
    output reg        audio_mute
);

    localparam [8:0] SS = 9'b101011100;   // K28.2
    localparam [8:0] SE = 9'b111111101;   // K29.7

    localparam [23:0] NAUD = 24'd32768;

    // wire bytes per lane between SS and SE
    localparam integer WB_PER_LANE = (LANE_COUNT == 1) ? 48 : 24;
    localparam integer INS_CYCLES  = (WB_PER_LANE + 2) / 2;   // incl. SS/SE

    // ------------------------------------------------------------------
    // GF(16) helpers: multiply by a^4 (g1) and a^1 (g0), LFSR step
    // ------------------------------------------------------------------
    function [3:0] g1m(input [3:0] c);
        g1m = {c[3]^c[2], c[2]^c[1], c[3]^c[1]^c[0], c[3]^c[0]};
    endfunction
    function [3:0] g0m(input [3:0] c);
        g0m = {c[2], c[1], c[3]^c[0], c[3]};
    endfunction

    // one LFSR step: state {x1,x0}, data nibble n
    function [7:0] lstep(input [7:0] x1x0, input [3:0] n);
        reg [3:0] fb;
        begin
            fb    = n ^ x1x0[7:4];
            lstep = {x1x0[3:0] ^ g1m(fb), g0m(fb)};   // {x1', x0'}
        end
    endfunction

    // parity byte for one header byte: feed lo nibble then hi nibble
    function [7:0] pb_byte(input [7:0] b);
        reg [7:0] st;
        begin
            st = lstep(8'h00, b[3:0]);
            st = lstep(st,    b[7:4]);
            pb_byte = {st[3:0], st[7:4]};             // {x0, x1}
        end
    endfunction

    // parity byte for a 4-byte payload group (byte 0 transmitted first)
    function [7:0] pb_group(input [31:0] g);
        reg [7:0] st;
        reg [31:0] t;
        integer i;
        begin
            st = 8'h00;
            t  = g;
            for (i = 0; i < 4; i = i + 1) begin
                st = lstep(st, t[3:0]);               // low nibble
                st = lstep(st, t[7:4]);               // high nibble
                t  = t >> 8;
            end
            pb_group = {st[3:0], st[7:4]};
        end
    endfunction

    // ------------------------------------------------------------------
    // Packet payload registers
    // ------------------------------------------------------------------
    // flattened vectors (Yosys cannot index reg arrays inside functions):
    // hbv = {HB3,HB2,HB1,HB0}, pbhv = matching parity, pbdv = P7..P0
    reg [31:0]  hbv;
    reg [255:0] db;                       // DB0 in [7:0] .. DB31 in [255:248]

    // Parity is a deep XOR tree (8 chained LFSR steps per group), so it
    // is REGISTERED: hbv/db are stable from the load cycle onward, and
    // insertion is delayed one cycle (pkt_armed) so the registered
    // parity is always settled before the first wire byte is read.
    wire [31:0] pbhv_c;
    wire [63:0] pbdv_c;
    assign pbhv_c = {pb_byte(hbv[31:24]), pb_byte(hbv[23:16]),
                     pb_byte(hbv[15:8]),  pb_byte(hbv[7:0])};
    genvar gi;
    generate
        for (gi = 0; gi < 8; gi = gi + 1) begin : g_pbd
            assign pbdv_c[8*gi +: 8] = pb_group(db[32*gi +: 32]);
        end
    endgenerate
    reg [31:0] pbhv;
    reg [63:0] pbdv;
    always @(posedge clk) begin
        pbhv <= pbhv_c;
        pbdv <= pbdv_c;
    end

    // ------------------------------------------------------------------
    // Pre-interleave lane sequences and wire-byte lookup
    //   2-lane: lane0 = HB0 PBh0 HB2 PBh2 [DBg0 P0][DBg2 P2][DBg4 P4][DBg6 P6]
    //           lane1 = HB1 PBh1 HB3 PBh3 [DBg1 P1][DBg3 P3][DBg5 P5][DBg7 P7]
    //           wire: lanes swap high nibbles byte-for-byte
    //   1-lane: consecutive codewords interleave (Fig 2-27/2-29)
    // ------------------------------------------------------------------
    function [7:0] pre_byte(input lane, input [5:0] k,
                            input [31:0] fhb, input [31:0] fpbh,
                            input [63:0] fpbd, input [255:0] fdb);
        begin
            case (k)
                6'd0: pre_byte = lane ? fhb[15:8] : fhb[7:0];
                6'd1: pre_byte = lane ? fpbh[15:8] : fpbh[7:0];
                6'd2: pre_byte = lane ? fhb[31:24] : fhb[23:16];
                6'd3: pre_byte = lane ? fpbh[31:24] : fpbh[23:16];
                6'd4: pre_byte = lane ? fdb[39:32] : fdb[7:0];
                6'd5: pre_byte = lane ? fdb[47:40] : fdb[15:8];
                6'd6: pre_byte = lane ? fdb[55:48] : fdb[23:16];
                6'd7: pre_byte = lane ? fdb[63:56] : fdb[31:24];
                6'd8: pre_byte = lane ? fpbd[15:8] : fpbd[7:0];
                6'd9: pre_byte = lane ? fdb[103:96] : fdb[71:64];
                6'd10: pre_byte = lane ? fdb[111:104] : fdb[79:72];
                6'd11: pre_byte = lane ? fdb[119:112] : fdb[87:80];
                6'd12: pre_byte = lane ? fdb[127:120] : fdb[95:88];
                6'd13: pre_byte = lane ? fpbd[31:24] : fpbd[23:16];
                6'd14: pre_byte = lane ? fdb[167:160] : fdb[135:128];
                6'd15: pre_byte = lane ? fdb[175:168] : fdb[143:136];
                6'd16: pre_byte = lane ? fdb[183:176] : fdb[151:144];
                6'd17: pre_byte = lane ? fdb[191:184] : fdb[159:152];
                6'd18: pre_byte = lane ? fpbd[47:40] : fpbd[39:32];
                6'd19: pre_byte = lane ? fdb[231:224] : fdb[199:192];
                6'd20: pre_byte = lane ? fdb[239:232] : fdb[207:200];
                6'd21: pre_byte = lane ? fdb[247:240] : fdb[215:208];
                6'd22: pre_byte = lane ? fdb[255:248] : fdb[223:216];
                6'd23: pre_byte = lane ? fpbd[63:56] : fpbd[55:48];
                default: pre_byte = 8'h00;
            endcase
        end
    endfunction

    // wire byte for lane l, position k (0..WB_PER_LANE-1)
    function [7:0] wire_byte(input [1:0] l, input [5:0] k,
                             input [31:0] fhb, input [31:0] fpbh,
                             input [63:0] fpbd, input [255:0] fdb);
        reg [7:0] a, b;
        begin
            if (LANE_COUNT == 2) begin
                a = pre_byte(l[0], k, fhb, fpbh, fpbd, fdb);
                b = pre_byte(~l[0], k, fhb, fpbh, fpbd, fdb);
                wire_byte = {b[7:4], a[3:0]};
            end else begin
                case (k)
                    6'd0: wire_byte = {fhb[11:8], fhb[3:0]};
                    6'd1: wire_byte = {fpbh[11:8], fpbh[3:0]};
                    6'd2: wire_byte = {fhb[7:4], fhb[15:12]};
                    6'd3: wire_byte = {fpbh[7:4], fpbh[15:12]};
                    6'd4: wire_byte = {fhb[27:24], fhb[19:16]};
                    6'd5: wire_byte = {fpbh[27:24], fpbh[19:16]};
                    6'd6: wire_byte = {fhb[23:20], fhb[31:28]};
                    6'd7: wire_byte = {fpbh[23:20], fpbh[31:28]};
                    6'd8: wire_byte = {fdb[35:32], fdb[3:0]};
                    6'd9: wire_byte = {fdb[43:40], fdb[11:8]};
                    6'd10: wire_byte = {fdb[51:48], fdb[19:16]};
                    6'd11: wire_byte = {fdb[59:56], fdb[27:24]};
                    6'd12: wire_byte = {fpbd[11:8], fpbd[3:0]};
                    6'd13: wire_byte = {fdb[7:4], fdb[39:36]};
                    6'd14: wire_byte = {fdb[15:12], fdb[47:44]};
                    6'd15: wire_byte = {fdb[23:20], fdb[55:52]};
                    6'd16: wire_byte = {fdb[31:28], fdb[63:60]};
                    6'd17: wire_byte = {fpbd[7:4], fpbd[15:12]};
                    6'd18: wire_byte = {fdb[99:96], fdb[67:64]};
                    6'd19: wire_byte = {fdb[107:104], fdb[75:72]};
                    6'd20: wire_byte = {fdb[115:112], fdb[83:80]};
                    6'd21: wire_byte = {fdb[123:120], fdb[91:88]};
                    6'd22: wire_byte = {fpbd[27:24], fpbd[19:16]};
                    6'd23: wire_byte = {fdb[71:68], fdb[103:100]};
                    6'd24: wire_byte = {fdb[79:76], fdb[111:108]};
                    6'd25: wire_byte = {fdb[87:84], fdb[119:116]};
                    6'd26: wire_byte = {fdb[95:92], fdb[127:124]};
                    6'd27: wire_byte = {fpbd[23:20], fpbd[31:28]};
                    6'd28: wire_byte = {fdb[163:160], fdb[131:128]};
                    6'd29: wire_byte = {fdb[171:168], fdb[139:136]};
                    6'd30: wire_byte = {fdb[179:176], fdb[147:144]};
                    6'd31: wire_byte = {fdb[187:184], fdb[155:152]};
                    6'd32: wire_byte = {fpbd[43:40], fpbd[35:32]};
                    6'd33: wire_byte = {fdb[135:132], fdb[167:164]};
                    6'd34: wire_byte = {fdb[143:140], fdb[175:172]};
                    6'd35: wire_byte = {fdb[151:148], fdb[183:180]};
                    6'd36: wire_byte = {fdb[159:156], fdb[191:188]};
                    6'd37: wire_byte = {fpbd[39:36], fpbd[47:44]};
                    6'd38: wire_byte = {fdb[227:224], fdb[195:192]};
                    6'd39: wire_byte = {fdb[235:232], fdb[203:200]};
                    6'd40: wire_byte = {fdb[243:240], fdb[211:208]};
                    6'd41: wire_byte = {fdb[251:248], fdb[219:216]};
                    6'd42: wire_byte = {fpbd[59:56], fpbd[51:48]};
                    6'd43: wire_byte = {fdb[199:196], fdb[231:228]};
                    6'd44: wire_byte = {fdb[207:204], fdb[239:236]};
                    6'd45: wire_byte = {fdb[215:212], fdb[247:244]};
                    6'd46: wire_byte = {fdb[223:220], fdb[255:252]};
                    6'd47: wire_byte = {fpbd[55:52], fpbd[63:60]};
                    default: wire_byte = 8'h00;
                endcase
            end
        end
    endfunction

    // ------------------------------------------------------------------
    // Channel status (IEC 60958-3 consumer LPCM) and subframe helpers
    // ------------------------------------------------------------------
    // bit 2: copyright not asserted; bits 24-27: sampling frequency
    // (48k=0100 in transmission order -> [27:24]=0010); bits 32-35: word
    // length (16-bit in 20-bit field). Values follow the hdl-util HDMI
    // module's proven defaults.
    wire [191:0] chan_status;
    assign chan_status[23:0]   = {8'h00, 8'h00, 8'b00000100};
    assign chan_status[27:24]  = (AUDIO_RATE == 44100) ? 4'b0000 :
                                 (AUDIO_RATE == 32000) ? 4'b0011 : 4'b0010;
    assign chan_status[31:28]  = 4'b0000;
    assign chan_status[35:32]  = 4'b0010;      // 16-bit samples
    assign chan_status[191:36] = 0;

    reg [7:0] iec_cnt;                         // IEC frame counter 0..191

    // subframe byte 3: {SP,0,PR[1:0],P,C,U,V}
    function [7:0] sf_b3(input [15:0] smp, input [1:0] pr, input c);
        reg p;
        begin
            p     = (^smp) ^ c;                // even parity over data+V+U+C (V=U=0)
            sf_b3 = {1'b1, 1'b0, pr, p, c, 1'b0, 1'b0};
        end
    endfunction

    // ------------------------------------------------------------------
    // Scheduler and packet load
    // ------------------------------------------------------------------
    reg        ts_due, info_due;
    reg        ts_sent_once, stream_sent_once;
    reg        pkt_pending;
    reg        pkt_armed;                      // parity registers settled
    reg [5:0]  ins_cyc;                        // 0 = idle
    reg        inserting;

    integer si;
    reg [15:0] sl, sr;
    reg        csb;
    reg [1:0]  pr_l;

    always @(posedge clk) begin
        if (reset) begin
            ts_due           <= 1'b0;
            info_due         <= 1'b0;
            ts_sent_once     <= 1'b0;
            stream_sent_once <= 1'b0;
            pkt_pending      <= 1'b0;
            pkt_armed        <= 1'b0;
            inserting        <= 1'b0;
            ins_cyc          <= 6'd0;
            iec_cnt          <= 8'd0;
            buffer_take      <= 1'b0;
            audio_mute       <= 1'b1;
            out_data         <= 73'b0;
        end else begin
            buffer_take <= 1'b0;
            pkt_armed   <= pkt_pending;

            if (frame_pulse) begin
                ts_due   <= 1'b1;
                info_due <= 1'b1;
            end

            audio_mute <= ~(ts_sent_once & stream_sent_once);

            // ----------------------------------------------------------
            // Load the next packet when idle
            // ----------------------------------------------------------
            if (!pkt_pending && !inserting) begin
                if (ts_due) begin
                    hbv <= {8'h44, 8'h17, 8'h01, 8'h00};
                    for (si = 0; si < 4; si = si + 1) begin
                        db[32*si     +: 32] <= {8'h00, maud[7:0], maud[15:8], maud[23:16]};
                        db[128+32*si +: 32] <= {8'h00, NAUD[7:0], NAUD[15:8], NAUD[23:16]};
                    end
                    pkt_pending <= 1'b1;
                    ts_due      <= 1'b0;
                end else if (buffer_ready) begin
                    hbv <= {8'h01, 8'h00, 8'h02, 8'h00};
                    for (si = 0; si < 4; si = si + 1) begin
                        sl  = buffer[si*2*AUDIO_BIT_WIDTH +: AUDIO_BIT_WIDTH];
                        sr  = buffer[si*2*AUDIO_BIT_WIDTH + AUDIO_BIT_WIDTH +: AUDIO_BIT_WIDTH];
                        csb = chan_status[iec_cnt + si[7:0]];
                        pr_l = ((iec_cnt + si[7:0]) == 8'd0) ? 2'b00 : 2'b01;
                        // DB(8i..8i+3) = ch1 subframe, DB(8i+4..8i+7) = ch2
                        db[64*si    +: 32] <= {sf_b3(sl, pr_l,  csb), sl[15:8], sl[7:0], 8'h00};
                        db[64*si+32 +: 32] <= {sf_b3(sr, 2'b10, csb), sr[15:8], sr[7:0], 8'h00};
                    end
                    iec_cnt     <= (iec_cnt >= 8'd188) ? 8'd0 : iec_cnt + 8'd4;
                    buffer_take <= 1'b1;
                    pkt_pending <= 1'b1;
                end else if (info_due) begin
                    hbv <= {8'h44, 8'h1B, 8'h84, 8'h00};
                    db          <= 256'h0;
                    db[7:0]     <= 8'h01;          // CT=0 (refer to header), CC=1 (2ch)
                    pkt_pending <= 1'b1;
                    info_due    <= 1'b0;
                end
            end

            // ----------------------------------------------------------
            // Insertion: SS, wire bytes, SE overlaid on the dummy region
            // ----------------------------------------------------------
            out_data <= in_data;                    // default passthrough

            if (!inserting) begin
                if (sdp_gap && pkt_pending && pkt_armed) begin
                    inserting <= 1'b1;
                    ins_cyc   <= 6'd1;
                    // slot0 = SS, slot1 = wire byte 0
                    out_data[17:0] <= {1'b0, wire_byte(2'd0, 6'd0, hbv, pbhv, pbdv, db), SS};
                    if (LANE_COUNT == 2)
                        out_data[35:18] <= {1'b0, wire_byte(2'd1, 6'd0, hbv, pbhv, pbdv, db), SS};
                end
            end else begin
                if (ins_cyc == INS_CYCLES[5:0]-1) begin
                    // last cycle: slot0 = final wire byte, slot1 = SE
                    out_data[17:0] <= {SE, 1'b0, wire_byte(2'd0, WB_PER_LANE[5:0]-1, hbv, pbhv, pbdv, db)};
                    if (LANE_COUNT == 2)
                        out_data[35:18] <= {SE, 1'b0, wire_byte(2'd1, WB_PER_LANE[5:0]-1, hbv, pbhv, pbdv, db)};
                    inserting   <= 1'b0;
                    ins_cyc     <= 6'd0;
                    pkt_pending <= 1'b0;
                    if (hbv[15:8] == 8'h01) ts_sent_once     <= 1'b1;
                    if (hbv[15:8] == 8'h02) stream_sent_once <= 1'b1;
                end else begin
                    // slot0 = wire byte 2c-1, slot1 = wire byte 2c
                    out_data[17:0] <= {1'b0, wire_byte(2'd0, {ins_cyc[4:0],1'b0}, hbv, pbhv, pbdv, db),
                                       1'b0, wire_byte(2'd0, {ins_cyc[4:0],1'b0}-6'd1, hbv, pbhv, pbdv, db)};
                    if (LANE_COUNT == 2)
                        out_data[35:18] <= {1'b0, wire_byte(2'd1, {ins_cyc[4:0],1'b0}, hbv, pbhv, pbdv, db),
                                            1'b0, wire_byte(2'd1, {ins_cyc[4:0],1'b0}-6'd1, hbv, pbhv, pbdv, db)};
                    ins_cyc <= ins_cyc + 1'b1;
                end
            end

            // a packet may legally overrun the gap into the next line's
            // leading dummy TU; suppress the switch-point flag there so the
            // idle inserter never swaps streams mid-packet
            if (inserting)
                out_data[72] <= 1'b0;
        end
    end

endmodule
