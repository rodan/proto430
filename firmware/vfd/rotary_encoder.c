
#ifdef CONFIG_ROT_ENCODER
#include <stdint.h>
#include "proj.h"
#include "sig.h"
#include "glue.h"
#include "vfd_extra.h"
#include "rotary_encoder.h"

encoder_t enc;
volatile uint8_t port8_last_event;
static uint8_t rot_enc_changed;
extern uint8_t brightness;

void rot_enc_init(void)
{
    rot_enc_changed = 0;
    enc.debouncing_A = 0;
    enc.counter = 0;

    P8DIR &= ~(BIT1 | BIT2);
    P8IFG = 0;
    P8IE |= BIT2;
    if (P8IN & BIT2) {
        // activate on a high-to-low transition
        P8IES |= BIT2;
    } else {
        // activate on a low-to-high transition
        P8IES &= ~BIT2;
    }
}

void rot_enc_increment(void)
{
    if (brightness < 8) {
        brightness++;
    }
}

void rot_enc_decrement(void)
{
    if (brightness > 2) {
        brightness--;
    }
}


// Port 8 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT8_VECTOR
__interrupt void port8_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT8_VECTOR))) port8_isr_handler(void)
#else
#error Compiler not supported!
#endif
{
    switch (P8IV) {
    case P8IV__P8IFG2:
        // listen for opposite edge
        P8IES ^= BIT2;
        //port8_last_event |= BIT2;
        //_BIC_SR_IRQ(LPM3_bits);
        
        if (enc.debouncing_A == 0) {
            enc.debouncing_A = 1;
            enc.change_A = ROT_ENC_CHA_IN;
            enc.debounced_B = ROT_ENC_CHB_IN; // consider B debounced

            // start the debounce timer
            TA3CCTL1 &= ~CCIE;
            TA3CCTL1 = 0;
            TA3CCR1 = TA3R + ROT_DEBOUNCE_PERIOD;
            TA3CCTL1 = CCIE;
        }

        break;
    }
    P8IFG = 0;
}

#endif
