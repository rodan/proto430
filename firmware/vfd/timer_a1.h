#ifndef __TIMER_A1_H__
#define __TIMER_A1_H__

#include "proj.h"

void timer_a1_init(void);
void timer_a1_halt(void);
uint16_t timer_a1_get_event(void);
void timer_a1_rst_event(void);

#define      TIMER_A1_EVENT_NONE  0x0
#define      TIMER_A1_EVENT_CCR0  0x1
#define      TIMER_A1_EVENT_CCR1  0x2

#endif
