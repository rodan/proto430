#ifndef __SIG_H__
#define __SIG_H__

// visible leds
#define sig0_on              P1OUT |= BIT0
#define sig0_off             P1OUT &= ~BIT0
#define sig0_switch          P1OUT ^= BIT0

#define sig1_on              P1OUT |= BIT1
#define sig1_off             P1OUT &= ~BIT1
#define sig1_switch          P1OUT ^= BIT1

#define sig2_on              P3OUT |= BIT3
#define sig2_off             P3OUT &= ~BIT3
#define sig2_switch          P3OUT ^= BIT3

#define sig3_on              P1OUT |= BIT4
#define sig3_off             P1OUT &= ~BIT4
#define sig3_switch          P1OUT ^= BIT4

#define sig4_on              P1OUT |= BIT5
#define sig4_off             P1OUT &= ~BIT5
#define sig4_switch          P1OUT ^= BIT5


// debug signals
#define sig5_on              P4OUT |= BIT1
#define sig5_off             P4OUT &= ~BIT1
#define sig5_switch          P4OUT ^= BIT1

#define sig6_on              P4OUT |= BIT2
#define sig6_off             P4OUT &= ~BIT2
#define sig6_switch          P4OUT ^= BIT2

#define sig7_on              P4OUT |= BIT3
#define sig7_off             P4OUT &= ~BIT3
#define sig7_switch          P4OUT ^= BIT3

#define sig8_on              P2OUT |= BIT5
#define sig8_off             P2OUT &= ~BIT5
#define sig8_switch          P2OUT ^= BIT5

#define sig9_on              P2OUT |= BIT6
#define sig9_off             P2OUT &= ~BIT6
#define sig9_switch          P2OUT ^= BIT6

#endif
