
//   timer a1 handling
//   CCR0 is used in decoding infrared remote control sequences
//   CCR1 is currently unused
//   CCR2 is currently unused
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         GNU GPLv3

#ifdef CONFIG_IR_RECEIVER

#include <stdint.h>
#include "glue.h"
#include "proj.h"
#include "sig.h"
#include "ir_remote_extra.h"
#include "timer_a1.h"

volatile uint16_t timer_a1_last_event;
uint16_t timer_a1_decode_timeout;

void timer_a1_init(void)
{
    timer_a1_decode_timeout = 0;
    __disable_interrupt();
    TA1CTL |= TASSEL__SMCLK + MC__UP;
    TA1CCR0 = IR_CCR;
    TA1R = 0;
    TA1CCTL0 |= CCIE;           // enable interrupt
    __enable_interrupt();
}

void timer_a1_halt(void)
{
    __disable_interrupt();
    TA1CTL = 0;
    TA1CTL &= ~TAIE;
    TA1CCTL0 &= ~CCIE;          // disable interrupt
    __enable_interrupt();
}

uint16_t timer_a1_get_event(void)
{
    return timer_a1_last_event;
}

void timer_a1_rst_event(void)
{
    timer_a1_last_event = TIMER_A1_EVENT_NONE;
}

__attribute__ ((interrupt(TIMER1_A0_VECTOR)))
void timer1_A0_ISR(void)
{
    sig5_switch;
    if (TA1CCR0 == IR_CCR) {
        timer_a1_decode_timeout++;
        timer_a1_last_event |= TIMER_A1_EVENT_CCR0;
        ir_isr();
        if (timer_a1_decode_timeout > IR_TIMEOUT_TICKS) {
            timer_a1_halt();
            ir_remote_init(IR_DEFAULT);
        }
        _BIC_SR_IRQ(LPM3_bits);
    }
}

#endif
