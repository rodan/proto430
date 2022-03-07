
#ifdef CONFIG_IR_RECEIVER
#include <stdint.h>
#include "glue.h"
#include "proj.h"
#include "sig.h"
#include "timer_a1.h"
#include "ir_remote_extra.h"

uint8_t ir_number;
extern uart_descriptor bc;

void ir_remote_init(const uint8_t flag)
{
    IR_DIR &= ~IR_PIN;
    // activate on a high-to-low transition
    IR_IES |= IR_PIN;
    // reset IRQ flags
    IR_IFG &= ~IR_PIN;
    // enable interrupt
    IR_IE |= IR_PIN;

    if (flag & IR_RST_SM) {
        ir_resume();
    }
}

uint8_t ir_remote_in(void)
{
    return (IR_IN & IR_PIN);
}

void ir_remote_mng(void)
{

    char buf[CONV_BASE_10_BUF_SZ];

    if (ir_decode(&results)) {

        ir_number = -1;

        switch (results.value) {
            // RC5 codes
        case 1:                // 1
            ir_number = 1;
            break;
        case 2:                // 2
            ir_number = 2;
            break;
        case 3:                // 3
            ir_number = 3;
            break;
        case 4:                // 4
            ir_number = 4;
            break;
        case 5:                // 5
            ir_number = 5;
            break;
        case 6:                // 6
            ir_number = 6;
            break;
        case 7:                // 7
            ir_number = 7;
            break;
        case 8:                // 8
            ir_number = 8;
            break;
        case 9:                // 9
            ir_number = 9;
            break;
        case 0:                // 0
            ir_number = 0;
            break;
        default:
            break;
        }

        uart_print(&bc, _utoh(&buf[0], results.value));
        uart_print(&bc, "\r\n");

        ir_resume();            // Receive the next value
    }
}


// Port 6 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT6_VECTOR
__interrupt void port6_isr_handler(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT6_VECTOR))) port6_isr_handler(void)
#else
#error Compiler not supported!
#endif
{
    switch (P6IV) {
    case P6IV__P6IFG3:
        // disable pin interrupt
        IR_IE &= ~IR_PIN;
        timer_a1_init();
        ir_isr();
        //_BIC_SR_IRQ(LPM3_bits);
        break;
    }
    P6IFG = 0;
}



#endif
