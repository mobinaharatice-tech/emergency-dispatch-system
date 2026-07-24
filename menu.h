#ifndef MENU_H
#define MENU_H

#include "common.h"


void menu_init(void);


void menu_main_loop(void);


int menu_get_current_turn(void);


void menu_handle_new_incident(void);

void menu_clear_input_buffer(void);

void show_simulation_report(void);

#endif // MENU_H