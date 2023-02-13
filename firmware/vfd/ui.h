#ifndef __UI_H__
#define __UI_H__

#include "proj.h"

void display_version(void);
void display_menu(void);
void display_test(void);
void parse_user_input(void);

void ui_vfd_refresh(void);

extern uint8_t jig_data[23];

#endif
