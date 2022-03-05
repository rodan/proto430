#ifndef __TIMER_A3_H__
#define __TIMER_A3_H__

#include "proj.h"

void timer_a3_init(void);

uint16_t timer_a3_get_event(void);
void timer_a3_rst_event(void);

#endif
