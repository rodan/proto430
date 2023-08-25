
#include <inttypes.h>
#include "proj.h"
#include "glue.h"
#include "jig.h"

spi_descriptor spid_jig_7000;
static uint8_t jig_output = JIG_OUTPUT_HR;      // type of output data formatting. see JIG_OUTPUT_* defines
static struct jig_t jig;
//static struct jig_t jig_7000;

struct jig_t *jig_get_p(void)
{
    return &jig;
}

void jig_7000_CS_high(void)
{
    P5OUT |= BIT3;
}

void jig_7000_CS_low(void)
{
    P5OUT &= ~BIT3;
}

uint8_t jig_7000_BSY(void)
{
    uint8_t rv;

    if (P3IN & BIT7) {
        rv = 1;
    } else {
        rv = 0;
    }

    return rv;
}

void jig_7000_read(const uint16_t addr, uint8_t *data, const uint8_t count)
{
    uint8_t tx_cmd[2];

    tx_cmd[0] = (addr & 0x7f00 ) >> 8;
    tx_cmd[1] = addr & 0xff;

    spi_resume();
    spid_jig_7000.cs_low();
    __delay_cycles(SPI_DELAY);
    spi_write_frame(spid_jig_7000.baseAddress, tx_cmd, 2);
    spi_read_frame(spid_jig_7000.baseAddress, data, count);
    __delay_cycles(SPI_DELAY);
    spid_jig_7000.cs_high();

    spi_pause();
}

void jig_set_output(const uint8_t type)
{
    switch (type) {
    case JIG_OUTPUT_HRD:
        jig_output = JIG_OUTPUT_HRD;
        break;
    case JIG_OUTPUT_HR:
        jig_output = JIG_OUTPUT_HR;
        break;
    case JIG_OUTPUT_MR:
        jig_output = JIG_OUTPUT_MR;
        break;
    default:
        jig_output = JIG_OUTPUT_NONE;
        break;
    }
}

#if 0
uint8_t jig_rx_handler(uart_descriptor *uartd, const uint8_t c)
{
    if (uartd->rx_enable && (uartd->p < UART_RXBUF_SZ - 1)) {
        uartd->rx_buf[uartd->p] = c;
        uartd->rx_buf[uartd->p+1] = 0;
        uartd->p++;
        timer_a0_delay_noblk_ccr1(intrchar_tmout);
    }

    return 0;
}
#endif

uint8_t jig_get_output(void)
{
    return jig_output;
}
