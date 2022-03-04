
#ifdef CONFIG_EERAM_48LXXX
#include <stdio.h>
#include <stdlib.h>
#include "eusci_b_spi.h"
#include "spi.h"
#include "lib_convert.h"
#include "lib_time.h"
#include "glue.h"
#include "eeram_48l_extra.h"

spi_descriptor spid_eeram;

void EERAM_48L_CS_high(void)
{
    P2OUT |= BIT6;
}

void EERAM_48L_CS_low(void)
{
    P2OUT &= ~BIT6;
}

void EERAM_48L_HOLD_assert(void)
{
    P2OUT &= ~BIT5;
}

void EERAM_48L_HOLD_deassert(void)
{
    P2OUT |= BIT5;
}

void EERAM_48L_init(void)
{
    EERAM_48L_CS_high();
    EERAM_48L_HOLD_deassert();

    spid_eeram.baseAddress = SPI_BASE_ADDR;
    spid_eeram.cs_low = EERAM_48L_CS_low;
    spid_eeram.cs_high = EERAM_48L_CS_high;
    spid_eeram.cf1_assert = EERAM_48L_HOLD_assert;
    spid_eeram.cf1_deassert = EERAM_48L_HOLD_deassert;

    // disable automatic backup to eeprom
    EERAM_48L_write_streg(&spid_eeram, EERAM_48L512_SR_ASE);

}


#endif
