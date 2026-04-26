/*
 * FPGA SPI — Hardware SPI0 master with manual GPIO CS# for BL616→FPGA
 * communication per BL616_SPI_PROTOCOL.md.
 *
 * Uses BL616 hardware SPI0 for SCLK/MOSI/MISO clocking at 20 MHz,
 * with GPIO0 as a manual chip-select output for full framing control.
 */

#include <string.h>
#include <stdio.h>
#include "bflb_gpio.h"
#include "bflb_spi.h"
#include "bflb_mtimer.h"
#include "fpga_spi.h"
#include "fpga_screen.h"

/* Pin definitions */
#define SPI_CS_PIN   GPIO_PIN_0   /* Manual CS# output */
#define SPI_CLK_PIN  GPIO_PIN_1   /* SPI0 CLK */
#define SPI_MISO_PIN GPIO_PIN_2   /* SPI0 MISO */
#define SPI_MOSI_PIN GPIO_PIN_3   /* SPI0 MOSI */

/* CS# control via GPIO SET/CLEAR registers (same pattern as jtag_process.c) */
#define SPI_GPIO_SET  (*(volatile uint32_t *)0x20000AEC)
#define SPI_GPIO_CLR  (*(volatile uint32_t *)0x20000AF4)
#define CS_BIT        (1 << 0)  /* GPIO0 */

#define CS_LOW()   SPI_GPIO_CLR = CS_BIT
#define CS_HIGH()  SPI_GPIO_SET = CS_BIT

static struct bflb_device_s *spi0;

void fpga_spi_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");

    /* GPIO0: Manual CS# output, default high (deasserted) */
    bflb_gpio_init(gpio, SPI_CS_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    CS_HIGH();

    /* GPIO1/2/3: SPI0 function */
    bflb_gpio_init(gpio, SPI_CLK_PIN,  GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, SPI_MISO_PIN, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, SPI_MOSI_PIN, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

    /* Initialize SPI0 hardware */
    spi0 = bflb_device_get_by_name("spi0");

    struct bflb_spi_config_s spi_cfg = {
        .freq = 20 * 1000 * 1000,      /* 20 MHz */
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE0,              /* CPOL=0 CPHA=0 per protocol spec */
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
    bflb_spi_init(spi0, &spi_cfg);
}

/* Send one byte and return the received byte (full duplex) */
static inline uint8_t spi_xfer_byte(uint8_t tx)
{
    return (uint8_t)bflb_spi_poll_send(spi0, tx);
}

uint8_t fpga_spi_reg_read(uint8_t reg)
{
    uint8_t val;
    CS_LOW();
    spi_xfer_byte(0x80 | (reg & 0x7E));  /* opcode: rw=1, reg[6:0] */
    val = spi_xfer_byte(0xFF);           /* clock out data byte */
    CS_HIGH();
    return val;
}

void fpga_spi_reg_write(uint8_t reg, uint8_t val)
{
    CS_LOW();
    spi_xfer_byte(reg & 0x7E);   /* opcode: rw=0, reg[6:0] */
    spi_xfer_byte(val);          /* payload */
    CS_HIGH();
}

/*
 * Build XFER subheader and send it.
 * SUB0 format: { 0, RES=0, CRC_EN=0, INC=1, SPACE[2:0], DIR }
 */
static void xfer_send_header(uint8_t space, uint32_t addr, uint16_t len, uint8_t dir)
{
    uint8_t sub0 = (1 << 4) | ((space & 0x07) << 1) | (dir & 1);  /* INC=1 */

    spi_xfer_byte(0x7F);                     /* XFER opcode */
    spi_xfer_byte(sub0);                     /* SUB0 */
    spi_xfer_byte((uint8_t)(addr & 0xFF));          /* ADDR low */
    spi_xfer_byte((uint8_t)((addr >> 8) & 0xFF));   /* ADDR mid */
    spi_xfer_byte((uint8_t)((addr >> 16) & 0xFF));  /* ADDR high */
    spi_xfer_byte((uint8_t)(len & 0xFF));           /* LEN low */
    spi_xfer_byte((uint8_t)((len >> 8) & 0xFF));    /* LEN high */
}

void fpga_spi_xfer_write(uint8_t space, uint32_t addr, const uint8_t *data, uint16_t len)
{
    CS_LOW();
    xfer_send_header(space, addr, len, 0);  /* DIR=0 (write) */
    bflb_spi_poll_exchange(spi0, data, NULL, len);
    CS_HIGH();
}

void fpga_spi_xfer_read(uint8_t space, uint32_t addr, uint8_t *data, uint16_t len)
{
    CS_LOW();
    xfer_send_header(space, addr, len, 1);  /* DIR=1 (read) */
    spi_xfer_byte(0xFF);                    /* dummy byte per protocol */
    bflb_spi_poll_exchange(spi0, NULL, data, len);
    CS_HIGH();
}

void fpga_spi_xfer_fill(uint8_t space, uint32_t addr, uint8_t val, uint16_t len)
{
    CS_LOW();
    xfer_send_header(space, addr, len, 0);  /* DIR=0 (write) */

    /* Send repeating byte — can't use poll_exchange with NULL tx since
     * that sends 0xFF. Send byte-by-byte for fill. */
    for (uint16_t i = 0; i < len; i++) {
        spi_xfer_byte(val);
    }

    CS_HIGH();
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
