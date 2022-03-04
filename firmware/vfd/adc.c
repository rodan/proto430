
#include <msp430.h>
#include <inttypes.h>
#include <stdlib.h>
#include "adc.h"

#include "proj.h"

uint16_t adc_last_event;
struct adc_t adc;

uint8_t adc_init(const uint8_t pins)
{
    uint8_t configured = 0;

    if (!pins) {
        return EXIT_FAILURE;
    }

    while (pins != configured) {
        // P3.1 is the 1 cell battery voltage
        if (pins & ADC_1_CELL) {
            P3SEL1 |= BIT1;
            P3SEL0 |= BIT1;
            configured |= ADC_1_CELL;
        }

        // P3.2 is the 2 cell battery voltage
        if (pins & ADC_2_CELL) {
            P3SEL1 |= BIT2;
            P3SEL0 |= BIT2;
            configured |= ADC_2_CELL;
        }

        // P3.3 is the module identifier voltage
        if (pins & ADC_ID) {
            P3SEL1 |= BIT3;
            P3SEL0 |= BIT3;
            configured |= ADC_ID;
        }
    }

    // set up ADC12
    ADC12CTL0 &= ~(ADC12ON + ADC12ENC + ADC12SC);
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;      // Sampling time, S&H=16, ADC12 on
    ADC12CTL1 = ADC12SHP;                   // Use sampling timer
    ADC12CTL2 |= ADC12RES_2;                // 12-bit conversion results
    ADC12IER0 |= ADC12IE0;                  // enable interrupt

    return EXIT_SUCCESS;
}

void adc_halt(void)
{
    ADC12CTL0 &= ~(ADC12ON + ADC12ENC + ADC12SC);
}

void vref_init(void)
{
    while (REFCTL0 & REFGENBUSY) {;}        // If ref generator busy, W
    REFCTL0 = REFVSEL_2 | REFON;            // 2.5V reference
    while(!(REFCTL0 & REFGENRDY)) {;}       // Wait for reference generator to settle
}

void vref_halt(void)
{
    REFCTL0 &= ~REFON;
}

uint16_t adc_get_event(void)
{
    return adc_last_event;
}

void adc_rst_event(void)
{
    adc_last_event = ADC_EV_NULL;
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_B_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_B_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(ADC12IV, ADC12IV__ADC12RDYIFG))
    {
        case ADC12IV__NONE:        break;   // Vector  0:  No interrupt
        case ADC12IV__ADC12OVIFG:  break;   // Vector  2:  ADC12MEMx Overflow
        case ADC12IV__ADC12TOVIFG: break;   // Vector  4:  Conversion time overflow
        case ADC12IV__ADC12HIIFG:  break;   // Vector  6:  ADC12BHI
        case ADC12IV__ADC12LOIFG:  break;   // Vector  8:  ADC12BLO
        case ADC12IV__ADC12INIFG:  break;   // Vector 10:  ADC12BIN
        case ADC12IV__ADC12IFG0:            // Vector 12:  ADC12MEM0 Interrupt
            adc_last_event = ADC_EV_CONV_RDY;
            adc.adc_mem = ADC12MEM0;
            _BIC_SR_IRQ(LPM3_bits);
            break;                          // Clear CPUOFF bit from 0(SR)
        case ADC12IV__ADC12IFG1:   break;   // Vector 14:  ADC12MEM1
        case ADC12IV__ADC12IFG2:   break;   // Vector 16:  ADC12MEM2
        case ADC12IV__ADC12IFG3:   break;   // Vector 18:  ADC12MEM3
        case ADC12IV__ADC12IFG4:   break;   // Vector 20:  ADC12MEM4
        case ADC12IV__ADC12IFG5:   break;   // Vector 22:  ADC12MEM5
        case ADC12IV__ADC12IFG6:   break;   // Vector 24:  ADC12MEM6
        case ADC12IV__ADC12IFG7:   break;   // Vector 26:  ADC12MEM7
        case ADC12IV__ADC12IFG8:   break;   // Vector 28:  ADC12MEM8
        case ADC12IV__ADC12IFG9:   break;   // Vector 30:  ADC12MEM9
        case ADC12IV__ADC12IFG10:  break;   // Vector 32:  ADC12MEM10
        case ADC12IV__ADC12IFG11:  break;   // Vector 34:  ADC12MEM11
        case ADC12IV__ADC12IFG12:  break;   // Vector 36:  ADC12MEM12
        case ADC12IV__ADC12IFG13:  break;   // Vector 38:  ADC12MEM13
        case ADC12IV__ADC12IFG14:  break;   // Vector 40:  ADC12MEM14
        case ADC12IV__ADC12IFG15:  break;   // Vector 42:  ADC12MEM15
        case ADC12IV__ADC12IFG16:  break;   // Vector 44:  ADC12MEM16
        case ADC12IV__ADC12IFG17:  break;   // Vector 46:  ADC12MEM17
        case ADC12IV__ADC12IFG18:  break;   // Vector 48:  ADC12MEM18
        case ADC12IV__ADC12IFG19:  break;   // Vector 50:  ADC12MEM19
        case ADC12IV__ADC12IFG20:  break;   // Vector 52:  ADC12MEM20
        case ADC12IV__ADC12IFG21:  break;   // Vector 54:  ADC12MEM21
        case ADC12IV__ADC12IFG22:  break;   // Vector 56:  ADC12MEM22
        case ADC12IV__ADC12IFG23:  break;   // Vector 58:  ADC12MEM23
        case ADC12IV__ADC12IFG24:  break;   // Vector 60:  ADC12MEM24
        case ADC12IV__ADC12IFG25:  break;   // Vector 62:  ADC12MEM25
        case ADC12IV__ADC12IFG26:  break;   // Vector 64:  ADC12MEM26
        case ADC12IV__ADC12IFG27:  break;   // Vector 66:  ADC12MEM27
        case ADC12IV__ADC12IFG28:  break;   // Vector 68:  ADC12MEM28
        case ADC12IV__ADC12IFG29:  break;   // Vector 70:  ADC12MEM29
        case ADC12IV__ADC12IFG30:  break;   // Vector 72:  ADC12MEM30
        case ADC12IV__ADC12IFG31:  break;   // Vector 74:  ADC12MEM31
        case ADC12IV__ADC12RDYIFG: break;   // Vector 76:  ADC12RDY
        default: break;
    }
}

struct adc_t * adc_get_p(void)
{
    return &adc;
}
