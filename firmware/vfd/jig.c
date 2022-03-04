
#include <inttypes.h>
#include "proj.h"
#include "glue.h"
#include "jig.h"

static uint8_t jig_output = JIG_OUTPUT_HR;      // type of output data formatting. see JIG_OUTPUT_* defines
static struct jig_t jig;

struct jig_t *jig_get_p(void)
{
    return &jig;
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
