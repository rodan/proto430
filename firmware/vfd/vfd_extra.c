
#ifdef CONFIG_VFD_NORITAKE
#include <stdio.h>
#include <stdlib.h>
#include "eusci_b_spi.h"
#include "spi.h"
#include "lib_convert.h"
#include "lib_time.h"
#include "eeram_48l_extra.h"

spi_descriptor spid_vfd;

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
    VFD_CS_high();
    VFD_RST_deassert();

    spid_vfd.baseAddress = SPI_BASE_ADDR;
    spid_vfd.cs_low = VFD_CS_low;
    spid_vfd.cs_high = VFD_CS_high;
    spid_vfd.cf1_assert = VFD_RST_assert;
    spid_vfd.cf1_deassert = VFD_RST_deassert;
    spid_vfd.dev_is_busy = VFD_BSY;
}


#endif
