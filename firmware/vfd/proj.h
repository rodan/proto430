#ifndef __PROJ_H__
#define __PROJ_H__

#include <msp430.h>
#include <stdlib.h>
#include <inttypes.h>

#define rail_3v3_on             P1OUT |= BIT3
#define rail_3v3_off            P1OUT &= ~BIT3

#define rail_5v_on              P1OUT |= BIT2
#define rail_5v_off             P1OUT &= ~BIT2

#define adc_2cell_check_on      P3OUT |= BIT0
#define adc_2cell_check_off     P3OUT &= ~BIT0

#define true                1
#define false               0

/*!
	\brief List of possible message types for the message bus.
	\sa eh_register()
*/

#define                SYS_MSG_NULL  0
//#define        SYS_MSG_TIMERA0_CCR1  0x1      // timer_a0_delay_noblk_ccr1
//#define        SYS_MSG_TIMERA0_CCR2  0x2      // timer_a0_delay_noblk_ccr2
#define        SYS_MSG_TIMERA1_CCR0  0x4      // IR decoder
#define          SYS_MSG_VFD_TX_RDY  0x8      // vfd display ready to receive
#define        SYS_MSG_JIG_7000_RDY  0x10     // 7000 jig is ready for new reading
#define         SYS_MSG_UART_BCL_RX  0x20     // UART received something
#define             SYS_MSG_P31_INT  0x80     // P3.1 interrupt
#define        SYS_MSG_SCH_POWEROFF  0x100    // timer_a2 schedule slot
#define         SYS_MSG_VFD_REFRESH  0x200    // timer_a2 schedule slot
#define         SYS_MSG_RTC_REFRESH  0x400    // timer_a2 schedule slot
#define        SYS_MSG_ADC_CONV_RDY  0x800    // adc conversion is ready
#define      SYS_MSG_SCH_CONV_2CELL  0x1000   // start adc conversion
#define      SYS_MSG_SCH_CONV_1CELL  0x2000   // start adc conversion
#define             SYS_MSG_P55_INT  0x4000   // P5.5 interrupt
#define             SYS_MSG_P56_INT  0x8000   // P5.6 interrupt
#define             SYS_MSG_P57_INT  0x10000  // P5.7 interrupt
#define       SYS_MSG_P55_TMOUT_INT  0x20000  // P5.5 long press
#define       SYS_MSG_P56_TMOUT_INT  0x40000  // P5.6 long press
#define       SYS_MSG_P57_TMOUT_INT  0x80000  // P5.7 long press
#define        SYS_MSG_TIMERA2_CCR1  0x100000  // timer_a2_delay_noblk_ccr1

#define       SCHEDULE_POWER_SAVING  TIMER_A2_SLOT_0
#define          SCHEDULE_PB_31_OFF  TIMER_A2_SLOT_1
#define             SCHEDULE_LED_ON  TIMER_A2_SLOT_2
#define            SCHEDULE_LED_OFF  TIMER_A2_SLOT_3
#define          SCHEDULE_ADC_2CELL  TIMER_A2_SLOT_4
#define          SCHEDULE_ADC_1CELL  TIMER_A2_SLOT_5
#define              SCHEDULE_PB_55  TIMER_A2_SLOT_6
#define              SCHEDULE_PB_56  TIMER_A2_SLOT_7
#define              SCHEDULE_PB_57  TIMER_A2_SLOT_8
#define        SCHEDULE_VFD_REFRESH  TIMER_A2_SLOT_9
#define        SCHEDULE_RTC_REFRESH  TIMER_A2_SLOT_10

//#define          POWER_SAVING_DELAY 180000UL // 100=1s, 6000=1m
//#define          POWER_SAVING_DELAY 1440000UL // 4h 100=1s, 6000=1m
#define          POWER_SAVING_DELAY 8640000UL // 24h 100=1s, 6000=1m

void check_events(void);
void halt(void);

#endif
