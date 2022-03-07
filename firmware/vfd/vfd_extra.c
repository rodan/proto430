
#ifdef CONFIG_VFD_NORITAKE
#include <stdio.h>
#include <stdlib.h>
#include "glue.h"
//#include "eusci_b_spi.h"
//#include "spi.h"
//#include "lib_convert.h"
//#include "lib_time.h"
#include "eeram_48l_extra.h"

spi_descriptor spid_vfd;
vfd_descriptor vfdd;

void VFD_CS_high(void)
{
    P8OUT |= BIT3;
}

void VFD_CS_low(void)
{
    P8OUT &= ~BIT3;
}

uint8_t VFD_BSY(void)
{
    uint8_t rv;

    if (P4IN & BIT4) {
        rv = 1;
    } else {
        rv = 0;
    }

    return rv;
}

void VFD_RST_assert(void)
{
    P7OUT &= ~BIT3;
}

void VFD_RST_deassert(void)
{
    P7OUT |= BIT3;
}

void VFD_init(void)
{
    // BSY pin
    P4DIR &= ~BIT4;
    P4IFG &= ~BIT4;

    VFD_CS_high();
    VFD_RST_deassert();

    spid_vfd.baseAddress = SPI_BASE_ADDR;
    spid_vfd.cs_low = VFD_CS_low;
    spid_vfd.cs_high = VFD_CS_high;
    spid_vfd.cf1_assert = VFD_RST_assert;
    spid_vfd.cf1_deassert = VFD_RST_deassert;
    spid_vfd.dev_is_busy = VFD_BSY;
    vfdd.spid = &spid_vfd;

    ringbuf_init(&vfdd.rbtx, vfdd.tx_buf, VFD_TXBUF_SZ);
    vfdd.tx_busy = 0;

    // activate on high-to-low transition
    P4IES |= BIT4;
    // enable interrupt
    P4IE |= BIT4;
}



// Port 6 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT4_VECTOR
__interrupt void port4_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT4_VECTOR))) port4_isr_handler(void)
#else
#error Compiler not supported!
#endif
{
    uint8_t t;

    switch (P4IV) {
    case P4IV__P4IFG4:
        if (ringbuf_get(&vfdd.rbtx, &t)) {
            vfdd.tx_busy = 1;
            vfdd.spid->cs_low();
            spi_write_frame(vfdd.spid->baseAddress, &t, 1);
            vfdd.spid->cs_high();
        } else {
            // nothing more to do
            vfdd.tx_busy = 0;
        }
        _BIC_SR_IRQ(LPM3_bits);
        break;
    }
    P4IFG = 0;
}

#endif
