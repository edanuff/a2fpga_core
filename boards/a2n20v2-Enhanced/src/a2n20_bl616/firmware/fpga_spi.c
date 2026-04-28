/*
 * FPGA SPI — Hardware SPI0 master with hardware-driven CS# for BL616→FPGA
 * communication per BL616_SPI_PROTOCOL.md.
 *
 * Pin mapping (SPI0 alternate function on GPIO0-3):
 *   GPIO0 = CS#  (SPI0_SS, hardware-driven by peripheral)
 *   GPIO1 = SCLK (SPI0_CLK)
 *   GPIO2 = MISO (SPI0_MISO, FPGA→MCU)
 *   GPIO3 = MOSI (SPI0_MOSI, MCU→FPGA)
 *
 * The peripheral runs in continuous-CS mode (SPI_CMD_SET_CS_INTERVAL=1) so
 * that SS stays asserted for the full duration of a single poll_exchange call.
 * Each protocol-level transaction (reg op, XFER op) is therefore packed into
 * exactly one poll_exchange so that CS# frames the whole burst — no inter-call
 * deassert, no software/hardware race between CS# and SCLK edges.
 */

#include <string.h>
#include <stdio.h>
#include "bflb_gpio.h"
#include "bflb_spi.h"
#include "bflb_mtimer.h"
#include "fpga_spi.h"
#include "fpga_screen.h"

#define SPI_CS_PIN   GPIO_PIN_0
#define SPI_CLK_PIN  GPIO_PIN_1
#define SPI_MISO_PIN GPIO_PIN_2
#define SPI_MOSI_PIN GPIO_PIN_3

/* Max payload per XFER chunk. Sized for one FAT sector (512B) plus the 8-byte
 * header+dummy prefix used by reads. Each chunk is one poll_exchange call. */
#define XFER_CHUNK_MAX 512
#define XFER_HDR_LEN   7   /* opcode + sub0 + addr[3] + len[2] */
#define XFER_DUMMY_LEN 1

static struct bflb_device_s *spi0;

/* TX scratch holds [header || (data | dummy+filler)] for one chunk. */
static uint8_t spi_tx_scratch[XFER_HDR_LEN + XFER_DUMMY_LEN + XFER_CHUNK_MAX];
/* RX scratch is only used for reads; the header bytes return status, the
 * dummy byte is discarded, then chunk_len data bytes follow. */
static uint8_t spi_rx_scratch[XFER_HDR_LEN + XFER_DUMMY_LEN + XFER_CHUNK_MAX];

void fpga_spi_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    /* All four pins as SPI0 alternate function. CS# is now driven by the
     * peripheral in lockstep with SCLK — no GPIO bitbang. */
    bflb_gpio_init(gpio, SPI_CS_PIN,   GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_DRV_1);
    bflb_gpio_init(gpio, SPI_CLK_PIN,  GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_DRV_1);
    bflb_gpio_init(gpio, SPI_MISO_PIN, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_DRV_1);
    bflb_gpio_init(gpio, SPI_MOSI_PIN, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_DRV_1);

    spi0 = bflb_device_get_by_name("spi0");

    struct bflb_spi_config_s spi_cfg = {
        .freq = 4 * 1000 * 1000,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE0,              /* CPOL=0 CPHA=0 per protocol spec */
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
    bflb_spi_init(spi0, &spi_cfg);

    /* Continuous CS: SS stays asserted across all bytes of one poll_exchange,
     * regardless of momentary TX FIFO underruns the polling driver might allow. */
    bflb_spi_feature_control(spi0, SPI_CMD_SET_CS_INTERVAL, 1);
}

uint8_t fpga_spi_reg_read(uint8_t reg)
{
    uint8_t tx[2] = {0x80 | (reg & 0x7F), 0xFF};
    uint8_t rx[2];
    bflb_spi_poll_exchange(spi0, tx, rx, 2);
    return rx[1];
}

void fpga_spi_reg_write(uint8_t reg, uint8_t val)
{
    uint8_t tx[2] = {reg & 0x7F, val};
    bflb_spi_poll_exchange(spi0, tx, NULL, 2);
}

/* Build the 7-byte XFER header at the start of spi_tx_scratch.
 * SUB0 format: { 0, RES=0, CRC_EN=0, INC=1, SPACE[2:0], DIR } */
static inline void build_xfer_header(uint8_t space, uint32_t addr, uint16_t chunk, uint8_t dir)
{
    spi_tx_scratch[0] = 0x7F;                                          /* XFER opcode */
    spi_tx_scratch[1] = (1 << 4) | ((space & 0x07) << 1) | (dir & 1); /* SUB0: INC=1 */
    spi_tx_scratch[2] = (uint8_t)(addr & 0xFF);
    spi_tx_scratch[3] = (uint8_t)((addr >> 8) & 0xFF);
    spi_tx_scratch[4] = (uint8_t)((addr >> 16) & 0xFF);
    spi_tx_scratch[5] = (uint8_t)(chunk & 0xFF);
    spi_tx_scratch[6] = (uint8_t)((chunk >> 8) & 0xFF);
}

void fpga_spi_xfer_write(uint8_t space, uint32_t addr, const uint8_t *data, uint16_t len)
{
    uint16_t remaining = len;
    uint32_t cur_addr = addr;
    const uint8_t *src = data;

    while (remaining > 0) {
        uint16_t chunk = (remaining > XFER_CHUNK_MAX) ? XFER_CHUNK_MAX : remaining;
        build_xfer_header(space, cur_addr, chunk, 0);  /* DIR=0 (write) */
        memcpy(&spi_tx_scratch[XFER_HDR_LEN], src, chunk);
        bflb_spi_poll_exchange(spi0, spi_tx_scratch, NULL, XFER_HDR_LEN + chunk);
        cur_addr += chunk;
        src      += chunk;
        remaining -= chunk;
    }
}

void fpga_spi_xfer_read(uint8_t space, uint32_t addr, uint8_t *data, uint16_t len)
{
    uint16_t remaining = len;
    uint32_t cur_addr = addr;
    uint8_t *dst = data;

    while (remaining > 0) {
        uint16_t chunk = (remaining > XFER_CHUNK_MAX) ? XFER_CHUNK_MAX : remaining;
        build_xfer_header(space, cur_addr, chunk, 1);  /* DIR=1 (read) */
        spi_tx_scratch[XFER_HDR_LEN] = 0xFF;            /* dummy byte */
        memset(&spi_tx_scratch[XFER_HDR_LEN + XFER_DUMMY_LEN], 0xFF, chunk);
        bflb_spi_poll_exchange(spi0, spi_tx_scratch, spi_rx_scratch,
                               XFER_HDR_LEN + XFER_DUMMY_LEN + chunk);
        memcpy(dst, &spi_rx_scratch[XFER_HDR_LEN + XFER_DUMMY_LEN], chunk);
        cur_addr += chunk;
        dst      += chunk;
        remaining -= chunk;
    }
}

void fpga_spi_xfer_fill(uint8_t space, uint32_t addr, uint8_t val, uint16_t len)
{
    uint16_t remaining = len;
    uint32_t cur_addr = addr;

    while (remaining > 0) {
        uint16_t chunk = (remaining > XFER_CHUNK_MAX) ? XFER_CHUNK_MAX : remaining;
        build_xfer_header(space, cur_addr, chunk, 0);  /* DIR=0 (write) */
        memset(&spi_tx_scratch[XFER_HDR_LEN], val, chunk);
        bflb_spi_poll_exchange(spi0, spi_tx_scratch, NULL, XFER_HDR_LEN + chunk);
        cur_addr  += chunk;
        remaining -= chunk;
    }
}

uint8_t fpga_spi_read_status(void)
{
    return fpga_spi_reg_read(0x06);
}

bool fpga_spi_wait_ready(uint32_t timeout_ms)
{
    uint64_t deadline = bflb_mtimer_get_time_ms() + timeout_ms;

    while (bflb_mtimer_get_time_ms() < deadline) {
        uint8_t status = fpga_spi_read_status();
        if ((status & FPGA_STATUS_FPGA_CONFIGURED) &&
            (status & FPGA_STATUS_SDRAM_READY)) {
            return true;
        }
        bflb_mtimer_delay_ms(10);
    }
    return false;
}

void fpga_spi_read_device_id(uint8_t *buf)
{
    for (int i = 0; i < 4; i++) {
        buf[i] = fpga_spi_reg_read(i);
    }
}

void fpga_service_init(void)
{
    printf("FPGA SPI: waiting for FPGA ready...\r\n");

    if (!fpga_spi_wait_ready(5000)) {
        printf("FPGA SPI: timeout waiting for FPGA\r\n");
        return;
    }

    /* Read and verify device ID */
    uint8_t id[4];
    fpga_spi_read_device_id(id);
    printf("FPGA SPI: Device ID = %c%c%c%c\r\n", id[0], id[1], id[2], id[3]);

    uint8_t status = fpga_spi_read_status();
    printf("FPGA SPI: STATUS = 0x%02X\r\n", status);

    /* Enable video, text mode, bus ready */
    fpga_spi_reg_write(0x10, 1);  /* VIDEO_ENABLE */
    fpga_spi_reg_write(0x11, 1);  /* TEXT_MODE */
    fpga_spi_reg_write(0x30, 1);  /* A2BUS_READY */

    /* Show hello message on Apple II display */
    fpga_screen_clear();
    fpga_screen_puts("Hello from the MCU!");

    /* Hold message for 3 seconds */
    bflb_mtimer_delay_ms(3000);

    /* Disable video overlay, release card ROM */
    fpga_spi_reg_write(0x10, 0);  /* VIDEO_ENABLE=0 */
    fpga_spi_reg_write(0x31, 1);  /* CARDROM_RELEASE */

    printf("FPGA SPI: init complete\r\n");
}
