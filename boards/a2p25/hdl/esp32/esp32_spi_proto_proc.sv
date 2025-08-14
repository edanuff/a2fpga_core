// ============================================================================
// Child: esp32_spi_proto_proc
//  - 3-wire SPI, no CS, Mode 0, full duplex
//  - Opcodes: [7]=R/W, [6:0]=reg (0..126 fixed-size 1B here; 127=XFER portal)
//  - XFER subheader: SUB0, ADDR[23:0] LE, LEN[15:0] LE
//  - Synchronous memory READ supported via mem_rd_valid latency handshake
//  - Deterministic one dummy byte before first READ payload byte
// ============================================================================
module esp32_spi_proto_proc #(
  parameter bit USE_SYNC    = 1,
  parameter bit USE_CRC     = 0,          // CRC-8 poly 0x07 (header/payload when flagged)
  parameter int IDLE_TO_CYC = 54_000      // ~1ms @ 54MHz
)(
  input  logic clk,
  input  logic rst_n,

  // 3-wire SPI (no CS), Mode 0
  input  logic sclk,
  input  logic mosi,
  output logic miso,

  // Register file (parent implements 0..15)
  output logic        reg_wr_req,         // pulse
  output logic [6:0]  reg_idx,
  output logic [7:0]  reg_wdata,
  input  logic [7:0]  reg_rdata,          // combinational OK (discrete flops)

  // Memory/XFER interface (SPACE selectable; parent implements SPACE=0)
  // WRITE path (byte-per-byte)
  output logic        mem_wr_en,          // pulse
  output logic [2:0]  mem_space,
  output logic [23:0] mem_wr_addr,
  output logic [7:0]  mem_wr_data,

  // READ path with synchronous latency
  output logic        mem_rd_req,         // pulse to request byte at mem_rd_addr
  output logic [2:0]  mem_rd_space,
  output logic [23:0] mem_rd_addr,
  input  logic        mem_rd_valid,       // asserted when mem_rd_data is valid
  input  logic [7:0]  mem_rd_data         // data for the previously requested addr
);

  // ---- Synchronize SPI into clk domain ----
  logic sclk_q1, sclk_q2, mosi_q1, mosi_q2;
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin sclk_q1<=0; sclk_q2<=0; mosi_q1<=0; mosi_q2<=0; end
    else begin sclk_q1<=sclk; sclk_q2<=sclk_q1; mosi_q1<=mosi; mosi_q2<=mosi_q1; end
  end
  wire sclk_rise = (sclk_q2==0)&&(sclk_q1==1);
  wire sclk_fall = (sclk_q2==1)&&(sclk_q1==0);

  // Idle reframing (no CS)
  logic [$clog2(IDLE_TO_CYC+1)-1:0] idle_ctr;
  wire idle_expired = (idle_ctr==IDLE_TO_CYC);
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) idle_ctr <= '0;
    else if (sclk_rise || sclk_fall) idle_ctr <= '0;
    else if (idle_ctr != IDLE_TO_CYC) idle_ctr <= idle_ctr + 1'b1;
  end

  // ---- Bit/byte shifters (Mode 0) ----
  logic [2:0] bit_cnt;
  logic [7:0] rx_shift, tx_shift, next_tx_byte;
  logic       byte_rx_stb, byte_tx_ld;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin bit_cnt<=0; rx_shift<=8'h00; byte_rx_stb<=0; end
    else begin
      byte_rx_stb <= 0;
      if (sclk_rise) begin
        rx_shift <= {rx_shift[6:0], mosi_q2};
        bit_cnt  <= bit_cnt + 3'd1;
        if (bit_cnt==3'd7) begin bit_cnt<=0; byte_rx_stb<=1; end
      end
      if (idle_expired) bit_cnt<=0;
    end
  end

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) tx_shift <= 8'hFF;
    else if (sclk_fall)  tx_shift <= {tx_shift[6:0], 1'b1}; // fill with '1's when idle
  end
  assign miso = tx_shift[7];
  assign byte_tx_ld = sclk_fall && (bit_cnt==3'd0);

  // ---- CRC-8 (0x07) ----
  function automatic [7:0] crc8_update(input [7:0] crc, input [7:0] data);
    integer i; reg [7:0] c,d; begin
      c=crc; d=data;
      for (i=0;i<8;i++) begin
        c = (c[7]^d[7]) ? ({c[6:0],1'b0} ^ 8'h07) : {c[6:0],1'b0};
        d = {d[6:0],1'b0};
      end
      return c;
    end
  endfunction

  // ---- STATUS byte ----
  localparam [7:0] PROTO_VER = 8'h01;
  typedef struct packed {logic[3:0] ver; logic align, crcerr, busy, ok;} status_t;
  status_t status;

  // ---- FSM ----
  typedef enum logic [4:0] {
    ST_IDLE, ST_SYNC0, ST_SYNC1, ST_OPCODE, ST_HDRCRC,
    ST_REG_RW,
    ST_X0, ST_XA0, ST_XA1, ST_XA2, ST_XL0, ST_XL1, ST_XHDRC,
    ST_XPAY_WR,            // write payload
    ST_XPAY_RD_DMY,        // deterministic 1 dummy byte before first RD data
    ST_XPAY_RD,            // steady-state read with pipeline
    ST_XPLCRC, ST_DONE, ST_ERR
  } st_e;
  st_e st;

  // Header fields
  logic       op_is_read; logic [6:0] op_reg;
  logic [7:0] sub0;       logic [23:0] addr; logic [15:0] len, len_cnt;
  logic       sub_dir, sub_crc, sub_inc; logic [2:0] sub_space;
  logic [7:0] crc_hdr, crc_pl;

  // TX source select
  typedef enum logic [1:0] {TX_STAT, TX_REG, TX_DATA, TX_IDLE} tx_mode_e;
  tx_mode_e tx_mode;

  // READ pipeline buffer
  logic [7:0] rd_buf;
  logic       rd_buf_valid;

  // Defaults / sequential logic
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      st <= ST_IDLE;
      status <= '{ver:PROTO_VER[3:0], align:0, crcerr:0, busy:0, ok:1};
      op_is_read <= 0; op_reg <= '0;
      sub0 <= '0; addr <= '0; len <= '0; len_cnt <= '0;
      sub_dir<=0; sub_crc<=0; sub_inc<=0; sub_space<=3'd0;
      crc_hdr<=0; crc_pl<=0;
      tx_mode <= TX_STAT;
      next_tx_byte <= 8'hFF; tx_shift <= 8'hFF;

      reg_wr_req <= 0; reg_idx <= '0; reg_wdata <= '0;

      mem_wr_en <= 0; mem_space<=3'd0; mem_wr_addr<='0; mem_wr_data<='0;
      mem_rd_req<= 0; mem_rd_space<=3'd0; mem_rd_addr<='0;
      rd_buf <= 8'h00; rd_buf_valid <= 1'b0;
    end else begin
      // one-shot outputs
      reg_wr_req  <= 0;
      mem_wr_en   <= 0;
      mem_rd_req  <= 0;
      status.align<= 0;

      // load next TX byte every byte boundary
      if (byte_tx_ld) begin
        unique case (tx_mode)
          TX_STAT: next_tx_byte <= {status.ver, status.align, status.crcerr, status.busy, status.ok};
          TX_REG : next_tx_byte <= reg_rdata;
          TX_DATA: next_tx_byte <= rd_buf_valid ? rd_buf : 8'hFF; // if late, still safe
          default: next_tx_byte <= 8'hFF;
        endcase
        tx_shift <= next_tx_byte;
      end

      // latch synchronous read data when valid
      if (mem_rd_valid) begin
        rd_buf       <= mem_rd_data;
        rd_buf_valid <= 1'b1;
      end

      // state machine (byte granularity)
      if (idle_expired) begin
        st <= ST_IDLE; tx_mode <= TX_STAT; rd_buf_valid<=1'b0;
      end else if (byte_rx_stb) begin
        unique case (st)
          ST_IDLE: begin
            status.ok<=1; status.crcerr<=0; status.busy<=0; tx_mode<=TX_STAT;
            crc_hdr<=0; rd_buf_valid<=0;
            st <= (USE_SYNC ? ST_SYNC0 : ST_OPCODE);
          end
          ST_SYNC0: begin st <= (rx_shift==8'hA5) ? ST_SYNC1 : ST_IDLE; end
          ST_SYNC1: begin
            if (rx_shift==8'h5A) begin status.align<=1; st<=ST_OPCODE; crc_hdr<=0; end
            else st<=ST_IDLE;
          end

          ST_OPCODE: begin
            op_is_read <= rx_shift[7];
            op_reg     <= rx_shift[6:0];
            if (USE_CRC) crc_hdr <= crc8_update(crc_hdr, rx_shift);
            tx_mode <= TX_STAT;
            rd_buf_valid <= 0;

            if (rx_shift[6:0] != 7'd127) begin
              // fixed-size reg (1B in this bring-up)
              reg_idx <= rx_shift[6:0];
              if (rx_shift[7]) begin
                tx_mode <= TX_REG;                    // source is reg_rdata
                st      <= (USE_CRC ? ST_HDRCRC : ST_REG_RW);
              end else begin
                st      <= (USE_CRC ? ST_HDRCRC : ST_REG_RW);
              end
            end else begin
              st <= ST_X0;
            end
          end

          ST_HDRCRC: begin
            if (USE_CRC) begin
              if (rx_shift != crc_hdr) begin status.ok<=0; status.crcerr<=1; st<=ST_ERR; end
              else st <= (op_reg!=7'd127) ? ST_REG_RW : ST_X0;
            end else st <= (op_reg!=7'd127) ? ST_REG_RW : ST_X0;
          end

          ST_REG_RW: begin
            if (op_reg!=7'd127) begin
              if (op_is_read) st <= ST_DONE;            // 1 byte already sourced
              else begin
                reg_idx   <= op_reg;
                reg_wdata <= rx_shift;
                reg_wr_req<= 1;
                st <= ST_DONE;
              end
            end else st <= ST_ERR;
          end

          // ---- XFER header ----
          ST_X0:   begin sub0<=rx_shift; if (USE_CRC) crc_hdr<=crc8_update(crc_hdr,rx_shift);
                            sub_dir<=rx_shift[0]; sub_space<=rx_shift[3:1];
                            sub_inc<=rx_shift[4]; sub_crc<=rx_shift[5];
                            addr<='0; len<='0; tx_mode<=TX_STAT; st<=ST_XA0; end
          ST_XA0:  begin addr[7:0]  <= rx_shift; if (USE_CRC) crc_hdr<=crc8_update(crc_hdr,rx_shift); st<=ST_XA1; end
          ST_XA1:  begin addr[15:8] <= rx_shift; if (USE_CRC) crc_hdr<=crc8_update(crc_hdr,rx_shift); st<=ST_XA2; end
          ST_XA2:  begin addr[23:16]<= rx_shift; if (USE_CRC) crc_hdr<=crc8_update(crc_hdr,rx_shift); st<=ST_XL0; end
          ST_XL0:  begin len[7:0]   <= rx_shift; if (USE_CRC) crc_hdr<=crc8_update(crc_hdr,rx_shift); st<=ST_XL1; end
          ST_XL1:  begin
            len[15:8] <= rx_shift; if (USE_CRC) crc_hdr<=crc8_update(crc_hdr,rx_shift);
            len_cnt   <= {rx_shift, len[7:0]};
            crc_pl    <= 8'h00;
            mem_space    <= sub_space;
            mem_rd_space <= sub_space;
            st        <= (USE_CRC ? ST_XHDRC : (sub_dir ? ST_XPAY_RD_DMY : ST_XPAY_WR));
            tx_mode   <= sub_dir ? TX_DATA : TX_STAT;   // data source used in RD phases
          end
          ST_XHDRC: begin
            if (USE_CRC) begin
              if (rx_shift != crc_hdr) begin status.ok<=0; status.crcerr<=1; st<=ST_ERR; end
              else st <= (sub_dir ? ST_XPAY_RD_DMY : ST_XPAY_WR);
            end else st <= (sub_dir ? ST_XPAY_RD_DMY : ST_XPAY_WR);
          end

          // ---- XFER WRITE payload ----
          ST_XPAY_WR: begin
            if (len_cnt==0) begin
              st <= (USE_CRC && sub_crc) ? ST_XPLCRC : ST_DONE;
            end else begin
              if (mem_space==3'd0) begin
                mem_wr_addr <= addr;
                mem_wr_data <= rx_shift;
                mem_wr_en   <= 1;
              end
              len_cnt <= len_cnt - 16'd1;
              if (sub_inc) addr <= addr + 24'd1;
              if (USE_CRC) crc_pl <= crc8_update(crc_pl, rx_shift);
            end
          end

          // ---- XFER READ: deterministic 1 dummy byte, then steady stream ----
          ST_XPAY_RD_DMY: begin
            // Issue first read request; TX_DATA will output 0xFF because rd_buf_valid==0
            mem_rd_addr  <= addr;
            mem_rd_req   <= 1;
            // Do NOT decrement len yet; dummy is not counted toward LEN
            // Next byte, data should be valid (for BRAM/1-cycle latency). If not, TX_DATA
            // will still send 0xFF until mem_rd_valid sets rd_buf_valid; we still won’t
            // decrement len until we actually consume rd_buf.
            st <= ST_XPAY_RD;
          end

          ST_XPAY_RD: begin
            if (len_cnt==0) begin
              st <= (USE_CRC && sub_crc) ? ST_XPLCRC : ST_DONE;
            end else begin
              if (rd_buf_valid) begin
                // We are about to send rd_buf during this byte. Consume 1 from LEN.
                if (USE_CRC) crc_pl <= crc8_update(crc_pl, rd_buf);
                rd_buf_valid <= 1'b0;           // consumed
                len_cnt <= len_cnt - 16'd1;

                // Prefetch next if any
                if (len_cnt > 16'd1) begin
                  mem_rd_addr <= sub_inc ? (addr + 24'd1) : addr;
                  mem_rd_req  <= 1;
                  if (sub_inc) addr <= addr + 24'd1;
                end else begin
                  // last byte being sent now; no further request
                  if (sub_inc) addr <= addr; // no-op
                end
              end
              // If rd_buf not valid yet, we output 0xFF (TX_DATA path) but DO NOT decrement len.
              // This allows variable-latency memories safely; master clocks through until data appears.
            end
          end

          ST_XPLCRC: begin
            if (USE_CRC) begin
              if (rx_shift != crc_pl) begin status.ok<=0; status.crcerr<=1; st<=ST_ERR; end
              else st<=ST_DONE;
            end else st<=ST_DONE;
          end

          ST_DONE: begin st<=ST_IDLE; tx_mode<=TX_STAT; rd_buf_valid<=0; end
          ST_ERR : begin st<=ST_IDLE; tx_mode<=TX_STAT; rd_buf_valid<=0; end
        endcase
      end
    end
  end

endmodule