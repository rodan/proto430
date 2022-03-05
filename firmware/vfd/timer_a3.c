
//   timer a3 handling
//
//   author:          Petre Rodan <2b4eda@subdimension.ro>
//   available from:  https://github.com/rodan/
//   license:         BSD

#include <stdint.h>
#include "rotary_encoder.h"
#include "sig.h"
#include "timer_a3.h"

volatile uint16_t timer_a3_last_event;        // bitwise flag of current timer_a2 interrupt events

void timer_a3_init(void)
{
    __disable_interrupt();
    TA3CTL = TASSEL__ACLK + MC__CONTINOUS + TACLR;
    TA3EX0 = 0;
    __enable_interrupt();
}

__attribute__ ((interrupt(TIMER3_A1_VECTOR)))
void timer3_A1_ISR(void)
{
    uint16_t iv = TA3IV;

#ifdef LED_SYSTEM_STATES
    sig2_on;
#endif
    if (iv == TAIV__TACCR1) {
        sig5_on;
        // timer used by timer_a2_delay_noblk_ccr1()
        // disable interrupt
        TA3CCTL1 = 0;
        
        if (ROT_ENC_CHA_IN == enc.change_A) {
            // correct activation
            enc.debounced_A = enc.change_A;
#ifdef ROT_ENC_FULL_CYCLE
            // only activate on a rising edge of CHA, skip first activation
            if (enc.counter && enc.debounced_A) {
                if (enc.debounced_B) {
                    rot_enc_decrement();
                } else {
                    rot_enc_increment();
                }
            }
#else
            // activate on both rising and falling edges of CHA, skip first activation
            if (enc.counter) {
                if (enc.debounced_A) {
                    if (enc.debounced_B) {
                        rot_enc_decrement();
                    } else {
                        rot_enc_increment();
                    }
                } else {
                    if (enc.debounced_B) {
                        rot_enc_increment();
                    } else {
                        rot_enc_decrement();
                    }
                }
            }
#endif
            
        }
        enc.counter = 1;  // skip the first activation (which is usually caused during uC power-on)
        enc.debouncing_A = 0;

    } else if (iv == TA3IV_TA3IFG) {
        // overflow
        TA3CTL &= ~TAIFG;
    }
#ifdef LED_SYSTEM_STATES
    sig2_off;
#endif
}
